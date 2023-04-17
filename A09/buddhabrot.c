/*----------------------------------------------
 * Author: Neville Linden
 * Date: 4/12/23
 * Description: A program that generates a ppm file containing a graphical representation of the
 *              probability distribution over the trajectories of points that escape the Mandelbrot fractal using 4 threads.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <pthread.h>
#include "read_ppm.h"
#include "write_ppm.h"
#include <string.h>
#include <sys/time.h>
#include <math.h>

//define global variable for Step 3, mutex, and barrier
const float factor = 1.0/0.681;
pthread_mutex_t mutex;
pthread_barrier_t barrier;

struct threadInfo {
  //size of the image
  int size;
  //min and max values for x and y
  float xmin;
  float xmax;
  float ymin;
  float ymax;
  //max iterations of the mandelbrot process?
  int maxIterations;
  //number thread {1,2,3,4}
  int threadID;
  //final image array
  struct ppm_pixel* image;
  //"start index" and "end index" for rows and cols
  int startrow;
  int endrow;
  int startcol;
  int endcol;
  //array of 'booleans' for membership at (row,col)
  int* continuebool;
  //array of counts corresponding 1-1 to the image
  int* countsArr;
  //max count
  int maxCount;
};

//void* function which computes the image
void* helperfunc(void* threadstruct) {
  struct threadInfo* mythreadstruct = (struct threadInfo*) threadstruct;
  //print
  printf("Thread %d sub-image block: cols (%d, %d) ", mythreadstruct->threadID, mythreadstruct->startcol, mythreadstruct->endcol);
  printf("to rows (%d, %d)\n", mythreadstruct->startrow, mythreadstruct->endrow);
  //Step 1: determine Mandelbrot set membership
  for(int i = mythreadstruct->startrow; i < mythreadstruct->endrow; i++) { //rows
    for(int j = mythreadstruct->startcol; j < mythreadstruct->endcol; j++) { //columns
      //instance vars
      float x = 0.0;
      float y = 0.0;
      float xtmp;
      int iter = 0;
      float xfrac = (i * 1.0)/mythreadstruct->size;
      float yfrac = (j * 1.0)/mythreadstruct->size;
      float x0 = mythreadstruct->xmin + xfrac * (mythreadstruct->xmax - mythreadstruct->xmin);
      float y0 = mythreadstruct->ymin + yfrac * (mythreadstruct->ymax - mythreadstruct->ymin);
      int index = j * mythreadstruct->size + i;

      //implementation of the algorithm given in the assignment
      while(iter < mythreadstruct->maxIterations && x*x + y*y < 4) {
        xtmp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtmp;
        iter++;
      }
      //if DIDN'T escape -- membership is true
      if(iter >= mythreadstruct->maxIterations) {
        mythreadstruct->continuebool[index] = 1;
      }
    }
  }
  for(int i = mythreadstruct->startrow; i < mythreadstruct->endrow; i++) { //rows
    for(int j = mythreadstruct->startcol; j < mythreadstruct->endcol; j++) { //columns
      //Step 2: compute visited counts
      int index = j * mythreadstruct->size + i;
      if(mythreadstruct->continuebool[index]) {
        continue;
      }
      float x = 0.0;
      float y = 0.0;
      float xtmp;
      float xfrac = (i * 1.0)/mythreadstruct->size;
      float yfrac = (j * 1.0)/mythreadstruct->size;
      float x0 = mythreadstruct->xmin + xfrac * (mythreadstruct->xmax - mythreadstruct->xmin);
      float y0 = mythreadstruct->ymin + yfrac * (mythreadstruct->ymax - mythreadstruct->ymin);
      while((x*x + y*y) < 4) {
        xtmp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtmp;
        int yrow = round(mythreadstruct->size * (y - mythreadstruct->ymin)/(mythreadstruct->ymax - mythreadstruct->ymin));
        int xcol = round(mythreadstruct->size * (x - mythreadstruct->xmin)/(mythreadstruct->xmax - mythreadstruct->xmin));
        if (yrow < 0 || yrow >= mythreadstruct->size) {
          continue; // out of range
        }
        if (xcol < 0 || xcol >= mythreadstruct->size) {
          continue; // out of range
        }
        int countindex = yrow * mythreadstruct->size + xcol;      
        pthread_mutex_lock(&mutex);
        mythreadstruct->countsArr[countindex]++;
        if(mythreadstruct->countsArr[countindex] > mythreadstruct->maxCount) {
          mythreadstruct->maxCount = mythreadstruct->countsArr[countindex];
        }
        pthread_mutex_unlock(&mutex);
      }  
    }
  }

  pthread_barrier_wait(&barrier); //forced synchronization point
  printf("All threads have reached the barrier!\n");
  //Step 3: Compute colors
  for(int i = mythreadstruct->startrow; i < mythreadstruct->endrow; i++) { //rows
    for(int j = mythreadstruct->startcol; j < mythreadstruct->endcol; j++) { //columns
      int index = j * mythreadstruct->size + i;
      float value = 0;
      if(mythreadstruct->countsArr[index] > 0) {
        value = log(mythreadstruct->countsArr[index]) / log(mythreadstruct->maxCount);
        value = pow(value, factor);
      }
      mythreadstruct->image[index].red = value * 255;
      mythreadstruct->image[index].green = value * 255;
      mythreadstruct->image[index].red = value * 255;
    }
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  //initialize default info
  int size = 480;
  float xmin = -2.5;
  float xmax = 1.0;
  float ymin = -1.12;
  float ymax = 1.12;
  int maxIterations = 1000;
  int numProcesses = 4;
  int mode = 0;

  int opt;
  while ((opt = getopt(argc, argv, ":s:l:r:t:b:p:")) != -1) {
    switch (opt) {
      case 's': size = atoi(optarg); break;
      case 'l': xmin = atof(optarg); break;
      case 'r': xmax = atof(optarg); break;
      case 't': ymax = atof(optarg); break;
      case 'b': ymin = atof(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
        "-b <ymin> -t <ymax> -p <numProcesses> -m <mode>\n", argv[0]); break;
    }
  }
  printf("Generating Mandelbrot with size %dx%d\n", size, size);
  printf("  Num processes = %d\n", numProcesses);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  // todo: your code here
  //instance vars
  pthread_t* threads;
  struct threadInfo* info;
  double timer;
  struct timeval tstart, tend;
  int timestamp = time(0);
  srand(timestamp);
  //start the timer
  gettimeofday(&tstart, NULL);

  //allocate memory
  struct ppm_pixel* image = malloc(sizeof(struct ppm_pixel) * size * size);
  int* countsArr = malloc(sizeof(int) * size * size);
  int* continuebool = malloc(sizeof(int) * size * size);
  //initialize to zero
  for(int x = 0; x < size * size; x++) {
    continuebool[x] = 0;
  }
  //alloc. memory for thread array and struct array
  threads = malloc(sizeof(pthread_t) * numProcesses);
  info = malloc(sizeof(struct threadInfo) * numProcesses);
  //initialize mutex
  pthread_mutex_init(&mutex, NULL);
  //initialize barrier
  pthread_barrier_init(&barrier, NULL, numProcesses);
  //create threads
  for(int i = 0; i < numProcesses; i++) { 
    info[i].size = size;
    info[i].xmin = xmin;
    info[i].xmax = xmax;
    info[i].ymin = ymin; 
    info[i].ymax = ymax;
    info[i].threadID = i + 1;
    info[i].maxIterations = maxIterations; 
    info[i].image = image; 
    info[i].countsArr = countsArr;
    info[i].maxCount = 0;
    //assume true
    info[i].continuebool = continuebool;

    if(i == 0) {
      info[i].startcol = 0;
      info[i].endcol = size/2;
      info[i].startrow = 0;
      info[i].endrow = size/2;
    }
    else if(i == 1) {
      info[i].startcol = size/2;
      info[i].endcol = size;
      info[i].startrow = 0;
      info[i].endrow = size/2;
    }
    else if(i == 2) {
      info[i].startcol = 0;
      info[i].endcol = size/2;
      info[i].startrow = size/2;
      info[i].endrow = size;
    }
    else {
      info[i].startcol = size/2;
      info[i].endcol = size;
      info[i].startrow = size/2;
      info[i].endrow = size;
    } 
    pthread_create(&threads[i], NULL, helperfunc, &info[i]); 
  }
  //join threads
  for(int j = 0; j < numProcesses; j++) {
    pthread_join(threads[j], NULL); 
    printf("Thread %d finished\n", info[j].threadID);
  }
  //destroy (free) the mutex
  pthread_mutex_destroy(&mutex);
  //and the barrier
  pthread_barrier_destroy(&barrier);
  //stop the timer
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;

  //print results
  printf("Computed Buddhabrot set (%dx%d) in %f seconds\n", size, size, timer);

  //write file
  char finalname[128] = "";
  char front[128] = "buddhabrot-";
  char hyphen[2] = "-";
  char back[5] = ".ppm";
  char timestampString[11] = "";
  char sizeString[16] = "";

  sprintf(timestampString, "%d", timestamp);
  sprintf(sizeString, "%d", size);
  strcat(finalname, sizeString);
  strcat(finalname, hyphen);
  strcat(finalname, timestampString);
  strcat(finalname, back);
  strcat(front, finalname);

  //write file
  printf("Writing file: %s\n", front);
  write_ppm(front, image, size, size);

  //free memory
  free(image);
  free(info);
  free(threads);
  return 0;
}

/*----------------------------------------------
 * Author: Neville Linden
 * Date: 4/5/23
 * Description: A program that generates a ppm file containing a graphical representation
 *              of a randomly colorized section of the Mandelbrot fractal using 4 threads.
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

//no global variables this time, so everything goes in the struct
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
  //color palette
  struct ppm_pixel* palette;
  //final image array
  struct ppm_pixel* image;
  //"start index" and "end index" for rows and cols
  int startrow;
  int endrow;
  int startcol;
  int endcol;
};

//void* function which computes the image
void* helperfunc(void* threadstruct) {
  struct threadInfo* mythreadstruct = (struct threadInfo*) threadstruct;
  //print
  printf("Thread %d sub-image block: cols (%d, %d) ", mythreadstruct->threadID, mythreadstruct->startcol, mythreadstruct->endcol);
  printf("to rows (%d, %d)\n", mythreadstruct->startrow, mythreadstruct->endrow);
  //code taken from multi_mandelbrot: exact same algorithm, except pointing to the struct for relevant info
  for(int i = mythreadstruct->startrow; (i < mythreadstruct->endrow) && (i < mythreadstruct->size); i++) { //rows
    for(int j = mythreadstruct->startcol; (j < mythreadstruct->endcol) && (j < mythreadstruct->size); j++) { //columns
      //instance vars
      float x = 0.0;
      float y = 0.0;
      int iter = 0;
      float xfrac = (i * 1.0)/mythreadstruct->size;
      float yfrac = (j * 1.0)/mythreadstruct->size;
      float x0 = mythreadstruct->xmin + xfrac * (mythreadstruct->xmax - mythreadstruct->xmin);
      float y0 = mythreadstruct->ymin + yfrac * (mythreadstruct->ymax - mythreadstruct->ymin);
      int index = j * mythreadstruct->size + i;

      //implementation of the algorithm given in the assignment
      while(iter < mythreadstruct->maxIterations && x*x + y*y < 4) {
        float xtmp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtmp;
        iter++;
      }
      //escaped
      if(iter < mythreadstruct->maxIterations) {
        mythreadstruct->image[index] = mythreadstruct->palette[iter];
      }
      //in the set -- color black
      else {
        mythreadstruct->image[index].red = 0;
        mythreadstruct->image[index].green = 0;
        mythreadstruct->image[index].blue = 0;
      }    
    }
  }
  return NULL;
}

int main(int argc, char* argv[]) {
  //initialize default info
  int size = 480;
  float xmin = -2.0;
  float xmax = 0.47;
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
      //added functionality: can set a "mode": if you want truly random colors vs. jittered, -m with 0 or not 0
      case 'm': mode = atoi(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
        "-b <ymin> -t <ymax> -p <numProcesses> -m <mode>\n", argv[0]); break;
    }
  }
  printf("Generating Mandelbrot with size %dx%d\n", size, size);
  printf("  Num processes = %d\n", numProcesses);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  // todo: your code here
  // generate palette
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
  struct ppm_pixel* palette = malloc(sizeof(struct ppm_pixel) * maxIterations);
  struct ppm_pixel* image = malloc(sizeof(struct ppm_pixel) * size * size);
  //randomize colors based on mode selected
  if(mode) {
    for(int i = 0; i<maxIterations; i++) {
      palette[i].red = rand() % 255;
      palette[i].green = rand() % 255;
      palette[i].blue = rand() % 225;
    }
  }
  else {
    int basered = rand() % 255;
    int basegreen = rand() % 255;
    int baseblue = rand() % 255;
    for(int i = 0; i<maxIterations; i++) {
      palette[i].red = basered + rand() % 100 - 50;
      palette[i].green = basegreen + rand() % 100 - 50;
      palette[i].blue = baseblue + rand() % 100 - 50;
    }
  }
  //alloc. memory for thread array and struct array
  threads = malloc(sizeof(pthread_t) * numProcesses);
  info = malloc(sizeof(struct threadInfo) * numProcesses);
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
    info[i].palette = palette;

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
  //stop the timer
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;

  //print results
  printf("Computed Mandelbrot set (%dx%d) in %f seconds\n", size, size, timer);

  //write file
  char finalname[128] = "";
  char front[128] = "mandelbrot-";
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
  free(palette);
  free(image);
  free(info);
  free(threads);
  return 0;
}

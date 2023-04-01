/*----------------------------------------------
 * Author: Neville Linden
 * Date: 3/29/23
 * Description: A program that generates a ppm file containing a graphical representation
 *              of a randomly colorized section of the Mandelbrot fractal using 4 processes.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char* argv[]) {
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
      case 'm': size = atoi(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> "
        "-b <ymin> -t <ymax> -p <numProcesses>\n", argv[0]); break;
    }
  }
  printf("Generating mandelbrot with size %dx%d\n", size, size);
  printf("  Num processes = %d\n", numProcesses);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  // todo: your code here
  // generate palette
  //instance vars
  double timer;
  struct timeval tstart, tend;
  int timestamp = time(0);
  srand(timestamp);
  //start the timer
  gettimeofday(&tstart, NULL);
  //allocate memory
  struct ppm_pixel* palette = malloc(sizeof(struct ppm_pixel) * maxIterations);
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

  //initialize shared memory for processes (code from in-class example)
  int shmid;
  shmid = shmget(IPC_PRIVATE, sizeof(struct ppm_pixel) * size * size, 0644 | IPC_CREAT);
  if (shmid == -1) {
    perror("Error: Cannot initialize shared memory.\n");
    exit(1);
  }
  //attach shared memory (adapted from in-class example)
  struct ppm_pixel* image = shmat(shmid, NULL, 0);
  if (image == (void*) -1) {
    perror("Error: Cannot access shared memory.\n");
    exit(1);
  }

  // compute image
  // Example: size = 480. Then:
  // first process: {(0,240), (0,240)}
  // second process: {(240,480), (0,240)}
  // third process: {(0, 240), (240,480)}
  // fourth process: {(240,480), (240,480)}
  // and so we can just hardcode this behavior:
  for(int h = 0; h < 4; h++) {
    //create new child processes
    int pid = fork();
    //if-else structure adapted from in-class example
    if(pid == 0) {   
      int startcol = 0;
      int endcol = 0; 
      int startrow = 0;
      int endrow = 0;

      if(h == 0) {
        startcol = 0;
        endcol = size/2;
        startrow = 0;
        endrow = size/2;
      }
      else if(h == 1) {
        startcol = size/2;
        endcol = size;
        startrow = 0;
        endrow = size/2;
      }
      else if(h == 2) {
        startcol = 0;
        endcol = size/2;
        startrow = size/2;
        endrow = size;

      }
      else {
        startcol = size/2;
        endcol = size;
        startrow = size/2;
        endrow = size;
      }

      for(int i = startrow; i < endrow; i++) { //rows
        for(int j = startcol; j < endcol; j++) { //columns
          //instance vars
          float x = 0.0;
          float y = 0.0;
          int iter = 0;
          float xfrac = (i * 1.0)/size;
          float yfrac = (j * 1.0)/size;
          float x0 = xmin + xfrac * (xmax - xmin);
          float y0 = ymin + yfrac * (ymax - ymin);
          int index = j * size + i;

          //implementation of the algorithm given in the assignment
          while(iter < maxIterations && x*x + y*y < 4) {
            float xtmp = x*x - y*y + x0;
            y = 2*x*y + y0;
            x = xtmp;
            iter++;
          }
          //escaped
          if(iter < maxIterations) {
            image[index] = palette[iter];
          }
          //in the set -- color black
          else {
            image[index].red = 0;
            image[index].green = 0;
            image[index].blue = 0;
          }    
        }
      }
      exit(0);
    }
    else {
      printf("Launched child process: %d\n", pid);
    }
  }
  
  //wait for each process to complete (taken from in-class example)
  for (int i = 0; i < 4; i++) {
    int status;
    int pid = wait(&status);
    printf("Child process complete: %d\n", pid);
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

  printf("Writing file: %s\n", front);
  write_ppm(front, image, size, size);

  free(palette);

  //error handling (from in-class example)
  if (shmdt(image) == -1) {
    perror("Error: Cannot detatch from shared memory.\n");
    exit(1);
  }

  if (shmctl(shmid, IPC_RMID, 0) == -1) {
    perror("Error: Cannot remove shared memory.\n");
    exit(1);
  }
}

/*----------------------------------------------
 * Author: Neville Linden
 * Date: 02/22/23
 * Description: A program that generates a ppm file containing a graphical representation
 *              of a randomly colorized section of the Mandelbrot fractal.
  ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char* argv[]) {
  int size = 480;
  float xmin = -2.0;
  float xmax = 0.47;
  float ymin = -1.12;
  float ymax = 1.12;
  int maxIterations = 1000;
  int mode = 0;

  int opt;
  while ((opt = getopt(argc, argv, ":s:l:r:t:b:")) != -1) {
    switch (opt) {
      case 's': size = atoi(optarg); break;
      case 'l': xmin = atof(optarg); break;
      case 'r': xmax = atof(optarg); break;
      case 't': ymax = atof(optarg); break;
      case 'b': ymin = atof(optarg); break;
      //added functionality: can set a "mode": if you want truly random colors vs. jittered, -m with 0 or not 0
      case 'm': size = atoi(optarg); break;
      case '?': printf("usage: %s -s <size> -l <xmin> -r <xmax> -b <ymin> -t <ymax> -m <mode (0 for jitter, 1 for random)>\n", argv[0]); break;
    }
  }
  printf("Generating Mandelbrot with size %dx%d\n", size, size);
  printf("  X range = [%.4f,%.4f]\n", xmin, xmax);
  printf("  Y range = [%.4f,%.4f]\n", ymin, ymax);

  // todo: your work here
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

  // compute image (using the algorithm provided in the assignment description)
  struct ppm_pixel* image = (malloc(sizeof(struct ppm_pixel) * size * size));
    for(int i = 0; i < size; i++) { //rows
        for(int j = 0; j < size; j++) { //columns
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
  //free allocated memory
  free(palette);
  free(image);
}
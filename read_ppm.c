/*----------------------------------------------
* Author: Neville Linden
 * Date: 02/22/23
 * Description: A program that reads ppm files into an array
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"

struct ppm_pixel* read_ppm(const char* filename, int* w, int* h) {
  //instance variable definitions
  FILE* ppmfile;
  char header[3];
  char comment[128];
  int numpixels, maxcolor;

  //open file, read header, comment, dimensions, and max color value
  printf("Testing file %s...\n", filename);
  ppmfile = fopen(filename, "rb");
  if(ppmfile == NULL) {
    printf("Error opening file: %s\n", filename);
    return NULL;
  }

  fscanf(ppmfile, "%s%*c", header);
  printf("PPM File Format: %s\n", header);
  fscanf(ppmfile, "%[^\n]%*c", comment);
  printf("Comment: %s\n", comment);
  fscanf(ppmfile, "%d %d", w, h);
  printf("Dimensions: %d x %d\n", *w, *h);
  numpixels = *w * *h;
  fscanf(ppmfile, "%d%*c", &maxcolor);
  printf("Maximum color value: %d\n", maxcolor);

  //allocate size for array of arrays
  struct ppm_pixel* matrix = malloc(sizeof(struct ppm_pixel) * numpixels);
  if(matrix == NULL) {
    printf("Error allocating memory: %s\n", filename);
    return NULL;
  }
  //quick implementation to see what's in the file
  //char row[1000];
  //while(fgets(row, 1000, ppmfile)) {
  //  printf("%s\n", row);
  //}
  fread(matrix, sizeof(struct ppm_pixel), numpixels, ppmfile);
  fclose(ppmfile);
  return matrix;
}

struct ppm_pixel** read_ppm_2d(const char* filename, int* w, int* h) {

  return NULL;
}

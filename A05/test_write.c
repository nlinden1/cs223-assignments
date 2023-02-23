/*----------------------------------------------
 * Author: Neville Linden
 * Date: 02/22/23
 * Description: A program that tests write_ppm.c
 ---------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
  int w, h;
  struct ppm_pixel* pixels = read_ppm("feep-raw.ppm", &w, &h);

  // test writing the file to test.ppm, reload it, and print the contents
  write_ppm("test.ppm", pixels, w, h);
  struct ppm_pixel* pixels2 = read_ppm("test.ppm", &w, &h);
  for(int i = 0; i < w; i++) {
    for(int j = 0; j < h; j++) {
      printf("(%d, %d, %d) ", pixels2[i*h+j].red, pixels2[i*h+j].green, pixels2[i*h+j].blue);
    }
    printf("\n");
  }
  free(pixels);
  free(pixels2);
  return 0;
}

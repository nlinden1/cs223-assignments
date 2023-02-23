/*----------------------------------------------
 * Author: Neville Linden
 * Date: 02/22/23
 * Description: A program that tests the read_ppm function
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "read_ppm.h"

int main() {
  int w, h;
  struct ppm_pixel* pixels = read_ppm("feep-raw.ppm", &w, &h);

  for(int i = 0; i < w; i++) {
    for(int j = 0; j < h; j++) {
      printf("(%d, %d, %d) ", pixels[i*h+j].red, pixels[i*h+j].green, pixels[i*h+j].blue);
    }
    printf("\n");
  }
  free(pixels);
  return 0;
}


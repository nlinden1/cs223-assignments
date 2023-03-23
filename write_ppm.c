/*----------------------------------------------
 * Author: Neville Linden
 * Date: 02/22/23
 * Description: A program that writes ppm files
  ---------------------------------------------*/

#include "write_ppm.h"
#include <stdio.h>
#include <string.h>

void write_ppm(const char* filename, struct ppm_pixel* pixels, int w, int h) {
    //initializing file to be written and number of pixels
    FILE* ppmfilew = fopen(filename, "wb");
    int numpixels = w * h;

    //writing the header
    fprintf(ppmfilew, "P6\n# Created by GIMP version 2.10.24 PNM plug-in\n");
    fprintf(ppmfilew, "%d %d\n255\n", w, h);
    fwrite(pixels, sizeof(struct ppm_pixel), numpixels, ppmfilew);
    fclose(ppmfilew);
}

void write_ppm_2d(const char* filename, struct ppm_pixel** pxs, int w, int h) {
}

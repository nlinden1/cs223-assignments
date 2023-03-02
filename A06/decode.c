/*----------------------------------------------
 * Author: Neville Linden
 * Date: 03/01/2023
 * Description: A program that finds hidden information in images
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "read_ppm.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage: decode <file.ppm>\n");
    return 0;
  }

  //define instance variables
  //width and height of the image
  int w, h;
  //mask for checking the least significant bit
  unsigned int mask = 0x1;
  //"counter" for checking if we're at the end of the message
  unsigned int checker = 0b11111111;
  //value used to set the rightmost bit to zero
  unsigned int zero = 0b11111110;
  
  //read in the image
  struct ppm_pixel* matrix = read_ppm(argv[1], &w, &h);
  if(matrix == NULL) {
    exit(1);
  }
  //total number of characters hidden in the image, including null
  int maxchars = (w * h * 3/8);
  //cast the matrix to an unsigned char* so we can use bitwise operators
  unsigned char* matrix2 = (unsigned char*) matrix;
  printf("Max number of characters in the image: %d\n", maxchars);
  //allocate memory for the char matrix containing the message
  unsigned char* matrix3 = malloc(sizeof(char) * maxchars);

  //i represents the index of the bit in the message
  for(int i = 0; i < w * h * 3; i++) {
    //extremely useful test code
    /*if(matrix2[i] & mask) {
      printf("1");
    }
    else {
      printf("0");
    }
    */

    //j represents the index of each char in the message
    int j = i/8;
    //printf("\n%02X", matrix3[j]);
    matrix3[j] = (matrix3[j] << 1);
    matrix3[j] += (matrix2[i] & mask);
    
    //break if null
    checker = checker << 1;
    if(matrix2[i] & mask) {  
      checker = checker | mask;
    }
    else {
      checker = checker & zero;
    }
    //if we're at the end of a byte and have seen 8 zeroes, we're done
    if(checker == 0 && i % 8 == 7) {
      break;
    }
  }
  printf("\n%s\n", matrix3);
  free(matrix);
  free(matrix3);
  return 0;
}

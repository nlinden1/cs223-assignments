/*----------------------------------------------
 * Author: Neville Linden
 * Date: 03/01/2023
 * Description: A program that inserts hidden information into images
 ---------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "read_ppm.h"
#include "write_ppm.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    printf("usage: encode <file.ppm>\n");
    return 0;
  }

  //define instance variables
  //width and height of the image
  int w, h;
  //values used to set the least significant bit to one and zero, respectively
  unsigned int one = 0x1;
  unsigned int zero = 0b11111110;
  struct ppm_pixel* matrix = read_ppm(argv[1], &w, &h);
  //read_ppm returns NULL if the file can't be read
  //it also prints some stuff, so we don't need to print anything here
  if(matrix == NULL) {
    exit(1);
  }

  //create a matrix 'filename' to store the encoded file's name
  char finalname[128] = "";
  char encoded[13] = "-encoded.ppm";
  strncpy(finalname, argv[1], (strlen(argv[1]) - 4));
  strcat(finalname, encoded);
  printf("%s\n", finalname);

  //max number of characters that can be inserted (need one for null terminator)
  int maxchars = (w * h * 3/8) - 1;
  //allocate memory for the char matrix containing the message, including null
  char* message = malloc(sizeof(char) * (maxchars + 1));
  printf("Reading %s with width %d and height %d...\n", argv[1], w, h);
  printf("Max number of characters able to be inserted: %d\n", maxchars);
  printf("Enter a phrase to be inserted: ");
  scanf("%s", message);
  if(strlen(message) > maxchars) {
    printf("Phrase too long, exiting!\n");
    exit(1);
  }

  //useful test code: printing hex values of each character
  /*
  for(int x = 0; x < strlen(message) + 1; x++) {
    printf("Hex value to be inserted: %02X\n", message[x]);
  }
  */

  //mask2 first checks the leftmost bit of the first char
  unsigned int mask2 = 0b10000000;
  //i is the index of the bit in the message
  int i = 0;

  //so we keep going until we've encoded every bit in the message
  while(i < ((strlen(message) + 1) * 8)) {
    //j is the index of the pixel in the matrix
    //k is the index of the character in the message
    int j = i/3;
    int k = i/8;

    //use mask2 to attain the leftmost bit of the character
    if(message[k] & mask2) {
      //put the leftmost bit into matrix2's red value using the if statement
      matrix[j].red = matrix[j].red | one;
    }
    else {
      matrix[j].red = matrix[j].red & zero;
    }
    //increment i, update k's value
    i++;
    k = i/8;

    //check if we're at the end yet
    if(i > (strlen(message) + 1) * 8) {
      break;
    }
    //bitshift mask2 to the right by 1 to check the subsequent bit
    mask2 = mask2 >> 1;
    //reset if needed
    if(mask2 == 0) {
      mask2 = 0b10000000;
    }

    //put the next bit into matrix's green value
    if(message[k] & mask2) {
      matrix[j].green = matrix[j].green | one;
    }
    else {
      matrix[j].green = matrix[j].green & zero;
    }
    i++;
    k = i/8;

    if(i > (strlen(message) + 1) * 8) {
      break;
    }
    mask2 = mask2 >> 1;
    if(mask2 == 0) {
      mask2 = 0b10000000;
    }

    //put the next bit into the blue value
    if(message[k] & mask2) {
      matrix[j].blue = matrix[j].blue | one;
    }
    else {
      matrix[j].blue = matrix[j].blue & zero;
    }
    i++;
    k = i/8;

    if(i > (strlen(message) + 1) * 8) {
      break;
    }
    mask2 = mask2 >> 1;
    if(mask2 == 0) {
      mask2 = 0b10000000;
    }
  }
  //write the final matrix to a new file
  write_ppm(finalname, matrix, w, h);
  //free memory
  free(message);
  return 0;
}
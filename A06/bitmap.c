/*----------------------------------------------
 * Author: Neville Linden
 * Date: 03/01/2023
 * Description: A program that turns hex numbers into bitmap images
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

//constant throughout the program
const int rows = 8;
const int cols = 8;

//prints the array
void printArr(char twodmat[rows][cols]) {
  for(int i = 0; i < rows; i++) { 
	  for(int j = 0; j < cols; j++) {
	    printf("%c ", twodmat[i][j]);
	  }
	printf("\n");
    }
}

int main() {
  //define instance variables
  unsigned long img;
  scanf(" %lx", &img);
  printf("Image (unsigned long): %lx\n", img);
  unsigned long mask = 0x1;
  char bitmap[rows][cols];
  //iterate through the array, use the mask to check the bit
  //if 1, add @ to the 2d array
  //this method works bottom to top, right to left
  for(int i = 0; i < rows; i++) {
    for(int j = 0; j < cols; j++) {
      if(img & mask) {
        bitmap[i][j] = '@';
      } 
      else {
        bitmap[i][j] = ' ';
      }
      mask = mask << 1;
    }
  }
  printArr(bitmap);
  return 0;
}
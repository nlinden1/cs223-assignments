/*----------------------------------------------
 * Author: Neville Linden
 * Date: 02/22/23
 * Description: A program that finds a common letter between two words 
 The program then displays these in "crossword" style
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_matrix(char** m, int rows, int cols) {
  for(int i = 0; i < rows; i++) {  // for each row i
    for(int j = 0; j < cols; j++) { // iterate over each column in row i
        m[i][j] = '.';
    }
  }
}

//lic = letter in common
//returns 1 if there is a letter in common, 0 otherwise
//also changes the values of pos1 and pos2 to the positions of the letter in common
int lic(char* word1, char* word2, int* pos1, int* pos2) {
  for(int i = 0; i < strlen(word1); i++) {
    for(int j = 0; j < strlen(word2); j++) {
      if (word1[i] == word2[j]) {
        *pos1 = i;
        *pos2 = j;
        return 1;
      }
    }
  }
  return 0;
}

//given a 2d array, a word, an int for position, and another int (0 for vertical, 1 for horizontal)
//insert the given word into the array
int insertword(char** arr, char* word, int pos, int roworcol) {
  if(roworcol) {
    for(int i = 0; i < strlen(word); i++) {
      arr[pos][i] = word[i];
    }
  }
  else {
    for (int j = 0; j < strlen(word); j++) {
      arr[j][pos] = word[j];
    }
  }
  return 1;
}

void printArr(char** twodmat, int rows, int cols) {
  for (int i = 0; i < rows; i++) { // rows
	  for (int j = 0; j < cols; j++) { // columns
	    printf("%c", twodmat[i][j]);
	  }
	printf("\n");
    }
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: %s <word1> <word2>\n", argv[0]);
    exit(0);
  }
  int pos1, pos2;
  //malloc some 2d array with the size of the two words
  int nrows = strlen(argv[1]);
  int ncols = strlen(argv[2]);
  char** matrix = malloc(sizeof(char*) * nrows);
  for (int i = 0; i < nrows; i++) {
    matrix[i] = malloc(sizeof(char) * ncols);
  }
  //initialize the array with ".", may need to include \0 at the end
  init_matrix(matrix, nrows, ncols);
  //loop through the first word, check if the second word has a letter in common
  //if it doesn't, print something and exit
  if(!lic(argv[1], argv[2], &pos1, &pos2)) {
    printf("No letters in common!\n");
    exit(0);
  }
  //otherwise: given two words with letters i and j in common, respectively
  //column j of the array should be the first word, and row i of the array should be the second word
  else {
    printf("Letter in common at position %d of %s and position %d of %s\n\n", pos1, argv[1], pos2, argv[2]);
    insertword(matrix, argv[1], pos2, 0);
    insertword(matrix, argv[2], pos1, 1);
  }
  //loop through the array and print it out
  printArr(matrix, nrows, ncols);
  //free memory
  for (int i = 0; i < nrows; i++) {
    free(matrix[i]);
  }
  free(matrix);
  matrix = NULL;
  return 0;
}
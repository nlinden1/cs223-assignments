#include <stdio.h>
#include <stdlib.h>

void init_matrix(int** m, int rows, int cols) {
  for (int i = 0; i < rows; i++) {  // for each row i
    for (int j = 0; j < cols; j++) { // iterate over each column in row i
        m[i][j] = 0;
    }
  }
}

void printArr(int** twodmat, int rows, int cols) {
  for(int i = 0; i < rows; i++) { // rows
	  for(int j = 0; j < cols; j++) { // columns
	    printf("%d", twodmat[i][j]);
	  }
	printf("\n");
    }
}

int main(int argc, char* argv[]) {
    if(argc < 3) {
        printf("Usage: %s <dimension1> <dimension2>\n", argv[0]);
        exit(0);
    }
    int w = atoi(argv[1]);
    int h = atoi(argv[2]);
    int** matrix = malloc(sizeof(int*) * (h));
    for(int i = 0; i < (h); i++) {
        matrix[i] = malloc(sizeof(int) * (w));
    }
    init_matrix(matrix, h, w);
    matrix[1][2] = 5;
    printArr(matrix, h, w);
    free(matrix);
    return 0;
}

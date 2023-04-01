/*----------------------------------------------
 * Author: Neville Linden
 * Date: 3/29/23
 * Description: A program that searches for a value in a binary file using two processes.
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  //instance vars
  FILE* data;
  int size, userIn;
  int* intList;
  int notfound = 1;

  //read the first line to initialize list size, allocate memory
  data = fopen("data.bin", "rb");
  fread(&size, sizeof(int), 1, data);
  intList = malloc(sizeof(int) * size);
  fread(intList, sizeof(int), size, data);

  //accept user input
  printf("Enter a value to search: ");
  scanf(" %d", &userIn);

  //create parent and child
  pid_t pid;
  pid = fork();
  //parent
  if(pid) {
    for(int i = 0; i < size/2; i++) {
      if(intList[i] == userIn) {
        printf("Parent found %d at index %d\n", userIn, i);
        notfound = 0;
        break;
      }
    }
    if(notfound) {
      printf("Not found!\n");
    }
  }
  //child
  else {
    for(int j = size - 1; j >= size/2; j--) {
      if(intList[j] == userIn) {
        printf("Child found %d at index %d\n", userIn, j);
        notfound = 0;
        break;
      }  
    }
    if(notfound) {
      printf("Not found!\n");
    }
  }
  //free memory
  fclose(data);
  free(intList);
  return 0;
}

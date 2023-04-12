/*----------------------------------------------
 * Author: Neville Linden
 * Date: 4/5/23
 * Description: A program that searches for a value in a binary file using N threads.
 * Note: Given M integers and N threads, the first N-1 threads will search Q ints.
 *       The Nth thread will search Q+R ints (given M = NQ + R)
 ---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//assignment description doesn't specify no glabal variables
//so I used some global variables to reduce struct size
int userIn, size;
int* intArr;

//struct holding relevant info
struct threadInfo {
  //number thread {1,2,...,n}
  int threadID;
  //size of the array segment that each thread covers
  int segsize;
  //starting & ending index of the given thread
  int startIndex;
  int endIndex;
  //index of the user input, when found
  int foundIndex;
};

//void* function
void* helperfunc(void* threadstruct) {
  struct threadInfo* mythreadstruct = (struct threadInfo*) threadstruct;
  //iterate from start to end for each thread, only stop when we reach the actual end of the array
  for(int h = mythreadstruct -> startIndex; (h < size) && (h < mythreadstruct -> endIndex); h++) {
    if(intArr[h] == userIn) {
      mythreadstruct->foundIndex = h;
      break;
    }
  }
  return NULL;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    printf("usage: %s <NumThreads>\n", argv[0]);
    exit(0);
  }

  //instance vars
  FILE* fp;
  int found = 0;
  int numthreads = atoi(argv[1]);
  pthread_t* threads;
  fp = fopen("data.bin", "rb");
  //catch exception: file not found
  if(fp == NULL){
    printf("Error: Unable to open file\n");
    exit(1);
  }

  //read file into integer array
  fread(&size, sizeof(int), 1, fp);
  intArr = malloc(sizeof(int) * size);
  fread(intArr, sizeof(int), size, fp);

  //accept user input
  printf("Enter a value to search: ");
  scanf(" %d", &userIn);

  //allocate space for thread identifiers and structs (from Dive into Systems, Chap 14)
  threads = malloc(sizeof(pthread_t) * numthreads);
  struct threadInfo* information = malloc(sizeof(struct threadInfo) * numthreads);

  //initialize thread info -- method taken from Dive into Systems, Chap 14.
  //won't work if numthreads = 1, so we have the code below
  for(int i = 0; i < numthreads - 1; i++) {
    information[i].threadID = i+1;
    information[i].segsize = size / numthreads;
    information[i].startIndex = information[i].segsize * i;
    information[i].endIndex = information[i].segsize * (i+1);
    information[i].foundIndex = -1;
    pthread_create(&threads[i], NULL, helperfunc, &information[i]);
  }
  //if there are multiple threads, account for remainder case
  if(numthreads > 1) {
    //last thread potentially covers more ground
    information[numthreads-1].threadID = numthreads;
    //segsize includes the remainder
    information[numthreads-1].segsize = size / numthreads + size % numthreads;
    //start index is +1 the previous thread's
    information[numthreads-1].startIndex = information[numthreads-2].endIndex + 1;
    //it's the last thread, so end index is obvious
    information[numthreads-1].endIndex = size - 1;
    //set foundIndex to impossible value -- only >= zero if actually found
    information[numthreads-1].foundIndex = -1;
    pthread_create(&threads[numthreads-1], NULL, helperfunc, (void*) &information[numthreads-1]);
  }
  //accounting for single-thread case
  else {
    information[0].threadID = 1;
    information[0].segsize = size;
    information[0].startIndex = 0;
    information[0].endIndex = size - 1;
    information[0].foundIndex = -1;
    pthread_create(&threads[0], NULL, helperfunc, &information[0]);
  }

  //join threads
  for(int j = 0; j < numthreads; j++) {
    pthread_join(threads[j], NULL);
    //nonzero condition
    if(information[j].foundIndex >= 0) { 
      printf("Thread %d found %d at index %d\n", information[j].threadID, userIn, information[j].foundIndex);
      //no need to print "not found" msg.
      found = information[j].foundIndex + 1;
      break;
    }
  }
  if(!found) {
    printf("Not found!\n");
  }

  //free memory
  fclose(fp);
  free(intArr);
  free(threads);
  free(information);
  return 0;
}
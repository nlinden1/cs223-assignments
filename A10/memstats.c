#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include "rand.h"

#define ROUNDS 3
#define BUFFER 5
#define LOOP 10

struct chunk {
  int size;
  int used;
  struct chunk *next;
};

void memstats(struct chunk* freelist, void* buffer[], int len) {
  //TODO: 3 lines (total blocks, total mem. allocated, underutilized mem)
  //NOTE: underutilized memory = unused memory/allocated memory
  int blocks = 0, blocks_free = 0, blocks_used = 0, bytes = 0, bytes_free = 0, bytes_used = 0, bytes_unused = 0;
  struct chunk* chk;
  //calculate used memory
  for(int i = 0; i < len; i++) {
    if(buffer[i] != NULL) {
      chk = (struct chunk*)buffer[i] - 1;
      bytes += chk->size;
      bytes_used += chk->size;
      blocks++;
      blocks_used++;
    }
  }
  //calculate free memory
  struct chunk* next_chunk = freelist;
  while(next_chunk != NULL) {
    blocks++;
    blocks_free++;
    bytes += next_chunk->size;
    bytes_free += next_chunk->size;
    next_chunk = next_chunk->next;
  }
  //print
  printf("Total blocks: %d Free blocks: %d Used blocks: %d\n", blocks, blocks_free, blocks_used);
  printf("Total memory allocated: %d Free memory: %d Used memory: %d\n", bytes, bytes_free, bytes_used);
  printf("Underutilized memory: %.2f\n", (bytes_unused * 1.0)/bytes_used);
}

int main ( int argc, char* argv[]) {
  printf("Starting test..\n");
  srand(100);
  double timer;
  struct timeval tstart, tend;

  gettimeofday(&tstart, NULL);

  void *buffer[BUFFER];
  for (int i = 0; i < BUFFER; i++) {
    buffer[i] = NULL;
  }

  void *init = sbrk(0);
  void *current;
  printf("The initial top of the heap is %p.\n", init);
  for (int j = 0 ; j < ROUNDS; j++) {
    printf("---------------\n%d\n", j);
    for (int i= 0 ; i < LOOP ; i++) {
      int index = rand() % BUFFER;
      if (buffer[index] != NULL) {
        free(buffer[index]);
        buffer[index] = NULL;
        printf("Freeing index %d\n", index);
      } 
      else {
        size_t size = (size_t) randExp(8, 4000); 
        int *memory = NULL;
        memory = malloc(size);

        if (memory == NULL) {
          fprintf(stderr, "malloc failed\n");
          return(1);
        } 
        *memory = 123;
        buffer[index] = memory;
        printf("Allocating %d bytes at index %d\n", (int) size, index);
      }
    }
    extern struct chunk* flist;
    current = sbrk(0);
    int allocated = current - init;
    init = current;

    printf("The new top of the heap is %p.\n", current);
    printf("Increased by %d (0x%x) bytes\n", allocated, allocated);
    memstats(flist, buffer, BUFFER);
  }

  for (int i = 0; i < BUFFER; i++) {
    free(buffer[i]); 
  }
  gettimeofday(&tend, NULL);
  timer = tend.tv_sec - tstart.tv_sec + (tend.tv_usec - tstart.tv_usec)/1.e6;
  printf("Time is %g\n", timer);

  return 0;
}

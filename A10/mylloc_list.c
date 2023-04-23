#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

struct chunk {
  int size;
  int used;
  struct chunk *next;
};
struct chunk *flist = NULL;

void *malloc (size_t size) {
  // TODO: Implement malloc with a free list (See reading for details)
  if(size == 0) {
    return NULL;
  }
  struct chunk *next = flist;
  struct chunk *prev = NULL;

  while(next != NULL) {
    if(next->size >= size) {
      if(prev != NULL) { 
        prev->next = next->next;
      }
      else {
        flist = next->next;
      }
      //update used w size of mem. allocated
      next->used = size;
      return (void*)(next+1);
    }
    else {
      prev = next;
      next = next->next;
    }
  }

  //use sbrk to allocate new memory
  //tricky to ask for more memory than the user kernel is requesting
  void *memory = sbrk(size + sizeof(struct chunk));
  if(memory == (void *) -1) {
    return NULL;
  }
  else {
    struct chunk *cnk = (struct chunk*) memory;
    cnk->size = size;
    //again, update used w size
    cnk->used = cnk->size;
    return (void*)(cnk + 1);
  }
}

void free(void *memory) {
  // TODO: Implement malloc with a free list (See reading for details)
  if(memory != NULL) {
    //we're jumping back one chunk position
    struct chunk *cnk = (struct chunk*) ((struct chunk*)memory - 1);
    cnk->next = flist;
    cnk->used = 0; //memory is freed -- no longer using
    flist = cnk;
  }
  return;
}

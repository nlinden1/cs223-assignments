/**
 * Author: Neville Linden
 * Date: 3/22/2023
 * Description: A program that spawns processes agording to the given diagram's specification
 *              and prints helpful output info.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
  pid_t pid;
  printf("%d] A0\n", getpid());
  pid = fork();
  if(pid){
      printf("%d] B0\n", getpid()); 
  }
  else {
    printf("%d] B1\n", getpid());
  }
  if(pid) { 
    pid = fork();
    if(pid) {
      printf("%d] C0\n", getpid());
    }
    else {
      printf("%d] C1\n", getpid());
    }
  }
  printf("%d] Bye!\n", getpid());
  return 0;
}
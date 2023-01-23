/***************************************************
 * mathgame.c
 * Author: Neville Linden 
 * Implements a math game
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
   int answer = 0;
   int norounds = 0;
   int correct = 0;
   time_t t;

   srand((unsigned) time(&t));

   printf("welcome to the math zone. how many math would u like to enjoy?\n");
   printf("(Please type the number of rounds you'd like to play below.)\n"); 
   scanf("%2d", &norounds);
   printf("ok, %2d rounds? get ready to experience some math...\n\n", norounds);

   int randints[norounds*2];
   for(int i = 0; i < norounds*2; i++) {
      int randval = rand() % 10;
      randints[i] = randval;
   }
   for(int i = 0; i < norounds*2; i = i + 2) {
      printf("%d + %d ?= \n ", randints[i],randints[i+1]);
      scanf("%2d", &answer);
      if(answer == randints[i] + randints[i+1]) {
        printf("Correct!\n\n");
        correct++;
      }
      else {
        printf("Incorrect...\n\n");
      }
   }
   printf("You got %d/%d correct.\n", correct, norounds);
   return 0;
}

#include <stdio.h>
#include <string.h>

int main()
{
   printf("Enter a phrase: ");
   char phrase[1024];
   char acro[1024];
   int counter = 0;
   scanf("%[^\n]%*c", phrase);
   for(int i = 0; i < strlen(phrase); i++) {
        if (phrase[i] >= 65 && phrase[i] <= 90) {
            acro[counter] = phrase[i];
            counter++;
        }
   }
   printf("Your acronym is: %s", acro);
   return 0; 
}

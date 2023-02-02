#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    printf("Enter a word: ");
    char buffer[32];
    char symbols[5] = {'@', '!', '#', '*'};
    scanf("%s", buffer);
    for(int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] >= 97 && buffer[i] <= 122) {
            int randval = rand() % 3;
	    //printf("randval: %d\n", randval);
            buffer[i] = symbols[randval];
        }
        
    }
    printf("OMG! %s!\n", buffer);
    return 0;
}

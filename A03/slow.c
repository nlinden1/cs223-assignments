
/*----------------------------------------------
 * Author: Neville Linden
 * Date: 2/8/23
 * Description: Program iteratively adding dots to phrases
 ---------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
    char word[32];
    char* slowspeech;
    char dots[2] = ".";
    int length;
    printf("it's tree time: welcome to the ent speech generator.\n\n");
    printf("please enter a word: ");
    scanf("%[^\n]%*c", word);
    printf("please enter a pause-length: ");
    scanf("%d", &length);
    //slowspeech's size must be the length of the phrase itself + length * the number of dots
    slowspeech = malloc(strlen(word) * (length + 1));

    for(int i = 0; i < strlen(word); i++) {
        //printf("\n%c\n\n", word[i]);
        strncat(slowspeech, &word[i], 1);
        for(int j = 0; j < length; j++) {
            strcat(slowspeech, dots);
        }
    }
    
    printf("%s\n", slowspeech);

    free(slowspeech);
    return 0;
}

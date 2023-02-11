
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
    char word[128];
    char dots[4] = ".";
    int length;
    printf("it's tree time: welcome to the ent speech generator.\n\n");
    printf("please enter a word: ");
    scanf("%[^\n]%*c", word);
    printf("please enter a pause-length: ");
    scanf("%d", &length);
    for(int i = 0; i < strlen(word); i++) {
        printf("%c", word[i]);
        for(int j = 0; j < length; j++) {
            printf("%s", dots);
        }
    }
    
    return 0;
}

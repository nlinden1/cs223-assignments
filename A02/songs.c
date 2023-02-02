#include <stdio.h>
#include <string.h>

//struct with the various song attributes
struct song {
    char title[32];
    char artist[32];
    int durationmin, durationsec;
    float danceability;
};

//method that prints formatted attributes of a song in a table
int printsongs(struct song s) {
    //catching the niche case of songs >10 minutes, just because it bothered me
    if(s.durationmin >= 10) {
        printf("%-32s artist: %-32s duration: %d:%-9d danceability: %3.2f\n",
           s.title, s.artist, s.durationmin, s.durationsec, s.danceability);
    }
    //normal case
    else {
        printf("%-32s artist: %-32s duration: %d:%-10d danceability: %3.2f\n",
           s.title, s.artist, s.durationmin, s.durationsec, s.danceability);
    }
    return 0;
}

//main method
int main() {
    //initialize songs and their attributes
    struct song song1, song2, song3;

    strcpy(song1.title, "Sinnerman");  
    strcpy(song1.artist, "Nina Simone");               
    song1.durationmin = 10;
    song1.durationsec = 19;
    song1.danceability = 0.40;
    
    strcpy(song2.title, "24K Magic");  
    strcpy(song2.artist, "Bruno Mars");               
    song2.durationmin = 3;
    song2.durationsec = 47;
    song2.danceability = 0.90;
    
    strcpy(song3.title, "Cheat Codes");  
    strcpy(song3.artist, "Danger Mouse & Black Thought");               
    song3.durationmin = 2;
    song3.durationsec = 15;
    song3.danceability = 0.10;
    
    //and put them into an array 
    struct song songs[4] = {song1, song2, song3};
    //initial print
    printf("Welcome to Neville's Music Collection!");
    printf("\n\n=======================\n\n");
    for(int i = 0; i < 3; i++) {
        printsongs(songs[i]);
    }
    
    //getting a song ID from user
    int id;
    printf("\n\n=======================\n\n");
    printf("Enter a song ID to edit [0,1,2]: ");
    scanf("%d", &id);
    
    if(id < 0 || id > 2) {
        printf("\nInvalid choice!");
    }
    else {
        //initialize strings for strcmp, "boolean" for final print
        //empty string for user response
        char a[] = "artist";
        char t[] = "title";
        char dur[] = "duration";
        char d[] = "danceability";
        int printtrigger = 1;
        char response[16];
        
        printf("\nWhich attribute do you wish to edit?");
        printf("[artist, title, duration, danceability]: ");
        scanf("%s", response);
        
        //duration needs two inputs
        if(strcmp(response, dur) == 0) {
            int durmin, dursec; 
            printf("\nEnter %s (min): ", response);
            scanf("%d", &durmin);
            printf("\nEnter %s (sec): ", response);
            scanf("%d", &dursec);
            songs[id].durationmin = durmin;
            songs[id].durationsec = dursec;
        }
        //artist and title are handled essentially the same way
        else if(strcmp(response, a) == 0) {
            char art[32];
            printf("\nEnter %s: ", response); 
            scanf(" %[^\n]%*c", art);
            strcpy(songs[id].artist, art);
        }
        else if(strcmp(response, t) == 0) {
            char tit[32];
            printf("\nEnter %s: ", response); 
            scanf(" %[^\n]%*c", tit);
            strcpy(songs[id].title, tit);
        }
        //danceability is the simplest case
        else if(strcmp(response, d) == 0) {
            printf("\nEnter %s: ", response); 
            float dc;
            scanf("%f", &dc);
            songs[id].danceability = dc;
        }
        else {
            printtrigger = 0;
            printf("\nAttribute not recognized!");
        }
        
        //if a valid edit was made, print to show the result
        if(printtrigger == 1) {
            printf("\n=======================\n\n");
            for(int i = 0; i < 3; i++) {
                printsongs(songs[i]);
            }  
        }
    }
    return 0;
}

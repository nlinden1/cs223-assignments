/*----------------------------------------------
 * Author: Neville Linden
 * Date: 2/8/22
 * Description: A program dynamically allocating memory based on a file's 
contents
 ---------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

struct dysong {
    char title[32];
    char artist[32];
    int duration;
    float danceability, energy, tempo, valence;
};

int printsongs(struct dysong s) {
    //catching the niche case of songs >10 minutes
    if(s.duration >= 600000) {
        printf("%-32s artist: %-32s duration: %d:%02d D: %3.2f\n E: %f T: %f V: %f",
           s.title, s.artist, (s.duration / 1000)/(60), (s.duration / 1000) % 60, 
           s.danceability, s.energy, s.tempo, s.valence);
    }
    //normal case
    else {
        printf("%-32s artist: %-32s duration: %d:%02d D: %3.2f\n E: %f T: %f V: %f",
           s.title, s.artist, (s.duration / 1000)/(60), (s.duration / 1000) % 60, 
           s.danceability, s.energy, s.tempo, s.valence);
    }
    printf("\n\n");
    return 0;
}

int main() {
    //the file itself
    FILE* songcsv;
    //the array of structs holding the song info
    struct dysong* songlist;
    //an array to hold each row of the .csv file
    char row[1000];
    //an array to hold each individual piece of the array
    char* data;
    //the comma
    const char comma[2] = ",";
    //index counter
    
    songcsv = fopen("songlist.csv","r");
    //grab the number of songs from the first row
    int numsongs = atoi(fgets(row, 1000, songcsv));
    //malloc an array of dysongs with correct size
    songlist = malloc(sizeof(struct dysong) * numsongs);
    //read in the second row to skip it
    fgets(row, 1000, songcsv);
    
    //now initialize a new song struct, read in the next line
    //separate tokens, then put into malloc array
    for(int i = 0; i < numsongs; i++)
    {
        struct dysong s;
        fgets(row, 1000, songcsv);
        //song name
        data = strtok(row, comma);
        strcpy(s.title, data);
        //song artist
        data =  strtok(NULL, comma);
        strcpy(s.artist, data);
        //duration
        data =  strtok(NULL, comma);
        s.duration = atoi(data);
        //danceability
        data =  strtok(NULL, comma);
        s.danceability = atof(data);
        //energy
        data =  strtok(NULL, comma);
        s.energy = atof(data);
        //tempo
        data =  strtok(NULL, comma);
        s.tempo = atof(data);
        //valence
        data =  strtok(NULL, comma);
        s.valence = atof(data);

        //put structs in the array
        songlist[i] = s;
    }
        
    //initial print
    printf("\nWelcome to Neville's Music Collection!");
    printf("\n\n=======================\n\n");
    for(int i = 0; i < numsongs; i++) {
        printsongs(songlist[i]);
    }
    
    free(songlist);
    
    return 0;
}

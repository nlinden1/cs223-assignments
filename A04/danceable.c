/*----------------------------------------------
 * Author: Neville Linden
 * Date: 2/14/22
 * Description: A program adding and removing data from a linked list
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

struct node {
  struct dysong d;
  struct node* next;
};

struct node* head = NULL;

//copied from Assignment 3
void printsongs(struct dysong s) {
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
}

//method that iterates through the linked list to print each node's data
void printList() {
   struct node *ptr = head;
   while(ptr != NULL) {
      printsongs(ptr->d);
      ptr = ptr->next;
   }
}

//method that iterates through the linked list to find length
int length() {
   int length = 0;
   struct node *thisnode;
   for(thisnode = head; thisnode != NULL; thisnode = thisnode->next) {
      length++;
   }
   return length;
}

bool isEmpty() {
  return length() == 0;
}

//method that iterates through the linked list, finding the most danceable song
struct dysong mostDanceable() {
   float maxdance = 0.0;
   struct dysong dsong;
   struct node *thisnode;
   for(thisnode = head; thisnode != NULL; thisnode = thisnode->next) {
      if(thisnode->d.danceability > maxdance) {
        maxdance = thisnode->d.danceability;
        dsong = thisnode->d;
      }
   }
   return dsong;
}

//a method that iterates through the linked list to de-link a node with given data
bool delete(struct dysong s) {
   struct node* current = head;
   struct node* previous = NULL;
	
   if(head == NULL) {
      return false;
   }

   while(strcmp(current->d.title, s.title) != 0) {
      if(current->next == NULL) {
         return NULL;
      } else {
         previous = current;
         current = current->next;
      }
   }

   if(current == head) {
      head = head->next;
   } else {
      previous->next = current->next;
   }    
	
   return true;
}

//method that inserts a new node with data from struct s
void insert_front(struct dysong s) {
   struct node* n = (struct node*) malloc(sizeof(struct node));
   
   if (n == NULL) {
    printf("ERROR: Out of space!\n");
    exit(1);
  }

   n->d = s;
   n->next = head;
   head = n;
}

//method that iterates through the list, freeing up memory
void freelist()
{
   struct node* tmp;

   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }

}

int main() {
    //the file itself
    FILE* songcsv;
    //an array to hold each row of the .csv file
    char row[1000];
    //an array to hold each individual piece of the array
    char* data;
    //the comma
    const char comma[2] = ",";
    
    songcsv = fopen("songlist.csv","r");
    //read in the first row to skip it
    fgets(row, 1000, songcsv);
    
    //now initialize a new song struct, read in the next line
    //separate tokens, then put into linked list
    while(fgets(row, 1000, songcsv))
    {
        struct dysong s;
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

        //add it to the linked list
        insert_front(s);
    }
        
    printf("\nWelcome to Neville's Music Collection!");

    //user interaction
    bool keepgoing = true;
    //string to hold char input
    char userint[2] = "\0";
    //string for comparison
    char dee[2] = "d";

    while(keepgoing) {
      printf("\n\n=======================\n\n");
      printList(); 
      printf("\nDataset contains %d songs", length());
      printf("\n\n=======================\n\n");
      printf("Press 'd' to show the most danceable song (any other key to quit): ");
      scanf("%s", userint);

      //quit if not d, otherwise show the most danceable and keep going
      if(strcmp(userint, dee) != 0) {
        keepgoing = false;
      }
      else {
        printf("\nMost Danceable:\n");
        printsongs(mostDanceable());
        delete(mostDanceable());
        printf("\n");
      }

      //handles the case where list has no remaining elements
      if(length() == 0) {
        keepgoing = false;
      }
    }
    freelist();
    printf("Bye!\n");
    return 0;
}
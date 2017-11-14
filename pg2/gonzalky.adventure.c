#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>



struct room
{
    char* name;
    char* type;
    int numConnections;
    char* connections[6];
    //struct room* outboundConnections[6]; //no less 3 no more than 6
}Rooms[7];

pthread_mutex_t myMutext;
pthread_mutex_t secondMutext;
//Read in rooms from directory
char* roomFiles[7]; 
 
//Room Types
char* roomType[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};


// Room List 10 rooms
char* roomNames[10] = {"Room1", "Room2", "Room3", "Room4", "Room5", "Room6", "Room7", "Room8", "Room9", "Room10"};



//https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
void* timeIsBut() {


    FILE *fp;
    fp = fopen("currentTime.txt", "w+");
    time_t rawtime;
    struct tm *info;
    char buffer[80];

   time( &rawtime );

   info = localtime( &rawtime );

   strftime(buffer,80,"%I:%M%p, %A, %B %d, %Y", info);
   
    fputs(buffer, fp);
    fclose(fp);

}

char*  recentDir()
{
  int newestDirTime = -1; // Modified timestamp of newest subdir examined
  char targetDirPrefix[32] = "gonzalky.rooms."; // Prefix we're looking for
  char* newestDirName= malloc(sizeof(char) * 64);; // Holds the name of the newest dir that contains prefix
  memset(newestDirName, '\0', sizeof(newestDirName));

  DIR* dirToCheck; // Holds the directory we're starting in
  struct dirent *fileInDir; // Holds the current subdir of the starting dir
  struct stat dirAttributes; // Holds information we've gained about subdir

  dirToCheck = opendir("."); // Open up the directory this program was run in

  if (dirToCheck > 0) // Make sure the current directory could be opened
  {
    while ((fileInDir = readdir(dirToCheck)) != NULL) // Check each entry in dir
    {
      if (strstr(fileInDir->d_name, targetDirPrefix) != NULL) // If entry has prefix
      {
        stat(fileInDir->d_name, &dirAttributes); // Get attributes of the entry

        if ((int)dirAttributes.st_mtime > newestDirTime) // If this time is bigger
        {
          newestDirTime = (int)dirAttributes.st_mtime;
          memset(newestDirName, '\0', sizeof(newestDirName));
          strcpy(newestDirName, fileInDir->d_name);
        }
      }
    }
  }

  closedir(dirToCheck); // Close the directory we opened

  //printf("Newest entry found is: %s\n", newestDirName);

 return newestDirName; 
}

/*
Going to run this function at the very beginning of int main and fill a Rooms[] with all the values parsed from the files
*/

void storeRoomData() {


	/*****going through directory and storing the names of files into an array *****/

		DIR  *d;
 		 struct dirent *dirEct;
 		 d = opendir(recentDir()); 
		int z; 
		z = 0; 
		printf("%s\n", d); 
	  if (d)
  	  {
   		     while ((dirEct = readdir(d)) != NULL)
    	            {
			if(!strcmp(dirEct->d_name, ".") || !strcmp(dirEct->d_name, ".."))
		        {
            			continue;
            		}
 		 	//printf("%s\n", dirEct->d_name);			 
			//filesList[i] = (char*) malloc (strlen(dir->d_name)+1);
        		//strncpy (filesList[i],dir->d_name, strlen(dir->d_name) );
			https://stackoverflow.com/questions/11291154/save-file-listing-into-array-or-something-else-c		

			/*Had an issue with this was  not allocating memory for roomFiles individual elements. 
			Assisgning it the "dirEct->d_name" (basically pointing each element of your filesList to a
			single d_name). Should be doing a malloc for each entry in there.*/

			roomFiles[z] = (char*) malloc (strlen(dirEct->d_name)+1);	
			strncpy (roomFiles[z],dirEct->d_name, strlen(dirEct->d_name) );  
		
			//roomFiles[z] = dirEct->d_name; 
			z++; 			
		     }

   	           closedir(d);
          }
}	
	/****************************************************************************/


void getRoomData()
{		


	int z = 7;       
         char dir[100];
	sprintf(dir, recentDir());
	int g; 
   for(g = 0; g < 7; g++)
   {	
         char* roomName;
	
        roomName = roomFiles[g];

        char filename[100];

        sprintf(filename, "./%s/%s", dir, roomName);
     
        FILE *fp;
        fp = fopen(filename, "r");
        if(fp == NULL)
            printf("Cannot open file"); ;
        
        char line[100];
 
        
        fgets(line, 100, fp);
        
        char *name = strtok(line, " ");
        name = strtok(NULL, " "); //we got to the first space SKIPS ROOM  put token at NAME:
        name = strtok(NULL, "\n");//then we got to newline whuch gives us roomName
         
         //calloc(MAX_MONTH_STR,sizeof(char));
        //roomFiles[z] = (char*) malloc (strlen(dirEct->d_name)+1); 
        //strncpy (roomFiles[z],dirEct->d_name, strlen(dirEct->d_name) ); 

        Rooms[g].name = (char*) calloc(25, sizeof(char)); 
        strncpy ( Rooms[g].name,name, strlen(name) );
    
         //printf("Rooms[%d].name: %s\n", g,Rooms[g].name); 
        
        /*****char *strtok(char *str, const char *delim)***/
        int b; 
        b = 0; 
        while(fgets(line, 100, fp) != NULL) 
        {
          char *token = strtok(line, " ");
       	    if(strcmp(token, "CONNECTION") == 0) 
            {
                token = strtok(NULL, " ");
                token = strtok(NULL, "\n");
            
                 //printf("Connection:%s\n", token);
                Rooms[g].connections[b] =  (char*) calloc(25, sizeof(char));  
                strncpy (Rooms[g].connections[b],token, strlen(token));
                
                //printf("Rooms[%d].connections[%d]:%s\n", b, b,  Rooms[g].connections[b]);
                b++; 
           
             }	else 
                {
                  
               		  token = strtok(NULL, " ");
                		token = strtok(NULL, "\n");

                    Rooms[g].type = (char*) calloc(25, sizeof(char)); 
                    strncpy(Rooms[g].type, token, strlen(token));
                   
       		      }
        }
        Rooms[g].numConnections = b; 
	     memset(filename, 0, 100);	//this resets the filename[100] values

    }	//end of for loop
    

}


int findRoomConnections(char* roomName) {
      int j;
  for (j = 0; j < Rooms[j].numConnections; j++) {
    if (strcmp(roomName, Rooms[j].name) == 0)
      return 1;
  }
  // If not found, return false
  return 0;
}

void threadCreation(){
  pthread_mutex_init(&secondMutext, NULL);
  pthread_mutex_lock(&secondMutext);
  pthread_mutex_unlock(&myMutext);
  pthread_t myThreadID; 
  int resultInt; 
  resultInt = pthread_create (&myThreadID,NULL, timeIsBut, NULL);
  pthread_mutex_lock(&myMutext);
  pthread_mutex_destroy(&myMutext);
     usleep(100);
}

//PRINTS currentTime.txt file
 void anIllusion() {

    char buffer[80];
    FILE *fp;
    fp = fopen("currentTime.txt", "r");
    if(fp == NULL) {
        perror("Error\n");
    }
    else {
            
        fgets(buffer, 50, fp);
        printf("\n%s\n", buffer);
        fclose(fp);
    }

} 

void playGame()
{  
    //getting initial start room
    int i;
    int room; 
    int conns;
    int steps = 0; 
    const char* pathTaken[500];
    int endRoom = 0;
    int correctInput = 0;
    char *buffer;
    size_t bufsize = 32;
    size_t characters; 
    char* deststring;
    deststring = (char *)calloc(bufsize, sizeof(char));

    buffer = (char *)calloc(bufsize, sizeof(char));
    
    if( buffer == NULL)
    { perror("Unable to allocate buffer");
      exit(1);
    }

    for (i = 0; i < 7; ++i)
    {
      if (strcmp(Rooms[i].type, roomType[0]) == 0)
      {
        room = i;
        break;  
      }
    }
  

  do {
    pthread_mutex_init(&myMutext, NULL);
    pthread_mutex_lock(&myMutext);

        //THE GAME BEGINS FINALLY!!!!!!!!
        printf("\nCURRENT LOCATION: %s\n",Rooms[room].name);
        printf("POSSIBLE CONNECTIONS:");
        for (conns = 0;  conns < Rooms[room].numConnections; conns++) {
            printf(" %s,", Rooms[room].connections[conns]);
        }

      
        printf(".\nWHERE TO? > ");
        characters = getline(&buffer,&bufsize,stdin);

        strncpy(deststring, buffer, strlen(buffer)-1);

       
        if(strcmp(deststring, "time") == 0) {
          pthread_mutex_unlock(&myMutext); 
          threadCreation(); 
          anIllusion(); 

        }else if (findRoomConnections(deststring) == 0)
        {
            printf("\nHUH? I DON'T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
        } 

        if(findRoomConnections(deststring) == 1)
          {
            for (i = 0; i < 7; ++i)
            {
              if (strcmp(Rooms[i].name, deststring) == 0)
              {
                room = i;
                break;  
              }
            }
            pathTaken[steps] = Rooms[room].name; 
            steps++;
          }

        if(strcmp(Rooms[room].type,roomType[1]) == 0)
        {
            endRoom = 1;
            break; 
        }
        

        free(buffer); 
        free(deststring); 
        
    } while(!endRoom);


    //ENDDD!!!!!!!!!!! SOOOO MANY BUGGSSSSSSSSS
    printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", steps);
    int end; 
    for(end = 0; end < steps; end++)
        printf("%s\n",pathTaken[end]);


}



int main()
{

  

	storeRoomData(); 
	getRoomData(); 
   
  playGame(); 
  

/*
  int i;
  int j;  
for (i = 0; i < 7; ++i)
{
    free(roomFiles[i]);
    free(Rooms[i].name);
    free(Rooms[i].type); 
   for (j = 0; j < Rooms[i].numConnections; ++j)
  {
    free(Rooms[i].connections[j]);
  }
}*/
   
 return 0; 
}






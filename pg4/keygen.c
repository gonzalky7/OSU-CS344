#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


//generate a key: The 27 characters are the 26 capital letters, and the space character ( ).



int main(int argc, const char * argv[]) {
	//The srand() requires an unsigned int as parameter (srand(unsigned int)) but time() returns a long int (long int time())
  srand((unsigned int) time(NULL) );
  int keyLength; //how long the user wants the key to be  
  
    
   if(argc < 2){
	printf("Need to enter a key length\n"); 
	exit(1); 
    } 
   //https://stackoverflow.com/questions/523237/how-to-read-in-numbers-as-command-arguments 
    keyLength = atoi(argv[1]); 

   
   int i; 
   char randomLetter;
   
   for(i = 0; i < keyLength; i++)
   {	
	randomLetter = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[rand() % 27];
    	printf("%c", randomLetter);
   }

    printf("\n"); 	 
    return 0;
}



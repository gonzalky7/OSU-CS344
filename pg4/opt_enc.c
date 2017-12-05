#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#define SIZE 70000

int socketFD, portNumber, charsRead, charsWritten;
struct sockaddr_in serverAddress;
char buffer[SIZE];
char completeMessage[SIZE];


void error(const char *msg) { perror(msg); exit(0); } // Error function used for reporting issues

//this function gets the arguments passed in the command line starting with the plaint text file and stores it in char completeMessage[]
//Then it gets the second argument which is the key and concatenates it with the plain text file to create a long string to send to server
int readFile(char* filename){
    
    FILE* infile = fopen(filename, "r");

    long numbytes;
    
    
    /* Get the number of bytes */
    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);
    
    
    fseek(infile, 0L, SEEK_SET);
    
   
    fread(buffer, sizeof(char), numbytes, infile);
    //buffer[strcspn(buffer, "\n")] = '\0';
    
  
    strcat(completeMessage, buffer);//Creating a complete message to send to the server
    
    strcat(completeMessage, "@");//adding th @symbol to know when the message ends and the key begins will handle in the opt_enc_d.c file
    
    //https://stackoverflow.com/questions/8679547/send-large-files-over-socket-in-c
    fclose(infile);
    return numbytes;
}

void readKeyFile(char* filename, int numbytes){
    
    FILE* infile = fopen(filename, "r");
    //printf("inside readKeyFIle numbytes:%d\n", numbytes);
    numbytes--;
    printf("inside readKeyFIle opt_enc numbytes:%d\n", numbytes);
    fread(buffer, sizeof(char), numbytes, infile);
  
    strcat(completeMessage, buffer);//Creating a complete message to send to the server
    
    strcat(completeMessage, "@");//adding th @symbol to know when the message ends and the key begins will handle in the opt_enc_d.c file
    
    //https://stackoverflow.com/questions/8679547/send-large-files-over-socket-in-c
    fclose(infile);
}


//This function checks the input of the plaintext file and makes sure that only the accepted characters are being passed
int checkInput(char* filename1){
    size_t n = 0;
    int c;
    int i;
    long fileSize;
    
    FILE* argumentFile = fopen(filename1, "r");
    
    if (argumentFile == NULL) {
        fprintf(stderr, "Can't open input file!\n");
        exit(1);
    }
    
    fseek(argumentFile, 0, SEEK_END);
    fileSize = ftell(argumentFile);//getting the number of bytes of the file
    fseek(argumentFile, 0, SEEK_SET);

    //http://www.cplusplus.com/reference/cctype/isalpha/
    while ((c = fgetc(argumentFile)) != EOF) { //parsing through the file char by char and making sure that the character is atleast a a charcter and a space
            if(isalpha(c) || isspace(c)){
            }
            else{
                fprintf(stderr, "Invalid characters in %s\n", filename1);
                return 0;
                exit(1);
            }
    }
    fclose(argumentFile);
    
    return 1;
}


int checkKeyForSize(char* filename1, char* keyFile){
   long keyFileSize;
    long messageSize;
    
    FILE* key = fopen(keyFile, "r"); //opening the keyfile to gets its size
    fseek(key, 0L, SEEK_END); //this will seek the End of file
  
    keyFileSize = ftell(key); //gets the size of the file in bytes
    
    FILE* message = fopen(filename1, "r");         //Get the text and the size of the text
    fseek(message, 0L , SEEK_END);
    
    messageSize = ftell(message);
    
    if(messageSize > keyFileSize){                     //If text is larger than the key, tell user they need a bigger key
        fprintf(stderr, "Error, the key is too short\n");
        //exit(1);
        return 0; //returns 0 if the key is too smal
    }
    return 1; //returns 1 if the key is the right size
}



int main(int argc, char *argv[])
{
    struct hostent* serverHostInfo;
    int check = 0;
    int checkSize = 0;
    // Set up the server address struct
     memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
     portNumber = atoi(argv[3]); // Get the port number, convert to an integer from a string
     serverAddress.sin_family = AF_INET; // Create a network-capable socket
     serverAddress.sin_port = htons(portNumber); // Store the port number
     serverHostInfo = gethostbyname("localhost"); // Convert the machine name into a special form of address
     if (serverHostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
     memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length);
    // Copy in the address
    memset(completeMessage,'\0',sizeof(completeMessage));

    // Set up the socket
     socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
     if (socketFD < 0) error("CLIENT: ERROR opening socket");
     // Connect to server
     if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to addy
     error("CLIENT: ERROR connecting");

    
    
    
    check = checkInput(argv[1]); //this will check for the right charceter in each files
    checkSize = checkKeyForSize(argv[1],argv[2]);//this will check that the key is not smaller than the message
    
   
    if (check == 1 && checkSize == 1){ //once both checks return correct our variables are set to 1 and the rest of the code can execute
        //Sending argument passed in the command line
        int textSize;
        textSize = readFile(argv[1]);
        //printf("textSize:%d\n", textSize);
        readKeyFile(argv[2], textSize); //send only the size of message for the ky
    
    // Send message to server
    charsWritten = send(socketFD, completeMessage, strlen(completeMessage), 0); // Write to the server
        if (charsWritten < 0) error("CLIENT: ERROR writing to socket");
        
        if (charsWritten < strlen(completeMessage)) printf("CLIENT: WARNING: Not all data written to socket!\n");
        
   
     // Get return message from server
     memset(buffer, '\0', sizeof(buffer));
     memset(completeMessage,'\0',sizeof(completeMessage));
     charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
     if (charsRead < 0) error("CLIENT: ERROR reading from socket");
    
     
     close(socketFD); // Close the socket
    }
    close(socketFD); // Close the socket
 return 0;
 }






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

/* otp_dec Similarly, this program will connect to otp_dec_d and will ask it to decrypt ciphertext using a passed-in ciphertext and key, and otherwise performs exactly like otp_enc, and must be runnable in the same three ways. otp_dec should NOT be able to connect to otp_enc_d, even if it tries to connect on the correct port - you'll need to have the programs reject each other, as described in otp_enc. */


//As you probably already know this code is mostly identical to otp_enc and otp_enc_d
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
    
    //https://stackoverflow.com/questions/8679547/send-large-files-over-socket-in-c
    /* Get the number of bytes */
    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);
    
    
    fseek(infile, 0L, SEEK_SET);
    
    
    fread(buffer, sizeof(char), numbytes, infile);
    //buffer[strcspn(buffer, "\n")] = '\0';
    
    
    strcat(completeMessage, buffer);//Creating a complete message to send to the server
    
    strcat(completeMessage, "$");//adding th  $ symbol to know when the message ends and the key begins will handle in the opt_enc_d.c file
    
    
    fclose(infile);
    return numbytes;
}

void readKeyFile(char* filename, int numbytes){
    //https://stackoverflow.com/questions/8679547/send-large-files-over-socket-in-c
    FILE* infile = fopen(filename, "r");

    //printf("inside readKeyFIle opt_dec numbytes:%d\n", numbytes);
    fread(buffer, sizeof(char), numbytes, infile);
    
    strcat(completeMessage, buffer);//Creating a complete message to send to the server
    
    strcat(completeMessage, "@@");//adding th @symbol to know when the message ends and the key begins will handle in the opt_enc_d.c file
    
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
        exit(1);
        //return 0; //returns 0 if the key is too smal
    }
    return 1; //returns 1 if the key is the right size
}



int main(int argc, char *argv[])
{
    struct hostent* serverHostInfo;
    int check = 0;
    int checkSize = 0;
    char serverCheck[] = "KEY@@%%TEXT**PLAIN TEXT";
    
    
    if(argc != 4){     //Checking the number of arguments that are passed expecting 2
        fprintf(stderr, "Incorrect number of arguments\n");
        exit(EXIT_FAILURE);
    }
    
    
    
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
    memset(buffer, '\0', sizeof(buffer));
    // Set up the socket
    socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (socketFD < 0) error("CLIENT: ERROR opening socket");
    // Connect to server
    if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to addy
        error("CLIENT: ERROR connecting");
    
    
    /*******This code sends a message to server to make sure it is the right one************/
    write(socketFD, serverCheck, sizeof(serverCheck));    //Authentication
    read(socketFD, buffer, sizeof(buffer));
    
    if(strcmp(buffer, "KEY@@%%TEXT**PLAIN TEXT") != 0){
        fprintf(stderr, "Error: could not contact otp_dec_d on port %d\n", portNumber);
        exit(2);
    }
    memset(buffer, '\0', sizeof(buffer));//clearing out buffer
    /*************End of checking for correct server******************************************/
    
    
    
    
    
    
    
    
    check = checkInput(argv[1]); //this will check for the right charceter in each files
    checkSize = checkKeyForSize(argv[1],argv[2]);//this will check that the key is not smaller than the message
    
    
    if (check == 1 && checkSize == 1){ //once both checks return correct our variables are set to 1 and the rest of the code can execute
        //Sending argument passed in the command line
        int textSize;
        textSize = readFile(argv[1]);
        readKeyFile(argv[2], textSize); //send only the size of message for the ky
        
        //printf("completeMessage in otp_dec:%s\n", completeMessage);
        size_t CHUNKS;
        CHUNKS = strlen(completeMessage);
        int n;
        int j;
        // Send message to server
        for (j = 0; j < CHUNKS;)//using a for loop to cycle through until the whole message is sent
        {
            
            n = write(socketFD, &completeMessage[j], CHUNKS);
            if (n < 0) {
                printf("error nothing written otp_enc\n");
                break;
            }
            j += n;
            //printf("j:%d\n", j);
            //printf("n:%d\n", n);
        }
  
        // Get return message from server
        memset(completeMessage,'\0',sizeof(completeMessage));
        
        
        //Receiving message --using the example given in the lecture to read bits at a time
        char readBuffer[100];
        memset(buffer, '\0', sizeof(buffer)); // Clear the buffer
        int r;
        while (strstr(buffer, "@@") == NULL) // As long as we haven't found the terminal...
        {
            memset(readBuffer, '\0', sizeof(readBuffer)); // Clear the buffer
            r = recv(socketFD, readBuffer, sizeof(readBuffer) - 1, 0); // Get the next chunk
            
            strcat(buffer, readBuffer); // Add that chunk to what we have so far
            
            //if (r == -1) { printf("r == -1\n"); break; } // Check for errors
            //if (r == 0) { printf("in otp_enc r == 0\n"); break; }
        }
        int terminalLocation = strstr(buffer, "@@") - buffer; // Where is the terminal
        buffer[terminalLocation] = '\0'; // End the string early to wipe out the terminal

        
        
        strcat(buffer, "\n");
        //this will be the decrypted message from server or otp_dec_d.c
        printf("%s", buffer);
        if (charsRead < 0) error("CLIENT: ERROR reading from socket");
        close(socketFD); // Close the socket
    }
    close(socketFD); // Close the socket
    return 0;
}





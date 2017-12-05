//
//  server.c
//  
//
//  Created by Kyleen Gonzalez on 11/19/17.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define SIZE 70000

char encryptedMessage[SIZE];
char message[SIZE];
char key[SIZE];
char buffer[SIZE];


//THis function takes a single character from message and returns the number of the location the character matches in the accepted characters array
int changeCharToInt(char charToChange){
    const char *characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; //Storing allowed variables in a char array
    unsigned long size;
    size = strlen(characters); // getting the size of char array so we can use it for the for loop
    int i;
    for (i = 0; i < size; i++) {
        if (charToChange == characters[i]) { //we take the passed char and search it from the characters array and return its position in the array
            return i;
        }
    }
    return 0;
}
//This function takes the characters that were changed from charToInt uses the algorithm according to OTP
char encryptedChar(int messageInt, int keyInt){
    const char *characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
    int messagePlusKey;
    
    messagePlusKey = messageInt + keyInt; //calculating the encryption by adding the messgeInt and Keyint that was returned from the function above
    
    if (messagePlusKey > 27 || messagePlusKey == 27){ //checking if number is greater than or equals to 27
        messagePlusKey = messagePlusKey - 27; // then we subtact it from 27 like the hw description explained
        return characters[messagePlusKey]; //we return the character from the position calculated from characters arra
    }
    
    return characters[messagePlusKey]; //if not greater than 27 or == 27 return char from character string
}


void encryptMessage(char k[], char m[]){

    memset(encryptedMessage, '\0', sizeof(encryptedMessage)); //clearing out
    
    unsigned long size;
    size = strlen(m);//getting the length of the message so we can encypt word for word
    size--; //to make up for newline as last charcter, not doing this ended up adding an extra encrypted character at the end and I was failing the gradingscript for plaintext size not matching ciphertext size
    
    int i;
    for (i = 0; i < size; i++) { //Here we cycle through the size of the message
        int messageToInt, keyToInt;
        char cipherChar;
        
        messageToInt = changeCharToInt(m[i]);//getting the returned number of char
        keyToInt = changeCharToInt(k[i]);//getting the returned number of char
        
        cipherChar = encryptedChar(messageToInt, keyToInt);//Getting the ciphered character if ciphered is a word !?!?!
        encryptedMessage[i] = cipherChar;//looping through encryptedMEssage and adding the cipher char into it
    }
}




void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

int main(int argc, char *argv[])
{

    if(argc != 2){                  //Checking the number of arguments that are passed expecting 2
        fprintf(stderr, "Incorrect number of arguments");
        exit(EXIT_FAILURE);
    }

    int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
    socklen_t sizeOfClientInfo;
    pid_t pid;

    struct sockaddr_in serverAddress, clientAddress;
    
    if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); } // Check usage & args
    
    // Set up the address struct for this process (the server)
    memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
    portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
    //printf("inside otp_enc_d port number:%d\n", portNumber);
    serverAddress.sin_family = AF_INET; // Create a network-capable socket
    serverAddress.sin_port = htons(portNumber); // Store the port number
    serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process
    
    // Set up the socket
    listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
    if (listenSocketFD < 0) error("ERROR opening socket");
   	
 
    // Enable the socket to begin listening
    if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
        error("ERROR on binding");
    listen(listenSocketFD, 5); // Flip the socket on - it can now receive up to 5 connections
   
    
while(1) {
    
    
    // Accept a connection, blocking if one is not available until one connects
    sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
    
    
    establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
    
    
    //printf("establishedConnection otp_enc_d:%d\n", establishedConnectionFD);
    if (establishedConnectionFD < 0) error("ERROR on accept");
    //printf("otp_enc_d: port %d\n", ntohs(clientAddress.sin_port));
    
    pid = fork(); //creating child process
    //printf("PID in otp_enc_d:%d\n", pid);
    if(pid < 0){
        perror("Error on fork");
        exit(1);
    }
    if(pid == 0){//if pid is child
        memset(buffer, '\0', sizeof(buffer));
        memset(message, '\0', sizeof(message));
        memset(key, '\0', sizeof(key));
        
    
        /*******This code sends a message to server to make sure it is the right one************/
        read(establishedConnectionFD, buffer, sizeof(buffer) - 1); //receiving message to make sure correct client is connecting
        
        if(strcmp(buffer, "@@PLAIN TEXT%%KEY TEXT**") != 0){//specific message that needs to match
            char reply[] = "reply unvalid";
            write(establishedConnectionFD, reply, sizeof(reply));
            exit(2);
        }else {
            char reply[] = "@@PLAIN TEXT%%KEY TEXT**";
            write(establishedConnectionFD, reply, sizeof(reply));
        }
         /*************End of checking for correct server******************************************/
        
        
            char readBuffer[100];
            memset(buffer, '\0', sizeof(buffer)); // Clear the buffer
            int r;
            while (strstr(buffer, "@@") == NULL) // As long as we haven't found the terminal...
            {
                memset(readBuffer, '\0', sizeof(readBuffer)); // Clear the buffer
                r = recv(establishedConnectionFD, readBuffer, sizeof(readBuffer) - 1, 0); // Get the next chunk
                
                strcat(buffer, readBuffer); // Add that chunk to what we have so far

                //if (r == -1) { printf("r == -1\n"); break; } // Check for errors
                //if (r == 0) { printf("int otp_enc_d r == 0\n"); break; }
            }
            int terminalLocation = strstr(buffer, "@@") - buffer; // Where is the terminal
            buffer[terminalLocation] = '\0'; // End the string early to wipe out the terminal
        
        
        
        /*************All this code between comment is parsing through received buffer and putting them in designated char arrays message and key*****************/
            int i;
            int m;
            int k;
            k = 0;
            m = 0;
            i = 0;
            while (buffer[i] != '$') {//while buffer does not equal the @ symbol fill in the message
                m++;//message size
                ++i;//STOP GETTING RID OF THIS YOU NEED TO GO THROUGH BUFFFFFFEERRRRRRRRR
            }
            strncpy(message, buffer, m); //copy buffer into message
        
            i++; //think I am at newline after above code
                while (buffer[i] != '@') {//while buffer does not equal the @ symbol fill in the message
                    key[k] = buffer[i];
                    ++i;//STOP GETTING RID OF THIS YOU NEED TO GO THROUGH BUFFFFFFEERRRRRRRRR
                    ++k;
                }
       // printf("message:%s\n" ,message);
        //printf("key:%s\n" ,key);
        
        encryptMessage(key, message);//function that encypts message
        /************************End parsing buffer and calling encryptMessage function********************************/
        
        
        strcat(encryptedMessage, "@@");//To use the code given in the lecture have to add this to the end of the char array
        
    
        
        // Send encrypted message back to clietn or otp_enc.c
        charsRead = send(establishedConnectionFD, encryptedMessage, strlen(encryptedMessage), 0); // Send success back
        if (charsRead < 0) error("ERROR writing to socket");
        
        
        //Clearning up
        memset(buffer, '\0', sizeof(buffer));
        memset(message, '\0', sizeof(message));
        memset(key, '\0', sizeof(key));
    }//end of if(pid == 0) loop
    close(establishedConnectionFD); // Close the existing socket which is connected to the client
 }//end of while loop
    
    close(listenSocketFD); // Close the listening socket
    return 0;
}


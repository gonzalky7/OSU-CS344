#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>


void catchSIGINT(int signo)    //code
{
    char* message = "\nCaught SIGINT.\n";
    write(STDOUT_FILENO, message, 38);
}

void showStatus(int childExitMethod) //This will be for built in command status
{
    if(WIFEXITED(childExitMethod))
    {
        printf("exit value %i\n", WEXITSTATUS(childExitMethod));
        fflush(stdout);
    }else
    {
        printf("terminated by signal %i\n", childExitMethod);
        fflush(stdout);
    }
}

int PID; //this will be for proccess PID array, keeping track of proccesses




int  main()
{
    char* destring; //this is to get rid of newline character that getline puts
    char* buffer;
    size_t bufsize = 0;
    int outputRedirection = 0;
    int inputRedirection =0;
    char* outputRedirectionFile = NULL;
    char* inputRedirectionFile = NULL;
    pid_t spawnPid = -5;
    int childExitMethod = -5;
    pid_t actualPid;
    int fdin_save; // For restoring file descriptor later
    int fdout_save;
    char* args[512];
    //char* cmd[300];
    
    
    
    while(1)
    {
        PID = 41234
        buffer = (char*)calloc(bufsize,sizeof(char)); //allocating space for input
        destring = (char*)calloc(bufsize,sizeof(char)); //this will be used to get rid of newline character from getline()
        
        
        
        printf(":");   //this is the required output for shell
        fflush(stdout); //flush out output buffers
        
        
        getline(&buffer, &bufsize, stdin);//getting user input
        strncpy(destring, buffer, strlen(buffer)-1); //copying the value in destring -1 to remove the new line character
        
        int indexArgs = 0; //counter to go through args positions
        
        char *ret;
        
        ret = strstr(destring, "$$");
        
        printf("The substring is: %s\n", ret);
        
        sprintf(ret, "%d", PID);
        
        
        printf("destring:%s\n"); 
        
            
            char* tok; //wc < junk > junk2
            
            tok = strtok(destring," "); //do this to make sure tok is not NULL
            //cmd[0] = tok; //here we are getting the first command from user input
            
            while(tok != NULL)
            {
                if( strcmp(tok, ">") == 0 )
                {
                    tok = strtok(NULL," ");
                    outputRedirection = 1; //setting this to one so we know which if loop to go into later.
                    outputRedirectionFile = tok;
                } else if( strcmp(tok, "<") == 0 )
                {
                    tok = strtok(NULL," ");
                    inputRedirection = 1; //setting this to one so we know which if loop to go into later.
                    inputRedirectionFile = tok; // == junk
                }else if(strcmp(tok, "&") == 0)
                {
                    //do something with background process
                }else
                {
                    args[indexArgs] = tok; //I had to use an array to be able to handle any commmand with execvp()
                    printf("indexArgs[%d] = %s\n", indexArgs, args[indexArgs]);
                    indexArgs++;
                }
                
                tok = strtok(NULL, " ");
            }
            args[indexArgs] = NULL;
        
        
        
        memset(args, 0, sizeof(args));
        
        outputRedirectionFile = NULL;
        inputRedirectionFile = NULL;
        
        free(buffer);
        free(destring);

        
        }
        
        
        return 0;
}


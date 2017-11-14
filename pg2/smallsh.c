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
        }else
        {
            printf("terminated by signal %i\n", childExitMethod);
        }
}

pid_t PID; //this will be for proccess PID array, keeping track of proccesses


int  main()
{
    char* destring; //this is to get rid of newline character that getline puts
    char* buffer;
    size_t  bufsize =32;
    int outputRedirection = 0;
    int inputRedirection =0;
    char* outputRedirectionFile = NULL;
    char* inputRedirectionFile = NULL;
    pid_t spawnPid = -5;
    int childExitMethod = -5;
    pid_t actualPid;
    int in;
    int out;
    in = dup(0);    //create a copy of the file desc
    out = dup(1);    //create a copy of the file desc
    
    buffer = (char*)calloc(bufsize,sizeof(char)); //allocating space for input
    destring = (char*)calloc(bufsize,sizeof(char)); //this will be used to get rid of newline character from getline()
    
    
        while(1)
        {
            printf(": ");   //this is the required output for shell
            fflush(stdout); //flush out output buffers
            
            memset(destring, '\0', sizeof(destring)); //memset() fills a block of memory, make sure block is cleared out.
            memset(buffer, '\0', sizeof(buffer)); //memset() fills a block of memory, make sure block is cleared out.
            
            getline(&buffer, &bufsize, stdin);//getting user input
            strncpy(destring, buffer, strlen(buffer)-1); //copying the value in destring -1 to remove the new line character
            
            
             /**********************************************
                     *         BUILT - in commands                *
                     *                                            *
                     * ********************************************/
            
            if ((strcmp(destring, "exit") == 0)) // exit command
            {
                exit(0);
                
            }else if(strncmp(destring, "cd", 2) == 0) // begin of built in cd command
             {
                char* token;  //getting reading to parse code if there is another string after cd
                char directoryInput[40]; //creating an array to capture input and see if user just entered cd or cd "some directory"
                memset(directoryInput, '\0', sizeof(directoryInput)); //clearing out directoryInput
                token = strtok(destring," ");
                
                    while(token != NULL)
                    {
                        strcpy(directoryInput,token); //Using the while loop to parse the getline input last command will be put into char[]
                        token = strtok(NULL, " ");
                        printf("Token: %s\n", token);
                    }
                     if(strcmp(directoryInput, "cd")== 0)  //seeing if there is a second command if not specified go to home
                     {
                        chdir(getenv("HOME"));
                        printf("Only cd was entered\n");
                     }else
                      {
                        chdir(directoryInput); //else change directory to specified path
                      }
                       if (errno != 0) //error checking
                       {
                        printf( "Error changing dir: %s\n", strerror(errno));
                       }
            }else if (strcmp(destring, "#") == 0) // Looking for comments just continue when comments are entered
             {
                continue;
             }else if(strcmp(destring, "status") == 0)
              {
                showStatus(childExitMethod);
              }
         /**********************************************************************
             *                     I/O redirection                                 *
             *                                                                     *
             * ********************************************************************/
        char* tok;
        
        tok = strtok(destring," ");
           
            while(tok != NULL)
            {
                if( strcmp(tok, ">") == 0 )
                {
                    
                    tok = strtok(NULL," ");
                    outputRedirection = 1; //setting this to one so we know which if loop to go into later.
                    outputRedirectionFile = tok;
                    
                }
                tok = strtok(NULL, " ");
            }
            
            //Output Redirection ls > foo.txt
            if(outputRedirection == 1 ) //this code is taken from the lectures and refactored to take the stdout and write it to file
            {
                int targetFD;
                targetFD = open(outputRedirectionFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);// this will return the file descriptor numberThe open() function shall return a file descriptor for the named file that is the lowest file descriptor not currently open for that process.
                //printf("targetFD: %d\n", targetFD);
                if (targetFD == -1) { perror("open()"); exit(1); }
                
                int result = dup2(targetFD, 1);
                if (result == -1) { perror("dup2"); exit(2); }
                outputRedirection = 0; //have to reset outputRedirection
            }
            
       /**********************************************
             *        Fork(), exec (), waitpid(           *
             *                                            *
             * ********************************************/
            
          
            
                    spawnPid = fork();
                    
                    if((strcmp(destring,"ls")== 0))&& spawnPid == 0)
                    {
                        execlp("ls", "ls", NULL);
                        perror("CHILD: exec failure!\n");
                    }
            
                    /* Waiting for child by calling waitpid */
                    actualPid = waitpid(spawnPid, &childExitMethod, 0);
                    printf("PARENT(%d): Child(%d) terminated, Exiting!\n",getpid(), actualPid);

                                /*****  Checking the EXIT STATUS   *****/
                                if (WIFEXITED(childExitMethod))
                                {
                                    //printf("The process exited normally\n");
                                    int exitStatus;
                                    exitStatus = WEXITSTATUS(childExitMethod);
                                    //printf("exit status was %d\n", exitStatus);
                                }else
                                {
                                   // printf("Child terminated by a signal\n");
                                    int exitSignal;
                                    exitSignal = WIFSIGNALED(childExitMethod);
                                    //printf("exit signal; was %d\n", exitSignal);
                                }
        }
    free(outputRedirectionFile);
    free(buffer);
    free(destring);
    
    return 0;
}




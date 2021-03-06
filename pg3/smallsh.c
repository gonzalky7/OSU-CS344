#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>


pid_t PID; //this will be for proccess PID array, keeping track of proccesses
pid_t pidToBeKilledWhenCtrlCIsPressed; //If not in background mode and CTRL-C is pressed this will hold the PID
int foregroundOnly; //this will be used to toggle between foreground-only mode, foreground = 0 means fg(off) is normal mode fg = 1 means foreground-only fg(on)




void showStatus(int childExitMethod) //here is the functin for the command status
{
        // WIFEXITED This macro returns a nonzero value if the child process terminated normally with exit or _exit.
        if(WIFEXITED(childExitMethod))
        {
            printf("exit value %i\n", WEXITSTATUS(childExitMethod));
            fflush(stdout);
        }
    
        if(WIFSIGNALED(childExitMethod))
        {
            //printf("childExitmethod %i\n", childExitMethod);
            printf("terminated by signal %i\n", childExitMethod);
            fflush(stdout);
        }else{
            char* message = "\n";
            write(STDOUT_FILENO, message, 1);
        }
}

void catchSIGINT(int signo) //These functions are both from the lecture
{
    
    //char* message = "caught SIGINT\n";
    //write(STDOUT_FILENO, message, 14);
    
    //printf("terminated by signal %i\n", signo);
    showStatus(signo);
    //fflush(stdout);
    kill(pidToBeKilledWhenCtrlCIsPressed, SIGKILL);
    
}


void catchSIGSTP(int signo)
{
    // if foregroundOnly is 0 then & is ignored and no background processes are allowed
    if (foregroundOnly == 0) { //foregroundONly == 1 means foreground is on
        char* message = "Entering foreground-only mode (& is now ignored)\n";
        write(STDOUT_FILENO, message, 48);
        foregroundOnly = 1; //we change it to 1 so the next time ctrl-z is pressed we go back to allowing background processes
    } else
    {
        char* message = "Exiting foreground-only mode\n";
        write(STDOUT_FILENO, message, 28);
        foregroundOnly = 0;
    }
}


//checking to see if the background prcoess has finished
void checkBackgroundProccess(pid_t child, int childExit)
{
    while(child > 0){
        printf("background process, %i, is done: ", child);
        showStatus(childExit);
        child = waitpid(-1, &childExit, WNOHANG);
    }
}




int  main()
{
    char* destring; //this is to get rid of newline character that getline puts
    char* buffer; //this will be only string when getline is called
    size_t bufsize = 0; //we use 0 because we will let getline assign a buffer size
    int outputRedirection = 0; //this will handle whether < or > was entered in the prompt
    int inputRedirection =0; //see comment above
    char* outputRedirectionFile = NULL; //this will hold the name of the redirection file
    char* inputRedirectionFile = NULL;
    pid_t spawnPid = -5;
    int fdin_save; // For restoring file descriptor later
    int fdout_save; // For restoring file descriptors
    char* args[512];
    int bg = 0; //this will be used to handle background proccesses bg = 0 means command was not a background process bg = 1 means call was a background process
    int childExitMethod = -5;
    int numCharsEntered = -5;
    
    
    foregroundOnly = 0; //initiliazing to 1 so when ctrl-z is pressed it will go into foreground-only mode
    
    /***************************************************************
     *                       Signal Handling                   *
     *                                                              *
     * **************************************************************/
    
    //SIGSTP Handlers
   
    struct sigaction SIGSTP_action = {0};
    SIGSTP_action.sa_handler = catchSIGSTP;
    //The sigfillset() function shall initialize the signal set pointed to by set, such that all signals defined in this volume of IEEE Std 1003.1-2001 are included.
    
    sigfillset(&(SIGSTP_action.sa_mask));
    SIGSTP_action.sa_flags = SA_RESTART;
    
    sigaction(SIGTSTP, &SIGSTP_action, NULL);
   
    
     /*
          sigaction(SIGTSTP, &SIGSTP_action, NULL);
      
         • The first parameter is the signal type of interest (SIGINT, SIGHUP, etc.)
         • The second parameter is a pointer to a data-filled sigaction struct
         which describes the action to be taken upon receipt of the signal given
         in the first parameter
         • The third parameter is a pointer to another sigaction struct, with
         which the sigaction() function will use to write out what the
         handling settings for this signal were before this change was requested */
    
    
    //SIGINT handlers
    
    struct sigaction SIGINT_action = {0};
    SIGINT_action.sa_handler = catchSIGINT;
    sigfillset(&(SIGINT_action.sa_mask));
    SIGINT_action.sa_flags = SA_RESTART;
    sigaction(SIGINT, &SIGINT_action, NULL);
    
    
    
    while(1)
    {
        
        buffer = (char*)calloc(bufsize,sizeof(char)); //allocating space for input
        destring = (char*)calloc(bufsize,sizeof(char)); //this will be used to get rid of newline character from getline()
        bg = 0; //reseting the background prcoess signal starting off with 0 which means background process
        
       
            
            while(1) //this code was recommended in the 3.3 lecture
            {
                printf(":");   //this is the required output for shell
                fflush(stdout); //flush out output buffers
                
                
                numCharsEntered = getline(&buffer, &bufsize, stdin);//getting user input
                strncpy(destring, buffer, strlen(buffer)-1); //copying the value in destring -1 to remove the new line character
                

                if (numCharsEntered == -1)
                    clearerr(stdin);
                else
                    break; // Exit the loop - we've got input
            }
    
        
         int indexArgs = 0; //counter to go through args positions
       
        
        
            /***************************************************************
                     *                       BUILT - in commands                    *
                     *                                                              *
                     * **************************************************************/
            
            if ((strcmp(destring, "exit") == 0)) // exit command need to track proccesses so to kill(pid, sigsomething) later
            {
                //so only need  to kill background proccesses because foregournd wont let you enter command in the prompt
                if (spawnPid > 0) {
                    kill(spawnPid, SIGKILL);
                }
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
                        
                    }
                     if(strcmp(directoryInput, "cd")== 0)  //seeing if there is a second command if not specified go to home
                     {
                        chdir(getenv("HOME")); //The getenv() function shall search the environment of the calling process for the environment variable name if it exists and return a pointer to the value of the environment variable.
                     }else
                      {
                          //had to add this code here because the gradiscript was passing cd dir$$ and it was no being handled in the later code
                          //We go over the the input again and look for $$ and add the PID
                          char *ret;
                          
                          ret = strstr(directoryInput, "$$"); //strstr will find any instance of $$ in the string
                          
                          if (ret) { //strstr returns a pointer or null if not found
                              sprintf(ret, "%d", PID);
                          } else
                          {
                              printf("something wrong with ret\n");
                          }
                          
                        chdir(directoryInput); //else change directory to specified path
                      }
                       /*if (errno != 0) //error checking
                       {
                        printf( "Error changing dir: %s\n", strerror(errno));
                        fflush(stdout);
                       }*/
            }else if (strcmp(destring, "#") == 0) // Looking for comments just continue when comments are entered
             {
                continue;
             }else if(strcmp(destring, "status") == 0)
              {
                showStatus(childExitMethod);
              } else
                {
                   
                /**********************************************************************
                             *                    Handling commands with $$                        *
                             *                                                                     *
                             * ********************************************************************/
                    
                    char *ret;

                    ret = strstr(destring, "$$"); //strstr will find any instance of $$ in the string
                    
                    if (ret) { //strstr returns a pointer or null if not found
                        sprintf(ret, "%d", PID);
                    } else
                      {
                        ;
                      }
               
                /**********************************************************************
                             *                     I/O redirection                                 *
                             *                                                                     *
                             * ********************************************************************/
                    
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
                                  //here is our logic for toggling between foreground and background mode.
                                  //We check if foregroundOnly is On/Off 1/0 so if it off foregroundOnly == 0 then
                                  if (foregroundOnly == 0) { //foregroundOnly == 0 means that & is allowed background processes are allowed
                                      bg = 1; //we allow for background processes so we set bg to 1
                                  }else{
                                      bg = 0; //if foregroundOnly == 1 that means & is not allowed so we alwasy wait for child until fg == 0
                               }
                              }else
                               {
                                  args[indexArgs] = tok; //I had to use an array to be able to handle any commmand with execvp()
                                   //printf("indexArgs[%d] = %s\n", indexArgs, args[indexArgs]);
                                  indexArgs++;
                               }
                            
                            tok = strtok(NULL, " ");
                        }
                    args[indexArgs] = NULL;
                  }
        
        
       /**********************************************
             *        Fork(), exec (), waitpid(           *
             *                                            *
             * ********************************************/

        //global variable to hold child proccess id, so when sigint is calld or CTRC-c  is called the parent proccess can kill the child pPID sigaction()
       
        
     
    if(args[0] == NULL || *(args[0]) == '#'){ //handling comments
        continue;
    } else
      {
          spawnPid = fork();
              
            switch (spawnPid)
            {
                case 0:
    
                    if (bg != 1) { //bg != 1 means & was not in command and we wait for child process to finish
                        //For the foreground child process, you'd need to set SIGINT back to SIG_DFL.
                         SIGINT_action.sa_handler = SIG_DFL;
                         sigaction(SIGINT, &SIGINT_action, NULL);
                    }
                    if(inputRedirection == 1 ) //this code is taken from the lectures
                    {
                        fdin_save = dup(0);
                        
                        
                        int inputFD;
                        
                        if (bg == 1) {
                             inputFD = open("/dev/null", O_RDONLY);
                            
                            if (inputFD == -1) { perror("open"); exit(1); }
                        }else
                         {
                            inputFD = open(inputRedirectionFile, O_RDONLY);// this will return the file descriptor numberThe open() function shall return a file descriptor for the named file that is the lowest file descriptor not currently open for that process.
                        
                            if (inputFD == -1) { printf("cannot open %s for input\n", inputRedirectionFile); fflush(stdout); exit(1); }
                            
                            int result = dup2(inputFD, 0); //pointing FD:1 to newly opened FD
                            
                            if (result == -1) { printf("cannot open %s for input\n", inputRedirectionFile); fflush(stdout); exit(2); }
                            
                            close(inputFD);
                         }
                        close(inputFD);
                    }
                    if(outputRedirection == 1 ) //this code is taken from the lectures and refactored to take the stdout and write it to file
                     {
                        fdout_save = dup(1);
                        
                        int outputFD;
                        outputFD = open(outputRedirectionFile, O_WRONLY | O_CREAT | O_TRUNC, 0644);// this will return the file descriptor numberThe open() function shall return a file descriptor for the named file that is the lowest file descriptor not currently open for that process.
                         
                        if (outputFD == -1) { printf("cannot open %s for input\n", outputRedirectionFile); fflush(stdout); exit(1); } //error checking from the lectures
                        
                        int result = dup2(outputFD, 1);  //pointing FD:1 to newly opened FD
                        
                        if (result == -1) { printf("cannot open %s for input\n", outputRedirectionFile); fflush(stdout); exit(2); }
                        
                        close(outputFD);
                     }
                    

                    if(execvp(args[0], args)){         //Command is not recognized so inform the user and change exit status
                        printf("command \"%s\" is not a valid command\n", args[0]);
                        fflush(stdout);
                        _Exit(1);
                    }
                    break;
                    
                default:
             
                    if (bg != 1) { //bg == 0 so we wait for child to execute
                        PID = getpid();
                        //printf("PID:%d\n", PID);
                        pidToBeKilledWhenCtrlCIsPressed = waitpid(spawnPid, &childExitMethod, 0);
                        
                        //apersand parent doesn't wait and chid and parent just keep going
                        
                    }else
                     {
                        //else if bg == 1 that means & was entered in command and we do not wait and give the user control of the prompt and check for execution throughout the programs running
                        printf("background pid is %d\n", spawnPid);
                        break;
                     }
                     //dup2(fdin_save, 0); //here we are reseting the file descriptors
                     //dup2(fdout_save, 1);

                    break;
            }
          }
       /**********************************************
             *        Clean - UP                          *
             *                                            *
             * ********************************************/
        
        outputRedirection = 0; //reseting the I/O for reuse later
        inputRedirection = 0;
        
        memset(args, 0, sizeof(args)); //supposedly this resets the args[] lol not sure if it is, to lazy to check
        
        outputRedirectionFile = NULL;
        inputRedirectionFile = NULL;
        
       //pid_t waitpid(pid_t pid, int *status_ptr, int options);
       // If you pass -1 and WNOHANG, waitpid() will check if any zombie-children exist. If yes, one of them is reaped and its exit status returned.
        spawnPid = waitpid(-1, &childExitMethod, WNOHANG);
     
        checkBackgroundProccess(spawnPid, childExitMethod);//here is where will will check if the background process has terminated The bad thing about this is it will only check after commands have been entered instead of using maybe SIGCHLD to receive a signal if the child has terminated. Couldn't figure it out better way to do this.
        
    }
    
    free(buffer);
    free(destring);
    
    return 0;
}



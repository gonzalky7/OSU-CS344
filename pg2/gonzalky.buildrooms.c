#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#define true 1
#define false 0


struct room
{
    char* name;
    char* type;
    int numConnections;
    char* connections[6];
    //struct room* outboundConnections[6]; //no less 3 no more than 6
}Rooms[7];


struct room GetRandomRoom(void);
bool CanAddConnectionFrom(struct room x);
bool ConnectionAlreadyExists(struct room x, struct room y);
void ConnectRoom(struct room x,struct room y);
bool IsSameRoom(struct room x,struct room y);



//Room Types
char* roomType[3] = {"START_ROOM", "END_ROOM", "MID_ROOM"};

// Room List 10 rooms
char* roomNames[10] = {"Room1", "Room2", "Room3", "Room4", "Room5", "Room6", "Room7", "Room8", "Room9", "Room10"};


/*----------------------------Making Directory ---------------------------*/
/*                                                                        */
/*                                                                        */
/*-------------------------------------------------------------------------*/

char* makeDir() {
    
    int proId = getpid();
    char *dirName = "gonzalky.rooms.";
    //https://stackoverflow.com/questions/5172107/c-string-and-int-concaten$
    char* buf= malloc(40);
    
    //printf("My process ID : %d\n", getpid());

    sprintf(buf, "%s%d", dirName, proId);

    mkdir(buf, 0700);

    return buf;
}


void createRoomFiles(char* dir)
{
    //htp://pubs.opengroup.org/onlinepubs/009695399/functions/chdir.html
      chdir(dir);
    
    int i;
    int k;
    
    
    for(i = 0; i<7; i++) {
        
        FILE *fp = fopen(Rooms[i].name, "w");
        fprintf(fp, "ROOM NAME: %s\n", Rooms[i].name);
        
        for(k = 0; k < Rooms[i].numConnections; k++) {
            fprintf(fp, "CONNECTION %d: %s\n", k + 1, Rooms[i].connections[k]);
        }
        
        fprintf(fp, "ROOM TYPE: %s\n", Rooms[i].type);
        
        fclose(fp);
    }
}



/*---------------------Generating Random Rooms ---------------------------*/
/*                                                                        */
/*                                                                        */
/*-------------------------------------------------------------------------*/
void genRandRoom(struct room Rooms[7])
{
    
    int genRooms,randomNumberForName;
    
    int i;
    for(genRooms = 0; genRooms < 7; genRooms++)
    {
        
    

        
        int roomUsed;
        roomUsed = true;
        do{
            randomNumberForName = rand() % 10;
            roomUsed = false;
            for (i = 0; i < 7; i++)
            {
                if(Rooms[i].name == roomNames[randomNumberForName])
                {
                    roomUsed = true;
                }
            }
        
        }while(roomUsed);

        Rooms[genRooms].name = roomNames[randomNumberForName];
        //creating random number of connections
        //Rooms[genRooms].numOutboundConnections = randomNumberForConnections;

        //printf("%s:%d\n", Rooms[genRooms].name, Rooms[genRooms].numOutboundCo$
        }

    /****************************************************************************
     Here I am establishing randomness for types for the room. I automactically choose
     two random number for START_ROOM and END_ROOM. Then I save those values and use
     if statement to check if random number for those two rooms are chosen if not
     the other rooms are labeled with MID_ROOM
     *********************************************************************************/

            int type;
            int randomNumberForStartRoom;
            int randomNumberForEndRoom;

            randomNumberForStartRoom = rand() % 7; //Need a random generated room from$
            randomNumberForEndRoom = rand() % 7;

            //printf("RandomNumberForStartRoom: %d\n", randomNumberForStartRoom);
            //printf("RandomNumberForEndRoom: %d\n", randomNumberForEndRoom);
            Rooms[randomNumberForStartRoom].type = roomType[0];


            if( randomNumberForStartRoom  == randomNumberForEndRoom)
            {
                randomNumberForEndRoom = rand() % 7;
                Rooms[randomNumberForEndRoom].type = roomType[1];
            }else{
                Rooms[randomNumberForEndRoom].type = roomType[1];
             }

            //printf("%s:%s\n", Rooms[randomNumberForStartRoom].name,Rooms[randomNu$
            //printf("%s:%s\n", Rooms[randomNumberForEndRoom].name,Rooms[randomNumb$



            for (type = 0; type < 7; type++)
            {
                if(type != randomNumberForStartRoom && type != randomNumberForEndRoom)
                {
                    //printf("Type:%d Start:%d End:%d\n", type, randomNumberForStartRo$
                    Rooms[type].type = roomType[2];
                }
            }
    
    /****************************************************************************
         Here I am adding a temp room to every generated room outbound connection.
        So later when I do a check if a room has an available outbound connection
         I will check if the room has a TEMP_ROOM struct room
      *********************************************************************************/
    
   
            int tmpRoom;
            int connections;
            for(tmpRoom = 0; tmpRoom < 7; tmpRoom++)
            {
                Rooms[tmpRoom].numConnections = 0;
                
                for (connections = 0; connections < 6; connections++)
                {
                    Rooms[tmpRoom].connections[connections] = "TEMP_ROOM";
            
                    //printf("Room[%d]:%s:Rooms[%d].outboundConnections[%d]->name: %s\n",tmpRoom, Rooms[tmpRoom].name,tmpRoom,connections, Rooms[tmpRoom].connections[connections]);
                }
                //printf("\n");
            }

    
}

/*---------------------LOGIC FOR CONNNECTING ROOM ------------------------*/
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*---------------------------------------------- ------------------------*/

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
bool IsGraphFull()
{
    int checkConnects;

    for (checkConnects = 0; checkConnects < 7; checkConnects++)
    {
        //printf("Connections: %s:%d\n",Rooms[checkConnects].name,  Rooms[checkConnects].numConnections);
        
        if (Rooms[checkConnects].numConnections < 3 || Rooms[checkConnects].numConnections == 5) {
            return  false;
        }
    }
    return true;
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection()
{
    //printf("In AddRandomConnection()\n\n");
    
    struct room A;  // Maybe a struct, maybe global arrays of ints
    struct room B;
    
    
    while(true)
    {
        A = GetRandomRoom();
        
        //printf("In AddRandomConnection()\nA = getRandomRoom() :%s\n\n", A.name);
        
        if (CanAddConnectionFrom(A) == true)
            break;
    }
    
    do
    {
        B = GetRandomRoom();
        //printf("In AddRandomConnection()\nB = getRandomRoom() :%s\n\n", B.name);
    }
    while(CanAddConnectionFrom(B) == false || IsSameRoom(A, B) == true || ConnectionAlreadyExists(A, B) == true);
    
    ConnectRoom(A, B);  // TODO: Add this connection to the real variables,
    ConnectRoom(B, A);  //  because this A and B will be destroyed when this function terminates
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
bool CanAddConnectionFrom(struct room x)
{
    
    int index;
    int i=0;
    for (index = 0; index < 7; index++) {
        if (Rooms[index].name == x.name) {
            i = index;
        }
    }
    
    if (x.numConnections < 6) {
        
        
        //printf("%s number of connections: %d\n", Rooms[i].name, Rooms[i].numConnections);
        return true;
    }
    return  false;
}


// Returns a random Room, does NOT validate if connection can be added
struct room GetRandomRoom()
{
    int randomRoom;
    randomRoom = rand() % 7;
    
    return Rooms[randomRoom];
}


// Returns true if a connection from Room x to Room y already exists, false otherwise
bool ConnectionAlreadyExists(struct room x, struct room y)
{
    int k;
    int index;
    k = 0;
    for (index = 0; index < 7; index++) {
        if (Rooms[index].name == x.name) {
            k = index;
        }
    }
    
    int i;
    for (i = 0; i < 6; ++i)
    {
        //printf("Rooms[1].connections[0] name:%s\n", Rooms[k].connections[0]);
        
        if (Rooms[k].connections[i] == y.name)
        {
            //printf("Inside");
            return true;
        }
    }
    return false;
}


// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(struct room x, struct room y)
{
    
    int index;
    int X = 0;
    int Y = 0;
    for (index = 0; index < 7; index++)
    {
        if (Rooms[index].name == x.name)
        {
            X = index;
        }
        if (Rooms[index].name == y.name) {
            Y = index;
        }

    }
    int connectionSuccesful;
    connectionSuccesful = true;
    
    char* tempR;
    tempR = "TEMP_ROOM";
    
        int i;
        int k;
    
        for (i = 0; i < 6; i++)
        {
            for(k = 0; k < 6; k++)
            {
              
                    if (Rooms[X].connections[i] == tempR && Rooms[Y].connections[k] == tempR)
                    {
                        Rooms[X].connections[i] = Rooms[Y].name;
			Rooms[X].numConnections++;
                        connectionSuccesful = false; 
                          goto end;
                    }
            }
        }
end:
    while (connectionSuccesful==false) {
        break;
    }
    
}

// Returns true if Rooms x and y are the same Room, false otherwise
bool IsSameRoom(struct room x,struct room y)
{
    if (x.name == y.name) {
        return true;
    }else
     {
        return false;
     }
}



int main()
{
    srand(time(NULL));
    genRandRoom(Rooms);
    
    
    while (IsGraphFull() == false)
    {
        AddRandomConnection();
    }
    
    /*
    int tmpRoom;
    int connections;
    for(tmpRoom = 0; tmpRoom < 7; tmpRoom++)
    {
        for (connections = 0; connections < 6; connections++)
        {
	printf("%s number of connections: %d %s\n", Rooms[tmpRoom].name, Rooms[tmpRoom].numConnections, Rooms[tmpRoom].connections[connections]);            
        }
        printf("\n");
    }*/

    char* dir; 
    dir  =  makeDir();
    createRoomFiles(dir); 


    
    return 0;
}


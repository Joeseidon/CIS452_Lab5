#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h"

#define CHAR_BUFFER 256

char path[CHAR_BUFFER];
int id = 'S';
int shmId;
char *shmPtr;
key_t semkey;

void retrieveSmKey(void);
void openSharedMemory(void);
void closeSharedMemory(void);


typedef struct commData{
	char msg[CHAR_BUFFER];
	int receive1;
	int receive2;
}

int running = 1;

int main () {
	commData *data;
	
	retrieveSmKey();
	
	while(running){
		//check for new input by checking receive flag
		
		//print new message
		
		//increment flag

		//wait till flag reset to 0, indicating new msg
		
	
	}
	
	return 0;
}

void retrieveSmKey(void){
	if ((semkey = ftok(getcwd(path,CHAR_BUFFER), id)) == (key_t) -1) {
		perror("IPC error: ftok"); 
		exit(1);
	}
}
//Need to update the following two functions to use semkey
void openSharedMemory(void){
	if ((shmId =
         shmget (IPC_PRIVATE, FOO,
                 IPC_CREAT | S_IRUSR | S_IWUSR)) < 0) {
        perror ("i can't get no..\n");
        exit (1);
    }
    if ((shmPtr = shmat (shmId, 0, 0)) == (void *) -1) {
        perror ("can't attach\n");
        exit (1);
    }
}


void closeSharedMemory(void){
	if (shmdt (shmPtr) < 0) {
        perror ("just can't let go\n");
        exit (1);
    }
    if (shmctl (shmId, IPC_RMID, 0) < 0) {
        perror ("can't deallocate\n");
        exit (1);
    }
}

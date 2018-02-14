#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

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
}commData;

commData *data;

int running = 1;

int main () {
	
	
	retrieveSmKey();
	
	while(running){
		
		printf("Key retrieved: %d\n",semkey);
		
		//wait for user input
		
		//save input to share memeory 
		
		//reset receive flags
		
		printf("Waiting for Readers...\n");
		while(1)
			;
		
		//check receive1 and receive2
		//once both are 1 accept new input message		
	
	}
	
	return 0;
}

void retrieveSmKey(void){
	getcwd(path,CHAR_BUFFER);
	if ((semkey = ftok(path, id)) == (key_t) -1) {
		perror("IPC error: ftok"); 
		exit(1);
	}
}

//Need to update the following two functions to use semkey
void openSharedMemory(void){
	if ((shmId = shmget(semkey,sizeof(data),IPC_CREAT|0666) < 0)) {
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

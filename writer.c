#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SIZE 4096

typedef struct commData{
	char *msg;
	int receive1;
	int receive2;
}

int running = 1;

int main () {
	commData *data;
	
	while(running){
		
		
		//wait for user input
		
		//save input to share memeory 
		
		//reset receive flags
		
		printf("Waiting for Readers...\n");
		
		//check receive1 and receive2
		//once both are 1 accept new input message		
	
	}
	
	return 0;
}

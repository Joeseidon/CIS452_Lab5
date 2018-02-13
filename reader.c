#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SIZE 4096

char path = "/tmp";
char id = 'S';
key_t semkey;

void retrieveSmKey(void);

typedef struct commData{
	char *msg;
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
	if ((semkey = ftok(path, id)) == (key_t) -1) {
		perror("IPC error: ftok"); 
		exit(1);
	}
}

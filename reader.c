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
		//check for new input by checking receive flag
		
		//print new message
		
		//increment flag

		//wait till flag reset to 0, indicating new msg
		
	
	}
	
	return 0;
}

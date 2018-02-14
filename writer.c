/*******************************************************************************
This programming assignment exercises use of a shared memory segment by having
one process that means to communicate to the others by editing the shared memory
and having those listening-processes edit their flags of "have received."

@author   - Joseph Cutino, Brendon Murthum
@version  - Winter 2018

Observations:
  - This assignment should have one running "Writer" process and two listening 
    "Reader" processes.
  - You should be able to create desired behavior with flag(s) for a simple
    "lockstep" synchronization. Writer goes, readers go, writer goes...
  - The Readers need to know which segment to attach to. The preferred method is
    for the Readers and Writer to agree upon a *passkey* that is used to get
    access to the resource. Suggestion of using ftok() to generate a common key.
  - When the Writer types "quit", the all processes should conclude with a
    graceful shutdown, releasing all system resources before termination.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define CHAR_BUFFER 256

char path[CHAR_BUFFER];
int id = 'S';
int shmId;

key_t shmkey;

void retrieveSmKey(void);
void openSharedMemory(void);
void closeSharedMemory(void);

typedef struct commData{
    char msg[CHAR_BUFFER];
    int receive1;
    int receive2;
}commData;

commData data;
commData *shmPtr;

int running = 1;

int main () {

    // Simple Prompt.
    printf("\n");
    printf("Text entered will be shared to two other listening processes.\n");
    printf("\n");

    // Generates a shared-memory "key."
    retrieveSmKey();

    // User-entry holder string
    char userEntry[CHAR_BUFFER];

    // Loop until the user hits "CTRL-C"
    while (running) {

        // Repeated User-Prompt.
        printf("\n");
        printf("Enter Text: ");

        // Waits for user-input for test, OR "quit" to exit.
        fgets(userEntry, CHAR_BUFFER, stdin);

        // To leave the program. Leave loop to avoid new creation of memory.
        if (strcmp(userEntry, "quit") == 0 || strcmp(userEntry, "Quit") == 0) {
            break;
        }

        // Showing key in main.
        // printf("Key retrieved: %x\n",shmkey);

        printf("Point A. \n");

        // A & B. Create and attach the shared memory to process address space.
        openSharedMemory();

        // Write to memory.
        strcpy(shmPtr->msg, "This is a test");
        // strcpy(shmPtr->msg, userEntry);

        // Reset receive flags.
        shmPtr->receive1 = 0;
        shmPtr->receive2 = 0;

        printf("Point B. \n");

        // Wait for both readers to get the message.
        int receiveToggle1 = 0;
        int receiveToggle2 = 0;
        while ( shmPtr->receive1 == 0 || shmPtr->receive2 == 0) {
            // If the reader changed the flag and we haven't seen it yet.
            if (shmPtr->receive1 == 1 && receiveToggle1 == 0) {
                printf("  Reader1 viewed message!\n");
                receiveToggle1 = 1;
            }
            // If the reader changed the flag and we haven't seen it yet.
            if (shmPtr->receive2 == 1 && receiveToggle2 == 0) {
                printf("  Reader2 viewed message!\n");
                receiveToggle2 = 1;
            }
        }

        // C & D. Detach and deallocate shared memory.
        closeSharedMemory();

        printf("Point C. \n");

    }

    // Outside of loop, run the closing instructions.

    return 0;
}

// Retrieve a process key.
void retrieveSmKey(void){
    // Puts the PATH to the variable. Used for the key.
    getcwd(path, CHAR_BUFFER);

    // printf("%s\n",path); // Prints the path.
    if ((shmkey = ftok(path, id)) == (key_t) -1) {
        perror("IPC error: ftok");
        exit(1);
    }
}

// Need to update the following two functions to use shmkey
void openSharedMemory(void) {

    // For debugging.
    printf("shmkey: %x\n",shmkey);

    // Create the shared memory segment.
    if ((shmId
             = shmget(shmkey,sizeof(commData),IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
        printf("id: %d\n",shmId);
        perror ("i can't get no..\n");
        exit (1);
    }

    // perror("cant get\n"); // For early debugging.

    // For debugging.
    printf("shmId: %d\n",shmId);

    // Attach to the new shared memory segment.
    if ((shmPtr = (commData *)shmat (shmId, NULL, 0)) == (void *) -1) {
        perror ("can't attach\n");
        printf("%s\n",strerror(errno));
        exit (1);
    }
}

void closeSharedMemory(void) {
    // Detaches from the shared memory segment.
    if (shmdt (shmPtr) < 0) {
        perror ("just can't let go\n");
        exit (1);
    }

    // Deallocated the shared memory segment.
    if (shmctl (shmId, IPC_RMID, 0) < 0) {
        perror ("can't deallocate\n");
        exit (1);
    }
}

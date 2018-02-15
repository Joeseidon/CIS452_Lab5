/*******************************************************************************
This programming assignment exercises use of a shared memory segment by having
one process that means to communicate to the others by editing the shared memory
and having those listening-processes edit their flags of "have received."

@author   - Joseph Cutino, Brendon Murthum
@version  - Winter 2018

Files:
  * writer.c   - Writes to shared memory
    reader1.c  - Reads from shared memory, edits its flag.
    reader2.c  - Reads from shared memory, edits its flag.

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

Assignment provided by Prof. Hans Dulimarta.
http://www.cis.gvsu.edu/~dulimarh/CS452/Labs/Lab05-SharedMem/

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
#include <signal.h>

#define CHAR_BUFFER 256

/* Variables used for sharing of memory. */

int shmId;
key_t shmkey;
int id = 'S';
char path[CHAR_BUFFER];

/* Declaring the functions used for shared-memory-managment. */

void retrieveSmKey(void);
void openSharedMemory(void);
void closeSharedMemory(void);

/* Declaring signal hanlder. */

void mainCloseSignalHandler(int);

/* Defining and declaring the shared memory structure. */

typedef struct commData{
    char msg[CHAR_BUFFER];
    int receive1;
    int receive2;
    int reader1Present;
    int reader2Present;
} commData;
commData *shmPtr;

/* To manage keyboard interrupt. */

int hasBeenInterrupted = 0;

/* The program's main process. */

int main () {

    // Assign Signal Handler
    signal (SIGINT, mainCloseSignalHandler);

    // Simple Prompt.
    printf("\n");
    printf("Text entered will be shared to two other listening processes.\n");
    printf("\n");

    // Generates a shared-memory "key."
    retrieveSmKey();

    // User-entry holder string.
    char userEntry[CHAR_BUFFER];

    // A & B. Create and attach the shared memory to process address space.
    openSharedMemory();

    // Set the presence of readers to default "Not Available". 
    shmPtr->reader1Present = 0;
    shmPtr->reader2Present = 0;

    // Reset receive flags to "nothing sent yet."
    shmPtr->receive1 = -1;
    shmPtr->receive2 = -1;

    // Wait for the two readers.
    printf("\n");
    printf("Wait for the two readers...\n");
    while (shmPtr->reader1Present == 0 || shmPtr->reader2Present == 0) {
        sleep(1);
        // printf("Reader-1 Status: %d\n", shmPtr->reader1Present);
        // printf("Reader-2 Status: %d\n", shmPtr->reader2Present);
    }
    printf("Ready.\n\n");

    // Loop until the user types "quit".
    int running = 1;
    while (running) {

        // Repeated User-Prompt.
        printf("Enter Text: ");

        // Waits for user-input for test, OR "quit" to exit.
        fgets(userEntry, CHAR_BUFFER, stdin);

        // To leave the program. Leave loop to avoid new creation of memory.
        if ( strcmp(userEntry, "quit\n") == 0 || 
                strcmp(userEntry, "Quit\n") == 0 || 
                hasBeenInterrupted == 1) {
            strcpy(userEntry, "quit\n");
            
            printf("Point A.\n");
            
            running = 0;
        }

        // Write to memory.
        // strcpy(shmPtr->msg, "This is a test");
        strcpy(shmPtr->msg, userEntry);

        // Reset receive flags.
        shmPtr->receive1 = 0;
        shmPtr->receive2 = 0;

        // Wait for both readers to get the message.
        int receiveToggle1 = 0;
        int receiveToggle2 = 0;
        int shouldLeave = 0;
        while (shouldLeave == 0) {
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
            // Run the exit instructions for this loop.
            if (shmPtr->receive1 == 0  || shmPtr->receive2 == 0 ||
                            shmPtr->receive1 == -1 || shmPtr->receive2 == -1 ) {
                shouldLeave = 1;    
            }
        }
    }

    /* Outside entry-loop, run the closing instructions. */

    // Detach and deallocate shared memory.
    closeSharedMemory();

    // For debugging.
	printf("\nAfter the closing of memory.\n");

    return 0;
}

// Retrieve a shared-memory key.
void retrieveSmKey(void) {
    // Puts the PATH to the variable. Used for the key.
    getcwd(path, CHAR_BUFFER);

    // printf("%s\n",path); // Prints the path.
    if ((shmkey = ftok(path, id)) == (key_t) -1) {
        perror("IPC error: ftok");
        exit(1);
    }
}

// Need to update the following two functions to use shmkey
// Allocates and attaches to a new shared memory.
void openSharedMemory(void) {

    // Create the shared memory segment.
    if ((shmId
             = shmget(shmkey,sizeof(commData),IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) {
        printf("id: %d\n",shmId);
        perror ("Failed to create new memory segment.\n");
        exit (1);
    }

    // For debugging.
    printf("  shmkey: %x\n",shmkey);

    // For debugging.
    printf("  shmId: %d\n",shmId);

    // Attach to the new shared memory segment.
    if ((shmPtr = (commData *)shmat (shmId, NULL, 0)) == (void *) -1) {
        perror ("Failed to attach.\n");
        printf("%s\n",strerror(errno));
        exit (1);
    }

}

// Writer Version - Detaches and deallocates the shared-memory.
void closeSharedMemory(void) {
    // Detaches from the shared memory segment.
    if (shmdt (shmPtr) < 0) {
        perror ("Failed to detach.\n");
        exit (1);
    }

    // Deallocated the shared memory segment.
    if (shmctl (shmId, IPC_RMID, 0) < 0) {
        perror ("Failed to deallocate.\n");
        exit (1);
    }
}

// Handling "CTRL-C" exit.
void mainCloseSignalHandler (int sigNum) {

    printf("\n");
    printf("Closing due to interrupt...\n");

    // Stops the main() loop.
    hasBeenInterrupted = 1;

    // Throws an error at the fgets() in main.
    raise(SIGSTOP);

    return;
}

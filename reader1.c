/*******************************************************************************
This programming assignment exercises use of a shared memory segment by having
one process that means to communicate to the others by editing the shared memory
and having those listening-processes edit their flags of "have received."

@author   - Joseph Cutino, Brendon Murthum
@version  - Winter 2018

Files:
    writer.c   - Writes to shared memory
  * reader1.c  - Reads from shared memory, edits its flag.
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
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <string.h>

#define CHAR_BUFFER 256

/* Variables used for sharing of memory. */

int shmId;
key_t shmkey;
int id = 'S';
char path[CHAR_BUFFER];
int sharedMemorySetup = 0;

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

/* Control of main loop. Being global allows an interrupt to end that loop. */

int running = 1;

/* The program's main process. */

int main () {

    // Assign Signal Handler
    signal (SIGINT, mainCloseSignalHandler);

    // Simple Prompt.
    printf("\nText entered on the other process will be seen here.\n\n");

    // Generates a shared-memory "key."
    retrieveSmKey();

    // Wait for shared memory to be available.
    // "Create" and attach the shared memory.
    while (sharedMemorySetup == 0) {
        openSharedMemory();
    }

    // Tell shared memory of presence.
    shmPtr->reader1Present = 1;

    // Wait for both readers to be present.
    printf("\n");
    if (shmPtr->reader1Present == 0 || shmPtr->reader2Present == 0) {
        while (shmPtr->reader1Present == 0 || shmPtr->reader2Present == 0) {
            sleep(2);
            printf("  Reader-2 Status: %d\n", shmPtr->reader2Present);
        }
        printf("\n");
    }

    // Loop until the user types "quit" on the other console.
    while (running) {

        // If nothing has been sent yet, wait here.
        while (shmPtr->receive1 == -1 && running == 1)
            ;

        // Print the message, change flag.
        if (shmPtr->receive1 == 0) {
            // If receives "quit" end the reader.
            if (strcmp(shmPtr->msg, "quit\n") == 0) {
                printf("\nWriter sent a close command...\n");
                running = 0;
            }
            else {
                sleep(1);
                printf("Message: %s", shmPtr->msg);
                shmPtr->receive1 = 1;
            }
        }
    }

    // Detach and deallocate shared memory.
    closeSharedMemory();

    // For debugging.
    // printf("After the closing of memory.\n");

    return 0;
}

// Retrieve a shared-memory key.
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

    // Find the shared memory segment.
    if ((shmId
             = shmget(shmkey,sizeof(commData),S_IRUSR|S_IWUSR)) < 0) {
        // printf("id: %d\n", shmId);
        // perror ("Failed to create new memory segment.\n");
        return;
    }

    // For debugging.
    printf("shmkey: %x\n", shmkey);

    // For debugging.
    printf("shmId: %d\n", shmId);

    // Attach to the new shared memory segment.
    if ((shmPtr = (commData *)shmat (shmId, NULL, 0)) == (void *) -1) {
        // perror ("Failed to attach.\n");
        // printf("%s\n", strerror(errno));
        return;
    }

    // The setup was successful.
    sharedMemorySetup = 1;

    return;
}

// Reader Version - Detaches from the shared-memory.
void closeSharedMemory(void) {
    // Detaches from the shared memory segment.
    if (shmdt (shmPtr) < 0) {
        perror ("Failed to detach. \n");
        exit (1);
    }
}

// Dispatch-Thread use, instrumental in smooth-exit on user's "CTRL-C".
void mainCloseSignalHandler (int sigNum) {

    printf("\n");
    printf ("Closing due to interrupt...\n");

    // Stops the main() loop.
    running = 0;

    // Throws an error at the fgets() in main.
    raise(SIGSTOP);

    return;
}

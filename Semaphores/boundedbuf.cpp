#include <iostream.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "semaphore.h"

const int MAXCHAR = 10;
const int BUFFSIZE = 3;
//PUT_ITEM = 0, TAKE_ITEM = 1
enum {PUT_ITEM, TAKE_ITEM}; // set up names of my 2 semaphores

void producer_proc(SEMAPHORE &, char *);
void parent_cleanup(SEMAPHORE &, int);
void consumer_proc(SEMAPHORE &, char *);

int main(){
	//Shared memory
	int shmid;
	char *shmBUF;

	//The list of semaphores, we need 2 Semaphores
	//=============================================================================
	//Construct an object that contains 2 semaphores inside of it which all
	//begin at zero values
	SEMAPHORE sem(2);
	sem.V(PUT_ITEM); // PUT_ITEM = 1
	sem.V(PUT_ITEM); // PUT_ITEM = 2
	sem.V(PUT_ITEM); // PUT_ITEM = 3
	//=============================================================================

	//Setup the shared memory
	//============================================================================
	//IPC_PRIVATE replaces ftok(".", "u"), IPC_PRIVATE allows the system to create
	//whatever key value it wants, sizeof # of bytes and permission value
	shmid = shmget(IPC_PRIVATE, BUFFSIZE*sizeof(char), PERMS);
	shmBUF = (char *)shmat(shmid, 0, SHM_RND); //shmBuf -> shmGet -> shmAt

	//============================================================================
	//Parent starts with ShamID, ShamBuffer, and Semaphore object
	//Child is then created with a copy of the ShamID, ShamBuffer, and Semaphore Object
	//This way the parent and child both point to the same shared storage
	//The same Semaphore is also being pointed to by the parent and child

	if(fork()){ /* parent process */

		producer_proc(sem, shmBUF);
		parent_cleanup(sem, shmid);

	} else { // child process
		consumer_proc(sem, shmBUF);
	}

	exit(0);
} // main

/*
Consumer Process
Loop
	P(TAKE)
	Take an item
	V(PUT)
	Consume
	Increment Pointer
end
*/
void consumer_proc(SEMAPHORE &sem, char *shmBUF) {
	char tmp;

	for(int k=0; k<MAXCHAR; k++){ //K takes on values between 0 to 9
		sem.P(TAKE_ITEM);
		//Adds either 0, 1, or 2 so that we can access either the first, second,
		//or third byte
		tmp = *(shmBUF+k%BUFFSIZE);
		sem.V(PUT_ITEM);
		cout << "(" << getpid() << ")  "
				<< "buf[" << k%BUFFSIZE << "] "
				<< tmp << endl; //The endl is important because it will flush the buffer
	}
} // child_proc

/*
Bounded By
Producer Process
Loop
	Produce
	P(PUT)
	Copy Item
	V(TAKE)
	increment pointer
end
*/
void producer_proc(SEMAPHORE &sem, char *shmBUF) {

	char data[128];
	cout << "(" << getpid() << ")  Please enter a string --> ";
	cin.getline(data, 127);

	char input;
	for(int k=0; k<MAXCHAR; k++){
		input = data[k];
		sem.P(PUT_ITEM);
		*(shmBUF+(k%BUFFSIZE)) = input;
		sem.V(TAKE_ITEM);
	}
} // parent_proc

void parent_cleanup (SEMAPHORE &sem, int shmid) {

	int status;			/* child status */
	//If there are a dozen child processes, to make the parent go after all the
	//children terminates, while( -1 != wait(0));
	wait(0);	/* wait for child to exit */
	//Deallocate the shared memory
	shmctl(shmid, IPC_RMID, NULL);	/* cleaning up */
	sem.remove();
} // parent_cleanup

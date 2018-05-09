//Daniel Kim
//main.cpp
/*
  Description:
  This file contains a parent program taht forks exactly 4 child processes that
  perform a set of calculations under specific terms.  Only 2 child processes
  may be active at a time and they both must be working on different
  calculations.  The parent and child programs are to terminate once "!wq" is
  typed into the parent program.
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string>
#include <signal.h>
#include "semaphore.cpp"

//Set global namespace
using namespace std;

//Constant Variables
const int CHILD_COUNT = 4;
const int MIN_THRESHHOLD = 100;
const int U_VAL = 827395609;
const int V_VAL = 962094883;
const int PROCESS_1 = 1;
const int PROCESS_2 = 2;
const int PROCESS_3 = 3;
const int PROCESS_4 = 4;
const int MAX_NUMBER = 2147483647;

//Enumerator for the Semaphores
enum {SEM_X};

//Forward declaring methods
void childProcess(SEMAPHORE &sem, bool *sharedMemBuf, int processNumber);
void childWork(SEMAPHORE &sem, bool *sharedMemBuf);
void parentCleanup(SEMAPHORE &sem, int sharedMemId);

int main() {
  int childIdArray{CHILD_COUNT};
  int childId = 0;

  //Create the Semaphores
  SEMAPHORE sem(2);

  //Initialize SEM_X value to 2
  sem.V(SEM_X);
  sem.V(SEM_X);

  //Create the shared memory space
  int sharedMemID = shmget(IPC_PRIVATE, sizeof(bool), PERMS);
  bool *sharedMemBuf = (bool *)shmat(sharedMemID, 0, SHM_RND);

  //Initialize the value of the Shared Buffer boolean value
  *(sharedMemBuf) = true;

  //Create 4 child processes
  for(int i = 0; i < CHILD_COUNT; i++) {
    //Retrieve the id of the child
    childId = fork();
    int proc = i + 1;
    if(childId == 0) {
      //Child Process
      //Initialize random number generator
      srand(time(NULL));
      childProcess(sem, sharedMemBuf, i + 1);
      return 0;
    }else {
      //Parent Process, store the child's id into an array
      cout << "Storing child id: " << childId << endl;
      childIdArray[i] = childId;
    }
  }

  //Store the user's command
  string userCommand = "";

  while(userCommand.compare("!wq")) {
    cout << "Please input a command : ";
    cin >> userCommand;
  }

  //Kill the child processes
  for(int i =0; i < CHILD_COUNT; i++) {
    //Retrieve the ID of the child
    childId = childIdArray[i];
    cout << "Killing child process : " << childId << endl;
    //Kill the child processes
    kill(childId, SIGKILL);
  }

  //Clean up
  parentCleanup(sem, sharedMemID);

  cout << "Exiting Parent program" << endl;
  //Exit the Program
  return 0;
}

//The processes that the children will run depending on their process number
void childProcess(SEMAPHORE &sem, bool *sharedMemBuf, int processNumber) {
  while(true) {
    sem.P(SEM_X);
    childWork(sem, sharedMemBuf);
    sem.V(SEM_X);
  }
}

//The child's work, once randNumber reaches a value less than 100 or when the
//value is 0 when it is modded against V or U, this method will return
void childWork(SEMAPHORE &sem, bool *sharedMemBuf) {
  //Check if we can work on U, else work on V
  bool uIsAvailable = *(sharedMemBuf);
  int calcValue = 0;
  int divVal = 0;
  int modVal = 0;
  //Check which one is going to be the mod value
  if(uIsAvailable) {
    calcValue = U_VAL;
    *(sharedMemBuf) = false;
  } else {
    calcValue = V_VAL;
  }

  int currId = getpid();

  //Continuously try to find a random number and break if conditions are met
  int randNumber = 101;
  float divRemainder = -1;

  while(true) {
    randNumber = rand() % MAX_NUMBER;
    if(randNumber < 100) {
      break;
    }
    //Number is bigger than a 100, check if it is a factor of the division
    //value
    divRemainder = (calcValue / randNumber) - floor(calcValue / randNumber);
    if(divRemainder == 0) {
      break;
    }
    //Number is not a factor of the division value, check if the mod is 0
    modVal = calcValue % randNumber;
    if(modVal == 0) {
      break;
    }
  }

  //You can work with the U value again
  if(calcValue == U_VAL) {
    *(sharedMemBuf) = true;
  }
}

//Clean up the shared memory and semaphores
void parentCleanup(SEMAPHORE &sem, int sharedMemId) {
  cout << "Cleaning up parent" << endl;
  //Deallocate the shared Memory
  shmctl(sharedMemId, IPC_RMID, NULL);
  sem.remove();
}

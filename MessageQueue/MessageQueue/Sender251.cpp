/*
 - File Name: Sender251
 - Sends each event to only ONE RECEIVER
 - Does not accept any acknowledgement messages
 - Terminates on "KILL" COMMAND FROM A SEPARATE TERMINAL
*/


//Initialize Constants
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <stdlib.h>
#include <climits>
#include <stdio.h>
#include "get_info.h"

using namespace std;
//Message sender ID
const int SENDER_ID = 251;
//Defines the size of the character array to be received
const int ARRAY_SIZE = 50;
//The receiver number
const int SENT_TO = 1;
//The number of the sender
const int SENDER_ID_NUM = 251;
//The number of the message type
const int SENT_MESSAGE_TYPE = 1;
//The maximum number that a random can generate to
const int MAX_RANDOM_NUM = 200000000;
//Defines the message flag stating that the first message on the queue is received
const int MESSAGE_FLAG = 0;

//Forward declaring methods
int generateRandomNum();
void initializeSRand();
int allocateQueue();
void setExitMessage(int);

//Message buffer
struct buffer{
    //Define the mtype
    long messageType;
    //Define the sender id number
    long senderID;
    //Define message size
    char message[ARRAY_SIZE];
    
};

/*
 * The main starting point of the application
 */
int main(){
    //Get the message queue id
    int messageQueueId = msgget(ftok(".",'u'), 0);
    cout << "Connecting to Message Queue ID: " << messageQueueId << endl;
    buffer sentMessage;
    int numGenerated;

    //Creates exit message sent to Receiver1 by patch
    setExitMessage(messageQueueId);

    while(true){
        int size = sizeof(sentMessage) - sizeof(long) - sizeof(long);
        numGenerated = generateRandomNum();
        //Only sent a message when the random number generated is divisible by 31
        if(numGenerated < 10){
            strcpy(sentMessage.message, to_string(numGenerated).c_str());
            cout << SENDER_ID << ": " << numGenerated << endl;
            sentMessage.messageType = SENT_MESSAGE_TYPE;
            sentMessage.senderID = SENDER_ID;
            msgsnd(messageQueueId, (struct msgbuf *)&sentMessage, size, 0);
        }
    }
    exit(0);
}

/*
 Sets the termination messaget that will be sent when patch code executes
 @Param: The ID of the message queue
 @Return: none
*/
void setExitMessage(int messageQueueId){
    //creates exit message
    buffer exitMessage;
    //gets message size
    int size = sizeof(exitMessage) - sizeof(long) - sizeof(long);
    //sets "Terminated" message
    strcpy(exitMessage.message, "Terminated");
    //sets Message type	
    exitMessage.messageType = SENT_MESSAGE_TYPE;
    //sets Sender ID
    exitMessage.senderID = SENDER_ID;
    //Calls Patch code function	
    get_info(messageQueueId,(struct msgbuf *)&exitMessage, size ,SENT_MESSAGE_TYPE);

}

/*
 Generate a random number
 @Param: None
 @Return: An integer value from 0 to 2,147,483,647
 */
int generateRandomNum(){
    return rand() % MAX_RANDOM_NUM;
}

/*
 Inititalize srand to the internal with the internal clokc of the computer
 @Param: None
 @Return: None
 */
void initializeSRand(){
    srand((int) time(NULL));
}

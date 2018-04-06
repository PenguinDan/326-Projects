/*
 - File Name: Sender257
 - Sends each event to only ONE RECEIVER
 - Does not accept any acknowledgement messages
 - Terminates when ITS RECEIVER STOPS RECEIVING EVENT NOTIFICATIONS
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
#include <time.h>
#include <climits>
using namespace std;

//Define the receiver id number
const int RECERIVER_ID = 2;
//Sender ID number
const int SENDER_ID = 257;
//Defines the size of the character array to be sent
const int ARRAY_SIZE = 50;
//Define the message type
const int MESSAGE_TYPE = 2;
//Defines the termination message type in which this message could receive
const int RECEIVABLE_MESSAGE_TYPE = 10;
//Start process mtype
const int START_MESSAGE_TYPE = 500;
//Defines the message flag stating that the first message on the queue is received
const int MESSAGE_FLAG = 0;
//Define the max number that a random number generator can generate
const int MAX_RANDOM_NUM = 5000000;

//Message buffer
struct buffer{
    //Define the mtype
    long messageType;
    //Define the sender id number
    long senderID;
    //Define message size
    char message[ARRAY_SIZE];
};

//Forward declaring methods
int generateRandomNum();
void initializeSRand();
int retrieveMessageSize(buffer);

/*
 * The main starting point of the application
 */
int main(){
    //Get the message queue id
    int messageQueueId = msgget(ftok(".", 'u'), 0);
    bool receiveNotifications = true;
    int numGenerated;
    //Keep track of how many message had been send
    int numOfMessagesSent;
    initializeSRand();
    //Message that prints out the message queue ID for easy deallocation
    //from the terminal
    cout << "Connected Message Queue ID: " << messageQueueId << endl;
    //Build buffer for sent message to receiver 2
    buffer sendMessage;
    //Build buffer for received message from 997
    buffer receivedMessage;
    //Set MESSAGE_SIZE
    int MESSAGE_SIZE = retrieveMessageSize(sendMessage);
    //Wait to receieve message from 997;
    msgrcv(messageQueueId, &receivedMessage, MESSAGE_SIZE,
           START_MESSAGE_TYPE, MESSAGE_FLAG);
    //Get the number of messsage that Sender997 has send
    numOfMessagesSent = stoi(receivedMessage.message);
    //Loop until it sent 500 message
    while(numOfMessagesSent < 5000){
        numGenerated = generateRandomNum();
        //Only sent a message to receiver 2 if a random number is less than 100
        if(numGenerated < 60){
            strcpy(sendMessage.message, to_string(numGenerated).c_str());
            cout << "Sent to receiver #" << RECERIVER_ID << ": " << numGenerated << endl;
            sendMessage.messageType = MESSAGE_TYPE;
            sendMessage.senderID = SENDER_ID;
            msgsnd(messageQueueId, (struct msgbuf *)&sendMessage, MESSAGE_SIZE, MESSAGE_FLAG);
            numOfMessagesSent++;
        }
    }
    cout << "Connection disconected..." << endl;
    exit(0);
}

/*
 * Wrapper method that outputs the size of the message to be received
 *
 * @Param:
 *    buffer message: The message buffer to be received
 *
 * @Return:
 *    The size of the message to be received
 */
int retrieveMessageSize(buffer message) {
    return sizeof(message) - sizeof(long) - sizeof(long);
}

/*
 Generate a random number
 @Param: None
 @Return: An integer value from 0 to 5,000,000;
 */
int generateRandomNum(){
    return rand() % MAX_RANDOM_NUM;
}

/*
 Inititalize srand to the internal with the internal clock of the computer
 @Param: None
 @Return: None
 */
void initializeSRand(){
    srand((int) time(NULL));
}

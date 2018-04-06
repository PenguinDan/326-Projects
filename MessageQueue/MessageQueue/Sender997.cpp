/*
 - File Name: Sender997
 - Send each event to ALL RECEIVERS
 - Requires acknowledgement for each message from BOTH RECEIVERS before it continues execution
 - Terminates when it gets/observes a RANDOM NUMBER SMALLER THAN 100
 */
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

//Sender ID number
const long SENDER_ID = 997;
//Define the size of the character array to be sent
const int ARRAY_SIZE = 50;
//Define the receivable message type
const int RECEIVABLE_MESSAGE_TYPE = 3;
//Define the mtype that the sender can sent
const int MESSAGE_TYPE_FOR_RECEIVER_1 = 1;
const int MESSAGE_TYPE_FOR_RECEIVER_2 = 2;
const int M_TYPE_FOR_257 = 500;
//Defines the message flag stating that the first message on the queue is received
const int MESSAGE_FLAG = 0;
//Define the max number that a random number generator can generate
const int MAX_VALUE = 10000;

//Message buffer
struct buffer{
    //Define the mtype
    long messageType;
    //Define the sender id number
    long senderID;
    //Define messge size
    char message[ARRAY_SIZE];
};

//Forward declaring methods
int generateRandomNum();
void initializeSRand();
int retrieveMessageSize(buffer);

int main(){
    //Get the message queue id
    int messageQueueId = msgget(ftok(".",'u'), 0);
    //Build buffer for sent message to receiver 1 and 2
    buffer sentMessage1, sentMessage2;
    //Build buffer for receiving acknowledgement and termination message
    buffer receivedMessage, terminationMessage;
    cout << "Connected Message Queue ID: " << messageQueueId << endl;
    //Retrieve the size of the message
    const int MESSAGE_SIZE = retrieveMessageSize(sentMessage1);
    initializeSRand();
    int numGenerated = generateRandomNum();
    //Counts the number of messages sent
    int numberOfMessagesSent = 0;
    //Only sent a message if the random generated number is greater than 100
    while(numGenerated >= 100){

        //Initialize message for Receiver 1
        sentMessage1.messageType = 1;
        sentMessage1.senderID = SENDER_ID;
        strcpy(sentMessage1.message, to_string(numGenerated).c_str());
        cout << "Sent to receiver " << 1 << ": " << numGenerated << endl;

        //Initialize message for Receiver 2
        sentMessage2.messageType = 2;
        sentMessage2.senderID = SENDER_ID;
        strcpy(sentMessage2.message, to_string(numGenerated).c_str());
        cout << "Sent to receiver " << 2 << ": " << numGenerated << endl;
        //Sends message to receiver 1
        msgsnd(messageQueueId, (struct message *)&sentMessage1, MESSAGE_SIZE, MESSAGE_FLAG);
        //Sends message to receiver 1
        msgsnd(messageQueueId, (struct message *)&sentMessage2, MESSAGE_SIZE, MESSAGE_FLAG);
        //Increment the message sent to receiver 2
        numberOfMessagesSent++;

        //waits to receive acknowledgement messages from Receiver1 and Receiver2
        msgrcv(messageQueueId, (struct msgbuf *)&receivedMessage, MESSAGE_SIZE, RECEIVABLE_MESSAGE_TYPE, MESSAGE_FLAG);
        cout << "Acknowledgment received from : " << receivedMessage.senderID << endl;
        msgrcv(messageQueueId, (struct msgbuf *)&receivedMessage, MESSAGE_SIZE, RECEIVABLE_MESSAGE_TYPE, MESSAGE_FLAG);
        cout << "Acknowledgment received from : " << receivedMessage.senderID << endl;

        numGenerated = generateRandomNum();
    }

    //Send termination message to 257 along with variable of messages sent
    terminationMessage.senderID = SENDER_ID;
    terminationMessage.messageType = 500;
    strcpy(terminationMessage.message, to_string(numberOfMessagesSent).c_str());
    msgsnd(messageQueueId, (struct message *)&terminationMessage, MESSAGE_SIZE, MESSAGE_FLAG);

    //Send a terminate notification to receiver 1
    strcpy(sentMessage1.message, "Terminated");
    sentMessage1.senderID = SENDER_ID;
    msgsnd(messageQueueId, (struct message *)&sentMessage1, MESSAGE_SIZE, MESSAGE_FLAG);
    cout << "Connection disconnected..." << endl;
    //Exit the program
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
 @Return: An integer value from 0 to 2,147,483,647
 */
int generateRandomNum(){
    return rand() % MAX_VALUE;
}

/*
 Inititalize srand to the internal with the internal clock of the computer
 @Param: None
 @Return: None
 */
void initializeSRand(){
    srand((int) time(NULL));
}

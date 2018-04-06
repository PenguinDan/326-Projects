/*
 - File Name: Receiver2
 - Each receiver repeatedly gets a message and displays the value and the Sender’s identity
 - Receiver #1
 -    Accepts from who:
 -       Only accepts messages from 251 AND 997 SENDERS
 -    Termination cause:
 -       Terminates after BOTH OF ITS SENDERS HAVE TERMINATED
 */

//All of the necessary libraries for the program to run
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>
#include <string>
using namespace std;

//Set sender ID
const int SENDER_ID = 2;
//Constants that define the Senders in which this Receiver listens to
const int ACCEPT_FROM_1 = 257;
const int ACCEPT_FROM_2 = 997;
//Defines the size of the character array to be received
const int ARRAY_SIZE = 50;
//Defines the maximum amount of messages that this Receiver could receive
const int MAX_MESSAGES = 5000;
//Defines the message type in which this message could receive
const int RECEIVABLE_MESSAGE_TYPE = 2;
//Defines the terminate message type in which the receive will sent
const int TERMINATE_MESSAGE_TYPE = 10;
//Defines the message flag stating that the first message on the queue is received
const int MESSAGE_FLAG = 0;

//Message buffer
struct buffer {
    //Define the mtype
    long messageType;
    //Define the sender id number
    long senderId;
    //Define message size
    char message[ARRAY_SIZE];
};

//Forward declaring methods
int retrieveMessageSize(buffer);

/*
 * The main starting point of the application
 */
int main() {
    //Connect to message queue with provided information
    int messageQueueId = msgget(ftok(".",'u'), 0);
    //Message that specifies the message queue that it connected to
    cout << "Connected Message Queue ID: " << messageQueueId << endl;
    //Build buffer for message acknowledgement to sender 997
    //Sender 997 only receives messages of messageType 2
    buffer acknowledgementMessage;

    //Create a buffer object to store the received messages
    buffer receivedMessage;
    //Creata a buffer object to store sent message
    buffer sentMessage;

    const int MESSAGE_SIZE = retrieveMessageSize(receivedMessage);

    //Variable that keeps track of total received messages, once it reaches
    //4999, it will force the loop to exit and terminate the message queue
    int receivedMessagesCount = 0;

    //This loop goign to run until receiver 2 has received 5000 messages
    while(receivedMessagesCount < 5000) {
        //Waits until a message with the proper message type specified by
        //the constant RECEIVABLE_MESSAGE_TYPE is the current first message
        //in the message queue
        if(msgrcv(messageQueueId, (struct msgbuf *)&receivedMessage, MESSAGE_SIZE,
                  RECEIVABLE_MESSAGE_TYPE, MESSAGE_FLAG) != -1){
            //The message has been received, specify which sender the message
            //was received from and the message
            cout << "Message from Sender #" << receivedMessage.senderId <<
              ": " << receivedMessage.message << endl;
            //Increment the message received counter by one
            receivedMessagesCount += 1;
            //Check if the message was sent from Sender #997
            if(receivedMessage.senderId == ACCEPT_FROM_2) {
                //Send the acknowledgement message to Sender 997
                cout << "Sent acknowledgement message" << endl;
                strcpy(acknowledgementMessage.message, "Received");
                acknowledgementMessage.messageType = 3;
                acknowledgementMessage.senderId = 2;
                msgsnd(messageQueueId,(struct message *)&acknowledgementMessage,
                 MESSAGE_SIZE, 0);
            }
        }
    }
    //Send a stop notification message to sender 257
    strcpy(sentMessage.message, "Terminated");
    sentMessage.messageType = TERMINATE_MESSAGE_TYPE;
    sentMessage.senderId = SENDER_ID;
    msgsnd(messageQueueId, (struct msgbuf *)&sentMessage, MESSAGE_SIZE, MESSAGE_FLAG);
    cout << "Connection disconnected" << endl;
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

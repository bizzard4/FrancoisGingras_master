#include "PapaTask.h"
#include "BazTask.h"
#include "FooTask.h"

#include "Message.h"
#include "BarMsg.h"
#include "TextMsg.h"
#include "System.h"
#include "fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>




// this file contains code that the language compiler/runtime
// would generated automatically
#include "generated.h"






/******************************
 * Programmer Code
 *****************************/


// message tags
enum {TEXT_MSG, BAR_MSG};



/*
 * This is the "main" method for the thread
 */
static void start(PapaTask this){

	// spawn two child threads from the current thread
	unsigned int child1_TaskID = FooTask_create();
	unsigned int child2_TaskID = FooTask_create();
	unsigned int child3_TaskID = BazTask_create();



	// ********** MESSAGE 1 *****************/
	// create a text message to send to child1
	TextMsg textMsg1 = TextMsg_create(TEXT_MSG);
	textMsg1->setMsg(textMsg1, "Hello World");

	// send to the child via its TaskID
	printf("sending a Text message [%s] from Task %d to Task %d...\n", textMsg1->getMsg(textMsg1), this->taskID, child1_TaskID);
	send(this, (Message)textMsg1, child1_TaskID );


	// ********** MESSAGE 2 *****************/
	// create a bar message to send to child2
	BarMsg barMsg1 = BarMsg_create(BAR_MSG);
	barMsg1->setValue(barMsg1, 44);

	// send to the child via its TaskID
	printf("sending a Bar message [%d] from Task %d to Task %d...\n", barMsg1->getValue(barMsg1), this->taskID, child2_TaskID);
	send(this, (Message)barMsg1, child2_TaskID );



	// ********** MESSAGE 3 *****************/
	// create a message to send to child1
	BarMsg barMsg2 = BarMsg_create(BAR_MSG);
	barMsg2->setValue(barMsg2, 99);

	// send to the child via its TaskID
	printf("sending a Bar message [%d] from Task %d to Task %d...\n", barMsg2->getValue(barMsg2), this->taskID, child1_TaskID);
	send(this, (Message)barMsg2, child1_TaskID );



	// ********** MESSAGE 4 *****************/
	// create a text message to send to child2
	TextMsg textMsg2 = TextMsg_create(TEXT_MSG);
	textMsg2->setMsg(textMsg2, "Goodbye World");

	// send to the child via its TaskID
	printf("sending a Text message [%s] from Task %d to Task %d...\n", textMsg2->getMsg(textMsg2), this->taskID, child2_TaskID);
	send(this, (Message)textMsg2, child2_TaskID );



	// ********** MESSAGE 5 *****************/
	// create a message to send to child3
	BarMsg barMsg3 = BarMsg_create(BAR_MSG);
	barMsg3->setValue(barMsg3, 11);

	// send to the child via its TaskID
	printf("sending a Bar message [%d] from Task %d to Task %d...\n", barMsg3->getValue(barMsg3), this->taskID, child3_TaskID);
	send(this, (Message)barMsg3, child3_TaskID );



	// ********** MESSAGE 5 *****************/
	// create a text message to send to child3
	TextMsg textMsg3 = TextMsg_create(TEXT_MSG);
	textMsg3->setMsg(textMsg3, "me too");

	// send to the child via its TaskID
	printf("sending a Text message [%s] from Task %d to Task %d...\n", textMsg3->getMsg(textMsg3), this->taskID, child3_TaskID);
	send(this, (Message)textMsg3, child3_TaskID );




	// exit gracefully
	textMsg1->destroy(textMsg1);
	textMsg2->destroy(textMsg2);
	textMsg3->destroy(textMsg3);
	barMsg1->destroy(barMsg1);
	barMsg2->destroy(barMsg2);
	barMsg3->destroy(barMsg3);
	pthread_exit(NULL);
}



static void receive(PapaTask this, int targetPapaTaskID){

	// if we needed to receive something, it would go here.
	// see FooTask for an example

}






#include "TaskSystem/Tasks/DirectSendTask/DirectSendTask.h"
#include "TaskSystem/Tasks/DirectReceiveTask/DirectReceiveTask.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/DirectSendTask/generated.h"

/******************************
 * Programmer Code
 *****************************/

// message tags
enum {TEXT_MSG, BAR_MSG};

/*
 * This is the "main" method for the thread
 */
static void start(DirectSendTask this){

	// Spawn a direct receive task
	unsigned int receiver_id = DirectReceiveTask_create();
	printf("DirectSendTask - Start");

	// Send the message without waiting
	BarMsg barMsg = BarMsg_create(BAR_MSG);
	barMsg->setValue(barMsg, 42);

	// send to the child via its TaskID
	printf("sending a Bar message [%d] from Task %d to Task %d...\n", barMsg->getValue(barMsg), this->taskID, receiver_id);
	send(this, (Message)barMsg, receiver_id);

	barMsg->destroy(barMsg);
}



static void receive(DirectSendTask this){

	// if we needed to receive something, it would go here.
	// see FooTask for an example

}






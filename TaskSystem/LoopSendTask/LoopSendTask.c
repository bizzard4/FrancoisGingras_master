#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "LoopSendTask.h"

#include "Message.h"
#include "BarMsg.h"
#include "System.h"
#include "fatal.h"

// Task needed by this one
#include "LoopReceiveTask.h"

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
static void start(LoopSendTask this){
	unsigned int receiver_id = LoopReceiveTask_create();

	for (int i = 0; i < 10; i++) {
		// Send the message without waiting
		BarMsg barMsg = BarMsg_create(BAR_MSG);
		barMsg->setValue(barMsg, i);

		// send to the child via its TaskID
		printf("sending a Bar message [%d] from Task %d to Task %d...\n", barMsg->getValue(barMsg), this->taskID, receiver_id);
		send(this, (Message)barMsg, receiver_id);

		barMsg->destroy(barMsg);
	}
}


static void receive(LoopSendTask this){

}






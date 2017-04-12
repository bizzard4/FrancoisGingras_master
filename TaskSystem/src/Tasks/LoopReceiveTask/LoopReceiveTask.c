#include "TaskSystem/Tasks/LoopReceiveTask/LoopReceiveTask.h"
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
#include "TaskSystem/Tasks/LoopReceiveTask/generated.h"

/******************************
 * Programmer Code
 *****************************/

// message tags
enum {TEXT_MSG, BAR_MSG};

/*
 * This is the "main" method for the thread
 */
static void start(LoopReceiveTask this){
	for (int i = 0; i < 10; i++) {
		receive(this);
	}
	printf("SUCCESS : All message received\n");
}

static void receive(LoopReceiveTask this){
	int tag = Comm->getMsgTag(Comm, this->taskID );

	while (tag < 0) { // We need to pool back
		tag = Comm->getMsgTag(Comm, this->taskID );
	}

	if(tag == -1) { // Missed the message, fail-safe this should not happen
		printf("FAILURE : MESSAGE MISS\n");
		return;
	}

	BarMsg barMsg;

	// match the message to the right message "handler"
	switch (tag) {
	case BAR_MSG :
		barMsg = (BarMsg)Comm->receive(Comm, this->taskID);
		handle_BarMsg(this, barMsg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(Comm, this->taskID);
	}
}

static void handle_BarMsg(LoopReceiveTask this, BarMsg barMsg){
	printf("\nTask %d Bar message handler, value: %d\n", this->taskID, barMsg->getValue(barMsg));
}






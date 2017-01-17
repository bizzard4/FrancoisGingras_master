#include "TaskSystem/Tasks/DirectMissReceiveTask/DirectMissReceiveTask.h"
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
#include "TaskSystem/Tasks/DirectMissReceiveTask/generated.h"

/******************************
 * Programmer Code
 *****************************/

// message tags
enum {TEXT_MSG, BAR_MSG};

/*
 * This is the "main" method for the thread
 */
static void start(DirectMissReceiveTask this){
	// Directly call receive method. This call could miss the message if this task is executed before
	// the sender was able to send the message. In this case, there is no way to know if a message was lost because
	// this method is non-blocking.
	printf("Calling receive\n");
	receive(this);
}



static void receive(DirectMissReceiveTask this){
	int tag = Comm->getMsgTag( this->taskID );

	if(tag == -1) { // Missed the message
		printf("SUCCESS : MESSAGE MISS\n");
		return;
	}

	BarMsg barMsg;

	// match the message to the right message "handler"
	switch (tag) {
	case BAR_MSG :
		barMsg = (BarMsg)Comm->receive(this->taskID);
		handle_BarMsg(this, barMsg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(this->taskID);
	}
}

static void handle_BarMsg(DirectMissReceiveTask this, BarMsg barMsg){
	printf("\nTask %d Bar message handler, value: %d\n", this->taskID, barMsg->getValue(barMsg));
}






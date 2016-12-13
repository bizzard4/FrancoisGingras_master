#include "ConsumerTask.h"

#include "Message.h"
#include "BarMsg.h"
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
enum {TEXT_MSG, BAR_MSG, PRODUCE_MSG};

static void start(ConsumerTask this){

}

static void receive(ConsumerTask this){
	int tag = Comm->getMsgTag( this->taskID );

	if(tag == -1) { // Missed the message
		printf("FAILURE : MESSAGE MISS\n");
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

static void handle_BarMsg(ConsumerTask this, BarMsg barMsg){
	printf("SUCCESS : MESSAGE RECEIVED\n");
	printf("\nTask %d Bar message handler, value: %d\n", this->taskID, barMsg->getValue(barMsg));
}






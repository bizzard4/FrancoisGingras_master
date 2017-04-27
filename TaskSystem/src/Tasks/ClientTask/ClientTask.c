#include "TaskSystem/Tasks/ClientTask/ClientTask.h"

#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include "TaskSystem/Messages/BarMsg/BarMsg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>




// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/ClientTask/generated.h"






/******************************
 * Programmer Code
 *****************************/


// message tags
enum {REQUEST_MSG, RESPONSE_MSG};

int done;

/*
 * This is the "main" method for the thread
 */
static void start(ClientTask this){
	printf("Client task has been started\n");

	// Get the database task id
	// TODO

	// Response to 10 requests
	for (int i = 0; i < 100; i++) {
		// Send a request
		BarMsg res = BarMsg_create(REQUEST_MSG);
		res->setValue(res, this->taskID);
		send(this, (Message)res, 1); 
		res->destroy(res);

		// Get the response
		receive(this);
	}

	done = 1;
}



static void receive(ClientTask this){
	int tag = Comm->getMsgTag(Comm, this->taskID);
	while (tag < 0) {
		tag = Comm->getMsgTag(Comm, this->taskID);
	}

	Message msg;

	// match the message to the right message "handler"
	switch (tag) {
	case REQUEST_MSG: // Should never receive this
		break;
	case RESPONSE_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_ResponseMsg(this, (BarMsg)msg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(Comm, this->taskID);
	}
}

static void handle_ResponseMsg(ClientTask this, BarMsg barMsg) {
	printf("Client task received a response res=%d\n", barMsg->value);
}




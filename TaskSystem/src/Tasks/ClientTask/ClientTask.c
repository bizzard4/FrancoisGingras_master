#include "TaskSystem/Tasks/ClientTask/ClientTask.h"

#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include "TaskSystem/Messages/RequestMsg/RequestMsg.h"
#include "TaskSystem/Messages/ResponseMsg/ResponseMsg.h"

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

	// Do 10 request
	for (int i = 0; i < 50; i++) {
		// Send a request
		RequestMsg req = RequestMsg_create(REQUEST_MSG);
		req->sender_task_id = this->taskID;
		req->request_type = SELECT_REQUEST;
		send(this, (Message)req, 1); 
		req->destroy(req);

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
		handle_ResponseMsg(this, (ResponseMsg)msg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(Comm, this->taskID);
	}
}

static void handle_ResponseMsg(ClientTask this, ResponseMsg responseMsg) {
	printf("Client task received a response code=%d\n", responseMsg->code);
}




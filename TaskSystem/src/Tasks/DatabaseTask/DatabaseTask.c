#include "TaskSystem/Tasks/DatabaseTask/DatabaseTask.h"

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
#include "TaskSystem/Tasks/DatabaseTask/generated.h"






/******************************
 * Programmer Code
 *****************************/


// message tags
enum {REQUEST_MSG, RESPONSE_MSG};

int done;

/*
 * This is the "main" method for the thread
 */
static void start(DatabaseTask this){
	printf("Database task has been started\n");

	// Response to 10 requests
	//for (int i = 0; i < 100; i++) {
	while (1) {
		// Get a request
		receive(this);

		// Response to the request (int for now)
		ResponseMsg res = ResponseMsg_create(RESPONSE_MSG);
		res->code = 200;
		send(this, (Message)res, this->current_requester_task_id); 
		res->destroy(res);
	}

	sleep(1); // Safety, if the main process close before the last message is read from 
	// the client, then we enter in a infinite loop
	done = 1;
}



static void receive(DatabaseTask this){
	int tag = Comm->getMsgTag(Comm, this->taskID);
	while (tag < 0) {
		tag = Comm->getMsgTag(Comm, this->taskID);
	}

	Message msg;

	// match the message to the right message "handler"
	switch (tag) {
	case REQUEST_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_RequestMsg(this, (RequestMsg)msg);
		break;
	case RESPONSE_MSG: // Should never receive this
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(Comm, this->taskID);
	}
}

static void handle_RequestMsg(DatabaseTask this, RequestMsg requestMsg) {
	//printf("Database task received a request (val=%d) \n", requestMsg->sender_task_id);
	this->current_requester_task_id = requestMsg->sender_task_id;
}




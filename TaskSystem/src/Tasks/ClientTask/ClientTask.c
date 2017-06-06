#include "TaskSystem/Tasks/ClientTask/ClientTask.h"

#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include "TaskSystem/Messages/RequestMsg/RequestMsg.h"
#include "TaskSystem/Messages/ResponseMsg/ResponseMsg.h"

#include "TaskSystem/TimeHelper.h"

// Business logic include
#include "TaskSystem/Tasks/ClientTask/SelectInfo.h"
#include "TaskSystem/Tasks/DatabaseTask/StudentInfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>




// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/ClientTask/generated.h"

// Print query request
#define VERBOSE




/******************************
 * Programmer Code
 *****************************/


// message tags
enum {REQUEST_MSG, RESPONSE_MSG};

// Global variable for testing purpose
int done;
int nb_request;

/*
 * This is the "main" method for the thread
 */
static void start(ClientTask this){
	printf("Client task has been started\n");

	// Get the database task id
	// TODO
	int database_id = 1;

	// Do 10 request
	struct timespec total_start, total_end;
	clock_gettime(CLOCK_MONOTONIC, &total_start);
	int count = 0;
	for (int i = 0; i < nb_request; i++) {
		count++;

		// Send a request
		RequestMsg req = RequestMsg_create(REQUEST_MSG);
		req->sender_task_id = this->taskID;

		int size = 0;

		// Select request
		struct SelectInfo req_data;
		req_data.id = (long)i;
		size = sizeof(struct SelectInfo);
		req->request_type = SELECT_REQUEST;

		// insert request
		//struct StudentInfo req_data;
		//req_data.id = (long)i;
		//strncpy(&req_data.name, "Francois Gingras", 30);
		//req_data.gpa = 4.3f;
		//size = sizeof(struct StudentInfo);
		//req->request_type = INSERT_REQUEST;

		// delete request
		//struct SelectInfo req_data;
		//req_data.id = (long)i;
		//size = sizeof(struct SelectInfo);
		//req->request_type = DELETE_REQUEST;


		unsigned char buffer[size];
		memcpy(buffer, &req_data, size);
#ifdef VERBOSE
		printf("Byte send : ");
		for (int j = 0; j < size; j++) {
			printf("%04x ", buffer[j]);
		}
		printf("\n");
#endif
		req->setData(req, buffer, size);
		send(this, (Message)req, database_id); 
		req->destroy(req);
		message_wait(this);

		// Get the response
		receive(this);
	}
	clock_gettime(CLOCK_MONOTONIC, &total_end);

	// Time stats
	struct timespec total_diff = diff(total_start, total_end);
	printf("TOTAL-TIME-TEST %lds,%ldms - Read time\n", total_diff.tv_sec, total_diff.tv_nsec/1000000);


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
		printf("SHOULD NOT RECEIVE REQUEST\n");
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
	//printf("Client task received a response code=%d\n", responseMsg->code);
}




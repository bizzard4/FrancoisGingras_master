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
int req_type;

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
		unsigned char* buffer;

		struct SelectInfo sel_data;
		struct StudentInfo student_data;

		switch (req_type) {
		case 1:
			// Select request
			sel_data.id = (long)i;
			size = sizeof(struct SelectInfo);
			buffer = malloc(size * sizeof(unsigned char));
			req->request_type = SELECT_REQUEST;
			memcpy(buffer, &sel_data, size);
			break;
		case 2:
			// insert request
			student_data.id = (long)i;
			strncpy(&student_data.name, "Francois Gingras", 30);
			student_data.gpa = 4.3f;
			size = sizeof(struct StudentInfo);
			buffer = malloc(size * sizeof(unsigned char));
			req->request_type = INSERT_REQUEST;
			memcpy(buffer, &student_data, size);
			break;
		case 3:
			// delete request
			sel_data.id = (long)i;
			size = sizeof(struct SelectInfo);
			buffer = malloc(size * sizeof(unsigned char));
			req->request_type = DELETE_REQUEST;
			memcpy(buffer, &sel_data, size);
			break;
		default:
			printf("Error in request type %d\n", req_type);
			exit(-1);
		}

#ifdef VERBOSE
		printf("Byte send : ");
		for (int j = 0; j < size; j++) {
			printf("%04x ", buffer[j]);
		}
		printf("\n");
#endif
		req->setData(req, buffer, size);
		send(this, (Message)req, database_id); 
		free(buffer);
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
#ifdef VERBOSE
	printf("Client task received a response code=%d data_size=%d\n", responseMsg->code, responseMsg->data_size);
	if (responseMsg->data_size > 0) {
		struct StudentInfo res_data;
		memcpy(&res_data, responseMsg->getData(responseMsg), sizeof(struct StudentInfo));
		printf("Student id=%ld name=%s gpa=%f\n", res_data.id, res_data.name, res_data.gpa);
	}
#endif
}




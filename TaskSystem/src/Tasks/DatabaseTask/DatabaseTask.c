#include "TaskSystem/Tasks/DatabaseTask/DatabaseTask.h"

#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Business logic include
#include "TaskSystem/Tasks/ClientTask/SelectInfo.h"

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/DatabaseTask/generated.h"



// Print query and result
#define VERBOSE



/******************************
 * Programmer Code
 *****************************/


// message tags
enum {REQUEST_MSG, RESPONSE_MSG};

int done;
int max_request; 

void setResponse(DatabaseTask this, int code, char* data, int data_size) {
	this->response_code = code;
	this->response_data = data;
	this->response_size = data_size;
}

/*
 * This is the "main" method for the thread
 */
static void start(DatabaseTask this){
	printf("Database task has been started\n");

	// Response to 10 requests
	//for (int i = 0; i < 100; i++) {
	int count = 0;
	while ((count < max_request) || (max_request == -1)) {

		count++;

		// Get a request
		receive(this);

		// Response to the request (int for now)
		ResponseMsg res = ResponseMsg_create(RESPONSE_MSG);
		res->code = this->response_code;
		if (this->response_size > 0) {
			res->setData(res, this->response_data, this->response_size);
		}
		send(this, (Message)res, this->current_requester_task_id); 
		res->destroy(res);
	}

#ifdef VERBOSE
	printf("Student list : \n");
	for (int i = 0; i < this->student_count; i++) {
		printf("Student %d id=%ld name=%s gpa=%f\n", i, this->students[i].id, this->students[i].name, this->students[i].gpa);
	}
#endif

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
#ifdef VERBOSE
	printf("Database task received a request sender=%d req_type=%d data_size=%d \n", requestMsg->sender_task_id, requestMsg->request_type, requestMsg->data_size);
	printf("Byte recv : ");
	for (int j = 0; j < requestMsg->data_size; j++) {
		printf("%04x ", requestMsg->data[j]);
	}
	printf("\n");
#endif

	this->current_requester_task_id = requestMsg->sender_task_id;

	struct SelectInfo sel_info;
	struct StudentInfo insert_info;
	struct SelectInfo del_info;

	int found;

	switch (requestMsg->request_type) {

	case SELECT_REQUEST:
		memcpy(&sel_info, requestMsg->getData(requestMsg), sizeof(struct SelectInfo));
#ifdef VERBOSE
		printf("Select query student_id=%ld\n", sel_info.id);
#endif

		// Find the record
		found = -1;
		for (int i = 0; i < this->student_count; i++) {
			if (this->students[i].id == sel_info.id) {
				found = i;
				break;
			}
		}

		// Set the response
		if (found >= 0) {
			setResponse(this, RESPONSE_OK, (char*)&this->students[found], sizeof(struct StudentInfo));
		} else {
			setResponse(this, RESPONSE_ERROR, NULL, 0);
		}
		break;

	case INSERT_REQUEST:
		memcpy(&insert_info, requestMsg->getData(requestMsg), sizeof(struct StudentInfo));
#ifdef VERBOSE
		printf("Insert query id=%ld name=%s gpa=%f\n", insert_info.id, insert_info.name, insert_info.gpa);
#endif
		// Put the new student in the buffer
		if (this->student_count < MAX_STUDENT) {
			this->students[this->student_count].id = insert_info.id;
			strncpy(this->students[this->student_count].name, insert_info.name, 30);
			this->students[this->student_count].gpa = insert_info.gpa;
			this->student_count++;

			// Set future response
			setResponse(this, RESPONSE_OK, NULL, 0);
		} else {
			setResponse(this, RESPONSE_ERROR, NULL, 0);
		}

		break;

	case DELETE_REQUEST:
		memcpy(&del_info, requestMsg->getData(requestMsg), sizeof(struct StudentInfo));
#ifdef VERBOSE
		printf("Delete query student_id=%ld\n", del_info.id);
#endif

		// Find the record and delete it
		found = -1;
		for (int i = 0; i < this->student_count; i++) {
			if (this->students[i].id == del_info.id) {
				this->students[i].id = -1;
				strncpy(this->students[i].name, "", 30);
				this->students[i].gpa = 0.0f;

				found = 1;
				break;
			}
		}

		if (found > 0) {
			setResponse(this, RESPONSE_OK, NULL, 0);
		} else {
			setResponse(this, RESPONSE_ERROR, NULL, 0);
		}
		break;
	default:
		printf("Database error, invalid request type %d\n", requestMsg->request_type);
	}
}




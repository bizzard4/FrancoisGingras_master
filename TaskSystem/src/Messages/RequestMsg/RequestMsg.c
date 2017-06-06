#include "TaskSystem/Messages/RequestMsg/RequestMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Messages/RequestMsg/generated.h"





/******************************
 * Programmer Code
 *****************************/


static int getTag(RequestMsg this){
	return this->tag;
}

static RequestMsg clone(RequestMsg this){

	RequestMsg tmp = RequestMsg_create(0);
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	tmp->sender_task_id = this->sender_task_id;
	tmp->request_type = this->request_type;
	tmp->data_size = this->data_size;
	// Data deep copy
	tmp->data = malloc(sizeof(char) * this->data_size);
	for (int i = 0; i < this->data_size; i++) {
		tmp->data[i] = this->data[i];
	}

	return tmp;
}

static void destroy(RequestMsg this){
	free(this->data);
	free(this);
}

static int writeAt(RequestMsg this, void* addr) {
	RequestMsg tmp = (RequestMsg)addr;
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	tmp->sender_task_id = this->sender_task_id;
	tmp->request_type = this->request_type;
	tmp->data_size = this->data_size;
	tmp->data = (char*)memcpy((void*)((long)addr + sizeof(struct RequestMsg)), this->data, this->data_size * sizeof(char));

	return this->msg_size;
}


static char* getData(RequestMsg this){
	return this->data;
}


static void setData(RequestMsg this, char* data, int size) {
	// Data deep copy
	this->data = malloc(sizeof(char) * size);
	for (int i = 0; i < size; i++) {
		this->data[i] = data[i];
	}
	this->data_size = size;
	this->msg_size += sizeof(char) * size;
}



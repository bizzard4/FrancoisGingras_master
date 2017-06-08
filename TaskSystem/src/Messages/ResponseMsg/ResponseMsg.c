#include "TaskSystem/Messages/ResponseMsg/ResponseMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Messages/ResponseMsg/generated.h"





/******************************
 * Programmer Code
 *****************************/


static int getTag(ResponseMsg this){
	return this->tag;
}

static ResponseMsg clone(ResponseMsg this){

	ResponseMsg tmp = ResponseMsg_create(0);
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	tmp->code = this->code;
	tmp->data_size = this->data_size;
	// Data deep copy
	tmp->data = malloc(sizeof(char) * this->data_size);
	for (int i = 0; i < this->data_size; i++) {
		tmp->data[i] = this->data[i];
	}

	return tmp;
}

static void destroy(ResponseMsg this){
	free(this->data);
	free(this);
}

static int writeAt(ResponseMsg this, void* addr) {
	ResponseMsg tmp = (ResponseMsg)addr;
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	tmp->code = this->code;
	tmp->data_size = this->data_size;
	tmp->data = (char*)memcpy((void*)((long)addr + sizeof(struct ResponseMsg)), this->data, this->data_size * sizeof(char));

	return this->msg_size;
}


static char* getData(ResponseMsg this){
	return this->data;
}


static void setData(ResponseMsg this, char* data, int size) {
	// Data deep copy
	this->data = malloc(sizeof(char) * size);
	for (int i = 0; i < size; i++) {
		this->data[i] = data[i];
	}
	this->data_size = size;
	this->msg_size += sizeof(char) * size;
}



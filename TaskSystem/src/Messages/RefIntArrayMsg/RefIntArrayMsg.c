#include "TaskSystem/Messages/RefIntArrayMsg/RefIntArrayMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TaskSystem/Messages/RefIntArrayMsg/generated.h"

/******************************
 * Programmer Code
 *****************************/

// NOTE : This message work do not work in IPC, values pointer will be invalid

static int getTag(RefIntArrayMsg this){
	return this->tag;
}

static RefIntArrayMsg clone(RefIntArrayMsg this){
	RefIntArrayMsg tmp = RefIntArrayMsg_create(this->tag);
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	tmp->size = this->size;
	tmp->values = this->values;

	return tmp;
}

// Who will be responsible to destroy the final array? Main creator?
static void destroy(RefIntArrayMsg this){
	free(this);
}

static int writeAt(RefIntArrayMsg this, void* addr) {
	RefIntArrayMsg tmp = (RefIntArrayMsg)addr;
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	// Ref int array msg members

	tmp->size = this->size;
	tmp->values = this->values;

	return this->msg_size;
}

static void setValues(RefIntArrayMsg this, int count, int* pointer_to_values) {
	this->size = count;
	this->values = pointer_to_values;
}

static int getSize(RefIntArrayMsg this) {
	return this->size;
}

static int getValue(RefIntArrayMsg this, int index) {
	return this->values[index];
}



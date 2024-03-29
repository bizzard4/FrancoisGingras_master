#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TaskSystem/Messages/IntArrayMsg/generated.h"

/******************************
 * Programmer Code
 *****************************/

static int getTag(IntArrayMsg this){
	return this->tag;
}

static IntArrayMsg clone(IntArrayMsg this){
	IntArrayMsg tmp = IntArrayMsg_create(this->tag);
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	// Start of int array msg

	tmp->size = this->size;
	tmp->values = malloc(this->size * sizeof(int)); // Deep copy
	for (int i = 0; i < this->size; i++) {
		tmp->values[i] = this->values[i];
	}

	return tmp;
}

static void destroy(IntArrayMsg this){
	free(this->values);
	free(this);
}

static int writeAt(IntArrayMsg this, void* addr) {
	IntArrayMsg tmp = (IntArrayMsg)addr;
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	// Start of int array msg
	tmp->size = this->size;
	tmp->values = (int*)memcpy((void*)((long)addr + sizeof(struct IntArrayMsg)), this->values, this->size * sizeof(int));
	
	return this->msg_size;
}

static void setValues(IntArrayMsg this, int count, int val[]) {
	this->size = count;
	this->values = malloc(this->size * sizeof(int));
	this->msg_size = sizeof(struct IntArrayMsg) + (count * sizeof(int));
	for (int i = 0; i < count; i++) {
		this->values[i] = val[i];
	}
}

static int getSize(IntArrayMsg this) {
	return this->size;
}

static int getValue(IntArrayMsg this, int index) {
	return this->values[index];
}



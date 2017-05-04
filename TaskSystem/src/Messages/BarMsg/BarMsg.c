#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Messages/BarMsg/generated.h"





/******************************
 * Programmer Code
 *****************************/


static int getTag(BarMsg this){
	return this->tag;
}

static BarMsg clone(BarMsg this){

	BarMsg tmp = BarMsg_create(0);
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	tmp->value = this->value;

	return tmp;
}

static void destroy(BarMsg this){
	free(this);
}

static int writeAt(BarMsg this, void* addr) {
	BarMsg tmp = (BarMsg)addr;
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	tmp->value = this->value;

	return this->msg_size;
}


static int getValue(BarMsg this){
	return this->value;
}


static void setValue(BarMsg this, int value){
	this->value = value;
}



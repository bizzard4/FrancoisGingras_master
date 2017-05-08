#include "TaskSystem/Messages/DoneMsg/DoneMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TaskSystem/Messages/DoneMsg/generated.h"

/******************************
 * Programmer Code
 *****************************/

static int getTag(DoneMsg this){
	return this->tag;
}

static DoneMsg clone(DoneMsg this){
	DoneMsg tmp = DoneMsg_create(this->tag);
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	tmp->success = this->success;

	return tmp;
}

static void destroy(DoneMsg this){
	free(this);
}

static int writeAt(DoneMsg this, void* addr) {
	DoneMsg tmp = (DoneMsg)addr;
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	// Start of done msg

	tmp->success = this->success;

	return this->msg_size;
}



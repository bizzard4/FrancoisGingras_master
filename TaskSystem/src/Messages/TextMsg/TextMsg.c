#include "TaskSystem/Messages/TextMsg/TextMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Messages/TextMsg/generated.h"





/******************************
 * Programmer Code
 *****************************/

static int getTag(TextMsg this){
	return this->tag;
}


static TextMsg clone(TextMsg this){

	TextMsg tmp = TextMsg_create(0);
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;
	strcpy(tmp->msg, this->msg);

	return tmp;
}

static void destroy(TextMsg this){
	free(this);
}

static int writeAt(TextMsg this, void* addr) {
	TextMsg tmp = (TextMsg)addr;
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	strcpy(tmp->msg, this->msg);

	return sizeof(struct TextMsg);
}


static char* getMsg(TextMsg this){
	return this->msg;
}


static void setMsg(TextMsg this, char* msg){
	strcpy(this->msg, msg);
}



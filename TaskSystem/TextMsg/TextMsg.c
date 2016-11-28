#include "TextMsg.h"

#include "fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// this file contains code that the language compiler/runtime
// would generated automatically
#include "generated.h"





/******************************
 * Programmer Code
 *****************************/

static int getTag(TextMsg this){
	return this->tag;
}


static TextMsg clone(TextMsg this){

	TextMsg tmp = TextMsg_create(0);
	tmp->tag = this->tag;
	if(strdup(this->msg) != NULL)
		tmp->msg = strdup(this->msg); // we MUST copy the string!

	return tmp;
}

static void destroy(TextMsg this){
	free(this->msg);
	free(this);
}


static char* getMsg(TextMsg this){
	return strdup(this->msg); // we MUST return a copy of the string!
}


static void setMsg(TextMsg this, char* msg){
	this->msg = strdup(msg); // we MUST create a copy of the string!
}



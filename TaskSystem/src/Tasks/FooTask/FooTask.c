#include "TaskSystem/Tasks/FooTask/FooTask.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/Messages/TextMsg/TextMsg.h"
#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/FooTask/generated.h"




/******************************
 * Programmer Code
 *****************************/


// programmer-defined message tags
enum {TEXT_MSG, BAR_MSG};


/*
 * This is the "main" method for the thread
 */
static void start(FooTask this){

	// ..do something

	// Check for first message
	receive(this);

	//... do something else

	// check for second message
	receive(this);

}



/*
 *  Programmer-defined list of valid message objects for this Task.
 *  Logically, this is similar to the style of the message "matching" in erlang.
 */
static void receive(FooTask this){

	// find out which message is next in the queue
	int tag = Comm->getMsgTag(Comm, this->taskID);

	if(tag == -1) // no messages in Q
		return;

	TextMsg textMsg;
	BarMsg barMsg;

	// match the message to the right message "handler"
	switch (tag) {

	case TEXT_MSG :
		textMsg = (TextMsg)Comm->receive(Comm, this->taskID);
		handle_TextMsg(this, textMsg);
		break;
	case BAR_MSG :
		barMsg = (BarMsg)Comm->receive(Comm, this->taskID);
		handle_BarMsg(this, barMsg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(Comm, this->taskID);
	}
}




/*
 * The actual receive functionality
 */

static void handle_TextMsg(FooTask this, TextMsg textMsg){
	printf("\nTask %d Text message handler: %s\n", this->taskID, textMsg->getMsg(textMsg));
}


static void handle_BarMsg(FooTask this, BarMsg barMsg){
	printf("\nTask %d Bar message handler, value: %d\n", this->taskID, barMsg->getValue(barMsg));
}





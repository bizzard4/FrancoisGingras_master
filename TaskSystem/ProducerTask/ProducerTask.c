#include "ProducerTask.h"

#include "Message.h"
#include "BarMsg.h"
#include "ProduceMsg.h"
#include "System.h"
#include "fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "generated.h"

/******************************
 * Programmer Code
 *****************************/

// message tags
enum {TEXT_MSG, BAR_MSG, PRODUCE_MSG};

// Task data
int consumer_id;
int next_to_produce;

static void start(ProducerTask this){
	consumer_id = -1; // For error handling

	// Get consumer id that we should receive first
	receive(this);
	if (consumer_id==-1) {
		printf("FAILURE : Consumer id = -1\n");
	}

	// Start to produce
	next_to_produce = 0;

	ProduceMsg nextProduce = ProduceMsg_create(PRODUCE_MSG);
	nextProduce->setProduceId(nextProduce, this->taskID);
	nextProduce->setProduceValue(nextProduce, next_to_produce);
	printf("Before send\n");
	Comm->send((Message)nextProduce, consumer_id);
	//send(this, (Message)nextProduce, consumer_id);
	printf("After send\n");
	nextProduce->destroy(nextProduce);
	next_to_produce++;
}

// This reciever will loop until the next message is received
static void receive(ProducerTask this){
	int tag = Comm->getMsgTag(this->taskID);
	while (tag < 0) {
		tag = Comm->getMsgTag(this->taskID);
	}

	BarMsg barMsg;

	// match the message to the right message "handler"
	switch (tag) {
	case BAR_MSG :
		barMsg = (BarMsg)Comm->receive(this->taskID);
		handle_BarMsg(this, barMsg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(this->taskID);
	}
}

static void handle_BarMsg(ProducerTask this, BarMsg barMsg) {
	printf("Producer id %d received consumer id : %d\n", this->taskID, barMsg->getValue(barMsg));
	consumer_id = barMsg->getValue(barMsg);
}





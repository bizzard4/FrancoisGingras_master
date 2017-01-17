#include "TaskSystem/Tasks/ConsumerTask/ConsumerTask.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/ProduceMsg/ProduceMsg.h"
#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/ConsumerTask/generated.h"

/******************************
 * Programmer Code
 *****************************/

#define BUFFER_SIZE 10
#define TOTAL_CONSUME 1000000

// message tags
enum {TEXT_MSG, BAR_MSG, PRODUCE_MSG};

// Goods
int goods[BUFFER_SIZE];
int itemCount;
int consumed_item;

static void start(ConsumerTask this) {

	// Initialize buffer to 0
	itemCount = 0;
	consumed_item = 0;
	for (int i = 0; i < BUFFER_SIZE; i++) {
		goods[i] = 0;
	}

	// We start to listen for incoming produce message
	while (consumed_item < TOTAL_CONSUME) { // We will consume 10 msg
		receive(this); // May read new product (this is non-blocking)

		// Now consume
		if (itemCount > 0) { // Consume only when item are available
			itemCount--;
			int consumed = goods[itemCount];
			printf("Consuming item %d at position %d\n", consumed, itemCount);
			goods[itemCount] = 0;
			consumed_item++;
		}
	}
}

static void receive(ConsumerTask this) {
	// Check if buffer is already full
	if (itemCount == BUFFER_SIZE) {
		return;
	}

	int tag = Comm->getMsgTag( this->taskID );

	if(tag == -1) { // No good to read
		return;
	}

	ProduceMsg new_good;

	switch (tag) {
	case PRODUCE_MSG:
		new_good = (ProduceMsg)Comm->receive(this->taskID);
		handle_ProduceMsg(this, new_good);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(this->taskID);
	}
}

static void handle_ProduceMsg(ConsumerTask this, ProduceMsg produceMsg) {
	printf("Storing new product (%d) at position %d from producer id=%d\n", produceMsg->getProduceValue(produceMsg), itemCount, produceMsg->getProduceId(produceMsg));
	goods[itemCount] = produceMsg->getProduceValue(produceMsg);
	itemCount++;
}





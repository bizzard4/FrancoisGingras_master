#include "TaskSystem/Tasks/BucketTask/BucketTask.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/TopologyMsg/TopologyMsg.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h" 
#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/BucketTask/generated.h"

#define K 2

// Messages
enum {TOPOLOGY_MSG, INTARRAY_MSG};

static void start(BucketTask this) {
	// Get topology
	receive(this);

	// Get sample data
	receive(this);

	// Pick sample and send them to root
	//int step = this->sample_size / K; // TODO : NEED K IN TOPOLOGY
	//int samples[100]; // TODO : Need to be dynamic
	//int count = 0;
	//for (int i = 0; i < this->sample_size; i += step) {
	//	samples[count] = this->sample_values[i];
	//	count++;
	//}
	//IntArrayMsg sample_msg = IntArrayMsg_create(INTARRAY_MSG);
	//ample_msg->setValues(sample_msg, count, samples);
	//send(this, (Message)sample_msg, this->root_id);

	// Get splitters
	// receive(this);

	// Propagate data

	// Send "done" to root

	// TODO : Delete topology array
	// TODO : Delete sample data
	// TODO : Delete final result
}

static void receive(BucketTask this) {
	int tag = Comm->getMsgTag(this->taskID);
	while (tag < 0) {
		tag = Comm->getMsgTag(this->taskID);
	}

	Message msg;

	printf("Bucket task %d received tag=%d\n", this->taskID, tag);

	// match the message to the right message "handler"
	switch (tag) {
	case TOPOLOGY_MSG:
		msg = Comm->receive(this->taskID);
		handle_TopologyMsg(this, (TopologyMsg)msg);
		break;
	case INTARRAY_MSG:
		msg = Comm->receive(this->taskID);
		handle_IntArrayMsg(this, (IntArrayMsg)msg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(this->taskID);
	}
}

static void handle_TopologyMsg(BucketTask this, TopologyMsg topologyMsg) {
	printf("Bucket task %d received topology\n", this->taskID);
	this->bucket_count = topologyMsg->bucket_count;
	this->bucket_ids = malloc(topologyMsg->bucket_count * sizeof(unsigned int));
	for (int i = 0; i < topologyMsg->bucket_count; i++) {
		this->bucket_ids[i] = topologyMsg->bucket_ids[i];
	}
	this->root_id = topologyMsg->root_id;
	// TODO : Add to topology message
	this->sample_size = 5; // SN
	this->data_size = 10; // N
}

static void handle_IntArrayMsg(BucketTask this, IntArrayMsg intarrayMsg) {
	printf("Bucket task %d received sample data\n", this->taskID);

	this->sample_count = intarrayMsg->getSize(intarrayMsg);
	this->sample_values = malloc(intarrayMsg->getSize(intarrayMsg) * sizeof(int));
	for (int i = 0; i < intarrayMsg->getSize(intarrayMsg); i++) {
		this->sample_values[i] = intarrayMsg->getValue(intarrayMsg, i);
	}
}
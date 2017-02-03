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

// Messages
enum {TOPOLOGY_MSG, INTARRAY_MSG};

static void start(BucketTask this) {
	// Get topology
	receive(this);

	// Get sample data
	receive(this);

	// Return sample

	// Get splitters

	// Propagate data

	// Send "done" to root
}

static void receive(BucketTask this) {
	int tag = Comm->getMsgTag(this->taskID);
	while (tag < 0) {
		tag = Comm->getMsgTag(this->taskID);
	}

	Message msg;

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
}

static void handle_IntArrayMsg(BucketTask this, IntArrayMsg intarrayMsg) {
	printf("Bucket task %d received sample data\n", this->taskID);
}

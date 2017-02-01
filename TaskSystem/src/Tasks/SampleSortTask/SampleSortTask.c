#include "TaskSystem/Tasks/SampleSortTask/SampleSortTask.h"
#include "TaskSystem/Tasks/BucketTask/BucketTask.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/TopologyMsg/TopologyMsg.h"
#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/SampleSortTask/generated.h"

int done; // For the test case, without any good way to "wait" on a task to be done, we will use that.

#define K 8 // TODO : Make this generic

// Messages
enum {TOPOLOGY_MSG};

static void start(SampleSortTask this) {

	// Create K bucket task
	unsigned int buckets[K];

	for(int i = 0; i < K; i++) {
		buckets[i] = BucketTask_create();
	}
	printf("Bucket tasks created\n");

	// Send topology to all K bucket task
	TopologyMsg topo_msg = TopologyMsg_create();
	topo_msg->setRootId(topo_msg, this->taskID);
	topo_msg->setBucketIds(topo_msg, K, buckets);
	for (int i = 0; i < K; i++) {
		send(this, (Message)topo_msg, buckets[i]);
	}
	topo_msg->destroy(topo_msg);
	printf("Topology send\n");

	// Send sample data to all K bucket task

	// Wait on K samples from bucket task

	// Compute and send splitters information

	// Wait on K done signal

	// TODO : This part, depending on how the data is store may be needed
	// Gather result

	// Send done signal to destroy task

	// To unlock test case
	sleep(3);
	done = 1;
}

static void receive(SampleSortTask this) {

}

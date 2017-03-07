#include "TaskSystem/Tasks/SampleSortTask/SampleSortTask.h"
#include "TaskSystem/Tasks/BucketTask/BucketTask.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/TopologyMsg/TopologyMsg.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h"
#include "TaskSystem/Messages/RefIntArrayMsg/RefIntArrayMsg.h"
#include "TaskSystem/Messages/DoneMsg/DoneMsg.h"
#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"
#include "TaskSystem/TimeHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/SampleSortTask/generated.h"

int done; // For the test case, without any good way to "wait" on a task to be done, we will use that.

// Messages (all)
enum {BAR_MSG, DONE_MSG, INTARRAY_MSG, REF_INTARRAY_MSG, TOPOLOGY_MSG};

#define LARGE_DATA

int samplesorttask_cmpfunc(const void* a, const void* b)
{
   return (*(int*)a - *(int*)b);
}

// Assumes 0 <= max <= RAND_MAX
// Returns in the closed interval [0, max]
// Reference : http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-from-within-a-range
long random_at_most(long max) {
  unsigned long
    // max <= RAND_MAX < ULONG_MAX, so this is okay.
    num_bins = (unsigned long) max + 1,
    num_rand = (unsigned long) RAND_MAX + 1,
    bin_size = num_rand / num_bins,
    defect   = num_rand % num_bins;

  long x;
  do {
   x = random();
  }
  // This is carefully written not to overflow
  while (num_rand - defect <= (unsigned long)x);

  // Truncated division is intentional
  return x/bin_size;
}

static void start(SampleSortTask this) {
	// Get the number of buckets
	struct timespec initialization_start, initialization_end;
	clock_gettime(CLOCK_MONOTONIC, &initialization_start);
	receive(this);

	// Get the data to sort
	receive(this);
	int size_per_task = this->size / this->K;
	printf("Size per task = %d (N=%d, K=%d) \n", size_per_task, this->size, this->K);
	clock_gettime(CLOCK_MONOTONIC, &initialization_end);

	// Gather random sample and prepare splitters
	struct timespec random_start, random_end;
	clock_gettime(CLOCK_MONOTONIC, &random_start);
	int splitters[this->K];
	printf("Splitter : ");
	srand(time(NULL));
	for (int i = 0; i < (this->K); i++) {
		long val = random_at_most(this->size-1);
		splitters[i] = this->data[val];
		printf("(i=%ld,v=%d) ", val, splitters[i]);
	}
	printf("\n");
	qsort(splitters, this->K, sizeof(int), samplesorttask_cmpfunc);
	clock_gettime(CLOCK_MONOTONIC, &random_end);

	// Create K bucket task
	struct timespec bucket_spawn_start, bucket_spawn_end;
	clock_gettime(CLOCK_MONOTONIC, &bucket_spawn_start);
	unsigned int buckets[this->K];
	for(int i = 0; i < this->K; i++) {
		buckets[i] = BucketTask_create();
	}
	printf("Bucket tasks created\n");

	// Send topology to all K bucket task
	TopologyMsg topo_msg = TopologyMsg_create(TOPOLOGY_MSG);
	topo_msg->setRootId(topo_msg, this->taskID);
	topo_msg->setBucketIds(topo_msg, this->K, buckets);
	topo_msg->sample_size = size_per_task;
	topo_msg->data_size = this->size;
	for (int i = 0; i < this->K; i++) {
		send(this, (Message)topo_msg, buckets[i]);
	}
	topo_msg->destroy(topo_msg);
	printf("Topology send\n");

	// Send ref of data to K buckets
	int current = 0;
	for (int ki = 0; ki < this->K; ki++) {
		RefIntArrayMsg data_msg = RefIntArrayMsg_create(REF_INTARRAY_MSG);
		if (ki < (this->K-1)) {
			data_msg->setValues(data_msg, size_per_task, &this->data[current]);
		} else { // Last one have the rest
			data_msg->setValues(data_msg, this->size-current, &this->data[current]);
		}
		send(this, (Message)data_msg, buckets[ki]);
		data_msg->destroy(data_msg);	
		current += size_per_task;
	}

	// Send splitters to K buckets
	IntArrayMsg splitters_msg = IntArrayMsg_create(INTARRAY_MSG);
	splitters_msg->setValues(splitters_msg, this->K, splitters);
	for (int ki = 0; ki < this->K; ki++) {
		send(this, (Message)splitters_msg, buckets[ki]);
	}
	splitters_msg->destroy(splitters_msg);
	clock_gettime(CLOCK_MONOTONIC, &bucket_spawn_end);

	// Wait on K done messages, this signal that a bucket is done
	struct timespec wait_bucket_start, wait_bucket_end;
	clock_gettime(CLOCK_MONOTONIC, &wait_bucket_start);
	for (int ki = 0; ki < this->K; ki++) {
		receive(this);
	}
	printf("Received all bucket done messaage\n");
	clock_gettime(CLOCK_MONOTONIC, &wait_bucket_end);

	// Printing timiing
	printf("Samplesort completed\n");

	struct timespec initialization_diff = diff(initialization_start, initialization_end);
	printf("SAMPLESORT TASK : Initialization time %lds, %ldms\n", initialization_diff.tv_sec, initialization_diff.tv_nsec/1000000);

	struct timespec random_diff = diff(random_start, random_end);
	printf("SAMPLESORT TASK : Sample random sampling time %lds, %ldms\n", random_diff.tv_sec, random_diff.tv_nsec/1000000);

	struct timespec bucket_spawn_diff = diff(bucket_spawn_start, bucket_spawn_end);
	printf("SAMPLESORT TASK : Sample bucket spawn time %lds, %ldms\n", bucket_spawn_diff.tv_sec, bucket_spawn_diff.tv_nsec/1000000);

	struct timespec wait_bucket_diff = diff(wait_bucket_start, wait_bucket_end);
	printf("SAMPLESORT TASK : Final waiting on bucket time %lds, %ldms\n", wait_bucket_diff.tv_sec, wait_bucket_diff.tv_nsec/1000000);

	// To unlock test case
	if (errno > 0) {
		printf("Error number=%d\n", errno);
	}
	done = 1;
}

static void receive(SampleSortTask this) {
	int tag = Comm->getMsgTag(this->taskID);
	while (tag < 0) {
		tag = Comm->getMsgTag(this->taskID);
	}

	Message msg;

	// match the message to the right message "handler"
	switch (tag) {
	case DONE_MSG:
		msg = Comm->receive(this->taskID);
		handle_DoneMsg(this, (DoneMsg)msg);
		break;
	case BAR_MSG:
		msg = Comm->receive(this->taskID);
		handle_BarMsg(this, (BarMsg)msg);
		break;
	case REF_INTARRAY_MSG:
		msg = Comm->receive(this->taskID);
		handle_RefIntArrayMsg(this, (RefIntArrayMsg)msg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(this->taskID);
	}
}

static void handle_DoneMsg(SampleSortTask this, DoneMsg doneMsg) {
	printf("Samplesort task id %d received done message\n", this->taskID);
}

static void handle_BarMsg(SampleSortTask this, BarMsg barMsg) {
	printf("Samplesort task id %d received K\n", this->taskID);
	this->K = barMsg->getValue(barMsg);
}

static void handle_RefIntArrayMsg(SampleSortTask this, RefIntArrayMsg refIntArrayMsg) {
	printf("Samplesort task id %d received initial data\n", this->taskID);
	this->size = refIntArrayMsg->getSize(refIntArrayMsg);
	this->data = refIntArrayMsg->values;
}

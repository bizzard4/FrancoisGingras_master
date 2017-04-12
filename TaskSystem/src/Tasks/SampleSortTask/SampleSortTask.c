#include "TaskSystem/Tasks/SampleSortTask/SampleSortTask.h"
#include "TaskSystem/Tasks/BucketTask/BucketTask.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/TopologyMsg/TopologyMsg.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h"
#include "TaskSystem/Messages/RefIntArrayMsg/RefIntArrayMsg.h"
#include "TaskSystem/Messages/RefTwoDimIntArrayMsg/RefTwoDimIntArrayMsg.h"
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

pthread_cond_t MainSleepCond; // For the test case, without any good way to "wait" on a task to be done, we will use that.

// Messages (all)
enum {
	// Pre-phase
	BAR_MSG, 
	REF_INTARRAY_MSG,
	// Phase 1 
	TOPOLOGY_MSG, // topology msg
	DATA_REF_MSG, // int ref array message
	SPLITTER_MSG, // int array message
	// Phase 2
	GET_SUB_ARRAY_MSG, // done message (signal)
	SUB_ARRAT_MSG, // 2 dimension ref int array message
	// Phase 3
	SET_SUB_ARRAY_MSG, // 2 dimension ref int array message
	DONE_MSG // done message (singla)
};

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

	// Pre-round - get initial data

	// Get the number of buckets
	struct timespec initialization_start, initialization_end;
	clock_gettime(CLOCK_MONOTONIC, &initialization_start);
	receive(this);

	// Get the data to sort
	receive(this);
	int size_per_task = this->size / this->K;
	clock_gettime(CLOCK_MONOTONIC, &initialization_end);
	printf("Size per task = %d (N=%d, K=%d) \n", size_per_task, this->size, this->K);

	// Gather random sample and prepare splitters
	// 1) Get x sample (from table)
	// 2) Sort them
	// 3) Pick from the sorted set
	struct timespec random_start, random_end;
	clock_gettime(CLOCK_MONOTONIC, &random_start);

	// 1) (TODO : TABLE)
	const int SAMPLE_SIZE = 1500;
	srand(time(NULL));
	int samples[SAMPLE_SIZE];
	for (int i = 0; i < SAMPLE_SIZE; i++) {
		long val = random_at_most(this->size-1);
		samples[i] = this->data[val];
	}

	// 2)
	qsort(samples, SAMPLE_SIZE, sizeof(int), samplesorttask_cmpfunc);

	// 3)
	int splitters[this->K];
	int step = SAMPLE_SIZE/(this->K);
	printf("Splitter : ");
	for (int i = 0; i < (this->K)-1; i++) {
		splitters[i] = samples[(i+1)*step];
		printf("(i=%d,v=%d) ", i, splitters[i]);
	}
	splitters[(this->K)-1] = 1000000; // Last splitter is not important
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC, &random_end);

	// Create K bucket task
	struct timespec bucket_spawn_start, bucket_spawn_end;
	clock_gettime(CLOCK_MONOTONIC, &bucket_spawn_start);
	unsigned int buckets[this->K];
	for(int i = 0; i < this->K; i++) {
		buckets[i] = BucketTask_create();
	}
	printf("Bucket tasks created\n");

	// ----------------------------------
	// Round 1) - Send topology, data reference and splitters to K buckets
	// ----------------------------------

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
		RefIntArrayMsg data_msg = RefIntArrayMsg_create(DATA_REF_MSG);
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
	IntArrayMsg splitters_msg = IntArrayMsg_create(SPLITTER_MSG);
	splitters_msg->setValues(splitters_msg, this->K, splitters);
	for (int ki = 0; ki < this->K; ki++) {
		send(this, (Message)splitters_msg, buckets[ki]);
	}
	splitters_msg->destroy(splitters_msg);
	clock_gettime(CLOCK_MONOTONIC, &bucket_spawn_end);

	printf("Phase 1 is done\n");

	// ----------------------------------
	// Round 2) - Get K buckets subarray
	// ----------------------------------

	struct timespec wait_bucket_start, wait_bucket_end;
	clock_gettime(CLOCK_MONOTONIC, &wait_bucket_start);
	this->buckets_values_arr = malloc(this->K * sizeof(RefTwoDimIntArrayMsg));
	this->buckets_values_arr_count = 0;
	DoneMsg getSubArraySignal = DoneMsg_create(GET_SUB_ARRAY_MSG);
	getSubArraySignal->success = 1;
	for (int ki = 0; ki < this->K; ki++) {
		send(this, (Message)getSubArraySignal, buckets[ki]);
	}
	getSubArraySignal->destroy(getSubArraySignal);

	// Phase 2 wait on K sub-array message
	for (int ki = 0; ki < this->K; ki++) {
		receive(this);
	}
	printf("Phase 2 is done\n");

	// ----------------------------------
	// Round 3) - Send K buckets reference to all subarray
	// ----------------------------------

	// Send sub array message to K buckets
	for (int ki = 0; ki < this->K; ki++) {
		RefTwoDimIntArrayMsg ref_to_bucket = RefTwoDimIntArrayMsg_create(SET_SUB_ARRAY_MSG);

		int* counts = malloc(this->K * sizeof(int));
		int** values = malloc(this->K * sizeof(int*));

		for (int kj = 0; kj < this->K; kj++) {
			counts[kj] = this->buckets_values_arr[kj]->counts[ki];
			values[kj] = this->buckets_values_arr[kj]->values[ki];
		}

		ref_to_bucket->size = this->K;
		ref_to_bucket->counts = counts;
		ref_to_bucket->values = values;
		send(this, (Message)ref_to_bucket, buckets[ki]);
		ref_to_bucket->destroy(ref_to_bucket);
	}

	// Phase 3 wait on K done message
	for (int ki = 0; ki < this->K; ki++) {
		receive(this);
	}
	clock_gettime(CLOCK_MONOTONIC, &wait_bucket_end);
	printf("Phase 3 is done\n");

	// Printing timiing
	printf("Samplesort completed\n");

	struct timespec initialization_diff = diff(initialization_start, initialization_end);
	printf("TIMING-SAMPLESORT %lds,%ldms - Initialization time \n", initialization_diff.tv_sec, initialization_diff.tv_nsec/1000000);

	struct timespec random_diff = diff(random_start, random_end);
	printf("TIMING-SAMPLESORT %lds,%ldms - Sample random sampling time \n", random_diff.tv_sec, random_diff.tv_nsec/1000000);

	struct timespec bucket_spawn_diff = diff(bucket_spawn_start, bucket_spawn_end);
	printf("TIMING-SAMPLESORT %lds,%ldms - Sample bucket spawn with data/splitter time \n", bucket_spawn_diff.tv_sec, bucket_spawn_diff.tv_nsec/1000000);

	struct timespec wait_bucket_diff = diff(wait_bucket_start, wait_bucket_end);
	printf("TIMING-SAMPLESORT %lds,%ldms - Final waiting on bucket time \n", wait_bucket_diff.tv_sec, wait_bucket_diff.tv_nsec/1000000);

	// To unlock test case
	if (errno > 0) {
		printf("Error number=%d\n", errno);
	}
	pthread_cond_signal(&MainSleepCond);
}

static void receive(SampleSortTask this) {
	int tag = Comm->getMsgTag(Comm, this->taskID);
	while (tag < 0) {
		tag = Comm->getMsgTag(Comm, this->taskID);
	}

	Message msg;

	// match the message to the right message "handler"
	switch (tag) {
		// Pre-phase
	case BAR_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_BarMsg(this, (BarMsg)msg);
		break;
	case REF_INTARRAY_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_RefIntArrayMsg(this, (RefIntArrayMsg)msg);
		break;
		// Phase 1
	case TOPOLOGY_MSG: // Should never be received
		break;
	case DATA_REF_MSG: // Should never be received
		break;
	case SPLITTER_MSG: // Should never be received
		break;
		// Phase 2
	case GET_SUB_ARRAY_MSG: // Should never be received
		break;
	case SUB_ARRAT_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_SubArrayMsg(this, (RefTwoDimIntArrayMsg)msg);
		break;
		// Phase 3
	case SET_SUB_ARRAY_MSG: // Should never be received
		break;
	case DONE_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_DoneMsg(this, (DoneMsg)msg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(Comm, this->taskID);
	}
}

// Pre-phase

static void handle_BarMsg(SampleSortTask this, BarMsg barMsg) {
	printf("Samplesort task id %d received K\n", this->taskID);
	this->K = barMsg->getValue(barMsg);
}

static void handle_RefIntArrayMsg(SampleSortTask this, RefIntArrayMsg refIntArrayMsg) {
	printf("Samplesort task id %d received initial data\n", this->taskID);
	this->size = refIntArrayMsg->getSize(refIntArrayMsg);
	this->data = refIntArrayMsg->values;
}

// Phase 1

// Phase 2

static void handle_SubArrayMsg(SampleSortTask this, RefTwoDimIntArrayMsg reftwodimMsg) {
	printf("Samplesort task id %d received sub array data\n", this->taskID);

	this->buckets_values_arr[this->buckets_values_arr_count] = reftwodimMsg;
	this->buckets_values_arr_count++;
}

// Phase 3

static void handle_DoneMsg(SampleSortTask this, DoneMsg doneMsg) {
	printf("Samplesort task id %d received done message\n", this->taskID);
}

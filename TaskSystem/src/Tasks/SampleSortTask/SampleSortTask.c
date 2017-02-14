#include "TaskSystem/Tasks/SampleSortTask/SampleSortTask.h"
#include "TaskSystem/Tasks/BucketTask/BucketTask.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/TopologyMsg/TopologyMsg.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h"
#include "TaskSystem/Messages/DoneMsg/DoneMsg.h"
#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/SampleSortTask/generated.h"

int done; // For the test case, without any good way to "wait" on a task to be done, we will use that.

// Messages
enum {TOPOLOGY_MSG, INTARRAY_MSG, DONE_MSG, BAR_MSG};

enum {WAITING_ON_DATA, WAITING_ON_SAMPLES};

#define LARGE_DATA

int samplesorttask_cmpfunc(const void* a, const void* b)
{
   return (*(int*)a - *(int*)b);
}

static void start(SampleSortTask this) {

	// Get the number of buckets
	receive(this);

	// Create K bucket task
	unsigned int buckets[this->K];

	for(int i = 0; i < this->K; i++) {
		buckets[i] = BucketTask_create();
	}
	printf("Bucket tasks created\n");

	// Get the data to sort
	this->state = WAITING_ON_DATA;
	receive(this);
	int size_per_task = this->size / this->K;

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

	// Send sample data to all K bucket task
	int current_index = 0;
	for (int ki = 0; ki < this->K; ki++) {
		IntArrayMsg data_msg = IntArrayMsg_create(INTARRAY_MSG);

		printf("Sending sample data=");
		int current_size = size_per_task;
		int* data_to_task = malloc(current_size * sizeof(int));
		for (int i = 0; i < size_per_task; i++) {
			int val = this->data[current_index+i];
#ifndef LARGE_DATA
			printf("%d ", val);
#endif
			data_to_task[i] = val;
		}
		current_index += size_per_task;
		if (ki==(this->K-1)) { // Need to happend the last part of the array to the end
			int rest = this->size - current_index;
			if (rest > 0) {
				data_to_task = realloc(data_to_task, (current_size+rest) * sizeof(int));
				for (int i = 0; i < rest; i++) {
					int val = this->data[current_index+i];
#ifndef LARGE_DATA
					printf("%d ", val);
#endif
					data_to_task[size_per_task+i] = val;
				}
				current_size += rest;
			}

		}
#ifdef LARGE_DATA
		printf("[Large data]");
#endif
		printf(" to task %d (size=%d)\n", buckets[ki], current_size);
		data_msg->setValues(data_msg, current_size, data_to_task);
		free(data_to_task);
		send(this, (Message)data_msg, buckets[ki]);
		data_msg->destroy(data_msg);
	}
	printf("Sample data send to bucket\n");


	// Wait on K samples from bucket task
	this->state = WAITING_ON_SAMPLES;
	this->samples = NULL;
	this->sample_size = 0;
	for (int ki = 0; ki < this->K; ki++) {
		receive(this);
	}
	printf("Received all samples from buckets\n");

	// Sort samples received
	qsort(this->samples, this->sample_size, sizeof(int), samplesorttask_cmpfunc);

	// Compute and send splitters information
	int step = this->sample_size / this->K;
	int splitters[100]; // TODO : Dynamic this
	int count = 0;
	printf("Splitters : ");
	for (int ni = 0; ni < this->sample_size; ni += step) {
		int val = this->samples[ni];
		printf("%d ", val);
		splitters[count] = val;
		count++;
	}
	printf(" (Size=%d) \n", count);
	IntArrayMsg splitters_msg = IntArrayMsg_create(INTARRAY_MSG);
	splitters_msg->setValues(splitters_msg, count, splitters);
	for (int ki = 0; ki < this->K; ki++) {
		send(this, (Message)splitters_msg, buckets[ki]);
	}
	splitters_msg->destroy(splitters_msg);
	printf("Done sending splitters\n");

	// Wait on K done signal
	for (int ki = 0; ki < this->K; ki++) {
		receive(this);
	}
	printf("Received all bucket done messaage\n");

	// TODO : This part, depending on how the data is store may be needed
	// Gather result (Each bucket will print final result at "done" reception")

	// Send done signal to destroy task
	DoneMsg done_msg = DoneMsg_create(DONE_MSG);
	done_msg->success = 1;
	for (int ki = 0; ki < this->K; ki++) {
		send(this, (Message)done_msg, buckets[ki]);
	}
	done_msg->destroy(done_msg);

	// Wait on K done signal
	printf("Waiting on K output done signal\n");
	for (int ki = 0; ki < this->K; ki++) {
		receive(this);
	}

	printf("Samplesort completed\n");

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
	case INTARRAY_MSG :
		msg = Comm->receive(this->taskID);
		handle_IntArrayMsg(this, (IntArrayMsg)msg);
		break;
	case DONE_MSG:
		msg = Comm->receive(this->taskID);
		handle_DoneMsg(this, (DoneMsg)msg);
		break;
	case BAR_MSG:
		msg = Comm->receive(this->taskID);
		handle_BarMsg(this, (BarMsg)msg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(this->taskID);
	}
}

static void handle_IntArrayMsg(SampleSortTask this, IntArrayMsg intarrayMsg) {

	switch(this->state) {
	case WAITING_ON_DATA:
		printf("Samplesort task id %d received initial data\n", this->taskID);

		this->size = intarrayMsg->getSize(intarrayMsg);

		// Deep copy message
		this->data = malloc(intarrayMsg->getSize(intarrayMsg) * sizeof(int));
		for (int i = 0; i < intarrayMsg->getSize(intarrayMsg); i++) {
			this->data[i] = intarrayMsg->getValue(intarrayMsg, i);
		}
		break;
	case WAITING_ON_SAMPLES:
		printf("Samplesort task id %d received sample\n", this->taskID);

		int old_size = this->sample_size;
		this->sample_size += intarrayMsg->getSize(intarrayMsg);

		if (this->samples == NULL) {
			this->samples = malloc(this->sample_size * sizeof(int));
		} else {
			this->samples = realloc(this->samples, this->sample_size * sizeof(int));
		}

		// Add new values (deep copy)
		printf("Adding sample : ");
		for (int i = old_size; i < this->sample_size; i++) {
			int val = intarrayMsg->getValue(intarrayMsg, i-old_size);
#ifndef LARGE_DATA
			printf("%d ", val);
#endif
			this->samples[i] = val;
		}
#ifdef LARGE_DATA
		printf("[Large data]");
#endif
		printf("\n");
		break;
	default:
		printf("SAMPLESORTTASK ERROR : Received intarray at unknown state");
	}
}

static void handle_DoneMsg(SampleSortTask this, DoneMsg doneMsg) {
	printf("Samplesort task id %d received done message\n", this->taskID);
}

static void handle_BarMsg(SampleSortTask this, BarMsg barMsg) {
	printf("Samplesort task id %d received K\n", this->taskID);
	this->K = barMsg->getValue(barMsg);
}

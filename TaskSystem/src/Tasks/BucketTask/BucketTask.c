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

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/BucketTask/generated.h"

//#define DEBUG_PROPAGATION

// Messages
enum {TOPOLOGY_MSG, INTARRAY_MSG, DONE_MSG, BAR_MSG};

enum {WAITING_ON_SAMPLE_DATA, WAITING_ON_SPLITTERS, WAITING_ON_DONE, RECEIVED_DONE};

int buckettask_cmpfunc(const void* a, const void* b)
{
   return (*(int*)a - *(int*)b);
}

static void start(BucketTask this) {
	// Get topology
	receive(this);

	// Get sample data
	this->state = WAITING_ON_SAMPLE_DATA;
	receive(this);

	// Sort received sample data
	qsort(this->sample_data_values, this->sample_data_size, sizeof(int), buckettask_cmpfunc);
	printf("Sorted data : ");
	for (int i = 0; i < this->sample_data_size; i++) {
		printf("%d ", this->sample_data_values[i]);
	}
	printf("\n");

	// Pick sample and send them to root
	int step = this->sample_size / this->bucket_count;
	if (step == 0) {
		step = 1;
	}
	int samples[100]; // TODO : Need to be dynamic
	int count = 0;
	for (int i = 0; i < this->sample_size; i += step) {
		samples[count] = this->sample_data_values[i];
		count++;
	}
	IntArrayMsg sample_msg = IntArrayMsg_create(INTARRAY_MSG);
	sample_msg->setValues(sample_msg, count, samples);
	send(this, (Message)sample_msg, this->root_id);

	// Get splitters
	this->state = WAITING_ON_SPLITTERS;
	this->final_data_size = 0;
	this->final_data_values = NULL;
	receive(this);


	// Propagate data
	for (int i = 0; i < this->sample_data_size; i++) {
		int val = this->sample_data_values[i];
		for (int si = 1; si < this->splitter_size; si++) {
			if (val < this->splitters[si]) {
#ifdef DEBUG_PROPAGATION
				printf("Bucket id=%d sending value %d to bucket i=%d\n", this->taskID, val, si-1);
#endif
				BarMsg bar_msg = BarMsg_create(BAR_MSG);
				bar_msg->setValue(bar_msg, val);
				send(this, (Message)bar_msg, this->bucket_ids[si-1]);
				bar_msg->destroy(bar_msg);
				break;
			} else if (si == (this->splitter_size-1)) {
#ifdef DEBUG_PROPAGATION
				printf("Bucket id=%d sending value %d to bucket i=%d\n", this->taskID, val, si);
#endif
				BarMsg bar_msg = BarMsg_create(BAR_MSG);
				bar_msg->setValue(bar_msg, val);
				send(this, (Message)bar_msg, this->bucket_ids[si]);
				bar_msg->destroy(bar_msg);
			}
		}
	}

	// Send "done" to root
	DoneMsg done_msg = DoneMsg_create(DONE_MSG);
	done_msg->success = 1;
	send(this, (Message)done_msg, this->root_id);

	// Recover final values for this bucket until "done" from root
	this->state = WAITING_ON_DONE;
	while (this->state == WAITING_ON_DONE) {
		receive(this);
	}

	// Receive "done" from root and printf final values
	printf("Bucket %d final values (count=%d) : ", this->taskID, this->final_data_size);
	for (int i = 0; i < this->final_data_size; i++) {
		printf("%d ", this->final_data_values[i]);
	}
	printf("\n");

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

	//printf("Bucket task %d received tag=%d\n", this->taskID, tag);

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

static void handle_TopologyMsg(BucketTask this, TopologyMsg topologyMsg) {
	printf("Bucket task %d received topology\n", this->taskID);
	this->bucket_count = topologyMsg->bucket_count; // K
	this->bucket_ids = malloc(topologyMsg->bucket_count * sizeof(unsigned int));
	for (int i = 0; i < topologyMsg->bucket_count; i++) {
		this->bucket_ids[i] = topologyMsg->bucket_ids[i];
	}
	this->root_id = topologyMsg->root_id;
	this->sample_size = topologyMsg->sample_size; // SN
	this->data_size = topologyMsg->data_size; // N
}

static void handle_IntArrayMsg(BucketTask this, IntArrayMsg intarrayMsg) {
	switch(this->state) {
	case WAITING_ON_SAMPLE_DATA:
		printf("Bucket task %d received sample data (size=%d)\n", this->taskID, intarrayMsg->getSize(intarrayMsg));

		this->sample_data_size = intarrayMsg->getSize(intarrayMsg);
		this->sample_data_values = malloc(intarrayMsg->getSize(intarrayMsg) * sizeof(int));
		for (int i = 0; i < intarrayMsg->getSize(intarrayMsg); i++) {
			this->sample_data_values[i] = intarrayMsg->getValue(intarrayMsg, i);
		}
		break;
	case WAITING_ON_SPLITTERS:
		printf("Bucket task %d received splitters\n", this->taskID);

		this->splitter_size = intarrayMsg->getSize(intarrayMsg);
		this->splitters = malloc(intarrayMsg->getSize(intarrayMsg) * sizeof(int));
		for (int i = 0; i < intarrayMsg->getSize(intarrayMsg); i++) {
			this->splitters[i] = intarrayMsg->getValue(intarrayMsg, i);
		}
		break;
	default:
		printf("BUCKETTASK ERROR : Received intarray at unknown state");
	}
}

static void handle_DoneMsg(BucketTask this, DoneMsg doneMsg) {
	printf("Bucket task %d received done message\n", this->taskID);
	this->state = RECEIVED_DONE;
}

static void handle_BarMsg(BucketTask this, BarMsg barMsg) {
#ifdef DEBUG_PROPAGATION
	printf("Bucket task %d received a value %d\n", this->taskID, barMsg->getValue(barMsg));
#endif

	// TODO : Inserting sort?
	this->final_data_size += 1;
	if (this->final_data_values == NULL) {
		this->final_data_values = malloc(this->final_data_size * sizeof(int));
	} else {
		this->final_data_values = realloc(this->final_data_values, this->final_data_size * sizeof(int));
	}
	this->final_data_values[this->final_data_size-1] = barMsg->getValue(barMsg);
	qsort(this->final_data_values, this->final_data_size, sizeof(int), buckettask_cmpfunc);
}

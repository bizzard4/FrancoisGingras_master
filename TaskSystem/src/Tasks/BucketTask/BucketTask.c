#include "TaskSystem/Tasks/BucketTask/BucketTask.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/TopologyMsg/TopologyMsg.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h" 
#include "TaskSystem/Messages/DoneMsg/DoneMsg.h"
#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"
#include "TaskSystem/TimeHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/BucketTask/generated.h"

//#define DEBUG_PROPAGATION
#define LARGE_DATA

// Messages
enum {TOPOLOGY_MSG, INTARRAY_MSG, DONE_MSG, BAR_MSG};

enum {WAITING_ON_SAMPLE_DATA, WAITING_ON_SPLITTERS, RECEIVED_SPLITTERS, WAITING_ON_DONE, RECEIVED_DONE};

int buckettask_cmpfunc(const void* a, const void* b)
{
   return (*(int*)a - *(int*)b);
}

static void addValue(BucketTask this, int val) {
	this->final_data_size += 1;
	if (this->final_data_values == NULL) {
		// We initially create the array with the sample_size received / 2
		this->final_data_capacity = this->sample_data_size/2;
		this->final_data_values = malloc(this->final_data_capacity * sizeof(int));
	} else {
		if (this->final_data_size == this->final_data_capacity) { // Array full, need to make it bigger
			this->final_data_capacity = this->final_data_capacity*2;
			this->final_data_values = realloc(this->final_data_values, this->final_data_capacity * sizeof(int));
		}
	}
	this->final_data_values[this->final_data_size-1] = val;
}

static void sendValueTo(BucketTask this, int val, unsigned int id) {
	if (id == this->taskID) {
		addValue(this, val);
	} else {
		BarMsg bar_msg = BarMsg_create(BAR_MSG);
		bar_msg->setValue(bar_msg, val);
		send(this, (Message)bar_msg, id);
		bar_msg->destroy(bar_msg);
	}
}



static void start(BucketTask this) {
	// Get topology
	receive(this);

	struct timespec get_sample_start, get_sample_end;
	clock_gettime(CLOCK_MONOTONIC, &get_sample_start);
	// Get sample data
	this->state = WAITING_ON_SAMPLE_DATA;
	receive(this);

	// Sort received sample data
	qsort(this->sample_data_values, this->sample_data_size, sizeof(int), buckettask_cmpfunc);
	printf("Sorted data : ");
#ifdef LARGE_DATA
	printf("[Large data]");
#else
	for (int i = 0; i < this->sample_data_size; i++) {
		printf("%d ", this->sample_data_values[i]);
	}
#endif
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC, &get_sample_end);

	struct timespec sample_pick_start, sample_pick_end;
	clock_gettime(CLOCK_MONOTONIC, &sample_pick_start);
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
	clock_gettime(CLOCK_MONOTONIC, &sample_pick_end);

	struct timespec get_splitter_start, get_splitter_end;
	clock_gettime(CLOCK_MONOTONIC, &get_splitter_start);
	// Get splitters
	this->state = WAITING_ON_SPLITTERS;
	this->final_data_size = 0;
	this->final_data_capacity = 0;
	this->final_data_values = NULL;
	while (this->state == WAITING_ON_SPLITTERS) {
		receive(this);
	}
	clock_gettime(CLOCK_MONOTONIC, &get_splitter_end);

	struct timespec send_data_start, send_data_end;
	clock_gettime(CLOCK_MONOTONIC, &send_data_start);
	// Propagate data
	printf("Bucket %d start propagating data (size=%d) \n", this->taskID, this->sample_data_size);
	for (int i = 0; i < this->sample_data_size; i++) {
		int val = this->sample_data_values[i];
		for (int si = 1; si < this->splitter_size; si++) {
			if (val < this->splitters[si]) {
#ifdef DEBUG_PROPAGATION
				printf("Bucket id=%d sending value %d to bucket i=%d\n", this->taskID, val, si-1);
#endif
				sendValueTo(this, val, this->bucket_ids[si-1]);
				break;
			} else if (si == (this->splitter_size-1)) {
#ifdef DEBUG_PROPAGATION
				printf("Bucket id=%d sending value %d to bucket i=%d\n", this->taskID, val, si);
#endif
				sendValueTo(this, val, this->bucket_ids[si]);
				break;
			}
		}
	}
	printf("Bucket %d done propagating data \n", this->taskID);
	// Free sample data received and splitters (no more needed)
	free(this->sample_data_values);
	free(this->splitters);

	// Send "done" to root
	DoneMsg done_msg = DoneMsg_create(DONE_MSG);
	done_msg->success = 1;
	send(this, (Message)done_msg, this->root_id);
	done_msg->destroy(done_msg);
	clock_gettime(CLOCK_MONOTONIC, &send_data_end);

	struct timespec get_data_start, get_data_end;
	clock_gettime(CLOCK_MONOTONIC, &get_data_start);
	// Recover final values for this bucket until "done" from root
	this->state = WAITING_ON_DONE;
	while (this->state == WAITING_ON_DONE) {
		receive(this);
	}
	clock_gettime(CLOCK_MONOTONIC, &get_data_end);

	struct timespec finalize_start, finalize_end;
	clock_gettime(CLOCK_MONOTONIC, &finalize_start);
	// Final sorting
	qsort(this->final_data_values, this->final_data_size, sizeof(int), buckettask_cmpfunc);

	// Receive "done" from root and printf final values
	printf("Bucket %d final values (count=%d) : ", this->taskID, this->final_data_size);
#ifndef LARGE_DATA
	for (int i = 0; i < this->final_data_size; i++) {
		printf("%d ", this->final_data_values[i]);
	}
#else
	printf("[Large data]");
#endif
	printf("\n");
	clock_gettime(CLOCK_MONOTONIC, &finalize_end);

	// Display statistics
	struct timespec get_sample_diff = diff(get_sample_start, get_sample_end);
	printf("BUCKET TASK %d : Get sample time %lds, %ldms\n",this->taskID, get_sample_diff.tv_sec, get_sample_diff.tv_nsec/1000000);

	struct timespec sample_pick_diff = diff(sample_pick_start, sample_pick_end);
	printf("BUCKET TASK %d : Time to pick sample %lds, %ldms\n",this->taskID, sample_pick_diff.tv_sec, sample_pick_diff.tv_nsec/1000000);

	struct timespec get_splitter_diff = diff(get_splitter_start, get_splitter_end);
	printf("BUCKET TASK %d : Waiting on splitter time %lds, %ldms\n",this->taskID, get_splitter_diff.tv_sec, get_splitter_diff.tv_nsec/1000000);

	struct timespec send_data_diff = diff(send_data_start, send_data_end);
	printf("BUCKET TASK %d : Propagation time %lds, %ldms\n",this->taskID, send_data_diff.tv_sec, send_data_diff.tv_nsec/1000000);

	struct timespec get_data_diff = diff(get_data_start, get_data_end);
	printf("BUCKET TASK %d : Receiving data time %lds, %ldms\n",this->taskID, get_data_diff.tv_sec, get_data_diff.tv_nsec/1000000);

	struct timespec finalize_diff = diff(finalize_start, finalize_end);
	printf("BUCKET TASK %d : Displat time %lds, %ldms\n",this->taskID, finalize_diff.tv_sec, finalize_diff.tv_nsec/1000000);

	// Send "done" to root signaling output is complete
	DoneMsg output_done_msg = DoneMsg_create(DONE_MSG);
	output_done_msg->success = 1;
	send(this, (Message)output_done_msg, this->root_id);
	output_done_msg->destroy(output_done_msg);

	printf("Bucket %d is done\n", this->taskID);
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
		this->state = RECEIVED_SPLITTERS;
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
	addValue(this, barMsg->getValue(barMsg));
}

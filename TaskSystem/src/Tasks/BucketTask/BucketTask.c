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

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/BucketTask/generated.h"

//#define DEBUG_DATA // If set, bucket will display all data 
#define DEBUG_OUTPUT // Is set, the ouput will be formated to be easily formatable, warning, this affect performance.

// To use validate_result script, set DEBUG_OUTPUT, but unset DEBUG_DATA

// Messages (all)
enum {BAR_MSG, DONE_MSG, INTARRAY_MSG, REF_INTARRAY_MSG, TOPOLOGY_MSG, PROPAGATION_MSG};

// States
enum {GET_TOPO, GET_DATA, GET_SPLITTERS, PROPAGATION};

// qsort compare function
int buckettask_cmpfunc(const void* a, const void* b)
{
   return (*(int*)a - *(int*)b);
}

static void start(BucketTask this) {
	printf("Bucket %d starting\n", this->taskID);

	this->state = GET_TOPO;

	// Get topology
	while (this->state == GET_TOPO) {
		receive(this);
	}

	// Get ref to data
	struct timespec get_sample_start, get_sample_end;
	clock_gettime(CLOCK_MONOTONIC, &get_sample_start);
	while (this->state == GET_DATA) {
		receive(this);
	}
#ifdef DEBUG_DATA
	printf("Bucket %d data (size=%d) : ", this->taskID, this->data_size);
	for (int i = 0; i < this->data_size; i++) {
		printf("%d ", this->data_ref[i]);
	}
	printf("\n");
#endif
	clock_gettime(CLOCK_MONOTONIC, &get_sample_end);

	// Get splitters
	struct timespec get_splitter_start, get_splitter_end;
	clock_gettime(CLOCK_MONOTONIC, &get_splitter_start);
	while (this->state == GET_SPLITTERS) {
		receive(this);
	}
#ifdef DEBUG_DATA
	printf("Bucket %d splitters=", this->taskID);
	for (int i = 0; i < this->splitter_size; i++) {
		printf("%d ", this->splitters[i]);
	}
	printf("\n");
#endif
	clock_gettime(CLOCK_MONOTONIC, &get_splitter_end);

	// Pick values to send to others
	struct timespec propagate_start, propagate_end;
	clock_gettime(CLOCK_MONOTONIC, &propagate_start);

	// 1) get count
	int counts[this->bucket_count];
	for (int i = 0; i < this->bucket_count; i++) {
		counts[i] = 0;
	}
	for (int i = 0; i < this->data_size; i++) {
		int val = this->data_ref[i];
		for (int si = 0; si < this->splitter_size; si++) {
			if (val <= this->splitters[si]) {
				counts[si]++;
				break;
			} else if (si == (this->splitter_size-1)) { // Last
				counts[si]++;
				break;
			}
		}
	}
#ifdef DEBUG_DATA
	printf("Bucket %d counts=", this->taskID);
	for (int i = 0; i < this->bucket_count; i++) {
		printf("%d ", counts[i]);
	}
	printf("\n");
#endif

	// 2) fill array
	int* values_to_propagate[this->bucket_count];
	int current[this->bucket_count];
	for (int i = 0; i < this->bucket_count; i++) {
		values_to_propagate[i] = malloc(sizeof(int) * counts[i]);
		current[i] = 0;
	}

	for (int i = 0; i < this->data_size; i++) {
		int val = this->data_ref[i];
		for (int si = 0; si < this->splitter_size; si++) {
			if (val <= this->splitters[si]) {
				values_to_propagate[si][current[si]] = val;
				current[si]++;
				break;
			} else if (si == (this->splitter_size-1)) { // Last
				values_to_propagate[si][current[si]] = val;
				current[si]++;
			}
		}
	}
#ifdef DEBUG_DATA
	printf("Bucket %d Propagation : ", this->taskID);
	for (int i = 0; i < this->bucket_count; i++) {
		printf("B=%d (", i+2);
		for (int j = 0; j < counts[i]; j++) {
			printf("%d ", values_to_propagate[i][j]);
		}
		printf("), ");
	}
	printf("\n");
#endif

	// 3) send ref to other buckets
	for (int i = 0; i < this->bucket_count; i++) {
		RefIntArrayMsg data_msg = RefIntArrayMsg_create(PROPAGATION_MSG);
		data_msg->setValues(data_msg, counts[i], values_to_propagate[i]);
		send(this, (Message)data_msg, this->bucket_ids[i]); // Dangerous
		data_msg->destroy(data_msg);
	}
	clock_gettime(CLOCK_MONOTONIC, &propagate_end);

	// Rebuild final data
	struct timespec rebuild_start, rebuild_end;
	clock_gettime(CLOCK_MONOTONIC, &rebuild_start);
	this->propagated_messages = malloc(sizeof(int*)*this->bucket_count);
	this->propagated_messages_counts = malloc(sizeof(int)*this->bucket_count);
	this->propagated_messages_current = 0;
	this->final_data_size = 0;
	for (int i = 0; i < this->bucket_count; i++) { // Need K ref_intarray
		receive(this);
	}
	this->final_data_values = malloc(sizeof(int) * this->final_data_size);
	int current_final = 0;
	for (int i = 0; i < this->bucket_count; i++) { // Copy values
		for (int j = 0; j < this->propagated_messages_counts[i]; j++) {
			this->final_data_values[current_final] = this->propagated_messages[i][j];
			current_final++;
		}
	}
	free(this->propagated_messages);
	free(this->propagated_messages_counts);
	clock_gettime(CLOCK_MONOTONIC, &rebuild_end);

	// Final sorting
	struct timespec finalize_start, finalize_end;
	clock_gettime(CLOCK_MONOTONIC, &finalize_start);
	qsort(this->final_data_values, this->final_data_size, sizeof(int), buckettask_cmpfunc);
	clock_gettime(CLOCK_MONOTONIC, &finalize_end);

	// Final display (debug)
#ifdef DEBUG_DATA
	printf("Bucket %d final values (count=%d) : ", this->taskID, this->final_data_size);
	for (int i = 0; i < this->final_data_size; i++) {
		printf("%d ", this->final_data_values[i]);
	}
	printf("\n");
#endif
#ifdef DEBUG_OUTPUT // To use with validate_result script
	for (int i = 0; i < this->final_data_size; i++) {
		printf("BUCKET=%d VALUE=%d\n", this->taskID, this->final_data_values[i]);
	}
#endif
	

	// Display statistics
	struct timespec get_sample_diff = diff(get_sample_start, get_sample_end);
	printf("BUCKET TASK %d : Get sample time %lds, %ldms\n",this->taskID, get_sample_diff.tv_sec, get_sample_diff.tv_nsec/1000000);

	struct timespec get_splitter_diff = diff(get_splitter_start, get_splitter_end);
	printf("BUCKET TASK %d : Waiting on splitter time %lds, %ldms\n",this->taskID, get_splitter_diff.tv_sec, get_splitter_diff.tv_nsec/1000000);

	struct timespec propagate_diff = diff(propagate_start, propagate_end);
	printf("BUCKET TASK %d : Propagation time %lds, %ldms\n",this->taskID, propagate_diff.tv_sec, propagate_diff.tv_nsec/1000000);

	struct timespec rebuild_diff = diff(rebuild_start, rebuild_end);
	printf("BUCKET TASK %d : Rebuild array time %lds, %ldms\n",this->taskID, rebuild_diff.tv_sec, rebuild_diff.tv_nsec/1000000);

	struct timespec finalize_diff = diff(finalize_start, finalize_end);
	printf("BUCKET TASK %d : Final sorting time %lds, %ldms\n",this->taskID, finalize_diff.tv_sec, finalize_diff.tv_nsec/1000000);

	printf("BUCKET TASK %d : Send time accumulator %lds, %ldms\n",this->taskID, this->send_time_acc.tv_sec, this->send_time_acc.tv_nsec/1000000);

	printf("BUCKET TASK %d : Receive waiting accumulator %lds, %ldms\n",this->taskID, this->receive_wait_acc.tv_sec, this->receive_wait_acc.tv_nsec/1000000);

	printf("BUCKET TASK %d : Reveived %d values\n", this->taskID, this->final_data_size);

	// Send "done" to root signaling output is complete
	DoneMsg output_done_msg = DoneMsg_create(DONE_MSG);
	output_done_msg->success = 1;
	send(this, (Message)output_done_msg, this->root_id);
	output_done_msg->destroy(output_done_msg);

	printf("Bucket %d is done\n", this->taskID);
}

static void receive(BucketTask this) {
	int tag = Comm->getMsgTag(this->taskID);

	if (tag < 0) { // To accumulate receive time
		struct timespec recv_start, recv_end;
		clock_gettime(CLOCK_MONOTONIC, &recv_start);
		while (tag < 0) { // Loop until message arrive.
			tag = Comm->getMsgTag(this->taskID);
		}
		clock_gettime(CLOCK_MONOTONIC, &recv_end);
		this->receive_wait_acc = tsAdd(this->receive_wait_acc, diff(recv_start, recv_end));
	}

	Message msg;

	// match the message to the right message "handler"
	switch (tag) {
	case TOPOLOGY_MSG:
		msg = Comm->receive(this->taskID);
		handle_TopologyMsg(this, (TopologyMsg)msg);
		break;
	case REF_INTARRAY_MSG:
		msg = Comm->receive(this->taskID);
		handle_RefIntArrayMsg(this, (RefIntArrayMsg)msg);
		break;
	case INTARRAY_MSG:
		msg = Comm->receive(this->taskID);
		handle_IntArrayMsg(this, (IntArrayMsg)msg);
		break;
	case PROPAGATION_MSG:
		msg = Comm->receive(this->taskID);
		handle_PropagationMsg(this, (RefIntArrayMsg)msg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(this->taskID);
	}
}

static void handle_TopologyMsg(BucketTask this, TopologyMsg topologyMsg) {
	printf("Bucket %d received topology\n", this->taskID);
	this->bucket_count = topologyMsg->bucket_count; // K
	this->bucket_ids = malloc(topologyMsg->bucket_count * sizeof(unsigned int));
	for (int i = 0; i < topologyMsg->bucket_count; i++) {
		this->bucket_ids[i] = topologyMsg->bucket_ids[i];
	}
	this->root_id = topologyMsg->root_id;

	this->state = GET_DATA;
}

static void handle_RefIntArrayMsg(BucketTask this, RefIntArrayMsg refintarrayMsg) {
	printf("Bucket %d received reference to data\n", this->taskID);
	this->data_size = refintarrayMsg->getSize(refintarrayMsg);
	this->data_ref = refintarrayMsg->values;

	this->state = GET_SPLITTERS;
}

static void handle_IntArrayMsg(BucketTask this, IntArrayMsg intarrayMsg) {
	printf("Bucket %d received splitters\n", this->taskID);
	this->splitter_size = intarrayMsg->getSize(intarrayMsg);
	this->splitters = malloc(intarrayMsg->getSize(intarrayMsg) * sizeof(int));
	for (int i = 0; i < intarrayMsg->getSize(intarrayMsg); i++) {
		this->splitters[i] = intarrayMsg->getValue(intarrayMsg, i);
	}

	this->state = PROPAGATION;
}

static void handle_PropagationMsg(BucketTask this, RefIntArrayMsg propagationMsg) {
	// If received before propagation, send back in Q
	if (this->state != PROPAGATION) {
		send(this, (Message)propagationMsg, this->taskID);
		return;
	}

	// Keep pointers and counts
	printf("Bucket %d received propagation message (size=%d) \n", this->taskID, propagationMsg->size);
#ifdef DEBUG_DATA
	printf("Bucket %d propagation values : ", this->taskID);
	for (int i = 0; i < propagationMsg->size; i++) {
		printf("%d ", propagationMsg->values[i]);
	}
	printf("\n");
#endif

	this->propagated_messages[this->propagated_messages_current] = propagationMsg->values;
	this->propagated_messages_counts[this->propagated_messages_current] = propagationMsg->size;
	this->propagated_messages_current++;
	this->final_data_size += propagationMsg->size;
}
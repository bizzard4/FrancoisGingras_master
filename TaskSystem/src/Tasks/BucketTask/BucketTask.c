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

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/BucketTask/generated.h"

//#define DEBUG_DATA // If set, bucket will display all data 
//#define DEBUG_OUTPUT // Is set, the ouput will be formated to be easily formatable, warning, this affect performance.

// To use validate_result script, set DEBUG_OUTPUT, but unset DEBUG_DATA

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

// qsort compare function
int buckettask_cmpfunc(const void* a, const void* b)
{
   return (*(int*)a - *(int*)b);
}

static void start(BucketTask this) {
	printf("Bucket %d starting\n", this->taskID);
	memset(&(this->send_time_acc), 0, sizeof(struct timespec));
	memset(&(this->receive_wait_acc), 0, sizeof(struct timespec));
	

	// ----------------------------------
	// Round 1) - Get topology, data reference and splitters
	// ----------------------------------

	// Get topology
	receive(this);
#ifdef DEBUG_DATA
	printf("Bucket %d tolopogy=", this->taskID);
	for (int i = 0; i < this->bucket_count; i++) {
		printf("%d ", this->bucket_ids[i]);
	}
	printf("\n");
#endif

	// Get ref to data
	struct timespec get_sample_start, get_sample_end;
	clock_gettime(CLOCK_MONOTONIC, &get_sample_start);
	receive(this);
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
	receive(this);
#ifdef DEBUG_DATA
	printf("Bucket %d splitters=", this->taskID);
	for (int i = 0; i < this->splitter_size; i++) {
		printf("%d ", this->splitters[i]);
	}
	printf("\n");
#endif
	clock_gettime(CLOCK_MONOTONIC, &get_splitter_end);

	// ----------------------------------
	// Round 2) - Create sub-array
	// ----------------------------------

	// Get command to start computing sub array
	receive(this);

	// Pick values to send to others
	struct timespec propagate_start, propagate_end;
	clock_gettime(CLOCK_MONOTONIC, &propagate_start);

 	// 1) get count
	int* counts = malloc(this->bucket_count * sizeof(int));
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

	// 3) computed reference to the root
	RefTwoDimIntArrayMsg ref_to_root = RefTwoDimIntArrayMsg_create(SUB_ARRAT_MSG);
	ref_to_root->size = this->bucket_count;
	ref_to_root->counts = counts;
	ref_to_root->values = values_to_propagate;
	send(this, (Message)ref_to_root, this->root_id);
	ref_to_root->destroy(ref_to_root);
	clock_gettime(CLOCK_MONOTONIC, &propagate_end);

	// ----------------------------------
	// Round 3) - Get K reference to all subarray and rebuild final array
	// ----------------------------------

	// Receive sub-array for this bucket
	receive(this);

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
	printf("TIMING-BUCKET %d %lds,%ldms - Get sample time\n",this->taskID, get_sample_diff.tv_sec, get_sample_diff.tv_nsec/1000000);

	struct timespec get_splitter_diff = diff(get_splitter_start, get_splitter_end);
	printf("TIMING-BUCKET %d %lds,%ldms - Waiting on splitter time\n",this->taskID, get_splitter_diff.tv_sec, get_splitter_diff.tv_nsec/1000000);

	struct timespec propagate_diff = diff(propagate_start, propagate_end);
	printf("TIMING-BUCKET %d %lds,%ldms - Propagation time\n",this->taskID, propagate_diff.tv_sec, propagate_diff.tv_nsec/1000000);

	struct timespec rebuild_diff = diff(this->rebuild_start, this->rebuild_end);
	printf("TIMING-BUCKET %d %lds,%ldms - Rebuild array time\n",this->taskID, rebuild_diff.tv_sec, rebuild_diff.tv_nsec/1000000);

	struct timespec finalize_diff = diff(finalize_start, finalize_end);
	printf("TIMING-BUCKET %d %lds,%ldms - Final sorting time\n",this->taskID, finalize_diff.tv_sec, finalize_diff.tv_nsec/1000000);

	printf("TIMING-BUCKET %d %lds,%ldms - Send time accumulator\n",this->taskID, this->send_time_acc.tv_sec, this->send_time_acc.tv_nsec/1000000);

	printf("TIMING-BUCKET %d %lds,%ldms - Receive waiting accumulator\n",this->taskID, this->receive_wait_acc.tv_sec, this->receive_wait_acc.tv_nsec/1000000);

	printf("TIMING-BUCKET %d %d - Reveived values\n", this->taskID, this->final_data_size);

	// Send "done" to root signaling output is complete
	DoneMsg output_done_msg = DoneMsg_create(DONE_MSG);
	output_done_msg->success = 1;
	send(this, (Message)output_done_msg, this->root_id);
	output_done_msg->destroy(output_done_msg);

	printf("Bucket %d is done\n", this->taskID);
}

static void receive(BucketTask this) {
	int tag = Comm->getMsgTag(Comm, this->taskID);

	if (tag < 0) { // To accumulate receive time
		struct timespec recv_start, recv_end;
		clock_gettime(CLOCK_MONOTONIC, &recv_start);
		while (tag < 0) { // Loop until message arrive.
			tag = Comm->getMsgTag(Comm, this->taskID);
		}
		clock_gettime(CLOCK_MONOTONIC, &recv_end);
		this->receive_wait_acc = tsAdd(this->receive_wait_acc, diff(recv_start, recv_end));
	}

	Message msg;

	// match the message to the right message "handler"
	switch (tag) {
		// Pre-phase
	case BAR_MSG: // Should not be received
		break; // Should not be received
	case REF_INTARRAY_MSG:
		break;
		// Phase 1
	case TOPOLOGY_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_TopologyMsg(this, (TopologyMsg)msg);
		break;
	case DATA_REF_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_DataRefMsg(this, (RefIntArrayMsg)msg);
		break;
	case SPLITTER_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_SplitterMsg(this, (IntArrayMsg)msg);
		break;
		// Phase 2
	case GET_SUB_ARRAY_MSG:
		msg = Comm->receive(Comm, this->taskID); 
		handle_GetSubArrayMsg(this, (DoneMsg)msg);
		break;
	case SUB_ARRAT_MSG: // Should never be received
		break;
		// Phase 3
	case SET_SUB_ARRAY_MSG:
		msg = Comm->receive(Comm, this->taskID); 
		handle_SetSubArrayMsg(this, (RefTwoDimIntArrayMsg)msg);
		break;
	case DONE_MSG: // Should never be received
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(Comm, this->taskID);
	}

}

// Phase 1

static void handle_TopologyMsg(BucketTask this, TopologyMsg topologyMsg) {
	printf("Bucket %d received topology\n", this->taskID);
	this->bucket_count = topologyMsg->bucket_count; // K
	this->bucket_ids = malloc(topologyMsg->bucket_count * sizeof(unsigned int));
	for (int i = 0; i < topologyMsg->bucket_count; i++) {
		this->bucket_ids[i] = topologyMsg->bucket_ids[i];
	}
	this->root_id = topologyMsg->root_id;
}

static void handle_DataRefMsg(BucketTask this, RefIntArrayMsg refintarrayMsg) {
	printf("Bucket %d received reference to data\n", this->taskID);
	this->data_size = refintarrayMsg->getSize(refintarrayMsg);
	this->data_ref = refintarrayMsg->values;
}

static void handle_SplitterMsg(BucketTask this, IntArrayMsg intarrayMsg) {
	printf("Bucket %d received splitters\n", this->taskID);

	this->splitter_size = intarrayMsg->getSize(intarrayMsg);
	this->splitters = malloc(intarrayMsg->getSize(intarrayMsg) * sizeof(int));
	for (int i = 0; i < intarrayMsg->getSize(intarrayMsg); i++) {
		this->splitters[i] = intarrayMsg->getValue(intarrayMsg, i);
	}
}

// Phase 2

static void handle_GetSubArrayMsg(BucketTask this, DoneMsg doneMsg) {
	printf("Bucket %d received GetSubArrayMsg request\n", this->taskID);
}

// Phase 3

static void handle_SetSubArrayMsg(BucketTask this, RefTwoDimIntArrayMsg reftwodimMsg) {
	printf("Bucket %d received SetSubArrayMsg\n", this->taskID);

	// Rebuild final data
	clock_gettime(CLOCK_MONOTONIC, &this->rebuild_start);
	this->final_data_size = 0;
	for (int i = 0; i < reftwodimMsg->size; i++) {
		this->final_data_size += reftwodimMsg->counts[i];
	}
	this->final_data_values = malloc(this->final_data_size * sizeof(int));
	int index = 0;
	for (int i = 0; i < reftwodimMsg->size; i++) {
		for (int j = 0; j < reftwodimMsg->counts[i]; j++) {
			this->final_data_values[index] = reftwodimMsg->values[i][j];
			index++;
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &this->rebuild_end);
}
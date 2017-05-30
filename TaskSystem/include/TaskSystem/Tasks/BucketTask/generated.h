
#ifndef BUCKETTASK_GENERATED_H_
#define BUCKETTASK_GENERATED_H_

#include <stdio.h>
#include <pthread.h>

/******************************
 *  System generated code
 ******************************/

static void send(BucketTask this, Message data, int targetTaskID);
static void receive(BucketTask this);

static void message_notify(BucketTask this);
static void message_wait(BucketTask this);
static int message_immediate(BucketTask this);

static void *run(void *BucketTaskkRef);
static void start(BucketTask this);

static void handle_TopologyMsg(BucketTask this, TopologyMsg topologyMsg);
static void handle_DataRefMsg(BucketTask this, RefIntArrayMsg refintarrayMsg);
static void handle_SplitterMsg(BucketTask this, IntArrayMsg intarrayMsg);

static void handle_GetSubArrayMsg(BucketTask this, DoneMsg doneMsg);

static void handle_SetSubArrayMsg(BucketTask this, RefTwoDimIntArrayMsg reftwodimMsg);

// The BucketTask "class"
struct BucketTask {

	// private data members
	pthread_t threadRef;
	int taskID;

	// Algorithm state

	// Topology information
	int bucket_count; // K
	unsigned int* bucket_ids;
	unsigned int root_id;

	// Data ref
	int* data_ref;
	int data_size;

	// Splitters
	int* splitters;
	int splitter_size;

	// Final value, copy
	int* final_data_values;
	int final_data_size;

	// Time accumulator
	struct timespec send_time_acc;
	struct timespec receive_wait_acc;

	// Rebuild time
	struct timespec rebuild_start;
	struct timespec rebuild_end;
};

// The BucketTask "constructor"
unsigned int BucketTask_create(){

	BucketTask newRec = malloc(sizeof(struct BucketTask));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in SampleSortTask_create");

	newRec->taskID = Comm->getNextTaskID(Comm);
	Comm->createMsgQ(Comm, newRec->taskID);

	int result = pthread_create( &(newRec->threadRef), NULL, run, (void *)newRec);
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", result);
	    exit(-1);
	}

	return newRec->taskID;
}

static void *run(void *BucketTaskRef){
	BucketTask this = (BucketTask)BucketTaskRef;
	start(this);
	pthread_exit(NULL);
}

static void send(BucketTask this, Message data, int targetID){
	struct timespec send_start, send_end;
	clock_gettime(CLOCK_MONOTONIC, &send_start);
	Comm->send(Comm, data, targetID);
	clock_gettime(CLOCK_MONOTONIC, &send_end);
	this->send_time_acc = tsAdd(this->send_time_acc, diff(send_start, send_end));
}

static void message_notify(BucketTask this) {
	Comm->message_notify(Comm, this->taskID);
}
static void message_wait(BucketTask this) {
	Comm->message_wait(Comm, this->taskID);
}
static int message_immediate(BucketTask this) {
	return Comm->message_immediate(Comm, this->taskID);
}

#endif /* BUCKETTASK_GENERATED_H_ */

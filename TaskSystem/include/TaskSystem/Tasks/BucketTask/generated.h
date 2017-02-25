
#ifndef BUCKETTASK_GENERATED_H_
#define BUCKETTASK_GENERATED_H_

#include <stdio.h>
#include <pthread.h>

/******************************
 *  System generated code
 ******************************/

static void send(BucketTask this, Message data, int targetTaskID);
static void receive(BucketTask this);

static void *run(void *BucketTaskkRef);
static void start(BucketTask this);

static void handle_TopologyMsg(BucketTask this, TopologyMsg topologyMsg);
static void handle_IntArrayMsg(BucketTask this, IntArrayMsg intarrayMsg);
static void handle_DoneMsg(BucketTask this, DoneMsg doneMsg);
static void handle_NewValuesMsg(BucketTask this, IntArrayMsg valuesMsg);

// The BucketTask "class"
struct BucketTask {

	// private data members
	pthread_t threadRef;
	int taskID;

	// Algorithm state
	int state;

	// Topology information
	int bucket_count; // K
	unsigned int* bucket_ids;
	unsigned int root_id;

	int sample_size; // SN
	int data_size; // N

	// Sample data
	int* sample_data_values;
	int sample_data_size;

	// Splitters
	int* splitters;
	int splitter_size;

	// Propagation ready-to-send
	int** ready_values;
	int* ready_values_count;
	int ready_values_max; // Value count before flush

	// Final value, copy
	int* final_data_values;
	int final_data_capacity; // Store current max val
	int final_data_size;

	// Time accumulator
	struct timespec send_time_acc;
	struct timespec receive_wait_acc;
};

// The BucketTask "constructor"
unsigned int BucketTask_create(){

	BucketTask newRec = malloc(sizeof(struct BucketTask));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in SampleSortTask_create");

	newRec->taskID = Comm->getNextTaskID();
	Comm->createMsgQ(newRec->taskID);

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
	Comm->send(data, targetID);
	clock_gettime(CLOCK_MONOTONIC, &send_end);
	this->send_time_acc = tsAdd(this->send_time_acc, diff(send_start, send_end));
}

#endif /* BUCKETTASK_GENERATED_H_ */

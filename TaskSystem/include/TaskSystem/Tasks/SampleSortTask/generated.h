
#ifndef SAMPLESORTTASK_GENERATED_H_
#define SAMPLESORTTASK_GENERATED_H_

#include <stdio.h>
#include <pthread.h>

/******************************
 *  System generated code
 ******************************/

static void send(SampleSortTask this, Message data, int targetTaskID);
static void receive(SampleSortTask this);

static void *run(void *SampleSortTaskRef);
static void start(SampleSortTask this);

// Pre-phase
static void handle_BarMsg(SampleSortTask this, BarMsg barMsg);
static void handle_RefIntArrayMsg(SampleSortTask this, RefIntArrayMsg refIntArrayMsg);

static void handle_SubArrayMsg(SampleSortTask this, RefTwoDimIntArrayMsg reftwodimMsg);

static void handle_DoneMsg(SampleSortTask this, DoneMsg doneMsg);

// The SampleSortTask "class"
struct SampleSortTask {

	// private data members
	pthread_t threadRef;
	int taskID;

	// Original data
	int* data;
	int size;
	int K;

	// Ref to send to buckets
	RefTwoDimIntArrayMsg* buckets_values_arr;
	int buckets_values_arr_count;
};

// The SampleSortTask "constructor"
unsigned int SampleSortTask_create(){

	SampleSortTask newRec = malloc(sizeof(struct SampleSortTask));
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

static void *run(void *SampleSortTaskRef){
	SampleSortTask this = (SampleSortTask)SampleSortTaskRef;
	start(this);
	pthread_exit(NULL);
}

static void send(SampleSortTask this, Message data, int targetID){
	Comm->send(Comm, data, targetID);
}

#endif /* SAMPLESORTTASK_GENERATED_H_ */

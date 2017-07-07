
#ifndef SAMPLESORTTASK_GENERATED_H_
#define SAMPLESORTTASK_GENERATED_H_

#include <stdio.h>
#include <pthread.h>

/******************************
 *  System generated code
 ******************************/

static void send(SampleSortTask this, Message data, int targetTaskID);
static void receive(SampleSortTask this);

static void message_notify(SampleSortTask this);
static void message_wait(SampleSortTask this);
static int message_immediate(SampleSortTask this);

static void repository_set_name(SampleSortTask this, char name[MAX_NAME_SIZE]);
static int repository_get_id(SampleSortTask this, char task_name[MAX_NAME_SIZE]);

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

static void message_notify(SampleSortTask this) {
	Comm->message_notify(Comm, this->taskID);
}
static void message_wait(SampleSortTask this) {
	Comm->message_wait(Comm, this->taskID);
}
static int message_immediate(SampleSortTask this) {
	return Comm->message_immediate(Comm, this->taskID);
}
static void repository_set_name(SampleSortTask this, char name[MAX_NAME_SIZE]) {
	Comm->repository_set_name(Comm, name, this->taskID);
}
static int repository_get_id(SampleSortTask this, char task_name[MAX_NAME_SIZE]) {
	return Comm->repository_get_id(Comm, task_name);
}

#endif /* SAMPLESORTTASK_GENERATED_H_ */


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

// The SampleSortTask "class"
struct SampleSortTask {

	// private data members
	pthread_t threadRef;
	int taskID;
};

// The SampleSortTask "constructor"
unsigned int SampleSortTask_create(){

	SampleSortTask newRec = malloc(sizeof(struct SampleSortTask));
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

static void *run(void *SampleSortTaskRef){
	SampleSortTask this = (SampleSortTask)SampleSortTaskRef;
	start(this);
	pthread_exit(NULL);
}

static void send(SampleSortTask this, Message data, int targetID){
	Comm->send(data, targetID);
}

#endif /* SAMPLESORTTASK_GENERATED_H_ */

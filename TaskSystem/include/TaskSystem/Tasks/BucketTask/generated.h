
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

// The BucketTask "class"
struct BucketTask {

	// private data members
	pthread_t threadRef;
	int taskID;
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
	Comm->send(data, targetID);
}

#endif /* BUCKETTASK_GENERATED_H_ */

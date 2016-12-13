
#ifndef PRODUCERTASK_GENERATED_H_
#define PRODUCERTASK_GENERATED_H_

#include <stdio.h>
#include <pthread.h>

/******************************
 *  System generated code
 ******************************/

static void send(ProducerTask this, Message data, int targetTaskID);
static void receive(ProducerTask this);

static void *run(void *ProducerTaskRef);
static void start(ProducerTask this);

static void handle_BarMsg(ProducerTask this, BarMsg barMsg);

// The ProducerTask "class"
struct ProducerTask {

	// private data members
	pthread_t threadRef;
	int taskID;

};

// The ProducerTask "constructor"
unsigned int ProducerTask_create(){

	ProducerTask newRec = malloc(sizeof(struct ProducerTask));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in ProducerTask_create");

	newRec->taskID = Comm->getNextTaskID();
	Comm->createMsgQ(newRec->taskID);

	int result = pthread_create( &(newRec->threadRef), NULL, run, (void *)newRec);
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", result);
	    exit(-1);
	}

	return newRec->taskID;
}

static void *run(void *ProducerTaskRef){
	ProducerTask this = (ProducerTask)ProducerTaskRef;
	start(this);
	pthread_exit(NULL);
}

static void send(ProducerTask this, Message data, int targetID){
	Comm->send(data, targetID);
}

#endif /* PRODUCERTASK_GENERATED_H_ */

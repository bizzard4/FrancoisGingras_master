
#ifndef CONSUMERTASK_GENERATED_H_
#define CONSUMERTASK_GENERATED_H_

#include <stdio.h>
#include <pthread.h>

/******************************
 *  System generated code
 ******************************/

static void send(ConsumerTask this, Message data, int targetTaskID);
static void receive(ConsumerTask this);

static void *run(void *ConsumerTaskRef);
static void start(ConsumerTask this);

static void handle_ProduceMsg(ConsumerTask this, ProduceMsg produceMsg);

// The ConsumerTask "class"
struct ConsumerTask {

	// private data members
	pthread_t threadRef;
	int taskID;

};

// The ConsumerTask "constructor"
unsigned int ConsumerTask_create(){

	ConsumerTask newRec = malloc(sizeof(struct ConsumerTask));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in ConsumerTask_create");

	newRec->taskID = Comm->getNextTaskID(Comm);
	Comm->createMsgQ(Comm, newRec->taskID);

	int result = pthread_create( &(newRec->threadRef), NULL, run, (void *)newRec);
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", result);
	    exit(-1);
	}

	return newRec->taskID;
}

static void *run(void *ConsumerTaskRef){
	ConsumerTask this = (ConsumerTask)ConsumerTaskRef;
	start(this);
	pthread_exit(NULL);
}

static void send(ConsumerTask this, Message data, int targetID){
	Comm->send(Comm, data, targetID);
}

#endif /* CONSUMERTASK_GENERATED_H_ */


#ifndef DIRECTMISSSENDTASK_GENERATED_H_
#define DIRECTMISSSENDTASK_GENERATED_H_

#include <stdio.h>
#include <pthread.h>

/******************************
 *  System generated code
 ******************************/

static void send(DirectMissSendTask this, Message data, int targetTaskID);
static void receive(DirectMissSendTask this);

//static void *run(void *PapaTaskRef);
static void *run(void *DirectMissSendTaskRef);
static void start(DirectMissSendTask this);

// The DirectMissSendTask "class"
struct DirectMissSendTask {

	// private data members
	pthread_t threadRef;
	int taskID;

};

// The DirectMissSendTask "constructor"
unsigned int DirectMissSendTask_create(){

	DirectMissSendTask newRec = malloc(sizeof(struct DirectMissSendTask));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in DirectMissSendTask_create");

	newRec->taskID = Comm->getNextTaskID(Comm);
	Comm->createMsgQ(Comm, newRec->taskID);

	int result = pthread_create( &(newRec->threadRef), NULL, run, (void *)newRec);
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", result);
	    exit(-1);
	}

	return newRec->taskID;
}

// a "wrapper" function that hides the complexity of the
// the pthread initialization. Ultimately, it just calls the programmer's
// start function.
static void *run(void *DirectMissSendTaskRef){

	DirectMissSendTask this = (DirectMissSendTask)DirectMissSendTaskRef;
	start(this);
	pthread_exit(NULL);
}

// The send method can be generated by the system since it always
// does the same thing
static void send(DirectMissSendTask this, Message data, int targetID){
	Comm->send(Comm, data, targetID);
}

#endif /* DIRECTMISSSENDTASK_GENERATED_H_ */

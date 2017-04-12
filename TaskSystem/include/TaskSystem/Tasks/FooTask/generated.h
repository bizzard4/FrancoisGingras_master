
#ifndef FOOTASK_GENERATED_H_
#define FOOTASK_GENERATED_H_


/******************************
 *  System generated code
 ******************************/


#include <stdio.h>
#include <pthread.h>


static void send(FooTask this, Message data, int targetFooTaskID);
static void receive(FooTask this);

static void *run(void *FooTaskRef);
static void start(FooTask this);

static void handle_TextMsg(FooTask this, TextMsg TextMsg);
static void handle_BarMsg(FooTask this, BarMsg barMsg);


// The FooTask "class"
struct FooTask {

	// private data members
	pthread_t threadRef;
	int taskID;

};


// The FooTask "constructor"
unsigned int FooTask_create(){

	FooTask newRec = malloc(sizeof(struct FooTask));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in FooTask_create");

	newRec->taskID = Comm->getNextTaskID(Comm);
	Comm->createMsgQ(Comm, newRec->taskID);

	int result = pthread_create( &(newRec->threadRef), NULL, run, (void *)newRec);
	//int result = pthread_create( &(newRec->threadRef), NULL, run, NULL);
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", result);
	    exit(-1);
	}

	return newRec->taskID;
}


// a "wrapper" function that hides the complexity of the
// the pthread initialization. Ultimately, it just calls the programmer's
// start function.
static void *run(void *FooTaskRef){
	FooTask this = (FooTask)FooTaskRef;
	start(this);
	pthread_exit(NULL);
}


// The send method can probably be generated by the system since it always
// does the same thing
static void send(FooTask this, Message data, int targetFooTaskID){
	Comm->send(Comm, data, targetFooTaskID);
}


/***************************************************************/







#endif /* FOOTASK_GENERATED_H_ */

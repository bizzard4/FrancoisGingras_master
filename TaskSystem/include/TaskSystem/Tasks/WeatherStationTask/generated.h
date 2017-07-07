
#ifndef WEATHERSTATIONTASK_GENERATED_H_
#define WEATHERSTATIONTASK_GENERATED_H_


#include <stdio.h>
#include <pthread.h>

#include "TaskSystem/Messages/TextMsg/TextMsg.h"

/******************************
 *  System generated code
 ******************************/

static void send(WeatherStationTask this, Message data, int targetTaskID);
static void receive(WeatherStationTask this);

static void message_notify(WeatherStationTask this);
static void message_wait(WeatherStationTask this);
static int message_immediate(WeatherStationTask this);

static void repository_set_name(WeatherStationTask this, char name[MAX_NAME_SIZE]);
static int repository_get_id(WeatherStationTask this, char task_name[MAX_NAME_SIZE]);

static void *run(void *WeatherStationTaskRef);
static void start(WeatherStationTask this);

static void handle_WeatherStationNameMsg(WeatherStationTask this, TextMsg textMsg);
static void handle_SubMsg(WeatherStationTask this, BarMsg barMsg);


// The WeatherStationTask "class"
struct WeatherStationTask {

	// private data members
	pthread_t threadRef;
	int taskID;

	int close_order;
	char station_name[MAX_NAME_SIZE];

	// Subcriber
	int sub_count;
	int subs[50]; // Max of 50 sub
};


// The WeatherStationTask "constructor"
unsigned int WeatherStationTask_create(){

	WeatherStationTask newRec = malloc(sizeof(struct WeatherStationTask));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in DatabaseTask_create");

	newRec->taskID = Comm->getNextTaskID(Comm);
	Comm->createMsgQ(Comm, newRec->taskID);

	newRec->close_order = 0;
	newRec->sub_count = 0;

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
static void *run(void *WeatherStationTaskRef){
	WeatherStationTask this = (WeatherStationTask)WeatherStationTaskRef;
	start(this);
	pthread_exit(NULL);
}


// The send method can be generated by the system since it always
// does the same thing
static void send(WeatherStationTask this, Message data, int targetTaskID){
	Comm->send(Comm, data, targetTaskID);
}

static void message_notify(WeatherStationTask this) {
	Comm->message_notify(Comm, this->taskID);
}
static void message_wait(WeatherStationTask this) {
	Comm->message_wait(Comm, this->taskID);
}
static int message_immediate(WeatherStationTask this) {
	return Comm->message_immediate(Comm, this->taskID);
}
static void repository_set_name(WeatherStationTask this, char name[MAX_NAME_SIZE]) {
	Comm->repository_set_name(Comm, name, this->taskID);
}
static int repository_get_id(WeatherStationTask this, char task_name[MAX_NAME_SIZE]) {
	return Comm->repository_get_id(Comm, task_name);
}

#endif /* WEATHERSTATIONTASK_GENERATED_H_ */

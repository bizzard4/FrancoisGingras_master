
#ifndef WEATHERSTATIONTASK_GENERATED_H_
#define WEATHERSTATIONTASK_GENERATED_H_


#include <stdio.h>
#include <pthread.h>

#include "TaskSystem/Messages/TextMsg/TextMsg.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h"

/******************************
 *  System generated code
 ******************************/

static int send(WeatherClientTask this, Message data, int targetTaskID);
static void receive(WeatherClientTask this);

static void message_notify(WeatherClientTask this);
static void message_wait(WeatherClientTask this);
static int message_immediate(WeatherClientTask this);

static void repository_set_name(WeatherClientTask this, char name[MAX_NAME_SIZE]);
static int repository_get_id(WeatherClientTask this, char task_name[MAX_NAME_SIZE]);

static void *run(void *WeatherClientTaskRef);
static void start(WeatherClientTask this);

static void handle_WeatherStationNameMsg(WeatherClientTask this, TextMsg textMsg);
static void handle_PostWeatherMsg(WeatherClientTask this, IntArrayMsg intArrayMsg);


// The WeatherClientTask "class"
struct WeatherClientTask {

	// private data members
	pthread_t threadRef;
	int taskID;

	int close_order;
	char station_name[MAX_NAME_SIZE];
};


// The WeatherClientTask "constructor"
unsigned int WeatherClientTask_create(){

	WeatherClientTask newRec = malloc(sizeof(struct WeatherClientTask));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in DatabaseTask_create");

	newRec->taskID = Comm->getNextTaskID(Comm);
	Comm->createMsgQ(Comm, newRec->taskID);

	newRec->close_order = 0;

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
static void *run(void *WeatherClientTaskRef){
	WeatherClientTask this = (WeatherClientTask)WeatherClientTaskRef;
	start(this);
	pthread_exit(NULL);
}


// The send method can be generated by the system since it always
// does the same thing
static int send(WeatherClientTask this, Message data, int targetTaskID){
	return Comm->send(Comm, data, targetTaskID);
}

static void message_notify(WeatherClientTask this) {
	Comm->message_notify(Comm, this->taskID);
}
static void message_wait(WeatherClientTask this) {
	Comm->message_wait(Comm, this->taskID);
}
static int message_immediate(WeatherClientTask this) {
	return Comm->message_immediate(Comm, this->taskID);
}
static void repository_set_name(WeatherClientTask this, char name[MAX_NAME_SIZE]) {
	Comm->repository_set_name(Comm, name, this->taskID);
}
static int repository_get_id(WeatherClientTask this, char task_name[MAX_NAME_SIZE]) {
	return Comm->repository_get_id(Comm, task_name);
}

#endif /* WEATHERSTATIONTASK_GENERATED_H_ */

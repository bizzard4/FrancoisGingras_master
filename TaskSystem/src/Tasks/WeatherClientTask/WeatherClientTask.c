#include "TaskSystem/Tasks/WeatherClientTask/WeatherClientTask.h"

#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/WeatherClientTask/generated.h"


/******************************
 * Programmer Code
 *****************************/

enum {WEATHER_STATION_NAME_MSG, SUB_MSG, POST_WEATHER_MSG};

int done;

/*
 * This is the "main" method for the thread
 */
static void start(WeatherClientTask this){
	printf("Weather client task has been started\n");

	// Receive station name
	receive(this);

	// Get id from repository
	int station_id = repository_get_id(this, this->station_name);
	printf("Weather client got station id %d\n", station_id);

	// Sub to the weather station
	BarMsg sub_msg = BarMsg_create(SUB_MSG);
	sub_msg->setValue(sub_msg, this->taskID);
	send(this, (Message)sub_msg, station_id);
	sub_msg->destroy(sub_msg);

	// Receive broadcasting
	while (1) {
		// Receive a broadcast
		receive(this);
	}

	sleep(1); // Safety, if the main process close before the last message is read from 
	// the client, then we enter in a infinite loop
	done = 1;
}

static void receive(WeatherClientTask this){
	int tag = Comm->getMsgTag(Comm, this->taskID);
	while (tag < 0) {
		tag = Comm->getMsgTag(Comm, this->taskID);
	}

	Message msg;

	// match the message to the right message "handler"
	switch (tag) {
	case WEATHER_STATION_NAME_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_WeatherStationNameMsg(this, (TextMsg)msg);
		break;
	case SUB_MSG: // Should never receive this
		break;
	case POST_WEATHER_MSG: // Should never receive this
		msg = Comm->receive(Comm, this->taskID);
		handle_PostWeatherMsg(this, (BarMsg)msg);
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(Comm, this->taskID);
	}
}

static void handle_WeatherStationNameMsg(WeatherClientTask this, TextMsg textMsg) {
	printf("Weather client received station name : %s\n", textMsg->getMsg(textMsg));
	strcpy(this->station_name, textMsg->getMsg(textMsg));
}

static void handle_PostWeatherMsg(WeatherClientTask this, BarMsg barMsg) {
	// TODO : Receive weather station id as a reference
	printf("Weather %d C\n", barMsg->getValue(barMsg));
}
#include "TaskSystem/Tasks/WeatherStationTask/WeatherStationTask.h"

#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// this file contains code that the language compiler/runtime
// would generated automatically
#include "TaskSystem/Tasks/WeatherStationTask/generated.h"


/******************************
 * Programmer Code
 *****************************/

enum {WEATHER_STATION_NAME_MSG, SUB_MSG, POST_WEATHER_MSG};

int done;

/*
 * This is the "main" method for the thread
 */
static void start(WeatherStationTask this){
	printf("Weather station task has been started\n");

	// Receive weather station name
	receive(this);

	// Set the name
	repository_set_name(this, this->station_name);

	// Response to request count
	long loop_count = 0;
	while (this->close_order == 0) {
		// Two strategies could be used here.
		// 1) Process all messages before broadcasting
		// 2) Process only 1 messages per loop then broadcast after an amount of time
		// For now I will go with 2)

		// Look if message is present
		if (message_immediate(this) == 0) {
			receive(this);
		}

		// Broadcast every x loop_count (for simplicity)
		loop_count++;
		if ((loop_count % 10000000) == 0) {
			// Broadcast weather to all subscriber
			BarMsg temp_msg = BarMsg_create(POST_WEATHER_MSG);
			temp_msg->setValue(temp_msg, 31); // 31 degree let say
			for (int i = 0; i < this->sub_count; i++) {
				send(this, (Message)temp_msg, this->subs[i]);
			}
			temp_msg->destroy(temp_msg);

			loop_count = 0;
		}


	}

	sleep(1); // Safety, if the main process close before the last message is read from 
	// the client, then we enter in a infinite loop
	done = 1;
}

static void receive(WeatherStationTask this){
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
	case SUB_MSG:
		msg = Comm->receive(Comm, this->taskID);
		handle_SubMsg(this, (BarMsg)msg);
		break;
	case POST_WEATHER_MSG: // Should never receive this
		break;
	default:
		printf("\nTask %d No Handler for tag = %d, dropping message! \n", this->taskID, tag);
		Comm->dropMsg(Comm, this->taskID);
	}
}

static void handle_WeatherStationNameMsg(WeatherStationTask this, TextMsg textMsg) {
	printf("Weather station received name : %s\n", textMsg->getMsg(textMsg));
	strcpy(this->station_name, textMsg->getMsg(textMsg));
}

static void handle_SubMsg(WeatherStationTask this, BarMsg barMsg) {
	int new_sub_id = barMsg->getValue(barMsg);
	printf("Weather station new subcriber id : %d\n", new_sub_id);

	if (this->sub_count < 50) {
		this->subs[this->sub_count] = new_sub_id;
		this->sub_count++;
	} else {
		printf("Weather station is at max sub count.\n");
	}
}


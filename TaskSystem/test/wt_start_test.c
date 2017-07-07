#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/System.h"
#include "test_util.h"
#include "TaskSystem/Tasks/WeatherStationTask/WeatherStationTask.h"
#include "TaskSystem/Messages/TextMsg/TextMsg.h"

System Comm;

// To close the test case
int done;

enum {WEATHER_STATION_NAME_MSG, SUB_MSG, POST_WEATHER_MSG};

/**
 * Database server test case
 */
int main(int argc, char *argv[]) {
	done = 0;

	// Read the station name
	char* station_name;
	if (argc == 2) {
		station_name = argv[1];
	} else {
		printf("Usage : wt_start_test station_name\n");
		exit(-1);
	}


	// Database initialize the system
	// TODO: Acquire if existing
	Comm = System_create();
	printf("System addr %p\n", Comm);
	printf("System shared data addr %p\n", Comm->data);

	unsigned int weather_station_task = WeatherStationTask_create();
	printf("Weather station task id = %d\n", weather_station_task);

	// Send station name
	TextMsg station_name_msg = TextMsg_create(WEATHER_STATION_NAME_MSG);
	station_name_msg->setMsg(station_name_msg, station_name);
	Comm->send(Comm, (Message)station_name_msg, weather_station_task);
	station_name_msg->destroy(station_name_msg);

	while(done == 0);

	// Destroy the shared memory, if this is done before a client is done
	// the client will have error
	// TODO : If acquired, do not destroy
	Comm->destroy(Comm);

	return EXIT_SUCCESS;
}
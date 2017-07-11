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
	int acquire_system;
	if (argc == 3) {
		station_name = argv[1];
		acquire_system = atoi(argv[2]);
	} else {
		printf("Usage : wt_start_test station_name [0/1] \n");
		exit(-1);
	}


	// Database initialize the system
	// Because system do not have any try_acquire capability, I pass
	// to the test argument the instruction to acquire or create.
	if (acquire_system == 0) {
		Comm = System_create();
	} else {
		Comm = System_acquire();
	}
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
	if (acquire_system == 0) {
		Comm->destroy(Comm);
	}

	return EXIT_SUCCESS;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/System.h"
#include "test_util.h"
#include "TaskSystem/Tasks/DatabaseTask/DatabaseTask.h"

System Comm;

/**
 * Database server test case
 */
int main(int argc, char *argv[]) {

	// Database initialize the system
	Comm = System_acquire();

	int next_id = Comm->data->nextTaskID;
	printf("Data next id = %d\n", next_id);

	next_id = Comm->getNextTaskID(Comm);
	printf("Function next id = %d\n", next_id);

	//unsigned int server_task = DatabaseTask_create();
	//printf("Database server task id = %d\n", server_task);

	sleep(1);

	//Comm->destroy(Comm);

	return EXIT_SUCCESS;
}
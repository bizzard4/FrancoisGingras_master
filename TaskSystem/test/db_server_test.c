#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/System.h"
#include "test_util.h"
#include "TaskSystem/Tasks/DatabaseTask/DatabaseTask.h"

System Comm;

// To close the test case
int done; 

/**
 * Database server test case
 */
int main(int argc, char *argv[]) {
	done = 0;

	// Database initialize the system
	Comm = System_create();
	printf("System addr %p\n", Comm);
	printf("System shared data addr %p\n", Comm->data);

	unsigned int server_task = DatabaseTask_create();
	printf("Database server task id = %d\n", server_task);

	while(done == 0);

	// Destroy the shared memory, if this is done before a client is done
	// the client will have error
	Comm->destroy(Comm);

	return EXIT_SUCCESS;
}
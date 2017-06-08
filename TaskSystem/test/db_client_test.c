#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/System.h"
#include "test_util.h"
#include "TaskSystem/Tasks/ClientTask/ClientTask.h"

System Comm;

// To close the test case at the end
int done;
int nb_request;
int req_type;

/**
 * Database server test case
 */
int main(int argc, char *argv[]) {
	done = 0;

	// Read request_count
	if (argc != 3) {
		printf("Usage : db_client_test N REQ_TYPE\n");
		return -1;
	}
	nb_request = atoi(argv[1]);
	req_type = atoi(argv[2]);
	printf("Client started with nb_request=%d\n", nb_request);

	// Database initialize the system
	Comm = System_acquire();
	printf("System addr %p\n", Comm);
	printf("System shared data addr %p\n", Comm->data);

	unsigned int client_task = ClientTask_create();
	printf("Client server task id = %d\n", client_task);

	while(done == 0);

	return EXIT_SUCCESS;
}
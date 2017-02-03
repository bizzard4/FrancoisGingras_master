#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/Tasks/SampleSortTask/SampleSortTask.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h"
#include "TaskSystem/System.h"
#include "test_util.h"

System Comm;

int done = 0; // Global variable for testing purpose

// Messages
enum {TOPOLOGY_MSG, INTARRAY_MSG};

/**
 * Sample sort test case.
 */

int main(void) {

	Comm = System_create();
	if (Comm == NULL) {
		TS_ERROR("Comm==null");
	}

	// Create the samplesort task
	unsigned int samplesort = SampleSortTask_create();


	// Send test data
	IntArrayMsg data_msg = IntArrayMsg_create(INTARRAY_MSG);
	int test_data[10] = {2, 5, 8, 6, 3, 1, 7, 9, 10, 4};
	data_msg->setValues(data_msg, 10, test_data);
	Comm->send((Message)data_msg, samplesort);
	data_msg->destroy(data_msg);

	// Without mecanic to wait on a task, we will use this temporary global variable.
	while (done == 0);

	// Clean
	Comm->destroy(Comm);

	return EXIT_SUCCESS;
}

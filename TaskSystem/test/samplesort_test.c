#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/Tasks/SampleSortTask/SampleSortTask.h"
#include "TaskSystem/System.h"
#include "test_util.h"

System Comm;

int done = 0; // Global variable for testing purpose

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

	// Without mecanic to wait on a task, we will use this temporary global variable.
	while (done == 0);

	// Clean
	Comm->destroy(Comm);

	return EXIT_SUCCESS;
}

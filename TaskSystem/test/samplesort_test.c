#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/Tasks/SampleSortTask/SampleSortTask.h"
#include "TaskSystem/System.h"
#include "test_util.h"

System Comm;

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

	// TODO : Wait for the task to finish (need some sort of signal)

	// Clean
	Comm->destroy(Comm);

	return EXIT_SUCCESS;
}

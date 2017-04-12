#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/Tasks/PapaTask/PapaTask.h"
#include "TaskSystem/System.h"
#include "test_util.h"

System Comm;

/**
 * Unit test for system component.
 */

int main(void) {

	// Create the system
	Comm = System_create();

	// Comm should be created
	if (Comm == NULL) {
		TS_ERROR("Comm==null");
	}

	// Test next id
	int next_id = Comm->getNextTaskID(Comm);
	if (next_id != 1) {
		TS_ERROR("Wrong next_id");
	}

	printf("Success\n");

	// Clean
	Comm->destroy(Comm);

	return EXIT_SUCCESS;
}

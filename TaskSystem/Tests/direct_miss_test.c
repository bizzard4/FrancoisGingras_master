#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "System.h"
#include "DirectMissSendTask.h"
#include "test_util.h"

System Comm;

/**
 * Test a direct missed message.
 *
 * This test show how easily it is to miss a message using a direct receive method.
 */

int main(void) {

	// Create the system
	Comm = System_create();
	if (Comm == NULL) {
		TS_ERROR("Comm==null");
	}

	unsigned int sender = DirectMissSendTask_create();

	// Delay to let all test finish
	sleep(3);

	return EXIT_SUCCESS;
}

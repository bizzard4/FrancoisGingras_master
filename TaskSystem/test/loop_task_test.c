#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/System.h"
#include "TaskSystem/Tasks/LoopSendTask/LoopSendTask.h"
#include "test_util.h"

System Comm;

/**
 * Test a loop receive method. Receiver will loop to check if new message has arrive and process them.
 * 
 * This test will success if all message has been received.
 *
 */

int main(void) {

	// Create the system
	Comm = System_create();
	if (Comm == NULL) {
		TS_ERROR("Comm==null");
	}

	unsigned int sender = LoopSendTask_create();

	// Delay to let all test finish
	sleep(3);

	return EXIT_SUCCESS;
}

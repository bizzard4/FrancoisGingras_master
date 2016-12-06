#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "System.h"
#include "DirectSendTask.h"
#include "test_util.h"

System Comm;

/**
 * Test a direct receive method. Direct receive is when the task call the receive method while not being in a loop.
 *
 * Task list:
 * DirectSendTask : Parent task that create the DirectReceiverTask and send it a message directly.
 * DirectReceiveTask : Child task who will receive the message.
 * 
 * This test will success if "send" from parent is done before the "receive" is called. See direct_receive_miss_test for 
 * an example where the child will miss the message.
 */

int main(void) {

	// Create the system
	Comm = System_create();
	if (Comm == NULL) {
		TS_ERROR("Comm==null");
	}

	printf("Creating task\n");
	DirectSendTask sender = DirectSendTask_create();
	if (sender == NULL) {
		TS_ERROR("sender==null");
	}

	// Delay to let all test finish
	sleep(2);

	return EXIT_SUCCESS;
}

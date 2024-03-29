#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/Tasks/PapaTask/PapaTask.h"
#include "TaskSystem/System.h"
#include "test_util.h"

System Comm;

/**
 * Simple test showing the execution of a simple task.
 */

int main(void) {

	Comm = System_create();
	printf("Hello task system\n");

	// create parent task. It will start running immediately.
	unsigned int taskX = PapaTask_create();

	// Delay to let all test finish
	sleep(2);

	return EXIT_SUCCESS;
}

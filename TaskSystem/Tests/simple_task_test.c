#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "PapaTask.h"
#include "System.h"
#include "test_util.h"

System Comm;

int main(void) {

	Comm = System_create();

	// create parent task. It will start running immediately.
	PapaTask taskX = PapaTask_create();

	// Delay to let all test finish
	sleep(2);

	return EXIT_SUCCESS;
}

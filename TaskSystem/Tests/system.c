#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../PapaTask/PapaTask.h"
#include "../System/System.h"

System Comm;

int main(void) {

	Comm = System_create();

	puts("\nSimple Task test...\n");

	// create parent task. It will start running immediately.
	PapaTask taskX = PapaTask_create();

	// wait for the threads to do everything.
	// eventually, we should do a "thread join" type of wait
	sleep(2);

	puts("\n...successful test!!");

	return EXIT_SUCCESS;
}

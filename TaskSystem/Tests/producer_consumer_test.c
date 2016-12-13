#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "ConsumerTask.h"
#include "ProducerTask.h"
#include "BarMsg.h"
#include "System.h"
#include "test_util.h"

System Comm;

/**
 * Producer(n)-consumer(1) example using task.
 */

// message tags
enum {TEXT_MSG, BAR_MSG};

int main(void) {

	// Create the system
	Comm = System_create();

	// Comm should be created
	if (Comm == NULL) {
		TS_ERROR("Comm==null");
	}

	// Create the consumer
	unsigned int consumer = ConsumerTask_create();

	// Create n producers and send them the id of the consumer
	unsigned int producer_1 = ProducerTask_create();
	unsigned int producer_2 = ProducerTask_create();

	// Wrap consumer task id in a bar message
	BarMsg barMsg = BarMsg_create(BAR_MSG);
	barMsg->setValue(barMsg, consumer);
	printf("Sending a Consumer id(%d) message\n", consumer);
	Comm->send((Message)barMsg, producer_1);
	Comm->send((Message)barMsg, producer_2);
	barMsg->destroy(barMsg);

	sleep(3);

	// Clean
	Comm->destroy(Comm);

	return EXIT_SUCCESS;
}

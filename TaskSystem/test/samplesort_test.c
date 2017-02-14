#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "TaskSystem/Tasks/SampleSortTask/SampleSortTask.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h"
#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/System.h"
#include "test_util.h"

System Comm;

int done = 0; // Global variable for testing purpose

// Messages
enum {TOPOLOGY_MSG, INTARRAY_MSG, DONE_MSG, BAR_MSG};

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

	int k;
	scanf("%d", &k);



	// Send K size
	BarMsg k_msg = BarMsg_create(BAR_MSG);
	k_msg->setValue(k_msg, k);
	Comm->send((Message)k_msg, samplesort);
	k_msg->destroy(k_msg);

	int n;
	scanf("%d", &n);

	printf("Starting samplesort for K=%d and N=%d\n", k, n);

	int temp;
	int* test_data = malloc(n * sizeof(int));
	for (int i = 0; i < n; i++) { // Read n values
		scanf("%d", &temp);
		test_data[i] = temp;
	}


	// Send test data
	IntArrayMsg data_msg = IntArrayMsg_create(INTARRAY_MSG);

	data_msg->setValues(data_msg, n, test_data);
	Comm->send((Message)data_msg, samplesort);
	data_msg->destroy(data_msg);
	free(test_data);

	// Without mecanic to wait on a task, we will use this temporary global variable.
	while (done == 0);
	sleep(2);

	// Clean
	Comm->destroy(Comm);

	return EXIT_SUCCESS;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "TaskSystem/Tasks/SampleSortTask/SampleSortTask.h"
#include "TaskSystem/Messages/RefIntArrayMsg/RefIntArrayMsg.h"
#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/System.h"
#include "test_util.h"
#include "TaskSystem/TimeHelper.h"

System Comm;

int done = 0; // Global variable for testing purpose

// Messages (all)
enum {
	// Pre-phase
	BAR_MSG, 
	REF_INTARRAY_MSG,
	// Phase 1 
	TOPOLOGY_MSG, // topology msg
	DATA_REF_MSG, // int ref array message
	SPLITTER_MSG, // int array message
	// Phase 2
	GET_SUB_ARRAY_MSG, // done message (signal)
	SUB_ARRAT_MSG, // 2 dimension ref int array message
	// Phase 3
	SET_SUB_ARRAY_MSG, // 2 dimension ref int array message
	DONE_MSG // done message (singla)
};

/**
 * Sample sort test case.
 * K N as parameters
 */

int main(int argc, char *argv[]) {

	Comm = System_create();
	if (Comm == NULL) {
		TS_ERROR("Comm==null");
	}

	// Read N and K
	if (argc != 3) {
		printf("Usage : samplesort_test	K N\n");
		return -1;
	}
	int k = atoi(argv[1]);
	int n = atoi(argv[2]);

	printf("Starting samplesort for K=%d and N=%d\n", k, n);

	// Read N int
	struct timespec read_start, read_end;
	clock_gettime(CLOCK_MONOTONIC, &read_start);
	int temp;
	int* test_data = malloc(n * sizeof(int));
	for (int i = 0; i < n; i++) { // Read n values
		scanf("%d", &temp);
		test_data[i] = temp;
	}
	clock_gettime(CLOCK_MONOTONIC, &read_end);

	// Create the samplesort task
	struct timespec create_start, create_end;
	clock_gettime(CLOCK_MONOTONIC, &create_start);
	unsigned int samplesort = SampleSortTask_create();

	// Send K size
	BarMsg k_msg = BarMsg_create(BAR_MSG);
	k_msg->setValue(k_msg, k);
	Comm->send((Message)k_msg, samplesort);
	k_msg->destroy(k_msg);

	// Send test data
	RefIntArrayMsg data_msg = RefIntArrayMsg_create(REF_INTARRAY_MSG);
	data_msg->setValues(data_msg, n, test_data);
	Comm->send((Message)data_msg, samplesort);
	data_msg->destroy(data_msg);

	clock_gettime(CLOCK_MONOTONIC, &create_end);

	// Without mecanic to wait on a task, we will use this temporary global variable.
	struct timespec wait_start, wait_end;
	clock_gettime(CLOCK_MONOTONIC, &wait_start);
	while (done == 0);
	clock_gettime(CLOCK_MONOTONIC, &wait_end);

	// Clean
	Comm->destroy(Comm);

	struct timespec read_diff = diff(read_start, read_end);
	printf("TIMING-TEST %lds,%ldms - Read time\n", read_diff.tv_sec, read_diff.tv_nsec/1000000);

	struct timespec create_diff = diff(create_end, create_end);
	printf("TIMING-TEST %lds,%ldms - Task initialization time\n", create_diff.tv_sec, create_diff.tv_nsec/1000000);  

	struct timespec wait_diff = diff(wait_start, wait_end);
	printf("TIMING-TEST %lds,%ldms - Task wait on done time\n", wait_diff.tv_sec, wait_diff.tv_nsec/1000000); 
	return EXIT_SUCCESS;
}
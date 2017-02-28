#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <time.h>

#include "TaskSystem/Messages/RefIntArrayMsg/RefIntArrayMsg.h"
#include "TaskSystem/System.h"
#include "test_util.h"

int main() {

	// Create an array
	int* arr = malloc(20 * sizeof(int));
	for (int i = 0; i < 20; i++) {
		arr[i] = i;
	}

	// Create 2 messages with this array
	RefIntArrayMsg msg1 = RefIntArrayMsg_create();
	msg1->setValues(msg1, 20, arr);

	RefIntArrayMsg msg2 = RefIntArrayMsg_create();
	msg2->setValues(msg2, 20, arr);

	// Update 1 value in message 1 
	msg1->values[0] = 1000;

	// Should be updated in message 2
	if (msg2->values[0] != 1000) {
		TS_ERROR("Value at 0 != 1000\n");
	}

	printf("Success\n");

	return 0;
}
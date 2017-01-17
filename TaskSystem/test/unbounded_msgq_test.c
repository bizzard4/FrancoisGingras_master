#include <stdio.h>

#include "TaskSystem/System.h"
#include "test_util.h"
#include "TaskSystem/UnboundedMsgQ.h"
#include "TaskSystem/Messages/BarMsg/BarMsg.h"

System Comm;

// message tags
enum {TEXT_MSG, BAR_MSG};

/**
 * Test the unbounded message queue.
 */

int main(void) {

	Queue Q;
	Q = CreateQueue();
	if (Q==NULL) {
		TS_ERROR("Queue is null\n");
	}

	if (Peek(Q) != NULL) {
		TS_ERROR("Q should be empty\n");
	}

	if (IsEmpty(Q) == 0) {
		TS_ERROR("Q should be empty\n");
	}

	// Add a mesage
	BarMsg msg1 = BarMsg_create(BAR_MSG);
	msg1->setValue(msg1, 100);

	int res = Enqueue(Q, (Message)msg1);
	if (res == 0) {
		TS_ERROR("Error in Enqueue2 msg1\n");
	}

	if (IsEmpty(Q)==1) {
		TS_ERROR("Should not be empty\n");
	}

	Message peeked = Peek(Q);
	if (peeked != (Message)msg1) {
		TS_ERROR("Peek error 1\n");
	}

	BarMsg msg2 = BarMsg_create(BAR_MSG);
	msg2->setValue(msg2, 101);
	res = Enqueue(Q, (Message)msg2);
	if (res == 0) {
		TS_ERROR("Error in Enqueue2 msg2\n");
	}

	BarMsg msg3 = BarMsg_create(BAR_MSG);
	msg3->setValue(msg1, 102);
	res = Enqueue(Q, (Message)msg3);
	if (res == 0) {
		TS_ERROR("Error in Enqueue2 msg3\n");
	}

	peeked = Peek(Q);
	if (peeked != (Message)msg1) {
		TS_ERROR("Peek error 2\n");
	}

	Message dequeued = Dequeue(Q);
	if (dequeued != (Message)msg1) {
		TS_ERROR("Dequeue2 error\n");
	}

	dequeued = Dequeue(Q);
	if (dequeued != (Message)msg2) {
		TS_ERROR("Dequeue2 error\n");
	}

	dequeued = Dequeue(Q);
	if (dequeued != (Message)msg3) {
		TS_ERROR("Dequeue2 error\n");
	}

	if (IsEmpty(Q)==0) {
		TS_ERROR("Should be empty\n");
	}

	DeleteQueue(Q);

	printf("Success\n");

	return 0;
}

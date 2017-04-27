#include <stdio.h>
#include <pthread.h>

#include "test_util.h"
#include "TaskSystem/CircularMsgQ.h"
#include "TaskSystem/Messages/Message.h"
#include "TaskSystem/Messages/BarMsg/BarMsg.h"

/// Pre-def for testing purpose
struct Queue
{
	pthread_mutex_t QLock; // the Queue mutex

	unsigned int head;
	unsigned int tail;
	unsigned int size;
};


int main(void) {
	Queue q = CreateQueue("/TEST_CIRCULAR_MSG_Q");

	printf("IsEmpty=%d\n", IsEmpty(q));
	printf("Head=%d Tail=%d Size=%d\n", q->head, q->tail, q->size);

	// Enqueue a message
	BarMsg msg = BarMsg_create(5);
	msg->setValue(msg, 10);
	int res = Enqueue(q, (Message)msg);
	msg->destroy(msg);
	printf("Enqueue done with res=%d\n", res);

	// Peek
	BarMsg peeked = (BarMsg)Peek(q);
	printf("Peeked msg with tag=%d and value=%d\n", peeked->tag, peeked->value);

	// Dequeue
	BarMsg poped = (BarMsg)Dequeue(q);
	printf("Dequeued msg with tag=%d and value=%d\n", poped->tag, poped->value);
	printf("IsEmpty=%d\n", IsEmpty(q));

	// Dequeue on empty
	if (Dequeue(q) == NULL) {
		printf("Dequeued returned null\n");
	}

	// Try to enq and deq 100
	for (int i = 0; i < 10; i++) {
		BarMsg m = BarMsg_create(3);
		msg->setValue(msg, i);
		int res = Enqueue(q, (Message)msg);
		msg->destroy(msg);
	}

	for (int i = 0; i < 10; i++) {
		BarMsg deq = (BarMsg)Dequeue(q);
		printf("Dequeued msg with tag=%d and value=%d\n", deq->tag, deq->value);
	}

	// TODO : Test full Q + circularity

	DeleteQueue("/TEST_CIRCULAR_MSG_Q");

	printf("Success\n");
	return 0;
}
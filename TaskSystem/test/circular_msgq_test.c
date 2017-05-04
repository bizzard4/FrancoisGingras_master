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
	int rollover_position; // 1 if tail has rollver
};


int main(void) {
	Queue q = CreateQueue("/TEST_CIRCULAR_MSG_Q");

	printf("IsEmpty=%d\n", IsEmpty(q));
	printf("IsRollover=%d\n", q->rollover_position);
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
	if (peeked->value != 10) {
		printf("Error: wrong peeked value\n");
		return -1;
	}

	// Dequeue
	BarMsg poped = (BarMsg)Dequeue(q);
	printf("Dequeued msg with tag=%d and value=%d\n", poped->tag, poped->value);
	printf("IsEmpty=%d\n", IsEmpty(q));

	// Dequeue on empty
	if (Dequeue(q) != NULL) {
		printf("Error: dequeue should return null");
		return -1;
	}
	printf("Dequeued returned null\n");

	// Now head position should be at : 56 + 72 = 128
	printf("Head = %d Tail = %d\n", q->head, q->tail);
	if (q->head != 128) {
		printf("Error: wrong head position\n");
		return -1;
	}

	// Fill the array (add 13 887 bar msg)
	for (int i = 0; i < 13887; i++) {
		BarMsg m = BarMsg_create(3);
		msg->setValue(msg, i);
		int res = Enqueue(q, (Message)msg);
		msg->destroy(msg);
	}
	printf("Head = %d Tail = %d\n", q->head, q->tail);
	if (q->head != 128) {
	 	printf("Error: wrong head position\n");
	 	return -1;
	}
	if (q->tail != 999992) {
		printf("Error: wrong tail position\n");
		return -1;
	}

	// Now we should still be able to add one
	// The next one should be at the beginning
	BarMsg m = BarMsg_create(3);
	msg->setValue(msg, 100);
	res = Enqueue(q, (Message)msg);
	msg->destroy(msg);
	printf("Head = %d Tail = %d\n", q->head, q->tail);
	if (q->head != 128) {
	 	printf("Error: wrong head position\n");
	 	return -1;
	}
	if (q->tail != 128) {
		printf("Error: wrong tail position\n");
		return -1;
	}

	// Enqueue on full array
	msg = BarMsg_create(10);
	msg->setValue(msg, 555);
	res = Enqueue(q, (Message)msg);
	if (res == 1) {
		printf("Error: Array should be full\n");
		return -1;
	}
	msg->destroy(msg);
	printf("Enqueue done with res=%d\n", res);

	// Dequeue 13887 bar msg
	for (int i = 0; i < 13887; i++) {
		BarMsg deq = (BarMsg)Dequeue(q);
	}
	printf("Head = %d Tail = %d\n", q->head, q->tail);
	if (q->head != 999992) {
	 	printf("Error: wrong head position\n");
	 	return -1;
	}
	if (q->tail != 128) {
		printf("Error: wrong tail position\n");
		return -1;
	}

	// Dequeue the last one
	BarMsg deq = (BarMsg)Dequeue(q);
	if (deq == NULL) {
		printf("Error : Should not be null\n");
		return -1;
	}
	printf("Head = %d Tail = %d\n", q->head, q->tail);
	printf("IsEmpty=%d\n", IsEmpty(q));
	printf("IsRollover=%d\n", q->rollover_position);

	// Should be empty

	// for (int i = 0; i < 10; i++) {
	// 	BarMsg deq = (BarMsg)Dequeue(q);
	// 	printf("Dequeued msg with tag=%d and value=%d\n", deq->tag, deq->value);
	// }

	// TODO : Test full Q + circularity

	DeleteQueue("/TEST_CIRCULAR_MSG_Q");

	printf("Success\n");
	return 0;
}
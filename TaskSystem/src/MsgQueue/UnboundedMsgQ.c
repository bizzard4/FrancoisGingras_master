#include "TaskSystem/UnboundedMsgQ.h"
#include "TaskSystem/fatal.h"

#include <stdlib.h>
#include <pthread.h>

/* Unbounded message queue. Will send error in case of memory issues. 
 * Each message queue has his own mutex.
 */

struct Node
{
	ElementType Element;
	Position    Next;
};

struct Queue
{
	pthread_mutex_t QLock; // the Queue mutex

	Position head;
	Position tail;
	int size;
};

Queue CreateQueue() {
	Queue Q;

	Q = malloc( sizeof( struct Queue ) );
	if( Q == NULL )
		FatalError( "Out of space!!!" );

	Q->size = 0;
	Q->head = NULL;
	Q->tail = NULL;

	int result = pthread_mutex_init(&(Q->QLock), NULL); // init mutex

	return Q;
}

void DeleteQueue(Queue Q) {
	if (Q != NULL) {
		Node next = Q->head;
		while (next != NULL) {
			Node todel = next;
			next = next->Next;
			free(todel);
		}
		pthread_mutex_destroy(&(Q->QLock));
		free(Q);
	}
}

ElementType Peek(Queue Q) {
	ElementType toret = NULL;
	if ((Q != NULL) && (!IsEmpty(Q))) {
		pthread_mutex_lock (&(Q->QLock));
		toret = Q->head->Element;
		pthread_mutex_unlock (&(Q->QLock));
	}
	return toret;
}

int IsEmpty(Queue Q) {
	int toret = 0;
	if (Q != NULL) {
		pthread_mutex_lock (&(Q->QLock));
		toret = (Q->head == NULL);
		pthread_mutex_unlock (&(Q->QLock));
	}
	return toret;
}

int Enqueue(Queue Q, ElementType item) {
	Node N;

	if ((Q != NULL) && (item != NULL)) {	
		N = malloc(sizeof(struct Node));
		if (N == NULL) {
			FatalError("Out of space");
		}

		N->Element = item;
		N->Next = NULL;

		pthread_mutex_lock (&(Q->QLock));
		if (Q->head == NULL) { // Empty Q corner case
			Q->head = N;
			Q->tail = N;
		} else { // Not empty
			Q->tail->Next = N;
			Q->tail = N;
			Q->size++;
		}
		pthread_mutex_unlock (&(Q->QLock));
		return 1;
	}

	return 0;
}

ElementType Dequeue(Queue Q) {
	if ((Q != NULL) && (!IsEmpty(Q))) {
		pthread_mutex_lock (&(Q->QLock));
		Node deq_node = Q->head;
		Q->head = deq_node->Next;

		// If it was the last node
		if (Q->head == NULL) {
			Q->tail = NULL;
		}
		Q->size--;
		pthread_mutex_unlock (&(Q->QLock));

		ElementType toret = deq_node->Element;
		free(deq_node);
		return toret;
	}
	return 0;
}
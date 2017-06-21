#include "TaskSystem/CircularMsgQ.h"
#include "TaskSystem/fatal.h"

#include <stdlib.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

//#define DEBUG_CIRCULAR

/* Unbounded message queue. Will send error in case of memory issues. 
 * Each message queue has his own mutex.
 */

struct Queue
{
	pthread_mutex_t QLock; // the Queue mutex

	unsigned int head; // Offset of start addr
	unsigned int tail; // Offset of start addr
	unsigned int size;
	int rollover_position; // offset where tail went back to start, -1 = no rollover
};

int IsEmpty_not_safe(Queue Q);

Queue CreateQueue(const char* unique_name) {
	// Create the shard space
	// TODO : Use O_EXCL to detect existance
	int fd = shm_open(unique_name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		printf("ERROR; Error creating shared segment %d \n", errno);
		exit(-1);
	}

	if (ftruncate(fd, Q_SIZE) == -1) {
		printf("ERROR; Error truncating shared segment \n");
		exit(-1);
	}

	void* addr = mmap(NULL, Q_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		printf("ERROR; mmap failed %d\n", errno);
		exit(-1);	
	}

	Queue Q = (Queue)addr;

	Q->size = 0;
	Q->head = sizeof(struct Queue);
	Q->tail = sizeof(struct Queue);
	Q->rollover_position = -1;

	pthread_mutexattr_t m_attr;
	pthread_mutexattr_init(&m_attr);
	pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&(Q->QLock), &m_attr); // init mutex
	pthread_mutexattr_destroy(&m_attr);

	return Q;
}

Queue AcquireQueue(const char* unique_name) {
	// Acquire the shared space
	// TODO : Use O_EXCL to detect existance
	int fd = shm_open(unique_name, O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		printf("ERROR; Error acquire Q for task %d \n", errno);
		return NULL;
	}

	void* addr = mmap(NULL, Q_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		printf("ERROR; mmap failed acquiring Q for task %d\n", errno);
		return NULL;	
	}

	return (Queue)addr;
}

void DeleteQueue(const char* unique_name) {
	shm_unlink(unique_name);
}

ElementType Peek(Queue Q) {
	ElementType toret = NULL;
	pthread_mutex_lock (&(Q->QLock));
	if ((Q != NULL) && (!IsEmpty_not_safe(Q))) {
		int read_pos = Q->head;
		if (Q->head == Q->rollover_position) {
			read_pos = sizeof(struct Queue);
		}
		toret = (Message)((long)Q + read_pos);
	}
	pthread_mutex_unlock (&(Q->QLock));
	return toret;
}

int IsEmpty(Queue Q) {
	int toret = 0;
	if (Q != NULL) {
		pthread_mutex_lock (&(Q->QLock));
		toret = (Q->size == 0);
		pthread_mutex_unlock (&(Q->QLock));
	}
	return toret;
}

// Only use this one inside a lock
int IsEmpty_not_safe(Queue Q) {
	int toret = 0;
	if (Q != NULL) {
		toret = (Q->size == 0);
	}
	return toret;
}

int Enqueue(Queue Q, ElementType item) {
	if ((Q != NULL) && (item != NULL)) {	
		pthread_mutex_lock (&(Q->QLock));

		// Predict future tail position
		int future_tail = Q->tail + item->msg_size;
		int has_to_rollover = -1;
		if (Q->rollover_position == -1) {
			if (future_tail > Q_SIZE) {
				future_tail = sizeof(struct Queue) + item->msg_size;
				has_to_rollover = Q->tail;
#ifdef DEBUG_CIRCULAR
				printf("Cirvular array will rollover %p %d %d\n", Q, has_to_rollover, future_tail	);
#endif
			}

			// Full array protection in case of overflow
			if (has_to_rollover != -1) {
				if (future_tail > Q->head) {
					printf("Circular array full\n");
					pthread_mutex_unlock (&(Q->QLock));
					return 0;
				}
			}
		} else { // Already overflow
			if (future_tail > Q->head) {
					printf("Circular array full\n");
					pthread_mutex_unlock (&(Q->QLock));
					;return 0;
			}
		}

		// Write to future

		int size = item->writeAt(item, (void*)((long)Q + future_tail - item->msg_size));
		Q->tail = future_tail;
		if (has_to_rollover > 0) { // Only dequeue can set rollover back to -1
			Q->rollover_position = has_to_rollover; 
		}
		Q->size++;

		#ifdef DEBUG_CIRCULAR
			printf("ENQ Q=%p Msize=%d tail=%d head=%d size=%d rollover_position=%d\n", Q, item->msg_size, Q->tail, Q->head, Q->size, Q->rollover_position);
		#endif

		pthread_mutex_unlock (&(Q->QLock));


		return 1;
	}

	return 0;
}

ElementType Dequeue(Queue Q) {
	ElementType toret = NULL;

	pthread_mutex_lock (&(Q->QLock));
	if ((Q != NULL) && (!IsEmpty_not_safe(Q))) {
		// First we read the message part, and using the tid we map
		// to the right rebuilder method
		int read_pos = Q->head;
		if (Q->head == Q->rollover_position) {
			read_pos = sizeof(struct Queue);
			Q->rollover_position = -1;
		}
		
		Message message_part = (Message)(void*)((long)Q + read_pos);
		Q->head = read_pos + message_part->msg_size;
		Q->size--;

		#ifdef DEBUG_CIRCULAR
			printf("DEQ Q=%p Msize=%d tail=%d head=%d size=%d rollover_position=%d\n", Q, message_part->msg_size, Q->tail, Q->head, Q->size, Q->rollover_position);
		#endif
		
		toret = message_part;
	}
	pthread_mutex_unlock (&(Q->QLock));
	return toret;
}
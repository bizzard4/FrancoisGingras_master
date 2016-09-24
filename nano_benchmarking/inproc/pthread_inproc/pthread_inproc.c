#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "list.h"

struct timespec diff(struct timespec start, struct timespec end);

// Global linked list
List list;
Position pos;

// Parameters
int thread_count;
int update_count;
int update_size;

// Mutex
pthread_mutex_t mutex;


// Thread that send message to add to list
void* add_proc(void* x_void_ptr) {
	for (int i = 0; i < update_count; i++) {
		pthread_mutex_lock(&mutex);
		// Insert into linked list
		Insert(update_size, list, pos);
		pos = Advance(pos);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit((void*) 0);
}

int main(int argc, char* argv[]) {
	// argv
	thread_count = atoi(argv[1]);
	update_count = atoi(argv[2]);
	update_size = atoi(argv[3]);

	// Threads
	pthread_t threads[thread_count];
	pthread_mutex_init(&mutex, NULL);

	// Timing variables
	struct timespec start, end;

	// Prepare linked list
	list = MakeEmpty(NULL);
	pos = Header(list);

	clock_gettime(CLOCK_MONOTONIC, &start);
	// Create/start threads
	for (int i = 0; i < thread_count; i++) {
		if (pthread_create(&threads[i], NULL, add_proc, NULL)) {
			printf("Error creating client %d thread\n", i);
			return -1;
		}
	}

	// Wait for all client to be done
	for (int i = 0; i < thread_count; i++) {
		if (pthread_join(threads[i], NULL)) {
			printf("Error joining\n");
			return 0;
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("%ld\n", diff(start, end).tv_nsec/1000000); // Not working need 64 bits
	printf("%ld\n", diff(start, end).tv_sec);

	pthread_mutex_destroy(&mutex);
	return 0;
}

// Method to calculate difference between 2 time
struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

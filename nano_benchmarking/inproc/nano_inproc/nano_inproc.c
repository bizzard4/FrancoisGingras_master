#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "nn.h"
#include "pair.h"
#include "pipeline.h"
#include "list.h"

#define SOCKET_ADDRESS "inproc://fr_master1"

struct timespec diff(struct timespec start, struct timespec end);

// Global linked list
List list;
Position pos;

// Global server socket id
int server_socket;

// Global communication variables
int len;
char* data;

// Parameters
int thread_count;
int update_count;
int update_size;


// Thread that send message to add to list
void* send_proc(void* x_void_ptr) {
	int* socket_id_ptr = (int*)x_void_ptr;
	for (int i = 0; i < update_count; i++) {
		int e = nn_send((*socket_id_ptr), data, len, 0);
		if (e < 0) {
			printf("Thread sending error %d\n", e);
		}
	}
}

// Receiving thread to update list, no lock required because it is message passing
void* recv_proc(void* x_void_ptr) {
	char* buf = (char*)malloc(len + 1);
	int i = 0;
	while (i<(thread_count*update_count)) {
		int e = nn_recv(server_socket, buf, len + 1, 0);
		if (e < 0) {
			printf("Thread receiving error %d\n", e);
		}
		//printf("Thread received %s\n", buf);
		// Insert into linked list
		Insert(update_size, list, pos);
		pos = Advance(pos);

		i++;
	}
	free(buf);
}

int main(int argc, char* argv[]) {
	// argv
	thread_count = atoi(argv[1]);
	update_count = atoi(argv[2]);
	update_size = atoi(argv[3]);

	// Socket variable
	int client_socket[thread_count];

	// Threads
	pthread_t server_thread;
	pthread_t client_thread[thread_count];

	// Timing variables
	struct timespec start, end;

	// Prepare linked list
	list = MakeEmpty(NULL);
	pos = Header(list);

	// Prepare nano socket
	server_socket = nn_socket(AF_SP, NN_PULL);
	if (server_socket < 0) {
		printf("Error creating sa socket");
		return -1;
	}

	for (int i = 0; i < thread_count; i++) {
		client_socket[i] = nn_socket(AF_SP, NN_PUSH);
		if (client_socket[i] < 0) {
			printf("Error creating sb socket");
			return -1;
		}
	}

	// Connect them to the inproc addr
	if (nn_bind(server_socket, SOCKET_ADDRESS) < 1) {
		printf("Error on binding\n");
		return -1;
	}

	for (int i = 0; i < thread_count; i++) {
		int r = nn_connect(client_socket[i], SOCKET_ADDRESS); // Variable number of client
		if (r < 1) {
			printf("Error on connect\n");
			return -1;
		}
	}

	data = "data";
	len = strlen(data);

	clock_gettime(CLOCK_MONOTONIC, &start);
	// Create/start threads
	if (pthread_create(&server_thread, NULL, recv_proc, NULL)) {
		printf("Error creating server thread\n");
		return -1;
	}

	for (int i = 0; i < thread_count; i++) {
		if (pthread_create(&client_thread[i], NULL, send_proc, &client_socket[i])) {
			printf("Error creating client %d thread\n", i);
			return -1;
		}
	}

	// Wait for all client to be done
	for (int i = 0; i < thread_count; i++) {
		if (pthread_join(client_thread[i], NULL)) {
			printf("Error joining\n");
			return 0;
		}
	}

	pthread_join(server_thread, NULL);

	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("%ld\n", diff(start, end).tv_nsec/1000000); // Not working need 64 bits
	printf("%ld\n", diff(start, end).tv_sec);

	nn_shutdown(server_socket, 0);
	for (int i = 0; i < thread_count; i++) {
		nn_shutdown(client_socket[i], 0);
	}

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

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

#define THREAD_COUNT 30
#define UPDATE_COUNT 10000
#define UPDATE_SIZE 10 // Not implemented yet 

struct timespec diff(struct timespec start, struct timespec end);

// Global linked list
List list;
Position pos;

// Global server socket id
int server_socket;

// Global communication variables
int len;
char* data;

// Thread that send message to add to list
void* send_proc(void* x_void_ptr) {
	int* socket_id_ptr = (int*)x_void_ptr;
	printf("Client socket %d start sending\n", (*socket_id_ptr));
	for (int i = 0; i < UPDATE_COUNT; i++) {
		nn_send(1, data, len, 0);
	}
}

// Receiving thread to update list, no lock required because it is message passing
void* recv_proc(void* x_void_ptr) {
	char* buf = (char*)malloc(len + 1);
	int i = 0;
	while (i<(THREAD_COUNT*UPDATE_COUNT)) {
		int e = nn_recv(server_socket, buf, len + 1, 0);
		if (e < 0) {
			printf("Thread receiving error %d\n", e);
		}
		//printf("Thread received %s\n", buf);
		// Insert into linked list
		Insert(UPDATE_SIZE, list, pos);
		pos = Advance(pos);

		i++;
	}
	free(buf);
}

int main() {
	// Socket variable
	int client_socket[THREAD_COUNT];

	// Threads
	pthread_t server_thread;
	pthread_t client_thread[THREAD_COUNT];

	// Timing variables
	struct timespec pstart, pend, start, end;

	clock_gettime(CLOCK_MONOTONIC, &pstart);
	printf("Hello nanomsg\n");

	// Prepare linked list
	list = MakeEmpty(NULL);
	pos = Header(list);

	// Prepare nano socket
	server_socket = nn_socket(AF_SP, NN_PULL);
	if (server_socket < 0) {
		printf("Error creating sa socket");
		return -1;
	}
	printf("Server socket %d\n", server_socket);

	for (int i = 0; i < THREAD_COUNT; i++) {
		client_socket[i] = nn_socket(AF_SP, NN_PUSH);
		if (client_socket[i] < 0) {
			printf("Error creating sb socket");
			return -1;
		}
		printf("Socket %i %d\n", i, client_socket[i]);
	}

	// Connect them to the inproc addr
	if (nn_bind(server_socket, SOCKET_ADDRESS) < 1) {
		printf("Error on binding\n");
		return -1;
	}
	printf("Server binded\n");

	for (int i = 0; i < THREAD_COUNT; i++) {
		int r = nn_connect(client_socket[i], SOCKET_ADDRESS); // Variable number of client
		if (r < 1) {
			printf("Error on connect\n");
			return -1;
		}
	}
	printf("Clients connected\n");

	data = "data";
	len = strlen(data);

	clock_gettime(CLOCK_MONOTONIC, &start);
	// Create/start threads
	if (pthread_create(&server_thread, NULL, recv_proc, NULL)) {
		printf("Error creating server thread\n");
		return -1;
	}

	for (int i = 0; i < THREAD_COUNT; i++) {
		if (pthread_create(&client_thread[i], NULL, send_proc, &client_socket[i])) {
			printf("Error creating client %d thread\n", i);
			return -1;
		}
	}

	// Wait for all client to be done
	printf("Starting join\n");
	for (int i = 0; i < THREAD_COUNT; i++) {
		if (pthread_join(client_thread[i], NULL)) {
			printf("Error joining\n");
			return 0;
		}
	}
	printf("Join done\n");

	pthread_join(server_thread, NULL);

	clock_gettime(CLOCK_MONOTONIC, &end);
	printf("Communication elapse %ld\n", diff(start, end).tv_nsec/1000000);

	nn_shutdown(server_socket, 0);
	for (int i = 0; i < THREAD_COUNT; i++) {
		nn_shutdown(client_socket[i], 0);
	}

	clock_gettime(CLOCK_MONOTONIC, &pend);
	printf("Program elapse %ld\n", diff(pstart, pend).tv_nsec/1000000);

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

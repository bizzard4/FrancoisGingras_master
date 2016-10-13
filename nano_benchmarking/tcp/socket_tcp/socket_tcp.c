#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <resolv.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "list.h"

// Global linked list
List list;
Position pos;

// Global socket id
int sc;
int* clients;

// Global communication variables
int len;
char* data;

// Parameters
int thread_count;
int update_count;
int update_size;
char execution_mode;


// Thread that send message to add to list
void send_proc() {
	for (int i = 0; i < update_count; i++) {
		int e = write(sc, data, len);
		//printf("Has send %d\n", i); // THIS SLOW SEND AND MAKE ALL WORK
		//usleep(10000); // Need to sleep otherwise it will jam
		if (e <= 0) {
			printf("Thread sending error %d\n", e);
		}
	}
}

// Receiving thread to update list, no lock required because it is message passing
void recv_proc() {
	// First we wait for all connection
	for (int i = 0; i < thread_count; i++) {
		clients[i] = accept(sc, NULL, NULL);
		if (clients[i] < 0) {
			printf("Error accepting connexion\n");
			return;
		}
	}

	char* buf = (char*)malloc(len);
	int i = 0;
	while (i<(thread_count*update_count)) {
		for (int t = 0; t < thread_count; t++) {
			int e = read(clients[t], buf, len);
			if (e <= 0) {
				printf("Thread receiving error\n");
			}
			//printf("Thread received %d\n", i);
			//sched_yield();
			// Insert into linked list

			if (e > 0) {
				Insert(update_size, list, pos);
				pos = Advance(pos);

				i++;
			}
		}
	}
	free(buf);
}

int main(int argc, char* argv[]) {
	struct sockaddr_in addr;

	// argv
	thread_count = atoi(argv[1]);
	update_count = atoi(argv[2]);
	update_size = atoi(argv[3]);
	execution_mode = argv[4][0]; // c=client, s=server

	if ((execution_mode != 's') && (execution_mode != 'c')) {
		printf("Execution mode need to be s or c\n");
		return -1;
	}

	// Prepare linked list
	list = MakeEmpty(NULL);
	pos = Header(list);

	// Prepare nano socket
	if (execution_mode=='s') {
		sc = socket(AF_INET, SOCK_STREAM, 0);
		if (sc < 0) {
			printf("Error creating server socket\n");
			return -1;
		}
	} else {
		sc = socket(AF_INET, SOCK_STREAM, 0);
		if (sc < 0) {
			printf("Error creating client socket\n");
			return -1;
		}
	}
	if (setsockopt(sc, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
		printf("Error setting SO_REUSEADDR");
		return -1;
	}

	// Prepare address
	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	inet_aton(argv[5], &addr.sin_addr.s_addr);
	addr.sin_port = htons(6060); // hardcoded port, maybe need to change?

	// Connect them to the inproc addr
	int eid = 0;
	if (execution_mode=='s') {
		eid = bind(sc, (struct sockaddr*)&addr, sizeof(addr));
		if (eid < 0) {
			printf("Error on binding %s\n", strerror(errno));
			return -1;
		}
	} else {
		eid = connect(sc, (struct sockaddr*)&addr, sizeof(addr));
		if (eid < 0) {
			printf("Error on connect %s\n", strerror(errno));
			return -1;
		}
	}

	data = "data";
	len = strlen(data);

	// Do work
	if (execution_mode=='s') {
		clients = malloc(thread_count*sizeof(int));
		if (listen(sc, thread_count) < 0) {
			printf("Listen error\n");
			return -1;
		}
		recv_proc();
	} else {
		send_proc();
	}

	close(sc);

	return 0;
}

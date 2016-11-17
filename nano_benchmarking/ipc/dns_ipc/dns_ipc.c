#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
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

char *socket_path = "\0hidden";


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
	struct sockaddr_un addr;

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
		sc = socket(AF_UNIX, SOCK_STREAM, 0);
		if (sc < 0) {
			printf("Error creating server socket\n");
			return -1;
		}
	} else {
		sc = socket(AF_UNIX, SOCK_STREAM, 0);
		if (sc < 0) {
			printf("Error creating client socket\n");
			return -1;
		}
	}

	// Prepare address
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, argv[5], sizeof(addr.sun_path)-1);

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
		unlink(argv[5]);
	} else {
		send_proc();
	}

	usleep(200000);
	return 0;
}

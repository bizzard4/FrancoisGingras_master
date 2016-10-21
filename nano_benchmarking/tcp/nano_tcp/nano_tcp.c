#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "nn.h"
#include "pipeline.h"
#include "ipc.h"
#include "list.h"

// Global linked list
List list;
Position pos;

// Global socket id
int socket;

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
	int i = 0;
	while (i < update_count) {
		int bsend = nn_send(socket, data, len, 0);

		if (bsend < 0) {
			if (errno == EAGAIN) {
				printf("Cant send now, retrying\n");
				continue;
			} else {
				printf("Error : %d\n", errno);
				break;
			}
		} else if (bsend == 0) {
			printf("Connection lost\n");
			break;
		} else { // bsend > 0
			i++;
		}
	}
}

// Receiving thread to update list, no lock required because it is message passing
void recv_proc() {
	int i = 0;
	while (i<(thread_count*update_count)) {
		char* buf = (char*)malloc(len);
		int brecv = nn_recv(socket, buf, len, 0);

		if (brecv < 0) {
			if (errno == EAGAIN) {
				printf("Data not ready, waiting\n");
				continue;
			} else {
				printf("Error : %d\n", errno);
				break;
			}
		} else if (brecv == 0) {
			printf("Connection lost\n");
			break;
		} else { // brecv > 0
			//printf("Data #%d = %s\n", i, buf);
			Insert(update_size, list, pos);
			pos = Advance(pos);
			i++;
		}
		free(buf);
	}
}

int main(int argc, char* argv[]) {
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
		socket = nn_socket(AF_SP, NN_PULL);
		if (socket < 0) {
			printf("Error creating server socket\n");
			return -1;
		}
	} else {
		socket = nn_socket(AF_SP, NN_PUSH);
		if (socket < 0) {
			printf("Error creating client socket\n");
			return -1;
		}
	}

	// Connect them to the inproc addr
	int eid = 0;
	if (execution_mode=='s') {
		eid = nn_bind(socket, argv[5]);
		if (eid < 1) {
			printf("Error on binding %d\n", nn_errno());
			return -1;
		}
	} else {
		eid = nn_connect(socket, argv[5]);
		if (eid < 1) {
			printf("Error on connect\n");
			return -1;
		}
	}

	data = "data";
	len = strlen(data);

	// Do work
	if (execution_mode=='s') {
		recv_proc();
	} else {
		send_proc();
	}

	nn_shutdown(socket, eid);

	return 0;
}

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include "common.h"

int request_count; 
int nb_client;

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

/**
 * Server main.
 */
int main(int argc, char *argv[]) {
	// Read max_request
	if (argc >= 3) {
		request_count = atoi(argv[1]);
		nb_client = atoi(argv[2]);
	} else {
		printf("Usage : server req_count nb_client\n");
		exit(-1);
	}
	printf("Server started with req_count=%d nb_client=%d\n", request_count, nb_client);

	// Prepare the socket
	int sc = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sc < 0) {
		printf("Error creating server socket\n");
		exit(-1);
	}

	// Address
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, str_addr, sizeof(addr.sun_path)-1);

	// Bind the socket
	unlink(str_addr);
	int eid = bind(sc, (struct sockaddr*)&addr, sizeof(addr));
	if (eid < 0) {
		printf("Error on binding %s\n", strerror(errno));
		exit(-1);
	}

	// Wait for all connexion
	int clients[nb_client];
	if (listen(sc, nb_client) < 0) {
		printf("Listen error\n");
		exit(-1);
	}
	for (int i = 0; i < nb_client; i++) {
		clients[i] = accept(sc, NULL, NULL);
		if (clients[i] < 0) {
			printf("Error accepting connexion %s\n", strerror(errno));
			exit(-1);
		}
	}

	// Receive all messages
	int len = 4;
	char* buf = (char*)malloc(len);
	int req = 0;
	struct timespec get_start, get_end;
	printf("Start receiving\n");
	clock_gettime(CLOCK_MONOTONIC, &get_start);
	while (req < request_count) {
		for (int t = 0; t < nb_client; t++) {
			int e = read(clients[t], buf, len);
			//printf("Received %d bytes\n", e);
			if (e < 0) {
				printf("Thread receiving error %s\n", strerror(errno));
				exit(-1);
			}
			if (e == 0) {
				printf("EOF at %d\n", req);
				exit(-1);
			}

			req++;
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &get_end);
	printf("End receiving\n");

	printf("Server is done\n");

	struct timespec get_diff = diff(get_start, get_end);
	printf("S-GET %lds,%ldms - Server recv time\n", get_diff.tv_sec, get_diff.tv_nsec/1000000);

	unlink(str_addr);
	return 0;
}
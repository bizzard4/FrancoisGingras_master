#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

#include "common.h"

int request_count;

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
	if (argc >= 2) {
		request_count = atoi(argv[1]);
	} else {
		printf("Usage : client req_count\n");
		exit(-1);
	}
	printf("Client started with req_count=%d\n", request_count);

	// Prepare the socket
	int sc = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sc < 0) {
		printf("Error creating client socket\n");
		exit(-1);
	}

	// Address
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(addr));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, str_addr, sizeof(addr.sun_path)-1);

	// Connect the socket
	int eid = connect(sc, (struct sockaddr*)&addr, sizeof(addr));
	if (eid < 0) {
		printf("Error on connect %s\n", strerror(errno));
		return -1;
	}

	// Send request
	char* data = "data";
	int len = strlen(data);
	struct timespec send_start, send_end;
	printf("Start sending\n");
	clock_gettime(CLOCK_MONOTONIC, &send_start);
	for (int i = 0; i < request_count; i++) {
		// Send a request
		int e = send(sc, data, len, 0);
#ifdef VERBOSE
		printf("%d Send %d bytes\n", i, e);
#endif

		// Get a response
		if (e == len) {
			char buffer[4];
			int res_e = recv(sc, buffer, 4, 0);
#ifdef VERBOSE
			printf("%d Received %d bytes\n", i, res_e);
#endif
			if (res_e != len) {
				printf("Received bad response from server res_e=%d %s\n", res_e, strerror(errno));
				exit(-1);
			}
		}

		if (e <= 0) {
			printf("Send error\n");
			exit(-1);
		}
	}
	clock_gettime(CLOCK_MONOTONIC, &send_end);
	printf("End sending\n");

	printf("Client is done\n");

	struct timespec send_diff = diff(send_start, send_end);
	printf("C-SEND %lds,%ldms - Client send time\n", send_diff.tv_sec, send_diff.tv_nsec/1000000);

	return 0;
}
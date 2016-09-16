#include <stdio.h>
#include "nn.h"
#include "pair.h"

#define SOCKET_ADDRESS "inproc://fr_master1"

int main() {
	printf("Hello nanomsg\n");

	// Prepare nano socket
	int sa = nn_socket(AF_SP, NN_PAIR);
	if (sa < 0) {
		printf("Error creating sa socket");
		return -1;
	}
	printf("Socket a %d\n", sa);

	int sb = nn_socket(AF_SP, NN_PAIR);
	if (sb < 0) {
		printf("Error creating sb socket");
		return -1;
	}
	printf("Socket b %d\n", sb);

	// Connect them to the inproc addr
	int ra = nn_connect(sa, SOCKET_ADDRESS);
	int rb = nn_bind(sb, SOCKET_ADDRESS);
	if (ra < 1 || rb < 1) {
		printf("Error with bind/connect\n");
		return -1;
	}
	printf("Connected ra=%d rb=%d\n", ra, rb);

	// Send
	//char* data = "hello";
	//int len = strlen(data);
	//int senda = nn_send(sa, data, len, 0);

	//char* buf = malloc(len + 1);
	//int recvb = nn_recv(sb, buf, len + 1, 0);
	//printf("Send %d\n", senda);
	//printf("Receive %d\n", recvb);

	char* data = "data";
	int len = strlen(data);

	// Multiple send
	char* buf = (char*)malloc(len + 1);
	for (int i = 0; i < 10000; i++) {
		nn_send(sa, data, len, 0);
		nn_recv(sb, buf, len + 1, 0);
	}
	free(buf);

	nn_shutdown(sa, 0);
	nn_shutdown(sb, 0);

	return 0;
}
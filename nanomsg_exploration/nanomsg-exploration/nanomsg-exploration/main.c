#include <stdio.h>
#include "nn.h"
#include "pair.h"
#include "list.h"

#define SOCKET_ADDRESS "inproc://fr_master1"

int main() {
	// Socket variable
	int sa;
	int sb;
	// Connection variable
	int ra;
	int rb;
	// Communication variables
	char* data;
	int len;
	char* buf;
	// Linked list variables
	List list;
	Position pos;

	printf("Hello nanomsg\n");

	// Prepare linked list
	list = MakeEmpty(NULL);
	pos = Header(list);

	// Prepare nano socket
	sa = nn_socket(AF_SP, NN_PAIR);
	if (sa < 0) {
		printf("Error creating sa socket");
		return -1;
	}
	printf("Socket a %d\n", sa);

	sb = nn_socket(AF_SP, NN_PAIR);
	if (sb < 0) {
		printf("Error creating sb socket");
		return -1;
	}
	printf("Socket b %d\n", sb);

	// Connect them to the inproc addr
	ra = nn_connect(sa, SOCKET_ADDRESS); // Variable number of client
	rb = nn_bind(sb, SOCKET_ADDRESS); // Should be the data structure server
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

	data = "data";
	len = strlen(data);

	// Multiple send
	buf = (char*)malloc(len + 1);
	for (int i = 0; i < 100000; i++) {
		nn_send(sa, data, len, NN_DONTWAIT);
		nn_recv(sb, buf, len + 1, 0);
		// Insert into linked list
		Insert(i, list, pos);
		pos = Advance(pos);
	}
	free(buf);

	nn_shutdown(sa, 0);
	nn_shutdown(sb, 0);

	return 0;
}
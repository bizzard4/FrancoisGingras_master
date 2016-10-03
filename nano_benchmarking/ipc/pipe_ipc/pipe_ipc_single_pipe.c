#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include "list.h"

// Named pipe information
int fd;
char* myfifo;

// Global linked list
List list;
Position pos;

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
		int ws = write(fd, data, len);
		printf("Has send, ws=%d i=%d\n", ws, i); // THIS SLOW SEND AND MAKE ALL WORK
		//sched_yield();
		//if (e <= 0) {
		//	printf("Thread sending error %d\n", e);
		//}
	}
}

// Receiving thread to update list, no lock required because it is message passing
void recv_proc() {
	char* buf = (char*)malloc(len + 1);
	int i = 0;
	while (i<(thread_count*update_count)) {
		int rs = read(fd, buf, len);
		if (rs > 0) {
			//int error = nn_errno();
			//if (e <= 0) {
			//	printf("Thread receiving error %d\n", error);
			//}
			printf("Thread received, rs=%d i=%d\n", rs, i);
			//sched_yield();
			// Insert into linked list
			Insert(update_size, list, pos);
			pos = Advance(pos);

			i++;
		} else if (i < 0) {
			printf("Nothign received %d\n", errno);
		}
	}
	free(buf);
}

int main(int argc, char* argv[]) {
	// argv
	thread_count = atoi(argv[1]);
	update_count = atoi(argv[2]);
	update_size = atoi(argv[3]);
	execution_mode = argv[4][0]; // c=client, s=server
	myfifo = argv[5];

	if ((execution_mode != 's') && (execution_mode != 'c')) {
		printf("Execution mode need to be s or c\n");
		return -1;
	}

	// Prepare linked list
	list = MakeEmpty(NULL);
	pos = Header(list);

	// Prepare named pipe
	if (execution_mode=='s') { // Server is reader
		fd = open(myfifo, O_CREAT|O_RDONLY|O_TRUNC, S_IRWXU);
		if (fd < 0) {
			printf("Error opening server pipe %d:%s\n", errno, strerror(errno));
			return -1;
		}
	} else { // Clients are writer
		fd = open(myfifo, O_WRONLY);
		if (fd < 0) {
			printf("Error opening client pipe %d:%s\n", errno, strerror(errno));
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

	close(fd);
	if (execution_mode=='s') { // Server kill pipe
		unlink(myfifo);
	}

	return 0;
}

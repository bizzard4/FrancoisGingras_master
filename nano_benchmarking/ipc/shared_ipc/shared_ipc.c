#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include "list.h"

// Global linked list
List list;
Position pos;

// Shared data
sem_t* sem_empty_addr; // Init to buffer size
sem_t* sem_full_addr; // Init to 0
sem_t* sem_mutex_addr;
int* in;
int* out;
int* count;
int* buffer;

int shmid; // Shared memory id
#define BUFFER_SIZE 50

// Parameters
int thread_count;
int update_count;
int update_size;
char execution_mode;

// Thread that send message to add to list
void send_proc() {
	for (int i = 0; i < update_count; i++) {
		int item = i;

		sem_wait(sem_empty_addr);

		sem_wait(sem_mutex_addr);
		buffer[*in] = item;
		*in = ((*in) + 1)%BUFFER_SIZE;
		*count = (*count) + 1;
		sem_post(sem_mutex_addr);

		sem_post(sem_full_addr);
	}
}

// Receiving thread to update list, no lock required because it is message passing
void recv_proc() {
	for (int i = 0; i < (update_count*thread_count); i++) {
		sem_wait(sem_full_addr);

		sem_wait(sem_mutex_addr);
		int item = buffer[*out];
		*out = ((*out) + 1)%BUFFER_SIZE;
		*count = (*count) - 1;
		sem_post(sem_mutex_addr);

		sem_post(sem_empty_addr);

		// Consume
		Insert(update_size, list, pos);
		pos = Advance(pos);
	}

	// Detach and remove shared memory space
	shmctl(shmid, IPC_RMID, NULL);
}

int main(int argc, char* argv[]) {
	key_t key = 0x1020304; // Shared memory key, if already exist it will return error
	void* addr; // Semaphore address start

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
		shmid = shmget(key, 3*sizeof(sem_t)+(BUFFER_SIZE+3)*sizeof(int), IPC_CREAT | 0666);
		if (shmid < 0) {
			printf("Error creating server segment \n");
			return -1;
		}
	} else {
		shmid = shmget(key, 3*sizeof(sem_t)+(BUFFER_SIZE+3)*sizeof(int), 0);
		if (shmid < 0) {
			printf("Error creating client segment \n");
			return -1;
		}
	}

	// Attach
	addr = shmat(shmid, NULL, 0);
	if ((int)addr == -1) {
		printf("Error in shmat %d\n", errno);
		return -1;
	}

	sem_empty_addr = (sem_t*)addr;
	sem_full_addr = (sem_t*)addr+sizeof(sem_t);
	sem_mutex_addr = (sem_t*)addr+2*sizeof(sem_t);
	in = (int*)addr+3*sizeof(sem_t);
	out = (int*)addr+3*sizeof(sem_t)+sizeof(int);
	count = (int*)addr+3*sizeof(sem_t)+2*sizeof(int);
	buffer = (int*)addr+3*sizeof(sem_t)+3*sizeof(int);

	// Do work
	if (execution_mode=='s') {
		// Initialize values
		if (sem_init(sem_empty_addr, 1, BUFFER_SIZE) == -1) {
			printf("Error in sem_empty_addr %d\n", errno);
			return -1;
		}
		if (sem_init(sem_full_addr, 1, 0) == -1) {
			printf("Error in sem_empty_addr %d\n", errno);
			return -1;
		}
		if (sem_init(sem_mutex_addr, 1, 1) == -1) {
			printf("Error in sem_empty_addr %d\n", errno);
			return -1;
		}
		*in = 0;
		*out = 0;
		*count = 0;
		for (int i = 0; i < BUFFER_SIZE; i++) {
			buffer[i] = 0;
		}
		recv_proc();
	} else {
		send_proc();
	}

	usleep(200000);
	return 0;
}

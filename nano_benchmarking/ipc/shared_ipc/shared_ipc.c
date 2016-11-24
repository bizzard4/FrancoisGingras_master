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

int shmid; // Shared memory id
#define BUFFER_SIZE 10000
typedef struct prod_cons_shared {
	sem_t sem_empty_addr;
	sem_t sem_full_addr;
	sem_t sem_mutex_addr;
	int in;
	int out;
	int count;
	int buffer[BUFFER_SIZE];
};
struct prod_cons_shared* shared_stuff;

// Parameters
int thread_count;
int update_count;
int update_size;
char execution_mode;

// Thread that send message to add to list
void send_proc() {
	for (int i = 0; i < update_count; i++) {
		int item = i;

		sem_wait(&shared_stuff->sem_empty_addr);

		sem_wait(&shared_stuff->sem_mutex_addr);
		shared_stuff->buffer[shared_stuff->in] = item;
		shared_stuff->in = ((shared_stuff->in) + 1)%BUFFER_SIZE;
		shared_stuff->count = (shared_stuff->count) + 1;
		sem_post(&shared_stuff->sem_mutex_addr);

		sem_post(&shared_stuff->sem_full_addr);
	}
}

// Receiving thread to update list, no lock required because it is message passing
void recv_proc() {
	for (int i = 0; i < (update_count*thread_count); i++) {
		sem_wait(&shared_stuff->sem_full_addr);

		sem_wait(&shared_stuff->sem_mutex_addr);
		int item = shared_stuff->buffer[shared_stuff->out];
		shared_stuff->out = ((shared_stuff->out) + 1)%BUFFER_SIZE;
		shared_stuff->count = (shared_stuff->count) - 1;
		sem_post(&shared_stuff->sem_mutex_addr);

		sem_post(&shared_stuff->sem_empty_addr);

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
		shmid = shmget(key, sizeof(struct prod_cons_shared), IPC_CREAT | 0666);
		if (shmid < 0) {
			printf("Error creating server segment \n");
			return -1;
		}
	} else {
		shmid = shmget(key, sizeof(struct prod_cons_shared), 0);
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

	shared_stuff = (struct prod_cons_shared*)addr;

	// Do work
	if (execution_mode=='s') {
		// Initialize values
		if (sem_init(&shared_stuff->sem_empty_addr, 1, BUFFER_SIZE) == -1) {
			printf("Error in sem_empty_addr %d\n", errno);
			return -1;
		}
		if (sem_init(&shared_stuff->sem_full_addr, 1, 0) == -1) {
			printf("Error in sem_empty_addr %d\n", errno);
			return -1;
		}
		if (sem_init(&shared_stuff->sem_mutex_addr, 1, 1) == -1) {
			printf("Error in sem_empty_addr %d\n", errno);
			return -1;
		}
		shared_stuff->in = 0;
		shared_stuff->out = 0;
		shared_stuff->count = 0;
		for (int i = 0; i < BUFFER_SIZE; i++) {
			shared_stuff->buffer[i] = 0;
		}
		recv_proc();
	} else {
		send_proc();
	}

	usleep(200000);
	return 0;
}

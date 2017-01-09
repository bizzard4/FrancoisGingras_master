#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

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

int main() {
	int shmid; // Shared memory id
	key_t key = ftok("/home/tmp/test_shared", 'b');
	void* addr; // Shared memory address start
	struct prod_cons_shared* shared_stuff;

	//sem_t* sem_empty_addr; // Init to buffer size
	//sem_t* sem_full_addr; // Init to 0
	//sem_t* sem_mutex_addr;
	//int* in;
	//int* out;
	//int* count;
	//int* buffer;

	// Key, size, flags
	int size = sizeof(struct prod_cons_shared);
	shmid = shmget(key, size, IPC_CREAT | 0666);
	if (shmid < 0) {
		printf("Error creating segment \n");
		return -1;
	}

	// Attach
	addr = shmat(shmid, NULL, 0);
	if ((int)addr == -1) {
		printf("Error in shmat %d\n", errno);
		return -1;
	}

	printf("Got %p\n", addr);
	shared_stuff = (struct prod_cons_shared*)addr;
	//sem_empty_addr = (sem_t*)addr;
	//sem_full_addr = (sem_t*)addr+sizeof(sem_t);
	//sem_mutex_addr = (sem_t*)addr+2*sizeof(sem_t);
	//in = (int*)addr+3*sizeof(sem_t);
	//out = (int*)addr+3*sizeof(sem_t)+sizeof(int);
	//count = (int*)addr+3*sizeof(sem_t)+2*sizeof(int);
	//buffer = (int*)addr+3*sizeof(sem_t)+3*sizeof(int);

	//printf("In addr = %p\n", in);
	//printf("Out addr = %p\n", out);
	//printf("Count addr = %p\n", count);

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

	printf("Start to consume\n");

	// Consume 100 times
	for (int i = 0; i < 100000; i++) {
		sem_wait(&shared_stuff->sem_full_addr);

		sem_wait(&shared_stuff->sem_mutex_addr);
		int item = shared_stuff->buffer[shared_stuff->out];
		shared_stuff->out = ((shared_stuff->out) + 1)%BUFFER_SIZE;
		shared_stuff->count = (shared_stuff->count) - 1;
		printf("I=%d In=%d Out=%d Count=%d\n", i, shared_stuff->in, shared_stuff->out, shared_stuff->count);
		sem_post(&shared_stuff->sem_mutex_addr);

		sem_post(&shared_stuff->sem_empty_addr);

		printf("Consumed %d\n", item);
	}

	// Detach and remove shared memory space
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}

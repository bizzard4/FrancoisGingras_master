#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>

#define BUFFER_SIZE 100

int main() {
	int shmid; // Shared memory id
	key_t key = 0x1020304; // Shared memory key, if already exist it will return error
	void* addr; // Semaphore address start

	sem_t* sem_empty_addr; // Init to buffer size
	sem_t* sem_full_addr; // Init to 0
	sem_t* sem_mutex_addr;
	int* in;
	int* out;
	int* count;
	int* buffer;

	// Key, size, flags
	shmid = shmget(key, 3*sizeof(sem_t)+(BUFFER_SIZE+3)*sizeof(int), IPC_CREAT | 0666);
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
	sem_empty_addr = (sem_t*)addr;
	sem_full_addr = (sem_t*)addr+sizeof(sem_t);
	sem_mutex_addr = (sem_t*)addr+2*sizeof(sem_t);
	in = (int*)addr+3*sizeof(sem_t);
	out = (int*)addr+3*sizeof(sem_t)+sizeof(int);
	count = (int*)addr+3*sizeof(sem_t)+2*sizeof(int);
	buffer = (int*)addr+3*sizeof(sem_t)+3*sizeof(int);

	printf("In addr = %p\n", in);
	printf("Out addr = %p\n", out);
	printf("Count addr = %p\n", count);

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

	// Consume 100 times
	for (int i = 0; i < 100; i++) {
		sem_wait(sem_full_addr);

		sem_wait(sem_mutex_addr);
		int item = buffer[*out];
		*out = ((*out) + 1)%BUFFER_SIZE;
		*count = (*count) - 1;
		sem_post(sem_mutex_addr);

		sem_post(sem_empty_addr);

		printf("Consumed %d\n", item);
	}

	// Detach and remove shared memory space
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
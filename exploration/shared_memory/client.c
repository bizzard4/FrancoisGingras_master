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
	printf("%x\n", key);
	void* addr; // Semaphore address start
	struct prod_cons_shared* shared_stuff;

	//sem_t* sem_empty_addr; // Init to buffer size
	//sem_t* sem_full_addr; // Init to 0
	//sem_t* sem_mutex_addr;
	//int* in;
	//int* out;
	//int* count;
	//int* buffer;

	// Key, size, flags
	//int size = 3*sizeof(sem_t)+(BUFFER_SIZE+3)*sizeof(int);
	int size = sizeof(struct prod_cons_shared);
	printf("Size %d\n", size);
	shmid = shmget(key, size, 0);
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

	printf("Start to produce\n");

	// Produce 100 times
	for (int i = 0; i < 50000; i++) {
		int item = i;

		sem_wait(&shared_stuff->sem_empty_addr);

		sem_wait(&shared_stuff->sem_mutex_addr);
		shared_stuff->buffer[shared_stuff->in] = item;
		shared_stuff->in = ((shared_stuff->in) + 1)%BUFFER_SIZE;
		shared_stuff->count = (shared_stuff->count) + 1;
		printf("I=%d In=%d Out=%d Count=%d\n", i, shared_stuff->in, shared_stuff->out, shared_stuff->count);
		sem_post(&shared_stuff->sem_mutex_addr);

		sem_post(&shared_stuff->sem_full_addr);

		printf("Produced %d\n", item);
	}

	return 0;
}

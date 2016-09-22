#include <pthread.h>
#include <stdio.h>

void* run_proc(void* x_void_ptr) {
	printf("Hello from thread\n");
}

int main() {
	printf("Creating threads\n");

	pthread_t t1;
	if (pthread_create(&t1, NULL, run_proc, NULL)) {
		printf("Error creating the thread\n");
		return 0;
	}

	if (pthread_join(t1, NULL)) {
		printf("Error joining\n");
		return 0;
	}

	printf("All good\n");
	return 0;
}

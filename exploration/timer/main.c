#include <time.h>
#include <stdio.h>

struct timespec diff(struct timespec start, struct timespec end);

int main() {

	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

	int temp = 0;
	for (int i = 0; i < 10000; i++) {
		temp = temp + 1;
	}
	clock_gettime(CLOCK_MONOTONIC, &end);

	printf("Time %ld\n", diff(start, end).tv_nsec); 

	return 0;
}


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

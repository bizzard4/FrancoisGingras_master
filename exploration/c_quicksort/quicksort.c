#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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

struct timespec tsAdd(struct  timespec  time1, struct  timespec  time2) 
{
    struct  timespec  result ;

    result.tv_sec = time1.tv_sec + time2.tv_sec ;
    result.tv_nsec = time1.tv_nsec + time2.tv_nsec ;
    if (result.tv_nsec >= 1000000000L) {		
        result.tv_sec++ ;  result.tv_nsec = result.tv_nsec - 1000000000L ;
    }

    return (result) ;
}

// Build command : gcc quicksort.c -o out

//#define DISPLAY_DEBUG

int cmpfunc(const void* a, const void* b)
{
   return (*(int*)a - *(int*)b);
}

int main(int argc, char *argv[]) {
	printf("Quicksort start\n");

	// Read N and load a N sized array of int
	struct timespec read_start, read_end;
	clock_gettime(CLOCK_MONOTONIC, &read_start);
	int n = atoi(argv[1]);

	printf("Start reading data\n");
	int temp;
	int* array_to_sort = malloc(n * sizeof(int));
	for (int i = 0; i < n; i++) {
		scanf("%d", &temp);
		array_to_sort[i] = temp;
	}
	printf("Done reading data\n");
	clock_gettime(CLOCK_MONOTONIC, &read_end);

	// Sort the array
	struct timespec sort_start, sort_end;
	clock_gettime(CLOCK_MONOTONIC, &sort_start);
	printf("Start sorting data\n");
	qsort(array_to_sort, n, sizeof(int), cmpfunc);
	printf("Done sorting data\n");
	clock_gettime(CLOCK_MONOTONIC, &sort_end);

	#ifdef DISPLAY_DEBUG
	for (int i = 0; i < n; i++) {
		printf("%d ", array_to_sort[i]);
	}
	printf("\n");
	#endif

	free(array_to_sort);

	printf("Quicksort done\n");

	// Statistics
	struct timespec read_diff = diff(read_start, read_end);
	printf("QS-READ %lds,%ldms - Get sample time\n", read_diff.tv_sec, read_diff.tv_nsec/1000000);

	struct timespec sort_diff = diff(sort_start, sort_end);
	printf("QS-SORT %lds,%ldms - Waiting on splitter time\n", sort_diff.tv_sec, sort_diff.tv_nsec/1000000);

	return 0;
}
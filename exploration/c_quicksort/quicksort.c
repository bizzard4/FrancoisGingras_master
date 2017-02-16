#include <stdio.h>
#include <stdlib.h>

// Build command : gcc quicksort.c -o out

//#define DISPLAY_DEBUG

int cmpfunc(const void* a, const void* b)
{
   return (*(int*)a - *(int*)b);
}

int main(int argc, char *argv[]) {
	printf("Quicksort start\n");

	// Read N and load a N sized array of int
	int n = atoi(argv[1]);

	printf("Start reading data\n");
	int temp;
	int* array_to_sort = malloc(n * sizeof(int));
	for (int i = 0; i < n; i++) {
		scanf("%d", &temp);
		array_to_sort[i] = temp;
	}
	printf("Done reading data\n");

	// Sort the array
	printf("Start sorting data\n");
	qsort(array_to_sort, n, sizeof(int), cmpfunc);
	printf("Done sorting data\n");

	#ifdef DISPLAY_DEBUG
	for (int i = 0; i < n; i++) {
		printf("%d ", array_to_sort[i]);
	}
	printf("\n");
	#endif

	free(array_to_sort);

	printf("Quicksort done\n");

	return 0;
}
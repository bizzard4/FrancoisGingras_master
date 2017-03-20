#include <stdio.h>      /* printf, scanf, puts, NULL */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

// Tool to generate random data
// Usage : gen N
// Compilation : gcc gen.c -o gen

int main(int argc, char **argv)
{
  int myNum;
  int n = atoi(argv[1]);

  /* initialize random seed: */
  srand (time(NULL));

  for (int i = 0; i < n; i++) {
  	/* generate secret number between 1 and 1M: */
  	myNum = rand() % 1000000 + 1;
  	printf("%d\n", myNum);
  }
}
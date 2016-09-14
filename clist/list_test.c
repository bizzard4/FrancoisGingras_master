#include <stdio.h>
#include "list.h"

int main() {
	List l;
    Position p;
	int empty;

    l = MakeEmpty(NULL);
    p = Header(l);
	empty = IsEmpty(l);
	
	printf("IsEmpty:%d\n", empty);
	return 0;
}
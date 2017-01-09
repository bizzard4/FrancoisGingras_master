#include <stdio.h>
#include "list.h"

int main() {
	List l;
    Position p;
	Position five_pos;
	int empty;

    l = MakeEmpty(NULL);
    p = Header(l);
	empty = IsEmpty(l);
	
	printf("IsEmpty:%d\n", empty);
	printf("Inserting 10 elemens\n");
	for (int i = 0; i < 10; i++) {
		Insert(i,l,p);
		p = Advance(p);
	}
	printf("Done inserting\n");
	empty = IsEmpty(l);
	printf("IsEmpty:%d\n", empty);
	
	five_pos = Find(5, l);
	int element = Retrieve(five_pos);
	printf("5 found at:%d\n", element);
	
	
	return 0;
}
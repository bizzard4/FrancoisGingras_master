
#ifndef DONEMSG_GENERATED_H_
#define DONEMSG_GENERATED_H_

#include <stdio.h>

static int getTag(DoneMsg this);
static DoneMsg clone(DoneMsg this);
static void destroy(DoneMsg this);

/*
 * Create a new DoneMsg object and initialize its function pointers.
 */
DoneMsg DoneMsg_create(int tag){
	DoneMsg newRec = malloc(sizeof(struct DoneMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in DoneMsg_create");

	newRec->tag = tag;

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	return newRec;
}

#endif /* DONEMSG_GENERATED_H_ */

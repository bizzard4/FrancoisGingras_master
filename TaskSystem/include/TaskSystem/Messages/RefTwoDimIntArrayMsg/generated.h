
#ifndef REFTWODIMINTARRAYMSG_GENERATED_H_
#define REFTWODIMINTARRAYMSG_GENERATED_H_

#include <stdio.h>

static int getTag(RefTwoDimIntArrayMsg this);
static RefTwoDimIntArrayMsg clone(RefTwoDimIntArrayMsg this);
static void destroy(RefTwoDimIntArrayMsg this);

/*
 * Create a new RefTwoDimIntArrayMsg object and initialize its function pointers.
 */
RefTwoDimIntArrayMsg RefTwoDimIntArrayMsg_create(int tag){
	RefTwoDimIntArrayMsg newRec = malloc(sizeof(struct RefTwoDimIntArrayMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in RefTwoDimIntArrayMsg_create");

	newRec->tag = tag;

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	return newRec;
}

#endif /* REFTWODIMINTARRAYMSG_GENERATED_H_ */

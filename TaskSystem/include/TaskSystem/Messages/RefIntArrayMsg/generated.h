
#ifndef REFINTARRAYMSG_GENERATED_H_
#define REFINTARRAYMSG_GENERATED_H_

#include <stdio.h>

static int getTag(RefIntArrayMsg this);
static RefIntArrayMsg clone(RefIntArrayMsg this);
static void destroy(RefIntArrayMsg this);

static void setValues(RefIntArrayMsg this, int count, int val[]);
static int getSize(RefIntArrayMsg this);
static int getValue(RefIntArrayMsg this, int index);

/*
 * Create a new RefIntArrayMsg object and initialize its function pointers.
 */
RefIntArrayMsg RefIntArrayMsg_create(int tag){
	RefIntArrayMsg newRec = malloc(sizeof(struct RefIntArrayMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in IntArrayMsg_create");

	//printf("MESSAGE CREATION - Creating intarray msg with tag=%d\n", tag);
	newRec->tag = tag;

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->setValues = setValues;
	newRec->getSize = getSize;
	newRec->getValue = getValue;

	return newRec;
}

#endif /* REFINTARRAYMSG_GENERATED_H_ */

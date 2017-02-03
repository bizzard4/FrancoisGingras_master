
#ifndef INTARRAYMSG_GENERATED_H_
#define INTARRAYMSG_GENERATED_H_

#include <stdio.h>

static int getTag(IntArrayMsg this);
static IntArrayMsg clone(IntArrayMsg this);
static void destroy(IntArrayMsg this);

static void setValues(IntArrayMsg this, int count, int val[]);
static int getSize(IntArrayMsg this);
static int getValue(IntArrayMsg this, int index);

/*
 * Create a new IntArrayMsg object and initialize its function pointers.
 */
IntArrayMsg IntArrayMsg_create(int tag){
	IntArrayMsg newRec = malloc(sizeof(struct IntArrayMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in IntArrayMsg_create");

	printf("MESSAGE CREATION - Creating intarray msg with tag=%d\n", tag);
	newRec->tag = tag;

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->setValues = setValues;
	newRec->getSize = getSize;
	newRec->getValue = getValue;

	return newRec;
}

#endif /* INTARRAYMSG_GENERATED_H_ */

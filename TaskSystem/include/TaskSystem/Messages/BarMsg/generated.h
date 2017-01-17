
#ifndef BARMSG_GENERATED_H_
#define BARMSG_GENERATED_H_

#include <stdio.h>

static int getTag(BarMsg this);
static BarMsg clone(BarMsg this);
static void destroy(BarMsg this);
static int getValue(BarMsg this);
static void setValue(BarMsg this, int value);


/*
 * Create a new BarMsg object and initialize its function pointers.
 */
BarMsg BarMsg_create(int tag){
	BarMsg newRec = malloc(sizeof(struct BarMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in BarMsg_create");

	newRec->tag = tag;

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->getValue = getValue;
	newRec->setValue = setValue;

	return newRec;
}




#endif /* BARMSG_GENERATED_H_ */

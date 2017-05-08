
#ifndef REFINTARRAYMSG_GENERATED_H_
#define REFINTARRAYMSG_GENERATED_H_

#include <stdio.h>

static int getTag(RefIntArrayMsg this);
static RefIntArrayMsg clone(RefIntArrayMsg this);
static void destroy(RefIntArrayMsg this);
static int writeAt(RefIntArrayMsg this, void* addr);

static void setValues(RefIntArrayMsg this, int count, int val[]);
static int getSize(RefIntArrayMsg this);
static int getValue(RefIntArrayMsg this, int index);

#define REFINTARRAY_MSG_ID 3

/*
 * Create a new RefIntArrayMsg object and initialize its function pointers.
 */
RefIntArrayMsg RefIntArrayMsg_create(int tag){
	RefIntArrayMsg newRec = malloc(sizeof(struct RefIntArrayMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in IntArrayMsg_create");

	//printf("MESSAGE CREATION - Creating intarray msg with tag=%d\n", tag);
	newRec->tag = tag;
	newRec->tid = REFINTARRAY_MSG_ID;
	newRec->msg_size = sizeof(struct RefIntArrayMsg);

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->writeAt = writeAt;

	// RefIntArray mesage members

	newRec->setValues = setValues;
	newRec->getSize = getSize;
	newRec->getValue = getValue;

	return newRec;
}

// Rebind method only
void RefIntArrayMsg_rebind(Message msg) {
	RefIntArrayMsg rebind_msg = (RefIntArrayMsg)msg;

	rebind_msg->getTag = getTag;
	rebind_msg->clone = clone;
	rebind_msg->destroy = destroy;

	rebind_msg->writeAt = writeAt;

	// Start of ref int array msg

	rebind_msg->setValues = setValues;
	rebind_msg->getSize = getSize;
	rebind_msg->getValue = getValue;
}

#endif /* REFINTARRAYMSG_GENERATED_H_ */

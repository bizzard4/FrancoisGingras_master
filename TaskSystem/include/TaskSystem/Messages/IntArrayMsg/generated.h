
#ifndef INTARRAYMSG_GENERATED_H_
#define INTARRAYMSG_GENERATED_H_

#include <stdio.h>

static int getTag(IntArrayMsg this);
static IntArrayMsg clone(IntArrayMsg this);
static void destroy(IntArrayMsg this);
static int writeAt(IntArrayMsg this, void* addr);

static void setValues(IntArrayMsg this, int count, int val[]);
static int getSize(IntArrayMsg this);
static int getValue(IntArrayMsg this, int index);

#define INTARRAY_MSG_ID 5

/*
 * Create a new IntArrayMsg object and initialize its function pointers.
 */
IntArrayMsg IntArrayMsg_create(int tag){
	IntArrayMsg newRec = malloc(sizeof(struct IntArrayMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in IntArrayMsg_create");

	//printf("MESSAGE CREATION - Creating intarray msg with tag=%d\n", tag);
	newRec->tag = tag;
	newRec->tid = INTARRAY_MSG_ID;
	newRec->msg_size = sizeof(struct IntArrayMsg);

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;
	newRec->writeAt = writeAt;

	// Start of int array msg

	newRec->setValues = setValues;
	newRec->getSize = getSize;
	newRec->getValue = getValue;

	return newRec;
}

// Rebind method only
void IntArrayMsg_rebind(Message msg) {
	IntArrayMsg rebind_msg = (IntArrayMsg)msg;

	rebind_msg->getTag = getTag;
	rebind_msg->clone = clone;
	rebind_msg->destroy = destroy;

	rebind_msg->writeAt = writeAt;

	// Start of bar msg

	rebind_msg->setValues = setValues;
	rebind_msg->getSize = getSize;
	rebind_msg->getValue = getValue;

	// Rebind the pointer to the end of the struct position
	rebind_msg->values = (void*)((long)msg + sizeof(struct IntArrayMsg));
}

#endif /* INTARRAYMSG_GENERATED_H_ */

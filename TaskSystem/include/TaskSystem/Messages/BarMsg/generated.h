
#ifndef BARMSG_GENERATED_H_
#define BARMSG_GENERATED_H_

#include <stdio.h>

static int getTag(BarMsg this);
static BarMsg clone(BarMsg this);
static void destroy(BarMsg this);
static int writeAt(BarMsg this, void* addr);

static int getValue(BarMsg this);
static void setValue(BarMsg this, int value);

#define BAR_MSG_ID 1

/*
 * Create a new BarMsg object and initialize its function pointers.
 */
BarMsg BarMsg_create(int tag){
	BarMsg newRec = malloc(sizeof(struct BarMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in BarMsg_create");

	newRec->tag = tag;
	newRec->tid = BAR_MSG_ID;
	newRec->msg_size = sizeof(struct BarMsg);

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->writeAt = writeAt;

	// Start of bar msg

	newRec->getValue = getValue;
	newRec->setValue = setValue;

	return newRec;
}

// Rebind method only
void BarMsg_rebind(Message msg) {
	BarMsg rebind_msg = (BarMsg)msg;

	rebind_msg->getTag = getTag;
	rebind_msg->clone = clone;
	rebind_msg->destroy = destroy;

	rebind_msg->writeAt = writeAt;

	// Start of bar msg

	rebind_msg->getValue = getValue;
	rebind_msg->setValue = setValue;
}




#endif /* BARMSG_GENERATED_H_ */

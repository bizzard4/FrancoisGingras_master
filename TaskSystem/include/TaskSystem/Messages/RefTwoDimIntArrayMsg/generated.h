
#ifndef REFTWODIMINTARRAYMSG_GENERATED_H_
#define REFTWODIMINTARRAYMSG_GENERATED_H_

#include <stdio.h>

static int getTag(RefTwoDimIntArrayMsg this);
static RefTwoDimIntArrayMsg clone(RefTwoDimIntArrayMsg this);
static void destroy(RefTwoDimIntArrayMsg this);
static int writeAt(RefTwoDimIntArrayMsg this, void* addr);

#define REFTWODIMINTARRAY_MSG_ID 7

/*
 * Create a new RefTwoDimIntArrayMsg object and initialize its function pointers.
 */
RefTwoDimIntArrayMsg RefTwoDimIntArrayMsg_create(int tag){
	RefTwoDimIntArrayMsg newRec = malloc(sizeof(struct RefTwoDimIntArrayMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in RefTwoDimIntArrayMsg_create");

	newRec->tag = tag;
	newRec->tid = REFTWODIMINTARRAY_MSG_ID;
	newRec->msg_size = sizeof(struct RefTwoDimIntArrayMsg);

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;
	newRec->writeAt = writeAt;

	// Start of ref two dim int array msg

	return newRec;
}

void RefTwoDimIntArrayMsg_rebind(Message msg) {
	RefTwoDimIntArrayMsg rebind_msg = (RefTwoDimIntArrayMsg)msg;

	rebind_msg->getTag = getTag;
	rebind_msg->clone = clone;
	rebind_msg->destroy = destroy;

	rebind_msg->writeAt = writeAt;

	// Start of bar msg
}

#endif /* REFTWODIMINTARRAYMSG_GENERATED_H_ */

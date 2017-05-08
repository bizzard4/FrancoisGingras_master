
#ifndef DONEMSG_GENERATED_H_
#define DONEMSG_GENERATED_H_

#include <stdio.h>

static int getTag(DoneMsg this);
static DoneMsg clone(DoneMsg this);
static void destroy(DoneMsg this);
static int writeAt(DoneMsg this, void* addr);

#define DONE_MSG_ID 6

/*
 * Create a new DoneMsg object and initialize its function pointers.
 */
DoneMsg DoneMsg_create(int tag){
	DoneMsg newRec = malloc(sizeof(struct DoneMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in DoneMsg_create");

	newRec->tag = tag;
	newRec->tid = DONE_MSG_ID;
	newRec->msg_size = sizeof(struct DoneMsg);

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;
	newRec->writeAt = writeAt;

	return newRec;
}

// Rebind method only
void DoneMsg_rebind(Message msg) {
	DoneMsg rebind_msg = (DoneMsg)msg;

	rebind_msg->getTag = getTag;
	rebind_msg->clone = clone;
	rebind_msg->destroy = destroy;

	rebind_msg->writeAt = writeAt;

	// Start of done msg
}

#endif /* DONEMSG_GENERATED_H_ */

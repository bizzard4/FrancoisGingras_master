
#ifndef RESPONSEMSG_GENERATED_H_
#define RESPONSEMSG_GENERATED_H_

#include <stdio.h>

static int getTag(ResponseMsg this);
static ResponseMsg clone(ResponseMsg this);
static void destroy(ResponseMsg this);
static int writeAt(ResponseMsg this, void* addr);

static char* getData(ResponseMsg this);
static void setData(ResponseMsg this, char* data, int size);

#define RESPONSE_MSG_ID 9

/*
 * Create a new ResponseMsg object and initialize its function pointers.
 */
ResponseMsg ResponseMsg_create(int tag){
	ResponseMsg newRec = malloc(sizeof(struct ResponseMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in BarMsg_create");

	newRec->tag = tag;
	newRec->tid = RESPONSE_MSG_ID;
	newRec->msg_size = sizeof(struct ResponseMsg);

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->writeAt = writeAt;

	// Start of bar msg
	newRec->data_size = 0;
	newRec->data = NULL;
	newRec->code = -1;

	newRec->getData = getData;
	newRec->setData = setData;

	return newRec;
}

// Rebind method only
void ResponseMsg_rebind(Message msg) {
	ResponseMsg rebind_msg = (ResponseMsg)msg;

	rebind_msg->getTag = getTag;
	rebind_msg->clone = clone;
	rebind_msg->destroy = destroy;

	rebind_msg->writeAt = writeAt;

	// Start of bar msg

	rebind_msg->getData = getData;
	rebind_msg->setData = setData;

	rebind_msg->data = (void*)((long)msg + sizeof(struct ResponseMsg));
}


#endif /* RESPONSEMSG_GENERATED_H_ */

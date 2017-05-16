
#ifndef REQUESTMSG_GENERATED_H_
#define REQUESTMSG_GENERATED_H_

#include <stdio.h>

static int getTag(RequestMsg this);
static RequestMsg clone(RequestMsg this);
static void destroy(RequestMsg this);
static int writeAt(RequestMsg this, void* addr);

static char* getData(RequestMsg this);
static void setData(RequestMsg this, char* data, int size);

#define REQUEST_MSG_ID 8

/*
 * Create a new RequestMsg object and initialize its function pointers.
 */
RequestMsg RequestMsg_create(int tag){
	RequestMsg newRec = malloc(sizeof(struct RequestMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in BarMsg_create");

	newRec->tag = tag;
	newRec->tid = REQUEST_MSG_ID;
	newRec->msg_size = sizeof(struct RequestMsg);

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->writeAt = writeAt;

	// Start of bar msg
	newRec->data_size = 0;
	newRec->data = NULL;
	newRec->sender_task_id = -1;
	newRec->request_type = -1;

	newRec->getData = getData;
	newRec->setData = setData;

	return newRec;
}

// Rebind method only
void RequestMsg_rebind(Message msg) {
	RequestMsg rebind_msg = (RequestMsg)msg;

	rebind_msg->getTag = getTag;
	rebind_msg->clone = clone;
	rebind_msg->destroy = destroy;

	rebind_msg->writeAt = writeAt;

	// Start of bar msg

	rebind_msg->getData = getData;
	rebind_msg->setData = setData;

	rebind_msg->data = (void*)((long)msg + sizeof(struct RequestMsg));
}


#endif /* REQUESTMSG_GENERATED_H_ */

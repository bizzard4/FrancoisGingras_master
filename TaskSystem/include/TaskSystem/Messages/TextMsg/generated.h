
#ifndef TEXTMSG_GENERATED_H_
#define TEXTMSG_GENERATED_H_

#include <stdio.h>

// prototypes
static int getTag(TextMsg this);
static TextMsg clone(TextMsg this);
static void destroy(TextMsg this);
static int writeAt(TextMsg this, void* addr);

static char* getMsg(TextMsg this);
static void setMsg(TextMsg this, char* msg);

#define TEXT_MSG_ID 2

/*
 * Create a new TextMsg object and initialize its function pointers.
 */
TextMsg TextMsg_create(int tag){
	TextMsg newRec = malloc(sizeof(struct TextMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in TextMsg_create");

	newRec->tag = tag;
	newRec->tid = TEXT_MSG_ID;
	newRec->msg_size = sizeof(struct TextMsg);

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->writeAt = writeAt;

	// Start of text msg

	newRec->getMsg = getMsg;
	newRec->setMsg = setMsg;

	return newRec;
}

// Rebind method only
void TextMsg_rebind(Message msg) {
	TextMsg rebind_msg = (TextMsg)msg;

	rebind_msg->getTag = getTag;
	rebind_msg->clone = clone;
	rebind_msg->destroy = destroy;

	rebind_msg->writeAt = writeAt;

	// Start of text msg

	rebind_msg->getMsg = getMsg;
	rebind_msg->setMsg = setMsg;
}



#endif /* TEXTMSG_GENERATED_H_ */

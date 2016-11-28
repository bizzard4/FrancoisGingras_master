
#ifndef TEXTMSG_GENERATED_H_
#define TEXTMSG_GENERATED_H_

#include <stdio.h>

// prototypes
static int getTag(TextMsg this);
static TextMsg clone(TextMsg this);
static void destroy(TextMsg this);
static char* getMsg(TextMsg this);
static void setMsg(TextMsg this, char* msg);


/*
 * Create a new TextMsg object and initialize its function pointers.
 */
TextMsg TextMsg_create(int tag){
	TextMsg newRec = malloc(sizeof(struct TextMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in TextMsg_create");

	newRec->tag = tag;

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->getMsg = getMsg;
	newRec->setMsg = setMsg;

	return newRec;
}



#endif /* TEXTMSG_GENERATED_H_ */

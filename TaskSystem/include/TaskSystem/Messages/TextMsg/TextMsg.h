
#ifndef TEXTMSG_H_
#define TEXTMSG_H_

#include "TaskSystem/Messages/Message.h"

typedef struct TextMsg *TextMsg;

struct TextMsg {

	/* parent class members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(TextMsg this);
	TextMsg 	(*clone)(TextMsg this);
	void 	(*destroy)(TextMsg this);
	// Serialization
	int (*writeAt)(TextMsg this, void* addr);


	/* new child class members */
	char	msg[100];
	char* 	(*getMsg)(TextMsg this);
	void 	(*setMsg)(TextMsg this, char* msg);
};


TextMsg TextMsg_create();
void TextMsg_rebind(Message msg);

#endif /* TEXTMSG_H_ */

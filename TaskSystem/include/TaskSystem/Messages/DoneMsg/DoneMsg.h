
#ifndef DONEMSG_H_
#define DONEMSG_H_

#include "TaskSystem/Messages/Message.h"


typedef struct DoneMsg *DoneMsg;

struct DoneMsg {

	/* parent class members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(DoneMsg this);
	DoneMsg 	(*clone)(DoneMsg this);
	void 	(*destroy)(DoneMsg this);

	// Serialization
	int (*writeAt)(DoneMsg this, void* addr);

	// Msg private members
	int success;
};


DoneMsg DoneMsg_create();
void DoneMsg_rebind(Message msg);

#endif /* DONEMSG_H_ */

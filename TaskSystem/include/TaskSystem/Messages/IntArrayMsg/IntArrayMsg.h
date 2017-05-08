
#ifndef INTARRAYMSG_H_
#define INTARRAYMSG_H_

#include "TaskSystem/Messages/Message.h"

typedef struct IntArrayMsg *IntArrayMsg;

struct IntArrayMsg {

	/* parent class members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(IntArrayMsg this);
	IntArrayMsg 	(*clone)(IntArrayMsg this);
	void 	(*destroy)(IntArrayMsg this);

	// Serialization
	int (*writeAt)(IntArrayMsg this, void* addr);

	// Msg private members;
	int size;
	int* values;

	void (*setValues)(IntArrayMsg this, int count, int val[]);
	int (*getSize)(IntArrayMsg this);
	int (*getValue)(IntArrayMsg this, int index);
};


IntArrayMsg IntArrayMsg_create();
void IntArrayMsg_rebind(Message msg);

#endif /* INTARRAYMSG_H_ */

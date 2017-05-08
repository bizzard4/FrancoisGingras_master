
#ifndef REFINTARRAYMSG_H_
#define REFINTARRAYMSG_H_

#include "TaskSystem/Messages/Message.h"

typedef struct RefIntArrayMsg *RefIntArrayMsg;

struct RefIntArrayMsg {

	/* parent class members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(RefIntArrayMsg this);
	RefIntArrayMsg 	(*clone)(RefIntArrayMsg this);
	void 	(*destroy)(RefIntArrayMsg this);

	// Serialization
	int (*writeAt)(RefIntArrayMsg this, void* addr);

	// Msg private members;
	int size;
	int* values;

	void (*setValues)(RefIntArrayMsg this, int count, int val[]);
	int (*getSize)(RefIntArrayMsg this);
	int (*getValue)(RefIntArrayMsg this, int index);
};


RefIntArrayMsg RefIntArrayMsg_create();
void RefIntArrayMsg_rebind(Message msg);

#endif /* REFINTARRAYMSG_H_ */

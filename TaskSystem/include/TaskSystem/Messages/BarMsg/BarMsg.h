
#ifndef BARMSG_H_
#define BARMSG_H_

#include "TaskSystem/Messages/Message.h"


typedef struct BarMsg *BarMsg;

struct BarMsg {

	/* parent class members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(BarMsg this);
	BarMsg 	(*clone)(BarMsg this);
	void 	(*destroy)(BarMsg this);

	// Serialization
	int (*writeAt)(BarMsg this, void* addr);


	/* new child class members */
	int		value;
	int 	(*getValue)(BarMsg this);
	void 	(*setValue)(BarMsg this, int value);
};


BarMsg BarMsg_create();
void BarMsg_rebind(Message msg);

#endif /* BARMSG_H_ */

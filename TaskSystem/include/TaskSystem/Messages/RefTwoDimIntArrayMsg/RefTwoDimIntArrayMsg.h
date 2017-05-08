
#ifndef REFTWODIMINTARRAYMSG_H_
#define REFTWODIMINTARRAYMSG_H_

#include "TaskSystem/Messages/Message.h"

typedef struct RefTwoDimIntArrayMsg *RefTwoDimIntArrayMsg;

struct RefTwoDimIntArrayMsg {

	/* parent class members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(RefTwoDimIntArrayMsg this);
	RefTwoDimIntArrayMsg 	(*clone)(RefTwoDimIntArrayMsg this);
	void 	(*destroy)(RefTwoDimIntArrayMsg this);

	// Serialization
	int (*writeAt)(RefTwoDimIntArrayMsg this, void* addr);

	// Msg private members;
	int size;
	int* counts;
	int** values;
};

RefTwoDimIntArrayMsg RefTwoDimIntArrayMsg_create();
void RefTwoDimIntArrayMsg_rebind(Message msg);

#endif /* REFTWODIMINTARRAYMSG_H_ */

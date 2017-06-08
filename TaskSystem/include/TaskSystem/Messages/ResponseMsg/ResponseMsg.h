

#ifndef RESPONSEMSG_H_
#define RESPONSEMSG_H_

#include "TaskSystem/Messages/Message.h"


typedef struct ResponseMsg *ResponseMsg;

enum {RESPONSE_ERROR, RESPONSE_OK};

// This message will be used over IPC and will perform copy

struct ResponseMsg {

	/* parent class members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(ResponseMsg this);
	ResponseMsg 	(*clone)(ResponseMsg this);
	void 	(*destroy)(ResponseMsg this);

	// Serialization
	int (*writeAt)(ResponseMsg this, void* addr);


	/* new child class members */
	int		code;
	int 	data_size;
	char*	data;

	char* 	(*getData)(ResponseMsg this);
	void 	(*setData)(ResponseMsg this, char* data, int size);
};


ResponseMsg ResponseMsg_create();
void ResponseMsg_rebind(Message msg);

#endif /* RESPONSEMSG_H_ */

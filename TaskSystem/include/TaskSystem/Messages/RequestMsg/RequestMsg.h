

#ifndef REQUESTMSG_H_
#define REQUESTMSG_H_

#include "TaskSystem/Messages/Message.h"


typedef struct RequestMsg *RequestMsg;

enum {SELECT_REQUEST, INSERT_REQUEST, DELETE_REQUEST};

// This message will be used over IPC and will perform copy

struct RequestMsg {

	/* parent class members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(RequestMsg this);
	RequestMsg 	(*clone)(RequestMsg this);
	void 	(*destroy)(RequestMsg this);

	// Serialization
	int (*writeAt)(RequestMsg this, void* addr);


	/* new child class members */
	int		sender_task_id;
	int		request_type; // See enum
	int 	data_size;
	char*	data;

	char* 	(*getData)(RequestMsg this);
	void 	(*setData)(RequestMsg this, char* data, int size);
};


RequestMsg RequestMsg_create();
void RequestMsg_rebind(Message msg);

#endif /* REQUESTMSG_H_ */

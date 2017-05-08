
#ifndef TOPOLOGYMSG_H_
#define TOPOLOGYMSG_H_

#include "TaskSystem/Messages/Message.h"


typedef struct TopologyMsg *TopologyMsg;

struct TopologyMsg {

	/* parent class members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(TopologyMsg this);
	TopologyMsg 	(*clone)(TopologyMsg this);
	void 	(*destroy)(TopologyMsg this);

	// Serialization
	int (*writeAt)(TopologyMsg this, void* addr);

	// Msg private members
	int bucket_count;
	int root_id;
	int sample_size;
	int data_size;
	unsigned int* bucket_ids;
	void (*setBucketIds)(TopologyMsg this, int count, unsigned int ids[]);
	unsigned int (*getBucketId)(TopologyMsg this, int index);
	void (*setRootId)(TopologyMsg this, unsigned int id);
	unsigned int (*getRootId)(TopologyMsg this);
};


TopologyMsg TopologyMsg_create();
void TopologyMsg_rebind(Message msg);

#endif /* TOPOLOGYMSG_H_ */

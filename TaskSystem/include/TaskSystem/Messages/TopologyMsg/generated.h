
#ifndef TOPOLOGYMSG_GENERATED_H_
#define TOPOLOGYMSG_GENERATED_H_

#include <stdio.h>

static int getTag(TopologyMsg this);
static TopologyMsg clone(TopologyMsg this);
static void destroy(TopologyMsg this);
static int writeAt(TopologyMsg this, void* addr);

static void setBucketIds(TopologyMsg this, int count, unsigned int ids[]);
static unsigned int getBucketId(TopologyMsg this, int index);
static void setRootId(TopologyMsg this, unsigned int id);
static unsigned int getRootId(TopologyMsg this);

#define TOPOLOGY_MSG_ID 4

/*
 * Create a new TopologyMsg object and initialize its function pointers.
 */
TopologyMsg TopologyMsg_create(int tag){
	TopologyMsg newRec = malloc(sizeof(struct TopologyMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in ProduceMsg_create");

	newRec->tag = tag;
	newRec->tid = TOPOLOGY_MSG_ID;
	newRec->msg_size = sizeof(struct TopologyMsg);

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->writeAt = writeAt;


	// Private members

	newRec->setBucketIds = setBucketIds;
	newRec->getBucketId = getBucketId;
	newRec->setRootId = setRootId;
	newRec->getRootId = getRootId;

	return newRec;
}

// Rebind method only
void TopologyMsg_rebind(Message msg) {
	TopologyMsg rebind_msg = (TopologyMsg)msg;

	rebind_msg->getTag = getTag;
	rebind_msg->clone = clone;
	rebind_msg->destroy = destroy;

	rebind_msg->writeAt = writeAt;

	// Start of topology msg

	rebind_msg->setBucketIds = setBucketIds;
	rebind_msg->getBucketId = getBucketId;
	rebind_msg->setRootId = setRootId;
	rebind_msg->getRootId = getRootId;

	// Rebind the pointer to the end of the struct position
	rebind_msg->bucket_ids = (void*)((long)msg + sizeof(struct TopologyMsg));
}

#endif /* TOPOLOGYMSG_GENERATED_H_ */

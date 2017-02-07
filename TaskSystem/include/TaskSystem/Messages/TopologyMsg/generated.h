
#ifndef TOPOLOGYMSG_GENERATED_H_
#define TOPOLOGYMSG_GENERATED_H_

#include <stdio.h>

static int getTag(TopologyMsg this);
static TopologyMsg clone(TopologyMsg this);
static void destroy(TopologyMsg this);

static void setBucketIds(TopologyMsg this, int count, unsigned int ids[]);
static unsigned int getBucketId(TopologyMsg this, int index);
static void setRootId(TopologyMsg this, unsigned int id);
static unsigned int getRootId(TopologyMsg this);

/*
 * Create a new TopologyMsg object and initialize its function pointers.
 */
TopologyMsg TopologyMsg_create(int tag){
	TopologyMsg newRec = malloc(sizeof(struct TopologyMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in ProduceMsg_create");

	//printf("MESSAGE CREATION - Creating topology msg with tag=%d\n", tag);
	newRec->tag = tag;

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->setBucketIds = setBucketIds;
	newRec->getBucketId = getBucketId;
	newRec->setRootId = setRootId;
	newRec->getRootId = getRootId;

	return newRec;
}

#endif /* TOPOLOGYMSG_GENERATED_H_ */

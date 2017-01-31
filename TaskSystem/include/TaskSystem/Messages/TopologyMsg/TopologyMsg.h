
#ifndef TOPOLOGYMSG_H_
#define TOPOLOGYMSG_H_


typedef struct TopologyMsg *TopologyMsg;

struct TopologyMsg {

	/* parent class members */
	int tag;

	// methods
	int 	(*getTag)(TopologyMsg this);
	TopologyMsg 	(*clone)(TopologyMsg this);
	void 	(*destroy)(TopologyMsg this);

	// Msg private members
	int bucket_count;
	int root_id;
	unsigned int* bucket_ids;
	void (*setBucketIds)(TopologyMsg this, int count, unsigned int ids[]);
	unsigned int (*getBucketId)(TopologyMsg this, int index);
	void (*setRootId)(TopologyMsg this, unsigned int id);
	unsigned int (*getRootId)(TopologyMsg this);
};


TopologyMsg TopologyMsg_create();

#endif /* TOPOLOGYMSG_H_ */

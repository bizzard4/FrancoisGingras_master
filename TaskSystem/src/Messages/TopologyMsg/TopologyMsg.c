#include "TaskSystem/Messages/TopologyMsg/TopologyMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TaskSystem/Messages/TopologyMsg/generated.h"

/******************************
 * Programmer Code
 *****************************/

static int getTag(TopologyMsg this){
	return this->tag;
}

static TopologyMsg clone(TopologyMsg this){
	TopologyMsg tmp = TopologyMsg_create(0);
	tmp->tag = this->tag;

	tmp->bucket_count = this->bucket_count;
	tmp->root_id = this->root_id;
	tmp->bucket_ids = malloc(this->bucket_count * sizeof(unsigned int)); // Deep copy
	for (int i = 0; i < this->bucket_count; i++) {
		tmp->bucket_ids[i] = this->bucket_ids[i];
	}

	return tmp;
}

static void destroy(TopologyMsg this){
	free(this->bucket_ids);
	free(this);
}

static void setBucketIds(TopologyMsg this, int count, unsigned int ids[]) {
	this->bucket_count = count;

	this->bucket_ids = malloc(count * sizeof(unsigned int));
	for (int i = 0; i < count; i++) {
		this->bucket_ids[i] = ids[i];
	}
}

static unsigned int getBucketId(TopologyMsg this, int index) {
	return this->bucket_ids[index];
}

static void setRootId(TopologyMsg this, unsigned int id) {
	this->root_id = id;
}

static unsigned int getRootId(TopologyMsg this) {
	return this->root_id;
}



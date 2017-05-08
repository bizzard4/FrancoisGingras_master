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
	TopologyMsg tmp = TopologyMsg_create(this->tag);
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;


	tmp->bucket_count = this->bucket_count;
	tmp->root_id = this->root_id;
	tmp->sample_size = this->sample_size;
	tmp->data_size = this->data_size;
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

static int writeAt(TopologyMsg this, void* addr) {
	TopologyMsg tmp = (TopologyMsg)addr;
	tmp->tag = this->tag;
	tmp->tid = this->tid;
	tmp->msg_size = this->msg_size;

	// Private members

	tmp->bucket_count = this->bucket_count;
	tmp->root_id = this->root_id;
	tmp->sample_size = this->sample_size;
	tmp->data_size = this->data_size;

	// Allocate the array after the struct and update msg_size
	tmp->bucket_ids = (unsigned int*)memcpy((void*)((long)addr + sizeof(struct TopologyMsg)), this->bucket_ids, this->bucket_count * sizeof(unsigned int));
	this->msg_size += this->bucket_count * sizeof(unsigned int);

	return this->msg_size;
}

static void setBucketIds(TopologyMsg this, int count, unsigned int ids[]) {
	this->bucket_count = count;
	this->msg_size = sizeof(struct TopologyMsg) + (count * sizeof(unsigned int));

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




#ifndef PRODUCEMSG_GENERATED_H_
#define PRODUCEMSG_GENERATED_H_

#include <stdio.h>

static int getTag(ProduceMsg this);
static ProduceMsg clone(ProduceMsg this);
static void destroy(ProduceMsg this);

static int getProduceId(ProduceMsg this);
static void setProduceId(ProduceMsg this, int value);
static int getProduceValue(ProduceMsg this);
static void setProduceValue(ProduceMsg this, int value);


/*
 * Create a new ProduceMsg object and initialize its function pointers.
 */
ProduceMsg ProduceMsg_create(int tag){
	ProduceMsg newRec = malloc(sizeof(struct ProduceMsg));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in ProduceMsg_create");

	newRec->tag = tag;

	newRec->getTag = getTag;
	newRec->clone = clone;
	newRec->destroy = destroy;

	newRec->getProduceId = getProduceId;
	newRec->setProduceId = setProduceId;
	newRec->getProduceValue = getProduceValue;
	newRec->setProduceValue = setProduceValue;

	return newRec;
}

#endif /* PRODUCEMSG_GENERATED_H_ */

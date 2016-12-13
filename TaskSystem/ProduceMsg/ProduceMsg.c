#include "ProduceMsg.h"
#include "fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "generated.h"

/******************************
 * Programmer Code
 *****************************/

static int getTag(ProduceMsg this){
	return this->tag;
}

static ProduceMsg clone(ProduceMsg this){
	printf("IN PRODUCE CLONE\n");
	ProduceMsg tmp = ProduceMsg_create(0);
	tmp->tag = this->tag;
	tmp->producer_id = this->producer_id;
	tmp->produce_value = this->produce_value;

	return tmp;
}

static void destroy(ProduceMsg this){
	free(this);
}


static int getProduceId(ProduceMsg this){
	return this->producer_id;
}


static void setProduceId(ProduceMsg this, int value){
	this->producer_id = value;
}

static int getProduceValue(ProduceMsg this){
	return this->produce_value;
}


static void setProduceValue(ProduceMsg this, int value){
	this->produce_value = value;
}



#include "TaskSystem/Messages/RefIntArrayMsg/RefIntArrayMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TaskSystem/Messages/RefIntArrayMsg/generated.h"

/******************************
 * Programmer Code
 *****************************/

static int getTag(RefIntArrayMsg this){
	return this->tag;
}

static RefIntArrayMsg clone(RefIntArrayMsg this){
	RefIntArrayMsg tmp = RefIntArrayMsg_create(this->tag);
	tmp->tag = this->tag;

	tmp->size = this->size;
	tmp->values = this->values;

	return tmp;
}

// Who will be responsible to destroy the final array? Main creator?
static void destroy(RefIntArrayMsg this){
	free(this);
}

static void setValues(RefIntArrayMsg this, int count, int* pointer_to_values) {
	this->size = count;
	this->values = pointer_to_values;
}

static int getSize(RefIntArrayMsg this) {
	return this->size;
}

static int getValue(RefIntArrayMsg this, int index) {
	return this->values[index];
}



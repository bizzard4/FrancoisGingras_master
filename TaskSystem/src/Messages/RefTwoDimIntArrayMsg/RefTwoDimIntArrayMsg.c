#include "TaskSystem/Messages/RefTwoDimIntArrayMsg/RefTwoDimIntArrayMsg.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "TaskSystem/Messages/RefTwoDimIntArrayMsg/generated.h"

/******************************
 * Programmer Code
 *****************************/

static int getTag(RefTwoDimIntArrayMsg this){
	return this->tag;
}

static RefTwoDimIntArrayMsg clone(RefTwoDimIntArrayMsg this){
	RefTwoDimIntArrayMsg tmp = RefTwoDimIntArrayMsg_create(this->tag);
	tmp->tag = this->tag;

	tmp->size = this->size;
	tmp->counts = this->counts;
	tmp->values = this->values;

	return tmp;
}

// Who will be responsible to destroy the final array? Main creator?
static void destroy(RefTwoDimIntArrayMsg this){
	free(this);
}


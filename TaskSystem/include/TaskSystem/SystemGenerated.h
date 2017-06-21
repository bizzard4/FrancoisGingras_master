#ifndef SYSTEMGENERATED_H_
#define SYSTEMGENERATED_H_

// This file will have to be generated at compilation.

#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/Messages/TextMsg/TextMsg.h"
#include "TaskSystem/Messages/RefIntArrayMsg/RefIntArrayMsg.h"
#include "TaskSystem/Messages/TopologyMsg/TopologyMsg.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h"
#include "TaskSystem/Messages/DoneMsg/DoneMsg.h"
#include "TaskSystem/Messages/RefTwoDimIntArrayMsg/RefTwoDimIntArrayMsg.h"
#include "TaskSystem/Messages/RequestMsg/RequestMsg.h"
#include "TaskSystem/Messages/ResponseMsg/ResponseMsg.h"

#define MAX_MESSAGE_TYPE 20

/**
 * Global mapping.
 */
void (*rebinder[MAX_MESSAGE_TYPE])(Message msg);

/**
 * Build the rebinder.
 */
void build_mapping();

#endif /* SYSTEMGENERATED_H_ */
#include "TaskSystem/SystemGenerated.h"

#include <stdio.h>

// This file will have to be generated at compilation.

/**
 * This will have to be build automatically.
 */
void build_mapping() {
	for (int i = 0; i < MAX_MESSAGE_TYPE; i++) {
		rebinder[i] = NULL;
	}

	// Should be generated
	rebinder[1] = &BarMsg_rebind;
	rebinder[2] = &TextMsg_rebind;
	rebinder[3] = &RefIntArrayMsg_rebind;
	rebinder[4] = &TopologyMsg_rebind;
	rebinder[5] = &IntArrayMsg_rebind;
	rebinder[6] = &DoneMsg_rebind;
	rebinder[7] = &RefTwoDimIntArrayMsg_rebind;
	rebinder[8] = &RequestMsg_rebind;
	rebinder[9] = &ResponseMsg_rebind;
}
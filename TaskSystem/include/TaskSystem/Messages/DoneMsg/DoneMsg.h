
#ifndef DONEMSG_H_
#define DONEMSG_H_


typedef struct DoneMsg *DoneMsg;

struct DoneMsg {

	/* parent class members */
	int tag;

	// methods
	int 	(*getTag)(DoneMsg this);
	DoneMsg 	(*clone)(DoneMsg this);
	void 	(*destroy)(DoneMsg this);

	// Msg private members
	int success;
};


DoneMsg DoneMsg_create();

#endif /* DONEMSG_H_ */

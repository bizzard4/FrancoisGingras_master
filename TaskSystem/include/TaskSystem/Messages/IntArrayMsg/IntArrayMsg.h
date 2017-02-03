
#ifndef INTARRAYMSG_H_
#define INTARRAYMSG_H_


typedef struct IntArrayMsg *IntArrayMsg;

struct IntArrayMsg {

	/* parent class members */
	int tag;

	// methods
	int 	(*getTag)(IntArrayMsg this);
	IntArrayMsg 	(*clone)(IntArrayMsg this);
	void 	(*destroy)(IntArrayMsg this);

	// Msg private members;
	int size;
	int* values;

	void (*setValues)(IntArrayMsg this, int count, int val[]);
	int (*getSize)(IntArrayMsg this);
	int (*getValue)(IntArrayMsg this, int index);
};


IntArrayMsg IntArrayMsg_create();

#endif /* INTARRAYMSG_H_ */

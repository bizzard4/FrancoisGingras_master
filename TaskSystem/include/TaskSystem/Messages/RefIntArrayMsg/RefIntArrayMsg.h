
#ifndef REFINTARRAYMSG_H_
#define REFINTARRAYMSG_H_


typedef struct RefIntArrayMsg *RefIntArrayMsg;

struct RefIntArrayMsg {

	/* parent class members */
	int tag;

	// methods
	int 	(*getTag)(RefIntArrayMsg this);
	RefIntArrayMsg 	(*clone)(RefIntArrayMsg this);
	void 	(*destroy)(RefIntArrayMsg this);

	// Msg private members;
	int size;
	int* values;

	void (*setValues)(RefIntArrayMsg this, int count, int val[]);
	int (*getSize)(RefIntArrayMsg this);
	int (*getValue)(RefIntArrayMsg this, int index);
};


RefIntArrayMsg RefIntArrayMsg_create();

#endif /* REFINTARRAYMSG_H_ */

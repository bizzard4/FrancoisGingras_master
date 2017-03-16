
#ifndef REFTWODIMINTARRAYMSG_H_
#define REFTWODIMINTARRAYMSG_H_


typedef struct RefTwoDimIntArrayMsg *RefTwoDimIntArrayMsg;

struct RefTwoDimIntArrayMsg {

	/* parent class members */
	int tag;

	// methods
	int 	(*getTag)(RefTwoDimIntArrayMsg this);
	RefTwoDimIntArrayMsg 	(*clone)(RefTwoDimIntArrayMsg this);
	void 	(*destroy)(RefTwoDimIntArrayMsg this);

	// Msg private members;
	int size;
	int* counts;
	int** values;
};

RefTwoDimIntArrayMsg RefTwoDimIntArrayMsg_create();

#endif /* REFTWODIMINTARRAYMSG_H_ */


#ifndef BARMSG_H_
#define BARMSG_H_


typedef struct BarMsg *BarMsg;

struct BarMsg {

	/* parent class members */
	int tag;

	// methods
	int 	(*getTag)(BarMsg this);
	BarMsg 	(*clone)(BarMsg this);
	void 	(*destroy)(BarMsg this);


	/* new child class members */
	int		value;
	int 	(*getValue)(BarMsg this);
	void 	(*setValue)(BarMsg this, int value);
};


BarMsg BarMsg_create();

#endif /* BARMSG_H_ */


#ifndef TEXTMSG_H_
#define TEXTMSG_H_


typedef struct TextMsg *TextMsg;

struct TextMsg {

	/* parent class members */
	int tag;

	// methods
	int 	(*getTag)(TextMsg this);
	TextMsg 	(*clone)(TextMsg this);
	void 	(*destroy)(TextMsg this);


	/* new child class members */
	char*	msg;
	char* 	(*getMsg)(TextMsg this);
	void 	(*setMsg)(TextMsg this, char* msg);
};


TextMsg TextMsg_create();

#endif /* TEXTMSG_H_ */

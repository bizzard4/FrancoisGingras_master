
#ifndef Message_H_
#define Message_H_


/*
 * The Message object functions as a generic interface. It is never
 * instantiated directly (there is no Message.c file). Instead, it is used
 * by other classes as a generic parameter type. This allows methods to
 * be written so that they can accept any type of message object, as long as
 * it contains the same members and methods. Other message objects can,
 * of course, provide other methods, but they must always be added AFTER
 * the members define in the basic Message object.
 */


typedef struct Message *Message;

struct Message {

	/* members */
	int tag;
	int tid;
	int msg_size;

	// methods
	int 	(*getTag)(Message this);
	Message (*clone)(Message this);
	void 	(*destroy)(Message this);

	// Serialization
	int (*writeAt)(Message this, void* addr);

};



Message Message_create();

#endif /* Message_H_ */

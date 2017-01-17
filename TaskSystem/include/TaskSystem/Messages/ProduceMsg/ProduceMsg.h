
#ifndef PRODUCEMSG_H_
#define PRODUCEMSG_H_


typedef struct ProduceMsg *ProduceMsg;

struct ProduceMsg {

	/* parent class members */
	int tag;

	// methods
	int 	(*getTag)(ProduceMsg this);
	ProduceMsg 	(*clone)(ProduceMsg this);
	void 	(*destroy)(ProduceMsg this);


	/* new child class members */
	int		producer_id;
	int		produce_value;
	int 	(*getProduceId)(ProduceMsg this);
	void 	(*setProduceId)(ProduceMsg this, int value);
	int 	(*getProduceValue)(ProduceMsg this);
	void 	(*setProduceValue)(ProduceMsg this, int value);
};


ProduceMsg ProduceMsg_create();

#endif /* PRODUCEMSG_H_ */

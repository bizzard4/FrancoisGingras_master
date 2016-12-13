#include "MsgQ.h"
#include "System.h"
#include "fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "PapaTask.h"
#include "Message.h"


static MsgQ TaskTable[1000]; // should be dynamically sized!
static unsigned int nextTaskID = 1;

pthread_mutex_t TaskIDLock = PTHREAD_MUTEX_INITIALIZER;



static void send(Message data, int targetTaskID){
	Message sendObj = data->clone(data);
	Enqueue(sendObj, TaskTable[targetTaskID]);
}


static Message receive(int targetTaskID){
	return FrontAndDequeue(TaskTable[targetTaskID]);
}


static void dropMsg(int targetTaskID){
	Message msg = FrontAndDequeue(TaskTable[targetTaskID]);
	msg->destroy(msg);

}

static int getMsgTag(int targetTaskID){

	if(IsEmpty(TaskTable[targetTaskID]))
		return -1;

	Message msg = Front(TaskTable[targetTaskID]);
	return msg->getTag(msg);
}


static unsigned int getNextTaskID(){
	pthread_mutex_lock (&TaskIDLock);
	unsigned int nextID = nextTaskID++;
	pthread_mutex_unlock (&TaskIDLock);

	return nextID;
}


static void createMsgQ(unsigned int taskID){
	TaskTable[taskID] = CreateMsgQ(10000);
}


static void destroy(System this){
	free(this);
}






/*
 * With other objects, the create method was placed into a "generated"
 * header file. We don't do this here since everything in this file
 * is provided by the language framework anyway.
 */
System System_create(){
	System newRec = malloc(sizeof(struct System));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in System_create");

	newRec->send = send;
	newRec->receive = receive;
	newRec->dropMsg = dropMsg;
	newRec->getMsgTag = getMsgTag;
	newRec->getNextTaskID = getNextTaskID;
	newRec->createMsgQ = createMsgQ;;
	newRec->destroy = destroy;

	return newRec;
}

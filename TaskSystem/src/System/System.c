#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


static void send(System this, Message data, int targetTaskID){
	Message sendObj = data->clone(data);
	Enqueue(this->TaskTable[targetTaskID], sendObj);
}


static Message receive(System this, int targetTaskID){
	return Dequeue(this->TaskTable[targetTaskID]);
}


static void dropMsg(System this, int targetTaskID){
	Message msg = Dequeue(this->TaskTable[targetTaskID]);
	if (msg != NULL) {
		msg->destroy(msg);
	}

}

static int getMsgTag(System this, int targetTaskID){

	// If Q is empty, we go to sleep
	if(IsEmpty(this->TaskTable[targetTaskID])) {
		pthread_mutex_lock(&(Comm->sleepers_lock));
 		pthread_cond_wait(&(Comm->sleepers[targetTaskID]), &(Comm->sleepers_lock));
 		pthread_mutex_unlock(&(Comm->sleepers_lock));
	}

	// Recheck
	Message msg = Peek(this->TaskTable[targetTaskID]);
	if (msg == NULL) {
		return -1;
	}
	return msg->getTag(msg);
}


static unsigned int getNextTaskID(System this){
	pthread_mutex_lock(&(this->TaskIDLock));
	unsigned int nextID = this->nextTaskID;
	this->nextTaskID++;
	pthread_mutex_unlock(&(this->TaskIDLock));

	return nextID;
}


static void createMsgQ(System this, unsigned int taskID){
	this->TaskTable[taskID] = CreateQueue();
}


static void destroy(System this){
	this->shutdown_signal = 1;
	// Wait for signal/wait loop to finish
	pthread_join(this->threadRef, NULL);
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

	// Methods
	newRec->send = send;
	newRec->receive = receive;
	newRec->dropMsg = dropMsg;
	newRec->getMsgTag = getMsgTag;
	newRec->getNextTaskID = getNextTaskID;
	newRec->createMsgQ = createMsgQ;;
	newRec->destroy = destroy;

	// Initialize variables
	newRec->nextTaskID = 1;
	int mutex_res = pthread_mutex_init(&(newRec->TaskIDLock), NULL);
	if (mutex_res) {
		printf("ERROR; TaskIDLock pthread_mutex_init is %d\n", mutex_res);
		exit(-1);
	}

	newRec->shutdown_signal = 0;
	mutex_res = pthread_mutex_init(&(newRec->sleepers_lock), NULL);
	if (mutex_res) {
		printf("ERROR; Sleeper lock pthread_mutex_init is %d\n", mutex_res);
		exit(-1);
	}
	pthread_mutex_lock(&(newRec->sleepers_lock));
	for (int i = 0; i < 100; i++) {
		pthread_cond_init(&(newRec->sleepers[i]), NULL);
	}
	pthread_mutex_unlock(&(newRec->sleepers_lock));

	// Wait signal thread
	int result = pthread_create(&(newRec->threadRef), NULL, run, (void *)newRec);
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", result);
	    exit(-1);
	}

	return newRec;
}

static void* run(void* SystemRef) {
	System this = (System)SystemRef;
	loop_wait_signal(this);
	pthread_exit(NULL);
}

static void loop_wait_signal(System this) {
	printf("System loop for wait and signal is started\n");

	while(this->shutdown_signal <= 0) {
		pthread_mutex_lock (&(this->TaskIDLock));
		int current_max_id = this->nextTaskID;
		pthread_mutex_unlock (&(this->TaskIDLock));

		for (int i = 0; i < current_max_id; i++) {
			if(!IsEmpty(this->TaskTable[i])) {
				pthread_cond_signal(&(this->sleepers[i]));
			}
		}

		usleep(10000); // 10ms
	}
	
	printf("System loop for wait and signal has shutdown\n");
}

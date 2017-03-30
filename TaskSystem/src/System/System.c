#include "TaskSystem/UnboundedMsgQ.h"
#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


static Queue TaskTable[100]; // should be dynamically sized!
static unsigned int nextTaskID = 1;

pthread_mutex_t TaskIDLock = PTHREAD_MUTEX_INITIALIZER;



static void send(Message data, int targetTaskID){
	Message sendObj = data->clone(data);
	Enqueue(TaskTable[targetTaskID], sendObj);
}


static Message receive(int targetTaskID){
	return Dequeue(TaskTable[targetTaskID]);
}


static void dropMsg(int targetTaskID){
	Message msg = Dequeue(TaskTable[targetTaskID]);
	if (msg != NULL) {
		msg->destroy(msg);
	}

}

static int getMsgTag(int targetTaskID){

	// If Q is empty, we go to sleep
	if(IsEmpty(TaskTable[targetTaskID])) {
		pthread_mutex_lock(&(Comm->sleepers_lock));
 		pthread_cond_wait(&(Comm->sleepers[targetTaskID]), &(Comm->sleepers_lock));
 		pthread_mutex_unlock(&(Comm->sleepers_lock));
	}

	// Recheck
	Message msg = Peek(TaskTable[targetTaskID]);
	if (msg == NULL) {
		return -1;
	}
	return msg->getTag(msg);
}


static unsigned int getNextTaskID(){
	pthread_mutex_lock (&TaskIDLock);
	unsigned int nextID = nextTaskID++;
	pthread_mutex_unlock (&TaskIDLock);

	return nextID;
}


static void createMsgQ(unsigned int taskID){
	TaskTable[taskID] = CreateQueue();
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

	newRec->send = send;
	newRec->receive = receive;
	newRec->dropMsg = dropMsg;
	newRec->getMsgTag = getMsgTag;
	newRec->getNextTaskID = getNextTaskID;
	newRec->createMsgQ = createMsgQ;;
	newRec->destroy = destroy;

	newRec->shutdown_signal = 0;
	int mutex_res = pthread_mutex_init(&(newRec->sleepers_lock), NULL);
	if (mutex_res) {
		printf("ERROR; Sleeper lock pthread_mutex_init is %d\n", mutex_res);
		exit(-1);
	}

	pthread_mutex_lock(&(newRec->sleepers_lock));
	for (int i = 0; i < 100; i++) {
		pthread_cond_init(&(newRec->sleepers[i]), NULL);
	}
	pthread_mutex_unlock(&(newRec->sleepers_lock));

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
		pthread_mutex_lock (&TaskIDLock);
		int current_max_id = nextTaskID;
		pthread_mutex_unlock (&TaskIDLock);

		for (int i = 0; i < current_max_id; i++) {
			if(!IsEmpty(TaskTable[i])) {
				pthread_cond_signal(&(this->sleepers[i]));
			}
		}

		usleep(10000); // 10ms
	}
	
	printf("System loop for wait and signal has shutdown\n");
}

#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/shm.h>

static void send(System this, Message msg_data, int targetTaskID){
	Message sendObj = msg_data->clone(msg_data);
	Enqueue(this->data->TaskTable[targetTaskID], sendObj);
}


static Message receive(System this, int targetTaskID){
	return Dequeue(this->data->TaskTable[targetTaskID]);
}


static void dropMsg(System this, int targetTaskID){
	Message msg = Dequeue(this->data->TaskTable[targetTaskID]);
	if (msg != NULL) {
		msg->destroy(msg);
	}

}

static int getMsgTag(System this, int targetTaskID){

	// If Q is empty, we go to sleep
	if(IsEmpty(this->data->TaskTable[targetTaskID])) {
		pthread_mutex_lock(&(this->data->sleepers_lock));
 		pthread_cond_wait(&(this->data->sleepers[targetTaskID]), &(this->data->sleepers_lock));
 		pthread_mutex_unlock(&(this->data->sleepers_lock));
	}

	// Recheck
	Message msg = Peek(this->data->TaskTable[targetTaskID]);
	if (msg == NULL) {
		return -1;
	}
	return msg->getTag(msg);
}


static unsigned int getNextTaskID(System this){
	pthread_mutex_lock(&(this->data->TaskIDLock));
	unsigned int nextID = this->data->nextTaskID;
	this->data->nextTaskID++;
	pthread_mutex_unlock(&(this->data->TaskIDLock));

	return nextID;
}


static void createMsgQ(System this, unsigned int taskID){
	this->data->TaskTable[taskID] = CreateQueue();
}


static void destroy(System this){
	this->data->shutdown_signal = 1;
	// Wait for signal/wait loop to finish
	pthread_join(this->data->threadRef, NULL);

	 // Detach and remove shared memory
	shmctl(this->shmid, IPC_RMID, NULL);
}

/*
 * With other objects, the create method was placed into a "generated"
 * header file. We don't do this here since everything in this file
 * is provided by the language framework anyway.
 */
System System_create(){
	// Create the shard space
	int size = sizeof(struct SystemData);
	key_t key = ftok(SYSTEM_SHARED_MEM_NAME, 'b');
	int tmp_shmid = shmget(key, size, IPC_CREAT | 0666);
	if (tmp_shmid < 0) {
		printf("ERROR; Error creating shared segment \n");
		exit(-1);
	}

	void* addr = shmat(tmp_shmid, NULL, 0);
	if ((intptr_t)addr == -1) {
		printf("ERROR; Shmat failed %d\n", errno);
		exit(-1);	
	}

	SystemData shared_data = (SystemData)addr;
	System newRec = malloc(sizeof(struct System));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in System_create");
	newRec->shmid = tmp_shmid;
	newRec->data = shared_data;

	// Methods
	newRec->send = send;
	newRec->receive = receive;
	newRec->dropMsg = dropMsg;
	newRec->getMsgTag = getMsgTag;
	newRec->getNextTaskID = getNextTaskID;
	newRec->createMsgQ = createMsgQ;;
	newRec->destroy = destroy;

	// Initialize variables
	newRec->data->nextTaskID = 1;
	int mutex_res = pthread_mutex_init(&(newRec->data->TaskIDLock), NULL);
	if (mutex_res) {
		printf("ERROR; TaskIDLock pthread_mutex_init is %d\n", mutex_res);
		exit(-1);
	}

	newRec->data->shutdown_signal = 0;
	mutex_res = pthread_mutex_init(&(newRec->data->sleepers_lock), NULL);
	if (mutex_res) {
		printf("ERROR; Sleeper lock pthread_mutex_init is %d\n", mutex_res);
		exit(-1);
	}
	pthread_mutex_lock(&(newRec->data->sleepers_lock));
	for (int i = 0; i < 100; i++) {
		pthread_cond_init(&(newRec->data->sleepers[i]), NULL);
	}
	pthread_mutex_unlock(&(newRec->data->sleepers_lock));

	// Wait signal thread
	int result = pthread_create(&(newRec->data->threadRef), NULL, run, (void *)newRec);
	if (result){
		printf("ERROR; return code from pthread_create() is %d\n", result);
	    exit(-1);
	}

	return newRec;
}

/*
 * This method will acquire an existing system from the shared memory.
 */
System System_acquire() {
	// Get the shared memory
	int size = sizeof(struct SystemData);
	key_t key = ftok(SYSTEM_SHARED_MEM_NAME, 'b');
	int tmp_shmid = shmget(key, size, 0);
	if (tmp_shmid < 0) {
		printf("ERROR; Error getting shared segment \n");
		exit(-1);
	}

	void* addr = shmat(tmp_shmid, NULL, 0);
	if ((intptr_t)addr == -1) {
		printf("ERROR; Shmat failed %d\n", errno);
		exit(-1);	
	}

	SystemData shared_data = (SystemData)addr;

	System newRec = malloc(sizeof(struct System));
	newRec->shmid = tmp_shmid;
	newRec->data = shared_data;

	// Methods
	newRec->send = send;
	newRec->receive = receive;
	newRec->dropMsg = dropMsg;
	newRec->getMsgTag = getMsgTag;
	newRec->getNextTaskID = getNextTaskID;
	newRec->createMsgQ = createMsgQ;;
	newRec->destroy = destroy;

	return newRec;
}

static void* run(void* SystemRef) {
	System this = (System)SystemRef;
	loop_wait_signal(this);
	pthread_exit(NULL);
}

static void loop_wait_signal(System this) {
	printf("System loop for wait and signal is started\n");

	while(this->data->shutdown_signal <= 0) {
		pthread_mutex_lock (&(this->data->TaskIDLock));
		int current_max_id = this->data->nextTaskID;
		pthread_mutex_unlock (&(this->data->TaskIDLock));

		for (int i = 0; i < current_max_id; i++) {
			if(!IsEmpty(this->data->TaskTable[i])) {
				pthread_cond_signal(&(this->data->sleepers[i]));
			}
		}

		usleep(10000); // 10ms
	}
	
	printf("System loop for wait and signal has shutdown\n");
}

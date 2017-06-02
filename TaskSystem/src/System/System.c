#include "TaskSystem/System.h"
#include "TaskSystem/fatal.h"

// TO REMOVE
#include "TaskSystem/Messages/BarMsg/BarMsg.h"
#include "TaskSystem/Messages/TextMsg/TextMsg.h"
#include "TaskSystem/Messages/RefIntArrayMsg/RefIntArrayMsg.h"
#include "TaskSystem/Messages/TopologyMsg/TopologyMsg.h"
#include "TaskSystem/Messages/IntArrayMsg/IntArrayMsg.h"
#include "TaskSystem/Messages/DoneMsg/DoneMsg.h"
#include "TaskSystem/Messages/RefTwoDimIntArrayMsg/RefTwoDimIntArrayMsg.h"
#include "TaskSystem/Messages/RequestMsg/RequestMsg.h"
#include "TaskSystem/Messages/ResponseMsg/ResponseMsg.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

static void send(System this, Message msg_data, int targetTaskID){
	if (this->TaskTable[targetTaskID] == NULL) {
		char buf[15];
		sprintf(buf, "/TS_T%d", targetTaskID);
		this->TaskTable[targetTaskID] = AcquireQueue(buf);
	}

	// TODO : Need to check enq return value to detect array full, busy loop may be used
	int res = Enqueue(this->TaskTable[targetTaskID], msg_data);
	if (res == 0) {
		printf("System error: SEND FAILED to Task %d\n", targetTaskID);
		exit(-1);
	}
}


static Message receive(System this, int targetTaskID){
	Message msg = Dequeue(this->TaskTable[targetTaskID]);
	// TODO : Map to rebind and clone
	if (msg->tid == 1) { // BAR MSG
		BarMsg_rebind(msg);
		// Msg is still from shared memory, this need to be done inside the Dequeue or
		// inside a lock
		return msg->clone(msg);
	} else if (msg->tid == 2) {
		TextMsg_rebind(msg);
		return msg->clone(msg);
	} else if (msg->tid == 3) {
		RefIntArrayMsg_rebind(msg);
		return msg->clone(msg);
	} else if (msg->tid == 4) {
		TopologyMsg_rebind(msg);
		return msg->clone(msg);
	} else if (msg->tid == 5) {
		IntArrayMsg_rebind(msg);
		return msg->clone(msg);
	} else if (msg->tid == 6) {
		DoneMsg_rebind(msg);
		return msg->clone(msg);
	} else if (msg->tid == 7) {
		RefTwoDimIntArrayMsg_rebind(msg);
		return msg->clone(msg);
	} else if (msg->tid == 8) {
		RequestMsg_rebind(msg);
		return msg->clone(msg);
	} else if (msg->tid == 9) {
		ResponseMsg_rebind(msg);
		return msg->clone(msg);
	} else {
		Queue tmp = this->TaskTable[targetTaskID];
		printf("SYSTEM ERROR : Unkown message %p type %d, tag=%d\n", msg, msg->tid, msg->tag);
		exit(-1);
	}
}


static void dropMsg(System this, int targetTaskID){
	if (this->TaskTable[targetTaskID] == NULL) {
		char buf[15];
		sprintf(buf, "/TS_T%d", targetTaskID);
		this->TaskTable[targetTaskID] = AcquireQueue(buf);
	}

	Dequeue(this->TaskTable[targetTaskID]);
}

static int getMsgTag(System this, int targetTaskID){
	if (this->TaskTable[targetTaskID] == NULL) {
		char buf[15];
		sprintf(buf, "/TS_T%d", targetTaskID);
		this->TaskTable[targetTaskID] = AcquireQueue(buf);
	}

	// Recheck
	Message msg = Peek(this->TaskTable[targetTaskID]);
	if (msg == NULL) {
		return -1;
	}
	return msg->tag; // USING METHOD WILL FAIL IF NOT REBOUND
}


static unsigned int getNextTaskID(System this){
	pthread_mutex_lock(&(this->data->TaskIDLock));
	unsigned int nextID = this->data->nextTaskID;
	this->data->nextTaskID++;
	pthread_mutex_unlock(&(this->data->TaskIDLock));

	return nextID;
}


static void createMsgQ(System this, unsigned int taskID){
	char buf[15];
	sprintf(buf, "/TS_T%d", taskID);
	this->TaskTable[taskID] = CreateQueue(buf);
}


static void destroy(System this){
	this->data->shutdown_signal = 1;
	// Wait for signal/wait loop to finish
	pthread_join(this->data->threadRef, NULL);

	// Detach each task shared memory
	for (int i = 1; i < this->data->nextTaskID; i++) {
		char buf[15];
		sprintf(buf, "/TS_T%d", i);
		DeleteQueue(buf);
	}

	 // Detach and remove shared memory
	shm_unlink(SYSTEM_SHARED_MEM_NAME);
}

/*
 * Return 1 if a message is in the Q.
 */
static int message_immediate(System this, unsigned int taskID) {
	if (this->TaskTable[taskID] == NULL) {
		char buf[15];
		sprintf(buf, "/TS_T%d", taskID);
		this->TaskTable[taskID] = AcquireQueue(buf);
	}

	return IsEmpty(this->TaskTable[taskID]);
}

/*
 * Put thread to sleep if no message is in the Q. Will be woke-up by the signal loop when
 * a message is present.
 */
static void message_notify(System this, unsigned int taskID) {
	// If Q is empty, we go to sleep
	if(message_immediate(this, taskID)) {
		pthread_mutex_lock(&(this->data->sleepers_lock));
 		pthread_cond_wait(&(this->data->sleepers[taskID]), &(this->data->sleepers_lock));
 		pthread_mutex_unlock(&(this->data->sleepers_lock));
	}
}

/*
 * Yield the CPU.
 */
static void message_wait(System this, unsigned int taskID) {
	sched_yield();
}

/*
 * With other objects, the create method was placed into a "generated"
 * header file. We don't do this here since everything in this file
 * is provided by the language framework anyway.
 */
System System_create(){
	// Create the shard space
	int size = sizeof(struct SystemData);
	// TODO : Use O_EXCL to detect existance
	int fd = shm_open(SYSTEM_SHARED_MEM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		printf("ERROR; Error creating shared segment %d \n", errno);
		exit(-1);
	}

	if (ftruncate(fd, size) == -1) {
		printf("ERROR; Error truncating shared segment \n");
		exit(-1);
	}

	void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		printf("ERROR; mmap failed %d\n", errno);
		exit(-1);	
	}

	SystemData shared_data = (SystemData)addr;
	System newRec = malloc(sizeof(struct System));
	if(newRec == NULL)
		FatalError("Cannot allocate memory in System_create");
	newRec->data = shared_data;

	// Methods
	newRec->send = send;
	newRec->receive = receive;
	newRec->dropMsg = dropMsg;
	newRec->getMsgTag = getMsgTag;
	newRec->getNextTaskID = getNextTaskID;
	newRec->createMsgQ = createMsgQ;;
	newRec->destroy = destroy;
	newRec->message_immediate = message_immediate;	
	newRec->message_notify = message_notify;
	newRec->message_wait = message_wait;

	// Local addr and SHM-id
	for (int i = 0; i < MAX_TASK; i++) {
		newRec->TaskTable[i] = NULL;
	}

	// Initialize ID and ID mutex
	newRec->data->nextTaskID = 1;
	pthread_mutexattr_t m_attr;
	pthread_mutexattr_init(&m_attr);
	pthread_mutexattr_setpshared(&m_attr, PTHREAD_PROCESS_SHARED);
	int mutex_res = pthread_mutex_init(&(newRec->data->TaskIDLock), &m_attr);
	if (mutex_res) {
		printf("ERROR; TaskIDLock pthread_mutex_init is %d\n", mutex_res);
		exit(-1);
	}

	// Initialize shutdown signal, wait and signal condition
	newRec->data->shutdown_signal = 0;
	mutex_res = pthread_mutex_init(&(newRec->data->sleepers_lock), &m_attr);
	if (mutex_res) {
		printf("ERROR; Sleeper lock pthread_mutex_init is %d\n", mutex_res);
		exit(-1);
	}
	pthread_condattr_t cond_attr;
    pthread_condattr_init(&cond_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_lock(&(newRec->data->sleepers_lock));
	for (int i = 0; i < 100; i++) {
		pthread_cond_init(&(newRec->data->sleepers[i]), &cond_attr);
	}
	pthread_mutex_unlock(&(newRec->data->sleepers_lock));

	pthread_condattr_destroy(&cond_attr);
	pthread_mutexattr_destroy(&m_attr);

	// Create the wait/signal thread
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
	int fd = shm_open(SYSTEM_SHARED_MEM_NAME, O_RDWR, S_IRUSR | S_IWUSR);
	if (fd < 0) {
		printf("ERROR; Error getting shared segment %d \n", errno);
		exit(-1);
	}

	void* addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (addr == MAP_FAILED) {
		printf("ERROR; mmap failed %d\n", errno);
		exit(-1);	
	}

	SystemData shared_data = (SystemData)addr;

	System newRec = malloc(sizeof(struct System));
	newRec->data = shared_data;

	// Methods
	newRec->send = send;
	newRec->receive = receive;
	newRec->dropMsg = dropMsg;
	newRec->getMsgTag = getMsgTag;
	newRec->getNextTaskID = getNextTaskID;
	newRec->createMsgQ = createMsgQ;;
	newRec->destroy = destroy;
	newRec->message_immediate = message_immediate;	
	newRec->message_notify = message_notify;
	newRec->message_wait = message_wait;

	// Local addr to Q only
	for (int i = 0; i < MAX_TASK; i++) {
		newRec->TaskTable[i] = NULL;
	}

	return newRec;
	return NULL;
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
			if(!IsEmpty(this->TaskTable[i])) {
				pthread_cond_signal(&(this->data->sleepers[i]));
			}
		}

		usleep(1000); // 10ms
	}
	
	printf("System loop for wait and signal has shutdown\n");
}

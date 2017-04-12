/*
 * Copyright (C) 2016 The Ceezur Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <pthread.h>

#include "TaskSystem/UnboundedMsgQ.h"
#include "TaskSystem/Messages/Message.h"

#define SYSTEM_SHARED_MEM_NAME "/tmp/TS_SYSTEM"
#define MAX_TASK 100

typedef struct System *System;

struct System {

	// Message Q and ID
	Queue TaskTable[MAX_TASK];
	unsigned int nextTaskID;
	pthread_mutex_t TaskIDLock;

	// Signal and wait
	int shutdown_signal;
	pthread_t threadRef;

	pthread_mutex_t sleepers_lock;
	pthread_cond_t sleepers[MAX_TASK]; // This max limit the # of task, same limit than the task Q


	// "class" methods
	void 	(*send)(System this, Message data, int targetTaskID);
	Message (*receive)(System this, int targetTaskID);
	void 	(*dropMsg)(System this, int targetTaskID);
	int 	(*getMsgTag)(System this, int targetTaskID);
	unsigned int 	(*getNextTaskID)(System this);
	void 	(*createMsgQ)(System this, unsigned int taskID);
	void 	(*destroy)(System this);
};

static void *run(void* SystemRef);
static void loop_wait_signal(System this);

System System_create();

extern System Comm; // will be declared in the main routine

#endif /* SYSTEM_H_ */

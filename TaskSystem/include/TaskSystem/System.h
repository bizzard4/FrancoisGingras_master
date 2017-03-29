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

#include "TaskSystem/Messages/Message.h"

typedef struct System *System;

struct System {

	// Signal and wait
	int shutdown_signal;
	pthread_t threadRef;

	pthread_mutex_t sleepers_lock;
	pthread_cond_t sleepers[100]; // This max limit the # of task, same limit than the task Q


	// "class" methods
	void 	(*send)(Message data, int targetTaskID);
	Message (*receive)(int targetTaskID);
	void 	(*dropMsg)(int targetTaskID);
	int 	(*getMsgTag)(int targetTaskID);
	unsigned int 	(*getNextTaskID)();
	void 	(*createMsgQ)(unsigned int taskID);
	void 	(*destroy)(System this);
};

static void *run(void* SystemRef);
static void loop_wait_signal(System this);

System System_create();

extern System Comm; // will be declared in the main routine

#endif /* SYSTEM_H_ */

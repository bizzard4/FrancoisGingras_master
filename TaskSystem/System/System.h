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

#include "Message.h"

typedef struct System *System;

struct System {

	// "class" methods
	void 	(*send)(Message data, int targetTaskID);
	Message (*receive)(int targetTaskID);
	void 	(*dropMsg)(int targetTaskID);
	int 	(*getMsgTag)(int targetTaskID);
	unsigned int 	(*getNextTaskID)();
	void 	(*createMsgQ)(unsigned int taskID);
	void 	(*destroy)(System this);
};


System System_create();

extern System Comm; // will be declared in the main routine

#endif /* SYSTEM_H_ */

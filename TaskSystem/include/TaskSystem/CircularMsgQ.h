#ifndef CircularMsgQ_H
#define CircularMsgQ_H

#include "TaskSystem/Messages/Message.h"

#define Q_SIZE 1000000 // 1 MB

typedef Message ElementType;

struct Queue;
typedef struct Queue* Queue;

Queue CreateQueue(const char* unique_name);
Queue AcquireQueue(const char* unique_name);
void DeleteQueue(const char* unique_name);

ElementType Peek(Queue Q);
int IsEmpty(Queue Q);

int Enqueue(Queue Q, ElementType item);
ElementType Dequeue(Queue Q);

#endif /* CircularMsgQ_H */
#ifndef UnboundedMsgQ_H
#define UnboundedMsgQ_H

#include "Message.h"

typedef Message ElementType;

struct Node;
typedef struct Node *Node;
typedef Node Position;

struct Queue;
typedef struct Queue* Queue;

Queue CreateQueue();
void DeleteQueue(Queue Q);

ElementType Peek(Queue Q);
int IsEmpty(Queue Q);

int Enqueue(Queue Q, ElementType item);
ElementType Dequeue(Queue Q);


// int IsEmpty2( List L );
// int IsLast( Position P, List L );
// Position Find( ElementType X, List L );
// void Delete( ElementType X, List L );
// Position FindPrevious( ElementType X, List L );
// void Insert( ElementType X, List L, Position P );
// void DeleteList( List L );
// Position Header( List L );
// Position First( List L );
// Position Advance( Position P );
// ElementType Retrieve( Position P );

#endif /* UnboundedMsgQ_H */

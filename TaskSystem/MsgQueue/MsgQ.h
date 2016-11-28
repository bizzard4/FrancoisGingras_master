
#ifndef MsgQ_H_
#define MsgQ_H_

#include "Message.h"

typedef Message ElementType;

struct MsgQRecord;
typedef struct MsgQRecord *MsgQ;

int IsEmpty( MsgQ Q );
int IsFull( MsgQ Q );
MsgQ CreateMsgQ( int MaxElements );
void DisposeQueue( MsgQ Q );
void MakeEmpty( MsgQ Q );
void Enqueue( ElementType X, MsgQ Q );
ElementType Front( MsgQ Q );
void Dequeue( MsgQ Q );
ElementType FrontAndDequeue( MsgQ Q );


#endif /* MsgQ_H_ */

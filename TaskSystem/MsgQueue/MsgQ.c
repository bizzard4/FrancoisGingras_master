#include "MsgQ.h"

#include "fatal.h"
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MinMsgQSize ( 10 )


/*
 * To ensure thread safety, each instance of a message Q has
 * its own internal mutex. This is lock and unlocked by any
 * method that needs to access mutable data
 */

struct MsgQRecord {

	pthread_mutex_t QLock; // the Queue mutex

	int Capacity;
	int Front;
	int Rear;
	int Size;
	ElementType *Array;
};


int IsEmpty( MsgQ Q ) {

	pthread_mutex_lock (&(Q->QLock));
	int result = Q->Size == 0;
	pthread_mutex_unlock (&(Q->QLock));

	return result;
}


// version to be used if called from another
// function that already holds the lock
static int IsEmpty_locked( MsgQ Q ) {
	return Q->Size == 0;
}



int IsFull( MsgQ Q ) {

	pthread_mutex_lock (&(Q->QLock));
	int result = Q->Size == Q->Capacity;
	pthread_mutex_unlock (&(Q->QLock));

	return result;
}


// version to be used if called from another
// function that already holds the lock
static int IsFull_locked( MsgQ Q ) {
	return Q->Size == Q->Capacity;
}


MsgQ CreateMsgQ( int MaxElements ) {

	MsgQ Q;

	if( MaxElements < MinMsgQSize )
		Error( "MsgQ size is too small" );

	Q = malloc( sizeof( struct MsgQRecord ) );
	if( Q == NULL )
		FatalError( "Out of space!!!" );

	Q->Array = malloc( sizeof( ElementType ) * MaxElements );
	if( Q->Array == NULL )
		FatalError( "Out of space!!!" );

	Q->Capacity = MaxElements;
	MakeEmpty( Q );

	int result = pthread_mutex_init(&(Q->QLock), NULL); // init mutex

	return Q;
}



void MakeEmpty( MsgQ Q ){
	Q->Size = 0;
	Q->Front = 1;
	Q->Rear = 0;
}


void DisposeQueue( MsgQ Q ){
	if( Q != NULL ){
		pthread_mutex_destroy(&(Q->QLock));
		free( Q->Array );
		free( Q );
	}
}



static int Succ( int Value, MsgQ Q ) {

	pthread_mutex_lock (&(Q->QLock));

	if( ++Value == Q->Capacity )
		Value = 0;

	pthread_mutex_unlock (&(Q->QLock));

	return Value;
}


// version to be used if called from another
// function that already holds the lock
static int Succ_locked( int Value, MsgQ Q ) {
	if( ++Value == Q->Capacity )
		Value = 0;
	return Value;
}


void Enqueue( ElementType X, MsgQ Q ) {

	pthread_mutex_lock (&(Q->QLock));

	if( IsFull_locked( Q ) )
		Error( "Full MsgQ" ); // Resize
	else {
		Q->Size++;
		Q->Rear = Succ_locked( Q->Rear, Q );
		Q->Array[ Q->Rear ] = X;
	}

	pthread_mutex_unlock (&(Q->QLock));

}




ElementType Front( MsgQ Q ) {

	pthread_mutex_lock (&(Q->QLock));

	if( !IsEmpty_locked( Q ) ){
		pthread_mutex_unlock (&(Q->QLock));
		return Q->Array[ Q->Front ];
	}

	Error( "Empty MsgQ" );

	pthread_mutex_unlock (&(Q->QLock));

	return 0;  /* Return value used to avoid warning */
}


void Dequeue( MsgQ Q ) {

	pthread_mutex_lock (&(Q->QLock));

	if( IsEmpty_locked( Q ) )
		Error( "Empty MsgQ" );

	else {
		Q->Size--;
		Q->Front = Succ_locked( Q->Front, Q );
	}

	pthread_mutex_unlock (&(Q->QLock));
}


ElementType FrontAndDequeue( MsgQ Q ) {

	pthread_mutex_lock (&(Q->QLock));

	ElementType X = 0;

	if( IsEmpty_locked( Q ) )
		Error( "Empty MsgQ" );

	else {
		Q->Size--;
		X = Q->Array[ Q->Front ];
		Q->Front = Succ_locked( Q->Front, Q );
	}

	pthread_mutex_unlock (&(Q->QLock));

	return X;
}


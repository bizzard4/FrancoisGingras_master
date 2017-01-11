#include "UnboundedMsgQ.h"
#include <stdlib.h>
#include "fatal.h"

/* Place in the interface file */
struct Node
{
	ElementType Element;
	Position    Next;
};

List MakeEmpty2( List L )
{
	if( L != NULL )
		DeleteList( L );
	L = malloc( sizeof( struct Node ) );
	if( L == NULL )
		FatalError( "Out of memory!" );
	L->Next = NULL;
	return L;
}

int IsEmpty2( List L )
{
	return L->Next == NULL;
}

int IsLast( Position P, List L )
{
	return P->Next == NULL;
}

Position Find( ElementType X, List L )
{
	Position P;

/* 1*/      P = L->Next;
/* 2*/      while( P != NULL && P->Element != X )
/* 3*/          P = P->Next;

/* 4*/      return P;
}

void Delete( ElementType X, List L )
{
	Position P, TmpCell;

	P = FindPrevious( X, L );

	if( !IsLast( P, L ) )  /* Assumption of header use */
	{                      /* X is found; delete it */
		TmpCell = P->Next;
		P->Next = TmpCell->Next;  /* Bypass deleted cell */
		free( TmpCell );
	}
}

Position FindPrevious( ElementType X, List L )
{
	Position P;

/* 1*/      P = L;
/* 2*/      while( P->Next != NULL && P->Next->Element != X )
/* 3*/          P = P->Next;

/* 4*/      return P;
}

void Insert( ElementType X, List L, Position P )
{
	Position TmpCell;

/* 1*/      TmpCell = malloc( sizeof( struct Node ) );
/* 2*/      if( TmpCell == NULL )
/* 3*/          FatalError( "Out of space!!!" );

/* 4*/      TmpCell->Element = X;
/* 5*/      TmpCell->Next = P->Next;
/* 6*/      P->Next = TmpCell;
}

void DeleteList( List L )
{
	Position P, Tmp;

/* 1*/      P = L->Next;  /* Header assumed */
/* 2*/      L->Next = NULL;
/* 3*/      while( P != NULL )
	{
/* 4*/          Tmp = P->Next;
/* 5*/          free( P );
/* 6*/          P = Tmp;
	}
}
/* END */

Position Header( List L )
{
	return L;
}

Position First( List L )
{
	return L->Next;
}

Position Advance( Position P )
{
	return P->Next;
}

ElementType Retrieve( Position P )
{
	return P->Element;
}

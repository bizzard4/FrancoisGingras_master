
#ifndef ERRORS_FATAL_H_
#define ERRORS_FATAL_H_

#include "stdio.h"

#define Error( Str )        FatalError( Str )
#define FatalError( Str )   fprintf( stderr, "%s\n", Str ), exit( 1 )

#endif /* ERRORS_FATAL_H_ */

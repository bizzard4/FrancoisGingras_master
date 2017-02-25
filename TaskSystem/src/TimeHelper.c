#include "TaskSystem/TimeHelper.h"

#include <time.h>

struct timespec diff(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

struct timespec tsAdd(struct  timespec  time1, struct  timespec  time2) 
{
    struct  timespec  result ;

    result.tv_sec = time1.tv_sec + time2.tv_sec ;
    result.tv_nsec = time1.tv_nsec + time2.tv_nsec ;
    if (result.tv_nsec >= 1000000000L) {		
        result.tv_sec++ ;  result.tv_nsec = result.tv_nsec - 1000000000L ;
    }

    return (result) ;
}
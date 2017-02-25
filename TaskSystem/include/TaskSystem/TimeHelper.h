#ifndef TIMEHELPER_H_
#define TIMEHELPER_H_

#include <time.h>

struct timespec diff(struct timespec start, struct timespec end);

struct  timespec  tsAdd (struct  timespec  time1, struct  timespec  time2);

#endif /* TIMEHELPER_H_ */
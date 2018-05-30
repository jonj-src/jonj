/*****************************************************************************
 *                                                                           *
 * sys/time.h                                                                *
 *                                                                           *
 * Freely redistributable and modifiable.  Use at your own risk.             *
 *                                                                           *
 * Copyright 1994 The Downhill Project                                       *
 *
 * Modified by Shane Caraveo for JONJ
 *
 *****************************************************************************/
#ifndef TIME_H
#define TIME_H

/* Include stuff ************************************************************ */
#include <time.h>
#include "jonj.h"

/* Struct stuff ************************************************************* */
struct timezone {
	int tz_minuteswest;
	int tz_dsttime;
};


struct itimerval {
	struct timeval it_interval;	/* next value */
	struct timeval it_value;	/* current value */
};

#ifndef timespec
struct timespec
{
	time_t   tv_sec;   /* seconds */
	long     tv_nsec;  /* nanoseconds */
};
#endif

#define ITIMER_REAL    0		/*generates sigalrm */
#define ITIMER_VIRTUAL 1		/*generates sigvtalrm */
#define ITIMER_VIRT    1		/*generates sigvtalrm */
#define ITIMER_PROF    2		/*generates sigprof */

/* Prototype stuff ********************************************************** */
JONJAPI extern int gettimeofday(struct timeval *time_Info, struct timezone *timezone_Info);

/* setitimer operates at 100 millisecond resolution */
JONJAPI extern int setitimer(int which, const struct itimerval *value,
					 struct itimerval *ovalue);

JONJAPI int nanosleep( const struct timespec * rqtp, struct timespec * rmtp );

JONJAPI int usleep(unsigned int useconds);

#endif

/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     LiHejia
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/sys/time.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/


#ifndef _SYS_TIME_H
#define _SYS_TIME_H

struct timeval {
	unsigned long tv_sec;	/* Seconds */
	unsigned long tv_usec;	/* Microseconds */
};

struct timezone {
	int	tz_minuteswest;		/* minutes west of Greenwich */
	int	tz_dsttime;		/* type of dst correction */
};

struct itimerval {
    struct timeval it_interval;	    /* next value */
    struct timeval it_value;	    /* current value */
};

#define ITIMER_REAL	0
#define ITIMER_VIRTUAL	1
#define ITIMER_PROF	2

#ifdef __cplusplus
extern "C" {
#endif

int utimes(const char * filename, struct timeval * utimes);

int gettimeofday (struct timeval *tv, struct timezone *tz);
int settimeofday (const struct timeval *tv, const struct timezone *tz);

int getitimer (int witch, struct itimerval * value);
int setitimer (int witch, const struct itimerval * value, struct itimerval * ovalue);

#ifdef __cplusplus
}
#endif

#endif // _SYS_TIME_H

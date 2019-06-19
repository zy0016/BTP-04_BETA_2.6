/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _TIME_H_
#define	_TIME_H_

#ifndef NULL
#define NULL	(void *)0
#endif

#ifndef _STRUCT_TIMESPEC
#define _STRUCT_TIMESPEC

struct timespec {
	unsigned long	tv_sec;		/* seconds */
	unsigned long	tv_nsec;	/* nanoseconds */
};

#endif /* _STRUCT_TIMESPEC */

struct tm
{
	int tm_sec;	/* Seconds.	[0-60] (1 leap second) */
	int tm_min;	/* Minutes.	[0-59] */
	int tm_hour;	/* Hours.	[0-23] */
	int tm_mday;	/* Day.		[1-31] */
	int tm_mon;	/* Month.	[0-11] */
	int tm_year;	/* Year	- 1900.  */
	int tm_wday;	/* Day of week.	[0-6] */
	int tm_yday;	/* Days in year.[0-365]	*/
	int tm_isdst;	/* DST.		[-1/0/1]*/
	long tm_gmtoff;		/* Seconds east of UTC.  */
	const char *tm_zone;	/* Timezone abbreviation.  */
};

#ifndef __CLOCK_T
#define __CLOCK_T
typedef long int clock_t;
#endif

#ifndef __SIZE_T
#define __SIZE_T
#if defined(__i386) || !defined (__GNUC__)
typedef unsigned int	size_t;
#else
typedef unsigned long	size_t;
#endif
#endif

#ifndef __TIME_T
#define __TIME_T
typedef unsigned long	time_t;
#endif

#define CLOCKS_PER_SEC	GetTicksPerSecond()
#define CLK_TCK		CLOCKS_PER_SEC

#define YEAR_BASE	1900

extern char **gettzname();

#define tzname	gettzname()

extern char * __tzname[2];

#ifdef __cplusplus
extern "C" {
#endif

char * asctime (const struct tm * timeptr);
char * ctime (const unsigned long * timep);
long difftime (unsigned long time1, unsigned long time2);
struct tm * gmtime (const unsigned long * timep);
struct tm * localtime (const unsigned long * timep);
unsigned long mktime(struct tm * timeptr);
unsigned long strftime (char *s, unsigned long maxsize, const char *format, const struct tm *tm);
unsigned long time (unsigned long * t);
void tzset (void);

int GetTicksPerSecond(void);
unsigned long GetSysClock (void);

#ifdef __cplusplus
}
#endif

#endif /* _TIME_H_ */


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
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/sys/times.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/05/13 07:59:55 $
 *
\**************************************************************************/

#ifndef	_SYS_TIMES_H
#define	_SYS_TIMES_H

/* Structure describing CPU time used by a process and its children.  */
struct tms {
	unsigned long tms_utime;	/* User CPU time.  */
	unsigned long tms_stime;	/* System CPU time.  */
	unsigned long tms_cutime;	/* User CPU time of dead children.  */
	unsigned long tms_cstime;	/* System CPU time of dead children.  */
};

#ifndef __CLOCK_T
#define __CLOCK_T
typedef long int clock_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

clock_t times(struct tms *buf);

#ifdef __cplusplus
}
#endif

#endif /* sys/times.h	*/

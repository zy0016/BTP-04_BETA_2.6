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
 * $Source: /cvs/hopencvs/src/include/hopen/sysinfo.h,v $
 * $Name:  $
 *
 * $Revision: 1.9 $     $Date: 2004/12/13 06:20:19 $
 * 
\**************************************************************************/

#ifndef _HOPEN_SYSINFO_H
#define _HOPEN_SYSINFO_H

/*
 * Existing tasks' info.
 */
struct task_info {
	unsigned short	taskid;			/* Tatk ID */
	unsigned short	procid;			/* Process ID */
	unsigned short	parent;			/* Parent task ID */
	unsigned char	state;			/* Task state */
	unsigned char	baseprio;		/* basal prior  */
	unsigned short	flags;			/* flag */
	signed	int		currprio;		/* curren prior */
	unsigned long	cputimes;		/* cpu time */
	void *			waitAddr;		/* waiting queue for this process */
	void *			private_data;	/* windows thread handle in win32 platform */
};

struct tasks_info {
	int count;						/* count of tasks info */
	struct task_info * tasksinfo;	/* array of tasks info */
};

/*
 * Existing processes' info.
 */
struct proc_info {
	unsigned short	procid;			/* Process ID */
	unsigned short	parent;			/* Process Parent ID */
	unsigned char	state;			/* main thread state */
	char *			filename;		/* File name of this process */
};

struct procs_info {
	int count;						/* count of process info */
	struct proc_info * procsinfo;	/* array of process info */
	char * filename_buf;			/* buffer of process name */
};

/*
 * System info.
 */

#define	SI_TICKS_PERSECOND	0
#define SI_SYSTIME			1
#define SI_REALTIME			2
#define SI_BASETIME			3

#define SI_LIBHANDLE		4
#define SI_LIBINFO			5
#define SI_UNLOADLIB		9

#define SI_TASKCOUNT		6
#define SI_TASKSINFO		7

#define SI_PROCCOUNT		10
#define SI_PROCSINFO		11

#define SI_PAGESIZE			12

/*
 * Process info.
 */

#define	PI_GETENVEDDR	1

#define	PI_SETENVEDDR	1
#define PI_LINKLIB	2

/*
 * Task info
 */

#define GET_TASKID		0
#define GET_ERRNO		1
#define GET_PRIORITY		2
#define GET_TASKSTATE		3
#define GET_NEXTTID		4
#define GET_NEXTCHILD   	5
#define GET_NEXTTRACE		6
#define GET_PROCID		7

#define GET_PROCESSTIMES	8

#define GET_REGISTER            20  /* get task registers */

#define SET_ERRNO		1
#define SET_PRIORITY		2
#define SET_PARENT		9

#define SET_REGISTER            20  /* Set register */

#ifdef __cplusplus
extern "C" {
#endif

int SetSysInfo( int info, unsigned long value );
unsigned long GetSysInfo( int info, unsigned long value );
int GetTaskInfo (int mode, int tid, void * pdata);

#ifdef __cplusplus
}
#endif

#endif //_HOPEN_SYSINFO_H

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
 * $Source: /cvs/hopencvs/src/include/sys/task.h,v $
 * $Name:  $
 *
 * $Revision: 1.4 $     $Date: 2004/12/13 06:24:12 $
 * 
\**************************************************************************/

#ifndef _SYS__TASK_H
#define _SYS__TASK_H

struct taskcreate {
	int  (* entry)();	/* New task entry*/
	void *  param;		/* Task parameter */
	short	prio;		/* Task PRI */
	short	flags;		/* Type of task */
	void *	stkTop;		/* Top of user stack */
};

/* Value of task flags */
#define TF_REALTIME	0x0200		/* real-time task */

#ifdef __cplusplus
extern "C" {
#endif

int CreateTaskEx (struct taskcreate * pCreateData);
int EndTask (int exitcode, int mode);
int GetTaskID (void);

int  DisableSched (void);
int  EnableSched (int mode);

int GetTaskInfo (int mode, int tid, void * pdata);
int SetTaskInfo (int mode, int tid, void * pdata);

struct tms;
int GetProcessTimes (int tid, struct tms *ptms);

int Sleep ( int miniseconds );
int SleepOn (int * addr, int value);
int Wakeup(int * addr, int number);

#ifdef __cplusplus
}
#endif

#endif  //_SYS__TASK_H

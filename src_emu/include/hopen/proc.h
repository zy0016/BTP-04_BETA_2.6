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
 * $Source: /cvs/hopencvs/src/include/hopen/proc.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2004/12/13 06:20:09 $
 * 
\**************************************************************************/

#ifndef _HOPEN_PROCESS_H_
#define	_HOPEN_PROCESS_H_

#include <hopen/mutex.h>
#include <hopen/file.h>

struct _sigact 
{
	void (*sa_handler)(int);
	unsigned long sa_mask;
//	int	sa_flags;	
};

struct proc {
        short   procid;                 /* Process ID is the task ID of control task */
        short   unused;
	int     exec_done;
	struct mutex	mutex;		
	char  * filename;		/* File name of this process */
	unsigned long envaddr;		/* Address of env block */ 
	unsigned long envsize;		/* Size of env block */ 
	void  (*sigHandle)();   	/* entry for dealing with signal */
	struct mm_struct * mm;		/* Memory map structure */
	struct proc_file file;		/* handle table */
	struct _sigact	sigact[32];	/* signal action table */
};

extern void _KERN_FreePCB (struct proc * pcb);

#endif

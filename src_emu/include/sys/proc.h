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
 * $Source: /cvs/hopencvs/src/include/sys/proc.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/05/28 03:47:11 $
 * 
\**************************************************************************/

#ifndef _SYS_PROC_H
#define _SYS_PROC_H

#include <sys/types.h>

/* Proc create mode bit values */
#define PROC_TRACE	0x0100

#ifdef __cplusplus
extern "C" {
#endif

int createproc (int mode, void (*entry)(void*), void * param);
int GetProcInfo (int mode, pid_t pid, void * pdata);
int SetProcInfo (int mode, pid_t pid, void * pdata);
int loadimage (int mode, char * filename, void * envaddr, long envsize);

#ifdef __cplusplus
}
#endif

#endif  //_SYS_PROC_H

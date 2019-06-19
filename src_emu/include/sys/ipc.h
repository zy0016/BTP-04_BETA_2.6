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
 * $Source: /cvs/hopencvs/src/include/sys/ipc.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2004/12/10 08:32:47 $
 * 
\**************************************************************************/

#ifndef _SYS_IPC_H
#define _SYS_IPC_H

#define	IPC_EVENT	0x01
#define	IPC_SEM		0x02
#define IPC_QUEUE	0x04
#define	MODE_MASK	0x07

/* manual reset event */
#define MANUALRESET	0x0001

#ifdef __cplusplus
extern "C" {
#endif

int CreateEvent (int key, int mode);
int OpenEvent (int key);
int DestroyEvent (int handle);
int WaitEvent (int handle, long timeout);
int SetEvent (int handle, int event);

int CreateSemaphore (int key, int value);
int OpenSemaphore (int key);
int DestroySemaphore (int handle);
int WaitSemaphore (int handle, long timeout);
int ReleaseSemaphore (int handle, int count);

int CreateQueue (int key, int itemsize, int qsize);
int OpenQueue (int key);
int DestroyQueue (int handle);
int RecvQueue (int handle, const void * buf, long timeout);
int SendQueue (int handle, const void * buf, long timeout);

#ifdef __cplusplus
}
#endif

#endif

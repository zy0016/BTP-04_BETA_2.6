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

#ifndef _HMAP_THREAD_H_
#define _HMAP_THREAD_H_

int ThreadCreate (int attrib, void (*entry)(void * param), int stack_size, void * param);
#define THREAD_WAIT	1

void ThreadEnd (void);
int ThreadKill (int thread_id, int wait);
int ThreadWait (int thread_id);

#endif  //_HMAP_THREAD_H_



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

#ifndef _HMAP_MONITOR_H_
#define _HMAP_MONITOR_H_

#ifdef LINUX
#include <pthread.h>

struct monitor {
    pthread_mutex_t mutex;
};

struct condvar {
    pthread_cond_t  cond;
};

#else
struct monitor {
    long     data[2];
};

struct condvar {
    long     data[2];
};
#endif

int MonitorCreate (struct monitor * monitor);
void MonitorEnter (struct monitor * monitor);
void MonitorLeave (struct monitor * monitor);
int CondVarCreate (struct condvar * condvar);
void CondWait (struct condvar * condvar, struct monitor * monitor);
void CondWakeup (struct condvar * condvar);

#endif



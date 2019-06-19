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
 * $Source: /cvs/hopencvs/src/include/hopen/monitor.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/05/22 06:25:17 $
 * 
\**************************************************************************/

#ifndef _HOPEN_MONITOR_H
#define _HOPEN_MONITOR_H

#include <hopen/task.h>
#include <hopen/mutex.h>

struct monitor {
	struct mutex m_mutex;
};

#define init_monitor(pm)	init_mutex(&(pm)->m_mutex)

#define enter_monitor(pm)	\
	do {			\
		wait_mutex(&(pm)->m_mutex);	\
		current->monitor = pm;		\
	} while (0)

#define leave_monitor(pm)	\
	do {			\
		current->monitor = NULL;	\
		release_mutex(&(pm)->m_mutex);	\
	} while (0)

#endif	// _HOPEN_MONITOR_H

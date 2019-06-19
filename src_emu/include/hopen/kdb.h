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
 * $Source: /cvs/hopencvs/src/include/hopen/kdb.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:58:59 $
 * 
\**************************************************************************/

#ifndef _HOPEN_KDB_H
#define _HOPEN_KDB_H

#include <hopen/task.h>

int kdb_get_task_list (unsigned short * plist, int listlen);
struct task * kdb_get_task_stat (int taskid);
int kdb_stop (int taskid);
int kdb_continue (int taskid);
void kdb_trap (int monitor_task);

#endif	//_HOPEN_KDB_H


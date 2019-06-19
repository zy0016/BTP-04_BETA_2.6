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
 * $Source: /cvs/hopencvs/src/include/hopen/usermem.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _HOPEN_USERMEM_H
#define	_HOPEN_USERMEM_H

int _KERN_AddUserMemory (void * addr, long size);
int _KERN_AllocGlobalMemory (void ** pret, unsigned long size);
int _KERN_FreeGlobalMemory (void * addr, unsigned long size);
/* Free all global memory allocated by given process, except saved area.
 * saved area normally used as arg and env */
void _KERN_FreeAllGlobalMemory (int procid, void * saveaddr, long savesize);
//long _KERN_AllocFixedSegment (void * addr, long size);
//int  _KERN_FreeSegment (void * addr);
//void _KERN_GetSegment (void * addr);
//int  _KERN_AppendSegment (void * addr, long newSize);

#endif


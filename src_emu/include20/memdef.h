/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	Window
 *
 * Purpose :    Definitions for memory management.
 *
 * Author  :	Shuhua Fang
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Revision: 1.1 $     $Date: 2003/03/06 06:48:50 $
 *
 * $History::                                                       $
 * 
\**************************************************************************/

#ifndef __MEMDEF_H
#define __MEMDEF_H

#include "stdlib.h"

typedef void * HGLOBAL;
typedef	void * HLOCAL;

#define GlobalAlloc(flag, size)     malloc(size)
#define GlobalFree(handle)          free(handle)
#define GlobalLock(handle)          (handle)
#define GlobalUnlock(handle)

#define LocalAlloc(flag, size)      malloc(size)
#define LocalFree(handle)           free(handle)
#define LocalRealloc(p, newsize)    realloc(p, newsize)
#define LocalLock(handle)           (handle)
#define LocalUnlock(handle)

#endif  // __MEMDEF_H

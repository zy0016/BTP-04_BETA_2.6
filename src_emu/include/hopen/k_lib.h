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
 * $Source: /cvs/hopencvs/src/include/hopen/k_lib.h,v $
 * $Name:  $
 *
 * $Revision: 1.4 $     $Date: 2004/12/13 06:19:24 $
 * 
\**************************************************************************/

#ifndef _HOPEN_LIB_H_
#define	_HOPEN_LIB_H_

#ifdef __MMU_
#include <hopen/mm.h>
#endif
#include <sys/procenv.h>
#include <sys/sharelib.h>

struct libtable{
	unsigned long libinfo_addr;
	struct libinfo libinfo;
#ifdef __MMU__
	struct vm_area_struct* vma[4];
#else
	int count;
#endif
};

extern struct libtable loadlib_table[];

/* functions in kernel shared lib */
extern int findlibrary(char* name, unsigned long version);
extern int getlibraryinfo (int handle, unsigned long addr, int size);
extern int linklibrary (int handle, int mode);
void* getproaddress (int handle, const char* name);
#endif

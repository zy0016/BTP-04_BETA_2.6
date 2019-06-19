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
 * $Source: /cvs/hopencvs/src/include/hopen/vmalloc.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2003/07/16 06:02:54 $
 *
\**************************************************************************/

#ifndef __HOPEN_VMALLOC_H
#define __HOPEN_VMALLOC_H

struct vm_struct {
	unsigned long flags;
	void * addr;
	unsigned long size;
	struct vm_struct * next;
};

struct vm_struct * get_vm_area(unsigned long size);
void free_vm_area(struct vm_struct *vm);

#define VMALLOC_VMADDR(a)   (unsigned long)(a)

void * vmalloc(unsigned long size);
void vfree(void * addr);
int  vblksize(void * addr);

#endif

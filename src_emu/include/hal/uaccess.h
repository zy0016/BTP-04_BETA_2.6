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
 * $Source: /cvs/hopencvs/src/include/hal-armv/uaccess.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2003/09/17 00:50:42 $
 * 
\**************************************************************************/

#ifndef _ARM_UACCESS_H
#define _ARM_UACCESS_H

extern int copy_from_user(void *to, const void * from, int n);
extern int copy_to_user(void * to, const void *from, int n);
extern int clear_user(void * addr, int len);
extern int strncpy_from_user(char *dst, const char * src, int count);
extern int strnlen_user(const char * str, int n);

static __inline__ int put_user(int val, void *dst)
{
	int tmp = val;
	return copy_to_user(dst, &tmp, sizeof(int));
}

#define get_user(val, src)	copy_from_user(&(val), (src), sizeof(int))

#endif /* _ARM_UACCESS_H */

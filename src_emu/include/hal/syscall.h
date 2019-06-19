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
 * $Source: /cvs/hopencvs/src/include/hal-armv/syscall.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/05/30 06:06:10 $
 * 
\**************************************************************************/

#ifndef _HAL_SYSCALL_H
#define _HAL_SYSCALL_H

#define hopen_syscall_0(cmd, retval) \
	do { \
		register long __res __asm ("%r0") ; \
                __asm volatile ("swi	%1" : "=r" (__res) :"i"(cmd) : "%r0"); \
		retval = __res; \
	} while (0)

#define hopen_syscall_1(cmd, retval, arg1) \
	do { \
		register long __res __asm ("%r0") ; \
                register long __a __asm ("%r0") = (long)(arg1); \
                __asm volatile ("swi	%1" : "=r" (__res) : "i"(cmd), "0" (__a) : "%r0"); \
		retval = __res; \
	} while (0)


#define hopen_syscall_2(cmd, retval, arg1, arg2) \
	do { \
		register long __res __asm ("%r0") ; \
		register long __a __asm ("%r0") = (long)(arg1); \
		register long __b __asm ("%r1") = (long)(arg2); \
                __asm volatile ("swi	%1" : "=r" (__res) : "i" (cmd), "0" (__a), "r" (__b) : "%r0"); \
		retval = __res; \
	} while (0)


#define hopen_syscall_3(cmd, retval, arg1, arg2, arg3) \
	do { \
		register long __res __asm ("%r0") ; \
		register long __a __asm ("%r0") = (long)(arg1); \
		register long __b __asm ("%r1") = (long)(arg2); \
		register long __c __asm ("%r2") = (long)(arg3); \
                __asm volatile ("swi	%1" : "=r" (__res) : "i" (cmd), "0" (__a), "r" (__b), "r" (__c) : "%r0"); \
		retval = __res; \
	} while (0)

#define hopen_syscall_4(cmd, retval, arg1, arg2, arg3, arg4) \
	do { \
		register long __res __asm ("%r0") ; \
		register long __a __asm ("%r0") = (long)(arg1); \
		register long __b __asm ("%r1") = (long)(arg2); \
		register long __c __asm ("%r2") = (long)(arg3); \
		register long __d __asm ("%r3") = (long)(arg4); \
		__asm volatile ("swi	%1" : "=r" (__res) : "i" (cmd), "0" (__a), "r" (__b), "r" (__c), "r" (__d) : "%r0"); \
		retval = __res; \
	} while (0)

#endif

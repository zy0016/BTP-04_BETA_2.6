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
 * Author  :     
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/sys/ptrace.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2003/06/03 02:47:07 $
 * 
\**************************************************************************/

#ifndef _SYS_PTRACE_H
#define _SYS_PTRACE_H

#define PTRACE_TRACEME		0
#define PTRACE_PEEKTEXT		1
#define PTRACE_PEEKDATA		2
#define PTRACE_PEEKUSER		3
#define PTRACE_POKETEXT		4
#define PTRACE_POKEDATA		5
#define PTRACE_POKEUSER		6
#define PTRACE_CONT		7
#define PTRACE_KILL		8
#define PTRACE_SINGLESTEP	9

#define PTRACE_ATTACH		10
#define PTRACE_DETACH		11

#define	PTRACE_GETREGS		12
#define PTRACE_SETREGS		13
#define PTRACE_SYSCALL		14

#define	PTRACE_GETFPREGS	15
#define	PTRACE_SETFPREGS	16


#ifdef __cplusplus
extern "C" {
#endif

int ptrace(long request, long pid, long addr, long data);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_PTRACE_H */

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
 * $Source: /cvs/hopencvs/src/include/hopen/ptrace.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/06/03 02:29:48 $
 *
\**************************************************************************/

#ifndef _HOPEN_PTRACE_H_
#define _HOPEN_PTRACE_H_

extern int _hal_read_long(struct task * tsk, unsigned long addr, unsigned long * result);
extern int _hal_write_long(struct task * tsk, unsigned long addr, unsigned long data);
extern int _hal_get_reg(struct task * tsk, unsigned long addr, unsigned long *result);
extern int _hal_put_reg(struct task * tsk, unsigned long addr, unsigned long data);

extern int _hal_clear_trap(struct task *task);
extern int _hal_set_trap(struct task *task);
extern int _hal_get_all_regs(struct task *task, long data);
extern int _hal_set_all_regs(struct task *task, long data);

#endif


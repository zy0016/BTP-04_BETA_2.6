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
 * $Source: /cvs/hopencvs/src/include/hal-armv/bitops.h,v $
 * $Name:  $
 *
 * $Revision: 1.4 $     $Date: 2004/02/23 02:12:13 $
 *
\**************************************************************************/

#ifndef _ARM_BITOPS_H
#define _ARM_BITOPS_H

#define ADDR (*(volatile long *) addr)

/* Atomic functions */
extern void set_bit(int nr, volatile void * addr);
extern void clear_bit(int nr, volatile void * addr);
extern void change_bit(int nr, volatile void * addr);
extern int test_bit(int nr, volatile void * addr);
extern int test_and_set_bit(int nr, volatile void * addr);
extern int test_and_clear_bit(int nr, volatile void * addr);
extern int test_and_change_bit(int nr, volatile void * addr);

extern int find_first_set_bit(unsigned mask);
extern int find_first_zero_bit(unsigned mask);
extern int find_next_zero_bit(void * addr, int size, int offset);

/*
 * None atomic functions.
 */

extern void __set_bit(int nr, volatile void * addr);
extern void __clear_bit(int nr, volatile void * addr);
extern void __change_bit(int nr, volatile void * addr);

#endif /* _ARM_BITOPS_H */

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
 * $Source: /cvs/hopencvs/src/include/hal-armv/atomic.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/05/30 06:06:08 $
 * 
\**************************************************************************/

#ifndef _ARM_ATOMIC_H_
#define	_ARM_ATOMIC_H_

typedef	long atomic_t;

extern void atomic_add (int i, volatile atomic_t *v);
extern void atomic_sub(int i, volatile atomic_t *v);
extern void atomic_inc(volatile atomic_t *v);
extern void atomic_dec(volatile atomic_t *v);
extern void atomic_set_mask(unsigned long mask, unsigned long *addr);
extern void atomic_clear_mask(unsigned long mask,  unsigned long *addr);
extern int  atomic_dec_and_test(volatile atomic_t *v);
extern int  atomic_check_and_set (volatile atomic_t *v);

#define atomic_set(v,val)   *(v) = val
#define atomic_clear(v)	    *(v) = 0
#define atomic_read(v)	    *(v)

extern unsigned __xchg_1(unsigned x, void * ptr);
extern unsigned __xchg_2(unsigned x, void * ptr);
extern unsigned __xchg_4(unsigned x, void * ptr);

#define xchg(ptr,x) (__typeof__(*(ptr)))((sizeof(*(ptr)) == 4) ? __xchg_4((unsigned)(x),(ptr)) : \
			(sizeof(*(ptr)) == 2) ? __xchg_2((unsigned)(x),(ptr)) : __xchg_1((unsigned)(x),(ptr)))

#endif /* _ARM_ATOMIC_H_ */

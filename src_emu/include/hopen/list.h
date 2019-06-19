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
 * $Source: /cvs/hopencvs/src/include/hopen/list.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2003/06/26 03:36:14 $
 * 
\**************************************************************************/

#ifndef _HOPEN_LIST_H
#define _HOPEN_LIST_H

#ifdef __KERNEL__

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct list_head {
	struct list_head *next, *prev;
};

#define LIST_HEAD(name)			struct list_head name = { &name, &name }

#define INIT_LIST_HEAD(ptr)		(ptr)->next = (ptr)->prev = (ptr)

/*
 * Insert a new entry between two known consecutive entries. 
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#define __list_add(new, prev, next)	\
	do { \
		(new)->next = (next); \
		(new)->prev = (prev); \
		(next)->prev = (new); \
		(prev)->next = (new); \
	} while (0)

/*
 * Insert a new entry after the specified head..
 */
#define list_add(new, head)	\
	do { \
		(new)->next = (head)->next; \
		(new)->prev = (head); \
		(head)->next->prev = (new); \
		(head)->next = (new); \
	} while (0)

/*
 * Insert a new entry at the list tail.
 */
#define list_add_tail(new, head)	\
	do { \
		(new)->next = (head); \
		(new)->prev = (head)->prev; \
		(head)->prev->next = (new); \
		(head)->prev = (new); \
	} while (0)

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
#define __list_del(prevn, nextn)	\
	do { \
		(nextn)->prev = prevn; \
		(prevn)->next = nextn; \
	} while (0)

#define list_del(entry) \
	do { \
		(entry)->next->prev = (entry)->prev; \
		(entry)->prev->next = (entry)->next; \
		(entry)->next = (entry)->prev = entry; \
	} while (0)

#define list_empty(head)	((head)->next == (head))

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)

#endif /* __KERNEL__ */

#endif	// _HOPEN_LIST_H

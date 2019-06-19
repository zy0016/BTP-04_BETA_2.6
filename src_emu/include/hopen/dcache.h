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
 * $Source: /cvs/hopencvs/src/include/hopen/dcache.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/12/13 06:17:48 $
 * 
\**************************************************************************/

#ifndef _HOPEN_DCACHE_H
#define _HOPEN_DCACHE_H

#ifdef __KERNEL__

#include	<hopen/inode.h>
#include	<hopen/list.h>
#include	<hal/atomic.h>

struct iattr;
struct inode;

#define IS_ROOT(x) ((x) == (x)->d_parent)

struct qstr {
	unsigned short len;
	unsigned short unused;
	unsigned long  hash;
	const char * name;
};

struct dentry_operations {
	int  (*d_hash) (struct dentry *, struct qstr *);
	int  (*d_compare) (struct dentry *, struct qstr *, struct qstr *);
};

#define DCACHE_NFSFS_RENAMED  0x0002    /* this dentry has been "silly
					 * renamed" and has to be
					 * deleted on the last dput()
					 */

struct dentry {
	struct list_head d_hash;	/* name hash table */
	struct dentry *	 d_parent;	/* parent entry */
	struct qstr	 d_name;	/* name */
	struct list_head d_lru;		/* LRU list */
	atomic_t	 d_count;	/* visiting count  */
	struct inode *	 d_inode;	/* pointer to inode */
	struct dentry_operations * d_op; /* especial operation */
};

#define	DENTRY_SIZE	64
#define	MAX_SHORTNAME	(63 - sizeof(struct dentry))

extern struct dentry * d_alloc(struct dentry * parent, const struct qstr *name);

extern struct inode * dentry_iget(struct dentry * entry);
extern void dentry_iput (struct dentry * dentry);

extern void d_instantiate(struct dentry *entry, struct inode * inode);
extern int  d_invalidate(struct dentry * dentry);
extern void d_rehash(struct dentry * dentry);
extern void d_move(struct dentry * old, struct dentry * new);
extern int  d_unhashed(struct dentry * dentry);

extern struct dentry * dget (struct dentry *dentry);
extern void dput(struct dentry * dentry);
extern void d_drop(struct dentry * dentry);
extern void d_delete(struct dentry * dentry);

extern void d_add (struct dentry * entry, struct inode * inode);

extern struct dentry * d_lookup(struct dentry * dir, struct qstr * name);
extern char * d_path(struct dentry * entry, char * buf, int buflen);

/* Name hashing routines. Initial hash value */
#define init_name_hash()		0

/*
 * partial hash update function. Assume roughly 4 bits per character.
 */
extern unsigned long partial_name_hash(unsigned long c, unsigned long prevhash);
/*
 * Finally: cut down the number of bits to a int value (and try to avoid losing bits).
 */
extern unsigned long end_name_hash(unsigned long hash);
/*
 * Compute the hash for a name string.
 */
extern unsigned int full_name_hash(const unsigned char * name, unsigned int len);

extern int is_subdir(struct dentry *, struct dentry *);
extern void shrink_dcache_parent(struct dentry *);
extern int dcache_flush_inode (void);

#endif /* __KERNEL__ */

#endif	/* __LINUX_DCACHE_H */

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
 * $Source: /cvs/hopencvs/src/include/hopen/inode.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2004/12/13 06:19:08 $
 * 
\**************************************************************************/

#ifndef _HOPEN_INODE_H
#define _HOPEN_INODE_H

#ifdef __KERNEL__

#include <hal/atomic.h>
#include <hopen/mutex.h>

struct list_head;
struct super_block;
struct dentry;
struct file;
struct page;

/*
 * Attribute flags.  These should be or-ed together to figure out what
 * has been changed!
 */

#define ATTR_MODE		1
#define ATTR_UID		2
#define ATTR_GID		4
#define ATTR_SIZE		8
#define ATTR_ATIME		16
#define ATTR_MTIME		32
#define ATTR_CTIME		64
#define ATTR_ATIME_SET		128
#define ATTR_MTIME_SET		256
#define ATTR_FORCE		512	/* Not a change, but a change it */
#define ATTR_ATTR_FLAG		1024

/*
 * This is the Inode Attributes structure, used for notify_change().  It
 * uses the above definitions as flags, to know which values have changed.
 * Also, in this manner, a Filesystem can look at only the values it cares
 * about.  Basically, these are the attributes that the VFS layer can
 * request to change from the FS layer.
 *
 * Derek Atkins <warlord@MIT.EDU> 94-10-20
 */
struct iattr {
	unsigned	ia_valid;
	umode_t		ia_mode;
	uid_t		ia_uid;
	gid_t		ia_gid;
	off_t		ia_size;
	time_t		ia_atime;
	time_t		ia_mtime;
	time_t		ia_ctime;
	unsigned	ia_attr_flags;
};

/*
 * This is the inode attributes flag definitions
 */
#define ATTR_FLAG_SYNCRONOUS	1 	/* Syncronous write */
#define ATTR_FLAG_NOATIME	2 	/* Don't update atime */
#define ATTR_FLAG_APPEND	4 	/* Append-only file */
#define ATTR_FLAG_IMMUTABLE	8 	/* Immutable file */
#define ATTR_FLAG_NODIRATIME	16 	/* Don't update atime for directory */

/*
 * Includes for diskquotas and mount structures.
 */
struct inode {
	struct list_head	i_hash;	/* hask table */
	unsigned long		i_ino;
	struct super_block *	i_sb;	/* pointer to supper block */
	struct inode_operations	*i_op;	/* operation for inode */
	struct wait_queue *	i_wait;
	struct list_head	i_lru;	/* LRU linker */
	struct list_head	i_link;	/* inode linker */
	/* Mutex for read and write the file contents. */
	struct mutex	i_mutex;

	atomic_t	i_count;	/* Reference count must atomic */
	atomic_t	i_nlink;	/* modify nlink don't need to lock inode */
	unsigned	i_state;
	unsigned	i_attr_flags;
	unsigned	i_flags;

	kdev_t		i_dev;
	kdev_t		i_rdev;
	umode_t		i_mode;
	uid_t		i_uid;
	gid_t		i_gid;
	off_t		i_size;
	unsigned	i_blocks;
	time_t		i_atime;
	time_t		i_mtime;
	time_t		i_ctime;
	
	unsigned long	i_data[1];
};

/* Inode state bits.. */
#define I_DIRTY			1
#define I_LOCK			2
#define I_FREEING		4

/* The allocated size of one inode is 256 */
#define	INODE_SIZE		256

#define	mark_inode_dirty(ino)	(ino)->i_state |= I_DIRTY

struct inode_operations {
	const struct file_operations * default_file_ops;
	int (*create) (struct inode *,struct dentry *,int);
	struct dentry * (*lookup) (struct inode *,struct dentry *);
	int (*link) (struct dentry *,struct inode *,struct dentry *);
	int (*unlink) (struct inode *,struct dentry *);
	int (*symlink) (struct inode *,struct dentry *,const char *);
	int (*mkdir) (struct inode *,struct dentry *,int);
	int (*rmdir) (struct inode *,struct dentry *);
	int (*mknod) (struct inode *,struct dentry *,int,int);
	int (*rename) (struct inode *, struct dentry *, struct inode *, struct dentry *);
	int (*readlink) (struct dentry *, char *,int);
	struct dentry * (*follow_link) (struct dentry *, struct dentry *, unsigned int);
	int (*readpage) (struct file *, struct page *);
	int (*writepage) (struct file *, struct page *);
	int (*bmap) (struct inode *,int);
	void (*truncate) (struct inode *);
	int (*permission) (struct inode *, int);
	int (*smap) (struct inode *,int);
	int (*updatepage) (struct file *, struct page *, unsigned long, unsigned int, int);
	int (*revalidate) (struct inode *);
};

extern struct inode * get_empty_inode(void);
extern struct inode * iget(struct super_block *, unsigned long ino);
extern void iput(struct inode *);
extern void __iput(struct inode *);
extern int  notify_change(struct inode * inode, struct iattr * attr);
extern void inode_setattr(struct inode * inode, struct iattr * attr);

extern void sync_inodes(kdev_t dev);
extern int invalidate_inodes(struct super_block * sb);

extern void lock_inode(struct inode * inode);
extern void unlock_inode(struct inode * inode);

extern void insert_inode_hash(struct inode *inode);
extern void remove_inode_hash(struct inode *inode);

extern void __wait_on_inode(struct inode *);

#define wait_on_inode(inode)	\
do { \
    if (inode->i_state & I_LOCK) \
	__wait_on_inode (inode); \
} while (0)

#endif /* __KERNEL__ */

#endif	/* __LINUX_DCACHE_H */

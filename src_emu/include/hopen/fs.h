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
 * $Source: /cvs/hopencvs/src/include/hopen/fs.h,v $
 * $Name:  $
 *
 * $Revision: 1.9 $     $Date: 2004/12/13 06:18:36 $
 * 
\**************************************************************************/

#ifndef _HOPEN_FS_H
#define _HOPEN_FS_H

/* count of file handle in a process */
#define NR_OPEN			64

/* block size is 1024 */
#define BLOCK_SIZE_BITS         10
#define BLOCK_SIZE		(1<<BLOCK_SIZE_BITS)

#define MAY_EXEC		1
#define MAY_WRITE		2
#define MAY_READ		4

#define FMODE_READ		1
#define FMODE_WRITE		2

#define READ			0
#define WRITE			1
#define READA			2	/* read-ahead  - don't block if no resources */
#define WRITEA			3	/* write-ahead - don't block if no resources */
/*
 * Kernel pointers have redundant information, so we can use a
 * scheme where we can return either an error code or a dentry
 * pointer with the same return value.
 *
 * This should be a per-architecture thing, to allow different
 * error and pointer decisions.
 */

#if defined (__CR16B__)
#define ERR_PTR(err)	((void *)(err & 0x001FFFFF))
#define PTR_ERR(ptr)	((int)(long)(ptr))
#define IS_ERR(ptr)	((unsigned long)(ptr) > 0x001FC000)
#else
#define ERR_PTR(err)	((void *)((int)(err)))
#define PTR_ERR(ptr)	((int)(ptr))
#define IS_ERR(ptr)	((unsigned)(ptr) > (unsigned)(-1000))
#endif


#ifdef __KERNEL__

#include    <sys/types.h>
#include    <hopen/list.h>
#include    <hopen/waitque.h>
#include    <hopen/ioctl.h>
#include    <hopen/inode.h>

struct poll_table_struct;

/* public flags for file_system_type */
#define FS_REQUIRES_DEV 1 
#define FS_NO_DCACHE    2 /* Only dcache the necessary things. */
#define FS_NO_PRELIM    4 /* prevent preloading of dentries, even if
			   * FS_NO_DCACHE is not set. */
/*
 * These are the fs-independent mount-flags: up to 16 flags are supported
 */
#define MS_RDONLY		1	/* Mount read-only */
#define MS_NOSUID		2	/* Ignore suid and sgid bits */
#define MS_NODEV		4	/* Disallow access to device special files */
#define MS_NOEXEC		8	/* Disallow program execution */
#define MS_SYNCHRONOUS		16	/* Writes are synced at once */
#define MS_REMOUNT		32	/* Alter flags of a mounted FS */
#define MS_MANDLOCK		64	/* Allow mandatory locks on an FS */

#define S_QUOTA			128	/* Quota initialized for file/directory/symlink */
#define S_APPEND		256	/* Append-only file */
#define S_IMMUTABLE		512	/* Immutable file */
#define S_NOATIME		1024	/* Do not update access times. */
#define S_NODIRATIME		2048    /* Do not update directory access times */
/*
 * Flags that can be altered by MS_REMOUNT
 */
#define MS_RMT_MASK (MS_RDONLY|MS_NOSUID|MS_NODEV|MS_NOEXEC|MS_SYNCHRONOUS|MS_MANDLOCK|MS_NOATIME|MS_NODIRATIME)

/*
 * Magic mount flag number. Has to be or-ed to the flag values.
 */
#define MS_MGC_VAL 0xC0ED0000	/* magic flag number to indicate "new" flags */
#define MS_MGC_MSK 0xffff0000	/* magic flag number mask */

/*
 * Note that read-only etc flags are inode-specific: setting some file-system
 * flags just means all the inodes inherit those flags by default. It might be
 * possible to override it selectively if you really wanted to with some
 * ioctl() that is not currently implemented.
 *
 * Exception: MS_RDONLY is always applied to the entire file system.
 *
 * Unfortunately, it is possible to change a filesystems flags with it mounted
 * with files in use.  This means that all of the inodes will not have their
 * i_flags updated.  Hence, i_flags no longer inherit the superblock mount
 * flags, so these have to be checked separately. -- rmk@arm.uk.linux.org
 */
#define __IS_FLG(inode,flg) (((inode)->i_sb && (inode)->i_sb->s_flags & (flg)) \
				|| (inode)->i_flags & (flg))

#define IS_RDONLY(inode) (((inode)->i_sb) && ((inode)->i_sb->s_flags & MS_RDONLY))
#define IS_NOSUID(inode)	__IS_FLG(inode, MS_NOSUID)
#define IS_NODEV(inode)		__IS_FLG(inode, MS_NODEV)
#define IS_NOEXEC(inode)	__IS_FLG(inode, MS_NOEXEC)
#define IS_SYNC(inode)		__IS_FLG(inode, MS_SYNCHRONOUS)
#define IS_MANDLOCK(inode)	__IS_FLG(inode, MS_MANDLOCK)

#define IS_QUOTAINIT(inode)	((inode)->i_flags & S_QUOTA)
#define IS_APPEND(inode)	((inode)->i_flags & S_APPEND)
#define IS_IMMUTABLE(inode)	((inode)->i_flags & S_IMMUTABLE)
#define IS_NOATIME(inode)	__IS_FLG(inode, MS_NOATIME)
#define IS_NODIRATIME(inode)	__IS_FLG(inode, MS_NODIRATIME)

/* Device major and mirror */
#define MINORBITS		16
#define MINORMASK		((1L << MINORBITS) - 1)

#define MAJOR(dev)		((unsigned int) ((dev) >> MINORBITS))
#define MINOR(dev)		((unsigned int) ((dev) & MINORMASK))
#define HASHDEV(dev)		((unsigned int) (dev))
#define NODEV			0
#define MKDEV(ma,mi)		(((long)(ma) << MINORBITS) | (mi))

#ifndef	UNNAMED_MAJOR
#define	UNNAMED_MAJOR		0
#endif

/* the read-only stuff doesn't really belong here, but any other place is
   probably as bad and I don't want to create yet another include file. */

#define BLKROSET	_IO(0x12,93)	/* set device read-only (0 = read-write) */
#define BLKROGET	_IO(0x12,94)	/* get read-only status (0 = read_write) */
#define BLKRRPART	_IO(0x12,95)	/* re-read partition table */
#define BLKGETSIZE	_IO(0x12,96)	/* return device size */
#define BLKFLSBUF	_IO(0x12,97)	/* flush buffer cache */
#define BLKRASET	_IO(0x12,98)	/* Set read ahead for block device */
#define BLKRAGET	_IO(0x12,99)	/* get current read ahead setting */
#define BLKFRASET	_IO(0x12,100)	/* set filesystem (mm/filemap.c) read-ahead */
#define BLKFRAGET	_IO(0x12,101)	/* get filesystem (mm/filemap.c) read-ahead */
#define BLKSECTSET	_IO(0x12,102)	/* set max sectors per request (ll_rw_blk.c) */
#define BLKSECTGET	_IO(0x12,103)	/* get max sectors per request (ll_rw_blk.c) */
#define BLKSSZGET	_IO(0x12,104)	/* get block device sector size (reserved for) */

#define BMAP_IOCTL	1		/* obsolete - kept for compatibility */
#define FIBMAP		_IO(0x00,1)	/* bmap access */
#define FIGETBSZ	_IO(0x00,2)	/* get the block size used for bmap */

#define UPDATE_ATIME(inode)

struct file_system_type;
struct vm_area_struct;
struct file_lock;
struct statfs;
struct qstr;
struct dentry;

struct fown_struct {
	int pid;		/* pid or -pgrp where SIGIO should be sent */
	int signum;		/* posix.1b rt signal to be delivered on IO */
};

struct file {
	mode_t		f_mode;
	umode_t		f_imode;
	loff_t		f_pos;
	atomic_t	f_count;
	unsigned 	f_flags;
	struct fown_struct	f_owner;
	struct inode *	f_inode;
	const struct file_operations *f_op;
	void	*	private_data;
};

#define FL_POSIX	1
#define FL_FLOCK	2
#define FL_BROKEN	4	/* broken flock() emulation */
#define FL_ACCESS	8	/* for processes suspended by mandatory locking */
#define FL_LOCKD	16	/* lock held by rpc.lockd */

struct fasync_struct {
	int    magic;
	int    fa_fd;
	struct fasync_struct	*fa_next; /* singly linked list */
	struct file 		*fa_file;
};

/*
 * This is the "filldir" function type, used by readdir() to let
 * the kernel specify what kind of dirent layout it wants to have.
 * This allows the kernel to read directories into kernel space or
 * to have different dirent layouts depending on the binary type.
 */
typedef int (*filldir_t)(void *, const char *, int, off_t, ino_t);

struct file_operations {
	loff_t (*llseek) (struct file *, loff_t, int);
	ssize_t (*read) (struct file *, char *, size_t, loff_t *);
	ssize_t (*write) (struct file *, const char *, size_t, loff_t *);
	int (*readdir) (struct file *, void *, filldir_t);
	int (*poll) (struct file *, int mask, struct poll_table_struct *);
	int (*ioctl) (struct inode *, struct file *, unsigned long, unsigned long);
	int (*mmap) (struct file *, struct vm_area_struct *);
	int (*open) (struct inode *, struct file *);
	int (*flush) (struct file *);
	int (*release) (struct inode *, struct file *);
	int (*fsync) (struct file *, struct dentry *);
	int (*fasync) (int, struct file *, int);
	int (*check_media_change) (kdev_t dev);
	int (*revalidate) (kdev_t dev);
	int (*lock) (struct file *, int, struct file_lock *);
};

extern struct dentry * fs_root;

extern int register_filesystem(struct file_system_type *);
extern int unregister_filesystem(struct file_system_type *);

extern int register_blkdev(unsigned int, const char *, struct file_operations *);
extern int unregister_blkdev(unsigned int major, const char * name);
extern int blkdev_open(struct inode * inode, struct file * filp);
extern int blkdev_release (struct inode * inode);
extern struct file_operations def_blk_fops;
extern struct inode_operations blkdev_inode_operations;

extern int register_chrdev(unsigned int, const char *, const struct file_operations *);
extern int unregister_chrdev(unsigned int major, const char * name);
extern int chrdev_open(struct inode * inode, struct file * filp);
extern struct file_operations def_chr_fops;
extern struct inode_operations chrdev_inode_operations;

extern struct inode_operations fifo_inode_operations;

extern void sync_supers(kdev_t dev);
extern void sync_inodes(kdev_t dev); 
extern struct super_block * get_super(kdev_t dev);
extern int invalidate_inodes(struct super_block * sb);
extern int invalidate_device(kdev_t dev, int do_sync);

extern char * bdevname(kdev_t dev);
extern char * cdevname(kdev_t dev);
extern char * kdevname(kdev_t dev);

extern void init_fifo(struct inode * inode);

/* Invalid inode operations -- fs/bad_inode.c */
extern void make_bad_inode(struct inode * inode);
extern int  is_bad_inode(struct inode * inode);

/* Fasync helper functions. */
extern int fasync_helper(int fd, struct file * filp, int on, struct fasync_struct **fapp);
extern void kill_fasync(struct fasync_struct *fa, int sig);

/* fs/dcache.c -- generic fs support functions */
extern ino_t find_inode_number(struct dentry *, struct qstr *);

extern int permission(struct inode * inode, int mask);

extern struct monitor fs_monitor;

struct file * fs_opendev (int major, int mirror, int flags);

#endif /* __KERNEL__ */

#endif

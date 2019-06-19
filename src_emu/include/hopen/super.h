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
 * $Source: /cvs/hopencvs/src/include/hopen/super.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/12/13 06:20:19 $
 * 
\**************************************************************************/

#ifndef _HOPEN_SUPER_H
#define _HOPEN_SUPER_H

#ifdef __KERNEL__

#include <hopen/list.h>

struct statfs;

struct super_block {
	struct file_system_type * s_type;
	struct super_operations	* s_op;
	struct wait_queue * s_wait;
	unsigned short	s_size;		/* sb size */
	unsigned short  s_isize;	/* sb inode size */
	kdev_t		s_dev;
	unsigned short	s_blocksize;	/* block size ,must be 1024 */
	unsigned char	s_lock;		/* lock the sb? */
	unsigned char	s_rd_only;
	unsigned char	s_dirt;
	unsigned int	s_magic;
	unsigned long	s_flags;
	struct file  *  s_devfile;	/* File structure of device */
	struct mutex	s_vfs_rename_mutex;
	struct dentry *	s_mount;	/* directory mounted by file system */
	struct inode *	s_root;		/* root inode */
	struct list_head s_inodes;	/* inode table */
	unsigned char *	s_devname;	/* device name */
	unsigned int	s_data[1];	/* private data */
};

/* We assume the size of super block is 256 */
#define SUPER_SIZE	256
struct iattr;

struct super_operations {
	void (*read_inode) (struct inode *);
	void (*write_inode) (struct inode *);
	void (*put_inode) (struct inode *);
	void (*delete_inode) (struct inode *);
	int  (*notify_change) (struct inode *, struct iattr *);
	void (*put_super) (struct super_block *);
	void (*write_super) (struct super_block *);
	int  (*statfs) (struct super_block *, struct statfs *, int);
	int  (*remount_fs) (struct super_block *, int *, char *);
	void (*clear_inode) (struct inode *);
	void (*umount_begin) (struct super_block *);
};

struct file_system_type {
	const char *name;		/* file system name */
	int	fs_flags;		/* flags */
	int	super_size;		/* sb size */
	struct super_block *(*read_super) (struct super_block *, void *, int);
};

extern struct dentry * fs_root;

extern void __wait_on_super(struct super_block *);

#define wait_on_super(sb)	\
	do { if ((sb)->s_lock) __wait_on_super(sb); } while(0)

#define lock_super(sb)		\
	do { if ((sb)->s_lock) __wait_on_super(sb); (sb)->s_lock = 1; } while (0)

#define unlock_super(sb)	\
	do { (sb)->s_lock = 0; wake_up(&(sb)->s_wait); } while(0)

#endif /* __KERNEL__ */

#endif

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
 * $Source: /cvs/hopencvs/src/include/hopen/proc_fs.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _HOPEN_PROC_FS_H
#define _HOPEN_PROC_FS_H

#include <hopen/fs.h>

/*
 * The proc filesystem constants/structures
 */

/* We always define these enumerators */

enum root_directory_inos {
	PROC_ROOT_INO = 1,
	PROC_IPC,
};

/* Finally, the dynamically allocatable proc entries are reserved: */

#define PROC_DYNAMIC_FIRST	4096
#define PROC_NDYNAMIC		4096

#define PROC_SUPER_MAGIC	0x9fa0

/*
 * This is not completely implemented yet. The idea is to
 * create an in-memory tree (like the actual /proc filesystem
 * tree) of these proc_dir_entries, so that we can dynamically
 * add new files to /proc.
 *
 * The "next" pointer creates a linked list of one /proc directory,
 * while parent/subdir create the directory structure (every
 * /proc file has a parent, but "subdir" is NULL for all
 * non-directory entries).
 *
 * "get_info" is called at "read", while "fill_inode" is used to
 * fill in file type/protection/owner information specific to the
 * particular /proc file.
 */

struct proc_dir_entry {
	struct proc_dir_entry *next, *parent, *subdir;

	unsigned short	low_ino;
	unsigned short	namelen;
	const char *	name;
	mode_t			mode;
	nlink_t			nlink;
	uid_t			uid;
	gid_t			gid;
	unsigned long	size;
	unsigned int	count;		/* use count */
	int				deleted;	/* delete flag */
	void *			data;
	struct inode_operations * ops;

	int  (*get_info)(char *, char **, off_t, int, int);
	void (*fill_inode)(struct inode *, int);
	int  (*read_proc)(char *page, char **start, off_t off,
			 int count, int *eof, void *data);
	int  (*write_proc)(struct file *file, const char *buffer,
			  unsigned long count, void *data);
	int  (*readlink_proc)(struct proc_dir_entry *de, char *page);
};

typedef	int (read_proc_t)(char *page, char **start, off_t off,
			  int count, int *eof, void *data);
typedef	int (write_proc_t)(struct file *file, const char *buffer,
			   unsigned long count, void *data);

extern struct proc_dir_entry		proc_root;
extern struct proc_dir_entry		proc_ipc;

extern void proc_root_init(void);
extern void proc_base_init(void);
extern void proc_ipc_init(void);

extern int proc_register(struct proc_dir_entry *, struct proc_dir_entry *);
extern int proc_unregister(struct proc_dir_entry *, int);

//extern struct dentry_operations proc_dentry_operations;
//extern struct super_block *proc_read_super(struct super_block *,void *,int);
//extern int init_proc_fs(void);
//extern struct inode * proc_get_inode(struct super_block *, int, struct proc_dir_entry *);
//extern int  proc_statfs(struct super_block *, struct statfs *, int);
//extern void proc_read_inode(struct inode *);
//extern void proc_write_inode(struct inode *);
//extern int  proc_permission(struct inode *, int);
//extern int  proc_match(int, const char *,struct proc_dir_entry *);

/*
 * These are generic /proc routines that use the internal
 * "struct proc_dir_entry" tree to traverse the filesystem.
 *
 * The /proc root directory has extended versions to take care
 * of the /proc/<pid> subdirectories.
 */
//extern int proc_readdir(struct file *, void *, filldir_t);
//extern struct dentry *proc_lookup(struct inode *, struct dentry *);

/* generic.c */
struct proc_dir_entry *create_proc_entry(const char *name, mode_t mode,
					 struct proc_dir_entry *parent);
void remove_proc_entry(const char *name, struct proc_dir_entry *parent);

/* proc_devtree.c */
extern void proc_device_tree_init(void);

#endif /* _LINUX_PROC_FS_H */

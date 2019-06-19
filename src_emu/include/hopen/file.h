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
 * $Source: /cvs/hopencvs/src/include/hopen/file.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/12/13 06:18:13 $
 * 
\**************************************************************************/

#ifndef _HOPEN_FILE_H
#define _HOPEN_FILE_H

struct file;

#ifndef MAX_FDS
#define	MAX_FDS		32
#endif

struct proc_file {
	unsigned	umask;		/* file mask */
	struct file   * image;		/* Image file of this process */
	struct dentry *	pwd;		/* current entry */
	struct dentry * root;		/* root entry*/
	struct file   * handle[MAX_FDS];/* handle table */
};

extern struct proc_file * get_proc_file(void);

extern int  get_unused_fd(void);
extern void put_unused_fd(int fd);
extern void fd_install(int fd, struct file *file);
/* return a original handle and clear it */
extern struct file * free_fd (int fd);

extern struct file * fget(int fd);
extern void fput(struct file *file); 
extern void put_filp(struct file *file);

extern struct file * get_empty_filp(void);
extern struct file *filp_open(const char *, int, int);
extern int filp_close(struct file *);

extern int filp_read(struct file *, char *, int);
extern int filp_write(struct file *, const char *, int);
//#define filp_read(fp,buf,len)	(fp)->f_op->read(fp, buf, len, &(fp)->f_pos)
//#define filp_write(fp,buf,len)	(fp)->f_op->write(fp, buf, len, &(fp)->f_pos)
#define filp_ioctl(fp,cmd,arg)	(fp)->f_op->ioctl((fp)->f_inode, fp, cmd, arg)

#endif //_HOPEN_FILE_H

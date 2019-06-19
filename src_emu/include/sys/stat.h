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
 * $Source: /cvs/hopencvs/src/include/sys/stat.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _SYS_STAT_H
#define _SYS_STAT_H

#include <sys/types.h>

struct stat {
	unsigned long	st_dev;
	unsigned long	st_ino;
	unsigned short	st_mode;
	unsigned short	st_nlink;
	unsigned short	st_uid;
	unsigned short	st_gid;
	unsigned long	st_rdev;
	unsigned long	st_size;
	unsigned long	st_blksize;
	unsigned long	st_blocks;
	unsigned long	st_atime;
	unsigned long	__unused1;
	unsigned long	st_mtime;
	unsigned long	__unused2;
	unsigned long	st_ctime;
	unsigned long	__unused3;
	unsigned long	__unused4;
	unsigned long	__unused5;
};

#define S_IFMT   0170000			// 0xF000
#define S_IFSOCK 0140000			// 0xC000
#define	S_IFIPC  0130000			// 0xB000
#define S_IFLNK	 0120000			// 0xA000
#define S_IFREG  0100000			// 0x8000
#define S_IFBLK  0060000			// 0x6000
#define S_IFDIR  0040000			// 0x4000
#define S_IFCHR  0020000			// 0x3000
#define S_IFIFO  0010000			// 0x1000
#define S_ISUID  0004000			// 0x0800
#define S_ISGID  0002000			// 0x0400
#define S_ISVTX  0001000			// 0x0200

#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#define S_ISIPC(m)	(((m) & S_IFMT) == S_IFIPC)
#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)

#define S_IRWXU 00700
#define S_IRUSR 00400
#define S_IWUSR 00200
#define S_IXUSR 00100

#define S_IRWXG 00070
#define S_IRGRP 00040
#define S_IWGRP 00020
#define S_IXGRP 00010

#define S_IRWXO 00007
#define S_IROTH 00004
#define S_IWOTH 00002
#define S_IXOTH 00001

#define S_IRWXUGO	(S_IRWXU|S_IRWXG|S_IRWXO)
#define S_IALLUGO	(S_ISUID|S_ISGID|S_ISVTX|S_IRWXUGO)
#define S_IRUGO		(S_IRUSR|S_IRGRP|S_IROTH)
#define S_IWUGO		(S_IWUSR|S_IWGRP|S_IWOTH)
#define S_IXUGO		(S_IXUSR|S_IXGRP|S_IXOTH)

#ifdef __cplusplus
extern "C" {
#endif

int chmod (const char * path, int mode);
int fchmod (int fildes, mode_t mode);
int stat (const char * file_name, struct stat * buf);
int fstat (int filedes, struct stat * buf);
int lstat (const char * file_name, struct stat * buf);
int mkdir (const char * pathname, int mode);
int umask (int mask);

int mknod (const char * filename, int mode, unsigned long dev);
int mkfifo ( const char *pathname, int mode );

#ifdef __cplusplus
}
#endif

#endif

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _HMAP_POSOXIO_H_
#define _HMAP_POSOXIO_H_

#if defined(WIN32) || defined(HMAP_DLL)
#include    <io.h>
#include    <fcntl.h>
#include    <direct.h>
#include    <sys/stat.h>
#include    <time.h>

#define PATH_MAX	_MAX_PATH
#define NAME_MAX	_MAX_NAME

typedef void DIR;

struct dirent
{
	long    d_ino;
	short	d_unused;
	short	d_reclen;
	char    d_name[1];		/* File name. */
};

DIR *opendir(const char *name);
int closedir(DIR * dir);
struct dirent *readdir(DIR *dir);
void rewinddir(DIR *dir);

int fcntl (int fd, int cmd, ...);
int fsync (int fd);

struct timeval;
struct timezone;

int gettimeofday(struct timeval *tv, struct timezone *tz);
int settimeofday(const struct timeval *tv, const struct timezone *tz);

#endif

#ifdef LINUX
#include    <limits.h>
#include    <unistd.h>
#include    <fcntl.h>
#include    <dirent.h>
#include    <time.h>
#include    <sys/stat.h>
#include    <sys/time.h>

/* No O_BINARY flags in linux file system */
#define     O_BINARY	0

#endif

#ifdef HP30
#include    <limits.h>
#include    <unistd.h>
#include    <fcntl.h>
#include    <dirent.h>
#include    <time.h>
#include    <sys/stat.h>
#include    <sys/time.h>

#define _MAX_PATH   256
#endif


#endif /* _HMAP_POSOXIO_H_ */

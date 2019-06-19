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

#ifndef _DIRENT_H_
#define _DIRENT_H_

struct dirent
{
	long		d_ino;
	unsigned short	d_unused;
	unsigned short	d_reclen;
	char		d_name[1];		/* File name. */
	/* NOTE: The name in the dirent structure points to the name in the
	 *       finddata_t structure in the DIR. */
};

/*
 * This is an internal data structure. Good programmers will not use it
 * except as an argument to one of the functions below.
 */

typedef struct
{
	int fd;
	int offset;
	int size;
	int allocation;
	char *data;
} DIR;

#ifdef	__cplusplus
extern "C" {
#endif

DIR *	opendir (const char* szPath);
struct dirent*	readdir (DIR* dir);
int	closedir (DIR* dir);
void	rewinddir (DIR* dir);

#ifdef __cplusplus
}
#endif

#endif	/* Not _DIREND_H_ */

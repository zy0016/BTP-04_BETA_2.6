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

#ifndef _DIR_H_
#define	_DIR_H_

/*
 * Attributes of files as returned by _findfirst et al.
 */
#define	_A_NORMAL	0x00000000
#define	_A_RDONLY	0x00000001
#define	_A_HIDDEN	0x00000002
#define	_A_SYSTEM	0x00000004
#define	_A_VOLID	0x00000008
#define	_A_SUBDIR	0x00000010
#define	_A_ARCH		0x00000020

/*
 * The following structure is filled in by _findfirst or _findnext when
 * they succeed in finding a match.
 */
struct _finddata_t
{
	unsigned		attrib;			/* Attributes, see constants above. */
	unsigned long	time_create;
	unsigned long	time_access;	/* always midnight local time */
	unsigned long	time_write;
	unsigned long	size;
	char			name[256];		/* may include spaces. */
};

#ifdef	__cplusplus
extern "C" {
#endif

/*
 * Functions for searching for files. _findfirst returns -1 if no match
 * is found. Otherwise it returns a handle to be used in _findnext and
 * _findclose calls. _findnext also returns -1 if no match could be found,
 * and 0 if a match was found. Call _findclose when you are finished.
 */
int		_findfirst	(const char* szFilespec, struct _finddata_t* find);
int		_findnext	(int nHandle, struct _finddata_t* find);
int		_findclose	(int nHandle);

int		_chdir	(const char* szPath);
char*	_getcwd	(char* caBuffer, int nBufferSize);
int		_mkdir	(const char* szPath, int mode);
char*	_mktemp	(char* szTemplate);
int		_rmdir	(const char* szPath);


#ifndef _NO_OLDNAMES

int		chdir	(const char* szPath);
char*	getcwd	(char* caBuffer, int nBufferSize);
int		mkdir	(const char* szPath, int mode);
char*	mktemp	(char* szTemplate);
int		rmdir	(const char* szPath);

#endif /* Not _NO_OLDNAMES */

#ifdef	__cplusplus
}
#endif

#endif	/* Not _DIR_H_ */

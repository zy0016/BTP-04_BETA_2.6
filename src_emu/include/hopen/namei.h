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
 * $Source: /cvs/hopencvs/src/include/hopen/namei.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _HOPEN_NAMEI_H
#define	_HOPEN_NAMEI_H

/*
 * The bitmask for a lookup event:
 *  - follow links at the end
 *  - require a directory
 *  - ending slashes ok even for nonexistent files
 *  - internal "there are more path compnents" flag
 */
#define LOOKUP_FOLLOW		(1)
#define LOOKUP_DIRECTORY	(2)
#define LOOKUP_SLASHOK		(4)
#define LOOKUP_CONTINUE		(8)

extern char * getname(const char * filename);
extern void putname(char * name);
extern struct dentry * lookup_dentry(const char * name, struct dentry * base, int lookup_flags);
extern struct dentry * __namei(const char *pathname, int lookup_flags);
extern int lookup_flags(unsigned int oflag);

#define namei(pathname)		__namei(pathname, 1)
#define lnamei(pathname)	__namei(pathname, 0)

#endif // _HOPEN_NAMEI_H

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
 * $Source: /cvs/hopencvs/src/include/sys/procenv.h,v $
 * $Name:  $
 *
 * $Revision: 1.1 $     $Date: 2003/05/28 03:47:11 $
 * 
\**************************************************************************/

#ifndef _PROCENV_H_
#define	_PROCENV_H_

typedef struct _procenv {
	void * stdin, * stdout, * stderr;
//	char ** environ;
} procenv;

#endif

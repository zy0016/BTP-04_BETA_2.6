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
 * $Source: /cvs/hopencvs/src/include/hopen/exportfunc.h,v $
 * $Name:  $
 *
 * $Revision: 1.1 $     $Date: 2003/05/30 06:48:55 $
 * 
\**************************************************************************/

#ifndef _HOPEN_EXPORTFUNC_H_
#define	_HOPEN_EXPORTFUNC_H_

extern unsigned long libc_find_proc_address (const char * func_name);

#ifdef WIN32
#define _JMP_TO_DLL_EXPORT_FUNC_(func_name)			\
	do {							\
		static unsigned long func_name##_addr = 0;	\
		if (func_name##_addr != 0 )			\
		{						\
			__asm jmp func_name##_addr		\
		}						\
		func_name##_addr = libc_find_proc_address (#func_name);	\
		__asm jmp func_name##_addr			\
	} while (0)

#endif

#endif	//_HOPEN_EXPORTFUNC_H_

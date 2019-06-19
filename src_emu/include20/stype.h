/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN Device Drive
 *
 * Purpose :	Device Drive
 *  
 * Author  :	Di Ling
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /Hopen/include/ddi/stype.h       $
 * $Workfile:: stype.h            $
 * $Revision:: 4  $     $Date:: 03-12-01 16:20      $
 *
\**************************************************************************/

/***************************************************************************
 *
 * Hopen Device Driver Interface
 *
 * $Revision: 4 $     $Date: 03-12-01 16:20 $    
 *
 * $Log: /Hopen/include/ddi/stype.h $
 * 
 * 4     03-12-01 16:20 Dgli
 * add _C166 for vme flatform
 * 
 * 3     99-12-27 9:39 Dling
 * include hopendef.h
 * 
 * 1     99-11-08 14:20 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _HOPEN_STYPE_H
#define _HOPEN_STYPE_H

#ifndef _WINDEF_
#include "hpdef.h"
#endif

/* interface */

#ifdef _MSC_VER
#define _STD_CALL __stdcall
#else
#define _STD_CALL
#endif /* _MSC_VER */

#if defined(__cplusplus) && !defined(DEV_CINTERFACE)

#define _INTERFACE class
#ifndef _C166
#define _STDMETHOD(method)				virtual UINT _STD_CALL method
#else
#define _STDMETHOD(method)				virtual DWORD _STD_CALL method
#endif
#define _STDMETHOD_(type, method)		virtual type _STD_CALL method

#else	/* C style interface */

#define _INTERFACE struct
#ifndef _C166
#define _STDMETHOD(method)				UINT (_STD_CALL *method)
#else
#define _STDMETHOD(method)				DWORD (_STD_CALL *method)
#endif
#define _STDMETHOD_(type, method)		type (_STD_CALL *method)

#endif

/* null pointer */
#ifndef NULL
#define NULL (void*)0
#endif // NULL

#ifdef _C166
#undef FAR
#undef LocalAlloc
#undef LocalFree
#undef OS_SetError
#define _HOPEN_VER      200
#define FAR	_huge
#define LocalAlloc(flag, size)      malloc(size)
#define LocalFree(handle)           free(handle)
#define OS_SetError(error)	((void)0)
extern void *        malloc ( unsigned int );
extern void          free   ( void * );
extern void debugex_nonl(char *format, ...);

extern int fs_stricmp(const _huge char *string1, const _huge char *string2);
extern int fs_strncmp(const _huge char *string1, const _huge char *string2, long count);
extern char *fs_strncpy(_huge char *strDest, const _huge char *strSource, long count);
extern void *fs_memcpy(_huge void *dest, const _huge void *src, long count);
#define stricmp(first, last)		fs_stricmp(first, last)
#define strncmp(first, last, count)	fs_strncmp(first, last, count)
#define strncpy(dst, src, count)	fs_strncpy(dst, src, count)
#define memcpy(dst, src, count)		fs_memcpy(dst, src, count)
#else
#undef FAR
#define FAR
#endif

#endif // _HOPEN_STYPE_H

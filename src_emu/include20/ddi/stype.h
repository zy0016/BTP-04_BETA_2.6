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
 * $Archive:: /hopen2.0/include/stype.h								$
 * $Workfile::	stype.h												$
 * $Revision:: 4  $     $Date:: 03-04-26 3:28p						$
 *
\**************************************************************************/

/***************************************************************************
 *
 * Hopen Device Driver Interface
 *
 * $Revision: 3 $     $Date: 99-12-27 9:39 $    
 *
 * $Log: /Hopen/include/ddi/stype.h $
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
#define _STDMETHOD(method)				virtual UINT _STD_CALL method
#define _STDMETHOD_(type, method)		virtual type _STD_CALL method

#else	/* C style interface */

#define _INTERFACE struct
#define _STDMETHOD(method)				UINT (_STD_CALL *method)
#define _STDMETHOD_(type, method)		type (_STD_CALL *method)

#endif

/* null pointer */
#ifndef NULL
#define NULL (void*)0
#endif // NULL


#endif // _HOPEN_STYPE_H

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : umutex.h
 *
 * Purpose  : Declare mutex os object struct
 *
\**************************************************************************/

#ifndef	_HAVE_MUTEX_H
#define	_HAVE_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

//	TODO	:	DEFINE MACRO CONST
#define	MXM_LOCK		(OSM_USER+0x0001)
#define	MXM_UNLOCK		(OSM_USER+0x0002)

//	TODO	:	DECLARE OUTPUT FUNCTION
BOOL	f_utCreateMutex ( PHUT_ENTRY pObj );

#ifdef __cplusplus
}
#endif

#endif // _HAVE_MUTEX_H

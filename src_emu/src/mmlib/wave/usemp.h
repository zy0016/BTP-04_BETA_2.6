/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : usemp.h
 *
 * Purpose  : Declare semaphore os object struct
 *
\**************************************************************************/

#ifndef	_HAVE_SEMAPHORE_H
#define	_HAVE_SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

//	TODO	:	DEFINE DATA STRUCT
typedef struct ut_sem_open_s	//	对象的基本参数
{
	int		v_nMax;
	int		v_nShare;
	int		v_nKey;
	int		v_nValue;
	char*	v_pName;
} UT_SEM_OPEN, *HUT_SEM_OPEN;

//	TODO	:	DEFINE MACRO CONST
#define	SEM_WAITE	(OSM_USER+0x0001)
#define	SEM_POST	(OSM_USER+0x0002)

//	TODO	:	DECLARE OUTPUT FUNCTION
BOOL	f_utCreateSemaphore ( PHUT_ENTRY pObj );

#ifdef __cplusplus
}
#endif

#endif // _HAVE_SEMAPHORE_H

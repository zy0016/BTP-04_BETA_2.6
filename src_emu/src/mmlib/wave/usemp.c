/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : usemp.c
 *
 * Purpose  : Define semaphore os object struct
 *
\**************************************************************************/

#include "utils.h"

//	TODO	:	DEFINE DATA STRUCT
typedef struct ut_semp_s		//	对象的基本参数
{
	struct	ut_entry_s	v_aOpt;

	int		v_nMax;
	int		v_nAttrib;
	HSEMP	v_hSemp;
} UT_SEMP, *HUT_SEMP;

//	TODO	:	DEFINE MACRO CONST
#define	SEMA_INITIALIZE		0x0001

//	TODO	:	DECLARE MODULE INLINE FUNCTION
static	BOOL	f_Operate	( struct ut_entry_s* hThis, int nCode,
							 unsigned long nOne, unsigned long nTwo );
static	BOOL	f_Initialize( struct ut_semp_s* hThis, HUT_SEM_OPEN pInfo );
static	BOOL	f_Release	( struct ut_semp_s* hThis );

static	BOOL	f_Waite		( struct ut_semp_s* hThis );
static	BOOL	f_Post		( struct ut_semp_s* hThis, int nNum );

/**************************************************************************\
 * Function	f_utCreateSemaphore( PHUT_ENTRY pObj )
 * Purpose	Create Semaphore object control
 * Params	pObj	: a pointer to receive object handle
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
BOOL f_utCreateSemaphore( PHUT_ENTRY pObj )
{
	HUT_SEMP	hThis;

	if ( NULL == pObj )
		return FALSE;

	*pObj = NULL;
	hThis = (HUT_SEMP)mmi_Malloc ( sizeof( UT_SEMP ) );
	if ( NULL == hThis )
		return FALSE;

	hThis->v_nMax		= 0;
	hThis->v_nAttrib	= 0;
	hThis->v_aOpt.f_Operate	= &f_Operate;

	return (BOOL)( *pObj = &hThis->v_aOpt );
}

/**************************************************************************\
 * Function	f_Operate( struct ut_entry_s* hThis, int nCode,
							int nOne, int nTwo )
 * Purpose	Process obj message
 * Params	hThis	: device handle
			nCode	: command code
			nOne	: 
			nTwo	: 
 * Return	Successed return ZERO
 * Remarks	
 **************************************************************************/
static	BOOL f_Operate( struct ut_entry_s* hThis, int nCode,
					   unsigned long nOne, unsigned long nTwo )
{
	HUT_SEMP	hObj;

	hObj = (HUT_SEMP)hThis;
	if ( NULL == hObj )
		return FALSE;

	switch ( nCode )
	{
	case OSM_INITIALIZE:
		return f_Initialize ( hObj, (HUT_SEM_OPEN)nOne );
	case OSM_RELEASE:
		return f_Release ( hObj );
	case SEM_WAITE:
		return f_Waite ( hObj );
	case SEM_POST:
		return f_Post ( hObj, nOne );
	}

	return FALSE;
}

/**************************************************************************\
 * Function	f_Initialize( struct ut_semp_s* hThis, HUT_SEM_OPEN pInfo )
 * Purpose	Create object unit
 * Params	hThis	: object handle
			pInfo	: create info
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
static	BOOL f_Initialize( struct ut_semp_s* hThis, HUT_SEM_OPEN pInfo )
{
	if ( hThis->v_nAttrib & SEMA_INITIALIZE )
		return TRUE;

#ifdef WIN32_OS
	hThis->v_hSemp	= CreateSemaphore ( NULL, pInfo->v_nValue, pInfo->v_nMax,
		pInfo->v_pName );
	if ( NULL == hThis->v_hSemp )
		return FALSE;

	return (BOOL)(hThis->v_nAttrib |= SEMA_INITIALIZE);
#endif

#ifdef POLLEX_OS
	hThis->v_hSemp	= CreateSemaphore ( pInfo->v_nKey, pInfo->v_nValue );
	if ( NULL == hThis->v_hSemp )
		return FALSE;

	return (BOOL)(hThis->v_nAttrib |= SEMA_INITIALIZE);
#endif

#ifdef LINUX_OS
	sem_init ( &hThis->v_hSemp, pInfo->v_nShare, pInfo->v_nValue );

	return (BOOL)(hThis->v_nAttrib |= SEMA_INITIALIZE);
#endif

	return FALSE;
}

/**************************************************************************\
 * Function	f_Release( struct ut_semp_s* hThis )
 * Purpose	release control object
 * Params	hThis	: object handle
 * Return	TRUE
 * Remarks	
 **************************************************************************/
static	BOOL f_Release( struct ut_semp_s* hThis )
{
	if ( !(hThis->v_nAttrib & SEMA_INITIALIZE) )
	{
		mmi_Free ( hThis );

		return TRUE;
	}

	f_Post ( hThis, 1 );

#ifdef WIN32_OS
	if ( NULL != hThis->v_hSemp )
		CloseHandle ( hThis->v_hSemp );
#endif

#ifdef POLLEX_OS
	DestroySemaphore ( hThis->v_hSemp );
#endif

#ifdef LINUX_OS
	sem_destroy( &hThis->v_hSemp );
#endif

	mmi_Free ( hThis );

	return TRUE;
}

/**************************************************************************\
 * Function	f_Waite( struct ut_semp_s* hThis )
 * Purpose	wait a semaphore obejct
 * Params	hThis	: mutex object handle
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
static	BOOL f_Waite( struct ut_semp_s* hThis )
{
	if ( !(hThis->v_nAttrib & SEMA_INITIALIZE) )
		return FALSE;

#ifdef WIN32_OS
	if ( NULL != hThis->v_hSemp )
		WaitForSingleObject ( hThis->v_hSemp, INFINITE );

	return TRUE;
#endif

#ifdef POLLEX_OS
	WaitSemaphore ( hThis->v_hSemp, -1 );

	return TRUE;
#endif

#ifdef LINUX_OS
	sem_wait ( &hThis->v_hSemp );

	return TRUE;
#endif

	return FALSE;
}

/**************************************************************************\
 * Function	f_Post( struct ut_semp_s* hThis, int nNum )
 * Purpose	Post a semaphore
 * Params	hThis	: mutex object handle
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
static	BOOL f_Post( struct ut_semp_s* hThis, int nNum )
{
	int	nVal;

	if ( !(hThis->v_nAttrib & SEMA_INITIALIZE) )
		return FALSE;

#ifdef WIN32_OS
	if ( NULL != hThis->v_hSemp )
		ReleaseSemaphore ( hThis->v_hSemp, nNum, NULL );

	return TRUE;
#endif

#ifdef POLLEX_OS
	ReleaseSemaphore ( hThis->v_hSemp, nNum );

	return TRUE;
#endif

#ifdef LINUX_OS
	sem_getvalue ( &hThis->v_hSemp, &nVal );
	if ( nVal >= hThis->v_nMax )
		return TRUE;

	sem_post ( &hThis->v_hSemp );

	return TRUE;
#endif

	return FALSE;
}

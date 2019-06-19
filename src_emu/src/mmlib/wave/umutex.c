/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : umutex.c
 *
 * Purpose  : Define mutex os object struct
 *
\**************************************************************************/

#include "utils.h"

//	TODO	:	DEFINE DATA STRUCT
typedef struct ut_mutex_s		//	对象的基本参数
{
	struct	ut_entry_s	v_aOpt;

	int		v_nAttrib;
	HMUTEX	v_hMutex;
} UT_MUTEX, *HUT_MUTEX;

//	TODO	:	DEFINE MACRO CONST
#define	MXA_INITIALIZE		0x0001

//	TODO	:	DECLARE MODULE INLINE FUNCTION
static	BOOL	f_Operate	( struct ut_entry_s* hThis, int nCode,
							 unsigned long nOne, unsigned long nTwo );
static	BOOL	f_Initialize( struct ut_mutex_s* hThis, char* pName );
static	BOOL	f_Release	( struct ut_mutex_s* hThis );

static	BOOL	f_Lock		( struct ut_mutex_s* hThis );
static	BOOL	f_UnLock	( struct ut_mutex_s* hThis );

/**************************************************************************
 * Function	f_utCreateMutex( PHUT_ENTRY pObj )
 * Purpose	Create object
 * Params	pObj	: a pointer to receive object handle
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
BOOL f_utCreateMutex( PHUT_ENTRY pObj )
{
	HUT_MUTEX	hThis;

	if ( NULL == pObj )
		return FALSE;

	*pObj = NULL;
	hThis = (HUT_MUTEX)mmi_Malloc ( sizeof( UT_MUTEX ) );
	if ( NULL == hThis )
		return FALSE;

	hThis->v_aOpt.f_Operate	= &f_Operate;
	hThis->v_nAttrib	= 0;

	return (BOOL)( *pObj = &hThis->v_aOpt );
}

/**************************************************************************
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
	HUT_MUTEX	hObj;

	hObj = (HUT_MUTEX)hThis;
	if ( NULL == hObj )
		return FALSE;

	switch ( nCode )
	{
	case OSM_RELEASE:
		return f_Release ( hObj );
	case OSM_INITIALIZE:
		return f_Initialize ( hObj, (char *)nOne );
	case MXM_LOCK:
		return f_Lock ( hObj );
	case MXM_UNLOCK:
		return f_UnLock( hObj );
	}

	return FALSE;
}

/**************************************************************************
* Function	f_Initialize( struct ut_mutex_s* hThis, char* pName )
* Purpose	Initialize object unit
* Params	hThis	: object handle
			pName	: create info
* Return	Successed return non ZERO
* Remarks	
**************************************************************************/
static	BOOL f_Initialize( struct ut_mutex_s* hThis, char* pName )
{
	if ( hThis->v_nAttrib & MXA_INITIALIZE )
		return TRUE;

#ifdef WIN32_OS
	hThis->v_hMutex	= CreateMutex ( NULL, FALSE, pName );
	if ( NULL == hThis->v_hMutex )
		return FALSE;

	return (BOOL)(hThis->v_nAttrib |= MXA_INITIALIZE);
#endif

#ifdef POLLEX_OS
	pthread_mutex_init ( &hThis->v_hMutex, NULL );

	return (BOOL)(hThis->v_nAttrib |= MXA_INITIALIZE);
#endif

#ifdef LINUX_OS
	pthread_mutex_init ( &hThis->v_hMutex, NULL );

	return (BOOL)(hThis->v_nAttrib |= MXA_INITIALIZE);
#endif

	return FALSE;
}

/**************************************************************************
 * Function	f_Release( struct ut_mutex_s* hThis )
 * Purpose	release control object
 * Params	hThis	: object handle
 * Return	TRUE
 * Remarks	
 **************************************************************************/
static	BOOL f_Release( struct ut_mutex_s* hThis )
{
	if ( !(hThis->v_nAttrib & MXA_INITIALIZE) )
	{
		mmi_Free ( hThis );

		return TRUE;
	}

	f_Lock ( hThis );

#ifdef WIN32_OS
	CloseHandle ( hThis->v_hMutex );
#endif

#ifdef POLLEX_OS
	pthread_mutex_destroy ( &hThis->v_hMutex );
#endif

#ifdef LINUX_OS
	pthread_mutex_destroy ( &hThis->v_hMutex );
#endif

	mmi_Free ( hThis );

	return TRUE;
}

/**************************************************************************
 * Function	f_Lock( struct ut_mutex_s* hThis )
 * Purpose	lock a mutex obejct
 * Params	hThis	: mutex object handle
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
static	BOOL f_Lock( struct ut_mutex_s* hThis )
{
	if ( !(hThis->v_nAttrib & MXA_INITIALIZE) )
		return FALSE;

#ifdef WIN32_OS
	if ( NULL != hThis->v_hMutex )
		WaitForSingleObject	( hThis->v_hMutex, INFINITE );

	return TRUE;
#endif

#ifdef POLLEX_OS
	pthread_mutex_lock ( &hThis->v_hMutex );

	return TRUE;
#endif

#ifdef LINUX_OS
	pthread_mutex_lock ( &hThis->v_hMutex );

	return TRUE;
#endif

	return FALSE;
}

/**************************************************************************
 * Function	f_UnLock( struct ut_mutex_s* hThis )
 * Purpose	unlock a mutex obejct
 * Params	hThis	: mutex object handle
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
static	BOOL f_UnLock( struct ut_mutex_s* hThis )
{
	if ( !(hThis->v_nAttrib & MXA_INITIALIZE) )
		return FALSE;

#ifdef WIN32_OS
	if ( NULL != hThis->v_hMutex )
		ReleaseMutex ( hThis->v_hMutex );

	return TRUE;
#endif

#ifdef POLLEX_OS
	pthread_mutex_unlock ( &hThis->v_hMutex );

	return TRUE;
#endif

#ifdef LINUX_OS
	pthread_mutex_unlock ( &hThis->v_hMutex );

	return TRUE;
#endif

	return FALSE;
}

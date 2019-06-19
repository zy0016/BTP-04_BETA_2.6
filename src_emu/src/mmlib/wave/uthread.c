/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : uthread.c
 *
 * Purpose  : Define thread os object struct
 *
\**************************************************************************/

#include "utils.h"

//	TODO	:	DEFINE DATA STRUCT
typedef struct ut_thread_s		//	对象的基本参数
{
	struct	ut_entry_s	v_aOpt;

	int		v_nID;
	int		v_nAttrib;
	int		v_nhandle;
	HTHREAD	v_hThread;
} UT_THREAD, *HUT_THREAD;

//	TODO	:	DEFINE MACRO CONST
#define	THREADA_INITIALIZE		0x0001

//	TODO	:	DECLARE MODULE INLINE FUNCTION
static	BOOL	f_Operate	( struct ut_entry_s* hThis, int nCode,
							 unsigned long nOne, unsigned long nTwo );
static	BOOL	f_Initialize( struct ut_thread_s* hThis,
							 unsigned long nhandle, THREAD_FUNC hFunc );
static	BOOL	f_Release	( struct ut_thread_s* hThis );

/**************************************************************************\
 * Function	f_utCreateThread( PHUT_ENTRY pObj )
 * Purpose	Create thread object control
 * Params	pObj	: a pointer to recieve object handle
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
BOOL f_utCreateThread( PHUT_ENTRY pObj )
{
	HUT_THREAD	hThis;

	if ( NULL == pObj )
		return FALSE;

	*pObj = NULL;
	hThis = (HUT_THREAD)mmi_Malloc ( sizeof( UT_THREAD ) );
	if ( NULL == hThis )
		return FALSE;

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
	HUT_THREAD	hObj;

	hObj = (HUT_THREAD)hThis;
	if ( NULL == hObj )
		return FALSE;

	switch ( nCode )
	{
	case OSM_INITIALIZE:
		return f_Initialize ( hObj, nOne, (THREAD_FUNC)nTwo );
	case OSM_RELEASE:
		return f_Release( hObj );
	}

	return FALSE;
}

/**************************************************************************\
* Function	f_Initialize( struct ut_thread_s* hThis, int nhandle,
							 THREAD_FUNC hFunc )
 * Purpose	Initialize object unit
 * Params	hThis	: object handle
			nhandle	: create info
			hFunc	: thread function
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
static	BOOL f_Initialize( struct ut_thread_s* hThis,
						  unsigned long nhandle, THREAD_FUNC hFunc )
{
	if ( hThis->v_nAttrib & THREADA_INITIALIZE )
		return TRUE;

	hThis->v_nhandle = nhandle;

#ifdef WIN32_OS
	hThis->v_hThread = CreateThread ( NULL, 0, hFunc, (void *)nhandle, 0, &hThis->v_nID );	

	return (BOOL)(hThis->v_nAttrib |= THREADA_INITIALIZE);
#endif

#ifdef POLLEX_OS
	pthread_create ( &hThis->v_hThread, 0, hFunc, (void *)nhandle );

	return (BOOL)(hThis->v_nAttrib |= THREADA_INITIALIZE);
#endif

#ifdef LINUX_OS
	pthread_create ( &hThis->v_hThread, 0, hFunc, (void *)nhandle );

	return (BOOL)(hThis->v_nAttrib |= THREADA_INITIALIZE);
#endif

	return FALSE;
}

/**************************************************************************\
 * Function	f_Release( struct ut_thread_s* hThis )
 * Purpose	release control object
 * Params	hThis	: object handle
 * Return	TRUE
 * Remarks	
 **************************************************************************/
static	BOOL f_Release( struct ut_thread_s* hThis )
{
	if ( hThis->v_nAttrib & THREADA_INITIALIZE )
	{
#ifdef WIN32_OS
		WaitForSingleObject ( hThis->v_hThread, INFINITE );

		CloseHandle ( hThis->v_hThread );
#endif

#ifdef POLLEX_OS
		pthread_join ( hThis->v_hThread, NULL );
#endif

#ifdef LINUX_OS
		pthread_join ( hThis->v_hThread, NULL );
#endif
	}

	mmi_Free ( hThis );

	return TRUE;
}

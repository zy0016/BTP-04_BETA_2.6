/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : utils.c
 *
 * Purpose  : Implement os compatible module
 *
\**************************************************************************/

#include "utils.h"

/**************************************************************************\
 * Function	mmi_Malloc( unsigned int nSize )
 * Purpose	malloc one memory
 * Params	nSize	: memory size
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
void* mmi_Malloc( unsigned int nSize )
{
	return malloc( nSize );
}

/**************************************************************************\
 * Function	mmi_Realloc( void* pd, int nSize )
 * Purpose	realloc one memory
 * Params	nSize	: memory size
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
void* mmi_Realloc( void* pd, unsigned int nSize )
{
	return realloc( pd, nSize );
}

/**************************************************************************\
 * Function	mmi_Free( void* pVoid )
 * Purpose	release one memory
 * Params	pVoid	: memory address
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
void mmi_Free( void* pVoid )
{
	if ( NULL == pVoid )
		return;

	free( pVoid );
}

/**************************************************************************\
 * Function	mmi_Set( void *pd, int n, unsigned int size_t )
 * Purpose	Clean one memory
 * Params	pd	: memory address
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
void* mmi_Set( void *pd, int n, unsigned int size_t )
{
	return memset( pd, n, size_t );
}

/**************************************************************************\
 * Function	mmi_Copy( void *pd, const void *ps, unsigned int size_t )
 * Purpose	Copy one memory
 * Params	pd	: memory address
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
void* mmi_Copy( void *pd, const void *ps, unsigned int size_t )
{
	return memcpy( pd, ps, size_t );
}

/**************************************************************************\
 * Function	mmi_Move( void *pd, const void *ps, unsigned int size_t )
 * Purpose	Move one memory
 * Params	pd	: memory address
 * Return	Successed return non ZERO
 * Remarks	
 **************************************************************************/
void* mmi_Move( void *pd, const void *ps, unsigned int size_t )
{
	return memmove( pd, ps, size_t );
}

/**************************************************************************\
* Function	f_GetTick( void )
* Purpose	return systime from systerm boot
* Params	void
* Return	timer ticks
* Remarks	
**************************************************************************/
DWORD f_GetTick( void )
{
#ifdef WIN32_OS
	return GetTickCount();
#endif

#ifdef POLLEX_OS
	return GetTickCount();
#endif

#ifdef LINUX_OS
	struct timeval timer;

	if ( gettimeofday( &timer, NULL ) != 0 )
		return 0;

	return (timer.tv_sec*1000+timer.tv_usec/1000);
#endif

	return 0;
}

/**************************************************************************\
* Function	f_Sleep( int nTime )
* Purpose	Delay a thread obejct
* Params	nTime	: delay Milliseconds TIME
* Return	void
* Remarks	
**************************************************************************/
void f_Sleep( long nTime )
{
#ifdef WIN32_OS
	Sleep( nTime );
#endif

#ifdef POLLEX_OS
	usleep( nTime*1000 );
#endif

#ifdef LINUX_OS
	usleep( nTime*1000 );
#endif
}

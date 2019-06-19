/**************************************************************************\
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 *
 * Model		tilib.h
 *
 * Purpose      Define some variable and functon for the porpose of tcpip lib
 *				Our Net Station.
 *  
 * Author       
 *
 *-------------------------------------------------------------------------
 *
 * $Archive::                                                       $
 * $Workfile::                                                      $
 * $Revision::    $     $Date::                                     $
 * 
\**************************************************************************/
/***************************************************************************
 *
 * Header file for LIB API.
 *
 * $Revision: 5 $     $Date: 01-03-20 11:49 $    
 *
 * $Log: /tcpipsec/include/tilib.h $
 * 
 *
 ***************************************************************************
 */


#ifndef	_TILIB_H_
#define	_TILIB_H_


#include "string.h"
#include "hp_kern.h"

#ifdef _TCPIP_BULID_VC_
#include "warning.h"
#endif
#ifndef _TCPIP_NOWIN_
#include "window.h"
#endif

#ifndef LIB_INTERNAL

typedef struct applock 
{
		int			lock;
		int			wait;
		OSHANDLE	hEvent;
} APPLOCK, * PAPPLOCK;

#endif

typedef struct apptimer 
{
		struct apptimer * pNext;
		struct apptimer * pPrev;
		char	state;
		char	flags;
		short	unused;
		int		expire;
		void (* func)(void * para);
		void  * para;
		int		value;
} APPTIMER, * PAPPTIMER;

union u_dword 
{
    unsigned char   b[4];
    unsigned short  w[2];
    unsigned long  d[1];
};

#ifdef _EMULATE_
	extern	void MsgOut (char *format, ...);
#endif

#ifndef TS_REALTIME
#define TS_REALTIME		0x200	/* 实时任务 */
#endif

/* app_event is initialized by APP_CriticalInit */

int  LIB_CreateCritical( PAPPLOCK pLock, char* name );
void LIB_EnterCritical( PAPPLOCK pLock );
void LIB_LeaveCritical( PAPPLOCK pLock );
void LIB_DestroyCritical( PAPPLOCK pLock );

int  LIB_InitTimer( void );
int  LIB_CreateTimer( PAPPTIMER pTimer ); 
int  LIB_StartTimer( PAPPTIMER pTimer );
int  LIB_StopTimer( PAPPTIMER pTimer );
int  LIB_SetTimerFunc( PAPPTIMER pTimer,
					   void (*func)(void * para), void * para );
int  LIB_SetTimerExpire( PAPPTIMER pTimer, int value );
int  LIB_QueryTimerEx( PAPPTIMER pTimer );
int  LIB_DestroyTimer( PAPPTIMER pTimer );

int CheckAndSet_d( int * p );
int CheckAndSet_w( short * p );
int DecAndCheck_w( short * p );
int DecAndCheck_d( int * p );
int IncAndCheck_w( short * p );
int IncAndCheck_d( int * p );

unsigned long LIB_GetTaskID();
#if (_HOPEN_VER >= 200)
int LIB_CreateMutex( MUTEX* Mutex );
int LIB_DestroyMutex( MUTEX* Mutex );
int LIB_WaitMutex( MUTEX* Mutex );
int LIB_SignalMutex( MUTEX* Mutex );
OSHANDLE LIB_CreateEvent( char* name );
int LIB_DestroyEvent( OSHANDLE handle );
int LIB_WaitEvent( OSHANDLE handle );
int LIB_SignalEvent( OSHANDLE handle, unsigned long event );
#else
int LIB_CreateMutex( int count );
int LIB_DestroyMutex( unsigned int handle );
int LIB_WaitMutex( unsigned int handle );
int LIB_SignalMutex( unsigned int handle );
int LIB_CreateEvent( char* name );
int LIB_DestroyEvent( unsigned int handle );
int LIB_WaitEvent( unsigned int handle );
int LIB_SignalEvent( unsigned int handle, unsigned long event );
#endif
int LIB_SetError( int error );
int LIB_GetError();

int LIB_CreateTask(char * pname, unsigned int prio,
		void (*entry)(void *), unsigned long stksize, void * para );
int LIB_GetSysClock();
int LIB_CreateMailbox( void );
int LIB_GetMail( unsigned int handle );
int LIB_SendMail( unsigned int handle, unsigned long mail );
int LIB_GetTicksPerSecond( void );
int LIB_SetTickProc( void (*func)(), int ticks );

unsigned short  get16(unsigned char* cp);
unsigned long  get32(unsigned char* cp);
unsigned char* put16(unsigned char* cp, unsigned short x);
unsigned char* put32(unsigned char* cp, unsigned long x);

char* ti_strdup(const char* sourcestr);

struct ModuleConfig;
struct ConfigMemoryImage;

struct KeywordEntry* FindKeyword( struct ModuleConfig* pSection,
		char* pStr );
struct ModuleConfig* FindSection( struct ConfigMemoryImage * pImage,
		char * pStr );
BOOL IsBroadcastAddr( char* Addr, int AddrLen );

#ifdef _TCPIPMEMDEB_

#if (_HOPEN_VER >= 200)
#define MEM_FIXED				LMEM_FIXED
#define MEM_MOVEABLE			LMEM_MOVEABLE
#define MEM_NOCOMPACT			LMEM_NOCOMPACT
#define MEM_NODISCARD			LMEM_NODISCARD
#endif
extern void* TCPIP_Lock( void* handle );
extern int TCPIP_Unlock( void* handle );
extern int TCPIP_Free( void* handle );
extern void* TCPIP_xMalloc( int flags, unsigned long size, char* file, unsigned long line);
#define TCPIP_Alloc( flags, size )	TCPIP_xMalloc(flags, size, __FILE__, __LINE__ )

#define MAXMEMDEBNUM			500
#define M_GlobalAlloc			TCPIP_Alloc
#define M_GlobalLock			TCPIP_Lock
#define M_GlobalUnlock			TCPIP_Unlock
#define M_GlobalFree			TCPIP_Free

#else

#if (_HOPEN_VER >= 200)
#define MEM_FIXED				LMEM_FIXED
#define MEM_MOVEABLE			LMEM_MOVEABLE
#define MEM_NOCOMPACT			LMEM_NOCOMPACT
#define MEM_NODISCARD			LMEM_NODISCARD

#define M_GlobalAlloc			LocalAlloc
#define M_GlobalLock			LocalLock
#define M_GlobalUnlock			LocalUnlock
#define M_GlobalFree			LocalFree
#else
#define M_GlobalAlloc			OS_GlobalAlloc
#define M_GlobalLock			OS_GlobalLock
#define M_GlobalUnlock			OS_GlobalUnlock
#define M_GlobalFree			OS_GlobalFree
#endif

#endif

#ifdef _PROJECT_AUTMEN_
#define TCPIP_MBUF_SIZE			(50 * 1024)
#define PPP_FRAMEBUF_NUM		70
#define PPP_STREAMBUF_NUM		20
#define PPP_MAX_REQ_TIME		30
#else
#define TCPIP_MBUF_SIZE			(200 * 1024)
#define PPP_FRAMEBUF_NUM		(100)
#define PPP_STREAMBUF_NUM		(20)
#define PPP_MAX_REQ_TIME		10
#endif

/* 硬件相关部分 */
/* 开关硬件中断 */
#ifdef _TCPIP_MOTOLA_

	/*------------- 摩托罗拉-------------- */
	#define DEFINTSTATE         register int i_state = 0
	extern int _CPU_EnterCritical();
	extern void _CPU_LeaveCritical( register );
	#define	ENTER_CRITICAL		i_state = _CPU_EnterCritical()
	#define	LEAVE_CRITICAL		_CPU_LeaveCritical(i_state)

#elif defined _TCPIP_X86_

	/*------------- X86平台 --------------*/
	#define	LITTLE_ENDIAN
	int _fastcall dirps( void );
	void _fastcall restore( int i_state );
	#define DEFINTSTATE         int i_state
	#define	ENTER_CRITICAL		i_state = dirps( )
	#define	LEAVE_CRITICAL		restore( i_state )

#elif defined _TCPIP_SB_

	/*------------- 上海Bell --------------*/
	#define DEFINTSTATE         int i_state
	#define	ENTER_CRITICAL		i_state = int_dirps( )
	#define	LEAVE_CRITICAL		int_restore( i_state )
        
#elif defined  _EMULATE_

	/*------------- 仿真环境 --------------*/
#ifdef _JINHUANTIANLANG_
	#define DEFINTSTATE         register int i_state = 0
/*	extern void _CPU_EnterCritical();
	extern void _CPU_LeaveCritical( );
	#define	ENTER_CRITICAL		_CPU_EnterCritical(); i_state = 0
	#define	LEAVE_CRITICAL		_CPU_LeaveCritical()*/
	#define	ENTER_CRITICAL		i_state = 1
	#define	LEAVE_CRITICAL		i_state = 0
#else
	#define DEFINTSTATE         register int i_state = 0
	extern void _CPU_EnterCritical();
	extern void _CPU_LeaveCritical( );
	#define	ENTER_CRITICAL		_CPU_EnterCritical(); i_state = 0
	#define	LEAVE_CRITICAL		_CPU_LeaveCritical()
#endif
	
#elif defined _JINHUANTIANLANG_
	#define DEFINTSTATE         register int i_state = 0
/*	extern void _CPU_EnterCritical();
	extern void _CPU_LeaveCritical( );
	#define	ENTER_CRITICAL		_CPU_EnterCritical(); i_state = 0
	#define	LEAVE_CRITICAL		_CPU_LeaveCritical()*/
	#define	ENTER_CRITICAL		i_state = 1
	#define	LEAVE_CRITICAL		i_state = 0

#elif defined _ARM_

	/*------------- ARM -------------------*/
	#define	LITTLE_ENDIAN
	#define DEFINTSTATE         register int i_state = 0
	#define	ENTER_CRITICAL		i_state = _CPU_dirps()
	#define	LEAVE_CRITICAL		_CPU_restore(i_state)

#elif defined 	_SP_EPSON_	// for legend mobile phone

	/*------------ 	_SP_EPSON_ -------------*/
	#define DEFINTSTATE         register int i_state = 0
	#define	ENTER_CRITICAL		i_state = _CPU_dirps()
	#define	LEAVE_CRITICAL		_CPU_restore(i_state)
	#define TCPIP_SINGELMODUL
	#define _TCPIP_NOUI_
	#define	LITTLE_ENDIAN
	
#else  

	/*-------------  WINBOND --------------*/
	#define DEFINTSTATE         register int i_state = 0
	#define	ENTER_CRITICAL		i_state = _dirps()
	#define	LEAVE_CRITICAL		_restore(i_state)

#endif

/****************定义一批初始化错误码*********************/
#define TIINIT_SOURCELOW		-10		//资源不够
#define TIINIT_OVERFLOW			-11		//溢出
#define TIINIT_KERNELERROR		-12		//核心协作出现问题
#define TIINIT_WNDERROR			-14		//窗口协作出现问题
#define TIINIT_UIERROR			-15		//UI协作出现问题
#define TIINIT_REGFAIL			-16		//注册失败		


/* 修改所有模块的诊断数据块 */
void LIB_ChangeReprotUnit( void* pReport, char index, int num, char method );
/***************本函数现在有BUG，关闭********************/
void TCPIP_ErrNote( int Proto, unsigned char* pNote, int notelen, const char* pDis );
/* TCPIP lib 全局控制块，用来模拟一些核心动作 */
typedef struct tagTILib_GlobalBlock
{
	int ifInit;
#if (_HOPEN_VER >= 200)
	MUTEX tempTILibMUTEX;
	MUTEX* TILibMUTEX;
	MUTEX tempReMutex;
	MUTEX* ReMutex;
	MUTEX tempCSMutex;
	MUTEX* CSMutex;
	MUTEX tempLogMutex;
	MUTEX* LOGMutex;
#else
	int TILibMUTEX;
	int ReMutex;
	int CSMutex;
	int LOGMutex;
#endif
	long Mailflag;
	OSHANDLE MAILEVENT;
	unsigned char* pEHead;
	unsigned char* pETail;
	unsigned char* pError;
}TILib_GlobalBlock;


extern TILib_GlobalBlock TILib_GB;


/*****************************************************\
			无UI状态下我们编制虚UI结束
\*****************************************************/

/********************************************************************\
			无窗口状态下我们编制模拟窗口，异步SOCKET功能放弃
\********************************************************************/
#ifdef _TCPIP_NOWIN_
#define MB_OK		0
#endif

BOOL LIB_PostMessage( void* hWnd, unsigned int wMsgCmd, unsigned long wParam, 
					 unsigned long lParam );
BOOL LIB_RegisterClass( void(*pFunc)(void* , unsigned int, long, long), char* pName );
void* LIB_CreateWindow( char* pClassName );
BOOL LIB_DestroyWindow( void* hWnd );
unsigned int LIB_SetTimer(void* hWnd, unsigned int nIDEvent, unsigned int uElapse, 
                        void(* pTimerFunc)(void* , unsigned int, unsigned int, unsigned long));
BOOL LIB_KillTimer(void* hWnd, unsigned int uIDEvent);
int LIB_DefWindowProc(void* hWnd, unsigned int wMsgCmd, unsigned long wParam, 
                                 unsigned long lParam);
int LIB_MessageBox( void* hWnd, char* pszText, char* pszCaption, unsigned int uType);

/* 数据输出函数 */
void TCPIP_DataOut(const unsigned char *fragment, unsigned short length,
    	    const unsigned char *name);

/********************************************************************\
			无窗口状态下我们编制模拟窗口结束
\********************************************************************/


#ifdef _TCPIPASSERT_
#include "hp_diag.h"
#define TCPIP_ASSERT	ASSERT
#else
#define TCPIP_ASSERT( x )	
#endif


#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    sysdep.h
Abstract:
	This file includes the definition of system dependent functions.
	Definition for different Operating system:
		CONFIG_OS_WIN32: For Windows 98/2000
		CONFIG_OS_LINUX: For Linux
		CONFIG_OS_WINCE: For Windows CE 2.12
		CONFIG_OS_WINKERNEL: For Windows Kernel module which is developed using VTOOLSD
		CONFIG_OS_REX:   For Rex
		CONFIG_OS_PSOS:	 For PSOS+
		CONFIG_OS_VXWORK:  For Vxwork 5.3 and above
		CONFIG_OS_NUCLEUS:	 For Nucleus
		CONFIG_OS_UC:	 For uC/OS-II
		CONFIG_OS_NONE:	 For no opeating system.

	Definition for other options:
		CONFIG_MEMORY_STATIC: use static memory allocation.
		CONFIG_ALIGN_ONEBYTE: one byte alignment for data structure.
		CONFIG_ALIGN_TWOBYTES: two byte alignment for data structure.
		CONFIG_LITTLE_ENDIAN: Using little endian encoding for integer.
Author:
    Gang He
Revision History:
	2000.9
	2002.1	Change the module to different library.
---------------------------------------------------------------------------*/
#ifndef SYSTEMDEP_H
#define SYSTEMDEP_H

#include "autoconf.h"
#include "stdio.h"
#include "string.h"
#include "stdarg.h"

#include "datatype.h"
#include "bterrno.h"
#ifdef  CONFIG_MEMORY_STATIC
#include "btmem.h"
#endif

typedef void(THREAD_FUNC)(void);
typedef void(THREAD_FUNC2)(void*);



/* System dependent functions */
SYSTEM_LOCK CREATE_CRITICAL(LPCSTR name);
void ENTER_CRITICAL(SYSTEM_LOCK mylock);
void LEAVE_CRITICAL(SYSTEM_LOCK mylock);
void FREE_CRITICAL(SYSTEM_LOCK mylock);
void WAITFOR_SINGLEOBJECT(HANDLE myobj);
void WAITFOR_SINGLEOBJECT_TIME(HANDLE myobj, DWORD milliseconds);
void BTCLOSE_HANDLE(HANDLE myobj);
void SET_EVENT(HANDLE myobj);
HANDLE CREATE_EVENT(LPCSTR name);
void CLOSE_EVENT(HANDLE myev);
void CREATE_THREAD(THREAD_FUNC * p);
void CREATE_THREAD2(THREAD_FUNC2 * p, void* param);
void CREATE_THREADEX(
	UCHAR *name,
	THREAD_FUNC2 * p, 		/* Start address */
	WORD		stksize,	/* Stack size */
	UCHAR*		stk,		/* Stack */
	WORD		pri,		/* Priority */
	void* param				/* Parameter */
);

void SLEEP(DWORD milli_sec);
DWORD GetCurrTime(void);

#ifdef  CONFIG_MEMORY_LEAK_DETECT
#define NEW(A)  DBG_NEW((int)A,__FILE__,__LINE__)
#define FREE(A)  DBG_FREE((void*)A,__FILE__,__LINE__)
void * DBG_NEW (int size, char* filename, int line);
void DBG_FREE (void * buf, char* filename, int line);
SYSTEM_LOCK DBG_CREATE_CRITICAL(char *a, char* filename, int line);
#else
#ifdef CONFIG_MEMORY_STATIC
#define NEW mem_malloc
#define FREE mem_free
void * mem_malloc (int size);
void mem_free(void * buf);
#else
void * NEW(int size);
void FREE(void * buf);
#endif
#endif
UCHAR CheckPool(int size);

#ifdef CONFIG_DEBUG
int DBG_PRINT( const char *format, ...);
#endif


/*  System dependent timer functions */
struct FsmTimer;
struct BuffStru;

int InitTimer(struct FsmTimer *ft);
int AddTimer(struct FsmTimer *ft);
int DelTimer(struct FsmTimer *ft);

/*
 * Codeing and decoding functions
*/
#define STRU_NOTRANS	 (0x80000000)
#define STRU_TRANS_MULTI (0x40000000)

WORD EncodeNew(UCHAR * dest, UCHAR * src, DWORD mask,UCHAR reverse);
WORD DecodeNew(UCHAR * dest, UCHAR * src, DWORD mask,UCHAR reverse);
#define Encode(a,b,c) EncodeNew(a,b,c,0)
#define Decode(a,b,c) DecodeNew(a,b,c,0)
UCHAR StruLen( DWORD mask );

WORD MyEncode( UCHAR * buf, char * fmt, ...);

/*
 * Unicode and ASCII transaction
*/
/* using CP_ACP code page */
WORD Get_ACP(void);
void Set_ACP(WORD acp);
int WideToByte(char* lpw,char* lpa,int len);
int ByteToWide(char* lpa,char* lpw,int len);
/* using CP_UTF8 code page */
int WideToUTF8(char* lpw,char* lpa,int len);
int UTF8ToWide(char* lpa,char* lpw,int len);

extern DWORD g_error;
#define GET_HCI_ERROR(a)		(a&0xff)
#define GET_STACK_ERROR(a)		((a&0xff00)>>8)
#define GET_PROFILE_ERROR(a)	((a&0xff0000)>>16)
#define GET_APP_ERROR(a)		((a&0xff000000)>>24)
DWORD BtGetLastError(void);

#define SET_HCI_ERROR(a)		g_error=(g_error&0xffffff00)|(a&0xff)
#define SET_STACK_ERROR(a)		g_error=(g_error&0xffff00ff)|((a&0xff)<<8)
#define SET_PROFILE_ERROR(a)	g_error=(g_error&0xff00ffff)|((a&0xff)<<16)
#define SET_APP_ERROR(a)		g_error=(g_error&0x00ffffff)|((a&0xff)<<24)
#define BtSetLastError(a)		g_error=a;

#ifndef FUNC_EXPORT 
#define FUNC_EXPORT 
#endif

#ifdef CONFIG_CHECK_STACK
#ifdef CONFIG_OS_WIN32
void STK_StartChk(UCHAR f,const char *func);
WORD STK_EnterFunc(const char *func);
void STK_LeaveFunc(const char *func);
void STK_StopChk(UCHAR f);
#else
#define STK_StartChk(f,d)
#define STK_EnterFunc(f)
#define STK_LeaveFunc(f)
#define STK_StopChk(f)
#endif
#else
#define STK_StartChk(f,d)
#define STK_EnterFunc(f)
#define STK_LeaveFunc(f)
#define STK_StopChk(f)
#endif

#ifdef  CONFIG_MEMORY_LEAK_DETECT
#ifdef CONFIG_OS_WIN32
void SetMemLeakFlag(void);
void MapFilenameToProt(char *fn,char *prot);
void RegProtForStat(char *affix);
void RegStackProtForStat(void);
void EndMemDetect();
#else
#define SetMemLeakFlag()
#define MapFilenameToProt(fn,prot) 
#define RegProtForStat(af)
#define RegStackProtForStat()
#define EndMemDetect()
#endif
#else
#define SetMemLeakFlag()
#define MapFilenameToProt(fn,prot) 
#define RegProtForStat(af)
#define RegStackProtForStat()
#define EndMemDetect()
#endif

#endif 

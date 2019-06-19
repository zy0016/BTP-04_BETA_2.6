/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : The global header file for PLX window system, must be 
 *            include in each C file of PLX window system.
 *            
\**************************************************************************/

#ifndef __HPWIN_H
#define __HPWIN_H

// For Microsoft Visual C++ redefine the compiler warning options
#ifdef _MSC_VER
#include "warning.h"
#endif /* _MSC_VER */

#include "osver.h"          /* PLXOS version definition       */
#include "wconfig.h"    // PLX窗口系统配置文件
#include "window.h"     // PLX窗口系统接口文件
#include "windowx.h"     // PLX窗口系统接口文件

// PLXOS assert macro definition
#if (_HOPEN_VER >= 300)
#include "assert.h"
#else
#include "hp_diag.h"
#endif  // _HOPEN_VER >= 300

// 标志窗口系统是否初始化的全局变量
#ifdef WINDEBUG
extern BOOL g_bInit;
#endif

extern  int         SysLangue;
extern  HINSTANCE   hInsMsgBox;

#define LANGUE_ENGLISH  0
#define LANGUE_CHINESE  1

/* 定义使用内部DefWindowProc的类属性，用于内部控件 */
#define CS_DEFWIN	0x8000

#if (_HOPEN_VER >= 300) || defined(_LINUX_OS_)
#include "stdlib.h"

typedef void * HGLOBAL;
typedef	void * HLOCAL;

#define GlobalAlloc(flag, size)     malloc(size)
#define GlobalFree(handle)          free(handle)
#define GlobalLock(handle)          (handle)
#define GlobalUnlock(handle)

#define LocalAlloc(flag, size)      malloc(size)
#define LocalFree(handle)           free(handle)
#define LocalRealloc(p, newsize)    realloc(p, newsize)
#define LocalLock(handle)           (handle)
#define LocalUnlock(handle)

#else   // _HOPEN_VER < 300
#include "hp_kern.h"        /* PLXOS kernel API prototypes    */
#endif  // _HOPEN_VER


// PLX窗口系统内部使用的内存分配和释放函数
#if (__MP_PLX_GUI)
#ifdef _EMULATE_
#include "importms.h"
#define WIN_LocalAlloc      EMU_LocalAlloc
#define WIN_LocalFree       EMU_LocalFree
#define WIN_LocalLock       EMU_LocalLock
#define WIN_LocalUnlock     EMU_LocalUnlock
#else
#define WIN_LocalAlloc      LocalAlloc
#define WIN_LocalFree       LocalFree
#define WIN_LocalLock       LocalLock
#define WIN_LocalUnlock     LocalUnlock
#endif //_EMULATE_
#else
#define WIN_LocalAlloc      LocalAlloc
#define WIN_LocalFree       LocalFree
#define WIN_LocalLock       LocalLock
#define WIN_LocalUnlock     LocalUnlock
#endif //__MP_PLX_GUI


#if (!USE_WINDOW_MEMORY)
#define MemAlloc(dwSize)    ((void*)WIN_LocalAlloc(LMEM_FIXED, dwSize))
#define MemFree(pMem)       WIN_LocalFree((HLOCAL)(pMem))
#define MemSet(pMem, value, size)   memset(pMem, value, size)
#else
#include "stdio.h"
#include "winmem.h"
#define MemAlloc(dwSize)    ((void*)WinAlloc(dwSize))
#define MemFree(pMem)       WinFree((HLOCAL)(pMem))
#define MemSet(pMem, value, size)   memset(pMem, value, size)
#endif

#if defined(_LINUX_OS_)
/*
static int stricmp(const char* s1, const char* s2)
{
while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2)
{
s1++;
s2++;
}
return (*s1) - (*s2);
}

static int strnicmp(const char* s1, const char* s2, int count)
{
while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2 && count--)
{
s1++;
s2++;
}

if (count == 0)
return 0;

return (*s1) - (*s2);
}
*/
#include "string.h"
#define stricmp         strcasecmp
#define strnicmp        strncasecmp
#endif

#define WIN_PRINT(format)

#ifdef TRACE
#undef TRACE
#undef TRACEERROR
#define TRACE printf
#define TRACEERROR() TRACE("Error: errno=%d occured!!\r\n", errno)
#endif

#endif // __HPWIN_H

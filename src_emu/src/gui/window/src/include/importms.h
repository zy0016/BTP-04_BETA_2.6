/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#ifndef _IMPORTDLL_H

#define _IMPORTDLL_H

#ifndef INFINITE
#define INFINITE 0xffffffff
#endif

#ifndef WINDLLAPI
#define WINDLLAPI   _declspec(dllimport) 
#endif

#define LMEM_FIXED          0x0000
#define LMEM_MOVEABLE       0x0002
#define LMEM_NOCOMPACT      0x0010
#define LMEM_NODISCARD      0x0020
#define LMEM_ZEROINIT       0x0040
#define LMEM_MODIFY         0x0080
#define LMEM_DISCARDABLE    0x0F00
#define LMEM_VALID_FLAGS    0x0F72
#define LMEM_INVALID_HANDLE 0x8000

#ifdef __cplusplus
extern "C" {
#endif

WINDLLAPI HANDLE    EMU_CreateSemaphore(char* name, int count);
WINDLLAPI BOOL      EMU_DestroySemaphore(HANDLE handle);
WINDLLAPI HANDLE    EMU_OpenSemaphore(char* name);
WINDLLAPI int       EMU_WaitSemaphore(HANDLE handle, int timeout);
WINDLLAPI int       EMU_ReleaseSemaphore(HANDLE handle, int count);

WINDLLAPI HANDLE    EMU_CreateMutex(void);
WINDLLAPI BOOL      EMU_DestroyMutex(HANDLE handle);
WINDLLAPI int       EMU_WaitMutex(HANDLE handle);
WINDLLAPI int       EMU_ReleaseMutex(HANDLE handle);

WINDLLAPI DWORD     EMU_GetCurrentProcessId(void);
WINDLLAPI DWORD     EMU_GetCurrentThreadId(void);

WINDLLAPI HLOCAL    EMU_LocalAlloc(int flag, size_t size);
WINDLLAPI void      EMU_LocalFree(HLOCAL hMem);
WINDLLAPI void*     EMU_LocalLock(HLOCAL hMem);
WINDLLAPI BOOL      EMU_LocalUnlock(HLOCAL hMem);
WINDLLAPI void*     EMU_malloc(size_t size);
WINDLLAPI void      EMU_free(void* p);

#ifdef __cplusplus
};
#endif

#endif //_IMPORTDLL_H

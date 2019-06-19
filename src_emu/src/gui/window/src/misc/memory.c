/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements local heap management functions : 
 *            MemAlloc
 *            MemFree
 *            
\**************************************************************************/

#define _OLDMEM             // Use the old memory api interface
#define USE_USER_MEMORY     // Use user memory

#include "hpwin.h"

#ifndef NOMEMMGR

#if (_HOPEN_VER < 200) && !defined(_LINUX_OS_)

#include "string.h"

/*
**  Function : GlobalAlloc
**  Purpose  :
**      Allocates the specified number of bytes from the heap. 
**  Params   :
**      uFlags  : Specifies how to allocate memory. If zero is specified, 
**                the default is GMEM_FIXED. 
**      dwBytes : Specifies the number of bytes to allocate. If this 
**                parameter is zero and the uFlags parameter specifies the 
**                GMEM_MOVEABLE flag, the function returns a handle to a 
**                memory object that is marked as discarded. 
**  Return   :
**      If the function succeeds, return the handle of the newly allocated 
**      memory object. If the function fails, return NULL. To get extended
**      error information, call GetLastError. 
*/
HGLOBAL WINAPI GlobalAlloc(UINT uFlags, DWORD dwBytes)
{
    return (HGLOBAL)OS_GlobalAlloc((int)uFlags, (int)dwBytes);
}

/*
**  Function : GlobalFree
**  Purpose  :
**      Frees the specified global memory object and invalidates its handle. 
**  Params   :
**      hMem : Identifies the global memory object. This handle is returned
**      by the GlobalAlloc function. 
**  Return   :
**      If the function succeeds, return NULL. 
**      If the function fails, return the handle of the global memory object.
*/
HGLOBAL WINAPI GlobalFree(HGLOBAL hMem)
{
    int nRet;

    if (!hMem)
        return NULL;

    nRet = (int)OS_GlobalFree((void*)hMem);

    // Call OS_GlobalFree failed, return the memroy handle to be free.
    if (nRet == -1)
    {
        SetLastError(1);
        return hMem;
    }

    // Call OS_GlobalFree function succeeds, return NULL
    return NULL;
}

/*
**  Function : GlobalLock
**  Purpose  :
**      Locks a global memory object and returns a pointer to the first 
**      byte of the object¡¯s memory block. The memory block associated 
**      with a locked memory object cannot be moved or discarded. 
**  Params   :
**      hMem : Identifies the global memory object. This handle is returned
**             by the GlobalAlloc function. 
**  Return   :
**      If the function succeeds, return a pointer to the first byte of the
**      memory block. If the function fails, return NULL.
*/
PVOID WINAPI GlobalLock(HGLOBAL hMem)
{
    return OS_GlobalLock((int)hMem);
}

/*
**  Function : GlobalUnlock
**  Purpose  :
**      Decrements the lock count associated with a memory object that was
**      allocated with the GMEM_MOVEABLE flag. This function has no effect
**      on memory objects allocated with the GMEM_FIXED flag. 
**  Params   :
**      hMem : Identifies the global memory object. This handle is returned
**             by the GlobalAlloc function. 
*/
BOOL WINAPI GlobalUnlock(HGLOBAL hMem)
{
    int nRet;

    if ((nRet = OS_GlobalUnlock((int)hMem)) == -1)
        return FALSE;

    return TRUE;
}

/*
**  Function : LocalAlloc
**  Purpose  :
**      Allocates the specified number of bytes from the heap. 
**  Params   :
**      uFlags  : Specifies how to allocate memory. If zero is specified, 
**                the default is LMEM_FIXED. 
**      dwBytes : Specifies the number of bytes to allocate. If this 
**                parameter is zero and the uFlags parameter specifies the 
**                GMEM_MOVEABLE flag, the function returns a handle to a 
**                memory object that is marked as discarded. 
**  Return   :
**      If the function succeeds, return the handle of the newly allocated 
**      memory object. If the function fails, return NULL. To get extended
**      error information, call GetLastError. 
*/
HLOCAL WINAPI LocalAlloc(UINT uFlags, UINT uBytes)
{
    return (HLOCAL)OS_GlobalAlloc((int)uFlags, (int)uBytes);
}

/*
**  Function : LocalFree
**  Purpose  :
**      Frees the specified local memory object and invalidates its handle. 
**  Params   :
**      hMem : Identifies the local memory object. This handle is returned 
**             by either the LocalAlloc function. 
**  Return   :
**      If the function succeeds, return NULL. 
**      If the function fails, return the handle of the local memory object.
*/
HLOCAL WINAPI LocalFree(HLOCAL hMem)
{
    int nRet;

    if (!hMem)
        return NULL;

    nRet = OS_GlobalFree((void*)hMem);

    // Call OS_GlobalFree failed, return the memroy handle to be free.
    if (nRet == -1)
    {
        SetLastError(1);
        return hMem;
    }

    // Call OS_GlobalFree function succeeds, return NULL
    return NULL;
}

/*
**  Function : LocalLock
**  Purpose  :
**      Locks a local memory object and returns a pointer to the first 
**      byte of the object¡¯s memory block. The memory block associated 
**      with a locked memory object cannot be moved or discarded. For 
**      memory objects allocated with the LMEM_MOVEABLE flag, the 
**      function increments the object¡¯s lock count. 
**  Params   :
**      hMem : Identifies the local memory object. This handle is returned
**             by either the LocalAlloc function. 
**  Return   :
**      If the function succeeds, return a pointer to the first byte of the
**      memory block. 
**      If the function fails, return NULL. To get extended error 
**      information, call GetLastError. 
*/
PVOID WINAPI LocalLock(HLOCAL hMem)
{
    if (!hMem)
        return NULL;

    return OS_GlobalLock((int)hMem);
}

/*
**  Function : LocalUnlock
**  Purpose  :
**      Decrements the lock count associated with a memory object that was
**      allocated with the LMEM_MOVEABLE flag. This function has no effect
**      on memory objects allocated with the LMEM_FIXED flag. 
**  Params   :
**      hMem : Identifies the local memory object. This handle is returned
**             by either the LocalAlloc function. 
**  Return   :
**      If the memory object is still locked after decrementing the lock 
**      count, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError.
*/
BOOL WINAPI LocalUnlock(HLOCAL hMem)
{
    int nRet;

    if ((nRet = OS_GlobalUnlock((int)hMem)) == -1)
        return FALSE;

    return TRUE;
}

#endif  /* #if _HOPEN_VER < 200 */

#endif // NOMEMMGR

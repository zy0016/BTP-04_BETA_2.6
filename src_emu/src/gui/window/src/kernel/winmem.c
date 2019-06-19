/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Manage the memory used by window system.
 *            In multi process, the memory is shared by all process.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "wsthread.h"
#include "winmem.h"
#include "string.h"

#if (__MP_PLX_GUI)
#ifdef _EMULATE_
#define SEMNAME_MEMORY      "$$PLXGUI_MEMORY$$"
#define WINMEM_CREATEOBJ(sem)        CREATEOBJ(sem, SEMNAME_MEMORY)
#define WINMEM_DESTROYOBJ(sem)       DESTROYOBJ(sem, SEMNAME_MEMORY)
#define WINMEM_WAITOBJ(sem)          WAITOBJ(sem, SEMNAME_MEMORY)
#define WINMEM_RELEASEOBJ(sem)       RELEASEOBJ(sem, SEMNAME_MEMORY)
#else
#define WINMEM_CREATEOBJ(sem)        CREATEOBJ(sem)
#define WINMEM_DESTROYOBJ(sem)       DESTROYOBJ(sem)
#define WINMEM_WAITOBJ(sem)          WAITOBJ(sem)
#define WINMEM_RELEASEOBJ(sem)       RELEASEOBJ(sem)
#endif
#else
#define WINMEM_CREATEOBJ(sem)        CREATEOBJ(sem)
#define WINMEM_DESTROYOBJ(sem)       DESTROYOBJ(sem)
#define WINMEM_WAITOBJ(sem)          WAITOBJ(sem)
#define WINMEM_RELEASEOBJ(sem)       RELEASEOBJ(sem)
#endif

typedef struct  _FREEMEMLIST
{
    struct  _FREEMEMLIST    *pNext;
    int             size;   // upper 8 bits may be used for extended flag
    int             flag;
}FREEMEMLIST;

#if (USE_WINDOW_MEMORY)

static COMMONMUTUALOBJ sem_winmem; 

unsigned char *MEMStart;
#define MYMEM_MAGIC             //key of shared memory
#define SYSTEMMEMRES_SIZE       0x00080000  // now use 2M memory pool
#define SYSTEMMEMRES_ADDR       MEMStart //0x7f000000
#define SYSTEMMEMRES_ENDADDR    (SYSTEMMEMRES_ADDR+SYSTEMMEMRES_SIZE)

#define MEM_FRAGMENT    16

FREEMEMLIST     *FreeMemPool;
void*   pWinMem;

static int TotalFreeSize, TotalUsedSize;
#endif //USE_WINDOW_MEMORY
/*********************************************************************\
* Function     WINMEM_Init
* Purpose      Initial the window system memory.Shared by all thread.
* Params       
* Return           
* Remarks      
**********************************************************************/
BOOL WINMEM_Init(int type)
{
#if (!USE_WINDOW_MEMORY)
    return TRUE;
#else //USE_WINDOW_MEMORY
#if (__MP_PLX_GUI)
    /* 多进程GUI，窗口共享内存 */
    static BYTE WinMemory[SYSTEMMEMRES_SIZE];
    pWinMem = WinMemory;
#else
    pWinMem = (void*)LocalAlloc(LMEM_FIXED, SYSTEMMEMRES_SIZE);
#endif
    if (pWinMem == NULL)
        return FALSE;

    WINMEM_CREATEOBJ(sem_winmem);
    SYSTEMMEMRES_ADDR = (unsigned char *)pWinMem;
    FreeMemPool = (FREEMEMLIST*)pWinMem;
    FreeMemPool->pNext = NULL;
    FreeMemPool->size = SYSTEMMEMRES_SIZE;
    FreeMemPool->flag = 0;
    
    TotalFreeSize = SYSTEMMEMRES_SIZE;
    TotalUsedSize = 0;
    return TRUE;
#endif //USE_WINDOW_MEMORY
}

/*********************************************************************\
* Function     WINMEM_Exit
* Purpose      
* Params       
* Return           
* Remarks      
**********************************************************************/
void WINMEM_Exit(void)
{
#if (USE_WINDOW_MEMORY)
    LocalFree((HLOCAL)pWinMem);
    WINMEM_DESTROYOBJ(sem_winmem);
#endif
}

#if (USE_WINDOW_MEMORY)
/*********************************************************************\
* Function     WinAlloc
* Purpose      Alloc window system memory and used only in window internal.
* Params       
* Return           
* Remarks      
**********************************************************************/
void* WinAlloc(int size)
{
    FREEMEMLIST *pCur, *pPrev, *pNew;

    WINMEM_WAITOBJ(sem_winmem);
    if (FreeMemPool == NULL)
    {
        WINMEM_RELEASEOBJ(sem_winmem);
        WIN_PRINT("No Memory!\n");
        return NULL;
    }

    size = (size + sizeof(FREEMEMLIST) + 3) & 0xFFFFFFFC;

    if (size < MEM_FRAGMENT)
        size = MEM_FRAGMENT;

    pPrev = pCur = FreeMemPool;
    /* 头节点处理 */
    if (pCur->size >= size)
    {
        if (pCur->size <= size + MEM_FRAGMENT)
        {
            size = pCur->size;
            FreeMemPool = pCur->pNext;
            /* 调试中窗口内存被分配光，需要调节窗口内存总量 */
            ASSERT(FreeMemPool != NULL);
        }
        else
        {
            pNew = (FREEMEMLIST*)((char*)pCur + size);
            pNew->size = pCur->size - size;
            pNew->pNext = pCur->pNext;
            FreeMemPool = pNew;

            pCur->size = size;
        }
    }
    else
    {
        while (pCur != NULL && pCur->size < size)
        {
            pPrev = pCur;
            pCur = pCur->pNext;
        }
        
        if (pCur == NULL)
        {
            WINMEM_RELEASEOBJ(sem_winmem);
            return NULL;
        }
        
        if (pCur->size <= size + MEM_FRAGMENT)
        {
            pPrev->pNext = pCur->pNext;
        }
        else
        {
            pNew = (FREEMEMLIST*)((char*)pCur + size);
            pNew->size = pCur->size - size;
            pNew->pNext = pCur->pNext;
            pPrev->pNext = pNew;
            
            pCur->size = size;
        }
    }

    TotalUsedSize += size;
    TotalFreeSize -= size;

    WINMEM_RELEASEOBJ(sem_winmem);
    return (char*)pCur + sizeof(FREEMEMLIST);
}

/*********************************************************************\
* Function     WinFree
* Purpose      Free the window system memory.
* Params       
* Return           
* Remarks      
**********************************************************************/
void WinFree(void* pMem)
{
    FREEMEMLIST *p1, *p2, *p3;

    if (((unsigned char*)pMem < SYSTEMMEMRES_ADDR) || 
        ((unsigned char*)pMem >= SYSTEMMEMRES_ENDADDR))
        return;

    WINMEM_WAITOBJ(sem_winmem);

    p3 = (FREEMEMLIST *)((char*)pMem - sizeof(FREEMEMLIST));

    TotalUsedSize -= p3->size;
    TotalFreeSize += p3->size;
    if (FreeMemPool == NULL)
    {
        p3->pNext = NULL;
        FreeMemPool = p3;

        WINMEM_RELEASEOBJ(sem_winmem);

        return;
    }

    p1 = p2 = FreeMemPool;


    /* 头节点插入 */
    if (p3 < FreeMemPool)
    {
        p3->pNext = p2;
        FreeMemPool = p3;
        p1 = p3;
    }
    else
    {
        while (p2 != NULL && p2 < p3)
        {
            p1 = p2;
            p2 = p2->pNext;
        }
        
        ASSERT(p2 != NULL);

        /* p3加到p1和p2之间 */
        p3->pNext = p2;
        p1->pNext = p3;
    }

    if ((unsigned int)p3 + p3->size == (unsigned int)p2)
    {
        p3->pNext = p2->pNext;
        p3->size += p2->size;
    }

    if ((unsigned int)p1 + p1->size == (unsigned int)p3)
    {
        p1->pNext = p3->pNext;
        p1->size += p3->size;
    }

    WINMEM_RELEASEOBJ(sem_winmem);

    return;
}

#endif //USE_WINDOW_MEMORY

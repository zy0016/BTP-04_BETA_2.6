/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements DC management fuctions
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"

#include "wsobj.h"
#include "wsodc.h"
#include "wsownd.h"
#include "wsthread.h"
/* 直接使用图形设备驱动，不再与显示或打印相关,但需要统一接口，目前暂时不改*/
#include "graphdev.h"
#include "devdisp.h"

#if (PRINTERSUPPORT)
#include "devprn.h"
#endif // PRINTERSUPPORT

#define DC_CACHE_SIZE  5                   // 普通窗口DC Cache的大小
static PDC WindowDCCache;

// Internal functin prototypes
static void SetDCItemsToDefault(PDC pDC);

/*
**  Function : DC_GetWindowDCFromCache
**  Purpose  :
**      Gets a general window dc from cache.
**  Params   :
**      pWin : Pointer to a specified window.
**  Return   :
**      If the function succeeds, return the pointer to the DC.
**      If the function fails, return NULL.
*/
BOOL DC_Init(void)
{
    int i;
    PDC pDC;

    WindowDCCache = MemAlloc(sizeof(DC) * DC_CACHE_SIZE);
    if (WindowDCCache == NULL)
    {
        SetLastError(1);
        return FALSE;
    }

    MemSet(WindowDCCache, 0, sizeof(DC) * DC_CACHE_SIZE);

    for (i = 0; i < DC_CACHE_SIZE; i++)
    {
        pDC = &WindowDCCache[i];
        // Adds DC to object table
        if (!WOT_RegisterObj((PWSOBJ)pDC, OBJ_DC, FALSE))
        {
            ASSERT(0);
        }

        pDC->bType   = DC_WINDOW;
        pDC->bDev    = DEV_DISPLAY;
        pDC->pDev    = NULL;
        pDC->wAttrib = WDA_CLIENT;
        pDC->pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);

        SetDCItemsToDefault(pDC);
#if (__MP_PLX_GUI)
        WOT_SetObjProcessId(pDC, INVALID_PROCESSID);
#endif
    }

    return TRUE;
}

/*
**  Function : DC_GetWindowDCFromCache
**  Purpose  :
**      Gets a general window dc from cache.
**  Params   :
**      pWin : Pointer to a specified window.
**  Return   :
**      If the function succeeds, return the pointer to the DC.
**      If the function fails, return NULL.
*/
PDC DC_GetWindowDCFromCache(PWINOBJ pWin, BOOL bClient, BOOL bUpdate)
{
    int i;
    PDC pDC;

    ASSERT(pWin != NULL);

    // 在Cache中寻找未用的Window DC
    for (i = 0; i < DC_CACHE_SIZE; i++)
    {
        pDC = &WindowDCCache[i];
        if (pDC->pDev == NULL)
        {
            pDC->pDev = pWin;
            pDC->wAttrib = 0;
            if (bClient)
                pDC->wAttrib |= WDA_CLIENT;
            if (bUpdate)
                pDC->wAttrib |= WDA_UPDATE;

            pDC->pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
#if (__MP_PLX_GUI)
            /* 多进程版本，CacheDC对象需要重设进程号*/
            WOT_SetObjProcessId(pDC, WS_GetCurrentProcessId());
#else
            SetDCItemsToDefault(pDC);
#endif
            return pDC;
        }
    }

    /* 在调试版中，给出提示，DC已经用光了，可能是应用没有释放或嵌套太多 
    ** 通常则提供DC的动态分配，最后释放。释放时只要不是CacheDC就释放内存。
    */
//    ASSERT(0);

    pDC = DC_CreateWindowDC(NULL, TRUE);
    if (pDC == NULL)
    {
        SetLastError(1);
        return NULL;
    }

    pDC->pDev = pWin;
    pDC->wAttrib = 0;
    if (bClient)
        pDC->wAttrib |= WDA_CLIENT;
    if (bUpdate)
        pDC->wAttrib |= WDA_UPDATE;

    pDC->pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
#if (!__MP_PLX_GUI)
    SetDCItemsToDefault(pDC);
#endif

    return pDC;
}

/*
**  Fucntion : DC_ReleaseWindowDCToCache
**  Purpose  :
**      Release a specified window DC to the window dc cache.
*/
void DC_ReleaseWindowDCToCache(PDC pDC)
{
    PDC pCacheDC;
    int i;

    ASSERT(pDC != NULL && pDC->pDev != NULL);

#if (__MP_PLX_GUI)
    /* 应该在释放时将pDC设置为缺省值，这样在多进程版本中才能保证下次被其他
    ** 线程使用
    */
    SetDCItemsToDefault(pDC);
#endif

    // 在Cache中寻找指定的Window DC
    for (i = 0; i < DC_CACHE_SIZE; i++)
    {
        pCacheDC = &WindowDCCache[i];
        if (pCacheDC == pDC)
            break;
    }

    if (pCacheDC == pDC)
    {
        pDC->pDev = NULL;
#if (__MP_PLX_GUI)
        /* 多进程版本，CacheDC对象需要重设进程号*/
        WOT_SetObjProcessId(pDC, INVALID_PROCESSID);
#endif
        return ;
    }

//    ASSERT(0);
    /*Cache中没有，必然是新创建的，需要删除 */
    DC_Destroy(pDC);
}

/*
**  Function : DC_DestroyWindow
**  Purpose  :
**      当窗口删除时判断是否窗口占据了DC Cache中的DC，如果是释放占据的DC.
*/
void DC_DestroyWindow(PWINOBJ pWin)
{
    PDC pCacheDC;
    int i;

    ASSERT(pWin != NULL);

    for (i = 0; i < DC_CACHE_SIZE; i++)
    {
        pCacheDC = &WindowDCCache[i];
        if (pCacheDC->pDev == pWin)
        {
            pCacheDC->pDev = NULL;
#if (__MP_PLX_GUI)
            /* 多进程版本，CacheDC对象需要重设进程号*/
            WOT_SetObjProcessId(pCacheDC, INVALID_PROCESSID);
#endif
        }
    }
/**/
}

/*
**  Function : DC_CreateWindowDC
**  Purpose  :
**      Creates a window DC.
**  Params   :
**      pWin : Pointer to a specified window.
**  Return   :
**      If the function succeeds, return the pointer to the DC.
**      If the function fails, return NULL.
*/
PDC DC_CreateWindowDC(PWINOBJ pWin, BOOL bClient)
{
    PDC pDC;

    pDC = MemAlloc(sizeof(DC));
    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    MemSet(pDC, 0, sizeof(DC));

    // Adds DC to object table
    // Ensure the DC object is added in object table successfully
#if (__MP_PLX_GUI)
    /* The root window should be shared so that the private DC can be 
    ** accessed by other process when drag the window in the desktop.
    */
    if (pWin == g_pRootWin)
    {
        if (!WOT_RegisterObj((PWSOBJ)pDC, OBJ_DC, TRUE))
        {
            MemFree(pDC);
            return NULL;
        }
    }
    else
#endif
    {
        if (!WOT_RegisterObj((PWSOBJ)pDC, OBJ_DC, FALSE))
        {
            MemFree(pDC);
            return NULL;
        }
    }

    pDC->bType  = DC_WINDOW;
    pDC->bDev   = DEV_DISPLAY;
    pDC->pDev   = pWin;

    if (bClient)
        pDC->wAttrib = WDA_CLIENT;
    else
        pDC->wAttrib = 0;

    pDC->pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
    SetDCItemsToDefault(pDC);

    return pDC;
}

/*
**  Function : DC_CreateMemoryDC
**  Purpose  :
**      Creates a memory DC.
**  Params   :
**      nWidth : Width of memory dc
**  Return   :
**      If the function succeeds, return the pointer to the DC.
**      If the function fails, return NULL.
*/
PDC DC_CreateMemoryDC(int nWidth, int nHeight)
{
    PDC pDC;
    HLOCAL hMemDev;
    void* pMemDev;
    int nRet;
    PGRAPHDEV pGraphDev;

    pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
    ASSERT(pGraphDev != NULL);

    pDC = MemAlloc(sizeof(DC));
    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    MemSet(pDC, 0, sizeof(DC));

    // Creates memory device for memory DC

    nRet = pGraphDev->drv.CreateMemoryDevice(pGraphDev->pDev, nWidth, nHeight, 
        NULL);
    if (nRet <= 0)
    {
        MemFree(pDC);
        return NULL;
    }

    // Uses user memory to alloc memory for memory device, because memory
    // device need large memory block
    hMemDev = (void*)WIN_LocalAlloc(LMEM_MOVEABLE, nRet);
    if (!hMemDev)
    {
        MemFree(pDC);
        return NULL;
    }

    pMemDev = WIN_LocalLock(hMemDev);
    nRet = pGraphDev->drv.CreateMemoryDevice(pGraphDev->pDev, nWidth, nHeight, 
        pMemDev);
    WIN_LocalUnlock((HLOCAL)hMemDev);

    if (nRet <= 0)
    {
        WIN_LocalFree((HLOCAL)hMemDev);
        MemFree(pDC);

        return NULL;
    }

    // Adds DC to object table
    if (!WOT_RegisterObj((PWSOBJ)pDC, OBJ_DC, FALSE))
    {
        pMemDev = WIN_LocalLock((HLOCAL)hMemDev);
        pGraphDev->drv.DestroyMemoryDevice(pMemDev);
        WIN_LocalUnlock((HLOCAL)hMemDev);

        WIN_LocalFree((HLOCAL)hMemDev);
        MemFree(pDC);

        return NULL;
    }

    pDC->bType  = DC_MEMORY;
    pDC->bDev   = DEV_DISPLAY;
    pDC->pDev   = hMemDev;

    pDC->pGraphDev = pGraphDev;
    SetDCItemsToDefault(pDC);

    return pDC;
}

/*
**  Function : DC_CreateCompatiableDC
**  Purpose  :
**      Creates a memory DC.
**  Params   :
**      pOldDC : Identifies the device context. If this handle is NULL, 
**               the function creates a memory device context compatible 
**               with the application’s current screen. 
**  Return   :
**      If the function succeeds, return the pointer to the DC.
**      If the function fails, return NULL.
*/
PDC DC_CreateCompatibleDC(PDC pOldDC)
{
    PDC pDC;

    pDC = MemAlloc(sizeof(DC));
    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    MemSet(pDC, 0, sizeof(DC));

    // Adds DC to object table
    if (!WOT_RegisterObj((PWSOBJ)pDC, OBJ_DC, FALSE))
    {
        MemFree(pDC);
        return NULL;
    }

    pDC->bType = DC_BITMAP;

    if (!pOldDC)
    {
        //  默认的设备为display
        pDC->bDev = DEV_DISPLAY;
        pDC->pDev = WOT_GetStockObj(DEFAULT_BITMAP);
        pDC->pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
    }
    else
    {
        // 暂时不考虑display之外的设备
        pDC->bDev = DEV_DISPLAY;
        pDC->pDev = WOT_GetStockObj(DEFAULT_BITMAP);
        pDC->pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
    }

    SetDCItemsToDefault(pDC);

    return pDC;
}

/*
**  Function : DC_CreateScreenDC
*/
PDC DC_CreateScreenDC(void)
{
    PDC pDC;

    pDC = MemAlloc(sizeof(DC));

    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    MemSet(pDC, 0, sizeof(DC));

    // Adds DC to object table
    // Ensure the DC object is added in object table successfully
    if (!WOT_RegisterObj((PWSOBJ)pDC, OBJ_DC, FALSE))
    {
        MemFree(pDC);
        return NULL;
    }

    pDC->bType  = DC_DEVICE;
    pDC->bDev   = DEV_DISPLAY;

    pDC->pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
    pDC->pDev = pDC->pGraphDev->pDev;
    SetDCItemsToDefault(pDC);

    return pDC;
}

PDC DC_CreateDC(LPCTSTR lpszDriver,      // driver name
              LPCTSTR lpszDevice,        // device name
              LPCTSTR lpszOutput,        // not used; should be NULL
              const void* lpInitData  // optional printer data
)
{
    int nDev;
    PDC pDC;
    PGRAPHDEV pGraphDev;

    if (!stricmp(lpszDriver, "DISPLAY"))
	{
        nDev = SCREEN_MAIN;
		//printf("&&&&&&&&&&&&&&&&&& Create DC:SCREEN_MAIN\r\n");
	}
    else if (!stricmp(lpszDriver, "DISPLAY_1"))
        nDev = SCREEN_SECOND;
    else if (!stricmp(lpszDriver, "DISPLAY_2"))
        nDev = SCREEN_THIRD;
#ifdef PIP_TRANS
	else if (!stricmp(lpszDriver, "DISPLAY_BACK"))
	{
        nDev = SCREEN_BACKGROUND;
		//printf("&&&&&&&&&&&&&&&&&& Create DC:SCREEN_BACKGROUND\r\n");
	}
#endif
    else
        return NULL;

	//printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&& nDev: %d\r\n", nDev);
    pGraphDev = GRAPHDEV_GetGraphDev(MAKE_GRAPHDEV_HANDLE(DEV_DISPLAY, nDev+1));
    if (pGraphDev == NULL)
        return NULL;

    pDC = MemAlloc(sizeof(DC));
    
    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }
    
    MemSet(pDC, 0, sizeof(DC));
    
    // Adds DC to object table
    // Ensure the DC object is added in object table successfully
    if (!WOT_RegisterObj((PWSOBJ)pDC, OBJ_DC, FALSE))
    {
        MemFree(pDC);
        return NULL;
    }
    
    pDC->bType = DC_SCREEN; //非主屏使用内存DC
    pDC->bDev   = DEV_DISPLAY;

    pDC->pGraphDev = pGraphDev;
    pDC->pDev = pDC->pGraphDev->pDev;

    SetDCItemsToDefault(pDC);

    return pDC;
}

#if (PRINTERSUPPORT)

/*
**  Function : DC_CreatePrinterDC
**  Purpose  :
**      Creates a printer DC.
*/
PDC DC_CreatePrinterDC(int width, int height)
{
    PDC pDC;
    HANDLE handle;

    pDC = MemAlloc(sizeof(DC));
    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    MemSet(pDC, 0, sizeof(DC));

    // Adds DC to object table
    // Ensure the DC object is added in object table successfully
    if (!WOT_RegisterObj((PWSOBJ)pDC, OBJ_DC))
    {
        MemFree(pDC);
        return NULL;
    }

    pDC->pDev = PRNDEV_CreatePrinterDev(width, height);
    if (!pDC->pDev)
    {
        MemFree(pDC);
        return NULL;
    }

    pDC->bType  = DC_DEVICE;
    pDC->bDev   = DEV_PRINTER;

    pDC->pGraphDev = GRAPHDEV_GetDefault(DEV_PRINTER);
    SetDCItemsToDefault(pDC);

    return pDC;
}

int DC_StartDoc(PDC pDC, const DOCINFO* pDocInfo)
{
    ASSERT(pDC != NULL);

    if (pDC->bDev != DEV_PRINTER)
        return 0;

    ASSERT(pDC->pDev != NULL);
    return PRNDEV_StartDoc(pDC->pDev, pDocInfo);
}

int DC_EndDoc(PDC pDC)
{
    ASSERT(pDC != NULL);

    if (pDC->bDev != DEV_PRINTER)
        return FALSE;

    ASSERT(pDC->pDev != NULL);
    return PRNDEV_EndDoc(pDC->pDev);
}

/*
**  Function : DC_StartPage
*/
BOOL DC_StartPage(PDC pDC)
{
    ASSERT(pDC != NULL);

    if (pDC->bDev != DEV_PRINTER)
        return 0;

    ASSERT(pDC->pDev != NULL);
    return PRNDEV_StartPage(pDC->pDev);
}

/*
**  Function : DC_EndPage
*/
BOOL DC_EndPage(PDC pDC)
{
    ASSERT(pDC != NULL);

    if (pDC->bDev != DEV_PRINTER)
        return FALSE;

    ASSERT(pDC->pDev != NULL);
    return PRNDEV_EndPage(pDC->pDev);
}

/*
**  Function : DC_StartBand
*/
int DC_StartBand(PDC pDC, PRECT pRect)
{
    int nRet;

    ASSERT(pDC != NULL);

    if (pDC->bDev != DEV_PRINTER)
        return 0;

    ASSERT(pDC->pDev != NULL);
    nRet = PRNDEV_StartBand(pDC->pDev, pRect);

    return nRet;
}

/*
**  Function : DC_EndBand
*/
BOOL DC_EndBand(PDC pDC)
{
    ASSERT(pDC != NULL);

    if (pDC->bDev != DEV_PRINTER)
        return FALSE;

    ASSERT(pDC->pDev != NULL);
    return PRNDEV_EndBand(pDC->pDev);
}

#endif // PRINTERSUPPORT

/*
**  Function : DC_Destroy
**  Purpose  :
**      Destroys the specified graph parameter contents.
**  Params   :
**      pDC : Specifies the graph parameter to be destroyed.
*/
void DC_Destroy(PDC pDC)
{
    ASSERT(pDC != NULL);

    // Remove the DC object from the object table
    WOT_UnregisterObj((PWSOBJ)pDC);

    if (pDC->bDev == DEV_DISPLAY)
    {
        switch (pDC->bType)
        {
        case DC_MEMORY :
            {
                void *pMemDev;
                ASSERT(GRAPHDEV_GetDefault(DEV_DISPLAY) == pDC->pGraphDev);

                pMemDev = WIN_LocalLock((HLOCAL)pDC->pDev);
                pDC->pGraphDev->drv.DestroyMemoryDevice(pMemDev);
                WIN_LocalUnlock((HLOCAL)pDC->pDev);
                LocalFree((HLOCAL)pDC->pDev);
            }
            break;
            
        case DC_BITMAP : 
            
            ASSERT(pDC->pDev != NULL);
            
            // 清除选中位图的选中标志, 可能引起位图的删除操作
            WOT_UnselectObj((PXGDIOBJ)(pDC->pDev));
            
            break;
            
        default :
            
            break;
        }
    }
#ifdef MONOMEMSUPPORT
    else if (pDC->bDev == DEV_MONOMEM)
    {
    }
#endif  // MONOMEMSUPPORT
#if (PRINTERSUPPORT)
    else if (pDC->bDev == DEV_PRINTER)
    {
        ASSERT(pDC->bType == DC_DEVICE);
        ASSERT(pDC->pDev != NULL);
        PRNDEV_DestroyPrinterDev(pDC->pDev);
    }
#endif  // PRINTERSUPPORT
    else
    {
        ASSERT(0);
    }

    // 删除DC中的物理pen, brush, font对象, 这里不需要判断原来的对象是否是
    // 静态对象, 静态对象的判断放在物理对象删除函数中进行
    /**/
    DC_ReleasePhysPen(pDC, pDC->pPen, pDC->pPhysPen);
    WOT_UnselectObj((PXGDIOBJ)pDC->pPen);

    DC_ReleasePhysBrush(pDC, pDC->pBrush, pDC->pPhysBrush);
    WOT_UnselectObj((PXGDIOBJ)pDC->pBrush);

    DC_ReleaseFontDev(pDC, pDC->pFont, pDC->pFontDev);
    WOT_UnselectObj((PXGDIOBJ)pDC->pFont);

    // Free space of the DC object
    MemFree(pDC);
}

/*
**  Function : DC_IsWindowDC
**  Purpose  :
**      Retrieves whether a specifed DC is window DC
*/
BOOL DC_IsWindowDC(PDC pDC)
{
    ASSERT(pDC != NULL);

    return (pDC->bDev == DEV_DISPLAY && pDC->bType == DC_WINDOW);
}

/*
**  Function : DC_GetWindow
*/
PWINOBJ DC_GetWindow(PDC pDC)
{
    PWINOBJ pWin;

    ASSERT(pDC != NULL);

    if (pDC->bType != DC_WINDOW)
        return NULL;

    ASSERT(pDC->bDev == DEV_DISPLAY);

    pWin = (PWINOBJ)(pDC->pDev);

    ASSERT(pWin && WND_IsWindow(pWin));
    if (pWin && !WND_IsWindow(pWin))
        pWin = NULL;

    return pWin;
}

/*
**  Function : DC_GetPaintArea
*/
int DC_GetPaintArea(PDC pDC)
{
    int nPaintArea;

    ASSERT(pDC != NULL);
    ASSERT(pDC->bDev == DEV_DISPLAY);
    ASSERT(pDC->bType == DC_WINDOW);

    if (pDC->wAttrib & WDA_CLIENT)
    {
        if (pDC->wAttrib & WDA_UPDATE)
            nPaintArea = PA_UPDATECLIENT;
        else
            nPaintArea = PA_CLIENT;
    }
    else
    {
        if (pDC->wAttrib & WDA_UPDATE)
            nPaintArea = PA_UPDATEWINDOW;
        else
            nPaintArea = PA_WINDOW;
    }

    return nPaintArea;
}

/*
**  Function : DC_GetDevice
*/
void* DC_GetDevice(PDC pDC)
{
    void* pDev;
    PGRAPHDEV pGraphDev;

    ASSERT(pDC != NULL);

    if (pDC->bDev == DEV_DISPLAY)
    {
        switch (pDC->bType)
        {
        case DC_WINDOW :

            if (pDC->pDev == NULL)
            {
                /* If the pDC is a cache DC, the pDev is the pWin, and if 
                ** it's null, the device should be null.
                ** Else the device should be the display device.
                */
                pDev = NULL;
            }
            else
            {
                pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
                pDev = pGraphDev->pDev;
            }
//            pDev = g_pDisplayDev;
            break;
            
        case DC_MEMORY :
            
            pDev = LocalLock((HLOCAL)pDC->pDev);
            break;
            
        case DC_BITMAP :
            
            pDev = BMP_GetDevice((PBMPOBJ)pDC->pDev);
            break;
            
        default :   // Screen DC
            
            pDev = pDC->pDev;
        }
    }
#if (PRINTERSUPPORT)
    else if (pDC->bDev == DEV_PRINTER)
    {
        ASSERT(pDC->bType == DC_DEVICE);
        pDev = pDC->pDev;
    }
#endif // PRINTERSUPPORT
    else
    {
        pDev = NULL;
        ASSERT(0);
    }

    return pDev;
}

/*
**  Function : DC_ReleaseDevice
**  Purpose  :
**      
*/
void DC_ReleaseDevice(PDC pDC)
{
    ASSERT(pDC != NULL);

    if (pDC->bDev == DEV_DISPLAY)
    {
        switch (pDC->bType)
        {
        case DC_MEMORY :
            
            LocalUnlock((HLOCAL)pDC->pDev);
            break;
            
        case DC_BITMAP :
            
            BMP_ReleaseDevice((PBMPOBJ)pDC->pDev);
            break;
        }
    }
#if (PRINTERSUPPORT)
    else if (pDC->bDev == DEV_PRINTER)
    {
        ASSERT(pDC->bType == DC_DEVICE);
    }
#endif
    else
    {
        ASSERT(0);
    }
}

/*
**  Function : DC_GetDeviceCaps
**  Purpose  :
**      Retrieves device-specific information about a specified device. 
**  Params   :
**      pDC    : Identifies the device context. 
**      nIndex : Specifies the item to return. 
**  Return   :
**      The return value specifies the value of the desired item. 
*/
int DC_GetDeviceCaps(PDC pDC, int nIndex)
{
    void* pDev;
    int nRet = 0;

    if (!pDC)
        return DISPDEV_GetCaps(nIndex);

    if (pDC->bDev == DEV_DISPLAY)
    {
        switch (pDC->bType)
        {
        case DC_BITMAP :

            // 注意，对于bitmap DC需要单独处理，因为调用
            // DC_ReleaseDevice会导致bitmap DC的对应的物理对象的
            // bitmap buffer指针为NULL
            pDev = BMP_GetPhysObj((PBMPOBJ)pDC->pDev);
            nRet = DISPDEV_GetDeviceCaps(pDev, nIndex);
            break;

        default :

            pDev = DC_GetDevice(pDC);
            nRet = DISPDEV_GetDeviceCaps(pDev, nIndex);
            DC_ReleaseDevice(pDC);
            break;
        }
    }
#if (PRINTERSUPPORT)
    else if (pDC->bDev == DEV_PRINTER)
    {
        ASSERT(pDC->bType == DC_DEVICE);
        nRet = PRNDEV_GetCaps(pDC->pDev, nIndex);
    }
#endif // PRINTERSUPPORT
    else
    {
        ASSERT(0);
    }

    return nRet;
}

/*
**  Function : DC_SelectObject
**  Purpose  :
**      Selects an object into the specified device context. The new 
**      object replaces the previous object of the same type.
**  Params   :
**      pDC     : Identifies the device context. 
**      hGdiObj : Identifies the object to be selected. 
**  Return   :
**      If the function succeeds, return the previously selected object 
**      of the specified type. An application should always replace a 
**      new object with the original, default object after it has finished 
**      drawing with the new object. If the function fails, return NULL.
*/
PXGDIOBJ DC_SelectObject(PDC pDC, PXGDIOBJ pObj)
{
    int nObjType;
    PXGDIOBJ pOldObj = NULL;

    ASSERT(pDC != NULL);
    if (pObj == NULL)
        return NULL;

    nObjType = WOT_GetObjType((PWSOBJ)pObj);

    // Before select a GDI object into DC, must ensure that the physical
    // object of the GDI object isn't NULL. If the physical object is
    // NULL, the select operation fails.

    switch (nObjType)
    {
        case OBJ_PEN :
        {
            PPHYSPEN pPhysPen;

            // 要选进DC的对象与DC的对象是同一对象, 直接返回
            if (pObj == (PXGDIOBJ)pDC->pPen)
                break;

            // 如果要选进DC的对象是有效对象, 并且成功获得它的物理对象, 进行对
            // 象的选进操作, 否则失败返回
            pPhysPen = DC_GetPhysPen(pDC, (PPENOBJ)pObj);
            if (pPhysPen)
            {
                // 保存原来的对象作为返回值, 选出，引用计数减1
                DC_ReleasePhysPen(pDC, pDC->pPen, pDC->pPhysPen);
                pOldObj = WOT_UnselectObj((PXGDIOBJ)pDC->pPen);
            
                // 将新对象选进DC中，引用计数加1
                pDC->pPen = (PPENOBJ)pObj;
                pDC->pPhysPen = pPhysPen;

                pDC->PenColor = pDC->pPen->logpen.color;
                pDC->PenPhysColor = DC_GetPhysColor(pDC, pDC->PenColor);
                WOT_SelectObj(pObj);
            }

            break;
        }

        case OBJ_BRUSH :
        {
            PPHYSBRUSH pPhysBrush;

            // 要选进DC的对象与DC的对象是同一对象, 直接返回
            if (pObj == (PXGDIOBJ)pDC->pBrush)
                break;

            // 如果要选进DC的对象是有效对象, 并且成功获得它的物理对象, 进行
            // 对象的选进操作, 否则失败返回

            pPhysBrush = DC_GetPhysBrush(pDC, (PBRUSHOBJ)pObj);
            if (pPhysBrush)
            {
                // 保存原来的对象句柄作为返回值
                DC_ReleasePhysBrush(pDC, pDC->pBrush, pDC->pPhysBrush);
                pOldObj = WOT_UnselectObj((PXGDIOBJ)pDC->pBrush);
            
                // 将新对象选进DC中
                pDC->pBrush = (PBRUSHOBJ)pObj;
                pDC->pPhysBrush = pPhysBrush;

                pDC->BrushColor = pDC->pBrush->logbrush.color;
                pDC->BrushPhysColor = DC_GetPhysColor(pDC, pDC->BrushColor);
                WOT_SelectObj(pObj);
            }

            break;
        }

        case OBJ_BITMAP :
        {

            // 位图对象只能被选进位图DC(DC_BITMAP)中, 不是位图DC, 直接返回
            if (pDC->bType != DC_BITMAP)
                break;

            // 要选进DC的对象与DC的对象是同一对象, 直接返回
            if (pObj == (PXGDIOBJ)pDC->pDev)
                break;

            // 只有与显示设备兼容的位图能被选进位图DC中, 而且只能选进一个位图
            // DC中, 因为如果同时选进两个位图DC中, 那么在这两个位图DC上进行的
            // 绘图操作将相互冲突.
            // 由于兼容DC的默认位图是单色位图，所以单色位图必须也能够选入兼容
            // DC中；否则，兼容DC的默认位图无法选回兼容DC中，这个bug是2000年
            // 9月5日帮助上海Bell的工程师调试程序时发现的。
            if (pObj && WOT_GetSelectCount(pObj) == 0)
            {
                // 保存原来的对象作为返回值
                pOldObj = WOT_UnselectObj((PXGDIOBJ)pDC->pDev);
            
                // 将新对象选进DC中, 并设置新位图的选中标志
                pDC->pDev = (PBMPOBJ)pObj;
                WOT_SelectObj(pObj);
            }
        
            break;
        }

        case OBJ_FONT :
        {
            PFONTDEV pFontDev;

            // 要选进DC的对象与DC的对象是同一对象, 直接返回
            if (pObj == (PXGDIOBJ)pDC->pFont)
                break;

            // 如果要选进DC的对象是有效对象, 并且成功获得它的物理对象, 进行
            // 对象的选进操作, 否则失败返回
            pFontDev = DC_GetFontDev(pDC, (PFONTOBJ)pObj);
            if (pFontDev)
            {
                DC_ReleaseFontDev(pDC, pDC->pFont, pDC->pFontDev);
                pOldObj = WOT_UnselectObj((PXGDIOBJ)pDC->pFont);

                // 将新对象选进DC中
                pDC->pFont = (PFONTOBJ)pObj;
                pDC->pFontDev = pFontDev;
            }

            break;
        }

        case OBJ_PALETTE :

            break;

        default:
            
            break;
    }

    return pOldObj;
}

/*
**  Function : DC_SetTextColor
**  Purpose  :
**      Sets the text color for the specified device context to the 
**      specified color.
**  Params   :
**      pDC     : Identifies the device context. 
**      color   : Specifies the color of the text.
**  Return   :
**      If the function succeeds, returns a the previous text color. 
**      If the function fails, returns CLR_INVALID. To get extended 
**      error information, call GetLastError. 
*/
COLORREF DC_SetTextColor(PDC pDC, COLORREF color)
{
    COLORREF crOld;

    ASSERT(pDC != NULL);

    crOld = pDC->TextColor;
    pDC->TextColor = color;

    pDC->TextPhysColor = DC_GetPhysColor(pDC, color);

    return crOld;
}

/*
**  Function : DC_GetTextColor
**  Purpose  :
**      Retrieves the current text color for the specified device context.
**  Params   :
**      pDC     : Identifies the device context. 
**      color   : Specifies the color of the text.
**  Return   :
**      If the function succeeds, returns a the previous text color. 
**      If the function fails, returns CLR_INVALID.
*/
COLORREF DC_GetTextColor(PDC pDC)
{
    ASSERT(pDC != NULL);

    return pDC->TextColor;
}

/*
**  Function : DC_SetTextAlign
*/
UINT DC_SetTextAlign(PDC pDC, UINT uMode)
{
    UINT uOldMode;

    ASSERT(pDC != NULL);

    uOldMode = pDC->uTextAlignMode;

    pDC->uTextAlignMode = uMode;

    return uOldMode;
}

/*
**  Function : DC_GetTextAlign
*/
UINT DC_GetTextAlign(PDC pDC)
{
    ASSERT(pDC != NULL);

    return pDC->uTextAlignMode;
}

/*
** Function DC_SetPenColor
*/
COLORREF DC_SetPenColor(PDC pDC, COLORREF color)
{
    COLORREF crOld;

    ASSERT(pDC != NULL);

    crOld = pDC->PenColor;
    pDC->PenColor = color;
    pDC->PenPhysColor = DC_GetPhysColor(pDC, color);

    return crOld;
}

/*
** Function DC_GetPenColor
*/
COLORREF DC_GetPenColor(PDC pDC)
{
//    COLORREF crOld;

    ASSERT(pDC != NULL);

    return pDC->PenColor;
}

/*
** Function DC_SetBrushColor
*/
COLORREF DC_SetBrushColor(PDC pDC, COLORREF color)
{
    COLORREF crOld;

    ASSERT(pDC != NULL);

    crOld = pDC->BrushColor;
    pDC->BrushColor = color;
    pDC->BrushPhysColor = DC_GetPhysColor(pDC, color);

    return crOld;
}

/*
** Function DC_GetBrushColor
*/
COLORREF DC_GetBrushColor(PDC pDC)
{
//    COLORREF crOld;

    ASSERT(pDC != NULL);

    return pDC->BrushColor;
}
/*
**  Function : DC_SetBkColor
**  Purpose  :
**      Sets the current background color to the specified color value, 
**      or to the nearest physical color if the device cannot represent 
**      the specified color value. 
**  Params   :
**      pDC  : Identifies the device context. 
**  Return   :
**      If the function succeeds, returns the current text color.
**      If the funciton fails, returns CLR_INVALID.
*/
COLORREF DC_SetBkColor(PDC pDC, COLORREF color)
{
    COLORREF crOld;
    BYTE     i;

    ASSERT(pDC != NULL);

    crOld = pDC->BkColor;
    pDC->BkColor = color;
    i = color >> 24;
    if (i == 0)
        pDC->BkPhysColor = DC_GetPhysColor(pDC, color);
    else
        pDC->BkPhysColor = color;

    return crOld;
}

/*
**  Function : DC_GetBkColor
**  Purpose  :
**      Returns the current background color for the specified device 
**      context. 
**  Params   :
**      pDC  : Indentifies the device context.
**  Return   :
**      If the function succeeds, returns a COLORREF value for the current
**      background color. If the function fails, the return value is 
**      CLR_INVALID. 
*/
COLORREF DC_GetBkColor(PDC pDC)
{
    ASSERT(pDC != NULL);

    return pDC->BkColor;
}

/*
**  Function : DC_SetBkMode
**  Purpose  :
**      Sets the background mix mode of the specified device context. The 
**      background mix mode is used with text, hatched brushes, and pen 
**      styles that are not solid lines. 
**  Params   :
**      hdc     : Indentifies the device context.
**      nBkMode : Specifies the background mode. This parameter can be 
**                either of the following values : 
**
**                    Value         Description
**
**                    OPAQUE        Background is filled with the current 
**                                  background color before the text, 
**                                  hatched brush, or pen is drawn.
**                    TRANSPARENT   Background remains untouched.
**  Return   :
**      If the function succeeds, return value specifies the previous 
**      background mode.If the function fails, returns zero. 
*/
int DC_SetBkMode(PDC pDC, int nBkMode)
{
    int nOldBkMode;

    ASSERT(pDC != NULL);

    if (nBkMode < BM_TRANSPARENT || nBkMode > BM_NEWALPHATRANSPARENT)
        return 0;

    nOldBkMode = pDC->nBkMode;
    pDC->nBkMode = nBkMode;

    return nOldBkMode;
}

/*
**  Function : DC_GetBkMode
**  Purpose  :
**      Returns the current background mix mode for a specified device 
**      context. The background mix mode of a device context affects 
**      text, hatched brushes, and pen styles that are not solid lines. 
**  Params   :
**      hdc  : Indentifies the device context.
**  Return   :
**      If the function succeeds, return the current background mix mode, 
**      either BM_OPAQUE or BM_TRANSPARENT. If the function fails, return
**      zero. 
*/
int DC_GetBkMode(PDC pDC)
{
    ASSERT(pDC != NULL);

    return pDC->nBkMode;
}

/*
**  Function : DC_SetROP2
**  Purpose  :
**      Sets the current foreground mix mode. The foreground mix mode is
**      used to combine pen and interiors of filled objects with the colors
**      already on the screen. The foreground mix mode defines how colors
**      from the brush or pen and the colors in the exist image are to be 
**      combined.
**  Params   :
**      hdc  : Indentifies the device context.
**      nRop : specified the new mix mode.
**  Return   :
**      If the function succeeds, the return value specifies the previous 
**      mix mode. If the function fails, the return value is zero. 
*/
int DC_SetROP2(PDC pDC, int nRop)
{
    int nOldRop;

    ASSERT(pDC != NULL);

    if (nRop < 1 || nRop > 16)
        nRop = ROP_SRC;

    nOldRop = pDC->nRop;
    pDC->nRop = nRop;

    return nOldRop;
}

/*
**  Function : DC_GetROP2
**  Purpose  :
**      The function retrieves the foreground mix mode of the specified 
**      device context. The mix mode specifies how the pen or interior 
**      color and the color already on the screen are combined to yield
**      a new color.
**  Params   :
**      pDc  : Specifies the device context.
**  Return   :
**      return the current mix mode of the specified device context.
*/
int DC_GetROP2(PDC pDC)
{
    ASSERT(pDC != NULL);

    return pDC->nRop;
}

/*
**  Function : DC_GetBrushOrg
**  Purpose  :
**      Retrieves the brush origin coordinate of the specified context.
*/
void DC_GetBrushOrg(PDC pDC, PPOINT pPoint)
{
    ASSERT(pDC != NULL);
    ASSERT(pPoint != NULL);

    pPoint->x = pDC->nBrushOrgX;
    pPoint->y = pDC->nBrushOrgY;
}

/*
**  Function : DC_SetBrushOrg
**  Purpose  :
**      Sets the brush origin coordinate of the specifed device context.
*/
void DC_SetBrushOrg(PDC pDC, int nOrgX, int nOrgY, PPOINT pPoint)
{
    ASSERT(pDC != NULL);

    if (pPoint)
    {
        pPoint->x = pDC->nBrushOrgX;
        pPoint->y = pDC->nBrushOrgY;
    }

    pDC->nBrushOrgX = nOrgX;
    pDC->nBrushOrgY = nOrgY;
}

/*
**  Function : DC_GetCurPos
**  Purpose  :
*/
void DC_GetCurPos(PDC pDC, PPOINT pPoint)
{
    ASSERT(pDC != NULL);
    ASSERT(pPoint != NULL);

    pPoint->x = pDC->nCurPosX;
    pPoint->y = pDC->nCurPosY;
}

/*
**  Function : DC_GetCurPos
*/
void DC_SetCurPos(PDC pDC, int x, int y, PPOINT pPoint)
{
    ASSERT(pDC != NULL);

    if (pPoint)
    {
        pPoint->x = pDC->nCurPosX;
        pPoint->y = pDC->nCurPosY;
    }

    pDC->nCurPosX = x;
    pDC->nCurPosY = y;
}

/*
**  Function : DC_GetWindowOrg
**  Purpose  :
*/
void DC_GetWindowOrg(PDC pDC, PPOINT pPoint)
{
    ASSERT(pDC != NULL);
    ASSERT(pPoint != NULL);

    pPoint->x = pDC->nWndOrgX;
    pPoint->y = pDC->nWndOrgY;
}

/*
**  Function : DC_SetWindowOrg
*/
void DC_SetWindowOrg(PDC pDC, int x, int y, PPOINT pPoint)
{
    ASSERT(pDC != NULL);

    if (pPoint)
    {
        pPoint->x = pDC->nWndOrgX;
        pPoint->y = pDC->nWndOrgY;
    }

    pDC->nWndOrgX = x;
    pDC->nWndOrgY = y;
}

/*
**  Function : DC_GetWindowExt
**  Purpose  :
*/
void DC_GetWindowExt(PDC pDC, PSIZE pSize)
{
    ASSERT(pDC != NULL);
    ASSERT(pSize != NULL);

    pSize->cx = pDC->nWndExtX;
    pSize->cy = pDC->nWndExtY;
}

/*
**  Function : DC_SetWindowExt
*/
void DC_SetWindowExt(PDC pDC, int nExtX, int nExtY, PSIZE pSize)
{
    ASSERT(pDC != NULL);

	if (pDC->nMapMode != MM_ISOTROPIC && pDC->nMapMode != MM_ANISOTROPIC)
		return;

    if (pSize)
    {
        pSize->cx = pDC->nWndExtX;
        pSize->cy = pDC->nWndExtY;
    }

    pDC->nWndExtX = nExtX;
    pDC->nWndExtY = nExtY;
}

/*
**  Function : DC_GetViewportOrg
**  Purpose  :
*/
void DC_GetViewportOrg(PDC pDC, PPOINT pPoint)
{
    ASSERT(pDC != NULL);
    ASSERT(pPoint != NULL);

    pPoint->x = pDC->nVportOrgX;
    pPoint->y = pDC->nVportOrgY;
}

/*
**  Function : DC_SetViewportOrg
*/
void DC_SetViewportOrg(PDC pDC, int x, int y, PPOINT pPoint)
{
    ASSERT(pDC != NULL);

    if (pPoint)
    {
        pPoint->x = pDC->nVportOrgX;
        pPoint->y = pDC->nVportOrgY;
    }

    pDC->nVportOrgX = x;
    pDC->nVportOrgY = y;
}

/*
**  Function : DC_GetViewportExt
**  Purpose  :
*/
void DC_GetViewportExt(PDC pDC, PSIZE pSize)
{
    ASSERT(pDC != NULL);
    ASSERT(pSize != NULL);

    pSize->cx = pDC->nVportExtX;
    pSize->cy = pDC->nVportExtY;
}

/*
**  Function : DC_SetViewportExt
*/
void DC_SetViewportExt(PDC pDC, int nExtX, int nExtY, PSIZE pSize)
{
    ASSERT(pDC != NULL);

	if (pDC->nMapMode != MM_ISOTROPIC && pDC->nMapMode != MM_ANISOTROPIC)
		return;

    if (pSize)
    {
        pSize->cx = pDC->nVportExtX;
        pSize->cy = pDC->nVportExtY;
    }

    pDC->nVportExtX = nExtX;
    pDC->nVportExtY = nExtY;
}

/*
**  Function : DC_SetArcDirection
*/
int DC_SetArcDirection(PDC pDC,int nArcDirection)
{
    int nOldDirection;

    ASSERT(pDC != NULL);

    nOldDirection = pDC->nArcDirection;
    pDC->nArcDirection = nArcDirection;

    return nOldDirection;
}

/*
**  Function : DC_GetArcDirection
*/
int DC_GetArcDirection(PDC pDC)
{
    ASSERT(pDC != NULL);

    return pDC->nArcDirection;
}

/*
**  Function : DC_GetMapMode
*/
int DC_GetMapMode(PDC pDC)
{
	ASSERT(pDC != NULL);

    return pDC->nMapMode;
}

/*
**  Function : DC_SetMapMode
*/
int DC_SetMapMode(PDC pDC, int nMapMode)
	{
	int nOldMapMode;

	ASSERT(pDC != NULL);

	if (nMapMode < MM_MIN || nMapMode > MM_MAX)
        return 0;

    nOldMapMode = pDC->nMapMode;
    pDC->nMapMode = nMapMode;

    return nOldMapMode;
}

/*
**  Function : DC_DPtoLP
*/
void DC_DPtoLP(PDC pDC, PPOINT pPoints, int nCount)
{
	int		i;

	ASSERT(pDC != NULL);

	for (i=0; i<nCount; i++)
	{
		pPoints[i].x = (pPoints[i].x - pDC->nVportOrgX) * 
						pDC->nWndExtX / pDC->nVportExtX + pDC->nWndOrgX;
		pPoints[i].y = (pPoints[i].y - pDC->nVportOrgY) * 
						pDC->nWndExtY / pDC->nVportExtY + pDC->nWndOrgY;
	}
}

/*
**  Function : DC_LPtoDP
*/
void DC_LPtoDP(PDC pDC, PPOINT pPoints, int nCount)
{
	int		i;

	ASSERT(pDC != NULL);

	for (i=0; i<nCount; i++)
	{
		pPoints[i].x = (pPoints[i].x - pDC->nWndOrgX) * 
						pDC->nVportExtX / pDC->nWndExtX + pDC->nVportOrgX;
		pPoints[i].y = (pPoints[i].y - pDC->nWndOrgY) * 
						pDC->nVportExtY / pDC->nWndExtY + pDC->nVportOrgY;
	}
}

/*
**  Function : DC_SetClipRect
*/
void DC_SetClipRect(PDC pDC, const RECT* prcClip)
{
    ASSERT(pDC != NULL);

    if (prcClip)
    {
        CopyRect(&pDC->rcClip, prcClip);
        pDC->bClip = TRUE;
    }
    else
        pDC->bClip = FALSE;
}

/*
**  Function : DC_GetClipRect
**  Purpose  :
**      Retrieves the clip rectangle of the specified device context.
**      
*/
void DC_GetClipRect(PDC pDC, PRECT prcClip)
{
    ASSERT(pDC != NULL);
    ASSERT(prcClip != NULL);

    if (!pDC->bClip)
        SetRectEmpty(prcClip);
    else
        *prcClip = pDC->rcClip;
}

/*
**  Function : DC_GetTextMetrics
**  Purpose  :
**      Fills the specified buffer with the metrics for the DC font.
*/
void DC_GetTextMetrics(PDC pDC, PTEXTMETRIC ptm)
{
    FONTINFO fontInfo;
    PFONTDEV pFontDev;
    PFONTOBJ pFontObj = NULL;

    ASSERT(ptm != NULL);

    if (pDC)
        pFontDev = pDC->pFontDev;
    else
    {
        pFontObj = (PFONTOBJ)WOT_GetStockObj(SYSTEM_FONT);
        pFontDev = DC_GetFontDev(NULL, pFontObj);
    }

    ASSERT(pFontDev != NULL);

    g_FontDrv.GetFontInfo(pFontDev, &fontInfo);

    if (pDC == NULL)
        DC_ReleaseFontDev(NULL, pFontObj, pFontDev);

    ptm->tmHeight           = fontInfo.height;
    ptm->tmAscent           = 0;
    ptm->tmDescent          = 0;
    ptm->tmInternalLeading  = 0;
    ptm->tmExternalLeading  = 0;
    ptm->tmAveCharWidth     = fontInfo.width / 2;
    ptm->tmMaxCharWidth     = fontInfo.width;
    ptm->tmWeight           = 0;
    ptm->tmOverhang         = 0;
    ptm->tmDigitizedAspectX = 0;
    ptm->tmDigitizedAspectY = 0;
    ptm->tmFirstChar        = 0;
    ptm->tmLastChar         = 0;
    ptm->tmDefaultChar      = 0;
    ptm->tmBreakChar        = 0;
    ptm->tmItalic           = 0;
    ptm->tmUnderlined       = 0;
    ptm->tmStruckOut        = 0;
    ptm->tmPitchAndFamily   = 0;
    ptm->tmCharSet          = fontInfo.charset;
}

#ifdef RGN_SUPPORT

/*
**  Function : DC_SelectClipRgn
**  Purpose  :
**      Combines the specified region with the current clipping region by 
**      using the specified mode. 
*/
int DC_SelectClipRgn(PDC pDC, PRGNOBJ prgn, int nMode)
{
    ASSERT(pDC != NULL);
    return 0;
}

/*
**  Function : DC_GetClipBox
**  Purpose  :
**      Retrieves the dimensions of the tightest bounding rectangle that 
**      can be drawn around the current visible area on the device. The 
**      visible area is defined by the current clipping region  as well 
**      as any overlapping windows. 
*/
int DC_GetClipBox(PDC pDC, RECT* pRect)
{
    ASSERT(pDC != NULL);
    return 0;
}

/*
**  Function : DC_IntersectClipRect
**  Purpose  :
**      Creates a new clipping region from the intersection of the 
**      current clipping region and the specified rectangle. 
*/
int DC_IntersectClipRect(PDC pDC, int x1, int y1, int x2, int y2)
{
    ASSERT(pDC != NULL);
    return 0;
}

/*
**  Function : DC_OffsetClipRgn
**  Purpose  :
**      Moves the clipping region of a device context by the specified 
**      offsets. 
*/
int DC_OffsetClipRgn(PDC pDC, int x, int y)
{
    ASSERT(pDC != NULL);
    return 0;
}

/*
**  Function : DC_ExcludeClipRect
**  Purpose  :
**      Creates a new clipping region that consists of the existing 
**      clipping region minus the specified rectangle. 
*/
int DC_ExcludeClipRect(PDC pDC, int x1, int y1, int x2, int y2)
{
    ASSERT(pDC != NULL);
    return 0;
}

/*
**  Function : DC_PtVisible
**  Purpose  :
**      Indicates whether the specified point is within the clipping region
**      of a device context. 
*/
BOOL DC_PtVisible(PDC pDC, int x, int y)
{
    ASSERT(pDC != NULL);
    return FALSE;
}

/*
**  Function : DC_RectVisible
**  Purpose  :
**      Determines whether any part of the specified rectangle lies within
**      the clipping region of a device context. 
*/
BOOL DC_RectVisible(PDC pDC, const RECT* pRect)
{
    ASSERT(pDC != NULL);
    return FALSE;
}

#endif  // RGNSUPPORT

/*
**  Function : DC_SetItemsToDefault
**  Purpose  :
**      Set DC items to defautl value.
*/
void DC_SetItemsToDefault(PDC pDC)
{
    ASSERT(pDC != NULL);

    SetDCItemsToDefault(pDC);
}

/*
**  Function : DC_ToGraphParam
**  Purpose  :
**      Converts parameter of DC to graph draw param structure.
*/
void DC_ToGraphParam(PDC pDC, PGRAPHPARAM pGraphParam)
{
    POINT ptOrg;
    RECT rect;
    PBRUSHOBJ pBrush;
    SIZE brush_size;
    int nPaintArea = PA_CLIENT;

    ASSERT(pDC != NULL);
    ASSERT(pGraphParam != NULL);

    MemSet(pGraphParam, 0, sizeof(GRAPHPARAM));

    /* If is window DC, convert coordinate from window to screen */
    if (pDC->bType == DC_WINDOW)
    {
        ASSERT(pDC->bDev == DEV_DISPLAY);

        if (pDC->wAttrib & WDA_CLIENT)
        {
            if (pDC->wAttrib & WDA_UPDATE)
                nPaintArea = PA_UPDATECLIENT;
            else
                nPaintArea = PA_CLIENT;
        }
        else
        {
            if (pDC->wAttrib & WDA_UPDATE)
                nPaintArea = PA_UPDATEWINDOW;
            else
                nPaintArea = PA_WINDOW;
        }

        WND_GetPaintOrg((PWINOBJ)(pDC->pDev), nPaintArea, &ptOrg);
    }
    else
    {
        ptOrg.x = ptOrg.y = 0;
    }

    // Set initial flags to be 0
    pGraphParam->flags = 0;

    if (pDC->bType == DC_WINDOW)
    {
        pGraphParam->flags |= GPF_CLIP_RECT;

        WND_GetPaintRect((PWINOBJ)(pDC->pDev), nPaintArea, 
            &pGraphParam->clip_rect);

        if (pDC->bClip)
        {
            // 注意，clip rect使用设备坐标, 所以不需要LPToDP的转换

            CopyRect(&rect, &pDC->rcClip);
            OffsetRect(&rect, ptOrg.x, ptOrg.y);

            IntersectRect(&pGraphParam->clip_rect, &pGraphParam->clip_rect, 
                &rect);
        }
    }
    else
    {
        pGraphParam->flags |= GPF_CLIP_RECT;
        SetRect(&pGraphParam->clip_rect, 0, 0, 
            DC_GetDeviceCaps(pDC, HORZRES), 
            DC_GetDeviceCaps(pDC, VERTRES));

        if (pDC->bClip)
        {
            // 注意，clip rect使用设备坐标, 所以不需要LPToDP的转换
            IntersectRect(&pGraphParam->clip_rect, &pGraphParam->clip_rect, 
                &pDC->rcClip);
        }
    }

    // If use pen, sets pen pointer
    ASSERT(pDC->pPhysPen != NULL);
    pGraphParam->flags |= GPF_PEN;
    pGraphParam->pPen = pDC->pPhysPen;

    // If use brush, copy brush pointer

    ASSERT(pDC->pPhysBrush != NULL);
    pGraphParam->flags |= GPF_BRUSH;
    pGraphParam->pBrush = pDC->pPhysBrush;
    
    // 获得DC中的brush对象指针, 为NULL(已被删除)取默认的刷子
    if (!pDC->pBrush)
    {
        pBrush = (PBRUSHOBJ)WOT_GetStockObj(WHITE_BRUSH);
        BRUSH_GetSize(pBrush, &brush_size);
    }
    else
    {
        BRUSH_GetSize(pDC->pBrush, &brush_size);
    }
    
    pGraphParam->brush_org_x = (ptOrg.x - pDC->nBrushOrgX) % brush_size.cx;
    pGraphParam->brush_org_y = (ptOrg.y - pDC->nBrushOrgY) % brush_size.cy;
    
    if (pGraphParam->brush_org_x < 0)
        pGraphParam->brush_org_x += (int16)brush_size.cx;
    
    if (pGraphParam->brush_org_y < 0)
        pGraphParam->brush_org_y += (int16)brush_size.cy;
    
    /* Set ROP to ROP2 */
    pGraphParam->flags      |= GPF_ROP2 | GPF_ROP3;
    pGraphParam->arcDirection = pDC->nArcDirection;
    pGraphParam->bk_mode     = pDC->nBkMode;
    pGraphParam->bk_color    = pDC->BkPhysColor;
    pGraphParam->text_color  = pDC->TextPhysColor;
    pGraphParam->rop         = pDC->nRop;
    pGraphParam->pen_color  = pDC->PenPhysColor;
    pGraphParam->brush_color = pDC->BrushPhysColor;
}

/*
**  Function : DC_GetStockPhysObj
**  Purpose  :
**      Gets the physical stock object of a spcified stock object. 
*/
void* DC_GetStockPhysObj(PDC pDC, PXGDIOBJ pObj)
{
    int uIndex;

    // Get index from the specified handle.
    uIndex = WOT_GetStockObjIndex(pObj);
    if (uIndex < 0)
        return NULL;

    if (pDC != NULL)
        return GRAPHDEV_GetStockPhysObj(pDC->pGraphDev, uIndex);
    
    return GRAPHDEV_GetStockPhysObj(GRAPHDEV_GetDefault(DEV_DISPLAY), uIndex);
}


/*
**  Function : DC_GetPhysPen
**  Purpose  :
**      Gets the physical pen object of a spcified pen object. 
*/
PPHYSPEN DC_GetPhysPen(PDC pDC, PPENOBJ pPen)
{
    int nRet;
    PPHYSPEN pPhysPen;

    ASSERT(pDC != NULL);
    if (pPen == NULL)
        return NULL;

    if (WOT_IsStockObj((PXGDIOBJ)pPen))
        return DC_GetStockPhysObj(pDC, (PXGDIOBJ)pPen);

    nRet = pDC->pGraphDev->drv.RealizePen(pDC->pGraphDev->pDev, &pPen->logpen, NULL);
    if (nRet <= 0)
        return NULL;
    
    pPhysPen = MemAlloc(nRet);
    if (!pPhysPen)
        return NULL;
    
    nRet = pDC->pGraphDev->drv.RealizePen(pDC->pGraphDev->pDev, &pPen->logpen, pPhysPen);
    if (nRet <= 0)
    {
        MemFree(pPhysPen);
        return NULL;
    }
    
    return pPhysPen;
}

/*
**  Function : DC_ReleasePhysPen
**  Purpose  :
**      Releases the physical pen. The effect of the function depends on
**      the type of pen object. It destroy only non-static pen object, 
**      It has no effect on the static pen object.
*/
void DC_ReleasePhysPen(PDC pDC, PPENOBJ pPen, PPHYSPEN pPhysPen)
{
    ASSERT(pDC != NULL);
    ASSERT(pPhysPen != NULL);

    /* 如果是静态对象，物理对象不删除 */
    if (WOT_IsStockObj((PXGDIOBJ)pPen))
        return ;

    pDC->pGraphDev->drv.UnrealizePen(pDC->pGraphDev->pDev, pPhysPen);
    MemFree(pPhysPen);

    return;
}

/*
**  Function : DC_GetPhysBrush
**  Purpose  :
**      Gets the physical brush object of a spcified brush object. 
*/
PPHYSBRUSH DC_GetPhysBrush(PDC pDC, PBRUSHOBJ pBrush)
{
    int nRet;
    PPHYSBRUSH pPhysBrush;

    ASSERT(pDC != NULL);
    if (!pBrush)
        return NULL;

    if (WOT_IsStockObj((PXGDIOBJ)pBrush))
        return DC_GetStockPhysObj(pDC, (PXGDIOBJ)pBrush);

    // 调用相应的设备驱动程序生成物理对象并返回
    
    nRet = pDC->pGraphDev->drv.RealizeBrush(pDC->pGraphDev->pDev, 
        &pBrush->logbrush, NULL);
    
    if (nRet <= 0)
        return NULL;
    
    pPhysBrush = MemAlloc(nRet);
    if (!pPhysBrush)
        return NULL;
    
    nRet = pDC->pGraphDev->drv.RealizeBrush(pDC->pGraphDev->pDev, 
        &pBrush->logbrush, pPhysBrush);

    if (nRet <= 0)
    {
        MemFree(pPhysBrush);
        return NULL;
    }
    
    return pPhysBrush;
}

/*
**  Function : DC_ReleasePhysBrush
**  Purpose  :
**      Releases the physical brush. The effect of the function debrushds on
**      the type of brush object. It destroy only non-static brush object, 
**      It has no effect on the static brush object.
*/
void DC_ReleasePhysBrush(PDC pDC, PBRUSHOBJ pBrush, PPHYSBRUSH pPhysBrush)
{
    ASSERT(pDC != NULL);
    ASSERT(pPhysBrush != NULL);

    if (WOT_IsStockObj((PXGDIOBJ)pBrush))
        return;

    pDC->pGraphDev->drv.UnrealizeBrush(pDC->pGraphDev->pDev, pPhysBrush);
    MemFree(pPhysBrush);
    
    return;

}

/*
**  Function : DC_GetFontDev
**  Purpose  :
**      Gets the font device of a spcified font object. 
*/
PFONTDEV DC_GetFontDev(PDC pDC, PFONTOBJ pFont)
{
//    ASSERT(pDC != NULL);
    if (pFont == NULL)
        return NULL;
    
    // 如果是静态对象, 物理对象已经生成, 直接返回相应的物理对象
    if (WOT_IsStockObj((PXGDIOBJ)pFont))
        return DC_GetStockPhysObj(pDC, (PXGDIOBJ)pFont);
    
    // 不是静态对象, 调用相应程序生成物理对象并返回
    return FONT_CreateFontDev(pFont);
}

/*
**  Function : DC_ReleaseFontDev
**  Purpose  :
**      Releases the physical brush. The effect of the function debrushds on
**      the type of brush object. It destroy only non-static brush object, 
**      It has no effect on the static brush object.
*/

void DC_ReleaseFontDev(PDC pDC, PFONTOBJ pFont, PFONTDEV pFontDev)
{
//    ASSERT(pDC != NULL);
    ASSERT(pFontDev != NULL);
    
    // 如果不是静态对象, 删除相应的物理对象. 静态对象的物理对象不能删除.
    // 空对象表示已经删除的非静态对象.
    if (WOT_IsStockObj((PXGDIOBJ)pFont))
        return;
    
    FONT_DestroyFontDev(pFont, pFontDev);

    return;

}
/*
**  Function : DC_GetPhysColor
**  Purpose  :
**      Gets the physical color of a specified logical color.
*/
DWORD DC_GetPhysColor(PDC pDC, COLORREF color)
{
    return pDC->pGraphDev->drv.RealizeColor(pDC->pGraphDev->pDev, NULL, color);
}

//*************************************************************************
//              Internal function prototypes
//*************************************************************************

/*
**  Function : SetDCItemsToDefault
**  Purpose  :
**      Sets DC items value to default value.
*/
static void SetDCItemsToDefault(PDC pDC)
{
    PGDIOBJ pGdiObj;

    pDC->nBkMode        = BM_OPAQUE;
    pDC->BkColor        = RGB(255, 255, 255);
    pDC->TextColor      = RGB(0, 0, 0);
    pDC->BkPhysColor    = DC_GetPhysColor(pDC, pDC->BkColor);
    pDC->TextPhysColor  = DC_GetPhysColor(pDC, pDC->TextColor);
    pDC->uTextAlignMode = 0;

    if (!pDC->pPhysPen)
    {
        ASSERT(!pDC->pPhysBrush);
        ASSERT(!pDC->pFontDev);

        // 物理GDI对象为NULL, 表示本次调用为初始化DC, 初始化每个GDI对象

        pDC->pPen       = (PPENOBJ)WOT_GetStockObj(BLACK_PEN);
        pDC->pPhysPen   = DC_GetPhysPen(pDC, pDC->pPen);
        pDC->PenColor   = RGB(0, 0, 0);
        pDC->PenPhysColor = DC_GetPhysColor(pDC, RGB(0, 0, 0));

        pDC->pBrush     = (PBRUSHOBJ)WOT_GetStockObj(WHITE_BRUSH);
        pDC->pPhysBrush = DC_GetPhysBrush(pDC, pDC->pBrush);
        pDC->BrushColor   = RGB(255, 255, 255);
        pDC->BrushPhysColor = DC_GetPhysColor(pDC, RGB(255, 255, 255));

        pDC->pFont      = (PFONTOBJ)WOT_GetStockObj(SYSTEM_FONT);
        pDC->pFontDev   = DC_GetFontDev(pDC, pDC->pFont);
    }
    else
    {
        // 物理GDI对象不为NULL, 表示本次调用为重新设置DC的GDI对象为默认
        // GDI对象

        pGdiObj = (PGDIOBJ)WOT_GetStockObj(BLACK_PEN);
        DC_SelectObject(pDC, (PXGDIOBJ)pGdiObj);

        pGdiObj = (PGDIOBJ)WOT_GetStockObj(WHITE_BRUSH);
        DC_SelectObject(pDC, (PXGDIOBJ)pGdiObj);

        pGdiObj = (PGDIOBJ)WOT_GetStockObj(SYSTEM_FONT);
        DC_SelectObject(pDC, (PXGDIOBJ)pGdiObj);
    }

    pDC->nBrushOrgX     = 0;
    pDC->nBrushOrgY     = 0;

    pDC->bClip          = FALSE;
    pDC->nRop           = ROP_SRC;
    pDC->nCurPosX       = 0;
    pDC->nCurPosY       = 0;
	pDC->nWndOrgX		= 0;
	pDC->nWndOrgY		= 0;
	pDC->nWndExtX		= 1;
	pDC->nWndExtY		= 1;
	pDC->nVportOrgX		= 0;
	pDC->nVportOrgY		= 0;
	pDC->nVportExtX		= 1;
	pDC->nVportExtY		= 1;
	pDC->nMapMode		= MM_TEXT;
    pDC->nArcDirection  = AD_COUNTERCLOCKWISE;
}

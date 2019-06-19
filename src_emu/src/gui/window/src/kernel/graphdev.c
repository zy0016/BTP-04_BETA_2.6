/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 实现图形设备的一般操作
 *            
\**************************************************************************/

#include "hpwin.h"

#include "wsobj.h"

/* Header files used to define stock object */
#include "wsopen.h"
#include "wsobrush.h"
#include "wsobmp.h"
#include "wsofont.h"

#include "graphdev.h"
#include "devdisp.h"

#if (PRINTERSUPPORT)
#include "devprn.h"
#endif

#include "string.h"

static PGRAPHDEVITEM    g_pGraphDevHead;

static PGRAPHDEVITEM    AddGraphDev(void);
static BOOL             RemoveGraphDev(PGRAPHDEVITEM pItem);
static PGRAPHDEVITEM    GetGraphDev(HGRAPHDEV handle);
static BOOL             CreateStockPhysObj(PGRAPHDEV pGraphDev);

/*********************************************************************\
* Function	   GRAPHDEV_Init
* Purpose      Initial the graphic device such as display and printer
* Params	   
* Return	   
    success: TRUE 
    fail:FALSE
* Remarks	  
    In the future, the initial function of graphic device will use unified
    interface.
**********************************************************************/
BOOL GRAPHDEV_Init(void)
{
    PGRAPHDEVITEM pItem;
    int index = FIRST_GRAPHDEV_INDEX;

    g_pGraphDevHead = NULL;

    while(1)
    {
        pItem = AddGraphDev();
        if (pItem == NULL)
        {
            break;
        }

        pItem->GraphDev.handle = MAKE_GRAPHDEV_HANDLE(DEV_DISPLAY, index);
		//printf("**************************** index = %d\r\n", index);
        if (!DISPDEV_Init(&(pItem->GraphDev)))
        {
			//printf("**************************** failedindex = %d\r\n", index);
            RemoveGraphDev(pItem);
            break;
        }
		//printf("**************************** sucess index = %d\r\n", index);

        CreateStockPhysObj(&(pItem->GraphDev));
        index++;
    }

    if (index == FIRST_GRAPHDEV_INDEX)
        return FALSE;

#if (PRINTERSUPPORT)
    index = FIRST_GRAPHDEV_INDEX;
    while(1)
    {
        pItem = AddGraphDev();
        if (pItem == NULL)
        {
            break;
        }
        pItem->GraphDev.handle = MAKE_GRAPHDEV_HANDLE(DEV_PRINTER, index);
        if (!PRNDEV_Init(&(pItem->GraphDev)))
        {
            RemoveGraphDev(pItem);
            break;
        }
        
        CreateStockPhysObj(&(pItem->GraphDev));
        index++;
    }
#endif

    return TRUE;
}

/*********************************************************************\
* Function	    GRAPHDEV_Exit
* Purpose       Exit the graphic devices.      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL GRAPHDEV_Exit(void)
{
    PGRAPHDEVITEM pItem;

    pItem = GetGraphDev(MAKE_GRAPHDEV_HANDLE(DEV_DISPLAY, 0));
    ASSERT(pItem != NULL);
    DISPDEV_Exit(&(pItem->GraphDev));
    RemoveGraphDev(pItem);

#if (PRINTERSUPPORT)

    pItem = GetGraphDev(MAKE_GRAPHDEV_HANDLE(DEV_PRINTER, 0));
    ASSERT(pItem != NULL);
    PRNDEV_Exit(&(pItem->GraphDev));
    RemoveGraphDev(pItem);

#endif

    return TRUE;
}

/*********************************************************************\
* Function	    GRAPHDEV_GetGraphDev
* Purpose       Get the graphic device.
* Params	   
    handle: the handle of the graphic device
* Return	 	   
    the only graphic device.
* Remarks	   
**********************************************************************/
PGRAPHDEV GRAPHDEV_GetGraphDev(HGRAPHDEV handle)
{
    PGRAPHDEVITEM pItem;
    
//printf("^^^^^^^^^^^^^^^handle : %d\r\n", handle);
    pItem = GetGraphDev(handle);
    if (pItem == NULL)
        return NULL;

    return &(pItem->GraphDev);
}

/*********************************************************************\
* Function	    GRAPHDEV_GetDefault
* Purpose       Get the default graphic device when pDC is NULL.
* Params	   
    type: the type of the graphic device such as DEV_DISPLAY or DEV_PRINTER
* Return	 	   
    the only graphic device.
* Remarks	   
**********************************************************************/
PGRAPHDEV GRAPHDEV_GetDefault(LONG type)
{
    HGRAPHDEV handle;
#ifdef PIP_TRANS
    handle = MAKE_GRAPHDEV_HANDLE(type, FIRST_GRAPHDEV_INDEX + 1);
#else
    handle = MAKE_GRAPHDEV_HANDLE(type, FIRST_GRAPHDEV_INDEX);
    
#endif
    
    return GRAPHDEV_GetGraphDev(handle);
}

/*********************************************************************\
* Function	    GRAPHDEV_GetCursorScreen
* Purpose       Get the screen graphic device that have cursor.
* Params	   
* Return	 	   
    the only graphic device.
* Remarks	   
**********************************************************************/
PGRAPHDEV GRAPHDEV_GetCursorScreen(int nScreen)
{
    HGRAPHDEV handle;
#ifdef PIP_TRANS
    handle = MAKE_GRAPHDEV_HANDLE(DEV_DISPLAY, FIRST_GRAPHDEV_INDEX + 1);
#else
    handle = MAKE_GRAPHDEV_HANDLE(DEV_DISPLAY, FIRST_GRAPHDEV_INDEX);
    
#endif
    
    return GRAPHDEV_GetGraphDev(handle);
}


/*********************************************************************\
* Function	    GRAPHDEV_GetStockPhysObj
* Purpose       Get the stock physical object.
* Params	   
    pGraphDev:  The device
    uIndex:     The index according with the logical stock object.
* Return	 	   
* Remarks	   
**********************************************************************/
void *GRAPHDEV_GetStockPhysObj(PGRAPHDEV pGraphDev, UINT uIndex)
{
    return pGraphDev->StockPhysObjTable[uIndex].pPhysObj;
}

/*Interner function */

/*********************************************************************\
* Function	    AddGraphDev
* Purpose       Add a new graphic device and return the pointer
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static PGRAPHDEVITEM AddGraphDev(void)
{
    PGRAPHDEVITEM pCur, pPrev, pItem;

    pItem = (PGRAPHDEVITEM)MemAlloc(sizeof(GRAPHDEVITEM));
    if (pItem == NULL)
        return NULL;

    MemSet(pItem, 0, sizeof(GRAPHDEVITEM));

    if (g_pGraphDevHead == NULL)
    {
        g_pGraphDevHead = pItem;
        return pItem;
    }

    pPrev = g_pGraphDevHead;
    pCur = g_pGraphDevHead->pNext;
    while (pCur != NULL)
    {
        pPrev = pCur;
        pCur = pCur->pNext;
    }

    pPrev->pNext = pItem;
    return pItem;
}

/*********************************************************************\
* Function	    RemoveGraphDev
* Purpose       Remove a graphic device.
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL RemoveGraphDev(PGRAPHDEVITEM pItem)
{
    PGRAPHDEVITEM pCur;

    if (g_pGraphDevHead == pItem)
    {
        g_pGraphDevHead = pItem->pNext;
        MemFree(pItem);
        return TRUE;
    }

    pCur = g_pGraphDevHead;
    while (pCur->pNext != NULL)
    {
        if (pCur->pNext == pItem)
        {
            pCur->pNext = pItem->pNext;
            MemFree(pItem);
            return TRUE;
        }

        pCur = pCur->pNext;
    }

    return FALSE;
}

/*********************************************************************\
* Function	    GetGraphDev
* Purpose       Get the graphic device.
* Params	   
    type: the type of the graphic device such as DEV_DISPLAY or DEV_PRINTER
* Return	 	   
* Remarks	   
**********************************************************************/
static PGRAPHDEVITEM GetGraphDev(HGRAPHDEV handle)
{
    PGRAPHDEVITEM pCur;

	//printf("^^^^^^^^^^^^^^^handle : %d\r\n", handle);

    pCur = g_pGraphDevHead;
    while (pCur != NULL)
    {
        if (pCur->GraphDev.handle == handle)
        {
            return pCur;
        }

        pCur = pCur->pNext;
    }

    return NULL;
}

/*********************************************************************\
* Function	    CreateStockPhysObj
* Purpose       Create the stock physical object
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL CreateStockPhysObj(PGRAPHDEV pGraphDev)
{
    int i;
    int nRet;
    PXGDIOBJ pObj;
    PPHYSPEN pPhysPen;
    PPHYSBRUSH pPhysBrush;
    PGRAPHDEVDRV pDrv = &(pGraphDev->drv);

    for (i = 0; i < MAX_STOCK_OBJS; i++)
    {
        pObj = WOT_GetStockObj(i);
        ASSERT(pObj != NULL);

        switch (WOT_GetObjType((PGDIOBJ)pObj))
        {
        case OBJ_PEN :
            
            nRet = pDrv->RealizePen(pGraphDev->pDev, 
                &((PPENOBJ)pObj)->logpen, NULL);
            if (nRet < 0)
            {
                ASSERT(0);
                return FALSE;
            }

            pPhysPen = MemAlloc(nRet);
            if (pPhysPen == NULL)
            {
                ASSERT(0);
                return FALSE;
            }
            
            nRet = pDrv->RealizePen(pGraphDev->pDev, 
                &((PPENOBJ)pObj)->logpen, pPhysPen);
            if (nRet < 0)
            {
                ASSERT(0);
                MemFree(pPhysPen);
                return FALSE;
            }

            ASSERT(pPhysPen != NULL);
            
            pGraphDev->StockPhysObjTable[i].pPhysObj = pPhysPen;

            break;
            
        case OBJ_BRUSH :
            
            nRet = pDrv->RealizeBrush(pGraphDev->pDev,
                &((PBRUSHOBJ)pObj)->logbrush, NULL);
            if (nRet < 0)
            {
                ASSERT(0);
                return FALSE;
            }

            pPhysBrush = MemAlloc(nRet);
            if (pPhysBrush == NULL)
            {
                ASSERT(0);
                return FALSE;
            }

            nRet = pDrv->RealizeBrush(pGraphDev->pDev,
                &((PBRUSHOBJ)pObj)->logbrush, pPhysBrush);
            if (nRet < 0)
            {
                ASSERT(nRet > 0);
                MemFree(pPhysBrush);
                return FALSE;
            }

            ASSERT(pPhysBrush != NULL);

            pGraphDev->StockPhysObjTable[i].pPhysObj = pPhysBrush;

            break;

/*对于字体和位图等其他静态对象，使用全局的对象，不单独创建*/
        case OBJ_FONT :
            
            pGraphDev->StockPhysObjTable[i].pPhysObj = 
                FONT_CreateFontDev((PFONTOBJ)pObj);

            ASSERT(pGraphDev->StockPhysObjTable[i].pPhysObj != NULL);

            break;
            
        case OBJ_BITMAP : 
            
            // The default bitmap is a mono bitmap, needn't to be realized
            // to a physical bitmap
            pGraphDev->StockPhysObjTable[i].pPhysObj = NULL;
            
            break;
        }
    }

    return TRUE;
}

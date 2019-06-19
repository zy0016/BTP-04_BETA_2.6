 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Graphic device management
 *            
\**************************************************************************/

#ifndef __GRAPHDEV_H
#define __GRAPHDEV_H
#include "display.h"

#if (PRINTERSUPPORT)
#include "printer.h"
#endif

/* Define the device type const */
#define DEV_DISPLAY     0           /* display device               */
#define DEV_MONOMEM     1           /* mono memory display device   */
#define DEV_PRINTER     2           /* printer device               */

typedef DWORD   HGRAPHDEV;
#define GET_GRAPHDEV_INDEX(handle)          ((handle) & 0x0000FFFFL)
#define GET_GRAPHDEV_TYPE(handle)           (((handle) & 0xFFFF0000L) >> 16)
#define DEFAULT_GRAPHDEV_INDEX   0
#ifdef PIP_TRANS
#define FIRST_GRAPHDEV_INDEX     0
#else
#define FIRST_GRAPHDEV_INDEX     1
#endif
#define MAKE_GRAPHDEV_HANDLE(type, index)   ((DWORD)(type) << 16 | (index))

#define MAX_STOCK_OBJS 16

typedef struct {
    void*   pPhysObj;   // physical object of gdi object
} STATICPHYSOBJ, *PSTATICPHYSOBJ;

/* Define graphic device driver interface   */
typedef struct tagGraphDevDrv{
    int32 devices;
    int32 func_mask1;
    int32 func_mask2;
    int   (*OpenDevice)(PDEV pDev, int dev);
    int   (*CloseDevice)(PDEV pDev);
    int   (*SuspendDevice)(PDEV pDev);
    int   (*ResumeDevice)(PDEV pDev);
    int   (*GetDevInfo)(PDEV pDev, PDISPLAYDEVINFO pInfo);
    int   (*EnumMode)(PDEV pDev, int nModeNum, PDISPMODE pDispMode);
    int   (*ChangeMode)(PDEV pDev, PDISPMODE pDispMode, uint32 flags);
    int   (*CreateMemoryDevice)(PDEV pDev, int width, int height,
                                PMEMDEV pMemDev);
    int   (*DestroyMemoryDevice)(PDEV pMemDev);
    int   (*CreateBitmap)(PDEV pDev, int width, int height, 
                          PDDBITMAP pBitmap);
    int   (*DestroyBitmap)(PDEV pDev, PDDBITMAP pBitmap);
    int   (*SetBitmapBuffer)(PDEV pDev, PDDBITMAP pBitmap, void* pBuffer);
    int   (*RealizePen)(PDEV pDev, PDRVLOGPEN pLogPen, PPHYSPEN pPhysPen);
    int   (*UnrealizePen)(PDEV pDev, PPHYSPEN pPen);
    int   (*RealizeBrush)(PDEV pDev, PDRVLOGBRUSH pLogBrush, 
                                    PPHYSBRUSH pPhysBrush);
    int   (*UnrealizeBrush)(PDEV pDev, PPHYSBRUSH pBrush);
    int   (*RealizePalette)(PDEV pDev, PDRVLOGPALETTE pLogPalette,
                            PPHYSPALETTE pPhysPalette);
    int   (*UnrealizePalette)(PDEV pDev, PPHYSPALETTE pPalette);
    int   (*RealizeColor)(PDEV pDev, PPHYSPALETTE pPalette, int32 color);
    int   (*SelectPalette)(PDEV pDev, PPHYSPALETTE pPalette);
    int   (*DrawGraph)(PDEV pDev, int func, PGRAPHPARAM pGraphPara, 
                       void* pDrawData);
    int   (*BitBlt)(PDEV pDev, PRECT pDstRect, PDEV pSrcDev,
                    int x, int y, PGRAPHPARAM pGraphParam);
    int   (*PatBlt)(PDEV pDev, PRECT pRect, PGRAPHPARAM pGraphParam);
    int   (*DIBitBlt)(PDEV pDev, PRECT pDestRect, PDIBITMAP pBitmap,
                      int x, int y, PGRAPHPARAM pGraphParam);
    int   (*TextBlt)(PDEV pDev, int x, int y, PTEXTBMP pTextBmp, 
                     PGRAPHPARAM pGraphParam);
    int   (*StretchBlt)(PDEV pDev, PRECT pDstRect, PDEV pSrcDev, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam);
    int   (*DIStretchBlt)(PDEV pDev, PRECT pDstRect, PDIBITMAP pBitMap, 
                        PRECT pSrcRect, PGRAPHPARAM pGraphParam);
/* only in display driver interface */
    int   (*RealizeCursor)(PDEV pDev, PDRVLOGCURSOR pLogCursor,
                           PPHYSCURSOR pPhysCursor);
    int   (*UnrealizeCursor)(PDEV pDev, PPHYSCURSOR pCursor);
    int   (*ShowCursor)(PDEV pDev, PPHYSCURSOR pCursor, int mode);
    int   (*SetCursorPos)(PDEV pDev, int x, int y);
    int   (*CheckCursor)(PDEV pDev);
    int   (*UpdateScreen)(PDEV pDev);
    int   (*SaveScreen)(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect);
    int   (*RestoreScreen)(PDEV pDev, PSCREENSAVE pScreenSave, PRECT pRect);
    int   (*ScrollScreen)(PDEV pDev, PRECT pRect, PRECT pClipRect, 
                          int x, int y);
/*only in printer driver interface */
    int   (*SetPageSize)(PDEV pDev, int width, int height);
    int   (*StartPrint)(PDEV pDev);
    int   (*EndPrint)(PDEV pDev);
    int   (*StartPage)(PDEV pDev);
    int   (*EndPage)(PDEV pDev);
    int   (*StartBand)(PDEV pDev, PRECT pRect);
    int   (*EndBand)(PDEV pDev);
    int   (*ResumePrint)(PDEV pDev);
    int   (*CancelPrint)(PDEV pDev);
    int   (*SetCallback)(PDEV pDev, uint32 pfnCallback);
}GRAPHDEVDRV, *PGRAPHDEVDRV;

typedef struct tagGraphDev
{
    /*设备类型*/
    HGRAPHDEV handle;
    /*这是设备接口函数表结构的指针，今后考虑使用标准结构，统一成标准的驱动
    **接口结构，保证显示、打印对窗口透明
    */
//    void*   pDrv;
    GRAPHDEVDRV drv;
    /*设备一般信息，如颜色位，模式，FB地址，内部使用
    */
    void*   pDev;
    /*设备一般信息，外部获取*/
    void*   pDevInfo;
    /*设备对应的静态物理对象表
    */
    STATICPHYSOBJ StockPhysObjTable[MAX_STOCK_OBJS];
}GRAPHDEV, *PGRAPHDEV;

typedef struct tagGraphDevItem 
{
    struct tagGraphDevItem *pNext;
    GRAPHDEV GraphDev;
}GRAPHDEVITEM, *PGRAPHDEVITEM;

BOOL        GRAPHDEV_Init(void);
BOOL        GRAPHDEV_Exit(void);
PGRAPHDEV   GRAPHDEV_GetGraphDev(HGRAPHDEV handle);
PGRAPHDEV   GRAPHDEV_GetDefault(LONG type);
PGRAPHDEV   GRAPHDEV_GetCursorScreen(int nScreen);
void*       GRAPHDEV_GetStockPhysObj(PGRAPHDEV pGraphDev, UINT uIndex);

#endif //__GRAPHDEV_H

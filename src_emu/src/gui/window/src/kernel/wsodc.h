/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Header file for GC object management
 *            
\**************************************************************************/

#ifndef __WSODC_H
#define __WSODC_H

#include "wsobj.h"
#include "wsopen.h"
#include "wsobrush.h"
#include "wsofont.h"
#include "wsobmp.h"
#include "wsorgn.h"

/* Define DC type constant */
#define DC_DEVICE       0           /* device DC                    */
#define DC_BITMAP       1           /* bitmap DC                    */
#define DC_MEMORY       2           /* memory DC                    */
#define DC_WINDOW       3           /* window DC                    */
#define DC_SCREEN       4           /* SCREEN DC                    */

/* Define the device type const */
#define DEV_DISPLAY     0           /* display device               */
#define DEV_MONOMEM     1           /* mono memory display device   */
#define DEV_PRINTER     2           /* printer device               */

/* Define mask value for window DC attrib   */ 
#define WDA_CLIENT      0x0001      /* client DC or Window DC   */
#define WDA_UPDATE      0x0002      /* update DC or whole DC    */

#define ISDISPLAYDC(pDC)    ((pDC)->bDev == DEV_DISPLAY)
#define ISMONOMEMDC(pDC)    ((pDC)->bDev == DEV_MONOMEM)
#define ISPRINTERDC(pDC)    ((pDC)->bDev == DEV_PRINTER)

typedef struct
{
    OBJHDR objhead;
} DCOBJHDR, *PDCOBJHDR;

// Define data struct for DC object
typedef struct tagDC
{
    DCOBJHDR    header;             /* handle of the DC object          */
    DWORD       dwThreadId;
    BYTE        bType;              /* type of the DC object            */
    BYTE        bDev;               /* device type of the DC object     */
    PGRAPHDEV   pGraphDev;          /* */
    WORD        wAttrib;            /* attrib of window DC              */

    int         nArcDirection;      /* Arc direct, added by wwh02/08/02 */
    int         nBkMode;            /* background mode                  */
    COLORREF    BkColor;            /* RGB background color             */
    COLORREF    TextColor;          /* RGB text color(foregound color)  */
    DWORD       BkPhysColor;        /* physical background color        */
    DWORD       TextPhysColor;      /* physical text(foregound color)   */
    UINT        uTextAlignMode;     /* Text aligh mode                  */

//    HANDLE      hPen;               /* current logical pen              */
//    HANDLE      hBrush;             /* current logical brush            */
//    HANDLE      hFont;              /* current logical font             */
//    HANDLE      hrgnClip;           /* current clip region              */
    PPENOBJ     pPen;               /* current logical pen              */
    PBRUSHOBJ   pBrush;             /* current logical brush            */
    PFONTOBJ    pFont;              /* current logical font             */
    PRGNOBJ     prgnClip;           /* current clip region              */
    
    PPHYSPEN    pPhysPen;           /* current physical pen object      */
    PPHYSBRUSH  pPhysBrush;         /* current physical brush object    */
    PFONTDEV    pFontDev;           /* current physical font object     */

    COLORREF    PenColor;         /* support SetDCPenColor */
    COLORREF    BrushColor;       /* support SetDCBrushColor */
    DWORD       PenPhysColor;     /* support SetDCPenColor */
    DWORD       BrushPhysColor;   /* support SetDCBrushColor */
    int         nBrushOrgX;         /* x-coordinate of brush org        */
    int         nBrushOrgY;         /* y-coordinate of brush org        */

    BOOL        bClip;              /* indicate whether rcClip is used  */
    RECT        rcClip;             /* current user defined clip rect   */

    int         nRop;               /* ROP value                        */

    int         nCurPosX;           /* x-coordinate of current position */
    int         nCurPosY;           /* y-coordinate of current position */

    int         nWndOrgX;           /* window origin X                  */
    int         nWndOrgY;           /* window origin Y                  */
    int         nWndExtX;           /* window width                     */
    int         nWndExtY;           /* window height                    */
    int         nVportOrgX;         /* viewport origin X                */
    int         nVportOrgY;         /* viewport origin Y                */
    int         nVportExtX;         /* viewport width                   */
    int         nVportExtY;         /* viewport height                  */
    int         nMapMode;           /* mapping mode                     */

    void*       pDev;               /* Paint device                     */
} DC;

// DC对象指针类型PDC已提前定义在wsobmp.h中, 此处不再定义, 否则在某些编译器
// 商会产生编译错误

// PWINOBJ的提前引用说明
typedef struct tagWINOBJ*   PWINOBJ;

/* Functions for DC object management */

PDC      DC_GetWindowDCFromCache(PWINOBJ pWin, BOOL bClient, BOOL bUpdate);
void     DC_ReleaseWindowDCToCache(PDC pDC);
void     DC_DestroyWindow(PWINOBJ pWin);

PDC      DC_CreateWindowDC(PWINOBJ pWin, BOOL bClient);
PDC      DC_CreateMemoryDC(int width, int height);
PDC      DC_CreateCompatibleDC(PDC pOldDC);
PDC      DC_CreateScreenDC(void);
PDC      DC_CreateDC(LPCTSTR lpszDriver, LPCTSTR lpszDevice, 
                     LPCTSTR lpszOutput, const void* lpInitData);
void     DC_Destroy(PDC pDC);

/**/
PPHYSPEN    DC_GetPhysPen(PDC pDC, PPENOBJ pPen);
void        DC_ReleasePhysPen(PDC pDC, PPENOBJ pPen, PPHYSPEN pPhysPen);
PPHYSBRUSH  DC_GetPhysBrush(PDC pDC, PBRUSHOBJ pBrush);
void        DC_ReleasePhysBrush(PDC pDC, PBRUSHOBJ pBrush, PPHYSBRUSH pPhysBrush);
PFONTDEV    DC_GetFontDev(PDC pDC, PFONTOBJ pFont);
void        DC_ReleaseFontDev(PDC pDC, PFONTOBJ pFont, PFONTDEV pFontDev);
DWORD       DC_GetPhysColor(PDC pDC, COLORREF color);
void*       DC_GetStockPhysObj(PDC pDC, PXGDIOBJ pObj);

#if (PRINTERSUPPORT)
PDC      DC_CreatePrinterDC(int width, int height);
int      DC_StartDoc(PDC pDC, const DOCINFO* pDocInfo);
int      DC_EndDoc(PDC pDC);
int      DC_StartPage(PDC pDC);
int      DC_EndPage(PDC pDC);
int      DC_StartPage(PDC pDC);
int      DC_EndPage(PDC pDC);
int      DC_StartBand(PDC pDC, PRECT pRect);
int      DC_EndBand(PDC pDC);
#endif // PRINTERSUPPORT

BOOL     DC_IsWindowDC(PDC pDC);
PWINOBJ  DC_GetWindow(PDC pDC);
void*    DC_GetDevice(PDC pDC);
void     DC_ReleaseDevice(PDC pDC);
int      DC_GetPaintArea(PDC pDC);

int      DC_GetDeviceCaps(PDC pDC, int nIndex);

PXGDIOBJ DC_SelectObject(PDC pDC, PXGDIOBJ pObj);

COLORREF DC_SetTextColor(PDC pDC, COLORREF color);
COLORREF DC_GetTextColor(PDC pDC);

COLORREF DC_SetPenColor(PDC pDC, COLORREF color);
COLORREF DC_GetPenColor(PDC pDC);
COLORREF DC_SetBrushColor(PDC pDC, COLORREF color);
COLORREF DC_GetBrushColor(PDC pDC);
COLORREF DC_SetBkColor(PDC pDC, COLORREF color);
COLORREF DC_GetBkColor(PDC pDC);

UINT     DC_SetTextAlign(PDC pDC, UINT uMode);
UINT     DC_GetTextAlign(PDC pDC);

int      DC_SetBkMode(PDC pDC, int nBkMode);
int      DC_GetBkMode(PDC pDC);

int      DC_SetROP2(PDC pDC, int nRop);
int      DC_GetROP2(PDC pDC);

void     DC_GetBrushOrg(PDC pDC, PPOINT pPoint);
void     DC_SetBrushOrg(PDC pDC, int nOrgX, int nOrgY, PPOINT pPoint);

void     DC_GetCurPos(PDC pDC, PPOINT pPoint);
void     DC_SetCurPos(PDC, int x, int y, PPOINT pPoint);

void     DC_GetClipRect(PDC pDC, PRECT prcClip);
void     DC_SetClipRect(PDC pDC, const RECT* prcClip);

void     DC_GetWindowOrg(PDC pDC, PPOINT pPoint);
void     DC_SetWindowOrg(PDC pDC, int x, int y, PPOINT pPoint);

void     DC_GetWindowExt(PDC pDC, PSIZE pSize);
void     DC_SetWindowExt(PDC pDC, int nExtX, int nExtY, PSIZE pSize);

void     DC_GetViewportOrg(PDC pDC, PPOINT pPoint);
void     DC_SetViewportOrg(PDC pDC, int x, int y, PPOINT pPoint);

void     DC_GetViewportExt(PDC pDC, PSIZE pSize);
void     DC_SetViewportExt(PDC pDC, int nExtX, int nExtY, PSIZE pSize);

void     DC_DPtoLP(PDC pDC, PPOINT pPoints, int nCount);
void     DC_LPtoDP(PDC pDC, PPOINT pPoints, int nCount);

int      DC_SetArcDirection(PDC pDC,int ArcDirection);
int      DC_GetArcDirection(PDC pDC);

int      DC_GetMapMode(PDC pDC);
int      DC_SetMapMode(PDC pDC, int nMapMode);

void     DC_GetClipRect(PDC pDC, PRECT prcClip);
void     DC_SetClipRect(PDC pDC, const RECT* prcClip);

void     DC_GetTextMetrics(PDC pDC, PTEXTMETRIC ptm);

// clipping support

int      DC_SelectClipRgn(PDC pDC, PRGNOBJ prgnObj, int nMode);
int      DC_GetClipBox(PDC pDC, RECT* pRect);
       
int      DC_IntersectClipRect(PDC pDC, int x1, int y1, int x2, int y2);
int      DC_OffsetClipRgn(PDC pDC, int x, int y);
int      DC_ExcludeClipRect(PDC pDC, int x1, int y1, int x2, int y2);

BOOL     DC_PtVisible(PDC pDC, int x, int y);
BOOL     DC_RectVisible(PDC pDC, const RECT* pRect);

// two function for internal use
void     DC_SetItemsToDefault(PDC pDC);
void     DC_ToGraphParam(PDC pDC, PGRAPHPARAM pGraphParam);

#endif //__WSODC_H

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements window system graph functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"

#include "wsobj.h"
#include "wsownd.h"
#include "wsobmp.h"
#include "font.h"
#include "wsgraph.h"

#if (PRINTERSUPPORT)
#include "devprn.h"
#endif // PRINTERSUPPORT

// 定义320x16的文本位图结构，用于定义输出文本时的临时文本位图，可以在文本
// 长度小于40(针对16点阵字体而言)时避免内存分配和释放，从而加速文本的显示，
// 提高文本输出的性能
typedef struct
{
    int16   width;
    int16   height;
    int32   data[160];
} TMPTEXTBMP;


static double cosine[91] =
{
	1.000000, 
	0.999848, 
	0.999391, 
	0.998630, 
	0.997564, 
	0.996195, 
	0.994522, 
	0.992546, 
	0.990268, 
	0.987688, 
	0.984808, 
	0.981627, 
	0.978148, 
	0.974370, 
	0.970296, 
	0.965926, 
	0.961262, 
	0.956305, 
	0.951057, 
	0.945519, 
	0.939693, 
	0.933580, 
	0.927184, 
	0.920505, 
	0.913545, 
	0.906308, 
	0.898794, 
	0.891007, 
	0.882948, 
	0.874620, 
	0.866025, 
	0.857167, 
	0.848048, 
	0.838671, 
	0.829038, 
	0.819152, 
	0.809017, 
	0.798636, 
	0.788011, 
	0.777146, 
	0.766044, 
	0.754710, 
	0.743145, 
	0.731354, 
	0.719340, 
	0.707107, 
	0.694658, 
	0.681998, 
	0.669131, 
	0.656059, 
	0.642788, 
	0.629320, 
	0.615661, 
	0.601815, 
	0.587785, 
	0.573576, 
	0.559193, 
	0.544639, 
	0.529919, 
	0.515038, 
	0.500000, 
	0.484810, 
	0.469472, 
	0.453990, 
	0.438371, 
	0.422618, 
	0.406737, 
	0.390731, 
	0.374607, 
	0.358368, 
	0.342020, 
	0.325568, 
	0.309017, 
	0.292372, 
	0.275637, 
	0.258819, 
	0.241922, 
	0.224951, 
	0.207912, 
	0.190809, 
	0.173648, 
	0.156434, 
	0.139173, 
	0.121869, 
	0.104528, 
	0.087156, 
	0.069756, 
	0.052336, 
	0.034899, 
	0.017452, 
    0.000000
};

// Internal function prototypes
static int _GetStringExt(PFONTDEV pFontDev, PCSTR pStr, int count, 
                         PTEXTBMP pTextBmp);
static BOOL PreDrawWindowDC(PDC pDestDC, PPOINT pptOrg,
                            PEXPOSETBL *ppExposeTbl, PGRAPHPARAM pGraphParam);

typedef struct tagFrameRect
{
    BOOL bUsed;
    BOOL bShow;
    WORD width;
    RECT rc;
    RECT rcClip;
    PBRUSHOBJ pBrush;
    WORD x_BrushOrg;
    WORD y_BrushOrg;
}FRAMERECTDATA, *PFRAMERECTDATA;

static FRAMERECTDATA g_FrameRectData;
//static PBRUSHOBJ g_pFrameBrush;

/* HideFrameRect 和ShowFrameRect必须成对调用 */
static void HideFrameRect(PDC pDC, const RECT* pRect)
{
    RECT rc;

    if (!IntersectRect(&rc, &g_FrameRectData.rcClip, pRect))
        return;

    if (g_FrameRectData.bUsed && g_FrameRectData.bShow && 
        IntersectRect(&rc, &rc, &g_FrameRectData.rc))
    {
        g_FrameRectData.bShow = FALSE;
        WS_DrawDragFrame(pDC, &g_FrameRectData.rc, &rc);
    }
}

static void ShowFrameRect(PDC pDC, const RECT* pRect)
{
    /* 只有当HideFrameRect真正被调用到，才调用该函数 */
    if (g_FrameRectData.bUsed && !g_FrameRectData.bShow)
    {
        g_FrameRectData.bShow = TRUE;
        WS_DrawDragFrame(pDC, &g_FrameRectData.rc, pRect);
    }
}

#define HIDEFRAMERECT(pDC, pRect)   HideFrameRect((pDC), (pRect))
#define SHOWFRAMERECT(pDC, pRect)   ShowFrameRect((pDC), (pRect))

void WS_EndDrag(void)
{
    g_FrameRectData.bUsed = FALSE;
    g_FrameRectData.width = 0;
    g_FrameRectData.pBrush = NULL;
    g_FrameRectData.bShow = FALSE;
}

BOOL WS_StartDrag(PBRUSHOBJ pBrush, int width)
{
    if (g_FrameRectData.pBrush != NULL)
        return FALSE;

    g_FrameRectData.bShow = TRUE;
    g_FrameRectData.width = width;
    g_FrameRectData.pBrush = pBrush;
    g_FrameRectData.x_BrushOrg = 0;
    g_FrameRectData.y_BrushOrg = 0;
    return TRUE;
}

void WS_DrawDragFrame(PDC pDC, const RECT* pRect, const RECT *pRectClip)
{
    PEXPOSETBL  pExposeTbl;
    PWINOBJ     pWin;
    PDEV        pDev;
    RECT        rcClip, rect, rcFrame;
    GRAPHPARAM  graph_param;
    PBRUSHOBJ   pBrush;
    POINT       ptOrg;
    int nWidth, nHeight, width;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    ASSERT(pRect != NULL);

    if (!DC_IsWindowDC(pDC))
        return;

    if (g_FrameRectData.pBrush == NULL ||
        g_FrameRectData.width == 0)
        return;

    if (pRectClip == NULL)
    {
        g_FrameRectData.bUsed = !g_FrameRectData.bUsed;
    }
    else if (g_FrameRectData.bUsed == FALSE)
    {
        return;
    }

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;
    
    rect = *pRect;
    pBrush = g_FrameRectData.pBrush;
    width = g_FrameRectData.width;

    // convert the logical coordinate to the device coordinate
    if (pRectClip == NULL)
        DC_LPtoDP(pDC, (PPOINT)&rect, 2);

    nWidth = rect.right - rect.left;
    nHeight = rect.bottom - rect.top;

    if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
        goto RET;

    // 将绘图参数的坐标转换为屏幕坐标
    if (pRectClip == NULL)
    {
        /* 窗口设备, 首先获得窗口DC对应的窗口对象 */
        pWin = DC_GetWindow(pDC);
        ASSERT(pWin != NULL);

        OffsetRect(&rect, ptOrg.x, ptOrg.y);
        g_FrameRectData.rc = rect;
        WND_GetRect(pWin, &g_FrameRectData.rcClip, W_CLIENT, XY_CLIENT);
        DC_LPtoDP(pDC, (PPOINT)&g_FrameRectData.rcClip, 2);
        OffsetRect(&g_FrameRectData.rcClip, ptOrg.x, ptOrg.y);
    }
    rect = g_FrameRectData.rc;
    {
        graph_param.brush_org_x = 0;
        graph_param.brush_org_y = 0;
    }
    /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
    /* 裁减区间                                                 */
//        rcClip = graph_param.clip_rect;
    IntersectRect(&rcClip, &graph_param.clip_rect, &g_FrameRectData.rcClip);
    if (pRectClip && !IntersectRect(&rcClip, &rcClip, pRectClip))
        goto RET;

    /* 将绘图参数中的刷子换为指定的刷子 */
    /**/
    //graph_param.pBrush = BRUSH_CreatePhysObj(pBrush);

    graph_param.pBrush = DC_GetPhysBrush(pDC, pBrush);
    graph_param.rop = ROP_SRC_XOR_DST;

    /* 对于每一个暴露块调用一次显示设备的绘图操作 */
    if (0)//pRectClip == NULL)
    {
        if (!IsRectEmpty(&graph_param.clip_rect))
        {
            /* 调用显示设备驱动程序绘图函数画四条边 */
            
            // Top line
            SetRect(&rcFrame, rect.left, rect.top, 
                rect.right, rect.top + width);
            pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
            
            // Right line
            SetRect(&rcFrame, rect.right - width, rect.top + width, 
                rect.right, rect.bottom - width);
            pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
            
            // Bottom line
            SetRect(&rcFrame, rect.left, rect.bottom - width, 
                rect.right, rect.bottom);
            pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
            
            // Left line
            SetRect(&rcFrame, rect.left, rect.top + width, 
                rect.left + width, rect.bottom - width);
            pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
        }
    }
    else
    {
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                /* 调用显示设备驱动程序绘图函数画四条边 */
                
                // Top line
                SetRect(&rcFrame, rect.left, rect.top, 
                    rect.right, rect.top + width);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
                
                // Right line
                SetRect(&rcFrame, rect.right - width, rect.top + width, 
                    rect.right, rect.bottom - width);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
                
                // Bottom line
                SetRect(&rcFrame, rect.left, rect.bottom - width, 
                    rect.right, rect.bottom);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
                
                // Left line
                SetRect(&rcFrame, rect.left, rect.top + width, 
                    rect.left + width, rect.bottom - width);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
            }
        }
    }

    // 物理刷子是在本程序生成的, 释生成的物理刷子
    /**/
    //BRUSH_DestroyPhysObj(pBrush, graph_param.pBrush);
    DC_ReleasePhysBrush(pDC, pBrush, graph_param.pBrush);

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
    return;
}

/*
**  Function : WS_DrawLine
*/
void WS_LineTo(PDC pDC, int x, int y)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    LINEDATA    line_data;
    POINT       ptCur, ptOrg;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    // Gets the start point of the line and set current point to 
    // (x2, y2)
    DC_SetCurPos(pDC, x, y, &ptCur);

    line_data.x1 = ptCur.x;
    line_data.y1 = ptCur.y;
    line_data.x2 = x;
    line_data.y2 = y;

    // Coverts logical coordinate to device coordinate
    DC_LPtoDP(pDC, (PPOINT)&line_data, 2);

    if (!DC_IsWindowDC(pDC))
    {
        /* 内存设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.DrawGraph(pDev, 
            DG_LINE, &graph_param, &line_data);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        line_data.x1 += ptOrg.x;
        line_data.x2 += ptOrg.x;
        line_data.y1 += ptOrg.y;
        line_data.y2 += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect, 
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, 
                    DG_LINE, &graph_param, &line_data);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_DrawRect
**  Purpose  :
**      Draw a specified rectangle.
*/
void WS_DrawRect(PDC pDC, const RECT* pRect)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    RECTDATA    rect_data;
    POINT       ptOrg;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    ASSERT(pRect != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    rect_data.x1 = pRect->left;
    rect_data.y1 = pRect->top;
    rect_data.x2 = pRect->right;
    rect_data.y2 = pRect->bottom;

    // convert the logical coordinate to the device coordinate
    DC_LPtoDP(pDC, (PPOINT)&rect_data, 2);

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.DrawGraph(pDev, 
            DG_RECT, &graph_param, &rect_data);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        rect_data.x1 += ptOrg.x;
        rect_data.x2 += ptOrg.x;
        rect_data.y1 += ptOrg.y;
        rect_data.y2 += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, 
                    DG_RECT, &graph_param, &rect_data);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_ClearRect
**  Purpose :
**      Clear a specified rectangle using a specified color.
*/
void WS_ClearRect(PDC pDC, const RECT* pRect, COLORREF color)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip, rcFill;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    ASSERT(pRect != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    rcFill = *pRect;

    // convert the logical coordinate to the device coordinate
    DC_LPtoDP(pDC, (PPOINT)&rcFill, 2);

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        graph_param.flags &= ~GPF_BRUSH;
        graph_param.bk_color = pDC->pGraphDev->drv.RealizeColor(pDev,
            NULL, color);
        
        /* 调用显示设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.PatBlt(pDev, &rcFill, &graph_param);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        OffsetRect(&rcFill, ptOrg.x, ptOrg.y);
        
        /* 清除使用刷子标记，将背景色设定为指定的颜色 */
        graph_param.flags &= ~GPF_BRUSH;
        graph_param.bk_color = pDC->pGraphDev->drv.RealizeColor(pDev, 
            NULL, color);
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFill, &graph_param);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_FrameRect
**  Purpose :
**      Draws a border around the specified rectangle by using the 
**      specified brush. The width and height of the border are always 
**      one pixel. 
*/
void WS_FrameRect(PDC pDC, const RECT* pRect, PBRUSHOBJ pBrush)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip, rect, rcFrame;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    int nWidth, nHeight;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    ASSERT(pRect != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    rect = *pRect;

    // convert the logical coordinate to the device coordinate
    DC_LPtoDP(pDC, (PPOINT)&rect, 2);

    nWidth = rect.right - rect.left;
    nHeight = rect.bottom - rect.top;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        /* 将绘图参数中的刷子换为指定的刷子 */
        /**/
        //graph_param.pBrush = BRUSH_CreatePhysObj(pBrush);
        graph_param.pBrush = DC_GetPhysBrush(pDC, pBrush);
        graph_param.brush_color = DC_GetPhysColor(pDC, pBrush->logbrush.color);
        
        /* 调用显示设备驱动程序绘图函数画四条边 */
        
        // Top line
        SetRect(&rcFrame, rect.left, rect.top, 
            rect.left + nWidth, rect.top + 1);
        pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
        
        // Right line
        SetRect(&rcFrame, rect.right - 1, rect.top + 1, 
            rect.right, rect.top + nHeight - 1);
        pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
        
        // Bottom line
        SetRect(&rcFrame, rect.left, rect.bottom - 1, 
            rect.left + nWidth, rect.bottom);
        pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
        
        // Left line
        SetRect(&rcFrame, rect.left, rect.top + 1, 
            rect.left + 1, rect.top + nHeight - 1);
        pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
        
        // 物理刷子是在本程序生成的, 释生成的物理刷子
        /**/
        //BRUSH_DestroyPhysObj(pBrush, graph_param.pBrush);
        DC_ReleasePhysBrush(pDC, pBrush, graph_param.pBrush);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        OffsetRect(&rect, ptOrg.x, ptOrg.y);
        
        /* 将绘图参数中的刷子换为指定的刷子 */
        /**/
        //graph_param.pBrush = BRUSH_CreatePhysObj(pBrush);
        graph_param.pBrush = DC_GetPhysBrush(pDC, pBrush);
        graph_param.brush_color = DC_GetPhysColor(pDC, pBrush->logbrush.color);
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                /* 调用显示设备驱动程序绘图函数画四条边 */

                HIDEFRAMERECT(pDC, &rcClip);
                // Top line
                SetRect(&rcFrame, rect.left, rect.top, 
                    rect.left + nWidth, rect.top + 1);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
                
                // Right line
                SetRect(&rcFrame, rect.right - 1, rect.top + 1, 
                    rect.right, rect.top + nHeight - 1);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
                
                // Bottom line
                SetRect(&rcFrame, rect.left, rect.bottom - 1, 
                    rect.left + nWidth, rect.bottom);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);

                // Left line
                SetRect(&rcFrame, rect.left, rect.top + 1, 
                    rect.left + 1, rect.top + nHeight - 1);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);

        // 物理刷子是在本程序生成的, 释生成的物理刷子
        /**/
        //BRUSH_DestroyPhysObj(pBrush, graph_param.pBrush);
        DC_ReleasePhysBrush(pDC, pBrush, graph_param.pBrush);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_FrameRectEx
**  Purpose :
**      Draws a border around the specified rectangle by using the 
**      specified brush with giving width. 
*/
void WS_FrameRectEx(PDC pDC, const RECT* pRect, PBRUSHOBJ pBrush, 
                    int width, const RECT *pRectClip)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip, rect, rcFrame;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    int nWidth, nHeight;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    ASSERT(pRect != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    rect = *pRect;

    // convert the logical coordinate to the device coordinate
    DC_LPtoDP(pDC, (PPOINT)&rect, 2);

    nWidth = rect.right - rect.left;
    nHeight = rect.bottom - rect.top;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        if (pRectClip && !IntersectRect(&rcClip, &graph_param.clip_rect, pRectClip))
            goto RET;
        
        /* 调用设备驱动程序绘图函数 */
        /* 将绘图参数中的刷子换为指定的刷子 */
        /**/
        //graph_param.pBrush = BRUSH_CreatePhysObj(pBrush);
        graph_param.pBrush = DC_GetPhysBrush(pDC, pBrush);
        
        /* 调用显示设备驱动程序绘图函数画四条边 */
        
        // Top line
        SetRect(&rcFrame, rect.left, rect.top, 
            rect.right, rect.top + width);
        pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
        
        // Right line
        SetRect(&rcFrame, rect.right - width, rect.top + width, 
            rect.right, rect.bottom - width);
        pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
        
        // Bottom line
        SetRect(&rcFrame, rect.left, rect.bottom - width, 
            rect.right, rect.bottom);
        pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
        
        // Left line
        SetRect(&rcFrame, rect.left, rect.top + width, 
            rect.left + width, rect.bottom - width);
        pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
        
        // 物理刷子是在本程序生成的, 释生成的物理刷子
        /**/
        //BRUSH_DestroyPhysObj(pBrush, graph_param.pBrush);
        DC_ReleasePhysBrush(pDC, pBrush, graph_param.pBrush);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        OffsetRect(&rect, ptOrg.x, ptOrg.y);
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        if (pRectClip && !IntersectRect(&rcClip, &rcClip, pRectClip))
            goto RET;


        /* 将绘图参数中的刷子换为指定的刷子 */
        /**/
        //graph_param.pBrush = BRUSH_CreatePhysObj(pBrush);

        graph_param.pBrush = DC_GetPhysBrush(pDC, pBrush);

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                /* 调用显示设备驱动程序绘图函数画四条边 */
    
                HIDEFRAMERECT(pDC, &rcClip);
                // Top line
                SetRect(&rcFrame, rect.left, rect.top, 
                    rect.right, rect.top + width);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
    
                // Right line
                SetRect(&rcFrame, rect.right - width, rect.top + width, 
                    rect.right, rect.bottom - width);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
    
                // Bottom line
                SetRect(&rcFrame, rect.left, rect.bottom - width, 
                    rect.right, rect.bottom);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
    
                // Left line
                SetRect(&rcFrame, rect.left, rect.top + width, 
                    rect.left + width, rect.bottom - width);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcFrame, &graph_param);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);

        // 物理刷子是在本程序生成的, 释生成的物理刷子
        /**/
        //BRUSH_DestroyPhysObj(pBrush, graph_param.pBrush);
        DC_ReleasePhysBrush(pDC, pBrush, graph_param.pBrush);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_RoundRect
**  Purpose  :
**      Draw a specified rectangle with rounded corners.
*/
void WS_RoundRect(PDC pDC, int x1, int y1, int x2, int y2, int width, 
                  int height)
{
#if (_DISPLAY_VER >= 0x0200)

    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    ROUNDRECTDATA roundrect_data;
    POINT       ptOrg;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    roundrect_data.x1 = x1;
    roundrect_data.y1 = y1;
    roundrect_data.x2 = x2;
    roundrect_data.y2 = y2;
    roundrect_data.width = width;
    roundrect_data.height = height;

    // convert the logical coordinate to the device coordinate
    DC_LPtoDP(pDC, (PPOINT)&roundrect_data, 2);

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.DrawGraph(pDev, DG_ROUNDRECT, &graph_param, 
            &roundrect_data);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        roundrect_data.x1 += ptOrg.x;
        roundrect_data.x2 += ptOrg.x;
        roundrect_data.y1 += ptOrg.y;
        roundrect_data.y2 += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, DG_ROUNDRECT, &graph_param, 
                    &roundrect_data);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);

#endif // (_DISPLAY_VER > 0x0200)
}

/*
**  Function : WS_DrawCircle
*/
void WS_DrawCircle(PDC pDC, int x, int y, int r)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    CIRCLEDATA  circle_data;
    POINT       ptOrg;
    POINT       points;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    // convert the logical coordinate to the device coordinate
    points.x = x;
    points.y = y;
    DC_LPtoDP(pDC, &points, 1);

    circle_data.x = points.x;
    circle_data.y = points.y;
    circle_data.r = r;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.DrawGraph(pDev, DG_CIRCLE, &graph_param, 
            &circle_data);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        circle_data.x += ptOrg.x;
        circle_data.y += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, DG_CIRCLE, &graph_param, 
                    &circle_data);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_SetPixel
*/
COLORREF WS_GetPixel(PDC pDC, int x, int y)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    POINTDATA   point_data;
    POINT       ptOrg;
    POINT       points;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return CLR_INVALID;

    // convert the logical coordinate to the device coordinate
    points.x = x;
    points.y = y;

    DC_LPtoDP(pDC, &points, 1);

    point_data.x = points.x;
    point_data.y = points.y;
    point_data.color = CLR_INVALID;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        if (ISDISPLAYDC(pDC))
        {
            pDC->pGraphDev->drv.DrawGraph(pDev, DG_GETPIXEL, &graph_param, 
                &point_data);
        }
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
        {
            point_data.color = CLR_INVALID;
            goto RET;
        }

        // 将绘图参数的坐标转换为屏幕坐标
        point_data.x += ptOrg.x;
        point_data.y += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (PtInRectXY(&pExposeTbl->rect[i], point_data.x, point_data.y)
                && IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, DG_GETPIXEL, &graph_param, 
                    &point_data);

                break;
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
    return point_data.color;
}

/*
**  Function : WS_SetPixel
*/
COLORREF WS_SetPixel(PDC pDC, int x, int y, COLORREF crColor)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    POINTDATA   point_data;
    POINT       ptOrg;
    POINT       points;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return CLR_INVALID;

    // convert the logical coordinate to the device coordinate
    points.x = x;
    points.y = y;

    DC_LPtoDP(pDC, &points, 1);

    point_data.x = points.x;
    point_data.y = points.y;
    point_data.color = crColor;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        if (ISDISPLAYDC(pDC))
        {
            pDC->pGraphDev->drv.DrawGraph(pDev, DG_SETPIXEL, &graph_param, 
                &point_data);
        }
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
        {
            point_data.color = CLR_INVALID;
            goto RET;
        }

        // 将绘图参数的坐标转换为屏幕坐标
        point_data.x += ptOrg.x;
        point_data.y += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
             if (PtInRectXY(&pExposeTbl->rect[i], point_data.x, point_data.y)
                 && IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, DG_SETPIXEL, &graph_param, 
                    &point_data);

                break;
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
    return point_data.color;
}

/*
**  Function : WS_DrawEllipse
*/
void WS_DrawEllipse(PDC pDC, int x1, int y1, int x2, int y2)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    ELLIPSEDATA  ellipse_data;
    POINT       ptOrg;
    POINT       points[2];
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    // convert the logical coordinate to the device coordinate
    points[0].x = x1;
    points[0].y = y1;
    points[1].x = x2;
    points[1].y = y2;

    DC_LPtoDP(pDC, points, 2);

    ellipse_data.x = (points[0].x + points[1].x) / 2;
    ellipse_data.y = (points[0].y + points[1].y) / 2;
    ellipse_data.d1 = (points[0].x > points[1].x) ? 
        (points[0].x - points[1].x) : (points[1].x - points[0].x);
    ellipse_data.d2 = (points[0].y > points[1].y) ? 
        (points[0].y - points[1].y) : (points[1].y - points[0].y);
    ellipse_data.rotate = 0;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.DrawGraph(pDev, DG_ELLIPSE, &graph_param, 
            &ellipse_data);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        ellipse_data.x += ptOrg.x;
        ellipse_data.y += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, DG_ELLIPSE, &graph_param, 
                    &ellipse_data);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

static void CircleToDescartes(int r, const int *angleCircle, 
                              PPOINT ptDescartes, int n)
{
    int i, angle;
    int x, y;
    for (i = 0; i < n; i++)
    {
        angle = angleCircle[i] % 360;
        x = r * cosine[angle] + 0.5;
        y = r * cosine[90 - angle] + 0.5;
        switch (angle / 90)
        {
        case 0:

            ptDescartes[i].x = x;
            ptDescartes[i].y = -y;

            break;

        case 1:
            
            ptDescartes[i].x = -x;
            ptDescartes[i].y = -y;

            break;

        case 2:

            ptDescartes[i].x = -x;
            ptDescartes[i].y = y;

            break;

        case 3:

            ptDescartes[i].x = x;
            ptDescartes[i].y = y;

            break;
        }
    }
}

/*
** Function : WS_AngleArc
*/
void WS_AngleArc(PDC pDC, int x, int y, DWORD dwRadius, 
                 FLOAT eStartAngle, FLOAT eSweepAngle)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    ELLIPSEARCDATA  ellipsearc_data;
    POINT       pt[3];
    int         angle[2];
    POINT       ptOrg;
    int         i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    
    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    angle[0] = (int)(eStartAngle + 0.5);
    angle[1] = (int)(eStartAngle + eSweepAngle + 0.5);
    CircleToDescartes(dwRadius, angle, pt, 2);
    pt[0].x += x;
    pt[0].y += y;
    pt[1].x += x;
    pt[1].y += y;
    pt[2].x = x;
    pt[2].y = y;

    DC_LPtoDP(pDC, pt, 3);
    
    WS_LineTo(pDC, pt[0].x, pt[0].y);

    ellipsearc_data.d1 = dwRadius * 2;
    ellipsearc_data.d2 = dwRadius * 2;
    ellipsearc_data.x = pt[2].x;
    ellipsearc_data.y = pt[2].y;
    ellipsearc_data.mode = AM_ARC;
    ellipsearc_data.start_x = pt[0].x;
    ellipsearc_data.start_y = pt[0].y;
    ellipsearc_data.end_x = pt[1].x;
    ellipsearc_data.end_y = pt[1].y;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.DrawGraph(pDev, DG_ELLIPSEARC, &graph_param, 
            &ellipsearc_data);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        ellipsearc_data.x += ptOrg.x;
        ellipsearc_data.y += ptOrg.y;
        ellipsearc_data.start_x += ptOrg.x;
        ellipsearc_data.start_y += ptOrg.y;
        ellipsearc_data.end_x += ptOrg.x;
        ellipsearc_data.end_y += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, DG_ELLIPSEARC, &graph_param, 
                    &ellipsearc_data);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

void WS_Arc(PDC pDC, int nLeftRect, int nTopRect, int nRightRect, 
            int nBottomRect, int nXStartArc, int nYStartArc, int nXEndArc, 
            int nYEndArc, int mode)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    ELLIPSEARCDATA  ellipsearc_data;
    POINT       pt[4];
    POINT       ptOrg;
    int         i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    
    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    pt[0].x = nLeftRect;
    pt[0].y = nTopRect;
    pt[1].x = nRightRect;
    pt[1].y = nBottomRect;
    pt[2].x = nXStartArc;
    pt[2].y = nYStartArc;
    pt[3].x = nXEndArc;
    pt[3].y = nYEndArc;

    DC_LPtoDP(pDC, pt, 4);

    ellipsearc_data.d1 = (pt[0].x > pt[1].x) ? 
        (pt[0].x - pt[1].x) : (pt[1].x - pt[0].x);
    ellipsearc_data.d2 = (pt[0].y > pt[1].y) ? 
        (pt[0].y - pt[1].y) : (pt[1].y - pt[0].y);
    ellipsearc_data.x = (pt[0].x + pt[1].x) / 2;
    ellipsearc_data.y = (pt[0].y + pt[1].y) / 2;
    ellipsearc_data.mode = mode;
    ellipsearc_data.start_x = pt[2].x;
    ellipsearc_data.start_y = pt[2].y;
    ellipsearc_data.end_x = pt[3].x;
    ellipsearc_data.end_y = pt[3].y;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.DrawGraph(pDev, DG_ELLIPSEARC, &graph_param, 
            &ellipsearc_data);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        ellipsearc_data.x += ptOrg.x;
        ellipsearc_data.y += ptOrg.y;
        ellipsearc_data.start_x += ptOrg.x;
        ellipsearc_data.start_y += ptOrg.y;
        ellipsearc_data.end_x += ptOrg.x;
        ellipsearc_data.end_y += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, DG_ELLIPSEARC, &graph_param, 
                    &ellipsearc_data);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_ArcTo
*/
void WS_ArcTo(PDC pDC, 
            int nLeftRect, int nTopRect, int nRightRect, int nBottomRect, 
            int nXStartArc, int nYStartArc, int nXEndArc, int nYEndArc)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    ELLIPSEARCDATA  ellipsearc_data;
    POINT       pt[4];
    POINT       ptOrg;
    int         i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    
    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    pt[0].x = nLeftRect;
    pt[0].y = nTopRect;
    pt[1].x = nRightRect;
    pt[1].y = nBottomRect;
    pt[2].x = nXStartArc;
    pt[2].y = nYStartArc;
    pt[3].x = nXEndArc;
    pt[3].y = nYEndArc;

    DC_LPtoDP(pDC, pt, 4);

    ellipsearc_data.d1 = (pt[0].x > pt[1].x) ? 
        (pt[0].x - pt[1].x) : (pt[1].x - pt[0].x);
    ellipsearc_data.d2 = (pt[0].y > pt[1].y) ? 
        (pt[0].y - pt[1].y) : (pt[1].y - pt[0].y);
    ellipsearc_data.x = (pt[0].x + pt[1].x) / 2;
    ellipsearc_data.y = (pt[0].y + pt[1].y) / 2;
    ellipsearc_data.mode = AM_ARC;
    ellipsearc_data.start_x = pt[2].x;
    ellipsearc_data.start_y = pt[2].y;
    ellipsearc_data.end_x = pt[3].x;
    ellipsearc_data.end_y = pt[3].y;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.DrawGraph(pDev, DG_ELLIPSEARC, &graph_param, 
            &ellipsearc_data);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        ellipsearc_data.x += ptOrg.x;
        ellipsearc_data.y += ptOrg.y;
        ellipsearc_data.start_x += ptOrg.x;
        ellipsearc_data.start_y += ptOrg.y;
        ellipsearc_data.end_x += ptOrg.x;
        ellipsearc_data.end_y += ptOrg.y;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, DG_ELLIPSEARC, &graph_param, 
                    &ellipsearc_data);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
        ellipsearc_data.start_x -= ptOrg.x;
        ellipsearc_data.start_y -= ptOrg.y;
        ellipsearc_data.end_x -= ptOrg.x;
        ellipsearc_data.end_y -= ptOrg.y;
    }

    pt[0].x = ellipsearc_data.start_x;
    pt[0].y = ellipsearc_data.start_y;
    pt[1].x = ellipsearc_data.end_x;
    pt[1].y = ellipsearc_data.end_y;

    DC_DPtoLP(pDC, pt, 4);

    WS_LineTo(pDC, pt[0].x, pt[0].y);
    DC_SetCurPos(pDC, pt[1].x, pt[1].y, NULL);

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_DrawPolygon
*/
void WS_DrawPolygon(PDC pDC, POINT* ppoints, int count)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip;
    GRAPHPARAM  graph_param;
    PPOLYLINEDATA  ppolygon_data;
    POINT       ptOrg;
    int         size;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    
    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    size = sizeof(POLYLINEDATA) + sizeof(POINT) * (count - 1);
    // convert the logical coordinate to the device coordinate
    ppolygon_data = (PPOLYLINEDATA) MemAlloc(size);
    memcpy(ppolygon_data->points, ppoints, sizeof(POINT) * count);
    DC_LPtoDP(pDC, ppolygon_data->points, count);

    ppolygon_data->mode = 1;
    ppolygon_data->count = count;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.DrawGraph(pDev, DG_POLYGON, &graph_param, 
            ppolygon_data);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        for (i = 0; i < count; i++)
        {
            ppolygon_data->points[i].x += ptOrg.x;
            ppolygon_data->points[i].y += ptOrg.y;
        }
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DrawGraph(pDev, DG_POLYGON, &graph_param, 
                    ppolygon_data);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    MemFree(ppolygon_data);
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_PatBlt
*/
void WS_PatBlt(PDC pDC, int x, int y, int width, int height, DWORD dwRop)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip, rcBlt;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    int i;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return;

    rcBlt.left = x;
    rcBlt.top = y;
    rcBlt.right = x + width;
    rcBlt.bottom = y + height;

    // convert the logical coordinate to the device coordinate
    DC_LPtoDP(pDC, (PPOINT)&rcBlt, 2);

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        graph_param.rop = dwRop;

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.PatBlt(pDev, &rcBlt, &graph_param);
    }
    else
    {
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        rcBlt.left   += ptOrg.x;
        rcBlt.right  += ptOrg.x;
        rcBlt.top    += ptOrg.y;
        rcBlt.bottom += ptOrg.y;
        
        graph_param.rop = dwRop;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect, 
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.PatBlt(pDev, &rcBlt, &graph_param);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);
}

/*
**  Function : WS_AlphaBlend
**  Purpose  :
**      
*/
void WS_AlphaBlend(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                   int nDestH, PDC pSrcDC, int nSrcX, int nSrcY, 
                   int nSrcW, int nSrcH, PBLENDFUNCTION pBlend)
{
    PEXPOSETBL  pExposeTbl;
    PWINOBJ     pWin;
    PDEV        pDev;
    RECT        rcClip, rcDest, rcSrc;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    PDEV        pSrcDev;
    int         i;
    int         nType;
    
    // Ensures the window object pointer isn't NULL
    ASSERT(pDestDC != NULL && pSrcDC != NULL);
    if (!ISDISPLAYDC(pDestDC))
        return;

    /* 获得pDestDC对应的显示设备 */
    pDev = DC_GetDevice(pDestDC);
    if (!pDev)
        return;

    // convert the logical coordinate to the device coordinate
    SetRect(&rcSrc, nSrcX, nSrcY, nSrcX + nSrcW, nSrcY + nSrcH);
    SetRect(&rcDest, nDestX, nDestY, nDestX + nDestW, nDestY + nDestH);
    DC_LPtoDP(pDestDC, (PPOINT)&rcDest, 2);

    // 如果是窗口DC, 首先获得窗口的暴露区表, 若窗口无暴露区域, 直接返回
    if (DC_IsWindowDC(pDestDC))
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDestDC, &ptOrg, &pExposeTbl, &graph_param))
        {
            // 预处理失败，可能没有暴露区或参数无效，释放对DC设备的占用
            DC_ReleaseDevice(pDestDC);
            return;
        }

        OffsetRect(&rcDest, ptOrg.x, ptOrg.y);
    }
    
    nType = WOT_GetObjType((PWSOBJ)pSrcDC);
    if (nType == OBJ_DC)
    {
        pSrcDev = DC_GetDevice(pSrcDC);
        if (!pSrcDev)
            goto RET;

        DC_LPtoDP(pSrcDC, (PPOINT)&rcSrc, 2);

        if (DC_IsWindowDC(pSrcDC))
        {
            /* 窗口设备, 首先获得窗口DC对应的窗口对象 */
            pWin = DC_GetWindow(pSrcDC);

            // 返回NULL表示窗口已被删除, 在Debug版本中ASSERT会报告这个
            // 错误, 在Release版本中直接返回以避免窗口系统出错
            ASSERT(pWin != NULL);
            if (!pWin)
                goto RET;

            /* 不考虑源窗口的暴露区间, 将源窗口上的矩形区域当作 */
            /* 屏幕上的矩形 */
            
            // 获得绘图原点的坐标
            WND_GetPaintOrg(pWin, DC_GetPaintArea(pSrcDC), &ptOrg);
            
            // 将源窗口的坐标转换为屏幕坐标
            OffsetRect(&rcSrc, ptOrg.x, ptOrg.y);
        }
    }
    else if (nType == OBJ_BITMAP)
    {
        if (BMP_IsCompatible(pDestDC, (PBMPOBJ)pSrcDC))
            pSrcDev = BMP_GetDevice((PBMPOBJ)pSrcDC);
        else
        {
            /* 不支持不兼容位图间的处理 */
            return ;
        }
    }
    else
    {
        ASSERT(0);
        return ;
    }

    if (!DC_IsWindowDC(pDestDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDestDC, &graph_param);

        graph_param.pBlend = pBlend;
        /* 调用设备驱动程序绘图函数 */
        if (nDestW == nSrcW && nDestH == nSrcH)
        {
            pDestDC->pGraphDev->drv.BitBlt(pDev, &rcDest, pSrcDev, nSrcX, nSrcY, 
                &graph_param);
        }
        else
        {
            pDestDC->pGraphDev->drv.StretchBlt(pDev, &rcDest, pSrcDev, &rcSrc, 
                &graph_param);
        }
    }
    else
    {
        graph_param.pBlend = pBlend;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        if (nDestW == nSrcW && nDestH == nSrcH)
        {
            for (i = 0; i < pExposeTbl->count; i++)
            {
                if (IntersectRect(&graph_param.clip_rect,
                    &pExposeTbl->rect[i], &rcClip))
                {
                    HIDEFRAMERECT(pDestDC, &rcClip);
                    pDestDC->pGraphDev->drv.BitBlt(pDev, &rcDest, pSrcDev, nSrcX, nSrcY, 
                        &graph_param);
                }
            }
            SHOWFRAMERECT(pDestDC, &rcClip);
        }
        else
        {
            for (i = 0; i < pExposeTbl->count; i++)
            {
                if (IntersectRect(&graph_param.clip_rect,
                    &pExposeTbl->rect[i], &rcClip))
                {
                    HIDEFRAMERECT(pDestDC, &rcClip);
                    pDestDC->pGraphDev->drv.StretchBlt(pDev, &rcDest, pSrcDev, &rcSrc, 
                        &graph_param);
                }
            }
            SHOWFRAMERECT(pDestDC, &rcClip);
        }
    }

RET : 

    // 释放对对源设备的占用
    if (nType == OBJ_DC)
        DC_ReleaseDevice(pSrcDC);
    else if (nType == OBJ_BITMAP)
    {
        if (BMP_IsCompatible(pDestDC, (PBMPOBJ)pSrcDC))
            BMP_ReleaseDevice((PBMPOBJ)pSrcDC);
    }

    // 释放对目的DC设备的占用
    DC_ReleaseDevice(pDestDC);
}


/*
**  Function : WS_AlphaDIBBlend
**  Purpose  :
**      
*/
void WS_AlphaDIBBlend(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                      int nDestH, const void *lpvBits, 
                      const BITMAPINFO *lpbmi, UINT fuColorUse, int nSrcX, 
                      int nSrcY, int nSrcW, int nSrcH, PBLENDFUNCTION pBlend)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip, rcSrc, rcDest;
    GRAPHPARAM  graph_param;
    PDEV        pSrcDev;
    POINT       ptOrg;
    int         i;
    DIBITMAP    DIBitmap;
    
    // Ensures the window object pointer isn't NULL
    ASSERT(pDestDC != NULL);
    ASSERT(lpvBits != NULL);
    ASSERT(lpbmi != NULL);
    ASSERT(lpbmi->bmiHeader.biWidth > 0);
    ASSERT(lpbmi->bmiHeader.biHeight != 0);

    if (!ISDISPLAYDC(pDestDC))
        return;

    if (!BMP_GetDIBITMAP(&DIBitmap, lpvBits, lpbmi, fuColorUse))
        return;

    /* 获得pDestDC对应的显示设备 */
    pDev = DC_GetDevice(pDestDC);
    if (!pDev)
        return;

    // convert the logical coordinate to the device coordinate
    SetRect(&rcSrc, nSrcX, nSrcY, nSrcX + nSrcW, nSrcY + nSrcH);
    SetRect(&rcDest, nDestX, nDestY, nDestX + nDestW, nDestY + nDestH);
    DC_LPtoDP(pDestDC, (PPOINT)&rcDest, 2);
    
    pSrcDev = NULL;
    
    if (!DC_IsWindowDC(pDestDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDestDC, &graph_param);
        graph_param.pBlend = pBlend;

        /* 调用设备驱动程序绘图函数 */
        if (nSrcW == nDestW && nSrcH == nDestH)
        {
            pDestDC->pGraphDev->drv.DIBitBlt(pDev, &rcDest, &DIBitmap, nSrcX, 
                nSrcY, &graph_param);                        
        }
        else
        {
            pDestDC->pGraphDev->drv.DIStretchBlt(pDev, &rcDest, &DIBitmap, &rcSrc, 
                &graph_param);                        
        }
    }
    else
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDestDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        OffsetRect(&rcDest, ptOrg.x, ptOrg.y);
        graph_param.pBlend = pBlend;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        if (nSrcW == nDestW && nSrcH == nDestH)
        {
            for (i = 0; i < pExposeTbl->count; i++)
            {
                if (IntersectRect(&graph_param.clip_rect,
                    &pExposeTbl->rect[i], &rcClip))
                {
                    HIDEFRAMERECT(pDestDC, &rcClip);
                    pDestDC->pGraphDev->drv.DIBitBlt(pDev, &rcDest, &DIBitmap, nSrcX, 
                        nSrcY, &graph_param);                        
                }
            }
            SHOWFRAMERECT(pDestDC, &rcClip);
        }
        else
        {
            for (i = 0; i < pExposeTbl->count; i++)
            {
                if (IntersectRect(&graph_param.clip_rect,
                    &pExposeTbl->rect[i], &rcClip))
                {
                    HIDEFRAMERECT(pDestDC, &rcClip);
                    pDestDC->pGraphDev->drv.DIStretchBlt(pDev, &rcDest, &DIBitmap, 
                        &rcSrc, &graph_param);                        
                }
            }
            SHOWFRAMERECT(pDestDC, &rcClip);
        }
    }

RET:
    // 释放对DC设备的占用
    DC_ReleaseDevice(pDestDC);

	return;
}

/*
**  Function : WS_TransparentBlt
**  Purpose  :
**      
*/
void WS_TransparentBlt(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                      int nDestH, PDC pSrcDC, int nSrcX, int nSrcY, 
                   int nSrcW, int nSrcH, UINT crTransparent)
{
    return;
}

/*
**  Function : WS_TransparentDIBits
**  Purpose  :
**      
*/
void WS_TransparentDIBits(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                      int nDestH, const void *lpvBits, 
                      const BITMAPINFO *lpbmi, UINT fuColorUse, int nSrcX, 
                      int nSrcY, int nSrcW, int nSrcH, UINT crTransparent)
{
    return;
}

/*
**  Function : WS_BitBlt
**  Purpose  :
**      
*/
void WS_BitBlt(PDC pDestDC, int nDestX, int nDestY, int nWidth, int nHeight, 
               PDC pSrcDC, int nSrcX, int nSrcY, DWORD dwRop)
{
    RGBQUAD MonoPalette[2];
    PEXPOSETBL  pExposeTbl;
    PWINOBJ     pWin;
    PDEV        pDev;
    RECT        rcClip, rcBlt;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    PDEV        pSrcDev;
    int         i;
    int         nType;
    POINT       points;
    DIBITMAP    DIBitmap;
    PBITMAP     pBitmap;
    BOOL        bMonoBmpSrc;
    COLORREF    crBack, crText;
    
    // Ensures the window object pointer isn't NULL
    ASSERT(pDestDC != NULL);

    /* 获得pDestDC对应的显示设备 */
    pDev = DC_GetDevice(pDestDC);
    if (!pDev)
        return;

    // convert the logical coordinate to the device coordinate
    SetRect(&rcBlt, nDestX, nDestY, nDestX + nWidth, nDestY + nHeight);
    DC_LPtoDP(pDestDC, (PPOINT)&rcBlt, 2);
    
    // 如果是窗口DC, 首先获得窗口的暴露区表, 若窗口无暴露区域, 直接返回
    if (DC_IsWindowDC(pDestDC))
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDestDC, &ptOrg, &pExposeTbl, &graph_param))
        {
            // 释放对DC设备的占用
            DC_ReleaseDevice(pDestDC);
            return;
        }

        OffsetRect(&rcBlt, ptOrg.x, ptOrg.y);
    }
    

    nType = 0;
    bMonoBmpSrc = FALSE;

    if (pSrcDC)
    {
        nType = WOT_GetObjType((PWSOBJ)pSrcDC);

        if (nType == OBJ_DC)
        {
            points.x = nSrcX;
            points.y = nSrcY;
            DC_LPtoDP(pSrcDC, &points, 1);
            nSrcX = points.x;
            nSrcY = points.y;

            pSrcDev = DC_GetDevice(pSrcDC);
            if (!pSrcDev)
                goto RET;

            if (DC_IsWindowDC(pSrcDC))
            {
                /* 窗口设备, 首先获得窗口DC对应的窗口对象 */
                pWin = DC_GetWindow(pSrcDC);

                // 返回NULL表示窗口已被删除, 在Debug版本中ASSERT会报告这个
                // 错误, 在Release版本中直接返回以避免窗口系统出错
                ASSERT(pWin != NULL);
                if (!pWin)
                    goto RET;

                /* 不考虑源窗口的暴露区间, 将源窗口上的矩形区域当作 */
                /* 屏幕上的矩形 */
                
                // 获得绘图原点的坐标
                WND_GetPaintOrg(pWin, DC_GetPaintArea(pSrcDC), &ptOrg);
                
                // 将源窗口的坐标转换为屏幕坐标
                nSrcX += ptOrg.x;
                nSrcY += ptOrg.y;
            }
        }
        else
        {
            if (nType == OBJ_BITMAP)
            {
                if (BMP_IsCompatible(pDestDC, (PBMPOBJ)pSrcDC))
                    pSrcDev = BMP_GetDevice((PBMPOBJ)pSrcDC);
                else
                {
                    // The bitmap object is a mono bitmap
                    bMonoBmpSrc = TRUE;

                    // Gets the bitmap struct
                    pBitmap = &((PBMPOBJ)pSrcDC)->bitmap;

                    // Fills a DIBitmap struct using the bitmap struct
                    DIBitmap.type   = BMP_MONO;
                    DIBitmap.width  = (int16)pBitmap->bmWidth;
                    DIBitmap.height = -(int16)pBitmap->bmHeight;
                    DIBitmap.palette_entries = 2;
                    DIBitmap.bmp_size = pBitmap->bmWidthBytes * 
                        pBitmap->bmHeight * pBitmap->bmPlanes;
                    DIBitmap.pBuffer = BMP_GetData((PBMPOBJ)pSrcDC);
                    DIBitmap.pPalette = (void*)MonoPalette;

                    pSrcDev = NULL;
                }
            }
            else
                pSrcDev = NULL;
        }
    }
    else
        pSrcDev = NULL;

    if (bMonoBmpSrc)
    {
        crBack = DC_GetBkColor(pDestDC);
        crText = DC_GetTextColor(pDestDC);
        
        MonoPalette[0].rgbBlue = GetBValue(crText);
        MonoPalette[0].rgbGreen = GetGValue(crText);
        MonoPalette[0].rgbRed = GetRValue(crText);
        MonoPalette[1].rgbBlue = GetBValue(crBack);
        MonoPalette[1].rgbGreen = GetGValue(crBack);
        MonoPalette[1].rgbRed = GetRValue(crBack);
    }
    
    if (!DC_IsWindowDC(pDestDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDestDC, &graph_param);

        graph_param.rop = dwRop;

        /* 调用设备驱动程序绘图函数 */
        if (ISDISPLAYDC(pDestDC))
        {
            if (bMonoBmpSrc)
            {
                pDestDC->pGraphDev->drv.DIBitBlt(pDev, &rcBlt, &DIBitmap, nSrcX, nSrcY, 
                    &graph_param);
            }
            else
            {
                pDestDC->pGraphDev->drv.BitBlt(pDev, &rcBlt, pSrcDev, nSrcX, nSrcY, 
                    &graph_param);
            }
        }
#if (PRINTERSUPPORT)
        else if (ISPRINTERDC(pDestDC))
        {
            if (nType == OBJ_BITMAP)
            {
                // Gets the bitmap struct
                pBitmap = &((PBMPOBJ)pSrcDC)->bitmap;

                // Fills a DIBitmap struct using the bitmap struct
                switch (pBitmap->bmBitsPixel)
                {
                case 1 :

                    DIBitmap.type = BMP_MONO;
                    break;

                case 16 :

                    DIBitmap.type = BMP_RGB16;
                    break;

                case 24 : 

                    DIBitmap.type = BMP_RGB24;
                    break;

                default :

                    goto RET;
                }

                DIBitmap.width  = (int16)pBitmap->bmWidth;
                DIBitmap.height = -(int16)pBitmap->bmHeight;
                DIBitmap.palette_entries = 0;
                DIBitmap.bmp_size = pBitmap->bmWidthBytes * 
                    pBitmap->bmHeight * pBitmap->bmPlanes;
                DIBitmap.pBuffer = BMP_GetData((PBMPOBJ)pSrcDC);
                DIBitmap.pPalette = NULL;
                
                pDestDC->pGraphDev->drv.DIBitBlt(pDev, &rcBlt, &DIBitmap, nSrcX, 
                    nSrcY, &graph_param);
            }
            else if (nType == OBJ_DC && pSrcDC->bType == DC_BITMAP)
            {
                pBitmap = &((PBMPOBJ)pSrcDC->pDev)->bitmap;

                // Fills a DIBitmap struct using the bitmap struct
                switch (pBitmap->bmBitsPixel)
                {
                case 1 :

                    DIBitmap.type = BMP_MONO;
                    break;

                case 16 :

                    DIBitmap.type = BMP_RGB16;
                    break;

                case 24 : 

                    DIBitmap.type = BMP_RGB24;
                    break;

                default :

                    goto RET;
                }

                DIBitmap.width  = (int16)pBitmap->bmWidth;
                DIBitmap.height = -(int16)pBitmap->bmHeight;
                DIBitmap.palette_entries = 0;
                DIBitmap.bmp_size = pBitmap->bmWidthBytes * 
                    pBitmap->bmHeight * pBitmap->bmPlanes;
                DIBitmap.pBuffer = BMP_GetData((PBMPOBJ)pSrcDC->pDev);
                DIBitmap.pPalette = NULL;
                
                pDestDC->pGraphDev->drv.DIBitBlt(pDev, &rcBlt, &DIBitmap, nSrcX, 
                    nSrcY, &graph_param);

                BMP_ReleaseData((PBMPOBJ)pSrcDC->pDev);
            }
        }
#endif // PRINTERSUPPORT
        else
        {
            ASSERT(0);
        }
    }
    else
    {
        graph_param.rop = dwRop;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        if (bMonoBmpSrc)
        {
            for (i = 0; i < pExposeTbl->count; i++)
            {
                if (IntersectRect(&graph_param.clip_rect,
                    &pExposeTbl->rect[i], &rcClip))
                {
                    HIDEFRAMERECT(pDestDC, &rcClip);
                    pDestDC->pGraphDev->drv.DIBitBlt(pDev, &rcBlt, &DIBitmap, 
                        nSrcX, nSrcY, &graph_param);
                }
            }
            SHOWFRAMERECT(pDestDC, &rcClip);
        }
        else
        {
            for (i = 0; i < pExposeTbl->count; i++)
            {
                if (IntersectRect(&graph_param.clip_rect,
                    &pExposeTbl->rect[i], &rcClip))
                {
                    HIDEFRAMERECT(pDestDC, &rcClip);
                    pDestDC->pGraphDev->drv.BitBlt(pDev, &rcBlt, pSrcDev, nSrcX, 
                        nSrcY, &graph_param);
                }
            }
            SHOWFRAMERECT(pDestDC, &rcClip);
        }

    }

RET : 

    // 释放对对源设备的占用
    if (pSrcDC != NULL)
    {
        if (nType == OBJ_DC)
            DC_ReleaseDevice(pSrcDC);
        else if (nType == OBJ_BITMAP)
        {
            if (BMP_IsCompatible(pDestDC, (PBMPOBJ)pSrcDC))
                BMP_ReleaseDevice((PBMPOBJ)pSrcDC);
            else
                BMP_ReleaseData((PBMPOBJ)pSrcDC);
        }
    }

    // 释放对目的DC设备的占用
    DC_ReleaseDevice(pDestDC);
}

/*
**  Function : WS_StretchBlt
**  Purpose  :
**      Copies a bitmap from a source rectangle into a destination 
**      rectangle, stretching or compressing the bitmap to fit the 
**      dimensions of the destination rectangle, if necessary. Windows
**      stretches or compresses the bitmap according to the stretching
**      mode currently set in the destination device context. 
*/
void WS_StretchBlt(PDC pDestDC, int nDestX, int nDestY, int nDestW, 
                   int nDestH, PDC pSrcDC, int nSrcX, int nSrcY, 
                   int nSrcW, int nSrcH, DWORD dwRop)
{
    RGBQUAD     MonoPalette[2];
    PEXPOSETBL  pExposeTbl;
    PWINOBJ     pWin;
    PDEV        pDev;
    RECT        rcClip, rcDest, rcSrc;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    PDEV        pSrcDev;
    int         i;
    int         nType;
    DIBITMAP    DIBitmap;
    PBITMAP     pBitmap;
    BOOL        bMonoBmpSrc;
    COLORREF    crBack, crText;
    
    // Ensures the window object pointer isn't NULL
    ASSERT(pDestDC != NULL);
    
    // Gets the destination device of the destination DC object
    pDev = DC_GetDevice(pDestDC);
    if (!pDev)
        return;

    // Convert the destination coordinate from logical to device
    SetRect(&rcDest, nDestX, nDestY, nDestX + nDestW, nDestY + nDestH);
    DC_LPtoDP(pDestDC, (PPOINT)&rcDest, 2);

    // 如果是窗口DC, 首先获得窗口的暴露区表, 若窗口无暴露区域, 直接返回
    if (DC_IsWindowDC(pDestDC))
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDestDC, &ptOrg, &pExposeTbl, &graph_param))
        {
            // 释放对DC设备的占用
            DC_ReleaseDevice(pDestDC);
            return;
        }

        OffsetRect(&rcDest, ptOrg.x, ptOrg.y);
    }
    
    SetRect(&rcSrc, nSrcX, nSrcY, nSrcX + nSrcW, nSrcY + nSrcH);

    nType = 0;
    bMonoBmpSrc = FALSE;

    if (pSrcDC)
    {
        nType = WOT_GetObjType((PWSOBJ)pSrcDC);

        if (nType == OBJ_DC)
        {
            pSrcDev = DC_GetDevice(pSrcDC);
            if (!pSrcDev)
                goto RET;

            // Converts logical coordinates to physical coordinates
            DC_LPtoDP(pSrcDC, (PPOINT)&rcSrc, 2);

            if (DC_IsWindowDC(pSrcDC))
            {
                // 窗口设备, 首先获得窗口DC对应的窗口对象
                pWin = DC_GetWindow(pSrcDC);

                // 返回NULL表示窗口已被删除, 在Debug版本中ASSERT会报告这个
                // 错误, 在Release版本中直接返回以避免窗口系统出错
                ASSERT(pWin != NULL);
                if (!pWin)
                    goto RET;

                // 不考虑源窗口的暴露区间, 将源窗口上的矩形区域当作
                // 屏幕上的矩形
                
                // 获得绘图原点的坐标
                WND_GetPaintOrg(pWin, DC_GetPaintArea(pDestDC), &ptOrg);
                
                // 将源窗口的坐标转换为屏幕坐标
                OffsetRect(&rcSrc, ptOrg.x, ptOrg.y);
            }
        }
        else
        {
            if (nType == OBJ_BITMAP)
            {
                if (BMP_IsCompatible(pDestDC, (PBMPOBJ)pSrcDC))
                {
                    pSrcDev = BMP_GetDevice((PBMPOBJ)pSrcDC);
                }
                else
                {
                    // The bitmap object is a mono bitmap
                    bMonoBmpSrc = TRUE;

                    // Gets the bitmap struct
                    pBitmap = &((PBMPOBJ)pSrcDC)->bitmap;

                    // Fills a DIBitmap struct using the bitmap struct
                    DIBitmap.type   = BMP_MONO;
                    DIBitmap.width  = (int16)pBitmap->bmWidth;
                    DIBitmap.height = -(int16)pBitmap->bmHeight;
                    DIBitmap.palette_entries = 2;
                    DIBitmap.bmp_size = pBitmap->bmWidthBytes * 
                        pBitmap->bmHeight * pBitmap->bmPlanes;
                    DIBitmap.pBuffer = BMP_GetData((PBMPOBJ)pSrcDC);
                    DIBitmap.pPalette = (void*)MonoPalette;

                    pSrcDev = NULL;
                }
            }
            else
                pSrcDev = NULL;
        }
    }
    else
        pSrcDev = NULL;

    if (bMonoBmpSrc)
    {
        crBack = DC_GetBkColor(pDestDC);
        crText = DC_GetTextColor(pDestDC);
        
        MonoPalette[0].rgbBlue = GetBValue(crText);
        MonoPalette[0].rgbGreen = GetGValue(crText);
        MonoPalette[0].rgbRed = GetRValue(crText);
        MonoPalette[1].rgbBlue = GetBValue(crBack);
        MonoPalette[1].rgbGreen = GetGValue(crBack);
        MonoPalette[1].rgbRed = GetRValue(crBack);
    }
    
    if (!DC_IsWindowDC(pDestDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDestDC, &graph_param);

        graph_param.rop = dwRop;

        /* 调用设备驱动程序绘图函数 */
        if (ISDISPLAYDC(pDestDC))
        {
            if (bMonoBmpSrc)
            {
                pDestDC->pGraphDev->drv.DIStretchBlt(pDev, &rcDest, &DIBitmap, &rcSrc, 
                    &graph_param);
            }
            else
            {
                pDestDC->pGraphDev->drv.StretchBlt(pDev, &rcDest, pSrcDev, &rcSrc, 
                    &graph_param);
            }
        }
#if (PRINTERSUPPORT)
        else if (ISPRINTERDC(pDestDC))
        {
            if (nType == OBJ_DC && pSrcDC->bType == DC_BITMAP)
            {
                pBitmap = &((PBMPOBJ)pSrcDC->pDev)->bitmap;

                // Fills a DIBitmap struct using the bitmap struct
                switch (pBitmap->bmBitsPixel)
                {
                case 16 :

                    DIBitmap.type = BMP_RGB16;
                    break;

                case 24 : 

                    DIBitmap.type = BMP_RGB24;
                    break;

                default :

                    ASSERT(0);
                }

                DIBitmap.width  = (int16)pBitmap->bmWidth;
                DIBitmap.height = -(int16)pBitmap->bmHeight;
                DIBitmap.palette_entries = 0;
                DIBitmap.bmp_size = pBitmap->bmWidthBytes * 
                    pBitmap->bmHeight * pBitmap->bmPlanes;
                DIBitmap.pBuffer = BMP_GetData((PBMPOBJ)pSrcDC->pDev);
                DIBitmap.pPalette = NULL;
                
                pDestDC->pGraphDev->drv.DIStretchBlt(pDev, &rcDest, &DIBitmap, &rcSrc, 
                    &graph_param);

                BMP_ReleaseData((PBMPOBJ)pSrcDC->pDev);
            }
        }
#endif // PRINTERSUPPORT
        else
        {
            ASSERT(0);
        }
    }
    else
    {
        graph_param.rop = dwRop;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        if (bMonoBmpSrc)
        {
            for (i = 0; i < pExposeTbl->count; i++)
            {
                if (IntersectRect(&graph_param.clip_rect,
                    &pExposeTbl->rect[i], &rcClip))
                {
                    HIDEFRAMERECT(pDestDC, &rcClip);
                    pDestDC->pGraphDev->drv.DIStretchBlt(pDev, &rcDest, &DIBitmap, &rcSrc, 
                        &graph_param);                        
                }
            }
            SHOWFRAMERECT(pDestDC, &rcClip);
        }
        else
        {
            for (i = 0; i < pExposeTbl->count; i++)
            {
                if (IntersectRect(&graph_param.clip_rect,
                    &pExposeTbl->rect[i], &rcClip))
                {
                    HIDEFRAMERECT(pDestDC, &rcClip);
                    pDestDC->pGraphDev->drv.StretchBlt(pDev, &rcDest, pSrcDev, &rcSrc, 
                        &graph_param);
                }
            }
            SHOWFRAMERECT(pDestDC, &rcClip);
        }
    }

RET : 

    if (pSrcDC != NULL)
    {
        if (nType == OBJ_DC)
            DC_ReleaseDevice(pSrcDC);
        else if (nType == OBJ_BITMAP)
        {
            if (BMP_IsCompatible(pDestDC, (PBMPOBJ)pSrcDC))
                BMP_ReleaseDevice((PBMPOBJ)pSrcDC);
            else
                BMP_ReleaseData((PBMPOBJ)pSrcDC);
        }
    }
}

/*
**  Function : WS_SetDIBitsToDevice
*/
int WS_SetDIBitsToDevice(PDC pDestDC, int nDestX, int nDestY, 
                         DWORD dwWidth, DWORD dwHeight, int nSrcX, 
                         int nSrcY, UINT uStartScan, 
                         UINT cScanLines, CONST VOID *lpvBits, 
                         CONST BITMAPINFO *lpbmi, UINT fuColorUse)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip, rcBlt;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    PDEV        pSrcDev;
    int         i;
    DIBITMAP    DIBitmap;
    int         nHeight;
    
    // Ensures the window object pointer isn't NULL
    ASSERT(pDestDC != NULL);
    ASSERT(lpvBits != NULL);
    ASSERT(lpbmi != NULL);
    ASSERT(lpbmi->bmiHeader.biWidth > 0);
    ASSERT(lpbmi->bmiHeader.biHeight != 0);

    if (!BMP_GetDIBITMAP(&DIBitmap, lpvBits, lpbmi, fuColorUse))
        return 0;

    nHeight = (lpbmi->bmiHeader.biHeight > 0) ? 
        (lpbmi->bmiHeader.biHeight) :-(lpbmi->bmiHeader.biHeight);

#if (COLUMN_BITMAP)

    if (lpbmi->bmiHeader.biCompression & BI_COLUMN)
    {
        if (uStartScan > (UINT)(lpbmi->bmiHeader.biWidth - 1))
        {
            SetLastError(1);
            return 0;
        }
        
        if (uStartScan + cScanLines > (UINT)(lpbmi->bmiHeader.biWidth))
            cScanLines = lpbmi->bmiHeader.biWidth  - uStartScan;

        if (dwWidth > cScanLines)
            dwWidth = cScanLines;
    }
    else
    {
        if (uStartScan > (UINT)(nHeight - 1))
        {
            SetLastError(1);
            return 0;
        }
        
        if (uStartScan + cScanLines > (UINT)(nHeight))
            cScanLines = nHeight - uStartScan;

        if (dwHeight > cScanLines)
            dwHeight = cScanLines;
    }

#else  // COLUMN_BITMAP

    if (uStartScan > (UINT)(nHeight - 1))
    {
        SetLastError(1);
        return 0;
    }

    if (uStartScan + cScanLines > (UINT)(nHeight))
        cScanLines = nHeight - uStartScan;

    if (dwHeight > cScanLines)
        dwHeight = cScanLines;

#endif // COLUMN_BITMAP

    /* 获得pDestDC对应的显示设备 */
    pDev = DC_GetDevice(pDestDC);
    if (!pDev)
        return 0;

    // convert the logical coordinate to the device coordinate
    SetRect(&rcBlt, nDestX, nDestY, nDestX + dwWidth, nDestY + dwHeight);
    DC_LPtoDP(pDestDC, (PPOINT)&rcBlt, 2);

    nSrcY += uStartScan;

    pSrcDev = NULL;
    
    if (!DC_IsWindowDC(pDestDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDestDC, &graph_param);

        graph_param.rop = ROP_SRC;

        /* 调用设备驱动程序绘图函数 */
        pDestDC->pGraphDev->drv.DIBitBlt(pDev, &rcBlt, &DIBitmap, nSrcX, nSrcY, 
            &graph_param);
    }
    else
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDestDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        OffsetRect(&rcBlt, ptOrg.x, ptOrg.y);
        graph_param.rop = ROP_SRC;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDestDC, &rcClip);
                pDestDC->pGraphDev->drv.DIBitBlt(pDev, &rcBlt, &DIBitmap,
                    nSrcX, nSrcY, &graph_param);
            }
        }
        SHOWFRAMERECT(pDestDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDestDC);

	return (int)nHeight;
}

/*
**  Function : WS_StretchDIBits
**  Purpose  :
**      Copies a bitmap from a device independent bitmap into a destination 
**      rectangle, stretching or compressing the bitmap to fit the 
**      dimensions of the destination rectangle, if necessary. Windows
**      stretches or compresses the bitmap according to the stretching
**      mode currently set in the destination device context. 
*/
int WS_StretchDIBits(PDC pDestDC, int nDestX, int nDestY, 
                     DWORD nDestWidth, DWORD nDestHeight, int nSrcX, 
                     int nSrcY, UINT nSrcWidth,UINT nSrcHeight, 
					 CONST VOID *lpvBits, CONST BITMAPINFO *lpbmi,
					 UINT fuColorUse, DWORD dwRop)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip, rcSrc, rcDest;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    PDEV        pSrcDev;
    int         i;
    DIBITMAP    DIBitmap;
    int         nHeight;
    
    // Ensures the window object pointer isn't NULL
    ASSERT(pDestDC != NULL);
    ASSERT(lpvBits != NULL);
    ASSERT(lpbmi != NULL);
    ASSERT(lpbmi->bmiHeader.biWidth > 0);
    ASSERT(lpbmi->bmiHeader.biHeight != 0);

    if (!BMP_GetDIBITMAP(&DIBitmap, lpvBits, lpbmi, fuColorUse))
        return 0;

	nHeight = (lpbmi->bmiHeader.biHeight > 0) ? 
        (lpbmi->bmiHeader.biHeight) :-(lpbmi->bmiHeader.biHeight);
	
    /* 获得pDestDC对应的显示设备 */
    pDev = DC_GetDevice(pDestDC);
    if (!pDev)
        return 0;

    // convert the logical coordinate to the device coordinate
    SetRect(&rcSrc, nSrcX, nSrcY, nSrcX + nSrcWidth, nSrcY + nSrcHeight);
    SetRect(&rcDest, nDestX, nDestY, nDestX + nDestWidth, nDestY + nDestHeight);
    DC_LPtoDP(pDestDC, (PPOINT)&rcDest, 2);
    
    pSrcDev = NULL;
    
    if (!DC_IsWindowDC(pDestDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDestDC, &graph_param);
		graph_param.rop = dwRop;

        /* 调用设备驱动程序绘图函数 */
        pDestDC->pGraphDev->drv.DIStretchBlt(pDev, &rcDest, &DIBitmap, &rcSrc, 
            &graph_param);                        
    }
    else
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDestDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        OffsetRect(&rcDest, ptOrg.x, ptOrg.y);
        graph_param.rop = dwRop;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDestDC, &rcClip);
				pDestDC->pGraphDev->drv.DIStretchBlt(pDev, &rcDest, &DIBitmap, 
                    &rcSrc, &graph_param);                        
            }
        }
        SHOWFRAMERECT(pDestDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDestDC);

	return (int)nHeight;
}

/*
**  Function : WS_DrawBitmap
**  Purpose  :
**      Draws a compatible bitmap on the specified device.
*/
BOOL WS_DrawBitmap(PDC pDC, int x, int y, int width, int height, 
                   int srcx, int srcy, const void* pvBits, DWORD dwRop)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip, rcBlt;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    PDEV        pSrcDev;
    int         i, nRet;
    
    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);

    if (!ISDISPLAYDC(pDC))
        return FALSE;

    /* 获得pDC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return FALSE;

    // 创建源设备

    nRet = pDC->pGraphDev->drv.CreateBitmap(pDev, width, height, NULL);
    if (nRet <= 0)
    {
        // 释放对DC设备的占用
        DC_ReleaseDevice(pDC);
        return FALSE;
    }
    
    pSrcDev = MemAlloc(nRet);
    if (!pSrcDev)
    {
        // 释放对DC设备的占用
        DC_ReleaseDevice(pDC);
        return FALSE;
    }
    
    nRet = pDC->pGraphDev->drv.CreateBitmap(pDev, width, height, 
        pSrcDev);
    ASSERT(nRet > 0);

    pDC->pGraphDev->drv.SetBitmapBuffer(pDev, pSrcDev, (void*)pvBits);

    // Convert the logical coordinate to the device coordinate
    rcBlt.left   = x;
    rcBlt.top    = y;
    DC_LPtoDP(pDC, (PPOINT)&rcBlt, 1);
    rcBlt.right  = rcBlt.left + width;
    rcBlt.bottom = rcBlt.top + height;

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);
        graph_param.rop = dwRop;

        pDC->pGraphDev->drv.BitBlt(pDev, &rcBlt, pSrcDev, 
            srcx, srcy, &graph_param);
    }
    else
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        OffsetRect(&rcBlt, ptOrg.x, ptOrg.y);
        graph_param.rop = dwRop;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;
        
        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.BitBlt(pDev, &rcBlt, pSrcDev, 
                    srcx, srcy, &graph_param);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET:
    // 释放用位图数据生成的源设备
//    pDC->pGraphDev->drv.DestroyBitmap(g_pDisplayDev, pSrcDev);
    pDC->pGraphDev->drv.DestroyBitmap(pDev, pSrcDev);
    MemFree(pSrcDev);

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);

    return TRUE;
}

/*
**  Function : WS_DrawDIBitmap
**  Purpose  :
**      Draws a bitmap specified by a bitmap file on the specified DC.
**  Params   :
**      pDC         : Identifies the device context.
**      x           : Specifies the x-coordinate of the upper-left corner 
**                    of the destination rectangle. 
**      y           : Specifies the y-coordinate of the upper-left corner 
**                    of the destination rectangle. 
**      srcx        : Specifies the x-coordinate of the upper-left corner 
**                    of the source rectangle. 
**      srcy        : Specifies the y-coordinate of the upper-left corner 
**                    of the source rectangle. 
**      pDIBitmap   : Specifies the device independent bitmap file.
**      dwRop       : Specifies a raster-operation code.
*/
BOOL WS_DrawDIBitmap(PDC pDC, int x, int y, int srcx, int srcy, 
                     const DIBITMAP* pDIBitmap, DWORD dwRop)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    RECT        rcClip, rcBlt;
    GRAPHPARAM  graph_param;
    POINT       ptOrg;
    PDEV        pSrcDev;
    int         i;

    ASSERT(pDC != NULL && pDIBitmap != NULL);

    if (!ISDISPLAYDC(pDC))
        return FALSE;

    /* 获得pDestDC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return FALSE;

    // convert the logical coordinate to the device coordinate
    rcBlt.left   = x;
    rcBlt.top    = y;
    DC_LPtoDP(pDC, (PPOINT)&rcBlt, 1);
    rcBlt.right  = rcBlt.left + pDIBitmap->width;
    if (pDIBitmap->height > 0)
        rcBlt.bottom = rcBlt.top + pDIBitmap->height;
    else
        rcBlt.bottom = rcBlt.top - pDIBitmap->height;

    pSrcDev = NULL;
    
    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备，从DC中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graph_param);

        graph_param.rop = (int32)dwRop;
        pDC->pGraphDev->drv.DIBitBlt(pDev, &rcBlt, (PDIBITMAP)pDIBitmap, 
            srcx, srcy, &graph_param);
    }
    else
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graph_param))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        OffsetRect(&rcBlt, ptOrg.x, ptOrg.y);
        graph_param.rop = (int32)dwRop;
        
        /* 保存绘图参数中的裁剪区间，因为下面每一次绘图操作都要修改 */
        /* 裁减区间                                                 */
        rcClip = graph_param.clip_rect;

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graph_param.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.DIBitBlt(pDev, &rcBlt, 
                    (PDIBITMAP)pDIBitmap, srcx, srcy, &graph_param);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);

    return TRUE;
}

/*
**  Function : WS_GetTextExtent
**  Purpose  :
**      Gets the width an dheight of the specified string.
**  Params   :
**      pDC  : Pointer to device context block.
**      pStr : Pointer to string.
**      count: Size of string.
**
*/
BOOL WS_GetTextExtent(PDC pDC, PCSTR pStr, int count, PSIZE pSize)
{
    TEXTBMP TextBmp;
    int ret;

    ASSERT(pSize != NULL);
    ASSERT(count >= 0);

    if (count == 0)
    {
        pSize->cx = 0;
        pSize->cy = 0;
        return TRUE;
    }

    if (pDC)
    {
       ret = _GetStringExt(pDC->pFontDev, pStr, count, &TextBmp);
    }
    else
    {
        PFONTOBJ pFont;

        pFont = (PFONTOBJ)WOT_GetStockObj(SYSTEM_FONT);
        ret = _GetStringExt(DC_GetStockPhysObj(pDC, (PXGDIOBJ)pFont),
            pStr, count, &TextBmp);
    }
    
    if (ret < 0)
        return FALSE;

    pSize->cx = TextBmp.width;
    pSize->cy = TextBmp.height;

    return TRUE;
}

/*
**  Function : WS_GetTextExtentExPoint
**  Purpose  :
**      Gets the width an dheight of the specified string.
**  Params   :
*/
BOOL WS_GetTextExtentExPoint(PDC pDC, PCSTR pStr, int count, int nMaxExtent, 
                             PINT lpnFit, PINT alpDx, PSIZE pSize)
{
    TEXTBMP TextBmp, *pTextBmp;
    int ret;

//    ASSERT(pSize != NULL);
    ASSERT(lpnFit != NULL);
    ASSERT(count > 0);

    if (pSize == NULL)
    {
        /* 不需要得到整个串的显示大小，只考虑给定范围内的可显示大小 */
        if (nMaxExtent == 0)
        {
            /* 给定范围空，直接返回 */
            *lpnFit = 0;
            return TRUE;
        }

        pTextBmp = NULL;
    }
    else
    {
        pTextBmp = &TextBmp;
    }

    if (pDC)
    {
       ret = g_FontDrv.GetStringExtEx(pDC->pFontDev, (BYTE*)pStr, count, 
           nMaxExtent, lpnFit, alpDx, pTextBmp);
    }
    else
    {
        PFONTOBJ pFont;

        pFont = (PFONTOBJ)WOT_GetStockObj(SYSTEM_FONT);
        ret = g_FontDrv.GetStringExtEx(DC_GetStockPhysObj(pDC, (PXGDIOBJ)pFont),
           (BYTE*)pStr, count, nMaxExtent, lpnFit, alpDx, pTextBmp);
    }
    
    if (ret < 0)
        return FALSE;

    if (pSize != NULL)
    {
        pSize->cx = TextBmp.width;
        pSize->cy = TextBmp.height;
    }

    return TRUE;
}

/*
**  Function : WS_TextOut
**  Purpose  :
*/
BOOL WS_TextOut(PDC pDC, int x, int y, PCSTR pStr, int count)
{
    PEXPOSETBL  pExposeTbl;
    PDEV        pDev;
    int         i, ret;
    int         height;
    POINT       ptOrg;
    RECT        rcClip, rcText;
    int         bmpsize;
    GRAPHPARAM  graphparam;
    PFONTDEV    pFontDev;
    POINT       points;
    TMPTEXTBMP  TextBmp;
    PTEXTBMP    pTextBmp;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    ASSERT(pStr != NULL);
    ASSERT(count > 0);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return FALSE;

    pFontDev = pDC->pFontDev;
    
    bmpsize = _GetStringExt(pFontDev, pStr, count, (PTEXTBMP)&TextBmp);
    if (bmpsize <= 0)
    {
        DC_ReleaseDevice(pDC);
        return FALSE;
    }

    // convert the logical coordinate to the device coordinate
    points.x = x;
    points.y = y;
    DC_LPtoDP(pDC, &points, 1);
    x = points.x;
    y = points.y;

    /* Compute output position */
    if ((pDC->uTextAlignMode & TA_CENTER) == TA_CENTER)
        x = x - TextBmp.width / 2 + 1;
    else 
    {
        if (pDC->uTextAlignMode & TA_RIGHT)
            x = x - TextBmp.width + 1;
    }

    if ((pDC->uTextAlignMode & TA_VCENTER) == TA_VCENTER)
        y = y - TextBmp.height / 2 + 1;
    else 
    {
        if (pDC->uTextAlignMode & TA_BOTTOM)
            y = y - TextBmp.height + 1;
    }

#if (OLDFONTDRV)
    bmpsize = sizeof(TEXTBMP) + (TextBmp.width + 31) / 32 * 4 * 
        TextBmp.height;
#endif

    if (bmpsize > sizeof(TMPTEXTBMP))
    {
        pTextBmp = MemAlloc(bmpsize);
        pTextBmp->width = TextBmp.width;
        pTextBmp->height = TextBmp.height;
    }
    else
        pTextBmp = (PTEXTBMP)&TextBmp;

    // pTextBmp清零为字体驱动程序要求，以后要去掉
    memset(pTextBmp->data, 0, bmpsize - 4);

    /* Output this bitmap */
    if (!DC_IsWindowDC(pDC))
    {
        ret = g_FontDrv.GetStringBmp(pFontDev, (BYTE*)pStr, count, 0, 
            pTextBmp);
        ASSERT(ret >= 0);

        /* 非窗口设备从C中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graphparam);

        /* 调用设备驱动程序绘图函数 */
        pDC->pGraphDev->drv.TextBlt(pDev, x, y, pTextBmp, &graphparam);
    }
    else
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graphparam))
            goto RET;

        // 将绘图参数的坐标转换为屏幕坐标
        x += ptOrg.x;
        y += ptOrg.y;
        
        SetRect(&rcText, x, y, x + TextBmp.width, y + TextBmp.height);

        if (!IntersectRect(NULL, &rcText, &graphparam.clip_rect))
        {
            goto RET;
        }

        ret = g_FontDrv.GetStringBmp(pFontDev, (BYTE*)pStr, count, 0, 
            pTextBmp);
        ASSERT(ret >= 0);
        
        // 保存绘图参数中的裁剪区间.
        rcClip = graphparam.clip_rect;

        /* 对于每一个暴露块调用一次显示设备的绘图操作 */
        for (i = 0; i < pExposeTbl->count; i++)
        {
            if (IntersectRect(&graphparam.clip_rect,
                &pExposeTbl->rect[i], &rcClip))
            {
                HIDEFRAMERECT(pDC, &rcClip);
                pDC->pGraphDev->drv.TextBlt(pDev, x, y, pTextBmp, 
                    &graphparam);
            }
        }
        SHOWFRAMERECT(pDC, &rcClip);
    }

RET :

    height = pTextBmp->height;

    if (pTextBmp != (PTEXTBMP)&TextBmp)
        MemFree(pTextBmp);

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);

    return height;
}

/*
**  Function : WS_DrawText
**  Purpose  :
*/
int WS_DrawText(PDC pDC, PCSTR pStr, int count, PRECT prcClear, UINT format)
{
    PEXPOSETBL  pExposeTbl = NULL;
    PDEV        pDev;
    int         i, j, ret;
    int         feed, height = 0;
    POINT       ptOrg;
    int         x, y;
    RECT        rcClear, rcClip;
    int         bmpsize;
    GRAPHPARAM  graphparam;
    PFONTDEV    pFontDev;
    TMPTEXTBMP  TextBmp;
    PTEXTBMP    pTextBmp;

    int         iLineOfStr, iCurrentLine, iMiddleLine;
    int         iLastReturnPos, iSubStrLen;
    PCSTR       pSubStr;

    // Ensures the window object pointer isn't NULL
    ASSERT(pDC != NULL);
    ASSERT(prcClear != NULL);
    ASSERT(count >= 0);

    /* 获得DC对应的显示设备 */
    pDev = DC_GetDevice(pDC);
    if (!pDev)
        return 0;

    rcClear = *prcClear;

    // convert the logical coordinate to the device coordinate
    DC_LPtoDP(pDC, (PPOINT)&rcClear, 2);

    if (!DC_IsWindowDC(pDC))
    {
        /* 非窗口设备从C中获得绘图参数结构 */
        DC_ToGraphParam(pDC, &graphparam);

        // 清除成底色
        if (format & DT_CLEAR)
        {
            graphparam.flags &= ~GPF_BRUSH;

            /* 调用设备驱动程序绘图函数 */
            pDC->pGraphDev->drv.PatBlt(pDev, &rcClear, &graphparam);
        }
    }
    else
    {
        /* 窗口设备, 首先进行预处理 */
        if (!PreDrawWindowDC(pDC, &ptOrg, &pExposeTbl, &graphparam))
            goto RET;

        OffsetRect(&rcClear, ptOrg.x, ptOrg.y);
        // 获得绘图参数中的裁剪区间.
        if (format & DT_NOCLIP)
            rcClip = graphparam.clip_rect;
        else
            IntersectRect(&rcClip, &graphparam.clip_rect, &rcClear);
        
        // 清除成底色
        if (format & DT_CLEAR)
        {
            graphparam.flags &= ~GPF_BRUSH;
            
            /* 对于每一个暴露块调用一次显示设备的绘图操作 */
            for (i = 0; i < pExposeTbl->count; i++)
            {
                if (IntersectRect(&graphparam.clip_rect,
                    &pExposeTbl->rect[i], &rcClip))
                {
                    HIDEFRAMERECT(pDC, &rcClip);
                    pDC->pGraphDev->drv.PatBlt(pDev, &rcClear, 
                        &graphparam);
                }
            }
            SHOWFRAMERECT(pDC, &rcClip);
        }
    }

    /* 如果没有文字输出, 返回 */
    if (!pStr || count == 0)
    {
        // 释放对DC设备的占用
        DC_ReleaseDevice(pDC);
        return 0;
    }
    
    pFontDev = pDC->pFontDev;
    
    // get line numbers of pStr
    iLineOfStr = 1;
    for (i = 0; i < count; i++)
    {
        if (pStr[i] == '\n')
            iLineOfStr++;
        if ((i == count - 1) && (pStr[i] == '\r') && (pStr[i - 1] != '\n'))
            iLineOfStr ++;
        else if (pStr[i] == '\r' && pStr[i - 1] != '\n' 
            && pStr[i + 1] != '\n')
            iLineOfStr ++;
    }

    // 最后一个字符是回车符, 行数减1
    if ((pStr[i - 1] == '\n') || (pStr[i - 1] == '\r'))
        iLineOfStr--;

    pSubStr = pStr;
    iCurrentLine = 0;
    iLastReturnPos = -1;
    for (i = 0; i < count; i++)
    {
         // arrive at "\n" or the end of pStr
        if ((pStr[i] == '\n') || (pStr[i] == '\r') || (i + 1 == count))
        {
            // record the current line
            iCurrentLine++;

            // compute the length of the sub string which will be displayed.
            if ((i+1 == count) && (pStr[i] != '\n') && (pStr[i] != '\r'))
                // arrive at the end of the string
                iSubStrLen = i - iLastReturnPos;
            else                                        // arrive at the "\n"
                iSubStrLen = i - iLastReturnPos - 1;

            if (!iSubStrLen)    // the first character is "\n"
            {
                if ((pStr[i+1] == '\n') ||(pStr[i+1] == '\r'))
                    i ++;
                pSubStr = pStr + i + 1;
                iLastReturnPos = i;     // record the last position of "\n"
                continue;
            }

            bmpsize = _GetStringExt(pFontDev, pSubStr, iSubStrLen, 
                (PTEXTBMP)&TextBmp);
            ASSERT(bmpsize > 0);
            feed = TextBmp.width;
            height = TextBmp.height;

            /* Compute output position */
            if (format & DT_VCENTER)
            {
                
                if ((iLineOfStr % 2) == 1)  // the line number of pStr is odd number
                {
                    iMiddleLine = iLineOfStr/2 + 1;
                    if (iCurrentLine <= iMiddleLine)
                        y = (rcClear.top + rcClear.bottom) / 2 - TextBmp.height / 2
                                - (iMiddleLine - iCurrentLine)*TextBmp.height;
                    else
                        y = (rcClear.top + rcClear.bottom) / 2 + TextBmp.height / 2 
                                + (iCurrentLine - iMiddleLine - 1)*TextBmp.height + 1;
                }
                else                        // the line number of pStr is even number
                {
                    iMiddleLine = iLineOfStr/2;
                    if (iCurrentLine <= iMiddleLine)
                        y = (rcClear.top + rcClear.bottom) / 2
                                - (iMiddleLine - iCurrentLine + 1)*TextBmp.height;
                    else
                        y = (rcClear.top + rcClear.bottom) / 2
                                + (iCurrentLine - iMiddleLine - 1)* TextBmp.height;
                }
            }
            else if (format & DT_BOTTOM)
                y = rcClear.bottom  - (iLineOfStr - iCurrentLine + 1)*TextBmp.height;
            else    // DT_TOP
                y = rcClear.top + (iCurrentLine - 1)*TextBmp.height;

            if (format & DT_HCENTER)
                x = (rcClear.left + rcClear.right - feed) / 2;
            else if (format & DT_RIGHT)
                x = rcClear.right - feed;
            else
                x = rcClear.left;

#if (OLDFONTDRV)
            bmpsize = sizeof(TEXTBMP) + (TextBmp.width + 31) / 32 * 4 * 
                        TextBmp.height;
#endif

            if (bmpsize > sizeof(TMPTEXTBMP))
            {
                pTextBmp = MemAlloc(bmpsize);
                pTextBmp->width = TextBmp.width;
                pTextBmp->height = TextBmp.height;
            }
            else
                pTextBmp = (PTEXTBMP)&TextBmp;
            
            // pTextBmp清零为字体驱动程序要求，以后要去掉
            memset(pTextBmp->data, 0, bmpsize - 4);

            ret = g_FontDrv.GetStringBmp(pFontDev, (BYTE*)pSubStr, 
                iSubStrLen, 0, pTextBmp);
            ASSERT(ret >= 0);

            /* Output this bitmap */
            if (!DC_IsWindowDC(pDC))
            {
                /* 调用设备驱动程序绘图函数 */
                pDC->pGraphDev->drv.TextBlt(pDev, x, y, pTextBmp, 
                    &graphparam);
            }
            else
            {
                /* 对于每一个暴露块调用一次显示设备的绘图操作 */
                for (j = 0; j < pExposeTbl->count; j++)
                {
                    if (IntersectRect(&graphparam.clip_rect,
                        &pExposeTbl->rect[j], &rcClip))
                    {
                        HIDEFRAMERECT(pDC, &rcClip);
                        pDC->pGraphDev->drv.TextBlt(pDev, x, y, pTextBmp, 
                            &graphparam);
                    }
                }
                SHOWFRAMERECT(pDC, &rcClip);
            }

            // Frees the text bitmap memory
            if (pTextBmp != (PTEXTBMP)&TextBmp)
                MemFree(pTextBmp);

            // get the next sub string
            if (i < count - 1 && (pStr[i + 1] == '\n' || pStr[i + 1] == '\r'))
                i ++;
            pSubStr = pStr + i + 1;
            // record the last position of "\n"
            iLastReturnPos = i;
        }
    }

    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);

    return iLineOfStr * height;

RET:
    // 释放对DC设备的占用
    DC_ReleaseDevice(pDC);

    return 0;
}

DWORD WS_SetDefaultChar(PDC pDC, DWORD dwChar)
{
    PFONTDEV pFontDev;
    DWORD dwOldChar;

    ASSERT(pDC != NULL);

    pFontDev = pDC->pFontDev;

    dwOldChar = g_FontDrv.SetDefaultChar(pFontDev, (WORD)dwChar);

    return dwOldChar;
}


/****************************************************************************
 *
 *  Get width and height of string. Internel functon.
 *
 *  INPUT:
 *
 *  pSBCSFont   Pointer to single byte code font.
 *  pDBCSFont   Pointer to double byte code font.
 *  pStr        Pointer to string.
 *  count      Size of string.
 *  pTextBmp    Pointer to a text bitmap header.
 *  
 *
 *  RETURN:
 *  HIWORD      Height of string extend.
 *  LOWORD      Width of string extend.
 *
 *
 ****************************************************************************
 */

/*
**  Function : _GetStringExt
**  Purpose  :
**      Get width and height of string.
*/
static int _GetStringExt(PFONTDEV pFontDev, PCSTR pStr, int count, 
                         PTEXTBMP pTextBmp)
{
    return g_FontDrv.GetStringExt(pFontDev, (BYTE*)pStr, count, pTextBmp);
}

/*********************************************************************\
* Function	   PreDrawWindowDC
* Purpose      当向窗口DC绘制时，进行预处理得到各参数。
* Params	   
* Return	 	   
    如果存在暴露区要绘制，返回TRUE；否则返回FALSE
* Remarks	   
**********************************************************************/
static BOOL PreDrawWindowDC(PDC pDestDC, PPOINT pptOrg, 
                            PEXPOSETBL *ppExposeTbl, PGRAPHPARAM pGraphParam)
{
    PWINOBJ pWin;

    ASSERT(pDestDC != NULL);
    ASSERT(ppExposeTbl != NULL);
    ASSERT(pGraphParam != NULL);

    /* 窗口设备, 首先获得窗口DC对应的窗口对象 */
    pWin = DC_GetWindow(pDestDC);

    // 返回NULL表示窗口已被删除, 在Debug版本中ASSERT会报告这个错误, 
    // 在Release版本中直接返回以避免窗口系统出错
    ASSERT(pWin != NULL);
    if (pWin == NULL)
        return FALSE;

    // 获得窗口DC对应的暴露区表
    *ppExposeTbl = WND_GetExposeTbl(pWin);
    
    // 确定窗口有暴露区间。如果pExposeTbl为空指针或数值为0，表示窗口
    // 被完全覆盖。
    if (*ppExposeTbl == NULL)
        return FALSE;

    if ((*ppExposeTbl)->count < 1)
        return FALSE;

    // 获得绘图原点的坐标
    WND_GetPaintOrg(pWin, DC_GetPaintArea(pDestDC), pptOrg);

    /* 从DC中获得绘图参数结构 */
    DC_ToGraphParam(pDestDC, pGraphParam);

    return TRUE;
}

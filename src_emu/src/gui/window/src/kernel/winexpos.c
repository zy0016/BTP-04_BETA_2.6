/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements the functions which handle the expose 
 *            table of the windows
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"
#include "stdio.h"

#include "wsobj.h"
#include "wsownd.h"

// Some definitions

#define TOTAL_OVERLAY   0
#define LEFT_OVERLAY    1
#define RIGHT_OVERLAY   2
#define CENTER_OVERLAY  3

#define TOP_OVERLAY     1
#define BOTTOM_OVERLAY  2

#define MAX_EXPOSE_RECT 200     // 窗口最大暴露矩形个数

static BYTE ExposeTable[(MAX_EXPOSE_RECT - 1) * sizeof(RECT) + sizeof(EXPOSETBL)];
static BOOL bExposeTableValid = FALSE;
static PWINOBJ pWinExpose = NULL;

// Internal function prototypes.
static PEXPOSETBL MakeWindowExposeTbl(PWINOBJ pWin);
static void OverlayClip(PEXPOSETBL pExposeTbl, PRECT prcOver);
static void InvalidateWindowTree(PWINOBJ pWin, const RECT* pRect);

/*
**  Function : WND_GetWindowExposeTbl
**  Purpose  :
**      Gets the expose rectangle table of the specified window.
*/
PEXPOSETBL WND_GetExposeTbl(PWINOBJ pWin)
{
    PEXPOSETBL pExposeTbl;

    ASSERT(pWin != NULL);

    // 如果窗口为隐藏状态, 直接返回NULL
    if (!ISMAPPED(pWin))
        return NULL;

    /* 与上次计算的暴露区相同，不用重算 */
    if (pWinExpose == pWin && bExposeTableValid == TRUE)
        return (PEXPOSETBL)ExposeTable;

    // 不裁剪兄弟窗口的子窗口同时具有CS_PARENTDC类风格，暴露区使用父窗口
    // 不裁剪子窗口的暴露区
    if (ISCHILD(pWin) && ISPARENTDC(pWin) && !ISCLIPSIBLINGS(pWin))
    {
        // If the parent window hasn't WS_CLIPCHILDREN style, just return the 
        // parent window expose table
        if (!ISCLIPCHILDREN(pWin->pParent))
            pExposeTbl = WND_GetExposeTbl(pWin->pParent);
        else
        {
            // Clear WS_CLIPCHILDREN style from parent window style
            pWin->pParent->dwStyle &= ~WS_CLIPCHILDREN;
            
            // Make the expose table
            pExposeTbl = MakeWindowExposeTbl(pWin->pParent);
            
            // Set WS_CLIPCHILDREN style from parent window style
            pWin->pParent->dwStyle |= WS_CLIPCHILDREN;
        }
    }
    // 使用窗口自身的暴露区, 如果窗口暴露区域数组结构无效, 需要重新计算窗
    // 口的暴露区域
    else
    {
        pExposeTbl = MakeWindowExposeTbl(pWin);
    }

    pWinExpose = pWin;
    bExposeTableValid = TRUE;
    
    return pExposeTbl;
}

/*
**  Function : WND_MakeExposeTbl
**  Purpose  :
**      Makes the parent window expose table of the specified window. 
**      The expose table is maked without clipping children.
*/
PEXPOSETBL WND_MakeExposeTbl(PWINOBJ pWin)
{
    return MakeWindowExposeTbl(pWin);
}

/*
**  Function : WND_InvalidateExposeTbl
**  Purpose  :
**      Invalidates the expose region table of the specified window.
*/
void WND_InvalidateExposeTbl(PWINOBJ pWin)
{
    ASSERT(pWin != NULL);

    /* 只有当现有的暴露区窗口需要更新暴露区时，才作标记 
    ** 对于其他窗口可以在绘制时保证
    */
    if (pWin == pWinExpose)
        bExposeTableValid = FALSE;

    return;
}

void WND_InvalidateExposeTblTree(PWINOBJ pWin, const RECT* prcExpose)
{
    WND_InvalidateExposeTbl(pWin);
}

/*
**  Function : WND_InvalidateExposeTblBelow
**  Purpose  :
**      Invalidates expose region table of the specified window's parent 
**      window and its brother window tree.
*/
void WND_InvalidateExposeTblBelow(PWINOBJ pWin, const RECT* prcExpose)
{
    return;
}
/*
**  Function : WND_Expose
**  Purpose  :
**      Exposes a specified rectangle in the specified window and its
**      child windows.
*/
void WND_Expose(PWINOBJ pWin, const RECT* pRect, PWINOBJ pEventWin)
{
    PWINOBJ pChild;
    RECT rcExpose;

    ASSERT(pWin != NULL);
    ASSERT(pRect != NULL);
    ASSERT(!pEventWin || pEventWin->pParent == pWin);

    // If the window isn't mapped, just return.
    if (!ISMAPPED(pWin))
        return;

    // If the expose rectangle is empty rectangle(the expose rectangle is 
    // invisible), no window expose table need to be invalidated, just 
    // return
    if (!IntersectRect(&rcExpose, &pWin->rcWindow, pRect))
        return;

    /* 对于不裁减子窗口的情况，处理暴露区时应该从父窗口向下依次进行无效区域
    ** 操作，这样才能保证WM_PAINT的顺序发送。
    ** 2002/11/18，改Bug：日历程序自己的子窗口绘制错误。
    */
    if (!ISCLIPCHILDREN(pWin))
    {
        // Invalidate window expose region table
        WND_InvalidateExposeTbl(pWin);

        // Adds the expose rectangle to window invaliate rectangle
        WND_InvalidateRect(pWin, &rcExpose, TRUE);
    }

    if (pWin->pChild)
    {
        // 处理暴露的窗口不裁剪子窗口
        if (!ISCLIPCHILDREN(pWin))
        {
            BOOL bTop;
            
            if (pEventWin)
                bTop = TRUE;
            else
                bTop = FALSE;
            
            // 获得最上面的子窗口
            pChild = pWin->pChild;
            
            // 从上之下依次处理每个子窗口的暴露, 事件窗口之上的窗口暴露区域
            // 不需要失效
            while (pChild != NULL)
            {
                if (pChild == pEventWin)
                    bTop = FALSE;
                else
                {
                    // 仅处理显示的窗口
                    if (ISMAPPED(pChild))
                    {
                        if (!bTop)
                            WND_InvalidateExposeTblTree(pChild, &rcExpose);
                        InvalidateWindowTree(pChild, &rcExpose);
                    }
                }
                
                // 下一个兄弟窗口
                pChild = pChild->pNext;
            }
        }
        else
        {
            // 处理子窗口的暴露, 在事件窗口之上的窗口以及事件窗口本身不处理
            pChild = pWin->pChild;

            // 获得位于事件窗口之下的最上面的窗口, 如果事件窗口为空, 则
            // 会获得最上面的子窗口
            if (pEventWin)
            {
                while (pChild != pEventWin)
                {
                    // 如果当前窗口是显示的窗口, 将当前窗口的窗口矩形从暴露
                    // 区域中减去, 如果暴露区域减为空, 说明暴露区域影响的窗
                    // 口已经处理完, 不需要继续处理下面的窗口, 直接返回
                    if (ISMAPPED(pChild))
                    {
                        SubtractRect(&rcExpose, &rcExpose, 
                            &pChild->rcWindow);
                        if (IsRectEmpty(&rcExpose))
                            return;
                    }

                    // 下一个兄弟窗口
                    pChild = pChild->pNext;
                }

                pChild = pEventWin->pNext;
            }
            
            // 从上之下依次处理每个子窗口的暴露
            while (pChild != NULL)
            {
                // 如果当前窗口是显示的窗口, 将当前窗口的窗口矩形从暴露
                // 区域中减去, 如果暴露区域减为空, 说明暴露区域影响的窗
                // 口已经处理完, 不需要继续处理下面的窗口, 直接返回
                if (ISMAPPED(pChild))
                {
                    WND_Expose(pChild, &rcExpose, NULL);
                    SubtractRect(&rcExpose, &rcExpose, 
                        &pChild->rcWindow);
                    if (IsRectEmpty(&rcExpose))
                        return;
                }
                
                // 下一个兄弟窗口
                pChild = pChild->pNext;
            }
        }
    }

    /* 对于裁减子窗口的情况，处理暴露区时无效区域在不断减小，因此父窗口的无
    ** 效操作被放到了最后，这样可以避免多余的父窗口重绘。
    ** 2002/11/18，改Bug：日历程序自己的子窗口绘制错误。
    */
    if (ISCLIPCHILDREN(pWin))
    {
        // Invalidate window expose region table
        WND_InvalidateExposeTbl(pWin);

        // Adds the expose rectangle to window invaliate rectangle
        WND_InvalidateRect(pWin, &rcExpose, TRUE);
    }
}

// Internal function prototypes

/*
**  Function : MakeWindowExposeTbl
**  Purpose  :
**      生成一个窗口中指定的矩形区域的暴露块数组结构
**  Params   :
**      pWin    : Specifies the window to be make expose table.
**      pRect   : Specifies the rectangle to expose. if NULL, indicates 
**                all window rectangle expose.
**  Remark   :
**      可能覆盖一个窗口的窗口有：
**          1.   该窗口的直接子窗口；
**          2.   该窗口的兄弟窗口；
**          3.   该窗口的祖先窗口的兄弟窗口；
**      另外，窗口除了被覆盖之外，还可能被祖先窗口裁剪。
*/
static PEXPOSETBL MakeWindowExposeTbl(PWINOBJ pWin)
{
//    static BYTE TmpBuf[(MAX_EXPOSE_RECT - 1) * sizeof(RECT) + 
//        sizeof(EXPOSETBL)];

    PEXPOSETBL pExposeTbl;
    PWINOBJ pTemp, pParent;
    RECT rect;
    PRECT pRect;

    ASSERT(ISMAPPED(pWin));

    // If window too small, don't draw it
    if (pWin->rcWindow.right - pWin->rcWindow.left < 2 || 
        pWin->rcWindow.bottom - pWin->rcWindow.top < 2)
        return NULL;

    pExposeTbl = (PEXPOSETBL)ExposeTable;

    // 初始值为整个窗口矩形
    pExposeTbl->count = 1;
    pExposeTbl->clientblks = 1;
    CopyRect(&pExposeTbl->rect[0], &pWin->rcWindow);

    // 首先计算祖先窗口的裁减，祖先窗口会限制窗口的暴露区域
    pRect = &pExposeTbl->rect[0];
    pParent = pWin;
    while (!ISROOTWIN(pParent))
    {
        if (ISNCCHILD(pParent))
        {
            if (!IntersectRect(pRect, pRect, &pParent->pParent->rcWindow))
                return NULL;
        }
        else
        {
            if (!IntersectRect(pRect, pRect, &pParent->pParent->rcClient))
                return NULL;
        }

        pParent = pParent->pParent;
    }
    
    // 是否被完全覆盖
    if (IsRectEmpty(pExposeTbl->rect))
        return NULL;

    // 计算被兄弟窗口和祖先窗口的兄弟覆盖
    pParent = pWin;
    while (!ISROOTWIN(pParent))
    {
        // 如果裁减兄弟窗口，考虑兄弟窗口的覆盖
        if (ISCLIPSIBLINGS(pParent))
        {
            pTemp = pParent->pParent->pChild;
            while (pTemp != pParent)
            {
                if (ISMAPPED(pTemp))
                {
                    OverlayClip(pExposeTbl, &pTemp->rcWindow);
                    if (pExposeTbl->count == 0)
                        return NULL;
                }

                pTemp = pTemp->pNext;
            }
        }

        pParent = pParent->pParent;
    } 

    // 处理被子窗口的裁剪
    if (ISCLIPCHILDREN(pWin))
    {
        pTemp = pWin->pChild;
        while (pTemp != NULL)
        {
            if (ISMAPPED(pTemp))
            {
                // Must clip the child window using the window client 
                // rectangle or window rectangle
                if (ISNCCHILD(pTemp))
                    IntersectRect(&rect, &pTemp->rcWindow, &pWin->rcWindow);
                else
                    IntersectRect(&rect, &pTemp->rcWindow, &pWin->rcClient);

                OverlayClip(pExposeTbl, &rect);

                if (pExposeTbl->count == 0)
                    return NULL;
            }

            pTemp = pTemp->pNext;
        }
    }

    return pExposeTbl;
}

/*
**  Function : RT_Delete
**  Purpose  :
**      Delete one rectangle from table.
*/
static void RT_Delete(PEXPOSETBL pExposeTbl, int index) 
{
    pExposeTbl->count--;

    if (index < pExposeTbl->clientblks)
        pExposeTbl->clientblks --;

    if (pExposeTbl->count > 0)
    {
        memmove(&pExposeTbl->rect[index], &pExposeTbl->rect[index + 1], 
            (pExposeTbl->count - index) * sizeof(RECT));
    }
}

/*
**  Function : RT_Insert
**  Purpose  :
**      Insert some items before index. 
*/
static void RT_Insert(PEXPOSETBL pExposeTbl, int index, int count) 
{
    // 确保窗口暴露矩形的个数小于MAX_EXPOSE_RECT
    ASSERT(pExposeTbl->count + count <= MAX_EXPOSE_RECT);
    if (pExposeTbl->count + count > MAX_EXPOSE_RECT)
        count = MAX_EXPOSE_RECT - pExposeTbl->count;

    if (pExposeTbl->count > index)
    {
        memmove(&pExposeTbl->rect[index + count], &pExposeTbl->rect[index],
            (pExposeTbl->count - index) * sizeof(RECT));
    }

    pExposeTbl->count += count;

    if (index <= pExposeTbl->clientblks)
        pExposeTbl->clientblks += count;
}

static void OverlayClip(PEXPOSETBL pExposeTbl, PRECT prcOver)
{
    PRECT   pRC, pSplit1, pSplit2, pSplit3;
    int     i;
    BYTE    xOverlay, yOverlay;
    
    for (i = pExposeTbl->count - 1; i >= 0; i --)
    {
        pRC = &pExposeTbl->rect[i];

        /* Check if no overlay */
        if (prcOver->right <= pRC->left || pRC->right <= prcOver->left ||
            prcOver->bottom <= pRC->top || pRC->bottom <= prcOver->top)
            continue;
        
        /* Check if total overlapped by overlay window */
        if (prcOver->left <= pRC->left && pRC->right <= prcOver->right &&
            prcOver->top <= pRC->top && pRC->bottom <= prcOver->bottom)
        {
            // Total overlapped, delete this node.
            RT_Delete(pExposeTbl, i);

            continue;
        }
        
        // Check overlap mode.
        if (prcOver->left > pRC->left)
        {
            if (pRC->right > prcOver->right)
                xOverlay = CENTER_OVERLAY;
            else
                xOverlay = RIGHT_OVERLAY;
        }
        else    // (prcOver->left <= pRC->left)
        {
            if (pRC->right <= prcOver->right)
                xOverlay = TOTAL_OVERLAY;
            else
                xOverlay = LEFT_OVERLAY;
        }

        if (prcOver->top > pRC->top)
        {
            if (pRC->bottom > prcOver->bottom)
                yOverlay = CENTER_OVERLAY;
            else
                yOverlay = BOTTOM_OVERLAY;
        }
        else    // (prcOver->top <= pRC->top)
        {
            if (pRC->bottom <= prcOver->bottom)
                yOverlay = TOTAL_OVERLAY;
            else
                yOverlay = TOP_OVERLAY;
        }

        switch ((xOverlay << 2) | yOverlay)
        {
        case (TOTAL_OVERLAY << 2) | TOTAL_OVERLAY :

            // Total overlied, delete this node.
            //
            // +---------------+    
            // | +---------+   |
            // | |         |   |
            // | |         |   |
            // | |         |   |
            // | |         |   |
            // | |         |   |
            // | +---------+   |
            // +---------------+
            //

            RT_Delete(pExposeTbl, i);

            break;

        case (TOTAL_OVERLAY << 2) | TOP_OVERLAY:

            // Change window Y1.
            //
            //  +----------------+  
            //  | +----------+   |
            //  | |          |   |
            //  | |          |   |
            //  +-+----------+---+
            //    |          |
            //    |    1     |
            //    +----------+
            //

            pRC->top = prcOver->bottom;

            break;

        case (TOTAL_OVERLAY << 2) | BOTTOM_OVERLAY:

            // Change window Y2.
            //
            //    +-----------+ 
            //    |           |
            //    |     1     |
            //    |           |
            // +--+-----------+---+
            // |  |           |   |
            // |  +-----------+   |
            // +------------------+
            //

            pRC->bottom = prcOver->top;

            break;

        case (LEFT_OVERLAY << 2) | TOTAL_OVERLAY:

            // Change window X1
            //
            // +--------+
            // |  +-----+----+
            // |  |     |    |
            // |  |     | 1  |
            // |  |     |    |
            // |  +-----+----+
            // |        |    
            // +--------+

            pRC->left = prcOver->right;

            break;

        case (RIGHT_OVERLAY << 2) | TOTAL_OVERLAY:

            // Change window X2
            //         +------+
            //         |      |
            //  +------+---+  |
            //  |      |   |  |
            //  | 1    |   |  |
            //  |      |   |  |
            //  +------+---+  |
            //         |      |
            //         +------+

            pRC->right = prcOver->left;

            break;

        case (TOTAL_OVERLAY << 2) | CENTER_OVERLAY:

            // Split this rectangle into 2 small rectangles.
            //
            //    +----------+
            //    |    1     |
            // +--+----------+--+
            // |  |          |  |
            // |  |          |  |
            // +--+----------+--+
            //    |          |
            //    |    2     |
            //    +----------+
            //

            RT_Insert(pExposeTbl, i+1, 1);

            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit1->left = pRC->left;
            pSplit1->right = pRC->right;
            pSplit1->top = prcOver->bottom;
            pSplit1->bottom = pRC->bottom;
            pRC->bottom = prcOver->top;

            break;

        case (CENTER_OVERLAY << 2) | TOTAL_OVERLAY:

            // Split window into 2 small windows.
            //
            //        +------+
            //        |      |
            // +------+------+-----+
            // |      |      |     |
            // |      |      |     |
            // |  1   |      |  2  |
            // |      |      |     |
            // +------+------+-----+
            //        |      |
            //        +------+
            //

            RT_Insert(pExposeTbl, i+1, 1);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit1->left = prcOver->right;
            pSplit1->right = pRC->right;
            pSplit1->top = pRC->top;
            pSplit1->bottom = pRC->bottom;
            pRC->right = prcOver->left;

            break;

        case (LEFT_OVERLAY << 2) | TOP_OVERLAY:

            // Split window into 2 small windows.
            //
            // +-------+
            // |       |
            // |  +----+-------+
            // |  |    |       |
            // |  |    |   1   |
            // +--+----+*******|
            //    |            |
            //    |      2     |
            //    +------------+
            //

            RT_Insert(pExposeTbl, i+1, 1);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit1->left = pRC->left;
            pSplit1->right = pRC->right;
            pSplit1->top = prcOver->bottom;
            pSplit1->bottom = pRC->bottom;
            pRC->left = prcOver->right;
            pRC->bottom = prcOver->bottom;
            break;

        case (RIGHT_OVERLAY << 2) | TOP_OVERLAY :

            // Split window into 2 small windows.
            //
            //       +--------+ 
            // +-----+-----   |
            // |     |    |   |
            // |  1  |    |   |
            // |*****+----+---+
            // |          |
            // |     2    |
            // +----------+
            //

            RT_Insert(pExposeTbl, i+1, 1);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit1->left = pRC->left;
            pSplit1->right = pRC->right;
            pSplit1->top = prcOver->bottom;
            pSplit1->bottom = pRC->bottom;
            pRC->right = prcOver->left;
            pRC->bottom = prcOver->bottom;
            break;

        case (LEFT_OVERLAY << 2) | BOTTOM_OVERLAY :

            // Split window into 2 small windows.
            //       +------------+
            //       |            |
            //       |    1       |
            //       |            |
            //   +---+------+*****|
            //   |   |      |     |
            //   |   |      |  2  |
            //   |   +------+-----+
            //   |          |
            //   +----------+
            //

            RT_Insert(pExposeTbl, i+1, 1);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit1->left = prcOver->right;
            pSplit1->right = pRC->right;
            pSplit1->top = prcOver->top;
            pSplit1->bottom = pRC->bottom;
            pRC->bottom = prcOver->top;
            break;

        case (RIGHT_OVERLAY << 2) | BOTTOM_OVERLAY :

            // Split window into 2 small windows.
            // +------------+
            // |            |
            // |    1       |
            // |*******+----+----+
            // |       |    |    |
            // |    2  |    |    |
            // |       |    |    |
            // +-------+----+    |
            //         |         |
            //         +---------+
            //

            RT_Insert(pExposeTbl, i+1, 1);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit1->left = pRC->left;
            pSplit1->right = prcOver->left;
            pSplit1->top = prcOver->top;
            pSplit1->bottom = pRC->bottom;
            pRC->bottom = prcOver->top;

            break;

        case (LEFT_OVERLAY << 2) | CENTER_OVERLAY:

            // Split window into 3 small windows.
            //
            //      +------------+
            //      |            |
            //      |     1      |
            //  +---+---+********|
            //  |   |   |  2     |
            //  |   |   |        |
            //  +---+---+********|
            //      |            |
            //      |      3     |
            //      +------------+

            RT_Insert(pExposeTbl, i+1, 2);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit2 = &pExposeTbl->rect[i+2];
            pSplit2->left = pRC->left;
            pSplit1->left = prcOver->right;
            pSplit1->right = pSplit2->right = pRC->right;

            pSplit2->bottom = pRC->bottom;
            pSplit1->bottom = pSplit2->top = prcOver->bottom;
            pRC->bottom = pSplit1->top = prcOver->top;
            break;

        case (RIGHT_OVERLAY << 2) | CENTER_OVERLAY :

            // Split window into 3 small windows.
            //
            //  +------------+
            //  |            |
            //  |     1      |
            //  |********+---+---+
            //  |     2  |   |   |
            //  |        |   |   |
            //  |********+---+---+
            //  |            |
            //  |      3     |
            //  +------------+

            RT_Insert(pExposeTbl, i+1, 2);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit2 = &pExposeTbl->rect[i+2];
            pSplit1->left = pSplit2->left = pRC->left;
            pSplit1->right = prcOver->left;
            pSplit2->right = pRC->right;
            
            pSplit2->bottom = pRC->bottom;
            pSplit1->bottom = pSplit2->top = prcOver->bottom;
            pRC->bottom = pSplit1->top = prcOver->top;
            break;

        case (CENTER_OVERLAY << 2) | TOP_OVERLAY :

            // Split window into 3 small windows.
            //
            //         +-----+
            //         |     |
            // +-------+-----+------+
            // |       |     |      |
            // |   1   |     |  2   |
            // |*******+-----+******|
            // |                    |
            // |          3         |
            // +--------------------+
            RT_Insert(pExposeTbl, i+1, 2);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit2 = &pExposeTbl->rect[i+2];
            pSplit1->left = prcOver->right;
            pSplit1->right = pSplit2->right = pRC->right;
            pSplit2->left = pRC->left;
            pRC->right = prcOver->left;
            
            pSplit1->top = pRC->top;
            pSplit2->bottom = pRC->bottom;
            pSplit1->bottom = pSplit2->top = pRC->bottom = prcOver->bottom;
            break;

        case (CENTER_OVERLAY << 2) | BOTTOM_OVERLAY :

            // Split window into 3 small windows.
            //
            // +--------------------+
            // |                    |
            // |          1         |
            // |*******+-----+******|
            // |   2   |     |  3   |
            // |       |     |      |
            // +-------+-----+------+
            //         |     |
            //         +-----+

            RT_Insert(pExposeTbl, i+1, 2);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit2 = &pExposeTbl->rect[i+2];
            pSplit1->left = pRC->left;
            pSplit1->right = prcOver->left;
            pSplit2->left = prcOver->right;
            pSplit2->right = pRC->right;
            
            pSplit1->bottom = pSplit2->bottom = pRC->bottom;
            pRC->bottom = pSplit1->top = pSplit2->top = prcOver->top;

            break;

        case (CENTER_OVERLAY << 2) | CENTER_OVERLAY :

            // Split window into 4 small windows.
            // +---------------+    
            // |      1        |
            // |***+-------+***|
            // |   |       |   |
            // | 2 |       | 3 |
            // |   |       |   |
            // |***+-------+***|
            // |       4       |
            // +---------------+
            //

            RT_Insert(pExposeTbl, i+1, 3);
            pSplit1 = &pExposeTbl->rect[i+1];
            pSplit2 = &pExposeTbl->rect[i+2];
            pSplit3 = &pExposeTbl->rect[i+3];
            pSplit2->right = pSplit3->right = pRC->right;
            pSplit1->left = pSplit3->left = pRC->left;
            pSplit1->right = prcOver->left;
            pSplit2->left = prcOver->right;
            
            pSplit3->bottom = pRC->bottom;
            pSplit1->bottom = pSplit2->bottom = pSplit3->top = prcOver->bottom;
            pRC->bottom = pSplit1->top = pSplit2->top = prcOver->top;

            break;
        }   /* End of switch */
    }       /* End of while  */
}

/*
**  Function : InvalidateWindowTree
*/
static void InvalidateWindowTree(PWINOBJ pWin, const RECT* pRect)
{
    PWINOBJ pChild;
    RECT rect;

    ASSERT(pWin != NULL);
    ASSERT(pRect != NULL);
    ASSERT(ISMAPPED(pWin));

    if (!IntersectRect(&rect, pRect, &pWin->rcWindow))
        return;

    if (!ISCLIPCHILDREN(pWin))
    {
        // 窗口不裁剪子窗口

        // Invalidates the specified window
        WND_InvalidateRect(pWin, &rect, TRUE);
        
        // invalidates the window's child windows whose window rectangle is 
        // intersected with the specified rectangle.
        pChild = pWin->pChild;
        while (pChild)
        {
            // 注意: 这里不能使用pRect来判断子窗口是否需要更新, 因为pRect
            // 并不是当前窗口最终的失效区. 由于失效区的合并操作, 窗口的失
            // 效区可能会大于指定的失效区. 因此应该使用窗口结构中的失效区
            // 判断子窗口是否需要失效
            if (ISMAPPED(pChild) && 
                IntersectRect(&rect, &pChild->rcWindow, &pWin->rcUpdate))
                InvalidateWindowTree(pChild, &rect);
            
            pChild = pChild->pNext;
        }
    }
    else
    {
        // 窗口裁剪子窗口

        if (pWin->pChild)
        {
            // 获得最上面子窗口
            pChild = pWin->pChild;
            
            // 从上之下依次处理每个子窗口的暴露
            while (pChild != NULL)
            {
                // 如果当前窗口是显示的窗口, 将当前窗口的窗口矩形从暴露
                // 区域中减去, 如果暴露区域减为空, 说明暴露区域影响的窗
                // 口已经处理完, 不需要继续处理下面的窗口, 直接返回
                if (ISMAPPED(pChild))
                {
                    InvalidateWindowTree(pChild, &rect);
                    SubtractRect(&rect, &rect, 
                        &pChild->rcWindow);
                }
                
                // 前一个兄弟窗口
                pChild = pChild->pNext;
            }
        }

        // Invalidates the specified window
        WND_InvalidateRect(pWin, &rect, TRUE);
    }
}

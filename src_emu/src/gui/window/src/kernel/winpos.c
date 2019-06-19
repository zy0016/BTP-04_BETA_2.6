/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Move and resize the window.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "wsobj.h"
#include "wsownd.h"

// Internal function prototypes
static void MoveChildren(PWINOBJ pWin, int dx, int dy);
static void MoveWindowTreeToTop(PWINOBJ pWin, PWINOBJ pEndWin);
static BOOL ScrollChildren(PWINOBJ pWin, int dx, int dy, 
                           const RECT* prcClip, BOOL bTop);
static BOOL RectInRect(const RECT* pRect1, const RECT* pRect2);

/*
**  Function : WND_Move
**  Purpose  :
**      Changes the position and dimensions of the specified window.
**  Remark   :
**      暂时忽略bRepaint参数，并且移动的窗口本身全部重画。
*/
void WND_Move(PWINOBJ pWin, int x, int y, int width, int height,
              BOOL bRepaint)
{
    NCCALCSIZE_PARAMS ncsp;
    RECT rcWindow, rcClient, rcExpose;

    if (width < 0 || height < 0)
        return;

    if (ISROOTWIN(pWin))
    {
        pWin->rcWindow.left = x;
        pWin->rcWindow.top = y;
        pWin->rcWindow.right = x + width;
        pWin->rcWindow.bottom = y + height;

        pWin->rcClient = pWin->rcWindow;

        // Expose the whole window area
        WND_Expose(pWin, &pWin->rcWindow, NULL);

        return;
    }

    // Save old window rectangle and old client window
    rcWindow = pWin->rcWindow;
    rcClient = pWin->rcClient;

    // Calculates the new window rectangle before send WM_NCCALCSIZE
    if (ISNCCHILD(pWin))
    {
        pWin->rcWindow.left = x + pWin->pParent->rcWindow.left;
        pWin->rcWindow.top = y + pWin->pParent->rcWindow.top;
    }
    else
    {
        pWin->rcWindow.left = x + pWin->pParent->rcClient.left;
        pWin->rcWindow.top = y + pWin->pParent->rcClient.top;
    }

    pWin->rcWindow.right = pWin->rcWindow.left + width;
    pWin->rcWindow.bottom = pWin->rcWindow.top + height;

    // 注意，此时窗口的客户区矩形尚未计算，因此，在处理WM_NCCALCSIZE消息，
    // 不能创建或移动客户区子窗口，但可以创建或移动非客户区子窗口

    // Calculates the client size by sending WM_NCCALCSIZE message

    // rgrc[0] : New window rectangle
    // rgrc[1] : Old window rectangle
    // rgrc[2] : Old client rectangle
    // lppos   : Not used now
    CopyRect(&ncsp.rgrc[0], &pWin->rcWindow);
    CopyRect(&ncsp.rgrc[1], &rcWindow);
    CopyRect(&ncsp.rgrc[2], &rcClient);
    ncsp.lppos = NULL;
    
    // Modifies the window's child window's coordinate, set to new position
    MoveChildren(pWin, pWin->rcWindow.left - rcWindow.left, 
        pWin->rcWindow.top - rcWindow.top);
    
    WND_SendMessage(pWin, WM_NCCALCSIZE, TRUE, (LPARAM)&ncsp);
    WND_NCCalcSize(pWin, &ncsp.rgrc[0]);


    // Send WM_SIZE message if the client size of window has changed
    if ((pWin->rcClient.right - pWin->rcClient.left != 
         rcClient.right - rcClient.left) || 
        (pWin->rcClient.bottom - pWin->rcClient.top != 
         rcClient.bottom - rcClient.top))
    {
        WND_SendNotifyMessage(pWin, WM_SIZE, (WPARAM)0,
            MAKELPARAM(pWin->rcClient.right - pWin->rcClient.left, 
            pWin->rcClient.bottom - pWin->rcClient.top));
    }

    // Send WM_MOVE message if the client size of window has changed
    if ((pWin->rcClient.left != rcClient.left) || 
        (pWin->rcClient.top != rcClient.top))
    {
        WND_SendNotifyMessage(pWin, WM_MOVE, (WPARAM)0,
            MAKELPARAM(pWin->rcClient.left, pWin->rcClient.top));
    }

    // The window is unmapped, just return
    if (!ISMAPPED(pWin))
        return;

    // The window isn't really moved, just return
    if (EqualRect(&pWin->rcWindow, &rcWindow) &&
        EqualRect(&pWin->rcClient, &rcClient))
        return;

    // Expose proper expose area for the window and the parent window
/*
    if (rcOld.left == pWin->ScreenX && rcOld.top == pWin->ScreenY &&
        (rcOld.right - rcOld.left == pWin->width ||
        rcOld.bottom - rcOld.top == pWin->height))
    {
        if (rcOld.right - rcOld.left == pWin->width)
        {
            if (rcOld.bottom - rcOld.top > pWin->height)
            {
                rcExpose.top = rcNew.bottom - pWin->wBottomNC;
            }
            else
            {
                rcExpose.top = rcOld.bottom - pWin->wBottomNC;
            }
            
            rcExpose.left = rcNew.left;
            rcExpose.right = rcNew.right;
            rcExpose.bottom = rcNew.bottom;
        }
        else
        {
            if (rcOld.right - rcOld.left > pWin->width)
            {
                rcExpose.left = rcNew.right - pWin->wRightNC;
            }
            else
            {
                rcExpose.left = rcOld.right - pWin->wRightNC;
            }
            
            rcExpose.top = rcNew.top;
            rcExpose.right = rcNew.right;
            rcExpose.bottom = rcNew.bottom;
        }
        
        IntersectRect(&rcExpose, &rcExpose, &pWin->rcWindow);
    }
    else
        CopyRect(&rcExpose, &pWin->rcWindow);
*/
    
    // Exposes proper area of parent window : rcOld - rcNew
    SubtractRect(&rcExpose, &rcWindow, &pWin->rcWindow);
    WND_Expose(pWin->pParent, &rcExpose, pWin);
    
    // Exposes proper area of the moving window
    CopyRect(&rcExpose, &pWin->rcWindow);
    WND_Expose(pWin, &rcExpose, NULL);
    

    // Invalidates the expose region table of below new window rectandle
    WND_InvalidateExposeTblBelow(pWin, &pWin->rcWindow);
}

/*
**  Function : WND_MoveToTop
**  Purpose  :
**      Moves a specified window to its brother list's top.
*/
void WND_MoveToTop(PWINOBJ pWin)
{
    PWINOBJ pEndWin;

    ASSERT(pWin != NULL);

    // The root window needn't be moved to top, because all other windows
    // are the descendant window of the root window
    if (ISROOTWIN(pWin))
        return;

    if (!ISTOPMOST(pWin) && ISTOPMOST(pWin->pParent->pChild))
    {
        pEndWin = pWin->pParent->pChild;
        while (ISTOPMOST(pEndWin->pNext))
            pEndWin = pEndWin->pNext;
    }
    else
        pEndWin = NULL;

   MoveWindowTreeToTop(pWin, pEndWin);
}

/*
**  Function : WND_ScrollChild
**  Purpose  :
**      Scrolls all descendent window of the specified window. 
*/
void WND_ScrollChild(PWINOBJ pWin, int dx, int dy, const RECT* prcClip, 
                     BOOL bTop)
{
    ASSERT(pWin != NULL);
    ASSERT(prcClip != NULL);

    // 不支持root window的滚动
    if (ISROOTWIN(pWin))
        return;

    if (pWin->pChild)
    {
        // 完成窗口后代窗口的滚动, 如果由于滚动这些窗口在裁剪区域产生新的
        // 暴露区域, 使这些新的暴露区域失效.
        if (ScrollChildren(pWin, dx, dy, prcClip, bTop))
        {
            // 如果窗口暴露区考虑子窗口的裁剪, 使窗口的暴露区表失效
            if (ISCLIPCHILDREN(pWin))
                WND_InvalidateExposeTbl(pWin);
        }
    }
}

// Internal functions

/*
**  Function : MoveChildren
**  Purpose  :
**      Moves a specified window's child windows to a new position.
*/
static void MoveChildren(PWINOBJ pWin, int dx, int dy)
{
    PWINOBJ pChild;
    BOOL bFromChild;


/* */
    /* 在处理窗口树的过程中，遍历方法是：
    ** 使用前序遍历方式调用OffsetRect
    */
    pChild = pWin->pChild;
    bFromChild = FALSE;
    while (pChild)
    {
        if (!bFromChild)
        {
            OffsetRect(&pChild->rcWindow, dx, dy);
            OffsetRect(&pChild->rcClient, dx, dy);
        }

        // 获得窗口树中下一个需要处理的窗口，原则如下：
        //     * 如果有子窗口需要处理，则当前窗口的子窗口为要处理的窗口。
        //     * 没有子窗口需要处理但有下一个兄弟窗口，则当前窗口的下一个兄
        //       弟窗口为要处理的窗口
        //     * 当前窗口没有子窗口和兄弟窗口需要处理，则取当前窗口的祖先窗
        //       口的兄弟窗口为要处理的窗口，如果回溯到起始祖先窗口，表示所
        //       有窗口已经处理完毕。
        if (pChild->pChild && !bFromChild)
            pChild = pChild->pChild;
        else
        {
            if (pChild->pNext)
            {
                pChild = pChild->pNext;
                bFromChild = FALSE;
            }
            else
            {
                pChild = pChild->pParent;
                if (pChild == pWin)
                {
                    pChild = NULL;
                    break;
                }

                bFromChild = TRUE;
            }
        }
    }
}

/*
**  Function : MoveWindowTreeToTop
**  Purpose  :
**      Moves a specified a window and its owned window to top.
*/
static void MoveWindowTreeToTop(PWINOBJ pWin, PWINOBJ pEndWin)
{
    PWINOBJ pTempWin, pLastWin;
    PWINOBJ pOwnerWin, pOwnedWin;
    RECT    rect;
    int     nOwnWins;

    pLastWin = pWin;
    if (ISPOPUP(pWin))
    {
        // 如果有属主窗口，首先将属主窗口MoveToTop
        if (pWin->pOwner)
            MoveWindowTreeToTop(pWin->pOwner, pEndWin);

        // 找出最后一个pWin拥有的窗口，包括直接拥有和间接拥有的窗口
        while (pLastWin->byOwnWins)
        {
            pOwnerWin = pLastWin;
            nOwnWins = pLastWin->byOwnWins;
            while (nOwnWins != 0)
            {
                pLastWin = pLastWin->pPrev;
                ASSERT(pLastWin != NULL);
                
                if (pLastWin->pOwner == pOwnerWin)
                    nOwnWins--;
            }
        }
    }

    // pLastWin的下一个窗口是pEndWin说明已经是合适的z-order了
    if (pLastWin->pPrev == pEndWin)
        return;

    // 此时pLastWin肯定不是第一个的窗口
    ASSERT(pLastWin->pPrev != NULL);

    // 计算pWin到pLastWin的一组窗口移动到pEndWin之下引起的窗口失效和窗口暴
    // 露区的失效
    pOwnedWin = pWin;
    do
    {
        pTempWin = pLastWin->pPrev;
        do
        {
            if (ISMAPPED(pOwnedWin) && ISMAPPED(pTempWin) &&
                IntersectRect(&rect, &pOwnedWin->rcWindow, 
                &pTempWin->rcWindow))
            {
                // Exposes owned window
                WND_Expose(pOwnedWin, &rect, NULL);
                
                // Invalidates expose region table of the topper window
                // and its child window
                WND_InvalidateExposeTblTree(pTempWin, &rect);
            }

            pTempWin = pTempWin->pPrev;
        }while (pTempWin != pEndWin);

        pOwnedWin = pOwnedWin->pPrev;
    }while (pOwnedWin != pLastWin->pPrev);

    // 将pWin到pLastWin的一组窗口从窗口链表中摘下，加入到pEndWin下边

    if (pWin->pNext)
        pWin->pNext->pPrev = pLastWin->pPrev;
    ASSERT(pLastWin->pPrev);
    pLastWin->pPrev->pNext = pWin->pNext;

    if (pEndWin)
    {
        pWin->pNext = pEndWin->pNext;
        if (pEndWin->pNext)
            pEndWin->pNext->pPrev = pWin;
        pEndWin->pNext = pLastWin;
        pLastWin->pPrev = pEndWin;
    }
    else
    {
        ASSERT(pWin->pParent->pChild);
        pWin->pNext = pWin->pParent->pChild;
        pWin->pParent->pChild->pPrev = pWin;
        pWin->pParent->pChild = pLastWin;
        pLastWin->pPrev = NULL;
    }
}

/*
**  Function : ScrollChildren
**  Purpose  :
**      完成窗口pChild及其后代窗口的滚动, 如果由于滚动这些窗口产生新的
**      暴露区域, 使这些新的暴露区域失效.
*/
static BOOL ScrollChildren(PWINOBJ pWin, int dx, int dy, 
                           const RECT* prcClip, BOOL bTop)
{
    PWINOBJ pChild;
    RECT rcOld, rcNew;
    RECT rcUpdate;
    BOOL bUpdate;
    BOOL bFromChild;

    ASSERT(pWin != NULL);
    ASSERT(pWin->pChild != NULL);
    ASSERT(!ISROOTWIN(pWin));
    ASSERT(prcClip != NULL);

    pChild = pWin->pChild;
    while (ISNCCHILD(pChild))
        pChild = pChild->pNext;
    
    // 没有客户区子窗口，返回FALSE表示没有子窗口需要移动
    if (!pChild)
        return FALSE;

    // bUpdate用来表示窗口是否需要判断由于滚动产生新的失效区域，如果prcClip
    // 为空矩形，则没有窗口产生新的失效区域。实际上如果父窗口不产生新的失效
    // 区域，则子窗口不需要再判断是否产生新的失效区域。这里为了简化程序，不
    // 做这种判断，只用prcClip是否为空矩形来决定是否判断窗口的失效。
    bUpdate = !IsRectEmpty(prcClip);

/* */
    /* 在处理窗口树的过程中，遍历方法是：
    ** 使用前序遍历方式调用,先处理根节点
    */
    bFromChild = FALSE;
    while (pChild)
    {
        if (ISCLIENTCHILD(pChild) && !bFromChild)
        {
            /* 
            ** the unmapped child window should not be invalidate the 
            ** update rect.
            */
            if (bTop && ISMAPPED(pChild))
            {
                // 不会产生新的失效区域, 直接修改窗口的位置
                if (!bUpdate)
                {
                    // 移动窗口位置
                    OffsetRect(&pChild->rcWindow, dx, dy);
                    OffsetRect(&pChild->rcClient, dx, dy);
                    
                    // 如果窗口的更新区域不为空, 移动更新区域
                    if (!IsRectEmpty(&pChild->rcUpdate))
                        OffsetRect(&pChild->rcUpdate, dx, dy);
                    
                    // 由于窗口位置移动, 所以使窗口的暴露区域表失效
                    WND_InvalidateExposeTbl(pChild);
                }
                else
                {
                    // 将窗口滚动前在裁剪区域中的区域以窗口坐标保存在rcOld
                    // 中, 坐标转换前必须首先判断举行是否是空矩形
                    if (IntersectRect(&rcOld, prcClip, &pChild->rcWindow))
                        OffsetRect(&rcOld, -pChild->rcWindow.left, 
                        -pChild->rcWindow.top);
                    
                    // 移动窗口位置
                    OffsetRect(&pChild->rcWindow, dx, dy);
                    OffsetRect(&pChild->rcClient, dx, dy);
                    
                    // 如果窗口的更新区域不为空, 移动更新区域
                    if (!IsRectEmpty(&pChild->rcUpdate))
                        OffsetRect(&pChild->rcUpdate, dx, dy);
                    
                    // 由于窗口位置移动, 所以使窗口的暴露区域表失效, 必须
                    // 在调用WND_InvalidateRect之前调用
                    // WND_InvalidateExposeTbl, 否则调用WND_InvalidateRect
                    // 可能非正常返回。
                    WND_InvalidateExposeTbl(pChild);
                    
                    // 将窗口滚动后在裁剪区域中的区域以窗口坐标保存在rcNew
                    // 中, 坐标转换前必须首先判断举行是否是空矩形
                    if (IntersectRect(&rcNew, prcClip, &pChild->rcWindow))
                        OffsetRect(&rcNew, -pChild->rcWindow.left, 
                        -pChild->rcWindow.top);
                    
                    // 如果窗口滚动后在裁剪区中的区域没有完全包含在滚动前
                    // 在裁剪区中的区域中, 说明滚动操作使窗口产生了新的暴
                    // 露区域, 使新的暴露区域失效; 否则, 说明滚动操作没有
                    // 使窗口产生新的暴露区域, 将更新矩形设置为空矩形
                    if (!RectInRect(&rcOld, &rcNew))
                    {
                        SubtractRect(&rcUpdate, &rcNew, &rcOld);
                        OffsetRect(&rcUpdate, pChild->rcWindow.left, 
                            pChild->rcWindow.top);
                        WND_InvalidateRect(pChild, &rcUpdate, TRUE);
                    }
                }
            }
            else
            {
                // 重新计算窗口的位置
                OffsetRect(&pChild->rcWindow, dx, dy);
                OffsetRect(&pChild->rcClient, dx, dy);
                
                // 由于窗口位置移动, 所以使窗口的暴露区域表失效, 必须在调用
                // WND_InvalidateRect之前调用WND_InvalidateExposeTbl, 否则
                // 调用WND_InvalidateRect可能非正常返回。
                WND_InvalidateExposeTbl(pChild);
                
                // 使整个窗口失效
                /* 
                ** the unmapped child window should not be invalidate the 
                ** update rect.
                */
                if (ISMAPPED(pChild))
                    WND_InvalidateRect(pChild, &pChild->rcWindow, TRUE);
            }
        }

        // 获得窗口树中下一个需要处理的窗口，原则如下：
        //     * 如果有子窗口需要处理，则当前窗口的子窗口为要处理的窗口。
        //     * 没有子窗口需要处理但有下一个兄弟窗口，则当前窗口的下一个兄
        //       弟窗口为要处理的窗口
        //     * 当前窗口没有子窗口和兄弟窗口需要处理，则取当前窗口的祖先窗
        //       口的兄弟窗口为要处理的窗口，如果回溯到起始祖先窗口，表示所
        //       有窗口已经处理完毕。
        if (pChild->pChild && ISCLIENTCHILD(pChild) && !bFromChild)
            pChild = pChild->pChild;
        else
        {
            if (pChild->pNext)
            {
                pChild = pChild->pNext;
                bFromChild = FALSE;
            }
            else
            {
                pChild = pChild->pParent;
                if (pChild == pWin)
                {
                    pChild = NULL;
                    break;
                }

                bFromChild = TRUE;
            }
        }
    }

    return TRUE;
}

/*
**  Function : RectInRect
**  Purpose  :
**      Retrieves whether a specified rectangle is in another specified
**      rectangle. Empty rectangle is always in another rectangle.
*/
static BOOL RectInRect(const RECT* pRect1, const RECT* pRect2)
{
    ASSERT(pRect1 != NULL);
    ASSERT(pRect2 != NULL);

    if (IsRectEmpty(pRect2))
        return TRUE;

    if (IsRectEmpty(pRect1))
        return FALSE;

    if (pRect2->left >= pRect1->left && pRect2->right <= pRect1->right &&
        pRect2->top >= pRect1->top && pRect2->bottom <= pRect1->bottom)
        return TRUE;

    return FALSE;
}

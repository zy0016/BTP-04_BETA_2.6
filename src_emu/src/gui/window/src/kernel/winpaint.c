/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements the functions that support window painting.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"
#include "wsobj.h"
#include "wsownd.h"
#include "wscaret.h"

#include "clstbl.h"

#include "wsthread.h"
#include "msgqueue.h"

/* Global Variable Define */
static WORD     wPaintNum;      // 当有且仅有一个窗口需要重画时, 标识这个
                                // 需要重画的窗口; 否则为NULL
//static PWINOBJ  pPaintWin;      // 当有且仅有一个窗口需要重画时, 标识这个
                                // 需要重画的窗口; 否则为NULL

// Internal function prototype
//static int  GetPaintMessageFromWindow(PWINOBJ pWin, PMSG pMsg);
//static BOOL GetPaintMessageFromWindowTree(PWINOBJ pWin, PMSG pMsg);
static BOOL IsTopWindow(PWINOBJ pWin);

/*
**  Function : WND_GetClientDC
**  Purpose  :
**      Gets DC for window client painting.
*/
PDC WND_GetClientDC(PWINOBJ pWin)
{
    PDC pDC;
    ASSERT(pWin != NULL);

    // 不是窗口私有DC或类私有DC，多次调用，可以得到多个不同DC，但都对应pWin。
    if (!ISOWNDC(pWin) && !ISCLASSDC(pWin))
    {
        pDC = DC_GetWindowDCFromCache(pWin, TRUE, TRUE);
        if (pDC == NULL)
            return NULL;
    // 由于pda860显示模式可能切换，所以将非窗口私有DC设置为默认值的操作由
    // 释放DC时进行改为申请DC时进行，否则DC的颜色值可能出错。
//        DC_SetItemsToDefault(pDC);
    }
    else
    {
        pDC = pWin->pDC;
    }

    pDC->wAttrib &= ~WDA_UPDATE;
    return pDC;
}

/*
**  Function : WND_GetWindowDC
**  Purpose  :
**      Retrieves the window dc of a specified window.
**
**  Remark :
**      Because no function(just like WND_BeginPaint) can be used when 
**      process the WM_NCPAINT message , so this funciton must process
**      the two situation : PA_UPDATEWINDOW & PA_WINDOW.
*/
/* 在释放时，应该考虑到这个DC一定是CacheDC，否则对于OWNERDC，
** 将丢失一个CacheDC
*/
PDC WND_GetWindowDC(PWINOBJ pWin)
{
    PDC pDC;

    ASSERT(pWin != NULL);

    if (pWin->bNCPaint)
        pDC = DC_GetWindowDCFromCache(pWin, FALSE, TRUE);
    else
        pDC = DC_GetWindowDCFromCache(pWin, FALSE, FALSE);

    return pDC;
}

/*
**  Function : WND_ReleaseDC
**  Purpose  :
**      Releases a device context (DC),
*/
BOOL WND_ReleaseDC(PWINOBJ pWin, PDC pDC)
{
    ASSERT(pWin != NULL);
    ASSERT(pDC != NULL);

    // 为保证从Cache中申请的DC的释放，这里不判断pDC与pWin的一致性
    /*
    if (pWin != DC_GetWindow(pDC))
    {
        SetLastError(1);
        return FALSE;
    }
    */

    // DC is from window DC cache, release it to the cache
    if (!ISOWNDC(pWin) && !ISCLASSDC(pWin))
    {
        DC_ReleaseWindowDCToCache(pDC);
    }
    /* BUG 当窗口具有CS_OWNER风格时，调用GetWindowDC，应该获得CacheDC，而释
    ** 放时没有进行判断，因此这里判断不是窗口对象内部DC，就一定是CacheDC
    */
    else if (pDC != pWin->pDC)
    {
        /* 对于OWNERDC，但调用GetWindowDC得到的仍是CacheDC */
        ASSERT(pWin->pDC != NULL);
        DC_ReleaseWindowDCToCache(pDC);
    }

    return TRUE;
}

/*
**  Function : WND_BeginPaint
**  Purpose  :
**      Prepares the specified window for painting and fills a PAINTSTRUCT
**      structure with information about the painting. 
*/
PDC WND_BeginPaint(PWINOBJ pWin, PPAINTSTRUCT pPaintStruct)
{
    RECT rcClient, rcPaint;

    ASSERT(pWin != NULL);

    // Hide the caret if the window owns the caret
    WS_HideCaret(pWin);


    // 不是窗口私有DC, 而且pWin->pDC(表示尚未取得DC)设置为默认值
    // 由于pda860显示模式可能切换，所以将非窗口私有DC设置为默认值的操作由
    // 释放DC时进行改为申请DC时进行，否则DC的颜色值可能出错。
    /* 对于多次调用BeginPaint函数，实际上在内部只进行第一次操作，其他无效*/
    if (!ISOWNDC(pWin) && !ISCLASSDC(pWin) && pWin->pDC == NULL)
    {
        pWin->pDC = DC_GetWindowDCFromCache(pWin, TRUE, TRUE);
        if (pWin->pDC == NULL)
            return NULL;

//        DC_SetItemsToDefault(pWin->pDC);
    }

    pWin->pDC->wAttrib |= WDA_UPDATE;

    // Gets the paint rectangle in client area
    IntersectRect(&rcPaint, &pWin->rcClient, &pWin->rcUpdate);

    // If the window update rectangle isn't equal to the client update 
    // rectangle, indicates the the non client area needs be updated, 
    // sends WM_NCPAINT message
    if (!EqualRect(&rcPaint, &pWin->rcUpdate))
    {
        pWin->bNCPaint = TRUE;  // Set the bNCPaint flag to TRUE
        WND_SendMessage(pWin, WM_NCPAINT, (WPARAM)0, (LPARAM)0);
        pWin->bNCPaint = FALSE; // Set the bNCPaint flag to FALSE        
    }

    if (pWin->bErase)
    {
        // Send WM_ERASEBKND Message to window proc function
        pWin->bErase = (BYTE)WND_SendMessage(pWin, WM_ERASEBKGND, 
            (WPARAM)WOT_GetHandle((PGDIOBJ)pWin->pDC), 0);
    }

    if (pPaintStruct)
    {
        pPaintStruct->hdc = (HDC)WOT_GetHandle((PGDIOBJ)pWin->pDC);
        pPaintStruct->fErase = pWin->bErase;

        CopyRect(&pPaintStruct->rcPaint, &rcPaint);
        WND_GetRect(pWin, &rcClient, W_CLIENT, XY_SCREEN);
        OffsetRect(&pPaintStruct->rcPaint, -rcClient.left, -rcClient.top);
    }

    return pWin->pDC;
}

/*
**  Function : WND_EndPaint
**  Purpose  :
**      Marks the end of painting in the specified window. This function
**      is required for each call to the BeginPaint function, but only 
**      after painting is complete. 
*/
void WND_EndPaint(PWINOBJ pWin, const PAINTSTRUCT* pPaintStruct)
{
    PWINOBJ pChild;
    RECT rcUpdate;
    BOOL bNCPaint;
    ASSERT(pWin != NULL);

    if (pWin->pDC == NULL)
        return;
    
    // 调用BeginPaint函数后调用EndPaint函数, DC应该有WDA_UPDATE属性. 如果DC
    // 没有WDA_UPDATE属性, 可能是由于以下原因:
    //     1. 没有调用BeginPaint函数; 
    //     2. 窗口重画操作形成了嵌套, EndPaint函数已被调用
    //     3. 在调用BeginPaint函数之后, 调用了GetDC和ReleaseDC函数.
    // 为使窗口系统能够在这些情况下运行, 这里不做DC有WDA_UPDATE属性的判断

    // 应用程序可能在响应WM_PAINT消息时直接或间接调用WND_InvalidateRect
    // (调用InvalidateRect, SetScrollInfo, MoveWindow等)再次使bUpdate标
    // 志置位, 此时有新的更新区域产生. 因此 bUpdate被置位时不能清空窗口
    // 的更新区域, 以便使窗口产生下一次窗口重画消息. 
    
    // 注: WM_NCPAINT消息的响应有相同的问题, 考虑到应用程序一般不响应
    // WM_NCPAINT消息, 处理完WM_NCPAINT消息后不考虑这种情况.
    
    /* PAINT_DRAWING 表明在绘制过程中，没有得到新的更新消息，可以设为
    ** PAINT_NO状态。
    ** 如果不是PAINT_DRAWING 状态，则必然又得到了新的更新消息，故其子窗口的
    ** 消息延迟到以后一齐实施。因为在绘制过程中，更新矩形并没有清空，在产生
    ** 新的更新矩形时，必然包含原有矩形，故对于子窗口不会丢掉更新区域。只不
    ** 过这次绘制浪费掉了。
    */
    if (pWin->pParent != NULL && 
        pWin->pParent->flagUpdate != PAINT_NO && 
        !ISCLIPCHILDREN(pWin->pParent) && 
        IntersectRect(&rcUpdate, &pWin->rcUpdate, &pWin->pParent->rcUpdate))
    {
    }
    else if (pWin->flagUpdate == PAINT_DRAWING)
    {
        pWin->flagUpdate = PAINT_NO;
        SetRectEmpty(&pWin->rcUpdate);
        pWin->bErase = FALSE;

        /* 不论是否裁减子窗口，都要查子窗口的标志，因为即使这一级的窗口
        ** 不裁减子窗口，但更上级的可能仍然裁减。而这在InvalidateRect中已经
        ** 做了判断。故这里都要进行标志判断。
        */
        pChild = pWin->pChild;
        while (pChild != NULL)
        {
            if (ISMAPPED(pChild) && pChild->flagUpdate == PAINT_WAIT)
            {
                wPaintNum ++;
                pChild->flagUpdate = PAINT_QUEUE;
                /* 子窗口需要判断重绘区域是否NC区 */
                bNCPaint = !IntersectRect(&rcUpdate, &pChild->rcUpdate, &pChild->rcClient);
                MSQ_InsertPaintMsg(pChild, bNCPaint);
            }

            pChild = pChild->pNext;
        }
    }
    
    // Show the caret if the window owns the caret
    WS_ShowCaret(pWin);

    // 不是窗口私有DC, 释放到cache
    if (!ISOWNDC(pWin) && !ISCLASSDC(pWin))
    {
        DC_ReleaseWindowDCToCache(pWin->pDC);
        pWin->pDC = NULL;
    }
    else
        pWin->pDC->wAttrib &= ~WDA_UPDATE;
}

/*
**  Function : WND_InvalidateTree
**  Purpose  :
**      Adds a rectangle to the specified window’s update rectangle, and
**      if the specified window hasn't WS_CLIPCHILDREN style, invalidates
**      the window's child windows whose window rectangle is intersected
**      with the specified rectangle.
*/
void WND_InvalidateTree(PWINOBJ pWin, const RECT* pRect, BOOL bErase, 
                        BOOL bInvalidateAll)
{
    PWINOBJ pChild;
    RECT rect;
    BOOL bFromChild;

    ASSERT(pWin != NULL);
    ASSERT(pRect != NULL);

    // 窗口没有映射或被完全覆盖, 直接返回。(注：在窗口新生成时，窗口的暴露
    // 区表一般还没有计算，此时，ISOVERLAPPED返回FALSE)
    if (!ISMAPPED(pWin))// || ISOVERLAPPED(pWin))
        return;

    // First invalidates the specified window
    WND_InvalidateRect(pWin, pRect, bErase);

    // If the specified window hasn't WS_CLIPCHILDREN style, invalidates
    // the window's child windows whose window rectangle is intersected
    // with the specified rectangle.
    if (!ISCLIPCHILDREN(pWin) || bInvalidateAll)
    {

/* */
    /* 在处理窗口树的过程中，遍历方法是：
    ** 使用前序遍历方式调用WND_InvalidateRect
    */

        pChild = pWin->pChild;
        bFromChild = FALSE;
        while (pChild)
        {
            // 注意: 这里不能使用pRect来判断子窗口是否需要更新, 因为pRect
            // 并不是当前窗口最终的失效区. 由于失效区的合并操作, 窗口的失
            // 效区可能会大于指定的失效区. 因此应该使用窗口结构中的失效区
            // 判断子窗口是否需要失效
            /* 
            ** 对于需要失效所有窗口的情况，不能只靠子窗口的失效矩形判断，
            ** 而是要根据是否有暴露区来判断
            */
            if (ISMAPPED(pChild) && !bFromChild)
            {
                if (bInvalidateAll)
                {
                    if (IntersectRect(&rect, &pChild->rcWindow, pRect))
                        WND_InvalidateRect(pChild, &rect, TRUE);
                }
                else
                {
                    if (IntersectRect(&rect, &pChild->rcWindow, &pWin->rcUpdate))
                        WND_InvalidateRect(pChild, &rect, TRUE);

                }
            }
/*
            if (ISMAPPED(pChild) && !bFromChild &&
                IntersectRect(&rect, &pChild->rcWindow, &pWin->rcUpdate))
            {
                WND_InvalidateRect(pChild, &rect, TRUE);
            }
*/
            // 获得窗口树中下一个需要处理的窗口，原则如下：
            //     * 如果有子窗口需要处理，则当前窗口的子窗口为要处理的窗口。
            //     * 没有子窗口需要处理但有下一个兄弟窗口，则当前窗口的下一个兄
            //       弟窗口为要处理的窗口
            //     * 当前窗口没有子窗口和兄弟窗口需要处理，则取当前窗口的祖先窗
            //       口的兄弟窗口为要处理的窗口，如果回溯到起始祖先窗口，表示所
            //       有窗口已经处理完毕。
            /* 
            ** 1、应该对没有CLIPCHILDREN属性的窗口才遍历
            ** 2、对于需要失效所有窗口的情况，则都要遍历
            */
/*
            if (pChild->pChild && ISMAPPED(pChild) && 
                ISCLIPCHILDREN(pChild) && !bFromChild)
            {
                pChild = pChild->pChild;
            }
*/
            if (pChild->pChild && ISMAPPED(pChild) && !bFromChild &&
                (!ISCLIPCHILDREN(pChild) || bInvalidateAll))
            {
                pChild = pChild->pChild;
            }
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
}

/*
**  Function : WND_InvalidateRect
**  Purpose  :
**      Adds a rectangle to the specified window's update rectangle.
*/
void WND_InvalidateRect(PWINOBJ pWin, const RECT* pRect, BOOL bErase)
{
    RECT rcUpdate;
    RECT rcClientUpdate;
    BOOL bNCPaint;

    ASSERT(pWin != NULL);
    ASSERT(ISMAPPED(pWin));

    // 窗口被完全覆盖, 直接返回
//    if (ISOVERLAPPED(pWin))
//        return;

    if (pRect)
        IntersectRect(&rcUpdate, pRect, &pWin->rcWindow);
    else
        CopyRect(&rcUpdate, &pWin->rcWindow);

    // If the client area has no update area, indicates that only the 
    // nonclient area need be updated, Insert WM_NCPAINT message to the
    // window directly.
    bNCPaint = !IntersectRect(&rcClientUpdate, &rcUpdate, &pWin->rcClient);

    if (!IsRectEmpty(&rcUpdate))
    {
        /* 
        ** wPaintNum应该表示当前paint队列中的消息个数 
        */
/*
        if (pWin->flagUpdate == PAINT_NO)
            wPaintNum ++;
*/
        if (ISROOTWIN(pWin))
        {
            ASSERT(pWin->flagUpdate != PAINT_WAIT);
            if (pWin->flagUpdate == PAINT_NO)
            {
                wPaintNum ++;
                pWin->flagUpdate = PAINT_QUEUE;
                MSQ_InsertPaintMsg(pWin, bNCPaint);
            }
        }
        else if (ISCLIPCHILDREN(pWin->pParent) && 
            pWin->pParent->flagUpdate != PAINT_WAIT)
        {
            /* 父窗口裁减子窗口，子窗口可以直接重画。
            ** 但这需要一个条件就是父窗口本身不出在等待状态，因为如果处在
            ** 等待状态，则该父窗口的祖先窗口必然有不裁减子窗口的情况，这里
            ** 也必须保证绘制顺序。
            ** 这样在EndPaint中就必须对每个窗口的所有子窗口都进行判断是否在
            ** 等待
            */
            if (pWin->flagUpdate != PAINT_QUEUE)
            {
                wPaintNum ++;
                pWin->flagUpdate = PAINT_QUEUE;
                MSQ_InsertPaintMsg(pWin, bNCPaint);
            }
        }
        else
        {
            /* 父窗口不裁减子窗口，需要保证父窗口绘完后，再绘制子窗口 */
            if (pWin->pParent->flagUpdate == PAINT_NO)
            {
                /* 或者父窗口不需要重画，子窗口可以直接重画 */
                if (pWin->flagUpdate != PAINT_QUEUE)
                {
                    wPaintNum ++;
                    pWin->flagUpdate = PAINT_QUEUE;
                    MSQ_InsertPaintMsg(pWin, bNCPaint);
                }
            }
            else if (pWin->flagUpdate == PAINT_QUEUE)
            {
                /* 可能其父窗口又一次得到了重绘消息，这时该子窗口的上次重绘
                ** 还没有进行，直接删掉队列中上次的消息，合并无效区域后，等
                ** 其父窗口再一次绘制完后，再得到WM_PAINT消息
                */
                ASSERT(wPaintNum != 0);
                wPaintNum --;
                MSQ_DeletePaintMsg(pWin);
                pWin->flagUpdate = PAINT_WAIT;
            }
            else if (pWin->flagUpdate == PAINT_DRAWING)
            {
                /* 子窗口正在处于重画的处理中，这时，父窗口又收到了重绘命令，
                ** 此时只能将子窗口的无效区域进行合并，等待父窗口绘完后再进行
                ** 重绘。实际上，已经重绘的部分就多余了，但能保证正确性。
                ** 设置标志为PAINT_WAIT。该子窗口绘完后，如果发现其标志不是
                ** PAINT_DRAWING，那么必然是需要再次重绘，就不设为PAINT_NO了。
                */
                pWin->flagUpdate = PAINT_WAIT;
            }
            else
            {
                /* 子窗口处于等待状态或者无需绘制状态，可以直接合并重绘区域*/
                pWin->flagUpdate = PAINT_WAIT;
            }
        }

        if (IsRectEmpty(&pWin->rcUpdate))
            CopyRect(&pWin->rcUpdate, &rcUpdate);
        else
        {
            // 窗口原来的失效区域不空, 将新的失效区域和旧的失效区域合并
            UnionRect(&pWin->rcUpdate, &pWin->rcUpdate, &rcUpdate);

            // 矩形失效区域的合并可能会扩大窗口的失效区域, 使本来没有失效
            // 的区域失效. 如果窗口没有WS_CLIPCHILDREN风格,必须位于增加的
            // 失效区域的子窗口失效, 因为窗口的重画可能会覆盖子窗口, 这种
            // 处理方法可能会引起窗口的多余重画, 但是能保证重画的正确性.

            if (!ISCLIPCHILDREN(pWin))
            {
                PWINOBJ pChild;
                RECT rect;

                pChild = pWin->pChild;
                while (pChild)
                {
                    if (IntersectRect(&rect, &pChild->rcWindow, 
                        &pWin->rcUpdate))
                        WND_InvalidateTree(pChild, &rect, TRUE, FALSE);
                    
                    pChild = pChild->pNext;
                }
            }

        }

        pWin->bErase = (BYTE)(pWin->bErase || bErase);
    }
}

/*
**  Function : WND_ValidateRect
**  Purpsoe  :
**      Validates the client area within a rectangle by removing the 
**      rectangle from the update region of the specified window.
*/
void WND_ValidateRect(PWINOBJ pWin, const RECT* pRect)
{
    ASSERT(pWin != NULL);
    ASSERT(!IsRectEmpty(pRect));

    if (!ISMAPPED(pWin))
        return;

    // If the update rectangle is empty, just return
    if (IsRectEmpty(&pWin->rcUpdate))
        return;

    // Subtracts the specified rectangle from window update rectangle
    if (pRect)
        SubtractRect(&pWin->rcUpdate, &pWin->rcUpdate, pRect);
    else
        SetRectEmpty(&pWin->rcUpdate);

    // 如果当前窗口失效区域为空, 清除窗口需要更新标志, 减少重画窗口计数
    if (pWin->flagUpdate != PAINT_NO && IsRectEmpty(&pWin->rcUpdate))
    {
        if (pWin->flagUpdate == PAINT_QUEUE)
        {
            ASSERT(wPaintNum != 0);
            wPaintNum --;
            MSQ_DeletePaintMsg(pWin);
        }
        pWin->flagUpdate = PAINT_NO;
    }
}

/*
**  Function : WND_UpdateTree
**  Purpose  :
**      Updates the specified window and its child windows, send WM_PAINT 
**      message if rcUpdate of window isn't empty.
*/
void WND_UpdateTree(PWINOBJ pWin)
{
    PWINOBJ pChild;
    BOOL bFromChild;

    /* 
    ** 调用UpdateWindow时，如果是子窗口，且父窗口需要重绘，那么
    ** 直接先Update父窗口。
    ** 这里认为不可能存在父窗口是PAINT_NO,而祖先窗口是非PAINT_NO。
    */
    while (pWin && ISCHILD(pWin) && !ISCLIPCHILDREN(pWin->pParent) 
        && pWin->pParent->flagUpdate != PAINT_NO)
    {
        pWin = pWin->pParent;
    }

    // The unmapped window needn't update
    if (!ISMAPPED(pWin))
        return;

    // Updates the specified window
    WND_Update(pWin);
    
    // No more window need to be updated, return
    if (wPaintNum == 0)
        return;
/**/
    /* 在处理窗口树的过程中，遍历方法是：
    ** 使用前序遍历方式调用WND_Update
    */

    // Now updates all the children of the specified window
    pChild = pWin->pChild;
    bFromChild = FALSE;
    while (pChild)
    {
        if (ISMAPPED(pChild) && !bFromChild)
        {
            // Updates the current window
            WND_Update(pChild);

            // No more window need to be updated, return
            if (wPaintNum == 0)
                return;
        }
            
        // 获得窗口树中下一个需要处理的窗口，原则如下：
        //     * 如果有子窗口需要处理，则当前窗口的子窗口为要处理的窗口。
        //     * 没有子窗口需要处理但有下一个兄弟窗口，则当前窗口的下一个兄
        //       弟窗口为要处理的窗口
        //     * 当前窗口没有子窗口和兄弟窗口需要处理，则取当前窗口的祖先窗
        //       口的兄弟窗口为要处理的窗口，如果回溯到起始祖先窗口，表示所
        //       有窗口已经处理完毕。
        if (pChild->pChild && ISMAPPED(pChild) && !bFromChild)
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
**  Function : WND_Update
**  Purpose  :
**      Updates the specified window, send WM_PAINT message if rcUpdate 
**      isn't empty.
*/
void WND_Update(PWINOBJ pWin)
{
    RECT rect;

    ASSERT(pWin != NULL);

    // If the specified window is unmapped, needn't be updated
    if (!ISMAPPED(pWin))
        return;
    if (pWin->flagUpdate == PAINT_NO)
        return;

    /* 对于Update调用，直接Send消息，绘制顺序由UpdateTree保证，应该清除
    ** 所有的相关PAINT队列消息。这个操作放到EndPaint中进行
    */

    // 用户在WM_PAINT消息之外调用BeginPaint + EndPaint会造成这种情况, 
    // DEBUG环境下给出警告, 否则直接返回.
    ASSERT(!IsRectEmpty(&pWin->rcUpdate));
    if (IsRectEmpty(&pWin->rcUpdate))
        return;

    // If the client area has update area, sends WM_PAINT message to 
    // the window, the WM_NCPAINT message will be sent when BeginPaint
    // function is called. If the client area has no update area, 
    // indicates that only the nonclient area need to be updated, send
    // WM_NCPAINT message to the window.
    if (IntersectRect(&rect, &pWin->rcUpdate, &pWin->rcClient))
    {
        /* 
        ** 对于NCPAINT消息不删除，留在队列中执行 
        */
        if (pWin->flagUpdate == PAINT_QUEUE)
        {
            ASSERT(wPaintNum != 0);
            wPaintNum --;
            MSQ_DeletePaintMsg(pWin);
        }

        pWin->flagUpdate = PAINT_DRAWING;
        WND_SendNotifyMessage(pWin, WM_PAINT, (WPARAM)0, (LPARAM)0);
    }
    else
    {
        /* 
        ** The flag should be updated.
        */
//        pWin->flagUpdate = PAINT_NO;

        /* 对非客户区不使用UpdateWindow*/
/*
        pWin->bNCPaint = TRUE;  // Set the bNCPaint flag to TRUE
        WND_SendNotifyMessage(pWin, WM_NCPAINT, (WPARAM)0, (LPARAM)0);
        pWin->bNCPaint = FALSE; // Set the bNCPaint flag to FALSE
        
        // Validates the window update area because the window has been 
        // updated already.
        SetRectEmpty(&pWin->rcUpdate);
        pWin->bErase = FALSE;
*/
    }
}

/*
**  Function : WND_GetPaintRect
**  Purpose  :
**      Retrieves the paint rectangle which is specified by paint area 
**      code. 
*/
void WND_GetPaintRect(PWINOBJ pWin, int nPaintArea, PRECT pRect)
{
    ASSERT(pWin != NULL);
    ASSERT(pRect != NULL);

    switch (nPaintArea)
    {
    case PA_WINDOW :

        CopyRect(pRect, &(pWin->rcWindow));
        break;

    case PA_UPDATEWINDOW :

        IntersectRect(pRect, &(pWin->rcWindow), &(pWin->rcUpdate));
        break;

    case PA_CLIENT :

        CopyRect(pRect, &(pWin->rcClient));
        break;

    default :

        IntersectRect(pRect, &(pWin->rcClient), &(pWin->rcUpdate));
        break;
    }
}

/*
**  Function : WND_GetPaintOrg
**  Purpose  :
**      Retrieves the screen coordinate of the paint origin points.
*/
void WND_GetPaintOrg(PWINOBJ pWin, int nPaintArea, PPOINT pptOrg)
{
    if (nPaintArea == PA_WINDOW || nPaintArea == PA_UPDATEWINDOW)
    {
        pptOrg->x = pWin->rcWindow.left;
        pptOrg->y = pWin->rcWindow.top;
    }
    else
    {
        pptOrg->x = pWin->rcClient.left;
        pptOrg->y = pWin->rcClient.top;
    }
}

/*
**  Function : WND_DestroyForPaint
**  Purpose  :
**      The function will be called when the window is being hiding or 
**      destroying.
*/
void WND_DestroyForPaint(PWINOBJ pWin)
{
    ASSERT(pWin != NULL);

    // 即将被删除或隐藏的窗口更新标志被置位, 清除更新标志, 并减少更新窗口
    // 计数. 否则会使更新窗口计数不正常.
    if (pWin->flagUpdate != PAINT_NO)
    {
        if (pWin->flagUpdate == PAINT_QUEUE)
        {
            // 减少重画窗口计数
            ASSERT(wPaintNum != 0);
            wPaintNum --;
            MSQ_DeletePaintMsg(pWin);
        }

        pWin->flagUpdate = PAINT_NO;
    }
}

/*
**  Function : WND_GetPaintMessage
**  Purpose  :
**      
*/
BOOL WND_GetPaintMessage(PMSG pMsg)
{
    PPAINTMSG pPaintMsg;
    PWINOBJ pWin;
    PTHREADINFO pThreadInfo;

    ASSERT(pMsg != NULL);

    pThreadInfo = WS_GetCurrentThreadInfo();
    /* 窗口销毁后，Paint消息队列中可能还有PAINT消息，需要得到有效的消息 */
    while(TRUE)
    {
        pPaintMsg = MSQ_GetPaintMsg(pThreadInfo, TRUE);
        if (pPaintMsg == NULL)
            return FALSE;

        *pMsg = pPaintMsg->msg;

        ReleaseMsgObj(pPaintMsg, MSGOBJ_PAINT);

        ASSERT(wPaintNum != 0);
        wPaintNum --;

        pWin = (PWINOBJ)WOT_GetObj((HANDLE)pMsg->hwnd, OBJ_WINDOW);
        if (pWin == NULL)
        {
            continue;
        }
        if (pWin->flagUpdate != PAINT_QUEUE)
        {
            TRACE("pWin->flagUpdate = %d\n", pWin->flagUpdate);
            ASSERT(0);
            continue;
        }
        
        if (pMsg->message == WM_NCPAINT)
        {
            RECT rcUpdate;
            /* 父窗口需要重绘,丢弃这个消息 */
            if (pWin->pParent != NULL && 
                pWin->pParent->flagUpdate != PAINT_NO && 
                !ISCLIPCHILDREN(pWin->pParent) && 
                IntersectRect(&rcUpdate, &pWin->rcUpdate, &pWin->pParent->rcUpdate))
            {
                /* 如果有这种情况，这个消息应该提前被删除了，不应该还存在，
                ** 这里调试给出ASSERT。
                */
                ASSERT(0);
                continue;
            }
            // If the client area has no update area, indicates that only the 
            // nonclient area need be updated, send WM_NCPAINT message to the
            // window directly.        
            pWin->flagUpdate = PAINT_DRAWING;
            pWin->bNCPaint = TRUE;  // Set the bNCPaint flag to TRUE
            WND_SendMessage(pWin, WM_NCPAINT, (WPARAM)0, (LPARAM)0);
            pWin->bNCPaint = FALSE; // Set the bNCPaint flag to FALSE        
            
            if (pWin->flagUpdate == PAINT_DRAWING)
            {
                /* 在处理NCPAINT中没有产生新的更新区，这样状态为PAINT_DRAWING */
                IntersectRect(&pWin->rcUpdate, &pWin->rcUpdate, &pWin->rcClient);
                if (IsRectEmpty(&pWin->rcUpdate))
                {
                    /* 修改更新区为客户区更新区 */
                    /* 如果客户区更新区为空，则设置 */
                    pWin->flagUpdate = PAINT_NO;
                    pWin->bErase = FALSE;
                }
                else
                {
                    /* 在插入NCPAINT消息后可能还会合并更新区，使得更新区不再仅限于
                    ** NC区，因此还要产生PAINT消息
                    */
                    pMsg->message = WM_PAINT;
                    break;
                }
            }

            continue;
        }

        break;
    }

    ASSERT(pWin != NULL);
    ASSERT(pWin->dwThreadId == pThreadInfo->dwThreadId);
    pWin->flagUpdate = PAINT_DRAWING;

    return TRUE;
}

// Internal functions 

/*
**  Function : WND_ScrollWindow
**  Purpose  :
**      Scrolls the content of the specified window’s client area. 
**  Params   :
**      pWin      : Indentifies the window where the client area is 
**                  scrolled.
**      dx        : Specifies the amount, in device units, of horizontal 
**                  scrolling. If the window being scrolled has the 
**                  CS_OWNDC or CS_CLASSDC style, then this parameter uses
**                  logical units rather than device units. This parameter
**                  must be a negative value to scroll the content of the 
**                  window to the left. 
**      dy        : Specifies the amount, in device units, of vertical 
**                  scrolling. If the window being scrolled has the 
**                  CS_OWNDC or CS_CLASSDC style, then this parameter uses
**                  logical units rather than device units. This parameter
**                  must be a negative value to scroll the content of the 
**                  window up. 
**      prcScroll : Points to the RECT structure specifying the portion of
**                  the client area to be scrolled. If this parameter is 
**                  NULL, the entire client area is scrolled. 
**      prcClip   : Points to the RECT structure containing the coordinates
**                  of the clipping rectangle. Only device bits within the
**                  clipping rectangle are affected. Bits scrolled from the
**                  outside of the rectangle to the inside are painted; 
**                  bits scrolled from the inside of the rectangle to the
**                  outside are not painted. 
**  Return   : 
**      If the function succeeds, return nonzero.
**      If the function fails, return zero. To get extended infomation, 
**      call GetLastError.
**  Remarks  :
**      If the caret is in the window being scrolled, ScrollWindow 
**      automatically hides the caret to prevent it from being erased and
**      then restores the caret after the scrolling is finished. The caret 
**      position is adjusted accordingly. 
**      The area uncovered by ScrollWindow is not repainted, but it is 
**      combined into the window’s update region. The application 
**      eventually receives a WM_PAINT message notifying it that the region
**      must be repainted. To repaint the uncovered area at the same time 
**      the scrolling is in action, call the UpdateWindow function 
**      immediately after calling ScrollWindow. 
**      If the prcScroll parameter is NULL, the positions of any child 
**      windows in the window are offset by the amount specified by the 
**      dx and dy parameters; invalid (unpainted) areas in the window are 
**      also offset. ScrollWindow is faster when prcScroll is NULL. 
**      If prcScroll is not NULL, the positions of child windows are not 
**      changed and invalid areas in the window are not offset. To prevent
**      updating problems when prcScroll is not NULL, call UpdateWindow to
**      repaint the window before calling ScrollWindow. 
*/
void WND_ScrollWindow(PWINOBJ pWin, int dx, int dy, const RECT* prcScroll,
                      const RECT* prcClip)
{
    RECT rcScroll, rcClip;
    RECT rcUpdate, rcClient, rcNCUpdate = {0, 0, 0, 0};
    PWINOBJ pCaretWin = NULL;
    BOOL bTop;
    PGRAPHDEV pGraphDev;
/*
    RECT rcSrc, rcDest, rcPartClip;
    PEXPOSETBL pExposeTbl;
    int i, j;
*/

    ASSERT(pWin != NULL);
    ASSERT(dx != 0 || dy != 0);

    // 不处理隐藏窗口德滚动
    if (!ISMAPPED(pWin))
        return;

    // 如果窗口的当前失效区域不为空, 滚动窗口的当前失效区域
    if (!IsRectEmpty(&pWin->rcUpdate))
    {
        /* 
        ** 这个改动在旧窗口系统中已经验证了。现在在新窗口系统中修改。
        ** 直接的SendMessage将NCPAINT消息执行后，相当于执行了
        ** UpdateWindow。而父窗口还没有绘制，必然这次绘制无效了。而
        ** 在父窗口绘完后，这个窗口的更新区却没有了非客户区部分，导致
        ** 非客户区没有更新。
        */
        // 如果窗口的更新区域包含窗口的非客户区部分, 保存非客户区
        SubtractRect(&rcNCUpdate, &pWin->rcUpdate, &pWin->rcClient);
        if (!IsRectEmpty(&rcNCUpdate))
        {
            OffsetRect(&pWin->rcUpdate, dx, dy);
            IntersectRect(&pWin->rcUpdate, &pWin->rcUpdate, 
                &pWin->rcClient);
        }

/*
        // 如果窗口的更新区域包含窗口的非客户区部分, 发送WM_NCPAINT消息使
        // 窗口函数重画窗口的非客户区
        if (pWin->rcUpdate.left < pWin->rcClient.left ||
            pWin->rcUpdate.right > pWin->rcClient.right ||
            pWin->rcUpdate.top < pWin->rcClient.top ||
            pWin->rcUpdate.bottom > pWin->rcClient.bottom)
        {
            pWin->bNCPaint = TRUE;  // Set the bNCPaint flag to TRUE
            WND_SendMessage(pWin, WM_NCPAINT, (WPARAM)0, (LPARAM)0);
            pWin->bNCPaint = FALSE; // Set the bNCPaint flag to FALSE        
            
            // 窗口的非客户区已经重画, 从窗口的失效区域中去掉非客户区部分
            IntersectRect(&pWin->rcUpdate, &pWin->rcUpdate, 
                &pWin->rcClient);
        }

        // 此时如果窗口的失效区域不为空, 滚动窗口的失效区域
        if (!IsRectEmpty(&pWin->rcUpdate))
        {
            OffsetRect(&pWin->rcUpdate, dx, dy);
            IntersectRect(&pWin->rcUpdate, &pWin->rcUpdate, 
                &pWin->rcClient);
        }
*/
    }

    // 计算需要滚动的矩形区域(屏幕坐标)
    if (prcScroll)
    {
        // 将滚动区域矩形的坐标由窗口客户区坐标转换为屏幕坐标
        rcScroll = *prcScroll;
        WND_GetRect(pWin, &rcClient, W_CLIENT, XY_SCREEN);
        OffsetRect(&rcScroll, rcClient.left, rcClient.top);

        // 滚动区域应在窗口的客户区内
        IntersectRect(&rcScroll, &rcScroll, &pWin->rcClient);
    }
    else    // 指定滚动区域的矩形指针为NULL, 则滚动区域为整个客户区
        rcScroll = pWin->rcClient;

    // 计算滚动的裁剪矩形(屏幕坐标)
    if (prcClip)
    {
        // 将滚动裁剪矩形的坐标由窗口客户区坐标转换为屏幕坐标
        rcClip = *prcClip;
        WND_GetRect(pWin, &rcClient, W_CLIENT, XY_SCREEN);
        OffsetRect(&rcClip, rcClient.left, rcClient.top);

        // 裁剪区域应在窗口的客户区内
        IntersectRect(&rcClip, &rcClip, &pWin->rcClient);
    }
    else    // 指定滚动区域的矩形指针为NULL, 则滚动区域为整个客户区
        rcClip = pWin->rcClient;

    // prcScroll参数为NULL, 移动所有的后代窗口到相应的新位置并使需要重画的
    // 后代窗口的相应区域失效;同时, 如果窗口拥有Caret, 移动Caret到相应的位
    // 置. 如果prcScroll参数不为NULL, 不移动窗口的后代窗口, 直接调用显示设
    // 备驱动程序屏幕滚动函数滚动相应的屏幕区域.
    if (!prcScroll)
    {
        pCaretWin = WS_GetCaretWindow();

        // 如果窗口或窗口的后代窗口拥有caret, 隐藏caret以免影响下面的滚动
        // 操作
        if (WND_IsDescendant(pCaretWin, pWin))
            WS_HideCaret(pCaretWin);
    }

    bTop = IsTopWindow(pWin);

    // 直接调用显示设备驱动程序屏幕滚动函数滚动相应的屏幕区域
    if (bTop)
    {
        RECT rcTemp;
        /* 对于滚动的目的区域如果与NC更新区相交，那么不执行滚屏，而是合并
        ** 到更新区域中
        */
        IntersectRect(&rcTemp, &rcScroll, &rcClip);
        OffsetRect(&rcTemp, dx, dy);

        if (IntersectRect(NULL, &pWin->rcUpdate, &rcTemp))
        {
            /* 如果滚动的区域有部分需要重画，那么这里不需要进行滚动操作，
            ** 只要添加更新区域
            */
            UnionRect(&rcNCUpdate, &rcNCUpdate, &rcTemp);
        }
        else if (IntersectRect(NULL, &rcNCUpdate, &rcTemp))
        {
            /* 如果滚动的区域与需要重画的NC区域相交，那么为了保证NC区绘制
            ** 与客户区的绘制同时发生，这里不进行滚动操作，只将滚动区域添
            ** 加到更新区域中
            */
            UnionRect(&rcNCUpdate, &rcNCUpdate, &rcTemp);
        }
        else
        {
            pGraphDev = GRAPHDEV_GetDefault(DEV_DISPLAY);
            ASSERT(pGraphDev != NULL);
            pGraphDev->drv.ScrollScreen(pGraphDev->pDev, &rcScroll, &rcClip, 
                rcScroll.left + dx, rcScroll.top + dy);
        }

    }

    if (!prcScroll)
    {
        // 如果窗口拥有caret, 相应改变caret的位置已完成caret的滚动
        if (pCaretWin == pWin)
            WS_ScrollCaret(pWin, dx, dy);
        
        // 如果窗口有子窗口, 滚动子窗口
        if (pWin->pChild)
            WND_ScrollChild(pWin, dx, dy, &rcClip, bTop);

        // 如果窗口拥有caret, 重新显示caret
        if (WND_IsDescendant(pCaretWin, pWin))
            WS_ShowCaret(pCaretWin);
    }

    // 滚动区域和裁剪区域相交时窗口可能需要更新, 计算窗口的失效区
    if (IntersectRect(&rcUpdate, &rcClip, &rcScroll))
    {
        if (bTop)
        {
            // 计算窗口需要更新的矩形区域
            if (dx == 0)        // 垂直滚动
            {
                if (dy > 0)     // 向下滚动
                {
                    if (dy < rcUpdate.bottom - rcUpdate.top)
                        rcUpdate.bottom = rcUpdate.top + dy;
                }
                else            // 向上滚动
                {
                    dy = -dy;
                    if (dy < rcUpdate.bottom - rcUpdate.top)
                        rcUpdate.top = rcUpdate.bottom - dy;
                }
            }                          
            else if (dy == 0)   // 水平滚动
            {
                if (dx > 0)     // 向右滚动
                {
                    if (dx < rcUpdate.right - rcUpdate.left)
                        rcUpdate.right = rcUpdate.left + dx;
                }
                else            // 向左滚动
                {
                    dx = -dx;
                    if (dx < rcUpdate.right - rcUpdate.left)
                        rcUpdate.left = rcUpdate.right - dx;
                }
            }
        }

        UnionRect(&rcUpdate, &rcNCUpdate, &rcUpdate);
        // 使窗口需要更新的区域失效
        WND_InvalidateRect(pWin, &rcUpdate, TRUE);
    }

/*
    // 对于每一个暴露块调用设备驱动程序屏幕滚动函数滚动相应的屏幕区域

    // 计算窗口的暴露区表, 不考虑对子窗口的裁剪
    if (ISCLIPCHILDREN(pWin) && pWin->pChild && !ISNCCHILD(pWin->pChild))
    {
        pWin->dwStyle &= ~WS_CLIPCHILDREN;
        pExposeTbl = WND_MakeExposeTbl(pWin);
        pWin->dwStyle |= WS_CLIPCHILDREN;
    }
    else
        pExposeTbl = WND_GetExposeTbl(pWin);

    // 依次滚动每个暴露区域
    for (i = 0; i < pExposeTbl->count; i++)
    {
        // 获得一个滚动区域
        if (!IntersectRect(&rcSrc, &rcScroll, &pExposeTbl->rect[i]))
            continue;

        // 计算该滚动区域滚动后的位置
        rcDest = rcSrc;
        OffsetRect(&rcDest, dx, dy);
        IntersectRect(&rcDest, &rcDest, &rcClip);

        for (j = 0; j < pExposeTbl->count; j++)
        {
            // 滚动后的区域与每个窗口暴露区的交为滚动的裁剪区域
            if (IntersectRect(&rcPartClip, &rcDest, &pExposeTbl->rect[j]))
            {
                // 滚动的目的区域不需要更新, 使滚动的目的区域有效
                // WND_ValidateRect(pWin, &rcPartClip);

                g_DisplayDrv.ScrollScreen(g_pDisplayDev, &rcSrc, 
                    &rcPartClip, rcSrc.left + dx, rcSrc.top + dy);
            }
        }

        // 滚动区域和裁剪区域相交时窗口可能需要更新, 计算窗口的失效区
        if (IntersectRect(&rcUpdate, &rcClip, &rcSrc))
        {
            // 计算窗口需要更新的矩形区域
            if (dx == 0)        // 垂直滚动
            {
                if (dy > 0)     // 向下滚动
                {
                    if (dy < rcUpdate.bottom - rcUpdate.top)
                        rcUpdate.bottom = rcUpdate.top + dy;
                }
                else            // 向上滚动
                {
                    dy = -dy;
                    if (dy < rcUpdate.bottom - rcUpdate.top)
                        rcUpdate.top = rcUpdate.bottom - dy;
                }
            }                          
            else if (dy == 0)   // 水平滚动
            {
                if (dx > 0)     // 向右滚动
                {
                    if (dx < rcUpdate.right - rcUpdate.left)
                        rcUpdate.right = rcUpdate.left + dx;
                }
                else            // 向左滚动
                {
                    dx = -dx;
                    if (dx < rcUpdate.right - rcUpdate.left)
                        rcUpdate.left = rcUpdate.right - dx;
                }
            }
            
            // 使窗口需要更新的区域失效
            WND_InvalidateRect(pWin, &rcUpdate, TRUE);
        }
    }

    if (ISCLIPCHILDREN(pWin) && pWin->pChild && !ISNCCHILD(pWin->pChild))
        MemFree(pExposeTbl);

    if (!prcScroll)
    {
        // 如果窗口拥有caret, 相应改变caret的位置已完成caret的滚动
        if (pCaretWin == pWin)
            WS_ScrollCaret(pWin, dx, dy);
        
        // 如果窗口有客户区子窗口, 滚动所有客户区子窗口
        if (pWin->pChild && !ISNCCHILD(pWin->pChild))
        {
            WND_ScrollChild(pWin, dx, dy, &rcClip);

            // 如果窗口暴露区考虑子窗口的裁剪, 使窗口的暴露区表失效
            if (ISCLIPCHILDREN(pWin))
                WND_InvalidateExposeTbl(pWin);
        }

        // 如果窗口拥有caret, 重新显示caret
        if (WND_IsDescendant(pCaretWin, pWin))
            WS_ShowCaret(pCaretWin);
    }
*/
}

/*
**  Function : IsTopWindow
**  Purpose  :
**      返回一个窗口是否是顶层窗口(没有除子窗口之外的其它窗口覆盖窗口的
**      客户区)
*/
static BOOL IsTopWindow(PWINOBJ pWin)
{
    PWINOBJ pParent, pBrother;

    ASSERT(pWin != NULL);

    // 判断是否有兄弟窗口或祖先窗口的兄弟窗口覆盖窗口的客户区, 若有, 
    // 返回TRUE; 否则, 返回FALSE

    pParent = pWin;
    while (!ISROOTWIN(pParent))
    {
        pBrother = pParent->pParent->pChild;
        while (pBrother != pParent)
        {
            if (ISMAPPED(pBrother) && 
                IntersectRect(NULL, &pWin->rcClient, &pBrother->rcWindow))
                return FALSE;
            
            pBrother = pBrother->pNext;
        }
        
        pParent = pParent->pParent;
    }

    return TRUE;
}

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
**      ��ʱ����bRepaint�����������ƶ��Ĵ��ڱ���ȫ���ػ���
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

    // ע�⣬��ʱ���ڵĿͻ���������δ���㣬��ˣ��ڴ���WM_NCCALCSIZE��Ϣ��
    // ���ܴ������ƶ��ͻ����Ӵ��ڣ������Դ������ƶ��ǿͻ����Ӵ���

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

    // ��֧��root window�Ĺ���
    if (ISROOTWIN(pWin))
        return;

    if (pWin->pChild)
    {
        // ��ɴ��ں�����ڵĹ���, ������ڹ�����Щ�����ڲü���������µ�
        // ��¶����, ʹ��Щ�µı�¶����ʧЧ.
        if (ScrollChildren(pWin, dx, dy, prcClip, bTop))
        {
            // ������ڱ�¶�������Ӵ��ڵĲü�, ʹ���ڵı�¶����ʧЧ
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
    /* �ڴ��������Ĺ����У����������ǣ�
    ** ʹ��ǰ�������ʽ����OffsetRect
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

        // ��ô���������һ����Ҫ����Ĵ��ڣ�ԭ�����£�
        //     * ������Ӵ�����Ҫ������ǰ���ڵ��Ӵ���ΪҪ����Ĵ��ڡ�
        //     * û���Ӵ�����Ҫ��������һ���ֵܴ��ڣ���ǰ���ڵ���һ����
        //       �ܴ���ΪҪ����Ĵ���
        //     * ��ǰ����û���Ӵ��ں��ֵܴ�����Ҫ������ȡ��ǰ���ڵ����ȴ�
        //       �ڵ��ֵܴ���ΪҪ����Ĵ��ڣ�������ݵ���ʼ���ȴ��ڣ���ʾ��
        //       �д����Ѿ�������ϡ�
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
        // ������������ڣ����Ƚ���������MoveToTop
        if (pWin->pOwner)
            MoveWindowTreeToTop(pWin->pOwner, pEndWin);

        // �ҳ����һ��pWinӵ�еĴ��ڣ�����ֱ��ӵ�кͼ��ӵ�еĴ���
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

    // pLastWin����һ��������pEndWin˵���Ѿ��Ǻ��ʵ�z-order��
    if (pLastWin->pPrev == pEndWin)
        return;

    // ��ʱpLastWin�϶����ǵ�һ���Ĵ���
    ASSERT(pLastWin->pPrev != NULL);

    // ����pWin��pLastWin��һ�鴰���ƶ���pEndWin֮������Ĵ���ʧЧ�ʹ��ڱ�
    // ¶����ʧЧ
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

    // ��pWin��pLastWin��һ�鴰�ڴӴ���������ժ�£����뵽pEndWin�±�

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
**      ��ɴ���pChild���������ڵĹ���, ������ڹ�����Щ���ڲ����µ�
**      ��¶����, ʹ��Щ�µı�¶����ʧЧ.
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
    
    // û�пͻ����Ӵ��ڣ�����FALSE��ʾû���Ӵ�����Ҫ�ƶ�
    if (!pChild)
        return FALSE;

    // bUpdate������ʾ�����Ƿ���Ҫ�ж����ڹ��������µ�ʧЧ�������prcClip
    // Ϊ�վ��Σ���û�д��ڲ����µ�ʧЧ����ʵ������������ڲ������µ�ʧЧ
    // �������Ӵ��ڲ���Ҫ���ж��Ƿ�����µ�ʧЧ��������Ϊ�˼򻯳��򣬲�
    // �������жϣ�ֻ��prcClip�Ƿ�Ϊ�վ����������Ƿ��жϴ��ڵ�ʧЧ��
    bUpdate = !IsRectEmpty(prcClip);

/* */
    /* �ڴ��������Ĺ����У����������ǣ�
    ** ʹ��ǰ�������ʽ����,�ȴ�����ڵ�
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
                // ��������µ�ʧЧ����, ֱ���޸Ĵ��ڵ�λ��
                if (!bUpdate)
                {
                    // �ƶ�����λ��
                    OffsetRect(&pChild->rcWindow, dx, dy);
                    OffsetRect(&pChild->rcClient, dx, dy);
                    
                    // ������ڵĸ�������Ϊ��, �ƶ���������
                    if (!IsRectEmpty(&pChild->rcUpdate))
                        OffsetRect(&pChild->rcUpdate, dx, dy);
                    
                    // ���ڴ���λ���ƶ�, ����ʹ���ڵı�¶�����ʧЧ
                    WND_InvalidateExposeTbl(pChild);
                }
                else
                {
                    // �����ڹ���ǰ�ڲü������е������Դ������걣����rcOld
                    // ��, ����ת��ǰ���������жϾ����Ƿ��ǿվ���
                    if (IntersectRect(&rcOld, prcClip, &pChild->rcWindow))
                        OffsetRect(&rcOld, -pChild->rcWindow.left, 
                        -pChild->rcWindow.top);
                    
                    // �ƶ�����λ��
                    OffsetRect(&pChild->rcWindow, dx, dy);
                    OffsetRect(&pChild->rcClient, dx, dy);
                    
                    // ������ڵĸ�������Ϊ��, �ƶ���������
                    if (!IsRectEmpty(&pChild->rcUpdate))
                        OffsetRect(&pChild->rcUpdate, dx, dy);
                    
                    // ���ڴ���λ���ƶ�, ����ʹ���ڵı�¶�����ʧЧ, ����
                    // �ڵ���WND_InvalidateRect֮ǰ����
                    // WND_InvalidateExposeTbl, �������WND_InvalidateRect
                    // ���ܷ��������ء�
                    WND_InvalidateExposeTbl(pChild);
                    
                    // �����ڹ������ڲü������е������Դ������걣����rcNew
                    // ��, ����ת��ǰ���������жϾ����Ƿ��ǿվ���
                    if (IntersectRect(&rcNew, prcClip, &pChild->rcWindow))
                        OffsetRect(&rcNew, -pChild->rcWindow.left, 
                        -pChild->rcWindow.top);
                    
                    // ������ڹ������ڲü����е�����û����ȫ�����ڹ���ǰ
                    // �ڲü����е�������, ˵����������ʹ���ڲ������µı�
                    // ¶����, ʹ�µı�¶����ʧЧ; ����, ˵����������û��
                    // ʹ���ڲ����µı�¶����, �����¾�������Ϊ�վ���
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
                // ���¼��㴰�ڵ�λ��
                OffsetRect(&pChild->rcWindow, dx, dy);
                OffsetRect(&pChild->rcClient, dx, dy);
                
                // ���ڴ���λ���ƶ�, ����ʹ���ڵı�¶�����ʧЧ, �����ڵ���
                // WND_InvalidateRect֮ǰ����WND_InvalidateExposeTbl, ����
                // ����WND_InvalidateRect���ܷ��������ء�
                WND_InvalidateExposeTbl(pChild);
                
                // ʹ��������ʧЧ
                /* 
                ** the unmapped child window should not be invalidate the 
                ** update rect.
                */
                if (ISMAPPED(pChild))
                    WND_InvalidateRect(pChild, &pChild->rcWindow, TRUE);
            }
        }

        // ��ô���������һ����Ҫ����Ĵ��ڣ�ԭ�����£�
        //     * ������Ӵ�����Ҫ������ǰ���ڵ��Ӵ���ΪҪ����Ĵ��ڡ�
        //     * û���Ӵ�����Ҫ��������һ���ֵܴ��ڣ���ǰ���ڵ���һ����
        //       �ܴ���ΪҪ����Ĵ���
        //     * ��ǰ����û���Ӵ��ں��ֵܴ�����Ҫ������ȡ��ǰ���ڵ����ȴ�
        //       �ڵ��ֵܴ���ΪҪ����Ĵ��ڣ�������ݵ���ʼ���ȴ��ڣ���ʾ��
        //       �д����Ѿ�������ϡ�
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

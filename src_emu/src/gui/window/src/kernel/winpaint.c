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
static WORD     wPaintNum;      // �����ҽ���һ��������Ҫ�ػ�ʱ, ��ʶ���
                                // ��Ҫ�ػ��Ĵ���; ����ΪNULL
//static PWINOBJ  pPaintWin;      // �����ҽ���һ��������Ҫ�ػ�ʱ, ��ʶ���
                                // ��Ҫ�ػ��Ĵ���; ����ΪNULL

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

    // ���Ǵ���˽��DC����˽��DC����ε��ã����Եõ������ͬDC��������ӦpWin��
    if (!ISOWNDC(pWin) && !ISCLASSDC(pWin))
    {
        pDC = DC_GetWindowDCFromCache(pWin, TRUE, TRUE);
        if (pDC == NULL)
            return NULL;
    // ����pda860��ʾģʽ�����л������Խ��Ǵ���˽��DC����ΪĬ��ֵ�Ĳ�����
    // �ͷ�DCʱ���и�Ϊ����DCʱ���У�����DC����ɫֵ���ܳ���
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
/* ���ͷ�ʱ��Ӧ�ÿ��ǵ����DCһ����CacheDC���������OWNERDC��
** ����ʧһ��CacheDC
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

    // Ϊ��֤��Cache�������DC���ͷţ����ﲻ�ж�pDC��pWin��һ����
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
    /* BUG �����ھ���CS_OWNER���ʱ������GetWindowDC��Ӧ�û��CacheDC������
    ** ��ʱû�н����жϣ���������жϲ��Ǵ��ڶ����ڲ�DC����һ����CacheDC
    */
    else if (pDC != pWin->pDC)
    {
        /* ����OWNERDC��������GetWindowDC�õ�������CacheDC */
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


    // ���Ǵ���˽��DC, ����pWin->pDC(��ʾ��δȡ��DC)����ΪĬ��ֵ
    // ����pda860��ʾģʽ�����л������Խ��Ǵ���˽��DC����ΪĬ��ֵ�Ĳ�����
    // �ͷ�DCʱ���и�Ϊ����DCʱ���У�����DC����ɫֵ���ܳ���
    /* ���ڶ�ε���BeginPaint������ʵ�������ڲ�ֻ���е�һ�β�����������Ч*/
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
    
    // ����BeginPaint���������EndPaint����, DCӦ����WDA_UPDATE����. ���DC
    // û��WDA_UPDATE����, ��������������ԭ��:
    //     1. û�е���BeginPaint����; 
    //     2. �����ػ������γ���Ƕ��, EndPaint�����ѱ�����
    //     3. �ڵ���BeginPaint����֮��, ������GetDC��ReleaseDC����.
    // Ϊʹ����ϵͳ�ܹ�����Щ���������, ���ﲻ��DC��WDA_UPDATE���Ե��ж�

    // Ӧ�ó����������ӦWM_PAINT��Ϣʱֱ�ӻ��ӵ���WND_InvalidateRect
    // (����InvalidateRect, SetScrollInfo, MoveWindow��)�ٴ�ʹbUpdate��
    // ־��λ, ��ʱ���µĸ����������. ��� bUpdate����λʱ������մ���
    // �ĸ�������, �Ա�ʹ���ڲ�����һ�δ����ػ���Ϣ. 
    
    // ע: WM_NCPAINT��Ϣ����Ӧ����ͬ������, ���ǵ�Ӧ�ó���һ�㲻��Ӧ
    // WM_NCPAINT��Ϣ, ������WM_NCPAINT��Ϣ�󲻿����������.
    
    /* PAINT_DRAWING �����ڻ��ƹ����У�û�еõ��µĸ�����Ϣ��������Ϊ
    ** PAINT_NO״̬��
    ** �������PAINT_DRAWING ״̬�����Ȼ�ֵõ����µĸ�����Ϣ�������Ӵ��ڵ�
    ** ��Ϣ�ӳٵ��Ժ�һ��ʵʩ����Ϊ�ڻ��ƹ����У����¾��β�û����գ��ڲ���
    ** �µĸ��¾���ʱ����Ȼ����ԭ�о��Σ��ʶ����Ӵ��ڲ��ᶪ����������ֻ��
    ** ����λ����˷ѵ��ˡ�
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

        /* �����Ƿ�ü��Ӵ��ڣ���Ҫ���Ӵ��ڵı�־����Ϊ��ʹ��һ���Ĵ���
        ** ���ü��Ӵ��ڣ������ϼ��Ŀ�����Ȼ�ü���������InvalidateRect���Ѿ�
        ** �����жϡ������ﶼҪ���б�־�жϡ�
        */
        pChild = pWin->pChild;
        while (pChild != NULL)
        {
            if (ISMAPPED(pChild) && pChild->flagUpdate == PAINT_WAIT)
            {
                wPaintNum ++;
                pChild->flagUpdate = PAINT_QUEUE;
                /* �Ӵ�����Ҫ�ж��ػ������Ƿ�NC�� */
                bNCPaint = !IntersectRect(&rcUpdate, &pChild->rcUpdate, &pChild->rcClient);
                MSQ_InsertPaintMsg(pChild, bNCPaint);
            }

            pChild = pChild->pNext;
        }
    }
    
    // Show the caret if the window owns the caret
    WS_ShowCaret(pWin);

    // ���Ǵ���˽��DC, �ͷŵ�cache
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
**      Adds a rectangle to the specified window��s update rectangle, and
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

    // ����û��ӳ�����ȫ����, ֱ�ӷ��ء�(ע���ڴ���������ʱ�����ڵı�¶
    // ����һ�㻹û�м��㣬��ʱ��ISOVERLAPPED����FALSE)
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
    /* �ڴ��������Ĺ����У����������ǣ�
    ** ʹ��ǰ�������ʽ����WND_InvalidateRect
    */

        pChild = pWin->pChild;
        bFromChild = FALSE;
        while (pChild)
        {
            // ע��: ���ﲻ��ʹ��pRect���ж��Ӵ����Ƿ���Ҫ����, ��ΪpRect
            // �����ǵ�ǰ�������յ�ʧЧ��. ����ʧЧ���ĺϲ�����, ���ڵ�ʧ
            // Ч�����ܻ����ָ����ʧЧ��. ���Ӧ��ʹ�ô��ڽṹ�е�ʧЧ��
            // �ж��Ӵ����Ƿ���ҪʧЧ
            /* 
            ** ������ҪʧЧ���д��ڵ����������ֻ���Ӵ��ڵ�ʧЧ�����жϣ�
            ** ����Ҫ�����Ƿ��б�¶�����ж�
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
            // ��ô���������һ����Ҫ����Ĵ��ڣ�ԭ�����£�
            //     * ������Ӵ�����Ҫ������ǰ���ڵ��Ӵ���ΪҪ����Ĵ��ڡ�
            //     * û���Ӵ�����Ҫ��������һ���ֵܴ��ڣ���ǰ���ڵ���һ����
            //       �ܴ���ΪҪ����Ĵ���
            //     * ��ǰ����û���Ӵ��ں��ֵܴ�����Ҫ������ȡ��ǰ���ڵ����ȴ�
            //       �ڵ��ֵܴ���ΪҪ����Ĵ��ڣ�������ݵ���ʼ���ȴ��ڣ���ʾ��
            //       �д����Ѿ�������ϡ�
            /* 
            ** 1��Ӧ�ö�û��CLIPCHILDREN���ԵĴ��ڲű���
            ** 2��������ҪʧЧ���д��ڵ��������Ҫ����
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

    // ���ڱ���ȫ����, ֱ�ӷ���
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
        ** wPaintNumӦ�ñ�ʾ��ǰpaint�����е���Ϣ���� 
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
            /* �����ڲü��Ӵ��ڣ��Ӵ��ڿ���ֱ���ػ���
            ** ������Ҫһ���������Ǹ����ڱ������ڵȴ�״̬����Ϊ�������
            ** �ȴ�״̬����ø����ڵ����ȴ��ڱ�Ȼ�в��ü��Ӵ��ڵ����������
            ** Ҳ���뱣֤����˳��
            ** ������EndPaint�оͱ����ÿ�����ڵ������Ӵ��ڶ������ж��Ƿ���
            ** �ȴ�
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
            /* �����ڲ��ü��Ӵ��ڣ���Ҫ��֤�����ڻ�����ٻ����Ӵ��� */
            if (pWin->pParent->flagUpdate == PAINT_NO)
            {
                /* ���߸����ڲ���Ҫ�ػ����Ӵ��ڿ���ֱ���ػ� */
                if (pWin->flagUpdate != PAINT_QUEUE)
                {
                    wPaintNum ++;
                    pWin->flagUpdate = PAINT_QUEUE;
                    MSQ_InsertPaintMsg(pWin, bNCPaint);
                }
            }
            else if (pWin->flagUpdate == PAINT_QUEUE)
            {
                /* �����丸������һ�εõ����ػ���Ϣ����ʱ���Ӵ��ڵ��ϴ��ػ�
                ** ��û�н��У�ֱ��ɾ���������ϴε���Ϣ���ϲ���Ч����󣬵�
                ** �丸������һ�λ�������ٵõ�WM_PAINT��Ϣ
                */
                ASSERT(wPaintNum != 0);
                wPaintNum --;
                MSQ_DeletePaintMsg(pWin);
                pWin->flagUpdate = PAINT_WAIT;
            }
            else if (pWin->flagUpdate == PAINT_DRAWING)
            {
                /* �Ӵ������ڴ����ػ��Ĵ����У���ʱ�����������յ����ػ����
                ** ��ʱֻ�ܽ��Ӵ��ڵ���Ч������кϲ����ȴ������ڻ�����ٽ���
                ** �ػ档ʵ���ϣ��Ѿ��ػ�Ĳ��־Ͷ����ˣ����ܱ�֤��ȷ�ԡ�
                ** ���ñ�־ΪPAINT_WAIT�����Ӵ��ڻ��������������־����
                ** PAINT_DRAWING����ô��Ȼ����Ҫ�ٴ��ػ棬�Ͳ���ΪPAINT_NO�ˡ�
                */
                pWin->flagUpdate = PAINT_WAIT;
            }
            else
            {
                /* �Ӵ��ڴ��ڵȴ�״̬�����������״̬������ֱ�Ӻϲ��ػ�����*/
                pWin->flagUpdate = PAINT_WAIT;
            }
        }

        if (IsRectEmpty(&pWin->rcUpdate))
            CopyRect(&pWin->rcUpdate, &rcUpdate);
        else
        {
            // ����ԭ����ʧЧ���򲻿�, ���µ�ʧЧ����;ɵ�ʧЧ����ϲ�
            UnionRect(&pWin->rcUpdate, &pWin->rcUpdate, &rcUpdate);

            // ����ʧЧ����ĺϲ����ܻ����󴰿ڵ�ʧЧ����, ʹ����û��ʧЧ
            // ������ʧЧ. �������û��WS_CLIPCHILDREN���,����λ�����ӵ�
            // ʧЧ������Ӵ���ʧЧ, ��Ϊ���ڵ��ػ����ܻḲ���Ӵ���, ����
            // ���������ܻ����𴰿ڵĶ����ػ�, �����ܱ�֤�ػ�����ȷ��.

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

    // �����ǰ����ʧЧ����Ϊ��, ���������Ҫ���±�־, �����ػ����ڼ���
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
    ** ����UpdateWindowʱ��������Ӵ��ڣ��Ҹ�������Ҫ�ػ棬��ô
    ** ֱ����Update�����ڡ�
    ** ������Ϊ�����ܴ��ڸ�������PAINT_NO,�����ȴ����Ƿ�PAINT_NO��
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
    /* �ڴ��������Ĺ����У����������ǣ�
    ** ʹ��ǰ�������ʽ����WND_Update
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
            
        // ��ô���������һ����Ҫ����Ĵ��ڣ�ԭ�����£�
        //     * ������Ӵ�����Ҫ������ǰ���ڵ��Ӵ���ΪҪ����Ĵ��ڡ�
        //     * û���Ӵ�����Ҫ��������һ���ֵܴ��ڣ���ǰ���ڵ���һ����
        //       �ܴ���ΪҪ����Ĵ���
        //     * ��ǰ����û���Ӵ��ں��ֵܴ�����Ҫ������ȡ��ǰ���ڵ����ȴ�
        //       �ڵ��ֵܴ���ΪҪ����Ĵ��ڣ�������ݵ���ʼ���ȴ��ڣ���ʾ��
        //       �д����Ѿ�������ϡ�
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

    /* ����Update���ã�ֱ��Send��Ϣ������˳����UpdateTree��֤��Ӧ�����
    ** ���е����PAINT������Ϣ����������ŵ�EndPaint�н���
    */

    // �û���WM_PAINT��Ϣ֮�����BeginPaint + EndPaint������������, 
    // DEBUG�����¸�������, ����ֱ�ӷ���.
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
        ** ����NCPAINT��Ϣ��ɾ�������ڶ�����ִ�� 
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

        /* �Էǿͻ�����ʹ��UpdateWindow*/
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

    // ������ɾ�������صĴ��ڸ��±�־����λ, ������±�־, �����ٸ��´���
    // ����. �����ʹ���´��ڼ���������.
    if (pWin->flagUpdate != PAINT_NO)
    {
        if (pWin->flagUpdate == PAINT_QUEUE)
        {
            // �����ػ����ڼ���
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
    /* �������ٺ�Paint��Ϣ�����п��ܻ���PAINT��Ϣ����Ҫ�õ���Ч����Ϣ */
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
            /* ��������Ҫ�ػ�,���������Ϣ */
            if (pWin->pParent != NULL && 
                pWin->pParent->flagUpdate != PAINT_NO && 
                !ISCLIPCHILDREN(pWin->pParent) && 
                IntersectRect(&rcUpdate, &pWin->rcUpdate, &pWin->pParent->rcUpdate))
            {
                /* �������������������ϢӦ����ǰ��ɾ���ˣ���Ӧ�û����ڣ�
                ** ������Ը���ASSERT��
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
                /* �ڴ���NCPAINT��û�в����µĸ�����������״̬ΪPAINT_DRAWING */
                IntersectRect(&pWin->rcUpdate, &pWin->rcUpdate, &pWin->rcClient);
                if (IsRectEmpty(&pWin->rcUpdate))
                {
                    /* �޸ĸ�����Ϊ�ͻ��������� */
                    /* ����ͻ���������Ϊ�գ������� */
                    pWin->flagUpdate = PAINT_NO;
                    pWin->bErase = FALSE;
                }
                else
                {
                    /* �ڲ���NCPAINT��Ϣ����ܻ���ϲ���������ʹ�ø��������ٽ�����
                    ** NC������˻�Ҫ����PAINT��Ϣ
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
**      Scrolls the content of the specified window��s client area. 
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
**      combined into the window��s update region. The application 
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

    // ���������ش��ڵ¹���
    if (!ISMAPPED(pWin))
        return;

    // ������ڵĵ�ǰʧЧ����Ϊ��, �������ڵĵ�ǰʧЧ����
    if (!IsRectEmpty(&pWin->rcUpdate))
    {
        /* 
        ** ����Ķ��ھɴ���ϵͳ���Ѿ���֤�ˡ��������´���ϵͳ���޸ġ�
        ** ֱ�ӵ�SendMessage��NCPAINT��Ϣִ�к��൱��ִ����
        ** UpdateWindow���������ڻ�û�л��ƣ���Ȼ��λ�����Ч�ˡ���
        ** �ڸ����ڻ����������ڵĸ�����ȴû���˷ǿͻ������֣�����
        ** �ǿͻ���û�и��¡�
        */
        // ������ڵĸ�������������ڵķǿͻ�������, ����ǿͻ���
        SubtractRect(&rcNCUpdate, &pWin->rcUpdate, &pWin->rcClient);
        if (!IsRectEmpty(&rcNCUpdate))
        {
            OffsetRect(&pWin->rcUpdate, dx, dy);
            IntersectRect(&pWin->rcUpdate, &pWin->rcUpdate, 
                &pWin->rcClient);
        }

/*
        // ������ڵĸ�������������ڵķǿͻ�������, ����WM_NCPAINT��Ϣʹ
        // ���ں����ػ����ڵķǿͻ���
        if (pWin->rcUpdate.left < pWin->rcClient.left ||
            pWin->rcUpdate.right > pWin->rcClient.right ||
            pWin->rcUpdate.top < pWin->rcClient.top ||
            pWin->rcUpdate.bottom > pWin->rcClient.bottom)
        {
            pWin->bNCPaint = TRUE;  // Set the bNCPaint flag to TRUE
            WND_SendMessage(pWin, WM_NCPAINT, (WPARAM)0, (LPARAM)0);
            pWin->bNCPaint = FALSE; // Set the bNCPaint flag to FALSE        
            
            // ���ڵķǿͻ����Ѿ��ػ�, �Ӵ��ڵ�ʧЧ������ȥ���ǿͻ�������
            IntersectRect(&pWin->rcUpdate, &pWin->rcUpdate, 
                &pWin->rcClient);
        }

        // ��ʱ������ڵ�ʧЧ����Ϊ��, �������ڵ�ʧЧ����
        if (!IsRectEmpty(&pWin->rcUpdate))
        {
            OffsetRect(&pWin->rcUpdate, dx, dy);
            IntersectRect(&pWin->rcUpdate, &pWin->rcUpdate, 
                &pWin->rcClient);
        }
*/
    }

    // ������Ҫ�����ľ�������(��Ļ����)
    if (prcScroll)
    {
        // ������������ε������ɴ��ڿͻ�������ת��Ϊ��Ļ����
        rcScroll = *prcScroll;
        WND_GetRect(pWin, &rcClient, W_CLIENT, XY_SCREEN);
        OffsetRect(&rcScroll, rcClient.left, rcClient.top);

        // ��������Ӧ�ڴ��ڵĿͻ�����
        IntersectRect(&rcScroll, &rcScroll, &pWin->rcClient);
    }
    else    // ָ����������ľ���ָ��ΪNULL, ���������Ϊ�����ͻ���
        rcScroll = pWin->rcClient;

    // ��������Ĳü�����(��Ļ����)
    if (prcClip)
    {
        // �������ü����ε������ɴ��ڿͻ�������ת��Ϊ��Ļ����
        rcClip = *prcClip;
        WND_GetRect(pWin, &rcClient, W_CLIENT, XY_SCREEN);
        OffsetRect(&rcClip, rcClient.left, rcClient.top);

        // �ü�����Ӧ�ڴ��ڵĿͻ�����
        IntersectRect(&rcClip, &rcClip, &pWin->rcClient);
    }
    else    // ָ����������ľ���ָ��ΪNULL, ���������Ϊ�����ͻ���
        rcClip = pWin->rcClient;

    // prcScroll����ΪNULL, �ƶ����еĺ�����ڵ���Ӧ����λ�ò�ʹ��Ҫ�ػ���
    // ������ڵ���Ӧ����ʧЧ;ͬʱ, �������ӵ��Caret, �ƶ�Caret����Ӧ��λ
    // ��. ���prcScroll������ΪNULL, ���ƶ����ڵĺ������, ֱ�ӵ�����ʾ��
    // ������������Ļ��������������Ӧ����Ļ����.
    if (!prcScroll)
    {
        pCaretWin = WS_GetCaretWindow();

        // ������ڻ򴰿ڵĺ������ӵ��caret, ����caret����Ӱ������Ĺ���
        // ����
        if (WND_IsDescendant(pCaretWin, pWin))
            WS_HideCaret(pCaretWin);
    }

    bTop = IsTopWindow(pWin);

    // ֱ�ӵ�����ʾ�豸����������Ļ��������������Ӧ����Ļ����
    if (bTop)
    {
        RECT rcTemp;
        /* ���ڹ�����Ŀ�����������NC�������ཻ����ô��ִ�й��������Ǻϲ�
        ** ������������
        */
        IntersectRect(&rcTemp, &rcScroll, &rcClip);
        OffsetRect(&rcTemp, dx, dy);

        if (IntersectRect(NULL, &pWin->rcUpdate, &rcTemp))
        {
            /* ��������������в�����Ҫ�ػ�����ô���ﲻ��Ҫ���й���������
            ** ֻҪ��Ӹ�������
            */
            UnionRect(&rcNCUpdate, &rcNCUpdate, &rcTemp);
        }
        else if (IntersectRect(NULL, &rcNCUpdate, &rcTemp))
        {
            /* �����������������Ҫ�ػ���NC�����ཻ����ôΪ�˱�֤NC������
            ** ��ͻ����Ļ���ͬʱ���������ﲻ���й���������ֻ������������
            ** �ӵ�����������
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
        // �������ӵ��caret, ��Ӧ�ı�caret��λ�������caret�Ĺ���
        if (pCaretWin == pWin)
            WS_ScrollCaret(pWin, dx, dy);
        
        // ����������Ӵ���, �����Ӵ���
        if (pWin->pChild)
            WND_ScrollChild(pWin, dx, dy, &rcClip, bTop);

        // �������ӵ��caret, ������ʾcaret
        if (WND_IsDescendant(pCaretWin, pWin))
            WS_ShowCaret(pCaretWin);
    }

    // ��������Ͳü������ཻʱ���ڿ�����Ҫ����, ���㴰�ڵ�ʧЧ��
    if (IntersectRect(&rcUpdate, &rcClip, &rcScroll))
    {
        if (bTop)
        {
            // ���㴰����Ҫ���µľ�������
            if (dx == 0)        // ��ֱ����
            {
                if (dy > 0)     // ���¹���
                {
                    if (dy < rcUpdate.bottom - rcUpdate.top)
                        rcUpdate.bottom = rcUpdate.top + dy;
                }
                else            // ���Ϲ���
                {
                    dy = -dy;
                    if (dy < rcUpdate.bottom - rcUpdate.top)
                        rcUpdate.top = rcUpdate.bottom - dy;
                }
            }                          
            else if (dy == 0)   // ˮƽ����
            {
                if (dx > 0)     // ���ҹ���
                {
                    if (dx < rcUpdate.right - rcUpdate.left)
                        rcUpdate.right = rcUpdate.left + dx;
                }
                else            // �������
                {
                    dx = -dx;
                    if (dx < rcUpdate.right - rcUpdate.left)
                        rcUpdate.left = rcUpdate.right - dx;
                }
            }
        }

        UnionRect(&rcUpdate, &rcNCUpdate, &rcUpdate);
        // ʹ������Ҫ���µ�����ʧЧ
        WND_InvalidateRect(pWin, &rcUpdate, TRUE);
    }

/*
    // ����ÿһ����¶������豸����������Ļ��������������Ӧ����Ļ����

    // ���㴰�ڵı�¶����, �����Ƕ��Ӵ��ڵĲü�
    if (ISCLIPCHILDREN(pWin) && pWin->pChild && !ISNCCHILD(pWin->pChild))
    {
        pWin->dwStyle &= ~WS_CLIPCHILDREN;
        pExposeTbl = WND_MakeExposeTbl(pWin);
        pWin->dwStyle |= WS_CLIPCHILDREN;
    }
    else
        pExposeTbl = WND_GetExposeTbl(pWin);

    // ���ι���ÿ����¶����
    for (i = 0; i < pExposeTbl->count; i++)
    {
        // ���һ����������
        if (!IntersectRect(&rcSrc, &rcScroll, &pExposeTbl->rect[i]))
            continue;

        // ����ù�������������λ��
        rcDest = rcSrc;
        OffsetRect(&rcDest, dx, dy);
        IntersectRect(&rcDest, &rcDest, &rcClip);

        for (j = 0; j < pExposeTbl->count; j++)
        {
            // �������������ÿ�����ڱ�¶���Ľ�Ϊ�����Ĳü�����
            if (IntersectRect(&rcPartClip, &rcDest, &pExposeTbl->rect[j]))
            {
                // ������Ŀ��������Ҫ����, ʹ������Ŀ��������Ч
                // WND_ValidateRect(pWin, &rcPartClip);

                g_DisplayDrv.ScrollScreen(g_pDisplayDev, &rcSrc, 
                    &rcPartClip, rcSrc.left + dx, rcSrc.top + dy);
            }
        }

        // ��������Ͳü������ཻʱ���ڿ�����Ҫ����, ���㴰�ڵ�ʧЧ��
        if (IntersectRect(&rcUpdate, &rcClip, &rcSrc))
        {
            // ���㴰����Ҫ���µľ�������
            if (dx == 0)        // ��ֱ����
            {
                if (dy > 0)     // ���¹���
                {
                    if (dy < rcUpdate.bottom - rcUpdate.top)
                        rcUpdate.bottom = rcUpdate.top + dy;
                }
                else            // ���Ϲ���
                {
                    dy = -dy;
                    if (dy < rcUpdate.bottom - rcUpdate.top)
                        rcUpdate.top = rcUpdate.bottom - dy;
                }
            }                          
            else if (dy == 0)   // ˮƽ����
            {
                if (dx > 0)     // ���ҹ���
                {
                    if (dx < rcUpdate.right - rcUpdate.left)
                        rcUpdate.right = rcUpdate.left + dx;
                }
                else            // �������
                {
                    dx = -dx;
                    if (dx < rcUpdate.right - rcUpdate.left)
                        rcUpdate.left = rcUpdate.right - dx;
                }
            }
            
            // ʹ������Ҫ���µ�����ʧЧ
            WND_InvalidateRect(pWin, &rcUpdate, TRUE);
        }
    }

    if (ISCLIPCHILDREN(pWin) && pWin->pChild && !ISNCCHILD(pWin->pChild))
        MemFree(pExposeTbl);

    if (!prcScroll)
    {
        // �������ӵ��caret, ��Ӧ�ı�caret��λ�������caret�Ĺ���
        if (pCaretWin == pWin)
            WS_ScrollCaret(pWin, dx, dy);
        
        // ��������пͻ����Ӵ���, �������пͻ����Ӵ���
        if (pWin->pChild && !ISNCCHILD(pWin->pChild))
        {
            WND_ScrollChild(pWin, dx, dy, &rcClip);

            // ������ڱ�¶�������Ӵ��ڵĲü�, ʹ���ڵı�¶����ʧЧ
            if (ISCLIPCHILDREN(pWin))
                WND_InvalidateExposeTbl(pWin);
        }

        // �������ӵ��caret, ������ʾcaret
        if (WND_IsDescendant(pCaretWin, pWin))
            WS_ShowCaret(pCaretWin);
    }
*/
}

/*
**  Function : IsTopWindow
**  Purpose  :
**      ����һ�������Ƿ��Ƕ��㴰��(û�г��Ӵ���֮����������ڸ��Ǵ��ڵ�
**      �ͻ���)
*/
static BOOL IsTopWindow(PWINOBJ pWin)
{
    PWINOBJ pParent, pBrother;

    ASSERT(pWin != NULL);

    // �ж��Ƿ����ֵܴ��ڻ����ȴ��ڵ��ֵܴ��ڸ��Ǵ��ڵĿͻ���, ����, 
    // ����TRUE; ����, ����FALSE

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

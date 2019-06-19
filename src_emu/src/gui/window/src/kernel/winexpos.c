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

#define MAX_EXPOSE_RECT 200     // �������¶���θ���

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

    // �������Ϊ����״̬, ֱ�ӷ���NULL
    if (!ISMAPPED(pWin))
        return NULL;

    /* ���ϴμ���ı�¶����ͬ���������� */
    if (pWinExpose == pWin && bExposeTableValid == TRUE)
        return (PEXPOSETBL)ExposeTable;

    // ���ü��ֵܴ��ڵ��Ӵ���ͬʱ����CS_PARENTDC���񣬱�¶��ʹ�ø�����
    // ���ü��Ӵ��ڵı�¶��
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
    // ʹ�ô�������ı�¶��, ������ڱ�¶��������ṹ��Ч, ��Ҫ���¼��㴰
    // �ڵı�¶����
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

    /* ֻ�е����еı�¶��������Ҫ���±�¶��ʱ��������� 
    ** �����������ڿ����ڻ���ʱ��֤
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

    /* ���ڲ��ü��Ӵ��ڵ����������¶��ʱӦ�ôӸ������������ν�����Ч����
    ** �������������ܱ�֤WM_PAINT��˳���͡�
    ** 2002/11/18����Bug�����������Լ����Ӵ��ڻ��ƴ���
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
        // ����¶�Ĵ��ڲ��ü��Ӵ���
        if (!ISCLIPCHILDREN(pWin))
        {
            BOOL bTop;
            
            if (pEventWin)
                bTop = TRUE;
            else
                bTop = FALSE;
            
            // �����������Ӵ���
            pChild = pWin->pChild;
            
            // ����֮�����δ���ÿ���Ӵ��ڵı�¶, �¼�����֮�ϵĴ��ڱ�¶����
            // ����ҪʧЧ
            while (pChild != NULL)
            {
                if (pChild == pEventWin)
                    bTop = FALSE;
                else
                {
                    // ��������ʾ�Ĵ���
                    if (ISMAPPED(pChild))
                    {
                        if (!bTop)
                            WND_InvalidateExposeTblTree(pChild, &rcExpose);
                        InvalidateWindowTree(pChild, &rcExpose);
                    }
                }
                
                // ��һ���ֵܴ���
                pChild = pChild->pNext;
            }
        }
        else
        {
            // �����Ӵ��ڵı�¶, ���¼�����֮�ϵĴ����Լ��¼����ڱ�������
            pChild = pWin->pChild;

            // ���λ���¼�����֮�µ�������Ĵ���, ����¼�����Ϊ��, ��
            // ������������Ӵ���
            if (pEventWin)
            {
                while (pChild != pEventWin)
                {
                    // �����ǰ��������ʾ�Ĵ���, ����ǰ���ڵĴ��ھ��δӱ�¶
                    // �����м�ȥ, �����¶�����Ϊ��, ˵����¶����Ӱ��Ĵ�
                    // ���Ѿ�������, ����Ҫ������������Ĵ���, ֱ�ӷ���
                    if (ISMAPPED(pChild))
                    {
                        SubtractRect(&rcExpose, &rcExpose, 
                            &pChild->rcWindow);
                        if (IsRectEmpty(&rcExpose))
                            return;
                    }

                    // ��һ���ֵܴ���
                    pChild = pChild->pNext;
                }

                pChild = pEventWin->pNext;
            }
            
            // ����֮�����δ���ÿ���Ӵ��ڵı�¶
            while (pChild != NULL)
            {
                // �����ǰ��������ʾ�Ĵ���, ����ǰ���ڵĴ��ھ��δӱ�¶
                // �����м�ȥ, �����¶�����Ϊ��, ˵����¶����Ӱ��Ĵ�
                // ���Ѿ�������, ����Ҫ������������Ĵ���, ֱ�ӷ���
                if (ISMAPPED(pChild))
                {
                    WND_Expose(pChild, &rcExpose, NULL);
                    SubtractRect(&rcExpose, &rcExpose, 
                        &pChild->rcWindow);
                    if (IsRectEmpty(&rcExpose))
                        return;
                }
                
                // ��һ���ֵܴ���
                pChild = pChild->pNext;
            }
        }
    }

    /* ���ڲü��Ӵ��ڵ����������¶��ʱ��Ч�����ڲ��ϼ�С����˸����ڵ���
    ** Ч�������ŵ�������������Ա������ĸ������ػ档
    ** 2002/11/18����Bug�����������Լ����Ӵ��ڻ��ƴ���
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
**      ����һ��������ָ���ľ�������ı�¶������ṹ
**  Params   :
**      pWin    : Specifies the window to be make expose table.
**      pRect   : Specifies the rectangle to expose. if NULL, indicates 
**                all window rectangle expose.
**  Remark   :
**      ���ܸ���һ�����ڵĴ����У�
**          1.   �ô��ڵ�ֱ���Ӵ��ڣ�
**          2.   �ô��ڵ��ֵܴ��ڣ�
**          3.   �ô��ڵ����ȴ��ڵ��ֵܴ��ڣ�
**      ���⣬���ڳ��˱�����֮�⣬�����ܱ����ȴ��ڲü���
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

    // ��ʼֵΪ�������ھ���
    pExposeTbl->count = 1;
    pExposeTbl->clientblks = 1;
    CopyRect(&pExposeTbl->rect[0], &pWin->rcWindow);

    // ���ȼ������ȴ��ڵĲü������ȴ��ڻ����ƴ��ڵı�¶����
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
    
    // �Ƿ���ȫ����
    if (IsRectEmpty(pExposeTbl->rect))
        return NULL;

    // ���㱻�ֵܴ��ں����ȴ��ڵ��ֵܸ���
    pParent = pWin;
    while (!ISROOTWIN(pParent))
    {
        // ����ü��ֵܴ��ڣ������ֵܴ��ڵĸ���
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

    // �����Ӵ��ڵĲü�
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
    // ȷ�����ڱ�¶���εĸ���С��MAX_EXPOSE_RECT
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
        // ���ڲ��ü��Ӵ���

        // Invalidates the specified window
        WND_InvalidateRect(pWin, &rect, TRUE);
        
        // invalidates the window's child windows whose window rectangle is 
        // intersected with the specified rectangle.
        pChild = pWin->pChild;
        while (pChild)
        {
            // ע��: ���ﲻ��ʹ��pRect���ж��Ӵ����Ƿ���Ҫ����, ��ΪpRect
            // �����ǵ�ǰ�������յ�ʧЧ��. ����ʧЧ���ĺϲ�����, ���ڵ�ʧ
            // Ч�����ܻ����ָ����ʧЧ��. ���Ӧ��ʹ�ô��ڽṹ�е�ʧЧ��
            // �ж��Ӵ����Ƿ���ҪʧЧ
            if (ISMAPPED(pChild) && 
                IntersectRect(&rect, &pChild->rcWindow, &pWin->rcUpdate))
                InvalidateWindowTree(pChild, &rect);
            
            pChild = pChild->pNext;
        }
    }
    else
    {
        // ���ڲü��Ӵ���

        if (pWin->pChild)
        {
            // ����������Ӵ���
            pChild = pWin->pChild;
            
            // ����֮�����δ���ÿ���Ӵ��ڵı�¶
            while (pChild != NULL)
            {
                // �����ǰ��������ʾ�Ĵ���, ����ǰ���ڵĴ��ھ��δӱ�¶
                // �����м�ȥ, �����¶�����Ϊ��, ˵����¶����Ӱ��Ĵ�
                // ���Ѿ�������, ����Ҫ������������Ĵ���, ֱ�ӷ���
                if (ISMAPPED(pChild))
                {
                    InvalidateWindowTree(pChild, &rect);
                    SubtractRect(&rect, &rect, 
                        &pChild->rcWindow);
                }
                
                // ǰһ���ֵܴ���
                pChild = pChild->pNext;
            }
        }

        // Invalidates the specified window
        WND_InvalidateRect(pWin, &rect, TRUE);
    }
}

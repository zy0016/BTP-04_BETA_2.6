/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements window map functions :
 *            WND_Map
 *            WND_Unmap
 *            
\**************************************************************************/

#include "hpwin.h"

#include "wsobj.h"
#include "wsownd.h"
#include "devinput.h"

// Internal fucntion prototypes
static void MapWindowTree(PWINOBJ pWin);
static void UnmapWindowTree(PWINOBJ pWin);
static void SendMoveSizeMessage(PWINOBJ pWin);

/*
**  Function : WND_Map
**  Purpose  :
**      Maps the specified window.
*/
void WND_Map(PWINOBJ pWin)
{
    ASSERT(pWin != NULL);
    ASSERT(pWin->dwStyle & WS_VISIBLE);
    ASSERT(!ISMAPPED(pWin));

    // Sends WM_SHOWWINDOW message to the window
    WND_SendNotifyMessage(pWin, WM_SHOWWINDOW, (WPARAM)TRUE, 1);

    // Maps the window and the child windows
    MapWindowTree(pWin);

    // If the window is really mapped, 
    if (ISMAPPED(pWin))
    {
        // Invalidates expose region table of below window.
        WND_InvalidateExposeTblBelow(pWin, &(pWin->rcWindow));
    }
}

/*
**  Function : WND_MapOwnedPopups
**  Purpose  :
**      Maps a specified window and its own windows.
*/
PWINOBJ WND_MapOwnedPopups(PWINOBJ pWin)
{
    PWINOBJ pPopup;
    PWINOBJ pTopWin = NULL;
    int nOwnWins;

    ASSERT(pWin != NULL);
    ASSERT(ISPOPUP(pWin));

    // ���û������������ָ������Ϊ��������, ֱ�ӷ���
    if (pWin->byOwnWins == 0)
        return pWin;

    // ���û������������ָ������Ϊ��������, ֱ�ӷ���

    nOwnWins = pWin->byOwnWins;
    
    // ��������ӵ�еĴ���һ��λ�ڴ���֮��, ����ֻ�������ǰ����֮�ϵ���
    // �ܴ���
    pPopup = pWin->pPrev;
    while (pPopup)
    {
        if (pPopup->pOwner == pWin)
        {
            // Maps the popup windows if the window isn't mapped
            if ((pPopup->dwStyle & WS_VISIBLE) && !ISMAPPED(pPopup))
                WND_Map(pPopup);

            // Maps the windows owned by the popup window
            pTopWin = WND_MapOwnedPopups(pPopup);
            
            // �Ѿ�û����Ҫָ������Ϊ�������ڵĴ�����, ����Ҫ����ɾ��, 
            // �˳�ѭ��
            nOwnWins--;
            if (nOwnWins == 0)
                break;
        }
        
        pPopup = pPopup->pPrev;
    }
    
    // ȷ��ɾ��ӵ�е����д���
    ASSERT(nOwnWins == 0);
    ASSERT(pTopWin != NULL);

    return pTopWin;
}

/*
**  Function : WND_Unmap
**  Purpsoe  :
**      Unmaps a specified window.
*/
void WND_Unmap(PWINOBJ pWin, BOOL bDestroy)
{
    ASSERT(pWin != NULL);
    ASSERT(ISMAPPED(pWin));

    // Can't unmap the root window
    if (ISROOTWIN(pWin))
        return;

    // Sends WM_SHOWWINDOW message to the window, 
    if (!bDestroy)
        WND_SendNotifyMessage(pWin, WM_SHOWWINDOW, (WPARAM)FALSE, 1);

    // Unmaps the window and the child windows
    UnmapWindowTree(pWin);

    // Expose entire window area
    WND_Expose(pWin->pParent, &pWin->rcWindow, pWin);

    // Notify input management module that the specified window has
    // been hided, this operation may cause the window's Z order
    // changed. 
    if (!bDestroy)
        INPUTDEV_HideWindowNotify(pWin);
}

/*
**  Function : WND_UnmapOwnedPopups
**  Purpsoe  :
**      Unmaps a specified window.
*/
void WND_UnmapOwnedPopups(PWINOBJ pWin)
{
    PWINOBJ pPopup;
    int nOwnWins;

    ASSERT(pWin != NULL);
    ASSERT(ISPOPUP(pWin));

    // ���û������������ָ������Ϊ��������, ֱ�ӷ���
    if (pWin->byOwnWins == 0)
        return;

    nOwnWins = pWin->byOwnWins;
    
    // ��������ӵ�еĴ���һ��λ�ڴ���֮��, ����ֻ�������ǰ����֮�ϵ���
    // �ܴ���
    pPopup = pWin->pPrev;
    while (pPopup)
    {
        if (pPopup->pOwner == pWin)
        {
            // Unmap the popup window if the popup window is mapped
            if (ISMAPPED(pPopup))
                WND_Unmap(pPopup, FALSE);

            // Unmaps the windows owned by the popup window
            WND_UnmapOwnedPopups(pPopup);
            
            // �Ѿ�û����Ҫָ������Ϊ�������ڵĴ�����, ����Ҫ����ɾ��, 
            // �˳�ѭ��
            nOwnWins--;
            if (nOwnWins == 0)
                break;
        }
        
        pPopup = pPopup->pPrev;
    }
    
    // ȷ��ɾ��ӵ�е����д���
    ASSERT(nOwnWins == 0);
}

// Internal function prototypes

/*
**  Function : MapWindowTree
**  Purpose  :
**      Maps a window and its child windows.
**  Remarks   :
**      ������ʾ�����غ�����������ʾ������û�й�ϵ, 
*/
static void MapWindowTree(PWINOBJ pWin)
{
    PWINOBJ pChild;
    BOOL bFromChild;
    BOOL bMapped;

    ASSERT(pWin != NULL);

    // If the window does not have WS_VISIBLE attribute, just return.
    if (!(pWin->dwStyle & WS_VISIBLE))
        return;

    // If the window's parent window is mapped, maps the window
    if (ISROOTWIN(pWin) || ISMAPPED(pWin->pParent))
    {
        // The expose table of the unmaped window is undefined
//        ASSERT(pWin->bOverlapStat == T_UNDEFINED);

        // Maps the window and invalidate the whole window rectangle
        pWin->bMapped = TRUE;
        WND_InvalidateRect(pWin, &pWin->rcWindow, TRUE);

        // Maps all child window
        pChild = pWin->pChild;
        bFromChild = FALSE;

        /* �ڴ��������Ĺ����У����������ǣ�
        ** ʹ��ǰ�������ʽ�Դ��ڵ���WND_InvalidateRect
        ** ʹ�ú��������ʽ����SendMoveSizeMessage
        */
        while (pChild != NULL)
        {
            if (pChild->dwStyle & WS_VISIBLE && !bFromChild)
            {
                /* Send WM_SHOWWINDOW to children */
                // Sends WM_SHOWWINDOW message to the window
                WND_SendMessage(pWin, WM_SHOWWINDOW, (WPARAM)TRUE, 1);
                
                // Maps the window and invalidate the whole window rectangle
                pChild->bMapped = TRUE;
                WND_InvalidateRect(pChild, &pChild->rcWindow, TRUE);
            }

            bMapped = ISMAPPED(pChild);
            /* ��ô���������һ����Ҫ����Ĵ��ڣ�ԭ�����£�
            **     * ���������Ҫ����ISMAPPED�����Ӵ��ڣ��ҵ�ǰ���ڲ�����
            **       ���Ӵ��ڻ��ݶ������������Ӵ���
            **     * ���û���Ӵ���Ҫ��������ǰ������ӳ�䣨ISMAPPED������
            **       �Ե�ǰ���ڷ���WM_SIZE��WM_MOVE��Ϣ
            **     * �����ǰ���ڴ����ֵܴ��ڣ�����һ��������Ϊ���ֵܴ��ڣ�
            **       ������bFromChildΪFALSE��ʾ��һ�������ڲ��ǻ��ݶ����ġ�
            **     * ���û���Ӵ��ں��ֵܴ���Ҫ����ȡ�����ڴ���������
            **       bFromChildΪTRUE����ʾ��һ���������ǻ��ݶ����ġ�
            */
            if (pChild->pChild && bMapped && !bFromChild)
                pChild = pChild->pChild;
            else
            {
                // û���Ӵ�����Ҫӳ�䣬�����ǰ�����Ѿ�ӳ�䣬����WM_SIZE��
                // WM_MOVE��Ϣ
                if (bMapped)
                    SendMoveSizeMessage(pChild);

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

        SendMoveSizeMessage(pWin);
    }
}

/*
**  Function : UnmapWindowTree
**  Purpose  :
**      Unmaps the specified window and its child window.
*/
static void UnmapWindowTree(PWINOBJ pWin)
{
    PWINOBJ pChild;
    BOOL bMapped;
    BOOL bFromChild;

    ASSERT(pWin != NULL);
    ASSERT(ISMAPPED(pWin));

    pWin->bMapped = FALSE;

    // Make the expose table of the window invalidate
    WND_InvalidateExposeTbl(pWin);

    // Notify window painting module that the specified window will be 
    // hided
    WND_DestroyForPaint(pWin);

/* */
    /* �ڴ��������Ĺ����У����������ǣ�
    ** ʹ��ǰ�������ʽ�Դ��ڵ���WND_InvalidateExposeTbl��
    */
    // Unmaps all child window
    pChild = pWin->pChild;
    bFromChild = FALSE;
    while (pChild != NULL)
    {
        bMapped = ISMAPPED(pChild);

        if (bMapped && !bFromChild)
        {
            pChild->bMapped = FALSE;
            
            // Make the expose table of the window invalidate
            WND_InvalidateExposeTbl(pChild);
            
            // Notify window painting module that the specified window will
            // be hided
            WND_DestroyForPaint(pChild);
        }
            
        /* ��ô���������һ����Ҫ����Ĵ��ڣ�ԭ�����£�
        **     * ���������Ҫ����ISMAPPED�����Ӵ��ڣ��ҵ�ǰ���ڲ�����
        **       ���Ӵ��ڻ��ݶ������������Ӵ���
        **     * ���û���Ӵ���Ҫ��������ǰ������ӳ�䣨ISMAPPED������
        **       �Ե�ǰ���ڷ���WM_SIZE��WM_MOVE��Ϣ
        **     * �����ǰ���ڴ����ֵܴ��ڣ�����һ��������Ϊ���ֵܴ��ڣ�
        **       ������bFromChildΪFALSE��ʾ��һ�������ڲ��ǻ��ݶ����ġ�
        **     * ���û���Ӵ��ں��ֵܴ���Ҫ����ȡ�����ڴ���������
        **       bFromChildΪTRUE����ʾ��һ���������ǻ��ݶ����ġ�
        */
        if (pChild->pChild && bMapped && !bFromChild)
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
**  Function : SendMoveSizeMessage
**  Purpose  :
**      Sends WM_MOVE and WM_SIZE message for the specified window
*/
static void SendMoveSizeMessage(PWINOBJ pWin)
{
    RECT rcClient;

    ASSERT(ISMAPPED(pWin));

    // Gets the client rect relative to parent window
    rcClient = pWin->rcClient;

    if (!ISROOTWIN(pWin))
    {
        if (ISNCCHILD(pWin))
            OffsetRect(&rcClient, -pWin->pParent->rcWindow.left, 
                -pWin->pParent->rcWindow.top);
        else
            OffsetRect(&rcClient, -pWin->pParent->rcClient.left, 
                -pWin->pParent->rcClient.top);
    }
    
    // Sends WM_SIZE message to the window
    WND_SendNotifyMessage(pWin, WM_SIZE, 0, MAKELPARAM(rcClient.right - 
        rcClient.left, rcClient.bottom - rcClient.top));
    
    // Sends WM_MOVE message to the window
    WND_SendNotifyMessage(pWin, WM_MOVE, 0, MAKELPARAM(rcClient.left, 
        rcClient.top));
}

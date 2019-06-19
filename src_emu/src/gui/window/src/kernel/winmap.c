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

    // 如果没有其它窗口以指定窗口为属主窗口, 直接返回
    if (pWin->byOwnWins == 0)
        return pWin;

    // 如果没有其它窗口以指定窗口为属主窗口, 直接返回

    nOwnWins = pWin->byOwnWins;
    
    // 属主窗口拥有的窗口一定位于窗口之上, 所以只需遍历当前窗口之上的兄
    // 弟窗口
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
            
            // 已经没有以要指定窗口为属主窗口的窗口了, 不需要继续删除, 
            // 退出循环
            nOwnWins--;
            if (nOwnWins == 0)
                break;
        }
        
        pPopup = pPopup->pPrev;
    }
    
    // 确保删除拥有的所有窗口
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

    // 如果没有其它窗口以指定窗口为属主窗口, 直接返回
    if (pWin->byOwnWins == 0)
        return;

    nOwnWins = pWin->byOwnWins;
    
    // 属主窗口拥有的窗口一定位于窗口之上, 所以只需遍历当前窗口之上的兄
    // 弟窗口
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
            
            // 已经没有以要指定窗口为属主窗口的窗口了, 不需要继续删除, 
            // 退出循环
            nOwnWins--;
            if (nOwnWins == 0)
                break;
        }
        
        pPopup = pPopup->pPrev;
    }
    
    // 确保删除拥有的所有窗口
    ASSERT(nOwnWins == 0);
}

// Internal function prototypes

/*
**  Function : MapWindowTree
**  Purpose  :
**      Maps a window and its child windows.
**  Remarks   :
**      窗口显示和隐藏和其属主的显示和隐藏没有关系, 
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

        /* 在处理窗口树的过程中，遍历方法是：
        ** 使用前序遍历方式对窗口调用WND_InvalidateRect
        ** 使用后序遍历方式调用SendMoveSizeMessage
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
            /* 获得窗口树中下一个需要处理的窗口，原则如下：
            **     * 如果存在需要处理（ISMAPPED）的子窗口，且当前窗口不是由
            **       其子窗口回溯而来，则处理其子窗口
            **     * 如果没有子窗口要处理，若当前窗口已映射（ISMAPPED），则
            **       对当前窗口发送WM_SIZE和WM_MOVE消息
            **     * 如果当前窗口存在兄弟窗口，则下一个处理窗口为其兄弟窗口，
            **       并设置bFromChild为FALSE表示下一个处理窗口不是回溯而来的。
            **     * 如果没有子窗口和兄弟窗口要处理，取父窗口处理，并设置
            **       bFromChild为TRUE，表示下一个处理窗口是回溯而来的。
            */
            if (pChild->pChild && bMapped && !bFromChild)
                pChild = pChild->pChild;
            else
            {
                // 没有子窗口需要映射，如果当前窗口已经映射，发送WM_SIZE和
                // WM_MOVE消息
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
    /* 在处理窗口树的过程中，遍历方法是：
    ** 使用前序遍历方式对窗口调用WND_InvalidateExposeTbl等
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
            
        /* 获得窗口树中下一个需要处理的窗口，原则如下：
        **     * 如果存在需要处理（ISMAPPED）的子窗口，且当前窗口不是由
        **       其子窗口回溯而来，则处理其子窗口
        **     * 如果没有子窗口要处理，若当前窗口已映射（ISMAPPED），则
        **       对当前窗口发送WM_SIZE和WM_MOVE消息
        **     * 如果当前窗口存在兄弟窗口，则下一个处理窗口为其兄弟窗口，
        **       并设置bFromChild为FALSE表示下一个处理窗口不是回溯而来的。
        **     * 如果没有子窗口和兄弟窗口要处理，取父窗口处理，并设置
        **       bFromChild为TRUE，表示下一个处理窗口是回溯而来的。
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

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implement create and destroy fucntion of window object.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"

#include "wsobj.h"

#include "wsownd.h"
#include "clstbl.h"
#include "devinput.h"
#include "wscaret.h"
#include "wstimer.h"
#include "wsgraph.h"

#include "wsthread.h"
#include "msgqueue.h"

#define WS_MASK (WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_DISABLED)

// Two internal function used to DefWindowProc to create and destroy
// data.
#define WM_DATACREATE   0x03FE
#define WM_DATADESTROY  0x03FF

// Define the minimum and maximum size of a window
#define MIN_WIN_WIDTH   0
#define MIN_WIN_HEIGHT  0
#define MAX_WIN_WIDTH   0xFFFF
#define MAX_WIN_HEIGHT  0xFFFF

// Internal function prototypes
static void AddToBrotherList(PWINOBJ pWin);
static void DestroyAWindow(PWINOBJ pWin);
static void DestroyWindowTree(PWINOBJ pWin);
static void DeleteFromBrotherList(PWINOBJ pWin);
static int GetDefWndDataSize(PWINOBJ pWin);
static void* GetUserExtraDataPtr(PWINOBJ pWin);

/*
**  Function : WND_Create
**  Purpose  :
**      Creates a device window.
*/
PWINOBJ WND_Create(DWORD dwExStyle, PCSTR pszClassName, PCSTR pszWindowName, 
                   DWORD dwStyle, int x, int y, int nWidth, int nHeight, 
                   HWND hwndParent, HMENU hMenu, HINSTANCE hInstance, 
                   PVOID pParam)
{
    CREATESTRUCT CreateData;
    PWINOBJ pWin;
    PCLSOBJ pClsObj;
    PWINOBJ pParent;
    RECT rcClient;
    PTHREADINFO pThreadInfo;

    /* 首次创建窗口时，创建窗口线程信息结构 */
    pThreadInfo = WS_GetCurrentThreadInfo();
    if (pThreadInfo == NULL)
    {
        pThreadInfo = THREADINFO_Create();
        if (pThreadInfo == NULL)
        {
            SetLastError(1);
            return NULL;
        }
    }

    // If the specified window size is invalid, return NULL
    if (nWidth < MIN_WIN_WIDTH || nHeight < MIN_WIN_HEIGHT ||
        nWidth > MAX_WIN_WIDTH || nHeight > MAX_WIN_HEIGHT)
        return NULL;

    // Gets the class object using the specified class name
    if (pszClassName)
        pClsObj = CLSTBL_GetClassObj(pszClassName, hInstance);
    else
        pClsObj = CLSTBL_GetClassObj(DEFWINDOWCLASSNAME, NULL);

    // If can't find the class object, indicates that the specified class
    // isn't registered, return NULL.
    if (!pClsObj)
        return NULL;

    // Gets the parent(owner) window object
    pParent = (PWINOBJ)WOT_GetObj((HANDLE)hwndParent, OBJ_WINDOW);

    // If the window is a child window, the parent window pointer 
    // mustn't be NULL or root window
    if ((dwStyle & WS_CHILD) && (!pParent || ISROOTWIN(pParent)))
        return NULL;
    
    // Allocates memory for window struct
    pWin = (PWINOBJ)MemAlloc(sizeof(WINOBJ) - 1 + pClsObj->wc.cbWndExtra);
    if (!pWin)
        return NULL;

    // Initializes the window struct
    memset(pWin, 0, sizeof(WINOBJ) - 1 + pClsObj->wc.cbWndExtra);

    // Adds the window object to object table
    if (!WOT_RegisterObj((PWSOBJ)pWin, OBJ_WINDOW, FALSE))
    {
        MemFree(pWin);
        return NULL;
    }

    pWin->dwThreadId = pThreadInfo->dwThreadId;

    // The top-level windows have the WS_CLIPSIBLINGS style, even if it
    // not specified
    //if (!(dwStyle & WS_CHILD))
        dwStyle |= WS_CLIPSIBLINGS;

    // Initializes the field of window struct
    pWin->pWndClass = &pClsObj->wc;
    pWin->pfnWndProc = pWin->pWndClass->lpfnWndProc;

    pWin->dwStyle = dwStyle;
    pWin->dwExStyle = dwExStyle;

    pWin->id = (DWORD)hMenu;
    pWin->hInst = hInstance;
    pWin->pParam = pParam;

    /* 创建相应的窗口客户区DC, 窗口非客户区DC将采用临时创建的方式，
    ** 对于PARENTDC，实际是子窗口拥有父窗口的裁减矩形，可以在父窗口上绘制。
    ** 而DC对象仍是从Cache中得到。
    */
    if (pClsObj->wc.style & CS_OWNDC)
    {
        pWin->pDC = DC_CreateWindowDC(pWin, TRUE);
        if (!pWin->pDC)
        {
            WOT_UnregisterObj((PWSOBJ)pWin);
            MemFree(pWin);
        
            return NULL;
        }
    }
    else if (pClsObj->wc.style & CS_CLASSDC)
    {
        pWin->pDC = pClsObj->pDC;
    }
    else
    {
        pWin->pDC = NULL;
    }

    if (dwStyle & WS_CHILD)
    {
        pWin->pParent = pParent;
        pWin->pOwner = NULL;
    }
    else
    {
        // The parent window of a popup window is the root window
        pWin->pParent = g_pRootWin;

        // Only an popup window can be an owner window, a child window can
        // not be one. If specifies a child window, assigns ownershiop to 
        // the top level parent window of the child window
        if (pParent && ISCHILD(pParent))
        {
            while (!ISROOTWIN(pParent->pParent))
                pParent = pParent->pParent;
        }

        // If the owner window is a topmost window, the new window is a 
        // topmost too. If the owner window isn't a topmost window, the new
        // can't be a topmost window
        if (pParent)
        {
            if (ISTOPMOST(pParent))
                pWin->dwStyle |= WS_TOPMOST;
            else
                pWin->dwStyle &= ~WS_TOPMOST;
        }

        pWin->pOwner = pParent;
    }

    pParent = pWin->pParent;

    // 计算窗口矩形，坐标相对于屏幕
    if (ISNCCHILD(pWin))
    {
        pWin->rcWindow.left = pParent->rcWindow.left + x;
        pWin->rcWindow.top = pParent->rcWindow.top + y;
    }
    else
    {
        pWin->rcWindow.left = pParent->rcClient.left + x;
        pWin->rcWindow.top = pParent->rcClient.top + y;
    }

    pWin->rcWindow.right = pWin->rcWindow.left + nWidth;
    pWin->rcWindow.bottom = pWin->rcWindow.top + nHeight;

    // 加入兄弟窗口链表中
    AddToBrotherList(pWin);

    // 如果窗口有属主窗口, 增加属主窗口拥有窗口计数
    if (pWin->pOwner)
    {
        // 不能拥有超过255个窗口
        ASSERT(pWin->pOwner->byOwnWins < 255);
        if (pWin->pOwner->byOwnWins < 255)
            pWin->pOwner->byOwnWins++;
    }

    //增加窗口类对象的引用计数，避免被删除。
    pClsObj->nRef ++;

    // 发送WM_DATECRATE，WM_NCCREATE，WM_NCCALCSIZE，注意，此时窗口的客户区
    // 矩形pWin->rcClient尚未计算，因此，在处理WM_NCCALCSIZE消息，不能创建
    // 或移动客户区子窗口，但可以创建或移动非客户区子窗口

    CreateData.dwExStyle        = dwExStyle;
    CreateData.lpszClass        = pszClassName;
    CreateData.lpszName         = pszWindowName;
    CreateData.style            = dwStyle;
    CreateData.x                = x;
    CreateData.y                = y;
    CreateData.cx               = nWidth;
    CreateData.cy               = nHeight;
    CreateData.hwndParent       = hwndParent;
    CreateData.hMenu            = hMenu;
    CreateData.hInstance        = hInstance;
    CreateData.lpCreateParams   = pParam;

    // Sends WM_DATACREATE to defwindowproc to create data for default
    // window

    if (!WND_SendMessage(pWin, WM_DATACREATE, 0, (LPARAM)&CreateData))
    {
        SetLastError(1);
        goto error;
    }

    // Sends WM_NCCREATE message to window proc
    if (!WND_SendMessage(pWin, WM_NCCREATE, 0, (LPARAM)&CreateData))
    {
        SetLastError(1);
        goto error;
    }

    // 发送WM_NCCALCSIZE消息，得到新的客户区矩形
	rcClient = pWin->rcWindow;
	WND_SendMessage(pWin, WM_NCCALCSIZE, (WPARAM)FALSE, (LPARAM)&rcClient);
    WND_NCCalcSize(pWin, &rcClient);

    // Sends WM_CREATE message to window proc
    if (WND_SendMessage(pWin, WM_CREATE, 0, (LPARAM)&CreateData) == -1)
        goto error;

    if ((pWin->dwStyle & WS_MASK) != (DWORD)(CreateData.style & WS_MASK))
    {
        pWin->dwStyle = (pWin->dwStyle & (unsigned long)~WS_MASK) | 
            (CreateData.style & WS_MASK);
    }

    // Shows the window if WS_VISIBLE style is specified
    if (pWin->dwStyle & WS_VISIBLE)
        WND_Show(pWin, SW_SHOW);

    return pWin;
    
error :

    pClsObj->nRef --;
    
    if (pWin)
    {
        if (pWin->pOwner)
        {
            // 不能拥有超过255个窗口
            ASSERT(pWin->pOwner->byOwnWins <= 255);
            if (pWin->pOwner->byOwnWins <= 255)
                pWin->pOwner->byOwnWins--;
        }

        if (WND_IsWindow(pWin))
        {
            if (pWin->pDC)
                DC_Destroy(pWin->pDC);

            DeleteFromBrotherList(pWin);

            WOT_UnregisterObj((PWSOBJ)pWin);
        }

        MemFree(pWin);
    }

    return NULL;
}

BOOL WND_IsWindow(PWINOBJ pWin)
{
    if (WOT_GetObjType((PWSOBJ)pWin) == OBJ_WINDOW)
        return TRUE;

    return FALSE;
}

/*
**  Function : WND_SendMessage
*/
LONG WND_SendMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam, 
                     LPARAM lParam)
{
    // Ensures the window object is a valid object
    ASSERT(pWin != NULL);
    ASSERT(WND_IsWindow(pWin));

    return MSQ_SendMessage(pWin, wMsgCmd, wParam, lParam);
}

/*
**  Function : WND_SendNotifyMessage
*/
BOOL WND_SendNotifyMessage(PWINOBJ pWin, UINT wMsgCmd, WPARAM wParam, 
                     LPARAM lParam)
{
    // Ensures the window object is a valid object
    ASSERT(pWin != NULL);
    ASSERT(WND_IsWindow(pWin));

    return MSQ_SendNotifyMessage(pWin, wMsgCmd, wParam, lParam);
}
/*
**  Function : WND_NCCalcSize
**  Purpose  :
**      Send WM_NCCALCSIZE and calculates the client rect from window 
**      position.
*/
void WND_NCCalcSize(PWINOBJ pWin, PRECT prcClient)
{
    // 如果给定客户区矩形不合理, 进行调整

    if (prcClient->left < pWin->rcWindow.left)
        prcClient->left = pWin->rcWindow.left;
    if (prcClient->right > pWin->rcWindow.right)
        prcClient->left = pWin->rcWindow.right;
    if (prcClient->top < pWin->rcWindow.top)
        prcClient->top = pWin->rcWindow.top;
    if (prcClient->bottom > pWin->rcWindow.bottom)
        prcClient->bottom = pWin->rcWindow.bottom;

    if (prcClient->right < prcClient->left)
        prcClient->right = prcClient->left;
    if (prcClient->bottom < prcClient->top)
        prcClient->bottom = prcClient->top;

    pWin->rcClient = *prcClient;
}

#if (__ST_PLX_GUI == 1)
BOOL WND_IsInSameThread(PWINOBJ pWin)
{
    return TRUE;
}

BOOL WND_IsCanBeDestroyed(DWORD dwThreadId)
{
    return TRUE;
}

BOOL WND_DestroyWindowsByThreadId(DWORD dwThreadId)
{
    return TRUE;
}

#else
BOOL WND_IsInSameThread(PWINOBJ pWin)
{
    PWINOBJ pChild, pPopup;
    DWORD dwThreadId;
    BOOL bFromChild = FALSE;
    int nOwnerWins;

    ASSERT(pWin != NULL);

    dwThreadId = pWin->dwThreadId;

    pChild = pWin->pChild;
    while (pChild != NULL)
    {
        if (pChild->dwThreadId != dwThreadId && !bFromChild)
            return FALSE;

        if (pChild->pChild != NULL && !bFromChild)
        {
            pChild = pChild->pChild;
        }
        else
        {
            if (pChild->pNext != NULL)
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
                }

                bFromChild = TRUE;
            }
        }
    }

    if (pWin->byOwnWins)
    {
        nOwnerWins = pWin->byOwnWins;
        pPopup = pWin->pPrev;
        while (pPopup)
        {
            if (pPopup->pOwner != pWin)
                pPopup = pPopup->pPrev;
            else
            {
                if (!WND_IsInSameThread(pPopup))
                    return FALSE;
                
                if (--nOwnerWins == pWin->byOwnWins)
                    break;

                pPopup = pPopup->pPrev;
            }
        }
    }

    return TRUE;
}

BOOL WND_IsCanBeDestroyed(DWORD dwThreadId)
{
    PWINOBJ pChild;
    BOOL bFromChild = FALSE;

    ASSERT(dwThreadId != g_pRootWin->dwThreadId);

    pChild = g_pRootWin->pChild;
    while (pChild != NULL)
    {
        if (pChild->dwThreadId != dwThreadId && !bFromChild)
        {
            if (pChild->pChild != NULL)
                pChild = pChild->pChild;
            else
            {
                if (pChild->pNext != NULL)
                {
                    pChild = pChild->pNext;
                    bFromChild = FALSE;
                }
                else
                {
                    pChild = pChild->pParent;
                    if (pChild == g_pRootWin)
                    {
                        pChild = NULL;
                    }

                    bFromChild = TRUE;
                }
            }

        }
        else
        {
            if (!WND_IsInSameThread(pChild) && !bFromChild)
                return FALSE;

            ASSERT(pChild->pParent->dwThreadId != dwThreadId);

            if (pChild->pNext != NULL)
            {
                pChild = pChild->pNext;
                bFromChild = FALSE;
            }
            else
            {
                pChild = pChild->pParent;
                if (pChild == g_pRootWin)
                {
                    pChild = NULL;
                }

                bFromChild = TRUE;
            }
        }
    }

    return TRUE;
}

BOOL WND_DestroyWindowsByThreadId(DWORD dwThreadId)
{
    PWINOBJ pWin, pChild;
    BOOL bFromChild = FALSE;

    if (!WND_IsCanBeDestroyed(dwThreadId))
        return FALSE;

    pChild = g_pRootWin->pChild;
    while (pChild != NULL)
    {
        if (pChild->dwThreadId == dwThreadId && !bFromChild)
        {
            if (pChild->pNext != NULL)
            {
                pWin = pChild->pNext;
                bFromChild = FALSE;
            }
            else
            {
                pWin = pChild->pParent;
                if (pWin == g_pRootWin)
                {
                    pWin = NULL;
                }
                bFromChild = TRUE;
            }

            WND_Destroy(pChild);
            
            pChild = pWin;
        }
        else 
        {
            if (pChild->pChild != NULL && !bFromChild)
                pChild = pChild->pChild;
            else if (pChild->pNext != NULL)
            {
                pChild = pChild->pNext;
                bFromChild = FALSE;
            }
            else
            {
                pChild = pChild->pParent;
                if (pChild == g_pRootWin)
                {
                    pChild = NULL;
                }
                bFromChild = TRUE;
            }
        }
    }

    return TRUE;
}
#endif

/*
**  Function : WND_Destroy
**  Purpose  :
**      Destroys a specified window.
**  Params   :
**      pWin : Specifies the window to be destroyed.
*/
void WND_Destroy(PWINOBJ pWin)
{
    PWINOBJ pPopup, pBrother;
    HWND hwndBrother = NULL;

    ASSERT(!ISROOTWIN(pWin));
    ASSERT(!pWin->bDestroying);

    // 防止删除操作重入
    if (pWin->bDestroying)
        return;

    // 设置窗口即将被删除标志
    pWin->bDestroying = TRUE;

    // 窗口处于显示状态，首先将窗口隐藏
    if (ISMAPPED(pWin))
        WND_Unmap(pWin, TRUE);

    // 如果有其它窗口以要删除窗口为属主窗口, 删除这些窗口
    if (pWin->byOwnWins)
    {
        // 属主窗口必须是popup窗口
        ASSERT(ISPOPUP(pWin));

        // 属主窗口拥有的窗口一定位于窗口之上, 所以只需遍历当前窗口之上的兄
        // 弟窗口
        pPopup = pWin->pPrev;
        while (pPopup)
        {
            if (pPopup->pOwner == pWin)
            {
                pBrother = pPopup->pPrev;
                
                // If next brother window isn't NULL, save the handle of 
                // next brother window.
                if (pBrother)
                    hwndBrother = (HWND)WOT_GetHandle((PWSOBJ)pBrother);
                
                // Destroy the popup window and its children
                WND_Destroy(pPopup);

                // 已经没有以要删除窗口为属主窗口的窗口了, 不需要继续删除, 
                // 退出循环
                if (pWin->byOwnWins == 0)
                    break;
                
                // 保存的兄弟窗口的句柄已经改变, 说明该兄弟窗口在上面调用
                // WND_Destroy函数中已经被删除, 无法继续循环下去, 重新从头
                // 开始, 否则继续
                if (pBrother && (hwndBrother != (HWND)WOT_GetHandle((PWSOBJ)pBrother)))
                    pPopup = pWin->pPrev;
                else
                    pPopup = pBrother;
            }
            else
                pPopup = pPopup->pPrev;
        }

        // 确保删除拥有的所有窗口
        ASSERT(pWin->byOwnWins == 0);
    }

    // 通知输入管理模块本窗口将要删除, 可能引起WM_ACTIVE, WM_ACTIVEAPP, 
    // WM_KILLFOCUS等消息的发送。这里不判断被删除的窗口是否显示，因为应用程
    // 序可能调用SetFocus、SetCapture等函数设置隐藏窗口，所以被删除的窗口都
    // 要调用INPUTDEV_DestroyWindowNotify
    INPUTDEV_DestroyWindowNotify(pWin);

    // 注：以前窗口删除时是在下面的DestroyWindowTree中调用
    // INPUTDEV_DestroyWindowNotify函数通知输入管理模块窗口窗口将要删除，引
    // 起焦点窗口、活动窗口的转移，这样可能会使焦点窗口、活动窗口转移到要删
    // 除的窗口上引起问题。现在改为在一次性调用INPUTDEV_DestroyWindowNotify，
    // 这样可以避免上述问题。INPUTDEV_DestroyWindowNotify函数必须考虑窗口删
    // 除时，其后代窗口会被删除的问题。

    // 确保窗口指针有效, 应用程序响应上面操作引起的消息时, 可能会再次调用
    // DestroyWindow重入,  暂时不允许这样删除窗口
    ASSERT(WND_IsWindow(pWin));

    // Delete the window and all its children
    DestroyWindowTree(pWin);
}

/*
**  Function : WND_GetParent
**  Purpsoe  :
**      Retrieves the parent window of the specified window.
*/
PWINOBJ WND_GetParent(PWINOBJ pWin)
{
    PWINOBJ pParent;

    ASSERT(pWin != NULL);

    if (ISCHILD(pWin))
        pParent = pWin->pParent;
    else
        pParent = NULL;

    return pParent;
}

/*
**  Function : WND_GetUserDataBase      winobj.c
**  Purpose  :
**      Get user data pointer of a kernel window object.
*/
void* WND_GetExtraDataPtrBase(PWINOBJ pWin)
{
    ASSERT(pWin != NULL);

    return pWin->data;
}

/*
**  Function : WND_GetExtraDataPtr          winobj.c
**  Purpose  :
**      Get user data pointer of a general window object.
*/
#if (!BASEWINDOW)

void* WND_GetExtraDataPtr(PWINOBJ pWin)
{
    ASSERT(pWin != NULL);

    return pWin->data + GetDefWndDataSize(pWin);
}
#endif

/*
**  Function : WND_GetWord
**  Purpsoe  :
**      Retrieves a 16-bit (word) value at the specified offset into the 
**      extra window memory for the specified window. 
*/
WORD WND_GetWord(PWINOBJ pWin, int nIndex)
{
    BYTE* pUserData;

    ASSERT(pWin != NULL);

    if (nIndex < 0)
    {
        SetLastError(1);
        return 0;
    }

    if ((nIndex % 2) || (nIndex > pWin->pWndClass->cbWndExtra - 2))
    {
        SetLastError(1);
        return 0;
    }

    pUserData = (BYTE*)GetUserExtraDataPtr(pWin);
    return *(WORD*)(pUserData + nIndex);
}

/*
**  Function : WND_SetWord
**  Purpose  :
**      Replaces the 16-bit (word) value at the specified offset into 
**      the extra window memory for the specified window. 
*/
WORD WND_SetWord(PWINOBJ pWin, int nIndex, WORD wNewValue)
{
    WORD wOldValue;
    BYTE* pUserData;

    ASSERT(pWin != NULL);

    if (nIndex < 0)
    {
        SetLastError(1);
        return 0;
    }

    if ((nIndex % 2) || (nIndex > pWin->pWndClass->cbWndExtra - 2))
    {
        SetLastError(1);
        return 0;
    }
    
    pUserData = (BYTE*)GetUserExtraDataPtr(pWin);

    wOldValue = *(WORD*)(pUserData + nIndex);
    *(WORD*)(pUserData + nIndex) = wNewValue;
    
    return wOldValue;
}

/*
**  Function : WND_GetLong
**  Purpsoe  :
*/
LONG WND_GetLong(PWINOBJ pWin, int nIndex)
{
    LONG lRet;
    BYTE* pUserData;

    ASSERT(pWin != NULL);

    if (nIndex >= 0)
    {
        pUserData = (BYTE*)GetUserExtraDataPtr(pWin);
        
        if ((nIndex % 4) || (nIndex > pWin->pWndClass->cbWndExtra - 4))
        {
            SetLastError(1);
            return 0;
        }

        return *(LONG*)(pUserData + nIndex);
    }

    lRet = 0;

    switch (nIndex)
    {
    case GWL_WNDPROC :

        lRet = (LONG)pWin->pfnWndProc;
        break;

    case GWL_HINSTANCE  :

        lRet = (LONG)pWin->hInst;
        break;

    case GWL_HWNDPARENT :

        if (ISCHILD(pWin))
            lRet = (LONG)WOT_GetHandle((PWSOBJ)pWin->pParent);
        else
        {
            if (pWin->pOwner)
                lRet = (LONG)WOT_GetHandle((PWSOBJ)pWin->pOwner);
            else
                lRet = 0;
        }

        break;

    case GWL_STYLE :

        lRet = (LONG)pWin->dwStyle;
        break;

    case GWL_EXSTYLE :

        lRet = (LONG)pWin->dwExStyle;
        break;

    case GWL_USERDATA :

        lRet = (LONG)pWin->pParam;
        break;

    case GWL_ID :

        lRet = (LONG)pWin->id;
        break;

    case GWL_USERDATAADDR :

        /* */
//        lRet = (LONG)pWin->data;
        lRet = (LONG)GetUserExtraDataPtr(pWin);
        break;

    default :

        SetLastError(1);
        break;
    }

    return lRet;
}

/*
**  Function : WND_SetLong
**  Purpose  :
**      Retrieves information about the specified window. 
*/
LONG WND_SetLong(PWINOBJ pWin, int nIndex, LONG lNewValue)
{
    LONG lOldValue;
    PWINOBJ pParent;
    BYTE* pUserData;

    ASSERT(pWin != NULL);

    lOldValue = 0;

    if (nIndex >= 0)
    {
        pUserData = (BYTE*)GetUserExtraDataPtr(pWin);
        if ((nIndex % 4) || (nIndex > pWin->pWndClass->cbWndExtra - 4))
        {
            SetLastError(1);
            return 0;
        }

        lOldValue = *(LONG*)(pUserData + nIndex);
        *(LONG*)(pUserData + nIndex) = lNewValue;

        return lOldValue;
    }

    switch (nIndex)
    {
    case GWL_WNDPROC :

        if (lNewValue != 0)
        {
            lOldValue = (LONG)pWin->pfnWndProc;
            pWin->pfnWndProc = (WNDPROC)lNewValue;

            // Repaint the window
            WND_InvalidateTree(pWin, &pWin->rcWindow, TRUE, FALSE);
        }

        break;

    case GWL_HINSTANCE  :

        lOldValue = (LONG)pWin->hInst;
        pWin->hInst = (HINSTANCE)lNewValue;
        break;

    case GWL_HWNDPARENT :

        // Can't change the root window's parent window
        if (ISROOTWIN(pWin))
        {
            lOldValue = 0;
            break;
        }

        pParent = (PWINOBJ)WOT_GetObj((HANDLE)lNewValue, OBJ_WINDOW);

        if (ISCHILD(pWin))  // Child Window
        {
            // Don't support swapping parent of a child window
            lOldValue = 0;
        }
        else                // Popup window
        {
            if (pWin->pOwner != pParent)
            {
                if (pWin->pOwner)
                {
                    lOldValue = (LONG)WOT_GetHandle((PWSOBJ)pWin->pOwner);
                    ASSERT(pWin->pOwner->byOwnWins > 0);
                    pWin->pOwner->byOwnWins--;
                }
                else
                    lOldValue = 0;

                if (pParent)
                {
                    ASSERT(pParent->byOwnWins < 255);
                    if (pParent->byOwnWins < 255)
                        pParent->byOwnWins++;
                }
                
                pWin->pOwner = pParent;
            }
        }

        break;

    case GWL_STYLE :

        // Only the clipping style WS_CLIPSIBLINGS and WS_CLIPCHILDREN 
        // can be changed by this function.

        lNewValue = lNewValue & (WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
        lNewValue |= (pWin->dwStyle) & (unsigned long)~(WS_CLIPSIBLINGS | WS_CLIPCHILDREN);

        // The clipping style of the window is changed
        if (lNewValue != (LONG)pWin->dwStyle)
        {
            lOldValue = (LONG)pWin->dwStyle;
            pWin->dwStyle = (DWORD)lNewValue;

            // If the window is a child window or WS_CLIPCHILDREN 
            // style is changed, invalidate the expose table of the
            // window. 
            // WS_CLIPSIBLING style is not used for a popup window. 
            // So change the WS_CLIPSIBLING style of a popup window 
            // neend't do nothing.
            if (ISCHILD(pWin) || ((lOldValue & WS_CLIPCHILDREN) != 
                (lNewValue & WS_CLIPCHILDREN)))
                WND_InvalidateExposeTbl(pWin);
        }

        break;

    case GWL_EXSTYLE :

        lOldValue = (LONG)pWin->dwExStyle;
        pWin->dwExStyle = (DWORD)lNewValue;
        break;

    case GWL_USERDATA :

        lOldValue = (LONG)pWin->pParam;
        pWin->pParam = (void*)lNewValue;
        break;

    case GWL_ID :

        if (ISCHILD(pWin))
        {
            lOldValue = (LONG)pWin->id;
            pWin->id = (DWORD)lNewValue;
        }
        else
        {
            lOldValue = (LONG)pWin->id;
            pWin->id = (DWORD)lNewValue;
        }

        break;

    default :

        SetLastError(1);
        break;
    }

    return lOldValue;
}

/*
**  Function : WND_IsEnabled
*/
BOOL WND_IsEnabled(PWINOBJ pWin)
{
    ASSERT(pWin != NULL);

    while (!ISROOTWIN(pWin))
    {
        if (pWin->dwStyle & WS_DISABLED)
            return FALSE;

        pWin = pWin->pParent;
    }

    return TRUE;
}

/*
**  Function : WND_Enable
**  Purpose  :
**      Enables or disables mouse and keyboard input to the specified 
**      window or control. When input is disabled, the window does not 
**      receive input such as mouse clicks and key presses. When input 
**      is enabled, the window receives all input. 
*/
BOOL WND_Enable(PWINOBJ pWin, BOOL bEnabled)
{
    BOOL bDisabled;

    ASSERT(pWin != NULL);

    // Can't change root window's enable state
    if (ISROOTWIN(pWin))
    {
        SetLastError(1);
        return FALSE;
    }

    if (pWin->dwStyle & WS_DISABLED)
        bDisabled = TRUE;
    else
        bDisabled = FALSE;

    if (bEnabled)
    {
        // Enable the disabled window
        if (bDisabled)
        {
            pWin->dwStyle &= ~WS_DISABLED;
            WND_SendNotifyMessage(pWin, WM_ENABLE, (WPARAM)1, 0);
        }
    }
    else
    {
        // Disable the window
        if (!bDisabled)
        {
            // First send WM_CANCELMODE message to the window to be 
            // disabled, this message enables the window to cancel modes, 
            // such as mouse capture
            WND_SendNotifyMessage(pWin, WM_CANCELMODE, 0, 0);

            pWin->dwStyle |= WS_DISABLED;
            WND_SendNotifyMessage(pWin, WM_ENABLE, (WPARAM)0, 0);
        }
    }

    return bDisabled;
}

/*
**  Function : IsWindowVisible
**  Purpose  :
**      Retrieves the visibility state of the specified window. 
**  Params   :
**      pWin : Indentifies the window object.
**  Return   :
**      If the specified window and its parent window have the WS_VISIBLE 
**      style, return nonzero. If the specified window and its parent 
**      window do not have the WS_VISIBLE style, return zero. Because the 
**      return value specifies whether the window has the WS_VISIBLE style, 
**      it may be nonzero even if the window is totally obscured by other 
**      windows. 
*/
BOOL WND_IsVisible(PWINOBJ pWin)
{
    ASSERT(pWin != NULL);

    return ISMAPPED(pWin);
}

/*
**  Function : WND_Show
**  Purpose  :
**      sets the specified window’s show state. 
**  Params   :
**      pWin     : Specifies the window.
**      nCmdShow : Specifies how the window is shown.
**  Return   :
**      If the window was previously visible, the return value is nonzero. 
**      If the window was previously hidden, the return value is zero. 
**  Remarks  :
**      nCmdShow specifies how the window is to be shown. this parameter 
**      can be one of the following values:
**      SW_HIDE            Hides the window and activates another window. 
**      SW_MAXIMIZE        Maximizes the specified window. 
**      SW_MINIMIZE        Minimizes the specified window and activates the 
**                         next top-level window in the Z order. 
**      SW_RESTORE         Activates and displays the window. If the window 
**                         is minimized or maximized,Windows restores it to
**                         its original size and position. An application 
**                         should specify this flag when restoring a 
**                         minimized window. 
**      SW_SHOW            Activates the window and displays it in its 
**                         current size and position. 
**      SW_SHOWMAXIMIZED   Activates the window and displays it as a 
**                         maximized window. 
**      SW_SHOWMINIMIZED   Activates the window and displays it as a 
**                         minimized window. 
**      SW_SHOWMINNOACTIVE Displays the window as a minimized window. The
**                         active window remains active. 
**      SW_SHOWNA          Displays the window in its current state. The 
**                         active window remains active. 
**      SW_SHOWNOACTIVATE  Displays a window in its most recent size and 
**                         position. The active window remains active. 
**      SW_SHOWNORMAL      Activates and displays a window. If the window 
**                         is minimized or maximized, Windows restores it 
**                         to its original size and position. An 
**                         application should specify this flag when 
**                         displaying the window for the first time. 
*/
BOOL WND_Show(PWINOBJ pWin, int nCmdShow)
{
    BOOL bVisible;

    ASSERT(pWin != NULL);

    bVisible = ISMAPPED(pWin);

    switch (nCmdShow)
    {
    case SW_HIDE :

        if (ISMAPPED(pWin))
            WND_Unmap(pWin, FALSE);

        // Must clear the WS_VISIBLE attribute after calling WND_Unmap
        pWin->dwStyle &= ~WS_VISIBLE;

        break;

    case SW_SHOWMAXIMIZED :

        break;

    case SW_SHOWNORMAL :
    case SW_SHOWNOACTIVATE :

        if (!ISMAPPED(pWin))
        {
            // Must set the WS_VISIBLE attribute before calling WND_Map
            pWin->dwStyle |= WS_VISIBLE;

            // If the window will be mapped, 
            if (ISMAPPED(pWin->pParent))
            {
                // 映射指定窗口
                WND_Map(pWin);

                // 如果是POPUP窗口映射窗口直接和间接拥有的窗口
                if (ISPOPUP(pWin))
                    pWin = WND_MapOwnedPopups(pWin);

                if (ISPOPUP(pWin) && nCmdShow != SW_SHOWNOACTIVATE)
                {
                    /* 
                    ** 此处不再根据WM_MOUSEACTIVATE消息返回值判断是否激活
                    */
                    INPUTDEV_SetActiveWindow(pWin, WA_ACTIVE, TRUE);

/*
                    // 设置新的活动窗口, 这里借用WM_MOUSEACTIVATE消息让窗口
                    // 函数选择是否设置为活动窗口, 这是一个不太规范的方法. 
                    nActivate = WND_SendMessage(pWin, WM_MOUSEACTIVATE, 
                        0, 0);
                    if (nActivate == MA_ACTIVATE || 
                        nActivate == MA_ACTIVATEANDEAT)
                        INPUTDEV_SetActiveWindow(pWin, WA_ACTIVE, TRUE);
*/
                }
            }
        }
        
        break;

    case SW_SHOW :
    case SW_SHOWNA :

        if (!ISMAPPED(pWin))
        {
            // Must set the WS_VISIBLE attribute before calling WND_Map
            pWin->dwStyle |= WS_VISIBLE;

            // If the window will be mapped, 
            if (ISMAPPED(pWin->pParent))
            {
                // 如果是POPUP窗口并且没有指定NA, 将窗口设置为活动窗口
                if (ISPOPUP(pWin) && nCmdShow != SW_SHOWNA)
                {
                    /* 
                    ** 此处不再根据WM_MOUSEACTIVATE消息返回值判断是否激活
                    */
                    INPUTDEV_SetActiveWindow(pWin, WA_ACTIVE, TRUE);

/*
                    // 借用WM_MOUSEACTIVATE消息让窗口函数选择是否设置为活动
                    // 窗口
                    nActivate = WND_SendMessage(pWin, WM_MOUSEACTIVATE, 
                        0, 0);
                    if (nActivate == MA_ACTIVATE || 
                        nActivate == MA_ACTIVATEANDEAT)
                        INPUTDEV_SetActiveWindow(pWin, WA_ACTIVE, TRUE);
*/
                }

                // 由于调用INPUTDEV_SetActiveWindow函数可能会导致窗口被
                // MoveToTop, 而窗口在隐藏的状态下MoveToTop效率较高, 所以
                // map操作应该在INPUTDEV_SetActiveWindow之后进行.
                WND_Map(pWin);
            }
        }

        break;

    case SW_MINIMIZE :

        // 极小化一个POPUP窗口时, 该窗口直接和间接拥有的窗口将隐藏, 当窗口
        // 恢复为Normal和Maximize状态时, 拥有的窗口自动显示.
        WND_UnmapOwnedPopups(pWin);

        // 极小化窗口暂时实现为隐藏窗口
        if (ISMAPPED(pWin))
            WND_Unmap(pWin, FALSE);

        // Must clear the WS_VISIBLE attribute after calling WND_Unmap
        pWin->dwStyle &= ~WS_VISIBLE;

        break;

    case SW_SHOWMINIMIZED :
    case SW_SHOWMINNOACTIVE :

        break;

    default :

        break;
    }

    return bVisible;
}

/*
**  Function : WND_GetRect
**  Purpose  : Retrieves the specified window part rectangle 
**             relative to spcified origin.
**  Params   :
**      pWin : Specified the window to be retrived.
**      pRect   : Points to a RECT structure that receives the 
**                coordinates of the upper-left and lower-right 
**                corners of the window part. 
**      byPart  : Specifies the window part.
**      byXYMode: Specifies the origin type.
*/
void WND_GetRect(PWINOBJ pWin, PRECT pRect, int nPart, int nXYMode)
{
    int nOrgX, nOrgY;

    ASSERT(pWin != NULL);
    ASSERT(pRect != NULL);

    // Gets the origin point coordinate
    switch (nXYMode)
    {
    case XY_SCREEN :

        nOrgX = 0;
        nOrgY = 0;

        break;

    case XY_WINDOW :

        nOrgX = pWin->rcWindow.left;
        nOrgY = pWin->rcWindow.top;

        break;

    default :       // XY_CLIENT

        nOrgX = pWin->rcClient.left;
        nOrgY = pWin->rcClient.top;
        
        break;
    }

    // Gets the rect relative to screen
    switch (nPart)
    {
    case W_CLIENT :
        
        *pRect = pWin->rcClient;
        break;

    case W_TOPNC :

        pRect->left = pWin->rcWindow.left;
        pRect->top = pWin->rcWindow.top;
        pRect->right = pWin->rcWindow.right;
        pRect->bottom = pWin->rcClient.top;

        break;

    case W_BOTTOMNC :

        pRect->left = pWin->rcWindow.left;
        pRect->top = pWin->rcClient.bottom;
        pRect->right = pWin->rcWindow.right;
        pRect->bottom = pWin->rcWindow.bottom;

        break;

    case W_LEFTNC :

        pRect->left = pWin->rcWindow.left;
        pRect->top = pWin->rcClient.top;
        pRect->right = pWin->rcClient.left;
        pRect->bottom = pWin->rcClient.bottom;

        break;

    case W_RIGHTNC :

        pRect->left = pWin->rcClient.right;
        pRect->top = pWin->rcClient.top;
        pRect->right = pWin->rcWindow.right;
        pRect->bottom = pWin->rcClient.bottom;

        break;

    default :   // W_WINDOW

        *pRect = pWin->rcWindow;
        break;
    }

    // Calculates the real rect coordinate
    pRect->left -= nOrgX;
    pRect->top -= nOrgY;
    pRect->right -= nOrgX;
    pRect->bottom -= nOrgY;
}

/*
**  Funciton : WND_FindWindow
*/
PWINOBJ WND_FindWindow(PCSTR pszClassName, PCSTR pszWindowName)
{
    PWINOBJ pWin;
    PSTR pBuffer;

    ASSERT(pszClassName != NULL);

    pWin = g_pRootWin->pChild;
    while (pWin != NULL)
    {
        if (!stricmp(pszClassName, pWin->pWndClass->lpszClassName))
        {
            if (!pszWindowName)
                return pWin;

            pBuffer = MemAlloc(strlen(pszWindowName) + 1);
            WND_SendMessage(pWin, WM_GETTEXT, strlen(pszWindowName) + 1, 
                (LPARAM)pBuffer);
            if (!strcmp(pszWindowName, pBuffer))
            {
                MemFree(pBuffer);
                return pWin;
            }
            
            MemFree(pBuffer);
        }

        pWin = pWin->pNext;
    }

    return NULL;
}

int WND_MakeHwndList(HWND *pHwnd, int nCount)
{
    PWINOBJ pWin;
    int nHwnd, i;
    BOOL bMake;

    bMake = (nCount != 0 && pHwnd != NULL);
    pWin = g_pRootWin->pChild;
    nHwnd = 0;
    i = 0;

    while (pWin != NULL)
    {
        nHwnd++;
        if (bMake)
        {
            if (i < nCount)
            {
                i++;
                *pHwnd++ = WOT_GetHandle((PWSOBJ)pWin);
            }
            else
                return i;
        }

        pWin = pWin->pNext;
    }

    if (bMake)
        return i;

    return nHwnd;
}

/*
**  Function : pEnumFunc
**  Purpose  :
**      Enumerates all top-level windows on the screen by passing the 
**      handle of each window, in turn, to an application-defined callback
**      function. EnumWindows continues until the last top-level window is
**      enumerated or the callback function returns FALSE. 
*/
void WND_EnumWindows(WNDENUMPROC pEnumFunc, LPARAM lParam)
{
    PWINOBJ pWin, pPreWin = NULL;
    BOOL bRet;

    ASSERT(pEnumFunc != NULL);

    pWin = g_pRootWin->pChild;

    /* 
    ** If the application destroy the window, the pWin is 
    ** no longer available. So we should record the prvoious
    ** processed window.
    */
    while (pWin)
    {
        LEAVEMONITOR;

        bRet = pEnumFunc((HWND)WOT_GetHandle((PWSOBJ)pWin), lParam);
        
        ENTERMONITOR;

        if (!bRet)
            return;

        if (WND_IsWindow(pWin))
        {
            pPreWin = pWin;
            pWin = pWin->pNext;
            break;
        }
        else
        {
            pWin = g_pRootWin->pChild;
        }
    }

    while (pWin)
    {
        LEAVEMONITOR;

        bRet = pEnumFunc((HWND)WOT_GetHandle((PWSOBJ)pWin), lParam);
        
        ENTERMONITOR;

        if (!bRet)
            return;

        if (WND_IsWindow(pWin))
        {
            pPreWin = pWin;
            pWin = pWin->pNext;
        }
        else if (WND_IsWindow(pPreWin))
        {
            /* The previous processed window is available */
            pWin = pPreWin->pNext;
        }
        else
        {
            /* Can't find the next window unless we enum all window 
            ** again. return direct.
            */
            return;
        }
    }
}

void WND_EnumChildWindows(PWINOBJ pParent, WNDENUMPROC pEnumFunc, 
                          LPARAM lParam)
{
}

/*
**  Function : WND_GetWindow
**  Purpose  :
**      Retrieves the  window that has the specified relationship (Z order
**      or owner) to the specified window. 
*/
PWINOBJ WND_GetWindow(PWINOBJ pWin, UINT uCmd)
{
    ASSERT(pWin != NULL);
    ASSERT(uCmd <= GW_MAX);

    switch (uCmd)
    {
    case GW_HWNDFIRST :

        if (!pWin->pParent)
            return pWin;

        return pWin->pParent->pChild;
        
    case GW_HWNDLAST :
        
        while (pWin->pNext != NULL)
            pWin = pWin->pNext;
        
        return pWin;
        
    case GW_HWNDNEXT :
        
        return pWin->pNext;
        
    case GW_HWNDPREV :

        return pWin->pPrev;
        
    case GW_OWNER :
        
        return pWin->pOwner;
        
    case GW_CHILD :

        return pWin->pChild;
        
    case GW_LASTCHILD :

        if (!pWin->pChild)
            return NULL;

        pWin = pWin->pChild;
        while (pWin->pNext)
            pWin = pWin->pNext;

        if (ISNCCHILD(pWin))
            return NULL;

        return pWin;
    }

    return NULL;
}

/*
**  Function : WND_IsDescendant
**  Purpose  : 
**      如果pWin是pForefatherWin的后代窗口，返回TRUE，否则返回FALSE。
*/
BOOL WND_IsDescendant(PWINOBJ pWin, PWINOBJ pForefatherWin)
{
    PWINOBJ pParent;

    if (!pWin || !pForefatherWin)
        return FALSE;

    if (pWin == pForefatherWin)
        return TRUE;

    pParent = pWin->pParent;
    while (pParent)
    {
        if (pParent == pForefatherWin)
            return TRUE;

        pParent = pParent->pParent;
    }

    return FALSE;
}

/*
**  Function : WND_IsOwned
**  Purpose  : 
**      如果pWin是pOwnerWin直接拥有或间接拥有的popup窗口，返回TRUE，否则返
**      回FALSE。
*/
BOOL WND_IsOwned(PWINOBJ pWin, PWINOBJ pOwnerWin)
{
    PWINOBJ pOwner;

    ASSERT(pOwnerWin != NULL);

    if (!pWin || !ISPOPUP(pOwnerWin) || !pOwnerWin->byOwnWins)
        return FALSE;

    if (pWin == pOwnerWin)
        return FALSE;

    pOwner = pWin->pOwner;
    while (pOwner)
    {
        if (pOwner == pOwnerWin)
            return TRUE;

        pOwner= pOwner->pOwner;
    }

    return FALSE;
}

/*
**  Function : WND_IsDestroying
**  Purpose  :
**      如果一个窗口即将被删除，返回TRUE，否则返回FALSE。
*/
BOOL WND_IsDestroying(PWINOBJ pWin)
{
    if (!pWin || ISROOTWIN(pWin))
        return FALSE;

    /* 如果pWin的父窗口是根窗口时，此处算法错误。改正之 */
    if (ISDESTROYING(pWin))
        return TRUE;

    // 判断祖先窗口是否正在被删除，祖先窗口正在被删除，说明指定窗口即将被
    // 删除
    while (!ISROOTWIN(pWin->pParent))
    {
        pWin = pWin->pParent;
        ASSERT(pWin != NULL); //只有根窗口父窗口为空

        if (ISDESTROYING(pWin))
            return TRUE;
    }

    return FALSE;
}

// Internal function prototypes
/*
**  Function : GetDefWndSize
**  Purpose  :
**      得到DefWnd需要的私有数据大小
*/
static int GetDefWndDataSize(PWINOBJ pWin)
{
    PCLSOBJ pClsDefWnd;

#if (PDAAPI)

    /* PDA平台有自己的DefWindow。而控件使用缺省的DefWindow，具有CS_DEFWIN */
    if (pWin->pWndClass->style & CS_DEFWIN)
        pClsDefWnd = CLSTBL_GetClassObj(DEFWINDOWCLASSNAME, NULL);
    else
        pClsDefWnd = CLSTBL_GetClassObj(PDADEFWINDOWCLASSNAME, NULL);
#else
    pClsDefWnd = CLSTBL_GetClassObj(DEFWINDOWCLASSNAME, NULL);
#endif

    if (pClsDefWnd == NULL)
        return 0;

    return pClsDefWnd->wc.cbWndExtra;
}

/*
**  Function : AddToBrotherList
**  Purpose  :
**      将新窗口加入兄弟窗口链表的的表头。
*/
static void AddToBrotherList(PWINOBJ pWin)
{
    PWINOBJ pTemp, pParent, pNcTemp;

    ASSERT(pWin != NULL);
    ASSERT(pWin->pParent != NULL);

    // 将新窗口加入兄弟窗口链表中，对于popup窗口和非客户区子窗口，使窗口在
    // z-order上处于顶端；对于客户区子窗口，使窗口z-order上处于底端。这里
    // 的处理与Microsoft Window一样，这样可以使窗口的创建顺序与窗口的重画
    // 顺序一致。

    // 如果popup窗口有属主窗口，由于popup窗口本身要加入到z-order的顶端，首
    // 先要将属主窗口移到z-order的顶端，因为窗口和属主窗口在z-order上应该
    // 相邻，中间不能有其它popup窗口
    if (pWin->pOwner)
        WND_MoveToTop(pWin->pOwner);

    pParent = pWin->pParent;
    pTemp = pParent->pChild;

    if (!ISCHILD(pWin))  // popup window
    {
        if (!pTemp || !ISTOPMOST(pTemp) || ISTOPMOST(pWin))
        {
            pWin->pNext = pParent->pChild;
            pWin->pPrev = NULL;
            if (pTemp)
                pTemp->pPrev = pWin;
            pParent->pChild = pWin;
        }
        else
        {
            // pTemp && ISTOPMOST(pTemp) && !ISTOPMOST(pWin)
            
            while ((pTemp->pNext != NULL) && ISTOPMOST(pTemp->pNext))
                pTemp = pTemp->pNext;
            
            // 加在pTemp后边
            pWin->pNext = pTemp->pNext;
            pWin->pPrev = pTemp;
            if (pTemp->pNext)
                pTemp->pNext->pPrev = pWin;
            pTemp->pNext = pWin;
        }
    }
    else    // child window
    {
        if (ISNCCHILD(pWin))
        {
            // Add to head of the brother list
            pWin->pNext = pParent->pChild;
            pWin->pPrev = NULL;
            if (pTemp)
                pTemp->pPrev = pWin;
            pParent->pChild = pWin;
        }
        else if (pTemp)
        {
            pNcTemp = pTemp;
            while(pTemp && ISNCCHILD(pTemp))
            {
                pNcTemp = pTemp;
                pTemp = pTemp->pNext;
            }
            pWin->pNext = pTemp;
            if (pTemp)
                pTemp->pPrev = pWin;
            if (pNcTemp == pTemp)
            {
                pWin->pPrev = NULL;
                pParent->pChild = pWin;
            }
            else
            {
                pWin->pPrev = pNcTemp;
                pNcTemp->pNext = pWin;
            }
        }
        else
        {
            pWin->pNext = NULL;
            pWin->pPrev = pTemp;
            pParent->pChild = pWin;
        }
    }
}

/*
**  Function : DestroyWindowTree
**  Purpose  :
**      Destroys a spcified window and its children window.
*/
static void DestroyWindowTree(PWINOBJ pWin)
{
    PWINOBJ pChild, pParent;
    BOOL bFromChild;    // 表示是否从子窗口返回

    // 确保窗口指针有效
    ASSERT(pWin && WND_IsWindow(pWin));

    // 删除子窗口之前发送WM_DESTROY消息
    WND_SendMessage(pWin, WM_DESTROY, (WPARAM)0, (LPARAM)0);

    // 删除窗口的所有后代窗口
    bFromChild = FALSE;
    pChild = pWin->pChild;
    while (pChild)
    {
        // 当前窗口之上的窗口已经删除，因此不存在当前窗口之上的窗口
        ASSERT(!pChild->pPrev);

        // 删除子窗口之前发送WM_DESTROY消息
        if (!bFromChild)
            WND_SendMessage(pChild, WM_DESTROY, (WPARAM)0, (LPARAM)0);

        // 如果有子窗口，需要先处理子窗口的删除
        if (pChild->pChild)
        {
            ASSERT(!bFromChild);
            pChild = pChild->pChild;
        }
        else 
        {
            // 没有子窗口，当前窗口可以删除了，保存其父窗口以继续后边的删除
            pParent = pChild->pParent;

            // 删除当前窗口
            DestroyAWindow(pChild);

            // 确保父窗口有效，如果在上面的删除窗口的过程中应用程序删除父窗
            // 口导致删除重入，会引起pParent无效，这里不处理这种特殊情况，
            // 只用ASSERT给出警告
            ASSERT(!pParent || WND_IsWindow(pParent));

            // 确定下一个要处理的窗口，如果父窗口依然有子窗口，选择父窗口的
            // 第一个子窗口作为要处理的窗口，否则，说明父窗口的子窗口删除完
            // 毕，选择父窗口作为要处理的窗口，如果已经是最初的窗口了，退出
            // 循环
            if (pParent->pChild)
            {
                pChild = pParent->pChild;
                bFromChild = FALSE;
            }
            else
            {
                if (pParent != pWin)
                {
                    pChild = pParent;
                    bFromChild = TRUE;
                }
                else
                    pChild = NULL;
            }
        }
    }

    // 删除窗口自己
    DestroyAWindow(pWin);
}

/*
**  Function : DestroyAWindow
**  Purpose  :
**      Destroy a specified window.
*/
static void DestroyAWindow(PWINOBJ pWin)
{
    PCLSOBJ pClsObj;
    // 确保窗口有效并且处于隐藏状态
    ASSERT(pWin && WND_IsWindow(pWin));
    ASSERT(!ISMAPPED(pWin));

    ASSERT(pWin->dwThreadId == WS_GetCurrentThreadId());
    // Kill all timer of this window
    WS_KillWindowTimer(pWin);

    // Destroy the current caret if the window own the current caret
    WS_DestroyWindowCaret(pWin);

    // Sends WM_NCDESTROY message to window proc
    WND_SendMessage(pWin, WM_NCDESTROY, (WPARAM)0, (LPARAM)0);

    // Sends WM_DATADESTROY to DefWindowProc to let DefWindowProc destroy
    // data.
    WND_SendMessage(pWin, WM_DATADESTROY, (WPARAM)0, (LPARAM)0);

    // 如果有属主窗口, 减少其属主窗口拥有的窗口计数
    if (pWin->pOwner)
    {
        ASSERT(pWin->pOwner->byOwnWins > 0);
        pWin->pOwner->byOwnWins--;
    }

    // Deletes the window from its brother list
    DeleteFromBrotherList(pWin);

    MSQ_DeleteSendMsg(pWin);
    MSQ_DeletePostMsg(pWin);
    MSQ_DeletePaintMsg(pWin);

    // Removes the window from window object table
    WOT_UnregisterObj((PWSOBJ)pWin);

    // Free DC for window
/* */
    if (pWin->pDC)
    {
        if (pWin->pWndClass->style & CS_OWNDC)
            DC_Destroy(pWin->pDC);
        else if (!(pWin->pWndClass->style & CS_CLASSDC))
            DC_ReleaseWindowDCToCache(pWin->pDC);
    }

    // Release DC from DC cache
    DC_DestroyWindow(pWin);

    if (pWin->pWndClass->style & CS_GLOBALCLASS)
    {
        pClsObj = CLSTBL_GetClassObj(pWin->pWndClass->lpszClassName, NULL);
        pClsObj->nRef --;
        if (pClsObj->nRef == 0 && pClsObj->bDeleted == TRUE)
            CLSTBL_Delete(pWin->pWndClass->lpszClassName, NULL);
    }
    else
    {
        pClsObj = CLSTBL_GetClassObj(pWin->pWndClass->lpszClassName, 
            pWin->pWndClass->hInstance);

        pClsObj->nRef --;
        if (pClsObj->nRef == 0 && pClsObj->bDeleted == TRUE)
            CLSTBL_Delete(pWin->pWndClass->lpszClassName, 
                pWin->pWndClass->hInstance);
    }

    // Free the window structure 
    MemFree(pWin);
}

/*
**  Function : DeleteFromBrotherList
**  Purpose  :
**      Deletes a specified window from its brother list
**  Params   :
**      pWin : Spcifies window to be deleted from its brother list.
*/
static void DeleteFromBrotherList(PWINOBJ pWin)
{
    ASSERT(pWin != NULL);
    ASSERT(pWin->pParent != NULL);

    if (!pWin->pPrev)
    {
        pWin->pParent->pChild = pWin->pNext;
        if (pWin->pNext!= NULL)
            pWin->pNext->pPrev = NULL;
    }
    else 
    {
        pWin->pPrev->pNext = pWin->pNext;
        if (pWin->pNext != NULL)
            pWin->pNext->pPrev = pWin->pPrev;
    }
}

static void* GetUserExtraDataPtr(PWINOBJ pWin)
{
#if (!BASEWINDOW)
    return WND_GetExtraDataPtr(pWin);
#else
    return WND_GetExtraDataPtrBase(pWin);
#endif
}


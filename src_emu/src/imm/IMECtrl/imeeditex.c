/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : imeeditex.c
 *
 * Purpose  : IMEEDITEX Class Implementation
 *            
\**************************************************************************/

#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "malloc.h"
#include "string.h"
#include "str_public.h"
#include "str_plx.h"

#include "imesys.h"
#include "imm.h"
#include "imc.h"

#define IDM_SAVE 1001
#define IDM_EXIT 1002

#define WND_CAPTIONSPACE     14

#define HP_WIN_WIDTH         176
#define HP_WIN_CLIENT_HEIGHT (PLX_WIN_HIGH - GetSystemMetrics(SM_CYCAPTION) \
                              - GetSystemMetrics(SM_CYMENU))
                           // CY_MAINWND - CY_TITLE - CY_MENU - CY_CANDIDATES

typedef struct tagEDITEXWNDLONG
{
    HWND   hwndNotify;
    HWND   hwndInnerEdit;
}
EDITEXWNDLONG, *PEDITEXWNDLONG;

static LRESULT CALLBACK ImeEditExWndProc(HWND hWnd, UINT uMsg,
                                  WPARAM wParam, LPARAM lParam);

static void OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnActivate(HWND hWnd, WPARAM wParam);
static BOOL OnKeyDown(HWND hWnd, WPARAM wParam);
static void OnDestroy(HWND hWnd);

/*********************************************************************\
* Function     RegisterImeEditExClass
* Purpose      
* Params       hInst
* Return           
* Remarks      
**********************************************************************/

BOOL RegisterImeEditExClass(HINSTANCE hInst)
{
    WNDCLASS wcImeEditEx;

    wcImeEditEx.style         = 0;
    wcImeEditEx.lpfnWndProc   = ImeEditExWndProc;
    wcImeEditEx.cbClsExtra    = 0;
    wcImeEditEx.cbWndExtra    = sizeof(PEDITEXWNDLONG);
    wcImeEditEx.hInstance     = hInst;
    wcImeEditEx.hIcon         = NULL;
    wcImeEditEx.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wcImeEditEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcImeEditEx.lpszMenuName  = NULL;
    wcImeEditEx.lpszClassName = "IMEEDITEX";
    
    return RegisterClass(&wcImeEditEx);
}

/*********************************************************************\
* Function     ImeEditExWndProc
* Purpose      
* Params       hWnd
*              uMsg
*              wParam
*              lParam
* Return           
* Remarks      
**********************************************************************/

static LRESULT CALLBACK ImeEditExWndProc(HWND hWnd, UINT uMsg,
                                  WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate(hWnd, wParam, lParam);
        return 0;

    case WM_COMMAND:
        OnCommand(hWnd, wParam, lParam);
        return 0;

    case WM_ACTIVATE:
        OnActivate(hWnd, wParam);
        return 0;

    case WM_KEYDOWN:
        if (OnKeyDown(hWnd, wParam))
        {
            return 0;
        }
        else
        {
            return PDADefWindowProc(hWnd, uMsg, wParam, lParam);
        }

    case WM_DESTROY:
        OnDestroy(hWnd);
        return 0;

//    case WM_TODESKTOP:
//        PostMessage(hWnd, WM_CLOSE, 0, 0);
//        return (LRESULT)TRUE;

    default:
        return PDADefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

/*********************************************************************\
* Function     OnCreate
* Purpose      
* Params       hWnd, wParam, lParam
* Return           
* Remarks      
**********************************************************************/

static void OnCreate(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PEDITEXWNDLONG pEditExLong = NULL;
    PIMEEDIT pUserData = NULL;
    int nLength = 0;
    PSTR pszText = NULL;
    DWORD dwCaretPos = 0, dwLimitText = 0;

    pUserData = (PIMEEDIT)(((LPCREATESTRUCT)lParam)->lpCreateParams);
    if (pUserData == NULL || pUserData->hwndNotify == NULL)
    {
        return;
    }
    pEditExLong = (PEDITEXWNDLONG)malloc(sizeof(EDITEXWNDLONG));
    memset(pEditExLong, 0, sizeof(EDITEXWNDLONG));

    nLength = GetWindowTextLength(pUserData->hwndNotify);
    pszText = (PSTR)malloc(nLength + 1);
    if (pszText == NULL)
    {
        return;
    }
    GetWindowText(pUserData->hwndNotify, pszText, nLength + 1);

    pEditExLong->hwndNotify = pUserData->hwndNotify;
    pUserData->hwndNotify = hWnd;
    pUserData->dwAttrib  &= ~IME_ATTRIB_RECEIVER;

    pEditExLong->hwndInnerEdit = CreateWindow(
        "IMEEDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL
        | ES_MULTILINE | ES_UNDERLINE | ES_AUTOVSCROLL,
        0,
        0,
        HP_WIN_WIDTH,
        HP_WIN_CLIENT_HEIGHT,
        hWnd,
        NULL,
        NULL,
        (PVOID)pUserData
        );

    dwLimitText = SendMessage(pEditExLong->hwndNotify, EM_GETLIMITTEXT,
        0, 0);
    SendMessage(pEditExLong->hwndInnerEdit, EM_SETLIMITTEXT,
        (WPARAM)dwLimitText, 0);

    SetWindowText(pEditExLong->hwndInnerEdit, pszText);

    if (pszText != NULL)
    {
        free(pszText);
    }

    if (pEditExLong->hwndInnerEdit == NULL)
    {
        free(pEditExLong);
    }

    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON,
        MAKEWPARAM(IDM_EXIT, 0),
        (LPARAM)ML("Cancel"));
    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON,
        MAKEWPARAM(IDM_SAVE, 1),
        (LPARAM)ML("Save"));

    SendMessage(hWnd, PWM_SETAPPICON, 0,
        (LPARAM)"ROM:/ime_mn.ico");
    
    dwCaretPos = SendMessage(pEditExLong->hwndNotify, EM_GETSEL,
        NULL, NULL);
    SendMessage(pEditExLong->hwndInnerEdit, EM_SETSEL, 
        (WPARAM)LOWORD(dwCaretPos), (LPARAM)HIWORD(dwCaretPos));

    SetWindowLong(hWnd, 0, (LONG)pEditExLong);
}

/*********************************************************************\
* Function     OnCommand
* Purpose      
* Params       hWnd, wParam, lParam
* Return           
* Remarks      
**********************************************************************/

static void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PEDITEXWNDLONG pEditExLong = NULL;
    PSTR pszText = NULL;
    int nTextLen = 0;
    DWORD dwCaretPos = 0;

    pEditExLong = (PEDITEXWNDLONG)GetWindowLong(hWnd, 0);

    if (pEditExLong == NULL)
    {
        return;
    }
    
    switch (LOWORD(wParam))
    {
    case IDM_SAVE:
        // 导出文本
        nTextLen = GetWindowTextLength(pEditExLong->hwndInnerEdit);
        pszText = (PSTR)malloc(nTextLen + 1);
        if (pszText == NULL)
        {
            return;
        }
        GetWindowText(pEditExLong->hwndInnerEdit, pszText,
            nTextLen + 1);
//        SetWindowText(pEditExLong->hwndNotify, pszText);
		SendMessage(pEditExLong->hwndNotify, WM_SETTEXT, 1, (LPARAM)pszText);
        free(pszText);

        // 设置光标
        dwCaretPos = SendMessage(pEditExLong->hwndInnerEdit,
            EM_GETSEL, NULL, NULL);
        SendMessage(pEditExLong->hwndNotify, EM_SETSEL,
            (WPARAM)LOWORD(dwCaretPos), (LPARAM)HIWORD(dwCaretPos));
        // 关闭窗口
        DestroyWindow(hWnd);
        break;

    case IDM_EXIT:
        nTextLen = GetWindowTextLength(pEditExLong->hwndInnerEdit);
        pszText = (PSTR)malloc(nTextLen + 1);
        if (pszText == NULL)
        {
            return;
        }
        GetWindowText(pEditExLong->hwndInnerEdit, pszText,
            nTextLen + 1);
		SendMessage(pEditExLong->hwndNotify, WM_SETTEXT, 0, (LPARAM)pszText);
//        SetWindowText(pEditExLong->hwndNotify, pszText);
        free(pszText);

        DestroyWindow(hWnd);
        break;

    default:
        break;
    }

}

/*********************************************************************\
* Function     OnActivate
* Purpose      
* Params       hWnd, wParam
* Return           
* Remarks      
**********************************************************************/

static void OnActivate(HWND hWnd, WPARAM wParam)
{
    PEDITEXWNDLONG pEditExLong = NULL;

    if ((int)wParam == WA_ACTIVE)
    {
        pEditExLong = (PEDITEXWNDLONG)GetWindowLong(hWnd, 0);
        
        if (pEditExLong == NULL)
        {
            return;
        }
        
        SetFocus(pEditExLong->hwndInnerEdit);
    }
}

/**********************************************************************
* Function     OnKeyDown
* Purpose      
* Params       hWnd, wParam
* Return       
* Remarks      
**********************************************************************/

static BOOL OnKeyDown(HWND hWnd, WPARAM wParam)
{
    BOOL bRet = FALSE;

    switch ((int)wParam)
    {
    case VK_RETURN:
        SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_SAVE, 0), 0);
        bRet = TRUE;
        break;

    case VK_F10:
        SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDM_EXIT, 0), 0);
        bRet = TRUE;
        break;

//    case VK_F2:
//        SendMessage(hWnd, WM_CLOSE, 0, 0);
//        bRet = TRUE;
//        break;
        
    default:
        break;
    }

    return bRet;
}

/*********************************************************************\
* Function     OnDestroy
* Purpose      
* Params       hWnd
* Return           
* Remarks      
**********************************************************************/

static void OnDestroy(HWND hWnd)
{
    PEDITEXWNDLONG pEditExLong = NULL;

    pEditExLong = (PEDITEXWNDLONG)GetWindowLong(hWnd, 0);

    if (pEditExLong != NULL)
    {
        free(pEditExLong);
        pEditExLong = NULL;
    }
}

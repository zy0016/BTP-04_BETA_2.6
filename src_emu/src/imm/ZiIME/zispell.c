/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  zispell.c
 *
 * Purpose  :  Zi spell UI for teach mode.
 *
\**************************************************************************/

#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "stdlib.h"
#include "string.h"

#include "imesys.h"
#include "imm.h"

#define ID_SPELLEDIT   1004

#define MAX_LEN_CAPTION  64
#define MAX_LEN_SOFTKEY  32
#define MAX_LEN_EDIT     32

#define CX_MARGIN         1
#define CY_MARGIN         3

#define RES_STR_TEACH    ML("Teach")
#define RES_STR_OK       ML("Ok")
#define RES_STR_CANCEL   ML("Cancel")
#define RES_STR_NEWWORD  ML("New word:")

typedef struct tagZISPELL
{
    HWND  hwndParent;
    HWND  hwndSpell;
    HWND  hwndTarget;
    char  szSpell[MAX_LEN_EDIT];
    char  szCaption[MAX_LEN_CAPTION];
    char  szLSoftKey[MAX_LEN_SOFTKEY];
    char  szRSoftKey[MAX_LEN_SOFTKEY];
    char  szMenuKey[MAX_LEN_SOFTKEY];
}
ZISPELL, *PZISPELL;

static LRESULT CALLBACK ZiSpellWndProc(HWND hWnd, UINT uMsg, WPARAM wParam,
                                       LPARAM lParam);
static void OnCreate(HWND hWnd, LPCREATESTRUCT lpcs);
static void OnShowWindow(HWND hWnd, BOOL bShow);
static void OnKeyDown(HWND hWnd, int nKeyCode, DWORD dwKeyData);
static void OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText);
static void OnDestroy(HWND hWnd);
static void OnKeyDownReturn(HWND hWnd);
static void OnKeyDownF10(HWND hWnd);

/**********************************************************************
 * Function     ImeZiSpellWinMain
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

int ImeZiSpellWinMain(char *pszSpell)
{
    HWND      hwndZiSpell = NULL;
    RECT      rc;
    PZISPELL  pUserData = NULL;

    pUserData = (PZISPELL)malloc(sizeof(ZISPELL));
    memset((void*)pUserData, 0, sizeof(ZISPELL));

    pUserData->hwndParent = GetFocus();
    while (GetParent(pUserData->hwndParent) != NULL)
    {
        pUserData->hwndParent = GetParent(pUserData->hwndParent);
    }

    strncpy(pUserData->szSpell, pszSpell, MAX_LEN_EDIT - 1);
    pUserData->szSpell[MAX_LEN_EDIT - 1] = '\0';

    GetClientRect(pUserData->hwndParent, &rc);

    hwndZiSpell = CreateWindow(
        "IMEZISPELL",
        "",
        WS_VISIBLE | WS_CHILD,
        rc.left,
        rc.top,
        rc.right - rc.left,
        rc.bottom - rc.top,
        pUserData->hwndParent,
        NULL,
        NULL,
        (LPVOID)pUserData
        );

    SendMessage(pUserData->hwndSpell, EM_SETSEL, -1, -1);

    return 0;
}

/**********************************************************************
 * Function     RegisterZiSpellClass
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

BOOL RegisterImeZiSpellClass(HINSTANCE hInst)
{
    WNDCLASS wc;

    wc.style         = 0;
    wc.lpfnWndProc   = ZiSpellWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInst;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "IMEZISPELL";

    return RegisterClass(&wc);
}

/**********************************************************************
 * Function     ZiSpellWndProc
 * Purpose      
 * Params       hWnd, uMsg, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT CALLBACK ZiSpellWndProc(HWND hWnd, UINT uMsg,
                                       WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        OnCreate(hWnd, (LPCREATESTRUCT)lParam);
        return 0;

    case WM_SHOWWINDOW:
        OnShowWindow(hWnd, (BOOL)wParam);
        return 0;

    case WM_KEYDOWN:
        OnKeyDown(hWnd, (int)wParam, (DWORD)lParam);
        return 0;

    case WM_SETLBTNTEXT:
        OnSetLBtnText(hWnd, (int)LOWORD(wParam), (BOOL)HIWORD(wParam),
            (LPSTR)lParam);
        return 0;

    case WM_DESTROY:
        OnDestroy(hWnd);
        return 0;

    default:
        return PDADefWindowProc(hWnd, uMsg, wParam, lParam);
    }
}

/**********************************************************************
 * Function     OnCreate
 * Purpose      
 * Params       hWnd, lpcs
 * Return       
 * Remarks      
 **********************************************************************/

static void OnCreate(HWND hWnd, LPCREATESTRUCT lpcs)
{
    PZISPELL  pUserData = NULL;
    IMEEDIT   ie;
    RECT      rc;
    int       iMax = 0;

    pUserData = (PZISPELL)(lpcs->lpCreateParams);

    GetClientRect(hWnd, &rc);

    memset((void*)&ie, 0, sizeof(IMEEDIT));
    ie.hwndNotify = hWnd;
    ie.dwAttrib = IME_ATTRIB_BASIC;
    iMax = strlen(pUserData->szSpell) - 1;
    if (('A' <= pUserData->szSpell[iMax]) && (pUserData->szSpell[iMax] <= 'Z'))
    {
        ie.pszImeName = "ABC";
    }

    pUserData->hwndSpell = CreateWindow(
        "IMEEDIT",
        pUserData->szSpell,
        WS_VISIBLE | WS_CHILD | ES_TITLE | ES_AUTOHSCROLL,
        rc.left + CX_MARGIN,
        rc.top + CY_MARGIN,
        rc.right - rc.left - 2 * CX_MARGIN,
        (rc.bottom - rc.top - CY_MARGIN) / 3,
        hWnd,
        (HMENU)ID_SPELLEDIT,
        NULL,
        (PVOID)&ie);

    SendMessage(pUserData->hwndSpell, EM_LIMITTEXT, MAX_LEN_EDIT, 0);
    SendMessage(pUserData->hwndSpell, EM_SETTITLE, 0, (LPARAM)RES_STR_NEWWORD);

    pUserData->hwndTarget = GetFocus();
    GetWindowText(pUserData->hwndParent, (PSTR)pUserData->szCaption,
        MAX_LEN_CAPTION);
    SendMessage(pUserData->hwndParent, PWM_GETBUTTONTEXT, 0,
        (LPARAM)pUserData->szRSoftKey);
    SendMessage(pUserData->hwndParent, PWM_GETBUTTONTEXT, 1,
        (LPARAM)pUserData->szLSoftKey);
    SendMessage(pUserData->hwndParent, PWM_GETBUTTONTEXT, 2,
        (LPARAM)pUserData->szMenuKey);
}

/**********************************************************************
 * Function     OnShowWindow
 * Purpose      
 * Params       hWnd, bShow
 * Return       
 * Remarks      
 **********************************************************************/

static void OnShowWindow(HWND hWnd, BOOL bShow)
{
    PZISPELL pUserData = NULL;

    pUserData = (PZISPELL)GetWindowLong(hWnd, GWL_USERDATA);

    if (bShow)
    {
        SetWindowText(pUserData->hwndParent, (PSTR)RES_STR_TEACH);
        SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 0,
            (LPARAM)RES_STR_CANCEL);
        SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 1,
            (LPARAM)RES_STR_OK);
        SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        SendMessage(pUserData->hwndParent, PWM_SETAPPICON,
            MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
        SetFocus(pUserData->hwndSpell);
    }
}

/**********************************************************************
 * Function     OnKeyDown
 * Purpose      
 * Params       hWnd, nKeyCode, dwKeyData
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKeyDown(HWND hWnd, int nKeyCode, DWORD dwKeyData)
{
    switch (nKeyCode)
    {
    case VK_RETURN:
        OnKeyDownReturn(hWnd);
        break;

    case VK_F10:
        OnKeyDownF10(hWnd);
        break;

    default:
        break;
    }
}

/**********************************************************************
 * Function     OnSetLBtnText
 * Purpose      
 * Params       hWnd, bEmpty, pszText
 * Return       
 * Remarks      
 **********************************************************************/

static void OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText)
{
    PZISPELL pUserData = NULL;

    pUserData = (PZISPELL)GetWindowLong(hWnd, GWL_USERDATA);

    if (nID == ID_SPELLEDIT)
    {
        SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 1,
            (LPARAM)(bEmpty ? "" : RES_STR_OK));
    }
    else
    {
        SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 1,
            (LPARAM)pszText);
    }
}

/**********************************************************************
 * Function     OnDestroy
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnDestroy(HWND hWnd)
{
    PZISPELL pUserData = NULL;
    pUserData = (PZISPELL)GetWindowLong(hWnd, GWL_USERDATA);

    free((void*)pUserData);
}

/**********************************************************************
 * Function     OnKeyDownReturn
 * Purpose      
 * Params       hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKeyDownReturn(HWND hWnd)
{
    PZISPELL pUserData = NULL;

    pUserData = (PZISPELL)GetWindowLong(hWnd, GWL_USERDATA);

    if (GetWindowTextLength(pUserData->hwndSpell) == 0)
        return;

    GetWindowText(pUserData->hwndSpell, (PSTR)pUserData->szSpell,
        MAX_LEN_EDIT);
    SendMessage(pUserData->hwndTarget, EM_REPLACESEL, 0,
        (LPARAM)pUserData->szSpell);

    SetWindowText(pUserData->hwndParent, pUserData->szCaption);
    SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 0,
        (LPARAM)pUserData->szRSoftKey);
    SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 1,
        (LPARAM)pUserData->szLSoftKey);
    SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 2,
        (LPARAM)pUserData->szMenuKey);

    SendMessage(hWnd, WM_CLOSE, 0, 0);
}

/**********************************************************************
 * Function     OnKeyDownF10
 * Purpose      
 * Params       hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKeyDownF10(HWND hWnd)
{
    PZISPELL pUserData = NULL;

    pUserData = (PZISPELL)GetWindowLong(hWnd, GWL_USERDATA);

    SendMessage(pUserData->hwndTarget, EM_REPLACESEL, 0,
        (LPARAM)pUserData->szSpell);

    SetWindowText(pUserData->hwndParent, pUserData->szCaption);
    SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 0,
        (LPARAM)pUserData->szRSoftKey);
    SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 1,
        (LPARAM)pUserData->szLSoftKey);
    SendMessage(pUserData->hwndParent, PWM_SETBUTTONTEXT, 2,
        (LPARAM)pUserData->szMenuKey);

    SendMessage(hWnd, WM_CLOSE, 0, 0);
}

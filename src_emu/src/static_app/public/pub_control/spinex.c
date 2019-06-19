/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 *    Copyright (c) 2005-2005 by Pollex Mobile Software Co., Ltd.
 *                       All Rights Reserved
 *
 * Module   :  spinex.c
 *
 * Purpose  :  Spin Box Extension Implementation
 *
\**************************************************************************/

#include "window.h"
#include "malloc.h"
#include "plx_pdaex.h"
#include "string.h"

#define SPIN_STATUS_FOCUS  0x00000001
#define MAX_PATH           256

#define CX_LEFTMARGIN      5
#define CX_RIGHTMARGIN     3
#define CY_TOPMARGIN1      4
#define CY_TOPMARGIN2      0

typedef struct tagSPINBOXEXDATA
{
    HWND    hwndSpinBox;
    DWORD   dwStatus;
    PSTR    pszTitle;
    int     nTitleLen;
    char    szDiaplay[128];
}
SPINBOXEXDATA, *PSPINBOXEXDATA;

static HBITMAP hbmpFocus = NULL;

static LRESULT CALLBACK SPINBOXEX_WndProc(HWND hWnd, UINT uMsgCmd,
                                          WPARAM wParam, LPARAM lParam);
static void OnCreate(HWND hWnd, LPARAM lParam);
static void OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void OnPaint(HWND hWnd);
static void OnCommand(HWND hWnd,WPARAM wParam,LPARAM lParam);
static void OnDestroy(HWND hWnd);
static BOOL OnSetTitle(HWND hWnd, LPARAM lParam);
static LRESULT OnGetTitle(HWND hWnd, int nBufLen, char* pszTitle);
static LRESULT OnCtlColorEdit(HWND hWnd, WPARAM wParam, LPARAM lParam);
static LRESULT OnSSBM(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

static BOOL GetTitleRect(HWND hWnd, PRECT pRect);
static void ChildRectToClient(HWND hWnd, HWND hwndChild, PRECT pRect);
extern int PLXOS_GetResourceDirectory(char *pathname);

/**********************************************************************
 * Function     SPINBOXEX_RegisterClass
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

BOOL SPINBOXEX_RegisterClass(void)
{
    WNDCLASS wc;
    char szPathName[MAX_PATH] = "";

    wc.style          = CS_OWNDC;
    wc.lpfnWndProc    = SPINBOXEX_WndProc;
    wc.cbClsExtra     = 0;
    wc.cbWndExtra     = sizeof(SPINBOXEXDATA);
    wc.hInstance      = NULL;
    wc.hIcon          = NULL;
    wc.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground  = NULL;
    wc.lpszMenuName   = NULL;
    wc.lpszClassName  = "SPINBOXEX";

    PLXOS_GetResourceDirectory(szPathName);
    strcat(szPathName, "listfocus.bmp");
    hbmpFocus = LoadImage(NULL, szPathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    return RegisterClass(&wc);
}

/**********************************************************************
 * Function     SPINBOXEX_WndProc
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT CALLBACK SPINBOXEX_WndProc(HWND hWnd, UINT uMsgCmd,
                                          WPARAM wParam, LPARAM lParam)
{
    switch (uMsgCmd)
    {
    case WM_CREATE:
        OnCreate(hWnd, lParam);
        return 0;

    case WM_SETFOCUS:
        OnSetFocus(hWnd, wParam, lParam);
        return 0;

    case WM_KILLFOCUS:
        OnKillFocus(hWnd, wParam, lParam);
        return 0;

    case WM_KEYDOWN:
        OnKeyDown(hWnd, wParam, lParam);
        return 0;

    case WM_KEYUP:
        OnKeyUp(hWnd, wParam, lParam);
        return 0;

    case WM_PAINT:
        OnPaint(hWnd);
        return 0;

    case WM_COMMAND:
        OnCommand(hWnd, wParam, lParam);
        return 0;

    case WM_DESTROY:
        OnDestroy(hWnd);
        return 0;

    case WM_CTLCOLOREDIT:
        return OnCtlColorEdit(hWnd, wParam, lParam);

    case SSBM_SETTITLE:
        return OnSetTitle(hWnd, lParam);

    case SSBM_GETTITLE:
        return OnGetTitle(hWnd, (int)wParam, (char*)lParam);

    case SSBM_ADDSTRING:
    case SSBM_DELETESTRING:
    case SSBM_SETCURSEL:
    case SSBM_GETCURSEL:
    case SSBM_SETTEXT:
    case SSBM_GETTEXT:
    case SSBM_GETTEXTLEN:
    case SSBM_GETCOUNT:
        return OnSSBM(hWnd, uMsgCmd, wParam, lParam);

    default:
        return DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
    }
}

/**********************************************************************
 * Function     OnCreate
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnCreate(HWND hWnd, LPARAM lParam)
{
    PSPINBOXEXDATA    pData = NULL;
    LPCREATESTRUCT  lpcs = NULL;
    RECT            rcTitle;
    WORD            wSpinStyle = 0;
    HDC             hdc = NULL;
    HFONT           hFont = NULL;
    BOOL            bSwapFont = FALSE;

    lpcs = (LPCREATESTRUCT)lParam;
    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    memset((void*)pData, 0, sizeof(SPINBOXEXDATA));
    wSpinStyle = LOWORD(lpcs->style);
    GetFontHandle(&hFont, SMALL_FONT);

    if (wSpinStyle & SSBS_SWAPFONT)
    {
        hdc = GetDC(hWnd);
        SelectObject(hdc, hFont);
        ReleaseDC(hWnd, hdc);
        wSpinStyle &= ~SSBS_SWAPFONT;
        bSwapFont = TRUE;
    }
    GetTitleRect(hWnd, &rcTitle);

    pData->hwndSpinBox = CreateWindow(
        "STRSPINBOX",
        "",
        WS_VISIBLE | WS_CHILD | CS_NOSYSCTRL | wSpinStyle,
        rcTitle.left,
        rcTitle.bottom + CY_TOPMARGIN2,
        rcTitle.right - rcTitle.left,
        lpcs->cy - rcTitle.bottom - 2 * CY_TOPMARGIN2,
        hWnd,
        NULL,
        NULL,
        NULL);

    if (!bSwapFont)
        SendMessage(pData->hwndSpinBox, WM_SETFONT, (WPARAM)hFont, FALSE);

    OnSetTitle(hWnd, (LPARAM)lpcs->lpszName);
}

/**********************************************************************
 * Function     OnSetFocus
 * Purpose      
 * Params       hWnd, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnSetFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXEXDATA pData = NULL;
    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    SetFocus(pData->hwndSpinBox);
}

/**********************************************************************
 * Function     OnKillFocus
 * Purpose      
 * Params       hWnd, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKillFocus(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

}

/**********************************************************************
 * Function     OnKeyDown
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKeyDown(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXEXDATA  pData = NULL;
    HWND          hwndParent = NULL;
    
    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    hwndParent = GetParent(hWnd);
    if (hwndParent == NULL)
        return;

    if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
    {
        SendMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
        return;
    }

    switch (wParam)
    {
    case VK_UP:
        SetFocus(GetNextDlgTabItem(hwndParent, hWnd, TRUE));
        break;

    case VK_DOWN:
        SetFocus(GetNextDlgTabItem(hwndParent, hWnd, FALSE));
        break;

    default:
        SendMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
        break;
    }
}

/**********************************************************************
 * Function     OnKeyUp
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnKeyUp(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXEXDATA  pData = NULL;
    HWND            hwndParent = NULL;
    
    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    hwndParent = GetParent(hWnd);
    if (hwndParent == NULL)
        return;

    if (GetWindowLong(hWnd, GWL_STYLE) & CS_NOSYSCTRL)
    {
        SendMessage(hwndParent, WM_KEYUP, wParam, lParam);
    }
}

/**********************************************************************
 * Function     OnPaint
 * Purpose      
 * Params       HWND hWnd
 * Return       
 * Remarks      
 **********************************************************************/

static void OnPaint(HWND hWnd)
{
    PSPINBOXEXDATA  pData = NULL;
    HDC           hdc = NULL;
    PAINTSTRUCT   ps;
    RECT          rc;
    int           nMode = 0;
    COLORREF      cr = 0;
    BITMAP        bmp;
    HBRUSH        hBrush = NULL;

    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    hdc = BeginPaint(hWnd, &ps);

    GetClientRect(hWnd, &rc);

    if (pData->dwStatus & SPIN_STATUS_FOCUS)
    {
        GetObject(hbmpFocus, sizeof(BITMAP), (void*)&bmp);
        StretchBlt(hdc, rc.left, rc.top, rc.right - rc.left,
            rc.bottom - rc.top, (HDC)hbmpFocus, 0, 0, bmp.bmWidth,
            bmp.bmHeight, ROP_SRC);

        cr = SetTextColor(hdc, COLOR_WHITE);
    }
    else
    {
        hBrush = (HBRUSH)SendMessage(GetParent(hWnd), WM_CTLCOLOREDIT,
            (WPARAM)hdc, (LPARAM)hWnd);
        FillRect(hdc, &rc, hBrush);
    }

    if (pData->pszTitle != NULL)
    {
        GetTitleRect(hWnd, &rc);
        
        nMode = SetBkMode(hdc, TRANSPARENT);
//        DrawText(hdc, pData->pszTitle, pData->nTitleLen, &rc,
//            DT_LEFT | DT_VCENTER);
        DrawText(hdc, pData->szDiaplay, -1, &rc, DT_LEFT | DT_VCENTER);
        SetBkMode(hdc, nMode);
    }

    if (pData->dwStatus & SPIN_STATUS_FOCUS)
    {
        SetTextColor(hdc, cr);
    }
    
    EndPaint(hWnd, &ps);
}

/**********************************************************************
 * Function     OnCommand
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXEXDATA  pData = NULL;
    HWND          hwndParent = NULL;
    DWORD         dwStyle = 0, dwID = 0;
    RECT          rc;

    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    dwID = GetWindowLong(hWnd, GWL_ID);

    if (dwStyle & WS_CHILD)
        hwndParent = GetParent(hWnd);
    else
        hwndParent = GetWindow(hWnd, GW_OWNER);

    switch (HIWORD(wParam))
    {
    case SSBN_SETFOCUS:
        pData->dwStatus |= SPIN_STATUS_FOCUS;

        InvalidateRect(hWnd, NULL, TRUE);

        SendMessage(hwndParent, PWM_SETSCROLLSTATE, SCROLLLEFT | SCROLLRIGHT | 
            SCROLLUP | SCROLLDOWN, MASKALL);
        break;

    case SSBN_KILLFOCUS:
        pData->dwStatus &= ~SPIN_STATUS_FOCUS;

        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case SSBN_ERASECTLBK:
        CopyRect(&rc, (const RECT*)lParam);
        ChildRectToClient(hWnd, pData->hwndSpinBox, &rc);

        rc.bottom -= 5; // Only patch for StretchBlt
        InvalidateRect(hWnd, (const RECT*)&rc, TRUE);
        break;

    default:
        break;
    }

    SendMessage(hwndParent, WM_COMMAND, MAKEWPARAM(dwID, HIWORD(wParam)),
        (LPARAM)hWnd);
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
    PSPINBOXEXDATA  pData = NULL;

    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    if (pData->pszTitle != NULL)
    {
        free((void*)pData->pszTitle);
    }
}

/**********************************************************************
 * Function     OnSetTitle
 * Purpose      
 * Params       HWND hWnd, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL OnSetTitle(HWND hWnd, LPARAM lParam)
{
    PSPINBOXEXDATA pData = NULL;
    int          nNewLen = 0;
    PSTR         pszNewTitle = NULL;
    RECT         rcTitle;
    HDC          hdc = NULL;
   
    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    pszNewTitle = (PSTR)lParam;

    nNewLen = strlen(pszNewTitle);

    if (pData->pszTitle == NULL)
    {
        pData->pszTitle = malloc(nNewLen + 1);
    }
    else if (pData->nTitleLen < nNewLen)
    {
        pData->pszTitle = realloc(pData->pszTitle, nNewLen + 1);
    }

    if (pData->pszTitle == NULL)
        return FALSE;

    strcpy((char*)pData->pszTitle, (const char*)pszNewTitle);
    pData->nTitleLen = nNewLen;

    GetTitleRect(hWnd, &rcTitle);
    hdc = GetDC(hWnd);
    GetExtentFittedText(hdc, pData->pszTitle, -1, pData->szDiaplay, 128,
        rcTitle.right - rcTitle.left, '.', 3);
    ReleaseDC(hWnd, hdc);
    InvalidateRect(hWnd, &rcTitle, TRUE);

    return TRUE;
}

/**********************************************************************
 * Function     OnGetTitle
 * Purpose      
 * Params       hWnd, nBufLen, pszTitle
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnGetTitle(HWND hWnd, int nBufLen, char* pszTitle)
{
    PSPINBOXEXDATA pData = NULL;
    int nCopied = 0;

    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    nCopied = min(pData->nTitleLen, nBufLen - 1);
    strncpy(pszTitle, pData->pszTitle, nCopied);
    pszTitle[nCopied] = '\0';

    return (LRESULT)nCopied;
}

/**********************************************************************
 * Function     OnCtlColorEdit
 * Purpose      
 * Params       HWND hWnd, WPARAM wParam, LPARAM lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnCtlColorEdit(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXEXDATA pData = NULL;

    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    if (pData->dwStatus & SPIN_STATUS_FOCUS)
    {
        return (LRESULT)GetStockObject(NULL_BRUSH);
    }
    else
    {
        return DefWindowProc(hWnd, WM_CTLCOLOREDIT, wParam, lParam);
    }
}

/**********************************************************************
 * Function     OnSSBM
 * Purpose      
 * Params       hWnd, uMsg, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT OnSSBM(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    PSPINBOXEXDATA pData = NULL;
    pData = (PSPINBOXEXDATA)GetUserData(hWnd);

    return SendMessage(pData->hwndSpinBox, uMsg, wParam, lParam);
}

/**********************************************************************
 * Function     GetTitleRect
 * Purpose      
 * Params       HWND hWnd, PRECT pRect
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL GetTitleRect(HWND hWnd, PRECT pRect)
{
    RECT   rcClient;
    HDC    hdc = NULL;
    SIZE   size;

    GetClientRect(hWnd, &rcClient);

    hdc = GetDC(hWnd);
    GetTextExtentPoint32(hdc, "T", -1, &size);
    ReleaseDC(hWnd, hdc);

    pRect->left   = rcClient.left + CX_LEFTMARGIN;
    pRect->top    = rcClient.top + CY_TOPMARGIN1;
    pRect->right  = rcClient.right - CX_RIGHTMARGIN;
    pRect->bottom = pRect->top + size.cy;

    return TRUE;
}

/**********************************************************************
 * Function     ChildRectToClient
 * Purpose      
 * Params       hWnd, hwndChild, pRect
 * Return       
 * Remarks      
 **********************************************************************/

static void ChildRectToClient(HWND hWnd, HWND hwndChild, PRECT pRect)
{
    POINT pt;

    pt.x = pRect->left;
    pt.y = pRect->top;
    ClientToScreen(hwndChild, &pt);
    ScreenToClient(hWnd, &pt);
    pRect->left = pt.x;
    pRect->top = pt.y;
    
    pt.x = pRect->right;
    pt.y = pRect->bottom;
    ClientToScreen(hwndChild, &pt);
    ScreenToClient(hWnd, &pt);
    pRect->right = pt.x;
    pRect->bottom = pt.y;
}

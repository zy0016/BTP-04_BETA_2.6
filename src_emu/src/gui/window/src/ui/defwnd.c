/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implents default window proc function.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "windowx.h"
#include "defwnd.h"
#include "string.h"
#include "control.h"

#if (COLUMN_BITMAP)
static const BYTE CloseBoxBmpData[] = 
{
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x10, 0x40, 0x00, 0x00,     // 00010000010000000000000000000000
    0x18, 0xC0, 0x00, 0x00,     // 00011000110000000000000000000000
    0x0D, 0x80, 0x00, 0x00,     // 00001101100000000000000000000000
    0x07, 0x00, 0x00, 0x00,     // 00000111000000000000000000000000
    0x07, 0x00, 0x00, 0x00,     // 00000111100000000000000000000000
    0x0D, 0x80, 0x00, 0x00,     // 00001101100000000000000000000000
    0x18, 0xC0, 0x00, 0x00,     // 00011000011000000000000000000000
    0x10, 0x40, 0x00, 0x00,     // 00010000010000000000000000000000
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00      // 00000000000000000000000000000000
};
#else
static const BYTE CloseBoxBmpData[] = 
{
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x18, 0x60, 0x00, 0x00,     // 00011000011000000000000000000000
    0x0C, 0xC0, 0x00, 0x00,     // 00001100110000000000000000000000
    0x07, 0x80, 0x00, 0x00,     // 00000111100000000000000000000000
    0x03, 0x00, 0x00, 0x00,     // 00000011000000000000000000000000
    0x07, 0x80, 0x00, 0x00,     // 00000111100000000000000000000000
    0x0C, 0xC0, 0x00, 0x00,     // 00001100110000000000000000000000
    0x18, 0x60, 0x00, 0x00,     // 00011000011000000000000000000000
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00      // 00000000000000000000000000000000
};
#endif

// Two internal function used to DefWindowProc to create and destroy
// data.
#define WM_DATACREATE   0x03FE
#define WM_DATADESTROY  0x03FF

// Define box(CloseBox, MaximizeBox, MinimizeBox and SysMenuBox)
// width and height
#define CLOSE_BOX_WIDTH     14
#define CLOSE_BOX_HEIGHT    13
#define CLOSE_BOX_GAP_X     2
#define CLOSE_BOX_GAP_Y     3    

// Define the valid offset used to enlarge/shrink the size of window by
// dragging the corner of the window.
#define CORNER_OFFSET       5

// define the minimum dimension of the window
#define MIN_DIMENSION       50

// Define WINOBJ data
typedef struct
{
    // Record the text of the window
    PSTR pszText;

    // Record the pos of pen pushing down.
    BYTE nPenDown;

    // For scroll bar
    BYTE bDisableNoHScroll;
    BYTE bDisableNoVScroll;
    SCROLLDATA* pHScrollData;
    SCROLLDATA* pVScrollData;

#if (!NOMENUS)     // Record the window of menubar

    HWND hwndMenuBar; 

#endif // NOMENUS

#if (!NODWMOVESIZE)    // For enlarging/shrinking a window)

    int nOrgX;
    int nOrgY;
    int nOffsetX;
    int nOffsetY;
    int nPrevX;
    int nPrevY;
    RECT rcMove;
    BYTE bMove;

#endif  // NODWMOVESIZE
} WNDDATA, *PWNDDATA;

static HBITMAP hbmpCloseBox;
static HBRUSH hbrScrollBar, hbrTransBk = NULL;

// Part consant for DoNCPaint
#define NC_TEXT         0   // Draw window text only
#define NC_CAPTION      1   // Draw window caption
#define NC_FOCUSBORDER  2   // Draw window focus border
#define NC_ALL          3   // Draw all window non client

// Internal function prototypes
static void DoNCCalcSize(HWND hWnd, PWNDDATA pWndData, WPARAM wParam, 
                         LPARAM lParam);
static void DoNCPaint(HWND hWnd, PWNDDATA pWndData, int nPart, 
                      BOOL bActive);
static int  FindNCHit(HWND hWnd, LPARAM lParam);
static void DoCancelMode(HWND hWnd, PWNDDATA pWndData);
static void DoNCActivate(HWND hWnd, PWNDDATA pWndData, BOOL bActive);
static BOOL EraseBkgnd(HWND hWnd, UINT message, HDC hdc);

#if (!NODWMOVESIZE)
static void DrawMovingRect(HWND hWnd, PRECT pRect, long dwStyle);
static void ScreenToParent(HWND hWnd, PRECT pRect);
#endif
static void DrawCloseBox(HWND hWnd, HDC hdc, const RECT* pRect, 
                         BOOL bDown);
static int  SetCloseBoxRect(HWND hWnd, int nPenX, int nPenY, PRECT rect);

static BOOL DEFWND_NCLBtnDown(HWND hWnd, PWNDDATA pWndData, WPARAM wParam,
                              LPARAM lParam);
static BOOL DEFWND_NCMouseMove(HWND hWnd, PWNDDATA pWndData, 
                               LPARAM lParam);
static BOOL DEFWND_NCLBtnUp(HWND hWnd, PWNDDATA pWndData, LPARAM lParam);
#if (!NOCURSOR)
static BOOL DEFWND_SetCursor(HWND hWnd, WPARAM wParam, LPARAM lParam);
#endif
static void GetWindowRectToParent(HWND hWnd, PRECT pRect);

static PWNDDATA GetWndData(HWND hWnd);

/**************************************************************************/
/*              Window proc implementation                                */
/**************************************************************************/

#if (!BASEWINDOW)
/*
**  Function : DefWindowProc
**  Purpose  :
**      Calls the default window procedure to provide default processing 
**      for any window messages that an application does not process. 
**      This function ensures that every message is processed. DefWindowProc 
**      is called with the same parameters received by the window procedure. 
**  Params   :
**      hWnd    : Identifies the window procedure that received the message. 
**      wMsgCmd : Specifies the message. 
**      wParam  : Specifies additional message information. The content of 
**                this parameter depends on the value of the Msg parameter. 
**      lParam  : Specifies additional message information. The content of 
**                this parameter depends on the value of the Msg parameter. 
**  Return   :
**      The return value is the result of the message processing and depends
**      on the message. 
*/
LRESULT WINAPI DefWindowProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                             LPARAM lParam)
{
    // 这里暂时不判断hWnd的有效性, hWnd的有效性将在DEFWND_Proc函数中判断
    return DEFWND_Proc(hWnd, wMsgCmd, wParam, lParam);
}

//******************************************************************
//              Window class register and unregister                
//******************************************************************
#if (PDAAPI)
BOOL WINPDA_RegisterClass(void);
#endif

/*
**  Function : RegisterDefWindowClass
**  Purpose  :
**      Registers default window class.
*/
BOOL DEFWND_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = 0;
    wc.lpfnWndProc      = DEFWND_Proc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = DEFWND_GetDataSize(NULL);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = DEFWINDOWCLASSNAME;

    if (!RegisterClass(&wc))
        return FALSE;

    hbmpCloseBox = CreateSharedBitmap(CLOSE_BOX_WIDTH, CLOSE_BOX_HEIGHT, 
                            1, 1, CloseBoxBmpData);

    // Creates background brush for scroll bar
    hbrScrollBar = CreateSharedBrush(BS_HATCHED, 
        GetSysColor(COLOR_SCROLLBAR), HS_SIEVE);
//    hbrScrollBar = CreateHatchBrush(HS_SIEVE, 
//        GetSysColor(COLOR_SCROLLBAR));
    hbrTransBk = CreateSolidBrush(COLOR_TRANSBK);

#if (PDAAPI)
    if (!(WINPDA_RegisterClass()))
        return FALSE;
#endif

    return TRUE;
}

#endif //BASEWINDOW
/*
**  Function : DEFWND_GetDataSize      defwnd.c
**  Purpose  :
**      Get size of user data of a default window object.
*/
DWORD DEFWND_GetDataSize(HWND hWnd)
{
    return sizeof(WNDDATA);
}


#if (!NOMENUS)

/*
**  Function : DEFWND_GetMenuCtrl
**  Purpose  :
**      Retrieves the menu ctrl of a specified window
*/
HWND DEFWND_GetMenuCtrl(HWND hWnd)
{
    PWNDDATA pWndData;

    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return NULL;

    return pWndData->hwndMenuBar;
}

#endif // NOMENUS

/*
**  Function : DEFWND_Proc
**  Purpose  :
**      Window proc for default window.
*/
LRESULT DEFWND_Proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    PWNDDATA pWndData;
    PCREATESTRUCT pcs;
    PAINTSTRUCT ps;
    RECT rect, rcClient;
    PSTR pszText;
    HWND hwndParent;
    HDC hdc;
    int nMaxCount, nTextLen;

#if (!NOMENUS)
    int nBorderW, nBorderH;
    int x, y, width, height;
    char pClassName[32];
#endif // NOMENUS

    // Gets the internal data for default window
    pWndData = GetWndData(hWnd);

    lResult = (LRESULT)TRUE;

    switch (message)
    {
    case WM_DATACREATE :

        pcs = (PCREATESTRUCT)lParam;

        // Initializes the window data
        if (!pWndData)
            return (LRESULT)FALSE;

        memset(pWndData, 0, DEFWND_GetDataSize(hWnd));

        // Saves the window name field
        if (pcs->lpszName)
        {
            pWndData->pszText = MemAlloc(strlen(pcs->lpszName) + 1);
            if (pWndData->pszText)
                strcpy(pWndData->pszText, pcs->lpszName);
        }

#if (!NOMENUS)

        // 如果创建的窗口不是菜单条窗口, 并且指定了窗口菜单, 创建菜单条子窗
        // 口(非客户区子窗口)
        GetClassName(hWnd, pClassName, 32);
        if (!(pcs->style & WS_CHILD) && pcs->hMenu && 
            strcmp(pClassName, "#MENU"))
        {
            // 计算边框的大小
            if ((pcs->style & WS_DLGFRAME) == WS_DLGFRAME)
            {
                nBorderW = GetSystemMetrics(SM_CXDLGFRAME);
                nBorderH = GetSystemMetrics(SM_CYDLGFRAME);
            }
            else if (pcs->style & WS_BORDER)
            {
                nBorderW = GetSystemMetrics(SM_CXBORDER);
                nBorderH = GetSystemMetrics(SM_CYBORDER);
            }
            else if (pcs->style & WS_THICKFRAME)
            {
                nBorderW = GetSystemMetrics(SM_CXFRAME);
                nBorderH = GetSystemMetrics(SM_CYFRAME);
            }
            else
            {
                nBorderW = 0;
                nBorderH = 0;
            }

            // 计算菜单窗口的位置和大小(相对于窗口原点)

            x = nBorderW;
            y = nBorderH;

            if (pcs->style & WS_CAPTION)
                y += GetSystemMetrics(SM_CYCAPTION);

            width = pcs->cx - 2 * nBorderW;
            height = GetSystemMetrics(SM_CYMENU);

            // 创建菜单窗口

            SendMessage(hWnd, WM_INITMENU, (WPARAM)pcs->hMenu, 0);            
            pWndData->hwndMenuBar = CreateWindow("#MENU", "", 
                WS_VISIBLE | WS_CHILD | WS_NCCHILD, 
                x, y, width, height, hWnd, 
                (HMENU)(pcs->hMenu), NULL, NULL);

            if (!pWndData->hwndMenuBar)
                return FALSE;
        }

#endif // NOMENUS

        if (pcs->style & WS_VSCROLL)
        {
            pWndData->pVScrollData = MemAlloc(sizeof(SCROLLDATA));
            if (!pWndData->pVScrollData)
                return FALSE;
            SB_Init(pWndData->pVScrollData, SBS_VERT | SBS_NCSCROLL, 
                pcs->hwndParent);
        }

        if (pcs->style & WS_HSCROLL)
        {
            pWndData->pHScrollData = MemAlloc(sizeof(SCROLLDATA));
            if (!pWndData->pHScrollData)
                return FALSE;
            SB_Init(pWndData->pHScrollData, SBS_HORZ | SBS_NCSCROLL, 
                pcs->hwndParent);
        }

        break;
        
    case WM_DATADESTROY:
        
        if (pWndData)
        {
            if (pWndData->pszText)
                MemFree(pWndData->pszText);
            
            if (pWndData->pVScrollData)
                MemFree(pWndData->pVScrollData);
            
            if (pWndData->pHScrollData)
                MemFree(pWndData->pHScrollData);
        }
        break;
        
    case WM_QUERYENDSESSION:
        return TRUE;

    case WM_PMKEYDOWN:
        return TRUE;

    case WM_TODESKTOP:
        DestroyWindow(hWnd);
        break;

    case WM_NCCALCSIZE :

        DoNCCalcSize(hWnd, pWndData, wParam, lParam);
        return 0;

    case WM_NCPAINT :

        DoNCPaint(hWnd, pWndData, NC_ALL, GetForegroundWindow() == hWnd);
        break;

    case WM_NCACTIVATE:

        // Do non-client drawing in response to activation or deactivation.
        DoNCActivate(hWnd, pWndData, (BOOL)wParam);
        return (LRESULT)TRUE;
        
    case WM_CANCELMODE:

        // Terminate any modes the system might be in, such as scrollbar 
        // tracking, menu mode, button capture, etc.
        DoCancelMode(hWnd, pWndData);
        break;

    case WM_ERASEBKGND :

        // 此时不能使用message作为调用EraseBkgnd的第二个参数，因为mcore平台
        // 的编译器此时会改变message的值，这应该是mcore平台的编译器的bug
        return (LRESULT)EraseBkgnd(hWnd, WM_ERASEBKGND, (HDC)wParam);

    case WM_ACTIVATE :

        // By default, windows set the focus to themselves when activated.
        if ((BOOL)wParam)
        {
            HWND hFocus;

            /* 2003/5/28 修改。焦点窗口可以设到子窗口。这样新创建popup窗口
            ** 可以得到焦点 
            */
            hFocus = GetFocus();
            if (IsChild(hWnd, hFocus))
                SetFocus(hFocus);
            else
                SetFocus(hWnd);
        }

        break;

#if (INTERFACE_MONO && FOCUSBORDER)

    case WM_SETFOCUS :
    case WM_KILLFOCUS :

        DoNCPaint(hWnd, pWndData, NC_FOCUSBORDER, FALSE);
        break;
        
#endif // defined(INTERFACE_MONO) && defined(FOCUSBORDER)
        
    case WM_MOUSEACTIVATE :

        // First give the parent a chance to process the message.
        hwndParent = GetParent(hWnd);
        if (hwndParent)
        {
            lResult = SendMessage(hwndParent, WM_MOUSEACTIVATE, wParam, 
                lParam);
            
            if (lResult != 0)
                return lResult;
        }
        
        // If the user clicked in the title bar, don't activate now:
        // the activation will take place later when the move or size
        // occurs.
        // if (LOWORD(lParam) == HTCAPTION)
        //    return((LRESULT)(LONG)MA_NOACTIVATE);
        
        return (LRESULT)MA_ACTIVATE;

    case WM_PAINT :

        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);

        break;

    case WM_CLOSE :

        // Default WM_CLOSE handling is to destroy the window.
        DestroyWindow(hWnd);

        break;

    case WM_NCHITTEST :

        // Determine what area the passed coordinate is in.
        return ((LRESULT)FindNCHit(hWnd, (LONG)lParam));

    case WM_GETTEXT :

        nMaxCount = wParam;
        pszText = (PSTR)lParam;

        if (nMaxCount <= 0)
            return 0;

        if (pWndData && pWndData->pszText)
        {
            nTextLen = strlen(pWndData->pszText);
            if (nTextLen > nMaxCount - 1)
                nTextLen = nMaxCount - 1;
            strncpy(pszText, pWndData->pszText, nTextLen);
            pszText[nTextLen] = 0;
            
            lResult = (LRESULT)strlen(pszText);
        }
        else
        {
            // Make sure we return an empty string
            pszText[0] = 0;
            lResult = 0;
        }

        break;

    case WM_SETTEXT :

        // The data of the default window isn't initialized, can't set
        // window text
        if (!pWndData)
            return 0;

        pszText = (PSTR)lParam;

        if (pWndData->pszText == NULL || 
            strlen(pWndData->pszText) < strlen(pszText))
        {
            if (pWndData->pszText)
                MemFree(pWndData->pszText);

            pWndData->pszText = MemAlloc(strlen(pszText) + 1);
        }

        if (pWndData->pszText)
        {
            strcpy(pWndData->pszText, pszText);

            DoNCPaint(hWnd, pWndData, NC_TEXT, GetForegroundWindow() == hWnd);
            lResult = (LRESULT)TRUE;
        }
        else
            lResult = (LRESULT)FALSE;

        break;

    case WM_GETTEXTLENGTH :

        if (pWndData && pWndData->pszText)
            lResult = (LRESULT)strlen(pWndData->pszText);
        else
            lResult = 0;

        break;

    case WM_NCLBUTTONDOWN :

        lResult = (LRESULT)DEFWND_NCLBtnDown(hWnd, pWndData, wParam, 
            lParam);

        break;

    case WM_NCMOUSEMOVE :

        lResult = (LRESULT)DEFWND_NCMouseMove(hWnd, pWndData, lParam);

        break;

    case WM_NCLBUTTONUP :

        lResult = (LRESULT)DEFWND_NCLBtnUp(hWnd, pWndData, lParam);

        break;

#if (!NOCURSOR)

    case WM_SETCURSOR :

        lResult = (LRESULT)DEFWND_SetCursor(hWnd, wParam, lParam);
        break;

#endif // NOCURSOR
    
    case WM_SYSTIMER :

        if (!pWndData)
            return FALSE;

        // Gets internal data pointer of the window
        if (!pWndData->nPenDown)
            return 0L;

        hdc = GetWindowDC(hWnd);

        if (pWndData->nPenDown == HTVSCROLL)
        {
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

            SetRect(&rect, rcClient.right, rcClient.top, 
                    rcClient.right + GetSystemMetrics(SM_CXVSCROLL),
                    rcClient.bottom);

            SB_Timer(hWnd, hdc, pWndData->pVScrollData, &rect);
        }
        else if (pWndData->nPenDown == HTHSCROLL)
        {
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

            SetRect(&rect, rcClient.left, rcClient.bottom,
                    rcClient.right, 
                    rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));
            SB_Timer(hWnd, hdc, pWndData->pHScrollData, &rect);
        }

        ReleaseDC(hWnd, hdc);

        break;

    // The following WM_CTLCOLOR... message 
    // Set up the supplied DC with the foreground and background
    // colors we want to use in the control, and return a brush
    // to use for filling.

    case WM_CTLCOLORSCROLLBAR :

        // Background = white
        // Foreground = COLOR_SCROLLBAR
        // brush = hbrScrollBar.
        
        SetBkColor((HDC)wParam, RGB(255, 255, 255));
        SetTextColor((HDC)wParam, GetSysColor(COLOR_SCROLLBAR));
        
        return (LRESULT)hbrScrollBar;
        
    case WM_CTLCOLORBTN :

        // Background = COLOR_BTNFACE
        // Foreground = black
        // brush = COLOR_BTNFACE.
        
        SetBkColor((HDC)wParam, COLOR_TRANSBK);//GetSysColor(COLOR_BTNFACE));
        SetTextColor((HDC)wParam, RGB(0, 0, 0));
        
        return (LRESULT)hbrTransBk;//(COLOR_BTNFACE + 1);

    case WM_CTLCOLORMSGBOX : 
    case WM_CTLCOLOREDIT : 
    case WM_CTLCOLORLISTBOX :
    case WM_CTLCOLORDLG :
    case WM_CTLCOLORSTATIC :

        // Background = COLOR_WINDOW
        // Foreground = COLOR_WINDOWTEXT
        // Brush = COLOR_WINDOW
        
        SetBkColor((HDC)wParam, COLOR_TRANSBK);
        SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
        
        return (LRESULT)hbrTransBk;//(COLOR_WINDOW + 1);

#if (!NOKEYBOARD)

    case WM_SYSKEYDOWN :

        // WM_SYSKEYDOWN消息由ALT键按下引起并且不是ALT键本身按下
        if ((lParam & 0x20000000) && wParam != VK_ALT)
        {
            HWND hwndParent, hwndTop;
            // ALT + F4 close the top level window
            if (wParam == VK_F4)
            {
                hwndTop = hWnd;
                hwndParent = hWnd;
                while ((hwndParent = GetParent(hwndParent)) != NULL)
                    hwndTop = hwndParent;

                PostMessage(hwndTop, WM_CLOSE, 0, 0);
            }
#if (!NOMENUS)
            else if (pWndData->hwndMenuBar)
            {
                SendMessage(pWndData->hwndMenuBar, WM_KEYDOWN, wParam, 
                    lParam);
            }
            else if (GetParent(hWnd)!= NULL)
            {
                PWNDDATA pTopWndData;
                
                hwndTop = hWnd;
                hwndParent = hWnd;
                while ((hwndParent = GetParent(hwndParent)) != NULL)
                {
                    hwndTop = hwndParent;
                    pTopWndData = GetWndData(hwndParent);
                    if (pTopWndData->hwndMenuBar)
                    {
                        SendMessage(pTopWndData->hwndMenuBar, WM_KEYDOWN, wParam, 
                            lParam);
                        break;
                    }
                }
            }
#endif // NOMENUS
        }

        break;

    case WM_SYSKEYUP :

#if (!NOMENUS)
        
        if (pWndData->hwndMenuBar)  // there is a menubar
        {
            if (wParam == VK_ALT)
            {
                SendMessage(pWndData->hwndMenuBar, WM_KEYUP, wParam, 
                    lParam);
            }
        }
        else if (GetParent(hWnd)!= NULL)
        {
            PWNDDATA pTopWndData;
            HWND hwndTop, hwndParent;
            
            hwndTop = hWnd;
            hwndParent = hWnd;
            while ((hwndParent = GetParent(hwndParent)) != NULL)
            {
                hwndTop = hwndParent;
                pTopWndData = GetWndData(hwndParent);
                
                if (pTopWndData->hwndMenuBar && (wParam == VK_ALT))
                {
                    SendMessage(pTopWndData->hwndMenuBar, WM_KEYDOWN, wParam, 
                        lParam);
                    break;
                }
            }
        }

#endif // NOMENUS

        break;

#endif // NOKEYBOARD

    case WM_GETDLGCODE :

        return 0;
    }

    return lResult;
}

static BOOL DEFWND_NCLBtnDown(HWND hWnd, PWNDDATA pWndData, WPARAM wParam,
                              LPARAM lParam)
{
    int     nPenX, nPenY;
    RECT    rect, rcBorder, rcClient, rcWindow;
    HDC     hdc;

#if (!NODWMOVESIZE)

    DWORD   dwStyle;
    HWND    hwndParent;

#endif // NODWMOVESIZE

    // The data of the default window isn't initialized, can't response
    // to the WM_NCPENDOWN message, just return.
    if (!pWndData)
        return FALSE;

    if ((pWndData->nPenDown = (BYTE)wParam) == HTNOWHERE)
        return FALSE;

    // Gets NC pen data
    nPenX = LOWORD(lParam);
    nPenY = HIWORD(lParam);

    if (pWndData->nPenDown == HTCLOSE || pWndData->nPenDown == HTVSCROLL ||
        pWndData->nPenDown == HTHSCROLL)
    {
        switch (pWndData->nPenDown)
        {
        case HTCLOSE :
            
            SetCloseBoxRect(hWnd, nPenX, nPenY, &rcBorder);
            
            hdc = GetWindowDC(hWnd);
            DrawCloseBox(hWnd, hdc, &rcBorder, TRUE);
            ReleaseDC(hWnd, hdc);
            
            break;
            
        case HTVSCROLL :
            
            hdc = GetWindowDC(hWnd);
            
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);
            GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);
            
            SetRect(&rect, rcClient.right, rcClient.top, 
                rcClient.right + GetSystemMetrics(SM_CXVSCROLL),
                rcClient.bottom);
            
            SB_PenProcess(hWnd, hdc, pWndData->pVScrollData, &rect,
                (UINT)WM_PENDOWN, 
                MAKELPARAM(nPenX - rcWindow.left, nPenY - rcWindow.top));
            
            ReleaseDC(hWnd, hdc);
            
            break;
            
        case HTHSCROLL :
            
            hdc = GetWindowDC(hWnd);
            
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);
            GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);
            
            SetRect(&rect, rcClient.left, rcClient.bottom,
                rcClient.right, 
                rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));
            
            SB_PenProcess(hWnd, hdc, pWndData->pHScrollData, &rect,
                (UINT)WM_PENDOWN, 
                MAKELPARAM(nPenX - rcWindow.left, nPenY - rcWindow.top));
            
            ReleaseDC(hWnd, hdc);
            
            break;
        }

        SetNCCapture(hWnd);
        return TRUE;
    }

#if (!NODWMOVESIZE)

    // when the border is thinBorder, don't enlarge/shrink the window.
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    if ((dwStyle & WS_DLGFRAME) != WS_THICKFRAME && 
        pWndData->nPenDown != HTCAPTION)
    {
        pWndData->nPenDown = HTNOWHERE;
        return FALSE;
    }

    // Gets the window rect
    GetWindowRect(hWnd, &pWndData->rcMove);

    // Calculate the pen down point offset to window 
    pWndData->nOffsetX = 0;
    pWndData->nOffsetY = 0;
        
    switch (pWndData->nPenDown)
    {
    case HTLEFT :
            
        pWndData->nOffsetX = pWndData->rcMove.left - nPenX;
        break;
            
    case HTRIGHT :
            
        pWndData->nOffsetX = pWndData->rcMove.right - nPenX;
        break;
            
    case HTTOP :
            
        pWndData->nOffsetY = pWndData->rcMove.top - nPenY;
        break;
            
    case HTBOTTOM :
            
        pWndData->nOffsetY = pWndData->rcMove.bottom - nPenY;
        break;
            
    case HTTOPLEFT :
            
        pWndData->nOffsetX = pWndData->rcMove.left - nPenX;
        pWndData->nOffsetY = pWndData->rcMove.top - nPenY;
        break;
            
    case HTTOPRIGHT :
            
        pWndData->nOffsetX = pWndData->rcMove.right - nPenX;
        pWndData->nOffsetY = pWndData->rcMove.top - nPenY;
        break;
            
    case HTBOTTOMLEFT :
            
        pWndData->nOffsetX = pWndData->rcMove.left - nPenX;
        pWndData->nOffsetY = pWndData->rcMove.bottom - nPenY;
        break;
            
    case HTBOTTOMRIGHT :
            
        pWndData->nOffsetX = pWndData->rcMove.right - nPenX;
        pWndData->nOffsetY = pWndData->rcMove.bottom - nPenY;
        break;

    case HTCAPTION :

        pWndData->bMove = FALSE;
        pWndData->nOrgX = nPenX;
        pWndData->nOrgY = nPenY;
        break;

    default :

        pWndData->nPenDown = HTNOWHERE;
        return FALSE;
    }
        
    pWndData->nPrevX = nPenX + pWndData->nOffsetX;
    pWndData->nPrevY = nPenY + pWndData->nOffsetY;

    StartDrag(hWnd, NULL, 4);

    if (pWndData->nPenDown != HTCAPTION ) 
        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);

    // get the parent window of the current window
    hwndParent = GetParent(hWnd);
    if (!hwndParent)    // parent window is null
//        hwndParent = GetDesktopWindow();
        hwndParent = HWND_DESKTOP;

    GetWindowRectEx(hwndParent, &rect, W_CLIENT, XY_SCREEN);
    ClipCursor(&rect);
    SetNCCapture(hWnd);

    return TRUE;

#else  // NODWMOVESIZE

    return FALSE;

#endif // NODWMOVESIZE
}

static BOOL DEFWND_NCMouseMove(HWND hWnd, PWNDDATA pWndData, LPARAM lParam)
{
    int     nPenX, nPenY;
    RECT    rect, rcBorder, rcClient, rcWindow;
    HDC     hdc;

#if (!NODWMOVESIZE)

    int deltax, deltay;
    DWORD dwStyle;

#endif  // NODWMOVESIZE

    // The data of the default window isn't initialized, can't response
    // to the WM_NCPENMOVE message, just return.
    if (!pWndData)
        return FALSE;

    // 下面语句之后不需要再判断窗口是否可以变大小, 因为响应WM_NCLBUTTONDOWN
    // 时已经判断, 如果窗口不能变大小, nPenDown为HTNOWHERE将直接返回
    if (pWndData->nPenDown == HTNOWHERE)
        return FALSE;

    nPenX = LOWORD(lParam);
    nPenY = HIWORD(lParam);

    if (pWndData->nPenDown == HTCLOSE || pWndData->nPenDown == HTVSCROLL ||
        pWndData->nPenDown == HTHSCROLL)
    {
        switch (pWndData->nPenDown)
        {
        case HTCLOSE:
            
            hdc = GetWindowDC(hWnd);
            if (SetCloseBoxRect(hWnd, nPenX, nPenY, &rcBorder))
                DrawCloseBox(hWnd, hdc, &rcBorder, TRUE);
            else
                DrawCloseBox(hWnd, hdc, &rcBorder, FALSE);
            ReleaseDC(hWnd, hdc);
            
            break;
            
        case HTVSCROLL:
            
            hdc = GetWindowDC(hWnd);
            
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);
            GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);
            
            SetRect(&rect, rcClient.right, rcClient.top, 
                rcClient.right + GetSystemMetrics(SM_CXVSCROLL),
                rcClient.bottom);
            
            SB_PenProcess(hWnd, hdc, pWndData->pVScrollData, &rect, 
                (UINT)WM_PENMOVE, 
                MAKELPARAM(nPenX - rcWindow.left, nPenY - rcWindow.top));
            
            ReleaseDC(hWnd, hdc);
            
            break;
            
        case HTHSCROLL:
            
            hdc = GetWindowDC(hWnd);
            
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);
            GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);
            
            SetRect(&rect, rcClient.left, rcClient.bottom,
                rcClient.right, 
                rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));
            
            SB_PenProcess(hWnd, hdc, pWndData->pHScrollData, &rect,
                (UINT)WM_PENMOVE, 
                MAKELPARAM(nPenX - rcWindow.left, nPenY - rcWindow.top));
            
            ReleaseDC(hWnd, hdc);
            
            break;
        }
        
        return TRUE;
    }

#if (!NODWMOVESIZE)

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    nPenX += pWndData->nOffsetX;
    nPenY += pWndData->nOffsetY;
        
    switch (pWndData->nPenDown)
    {
    case HTLEFT :
            
        // the width of the window can't be less than MIN_DIMENSION
        if (nPenX > pWndData->rcMove.right - MIN_DIMENSION)
            return FALSE;
     
        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);
            
        pWndData->rcMove.left += nPenX - pWndData->nPrevX;
            
        break;
            
    case HTRIGHT :
            
        if (nPenX < pWndData->rcMove.left + MIN_DIMENSION)
            return FALSE;

        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);
            
        pWndData->rcMove.right += nPenX - pWndData->nPrevX;
            
        break;
            
    case HTTOP :
            
        if (nPenY > pWndData->rcMove.bottom - MIN_DIMENSION)
            return FALSE;
            
        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);
            
        pWndData->rcMove.top += nPenY - pWndData->nPrevY;
            
        break;
            
    case HTBOTTOM :
            
        if (nPenY < pWndData->rcMove.top + MIN_DIMENSION)
            return FALSE;
            
        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);
            
        pWndData->rcMove.bottom += nPenY - pWndData->nPrevY;
            
        break;
            
    case HTTOPLEFT :
            
        if (nPenX > pWndData->rcMove.right - MIN_DIMENSION)
            return FALSE;
            
        if (nPenY > pWndData->rcMove.bottom - MIN_DIMENSION)
            return FALSE;
            
        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);
            
        pWndData->rcMove.left += nPenX - pWndData->nPrevX;
        pWndData->rcMove.top += nPenY - pWndData->nPrevY;
            
        break;
            
    case HTTOPRIGHT :
            
        if (nPenX < pWndData->rcMove.left + MIN_DIMENSION)
            return FALSE;
            
        if (nPenY > pWndData->rcMove.bottom - MIN_DIMENSION)
            return FALSE;
            
        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);
            
        pWndData->rcMove.right += nPenX - pWndData->nPrevX;
        pWndData->rcMove.top += nPenY - pWndData->nPrevY;
            
        break;
            
    case HTBOTTOMLEFT :
            
        if (nPenX > pWndData->rcMove.right - MIN_DIMENSION)
            return FALSE;
            
        if (nPenY < pWndData->rcMove.top + MIN_DIMENSION)
            return FALSE;
            
        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);
            
        pWndData->rcMove.left += nPenX - pWndData->nPrevX;
        pWndData->rcMove.bottom += nPenY - pWndData->nPrevY;
            
        break;
            
   case HTBOTTOMRIGHT :
            
        if (nPenX < pWndData->rcMove.left + MIN_DIMENSION)
            return FALSE;
            
        if (nPenY < pWndData->rcMove.top + MIN_DIMENSION)
            return FALSE;
            
        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);
            
        pWndData->rcMove.right += nPenX - pWndData->nPrevX;
        pWndData->rcMove.bottom += nPenY - pWndData->nPrevY;
            
        break;

    case HTCAPTION :
            
        // Get parent window position.
        GetWindowRect(hWnd, &pWndData->rcMove);

        // Hides the previous rectangle
        if (pWndData->bMove)
        {
            deltax = pWndData->nPrevX - pWndData->nOrgX;
            deltay = pWndData->nPrevY - pWndData->nOrgY;

            rect = pWndData->rcMove;

            rect.left += deltax;
            rect.top += deltay;
            rect.right += deltax;
            rect.bottom += deltay;

            DrawMovingRect(hWnd, &rect, dwStyle);
        }
        else
            pWndData->bMove = TRUE;

        // Draws new rectangle

        rect = pWndData->rcMove;

        deltax = nPenX - pWndData->nOrgX;
        deltay = nPenY - pWndData->nOrgY;

        rect.left += deltax;
        rect.top += deltay;
        rect.right += deltax;
        rect.bottom += deltay;

        pWndData->rcMove = rect;

        break;

    default :

        return FALSE;
    }

    DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);
    
    pWndData->nPrevX = nPenX;
    pWndData->nPrevY = nPenY;

    return TRUE;

#else   // NODWMOVESIZE

    return FALSE;

#endif  // NODWMOVESIZE
}

static BOOL DEFWND_NCLBtnUp(HWND hWnd, PWNDDATA pWndData, LPARAM lParam)
{
    int     nPenX, nPenY;
    RECT    rect, rcBorder, rcClient, rcWindow;
    HDC     hdc;

#if (!NODWMOVESIZE)

    int deltax, deltay;
    DWORD dwStyle;

#endif  // NODWMOVESIZE

    // The data of the default window isn't initialized, can't response
    // to the WM_NCPENMOVE message, just return.
    if (!pWndData)
        return FALSE;

    // 下面语句之后不需要再判断窗口是否可以变大小, 因为响应WM_NCLBUTTONDOWN
    // 时已经判断, 如果窗口不能变大小, nPenDown为HTNOWHERE将直接返回
    if (pWndData->nPenDown == HTNOWHERE)
        return FALSE;

    nPenX = LOWORD(lParam);
    nPenY = HIWORD(lParam);

    switch (pWndData->nPenDown)
    {
    case HTCLOSE:

        hdc = GetWindowDC(hWnd);

        // in the close button
        if (SetCloseBoxRect(hWnd, nPenX, nPenY, &rcBorder))
            PostMessage(hWnd, WM_CLOSE, 0, 0);

        DrawCloseBox(hWnd, hdc, &rcBorder, FALSE);

        ReleaseDC(hWnd, hdc);

        // Release mouse capture and reset nPenDown state
        ReleaseCapture();
        pWndData->nPenDown = HTNOWHERE;

        break;

    case HTVSCROLL:
            
        hdc = GetWindowDC(hWnd);

        GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);
        GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);

        SetRect(&rect, rcClient.right, rcClient.top, 
                rcClient.right + GetSystemMetrics(SM_CXVSCROLL),
                rcClient.bottom);
  
        // Release mouse capture and reset nPenDown state
        ReleaseCapture();
        pWndData->nPenDown = HTNOWHERE;

        SB_PenProcess(hWnd, hdc, pWndData->pVScrollData, &rect, 
            (UINT)WM_PENUP, 
            MAKELPARAM(nPenX - rcWindow.left, nPenY - rcWindow.top));

        ReleaseDC(hWnd, hdc);

        break;
            
    case HTHSCROLL:

        hdc = GetWindowDC(hWnd);

        GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);
        GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);

        SetRect(&rect, rcClient.left, rcClient.bottom,
                rcClient.right, 
                rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));

        // Release mouse capture and reset nPenDown state
        ReleaseCapture();
        pWndData->nPenDown = HTNOWHERE;

        SB_PenProcess(hWnd, hdc, pWndData->pHScrollData, &rect, 
            (UINT)WM_PENUP, 
            MAKELPARAM(nPenX - rcWindow.left, nPenY - rcWindow.top));

        ReleaseDC(hWnd, hdc);

        break;

#if (!NODWMOVESIZE)

    case HTCAPTION:

        // Get window position.
        GetWindowRect(hWnd, &pWndData->rcMove);

        // Hides the track rectangle
        if (pWndData->bMove)
        {
            deltax = pWndData->nPrevX - pWndData->nOrgX;
            deltay = pWndData->nPrevY - pWndData->nOrgY;

            rect = pWndData->rcMove;

            rect.left += deltax;
            rect.top += deltay;
            rect.right += deltax;
            rect.bottom += deltay;

            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            DrawMovingRect(hWnd, &rect, dwStyle);

            // Move window to new position
            deltax = nPenX - pWndData->nOrgX;
            deltay = nPenY - pWndData->nOrgY;
            
            rect = pWndData->rcMove;
            
            rect.left += deltax;
            rect.top += deltay;
            rect.right += deltax;
            rect.bottom += deltay;
            
            if (deltax != 0 || deltay != 0)
            {
                ScreenToParent(hWnd, &rect);
                MoveWindow(hWnd, rect.left, rect.top, 
                    rect.right - rect.left, rect.bottom - rect.top, TRUE);
            }
        }
        
        // Clip cursor to screen
        ClipCursor(NULL);
        
        // Release mouse capture and reset nPenDown state
        ReleaseCapture();
        pWndData->nPenDown = HTNOWHERE;

        EndDrag(hWnd);
            
        break;

    default:

        // Clears the moving window
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        DrawMovingRect(hWnd, &pWndData->rcMove, dwStyle);

        ScreenToParent(hWnd, &pWndData->rcMove);

        MoveWindow(hWnd, pWndData->rcMove.left, pWndData->rcMove.top, 
            pWndData->rcMove.right - pWndData->rcMove.left, 
            pWndData->rcMove.bottom - pWndData->rcMove.top, TRUE);

        // Clip cursor to screen
        ClipCursor(NULL);
        
        // Release mouse capture and reset nPenDown state
        ReleaseCapture();
        pWndData->nPenDown = HTNOWHERE;

        EndDrag(hWnd);
        
        break;

#endif  // NODWMOVESIZE
    }
        
    return TRUE;
}

#if (!NOCURSOR)

/*
**  Function : DEFWND_SetCursor
**  Purpose  :
**      First forward WM_SETCURSOR message to parent. If it returns TRUE
**      (i.e., it set the cursor), just return. Otherwise, set the cursor
**      based on codeHT and msg.
*/
static BOOL DEFWND_SetCursor(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    HWND    hwndParent;
    int     nHittest;
    long    dwStyle;
    BOOL    bThickFrame;
    LRESULT lResult;
    HCURSOR hcurClass;
    
    // First give the parent a chance to process the message.
    hwndParent = GetParent(hWnd);
    if (!hwndParent)    // parent window is null
//        hwndParent = GetDesktopWindow();
        hwndParent = HWND_DESKTOP;

    lResult = SendMessage(hwndParent, WM_SETCURSOR, wParam, lParam);
    if (lResult != 0)
        return TRUE;

    // If the the window isn't the cursor window, return FALSE 
    // indicates that the cursor isn't setted
    if ((HWND)wParam != hWnd)
        return FALSE;

    nHittest = LOWORD(lParam);  // hit-test code 
    if (nHittest == HTERROR)
    {
        MessageBeep((UINT)-1);
        return FALSE;
    }

    // when the border is thinBorder, don't enlarge/shrink the window.
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    bThickFrame = (dwStyle & WS_DLGFRAME) == WS_THICKFRAME;

    switch (nHittest)
    {
    case HTLEFT : 
    case HTRIGHT :          // set the current curosr to IDC_SIZEWE

        if (bThickFrame)
            SetCursor(LoadCursor(NULL, IDC_SIZEWE));
        else
            SetCursor(LoadCursor(NULL, IDC_ARROW));

        break;
            
    case HTTOP : 
    case HTBOTTOM :         // set the current curosr to IDC_SIZENS

        if (bThickFrame)
            SetCursor(LoadCursor(NULL, IDC_SIZENS));
        else
            SetCursor(LoadCursor(NULL, IDC_ARROW));

        break;
            
    case HTTOPLEFT : 
    case HTBOTTOMRIGHT :    // set the current curosr to IDC_SIZENWSE
           
        if (bThickFrame)            
            SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
        else
            SetCursor(LoadCursor(NULL, IDC_ARROW));

        break;
            
    case HTTOPRIGHT :       
    case HTBOTTOMLEFT :     // set the current curosr to IDC_SIZENESW
            
        if (bThickFrame)            
            SetCursor(LoadCursor(NULL, IDC_SIZENESW));
        else
            SetCursor(LoadCursor(NULL, IDC_ARROW));

        break;

    case HTCLIENT:

        hcurClass = (HCURSOR)GetClassLong(hWnd, GCL_HCURSOR);
        if (hcurClass)
            SetCursor(hcurClass);

        break;

    default :

        SetCursor(LoadCursor(NULL, IDC_ARROW));
        break;
    }

    return TRUE;
}

#endif // NOCURSOR

#if (!NODWMOVESIZE)

/*
**  Function : DrawMovingRect
**  Purpose  :
*/
static void DrawMovingRect(HWND hWnd, PRECT pRect, long lStyle)
{
    int MR_WIDTH;
    int MR_HEIGHT;

    RECT    rect, rect0;
    HWND    hwndParent;
    int     width, height;
    HDC     hdc;
    long    dwStyle;
    COLORREF crOldBk;
    int     nOldBkMode;

    ASSERT(hWnd != NULL);

    hwndParent = GetParent(hWnd);
    if (!hwndParent)    // parent window is null
//        hwndParent = GetDesktopWindow();
        hwndParent = HWND_DESKTOP;

    hdc = GetDC(hwndParent);

    if (!(lStyle & WS_THICKFRAME) && 
        ((lStyle & WS_DLGFRAME) != WS_DLGFRAME)) // thin border
    {
        MR_WIDTH = 1;
        MR_HEIGHT = 1;
    }
    else
    {
        MR_WIDTH = 4;
        MR_HEIGHT = 4;
    }

    rect0 = *pRect;
    ScreenToParent(hWnd, &rect0);

    rect = rect0;

    width = rect.right - rect.left;
    height = rect.bottom - rect.top - 2 * MR_HEIGHT;

//    hbrDot = CreateBrush(BS_HATCHED, RGB(0, 0, 0), HS_SIEVE);
//   hbrOld = SelectObject(hdc, hbrDot);

    crOldBk = SetBkColor(hdc, RGB(255, 255, 255));
    nOldBkMode = SetBkMode(hdc, OPAQUE);

    // delete the attribute WS_CLIPCHILDREN of the parent window
    dwStyle = GetWindowLong(hwndParent, GWL_STYLE);
    SetWindowLong(hwndParent, GWL_STYLE, dwStyle & ~WS_CLIPCHILDREN);

    DrawDragFrame(hdc, &rect);
    
/*
    rect.right = rect.left + width;
    rect.bottom = rect.top + MR_HEIGHT;
    PatBlt(hdc, rect.left, rect.top, width, MR_HEIGHT, ROP_SRC_XOR_DST);
    
    rect.top = rect0.bottom - MR_HEIGHT;
    rect.bottom = rect0.bottom;
    PatBlt(hdc, rect.left, rect.top, width, MR_HEIGHT, ROP_SRC_XOR_DST);
    
    rect.top = rect0.top + MR_HEIGHT;
    rect.right = rect0.left + MR_WIDTH;
    rect.bottom = rect.top + height;
    PatBlt(hdc, rect.left, rect.top, MR_WIDTH, height, ROP_SRC_XOR_DST);
    
    rect.left = rect0.right - MR_WIDTH;
    rect.top = rect0.top + MR_HEIGHT;
    rect.right = rect0.right;
    rect.bottom = rect.top + height;
    PatBlt(hdc, rect.left, rect.top, MR_WIDTH, height, ROP_SRC_XOR_DST);
*/
    
    // restore the attribute WS_CLIPCHILDREN of the parent window.
    SetWindowLong(hwndParent, GWL_STYLE, dwStyle);

//    SelectObject(hdc, hbrOld);
//    DeleteObject(hbrDot);

    // Restores the background color and background mode 
    SetBkColor(hdc, crOldBk);
    SetBkMode(hdc, nOldBkMode);

    ReleaseDC(hwndParent, hdc);
}

static void ScreenToParent(HWND hWnd, PRECT pRect)
{
    RECT rect;
    HWND hwndParent;

    ASSERT(hWnd != NULL);
    ASSERT(pRect != NULL);

    // get the parent window of the current window
    hwndParent = GetParent(hWnd);
    if (!hwndParent)    // parent window is null
//        hwndParent = GetDesktopWindow();
        hwndParent = HWND_DESKTOP;

    GetWindowRectEx(hwndParent, &rect, W_CLIENT, XY_SCREEN);

    pRect->left -= rect.left;
    pRect->top -= rect.top;
    pRect->right -= rect.left;
    pRect->bottom -= rect.top;
}

#endif  // NODWMOVESIZE

/*************************************************************************/
/*      Internal functions to response message sent to DefWindowProc     */ 
/*************************************************************************/

/*
**  Function : DoNCCalcSize
**  Purpose  :
*/
static void DoNCCalcSize(HWND hWnd, PWNDDATA pWndData, WPARAM wParam, 
                         LPARAM lParam)
{
    BOOL fCalcValidRects;
    PNCCALCSIZE_PARAMS pncsp;
    DWORD dwStyle;
    int nBorderW, nBorderH;
    int nCaptionH;
    RECT rcClient;
    PRECT prcWindow;

#if (!NOMENUS)
    int nMenuBarGap;
#endif

    //
    // wParam = fCalcValidRects
    // lParam = LPRECT rgrc[3]:
    //        lprc[0] = rcWindowNew = New window rectangle
    //    if fCalcValidRects:
    //        lprc[1] = rcWindowOld = Old window rectangle
    //        lprc[2] = rcClientOld = Old client rectangle
    //
    // On return:
    //        rgrc[0] = rcClientNew = New client rect
    //    if fCalcValidRects:
    //        rgrc[1] = rcValidDst  = Destination valid rectangle
    //        rgrc[2] = rcValidSrc  = Source valid rectangle
    //
    
    fCalcValidRects = (BOOL)wParam;
    if (fCalcValidRects)
        pncsp = (PNCCALCSIZE_PARAMS)lParam;
    
    // 获得新的窗口矩形(父窗口坐标)
    prcWindow = (PRECT)lParam;
    
    // 计算边框的大小
    dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
    if ((dwStyle & WS_DLGFRAME) == WS_DLGFRAME)
    {
        nBorderW = GetSystemMetrics(SM_CXDLGFRAME);
        nBorderH = GetSystemMetrics(SM_CYDLGFRAME);
    }
    else if (dwStyle & WS_BORDER)
    {
        nBorderW = GetSystemMetrics(SM_CXBORDER);
        nBorderH = GetSystemMetrics(SM_CYBORDER);
    }
    else if (dwStyle & WS_THICKFRAME)
    {
        nBorderW = GetSystemMetrics(SM_CXFRAME);
        nBorderH = GetSystemMetrics(SM_CYFRAME);
    }
    else
    {
        nBorderW = 0;
        nBorderH = 0;
    }

#if (INTERFACE_MONO && FOCUSBORDER)

    // 单色界面时使用fucus border表示具有WS_TABSTOP风格的子窗口的键盘焦点
    // 的转移
//    if ((dwStyle & WS_CHILD) && (dwStyle & WS_TABSTOP))
//    {
//        nBorderW += 1;
//        nBorderH += 1;
//        nBorderW += 2;
//        nBorderH += 2; //手机项目为2
//    }

#endif // defined(INTERFACE_MONO) && defined(FOCUSBORDER)
    
    if (dwStyle & WS_CAPTION)
        nCaptionH = GetSystemMetrics(SM_CYCAPTION);
    else
        nCaptionH = 0;

#if (!NOMENUS)
    if (pWndData->hwndMenuBar)
    {
#if (INTERFACE_MONO)
        nMenuBarGap = 1;
#else  // INTERFACE_MONO
        if (GetDeviceCaps(NULL, BITSPIXEL) == 1)
            nMenuBarGap = 1;
        else
            nMenuBarGap = 2;
#endif // INTERFACE_MONO
    }
    else
        nMenuBarGap = 0;
#endif // NOMENUS
    
    // 计算新的客户区矩形
    
    rcClient.left = prcWindow->left + nBorderW;
    rcClient.top = prcWindow->top + nBorderH + nCaptionH;
    
#if (!NOMENUS)
    
    if (pWndData->hwndMenuBar)
    {
        // Caculate the height of the separator between the menu and 
        // the client area.
        rcClient.top += GetSystemMetrics(SM_CYMENU) + nMenuBarGap;

#if (!INTERFACE_MONO)
        if (GetDeviceCaps(NULL, BITSPIXEL) > 1)
            rcClient.left += 2;
#endif  // INTERFACE_MONO
    }
    
#endif // NOMENUS
    
    if (rcClient.left >= prcWindow->right)
        rcClient.left = prcWindow->right - 1;
    if (rcClient.top >= prcWindow->bottom)
        rcClient.top = prcWindow->bottom - 1;
    
    rcClient.right = prcWindow->right - nBorderW;
    rcClient.bottom = prcWindow->bottom - nBorderH;
    
    // 有垂直滚动条并且垂直滚动条不处于隐藏状态
    if (pWndData->pVScrollData)
    {
        if (SB_IsEnabled(pWndData->pVScrollData, NULL) || 
            pWndData->bDisableNoVScroll)
            rcClient.right -= GetSystemMetrics(SM_CXVSCROLL);
    }
    
    // 有水平滚动条并且水平滚动条不处于隐藏状态
    if (pWndData->pHScrollData)
    {
        if (SB_IsEnabled(pWndData->pHScrollData, NULL) || 
            pWndData->bDisableNoHScroll)
            rcClient.bottom -= GetSystemMetrics(SM_CYHSCROLL);
    }
    
    if (rcClient.right < rcClient.left)
        rcClient.right = rcClient.left;
    if (rcClient.bottom < rcClient.top)
        rcClient.bottom = rcClient.top;
    
    // 通知滚动条大小发生改变
    
    if (pWndData->pVScrollData)
    {
        SB_Size(pWndData->pVScrollData, 
            rcClient.bottom - rcClient.top);
    }
    
    if (pWndData->pHScrollData)
    {
        SB_Size(pWndData->pHScrollData, 
            rcClient.right - rcClient.left);
    }
    
#if (!NOMENUS)
    
    // 如果需要移动菜单子窗口
    if (pWndData->hwndMenuBar && fCalcValidRects)
    {
        RECT rcMenu;
        int x, y, width, height;
        
        // 获得菜单窗口当前客户区矩形
        GetClientRect(pWndData->hwndMenuBar, &rcMenu);
        
        // 计算菜单窗口新的大小
        x = nBorderW;
        y = nBorderH + nCaptionH;
        
        width = prcWindow->right - prcWindow->left - nBorderW * 2;
        if (width < 1)
            width = 1;
        
        height = rcClient.top - nMenuBarGap - prcWindow->top - y;
        if (height < 1)
            height = 1;
        
        if (width != rcMenu.right || height != rcMenu.bottom)
        {
            MoveWindow(pWndData->hwndMenuBar, x, y, width, height, 
                FALSE);
        }
    }
    
    
#endif // NOMENUS
    
    // 将新的客户区矩形返回
    *prcWindow = rcClient;
}

/*
**  Function : DoNCPaint
**  Purpose  :
*/
static void DoNCPaint(HWND hWnd, PWNDDATA pWndData, int nPart, BOOL bActive)
{
    #define MAX_WINDOW_TEXT 128             // Buffer size for window text
    char     achTextBuf[MAX_WINDOW_TEXT];   // Buffer for window text

    COLORREF crOldText, crOldBk;
    DWORD    dwStyle;
    RECT     rect, rcWindow, rcClient;
    RECT     rcBorder;
    int      nBorderWidth, nTextLen;
    HDC      hdc;

    if (!pWndData)
        return;

    hdc = GetWindowDC(hWnd);

    GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_WINDOW);

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

#if (INTERFACE_MONO && FOCUSBORDER)

    // 单色界面时使用fucus border表示具有WS_TABSTOP风格的子窗口的键盘焦点
    // 的转移
    if ((nPart == NC_FOCUSBORDER) || (nPart == NC_ALL))
    {
//        if ((dwStyle & WS_CHILD) && (dwStyle & WS_TABSTOP))
//        {
///*
//            HWND hwndFocus;
//
//            hwndFocus = GetFocus();
//
//            if ((hwndFocus == hWnd) || (GetParent(hwndFocus) == hWnd))
//                SelectObject(hdc, GetStockObject(BLACK_PEN));
//            else
//                SelectObject(hdc, GetStockObject(WHITE_PEN));
//            
//            SelectObject(hdc, GetStockObject(NULL_BRUSH));
//            
//            DrawRect(hdc, &rcWindow);
//            
//            InflateRect(&rcWindow, -1, -1);
//*/
//            HWND hwndFocus;
//
//            hwndFocus = GetFocus();
//
//			SelectObject(hdc, GetStockObject(NULL_BRUSH));
//                
//            if ((hwndFocus == hWnd) || (GetParent(hwndFocus) == hWnd))
//			{
//				hOldPen = SelectObject(hdc, GetStockObject(WHITE_PEN));
//				DrawLine(hdc, rcWindow.left, rcWindow.top, rcWindow.right,
//					rcWindow.top);
//				DrawLine(hdc, rcWindow.left, rcWindow.top, rcWindow.left,
//					rcWindow.bottom);
//                SelectObject(hdc, GetStockObject(LTGRAY_PEN));
//                DrawLine(hdc, rcWindow.left + 1, rcWindow.top + 1, 
//					rcWindow.left + 1,	rcWindow.bottom);
//				DrawLine(hdc, rcWindow.left + 1, rcWindow.top + 1, 
//					rcWindow.right, rcWindow.top + 1);
//				SelectObject(hdc, GetStockObject(BLACK_PEN));
//				DrawLine(hdc, rcWindow.left + 1, rcWindow.bottom - 1, 
//					rcWindow.right,	rcWindow.bottom - 1);
//				DrawLine(hdc, rcWindow.right- 1, rcWindow.top + 1, 
//					rcWindow.right - 1, rcWindow.bottom);
//				SelectObject(hdc, GetStockObject(GRAY_PEN));
//				DrawLine(hdc, rcWindow.left + 1, rcWindow.bottom - 2, 
//					rcWindow.right,	rcWindow.bottom - 2);
//				DrawLine(hdc, rcWindow.right- 2, rcWindow.top + 1, 
//					rcWindow.right - 2, rcWindow.bottom - 1);
//				SelectObject(hdc, hOldPen);
//				//DrawRect(hdc, &rcWindow);
//			}
//            else
//			{
//				hOldPen = SelectObject(hdc, GetStockObject(WHITE_PEN));
//				DrawRect(hdc, &rcWindow);
//                DrawLine(hdc, rcWindow.left + 1, rcWindow.top, 
//					rcWindow.left + 1,	rcWindow.bottom);
//				DrawLine(hdc, rcWindow.left, rcWindow.top + 1, 
//					rcWindow.right, rcWindow.top + 1);
//				DrawLine(hdc, rcWindow.left, rcWindow.bottom - 2, 
//					rcWindow.right,	rcWindow.bottom - 2);
//				DrawLine(hdc, rcWindow.right- 2, rcWindow.top, 
//					rcWindow.right - 2, rcWindow.bottom - 1);
//				SelectObject(hdc, hOldPen);
//			}
//            InflateRect(&rcWindow, -2, -2);
//        }

        if (nPart == NC_FOCUSBORDER)
        {
            ReleaseDC(hWnd, hdc);
            return;
        }
    }

#endif // defined(INTERFACE_MONO) && defined(FOCUSBORDER)
    
    // Gets border width
    if ((dwStyle & WS_DLGFRAME) == WS_DLGFRAME)
        nBorderWidth = GetSystemMetrics(SM_CXDLGFRAME);
    else if (dwStyle & WS_THICKFRAME)
        nBorderWidth = GetSystemMetrics(SM_CXFRAME);
    else if (dwStyle & WS_BORDER)
        nBorderWidth = GetSystemMetrics(SM_CXBORDER);
    else 
        nBorderWidth = 0;

    InflateRect(&rcWindow, -nBorderWidth, -nBorderWidth);
    
    GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

    // Paints the caption if nesseary

    if (dwStyle & WS_CAPTION)
    {
        RECT rcText;

        if (nPart != NC_TEXT)
        {   
            // 1 stands for the line between the title and the client
            SetRect(&rect, rcWindow.left, rcWindow.top, rcWindow.right, 
                rcWindow.top + GetSystemMetrics(SM_CYCAPTION) - 1); 
        }
        else
        {
            SetRect(&rect, rcWindow.left, rcWindow.top, 
                rcWindow.right - CLOSE_BOX_WIDTH - CLOSE_BOX_GAP_X - 4, 
                rcWindow.top + GetSystemMetrics(SM_CYCAPTION) - 1);
        }

        SetRect(&rcText, rcWindow.left, rcWindow.top, 
            rcWindow.right - CLOSE_BOX_WIDTH - CLOSE_BOX_GAP_X - 4, 
            rcWindow.top + GetSystemMetrics(SM_CYCAPTION) - 1);
        
        if (bActive)
        {
            crOldText = SetTextColor(hdc, GetSysColor(COLOR_CAPTIONTEXT));
            crOldBk = SetBkColor(hdc, GetSysColor(COLOR_ACTIVECAPTION));
        }
        else
        {
            crOldText = SetTextColor(hdc, 
                GetSysColor(COLOR_INACTIVECAPTIONTEXT));
            crOldBk = SetBkColor(hdc, 
                GetSysColor(COLOR_INACTIVECAPTION));
        }

        // Gets the window text by calling GetWindowTextLength and 
        // GetWindowText instead of using pWndData->pszText directly, 
        // because the window proc maybe process WM_SETTEXT itself.
        nTextLen = GetWindowTextLength(hWnd);
        if (nTextLen > MAX_WINDOW_TEXT - 1)
            nTextLen = MAX_WINDOW_TEXT - 1;
        GetWindowText(hWnd, achTextBuf, MAX_WINDOW_TEXT);

        // Clear the caption and draw window text, the first DrawText is 
        // used to only clear the caption
//        DrawText(hdc, achTextBuf, 0, &rect, DT_CLEAR);
//        DrawText(hdc, achTextBuf, nTextLen, &rcText, DT_LEFT | DT_VCENTER);
        DrawText(hdc, achTextBuf, 0, &rect, DT_CLEAR);
        DrawText(hdc, achTextBuf, nTextLen, &rcText, DT_HCENTER | DT_VCENTER); //DT_LEFT
        // Restore the old text color and back color
        SetTextColor(hdc, crOldText);
        SetBkColor(hdc, crOldBk);

        // Only update window text, just return
        if (nPart == NC_TEXT)
        {
            ReleaseDC(hWnd, hdc);
            return;
        }
        
        // Draws other parts of window caption

#if (INTERFACE_MONO)
        SelectObject(hdc, GetStockObject(BLACK_PEN));
#else // INTERFACE_MONO
        if (GetDeviceCaps(NULL, BITSPIXEL) == 1)    // mono color
            SelectObject(hdc, GetStockObject(BLACK_PEN));
        else
            SelectObject(hdc, GetStockObject(GRAY_PEN));
#endif // INTERFACE_MONO

        DrawLine(hdc, rect.left, rect.bottom, rect.right, rect.bottom);
        
        // Draws the close box button if the CS_NOCLOSE class style is 
        // not specified.
        if (!(GetClassLong(hWnd, GCL_STYLE) & CS_NOCLOSE))
        {
            SetRect(&rcBorder, 
                rect.right - CLOSE_BOX_WIDTH - CLOSE_BOX_GAP_X - 2,
                rect.top + CLOSE_BOX_GAP_Y - 1, 
                rect.right - CLOSE_BOX_GAP_X, 
                rect.top + CLOSE_BOX_GAP_Y + CLOSE_BOX_HEIGHT);
            DrawCloseBox(hWnd, hdc, &rcBorder, FALSE);
        }
    }

    // Only draw caption, return
    if (nPart != NC_ALL)
    {
        ReleaseDC(hWnd, hdc);
        return;
    }

#if (!NOMENUS)
  
    // paint separator below menu bar if there is a menu bar
    if (pWndData->hwndMenuBar)
    {
        // paint a separator line between the menu and the client area

#if (INTERFACE_MONO)

        SelectObject(hdc, GetStockObject(BLACK_PEN));
        DrawLine(hdc, rcWindow.left, rcClient.top - 1, rcWindow.right,
            rcClient.top - 1);

#else // INTERFACE_MONO

        if (GetDeviceCaps(NULL, BITSPIXEL) == 1)    // mono color
        {
            SelectObject(hdc, GetStockObject(BLACK_PEN));
            DrawLine(hdc, rcWindow.left, rcClient.top - 1, rcWindow.right,
                rcClient.top - 1);
        }
        else
        {
            // draw two darkgray lines
            SelectObject(hdc, GetStockObject(GRAY_PEN));
            DrawLine(hdc, rcWindow.left, rcClient.top - 2, rcWindow.right,
                rcClient.top - 2);
            DrawLine(hdc, rcWindow.left, rcClient.top - 2, rcWindow.left, 
                rcWindow.bottom);

            // draw two black lines
            SelectObject(hdc, GetStockObject(BLACK_PEN));
            DrawLine(hdc, rcWindow.left + 1, rcClient.top - 1, 
                rcWindow.right, rcClient.top - 1);
            DrawLine(hdc, rcWindow.left + 1, rcClient.top - 1, 
                rcWindow.left + 1, rcWindow.bottom);
        }

#endif // INTERFACE_MONO
    }

#endif

    // paint vertical scrollbar
    if (pWndData->pVScrollData && (pWndData->bDisableNoVScroll || 
        SB_IsEnabled(pWndData->pVScrollData, NULL)))
    {
        SetRect(&rect, rcClient.right, rcClient.top, 
                rcClient.right + GetSystemMetrics(SM_CXVSCROLL),
                rcClient.bottom);
        SB_Paint(hdc, pWndData->pVScrollData, &rect);
    }

    // paint horizontal scrollbar
    if (pWndData->pHScrollData && (pWndData->bDisableNoHScroll || 
        SB_IsEnabled(pWndData->pHScrollData, NULL)))
    {
        SetRect(&rect, rcClient.left, rcClient.bottom,
                rcClient.right, 
                rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));
        SB_Paint(hdc, pWndData->pHScrollData, &rect);
    }

    // paint the rect of intersection of two scrollbars
    if (dwStyle & WS_VSCROLL && dwStyle & WS_HSCROLL)
    {
        SetRect(&rect, rcClient.right, rcClient.bottom,
                rcClient.right + GetSystemMetrics(SM_CXVSCROLL), 
                rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));
#if (INTERFACE_MONO)
        ClearRect(hdc, &rect, GetSysColor(COLOR_WINDOW));
#else // INTERFACE_MONO
        if (GetDeviceCaps(NULL, BITSPIXEL) == 1)
            ClearRect(hdc, &rect, GetSysColor(COLOR_WINDOW));
        else
            ClearRect(hdc, &rect, GetSysColor(COLOR_SCROLLBAR));
#endif // INTERFACE_MONO
    }

    // Paints the border if border width isn't 0
    if (nBorderWidth != 0)
    {
        // Restores the rcWindow to window size
        InflateRect(&rcWindow, nBorderWidth, nBorderWidth);

        if (nBorderWidth == GetSystemMetrics(SM_CXBORDER))  // thin border
        {
            SelectObject(hdc, GetStockObject(BLACK_PEN));
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            DrawRect(hdc, &rcWindow);
        }
        else    // thick frame or dialog frame
        {
            HPEN hpenBorder, hpenOld;

#if (INTERFACE_MONO)

            hpenBorder = CreatePen(PS_INSIDEFRAME, nBorderWidth, 
                COLOR_BLACK);
            hpenOld = SelectObject(hdc, hpenBorder);
            
            // Sets the brush to null brush
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            
            DrawRect(hdc, &rcWindow);
            
            SelectObject(hdc, hpenOld);
            DeleteObject(hpenBorder);

#else // INTERFACE_MONO

            if (GetDeviceCaps(NULL, BITSPIXEL) == 1)   // mono device
            {
                hpenBorder = CreatePen(PS_INSIDEFRAME, nBorderWidth, 
                    COLOR_BLACK);
                hpenOld = SelectObject(hdc, hpenBorder);
                
                // Sets the brush to null brush
                SelectObject(hdc, GetStockObject(NULL_BRUSH));
                
                DrawRect(hdc, &rcWindow);
                
                SelectObject(hdc, hpenOld);
                DeleteObject(hpenBorder);
            }
            else    // color device
            {
                // Sets the brush to null brush
                SelectObject(hdc, GetStockObject(NULL_BRUSH));

                // Sets the pen to light gray pen
                SelectObject(hdc, GetStockObject(LTGRAY_PEN));
                
                // draw left vertical lightgray line
                DrawLine(hdc, rcWindow.left, rcWindow.top, 
                    rcWindow.left, rcWindow.bottom - 1);
                
                // draw top horizontal lightgray line
                DrawLine(hdc, rcWindow.left + 1, rcWindow.top, 
                    rcWindow.right - 1, rcWindow.top);
                
                // draw two lightgray rect in the internal part
                SetRect(&rcBorder, rcWindow.left + 2, rcWindow.top + 2, 
                    rcWindow.right - 2, rcWindow.bottom - 2);
                
                DrawRect(hdc, &rcBorder);
                
                SetRect(&rcBorder, rcWindow.left + 3, rcWindow.top + 3, 
                    rcWindow.right - 3, rcWindow.bottom - 3);
                DrawRect(hdc, &rcBorder);
                
                SelectObject(hdc, GetStockObject(BLACK_PEN));
                
                // draw right vertical black line
                DrawLine(hdc, rcWindow.right - 1, rcWindow.top, 
                    rcWindow.right - 1, rcWindow.bottom);
                
                // draw bottom horizontal black line
                DrawLine(hdc, rcWindow.left, rcWindow.bottom - 1, 
                    rcWindow.right - 1, rcWindow.bottom - 1);
                
                SelectObject(hdc, GetStockObject(WHITE_PEN));
                
                // draw left vertical white line
                DrawLine(hdc, rcWindow.left + 1, rcWindow.top + 1, 
                    rcWindow.left + 1, rcWindow.bottom - 2);
                
                // draw top horizontal white line
                DrawLine(hdc, rcWindow.left + 2, rcWindow.top + 1,
                    rcWindow.right - 2, rcWindow.top + 1);
                
                SelectObject(hdc, GetStockObject(GRAY_PEN));
                
                // draw right vertical darkgray line
                DrawLine(hdc, rcWindow.right - 2, rcWindow.top + 1, 
                    rcWindow.right - 2, rcWindow.bottom - 1);
                
                // draw bottom horizontal darkgray line
                DrawLine(hdc, rcWindow.left + 1, rcWindow.bottom - 2,
                    rcWindow.right - 2, rcWindow.bottom - 2);
            }
#endif // INTERFACE_MONO
        }
    }
       
    ReleaseDC(hWnd, hdc);
}

/*
**  Function : FindNCHit
**  Purpose  :
**      Determine what area the passed coordinate is in.
*/
static int FindNCHit(HWND hWnd, LPARAM lParam)
{
    DWORD dwStyle;
    RECT rcWindow, rcClient;
    RECT rect;
    int nBorderW, nBorderH;
    POINT pt; 

    pt.x = LOWORD(lParam);
    pt.y = HIWORD(lParam);

    GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);
    GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

    // Converts screen coordinate of the point to window coordinate
    pt.x -= rcWindow.left;
    pt.y -= rcWindow.top;

    // Converts screen coordinate of window rect to window coordinate
    OffsetRect(&rcWindow, -rcWindow.left, -rcWindow.top);

    if (!PtInRect(&rcWindow, pt))
        return HTNOWHERE;

    if (PtInRect(&rcClient, pt))
        return HTCLIENT;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if ((dwStyle & WS_DLGFRAME) == WS_DLGFRAME)
    {
        nBorderW = GetSystemMetrics(SM_CXDLGFRAME);
        nBorderH = GetSystemMetrics(SM_CYDLGFRAME);
    }
    else if (dwStyle & WS_BORDER)
    {
        nBorderW = GetSystemMetrics(SM_CXBORDER);
        nBorderH = GetSystemMetrics(SM_CYBORDER);
    }
    else if (dwStyle & WS_THICKFRAME)
    {
        nBorderW = GetSystemMetrics(SM_CXFRAME);
        nBorderH = GetSystemMetrics(SM_CYFRAME);
    }
    else
    {
        nBorderW = 0;
        nBorderH = 0;
    }

    InflateRect(&rcWindow, -nBorderW, -nBorderH);

    if (!PtInRect(&rcWindow, pt)) // the frame of the window is pressed down
    {   
#if (!NODWMOVESIZE)

        // top-left corner of the window
        if (pt.x < rcWindow.left + CORNER_OFFSET &&
            pt.y < rcWindow.top + CORNER_OFFSET)    
            return HTTOPLEFT;
        
        // bottom-left corner of the window
        if (pt.x < rcWindow.left + CORNER_OFFSET &&
            pt.y >= rcWindow.bottom - CORNER_OFFSET) 
            return HTBOTTOMLEFT;
        
        // top-right corner of the window
        if (pt.x >= rcWindow.right - CORNER_OFFSET &&
            pt.y < rcWindow.top + CORNER_OFFSET)    
            return HTTOPRIGHT;
        
        // bottom-right corner of the window
        if (pt.x > rcWindow.right - CORNER_OFFSET &&
            pt.y >= rcWindow.bottom - CORNER_OFFSET) 
            return HTBOTTOMRIGHT;

        if (pt.x < rcWindow.left)    // left border
            return HTLEFT;

        if (pt.x >= rcWindow.right)  // right border
            return HTRIGHT;

        if (pt.y < rcWindow.top)     // top border
            return HTTOP;

        if (pt.y >= rcWindow.bottom) // bottom border
            return HTBOTTOM;

#else   // NODWMOVESIZE
        return HTNOWHERE;
#endif  // NODWMOVESIZE
    }

    if (pt.y <= rcClient.top)       // the title bar is pressed down
    {
        if (dwStyle & WS_CAPTION)  // if there is a title bar in the window
        {
            SetRect(&rect, 
                rcWindow.right - CLOSE_BOX_WIDTH - CLOSE_BOX_GAP_X - 2, 
                rcWindow.top + CLOSE_BOX_GAP_Y - 1, 
                rcWindow.right - CLOSE_BOX_GAP_X, 
                rcWindow.top + CLOSE_BOX_GAP_Y + CLOSE_BOX_HEIGHT);
            
            // the close button in the title bar is pressed
            if (PtInRect(&rect, pt)) 
            {
                // 没有CS_NOCLOSE类风格才能返回HTCLOSE
                if (!(GetClassLong(hWnd, GCL_STYLE) & CS_NOCLOSE))
                    return HTCLOSE;
            }

            return HTCAPTION;
        }
    }
        
    if (dwStyle & WS_VSCROLL)
    {
        if (pt.x >= rcClient.right && 
            pt.x < rcClient.right + GetSystemMetrics(SM_CXVSCROLL) &&
            pt.y >= rcClient.top && pt.y < rcClient.bottom)
        {
            return HTVSCROLL;
        }
    }

    if (dwStyle & WS_HSCROLL)
    {
        if (pt.x >= rcClient.left && pt.x < rcClient.right &&
            pt.y >= rcClient.bottom && 
            pt.y < rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL))
        {
            return HTHSCROLL;
        }
    }

#if (!NODWMOVESIZE)

    if ((dwStyle & WS_VSCROLL) && (dwStyle & WS_HSCROLL))
    {
        if (pt.x >= rcClient.right && 
            pt.x < rcClient.right + GetSystemMetrics(SM_CXVSCROLL) &&
            pt.y >= rcClient.bottom && 
            pt.y < rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL))
        {
            return HTBOTTOMRIGHT;
        }
    }

#endif // NODWMOVESIZE

    return HTERROR;
}

/*
**  Function : DoCancelMode
**  Purpose  :
**      Terminate any modes the system might be in, such as scrollbar 
**      tracking, menu mode, button capture, etc.
*/
static void DoCancelMode(HWND hWnd, PWNDDATA pWndData)
{
    POINT   ptCursor;

    // The data of the default window isn't initialized, can't response
    // to the WM_NCPENMOVE message, just return.
    if (!pWndData)
        return;

    // 下面语句之后不需要再判断窗口是否可以变大小, 因为响应WM_NCLBUTTONDOWN
    // 时已经判断, 如果窗口不能变大小, nPenDown为HTNOWHERE将直接返回
    if (pWndData->nPenDown == HTNOWHERE)
        return;

    // 取当前的鼠标位置
    GetCursorPos(&ptCursor);

    // 发送WM_NCLBUTTONUP消息，使当前的鼠标操作正常结束并释放对鼠标的捕获
    SendMessage(hWnd, WM_NCLBUTTONUP, (WPARAM)HTNOWHERE, 
        MAKELPARAM(ptCursor.x, ptCursor.y));

    ASSERT(pWndData->nPenDown == HTNOWHERE);
}

/*
**  Function : DoNCActive
**  Purpose  :
**      Do non-client drawing in response to activation or deactivation.
*/
static void DoNCActivate(HWND hWnd, PWNDDATA pWndData, BOOL bActive)
{
#if (INTERFACE_MONO)
#else //INTERFACE_MONO
    // 单色和四级灰度情况下不需要重画NC区
    if (GetDeviceCaps(NULL, BITSPIXEL) <= 2)
        return;

    DoNCPaint(hWnd, pWndData, NC_CAPTION, bActive);
#endif // ITNERFACE_MONO
}

/*
**  Function : EraseBkgnd
**  Purpose  :
**      Response the WM_ERASEBKGND or WM_ICONERASEBKGND message to paint the 
**      window background.
*/
static BOOL EraseBkgnd(HWND hWnd, UINT message, HDC hdc)
{
    BOOL bRet;
    RECT rcClient;
    HBRUSH hbrBackground;

    if (message == WM_ERASEBKGND)
    {
        hbrBackground = (HBRUSH)GetClassLong(hWnd, GCL_HBRBACKGROUND);

        GetClientRect(hWnd, &rcClient);
        DPtoLP(hdc, (PPOINT)&rcClient, 2);

        if (hbrBackground)
        {
            FillRect(hdc, &rcClient, hbrBackground);
            bRet = TRUE;
        }
        else
        {
            FillRect(hdc, &rcClient, hbrTransBk);
            bRet = FALSE;
        }
    }
    else
    {
        // Erase
        bRet = FALSE;
    }

    return bRet;
}

static void DrawCloseBox(HWND hWnd, HDC hdc, const RECT* pRect, 
                           BOOL bDown)
{
#if (INTERFACE_MONO)

    if (!bDown)
    {
        BitBlt(hdc, pRect->left + 1, pRect->top + 1, CLOSE_BOX_WIDTH, 
            CLOSE_BOX_HEIGHT, (HDC)hbmpCloseBox, 0, 0, ROP_NSRC);
    }
    else
    {
        RECT rect;

        BitBlt(hdc, pRect->left + 1, pRect->top + 1, CLOSE_BOX_WIDTH, 
            CLOSE_BOX_HEIGHT, (HDC)hbmpCloseBox, 0, 0, ROP_SRC);
        
        SelectObject(hdc, GetStockObject(WHITE_PEN));
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        SetRect(&rect, pRect->left + 1, pRect->top + 1, 
            pRect->left + 1 + CLOSE_BOX_WIDTH, 
            pRect->top + 1 + CLOSE_BOX_HEIGHT);
        DrawRect(hdc, &rect);
    }
#else
    if (GetDeviceCaps(NULL, BITSPIXEL) == 1)
    {
        // Draw the close box bitmap
        if (!bDown)
        {
            BitBlt(hdc, pRect->left + 1, pRect->top + 1, CLOSE_BOX_WIDTH, 
                CLOSE_BOX_HEIGHT, (HDC)hbmpCloseBox, 0, 0, ROP_NSRC);
        }
        else
        {
            BitBlt(hdc, pRect->left + 1, pRect->top + 1, CLOSE_BOX_WIDTH, 
                CLOSE_BOX_HEIGHT, (HDC)hbmpCloseBox, 0, 0, ROP_SRC);

            SelectObject(hdc, GetStockObject(WHITE_PEN));
            SelectObject(hdc, GetStockObject(NULL_BRUSH));
            DrawRect(hdc, pRect);
        }
    }
    else
    {
        // Set the foreground and background color of the bitmap
        SetBkColor(hdc, RGB(0, 0, 0));
        SetTextColor(hdc, RGB(192, 192, 192));
        
        // Set the face of the button
        if (!bDown)
        {
            BitBlt(hdc, pRect->left + 1, pRect->top, CLOSE_BOX_WIDTH, 
                CLOSE_BOX_HEIGHT, (HDC)hbmpCloseBox, 0, 0, ROP_SRC);
        }
        else
        {
            BitBlt(hdc, pRect->left + 2, pRect->top + 1, CLOSE_BOX_WIDTH, 
                CLOSE_BOX_HEIGHT, (HDC)hbmpCloseBox, 0, 0, ROP_SRC);
        }
        
        // Draw left and top line
        
        if (!bDown)
            SelectObject(hdc, GetStockObject(WHITE_PEN));
        else
            SelectObject(hdc, GetStockObject(BLACK_PEN));
        
        DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom - 1);
        DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, 
            pRect->top);
        
        // Draw left and top white line or darkgray line
        
        if (!bDown)
            SelectObject(hdc, GetStockObject(LTGRAY_PEN));
        else
            SelectObject(hdc, GetStockObject(GRAY_PEN));
        
        DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,
            pRect->bottom - 2);
        DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right - 2, 
            pRect->top + 1);
        
        // Draw right and bottom gray line
        
        if (!bDown)
            SelectObject(hdc, GetStockObject(GRAY_PEN));
        else
            SelectObject(hdc, GetStockObject(LTGRAY_PEN));
        
        DrawLine(hdc, pRect->right - 2, pRect->top + 1, pRect->right - 2,
            pRect->bottom - 2);
        DrawLine(hdc, pRect->left + 1, pRect->bottom - 2, pRect->right - 1,
            pRect->bottom - 2);
        
        if (!bDown)
            SelectObject(hdc, GetStockObject(BLACK_PEN));
        else
            SelectObject(hdc, GetStockObject(WHITE_PEN));
        
        DrawLine(hdc, pRect->right - 1, pRect->top, pRect->right - 1,
            pRect->bottom - 1);
        DrawLine(hdc, pRect->left, pRect->bottom - 1, pRect->right,
            pRect->bottom - 1);
    }
#endif
}

static int SetCloseBoxRect(HWND hWnd, int nPenX, int nPenY, PRECT rect)
{
    DWORD dwStyle;
    RECT rcWindow;
    int nBorderWidth;
    POINT pt; 

    pt.x = nPenX;
    pt.y = nPenY;

    GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_SCREEN);

    // Converts screen coordinate of the point to window coordinate
    pt.x -= rcWindow.left;
    pt.y -= rcWindow.top;

    // Converts screen coordinate of window rect to window coordinate
    OffsetRect(&rcWindow, -rcWindow.left, -rcWindow.top);

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if ((dwStyle & WS_DLGFRAME) == WS_DLGFRAME)
        nBorderWidth = GetSystemMetrics(SM_CXDLGFRAME);
    else if (dwStyle & WS_BORDER)
        nBorderWidth = GetSystemMetrics(SM_CXBORDER);
    else if ((dwStyle & WS_THICKFRAME) == WS_THICKFRAME)
        nBorderWidth = GetSystemMetrics(SM_CXFRAME);
    else
        nBorderWidth = 0;

    InflateRect(&rcWindow, -nBorderWidth, -nBorderWidth);

    SetRect(rect, 
        rcWindow.right - CLOSE_BOX_WIDTH - CLOSE_BOX_GAP_X - 2, 
        rcWindow.top + CLOSE_BOX_GAP_Y - 1, 
        rcWindow.right - CLOSE_BOX_GAP_X, 
        rcWindow.top + CLOSE_BOX_GAP_Y + CLOSE_BOX_HEIGHT);

    if (PtInRect(rect, pt)) // the close button in the title bar is pressed
        return HTCLOSE;

    return 0;

}

/*
**  Function : DEFWND_SetScrollInfo
**  Purpose  :
**      Set the scrollbar flag to PWINDATA
*/
int DEFWND_SetScrollInfo(HWND hWnd, int nBar, PCSCROLLINFO pScrollInfo,
                         BOOL bRedraw)
{
    PWNDDATA pWndData;
    RECT     rcClient, rect, rcWindow;
    HDC      hdc;
    int      nRet = 0;
    BOOL     bMove = FALSE;
    
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);
    ASSERT(pScrollInfo != NULL);

    // Gets the internal data for default window
    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return FALSE;

    if (nBar == SB_VERT && pWndData->pVScrollData)
    {
        // 判断是enabled, 判断滚动条是否会消失
        if (SB_IsEnabled(pWndData->pVScrollData, NULL))
        {
            if (!SB_IsEnabled(pWndData->pVScrollData, pScrollInfo))
            {
                if (pScrollInfo->fMask & SIF_DISABLENOSCROLL)
                    pWndData->bDisableNoVScroll = TRUE;
                else
                {
                    // Show ==> Hide

                    pWndData->bDisableNoVScroll = FALSE;

                    // 滚动条将消失, 不需要重画
                    bRedraw = FALSE;

                    // 滚动条将消失, 需要移动窗口
                    bMove = TRUE;
                }
            }
        }
        else
        {
            // 滚动条是禁止的

            if (SB_IsEnabled(pWndData->pVScrollData, pScrollInfo))
            {
                // 禁止 ==> 非禁止

                if (!pWndData->bDisableNoVScroll)
                {
                    // 滚动条将从隐藏到显示, 需要移动窗口
                    bMove = TRUE;

                    // 滚动条处于隐藏状态, 不需要重画
                    bRedraw = FALSE;
                }
            }
            else
            {
                // 滚动条保持禁止状态, 不需要重画
                bRedraw = FALSE;
            }
        }

        if (bRedraw)
        {
            hdc = GetWindowDC(hWnd);
            
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);
            SetRect(&rect, rcClient.right, rcClient.top, 
                rcClient.right + GetSystemMetrics(SM_CXVSCROLL),
                rcClient.bottom);
        }
        else
        {
            hdc = NULL;
            SetRectEmpty(&rect);
        }

        nRet = SB_SetScrollInfo(hdc, pWndData->pVScrollData, &rect, 
            (WPARAM)bRedraw, (LPARAM)pScrollInfo);

        if (bRedraw)
            ReleaseDC(hWnd, hdc);

        // 通过调用MoveWindow函数以引起WM_NCCALCSIZE消息重新计算窗口的
        // 非客户区, 从而实现滚动条的动态消失和出现
        if (bMove)
        {
            GetWindowRectToParent(hWnd, &rcWindow);
            MoveWindow(hWnd, rcWindow.left, rcWindow.top, 
                rcWindow.right - rcWindow.left, 
                rcWindow.bottom - rcWindow.top, TRUE);
        }
    }
    else if (nBar == SB_HORZ && pWndData->pHScrollData)
    {
        // 判断是enabled, 判断滚动条是否会消失
        if (SB_IsEnabled(pWndData->pHScrollData, NULL))
        {
            if (!SB_IsEnabled(pWndData->pHScrollData, pScrollInfo))
            {
                if (pScrollInfo->fMask & SIF_DISABLENOSCROLL)
                    pWndData->bDisableNoHScroll = TRUE;
                else
                {
                    // Show ==> Hide

                    pWndData->bDisableNoHScroll = FALSE;

                    // 滚动条将消失, 不需要重画
                    bRedraw = FALSE;

                    // 滚动条将消失, 需要移动窗口
                    bMove = TRUE;
                }
            }
        }
        else
        {
            // 滚动条是禁止的

            if (SB_IsEnabled(pWndData->pHScrollData, pScrollInfo))
            {
                // 禁止 ==> 非禁止

                if (!pWndData->bDisableNoHScroll)
                {
                    // 滚动条将从隐藏到显示, 需要移动窗口
                    bMove = TRUE;

                    // 滚动条处于隐藏状态, 不需要重画
                    bRedraw = FALSE;
                }
            }
            else
            {
                // 滚动条保持禁止状态, 不需要重画
                bRedraw = FALSE;
            }
        }

        if (bRedraw)
        {
            hdc = GetWindowDC(hWnd);
            
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);
            SetRect(&rect, rcClient.left, rcClient.bottom, rcClient.right,
                rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));
        }
        else
        {
            hdc = NULL;
            SetRectEmpty(&rect);
        }

        nRet = SB_SetScrollInfo(hdc, pWndData->pHScrollData, &rect, 
            (WPARAM)bRedraw, (LPARAM)pScrollInfo);

        if (bRedraw)
            ReleaseDC(hWnd, hdc);

        // 通过调用MoveWindow函数以引起WM_NCCALCSIZE消息重新计算窗口的
        // 非客户区, 从而实现滚动条的动态消失和出现
        if (bMove)
        {
            GetWindowRectToParent(hWnd, &rcWindow);
            MoveWindow(hWnd, rcWindow.left, rcWindow.top, 
                rcWindow.right - rcWindow.left, 
                rcWindow.bottom - rcWindow.top, TRUE);
        }
    }

    return nRet;
}

BOOL DEFWND_GetScrollInfo(HWND hWnd, int nBar, PSCROLLINFO pScrollInfo)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);
    ASSERT(pScrollInfo != NULL);

    // Gets the internal data for default window
    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return FALSE;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (nBar == SB_VERT && (dwStyle & WS_VSCROLL))
    {
        return SB_GetScrollInfo(pWndData->pVScrollData, 
            (LPARAM)pScrollInfo);
    }

    if (nBar == SB_HORZ && (dwStyle & WS_HSCROLL))
    {
        return SB_GetScrollInfo(pWndData->pHScrollData, 
            (LPARAM)pScrollInfo);
    }

    // 出错时根据pScrollInfo的fMask的值设置相应的默认值，这样才能与
    // WIN32在出错时保持兼容性
    
    if (pScrollInfo->fMask & SIF_PAGE)
        pScrollInfo->nPage = 0;
    
    if (pScrollInfo->fMask & SIF_POS)
        pScrollInfo->nPos = 0;
    
    if (pScrollInfo->fMask & SIF_RANGE)
    {
        pScrollInfo->nMin = 0;
        pScrollInfo->nMax = 100;
    }
    
    if (pScrollInfo->fMask & SIF_TRACKPOS)
        pScrollInfo->nTrackPos = 0;
    
    return FALSE;
}

BOOL DEFWND_EnableScrollBar(HWND hWnd, int nBar, int nArrow)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
    RECT        rcClient, rect;
    HDC         hdc;
    BOOL        bRet = FALSE;
 
    ASSERT(nBar == SB_BOTH || nBar == SB_HORZ || nBar == SB_VERT);

    // Gets the internal data for default window
    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return FALSE;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if ((nBar == SB_VERT || nBar == SB_BOTH) && (dwStyle & WS_VSCROLL))
    {
        hdc = GetWindowDC(hWnd);

        GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

        SetRect(&rect, rcClient.right, rcClient.top, 
               rcClient.right + GetSystemMetrics(SM_CXVSCROLL),
               rcClient.bottom);

        bRet = SB_Enable_Arrows(hdc, pWndData->pVScrollData, &rect, 
            (WPARAM)nArrow);

        ReleaseDC(hWnd, hdc);
    }

    if ((nBar == SB_HORZ || nBar == SB_BOTH) && (dwStyle & WS_HSCROLL))
    {
        hdc = GetWindowDC(hWnd);

        GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

        SetRect(&rect, rcClient.left, rcClient.bottom,
                rcClient.right, 
                rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));

        bRet = SB_Enable_Arrows(hdc, pWndData->pHScrollData, &rect, 
            (WPARAM)nArrow);

        ReleaseDC(hWnd, hdc);
    }

    return bRet;
}

int DEFWND_SetScrollPos(HWND hWnd, int nBar, int nPos, BOOL bRedraw)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
    RECT        rcClient, rect;
    HDC         hdc;
    int         nRet = 0;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);

    // Gets the internal data for default window
    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return FALSE;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (nBar == SB_VERT && (dwStyle & WS_VSCROLL))
    {
        hdc = GetWindowDC(hWnd);

        GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

        SetRect(&rect, rcClient.right, rcClient.top, 
               rcClient.right + GetSystemMetrics(SM_CXVSCROLL),
               rcClient.bottom);

        nRet = SB_SetPos(hdc, pWndData->pVScrollData, &rect, (WPARAM)nPos, 
            (LPARAM)bRedraw);

        ReleaseDC(hWnd, hdc);
    }
    else if (nBar == SB_HORZ && (dwStyle & WS_HSCROLL))
    {
        hdc = GetWindowDC(hWnd);

        GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

        SetRect(&rect, rcClient.left, rcClient.bottom,
                rcClient.right, 
                rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));
        nRet = SB_SetPos(hdc, pWndData->pHScrollData, &rect, (WPARAM)nPos, 
            (LPARAM)bRedraw);

        ReleaseDC(hWnd, hdc);
    }

    return nRet;
}

int  DEFWND_GetScrollPos(HWND hWnd, int nBar)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
    int         nRet = 0;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);

    // Gets the internal data for default window
    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return FALSE;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (nBar == SB_VERT && (dwStyle & WS_VSCROLL))
        nRet = SB_GetPos(pWndData->pVScrollData);
    else if (nBar == SB_HORZ && (dwStyle & WS_HSCROLL))
        nRet = SB_GetPos(pWndData->pHScrollData);

    return nRet;
}

BOOL DEFWND_SetScrollRange(HWND hWnd, int nBar, int nMinPos, int nMaxPos, 
                           BOOL bRedraw)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
    RECT        rcClient, rect;
    HDC         hdc;
    int         nRet = 0;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);

    // Gets the internal data for default window
    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return FALSE;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (nBar == SB_VERT && (dwStyle & WS_VSCROLL))
    {
        if (bRedraw)   // the scrollbar need to be redrawed.
        {
            hdc = GetWindowDC(hWnd);

            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

            SetRect(&rect, rcClient.right, rcClient.top, 
                   rcClient.right + GetSystemMetrics(SM_CXVSCROLL),
                   rcClient.bottom);

            nRet = SB_SetRangeRedraw(hdc, pWndData->pVScrollData, &rect, 
                (WPARAM)nMinPos, (LPARAM)nMaxPos);

            ReleaseDC(hWnd, hdc);
        }
        else
            nRet = SB_SetRange(pWndData->pVScrollData, (WPARAM)nMinPos,
            (LPARAM)nMaxPos);
    }
    else if (nBar == SB_HORZ && (dwStyle & WS_HSCROLL))
    {
        if (bRedraw)    // the scrollbar need to be redrawed.
        {
            hdc = GetWindowDC(hWnd);

            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

            SetRect(&rect, rcClient.left, rcClient.bottom,
                    rcClient.right, 
                    rcClient.bottom + GetSystemMetrics(SM_CYHSCROLL));
            nRet = SB_SetRangeRedraw(hdc,  pWndData->pHScrollData, &rect, 
                (WPARAM)nMinPos, (LPARAM)nMaxPos);

            ReleaseDC(hWnd, hdc);
        }
        else
            nRet = SB_SetRange(pWndData->pHScrollData, (WPARAM)nMinPos, 
                (LPARAM)nMaxPos);
    }

    if (nRet)
        return TRUE;

    return FALSE;
}

BOOL DEFWND_GetScrollRange(HWND hWnd, int nBar, LPINT lpMinPos, 
                           LPINT lpMaxPos)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);

    // Gets the internal data for default window
    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return FALSE;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (nBar == SB_VERT && (dwStyle & WS_VSCROLL))
        SB_GetRange(pWndData->pVScrollData, (WPARAM)lpMinPos,
            (LPARAM)lpMaxPos);
    else if (nBar == SB_HORZ && (dwStyle & WS_HSCROLL))
        SB_GetRange(pWndData->pHScrollData, (WPARAM)lpMinPos, 
            (LPARAM)lpMaxPos);

    return TRUE;
}

/*
**  Function : DEFWND_CalcMinSize
**  Purpose  :
**      Calculates the minimize size of a window with specified window 
**      style, this function will be called by CreateWindowEx function.
*/
void DEFWND_CalcMinSize(DWORD dwExStyle, DWORD dwStyle, HMENU hMenu, 
                        PSIZE pMinSize)
{
    int nBorderW, nBorderH;

    ASSERT(pMinSize != NULL);

    pMinSize->cx = 0;
    pMinSize->cy = 0;

    // 计算边框的大小
    if ((dwStyle & WS_DLGFRAME) == WS_DLGFRAME)
    {
        nBorderW = GetSystemMetrics(SM_CXDLGFRAME);
        nBorderH = GetSystemMetrics(SM_CYDLGFRAME);
    }
    else if (dwStyle & WS_BORDER)
    {
        nBorderW = GetSystemMetrics(SM_CXBORDER);
        nBorderH = GetSystemMetrics(SM_CYBORDER);
    }
    else if (dwStyle & WS_THICKFRAME)
    {
        nBorderW = GetSystemMetrics(SM_CXFRAME);
        nBorderH = GetSystemMetrics(SM_CYFRAME);
    }
    else
    {
        nBorderW = 0;
        nBorderH = 0;
    }

    pMinSize->cx += nBorderW * 2;
    pMinSize->cy += nBorderH * 2;

    if (dwStyle & WS_CAPTION)
    {
//        pMinSize->cx += 100;
        pMinSize->cx += 50;  //100 (11/29/01)
        pMinSize->cy += GetSystemMetrics(SM_CYCAPTION);
    }

#if (!NOMENUS)

    if (!(dwStyle & WS_CHILD) && hMenu && IsMenu(hMenu))
        pMinSize->cy += GetSystemMetrics(SM_CYMENU);

#endif

    if (dwStyle & WS_HSCROLL)
        pMinSize->cy += GetSystemMetrics(SM_CYHSCROLL);

    if (dwStyle & WS_VSCROLL)
        pMinSize->cx += GetSystemMetrics(SM_CXVSCROLL);
}

static void GetWindowRectToParent(HWND hWnd, PRECT pRect)
{
    HWND hwndParent;
    RECT rcParent;

    GetWindowRect(hWnd, pRect);

    hwndParent = GetParent(hWnd);
    if (hwndParent)
    {
        GetWindowRectEx(hwndParent, &rcParent, W_CLIENT, XY_SCREEN);
        OffsetRect(pRect, -rcParent.left, -rcParent.top);
    }
}

static PWNDDATA GetWndData(HWND hWnd)
{
    return (PWNDDATA)((BYTE*)GetUserDataBase(hWnd));
}

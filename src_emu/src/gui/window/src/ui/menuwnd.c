/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements menu window class.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "plx_pdaex.h"

#if (!NOMENUS)
#include "windowx.h"
#include "string.h"
#include "menuobj.h"
#include "menuwnd.h"
#include "winpda.h"
#include "hpfile.h"

#define MS_HIDE     0x00000001  // 用于指定menubar是否隐藏

#define ACTION_DO_NOTHING       0
#define ACTION_SELECT           1
#define ACTION_SELECT_SUBMENU   2
#define ACTION_KEYSELECT_L      3
#define ACTION_KEYSELECT_R      4
#define ACTION_ESCAPE		    5

// Define exit code constant
#define EXIT_WAIT               0
#define EXIT_DO_NOTHING         1
#define EXIT_RESEND_MESSAGE     2
#define EXIT_SELECT             3
#define EXIT_MULTIPOPUP         4
#define EXIT_ESCAPE             5

#if (INTERFACE_MONO)

#define MI_HGAP                 4    // horizontal gap between two items
#define MI_VGAP                 5    // vertical gap between two items
#define MI_SEPARATOR            4

#define MI_LEFT_MARGIN          4    // left margin in a popup menu
#define MI_TOP_MARGIN           3    // top margin in a popup menu
#define MI_RIGHT_BITMAP         15   // right bitmap width
#define MI_RIGHT_MARGIN         10   // right margin in a popup menu
#define MI_CY_SUBTITLE          30   // height of sub-menu title

#else  // INTERFACE_MONO

#define MI_HGAP                 8    // horizontal gap between two items
#define MI_VGAP                 4    // vertical gap between two items
#define MI_SEPARATOR            8

#define MI_LEFT_MARGIN          20   // left margin in a popup menu
#define MI_TOP_MARGIN           20   // top margin in a popup menu
#define MI_RIGHT_BITMAP         20   // right bitmap width
#define MI_RIGHT_MARGIN         20   // right margin in a popup menu

#endif // INTERFACE_MONO

#define TAB_LEN                 7    // define the length of tab key

#define CHECK_BMP_WIDTH         9
#define CHECK_BMP_HEIGHT        9

#define ARROW_BMP_WIDTH         8
#define ARROW_BMP_HEIGHT        9

#ifdef MI_CY_SUBTITLE
#define CR_SUBTITLE             RGB(160, 175, 230)
#endif

#define IDT_REPEAT              1
#define ET_REPEAT_FIRST         300
#define ET_REPEAT_LATER         100

#define IDT_SLIDE               2
#define ET_SLIDE                100
#define CY_SLIDESTART           80
#define DCY_SLIDEONCE           (-10)

#if (COLUMN_BITMAP)
static const BYTE CheckBmpData[] =
{
    0xFF, 0x80, 0x00, 0x00,     // 11111111100000000000000000000000
    0x8F, 0x80, 0x00, 0x00,     // 10001111100000000000000000000000
    0xC7, 0x80, 0x00, 0x00,     // 11000111100000000000000000000000
    0xE3, 0x80, 0x00, 0x00,     // 11100011100000000000000000000000
    0xF1, 0x80, 0x00, 0x00,     // 11110001100000000000000000000000
    0xF8, 0x80, 0x00, 0x00,     // 11111000100000000000000000000000
    0xF1, 0x80, 0x00, 0x00,     // 11110001100000000000000000000000
    0xE3, 0x80, 0x00, 0x00,     // 11100011100000000000000000000000
    0xFF, 0x80, 0x00, 0x00      // 11111111100000000000000000000000
};
#else
static const BYTE CheckBmpData[] =
{
    0xFF, 0x80, 0x00, 0x00,     // 11111111100000000000000000000000
    0xFE, 0x80, 0x00, 0x00,     // 11111110100000000000000000000000
    0xFC, 0x80, 0x00, 0x00,     // 11111100100000000000000000000000
    0xB8, 0x80, 0x00, 0x00,     // 10111000100000000000000000000000
    0x91, 0x80, 0x00, 0x00,     // 10010001100000000000000000000000
    0x83, 0x80, 0x00, 0x00,     // 10000011100000000000000000000000
    0xC7, 0x80, 0x00, 0x00,     // 11000111100000000000000000000000
    0xEF, 0x80, 0x00, 0x00,     // 11101111100000000000000000000000
    0xFF, 0x80, 0x00, 0x00      // 11111111100000000000000000000000
};
#endif

#if (COLUMN_BITMAP)
static const BYTE RightArrowBmpData[] = 
{
    0xFF, 0x80, 0x00, 0x00,    // 11111111100000000000000000000000
    0xFF, 0x80, 0x00, 0x00,    // 11111111100000000000000000000000
    0xF7, 0x80, 0x00, 0x00,    // 11110111100000000000000000000000
    0xE3, 0x80, 0x00, 0x00,    // 11100011100000000000000000000000
    0xC1, 0x80, 0x00, 0x00,    // 11000001100000000000000000000000
    0x80, 0x80, 0x00, 0x00,    // 10000000100000000000000000000000
    0xFF, 0x80, 0x00, 0x00,    // 11111111100000000000000000000000
    0xFF, 0x80, 0x00, 0x00     // 11111111100000000000000000000000
};
#else
static const BYTE RightArrowBmpData[] = 
{
    0xFF, 0x00, 0x00, 0x00,     // 11111111000000000000000000000000
    0xDF, 0x00, 0x00, 0x00,     // 11011111000000000000000000000000
    0xCF, 0x00, 0x00, 0x00,     // 11001111000000000000000000000000
    0xC7, 0x00, 0x00, 0x00,     // 11000111000000000000000000000000
    0xC3, 0x00, 0x00, 0x00,     // 11000011000000000000000000000000
    0xC7, 0x00, 0x00, 0x00,     // 11000111000000000000000000000000
    0xCF, 0x00, 0x00, 0x00,     // 11001111000000000000000000000000
    0xDF, 0x00, 0x00, 0x00,     // 11011111000000000000000000000000
    0xFF, 0x00, 0x00, 0x00      // 11111111000000000000000000000000
};
#endif

#if (COLUMN_BITMAP)
static const BYTE GrayedRightArrowBmpData[] = 
{
    0x00, 0x00, 0x00, 0x00,   // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00,   // 00000000000000000000000000000000
    0x08, 0x00, 0x00, 0x00,   // 00001000000000000000000000000000
    0x1C, 0x00, 0x00, 0x00,   // 00011100000000000000000000000000
    0x3E, 0x00, 0x00, 0x00,   // 00111110000000000000000000000000
    0x7F, 0x00, 0x00, 0x00,   // 01111111000000000000000000000000
    0x00, 0x00, 0x00, 0x00,   // 00000000000000000000000000000000
    0x00, 0x00, 0x00, 0x00    // 00000000000000000000000000000000
};
#else
static const BYTE GrayedRightArrowBmpData[] = 
{
    0x00, 0x00, 0x00, 0x00,     // 00000000000000000000000000000000
    0x20, 0x00, 0x00, 0x00,     // 00100000000000000000000000000000
    0x30, 0x00, 0x00, 0x00,     // 00110000000000000000000000000000
    0x38, 0x00, 0x00, 0x00,     // 00111000000000000000000000000000
    0x3C, 0x00, 0x00, 0x00,     // 00111100000000000000000000000000
    0x38, 0x00, 0x00, 0x00,     // 00111000000000000000000000000000
    0x30, 0x00, 0x00, 0x00,     // 00110000000000000000000000000000
    0x20, 0x00, 0x00, 0x00,     // 00100000000000000000000000000000
    0x00, 0x00, 0x00, 0x00      // 00000000000000000000000000000000
};
#endif

// menu data definition
typedef struct
{
    HWND        hwndParentMenu;
    PMENUOBJ    pMenu;
    PMENUITEM   pCurItem;
    BOOL        bEnterLoop;
    BOOL        bAutoSelect;
    int         nExitCode;
    MSG         msg;
    BOOL        bIsMenubar;
    HWND        hwndVScroll;
    WORD        wKeyCode;
    int         nRepeats;
    int         x;
    int         y;
    int         cx;
    int         cy;
    int         yFinal;
    int         cyFinal;
    int         cySlide;
} MENUWNDDATA, *PMENUWNDDATA;

static HBITMAP hbmpRightArrow, hbmpGrayedRightArrow;
static HBITMAP hbmpCheck, hbmpMenuTop, hbmpMenuBk, hbmpMenuFocus = NULL;

#define IsPopup(pMenuWndData)   (pMenuWndData->pMenu->hParentMenuWin != NULL || \
                                 pMenuWndData->pMenu->wFlags == MT_POPUP)

// Internal function prototypes
static void PaintMenuBar(HWND hWnd, HDC hdc, PMENUWNDDATA pMenuWndData, 
                         const RECT* pRect);
static void PaintPopupMenu(HWND hWnd, HDC hdc, PMENUWNDDATA pMenuWndData, 
                           const RECT* pRect);
static void DrawCurrentItem(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                            BOOL bSelect);
static void DrawMenuItem(HWND hWnd, HDC hdc, PMENUOBJ pMenu, 
                         PMENUITEM pMenuItem, PRECT pRect, int srcy, int srcheight,
                         BOOL bPopup, BOOL bSelect);
static void GetCurrentItemRect(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                               PRECT pRect);
static void TrackMouse(HWND hWnd, PMENUWNDDATA pMenuWndData, int x, int y);
static void MENU_Patch(HWND hWnd, PMENUWNDDATA pMenuWndData, UINT wMsgCmd,
                       WPARAM wParam, int nAction);

static BOOL PointInWindow(HWND hWnd, int x, int y);
static BOOL PointInOwnerItem(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                             int x, int y);
static BOOL PointInMenus(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                         int x, int y);

static int  MenuMessageLoop(HWND hWnd);

static HWND CreatePopupMenuWindow(HWND hWnd, PMENUWNDDATA pMenuWndData);
static PMENUITEM SearchItem(HWND hWnd, PMENUWNDDATA pMenuWndData, int x, 
                            int y);
static void MENU_GetTextExtent(HDC hdc, PMENUITEM pMenuItem, PSIZE pSize);
static void MENU_DrawText(HDC hdc, PRECT pRect, PMENUITEM pMenuItem,
                          UINT uFormat);
static BOOL MENU_PointInValidBar(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                                 int x, int y);
static void WindowToScreen(HWND hWnd, int* left, int* top);
static void ScreenToWindow(HWND hWnd, int* left, int* top);

#if (!NOKEYBOARD)

static PMENUITEM prevItem( PMENUWNDDATA pMenuWndData );
static PMENUITEM nextItem( PMENUWNDDATA pMenuWndData );
static void trackKey(HWND hWnd, PMENUWNDDATA pMenuWndData, BOOL findNext);
static BOOL MENU_TrackHotKey(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                             WORD wVirtKey);
static void MENU_Keydown(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                         UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
#endif

// External funciton prototypes
static LRESULT CALLBACK MENU_WndProc(HWND hWnd, UINT wMsgCmd, 
                                     WPARAM wParam, LPARAM lParam);

static BOOL MENU_Init(HWND hWnd, PMENUWNDDATA pMenuWndData, HMENU hMenu);
static LRESULT MENU_NCCreate(HWND hWnd, PMENUWNDDATA pMenuWndData,
                             LPCREATESTRUCT lpcs);
static void MENU_Paint(HWND hWnd, HDC hdc, PMENUWNDDATA pMenuWndData, 
                       const RECT* pRect);
static int  MENU_Pendown(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                         UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void MENU_Penmove(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                         UINT wMsgCmd, LPARAM lParam);
static void MENU_Penup(HWND hWnd, PMENUWNDDATA pMenuWndData, UINT wMsgCmd, 
                       LPARAM lParam, int nPenDownAction);
static void MENU_Size(HWND hWnd, PMENUWNDDATA pMenuWndData, LPARAM lParam);
static void MENU_Timer(HWND hWnd, PMENUWNDDATA pMenuWndData, WPARAM wParam);
static void MENU_Refresh(HWND hWnd, PMENUWNDDATA pMenuWndData, BOOL bRefresh,
                         LPARAM lParam);
static void MENU_KeyUp(HWND hWnd, PMENUWNDDATA pMenuWndData, UINT wMsgCmd,
                       WPARAM wParam, LPARAM lParam);

BOOL MENU_RegisterClass(void)
{
    WNDCLASS wc;    
    int  nPathLen;
    char PathName[MAX_PATH];

    wc.style            = CS_DEFWIN;
    wc.lpfnWndProc      = MENU_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(MENUWNDDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
//    wc.hbrBackground    = CreateBrush(BS_SOLID, GetSysColor(COLOR_MENU), 0);
    wc.hbrBackground    = CreateSharedBrush(BS_SOLID, GetSysColor(COLOR_MENU), 0);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "#MENU";

    if (!RegisterClass(&wc))
        return FALSE;

    // create the check bitmap
    hbmpCheck = CreateSharedBitmap(CHECK_BMP_WIDTH, CHECK_BMP_HEIGHT, 1, 1,
        CheckBmpData);

    // create the arrow bitmap identifing the submenu.
    hbmpRightArrow = CreateSharedBitmap(ARROW_BMP_WIDTH, ARROW_BMP_HEIGHT, 1, 1,
        RightArrowBmpData);
    hbmpGrayedRightArrow = CreateSharedBitmap(ARROW_BMP_WIDTH, ARROW_BMP_HEIGHT, 
        1, 1, GrayedRightArrowBmpData);

    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);
    strcat(PathName, "menutop.bmp");
    hbmpMenuTop = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);
    strcat(PathName, "menubk.bmp");
    hbmpMenuBk = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);
    strcat(PathName, "listfocus.bmp");
    hbmpMenuFocus = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    return TRUE;
}

static LRESULT CALLBACK MENU_WndProc(HWND hWnd, UINT wMsgCmd, 
                                     WPARAM wParam, LPARAM lParam)
{
    static int      nAction = ACTION_DO_NOTHING;

    LRESULT         lResult;
    PMENUWNDDATA    pMenuWndData;
    HDC             hdc;
    RECT            rcClient;

    lResult = 0;
    pMenuWndData  = (PMENUWNDDATA)GetUserData(hWnd);
 
    switch (wMsgCmd)
    {
    case WM_NCCREATE :

        lResult = MENU_NCCreate(hWnd, pMenuWndData, (LPCREATESTRUCT)lParam);

        break;

    case WM_SETMENU :

        lResult = MENU_Init(hWnd, pMenuWndData, (HMENU)lParam);
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_NCDESTROY :

        // If it is popup menu, ungrab the pen message
        if (IsPopup(pMenuWndData))
        {
			UngrabPen();

#if (!NOKEYBOARD)

			UngrabKeyboard();
#endif
		}
        else
        {
            // 由于菜单对象可能被应用程序删除，这里必须判断菜单对象的有效性
            if (IsMenu((HANDLE)pMenuWndData->pMenu))
                MENU_Destroy(pMenuWndData->pMenu);
            pMenuWndData->pMenu = NULL;
        }

        break;

    case WM_CANCELMODE :
    {
        HWND hwndParentMenu;

        KillTimer(hWnd, IDT_REPEAT);
        pMenuWndData->wKeyCode = 0;
        pMenuWndData->nRepeats = 0;

        while (IsPopup(pMenuWndData))
        {
            hwndParentMenu = pMenuWndData->hwndParentMenu;
            DestroyWindow(hWnd);

            if (!hwndParentMenu)
                return lResult;

            hWnd = hwndParentMenu;
            pMenuWndData = (PMENUWNDDATA)GetUserData(hWnd);
        }

        pMenuWndData->nExitCode = EXIT_DO_NOTHING;

        if (hWnd != NULL)
            DrawCurrentItem(hWnd, pMenuWndData, FALSE);

        break;
    }

    case WM_SHOWWINDOW :

        if (wParam && (GetWindowLong(hWnd, GWL_STYLE) & MS_HIDE))
        {
            SetCapture(hWnd);
        }

        break;

//    case WM_MOUSEACTIVATE :
//
//        return MA_NOACTIVATE;

    case WM_SIZE:
        MENU_Size(hWnd, pMenuWndData, lParam);
        return 0;

    case WM_PAINT :

        hdc = BeginPaint(hWnd, NULL);

        GetClientRect(hWnd, &rcClient);
        MENU_Paint(hWnd, hdc, pMenuWndData, &rcClient);
    
        EndPaint(hWnd, NULL);

        break;

    case WM_LBUTTONDOWN :

        nAction = MENU_Pendown(hWnd, pMenuWndData, wMsgCmd, wParam, 
            lParam);
        break;

    case WM_MOUSEMOVE :

        MENU_Penmove(hWnd, pMenuWndData, wMsgCmd, lParam);
        break;

    case WM_LBUTTONUP :

        MENU_Penup(hWnd, pMenuWndData, wMsgCmd, lParam, nAction);
        nAction = ACTION_DO_NOTHING;

        break;

    case WM_TIMER:
        MENU_Timer(hWnd, pMenuWndData, wParam);
        break;

    case WM_REFRESHMENU:
        MENU_Refresh(hWnd, pMenuWndData, (BOOL)wParam, lParam);
        break;

#if (!NOKEYBOARD)

    case WM_KEYDOWN :

        MENU_Keydown(hWnd, pMenuWndData, wMsgCmd, wParam, lParam);

        break;

    case WM_KEYUP:

        MENU_KeyUp(hWnd, pMenuWndData, wMsgCmd, wParam, lParam);

        break;

    case WM_SYSKEYUP :
        
        // response the single ALT key up only in the menu-bar window, 
        // quit message loop.
        if (!pMenuWndData->hwndParentMenu)  
            MENU_Keydown(hWnd, pMenuWndData, wMsgCmd, wParam, lParam);

        break;
        
    case WM_SYSKEYDOWN :

        MENU_Keydown(hWnd, pMenuWndData, wMsgCmd, wParam, lParam);

        break;
#endif

    default :

        return DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}

static BOOL MENU_Init(HWND hWnd, PMENUWNDDATA pMenuWndData, HMENU hMenu)
{
    PMENUOBJ    pMenu;

    if (!IsMenu(hMenu))
        return FALSE;

    pMenu = (PMENUOBJ)hMenu;

    pMenuWndData->hwndParentMenu = (HWND)pMenu->hParentMenuWin;
    pMenuWndData->pMenu = pMenu;
    pMenuWndData->pCurItem = NULL;
    pMenuWndData->bEnterLoop = FALSE;
    pMenuWndData->bAutoSelect = FALSE;
    pMenuWndData->wKeyCode = 0;
    pMenuWndData->nRepeats = 0;

    // If popup menu, grab pen message
    if (IsPopup(pMenuWndData))
    {
        GrabPen(hWnd);

#if (!NOKEYBOARD)

	    GrabKeyboard(hWnd);
#endif

    }

    return TRUE;
}

static LRESULT MENU_NCCreate(HWND hWnd, PMENUWNDDATA pMenuWndData,
                          LPCREATESTRUCT lpcs)
{
    LRESULT lResult = 0;
    HWND hwndOwner = NULL;
    RECT rcScreen, rc;
    PMENUOBJ pMenu = NULL;

    lResult = (LRESULT)MENU_Init(hWnd, pMenuWndData, lpcs->hMenu);
    
    hwndOwner = GetWindow(hWnd, GW_OWNER);
    GetWindowRectEx(hwndOwner, &rcScreen, W_CLIENT, XY_SCREEN);

    pMenuWndData->x       = lpcs->x;
    pMenuWndData->y       = lpcs->y;
    pMenuWndData->cx      = lpcs->cx;
    pMenuWndData->cy      = lpcs->cy;
    pMenuWndData->yFinal  = rcScreen.top;
    pMenuWndData->cyFinal = rcScreen.bottom - rcScreen.top;
    pMenuWndData->cySlide = CY_SLIDESTART;

    pMenu = (PMENUOBJ)lpcs->hMenu;

    if (pMenu->hParentMenuWin != NULL)
    {
        GetWindowRect(hWnd, &rc);
        pMenuWndData->hwndVScroll = CreateWindow(
            "SCROLLBAR",
            "",
            WS_VISIBLE | WS_CHILD | SBS_VERT,
            rc.right - rc.left - GetSystemMetrics(SM_CXVSCROLL),
            rc.top + MI_CY_SUBTITLE,
            GetSystemMetrics(SM_CXVSCROLL),
            pMenuWndData->cyFinal - MI_CY_SUBTITLE,
            hWnd,
            NULL,
            NULL,
            NULL);
    }
    else
    {
        pMenuWndData->hwndVScroll = NULL;
    }

    return lResult;
}

static void MENU_Paint(HWND hWnd, HDC hdc, PMENUWNDDATA pMenuWndData, 
                       const RECT* pRect)
{
    //SetBkMode(hdc, BM_TRANSPARENT);

    if (!IsPopup(pMenuWndData))
        PaintMenuBar(hWnd, hdc, pMenuWndData, pRect);
    else
        PaintPopupMenu(hWnd, hdc, pMenuWndData, pRect);    
    
}

static int MENU_Pendown(HWND hWnd, PMENUWNDDATA pMenuWndData, UINT wMsgCmd, 
                        WPARAM wParam, LPARAM lParam)
{
    IWORD   x, y;
    int     nAction = ACTION_DO_NOTHING;

    // If the menu is normal menu, and not in menu message loop, 
    // resend the WM_PENDOWN message and enter menu message loop

    if (!pMenuWndData->bEnterLoop)
    {
        ASSERT(!IsPopup(pMenuWndData));

        PostMessage(hWnd, wMsgCmd, wParam, lParam);

        MenuMessageLoop(hWnd);

        if (GetWindowLong(hWnd, GWL_STYLE) & MS_HIDE)
            ShowWindow(hWnd, SW_HIDE);
        else
        {
            if (pMenuWndData->pCurItem)
            {
                DrawCurrentItem(hWnd, pMenuWndData, FALSE);
                pMenuWndData->pCurItem = NULL;
            }
            
            UngrabPen();
            
#if (!NOKEYBOARD)
            
            UngrabKeyboard();
#endif
        }

        return ACTION_DO_NOTHING;
    }

    x = LOWORD(lParam);
    y = HIWORD(lParam);

    if (PointInWindow(hWnd, x, y) || 
        PointInOwnerItem(hWnd, pMenuWndData, x, y))
    {
        TrackMouse(hWnd, pMenuWndData, x, y);

        if (!IsPopup(pMenuWndData) && !pMenuWndData->pCurItem)
            pMenuWndData->nExitCode = EXIT_DO_NOTHING;
        else
            nAction = ACTION_SELECT_SUBMENU;

        if (IsPopup(pMenuWndData) && pMenuWndData->pCurItem)
            nAction = ACTION_SELECT_SUBMENU;
    }
    else
        pMenuWndData->nExitCode = EXIT_RESEND_MESSAGE;

    // do some work according to the value of nAction
    MENU_Patch(hWnd, pMenuWndData, wMsgCmd, wParam, nAction);

    // there is any action choosing a menu, so exit the menu
    if (pMenuWndData->nExitCode != 0)
        nAction = ACTION_DO_NOTHING;

    return nAction;
}


static void MENU_Penmove(HWND hWnd, PMENUWNDDATA pMenuWndData, UINT wMsgCmd,
                         LPARAM lParam)
{
    IWORD   x, y;
    int     nAction = ACTION_DO_NOTHING;

    if (!pMenuWndData->bEnterLoop)
        return;

    x = LOWORD(lParam);
    y = HIWORD(lParam);

    TrackMouse(hWnd, pMenuWndData, x, y);

    if (!(PointInWindow(hWnd, x, y) || 
        PointInOwnerItem(hWnd, pMenuWndData, x, y)) && 
        PointInMenus(hWnd, pMenuWndData, x, y))
    {
        pMenuWndData->nExitCode = EXIT_RESEND_MESSAGE;
    }
    else
        nAction = ACTION_SELECT_SUBMENU;

    // do some work according to the value of nAction
    MENU_Patch(hWnd, pMenuWndData, wMsgCmd, 0, nAction);
}

static void MENU_Penup(HWND hWnd, PMENUWNDDATA pMenuWndData, UINT wMsgCmd, 
                       LPARAM lParam, int nPenDownAction)
{
    IWORD   x, y;
    int     nAction = ACTION_DO_NOTHING;

    if (!pMenuWndData->bEnterLoop)
        return;

    x = LOWORD(lParam);
    y = HIWORD(lParam);

    TrackMouse(hWnd, pMenuWndData, x, y);

    if (PointInOwnerItem(hWnd, pMenuWndData, x, y) && !PointInWindow(hWnd, x, y))
    {
        //pMenuWndData->pCurItem = pMenuWndData->pMenu->pItems; 
        //DrawCurrentItem(hWnd, pMenuWndData, TRUE);
    }
    else
    {
        // if the current menu is a popup menu by TrackPopupMenu and
        // there is not WM_PENDOWN, return directly.
        if (pMenuWndData->pMenu->wFlags == MT_POPUP && 
            nPenDownAction == ACTION_DO_NOTHING)
        {
            //pMenuWndData->pCurItem = pMenuWndData->pMenu->pItems; 
            //DrawCurrentItem(hWnd, pMenuWndData, TRUE);
            return;
        }
        else
        {
            if (pMenuWndData->pCurItem && pMenuWndData->pCurItem->wFlags & MF_POPUP)
                nAction = ACTION_DO_NOTHING;
            else if (pMenuWndData->pCurItem)
                nAction = ACTION_SELECT;
            else
                pMenuWndData->nExitCode = EXIT_DO_NOTHING;
        }
    }

    // do some work according to the value of nAction
    MENU_Patch(hWnd, pMenuWndData, wMsgCmd, 0, nAction);
}


#if (!NOKEYBOARD)

static void MENU_Keydown(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                         UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    static BOOL  bSysKeyUp = FALSE;

    WORD         wVirtKey;
  	PMENUITEM    pNewItem;
    int          nItemCount;
    int          nValidHotKey = -1;
    int          nAction = ACTION_DO_NOTHING;

    // When the menu window isn't in visibility state, do not response any
    // key message. This is used for hide menu in some PDA project
    if (!IsWindowVisible(hWnd))
        return;

	wVirtKey = LOWORD(wParam);

    if ((pMenuWndData->nRepeats > 0) && (wVirtKey != pMenuWndData->wKeyCode))
    {
        KillTimer(hWnd, IDT_REPEAT);
        pMenuWndData->nRepeats = 0;
    }

    pMenuWndData->wKeyCode = wVirtKey;
    pMenuWndData->nRepeats++;

    switch(wVirtKey)
    {
    case VK_UP :
    case VK_DOWN :

        if (IsPopup(pMenuWndData))
            trackKey(hWnd, pMenuWndData, (wVirtKey == VK_DOWN));
        else if (wVirtKey == VK_DOWN)
            nAction = ACTION_SELECT_SUBMENU;
        
        if (pMenuWndData->nRepeats == 1)
        {
            SetTimer(hWnd, IDT_REPEAT, ET_REPEAT_FIRST, NULL);
        }
        break;

//    case VK_LEFT :
//    case VK_RIGHT :
//
//        if (!IsPopup(pMenuWndData))
//		{
//            // when the ALT key is pressed down, don't response the 
//            // combination key of ALT and left/right key.
//            if (lParam & 0x20000000)
//                break;
//
//			if (pMenuWndData->pCurItem == 0)
//				pMenuWndData->pCurItem = pMenuWndData->pMenu->pItems; 
//
//			trackKey(hWnd, pMenuWndData, (wVirtKey == VK_RIGHT));
//
//			nAction = ACTION_SELECT_SUBMENU;
//		}
//		else
//		{
//			if (wVirtKey == VK_LEFT && pMenuWndData->pCurItem)
//			{
//				if (pMenuWndData->nExitCode == EXIT_MULTIPOPUP &&
//					(pMenuWndData->pCurItem->wFlags & MF_POPUP) && 
//                    pMenuWndData->pCurItem->hIDorPopup != 0)
//                {
//                    pMenuWndData->bEnterLoop = TRUE;
//					pMenuWndData->nExitCode = 0;
//                }
//				else
//				{
//					pMenuWndData->pCurItem = pMenuWndData->pMenu->pItems; 
//					nAction = ACTION_KEYSELECT_L;
//				}
//			}
//
//			if (wVirtKey == VK_RIGHT && pMenuWndData->pCurItem)
//			{
//				if (pMenuWndData->nExitCode != EXIT_MULTIPOPUP &&
//					(pMenuWndData->pCurItem->wFlags & MF_POPUP) && 
//                    pMenuWndData->pCurItem->hIDorPopup != 0) 
//                {
//					nAction = ACTION_SELECT_SUBMENU;
//                }
//				else
//				{
//					pMenuWndData->pCurItem = pMenuWndData->pMenu->pItems; 
//					nAction = ACTION_KEYSELECT_R;
//				}
//			}
//        }
//
//        break;

    case VK_HOME :
    case VK_END :

        if (IsPopup(pMenuWndData))
        {
            pNewItem = pMenuWndData->pMenu->pItems;

			if (pMenuWndData->pCurItem != NULL)
				DrawCurrentItem(hWnd, pMenuWndData, FALSE);

            if (wVirtKey == VK_END)
            {
                nItemCount = pMenuWndData->pMenu->cUsedItems;
                if (nItemCount >= 1)
                    pNewItem = &pMenuWndData->pMenu->pItems[nItemCount - 1];
            }

			pMenuWndData->pCurItem = pNewItem;

            if (pNewItem && !(pNewItem->wFlags & MF_SEPARATOR))
				DrawCurrentItem(hWnd, pMenuWndData, TRUE);
		}

        break;

//    case VK_RETURN :
	case VK_F5 :

        if (!IsPopup(pMenuWndData))
			nAction = ACTION_SELECT_SUBMENU;
		else
		{
            if(pMenuWndData->pCurItem)
            {
                if ((pMenuWndData->pCurItem->wFlags & MF_POPUP) && 
                    pMenuWndData->pCurItem->hIDorPopup != 0) 
                    nAction = ACTION_SELECT_SUBMENU;
                else
                {
                    nAction = ACTION_SELECT;
                    
                    // set the bSysKeyUp flag to TRUE indicate that don't response
                    // the following WM_SYSKEYUP from the default window which is sent
                    // to the menu-bar window by WM_KEYDOWN message .
                    if (lParam & 0x20000000)
                        bSysKeyUp = TRUE;
                }
            }
            else
            {
                if(lParam & 0x20000000)
                    bSysKeyUp = TRUE;
                nAction = ACTION_ESCAPE;
            }
		}

		break;

    case VK_ESCAPE :
	case VK_F10:

        // don't response the combination key of ALT and the escape key.
        if (lParam & 0x20000000)
            break;

		nAction = ACTION_ESCAPE;

        break;

    case VK_ALT:    // the single ALT key is pressed down or up

        if (wMsgCmd == WM_KEYDOWN)
        {
            // don't response the ALT+Alpha up message which is come
            // from the default window(WM_SYSKEYUP) after quiting
            // the message loop by pressing down the ALT key.
            if (bSysKeyUp)
            {
                bSysKeyUp = FALSE;
                return;
            }
            
            if (!pMenuWndData->bEnterLoop)
            {
                ASSERT(!IsPopup(pMenuWndData));
                
                PostMessage(hWnd, wMsgCmd, wParam, lParam);
                
                GrabPen(hWnd);
                GrabKeyboard(hWnd);
                
                MenuMessageLoop(hWnd);
                
                if (pMenuWndData->pCurItem)
                {
                    DrawCurrentItem(hWnd, pMenuWndData, FALSE);
                    pMenuWndData->pCurItem = NULL;
                }
                
                UngrabPen();
                
#if (!NOKEYBOARD)
                
                UngrabKeyboard();
#endif
                
                return;
            }

            if (pMenuWndData->pCurItem)
                DrawCurrentItem(hWnd, pMenuWndData, FALSE);
            
            pMenuWndData->pCurItem = pMenuWndData->pMenu->pItems;
            
            if (pMenuWndData->pCurItem)
                DrawCurrentItem(hWnd, pMenuWndData, TRUE);
        }
        // if the ALT key is pressed down again and up at once, quit 
        // the message loop of the menu-bar.
        else if (wMsgCmd == WM_SYSKEYUP)
        {
            if (bSysKeyUp)
            {
                bSysKeyUp = FALSE;
                return;
            }

            nAction = ACTION_ESCAPE;
        }  
        // should quit all message loops after the ALT key is pressed down,
        // and set the bSysKeyUp flag to TRUE indicate that don't response
        // the following WM_SYSKEYUP from the default window which is sent
        // to the menu-bar window by WM_KEYDOWN message .
        else if (wMsgCmd == WM_SYSKEYDOWN && pMenuWndData->hwndParentMenu)
        {
            pMenuWndData->nExitCode = EXIT_DO_NOTHING;
            bSysKeyUp = TRUE;
        }
 
        break;

    default:    // response a ALT+Alpha key or a single Alpha key

        // judge whether the hot key is valid
        nValidHotKey = MENU_TrackHotKey(hWnd, pMenuWndData, wVirtKey);
        if (nValidHotKey == -1)  // invalid hot key
        {
            UINT uFlag;
            HWND hwndTarget;

            // the current window is the menu-bar and don't response
            // invalid hot key, so set the bSysKeyUp flag to TRUE.
            if (lParam & 0x20000000 && !pMenuWndData->hwndParentMenu)
                bSysKeyUp = TRUE;

            // send WM_MENUCHAR to the window which is the parent window
            // or the owner window of the current menu when the key is not
            // the valid hot key.
            hwndTarget = GetParent(hWnd);
            if (!hwndTarget)    // hWnd is a window of a submenu
                hwndTarget = GetWindow(hWnd, GW_OWNER);

            if (IsPopup(pMenuWndData))
                uFlag = MF_POPUP;
            else
                uFlag = 0;

            SendMessage(hwndTarget, WM_MENUCHAR, 
                (WPARAM)MAKELONG(wVirtKey, uFlag),
                (LPARAM)(pMenuWndData->pMenu));

            break;
        }

        if (lParam & 0x20000000)
        {
            // the other key is pressed down before the ALT key is released,
            // set the flag bSysKeyUp to FALSE indicate that no WM_SYSKEYUP
            // message will be sent to the default window.
            bSysKeyUp = FALSE;

            if (!pMenuWndData->bEnterLoop)
            {
                ASSERT(!IsPopup(pMenuWndData));

                PostMessage(hWnd, wMsgCmd, wParam, lParam);

                GrabPen(hWnd);
                GrabKeyboard(hWnd);

                MenuMessageLoop(hWnd);

                if (pMenuWndData->pCurItem)
                {
                    DrawCurrentItem(hWnd, pMenuWndData, FALSE);
                    pMenuWndData->pCurItem = NULL;
                }

                UngrabPen();

#if (!NOKEYBOARD)

                UngrabKeyboard();
    #endif
 
                return;
            }
        }

        if (pMenuWndData->pCurItem != NULL)
            DrawCurrentItem(hWnd, pMenuWndData, FALSE);

        pMenuWndData->pCurItem = &pMenuWndData->pMenu->pItems[nValidHotKey];

        // decide what action will happen.
        if (!IsPopup(pMenuWndData))
		    nAction = ACTION_SELECT_SUBMENU;
        else
        {
            if ((pMenuWndData->pCurItem->wFlags & MF_POPUP) && 
                pMenuWndData->pCurItem->hIDorPopup != 0) 
                nAction = ACTION_SELECT_SUBMENU;
            else
                nAction = ACTION_SELECT;
        }

        // when a submenu is choosed, need to redraw menu items.
        if (nAction == ACTION_SELECT_SUBMENU)
        {
            if (pMenuWndData->pCurItem != NULL)
                DrawCurrentItem(hWnd, pMenuWndData, TRUE);
        }
        else
        {
            // set the bSysKeyUp flag to TRUE indicate that don't response
            // the following WM_SYSKEYUP from the default window which is sent
            // to the menu-bar window by WM_KEYDOWN message .
            if (lParam & 0x20000000)
                bSysKeyUp = TRUE;
        }
            
        break;
    }
     
    // do some work according to the value of nAction
    MENU_Patch(hWnd, pMenuWndData, wMsgCmd, wParam, nAction);
}


/*
*  Judge whether the hot key is pressed down
*/
static int MENU_TrackHotKey(HWND hWnd, PMENUWNDDATA pMenuWndData, WORD wVirtKey)
{
    PMENUOBJ pMenu;
    LPSTR    lpName;
    int      i, nItemCount;
    WORD     wHotPos;

    pMenu = pMenuWndData->pMenu;
    nItemCount = pMenu->cUsedItems;

    for (i = 0; i < nItemCount; i++)
    {
        if (pMenu->pItems[i].xULStart == 0)
            continue;
        
        lpName = (LPSTR)(pMenu->pItems[i].hStrOrBmp);
        if (!lpName)
            continue;

        wHotPos = pMenu->pItems[i].xULStart;
        if (lpName[wHotPos] == wVirtKey)    // is valid hot key
            return i;
    }

    return -1;
}

#endif


/*
*  Implementate the function which pop up a menu at the position of
*  pen pressed down.
*/
BOOL MENU_TrackPopupMenu(PMENUOBJ pMenuObj, UINT uFlags, int x, int y, 
                         int nReserved, HWND hWnd, CONST RECT *prcRect)
{
    int  width, height;
//    int  i, nItemCount;
//    HDC  hdc;
//    SIZE size;
    RECT rcScreen;
    HWND hPopupWin;
    DWORD dwStyle;
    int  nBorderW, nBorderH;
//    BOOL bSubMenu = FALSE;

    ASSERT(pMenuObj != NULL);
    ASSERT(hWnd != NULL);

#if (INTERFACE_MONO)
    dwStyle = WS_POPUP | WS_VISIBLE | WS_TOPMOST | WS_VSCROLL;
#else
    dwStyle = WS_POPUP | WS_VISIBLE | WS_THICKFRAME | WS_TOPMOST;
#endif

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

    // has not parent menu
    pMenuObj->hParentMenuWin = 0;

    // set flag for popup menu
    pMenuObj->wFlags = MT_POPUP;

//    hdc = GetDC(hWnd);

    SendMessage(hWnd, WM_INITMENU, 
        (WPARAM)pMenuObj, 0);
    
    SendMessage(hWnd, WM_INITMENUPOPUP, 
        (WPARAM)pMenuObj, 
        (LPARAM)MAKELONG(0, FALSE));    
    // caculate the dimension of the menu
//    width = 0;
//    height = 0;
//
//    nItemCount = pMenuObj->cUsedItems;
//    for (i = 0; i < nItemCount; i++)
//    {
//        if (pMenuObj->pItems[i].wFlags & MF_POPUP)
//            bSubMenu = TRUE;
//
//        if ((LPSTR)(pMenuObj->pItems[i].hStrOrBmp))
//        {
//            MENU_GetTextExtent(hdc, &pMenuObj->pItems[i], &size);
//
//            if (size.cx > width)
//                width = size.cx;
//        }
//        else
//            GetTextExtent(hdc, "T", -1, &size);
// 
//        if (pMenuObj->pItems[i].wFlags & MF_SEPARATOR)
//            height += MI_SEPARATOR;
//        else
//            height += size.cy + MI_VGAP;
//    }
//
//    ReleaseDC(hWnd, hdc);
//
//    // patch the dimension of a popup menu
//    width += MI_LEFT_MARGIN + MI_RIGHT_MARGIN + 2 * nBorderW;
//    height += 2 * nBorderH;
//
//    if (bSubMenu)
//        width += MI_RIGHT_BITMAP;
//
//    height += MI_TOP_MARGIN;
//
//    // get the rect of the root window
////    GetClientRect(GetDesktopWindow(), &rcScreen);
//    GetClientRect(HWND_DESKTOP, &rcScreen);
//
//    // caculate (x,y) according to uFlags
//    if (uFlags & TPM_CENTERALIGN)
//    {
//        x -= width / 2;
//    }
//    else if (uFlags & TPM_RIGHTALIGN)
//    {
//        x -= width;
//    }
//
//    if (uFlags & TPM_BOTTOMALIGN)
//    {
//        y -= height;
//    }
//    else if (uFlags & TPM_VCENTERALIGN)
//    {
//        y -= height / 2;
//    }
//
//    // check whether the menu is beyond the screen and do 
//    // adjustment accordingly.
//    if (x < rcScreen.left)
//        x = rcScreen.left;
//    else if (x + width > rcScreen.right)
//    {
//        x = rcScreen.right - width;
//    }
//
//    if (y < rcScreen.top)
//        y = rcScreen.top;
//    else if (y + height > rcScreen.bottom)
//        y = rcScreen.bottom - height;

    GetWindowRect(hWnd, &rcScreen);
    x      = rcScreen.left;
    width  = rcScreen.right - rcScreen.left;

    GetWindowRectEx(hWnd, &rcScreen, W_CLIENT, XY_SCREEN);
    y      = rcScreen.bottom - CY_SLIDESTART;
    height = CY_SLIDESTART;
//    height = rcScreen.bottom - rcScreen.top + 2 * nBorderH;

    hPopupWin =  CreateWindow("#MENU", "", dwStyle, x, y, width, height,
        hWnd, (HMENU)(pMenuObj), NULL, NULL);

    SetTimer(hPopupWin, IDT_SLIDE, ET_SLIDE, NULL);

    if (!hPopupWin)
        return FALSE;

    // enter menu message loop
    MenuMessageLoop(hPopupWin);

    return TRUE;
}


/*
*  Handle some menu actions include popping up a submenu, choosing a menu
*  item, etc. 
*/
static void MENU_Patch(HWND hWnd, PMENUWNDDATA pMenuWndData, UINT wMsgCmd,
                       WPARAM wParam, int nAction)
{
    int             nExitCode;
    HWND            hwndPopup;

#if (!NOKEYBOARD)
    PMENUWNDDATA    pSubMenuWndData;
#endif

    if (nAction == ACTION_DO_NOTHING)
        return;

    switch (nAction)
    {

#if (!NOKEYBOARD)

	case ACTION_ESCAPE :

        pMenuWndData->nExitCode = EXIT_ESCAPE;
		break;
        
	case ACTION_KEYSELECT_R :       // keyboard action

        if (pMenuWndData->pCurItem && 
            (!(pMenuWndData->pCurItem->wFlags & MF_DISABLED)) &&
            (!(pMenuWndData->pCurItem->wFlags & MF_GRAYED)))
        {
		    pMenuWndData->msg.message = WM_KEYDOWN;
		    pMenuWndData->msg.hwnd = pMenuWndData->hwndParentMenu;
		    pMenuWndData->msg.wParam = wParam;
            pMenuWndData->msg.lParam = 0L;

            pMenuWndData->nExitCode = EXIT_SELECT;
        }

        break;
    case ACTION_KEYSELECT_L :
        if (pMenuWndData->pCurItem)
        {
		    pMenuWndData->msg.message = WM_KEYDOWN;
		    pMenuWndData->msg.hwnd = pMenuWndData->hwndParentMenu;
		    pMenuWndData->msg.wParam = wParam;
            pMenuWndData->msg.lParam = 0L;

            pMenuWndData->nExitCode = EXIT_SELECT;
        }

        break;
#endif

    case ACTION_SELECT_SUBMENU :        // select a submenu

        if (pMenuWndData->pCurItem && 
            ((pMenuWndData->pCurItem->wFlags & MF_POPUP) && 
            pMenuWndData->pCurItem->hIDorPopup != 0 ) &&
            (!(pMenuWndData->pCurItem->wFlags & MF_DISABLED)) &&
            (!(pMenuWndData->pCurItem->wFlags & MF_GRAYED)))
        {
            hwndPopup = CreatePopupMenuWindow(hWnd, pMenuWndData);
            if (!hwndPopup)
                break;
            
#if (!NOKEYBOARD)

			if (wMsgCmd == WM_KEYDOWN || wMsgCmd == WM_SYSKEYDOWN)
			{
				pSubMenuWndData = (PMENUWNDDATA)GetUserData(hwndPopup);

				if (!pSubMenuWndData->pCurItem)
				{
					pSubMenuWndData->pCurItem = pSubMenuWndData->pMenu->pItems; 
					DrawCurrentItem(hwndPopup, pSubMenuWndData, TRUE);
				}
				else
				{
					//if (pSubMenuWndData->pCurItem)
					nAction = ACTION_SELECT;
					//else                               // never arrive at else ??? 
						//pSubMenuWndData->nExitCode = EXIT_DO_NOTHING;
				}
			}
#endif					
			nExitCode = MenuMessageLoop(hwndPopup);
                
			if (nExitCode == EXIT_MULTIPOPUP || nExitCode == EXIT_ESCAPE)
			{
                GrabPen(hWnd);

#if (!NOKEYBOARD)

                GrabKeyboard(hWnd);
#endif
			}
			else
			{
				if (nExitCode != EXIT_SELECT && 
                    nExitCode != EXIT_DO_NOTHING)
				{
					GrabPen(hWnd);

#if (!NOKEYBOARD)

					GrabKeyboard(hWnd);
#endif
				}
				else
				{
		           pMenuWndData->nExitCode = EXIT_DO_NOTHING;
				}
			}
        }

        break;

    case ACTION_SELECT :        // select a menu command

        if (pMenuWndData->pCurItem && 
            (!(pMenuWndData->pCurItem->wFlags & MF_DISABLED)) &&
            (!(pMenuWndData->pCurItem->wFlags & MF_GRAYED)) &&
            (!(pMenuWndData->pCurItem->wFlags & MF_SEPARATOR)))
        {
            HWND hwndTarget;

			pMenuWndData->msg.message = WM_COMMAND;
            hwndTarget = GetParent(hWnd);
            if (!hwndTarget)    // hWnd is a window of a submenu
                hwndTarget = GetWindow(hWnd, GW_OWNER);
            pMenuWndData->msg.hwnd = hwndTarget;
			pMenuWndData->msg.wParam = 
                (WPARAM)MAKELONG(pMenuWndData->pCurItem->hIDorPopup, 0);
            pMenuWndData->msg.lParam = 0L;

            pMenuWndData->nExitCode = EXIT_SELECT;
        }

        break;
    }
}

/**********************************************************************
 * Function     MENU_Size
 * Purpose      
 * Params       hWnd, pMenuWndData, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static void MENU_Size(HWND hWnd, PMENUWNDDATA pMenuWndData, LPARAM lParam)
{
    SCROLLINFO si;
    RECT rc;
    HDC hdc = NULL;
    SIZE size;
    int nItems = 0, nHeight = 0;

    if (pMenuWndData->y != pMenuWndData->yFinal)
    {
        nHeight = pMenuWndData->cyFinal;
    }
    else
    {
        GetClientRect(hWnd, &rc);
        nHeight = rc.bottom - rc.top;
    }

    hdc = GetDC(hWnd);
    GetTextExtentPoint32(hdc, "T", -1, &size);
    ReleaseDC(hWnd, hdc);

#ifdef MI_CY_SUBTITLE
    if (pMenuWndData->hwndParentMenu != NULL)
    {
        nHeight -= MI_CY_SUBTITLE;
    }
#endif

    nItems = nHeight / (size.cy + MI_VGAP);

    memset((void*)&si, 0, sizeof(SCROLLINFO));

    si.cbSize = sizeof(SCROLLINFO);
    si.fMask  = SIF_RANGE | SIF_PAGE | SIF_DISABLENOSCROLL;
    si.nMin   = 0;
    si.nMax   = pMenuWndData->pMenu->cUsedItems - 1;
    si.nPage  = nItems;

    if (pMenuWndData->hwndParentMenu == NULL)
    {
        SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
    }
    else
    {
        SetScrollInfo(pMenuWndData->hwndVScroll, SB_CTL, &si, TRUE);
    }
}

/**********************************************************************
 * Function     MENU_KeyUp
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static void MENU_KeyUp(HWND hWnd, PMENUWNDDATA pMenuWndData, UINT wMsgCmd,
                       WPARAM wParam, LPARAM lParam)
{
    pMenuWndData->nRepeats = 0;

    switch (LOWORD(wParam))
    {
    case VK_DOWN:
    case VK_UP:
        KillTimer(hWnd, IDT_REPEAT);
        break;

    default:
        break;
    }
}

/**********************************************************************
 * Function     MENU_Timer
 * Purpose      
 * Params       hWnd, pMenuWndData, wParam
 * Return       
 * Remarks      
 **********************************************************************/

static void MENU_Timer(HWND hWnd, PMENUWNDDATA pMenuWndData, WPARAM wParam)
{
    switch (wParam)
    {
    case IDT_REPEAT:
        if (pMenuWndData->nRepeats == 1)
        {
            KillTimer(hWnd, wParam);
            SetTimer(hWnd, IDT_REPEAT, ET_REPEAT_LATER, NULL);
        }
        keybd_event(pMenuWndData->wKeyCode, 0, 0, 0);
        break;

    case IDT_SLIDE:
        pMenuWndData->cySlide += DCY_SLIDEONCE;
        if (pMenuWndData->cySlide < -DCY_SLIDEONCE)
        {
            pMenuWndData->cySlide = -DCY_SLIDEONCE;
        }
        pMenuWndData->y -= pMenuWndData->cySlide;
        pMenuWndData->cy += pMenuWndData->cySlide;
        if (pMenuWndData->y <= pMenuWndData->yFinal)
        {
            pMenuWndData->y  = pMenuWndData->yFinal;
            pMenuWndData->cy = pMenuWndData->cyFinal;
            KillTimer(hWnd, wParam);
            pMenuWndData->cySlide = CY_SLIDESTART;
        }
        MoveWindow(hWnd, pMenuWndData->x, pMenuWndData->y,
            pMenuWndData->cx, pMenuWndData->cy, FALSE);
        break;

    default:
        KillTimer(hWnd, wParam);
        break;
    }
}

/**********************************************************************
 * Function     MENU_Refresh
 * Purpose      
 * Params       hWnd, bRefresh
 * Return       
 * Remarks      
 **********************************************************************/

static void MENU_Refresh(HWND hWnd, PMENUWNDDATA pMenuWndData, BOOL bRefresh,
                         LPARAM lParam)
{
    RECT rc;
    int i = 0, nCurIndex = 0, *pnCurIndex = NULL;
    SCROLLINFO si;

    if (bRefresh)
    {
        nCurIndex = lParam;
        GetClientRect(hWnd, &rc);
        MENU_Size(hWnd, pMenuWndData, MAKELPARAM(rc.right - rc.left,
            rc.bottom - rc.top));

        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;
        if (pMenuWndData->hwndParentMenu == NULL)
        {
            GetScrollInfo(hWnd, SB_VERT, &si);
        }
        else
        {
            GetScrollInfo(pMenuWndData->hwndVScroll, SB_CTL, &si);
        }
        if ((nCurIndex >= si.nPos + (int)si.nPage - 1)
            && (si.nPos + (int)si.nPage <= si.nMax))
        {
            si.fMask = SIF_POS;
            si.nPos++;
            if (pMenuWndData->hwndParentMenu == NULL)
            {
                SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
            }
            else
            {
                SetScrollInfo(pMenuWndData->hwndVScroll, SB_CTL, &si, TRUE);
            }
        }
        else if (((nCurIndex <= si.nPos) || (si.nPos + (int)si.nPage > si.nMax))
            && (si.nPos > si.nMin))
        {
            si.fMask = SIF_POS;
            si.nPos--;
            if (pMenuWndData->hwndParentMenu == NULL)
            {
                SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
            }
            else
            {
                SetScrollInfo(pMenuWndData->hwndVScroll, SB_CTL, &si, TRUE);
            }
        }
        pMenuWndData->pCurItem = &pMenuWndData->pMenu->pItems[nCurIndex];
        InvalidateRect(hWnd, NULL, TRUE);
    }
    else
    {
        pnCurIndex = (int*)lParam;
        for (i = 0; i < pMenuWndData->pMenu->cUsedItems; i++)
        {
            if (&pMenuWndData->pMenu->pItems[i] == pMenuWndData->pCurItem)
                break;
        }
        *pnCurIndex = i;
    }
}

static void PaintMenuBar(HWND hWnd, HDC hdc, PMENUWNDDATA pMenuWndData,
                         const RECT* pRect)
{
    RECT      rect;
    SIZE      size;
    int       nItemCount, i, srcy, srcheight;
    PMENUOBJ  pMenu;

    // erase the background
    ClearRect(hdc, pRect, GetSysColor(COLOR_LIGHTGRAY));

    pMenu = pMenuWndData->pMenu;
    nItemCount = pMenu->cUsedItems;

    rect.left = pRect->left;
    rect.top = pRect->top;
    rect.bottom = pRect->bottom;

    for (i = 0; i < nItemCount; i++)
    {
        if (pMenu->pItems[i].wFlags & MF_SEPARATOR ||
            (!(pMenu->pItems[i].wFlags & MF_BITMAP) && 
            !(pMenu->pItems[i].hStrOrBmp)))
        {
            continue;
        }

        // caculate the rect of a menu item
        if ((LPSTR)(pMenu->pItems[i].hStrOrBmp))
            MENU_GetTextExtent(hdc, &pMenu->pItems[i], &size);
        else
            GetTextExtent(hdc, "T", -1, &size);

        rect.right = rect.left + size.cx + MI_HGAP * 2;
        
        srcy = 151 * i / nItemCount;
        srcheight = 151 / nItemCount;
        if (&pMenu->pItems[i] == pMenuWndData->pCurItem)  // the item is choosed.
            DrawMenuItem(hWnd, hdc, pMenu, &pMenu->pItems[i], &rect, srcy, srcheight,
                FALSE, TRUE);
        else
            DrawMenuItem(hWnd, hdc, pMenu, &pMenu->pItems[i], &rect, srcy, srcheight,
                FALSE, FALSE);
        
        rect.left = rect.right;
    }
}


static void PaintPopupMenu(HWND hWnd, HDC hdc, PMENUWNDDATA pMenuWndData, 
                           const RECT* pRect)
{
    RECT          rect;
    PMENUOBJ      pMenu;
    SIZE          size;
    int           nItemCount, i, srcy, srcheight;
    SCROLLINFO    si;
#ifdef MI_CY_SUBTITLE
    PMENUWNDDATA  pParentWndData = NULL;
    PMENUITEM     pParentItem = NULL;
    int           nMode = 0;
#endif

    rect.left = pRect->left;
    rect.right = pRect->right;
    rect.top = pRect->top;
    rect.bottom = pRect->bottom;

    pMenu = pMenuWndData->pMenu;

#ifdef MI_CY_SUBTITLE
    if (pMenu->hParentMenuWin != NULL)
    {
        pParentWndData = (PMENUWNDDATA)GetUserData(pMenu->hParentMenuWin);
        pParentItem = pParentWndData->pCurItem;

        rect.bottom = rect.top + MI_CY_SUBTITLE;

        ClearRect(hdc, &rect, CR_SUBTITLE);
        nMode = SetBkMode(hdc, TRANSPARENT);
        DrawText(hdc, (PSTR)pParentItem->hStrOrBmp, (int)pParentItem->cbItemLen,
            &rect, DT_CENTER | DT_VCENTER);
        SetBkMode(hdc, nMode);

        rect.top    += MI_CY_SUBTITLE;
        rect.bottom  = pRect->bottom;
        rect.right  -= GetSystemMetrics(SM_CXVSCROLL);
    }
#endif

//    StretchBlt(hdc, rect.left, rect.top, rect.right - rect.left, 20, (HDC)hbmpMenuTop,
//        0, 0, 157, 20, SRCCOPY);
//    StretchBlt(hdc, rect.left, rect.top + 20, rect.right - rect.left,
//        rect.bottom - rect.top - 20, (HDC)hbmpMenuBk, 0, 0,157, 151, SRCCOPY);

    rect.top += MI_TOP_MARGIN;
    nItemCount = pMenu->cUsedItems;

    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    
    if (pMenuWndData->hwndParentMenu == NULL)
    {
        GetScrollInfo(hWnd, SB_VERT, &si);
    }
    else
    {
        GetScrollInfo(pMenuWndData->hwndVScroll, SB_CTL, &si);
    }
    
    for (i = si.nPos; i < (int)(si.nPos + si.nPage); i++)
    {
        if (i > pMenu->cUsedItems - 1)
            break;
        
        if ((LPSTR)(pMenu->pItems[i].hStrOrBmp))
            MENU_GetTextExtent(hdc, &pMenu->pItems[i], &size);
        else
            GetTextExtent(hdc, "T", -1, &size);

        // caculate the rect of a menu item
        if (pMenu->pItems[i].wFlags & MF_SEPARATOR)
            rect.bottom = rect.top + MI_SEPARATOR;
        else
            rect.bottom = rect.top + size.cy + MI_VGAP;

        srcy = 151 * i / nItemCount;
        srcheight = 151 / nItemCount;
        if (&pMenu->pItems[i] == pMenuWndData->pCurItem) // the item is choosed.
            DrawMenuItem(hWnd, hdc, pMenu, &pMenu->pItems[i], 
                &rect, srcy, srcheight, TRUE, TRUE);
        else
            DrawMenuItem(hWnd, hdc, pMenu, &pMenu->pItems[i],
                &rect, srcy, srcheight, TRUE, FALSE);

        rect.top = rect.bottom;
    }
}

static HWND CreatePopupMenuWindow(HWND hWnd, PMENUWNDDATA pMenuWndData)
{
    RECT    rcScreen;
    int     x, y, width, height;

    PMENUOBJ pSubMenu;
    HANDLE   handle;
    HWND     hwndSubMenu = NULL, hwndParent;
//    HDC      hdc;
//    int      i, nItemCount;
//    SIZE     size;

    DWORD dwStyle;
    int  nBorderW, nBorderH;
//    BOOL bSubMenu = FALSE;;

#if (INTERFACE_MONO)
    dwStyle = WS_POPUP | WS_VISIBLE | WS_TOPMOST;
#else
    dwStyle = WS_POPUP | WS_VISIBLE | WS_DLGFRAME | WS_TOPMOST;
#endif

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

//    // Calculate the proper rectangle for popup menu
//    GetCurrentItemRect(hWnd, pMenuWndData, &rect);
//
//    WindowToScreen(hWnd, (int*)&rect.left, (int*)&rect.top);
//    WindowToScreen(hWnd, (int*)&rect.right, (int*)&rect.bottom);
//    
//    if (!IsPopup(pMenuWndData))
//    {
//        x = rect.left;
//        y = rect.bottom;
//    }
//    else
//    {
//        x = rect.right;
//        y = rect.top;
//    }

    // record the window of parent menu of the current menu
    handle = (HANDLE)pMenuWndData->pCurItem->hIDorPopup;
    if (!IsMenu((HMENU)handle))
        return FALSE;
    pSubMenu = (PMENUOBJ)handle;
    if (pSubMenu->cUsedItems == 0)
        return FALSE;

    pSubMenu->hParentMenuWin = (HANDLE)hWnd;

    // set flag for a menu which is not popped up by TrackPopupMenu
    pSubMenu->wFlags = MT_NORMAL;

//    hdc = GetDC(hWnd);

    // caculate the dimension of the menu
//    width = 0;
//    height = 0;
//
//    nItemCount = pSubMenu->cUsedItems;
//    for (i = 0; i < nItemCount; i++)
//    {
//        if (pSubMenu->pItems[i].wFlags & MF_POPUP)
//            bSubMenu = TRUE;
//
//        if ((LPSTR)(pSubMenu->pItems[i].hStrOrBmp))
//        {
//            MENU_GetTextExtent(hdc, &pSubMenu->pItems[i], &size);
//
//            if (size.cx > width)
//                width = size.cx;
//        }
//        else
//            GetTextExtent(hdc, "T", -1, &size);
//
//        if (pSubMenu->pItems[i].wFlags & MF_SEPARATOR)
//            height += MI_SEPARATOR;
//        else
//            height += size.cy + MI_VGAP;
//    }
//    height += MI_TOP_MARGIN;
//    ReleaseDC(hWnd, hdc);
//
//    // patch the dimension of a popup menu
//    width += MI_LEFT_MARGIN + MI_RIGHT_MARGIN + 2 * nBorderW;
//    height += 2 * nBorderH;
//
//    if (bSubMenu)
//        width += MI_RIGHT_BITMAP;
//
//    // get the rect of the root window
////    GetClientRect(GetDesktopWindow(), &rcScreen);
//    GetClientRect(HWND_DESKTOP, &rcScreen);
//    // invert the screen coordinate to the coordinate which is 
//    // related to the parent window.
//    if (x + width > rcScreen.right)
//    {
//        x = rcScreen.right - width;
//        if((x < rect.right) && (pMenuWndData->hwndParentMenu != NULL))
//            x = rect.left - width;
//    }
//    
//    if (y + height > rcScreen.bottom - CAPTIONHEIGHT - 3)
//    {
//        y = rcScreen.bottom - height - CAPTIONHEIGHT - 3;
//    }

    // get the parent window or owner window of the current window
    // which is the window of the menubar or the submenu.
    hwndParent = GetParent(hWnd);
    if (!hwndParent)    // hWnd is a window of a submenu
        hwndParent = GetWindow(hWnd, GW_OWNER);

    ASSERT(hwndParent != NULL);

    GetWindowRect(hwndParent, &rcScreen);
    x      = rcScreen.left;
    width  = rcScreen.right - rcScreen.left;

    GetWindowRectEx(hwndParent, &rcScreen, W_CLIENT, XY_SCREEN);
    y      = rcScreen.bottom - CY_SLIDESTART;
    height = CY_SLIDESTART;
//    height = rcScreen.bottom - rcScreen.top + 2 * nBorderH;

    hwndSubMenu = CreateWindow("#MENU", "", dwStyle, x, y, width, height,
        hwndParent, (HMENU)handle, NULL, NULL);

    SetTimer(hwndSubMenu, IDT_SLIDE, ET_SLIDE, NULL);

    return hwndSubMenu;
}

static void DrawCurrentItem(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                            BOOL bSelect)
{
    RECT rect;
    HDC hdc;
    int srcy, srcheight;
    int i, nItemCount;

    if (!pMenuWndData->pCurItem)
        return;

    if (pMenuWndData->pCurItem->wFlags & MF_SEPARATOR)
        return;

    hdc = GetDC(hWnd);

    GetCurrentItemRect(hWnd, pMenuWndData, &rect);


    nItemCount = pMenuWndData->pMenu->cUsedItems;
    if (nItemCount == 0)
    {
        ReleaseDC(hWnd, hdc);
        return;
    }
    for (i = 0; i < nItemCount; i++)
    {
        if (&pMenuWndData->pMenu->pItems[i] == pMenuWndData->pCurItem)
            break;
    }
    srcy = 151 * i / nItemCount;
    srcheight = 151 / nItemCount;
    if (!IsPopup(pMenuWndData))   // is menubar
        DrawMenuItem(hWnd, hdc, pMenuWndData->pMenu, 
            pMenuWndData->pCurItem, &rect, srcy, srcheight, FALSE, bSelect);
    else
        DrawMenuItem(hWnd, hdc, pMenuWndData->pMenu, 
            pMenuWndData->pCurItem, &rect, srcy, srcheight, TRUE, bSelect);

    ReleaseDC(hWnd, hdc);
}


static void GetCurrentItemRect(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                        PRECT pRect)
{
    PMENUOBJ pMenu;
    int      i, nItemCount, nIndex = 0;
    int      x, y;
    SIZE     size;
    RECT     rcClient;
    HDC      hdc;
    SCROLLINFO si;

    GetClientRect(hWnd, &rcClient);
    hdc = GetDC(hWnd);

    if (!IsPopup(pMenuWndData))      // is menubar
    {
        x = 0;

        pMenu = pMenuWndData->pMenu;
        nItemCount = pMenuWndData->pMenu->cUsedItems;

        i = 0;
        while (&pMenu->pItems[i] != pMenuWndData->pCurItem)
        {
            if ((LPSTR)(pMenu->pItems[i].hStrOrBmp))
                MENU_GetTextExtent(hdc, &pMenu->pItems[i], &size);
            else
                GetTextExtent(hdc, "T", -1, &size);

            x += size.cx + 2 * MI_HGAP;

            i++;
        }
        
        if ((LPSTR)(pMenu->pItems[i].hStrOrBmp))
            MENU_GetTextExtent(hdc, &pMenu->pItems[i], &size);
        else
            GetTextExtent(hdc, "T", -1, &size);

        SetRect(pRect, x, rcClient.top, (x + size.cx + 2 * MI_HGAP), 
            rcClient.bottom);
    }
    else    // is popup menu
    {
        y = MI_TOP_MARGIN;

        pMenu = pMenuWndData->pMenu;

#ifdef MI_CY_SUBTITLE
        if (pMenu->hParentMenuWin != NULL)
        {
            y += MI_CY_SUBTITLE;
            rcClient.right -= GetSystemMetrics(SM_CXVSCROLL);
        }
#endif

        nItemCount = pMenuWndData->pMenu->cUsedItems;

        for (nIndex = 0; nIndex < pMenuWndData->pMenu->cUsedItems; nIndex++)
        {
            if (&pMenuWndData->pMenu->pItems[nIndex] == pMenuWndData->pCurItem)
            {
                break;
            }
        }

        memset((void*)&si, 0, sizeof(SCROLLINFO));
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;

        if (pMenuWndData->hwndParentMenu == NULL)
        {
            GetScrollInfo(hWnd, SB_VERT, &si);
        }
        else
        {
            GetScrollInfo(pMenuWndData->hwndVScroll, SB_CTL, &si);
        }

//        i = 0;
//        while (&pMenu->pItems[i] != pMenuWndData->pCurItem)
        for (i = si.nPos; i < nIndex; i++)
        {
            if ((LPSTR)(pMenu->pItems[i].hStrOrBmp))
                MENU_GetTextExtent(hdc, &pMenu->pItems[i], &size);
            else
                GetTextExtent(hdc, "T", -1, &size);

            if (pMenu->pItems[i].wFlags & MF_SEPARATOR)
                y += MI_SEPARATOR;
            else
                y += size.cy + MI_VGAP;
        }

        if ((LPSTR)(pMenu->pItems[i].hStrOrBmp))
            MENU_GetTextExtent(hdc, &pMenu->pItems[i], &size);
        else
            GetTextExtent(hdc, "T", -1, &size);

        if (pMenu->pItems[i].wFlags & MF_SEPARATOR)
            SetRect(pRect, rcClient.left, y, rcClient.right, 
                (IWORD)(y + MI_SEPARATOR));
        else
            SetRect(pRect, rcClient.left, y, rcClient.right, 
                (IWORD)(y + size.cy + MI_VGAP));
    }

    ReleaseDC(hWnd, hdc);
}


/*
*  draw a specified menu item.
*/
static void DrawMenuItem(HWND hWnd, HDC hdc, PMENUOBJ pMenu, 
                         PMENUITEM pMenuItem, PRECT pRect, int srcy, int srcheight,
                         BOOL bPopup, BOOL bSelect)
{
    SIZE        size;
    COLORREF    clrBkOld = RGB(255, 255, 255), clrTextOld;
    BITMAP      bitmap;
    long        lx, ly; 
    HWND        hwndParent;
    UINT        uItem = 0;
//    int         nBkMode, nPathLen;
//    char        PathName[256];

    if (pMenuItem == NULL)
        return;

    if (bSelect)
    {
#if (INTERFACE_MONO)

        /*if (pMenuItem->wFlags & MF_GRAYED)
        {
            ClearRect(hdc, pRect, GetWindowColor(CAP_COLOR));
            clrBkOld = SetBkColor(hdc, GetWindowColor(CAP_COLOR));
        }
        else*/
        {
//            ClearRect(hdc, pRect, RGB(110, 169, 229));
//            clrBkOld = SetBkColor(hdc, RGB(110, 169, 229));
            GetObject(hbmpMenuFocus, sizeof(BITMAP), (void*)&bitmap);
            StretchBlt(hdc, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top,
                (HDC)hbmpMenuFocus, 0, 0, bitmap.bmWidth, bitmap.bmHeight, ROP_SRC);
        }

#else // INTERFACE_MONO
        ClearRect(hdc, pRect, GetSysColor(COLOR_HIGHLIGHT));
        clrBkOld = SetBkColor(hdc, GetSysColor(COLOR_HIGHLIGHT));
#endif

        // send a message used for notifing the owner window of this menu
        // that a menu item is selected.
        hwndParent = GetParent(hWnd);
        if (!hwndParent)    // hWnd is a window of a submenu
            hwndParent = GetWindow(hWnd, GW_OWNER);

        if (pMenuItem->wFlags & MF_POPUP)
        {
            int i;

            for (i = 0; i < pMenu->cUsedItems; i++)
            {
                if (pMenu->pItems[i].hIDorPopup == pMenuItem->hIDorPopup)
                {
                    uItem = i;
                    break;
                }
            }
        }
        else
            uItem = pMenuItem->hIDorPopup;
        
        SendMessage(hwndParent, WM_MENUSELECT, 
            (WPARAM)MAKELONG(uItem, pMenuItem->wFlags), 
            (LPARAM)(pMenu));
    }
    else
    {
       // ClearRect(hdc, pRect, GetWindowColor(CAP_COLOR));

        //clrBkOld = SetBkColor(hdc, GetWindowColor(CAP_COLOR));
//        StretchBlt(hdc, pRect->left, pRect->top, pRect->right - pRect->left, 
//            pRect->bottom - pRect->top, (HDC)hbmpMenuBk, 0, srcy, 157, srcheight, SRCCOPY);
        ClearRect(hdc, (const RECT*)pRect, GetSysColor(COLOR_MENU));
    }

    if (pMenuItem->wFlags & MF_GRAYED)
    {
#if (INTERFACE_MONO)
        clrTextOld = SetTextColor(hdc, RGB(128, 128, 128));
//        clrTextOld = SetTextColor(hdc, COLOR_MENUTEXT);
#else // INTERFACE_MONO
        if (GetDeviceCaps(hdc, BITSPIXEL) == 1 && !bSelect) // mono color
            clrTextOld = SetTextColor(hdc, COLOR_BLACK);
        else
            clrTextOld = SetTextColor(hdc, GetSysColor(COLOR_GRAYTEXT));
#endif // INTERFACE_MONO
    }       
    else
    {
        clrTextOld = SetTextColor(hdc, bSelect ? COLOR_WHITE : COLOR_BLACK);
    }

    if (!bPopup)
    {
        if (!(pMenuItem->wFlags & MF_SEPARATOR) &&
            (LPSTR)(pMenuItem->hStrOrBmp))
        {
            pRect->left += MI_HGAP;
            MENU_DrawText(hdc, pRect, pMenuItem, DT_VCENTER);
            pRect->left -= MI_HGAP;
        }
    }
    else
    {
        if (pMenuItem->wFlags & MF_SEPARATOR) // draw a separator
        {
#if (INTERFACE_MONO)
            SelectObject(hdc, GetStockObject(BLACK_PEN));
#else // INTERFACE_MONO
            if (GetDeviceCaps(hdc, BITSPIXEL) == 1)   // mono color
                SelectObject(hdc, GetStockObject(BLACK_PEN));
            else
                SelectObject(hdc, GetStockObject(GRAY_PEN));
#endif // INTERFACE_MONO
             
            GetTextExtent(hdc, "T", -1, &size);
//            DrawLine(hdc, pRect->left, 
//                pRect->top + (MI_SEPARATOR) / 2,
//                pRect->right, 
//                pRect->top + (MI_SEPARATOR) / 2);

#if (!INTERFACE_MONO)
            if (GetDeviceCaps(hdc, BITSPIXEL) != 1)   // is not mono color.
            {
                SelectObject(hdc, GetStockObject(WHITE_PEN));

                // drawing the line can make 3d effect
                DrawLine(hdc, pRect->left, 
                    pRect->top + (MI_SEPARATOR) / 2 + 1,
                    pRect->right, 
                    pRect->top + (MI_SEPARATOR) / 2 + 1);
            }
#endif // INTERFACE_MONO
        }
        else
        {
            // Draw checked mark if neccessary
            if (pMenuItem->wFlags & MF_CHECKED)
            {
                GetObject((HGDIOBJ)hbmpCheck, sizeof(BITMAP), &bitmap);

                lx = pRect->left + (MI_LEFT_MARGIN - bitmap.bmWidth) / 2;
                ly = pRect->top + 
                    (pRect->bottom - pRect->top - bitmap.bmHeight) / 2;

#if (INTERFACE_MONO)

                if (bSelect && (GetDeviceCaps(hdc, BITSPIXEL) == 1))
                {
                    BitBlt(hdc, lx, ly, bitmap.bmWidth, bitmap.bmHeight,
                        (HDC)hbmpCheck, 0, 0, ROP_NSRC);
                }
                else
                {
                    BitBlt(hdc, lx, ly, bitmap.bmWidth, bitmap.bmHeight,
                        (HDC)hbmpCheck, 0, 0, ROP_SRC);
                }

#else // INTERFACE_MONO

                BitBlt(hdc, lx, ly, bitmap.bmWidth, bitmap.bmHeight,
                    (HDC)hbmpCheck, 0, 0, ROP_SRC);

#endif // INTERFACE_MONO
            }

            // if the menu item is not null, display it.
            if ((LPSTR)(pMenuItem->hStrOrBmp))
            {
                // draw a menu item
                pRect->left += MI_LEFT_MARGIN;
                MENU_DrawText(hdc, pRect, pMenuItem, DT_VCENTER);
                pRect->left -= MI_LEFT_MARGIN;
            }

            // draw a arrow used for identifing a submenu
            if (pMenuItem->wFlags & MF_POPUP)    // can popup a submenu
            {
//                GetObject((HGDIOBJ)hbmpRightArrow, sizeof(BITMAP), &bitmap);
//
//                lx = pRect->right - 20;//- bitmap.bmWidth - 4;
//                ly = pRect->top + 3;//+ 
//                    //(pRect->bottom - pRect->top - bitmap.bmHeight) / 2;
//
//                nBkMode = SetBkMode(hdc, BM_NEWTRANSPARENT);
//                BkColor = SetBkColor(hdc, RGB(255, 255, 255));
//                PLXOS_GetResourceDirectory(PathName);
//                nPathLen = strlen(PathName);                
//                strcat(PathName, "menupop.bmp");
//                DrawBitmapFromFile(hdc, lx, ly, PathName, SRCCOPY);
//                SetBkMode(hdc, nBkMode);
//                SetBkColor(hdc, BkColor);

            }
        }
    }

    SetTextColor(hdc, clrTextOld);
    SetBkColor(hdc, clrBkOld);
}


/*
*  output the text of the menu item and draw an underline below
*  the hot key.
*/
static void MENU_DrawText(HDC hdc, PRECT pRect, PMENUITEM pMenuItem, 
                          UINT uFormat)
{
    WORD    wHotPos;
    BOOL    bHasTab;
    LPSTR   lpName;
    char    *sDisplayStr;
    int     nNew, nOld;
    SIZE    size, sizeOfHot;
    int     nNum, nBkMode;



    // recognize the hot key or the tab key and adjust the menu 
    // item.

    lpName = (LPSTR)(pMenuItem->hStrOrBmp);
    //if (!lpName)
    //    return;
    ASSERT(lpName != NULL);

    sDisplayStr = MemAlloc(pMenuItem->cbItemLen + TAB_LEN);
    memset(sDisplayStr, ' ', pMenuItem->cbItemLen + TAB_LEN);
                
    wHotPos = 0;
    bHasTab = FALSE;
    nNew = 0;
    nNum = 0;
    for (nOld = 0; nOld < pMenuItem->cbItemLen; nOld++)
    {
        if (lpName[nOld] == '&' && lpName[nOld + 1] != '&')
        {
            wHotPos = (WORD)(nOld + 1 - nNum);
            nNum ++;
        }
        else if (lpName[nOld] == '&' && lpName[nOld + 1] == '&')
        {
            sDisplayStr[nNew++] = lpName[nOld];
            nOld ++;
            nNum ++;
        }
        else if (lpName[nOld] == '\t' && (BYTE)lpName[nOld+1] < 0x80 &&
                 !bHasTab)
        {
            nNew += TAB_LEN;
            bHasTab = TRUE;
        }
        else
            sDisplayStr[nNew++] = lpName[nOld];
    }

    sDisplayStr[nNew] = '\0';

    if (pMenuItem->wFlags & MF_POPUP)
    {
        if (pMenuItem->cbItemLen + TAB_LEN <= (WORD)(nNew + strlen("...")))
        {
            nNew = pMenuItem->cbItemLen + TAB_LEN - 1 - strlen("...");
            sDisplayStr[nNew] = '\0';
        }
        strcat(sDisplayStr, "...");
    }

    nBkMode = SetBkMode(hdc, BM_TRANSPARENT);
   	DrawText(hdc, sDisplayStr, -1, pRect, uFormat);
    SetBkMode(hdc, nBkMode);
/*
#if (INTERFACE_MONO)
    if(pMenuItem->wFlags & MF_GRAYED)
    {
        y = pRect->top;
        SelectObject(hdc, GetStockObject(WHITE_PEN));
        while(y < pRect->bottom)
        {
            DrawLine(hdc, pRect->left, y, pRect->right, y);
            y += 2;
        }
    }
#endif // INTERFACE_MONO
*/

    // draw the underline under the hot key
    if (wHotPos != 0)
    {
#if (INTERFACE_MONO)
        SelectObject(hdc, GetStockObject(BLACK_PEN));
#else // INTERFACE_MONO
        if (pMenuItem->wFlags & MF_GRAYED && 
            GetDeviceCaps(hdc, BITSPIXEL) != 1)
            SelectObject(hdc, GetStockObject(GRAY_PEN));
        else
            SelectObject(hdc, GetStockObject(BLACK_PEN));
#endif // INTERFACE_MONO

        GetTextExtent(hdc, sDisplayStr, wHotPos - 1, &size);
        GetTextExtent(hdc, &sDisplayStr[wHotPos - 1], 1, &sizeOfHot);
                
//        DrawLine(hdc, pRect->left + size.cx, pRect->top + sizeOfHot.cy + 1,
//            pRect->left + size.cx + sizeOfHot.cx, 
//            pRect->top + sizeOfHot.cy + 1);
    }

    MemFree(sDisplayStr);
}


static BOOL PointInWindow(HWND hWnd, int x, int y)
{
    RECT rcWindow;

    GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_CLIENT);

    return PtInRectXY(&rcWindow, x, y);
}

static BOOL PointInOwnerItem(HWND hWnd, PMENUWNDDATA pMenuWndData,
                             int x, int y)
{
    PMENUWNDDATA pParentMenuWndData;
    RECT rect;
    HWND hWnd1;
    int x1, y1;

    if (!pMenuWndData->hwndParentMenu)
        return FALSE;
    x1 = x;
    y1 = y;
    hWnd1 = hWnd;
    pParentMenuWndData = pMenuWndData;
    
    while(pParentMenuWndData->hwndParentMenu)
    {
     
        WindowToScreen(hWnd1, &x1, &y1);
        ScreenToWindow(pParentMenuWndData->hwndParentMenu, &x1, &y1);
        hWnd1 = pParentMenuWndData->hwndParentMenu;
        pParentMenuWndData = GetUserData(pParentMenuWndData->hwndParentMenu);
        GetCurrentItemRect(hWnd1, pParentMenuWndData, &rect);
        
        if (pParentMenuWndData->hwndParentMenu)  // is not menu-bar
        {
#if (INTERFACE_MONO)
            rect.right += GetSystemMetrics(SM_CXBORDER);
#else
            rect.right += GetSystemMetrics(SM_CXFRAME);
#endif
        }
        if (PtInRectXY(&rect, x1, y1))
            return TRUE;
    }

    return FALSE;
}


static BOOL PointInMenus(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                         int x, int y)
{
    HWND hwndParentMenu, hwndParent;
    int iwScreenX, iwScreenY;

    iwScreenX = x;
    iwScreenY = y;

    WindowToScreen(hWnd, &iwScreenX, &iwScreenY);

    hwndParentMenu = pMenuWndData->hwndParentMenu;

    while (hwndParentMenu)
    {
        x = iwScreenX;
        y = iwScreenY;

        ScreenToWindow(hwndParentMenu, &x, &y);

        if (PointInWindow(hwndParentMenu, x, y))
            break;

        pMenuWndData = GetUserData(hwndParentMenu);
        hwndParentMenu = pMenuWndData->hwndParentMenu;
    }

    if (!hwndParentMenu)
        return FALSE;
    else
    {
        pMenuWndData = GetUserData(hwndParentMenu);
        hwndParent = pMenuWndData->hwndParentMenu;
        
        // if hwndParentMenu is the window of menu-bar, judge whether
        // (x,y) is in the valid area of the menu-bar.
        if (!hwndParent &&
            !MENU_PointInValidBar(hwndParentMenu, pMenuWndData, x, y))    
            return FALSE;
    }

    return TRUE;
}


static BOOL MENU_PointInValidBar(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                                 int x, int y)
{
    PMENUOBJ pMenu;
    int      nItemCount, i;
    int      nEndPos;
    SIZE     size;
    RECT     rcClient;
    HDC      hdc;

    GetClientRect(hWnd, &rcClient);
    hdc = GetDC(hWnd);

    nEndPos = 0;

    pMenu = pMenuWndData->pMenu;
    nItemCount = pMenuWndData->pMenu->cUsedItems;

    for (i = 0; i < nItemCount; i++)
    {
        if ((LPSTR)(pMenu->pItems[i].hStrOrBmp))
            MENU_GetTextExtent(hdc, &pMenu->pItems[i], &size);
        else
            GetTextExtent(hdc, "T", -1, &size);

        nEndPos += size.cx + 2 * MI_HGAP;
    }

    ReleaseDC(hWnd, hdc);
    if (x >= nEndPos || y >= rcClient.bottom)
        return FALSE;

    return TRUE;
}


/*
*  hillite the menu item according to the position of the mouse
*/
static void TrackMouse(HWND hWnd, PMENUWNDDATA pMenuWndData, int x, int y)
{
    PMENUITEM pNewItem;

    pNewItem = SearchItem(hWnd, pMenuWndData, x, y);

    if (pNewItem == pMenuWndData->pCurItem)
        return;

    if (pMenuWndData->pCurItem != NULL)
        DrawCurrentItem(hWnd, pMenuWndData, FALSE);

    pMenuWndData->pCurItem = pNewItem;

    if (pNewItem && !(pNewItem->wFlags & MF_SEPARATOR))
        DrawCurrentItem(hWnd, pMenuWndData, TRUE);
}


#if (!NOKEYBOARD)

static PMENUITEM nextItem(PMENUWNDDATA pMenuWndData)
{
	PMENUITEM	pCurItem = pMenuWndData->pCurItem;
    PMENUOBJ    pMenu;
    int         nItemCount, i;

    pMenu = pMenuWndData->pMenu;
    nItemCount = pMenu->cUsedItems;

    for (i = 0; i < nItemCount; i++)
    {
        if (&pMenu->pItems[i] == pCurItem)
            break;
    }

    if (i == nItemCount || (i+1) == nItemCount)
        pCurItem = pMenu->pItems;
    else
        pCurItem = &pMenu->pItems[++i];

    while(i+1 < nItemCount && (pCurItem->wFlags & MF_SEPARATOR))
		pCurItem = &pMenu->pItems[++i];

    if (pCurItem->wFlags & MF_SEPARATOR)
        pCurItem = pMenu->pItems;

	return pCurItem;
}


static PMENUITEM prevItem(PMENUWNDDATA pMenuWndData)
{
	PMENUITEM	pCurItem = pMenuWndData->pCurItem;
    PMENUOBJ    pMenu;
    int         nItemCount, i;

    pMenu = pMenuWndData->pMenu;
    nItemCount = pMenu->cUsedItems;

    for (i = 0; i < nItemCount; i++)
    {
        if (&pMenu->pItems[i] == pCurItem)
            break;
    }

    if (i == nItemCount || i == 0)
        pCurItem = &pMenu->pItems[nItemCount - 1];
    else
        pCurItem = &pMenu->pItems[--i];

    while(i - 1 >= 0 && (pCurItem->wFlags & MF_SEPARATOR))
		pCurItem = &pMenu->pItems[--i];

    if (pCurItem->wFlags & MF_SEPARATOR)
        pCurItem = &pMenu->pItems[nItemCount - 1];

	return pCurItem;
}

/*
*  hillite the current item according the direction of the key.
*/
static void trackKey(HWND hWnd, PMENUWNDDATA pMenuWndData, BOOL findNext)
{
	PMENUITEM	pNewItem;
    int i = 0, nIndex = 0, nPosPrev = 0;
    SCROLLINFO si;
    HDC hdc = NULL;
    SIZE size;
    RECT rc;
    BOOL bSpan = FALSE;

    // when no item is choosed, choose the first item automatically
    if (pMenuWndData->pCurItem == NULL) 
        pNewItem = pMenuWndData->pMenu->pItems;
    else
    {
        do {
            if (findNext)
                pNewItem = nextItem(pMenuWndData);
            else
                pNewItem = prevItem(pMenuWndData);
        } while (pNewItem && pNewItem->hStrOrBmp == 0);
    }

    if (pMenuWndData->pCurItem != NULL)
        DrawCurrentItem(hWnd, pMenuWndData, FALSE);

    if (pNewItem)
		pMenuWndData->pCurItem = pNewItem;

    if (pNewItem && !(pNewItem->wFlags & MF_SEPARATOR))
        DrawCurrentItem(hWnd, pMenuWndData, TRUE);

    for (i = 0; i < pMenuWndData->pMenu->cUsedItems; i++)
    {
        if (&pMenuWndData->pMenu->pItems[i] == pMenuWndData->pCurItem)
        {
            nIndex = i;
            break;
        }
    }

    memset((void*)&si, 0, sizeof(SCROLLINFO));
    si.cbSize = sizeof(SCROLLINFO);
    si.fMask = SIF_ALL;
    
    if (pMenuWndData->hwndParentMenu == NULL)
    {
        GetScrollInfo(hWnd, SB_VERT, &si);
    }
    else
    {
        GetScrollInfo(pMenuWndData->hwndVScroll, SB_CTL, &si);
    }

    nPosPrev = si.nPos;

    if (findNext)
    {
        if (nIndex != 0)
        {
            if ((nIndex - nPosPrev + 1 >= (int)si.nPage)
                && ((int)(nPosPrev + si.nPage) <= si.nMax))
            {
                si.nPos++;
            }
        }
        else
        {
            if (pMenuWndData->pMenu->cUsedItems > si.nPage)
            {
                si.nPos = si.nMin;
                bSpan = TRUE;
            }
        }
    }
    else
    {
        if (nIndex != si.nMax)
        {
            if ((nIndex <= si.nPos) && (nIndex != 0))
            {
                si.nPos--;
            }
        }
        else
        {
            if (pMenuWndData->pMenu->cUsedItems > si.nPage)
            {
                si.nPos = si.nMax - si.nPage + 1;
                bSpan = TRUE;
            }
        }
    }

    if (si.nPos != nPosPrev)
    {
        GetClientRect(hWnd, &rc);
        rc.top += MI_TOP_MARGIN;

        si.fMask = SIF_POS;

        if (pMenuWndData->hwndParentMenu == NULL)
        {
            SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
        }
        else
        {
            SetScrollInfo(pMenuWndData->hwndVScroll, SB_CTL, &si, TRUE);
            
#ifdef MI_CY_SUBTITLE
            rc.top   += MI_CY_SUBTITLE;
            rc.right -= GetSystemMetrics(SM_CXVSCROLL);
#endif
        }

        if (!bSpan)
        {
            hdc = GetDC(hWnd);
            GetTextExtent(hdc, "T", -1, &size);
            ReleaseDC(hWnd, hdc);

            ScrollWindow(hWnd, 0, (size.cy + MI_VGAP) * (nPosPrev - si.nPos),
                (const RECT*)&rc, (const RECT*)&rc);
            UpdateWindow(hWnd);
        }
        else
        {
            InvalidateRect(hWnd, (const RECT*)&rc, TRUE);
        }
    }
}

#endif


/*
*  which menu item is choosed according to the mouse's position.
*/
static PMENUITEM SearchItem(HWND hWnd, PMENUWNDDATA pMenuWndData, 
                            int x, int y)
{
    SIZE size;
    int x1, y1;
    HDC hdc;

    PMENUOBJ pMenu;
    int      nItemCount, i;

    if (!PointInWindow(hWnd, x, y))
        return NULL;

    pMenu = pMenuWndData->pMenu;
    nItemCount = pMenu->cUsedItems;

    hdc = GetDC(hWnd);

    if (!IsPopup(pMenuWndData))
    {
        x1 = 0;

        for ( i = 0; i < nItemCount; i++)
        {
            if ((LPSTR)(pMenu->pItems[i].hStrOrBmp))
                MENU_GetTextExtent(hdc, &pMenu->pItems[i], &size);
            else
                GetTextExtent(hdc, "T", -1, &size);

            if (x < x1 + size.cx + 2 * MI_HGAP)
                break;

            x1 += size.cx + 2 * MI_HGAP;
        }
    }
    else
    {
        y1 = MI_TOP_MARGIN;
        if (y < y1)
            return NULL;

        for ( i = 0; i < nItemCount; i++)
        {
            if ((LPSTR)(pMenu->pItems[i].hStrOrBmp))
                MENU_GetTextExtent(hdc, &pMenu->pItems[i], &size);
            else
                GetTextExtent(hdc, "T", -1, &size);

            if (pMenu->pItems[i].wFlags & MF_SEPARATOR)
            {
                if (y < y1 + MI_SEPARATOR)
                    break;
                
                y1 += MI_SEPARATOR;
            }
            else
            {
                if (y < y1 + size.cy + MI_VGAP)
                    break;
                
                y1 += size.cy + MI_VGAP;
            }
        }
    }

    ReleaseDC(hWnd, hdc);

    if (i == nItemCount)
        return NULL;

    return &pMenu->pItems[i];
}

/*
*  internal message loop of the menu
*/
static int MenuMessageLoop(HWND hWnd)
{
    MSG     msg;
    BOOL    bExit;
    int     nExitCode = 0, i;
    HWND    hwndParent = NULL, hwndTarget;
    UINT    uItem = 0;

    PMENUWNDDATA pMenuWndData, pParentMenuWndData;

    pMenuWndData = (PMENUWNDDATA)GetUserData(hWnd);

    // send WM_INITMENU or WM_INITMENUPOPUP to the window
    // which is the parent window or the owner window of 
    // the current menu.

    hwndTarget = GetParent(hWnd);
    if (!hwndTarget)    // hWnd is a window of a submenu
        hwndTarget = GetWindow(hWnd, GW_OWNER);
    
    // if it is a menu-bar or a popup menu by tracking.
    if (!pMenuWndData->hwndParentMenu)
    {
        // Send these messages before create window in order to 
        // calculate the size of window.
        /*SendMessage(hwndTarget, WM_INITMENU, 
            (WPARAM)pMenuWndData->pMenu, 0);

        if (pMenuWndData->pMenu->wFlags == MT_POPUP)
            SendMessage(hwndTarget, WM_INITMENUPOPUP, 
                (WPARAM)pMenuWndData->pMenu, 
                (LPARAM)MAKELONG(0, FALSE));*/
    }
    else // is submenu
    {
        pParentMenuWndData = 
            (PMENUWNDDATA)GetUserData(pMenuWndData->hwndParentMenu);
        
        for (i = 0; i < pParentMenuWndData->pMenu->cUsedItems; i++)
        {
            if ((HANDLE)pParentMenuWndData->pMenu->pItems[i].hIDorPopup == 
                pMenuWndData->pMenu)
            {
                uItem = i;
                break;
            }
        }

        SendMessage(hwndTarget, WM_INITMENUPOPUP, 
            (WPARAM)pMenuWndData->pMenu,
            (LPARAM)MAKELONG(uItem, FALSE));
    }

    pMenuWndData->bEnterLoop = TRUE;
    pMenuWndData->nExitCode = EXIT_WAIT;

    bExit = FALSE;

    while (!bExit)
    {
        // Get current message
        GetMessage(&msg, 0, 0, 0);

        DispatchMessage(&msg);

        if (IsWindow(hWnd))
        {
            if (pMenuWndData->nExitCode != EXIT_WAIT && 
                pMenuWndData->nExitCode != EXIT_MULTIPOPUP)
            {
                bExit = TRUE;

                nExitCode = pMenuWndData->nExitCode;

                switch (nExitCode)
                {
                case EXIT_SELECT :

                    memcpy(&msg, &pMenuWndData->msg, sizeof(MSG));

                    break;

                case EXIT_RESEND_MESSAGE :

                    ResendMouseMessage(msg.hwnd, msg.message, 
                        msg.wParam, msg.lParam);

                    break;

                default: 

                    break;
                }

                if (nExitCode == EXIT_SELECT)
                {
                    PostMessage(msg.hwnd, msg.message, msg.wParam, 
                        msg.lParam);
                }

                if (nExitCode == EXIT_SELECT && msg.message == WM_KEYDOWN)
                {
                    hwndParent = pMenuWndData->hwndParentMenu;
                }
                
                if (nExitCode == EXIT_SELECT && 
                    msg.message == WM_KEYDOWN && hwndParent != 0)
                {
                    nExitCode = EXIT_MULTIPOPUP;

                    pParentMenuWndData = (PMENUWNDDATA)GetUserData(hwndParent);
                    if (pParentMenuWndData->nExitCode == 0)
                        pParentMenuWndData->nExitCode = EXIT_MULTIPOPUP;
                }
                
            }
        }
        else
            bExit = TRUE;
    }

    // if the menu is a popup menu, destroy the window of owning 
    // the menu.
    if (IsWindow(hWnd))
    {
        if (IsPopup(pMenuWndData))
            DestroyWindow(hWnd);
        else
            pMenuWndData->bEnterLoop = FALSE;
    }

    return nExitCode;
}


static void WindowToScreen(HWND hWnd, int* left, int* top)
{
	RECT rect;

    GetWindowRectEx(hWnd, &rect, W_CLIENT, XY_SCREEN);
	
	*left += rect.left;
	*top += rect.top;
}

static void ScreenToWindow(HWND hWnd, int* left, int* top)
{
	RECT rect;

    GetWindowRectEx(hWnd, &rect, W_CLIENT, XY_SCREEN);
	
	*left -= rect.left;
	*top -= rect.top;
}


/*
*  get the size of the specified menu item
*/
static void MENU_GetTextExtent(HDC hdc, PMENUITEM pMenuItem, PSIZE pSize)
{
    SIZE sizeOfAlpha;

    //if ((LPSTR)(pMenuItem->hStrOrBmp) == NULL)
    //    return;
    ASSERT((LPSTR)(pMenuItem->hStrOrBmp) != NULL);

    GetTextExtent(hdc, (LPSTR)(pMenuItem->hStrOrBmp), -1, pSize);

    GetTextExtent(hdc, "T", -1, &sizeOfAlpha);

    // there is a tab key
    if (pMenuItem->xTab != 0)
        pSize->cx += (TAB_LEN - 1) * sizeOfAlpha.cx;

    // there is a "&" key
    if (pMenuItem->xULNum != 0)
        pSize->cx -= sizeOfAlpha.cx * pMenuItem->xULNum;
}

/*
static UINT MENU_GetSubMenuIndex(PMENUOBJ pMenu, DWORD hIDorPopup)
{
    WORD i;

    for (i = 0; i < pMenu->cItems; i++)
    {
        if (pMenu->pItems[i].hIDorPopup == hIDorPopup)
            return i;
    }

    return 0xFFFFFFFF;
}
*/

#else   // NOMENUS

static void UnusedFunc(void)
{
}

#endif  // NOMENUS

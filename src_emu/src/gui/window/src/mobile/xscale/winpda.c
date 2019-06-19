/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 
 *
\**************************************************************************/


#include "hpwin.h"

#include "../../src/ui/control.h"

#include "windowx.h"
#include "str_public.h"
#include "str_plx.h"
#include "string.h"

#include "winpda.h"
#include "plx_pdaex.h"

#define ARROW_BMP_WIDTH     UDARROW_BMP_WIDTH
#define ARROW_BMP_HEIGHT    UDARROW_BMP_HEIGHT
#define ARROW_RECT_WIDTH    (UDARROW_BMP_WIDTH + 8)

// WM_NCHITTEST返回值，用于指定鼠标在Caption的button区域
#define HTCAPTIONBUTTON     30
#define HTCAPTIONSCSTATE    31  
#define ARROW_GAP           58
#define MAX_BUTTON_TEXT     16

#define BMP_WIDTH           0
#define BMP_HEIGHT          6

#define BTN_WIDTH			78
#define BTN_TEXT_WIDTH      (BTN_WIDTH - MENU_LEFT_GAP - MENU_RIGHT_GAP)
#define ICON_UP_WIDTH       7
#define ICON_UP_HEIGHT      4
#define ICON_DOT_WIDTH      5

#define SEF_SMARTPHONE

#ifdef SEF_SMARTPHONE
#define APPICONWIDTH        20
#define APPICONHEIGHT		28
#endif //SEF_SMARTPHONE


static const BYTE UPDATA[] = 
{
    0xFB,0xE0,0x00,0x00,      // 11111011111000000000000000000000
    0xF1,0xE0,0x00,0x00,      // 11110001111000000000000000000000
    0xE0,0xE0,0x00,0x00,      // 11100000111000000000000000000000
    0xC0,0x60,0x00,0x00,      // 11000000011000000000000000000000
    0x80,0x20,0x00,0x00,      // 10000000001000000000000000000000
    0x00,0x00,0x00,0x00,      // 00000000000000000000000000000000
};
static const BYTE DOWNDATA[] = 
{
    0x00,0x10,0x00,0x00,      // 00000000000100000000000000000000
    0x80,0x30,0x00,0x00,      // 10000000001100000000000000000000
    0xC0,0x70,0x00,0x00,      // 11000000011100000000000000000000
    0xE0,0xF0,0x00,0x00,      // 11100000111100000000000000000000
    0xF1,0xF0,0x00,0x00,      // 11110001111100000000000000000000
    0xFB,0xF0,0x00,0x00,      // 11111011111100000000000000000000
};
static const BYTE LEFTDATA[] = 
{
    0xF8,0x00,0x00,0x00,      // 11111000000000000000000000000000
    0xF0,0x00,0x00,0x00,      // 11110000000000000000000000000000
    0xE0,0x00,0x00,0x00,      // 11100000000000000000000000000000
    0xC0,0x00,0x00,0x00,      // 11000000000000000000000000000000
    0x80,0x00,0x00,0x00,      // 10000000000000000000000000000000
    0x00,0x00,0x00,0x00,      // 00000000000000000000000000000000
    0x80,0x00,0x00,0x00,      // 10000000000000000000000000000000
    0xC0,0x00,0x00,0x00,      // 11000000000000000000000000000000
    0xE0,0x00,0x00,0x00,      // 11100000000000000000000000000000
    0xF0,0x00,0x00,0x00,      // 11110000000000000000000000000000
    0xF8,0x00,0x00,0x00,      // 11111000000000000000000000000000
};
static const BYTE RIGHTDATA[] = 
{
    0x7C,0x00,0x00,0x00,      // 01111100000000000000000000000000
    0x3C,0x00,0x00,0x00,      // 00111100000000000000000000000000
    0x1C,0x00,0x00,0x00,      // 00011100000000000000000000000000
    0x0C,0x00,0x00,0x00,      // 00001100000000000000000000000000
    0x04,0x00,0x00,0x00,      // 00000100000000000000000000000000
    0x00,0x00,0x00,0x00,      // 00000000000000000000000000000000
    0x04,0x00,0x00,0x00,      // 00000100000000000000000000000000
    0x0C,0x00,0x00,0x00,      // 00001100000000000000000000000000
    0x1C,0x00,0x00,0x00,      // 00011100000000000000000000000000
    0x3C,0x00,0x00,0x00,      // 00111100000000000000000000000000
    0x7C,0x00,0x00,0x00,      // 01111100000000000000000000000000
};

// Two internal function used to DefWindowProc to create and destroy
// data.
#define WM_DATACREATE   0x03FE
#define WM_DATADESTROY  0x03FF

//#define MENU_TEXT   "菜单"
//#define MENU_INTER   "进入"
//#define MENU_OK      "确定"
//#define RETURN_TEXT  "返回"

#define MENU_LEFT_GAP       2
#define MENU_TEXT_GAP       3
#define MENU_RIGHT_GAP      5           // at least 3
#define CX_MENUICON         18
#define CY_BOUNDARY         0

typedef struct
{
    RECT rect;                          // button rectangle
    int id;                             // id of button
    BOOL bDown;                         // state of button : down or up
    char text[16];
    char szDisplay[16];
    //HBITMAP hbmp;                       // button bitmap
} BUTTON, *PBUTTON;

#define MAX_BUTTONS 2   

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

    // For caption button
    //int nButtons;
    BUTTON buttons;
    BUTTON LeftButton;
    //BOOL   bDown;
    //int ScrollState;      //记录笔点击在哪个箭头上。
    int  SetScrollState;    //记录当前需要显示的箭头。
    //int  ScrollMask;
    int  MenuState;

    HWND    hwndFocus;
    HWND    hWndTmp;
    BOOL    bSignal;
    BOOL    bPower;
	BOOL	bPActivate;

    char IconName[128];
	char IconName2[128];
    HBITMAP hBitmap;
    HBITMAP hBitmap2;
    char MenuText[50];
    char UseMenu[50];
    char  oldText[3][32];
    char szCapBmpName[128];
} WNDDATA, *PWNDDATA;

static HBITMAP  hbmpUp, hbmpDown, hbmpLeft, hbmpRight;
static HBRUSH   hbrScrollBar, hFillBrush;
static BOOL     bInit;
static COLORREF CapColor, BarColor, BtnColor, FocusColor;
static char Win_SigIcon[128] = "\0", Win_PowIcon[128] = "\0", Win_PhoneIcon[128] = "\0";

// Part consant for DoNCPaint
#define NC_MENU         0x0001  // Draw menu area
#define NC_WINDOWTEXT   0x0002  // Draw window text
#define NC_BUTTON       0x0004  // Draw non-client button only
#define NC_SCROLLSTATE  0x0008
#define NC_MENUBAR      0x0005
#define NC_CAPTION      0x000F  // Draw window bottom caption
//#define NC_NOCAPTION    0x0008  // Draw non-client except caption
#define NC_ALL          0x000F  // Draw all window non client

// Internal function prototypes

static BOOL WINPDA_Init(void);
static BOOL WINPDA_Exit(void);
static int  WINPDA_SetScrollInfo(HWND hWnd, int nBar, 
                                 PCSCROLLINFO pScrollInfo, BOOL bRedraw);
static BOOL WINPDA_GetScrollInfo(HWND hWnd, int nBar, 
                                 PSCROLLINFO pScrollInfo);
static BOOL WINPDA_EnableScrollBar(HWND hWnd, int nBar, int nArrow);
static int  WINPDA_SetScrollPos(HWND hWnd, int nBar, int nPos, 
                                BOOL bRedraw); 
static int  WINPDA_GetScrollPos(HWND hWnd, int nBar); 
static BOOL WINPDA_SetScrollRange(HWND hWnd, int nBar, int nMinPos, 
                                  int nMaxPos, BOOL bRedraw); 
static BOOL WINPDA_GetScrollRange(HWND hWnd, int nBar, LPINT lpMinPos, 
                                  LPINT lpMaxPos); 

static void DoNCCalcSize(HWND hWnd, PWNDDATA pWndData, WPARAM wParam, 
                         LPARAM lParam);
static void DoNCPaint(HWND hWnd, PWNDDATA pWndData, int nPart);
static void DoCancelMode(HWND hWnd, PWNDDATA pWndData);
static void DoNCActivate(HWND hWnd, PWNDDATA pWndData, BOOL bActive);
static BOOL EraseBkgnd(HWND hWnd, UINT message, HDC hdc);
static void ScreenToParent(HWND hWnd, PRECT pRect);

static BOOL DoSetCursor(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void GetWindowRectToParent(HWND hWnd, PRECT pRect);
static void CalcCaptionRect(HWND hWnd, PWNDDATA pWndData, PRECT prcBottomCaption, 
                            PRECT prcTopCaption, PRECT prcMenu, PRECT prcText,
                            PRECT prcButton);
static void DrawButtonRect(HDC hdc, const RECT* pRect, BOOL bLeft);
static LRESULT CALLBACK ListWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, 
                                    LPARAM lParam);
static int CalcListBoxHeight(HWND hwndListBox);
static int GetBorderWidth(DWORD dwStyle);
static int GetBorderHeight(DWORD dwStyle);
static HWND GetTopWindow(HWND hWnd);

static PWNDDATA GetWndData(HWND hWnd);

static LRESULT WINAPI FrameDefWindowProc(HWND hWnd, UINT message, WPARAM wParam, 
                                LPARAM lParam);


//extern void PaintIcon(HDC hdc, PCSTR pszIconName, int x, int y, int iw,int ih);


/*
**  Function : RegisterPDAWindowClass
**  Purpose  :
**      Registers PDA default window class.
*/
BOOL WINPDA_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = 0;
    wc.lpfnWndProc      = PDADefWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       =  sizeof(WNDDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = PDADEFWINDOWCLASSNAME;

    if (!RegisterClass(&wc))
        return FALSE;

    wc.style            = 0;
    wc.lpfnWndProc      = FrameDefWindowProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       =  sizeof(WNDDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "FRAMEWINDOW";

    if (!RegisterClass(&wc))
        return FALSE;

    if (!WINPDA_Init())
        return FALSE;

    hFillBrush = CreateSolidBrush(COLOR_TRANSBK);  

    return TRUE;
}

/*
**  Function : PDADefWindowProc
**  Purpose  :
**      Window proc for default window.
*/
LRESULT WINAPI PDADefWindowProc(HWND hWnd, UINT message, WPARAM wParam, 
                                LPARAM lParam)
{
    LRESULT lResult;
    PWNDDATA pWndData;
    PCREATESTRUCT pcs;
    PAINTSTRUCT ps;
    RECT rect, rcClient;
    PSTR pszText;
    HWND hwndParent, hChildLast;
    HDC hdc;
    int nBorderW, nBorderH;
    int nMaxCount, nTextLen;    
    SIZE szChar;
    int scdata;
    DWORD dwStyle;
    char * GetText;
    HWND hwndFocus;
    
    // Gets the user data pointer of the specified window, if the result 
    // pointer is NULL, indicats that the window handle is a invalid 
    // handle(maybe the window has been destroyed), just return.
//    pUserData = GetUserData(hWnd);
//    if (!pUserData)
//        return 0;
//
//    // Gets the internal data for default window
//    pWndData = *(PWNDDATA*)(pUserData - 4);

    // Gets the internal data for default window
    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return 0;
    
    lResult = (LRESULT)TRUE;

    switch (message)
    { 
    case WM_DATACREATE :

        if (!bInit)
        {
            if (!WINPDA_Init())
                return (LRESULT)FALSE;

            bInit = TRUE;
        }

        pcs = (PCREATESTRUCT)lParam;

//        // Initializes the window data
//        pWndData = MemAlloc(sizeof(WNDDATA));
//        if (!pWndData)
//            return (LRESULT)FALSE;
//
//        *(DWORD*)(pUserData - 4) = (DWORD)pWndData;
        memset(pWndData, 0, sizeof(WNDDATA));

        // Saves the window name field
        if (pcs->lpszName)
        {
            pWndData->pszText = MemAlloc(strlen(pcs->lpszName) + 1);
            if (pWndData->pszText)
                strcpy(pWndData->pszText, pcs->lpszName);
        }

        nBorderW = GetBorderWidth(pcs->style);
        nBorderH = GetBorderHeight(pcs->style);

        // Initializes buttons

        pWndData->buttons.id = NULL;
        pWndData->LeftButton.id = NULL;
        pWndData->SetScrollState = 0;
        //pWndData->ScrollMask = 0;
        pWndData->MenuState = 0;
        //pWndData->ScrollState = 0;

#ifdef SEF_SMARTPHONE
        pWndData->IconName[0] = '\0';
		pWndData->IconName2[0] = '\0';
        pWndData->hBitmap = NULL;
        pWndData->hBitmap2 = NULL;
        pWndData->bSignal = FALSE;
        pWndData->bPower = FALSE;
        pWndData->MenuText[0] = '\0';
#endif //SEF_SMARTPHONE

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
        dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
        if (dwStyle & WS_CHILD)
        {
            hChildLast = GetWindow(hWnd, GW_HWNDNEXT);
            SendMessage(hChildLast, PWM_ACTIVATE, WA_INACTIVE, NULL);
			pWndData->bPActivate = TRUE;
//            PostMessage(hWnd, PWM_ACTIVATE, WA_ACTIVE, NULL);
        }

        break;

    case WM_SHOWWINDOW:
        
        if (wParam == FALSE)
        {
            dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & WS_CHILD)
                SendMessage(hWnd, PWM_ACTIVATE, WA_INACTIVE, NULL);
        }
            
        break;

    case PWM_ACTIVATE:
        if (wParam == WA_INACTIVE)
        {
			if(!pWndData->bPActivate)
				break;
			pWndData->bPActivate = FALSE;
            pWndData->hwndFocus = GetFocus();
            hwndFocus = pWndData->hwndFocus;
            while(hwndFocus != NULL && hwndFocus != hWnd)
                hwndFocus = GetParent(hwndFocus);
            if (hwndFocus == NULL)
                pWndData->hwndFocus = NULL;
        }
        else if (wParam == WA_ACTIVE)
        {
			if(pWndData->bPActivate)
				break;
			pWndData->bPActivate = TRUE;
			dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
            if (IsWindow(pWndData->hwndFocus) && (dwStyle & WS_VISIBLE))
                SetFocus(pWndData->hwndFocus);
            else
                SetFocus(hWnd);
        }
        break;

    case WM_NCCALCSIZE :

        DoNCCalcSize(hWnd, pWndData, wParam, lParam);
        return 0;

    case WM_NCPAINT :

        DoNCPaint(hWnd, pWndData, NC_ALL);
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

        return (LRESULT)EraseBkgnd(hWnd, message, (HDC)wParam);

    case WM_ACTIVATE :

        // By default, windows set the focus to themselves when activated.
        {
            if ((BOOL)wParam)
            {
                if (IsWindow(pWndData->hwndFocus))
                    SetFocus(pWndData->hwndFocus);
                else
                    SetFocus(hWnd);
            }
            else
            {
                pWndData->hwndFocus = GetFocus();
                hwndFocus = pWndData->hwndFocus;
                while(hwndFocus != NULL && hwndFocus != hWnd)
                    hwndFocus = GetParent(hwndFocus);
                if (hwndFocus == NULL)
                    pWndData->hwndFocus = NULL;
            }

        break;
        }


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

    case WM_DATADESTROY :

        if (pWndData)
        {
            HMENU hmenu;

            // 直接删除hwndListWnd以加快窗口的删除速度
            dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & WS_CHILD)
                SendMessage(GetParent(hWnd), PWM_CLOSEWINDOWEX, (WPARAM)hWnd, NULL);
            if (pWndData->pszText)
                MemFree(pWndData->pszText);

            if (pWndData->pVScrollData)
                MemFree(pWndData->pVScrollData);

            if (pWndData->pHScrollData)
                MemFree(pWndData->pHScrollData);

            if (!(GetWindowLong(hWnd, GWL_STYLE) & (WS_CHILD | WS_NCCHILD)))
            {
                hmenu = (HMENU)GetWindowLong(hWnd, GWL_ID);
                if (hmenu)
                    DestroyMenu(hmenu);
            }
//            MemFree(pWndData);
//
//            // 窗口结构中的数据指针清空表示窗口数据已被删除
//            *(DWORD*)(pUserData - 4) = 0;
        }

        break;
        
    case PWM_GETSCROLLSTATE:
        return pWndData->SetScrollState;
    case PWM_SETSCROLLSTATE:
//        lResult = pWndData->SetScrollState;
//        if (lParam == MASKALL)
//            pWndData->SetScrollState = wParam;
//        else
//        {
//            if (lParam & MASKUP)
//            {
//                if (wParam & SCROLLUP)
//                    pWndData->SetScrollState |= SCROLLUP;
//                else
//                    pWndData->SetScrollState &= (0xFFFF - SCROLLUP);
//            }
//            if (lParam & MASKDOWN)
//            {
//                if (wParam & SCROLLDOWN)
//                    pWndData->SetScrollState |= SCROLLDOWN;
//                else
//                    pWndData->SetScrollState &= (0xFFFF - SCROLLDOWN);
//            }
//            if (lParam & MASKLEFT)
//            {
//                if (wParam & SCROLLLEFT)
//                    pWndData->SetScrollState |= SCROLLLEFT;
//                else
//                    pWndData->SetScrollState &= (0xFFFF - SCROLLLEFT);
//            }
//            if (lParam & MASKRIGHT)
//            {
//                if (wParam & SCROLLRIGHT)
//                    pWndData->SetScrollState |= SCROLLRIGHT;
//                else
//                    pWndData->SetScrollState &= (0xFFFF - SCROLLRIGHT);
//            }
//            if (lParam & MASKMIDDLE)
//            {
//                if (wParam & SCROLLMIDDLE)
//                    pWndData->SetScrollState |= SCROLLMIDDLE;
//                else
//                    pWndData->SetScrollState &= (0xFFFF - SCROLLMIDDLE);
//            }
//        }
//        //pWndData->ScrollMask = lParam;
//        DoNCPaint(hWnd, pWndData, NC_SCROLLSTATE);

        break;

    case PWM_CREATECAPTIONBUTTON :

		//printf("不能在PDADefWindowProc中创建staticBar区域上的Button和菜单\r\n");
        if (HIWORD(wParam) == 0)
        {
			int nStyle;
            HDC hdc = NULL;

			if(lParam==NULL) //by axli to avoid down when implement strlen(lparam)
				break;

            GetTextExtentPoint(NULL, "A", 1, &szChar);
            pWndData->buttons.id = LOWORD(wParam);
            nTextLen = strlen((PCSTR)lParam);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(pWndData->buttons.text, (PCSTR)lParam, nTextLen);
            pWndData->buttons.text[nTextLen] = '\0';
            hdc = GetDC(hWnd);
            GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                pWndData->buttons.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
            ReleaseDC(hWnd, hdc);
            pWndData->buttons.rect.left = pWndData->buttons.rect.right -
		        MENU_TEXT_GAP * 2 - szChar.cx * strlen(pWndData->buttons.text);
			nStyle =  GetWindowLong(hWnd,GWL_STYLE);
			nStyle |= PWS_STATICBAR;
			SetWindowLong(hWnd,GWL_STYLE,nStyle);
            DoNCPaint(hWnd, pWndData, NC_MENUBAR);
        }
        else
        {
			int nStyle;

			if(lParam==NULL) //by axli
				break;

            GetTextExtentPoint(NULL, "A", 1, &szChar);
            pWndData->LeftButton.id = LOWORD(wParam);
            nTextLen = strlen((PCSTR)lParam);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(pWndData->LeftButton.text, (PCSTR)lParam, nTextLen);
            pWndData->LeftButton.text[nTextLen] = '\0';
            hdc = GetDC(hWnd);
            GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                pWndData->LeftButton.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
            ReleaseDC(hWnd, hdc);
            pWndData->LeftButton.rect.right = pWndData->LeftButton.rect.left 
                + MENU_TEXT_GAP* 2 + szChar.cx * strlen(pWndData->LeftButton.text);
			nStyle =  GetWindowLong(hWnd,GWL_STYLE);
			nStyle |= PWS_STATICBAR;
			SetWindowLong(hWnd,GWL_STYLE,nStyle);
            DoNCPaint(hWnd, pWndData, NC_MENUBAR);
        }
        

        break;

    case PWM_SETBUTTONTEXT :

        GetTextExtentPoint(NULL, "A", 1, &szChar);

        if ((LOWORD(wParam) == 0) && (pWndData->buttons.id != NULL))
        {
            if (pWndData->MenuState)
            {
                if (lParam == NULL)
                    pWndData->oldText[0][0] = '\0';
                else
                {
                    nTextLen = strlen((PCSTR)lParam);
                    if (nTextLen > MAX_BUTTON_TEXT - 1)
                        nTextLen = MAX_BUTTON_TEXT - 1;
                    strncpy(pWndData->oldText[0], (PCSTR)lParam, nTextLen);
                    pWndData->oldText[0][nTextLen] = '\0';
                    hdc = GetDC(hWnd);
                    GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                        pWndData->buttons.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
                    ReleaseDC(hWnd, hdc);
                }
            }
            else
            {
                if (lParam == NULL)
                {
                    pWndData->buttons.text[0] = '\0';
                    DoNCPaint(hWnd, pWndData, NC_ALL);//NC_BUTTON
                }
                else
                {
                    nTextLen = strlen((PCSTR)lParam);
                    if (nTextLen > MAX_BUTTON_TEXT - 1)
                        nTextLen = MAX_BUTTON_TEXT - 1;
                    strncpy(pWndData->buttons.text, (PCSTR)lParam, nTextLen);
                    pWndData->buttons.text[nTextLen] = '\0';
                    hdc = GetDC(hWnd);
                    GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                        pWndData->buttons.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
                    ReleaseDC(hWnd, hdc);
                    pWndData->buttons.rect.left = pWndData->buttons.rect.right -  
                        MENU_TEXT_GAP * 2 - szChar.cx * strlen(pWndData->buttons.text);
                    DoNCPaint(hWnd, pWndData, NC_MENUBAR);//NC_BUTTON
                }
            }
        }
        else if ((LOWORD(wParam) == 1) && (pWndData->LeftButton.id != NULL))
        {
            //nTextLen = strlen((PCSTR)pWndData->LeftButton.text) + 1;
            if (pWndData->MenuState)
            {
                if (lParam == NULL)
                    pWndData->oldText[1][0] = '\0';
                else
                {
                    nTextLen = strlen((PCSTR)lParam);
                    if (nTextLen > MAX_BUTTON_TEXT - 1)
                        nTextLen = MAX_BUTTON_TEXT - 1;
                    strncpy(pWndData->oldText[1], (PCSTR)lParam, nTextLen);
                    pWndData->oldText[1][nTextLen] = '\0';
                    hdc = GetDC(hWnd);
                    GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                        pWndData->LeftButton.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
                    ReleaseDC(hWnd, hdc);
                }
            }
            else
            {
                if (lParam == NULL)
                {
                    pWndData->LeftButton.text[0] = '\0';
                    DoNCPaint(hWnd, pWndData, NC_ALL);
                }
                else
                {
                    nTextLen = strlen((PCSTR)lParam);
                    if (nTextLen > MAX_BUTTON_TEXT - 1)
                        nTextLen = MAX_BUTTON_TEXT - 1;
                    strncpy(pWndData->LeftButton.text, (PCSTR)lParam,
                        nTextLen);
                    pWndData->LeftButton.text[nTextLen] = '\0';
                    hdc = GetDC(hWnd);
                    GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                        pWndData->LeftButton.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
                    ReleaseDC(hWnd, hdc);
                    pWndData->LeftButton.rect.right = pWndData->LeftButton.rect.left +
                        MENU_TEXT_GAP* 2 + szChar.cx * strlen(pWndData->LeftButton.text);
                    DoNCPaint(hWnd, pWndData, NC_MENUBAR);
                }
            }
        }
#ifdef SEF_SMARTPHONE
        else if (LOWORD(wParam) == 2)
        {
            nTextLen = strlen((PCSTR)lParam);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(pWndData->MenuText, (PCSTR)lParam, nTextLen);
            pWndData->MenuText[nTextLen] = '\0';
            DoNCPaint(hWnd, pWndData, NC_MENUBAR);
        }
        else if (LOWORD(wParam) == 3)
        {
            nTextLen = strlen((PCSTR)lParam);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(pWndData->UseMenu, (PCSTR)lParam, nTextLen);
            pWndData->MenuText[nTextLen] = '\0';
            DoNCPaint(hWnd, pWndData, NC_MENUBAR);
        }
#endif //SEF_SMARTPHONE
        break;

    case PWM_GETBUTTONTEXT :

        if (LOWORD(wParam) == 0)
        {
            GetText = (char *)lParam;
            GetTextExtentPoint(NULL, "A", 1, &szChar);
            nTextLen = strlen((PCSTR)pWndData->buttons.text);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(GetText, pWndData->buttons.text, nTextLen);
            GetText[nTextLen] = '\0';
        }
        else if (LOWORD(wParam) == 1)
        {
            GetText = (char *)lParam;
            nTextLen = strlen((PCSTR)pWndData->LeftButton.text);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(GetText, pWndData->LeftButton.text, nTextLen);
            GetText[nTextLen] = '\0';
        }
        else if (LOWORD(wParam) == 2)
        {
            GetText = (char*)lParam;
            strcpy(GetText, (const char*)pWndData->MenuText);
        }
        break;

	case PWM_SETDEFWINDOWCOLOR :
		switch(wParam)
        {
        case CAP_COLOR:
            CapColor = (COLORREF)lParam;
            break;
        case BAR_COLOR:
            BarColor = (COLORREF)lParam;
            break;
        case BTN_COLOR:
            BtnColor = (COLORREF)lParam;
            break;
        case FOCUS_COLOR:
            FocusColor = (COLORREF)lParam;
            break;
        }
        break;

    case PWM_SETSIGNAL:
        if (HIWORD(wParam) == SIGNALICON && lParam)
		{
            strncpy(Win_SigIcon, (char *)lParam, 127);
            Win_SigIcon[127] = '\0';
			if (pWndData->bSignal)
				DoNCPaint(hWnd, pWndData, NC_WINDOWTEXT);
		}
        if (HIWORD(wParam) == POWERICON && lParam)
		{
            strncpy(Win_PowIcon, (char *)lParam, 127);
            Win_PowIcon[127] = '\0';
			if (pWndData->bPower)
				DoNCPaint(hWnd, pWndData, NC_WINDOWTEXT);
		}
        break;

#ifdef SEF_SMARTPHONE
    case PWM_SETAPPICON:
        if (LOWORD(wParam) == IMAGE_ICON)
        {
            if (HIWORD(wParam) == LEFTICON)
            {
                if (lParam)
                {
                    strncpy(pWndData->IconName, (char *)lParam, 127);
                    pWndData->IconName[127] = '\0';
                }
                pWndData->bSignal = FALSE;
                
                pWndData->hBitmap = NULL;
            }
            else if (HIWORD(wParam) == CALLICON)
            {
                if (lParam)
                {
                    strncpy(Win_PhoneIcon, (char *)lParam, 127);
                    Win_PhoneIcon[127] = '\0';
                }
            }
            else if (HIWORD(wParam) == ENDCALLICON)
            {
                Win_PhoneIcon[0] = '\0';
            }
            else
            {
                if (lParam)
                {
                    strncpy(pWndData->IconName2, (char *)lParam, 127);
                    pWndData->IconName2[127] = '\0';
                }
                pWndData->bPower = FALSE;
                
                pWndData->hBitmap2 = NULL;
            }
        }
        else
        {
            if (HIWORD(wParam) == SIGNALICON)
            {
                pWndData->bSignal = TRUE;
                pWndData->hBitmap = NULL;
            }
            else if (HIWORD(wParam) == POWERICON)
            {
                pWndData->bPower = TRUE;
                pWndData->hBitmap2 = NULL;
            }
            else if (HIWORD(wParam) == LEFTICON)
            {
                pWndData->hBitmap = (HBITMAP)lParam;
                pWndData->IconName[0] = '\0'; 
            }
            else
            {
                pWndData->hBitmap2 = (HBITMAP)lParam;
                pWndData->IconName2[0] = '\0';
            }
        }
        DoNCPaint(hWnd, pWndData, NC_WINDOWTEXT);
        break;
#endif

    case WM_CLOSE :

        // Default WM_CLOSE handling is to destroy the window.
        DestroyWindow(hWnd);

        break;


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
        if (wParam == 1)
        {
            if (pszText)
            {                
                if (strlen(pszText) >= 127)
                {
                    pWndData->szCapBmpName[0] = '\0';
                    lResult = FALSE;
                }
                else
                {
                    strcpy(pWndData->szCapBmpName, pszText);
                    pWndData->szCapBmpName[strlen(pszText)] = '\0';
                    DoNCPaint(hWnd, pWndData, NC_WINDOWTEXT);
                    lResult = TRUE;
                }                
            }
            else
            {
                pWndData->szCapBmpName[0] = '\0';
                DoNCPaint(hWnd, pWndData, NC_WINDOWTEXT);
                lResult = TRUE;
            }
            break;
        }

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

            if (pWndData->szCapBmpName[0] == '\0')
                DoNCPaint(hWnd, pWndData, NC_WINDOWTEXT);
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

    case WM_KEYDOWN :

        switch (wParam)
        {
#ifndef SEF_SMARTPHONE
        case VK_F2 :

            PostMessage(GetTopWindow(hWnd), WM_CLOSE, 0, 0);
            break;
#endif //SEF_SMARTPHONE
        case VK_F10:
            dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
            if((dwStyle & PWS_STATICBAR) && (pWndData->buttons.text[0] != '\0'))
                SendMessage(hWnd, WM_COMMAND, pWndData->buttons.id, NULL);
            break;

        case VK_RETURN :
            dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
            if((dwStyle & PWS_STATICBAR) && (pWndData->LeftButton.text[0] != '\0'))
                SendMessage(hWnd, WM_COMMAND, pWndData->LeftButton.id, NULL);
            break;

        case VK_F5:
        {
            HMENU hMenu;
            RECT rcMenu, rcInvert, rcWindow;
//			HWND  hwndFocus;
            

            dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
            hMenu = (HMENU)GetWindowLong(hWnd, GWL_ID);
            if (hMenu && (dwStyle & PWS_STATICBAR))
            {
                CalcCaptionRect(hWnd, pWndData, NULL, NULL, &rcMenu, NULL, NULL);
                
                CopyRect(&rcInvert, &rcMenu);
                
                rcInvert.left += MENU_LEFT_GAP;
                rcInvert.top += 1;
                rcInvert.right -= MENU_RIGHT_GAP;
                rcInvert.bottom -= 1;
                
                InflateRect(&rcInvert, -1, -1);
                
                hdc = GetWindowDC(hWnd);
                //InvertRect(hdc, &rcInvert);
                pWndData->MenuState = 1;
                ReleaseDC(hWnd, hdc);
                
                // Converts to screen coordinate
                GetWindowRect(hWnd, &rcWindow);
                OffsetRect(&rcMenu, rcWindow.left, rcWindow.top);
                
                scdata = SendMessage(hWnd, PWM_SETSCROLLSTATE, 
                    (WPARAM)SCROLLUP | SCROLLDOWN, (LPARAM)MASKALL);                
                strcpy((char*)pWndData->oldText[0], (const char*)pWndData->buttons.szDisplay);
                strcpy((char*)pWndData->oldText[1], (const char*)pWndData->LeftButton.szDisplay);
                strcpy((char*)pWndData->oldText[2], (const char*)pWndData->MenuText);
                //strcpy(pWndData->buttons.text, (PCSTR)ML("Back"));
                strcpy(pWndData->buttons.szDisplay, (PCSTR)ML("Back"));
                //strcpy(pWndData->LeftButton.text, "");
                strcpy(pWndData->LeftButton.szDisplay, (PCSTR)"");
                strcpy(pWndData->MenuText, "Select");
                DoNCPaint(hWnd, pWndData, NC_ALL);

//				hwndFocus = SetFocus(hWnd);
                keybd_event(VK_DOWN, 0, 0, 0);
                keybd_event(VK_DOWN, 0, KEYEVENTF_KEYUP, 0);
                TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN, 
                    rcMenu.left + 1, rcMenu.top - 1, 0, hWnd, NULL);
//                if (IsWindowVisible(hwndFocus) && (hWnd == GetFocus()))
//                {
//                    SetFocus(hwndFocus);
//                }
                //SetMenuDefaultItem(hMenu, 0, FALSE);
                SendMessage(hWnd, PWM_SETSCROLLSTATE, (WPARAM)scdata, (LPARAM)MASKALL);
                strcpy(pWndData->buttons.text, (const char*)pWndData->oldText[0]);
                strcpy(pWndData->buttons.szDisplay, (const char*)pWndData->oldText[0]);
                strcpy(pWndData->LeftButton.text, (const char*)pWndData->oldText[1]);
                strcpy(pWndData->LeftButton.szDisplay, (const char*)pWndData->oldText[1]);
                strcpy(pWndData->MenuText, (const char*)pWndData->oldText[2]);

                hdc = GetWindowDC(hWnd);
                //InvertRect(hdc, &rcInvert);
                pWndData->MenuState = 0;
                DoNCPaint(hWnd, pWndData, NC_ALL);
                ReleaseDC(hWnd, hdc);
                
                // 清空输入队列防止菜单重发的WM_LBUTTONDOWN消息引起MENU再次
                // 弹出
                FlushInputMessages();
            }
//            else if((dwStyle & PWS_STATICBAR) && (pWndData->LeftButton.text[0] != '\0'))
//                SendMessage(hWnd, WM_COMMAND, pWndData->LeftButton.id, NULL);
            break;
        }

        default :

            break;
        }
        break;
#if (!NOCURSOR)

    case WM_SETCURSOR :

        lResult = (LRESULT)DoSetCursor(hWnd, wParam, lParam);
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
        // Foreground = black
        // brush = COLOR_SCROLLBAR.
        
        SetBkColor((HDC)wParam, COLOR_WHITE);
        SetTextColor((HDC)wParam, COLOR_BLACK);
        
        return (LRESULT)hbrScrollBar;
        
    case WM_CTLCOLORBTN :

        // Background = white
        // Foreground = black
        // brush = COLOR_SCROLLBAR.
        
        SetBkColor((HDC)wParam, COLOR_TRANSBK);//GetSysColor(COLOR_BTNFACE)
        SetTextColor((HDC)wParam, COLOR_BLACK);
        
        return (LRESULT)hFillBrush;//(COLOR_BTNFACE + 1);

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

        return (LRESULT)hFillBrush;//(COLOR_WINDOW + 1);

    case WM_SETLBTNTEXT:
        if (lParam != 0)
        {
            if (GetWindowLong(hWnd, GWL_STYLE) & PWS_STATICBAR)
            {
                SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, lParam);
            }
            else
            {
                SendMessage(GetParent(hWnd), PWM_SETBUTTONTEXT, 1, lParam);
            }
        }
        break;

    case WM_SETRBTNTEXT:
        if (GetWindowLong(hWnd, GWL_STYLE) & PWS_STATICBAR)
        {
            SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, lParam);
        }
        else
        {
            SendMessage(GetParent(hWnd), PWM_SETBUTTONTEXT, 0, lParam);
        }
        break;

    case WM_IMESWITCHED:
        if (GetWindowLong(hWnd, GWL_STYLE) & WS_CAPTION)
        {
            SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(wParam, RIGHTICON),
                lParam);
        }
        else
        {
            SendMessage(GetParent(hWnd), message, wParam, lParam);
        }
        break;

#if (!NOKEYBOARD)

    case WM_SYSKEYDOWN :

        // WM_SYSKEYDOWN消息由ALT键按下引起并且不是ALT键本身按下
        if ((lParam & 0x20000000) && wParam != VK_ALT)
        {
            // ALT + F4 close the top level window
            if (wParam == VK_F4)
                PostMessage(GetTopWindow(hWnd), WM_CLOSE, 0, 0);
        }

        break;

    case WM_SYSKEYUP :

        break;

#endif // NOKEYBOARD

    case WM_GETDLGCODE :

        return 0;
    }

    return lResult;
}

#if (!NOMENUS)

/*
**  Funciton : PDAGetMenu
**  Purpose  :
**      The GetMenu function retrieves the handle of the menu assigned
**      to the given window.  
**  Params   :
**      hWnd    : Identifies the window whose menu handle is retrieved. 
**  Return   :
**      If the function succeeds, the return value is the handle of the
**      menu. If the given window has no menu, the return value is NULL.
**      If the window is a child window, the return value is undefined. 
*/
HMENU WINAPI PDAGetMenu(HWND hWnd)
{
    if (GetParent(hWnd))
    {
        SetLastError(1);
        return FALSE;
    }

    return (HMENU)GetWindowLong(hWnd, GWL_ID);
}

/*
**  Funciton : PDASetMenu
**  Purpose  :
**      The SetMenu function assigns a new menu to the specified window.  
**  Params   :
**      hWnd    : Identifies the window to which the menu is to be 
**                assigned.
**      hMenu   : Identifies the new menu. If this parameter is NULL, the 
**                window’s current menu is removed. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended 
**      error information, call GetLastError.
*/
BOOL WINAPI PDASetMenu(HWND hWnd, HMENU hMenu)
{
    if (GetParent(hWnd))
    {
        SetLastError(1);
        return FALSE;
    }

    SetWindowLong(hWnd, GWL_ID, (LONG)hMenu);
    SendMessage(hWnd, WM_NCPAINT, NULL, NULL);

    return TRUE;
}

/*
**  Funciton : PDADrawMenuBar
**  Purpose  :
**      The DrawMenuBar function redraws the menu bar of the specified
**      window.
**      If the menu bar changes after Windows has created the window, 
**      this function must be called to draw the changed menu bar.  
**  Params   :
**      hWnd    : Identifies the window whose menu bar needs redrawing.
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. To get extended
**      error information, call GetLastError
*/
BOOL WINAPI PDADrawMenuBar(HWND hWnd)
{
    return TRUE;
}

#endif // NOMENUS

#ifndef NOSCROLL

/*
**  Function : PDAGetScrollInfo
**  Purpose  :
**      Retrieves the parameters of a scroll bar, including the minimum 
**      and maximum scrolling positions, the page size, and the position
**      of the scroll box (thumb). 
**  Params   :
**      hWnd        : Identifies a scroll bar control or a window with a
**                    standard scroll bar, depending on the value of the
**                    nBar parameter. 
**      nBar        : Specifies the type of scroll bar for which to 
**                    retrieve parameters.
**      pScrollInfo : Points to a SCROLLINFO structure whose fMask member, 
**                    upon entry to the function, specifies the scroll bar
**                    parameters to retrieve. Before returning, the 
**                    function copies the specified parameters to the 
**                    appropriate members of the structure. 
**  Return  :
**      If the function retrieved any values, returnis nonzero. 
**      If the function does not retrieve any values, return zero. 
**  Remark  :
**      The GetScrollInfo function enables applications to use 32-bit 
**      scroll positions. Although the messages that indicate scroll-bar
**      position, WM_HSCROLL and WM_VSCROLL, provide only 16 bits of 
**      position data, the functions SetScrollInfo and GetScrollInfo 
**      provide 32 bits of scroll-bar position data. Thus, an application
**      can call GetScrollInfo while processing either the WM_HSCROLL or 
**      WM_VSCROLL messages to obtain 32-bit scroll-bar position data. 
*/
BOOL WINAPI PDAGetScrollInfo(HWND hWnd, int nBar, PSCROLLINFO pScrollInfo)
{
    // If the parameter is invalid, return FALSE.
    if ((nBar != SB_CTL && nBar != SB_HORZ && nBar != SB_VERT) || 
        !pScrollInfo)
    {
        SetLastError(1);
        return FALSE;
    }

    if (nBar != SB_CTL)
        return WINPDA_GetScrollInfo(hWnd, nBar, pScrollInfo);

    return SendMessage(hWnd, SBM_GETSCROLLINFO, (WPARAM)0, 
        (LPARAM)pScrollInfo);
}

BOOL WINAPI PDASetScrollInfo(HWND hWnd, int nBar, 
                                PCSCROLLINFO pScrollInfo, BOOL bRedraw)
{
    // If the parameter is invalid, return FALSE.
    if ((nBar != SB_CTL && nBar != SB_HORZ && nBar != SB_VERT) || 
        !pScrollInfo)
    {
        SetLastError(1);
        return FALSE;
    }

    if (nBar != SB_CTL)
        return WINPDA_SetScrollInfo(hWnd, nBar, pScrollInfo, bRedraw);

    return SendMessage(hWnd, SBM_SETSCROLLINFO, (WPARAM)bRedraw, 
        (LPARAM)pScrollInfo);
}


/*
**  Function : PDAEnableScrollBar
**  Purpose  :
**      Enables or disables one or both scroll bar arrows. 
**  Params   :
**      hWnd     : Identifies a window or a scroll bar control, depending 
**                 on the value of the wSBflags parameter. 
**      nSBFlags : Specifies the scroll bar type. This parameter can be 
**                 SB_BOTH, SB_CTL, SB_HORZ or SB_VERT. 
**      nArrow   : Specifies whether the scroll bar arrows are enabled or 
**                 disabled and indicates which arrows are enabled or 
**                 disabled. 
**  Return   :
**      If the arrows are enabled or disabled as specified, return nonzero. 
**      If the arrows are already in the requested state or an error 
**      occurs, return zero. 
*/
BOOL WINAPI PDAEnableScrollBar(HWND hWnd, int nBar, int nArrow)
{
    // If the parameter is invalid, return FALSE.
    if (nBar != SB_CTL && nBar != SB_HORZ && nBar != SB_VERT && 
        nBar != SB_BOTH)
    {
        SetLastError(1);
        return FALSE;
    }

    if (nBar != SB_CTL)
        return WINPDA_EnableScrollBar(hWnd, nBar, nArrow);

    return SendMessage(hWnd, SBM_ENABLE_ARROWS, (WPARAM)nArrow, 0);
}

#endif  /* NOSCROLL */

/*************************************************************************/
/*                      模块内部静态函数实现                             */
/*************************************************************************/

/*
**  Function : WINPDA_Init
**  Purpose  :
**      窗口系统PDA定制模块初始化函数。
*/
static BOOL WINPDA_Init(void)
{
    if (bInit)
        return TRUE;

    /*hbmpDown = CreateBitmap(BMP_WIDTH, BMP_HEIGHT, 1, 1, DOWNDATA);
    hbmpUp = CreateBitmap(BMP_WIDTH, BMP_HEIGHT, 1, 1, UPDATA);
    hbmpLeft = CreateBitmap(BMP_HEIGHT, BMP_WIDTH, 1, 1, LEFTDATA);
    hbmpRight = CreateBitmap(BMP_HEIGHT, BMP_WIDTH, 1, 1, RIGHTDATA);*/
    /*hbmpDown = LoadImage(NULL, "ROM:down.bmp", IMAGE_BITMAP,
        0, 0, LR_LOADFROMFILE);
    hbmpUp = LoadImage(NULL, "ROM:up.bmp", IMAGE_BITMAP,
        0, 0, LR_LOADFROMFILE);
    hbmpLeft = LoadImage(NULL, "ROM:left.bmp", IMAGE_BITMAP,
        0, 0, LR_LOADFROMFILE);
    hbmpRight = LoadImage(NULL, "ROM:right.bmp", IMAGE_BITMAP,
        0, 0, LR_LOADFROMFILE);*/

    // Creates background brush for scroll bar
    hbrScrollBar = CreateHatchBrush(HS_SIEVE, 
        GetSysColor(COLOR_SCROLLBAR));

    bInit = TRUE;

    return TRUE;
}

/*
**  Function : WINPDA_Exit
**  Purpose  :
**      窗口系统PDA定制模块退出函数。
*/
static BOOL WINPDA_Exit(void)
{
    if (bInit)
    {
        /*DeleteObject(hbmpUp);
        DeleteObject(hbmpDown);
        DeleteObject(hbmpLeft);
        DeleteObject(hbmpRight);*/
        
        DeleteObject(hbrScrollBar);

        bInit = FALSE;
    }

    return TRUE;
}

/*
**  Function : WINPDA_SetScrollInfo
**  Purpose  :
**      Set the scrollbar flag to PWINDATA
*/
int WINPDA_SetScrollInfo(HWND hWnd, int nBar, PCSCROLLINFO pScrollInfo,
                         BOOL bRedraw)
{
    PWNDDATA pWndData;
    RECT     rcClient, rect, rcWindow;
    HDC      hdc;
    int      nRet = 0;
    BOOL     bMove = FALSE;
    
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);
    ASSERT(pScrollInfo != NULL);

    /*pUserData = GetUserData(hWnd);
    if (!pUserData)
        return FALSE;*/

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

BOOL WINPDA_GetScrollInfo(HWND hWnd, int nBar, PSCROLLINFO pScrollInfo)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
    BOOL        bRet = FALSE;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);
    ASSERT(pScrollInfo != NULL);

//    pUserData = GetUserData(hWnd);
//    if (!pUserData)
//        return FALSE;

    // Gets the internal data for default window
//    pWndData = *(PWNDDATA*)(pUserData - 4);
//    if (!pWndData)
//        return FALSE;

    pWndData = GetWndData(hWnd);
    if (!pWndData)
        return FALSE;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (nBar == SB_VERT && (dwStyle & WS_VSCROLL))
        bRet = SB_GetScrollInfo(pWndData->pVScrollData, 
            (LPARAM)pScrollInfo);
    else if (nBar == SB_HORZ && (dwStyle & WS_HSCROLL))
        bRet = SB_GetScrollInfo(pWndData->pHScrollData, 
            (LPARAM)pScrollInfo);

    return bRet;
}

BOOL WINPDA_EnableScrollBar(HWND hWnd, int nBar, int nArrow)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
    RECT        rcClient, rect;
    HDC         hdc;
    BOOL        bRet = FALSE;
 
    ASSERT(nBar == SB_BOTH || nBar == SB_HORZ || nBar == SB_VERT);

//    pUserData = GetUserData(hWnd);
//    if (!pUserData)
//        return FALSE;
//
//    // Gets the internal data for default window
//    pWndData = *(PWNDDATA*)(pUserData - 4);
//    if (!pWndData)
//        return FALSE;
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

int WINPDA_SetScrollPos(HWND hWnd, int nBar, int nPos, BOOL bRedraw)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
    RECT        rcClient, rect;
    HDC         hdc;
    int         nRet = 0;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);

//    pUserData = GetUserData(hWnd);
//    if (!pUserData)
//        return FALSE;
//
//    // Gets the internal data for default window
//    pWndData = *(PWNDDATA*)(pUserData - 4);
//    if (!pWndData)
//        return FALSE;
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

int  WINPDA_GetScrollPos(HWND hWnd, int nBar)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
    int         nRet = 0;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);

//    pUserData = GetUserData(hWnd);
//    if (!pUserData)
//        return FALSE;
//
//    // Gets the internal data for default window
//    pWndData = *(PWNDDATA*)(pUserData - 4);
//    if (!pWndData)
//        return FALSE;
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

BOOL WINPDA_SetScrollRange(HWND hWnd, int nBar, int nMinPos, int nMaxPos, 
                           BOOL bRedraw)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
    RECT        rcClient, rect;
    HDC         hdc;
    int         nRet = 0;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);

//    pUserData = GetUserData(hWnd);
//    if (!pUserData)
//        return FALSE;
//
//    // Gets the internal data for default window
//    pWndData = *(PWNDDATA*)(pUserData - 4);
//    if (!pWndData)
//        return FALSE;
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

BOOL WINPDA_GetScrollRange(HWND hWnd, int nBar, LPINT lpMinPos, 
                           LPINT lpMaxPos)
{
    PWNDDATA    pWndData;
    DWORD       dwStyle;
 
    ASSERT(nBar == SB_HORZ || nBar == SB_VERT);

//    pUserData = GetUserData(hWnd);
//    if (!pUserData)
//        return FALSE;
//
//    // Gets the internal data for default window
//    pWndData = *(PWNDDATA*)(pUserData - 4);
//    if (!pWndData)
//        return FALSE;
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
 

#if (!NOCURSOR)

/*
**  Function : DoSetCursor
**  Purpose  :
**      First forward WM_SETCURSOR message to parent. If it returns TRUE
**      (i.e., it set the cursor), just return. Otherwise, set the cursor
**      based on codeHT and msg.
*/
static BOOL DoSetCursor(HWND hWnd, WPARAM wParam, LPARAM lParam)
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
        MessageBeep(0xFFFFFFFF);
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
    HBRUSH  hbrDot, hbrOld;
    long    dwStyle;
    COLORREF crOldBk;
    int     nOldBkMode;

    ASSERT(hWnd != NULL);

    hwndParent = GetParent(hWnd);
    if (!hwndParent)    // parent window is null
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

    hbrDot = CreateBrush(BS_HATCHED, COLOR_BLACK, HS_SIEVE);
    hbrOld = SelectObject(hdc, hbrDot);

    crOldBk = SetBkColor(hdc, COLOR_WHITE);
    nOldBkMode = SetBkMode(hdc, OPAQUE);

    // delete the attribute WS_CLIPCHILDREN of the parent window
    dwStyle = GetWindowLong(hwndParent, GWL_STYLE);
    SetWindowLong(hwndParent, GWL_STYLE, dwStyle & ~WS_CLIPCHILDREN);

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
    
    // restore the attribute WS_CLIPCHILDREN of the parent window.
    SetWindowLong(hwndParent, GWL_STYLE, dwStyle);

    SelectObject(hdc, hbrOld);
    DeleteObject(hbrDot);

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
    int nTopCaptionH, nBottomCaptionH;
    RECT rcClient;
    PRECT prcWindow;
    RECT rcTopCaption, rcBottomCaption;
	SIZE szChar;

    fCalcValidRects = (BOOL)wParam;
    if (fCalcValidRects)
        pncsp = (PNCCALCSIZE_PARAMS)lParam;
    
    // 获得新的窗口矩形(父窗口坐标)
    prcWindow = (PRECT)lParam;
    
    // 计算边框的大小
    dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
    nBorderW = GetBorderWidth(dwStyle);
    nBorderH = GetBorderWidth(dwStyle);
    
    if (dwStyle & WS_CAPTION)
        nTopCaptionH = GetSystemMetrics(SM_CYCAPTION) - CY_BOUNDARY;
    else
        nTopCaptionH = 0;

    if (dwStyle & PWS_STATICBAR)
        nBottomCaptionH = GetSystemMetrics(SM_CYMENU) + CY_BOUNDARY;
    else 
        nBottomCaptionH = 0;
    
    // 计算新的客户区矩形
    
    rcClient.left = prcWindow->left + nBorderW;
    rcClient.top = prcWindow->top + nBorderH + nTopCaptionH;
    
    if (rcClient.left >= prcWindow->right)
        rcClient.left = prcWindow->right - 1;
    if (rcClient.top >= prcWindow->bottom)
        rcClient.top = prcWindow->bottom - 1;
    
    rcClient.right = prcWindow->right - nBorderW;
    rcClient.bottom = prcWindow->bottom - nBorderH - nBottomCaptionH;

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

	SetRect(&rcBottomCaption, prcWindow->left + nBorderW, 
        prcWindow->bottom - nBorderH - GetSystemMetrics(SM_CYMENU), 
        prcWindow->right - nBorderW, prcWindow->bottom - nBorderH);
    OffsetRect(&rcBottomCaption, -prcWindow->left, -prcWindow->top);

    SetRect(&rcTopCaption, prcWindow->left + nBorderW, 
        prcWindow->top + nBorderH, prcWindow->right - nBorderW, 
        prcWindow->top + nBorderH + GetSystemMetrics(SM_CYCAPTION) - 2);
    OffsetRect(&rcTopCaption, -prcWindow->left, -prcWindow->top);

	GetTextExtentPoint(NULL, "A", 1, &szChar);
    pWndData->buttons.rect.top = rcBottomCaption.top + 1 ;
    pWndData->buttons.rect.bottom = rcBottomCaption.bottom;
    pWndData->buttons.rect.right = rcBottomCaption.right - MENU_LEFT_GAP;	
	pWndData->buttons.rect.left = rcBottomCaption.right -  MENU_LEFT_GAP - 
		MENU_TEXT_GAP * 2;
	if (pWndData->buttons.text[0] != '\0')
		pWndData->buttons.rect.left = pWndData->buttons.rect.right - MENU_LEFT_GAP- 
		MENU_TEXT_GAP * 2 - szChar.cx * strlen(pWndData->buttons.text);

    pWndData->LeftButton.rect.top = rcBottomCaption.top + 1;
    pWndData->LeftButton.rect.bottom = rcBottomCaption.bottom;
    pWndData->LeftButton.rect.left = rcBottomCaption.left + MENU_LEFT_GAP;
    pWndData->LeftButton.rect.right = rcBottomCaption.left + MENU_LEFT_GAP
        + MENU_TEXT_GAP* 2;
	if (pWndData->LeftButton.text[0] != '\0')
		pWndData->LeftButton.rect.right = pWndData->LeftButton.rect.left
        + MENU_LEFT_GAP + MENU_TEXT_GAP* 2 +
        szChar.cx * strlen(pWndData->LeftButton.text);
    

    // 将新的客户区矩形返回
    *prcWindow = rcClient;
}

/*
**  Function : DoNCPaint
**  Purpose  :
*/
static void DoNCPaint(HWND hWnd, PWNDDATA pWndData, int nPart)
{
    #define MAX_TEXT 128        // Buffer size for window text
    char achTextBuf[MAX_TEXT];  // Buffer for window text
    char szDisplay[MAX_TEXT] = "";

    DWORD    dwStyle;
    RECT     rect, rcWindow, rcClient;
    int      nBorderW, nBorderH;
    int      nTextLen, nBkMode,nMemBkMode;
    HDC      hdc;
    HMENU    hMenu;
    RECT    rcTopCaption, rcMenu, rcText, rcButton, rcBottomCaption, rcLeftButton;
//    RECT    rcClearMenu;
    HBRUSH  hBrush;
	PCOLORSCHEME pColorScheme;
//	HBITMAP	hBmpCap, hBmpBar;
    BITMAP bmp;
	HDC hMemDC;
	RECT rc;
    COLORREF crBkOld = 0;

    if (!pWndData || nPart == 0)
        return;

	pColorScheme = GetColorScheme();

    hdc = GetWindowDC(hWnd);

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    nBorderW = GetBorderWidth(dwStyle);
    nBorderH = GetBorderWidth(dwStyle);

    GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_WINDOW);
    GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_WINDOW);

    InflateRect(&rcWindow, -nBorderW, -nBorderH);
    
    // Paints the caption if nesseary

    CalcCaptionRect(hWnd, pWndData, &rcBottomCaption, &rcTopCaption, &rcMenu,
        &rcText, &rcButton);    
    
    nBkMode = SetBkMode(hdc, BM_TRANSPARENT);

    if ((nPart & NC_WINDOWTEXT) && (dwStyle & WS_CAPTION))
    {
        ASSERT(!IsRectEmpty(&rcText));
        
		hMemDC = CreateMemoryDC(rcTopCaption.right-rcTopCaption.left, 
			rcTopCaption.bottom-rcTopCaption.top);
        // Gets the window text by calling GetWindowTextLength and 
        // GetWindowText instead of using pWndData->pszText directly, 
        // because the window proc maybe process WM_SETTEXT itself. 
        //ClearRect(hdc, &rcTopCaption, RGB(247, 243, 0));
        
#ifdef SEF_SMARTPHONE
        
        SetRect(&rc, 0, 0, rcTopCaption.right - rcTopCaption.left, 
			rcTopCaption.bottom-rcTopCaption.top);
		FillRect(hMemDC, &rc, hFillBrush);

        if (pWndData->MenuState == 0)
        {
            if (Win_PhoneIcon[0] != '\0' && !(dwStyle & PWS_NOTSHOWPI))
            {
                DrawIconFromFile(hMemDC, Win_PhoneIcon, 2, 
                    2, APPICONWIDTH, APPICONHEIGHT);
            }
            else if (pWndData->bSignal)
            {
                DrawIconFromFile(hMemDC, Win_SigIcon, 2, 
                    2, APPICONWIDTH, APPICONHEIGHT);
            }
            else if (pWndData->IconName[0] != '\0')
            {
                DrawIconFromFile(hMemDC, pWndData->IconName, 2, 
                    2, APPICONWIDTH, APPICONHEIGHT);
            }
            else if( pWndData->hBitmap != NULL)
            {
                GetObject(pWndData->hBitmap, sizeof(BITMAP), (void*)&bmp);
                nBkMode = SetBkMode(hMemDC, NEWTRANSPARENT);
                crBkOld = SetBkColor(hMemDC, COLOR_TRANSBK);
                StretchBlt(hMemDC, 2, 2,
                    bmp.bmWidth, APPICONHEIGHT, (HDC)pWndData->hBitmap,
                    0, 0, bmp.bmWidth, bmp.bmHeight, SRCCOPY);
                SetBkColor(hMemDC, crBkOld);
                SetBkMode(hMemDC, nBkMode);
            }
            
            if (pWndData->bPower)
            {
                DrawIconFromFile(hMemDC, Win_PowIcon,
                    rcTopCaption.right - 2 - APPICONWIDTH, 
                    2, APPICONWIDTH, APPICONHEIGHT);
            }
            else if (pWndData->IconName2[0] != '\0')
            {
                DrawIconFromFile(hMemDC, pWndData->IconName2,
                    rcTopCaption.right - 2 - APPICONWIDTH, 
                    2, APPICONWIDTH, APPICONHEIGHT);
            }
            else if(pWndData->hBitmap2 != NULL)
            {
                GetObject(pWndData->hBitmap2, sizeof(BITMAP), (void*)&bmp);
                nBkMode = SetBkMode(hMemDC, NEWTRANSPARENT);
                crBkOld = SetBkColor(hMemDC, COLOR_TRANSBK);
                StretchBlt(hMemDC, rcTopCaption.right - 2 - bmp.bmWidth,
                    2, bmp.bmWidth, APPICONHEIGHT,
                    (HDC)pWndData->hBitmap2, 0, 0, bmp.bmWidth, bmp.bmHeight,
                    SRCCOPY);
                SetBkColor(hMemDC, crBkOld);
                SetBkMode(hMemDC, nBkMode);
            }
        }

#endif //SEF_SMARTPHONE

        if (pWndData->szCapBmpName[0] == '\0')
        {
            nTextLen = GetWindowTextLength(hWnd);
            if (nTextLen > MAX_TEXT - 1)
                nTextLen = MAX_TEXT - 1;
            GetWindowText(hWnd, achTextBuf, MAX_TEXT);

            if ((pWndData->hBitmap != NULL) || (pWndData->hBitmap2 != NULL)
                || (pWndData->IconName[0] != '\0') || (pWndData->IconName2[0] != '\0'))
            {
                rc.left += APPICONWIDTH + 2;
                rc.right -= APPICONWIDTH + 2;
            }

            GetExtentFittedText(hMemDC, achTextBuf, -1, szDisplay, MAX_TEXT,
                rc.right - rc.left, '.', 3);
            nMemBkMode = SetBkMode(hMemDC, BM_TRANSPARENT);
#ifdef SEF_SMARTPHONE
            DrawText(hMemDC, szDisplay, -1, &rc, 
                DT_HCENTER | DT_VCENTER);
#else //SEF_SMARTPHONE
            DrawText(hMemDC, achTextBuf, nTextLen, &rc, 
                DT_HCENTER | DT_VCENTER);
#endif //SEF_SMARTPHONE
            SetBkMode(hMemDC, nMemBkMode);
        }
        else
        {
            nMemBkMode = SetBkMode(hMemDC, BM_NEWALPHATRANSPARENT);
            crBkOld = SetBkColor(hMemDC, COLOR_WHITE);
            rc.left += APPICONWIDTH + 2;
            DrawBitmapFromFile(hMemDC, rc.top, rc.left, pWndData->szCapBmpName,
                SRCCOPY);
            SetBkColor(hMemDC, crBkOld);
            SetBkMode(hMemDC, nMemBkMode);
        }
		
		BitBlt(hdc, rcTopCaption.left, rcTopCaption.top, rcTopCaption.right - rcTopCaption.left,
			rcTopCaption.bottom - rcTopCaption.top, hMemDC, 0, 0, SRCCOPY);
		DeleteDC(hMemDC);
		
        // Only redraw window text, return
        if (nPart == NC_WINDOWTEXT)
        {
            ReleaseDC(hWnd, hdc);
            return;
        }
    }
    //SetBkColor(hdc, BarColor);
    
    if (dwStyle & PWS_STATICBAR)
    {
		hMemDC = CreateMemoryDC(rcBottomCaption.right-rcBottomCaption.left, 
			rcBottomCaption.bottom-rcBottomCaption.top);
		nMemBkMode = SetBkMode(hMemDC, BM_TRANSPARENT);
		SetRect(&rc, 0, 0, rcBottomCaption.right - rcBottomCaption.left, 
			rcBottomCaption.bottom-rcBottomCaption.top);
        //if (nPart & NC_MENUBAR)
        FillRect(hMemDC, &rc, hFillBrush);

        ASSERT(!IsRectEmpty(&rcMenu));
		
		SetRect(&rcLeftButton, rcMenu.left, 0, rcMenu.right - rcMenu.left,
			rcMenu.bottom - rcMenu.top);
		hMenu = (HMENU)GetWindowLong(hWnd, GWL_ID);

        rcMenu.left += (PLX_WIN_WIDTH - CX_MENUICON) >> 1;
		rcMenu.top = 4;
		rcMenu.right += (PLX_WIN_WIDTH + CX_MENUICON) >> 1;

        if (stricmp(pWndData->MenuText, "Select") == 0)
		{
			DrawIconFromFile(hMemDC, (LPCSTR)pColorScheme->BarMenuUpBmpName,
				rcMenu.left, rcMenu.top, rcMenu.right - rcMenu.left,
				rcMenu.bottom - rcMenu.top);
		}
		else if (stricmp(pWndData->MenuText, "Options") == 0)
		{
			DrawIconFromFile(hMemDC, (LPCSTR)pColorScheme->BarMenuDownBmpName,
				rcMenu.left, rcMenu.top, rcMenu.right - rcMenu.left,
				rcMenu.bottom - rcMenu.top);
		}
		else if (stricmp(pWndData->MenuText, "Home") == 0)
		{
			DrawIconFromFile(hMemDC, (LPCSTR)pColorScheme->BarBtnDownBmpName,
				rcMenu.left, rcMenu.top, rcMenu.right - rcMenu.left,
				rcMenu.bottom - rcMenu.top);
		}

        InflateRect(&rcMenu, -1, 0);
		rcMenu.top += 1;

        if ((pWndData->LeftButton.text[0] != '\0')
            || (pWndData->LeftButton.szDisplay[0] != '\0'))
		{
			rcLeftButton.left += MENU_LEFT_GAP;
			rcLeftButton.right -= MENU_RIGHT_GAP;
			
			DrawText(hMemDC, pWndData->LeftButton.szDisplay, -1, &rcLeftButton, 
				DT_LEFT | DT_VCENTER);
		}

		if ((pWndData->buttons.text[0] != '\0')
            || (pWndData->buttons.szDisplay[0] != '\0'))
		{
            rcButton.left += MENU_RIGHT_GAP;
            rcButton.right -= MENU_LEFT_GAP;

            rcButton.bottom = rcButton.bottom - rcButton.top;
            rcButton.top = 0;
            DrawText(hMemDC, pWndData->buttons.szDisplay, -1, &rcButton,
                DT_RIGHT | DT_VCENTER);
		}
		BitBlt(hdc, rcBottomCaption.left, rcBottomCaption.top, rcBottomCaption.right-rcBottomCaption.left,
			rcBottomCaption.bottom - rcBottomCaption.top, hMemDC, 0, 0, SRCCOPY);
		SetBkMode(hMemDC, nMemBkMode);
		DeleteDC(hMemDC);

        SetBkMode(hdc, nBkMode);
		dwStyle = GetWindowLong(hWnd, GWL_STYLE);
	}

    if (nPart != NC_ALL)
    {
        ReleaseDC(hWnd, hdc);
        return;
    }

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
    if (nBorderW != 0)
    {
        // Restores the rcWindow to window size
        InflateRect(&rcWindow, nBorderW, nBorderH);

        if (nBorderW == GetSystemMetrics(SM_CXBORDER))  // thin border
        {
            SelectObject(hdc, GetStockObject(BLACK_PEN));
            hBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            DrawRect(hdc, &rcWindow);
            SelectObject(hdc, hBrush);
        }
        else    // thick frame or dialog frame
        {
            HPEN hpenBorder, hpenOld;

#if (INTERFACE_MONO)

            hpenBorder = CreatePen(PS_INSIDEFRAME, nBorderW, 
                COLOR_BLACK);
            hpenOld = SelectObject(hdc, hpenBorder);
            
            // Sets the brush to null brush
            hBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
            
            DrawRect(hdc, &rcWindow);
            SelectObject(hdc, hBrush);
            
            SelectObject(hdc, hpenOld);
            DeleteObject(hpenBorder);

#else // INTERFACE_MONO

            if (GetDeviceCaps(NULL, BITSPIXEL) == 1)   // mono device
            {
                hpenBorder = CreatePen(PS_INSIDEFRAME, nBorderW, 
                    COLOR_BLACK);
                hpenOld = SelectObject(hdc, hpenBorder);
                
                // Sets the brush to null brush
                hBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
                
                DrawRect(hdc, &rcWindow);
                SelectObject(hdc, hBrush);
                
                SelectObject(hdc, hpenOld);
                DeleteObject(hpenBorder);
            }
            else    // color device
            {
                // Sets the brush to null brush
                hBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));

                // Sets the pen to light gray pen
                SelectObject(hdc, GetStockObject(LTGRAY_PEN));
                
                // draw left vertical lightgray line
                DrawLine(hdc, rcWindow.left, rcWindow.top, 
                    rcWindow.left, rcWindow.bottom - 1);
                
                // draw top horizontal lightgray line
                DrawLine(hdc, rcWindow.left + 1, rcWindow.top, 
                    rcWindow.right - 1, rcWindow.top);
                
                // draw two lightgray rect in the internal part
                SetRect(&rect, rcWindow.left + 2, rcWindow.top + 2, 
                    rcWindow.right - 2, rcWindow.bottom - 2);
                
                DrawRect(hdc, &rect);
                
                SetRect(&rect, rcWindow.left + 3, rcWindow.top + 3, 
                    rcWindow.right - 3, rcWindow.bottom - 3);
                DrawRect(hdc, &rect);
                
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
                
                SelectObject(hdc, hBrush);
            }
#endif // INTERFACE_MONO
        }
    }
       
    ReleaseDC(hWnd, hdc);
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
#else INTERFACE_MONO
    // 单色和四级灰度情况下不需要重画NC区
    if (GetDeviceCaps(NULL, BITSPIXEL) <= 2)
        return;

    DoNCPaint(hWnd, pWndData, NC_CAPTION);
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
        
        if (hbrBackground)
        {
            GetClientRect(hWnd, &rcClient);
            DPtoLP(hdc, (PPOINT)&rcClient, 2);
            FillRect(hdc, &rcClient, hbrBackground);
            bRet = TRUE;
        }
        else
        {
            GetClientRect(hWnd, &rcClient);
            DPtoLP(hdc, (PPOINT)&rcClient, 2);
            FillRect(hdc, &rcClient, hFillBrush);
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

static int GetBorderWidth(DWORD dwStyle)
{
    int nBorder;

    if ((dwStyle & WS_DLGFRAME) == WS_DLGFRAME)
        nBorder = GetSystemMetrics(SM_CXDLGFRAME);
    else if (dwStyle & WS_BORDER)
        nBorder = GetSystemMetrics(SM_CXBORDER);
    else if (dwStyle & WS_THICKFRAME)
        nBorder = GetSystemMetrics(SM_CXFRAME);
    else
        nBorder = 0;

    return nBorder;
}

static int GetBorderHeight(DWORD dwStyle)
{
    int nBorder;

    if ((dwStyle & WS_DLGFRAME) == WS_DLGFRAME)
        nBorder = GetSystemMetrics(SM_CYDLGFRAME);
    else if (dwStyle & WS_BORDER)
        nBorder = GetSystemMetrics(SM_CYBORDER);
    else if (dwStyle & WS_THICKFRAME)
        nBorder = GetSystemMetrics(SM_CYFRAME);
    else
        nBorder = 0;

    return nBorder;
}

static void CalcCaptionRect(HWND hWnd, PWNDDATA pWndData, PRECT prcBottomCaption, 
                            PRECT prcTopCaption, PRECT prcMenu, PRECT prcText,
                            PRECT prcButton)
{
    SIZE szChar;
    RECT rcBottomCaption, rcTopCaption;
    int nBorderW, nBorderH;
    DWORD dwStyle;
    RECT rcWindow;

    GetTextExtentPoint(NULL, "A", 1, &szChar);

    if (!prcBottomCaption)
        prcBottomCaption = &rcBottomCaption;
    if (!prcTopCaption)
        prcTopCaption = &rcTopCaption;

    // First calculates the caption rectangle

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
    
    GetWindowRectEx(hWnd, &rcWindow, W_WINDOW, XY_WINDOW);
    InflateRect(&rcWindow, -nBorderW, -nBorderH);
    
    SetRect(prcBottomCaption, rcWindow.left, 
        rcWindow.bottom - GetSystemMetrics(SM_CYMENU), 
        rcWindow.right, rcWindow.bottom);
    SetRect(prcTopCaption, rcWindow.left, rcWindow.top, 
        rcWindow.right, rcWindow.top + GetSystemMetrics(SM_CYCAPTION));

    if (prcMenu != NULL)
    {
        if (pWndData->LeftButton.id != NULL)
        {
            prcMenu->left = prcBottomCaption->left;
            prcMenu->top = prcBottomCaption->top;
            prcMenu->right =  prcBottomCaption->left + BTN_WIDTH;
            prcMenu->bottom = prcBottomCaption->bottom;
        }
        else
        {
            prcMenu->left = prcBottomCaption->left;
            prcMenu->top = prcBottomCaption->top;
            prcMenu->right = prcBottomCaption->left + BTN_WIDTH;
            prcMenu->bottom = prcBottomCaption->bottom;
        }
    }

    if (prcText)
    {
        prcText->left = prcTopCaption->left + MENU_LEFT_GAP + 
            szChar.cx * MAX_TEXT + MENU_TEXT_GAP * 2 + 
            MENU_RIGHT_GAP;
        prcText->top = prcTopCaption->top;
        prcText->right = prcTopCaption->right;
        prcText->bottom = prcTopCaption->bottom;
    }

    if (prcButton != NULL)
    {
        if (pWndData->buttons.id != NULL)
        {
            prcButton->left = prcBottomCaption->right - BTN_WIDTH;
            prcButton->top = prcBottomCaption->top;
            prcButton->right = prcBottomCaption->right;
            prcButton->bottom = prcBottomCaption->bottom;
        }
        else
        {
//            prcButton->left =0;
//            prcButton->top = 0;
//            prcButton->right = 0;
//            prcButton->bottom = 0;
            prcButton->left = prcBottomCaption->right - BTN_WIDTH;
            prcButton->top = prcBottomCaption->top;
            prcButton->right = prcBottomCaption->right;
            prcButton->bottom = prcBottomCaption->bottom;
        }
    }
}

static void DrawButtonRect(HDC hdc, const RECT* pRect, BOOL bLeft)
{
#if 0
	DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, 
        pRect->top);
    DrawLine(hdc, pRect->left + 1, pRect->bottom - 1, pRect->right - 1, 
        pRect->bottom - 1);
    DrawLine(hdc, pRect->left, pRect->top + 1, pRect->left, 
        pRect->bottom - 1);
    DrawLine(hdc, pRect->right - 1, pRect->top + 1, pRect->right - 1, 
        pRect->bottom - 1);
#endif
//	PCOLORSCHEME pColorScheme;
//	
//	pColorScheme = GetColorScheme();
//	if (bLeft)
//	{
//		DrawBitmapFromFile(hdc, pRect->left, pRect->top,
//			pColorScheme->BarBtnDownBmpName, SRCCOPY);
//	}
//	else
//		DrawBitmapFromFile(hdc, pRect->left, pRect->top,
//			pColorScheme->BarMenuDownBmpName, SRCCOPY);
//
}

static HWND GetTopWindow(HWND hWnd)
{
    HWND hwndTop;
    
    hwndTop = hWnd;
    while ((hWnd = GetParent(hWnd)) != NULL)
        hwndTop = hWnd;

    return hwndTop;
}

COLORREF GetWindowColor(int index)
{
    if (index == FOCUS_COLOR)
        return FocusColor;
    else if (index == BTN_COLOR)
        return BtnColor;
    else if (index == BAR_COLOR)
        return BarColor;
    else if (index == CAP_COLOR)
        return CapColor;
    else 
        return NULL;
}  

void SetWindowColor(int index, COLORREF color)
{
    if (index == FOCUS_COLOR)
        FocusColor = color;
    else if (index == BTN_COLOR)
        BtnColor = color;
    else if (index == BAR_COLOR)
        BarColor = color;
    else if (index == CAP_COLOR)
        CapColor = color;
    return;
}  

static PWNDDATA GetWndData(HWND hWnd)
{
    return (PWNDDATA)((BYTE*)GetUserDataBase(hWnd));
}

HWND WINAPI CreateFrameWindow(DWORD dwstyle)
{
    HWND hWnd;

    hWnd = CreateWindow("FRAMEWINDOW", NULL, dwstyle | WS_POPUP | WS_VISIBLE, PLX_WIN_POSITION /*0, 0, 176, 205,220*/,
        NULL, NULL, NULL, NULL);  //???176*220,应该比这小点吧
    return hWnd;
}

static LRESULT WINAPI FrameDefWindowProc(HWND hWnd, UINT message, WPARAM wParam, 
                                         LPARAM lParam)
{
    PWNDDATA pFrameWndData;
    LRESULT lResult;
    SIZE szChar;
//    int scdata;
    PSTR pszText;
//    HDC hdc;
    int nMaxCount, nTextLen;    
    DWORD dwStyle;
    char * GetText;
    HWND hChildLast;
    PAINTSTRUCT ps;
	PCREATESTRUCT pcs;
	int nBorderW,nBorderH;
    HDC hdc = NULL;
    
    pFrameWndData = (PWNDDATA)GetWndData(hWnd);
    if (!pFrameWndData)
        return 0;
    
    lResult = (LRESULT)TRUE;
    
    switch (message)
    {

    case WM_DATACREATE :

        if (!bInit)
        {
            if (!WINPDA_Init())
                return (LRESULT)FALSE;

            bInit = TRUE;
        }

        pcs = (PCREATESTRUCT)lParam;
        memset(pFrameWndData, 0, sizeof(WNDDATA));

        // Saves the window name field
        if (pcs->lpszName)
        {
            pFrameWndData->pszText = MemAlloc(strlen(pcs->lpszName) + 1);
            if (pFrameWndData->pszText)
                strcpy(pFrameWndData->pszText, pcs->lpszName);
        }

        nBorderW = GetBorderWidth(pcs->style);
        nBorderH = GetBorderHeight(pcs->style);

        // Initializes buttons

        pFrameWndData->buttons.id = NULL;
        pFrameWndData->LeftButton.id = NULL;
        pFrameWndData->SetScrollState = 0;
        //pFrameWndData->ScrollMask = 0;
        pFrameWndData->MenuState = 0;
        //pFrameWndData->ScrollState = 0;

        pFrameWndData->IconName[0] = '\0';
		pFrameWndData->IconName2[0] = '\0';
        pFrameWndData->hBitmap = NULL;
        pFrameWndData->hBitmap2 = NULL;
        pFrameWndData->bSignal = FALSE;
        pFrameWndData->bPower = FALSE;
        pFrameWndData->MenuText[0] = '\0';

        if (pcs->style & WS_VSCROLL)
        {
            pFrameWndData->pVScrollData = MemAlloc(sizeof(SCROLLDATA));
            if (!pFrameWndData->pVScrollData)
                return FALSE;

            SB_Init(pFrameWndData->pVScrollData, SBS_VERT | SBS_NCSCROLL, 
                pcs->hwndParent);
        }

        if (pcs->style & WS_HSCROLL)
        {
            pFrameWndData->pHScrollData = MemAlloc(sizeof(SCROLLDATA));
            if (!pFrameWndData->pHScrollData)
                return FALSE;

            SB_Init(pFrameWndData->pHScrollData, SBS_HORZ | SBS_NCSCROLL, 
                pcs->hwndParent);
        }

        break;

    case WM_SETTEXT :

        // The data of the default window isn't initialized, can't set
        // window text
        if (!pFrameWndData)
            return 0;

        pszText = (PSTR)lParam;
        if (wParam == 1)
        {
            if (pszText)
            {                
                if (strlen(pszText) >= 127)
                {
                    pFrameWndData->szCapBmpName[0] = '\0';
                    lResult = FALSE;
                }
                else
                {
                    strcpy(pFrameWndData->szCapBmpName, pszText);
                    pFrameWndData->szCapBmpName[strlen(pszText)] = '\0';
                    DoNCPaint(hWnd, pFrameWndData, NC_WINDOWTEXT);
                    lResult = TRUE;
                }                
            }
            else
            {
                pFrameWndData->szCapBmpName[0] = '\0';
                DoNCPaint(hWnd, pFrameWndData, NC_WINDOWTEXT);
                lResult = TRUE;
            }
            break;
        }

        if (pFrameWndData->pszText == NULL || 
            strlen(pFrameWndData->pszText) < strlen(pszText))
        {
            if (pFrameWndData->pszText)
                MemFree(pFrameWndData->pszText);

            pFrameWndData->pszText = MemAlloc(strlen(pszText) + 1);
        }

        if (pFrameWndData->pszText)
        {
            strcpy(pFrameWndData->pszText, pszText);

            if (pFrameWndData->szCapBmpName[0] == '\0')
                DoNCPaint(hWnd, pFrameWndData, NC_WINDOWTEXT);
            lResult = (LRESULT)TRUE;
        }
        else
            lResult = (LRESULT)FALSE;

        break;
    case WM_ACTIVATE:
		if (WA_INACTIVE != LOWORD(wParam))
		{
            hChildLast = GetWindow(hWnd, GW_CHILD);
			dwStyle = (DWORD)GetWindowLong(hChildLast, GWL_STYLE);
			while (dwStyle && !(dwStyle & WS_VISIBLE))
			{
				hChildLast = GetWindow((HWND)hChildLast, GW_HWNDNEXT);
				dwStyle = (DWORD)GetWindowLong(hChildLast, GWL_STYLE);
			}

			if (hChildLast)
            {
				SendMessage(hChildLast, PWM_SHOWWINDOW, (WPARAM)hWnd, 0);
                SendMessage(hChildLast, PWM_ACTIVATE, WA_ACTIVE, 0);
            }
		}
        else
        {
            hChildLast = GetWindow(hWnd, GW_CHILD);
			if (hChildLast)
				SendMessage(hChildLast, PWM_ACTIVATE, WA_INACTIVE, 0);
        }
        break;

    case PWM_CLOSEWINDOW:
        hChildLast = GetWindow(hWnd, GW_CHILD);
        if (hChildLast == (HWND)wParam)
        {
            hChildLast = GetWindow((HWND)wParam, GW_HWNDNEXT);
            if (!hChildLast)
                PostMessage(hWnd, WM_CLOSE, 0, 0);
        }
        break;
    case PWM_CLOSEWINDOWEX:
        hChildLast = GetWindow(hWnd, GW_CHILD);
        if (hChildLast == (HWND)wParam)
        {
            SendMessage(hChildLast, PWM_ACTIVATE, WA_INACTIVE, 0);
            if (hChildLast == GetWindow(hWnd, GW_CHILD))
            {
                hChildLast = GetWindow((HWND)wParam, GW_HWNDNEXT);
                dwStyle = (DWORD)GetWindowLong(hChildLast, GWL_STYLE);
                while (dwStyle && !(dwStyle & WS_VISIBLE))
                {
                    hChildLast = GetWindow((HWND)hChildLast, GW_HWNDNEXT);
                    dwStyle = (DWORD)GetWindowLong(hChildLast, GWL_STYLE);
                }
                
                if (hChildLast)
                {
					if (hWnd == GetActiveWindow())
					{
						SendMessage(hChildLast, PWM_SHOWWINDOW, (WPARAM)hWnd, 0);
						SendMessage(hChildLast, PWM_ACTIVATE, WA_ACTIVE, 0);
					}
                }
                else
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
            }
        }
        break;

    case WM_CLOSE :

        // Default WM_CLOSE handling is to destroy the window.
        DestroyWindow(hWnd);

        break;

    case WM_PAINT :

        BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);

        break;

    case WM_ERASEBKGND :

        return (LRESULT)EraseBkgnd(hWnd, message, (HDC)wParam);
    

    case WM_NCACTIVATE:

        // Do non-client drawing in response to activation or deactivation.
        DoNCActivate(hWnd, pFrameWndData, (BOOL)wParam);
        return (LRESULT)TRUE;

    case PWM_CREATECAPTIONBUTTON :
        
        if (HIWORD(wParam) == 0)
        {
            int nStyle;

			if(lParam==NULL) //by axli to avoid down when implement strlen(lparam)
				break;

            GetTextExtentPoint(NULL, "A", 1, &szChar);
            pFrameWndData->buttons.id = LOWORD(wParam);
            nTextLen = strlen((PCSTR)lParam);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(pFrameWndData->buttons.text, (PCSTR)lParam, nTextLen);
            pFrameWndData->buttons.text[nTextLen] = '\0';
            hdc = GetDC(hWnd);
            GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                pFrameWndData->buttons.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
            ReleaseDC(hWnd, hdc);
            pFrameWndData->buttons.rect.left = pFrameWndData->buttons.rect.right -
                MENU_TEXT_GAP * 2 - szChar.cx * strlen(pFrameWndData->buttons.text);
            nStyle =  GetWindowLong(hWnd,GWL_STYLE);
            nStyle |= PWS_STATICBAR;
            SetWindowLong(hWnd,GWL_STYLE,nStyle);
            DoNCPaint(hWnd, pFrameWndData, NC_MENUBAR);
        }
        else
        {
            int nStyle;

			if(lParam==NULL) //by axli
				break;

            GetTextExtentPoint(NULL, "A", 1, &szChar);
            pFrameWndData->LeftButton.id = LOWORD(wParam);
            nTextLen = strlen((PCSTR)lParam);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(pFrameWndData->LeftButton.text, (PCSTR)lParam, nTextLen);
            pFrameWndData->LeftButton.text[nTextLen] = '\0';
            hdc = GetDC(hWnd);
            GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                pFrameWndData->LeftButton.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
            ReleaseDC(hWnd, hdc);
            pFrameWndData->LeftButton.rect.right = pFrameWndData->LeftButton.rect.left 
                + MENU_TEXT_GAP* 2 + szChar.cx * strlen(pFrameWndData->LeftButton.text);
            nStyle =  GetWindowLong(hWnd,GWL_STYLE);
            nStyle |= PWS_STATICBAR;
            SetWindowLong(hWnd,GWL_STYLE,nStyle);
            DoNCPaint(hWnd, pFrameWndData, NC_MENUBAR);
        }
        
        
        break;
        
    case PWM_SETBUTTONTEXT :
        
        GetTextExtentPoint(NULL, "A", 1, &szChar);
        
        if ((LOWORD(wParam) == 0) && (pFrameWndData->buttons.id != NULL))
        {
            if (pFrameWndData->MenuState)
            {
                if (lParam == NULL)
				{
                    pFrameWndData->oldText[0][0] = '\0';
					pFrameWndData->buttons.text[0] = '\0';
				}
                else
                {
                    nTextLen = strlen((PCSTR)lParam);
                    if (nTextLen > MAX_BUTTON_TEXT - 1)
                        nTextLen = MAX_BUTTON_TEXT - 1;
					strncpy(pFrameWndData->buttons.text, (PCSTR)lParam, nTextLen);
                    pFrameWndData->buttons.text[nTextLen] = '\0';
                    hdc = GetDC(hWnd);
                    GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                        pFrameWndData->oldText[0], 16, BTN_TEXT_WIDTH, '.', 0);
                    ReleaseDC(hWnd, hdc);
                }
            }
            else
            {
                if (lParam == NULL)
                {
                    pFrameWndData->buttons.text[0] = '\0';
                    DoNCPaint(hWnd, pFrameWndData, NC_MENUBAR);//NC_BUTTON
                }
                else
                {
                    nTextLen = strlen((PCSTR)lParam);
                    if (nTextLen > MAX_BUTTON_TEXT - 1)
                        nTextLen = MAX_BUTTON_TEXT - 1;
                    strncpy(pFrameWndData->buttons.text, (PCSTR)lParam, nTextLen);
                    pFrameWndData->buttons.text[nTextLen] = '\0';
                    hdc = GetDC(hWnd);
                    GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                        pFrameWndData->buttons.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
                    ReleaseDC(hWnd, hdc);
                    pFrameWndData->buttons.rect.left = pFrameWndData->buttons.rect.right -  
                        MENU_TEXT_GAP * 2 - szChar.cx * strlen(pFrameWndData->buttons.text);
                    DoNCPaint(hWnd, pFrameWndData, NC_MENUBAR);//NC_BUTTON
                }
            }
        }
        else if ((LOWORD(wParam) == 1) && (pFrameWndData->LeftButton.id != NULL))
        {
            //nTextLen = strlen((PCSTR)pFrameWndData->LeftButton.text) + 1;
            if (pFrameWndData->MenuState)
            {
                if (lParam == NULL)
				{
                    pFrameWndData->oldText[1][0] = '\0';
					pFrameWndData->LeftButton.text[0] = '\0';
				}
                else
                {

					nTextLen = strlen((PCSTR)lParam);
                    if (nTextLen > MAX_BUTTON_TEXT - 1)
                        nTextLen = MAX_BUTTON_TEXT - 1;
					strncpy(pFrameWndData->LeftButton.text, (PCSTR)lParam, nTextLen);
                    pFrameWndData->LeftButton.text[nTextLen] = '\0';
                    hdc = GetDC(hWnd);
                    GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                        pFrameWndData->oldText[1], 16, BTN_TEXT_WIDTH, '.', 0);
                    ReleaseDC(hWnd, hdc);
                }
            }
            else
            {
                if (lParam == NULL)
                {
                    pFrameWndData->LeftButton.text[0] = '\0';
                    DoNCPaint(hWnd, pFrameWndData, NC_MENUBAR);
                }
                else
                {
                    nTextLen = strlen((PCSTR)lParam);
                    if (nTextLen > MAX_BUTTON_TEXT - 1)
                        nTextLen = MAX_BUTTON_TEXT - 1;
                    strncpy(pFrameWndData->LeftButton.text, (PCSTR)lParam, nTextLen);
                    pFrameWndData->LeftButton.text[nTextLen] = '\0';
                    hdc = GetDC(hWnd);
                    GetExtentFittedText(hdc, (PCSTR)lParam, nTextLen,
                        pFrameWndData->LeftButton.szDisplay, 16, BTN_TEXT_WIDTH, '.', 0);
                    ReleaseDC(hWnd, hdc);
                    pFrameWndData->LeftButton.rect.right = pFrameWndData->LeftButton.rect.left +
                        MENU_TEXT_GAP* 2 + szChar.cx * strlen(pFrameWndData->LeftButton.text);
                    DoNCPaint(hWnd, pFrameWndData, NC_MENUBAR);
                }
            }
        }
#ifdef SEF_SMARTPHONE
        else if (LOWORD(wParam) == 2)
        {
            nTextLen = strlen((PCSTR)lParam);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(pFrameWndData->MenuText, (PCSTR)lParam, nTextLen);
            pFrameWndData->MenuText[nTextLen] = '\0';
            DoNCPaint(hWnd, pFrameWndData, NC_MENUBAR);
        }
        else if (LOWORD(wParam) == 3)
        {
            nTextLen = strlen((PCSTR)lParam);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(pFrameWndData->UseMenu, (PCSTR)lParam, nTextLen);
            pFrameWndData->MenuText[nTextLen] = '\0';
            DoNCPaint(hWnd, pFrameWndData, NC_MENUBAR);
        }
#endif //SEF_SMARTPHONE
        break;
        
    case PWM_GETBUTTONTEXT :
        
        if (LOWORD(wParam) == 0)
        {
            GetText = (char *)lParam;
            GetTextExtentPoint(NULL, "A", 1, &szChar);
            nTextLen = strlen((PCSTR)pFrameWndData->buttons.text);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(GetText, pFrameWndData->buttons.text, nTextLen);
            GetText[nTextLen] = '\0';
        }
        else if (LOWORD(wParam) == 1)
        {
            GetText = (char *)lParam;
            nTextLen = strlen((PCSTR)pFrameWndData->LeftButton.text);
            if (nTextLen > MAX_BUTTON_TEXT - 1)
                nTextLen = MAX_BUTTON_TEXT - 1;
            strncpy(GetText, pFrameWndData->LeftButton.text, nTextLen);
            GetText[nTextLen] = '\0';
        }
        else if (LOWORD(wParam) == 2)
        {
            GetText = (char*)lParam;
            strcpy(GetText, (const char*)pFrameWndData->MenuText);
        }
        break;
        
    case PWM_SETDEFWINDOWCOLOR :
        switch(wParam)
        {
        case CAP_COLOR:
            CapColor = (COLORREF)lParam;
            break;
        case BAR_COLOR:
            BarColor = (COLORREF)lParam;
            break;
        case BTN_COLOR:
            BtnColor = (COLORREF)lParam;
            break;
        case FOCUS_COLOR:
            FocusColor = (COLORREF)lParam;
            break;
        }
        break;

        case PWM_SETSIGNAL:
            if (HIWORD(wParam) == SIGNALICON && lParam)
            {
                strncpy(Win_SigIcon, (char *)lParam, 127);
                Win_SigIcon[127] = '\0';
            }
            if (HIWORD(wParam) == POWERICON && lParam)
            {
                strncpy(Win_PowIcon, (char *)lParam, 127);
                Win_PowIcon[127] = '\0';
            }
            break;
            
        case PWM_SETAPPICON:
            if (LOWORD(wParam) == IMAGE_ICON)
            {
                if (HIWORD(wParam) == LEFTICON)
                {
                    if (lParam)
                    {
                        strncpy(pFrameWndData->IconName, (char *)lParam, 127);
                        pFrameWndData->IconName[127] = '\0';
                    }
                    pFrameWndData->bSignal = FALSE;
                    pFrameWndData->hBitmap = NULL;
                }
                else if (HIWORD(wParam) == CALLICON)
                {
                    if (lParam)
                    {
                        strncpy(Win_PhoneIcon, (char *)lParam, 127);
                        Win_PhoneIcon[127] = '\0';
                    }
                }
                else if (HIWORD(wParam) == ENDCALLICON)
                {
                    Win_PhoneIcon[0] = '\0';
                }
                else
                {
                    if (lParam)
                    {
                        strncpy(pFrameWndData->IconName2, (char *)lParam, 127);
                        pFrameWndData->IconName2[127] = '\0';
                    }
                    pFrameWndData->bPower = FALSE;
                    pFrameWndData->hBitmap2 = NULL;
                }
            }
            else
            {
                if (HIWORD(wParam) == SIGNALICON)
                {
                    pFrameWndData->bSignal = TRUE;
                    pFrameWndData->hBitmap = NULL;
                }
                else if (HIWORD(wParam) == POWERICON)
                {
                    pFrameWndData->bPower = TRUE;
                    pFrameWndData->hBitmap2 = NULL;
                }
				else if (HIWORD(wParam) == LEFTICON)
                {
                    pFrameWndData->hBitmap = (HBITMAP)lParam;
                    pFrameWndData->bSignal = FALSE;
                    pFrameWndData->IconName[0] = '\0'; 
                }
                else
                {
                    pFrameWndData->hBitmap2 = (HBITMAP)lParam;
                    pFrameWndData->bPower = FALSE;
                    pFrameWndData->IconName2[0] = '\0';
                }
            }
            DoNCPaint(hWnd, pFrameWndData, NC_WINDOWTEXT);
            break;
        case WM_NCPAINT :
            
            DoNCPaint(hWnd, pFrameWndData, NC_ALL);
            break;
        case WM_NCCALCSIZE :
            
            DoNCCalcSize(hWnd, pFrameWndData, wParam, lParam);
            return 0;
        case WM_DATADESTROY :
            
            if (pFrameWndData)
            {
                HMENU hmenu;
                
                // 直接删除hwndListWnd以加快窗口的删除速度
                if (pFrameWndData->pszText)
                    MemFree(pFrameWndData->pszText);
                
                if (pFrameWndData->pVScrollData)
                    MemFree(pFrameWndData->pVScrollData);
                
                if (pFrameWndData->pHScrollData)
                    MemFree(pFrameWndData->pHScrollData);
                
                if (!(GetWindowLong(hWnd, GWL_STYLE) & (WS_CHILD | WS_NCCHILD)))
                {
                    hmenu = (HMENU)GetWindowLong(hWnd, GWL_ID);
                    if (hmenu)
                        DestroyMenu(hmenu);
                }
                //            MemFree(pFrameWndData);
                //
                //            // 窗口结构中的数据指针清空表示窗口数据已被删除
                //            *(DWORD*)(pUserData - 4) = 0;
            }
            
            break;

        case WM_COMMAND:  //???
            hChildLast = GetWindow(hWnd, GW_CHILD);
            SendMessage(hChildLast, WM_COMMAND, wParam, lParam);
            break;

        case WM_INITMENU:
        case WM_INITMENUPOPUP:
            hChildLast = GetWindow(hWnd, GW_CHILD);
            SendMessage(hChildLast, WM_COMMAND, wParam, lParam);
            SendMessage(hChildLast, message, wParam, lParam);
            break;
            
        case WM_GETTEXT :
            
            nMaxCount = wParam;
            pszText = (PSTR)lParam;
            
            if (nMaxCount <= 0)
                return 0;
            
            if (pFrameWndData && pFrameWndData->pszText)
            {
                nTextLen = strlen(pFrameWndData->pszText);
                if (nTextLen > nMaxCount - 1)
                    nTextLen = nMaxCount - 1;
                strncpy(pszText, pFrameWndData->pszText, nTextLen);
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
            
            
            
        case WM_GETTEXTLENGTH :
            
            if (pFrameWndData && pFrameWndData->pszText)
                lResult = (LRESULT)strlen(pFrameWndData->pszText);
            else
                lResult = 0;
            
            break;
            
        case WM_KEYDOWN :
            
            switch (wParam)
            {
#ifndef SEF_SMARTPHONE
            case VK_F2 :
                
                PostMessage(GetTopWindow(hWnd), WM_CLOSE, 0, 0);
                break;
#endif //SEF_SMARTPHONE
            case VK_F10:
                dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
                if((dwStyle & PWS_STATICBAR) && (pFrameWndData->buttons.text[0] != '\0'))
                    SendMessage(hWnd, WM_COMMAND, pFrameWndData->buttons.id, NULL);
                break;
                
            case VK_F5 :
                lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
                break;
//                {
//                    HMENU hMenu;
//                    RECT rcMenu, rcInvert, rcWindow;
//                    HWND  hwndFocus;
//                    char  oldText[16];
//                    
//                    dwStyle = (DWORD)GetWindowLong(hWnd, GWL_STYLE);
//                    hMenu = (HMENU)GetWindowLong(hWnd, GWL_ID);
//                    if (hMenu && (dwStyle & PWS_STATICBAR))
//                    {
//                        CalcCaptionRect(hWnd, pFrameWndData, NULL, NULL, &rcMenu, NULL, NULL);
//                        
//                        CopyRect(&rcInvert, &rcMenu);
//                        
//                        rcInvert.left += MENU_LEFT_GAP;
//                        rcInvert.top += 1;
//                        rcInvert.right -= MENU_RIGHT_GAP;
//                        rcInvert.bottom -= 1;
//                        
//                        InflateRect(&rcInvert, -1, -1);
//                        
//                        hdc = GetWindowDC(hWnd);
//                        //InvertRect(hdc, &rcInvert);
//                        pFrameWndData->MenuState = 1;
//                        ReleaseDC(hWnd, hdc);
//                        
//                        // Converts to screen coordinate
//                        GetWindowRect(hWnd, &rcWindow);
//                        OffsetRect(&rcMenu, rcWindow.left, rcWindow.top);
//                        
//                        scdata = SendMessage(hWnd, PWM_SETSCROLLSTATE, 
//                            (WPARAM)SCROLLUP | SCROLLDOWN, (LPARAM)MASKALL);                
//                        strcpy(oldText, pFrameWndData->buttons.text);
//                        strcpy(pFrameWndData->buttons.text, (PCSTR)GetString(STR_WINDOW_EXIT));
//                        DoNCPaint(hWnd, pFrameWndData, NC_ALL);
//                        
//                        hwndFocus = SetFocus(hWnd);
//                        keybd_event(VK_DOWN, 0, 0, 0);
//                        TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN, 
//                            rcMenu.left + 1, rcMenu.top - 1, 0, hWnd, NULL);
//                        if (IsWindowVisible(hwndFocus) && (hWnd == GetFocus()))
//                        {
//                            SetFocus(hwndFocus);
//                        }
//                        //SetMenuDefaultItem(hMenu, 0, FALSE);
//                        SendMessage(hWnd, PWM_SETSCROLLSTATE, (WPARAM)scdata, (LPARAM)MASKALL);
//                        //SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)oldText);
//                        strcpy(pFrameWndData->buttons.text, oldText);
//                        
//                        hdc = GetWindowDC(hWnd);
//                        //InvertRect(hdc, &rcInvert);
//                        pFrameWndData->MenuState = 0;
//                        DoNCPaint(hWnd, pFrameWndData, NC_ALL);
//                        ReleaseDC(hWnd, hdc);
//                        
//                        // 清空输入队列防止菜单重发的WM_LBUTTONDOWN消息引起MENU再次
//                        // 弹出
//                        FlushInputMessages();
//                    }
//                    else if((dwStyle & PWS_STATICBAR) && (pFrameWndData->LeftButton.text[0] != '\0'))
//                        SendMessage(hWnd, WM_COMMAND, pFrameWndData->LeftButton.id, NULL);
//                    break;
//                }
            default :
                break;
            }
            break;

        case WM_SETLBTNTEXT:
            if (lParam != 0)
            {
                SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, lParam);
            }
            break;

        case WM_SETRBTNTEXT:
            SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, lParam);
            break;

        case WM_IMESWITCHED:
            SendMessage(hWnd, PWM_SETAPPICON, MAKEWPARAM(wParam, RIGHTICON),
                lParam);
            break;
    }

    return lResult;
}

/**********************************************************************
 * Function     GetExtentFittedText
 * Purpose      Get the auto truncated string within a specified space.
 * Params       hdc
 *                [in] Handle to the device context.
 *              lpszSrc
 *                [in] Points to the character string to be converted.
 *              cchSrc
 *                [in] Specifies the size in bytes of the string pointed
 *                to by the lpszSrc parameter, or it can be -1 if the
 *                string is null terminated.
 *              lpszDst
 *                [out] Points to a buffer that receives the truncated string.
 *              cchDst
 *                [in] Specifies the size in bytes of the buffer pointed
 *                to by the lpszDst parameter.
 *              nMaxExtent
 *                [in] Specifies the maximum allowable width, in logical
 *                units(i.e. in pixels), of the formatted string.
 *              chSuffix
 *                [in] Specifies the masking character behind the display
 *                string.
 *              nRepeat
 *                [in] Specifies the number of masking character.
 * Return       If the function succeeds, the return value is nonzero.
 *              If the function fails, the return value is zero.
 * Remarks      
 **********************************************************************/

BOOL GetExtentFittedText(HDC hdc, LPCSTR lpszSrc, int cchSrc,
                         LPSTR lpszDst, int cchDst, int nMaxExtent,
                         char chSuffix, int nRepeat)
{
    int   nFit = 0, i = 0, nCopy = 0;
    SIZE  size;
    PINT  ppDx = NULL;
    PSTR  pszSuffix = NULL;

    if (lpszSrc == NULL || cchSrc == 0 || lpszDst == NULL || cchDst <= 1
        || nMaxExtent <= 0)
    {
        if ((cchDst > 0) && (lpszDst != NULL))
        {
            *lpszDst = '\0';
        }
        return FALSE;
    }

    if (cchSrc == -1)
    {
        cchSrc = strlen(lpszSrc);
    }

    ppDx = (PINT)malloc(cchSrc * sizeof(int));
    GetTextExtentExPoint(hdc, lpszSrc, cchSrc, nMaxExtent, &nFit, ppDx, &size);
    if (nFit < cchSrc)
    {
        if (chSuffix != '\0' && nRepeat != 0)
        {
            pszSuffix = (PSTR)malloc(nRepeat + 1);
            memset((void*)pszSuffix, (int)chSuffix, nRepeat);
            pszSuffix[nRepeat] = '\0';
        }
        if (nFit > 1)
        {
            GetTextExtentPoint32(hdc, (LPCSTR)pszSuffix, nRepeat, &size);
            nMaxExtent -= size.cx;
            for (i = 0; i < nFit; i++)
            {
                if (ppDx[i] >= nMaxExtent)
                    break;
            }
            nCopy = max(1, min(i, cchDst - 1));
        }
        else
        {
            nCopy = 1;
        }
        strncpy(lpszDst, (const char*)lpszSrc, nCopy);
        cchDst -= nCopy;
        lpszDst[nCopy] = '\0';
        cchDst--;
        if (chSuffix != '\0' && nRepeat != 0)
        {
            if (cchDst > nRepeat + 1)
            {
                strcat(lpszDst, (const char*)pszSuffix);
            }
            free(pszSuffix);
        }
    }
    else
    {
        nCopy = min(cchSrc, cchDst - 1);
        strncpy(lpszDst, lpszSrc, nCopy);
        lpszDst[nCopy] = '\0';
    }
    free(ppDx);

    return TRUE;
}

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
#include "hpfile.h"
#include "control.h"
#include "string.h"

#define MINTHUMBRANGE       8
#define MONO_GAP            0

#define NOPENDOWN           0
#define PENDOWN_LINEUP      1
#define PENDOWN_LINEDOWN    2
#define PENDOWN_PAGE        3
#define PENDOWN_THUMB       4
#define PENDOWN             5

#define THUMB_LENGTH        40
#define THUMB_WIDTH         5

static HBITMAP hbmpUpArrow, hbmpDownArrow, hbmpMiddle, hbmpbk, hbmpbkDisabled;
static HBITMAP hbmpLeftArrow, hbmpRightArrow, hBmpHorzMidd, hbmpHorzBk;

#if (INTERFACE_MONO)
static HBITMAP hbmpUpArrowDisabled, hbmpDownArrowDisabled;
static HBITMAP hbmpLeftArrowDisabled, hbmpRightArrowDisabled;
#endif // INTERFACE_MONO

static HBRUSH hbrScrollBar;

// Internal function prototypes
static LRESULT CALLBACK SCROLL_WndProc(HWND hWnd, UINT wMsgCmd, 
                                       WPARAM wParam, LPARAM lParam);
static void ScrollFixed(HWND hWnd, HDC hdc, PSCROLLDATA pScrollData, IWORD wType, 
                        BOOL bLine, const RECT* pRect);
static void ScrollProcess(HWND hWnd, HDC hdc, PSCROLLDATA pScrollData, 
                          IWORD x, IWORD y, const RECT* pRect);

#if (!INTERFACE_MONO)
static void Draw3DScrollRect(HDC hdc, const RECT* pRect, int nBitsPixel, 
                             BOOL bFillRect, BOOL bDown);
#endif
static void GetArrowRect(PSCROLLDATA pScrollData, const RECT* pRcScroll, 
                         IWORD wType, RECT* pRect);

static void MoveScrollThumb(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect, 
                            int oldPos, int oldThumbRange);
static void PaintScrollThumb(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect);
static void EraseScrollThumb(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect);
static void PaintArrow(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect,
                       BOOL bLTArrow, BOOL bRBArrow);
static void PaintBKGND(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect);

BOOL SCROLL_RegisterClass(void)
{
    WNDCLASS wc;
    int  nPathLen;
    char PathName[MAX_PATH];

    wc.style            = CS_PARENTDC | CS_DEFWIN;
    wc.lpfnWndProc      = SCROLL_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(SCROLLDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "SCROLLBAR";

    if (!RegisterClass(&wc))
        return FALSE;

    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);

    hbrScrollBar = CreateHatchBrush(HS_SIEVE, 
        GetSysColor(COLOR_SCROLLBAR));

    /*hbmpUpArrow = CreateBitmap(UDARROW_BMP_WIDTH, UDARROW_BMP_HEIGHT, 
        1, 1, UpArrowBmpData);
    hbmpDownArrow = CreateBitmap(UDARROW_BMP_WIDTH, UDARROW_BMP_HEIGHT, 
        1, 1, DownArrowBmpData);*/

    strcat(PathName, "scbar.bmp");
    hbmpMiddle = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scbar1.bmp");
    hBmpHorzMidd = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    
    PathName[nPathLen] = '\0';
    strcat(PathName, "scbg2.bmp");
    hbmpHorzBk = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scbg.bmp");
    hbmpbk = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scdisbg.bmp");
    hbmpbkDisabled = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scup.bmp");
    hbmpUpArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scdn.bmp");
    hbmpDownArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scle.bmp");
    hbmpLeftArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scri.bmp");
    hbmpRightArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

#if (INTERFACE_MONO)
    PathName[nPathLen] = '\0';
    strcat(PathName, "scdisup.bmp");
    hbmpUpArrowDisabled = LoadImage(NULL, PathName, IMAGE_BITMAP, 
        0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scdisdn.bmp");
    hbmpDownArrowDisabled = LoadImage(NULL, PathName, IMAGE_BITMAP, 
        0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scdisle.bmp");
    hbmpLeftArrowDisabled = LoadImage(NULL, PathName, IMAGE_BITMAP, 
        0, 0, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "scdisri.bmp");
    hbmpRightArrowDisabled = LoadImage(NULL, PathName, IMAGE_BITMAP, 
        0, 0, LR_LOADFROMFILE);
#endif // INTERFACE_MONO

    return TRUE;
}

static LRESULT CALLBACK SCROLL_WndProc(HWND hWnd, UINT wMsgCmd, 
                                       WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    PSCROLLDATA     pScrollData;
    RECT            rect;
    HDC             hdc;
    HBRUSH          hBrush;
    PCREATESTRUCT   pCreateData;
    int             nWidth, nHeight;

#if (!NOSENDCTLCOLOR)
    HWND            hWndParent;
#endif 

    lResult = 0;
    pScrollData = (PSCROLLDATA)GetUserData(hWnd);
    
    switch (wMsgCmd)
    {
    case WM_NCCREATE :
        
        pCreateData = (PCREATESTRUCT)lParam;
        SB_Init(pScrollData, pCreateData->style, pCreateData->hwndParent);

        // Creating succeed.
        lResult = (LRESULT)TRUE;
        
        break;
        
    case WM_NCDESTROY :
        
        break;
        
    case WM_PAINT :
        
        hdc = BeginPaint(hWnd, NULL);
        
        GetClientRect(hWnd, &rect);
        SB_Paint(hdc, pScrollData, &rect);

        EndPaint(hWnd, NULL);
        
        break;

    case WM_ERASEBKGND:
        
        if (pScrollData->byVScroll & SBS_NCSCROLL)
            break;

        hdc = (HDC)wParam;

        // Only scroll control can receive WM_ERASEBKGND message, 
        // NCScroll call SB_PAINT directly.
#if (!NOSENDCTLCOLOR)
        if (pScrollData->byVScroll & SBS_CHILDWND)
            hWndParent = GetParent(hWnd);
        else
            hWndParent = GetWindow(hWnd, GW_OWNER);
        
        if (!hWndParent)
#endif
        {
            SetBkColor(hdc, COLOR_WHITE);
            SetTextColor(hdc, GetSysColor(COLOR_SCROLLBAR));

#if (INTERFACE_MONO)
            hBrush = GetStockObject(WHITE_BRUSH);
#endif // INTERFACE_MONO
        }
#if (!NOSENDCTLCOLOR)
        else
        {
            hBrush = (HBRUSH)SendMessage(hWndParent, WM_CTLCOLORSCROLLBAR, 
                (WPARAM)hdc, (LPARAM)hWnd);
        }
#endif

        GetClientRect(hWnd, &rect);
        ClearRect(hdc, &rect, COLOR_WHITE);
        lResult = (LRESULT)TRUE;
        break;
  
    case WM_GETDLGCODE:
        
        lResult = (LRESULT)DLGC_WANTARROWS;
        break;

    case WM_SIZE :
        
        nWidth = LOWORD(lParam);  // width of client area 
        nHeight = HIWORD(lParam); 
        
        if (pScrollData->byVScroll & SBS_VERT)
            lResult = (LRESULT)SB_Size(pScrollData, nHeight);
        else
            lResult = (LRESULT)SB_Size(pScrollData, nWidth);

        break;

    case WM_PENDOWN :
    case WM_PENMOVE :
    case WM_PENUP :
        
        GetClientRect(hWnd, &rect);
        hdc = GetDC(hWnd);
        SB_PenProcess(hWnd, hdc, pScrollData, &rect, wMsgCmd, lParam);
        ReleaseDC(hWnd, hdc);
        break;
        
    case WM_SYSTIMER :

        GetClientRect(hWnd, &rect);
        hdc = GetDC(hWnd);
        SB_Timer(hWnd, hdc, pScrollData, &rect);
        ReleaseDC(hWnd, hdc);
        break;

    case SBM_GETSCROLLINFO :

        lResult = (LRESULT)SB_GetScrollInfo(pScrollData, lParam);
        break;

    case SBM_SETSCROLLINFO :
        
        GetClientRect(hWnd, &rect);
        hdc = GetDC(hWnd);

        lResult = (LRESULT)SB_SetScrollInfo(hdc, pScrollData, &rect, wParam, lParam);

        ReleaseDC(hWnd, hdc);
        break;
    
    case SBM_SETPOS :       

        GetClientRect(hWnd, &rect);
        hdc = GetDC(hWnd);

        lResult = (LRESULT)SB_SetPos(hdc, pScrollData, &rect, wParam, lParam);
        ReleaseDC(hWnd, hdc);

        break;
    
    case SBM_GETPOS :     

        lResult = (LRESULT)SB_GetPos(pScrollData);
        break;
    
    case SBM_SETRANGE :

        lResult = (LRESULT)SB_SetRange(pScrollData, wParam, lParam);

        break;
    
    case SBM_SETRANGEREDRAW :

        GetClientRect(hWnd, &rect);
        hdc = GetDC(hWnd);

        lResult = (LRESULT)SB_SetRangeRedraw(hdc, pScrollData, &rect, wParam, lParam);
        ReleaseDC(hWnd, hdc);

        break;
    
    case SBM_GETRANGE :   

        lResult = (LRESULT)SB_GetRange(pScrollData, wParam, lParam);
        break;

    case SBM_ENABLE_ARROWS :

        GetClientRect(hWnd, &rect);
        hdc = GetDC(hWnd);

        lResult = (LRESULT)SB_Enable_Arrows(hdc, pScrollData, &rect, wParam);
        ReleaseDC(hWnd, hdc);
        break;

#if (!NOKEYBOARD)
//#ifndef SCP_NOKEYBOARD
    case WM_KEYDOWN :
    {
        HWND hWndParent;
        
        //Process control keyboard.
        switch ((WORD)wParam)
        {
        default :
            break;
            
        case VK_LEFT:
        case VK_UP:
            
            hdc = GetDC(hWnd);
            ScrollFixed(hWnd, hdc, pScrollData, 1, TRUE, NULL);
            ReleaseDC(hWnd, hdc);
            break;
            
        case VK_RIGHT:
        case VK_DOWN:
            
            hdc = GetDC(hWnd);
            ScrollFixed(hWnd, hdc, pScrollData, 2, TRUE, NULL);
            ReleaseDC(hWnd, hdc);
            break;
            
        case VK_PAGEUP:
            
            hdc = GetDC(hWnd);
            ScrollFixed(hWnd, hdc, pScrollData, 1, FALSE, NULL);
            ReleaseDC(hWnd, hdc);
            break;
            
        case VK_PAGEDOWN:
            
            hdc = GetDC(hWnd);
            ScrollFixed(hWnd, hdc, pScrollData, 2, FALSE, NULL);
            ReleaseDC(hWnd, hdc);
            break;
            
        case VK_HOME:
            
            if (pScrollData->byVScroll & SBS_CHILDWND)
                hWndParent = GetParent(hWnd);
            else
                hWndParent = GetWindow(hWnd, GW_OWNER);
            
            if (pScrollData->byVScroll & SBS_VERT)
                PostMessage(hWndParent, WM_VSCROLL, MAKEWPARAM(SB_TOP, 0), (LPARAM)hWnd);
            else
                PostMessage(hWndParent, WM_HSCROLL, MAKEWPARAM(SB_TOP, 0), (LPARAM)hWnd);

            break;
            
        case VK_END:
            
            if (pScrollData->byVScroll & SBS_CHILDWND)
                hWndParent = GetParent(hWnd);
            else
                hWndParent = GetWindow(hWnd, GW_OWNER);
            
            if (pScrollData->byVScroll & SBS_VERT)
                PostMessage(hWndParent, WM_VSCROLL, MAKEWPARAM(SB_BOTTOM, 0), (LPARAM)hWnd);
            else
                PostMessage(hWndParent, WM_HSCROLL, MAKEWPARAM(SB_BOTTOM, 0), (LPARAM)hWnd);
            
            break;
            
        }
        
        break;
    }
//#endif //SCP_NOKEYBOARD
#endif // NOKEYBOARD

    default :
        
        lResult = DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}

//distinguish line down or page down, line up or page up.
static void ScrollProcess(HWND hWnd, HDC hdc, PSCROLLDATA pScrollData, IWORD x, IWORD y, 
                   const RECT* pRect)
{
    RECT rect1, rect2;
    
    if (pScrollData->byVScroll & SBS_VERT)
    {
        //is thumb
        if (x >= pRect->left && x < pRect->right &&
            y >= pRect->top + pScrollData->nPos && 
            y < pRect->top + pScrollData->nPos + (int)pScrollData->ThumbRange)
        {
            return;
        }
        
        rect1.left = pRect->left;
        rect1.top = pRect->top;
        rect1.right = pRect->right;
        rect1.bottom = rect1.top + pScrollData->nCYVScroll;
        
        rect2.left = pRect->left;
        rect2.top = pRect->bottom - pScrollData->nCYVScroll;
        rect2.right = pRect->right;
        rect2.bottom = pRect->bottom;
    }
    else
    {
        if (y >= pRect->top && y < pRect->bottom &&
            x >= pRect->left + pScrollData->nPos && 
            x < pRect->left + pScrollData->nPos + (int)pScrollData->ThumbRange)
        {
            return;
        }
        
        rect1.left = pRect->left;
        rect1.top = pRect->top;
        rect1.right = pRect->left + pScrollData->nCYVScroll;
        rect1.bottom = pRect->bottom;
        
        rect2.left = pRect->right - pScrollData->nCYVScroll;
        rect2.top = pRect->top;
        rect2.right = pRect->right;
        rect2.bottom = pRect->bottom;
    }
    
    //line_up.
    if (x >= rect1.left && x < rect1.right &&
        y >= rect1.top && y < rect1.bottom)
    {
        pScrollData->byPenDown = PENDOWN_LINEUP;

        if (!pScrollData->bLTArrowDisabled)
            ScrollFixed(hWnd, hdc, pScrollData, 1, TRUE, pRect);
        else
        {
            pScrollData->byPenDown = NOPENDOWN;
            ReleaseCapture();

            if (pScrollData->uTimerID)
            {
                pScrollData->bTimerIn = FALSE;
            }
        }

        return;
    }

    //line_down.
    if (x >= rect2.left && x < rect2.right &&
        y >= rect2.top && y < rect2.bottom)
    {
        pScrollData->byPenDown = PENDOWN_LINEDOWN;

        if (!pScrollData->bRBArrowDisabled)
            ScrollFixed(hWnd, hdc, pScrollData, 2, TRUE, pRect);
        else
        {
            pScrollData->byPenDown = NOPENDOWN;
            ReleaseCapture();

            if (pScrollData->uTimerID)
            {
                pScrollData->bTimerIn = FALSE;
            }
        }

        return;
    }

    //page_up or page_down
    if (pScrollData->byVScroll & SBS_VERT)
    {
        pScrollData->byPenDown = PENDOWN_PAGE;
        
        if (y < pRect->top + pScrollData->nPos)
            ScrollFixed(hWnd, hdc, pScrollData, 1, FALSE, pRect);
        else
            if (y >= pRect->top + pScrollData->nPos + (int)pScrollData->ThumbRange)
                ScrollFixed(hWnd, hdc, pScrollData, 2, FALSE, pRect);
    }
    else
    {
        pScrollData->byPenDown = PENDOWN_PAGE;

        if (x < pRect->left + pScrollData->nPos)
            ScrollFixed(hWnd, hdc, pScrollData, 1, FALSE, pRect);
        else        
            if (x >= pRect->left + pScrollData->nPos + (int)pScrollData->ThumbRange)
                ScrollFixed(hWnd, hdc, pScrollData, 2, FALSE, pRect);
    }
}

//process line up/down/left/right, and page up/down/left/right.
//1 of wType is up, 2 is down.
static void ScrollFixed(HWND hWnd, HDC hdc, PSCROLLDATA pScrollData, 
                        IWORD wType, BOOL bLine, const RECT* pRect)
{
    WPARAM  wParam;
    LPARAM  lParam;
    HWND    hWndParent;
    //char    achClassName[32];
    //IWORD   TempPOS;
    //int     oldPos, oldThumbRange;

    if (!pScrollData->nMaxRangeLogic)
        return;

    if (pScrollData->byVScroll & SBS_VERT)
    {
        if (wType == 1)
        {
            if (bLine)
                wParam = MAKELONG(SB_LINEUP, 0);
            else
                wParam = MAKELONG(SB_PAGEUP, 0);
        }
        else
        {
            if (bLine)
                wParam = MAKELONG(SB_LINEDOWN, 0);
            else
                wParam = MAKELONG(SB_PAGEDOWN, 0);
        }
    }
    else
    {
        if (wType == 1)
        {
            if (bLine)
                wParam = MAKELONG(SB_LINELEFT, 0);
            else
                wParam = MAKELONG(SB_PAGELEFT, 0);
        }
        else
        {
            if (bLine)
                wParam = MAKELONG(SB_LINERIGHT, 0);
            else
                wParam = MAKELONG(SB_PAGERIGHT, 0);
        }
    }

    lParam = (LPARAM)hWnd;

    if (pScrollData->byVScroll & SBS_NCSCROLL)
    {
        hWndParent = hWnd;
    }
    else
    {
        if (pScrollData->byVScroll & SBS_CHILDWND)
            hWndParent = GetParent(hWnd);
        else
            hWndParent = GetWindow(hWnd, GW_OWNER);
    }

    if (pScrollData->byVScroll & SBS_VERT)
        PostMessage(hWndParent, WM_VSCROLL, wParam, lParam);
    else
        PostMessage(hWndParent, WM_HSCROLL, wParam, lParam);
/* 
    GetClassName(hWndParent, achClassName, 32);
    achClassName[32 - 1] = 0;

    if (stricmp(achClassName, "EDIT") && stricmp(achClassName, "LISTBOX"))
        return;

    if (bLine)
        TempPOS = pScrollData->wLineRange;
    else
        TempPOS = pScrollData->nPageRange;

    oldReDrawThumbRange = pScrollData->ThumbRange;
    oldPos = pScrollData->nPos;

    oldThumbRange = pScrollData->nPageRangeLogic * pScrollData->nMaxRange
                      / pScrollData->nMaxRangeLogic ;

    //line_up or page_up        
    if (wType == 1)
    {
        if (bLine)
        {
            if ((pScrollData->nPos - TempPOS * 2) >= pScrollData->nCYVScroll)
            {
                pScrollData->nPosLogic--;
                
                pScrollData->nPos = pScrollData->nCYVScroll + 
                    pScrollData->nPosLogic * 
                    (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                    / pScrollData->nMaxRangeLogic;
            }
            else
            {
                pScrollData->nPos = pScrollData->nCYVScroll;
                pScrollData->nPosLogic = 0;
            }
        }
        else
        {
            if ((pScrollData->nPos - TempPOS) > pScrollData->nCYVScroll)
            {
                //pScrollData->nPos -= TempPOS;
                pScrollData->nPosLogic -= pScrollData->nPageRangeLogic; 

                pScrollData->nPos = pScrollData->nCYVScroll + 
                    pScrollData->nPosLogic * 
                    (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                    / pScrollData->nMaxRangeLogic;
            }
            else
            {
                pScrollData->nPos = pScrollData->nCYVScroll;
                pScrollData->nPosLogic = 0; 
            }
        }
    }
    else    //line_down or page_down
    {
        if (bLine)
        {
            if ((pScrollData->nPos + TempPOS * 2) <= 
                (pScrollData->nMaxRange + pScrollData->nCYVScroll - 
                (int)pScrollData->ThumbRange))
            {
                //pScrollData->nPos += TempPOS;
                pScrollData->nPosLogic++;

                pScrollData->nPos = pScrollData->nCYVScroll + 
                    pScrollData->nPosLogic * 
                    (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                    / pScrollData->nMaxRangeLogic;
            }
            else
            {
                pScrollData->nPos = pScrollData->nMaxRange + 
                    pScrollData->nCYVScroll - pScrollData->ThumbRange;

                pScrollData->nPosLogic = pScrollData->nMaxRangeLogic - 
                    pScrollData->nPageRangeLogic; 
            }
        }
        else
        {
            if ((pScrollData->nPos + TempPOS) < (pScrollData->nMaxRange + 
                pScrollData->nCYVScroll - (int)pScrollData->ThumbRange))
            {
                //pScrollData->nPos += TempPOS;
                pScrollData->nPosLogic += pScrollData->nPageRangeLogic; 
                
                if (pScrollData->nMaxRangeLogic - pScrollData->nPosLogic <= 
                    (int)pScrollData->nPageRangeLogic)
                    //For the last page
                {
                    pScrollData->nPos = pScrollData->nMaxRange + 
                        pScrollData->nCYVScroll - pScrollData->ThumbRange;
                }
                else
                {
                    pScrollData->nPos = pScrollData->nCYVScroll + 
                        pScrollData->nPosLogic * 
                        (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                        / pScrollData->nMaxRangeLogic;
                }
            }
            else
            {
                pScrollData->nPos = pScrollData->nMaxRange + 
                    pScrollData->nCYVScroll - pScrollData->ThumbRange;
                
                pScrollData->nPosLogic = pScrollData->nMaxRangeLogic - 
                    pScrollData->nPageRangeLogic; 
                //pScrollData->nPosLogic = pScrollData->nMaxRangeLogic; 
            }
        }
    }

    MoveScrollThumb(hdc, pScrollData, pRect, oldPos, oldReDrawThumbRange);*/
}

void PaintScrollThumb(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect)
{
    RECT        rect1;
    int         nBitsPixel, nTempPos;
    COLORREF    crBkOld;

    if (pScrollData->bDisabled || pScrollData->ThumbRange < MINTHUMBRANGE)
        return;
    
    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);
    
    if (pScrollData->byPenDown == PENDOWN_THUMB)
        nTempPos = pScrollData->nTrackPos;
    else
        nTempPos = pScrollData->nPos;
    
    if (pScrollData->byVScroll & SBS_VERT)
    {
        if ((pRect->top + nTempPos + (int)pScrollData->ThumbRange) > 
            (pRect->bottom - pScrollData->nCYVScroll))
        {
            nTempPos = pScrollData->nMaxRange + pScrollData->nCYVScroll - 
                pScrollData->ThumbRange;
        }

#if (INTERFACE_MONO)

        SetRect(&rect1, pRect->left + MONO_GAP, pRect->top + nTempPos, 
            pRect->right - MONO_GAP, 
            pRect->top + nTempPos + pScrollData->ThumbRange);

#endif // INTERFACE_MONO

    }
    else
    {
        if ((pRect->left + nTempPos + (int)pScrollData->ThumbRange) > 
            (pRect->right - pScrollData->nCYVScroll))
        {
            nTempPos = pScrollData->nMaxRange + pScrollData->nCYVScroll - 
                (int)pScrollData->ThumbRange;
        }

#if (INTERFACE_MONO)

        SetRect(&rect1, pRect->left + nTempPos, pRect->top + MONO_GAP, 
            pRect->left + nTempPos + pScrollData->ThumbRange, 
            pRect->bottom - MONO_GAP);

#endif // INTERFACE_MONO

    }

    crBkOld = SetBkColor(hdc, COLOR_WHITE);
    //crTextOld = SetTextColor(hdc, COLOR_BLACK);

#if (INTERFACE_MONO)
    
    if (pScrollData->byVScroll & SBS_VERT)
    {    
        StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
            rect1.bottom - rect1.top, (HDC)hbmpMiddle, 0, 0, THUMB_WIDTH, THUMB_LENGTH, SRCCOPY);
    }
    else
    {    
        StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
            rect1.bottom - rect1.top, (HDC)hBmpHorzMidd, 0, 0, THUMB_LENGTH, THUMB_WIDTH, SRCCOPY);
    }
#endif // INTERFACE_MONO

    //SetBkColor(hdc, crBkOld);
    //SetTextColor(hdc, crTextOld);
}

static void MoveScrollThumb(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect, 
                            int oldPos, int oldThumbRange)
{
    RECT    rect1, rect2;
    int     nLeftTop, nRightBottom, nTempPos, nEdge;
    int     nBitsPixel, nRePaintStart, nRePaintEnd, nRePaintStart1 = 0,
            nRePaintEnd1 = 0;
    
    if (pScrollData->byPenDown == PENDOWN_THUMB)
        nTempPos = pScrollData->nTrackPos;
    else
        nTempPos = pScrollData->nPos;
    
    if (nTempPos == oldPos && 
        (int)pScrollData->ThumbRange == oldThumbRange)//not moved.
    {
        return;
    }

    if ((oldThumbRange && !pScrollData->ThumbRange) ||
        pScrollData->bDisabled || pScrollData->ThumbRange < MINTHUMBRANGE)
    {
        int curPos, curThumbRange;
        
        curPos = pScrollData->nPos;
        curThumbRange = pScrollData->ThumbRange;
        pScrollData->nPos = oldPos;
        pScrollData->ThumbRange = oldThumbRange;
        
        EraseScrollThumb(hdc, pScrollData, pRect);
        pScrollData->nPos = curPos;
        pScrollData->ThumbRange = curThumbRange;
        return;
    }
    
    if (pScrollData->ThumbRange && !oldThumbRange)
    {
        PaintScrollThumb(hdc, pScrollData, pRect);
        return;
    }

    if (pScrollData->byVScroll & SBS_VERT)
    {
        nLeftTop = pRect->top;
        nRightBottom = pRect->bottom;
    }
    else
    {
        nLeftTop = pRect->left;
        nRightBottom = pRect->right;
    }

    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

#if (INTERFACE_MONO)
    nEdge = MONO_GAP;
#endif // INTERFACE_MONO

    if (nTempPos > oldPos)//Move downwards.
    {
        if (nTempPos + pScrollData->ThumbRange > 
            (WORD)(oldPos + oldThumbRange))
        {
            //滚动后露出的区域的起始点，需要把它刷成细网筛。
            nRePaintStart = oldPos + nLeftTop;
            nRePaintEnd = nTempPos + nLeftTop;

            //Scroll thumb 新滚出的部分.
            nRePaintStart1 = oldPos + oldThumbRange + nLeftTop;
            nRePaintEnd1 = nTempPos + pScrollData->ThumbRange + nLeftTop;

            if (pScrollData->byVScroll & SBS_VERT)
            {
                SetRect(&rect1, pRect->left + MONO_GAP, pRect->top + nTempPos, 
                    pRect->right - MONO_GAP, 
                    pRect->top + nTempPos + pScrollData->ThumbRange);                
            }
            else
            {
                SetRect(&rect1, pRect->left + nTempPos, pRect->top + MONO_GAP, 
                    pRect->left + nTempPos + pScrollData->ThumbRange, 
                    pRect->bottom - MONO_GAP);
            }

#if (INTERFACE_MONO)
            if (pScrollData->byVScroll & SBS_VERT)
            {    
                StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
                    rect1.bottom - rect1.top, (HDC)hbmpMiddle, 0, 0, THUMB_WIDTH, THUMB_LENGTH, SRCCOPY);
            }
            else
            {
                StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
                    rect1.bottom - rect1.top, (HDC)hBmpHorzMidd, 0, 0, 118, 23, SRCCOPY);
            }
#endif // INTERFACE_MONO

            nRePaintStart1 = 0;
            nRePaintEnd1 = 0;
        }
        else
        {
            // 滚动后露出的区域的起始点，需要把它刷成细网筛。
            nRePaintStart = oldPos + nLeftTop;
            nRePaintEnd = nTempPos + nLeftTop;
            nRePaintStart1 = nTempPos + pScrollData->ThumbRange + nLeftTop;
            nRePaintEnd1 = oldPos + oldThumbRange + nLeftTop;

        }
    }
    else//nTempPos <= oldPos, Move upwards.
    {
        if (nTempPos + pScrollData->ThumbRange > (WORD)(oldPos + oldThumbRange))
        {
            //Scroll thumb 新滚出的部分.
            nRePaintStart = nTempPos + nLeftTop;
            nRePaintEnd = oldPos + nLeftTop;
            nRePaintStart1 = oldPos + oldThumbRange + nLeftTop;
            nRePaintEnd1 = nTempPos + pScrollData->ThumbRange + nLeftTop;
        
            if (pScrollData->byVScroll & SBS_VERT)
            {
                SetRect(&rect1, pRect->left + MONO_GAP, pRect->top + nTempPos, 
                    pRect->right - MONO_GAP, 
                    pRect->top + nTempPos + pScrollData->ThumbRange);                
            }
            else
            {
                SetRect(&rect1, pRect->left + nTempPos, pRect->top + MONO_GAP, 
                    pRect->left + nTempPos + pScrollData->ThumbRange, 
                    pRect->bottom - MONO_GAP);
            }
            
#if (INTERFACE_MONO)
            if (pScrollData->byVScroll & SBS_VERT)
            {    
                StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
                    rect1.bottom - rect1.top, (HDC)hbmpMiddle, 0, 0, THUMB_WIDTH, THUMB_LENGTH, SRCCOPY);
            }
            else
            {
                StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
                    rect1.bottom - rect1.top, (HDC)hBmpHorzMidd, 0, 0, 118, 23, SRCCOPY);
            }            
#endif // INTERFACE_MONO

            if (pScrollData->byVScroll & SBS_VERT)
            {
                SetRect(&rect1, pRect->left + MONO_GAP, pRect->top + nTempPos, 
                    pRect->right - MONO_GAP, 
                    pRect->top + nTempPos + pScrollData->ThumbRange);                
            }
            else
            {
                SetRect(&rect1, pRect->left + nTempPos, pRect->top + MONO_GAP, 
                    pRect->left + nTempPos + pScrollData->ThumbRange, 
                    pRect->bottom - MONO_GAP);
            }
#if (INTERFACE_MONO)
            if (pScrollData->byVScroll & SBS_VERT)
            {    
                StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
                    rect1.bottom - rect1.top, (HDC)hbmpMiddle, 0, 0, THUMB_WIDTH, THUMB_LENGTH, SRCCOPY);
            }
            else
            {
                StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
                    rect1.bottom - rect1.top, (HDC)hBmpHorzMidd, 0, 0, 118, 23, SRCCOPY);
            }            
#endif // INTERFACE_MONO

            //滚动后露出的区域的起始点，需要把它刷成细网筛。
            nRePaintStart = nLeftTop;
            nRePaintEnd = nLeftTop;
            nRePaintStart1 = 0;
            nRePaintEnd1 = 0;
        }
        else
        {
            //Scroll thumb 新滚出的部分.
            nRePaintStart = nTempPos + nLeftTop;
            nRePaintEnd = oldPos + nLeftTop;
            nRePaintStart1 = oldPos + oldThumbRange + nLeftTop;
            nRePaintEnd1 = nTempPos + pScrollData->ThumbRange + nLeftTop;
        
            if (pScrollData->byVScroll & SBS_VERT)
            {
                SetRect(&rect1, pRect->left + MONO_GAP, pRect->top + nTempPos, 
                    pRect->right - MONO_GAP, 
                    pRect->top + nTempPos + pScrollData->ThumbRange);                
            }
            else
            {
                SetRect(&rect1, pRect->left + nTempPos, pRect->top + MONO_GAP, 
                    pRect->left + nTempPos + pScrollData->ThumbRange, 
                    pRect->bottom - MONO_GAP);
            }
            
#if (INTERFACE_MONO)
            if (pScrollData->byVScroll & SBS_VERT)
            {    
                StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
                    rect1.bottom - rect1.top, (HDC)hbmpMiddle, 0, 0, THUMB_WIDTH, THUMB_LENGTH, SRCCOPY);
            }
            else
            {
                StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
                    rect1.bottom - rect1.top, (HDC)hBmpHorzMidd, 0, 0, 118, 23, SRCCOPY);
            }            
#endif // INTERFACE_MONO

            //滚动后露出的区域的起始点，需要把它刷成细网筛。
            nRePaintStart = nLeftTop + nTempPos + pScrollData->ThumbRange;
            nRePaintEnd = nLeftTop + oldPos + oldThumbRange;
            nRePaintStart1 = 0;
            nRePaintEnd1 = 0;
        }
    }

    // PDA的风格与STB的风格不同.
    if (pScrollData->byVScroll & SBS_VERT)
    {
#if (INTERFACE_MONO)

        SetRect(&rect1, pRect->left, nRePaintStart, 
            pRect->right, nRePaintEnd);
        
        if (nRePaintStart1 < nRePaintEnd1)
        {
            SetRect(&rect2, pRect->left, nRePaintStart1, 
                pRect->right, nRePaintEnd1);
        }

#endif // INTERFACE_MONO
    }
    else
    {
#if (INTERFACE_MONO)

        SetRect(&rect1, nRePaintStart, pRect->top, 
            nRePaintEnd, pRect->bottom);
        
        if (nRePaintStart1 < nRePaintEnd1)
        {
            SetRect(&rect2, nRePaintStart1, pRect->top, 
                nRePaintEnd1, pRect->bottom);
        }

#endif // INTERFACE_MONO
    }

    if (nRePaintStart < nRePaintEnd)
    {
        if (pScrollData->byVScroll & SBS_VERT)
        {    
            StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left,
                rect1.bottom - rect1.top, (HDC)hbmpbk, 0, 0, 23, 207, SRCCOPY);
        }
        else
        {    
            StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left,
                rect1.bottom - rect1.top, (HDC)hbmpHorzBk, 0, 0, 207, 23, SRCCOPY);
        }
    }
    if (nRePaintStart1 < nRePaintEnd1)
    {
        if (pScrollData->byVScroll & SBS_VERT)
        {        
		    StretchBlt(hdc, rect2.left, rect2.top, rect2.right - rect2.left,
                rect2.bottom - rect2.top, (HDC)hbmpbk, 0, 0, 23, 207, SRCCOPY);
        }
        else
        {        
            StretchBlt(hdc, rect2.left, rect2.top, rect2.right - rect2.left,
                rect2.bottom - rect2.top, (HDC)hbmpHorzBk, 0, 0, 207, 23, SRCCOPY);
        }
    }
}

void EraseScrollThumb(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect)
{
    RECT rect1;
    int nBitsPixel, nTempPos;

    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);
    
    if (pScrollData->byPenDown == PENDOWN_THUMB)
        nTempPos = pScrollData->nTrackPos;
    else
        nTempPos = pScrollData->nPos;
    
    // PDA的风格与STB的风格不同.
    if (pScrollData->byVScroll & SBS_VERT)
    {
#if (INTERFACE_MONO)

        SetRect(&rect1, pRect->left + MONO_GAP, pRect->top + nTempPos, 
            pRect->right - MONO_GAP, 
            pRect->top + nTempPos + pScrollData->ThumbRange);

#endif // INTERFACE_MONO
    }
    else
    {
#if (INTERFACE_MONO)

        SetRect(&rect1, pRect->left + nTempPos, pRect->top + MONO_GAP, 
            pRect->left + nTempPos + pScrollData->ThumbRange, 
            pRect->bottom - MONO_GAP);

#endif // INTERFACE_MONO
    }

    if (pScrollData->byVScroll & SBS_VERT)
    {    
        StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
            rect1.bottom - rect1.top, (HDC)hbmpMiddle, 0, 0, THUMB_WIDTH, THUMB_LENGTH, SRCCOPY);
    }
    else
    {
        StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left, 
            rect1.bottom - rect1.top, (HDC)hBmpHorzMidd, 0, 0, 118, 23, SRCCOPY);
    }    
}

#if (!INTERFACE_MONO)
static void Draw3DScrollRect(HDC hdc, const RECT* pRect, int nBitsPixel, 
                             BOOL bFillRect, BOOL bDown)
{
    HPEN hOldPen;
    RECT rc;

    if (!pRect)
        return;

    if (bDown && (nBitsPixel != 1))
    {
        RECT    rc;
		
        FrameRect(hdc, pRect, GetStockObject(GRAY_BRUSH));

        SetRect(&rc, pRect->left + 1, pRect->top + 1, pRect->right - 1, 
            pRect->bottom - 1);
		FrameRect(hdc, &rc, GetStockObject(LTGRAY_BRUSH));

        return;
    }

    // Draw left and top black line

    if (nBitsPixel != 2)
        hOldPen = SelectObject(hdc, GetStockObject(WTGRAY_PEN));
    else
        hOldPen = SelectObject(hdc, GetStockObject(GRAY_PEN));

    DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom - 1);

    DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, pRect->top);

    // Draw left and top white line or darkgray line

    SelectObject(hdc, GetStockObject(WHITE_PEN));

    DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,
        pRect->bottom - 2);

    DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right - 2, 
        pRect->top + 1);

    // Draw right and bottom gray line

    SelectObject(hdc, GetStockObject(GRAY_PEN));

    DrawLine(hdc, pRect->right - 2, pRect->top + 1, pRect->right - 2,
        pRect->bottom - 2);

    DrawLine(hdc, pRect->left + 1, pRect->bottom - 2, pRect->right - 1,
        pRect->bottom - 2);

    SelectObject(hdc, GetStockObject(BLACK_PEN));

    DrawLine(hdc, pRect->right - 1, pRect->top, pRect->right - 1,
        pRect->bottom - 1);

    DrawLine(hdc, pRect->left, pRect->bottom - 1, pRect->right,
        pRect->bottom - 1);

    //draw button face
    if (bFillRect)
    {
        SetRect(&rc, pRect->left + 2, pRect->top + 2, pRect->right - 2,
            pRect->bottom - 2);
        ClearRect(hdc, &rc, GetSysColor(COLOR_BTNFACE));
    }

    //Restore the old pen
    SelectObject(hdc, hOldPen);
}
#endif

void SB_Paint(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect)
{
    int nCYVScroll;

    // Not scroll control, when it's mono, scroll thumb region is narrow, 
    // need erase the whole region. 
//    if (pScrollData->byVScroll & SBS_NCSCROLL)
//    {
//#if (INTERFACE_MONO)
//        ClearRect(hdc, pRect, COLOR_WHITE);
//#endif // INTERFACE
//    }

    nCYVScroll = GetSystemMetrics(SM_CYVSCROLL);

    //Caculate the arrow size.
    if (pScrollData->byVScroll & SBS_VERT)
    {
        if (pRect->bottom - pRect->top < 2 * nCYVScroll)
        {
            pScrollData->nCYVScroll = (pRect->bottom - pRect->top) / 2;
            
            if (pScrollData->nCYVScroll < 0)
                pScrollData->nCYVScroll = 0;
        }
        else
            pScrollData->nCYVScroll = nCYVScroll;
    }
    else
    {
        if (pRect->right - pRect->left < 2 * nCYVScroll)
        {
            pScrollData->nCYVScroll = (pRect->right - pRect->left) / 2;
        
            if (pScrollData->nCYVScroll < 0)
                pScrollData->nCYVScroll = 0;
        }
        else
            pScrollData->nCYVScroll = nCYVScroll;
    }

	PaintArrow(hdc, pScrollData, pRect, TRUE, TRUE);
    PaintBKGND(hdc, pScrollData, pRect);
    
    PaintScrollThumb(hdc, pScrollData, pRect);
}

//Draw arrow.
static void PaintArrow(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect,
                   BOOL bLTArrow, BOOL bRBArrow)
{
    RECT        rect1;
    COLORREF    crBkOld, crTextOld;
    BITMAP      bitmap;
    int         nArrowLeft, nArrowTop, nBitsPixel;
    int         n3DBorder, nSysArrowSize, nBmpHeight, nBmpWidth;

    if (!pRect)
        return;
    
    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);
    
    crTextOld = SetTextColor(hdc, COLOR_BLACK);

#if(INTERFACE_MONO)
    crBkOld = SetBkColor(hdc, COLOR_WHITE);
    n3DBorder = 0;
#endif // INTERFACE_MONO

    nSysArrowSize = GetSystemMetrics(SM_CYVSCROLL);

    if (pScrollData->byVScroll & SBS_VERT)
    {
        GetObject((HGDIOBJ)hbmpUpArrow, sizeof(BITMAP), &bitmap);
        nBmpHeight = bitmap.bmHeight;
        nBmpWidth = bitmap.bmWidth;

        if (pScrollData->nCYVScroll < nSysArrowSize)
        {
            if (nBmpHeight > pScrollData->nCYVScroll - 2 * n3DBorder - 2)
                nBmpHeight = pScrollData->nCYVScroll - 2 * n3DBorder - 2;
        
            if (nBmpHeight < 0)
                nBmpHeight = 0;
        }

        if (bLTArrow)
        {
            SetRect(&rect1, pRect->left, pRect->top, 
                pRect->right, pRect->top + pScrollData->nCYVScroll);

#if (!INTERFACE_MONO)
            if (nBitsPixel != 1)//Not mono.
            {
                Draw3DScrollRect(hdc, &rect1, nBitsPixel, TRUE, FALSE);
                InflateRect(&rect1, -2, -2);
            }
#endif // INTERFACE_MONO
            
            nArrowLeft = rect1.left +
                (rect1.right - rect1.left - nBmpWidth) / 2;
            
            nArrowTop = rect1.top +
                (rect1.bottom - rect1.top - nBmpHeight) / 2; 
            
#if (INTERFACE_MONO)
            if (!(pScrollData->bLTArrowDisabled || pScrollData->bDisabled))
            {
                /*HPEN hOldPen;
                hOldPen = SelectObject(hdc, GetStockObject(LTGRAY_PEN));
                DrawRect(hdc, &rect1);
                SelectObject(hdc, hOldPen);*/
                BitBlt(hdc, nArrowLeft, nArrowTop, nBmpWidth, nBmpHeight, 
                    (HDC)hbmpUpArrow, 0, 0, ROP_SRC);
				
//                if (pScrollData->byPenDown == PENDOWN_LINEUP)
//                {
//                    RECT rc;
//                    rc.left = nArrowLeft - 1;
//                    rc.right = nArrowLeft + nBmpWidth + 1;
//                    rc.top = nArrowTop - 2;
//                    rc.bottom = nArrowTop + nBmpHeight + 2;
//                    InvertRect(hdc, &rc);
//                }

            }
            else
            {
                BitBlt(hdc, nArrowLeft, nArrowTop, nBmpWidth, nBmpHeight, 
                    (HDC)hbmpUpArrowDisabled, 0, 0, ROP_SRC);				
            }
#endif // INTERFACE_MONO
        }

        if (bRBArrow)
        {
            SetRect(&rect1, pRect->left, pRect->bottom - pScrollData->nCYVScroll, 
                pRect->right, pRect->bottom);

#if (!INTERFACE_MONO)
            if (nBitsPixel != 1)//Not mono.
            {
                Draw3DScrollRect(hdc, &rect1, nBitsPixel, TRUE, FALSE);
                InflateRect(&rect1, -2, -2);
            }
#endif // INTERFACE_MONO
            
            nArrowLeft = rect1.left + 
                (rect1.right - rect1.left - nBmpWidth) / 2;
            
            nArrowTop = rect1.top + 
                (rect1.bottom - rect1.top - nBmpHeight) / 2; 
            
#if (INTERFACE_MONO)
            if (!(pScrollData->bRBArrowDisabled || pScrollData->bDisabled))
            {
                /*HPEN hOldPen;
                hOldPen = SelectObject(hdc, GetStockObject(LTGRAY_PEN));
                DrawRect(hdc, &rect1);
                SelectObject(hdc, hOldPen);*/
				BitBlt(hdc, nArrowLeft, nArrowTop, nBmpWidth, nBmpHeight, 
                    (HDC)hbmpDownArrow, 0, 0, ROP_SRC);
            }
            else
            {
				BitBlt(hdc, nArrowLeft, nArrowTop, nBmpWidth, nBmpHeight, 
                    (HDC)hbmpDownArrowDisabled, 0, 0, ROP_SRC);
            }
#endif // INTERFACE_MONO
        }
    }
    else    // SBS_HORZ
    {
        GetObject((HGDIOBJ)hbmpLeftArrow, sizeof(BITMAP), &bitmap);
        nBmpHeight = bitmap.bmHeight;
        nBmpWidth = bitmap.bmWidth;

        if (pScrollData->nCYVScroll < nSysArrowSize)
        {
            if (nBmpWidth > pScrollData->nCYVScroll - 2 * n3DBorder - 2)
                nBmpWidth = pScrollData->nCYVScroll - 2 * n3DBorder - 2;
        
            if (nBmpWidth < 0)
                nBmpWidth = 0;
        }
        
        if (bLTArrow)
        {
            SetRect(&rect1, pRect->left, pRect->top, 
                pRect->left + pScrollData->nCYVScroll, pRect->bottom);

#if (!INTERFACE_MONO)
            if (nBitsPixel != 1)//Not mono.
            {
                Draw3DScrollRect(hdc, &rect1, nBitsPixel, TRUE, FALSE);
                InflateRect(&rect1, -2, -2);
            }
#endif // INTERFACE_MONO
            
            nArrowLeft = rect1.left + 
                (rect1.right - rect1.left - nBmpWidth) / 2;
            
            nArrowTop = rect1.top + 
                (rect1.bottom - rect1.top - nBmpHeight) / 2; 

#if (INTERFACE_MONO)
            if (!(pScrollData->bLTArrowDisabled || pScrollData->bDisabled))
            {
                BitBlt(hdc, nArrowLeft, nArrowTop, nBmpWidth, nBmpHeight, 
                    (HDC)hbmpLeftArrow, 0, 0, ROP_SRC);
            }
            else
            {
                BitBlt(hdc, nArrowLeft, nArrowTop, nBmpWidth, nBmpHeight, 
                    (HDC)hbmpLeftArrowDisabled, 0, 0, ROP_SRC);
            }

#endif // INTERFACE_MONO
        }

        if (bRBArrow)
        {
            SetRect(&rect1, pRect->right - pScrollData->nCYVScroll, 
                pRect->top, pRect->right, pRect->bottom);

#if (!INTERFACE_MONO)
            if (nBitsPixel != 1)//Not mono.
            {
                Draw3DScrollRect(hdc, &rect1, nBitsPixel, TRUE, FALSE);
                InflateRect(&rect1, -2, -2);
            }
#endif // INTERFACE_MONO
            
            nArrowLeft = rect1.left + 
                (rect1.right - rect1.left - nBmpWidth) / 2;
            
            nArrowTop = rect1.top + 
                (rect1.bottom - rect1.top - nBmpHeight) / 2; 

#if (INTERFACE_MONO)
            if (!(pScrollData->bRBArrowDisabled || pScrollData->bDisabled))
            {
                BitBlt(hdc, nArrowLeft, nArrowTop, nBmpWidth, nBmpHeight, 
                    (HDC)hbmpRightArrow, 0, 0, ROP_SRC);
            }
            else
            {
                BitBlt(hdc, nArrowLeft, nArrowTop, nBmpWidth, nBmpHeight, 
                    (HDC)hbmpRightArrowDisabled, 0, 0, ROP_SRC);
            }
#endif // INTERFACE_MONO
        }
    }

    SetBkColor(hdc, crBkOld);
    SetTextColor(hdc, crTextOld);
}

//Paint the hatched background.
static void PaintBKGND(HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect)
{
    RECT        rect1;
    int         nBitsPixel;

    if (!pRect)
        return;

    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

    //PDA的风格与STB的风格不同. RealColor时，WM_ERASEBKGND已经刷过底色。
    //而PDA的scroll thumb region窄一些，所以重画。
    if (pScrollData->byVScroll & SBS_VERT)
    {
#if (INTERFACE_MONO)

        SetRect(&rect1, pRect->left, 
            pRect->top + pScrollData->nCYVScroll, 
            pRect->right, 
            pRect->bottom - pScrollData->nCYVScroll);

#endif  // INTERFACE_MONO
    }
    else
    {
#if (INTERFACE_MONO)

        SetRect(&rect1, pRect->left + pScrollData->nCYVScroll, 
            pRect->top, 
            pRect->right - pScrollData->nCYVScroll, 
            pRect->bottom);


#endif // INTERFACE_MONO
    }

    //ClearRect(hdc, &rect1, COLOR_WHITE);
    if (pScrollData->bDisabled)
        StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left,
            rect1.bottom - rect1.top, (HDC)hbmpbkDisabled, 0, 0, 23, 209, SRCCOPY);
    else
    {
        if (pScrollData->byVScroll & SBS_VERT)
        {        
            StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left,
                rect1.bottom - rect1.top, (HDC)hbmpbk, 0, 0, 23, 207, SRCCOPY);
        }
        else
        {        
            StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left,
                rect1.bottom - rect1.top, (HDC)hbmpHorzBk, 0, 0, 207, 23, SRCCOPY);
        }
    }
}

int SB_Size(PSCROLLDATA pScrollData, int nMaxRange)
{
    SCROLLINFO  ScrollInfo;

    pScrollData->nMaxRange  = nMaxRange - 2 * pScrollData->nCYVScroll;

    if (pScrollData->nMaxRange < 0)
        pScrollData->nMaxRange = 0;

    // 只是为了重新计算nPos, nThumbPage, nPageRange.
    memset(&ScrollInfo, 0, sizeof(SCROLLINFO));
    SB_SetScrollInfo(NULL, pScrollData, NULL, FALSE, (LPARAM)&ScrollInfo);
    return 0;
}

void SB_Init(PSCROLLDATA pScrollData, DWORD wStyle, HWND hWndParent)
{
    memset(pScrollData, 0, sizeof(SCROLLDATA));
    
    pScrollData->nCYVScroll = GetSystemMetrics(SM_CYVSCROLL);
    
    // Initializes the internal data
    pScrollData->wLineRangeLogic = 1;
    pScrollData->nMaxRangeLogic = 100;
    pScrollData->nPageRangeLogic = 10;

    if (wStyle & SBS_NCSCROLL)
        pScrollData->byVScroll |= SBS_NCSCROLL;
    else if (wStyle & WS_CHILD)
        pScrollData->byVScroll |= SBS_CHILDWND;
    
    if (wStyle & SBS_VERT)
    {
        pScrollData->byVScroll |= SBS_VERT;
    }

    pScrollData->nPos = pScrollData->nCYVScroll;
    
    pScrollData->hwndParent = hWndParent;
}

void SB_PenProcess(HWND hWnd, HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect, 
                   UINT wMsgCmd, LPARAM lParam)
{
    IWORD   x, y;
    WORD    wPos;
    BOOL    fPosInThumb;
    int     oldThumbRange, oldPos, oldReDrawThumbRange = 0, nBitsPixel;
    HWND    hWndParent;
    DWORD   dwStyle;
    char    achClassName[32];
    
    x = LOWORD(lParam);
    y = HIWORD(lParam);
            
    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

    if (pScrollData->byVScroll & SBS_NCSCROLL)
    {
        hWndParent = hWnd;
    }
    else
    {
        if (pScrollData->byVScroll & SBS_CHILDWND)
            hWndParent = GetParent(hWnd);
        else
            hWndParent = GetWindow(hWnd, GW_OWNER);
    }

    GetClassName(hWndParent, achClassName, 32);
    achClassName[32 - 1] = 0;
    
    switch (wMsgCmd)
    {
    case WM_PENDOWN :

//#ifndef _EMULATE_
//        if (IsWindowEnabled(hWnd))
//            TouchAlert();
//#endif  /* _EMULATE_ */
        
        if (pScrollData->bDisabled)
            break;
        
        if (!PtInRectXY(pRect, x, y))
            break;

        if (!(pScrollData->byVScroll & SBS_NCSCROLL))
        {
            SetCapture(hWnd);
            SetFocus(hWnd);
        }

        pScrollData->byPenDown = PENDOWN;

        //drag the thumbbox
        if (pScrollData->byVScroll & SBS_VERT)
        {
            if (y >= pRect->top + pScrollData->nPos &&
                y < pRect->top + pScrollData->nPos + (int)pScrollData->ThumbRange)
            {
                pScrollData->byPenDown = PENDOWN_THUMB;
                pScrollData->ThumbToPosRange = y - (pScrollData->nPos + pRect->top);
                pScrollData->nOldPosLogic = pScrollData->nPosLogic;
                pScrollData->nTrackPos = pScrollData->nOldPos = pScrollData->nPos;
                break;
            }
        }
        else
        {
            if (x >= pRect->left + pScrollData->nPos &&
                x < pRect->left + pScrollData->nPos + (int)pScrollData->ThumbRange)
            {
                pScrollData->byPenDown = PENDOWN_THUMB;
                pScrollData->ThumbToPosRange = x - (pScrollData->nPos + pRect->left);
                pScrollData->nOldPosLogic = pScrollData->nPosLogic;
                pScrollData->nTrackPos = pScrollData->nOldPos = pScrollData->nPos;
                break;
            }
        }
        
        pScrollData->nOldPos = pScrollData->nPos;
        pScrollData->curPenX = x;
        pScrollData->curPenY = y;

        ScrollProcess(hWnd, hdc, pScrollData, x, y, pRect);

        if ((pScrollData->byPenDown == PENDOWN_LINEUP) || 
            (pScrollData->byPenDown == PENDOWN_LINEDOWN))
        {
            RECT    rc;

#if (INTERFACE_MONO)
#else // INTERFACE_MONO
            if ((nBitsPixel != 1) && (nBitsPixel != 2))
            {
                pScrollData->uTimerID = SetSystemTimer(hWnd, 1, 200, NULL);
                pScrollData->bTimerIn = TRUE;
            }
#endif // INTERFACE_MONO
            
            if (pScrollData->byPenDown == PENDOWN_LINEUP)
            {
                if (!pScrollData->bLTArrowDisabled)
                {
                    GetArrowRect(pScrollData, pRect, PENDOWN_LINEUP, &rc);

#if (INTERFACE_MONO)
                    // InvertRect(hdc, &rc);
#else // INTERFACE_MONO
                    if (nBitsPixel != 1)
                        Draw3DScrollRect(hdc, &rc, nBitsPixel, FALSE, TRUE);
#endif // INTERFACE_MONO
                }
            }
            else
            {
                if (!pScrollData->bRBArrowDisabled)
                {
                    GetArrowRect(pScrollData, pRect, PENDOWN_LINEDOWN, &rc);

#if (INTERFACE_MONO)
                    // InvertRect(hdc, &rc);
#else // INTERFACE_MONO
                    if (nBitsPixel != 1)
                        Draw3DScrollRect(hdc, &rc, nBitsPixel, FALSE, TRUE);
#endif // INTERFACE_MONO
                }
            }
        }
        else if (pScrollData->byPenDown == PENDOWN_PAGE)
        {
#if (INTERFACE_MONO)
#else // INTERFACE_MONO
            if ((nBitsPixel != 1) && (nBitsPixel != 2))
            {
                pScrollData->uTimerID = SetSystemTimer(hWnd, 1, 200, NULL);
                pScrollData->bTimerIn = TRUE;
            }
#endif // INTERFACE_MONO
        }

        break;
        
    case WM_PENUP :

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        
        //Item list of combobox is a popup listbox window.        
        if (dwStyle & LBS_COMBOLBOX)
            SetCapture(pScrollData->hwndParent);

        if (pScrollData->bDisabled)
            break;

        if (pScrollData->byPenDown == NOPENDOWN)
            break;
        
        if (!(pScrollData->byVScroll & SBS_NCSCROLL))//is scroll control.
            ReleaseCapture();

        if ((pScrollData->byPenDown == PENDOWN_LINEUP) || 
            (pScrollData->byPenDown == PENDOWN_LINEDOWN))
        {
            RECT    rc;
            
            if (pScrollData->byPenDown == PENDOWN_LINEUP)
            {
                if (!pScrollData->bLTArrowDisabled)
                {
                    GetArrowRect(pScrollData, pRect, PENDOWN_LINEUP, &rc);

#if (INTERFACE_MONO)
                    // InvertRect(hdc, &rc);
#else // INTERFACE_MONO
                    if (nBitsPixel != 1)
                        Draw3DScrollRect(hdc, &rc, nBitsPixel, FALSE, FALSE);
#endif // INTERFACE_MONO
                }
            }
            else
            {
                if (!pScrollData->bRBArrowDisabled)
                {
                    GetArrowRect(pScrollData, pRect, PENDOWN_LINEDOWN, &rc);

#if (INTERFACE_MONO)
                    // InvertRect(hdc, &rc);
#else // INTERFACE_MONO
                    if (nBitsPixel != 1)
                        Draw3DScrollRect(hdc, &rc, nBitsPixel, FALSE, FALSE);
#endif // INTERFACE_MONO
                }
            }
        }

        if (pScrollData->uTimerID)
        {
            KillSystemTimer(hWnd, pScrollData->uTimerID);
            pScrollData->nTimerCount = 0;
            pScrollData->uTimerID = 0;
            pScrollData->byPenDown = NOPENDOWN;
            break;
        }
        
        if (pScrollData->byPenDown != PENDOWN_THUMB) 
        {
            pScrollData->byPenDown = NOPENDOWN;
            break;
        }
        
        pScrollData->byPenDown = NOPENDOWN;

        //If exceed the certain range, will restore the original position.
        if (pScrollData->byVScroll & SBS_VERT)
        {
            if ( x < (pRect->left - (pRect->right - pRect->left)) ||
                x > (pRect->right + (pRect->right - pRect->left)) ||
                y < (pRect->top - 2 * pScrollData->nCYVScroll) ||
                y > (pRect->bottom + 2 * pScrollData->nCYVScroll))
            {
                break;
            }
        }
        else
        {
            if ( y < (pRect->top - (pRect->bottom - pRect->top)) ||
                y > (pRect->bottom + (pRect->bottom - pRect->top)) ||
                x < (pRect->left - 2 * pScrollData->nCYVScroll) ||
                x > (pRect->right + 2 * pScrollData->nCYVScroll))
            {
                break;
            }
        }

        //process thumb box repaint.
        fPosInThumb = FALSE;

        oldReDrawThumbRange = pScrollData->ThumbRange;
        oldPos = pScrollData->nTrackPos;

        //old thumb box size.
        oldThumbRange = pScrollData->nPageRangeLogic * pScrollData->nMaxRange
                              / pScrollData->nMaxRangeLogic;
        
        if (pScrollData->byVScroll & SBS_VERT)
        {
            //Thumb box position. 
            y = y - pScrollData->ThumbToPosRange;

            if (y < pRect->top + pScrollData->nCYVScroll)
                pScrollData->nTrackPos = pScrollData->nCYVScroll;
            else if (y > (pRect->bottom - pScrollData->nCYVScroll - 
                (int)pScrollData->ThumbRange))
            {
                pScrollData->nTrackPos = pScrollData->nMaxRange + 
                    pScrollData->nCYVScroll - oldThumbRange;
            }
            else if (y <= pRect->top + pScrollData->nTrackPos || 
                y > pRect->top + pScrollData->nTrackPos )//+ (int)pScrollData->ThumbRange)
                // + pScrollData->ThumbRange))
            {
                pScrollData->nTrackPos = y - pRect->top;
            }
            else
                fPosInThumb = TRUE;//position not change.
        }
        else
        {
            x = x - pScrollData->ThumbToPosRange;

            if (x < pRect->left + pScrollData->nCYVScroll)
                pScrollData->nTrackPos = pScrollData->nCYVScroll;
            else if (x > (pRect->right - pScrollData->nCYVScroll - 
                (int)pScrollData->ThumbRange))
            {
                pScrollData->nTrackPos = pScrollData->nMaxRange + 
                    pScrollData->nCYVScroll - oldThumbRange;
            }
            else if (x <= pRect->left + pScrollData->nTrackPos || 
                x > pRect->left + pScrollData->nTrackPos)
                // + pScrollData->ThumbRange)) 
            {
                pScrollData->nTrackPos = x;
            }
            else
                fPosInThumb = TRUE;
        }
        
        if (!fPosInThumb)
        {
            if (pScrollData->byVScroll & SBS_VERT)
            {
                wPos = (pScrollData->nTrackPos - pScrollData->nCYVScroll) * 
                    pScrollData->nMaxRangeLogic /
                    (pRect->bottom - pRect->top - 2 * pScrollData->nCYVScroll + 
                    oldThumbRange - pScrollData->ThumbRange);
                
                if (wPos > pScrollData->nMaxRangeLogic - pScrollData->nPageRangeLogic)
                    wPos = pScrollData->nMaxRangeLogic - pScrollData->nPageRangeLogic;
                
                pScrollData->nTrackPosLogic = wPos;
                
                if (pScrollData->nMaxRangeLogic - pScrollData->nTrackPosLogic <= 
                    (int)pScrollData->nPageRangeLogic)
                    //For the last page
                {
                    pScrollData->nTrackPos = pScrollData->nMaxRange + 
                        pScrollData->nCYVScroll - pScrollData->ThumbRange;
                }
                else
                {
                    //为避免误差，把位置pScrollData->nPos换算回去。
                    pScrollData->nTrackPos = pScrollData->nCYVScroll + wPos * 
                                    (pRect->bottom - pRect->top - 
                                    2 * pScrollData->nCYVScroll + 
                                    oldThumbRange - pScrollData->ThumbRange)
                                    / pScrollData->nMaxRangeLogic;
                    // + oldThumbRange - pScrollData->ThumbRange;
                }
            }
            else
            {
                wPos = (pScrollData->nTrackPos - pScrollData->nCYVScroll) * 
                    pScrollData->nMaxRangeLogic /
                    (pRect->right - pRect->left - 2 * pScrollData->nCYVScroll + 
                    oldThumbRange - pScrollData->ThumbRange);
                
                if (wPos > pScrollData->nMaxRangeLogic - pScrollData->nPageRangeLogic)
                    wPos = pScrollData->nMaxRangeLogic - pScrollData->nPageRangeLogic;

                pScrollData->nTrackPosLogic = wPos;

                if (pScrollData->nMaxRangeLogic - pScrollData->nTrackPosLogic <= 
                    (int)pScrollData->nPageRangeLogic)
                    //For the last page
                {
                    pScrollData->nTrackPos = pScrollData->nMaxRange + 
                        pScrollData->nCYVScroll - pScrollData->ThumbRange;
                }
                else
                {
                    pScrollData->nTrackPos = pScrollData->nCYVScroll + wPos * 
                                    (pRect->right - pRect->left - 
                                    2 * pScrollData->nCYVScroll + 
                                    oldThumbRange - pScrollData->ThumbRange)
                                    / pScrollData->nMaxRangeLogic ;
                }
            }
            
            if (pScrollData->byVScroll & SBS_VERT)
            {
                SendMessage(hWndParent, WM_VSCROLL, 
                    (WPARAM)MAKELONG(SB_THUMBPOSITION, wPos + pScrollData->nMinRangeLogic), 
                    (LPARAM)hWnd);
                SendMessage(hWndParent, WM_VSCROLL, 
                    (WPARAM)MAKELONG(SB_THUMBTRACK, wPos + pScrollData->nMinRangeLogic), 
                    (LPARAM)hWnd);
            }
            else
            {
                SendMessage(hWndParent, WM_HSCROLL, 
                    (WPARAM)MAKELONG(SB_THUMBPOSITION, wPos + pScrollData->nMinRangeLogic), 
                    (LPARAM)hWnd);
                SendMessage(hWndParent, WM_HSCROLL, 
                    (WPARAM)MAKELONG(SB_THUMBTRACK, wPos + pScrollData->nMinRangeLogic), 
                    (LPARAM)hWnd);
            }
            
            //PENUP时，按用户响应SB_THUMBTRACK or SB_THUMBPOS所SetScrollInfo的值去画, 
            //否则恢复PENDOWN时的状态。
            if (pScrollData->nTrackPos != pScrollData->nPos)
            {
                pScrollData->nTrackPos = pScrollData->nPos = pScrollData->nOldPos;
                pScrollData->nTrackPosLogic = pScrollData->nPosLogic = 
                    pScrollData->nOldPosLogic;
            }
                
            MoveScrollThumb(hdc, pScrollData, pRect, oldPos, oldReDrawThumbRange);
        }

        pScrollData->byPenDown = NOPENDOWN;
        break;

    case WM_PENMOVE :

        if (pScrollData->bDisabled || pScrollData->byPenDown == NOPENDOWN)
            break;

        //处理在Page or Line Down or Up 时,鼠标的移进或移出.

        //Line up or Line down        
        if ((pScrollData->byPenDown == PENDOWN_LINEUP) ||
            (pScrollData->byPenDown == PENDOWN_LINEDOWN))
        {

#if (INTERFACE_MONO)

            if (pScrollData->curPenY >= pRect->top && 
                pScrollData->curPenY <= pRect->bottom &&
                pScrollData->curPenX >= pRect->left &&
                pScrollData->curPenX <= (pRect->left  + 
                pScrollData->nCYVScroll))
            {
                if (pScrollData->uTimerID)
                {
                    //drag out of arrow region.
                    if (y < pRect->top ||
                        y > pRect->bottom ||
                        x < pRect->left ||
                        x > (pRect->left  + pScrollData->nCYVScroll))
                    {
                        if (pScrollData->bTimerIn)
                        {
                            RECT    rc;
                            
                            pScrollData->bTimerIn = FALSE;
                            
                            GetArrowRect(pScrollData, pRect, PENDOWN_LINEUP, &rc);
                        }
                    }
                    else
                    {
                        if (!pScrollData->bTimerIn)
                        {
                            RECT    rc;
                            
                            pScrollData->bTimerIn = TRUE;
                            
                            GetArrowRect(pScrollData, pRect, PENDOWN_LINEUP, &rc);
                        }
                    }
                }
                
                break;
            }
            
            if (pScrollData->curPenY >= pRect->top &&
                pScrollData->curPenY <= pRect->bottom &&
                pScrollData->curPenX >= (pRect->right - pScrollData->nCYVScroll) &&
                pScrollData->curPenX <= pRect->right)//down arrow.
            {
                if (pScrollData->uTimerID)
                {
                    if (y < pRect->top ||
                        y > pRect->bottom ||
                        x < (pRect->right - pScrollData->nCYVScroll) ||
                        x > pRect->right)
                    {
                        if (pScrollData->bTimerIn)
                        {
                            RECT    rc;
                            
                            pScrollData->bTimerIn = FALSE;
                            
                            GetArrowRect(pScrollData, pRect, PENDOWN_LINEDOWN, &rc);
                        }
                    }
                    else
                    {
                        if (!pScrollData->bTimerIn)
                        {
                            RECT    rc;
                            
                            pScrollData->bTimerIn = TRUE;
                            
                            GetArrowRect(pScrollData, pRect, PENDOWN_LINEDOWN, &rc);
                        }
                    }
                }
                
                break;
            }

#endif // INTERFACE_MONO

            break;
        }
        
        if (pScrollData->byPenDown == PENDOWN_PAGE)//Page up/down.
        {
#if (INTERFACE_MONO)
#else // INTERFACE_MONO
            if ((nBitsPixel != 1) && (nBitsPixel != 2))
            {
                if (pScrollData->byVScroll & SBS_VERT)
                {
                    if (pScrollData->nPos > pScrollData->nOldPos)//Page down.
                    {   
                        if (pScrollData->uTimerID)
                        {
                            if (x < pRect->left ||
                                x > pRect->right ||
                                y < (pRect->top + pScrollData->nPos + 
                                (int)pScrollData->ThumbRange) ||
                                y > (pScrollData->nMaxRange + pScrollData->nCYVScroll))
                            {
                                pScrollData->bTimerIn = FALSE;
                                break;
                            }
                        }
                        pScrollData->curPenX = x;
                        pScrollData->curPenY = y;
                        pScrollData->bTimerIn = TRUE;
                        break;
                    }
                    
                    if (pScrollData->nPos < pScrollData->nOldPos)//Page up.
                    {   
                        if (pScrollData->uTimerID)
                        {
                            if (x < pRect->left ||
                                x > pRect->right ||
                                y < (pRect->top + pScrollData->nCYVScroll) ||
                                y > (pRect->top + pScrollData->nPos))
                            {
                                pScrollData->bTimerIn = FALSE;
                                break;
                            }
                        }
                        pScrollData->curPenX = x;
                        pScrollData->curPenY = y;
                        pScrollData->bTimerIn = TRUE;
                        break;
                    }
                }
                else
                {
                    if (pScrollData->nPos > pScrollData->nOldPos)//Page down.
                    {   
                        if (pScrollData->uTimerID)
                        {
                            if (y < pRect->top ||
                                y > pRect->bottom ||
                                x < (pRect->left + pScrollData->nPos + 
                                (int)pScrollData->ThumbRange) ||
                                x > (pScrollData->nMaxRange + pScrollData->nCYVScroll))
                            {
                                pScrollData->bTimerIn = FALSE;
                                break;
                            }
                        }
                        pScrollData->curPenX = x;
                        pScrollData->curPenY = y;
                        pScrollData->bTimerIn = TRUE;
                        break;
                    }
                    
                    if (pScrollData->nPos < pScrollData->nOldPos)//Page up.
                    {   
                        if (pScrollData->uTimerID)
                        {
                            if (y < pRect->top ||
                                y > pRect->bottom ||
                                x < (pRect->left + pScrollData->nCYVScroll) ||
                                x > (pRect->left + pScrollData->nPos))
                            {
                                pScrollData->bTimerIn = FALSE;
                                break;
                            }
                        }
                        pScrollData->curPenX = x;
                        pScrollData->curPenY = y;
                        pScrollData->bTimerIn = TRUE;
                        break;
                    }
                }
            }

#endif // INTERFACE_MONO

            break;
        }
        
        //If exceed the certain range, will restore the original position.
        if (pScrollData->byVScroll & SBS_VERT)
        {
            if ( x < (pRect->left - (pRect->right - pRect->left)) ||
                x > (pRect->right + pRect->right - pRect->left) ||
                y < (pRect->top - 2 * pScrollData->nCYVScroll) ||
                y > (pRect->bottom + 2 * pScrollData->nCYVScroll))
            {
                if (pScrollData->nTrackPos == pScrollData->nOldPos)
                    break;

                EraseScrollThumb(hdc, pScrollData, pRect);

                pScrollData->nTrackPos = pScrollData->nOldPos;
                pScrollData->nTrackPosLogic = pScrollData->nOldPosLogic;

                PaintBKGND(hdc, pScrollData, pRect);
				PaintScrollThumb(hdc, pScrollData, pRect);

                /*SendMessage(hWndParent, WM_VSCROLL, 
                    (WPARAM)MAKELONG(SB_THUMBTRACK, 
                    pScrollData->nTrackPosLogic + pScrollData->nMinRangeLogic), 
                    (LPARAM)hWnd);*/
                break;
            }
        }
        else
        {
            if ( y < (pRect->top - (pRect->bottom - pRect->top)) ||
                y > (pRect->bottom + pRect->bottom - pRect->top) ||
                x < (pRect->left - 2 * pScrollData->nCYVScroll) ||
                x > (pRect->right + 2 * pScrollData->nCYVScroll))
            {
                if (pScrollData->nTrackPos == pScrollData->nOldPos)
                    break;

                EraseScrollThumb(hdc, pScrollData, pRect);

                pScrollData->nTrackPos = pScrollData->nOldPos;
                pScrollData->nTrackPosLogic = pScrollData->nOldPosLogic;

                
				PaintBKGND(hdc, pScrollData, pRect);
				PaintScrollThumb(hdc, pScrollData, pRect);

                /*SendMessage(hWndParent, WM_HSCROLL, 
                    (WPARAM)MAKELONG(SB_THUMBTRACK, 
                    pScrollData->nTrackPosLogic + pScrollData->nMinRangeLogic), 
                    (LPARAM)hWnd);*/

                break;
            }
        }

        //在Page Down/Up的过程中, 是否不再是Down Or Up?
        fPosInThumb = FALSE;
        
        oldReDrawThumbRange = pScrollData->ThumbRange;
        oldPos = pScrollData->nTrackPos;

        oldThumbRange = pScrollData->nPageRangeLogic * pScrollData->nMaxRange
                              / pScrollData->nMaxRangeLogic ;

        if (pScrollData->byVScroll & SBS_VERT)
        {
            y = y - pScrollData->ThumbToPosRange;

            if (y < (pScrollData->nCYVScroll + pRect->top))
            {
                pScrollData->nTrackPos = pScrollData->nCYVScroll;
            }
            else if (y > (pRect->bottom - pScrollData->nCYVScroll - 
                     (int)pScrollData->ThumbRange))
            {
                pScrollData->nTrackPos = pScrollData->nMaxRange + 
                    pScrollData->nCYVScroll - oldThumbRange;
            }
            else if ((y < pRect->top + pScrollData->nTrackPos || 
                y > pRect->top + pScrollData->nTrackPos))
                // + pScrollData->ThumbRange))
            {
                pScrollData->nTrackPos = y - pRect->top;
            }
            else
                fPosInThumb = TRUE;
        }
        else
        {
            x = x - pScrollData->ThumbToPosRange;

            if (x < pScrollData->nCYVScroll + pRect->left)
            {
                pScrollData->nTrackPos = pScrollData->nCYVScroll;
            }
            else if (x > (pRect->right - pScrollData->nCYVScroll - 
                     (int)pScrollData->ThumbRange))
            {
                pScrollData->nTrackPos = pScrollData->nMaxRange + 
                    pScrollData->nCYVScroll - oldThumbRange;
            }
            else if ((x < pScrollData->nTrackPos + pRect->left || 
                x > pScrollData->nTrackPos + pRect->left))
                // + pScrollData->ThumbRange)) 
            {
                pScrollData->nTrackPos = x - pRect->left;
            }
            else
                fPosInThumb = TRUE;
        }
        
        if (!fPosInThumb)
        {
            if (pScrollData->byVScroll & SBS_VERT)
            {
                wPos = (pScrollData->nTrackPos - pScrollData->nCYVScroll) * 
                    pScrollData->nMaxRangeLogic /
                    (pRect->bottom - pRect->top - 2 * pScrollData->nCYVScroll + 
                    oldThumbRange - pScrollData->ThumbRange);
                
                if (wPos > pScrollData->nMaxRangeLogic - pScrollData->nPageRangeLogic)
                    wPos = pScrollData->nMaxRangeLogic - pScrollData->nPageRangeLogic;
                
                pScrollData->nTrackPosLogic = wPos;

                if (pScrollData->nMaxRangeLogic - pScrollData->nTrackPosLogic <= 
                    (int)pScrollData->nPageRangeLogic)
                    //For the last page
                {
                    pScrollData->nTrackPos = pScrollData->nMaxRange + 
                        pScrollData->nCYVScroll - pScrollData->ThumbRange;
                }
                else
                {
                    //为避免误差，把位置pScrollData->nPos换算回去。
                    pScrollData->nTrackPos = pScrollData->nCYVScroll + wPos * 
                        (pRect->bottom - pRect->top - 
                        2 * pScrollData->nCYVScroll + 
                        oldThumbRange - pScrollData->ThumbRange)
                        / pScrollData->nMaxRangeLogic;
                    // + oldThumbRange - pScrollData->ThumbRange;
                }
            }
            else
            {
                wPos = (pScrollData->nTrackPos - pScrollData->nCYVScroll) * 
                    pScrollData->nMaxRangeLogic /
                    (pRect->right - pRect->left - 2 * pScrollData->nCYVScroll + 
                    oldThumbRange - pScrollData->ThumbRange);

                if (wPos > pScrollData->nMaxRangeLogic - pScrollData->nPageRangeLogic)
                    wPos = pScrollData->nMaxRangeLogic - pScrollData->nPageRangeLogic;
                
                pScrollData->nTrackPosLogic = wPos;

                if (pScrollData->nMaxRangeLogic - pScrollData->nTrackPosLogic <= 
                    (int)pScrollData->nPageRangeLogic)
                    //For the last page
                {
                    pScrollData->nTrackPos = pScrollData->nMaxRange + 
                        pScrollData->nCYVScroll - pScrollData->ThumbRange;
                }
                else
                    pScrollData->nTrackPos = pScrollData->nCYVScroll + wPos * 
                                    (pRect->right - pRect->left - 
                                    2 * pScrollData->nCYVScroll + 
                                    oldThumbRange - pScrollData->ThumbRange)
                                    / pScrollData->nMaxRangeLogic ;
            }

            MoveScrollThumb(hdc, pScrollData, pRect, oldPos, oldReDrawThumbRange);

            /*if (pScrollData->byVScroll & SBS_VERT)
            {
                SendMessage(hWndParent, WM_VSCROLL, 
                    (WPARAM)MAKELONG(SB_THUMBTRACK, wPos + pScrollData->nMinRangeLogic), 
                    (LPARAM)hWnd);
            }
            else
            {
                SendMessage(hWndParent, WM_HSCROLL, 
                    (WPARAM)MAKELONG(SB_THUMBTRACK, wPos + pScrollData->nMinRangeLogic), 
                    (LPARAM)hWnd);
            }*/
        }

        break;
    }
}

void SB_Timer(HWND hWnd, HDC hdc, PSCROLLDATA pScrollData, const RECT* pRect)
{

    if (pScrollData->bTimerIn)
    {
        if (pScrollData->nTimerCount < 1)
        {
            pScrollData->nTimerCount++;
        }
        else
        {
            ScrollProcess(hWnd, hdc, pScrollData, pScrollData->curPenX, 
                pScrollData->curPenY, pRect);
        }
    }
}

BOOL SB_GetScrollInfo(PSCROLLDATA pScrollData, LPARAM lParam)
{
    PSCROLLINFO pScrollInfo;

    pScrollInfo = (PSCROLLINFO)lParam;

    if (!pScrollInfo)
        return FALSE;
    
    if (pScrollInfo->fMask & SIF_PAGE)
        pScrollInfo->nPage = pScrollData->nPageRangeLogic;
    
    if (pScrollInfo->fMask & SIF_POS)
        pScrollInfo->nPos = pScrollData->nPosLogic + pScrollData->nMinRangeLogic;
    
    if (pScrollInfo->fMask & SIF_RANGE)
    {
        pScrollInfo->nMin = pScrollData->nMinRangeLogic;
        pScrollInfo->nMax = pScrollData->nMaxRangeLogic + pScrollData->nMinRangeLogic - 1;
    }
    
    pScrollInfo->nTrackPos = pScrollData->nTrackPosLogic + pScrollData->nMinRangeLogic;
    return TRUE;
}

BOOL SB_IsEnabled(PSCROLLDATA pScrollData, PCSCROLLINFO pScrollInfo)
{
    ASSERT(pScrollData != NULL);

    if (pScrollInfo)
    {
        int nMax, nPage;

        if (pScrollInfo->fMask & SIF_PAGE)
        {
            nPage = pScrollInfo->nPage;
            if (nPage < 0)
                nPage = 0;
        }
        else
            nPage = pScrollData->nPageRangeLogic;
        
        if (pScrollInfo->fMask & SIF_RANGE)
        {
            nMax = pScrollInfo->nMax - pScrollInfo->nMin + 1;
            if (nMax < 0)
                nMax = 0;
        }
        else
            nMax = pScrollData->nMaxRangeLogic;

        return nMax > nPage;
    }

    return !pScrollData->bDisabled;
}

int  SB_SetScrollInfo(HDC hdc, PSCROLLDATA pScrollData, 
                      const RECT* pRect, WPARAM wParam, LPARAM lParam)
{
    PSCROLLINFO pScrollInfo;
    int         oldThumbRange, oldPos, oldReDrawThumbRange = 0;
    int         nOldMinRangeLogic = pScrollData->nMinRangeLogic;
    BOOL        bDisabledOld;    
        
    bDisabledOld = pScrollData->bDisabled;
    
    pScrollInfo = (PSCROLLINFO)lParam;

    if (!pScrollInfo)
        return pScrollData->nPosLogic + nOldMinRangeLogic;

    if (pScrollInfo->fMask & SIF_PAGE)
    {
        pScrollData->nPageRangeLogic = pScrollInfo->nPage;

        if (pScrollData->nPageRangeLogic < 0)
            pScrollData->nPageRangeLogic = 0;
    }
    
    if (pScrollInfo->fMask & SIF_RANGE)
    {
        pScrollData->nMinRangeLogic  = pScrollInfo->nMin;
        pScrollData->nMaxRangeLogic  = pScrollInfo->nMax - pScrollInfo->nMin + 1;

        if (pScrollData->nMaxRangeLogic < 0)
            pScrollData->nMaxRangeLogic = 0;
    }
    
    if (pScrollInfo->fMask & SIF_POS)
    {
        pScrollData->nPosLogic = pScrollInfo->nPos - pScrollData->nMinRangeLogic;

        if (pScrollData->nPosLogic < pScrollData->nMinRangeLogic)
            pScrollData->nPosLogic = pScrollData->nMinRangeLogic;
    }


    if ((BOOL)wParam && pRect)
    {
        oldPos = pScrollData->nPos;
        oldReDrawThumbRange = pScrollData->ThumbRange;
    }

    if (pScrollData->byVScroll & SBS_VERT)
    {
        if (pScrollData->nMaxRangeLogic > (int)pScrollData->nPageRangeLogic)
        {
            pScrollData->ThumbRange = pScrollData->nPageRangeLogic * 
                pScrollData->nMaxRange / pScrollData->nMaxRangeLogic;
            pScrollData->bDisabled = FALSE;
        }
        else
        {
            pScrollData->ThumbRange = 0;
            pScrollData->bDisabled = TRUE;
        }
        
        oldThumbRange = pScrollData->ThumbRange;
        
        //Scroll thumb exist and is shorter than minimum value.
        if (pScrollData->nMaxRangeLogic > (int)pScrollData->nPageRangeLogic &&
            pScrollData->nPageRangeLogic > 0 && pScrollData->ThumbRange < MINTHUMBRANGE &&
            pScrollData->nMaxRange > MINTHUMBRANGE)
        {
            if (pScrollData->nMaxRange > MINTHUMBRANGE)
                pScrollData->ThumbRange = MINTHUMBRANGE;
        }
        
        if (pScrollData->nMaxRangeLogic)
        {
            pScrollData->nPageRange = pScrollData->nPageRangeLogic * 
                (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                / pScrollData->nMaxRangeLogic;
            
            pScrollData->wLineRange = pScrollData->wLineRangeLogic * 
                (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                / pScrollData->nMaxRangeLogic;
            
            if (pScrollData->wLineRange == 0)
                pScrollData->wLineRange = 1;
            
            if (pScrollData->nMaxRangeLogic - pScrollData->nPosLogic <= 
                (int)pScrollData->nPageRangeLogic)
                //For the last page
            {
                pScrollData->nPos = pScrollData->nMaxRange + pScrollData->nCYVScroll - 
                    pScrollData->ThumbRange;
            }
            else
            {
                pScrollData->nPos = pScrollData->nCYVScroll + 
                    pScrollData->nPosLogic * 
                    (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                    / pScrollData->nMaxRangeLogic;
            }
        }
        else
        {
            pScrollData->nPageRange = 0;
            pScrollData->wLineRange = 0;
            pScrollData->nPos = pScrollData->nCYVScroll;
        }
    }
    else//HORZ
    {
        if (pScrollData->nMaxRangeLogic > (int)pScrollData->nPageRangeLogic)
        {
            pScrollData->ThumbRange = pScrollData->nPageRangeLogic * 
                pScrollData->nMaxRange / pScrollData->nMaxRangeLogic ;
            pScrollData->bDisabled = FALSE;
        }
        else
        {
            pScrollData->ThumbRange = 0;
            pScrollData->bDisabled = TRUE;
        }
        
        oldThumbRange = pScrollData->ThumbRange;
        
        if (pScrollData->nMaxRangeLogic > (int)pScrollData->nPageRangeLogic &&
            pScrollData->nPageRangeLogic > 0 && pScrollData->ThumbRange < MINTHUMBRANGE &&
            pScrollData->nMaxRange > MINTHUMBRANGE)
        {
            pScrollData->ThumbRange = MINTHUMBRANGE;
        }
        
        if (pScrollData->nMaxRangeLogic)
        {
            pScrollData->nPageRange = pScrollData->nPageRangeLogic * 
                (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                / pScrollData->nMaxRangeLogic ;

            pScrollData->wLineRange = pScrollData->wLineRangeLogic * 
                (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                / pScrollData->nMaxRangeLogic ;

            if (pScrollData->wLineRange == 0)
                pScrollData->wLineRange = 1;

            if (pScrollData->nMaxRangeLogic - pScrollData->nPosLogic <= 
                (int)pScrollData->nPageRangeLogic)
                //For the last page
            {
                pScrollData->nPos = pScrollData->nMaxRange + pScrollData->nCYVScroll - 
                    pScrollData->ThumbRange;
            }
            else
            {
                pScrollData->nPos = pScrollData->nCYVScroll + 
                    pScrollData->nPosLogic * 
                    (pScrollData->nMaxRange + oldThumbRange - pScrollData->ThumbRange)
                    / pScrollData->nMaxRangeLogic ;
            }
        }
        else
        {
            pScrollData->nPageRange = 0;
            pScrollData->wLineRange = 0;
            pScrollData->nPos = pScrollData->nCYVScroll;
        }
    }
    
    if (pScrollData->byPenDown != PENDOWN_THUMB)
    {
        pScrollData->nTrackPosLogic = pScrollData->nPosLogic;
        pScrollData->nTrackPos = pScrollData->nPos;
    }

    if (bDisabledOld != pScrollData->bDisabled)
    {
        PaintArrow(hdc, pScrollData, pRect, TRUE, TRUE);
    }

    if ((BOOL)wParam && pRect)
    {
        if (pScrollData->byPenDown != PENDOWN_THUMB)
        {
            //if (pScrollData->bNeedReDraw)
            {
                PaintBKGND(hdc, pScrollData, pRect);
                PaintScrollThumb(hdc, pScrollData, pRect);
                
                pScrollData->bNeedReDraw = FALSE;
            }
            //else
             //   MoveScrollThumb(hdc, pScrollData, pRect, oldPos, oldReDrawThumbRange);
        }
    }
    else
    {
        pScrollData->bNeedReDraw = TRUE;
    }
    
    return pScrollData->nPosLogic + nOldMinRangeLogic;
}

int  SB_SetPos(HDC hdc, PSCROLLDATA pScrollData, 
               const RECT* pRect, WPARAM wParam, LPARAM lParam)
{
    int wPos, wRetPos = 0;
    BOOL fRedraw;
    SCROLLINFO ScrollInfo;

    wPos = (WORD)wParam ;    // new position of scroll box 
    fRedraw = (BOOL)lParam ; // redraw flag
    
    wPos -= pScrollData->nMinRangeLogic;

    if (wPos > pScrollData->nMaxRangeLogic) 
        wPos = pScrollData->nMaxRangeLogic;
    if (wPos < 0)
        wPos = 0;

    wRetPos = pScrollData->nPosLogic;
    pScrollData->nPosLogic = wPos;
    
    if ((wRetPos != pScrollData->nPosLogic) || fRedraw)
    {
        //为了重新计算nPos, nThumbPage, nPageRange，并重画。
        memset(&ScrollInfo, 0, sizeof(SCROLLINFO));
        SB_SetScrollInfo(hdc, pScrollData, pRect, fRedraw, (LPARAM)&ScrollInfo);
    }
    return wRetPos + pScrollData->nMinRangeLogic;
}

int  SB_GetPos(PSCROLLDATA pScrollData)
{
    int nPos;

    nPos = pScrollData->nPosLogic + pScrollData->nMinRangeLogic;

    if (nPos >= (pScrollData->nMinRangeLogic + pScrollData->nMaxRangeLogic))
    {
        if (pScrollData->ThumbRange)
        {
            nPos = pScrollData->nMinRangeLogic + pScrollData->nMaxRangeLogic - 
                pScrollData->nPageRangeLogic;
        }
        else
            nPos = pScrollData->nMinRangeLogic;
    }

    return nPos;
}

int  SB_SetRange(PSCROLLDATA pScrollData, WPARAM wParam, LPARAM lParam)
{
    int         nMaxPos, nMinPos, nOldMinRangeLogic, nOldMaxRangeLogic, nOldPosLogic;
    SCROLLINFO  ScrollInfo;

    nMinPos = (WORD)wParam; // minimum scrolling position 
    nMaxPos = (WORD)lParam; // maximum scrolling position 

    nOldPosLogic = pScrollData->nPosLogic;
    nOldMinRangeLogic = pScrollData->nMinRangeLogic;
    nOldMaxRangeLogic = pScrollData->nMaxRangeLogic;

    pScrollData->nMinRangeLogic = nMinPos;
    pScrollData->nMaxRangeLogic = nMaxPos - nMinPos + 1;
    
    //If the minimum and maximum position values are equal, 
    //the scroll bar control is hidden and, in effect, disabled. 
    if ((UINT)pScrollData->nMaxRangeLogic <= pScrollData->nPageRangeLogic)
        pScrollData->bDisabled = TRUE;
    else
        pScrollData->bDisabled = FALSE;
    
    if (pScrollData->nMaxRangeLogic < 0)
        pScrollData->nMaxRangeLogic = 0;
    
    if (nOldMaxRangeLogic != pScrollData->nMaxRangeLogic)
    {
        //为了重新计算nPos, nThumbPage, nPageRange。
        memset(&ScrollInfo, 0, sizeof(SCROLLINFO));
        SB_SetScrollInfo(NULL, pScrollData, NULL, FALSE, (LPARAM)&ScrollInfo);
        pScrollData->bNeedReDraw = TRUE;
    }
    
    return nOldPosLogic + nOldMinRangeLogic;
    //If the position of the scroll box changed, the return value is the 
    //previous position of the scroll box; otherwise, it is zero. 
}

int SB_GetRange(PSCROLLDATA pScrollData, WPARAM wParam, LPARAM lParam)
{
    WORD *lpnMaxPos, *lpnMinPos;
    int lResult;

    lResult = ((pScrollData->nMinRangeLogic + pScrollData->nMaxRangeLogic) << 16) + 
        pScrollData->nMinRangeLogic;

    lpnMinPos = (WORD *)wParam ; // minimum position 
    lpnMaxPos = (WORD *)lParam; // maximum position 

    if (!lpnMinPos || !lpnMaxPos)
        return lResult;
    
    *lpnMinPos = pScrollData->nMinRangeLogic;
    *lpnMaxPos = pScrollData->nMinRangeLogic + pScrollData->nMaxRangeLogic;

    return lResult;
}

int  SB_SetRangeRedraw(HDC hdc, PSCROLLDATA pScrollData, 
                      const RECT* pRect, WPARAM wParam, LPARAM lParam)
{
    int         nMaxPos, nMinPos, nOldMinRangeLogic, nOldMaxRangeLogic, nOldPosLogic;
    SCROLLINFO  ScrollInfo;

    nMinPos = (WORD)wParam; // minimum scrolling position 
    nMaxPos = (WORD)lParam; // maximum scrolling position 
    
    nOldPosLogic = pScrollData->nPosLogic;
    nOldMinRangeLogic = pScrollData->nMinRangeLogic;
    nOldMaxRangeLogic = pScrollData->nMaxRangeLogic;

    pScrollData->nMinRangeLogic = nMinPos;
    pScrollData->nMaxRangeLogic = nMaxPos - nMinPos + 1;
    
    if (pScrollData->nMaxRangeLogic < 0)
        pScrollData->nMaxRangeLogic = 0;
    
    //If the minimum and maximum position values are equal, 
    //the scroll bar control is hidden and, in effect, disabled. 
    if ((UINT)pScrollData->nMaxRangeLogic <= pScrollData->nPageRangeLogic)
        pScrollData->bDisabled = TRUE;
    else
        pScrollData->bDisabled = FALSE;
    
    //if (nOldMaxRangeLogic != pScrollData->nMaxRangeLogic)
    {
        //为了重新计算nPos, nThumbPage, nPageRange，并重画。
        memset(&ScrollInfo, 0, sizeof(SCROLLINFO));
        SB_SetScrollInfo(hdc, pScrollData, pRect, TRUE, (LPARAM)&ScrollInfo);
    }
    
    return nOldPosLogic + nOldMinRangeLogic;
    //If the position of the scroll box changed, the return value is the 
    //previous position of the scroll box; otherwise, it is zero. 
}

BOOL SB_Enable_Arrows(HDC hdc, PSCROLLDATA pScrollData, 
                      const RECT* pRect, WPARAM wParam)
{
    WORD fuArrowFlags;
    BOOL bLTArrowDisabledOld, bRBArrowDisabledOld;

    bLTArrowDisabledOld = pScrollData->bLTArrowDisabled;
    bRBArrowDisabledOld = pScrollData->bRBArrowDisabled;

    fuArrowFlags = (WORD)wParam; // scroll-bar arrow flags 
    
    if (!fuArrowFlags)//Enable both.
    {
        pScrollData->bLTArrowDisabled = FALSE;
        pScrollData->bRBArrowDisabled = FALSE;
    }
    else
    {
        if (fuArrowFlags & ESB_DISABLE_UP)
            pScrollData->bLTArrowDisabled = TRUE;
        
        if (fuArrowFlags & ESB_DISABLE_DOWN)
            pScrollData->bRBArrowDisabled = TRUE;
    }

    PaintArrow(hdc, pScrollData, pRect, 
        bLTArrowDisabledOld != pScrollData->bLTArrowDisabled,
        bRBArrowDisabledOld != pScrollData->bRBArrowDisabled);
    PaintBKGND(hdc, pScrollData, pRect);
    
    PaintScrollThumb(hdc, pScrollData, pRect);
    
    return TRUE;
}

static void GetArrowRect(PSCROLLDATA pScrollData, const RECT* prcScroll, 
                         IWORD wType, RECT* pRect)
{
    if (pScrollData->byVScroll & SBS_VERT)
    {
        if (wType == PENDOWN_LINEUP)
        {
            SetRect(pRect, prcScroll->left, prcScroll->top, 
                prcScroll->right, prcScroll->top + pScrollData->nCYVScroll - 1);
        }
        else
        {
            SetRect(pRect, prcScroll->left, prcScroll->bottom - pScrollData->nCYVScroll + 1, 
                prcScroll->right, prcScroll->bottom);
        }
    }
    else
    {
        if (wType == PENDOWN_LINEUP)
        {
            SetRect(pRect, prcScroll->left, prcScroll->top, 
                prcScroll->left + pScrollData->nCYVScroll, prcScroll->bottom);
        }
        else
        {
            SetRect(pRect, prcScroll->right - pScrollData->nCYVScroll, 
                prcScroll->top, prcScroll->right, prcScroll->bottom);
        }
    }
}

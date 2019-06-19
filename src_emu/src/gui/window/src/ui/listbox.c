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
#include "plx_pdaex.h"
#include "control.h"
#include "string.h"
#include "winpda.h"
#include "hpfile.h"

#define STR_MAXLENGTH     1024
#define MAX_DISPLAYTEXT   64
#define CX_LEFTMARGIN     5
#define CX_RIGHTMARGIN    3
#define CY_TOPMARGIN      3
#define CY_BTMMARGIN      3
#define CX_LEFTICON       27
#define CX_RIGHTICON      20
#define X_RIGHTICON       4
#define LINE_PRESCROLL    1

#define MAX_ICON_NUM      2

#define IDT_REPEAT        1
#define ET_REPEAT_FIRST   300
#define ET_REPEAT_LATER   100

#ifndef LANGUAGE_CHN
#define SPACE             6
#else
#define SPACE             11
#endif

typedef struct NODE
{
    PSTR   pNodeString;
    PSTR   pszAuxText;
    DWORD  dwNodeData;
    struct NODE* pPrev;
    struct NODE* pNext;
    HANDLE hImage[MAX_ICON_NUM];
    IWORD  wStrLen;
    IWORD  wAuxTextLen;
    BYTE   bSelect;
    BYTE   bCache;
	BYTE   bStringCache;
}LISTNODE, *PLISTNODE;

typedef struct
{
    PLISTNODE   pHead;
    PLISTNODE   pTail;
    PLISTNODE   pCurNode;
    PLISTNODE   pNodeCache;
    // I changed here
    char*       pStringCache;
    char*       pCurString;
    WORD        wStringCount;
    WORD        wCacheCount;
    IWORD       wIndex;
    WORD        wCurFirPOS;
    WORD        wLineRange;
    IWORD       wPageRange;
    BYTE        bVScroll;
    BYTE        bHScroll;
    WORD        wItemCount;
    WORD        wID;
    IWORD       wVirtFocus;
    BYTE        bFocus;
    BYTE        bPenDown;
    BYTE        bSelected;
    BYTE        bComboLBPenMoveIn;
    BYTE        bMultiSelect;
    BYTE        bSelChanged;
    BYTE        bChildWnd;
    BYTE        bInitData;
    HWND        hParentWnd;
    WORD        wBitmapWidth[MAX_ICON_NUM];
    WORD        wBitmapHeight[MAX_ICON_NUM];
    int         nColumnWidth;
    WORD        wColumns;
    WORD        wMaxTextLen;
    BYTE        byPageTextLen;
    WORD        wStartTextPos;
    BYTE        bBeginInit;
    BOOL        bRealFocus;
    BOOL        bIsNotSysCtrl;
    int         bIsSingle;
    int         xRightIcon;
    WORD        wKeyCode;
    int         nRepeats;
} LISTBOXDATA, *PLISTBOXDATA;

static HBITMAP  hListFocus;

// Internal function prototypes
LRESULT CALLBACK LISTBOX_WndProc(HWND hWnd, UINT wMsgCmd, 
                                 WPARAM wParam, LPARAM lParam);
static void LISTBOX_Paint(HWND hWnd, HDC hdc, PLISTBOXDATA pListBoxData, 
                          PLISTNODE pOldCurNode, IWORD wOldIndex, IWORD wOldFirPOS, 
                          BOOL bClear, int nLineStart, int nLineEnd, 
                          int nColStart, int nColEnd);

#if (!NOKEYBOARD)
static void SetAndLeaveFocus(HWND hWnd, HDC hdc, PLISTBOXDATA pListBoxData);
static void DrawFocus(HDC hdc, RECT* pRect);
#endif // NOKEYBOARD

static PLISTNODE NewNode(HWND hWnd, PLISTBOXDATA pListBoxData, const void* pData, 
                         BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings);
static void DeleteNode(PLISTNODE p);
//static void InitList(PLISTBOXDATA p);

static void FreeCache(PLISTBOXDATA p);
static void FreeList(PLISTBOXDATA p);
static void RemoveHead(PLISTBOXDATA p);
static void RemoveTail(PLISTBOXDATA p);
static BOOL AddHead(HWND hWnd, PLISTBOXDATA p, const void* pData, 
                    BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings);
static BOOL AddTail(HWND hWnd, PLISTBOXDATA p, const void* pData, 
                    BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings);
static BOOL InsertNode(HWND hWnd, PLISTBOXDATA p, const void* pData, int nPos, 
                       BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings);
static int InsertNodeSort(HWND hWnd, PLISTBOXDATA p, const void* pData, 
                       BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings);
static void RemoveNode(PLISTBOXDATA p, int nPos);
static PLISTNODE FindNode(PLISTBOXDATA p, IWORD wIndex);
//static IWORD CacuCurNodeIndex(PLISTBOXDATA pListBoxData);
//static IWORD SortList(PLISTBOXDATA pListBoxData, LPSTR lpsz);
static void ProcessKeyDown(HWND hWnd, PLISTBOXDATA pListBoxData, WPARAM wParam, LPARAM lParam);
static void ProcessKeyUp(HWND hWnd, PLISTBOXDATA pListBoxData, WPARAM wParam, LPARAM lParam);
static void DoHScroll(HWND hWnd, PLISTBOXDATA pListBoxData, int nScrollCode, IWORD nPos);
static void DoVScroll(HWND hWnd, PLISTBOXDATA pListBoxData, int nScrollCode, IWORD nPos);
static void Draw3DListBoxRect_W95(PLISTBOXDATA pListBoxData, HDC hdc, RECT* pRect, 
                                  BOOL bVScroll, BOOL bHscroll, DWORD dwStyle);
static LRESULT ProcessPen(HWND hWnd, PLISTBOXDATA pListBoxData, UINT wMsgCmd, 
                          WPARAM wParam, LPARAM lParam);
static void Bitmap_Paint(HDC hdc, HANDLE hImage, RECT* pRect);
static LRESULT ProcessSetImage(HWND hWnd, PLISTBOXDATA pListBoxData, 
                               WPARAM wParam, LPARAM lParam);
static LRESULT ProcessAddString(HWND hWnd, PLISTBOXDATA pListBoxData, 
                                IWORD wStrLen, LPARAM lParam);
static LRESULT ProcessSelectString(HWND hWnd, PLISTBOXDATA pListBoxData, 
                               WPARAM wParam, LPARAM lParam);
static LRESULT ProcessSetCurSel(HWND hWnd, PLISTBOXDATA pListBoxData, WPARAM wParam);
static LRESULT ProcessFindStringExact(HWND hWnd, PLISTBOXDATA pListBoxData, 
                               WPARAM wParam, LPARAM lParam);
static void ProcessResetContent(HWND hWnd, PLISTBOXDATA pListBoxData);
static LRESULT ProcessSetSel(HWND hWnd, PLISTBOXDATA pListBoxData, 
                               WPARAM wParam, LPARAM lParam);

// Update for Multiline List Box by Wang Wenchao
static LRESULT ProcessGetItemRect(HWND hWnd, PLISTBOXDATA pListBoxData,
                                  WPARAM wParam, LPARAM lParam);
static LRESULT ProcessSetAuxText(HWND hWnd, PLISTBOXDATA pListBoxData,
                                 WPARAM wParam, LPARAM lParam);
static void ProcessTimer(HWND hWnd, PLISTBOXDATA pListBoxData, WPARAM wParam);
// End of Multiline List Box

static HBRUSH SetCtlColorListBox(HWND hWnd, HDC hdc, PLISTBOXDATA pListBoxData);
static void ConvertToLBClient(HWND hWnd, RECT* pRect, PLISTBOXDATA pListBoxData,
                              int nBitsPixel, DWORD dwStyle);
char*  LocalString(int);        
static void DrawVirtFrame(PLISTBOXDATA pListBoxData, HDC hdc, RECT* pRect, 
                                BOOL bVScroll, BOOL bHScroll, DWORD dwStyle);
static int GetItemHeight(HWND hWnd, PLISTBOXDATA pListBoxData,
                         int *pnLine1Height, int *pnLine2Height);
static BOOL IsValidObject(HDC hdc, HGDIOBJ hGdiObj);
static int Strnicmp(const char *, const char *);


BOOL LISTBOX_RegisterClass(void)
{
    WNDCLASS wc;
    char PathName[256];
    int nPathLen;

    wc.style            = CS_OWNDC | CS_DEFWIN;//CS_PARENTDC;
    wc.lpfnWndProc      = LISTBOX_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(LISTBOXDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "LISTBOX";
    
    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);                    
    strcat(PathName, "listfocus.bmp");
    hListFocus = LoadImage(NULL, PathName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!RegisterClass(&wc))
        return FALSE;
    return TRUE;
}

LRESULT CALLBACK LISTBOX_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    PLISTBOXDATA    pListBoxData;
    PCREATESTRUCT   pCreateStruct;
    SCROLLINFO      ScrollInfo, ScrollInfoOld;
    RECT            rect, rcList;
    HDC             hdc;
    SIZE            size;
    int             nScrollCode, i, j, nWidth, nHeight, nBitsPixel, nItemHeight;
    IWORD           nPos, wOldCurFirstPOS;
    LPSTR           lpsz;
    DWORD           dwStyle;
    IWORD           index, cItems, wFirst, wLast;
    PLISTNODE       p, pOldCurNode;
    PINT            lpnItems;
    BOOL            fSelect;
    //char            achClassName[32];
	DWORD	        fImageType;
    HBRUSH          hBrush;
    PAINTSTRUCT     ps;
    int             lineStart, lineEnd, colStart = 0, colEnd = 0;
    HFONT           hFont;
    BOOL            fRedraw;

    lResult = 0;

    // Get user data for listbox.
    pListBoxData = (PLISTBOXDATA)GetUserData(hWnd);

    switch (wMsgCmd)
    {
    case WM_CREATE :

        pCreateStruct = (PCREATESTRUCT)lParam;
        
        memset(pListBoxData, 0, sizeof(LISTBOXDATA));

        pListBoxData->wID = (WORD)(DWORD)pCreateStruct->hMenu;
        pListBoxData->hParentWnd = pCreateStruct->hwndParent;

        if (pCreateStruct->style & WS_VSCROLL)
            pListBoxData->bVScroll = TRUE;

        if (pCreateStruct->style & WS_HSCROLL)
            pListBoxData->bHScroll = TRUE;

        pListBoxData->wLineRange = 1;
        
        nWidth = pCreateStruct->cx;
        nHeight = pCreateStruct->cy;

        hdc = GetDC(hWnd);
        GetTextExtentPoint32(hdc, "T", -1, &size);
        ReleaseDC(hWnd, hdc);
        
        nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);
        if (nBitsPixel != 1 && nBitsPixel != 2 && 
            !(pCreateStruct->style & LBS_COMBOLBOX))
        {
            nWidth -= 2;
            nHeight -= 2;

            if (!pListBoxData->bHScroll || !(pCreateStruct->style & LBS_DISABLENOSCROLL))
                nHeight -= 2;
            
            if (!pListBoxData->bVScroll || !(pCreateStruct->style & LBS_DISABLENOSCROLL))
                nWidth -= 2;
        }
        //pListBoxData->wPageRange = nHeight / (size.cy + SPACE);
        
        if (pListBoxData->wPageRange < 0)
            pListBoxData->wPageRange = 0;

        if (pCreateStruct->style & CS_NOSYSCTRL)
            pListBoxData->bIsNotSysCtrl = TRUE;
        else
            pListBoxData->bIsNotSysCtrl = FALSE;
        
        if (pCreateStruct->style & LBS_COMBOLBOX)
            pListBoxData->bRealFocus = TRUE;
        else
            pListBoxData->bRealFocus = FALSE;
        pListBoxData->bIsSingle = -1;
        
        pListBoxData->wVirtFocus = -1;
        pListBoxData->wIndex = -1;

        if (pCreateStruct->style & LBS_MULTIPLESEL)
            pListBoxData->bMultiSelect = TRUE;
            
        if (pCreateStruct->style & WS_CHILD)
            pListBoxData->bChildWnd = TRUE;

        if (pCreateStruct->style & LBS_MULTICOLUMN)
        {
            pListBoxData->nColumnWidth = 120;
         
            pListBoxData->wColumns = nWidth / pListBoxData->nColumnWidth;
            
            if (nWidth % pListBoxData->nColumnWidth)
                pListBoxData->wColumns ++;
        }
        else
        {
            nWidth -= 10;
            pListBoxData->byPageTextLen = nWidth / size.cx;
            //if (nWidth % size.cx)
            //    pListBoxData->byPageTextLen ++;
        }
        pListBoxData->bBeginInit = FALSE;
        pListBoxData->wKeyCode = 0;
        pListBoxData->nRepeats = 0;

        // Creating succeed.
        lResult = (LRESULT)TRUE;

        break;

    case WM_SIZE :

        nWidth = LOWORD(lParam);  // width of client area 
        nHeight = HIWORD(lParam); // height of client area 

        //nCXMin = GetSystemMetrics(SM_CXMIN);
        //nCYMin = GetSystemMetrics(SM_CYMIN);

        // Initializes the internal data
        hdc = GetDC(hWnd);
        GetTextExtentPoint32(hdc, "T", -1, &size);
        ReleaseDC(hWnd, hdc);
//        nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE); 
        nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
        
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        SetRect(&rect, 0, 0, nWidth, nHeight);
        ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
        nWidth = rect.right - rect.left;
        nHeight = rect.bottom - rect.top;

        pListBoxData->wPageRange = nHeight / nItemHeight;
        if (pListBoxData->wPageRange < 0)
            pListBoxData->wPageRange = 0;
        
        ScrollInfo.cbSize = sizeof(SCROLLINFO);
        ScrollInfo.fMask = SIF_ALL;
        if (dwStyle & LBS_DISABLENOSCROLL)
            ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
        ScrollInfo.nMin = 0;

        if (dwStyle & LBS_MULTICOLUMN)
        {
            pListBoxData->wColumns = nWidth / pListBoxData->nColumnWidth;
            if (nWidth % pListBoxData->nColumnWidth)
                pListBoxData->wColumns ++;
            
            if (pListBoxData->wItemCount <= pListBoxData->wPageRange *
                (pListBoxData->wColumns - 1))
            {
                pListBoxData->wCurFirPOS = 0;
            }
            else if (pListBoxData->wPageRange > 0 && (pListBoxData->wCurFirPOS + 
                pListBoxData->wPageRange * (pListBoxData->wColumns - 2)) >= 
                pListBoxData->wItemCount)
            {
                pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                    pListBoxData->wItemCount % pListBoxData->wPageRange -
                    pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
                
                if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                    pListBoxData->wCurFirPOS += pListBoxData->wPageRange;
            }
            
            if (pListBoxData->wIndex >= 0)
            {
                if (pListBoxData->wIndex < pListBoxData->wCurFirPOS)
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                        pListBoxData->wIndex % pListBoxData->wPageRange;
                }
                else if (pListBoxData->wPageRange > 0 && pListBoxData->wIndex >= 
                    (pListBoxData->wCurFirPOS + pListBoxData->wPageRange *
                    pListBoxData->wColumns))
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                        pListBoxData->wIndex % pListBoxData->wPageRange -
                        pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
                }
            }
            else
                pListBoxData->wCurFirPOS = 0;

            if (pListBoxData->wPageRange > 0)
            {
                ScrollInfo.nMax = pListBoxData->wItemCount / pListBoxData->wPageRange + 1;
                ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
            }
            else
                ScrollInfo.nMax = ScrollInfo.nPos = 0;

            if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                ScrollInfo.nMax ++;
            
            ScrollInfo.nPage = pListBoxData->wColumns;
            if (ScrollInfo.nMax != 0)
                ScrollInfo.nMax --;
            
            SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);

            if (pListBoxData->bVScroll)
            {
                ScrollInfo.nMax = ScrollInfo.nPage = 0;
                SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
            }
        }
        else
        {
            nWidth -= 10;
            pListBoxData->byPageTextLen = nWidth / size.cx;
            //if (nWidth % size.cx)
             //   pListBoxData->byPageTextLen ++;

            if (pListBoxData->wItemCount <= pListBoxData->wPageRange)
                pListBoxData->wCurFirPOS = 0;
            else if (pListBoxData->wPageRange > 0 && (pListBoxData->wCurFirPOS + 
                pListBoxData->wPageRange) > pListBoxData->wItemCount)
            {
                pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                    pListBoxData->wPageRange;
            }
            
            if (pListBoxData->wIndex >= 0)
            {
                if (pListBoxData->wIndex < pListBoxData->wCurFirPOS)
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex;
                else if (pListBoxData->wPageRange > 0 && pListBoxData->wIndex >= 
                    (pListBoxData->wCurFirPOS + pListBoxData->wPageRange))
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                        pListBoxData->wPageRange + 1;
                }
            }
            else
                pListBoxData->wCurFirPOS = 0;

            if (pListBoxData->bVScroll)
            {
                ScrollInfo.nMax = pListBoxData->wItemCount;
                ScrollInfo.nPage = pListBoxData->wPageRange;
                ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                
                if (ScrollInfo.nMax != 0)
                    ScrollInfo.nMax --;
                SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
            }

            if (pListBoxData->bHScroll)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_ALL;
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nMin = 0;
                ScrollInfo.nMax = pListBoxData->wMaxTextLen;
                ScrollInfo.nPage = pListBoxData->byPageTextLen;
                ScrollInfo.nPos = pListBoxData->wStartTextPos;
                
                if (ScrollInfo.nMax != 0)
                    ScrollInfo.nMax --;
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
            }
        }

        InvalidateRect(hWnd, NULL, TRUE);           

        break;

    case WM_NCDESTROY :

        FreeList(pListBoxData);
		FreeCache(pListBoxData);
						
        break;

    case WM_PAINT :

        hdc = BeginPaint(hWnd, &ps);

        nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        GetClientRect(hWnd, &rect);

        SetRect(&rcList, rect.left, rect.top, rect.right, rect.bottom);
        ConvertToLBClient(hWnd, &rcList, pListBoxData, nBitsPixel, dwStyle);

        if ((ps.rcPaint.left < rcList.left) ||
            (ps.rcPaint.top < rcList.top) || 
            (ps.rcPaint.right > rcList.right) ||
            (ps.rcPaint.bottom > rcList.bottom))
        {
            if (nBitsPixel != 1 && nBitsPixel != 2 && 
                !(dwStyle & LBS_COMBOLBOX) && !pListBoxData->wStartTextPos)
            {
                Draw3DListBoxRect_W95(pListBoxData, hdc, &rect, 
                    pListBoxData->bVScroll, pListBoxData->bHScroll, dwStyle);
            }
            if ((nBitsPixel == 1 || nBitsPixel == 2) && 
                !(dwStyle & LBS_COMBOLBOX) && !pListBoxData->wStartTextPos)
            {
                DrawVirtFrame(pListBoxData, hdc, &rect, 
                    pListBoxData->bVScroll, pListBoxData->bHScroll, dwStyle);
            }

        }

        nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);

        if (dwStyle & LBS_MULTICOLUMN)
        {
            colStart = ps.rcPaint.left / pListBoxData->nColumnWidth;
            colEnd = ps.rcPaint.right / pListBoxData->nColumnWidth;
        }

        lineStart = ps.rcPaint.top / nItemHeight;
        lineEnd = ps.rcPaint.bottom / nItemHeight;
        
        if (pListBoxData->pCurNode)
            pListBoxData->wVirtFocus = -1;
        LISTBOX_Paint(hWnd, hdc, pListBoxData, 
            NULL, -1, -1, FALSE, lineStart, lineEnd, colStart, colEnd);
        
        EndPaint(hWnd, &ps);
        break;

    case WM_MOUSEACTIVATE :
        
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);

        if (dwStyle & LBS_COMBOLBOX)
            lResult = (LRESULT)MA_NOACTIVATE;

        break;

    case WM_ERASEBKGND:
        
        GetClientRect(hWnd, &rect);
        hdc = (HDC)wParam;
        
        //多选列表框的背景用白色。
//        if (pListBoxData->bMultiSelect)
//        {
//            ClearRect(hdc, &rect, COLOR_WHITE);
//            break;
//        }

        hBrush = SetCtlColorListBox(hWnd, hdc, pListBoxData);
        
        FillRect(hdc, &rect, hBrush);

        lResult = (LRESULT)TRUE;
        break;

    case WM_CANCELMODE:
        
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (pListBoxData->bPenDown)
        {
            pListBoxData->bPenDown = 0;
            if (!(dwStyle & LBS_COMBOLBOX))
                ReleaseCapture();

        }
        if (dwStyle & LBS_COMBOLBOX)
        {
            SendMessage(pListBoxData->hParentWnd, WM_CANCELMODE, wParam, lParam);
        }

        break;

    case WM_GETDLGCODE:

        lResult = (LRESULT)DLGC_WANTARROWS | DLGC_WANTCHARS
			| DLGC_WANTRETURN;// | DLGC_WANTALLKEYS; 

        break;
#if (!NOKEYBOARD)
//#ifndef SCP_NOKEYBOARD
    case WM_KEYDOWN :

        ProcessKeyDown(hWnd, pListBoxData, wParam, lParam);
        
        break;

    case WM_KEYUP:
        ProcessKeyUp(hWnd, pListBoxData, wParam, lParam);
        break;
        
//#endif //SCP_NOKEYBOARD
#endif // NOKEYBOARD

    case WM_PENDOWN :
    case WM_PENMOVE :
    case WM_PENUP :
        
        lResult = ProcessPen(hWnd, pListBoxData, wMsgCmd, wParam, lParam);
        
        break;
        
    case WM_HSCROLL :

        nScrollCode = (int) LOWORD(wParam);
        nPos = (short int) HIWORD(wParam);

        DoHScroll(hWnd, pListBoxData, nScrollCode, nPos);
        break;

    case WM_VSCROLL :

        nScrollCode = (int) LOWORD(wParam);
        nPos = (short int) HIWORD(wParam);

        DoVScroll(hWnd, pListBoxData, nScrollCode, nPos);
        
        break;


    case WM_SETFOCUS :
        
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (pListBoxData->hParentWnd && !pListBoxData->bIsNotSysCtrl)
        {
            HWND    hwndNewFocus;
            if (pListBoxData->bIsSingle == -1)
            {
                hwndNewFocus = GetNextDlgTabItem(pListBoxData->hParentWnd, hWnd, TRUE);
                if (hwndNewFocus == hWnd)
                    pListBoxData->bIsSingle = 1;
                else
                    pListBoxData->bIsSingle = 0;
            }
            if ((pListBoxData->bIsSingle == 0) && !pListBoxData->bPenDown)
            {
                pListBoxData->bRealFocus = FALSE;
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP | SCROLLDOWN, MASKALL);
            }
            else
                pListBoxData->bRealFocus = TRUE;
        }
        else
            pListBoxData->bRealFocus = FALSE;
        if ((pListBoxData->bRealFocus == TRUE) && (pListBoxData->pCurNode != NULL) && 
            (pListBoxData->pHead != pListBoxData->pTail))
        {
            if ((pListBoxData->pHead  != pListBoxData->pTail))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLDOWN | SCROLLUP | SCROLLMIDDLE, MASKALL);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if ((pListBoxData->bHScroll) &&
                (pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
            else if (!(pListBoxData->bHScroll) && 
                (pListBoxData->wItemCount > pListBoxData->wPageRange))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
        }
        if (pListBoxData->pHead != NULL)
            SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                SCROLLMIDDLE, MASKMIDDLE);
        else
            SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                NULL, MASKMIDDLE);
        DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        pListBoxData->bFocus = TRUE;

#if (!NOKEYBOARD)
        if (IsWindowVisible(hWnd))
        {
            hdc = GetDC(hWnd);
            SetAndLeaveFocus(hWnd, hdc, pListBoxData);
            ReleaseDC(hWnd, hdc);
        }
#endif

        SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
            MAKELONG(pListBoxData->wID, LBN_SETFOCUS), (LPARAM)hWnd);
        

        break;

    case WM_KILLFOCUS :

        if (pListBoxData->bIsSingle == 0)
            pListBoxData->bRealFocus = FALSE;
        if (!pListBoxData->bRealFocus)
        {
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
        }
        DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        pListBoxData->bFocus = FALSE;

#if (!NOKEYBOARD)
        if (IsWindowVisible(hWnd))
        {
            hdc = GetDC(hWnd);
            SetAndLeaveFocus(hWnd, hdc, pListBoxData);
            ReleaseDC(hWnd, hdc);
        }
#endif

        pListBoxData->wVirtFocus = -1;
        
        //GetClassName(pListBoxData->hParentWnd, achClassName, 32);
        //achClassName[32] = 0;
        
        //if (stricmp(achClassName, "COMBOBOX"))
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (!(dwStyle & LBS_COMBOLBOX))
        {
            SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pListBoxData->wID, LBN_KILLFOCUS), (LPARAM)hWnd);
        }
        else
        {
            //Here(only in ComboBox) lParam is handle of window receiving focus.
            SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pListBoxData->wID, LBN_KILLFOCUS), (LPARAM)wParam);
        }

        KillTimer(hWnd, IDT_REPEAT);
        pListBoxData->wKeyCode = 0;
        pListBoxData->nRepeats = 0;
        break;

    case WM_SETFONT:
        hFont = (HFONT)wParam;
        fRedraw = lParam;
        hdc = GetDC(hWnd);
        SelectObject(hdc, hFont);
        ReleaseDC(hWnd, hdc);

        GetClientRect(hWnd, &rect);
//        GetTextExtentPoint32(hdc, "T", -1, &size);
//        nItemHeight = max (pListBoxData->wBitmapHeight, size.cy + SPACE); 
        nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
        pListBoxData->wPageRange = (rect.bottom - rect.top) / nItemHeight;

        if (fRedraw && IsWindowVisible(hWnd))
            InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_TIMER:
        ProcessTimer(hWnd, pListBoxData, wParam);
        break;

    case LB_BEGININIT:

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        /*
        if ((dwStyle & LBS_USERSTRINGS) || !(dwStyle & LBS_HASSTRINGS) && 
            ((dwStyle & LBS_OWNERDRAWFIXED) || (dwStyle & LBS_OWNERDRAWVARIABLE)))
        {
        */
        if (wParam > 0)
        {
			pListBoxData->bInitData = TRUE;
			pListBoxData->bBeginInit = TRUE;
            pListBoxData->wCacheCount = wParam;
            pListBoxData->pNodeCache = (LISTNODE *)LocalAlloc(LMEM_FIXED, 
                sizeof(LISTNODE) * pListBoxData->wCacheCount);
			
			if (!(dwStyle & LBS_USERSTRINGS))
			{
				pListBoxData->wStringCount = wParam;
				pListBoxData->pStringCache = pListBoxData->pCurString =
	                LocalString(pListBoxData->wStringCount);
			}
        }
        
        break;

    case LB_ENDINIT :

        pListBoxData->bInitData = FALSE;
		pListBoxData->bBeginInit = FALSE;//changed 7/13

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);

        if (dwStyle & LBS_MULTICOLUMN)
        {
            if (pListBoxData->wPageRange > 0)
            {
                if ((pListBoxData->wItemCount / pListBoxData->wPageRange) >
                    pListBoxData->wColumns || (((pListBoxData->wItemCount / 
                    pListBoxData->wPageRange) == pListBoxData->wColumns) && 
                    (pListBoxData->wItemCount % pListBoxData->wPageRange)))
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_ALL;
                    if (dwStyle & LBS_DISABLENOSCROLL)
                        ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                    
                    ScrollInfo.nMin = 0;
                    ScrollInfo.nMax = pListBoxData->wItemCount / pListBoxData->wPageRange + 1;
                    if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                        ScrollInfo.nMax ++;
                    
                    ScrollInfo.nPage = pListBoxData->wColumns;
                    ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
        
                    if (ScrollInfo.nMax != 0)
                        ScrollInfo.nMax --;
                    if (IsWindowVisible(hWnd))
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                    else
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
                }
            }
        }
        else
        {
            if ((pListBoxData->wItemCount > pListBoxData->wPageRange) &&
                pListBoxData->bVScroll)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_ALL;
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nMin = 0;
                ScrollInfo.nMax = pListBoxData->wItemCount;
                ScrollInfo.nPage = pListBoxData->wPageRange;
                ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                
                if (ScrollInfo.nMax != 0)
                    ScrollInfo.nMax --;
                if (IsWindowVisible(hWnd))
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                else
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
            }

            if (pListBoxData->bHScroll)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_ALL;
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nMin = 0;
                ScrollInfo.nMax = pListBoxData->wMaxTextLen;
                ScrollInfo.nPage = pListBoxData->byPageTextLen;
                ScrollInfo.nPos = pListBoxData->wStartTextPos;
                
                if (ScrollInfo.nMax != 0)
                    ScrollInfo.nMax --;
                if (IsWindowVisible(hWnd))
                    SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                else
                    SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
            }
        }

        if (IsWindowVisible(hWnd))
            InvalidateRect(hWnd, NULL, TRUE);

        break;

    case LB_ADDSTRING :         

        lResult = ProcessAddString(hWnd, pListBoxData, (WORD)wParam, lParam);
        break;

    case LB_INSERTSTRING :      

        lpsz = (LPSTR)lParam ; // address of string to insert 
        
        if (lpsz == NULL)
        {
            lResult = (LRESULT)LB_ERR;
            break;
        }

        index = LOWORD(wParam);          // item index 
        
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);

        if (index == -1)
        {
            index = pListBoxData->wItemCount;
        }
        else if (index < -1 || index > pListBoxData->wItemCount)
        {
            lResult = (LRESULT)LB_ERR;
            break;
        }
        
        wOldCurFirstPOS = pListBoxData->wCurFirPOS;
        if (!InsertNode(hWnd, pListBoxData, lpsz, index, 
            !(!(dwStyle & LBS_HASSTRINGS) && ((dwStyle & LBS_OWNERDRAWFIXED) ||
            (dwStyle & LBS_OWNERDRAWVARIABLE))), 
            HIWORD(wParam), (dwStyle & LBS_USERSTRINGS)))
        {
            lResult = (LRESULT)LB_ERRSPACE;
            break;
        }
        
        pListBoxData->wItemCount +=1;
        if (index <= pListBoxData->wIndex)
            pListBoxData->wIndex ++;

        lResult = (LRESULT)index;
        
        if (pListBoxData->bInitData)
            break;

        if (dwStyle & LBS_MULTICOLUMN)
        {
            if (pListBoxData->wPageRange > 0)
            {
                if ((pListBoxData->wItemCount / pListBoxData->wPageRange) >
                    (pListBoxData->wColumns - 1) || (((pListBoxData->wItemCount / 
                    pListBoxData->wPageRange) == (pListBoxData->wColumns - 1)) && 
                    (pListBoxData->wItemCount % pListBoxData->wPageRange)))
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_ALL;
                    if (dwStyle & LBS_DISABLENOSCROLL)
                        ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                    
                    ScrollInfo.nMin = 0;
                    ScrollInfo.nMax = pListBoxData->wItemCount / 
                        pListBoxData->wPageRange + 1;
                    if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                        ScrollInfo.nMax ++;
                    
                    ScrollInfo.nPage = pListBoxData->wColumns;
                    ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
                    
                    if (ScrollInfo.nMax != 0)
                        ScrollInfo.nMax --;
                    if (IsWindowVisible(hWnd))
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                    else
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
                }
            }
        }
        else
        {
            if ((pListBoxData->wItemCount >= pListBoxData->wPageRange) &&
                pListBoxData->bVScroll)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_ALL;
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nMin = 0;
                ScrollInfo.nMax = pListBoxData->wItemCount;
                ScrollInfo.nPage = pListBoxData->wPageRange;
                ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                
                if (ScrollInfo.nMax != 0)
                    ScrollInfo.nMax --;
                if (IsWindowVisible(hWnd))
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                else
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
            }

            if (pListBoxData->bHScroll)
            {
                SCROLLINFO ScrollInfoOld;
                
                ScrollInfoOld.cbSize = sizeof(SCROLLINFO);
                ScrollInfoOld.fMask = SIF_ALL;
                GetScrollInfo(hWnd, SB_HORZ, &ScrollInfoOld);
                
                if ((UINT)ScrollInfoOld.nMax < pListBoxData->wMaxTextLen)
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_ALL;
                    if (dwStyle & LBS_DISABLENOSCROLL)
                        ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                    
                    ScrollInfo.nMin = 0;
                    ScrollInfo.nMax = pListBoxData->wMaxTextLen;
                    ScrollInfo.nPage = pListBoxData->byPageTextLen;
                    ScrollInfo.nPos = pListBoxData->wStartTextPos;
                    
                    if (ScrollInfo.nMax != 0)
                        ScrollInfo.nMax --;
                    if (IsWindowVisible(hWnd))
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                    else
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
                }
            }
        }

        if (IsWindowVisible(hWnd) && ((!(dwStyle & LBS_MULTICOLUMN) && 
            (index < (wOldCurFirstPOS + pListBoxData->wPageRange))) || 
            ((dwStyle & LBS_MULTICOLUMN) && 
            (index < (wOldCurFirstPOS + pListBoxData->wPageRange *
            pListBoxData->wColumns)))))
        {
            int nItemHeight;
//            SIZE size;
            RECT rcList;

            GetClientRect(hWnd, &rect);
            ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
            SetRect(&rcList, rect.left, rect.top, rect.right, rect.bottom);

//            hdc = GetDC(hWnd);
//            GetTextExtentPoint32(hdc, "T", -1, &size);
            nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
//            ReleaseDC(hWnd, hdc);
//            nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);
            
            if (wOldCurFirstPOS == pListBoxData->wCurFirPOS)
            {
                if (dwStyle & LBS_MULTICOLUMN)
                {
                    rect.left += pListBoxData->nColumnWidth * 
                        ((index - wOldCurFirstPOS) / pListBoxData->wPageRange);
                    //rect.top += nItemHeight * ((index - wOldCurFirstPOS) %
                        //pListBoxData->wPageRange);
                }
                else
                {
                    rect.top += (index - wOldCurFirstPOS) * nItemHeight;
                }
            }

            //According MS Windows, paint with InvalidateRect.
            InvalidateRect(hWnd, &rect, TRUE);
        }
        
        break;

    case LB_DELETESTRING :
        
        index = (IWORD)wParam;    // index of string to delete 
        if (index >= 0 && index < pListBoxData->wItemCount)
        {
            IWORD   nCurIndex;

            pOldCurNode = pListBoxData->pCurNode;
            wOldCurFirstPOS = pListBoxData->wCurFirPOS;
            nCurIndex = pListBoxData->wIndex;

            RemoveNode(pListBoxData, index);

            pListBoxData->wItemCount -=1;

            if (pListBoxData->pCurNode && (nCurIndex == index))
            {
                pListBoxData->pCurNode = NULL;

                if (pListBoxData->wIndex - 1 >= 0)
                {
                    pListBoxData->wIndex --;
                    pListBoxData->wVirtFocus = -1;
                }
                else
                {
                    pListBoxData->wVirtFocus = 0;
                    pListBoxData->wIndex = -1;
                }
            }
            else
            {
                if (pListBoxData->wIndex >= index)
                    pListBoxData->wIndex --;
            }

            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & LBS_MULTICOLUMN)
            {
                if (pListBoxData->wItemCount <= pListBoxData->wPageRange *
                    (pListBoxData->wColumns - 1))
                {
                    pListBoxData->wCurFirPOS = 0;
                }
                else if (pListBoxData->wPageRange > 0 && (pListBoxData->wCurFirPOS + 
                    pListBoxData->wPageRange * (pListBoxData->wColumns - 2)) >= 
                    pListBoxData->wItemCount)
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                        pListBoxData->wItemCount % pListBoxData->wPageRange -
                        pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
                    
                    if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                        pListBoxData->wCurFirPOS += pListBoxData->wPageRange;
                }
            }
            else
            {
                if (pListBoxData->wItemCount <= pListBoxData->wPageRange)
                    pListBoxData->wCurFirPOS = 0;
                else if (pListBoxData->wPageRange > 0 && (pListBoxData->wCurFirPOS + 
                    pListBoxData->wPageRange) > pListBoxData->wItemCount)
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                        pListBoxData->wPageRange;
                }
            }
            
            if (dwStyle & LBS_MULTICOLUMN)
            {
                if (pListBoxData->wPageRange > 0)
                {
                    ScrollInfoOld.cbSize = sizeof(SCROLLINFO);
                    ScrollInfoOld.fMask = SIF_ALL;
                    GetScrollInfo(hWnd, SB_HORZ, &ScrollInfoOld);
                    
                    //Have scroll bar.
                    if ((UINT)ScrollInfoOld.nMax >= ScrollInfoOld.nPage)
                    {
                        ScrollInfo.cbSize = sizeof(SCROLLINFO);
                        ScrollInfo.fMask = SIF_ALL;
                        if (dwStyle & LBS_DISABLENOSCROLL)
                            ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                        
                        ScrollInfo.nMin = 0;
                        ScrollInfo.nMax = pListBoxData->wItemCount / 
                            pListBoxData->wPageRange + 1;
                        if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                            ScrollInfo.nMax ++;
                        
                        ScrollInfo.nPage = pListBoxData->wColumns;
                        ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
                        
                        if (ScrollInfo.nMax != 0)
                            ScrollInfo.nMax --;
                        if (IsWindowVisible(hWnd))
                            SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                        else
                            SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
                    }
                }
            }
            else if (pListBoxData->bVScroll)
            {
                ScrollInfoOld.cbSize = sizeof(SCROLLINFO);
                ScrollInfoOld.fMask = SIF_ALL;
                GetScrollInfo(hWnd, SB_VERT, &ScrollInfoOld);
                
                //Have scroll bar.
                if ((UINT)ScrollInfoOld.nMax >= ScrollInfoOld.nPage)
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_ALL;
                    
                    if (dwStyle & LBS_DISABLENOSCROLL)
                        ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                    
                    ScrollInfo.nMin = 0;
                    ScrollInfo.nMax = pListBoxData->wItemCount;
                    ScrollInfo.nPage = pListBoxData->wPageRange;
                    ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                    
                    if (ScrollInfo.nMax != 0)
                        ScrollInfo.nMax --;
                    if (IsWindowVisible(hWnd))
                        SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                    else
                        SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
                }
            }
            if (pListBoxData->bHScroll)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_ALL;
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                pListBoxData->wStartTextPos = 0;
                
                ScrollInfo.nMin = 0;
                ScrollInfo.nMax = pListBoxData->wMaxTextLen;
                ScrollInfo.nPage = pListBoxData->byPageTextLen;
                ScrollInfo.nPos = pListBoxData->wStartTextPos;
                
                if (ScrollInfo.nMax != 0)
                    ScrollInfo.nMax --;
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);

            }

            lResult = (LRESULT)pListBoxData->wItemCount;

            /*if (IsWindowVisible(hWnd) && ((!(dwStyle & LBS_MULTICOLUMN) && 
                (index < (wOldCurFirstPOS + pListBoxData->wPageRange))) || 
                ((dwStyle & LBS_MULTICOLUMN) && 
                (index < (wOldCurFirstPOS + pListBoxData->wPageRange *
                pListBoxData->wColumns)))))
            {
                int nItemHeight;
                SIZE size;
                RECT rcList;
                
                GetClientRect(hWnd, &rect);
                ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);

                if (wOldCurFirstPOS != pListBoxData->wCurFirPOS)
                    InvalidateRect(hWnd, &rect, TRUE);
                else
                {
                    SetRect(&rcList, rect.left, rect.top, rect.right, rect.bottom);
                    hdc = GetDC(hWnd);
                    GetTextExtentPoint32(hdc, "T", -1, &size);
                    ReleaseDC(hWnd, hdc);
                    nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);
                    
                    if (dwStyle & LBS_MULTICOLUMN)
                    {
                        rect.left += pListBoxData->nColumnWidth * 
                            ((index - wOldCurFirstPOS) / pListBoxData->wPageRange);
                        //rect.top += nItemHeight * ((index - wOldCurFirstPOS) %
                            //pListBoxData->wPageRange);
                    }
                    else
                        rect.top += (index - wOldCurFirstPOS) * nItemHeight;

                    InvalidateRect(hWnd, &rect, TRUE);
                }
            }*/
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else
            lResult = (LRESULT)LB_ERR;

        break;

    case LB_RESETCONTENT :

        ProcessResetContent(hWnd, pListBoxData);

        //This message does not return a value. 
        break;

    case LB_SETSEL :

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (dwStyle & WS_TABSTOP)
            pListBoxData->bRealFocus = TRUE;
        lResult = ProcessSetSel(hWnd, pListBoxData, wParam, lParam);

        break;

    case LB_SETCURSEL :
        
        lResult = ProcessSetCurSel(hWnd, pListBoxData, wParam);
        
        break;

    case LB_GETSEL :
        index = (IWORD) wParam;     // item index       
        
        if (index >= 0 && index < pListBoxData->wItemCount)
        {
            p = FindNode(pListBoxData, index);
        
            if (pListBoxData->bMultiSelect)
            { 
                lResult = (LRESULT)p->bSelect;   // selection flag 
            }
            else if (p == pListBoxData->pCurNode)
                lResult = (LRESULT)TRUE;
        }
        else
            lResult = (LRESULT)LB_ERR;

        break;
        
    case LB_GETCURSEL :
        
        if (!pListBoxData->bMultiSelect)
        {
            if (pListBoxData->pCurNode)
                lResult = (LRESULT)pListBoxData->wIndex;
            else
                lResult = (LRESULT)LB_ERR;
        }
        else
        {
            //returns the index of the item that has the focus rectangle. 
            lResult = (LRESULT)pListBoxData->wIndex;
            if (lResult == -1)//If no items are selected, it returns zero. 
                lResult = (LRESULT)0;
        }

        break;

    case LB_GETTEXT :

        index = (IWORD) wParam;     // item index       
        if (index < 0 || index >= pListBoxData->wItemCount)
        {
            lResult = (LRESULT)LB_ERR;
            break;
        }
        
        lpsz = (LPSTR)lParam; // address of buffer        
        if (lpsz)
        {
            p = FindNode(pListBoxData, index);
            
            if (p->pNodeString)
            {
                dwStyle = GetWindowLong(hWnd, GWL_STYLE);
                if (dwStyle & LBS_USERSTRINGS)
                {
                    memcpy((LPSTR)lpsz, p->pNodeString, p->wStrLen);
                    *(lpsz + p->wStrLen) = 0;
                    lResult = (LRESULT)(p->wStrLen);
                }
                else
                {
                    strcpy((LPSTR)lpsz, p->pNodeString);
                    lResult = (LRESULT)(strlen(lpsz));
                }
            }
        }

        break;

    case LB_SETTEXT :

        index = LOWORD(wParam);     // item index       
        if (index < 0 || index >= pListBoxData->wItemCount)
        {
            lResult = (LRESULT)LB_ERR;
            break;
        }
        
        lpsz = (LPSTR)lParam; // address of buffer        
        if (lpsz)
        {
            p = FindNode(pListBoxData, index);
            
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & LBS_USERSTRINGS)
            {
                p->pNodeString = lpsz;
                p->wStrLen = HIWORD(wParam);
                if (p->wStrLen == -1)
                    p->wStrLen = strlen(lpsz);

                lResult = (LRESULT)(p->wStrLen);
            }
            else
            {
                PLISTNODE   s, pPrev = p->pPrev, pNext = p->pNext;
                WORD        len = strlen(lpsz), oldLen = strlen(p->pNodeString);
                BOOL        bCurNode = FALSE;
                HANDLE      hImage[2] = {NULL};
                int         ItemData;
                
                if (oldLen < len)
                {
                    if (p == pListBoxData->pCurNode)
                        bCurNode = TRUE;

                    if (p->hImage[0])
                        hImage[0] = p->hImage[0];
                    if (p->hImage[1] != NULL)
                        hImage[1] = p->hImage[1];
                    ItemData = p->dwNodeData;
                        
                    DeleteNode(p);
                    
                    s = NewNode(hWnd, pListBoxData, lpsz, 
                        !(!(dwStyle & LBS_HASSTRINGS) && ((dwStyle & LBS_OWNERDRAWFIXED) ||
                        (dwStyle & LBS_OWNERDRAWVARIABLE))), 
                        0, (dwStyle & LBS_USERSTRINGS));
                    if (hImage[0])
                        s->hImage[0] = hImage[0];
                    if (hImage[1] != NULL)
                        s->hImage[1] = hImage[1];
                    s->dwNodeData = ItemData;

                    if (!s)
                    {
                        lResult = (LRESULT)LB_ERRSPACE;
                        break;
                    }

                    if (pPrev)
                        pPrev->pNext = s;
                    else
                        pListBoxData->pHead = s;
                    
                    if (pNext)
                        pNext->pPrev = s;
                    else
                        pListBoxData->pTail = s;

                    s->pNext = pNext;
                    s->pPrev = pPrev;

                    if (bCurNode)
                        pListBoxData->pCurNode = s;

                    p = s;
                }

                memcpy((LPSTR)p->pNodeString, lpsz, len);
                for (i = len; i < oldLen; i++)
                    *(p->pNodeString + i) = '\0';

                lResult = (LRESULT)len;
            }

            if (IsWindowVisible(hWnd) && index >= pListBoxData->wCurFirPOS && 
                ((!(dwStyle & LBS_MULTICOLUMN) && 
                (index < (pListBoxData->wCurFirPOS + pListBoxData->wPageRange))) || 
                ((dwStyle & LBS_MULTICOLUMN) && 
                (index < (pListBoxData->wCurFirPOS + pListBoxData->wPageRange *
                pListBoxData->wColumns)))))
            {
                int     nItemHeight;
//                SIZE    size;
                RECT    rcList;
                
                GetClientRect(hWnd, &rect);
                ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
                SetRect(&rcList, rect.left, rect.top, rect.right, rect.bottom);

//                hdc = GetDC(hWnd);
//                GetTextExtentPoint32(hdc, "T", -1, &size);
                nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
//                ReleaseDC(hWnd, hdc);
//                nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);
                
                if (dwStyle & LBS_MULTICOLUMN)
                {
                    rect.left += pListBoxData->nColumnWidth * 
                        ((index - pListBoxData->wCurFirPOS) / pListBoxData->wPageRange);
                    rect.right = rect.left + pListBoxData->nColumnWidth;
                    if (rect.right > rcList.right)
                        rect.right = rcList.right;

                    rect.top += nItemHeight * ((index - pListBoxData->wCurFirPOS) %
                        pListBoxData->wPageRange);
                }
                else
                    rect.top += nItemHeight * (index - pListBoxData->wCurFirPOS);

                rect.bottom = rect.top + nItemHeight; 

                InvalidateRect(hWnd, &rect, TRUE);
            }
        }
        else
            lResult = (LRESULT)LB_ERR;

        break;

    case LB_GETTEXTLEN :

        index = (IWORD) wParam;     // item index       
        if (index < 0 || index >= pListBoxData->wItemCount)
        {
            lResult = (LRESULT)LB_ERR;
            break;
        }

        p = FindNode(pListBoxData, index);

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (dwStyle & LBS_USERSTRINGS)
            lResult = (LRESULT)(p->wStrLen);
        else
            lResult = (LRESULT)strlen(p->pNodeString);   // selection flag 

        break;

    case LB_GETCOUNT :

        lResult = (LRESULT)pListBoxData->wItemCount;
        break;

    case LB_SELECTSTRING :       
        
        lResult = ProcessSelectString(hWnd, pListBoxData, wParam, lParam);
        break;

    case LB_GETTOPINDEX :
        
        lResult = (LRESULT)pListBoxData->wCurFirPOS;
        break;

    case LB_FINDSTRING :

        index = (IWORD)wParam;        // item before start of search 
        lpsz = (LPSTR)lParam;  // address of search string 
         
        if (!lpsz || !strlen(lpsz))
        {
            lResult = (LRESULT)LB_ERR;
            break;
        }

        //According MS Windows.
        if (index < 0 || index >= pListBoxData->wItemCount - 1)
        {
            p = pListBoxData->pHead;
            index = i = 0;
        }
        else
        {
            p = FindNode(pListBoxData, (IWORD)(index + 1));
            i = index = index + 1;
        }

        do
        {
            if (p == NULL)
                break;
            
            if (Strnicmp(p->pNodeString, lpsz) == 0)
            {
                lResult = (LRESULT)i;
                return lResult;
            }
            p = p->pNext;
            i++;
        }while(1);
            
        if (!lResult && index != 0)
        {
            p = pListBoxData->pHead;
            i = 0;
            do
            {
                if (p == NULL)
                    break;
                
                if (Strnicmp(p->pNodeString, lpsz) == 0)
                {
                    lResult = (LRESULT)i;
                    return lResult;
                }
                p = p->pNext;
                i++;
            }while(i < index);
        }

        if (!lResult)
            lResult = (LRESULT)LB_ERR;
        break;

    case LB_GETSELCOUNT :       

        if (!pListBoxData->bMultiSelect)
        {
            lResult = (LRESULT)LB_ERR;
            break;
        }

        p = pListBoxData->pHead;
        i = 0;
        do
        {
            if (p == NULL)
                break;
            
            if (p->bSelect)
            {
                i++;
            }
            p = p->pNext;
        }while(1);

        lResult = (LRESULT)i;
        break;

    case LB_GETSELITEMS :

        if (!pListBoxData->bMultiSelect)
        {
            lResult = (LRESULT)LB_ERR;
            break;
        }

        cItems = (WORD)wParam;           // maximum number of items 
        lpnItems = (PINT)lParam; // address of buffer 
        
        p = pListBoxData->pHead;
        i = 0;
        j = 0;

        do
        {
            if (p == NULL)
                break;
            
            if (j >= cItems)
                break;

            if (p->bSelect)
            {
                *(lpnItems + j) = i;
                j++;
            }
            i++;

            p = p->pNext;
        }while(1);

        lResult = (LRESULT)j;
        break;

    case LB_SETTOPINDEX :       

        index = (IWORD)wParam;
        wOldCurFirstPOS = pListBoxData->wCurFirPOS;

        //According MS Windows.
        if (index < 0 || index >= pListBoxData->wItemCount)
        {
            lResult = (LRESULT)LB_ERR;
            break;
        }

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (((dwStyle & LBS_MULTICOLUMN) && pListBoxData->wItemCount <= 
            pListBoxData->wPageRange * (pListBoxData->wColumns - 1)) || 
            (!(dwStyle & LBS_MULTICOLUMN) && pListBoxData->wItemCount <= 
            pListBoxData->wPageRange))
        {
            break;
        }

        if (dwStyle & LBS_MULTICOLUMN)
        {
            if (pListBoxData->wPageRange > 0 && (index - 
                index % pListBoxData->wPageRange + 
                pListBoxData->wPageRange * (pListBoxData->wColumns - 1)) >
                pListBoxData->wItemCount)
            {
                pListBoxData->wCurFirPOS = index - index % pListBoxData->wPageRange -
                    pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
            }
            else if (pListBoxData->wItemCount <= pListBoxData->wPageRange * 
                pListBoxData->wColumns)
            {
                pListBoxData->wCurFirPOS = 0;
            }
            else
            {
                pListBoxData->wCurFirPOS = index - index % pListBoxData->wPageRange;
            }
        }
        else
        {
            if (pListBoxData->wPageRange > 0 && index > pListBoxData->wItemCount - 
                pListBoxData->wPageRange)
            {
                pListBoxData->wCurFirPOS = pListBoxData->wItemCount 
                    - pListBoxData->wPageRange;
            }
            else
                pListBoxData->wCurFirPOS = index;
        }

        if (pListBoxData->wCurFirPOS == wOldCurFirstPOS)
            break;
                    
        if (dwStyle & LBS_MULTICOLUMN)
        {
            if (pListBoxData->wItemCount > pListBoxData->wPageRange *
                (pListBoxData->wColumns - 1))
            {
                ScrollInfoOld.cbSize = sizeof(SCROLLINFO);
                ScrollInfoOld.fMask = SIF_ALL;
                GetScrollInfo(hWnd, SB_HORZ, &ScrollInfoOld);
                
                if ((UINT)ScrollInfoOld.nMax >= ScrollInfoOld.nPage)
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_POS;
                    if (dwStyle & LBS_DISABLENOSCROLL)
                        ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                    
                    ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
                    
                    if (IsWindowVisible(hWnd))
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                    else
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
                }
            }
        }
        else if (pListBoxData->bVScroll)
        {
            ScrollInfoOld.cbSize = sizeof(SCROLLINFO);
            ScrollInfoOld.fMask = SIF_ALL;
            GetScrollInfo(hWnd, SB_VERT, &ScrollInfoOld);
            
            if ((UINT)ScrollInfoOld.nMax >= ScrollInfoOld.nPage)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_POS;
                
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                
                if (IsWindowVisible(hWnd))
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                else
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
            }
        }

        if (IsWindowVisible(hWnd))
		{
            GetClientRect(hWnd, &rect);
            ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
            InvalidateRect(hWnd, &rect, TRUE);
        }
      
        break;

    case LB_GETITEMDATA :       

        index = (IWORD)wParam;          // item index 
        
        p = FindNode(pListBoxData, index);
        if (p != NULL)
        {
            lResult = (LRESULT)p->dwNodeData;
        }
        else
            lResult = (LRESULT)LB_ERR;
        
        break;

    case LB_SETITEMDATA :      

        index = (IWORD)wParam;          // item index 
        
        p = pListBoxData->pHead;
        
        if (index != -1)
        {
            lResult = (LRESULT)LB_ERR;
        }
        else
            lResult = (LRESULT)TRUE;
            
        i = 0;
        do
        {
            if (p == NULL)
                break;
            
            if (index == -1)
                p->dwNodeData = (DWORD)lParam ; // address of string to insert 
            else if (i == index)
            {
                p->dwNodeData = (DWORD)lParam ; // address of string to insert 
                lResult = (LRESULT)TRUE;
                break;
            }
            p = p->pNext;
            i++;
        } while(1);
        
        break;

    case LB_SELITEMRANGE :   

        if (pListBoxData->bMultiSelect)
        {
            int nItemHeight;
//            SIZE size;
            
            fSelect = (BOOL) wParam;
            wFirst = LOWORD(lParam);
            wLast = HIWORD(lParam);
            
            if ((wFirst > wLast || wFirst < 0) && wLast >= 0 && 
                wLast < pListBoxData->wItemCount)
            {
                index = wFirst;
                wFirst = wLast;
                wLast = index;
            }
            if (wLast < 0)
                wLast = pListBoxData->wItemCount - 1;

            p = FindNode(pListBoxData, wFirst);

            i = wFirst;

            GetClientRect(hWnd, &rect);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
            SetRect(&rcList, rect.left, rect.top, rect.right, rect.bottom);

//            hdc = GetDC(hWnd);
//            GetTextExtentPoint32(hdc, "T", -1, &size);
            nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
//            ReleaseDC(hWnd, hdc);
//            nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);

            do
            {
                if (p == NULL)
                    break;
                
                if (p->bSelect != fSelect)
                {
                    p->bSelect = fSelect;

                    //MS Window also paint it with InvalidateRect.
                    if (IsWindowVisible(hWnd) && ((!(dwStyle & LBS_MULTICOLUMN) && (i < 
                        (pListBoxData->wCurFirPOS + pListBoxData->wPageRange))) || 
                        ((dwStyle & LBS_MULTICOLUMN) && 
                        (i < (pListBoxData->wCurFirPOS + pListBoxData->wPageRange *
                        pListBoxData->wColumns)))))
                    {
                        if (dwStyle & LBS_MULTICOLUMN)
                        {
                            rect.left = rcList.left + pListBoxData->nColumnWidth * 
                                ((i - pListBoxData->wCurFirPOS) / pListBoxData->wPageRange);
                            rect.right = rect.left + pListBoxData->nColumnWidth;
                            if (rect.right > rcList.right)
                                rect.right = rcList.right;
                            
                            rect.top = nItemHeight * ((i - pListBoxData->wCurFirPOS) %
                                pListBoxData->wPageRange) + rcList.top;
                        }
                        else
                        {
                            rect.top = rcList.top + 
                                (i - pListBoxData->wCurFirPOS) * nItemHeight;
                        }

                        rect.bottom = rect.top + nItemHeight;

                        InvalidateRect(hWnd, &rect, TRUE);
                    }
                }

                p = p->pNext;
                i++;
            } while(i <= wLast);
        }
        else
        {
            lResult = (LRESULT)LB_ERR;
        }

        break;

    case LB_FINDSTRINGEXACT :

        lResult = ProcessFindStringExact(hWnd, pListBoxData, wParam, lParam);

        break;
    
    case LB_GETIMAGE :

        fImageType = (WORD)wParam;       // image-type flag 
        index = (IWORD)lParam;
		
        p = FindNode(pListBoxData, index);
        if (!p)
            break;

        dwStyle = GetWindowLong(hWnd, GWL_STYLE);

		if (((fImageType == IMAGE_BITMAP) && (dwStyle & LBS_BITMAP)) ||
			((fImageType == IMAGE_ICON) && (dwStyle & LBS_ICON)))
		{
			lResult = (LRESULT)p->hImage[0];
		}

        break;

    case LB_SETIMAGE :
		
        lResult = ProcessSetImage(hWnd, pListBoxData, wParam, lParam);

        break;

    case LB_SETCOLUMNWIDTH :

        pListBoxData->nColumnWidth = wParam;
        
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        GetClientRect(hWnd, &rect);
        ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);

        nWidth = rect.right - rect.left;

        pListBoxData->wColumns = nWidth / pListBoxData->nColumnWidth;
        if (nWidth % pListBoxData->nColumnWidth)
            pListBoxData->wColumns ++;

        break;

    case LB_GETITEMRECT:
        
        ProcessGetItemRect(hWnd, pListBoxData, wParam, lParam);

        break;

    case LB_SETAUXTEXT:
        
        ProcessSetAuxText(hWnd, pListBoxData, wParam, lParam);

        break;

    default :

        lResult = DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}
static void LISTBOX_Paint(HWND hWnd, HDC hdc, PLISTBOXDATA pListBoxData, 
                          PLISTNODE pOldCurNode, IWORD wOldIndex, IWORD wOldFirPOS, 
                          BOOL bClear, int nLineStart, int nLineEnd,
                          int nColStart, int nColEnd)
{
    RECT        rcList, rect, rect1, rcLine1, rcLine2;
    PLISTNODE   p;
    DWORD       dwStyle;
	WORD	    j, wPageCount;
    SIZE        size;
    int         nItemHeight, nLine1Height = 0, nLine2Height = 0,
                startIndex = -1, endIndex = -1, nPageTextLen, nOldBkMode,
                nTextWidth = 0, nLength = 0, nIndent = 0, nTextMaxWidth = 0;
    COLORREF    crTextOld;
    BOOL        bRealClr, bSetFont = FALSE;
    HBRUSH      hBrush;
    char        szTextDisplay[MAX_DISPLAYTEXT] = "";
    HFONT       hFont = NULL, hfontOld = NULL;
    BITMAP      bmp;
    
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    GetClientRect(hWnd, &rcList);
    SetRect(&rect, rcList.left, rcList.top, rcList.right, rcList.bottom);
    ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);

    GetTextExtentPoint32(hdc, "T", -1, &size);

    nItemHeight = GetItemHeight(hWnd, pListBoxData, &nLine1Height, &nLine2Height);

    if ((rect.bottom - rect.top) < nItemHeight)
        return;
    
    rect.bottom -= (rect.bottom - rect.top) % nItemHeight;

    if (dwStyle & LBS_MULTICOLUMN)
    {
        if (nColStart > 0)
            startIndex = nColStart * pListBoxData->wPageRange;

        if (nColEnd > 0)
            endIndex = nColEnd * pListBoxData->wPageRange;
    }
    
    if (nLineStart > 0)
    {
        if (startIndex >= 0)
            startIndex += nLineStart;
        else
            startIndex = nLineStart;
    }

    if (nLineEnd > 0)
    {
        if (endIndex >= 0)
            endIndex += nLineEnd;
        else
            endIndex = nLineEnd;
    }

    if (startIndex >= 0)
        j = startIndex;
    else
        j = 0;

    p = FindNode(pListBoxData, 
        (IWORD)(pListBoxData->wCurFirPOS + j));

    if (dwStyle & LBS_MULTICOLUMN)
    {
        rect1.left = rect.left + pListBoxData->wBitmapWidth[0] + 1;

        if (nColStart > 0)
            rect1.left += pListBoxData->nColumnWidth * nColStart;

        if (nLineStart > 0)
            rect1.top = rect.top + nItemHeight * nLineStart;
        else
            rect1.top = rect.top;
        
        rect1.right = rect1.left + pListBoxData->nColumnWidth - 
            pListBoxData->wBitmapWidth[0] - 1;
        if (rect1.right > rect.right)
            rect1.right = rect.right;
        
        wPageCount = pListBoxData->wPageRange * 
            ((rect.right - rect.left) / pListBoxData->nColumnWidth + 1);
    }
    else
    {
        rect1.left = rect.left - 
            size.cx * pListBoxData->wStartTextPos;
        rect1.top = rect.top + nItemHeight * j;
        
        rect1.right = rect.right;

        wPageCount = pListBoxData->wPageRange;
    }

    rect1.bottom = rect1.top + nItemHeight;

    if (dwStyle & LBS_MULTILINE)
    {
        rcLine1.left = rect1.left;
        rcLine1.top  = rect1.top;
        rcLine1.right = rect1.right;
        rcLine1.bottom = rcLine1.top + nLine1Height;

        rcLine2.left = rect1.left;
        rcLine2.top = rect1.bottom - nLine2Height;
        rcLine2.right = rect1.right;
        rcLine2.bottom = rect1.bottom;
    }

	while(p && (j < wPageCount) &&
		!(!(dwStyle & LBS_HASSTRINGS) && ((dwStyle & LBS_OWNERDRAWFIXED) ||
		(dwStyle & LBS_OWNERDRAWVARIABLE))))
	{
		if (startIndex >= 0 && j > endIndex)
			break;

		if (p->hImage[1] != NULL)
		{
			GetTextExtentPoint32(hdc, p->pNodeString, p->wStrLen, &size);
			if (size.cx > nTextMaxWidth)
			{
				nTextMaxWidth = size.cx;
			}
		}

		p = p->pNext;
        if (!p)
            break;
        j++;
	}
	
    if (startIndex >= 0)
        j = startIndex;
    else
        j = 0;
	
    p = FindNode(pListBoxData, 
        (IWORD)(pListBoxData->wCurFirPOS + j));
	
    bRealClr = FALSE;

    hBrush = SetCtlColorListBox(hWnd, hdc, pListBoxData);

    while((j < wPageCount) &&
        !(!(dwStyle & LBS_HASSTRINGS) && ((dwStyle & LBS_OWNERDRAWFIXED) ||
        (dwStyle & LBS_OWNERDRAWVARIABLE))))
    {
        if (startIndex >= 0 && j > endIndex)
            break;

        //Draw current selected item.
        if (p && ((pListBoxData->pCurNode == p && !pListBoxData->bMultiSelect) || 
            (pListBoxData->bMultiSelect && p->bSelect)))
        {
            /*if (pListBoxData->bRealFocus == TRUE)
                crBkOld = SetBkColor(hdc, GetWindowColor(FOCUS_COLOR));//GetSysColor(COLOR_HIGHLIGHT)BLUE
            else
                crBkOld = SetBkColor(hdc, GetWindowColor(CAP_COLOR));*/
//            crBkOld = SetBkColor(hdc, RGB(193, 213, 237));
            crTextOld = SetTextColor(hdc, COLOR_WHITE);//GetSysColor(COLOR_HIGHLIGHTTEXT)
            bRealClr = TRUE;
        }
        
        if (p && ((pListBoxData->wCurFirPOS != wOldFirPOS) ||
            (pListBoxData->pCurNode == p) || (pOldCurNode == p && 
            !pListBoxData->bMultiSelect)))
        {
            RECT    rcImage;
            IWORD   wStrLen;

            if (bRealClr || bClear)
            {
                if (bRealClr)
                {
                    GetObject(hListFocus, sizeof(BITMAP), (void*)&bmp);
                    if (dwStyle & LBS_MULTILINE)
                    {
                        StretchBlt(hdc, rect1.left + 1, rect1.top + 2, rect1.right - rect1.left - 2,
                            rect1.bottom - rect1.top - 4, (HDC)hListFocus, 0, 0, bmp.bmWidth,
                            bmp.bmHeight, SRCCOPY);
                    }
                    else
                    {
                        StretchBlt(hdc, rect1.left, rect1.top, rect1.right - rect1.left,
                            rect1.bottom - rect1.top, (HDC)hListFocus, 0, 0, bmp.bmWidth,
                            bmp.bmHeight, SRCCOPY);
                    }
                }
                else
                    FillRect(hdc, &rect1, hBrush);
            }            
            
            SetRect(&rcImage, rect1.left + CX_LEFTMARGIN,
                rect1.top + ((dwStyle & LBS_MULTILINE) ? CY_TOPMARGIN : 0),
                rect1.left + CX_LEFTMARGIN + CX_LEFTICON, rect1.bottom);
            
            if (p->hImage[0])
                Bitmap_Paint(hdc, p->hImage[0], &rcImage);
            else if (pListBoxData->wBitmapWidth[0] && pListBoxData->wBitmapHeight[0] )
            {
                //多选列表框的背景用白色。
//                if (pListBoxData->bMultiSelect)
//                {
//                    ClearRect(hdc, &rcImage, COLOR_WHITE);//GetSysColor(COLOR_HIGHLIGHTTEXT)
//                }
//                else
//                {
                    //FillRect(hdc, &rcImage, hBrush);
                    
//                    if (bRealClr)
//                    {
//                        /*if (pListBoxData->bRealFocus == TRUE)
//                            SetBkColor(hdc, GetWindowColor(FOCUS_COLOR));//GetSysColor(COLOR_HIGHLIGHT)
//                        else 
//                            SetBkColor(hdc, GetWindowColor(CAP_COLOR));*/
////                        SetBkColor(hdc, RGB(193, 213, 237));
////                        SetTextColor(hdc, COLOR_WHITE);//GetSysColor(COLOR_HIGHLIGHTTEXT)
//                    }
//                }
            }
            
            if (dwStyle & LBS_USERSTRINGS)
                wStrLen = p->wStrLen;
            else
                wStrLen = strlen(p->pNodeString);
            
//            if (dwStyle & LBS_MULTICOLUMN)
//            {
//                nPageTextLen = (rect1.right - rect1.left) / size.cx;
//                if ((rect1.right - rect1.left) % size.cx)
//                    nPageTextLen ++;
//            }
//            else
                nPageTextLen = pListBoxData->byPageTextLen;

            if ((wStrLen > pListBoxData->wStartTextPos)
                || (p->wAuxTextLen > pListBoxData->wStartTextPos))
            {
                nOldBkMode = SetBkMode(hdc, BM_TRANSPARENT);
                if (!bRealClr)
                    crTextOld = SetTextColor(hdc, RGB(1, 31, 65));

                nIndent = CX_LEFTMARGIN;
                if (pListBoxData->wBitmapWidth[0] != 0)
                {
                    nIndent += CX_LEFTICON;
                }

                if (!(dwStyle & LBS_MULTILINE))
                {
                    nTextWidth = rect1.right - (rect1.left + nIndent);
                    if (pListBoxData->wBitmapWidth[1] != 0)
                    {
                        nTextWidth -= CX_RIGHTICON + 2 * CX_RIGHTMARGIN;
                    }
                    GetTextExtentPoint32(hdc, p->pNodeString, p->wStrLen, &size);
                    GetExtentFittedText(hdc, p->pNodeString, p->wStrLen,
                        szTextDisplay, MAX_DISPLAYTEXT, nTextWidth, '.', 3);

                    TextOut(hdc, rect1.left + nIndent,
                        (rect1.bottom + rect1.top - size.cy) / 2,
                        szTextDisplay, -1);
                }
                else
                {
                    nTextWidth = rect1.right - (rect1.left + nIndent);

                    hFont = (HFONT)SendMessage(pListBoxData->hParentWnd, WM_COMMAND,
                        MAKEWPARAM(pListBoxData->wID, LBN_SETFONT), 0);

                    bSetFont = IsValidObject(hdc, hFont);

                    if (bSetFont)
                    {
                        hfontOld = SelectObject(hdc, (HGDIOBJ)hFont);
                    }

                    GetTextExtentPoint32(hdc, p->pNodeString, p->wStrLen, &size);
                    GetExtentFittedText(hdc, p->pNodeString, p->wStrLen,
                        szTextDisplay, MAX_DISPLAYTEXT, nTextWidth, '.', 3);

                    TextOut(hdc, rcLine1.left + nIndent,
                        (rcLine1.bottom + rcLine1.top 
                        - size.cy) / 2 + CY_TOPMARGIN, szTextDisplay, -1);

                    pListBoxData->xRightIcon = rcLine1.left + nIndent + nTextMaxWidth
                        + X_RIGHTICON;

                    if (bSetFont)
                    {
                        SelectObject(hdc, (HGDIOBJ)hfontOld);
                    }

                    if (p->pszAuxText != NULL)
                    {
                        hFont = (HFONT)SendMessage(pListBoxData->hParentWnd, WM_COMMAND,
                            MAKEWPARAM(pListBoxData->wID, LBN_SETFONT), 1);

                        bSetFont = IsValidObject(hdc, hFont);

                        if (!bSetFont)
                        {
                            GetFontHandle(&hFont, SMALL_FONT);
                        }

                        hfontOld = SelectObject(hdc, (HGDIOBJ)hFont);

                        nLength = min(p->wAuxTextLen, (int)strlen(p->pszAuxText));

                        GetTextExtentPoint32(hdc, p->pszAuxText, p->wAuxTextLen, &size);
                        GetExtentFittedText(hdc, p->pszAuxText, p->wAuxTextLen,
                            szTextDisplay, MAX_DISPLAYTEXT, nTextWidth, '.', 3);
                        TextOut(hdc, rcLine2.left + nIndent,
                            (rcLine2.bottom + rcLine2.top - size.cy) / 2 - CY_BTMMARGIN,
                            szTextDisplay, -1);

                        SelectObject(hdc, (HGDIOBJ)hfontOld);
                    }
                }
                SetBkMode(hdc, nOldBkMode);
                if (!bRealClr)
                    SetTextColor(hdc, crTextOld);
            }

            if (p->hImage[1] != NULL)
            {
                if (dwStyle & LBS_MULTILINE)
                {
                    SetRect(&rcImage, pListBoxData->xRightIcon, rect1.top + CY_TOPMARGIN,
                        pListBoxData->xRightIcon + CX_RIGHTICON, rect1.bottom);
                }
                else
                {
                    SetRect(&rcImage, rect1.right - CX_RIGHTICON - 2 * CX_RIGHTMARGIN,
                        rect1.top, rect1.right - CX_RIGHTMARGIN, rect1.bottom);
                }
                Bitmap_Paint(hdc, p->hImage[1], &rcImage);
            }
        }
        
#if (!NOKEYBOARD)
        //Begin to Erase Focus Frame.
        if (p)
        {
            /*if (bRealClr)
            {
                if (pListBoxData->bRealFocus == TRUE)
                    hBrushTmp = CreateSolidBrush(GetWindowColor(FOCUS_COLOR));//GetSysColor(COLOR_HIGHLIGHT)
                else
                    hBrushTmp = CreateSolidBrush(GetWindowColor(CAP_COLOR));
                SetRect(&rcTemp, rect1.left - 1 - pListBoxData->wBitmapWidth[0], 
                    rect1.top, rect1.right, rect1.bottom);
                
                if (!(dwStyle & LBS_MULTICOLUMN) && (dwStyle & WS_HSCROLL) &&
                    pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen)
                {
                    rcTemp.right ++;
                }
                FrameRect(hdc, &rcTemp, hBrushTmp);
                DrawLine(hdc, rcTemp.left, rcTemp.bottom - 1, rcTemp.right, rcTemp.bottom - 1);
                DrawLine(hdc, rcTemp.right - 1, rcTemp.top, rcTemp.right - 1, rcTemp.bottom);
                DeleteObject(hBrushTmp);
            }
            else
            {
                //left line.
                SetRect(&rcTemp, rect1.left - 1 - pListBoxData->wBitmapWidth[0], rect1.top, 
                    rect1.left - pListBoxData->wBitmapWidth, rect1.bottom);
                FillRect(hdc, &rcTemp, hBrush);
                //top line.
                SetRect(&rcTemp, rect1.left - pListBoxData->wBitmapWidth, rect1.top, 
                    rect1.right, rect1.top + 1);
                FillRect(hdc, &rcTemp, hBrush);
                
                //right line.
                if (!(!(dwStyle & LBS_MULTICOLUMN) && (dwStyle & WS_HSCROLL) &&
                    pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen) &&
                    rect1.right <= rect.right)
                {
                    SetRect(&rcTemp, rect1.right - 1, rect1.top, 
                        rect1.right, rect1.bottom);
                    FillRect(hdc, &rcTemp, hBrush);
                }

                //bottom line.
                SetRect(&rcTemp, rect1.left - pListBoxData->wBitmapWidth, rect1.bottom - 1, 
                    rect1.right, rect1.bottom);
                FillRect(hdc, &rcTemp, hBrush);
            }*/
        }
        //End of Erase Focus Frame.
        
        //Draw dot frame for current selected item.
        //When item count is 0, listbox draw dot frame, but comboBox not.
        if (((pListBoxData->pCurNode == p) || (pListBoxData->wIndex == 
            (j + pListBoxData->wCurFirPOS)) || (pOldCurNode == p) ||
            ((p == pListBoxData->pHead) && (pListBoxData->wVirtFocus == 0))) &&
            (pListBoxData->bFocus ||
            ((pListBoxData->wItemCount > 0) && (dwStyle & LBS_COMBOLBOX))) &&
            (pListBoxData->pCurNode != NULL))
        {
            if (p != NULL || j == 0)
            {
                RECT    rcFocus;
                
                SetRect(&rcFocus, rect1.left - pListBoxData->wBitmapWidth[0] - 1, 
                    rect1.top, rect1.right, rect1.bottom);
                
                if (dwStyle & LBS_MULTICOLUMN)
                {
                    rcFocus.right = rcFocus.left + pListBoxData->nColumnWidth;
                    
                    //it maybe draw out of client rect, but this is OK because 
                    //sometimes it is LBS_MULTICOLUMN with WS_HSCROLL. 
                    //Or the scroll has error. According MS Windows.
                    if (rcFocus.right > rect.right)
                        rcFocus.right = rect.right + 1;
                }
                else if ((dwStyle & WS_HSCROLL) &&
                    pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen)
                    rcFocus.right ++;
                if (pListBoxData->bMultiSelect)
                    DrawFocus(hdc, &rcFocus);
            }
        }
        
        if ((dwStyle & LBS_MULTICOLUMN) && j >= pListBoxData->wPageRange * 
            (pListBoxData->wColumns - 1) && !pListBoxData->wStartTextPos)
        {
            Draw3DListBoxRect_W95(pListBoxData, hdc, &rcList, 
                pListBoxData->bVScroll, pListBoxData->bHScroll, dwStyle);
        }
#endif

        if (!p)
            break;

        if (bRealClr)
        {
 //           SetBkColor(hdc, crBkOld);
            SetTextColor(hdc, crTextOld);
        }
        bRealClr = FALSE;
        
        p = p->pNext;
        if (!p)
            break;
        j++;
        
        if ((dwStyle & LBS_MULTICOLUMN) &&
            !((j - pListBoxData->wCurFirPOS) % pListBoxData->wPageRange))
        {
            rect1.left += pListBoxData->nColumnWidth;
            rect1.right = rect1.left + pListBoxData->nColumnWidth - 
                pListBoxData->wBitmapWidth[0] - 1;
            
            if (rect1.right > rect.right)
                rect1.right = rect.right;

            rect1.top = rect.top;
            rect1.bottom = rect1.top + nItemHeight;
        }
        else
        {
            rect1.top += nItemHeight;
            rect1.bottom += nItemHeight;
        }

        if (dwStyle & LBS_MULTILINE)
        {
            rcLine1.top = rect1.top;
            rcLine1.bottom = rect1.top + nLine1Height;

            rcLine2.top = rect1.bottom - nLine2Height;
            rcLine2.bottom = rect1.bottom;
        }
    }

    if (pListBoxData->wStartTextPos)
    {
        Draw3DListBoxRect_W95(pListBoxData, hdc, &rcList, 
            pListBoxData->bVScroll, pListBoxData->bHScroll, dwStyle);
   }
}

static void Bitmap_Paint(HDC hdc, HANDLE hImage, RECT* pRect)
{
    char PathName[256];
    int nPathLen, nBkMode;
    COLORREF BkColor;

    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);                    
    strcat(PathName, "iconbk.bmp");
    
    BkColor = SetBkColor(hdc, RGB(255, 255, 255));
    nBkMode = SetBkMode(hdc, BM_NEWTRANSPARENT);
    DrawBitmapFromFile(hdc, pRect->left, pRect->top + 2, 
        PathName, ROP_SRC);
    SetBkMode(hdc, nBkMode);
    SetBkColor(hdc, BkColor);

    if (hImage)
    {
        nBkMode = SetBkMode(hdc, BM_NEWTRANSPARENT);
        BkColor = SetBkColor(hdc, COLOR_BLACK);
        BitBlt(hdc, pRect->left + 1, pRect->top + 6, pRect->right - pRect->left,
            pRect->bottom - pRect->top,
            (HDC)hImage, 0, 0, ROP_SRC);
        SetBkColor(hdc, BkColor);
        SetBkMode(hdc, nBkMode);
    }
}

#if (!NOKEYBOARD)
static void SetAndLeaveFocus(HWND hWnd, HDC hdc, PLISTBOXDATA pListBoxData)
{
    RECT    rect;
    int     nItemHeight;
//    SIZE    size;
    IWORD   j;
    DWORD   dwStyle;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    GetClientRect(hWnd, &rect);
    ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
    
//    GetTextExtentPoint32(hdc, "T", -1, &size);
//    nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);
    nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);

    if ((rect.bottom - rect.top) < nItemHeight)
        return;

    j = pListBoxData->wIndex;

    if (j < 0)
    {
        j = pListBoxData->wVirtFocus = 0;
        //If there is no current node, then let the first item SetFocus.
    }
    else
        pListBoxData->wVirtFocus = -1;
    
    if (j != pListBoxData->wItemCount && (j < pListBoxData->wCurFirPOS || 
        j >= (pListBoxData->wCurFirPOS + pListBoxData->wPageRange)))
    {
        return;
    }

    if (dwStyle & LBS_MULTICOLUMN)
    {
        int line, col;

        line = (j - pListBoxData->wCurFirPOS) % pListBoxData->wPageRange;
        col = (j - pListBoxData->wCurFirPOS) / pListBoxData->wPageRange;

        rect.left += col * pListBoxData->nColumnWidth;
        rect.top += line * nItemHeight;

        rect.right = rect.left + pListBoxData->nColumnWidth;
        rect.bottom = rect.top + nItemHeight;
    }
    else
    {
        rect.top += (j - pListBoxData->wCurFirPOS) * nItemHeight;
        rect.bottom = rect.top + nItemHeight;
    }

    if (!(dwStyle & LBS_MULTICOLUMN) && (dwStyle & WS_HSCROLL) &&
        pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen)
    {
        rect.right ++;
    }
    if (pListBoxData->bMultiSelect)
        DrawFocus(hdc, &rect);
}

static void DrawFocus(HDC hdc, RECT* pRect)
{
//#ifndef NOFOCUSRECT
	HBRUSH  hBrush;
    int     fnOldMode;
    COLORREF crOld;

    crOld = SetBkColor(hdc, COLOR_BLACK);//GetSysColor(COLOR_HIGHLIGHT));//COLOR_BLACK

    fnOldMode = SetROP2(hdc, ROP_SRC_XOR_DST);
    
    hBrush = CreateHatchBrush(HS_SIEVE, COLOR_WHITE);//GetSysColor(COLOR_HIGHLIGHTTEXT));
    FrameRect(hdc, pRect, hBrush);

    DeleteObject(hBrush);
    
    SetROP2(hdc, fnOldMode);
    SetBkColor(hdc, crOld);
//#endif
}
#endif // NOKEYBOARD

char * LocalString( int nSize)
{
    char* pTemp; 
    pTemp = (char*)LocalAlloc(LMEM_FIXED, (nSize / 100 + 1) * STR_MAXLENGTH);//changed 7/13
    return pTemp;
}

// Create a new node
static PLISTNODE NewNode(HWND hWnd, PLISTBOXDATA pListBoxData, const void* pData, 
                         BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings)
{
    PLISTNODE   p;
    int         nLength = 0;
    DWORD       dwStyle = 0;
    RECT        rcClient;
    int         width, len = 0;
    SIZE        size;
    int         i = 0;
    HDC         hdc;
    //BOOL        bDb;
    
	if (pListBoxData->bBeginInit)
    {
        if (pListBoxData->wCacheCount)
        {
            pListBoxData->wCacheCount --;
            p = (LISTNODE *)(pListBoxData->pNodeCache + pListBoxData->wCacheCount);
            ASSERT(p != NULL);
            
            memset(p, 0, sizeof(LISTNODE));
            p->bCache = TRUE;
        }
        else
        {
            p = (LISTNODE *)LocalAlloc(LMEM_FIXED, sizeof(LISTNODE));
            if (p == NULL)
                return NULL;
            memset(p, 0, sizeof(LISTNODE));
			p->bCache = FALSE;
        }
    }
    else
    {
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (!(dwStyle & WS_HSCROLL))
        {
            GetClientRect(hWnd, &rcClient);
            if(pListBoxData->wBitmapWidth[0])
                width = rcClient.right - rcClient.left - 39;
            else
		width = rcClient.right - rcClient.left - 3;

            hdc = GetDC(hWnd);
            GetTextExtentPoint32(hdc, "T", -1, &size);
            ReleaseDC(hWnd, hdc);
            for (i = 0; i < (int)strlen((LPSTR)pData); i++)
            {
                if ((*((LPSTR)pData + i) > 0xa1) && (*((LPSTR)pData + i + 1) > 0xa1))
                {
                    len += size.cx * 2;
                    if (len > width)
                        break;
                    i = i + 1;
                    //bDb = TRUE;
                    
                }
                else
                {
                    len += size.cx;
                    //bDb = FALSE;
                    if (len > width)
                        break;
                }
            }
            p = (LISTNODE *)LocalAlloc(LMEM_FIXED, sizeof(LISTNODE)+strlen((LPSTR)pData) + 1);
            if (p == NULL)
                return NULL;
            memset(p, 0, sizeof(LISTNODE));
            p->bCache = FALSE;
        }
        else
        {
            p = (LISTNODE *)LocalAlloc(LMEM_FIXED, sizeof(LISTNODE) + 
                strlen((LPSTR)pData) + 1);
            if (p == NULL)
                return NULL;
            memset(p, 0, sizeof(LISTNODE));
            p->bCache = FALSE;
        }
    }

    if (bStringNode) 
    {
        if(bUserStrings)
        {
            p->pNodeString = (PSTR)pData;
            wStrLen = strlen((LPSTR)pData);
            nLength = p->wStrLen = wStrLen;
			p->bStringCache = 1;
        }
        else if(pListBoxData->bBeginInit)
        {
            nLength = strlen((LPSTR)pData);
            
            if (pListBoxData->pCurString + nLength + 1 < pListBoxData->pStringCache
                + (pListBoxData->wStringCount/100 + 1) * STR_MAXLENGTH)
            {
                p->pNodeString = pListBoxData->pCurString;
                pListBoxData->pCurString += (nLength+1);
				p->bStringCache = 1;
            }
            else
			{
                p->pNodeString = (PSTR)LocalAlloc(LMEM_FIXED, strlen((LPSTR)pData) + 1);
				p->bStringCache = 0;
			}            
            memcpy(p->pNodeString, pData, nLength);
            *((PSTR)p->pNodeString + nLength) = 0;
            p->wStrLen = nLength;
        }
        else
        {
            if ((dwStyle & WS_HSCROLL) || (i == (int)strlen((LPSTR)pData)))
            {
                nLength = strlen((LPSTR)pData);
                p->pNodeString = (PSTR)(p + 1);
                memcpy(p->pNodeString, pData, nLength);
                *((PSTR)p->pNodeString + nLength) = 0;
                p->wStrLen = nLength;
                p->bStringCache = 1;
            }
            else
            {
                nLength = strlen((LPSTR)pData);
                
                p->pNodeString = (PSTR)(p + 1);
                memcpy(p->pNodeString, pData, nLength);
                *(p->pNodeString + nLength) = 0;
                p->wStrLen = nLength;
                p->bStringCache = 1;
            }
        }
        if (nLength >= pListBoxData->wMaxTextLen)
            pListBoxData->wMaxTextLen = nLength + 1;
    }
    else
        p->dwNodeData = (DWORD)pData;
    return p;
}

static void DeleteNode(PLISTNODE p)
{
    ASSERT(p);

    if (!p->bStringCache)
		LocalFree((HANDLE)p->pNodeString);

    if (p->pszAuxText != NULL)
        LocalFree((HANDLE)p->pszAuxText);

    if (!p->bCache)
        LocalFree((HANDLE)p);
}

//static void InitList(PLISTBOXDATA p)
//{
//    p->pHead = NULL;
//    p->pTail = NULL;
//
//    return;
//}

static void FreeList(PLISTBOXDATA p)
{
    PLISTNODE s;

    if (p->pHead == NULL)
    {
        ASSERT(p->pTail == NULL);
        return;
    }

    while (p->pHead != NULL)
    {
        s = p->pHead;
        p->pHead = p->pHead->pNext;

        DeleteNode(s);
    }

    p->pHead = NULL;
    p->pTail = NULL;
    p->wItemCount = 0;
    p->wIndex = -1;

    return;
}
//changed 7/16
static void FreeCache(PLISTBOXDATA p)
{
    if (p->pNodeCache)
	{
        LocalFree((HANDLE)p->pNodeCache);
		p->pNodeCache = NULL;
		p->wCacheCount = 0;
	}

    if (p->pStringCache)
    {
		LocalFree((HANDLE)p->pStringCache);
		p->pStringCache = NULL;
		p->wStringCount = 0;
	}
	return ;
}


static void RemoveHead(PLISTBOXDATA p)
{
    PLISTNODE s;

    if (p->pHead == NULL)
        return;

    s = p->pHead;

    p->pHead = s->pNext;
    if (p->pHead)
        p->pHead->pPrev = NULL;
    else
        p->pTail = NULL;

    DeleteNode(s);

    return;
}

static void RemoveTail(PLISTBOXDATA p)
{
    PLISTNODE s;

    if (p->pTail == NULL)
        return;

    s = p->pTail;

    p->pTail = s->pPrev;
    if (p->pTail)
        p->pTail->pNext = NULL;
    else
        p->pHead = NULL;

    DeleteNode(s);

    return;
}

static BOOL AddHead(HWND hWnd, PLISTBOXDATA p, const void* pData, 
                    BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings)
{
    PLISTNODE s;

    s = NewNode(hWnd, p, pData, bStringNode, wStrLen, bUserStrings);
    if (s == NULL)
        return FALSE;

    s->pNext = p->pHead;

    if (p->pHead)
        p->pHead->pPrev = s;
    else
        p->pTail = s;
    
    s->pPrev = NULL;
    p->pHead = s;
    
    return TRUE;
}

static BOOL AddTail(HWND hWnd, PLISTBOXDATA p, const void* pData, 
                    BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings)
{
    PLISTNODE s;

    s = NewNode(hWnd, p, pData, bStringNode, wStrLen, bUserStrings);

    if (s == NULL)
        return FALSE;

    s->pPrev = p->pTail;

    if (p->pTail)
        p->pTail->pNext = s;
    else
        p->pHead = s;

    s->pNext = NULL;
    p->pTail = s;

    return TRUE;
}

static BOOL InsertNode(HWND hWnd, PLISTBOXDATA pListBoxData, const void* pData, int nPos, 
                       BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings)
{
    PLISTNODE p, s;

    ASSERT(nPos >= 0 && nPos <= pListBoxData->wItemCount);

    if (nPos == 0)
        return AddHead(hWnd, pListBoxData, pData, bStringNode, wStrLen, bUserStrings);

    if (nPos == pListBoxData->wItemCount)
        return AddTail(hWnd, pListBoxData, pData, bStringNode, wStrLen, bUserStrings);

    p = FindNode(pListBoxData, (IWORD)(nPos - 1));
    ASSERT(p != NULL);

    s = NewNode(hWnd, pListBoxData, pData, bStringNode, wStrLen, bUserStrings);
    if (s == NULL)
        return FALSE;

    s->pNext = p->pNext;
    p->pNext->pPrev = s;
    p->pNext = s;
    s->pPrev = p;

    return TRUE;
}

static int InsertNodeSort(HWND hWnd, PLISTBOXDATA pListBoxData, const void* pData, 
                       BOOL bStringNode, IWORD wStrLen, BOOL bUserStrings)
{
	int  nInsertPos = 0, f, l;
	const char * src;
    char *dst ;
	PLISTNODE	p, s;
	
	p = pListBoxData->pHead;

	while(p)
	{
		dst = p->pNodeString;
		src = pData;

		do
		{
			if ( ((f = (unsigned char)(*(dst++))) >= 'A') && (f <= 'Z') )
				f -= ('A' - 'a');
			
			if ( ((l = (unsigned char)(*(src++))) >= 'A') && (l <= 'Z') )
				l -= ('A' - 'a');
		} while ( f && (f == l) );
		
        if ( (f - l) >= 0 )
			break;
		nInsertPos++;
		p = p->pNext;
	}
    
	s = NewNode(hWnd, pListBoxData, pData, bStringNode, wStrLen, bUserStrings);
    if (s == NULL)
        return -1;

	if (!nInsertPos)
	{
		s->pNext = pListBoxData->pHead;
		
		if (pListBoxData->pHead)
			pListBoxData->pHead->pPrev = s;
		else
			pListBoxData->pTail = s;
		
		s->pPrev = NULL;
		pListBoxData->pHead = s;
		return nInsertPos;
	}

	if (nInsertPos == pListBoxData->wItemCount)
	{
		s->pPrev = pListBoxData->pTail;
		
		if (pListBoxData->pTail)
			pListBoxData->pTail->pNext = s;
		else
			pListBoxData->pHead = s;
		
		s->pNext = NULL;
		pListBoxData->pTail = s;
		return nInsertPos;
	}
	p = p->pPrev;
    s->pNext = p->pNext;
    p->pNext->pPrev = s;
    p->pNext = s;
    s->pPrev = p;

    return nInsertPos;
}

static void RemoveNode(PLISTBOXDATA pListBoxData, int nPos)
{
    PLISTNODE p;
    WORD i;

    if (pListBoxData->pHead == NULL)
        return;

    if (nPos == 0)
    {
        if (pListBoxData->wMaxTextLen == (pListBoxData->pHead->wStrLen + 1))
        {
            PLISTNODE q;
            
            q = pListBoxData->pHead->pNext;
            if (q != NULL)
                pListBoxData->wMaxTextLen = q->wStrLen + 1;
            else
                pListBoxData->wMaxTextLen = 0;
            while (q != NULL)
            {
                if (q->wStrLen >= pListBoxData->wMaxTextLen)
                    pListBoxData->wMaxTextLen = q->wStrLen + 1;
                q = q->pNext;
            }
        }
        RemoveHead(pListBoxData);
        return;
    }

    p = pListBoxData->pHead;

    for (i = 0; i < nPos; i++)
    {
        if (p == NULL)
            break;

        p = p->pNext;
    }

    if (p->pNext == NULL)
    {
        if (pListBoxData->wMaxTextLen == (p->wStrLen + 1))
        {
            PLISTNODE q;
            
            q = pListBoxData->pHead;
            if (q)
            {
                pListBoxData->wMaxTextLen = q->wStrLen + 1;
                while (q != pListBoxData->pTail)
                {
                    if (q->wStrLen >= pListBoxData->wMaxTextLen)
                        pListBoxData->wMaxTextLen = q->wStrLen + 1;
                    q = q->pNext;
                }
            }
            else
                pListBoxData->wMaxTextLen = 0;  
        }
        RemoveTail(pListBoxData);
        return;
    }

    if (p->pPrev)
        p->pPrev->pNext = p->pNext;
    
    if (p->pNext)
        p->pNext->pPrev = p->pPrev;

    if (pListBoxData->wMaxTextLen == (p->wStrLen + 1))
    {
        PLISTNODE q;
        
        q = pListBoxData->pHead;
        if (q)
        pListBoxData->wMaxTextLen = q->wStrLen + 1;
        while (q != NULL)
        {
            if (q->wStrLen >= pListBoxData->wMaxTextLen)
                pListBoxData->wMaxTextLen = q->wStrLen + 1;
            q = q->pNext;
        }
    }

    DeleteNode(p);

    return;
}

static PLISTNODE FindNode(PLISTBOXDATA pListBoxData, IWORD wIndex)
{
    PLISTNODE   p;
    int         i = 0;
    
    if (wIndex >= pListBoxData->wItemCount || wIndex < 0)
        return NULL;

    if (wIndex == (pListBoxData->wItemCount - 1))
        return pListBoxData->pTail;

    if (pListBoxData->pCurNode && (wIndex == pListBoxData->wIndex))
        return pListBoxData->pCurNode;

    if (pListBoxData->pCurNode && pListBoxData->wVirtFocus == -1)
    {
        if (wIndex <= pListBoxData->wIndex)
        {
            if (wIndex <= pListBoxData->wIndex / 2)
            {
                p = pListBoxData->pHead;
                while ((p != NULL) && (wIndex > 0))
                {
                    p = p->pNext;
                    wIndex--;
                }
            }
            else
            {
                p = pListBoxData->pCurNode;
                while ((p != NULL) && (pListBoxData->wIndex - i > wIndex))
                {
                    p = p->pPrev;
                    i++;
                }
            }
        }
        else
        {
            if (wIndex < pListBoxData->wIndex + (pListBoxData->wItemCount - 1 -
                pListBoxData->wIndex) / 2)
            {
                p = pListBoxData->pCurNode;
                while ((p != NULL) && (pListBoxData->wIndex + i < wIndex))
                {
                    p = p->pNext;
                    i++;
                }
            }
            else
            {
                p = pListBoxData->pTail;
                while ((p != NULL) && (pListBoxData->wItemCount - 1 - i > wIndex))
                {
                    p = p->pPrev;
                    i++;
                }
            }
        }
    }
    else
    {
        if (wIndex <= (pListBoxData->wItemCount - 1) / 2)
        {
            p = pListBoxData->pHead;
            while ((p != NULL) && (wIndex > 0))
            {
                p = p->pNext;
                wIndex--;
            }
        }
        else
        {
            p = pListBoxData->pTail;
            while ((p != NULL) && (pListBoxData->wItemCount - 1 - i > wIndex))
            {
                p = p->pPrev;
                i++;
            }
        }
    }

    return p;
}

//static IWORD CacuCurNodeIndex(PLISTBOXDATA pListBoxData)
//{
//    PLISTNODE p;
//    IWORD i;
//
//    if (pListBoxData->pCurNode == NULL)
//        return -1;
//
//    p = pListBoxData->pHead;
//    i = 0;
//    
//    while ((p != NULL) && (p != pListBoxData->pCurNode))
//    {
//        p = p->pNext;
//        i++;
//    }
//    
//    if (i >= pListBoxData->wItemCount)
//        i = -1;
//    return i;
//}

#if (!NOKEYBOARD)
static void ProcessKeyDown(HWND hWnd, PLISTBOXDATA pListBoxData, WPARAM wParam, LPARAM lParam)
{
    IWORD       wOldCurFirstPOS, wOldIndex;
    BOOL        bRepaint;
    HDC         hdc;
    SCROLLINFO  ScrollInfo;
    PLISTNODE   pOldCurNode = pListBoxData->pCurNode;
    DWORD       dwStyle;
    int         i;
    WORD        wVirtKey = 0;

	if (pListBoxData->bPenDown)
		return;

    wVirtKey = LOWORD(wParam);
    
    if ((pListBoxData->nRepeats > 0) && (wVirtKey != pListBoxData->wKeyCode))
    {
        KillTimer(hWnd, IDT_REPEAT);
        pListBoxData->nRepeats = 0;
    }
    
    pListBoxData->wKeyCode = wVirtKey;
    pListBoxData->nRepeats++;

    wOldCurFirstPOS = pListBoxData->wCurFirPOS;
    wOldIndex = pListBoxData->wIndex;
    bRepaint = FALSE;
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    
	if (dwStyle & LBS_MULTICOLUMN)
        return;
    
    switch ((WORD)wParam)
    {
    default :
        break;

    case VK_SPACE:
        if (pListBoxData->bMultiSelect)
        {
            LRESULT lsel;
            lsel = SendMessage(hWnd, LB_GETSEL, pListBoxData->wIndex, 0);
            if (lsel)
                SendMessage(hWnd, LB_SETSEL, (WPARAM)FALSE, (LPARAM)pListBoxData->wIndex);
            else
                SendMessage(hWnd, LB_SETSEL, (WPARAM)TRUE, (LPARAM)pListBoxData->wIndex);
        }
        break;

    case VK_UP:

        if (!pListBoxData->bRealFocus)
        {
            if(pListBoxData->hParentWnd)
            {
                if (pListBoxData->bIsNotSysCtrl)
                {
                    SendMessage(pListBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
                    return;
                }
                SetFocus(GetNextDlgTabItem(pListBoxData->hParentWnd, hWnd, TRUE));
                return;
            }
        }
        if (pListBoxData->bMultiSelect)
        {
            SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)hWnd);
        }

        if (!pListBoxData->pCurNode)
        {
            if (pListBoxData->wIndex >= 0)
                pListBoxData->pCurNode = FindNode(pListBoxData, pListBoxData->wIndex);
            else
            {
                pListBoxData->pCurNode = FindNode(pListBoxData, 
                    pListBoxData->wCurFirPOS);
                pListBoxData->wIndex = pListBoxData->wCurFirPOS;
            }
            
            if (pListBoxData->pCurNode)
                bRepaint = TRUE;
            break;
        }
        
        if (pListBoxData->pCurNode->pPrev)
        {
            bRepaint = TRUE;
            pListBoxData->pCurNode = pListBoxData->pCurNode->pPrev;

            pListBoxData->wIndex --;
        }
        else
        {
            bRepaint = TRUE;
            pListBoxData->pCurNode = pListBoxData->pTail;
            pListBoxData->wIndex = pListBoxData->wItemCount - 1;
        }
        if (dwStyle & LBS_MULTICOLUMN)
        {
        }
        else
        {
            if ((pListBoxData->wIndex < pListBoxData->wCurFirPOS + LINE_PRESCROLL)
                && (pListBoxData->wIndex > 0))
            {
                pListBoxData->wCurFirPOS = pListBoxData->wIndex - LINE_PRESCROLL;
            }
            else if (pListBoxData->wItemCount > pListBoxData->wPageRange)
            {
                if (pListBoxData->wIndex > 
                    (pListBoxData->wCurFirPOS + pListBoxData->wPageRange - 1))
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                        pListBoxData->wPageRange + 1;
                }
            }
        }
        if ((pListBoxData->pCurNode != NULL) && 
            (pListBoxData->pHead != pListBoxData->pTail))
        {
            if ((pListBoxData->pHead  != pListBoxData->pTail))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLDOWN | SCROLLUP | SCROLLMIDDLE, MASKALL);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if ((pListBoxData->bHScroll) &&
                (pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
            else if (!(pListBoxData->bHScroll) && 
                (pListBoxData->wItemCount > pListBoxData->wPageRange))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
        }
        if (pListBoxData->nRepeats == 1)
        {
            SetTimer(hWnd, IDT_REPEAT, ET_REPEAT_FIRST, NULL);
        }
        break;
        
    case VK_DOWN:
        if (!pListBoxData->bRealFocus)
        {
            if (pListBoxData->hParentWnd)
            {
                if (pListBoxData->bIsNotSysCtrl)
                {
                    SendMessage(pListBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
                    return;
                }
                SetFocus(GetNextDlgTabItem(pListBoxData->hParentWnd, hWnd, FALSE));
                return;
            }
        }
        if (pListBoxData->bMultiSelect)
        {
            SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)hWnd);
        }

        if (!pListBoxData->pCurNode)
        {
            if (pListBoxData->wIndex >= 0)
                pListBoxData->pCurNode = FindNode(pListBoxData, pListBoxData->wIndex);
            else
            {
                pListBoxData->pCurNode = FindNode(pListBoxData, 
                    pListBoxData->wCurFirPOS);
                pListBoxData->wIndex = pListBoxData->wCurFirPOS;
            }
            if ((pListBoxData->pCurNode != NULL) && 
                (pListBoxData->pHead != pListBoxData->pTail))
            {
                if ((pListBoxData->pHead  != pListBoxData->pTail))
                    SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLDOWN | SCROLLUP | SCROLLMIDDLE, MASKALL);
                dwStyle = GetWindowLong(hWnd, GWL_STYLE);
                if ((pListBoxData->bHScroll) &&
                    (pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen))
                    SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
                else if (!(pListBoxData->bHScroll) && 
                (pListBoxData->wItemCount > pListBoxData->wPageRange))
                    SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
            }

            if (pListBoxData->pCurNode)
                bRepaint = TRUE;
            break;
        }

        if (pListBoxData->pCurNode->pNext)
        {
            bRepaint = TRUE;
            pListBoxData->pCurNode = pListBoxData->pCurNode->pNext;

            pListBoxData->wIndex ++;           
        }
        else
        {
            bRepaint = TRUE;
            pListBoxData->pCurNode = pListBoxData->pHead;
            pListBoxData->wIndex = 0;
        }
        if (dwStyle & LBS_MULTICOLUMN)
        {
        }
        else
        {
            if (pListBoxData->wPageRange <= 0)
                break;
            
            if ((pListBoxData->wIndex >= 
                pListBoxData->wCurFirPOS + pListBoxData->wPageRange - LINE_PRESCROLL)
                && (pListBoxData->wIndex < pListBoxData->wItemCount - LINE_PRESCROLL))
            {
                pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                    pListBoxData->wPageRange + 1 + LINE_PRESCROLL;
            }
            else if (pListBoxData->wIndex < pListBoxData->wCurFirPOS)
                pListBoxData->wCurFirPOS = pListBoxData->wIndex;
        }

        if ((pListBoxData->pCurNode != NULL) && 
            (pListBoxData->pHead != pListBoxData->pTail))
        {
            if ((pListBoxData->pHead  != pListBoxData->pTail))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLDOWN | SCROLLUP | SCROLLMIDDLE, MASKALL);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if ((pListBoxData->bHScroll) &&
                (pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
            else if (!(pListBoxData->bHScroll) && 
                (pListBoxData->wItemCount > pListBoxData->wPageRange))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
        }
        if (pListBoxData->nRepeats == 1)
        {
            SetTimer(hWnd, IDT_REPEAT, ET_REPEAT_FIRST, NULL);
        }
        break;

    case VK_F5:
        if (pListBoxData->bIsSingle && !pListBoxData->bMultiSelect)
        {
            SendMessage(pListBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
            return;
        }
        else if (!pListBoxData->bRealFocus)
        {
            pListBoxData->bRealFocus = !pListBoxData->bRealFocus;
            if ((pListBoxData->pCurNode != NULL) &&
                (pListBoxData->pHead != pListBoxData->pTail))
            {
                if ((pListBoxData->pHead  != pListBoxData->pTail))
                    SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLDOWN | SCROLLUP | SCROLLMIDDLE, MASKALL);
                dwStyle = GetWindowLong(hWnd, GWL_STYLE);
                if ((pListBoxData->bHScroll) &&
                    (pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen))
                    SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
                else if (!(pListBoxData->bHScroll) && 
                (pListBoxData->wItemCount > pListBoxData->wPageRange))
                    SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                    SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
            }
            if (LB_ERR == SendMessage(hWnd, LB_GETCURSEL, 0, 0))
                SendMessage(hWnd, LB_SETCURSEL, 0, 0);
            else
            {
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        else
        {
            if (dwStyle & LBS_MULTIPLESEL)
            {
                SendMessage(hWnd, WM_KEYDOWN, VK_SPACE, lParam);
                return;
            }
            if (pListBoxData->hParentWnd && (pListBoxData->bIsSingle == 0))
            {
                pListBoxData->bRealFocus = !pListBoxData->bRealFocus;
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                    SCROLLUP | SCROLLDOWN | SCROLLMIDDLE, MASKALL);
                return;
            }
            if (pListBoxData->bIsSingle)
                SendMessage(pListBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
        }
        break;

    case VK_PAGEUP:

        //For a multiple-selection list box, the LBN_SELCHANGE notification is sent 
        //whenever the user presses an arrow key, even if the selection does not change. 
        if ((dwStyle & WS_HSCROLL) && ((WORD)wParam == VK_LEFT))
        {
            SendMessage(hWnd, WM_HSCROLL, SB_PAGEUP, NULL);
            return;
        }
        pListBoxData->bRealFocus = TRUE;
        InvalidateRect(hWnd, NULL, TRUE);        

        if (!pListBoxData->pCurNode)
        {
            if (pListBoxData->wIndex >= 0)
                pListBoxData->pCurNode = FindNode(pListBoxData, pListBoxData->wIndex);
            else
            {
                pListBoxData->pCurNode = FindNode(pListBoxData, 
                    pListBoxData->wCurFirPOS);
                pListBoxData->wIndex = pListBoxData->wCurFirPOS;
            }

            if (pListBoxData->pCurNode)
                bRepaint = TRUE;
            break;
        }

        if (dwStyle & LBS_MULTICOLUMN)
        {
        }
        else
        {
            if (pListBoxData->wItemCount > pListBoxData->wPageRange)
            {
                bRepaint = TRUE;
                

                if (pListBoxData->wIndex == 0)
                {
                    pListBoxData->wIndex = pListBoxData->wItemCount - 1;
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex - 
                        pListBoxData->wPageRange + 1;
                    
                    pListBoxData->pCurNode = pListBoxData->pTail;

                }
                else if ((pListBoxData->wIndex - pListBoxData->wPageRange + 1) <= 0)
                {
                    pListBoxData->wCurFirPOS = 0;
                    pListBoxData->wIndex = 0;
                    pListBoxData->pCurNode = pListBoxData->pHead;
                }
                else
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex - 
                        pListBoxData->wPageRange + 1;
                    pListBoxData->wIndex = pListBoxData->wCurFirPOS;
                    
                    for (i = 0; i < pListBoxData->wPageRange - 1; i++)
                    {
                        if (pListBoxData->pCurNode)
                            pListBoxData->pCurNode = pListBoxData->pCurNode->pPrev;
                    }
                }
            }
            else //if (pListBoxData->wIndex != 0)
            {
             /*   bRepaint = TRUE;
                
                pListBoxData->pCurNode = pListBoxData->pHead;
                pListBoxData->wIndex = 0;*/
                break;
            }
        }
        if ((pListBoxData->pHead  != pListBoxData->pTail))
            SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
            SCROLLDOWN | SCROLLUP | SCROLLMIDDLE, MASKALL);
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if ((pListBoxData->bHScroll) &&
            (pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen))
            SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
            SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
        else if (!(pListBoxData->bHScroll) && 
                (pListBoxData->wItemCount > pListBoxData->wPageRange))
            SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
            SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
        if (pListBoxData->bMultiSelect)
        {
            SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)hWnd);
        }
        
        break;

    case VK_PAGEDOWN:

        if (!pListBoxData->bRealFocus)
        {
            if (pListBoxData->hParentWnd)
            {
                if (pListBoxData->bIsNotSysCtrl)
                {
                    SendMessage(pListBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
                    return;
                }
            }
        }

        //For a multiple-selection list box, the LBN_SELCHANGE notification is sent 
        //whenever the user presses an arrow key, even if the selection does not change. 
        if ((dwStyle & WS_HSCROLL) && ((WORD)wParam == VK_RIGHT))
        {
            SendMessage(hWnd, WM_HSCROLL, SB_PAGEDOWN, NULL);
            return;
        }
        pListBoxData->bRealFocus = TRUE;
        InvalidateRect(hWnd, NULL, TRUE);
        if (pListBoxData->bMultiSelect)
        {
            SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)hWnd);
        }

        if (!pListBoxData->pCurNode)
        {
            if (pListBoxData->wIndex >= 0)
                pListBoxData->pCurNode = FindNode(pListBoxData, pListBoxData->wIndex);
            else
            {
                pListBoxData->pCurNode = FindNode(pListBoxData, 
                    pListBoxData->wCurFirPOS);
                pListBoxData->wIndex = pListBoxData->wCurFirPOS;
            }

            if (pListBoxData->pCurNode)
                bRepaint = TRUE;
        }

        if (dwStyle & LBS_MULTICOLUMN)
        {
        }
        else
        {
            if (pListBoxData->wItemCount > pListBoxData->wPageRange)
            {
                bRepaint = TRUE;
                
                if ((pListBoxData->wIndex + pListBoxData->wPageRange - 1) <=
                    (pListBoxData->wItemCount - 1))
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex;
                    pListBoxData->wIndex = pListBoxData->wCurFirPOS + 
                        pListBoxData->wPageRange - 1;
                    
                    for (i = 0; i < pListBoxData->wPageRange - 1; i++)
                        pListBoxData->pCurNode = pListBoxData->pCurNode->pNext;
                }
                else if (pListBoxData->wIndex == (pListBoxData->wItemCount - 1))
                {
                    pListBoxData->wCurFirPOS = 0;
                    pListBoxData->wIndex = 0;
                    pListBoxData->pCurNode = pListBoxData->pHead;
                }
                else
                {
                    pListBoxData->wIndex = pListBoxData->wItemCount - 1;
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex - 
                        pListBoxData->wPageRange + 1;
                    
                    pListBoxData->pCurNode = pListBoxData->pTail;
                }
            }
            else //if (pListBoxData->wIndex != pListBoxData->wItemCount - 1)
            {
             /*   bRepaint = TRUE;
                
                pListBoxData->pCurNode = pListBoxData->pTail;
                pListBoxData->wIndex = pListBoxData->wItemCount - 1;*/
                break;
            }
        }
        
        if ((pListBoxData->pHead  != pListBoxData->pTail))
            SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
            SCROLLDOWN | SCROLLUP | SCROLLMIDDLE, MASKALL);
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if ((pListBoxData->bHScroll) &&
            (pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen))
            SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
            SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
        else if (!(pListBoxData->bHScroll) && 
                (pListBoxData->wItemCount > pListBoxData->wPageRange))
            SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
            SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);

        break;
    
    case VK_LEFT:
    case VK_RIGHT:
        SendMessage(pListBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
        break;

    case VK_HOME:

        //For a multiple-selection list box, the LBN_SELCHANGE notification is sent 
        //whenever the user presses an arrow key, even if the selection does not change. 
        if (pListBoxData->bMultiSelect)
        {
            SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)hWnd);
        }

        if (pListBoxData->pCurNode != pListBoxData->pHead)
        {
            bRepaint = TRUE;
            pListBoxData->pCurNode = pListBoxData->pHead;
            pListBoxData->wIndex = 0;

            if (pListBoxData->wCurFirPOS != 0)
            {
                pListBoxData->wCurFirPOS = 0;
            }
        }

        break;
    
    case VK_END:
        
        //For a multiple-selection list box, the LBN_SELCHANGE notification is sent 
        //whenever the user presses an arrow key, even if the selection does not change. 
        if (pListBoxData->bMultiSelect)
        {
            SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)hWnd);
        }

        if (pListBoxData->pCurNode != pListBoxData->pTail)
        {
            bRepaint = TRUE;
            pListBoxData->pCurNode = pListBoxData->pTail;
            pListBoxData->wIndex = pListBoxData->wItemCount - 1;

            if (dwStyle & LBS_MULTICOLUMN)
            {
            }
            else
            {
                if (pListBoxData->wItemCount > pListBoxData->wPageRange)
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                        pListBoxData->wPageRange;
                }
            }
        }

        break;

    case VK_F10:
        if ((dwStyle & LBS_MULTIPLESEL) && pListBoxData->bRealFocus 
            && (pListBoxData->bIsSingle == 0))
        {
            pListBoxData->bRealFocus = !pListBoxData->bRealFocus;
            InvalidateRect(hWnd, NULL, TRUE);
            UpdateWindow(hWnd);
            SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                SCROLLUP | SCROLLDOWN | SCROLLMIDDLE, MASKALL);
            return;
        }
		else
			SendMessage(pListBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
        break;
		
	case VK_RETURN:
        if (pListBoxData->bMultiSelect)
        {
            LRESULT lsel;
            lsel = SendMessage(hWnd, LB_GETSEL, pListBoxData->wIndex, 0);
            if (lsel)
                SendMessage(hWnd, LB_SETSEL, (WPARAM)FALSE, (LPARAM)pListBoxData->wIndex);
            else
                SendMessage(hWnd, LB_SETSEL, (WPARAM)TRUE, (LPARAM)pListBoxData->wIndex);
            bRepaint = TRUE;
        }
        else
        {
            //SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
             //   MAKELONG(pListBoxData->wID, LBN_DBLCLK), (LPARAM)hWnd);
			SendMessage(pListBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
            return;
        }
        break;

    case VK_F1:
    case VK_F2:
        SendMessage(pListBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
        break;
    }
    
    if (bRepaint)
    {
        if (pListBoxData->pCurNode)
            pListBoxData->wVirtFocus = -1;
        
        if (pListBoxData->wCurFirPOS != wOldCurFirstPOS)
        {
//            SIZE    size;
//            int     nDistance, surplus, nItemHeight;
//            RECT    rcClient, rcInvalidate;

//            nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
//            nDistance = (wOldCurFirstPOS - pListBoxData->wCurFirPOS) * nItemHeight;
//            GetClientRect(hWnd, &rcClient);
//            ConvertToLBClient(hWnd, &rcClient, pListBoxData, 0, dwStyle);
//            surplus = (rcClient.bottom - rcClient.top) % nItemHeight;
//            rcClient.bottom -= surplus;
            
//            ScrollWindow(hWnd, 0, nDistance, &rcClient, &rcClient);
            InvalidateRect(hWnd, NULL, TRUE);

//            //if Invalidate before ScrollWindow, then paint incorrectly.
//            if (wOldIndex != -1)
//            {
//                SetRect(&rcInvalidate, rcClient.left, 0, rcClient.right, 0);
//
//                rcInvalidate.top = rcClient.top + 
//                    (wOldIndex - pListBoxData->wCurFirPOS) * nItemHeight;
//                rcInvalidate.bottom = rcInvalidate.top + nItemHeight;
//
//                InvalidateRect(hWnd, &rcInvalidate, TRUE);
//                UpdateWindow(NULL);
//            }
        }
        else
        {
            hdc = GetDC(hWnd);
            LISTBOX_Paint(hWnd, hdc, pListBoxData, pOldCurNode, wOldIndex,
                wOldCurFirstPOS, TRUE, -1, -1, -1, -1);
            ReleaseDC(hWnd, hdc);
        }

        if (!pListBoxData->bMultiSelect)
        {
            SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)hWnd);
        }
    }
    
    if ((!IsWindowVisible(hWnd)) && (dwStyle & LBS_COMBOLBOX)) //For comboListbox.
        ShowWindow(hWnd, SW_SHOW);
    
    if (pListBoxData->wCurFirPOS != wOldCurFirstPOS)
    {
        if (dwStyle & LBS_MULTICOLUMN)
        {
            if (((pListBoxData->wItemCount / pListBoxData->wPageRange) >
                pListBoxData->wColumns) || (((pListBoxData->wItemCount / 
                pListBoxData->wPageRange) == pListBoxData->wColumns) && 
                (pListBoxData->wItemCount % pListBoxData->wPageRange)))
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_POS;
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
                
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
            }
        }
        else if (pListBoxData->bVScroll)
        {
            ScrollInfo.cbSize = sizeof(SCROLLINFO);
            ScrollInfo.fMask = SIF_POS;
            
            if (dwStyle & LBS_DISABLENOSCROLL)
                ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
            
            ScrollInfo.nPos = pListBoxData->wCurFirPOS;
            
            SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
        }
    }
}

/**********************************************************************
 * Function     ProcessKeyUp
 * Purpose      
 * Params       
 * Return       
 * Remarks      
 **********************************************************************/

static void ProcessKeyUp(HWND hWnd, PLISTBOXDATA pListBoxData,
                       WPARAM wParam, LPARAM lParam)
{
    pListBoxData->nRepeats = 0;

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

#endif // NOKEYBOARD

static void DoHScroll(HWND hWnd, PLISTBOXDATA pListBoxData, int nScrollCode, IWORD nPos)
{
    IWORD       wOldCurFirstPOS;
    DWORD       dwStyle;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (dwStyle & LBS_MULTICOLUMN)
        wOldCurFirstPOS = pListBoxData->wCurFirPOS;
    else
        wOldCurFirstPOS = pListBoxData->wStartTextPos;

    switch (nScrollCode)
    {
    case SB_LINEUP :
        
        if (dwStyle & LBS_MULTICOLUMN)
        {
            if (pListBoxData->wCurFirPOS > 0)
                pListBoxData->wCurFirPOS -= pListBoxData->wPageRange;
        }
        else
        {
            if (pListBoxData->wStartTextPos)
                pListBoxData->wStartTextPos --;
        }
        
        break;
        
    case SB_LINEDOWN :
        
        if (dwStyle & LBS_MULTICOLUMN)
        {
            if ((pListBoxData->wCurFirPOS + pListBoxData->wPageRange * 
                (pListBoxData->wColumns - 1)) < pListBoxData->wItemCount)
            {
                pListBoxData->wCurFirPOS += pListBoxData->wPageRange;
                
            }
        }
        else
        {
            if (pListBoxData->wStartTextPos + pListBoxData->byPageTextLen <
                pListBoxData->wMaxTextLen)
            {
                pListBoxData->wStartTextPos ++;
            }
        }

        break;
        
    case SB_PAGEUP :
         
        if (dwStyle & LBS_MULTICOLUMN)
        {
            if (pListBoxData->wCurFirPOS > 0)
            {
                if (pListBoxData->wCurFirPOS < pListBoxData->wPageRange * 
                    pListBoxData->wColumns)
                {
                    pListBoxData->wCurFirPOS = 0;
                }
                else
                {
                    pListBoxData->wCurFirPOS -= pListBoxData->wPageRange * 
                        pListBoxData->wColumns;
                }
            }
        }
        else
        {
            if (pListBoxData->wStartTextPos)
            {
                if (pListBoxData->wStartTextPos < pListBoxData->byPageTextLen)
                    pListBoxData->wStartTextPos = 0;
                else
                    pListBoxData->wStartTextPos -= pListBoxData->byPageTextLen - 1;
            }
        }
       
        break;
        
    case SB_PAGEDOWN :
        
        if (dwStyle & LBS_MULTICOLUMN)
        {
            if ((pListBoxData->wCurFirPOS + 
                pListBoxData->wPageRange * (2 * pListBoxData->wColumns - 2)) < 
                pListBoxData->wItemCount)
            {
                pListBoxData->wCurFirPOS += pListBoxData->wPageRange * 
                    pListBoxData->wColumns;
            }
            else if ((pListBoxData->wCurFirPOS + 
                pListBoxData->wPageRange * (pListBoxData->wColumns - 1)) < 
                pListBoxData->wItemCount)
            {
                pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                    pListBoxData->wItemCount % pListBoxData->wPageRange -
                    pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
                
                if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                    pListBoxData->wCurFirPOS += pListBoxData->wPageRange;
            }
        }
        else
        {
            if (pListBoxData->wStartTextPos + pListBoxData->byPageTextLen -1 <=
                pListBoxData->wMaxTextLen - pListBoxData->byPageTextLen)
            {
                pListBoxData->wStartTextPos += pListBoxData->byPageTextLen - 1;
            }
            else
            {
                if (pListBoxData->wMaxTextLen >= pListBoxData->byPageTextLen)
                pListBoxData->wStartTextPos = pListBoxData->wMaxTextLen - 
                    pListBoxData->byPageTextLen;
            }
        }
        break;
        
#if (!NOKEYBOARD)
    case SB_THUMBTRACK:
#endif // NOKEYBOARD
    case SB_THUMBPOSITION :

        if (dwStyle & LBS_MULTICOLUMN)
        {
            if ((nPos >= 0) &&
                (pListBoxData->wPageRange * (nPos + pListBoxData->wColumns - 1) <= 
                pListBoxData->wItemCount - pListBoxData->wItemCount %
                pListBoxData->wPageRange))
            {
                pListBoxData->wCurFirPOS = nPos * pListBoxData->wPageRange;
            }
            else
            {
                pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                    pListBoxData->wItemCount % pListBoxData->wPageRange -
                    pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
                
                if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                    pListBoxData->wCurFirPOS += pListBoxData->wPageRange;
            }
        }
        else
        {
            if (nPos <= pListBoxData->wMaxTextLen - pListBoxData->byPageTextLen)
            {
                pListBoxData->wStartTextPos = nPos;
            }
            else
            {
                pListBoxData->wStartTextPos = pListBoxData->wMaxTextLen - 
                    pListBoxData->byPageTextLen;
            }
        }
        
        break;
    }
    
    if (dwStyle & LBS_MULTICOLUMN)
    {
        if (pListBoxData->wCurFirPOS != wOldCurFirstPOS)
        {
//            int     nDistance;
//            RECT    rcClient;
//            
//            nDistance = (wOldCurFirstPOS - pListBoxData->wCurFirPOS) / 
//                pListBoxData->wPageRange * pListBoxData->nColumnWidth;
//            
//            GetClientRect(hWnd, &rcClient);
//            ConvertToLBClient(hWnd, &rcClient, pListBoxData, 0, dwStyle);

//            ScrollWindow(hWnd, nDistance, 0, &rcClient, &rcClient);
            InvalidateRect(hWnd, NULL, TRUE);

            if (pListBoxData->bHScroll)
            {
                SCROLLINFO  ScrollInfo;
                
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_POS;
                
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
                
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
            }
        }
    }
    else
    {
        if (wOldCurFirstPOS != pListBoxData->wStartTextPos)
        {
//            int     nDistance;
//            RECT    rcClient;
//            SIZE    size;
//            HDC     hdc;
            
//            hdc = GetDC(hWnd);
//            GetTextExtentPoint32(hdc, "T", -1, &size);
//            ReleaseDC(hWnd, hdc);
//            nDistance = (wOldCurFirstPOS - pListBoxData->wStartTextPos) * size.cx;

//            GetClientRect(hWnd, &rcClient);
//            ConvertToLBClient(hWnd, &rcClient, pListBoxData, 0, dwStyle);

            //ScrollWindow(hWnd, nDistance, 0, &rcClient, &rcClient);
            InvalidateRect(hWnd, NULL, TRUE);

            if (pListBoxData->bHScroll)
            {
                SCROLLINFO  ScrollInfo;
                
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_POS;
                
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nPos = pListBoxData->wStartTextPos;
                
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
            }
        }
    }
}

static void DoVScroll(HWND hWnd, PLISTBOXDATA pListBoxData, int nScrollCode, IWORD nPos)
{
    IWORD       wOldCurFirstPOS;
    DWORD       dwStyle;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (dwStyle & LBS_MULTICOLUMN)
        return;

    wOldCurFirstPOS = pListBoxData->wCurFirPOS;

    switch (nScrollCode)
    {
    case SB_LINEUP :
        
        if (pListBoxData->wCurFirPOS > 0)
        {
            pListBoxData->wCurFirPOS--;
        }
        
        break;
        
    case SB_LINEDOWN :
        
        if ((pListBoxData->wCurFirPOS + pListBoxData->wPageRange - 1) <
            (pListBoxData->wItemCount - 1))
        {
            pListBoxData->wCurFirPOS++;
        }
        
        break;
        
    case SB_PAGEUP :
        
        if (pListBoxData->wCurFirPOS > 0)
        {
            if (pListBoxData->wCurFirPOS < pListBoxData->wPageRange)
            {
                pListBoxData->wCurFirPOS = 0;
            }
            else
            {
                pListBoxData->wCurFirPOS -= pListBoxData->wPageRange;
            }
        }
        
        break;
        
    case SB_PAGEDOWN :
        
        if ((pListBoxData->wCurFirPOS + pListBoxData->wPageRange * 2 - 1) < 
            pListBoxData->wItemCount)
        {
            pListBoxData->wCurFirPOS += pListBoxData->wPageRange;
            
        }
        else if (pListBoxData->wItemCount > pListBoxData->wPageRange)
        {
            pListBoxData->wCurFirPOS = pListBoxData->wItemCount
                - pListBoxData->wPageRange;
        }
        break;
        
#if (!NOKEYBOARD)
    case SB_THUMBTRACK:
#endif // NOKEYBOARD
    case SB_THUMBPOSITION :

        if (pListBoxData->wItemCount <= pListBoxData->wPageRange)
            pListBoxData->wCurFirPOS = 0;
        else if ((nPos >= 0) &&
            ((nPos + pListBoxData->wPageRange) <= pListBoxData->wItemCount))
        {
            pListBoxData->wCurFirPOS = nPos;
        }
        else
        {
            pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                pListBoxData->wPageRange;
        }
        
        break;
    }
    
    if (pListBoxData->wCurFirPOS != wOldCurFirstPOS)
    {
//        int     nDistance, surplus, nItemHeight;
//        RECT    rcClient;

//        nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
//        nDistance = (wOldCurFirstPOS - pListBoxData->wCurFirPOS) * nItemHeight;
//        GetClientRect(hWnd, &rcClient);
//        ConvertToLBClient(hWnd, &rcClient, pListBoxData, 0, dwStyle);
//        surplus = (rcClient.bottom - rcClient.top) % nItemHeight;
//        rcClient.bottom -= surplus;

//        ScrollWindow(hWnd, 0, nDistance, &rcClient, &rcClient);
        InvalidateRect(hWnd, NULL, TRUE);

        if (pListBoxData->bVScroll)
        {
            SCROLLINFO  ScrollInfo;

            ScrollInfo.cbSize = sizeof(SCROLLINFO);
            ScrollInfo.fMask = SIF_POS;
            
            if (dwStyle & LBS_DISABLENOSCROLL)
                ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
            
            ScrollInfo.nPos = pListBoxData->wCurFirPOS;
            
            SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
        }
    }
}

//two pixel horizontal lines on top and bottom, two pixel vertical lines on left and right.
static void Draw3DListBoxRect_W95(PLISTBOXDATA pListBoxData, HDC hdc, RECT* pRect, 
                                  BOOL bVScroll, BOOL bHScroll, DWORD dwStyle)
{
#if (INTERFACE_MONO)
#else // INTERFACE_MONO
    HPEN hOldPen;

    if (!pRect)
        return;

    if (bHScroll && !(dwStyle & LBS_DISABLENOSCROLL) && 
        ((!(dwStyle & LBS_MULTICOLUMN) && !(pListBoxData->wMaxTextLen > 
        pListBoxData->byPageTextLen)) || ((dwStyle & LBS_MULTICOLUMN) && 
        pListBoxData->wItemCount <= pListBoxData->wPageRange * 
        (pListBoxData->wColumns - 1))))
    {
        bHScroll = FALSE;
    }

    if (bVScroll && !(dwStyle & LBS_DISABLENOSCROLL) && 
        (((!(dwStyle & LBS_MULTICOLUMN) && 
        (pListBoxData->wItemCount <= pListBoxData->wPageRange)) || 
        (dwStyle & LBS_MULTICOLUMN))))
    {
        bVScroll = FALSE;
    }

    // Draw left and top darkgray line

    hOldPen = SelectObject(hdc, GetStockObject(GRAY_PEN));

    if (bHScroll)
        DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom);
    else
        DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom - 1);

    if (bVScroll)
        DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right, pRect->top);
    else
        DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, pRect->top);

    // Draw left and top black line

    SelectObject(hdc, GetStockObject(BLACK_PEN));

    if (bHScroll)
    {
        DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,
            pRect->bottom);
    }
    else
    {
        DrawLine(hdc, pRect->left + 1, pRect->top + 1, pRect->left + 1,
            pRect->bottom - 2);
    }

    if (bVScroll)
    {
        DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right, 
            pRect->top + 1);
    }
    else
    {
        DrawLine(hdc, pRect->left + 2, pRect->top + 1, pRect->right - 2, 
            pRect->top + 1);
    }

    // Draw right and bottom light gray line

    SelectObject(hdc, GetStockObject(WTGRAY_PEN));

    if (!bVScroll)
    {
        DrawLine(hdc, pRect->right - 2, pRect->top + 1, pRect->right - 2,
            pRect->bottom);
    }

    if (!bHScroll)
    {
        DrawLine(hdc, pRect->left + 1, pRect->bottom - 2, pRect->right - 1,
            pRect->bottom - 2);
    }

    SelectObject(hdc, GetStockObject(WHITE_PEN));

    if (!bVScroll)
    {
        DrawLine(hdc, pRect->right - 1, pRect->top + 1, pRect->right - 1,
            pRect->bottom - 1);
    }

    if (!bHScroll)
    {
        DrawLine(hdc, pRect->left + 1, pRect->bottom - 1, pRect->right - 1,
            pRect->bottom - 1);
    }

    // Restore the old brush
    SelectObject(hdc, hOldPen);

#endif // ITNERFACE_MONO
}
static void DrawVirtFrame(PLISTBOXDATA pListBoxData, HDC hdc, RECT* pRect, 
                                  BOOL bVScroll, BOOL bHScroll, DWORD dwStyle)
{
    HPEN hOldPen, hpnNew;

    if (!pRect)
        return;

    if (bHScroll && !(dwStyle & LBS_DISABLENOSCROLL) && 
        ((!(dwStyle & LBS_MULTICOLUMN) && !(pListBoxData->wMaxTextLen > 
        pListBoxData->byPageTextLen)) || ((dwStyle & LBS_MULTICOLUMN) && 
        pListBoxData->wItemCount <= pListBoxData->wPageRange * 
        (pListBoxData->wColumns - 1))))
    {
        bHScroll = FALSE;
    }

    if (bVScroll && !(dwStyle & LBS_DISABLENOSCROLL) && 
        (((!(dwStyle & LBS_MULTICOLUMN) && 
        (pListBoxData->wItemCount <= pListBoxData->wPageRange)) || 
        (dwStyle & LBS_MULTICOLUMN))))
    {
        bVScroll = FALSE;
    }

    // Draw left and top darkgray line
    hpnNew = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));//PS_DASH 
    hOldPen = SelectObject(hdc, hpnNew);

    if (bHScroll)
        DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom);
    else
        DrawLine(hdc, pRect->left, pRect->top, pRect->left, pRect->bottom - 1);

    if (bVScroll)
        DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right, pRect->top);
    else
        DrawLine(hdc, pRect->left + 1, pRect->top, pRect->right - 1, pRect->top);

    // Draw right and bottom light gray line

    if (!bVScroll)
    {
        DrawLine(hdc, pRect->right - 1, pRect->top + 1, pRect->right - 1,
            pRect->bottom - 1);
    }

    if (!bHScroll)
    {
        DrawLine(hdc, pRect->left + 1, pRect->bottom - 1, pRect->right - 1,
            pRect->bottom - 1);
    }

    // Restore the old brush
    SelectObject(hdc, hOldPen);
    DeleteObject(hpnNew);
}

static LRESULT ProcessPen(HWND hWnd, PLISTBOXDATA pListBoxData, UINT wMsgCmd, 
                          WPARAM wParam, LPARAM lParam)
{
    IWORD       x, y, wPos;//x, y is signed.
    RECT        rect, rcScroll, rcClient;
    HDC         hdc;
//    SIZE        size;
    int         surplus, nBitsPixel, nItemHeight; 
    DWORD       dwStyle;
    IWORD       wOldIndex, wOldCurFirstPOS;
    PLISTNODE   p, pOldCurNode;
    SCROLLINFO  ScrollInfo;
    LRESULT     lResult;
	static	int	wPenDownIndex;
	static PLISTNODE pPenDownPtr;

    lResult = 0;

    nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    GetClientRect(hWnd, &rcClient);
    SetRect(&rect, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);
    ConvertToLBClient(hWnd, &rect, pListBoxData, nBitsPixel, dwStyle);
    
    hdc = GetDC(hWnd);

//    GetTextExtentPoint32(hdc, "T", -1, &size);
//    nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);
    nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);

    surplus = (rect.bottom - rect.top) % nItemHeight;
    SetRect(&rect, rect.left, rect.top, rect.right, rect.bottom - surplus);

    switch(wMsgCmd)
    {
    case WM_PENDOWN :

        //if (!pListBoxData->bRealFocus)
        //{
            //pListBoxData->bInsert = TRUE;
//#ifndef _EMULATE_
//        if (IsWindowEnabled(hWnd))
//            TouchAlert();
//#endif  /* _EMULATE_ */
        pListBoxData->bRealFocus = TRUE;
        InvalidateRect(hWnd, NULL, TRUE);
        //UpdateWindow(hWnd);
        //}

        x = LOWORD(lParam);
        y = HIWORD(lParam);
        
        if (dwStyle & LBS_COMBOLBOX)
        {
            SetRect(&rcScroll, rcClient.right, rcClient.top, 
                rcClient.right + GetSystemMetrics(SM_CXVSCROLL), rcClient.bottom);

            if (PtInRectXY(&rcScroll, x, y))
            {
                GetWindowRectEx(hWnd, &rect, W_CLIENT, XY_SCREEN);
                x += (IWORD)rect.left;
                y += (IWORD)rect.top;

                lResult = DefWindowProc(hWnd, WM_NCPENDOWN, HTVSCROLL, 
                    MAKELPARAM(x, y));
                break;
            }
        }
                
        if (!PtInRectXY(&rect, x, y))
            break;

        //Don't capture focus and mouse, but by combobox resend message.
        pListBoxData->bPenDown = TRUE;
        if (!(dwStyle & LBS_COMBOLBOX))
        {
            SetFocus(hWnd);
        }

        wPos = y / (IWORD)nItemHeight + pListBoxData->wCurFirPOS;
        if (dwStyle & LBS_MULTICOLUMN)
            wPos += x / pListBoxData->nColumnWidth * pListBoxData->wPageRange;

        if (wPos > pListBoxData->wItemCount - 1)
        {
            pListBoxData->bPenDown = FALSE;
            break;
        }
        wPenDownIndex = wOldIndex = pListBoxData->wIndex;
		
        /*if (!(dwStyle & LBS_COMBOLBOX))
        {
            SetCapture(hWnd);
        }*/

        pPenDownPtr = pOldCurNode = pListBoxData->pCurNode;
        p = FindNode(pListBoxData, (WORD)wPos);
        
        if (wPos == wOldIndex && pListBoxData->pCurNode && 
            !pListBoxData->bMultiSelect)
        {
            pListBoxData->bSelChanged = TRUE;

            if (pListBoxData->bSelected && !(dwStyle & LBS_COMBOLBOX))
            {
				SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
					MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)hWnd);
                SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                    MAKELONG(pListBoxData->wID, LBN_DBLCLK), (LPARAM)hWnd);
				pListBoxData->bPenDown = FALSE;

				/*if (!(dwStyle & LBS_COMBOLBOX))
					ReleaseCapture();*/
			}
            break;
        }

        if (!(dwStyle & LBS_COMBOLBOX))
            SetCapture(hWnd);
        
        if (pOldCurNode != p)
            pListBoxData->bSelected = FALSE;

        if (p)
            pListBoxData->bSelChanged = TRUE;

        if ((p != NULL) && 
            (pListBoxData->pCurNode != p || pListBoxData->bMultiSelect))
        {
            pListBoxData->wIndex = wPenDownIndex = wPos;
			pPenDownPtr = p;
            if ((wOldIndex != pListBoxData->wIndex) || !pListBoxData->pCurNode || 
                pListBoxData->bMultiSelect)
            {
                if (pListBoxData->bMultiSelect)
                    p->bSelect = !p->bSelect;

                pListBoxData->pCurNode = p;

                if (pListBoxData->pCurNode)
                    pListBoxData->wVirtFocus = -1;

                LISTBOX_Paint(hWnd, hdc, pListBoxData, pOldCurNode, wOldIndex,
                    pListBoxData->wCurFirPOS, TRUE, -1, -1, -1, -1);
            }
        }

        break;

    case WM_PENMOVE :

        x = LOWORD(lParam);
        y = HIWORD(lParam);

        if (dwStyle & LBS_COMBOLBOX)
        {
            //After click ComboBox arrow region, need process PENMOVE 
            //without has been PENDOWN.
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_SCREEN);
            
            lResult = DefWindowProc(hWnd, WM_NCPENMOVE, HTVSCROLL, 
                MAKELPARAM(x + rcClient.left, y + rcClient.top));

            if (!(HIWORD(wParam) || pListBoxData->bPenDown))
                break;
        }
        else if (!pListBoxData->bPenDown)
            break;

        pOldCurNode = pListBoxData->pCurNode;
        wOldCurFirstPOS = pListBoxData->wCurFirPOS;
        wOldIndex = pListBoxData->wIndex;
        
        if (y >= rect.bottom && (!(dwStyle & LBS_COMBOLBOX) || 
            pListBoxData->bComboLBPenMoveIn) && (dwStyle & WS_VSCROLL))
        {
            if (!(dwStyle & LBS_COMBOLBOX))
            {
                if (dwStyle & LBS_MULTICOLUMN)
                {
/*                    wPos = (x / pListBoxData->nColumnWidth) * 
                        pListBoxData->wPageRange + (wOldIndex - wOldCurFirstPOS) %
                        pListBoxData->wPageRange;
                    
                    if (wPos < 0)
                        wPos = 0;

                    if (wPos >= pListBoxData->wItemCount)
                        wPos = pListBoxData->wItemCount - 1;
                    
                    pListBoxData->wIndex = wPos;
                    if (wPos != wOldIndex)
                        pListBoxData->pCurNode = FindNode(pListBoxData, (WORD)wPos);*/
                }
                else
                {
                    if (!pListBoxData->pCurNode)
                    {
                        pListBoxData->pCurNode = pListBoxData->pTail;
                        pListBoxData->wIndex = pListBoxData->wItemCount - 1;
                        pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                            pListBoxData->wPageRange;
                    }
                    else if (pListBoxData->pCurNode->pNext)
                    {
                        pListBoxData->pCurNode = pListBoxData->pCurNode->pNext;
                        if (pListBoxData->wCurFirPOS < pListBoxData->wItemCount - 
                            pListBoxData->wPageRange)
                        {
                            pListBoxData->wCurFirPOS ++;
                        }
                        
                        if (pListBoxData->wIndex > 0)
                            pListBoxData->wIndex++;
                    }
                }
            }
            else
            {
                if ((pListBoxData->wCurFirPOS + pListBoxData->wPageRange) <
                    pListBoxData->wItemCount)
                {
                    pListBoxData->wCurFirPOS++;
                }

                pListBoxData->pCurNode = pPenDownPtr;
                pListBoxData->wIndex = wPenDownIndex;
            }
        }
        else if (y <= rect.top && (!(dwStyle & LBS_COMBOLBOX) || 
            pListBoxData->bComboLBPenMoveIn) && (dwStyle & WS_VSCROLL))
        {
            if (!(dwStyle & LBS_COMBOLBOX))
            {
                if (dwStyle & LBS_MULTICOLUMN)
                {
/*                    wPos = (x / pListBoxData->nColumnWidth) * 
                        pListBoxData->wPageRange + (wOldIndex - wOldCurFirstPOS) %
                        pListBoxData->wPageRange;

                    if (wPos < 0)
                        wPos = 0;

                    if (wPos >= pListBoxData->wItemCount)
                        wPos = pListBoxData->wItemCount - 1;
                    
                    pListBoxData->wIndex = wPos;
                    if (wPos != wOldIndex)
                        pListBoxData->pCurNode = FindNode(pListBoxData, (WORD)wPos);*/
                }
                else
                {
                    if (!pListBoxData->pCurNode)
                    {
                        pListBoxData->pCurNode = pListBoxData->pHead;
                        
                        pListBoxData->wIndex = pListBoxData->wCurFirPOS = 0;
                    }
                    else if (pListBoxData->pCurNode->pPrev)
                    {
                        pListBoxData->pCurNode = pListBoxData->pCurNode->pPrev;
                        if (pListBoxData->wCurFirPOS > 0)
                            pListBoxData->wCurFirPOS --;
                        
                        if (pListBoxData->wIndex > 0)
                            pListBoxData->wIndex--;
                    }
                }
            }
            else
            {
                if (pListBoxData->wCurFirPOS > 0)
                {
                    pListBoxData->wCurFirPOS--;
                }
                pListBoxData->pCurNode = pPenDownPtr;
                pListBoxData->wIndex = wPenDownIndex;
            }
        }
        else if ((y > rect.top) && (y < rect.bottom))
        {
            wPos = y / (IWORD)nItemHeight;
            if (wPos >= pListBoxData->wPageRange)
                break;

            wPos += pListBoxData->wCurFirPOS;
            if (dwStyle & LBS_MULTICOLUMN)
            {
                if (x > rect.left && x < rect.right)
                    wPos += x / pListBoxData->nColumnWidth * pListBoxData->wPageRange;
                else
                {
                    break;
                    //wPos += (wOldIndex - wOldCurFirstPOS) / pListBoxData->wPageRange *
                        //pListBoxData->wPageRange;
                }
            }
            
            p = FindNode(pListBoxData, (WORD)wPos);
            
            if (p != NULL) 
            {
                pListBoxData->wIndex = wPos;
                if (wOldIndex != pListBoxData->wIndex)
                {
                    pListBoxData->pCurNode = p;

                    if (pListBoxData->pCurNode)
                        pListBoxData->wVirtFocus = -1;
                    
                    LISTBOX_Paint(hWnd, hdc, pListBoxData, pOldCurNode, wOldIndex,
                        wOldCurFirstPOS, TRUE, -1, -1, -1, -1);
                }
            }

            pListBoxData->bComboLBPenMoveIn = TRUE;
        
            if (pListBoxData->pCurNode != pOldCurNode)
            {
                pListBoxData->bSelChanged = TRUE;
                pListBoxData->bSelected = FALSE;
            }

            break;
        }

        if (pOldCurNode != pListBoxData->pCurNode)
            pListBoxData->bSelected = FALSE;

        if (pListBoxData->bVScroll && ((y >= rect.bottom) ||
            (y <= rect.top)))
        {
            if ((pListBoxData->wCurFirPOS != wOldCurFirstPOS) ||
                (pListBoxData->pCurNode != pOldCurNode))
            {
                if (pListBoxData->pCurNode)
                    pListBoxData->wVirtFocus = -1;

                LISTBOX_Paint(hWnd, hdc, pListBoxData, pOldCurNode, wOldIndex,
                    wOldCurFirstPOS, TRUE, -1, -1, -1, -1);
                
                if (pListBoxData->wCurFirPOS != wOldCurFirstPOS)
                {
                    if (dwStyle & LBS_MULTICOLUMN)
                    {
                        if (((pListBoxData->wItemCount / pListBoxData->wPageRange) >
                            pListBoxData->wColumns) || (((pListBoxData->wItemCount / 
                            pListBoxData->wPageRange) == pListBoxData->wColumns) && 
                            (pListBoxData->wItemCount % pListBoxData->wPageRange)))
                        {
                            ScrollInfo.cbSize = sizeof(SCROLLINFO);
                            ScrollInfo.fMask = SIF_POS;
                            if (dwStyle & LBS_DISABLENOSCROLL)
                                ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                            
                            ScrollInfo.nPos = pListBoxData->wCurFirPOS / 
                                pListBoxData->wPageRange;
                            
                            SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                        }
                    }
                    else if (pListBoxData->bVScroll)
                    {
                        ScrollInfo.cbSize = sizeof(SCROLLINFO);
                        ScrollInfo.fMask = SIF_POS;
                        if (dwStyle & LBS_DISABLENOSCROLL)
                            ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                        
                        ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                        
                        SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                    }
                }
            }
        }

        break;

    case WM_PENUP :

        pListBoxData->bComboLBPenMoveIn = FALSE;
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        
        if (dwStyle & LBS_COMBOLBOX)
        {
            //After click ComboBox arrow region, need process PENUP 
            //without has been PENDOWN in ListBox.
            GetWindowRectEx(hWnd, &rcClient, W_CLIENT, XY_SCREEN);
            
            lResult = DefWindowProc(hWnd, WM_NCLBUTTONUP, HTVSCROLL, 
                MAKELPARAM(x + rcClient.left, y + rcClient.top));

            if (!(HIWORD(wParam) || pListBoxData->bPenDown))
                break;
        }

        if (!(dwStyle & LBS_COMBOLBOX) && pListBoxData->bRealFocus 
            && (pListBoxData->pCurNode != NULL) 
            && (pListBoxData->pHead != pListBoxData->pTail))
        {
            if ((pListBoxData->pHead  != pListBoxData->pTail))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLDOWN | SCROLLUP | SCROLLMIDDLE, MASKALL);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if ((pListBoxData->bHScroll) &&
                (pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
            else if (!(pListBoxData->bHScroll) && 
                (pListBoxData->wItemCount > pListBoxData->wPageRange))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);

        }

        if (pListBoxData->bPenDown || (dwStyle & LBS_COMBOLBOX))
        {
            pListBoxData->bPenDown = FALSE;
            
            pListBoxData->bSelected = TRUE;

            if (!(dwStyle & LBS_COMBOLBOX))
                ReleaseCapture();

            if ((dwStyle & LBS_COMBOLBOX))
            {
                PostMessage(pListBoxData->hParentWnd, WM_ENDLOOP, 0, 0);
            }

            if (pListBoxData->bSelChanged && 
                (!(dwStyle & LBS_COMBOLBOX) || pListBoxData->pCurNode))
            {  
                pListBoxData->bSelChanged = FALSE;

                if ((dwStyle & LBS_COMBOLBOX) && !PtInRectXY(&rect, x, y))
                {
                    SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)TRUE);
					   //FALSE,changed for cbs_dropDown
                }
                else
                {
                    SendMessage(pListBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pListBoxData->wID, LBN_SELCHANGE), (LPARAM)hWnd);
                }
            }
        }
        if (!(dwStyle & WS_TABSTOP))
        {
            //SendMessage(pListBoxData->hParentWnd, WM_ACTIVATE, WA_ACTIVE, 0);
            //?????
            pListBoxData->bRealFocus = TRUE;
        }

        break;
    }

    ReleaseDC(hWnd, hdc);
                
    return  lResult;
}

static LRESULT ProcessSetImage(HWND hWnd, PLISTBOXDATA pListBoxData, 
                               WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    SCROLLINFO      ScrollInfo;
    RECT            rect;
//    SIZE            size;
    DWORD           dwStyle;
    IWORD           index;
    PLISTNODE       p;
	DWORD	        fImageType;
    BITMAP          bitmap;
	HANDLE	        hImage;
//    HDC             hdc;
	int				nItemHeight;
    int             nIconPos = 0;

    lResult = (LRESULT)FALSE;

    fImageType = LOBYTE(LOWORD(wParam));
    nIconPos = HIBYTE(LOWORD(wParam));
    if (nIconPos > 1)
        nIconPos = 0;
    index = HIWORD(wParam);

    hImage = (HANDLE) lParam;    // handle of the image 
    
    p = FindNode(pListBoxData, index);
    if (!p)
        return lResult;
    
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
//    hdc = GetDC(hWnd);
//    GetTextExtentPoint32(hdc, "T", -1, &size);
//    ReleaseDC(hWnd, hdc);
    
    if (((fImageType == IMAGE_BITMAP) && (dwStyle & LBS_BITMAP)) ||
        ((fImageType == IMAGE_ICON) && (dwStyle & LBS_ICON)))
    {
        lResult = (LRESULT)p->hImage[nIconPos];
        p->hImage[nIconPos] = hImage;
        
        if (p->hImage[nIconPos] != (HANDLE)lResult)
        {
            WORD   wOldPageRange, wOldBmpHeight, wOldBmpWidth;
            
            if (p->hImage[nIconPos])
                GetObject((HGDIOBJ)p->hImage[nIconPos], sizeof(BITMAP), &bitmap);
            
            wOldBmpWidth = pListBoxData->wBitmapWidth[nIconPos]; 
            wOldBmpHeight = pListBoxData->wBitmapHeight[nIconPos];
            GetClientRect(hWnd, &rect);
            ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
            
            if (p->hImage[nIconPos] && ((bitmap.bmWidth > pListBoxData->wBitmapWidth[nIconPos]) || 
                (bitmap.bmHeight > pListBoxData->wBitmapHeight[nIconPos])))
            {
                pListBoxData->wBitmapWidth[nIconPos] = (WORD)bitmap.bmWidth;
                pListBoxData->wBitmapHeight[nIconPos] = (WORD)bitmap.bmHeight;
                
                wOldPageRange = pListBoxData->wPageRange;
                
//                hdc = GetDC(hWnd);
                nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
//                ReleaseDC(hWnd, hdc);
//				nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);
                pListBoxData->wPageRange = (rect.bottom - rect.top) / nItemHeight;
                
                if (pListBoxData->wPageRange < 0)
                    pListBoxData->wPageRange = 0;
                if (dwStyle & LBS_MULTICOLUMN)
                {
                    if (pListBoxData->wItemCount <= pListBoxData->wPageRange *
                        (pListBoxData->wColumns - 1))
                    {
                        pListBoxData->wCurFirPOS = 0;
                    }
                    else if (pListBoxData->wPageRange > 0 && (pListBoxData->wCurFirPOS + 
                        pListBoxData->wPageRange * (pListBoxData->wColumns - 2)) >= 
                        pListBoxData->wItemCount)
                    {
                        pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                            pListBoxData->wItemCount % pListBoxData->wPageRange -
                            pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
                        
                        if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                            pListBoxData->wCurFirPOS += pListBoxData->wPageRange;
                    }
                }
                else
                {
                    if (pListBoxData->wItemCount <= pListBoxData->wPageRange)
                        pListBoxData->wCurFirPOS = 0;
                    else if (((pListBoxData->wCurFirPOS + pListBoxData->wPageRange) > 
                        pListBoxData->wItemCount))
                    {
                        pListBoxData->wCurFirPOS = pListBoxData->wItemCount - 
                            pListBoxData->wPageRange;
                    }
                }
                
                if (pListBoxData->wPageRange != wOldPageRange)
                {
                    if (dwStyle & LBS_MULTICOLUMN)
                    {
                        ScrollInfo.cbSize = sizeof(SCROLLINFO);
                        ScrollInfo.fMask = SIF_ALL;
                        if (dwStyle & LBS_DISABLENOSCROLL)
                            ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                        
                        ScrollInfo.nMin = 0;
                        ScrollInfo.nMax = pListBoxData->wItemCount / 
                            pListBoxData->wPageRange + 1;
                        if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                            ScrollInfo.nMax ++;
                        
                        ScrollInfo.nPage = pListBoxData->wColumns;
                        ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;

                        if (ScrollInfo.nMax != 0)
                            ScrollInfo.nMax --;
                        if (IsWindowVisible(hWnd))
                            SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                        else
                            SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
                    }
                    else if (pListBoxData->bVScroll)
                    {
                        ScrollInfo.cbSize = sizeof(SCROLLINFO);
                        ScrollInfo.fMask = SIF_ALL;
                        if (dwStyle & LBS_DISABLENOSCROLL)
                            ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                        
                        ScrollInfo.nMin = 0;
                        ScrollInfo.nMax = pListBoxData->wItemCount;
                        ScrollInfo.nPage = pListBoxData->wPageRange;
                        ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                        
                        if (ScrollInfo.nMax != 0)
                            ScrollInfo.nMax --;
                        if (IsWindowVisible(hWnd))
                            SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                        else
                            SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
                    }
                }
            }

            if (IsWindowVisible(hWnd))
            {
                if (wOldBmpWidth != pListBoxData->wBitmapWidth[nIconPos] ||
                    wOldBmpHeight != pListBoxData->wBitmapHeight[nIconPos])
                {
                    InvalidateRect(hWnd, &rect, TRUE);
                }
                else if ((!(dwStyle & LBS_MULTICOLUMN) && 
                    index >= pListBoxData->wCurFirPOS &&
                    (index < (pListBoxData->wCurFirPOS + pListBoxData->wPageRange))) || 
                    ((dwStyle & LBS_MULTICOLUMN) && 
                    (index < (pListBoxData->wCurFirPOS + pListBoxData->wPageRange *
                    pListBoxData->wColumns))))
                {
                    int     nItemHeight;

//                    hdc = GetDC(hWnd);
                    nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
//                    ReleaseDC(hWnd, hdc);
//                    nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);

                    if (dwStyle & LBS_MULTICOLUMN)
                    {
                        RECT rcList;

                        SetRect(&rcList, rect.left, rect.top, rect.right, rect.bottom);
                        
                        rect.left += pListBoxData->nColumnWidth * 
                            ((index - pListBoxData->wCurFirPOS) / pListBoxData->wPageRange);
                        rect.right = rect.left + pListBoxData->nColumnWidth;
                        if (rect.right > rcList.right)
                            rect.right = rcList.right;
                        
                        rect.top += nItemHeight * ((index - pListBoxData->wCurFirPOS) %
                            pListBoxData->wPageRange);
                    }
                    else
                        rect.top += nItemHeight * (index - pListBoxData->wCurFirPOS);

                    rect.bottom = rect.top + nItemHeight; 
                    
                    InvalidateRect(hWnd, &rect, TRUE);
                }
            }
        }
    }

    return lResult;
}

static LRESULT ProcessAddString(HWND hWnd, PLISTBOXDATA pListBoxData, 
                                IWORD wStrLen, LPARAM lParam)
{
    LRESULT         lResult;
    SCROLLINFO      ScrollInfo;
    DWORD           dwStyle;
    IWORD           nInsertPos, oldIndex, wOldCurFirstPOS;
    LPCSTR          lpsz;
    RECT            rect;

    lResult = FALSE;
    lpsz = (LPCSTR)lParam;   // address of string to add
    
    if (lpsz == NULL)
    {
        lResult = (LRESULT)LB_ERR;
        return lResult;
    }
    
    oldIndex = pListBoxData->wIndex;
    wOldCurFirstPOS = pListBoxData->wCurFirPOS;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    
    if (dwStyle & LBS_SORT)
    {
        nInsertPos = InsertNodeSort(hWnd, pListBoxData, lpsz, 
            !(!(dwStyle & LBS_HASSTRINGS) && ((dwStyle & LBS_OWNERDRAWFIXED) ||
            (dwStyle & LBS_OWNERDRAWVARIABLE))), 
			wStrLen, (dwStyle & LBS_USERSTRINGS));
        if (nInsertPos == -1)
        {
            lResult = (LRESULT)LB_ERRSPACE;
            return lResult;
        }
    }
    else
    {
        nInsertPos = pListBoxData->wItemCount;
        
        if (!AddTail(hWnd, pListBoxData, lpsz, 
            !(!(dwStyle & LBS_HASSTRINGS) && ((dwStyle & LBS_OWNERDRAWFIXED) ||
            (dwStyle & LBS_OWNERDRAWVARIABLE))), wStrLen, (dwStyle & LBS_USERSTRINGS)))
        {
            lResult = (LRESULT)LB_ERRSPACE;
            return lResult;
        }
    }
    
    lResult = (LRESULT)nInsertPos;
    
    pListBoxData->wItemCount +=1;
    if (nInsertPos <= oldIndex)
        pListBoxData->wIndex ++;
    
    if (pListBoxData->bInitData)
        return lResult;

    if (dwStyle & LBS_MULTICOLUMN)
    {
        if ((pListBoxData->wPageRange) && ((pListBoxData->wItemCount / 
            pListBoxData->wPageRange) > (pListBoxData->wColumns - 1) || 
            (((pListBoxData->wItemCount / pListBoxData->wPageRange) == 
            (pListBoxData->wColumns - 1)) && 
            (pListBoxData->wItemCount % pListBoxData->wPageRange))))
        {
            ScrollInfo.cbSize = sizeof(SCROLLINFO);
            ScrollInfo.fMask = SIF_ALL;
            if (dwStyle & LBS_DISABLENOSCROLL)
                ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
            
            ScrollInfo.nMin = 0;
            ScrollInfo.nMax = pListBoxData->wItemCount / pListBoxData->wPageRange + 1;
            if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                ScrollInfo.nMax ++;
            
            ScrollInfo.nPage = pListBoxData->wColumns;
            ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
            
            if (ScrollInfo.nMax != 0)
                ScrollInfo.nMax --;
            if (IsWindowVisible(hWnd))
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
            else
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
        }
    }
    else
    {
        if ((pListBoxData->wItemCount >= pListBoxData->wPageRange) &&
            pListBoxData->bVScroll)
        {
            ScrollInfo.cbSize = sizeof(SCROLLINFO);
            ScrollInfo.fMask = SIF_ALL;
            if (dwStyle & LBS_DISABLENOSCROLL)
                ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
            
            ScrollInfo.nMin = 0;
            ScrollInfo.nMax = pListBoxData->wItemCount;
            ScrollInfo.nPage = pListBoxData->wPageRange;
            ScrollInfo.nPos = pListBoxData->wCurFirPOS;
            
            if (ScrollInfo.nMax != 0)
                ScrollInfo.nMax --;
            if (IsWindowVisible(hWnd))
                SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
            else
                SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
        }
        
        if (pListBoxData->bHScroll)
        {
            SCROLLINFO ScrollInfoOld;

            ScrollInfoOld.cbSize = sizeof(SCROLLINFO);
            ScrollInfoOld.fMask = SIF_ALL;
            GetScrollInfo(hWnd, SB_HORZ, &ScrollInfoOld);
            
            if ((UINT)ScrollInfoOld.nMax < pListBoxData->wMaxTextLen)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_ALL;
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nMin = 0;
                ScrollInfo.nMax = pListBoxData->wMaxTextLen;
                ScrollInfo.nPage = pListBoxData->byPageTextLen;
                ScrollInfo.nPos = pListBoxData->wStartTextPos;
                
                if (ScrollInfo.nMax != 0)
                    ScrollInfo.nMax --;
                if (IsWindowVisible(hWnd))
                    SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                else
                    SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
            }
        }
    }
    
    if ((IsWindowVisible(hWnd)) && ((!(dwStyle & LBS_MULTICOLUMN) && 
        (nInsertPos < (wOldCurFirstPOS + pListBoxData->wPageRange))) || 
        ((dwStyle & LBS_MULTICOLUMN) && 
        (nInsertPos < (wOldCurFirstPOS + pListBoxData->wPageRange *
        pListBoxData->wColumns)))))
    {
        int nItemHeight;
//        SIZE size;
//        HDC hdc;
        
        GetClientRect(hWnd, &rect);
        ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
        
//        hdc = GetDC(hWnd);
        nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
//        GetTextExtentPoint32(hdc, "T", -1, &size);
//        ReleaseDC(hWnd, hdc);
//        nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);
        
        if (wOldCurFirstPOS == pListBoxData->wCurFirPOS)
        {
            if (dwStyle & LBS_MULTICOLUMN)
            {
                RECT rcList;
                
                SetRect(&rcList, rect.left, rect.top, rect.right, rect.bottom);
                
                rect.left += pListBoxData->nColumnWidth * 
                    ((nInsertPos - wOldCurFirstPOS) / pListBoxData->wPageRange);
                //rect.top += nItemHeight * ((nInsertPos - wOldCurFirstPOS) %
                    //pListBoxData->wPageRange);
            }
            else
                rect.top += (nInsertPos - wOldCurFirstPOS) * nItemHeight;
        }

        InvalidateRect(hWnd, &rect, TRUE);
    }

    return lResult;
}

static LRESULT ProcessSelectString(HWND hWnd, PLISTBOXDATA pListBoxData, 
                               WPARAM wParam, LPARAM lParam)
{
    HDC             hdc;
    DWORD           dwStyle;
    int             i;
    IWORD           index, wOldCurFirstPOS, wOldIndex;
    PLISTNODE       p, pOldCurNode;
    LRESULT         lResult;
    SCROLLINFO      ScrollInfo;
    LPCSTR          lpsz;

    lResult = FALSE;
    lpsz = (LPCSTR)lParam;  // address of search string 
    
    lResult = (LRESULT)LB_ERR;
    if (pListBoxData->bMultiSelect || !lpsz || !strlen(lpsz))
    {
        return lResult;
    }
    
    index = (IWORD)wParam;  // item before start of search 
    
    if (index < -1 || index >= pListBoxData->wItemCount - 1)
        index = -1;
    
    if (index == -1)
    {
        p = pListBoxData->pHead;
        i = 0;
    }
    else
    {
        p = FindNode(pListBoxData, (IWORD)(index + 1));
        i = index = index + 1;
    }
    
    pOldCurNode = pListBoxData->pCurNode;
    wOldCurFirstPOS = pListBoxData->wCurFirPOS;
    wOldIndex = pListBoxData->wIndex;
    
    do
    {
        if (p == NULL)
            break;
        
        if (Strnicmp(p->pNodeString, (PSTR)lpsz) == 0)
        {
            pListBoxData->pCurNode = FindNode(pListBoxData, (IWORD)i);
            pListBoxData->wIndex = i;
            
            lResult = (LRESULT)i;
            break;
        }
        p = p->pNext;
        i++;
    }while(1);
    
    if (lResult == -1 && index != -1)
    {
        p = pListBoxData->pHead;
        i = 0;
        do
        {
            if (p == NULL)
                break;
            
            if (Strnicmp(p->pNodeString, (PSTR)lpsz) == 0)
            {
                pListBoxData->pCurNode = FindNode(pListBoxData, (IWORD)i);
                pListBoxData->wIndex = i;
                
                lResult = (LRESULT)i;
                break;
            }
            p = p->pNext;
            i++;
        }while(i < index);
    }
    
    if (lResult == -1)
        lResult = (LRESULT)LB_ERR;
    else
    {
        if (pListBoxData->pCurNode)
            pListBoxData->wVirtFocus = -1;
        
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (dwStyle & LBS_MULTICOLUMN)
        {
            if (pListBoxData->wIndex < pListBoxData->wCurFirPOS)
            {
                pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                    pListBoxData->wIndex % pListBoxData->wPageRange;
            }
            else if (pListBoxData->wPageRange > 0 && pListBoxData->wIndex >= 
                (pListBoxData->wCurFirPOS + pListBoxData->wPageRange *
                pListBoxData->wColumns))
            {
                pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                    pListBoxData->wIndex % pListBoxData->wPageRange -
                    pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
            }
        }
        else
        {
            if (pListBoxData->wIndex < pListBoxData->wCurFirPOS)
                pListBoxData->wCurFirPOS = pListBoxData->wIndex;
            else if (pListBoxData->wPageRange && pListBoxData->wIndex >= 
                (pListBoxData->wCurFirPOS + pListBoxData->wPageRange))
            {
                pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                    pListBoxData->wPageRange + 1;
            }
            
        }

        if (pListBoxData->bInitData)
            return lResult;
        
        if (pListBoxData->wCurFirPOS != wOldCurFirstPOS)
        {
            if (dwStyle & LBS_MULTICOLUMN)
            {
                if (((pListBoxData->wItemCount / pListBoxData->wPageRange) >
                    pListBoxData->wColumns) || (((pListBoxData->wItemCount / 
                    pListBoxData->wPageRange) == pListBoxData->wColumns) && 
                    (pListBoxData->wItemCount % pListBoxData->wPageRange)))
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_POS;
                    if (dwStyle & LBS_DISABLENOSCROLL)
                        ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                    
                    ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
                    
                    if (IsWindowVisible(hWnd))
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                    else
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
                }
            }
            else if (pListBoxData->bVScroll)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_POS;
                
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                
                if (IsWindowVisible(hWnd))
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                else
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
            }
        }
        
        if (IsWindowVisible(hWnd))
        {
            hdc = GetDC(hWnd);
            //MS Window also process this message with GetDC.
            LISTBOX_Paint(hWnd, hdc, pListBoxData, pOldCurNode, wOldIndex,
                wOldCurFirstPOS, TRUE, -1, -1, -1, -1);
            
            ReleaseDC(hWnd, hdc);
        }
    }
    return lResult;
}    

static LRESULT ProcessSetCurSel(HWND hWnd, PLISTBOXDATA pListBoxData, WPARAM wParam)
{
    LRESULT         lResult;
    DWORD           dwStyle;
    IWORD           index, wOldCurFirstPOS, wOldIndex;
    SCROLLINFO      ScrollInfo;
    HDC             hdc;

    lResult = FALSE;
    if (pListBoxData->bMultiSelect || wParam >= pListBoxData->wItemCount ||
        wParam < -1)
    {
        lResult = (LRESULT)LB_ERR;
        return lResult;
    }
    else
    {
        PLISTNODE pOldCurNode = pListBoxData->pCurNode;
        
        wOldCurFirstPOS = pListBoxData->wCurFirPOS;
        wOldIndex = pListBoxData->wIndex;
            
        index = (IWORD) wParam;     // item index       
        
        if (index == -1)    
        {
            //The old selected item change to a focus frame.
            pListBoxData->pCurNode = NULL;
            lResult = (LRESULT)LB_ERR;
        }
        else
        {
            lResult = (LRESULT)index;
            
            pListBoxData->pCurNode = FindNode(pListBoxData, index);
            
			if (pListBoxData->pCurNode)
			{
                pListBoxData->wVirtFocus = -1;
				pListBoxData->bSelected = 1;
			}
			else
	            lResult = (LRESULT)LB_ERR;
            
            pListBoxData->wIndex = index;
            
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & LBS_MULTICOLUMN)
            {
                if (pListBoxData->wIndex < pListBoxData->wCurFirPOS)
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                        pListBoxData->wIndex % pListBoxData->wPageRange;
                }
                else if (pListBoxData->wPageRange > 0 && pListBoxData->wIndex >= 
                    (pListBoxData->wCurFirPOS + pListBoxData->wPageRange *
                    pListBoxData->wColumns))
                {
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                        pListBoxData->wIndex % pListBoxData->wPageRange -
                        pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
                }
            }
            else
            {
                if (pListBoxData->wIndex < pListBoxData->wCurFirPOS)
                    pListBoxData->wCurFirPOS = pListBoxData->wIndex;
                else if (pListBoxData->wPageRange > 0 && pListBoxData->wIndex >= 
                    (pListBoxData->wCurFirPOS + pListBoxData->wPageRange - LINE_PRESCROLL))
                {
                    if (pListBoxData->wIndex < pListBoxData->wItemCount - 1)
                    {
                        pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                            pListBoxData->wPageRange + 1 + LINE_PRESCROLL;
                    }
                    else
                    {
                        pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                            pListBoxData->wPageRange + 1;
                    }
                }
            }

            if (pListBoxData->bInitData)
                return lResult;

            if (pListBoxData->wCurFirPOS != wOldCurFirstPOS)
            {
                if (dwStyle & LBS_MULTICOLUMN)
                {
                    if (((pListBoxData->wItemCount / pListBoxData->wPageRange) >
                        pListBoxData->wColumns) || (((pListBoxData->wItemCount / 
                        pListBoxData->wPageRange) == pListBoxData->wColumns) && 
                        (pListBoxData->wItemCount % pListBoxData->wPageRange)))
                    {
                        ScrollInfo.cbSize = sizeof(SCROLLINFO);
                        ScrollInfo.fMask = SIF_POS;
                        if (dwStyle & LBS_DISABLENOSCROLL)
                            ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                        
                        ScrollInfo.nPos = pListBoxData->wCurFirPOS / 
                            pListBoxData->wPageRange;
                        
                        if (IsWindowVisible(hWnd))
                            SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                        else
                            SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
                    }
                }
                else if (pListBoxData->bVScroll)
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_POS;
                    
                    if (dwStyle & LBS_DISABLENOSCROLL)
                        ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                    
                    ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                    
                    if (IsWindowVisible(hWnd))
                        SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                    else
                        SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
                }
            }
        }
        if ((pListBoxData->bRealFocus == TRUE) && (pListBoxData->pCurNode != NULL) && 
            (pListBoxData->pHead != pListBoxData->pTail))
        {
            if ((pListBoxData->pHead  != pListBoxData->pTail))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLDOWN | SCROLLUP | SCROLLMIDDLE, MASKALL);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if ((pListBoxData->bHScroll) &&
                (pListBoxData->wMaxTextLen > pListBoxData->byPageTextLen))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
            else if (!(pListBoxData->bHScroll) && 
                (pListBoxData->wItemCount > pListBoxData->wPageRange))
                SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
        }

        if (IsWindowVisible(hWnd))
        {
            hdc = GetDC(hWnd);
            
            LISTBOX_Paint(hWnd, hdc, pListBoxData, pOldCurNode, wOldIndex,
                wOldCurFirstPOS, TRUE, -1, -1, -1, -1);
            //MS Window also paint it with GetDC, but not InvalidateRect.
            
            ReleaseDC(hWnd, hdc);
        }
    }
    return lResult;
}

static LRESULT ProcessFindStringExact(HWND hWnd, PLISTBOXDATA pListBoxData, 
                               WPARAM wParam, LPARAM lParam)
{
    int             i;
    IWORD           index;
    PLISTNODE       p;
    LRESULT         lResult;
    LPCSTR          lpsz;

    lResult = FALSE;
    index = (IWORD)wParam;        // item before start of search 
    lpsz = (LPCSTR)lParam;  // address of search string 
    
    if (!lpsz || !strlen(lpsz))
    {
        lResult = (LRESULT)LB_ERR;
        return lResult;
    }
    
    if (index < 0 || index >= pListBoxData->wItemCount - 1)
    {
        p = pListBoxData->pHead;
        i = index = 0;
    }
    else
    {
        p = FindNode(pListBoxData, (IWORD)(index + 1));
        i = index = index + 1;
    }
    
    do
    {
        if (p == NULL)
            break;
        
        if (strlen(lpsz) == strlen(p->pNodeString))
        {
            if (stricmp(p->pNodeString, lpsz) == 0)
            {
                lResult = (LRESULT)i;
                return lResult;
            }
        }
        p = p->pNext;
        i++;
    }while(1);
    
    if (!lResult && index != 0)
    {
        p = pListBoxData->pHead;
        i = 0;
        do
        {
            if (p == NULL)
                break;
            
            if (strlen(lpsz) == strlen(p->pNodeString))
            {
                if (stricmp(p->pNodeString, lpsz) == 0)
                {
                    lResult = (LRESULT)i;
                    return lResult;
                }
            }
            p = p->pNext;
            i++;
        }while(i < index);
    }
    
    if (!lResult)
        lResult = (LRESULT)LB_ERR;

    return lResult;
}

static void ProcessResetContent(HWND hWnd, PLISTBOXDATA pListBoxData)
{
    SCROLLINFO      ScrollInfo, ScrollInfoOld;
    DWORD           dwStyle;

    FreeList(pListBoxData);
	FreeCache(pListBoxData);

    pListBoxData->wCurFirPOS = 0;
    pListBoxData->wStartTextPos = 0;
    pListBoxData->pCurNode = NULL;
    pListBoxData->wIndex = -1;
    pListBoxData->wVirtFocus = 0;
    pListBoxData->wMaxTextLen = 0;
    pListBoxData->wBitmapWidth[0] = 0;
    pListBoxData->wBitmapHeight[0] = 0;

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    if (dwStyle & LBS_MULTICOLUMN)
    {
        ScrollInfoOld.cbSize = sizeof(SCROLLINFO);
        ScrollInfoOld.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_HORZ, &ScrollInfoOld);
        
        if ((UINT)ScrollInfoOld.nMax >= ScrollInfoOld.nPage)
        {
            ScrollInfo.cbSize = sizeof(SCROLLINFO);
            ScrollInfo.fMask = SIF_ALL;
            
            if (dwStyle & LBS_DISABLENOSCROLL)
                ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
            
            ScrollInfo.nMin = 0;
            if (pListBoxData->wPageRange > 0)
            {
                ScrollInfo.nMax = pListBoxData->wItemCount / pListBoxData->wPageRange + 1;
                if (pListBoxData->wItemCount % pListBoxData->wPageRange)
                    ScrollInfo.nMax ++;
            }
            else
                ScrollInfo.nMax = 0;
            
            ScrollInfo.nPage = pListBoxData->wColumns;
            ScrollInfo.nPos = pListBoxData->wCurFirPOS;

            if (ScrollInfo.nMax != 0)
                ScrollInfo.nMax --;
            if (IsWindowVisible(hWnd))
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
            else
                SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
        }
    }
    else if (pListBoxData->bVScroll)
    {
        ScrollInfoOld.cbSize = sizeof(SCROLLINFO);
        ScrollInfoOld.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &ScrollInfoOld);
        
        if ((UINT)ScrollInfoOld.nMax >= ScrollInfoOld.nPage)
        {
            ScrollInfo.cbSize = sizeof(SCROLLINFO);
            ScrollInfo.fMask = SIF_ALL;
            
            if (dwStyle & LBS_DISABLENOSCROLL)
                ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
            
            ScrollInfo.nMin = 0;
            ScrollInfo.nMax = pListBoxData->wItemCount;
            ScrollInfo.nPage = pListBoxData->wPageRange;
            ScrollInfo.nPos = pListBoxData->wCurFirPOS;
            
            if (ScrollInfo.nMax != 0)
                ScrollInfo.nMax --;
            if (IsWindowVisible(hWnd))
                SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
            else
                SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
        }
    }
    if (pListBoxData->bHScroll)
    {
        ScrollInfo.cbSize = sizeof(SCROLLINFO);
        ScrollInfo.fMask = SIF_ALL;
        if (dwStyle & LBS_DISABLENOSCROLL)
            ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
        
        ScrollInfo.nMin = 0;
        ScrollInfo.nMax = pListBoxData->wMaxTextLen;
        ScrollInfo.nPage = pListBoxData->byPageTextLen;
        ScrollInfo.nPos = pListBoxData->wStartTextPos;
        
        if (ScrollInfo.nMax != 0)
            ScrollInfo.nMax --;
        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
        
    }
    SendMessage(pListBoxData->hParentWnd, PWM_SETSCROLLSTATE, NULL, MASKALL);
    //According MS Windows, paint it with InvalidaRect. 
    if (IsWindowVisible(hWnd))
    {
//        RECT    rect;

        //GetClientRect(hWnd, &rect);
        //ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
        //InvalidateRect(hWnd, &rect, TRUE);
        InvalidateRect(hWnd, NULL, TRUE);
    }
}

static LRESULT ProcessSetSel(HWND hWnd, PLISTBOXDATA pListBoxData, 
                               WPARAM wParam, LPARAM lParam)
{
    DWORD           dwStyle;
    int             i;
    IWORD           index;
    PLISTNODE       p;
    LRESULT         lResult;
    IWORD           wOldCurFirstPOS;
    SCROLLINFO      ScrollInfo;

    lResult = FALSE;

    if (pListBoxData->bMultiSelect)
    {
        index = (UINT) lParam;     // item index        
        
        if (index < -1 || index >= pListBoxData->wItemCount)
        {
            lResult = (LRESULT)LB_ERR;
            return lResult;
        }
        pListBoxData->wVirtFocus = -1;
        wOldCurFirstPOS = pListBoxData->wCurFirPOS;
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        
        p = pListBoxData->pHead;
        i = 0;
        do
        {
            if (p == NULL)
                break;
            
            if (index == -1)
                p->bSelect = (BOOL) wParam;   // selection flag 
            else if (i == index)
            {
                pListBoxData->wIndex = index;
                pListBoxData->pCurNode = p;
                p->bSelect = (BOOL) wParam;

                if (dwStyle & LBS_MULTICOLUMN)
                {
                    if (pListBoxData->wIndex < pListBoxData->wCurFirPOS)
                    {
                        pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                            pListBoxData->wIndex % pListBoxData->wPageRange;
                    }
                    else if (pListBoxData->wPageRange > 0 && pListBoxData->wIndex >= 
                        (pListBoxData->wCurFirPOS + pListBoxData->wPageRange *
                        pListBoxData->wColumns))
                    {
                        pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                            pListBoxData->wIndex % pListBoxData->wPageRange -
                            pListBoxData->wPageRange * (pListBoxData->wColumns - 1);
                    }
                }
                else
                {
                    if (pListBoxData->wIndex < pListBoxData->wCurFirPOS)
                        pListBoxData->wCurFirPOS = pListBoxData->wIndex;
                    else if (pListBoxData->wPageRange > 0 && pListBoxData->wIndex >= 
                        (pListBoxData->wCurFirPOS + pListBoxData->wPageRange))
                    {
                        pListBoxData->wCurFirPOS = pListBoxData->wIndex -
                            pListBoxData->wPageRange + 1;
                    }
                }

                break;
            }
            
            p = p->pNext;
            i++;
        } while(1);
        
        if (pListBoxData->wCurFirPOS != wOldCurFirstPOS)
        {
            if (dwStyle & LBS_MULTICOLUMN)
            {
                if (((pListBoxData->wItemCount / pListBoxData->wPageRange) >
                    pListBoxData->wColumns) || (((pListBoxData->wItemCount / 
                    pListBoxData->wPageRange) == pListBoxData->wColumns) && 
                    (pListBoxData->wItemCount % pListBoxData->wPageRange)))
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_POS;
                    if (dwStyle & LBS_DISABLENOSCROLL)
                        ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                    
                    ScrollInfo.nPos = pListBoxData->wCurFirPOS / pListBoxData->wPageRange;
                    
                    if (IsWindowVisible(hWnd))
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, TRUE);
                    else
                        SetScrollInfo(hWnd, SB_HORZ, &ScrollInfo, FALSE);
                }
            }
            else if (pListBoxData->bVScroll)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_POS;
                
                if (dwStyle & LBS_DISABLENOSCROLL)
                    ScrollInfo.fMask |= SIF_DISABLENOSCROLL;
                
                ScrollInfo.nPos = pListBoxData->wCurFirPOS;
                
                if (IsWindowVisible(hWnd))
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                else
                    SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, FALSE);
            }
        }
        //MS Window also paint it with InvalidateRect.
        if (IsWindowVisible(hWnd))
        {
            RECT    rect;
            
            GetClientRect(hWnd, &rect);
            ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);

            if (index == -1 || wOldCurFirstPOS != pListBoxData->wCurFirPOS)
                InvalidateRect(hWnd, &rect, TRUE);
            else
            {
                int     nItemHeight;
//                SIZE    size;
//                HDC     hdc;

//                hdc = GetDC(hWnd);
//                GetTextExtentPoint32(hdc, "T", -1, &size);
                nItemHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
//                ReleaseDC(hWnd, hdc);
//                nItemHeight = max(pListBoxData->wBitmapHeight, size.cy + SPACE);

                if (dwStyle & LBS_MULTICOLUMN)
                {
                    RECT rcList;
                    
                    SetRect(&rcList, rect.left, rect.top, rect.right, rect.bottom);
                    
                    rect.left += pListBoxData->nColumnWidth * 
                        ((index - pListBoxData->wCurFirPOS) / pListBoxData->wPageRange);
                    rect.right = rect.left + pListBoxData->nColumnWidth;
                    if (rect.right > rcList.right)
                        rect.right = rcList.right;
                        
                    rect.top += nItemHeight * ((index - pListBoxData->wCurFirPOS) %
                        pListBoxData->wPageRange);
                }
                else
                    rect.top += nItemHeight * (index - pListBoxData->wCurFirPOS);

                rect.bottom = rect.top + nItemHeight; 

                InvalidateRect(hWnd, &rect, TRUE);
            }
        }
    }
    else
        lResult = (LRESULT)LB_ERR;

    return lResult;
}

/**********************************************************************
 * Function     ProcessGetItemRect
 * Purpose      
 * Params       hWnd, pListBoxData, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT ProcessGetItemRect(HWND hWnd, PLISTBOXDATA pListBoxData,
                                  WPARAM wParam, LPARAM lParam)
{
    RECT rc, *prcItem = NULL;
    int nHeight = 0, nIndex = 0;

    nIndex = (int)wParam;

    if ((nIndex < pListBoxData->wCurFirPOS)
        || (nIndex > pListBoxData->wCurFirPOS + pListBoxData->wPageRange - 1))
    {
        return LB_ERR;
    }

    prcItem = (PRECT)lParam;

    GetClientRect(hWnd, &rc);
    nHeight = GetItemHeight(hWnd, pListBoxData, NULL, NULL);
    rc.top += (nIndex - pListBoxData->wCurFirPOS) * nHeight;
    rc.bottom = rc.top + nHeight;

    SetRect(prcItem, rc.left + 1, rc.top + 2, rc.right - 1, rc.bottom - 1);

    return 0;
}

/**********************************************************************
 * Function     ProcessSetAuxText
 * Purpose      
 * Params       hWnd, pListBoxData, wParam, lParam
 * Return       
 * Remarks      
 **********************************************************************/

static LRESULT ProcessSetAuxText(HWND hWnd, PLISTBOXDATA pListBoxData,
                                 WPARAM wParam, LPARAM lParam)
{
    int        nIndex = 0, nItemHeight = 0, nLine1Height = 0, nLine2Height = 0;
    IWORD      nLength = 0;
    DWORD      dwStyle = 0;
    RECT       rect, rcList;
    LPSTR      pszText = NULL;
    PLISTNODE  pNode = NULL;
    LRESULT    lResult = 0;
    
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (!(dwStyle & LBS_MULTILINE))
        return LB_ERR;

    nIndex = (int)LOWORD(wParam);
    pszText = (LPSTR)lParam;

    if ((nIndex < 0) || (nIndex >= pListBoxData->wItemCount)
        || (pszText == NULL))
    {
        return LB_ERR;
    }

    nLength = (HIWORD(wParam) == (WORD)-1) ? strlen(pszText) : HIWORD(wParam);

    pNode = FindNode(pListBoxData, (IWORD)nIndex);

    if (dwStyle & LBS_USERSTRINGS)
    {
        pNode->pszAuxText = pszText;
    }
    else
    {
        if (pNode->pszAuxText == NULL)
        {
            pNode->pszAuxText = LocalAlloc(LMEM_FIXED, nLength + 1);

            if (pNode->pszAuxText == NULL)
                return LB_ERR;
        }
        else if (nLength > (int)pNode->wAuxTextLen)
        {
            pNode->pszAuxText = LocalRealloc(pNode->pszAuxText, nLength + 1);

            if (pNode->pszAuxText == NULL)
                return LB_ERR;
        }
        
        strncpy((char*)pNode->pszAuxText, (const char*)pszText, nLength);
        pNode->pszAuxText[nLength] = '\0';
    }

    pNode->wAuxTextLen = nLength;
    lResult = (LRESULT)(pNode->wAuxTextLen);

    if (IsWindowVisible(hWnd) && nIndex >= pListBoxData->wCurFirPOS && 
        ((!(dwStyle & LBS_MULTICOLUMN) && 
        (nIndex < (pListBoxData->wCurFirPOS + pListBoxData->wPageRange))) || 
        ((dwStyle & LBS_MULTICOLUMN) && 
        (nIndex < (pListBoxData->wCurFirPOS + pListBoxData->wPageRange *
        pListBoxData->wColumns)))))
    {
        GetClientRect(hWnd, &rect);
        ConvertToLBClient(hWnd, &rect, pListBoxData, 0, dwStyle);
        SetRect(&rcList, rect.left, rect.top, rect.right, rect.bottom);
        
//        hdc = GetDC(hWnd);
        nItemHeight = GetItemHeight(hWnd, pListBoxData, &nLine1Height,
            &nLine2Height);
//        ReleaseDC(hWnd, hdc);

        if (dwStyle & LBS_MULTICOLUMN)
        {
            rect.left += pListBoxData->nColumnWidth * 
                ((nIndex - pListBoxData->wCurFirPOS) / pListBoxData->wPageRange);
            rect.right = rect.left + pListBoxData->nColumnWidth;
            if (rect.right > rcList.right)
                rect.right = rcList.right;
            
            rect.top += nItemHeight * ((nIndex - pListBoxData->wCurFirPOS) %
                pListBoxData->wPageRange);
        }
        else
        {
            rect.top += nItemHeight * (nIndex - pListBoxData->wCurFirPOS)
                + nLine1Height;
        }
        
        rect.bottom = rect.top + nLine2Height; 
        
        InvalidateRect(hWnd, &rect, TRUE);
//        UpdateWindow(hWnd);
    }
        
    return lResult;
}

/**********************************************************************
 * Function     ProcessTimer
 * Purpose      
 * Params       hWnd, pMenuWndData, wParam
 * Return       
 * Remarks      
 **********************************************************************/

static void ProcessTimer(HWND hWnd, PLISTBOXDATA pListBoxData, WPARAM wParam)
{
    switch (wParam)
    {
    case IDT_REPEAT:
        if (pListBoxData->nRepeats == 1)
        {
            KillTimer(hWnd, wParam);
            SetTimer(hWnd, IDT_REPEAT, ET_REPEAT_LATER, NULL);
        }
        keybd_event(pListBoxData->wKeyCode, 0, 0, 0);
        break;

    default:
        KillTimer(hWnd, wParam);
        break;
    }
}

static HBRUSH SetCtlColorListBox(HWND hWnd, HDC hdc, PLISTBOXDATA pListBoxData)
{
    HBRUSH  hBrush;

#if (!NOSENDCTLCOLOR)
    if (!pListBoxData->hParentWnd)
    {
#endif
        SetBkColor(hdc, GetSysColor(COLOR_WINDOW));
        SetTextColor(hdc, GetSysColor(COLOR_WINDOWTEXT));
        hBrush = (HBRUSH)(COLOR_WINDOW + 1);
#if (!NOSENDCTLCOLOR)
    }
    else
    {
        hBrush = (HBRUSH)SendMessage(pListBoxData->hParentWnd, (WORD)WM_CTLCOLORLISTBOX, 
            (WPARAM)hdc, (LPARAM)hWnd);
    }
#endif

    return hBrush;
}

static void ConvertToLBClient(HWND hWnd, RECT* pRect, PLISTBOXDATA pListBoxData,
                              int nBitsPixel, DWORD dwStyle)
{
#if (INTERFACE_MONO)
#else // INTERFACE_MONO
    if (!nBitsPixel)
        nBitsPixel = GetDeviceCaps(NULL, BITSPIXEL);

    if (nBitsPixel != 1 && nBitsPixel != 2 && !(dwStyle & LBS_COMBOLBOX))
    {
        pRect->left += 2;
        pRect->top += 2;
        
        if (!(dwStyle & WS_HSCROLL))
            pRect->bottom -= 2;
        else if (!(dwStyle & LBS_DISABLENOSCROLL) && 
            ((!(dwStyle & LBS_MULTICOLUMN) && !(pListBoxData->wMaxTextLen > 
            pListBoxData->byPageTextLen)) || 
            ((dwStyle & LBS_MULTICOLUMN) && pListBoxData->wPageRange &&
            pListBoxData->wItemCount <= pListBoxData->wPageRange * 
            (pListBoxData->wColumns - 1))))
        {
            pRect->bottom -= 2;
        }

        if (!pListBoxData->bVScroll)
            pRect->right -= 2;
        else if (!(dwStyle & LBS_DISABLENOSCROLL) && 
            (((!(dwStyle & LBS_MULTICOLUMN) && 
            (pListBoxData->wItemCount <= pListBoxData->wPageRange)) || 
            (dwStyle & LBS_MULTICOLUMN))))
        {
            pRect->right -= 2;
        }
    }
#endif // INTERFACE_MONO
}

/**********************************************************************
 * Function     GetItemHeight
 * Purpose      
 * Params       void
 * Return       
 * Remarks      
 **********************************************************************/

static int GetItemHeight(HWND hWnd, PLISTBOXDATA pListBoxData,
                         int *pnLine1Height, int *pnLine2Height)
{
    HDC    hdc = NULL;
    HFONT  hFont = NULL, hfontOld = NULL;
    SIZE   sizeLine1, sizeLine2;
    int    nItemHeight = 0, nLine1Height = 0, nLine2Height = 0;
    DWORD  dwStyle = 0;
    BOOL   bSetFont = FALSE;

    hdc = GetDC(hWnd);

    dwStyle = GetWindowLong(hWnd, GWL_STYLE);

    if (!(dwStyle & LBS_MULTILINE))
    {
        GetTextExtentPoint32(hdc, "T", -1, &sizeLine1);
        nItemHeight = max(pListBoxData->wBitmapHeight[0], sizeLine1.cy + SPACE);
    }
    else
    {
        hFont = (HFONT)SendMessage(pListBoxData->hParentWnd, WM_COMMAND,
            MAKEWPARAM(pListBoxData->wID, LBN_SETFONT), 0);

        bSetFont = IsValidObject(hdc, hFont);

        if (bSetFont)
        {
            hfontOld = SelectObject(hdc, (HGDIOBJ)hFont);
        }
        GetTextExtentPoint32(hdc, "T", -1, &sizeLine1);
        if (bSetFont)
        {
            SelectObject(hdc, (HGDIOBJ)hfontOld);
        }
        nLine1Height = max(pListBoxData->wBitmapHeight[0], sizeLine1.cy + SPACE);

        hFont = (HFONT)SendMessage(pListBoxData->hParentWnd, WM_COMMAND,
            MAKEWPARAM(pListBoxData->wID, LBN_SETFONT), 1);

        bSetFont = IsValidObject(hdc, hFont);

        if (!bSetFont)
        {
            GetFontHandle(&hFont, SMALL_FONT);
        }
        hfontOld = SelectObject(hdc, (HGDIOBJ)hFont);
        GetTextExtentPoint32(hdc, "T", -1, &sizeLine2);
        SelectObject(hdc, (HGDIOBJ)hfontOld);

        nLine2Height = max(pListBoxData->wBitmapHeight[0], sizeLine2.cy + SPACE - 1);
        nItemHeight = nLine1Height + nLine2Height;
    }

    ReleaseDC(hWnd, hdc);

    if ((pnLine1Height != NULL) && (pnLine2Height != NULL))
    {
        *pnLine1Height = nLine1Height;
        *pnLine2Height = nLine2Height;
    }

    return nItemHeight;
}

/**********************************************************************
 * Function     IsValidObject
 * Purpose      
 * Params       HGDIOBJ hGdiObj
 * Return       
 * Remarks      
 **********************************************************************/

static BOOL IsValidObject(HDC hdc, HGDIOBJ hGdiObj)
{
    HGDIOBJ hGdiObjOld = NULL;

    if (hGdiObj == NULL || hGdiObj == (HGDIOBJ)1)
        return FALSE;

    hGdiObjOld = SelectObject(hdc, hGdiObj);

    if (hGdiObjOld == NULL)
        return FALSE;

    SelectObject(hdc, hGdiObjOld);

    return TRUE;
}

static int Strnicmp (const char * first, const char * last)
{
	int f,l;
	
	do 
	{
		if (((f = (unsigned char)(*(first++))) >= 'A') &&
			(f <= 'Z') )
			f -= 'A' - 'a';
		
		if (((l = (unsigned char)(*(last++))) >= 'A') &&
			(l <= 'Z') )
			l -= 'A' - 'a';
		
	} while (*last && *first && (f == l));

	return( f - l );
}

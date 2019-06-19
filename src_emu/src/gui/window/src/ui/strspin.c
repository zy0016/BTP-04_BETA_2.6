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
#include "stdio.h"
#include "plx_pdaex.h"
#include "winpda.h"

typedef struct NODE
{
    PSTR            pNodeString;
    WORD            wIndex;
    struct NODE *   pNext;
    struct NODE *   pPrev;
}STRSPINBOXNODE, *PSTRSPINBOXNODE;
typedef struct
{
    DWORD           wState;
    WORD            wID;
    WORD            wItemCount;
    BYTE            bFocus;
    RECT            rcLeft;
    RECT            rcRight;
    RECT            rcText;
    PSTRSPINBOXNODE pCurNode;
    PSTRSPINBOXNODE pHeadNode;
    PSTRSPINBOXNODE pTailNode;
    
}STRSPINBOXDATA, *PSTRSPINBOXDATA;

#define SPACE   2
#define LARROW_WIDTH 8
#define LARROW_HEIGHT 7

static HBITMAP hBmpLeftArrow, hBmpRightArrow;

static LRESULT CALLBACK StrSpinBox_Func(HWND hWnd, UINT uMsgCmd, 
                                 WPARAM wParam, LPARAM lParam);
static void PaintString(HWND hWnd, HDC hDC, 
                      PSTRSPINBOXDATA pStrSpinBoxData, const RECT Rect);
static HBRUSH SetCtlColorStrSpin(PSTRSPINBOXDATA pStrSpinBoxData, 
                                 HWND hWnd, HDC hDC);
static LRESULT ProcessAddString(HWND hWnd, PSTRSPINBOXDATA pStrSpinBoxData, 
                                LPARAM lParam);
static LRESULT ProcessDeleteString(HWND hWnd, PSTRSPINBOXDATA pStrSpinBoxData, 
                                WPARAM wParam);

BOOL STRSPINBOX_RegisterClass(void)
{
    WNDCLASS    wc;
    int  nPathLen;
    char PathName[MAX_PATH];

    wc.style            = CS_OWNDC | CS_DEFWIN;
    wc.lpfnWndProc      = StrSpinBox_Func;
    wc.cbClsExtra       = NULL;
    wc.cbWndExtra       = sizeof(STRSPINBOXDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.hInstance        = NULL;
    wc.lpszClassName    = "STRSPINBOX";

    if (!RegisterClass(&wc))
        return FALSE;

    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);

    strcat(PathName, "strspinleft.bmp");
    hBmpLeftArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, LARROW_WIDTH,
        LARROW_HEIGHT, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "strspinright.bmp");
    hBmpRightArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, LARROW_WIDTH, 
        LARROW_HEIGHT, LR_LOADFROMFILE);
    /*hBmpLeftArrow = CreateBitmap(LRARROW_BMP_WIDTH, LRARROW_BMP_HEIGHT,
        1, 1, LeftArrowBmpData);
    hBmpRightArrow = CreateBitmap(LRARROW_BMP_WIDTH, LRARROW_BMP_HEIGHT,
        1, 1, RightArrowBmpData);
    hBmpLeftArrow = LoadImage(NULL, "ROM:left.bmp",IMAGE_BITMAP, LARROW_WIDTH,
        LARROW_HEIGHT, LR_LOADFROMFILE);
    hBmpRightArrow = LoadImage(NULL, "ROM:right.bmp",IMAGE_BITMAP, LARROW_WIDTH, 
        LARROW_HEIGHT, LR_LOADFROMFILE);*/
    return TRUE;
}

static LRESULT CALLBACK StrSpinBox_Func(HWND hWnd, UINT uMsgCmd, 
                                 WPARAM wParam, LPARAM lParam)
{
    PCREATESTRUCT   pCreateStruct;
    PSTRSPINBOXDATA pStrSpinBoxData;
    static RECT     rcClient;
    int             nHeightMin, nWidthMin, nSpaceVert, x, y, nMode = 0;
    SIZE            size;
    PAINTSTRUCT     ps;
    LRESULT         lResult;
    HDC             hDC;
    HWND            hWndParent;
    HBRUSH          hBrush;
    DWORD           dwStyle;
    POINT           pt;
    PSTRSPINBOXNODE p;
    HFONT           hFont;
    BOOL            fRedraw;
    COLORREF        cr = 0;

    pStrSpinBoxData = (PSTRSPINBOXDATA)GetUserData(hWnd);
    lResult = 0;

    switch(uMsgCmd)
    {
    case WM_CREATE:
        pCreateStruct = (PCREATESTRUCT)lParam;
        pStrSpinBoxData->wID = (WORD)(DWORD)pCreateStruct->hMenu;
        pStrSpinBoxData->wState = pCreateStruct->style;
        pStrSpinBoxData->bFocus = 0;
        pStrSpinBoxData->pCurNode = NULL;
        pStrSpinBoxData->pHeadNode = NULL;
        pStrSpinBoxData->pTailNode = NULL;
        pStrSpinBoxData->wItemCount = 0;

        lResult = TRUE;
        break;

    case WM_DESTROY:
        {
            PSTRSPINBOXNODE p;
            pStrSpinBoxData->pCurNode = NULL;
            pStrSpinBoxData->pTailNode = NULL;
            while(pStrSpinBoxData->pHeadNode)
            {
                p = pStrSpinBoxData->pHeadNode;
                pStrSpinBoxData->pHeadNode = p->pNext;
                LocalFree((HANDLE)p->pNodeString);
                LocalFree((HANDLE)p);
            }
            break;
        }

    case WM_SIZE:
        hDC = GetDC(hWnd);
        GetClientRect(hWnd, &rcClient);
        GetTextExtentPoint32(hDC, "T", -1, &size);
        nHeightMin = max(size.cy, LARROW_HEIGHT);
        nWidthMin =  2 * LARROW_WIDTH + 4 * SPACE + size.cx;

        nSpaceVert = (rcClient.bottom - rcClient.top - LARROW_HEIGHT) / 2;

        if (pStrSpinBoxData->wState & SSBS_ARROWRIGHT)
        {
            SetRect(&pStrSpinBoxData->rcLeft, rcClient.right - 2 * (LARROW_WIDTH + SPACE),
                rcClient.top + nSpaceVert, rcClient.right - (LARROW_WIDTH + 2 * SPACE),
                rcClient.bottom);
            SetRect(&pStrSpinBoxData->rcRight, rcClient.right - (LARROW_WIDTH + SPACE) + 1, 
                rcClient.top + nSpaceVert, rcClient.right - SPACE + 1, rcClient.bottom);
            SetRect(&pStrSpinBoxData->rcText, rcClient.left, rcClient.top,
                pStrSpinBoxData->rcLeft.left, rcClient.bottom);
        }
        else
        {
            SetRect(&pStrSpinBoxData->rcLeft, rcClient.left + SPACE, rcClient.top + nSpaceVert,
                rcClient.left + LARROW_WIDTH + SPACE, rcClient.bottom);
            SetRect(&pStrSpinBoxData->rcRight, rcClient.right - LARROW_WIDTH - SPACE, 
                rcClient.top + nSpaceVert, rcClient.right, rcClient.bottom);
            SetRect(&pStrSpinBoxData->rcText, pStrSpinBoxData->rcLeft.right + SPACE, rcClient.top,
                pStrSpinBoxData->rcRight.left - SPACE, rcClient.bottom);
        }

        InvalidateRect(hWnd, &rcClient, TRUE);
        ReleaseDC(hWnd, hDC);
        break;

    case WM_SETFONT:
        hFont = (HFONT)wParam;
        fRedraw = lParam;
        hDC = GetDC(hWnd);
        SelectObject(hDC, hFont);
        ReleaseDC(hWnd, hDC);

        if (fRedraw && IsWindowVisible(hWnd))
            InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_PAINT:
        hDC = BeginPaint(hWnd, &ps);
        PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
        if (!(pStrSpinBoxData->wState & SSBS_ARROWRIGHT)
            || ((pStrSpinBoxData->wState & SSBS_ARROWRIGHT)
            && (pStrSpinBoxData->bFocus) && (pStrSpinBoxData->wItemCount > 1)))
        {
            nMode = SetBkMode(hDC, NEWTRANSPARENT);
            cr = SetBkColor(hDC, RGB(0, 0, 0));
            BitBlt(hDC, pStrSpinBoxData->rcLeft.left, pStrSpinBoxData->rcLeft.top, LARROW_WIDTH,
                LARROW_HEIGHT, (HDC)hBmpLeftArrow, 0, 0, ROP_SRC);
            BitBlt(hDC, pStrSpinBoxData->rcRight.left, pStrSpinBoxData->rcRight.top, LARROW_WIDTH,
                LARROW_HEIGHT, (HDC)hBmpRightArrow, 0, 0, ROP_SRC);
            SetBkColor(hDC, cr);
            SetBkMode(hDC, nMode);
        }
        EndPaint(hWnd, &ps);
        break;

    case WM_ERASEBKGND:
        hDC = (HDC)wParam;
        hBrush = SetCtlColorStrSpin(pStrSpinBoxData, hWnd, hDC);
        GetClientRect(hWnd, &rcClient);
        FillRect(hDC, &rcClient, hBrush);
        lResult = (LRESULT)TRUE;
        break;

//#ifndef SCP_NOKEYBOARD
    case WM_KEYDOWN:
        switch(wParam)
        {
        case VK_UP:
            hWndParent = GetParent(hWnd);
            if (pStrSpinBoxData->wState & CS_NOSYSCTRL)
                SendMessage(hWndParent, uMsgCmd, wParam, lParam);
            else if (hWndParent)
            {
                SetFocus(GetNextDlgTabItem(hWndParent, hWnd, TRUE));
                return 0;
            }
            break;
        case VK_LEFT:
            if (pStrSpinBoxData->pCurNode == NULL)
                break;
            pStrSpinBoxData->pCurNode = pStrSpinBoxData->pCurNode->pNext;
            if (pStrSpinBoxData->pCurNode == NULL)
                pStrSpinBoxData->pCurNode = pStrSpinBoxData->pHeadNode;
            hDC = GetDC(hWnd);
            SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(
                pStrSpinBoxData->wID, SSBN_ERASECTLBK),
                (LPARAM)(&pStrSpinBoxData->rcText));
//            PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
            ReleaseDC(hWnd, hDC);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & WS_CHILD)
                hWndParent = GetParent(hWnd);
            else
                hWndParent = GetWindow(hWnd, GW_OWNER);
            SendMessage(hWndParent, WM_COMMAND, 
                MAKELONG(pStrSpinBoxData->wID, SSBN_CHANGE), (LPARAM)hWnd);
            break;

        case VK_DOWN:
            
            hWndParent = GetParent(hWnd);
            if (pStrSpinBoxData->wState & CS_NOSYSCTRL)
                SendMessage(hWndParent, uMsgCmd, wParam, lParam);
            else if (hWndParent)
            {
                SetFocus(GetNextDlgTabItem(hWndParent, hWnd, FALSE));
                return 0;
            }
            break;
        case VK_RIGHT:
            if (pStrSpinBoxData->pCurNode == NULL)
                break;
            pStrSpinBoxData->pCurNode = pStrSpinBoxData->pCurNode->pPrev;
            if (pStrSpinBoxData->pCurNode == NULL)
                pStrSpinBoxData->pCurNode = pStrSpinBoxData->pTailNode;
            hDC = GetDC(hWnd);
            SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(
                pStrSpinBoxData->wID, SSBN_ERASECTLBK),
                (LPARAM)(&pStrSpinBoxData->rcText));
//            PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
            ReleaseDC(hWnd, hDC);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & WS_CHILD)
                hWndParent = GetParent(hWnd);
            else
                hWndParent = GetWindow(hWnd, GW_OWNER);
            SendMessage(hWndParent, WM_COMMAND, 
                MAKELONG(pStrSpinBoxData->wID, SSBN_CHANGE), (LPARAM)hWnd);
            break;

        case VK_F10:
		case VK_RETURN:
        case VK_F5:
            hWndParent = GetParent(hWnd);
			SendMessage(hWndParent, WM_KEYDOWN, wParam, lParam);
			return 0;
        }
        break;

    case WM_KEYUP:
        {
            switch (wParam)
            {
            case VK_DOWN:
            case VK_UP:
                if (pStrSpinBoxData->wState & CS_NOSYSCTRL)
                {
                    SendMessage(GetParent(hWnd), WM_KEYUP, wParam, lParam);
                }
                break;
                
            default:
                break;
            }
        }
        break;
//#endif //SCP_NOKEYBOARD

    case WM_PENDOWN:

//#ifndef _EMULATE_
//		if (IsWindowEnabled(hWnd))
//			
//#endif //_EMULATE_
        if (GetFocus() != hWnd)
        {
            SetFocus(hWnd);
            break;
        }

        SetCapture(hWnd);
        SetFocus(hWnd);
        break;

    case WM_PENUP :
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        if (GetCapture() != hWnd)
            break;
        ReleaseCapture();
        
        GetClientRect(hWnd, &rcClient);
        if (!PtInRectXY(&rcClient, x, y))
            break;
        pStrSpinBoxData->bFocus = 1;
        
        if (pStrSpinBoxData->pCurNode == NULL)
            break;
        if (PtInRectXY(&pStrSpinBoxData->rcLeft, x, y))
        {
            pStrSpinBoxData->pCurNode = pStrSpinBoxData->pCurNode->pNext;
            if (pStrSpinBoxData->pCurNode == NULL)
                pStrSpinBoxData->pCurNode = pStrSpinBoxData->pHeadNode;
            hDC = GetDC(hWnd);
            PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
            ReleaseDC(hWnd, hDC);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & WS_CHILD)
                hWndParent = GetParent(hWnd);
            else
                hWndParent = GetWindow(hWnd, GW_OWNER);
            SendMessage(hWndParent, WM_COMMAND, 
                MAKELONG(pStrSpinBoxData->wID, SSBN_CHANGE), (LPARAM)hWnd);
            
            break;
        }
        else if (PtInRectXY(&pStrSpinBoxData->rcRight, x, y))
        {
            pStrSpinBoxData->pCurNode = pStrSpinBoxData->pCurNode->pPrev;
            if (pStrSpinBoxData->pCurNode == NULL)
                pStrSpinBoxData->pCurNode = pStrSpinBoxData->pTailNode;
            hDC = GetDC(hWnd);
            PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
            ReleaseDC(hWnd, hDC);
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & WS_CHILD)
                hWndParent = GetParent(hWnd);
            else
                hWndParent = GetWindow(hWnd, GW_OWNER);
            SendMessage(hWndParent, WM_COMMAND, 
                MAKELONG(pStrSpinBoxData->wID, SSBN_CHANGE), (LPARAM)hWnd);
            break;
        }
        /*else
        {
            dwStyle = GetWindowLong(hWnd, GWL_STYLE);
            if (dwStyle & WS_CHILD)
                hWndParent = GetParent(hWnd);
            else
                hWndParent = GetWindow(hWnd, GW_OWNER);
            nReturn = SendMessage(hWndParent, WM_COMMAND, 
                MAKELONG(pStrSpinBoxData->wID, SSBN_CLICKED), (LPARAM)hWnd);
            if (nReturn == TRUE)
            {
                pStrSpinBoxData->pCurNode = pStrSpinBoxData->pCurNode->pPrev;
                if (pStrSpinBoxData->pCurNode == NULL)
                    pStrSpinBoxData->pCurNode = pStrSpinBoxData->pTailNode;
                hDC = GetDC(hWnd);
                PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
                ReleaseDC(hWnd, hDC);
                SendMessage(hWndParent, WM_COMMAND, 
                    MAKELONG(pStrSpinBoxData->wID, SSBN_CHANGE), (LPARAM)hWnd);
                break;
            }
            else
                break;

        }*/
        break;

    case WM_SETFOCUS:
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (dwStyle & WS_CHILD)
            hWndParent = GetParent(hWnd);
        else
            hWndParent = GetWindow(hWnd, GW_OWNER);
        
        SendMessage(hWndParent, PWM_SETSCROLLSTATE, SCROLLLEFT | SCROLLRIGHT | 
            SCROLLUP | SCROLLDOWN, MASKALL);

		DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
        hDC = GetDC(hWnd);
        pStrSpinBoxData->bFocus = 1;
        PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
        ReleaseDC(hWnd, hDC);
        SendMessage(hWndParent, WM_COMMAND, 
            MAKELONG(pStrSpinBoxData->wID, SSBN_SETFOCUS), (LPARAM)hWnd);
        break;


    case WM_KILLFOCUS :
        DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
        pStrSpinBoxData->bFocus = 0;
        hDC = GetDC(hWnd);
        PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
        dwStyle = GetWindowLong(hWnd, GWL_STYLE);
        if (dwStyle & WS_CHILD)
            hWndParent = GetParent(hWnd);
        else
            hWndParent = GetWindow(hWnd, GW_OWNER);
        SendMessage(hWndParent, WM_COMMAND, 
            MAKELONG(pStrSpinBoxData->wID, SSBN_KILLFOCUS), (LPARAM)hWnd);
        ReleaseDC(hWnd, hDC);
        break;

    case WM_GETDLGCODE:
        lResult = (LRESULT)DLGC_WANTARROWS | DLGC_WANTALLKEYS;
        break;

    case WM_CANCELMODE :
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		SendMessage(hWnd, WM_PENUP, 0, MAKELPARAM(pt.x, pt.y));
		break;

    case SSBM_ADDSTRING:
        lResult = ProcessAddString(hWnd, pStrSpinBoxData, lParam);
        break;
    case SSBM_DELETESTRING:
        lResult = ProcessDeleteString(hWnd, pStrSpinBoxData, wParam);
        break;

    case SSBM_SETCURSEL:
        if (wParam >= pStrSpinBoxData->wItemCount)
            return FALSE;
        if (pStrSpinBoxData->pCurNode == NULL)
            pStrSpinBoxData->pCurNode = pStrSpinBoxData->pTailNode;
        while(pStrSpinBoxData->pCurNode->wIndex != wParam)
        {
            pStrSpinBoxData->pCurNode = pStrSpinBoxData->pCurNode->pPrev;
            if (pStrSpinBoxData->pCurNode == NULL)
                pStrSpinBoxData->pCurNode = pStrSpinBoxData->pTailNode;
        }
        hDC = GetDC(hWnd);
        PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
        ReleaseDC(hWnd, hDC);
        lResult = TRUE;
        break;

    case SSBM_GETCOUNT:
        lResult = pStrSpinBoxData->wItemCount;
        break;

    case SSBM_GETCURSEL:
        if (pStrSpinBoxData->pCurNode == NULL)
            return FALSE;
        lResult = pStrSpinBoxData->pCurNode->wIndex;
        break;

    case SSBM_SETTEXT:
        {
            int len;

            p = pStrSpinBoxData->pHeadNode;
            if (p == NULL)
                return FALSE;
            while (p->wIndex != wParam)
            {
                p = p->pNext;
                if (p == NULL)
                    return FALSE;
            }
            LocalFree(p->pNodeString);
            len = strlen((char *)lParam);
            p->pNodeString = LocalAlloc(LMEM_FIXED, len + 1);
            if (p->pNodeString == NULL)
                return FALSE;
            strcpy(p->pNodeString, (char *)lParam);
            hDC = GetDC(hWnd);
            SendMessage(GetParent(hWnd), WM_COMMAND, MAKEWPARAM(
                pStrSpinBoxData->wID, SSBN_ERASECTLBK),
                (LPARAM)(&pStrSpinBoxData->rcText));
//            PaintString(hWnd, hDC, pStrSpinBoxData, pStrSpinBoxData->rcText);
             ReleaseDC(hWnd, hDC);
            lResult = TRUE;

            break;
        }

    case SSBM_GETTEXTLEN:
        p = pStrSpinBoxData->pHeadNode;
        if (p == NULL)
            return FALSE;
        while (p->wIndex != wParam)
        {
            p = p->pNext;
            if (p == NULL)
                return FALSE;
        }
        lResult = strlen(p->pNodeString);
        break;

    case SSBM_GETTEXT:
        p = pStrSpinBoxData->pHeadNode;
        if (p == NULL)
            return FALSE;
        while (p->wIndex != wParam)
        {
            p = p->pNext;
            if (p == NULL)
                return FALSE;
        }
        strcpy((char*)lParam, p->pNodeString);
        lResult = strlen((char *)lParam);
        break;

    default :
        return DefWindowProc(hWnd, uMsgCmd, wParam, lParam);    
    }
    return lResult;
}
 

static void PaintString(HWND hWnd, HDC hDC, 
                      PSTRSPINBOXDATA pStrSpinBoxData, const RECT rcText)
{
    COLORREF    clrBKOld, clrTextOld = RGB(0, 0, 0);
    SIZE        size;
    RECT        rc;
    int         nMode = 0;
    char        szDisplay[128];
    
    GetTextExtentPoint32(hDC, "T", -1, &size);
    if (pStrSpinBoxData->wItemCount == 0)
        return;
    if (pStrSpinBoxData->pCurNode == NULL)
        return;

    if (pStrSpinBoxData->bFocus)
    {
        clrBKOld = SetBkColor(hDC, GetWindowColor(FOCUS_COLOR));
        CopyRect(&rc, &rcText);
        rc.top ++;
        rc.bottom --;
//        ClearRect(hDC, &rc, GetWindowColor(FOCUS_COLOR));
        clrTextOld = SetTextColor(hDC, COLOR_WHITE);
    }
    else
    {
        clrBKOld = SetBkColor(hDC, COLOR_WHITE);
        CopyRect(&rc, &rcText);
        rc.top ++;
        rc.bottom --;
        ClearRect(hDC, &rc, COLOR_TRANSBK);
    }
    //clrBKOld = SetBkColor(hDC, GetWindowColor(CAP_COLOR));
    nMode = SetBkMode(hDC, TRANSPARENT);

    if (pStrSpinBoxData->wItemCount <= 1)
    {
        GetClientRect(hWnd, &rc);
    }
    else
    {
        CopyRect(&rc, &rcText);
    }

    GetExtentFittedText(hDC, pStrSpinBoxData->pCurNode->pNodeString,
            -1, szDisplay, 128, rc.right - rc.left, '.', 3);

    if (pStrSpinBoxData->wState & SSBS_LEFT)
        DrawText(hDC, szDisplay, // pStrSpinBoxData->pCurNode->pNodeString,
            -1, (PRECT)&rc, DT_LEFT | DT_VCENTER);
    else if (pStrSpinBoxData->wState & SSBS_RIGHT)
        DrawText(hDC, szDisplay, // pStrSpinBoxData->pCurNode->pNodeString,
            -1, (PRECT)&rc, DT_RIGHT | DT_VCENTER);
    else
        DrawText(hDC, szDisplay, // pStrSpinBoxData->pCurNode->pNodeString,
            -1, (PRECT)&rc, DT_CENTER | DT_VCENTER);

    if (pStrSpinBoxData->bFocus)
        SetTextColor(hDC, clrTextOld);

    SetBkColor(hDC, clrBKOld);
    SetBkMode(hDC, nMode);
}

static HBRUSH SetCtlColorStrSpin(PSTRSPINBOXDATA pStrSpinBoxData, HWND hWnd, HDC hDC)
{
    HBRUSH  hBrush;
    DWORD   dwStyle;
    HWND    hWndParent;
    
    dwStyle = GetWindowLong(hWnd, GWL_STYLE);
    
    if (dwStyle & WS_CHILD)
        hWndParent = GetParent(hWnd);
    else
        hWndParent = GetWindow(hWnd, GW_OWNER);

#if (!NOSENDCTLCOLOR)
    if (!hWndParent)
    {
#endif
        SetBkColor(hDC, GetSysColor(COLOR_WINDOW));
        SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
        hBrush = (HBRUSH)(COLOR_WINDOW + 1);
#if (!NOSENDCTLCOLOR)
    }
    else
    {
        hBrush = (HBRUSH)SendMessage(hWndParent, (WORD)WM_CTLCOLOREDIT, 
            (WPARAM)hDC, (LPARAM)hWnd);
    }
#endif

    return hBrush;
}

static LRESULT ProcessAddString(HWND hWnd, PSTRSPINBOXDATA pStrSpinBoxData, 
                                LPARAM lParam)
{
    LRESULT         lResult;
    LPCSTR          lpsz;
    PSTRSPINBOXNODE p;
    int             len;

    lpsz = (LPCSTR)lParam;   // address of string to add
    
    if (lpsz == NULL)
    {
        lResult = (LRESULT)-1;
        return lResult;
    }

    p = (PSTRSPINBOXNODE)LocalAlloc(LMEM_FIXED, sizeof(STRSPINBOXNODE));
    if (p == NULL)
        return -1;
    len = strlen((char *)lParam);
    p->pNodeString = (char *)LocalAlloc(LMEM_FIXED, sizeof(char) * (len + 1));
    if (p->pNodeString == NULL)
        return -1;
    //LocalFree(p->pNodeString);
    strcpy(p->pNodeString, (char *)lParam);
    p->pNext = NULL;
    p->pPrev = NULL;
    p->wIndex = pStrSpinBoxData->wItemCount;
    pStrSpinBoxData->wItemCount ++;

    if (pStrSpinBoxData->pHeadNode == NULL)
    {
        pStrSpinBoxData->pHeadNode = p;
        pStrSpinBoxData->pTailNode = p;
        pStrSpinBoxData->pCurNode = p;
    }
    else
    {
        pStrSpinBoxData->pHeadNode->pPrev = p;
        p->pNext = pStrSpinBoxData->pHeadNode;
        pStrSpinBoxData->pHeadNode = p;
        pStrSpinBoxData->pCurNode = p;
    }
    lResult = pStrSpinBoxData->wItemCount - 1;
    return lResult;
}

static LRESULT ProcessDeleteString(HWND hWnd, PSTRSPINBOXDATA pStrSpinBoxData, 
                                WPARAM wParam)
{
    PSTRSPINBOXNODE p, pPre, pTmp;
    
    p = pStrSpinBoxData->pHeadNode;
    pPre = NULL;
    while (p)
    {
        if (wParam == p->wIndex)
            break;
        pPre = p;
        p = p->pNext;
    }
    if (p == NULL)
        return FALSE;

    pTmp = pStrSpinBoxData->pHeadNode;
    while (pTmp != p)
    {
         pTmp->wIndex --;
         pTmp = pTmp->pNext;
    }
    if (p == pStrSpinBoxData->pCurNode)
    {
        if (p->pNext != NULL)
            pStrSpinBoxData->pCurNode = p->pNext;
        else
            pStrSpinBoxData->pCurNode = p->pPrev;
    }
    if (pPre == NULL)
    {
        if (p->pNext == NULL)
        {
            pStrSpinBoxData->pTailNode = NULL;
            pStrSpinBoxData->pHeadNode = NULL;
        }
        else
        {
            pStrSpinBoxData->pHeadNode = p->pNext;
            pStrSpinBoxData->pHeadNode->pPrev = NULL;
        }
    }
    else
    {
        if (p->pNext != NULL)
        {
            pPre->pNext = p->pNext;
            p->pNext->pPrev = pPre;
        }
        else
        {
            pPre ->pNext = NULL;
            pStrSpinBoxData->pTailNode = pPre;
        }
    }
    
    LocalFree((HANDLE)p->pNodeString);
    LocalFree((HANDLE)p);
    pStrSpinBoxData->wItemCount --;
    return TRUE;
}

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements spin control.
 *            
\**************************************************************************/

#include "hpwin.h"
#include "windowx.h"
#include "control.h"
#include "string.h"
#include "stdio.h"

#define Range_Max 10
#define Range_Min 1
#define Range_Inc 1
#define SPACE     1

typedef struct
{
    IWORD    wRangeMax;
    IWORD    wRangeMin;
    IWORD    wRangeInc;
    IWORD    wData;
    DWORD    wState;
    WORD     wID;
    HWND     hParentWnd;
    BYTE     bFocus;
    BYTE     bRightDisabled;
    BYTE     bLeftDisabled;
}NUMSPINBOXDATA, *PNUMSPINBOXDATA;

static LRESULT CALLBACK NumSpinBox_Func(HWND hWnd, UINT wMsgCmd, 
                                 WPARAM wParam, LPARAM lParam);
static void PaintText(HWND hWnd, HDC hDC, PNUMSPINBOXDATA pNumSpinBoxData, const RECT Rect);
static void PaintArrow(HDC hDC, BOOL bLeft, BOOL bGrayed, const RECT Rect);
static HBRUSH SetCtlColorNumSpin(PNUMSPINBOXDATA pNumSpinBoxData, HWND hWnd, HDC hdc);

static HBITMAP hBmpLeftArrow, hBmpRightArrow,
                hBmpLeftArrowDisabled, hBmpRightArrowDisabled; 

BOOL SPINBOX_RegisterClass(void)
{
    WNDCLASS    wcl;

    wcl.style           = CS_PARENTDC;
    wcl.lpfnWndProc     = NumSpinBox_Func;
    wcl.cbClsExtra      = NULL;
    wcl.cbWndExtra      = sizeof(NUMSPINBOXDATA);
    wcl.hbrBackground   = NULL;
    wcl.hCursor         = LoadCursor(NULL, IDC_ARROW);;
    wcl.hIcon           = NULL;
    wcl.lpszMenuName    = NULL;
    wcl.lpszClassName   = "NumSpinBox";

    if (!RegisterClass(&wcl))
        return FALSE;

    hBmpLeftArrow = CreateSharedBitmap(LRARROW_BMP_WIDTH, LRARROW_BMP_HEIGHT, 
        1, 1, LeftArrowBmpData);
    hBmpRightArrow = CreateSharedBitmap(LRARROW_BMP_WIDTH, LRARROW_BMP_HEIGHT, 
        1, 1, RightArrowBmpData);

#if (INTERFACE_MONO)
    hBmpLeftArrowDisabled = CreateBitmap(LRARROW_BMP_WIDTH, 
        LRARROW_BMP_HEIGHT, 1, 1, LeftArrowDisabledBmpData);
    hBmpRightArrowDisabled = CreateBitmap(LRARROW_BMP_WIDTH, 
        LRARROW_BMP_HEIGHT, 1, 1, RightArrowDisabledBmpData);
#endif // INTERFACE_MONO
    
    return TRUE;
}

static LRESULT CALLBACK NumSpinBox_Func(HWND hWnd, UINT uMsgCmd, 
                                 WPARAM wParam, LPARAM lParam) 
{
    PCREATESTRUCT   pCreateStruct;
    PNUMSPINBOXDATA pNumSpinBoxData;
    PAINTSTRUCT     ps;
    LRESULT         lResult;
    static RECT     rcClient, rcLeft, rcRight, rcText;
    HDC             hDC;
    HBRUSH          hBrush;
    SIZE            size;
    int             x, y, nSpaceVert, nHeightMin, nWidthMin;
    POINT           pt;

    pNumSpinBoxData = (PNUMSPINBOXDATA)GetUserData(hWnd);
    lResult = 0;

    switch(uMsgCmd)
    {
    case WM_CREATE:
        pCreateStruct = (PCREATESTRUCT)lParam;
        pNumSpinBoxData->wID = (WORD)(DWORD)pCreateStruct->hMenu;
        pNumSpinBoxData->hParentWnd = pCreateStruct->hwndParent;
        pNumSpinBoxData->wState = pCreateStruct->style;
        pNumSpinBoxData->wRangeMax = Range_Max;
        pNumSpinBoxData->wRangeMin = Range_Min;
        pNumSpinBoxData->wRangeInc = Range_Inc;
        pNumSpinBoxData->wData     = Range_Min;

        pNumSpinBoxData->bFocus = 0;
        pNumSpinBoxData->bRightDisabled = 0;
        pNumSpinBoxData->bLeftDisabled = 1;
        lResult = TRUE;
        break;

    case WM_DESTROY :
        break;

    case WM_SIZE :
        hDC = GetDC(hWnd);
        GetClientRect(hWnd, &rcClient);
        GetTextExtentPoint32(hDC, "t", -1, &size);
        nHeightMin = max(size.cy, LRARROW_BMP_HEIGHT);
        nWidthMin =  2 * LRARROW_BMP_WIDTH + 4 * SPACE + size.cx;

        ASSERT(rcClient.right - rcClient.left >= nWidthMin);
        ASSERT(rcClient.bottom - rcClient.top >= nHeightMin);

        nSpaceVert = (rcClient.bottom - rcClient.top - LRARROW_BMP_HEIGHT) / 2;       
        SetRect(&rcLeft, rcClient.left + SPACE, rcClient.top + nSpaceVert,
            rcClient.left + LRARROW_BMP_WIDTH + SPACE, rcClient.bottom);
        SetRect(&rcRight, rcClient.right - LRARROW_BMP_WIDTH - SPACE, 
            rcClient.top + nSpaceVert, rcClient.right, rcClient.bottom);
        SetRect(&rcText, rcLeft.right + SPACE, rcClient.top,
            rcRight.left - SPACE, rcClient.bottom);

        InvalidateRect(hWnd, &rcClient, TRUE);
        break;

    case WM_PAINT :
        hDC = BeginPaint(hWnd, &ps);        
        PaintText(hWnd, hDC, pNumSpinBoxData, rcText);
        
        if (pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin)
            PaintArrow(hDC, 1, 0, rcLeft);
        else
            PaintArrow(hDC, 1, 1, rcLeft);
                 
        if (pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax)
            PaintArrow(hDC, 0, 0, rcRight);
        else
            PaintArrow(hDC, 0, 1, rcRight);
        EndPaint(hWnd, &ps);

        break;

    case WM_ERASEBKGND:
        hDC = (HDC)wParam;
        hBrush = SetCtlColorNumSpin(pNumSpinBoxData, hWnd, hDC);
        FillRect(hDC, &rcClient, hBrush);
        lResult = (LRESULT)TRUE;
        break;

    case WM_SETFOCUS:
        hDC = GetDC(hWnd);
        pNumSpinBoxData->bFocus = 1;
        SetCapture(hWnd);
        PaintText(hWnd, hDC, pNumSpinBoxData, rcText);
        ReleaseDC(hWnd, hDC);
        
        SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
            MAKELONG(pNumSpinBoxData->wID, NSBN_SETFOCUS), (LPARAM)hWnd);
        break;

    case WM_KILLFOCUS :
        pNumSpinBoxData->bFocus = 0;
        hDC = GetDC(hWnd);
        PaintText(hWnd, hDC, pNumSpinBoxData, rcText);
        ReleaseDC(hWnd, hDC);
        ReleaseCapture();
        SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
            MAKELONG(pNumSpinBoxData->wID, NSBN_KILLFOCUS), (LPARAM)hWnd);
        break;

    case WM_GETDLGCODE:
        lResult = (LRESULT)DLGC_WANTARROWS | DLGC_WANTALLKEYS;
        break;

    case WM_CANCELMODE :
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		SendMessage(hWnd, WM_PENUP, 0, MAKELPARAM(pt.x, pt.y));
		break;

#if (!NOKEYBOARD)
    case WM_KEYDOWN :
        hDC = GetDC(hWnd);
        switch(wParam)
        {
        case VK_UP:
        case VK_LEFT:
            if (pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin)
            {
                pNumSpinBoxData->wData -= pNumSpinBoxData->wRangeInc;
                
                if (pNumSpinBoxData->wData <=
                    pNumSpinBoxData->wRangeMin) 
                {
                    PaintArrow(hDC, 1, 1, rcLeft);
                    pNumSpinBoxData->bLeftDisabled = 1;
                    pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMin;
                    SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pNumSpinBoxData->wID, NSBN_RANGEMIN), (LPARAM)hWnd);
                }
                PaintText(hWnd, hDC, pNumSpinBoxData, rcText);

                if (pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax
                    && pNumSpinBoxData->bRightDisabled)
                {
                    PaintArrow(hDC, 0, 0, rcRight);
                    pNumSpinBoxData->bRightDisabled = 0;
                }
                SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                    MAKELONG(pNumSpinBoxData->wID, NSBN_CHANGE), (LPARAM)hWnd);
            }
            break;

        case VK_DOWN:
        case VK_RIGHT:
            if (pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax) 
            {
                pNumSpinBoxData->wData += pNumSpinBoxData->wRangeInc;

                if (pNumSpinBoxData->wData >= pNumSpinBoxData->wRangeMax) 
                {
                    PaintArrow(hDC, 0, 1, rcRight);
                    pNumSpinBoxData->bRightDisabled = 1;
                    pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMax;
                    SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pNumSpinBoxData->wID, NSBN_RANGEMAX), (LPARAM)hWnd);
                }
                PaintText(hWnd, hDC, pNumSpinBoxData, rcText);
                    
                if (pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin
                    && pNumSpinBoxData->bLeftDisabled)
                {
                    PaintArrow(hDC, 1, 0, rcLeft);
                    pNumSpinBoxData->bLeftDisabled = 0;
                }
                SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                    MAKELONG(pNumSpinBoxData->wID, NSBN_CHANGE), (LPARAM)hWnd);
            }
            break;
        }
        ReleaseDC(hWnd, hDC);
        break;
#endif
        case WM_PENDOWN :
            hDC = GetDC(hWnd);
            x = LOWORD(lParam);
            y = HIWORD(lParam);
            
            if (!PtInRectXY(&rcClient, x, y))
                break;
            SetCapture(hWnd);
            pNumSpinBoxData->bFocus = 1;

            if (PtInRectXY(&rcLeft, x, y))
            {
                if (pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin)
                {
                    pNumSpinBoxData->wData -= pNumSpinBoxData->wRangeInc;
                    
                    if (pNumSpinBoxData->wData <= pNumSpinBoxData->wRangeMin) 
                    {
                        PaintArrow(hDC, 1, 1, rcLeft);
                        pNumSpinBoxData->bLeftDisabled = 1;
                        pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMin;
                        SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                            MAKELONG(pNumSpinBoxData->wID, NSBN_RANGEMIN), (LPARAM)hWnd);
                    }
                    PaintText(hWnd, hDC, pNumSpinBoxData, rcText);
                    SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pNumSpinBoxData->wID, NSBN_CHANGE), (LPARAM)hWnd);
                    ReleaseDC(hWnd, hDC);
                    SetFocus(hWnd);
                }
                break;
            }
            else if (PtInRectXY(&rcRight, x, y))
            {
                if (pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax)
                {
                    pNumSpinBoxData->wData += pNumSpinBoxData->wRangeInc;
                    
                    if (pNumSpinBoxData->wData >= pNumSpinBoxData->wRangeMax) 
                    {
                        PaintArrow(hDC, 0, 1, rcRight);
                        pNumSpinBoxData->bRightDisabled = 1;
                        pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMax;
                        SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                            MAKELONG(pNumSpinBoxData->wID, NSBN_RANGEMAX), (LPARAM)hWnd);
                    }

                    PaintText(hWnd, hDC, pNumSpinBoxData, rcText);
                    SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pNumSpinBoxData->wID, NSBN_CHANGE), (LPARAM)hWnd);
                    ReleaseDC(hWnd, hDC);
                    SetFocus(hWnd);
                }
                break;
            }

            PaintText(hWnd, hDC, pNumSpinBoxData, rcText);
            ReleaseDC(hWnd, hDC);
            SetFocus(hWnd);
            break;

        case WM_PENUP:
            hDC = GetDC(hWnd);
            
            if (pNumSpinBoxData->bRightDisabled && 
                pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax)
            {
                PaintArrow(hDC, 0, 0, rcRight);
                pNumSpinBoxData->bRightDisabled = 0;
            }
            else if (pNumSpinBoxData->bLeftDisabled && 
                pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin)
            {
                PaintArrow(hDC, 1, 0, rcLeft);
                pNumSpinBoxData->bLeftDisabled = 0;
            }
            ReleaseCapture();
            ReleaseDC(hWnd, hDC);
            break;

        case NSBM_SETINITDATA:
            pNumSpinBoxData->wRangeMin = (IWORD)LOWORD(wParam);
            pNumSpinBoxData->wRangeMax = (IWORD)HIWORD(wParam);
            pNumSpinBoxData->wRangeInc = (IWORD)LOWORD(lParam);
            pNumSpinBoxData->wData = (IWORD)HIWORD(lParam);
            
            if (pNumSpinBoxData->wRangeMin < 0)
                pNumSpinBoxData->wRangeMin = 0;
            if (pNumSpinBoxData->wRangeMax < 0)
                pNumSpinBoxData->wRangeMax = 0;
            if (pNumSpinBoxData->wRangeInc < 0)
                pNumSpinBoxData->wRangeInc = 1;
            if (pNumSpinBoxData->wData < 0)
                pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMin;

            if (pNumSpinBoxData->wRangeMax < pNumSpinBoxData->wRangeMin)
                pNumSpinBoxData->wRangeMax = pNumSpinBoxData->wRangeMin;
            
            if (pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMin)
                pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMin;
            else if (pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMax)
                pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMax;

            if (pNumSpinBoxData->wData == pNumSpinBoxData->wRangeMin)
                pNumSpinBoxData->bLeftDisabled = TRUE;
            else
                pNumSpinBoxData->bLeftDisabled = FALSE;

            if (pNumSpinBoxData->wData == pNumSpinBoxData->wRangeMax)
                pNumSpinBoxData->bRightDisabled = TRUE;
            else
                pNumSpinBoxData->bRightDisabled = FALSE;
            
            if (IsWindowVisible(hWnd))
                InvalidateRect(hWnd, NULL, FALSE);       

           
            SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                MAKELONG(pNumSpinBoxData->wID, NSBN_CHANGE), (LPARAM)hWnd);

            break;

        case NSBM_GETDATA:
            lResult = pNumSpinBoxData->wData;
            break;

        default :
            return DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
    }

    return lResult;
}

static void PaintText(HWND hWnd, HDC hDC, PNUMSPINBOXDATA pNumSpinBoxData, const RECT rcText)
{
    static char Buffer[16];
    COLORREF    clrBKOld, clrTextOld;
    int         len;
    SIZE        size;
    HBRUSH      hBrush;

    hBrush = SetCtlColorNumSpin(pNumSpinBoxData, hWnd, hDC);
    GetTextExtentPoint32(NULL, "T", -1, &size);
    sprintf(Buffer, "%d", (int)pNumSpinBoxData->wData);
    
    if (pNumSpinBoxData->bFocus)
    {
        clrBKOld = SetBkColor(hDC, COLOR_BLUE);
        clrTextOld = SetTextColor(hDC, COLOR_WHITE);
    }
    len = strlen(Buffer);
    FillRect(hDC, &rcText, hBrush);
    
    if (pNumSpinBoxData->wState & NSBS_LEFT)
        DrawText(hDC, Buffer, len, (PRECT)&rcText, DT_LEFT | DT_VCENTER);
    else if (pNumSpinBoxData->wState & NSBS_RIGHT)
        DrawText(hDC, Buffer, len, (PRECT)&rcText, DT_RIGHT | DT_VCENTER);
    else
        DrawText(hDC, Buffer, len, (PRECT)&rcText, DT_CENTER | DT_VCENTER);

    if (pNumSpinBoxData->bFocus)
    {
        SetBkColor(hDC, clrBKOld);
        SetTextColor(hDC, clrTextOld);
    }
}

static void PaintArrow(HDC hDC, BOOL bLeft, BOOL bGrayed, const RECT rcArrow)
{
    if (bLeft)
    {
        if (!bGrayed)
           BitBlt(hDC, rcArrow.left, rcArrow.top, LRARROW_BMP_WIDTH,
                LRARROW_BMP_HEIGHT, (HDC)hBmpLeftArrow, 0, 0, ROP_SRC);
        else
        {
           BitBlt(hDC, rcArrow.left, rcArrow.top, LRARROW_BMP_WIDTH,
                LRARROW_BMP_HEIGHT, (HDC)hBmpLeftArrowDisabled, 0, 0, ROP_SRC);
        }
    }
    else
    {
        if (!bGrayed)
           BitBlt(hDC, rcArrow.left, rcArrow.top, LRARROW_BMP_WIDTH,
                LRARROW_BMP_HEIGHT, (HDC)hBmpRightArrow, 0, 0, ROP_SRC);
        else
           BitBlt(hDC, rcArrow.left, rcArrow.top, LRARROW_BMP_WIDTH,
                LRARROW_BMP_HEIGHT, (HDC)hBmpRightArrowDisabled, 0, 0, ROP_SRC);

    }
}

static HBRUSH SetCtlColorNumSpin(PNUMSPINBOXDATA pNumSpinBoxData, HWND hWnd, HDC hDC)
{
    HBRUSH  hBrush;

#if (!NOSENDCTLCOLOR)
    if (!pNumSpinBoxData->hParentWnd)
    {
#endif
        SetBkColor(hDC, GetSysColor(COLOR_WINDOW));
        SetTextColor(hDC, GetSysColor(COLOR_WINDOWTEXT));
        hBrush = (HBRUSH)(COLOR_WINDOW + 1);
#if (!NOSENDCTLCOLOR)
    }
    else
    {
        hBrush = (HBRUSH)SendMessage(pNumSpinBoxData->hParentWnd, (WORD)WM_CTLCOLOREDIT, 
            (WPARAM)hDC, (LPARAM)hWnd);
    }
#endif

    return hBrush;
}

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

#define Range_Max 10
#define Range_Min 1
#define Range_Inc 1
#define SPACE     1
#define LARROW_WIDTH 8
#define LARROW_HEIGHT 14

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
    RECT     rcLeft;
    RECT     rcRight;
    RECT     rcText;
}NUMSPINBOXDATA, *PNUMSPINBOXDATA;

static LRESULT CALLBACK NumSpinBox_Func(HWND hWnd, UINT wMsgCmd, 
                                 WPARAM wParam, LPARAM lParam);
static void PaintText(HWND hWnd, HDC hDC, PNUMSPINBOXDATA pNumSpinBoxData, const RECT Rect);
static void PaintArrow(HDC hDC, BOOL bLeft, BOOL bGrayed, const RECT Rect);
static HBRUSH SetCtlColorNumSpin(PNUMSPINBOXDATA pNumSpinBoxData, HWND hWnd, HDC hdc);

static HBITMAP hBmpLeftArrow, hBmpRightArrow,
                hBmpLeftArrowDisabled, hBmpRightArrowDisabled; 

BOOL NUMSPINBOX_RegisterClass(void)
{
    WNDCLASS    wcl;
    int  nPathLen;
    char PathName[MAX_PATH];

    wcl.style           = CS_OWNDC | CS_DEFWIN;
    wcl.lpfnWndProc     = NumSpinBox_Func;
    wcl.cbClsExtra      = NULL;
    wcl.cbWndExtra      = sizeof(NUMSPINBOXDATA);
    wcl.hbrBackground   = NULL;
    wcl.hCursor         = LoadCursor(NULL, IDC_ARROW);;
    wcl.hIcon           = NULL;
    wcl.lpszMenuName    = NULL;
    wcl.hInstance       = NULL;
    wcl.lpszClassName   = "NumSpinBox";

    if (!RegisterClass(&wcl))
        return FALSE;

    PLXOS_GetResourceDirectory(PathName);
    nPathLen = strlen(PathName);

    strcat(PathName, "numspinleft.bmp");
    hBmpLeftArrow = LoadImage(NULL, PathName, IMAGE_BITMAP, LARROW_WIDTH,
        LARROW_HEIGHT, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "numspinright.bmp");
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

#if (INTERFACE_MONO)
    PathName[nPathLen] = '\0';
    strcat(PathName, "disnumspinleft.bmp");
    hBmpLeftArrowDisabled = LoadImage(NULL, PathName, IMAGE_BITMAP, LARROW_WIDTH,
        LARROW_HEIGHT, LR_LOADFROMFILE);

    PathName[nPathLen] = '\0';
    strcat(PathName, "disnumspinright.bmp");
    hBmpRightArrowDisabled = LoadImage(NULL, PathName, IMAGE_BITMAP, LARROW_WIDTH, 
        LARROW_HEIGHT, LR_LOADFROMFILE);
    /*hBmpLeftArrowDisabled = CreateBitmap(LRARROW_BMP_WIDTH, 
        LRARROW_BMP_HEIGHT, 1, 1, LeftArrowDisabledBmpData);
    hBmpRightArrowDisabled = CreateBitmap(LRARROW_BMP_WIDTH, 
        LRARROW_BMP_HEIGHT, 1, 1, RightArrowDisabledBmpData);
    hBmpLeftArrowDisabled = LoadImage(NULL, "ROM:disleft.bmp",IMAGE_BITMAP, LARROW_WIDTH,
        LARROW_HEIGHT, LR_LOADFROMFILE);
    hBmpRightArrowDisabled = LoadImage(NULL, "ROM:disright.bmp",IMAGE_BITMAP, LARROW_WIDTH, 
        LARROW_HEIGHT, LR_LOADFROMFILE);*/
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
    static RECT     rcClient;
    HDC             hDC;
    HBRUSH          hBrush;
    SIZE            size;
    int             x, y, nSpaceVert, nHeightMin, nWidthMin;
    POINT           pt;
    HFONT           hFont;
    BOOL            fRedraw;

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
        nHeightMin = max(size.cy, LARROW_HEIGHT);
        nWidthMin =  2 * LARROW_WIDTH + 4 * SPACE + size.cx;

        nSpaceVert = (rcClient.bottom - rcClient.top - LARROW_HEIGHT) / 2;       
        SetRect(&pNumSpinBoxData->rcLeft, rcClient.left + SPACE, rcClient.top + nSpaceVert,
            rcClient.left + LARROW_WIDTH + SPACE, rcClient.bottom);
        SetRect(&pNumSpinBoxData->rcRight, rcClient.right - LARROW_WIDTH - SPACE, 
            rcClient.top + nSpaceVert, rcClient.right, rcClient.bottom);
        SetRect(&pNumSpinBoxData->rcText, pNumSpinBoxData->rcLeft.right + SPACE, rcClient.top,
            pNumSpinBoxData->rcRight.left - SPACE, rcClient.bottom);

        InvalidateRect(hWnd, &rcClient, TRUE);
        ReleaseDC(hWnd, hDC);
        break;

    case WM_PAINT :
        hDC = BeginPaint(hWnd, &ps);        
        PaintText(hWnd, hDC, pNumSpinBoxData, pNumSpinBoxData->rcText);
        
        if (pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin)
            PaintArrow(hDC, 1, 0, pNumSpinBoxData->rcLeft);
        else
            PaintArrow(hDC, 1, 1, pNumSpinBoxData->rcLeft);
                 
        if (pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax)
            PaintArrow(hDC, 0, 0, pNumSpinBoxData->rcRight);
        else
            PaintArrow(hDC, 0, 1, pNumSpinBoxData->rcRight);
        EndPaint(hWnd, &ps);

        break;

    case WM_ERASEBKGND:
        hDC = (HDC)wParam;
        hBrush = SetCtlColorNumSpin(pNumSpinBoxData, hWnd, hDC);
        GetClientRect(hWnd, &rcClient);
        FillRect(hDC, &rcClient, hBrush);
        lResult = (LRESULT)TRUE;
        break;

    case WM_SETFOCUS:
        DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
        SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
            SCROLLUP | SCROLLDOWN, MASKUP | MASKDOWN | MASKMIDDLE);
        if (pNumSpinBoxData->wData >= pNumSpinBoxData->wRangeMax)
            SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT, MASKLEFT | MASKRIGHT);
        else if (pNumSpinBoxData->wData <= pNumSpinBoxData->wRangeMin)
            SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLRIGHT, MASKLEFT | MASKRIGHT);
        else
            SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE, 
                SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
        hDC = GetDC(hWnd);
        pNumSpinBoxData->bFocus = 1;
        PaintText(hWnd, hDC, pNumSpinBoxData, pNumSpinBoxData->rcText);
        ReleaseDC(hWnd, hDC);
        
        SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
            MAKELONG(pNumSpinBoxData->wID, NSBN_SETFOCUS), (LPARAM)hWnd);
        break;

    case WM_KILLFOCUS :
        DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
        pNumSpinBoxData->bFocus = 0;
        hDC = GetDC(hWnd);
        PaintText(hWnd, hDC, pNumSpinBoxData, pNumSpinBoxData->rcText);
        ReleaseDC(hWnd, hDC);
        SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
            MAKELONG(pNumSpinBoxData->wID, NSBN_KILLFOCUS), (LPARAM)hWnd);
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

    case WM_GETDLGCODE:
        lResult = (LRESULT)DLGC_WANTARROWS | DLGC_WANTALLKEYS;
        break;

    case WM_CANCELMODE :
		GetCursorPos(&pt);
		ScreenToClient(hWnd, &pt);
		SendMessage(hWnd, WM_PENUP, 0, MAKELPARAM(pt.x, pt.y));
		break;

#if (!NOKEYBOARD)
//#ifndef SCP_NOKEYBOARD
    case WM_KEYDOWN :
        hDC = GetDC(hWnd);
        switch(wParam)
        {
        case VK_UP:
            if (pNumSpinBoxData->wState & CS_NOSYSCTRL)
                SendMessage(pNumSpinBoxData->hParentWnd, uMsgCmd, wParam, lParam);
            else if (pNumSpinBoxData->hParentWnd)
            {
                SetFocus(GetNextDlgTabItem(pNumSpinBoxData->hParentWnd, hWnd, TRUE));
                ReleaseDC(hWnd, hDC);
                return 0;
            }
            break;
        case VK_LEFT:
            if (pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin)
            {
                pNumSpinBoxData->wData -= pNumSpinBoxData->wRangeInc;
                
                if (pNumSpinBoxData->wData <=
                    pNumSpinBoxData->wRangeMin) 
                    
                {
                    PaintArrow(hDC, 1, 1, pNumSpinBoxData->rcLeft);
                    pNumSpinBoxData->bLeftDisabled = 1;
                    pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMin;
                    SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pNumSpinBoxData->wID, NSBN_RANGEMIN), (LPARAM)hWnd);
                    SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                        SCROLLRIGHT, MASKLEFT | MASKRIGHT);
                }
                else  if (pNumSpinBoxData->wData == 
                    (pNumSpinBoxData->wRangeMax - pNumSpinBoxData->wRangeInc))
                    SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                        SCROLLRIGHT | SCROLLLEFT, MASKLEFT | MASKRIGHT);
                PaintText(hWnd, hDC, pNumSpinBoxData, pNumSpinBoxData->rcText);

                if (pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax
                    && pNumSpinBoxData->bRightDisabled)
                {
                    PaintArrow(hDC, 0, 0, pNumSpinBoxData->rcRight);
                    pNumSpinBoxData->bRightDisabled = 0;
                }
                SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                    MAKELONG(pNumSpinBoxData->wID, NSBN_CHANGE), (LPARAM)hWnd);
            }
            break;

        case VK_DOWN:
            if (pNumSpinBoxData->wState & CS_NOSYSCTRL)
                SendMessage(pNumSpinBoxData->hParentWnd, uMsgCmd, wParam, lParam);
            else if (pNumSpinBoxData->hParentWnd)
            {
                SetFocus(GetNextDlgTabItem(pNumSpinBoxData->hParentWnd, hWnd, FALSE));
                ReleaseDC(hWnd, hDC);
                return 0;
            }
            break;
        case VK_RIGHT:
            if (pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax) 
            {
                pNumSpinBoxData->wData += pNumSpinBoxData->wRangeInc;

                if (pNumSpinBoxData->wData >= pNumSpinBoxData->wRangeMax) 
                {
                    PaintArrow(hDC, 0, 1, pNumSpinBoxData->rcRight);
                    pNumSpinBoxData->bRightDisabled = 1;
                    pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMax;
                    SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pNumSpinBoxData->wID, NSBN_RANGEMAX), (LPARAM)hWnd);
                    SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                        SCROLLLEFT, MASKLEFT | MASKRIGHT);
                }
                else if (pNumSpinBoxData->wData == 
                    (pNumSpinBoxData->wRangeInc + pNumSpinBoxData->wRangeMin))
                    SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                        SCROLLLEFT | SCROLLRIGHT, MASKLEFT | MASKRIGHT);
                PaintText(hWnd, hDC, pNumSpinBoxData, pNumSpinBoxData->rcText);
                    
                if (pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin
                    && pNumSpinBoxData->bLeftDisabled)
                {
                    PaintArrow(hDC, 1, 0, pNumSpinBoxData->rcLeft);
                    pNumSpinBoxData->bLeftDisabled = 0;
                }
                SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                    MAKELONG(pNumSpinBoxData->wID, NSBN_CHANGE), (LPARAM)hWnd);
            }
            break;

        case VK_F10://ÓÒsoftkeyÍË³ö
		case VK_RETURN:
            SendMessage(pNumSpinBoxData->hParentWnd, WM_KEYDOWN, wParam, lParam);
            ReleaseDC(hWnd, hDC);
			return 0;
        }
        ReleaseDC(hWnd, hDC);
        break;
//#endif //SCP_NOKEYBOARD
#endif // NOKEYBOARD
        case WM_PENDOWN :

//#ifndef _EMULATE_
//			if (IsWindowEnabled(hWnd))
//				
//#endif //_EMULATE_

            
            if (GetFocus() != hWnd)
            {
                SetFocus(hWnd);
                break;
            }
            x = LOWORD(lParam);
            y = HIWORD(lParam);
            
            GetClientRect(hWnd, &rcClient);
            if (!PtInRectXY(&rcClient, x, y))
                break;
            hDC = GetDC(hWnd);
            SetCapture(hWnd);
            pNumSpinBoxData->bFocus = 1;

            if (PtInRectXY(&pNumSpinBoxData->rcLeft, x, y))
            {
                if (pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin)
                {
                    pNumSpinBoxData->wData -= pNumSpinBoxData->wRangeInc;
                    
                    if (pNumSpinBoxData->wData <= pNumSpinBoxData->wRangeMin) 
                    {
                        PaintArrow(hDC, 1, 1, pNumSpinBoxData->rcLeft);
                        pNumSpinBoxData->bLeftDisabled = 1;
                        pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMin;
                        SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                            MAKELONG(pNumSpinBoxData->wID, NSBN_RANGEMIN), (LPARAM)hWnd);
                        SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                            SCROLLRIGHT, MASKLEFT | MASKRIGHT);
                    }
                    else if(pNumSpinBoxData->wData == 
                        (pNumSpinBoxData->wRangeMax - pNumSpinBoxData->wRangeInc))
                        SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                            SCROLLRIGHT | SCROLLLEFT, MASKLEFT | MASKRIGHT);
                    PaintText(hWnd, hDC, pNumSpinBoxData, pNumSpinBoxData->rcText);
                    SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pNumSpinBoxData->wID, NSBN_CHANGE), (LPARAM)hWnd);
                    ReleaseDC(hWnd, hDC);
                    SetFocus(hWnd);
                }
                break;
            }
            else if (PtInRectXY(&pNumSpinBoxData->rcRight, x, y))
            {
                if (pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax)
                {
                    pNumSpinBoxData->wData += pNumSpinBoxData->wRangeInc;
                    
                    if (pNumSpinBoxData->wData >= pNumSpinBoxData->wRangeMax) 
                    {
                        PaintArrow(hDC, 0, 1, pNumSpinBoxData->rcRight);
                        pNumSpinBoxData->bRightDisabled = 1;
                        pNumSpinBoxData->wData = pNumSpinBoxData->wRangeMax;
                        SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                            MAKELONG(pNumSpinBoxData->wID, NSBN_RANGEMAX), (LPARAM)hWnd);
                        SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                            SCROLLLEFT, MASKLEFT | MASKRIGHT);
                    }
                    else if(pNumSpinBoxData->wData == 
                        (pNumSpinBoxData->wRangeMin + pNumSpinBoxData->wRangeInc))
                        SendMessage(pNumSpinBoxData->hParentWnd, PWM_SETSCROLLSTATE,
                            SCROLLRIGHT | SCROLLLEFT, MASKLEFT | MASKRIGHT);

                    PaintText(hWnd, hDC, pNumSpinBoxData, pNumSpinBoxData->rcText);
                    SendMessage(pNumSpinBoxData->hParentWnd, WM_COMMAND, 
                        MAKELONG(pNumSpinBoxData->wID, NSBN_CHANGE), (LPARAM)hWnd);
                    ReleaseDC(hWnd, hDC);
                    SetFocus(hWnd);
                }
                break;
            }

            PaintText(hWnd, hDC, pNumSpinBoxData, pNumSpinBoxData->rcText);
            ReleaseDC(hWnd, hDC);
            SetFocus(hWnd);
            break;

        case WM_PENUP:
            hDC = GetDC(hWnd);
            
            if (pNumSpinBoxData->bRightDisabled && 
                pNumSpinBoxData->wData < pNumSpinBoxData->wRangeMax)
            {
                PaintArrow(hDC, 0, 0, pNumSpinBoxData->rcRight);
                pNumSpinBoxData->bRightDisabled = 0;
            }
            else if (pNumSpinBoxData->bLeftDisabled && 
                pNumSpinBoxData->wData > pNumSpinBoxData->wRangeMin)
            {
                PaintArrow(hDC, 1, 0, pNumSpinBoxData->rcLeft);
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
    COLORREF    clrBKOld, clrTextOld = RGB(0, 0, 0) ;
    int         len;
    SIZE        size;
    RECT        rc;

    GetTextExtentPoint32(hDC, "T", -1, &size);
    sprintf(Buffer, "%d", (int)pNumSpinBoxData->wData);
    
    if (pNumSpinBoxData->bFocus)
    {
        clrBKOld = SetBkColor(hDC, GetWindowColor(FOCUS_COLOR));
        clrTextOld = SetTextColor(hDC, COLOR_WHITE);
        CopyRect(&rc, &rcText);
        rc.top ++;
        rc.bottom --;
        ClearRect(hDC, &rc, GetWindowColor(FOCUS_COLOR));
    }
    else
    {
        clrBKOld = SetBkColor(hDC, COLOR_WHITE);
        CopyRect(&rc, &rcText);
        rc.top ++;
        rc.bottom --;
        ClearRect(hDC, &rc, COLOR_WHITE);
    }
        //clrBKOld = SetBkColor(hDC, GetWindowColor(CAP_COLOR));
    len = strlen(Buffer);
    
    
    if (pNumSpinBoxData->wState & NSBS_LEFT)
        DrawText(hDC, Buffer, len, (PRECT)&rc, DT_LEFT | DT_VCENTER);
    else if (pNumSpinBoxData->wState & NSBS_RIGHT)
        DrawText(hDC, Buffer, len, (PRECT)&rc, DT_RIGHT | DT_VCENTER);
    else
        DrawText(hDC, Buffer, len, (PRECT)&rc, DT_CENTER | DT_VCENTER);

    if (pNumSpinBoxData->bFocus)
        SetTextColor(hDC, clrTextOld);
    SetBkColor(hDC, clrBKOld);
}

static void PaintArrow(HDC hDC, BOOL bLeft, BOOL bGrayed, const RECT rcArrow)
{
    if (bLeft)
    {
        if (!bGrayed)
           BitBlt(hDC, rcArrow.left, rcArrow.top, LARROW_WIDTH,
                LARROW_HEIGHT, (HDC)hBmpLeftArrow, 0, 0, ROP_SRC);
        else
        {
           BitBlt(hDC, rcArrow.left, rcArrow.top, LARROW_WIDTH,
                LARROW_HEIGHT, (HDC)hBmpLeftArrowDisabled, 0, 0, ROP_SRC);
        }
    }
    else
    {
        if (!bGrayed)
           BitBlt(hDC, rcArrow.left, rcArrow.top, LARROW_WIDTH,
                LARROW_HEIGHT, (HDC)hBmpRightArrow, 0, 0, ROP_SRC);
        else
           BitBlt(hDC, rcArrow.left, rcArrow.top, LARROW_WIDTH,
                LARROW_HEIGHT, (HDC)hBmpRightArrowDisabled, 0, 0, ROP_SRC);

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

/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleMonthView.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"

static LRESULT CALLBACK CaleMonthWndProc(HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam);
static void CALE_DrawDate(HWND hWnd, HDC hDC, PCaleMonth pMonth, PRECT pRect);
static void CALE_DrawLine(HDC hDC, PCaleMonth pMonth, PRECT pRect);
static void CALE_DrawSymbol(HDC hDC, CaleMonthSch *pMonthSch, PRECT pTmpRc);
static void CALE_DrawWeek(HDC hDC, PCaleMonth pMonth, PRECT pRect);
static void CALE_PaintMonth(HWND hWnd, HDC hDC, PCaleMonth pCaleMonth, PRECT pRect);

extern int CALE_GetWeekStart();
extern int CALE_AlmDaysOfMonth(SYSTEMTIME *pDate);
extern BOOL CALE_SetScheduleFlag(CaleMonthSch *pSched);
extern BOOL CALE_DayReduce(SYSTEMTIME *psystime, unsigned int day);

BOOL CALE_RegisterMonthClass(void* hInst)
{
    WNDCLASS wc;
    static HINSTANCE hInstance;

    hInstance = (HINSTANCE)hInst;
    
    wc.style        = CS_OWNDC;
    wc.lpfnWndProc  = CaleMonthWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CaleMonth);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CALEMONTHCLASS";

    if(!RegisterClass(&wc))
        return FALSE;

    return TRUE;
}

BOOL CALE_UnRegisterMonthClass()
{
    UnregisterClass("CALEMONTHCLASS", NULL);
    return TRUE;
}

static LRESULT CALLBACK CaleMonthWndProc(HWND hWnd, UINT message,
                                WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    HDC hDC, hMemDC;
    PCREATESTRUCT pCreateStruct;
    RECT rcTemp, rcTemp1; 
    RECT rcClient;

    PAINTSTRUCT ps;
    HBITMAP hBmp, hBmpOld;
    HBRUSH hBrush;
    static HFONT hFontLit;
    static HFONT hFontOld;

    int tmpRow, tmpLine;
    int nDays;
    
    PCaleMonth pCaleMonth;

    hFontLit = NULL;
    hFontOld = NULL;
    lResult = (LRESULT)TRUE;
    pCaleMonth = GetUserData(hWnd);

    switch(message)
    {
    case WM_CREATE:
        {
            hDC = GetDC(hWnd);
            GetFontHandle(&hFontLit, 0);
            hFontOld = SelectObject(hDC, hFontLit);
            ReleaseDC(hWnd, hDC);
            
            memset(pCaleMonth, 0x0, sizeof(CaleMonth));
            pCreateStruct = (LPCREATESTRUCT)lParam;

            pCaleMonth->dwStyle = pCreateStruct->style;
            pCaleMonth->hwndParent = pCreateStruct->hwndParent;
            pCaleMonth->wID = (WORD)(DWORD)pCreateStruct->hMenu;
            pCaleMonth->x = pCreateStruct->x;
            pCaleMonth->y = pCreateStruct->y;
            pCaleMonth->width = pCreateStruct->cx;
            pCaleMonth->height = pCreateStruct->cy;

            pCaleMonth->MaxYear = MAX_YEAR;
            pCaleMonth->MinYear = MIN_YEAR;

            GetLocalTime(&pCaleMonth->sysCurTime);
            UpdateWindow(hWnd);
        }
    	break;

    case WM_SIZE:
        {
        }
    	break;

    case WM_PAINT:
        {
            pCaleMonth->nWeekStart = CALE_GetWeekStart();
            hDC = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rcClient);
            CopyRect(&rcTemp, &rcClient);
            hMemDC = CreateCompatibleDC(hDC);
            hBmp = CreateCompatibleBitmap(hDC, rcClient.right, rcClient.bottom);
            hBmpOld = (HBITMAP)SelectObject(hMemDC, (HGDIOBJ)hBmp);
            hBrush = CreateBrush(BS_SOLID, CALE_WHITE, 0);
            FillRect(hMemDC, &rcClient, (HBRUSH)hBrush);

            CALE_PaintMonth(hWnd, hMemDC, pCaleMonth, &rcClient);
            
            BitBlt(hDC, 0, 0, rcTemp.right, rcTemp.bottom, hMemDC, 0, 0, SRCCOPY);
            SelectObject(hMemDC, hBmpOld);
            DeleteObject(hBrush);
            DeleteObject(hBmp);
            DeleteObject(hMemDC);

            EndPaint(hWnd, &ps);
        }
        break;

    case WM_DESTROY:
        {
            hDC = GetDC(hWnd);
            SelectObject(hDC, hFontOld);
            ReleaseDC(hWnd, hDC);
        }
        break;

    case WM_KEYDOWN:
        {
            switch(LOWORD(wParam)) 
            {
            case VK_LEFT:
                {
                    if(pCaleMonth->sysCurTime.wDay == 1)
                    {
                        if((pCaleMonth->CurSch != 0) && (pCaleMonth->MonthSch[pCaleMonth->CurSch - 1].wYear < MIN_YEAR))
                        {
                            break;
                        }
                        else
                        {
                            if(pCaleMonth->CurSch != 0)
                            {
                                pCaleMonth->sysCurTime.wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch - 1].wYear;
                                pCaleMonth->sysCurTime.wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch - 1].wMonth;
                                pCaleMonth->sysCurTime.wDay = pCaleMonth->MonthSch[pCaleMonth->CurSch - 1].wDay;
                                SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                                    MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                                pCaleMonth->CurSch --;
                            }
                            else
                            {
                                CALE_DayReduce(&pCaleMonth->sysCurTime, 1);
                                SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                                    MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                            }
                            
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);

                            SendMessage(pCaleMonth->hwndParent, PWM_SHOWWINDOW, 0, 0); 
                        }
                    }
                    else if(pCaleMonth->sysCurTime.wDay > 1)
                    {
                        hDC = GetDC(hWnd);
                        GetClientRect(hWnd, &rcTemp);

                        tmpRow = pCaleMonth->CurSch / (MAX_LINE - 1);
                        tmpLine = pCaleMonth->CurSch % (MAX_LINE - 1);

                        rcTemp1.left = rcTemp.left + (CALE_MONTH_WEEK_WIDTH + 1) + 
                            (CALE_MONTH_DAY_WIDTH + 1) * tmpLine + 1;
                        rcTemp1.top = rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (tmpRow + 1) + 1;
                        rcTemp1.right = rcTemp1.left + CALE_MONTH_DAY_WIDTH;
                        rcTemp1.bottom = rcTemp1.top + CALE_MONTH_DAY_HEIGHT;
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].bFlag &= ~(CALE_SCHED_TODAY);
                        CALE_DrawSymbol(hDC, &(pCaleMonth->MonthSch[pCaleMonth->CurSch]), &rcTemp1);

                        pCaleMonth->sysCurTime.wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch - 1].wYear;
                        pCaleMonth->sysCurTime.wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch - 1].wMonth;
                        pCaleMonth->sysCurTime.wDay = pCaleMonth->MonthSch[pCaleMonth->CurSch - 1].wDay;
                        pCaleMonth->CurSch --;
                        
                        tmpRow = pCaleMonth->CurSch / (MAX_LINE - 1);
                        tmpLine = pCaleMonth->CurSch % (MAX_LINE - 1);

                        rcTemp1.left = rcTemp.left + (CALE_MONTH_WEEK_WIDTH + 1) + 
                            (CALE_MONTH_DAY_WIDTH + 1) * tmpLine + 1;
                        rcTemp1.top = rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (tmpRow + 1) + 1;
                        rcTemp1.right = rcTemp1.left + CALE_MONTH_DAY_WIDTH;
                        rcTemp1.bottom = rcTemp1.top + CALE_MONTH_DAY_HEIGHT;
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].bFlag |= CALE_SCHED_TODAY;
                        CALE_DrawSymbol(hDC, &(pCaleMonth->MonthSch[pCaleMonth->CurSch]), &rcTemp1);

                        ReleaseDC(hWnd, hDC);
                        SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                            MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                    }
                }
            	break;

            case VK_RIGHT:
                {
                    nDays = CALE_AlmDaysOfMonth(&pCaleMonth->sysCurTime);
                    if(pCaleMonth->sysCurTime.wDay == nDays)
                    {
                        if(pCaleMonth->MonthSch[pCaleMonth->CurSch + 1].wYear > MAX_YEAR)
                        {
                            break;
                        }
                        else
                        {
                            pCaleMonth->sysCurTime.wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch + 1].wYear;
                            pCaleMonth->sysCurTime.wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch + 1].wMonth;
                            pCaleMonth->sysCurTime.wDay = pCaleMonth->MonthSch[pCaleMonth->CurSch + 1].wDay;
                            SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                                MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                            pCaleMonth->CurSch ++;
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);

                            SendMessage(pCaleMonth->hwndParent, PWM_SHOWWINDOW, 0, 0);
                        }
                    }
                    else if(pCaleMonth->sysCurTime.wDay < nDays)
                    {
                        hDC = GetDC(hWnd);
                        GetClientRect(hWnd, &rcTemp);

                        tmpRow = pCaleMonth->CurSch / (MAX_LINE - 1);
                        tmpLine = pCaleMonth->CurSch % (MAX_LINE - 1);
                        
                        rcTemp1.left = rcTemp.left + (CALE_MONTH_WEEK_WIDTH + 1) + 
                            (CALE_MONTH_DAY_WIDTH + 1) * tmpLine + 1;
                        rcTemp1.top = rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (tmpRow + 1) + 1;
                        rcTemp1.right = rcTemp1.left + CALE_MONTH_DAY_WIDTH;
                        rcTemp1.bottom = rcTemp1.top + CALE_MONTH_DAY_HEIGHT;
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].bFlag &= ~(CALE_SCHED_TODAY);
                        CALE_DrawSymbol(hDC, &(pCaleMonth->MonthSch[pCaleMonth->CurSch]), &rcTemp1);
                        
                        pCaleMonth->sysCurTime.wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch + 1].wYear;
                        pCaleMonth->sysCurTime.wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch + 1].wMonth;
                        pCaleMonth->sysCurTime.wDay = pCaleMonth->MonthSch[pCaleMonth->CurSch + 1].wDay;
                        pCaleMonth->CurSch ++;

                        tmpRow = pCaleMonth->CurSch / (MAX_LINE - 1);
                        tmpLine = pCaleMonth->CurSch % (MAX_LINE - 1);

                        rcTemp1.left = rcTemp.left + (CALE_MONTH_WEEK_WIDTH + 1) + 
                            (CALE_MONTH_DAY_WIDTH + 1) * tmpLine + 1;
                        rcTemp1.top = rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (tmpRow + 1) + 1;
                        rcTemp1.right = rcTemp1.left + CALE_MONTH_DAY_WIDTH;
                        rcTemp1.bottom = rcTemp1.top + CALE_MONTH_DAY_HEIGHT;
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].bFlag |= CALE_SCHED_TODAY;
                        CALE_DrawSymbol(hDC, &(pCaleMonth->MonthSch[pCaleMonth->CurSch]), &rcTemp1);
                        
                        ReleaseDC(hWnd, hDC);
                        SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                            MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                    }
                }
            	break;

            case VK_UP:
                {
                    if((pCaleMonth->CurSch - 7) < 0)
                    {
                        if(pCaleMonth->sysCurTime.wMonth == 1)
                        {
                            if(pCaleMonth->sysCurTime.wYear <= MIN_YEAR)
                            {
                                break;
                            }
                            else
                            {
                                pCaleMonth->sysCurTime.wMonth = 12;
                                pCaleMonth->sysCurTime.wYear --;
                            }
                        }
                        else
                        {
                            pCaleMonth->sysCurTime.wMonth --;
                        }

                        nDays = CALE_AlmDaysOfMonth(&pCaleMonth->sysCurTime);
#ifdef _BENEFON_CALE_
                        //benefon style
                        pCaleMonth->sysCurTime.wDay = nDays;
#endif
#ifdef _NOKIA_CALE_
                        //nokia style
                        pCaleMonth->sysCurTime.wDay = pCaleMonth->sysCurTime.wDay + nDays - 7;
#endif
                        SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                            MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                        InvalidateRect(hWnd, NULL, TRUE);
                        UpdateWindow(hWnd);

                        SendMessage(pCaleMonth->hwndParent, PWM_SHOWWINDOW, 0, 0);
                    }
                    else if(pCaleMonth->MonthSch[pCaleMonth->CurSch - 7].wMonth != 
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].wMonth)
                    {
                        if(pCaleMonth->MonthSch[pCaleMonth->CurSch - 7].wYear < MIN_YEAR &&
							pCaleMonth->sysCurTime.wMonth == 1)
                        {
                            break;
                        }
                        else
                        {
                            pCaleMonth->sysCurTime.wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch - 7].wYear;
                            pCaleMonth->sysCurTime.wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch - 7].wMonth;
#ifdef _BENEFON_CALE_
                            //benefon style
                            nDays = CALE_AlmDaysOfMonth(&pCaleMonth->sysCurTime);
                            pCaleMonth->sysCurTime.wDay = nDays;
#endif
#ifdef _NOKIA_CALE_
                            //nokia style
                            pCaleMonth->sysCurTime.wDay = pCaleMonth->MonthSch[pCaleMonth->CurSch - 7].wDay;
                            pCaleMonth->CurSch -= 7;
#endif
                            SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                                MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);

                            SendMessage(pCaleMonth->hwndParent, PWM_SHOWWINDOW, 0, 0);
                        }
                    }
                    else
                    {
                        hDC = GetDC(hWnd);
                        GetClientRect(hWnd, &rcTemp);
                        
                        tmpRow = pCaleMonth->CurSch / (MAX_LINE - 1);
                        tmpLine = pCaleMonth->CurSch % (MAX_LINE - 1);
                        
                        rcTemp1.left = rcTemp.left + (CALE_MONTH_WEEK_WIDTH + 1) + 
                            (CALE_MONTH_DAY_WIDTH + 1) * tmpLine + 1;
                        rcTemp1.top = rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (tmpRow + 1) + 1;
                        rcTemp1.right = rcTemp1.left + CALE_MONTH_DAY_WIDTH;
                        rcTemp1.bottom = rcTemp1.top + CALE_MONTH_DAY_HEIGHT;
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].bFlag &= ~(CALE_SCHED_TODAY);
                        CALE_DrawSymbol(hDC, &(pCaleMonth->MonthSch[pCaleMonth->CurSch]), &rcTemp1);
                        
                        pCaleMonth->sysCurTime.wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch - 7].wYear;
                        pCaleMonth->sysCurTime.wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch - 7].wMonth;
                        pCaleMonth->sysCurTime.wDay = pCaleMonth->MonthSch[pCaleMonth->CurSch - 7].wDay;
                        pCaleMonth->CurSch -= 7;
                        
                        tmpRow = pCaleMonth->CurSch / (MAX_LINE - 1);
                        tmpLine = pCaleMonth->CurSch % (MAX_LINE - 1);
                        
                        rcTemp1.left = rcTemp.left + (CALE_MONTH_WEEK_WIDTH + 1) + 
                            (CALE_MONTH_DAY_WIDTH + 1) * tmpLine + 1;
                        rcTemp1.top = rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (tmpRow + 1) + 1;
                        rcTemp1.right = rcTemp1.left + CALE_MONTH_DAY_WIDTH;
                        rcTemp1.bottom = rcTemp1.top + CALE_MONTH_DAY_HEIGHT;
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].bFlag |= CALE_SCHED_TODAY;
                        CALE_DrawSymbol(hDC, &(pCaleMonth->MonthSch[pCaleMonth->CurSch]), &rcTemp1);
                        
                        ReleaseDC(hWnd, hDC);
                        SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                            MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                    }
                }
                break;

            case VK_DOWN:
                {
                    if((pCaleMonth->CurSch + 7) >= MAX_SCH)
                    {
                        nDays = CALE_AlmDaysOfMonth(&pCaleMonth->sysCurTime);
#ifdef _NOKIA_CALE_
                        //nokia style
                        pCaleMonth->sysCurTime.wDay = pCaleMonth->sysCurTime.wDay - nDays + 7;
#endif
#ifdef _BENEFON_CALE_
                        //benefon style
                        pCaleMonth->sysCurTime.wDay = 1;
#endif
                        
                        if(pCaleMonth->sysCurTime.wMonth == 12)
                        {
                            if(pCaleMonth->sysCurTime.wYear >= MAX_YEAR)
                            {
                                break;
                            }
                            else
                            {
                                pCaleMonth->sysCurTime.wMonth = 1;
                                pCaleMonth->sysCurTime.wYear ++;
                            }
                        }
                        else
                        {
                            pCaleMonth->sysCurTime.wMonth ++;
                        }
                        
                        SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                            MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                        InvalidateRect(hWnd, NULL, TRUE);
                        UpdateWindow(hWnd);

                        SendMessage(pCaleMonth->hwndParent, PWM_SHOWWINDOW, 0, 0);
                    }
                    else if(pCaleMonth->MonthSch[pCaleMonth->CurSch + 7].wMonth != 
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].wMonth)
                    {
                        if(pCaleMonth->MonthSch[pCaleMonth->CurSch + 7].wYear > MAX_YEAR &&
							pCaleMonth->sysCurTime.wMonth == 12)
                        {
                            break;
                        }
                        else
                        {
                            pCaleMonth->sysCurTime.wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch + 7].wYear;
                            pCaleMonth->sysCurTime.wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch + 7].wMonth;
#ifdef _NOKIA_CALE_
                            //nokia style
                            pCaleMonth->sysCurTime.wDay = pCaleMonth->MonthSch[pCaleMonth->CurSch + 7].wDay;
                            pCaleMonth->CurSch += 7;
#endif
#ifdef _BENEFON_CALE_
                            //benefon sytle
                            pCaleMonth->sysCurTime.wDay = 1;
#endif
                            SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                                MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);

                            SendMessage(pCaleMonth->hwndParent, PWM_SHOWWINDOW, 0, 0);
                        }
                    }
                    else
                    {
                        hDC = GetDC(hWnd);
                        GetClientRect(hWnd, &rcTemp);
                        
                        tmpRow = pCaleMonth->CurSch / (MAX_LINE - 1);
                        tmpLine = pCaleMonth->CurSch % (MAX_LINE - 1);
                        
                        rcTemp1.left = rcTemp.left + (CALE_MONTH_WEEK_WIDTH + 1) + 
                            (CALE_MONTH_DAY_WIDTH + 1) * tmpLine + 1;
                        rcTemp1.top = rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (tmpRow + 1) + 1;
                        rcTemp1.right = rcTemp1.left + CALE_MONTH_DAY_WIDTH;
                        rcTemp1.bottom = rcTemp1.top + CALE_MONTH_DAY_HEIGHT;
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].bFlag &= ~(CALE_SCHED_TODAY);
                        CALE_DrawSymbol(hDC, &(pCaleMonth->MonthSch[pCaleMonth->CurSch]), &rcTemp1);
                        
                        pCaleMonth->sysCurTime.wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch + 7].wYear;
                        pCaleMonth->sysCurTime.wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch + 7].wMonth;
                        pCaleMonth->sysCurTime.wDay = pCaleMonth->MonthSch[pCaleMonth->CurSch + 7].wDay;
                        pCaleMonth->CurSch += 7;
                        
                        tmpRow = pCaleMonth->CurSch / (MAX_LINE - 1);
                        tmpLine = pCaleMonth->CurSch % (MAX_LINE - 1);
                        
                        rcTemp1.left = rcTemp.left + (CALE_MONTH_WEEK_WIDTH + 1) + 
                            (CALE_MONTH_DAY_WIDTH + 1) * tmpLine + 1;
                        rcTemp1.top = rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (tmpRow + 1) + 1;
                        rcTemp1.right = rcTemp1.left + CALE_MONTH_DAY_WIDTH;
                        rcTemp1.bottom = rcTemp1.top + CALE_MONTH_DAY_HEIGHT;
                        pCaleMonth->MonthSch[pCaleMonth->CurSch].bFlag |= CALE_SCHED_TODAY;
                        CALE_DrawSymbol(hDC, &(pCaleMonth->MonthSch[pCaleMonth->CurSch]), &rcTemp1);
                        
                        ReleaseDC(hWnd, hDC);
                        SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                            MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
                    }
                }
                break;
                
            case VK_F10:
            case VK_F5:
            case VK_RETURN:
                {
                    SendMessage(pCaleMonth->hwndParent, WM_KEYDOWN, wParam, lParam);
                }
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }//end switch
        }
        break;

    case CALE_CALC_SETCUR:
        {
            if((LPSYSTEMTIME)lParam != NULL)
            {
                if (((LPSYSTEMTIME)lParam)->wYear < MIN_YEAR || 
                    ((LPSYSTEMTIME)lParam)->wYear > MAX_YEAR)
                    return FALSE;
                
                if (((LPSYSTEMTIME)lParam)->wMonth < 1 || 
                    ((LPSYSTEMTIME)lParam)->wMonth > 12)
                    return FALSE;
                
                pCaleMonth->sysCurTime.wYear = ((LPSYSTEMTIME)lParam)->wYear;
                pCaleMonth->sysCurTime.wMonth = ((LPSYSTEMTIME)lParam)->wMonth;

                nDays = CALE_AlmDaysOfMonth(&pCaleMonth->sysCurTime);
                if (((LPSYSTEMTIME)lParam)->wDay < 1 || 
                    ((LPSYSTEMTIME)lParam)->wDay > nDays)
                {
                    pCaleMonth->sysCurTime.wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch].wYear;
                    pCaleMonth->sysCurTime.wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch].wMonth;
                    return FALSE;
                }
                pCaleMonth->sysCurTime.wDay = ((LPSYSTEMTIME)lParam)->wDay;
                pCaleMonth->sysCurTime.wDayOfWeek = ((LPSYSTEMTIME)lParam)->wDayOfWeek;

                InvalidateRect(hWnd, NULL, TRUE);
                //UpdateWindow(hWnd);
                SendMessage(pCaleMonth->hwndParent, WM_COMMAND, 
                    MAKEWPARAM(pCaleMonth->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleMonth->sysCurTime));
            }//end if((LPSYSTEMTIME)lParam != NULL)
        }
        break;
            
    case CALE_CALC_GETCUR:
        {
            ((LPSYSTEMTIME)lParam)->wYear = pCaleMonth->MonthSch[pCaleMonth->CurSch].wYear;
            ((LPSYSTEMTIME)lParam)->wMonth = pCaleMonth->MonthSch[pCaleMonth->CurSch].wMonth;
            ((LPSYSTEMTIME)lParam)->wDay = pCaleMonth->MonthSch[pCaleMonth->CurSch].wDay;
            ((LPSYSTEMTIME)lParam)->wDayOfWeek = pCaleMonth->MonthSch[pCaleMonth->CurSch].wWeek;
        }
        break;

    case CALE_APP_NEWC:
        {
            switch(LOWORD(wParam))
            {
            case CALE_MEETING:
            case CALE_ANNI:
            case CALE_EVENT:
                {
                    InvalidateRect(hWnd, NULL, TRUE);
                    UpdateWindow(hWnd);
                }
                break;
                
            default:
                lResult = DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
        
    default:
        lResult = DefWindowProc(hWnd, message, wParam, lParam);
    }
    return lResult;
}

static void CALE_PaintMonth(HWND hWnd, HDC hDC, PCaleMonth pCaleMonth, PRECT pRect)
{
    COLORREF hOldBkClr;
    static HFONT hFontLit;
    static HFONT hFontOld;
    
    hFontLit = NULL;
    hFontOld = NULL;
    GetFontHandle(&hFontLit, 0);
    hFontOld = SelectObject(hDC, hFontLit);

    hOldBkClr = SetBkColor(hDC, CALE_WHITE);

    CALE_DrawLine(hDC, pCaleMonth, pRect);
    CALE_DrawWeek(hDC, pCaleMonth, pRect);
    CALE_DrawDate(hWnd, hDC, pCaleMonth, pRect);

    SetBkColor(hDC, hOldBkClr);
    SelectObject(hDC, hFontOld);
    return;
}

static void CALE_DrawLine(HDC hDC, PCaleMonth pMonth, PRECT pRect)
{
    HPEN hPen, hOldPen;
    RECT rcTemp;
    int i;

    CopyRect(&rcTemp, pRect);

    hPen = CreatePen(PS_SOLID, 1, (COLORREF)CALE_PURPLE);
    hOldPen = (HPEN)SelectObject(hDC, (HGDIOBJ)hPen);
    
    MoveTo(hDC, rcTemp.left, rcTemp.top, NULL);
    LineTo(hDC, rcTemp.left + (CALE_MONTH_DAY_WIDTH + 1) * (MAX_LINE - 1) + (CALE_MONTH_WEEK_WIDTH + 1), 
        rcTemp.top);
    MoveTo(hDC, rcTemp.left, rcTemp.top, NULL);
    LineTo(hDC, rcTemp.left, rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * MAX_ROW);
    MoveTo(hDC, rcTemp.left + CALE_MONTH_WEEK_WIDTH + 1, rcTemp.top + CALE_MONTH_DAY_HEIGHT + 1, NULL);
    LineTo(hDC, rcTemp.left + (CALE_MONTH_DAY_WIDTH + 1) * (MAX_LINE - 1) + (CALE_MONTH_WEEK_WIDTH + 1), 
        rcTemp.top + CALE_MONTH_DAY_HEIGHT + 1);

    for(i = 0; i< MAX_ROW - 1; i ++)
    {
        MoveTo(hDC, rcTemp.left, rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (i + 2), NULL);
        LineTo(hDC, rcTemp.left + (CALE_MONTH_DAY_WIDTH + 1) * (MAX_LINE - 1) + (CALE_MONTH_WEEK_WIDTH + 1), 
            rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * (i + 2));
    }
    for(i = 0; i < MAX_LINE + 1; i ++)
    {
        MoveTo(hDC, rcTemp.left + (CALE_MONTH_DAY_WIDTH + 1) * i + (CALE_MONTH_WEEK_WIDTH + 1), 
            rcTemp.top, NULL);
        LineTo(hDC, rcTemp.left + (CALE_MONTH_DAY_WIDTH + 1) * i + (CALE_MONTH_WEEK_WIDTH + 1), 
            rcTemp.top + (CALE_MONTH_DAY_HEIGHT + 1) * MAX_ROW);
    }

    SelectObject(hDC, (HGDIOBJ)hOldPen);
    DeleteObject((HGDIOBJ)hPen);

    return;
}

static void CALE_DrawWeek(HDC hDC, PCaleMonth pMonth, PRECT pRect)
{
    int i, j;
    RECT rcNum;
    COLORREF hOldText;

    CopyRect(&rcNum, pRect);
    rcNum.left = pRect->left + CALE_MONTH_WEEK_WIDTH + 2;
    rcNum.top = pRect->top + 1;
    rcNum.right = rcNum.left + CALE_MONTH_DAY_WIDTH;
    rcNum.bottom = rcNum.top + CALE_MONTH_DAY_HEIGHT;

    hOldText = SetTextColor(hDC, CALE_BLACK);

    for(i = 0; i < (MAX_LINE - 1); i ++)
    {        
        j = i + pMonth->nWeekStart;
        if(j > 6)
        {
            j -= 7;
        }
        if(j == 0)
        {
            DrawText(hDC, IDP_IME_SUNDAY, -1, &rcNum, DT_VCENTER | DT_CENTER);
        }
        else if(j == 1)
        {
            DrawText(hDC, IDP_IME_MONDAY, -1, &rcNum, DT_VCENTER | DT_CENTER);
        }
        else if(j == 2)
        {
            DrawText(hDC, IDP_IME_TUESDAY, -1, &rcNum, DT_VCENTER | DT_CENTER);
        }
        else if(j == 3)
        {
            DrawText(hDC, IDP_IME_WEDNESDAY, -1, &rcNum, DT_VCENTER | DT_CENTER);
        }
        else if(j == 4)
        {
            DrawText(hDC, IDP_IME_THUESDAY, -1, &rcNum, DT_VCENTER | DT_CENTER);
        }
        else if(j == 5)
        {
            DrawText(hDC, IDP_IME_FRIDAY, -1, &rcNum, DT_VCENTER | DT_CENTER);
        }
        else
        {
            DrawText(hDC, IDP_IME_SATURDAY, -1, &rcNum, DT_VCENTER | DT_CENTER);
        }
        rcNum.left += CALE_MONTH_DAY_WIDTH + 1;
        rcNum.right = rcNum.left + CALE_MONTH_DAY_WIDTH;
    }
    SetTextColor(hDC, hOldText);

    return;
}

static void CALE_DrawDate(HWND hWnd, HDC hDC, PCaleMonth pMonth, PRECT pRect)
{
    WORD wYear, wMonth, wDay; 
    WORD wYearSys, wMonthSys, wDaySys;
    int nDateWidth, nDateHeight;
    int nStart, nDays, nTempDays;
    int nYearStart, nWeekNo;
    BOOL bLunar = 0;
    int i, j;
    char buff[3];
    
    SYSTEMTIME sysTime;
    RECT rcNum;
    COLORREF hOldText;
    int nSch;

    memset(pMonth->MonthSch, 0x0, sizeof(CaleMonthSch) * MAX_SCH);

    wYear = pMonth->sysCurTime.wYear;
    wMonth = pMonth->sysCurTime.wMonth;
    wDay = pMonth->sysCurTime.wDay;

    GetLocalTime(&sysTime);
    wYearSys = sysTime.wYear;
    wMonthSys = sysTime.wMonth;
    wDaySys = sysTime.wDay;

    nDateWidth = pRect->right / 8;
    nDateHeight = pRect->bottom / MAX_ROW;

    if((wYear % 4 == 0) && (wYear % 100 != 0) || (wYear % 400 == 0))
    {
        bLunar = 1;
    }

    if(wYear >= 1980)
    {
        nStart = (wYear - 1980) * 365 + (wYear - 1980) / 4 + 2 + 1;
        if((wYear % 4 == 0) && (wYear % 100 != 0) || (wYear % 400 == 0))
        {
            nStart -= 1;
        }
    }
    else
    {
        nStart = (1980 - wYear) * 365 + (1980 - wYear) / 4 + 2 + 1;
        if((wYear % 4 == 0) && (wYear % 100 != 0) || (wYear % 400 == 0))
        {
            nStart -= 1;
        }
    }
    nDays = Days[wMonth - 1];
    nYearStart = (bLunar ? nStart + 1 : nStart) % 7;

    if(wMonth != 1)
    {
        nStart += Days[wMonth - 2];
        nDays = Days[wMonth - 1] - Days[wMonth - 2];
        if(wMonth == 2 && bLunar)
            nDays ++;
    }
    if(bLunar && ((pMonth->sysCurTime.wYear <= 1998) ||
        ((pMonth->sysCurTime.wYear > 1998) && (pMonth->sysCurTime.wMonth > 2))))
    {
        nStart ++;
    }
    nStart %= 7;

    //calculate the schedule
    j = nStart - pMonth->nWeekStart;
    if(j < 0)
    {
        j += 7;
    }
    //pre current month
    if(wMonth > 2)
    {
        nTempDays = Days[wMonth - 2] - Days[wMonth - 3];
        if(wMonth == 3 && bLunar)
            nTempDays ++;
    }
    else
    {
        nTempDays = 31;
    }
    for(i = 0; i < j; i++)
    {
        if(wMonth != 1)
        {
            pMonth->MonthSch[i].wYear = wYear;
            pMonth->MonthSch[i].wMonth = wMonth - 1;
            pMonth->MonthSch[i].wDay = nTempDays - (j - i - 1);
            pMonth->MonthSch[i].wWeek = (pMonth->nWeekStart + i) % 7;
            pMonth->MonthSch[i].bFlag = 0;
        }
        else
        {
            pMonth->MonthSch[i].wYear = wYear - 1;
            pMonth->MonthSch[i].wMonth = 12;
            pMonth->MonthSch[i].wDay = nTempDays - (j - i - 1);
            pMonth->MonthSch[i].wWeek = (pMonth->nWeekStart + i) % 7;
            pMonth->MonthSch[i].bFlag = 0;
        }
    }
    //current month
    if(wMonth > 1)
    {
        nTempDays = Days[wMonth - 1] - Days[wMonth - 2];
        if(wMonth == 2 && bLunar)
            nTempDays ++;
    }
    else
    {
        nTempDays = 31;
    }
    for(i = 0; i < nTempDays; i ++)
    {
        pMonth->MonthSch[i+j].wYear = wYear;
        pMonth->MonthSch[i+j].wMonth = wMonth;
        pMonth->MonthSch[i+j].wDay = i + 1;
        pMonth->MonthSch[i+j].wWeek = (pMonth->nWeekStart + i + j) % 7;
        pMonth->MonthSch[i+j].bFlag = 0;
    }
    //after current month
    for(i = j + nTempDays; i < MAX_SCH; i ++)
    {
        if(wMonth == 12)
        {
            pMonth->MonthSch[i].wYear = wYear + 1;
            pMonth->MonthSch[i].wMonth = 1;
            pMonth->MonthSch[i].wDay = i - j - nTempDays + 1;
            pMonth->MonthSch[i].wWeek = (pMonth->nWeekStart + i) % 7;
            pMonth->MonthSch[i].bFlag = 0;
        }
        else
        {
            pMonth->MonthSch[i].wYear = wYear;
            pMonth->MonthSch[i].wMonth = wMonth + 1;
            pMonth->MonthSch[i].wDay = i - j - nTempDays + 1;
            pMonth->MonthSch[i].wWeek = (pMonth->nWeekStart + i) % 7;
            pMonth->MonthSch[i].bFlag = 0;
        }
    }

    if(!CALE_SetScheduleFlag(pMonth->MonthSch))
    {
        return;
    }

    //calculate the No.week
    if(wMonth >= 2)
    {
        nTempDays = Days[wMonth - 2];
    }
    else
    {
        nTempDays = 0;
    }
//	nWeekNo = (nTempDays + nYearStart) / 7 + 1;
	nWeekNo = (nTempDays + (nYearStart+7-pMonth->nWeekStart)%7) / 7 + 1;
    
    //draw the No.week and date
    for(i = 0; i < MAX_ROW - 1; i ++)
    {
        rcNum.top = pRect->top + (CALE_MONTH_DAY_HEIGHT + 1) * (i + 1) + 1;
        rcNum.bottom = rcNum.top + CALE_MONTH_DAY_HEIGHT;
        rcNum.left = pRect->left + 1;
        rcNum.right = rcNum.left + CALE_MONTH_DAY_WIDTH;
        sprintf(buff, "%d", (nWeekNo + i));

        for(j = 0; j < MAX_LINE; j++)
        {
            if(j == 0)
            {
                rcNum.right = rcNum.left + CALE_MONTH_WEEK_WIDTH;
                hOldText = SetTextColor(hDC, CALE_BLACK);
                rcNum.top += 2;
                DrawText(hDC, buff, -1, &rcNum, DT_VCENTER | DT_CENTER);
                rcNum.top -= 2;
                SetTextColor(hDC, hOldText);
            }
            else
            {
                nSch = i * (MAX_LINE - 1) + j -1;
                if((pMonth->MonthSch[nSch].wYear == sysTime.wYear) && (pMonth->MonthSch[nSch].wMonth == sysTime.wMonth) 
                    && (pMonth->MonthSch[nSch].wDay == sysTime.wDay))
                {
                    pMonth->MonthSch[nSch].bFlag |= CALE_SCHED_SYS;
                }
                if((pMonth->MonthSch[nSch].wYear == wYear) && (pMonth->MonthSch[nSch].wMonth == wMonth) 
                    && (pMonth->MonthSch[nSch].wDay == wDay))
                {
                    pMonth->MonthSch[nSch].bFlag |= CALE_SCHED_TODAY;
                    pMonth->CurSch = nSch;
                }
                if(pMonth->MonthSch[nSch].wMonth != pMonth->sysCurTime.wMonth)
                {
                    pMonth->MonthSch[nSch].bFlag |= CALE_SCHED_OTHERMON;
                }
                CALE_DrawSymbol(hDC, &(pMonth->MonthSch[nSch]), &rcNum);
            }
            rcNum.left = rcNum.right + 1;
            rcNum.right = rcNum.left + CALE_MONTH_DAY_WIDTH;
        }
    }

    return;
}

static void CALE_DrawSymbol(HDC hDC, CaleMonthSch *pMonthSch, PRECT pTmpRc)
{
    HPEN hPen, hOldPen;
    HBRUSH hBrush, hOldBrush;
    COLORREF hOldText;
    char buff[3];
    RECT TmpRc;
    POINT triangle[3];
    COLORREF hOldBkClr = 0;

    CopyRect(&TmpRc, pTmpRc);
    TmpRc.top += 2;

    if(pMonthSch->bFlag & CALE_SCHED_OTHERMON)
    {
        hBrush = CreateBrush(BS_SOLID, CALE_CAMBRIDGEBLUE, 0);
        FillRect(hDC, pTmpRc, (HBRUSH)hBrush);
        DeleteObject(hBrush);
        
        return;
    }

    sprintf(buff, "%d", pMonthSch->wDay);
    if(pMonthSch->bFlag & CALE_SCHED_TODAY)
    {
        hBrush = CreateBrush(BS_SOLID, CALE_NAVYBLUE, 0);
        FillRect(hDC, pTmpRc, (HBRUSH)hBrush);
        hOldBkClr = SetBkColor(hDC, CALE_NAVYBLUE);
        hOldText = SetTextColor(hDC, CALE_WHITE);
        DrawText(hDC, buff, -1, &TmpRc, DT_VCENTER | DT_CENTER);
        SetTextColor(hDC, hOldText);
        SetBkColor(hDC, hOldBkClr);
        DeleteObject(hBrush);
    }
    else if(pMonthSch->bFlag & CALE_SCHED_SYS)
    {
        hBrush = CreateBrush(BS_SOLID, CALE_BLACK, 0);
        FillRect(hDC, pTmpRc, (HBRUSH)hBrush);
        hOldBkClr = SetBkColor(hDC, CALE_BLACK);
        hOldText = SetTextColor(hDC, CALE_WHITE);
        DrawText(hDC, buff, -1, &TmpRc, DT_VCENTER | DT_CENTER);
        SetTextColor(hDC, hOldText);
        SetBkColor(hDC, hOldBkClr);
        DeleteObject(hBrush);
    }
    else
    {
        hOldBkClr = SetBkColor(hDC, CALE_WHITE);
        hBrush = CreateBrush(BS_SOLID, CALE_WHITE, 0);
        FillRect(hDC, pTmpRc, (HBRUSH)hBrush);
        hOldText = SetTextColor(hDC, CALE_BLACK);
#ifdef _NOKIA_CALE_
        if(pMonthSch->wWeek == START_SATURDAY)
        {
            hOldText = SetTextColor(hDC, RGB(150, 200, 150));
        }
        else if(pMonthSch->wWeek == START_SUNDAY)
        {
            hOldText = SetTextColor(hDC, RGB(255, 0, 0));
        }
        else
        {
            hOldText = SetTextColor(hDC, CALE_BLACK);
        }
#endif
        DrawText(hDC, buff, -1, &TmpRc, DT_VCENTER | DT_CENTER);
        SetTextColor(hDC, hOldText);
        DeleteObject(hBrush);
        SetBkColor(hDC, hOldBkClr);
    }

    if((pMonthSch->bFlag & CALE_SCHED_MEET) || (pMonthSch->bFlag & CALE_SCHED_ANNI) 
        || (pMonthSch->bFlag & CALE_SCHED_EVENT))
    {
        hPen = CreatePen(PS_SOLID, 1, CALE_RED);
        hOldPen = SelectObject(hDC, hPen);
        hBrush = CreateBrush(BS_SOLID, CALE_RED, 0);
        hOldBrush = SelectObject(hDC, hBrush);
        
        triangle[0].x = pTmpRc->left;
        triangle[0].y = pTmpRc->bottom - 6;
        triangle[1].x = pTmpRc->left + 5;
        triangle[1].y = pTmpRc->bottom - 1;
        triangle[2].x = pTmpRc->left;
        triangle[2].y = pTmpRc->bottom - 1;
        
        Polygon(hDC, triangle, 3);
        
        SelectObject(hDC, hOldBrush);
        DeleteObject(hBrush);
        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
    }

    return;
}

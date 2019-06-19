/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : CaleWeekView.c
*
* Purpose  : 
*
\**************************************************************************/

#include "CaleHeader.h"

static LRESULT CALLBACK CaleWeekWndProc(HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam);
static void CALE_CalcDate(PCaleWeek pWeek);
static void CALE_DrawLine(HDC hDC, PCaleWeek pWeek, PRECT pRect);
static void CALE_DrawIcon(HDC hDC, PCaleWeek pWeek, PRECT pRect);
static void CALE_DrawSymbol(HDC hDC, PCaleWeek pWeek, PRECT pRect);
static void CALE_DrawTitle(HDC hDC, PCaleWeek pWeek, PRECT pRect);
static void CALE_PaintWeek(HWND hWnd, HDC hDC, PCaleWeek pCaleWeek, PRECT pRect);
void CALE_DrawScroll(PCaleWeek pWeek);

extern int CALE_GetWeekStart(void);
extern int CALE_GetDayStart(void);
extern BOOL CALE_DayIncrease(SYSTEMTIME *psystime, unsigned int day);
extern BOOL CALE_DayReduce(SYSTEMTIME *psystime, unsigned int day);
extern int CALE_CmpSysDate(SYSTEMTIME *pFirstDate, SYSTEMTIME *pSecondDate);
extern BOOL CALE_SetWeekSchedule(PCaleWeek pWeek);
extern TIMEFORMAT GetTimeFormt(void);
extern int CALE_AlmDaysOfMonth(SYSTEMTIME *pDate);
extern void CALE_ReleaseWeekSchNode(CaleWeekSchHead *pHead);
extern BOOL CALE_GetWeekSchByInt(CaleWeekSchHead *pHead, CaleWeekSchNode **pTmpNode, int iTmp);

BOOL CALE_RegisterWeekClass(void* hInst)
{
    WNDCLASS wc;
    HINSTANCE hInstance;
    
    hInstance = (HINSTANCE)hInst;
    
    wc.style        = CS_OWNDC;
    wc.lpfnWndProc  = CaleWeekWndProc;
    wc.cbClsExtra   = 0;
    wc.cbWndExtra   = sizeof(CaleWeek);
    wc.hInstance    = NULL;
    wc.hIcon        = NULL;
    wc.hCursor      = NULL;
    wc.hbrBackground= (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName= "CALEWEEKCLASS";
    
    if(!RegisterClass(&wc))
        return FALSE;
    
    return TRUE;
}

BOOL CALE_UnRegisterWeekClass()
{
    UnregisterClass("CALEWEEKCLASS", NULL);
    return TRUE;
}

static LRESULT CALLBACK CaleWeekWndProc(HWND hWnd, UINT message,
                                         WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult;
    PCREATESTRUCT pCreateStruct;

    PCaleWeek pCaleWeek;
    SCROLLINFO vsi;

    HDC hDC, hMemDC;
    RECT rcClient;
    RECT rcTemp;
    PAINTSTRUCT ps;
    HBITMAP hBmp, hBmpOld;
    HBRUSH hBrush;
//    COLORREF hOldBkClr;
    static HFONT hFontLit;
    static HFONT hFontOld;

    int nDays;
    
    hFontLit = NULL;
    hFontOld = NULL;
    lResult = (LRESULT)TRUE;
    pCaleWeek = GetUserData(hWnd);
    
    switch(message)
    {
    case WM_CREATE:
        {
            hDC = GetDC(hWnd);
            GetFontHandle(&hFontLit, 0);
            hFontOld = SelectObject(hDC, hFontLit);
            ReleaseDC(hWnd, hDC);
            
            memset(pCaleWeek, 0x0, sizeof(CaleWeek));
            pCreateStruct = (LPCREATESTRUCT)lParam;
            
            pCaleWeek->dwStyle = pCreateStruct->style;
            pCaleWeek->hwndParent = pCreateStruct->hwndParent;
            pCaleWeek->wID = (WORD)(DWORD)pCreateStruct->hMenu;
            pCaleWeek->x = pCreateStruct->x;
            pCaleWeek->y = pCreateStruct->y;
            pCaleWeek->width = pCreateStruct->cx;
            pCaleWeek->height = pCreateStruct->cy;
            
            pCaleWeek->MaxYear = MAX_YEAR;
            pCaleWeek->MinYear = MIN_YEAR;
            
            GetLocalTime(&pCaleWeek->sysCurTime);

            pCaleWeek->nWeekStart = CALE_GetWeekStart();
            pCaleWeek->nTimeStart = CALE_GetDayStart();
            pCaleWeek->CurX = pCaleWeek->nTimeStart;
            pCaleWeek->CurY = 0;
            pCaleWeek->bAnEv = 0;
            
            //pCaleWeek->hMeet = LoadImage(NULL, CALE_ICON_MEET, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            pCaleWeek->hAnni = LoadImage(NULL, CALE_ICON_ANNI, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            pCaleWeek->hEvent = LoadImage(NULL, CALE_ICON_EVENT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            //pCaleWeek->hAnniEvent = LoadImage(NULL, CALE_ICON_ANNIEVENT, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
            pCaleWeek->hScroll = CreateWindow(
                "SCROLLBAR",
                "",
                SBS_VERT | WS_VISIBLE | WS_CHILD,
                (CALE_WEEK_TIME_WIDTH + 1) + (CALE_WEEK_DATE_WIDTH + 1) * 7 + 1,
                CALE_WEEK_DATE_HEIGHT + 2,
                5,
                (CALE_WEEK_DATE_HEIGHT + 1) * 7,
                hWnd,
                NULL, NULL, NULL
                );
            
            if (pCaleWeek->hScroll == NULL)
                return FALSE;

            UpdateWindow(hWnd);
        }
    	break;

    case WM_SIZE:
        {
        }
        break;
        
    case WM_PAINT:
        {
            hDC = BeginPaint(hWnd, &ps);
            GetClientRect(hWnd, &rcClient);
            CopyRect(&rcTemp, &rcClient);
            hMemDC = CreateCompatibleDC(hDC);
            hBmp = CreateCompatibleBitmap(hDC, rcClient.right, rcClient.bottom);
            hBmpOld = (HBITMAP)SelectObject(hMemDC, (HGDIOBJ)hBmp);
            hBrush = CreateBrush(BS_SOLID, CALE_WHITE, 0);
            FillRect(hMemDC, &rcClient, (HBRUSH)hBrush);

            rcClient.right -= 3;
            CALE_PaintWeek(hWnd, hMemDC, pCaleWeek, &rcClient);
            
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
            int i, j;
            
            for(i=0; i<MAX_WEEK_ROW; i++)
            {
                for(j=0; j<MAX_WEEK_LINE; j++)
                {
                    CALE_ReleaseWeekSchNode(&pCaleWeek->TimeRow[i][j]);
                }
            }
            
            if (pCaleWeek->hAnni)
            {
                DeleteObject(pCaleWeek->hAnni);
                pCaleWeek->hAnni = NULL;
            }
            if (pCaleWeek->hEvent)
            {
                DeleteObject(pCaleWeek->hEvent);
                pCaleWeek->hEvent = NULL;
            }

            DestroyWindow(pCaleWeek->hScroll);
            pCaleWeek->hScroll = NULL;

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
                    pCaleWeek->CurY --;
                    if(pCaleWeek->CurY != -1)
                    {
                        if(CALE_DayReduce(&pCaleWeek->sysCurTime, 1))
                        {
                            if(pCaleWeek->sysCurTime.wDayOfWeek == 0)
                            {
                                pCaleWeek->sysCurTime.wDayOfWeek = 6;
                            }
                            else
                            {
                                pCaleWeek->sysCurTime.wDayOfWeek --;
                            }
							/*
                            hDC = GetDC(hWnd);
                            hOldBkClr = SetBkColor(hDC, CALE_WHITE);
                            rcClient.right -= 3;

                            CALE_DrawTitle(hDC, pCaleWeek, &rcClient);
                            CALE_DrawIcon(hDC, pCaleWeek, &rcClient);
                            CALE_DrawSymbol(hDC, pCaleWeek, &rcClient);

                            SetBkColor(hDC, hOldBkClr);
                            ReleaseDC(hWnd, hDC);
							//*/
                            GetClientRect(hWnd, &rcClient);
							rcClient.left = (CALE_WEEK_DATE_WIDTH + 1) * pCaleWeek->CurY + (CALE_WEEK_TIME_WIDTH + 1);
							rcClient.right = (CALE_WEEK_DATE_WIDTH + 1) * (pCaleWeek->CurY+2) + (CALE_WEEK_TIME_WIDTH + 1);
							InvalidateRect(hWnd, &rcClient, FALSE);
                            SendMessage(pCaleWeek->hwndParent, WM_COMMAND, 
                                MAKEWPARAM(pCaleWeek->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleWeek->sysCurTime));
							if(pCaleWeek->sysCurTime.wDay == 31 && pCaleWeek->sysCurTime.wMonth == 12)
							{	// jump between years
								SendMessage(pCaleWeek->hwndParent, PWM_SHOWWINDOW, 0, 0);
							}
                        }
                        else
                        {
                            pCaleWeek->CurY ++;
                            break;
                        }
                    }
                    else
                    {
                        if(CALE_DayReduce(&pCaleWeek->sysCurTime, 1))
                        {
                            if(pCaleWeek->sysCurTime.wDayOfWeek == 0)
                            {
                                pCaleWeek->sysCurTime.wDayOfWeek = 6;
                            }
                            else
                            {
                                pCaleWeek->sysCurTime.wDayOfWeek --;
                            }
                            
                            pCaleWeek->CurY = 6;
                            pCaleWeek->CurX = pCaleWeek->nTimeStart;
                            SendMessage(pCaleWeek->hwndParent, WM_COMMAND, 
                                MAKEWPARAM(pCaleWeek->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleWeek->sysCurTime));
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);

                            SendMessage(pCaleWeek->hwndParent, PWM_SHOWWINDOW, 0, 0);
                        }
                        else
                        {
                            pCaleWeek->CurY ++;
                            break;
                        }
                    }
                }
                break;

            case VK_RIGHT:
                {
                    pCaleWeek->CurY ++;
                    if(pCaleWeek->CurY != 7)
                    {
                        if(CALE_DayIncrease(&pCaleWeek->sysCurTime, 1))
                        {
                            pCaleWeek->sysCurTime.wDayOfWeek ++;
                            if(pCaleWeek->sysCurTime.wDayOfWeek == 7)
                            {
                                pCaleWeek->sysCurTime.wDayOfWeek -= 7;
                            }
							/*
                            hDC = GetDC(hWnd);
                            GetClientRect(hWnd, &rcClient);
                            rcClient.right -= 3;
                            hOldBkClr = SetBkColor(hDC, CALE_WHITE);

							CALE_DrawTitle(hDC, pCaleWeek, &rcClient);
							CALE_DrawIcon(hDC, pCaleWeek, &rcClient);
							CALE_DrawSymbol(hDC, pCaleWeek, &rcClient);

                            SetBkColor(hDC, hOldBkClr);
                            ReleaseDC(hWnd, hDC);
							//*/
                            GetClientRect(hWnd, &rcClient);
							rcClient.left = (CALE_WEEK_DATE_WIDTH + 1) * (pCaleWeek->CurY-1) + (CALE_WEEK_TIME_WIDTH + 1);
							rcClient.right = (CALE_WEEK_DATE_WIDTH + 1) * (pCaleWeek->CurY+1) + (CALE_WEEK_TIME_WIDTH + 1);
							InvalidateRect(hWnd, &rcClient, FALSE);

                            SendMessage(pCaleWeek->hwndParent, WM_COMMAND, 
                                MAKEWPARAM(pCaleWeek->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleWeek->sysCurTime));
							if(pCaleWeek->sysCurTime.wDay == 1 && pCaleWeek->sysCurTime.wMonth == 1)
							{	// jump between years
								SendMessage(pCaleWeek->hwndParent, PWM_SHOWWINDOW, 0, 0);
							}
                        }
                        else
                        {
                            pCaleWeek->CurY --;
                            break;
                        }
                    }
                    else
                    {
                        if(CALE_DayIncrease(&pCaleWeek->sysCurTime, 1))
                        {
                            pCaleWeek->sysCurTime.wDayOfWeek ++;
                            if(pCaleWeek->sysCurTime.wDayOfWeek == 7)
                            {
                                pCaleWeek->sysCurTime.wDayOfWeek -= 7;
                            }
                            pCaleWeek->CurY = 0;
                            pCaleWeek->CurX = pCaleWeek->nTimeStart;
                            SendMessage(pCaleWeek->hwndParent, WM_COMMAND, 
                                MAKEWPARAM(pCaleWeek->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleWeek->sysCurTime));
                            InvalidateRect(hWnd, NULL, TRUE);
                            UpdateWindow(hWnd);

                            SendMessage(pCaleWeek->hwndParent, PWM_SHOWWINDOW, 0, 0);
                        }
                        else
                        {
                            pCaleWeek->CurY --;
                            break;
                        }
                    }
                }
                break;

            case VK_UP:
                {
                    pCaleWeek->CurX --;
                    if(pCaleWeek->CurX != -1)
                    {
						/*
                        hDC = GetDC(hWnd);
                        GetClientRect(hWnd, &rcClient);
                        rcClient.right -= 3;
                        hOldBkClr = SetBkColor(hDC, CALE_WHITE);

                        CALE_DrawIcon(hDC, pCaleWeek, &rcClient);
                        CALE_DrawSymbol(hDC, pCaleWeek, &rcClient);

                        SetBkColor(hDC, hOldBkClr);
                        ReleaseDC(hWnd, hDC);
						//*/
						GetClientRect(hWnd, &rcClient);
						rcClient.right -= 3;
						rcClient.top += CALE_WEEK_TIME_HEIGHT+1;
						InvalidateRect(hWnd, &rcClient, FALSE);
                        memset(&vsi, 0x0, sizeof(SCROLLINFO));
                        vsi.cbSize = sizeof(vsi);
                        vsi.fMask  = SIF_ALL;
                        GetScrollInfo(pCaleWeek->hScroll, SB_CTL, &vsi);
                        vsi.fMask = SIF_POS;
                        vsi.nPos = pCaleWeek->CurX;
                        SetScrollInfo(pCaleWeek->hScroll, SB_CTL, &vsi, TRUE);
                        //InvalidateRect(pCaleWeek->hScroll, NULL, TRUE);
                    }
                    else
                    {
                        pCaleWeek->CurX = 0;
                        break;
                    }
                }
                break;

            case VK_DOWN:
                {
                    int iHour;

                    iHour = 0;
                    if(pCaleWeek->bAnEv & CALE_SCHED_ANNI)
                    {
                        iHour ++;
                    }
                    if(pCaleWeek->bAnEv & CALE_SCHED_EVENT)
                    {
                        iHour ++;
                    }
                    
                    pCaleWeek->CurX ++;
                    if(pCaleWeek->CurX <= (23 - (MAX_ROW_WEEK - iHour - 1)) + 1)
                    {
						/*
                        hDC = GetDC(hWnd);
                        GetClientRect(hWnd, &rcClient);
                        rcClient.right -= 3;
                        hOldBkClr = SetBkColor(hDC, CALE_WHITE);

                        CALE_DrawIcon(hDC, pCaleWeek, &rcClient);
                        CALE_DrawSymbol(hDC, pCaleWeek, &rcClient);

                        SetBkColor(hDC, hOldBkClr);
                        ReleaseDC(hWnd, hDC);
						//*/
						GetClientRect(hWnd, &rcClient);
						rcClient.right -= 3;
						rcClient.top += CALE_WEEK_TIME_HEIGHT+1;
						InvalidateRect(hWnd, &rcClient, FALSE);

                        memset(&vsi, 0x0, sizeof(SCROLLINFO));
                        vsi.cbSize = sizeof(vsi);
                        vsi.fMask  = SIF_ALL;
                        GetScrollInfo(pCaleWeek->hScroll, SB_CTL, &vsi);
                        vsi.fMask = SIF_POS;
                        vsi.nPos = pCaleWeek->CurX;
                        SetScrollInfo(pCaleWeek->hScroll, SB_CTL, &vsi, TRUE);
                        //InvalidateRect(pCaleWeek->hScroll, NULL, TRUE);
                    }
                    else
                    {
                        pCaleWeek->CurX = (23 - (MAX_ROW_WEEK - iHour - 1)) + 1;
                        break;
                    }
                }
                break;
                
            case VK_F10:
            case VK_F5:
            case VK_RETURN:
                {
                    SendMessage(pCaleWeek->hwndParent, WM_KEYDOWN, wParam, lParam);
                }
                break;
                
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
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
                
                nDays = CALE_AlmDaysOfMonth((LPSYSTEMTIME)lParam);
                if (((LPSYSTEMTIME)lParam)->wDay < 1 || 
                    ((LPSYSTEMTIME)lParam)->wDay > nDays)
                {
                    return FALSE;
                }

                pCaleWeek->sysCurTime.wYear = ((LPSYSTEMTIME)lParam)->wYear;
                pCaleWeek->sysCurTime.wMonth = ((LPSYSTEMTIME)lParam)->wMonth;
                pCaleWeek->sysCurTime.wDay = ((LPSYSTEMTIME)lParam)->wDay;
                pCaleWeek->sysCurTime.wDayOfWeek = ((LPSYSTEMTIME)lParam)->wDayOfWeek;
                
                InvalidateRect(hWnd, NULL, TRUE);
            //    UpdateWindow(hWnd);
                SendMessage(pCaleWeek->hwndParent, WM_COMMAND, 
                    MAKEWPARAM(pCaleWeek->wID, CALE_CALC_CHANGED), (LPARAM)(&pCaleWeek->sysCurTime));
            }//end if((LPSYSTEMTIME)lParam != NULL)
        }
        break;
        
    case CALE_CALC_GETCUR:
        {
            ((LPSYSTEMTIME)lParam)->wYear = pCaleWeek->sysCurTime.wYear;
            ((LPSYSTEMTIME)lParam)->wMonth = pCaleWeek->sysCurTime.wMonth;
            ((LPSYSTEMTIME)lParam)->wDay = pCaleWeek->sysCurTime.wDay;
            ((LPSYSTEMTIME)lParam)->wDayOfWeek = pCaleWeek->sysCurTime.wDayOfWeek;
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
                //    UpdateWindow(hWnd);
                }
                break;
                
            default:
                lResult = DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case CALE_CONFIG_CHANGED:
        {
            pCaleWeek->nTimeStart = CALE_GetDayStart();
            pCaleWeek->CurX = pCaleWeek->nTimeStart;
            InvalidateRect(hWnd, NULL, TRUE);
        //    UpdateWindow(hWnd);
        }
        break;

    default:
        lResult = DefWindowProc(hWnd, message, wParam, lParam);
    }
    return lResult;
}

static void CALE_PaintWeek(HWND hWnd, HDC hDC, PCaleWeek pCaleWeek, PRECT pRect)
{
    int betWeek;
    COLORREF hOldBkClr;
    static HFONT hFontLit;
    static HFONT hFontOld;
    
    hFontLit = NULL;
    hFontOld = NULL;
    GetFontHandle(&hFontLit, 0);
    hFontOld = SelectObject(hDC, hFontLit);
    
    hOldBkClr = SetBkColor(hDC, CALE_WHITE);

    pCaleWeek->nWeekStart = CALE_GetWeekStart();

    betWeek = pCaleWeek->sysCurTime.wDayOfWeek - pCaleWeek->nWeekStart;
    if(betWeek < 0)
    {
        betWeek += 7;
    }
    memcpy(&pCaleWeek->StartTime, &pCaleWeek->sysCurTime, sizeof(SYSTEMTIME));
    CALE_DayReduce(&pCaleWeek->StartTime, betWeek);
    pCaleWeek->StartTime.wDayOfWeek = pCaleWeek->nWeekStart;
    pCaleWeek->StartTime.wHour = 0;
    pCaleWeek->StartTime.wMinute = 0;
    
    memcpy(&pCaleWeek->EndTime, &pCaleWeek->sysCurTime, sizeof(SYSTEMTIME));
    CALE_DayIncrease(&pCaleWeek->EndTime, (7 - betWeek - 1));
    if(pCaleWeek->nWeekStart == 0)
    {
        pCaleWeek->EndTime.wDayOfWeek = 6;
    }
    else
    {
        pCaleWeek->EndTime.wDayOfWeek = pCaleWeek->nWeekStart - 1;
    }
    pCaleWeek->EndTime.wHour = 23;
    pCaleWeek->EndTime.wMinute = 59;

    pCaleWeek->CurY = betWeek;
    
    CALE_CalcDate(pCaleWeek);
    CALE_DrawLine(hDC, pCaleWeek, pRect);
    CALE_DrawTitle(hDC, pCaleWeek, pRect);
    CALE_DrawIcon(hDC, pCaleWeek, pRect);
    CALE_DrawSymbol(hDC, pCaleWeek, pRect);
    CALE_DrawScroll(pCaleWeek);

    SetBkColor(hDC, hOldBkClr);
    SelectObject(hDC, hFontOld);
    
    return;
}

static void CALE_DrawLine(HDC hDC, PCaleWeek pWeek, PRECT pRect)
{
    HPEN hPen, hOldPen;
    RECT rcTemp;
    int i;
    int j;
    
    CopyRect(&rcTemp, pRect);
    
    hPen = CreatePen(PS_SOLID, 1, (COLORREF)CALE_PURPLE);
    hOldPen = (HPEN)SelectObject(hDC, (HGDIOBJ)hPen);
    
    //draw line
    MoveTo(hDC, rcTemp.left, rcTemp.top, NULL);
    LineTo(hDC, rcTemp.left + (CALE_WEEK_DATE_WIDTH + 1) * (MAX_LINE - 1) + (CALE_WEEK_TIME_WIDTH + 1) + 13, 
        rcTemp.top);
    MoveTo(hDC, rcTemp.left, rcTemp.top, NULL);
    LineTo(hDC, rcTemp.left, rcTemp.top + (CALE_WEEK_DATE_HEIGHT + 1) * MAX_ROW_WEEK);
    MoveTo(hDC, rcTemp.left + CALE_WEEK_TIME_WIDTH + 1, rcTemp.top + CALE_WEEK_DATE_HEIGHT + 1, NULL);
    LineTo(hDC, rcTemp.left + (CALE_WEEK_DATE_WIDTH + 1) * (MAX_LINE - 1) + (CALE_WEEK_TIME_WIDTH + 1) + 13, 
        rcTemp.top + CALE_WEEK_DATE_HEIGHT + 1);

    j = 0;
    if((pWeek->bAnEv & CALE_SCHED_ANNI) && (pWeek->bAnEv & CALE_SCHED_EVENT))
    {
        j++;
        MoveTo(hDC, rcTemp.left + CALE_WEEK_TIME_WIDTH + 1, rcTemp.top + (CALE_WEEK_DATE_HEIGHT + 1) * (j + 1), NULL);
        LineTo(hDC, rcTemp.left + (CALE_WEEK_DATE_WIDTH + 1) * (MAX_LINE - 1) + (CALE_WEEK_TIME_WIDTH + 1) + 13, 
            rcTemp.top + (CALE_WEEK_DATE_HEIGHT + 1) * (j + 1));
    }
    
    for(i = 0; i< MAX_ROW_WEEK - 1; i ++)
    {
        MoveTo(hDC, rcTemp.left, rcTemp.top + (CALE_WEEK_DATE_HEIGHT + 1) * (i + j + 2), NULL);
        LineTo(hDC, rcTemp.left + (CALE_WEEK_DATE_WIDTH + 1) * (MAX_LINE - 1) + (CALE_WEEK_TIME_WIDTH + 1), 
            rcTemp.top + (CALE_WEEK_DATE_HEIGHT + 1) * (i + j + 2));
    }
    for(i = 0; i < MAX_LINE + 1; i ++)
    {
        MoveTo(hDC, rcTemp.left + (CALE_WEEK_DATE_WIDTH + 1) * i + (CALE_WEEK_TIME_WIDTH + 1), 
            rcTemp.top, NULL);
        LineTo(hDC, rcTemp.left + (CALE_WEEK_DATE_WIDTH + 1) * i + (CALE_WEEK_TIME_WIDTH + 1), 
            rcTemp.top + (CALE_WEEK_DATE_HEIGHT + 1) * MAX_ROW_WEEK);
    }
    
    SelectObject(hDC, (HGDIOBJ)hOldPen);
    DeleteObject((HGDIOBJ)hPen);
    
    return;
}

static void CALE_DrawTitle(HDC hDC, PCaleWeek pWeek, PRECT pRect)
{
    int i, j;
    RECT rcNum;
    COLORREF hOldText, hOldText1;
    HBRUSH hBrush;
    COLORREF hOldBkClr;
    
    SYSTEMTIME sysTime;
    int CmpRes;
    
    CopyRect(&rcNum, pRect);
    rcNum.left = pRect->left + CALE_WEEK_TIME_WIDTH + 2;
    rcNum.top = pRect->top + 1;
    rcNum.right = rcNum.left + CALE_WEEK_DATE_WIDTH;
    rcNum.bottom = rcNum.top + CALE_WEEK_DATE_HEIGHT;
    
    GetLocalTime(&sysTime);
    CmpRes = CALE_CmpSysDate(&sysTime, &pWeek->StartTime);
    if(CmpRes != Early)
    {
        CmpRes = CALE_CmpSysDate(&sysTime, &pWeek->EndTime);
        if(CmpRes != Late)
        {
            CmpRes = 4;
        }
    }
    
    hOldText = SetTextColor(hDC, CALE_BLACK);
    
    for(i = 0; i < MAX_LINE - 1; i ++)
    {
        j = i + pWeek->nWeekStart;
        
        if(j > 6)
        {
            j -= 7;
        }
        if(i == pWeek->CurY)
        {
            hBrush = CreateBrush(BS_SOLID, CALE_NAVYBLUE, 0);
            FillRect(hDC, &rcNum, (HBRUSH)hBrush);
            DeleteObject(hBrush);
            hOldBkClr = SetBkColor(hDC, CALE_NAVYBLUE);
            hOldText1 = SetTextColor(hDC, CALE_WHITE);
        }
        else if((CmpRes == 4) && (j == sysTime.wDayOfWeek))
        {
            hBrush = CreateBrush(BS_SOLID, CALE_BLACK, 0);
            FillRect(hDC, &rcNum, (HBRUSH)hBrush);
            DeleteObject(hBrush);
            hOldBkClr = SetBkColor(hDC, CALE_BLACK);
            hOldText1 = SetTextColor(hDC, CALE_WHITE);
        }
        else
        {
            hBrush = CreateBrush(BS_SOLID, CALE_WHITE, 0);
            FillRect(hDC, &rcNum, (HBRUSH)hBrush);
            //hOldBkClr = SetBkColor(hDC, CALE_WHITE);
            DeleteObject(hBrush);
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

        if((i == pWeek->CurY) || ((CmpRes == 4) && (j == sysTime.wDayOfWeek)))
        {
            SetTextColor(hDC, hOldText1);
            SetBkColor(hDC, hOldBkClr);
        }
        rcNum.left = rcNum.right + 1;
        rcNum.right = rcNum.left + CALE_WEEK_DATE_WIDTH;
    }
    SetTextColor(hDC, hOldText);

    return;
}

static void CALE_CalcDate(PCaleWeek pWeek)
{
    int i, j;

    pWeek->bAnEv = 0;
    for(i=0; i<MAX_WEEK_ROW; i++)
    {
        for(j=0; j<MAX_WEEK_LINE; j++)
        {
            CALE_ReleaseWeekSchNode(&pWeek->TimeRow[i][j]);
        }
    }
    memset(pWeek->IconRow, 0x0, (sizeof(BYTE) * (MAX_WEEK_LINE)));
    memset(pWeek->TimeRow, 0x0, (sizeof(CaleWeekSchHead) * (MAX_WEEK_LINE) * (MAX_WEEK_ROW)));
    if(!CALE_SetWeekSchedule(pWeek))
    {
        CALE_ReleaseWeekSchNode(&pWeek->TimeRow[i][j]);
    }

    return;
}

static void CALE_DrawIcon(HDC hDC, PCaleWeek pWeek, PRECT pRect)
{
    int i;
    RECT rcNum;
    HBRUSH hBrush;
    BITMAP tmpbm;
    COLORREF hOldBkClr = 0;
    
    if(pWeek->bAnEv == 0)
    {
        return;
    }
    
    CopyRect(&rcNum, pRect);

    rcNum.left = pRect->left + (CALE_WEEK_TIME_WIDTH + 1) + 1;
    rcNum.top = pRect->top + (CALE_WEEK_DATE_HEIGHT + 1) + 1;
    rcNum.right = rcNum.left + CALE_WEEK_DATE_WIDTH;
    rcNum.bottom = rcNum.top + CALE_WEEK_DATE_HEIGHT;
    
    memset(&tmpbm, 0x0, sizeof(BITMAP));
    GetObject(pWeek->hAnni, sizeof(BITMAP), &tmpbm);

    if(pWeek->bAnEv & CALE_SCHED_ANNI)
    {
        for(i = 0; i < (MAX_LINE - 1); i ++)
        {
            if(i == pWeek->CurY)
            {
                hBrush = CreateBrush(BS_SOLID, CALE_NAVYBLUE, 0);
                FillRect(hDC, &rcNum, (HBRUSH)hBrush);
                DeleteObject(hBrush);
                hOldBkClr = SetBkColor(hDC, CALE_NAVYBLUE);
            }
            else
            {
                hBrush = CreateBrush(BS_SOLID, CALE_WHITE, 0);
                FillRect(hDC, &rcNum, (HBRUSH)hBrush);
                DeleteObject(hBrush);
            }
            if(pWeek->IconRow[i] & CALE_SCHED_ANNI)
            {
                if((tmpbm.bmWidth > CALE_WEEK_DATE_WIDTH) || (tmpbm.bmHeight > CALE_WEEK_DATE_HEIGHT))
                {
                    BitBlt(hDC,
                        rcNum.left,
                        rcNum.top,
                        CALE_WEEK_DATE_WIDTH, 
                        CALE_WEEK_DATE_HEIGHT, 
                        (HDC)pWeek->hAnni, 0, 0, ROP_SRC);
                }
                else
                {
                    BitBlt(hDC, 
                        (rcNum.left + (CALE_WEEK_DATE_WIDTH - tmpbm.bmWidth) / 2), 
                        (rcNum.top + (CALE_WEEK_DATE_HEIGHT - tmpbm.bmHeight) / 2),
                        tmpbm.bmWidth,
                        tmpbm.bmHeight, 
                        (HDC)pWeek->hAnni, 0, 0, ROP_SRC);
                }
            }
            
            if(i == pWeek->CurY)
            {
                SetBkColor(hDC, hOldBkClr);
            }
            rcNum.left = rcNum.right + 1;
            rcNum.right = rcNum.left + CALE_WEEK_DATE_WIDTH;
        }
        rcNum.left = pRect->left + (CALE_WEEK_TIME_WIDTH + 1) + 1;
        rcNum.top = rcNum.bottom + 1;
        rcNum.right = rcNum.left + CALE_WEEK_DATE_WIDTH;
        rcNum.bottom = rcNum.top + CALE_WEEK_DATE_HEIGHT;
    }
    if(pWeek->bAnEv & CALE_SCHED_EVENT)
    {
        for(i = 0; i < (MAX_LINE - 1); i ++)
        {
            if(i == pWeek->CurY)
            {
                hBrush = CreateBrush(BS_SOLID, CALE_NAVYBLUE, 0);
                FillRect(hDC, &rcNum, (HBRUSH)hBrush);
                DeleteObject(hBrush);
                hOldBkClr = SetBkColor(hDC, CALE_NAVYBLUE);
            }
            else
            {
                hBrush = CreateBrush(BS_SOLID, CALE_WHITE, 0);
                FillRect(hDC, &rcNum, (HBRUSH)hBrush);
                DeleteObject(hBrush);
            }
            if(pWeek->IconRow[i] & CALE_SCHED_EVENT)
            {
                if((tmpbm.bmWidth > CALE_WEEK_DATE_WIDTH) || (tmpbm.bmHeight > CALE_WEEK_DATE_HEIGHT))
                {
                    BitBlt(hDC,
                        rcNum.left,
                        rcNum.top,
                        CALE_WEEK_DATE_WIDTH, 
                        CALE_WEEK_DATE_HEIGHT, 
                        (HDC)pWeek->hEvent, 0, 0, ROP_SRC);
                }
                else
                {
                    BitBlt(hDC, 
                        (rcNum.left + (CALE_WEEK_DATE_WIDTH - tmpbm.bmWidth) / 2), 
                        (rcNum.top + (CALE_WEEK_DATE_HEIGHT - tmpbm.bmHeight) / 2),
                        tmpbm.bmWidth,
                        tmpbm.bmHeight, 
                        (HDC)pWeek->hEvent, 0, 0, ROP_SRC);
                }
            }
            
            if(i == pWeek->CurY)
            {
                SetBkColor(hDC, hOldBkClr);
            }
            rcNum.left = rcNum.right + 1;
            rcNum.right = rcNum.left + CALE_WEEK_DATE_WIDTH;
        }
    }
    
    return;
}

static void CALE_DrawSymbol(HDC hDC, PCaleWeek pWeek, PRECT pRect)
{
    int i, j;
    int k;
    int iStart, iEnd, iRem;
    int timeformat;
    RECT rcNum;
    char buff[5];
    
    COLORREF hOldText;
    HBRUSH hBrush, hOldBrush;
    HPEN hPen, hOldPen;
    COLORREF hOldBkClr = 0;

    CaleWeekSchNode *DrawSchNode;
    
    timeformat = GetTimeFormt();

    k = 0;
    if(pWeek->bAnEv & CALE_SCHED_ANNI)
    {
        k ++;
    }
    if(pWeek->bAnEv & CALE_SCHED_EVENT)
    {
        k ++;
    }
    
    //draw the No.week and date
    for(i = 0; i < MAX_ROW_WEEK - k - 1; i ++)
    {
        memset(buff, 0x0, 5);

        if(timeformat == TF_12)
        {
            if((pWeek->CurX + i) == 0)
            {
                sprintf(buff, "%dAM", 12);
            }
            else if((pWeek->CurX + i) < 10)
            {
                sprintf(buff, "%0dAM", pWeek->CurX + i);
            }
            else if((pWeek->CurX + i) < 12)
            {
                sprintf(buff, "%dAM", pWeek->CurX + i);
            }
            else if((pWeek->CurX + i) == 12)
            {
                sprintf(buff, "%dPM", 12);
            }
            else if((pWeek->CurX + i) < 22)
            {
                sprintf(buff, "%0dPM", pWeek->CurX - 12 + i);
            }
            else
            {
                sprintf(buff, "%dPM", pWeek->CurX - 12 + i);
            }
        }
        else
        {
            if((pWeek->CurX + i) < 10)
            {
                sprintf(buff, "0%d:00", pWeek->CurX + i);
            }
            else
            {
                sprintf(buff, "%d:00", pWeek->CurX + i);
            }
        }
        
        rcNum.top = pRect->top + (CALE_WEEK_DATE_HEIGHT + 1) * (i + k + 1) + 1;
        rcNum.bottom = rcNum.top + CALE_WEEK_DATE_HEIGHT;
        rcNum.left = pRect->left + 1;
        rcNum.right = rcNum.left + CALE_WEEK_TIME_WIDTH;

        for(j = 0; j < MAX_LINE; j++)
        {
            if(pWeek->CurX + i > 23)
            {
                return;
            }
            if(j == 0)
            {
                hBrush = CreateBrush(BS_SOLID, CALE_WHITE, 0);
                FillRect(hDC, &rcNum, (HBRUSH)hBrush);
                hOldBkClr = SetBkColor(hDC, CALE_WHITE);
                DeleteObject(hBrush);
                
                rcNum.right = rcNum.left + CALE_WEEK_TIME_WIDTH;
                hOldText = SetTextColor(hDC, CALE_BLACK);
                DrawText(hDC, buff, -1, &rcNum, DT_VCENTER | DT_CENTER);
                SetTextColor(hDC, hOldText);
                SetBkColor(hDC, hOldBkClr);
            }
            else
            {
				//setup background color
				if(pWeek->CurY == (j - 1))
				{
					hBrush = CreateBrush(BS_SOLID, CALE_NAVYBLUE, 0);
					FillRect(hDC, &rcNum, (HBRUSH)hBrush);
					hOldBkClr = SetBkColor(hDC, CALE_NAVYBLUE);
					DeleteObject(hBrush);
				}
				else
				{
					hBrush = CreateBrush(BS_SOLID, CALE_WHITE, 0);
					FillRect(hDC, &rcNum, (HBRUSH)hBrush);
					hOldBkClr = SetBkColor(hDC, CALE_WHITE);
					DeleteObject(hBrush);
				}

                //draw symbol
                if(pWeek->TimeRow[pWeek->CurX + i][j - 1].Num && pWeek->TimeRow[pWeek->CurX + i][j - 1].pNext)
                {
                    hBrush = CreateBrush(BS_SOLID, CALE_YELLOW, 0);
                    hOldBrush = SelectObject(hDC, hBrush);
                    hPen = CreatePen(PS_SOLID, 1, CALE_YELLOW);
                    hOldPen = SelectObject(hDC, hPen);

                    DrawSchNode = pWeek->TimeRow[pWeek->CurX + i][j - 1].pNext;
                    while(DrawSchNode)
                    {
                        if(DrawSchNode->nStart == DrawSchNode->nEnd)
                        {
                            DrawSchNode = DrawSchNode->pNext;
                            continue;
                        }
                        iStart = rcNum.top + CALE_WEEK_DATE_HEIGHT * DrawSchNode->nStart / 10;
                        iRem = CALE_WEEK_DATE_HEIGHT * DrawSchNode->nStart % 10;
                        iStart = (iRem > 4) ? (iStart + 1) : iStart;
                        
                        iEnd = rcNum.top + (CALE_WEEK_DATE_HEIGHT * DrawSchNode->nEnd / 10);
                        iRem = CALE_WEEK_DATE_HEIGHT * DrawSchNode->nEnd % 10;
                        iEnd = (iRem > 4) ? (iEnd + 1) : iEnd;
                        Rectangle(hDC, (rcNum.left + 3), iStart, (rcNum.right - 3), iEnd);

                        DrawSchNode = DrawSchNode->pNext;
                    }

                    SelectObject(hDC, hOldPen);
                    DeleteObject(hPen);
                    SelectObject(hDC, hOldBrush);
                    DeleteObject(hBrush);
                }

                SetBkColor(hDC, hOldBkClr);
                hOldBkClr = 0;
            }
            rcNum.left = rcNum.right + 1;
            rcNum.right = rcNum.left + CALE_WEEK_DATE_WIDTH;
        }
    }
    
    return;
}

void CALE_DrawScroll(PCaleWeek pWeek)
{
    SCROLLINFO ScrollInfo;
    int iHour;
    
    iHour = 0;
    if(pWeek->bAnEv & CALE_SCHED_ANNI)
    {
        iHour ++;
    }
    if(pWeek->bAnEv & CALE_SCHED_EVENT)
    {
        iHour ++;
    }

    memset(&ScrollInfo, 0x0, sizeof(SCROLLINFO));
    ScrollInfo.cbSize = sizeof(SCROLLINFO);
    ScrollInfo.fMask  = SIF_ALL;
    ScrollInfo.nMin = 0;
    ScrollInfo.nMax = 23;
    ScrollInfo.nPos = pWeek->CurX;
    ScrollInfo.nPage = MAX_ROW_WEEK - iHour - 1;
    SetScrollInfo(pWeek->hScroll, SB_CTL, &ScrollInfo, TRUE);

    return;
}

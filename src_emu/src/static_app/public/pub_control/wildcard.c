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

//#include "hpwin.h"
#include "window.h"
#include "plx_pdaex.h"
#include "string.h"

#define MAXCHARWIDTH    14
#define MAXHEIGHT       23
#define CHARHEIGHT      23
#define TIMERID         100
#define ET_CARET        500
#define TITLEHEIGHT     25
#define LEFTGAP         3

#define BKBMPNAME "/usr/local/lib/gui/resources/listfocus.bmp"
#define RES_STR_CALLTO   ML("Call to:")

typedef struct
{
    char    cContent[46];
    BYTE    IsWildCard[46];
	DWORD   wState;			// style
    WORD    wID;
    BOOL    bFocus;
    int     nIndex;
    int     nLen;
    int     nLenInit;
    int     nWildCard;
    int     nInitWildCard;
    int     nLenMax;
    BOOL    bShow;
    BOOL    bShowCaret;
    BOOL    bTimer;
} WILDCARDDATA, *PWILDCARDDATA;


static LRESULT CALLBACK WILDCARD_WndProc(HWND hWnd, UINT wMsgCmd, 
                                       WPARAM wParam, LPARAM lParam);
static void PaintWildCard(HWND hWnd, HDC hdc, PWILDCARDDATA pWildCardData);

BOOL WILDCARD_RegisterClass(void)
{
    WNDCLASS wc;

    wc.style            = CS_OWNDC;//CS_PARENTDC;
    wc.lpfnWndProc      = WILDCARD_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(WILDCARDDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "WILDCARD";

    if (!RegisterClass(&wc))
        return FALSE;

   
    return TRUE;
}

static LRESULT CALLBACK WILDCARD_WndProc(HWND hWnd, UINT wMsgCmd, 
                                       WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    PWILDCARDDATA   pWildCardData;
    PCREATESTRUCT   pCreateStruct;
    HDC             hdc;
    WORD            vkey;
    PSTR            pch, pszText;
    RECT            rcChar, rcClient;
    int             nCount, x, y, i, nOldIndex;
    char            cchar[2];
    HFONT           hFont;
    BOOL            fRedraw;
    int             nBkMode, nTextMax;
    COLORREF        BkColor;
    BOOL            bRet;

    pWildCardData = (PWILDCARDDATA)GetUserData(hWnd);
	lResult = 0; 
	
    switch (wMsgCmd)
    {
    case WM_NCCREATE :

        pCreateStruct = (PCREATESTRUCT)lParam;

        memset(pWildCardData, 0, sizeof(WILDCARDDATA));

        // Initializes the internal data
		pWildCardData->wState = LOWORD(pCreateStruct->style);
		pWildCardData->wID = (WORD)(DWORD)pCreateStruct->hMenu;
        strncpy(pWildCardData->cContent, pCreateStruct->lpszName, 41);
        pWildCardData->nIndex = -1;
        pWildCardData->nLen = strlen(pWildCardData->cContent);
        pWildCardData->nLenInit = pWildCardData->nLen;
        
        if (pCreateStruct->cy > TITLEHEIGHT)
            pWildCardData->nLenMax = (pCreateStruct->cx / MAXCHARWIDTH) * ((pCreateStruct->cy - TITLEHEIGHT) / MAXHEIGHT);
        else
            pWildCardData->nLenMax = 0;
        
        pch = pWildCardData->cContent;
        i = 0;
        while(pch[0] != '\0')
        {
            if (pch[0] == 0x3F)
            {
                pWildCardData->nWildCard ++;
                pWildCardData->IsWildCard[i] = 1;

            }
            pch ++;
            i++;
        }
        pWildCardData->nInitWildCard = pWildCardData->nWildCard;
        CreateCaret(hWnd, (HBITMAP)1, 2, CHARHEIGHT);
  
        i = 0;
        while (pWildCardData->nLen > pWildCardData->nLenMax)
        {
            pWildCardData->nLenMax += pCreateStruct->cx / MAXCHARWIDTH;
            i ++;
        }
        if (i != 0)
        {
            MoveWindow(hWnd, 0, 0, pCreateStruct->cx, 
                pCreateStruct->cy + i * MAXHEIGHT + 5, TRUE);
        }

        lResult = (LRESULT)TRUE;

        break;

    case WM_SETFOCUS:
        pWildCardData->bFocus = 1;
        if (pWildCardData->nIndex == -1 && pWildCardData->nWildCard != 0)
        {
            pWildCardData->nIndex = 0;
            pch = pWildCardData->cContent;
            while(pch[0] != 0x3F && pWildCardData->nIndex != pWildCardData->nLen)
            {
                pWildCardData->nIndex ++;
                pch ++ ;
            }
        }
        if (pWildCardData->nWildCard && !pWildCardData->bTimer)
        {
            SetTimer(hWnd, TIMERID, ET_CARET, NULL);
            pWildCardData->bTimer = TRUE;
        }
        if(pWildCardData->bShowCaret)
        {
            bRet = ShowCaret(hWnd);
            if (bRet == FALSE)
            {
                GetClientRect(hWnd, &rcClient);
                CreateCaret(hWnd, (HBITMAP)1, 2, CHARHEIGHT);
                nCount = rcClient.right / MAXCHARWIDTH;
                x = (pWildCardData->nLen % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                y = (pWildCardData->nLen / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                SetCaretPos(x, y);
                ShowCaret(hWnd);
            }
        }
        SetCaretColor(RGB(255, 255, 255));
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_KILLFOCUS:
        pWildCardData->bFocus = 0;
        if (pWildCardData->nWildCard && pWildCardData->bTimer)
        {
            KillTimer(hWnd, TIMERID);
            pWildCardData->bTimer = FALSE;
            pWildCardData->bShow = TRUE;
        }
        if (pWildCardData->bShowCaret)
        {
            pWildCardData->bShowCaret = FALSE;
            HideCaret(hWnd);
        }
        
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_TIMER:
        GetClientRect(hWnd, &rcClient);
        rcClient.top += TITLEHEIGHT;
        nCount = rcClient.right / MAXCHARWIDTH;
        rcChar.left = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
        rcChar.right = ((pWildCardData->nIndex % nCount) + 1)* MAXCHARWIDTH + rcClient.left + LEFTGAP;
        rcChar.top = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top;
        rcChar.bottom = ((pWildCardData->nIndex / nCount) + 1)* CHARHEIGHT + rcClient.top;
        
        if (!pWildCardData->bShow)
        {
            /*cchar[0] = pWildCardData->cContent[pWildCardData->nIndex];
            cchar[1] = '\0';
            DrawText(hdc, cchar, 1, &rcChar, DT_VCENTER | DT_HCENTER);*/
            pWildCardData->bShow = TRUE;
        }
        else
        {
            //ClearRect(hdc, &rcChar, COLOR_WHITE);
            pWildCardData->bShow = FALSE;
        }
        InvalidateRect(hWnd, &rcChar, NULL);
        break;
        
    case WM_GETTEXT:
        nTextMax = (int)wParam;
        pszText =  (PSTR)lParam;

        if (pWildCardData->nLen < nTextMax)
        {
            strncpy(pszText, pWildCardData->cContent, pWildCardData->nLen);
            pszText[pWildCardData->nLen] = '\0';
            return pWildCardData->nLen;
        }
        else
        {
            strncpy(pszText, pWildCardData->cContent, nTextMax - 1);
            pszText[nTextMax - 1] = '\0';
            return nTextMax;
        }

        
    case WM_GETTEXTLENGTH:
        return (LRESULT)pWildCardData->nLen;
        
    case WM_KEYDOWN:
        vkey = LOWORD(wParam);
        
        switch (vkey)
        {
        case VK_F1:
        case VK_F5:
        case VK_RETURN:
        case VK_F2://¹Ò¶Ï¼üÍË³ö
        
            SendMessage(GetParent(hWnd), WM_KEYDOWN, wParam, lParam);
            return 0;

        case VK_F10:
            if (pWildCardData->nIndex == pWildCardData->nLen && 
                pWildCardData->nIndex > pWildCardData->nLenInit)
            {
                pWildCardData->nIndex --;
                pWildCardData->nLen--;
                GetClientRect(hWnd, &rcClient);
                nCount = rcClient.right / MAXCHARWIDTH;
                x = (pWildCardData->nLen % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                y = (pWildCardData->nLen / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                SetCaretPos(x, y);
                pWildCardData->cContent[pWildCardData->nIndex] = '\0';
                if ((pWildCardData->nLen + 1) % (rcClient.right / MAXCHARWIDTH) == 0)
                {
                    MoveWindow(hWnd, 0, 0, rcClient.right - rcClient.left, 
                        rcClient.bottom - rcClient.top - MAXHEIGHT, TRUE);
                    pWildCardData->nLenMax -= (rcClient.right - rcClient.left)/ MAXCHARWIDTH;
                }
            }
            else if (pWildCardData->nIndex > pWildCardData->nLenInit)
            {
                for(i = pWildCardData->nIndex - 1; i < pWildCardData->nLen; i++)
                    pWildCardData->cContent[i] = pWildCardData->cContent[i + 1];
                pWildCardData->nLen --;
                pWildCardData->nIndex --;
                GetClientRect(hWnd, &rcClient);
                nCount = rcClient.right / MAXCHARWIDTH;
                x = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                y = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                SetCaretPos(x, y);
                if ((pWildCardData->nLen + 1) % (rcClient.right / MAXCHARWIDTH) == 0)
                {
                    MoveWindow(hWnd, 0, 0, rcClient.right - rcClient.left, 
                        rcClient.bottom - rcClient.top - MAXHEIGHT, TRUE);
                    pWildCardData->nLenMax -= (rcClient.right - rcClient.left)/ MAXCHARWIDTH;
                }
            }
            else
            {
                if (!pWildCardData->bTimer)
                {
                    SetTimer(hWnd, TIMERID, ET_CARET, NULL);
                    pWildCardData->bTimer = TRUE;
                }
                GetClientRect(hWnd, &rcClient);
                nCount = rcClient.right / MAXCHARWIDTH;
                if (pWildCardData->IsWildCard[pWildCardData->nIndex] == 1 && 
                        pWildCardData->cContent[pWildCardData->nIndex] == 0x3F)
                {
//                    rcChar.left = ((pWildCardData->nIndex) % nCount) * MAXCHARWIDTH + rcClient.left;
//                    rcChar.right = (((pWildCardData->nIndex)% nCount) + 1)* MAXCHARWIDTH + rcClient.left;
//                    rcChar.top = (pWildCardData->nIndex/ nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
//                    rcChar.bottom = ((pWildCardData->nIndex/ nCount) + 1)* CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                    
                    cchar[0] = 0x3F;
                    cchar[1] = '\0';
//                    nBkMode = SetBkMode(hdc, BM_TRANSPARENT);
//                    BkColor = SetTextColor(hdc, COLOR_WHITE);
//                    DrawText(hdc, cchar, 1, &rcChar, DT_VCENTER | DT_HCENTER);
//                    SetTextColor(hdc, BkColor);
//                    SetBkMode(hdc, nBkMode);
                }
                
                while(pWildCardData->nIndex != -1)
                {
                    
                    if (pWildCardData->IsWildCard[pWildCardData->nIndex] == 1 && 
                        pWildCardData->cContent[pWildCardData->nIndex] != 0x3F)
                    {
//                        rcChar.left = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left;
//                        rcChar.right = ((pWildCardData->nIndex % nCount) + 1)* MAXCHARWIDTH + rcClient.left;
//                        rcChar.top = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
//                        rcChar.bottom = ((pWildCardData->nIndex / nCount) + 1)* CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                        
                        cchar[0] = 0x3F;
                        cchar[1] = '\0';
//                        nBkMode = SetBkMode(hdc, BM_TRANSPARENT);
//                        BkColor = SetTextColor(hdc, COLOR_WHITE);
//                        DrawText(hdc, cchar, 1, &rcChar, DT_VCENTER | DT_HCENTER);
//                        SetTextColor(hdc, BkColor);
//                        SetBkMode(hdc, nBkMode);
                        pWildCardData->cContent[pWildCardData->nIndex] = 0x3F;

                        if (pWildCardData->bShowCaret)
                        {
                            HideCaret(hWnd);
                            pWildCardData->bShowCaret = FALSE;
                        }
                        
                        pWildCardData->nWildCard ++;
                        break;
                    }
                    pWildCardData->nIndex --;
                }
                if (pWildCardData->nWildCard == pWildCardData->nInitWildCard)
                    SendMessage(GetParent(hWnd), WM_SETRBTNTEXT, NULL, (LPARAM)ML("Cancel"));
                if (pWildCardData->nIndex == -1)
                {
                    SendMessage(GetParent(hWnd), WM_KEYDOWN, wParam, lParam);
                    break;
                }
                else
                    SendMessage(GetParent(hWnd), WM_COMMAND, 
                        MAKELONG(pWildCardData->wID, EN_CHANGE), (LPARAM)hWnd);
            }
            InvalidateRect(hWnd, NULL, TRUE);
            break;

        case VK_F4:
            SendChar(hWnd, '#');
            break;
        case VK_F3:
            SendChar(hWnd, '*');
            break;

        case VK_LEFT:
            if (pWildCardData->nIndex != -1 && !pWildCardData->bShow)
            {
                hdc = GetDC(hWnd);
                GetClientRect(hWnd, &rcClient);
                nCount = rcClient.right / MAXCHARWIDTH;
                rcChar.left = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                rcChar.right = ((pWildCardData->nIndex % nCount) + 1)* MAXCHARWIDTH + rcClient.left + LEFTGAP;
                rcChar.top = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                rcChar.bottom = ((pWildCardData->nIndex / nCount) + 1)* CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                
                cchar[0] = pWildCardData->cContent[pWildCardData->nIndex];
                cchar[1] = '\0';
                nBkMode = SetBkMode(hdc, BM_TRANSPARENT);
                BkColor = SetTextColor(hdc, COLOR_WHITE);
                DrawText(hdc, cchar, 1, &rcChar, DT_VCENTER | DT_HCENTER);
                SetTextColor(hdc, BkColor);
                SetBkMode(hdc, nBkMode);
                pWildCardData->bShow = TRUE;
                ReleaseDC(hWnd, hdc);
            }
            if (pWildCardData->nIndex == -1)
            {
                if (pWildCardData->nWildCard != 0)
                {
                    pch = pWildCardData->cContent + pWildCardData->nLen;
                    pWildCardData->nIndex = pWildCardData->nLen;
                    while(pch[0] != 0x3F && pWildCardData->nIndex != -1)
                    {
                        pWildCardData->nIndex --;
                        pch -- ;
                    }
                }
                else
                {
                    pWildCardData->nIndex = pWildCardData->nLen;
                }
            }
            else if (pWildCardData->nIndex == 0)
                break;
            else if (pWildCardData->nIndex < pWildCardData->nLenInit)
            {
                nOldIndex = pWildCardData->nIndex;
                pWildCardData->nIndex --;
                while(pWildCardData->nIndex != -1)
                {
                    if (pWildCardData->IsWildCard[pWildCardData->nIndex] == 1)
                        break;
                    pWildCardData->nIndex --;
                }
                if (pWildCardData->nIndex == -1)
                    pWildCardData->nIndex = nOldIndex;
            }
            else if (pWildCardData->nIndex == pWildCardData->nLenInit)
            {
                nOldIndex = pWildCardData->nIndex;
                pWildCardData->nIndex --;
                while(pWildCardData->nIndex != -1)
                {
                    if (pWildCardData->IsWildCard[pWildCardData->nIndex] == 1)
                        break;
                    pWildCardData->nIndex --;
                }
                if (pWildCardData->nIndex == -1)
                    pWildCardData->nIndex = nOldIndex;
                if (!pWildCardData->bTimer)
                {
                    SetTimer(hWnd, TIMERID, ET_CARET, NULL);
                    pWildCardData->bTimer = TRUE;
                }
                if (pWildCardData->bShowCaret)
                {
                    HideCaret(hWnd);
                    pWildCardData->bShowCaret = FALSE;
                }
            }
            else
            {
                pWildCardData->nIndex --;
                GetClientRect(hWnd, &rcClient);
                nCount = rcClient.right / MAXCHARWIDTH;
                
                //ClearRect(hdc, &rcChar, COLOR_WHITE);
                
                x = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                y = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                SetCaretPos(x, y);
            }
            //InvalidateRect(hWnd, NULL, TRUE);
            break;

        case VK_RIGHT:
            if (pWildCardData->nIndex != -1 && !pWildCardData->bShow)
            {
                hdc = GetDC(hWnd);
                GetClientRect(hWnd, &rcClient);
                nCount = rcClient.right / MAXCHARWIDTH;
                rcChar.left = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                rcChar.right = ((pWildCardData->nIndex % nCount) + 1)* MAXCHARWIDTH + rcClient.left + LEFTGAP;
                rcChar.top = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                rcChar.bottom = ((pWildCardData->nIndex / nCount) + 1)* CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                
                cchar[0] = pWildCardData->cContent[pWildCardData->nIndex];
                cchar[1] = '\0';
                nBkMode = SetBkMode(hdc, BM_TRANSPARENT);
                BkColor = SetTextColor(hdc, COLOR_WHITE);
                DrawText(hdc, cchar, 1, &rcChar, DT_VCENTER | DT_HCENTER);
                SetTextColor(hdc, BkColor);
                SetBkMode(hdc, nBkMode);
                pWildCardData->bShow = TRUE;
                ReleaseDC(hWnd, hdc);
            }
            if (pWildCardData->nIndex == -1)
            {
                if (pWildCardData->nWildCard != 0)
                {
                    pch = pWildCardData->cContent;
                    pWildCardData->nIndex = 0;
                    while(pch[0] != 0x3F && pWildCardData->nIndex != pWildCardData->nLen)
                    {
                        pWildCardData->nIndex ++;
                        pch ++ ;
                    }
                }
                else
                {
                    pWildCardData->nIndex = pWildCardData->nLen;
                }
            }
            else if (pWildCardData->nIndex == pWildCardData->nLen)
                break;
            else if (pWildCardData->nIndex < pWildCardData->nLenInit)
            {
                pWildCardData->nIndex ++;
                while(pWildCardData->nIndex != pWildCardData->nLenInit)
                {
                    if (pWildCardData->IsWildCard[pWildCardData->nIndex] == 1)
                        break;
                    pWildCardData->nIndex ++;
                }
            }
            else
            {
                pWildCardData->nIndex ++;
            }
            if (pWildCardData->bTimer && pWildCardData->nIndex >= pWildCardData->nLenInit)
            {
                KillTimer(hWnd, TIMERID);
                pWildCardData->bTimer = FALSE;
                pWildCardData->bShow = TRUE;
            }
            if (!pWildCardData->bShowCaret && pWildCardData->nIndex >= pWildCardData->nLenInit)
            {
                ShowCaret(hWnd);
                pWildCardData->bShowCaret = TRUE;
            }
            if (pWildCardData->bShowCaret)
            {
                GetClientRect(hWnd, &rcClient);
                nCount = rcClient.right / MAXCHARWIDTH;
                
                //ClearRect(hdc, &rcChar, COLOR_WHITE);
                
                x = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                y = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                SetCaretPos(x, y);
            }
            
            break;

        default :
            break;
        }
        break;

    case WM_SIZE :
        
        InvalidateRect(hWnd, NULL, TRUE);			
        break;

    case WM_CHAR:
        if (wParam == VK_RETURN || wParam == VK_F10 || wParam == VK_F1 || wParam == VK_F2)
            break;
        if (pWildCardData->nIndex == -1)
            break;
        if (pWildCardData->nLen == 41)
            break;
        
        if (pWildCardData->nWildCard == pWildCardData->nInitWildCard)
            SendMessage(GetParent(hWnd), WM_SETRBTNTEXT, NULL, (LPARAM)ML("Clear"));
        

        if (pWildCardData->cContent[pWildCardData->nIndex] == 0x3F)
        {
            pWildCardData->cContent[pWildCardData->nIndex] = (char)wParam;
            pWildCardData->nWildCard --;
            if (!pWildCardData->bShow)
            {
                hdc = GetDC(hWnd);
                GetClientRect(hWnd, &rcClient);
                nCount = rcClient.right / MAXCHARWIDTH;
                rcChar.left = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                rcChar.right = ((pWildCardData->nIndex % nCount) + 1)* MAXCHARWIDTH + rcClient.left + LEFTGAP;
                rcChar.top = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                rcChar.bottom = ((pWildCardData->nIndex / nCount) + 1)* CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                
                cchar[0] = pWildCardData->cContent[pWildCardData->nIndex];
                cchar[1] = '\0';
                nBkMode = SetBkMode(hdc, BM_TRANSPARENT);
                BkColor = SetTextColor(hdc, COLOR_WHITE);
                DrawText(hdc, cchar, 1, &rcChar, DT_VCENTER | DT_HCENTER);
                SetTextColor(hdc, BkColor);
                SetBkMode(hdc, nBkMode);
                pWildCardData->bShow = TRUE;
                ReleaseDC(hWnd, hdc);
            }
            else
                InvalidateRect(hWnd, NULL, TRUE);
            
            if (pWildCardData->nWildCard != 0)
            {
                pch = pWildCardData->cContent + pWildCardData->nIndex;
                while(pch[0] != 0x3F && pWildCardData->nIndex != pWildCardData->nLen)
                {
                    pWildCardData->nIndex ++;
                    pch ++ ;
                }
                if (pWildCardData->nIndex == pWildCardData->nLen)
                {
                    pch = pWildCardData->cContent;
                    pWildCardData->nIndex = 0;
                    while(pch[0] != 0x3F && pWildCardData->nIndex != pWildCardData->nLen)
                    {
                        pWildCardData->nIndex ++;
                        pch ++;
                    }
                }
            }
            else
            {
                if (pWildCardData->bTimer)
                {
                    KillTimer(hWnd, TIMERID);
                    pWildCardData->bTimer = FALSE;
                    pWildCardData->bShow = TRUE;
                }
                pWildCardData->nIndex = pWildCardData->nLen;
                GetClientRect(hWnd, &rcClient);
                nCount = rcClient.right / MAXCHARWIDTH;
                x = (pWildCardData->nLen % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                y = (pWildCardData->nLen / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                SetCaretPos(x, y);
                if (pWildCardData->nWildCard == 0 && pWildCardData->bFocus && pWildCardData->bShowCaret == FALSE)
                {
                    ShowCaret(hWnd);
                    pWildCardData->bShowCaret = TRUE;
                }
            }
        }
        else if (pWildCardData->IsWildCard[pWildCardData->nIndex] == 1)
        {
            pWildCardData->cContent[pWildCardData->nIndex] = (char)wParam;
            GetClientRect(hWnd, &rcClient);
            nCount = rcClient.right / MAXCHARWIDTH;
            rcChar.left = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
            rcChar.right = ((pWildCardData->nIndex % nCount) + 1)* MAXCHARWIDTH + rcClient.left + LEFTGAP;
            rcChar.top = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
            rcChar.bottom = ((pWildCardData->nIndex / nCount) + 1)* CHARHEIGHT + rcClient.top + TITLEHEIGHT;
            
            cchar[0] = pWildCardData->cContent[pWildCardData->nIndex];
            cchar[1] = '\0';
            InvalidateRect(hWnd, &rcChar, TRUE);
            
            pWildCardData->nIndex ++;
            while (pWildCardData->nIndex < pWildCardData->nLenInit)
            {
                if (pWildCardData->IsWildCard[pWildCardData->nIndex] == 1)
                    break;
                pWildCardData->nIndex ++;
            }
            if (pWildCardData->nIndex == pWildCardData->nLenInit)
            {
                if (pWildCardData->bTimer)
                {
                    KillTimer(hWnd, TIMERID);
                    pWildCardData->bTimer = FALSE;
                    pWildCardData->bShow = TRUE;
                }
                if (!pWildCardData->bShowCaret)
                {
                    GetClientRect(hWnd, &rcClient);
                    nCount = rcClient.right / MAXCHARWIDTH;
                    x = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
                    y = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
                    SetCaretPos(x, y);
                    ShowCaret(hWnd);
                    pWildCardData->bShowCaret = TRUE;
                }
            }
        }
        else if(pWildCardData->nIndex < pWildCardData->nLen)
        {
            for(i = pWildCardData->nLen; i > pWildCardData->nIndex; i--)
            {
                pWildCardData->cContent[i] = pWildCardData->cContent[i - 1];
            }
            pWildCardData->cContent[pWildCardData->nIndex] = (char)wParam;
            GetClientRect(hWnd, &rcClient);
            if (pWildCardData->nLen >= pWildCardData->nLenMax - 1)
            {
                MoveWindow(hWnd, 0, 0, rcClient.right - rcClient.left, 
                    rcClient.bottom - rcClient.top + MAXHEIGHT, TRUE);
                pWildCardData->nLenMax += (rcClient.right - rcClient.left)/ MAXCHARWIDTH;
            }

            pWildCardData->nIndex ++;
            pWildCardData->nLen++;
            nCount = rcClient.right / MAXCHARWIDTH;
            x = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
            y = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
            SetCaretPos(x, y);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else
        {
            pWildCardData->cContent[pWildCardData->nIndex] = (char)wParam;
            hdc = GetDC(hWnd);
            GetClientRect(hWnd, &rcClient);
            nCount = rcClient.right / MAXCHARWIDTH;
            rcChar.left = (pWildCardData->nIndex % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
            rcChar.right = ((pWildCardData->nIndex % nCount) + 1)* MAXCHARWIDTH + rcClient.left + LEFTGAP;
            rcChar.top = (pWildCardData->nIndex / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
            rcChar.bottom = ((pWildCardData->nIndex / nCount) + 1)* CHARHEIGHT + rcClient.top + TITLEHEIGHT;
            
            cchar[0] = pWildCardData->cContent[pWildCardData->nIndex];
            cchar[1] = '\0';

            if (pWildCardData->nLen >= pWildCardData->nLenMax - 1)
            {
                MoveWindow(hWnd, 0, 0, rcClient.right - rcClient.left, 
                    rcClient.bottom - rcClient.top + MAXHEIGHT, TRUE);
                pWildCardData->nLenMax += (rcClient.right - rcClient.left)/ MAXCHARWIDTH;
            }
            else
            {
                nBkMode = SetBkMode(hdc, BM_TRANSPARENT);
                BkColor = SetTextColor(hdc, COLOR_WHITE);
                DrawText(hdc, cchar, 1, &rcChar, DT_VCENTER | DT_HCENTER);
                SetTextColor(hdc, BkColor);
                SetBkMode(hdc, nBkMode);
            }

            pWildCardData->nIndex ++;
            pWildCardData->nLen++;

            x = (pWildCardData->nLen % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
            y = (pWildCardData->nLen / nCount) * CHARHEIGHT + rcClient.top + TITLEHEIGHT;
            SetCaretPos(x, y);
            ReleaseDC(hWnd, hdc);
        }
        //InvalidateRect(hWnd, NULL, TRUE);
        SendMessage(GetParent(hWnd), WM_COMMAND, 
                MAKELONG(pWildCardData->wID, EN_CHANGE), (LPARAM)hWnd);
        break;

    case WM_NCDESTROY :
		DestroyCaret();
        break;

    case WM_SETFONT:
        hFont = (HFONT)wParam;
        fRedraw = lParam;
        hdc = GetDC(hWnd);
        SelectObject(hdc, hFont);
        ReleaseDC(hWnd, hdc);

        if (fRedraw && IsWindowVisible(hWnd))
            InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_ERASEBKGND:
        break;

    case WM_PAINT :

        hdc = BeginPaint(hWnd, NULL);
        PaintWildCard(hWnd, hdc, pWildCardData);
        EndPaint(hWnd, NULL);

        break;
	
    case WM_NCHITTEST:

        lResult = (LRESULT)HTTRANSPARENT;

        break;

    case WM_GETDLGCODE:

        lResult = (LRESULT)DLGC_STATIC;
        break;
        
	default :

        lResult = DefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}

static void PaintWildCard(HWND hWnd, HDC hdc, PWILDCARDDATA pWildCardData)
{
    RECT rcClient, rcChar, rcTitle;
    int nCount, i;
    char cchar[2];
    HFONT hFont, hfontOld;
    int nMode;
    COLORREF cr;
    HBITMAP hBmp;
    BITMAP bmp;
    HDC     hMemDC;

    GetClientRect(hWnd, &rcClient);

    hMemDC = CreateMemoryDC(rcClient.right, rcClient.bottom);

    hBmp = LoadImage(NULL, BKBMPNAME, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    GetObject(hBmp, sizeof(BITMAP), (void*)&bmp);
    StretchBlt(hMemDC, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom,
        (HDC)hBmp, 0, 0, bmp.bmWidth, bmp.bmHeight, ROP_SRC);
    
    nCount = rcClient.right / MAXCHARWIDTH;
    
    GetFontHandle(&hFont, 0);
    hfontOld = SelectObject(hMemDC, (HGDIOBJ)hFont);
    
    nMode = SetBkMode(hMemDC, TRANSPARENT);
    cr = SetTextColor(hMemDC, COLOR_WHITE);

    SetRect(&rcTitle, rcClient.left + 10, rcClient.top + 5, rcClient.right, rcClient.top + TITLEHEIGHT);
    
    DrawText(hMemDC, RES_STR_CALLTO, -1, &rcTitle,
        DT_LEFT | DT_VCENTER);

    SelectObject(hMemDC, (HGDIOBJ)hfontOld);

    for(i = 0; i < pWildCardData->nLen; i++)
    {
        rcChar.left = (i % nCount) * MAXCHARWIDTH + rcClient.left + LEFTGAP;
        rcChar.right = ((i % nCount) + 1)* MAXCHARWIDTH + rcClient.left + LEFTGAP;
        rcChar.top = (i / nCount) * CHARHEIGHT + rcTitle.bottom;
        rcChar.bottom = ((i / nCount) + 1)* CHARHEIGHT + rcTitle.bottom;
        if (i == pWildCardData->nIndex)
        {
            if (pWildCardData->bShow)
            {
                cchar[0] = pWildCardData->cContent[i];
                cchar[1] = '\0';
                DrawText(hMemDC, cchar, 1, &rcChar, DT_VCENTER | DT_HCENTER);
            }
            //else
                //ClearRect(hMemDC, &rcChar, COLOR_WHITE);
        }
        else
        {
            cchar[0] = pWildCardData->cContent[i];
            cchar[1] = '\0';
            DrawText(hMemDC, cchar, 1, &rcChar, DT_VCENTER | DT_HCENTER);
        }
    }
    SetTextColor(hMemDC, cr);
    
    SetBkMode(hMemDC, nMode);
    DeleteObject((HGDIOBJ)hBmp);
    BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hMemDC, 0, 0, SRCCOPY);
    DeleteDC(hMemDC);
}

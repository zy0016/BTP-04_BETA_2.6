/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Pass Change
 *
 * Purpose  : Provide a control to select a color.    
 *            
\**************************************************************************/

#include "window.h"
#include "str_public.h"
#include "str_plx.h"
#include "winpda.h"
#include "hpimage.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "plx_pdaex.h"
//#include "hpdebug.h"

#include "pubapp.h"
typedef struct
{
    PSTR    szContent;
    BOOL    bImage;
    PSTR    szImgPath;
    PBYTE   pImgData;
    HWND    hParent;
    HWND    hFrame;
    int     nDataSize;
    int     nTextLen;
} TEXTVIEW , *PTEXTVIEW;

typedef struct
{
    PSTR    szContent;
    int     nImage;
    PSTR    szImgPath;
    PBYTE   pImgData;
    int     nDataSize;
    HWND    hParent;
    HWND    hFrame;
    int     txtlen;
    int     width;
    int     height;
    int     nFind;
    int     offset;
    int     len;
    int     nTopIndex;
    int     nTextStart;
    HBITMAP hBitmap;
    int     nBmpWidth;
    int     nBmpHeight;
    int     nSrcWidth;
    int     nSrcHeight;
    int     nBmpleft;
    BOOL    bTran;
    COLORREF color;
    int     nMaxLines;
    int     nLines;
    unsigned short    *pnLines;
    HGIFANIMATE hGif;
} TEXTVIEWDATA , *PTEXTVIEWDATA;


LRESULT CALLBACK TextView_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static int CalLineNum(HWND hWnd, PTEXTVIEWDATA pTVData);
static	void	fnPaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL bEnd, int x, int y, HDC hdc);
static void TextView_Paint(HWND hWnd, HDC hdc, PTEXTVIEWDATA pTVData);
static void TextView_Create(HWND hWnd, PTEXTVIEWDATA pTVData, LPARAM lParam);


#define LINE_HEIGHT     28
#define IMG_GIF       1
#define IMG_BITMAP    2

//Define data structure for passcheck control


/*********************************************************************\
* Function: PlxTextView
* Purpose:  Create and show TextView interface  
* Params:   hFrame
*           hParent
*           szContent
*           nTextLen
*           bImage
*           szImgPath	   
*           pImgData
*           nDataSize
* Return		   
* Remarks	   
**********************************************************************/

HWND PlxTextView(HWND hFrame, HWND hParent, PSTR szContent, int nTextLen, 
                 BOOL bImage, PSTR szImgPath, PVOID pImgData, int nDataSize)
{
    WNDCLASS wc;
    HWND    hWnd;
    RECT    rcClient;
    TEXTVIEW TextData;
   
    
    wc.style            = CS_OWNDC;//CS_VREDRAW;//CS_PARENTDC;
    wc.lpfnWndProc      = TextView_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(TEXTVIEWDATA);
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;//LoadCursor(NULL, IDC_IBEAM);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "TEXTVIEW";
    wc.hbrBackground    = NULL;//GetStockObject(WHITE_BRUSH);

    RegisterClass(&wc);

    GetClientRect(hParent, &rcClient);
    TextData.bImage = bImage;
    TextData.szContent = szContent;
    TextData.pImgData = pImgData;
    TextData.szImgPath = szImgPath;
    TextData.hParent = hParent;
    TextData.hFrame = hFrame;
    TextData.nDataSize = nDataSize;
    if (nTextLen == -1)
        nTextLen = strlen(szContent);
    TextData.nTextLen = nTextLen;

    hWnd = CreateWindow("TEXTVIEW",NULL,
		       WS_VISIBLE | WS_CHILD | WS_VSCROLL,
		       rcClient.left, rcClient.top, rcClient.right - rcClient.left, 
               rcClient.bottom - rcClient.top,
		       hParent,NULL,NULL,(LPVOID)&TextData);
    return hWnd;
    
}


/*********************************************************************\
* Function: TextView_WndProc
* Purpose:  Deal with messages.    
* Params:   hWnd
*           wMsgCmd
*           wParam
*           lParam	   
* Return		   
* Remarks	   
**********************************************************************/
LRESULT CALLBACK TextView_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    PTEXTVIEWDATA   pTVData;
    HDC             hdc;
    SCROLLINFO      sci;
//    DWORD           dwData;
    LRESULT         lResult = TRUE;
    int             newoffset, newlen;
    int             i;
    BOOL            bRet;

    newoffset = 0;
    newlen = 0;
    pTVData = (PTEXTVIEWDATA)GetUserData(hWnd);            
    switch (wMsgCmd)	
    {
    case WM_CREATE :
        TextView_Create(hWnd, pTVData, lParam);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, NULL);
        TextView_Paint(hWnd, hdc, pTVData);
        EndPaint(hWnd, NULL);
        break;

    case TVM_FINDNHL:
        pTVData->nFind = lParam;
        pTVData->offset = 0;
        pTVData->len = 0;
        bRet = GetCursel(pTVData->szContent, pTVData->txtlen, 0, 0, 
            &newoffset, &newlen, pTVData->nFind, 0);
        if (bRet != -1)
        {
            pTVData->offset = newoffset;
            pTVData->len = newlen;
            
            sci.cbSize = sizeof(SCROLLINFO);
            sci.fMask = SIF_ALL;
            GetScrollInfo(hWnd, SB_VERT, &sci);
            for (i = 0; i < pTVData->nLines; i++)
            {
                if (newoffset < pTVData->pnLines[i])
                    break;
            }
            
            sci.nPos = i + pTVData->nTextStart - 1;
            if (sci.nPos + (int)sci.nPage > sci.nMax)
                sci.nPos = sci.nMax - (int)sci.nPage + 1;
            
            SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
            if (pTVData->hGif)
            {
                EndAnimatedGIF(pTVData->hGif);
                pTVData->hGif = NULL;
            }
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else
        {
            pTVData->nFind = 0;
            return -1;
        }
        break;
        
    case TVM_GETCURHL:
        if (pTVData->nFind == 0)
            return -1;
        *(DWORD*)wParam = ((pTVData->len << 16) | pTVData->offset);
        *(DWORD*)lParam = pTVData->nFind;
        
        break;

    case TVM_HIDEHL:
        pTVData->nFind = 0;
        pTVData->offset = 0;
        pTVData->len = 0;
        pTVData->nTopIndex = 0;
        sci.cbSize = sizeof(SCROLLINFO);
        sci.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &sci);
        sci.nPos = sci.nMin;
        SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
        if (pTVData->hGif)
        {
            EndAnimatedGIF(pTVData->hGif);
            pTVData->hGif = NULL;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case WM_DESTROY:
        if (pTVData->hBitmap)
            DeleteObject((HGDIOBJ)pTVData->hBitmap);
        if (pTVData->pnLines)
            free(pTVData->pnLines);
        if (pTVData->hGif)
            EndAnimatedGIF(pTVData->hGif);
        break;

    case WM_KEYDOWN :
        switch(wParam)
        {
        case VK_UP:
            if (pTVData->nFind == 0)
            {
                sci.cbSize = sizeof(SCROLLINFO);
                sci.fMask = SIF_ALL;
                GetScrollInfo(hWnd, SB_VERT, &sci);
                if (sci.nPos > 0)
                {
                    sci.nPos --;
                    SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
                    if (pTVData->hGif)
                    {
                        EndAnimatedGIF(pTVData->hGif);
                        pTVData->hGif = NULL;
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                else
                    SendMessage(GetParent(hWnd), wMsgCmd, wParam, lParam);
            }
            else
            {
                bRet = GetCursel(pTVData->szContent, pTVData->txtlen, pTVData->offset, 
                    pTVData->len, &newoffset, &newlen, pTVData->nFind, 1);
                if (bRet != -1)
                {
                    pTVData->offset = newoffset;
                    pTVData->len = newlen;
                    
                    sci.cbSize = sizeof(SCROLLINFO);
                    sci.fMask = SIF_ALL;
                    GetScrollInfo(hWnd, SB_VERT, &sci);
                    for (i = 0; i < pTVData->nLines; i++)
                    {
                        if (newoffset < pTVData->pnLines[i])
                            break;
                    }
                    
                    sci.nPos = i + pTVData->nTextStart - 1;
                    if (sci.nPos + (int)sci.nPage > sci.nMax)
                        sci.nPos = sci.nMax - (int)sci.nPage + 1;
                    
                    SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
                    if (pTVData->hGif)
                    {
                        EndAnimatedGIF(pTVData->hGif);
                        pTVData->hGif = NULL;
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }

            break;
        case VK_DOWN:
            if (pTVData->nFind == 0)
            {
                sci.cbSize = sizeof(SCROLLINFO);
                sci.fMask = SIF_ALL;
                GetScrollInfo(hWnd, SB_VERT, &sci);
                if (sci.nPos + (int)sci.nPage <= sci.nMax)
                {
                    sci.nPos ++;
                    SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
                    if (pTVData->hGif)
                    {
                        EndAnimatedGIF(pTVData->hGif);
                        pTVData->hGif = NULL;
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                }
                else
                    SendMessage(GetParent(hWnd), wMsgCmd, wParam, lParam);
                
            }
            else
            {
                bRet = GetCursel(pTVData->szContent, pTVData->txtlen, pTVData->offset, 
                    pTVData->len, &newoffset, &newlen, pTVData->nFind, 0);
                if (bRet != -1)
                {
                    pTVData->offset = newoffset;
                    pTVData->len = newlen;
                    
                    sci.cbSize = sizeof(SCROLLINFO);
                    sci.fMask = SIF_ALL;
                    GetScrollInfo(hWnd, SB_VERT, &sci);
                    for (i = 0; i < pTVData->nLines; i++)
                    {
                        if (newoffset < pTVData->pnLines[i])
                            break;
                    }
                    
                    sci.nPos = i + pTVData->nTextStart - 1;
                    if (sci.nPos + (int)sci.nPage > sci.nMax)
                        sci.nPos = sci.nMax - (int)sci.nPage + 1;
                    
                    SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
                    if (pTVData->hGif)
                    {
                        EndAnimatedGIF(pTVData->hGif);
                        pTVData->hGif = NULL;
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                }
            }
            break;
            
        case VK_RETURN:
        case VK_F10:
        case VK_F2:
        case VK_F5:
        case VK_LEFT:
        case VK_RIGHT:
        case VK_F1:
            SendMessage(pTVData->hParent,wMsgCmd,wParam,lParam);
            break;

        default:
            return lResult;
        }
        break;
	default :

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        break;
	
    };

	return lResult;

}

static int CalLineNum(HWND hWnd, PTEXTVIEWDATA pTVData)
{
    int nPos = 0;
    char* pTxt;
    int nFit = 0, i, nFitTmp = 0, nFitLast = 0;
    int bRet;
    unsigned short *pTmp;

    if (pTVData->szContent == NULL)
    {
        pTVData->nLines = 0;
        return 0;
    }
    pTVData->nMaxLines = 50;
    pTVData->pnLines = (unsigned short*)malloc(pTVData->nMaxLines * 
        sizeof(unsigned short));
    
    pTxt = pTVData->szContent;
    pTVData->nLines = 0;
    pTVData->pnLines[0] = 0;
    do {
        GetTextExtentExPoint(NULL, pTxt, -1, pTVData->width, &nFitTmp, NULL, NULL);
        nFit = nFitLast + nFitTmp;
        bRet = FALSE;
        for(i = 0; i <= nFitTmp; i++)
        {
            if (*(pTxt+i) == '\r' && *(pTxt+i+1) == '\n')
            {
                nFit = nFitLast + i + 2;
                bRet = TRUE;
                break;
            }
            else if (*(pTxt+i) == '\r')
            {
                nFit = nFitLast + i + 1;
                bRet = TRUE;
                break;
            }
            else if (*(pTxt+i) == '\n')
            {
                nFit = nFitLast + i + 1;
                bRet = TRUE;
                break;
            }
        }
        if (!bRet && nFit != pTVData->txtlen)
        {
            for(i = nFitTmp; i >= 0; i--)
            {
                if (*(pTxt+i) == ' ')
                {
                    nFit = nFitLast + i + 1;
                    break;
                }
            }
        }
        pTVData->nLines ++;
        if (pTVData->nLines == pTVData->nMaxLines)
        {
            pTVData->nMaxLines += 50;
            pTmp = pTVData->pnLines;
            pTVData->pnLines = (unsigned short*)malloc(pTVData->nMaxLines * 
                sizeof(unsigned short));
            memcpy(pTVData->pnLines, pTmp, (pTVData->nMaxLines - 50) * 
                sizeof(unsigned short));
            free(pTmp);
        }
        pTVData->pnLines[pTVData->nLines] = nFit;
    	pTxt = pTVData->szContent + nFit;
        nFitLast = nFit;
        
    } while(*pTxt);
    return pTVData->nLines;
}

static	void	fnPaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL bEnd, int x, int y, HDC hdc)
{

	int nHdcWidth,nHdcHeight;
	RECT rect;

	nHdcWidth	=	GetDeviceCaps (hdc, HORZRES);
	nHdcHeight	=	GetDeviceCaps (hdc, VERTRES);

	rect.left = 0;
	rect.top = 0;
	rect.right = nHdcWidth;
	rect.bottom = nHdcHeight;
    ClearRect(hdc, &rect, COLOR_TRANSBK);
	//FillRect ( hdcMem, &rect, hWhiteBrush );
	
	return;
}


static void TextView_Paint(HWND hWnd, HDC hdc, PTEXTVIEWDATA pTVData)
{
    SCROLLINFO      sci;
    COLORREF        OldBkColor, OldTextColor;
    int             nBkModeOld;
    int             i, index;
    SIZE            TxtSize;

    
        sci.cbSize = sizeof(SCROLLINFO);
        sci.fMask = SIF_ALL;
        GetScrollInfo(hWnd, SB_VERT, &sci);
        nBkModeOld = SetBkMode(hdc, BM_TRANSPARENT);
        if (sci.nPos < pTVData->nTextStart)
        {
            if (pTVData->hBitmap)
            {
                if (pTVData->bTran)
                {
                    SetBkMode(hdc, BM_NEWTRANSPARENT);
                    OldBkColor = SetBkColor(hdc, pTVData->color);
                }
                StretchBlt(hdc, pTVData->nBmpleft, 0 - sci.nPos * LINE_HEIGHT,
                    pTVData->nBmpWidth, pTVData->nBmpHeight, (HDC)pTVData->hBitmap, 
                    0, 0, pTVData->nSrcWidth, pTVData->nSrcHeight, SRCCOPY);
                if (pTVData->bTran)
                {
                    SetBkMode(hdc, BM_TRANSPARENT);
                    SetBkColor(hdc, OldBkColor);
                }
            }
            else if (pTVData->nImage == IMG_GIF)
            {
                if (pTVData->hGif)
                    PaintAnimatedGIF(hdc, pTVData->hGif);
                else
                {
                    if (pTVData->pImgData)
                        pTVData->hGif = StartAnimatedGIFFromDataEx(hWnd, pTVData->pImgData,
                        pTVData->nDataSize, pTVData->nBmpleft, 0 - sci.nPos * LINE_HEIGHT, 
                        pTVData->nBmpWidth, pTVData->nBmpHeight, DM_NONE);
                    else if (pTVData->szImgPath)
                        pTVData->hGif = StartAnimatedGIFFromFileEx(hWnd, pTVData->szImgPath, 
                        pTVData->nBmpleft, 0 - sci.nPos * LINE_HEIGHT, pTVData->nBmpWidth, 
                        pTVData->nBmpHeight, DM_NONE);
                    SetPaintBkFunc(pTVData->hGif, (PAINTBKFUNC)fnPaintBkFunc);
                    PaintAnimatedGIF(hdc, pTVData->hGif);

                }
            }
            if (!pTVData->nLines)
                return;
            if (sci.nPos + (int)sci.nPage >= pTVData->nTextStart)
            {
                index = pTVData->nTextStart - sci.nPos;
                if (pTVData->nFind != 0 && pTVData->len)
                {
                    for(i = index; i < (int)sci.nPage; i++)
                    {
                        if (i - index == pTVData->nLines - 1)
                        {
                            if (pTVData->offset + pTVData->len < pTVData->pnLines[i - index])
                                TextOut(hdc, 0, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->pnLines[i - index], -1);
                            else if (pTVData->offset > pTVData->pnLines[i - index])
                            {
                                TextOut(hdc, 0, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->pnLines[i - index], 
                                    pTVData->offset - pTVData->pnLines[i - index]);
                                OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                                SetBkMode(hdc, BM_OPAQUE);
                                GetTextExtentPoint(hdc, 
                                    pTVData->szContent + pTVData->pnLines[i - index], 
                                    pTVData->offset - pTVData->pnLines[i - index], &TxtSize);
                                TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->offset, pTVData->len);
                                SetTextColor(hdc, OldTextColor);
                                SetBkColor(hdc, OldBkColor);
                                SetBkMode(hdc, BM_TRANSPARENT);
                                if (pTVData->offset + pTVData->len != pTVData->txtlen - 1)
                                {
                                    GetTextExtentPoint(hdc, 
                                        pTVData->szContent + pTVData->pnLines[i - index], 
                                        pTVData->offset + pTVData->len - pTVData->pnLines[i - index], 
                                        &TxtSize);
                                    TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                        pTVData->szContent + pTVData->offset
                                        + pTVData->len, -1);
                                }
                            }
                            else
                            {
                                OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                OldBkColor = SetBkColor(hdc, COLOR_BLUE);                                
                                SetBkMode(hdc, BM_OPAQUE);
                                TextOut(hdc, 0, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->pnLines[i - index],
                                    pTVData->offset + pTVData->len - pTVData->pnLines[i - index]);
                                SetTextColor(hdc, OldTextColor);
                                SetBkColor(hdc, OldBkColor);
                                SetBkMode(hdc, BM_TRANSPARENT);
                                if (pTVData->offset + pTVData->len != pTVData->txtlen - 1)
                                {
                                    GetTextExtentPoint(hdc, 
                                        pTVData->szContent + pTVData->pnLines[i - index], 
                                        pTVData->offset + pTVData->len - pTVData->pnLines[i - index], 
                                        &TxtSize);
                                    TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                        pTVData->szContent + pTVData->offset + pTVData->len, -1);
                                }
                            }
                            return;
                        }
                        else
                        {
                            if (pTVData->offset + pTVData->len < pTVData->pnLines[i - index] || 
                                pTVData->offset >= pTVData->pnLines[i - index + 1])
                                TextOut(hdc, 0, i * LINE_HEIGHT, 
                                pTVData->szContent + pTVData->pnLines[i - index], 
                                    pTVData->pnLines[i - index + 1] - pTVData->pnLines[i - index]);
                            else if (pTVData->offset > pTVData->pnLines[i - index])
                            {
                                TextOut(hdc, 0, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->pnLines[i - index],
                                    pTVData->offset - pTVData->pnLines[i - index]);
                                if (pTVData->offset + pTVData->len < 
                                    pTVData->pnLines[i - index + 1] - 1)
                                {
                                    OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                    OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                                    SetBkMode(hdc, BM_OPAQUE);
                                    GetTextExtentPoint(hdc, 
                                        pTVData->szContent + pTVData->pnLines[i - index], 
                                        pTVData->offset - pTVData->pnLines[i - index], &TxtSize);
                                    TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                        pTVData->szContent + pTVData->offset, pTVData->len);
                                    SetTextColor(hdc, OldTextColor);
                                    SetBkColor(hdc, OldBkColor);
                                    SetBkMode(hdc, BM_TRANSPARENT);
                                    GetTextExtentPoint(hdc, 
                                        pTVData->szContent + pTVData->pnLines[i - index], 
                                        pTVData->offset + pTVData->len - pTVData->pnLines[i - index], 
                                        &TxtSize);
                                    TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                        pTVData->szContent + pTVData->offset + pTVData->len, 
                                        pTVData->pnLines[i - index + 1] - pTVData->offset - pTVData->len);
                                }
                                else
                                {
                                    OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                    OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                                    SetBkMode(hdc, BM_OPAQUE);
                                    GetTextExtentPoint(hdc, 
                                        pTVData->szContent + pTVData->pnLines[i - index], 
                                        pTVData->offset - pTVData->pnLines[i - index], &TxtSize);
                                    TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                        pTVData->szContent + pTVData->offset,
                                        pTVData->pnLines[i - index + 1] - pTVData->offset);
                                    SetTextColor(hdc, OldTextColor);
                                    SetBkColor(hdc, OldBkColor);
                                    SetBkMode(hdc, BM_TRANSPARENT);
                                }
                            }
                            else
                            {
                                if (pTVData->offset + pTVData->len < 
                                    pTVData->pnLines[i - index + 1] - 1)
                                {
                                    OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                    OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                                    SetBkMode(hdc, BM_OPAQUE);
                                    TextOut(hdc, 0, i * LINE_HEIGHT,
                                        pTVData->szContent + pTVData->pnLines[i - index],
                                        pTVData->offset + pTVData->len - pTVData->pnLines[i - index]);
                                    SetTextColor(hdc, OldTextColor);
                                    SetBkColor(hdc, OldBkColor);
                                    SetBkMode(hdc, BM_TRANSPARENT);
                                    GetTextExtentPoint(hdc, 
                                        pTVData->szContent + pTVData->pnLines[i - index], 
                                        pTVData->offset + pTVData->len - pTVData->pnLines[i - index], 
                                        &TxtSize);
                                    TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                        pTVData->szContent + pTVData->offset +
                                        pTVData->len, 
                                        pTVData->pnLines[i - index + 1] - pTVData->offset - pTVData->len);
                                }
                                else
                                {
                                    OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                    OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                                    SetBkMode(hdc, BM_OPAQUE);
                                    TextOut(hdc, 0, i * LINE_HEIGHT, 
                                        pTVData->szContent + pTVData->pnLines[i - index],
                                        pTVData->pnLines[i - index + 1] - pTVData->pnLines[i - index]);
                                    SetTextColor(hdc, OldTextColor);
                                    SetBkColor(hdc, OldBkColor);
                                    SetBkMode(hdc, BM_TRANSPARENT);
                                }
                            }
                        }
                    }
                }
                else
                {
                    for(i = index; i < (int)sci.nPage; i++)
                    {
                        if (i - index == pTVData->nLines - 1)
                        {
                            TextOut(hdc, 0, i * LINE_HEIGHT, 
                                pTVData->szContent + pTVData->pnLines[i - index], -1);
                            return;
                        }
                        else
                        {
                            TextOut(hdc, 0, i * LINE_HEIGHT, 
                                pTVData->szContent + pTVData->pnLines[i - index], 
                            pTVData->pnLines[i - index + 1] - pTVData->pnLines[i - index]);
                        }
                    }
                }
            }
        }
        else
        {
            if (pTVData->hGif)
            {
                EndAnimatedGIF(pTVData->hGif);
                pTVData->hGif = NULL;
            }
            if (pTVData->nLines == 0)
                return;
            if (pTVData->nFind != 0 && pTVData->len)
            {
                for(i = 0; i < (int)sci.nPage; i++)
                {
                    if (i + sci.nPos - pTVData->nTextStart + 1 == pTVData->nLines)
                    {
                        if (pTVData->offset + pTVData->len < 
                            pTVData->pnLines[i + sci.nPos - pTVData->nTextStart])
                            TextOut(hdc, 0, i * LINE_HEIGHT, 
                            pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], -1);
                        else if (pTVData->offset > 
                            pTVData->pnLines[i + sci.nPos - pTVData->nTextStart])
                        {
                            TextOut(hdc, 0, i * LINE_HEIGHT, 
                                pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                pTVData->offset - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart]);
                            OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                            OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                            SetBkMode(hdc, BM_OPAQUE);
                            GetTextExtentPoint(hdc,
                                pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                pTVData->offset - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                &TxtSize);
                            TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                pTVData->szContent + pTVData->offset, pTVData->len);
                            SetTextColor(hdc, OldTextColor);
                            SetBkColor(hdc, OldBkColor);
                            SetBkMode(hdc, BM_TRANSPARENT);
                            if (pTVData->offset + pTVData->len != pTVData->txtlen - 1)
                            {
                                GetTextExtentPoint(hdc, 
                                    pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    pTVData->offset + pTVData->len - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    &TxtSize);
                                TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->offset + pTVData->len, -1);
                            }
                        }
                        else
                        {
                            OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                            OldBkColor = SetBkColor(hdc, COLOR_BLUE);                                
                            SetBkMode(hdc, BM_OPAQUE);
                            TextOut(hdc, 0, i * LINE_HEIGHT, 
                                pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart],
                                pTVData->offset + pTVData->len - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart]);
                            SetTextColor(hdc, OldTextColor);
                            SetBkColor(hdc, OldBkColor);
                            SetBkMode(hdc, BM_TRANSPARENT);
                            if (pTVData->offset + pTVData->len != pTVData->txtlen - 1)
                            {
                                GetTextExtentPoint(hdc, 
                                    pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    pTVData->offset + pTVData->len - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    &TxtSize);
                                TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->offset + pTVData->len, -1);
                            }
                        }
                        return;
                    }     
                    else
                    {
                        if (pTVData->offset + pTVData->len < pTVData->pnLines[i + sci.nPos - pTVData->nTextStart] || 
                            pTVData->offset >= pTVData->pnLines[i + sci.nPos - pTVData->nTextStart + 1])
                            TextOut(hdc, 0, i * LINE_HEIGHT, 
                                pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart],
                                pTVData->pnLines[i + sci.nPos + 1 - pTVData->nTextStart] - 
                                pTVData->pnLines[i + sci.nPos - pTVData->nTextStart]);
                        else if (pTVData->offset > pTVData->pnLines[i + sci.nPos - pTVData->nTextStart])
                        {
                            TextOut(hdc, 0, i * LINE_HEIGHT, 
                                pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart],
                                pTVData->offset - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart]);
                            if (pTVData->offset + pTVData->len < 
                                pTVData->pnLines[i + sci.nPos - pTVData->nTextStart + 1])
                            {
                                OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                                SetBkMode(hdc, BM_OPAQUE);
                                GetTextExtentPoint(hdc, 
                                    pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    pTVData->offset - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    &TxtSize);
                                TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->offset, pTVData->len);
                                SetTextColor(hdc, OldTextColor);
                                SetBkColor(hdc, OldBkColor);
                                SetBkMode(hdc, BM_TRANSPARENT);
                                GetTextExtentPoint(hdc, 
                                    pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    pTVData->offset + pTVData->len - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    &TxtSize);
                                TextOut(hdc, TxtSize.cx, 
                                    i * LINE_HEIGHT, pTVData->szContent + pTVData->offset + pTVData->len, 
                                    pTVData->pnLines[i + sci.nPos - pTVData->nTextStart + 1] - 
                                    pTVData->offset - pTVData->len);
                            }
                            else
                            {
                                OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                                SetBkMode(hdc, BM_OPAQUE);
                                GetTextExtentPoint(hdc, 
                                    pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    pTVData->offset - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], &TxtSize);
                                TextOut(hdc, TxtSize.cx, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->offset,
                                    pTVData->pnLines[i + sci.nPos - pTVData->nTextStart + 1] - pTVData->offset);
                                SetTextColor(hdc, OldTextColor);
                                SetBkColor(hdc, OldBkColor);
                                SetBkMode(hdc, BM_TRANSPARENT);
                            }
                        }
                        else
                        {
                            if (pTVData->offset + pTVData->len < 
                                pTVData->pnLines[i + sci.nPos - pTVData->nTextStart + 1] - 1)
                            {
                                OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                                SetBkMode(hdc, BM_OPAQUE);
                                TextOut(hdc, 0, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart],
                                    pTVData->offset + pTVData->len - 
                                    pTVData->pnLines[i + sci.nPos - pTVData->nTextStart]);
                                SetTextColor(hdc, OldTextColor);
                                SetBkColor(hdc, OldBkColor);
                                SetBkMode(hdc, BM_TRANSPARENT);
                                GetTextExtentPoint(hdc, 
                                    pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    pTVData->offset + pTVData->len - pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                                    &TxtSize);
                                TextOut(hdc, TxtSize.cx, 
                                    i * LINE_HEIGHT, pTVData->szContent + pTVData->offset + pTVData->len, 
                                    pTVData->pnLines[i + sci.nPos - pTVData->nTextStart + 1] - pTVData->offset - pTVData->len);
                            }
                            else
                            {
                                OldTextColor = SetTextColor(hdc, COLOR_WHITE);
                                OldBkColor = SetBkColor(hdc, COLOR_BLUE);
                                SetBkMode(hdc, BM_OPAQUE);
                                TextOut(hdc, 0, i * LINE_HEIGHT, 
                                    pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart],
                                    pTVData->pnLines[i + sci.nPos - pTVData->nTextStart + 1] -
                                    pTVData->pnLines[i + sci.nPos - pTVData->nTextStart]);
                                SetTextColor(hdc, OldTextColor);
                                SetBkColor(hdc, OldBkColor);
                                SetBkMode(hdc, BM_TRANSPARENT);
                            }
                        }
                    }
                }
            }
            else
            {
                for(i = 0; i < (int)sci.nPage; i++)
                {
                    if (i + sci.nPos - pTVData->nTextStart == pTVData->nLines - 1)
                    {
                        TextOut(hdc, 0, i * LINE_HEIGHT, 
                        pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], -1);
                        return;
                    }
                    else
                        TextOut(hdc, 0, i * LINE_HEIGHT, 
                        pTVData->szContent + pTVData->pnLines[i + sci.nPos - pTVData->nTextStart], 
                        pTVData->pnLines[i + sci.nPos - pTVData->nTextStart + 1] - 
                        pTVData->pnLines[i + sci.nPos - pTVData->nTextStart]);
                }
                
            }
        }
        
        SetBkMode(hdc, nBkModeOld);
}


static void TextView_Create(HWND hWnd, PTEXTVIEWDATA pTVData, LPARAM lParam)
{
    PCREATESTRUCT   pCreateData;
    PTEXTVIEW       pTextView;
    HDC             hdc;
    SIZE            size;
    FILE*           nFile;
    char            pbuf[8];
    SCROLLINFO      sci;

    pCreateData = (PCREATESTRUCT)lParam;
    pTextView = (PTEXTVIEW)pCreateData->lpCreateParams;
    memset(pTVData, 0, sizeof(TEXTVIEWDATA));
    pTVData->szContent = pTextView->szContent;
    pTVData->pImgData = pTextView->pImgData;
    pTVData->szImgPath = pTextView->szImgPath;
    pTVData->nDataSize = pTextView->nDataSize;
    pTVData->txtlen = pTextView->nTextLen;
    pTVData->hFrame = pTextView->hFrame;
    pTVData->hParent = pTextView->hParent;
    pTVData->width = pCreateData->cx - 7;
    pTVData->height = pCreateData->cy;
    if (pTextView->bImage)
    {
        size.cx = 0;
        size.cy = 0;
        if (pTVData->pImgData)
        {
            GetImageDimensionFromData(pTVData->pImgData, pTVData->nDataSize, &size);
            if ((pTVData->pImgData[0] == 'G') && (pTVData->pImgData[1] == 'I') && 
                (pTVData->pImgData[2] == 'F'))
                pTVData->nImage = IMG_GIF;
            else
            {
                pTVData->nImage = IMG_BITMAP;
                hdc = GetDC(hWnd);
                pTVData->hBitmap = CreateBitmapFromImageData(hdc, pTVData->pImgData, pTVData->nDataSize,
                    &pTVData->color, &pTVData->bTran);
                ReleaseDC(hWnd, hdc);
                if (!pTVData->hBitmap)
                {
                    pTVData->nImage = 0;
                    CalLineNum(hWnd, pTVData);
                    sci.cbSize = sizeof(SCROLLINFO);
                    sci.fMask = SIF_ALL;
                    sci.nPage = pTVData->height/LINE_HEIGHT;
                    sci.nMin = 0;
                    sci.nMax = pTVData->nTextStart + pTVData->nLines - 1;
                    sci.nPos = 0;
                    sci.nTrackPos = 0;
                    SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
                    return;
                }
            }
        }
        else if (pTVData->szImgPath)
        {
            GetImageDimensionFromFile(pTVData->szImgPath, &size);
            nFile = fopen(pTVData->szImgPath, "r");
            if (nFile == NULL)
            {
                pTVData->nImage = 0;
                CalLineNum(hWnd, pTVData);
                sci.cbSize = sizeof(SCROLLINFO);
                sci.fMask = SIF_ALL;
                sci.nPage = pTVData->height/LINE_HEIGHT;
                sci.nMin = 0;
                sci.nMax = pTVData->nTextStart + pTVData->nLines - 1;
                sci.nPos = 0;
                sci.nTrackPos = 0;
                SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
                return;
            }    
            
            
            fread(pbuf, 6, 1, nFile);
            if ((pbuf[0] == 'G') && (pbuf[1] == 'I') && (pbuf[2] == 'F'))
                pTVData->nImage = IMG_GIF;
            else
            {
                pTVData->nImage = IMG_BITMAP;
                hdc = GetDC(hWnd);
                pTVData->hBitmap = CreateBitmapFromImageFile(hdc, pTVData->szImgPath,
                    &pTVData->color, &pTVData->bTran);
                ReleaseDC(hWnd, hdc);
                if (!pTVData->hBitmap)
                {
                    fclose(nFile);
                    pTVData->nImage = 0;
                    CalLineNum(hWnd, pTVData);
                    sci.cbSize = sizeof(SCROLLINFO);
                    sci.fMask = SIF_ALL;
                    sci.nPage = pTVData->height/LINE_HEIGHT;
                    sci.nMin = 0;
                    sci.nMax = pTVData->nTextStart + pTVData->nLines - 1;
                    sci.nPos = 0;
                    sci.nTrackPos = 0;
                    SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
                    return;
                }
            }
            fclose(nFile);
        }
        pTVData->nSrcHeight = size.cy;
        pTVData->nSrcWidth = size.cx;
        if (size.cx == 0 || size.cy == 0)
        {
            CalLineNum(hWnd, pTVData);
            sci.cbSize = sizeof(SCROLLINFO);
            sci.fMask = SIF_ALL;
            sci.nPage = pTVData->height/LINE_HEIGHT;
            sci.nMin = 0;
            sci.nMax = pTVData->nTextStart + pTVData->nLines - 1;
            sci.nPos = 0;
            sci.nTrackPos = 0;
            SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
            return;
        }
        if (size.cx >= pTVData->width && size.cy >= pTVData->height)
        {
            if ((size.cx / pTVData->width) > (size.cy >= pTVData->height))
            {
                pTVData->nBmpleft = 0;
                pTVData->nBmpWidth = pTVData->width;
                pTVData->nBmpHeight = (size.cy * pTVData->width) / size.cx;
            }
            else
            {
                pTVData->nBmpHeight = pTVData->height;
                pTVData->nBmpWidth = (size.cx * pTVData->height) / size.cy;
                pTVData->nBmpleft = (pTVData->width - pTVData->nBmpWidth) / 2;
            }
        }
        else if (size.cx >= pTVData->width)
        {
            pTVData->nBmpleft = 0;
            pTVData->nBmpWidth = pTVData->width;
            pTVData->nBmpHeight = (size.cy * pTVData->width) / size.cx;
        }
        else if ( size.cy >= pTVData->height)
        {
            pTVData->nBmpHeight = pTVData->height;
            pTVData->nBmpWidth = (size.cx * pTVData->height) / size.cy;
            pTVData->nBmpleft = (pTVData->width - pTVData->nBmpWidth) / 2;
            
        }
        else
        {
            pTVData->nBmpleft = (pTVData->width - size.cx) / 2;
            pTVData->nBmpWidth = size.cx;
            pTVData->nBmpHeight = size.cy;
        }
        pTVData->nTextStart = pTVData->nBmpHeight/LINE_HEIGHT + 1;
    }
    CalLineNum(hWnd, pTVData);
    sci.cbSize = sizeof(SCROLLINFO);
    sci.fMask = SIF_ALL;
    sci.nPage = pTVData->height/LINE_HEIGHT;
    sci.nMin = 0;
    sci.nMax = pTVData->nTextStart + pTVData->nLines - 1;
    sci.nPos = 0;
    sci.nTrackPos = 0;
    SetScrollInfo(hWnd, SB_VERT, &sci, TRUE);
    return;    
}

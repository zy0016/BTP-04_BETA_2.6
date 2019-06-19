/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : app controls model for mobile product
 *
 * Purpose  : implement MenuList control
 *            
\**************************************************************************/

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "menulist.h"
#include "callapp/MBPublic.h"
#include "plx_pdaex.h"
#include "PreBrowHead.h"

#ifndef _MENULIST_
#define _MENULIST_
#endif

//#define  DEFITEMPIC "/rom/public/listfocus.bmp"
#define  DEFITEMPIC	"/usr/local/lib/gui/resources/listfocus.bmp"
#define  ICONBK     "/rom/public/iconbk.gif"
#define  ITEMGAP    4//8
#define  ICONBKGAP  2//4//19
#define  ICONGAP    4//8//23
#define  TEXTGAP    5//10

#define	ID_DOWNFIRSTTIMER	1
#define	ID_UPFIRSTTIMER		2
#define	ID_DOWNREPEATTIMER	3
#define	ID_UPREPEATTIMER	4

static BOOL bHasRegistered = FALSE;
static HGIFANIMATE hDefItemPic = NULL;
static HGIFANIMATE hIconBk = NULL;
static SIZE        IconBkSize;
static int         nInstCount = 0;

BOOL WINAPI GetIconDimension(HICON hIcon, PSIZE pSize);
HICON WINAPI CreateIconFromFile(PCSTR pszIconName, int nWidth,int nHeight);

static LRESULT CALLBACK MENULIST_WndProc(HWND hWnd, UINT uMsgCmd, 
                                         WPARAM wParam, LPARAM lParam);

/*********************************************************************\
* Function  : MENULIST_RegisterClass
* Purpose   :
*     Register MenuList window class.
* Params    :
*     None
* Return    :
*     If success, return TRUE, otherwise return FALSE.
* Remarks
**********************************************************************/
BOOL MENULIST_RegisterClass(void)
{
    WNDCLASS wc;

    if (bHasRegistered)
        return TRUE;
//	printf("call MENULIST_RegisterClass\n");
    wc.style = 0;
    wc.hInstance = NULL;
    wc.lpszClassName = ML_CLASSNAME;
    wc.lpfnWndProc = MENULIST_WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = sizeof(MENULISTDATA);

    wc.hIcon = NULL;
    wc.hCursor = (HCURSOR)LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName = NULL;

    if (!RegisterClass(&wc))
        return FALSE;
 
    bHasRegistered = TRUE;

    return TRUE;
}

/*********************************************************************\
* Function  : MENULIST_PaintBg
* Purpose   :
*     Paint the background of the control.
* Params    :
*     hdc           : dc handle of the painted control
*     pMenuListData : pointer to the private data
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_PaintBg(HDC hdc, PMENULISTDATA pMenuListData)
{
    HPEN hPen, hPenOld;
    HBRUSH hBkBrush;
    
    hPen = CreatePen(PS_SOLID, 1, RGB(255, 188, 128));
    hBkBrush = CreateSolidBrush(pMenuListData->BkColor);
    
    hPenOld = SelectObject(hdc, hPen);

    if(pMenuListData->hGifBg)//有背景图
    {
        DrawImage(hdc, 0, 0, 
            pMenuListData->nCliWidth, 
            pMenuListData->nCliHeight,
            (HIMAGE)pMenuListData->hGifBg, 
            0, 0, ROP_SRC);
    }
    
    SelectObject(hdc, hPenOld);
    DeleteObject(hPen);
    DeleteObject(hBkBrush);
}

/*********************************************************************\
* Function  : MENULIST_DrawItem
* Purpose   :
*     Draw MenuList item.
* Params    :
*     hdc           : dc handle of the painted control
*     pMenuListData : pointer to the private data
*     pNode         : pointer to the item node
*     nTopLeftX     : x position of the top item from topleft corner
*     pMenuListData : y position of the top item from topleft corner
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_DrawItem(HDC hdc, PMENULISTDATA pMenuListData, 
                              PMLINODE pNode, int nTopLeftX, int nTopLeftY)
{
    int  x, y;
    SIZE size;
    RECT rcText;
//	RECT rcNum;
    int  nBkMode,BkColor;
    UINT uFormat = 0;	
    COLORREF crColor;
    HBRUSH   hBkBrush, hOldBrush;
    HPEN     hBkPen, hOldPen;
	char	token[40] = "";
	char	szTextDisplay[40] = "";
	char	*numstr = NULL;
	BITMAP	bmp;
//	char		tmp[255];
//try to change font size
	HFONT hFont;
	HFONT hOldFont;
//	LOGFONT lf;
//	int		OldStyle;

    //pic item
    if(pMenuListData->hGifItemF && pNode->dwFlags & MLI_SELECTED)
    {
		DrawImage(hdc, nTopLeftX, nTopLeftY, 
            pMenuListData->nItemWidth+2, pMenuListData->nItemHeight,
            (HIMAGE)pMenuListData->hGifItemF, 0, 0, ROP_SRC);
		//		DrawImageFromFile(hdc,DEFITEMPIC,nTopLeftX,nTopLeftY,ROP_SRC);
    }
    else if(!(pNode->dwFlags & MLI_SELECTED))
    {
        hBkBrush = CreateSolidBrush(pMenuListData->BkColor);
        hBkPen   = CreatePen(PS_NULL, 0, 0);

        hOldBrush = (HBRUSH)SelectObject(hdc, hBkBrush);
        hOldPen   = (HPEN)SelectObject(hdc, hBkPen);

        Rectangle(hdc, nTopLeftX, nTopLeftY, 
            nTopLeftX + pMenuListData->nCliWidth,
            nTopLeftY + pMenuListData->nItemHeight);

        SelectObject(hdc, hOldBrush);
        SelectObject(hdc, hOldPen);
        DeleteObject(hBkBrush);
        DeleteObject(hBkPen);
    }

    if (pNode->hIcon)
    {
        //计算icon大小
		if (pNode->nFileType == PREBROW_FILETYPE_ICON) 
		{
			GetIconDimension((HICON)pNode->hIcon, &size);
			size.cy = size.cy / 2;
			
			x = ICONGAP;
			y = nTopLeftY + (pMenuListData->nItemHeight - size.cy) / 2;
			/*
			if(hIconBk != NULL)
			{
				DrawImage(hdc, ICONBKGAP, 
					nTopLeftY+2, IconBkSize.cx, IconBkSize.cy,
					(HIMAGE)hIconBk, 0, 0, ROP_SRC);
			}
			*/
			DrawIcon(hdc, x, y, (HICON)pNode->hIcon);
		}
		if (pNode->nFileType == PREBROW_FILETYPE_BMP) 
		{
			//GetImageDimension(pNode->hIcon, &size);
			GetObject(pNode->hIcon, sizeof(BITMAP), (void *)&bmp);
			size.cx = bmp.bmWidth;
			size.cy = bmp.bmHeight;
			x = ICONGAP;
			y = nTopLeftY + (pMenuListData->nItemHeight - size.cy) / 2;
			/*
			if(hIconBk != NULL)
			{
				DrawImage(hdc, ICONBKGAP, 
					nTopLeftY+2, IconBkSize.cx, IconBkSize.cy,
					(HIMAGE)hIconBk, 0, 0, ROP_SRC);
			}
			*/
			//DrawBitmapFromFile(hdc, x, y, pNode->hIcon);
			BkColor = SetBkColor(hdc, COLOR_BLACK);
			nBkMode = SetBkMode(hdc, BM_NEWTRANSPARENT);
			BitBlt(hdc, x, y, size.cx, size.cy,	(HDC)pNode->hIcon, 0, 0, ROP_SRC);
			SetBkMode(hdc, nBkMode);
			SetBkColor(hdc, BkColor);
		}
        
    }
    else if(pNode->hIcon == NULL)
    {
        x = 1;
    }
  
    if (!pNode->pNodeString)
        return;

    // Draw text string.
    if ((pNode->dwFlags & MLI_HMASK) == MLI_LEFT)
        uFormat |= DT_LEFT;
    else if ((pNode->dwFlags & MLI_HMASK) == MLI_CENTER)
        uFormat |= DT_CENTER;
    else
        uFormat |= DT_RIGHT;
    uFormat |= DT_VCENTER;
	
    rcText.left = ICONBKGAP + IconBkSize.cx + TEXTGAP;
    rcText.top = nTopLeftY + 1;
    rcText.right = pMenuListData->nCliWidth - 8;
    rcText.bottom = nTopLeftY + pMenuListData->nItemHeight - 2;
	
	nBkMode = SetBkMode(hdc, TRANSPARENT);
	if (pNode->dwFlags & MLI_SELECTED) {
		crColor = SetTextColor(hdc, COLOR_WHITE);
	}
	else
		crColor = SetTextColor(hdc, RGB(1, 31, 65));
	//todo:should I  add code here to implenment function? 
	//strcpy(pNode->pNodeString,"lintmx-t123");
	if((numstr=strstr(pNode->pNodeString,"-t"))==NULL)
	{
		GetExtentFittedText(hdc, pNode->pNodeString, strlen(pNode->pNodeString),
							szTextDisplay, 32, rcText.right - rcText.left, '.', 3);
		DrawText(hdc, szTextDisplay, strlen(szTextDisplay), &rcText, uFormat);
	}
	else
	{

		memset(token,0,sizeof(token));

//		token = pNode->pNodeString;
//		*(token+(numstr-2-token)) ='\0';
		strncpy(token,pNode->pNodeString,numstr - pNode->pNodeString);

		GetExtentFittedText(hdc, token, strlen(token),
							szTextDisplay, 32, rcText.right - rcText.left - 15, '.', 3);
		
		DrawText(hdc, szTextDisplay, strlen(szTextDisplay), &rcText, uFormat);

		GetFontHandle(&hFont,0);
		hOldFont = SelectObject(hdc, hFont);
		numstr+=2;
		DrawText(hdc, numstr, strlen(numstr), &rcText, DT_RIGHT|DT_VCENTER);

		SelectObject(hdc, hOldFont);
		//------------------------------------------------------
	}
    SetBkMode(hdc, nBkMode);
    SetTextColor(hdc, crColor);

    if(pMenuListData->hGifItemF || pMenuListData->hGifItemNF)
        return;
    else if (pNode->dwFlags & MLI_SELECTED)
    {
        HPEN hPen;

        rcText.left = 0;

        hPen = (HPEN)SelectObject(hdc, GetStockObject(GRAY_PEN));
        DrawLine(hdc, pMenuListData->nTopLeftX + 2, rcText.top, 
            pMenuListData->nTopLeftX + 2, rcText.bottom);
        DrawLine(hdc, pMenuListData->nTopLeftX + 3, rcText.top, 
            rcText.right - 1, rcText.top);

        SelectObject(hdc, GetStockObject(LTGRAY_PEN));
        DrawLine(hdc, pMenuListData->nTopLeftX + 3, rcText.top + 1, 
            pMenuListData->nTopLeftX + 3, rcText.bottom - 1);
        DrawLine(hdc, pMenuListData->nTopLeftX + 4, rcText.top + 1, 
            rcText.right - 2, rcText.top + 1);

        SelectObject(hdc, GetStockObject(GRAY_PEN));
        DrawLine(hdc, rcText.right - 2, rcText.top + 1, 
            rcText.right - 2, rcText.bottom - 1);
        DrawLine(hdc, pMenuListData->nTopLeftX + 3, rcText.bottom - 1, 
            rcText.right - 1, rcText.bottom - 1);

        SelectObject(hdc, GetStockObject(BLACK_PEN));
        DrawLine(hdc, rcText.right - 1, rcText.top, 
            rcText.right - 1, rcText.bottom);
        DrawLine(hdc, pMenuListData->nTopLeftX + 2, rcText.bottom, 
            rcText.right, rcText.bottom);
        SelectObject(hdc, hPen);
    }			   
}

/*********************************************************************\
* Function  : MENULIST_OnPaint
* Purpose   :
*     Paint MenuList.
* Params    :
*     hWnd          : window handle of the painted control
*     hdc           : dc handle of the painted control
*     pMenuListData : pointer to the private data
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_OnPaint(HWND hWnd, HDC hdc, 
                   PMENULISTDATA pMenuListData, RECT* prcInvalid)
{
    int count;
    int nTopLeftX;
    int nTopLeftY;
    RECT rect;
    PMLINODE pNode;

    // paint background.
    MENULIST_PaintBg(hdc, pMenuListData);

    if (pMenuListData->wTotalItemCount == 0 || !pMenuListData->pTopNode)
        return;

    count = pMenuListData->wPageItemCount;
    if (count == 0)
        return;

    // paint each item within the curren page.
    pNode = pMenuListData->pTopNode;
    nTopLeftX = pMenuListData->nTopLeftX;
    nTopLeftY = pMenuListData->nTopLeftY;
    while (pNode != NULL && (count-- != 0))
    {
        SetRect(&rect, nTopLeftX, nTopLeftY, 
            nTopLeftX + pMenuListData->nItemWidth, 
            nTopLeftY + pMenuListData->nItemHeight);
        if(IntersectRect(&rect, &rect, prcInvalid) == TRUE)
            MENULIST_DrawItem(hdc, pMenuListData, 
                        pNode, nTopLeftX, nTopLeftY);

        pNode = pNode->pNext;
        nTopLeftY += pMenuListData->nItemHeight;	
    }

}

/*********************************************************************\
* Function  : MENULIST_InvalidateItems
* Purpose   :
*     Invalidate consecutive MenuList items.
* Params    :
*     pMenuListData : pointer to the private data
*     pNode         : pointer the the first item to invalidate
*     nInvalid      : item count to invalidate
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_InvalidateItems(PMENULISTDATA pMenuListData, 
                                PMLINODE pNode, int nInvalid)
{
    RECT rcInvalid;
    PMLINODE pTopNode;
    int nTopLeftX;
    int nTopLeftY;

    pTopNode = pMenuListData->pTopNode;
    nTopLeftX = pMenuListData->nTopLeftX;
    nTopLeftY = pMenuListData->nTopLeftY;

    rcInvalid.left = nTopLeftX;
    rcInvalid.top = nTopLeftY + pMenuListData->nItemHeight * 
        (pNode->wSerial - pTopNode->wSerial);
    rcInvalid.right = nTopLeftX + pMenuListData->nCliWidth; //nItemWidth;
    rcInvalid.bottom = rcInvalid.top + pMenuListData->nItemHeight * 
        nInvalid;

    InvalidateRect(pMenuListData->hWnd, &rcInvalid, TRUE);
}

/*********************************************************************\
* Function  : MENULIST_HandleKeyDown
* Purpose   :
*     Handle key events.
* Params    :
*     pMenuListData : pointer to the private data
*     wParam        : word parameter
*     lParam        : long parameter
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_HandleKeyDown(PMENULISTDATA pMenuListData, 
                                   WPARAM wParam, LPARAM lParam)
{
    PMLINODE pTopNode;
    PMLINODE pCurNode;
    PMLINODE pInvalidNode;
    MLINODE* pNode;
    int nInvalid;
    int number;
    int nItemCount;
    HWND hwndParent;
    SCROLLINFO ScrollInfo;

    hwndParent = GetParent(pMenuListData->hWnd);

    pTopNode = pMenuListData->pTopNode;
    pCurNode = pMenuListData->pCurNode;
    if (!pTopNode || !pCurNode)
        return;

    number = pCurNode->wSerial - pTopNode->wSerial + 1;
    switch (wParam)
    {
    case VK_DOWN:
        if (pCurNode->pNext)
        {
            // Scroll page down
            if ((number >= pMenuListData->wPageItemCount -1) && (pCurNode->pNext != pMenuListData->pTail))
            {
				pMenuListData->pTopNode = pMenuListData->pTopNode->pNext;
			
                // wSerial is zero based.

/*@**#---2005-07-20 17:46:40 (mxlin)---#**@

                number = pMenuListData->wTotalItemCount - 
                    pCurNode->pNext->wSerial;
                if (number >= pMenuListData->wPageItemCount)
                {
                    // Items left is more than one page.
                    pMenuListData->pTopNode = pCurNode->pNext;
                }
                else
                {
                    // Items left is less than one page.
                    pMenuListData->pTopNode = pCurNode->pNext;
                    number = pMenuListData->wPageItemCount - number;
                    while (number-- != 0)
                    {
                        pMenuListData->pTopNode = 
                            pMenuListData->pTopNode->pPrev;
                    }
                }
*/
                pInvalidNode = pMenuListData->pTopNode;
                nInvalid = pMenuListData->wPageItemCount;
/*@**#---2005-07-20 18:17:28 (mxlin)---#**@

                if(pMenuListData->bVScroll)
                {
                    ScrollInfo.fMask = SIF_POS;
                    ScrollInfo.nPos = pMenuListData->pTopNode->wSerial;
                    SetScrollInfo(pMenuListData->hWnd, 
                        SB_VERT, &ScrollInfo, TRUE);
                }*/
            }
            else
            {
                pInvalidNode = pCurNode;
                nInvalid = 2;
            }
			if(pMenuListData->bVScroll)
            {
                ScrollInfo.fMask = SIF_POS;
                ScrollInfo.nPos = pMenuListData->pTopNode->wSerial;
                SetScrollInfo(pMenuListData->hWnd, 
                    SB_VERT, &ScrollInfo, TRUE);
            }

            pCurNode->dwFlags &= ~MLI_SELECTED;
            pCurNode->pNext->dwFlags |= MLI_SELECTED;
            pMenuListData->pCurNode = pCurNode->pNext;

            if (!(pMenuListData->pCurNode->pNext) || 
                !(pMenuListData->pCurNode->pPrev->pPrev))   // draw arrow
            {
                MENULIST_InvalidateItems(pMenuListData, 
                    pInvalidNode, nInvalid);
            }
            else
            MENULIST_InvalidateItems(pMenuListData, pInvalidNode, nInvalid);

            if ((pMenuListData->pCurNode->dwFlags & MLI_NOTIFY) && 
                hwndParent)
            {
                PostMessage(hwndParent, WM_COMMAND, 
                    MAKEWPARAM(pMenuListData->pCurNode->wID, 
                    ML_FOCUSNOTIFY),
                    (LPARAM)pMenuListData->hWnd);
            }
        }
        else //最后一项继续向下，循环指回第一项
        {
            pCurNode->dwFlags &= ~MLI_SELECTED;
            pMenuListData->pCurNode = pMenuListData->pHead;
            pMenuListData->pCurNode->dwFlags |= MLI_SELECTED;
            pMenuListData->pTopNode = pMenuListData->pCurNode;

            nInvalid = min(pMenuListData->wPageItemCount, 
                pCurNode->wSerial-pMenuListData->pTopNode->wSerial+1);
            MENULIST_InvalidateItems(pMenuListData, 
                pMenuListData->pTopNode, nInvalid);

            if ((pMenuListData->pCurNode->dwFlags & MLI_NOTIFY) && 
                hwndParent)
            {
                PostMessage(hwndParent, WM_COMMAND, 
                    MAKEWPARAM(pMenuListData->pCurNode->wID, 
                    ML_FOCUSNOTIFY),
                    (LPARAM)pMenuListData->hWnd);
            }
            if(pMenuListData->bVScroll)
            {
                ScrollInfo.fMask = SIF_POS;
                ScrollInfo.nPos = pMenuListData->pTopNode->wSerial;
                SetScrollInfo(pMenuListData->hWnd, 
                    SB_VERT, &ScrollInfo, TRUE);
            }
        }
		SetTimer(pMenuListData->hWnd, ID_DOWNFIRSTTIMER, 300, NULL);

        break;

    case VK_UP:
        if (pCurNode->pPrev)
        {
            // Scroll page up
            if (number <= 2&&pCurNode->pPrev!=pMenuListData->pHead)
            {
				pMenuListData->pTopNode = pMenuListData->pTopNode->pPrev;
                // wSerial is zero based.
/*@**#---2005-07-20 18:12:34 (mxlin)---#**@

                number = pCurNode->pPrev->wSerial + 1;
                if (number >= pMenuListData->wPageItemCount)
                {
                    // Items to scroll is more than one page.
                    number = pMenuListData->wPageItemCount;
                    pMenuListData->pTopNode = pCurNode->pPrev;
                    while (--number != 0)
                    {
                        pMenuListData->pTopNode = 
                            pMenuListData->pTopNode->pPrev;
                    }
                }
                else
                {
                    // Items to scroll is less than one page.
                    pMenuListData->pTopNode = pMenuListData->pHead;
                }
*/
                pInvalidNode = pMenuListData->pTopNode;
                nInvalid = pMenuListData->wPageItemCount;
/*@**#---2005-07-20 18:17:19 (mxlin)---#**@

                if(pMenuListData->bVScroll)
                {
                    ScrollInfo.fMask = SIF_POS;
                    ScrollInfo.nPos = pMenuListData->pTopNode->wSerial;
                    SetScrollInfo(pMenuListData->hWnd, 
                        SB_VERT, &ScrollInfo, TRUE);
                }*/
            }
            else
            {
                pInvalidNode = pCurNode->pPrev;
                nInvalid = 2;
            }
			if(pMenuListData->bVScroll)
            {
                ScrollInfo.fMask = SIF_POS;
                ScrollInfo.nPos = pMenuListData->pTopNode->wSerial;
                SetScrollInfo(pMenuListData->hWnd, 
                    SB_VERT, &ScrollInfo, TRUE);
            }

            pCurNode->dwFlags &= ~MLI_SELECTED;
            pCurNode->pPrev->dwFlags |= MLI_SELECTED;
            pMenuListData->pCurNode = pCurNode->pPrev;

            if (!(pMenuListData->pCurNode->pPrev) || 
                !(pMenuListData->pCurNode->pNext->pNext))   // draw arrow
            {
                //InvalidateRect(pMenuListData->hWnd, NULL, TRUE);
                MENULIST_InvalidateItems(pMenuListData, 
                    pInvalidNode, nInvalid);
            }
            else
            MENULIST_InvalidateItems(pMenuListData, pInvalidNode, nInvalid);

            if ((pMenuListData->pCurNode->dwFlags & MLI_NOTIFY) && 
                hwndParent)
            {
                PostMessage(hwndParent, WM_COMMAND, 
                   MAKEWPARAM(pMenuListData->pCurNode->wID, ML_FOCUSNOTIFY),
                   (LPARAM)pMenuListData->hWnd);
                if(pMenuListData->bVScroll)
                {
                    ScrollInfo.fMask = SIF_POS;
                    ScrollInfo.nPos = pMenuListData->pTopNode->wSerial;
                    SetScrollInfo(pMenuListData->hWnd, 
                        SB_VERT, &ScrollInfo, TRUE);
                }
            }
		}
        else //滚动到最后一项
        {
            nInvalid = min(pMenuListData->wPageItemCount, 
                pMenuListData->pTail->wSerial-pCurNode->wSerial+1);
            nItemCount = nInvalid;

            pCurNode->dwFlags &= ~MLI_SELECTED;
            pMenuListData->pCurNode = pMenuListData->pTail;
            pMenuListData->pCurNode->dwFlags |= MLI_SELECTED;
            
            pNode = pMenuListData->pTail;

            while(nItemCount > 1)
            {
                pNode = pNode->pPrev;
                nItemCount--;
            }
         
            pMenuListData->pTopNode = pNode;
            MENULIST_InvalidateItems(pMenuListData,
                pMenuListData->pTopNode, nInvalid);
            
            if ((pMenuListData->pCurNode->dwFlags & MLI_NOTIFY) && 
                hwndParent)
            {
                PostMessage(hwndParent, WM_COMMAND, 
                   MAKEWPARAM(pMenuListData->pCurNode->wID, ML_FOCUSNOTIFY),
                   (LPARAM)pMenuListData->hWnd);
                if(pMenuListData->bVScroll)
                {
                    ScrollInfo.fMask = SIF_POS;
                    ScrollInfo.nPos = pMenuListData->pTail->wSerial;
                    SetScrollInfo(pMenuListData->hWnd, 
                        SB_VERT, &ScrollInfo, TRUE);
                }
            }
        }
		SetTimer(pMenuListData->hWnd, ID_UPFIRSTTIMER, 300, NULL);

		break;

	case VK_RETURN:
		PostMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
//		if (hwndParent) {
//            PostMessage(hwndParent, WM_COMMAND, 
//                MAKEWPARAM(pMenuListData->pCurNode->wID, ML_SELNOTIFY), 
//                (LPARAM)pMenuListData->hWnd);
//		}
		break;
	case VK_F5:
        if (hwndParent)
        {
			/*
            PostMessage(hwndParent, WM_COMMAND, 
                MAKEWPARAM(pMenuListData->pCurNode->wID, ML_SELNOTIFY), 
                (LPARAM)pMenuListData->hWnd);
			*/
            PostMessage(hwndParent, WM_KEYDOWN, wParam, lParam);
        }
        break;

    default:
        break;
    }

    return;
}

/*********************************************************************\
* Function  : MENULIST_HandlePenDown
* Purpose   :
*     Handle pen events.
* Params    :
*     pMenuListData : pointer to the private data
*     wParam        : word parameter
*     lParam        : long parameter
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_HandlePenDown(PMENULISTDATA pMenuListData, 
                                   WPARAM wParam, LPARAM lParam)
{
    PMLINODE pTopNode;
    PMLINODE pCurNode;
    PMLINODE pClickNode;
    PMLINODE pInvalidNode;
    int nInvalid;
    int number;
    int x, y;
    int right;
    int bottom;
    int offset;
    HWND hwndParent;

    pTopNode = pMenuListData->pTopNode;
    pCurNode = pMenuListData->pCurNode;
    if (!pTopNode || !pCurNode)
        return;

    x = LOWORD(lParam);
    y = HIWORD(lParam);

    // Check if the point is within items' area.
    right = pMenuListData->nTopLeftX + pMenuListData->nItemWidth;
    bottom = pMenuListData->nTopLeftY + pMenuListData->nItemHeight * 
        pMenuListData->wPageItemCount;
    if (x < pMenuListData->nTopLeftX || 
        x > right || 
        y < pMenuListData->nTopLeftY || 
        y > bottom)
        return;

    // Calculate number of items to move down from the top item.
    offset = y - pMenuListData->nTopLeftY;
    number = offset / pMenuListData->nItemHeight;
    if ((offset % pMenuListData->nItemHeight) == 0)
        number--;

    // Get parent window.
    hwndParent = GetParent(pMenuListData->hWnd);

    pClickNode = pTopNode;
    while (pClickNode != NULL && number-- != 0)
    {
        pClickNode = pClickNode->pNext;
    }
    if (!pClickNode)
        return;
    // Click the current focus item, just return.
    if (pClickNode == pCurNode)
    {
        // Send notify message.
        if (hwndParent)
            PostMessage(hwndParent, WM_COMMAND, 
                MAKEWPARAM(pMenuListData->pCurNode->wID, ML_SELNOTIFY), 
                (LPARAM)pMenuListData->hWnd);
        return;
    }

    if (pClickNode->wSerial > pCurNode->wSerial)
    {
        pInvalidNode = pCurNode;
        nInvalid = pClickNode->wSerial - pCurNode->wSerial + 1;
    }
    else
    {
        pInvalidNode = pClickNode;
        nInvalid = pCurNode->wSerial - pClickNode->wSerial + 1;
    }
    pCurNode->dwFlags &= ~MLI_SELECTED;
    pClickNode->dwFlags |= MLI_SELECTED;
    pMenuListData->pCurNode = pClickNode;
    MENULIST_InvalidateItems(pMenuListData, pInvalidNode, nInvalid);

    // Send notify message.
    if (hwndParent)
        PostMessage(hwndParent, WM_COMMAND, 
            MAKEWPARAM(pMenuListData->pCurNode->wID, ML_SELNOTIFY), 
            (LPARAM)pMenuListData->hWnd);
}

/*********************************************************************\
* Function  : MENULIST_SetItemData
* Purpose   :
*     Set item data.
* Params    :
*     pMenuListData : pointer to the private data
*     wID           : item ID
*     pData         : data to set
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_SetItemData(PMENULISTDATA pMenuListData, WORD wID, 
                                 void *pData)
{
    PMLINODE pNode;

    if (pMenuListData == NULL)
    {
        //ASSERT(0);
        return;
    }

    pNode = pMenuListData->pHead;
    while (pNode != NULL)
    {
        if (pNode->wID == wID)
        {
            // The specified item is found.
            pNode->pNodeData = pData;
            break;
        }

        pNode = pNode->pNext;
    }
}

/*********************************************************************\
* Function  : MENULIST_GetItemData
* Purpose   :
*     Get item data.
* Params    :
*     pMenuListData : pointer to the private data
*     wID           : item ID
* Return    :
*     return item data.
* Remarks
**********************************************************************/
static void *MENULIST_GetItemData(PMENULISTDATA pMenuListData, WORD wID)
{
    PMLINODE pNode;

    if (!pMenuListData)
    {
       // ASSERT(0);
        return NULL;
    }

    pNode = pMenuListData->pHead;
    while (pNode != NULL)
    {
        if (pNode->wID == wID)
        {
            // The specified item is found.
            return pNode->pNodeData;
        }

        pNode = pNode->pNext;
    }

    return NULL;
}

/*********************************************************************\
* Function  : MENULIST_FreeResource
* Purpose   :
*     Free MenuList resources.
* Params    :
*     pMenuListData : pointer to the private data
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_FreeResource(PMENULISTDATA pMenuListData)
{
    PMLINODE pNode;
    PMLINODE pNext;

    if (!pMenuListData)
    {
       // ASSERT(0);
        return;
    }

    if (pMenuListData->hGifBg)
        DeleteImage((HIMAGE)pMenuListData->hGifBg);
    if (pMenuListData->hGifSeparator)
        DeleteImage((HIMAGE)pMenuListData->hGifSeparator);
    if (pMenuListData->hGifDown)
        DeleteImage((HIMAGE)pMenuListData->hGifDown);
    if(pMenuListData->hGifItemF && pMenuListData->hGifItemF != hDefItemPic)
        DeleteImage((HIMAGE)pMenuListData->hGifItemF);
    if(pMenuListData->hGifItemNF)
        DeleteImage((HIMAGE)pMenuListData->hGifItemNF);

    pNode = pMenuListData->pHead;
    while (pNode != NULL)
    {
        pNext = pNode->pNext;

        if (pNode->nFileType == PREBROW_FILETYPE_ICON)
		{
			if (pNode->hIcon)
				DestroyIcon((HICON)pNode->hIcon);
		}
		else if (pNode->nFileType == PREBROW_FILETYPE_BMP)
		{
			if (pNode->hIcon)
				DeleteObject(pNode->hIcon);
		}

        if (pNode->pNodeString)
            free(pNode->pNodeString);
        if (pNode->pSelIconName)
            free(pNode->pSelIconName);
        free(pNode);

        pNode = pNext;
    }
}

/*********************************************************************\
* Function  : MENULIST_SetBgPic
* Purpose   :
*     Set background picture
* Params    :
*     pMenuListData : pointer to the private data
*     lpszBgPic     : string buffer of the picture name
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_SetBgPic(PMENULISTDATA pMenuListData, PCSTR lpszBgPic)
{
    SIZE size;

    if (!lpszBgPic)
        return;
    
    pMenuListData->hGifBg = (HGIFANIMATE)CreateImageFromFile(lpszBgPic);
    if(pMenuListData->hGifBg)
    {
        GetImageDimensionFromFile(lpszBgPic, &size);
        pMenuListData->nBmpWidth = size.cx;
        pMenuListData->nBmpHeight = size.cy;
    }
    
}


static BOOL MENULIST_SetItemPics(PMENULISTDATA pMenuListData,
                                 PCSTR lpszItemFocus,
                                 PCSTR lpszItemNFocus)
{
    if(!pMenuListData)
        return FALSE;
    if(!lpszItemFocus || !lpszItemNFocus)
        if(hDefItemPic != NULL)
        {
            pMenuListData->hGifItemF = hDefItemPic;
            return TRUE;
        }

    pMenuListData->hGifItemF  = 
        (HGIFANIMATE)CreateImageFromFile(lpszItemFocus);

    if(pMenuListData->hGifItemF == NULL)
        return FALSE;
    pMenuListData->hGifItemNF = 
        (HGIFANIMATE)CreateImageFromFile(lpszItemNFocus);

    if(pMenuListData->hGifItemNF == NULL)
        return FALSE;

    return TRUE;
}

/*********************************************************************\
* Function  : MENULIST_CreateNode
* Purpose   :
*     Create a item node from item template.
* Params    :
*     pMenuListData : pointer to the private data
*     pItemTemplate : pointer to the item template
* Return    :
*     Return newly created node.
* Remarks
**********************************************************************/
static PMLINODE MENULIST_CreateNode(PMENULISTDATA pMenuListData, 
                              const MENULISTITEMTEMPLATE *pItemTemplate)
{
    PMLINODE	pNode;
    SIZE		size;
	

    pNode = (PMLINODE)malloc(sizeof(MLINODE));
    if (!pNode)
    {
        //ASSERT(0);
        return NULL;
    }

    memset(pNode, 0, sizeof(MLINODE));

    // Get data from item template.
    pNode->dwFlags = pItemTemplate->dwFlags;
    pNode->wID = pItemTemplate->wID;
    pNode->nPicAreaWidth = pItemTemplate->nPicAreaWidth;
    if (pItemTemplate->lpszItemName)
    {
        int nStrLen = strlen(pItemTemplate->lpszItemName);
        pNode->pNodeString = (char *)malloc(nStrLen + 1);
        if(pNode->pNodeString == NULL)
            return NULL;
        strcpy(pNode->pNodeString, pItemTemplate->lpszItemName);
        pNode->nStrLen = nStrLen;
    }
    //item's icon
    if (pItemTemplate->lpszIconName)
    {
		pNode->nFileType = GetFileTypeByName((LPCTSTR)pItemTemplate->lpszIconName,TRUE);
		if (pNode->nFileType == PREBROW_FILETYPE_ICON) {
			GetIconDimensionFromFile(pItemTemplate->lpszIconName, 
				&pNode->IconSize);
		}
        if (pNode->nFileType == PREBROW_FILETYPE_BMP) {
			GetImageDimensionFromFile(pItemTemplate->lpszIconName, 
				&pNode->IconSize);
        }

        if(pNode->IconSize.cx >= pMenuListData->nItemWidth / 2)
            pNode->IconSize.cx = pMenuListData->nItemWidth / 2 - 2;
        if(pNode->IconSize.cy / 2 > pMenuListData->nItemHeight - 2)
        {
            size.cy = pNode->IconSize.cy / 2 + 2;
            size.cx = pMenuListData->nItemWidth;

            SendMessage(pMenuListData->hWnd, 
                ML_SETITEMSIZE, 0, (LPARAM)&size);
        }

		if(pNode->nFileType == PREBROW_FILETYPE_ICON)
		{
			pNode->hIcon = (HIMAGE)CreateIconFromFile(pItemTemplate->lpszIconName,
				pNode->IconSize.cx, pNode->IconSize.cy);
        }
		if (pNode->nFileType == PREBROW_FILETYPE_BMP) 
		{
			pNode->hIcon = (HIMAGE)CreateBitmapFromImageFile(NULL, pItemTemplate->lpszIconName, NULL, NULL);
		}

        if (pItemTemplate->dwFlags & MLI_CHANGEICON)
        {
            int nStrLen = strlen(pItemTemplate->lpszIconName);	
            pNode->pSelIconName = (char *)malloc(nStrLen + 3);
            if(pNode->pSelIconName == NULL)
                return NULL;
            strncpy(pNode->pSelIconName, 
                pItemTemplate->lpszIconName, nStrLen - 4);

            pNode->pSelIconName[nStrLen-4] = 0;
            strcat(pNode->pSelIconName, "_f.ico");			
        }
        else
            pNode->pSelIconName = NULL;
    }

    return pNode;
}

/*********************************************************************\
* Function  : MENULIST_AddItem
* Purpose   :
*     Add an item
* Params    :
*     pMenuListData     : pointer to the private data
*     pItemTemplate     : pointer to an item template
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_AddItem(PMENULISTDATA pMenuListData, 
                             const MENULISTITEMTEMPLATE *pItemTemplate)
{
    PMLINODE pNode;
    PMLINODE pLast;
    RECT rcInvalid;
    WORD offset;
    SCROLLINFO  ScrollInfo;

    if (!pMenuListData || !pItemTemplate)
    {
        //ASSERT(0);
        return;
    }

    // Create node from template.
    pNode = MENULIST_CreateNode(pMenuListData, pItemTemplate);
    if (!pNode)
        return;

    if (!(pMenuListData->pHead))
    {
        pNode->wSerial = 0;
        pNode->dwFlags |= MLI_SELECTED; // fixedly select the first item
        pNode->pPrev = NULL;
        pMenuListData->pHead = pNode;
        pMenuListData->pTail = pNode;
        pMenuListData->pTopNode = pNode;
        pMenuListData->pCurNode = pNode;
        pMenuListData->wTotalItemCount++;

        rcInvalid.left = pMenuListData->nTopLeftX;
        rcInvalid.top = pMenuListData->nTopLeftY;
        rcInvalid.right = rcInvalid.left + pMenuListData->nItemWidth;
        rcInvalid.bottom = rcInvalid.top + pMenuListData->nItemHeight;
        InvalidateRect(pMenuListData->hWnd, &rcInvalid, FALSE);
        return;
    }

    pLast = pMenuListData->pHead;
    while (pLast->pNext != NULL)
        pLast = pLast->pNext;
    pLast->pNext = pNode;
    pNode->pPrev = pLast;
    pNode->wSerial = pLast->wSerial + 1;
    pNode->dwFlags &= ~MLI_SELECTED;
    pMenuListData->wTotalItemCount++;
    pMenuListData->pTail = pNode;

    offset = pNode->wSerial - pMenuListData->pTopNode->wSerial;
    if (offset < pMenuListData->wPageItemCount)
    {
        rcInvalid.left = pMenuListData->nTopLeftX;
        rcInvalid.top = pMenuListData->nTopLeftY + 
            offset * pMenuListData->nItemHeight;
        rcInvalid.right = rcInvalid.left + pMenuListData->nItemWidth;
        rcInvalid.bottom = rcInvalid.top + pMenuListData->nItemHeight;
        InvalidateRect(pMenuListData->hWnd, &rcInvalid, FALSE);
    }

    if(pMenuListData->bVScroll)
    {
        if(pMenuListData->wTotalItemCount > pMenuListData->wPageItemCount)
        {
            ScrollInfo.cbSize = sizeof(SCROLLINFO);
            ScrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
            ScrollInfo.nMin = 0;
            ScrollInfo.nMax = pMenuListData->wTotalItemCount-1;
            ScrollInfo.nPage = pMenuListData->wPageItemCount;
            ScrollInfo.nPos = 0;
            
            SetScrollInfo(pMenuListData->hWnd, SB_VERT, &ScrollInfo, TRUE);
            
            if(pMenuListData->wTotalItemCount-1 <= 
                pMenuListData->wPageItemCount)
                EnableScrollBar(pMenuListData->hWnd, 
                SB_VERT, ESB_ENABLE_BOTH);
        }
        else
        {
            ScrollInfo.cbSize = sizeof(SCROLLINFO);
            ScrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
            ScrollInfo.nMin = 0;
            ScrollInfo.nMax = pMenuListData->wTotalItemCount-1;
            ScrollInfo.nPage = pMenuListData->wPageItemCount;
            ScrollInfo.nPos = 0;
            
            SetScrollInfo(pMenuListData->hWnd, SB_VERT, &ScrollInfo, TRUE);
            EnableScrollBar(pMenuListData->hWnd, SB_VERT, ESB_DISABLE_BOTH);
        }
        if(pMenuListData->nItemWidth > pMenuListData->nCliWidth)
            pMenuListData->nItemWidth = pMenuListData->nCliWidth-2;
    }
}

/*********************************************************************\
* Function  : MENULIST_AdjustNode
* Purpose   :
*     Adjust top and current node while deleting an item
* Params    :
*     pMenuListData : pointer to the private data
*     pDelNode      : pointer to the deleted node
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_AdjustNode(PMENULISTDATA pMenuListData, 
                                PMLINODE pDelNode)
{
    if (pMenuListData->pTopNode == pDelNode)
    {
        if (pDelNode->pNext)
            pMenuListData->pTopNode = pDelNode->pNext;
        else
            pMenuListData->pTopNode = pDelNode->pPrev;
    }

    if (pMenuListData->pCurNode == pDelNode)
    {
        if (pDelNode->pNext)
            pMenuListData->pCurNode = pDelNode->pNext;
        else
            pMenuListData->pCurNode = pDelNode->pPrev;

        if (pMenuListData->pCurNode)
            pMenuListData->pCurNode->dwFlags |= MLI_SELECTED;
    }
}

/*********************************************************************\
* Function  : MENULIST_AdjustSerial
* Purpose   :
*     Adjust serial of nodes from the specified node
* Params    :
*     pNode : pointer to the specified node
*     val   : adjust value
* Return    :
*     None.
* Remarks
**********************************************************************/
static void MENULIST_AdjustSerial(PMLINODE pNode, int val)
{
    while (pNode != NULL)
    {
        pNode->wSerial += val;
        pNode = pNode->pNext;
    }
}

/*********************************************************************\
* Function  : MENULIST_DelItem
* Purpose   :
*     Delete an item
* Params    :
*     pMenuListData : pointer to the private data
*     wID           : ID of the specified item
* Return    :
*     If success return TRUE, or else return FALSE.
* Remarks
**********************************************************************/
static BOOL MENULIST_DelItem(PMENULISTDATA pMenuListData, WORD wID)
{
    PMLINODE pNode;
    PMLINODE pNext;
    SCROLLINFO  ScrollInfo;

    if (!pMenuListData)
    {
        //ASSERT(0);
        return FALSE;
    }

    pNode = pMenuListData->pHead;
    if (pNode == NULL)
        return FALSE;
    if (pNode->wID == wID)
    {
        // The specified node is head.
        if(pMenuListData->pTail == pMenuListData->pHead)
            pMenuListData->pTail = pNode->pNext;
        pMenuListData->pHead = pNode->pNext;
        if (pMenuListData->pHead != NULL)
            pMenuListData->pHead->pPrev = NULL;
        MENULIST_AdjustNode(pMenuListData, pNode);

        if (pNode->nFileType == PREBROW_FILETYPE_ICON)
		{
			if (pNode->hIcon)
				DestroyIcon((HICON)pNode->hIcon);
		}
		else if (pNode->nFileType == PREBROW_FILETYPE_BMP)
		{
			if (pNode->hIcon)
				DeleteObject(pNode->hIcon);
		}
        if (pNode->pNodeString != NULL)
            free(pNode->pNodeString);
        if (pNode->pSelIconName != NULL)
            free(pNode->pSelIconName);
        free(pNode);

        MENULIST_AdjustSerial(pMenuListData->pHead, -1);
        pMenuListData->wTotalItemCount--;
        InvalidateRect(pMenuListData->hWnd, NULL, TRUE);
        return TRUE;
    }

    // Find the specified node.
    while ((pNext = pNode->pNext) != NULL)
    {
        if (pNext->wID == wID)
        {
            if(pNext == pMenuListData->pTail)
                pMenuListData->pTail = pNode;
            // got it.
            pNode->pNext = pNext->pNext;
            if (pNode->pNext != NULL)
                pNode->pNext->pPrev = pNode;
            MENULIST_AdjustNode(pMenuListData, pNext);

            if (pNext->hIcon != NULL)
                DeleteObject(pNext->hIcon);
            if (pNext->pNodeString != NULL)
                free(pNext->pNodeString);
            if (pNode->pSelIconName != NULL)
                free(pNode->pSelIconName);
            free(pNext);

            MENULIST_AdjustSerial(pNode->pNext, -1);
            pMenuListData->wTotalItemCount--;

            if(pMenuListData->bVScroll)
            {
                if(pMenuListData->wTotalItemCount > 
                    pMenuListData->wPageItemCount)
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
                    ScrollInfo.nMin = 0;
                    ScrollInfo.nMax = pMenuListData->wTotalItemCount-1;
                    ScrollInfo.nPage = pMenuListData->wPageItemCount;
                    ScrollInfo.nPos = 0;
            
                    SetScrollInfo(pMenuListData->hWnd, 
                        SB_VERT, &ScrollInfo, TRUE);
                }
                else
                {
                    ScrollInfo.cbSize = sizeof(SCROLLINFO);
                    ScrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
                    ScrollInfo.nMin = 0;
                    ScrollInfo.nMax = pMenuListData->wTotalItemCount-1;
                    ScrollInfo.nPage = pMenuListData->wPageItemCount;
                    ScrollInfo.nPos = 0;
            
                    SetScrollInfo(pMenuListData->hWnd, 
                        SB_VERT, &ScrollInfo, TRUE);

                    if(pMenuListData->wTotalItemCount+1 > 
                        pMenuListData->wPageItemCount)
                        EnableScrollBar(pMenuListData->hWnd, 
                        SB_VERT, ESB_DISABLE_BOTH);
                
                }
                if(pMenuListData->nItemWidth > pMenuListData->nCliWidth)
                    pMenuListData->nItemWidth = pMenuListData->nCliWidth-2;
            }

            InvalidateRect(pMenuListData->hWnd, NULL, TRUE);
            return TRUE;
        }

        pNode = pNext;
    }

    return FALSE;
}

/*********************************************************************\
* Function  : MENULIST_SetItemText
* Purpose   :
*     Set the specified item's text
* Params    :
*     pMenuListData : pointer to the private data
*     wID           : ID of the specified item
*     szText        : Text to be set
* Return    :
*     If success return TRUE, or else return FALSE.
* Remarks
**********************************************************************/
static BOOL MENULIST_SetItemText(PMENULISTDATA pMenuListData, WORD wID, PSTR szText)
{
    PMLINODE pNode;

    if (!pMenuListData)
    {
        //ASSERT(0);
        return FALSE;
    }

    pNode = pMenuListData->pHead;
    while (pNode != NULL)
    {
        if (pNode->wID == wID)
        {
            // The specified item is found.
            if(pNode->pNodeString != NULL)
            {
                free(pNode->pNodeString);
                pNode->pNodeString = NULL;
            }

            if(szText != NULL)
            {
                pNode->pNodeString = (PSTR)malloc( strlen(szText) + 1 );
                if(pNode->pNodeString == NULL)
                    return FALSE;
//todo:should add code to implement big and small font both exist in a item as name here
                strcpy(pNode->pNodeString, szText);
                MENULIST_InvalidateItems(pMenuListData, pNode, 1);
                
                return TRUE;
            }
            
        }

        pNode = pNode->pNext;
    }
    return FALSE;
}

/*********************************************************************\
* Function  : MENULIST_GetItemTextLen
* Purpose   :
*     Get the length of the specified item's text
* Params    :
*     pMenuListData : pointer to the private data
*     wID           : ID of the specified item
* Return    :
*     If success return the length, or else return -1.
* Remarks
**********************************************************************/
static int MENULIST_GetItemTextLen(PMENULISTDATA pMenuListData, WORD wID)
{
    PMLINODE pNode;

    if (!pMenuListData)
    {
        //ASSERT(0);
        return -1;
    }

    pNode = pMenuListData->pHead;
    while (pNode != NULL)
    {
        if (pNode->wID == wID)
        {
            // The specified item is found.
            if(pNode->pNodeString != NULL)
            {
                return strlen(pNode->pNodeString);
            }
            else
                return -1;
        }
        pNode = pNode->pNext;
    }
    return -1;
}

/*********************************************************************\
* Function  : MENULIST_GetItemText
* Purpose   :
*     Get the specified item's text
* Params    :
*     pMenuListData : pointer to the private data
*     wID           : ID of the specified item
*     szText        : Recieve the text
* Return    :
*     If success return TRUE, or else return FALSE.
* Remarks
**********************************************************************/
static BOOL 
MENULIST_GetItemText(PMENULISTDATA pMenuListData, WORD wID, PSTR szText)
{
    PMLINODE pNode;

    if (!pMenuListData)
    {
        //ASSERT(0);
        return FALSE;
    }

    pNode = pMenuListData->pHead;
    while (pNode != NULL)
    {
        if (pNode->wID == wID)
        {
            // The specified item is found.
            if(pNode->pNodeString != NULL)
            {
                strcpy(szText, pNode->pNodeString);
                return TRUE;
            }
            else
                return FALSE;
        }
        pNode = pNode->pNext;
    }
    return FALSE;

}

/*********************************************************************\
* Function  : MENULIST_SetCurSel
* Purpose   :
*     
* Params    :
*     
* Return    :
*     
* Remarks
**********************************************************************/
static WORD MENULIST_SetCurSel(PMENULISTDATA pMenuListData, WORD wID)
{
    PMLINODE pNode;
    WORD     wOldSelID;

    if(!pMenuListData)
    {
        //ASSERT(0);
        return -1;
    }

    pNode = pMenuListData->pHead;
    while(pNode != NULL)
    {
        if(pNode->wID == wID)
        {
            pMenuListData->pCurNode->dwFlags &= ~MLI_SELECTED;
            pNode->dwFlags |= MLI_SELECTED;

            MENULIST_InvalidateItems(pMenuListData, 
                pMenuListData->pCurNode, 1);
            if(pMenuListData->pCurNode->pNext == pNode ||
               pNode->pNext == pMenuListData->pCurNode)
            {
                UpdateWindow(pMenuListData->hWnd);
            }

            MENULIST_InvalidateItems(pMenuListData, pNode, 1);
            //UpdateWindow(pMenuListData->hWnd);

            wOldSelID = pMenuListData->pCurNode->wID;
            pMenuListData->pCurNode = pNode;

            return wOldSelID;
        }

        pNode = pNode->pNext;
    }

    return -1;
}

/*********************************************************************\
* Function  : MENULIST_ResetContent
* Purpose   :
*     
* Params    :
*     
* Return    :
*     
* Remarks
**********************************************************************/
static BOOL MENULIST_ResetContent(PMENULISTDATA pMenuListData)
{
    PMLINODE pNode;
    PMLINODE pNext;


    if(!pMenuListData)
    {
        //ASSERT(0);
        return FALSE;
    }

    pNode = pMenuListData->pHead;
    while(pNode != NULL)
    {
        pNext = pNode->pNext;

        if (pNode->nFileType == PREBROW_FILETYPE_ICON)
		{
			if (pNode->hIcon)
				DestroyIcon((HICON)pNode->hIcon);
		}
		else if (pNode->nFileType == PREBROW_FILETYPE_BMP)
		{
			if (pNode->hIcon)
				DeleteObject(pNode->hIcon);
		}
        if (pNode->pNodeString != NULL)
            free(pNode->pNodeString);
        if (pNode->pSelIconName != NULL)
            free(pNode->pSelIconName);
        free(pNode);

        pNode = pNext;
    }

    pMenuListData->pHead = NULL;
    pMenuListData->pCurNode = NULL;
    pMenuListData->pTopNode = NULL;
    pMenuListData->wTotalItemCount = 0;

    InvalidateRect(pMenuListData->hWnd, NULL, TRUE);

    return TRUE;

}

/*********************************************************************\
* Function  : MENULIST_DoPageUp
* Purpose   :
*     
* Params    :
*     
* Return    :
*     
* Remarks
**********************************************************************/
static void MENULIST_DoPageUp(PMENULISTDATA pMenuListData)
{
    PMLINODE pNode;
    //RECT     rcInvalid;
    int      nInvalid;
    int      nTopSerial;

    if(pMenuListData->pTopNode == pMenuListData->pHead)
        return;

    if(pMenuListData->pTopNode->wSerial - 
        pMenuListData->wPageItemCount <= 0)
    {
        pMenuListData->pTopNode = pMenuListData->pHead;
        nInvalid = pMenuListData->wPageItemCount;
    }
    else
    {
        nTopSerial = pMenuListData->pTopNode->wSerial - 
            pMenuListData->wPageItemCount;
        pNode = pMenuListData->pTopNode;
        while(pNode != NULL && pNode->wSerial != nTopSerial)
            pNode = pNode->pPrev;
        
        if(pNode != NULL)
            pMenuListData->pTopNode = pNode;

        nInvalid = pMenuListData->wPageItemCount;
    }

    MENULIST_InvalidateItems(pMenuListData, 
        pMenuListData->pTopNode, nInvalid);

    return;
}

/*********************************************************************\
* Function  : MENULIST_DoPageDown
* Purpose   :
*     
* Params    :
*     
* Return    :
*     
* Remarks
**********************************************************************/
static void MENULIST_DoPageDown(PMENULISTDATA pMenuListData)
{
    int      nInvalid;
    int      i;
    if(pMenuListData->wTotalItemCount <= pMenuListData->wPageItemCount)
    {
        return;
    }
    else if(pMenuListData->wTotalItemCount - 
        (pMenuListData->pTopNode->wSerial + 
        pMenuListData->wPageItemCount) >= 
        pMenuListData->wPageItemCount)
    {
        i = 0;
        while(i < pMenuListData->wPageItemCount)
        {
            pMenuListData->pTopNode = pMenuListData->pTopNode->pNext;
            i++;
        }
        nInvalid = pMenuListData->wPageItemCount;
    }
    else
    {
        i = nInvalid = pMenuListData->wPageItemCount;

        pMenuListData->pTopNode = pMenuListData->pTail;
        while(i > 1)
        {
            pMenuListData->pTopNode = pMenuListData->pTopNode->pPrev;
            i--;
        }
    }
    
    MENULIST_InvalidateItems(pMenuListData, 
        pMenuListData->pTopNode, nInvalid);

    return;
}

/*********************************************************************\
* Function  : MENULIST_DoLineUp
* Purpose   :
*     
* Params    :
*     
* Return    :
*     
* Remarks
**********************************************************************/
static void MENULIST_DoLineUp(PMENULISTDATA pMenuListData)
{
//    PMLINODE pNode;
    int      nInvalid;

    if(pMenuListData->pTopNode == pMenuListData->pHead)
        return;
   
    pMenuListData->pTopNode = pMenuListData->pTopNode->pPrev;
    nInvalid = pMenuListData->wPageItemCount;

    MENULIST_InvalidateItems(pMenuListData, 
        pMenuListData->pTopNode, nInvalid);

    return;
}

/*********************************************************************\
* Function  : MENULIST_DoLineDown
* Purpose   :
*     
* Params    :
*     
* Return    :
*     
* Remarks
**********************************************************************/
static void MENULIST_DoLineDown(PMENULISTDATA pMenuListData)
{
//    PMLINODE pNode;
    int      nInvalid;

    if(pMenuListData->pTopNode->wSerial == 
        pMenuListData->pTail->wSerial - pMenuListData->wPageItemCount + 1)
        return;

    pMenuListData->pTopNode = pMenuListData->pTopNode->pNext;
    nInvalid = pMenuListData->wPageItemCount;

    MENULIST_InvalidateItems(pMenuListData,  
        pMenuListData->pTopNode, nInvalid);
 
    return;
}

/*********************************************************************\
* Function  : MENULIST_DoThumbPos
* Purpose   :
*     
* Params    :
*     
* Return    :
*     
* Remarks
**********************************************************************/
static void MENULIST_DoThumbPos(PMENULISTDATA pMenuListData, int nPos)
{
    PMLINODE pNode;
    int      nInvalid;

    if(nPos < pMenuListData->pHead->wSerial || 
        nPos > pMenuListData->pTail->wSerial)
        return;

    if(nPos == pMenuListData->pTopNode->wSerial)
        return;
    else if(nPos > pMenuListData->pTopNode->wSerial)
    {
        pNode = pMenuListData->pTopNode;
        while(pNode != NULL && pNode->wSerial != nPos)
            pNode = pNode->pNext;
        
        if(pNode != NULL)
            pMenuListData->pTopNode = pNode;

        nInvalid = pMenuListData->wPageItemCount;
    }
    else
    {
        pNode = pMenuListData->pTopNode;
        while(pNode != NULL && pNode->wSerial != nPos)
            pNode = pNode->pPrev;
        
        if(pNode != NULL)
            pMenuListData->pTopNode = pNode;

        nInvalid = pMenuListData->wPageItemCount;
    }

    MENULIST_InvalidateItems(pMenuListData,  
        pMenuListData->pTopNode, nInvalid);
    return;
}

/*********************************************************************\
* Function  : MENULIST_WndProc
* Purpose   :
*     Window proc of MenuList control.
* Params    :
*     hWnd          : window handle
*     uMsgCmd       : message value
*     wParam        : word param
*     lParam        : long param
* Return    :
*     The return value is determined by the message value.
* Remarks
**********************************************************************/
static LRESULT CALLBACK MENULIST_WndProc(HWND hWnd, UINT uMsgCmd, 
                                         WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rcClient;
    SIZE size;
    PMENULISTDATA pMenuListData;
    PMENULISTDATA pTemplateData;
    PCREATESTRUCT pCreateStruct;
    PAINTSTRUCT   ps;
    LRESULT lResult = TRUE;
    SCROLLINFO  ScrollInfo;

    // Get private data pointer
    pMenuListData = (PMENULISTDATA)GetUserData(hWnd);
    if (pMenuListData == NULL)
    {
        //ASSERT(0);
        return 0;
    }

    switch (uMsgCmd)
    {
    case WM_CREATE:

#ifdef _MENULIST_
//    StartObjectDebug();
#endif
        memset(pMenuListData, 0, sizeof(MENULISTDATA));
        GetClientRect(hWnd, &rcClient);

        pCreateStruct = (PCREATESTRUCT)lParam;

        if ((pCreateStruct->lpCreateParams) != NULL)
        {
            // Get data from menulist template.
            pTemplateData = (PMENULISTDATA)pCreateStruct->lpCreateParams;
            memcpy(pMenuListData, pTemplateData, sizeof(MENULISTDATA));
        }

        pMenuListData->wID = (WORD)(pCreateStruct->hMenu);
        
        if ((pCreateStruct->style & WS_VSCROLL) != 0)
            pMenuListData->bVScroll = TRUE;
        else
            SetWindowLong(hWnd, GWL_STYLE, 
            (long)(pCreateStruct->style|WS_VSCROLL));

        pMenuListData->hWnd = hWnd;
        //pMenuListData->BkColor = RGB(255, 255, 255);
		pMenuListData->BkColor = RGB(255, 0, 255);

        //GetClientRect(hWnd, &rcClient);
        pMenuListData->nCliWidth = rcClient.right - rcClient.left;
        pMenuListData->nCliHeight = rcClient.bottom - rcClient.top;

        // Check the position of the top item.
        if (pMenuListData->nTopLeftX >= pMenuListData->nCliWidth)
            pMenuListData->nTopLeftX = pMenuListData->nCliWidth - 1;
        if (pMenuListData->nTopLeftY >= pMenuListData->nCliHeight)
            pMenuListData->nTopLeftY = pMenuListData->nCliHeight - 1;

        if (pMenuListData->nItemHeight != 0)
            pMenuListData->wPageItemCount = 
                (WORD)((pMenuListData->nCliHeight - 
                pMenuListData->nTopLeftY) / pMenuListData->nItemHeight);
        else
            pMenuListData->wPageItemCount = 0;

        if((pMenuListData->bVScroll) == TRUE)
        {
            if(pMenuListData->wTotalItemCount > 
                pMenuListData->wPageItemCount)
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
                ScrollInfo.nMin = 0;
                ScrollInfo.nMax = pMenuListData->wTotalItemCount-1;
                ScrollInfo.nPage = pMenuListData->wPageItemCount;
                ScrollInfo.nPos = 0;
            
                SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
            }
            else
            {
                ScrollInfo.cbSize = sizeof(SCROLLINFO);
                ScrollInfo.fMask = SIF_ALL | SIF_DISABLENOSCROLL;
                ScrollInfo.nMin = 0;
                ScrollInfo.nMax = pMenuListData->wTotalItemCount-1;
                ScrollInfo.nPage = pMenuListData->wPageItemCount;
                ScrollInfo.nPos = 0;
            
                SetScrollInfo(hWnd, SB_VERT, &ScrollInfo, TRUE);
                EnableScrollBar(hWnd, SB_VERT, ESB_DISABLE_BOTH);
                
            }
            if(pMenuListData->nItemWidth > pMenuListData->nCliWidth)
                pMenuListData->nItemWidth = pMenuListData->nCliWidth-2;
        }

        // Check item width
        if (pMenuListData->nItemWidth < MINITEMWIDTH)
            pMenuListData->nItemWidth = MINITEMWIDTH;

        hdc = GetDC(hWnd);
        GetTextExtentPoint32(hdc, "T", -1, &size);
        ReleaseDC(hWnd, hdc);

        return 0;

    case WM_SIZE:
        pMenuListData->nCliWidth = (int)LOWORD(lParam);
        pMenuListData->nCliHeight = (int)HIWORD(lParam);
        
        if(pMenuListData->nItemWidth > pMenuListData->nCliWidth || 
            pMenuListData->nItemWidth > 210)
            pMenuListData->nItemWidth = 210; 

        if (pMenuListData->nItemHeight != 0)
            pMenuListData->wPageItemCount = 
            (WORD)((pMenuListData->nCliHeight - 
                pMenuListData->nTopLeftY) / pMenuListData->nItemHeight);
        else
            pMenuListData->wPageItemCount = 0;
        return 0;

    case WM_PAINT:

        hdc = BeginPaint(hWnd, &ps); //NULL);
        MENULIST_OnPaint(hWnd, hdc, pMenuListData, &(ps.rcPaint));
        EndPaint(hWnd, &ps); //NULL);

        return 0;

	case WM_TIMER:
		switch(wParam) 
		{
		case ID_DOWNFIRSTTIMER:
			KillTimer(hWnd, ID_DOWNFIRSTTIMER);
			SetTimer(hWnd, ID_DOWNREPEATTIMER, 100, NULL);
			break;

		case ID_DOWNREPEATTIMER:
			//SendMessage(hWnd, WM_KEYDOWN, VK_DOWN, 0);
			keybd_event(VK_DOWN, 0, 0, 0);
			break;

		case ID_UPFIRSTTIMER:
			KillTimer(hWnd, ID_UPFIRSTTIMER);
			SetTimer(hWnd, ID_UPREPEATTIMER, 100, NULL);
			break;

		case ID_UPREPEATTIMER:
			//SendMessage(hWnd, WM_KEYDOWN, VK_UP, 0);
			keybd_event(VK_UP, 0, 0, 0);
			break;
			
		default:
			return DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
		}
		return 0;
	
	case WM_KILLFOCUS:
		KillTimer(hWnd, ID_DOWNFIRSTTIMER);
		KillTimer(hWnd, ID_DOWNREPEATTIMER);
		KillTimer(hWnd, ID_UPFIRSTTIMER);
		KillTimer(hWnd, ID_UPREPEATTIMER);
		break;

	case WM_KEYUP:
		switch(LOWORD(wParam)) 
		{
		case VK_DOWN:
			KillTimer(hWnd, ID_DOWNFIRSTTIMER);
			KillTimer(hWnd, ID_DOWNREPEATTIMER);
			break;

		case VK_UP:
			KillTimer(hWnd, ID_UPFIRSTTIMER);
			KillTimer(hWnd, ID_UPREPEATTIMER);

		default:
			return DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
		}
		return 0;

    case WM_KEYDOWN:
        switch(wParam) 
        {		
        case VK_F2:		
        case VK_F10:
		case VK_LEFT:
		case VK_RIGHT:
            PostMessage(GetParent(pMenuListData->hWnd), 
                WM_KEYDOWN, wParam, lParam);
            break;
        default:
            MENULIST_HandleKeyDown(pMenuListData, wParam, lParam);
        }
        return 0;

    case WM_PENDOWN:
        MENULIST_HandlePenDown(pMenuListData, wParam, lParam);
        return 0;

    case ML_SETBGPIC:
        MENULIST_SetBgPic(pMenuListData, (PCSTR)lParam);
        InvalidateRect(hWnd, NULL, TRUE);
        break;

    case ML_SETITEMSIZE:
        if (lParam != 0)
        {
            PSIZE pSize = (PSIZE)lParam;

            if (pMenuListData->nItemWidth != pSize->cx || 
                pMenuListData->nItemHeight != pSize->cy)
                InvalidateRect(hWnd, NULL, TRUE);
            if(pSize->cx > 176 || pSize->cx > pMenuListData->nCliWidth)
                pMenuListData->nItemWidth = 176;
            else
                pMenuListData->nItemWidth = (int)pSize->cx;
			if (pSize->cy!=0) {
				pMenuListData->nItemHeight = pSize->cy;
			}
			else
			{
				pMenuListData->nItemHeight = 29;
            }
            if (pMenuListData->nItemHeight != 0)
                pMenuListData->wPageItemCount = 
                    (WORD)((pMenuListData->nCliHeight - 
                    pMenuListData->nTopLeftY) / pMenuListData->nItemHeight);
            else
                pMenuListData->wPageItemCount = 0;

            // Check item width
            if (pMenuListData->nItemWidth < MINITEMWIDTH)
                pMenuListData->nItemWidth = MINITEMWIDTH;
        }
        break;

    case ML_SETITEMSTARTPOS:
        lResult = TRUE;
        break;
  
    case ML_SETCURSEL:
        lResult = MENULIST_SetCurSel(pMenuListData, (WORD)wParam);
        break;
        
    case ML_GETCURSEL:
        if(pMenuListData->pCurNode != NULL)
            lResult = pMenuListData->pCurNode->wID;
        else
            lResult = -1;

        break;

    case ML_SETICONOFFSET:
        if (pMenuListData->nLeftGap != (int)lParam)
            InvalidateRect(pMenuListData->hWnd, NULL, TRUE);

        pMenuListData->nLeftGap = (int)lParam;
        if (pMenuListData->nLeftGap >= pMenuListData->nCliWidth)
        {
            pMenuListData->nLeftGap = pMenuListData->nCliWidth - 1;
        }
        break;

    case ML_SETITEMICON:
        break;

    case ML_SETITEMDATA:
        MENULIST_SetItemData(pMenuListData, (WORD)wParam, (void*)lParam);
        break;

    case ML_GETITEMDATA:
        {
            void *pData = MENULIST_GetItemData(pMenuListData, (WORD)wParam);
            return (LRESULT)pData;
        }

    case ML_ADDITEM:
        MENULIST_AddItem(pMenuListData, (PMENULISTITEMTEMPLATE)lParam);
        break;

    case ML_DELITEM:
        lResult = MENULIST_DelItem(pMenuListData, (WORD)wParam);
        break;

    case ML_SETITEMTEXT:
        lResult = MENULIST_SetItemText(pMenuListData,
            (WORD)wParam, (PSTR)lParam);
        break;

    case ML_GETITEMTEXTLEN:
        lResult = MENULIST_GetItemTextLen(pMenuListData, (WORD)wParam);
        break;

    case ML_GETITEMTEXT:
        lResult = MENULIST_GetItemText(pMenuListData, 
            (WORD)wParam, (PSTR)lParam);
        break;

    case ML_RESETCONTENT:
        lResult = MENULIST_ResetContent(pMenuListData);
        break;

    case ML_SETITEMPIC:
        if((BOOL)wParam == TRUE) //focus pic
        {
            if((PCSTR)lParam == NULL)
                return FALSE;

            if(pMenuListData->hGifItemF != NULL)
            {
                DeleteImage((HIMAGE)pMenuListData->hGifItemF);
                pMenuListData->hGifItemF = NULL;
            }

            pMenuListData->hGifItemF = 
                (HGIFANIMATE)CreateImageFromFile((PCSTR)lParam);

            InvalidateRect(hWnd, NULL, TRUE);

            if(pMenuListData->hGifItemF != NULL)
                return TRUE;
            else
                return FALSE;
        }
        else if((BOOL)wParam == FALSE)
        {
            if((PCSTR)lParam == NULL)
                return FALSE;

            if(pMenuListData->hGifItemNF != NULL)
            {
                DeleteImage((HIMAGE)pMenuListData->hGifItemNF);
                pMenuListData->hGifItemNF = NULL;
            }

            pMenuListData->hGifItemNF = 
                (HGIFANIMATE)CreateImageFromFile((PCSTR)lParam);

            InvalidateRect(hWnd, NULL, TRUE);

            if(pMenuListData->hGifItemNF != NULL)
                return TRUE;
            else
                return FALSE;
        }
        break;

    case ML_GETITEMPIC:
        if((BOOL)wParam == TRUE) //focus pic
        {
            return (LRESULT)pMenuListData->hGifItemF;
        }
        else if((BOOL)wParam == FALSE)
        {
            return (LRESULT)pMenuListData->hGifItemNF;
        }
        break;

    case ML_SETBKCOL:
        if(lParam >= RGB(0,0,0) && lParam <= RGB(255,255,255))
        {
            lResult = (LRESULT)pMenuListData->BkColor;
            pMenuListData->BkColor = lParam;

            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case ML_GETBKCOL:
        if(pMenuListData->BkColor >= RGB(0,0,0) &&
           pMenuListData->BkColor <= RGB(255,255,255))
           lResult = (LRESULT)pMenuListData->BkColor;
        break;

    case WM_VSCROLL:
        {
            int nScrollCode = (int)LOWORD(wParam);
            int nPos        = (int)HIWORD(wParam);
            //SCROLLINFO OldInfo;

            switch(nScrollCode)
            {
            case SB_LINEUP:
                MENULIST_DoLineUp(pMenuListData);
                break;
            case SB_LINEDOWN:
                MENULIST_DoLineDown(pMenuListData);
                break;
            case SB_PAGEUP:
                MENULIST_DoPageUp(pMenuListData);
                break;
            case SB_PAGEDOWN:
                MENULIST_DoPageDown(pMenuListData);
                break;
            case SB_THUMBPOSITION:
                MENULIST_DoThumbPos(pMenuListData, nPos);
                break;
            }

            if(pMenuListData->bVScroll)
            {
                ScrollInfo.fMask = SIF_POS;
                ScrollInfo.nPos = (int)pMenuListData->pTopNode->wSerial;
                SetScrollInfo(pMenuListData->hWnd, 
                    SB_VERT, &ScrollInfo, TRUE);
            }
        }
        break;

    case WM_DESTROY:
        nInstCount--;

        MENULIST_FreeResource(pMenuListData);
        if(hDefItemPic != NULL && nInstCount == 0)
        {
            DeleteImage((HIMAGE)hDefItemPic);
            hDefItemPic = NULL;
        }
        if(hIconBk != NULL && nInstCount == 0)
        {
            DeleteImage((HIMAGE)hIconBk);
            hIconBk = NULL;
        }

#ifdef _MENULIST_
//    EndObjectDebug();
#endif
        //free(pMenuListData);
        return 0;

    default:
        return DefWindowProc(hWnd, uMsgCmd, wParam, lParam);
    }

    return lResult;
}

/*********************************************************************\
* Function  : LoadMenuListIndirect
* Purpose   :
*     Load MenuList data from MenuList template.
* Params    :
*     pMenuListTemplate     : pointer to the MenuList template
* Return    :
*     Return handle of the loaded data.
* Remarks
**********************************************************************/
HMENULIST LoadMenuListIndirect(const MENULISTTEMPLATE* pMenuListTemplate)
{
    int i;
    PMLINODE pNode;
    PMLINODE pPrev;
    PMENULISTDATA pMenuListData;
    //BOOL bHasSel = FALSE;


    if(hDefItemPic == NULL && nInstCount == 0)
    {
        hDefItemPic = (HGIFANIMATE)CreateImageFromFile(DEFITEMPIC);
        if(hDefItemPic == NULL)
            return FALSE;
    }
    
    if(hIconBk == NULL && nInstCount == 0)
    {
        hIconBk = (HGIFANIMATE)CreateImageFromFile(ICONBK);
        if(hIconBk != NULL)
            GetImageDimension((HIMAGE)hIconBk, &IconBkSize);
        else
            return FALSE;
    }

    nInstCount++;

    pMenuListData = (PMENULISTDATA)malloc(sizeof(MENULISTDATA));
    if(pMenuListData == NULL)
        return NULL;
    memset (pMenuListData, 0, sizeof(MENULISTDATA));

    // Set background bitmap.
    MENULIST_SetBgPic(pMenuListData, pMenuListTemplate->lpszBgPic);
    
    // Set separator bitmap.
    if ((pMenuListTemplate->dwStyle & MLS_SEPAPIC) && 
        (pMenuListTemplate->separator != NULL))
    {
        pMenuListData->hGifSeparator = 
            (HGIFANIMATE)CreateImageFromFile(pMenuListTemplate->separator);
    }

    MENULIST_SetItemPics(pMenuListData, 
                         pMenuListTemplate->lpszItemFocus,
                         pMenuListTemplate->lpszItemNFocus);

    // Set item position parameters.
    if(pMenuListTemplate->nItemWidth > 180)//170)
        pMenuListData->nItemWidth = 180;//170;
    else
        pMenuListData->nItemWidth = pMenuListTemplate->nItemWidth;
    
    pMenuListData->nItemHeight = 29;//pMenuListTemplate->nItemHeight;
    pMenuListData->nTopLeftX = 0;//8;//pMenuListTemplate->nTopLeftX;
    pMenuListData->nTopLeftY = 2;//pMenuListTemplate->nTopLeftY;
    pMenuListData->nLeftGap = 8;//pMenuListTemplate->nLeftGap;

    if (pMenuListTemplate->cmlit <= 0 || !(pMenuListTemplate->pmlItems))
        return (HMENULIST)pMenuListData;

    // Create head node.
    pNode = MENULIST_CreateNode(pMenuListData, 
        &(pMenuListTemplate->pmlItems[0]));
    if (!pNode)
        return (HMENULIST)pMenuListData;

    pNode->wSerial = 0;
    pNode->pPrev = NULL;
    pMenuListData->pHead = pNode;
    pMenuListData->pTopNode = pNode;
    pMenuListData->pCurNode = pNode;
    
    // Create other nodes.
    pPrev = pNode;
    for (i = 1; i < (int)pMenuListTemplate->cmlit; i++)
    {
        pNode = MENULIST_CreateNode(pMenuListData, 
            &(pMenuListTemplate->pmlItems[i]));
        if (!pNode)
        {
            pMenuListData->wTotalItemCount = (WORD)i;
            return (HMENULIST)pMenuListData;
        }
        pNode->wSerial = i;
        pPrev->pNext = pNode;
        pNode->pPrev = pPrev;
        
        pNode->dwFlags &= ~MLI_SELECTED;
        pPrev = pNode;
    }

    // Set actual item count.
    pMenuListData->wTotalItemCount = (WORD)i;
    //if (!bHasSel)
        pMenuListData->pHead->dwFlags |= MLI_SELECTED;

    return (HMENULIST)pMenuListData;
}

/*********************************************************************\
* Function  : DestroyMenuList
* Purpose   :
*     Free MenuList resources loaded from MenuList template.
* Params    :
*     pMenuListData : pointer to the private data
*     hMenuList     : handled of the loaded data
* Return    :
*     if success, return TRUE, otherwise return FALSE.
* Remarks
**********************************************************************/
BOOL DestroyMenuList(HMENULIST hMenuList)
{
    PMENULISTDATA pMenuListData;

    pMenuListData = (PMENULISTDATA)hMenuList;
    if (!pMenuListData)
    {
        //ASSERT(0);
        return FALSE;
    }

    MENULIST_FreeResource(pMenuListData);
    free(pMenuListData);

    return TRUE;
}

/********************************************************************
* Function   LoadMenuList  
* Purpose    创建HMENULIST
* Params     bLostkeyNotify:
            如果希望点击左softkey的时候,menulist收不到消息,那么请取FALSE
            如果希望点击左softkey的时候后,只有左softkey收到消息,
            而menulis收不到消息,那么请取TRUE
* Return     
* Remarks      
**********************************************************************/
HMENULIST   LoadMenuList(MENULISTTEMPLATE * MenuListElement,BOOL bLostkeyNotify)
{
    int isw,ish;

    if (NULL == MenuListElement)
        return NULL;
    isw = GetScreenUsableWH1(SM_CXSCREEN);
    ish = GetScreenUsableWH1(SM_CYSCREEN);
    MenuListElement->cmlit          = 0;    
    MenuListElement->lpszBgPic      = NULL;
    MenuListElement->nItemWidth     = isw;
    MenuListElement->nItemHeight    = 22;
    MenuListElement->nTopLeftX      = 0;
    MenuListElement->nTopLeftY      = 0;
    MenuListElement->nLeftGap       = 5;

    MenuListElement->nUArrowX       = isw - 20;
    MenuListElement->nUArrowY       = 5;
    MenuListElement->nUArrowHeight  = 10;
    MenuListElement->nUArrowWidth   = 10;
    
    MenuListElement->lpszUAPic      = NULL;
    MenuListElement->nDArrowX       = isw - 20;
    MenuListElement->nDArrowY       = ish - 10;
    MenuListElement->nDArrowHeight  = 10;
    MenuListElement->nDArrowWidth   = 10;
    MenuListElement->lpszDAPic      = NULL;
    MenuListElement->pmlItems       = 0;

    MenuListElement->dwStyle        = MLI_LEFT | MLI_NOTIFY;

    return (LoadMenuListIndirect((const MENULISTTEMPLATE *)MenuListElement));
}
/********************************************************************
* Function   MenuList_AddString  
* Purpose    重新加载数据到Menulist中
* Params     **P:需要添加的文字,* id:各个文字对应的项目的id,** pic:各个项目对应的图标文件名
* Return     
* Remarks      
**********************************************************************/
void    MenuList_AddString(HWND hMenuList,char ** p,int * id,char ** pic)
{
    MENULISTITEMTEMPLATE tmpValue;

    while (** p)
    {
        memset(&tmpValue,0x00,sizeof(MENULISTITEMTEMPLATE));
        if (pic != NULL)
            tmpValue.lpszIconName = * pic++;
        else
            tmpValue.lpszIconName = NULL;

        tmpValue.dwFlags        = MLI_LEFT | MLI_NOTIFY;
        tmpValue.nPicAreaWidth  = 20;
        tmpValue.wID            = * id++;
        tmpValue.lpszItemName   = * p++;
        SendMessage(hMenuList, ML_ADDITEM, 0, (LPARAM)&tmpValue);
    }
}
/********************************************************************
* Function   MenuList_AddStringOne  
* Purpose    加载一项数据到Menulist中
* Params     pItemName:需要添加的文字,id:文字对应的项目的id,pIconName:项目对应的图标文件名
* Return     
* Remarks      
**********************************************************************/
void    MenuList_AddStringOne(HWND hMenuList,const char * pItemName,int id,const char * pIconName)
{
    MENULISTITEMTEMPLATE tmpValue;
    memset(&tmpValue,0x00,sizeof(MENULISTITEMTEMPLATE));

    tmpValue.lpszIconName = pIconName;
    
    tmpValue.dwFlags        = MLI_LEFT | MLI_NOTIFY;
    tmpValue.nPicAreaWidth  = 20;
    tmpValue.wID            = id;
    tmpValue.lpszItemName   = pItemName;
    SendMessage(hMenuList, ML_ADDITEM, 0, (LPARAM)&tmpValue);
}
/********************************************************************
* Function   ResetMenuList  
* Purpose    清除MENULIST中的全部数据,*id需要以-1为结束标志
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL    ResetMenuList(HWND hMenuList,int * id)
{
    if (NULL == id)
        return FALSE;

    while (* id > 0)
        SendMessage(hMenuList,ML_DELITEM,* id++,0);
    
    return TRUE;
}

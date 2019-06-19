/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PUBCONTROL
 *
 * Purpose  : 
 *            
\**************************************************************************/

#include "PreBrowhead.h"
#include "assert.h"

#define	LABELTOP		5
#define	CONTENTTOP		22
#define	LEFT	6
#define	WIDTH	160
#define	LABLEHEIGHT	16
#define	ITEN_INTEVAL	9
#define	BKBMP	"/rom/progman/grpbg.bmp"
#define	SFHEIGHT	16
#define	LFHEIGHT	23

static	LRESULT	FormViewerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
static	BOOL	FormView_Paint(HWND hwnd,PLISTBUF pListBuf);
static	BOOL	PREBROW_AddFormViewerData (PLISTBUF pListBuf, const char* szData, const char* szFullData);
static	void	UpdateDataListBuf(HWND hwnd, PLISTBUF pListBuf);
static	BOOL	FormView_RePaint(HWND hwnd,PLISTBUF pListBuf);
static	BOOL	DrawItem(PLISTDATA ptmp, HDC hdc, PRECT	rc);
static	int		GetPixelAllItem(PLISTBUF pListBuf);	

static	RECT	firstlableRect	={6,5,166,21};

static	RECT	firsttextRect	={6,22,166,45};
static	RECT	secondlableRect	={6,55,166,71};
static	RECT	secondtextRect	={6,72,166,95};
static	RECT	thirdlableRect	={6,105,166,121};
static	RECT	thirdtextRect	={6,122,166,145};


BOOL	FormView_Register(void)
{
	WNDCLASS wc;

    
    wc.style            = NULL;
    wc.lpfnWndProc      = FormViewerWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(LISTBUF);
    wc.hInstance        = NULL;//(HINSTANCE)hInst;
    wc.hIcon            = NULL;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = FORMVIEWER;
    
    if (!RegisterClass(&wc))
        return FALSE;
    
    return TRUE;
}
/*
HWND	FormView(HWND hParentWnd, BOOL bBg, PLISTBUF pListtmp)
{
	HWND		hmainWnd = NULL;
	RECT		rcClient;
	FormView_Register(NULL);
	GetClientRect(hParentWnd, &rcClient);
//	PREBROW_InitListBuf(&Listtmp);
//	PREBROW_AddData(&Listtmp,ML("picture"),ML("1234Kb"));
//	PREBROW_AddData(&Listtmp,ML("Notepad"),ML("2234Kb"));
//	PREBROW_AddData(&Listtmp,ML("Game"),ML("3234Kb"));
//	PREBROW_AddData(&Listtmp,ML("sms"),ML("4234Kb"));
	hmainWnd = CreateWindow(WC_FORMVIEW,NULL,
		WS_VISIBLE  |WS_CHILD|WS_VSCROLL,
		rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		hParentWnd, NULL, NULL, (PVOID)pListtmp);
	if (!hmainWnd) {
		return NULL;
	}
	return hmainWnd;	
}
*/
static	LRESULT	FormViewerWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT	lResult;
	PLISTBUF	pListBuf;
	PCREATESTRUCT	pCreatstruct;
	SCROLLINFO		ScrollInfo;
//	int				nCurPos;
	int				nScrollSwing;


	pListBuf = (PLISTBUF)GetUserData(hwnd);
	lResult	=	TRUE;

	switch(uMsg) {
	case WM_CREATE:
		{
			int		nTotal;

			pCreatstruct	= (PCREATESTRUCT)lParam;
			memcpy(pListBuf, (PLISTBUF)pCreatstruct->lpCreateParams, sizeof(LISTBUF));
			UpdateDataListBuf(hwnd,pListBuf);
			nTotal = GetPixelAllItem(pListBuf);
			memset(&ScrollInfo, 0, sizeof(SCROLLINFO));
			if (pCreatstruct->style&WS_VSCROLL) 
			{
				/*
				ScrollInfo.cbSize = sizeof(SCROLLINFO);
				ScrollInfo.fMask = SIF_ALL|SIF_DISABLENOSCROLL;
				ScrollInfo.nMin = 0;
				ScrollInfo.nMax = pListBuf->nDataNum-1;
				ScrollInfo.nPage = 3;
				ScrollInfo.nPos = 0;
				*/
				ScrollInfo.cbSize = sizeof(SCROLLINFO);
				ScrollInfo.fMask = SIF_ALL|SIF_DISABLENOSCROLL;
				ScrollInfo.nMin = 0;
				ScrollInfo.nMax = nTotal + 10 + pListBuf->nDataNum;
				ScrollInfo.nPage = 150;
				ScrollInfo.nPos = 0;
				printf("\n ###########scroll tatal: %d \n", ScrollInfo.nMax);
				SetScrollInfo(hwnd, SB_VERT, &ScrollInfo, TRUE);				
			}
		}
		break;
//
//	case WM_ACTIVATE:
//		if (wParam == WA_ACTIVE) {
//			SetFocus(hwnd);
//		}
//		
//		break;
	case WM_PAINT:
		FormView_RePaint(hwnd, pListBuf);
		break;
	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_UP:
			{
				if (pListBuf->pCurrentData->nVisibleLine == pListBuf->pCurrentData->nFVLine) 
				{
					if (pListBuf->pCurrentData == pListBuf->pDataHead) 
					{
						break;
					}
					else
					{
						pListBuf->pCurrentData = pListBuf->pCurrentData->pPre;
						pListBuf->pCurrentData->nVisibleLine++;
						if (pListBuf->pCurrentData->nVisibleLine == pListBuf->pCurrentData->nFVLine)
						{
							nScrollSwing = SFHEIGHT + LFHEIGHT ;
						}
						else
							nScrollSwing = LFHEIGHT ;
					}
					
				}
				else
				{
					pListBuf->pCurrentData->nVisibleLine++;
					if (pListBuf->pCurrentData->nVisibleLine == pListBuf->pCurrentData->nFVLine)
					{
						nScrollSwing = SFHEIGHT + LFHEIGHT;
					}
					else
						nScrollSwing = LFHEIGHT;
				}
				memset(&ScrollInfo, 0, sizeof(SCROLLINFO));
				ScrollInfo.cbSize = sizeof(SCROLLINFO);
				ScrollInfo.fMask = SIF_ALL;
				GetScrollInfo(hwnd, SB_VERT, &ScrollInfo);
				
				pListBuf->nCurrentIndex = 1;

				ScrollInfo.fMask = SIF_POS;
				//ScrollInfo.nPos = pListBuf->pCurrentData->nIndex;
				ScrollInfo.nPos -= nScrollSwing;
				SetScrollInfo(hwnd, SB_VERT, &ScrollInfo, TRUE);
				printf("\r\n scroll nMax:%d &&&\tnPos:%d ***\tnScrollSwing:%d\n", ScrollInfo.nMax, ScrollInfo.nPos, nScrollSwing);
				//pListBuf->pCurrentData = pListBuf->pCurrentData->pPre;
				InvalidateRect(hwnd,NULL,TRUE);
			}
			break;
		case VK_DOWN:
			if (pListBuf->pCurrentData == pListBuf->pDataHead->pPre && pListBuf->pCurrentData->nVisibleLine <7) 
			{
				break;
			}
			if (pListBuf->nCurrentIndex < 0) 
			{
				break;
			}
			pListBuf->pCurrentData->nVisibleLine--;
			if (0 == pListBuf->pCurrentData->nVisibleLine) 
			{
				if (pListBuf->pCurrentData->nFVLine == 1)
				{
					nScrollSwing = SFHEIGHT + LFHEIGHT;
				}
				else
					nScrollSwing = LFHEIGHT;
				pListBuf->pCurrentData = pListBuf->pCurrentData->pNext;
			}
			else
				nScrollSwing = LFHEIGHT;

			memset(&ScrollInfo, 0, sizeof(SCROLLINFO));
			ScrollInfo.cbSize = sizeof(SCROLLINFO);
			ScrollInfo.fMask = SIF_ALL;
			GetScrollInfo(hwnd, SB_VERT, &ScrollInfo);

			ScrollInfo.fMask = SIF_POS;
            //ScrollInfo.nPos = pListBuf->pCurrentData->nIndex;
			ScrollInfo.nPos += nScrollSwing;
            SetScrollInfo(hwnd, SB_VERT, &ScrollInfo, TRUE);
			printf("scroll nMax:%d &&&\tnPos:%d ***\tnScrollSwing:%d\n", ScrollInfo.nMax, ScrollInfo.nPos, nScrollSwing);
			InvalidateRect(hwnd,NULL,TRUE);
			break;
		default:
			lResult = PostMessage(GetParent(hwnd),uMsg,wParam,lParam);
			break;
		}
		break;

	case WM_DESTROY:
		PREBROW_FreeListBuf(pListBuf);
		break;

	default:
		lResult = PDADefWindowProc(hwnd,uMsg,wParam,lParam);
		break;
	}
	return	lResult;
}


static	BOOL	FormView_Paint(HWND hwnd,PLISTBUF pListBuf)
{
//	PLISTDATA	pTempNode;
	HFONT	hSmallfont,holdFont;
	HDC		hdc;
	int		oldbm;
	
	GetFontHandle(&hSmallfont,SMALL_FONT);
	hdc = BeginPaint(hwnd, NULL);
	if (!pListBuf->pCurrentData) {
		pListBuf->pCurrentData = pListBuf->pDataHead;
	}
	DrawImageFromFile(hdc, BKBMP,0,0,ROP_SRC);
	holdFont = SelectObject(hdc,hSmallfont);
	oldbm = SetBkMode(hdc,TRANSPARENT);
	//there isn't corresponding operation to exception
	DrawText(hdc,pListBuf->pCurrentData->szData,-1, &firstlableRect,DT_LEFT|DT_VCENTER);
	//no circulation
	if (pListBuf->pCurrentData->pNext != pListBuf->pDataHead) {	
		DrawText(hdc,pListBuf->pCurrentData->pNext->szData,-1, &secondlableRect,DT_LEFT|DT_VCENTER);	
		if (pListBuf->pCurrentData->pNext->pNext != pListBuf->pDataHead) {		
			DrawText(hdc,pListBuf->pCurrentData->pNext->pNext->szData,-1, &thirdlableRect,DT_LEFT|DT_VCENTER);
		}
	}
	SelectObject(hdc,holdFont);
	DrawText(hdc,pListBuf->pCurrentData->szFullData,-1, &firsttextRect,DT_LEFT|DT_VCENTER);
	if (pListBuf->pCurrentData->pNext != pListBuf->pDataHead) {	
		DrawText(hdc,pListBuf->pCurrentData->pNext->szFullData,-1, &secondtextRect,DT_LEFT|DT_VCENTER);
		if (pListBuf->pCurrentData->pNext->pNext != pListBuf->pDataHead) {
			DrawText(hdc,pListBuf->pCurrentData->pNext->pNext->szFullData,-1, &thirdtextRect,DT_LEFT|DT_VCENTER);
		}
	}
	SetBkMode(hdc,oldbm);
	EndPaint(hwnd,NULL);
	return TRUE;
}


/*********************************************************************
 * Function Declare : FormView_RePaint
 * Explain : repaint wnd
 * Parameters : 
 * HWND hwnd -- 
 * PLISTBUF pListBuf -- 
 * Return : 
 * static BOOL  -- 
 * Author : mxlin 
 * Time : 2005-07-21 09:16:52 
*********************************************************************/
static	BOOL	FormView_RePaint(HWND hwnd,PLISTBUF pListBuf)
{
	PLISTDATA	pTempNode;
	
	HDC		hdc;
	int		iCount = 0;
	int		oldbm;
	RECT	rcClient;
	RECT	rcText;
	PSTR	pBuf;

	
	HFONT	hSmallfont,holdFont;

	GetFontHandle(&hSmallfont,SMALL_FONT);
	GetClientRect(hwnd,&rcClient);
	hdc = BeginPaint(hwnd, NULL);
	oldbm = SetBkMode(hdc,TRANSPARENT);
	if (!pListBuf->pCurrentData) {
		pListBuf->pCurrentData = pListBuf->pDataHead;
	}
	pTempNode = pListBuf->pCurrentData ;
	//DrawImageFromFile(hdc, BKBMP,0,0,ROP_SRC);
	//if only part of current node should be shown

	if (pTempNode->nFVLine > pTempNode->nVisibleLine) 
	{
		rcText.top = LABELTOP;
		rcText.left = LEFT;
		rcText.right = LEFT + WIDTH;
		rcText.bottom = rcText.top + 24*pTempNode->nVisibleLine;
		iCount =  strlen(pTempNode->szFullData)+pTempNode->nVisibleLine+3;
		pBuf = malloc(iCount);
		if (!pBuf) {
			return FALSE;
		}
//		for (i=0;i<iCount; i++) {
//			pBuf[i] = NULL;
//		}
		/*
		for (i=0; i<pTempNode->nVisibleLine; i++) 
		{
			strcat(pBuf,pTempNode->FVbuf[i+pTempNode->nFVLine -pTempNode->nVisibleLine]);
			strcat(pBuf,"\n");
		}
		*/
		pBuf[0] = 0;
		if(JoinAllNodeToString(pTempNode->pFormViewerContent, pTempNode->nFVLine - pTempNode->nVisibleLine,pBuf) == FALSE)
		{
			SetBkMode(hdc,oldbm);
			EndPaint(hwnd,NULL);
			return FALSE;
		}
		
		DrawText(hdc,pBuf,-1, &rcText,DT_LEFT);
		if (pBuf) {
			free(pBuf);
			pBuf = NULL;
		}
	}
	else
	{
		if(pTempNode->szData)
		{
			holdFont = SelectObject(hdc, hSmallfont);
			DrawText(hdc,pTempNode->szData, -1, &firstlableRect, DT_LEFT|DT_VCENTER);
			SelectObject(hdc,holdFont);
			rcText.top = CONTENTTOP;
		}
		else
			rcText.top = LABELTOP;
		rcText.left = LEFT;
		rcText.right = LEFT + WIDTH;
		rcText.bottom = rcText.top  + 24*pTempNode->nVisibleLine;
		iCount =  strlen(pTempNode->szFullData)+pTempNode->nVisibleLine+1;
		pBuf = malloc(iCount);
		if (!pBuf) {
			return FALSE;
		}
//		for (i=0;i<iCount; i++) {
//			pBuf[i] = NULL;
//		}
//		for (i=0; i<pTempNode->nVisibleLine; i++) 
//		{
//			strcat(pBuf,pTempNode->FVbuf[i+pTempNode->nFVLine -pTempNode->nVisibleLine]);
//			strcat(pBuf,"\n");
//		}
		pBuf[0] = 0;
		if(JoinAllNodeToString(pTempNode->pFormViewerContent, 0, pBuf) == FALSE)
		{
			SetBkMode(hdc,oldbm);
			EndPaint(hwnd,NULL);
			return FALSE;
		}
		
		DrawText(hdc,pBuf,-1, &rcText,DT_LEFT);
		if (pBuf) {
			free(pBuf);
			pBuf = NULL;
		}
	}
	if (pTempNode == pListBuf->pDataHead->pPre) {
		SetBkMode(hdc,oldbm);
		EndPaint(hwnd,NULL);
		return TRUE;
	}
/*
	pTempNode = pTempNode->pNext;	
	DrawItem(pTempNode,hdc,&rcText);
*/
	while (rcClient.bottom - rcText.bottom > 0) {
		
		if (pTempNode == pListBuf->pDataHead->pPre) {
			pListBuf->nCurrentIndex = -1;
			SetBkMode(hdc,oldbm);
			EndPaint(hwnd,NULL);
			return TRUE;
		}
		pTempNode = pTempNode->pNext;
		if(DrawItem(pTempNode,hdc,&rcText) == FALSE)
			break;
	}
/*	
	if (rcClient.bottom - rcText.bottom > 9) {
		
		if (pTempNode == pListBuf->pDataHead->pPre) {
			SetBkMode(hdc,oldbm);
			EndPaint(hwnd,NULL);
			return TRUE;
		}
		pTempNode = pTempNode->pNext;
		DrawItem(pTempNode,hdc,&rcText);
	}
*/	
	SetBkMode(hdc,oldbm);
	EndPaint(hwnd,NULL);
	return TRUE;
}
/*********************************************************************
 * Function Declare : DrawItem
 * Explain : draw a item that isn't the first item
 * Parameters : 
 * PLISTDATA pTempNode -- 
 *  HDC hdc -- 
 *  PRECT rcText -- 
 * Return : 
 * static void  -- 
 * Author : mxlin 
 * Time : 2005-07-21 09:17:40 
*********************************************************************/
static	BOOL	DrawItem(PLISTDATA pTempNode, HDC hdc, PRECT	rcText)
{
	int		len;
	PSTR	pBuf;
	HFONT	hSmallfont,holdFont;
	
	GetFontHandle(&hSmallfont,SMALL_FONT);
	if(pTempNode->szData)
	{
		holdFont = SelectObject(hdc, hSmallfont);
		rcText->top = rcText->bottom+9;
		rcText->left = LEFT;
		rcText->right = rcText->left + WIDTH;
		rcText->bottom = rcText->top + 16;
		DrawText(hdc,pTempNode->szData, -1, rcText, DT_LEFT|DT_VCENTER);
		SelectObject(hdc,holdFont);
	}
	len = strlen(pTempNode->szFullData)+pTempNode->nVisibleLine+1;
	pBuf = malloc(len);
	if (!pBuf) {
		return FALSE;
	}
//	for (i=0;i<len; i++) {
//		pBuf[i] = NULL;
//	}
//	for (i=0; i<pTempNode->nVisibleLine; i++) 
//	{
//		strcat(pBuf,pTempNode->FVbuf[i+pTempNode->nFVLine -pTempNode->nVisibleLine]);
//		strcat(pBuf,"\n");
//	}
	pBuf[0] = 0;
	if(JoinAllNodeToString(pTempNode->pFormViewerContent, 0, pBuf) == FALSE)
	{		
		return FALSE;
	}
	rcText->top = rcText->bottom+1;
	rcText->left = LEFT;
	rcText->right = rcText->left + WIDTH;
	rcText->bottom = rcText->top + 24*pTempNode->nVisibleLine;
	DrawText(hdc,pBuf,-1, rcText,DT_LEFT);
	if (pBuf) 
	{
		free(pBuf);
		pBuf = NULL;
	}
	return TRUE;
}

/*********************************************************************
 * Function Declare : UpdateDataListBuf
 * Explain : 
 * Parameters : 
 * HWND hwnd -- 
 *  PLISTBUF pListBuf -- 
 * Return : 
 * static void  -- 
 * Author : mxlin 
 * Time : 2005-07-21 09:18:22 
*********************************************************************/
static void	UpdateDataListBuf(HWND hwnd, PLISTBUF pListBuf)
{
	HDC	hdc;
	PLISTDATA	pTmpNode = NULL;
	PSTR	pFullData;
	int	i = 0;
	BOOL	bCut;
	unsigned int Offset = 0;
	char	tmpbuf[32];
	char	pString[32] = "";
	char	*pnewline;
	PSTR	ptr;
	hdc = GetDC(hwnd);
	if (pListBuf->pDataHead) {
		pTmpNode = pListBuf->pDataHead;
		do
		{
			pFullData = pTmpNode->szFullData;
			pTmpNode->pFormViewerContent = malloc(sizeof(FORMVEWERDATA));
			if (!pTmpNode->pFormViewerContent) {
				return;
			}
			memset(pTmpNode->pFormViewerContent, 0, sizeof(FORMVEWERDATA));
			pTmpNode->pFormViewerContent->pNext = NULL;
			if (!pFullData) {
				FormViewerAddData(pTmpNode->pFormViewerContent, "");

			}
			else
			{
				do {
					bCut = FALSE;
					//should consider the condition that the symbol '\n' exist in pFullData.?????
					GetTextExtentExPoint(hdc, pFullData, strlen(pFullData), WIDTH, &Offset,NULL,NULL);
					memset(tmpbuf, 0, sizeof(tmpbuf));
					strncpy(tmpbuf,pFullData,Offset);
					ptr = pnewline = tmpbuf;
					while((pnewline = strchr(ptr,'\n'))!=NULL ) 
					{
						bCut = TRUE;
						strncpy(pString,ptr,pnewline - ptr);
						pString[pnewline - ptr]=NULL;
						FormViewerAddData(pTmpNode->pFormViewerContent, pString);
						i++;
						pnewline++;
						pFullData = pFullData + (pnewline - ptr);
						ptr = pnewline;
						
						if (pnewline >= tmpbuf + strlen(tmpbuf)) {
							break;
						}
					}
					if (!bCut) {				
						strncpy(pString,tmpbuf,Offset);
						pString[Offset]=NULL;
						FormViewerAddData(pTmpNode->pFormViewerContent, pString);
						i++;
						pFullData+=Offset;
					}
					if (Offset == 0||Offset ==strlen(pFullData)) {
						break;
					}
					//				strncpy(pTmpNode->FVbuf[i],pFullData,Offset);
					
					
					if (pFullData >= pTmpNode->szFullData + strlen(pTmpNode->szFullData)) {
						break;
					}
				}while (Offset !=0 ) ;
			}
			pTmpNode->nVisibleLine = pTmpNode->nFVLine = i;
			//cal pixel
			pTmpNode->atime = 16+pTmpNode->nFVLine*23;
			//
			pTmpNode = pTmpNode->pNext;
			i = 0;
		}while (pTmpNode != pListBuf->pDataHead); 
	}
	ReleaseDC(hwnd,hdc);

}

BOOL	PREBROW_AddFormViewerData (PLISTBUF pListBuf, const char* szData, const char* szFullData)
{
	PLISTDATA	tempdata;
	PLISTDATA	pTail;
//	int			i;
	PSTR		pFullData;
//	char		*ptr;
	pFullData = (PSTR)szFullData;
	if (pListBuf->nDataNum == 0)//Initialize the chain
	{
		if ( NULL == ( pListBuf->pDataHead = malloc ( sizeof (LISTDATA) )) )
			return FALSE;

		pListBuf->pCurrentData	=	pListBuf->pDataHead;

		pListBuf->pCurrentData->nIndex	=	pListBuf->nDataNum;
		strcpy( pListBuf->pCurrentData->szData, szData );
		strcpy( pListBuf->pCurrentData->szFullData, szFullData );
		pListBuf->pCurrentData->hbmp = NULL;
//		ptr = GetTextExtentExPoint()
//		pListBuf->pCurrentData->pFVData = malloc(strlen(szFullData)+10);

		pListBuf->pCurrentData->pNext =	pListBuf->pDataHead;
		pListBuf->pCurrentData->pPre  = pListBuf->pDataHead;		


		pListBuf->nCurrentIndex	=	pListBuf->pCurrentData->nIndex;
		pListBuf->nDataNum++;
		return TRUE;
	}
	//add node to the chain
	if (NULL == (tempdata = malloc ( sizeof(LISTDATA) ) ) )
		return FALSE;	

	tempdata->nIndex = pListBuf->nDataNum;
	strcpy(tempdata->szData , szData);
	strcpy(tempdata->szFullData , szFullData);
	tempdata->hbmp = NULL;
//	tempdata->pFVData = malloc(strlen(szFullData)+10);

	pTail = pListBuf->pDataHead->pPre;

	pTail->pNext = tempdata;
	tempdata->pNext = pListBuf->pDataHead;

	tempdata->pPre  = pTail;
	pListBuf->pDataHead->pPre = tempdata;	
	
	pListBuf->nDataNum++;

	return TRUE;
}
static	int		GetPixelAllItem(PLISTBUF pListBuf)
{
	int		nTotalPixel = 0;
	int		i = 0;
	PLISTDATA	pHead;
	pHead = pListBuf->pDataHead;
	if (!pHead)
	{
		return 0;
	}
	for (i=0; i<pListBuf->nDataNum; i++)
	{
		nTotalPixel += pHead->atime;
		pHead = pHead->pNext;
	}
	return nTotalPixel;
}

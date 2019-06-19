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

/*---------- Dependencies ----------------------------------------------*/

#include "pollex.h"
#include "window.h"
#include "winpda.h"
//#include "fapi.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h" 

#include "string.h"
#include "stdio.h"
#include "plx_pdaex.h"
#include "app.h"
#include "stdlib.h"
#include "str_plx.h"
#include "str_public.h"
#include "setting.h"
#include "hpimage.h"
#include "pubapp.h"
#include "mullang.h"
#include "prioman.h"


/*---------- Constant / Macro Definitions ------------------------------*/

#define         WC_MSGSELWINEX                "MsgSelWinClassEx"
//#define         SEL_WIN_ICON                "ROM:SelWnd.ico"
#define         MAX_PROMPT_LEN              255

//#define         CHARWIDTH                   12
//#define         CHARHEIGHT                  23


#define         WINDOW_WIDTH                120
#define         INTERVAL                    5

#define			ID_YES		WM_USER+1001
#define			ID_NO		WM_USER+1002

/*---------- Type Declarations -----------------------------------------*/

static BOOL     bRegist = FALSE;
static	BOOL	bCOnfirmEx = FALSE;
//创建提示窗口的储存结构
typedef struct tagMsgSelWinStruct
{
	char		strCongif[50];
    HWND        hMsgSelWin;
    HWND        hWnd;
	HWND		hFrameWnd;
    UINT        msg;
    DWORD       Format;
    RECT        rPrompt;
//    RECT        rOK;
//    RECT        rCancel;
    char        szOk[25];
    char        szCancel[25];
	PSTR		szCaption;
    char        sMsgSelPrompt[MAX_PROMPT_LEN+1];
    struct tagMsgSelWinStruct *next;
}*PMSGSELWINSTRUCT, MSGSELWINSTRUCT;
//处理重入
static PMSGSELWINSTRUCT pMsgCurrentSelWin, pMsgSelWinHead = NULL;
//static	char	*szNotifyPrompt = NULL;
/*---------- function Declarations -------------------------------------*/

static LRESULT CALLBACK MsgSelWinProc(HWND, UINT, WPARAM, LPARAM);
static void FillMsgSelWinStruct(PMSGSELWINSTRUCT pMsgSelWinNode, 
                                HWND hMsgSelWnd, 
                                HWND hWnd,
                                UINT msg,
                                const char *szPrompt, NOTIFYTYPE eTypePic);
static PMSGSELWINSTRUCT FindCurrentMsgSelWin(HWND hWnd);
static PMSGSELWINSTRUCT DelMsgCurrentSelWin(HWND hMsgWin);
static BOOL AddMsgSelWinNode(HWND hMsgSelWnd, HWND hWnd, HWND	hFrame, UINT msg, const char *szok,
                             const char *szcancel, const char *szPrompt, NOTIFYTYPE eTypePic, PCSTR	szCaption);
BOOL IsConFirmEx();
/*------------------END-------------------------------------------------*/

/*************************************************************************
  Function   :HPConfirmWin
--------------------------------------------------------------------------
  Description:To show a message box with user specified prompt,caption and 
              time
--------------------------------------------------------------------------
  Input      :szPrompt     ----    address of text in message box
              szCaption    ----    address of title of message box
              nTimeout     ----    destroy the window after nTimeout*100 
                                   millisecond.If nTimeout = 0 never destroy
                                   this message window.
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
              None.
*************************************************************************/
BOOL PLXConfirmWinEx(HWND hFrame, HWND hWnd, PCSTR szPrompt, NOTIFYTYPE ePic, PCSTR szCaption, 
                  PCSTR szOk, PCSTR szCancel, UINT msg)
{
    BOOL        bRet = TRUE;
    WNDCLASS    wc;
	RECT		rc;

    HWND        hMsgSelWin;

    if (!bRegist)
    {
        wc.style         = 0;
        wc.lpfnWndProc   = MsgSelWinProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = NULL;
        wc.hIcon         = NULL;
        wc.hCursor       = NULL;
        wc.hbrBackground = GetStockObject(WHITE_BRUSH);;
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = WC_MSGSELWINEX;    
        if( !RegisterClass(&wc) )
            return FALSE;   /* ??? */
        bRegist = TRUE;
    }


	if (hFrame) 
	{
		GetClientRect(hFrame, &rc);
		hMsgSelWin = CreateWindow(WC_MSGSELWINEX, NULL,
			WS_CHILD,
			//CWIN_X, CWIN_Y, CWIN_WIDTH, CWIN_HEIGHT, 
			rc.left,
			rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			hFrame, NULL, NULL, NULL);
		if (szCaption) {
			SetWindowText(hFrame,szCaption);
		}
		
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
	}
	else
	{
		if (szCaption&&(*szCaption != NULL)) 
		{			
			hMsgSelWin = CreateWindow(WC_MSGSELWINEX, szCaption,
				PWS_STATICBAR|WS_CAPTION,
				//CWIN_X, CWIN_Y, CWIN_WIDTH, CWIN_HEIGHT, 
				PLX_WIN_POSITION,
				hWnd, NULL, NULL, NULL);
		}
		else
		{
			hMsgSelWin = CreateWindow(WC_MSGSELWINEX, NULL,
				PWS_STATICBAR,
				//CWIN_X, CWIN_Y, CWIN_WIDTH, CWIN_HEIGHT, 
				PLX_NOTIFICATON_POS,
				hWnd, NULL, NULL, NULL);
		}
	}

	if (hFrame) 
	{
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		UpdateWindow(hFrame);
		ShowWindow(hFrame, SW_SHOW);
		SetFocus(hMsgSelWin);
	}
//TRACELINE();
    if( hMsgSelWin != NULL )
    {
        AddMsgSelWinNode(hMsgSelWin, hWnd, hFrame, msg, szOk, szCancel, szPrompt, ePic, szCaption);
		bCOnfirmEx = TRUE;
		ShowWindow( hMsgSelWin, SW_SHOW );
		UpdateWindow( hMsgSelWin );
		SetFocus(hMsgSelWin);
    }
	if(szOk)
	{
		if(hFrame)
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)szOk);
		else
			SendMessage(hMsgSelWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_YES,1), (LPARAM)szOk);
	}
	else
	{
		if (hFrame) {
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		}
	}
	if(szCancel)
	{
		if (hFrame) 
		{
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)szCancel);
		}
		else
			SendMessage(hMsgSelWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_NO,0), (LPARAM)szCancel);
	}
	else
	{
		if (hFrame) 
		{
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		}
	}
    return TRUE;
}
/*************************************************************************
  Function   :MsgSelWinProc
--------------------------------------------------------------------------
  Description:This is MsgSelWin's message process procudure.
--------------------------------------------------------------------------
  Input      :hWnd         ----    Window's handle
              wMsgCmd      ----    Message ID
              wParam       ----    parameter
              lParam       ----    parameter
--------------------------------------------------------------------------
  Return     :TRUE         ----    Success
              FALSE        ----    Falied
--------------------------------------------------------------------------
  IMPORTANT NOTES:
              None.
*************************************************************************/
static LRESULT CALLBACK MsgSelWinProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LONG lResult;
    HDC hdc;
    PAINTSTRUCT ps;
    static int  index = 0;
    int         nBkMode;
	HBRUSH		newbrush,oldbrush;
	RECT		rcClient,rcCapicon;
//    PCOLORSCHEME pColorSche;
    
    pMsgCurrentSelWin = FindCurrentMsgSelWin(hWnd);
    lResult   = (LRESULT)TRUE;
    
    switch( wMsgCmd )
    {
	case WM_CREATE:
		PrioMan_CallMusic(PRIOMAN_PRIORITY_WARNING,1);
		DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
		DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
		DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);
		break;

	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			bCOnfirmEx = FALSE;
		}
		else
		{
			bCOnfirmEx = TRUE;
		}
		break;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
			if (strlen(pMsgCurrentSelWin->szOk) > 0)
			{
				if (pMsgCurrentSelWin->hFrameWnd) {
					SendMessage(pMsgCurrentSelWin->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				}
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				PostMessage(pMsgCurrentSelWin->hWnd, pMsgCurrentSelWin->msg, 0, 1);
				
			}            
            break;

        case VK_F10:
			if (strlen(pMsgCurrentSelWin->szCancel) > 0)
			{
				if (pMsgCurrentSelWin->hFrameWnd) {
					SendMessage(pMsgCurrentSelWin->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				}
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				PostMessage(pMsgCurrentSelWin->hWnd, pMsgCurrentSelWin->msg, 0, 0);
				
			}
           
            break;
       
        }            
        break;

	case PWM_SHOWWINDOW:
		if (!pMsgCurrentSelWin) {
			SetFocus(hWnd);
			break;
		}
		if (strlen(pMsgCurrentSelWin->szOk)>0) {
			SendMessage(pMsgCurrentSelWin->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)pMsgCurrentSelWin->szOk);
		}
		if (strlen(pMsgCurrentSelWin->szCancel)>0) {
			SendMessage(pMsgCurrentSelWin->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)pMsgCurrentSelWin->szCancel);
		}
		if (pMsgCurrentSelWin->szCaption) {
			SetWindowText(pMsgCurrentSelWin->hFrameWnd, pMsgCurrentSelWin->szCaption);
		}
		SendMessage(pMsgCurrentSelWin->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(pMsgCurrentSelWin->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
		SendMessage(pMsgCurrentSelWin->hFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
		SetFocus(hWnd);
		break;

    case WM_PAINT:
		{
			char	*szDisplay = NULL;
			HPEN hPen, hOldPen;
			
			hdc  = BeginPaint( hWnd, &ps );
			GetClientRect(hWnd, &rcClient);
			
			newbrush	= CreateBrush(BS_SOLID,RGB(163,176,229),NULL);
			oldbrush	= SelectObject(hdc,newbrush);

			hPen = CreatePen(PS_SOLID, 1, RGB(163,176,229));
			hOldPen = (HPEN)SelectObject(hdc, hPen);
			
			SetRect(&rcCapicon,rcClient.left,rcClient.top,rcClient.right,31);
			DrawRect(hdc,&rcCapicon);
			
			SelectObject(hdc, hOldPen);
			DeleteObject(hPen);
			
			newbrush	= SelectObject(hdc,oldbrush);
			DeleteObject(newbrush);
			
			nBkMode = SetBkMode(hdc, NEWTRANSPARENT);
			DrawImageFromFile(hdc,pMsgCurrentSelWin->strCongif,80,8,ROP_SRC);

			printf("\r\n***FILE: %s\t LINE: %d \tPROMPT: %s\n OK: %s \r\n", __FILE__,  __LINE__, pMsgCurrentSelWin->sMsgSelPrompt, pMsgCurrentSelWin->szOk);

			szDisplay = malloc(strlen(pMsgCurrentSelWin->sMsgSelPrompt)*2 + 1);
			if (!szDisplay)
			{
				DrawText(hdc, szDisplay, strlen(szDisplay), 
					&pMsgCurrentSelWin->rPrompt, DT_CENTER|DT_VCENTER);
			}
			else
			{
				
				FormatStr(szDisplay, pMsgCurrentSelWin->sMsgSelPrompt);
				
				DrawText(hdc, szDisplay, strlen(szDisplay), 
					&pMsgCurrentSelWin->rPrompt, DT_CENTER|DT_VCENTER);
				free(szDisplay);
			}
			SetBkMode(hdc, nBkMode);
			
			EndPaint( hWnd, &ps );
		}
//          OS_UpdateScreen(0, 0, 0, 0);
        break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

    case WM_DESTROY:
        DelMsgCurrentSelWin(pMsgCurrentSelWin->hMsgSelWin);
		PrioMan_EndCallMusic(PRIOMAN_PRIORITY_WARNING, TRUE);
		bCOnfirmEx = FALSE;
        break;
		
    default:
        return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
    return lResult;
}
/*************************************************************************
  Function   :FillMsgSelWinStruct
--------------------------------------------------------------------------
  Description:Fill the list node struct.
--------------------------------------------------------------------------
  Input      :pMsgSelWinNode ----    the list node pointer
              hWnd       ----    the current window handle
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static void FillMsgSelWinStruct(PMSGSELWINSTRUCT pMsgSelWinNode, 
                                HWND hMsgSelWnd, 
                                HWND hWnd,
                                UINT msg,
                                const char *szPrompt, NOTIFYTYPE eTypePic)
{
    RECT     rWndRect;
	int		nEnd;

    GetClientRect(hMsgSelWnd, &rWndRect);
	SureIcon(pMsgSelWinNode->strCongif,eTypePic);
    SetRect(&pMsgSelWinNode->rPrompt, rWndRect.left+INTERVAL, 
        rWndRect.top+31, rWndRect.right-INTERVAL, rWndRect.bottom);
/*
    SetRect(&pMsgSelWinNode->rOK, rWndRect.left+INTERVAL, 
        rWndRect.bottom - 20, rWndRect.left+INTERVAL + 50, rWndRect.bottom-2);
    SetRect(&pMsgSelWinNode->rCancel, rWndRect.right - 50 - INTERVAL, 
        rWndRect.bottom - 20, rWndRect.right -INTERVAL, rWndRect.bottom-2);
*/
    //NUMBERLINE = (pMsgSelWinNode->rPrompt.right - pMsgSelWinNode->rPrompt.left)/CHARWIDTH;
    //NUMBERROW = (pMsgSelWinNode->rPrompt.bottom - pMsgSelWinNode->rPrompt.top)/CHARHEIGHT;

    pMsgSelWinNode->hMsgSelWin = hMsgSelWnd;
    pMsgSelWinNode->hWnd = hWnd;
    pMsgSelWinNode->msg = msg;
//	if (strchr(szPrompt, '\n')) {
//		strcpy(pMsgSelWinNode->sMsgSelPrompt,szPrompt);
//	}
//	else
//	{
	memset(pMsgSelWinNode->sMsgSelPrompt, 0, MAX_PROMPT_LEN);
	if (szPrompt)
	{
		strncpy(pMsgSelWinNode->sMsgSelPrompt,szPrompt,MAX_PROMPT_LEN);
		nEnd = strlen(szPrompt) > MAX_PROMPT_LEN? MAX_PROMPT_LEN: strlen(szPrompt);
		pMsgSelWinNode->sMsgSelPrompt[nEnd] = 0;
	}
	else
		strcpy(pMsgSelWinNode->sMsgSelPrompt,"");
	
		//pMsgSelWinNode->Format = FormatStr(pMsgSelWinNode->sMsgSelPrompt,(PSTR)szPrompt);
//	}
	/*
    pMsgSelWinNode->Format = FormatStr(pMsgSelWinNode->sMsgSelPrompt, 
        szPrompt, NUMBERROW, NUMBERLINE);
		*/
    pMsgSelWinNode->next = NULL;
}
/*************************************************************************
  Function   :AddMsgSelWinNode
--------------------------------------------------------------------------
  Description:Create a new node, and fill the struct.
--------------------------------------------------------------------------
  Input      :hCall        ----    the caller
              szPrompt     ----    address of text in message box
              szCaption    ----    address of title of message box
              nTimeout     ----    destroy the window after nTimeout*100 
                                   millisecond.If nTimeout = 0 never destroy
                                   this message window.
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static BOOL AddMsgSelWinNode(HWND hMsgSelWnd, HWND hWnd, HWND hFrame,UINT msg, const char *szok,
                             const char *szcancel, const char *szPrompt, NOTIFYTYPE eTypePic, PCSTR	szCaption)
{
    pMsgCurrentSelWin = malloc(sizeof(MSGSELWINSTRUCT));
    if (!pMsgCurrentSelWin)
        return FALSE;
	memset(pMsgCurrentSelWin, 0, sizeof(MSGSELWINSTRUCT));
	if (!szPrompt)
	{
		return FALSE;
	}
    FillMsgSelWinStruct(pMsgCurrentSelWin, hMsgSelWnd, hWnd, msg, szPrompt, eTypePic);
	if (szok) {
		strcpy(pMsgCurrentSelWin->szOk, szok);
	}
    if (szcancel) {
		strcpy(pMsgCurrentSelWin->szCancel, szcancel);
    }
	if (hFrame) {
		pMsgCurrentSelWin->hFrameWnd = hFrame;
	}
	if (szCaption) {
		pMsgCurrentSelWin->szCaption = malloc(strlen(szCaption)+1);
		if (pMsgCurrentSelWin->szCaption==NULL)
		{
			return FALSE;
		}
		strcpy(pMsgCurrentSelWin->szCaption, szCaption);
	}
    
    
    if (!pMsgSelWinHead)
        pMsgSelWinHead = pMsgCurrentSelWin;
    else
    {
        pMsgCurrentSelWin->next = pMsgSelWinHead;
        pMsgSelWinHead = pMsgCurrentSelWin;
    }

    return TRUE;
}/*************************************************************************
  Function   :FindCurrentMsgSelWin
--------------------------------------------------------------------------
  Description:Find the list node by the given handle.
--------------------------------------------------------------------------
  Input      :hWnd ----  handle
--------------------------------------------------------------------------
  Return     :the node .
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static PMSGSELWINSTRUCT FindCurrentMsgSelWin(HWND hWnd)
{
    PMSGSELWINSTRUCT pTempMsgWin;

    pTempMsgWin = pMsgSelWinHead;
    while (pTempMsgWin)
    {
        if (pTempMsgWin->hMsgSelWin == hWnd)
            return pTempMsgWin;
        pTempMsgWin = pTempMsgWin->next;
    }
    return NULL;
}
/*************************************************************************
  Function   :DelMsgCurrentSelWin
--------------------------------------------------------------------------
  Description:Delete the list node about current window.
--------------------------------------------------------------------------
  Input      :hMsgWin ----    the current window handle
--------------------------------------------------------------------------
  Return     :the list head pointer.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static PMSGSELWINSTRUCT DelMsgCurrentSelWin(HWND hMsgWin)
{
    PMSGSELWINSTRUCT pDelMsgWin, pPreviouMsgWin;

    pDelMsgWin = pMsgSelWinHead;
    pPreviouMsgWin = pDelMsgWin;

    while (pDelMsgWin)
    {
        if (pDelMsgWin->hMsgSelWin == hMsgWin)
        {
            if (pDelMsgWin == pMsgSelWinHead)
                pMsgSelWinHead = pDelMsgWin->next;
            else
                pPreviouMsgWin->next = pDelMsgWin->next;
			if (pDelMsgWin->szCaption) {
				free(pDelMsgWin->szCaption);
			}
            free(pDelMsgWin);

            return pMsgSelWinHead;
        }
        pPreviouMsgWin = pDelMsgWin;
        pDelMsgWin = pDelMsgWin->next;
    }
    return NULL;
}
BOOL IsConFirmEx()
{
	PMSGSELWINSTRUCT	pMsgSelWin;
	pMsgSelWin = pMsgSelWinHead;
	while (pMsgSelWin)
	{
		if (IsWindow(pMsgSelWin->hMsgSelWin))
		{
			return TRUE;
		}
		else
			pMsgSelWin = pMsgSelWin->next;
	}
	return FALSE;
}
BOOL	UpdateConfirmExPrompt(PCSTR pBuf)
{
	if (!pBuf)
	{
		return FALSE;
	}
	if (!pMsgCurrentSelWin)
	{
		return FALSE;
	}
	strncpy(pMsgCurrentSelWin->sMsgSelPrompt,pBuf,(strlen(pBuf) >MAX_PROMPT_LEN)? MAX_PROMPT_LEN: strlen(pBuf));
	InvalidateRect(pMsgCurrentSelWin->hMsgSelWin, &pMsgCurrentSelWin->rPrompt, TRUE);
	return TRUE;
}
/*************************End of MsgSelWin.c*********************************/

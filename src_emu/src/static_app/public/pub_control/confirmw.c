/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

/*---------- Dependencies ----------------------------------------------*/
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
#ifndef _CONFIRMWIN_
#define _CONFIRMWIN_
#endif
/*---------- Global Definitions and Declarations -----------------------*/
/* MsgWin's  handle */
//static HWND	hCallWnd = NULL;
/*---------- Constant / Macro Definitions ------------------------------*/
#define        CW_EXITMESSAGE                (WM_USER+100)
#define        WC_MSGSELWIN				     "MsgSelWinClass"
//#define        GRAYRATE                      2 / 3

#define        MSGWIN_HEIGHT                 88 //101
#define        MSGWIN_WIDTH                  110 //201
#define        MSGWIN_LEFT                   55 //19
#define        MSGWIN_TOP                    44 //109

#define        BTNXINTERVAL                  3 //12
#define        BTNYINTERVAL                  3 //5
#define        GIFLEFT                       30//55
#define        GIFTOP                        121//6
#define        GIFWIDTH                      40
#define        GIFHEIGHT                     43

/*---------- Type Declarations -----------------------------------------*/
static RECT		TRect = {0, 46, 176, 130}; //{80, 120, 210, 180};
static BOOL		bRegist = FALSE;
static	BOOL	bConfirm = FALSE;

#define	ID_YES			WM_USER+100
#define	ID_NO			WM_USER+101



//创建提示窗口的储存结构
typedef struct tagMsgSelwinStruct
{
	char		strCongif[50];
	HWND        hMsgWin;
	HWND        hCall;
	HWND		hFrameWnd;
    
	DWORD       Format;
    char*       sMsgWinPrompt;
	COLORREF	GidfDlgColor;

    PSTR        szOk;
    PSTR        szCancel;
	PSTR		szCaption;
    struct tagMsgSelwinStruct *next;
}*PMSGSELWINSTRUCT, MSGSELWINSTRUCT;

//处理重入
static PMSGSELWINSTRUCT pMsgWinHead = NULL; 
/*---------- function Declarations -------------------------------------*/
static LRESULT CALLBACK MsgWinSelProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	   OnCreateMsg(HWND, PMSGSELWINSTRUCT);

/*extern*/static PMSGSELWINSTRUCT FindCurrentMsgWin(HWND hWnd);
/*extern*/static PMSGSELWINSTRUCT DelMsgCurrentWin(HWND hMsgWin);
static BOOL AddMsgWinNode(PMSGSELWINSTRUCT pMsgWinNode, 
                          HWND hMsgWin, HWND hCall, char *szPrompt,
                          PCSTR szOk, PCSTR szCancel, NOTIFYTYPE eTypePic, PCSTR szcaption);
static void FillMsgWinNode(PMSGSELWINSTRUCT pMsgCurrentWin, 
                           HWND hMsgWin, HWND hCall, char *szPrompt,
                           PCSTR szOk, PCSTR szCancel, NOTIFYTYPE eTypePic, PCSTR szcaption);
/*------------------END-------------------------------------------------*/
extern BOOL IsConFirmEx();
extern BOOL IsTipsWin();
extern BOOL IsWaitWnd();
extern BOOL IsSSPassCheck();
/*************************************************************************
  Function   :PLXTipsWin
--------------------------------------------------------------------------
  Description:To show a message box with user specified prompt,caption and 
              time
--------------------------------------------------------------------------
  Input      :szPrompt     ----    address of text in message box
              szCaption	   ----    address of title of message box
              nTimeout	   ----    destroy the window after nTimeout*100 
			                       millisecond.If nTimeout = 0 never destroy
								   this message window.
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
              None.
*************************************************************************/
BOOL PLXConfirmWin(HWND hFrame, HWND hParent, PCSTR szPrompt, NOTIFYTYPE TipsPic, PCSTR szCaption, 
                   PCSTR szOk, PCSTR szCancel )
{
    MSG msg;
	HWND hMsgWin;
	WNDCLASS wc;
    BOOL bRet;
	RECT	rClient;
    PMSGSELWINSTRUCT pMsgCurrentWin;

//	SureIcon(CONFGIF,TipsPic);
	if (!bRegist)
	{
		wc.style         = 0;
		wc.lpfnWndProc   = MsgWinSelProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = WC_MSGSELWIN;	
		if( !RegisterClass(&wc) )
			return FALSE;
       
//        uTimers = 1;
        //bFirstWin = TRUE;
        bRegist = TRUE;
    }

    pMsgCurrentWin = (PMSGSELWINSTRUCT)malloc(sizeof(MSGSELWINSTRUCT));

    if(pMsgCurrentWin == NULL)
    {
        if (!pMsgWinHead)
	    {
            UnregisterClass(WC_MSGSELWIN, NULL);
            bRegist = FALSE;
            return FALSE;
	    }
    }
    memset(pMsgCurrentWin, 0, sizeof(MSGSELWINSTRUCT));


	if (!hFrame) {
		if (szCaption&&(*szCaption!=NULL)) {
			
			hMsgWin = CreateWindow(WC_MSGSELWIN, szCaption,
				WS_POPUP|WS_CAPTION|PWS_STATICBAR,
				
				PLX_WIN_POSITION,
				hParent, NULL, NULL, pMsgCurrentWin);
		}
		else
		{
			hMsgWin = CreateWindow(WC_MSGSELWIN, NULL,
				WS_POPUP|PWS_STATICBAR,
				
				PLX_NOTIFICATON_POS,
				hParent, NULL, NULL, pMsgCurrentWin);
		}
		
	}
	else
	{
		GetClientRect(hFrame, &rClient);
		hMsgWin = CreateWindow(WC_MSGSELWIN, NULL,
                              WS_CHILD,
                              
							  rClient.left,
							  rClient.top,
							  rClient.right - rClient.left,
							  rClient.bottom - rClient.top,
                              hFrame, NULL, NULL, pMsgCurrentWin);
		
	}

	
	if( hMsgWin != NULL )
	{
		bConfirm = TRUE;
		if (!AddMsgWinNode(pMsgCurrentWin, hMsgWin, hParent, 
            (char*)szPrompt, szOk, szCancel,TipsPic, szCaption))
			DestroyWindow(hMsgWin);

	}
	else if (!pMsgWinHead)
	{
        if(pMsgCurrentWin != NULL)
            free(pMsgCurrentWin);
        UnregisterClass(WC_MSGSELWIN, NULL);
        bRegist = FALSE;
        return FALSE;
	}
	ShowWindow(hMsgWin, SW_SHOW);
	if (hFrame) 
	{
		pMsgCurrentWin->hFrameWnd = hFrame;
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");	
		UpdateWindow(hFrame);
		ShowWindow(hFrame, SW_SHOW);
		SetFocus(hMsgWin);
	}
	if(szOk)
	{
		if (hFrame) 
		{
			SendMessage(hFrame,PWM_SETBUTTONTEXT, 1, (LPARAM)szOk);
		}
		else
		{
			SendMessage(hMsgWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_YES,1), (LPARAM)szOk);
		}
	}
	else
	{
		if (hFrame) 
		{
			SendMessage(hFrame,PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		}
	}
	if(szCancel)
	{
		if (hFrame) 
		{
			SendMessage(hFrame,PWM_SETBUTTONTEXT, 0, (LPARAM)szCancel);
		}
		else
		{
			SendMessage(hMsgWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_NO,0), (LPARAM)szCancel);
		}
	}
	else
	{
		if (hFrame) 
		{
			SendMessage(hFrame,PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		}
	}
//	
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == CW_EXITMESSAGE)
        {
            if((HWND)msg.wParam == hMsgWin)
            {
                bRet = (BOOL)msg.lParam;
				SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hMsgWin, 0);
                DestroyWindow(hMsgWin);
				bConfirm = FALSE;
                break;
            }
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);

    }

    if (!pMsgWinHead)
	{
        UnregisterClass(WC_MSGSELWIN, NULL);
        bRegist = FALSE;
	}

    return bRet;
}
/*************************************************************************
  Function   :MsgWinProc
--------------------------------------------------------------------------
  Description:This is MsgWin's message process procudure.
--------------------------------------------------------------------------
  Input      :hWnd         ----    Window's handle
              wMsgCmd	   ----    Message ID
              wParam	   ----    parameter
              lParam       ----    parameter
--------------------------------------------------------------------------
  Return     :TRUE         ----    Success
              FALSE        ----    Falied
--------------------------------------------------------------------------
  IMPORTANT NOTES:
              None.
*************************************************************************/
static LRESULT CALLBACK MsgWinSelProc(HWND hWnd, UINT wMsgCmd, 
                                      WPARAM wParam, LPARAM lParam)
{
	LONG lResult;
    HDC  hdc;
    RECT rcClient;
//    POINT ptMouse;
	PAINTSTRUCT ps;
	PMSGSELWINSTRUCT pMsgCurrentWin;
    PMSGSELWINSTRUCT pCreateData;
    PCREATESTRUCT    pCreateStruct;
    static int       nScreenX, nScreenY;
	HBRUSH			oldbrush;
	HBRUSH			newbrush;
	RECT			rcCapicon;

	pMsgCurrentWin = FindCurrentMsgWin(hWnd);
    lResult   = (LRESULT)TRUE;
    
    switch( wMsgCmd )
    {
		case WM_CREATE:
			PrioMan_CallMusic(PRIOMAN_PRIORITY_WARNING,1);
			DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
			DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
			DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);
            pCreateStruct = (PCREATESTRUCT)lParam;
            pCreateData = (PMSGSELWINSTRUCT)(pCreateStruct->lpCreateParams);
            pCreateData->hMsgWin = hWnd;
			if (!OnCreateMsg( hWnd, pCreateData ))
				return -1;

            nScreenX = pCreateStruct->cx;
            nScreenY = pCreateStruct->cy;
            
            SetCapture( pCreateData->hMsgWin );
			GrabKeyboard(pCreateData->hMsgWin);

            lResult = 0;
			break;

		case PWM_SHOWWINDOW:
			if (!pMsgCurrentWin) {
				SetFocus(hWnd);
				break;
			}
			if (pMsgCurrentWin->szOk) {
				SendMessage(pMsgCurrentWin->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)pMsgCurrentWin->szOk);
			}
			if (pMsgCurrentWin->szCancel) {
				SendMessage(pMsgCurrentWin->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)pMsgCurrentWin->szCancel);
			}
			if (pMsgCurrentWin->szCaption) {
				SetWindowText(pMsgCurrentWin->hFrameWnd, pMsgCurrentWin->szCaption);
			}
			SendMessage(pMsgCurrentWin->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SetFocus(hWnd);
			break;

		case WM_ACTIVATE:
			if (!pMsgCurrentWin) {
				SetFocus(hWnd);
				break;
			}
			if (wParam == WA_ACTIVE)
			{
				bConfirm = TRUE;
				SetCapture( pMsgCurrentWin->hMsgWin );
				GrabKeyboard(pMsgCurrentWin->hMsgWin);
				

				SetFocus(pMsgCurrentWin->hMsgWin);
			}
			
			if (wParam == WA_INACTIVE)
			{
				ReleaseCapture();
				UngrabKeyboard();
				bConfirm = FALSE;
			}
			break;

        case WM_PAINT:
			{
				int nOldMode;
				RECT	rcText;
				HPEN hPen, hOldPen;
				//COLORREF	oldcolor;
           
				hdc = BeginPaint( hWnd, &ps );
				
                /*
                if(pMsgCurrentWin->hBmpScreen != NULL)
                    BitBlt(hdc, 0, 0, nScreenX, nScreenY, 
                    (HDC)pMsgCurrentWin->hBmpScreen, 0, 0, SRCCOPY);
*/
				//---
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

				nOldMode = SetBkMode(hdc, NEWTRANSPARENT);
                DrawImageFromFile(hdc, pMsgCurrentWin->strCongif, 80, 8, ROP_SRC);
               
				SetRect(&rcText, rcClient.left, rcClient.top+31, rcClient.right, rcClient.bottom);
                if(pMsgCurrentWin->sMsgWinPrompt != NULL)
                {
//					SetBkColor(hdc,oldcolor);
                    DrawText(hdc, pMsgCurrentWin->sMsgWinPrompt, -1, &rcText, DT_VCENTER|DT_HCENTER);
                    
                }
				SetBkMode(hdc, nOldMode);
	
                
				EndPaint( hWnd, &ps );

			}
			break;

        case WM_KEYDOWN:
			switch ((int)wParam)
			{
            case VK_F10:
				if (pMsgCurrentWin->szCancel)
				{
					if (pMsgCurrentWin->hFrameWnd) {
						SendMessage(pMsgCurrentWin->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					}
					PostMessage(pMsgCurrentWin->hMsgWin, CW_EXITMESSAGE, (WPARAM)pMsgCurrentWin->hMsgWin, 0);
				}
                break;

			case VK_RETURN:
				if (pMsgCurrentWin->szOk) 
				{
					if (pMsgCurrentWin->hFrameWnd) {
						SendMessage(pMsgCurrentWin->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					}
					PostMessage(pMsgCurrentWin->hMsgWin, 
                    CW_EXITMESSAGE, (WPARAM)pMsgCurrentWin->hMsgWin, 1);
				}
			    
				break;
			}
			break;

		case WM_DESTROY:			
			//if (GetFocus() == pMsgCurrentWin->hMsgWin)
			//{
				ReleaseCapture();
				UngrabKeyboard();
			//}
			PrioMan_EndCallMusic(PRIOMAN_PRIORITY_WARNING, TRUE);
            DelMsgCurrentWin(pMsgCurrentWin->hMsgWin);
			
			break;
        default:
            return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
    return lResult;
}
/*************************************************************************
  Function   :OnCreateMsg
--------------------------------------------------------------------------
  Description:Process window's create message.
--------------------------------------------------------------------------
  Input      :hWnd         ----    Window's handle
--------------------------------------------------------------------------
  Return     :TRUE         ----    Process success
              FALSE        ----    Process falied
--------------------------------------------------------------------------
  IMPORTANT NOTES:
              None.
*************************************************************************/
static BOOL	OnCreateMsg(HWND hWnd, PMSGSELWINSTRUCT pMsgNode)
{

	SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
 

	return TRUE;
}
/*************************************************************************
  Function   :AddMsgWinNode
--------------------------------------------------------------------------
  Description:Create a new node, and fill the struct.
--------------------------------------------------------------------------
  Input      :hCall		   ----    the caller
			  szPrompt     ----    address of text in message box
              szCaption	   ----    address of title of message box
              nTimeout	   ----    destroy the window after nTimeout*1000 
			                       millisecond.If nTimeout = 0 never destroy
								   this message window.
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static BOOL AddMsgWinNode(PMSGSELWINSTRUCT pMsgWinNode, HWND hMsgWin, 
               HWND hCall, char *szPrompt, PCSTR szOk, PCSTR szCancel,NOTIFYTYPE eTypePic, PCSTR szcaption)
{
	if (!pMsgWinNode) 
		return FALSE;

	FillMsgWinNode(pMsgWinNode, hMsgWin, 
        hCall, szPrompt, szOk, szCancel, eTypePic, szcaption);
	
	if (!pMsgWinHead)
		pMsgWinHead = pMsgWinNode; 
	else
	{
		pMsgWinNode->next = pMsgWinHead;
		pMsgWinHead = pMsgWinNode; 
	}

	return TRUE;
}

/*************************************************************************
  Function   :FillMsgWinNode
--------------------------------------------------------------------------
  Description:Create a new node, and fill the struct.
--------------------------------------------------------------------------
  Input      :hCall		   ----    the caller
			  szPrompt     ----    address of text in message box
              szCaption	   ----    address of title of message box
              nTimeout	   ----    destroy the window after nTimeout*100 
			                       millisecond.If nTimeout = 0 never destroy
								   this message window.
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static void FillMsgWinNode(PMSGSELWINSTRUCT pMsgCurrentWin, 
                           HWND hMsgWin, HWND hCall, 
						   char *szPrompt, PCSTR szOk, PCSTR szCancel, NOTIFYTYPE eTypePic, PCSTR szcaption)
{
 //   int nNumPerLine;
 //   int nLineCount;
//    int nCharWidth;
    SIZE size;
    HDC hdc;
    
    hdc = GetDC(hMsgWin);
	SureIcon(pMsgCurrentWin->strCongif, eTypePic);
    GetTextExtentPoint32(hdc, "T", -1, &size);
    //nCharWidth = size.cx;
//    nNumPerLine = (TRect.right - TRect.left+20) / nCharWidth;
//    nLineCount = (TRect.bottom - TRect.top) / size.cy;

    ReleaseDC(hMsgWin, hdc);

    if(szPrompt != NULL)
    {
        pMsgCurrentWin->sMsgWinPrompt = 
            (char*)malloc(strlen(szPrompt)+strlen(szPrompt)/5+5);
		
        if(pMsgCurrentWin->sMsgWinPrompt == NULL)
            return;
//		if (strchr(szPrompt, '\n')) {
//			
//			strcpy(pMsgCurrentWin->sMsgWinPrompt,szPrompt);
//		}
//		else
			pMsgCurrentWin->Format = 
            FormatStr(pMsgCurrentWin->sMsgWinPrompt, szPrompt);
    }
    else
        pMsgCurrentWin->sMsgWinPrompt = NULL;

    //
	pMsgCurrentWin->hMsgWin = hMsgWin;	    //
	pMsgCurrentWin->hCall = hCall;			//
	if(szOk)
	{
		pMsgCurrentWin->szOk = (PSTR)malloc(strlen(szOk)+1);
    
		if(pMsgCurrentWin->szOk == NULL)
			return;
		strcpy(pMsgCurrentWin->szOk, szOk);
    }
	if(szCancel)
	{
		pMsgCurrentWin->szCancel = (PSTR)malloc(strlen(szCancel)+1);
		                          
		if(pMsgCurrentWin->szCancel == NULL)
			return;
		strcpy(pMsgCurrentWin->szCancel, szCancel);
	}
	if (szcaption) 
	{
		pMsgCurrentWin->szCaption = malloc(strlen(szcaption)+1);
		if(pMsgCurrentWin->szCaption == NULL)
			return;
		strcpy(pMsgCurrentWin->szCaption, szcaption);
	}
	pMsgCurrentWin->next = NULL;			//
}

/*************************************************************************
  Function   :FindCurrentMsgWin
--------------------------------------------------------------------------
  Description:Find the list node by the given handle.
--------------------------------------------------------------------------
  Input      :hWnd ----  handle
--------------------------------------------------------------------------
  Return     :the node .
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static PMSGSELWINSTRUCT FindCurrentMsgWin(HWND hWnd)
{
	PMSGSELWINSTRUCT pTempMsgWin;

	pTempMsgWin = pMsgWinHead;
	while (pTempMsgWin)
	{
		if (pTempMsgWin->hMsgWin == hWnd)
			return pTempMsgWin;
		pTempMsgWin = pTempMsgWin->next;
	}
	return NULL;
}
/*************************************************************************
  Function   :DelMsgCurrentWin
--------------------------------------------------------------------------
  Description:Delete the list node about current window.
--------------------------------------------------------------------------
  Input      :hMsgWin ----    the current window handle
--------------------------------------------------------------------------
  Return     :the list head pointer.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static PMSGSELWINSTRUCT DelMsgCurrentWin(HWND hMsgWin)
{
	PMSGSELWINSTRUCT pDelMsgWin, pPreviouMsgWin;

	pDelMsgWin = pMsgWinHead;
	pPreviouMsgWin = pDelMsgWin;

	while (pDelMsgWin)
	{
		if (pDelMsgWin->hMsgWin == hMsgWin)
		{
			if (pDelMsgWin == pMsgWinHead)
				pMsgWinHead = pDelMsgWin->next;
			else
				pPreviouMsgWin->next = pDelMsgWin->next;

            if(pDelMsgWin->szOk != NULL)
                free(pDelMsgWin->szOk);
                //Free(pDelMsgWin->szOk);
            if(pDelMsgWin->szCancel != NULL)
                free(pDelMsgWin->szCancel);
                //Free(pDelMsgWin->szCancel);
            if(pDelMsgWin->sMsgWinPrompt != NULL)
                free(pDelMsgWin->sMsgWinPrompt);
			if (pDelMsgWin->szCaption)
			{
				free(pDelMsgWin->szCaption);
			}
                //Free(pDelMsgWin->sMsgWinPrompt);
            //For grayed screen
            //SelectObject(pDelMsgWin->hCmpDC, pDelMsgWin->hOldBmp);
            //DeleteObject(pDelMsgWin->hBmpScreen);
            //DeleteDC(pDelMsgWin->hCmpDC);
            //DeleteDC(pDelMsgWin->hdcScreen);
            
            free(pDelMsgWin);
            //Free(pDelMsgWin);

			return pMsgWinHead;
		}
		pPreviouMsgWin = pDelMsgWin;
		pDelMsgWin = pDelMsgWin->next;
	}
	return NULL;
}

/*************************************************************************
  Function   :FormatStr
--------------------------------------------------------------------------
  Description:Format the prompt string.
--------------------------------------------------------------------------
  Input      :szMsgWinPrompt ----    string formatted
              szPrompt       ----    prompt string
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
DWORD FormatStr(char* szMsgWinPrompt, char* szPrompt)
{
	int i = 0, Offset,x;
	PSTR	ptr, pNewLine;
	char	tmpbuf[30];
	BOOL	bCut = FALSE;
	*szMsgWinPrompt = 0;
	
	if (!szPrompt)
	{
		return DT_CENTER | DT_VCENTER;
	}
	x = (int)strlen(szPrompt);

	if (x == 0)
	{
		strcpy(szMsgWinPrompt, "");
		return DT_CENTER | DT_VCENTER;
	}

	GetTextExtentExPoint(NULL, szPrompt, strlen(szPrompt), 160, &Offset,NULL,NULL);
	if (Offset == x) {
		strcpy(szMsgWinPrompt, szPrompt);
		return DT_CENTER | DT_VCENTER; 
	}
	do {
		
		//should consider the condition that the symbol '\n' exist in szPrompt.?????
		GetTextExtentExPoint(NULL, szPrompt, strlen(szPrompt), 160, &Offset,NULL,NULL);
		memset(tmpbuf, 0, 30);
		strncpy(tmpbuf,szPrompt,Offset);		
		ptr = szPrompt+Offset;
		if (pNewLine = strchr(tmpbuf, '\n')) {
			//strncpy(tmpbuf, szPrompt, pNewLine - tmpbuf);
			tmpbuf[pNewLine - tmpbuf] = '\0';
			strcat(szMsgWinPrompt, tmpbuf);
			x = strlen(szMsgWinPrompt);
			szMsgWinPrompt[x] = '\n';
			szMsgWinPrompt[x+1] = '\0';
			szPrompt = pNewLine - tmpbuf + szPrompt + 1;
			continue;
		}
		if ((*ptr == ' ')||(*(ptr-1) == ' ')||(*ptr == NULL)||(!strchr(tmpbuf,' '))) 
		{
			strcat(szMsgWinPrompt, tmpbuf);
			x = strlen(szMsgWinPrompt);
			szMsgWinPrompt[x] = '\n';
			szMsgWinPrompt[x+1] = '\0';
			szPrompt = szPrompt + Offset;
			if (*ptr == NULL) {
				break;
			}
		}
		else
		{
			for (i = Offset; i>0; i--) 
			{
				if (*(szPrompt + i) == ' ') 
				{
					memset(tmpbuf, 0, 30);
					strncpy(tmpbuf,szPrompt,i);
					strcat(szMsgWinPrompt, tmpbuf);
					x = strlen(szMsgWinPrompt);
					szMsgWinPrompt[x] = '\n';
					szMsgWinPrompt[x+1] = '\0';
					szPrompt = szPrompt + i+1;
					break;
				}
			}
			if (*(szPrompt+1) == NULL) {
				break;
			}
		}
		
		
	}while (Offset !=0 ) ;
	
	return DT_CENTER | DT_VCENTER;

}

void SureIcon(char* GifSign,NOTIFYTYPE TipsPic)
{
	if (!GifSign)
	{
		return;
	}
	GifSign[0] = 0;
	switch(TipsPic) 
	{
	case Notify_Info:
		strcpy(GifSign , "/rom/public/info.bmp");
		break;
	case Notify_Failure:
		strcpy(GifSign , "/rom/public/failure.bmp");
		break;
	case Notify_Success:
		strcpy(GifSign , "/rom/public/success.bmp");
		break;
	case Notify_Alert:
		strcpy(GifSign , "/rom/public/alert.bmp");
		break;
	case Notify_Request:
		strcpy(GifSign , "/rom/public/request.bmp");
		break;
	case Notify_Wait:
		strcpy(GifSign, "/rom/public/waiting.bmp");
		break;
	default:
		strcpy(GifSign , "/rom/public/info.bmp");;
		break;
	}
}
BOOL	TopWndIsNotification()
{
	PMSGSELWINSTRUCT	pMsgSelWin;
	pMsgSelWin = pMsgWinHead;
	while (pMsgSelWin)
	{
		if (IsWindow(pMsgSelWin->hMsgWin))
		{
			bConfirm = TRUE;
			return	TRUE;
		}
		else
			pMsgSelWin = pMsgSelWin->next;
	}
	bConfirm = FALSE;

	return bConfirm |IsWaitWnd() |IsConFirmEx()|IsTipsWin()|IsSSPassCheck();
}
/*************************End of MsgWin.c*********************************/
/*void Setup_PlayRing(int a, BOOL b)
{
	return;
}
*/
/*static COLORREF SetupGetDeskColor(int iIndex)
{
	return RGB(0,0,255);
}*/

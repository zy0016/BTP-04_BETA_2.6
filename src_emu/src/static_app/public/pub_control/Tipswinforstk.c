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
#include "winpda.h"
#include "setting.h"
#include "hpimage.h"
//#include "hpdebug.h"
#include "prioman.h"
#include "pubapp.h"
#include "mullang.h"
#ifndef _TIPSWIN_
#define _TIPSWIN_
#endif
/*---------- Global Definitions and Declarations -----------------------*/
/* MsgWin's  handle */
//static HWND	hCallWnd = NULL;
/*---------- Constant / Macro Definitions ------------------------------*/
#define        CW_EXITMESSAGE                WM_USER+100
#define        WC_MSGWIN                     "MSGWINFORSTK"

#define			ID_YES	1301
#define			ID_NO	1302
/*---------- Type Declarations -----------------------------------------*/
static BOOL		bRegist = FALSE;
//static UINT     uTimers = 2;
static	BOOL bTipsWind = FALSE;
static RECT     rcPrompt = {0, 0, 170, 150}; //{80, 120, 210, 180};

//创建提示窗口的储存结构
typedef struct tagMsgwinStruct
{
	char		strCongif[50];
	HWND        hMsgwin;
	HWND        hCall;
	HWND		hFrame;
    BOOL        bPenDown;
    BOOL        bInBtn;
    RECT        rcBtn;
    int         nMsgWinTimeout;
    char*       sMsgWinPrompt;
    //HDC         hdcScreen;
    //HDC         hCmpDC;
    //HBITMAP     hBmpScreen;
    //HBITMAP     hOldBmp;
    UINT        uTimerID;
	UINT		rMsg;
	PSTR		szOk;
	PSTR		szCancel;
	PSTR		szCaption;
    struct tagMsgwinStruct *next;
}*PMSGWINSTRUCT, MSGWINSTRUCT;

//处理重入
static PMSGWINSTRUCT pMsgWinHead = NULL;
/*---------- function Declarations -------------------------------------*/
static LRESULT CALLBACK MsgWinProc(HWND, UINT, WPARAM, LPARAM);
static BOOL	   OnCreateMsg(HWND, PMSGWINSTRUCT);
/*extern*/static PMSGWINSTRUCT FindCurrentMsgWin(HWND hWnd);
/*extern*/static PMSGWINSTRUCT DelMsgCurrentWin(HWND hMsgWin);
static BOOL AddMsgWinNode(PMSGWINSTRUCT pMsgWinNode, 
                          HWND hMsgWin, HWND hCall,char *szPrompt, 
                          int nTimeout, NOTIFYTYPE eTypePic, UINT rMsg, PCSTR szCaption);
static void FillMsgWinNode(PMSGWINSTRUCT pMsgCurrentWin, 
                           HWND hMsgWin, HWND hCall,char *szPrompt, 
                           int nTimeout, NOTIFYTYPE eTypePic, UINT	rMsg);
BOOL IsTipsWin();
/*------------------END-------------------------------------------------*/

/*************************************************************************
  Function   :PLXTipsWin
--------------------------------------------------------------------------
  Description:To show a message box with user specified prompt,caption and 
              time
--------------------------------------------------------------------------
  Input      :szPrompt     ----    address of text in message box
              Tipspic	   ----    icon of the notification window
              nTimeout	   ----    destroy the window after nTimeout*100 
			                       millisecond.If nTimeout = 0 never destroy
								   this message window.
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
              None.
*************************************************************************/
void PLXTipsWinForSTK(HWND hFrame, HWND hParent, UINT	rMsg, PCSTR szPrompt,PCSTR szCaption,NOTIFYTYPE Tipspic,PCSTR szOK, PCSTR szCancel, int nTimeout )
{
    
    HWND hMsgWin;
	WNDCLASS wc;
    PMSGWINSTRUCT pMsgCurrentWin;
	RECT	rc;
	
	if (!bRegist)
	{
		wc.style         = 0;
		wc.lpfnWndProc   = MsgWinProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = WC_MSGWIN;	
		if( !RegisterClass(&wc) )
			return;
        
        //uTimers = 2;
        
        bRegist = TRUE;
    }

    pMsgCurrentWin = (PMSGWINSTRUCT)malloc(sizeof(MSGWINSTRUCT));
    //pMsgCurrentWin = (PMSGWINSTRUCT)malloc(sizeof(MSGWINSTRUCT));
    if(pMsgCurrentWin == NULL)
    {
        if (!pMsgWinHead)
	    {
            UnregisterClass(WC_MSGWIN, NULL);
            bRegist = FALSE;
            return;
	    }
    }
    memset(pMsgCurrentWin, 0, sizeof(MSGWINSTRUCT));

	if (hFrame) {
		GetClientRect(hFrame, &rc);
		hMsgWin = CreateWindow(WC_MSGWIN, NULL,
			WS_CHILD,			
			rc.left,
			rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			hFrame, NULL, NULL, pMsgCurrentWin);
		if (szCaption) {
			SetWindowText(hFrame, szCaption);
		}
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
	}
	else{
		if (szCaption&&(*szCaption != NULL)) 
		{
			hMsgWin = CreateWindow(WC_MSGWIN, szCaption,
                              PWS_STATICBAR|WS_CAPTION,
		                      
							  PLX_WIN_POSITION,
		                      hParent, NULL, NULL, pMsgCurrentWin);
		}
		else
		{
			hMsgWin = CreateWindow(WC_MSGWIN, NULL,
                              PWS_STATICBAR,
		                      
							  PLX_NOTIFICATON_POS,
		                      hParent, NULL, NULL, pMsgCurrentWin);
		}
		
	}
	
	pMsgCurrentWin->hFrame = hFrame;


	if (hFrame) 
	{
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		

	}
	
	if( hMsgWin != NULL )
	{
		//bTipsWind = TRUE;
		AddMsgWinNode(pMsgCurrentWin, hMsgWin, 
            hParent, (char *)szPrompt, nTimeout, Tipspic, rMsg, szCaption);		
	}
	else if (!pMsgWinHead)
	{
        if(pMsgCurrentWin != NULL)
            free(pMsgCurrentWin);
            //Free(pMsgCurrentWin);
        UnregisterClass(WC_MSGWIN, NULL);
        bRegist = FALSE;
        return;
	}
	//避免edit将button改变
	ShowWindow(hMsgWin, SW_SHOW);
	SetFocus(hMsgWin);
	
	if(szOK)
	{
		pMsgCurrentWin->szOk = malloc(strlen(szOK)+1);
		strcpy(pMsgCurrentWin->szOk, szOK);
		if(hFrame)
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)szOK);
		else
			SendMessage(hMsgWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_YES,1), (LPARAM)szOK);

	}
	else
	{
		pMsgCurrentWin->szOk = NULL;
		if (hFrame) {
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		}
	}
	if(szCancel)
	{
		pMsgCurrentWin->szCancel = malloc(strlen(szCancel)+1);
		strcpy(pMsgCurrentWin->szCancel, szCancel);
		if (hFrame) 
		{
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)szCancel);
		}
		else
			SendMessage(hMsgWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_NO,0), (LPARAM)szCancel);
	}
	else
	{
		pMsgCurrentWin->szCancel = NULL;
		if (hFrame) {
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		}
	}
	
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
static LRESULT CALLBACK MsgWinProc(HWND hWnd, UINT wMsgCmd, 
                                   WPARAM wParam, LPARAM lParam)
{
	LONG lResult;
    HDC hdc;

	RECT rcClient;
	RECT rcCapicon;
	HBRUSH			oldbrush;
	HBRUSH			newbrush;
	PAINTSTRUCT ps;
	PMSGWINSTRUCT pMsgCurrentWin;
    PMSGWINSTRUCT pCreateData;
    PCREATESTRUCT    pCreateStruct;
	POINT	ptMouse;


	pMsgCurrentWin = FindCurrentMsgWin(hWnd);
    lResult   = (LRESULT)TRUE;
    
    switch( wMsgCmd )
    {
		case WM_CREATE:
            pCreateStruct = (PCREATESTRUCT)lParam;
            pCreateData = (PMSGWINSTRUCT)(pCreateStruct->lpCreateParams);
            pCreateData->hMsgwin = hWnd;
			if (!OnCreateMsg( hWnd, pCreateData ))
				return -1;            

//            SetCapture( pCreateData->hMsgwin );
		    GrabKeyboard(pCreateData->hMsgwin);
			PrioMan_CallMusic(33,1);

			DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
			DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
			DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);

            lResult = 0;
			bTipsWind = TRUE;
			break;

		case PWM_SHOWWINDOW:
			if (!pMsgCurrentWin) {
				return FALSE;
			}
			if (pMsgCurrentWin->szOk) {
				SendMessage(pMsgCurrentWin->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)pMsgCurrentWin->szOk);
			}
			if (pMsgCurrentWin->szCancel) {
				SendMessage(pMsgCurrentWin->hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)pMsgCurrentWin->szCancel);
			}
			if (pMsgCurrentWin->szCaption) {
				SetWindowText(pMsgCurrentWin->hFrame,pMsgCurrentWin->szCaption);
			}
			SendMessage(pMsgCurrentWin->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SendMessage(pMsgCurrentWin->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
			SendMessage(pMsgCurrentWin->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
			SetFocus(hWnd);
			break;

		case WM_ACTIVATE:
			if (wParam == WA_ACTIVE)
			{
//				SetCapture( pMsgCurrentWin->hMsgwin );
				GrabKeyboard(pMsgCurrentWin->hMsgwin);
				SetFocus(pMsgCurrentWin->hMsgwin);
				//bTipsWind = TRUE;

//                pMsgCurrentWin->bPenDown = FALSE;
//                pMsgCurrentWin->bInBtn = FALSE;
            }
			else if (wParam == WA_INACTIVE)
			{
				//				ReleaseCapture();
				UngrabKeyboard();

				//if (pMsgCurrentWin->hFrame) {
				//	SendMessage(pMsgCurrentWin->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				//}
				//bTipsWind = FALSE;
				//PostMessage(pMsgCurrentWin->hCall, pMsgCurrentWin->rMsg, 0, 2);
				//PostMessage(hWnd, WM_CLOSE, 0, 0);
				
			}
			break;
		
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

        case WM_PAINT:
			{
				int nOldMode;
                char	*szDisplay = NULL;
				HPEN	hPen, hOldPen;
				
				hdc  = BeginPaint( hWnd, &ps );
			
                //提示文字
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
				if(DrawImageFromFile(hdc, pMsgCurrentWin->strCongif, 80, 8, ROP_SRC))
					printf("\r\nsucceded to drawimage %s", pMsgCurrentWin->strCongif);
				else
					printf("\r\nfailed to drawimage %s", pMsgCurrentWin->strCongif);
				
				rcClient.top += 31;
                if(pMsgCurrentWin->sMsgWinPrompt != NULL)
                {
                    szDisplay = malloc(strlen(pMsgCurrentWin->sMsgWinPrompt)*2 +1);
					if (szDisplay) 
					{
						FormatStr(szDisplay, pMsgCurrentWin->sMsgWinPrompt);
						DrawText(hdc, szDisplay, strlen(szDisplay), 
							&rcClient, DT_VCENTER|DT_CENTER);
						free(szDisplay);
					}
					else
					{
						DrawText(hdc, pMsgCurrentWin->sMsgWinPrompt, 
							strlen(pMsgCurrentWin->sMsgWinPrompt), 
							&rcClient, DT_VCENTER|DT_CENTER);
					}
					
                    
                }
				SetBkMode(hdc, nOldMode);
				
				EndPaint( hWnd, &ps );
			}
			break;

		case WM_KEYDOWN:
			switch (LOWORD(wParam))
			{
			case VK_RETURN:
				if (pMsgCurrentWin->szOk) 
				{				
					if (pMsgCurrentWin->hFrame) {
						SendMessage(pMsgCurrentWin->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					}
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					PostMessage(pMsgCurrentWin->hCall, pMsgCurrentWin->rMsg, 0, 1);
					
					
				}
				break;

			case VK_F2:
				if (pMsgCurrentWin->hFrame) {
					SendMessage(pMsgCurrentWin->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				}
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				PostMessage(pMsgCurrentWin->hCall, pMsgCurrentWin->rMsg, 0, 3);
				break;

			case VK_F10:
				if (pMsgCurrentWin->szCancel) {
					
					if (pMsgCurrentWin->hFrame) {
						SendMessage(pMsgCurrentWin->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					}
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					PostMessage(pMsgCurrentWin->hCall, pMsgCurrentWin->rMsg, 0, 0);
					
				}
				break;
			}
			break;

		case WM_DESTROY:
			if( pMsgCurrentWin->nMsgWinTimeout > 0 )
			{
				KillTimer( pMsgCurrentWin->hMsgwin, 1 );
				pMsgCurrentWin->nMsgWinTimeout = -1;
					
			}

//			ReleaseCapture();
			UngrabKeyboard();

			
            DelMsgCurrentWin(pMsgCurrentWin->hMsgwin);
            
            if (!pMsgWinHead)
	        {
                UnregisterClass(WC_MSGWIN, NULL);
                bRegist = FALSE;
	        }
			PrioMan_EndCallMusic(33, TRUE);
			bTipsWind = FALSE;
			break;
			
	

		case WM_TIMER:
            {
                UINT uTimerID = (UINT)wParam;

                switch(uTimerID)
                {
                case 1: //计时timer
                    {
                       //printf("in msgwin timer time arrive\r\n");
						if (pMsgCurrentWin->hFrame) 
						{
							
							SendMessage(pMsgCurrentWin->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
						}
						PostMessage(hWnd, WM_CLOSE, 0, 0);
						PostMessage(pMsgCurrentWin->hCall, pMsgCurrentWin->rMsg, 0, 2);
                        
                        
                    }
                    break;

				default:
					lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
					break;

                } //switch
                
            }
			break;
#if 1
        case WM_PENDOWN:
			if( pMsgCurrentWin->nMsgWinTimeout > 0 )
				break;
			ptMouse.x    = LOWORD(lParam); 
			ptMouse.y    = HIWORD(lParam); 
			if( PtInRect( &pMsgCurrentWin->rcBtn, ptMouse ) )
			{
				pMsgCurrentWin->bInBtn   = TRUE; 
				pMsgCurrentWin->bPenDown = TRUE;
			}
			else
			{
				pMsgCurrentWin->bInBtn   = FALSE;
				pMsgCurrentWin->bPenDown = FALSE;
			}
			break;

        case WM_PENMOVE:
			if( pMsgCurrentWin->nMsgWinTimeout > 0 )
				break;
			ptMouse.x    = LOWORD(lParam);  
			ptMouse.y    = HIWORD(lParam);  
			if( !PtInRect( &pMsgCurrentWin->rcBtn, ptMouse ) && 
                pMsgCurrentWin->bInBtn)
			{
				pMsgCurrentWin->bInBtn   = FALSE;
				pMsgCurrentWin->bPenDown = TRUE;
			}
			if (PtInRect( &pMsgCurrentWin->rcBtn, ptMouse) && 
                pMsgCurrentWin->bPenDown && !pMsgCurrentWin->bInBtn)
			{
				pMsgCurrentWin->bInBtn   = TRUE;
			}
			break;

        case WM_PENUP:
			if( pMsgCurrentWin->nMsgWinTimeout > 0 )
				break;
			ptMouse.x    = LOWORD(lParam);  
			ptMouse.y    = HIWORD(lParam);  
			if( PtInRect( &pMsgCurrentWin->rcBtn, ptMouse ) && 
                pMsgCurrentWin->bInBtn )
			{
                PostMessage(pMsgCurrentWin->hMsgwin, CW_EXITMESSAGE, 
                    (WPARAM)pMsgCurrentWin->hMsgwin, 0);
			}
            pMsgCurrentWin->bPenDown = FALSE;
            pMsgCurrentWin->bInBtn = FALSE;

			break;
#endif
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
static BOOL	OnCreateMsg(HWND hWnd, PMSGWINSTRUCT pMsgNode)
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
              nTimeout	   ----    destroy the window after nTimeout*1000 
			                       millisecond.If nTimeout = 0 never destroy
								   this message window.
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static BOOL AddMsgWinNode(PMSGWINSTRUCT pMsgWinNode, 
                          HWND hMsgWin, HWND hCall, 
                          char *szPrompt, int nTimeout, NOTIFYTYPE eTypePic, UINT rMsg, PCSTR szCaption)
{
	if (!pMsgWinNode) 
		return FALSE;

	if( nTimeout > 0 )	//set timeout
	{
		if (SetTimer( hMsgWin, 1, nTimeout * 100, NULL ) == 0)
			nTimeout = 0;
	}
	if (szCaption) 
	{
		pMsgWinNode->szCaption = malloc(strlen(szCaption)+1);
		if (!pMsgWinNode->szCaption) 
		{
			return FALSE;
		}
		strcpy(pMsgWinNode->szCaption,szCaption);
	}

	FillMsgWinNode(pMsgWinNode, hMsgWin, hCall, 
        szPrompt, nTimeout, eTypePic, rMsg);
	
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

              nTimeout	   ----    destroy the window after nTimeout*100 
			                       millisecond.If nTimeout = 0 never destroy
								   this message window.
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
static void FillMsgWinNode(PMSGWINSTRUCT pMsgCurrentWin,
                           HWND hMsgWin, HWND hCall, 
						   char *szPrompt, int nTimeout, NOTIFYTYPE eTypePic, UINT rMsg)
{

    ///////////////////////////
   
    pMsgCurrentWin->nMsgWinTimeout = nTimeout;	
	SureIcon(pMsgCurrentWin->strCongif, eTypePic);
	
	pMsgCurrentWin->hMsgwin = hMsgWin;	    //handle of this wnd
	pMsgCurrentWin->hCall = hCall;			//window handle of calling
	pMsgCurrentWin->rMsg = rMsg;

    if(szPrompt != NULL)
    {
        pMsgCurrentWin->sMsgWinPrompt = 
                            (char*)malloc(strlen(szPrompt)+1);
        //pMsgCurrentWin->sMsgWinPrompt = (char*)malloc(strlen(szPrompt)+nLineCount*2);
        if(pMsgCurrentWin->sMsgWinPrompt == NULL)
            return;
//		if (strchr(szPrompt, '\n')) {
//			strcpy(pMsgCurrentWin->sMsgWinPrompt,szPrompt);
//		}
//		else
		if (szPrompt)
		{
			strcpy(pMsgCurrentWin->sMsgWinPrompt,szPrompt);
		}
		else
			strcpy(pMsgCurrentWin->sMsgWinPrompt,"");
			//FormatStr(pMsgCurrentWin->sMsgWinPrompt, szPrompt);
        
    }
    else
        pMsgCurrentWin->sMsgWinPrompt = NULL;
    
	pMsgCurrentWin->next = NULL;			//tail
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
static PMSGWINSTRUCT FindCurrentMsgWin(HWND hWnd)
{
	PMSGWINSTRUCT pTempMsgWin;

	pTempMsgWin = pMsgWinHead;
	while (pTempMsgWin)
	{
		if (pTempMsgWin->hMsgwin == hWnd)
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
static PMSGWINSTRUCT DelMsgCurrentWin(HWND hMsgWin)
{
	PMSGWINSTRUCT pDelMsgWin, pPreviouMsgWin;

	pDelMsgWin = pMsgWinHead;
	pPreviouMsgWin = pDelMsgWin;

	while (pDelMsgWin)
	{
		if (pDelMsgWin->hMsgwin == hMsgWin)
		{
			if (pDelMsgWin == pMsgWinHead)
				pMsgWinHead = pDelMsgWin->next;
			else
				pPreviouMsgWin->next = pDelMsgWin->next;

            if(pDelMsgWin->sMsgWinPrompt != NULL)
                free(pDelMsgWin->sMsgWinPrompt);

			if (pDelMsgWin->szOk)
			{
				free(pDelMsgWin->szOk);
			}
			if (pDelMsgWin->szCancel)
			{
				free(pDelMsgWin->szCancel);
			}
			if (pDelMsgWin->szCaption)
			{
				free(pDelMsgWin->szCaption);
			}
                //Free(pDelMsgWin->sMsgWinPrompt);
            
            /*if(pDelMsgWin->hCmpDC)
            {
                SelectObject(pDelMsgWin->hCmpDC, pDelMsgWin->hOldBmp);
                DeleteObject(pDelMsgWin->hBmpScreen);
                DeleteDC(pDelMsgWin->hCmpDC);
                //DeleteDC(pDelMsgWin->hdcScreen);
            }
			*/
			free(pDelMsgWin);
			//Free(pDelMsgWin);

			return pMsgWinHead;
		}
		pPreviouMsgWin = pDelMsgWin;
		pDelMsgWin = pDelMsgWin->next;
	}
	return NULL;
}

BOOL IsTipsWinForSTK()
{
	PMSGWINSTRUCT pMsgWin = NULL;
	pMsgWin = pMsgWinHead;
	while (pMsgWin)
	{
		if (IsWindow(pMsgWin->hMsgwin))
		{
			return TRUE;
		}
		else
			pMsgWin = pMsgWin->next;
	}
	return FALSE;
}

BOOL	UpdateTipswinSTKPrompt(PCSTR pBuf)
{
	if (!pBuf)
	{
		return FALSE;
	}
	if (!pMsgWinHead)
	{
		return FALSE;
	}
	pMsgWinHead->sMsgWinPrompt = realloc(pMsgWinHead->sMsgWinPrompt, strlen(pBuf)+1);
	strcpy(pMsgWinHead->sMsgWinPrompt, pBuf);
	InvalidateRect(pMsgWinHead->hMsgwin, NULL, TRUE);
	return TRUE;
}
/*************************End of MsgWin.c*********************************/
/*void Setup_PlayRing(int a, BOOL b)
{
	return;
}

static COLORREF SetupGetDeskColor(int iIndex)
{
	return RGB(0,0,255);
}*/

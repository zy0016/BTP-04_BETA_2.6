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
#define        WC_MSGWIN                     "MsgWinClass"
#define        GRAYRATE                      2 / 3

#define        MSGWIN_HEIGHT                 88  //101
#define        MSGWIN_WIDTH                  110 //201
#define        MSGWIN_LEFT                   55  //19
#define        MSGWIN_TOP                    44    //109
#define        GIFLEFT                       30 //45
#define        GIFTOP                        121 //6
#define        GIFWIDTH                      40
#define        GIFHEIGHT                     43

#define        STRINTERVAL                   5
#define        BTNINTERVAL                   5

#define        TIPSDLGGIF        "/rom/public/tipswindlg.gif"
#define        TIPSGIF           "/rom/public/normal.gif"

#define			ID_YES	1001
#define			ID_NO	1002
/*---------- Type Declarations -----------------------------------------*/
static BOOL		bRegist = FALSE;
//static UINT     uTimers = 2;

static RECT     rcPrompt = {0, 0, 170, 150}; //{80, 120, 210, 180};

//������ʾ���ڵĴ���ṹ
typedef struct tagMsgwinStruct
{
	char		strCongif[50];
	HWND        hMsgwin;
	HWND        hCall;
	HWND		hFrame;
//	BOOL        bTimerOn;
    BOOL        bPenDown;
    BOOL        bInBtn;
//	DWORD       Format;
    RECT        rcBtn;
//    HWND        hWndGrayScreen;
    int         nMsgWinTimeout;
    char*       sMsgWinPrompt;
//	HGIFANIMATE hGifDlg;
    //HGIFANIMATE hGif;
    
	UINT		rMsg;
	PSTR		szOk;
	PSTR		szCancel;
	PSTR		szCaption;
    struct tagMsgwinStruct *next;
}*PMSGWINSTRUCT, MSGWINSTRUCT;

//��������
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
void PLXTipsWinOem(HWND hFrame, HWND hParent, UINT	rMsg, PCSTR szPrompt,PCSTR szCaption,NOTIFYTYPE Tipspic,PCSTR szOK, PCSTR szCancel, int nTimeout )
{
    //MSG msg;
    HWND hMsgWin;
	WNDCLASS wc;
    //int nScreenX, nScreenY;
    PMSGWINSTRUCT pMsgCurrentWin;
//	RECT	rc;
	
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
	hMsgWin = CreateWindow(WC_MSGWIN, NULL,
		PWS_STATICBAR,		                      
		0, 0, 176, 220,
		hParent, NULL, NULL, pMsgCurrentWin);
	
	
	pMsgCurrentWin->hFrame = hFrame;
	if(szOK)
	{
		pMsgCurrentWin->szOk = malloc(strlen(szOK)+1);
		strcpy(pMsgCurrentWin->szOk, szOK);
		SendMessage(hMsgWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_YES,1), (LPARAM)szOK);

	}
	if(szCancel)
	{
		pMsgCurrentWin->szCancel = malloc(strlen(szCancel)+1);
		strcpy(pMsgCurrentWin->szCancel, szCancel);
		SendMessage(hMsgWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_NO,0), (LPARAM)szCancel);
	}
	
	
	if( hMsgWin != NULL )
	{
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
	
	ShowWindow(hMsgWin, SW_SHOW);
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
    POINT ptMouse;
    PCREATESTRUCT    pCreateStruct;
//    HPEN             hPen, hOldPen;
    //static int       nDlgLeft, nDlgTop;
    static int       nScreenX, nScreenY;

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

            nScreenX = pCreateStruct->cx;
            nScreenY = pCreateStruct->cy;
            //nDlgLeft = nScreenX / 2 - MSGWIN_WIDTH / 2;
            //nDlgTop  = nScreenY / 2 - MSGWIN_HEIGHT / 2;

			PrioMan_CallMusic(PRIOMAN_PRIORITY_WARNING,1);
			
			DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
			DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
			DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);
			
            SetCapture( pCreateData->hMsgwin );
		    GrabKeyboard(pCreateData->hMsgwin);

//            pCreateData->uTimerID = SetTimer(hWnd, uTimers, 0, NULL);
            //uTimers++;

            lResult = 0;
			break;


		case WM_ACTIVATE:
			if (wParam == WA_ACTIVE)
			{
				SetCapture( pMsgCurrentWin->hMsgwin );
				GrabKeyboard(pMsgCurrentWin->hMsgwin);
				SetFocus(pMsgCurrentWin->hMsgwin);

                pMsgCurrentWin->bPenDown = FALSE;
                pMsgCurrentWin->bInBtn = FALSE;
            }
			else if (wParam == WA_INACTIVE)
			{
				ReleaseCapture();
				UngrabKeyboard();
			}
			break;
		
		case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

        case WM_PAINT:
			{
				int nOldMode;
                RECT	rCaptext, rGps;
				HPEN hPen, hOldPen;
				//HBITMAP	hbmpCap;
				hdc  = BeginPaint( hWnd, &ps );
				
                //��ʾ����
				GetClientRect(hWnd, &rcClient);

				newbrush	= CreateBrush(BS_SOLID,RGB(163,176,229),NULL);
				oldbrush	= SelectObject(hdc,newbrush);

				hPen = CreatePen(PS_SOLID, 1, RGB(163,176,229));
				hOldPen = (HPEN)SelectObject(hdc, hPen);

				SetRect(&rcCapicon, 1, 40, 173, 70);
				DrawRect(hdc,&rcCapicon);

				SelectObject(hdc, hOldPen);
				DeleteObject(hPen);
				
				newbrush	= SelectObject(hdc,oldbrush);
				DeleteObject(newbrush);

				nOldMode = SetBkMode(hdc, NEWTRANSPARENT);
				DrawImageFromFile(hdc, pMsgCurrentWin->strCongif, 80, 48, ROP_SRC);
				//hbmpCap = LoadImage(NULL,"/usr/local/lib/gui/resources/capbmp.bmp", IMAGE_BITMAP, 176, 24,LR_LOADFROMFILE);
				//BitBlt(hdc, 0, 16, rcClient.right, 40, (HDC)hbmpCap, 0, 0, ROP_SRC);
				//DrawImageFromFile(hdc, "/usr/local/lib/gui/resources/capbmp.bmp", 0, 16, ROP_SRC);
				SetBkMode(hdc, TRANSPARENT);
				SetRect(&rCaptext, 0, 15, 176, 40);
				SetRect(&rcClient, 0, 71, 176, 196);
                if(pMsgCurrentWin->sMsgWinPrompt != NULL)
                {                    
                    DrawText(hdc, pMsgCurrentWin->sMsgWinPrompt, 
                        strlen(pMsgCurrentWin->sMsgWinPrompt), 
						&rcClient, DT_VCENTER|DT_CENTER);                    
                }
				if (pMsgCurrentWin->szCaption) {
					DrawText(hdc, pMsgCurrentWin->szCaption, -1, &rCaptext, DT_CENTER|DT_VCENTER);
				}
				SetBkMode(hdc, nOldMode);
				SetRect(&rGps, 0, 0,176, 15);
				ClearRect(hdc, &rGps, RGB(206, 211, 214));//grey
				DrawIconFromFile(hdc, "/rom/progman/icon/icon_off.ico", 0, 0, 14, 14);
//				if (hbmpCap) {
//					DeleteObject(hbmpCap);
//				}
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
					
					PostMessage(pMsgCurrentWin->hCall, pMsgCurrentWin->rMsg, 0, 1);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					
				}
				break;

			case VK_F10:
				if (pMsgCurrentWin->szCancel) {
					
					if (pMsgCurrentWin->hFrame) {
						SendMessage(pMsgCurrentWin->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					}
					PostMessage(pMsgCurrentWin->hCall, pMsgCurrentWin->rMsg, 0, 0);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
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
			/* Release mouse capture */
			if (GetFocus() == hWnd)
			{
				ReleaseCapture();
				UngrabKeyboard();
			}
			
            DelMsgCurrentWin(pMsgCurrentWin->hMsgwin);

			//end audio
            PrioMan_EndCallMusic(PRIOMAN_PRIORITY_WARNING, TRUE);

            if (!pMsgWinHead)
	        {
                UnregisterClass(WC_MSGWIN, NULL);
                bRegist = FALSE;
	        }

			break;
			
	

		case WM_TIMER:
            {
                UINT uTimerID = (UINT)wParam;

                switch(uTimerID)
                {
                case 1: //��ʱtimer
                    {
                       //printf("in msgwin timer time arrive\r\n");
						if (pMsgCurrentWin->hFrame) {
						
						PostMessage(pMsgCurrentWin->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
						}
						PostMessage(pMsgCurrentWin->hCall, pMsgCurrentWin->rMsg, 0, 1);
                        PostMessage(hWnd, WM_CLOSE, 0, 0);
                        
                    }
                    break;

                } //switch
                
            }
			break;

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

 //   pMsgNode->hGifDlg = (HGIFANIMATE)CreateImageFromFile(TIPSDLGGIF);

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
    int  nNumPerLine;
    int  nLineCount;
//    int  nBtnLeft;
    SIZE size;
    HDC  hdc;
    
    ///////////////////////////
    hdc = GetDC(hMsgWin);
    
    pMsgCurrentWin->nMsgWinTimeout = nTimeout;	
	SureIcon(pMsgCurrentWin->strCongif, eTypePic);
	/*
    if(pMsgCurrentWin->nMsgWinTimeout <= 0)
    { 
        GetTextExtentPoint32(hdc, ML("OK"), -1, &size);
        nBtnLeft = (MSGWIN_WIDTH - size.cx - 8) / 2;

        SetRect(&pMsgCurrentWin->rcBtn, 
                MSGWIN_LEFT+nBtnLeft, 
                MSGWIN_TOP+MSGWIN_HEIGHT - BTNINTERVAL-size.cy-4, 
                MSGWIN_LEFT+nBtnLeft+size.cx+8, 
                MSGWIN_TOP+MSGWIN_HEIGHT-BTNINTERVAL);
        
        pMsgCurrentWin->bPenDown = FALSE;
        pMsgCurrentWin->bInBtn = FALSE;
    }
	*/
    GetTextExtentPoint32(hdc, "T", -1, &size);
    nNumPerLine = (rcPrompt.right - rcPrompt.left) / size.cx;
    nLineCount = (rcPrompt.bottom - rcPrompt.top) / size.cy;

    ReleaseDC(hMsgWin, hdc);

	//fill a node of linked list 
	pMsgCurrentWin->hMsgwin = hMsgWin;	    //handle of this wnd
	pMsgCurrentWin->hCall = hCall;			//window handle of calling
	pMsgCurrentWin->rMsg = rMsg;

    if(szPrompt != NULL)
    {
        pMsgCurrentWin->sMsgWinPrompt = 
                            (char*)malloc(strlen(szPrompt)+nLineCount*2);
        //pMsgCurrentWin->sMsgWinPrompt = (char*)malloc(strlen(szPrompt)+nLineCount*2);
        if(pMsgCurrentWin->sMsgWinPrompt == NULL)
            return;
        //format the prompt
        //@**#---2005-07-20 14:20:38 (mxlin)---#**@
//pMsgCurrentWin->Format = FormatStr(pMsgCurrentWin->sMsgWinPrompt, 
//		                               szPrompt, nLineCount, nNumPerLine);
		strcpy(pMsgCurrentWin->sMsgWinPrompt,szPrompt);
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
            /*
            if(pDelMsgWin->hCmpDC)
            {
                SelectObject(pDelMsgWin->hCmpDC, pDelMsgWin->hOldBmp);
                DeleteObject(pDelMsgWin->hBmpScreen);
                DeleteDC(pDelMsgWin->hCmpDC);
                DeleteDC(pDelMsgWin->hdcScreen);
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

/*************************End of MsgWin.c*********************************/
/*void Setup_PlayRing(int a, BOOL b)
{
	return;
}

static COLORREF SetupGetDeskColor(int iIndex)
{
	return RGB(0,0,255);
}*/

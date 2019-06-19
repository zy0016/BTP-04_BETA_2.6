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



/*---------- Constant / Macro Definitions ------------------------------*/

#define         FULLSCREENMSGWIN                "FullScreenMsgSelWinClassEx"
#define         MAX_PROMPT_LEN              120

#define         INTERVAL                    5

#define			ID_YES		1301
#define			ID_NO		1302

/*---------- Type Declarations -----------------------------------------*/

static BOOL     bRegist = FALSE;

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
    char        szOk[15];
    char        szCancel[15];
	PSTR		szCaption;
    char        sMsgSelPrompt[MAX_PROMPT_LEN];
    struct tagMsgSelWinStruct *next;
}*PMSGSELWINSTRUCT, MSGSELWINSTRUCT;
//处理重入
static PMSGSELWINSTRUCT pMsgCurrentSelWin, pMsgSelWinHead = NULL;

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
BOOL PLXConfirmWinExOem(HWND hFrame, HWND hWnd, PCSTR szPrompt, NOTIFYTYPE ePic, PCSTR szCaption, 
                  PCSTR szOk, PCSTR szCancel, UINT msg)
{
    BOOL        bRet = TRUE;
    WNDCLASS    wc;
//	RECT		rc;

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
        wc.lpszClassName = FULLSCREENMSGWIN;    
        if( !RegisterClass(&wc) )
            return FALSE;   /* ??? */
        bRegist = TRUE;
    }			
	hMsgSelWin = CreateWindow(FULLSCREENMSGWIN, szCaption,
		PWS_STATICBAR,
		0, 0, 176, 220,
		hWnd, NULL, NULL, NULL);	
	if(szOk)
	{
		
		SendMessage(hMsgSelWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_YES,1), (LPARAM)szOk);
	}
	
	if(szCancel)
	{
		SendMessage(hMsgSelWin, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_NO,0), (LPARAM)szCancel);
	}
    if( hMsgSelWin != NULL )
    {
        AddMsgSelWinNode(hMsgSelWin, hWnd, hFrame, msg, szOk, szCancel, szPrompt, ePic, szCaption);
		
		ShowWindow( hMsgSelWin, SW_SHOW );
		UpdateWindow( hMsgSelWin );
		
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
		
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
			if (strlen(pMsgCurrentSelWin->szOk) > 0)
			{
				
				PostMessage(pMsgCurrentSelWin->hWnd, pMsgCurrentSelWin->msg, 0, 1);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}            
            break;

        case VK_F10:
			if (strlen(pMsgCurrentSelWin->szCancel) > 0)
			{
				
				PostMessage(pMsgCurrentSelWin->hWnd, pMsgCurrentSelWin->msg, 0, 0);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
           
            break;
       
        }            
        break;

    case WM_PAINT:
		{
			RECT	rCaptext, rGps;
			HPEN hPen, hOldPen;
			//HBITMAP	hbmpCap;
			hdc  = BeginPaint( hWnd, &ps );
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
			
			nBkMode = SetBkMode(hdc, NEWTRANSPARENT);
			DrawImageFromFile(hdc,pMsgCurrentSelWin->strCongif,80,48,ROP_SRC);
			//hbmpCap = LoadImage(NULL,"/usr/local/lib/gui/resources/capbmp.bmp", IMAGE_BITMAP, 176, 24,LR_LOADFROMFILE);
			//BitBlt(hdc, 0, 16, rcClient.right, 40, (HDC)hbmpCap, 0, 0, ROP_SRC);
			//DrawImageFromFile(hdc, "/usr/local/lib/gui/resources/capbmp.bmp", 0, 16, ROP_SRC);
			
			SetBkMode(hdc, TRANSPARENT);
			SetRect(&rCaptext, 0, 16, 176, 40);
			DrawText(hdc, pMsgCurrentSelWin->szCaption, -1, &rCaptext, DT_CENTER|DT_VCENTER);
			DrawText(hdc, pMsgCurrentSelWin->sMsgSelPrompt, strlen(pMsgCurrentSelWin->sMsgSelPrompt), 
				&pMsgCurrentSelWin->rPrompt, DT_CENTER|DT_VCENTER);
			SetBkMode(hdc, nBkMode);

			SetRect(&rGps, 0, 0,176, 15);
			ClearRect(hdc, &rGps, RGB(206, 211, 214));//grey
			DrawIconFromFile(hdc, "/rom/progman/icon/icon_off.ico", 0, 0, 14, 14);
//			if (hbmpCap) {
//				DeleteObject(hbmpCap);
//			}
			EndPaint( hWnd, &ps );
		}

        break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

    case WM_DESTROY:
		PrioMan_EndCallMusic(PRIOMAN_PRIORITY_WARNING, TRUE);
        DelMsgCurrentSelWin(pMsgCurrentSelWin->hMsgSelWin);
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

    GetClientRect(hMsgSelWnd, &rWndRect);
	SureIcon(pMsgSelWinNode->strCongif,eTypePic);
    SetRect(&pMsgSelWinNode->rPrompt, rWndRect.left+INTERVAL, 
        77, 170, rWndRect.bottom);
    pMsgSelWinNode->hMsgSelWin = hMsgSelWnd;
    pMsgSelWinNode->hWnd = hWnd;
    pMsgSelWinNode->msg = msg;
	if (strchr(szPrompt, '\n')) {
		strcpy(pMsgSelWinNode->sMsgSelPrompt,szPrompt);
	}
	else
	{
		pMsgSelWinNode->Format = FormatStr(pMsgSelWinNode->sMsgSelPrompt,(PSTR)szPrompt);
	}
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

/*************************End of MsgSelWin.c*********************************/

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : WaitWin.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "winpda.h"
//#include "fapi.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h" 
#include "assert.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"
#include "plx_pdaex.h"
#include "app.h"
#include "stdlib.h"
//#include "hp_icon.h"
#include "str_public.h"
#include "str_plx.h"
#include "hpimage.h"
//#include "hpdebug.h"
#include "mullang.h"
#include "pubapp.h"
#include "prioman.h"
#ifndef _WAITWIN_
#define _WAITWIN_
#endif

#define WC_WAITWINDOW     "WaitWinClass"
#define	STRSIZE		      120
//#define WAITDLGGIF        "/rom/public/tipswindlg.gif"
#define WAITGIF           "/rom/public/waiting.bmp"
#define WAITWIN_HEIGHT     88//101
#define WAITWIN_WIDTH      110//201
#define WAITWIN_LEFT       55//19
#define WAITWIN_TOP        44//109
//#define CHARWIDTH		  6
//#define CHARHEIGHT		  12
//#define GIFLEFT           12
//#define GIFTOP            13
//#define GIFWIDTH          40
//#define GIFHEIGHT         43

#define	IDM_BACK		(WM_USER+100)
#define	IDM_OK			(WM_USER+101)

#define	ID_TIMERFGX		1

static HWND     m_hWaitWindow = NULL;
static HWND		hParentWindow = NULL;
//static int		iContentLen;
static BOOL		isRegist = FALSE;
static BOOL		bWaitWin = FALSE;

//static COLORREF	BgRGB;
static char		cPromptContent[STRSIZE] ;
static UINT		reMsg = 0;
//static BOOL		bLeftKey = FALSE;
//static BOOL		bRightKey = FALSE;
static	UINT	uTimerGx	=	0;
/************************************************************/

static VOID WaitWindow_OnPaint(HWND hwnd, HGIFANIMATE hGifDlg, HGIFANIMATE hGif);
static LRESULT CALLBACK WaitWindowProcedure(HWND, UINT, WPARAM, LPARAM);
static LRESULT CALLBACK WaitWin_MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WaitWin_KeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK WaitWindowExtProcedure(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                   LPARAM lParam);
BOOL IsWaitWnd();

/**********************函数声明******************************/
void Draw3DBorder(HDC hdc, RECT *rc, int InOut);
//extern HICON WINAPI CreateIconFromFile(PCSTR pszIconName, int nWidth, int nHeight);
/************************************************************/

//static int nWinX, nWinY;
//static HHOOK	hMouseHook, hKeybrdHook;//
static RECT rcText = {3, 3, 173, 145}; //{63, 11, 190, 93};
/*===========================================================================
*	Function:	Init_WaitWindow
*
*	Purpose :   初始化等待窗口
*
*	Argument:	None
*
*	Return  :   无
*   
*	Explain :	
*===========================================================================*/

BOOL Init_WaitWindow()
{
	WNDCLASS	wc;

	//窗口注册
	if (!isRegist)
	{
		wc.style         = CS_NOFOCUS;
		wc.lpfnWndProc   = WaitWindowProcedure;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = (HINSTANCE)NULL;
		wc.hIcon         = (HICON)NULL;
		wc.hCursor       = (HCURSOR)NULL;
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = (LPCSTR)NULL;
		wc.lpszClassName = WC_WAITWINDOW;
		
		RegisterClass(&wc);

		isRegist = TRUE;

//		nWinX = GetSystemMetrics(SM_CXSCREEN) / 2 - WAITWIN_WIDTH  / 2;
//		nWinY = GetSystemMetrics(SM_CYSCREEN) / 2 - WAITWIN_HEIGHT / 2;

		return TRUE;
	}
	return FALSE;
}

/*===========================================================================
*	Function:	IsWaitWindowShow
*
*	Purpose :   判断当前窗口是否存在
*
*	Argument:	bShow = TRUE  : 等待窗口显示；
*               bShow = FALSE : 等待窗口隐藏；
*
*	Return  :   None	
*===========================================================================*/
BOOL IsWaitWindowShow()
{
    return IsWindow(m_hWaitWindow);
}

/*===========================================================================
*	Function:	WaitWindowStateEx
*
*	Purpose :   允许用户修改提示文字
*
*	Argument:	bShow = TRUE  : show
*               bShow = FALSE : hide
*
*	Return  :   None	
*===========================================================================*/
void WaitWindowStateDaemonEx(HWND hParent, BOOL bShow)
{
//    int nStrlen, nStaticLen;

    
#ifdef _WAITWIN_
//    StartObjectDebug();
#endif

	cPromptContent[0] = '\0';
	hParentWindow = hParent;
	////////////////////////////////////
    if( bShow )
	{
		if (IsWindow(m_hWaitWindow))
		{
			//ShowWindow(m_hWaitWindow,SW_SHOW);
			return;
		}

			m_hWaitWindow = CreateWindow(WC_WAITWINDOW, NULL,
				0,
				0,0,0,0,
				hParent, NULL, NULL, NULL);
		
		
		if (m_hWaitWindow == NULL)
			return;
		//bWaitWin = TRUE;
        ShowWindow(m_hWaitWindow, SW_SHOW);
        UpdateWindow(m_hWaitWindow);

	}
	else
	{
		if (!IsWindow(m_hWaitWindow))
			return;
		DestroyWindow(m_hWaitWindow);
		m_hWaitWindow = NULL;
		//bWaitWin = FALSE;
	}
}
void WaitWindowStateEx(HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption,PCSTR szOk,PCSTR szCancel)
{
//    int nStrlen, nStaticLen;

    
#ifdef _WAITWIN_
//    StartObjectDebug();
#endif
	hParentWindow = hParent;
	if (cPrompt != NULL && strlen(cPrompt) < STRSIZE)
		strcpy(cPromptContent, cPrompt);
	else
		cPromptContent[0] = '\0';
	////////////////////////////////////
    if( bShow )
	{
		if (IsWindow(m_hWaitWindow))
		{
			//ShowWindow(m_hWaitWindow,SW_SHOW);
			//bWaitWin = TRUE;
			return;
		}
		if (szCaption) {
			m_hWaitWindow = CreateWindow(WC_WAITWINDOW, szCaption,
				WS_CAPTION|PWS_STATICBAR,
				PLX_WIN_POSITION,
				hParent, NULL, NULL, NULL);
		}
		else
		{
			m_hWaitWindow = CreateWindow(WC_WAITWINDOW, NULL,
				PWS_STATICBAR,
				PLX_NOTIFICATON_POS,
				hParent, NULL, NULL, NULL);
		}
		if (m_hWaitWindow == NULL)
			return;
		//bWaitWin = TRUE;
		if (szOk) 
		{
			//bLeftKey = TRUE;				
			SendMessage(m_hWaitWindow,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_OK,1),(LPARAM)szOk);
		}
		if (szCancel)
		{
			//bRightKey = TRUE;			
			SendMessage(m_hWaitWindow,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_BACK,0),(LPARAM)szCancel);
		}
        ShowWindow(m_hWaitWindow, SW_SHOW);
        UpdateWindow(m_hWaitWindow);

		

	}
	else
	{
		if (!IsWindow(m_hWaitWindow))
			return;
		DestroyWindow(m_hWaitWindow);
		m_hWaitWindow = NULL;
		//bWaitWin = FALSE;
		
	}
    //////////////////////////////////////////////////////////
}

void WaitWin(HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption,PCSTR szOk,PCSTR szCancel, UINT rMsg)
{
//    int nStrlen, nStaticLen;

    
#ifdef _WAITWIN_
//    StartObjectDebug();
#endif

	hParentWindow = hParent;
	if (cPrompt != NULL && strlen(cPrompt) < STRSIZE)
		strcpy(cPromptContent, cPrompt);
	else
		cPromptContent[0] = '\0';
	reMsg = rMsg;
	////////////////////////////////////
    if( bShow )
	{
		if (IsWindow(m_hWaitWindow))
		{
			//ShowWindow(m_hWaitWindow,SW_SHOW);
			//bWaitWin = TRUE;
			return;
		}
		if (szCaption) {
			m_hWaitWindow = CreateWindow(WC_WAITWINDOW, szCaption,
				WS_CAPTION|PWS_STATICBAR,
				PLX_WIN_POSITION,
				hParent, NULL, NULL, NULL);
		}
		else
		{
			m_hWaitWindow = CreateWindow(WC_WAITWINDOW, NULL,
				PWS_STATICBAR,
				PLX_NOTIFICATON_POS,
				hParent, NULL, NULL, NULL);
		}
		if (m_hWaitWindow == NULL)
			return;
		//bWaitWin= TRUE;
		if (szOk) 
		{
			//bLeftKey = TRUE;				
			SendMessage(m_hWaitWindow,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_OK,1),(LPARAM)szOk);
		}
		if (szCancel)
		{
			//bRightKey = TRUE;			
			SendMessage(m_hWaitWindow,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_BACK,0),(LPARAM)szCancel);
		}
        ShowWindow(m_hWaitWindow, SW_SHOW);
        UpdateWindow(m_hWaitWindow);

		

	}
	else
	{
		if (!IsWindow(m_hWaitWindow))
			return;
		DestroyWindow(m_hWaitWindow);
		m_hWaitWindow = NULL;
		//bWaitWin = FALSE;
		
	}
    //////////////////////////////////////////////////////////
}

void WaitWinWithTimer(HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption,PCSTR szOk,PCSTR szCancel, UINT rMsg, UINT uTimer)
{


	hParentWindow = hParent;
	if (cPrompt != NULL && strlen(cPrompt) < STRSIZE)
		strcpy(cPromptContent, cPrompt);
	else
		cPromptContent[0] = '\0';
	reMsg = rMsg;
	uTimerGx = uTimer;
	////////////////////////////////////
    if( bShow )
	{
		if (IsWindow(m_hWaitWindow))
		{
			//ShowWindow(m_hWaitWindow,SW_SHOW);
			//bWaitWin = TRUE;
			return;
		}
		if (szCaption) {
			m_hWaitWindow = CreateWindow(WC_WAITWINDOW, szCaption,
				WS_CAPTION|PWS_STATICBAR,
				PLX_WIN_POSITION,
				hParent, NULL, NULL, NULL);
		}
		else
		{
			m_hWaitWindow = CreateWindow(WC_WAITWINDOW, NULL,
				PWS_STATICBAR,
				PLX_NOTIFICATON_POS,
				hParent, NULL, NULL, NULL);
		}
		if (m_hWaitWindow == NULL)
			return;
		//bWaitWin= TRUE;
		if (szOk) 
		{
			//bLeftKey = TRUE;				
			SendMessage(m_hWaitWindow,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_OK,1),(LPARAM)szOk);
		}
		if (szCancel)
		{
			//bRightKey = TRUE;			
			SendMessage(m_hWaitWindow,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_BACK,0),(LPARAM)szCancel);
		}
        ShowWindow(m_hWaitWindow, SW_SHOW);
        UpdateWindow(m_hWaitWindow);

		

	}
	else
	{
		if (!IsWindow(m_hWaitWindow))
			return;
		DestroyWindow(m_hWaitWindow);
		m_hWaitWindow = NULL;
		//bWaitWin = FALSE;
		
	}
    //////////////////////////////////////////////////////////
}

/*===========================================================================
*	Function:	WaitWindowState
*
*	Purpose :   
*
*	Argument:	bShow = TRUE  : show
*               bShow = FALSE : hide
*
*	Return  :   None	
*===========================================================================*/

void WaitWindowState(HWND hParent, BOOL bShow)
{

    WaitWindowStateEx(hParent, bShow, NULL,NULL,NULL,NULL);
}

/*===========================================================================
*	Function:	WaitWindowProcedure
*
*	Purpose :   
*===========================================================================*/

static LRESULT CALLBACK WaitWindowProcedure(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                   LPARAM lParam)
{
//    HDC  hdc;
	LONG lResult;

	

    lResult = (LRESULT)TRUE;
    
    switch( wMsgCmd )
    {
		case WM_CREATE:
			bWaitWin = TRUE;
			if (uTimerGx != 0) {
				SetTimer(hWnd, ID_TIMERFGX, 100*uTimerGx, NULL);
			}			
			SetCapture(hWnd);
			GrabKeyboard(hWnd);
			
			//PrioMan_CallMusic(PRIOMAN_PRIORITY_WARNING,1);
			
			DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
			DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 20);
			DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);
//			PrioMan_CallMusic(33,1);
			break;

		case WM_ACTIVATE:
			if (wParam == WA_ACTIVE)
			{				
				SetCapture(hWnd);
				GrabKeyboard(hWnd);
				SetFocus(hWnd);
				//bWaitWin = TRUE;
			}
			else if (wParam == WA_INACTIVE)
			{
				ReleaseCapture();
				UngrabKeyboard();
				//bWaitWin = FALSE;
				//PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		case WM_TIMER:
			switch(wParam) 
			{
			case ID_TIMERFGX:
				KillTimer(hWnd, ID_TIMERFGX);
				uTimerGx = 0;
				PostMessage(hParentWindow, reMsg, 0, 0);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
			break;

		case WM_COMMAND:
			switch(wParam) 
			{
			case IDM_BACK:
				PostMessage(hParentWindow, reMsg, 0, 0);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				//SendMessage(GetWindow(hWnd, GW_OWNER), reMsg, 0, 0);
				//DestroyWindow(hWnd);
				break;

			case IDM_OK:
				PostMessage(hParentWindow, reMsg, 0, 1);
				//SendMessage(GetWindow(hWnd, GW_OWNER), reMsg, 0, 1);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				//DestroyWindow(hWnd);
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
			break;
			
        case WM_PAINT:
            WaitWindow_OnPaint(hWnd, NULL, NULL);
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			m_hWaitWindow = NULL;
			break;

		case WM_DESTROY:
			{
				cPromptContent[0] = '\0';
				reMsg = 0;
				bWaitWin = FALSE;
				ReleaseCapture();
				UngrabKeyboard();
				if (uTimerGx != 0) {
					KillTimer(hWnd, ID_TIMERFGX);
					uTimerGx = 0;
				}
				//PrioMan_EndCallMusic(PRIOMAN_PRIORITY_WARNING, TRUE);
//				PrioMan_EndCallMusic(33, TRUE);
			}
			break;
		
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
    }

    return lResult;
}

/*===========================================================================
*	Function:	WaitWindow_OnPaint
*
*	Purpose :   
*===========================================================================*/

static VOID WaitWindow_OnPaint(HWND hwnd, HGIFANIMATE hGifDlg, HGIFANIMATE hGif)
{
    HDC    hdc;
	int    OldMode;
	RECT   rcClient,rcCapicon;
	HBRUSH	newbrush,oldbrush;
	HPEN	hPen, hOldPen;
	PSTR	szEdit = NULL;

	GetClientRect(hwnd, &rcClient);

	hdc = BeginPaint(hwnd,NULL);

	newbrush = CreateBrush(BS_SOLID, RGB(163,176,229),NULL);
	oldbrush	= SelectObject(hdc,newbrush);

	hPen = CreatePen(PS_SOLID, 1, RGB(163,176,229));
	hOldPen = (HPEN)SelectObject(hdc, hPen);
	
	SetRect(&rcCapicon, rcClient.left, rcClient.top, rcClient.right, 31);
	DrawRect(hdc,&rcCapicon);

	newbrush	= SelectObject(hdc,oldbrush);
	DeleteObject(newbrush);
	
	SelectObject(hdc, hOldPen);
	DeleteObject(hPen);
	
	OldMode = SetBkMode(hdc, NEWTRANSPARENT);
	DrawImageFromFile(hdc,WAITGIF,80,8,ROP_SRC);

	SetBkMode(hdc, BM_TRANSPARENT);

	rcClient.top += 31;
	if (cPromptContent[0] == '\0')
        DrawText(hdc, ML("Please wait..."), strlen(ML("Please wait...")), &rcClient, DT_CENTER|DT_VCENTER);
	else
	{
		szEdit = malloc(strlen(cPromptContent) + strlen(cPromptContent)/5 +5);
		if (!szEdit) 
		{
			DrawText(hdc, cPromptContent, -1, &rcClient, DT_CENTER|DT_VCENTER);
		}
		else
		{
			FormatStr(szEdit, cPromptContent);
			DrawText(hdc, szEdit, -1, &rcClient, DT_CENTER|DT_VCENTER);
			free(szEdit);
		}
	}
		
	SetBkMode(hdc, OldMode);

	EndPaint(hwnd, NULL);

//	OS_UpdateScreen(0, 0, 0, 0);
    
	return;
}
/*************************************************************************
  Function   :WaitWin_MouseProc
--------------------------------------------------------------------------
  Description:Mouse hook proc.
--------------------------------------------------------------------------
  Input      :
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
#if 0
static LRESULT CALLBACK WaitWin_MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)  
		return CallNextHookEx(hMouseHook, nCode, wParam, lParam);

	return 1;
}
#endif
/*************************************************************************
  Function   :WaitWin_MouseProc
--------------------------------------------------------------------------
  Description:Mouse hook proc.
--------------------------------------------------------------------------
  Input      :
--------------------------------------------------------------------------
  Return     :None.
--------------------------------------------------------------------------
  IMPORTANT NOTES:
*************************************************************************/
#if 0
static LRESULT CALLBACK WaitWin_KeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
//	static	char	lastkeysc;	//last key pressed
	static	BYTE	lastkeyflag =1;
	BYTE	keyflag;		//info about key
//	char	keysc;			//scancode of the current pressed key
//	WORD id =HIWORD(lParam);
	keyflag	= (BYTE)(HIWORD(lParam)>>8);
//	keysc	= (char)(id);
	

    if (nCode < 0 )  // do not process the message 
        return CallNextHookEx(hKeybrdHook, nCode, wParam, lParam);
	if (bLeftKey &&(wParam == VK_RETURN)) {
		return CallNextHookEx(hKeybrdHook, nCode, wParam, lParam);
	}
	if (bRightKey &&(wParam == VK_F10)) {
		return CallNextHookEx(hKeybrdHook, nCode, wParam, lParam);
	}
	if(keyflag==0xc0)//if msg is keyup
	{
		//printf("keyup\n");
		if(lastkeyflag!= 0)	//last msg isn't keydown
			return CallNextHookEx(hKeybrdHook, nCode, wParam, lParam);
	}
	lastkeyflag	=	keyflag;
	/*
	if (keyflag==0)			//if msg is keydown
	{

	}
*/
	
	return 1;
}
#endif
//////////////////////////////////////////////////////////////////////////////
/**********************************************************/
//void Draw3DBorder(HDC hdc, RECT *rc, int In);
//
//In :  0： 
/**********************************************************/
#if 0
void Draw3DBorder(HDC hdc, RECT *rc, int In)
{
	if(In)
	{
		//dark gray
		SelectObject(hdc, GetStockObject(GRAY_PEN));
		DrawLine( hdc, rc->left, rc->top, rc->left, rc->bottom);
		DrawLine( hdc, rc->left, rc->top, rc->right, rc->top);
		//black
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		DrawLine( hdc, rc->left+1, rc->top+1, rc->left+1, rc->bottom-1);
		DrawLine( hdc, rc->left+1, rc->top+1, rc->right-1, rc->top+1);
		//light gray
		SelectObject(hdc, GetStockObject(LTGRAY_PEN));
		DrawLine( hdc, rc->right-1, rc->top+1, rc->right-1, rc->bottom);
		DrawLine( hdc, rc->left+1, rc->bottom-1, rc->right-1, rc->bottom-1);

		//white
		SelectObject(hdc, GetStockObject(WHITE_PEN));
		DrawLine( hdc, rc->right, rc->top, rc->right, rc->bottom);
		DrawLine( hdc, rc->left, rc->bottom, rc->right+1, rc->bottom);
	}
	else
	{
		//dark gray
		SelectObject(hdc, GetStockObject(LTGRAY_PEN));
		DrawLine( hdc, rc->left, rc->top, rc->left, rc->bottom);
		DrawLine( hdc, rc->left, rc->top, rc->right, rc->top);
		//black
		SelectObject(hdc, GetStockObject(WHITE_PEN));
		DrawLine( hdc, rc->left+1, rc->top+1, rc->left+1, rc->bottom-1);
		DrawLine( hdc, rc->left+1, rc->top+1, rc->right-1, rc->top+1);
		//light gray
		SelectObject(hdc, GetStockObject(GRAY_PEN));
		DrawLine( hdc, rc->right-1, rc->top+1, rc->right-1, rc->bottom);
		DrawLine( hdc, rc->left+1, rc->bottom-1, rc->right-1, rc->bottom-1);

		//white
		SelectObject(hdc, GetStockObject(BLACK_PEN));
		DrawLine( hdc, rc->right, rc->top, rc->right, rc->bottom);
		DrawLine( hdc, rc->left, rc->bottom, rc->right+1, rc->bottom);
	}

}
#endif
/*	该函数在启功时调用		
COLORREF colorref		
HWND hWndB				
*/
/*
HWND InitColorWin(HWND hWndB)
{
    return NULL;
}
*/
BOOL Init_WaitWindowExt()
{
	WNDCLASS	wc;

	//窗口注册
//	if (!isRegist)
//	{
		wc.style         = CS_NOFOCUS;
		wc.lpfnWndProc   = WaitWindowExtProcedure;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = (HINSTANCE)NULL;
		wc.hIcon         = (HICON)NULL;
		wc.hCursor       = (HCURSOR)NULL;
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = (LPCSTR)NULL;
		wc.lpszClassName = "WAITWINDEPEXTOP";
		
		RegisterClass(&wc);

//		isRegist = TRUE;

//		nWinX = GetSystemMetrics(SM_CXSCREEN) / 2 - WAITWIN_WIDTH  / 2;
//		nWinY = GetSystemMetrics(SM_CYSCREEN) / 2 - WAITWIN_HEIGHT / 2;

		return TRUE;
//	}
//	return isRegist;
}

void WaitWinDepExtOp(HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption,PCSTR szOk,PCSTR szCancel, UINT rMsg, UINT uTimer)
{


	hParentWindow = hParent;
	if (cPrompt != NULL && strlen(cPrompt) < STRSIZE)
		strcpy(cPromptContent, cPrompt);
	else
		cPromptContent[0] = '\0';
	reMsg = rMsg;
	uTimerGx = uTimer;
	////////////////////////////////////
    if( bShow )
	{
		if (IsWindow(m_hWaitWindow))
		{
			//ShowWindow(m_hWaitWindow,SW_SHOW);
			//bWaitWin = TRUE;
			return;
		}
		Init_WaitWindowExt();
		if (szCaption) {
			m_hWaitWindow = CreateWindow("WAITWINDEPEXTOP", szCaption,
				WS_CAPTION|PWS_STATICBAR,
				PLX_WIN_POSITION,
				hParent, NULL, NULL, NULL);
		}
		else
		{
			m_hWaitWindow = CreateWindow("WAITWINDEPEXTOP", NULL,
				PWS_STATICBAR,
				PLX_NOTIFICATON_POS,
				hParent, NULL, NULL, NULL);
		}
		if (m_hWaitWindow == NULL)
			return;
		//bWaitWin= TRUE;
		if (szOk) 
		{
			//bLeftKey = TRUE;				
			SendMessage(m_hWaitWindow,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_OK,1),(LPARAM)szOk);
		}
		if (szCancel)
		{
			//bRightKey = TRUE;			
			SendMessage(m_hWaitWindow,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDM_BACK,0),(LPARAM)szCancel);
		}
        ShowWindow(m_hWaitWindow, SW_SHOW);
        UpdateWindow(m_hWaitWindow);

		

	}
	else
	{
		if (!IsWindow(m_hWaitWindow))
			return;
		DestroyWindow(m_hWaitWindow);
		m_hWaitWindow = NULL;
		//bWaitWin = FALSE;
		
	}
    //////////////////////////////////////////////////////////
}
static LRESULT CALLBACK WaitWindowExtProcedure(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                   LPARAM lParam)
{
//    HDC  hdc;
	LONG lResult;

	

    lResult = (LRESULT)TRUE;
    
    switch( wMsgCmd )
    {
		case WM_CREATE:
			bWaitWin = TRUE;
			if (uTimerGx != 0) {
				SetTimer(hWnd, ID_TIMERFGX, 100*uTimerGx, NULL);
			}			
			SetCapture(hWnd);
			GrabKeyboard(hWnd);
//			PrioMan_CallMusic(33,1);
			break;

		case WM_ACTIVATE:
			if (wParam == WA_ACTIVE)
			{				
				SetCapture(hWnd);
				GrabKeyboard(hWnd);
				SetFocus(hWnd);
				//bWaitWin = TRUE;
			}
			else if (wParam == WA_INACTIVE)
			{
				ReleaseCapture();
				UngrabKeyboard();
				//bWaitWin = FALSE;
				//PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		case WM_TIMER:
			switch(wParam) 
			{
			case ID_TIMERFGX:
				KillTimer(hWnd, ID_TIMERFGX);
				uTimerGx = 0;
				PostMessage(hParentWindow, reMsg, 0, 0);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
			break;

		case WM_COMMAND:
			switch(wParam) 
			{
			case IDM_BACK:
				PostMessage(hParentWindow, reMsg, 0, 0);
				//PostMessage(hWnd, WM_CLOSE, 0, 0);
				//SendMessage(GetWindow(hWnd, GW_OWNER), reMsg, 0, 0);
				//DestroyWindow(hWnd);
				break;

			case IDM_OK:
				PostMessage(hParentWindow, reMsg, 0, 1);
				//SendMessage(GetWindow(hWnd, GW_OWNER), reMsg, 0, 1);
				//PostMessage(hWnd, WM_CLOSE, 0, 0);
				//DestroyWindow(hWnd);
				break;
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
			break;
			
        case WM_PAINT:
            WaitWindow_OnPaint(hWnd, NULL, NULL);
			break;

		case WM_CLOSE:
			DestroyWindow(hWnd);
			m_hWaitWindow = NULL;
			break;

		case WM_DESTROY:
			{
				cPromptContent[0] = '\0';
				reMsg = 0;
				bWaitWin = FALSE;
				ReleaseCapture();
				UngrabKeyboard();
				if (uTimerGx != 0) {
					KillTimer(hWnd, ID_TIMERFGX);
					uTimerGx = 0;
				}
//				PrioMan_EndCallMusic(33, TRUE);
			}
			break;
		
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
    }

    return lResult;
}

BOOL	UpdateWaitwinContext(PCSTR buf)
{
	if (!m_hWaitWindow)
	{
		return FALSE;
	}
	strcpy(cPromptContent, buf);
	InvalidateRect(m_hWaitWindow, NULL, TRUE);
	//SendMessage(m_hWaitWindow, WM_PAINT, 0, 0);
	return TRUE;
}
BOOL IsWaitWnd()
{
	return bWaitWin;
}

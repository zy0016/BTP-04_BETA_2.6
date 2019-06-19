/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PreBrow.c
 *
 * Purpose  : 通用预览预览选择多媒体文件 
 *            
\**************************************************************************/
#if 0
#include "winpda.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
//#include "fapi.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h" 

#include "plx_pdaex.h"
#include "hpimage.h"
#include "GameHelp.h"
#include "str_public.h"
#include "str_plx.h"
#include "PreBrow.h"
#include "PreBrowHead.h"
#include "mullang.h"
/************************************************************************/
/* 全局参数宏															*/
/************************************************************************/
#define SMS_BUTTON_BITMAP_NORMAL	"/rom/PRENORM.bmp"//"ROM:bnsel.bmp"
#define SMS_BUTTON_BITMAP_DOWN		"/rom/PREPUT.bmp"//"ROM:bbsel.bmp"
#define SMS_BUTTON_BITMAP_FOCUS		"/rom/PREFOC.bmp"//"ROM:bfocu.bmp"


#define	PREBROW_BTN_NUM		4				/*按钮的数目*/


/************************************************************************/
/* 控件位置宏                                                           */
/************************************************************************/
#define	PREBROW_BTN_WIDTH		((PREBROW_WND_WIDTH*2)/3)			/*选择按钮的宽度*/
#define	PREBROW_BTN_HEIGHT		25							/*选择按钮的高度*/
#define	PREBROW_BTN_STARTX		( ( PREBROW_WND_WIDTH - PREBROW_BTN_WIDTH ) / 2)
															/*第一个按钮的起始x位置*/
#define	PREBROW_BTN_STARTY		((( PREBROW_WND_HEIGHT/PREBROW_BTN_NUM ) - PREBROW_BTN_HEIGHT ) / 2 )
															/*第一按钮的起始Y位置*/

#define	PREBROW_BTN_INTERVAL	(PREBROW_BTN_STARTY + PREBROW_BTN_HEIGHT)
															/*两个按钮Y起始位置的间隔*/ 


/************************************************************************/
/* 消息标记																*/
/************************************************************************/
#define ID_OK			2001
#define ID_EXIT			2002
#define IDM_RETURNMSG	2003

#define	IDM_BTN_PIC		101
#define	IDM_BTN_RING	102
#define	IDM_BTN_PHRASE	103
#define	IDM_BTN_TEXT	104

/*@**#---2005-04-21 14:54:22 (mxlin)---#**@
//字符串
#define	IDS_BIGMAP		GetString(STR_PREBROW_BIGMAP)	//"图片太大，无法显示"
#define IDS_PREBROW		GetString(STR_PREBROW_PREBROW)	//"预览"
#define IDS_PICTURES	GetString(STR_PREBROW_PIC)		//"图片"
#define IDS_RINGS		GetString(STR_PREBROW_RING)		//"铃声"
#define IDS_TEXTS		GetString(STR_PREBROW_TEXT)		//"文本"
#define IDS_QTEXTS		GetString(STR_PREBROW_QTEXT)	//"短语"
#define IDS_SELECT		GetString(STR_PREBROW_SEL)		//"选定"
#define IDS_RINGSTART	GetString(STR_PREBROW_BEEP)		//"播放"
#define IDS_RINGEND		GetString(STR_PREBROW_STOP)		//"停止" 
#define IDS_OK			GetString(STR_WINDOW_OK)
#define IDS_BACK		GetString(STR_WINDOW_BACK)
*/
//字符串
#define	IDS_BIGMAP		ML("Too big Picture！")	//"图片太大，无法显示"
#define IDS_PREBROW		ML("Brow")	//"预览"
#define IDS_PICTURES	ML("Pic")		//"图片"
#define IDS_RINGS		ML("Ring")		//"铃声"
#define IDS_TEXTS		ML("Text")		//"文本"
#define IDS_QTEXTS		ML("msgs")	//"短语"
#define IDS_SELECT		ML("Select")		//"选定"
#define IDS_RINGSTART	ML("Play")		//"播放"
#define IDS_RINGEND		ML("Stop")		//"停止" 
#define IDS_OK			ML("OK")
#define IDS_BACK		ML("Cancel")
/**********************************************
变量定义
\**********************************************/
static	BOOL	bPreBrowseMainWndClass = FALSE;
static	HWND	hWndPreBrowMain = NULL;
static	UINT	uReturnMessage;			/*返回消息*/
static	DWORD	dwStyle = NULL;				/*预览的文件类型*/
static	DWORD	dwMaskStyle = NULL;
static	HWND	hBtnPic, hBtnRing, hBtnText, hBtnPhrase;
static	HWND	hFocusBtn;

static HBITMAP	hBmpBtnNormal = NULL;
static HBITMAP	hBmpBtnFocus = NULL;

/******************************************************************
函数声明
\******************************************************************/
static LRESULT CALLBACK PreBrowseMainWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);

static	BOOL	InitButton( HWND hWnd );


/*********************************************************************
* Function	   PreBrowseInterface
* Purpose      预览多媒体文件接口
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
HWND	APP_PreviewSelect(HWND hWnd,UINT returnmsg,DWORD dw_Style, DWORD dwMask)
{
	return NULL;
}
/*
HWND	APP_PreviewSelect(HWND hWnd,UINT returnmsg,DWORD dw_Style, DWORD dwMask)
{
    WNDCLASS wc;
    
	dwStyle = dw_Style;
	dwMaskStyle = dwMask;

	if (!bPreBrowseMainWndClass)
	{
		wc.style            = NULL;
		wc.lpfnWndProc      = PreBrowseMainWndProc;
		wc.cbClsExtra       = 0;
		wc.cbWndExtra       = NULL;
		wc.hInstance        = NULL;
		wc.hIcon            = NULL;
		wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName     = NULL;
		wc.lpszClassName    = "PreBrowseMainWndClass";
		
		if (!RegisterClass(&wc))
			return NULL;
		
		bPreBrowseMainWndClass = TRUE;
	}

	if (IsWindow(hWndPreBrowMain))
	{
		ShowWindow(hWndPreBrowMain,SW_SHOW);
		UpdateWindow(hWndPreBrowMain);
	}
	else
	{
		hWndPreBrowMain = CreateWindow(
			"PreBrowseMainWndClass", 
			IDS_PREBROW, 
			WS_BORDER|WS_VISIBLE|PWS_STATICBAR|WS_CAPTION,
			PLX_WIN_POSITION,
			hWnd, 
			NULL,
			NULL, 
			NULL
			);

		if (!IsWindow(hWndPreBrowMain))
		{
			return NULL;
		}
		//hWndCall = hWnd;
		uReturnMessage = returnmsg;

		ShowWindow(hWndPreBrowMain,SW_SHOW);
		UpdateWindow(hWndPreBrowMain);
	}
    
	return hWndPreBrowMain;
}
*/
/*********************************************************************
* Function	   PreBrowseMainWndProc
* Purpose      预览主界面窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*
static LRESULT CALLBACK PreBrowseMainWndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	
	LRESULT lResult;
	HDC		hdc;

	lResult = TRUE;


	switch(wMsgCmd)
	{
	case WM_CREATE:
		hBmpBtnNormal = LoadImage(NULL,SMS_BUTTON_BITMAP_NORMAL, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
		hBmpBtnFocus = LoadImage(NULL,SMS_BUTTON_BITMAP_FOCUS, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);		
		InitButton( hWnd );		
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_OK,1), (LPARAM)IDS_OK);
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT,0), (LPARAM)IDS_BACK);
		
		break;

	case WM_ACTIVATE:
		if (WA_ACTIVE == LOWORD(wParam))
		{
			SetFocus (hFocusBtn);			
		}
		break;
	

	case WM_COMMAND:		
		switch(LOWORD(wParam))
		{
		case IDM_BTN_PIC:
			if ( HIWORD(wParam) == BN_CLICKED)
			{
				hFocusBtn = hBtnPic;				
				APP_PreviewPicture(hWnd,IDM_RETURNMSG, dwMaskStyle);				
			}
			break;
		case IDM_BTN_RING:
			if ( HIWORD(wParam) == BN_CLICKED)
			{
				hFocusBtn = hBtnRing;				
				APP_PreviewRing(hWnd, IDM_RETURNMSG, "", 0, dwMaskStyle);				
			}
			break;
		case IDM_BTN_TEXT:
			if ( HIWORD(wParam) == BN_CLICKED)
			{
				hFocusBtn = hBtnText;				
				APP_PreviewText(hWnd,IDM_RETURNMSG, dwMaskStyle);				
			}
			break;

		case IDM_BTN_PHRASE:
			if ( HIWORD(wParam) == BN_CLICKED)
			{
				hFocusBtn = hBtnPhrase;
				APP_PreviewPhrase(hWnd, IDM_RETURNMSG, "",  0, dwMaskStyle);				
			}
			break;			
			
		case ID_EXIT:
			DestroyWindow(hWnd);
			break;

		case ID_OK:			
			hFocusBtn = GetFocus();
			SendMessage(hFocusBtn, WM_KEYDOWN, VK_F5, NULL);			
			break;		
		}
		
		break;
		
	case IDM_RETURNMSG:
		if ( HIWORD(wParam) != RTN_NOSELECT )
		{
			PostMessage( hWnd , WM_CLOSE , NULL , NULL );
			SendMessage(GetWindow(hWnd,GW_OWNER),uReturnMessage,wParam,lParam);//GetParent(hWnd)			
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd,NULL);
		
		EndPaint(hWnd,NULL);
		break;

	case WM_KEYDOWN:

	    switch(wParam)
		{
		case VK_RETURN:
			
			hFocusBtn = GetFocus();
			SendMessage(hFocusBtn, WM_KEYDOWN, VK_F5, NULL);			
			break;
			
		case VK_F2:

            DestroyWindow(hWnd);
			break;

		case VK_F10:

            DestroyWindow(hWnd);
			break;

		default:
			PostMessage(GetParent(hWnd),wMsgCmd,wParam,lParam);
			break;
		}
		break;


		

	case WM_DESTROY:
		if (hBmpBtnNormal)
		{
			DeleteObject(hBmpBtnNormal);
			hBmpBtnNormal = NULL;
		}

		if (hBmpBtnFocus)
		{
			DeleteObject(hBmpBtnFocus);
			hBmpBtnFocus = NULL;
		}

		hWndPreBrowMain = NULL;
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}
*/

/*************************************************************
初始化Button
\*************************************************************/
static	BOOL	InitButton( HWND hWnd )
{
	int i;
	i = 0;

	if ( dwStyle & PIC_PREBROW )
	{
		
		hBtnPic = CreateWindow(
			"BUTTON",IDS_PICTURES, 
			WS_TABSTOP|WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON |WS_BITMAP,
			PREBROW_BTN_STARTX,
			PREBROW_BTN_STARTY + PREBROW_BTN_INTERVAL * i,
			PREBROW_BTN_WIDTH,
			PREBROW_BTN_HEIGHT,
			hWnd, 
			(HMENU)IDM_BTN_PIC,
			NULL,
			NULL
			);

		if ( i == 0 )
			hFocusBtn = hBtnPic;
		i ++;
		

		if ( hBtnPic == NULL )
			return FALSE;
			/*	SendMessage(hBtnPic, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_DISABLE), (LPARAM)hBmpBtnNormal);
			SendMessage(hBtnPic, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_FOCUS), (LPARAM)hBmpBtnFocus);
			SendMessage(hBtnPic, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_PUSHDOWN), (LPARAM)hBmpBtnFocus);
		*/
	}
	
	if ( dwStyle & RING_PREBROW)
	{
		
		hBtnRing = CreateWindow(
			"BUTTON",IDS_RINGS, 
			WS_TABSTOP|WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON |WS_BITMAP,
			PREBROW_BTN_STARTX,		PREBROW_BTN_STARTY + PREBROW_BTN_INTERVAL*i,
			PREBROW_BTN_WIDTH,
			PREBROW_BTN_HEIGHT,
			hWnd, 
			(HMENU)IDM_BTN_RING,
			NULL,
			NULL
			);
		if ( i == 0 )
			hFocusBtn = hBtnRing;
		i++;
		

		if ( hBtnRing == NULL )
			return FALSE;
			/*	SendMessage(hBtnRing, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_DISABLE), (LPARAM)hBmpBtnNormal);
			SendMessage(hBtnRing, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_FOCUS), (LPARAM)hBmpBtnFocus);
			SendMessage(hBtnRing, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_PUSHDOWN), (LPARAM)hBmpBtnFocus);
		*/
	}
	
	if ( dwStyle & TEXT_PREBROW)
	{
		hBtnText = CreateWindow(
			"BUTTON",IDS_TEXTS, 
			WS_TABSTOP|WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON |WS_BITMAP,
			PREBROW_BTN_STARTX,
			PREBROW_BTN_STARTY + PREBROW_BTN_INTERVAL*i,
			PREBROW_BTN_WIDTH,
			PREBROW_BTN_HEIGHT,
			hWnd, 
			(HMENU)IDM_BTN_TEXT,
			NULL,
			NULL
			);
		if ( i == 0 )
			hFocusBtn = hBtnText;
		i++;
		
		if ( hBtnText == NULL )
			return FALSE;
			/*	SendMessage(hBtnText, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_DISABLE), (LPARAM)hBmpBtnNormal);
			SendMessage(hBtnText, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_FOCUS), (LPARAM)hBmpBtnFocus);
			SendMessage(hBtnText, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_PUSHDOWN), (LPARAM)hBmpBtnFocus);
		*/
	}
	
	if ( dwStyle & QTEXT_PREBROW)
	{
		hBtnPhrase = CreateWindow(
			"BUTTON",IDS_QTEXTS, 
			WS_TABSTOP|WS_CHILD | WS_VISIBLE |BS_PUSHBUTTON |WS_BITMAP,
			PREBROW_BTN_STARTX,
			PREBROW_BTN_STARTY + PREBROW_BTN_INTERVAL*i,
			PREBROW_BTN_WIDTH,
			PREBROW_BTN_HEIGHT,
			hWnd, 
			(HMENU)IDM_BTN_PHRASE,
			NULL,
			NULL
			);
		if ( i == 0 )
			hFocusBtn = hBtnPhrase;
		i++;

		if ( hBtnPhrase == NULL )
			return FALSE;
			/*	SendMessage(hBtnPhrase, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_DISABLE), (LPARAM)hBmpBtnNormal);
			SendMessage(hBtnPhrase, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_FOCUS), (LPARAM)hBmpBtnFocus);
			SendMessage(hBtnPhrase, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_PUSHDOWN), (LPARAM)hBmpBtnFocus);
			
		*/
	}
	
	

	return TRUE;	
}
#endif

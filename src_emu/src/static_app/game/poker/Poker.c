/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2005 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#define __MODULE__ "POKER"

#include "winpda.h"
#include "window.h"
#include "hpimage.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
//#include "fapi.h"
#include "plx_pdaex.h"
#include "Poker.h"
//#include "hp_icon.h"
//#include "msgwin.h"
#include "pubapp.h"
#include "GameHelp.h"
#include "plxdebug.h"



static char strmoney[30];
static char strlasthand[30];

//static HWND	hFrameWnd;
static	HMENU	hPokerMenu = NULL;


// Internal function prototypes

static BYTE Poker_random();
static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                   LPARAM lParam);
static LRESULT CALLBACK PokerGameWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                   LPARAM lParam);
//static LRESULT CALLBACK PokerHelpWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
  //                                 LPARAM lParam);

//static void	OnMenuCmdMsg( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

#ifndef SEF_SMARTPHONE
static HINSTANCE hInstance = NULL;
#endif
//  Function : POKER_AppControl
//  Purpose  :
//  Application control funciton.
static MENUITEMTEMPLATE PokerMenu[] =
{
	{ MF_STRING, IDM_START, "New", NULL},
	{ MF_STRING, IDM_HELP, "help", NULL },
//	{ MF_STRING, ID_SETTING, IDS_SETTING, NULL },
    { MF_END, 0, NULL, NULL }
};
//#endif

static const MENUTEMPLATE PokerMenuTemplate =
{
    0,
    PokerMenu
};

DWORD POKER_AppControl(int nCode, void* pInstance, WPARAM wParam,  LPARAM lParam)
{
    WNDCLASS wc;
    DWORD dwRet;
//	RECT	rf;

    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT :     
        
		hInstance = (HINSTANCE)pInstance;

        break;

   case APP_GETOPTION :

		if (wParam == AS_STATE)
		{
			if(hwndApp)
			{
			   if (IsWindowVisible(hwndApp))
					dwRet = AS_ACTIVE;
			  else
					dwRet = AS_INACTIVE;
			}
			else
			{
				dwRet = AS_NOTRUNNING;
			}
		}
		if (wParam == AS_APPWND)
			return (DWORD)hwndApp;
 
    case APP_ACTIVE :
		
        if (hwndApp)
        {
			ShowWindow(hwndApp, SW_SHOW);
			ShowOwnedPopups(hwndApp, TRUE);		
            //SetFocus(hwndApp);
        }
        else
        {
			
#ifdef _MODULE_DEBUG_
  StartObjectDebug();
#endif
			wc.style         = 0;
			wc.lpfnWndProc   = AppWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = NULL;
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = "PokerClass";
			
			RegisterClass(&wc);
			
			hPokerMenu = LoadMenuIndirect(&PokerMenuTemplate);
			ModifyMenu(hPokerMenu, IDM_START, MF_BYCOMMAND, IDM_START, IDS_NEW);
			ModifyMenu(hPokerMenu, IDM_HELP, MF_BYCOMMAND, IDM_HELP, IDS_HELP);
			
			//hFrameWnd = CreateFrameWindow(WS_VISIBLE|PWS_STATICBAR);
			//GetClientRect(hFrameWnd, &rf);
            // Creates window
            hwndApp = CreateWindow(
				"PokerClass",
				IDS_POKER,//GetString(STR_CARD_CAPTION), 
                WS_VISIBLE|WS_CAPTION|WS_CAPTION|PWS_STATICBAR,
				PLX_WIN_POSITION,
				NULL,
				hPokerMenu,
				NULL,
				NULL);
			SendMessage(hwndApp, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_EXIT,0), (LPARAM)IDS_EXIT);
			SendMessage(hwndApp, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_CONFIRM,1), (LPARAM)IDS_NEW);
			SendMessage(hwndApp, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			//PDASetMenu(hFrameWnd, hPokerMenu);
			UpdateWindow(hwndApp);
			//ShowWindow(hFrameWnd, SW_SHOW);			
			//SetFocus(hwndApp);			
        }
        break;

    case APP_INACTIVE :
		ShowWindow(hwndApp, SW_HIDE);
		ShowOwnedPopups(hwndApp, FALSE);
        break;
    }

    return dwRet;
}

static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                   LPARAM lParam)
{
    LONG lResult;    
    HDC hDC;
//	static int nTimer;
//	static  HBITMAP  hAppIcon;

    lResult = (LRESULT)TRUE;
    
    switch (wMsgCmd)
    {
    case WM_CREATE :
		/* Caption上的图标 */
		{
			COLORREF Color;
			BOOL bTran;
			HDC hdc;

			hdc = GetDC(hWnd);

			hBmpFace = CreateBitmapFromImageFile(hdc,IMG_FACE,&Color,&bTran);
//			hbmpCvMenu = CreateBitmapFromImageFile(hdc,IMG_MENU,&Color,&bTran);
//			hbmpCvCurs = CreateBitmapFromImageFile(hdc,IMG_CURS,&Color,&bTran);

			CoverBGClr = Color;
			ReleaseDC(hWnd,hdc);
		}
	//	hBmpFace = LoadImage(NULL, IMG_FACE, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);

		nSelMenuItem = 0;
		strcpy(MenuItemText[0],ML("New Game"));
		strcpy(MenuItemText[1], ML("Help"));
		break;
/*
	case PWM_SHOWWINDOW:
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_CONFIRM);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
		//UpdateWindow(hFrameWnd);
		SetFocus(hWnd);
		break;
*/

    case WM_PAINT:

		hDC = BeginPaint(hWnd, NULL);
		BitBlt(hDC, 0, 0, 176, 182, (HDC)hBmpFace, 0, 0, ROP_SRC);
//		if( !bEverEnter )
			DrawMenuItem(hDC,nSelMenuItem);
		EndPaint(hWnd, NULL);

		break;

//	case WM_PENMOVE:
//		{
//			POINT pt;
//			int nSel;
//			pt.x = LOWORD(lParam);
//			pt.y = HIWORD(lParam);
//			nSel = SelOnMenuRect(&pt);
//			if (nSel < 0)
//				break;
//			nSelMenuItem = nSel;
//			InvalidateRect(hWnd, NULL, FALSE);
//		}
//		break;

	case WM_PENUP:
		if( !bEverEnter )
		{
			POINT pt;
			int nSel;
			pt.x = LOWORD(lParam);
			pt.y = HIWORD(lParam);
			nSel = SelOnMenuRect(&pt);
			if (nSel < 0)
				break;
			nSelMenuItem = nSel;
			PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_CONFIRM,NULL),NULL);
		}
		break;
#ifdef _MXLINCOMMENT_
	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_RETURN:
			ShowPokerGameWnd();
			break;	
			
		case VK_F10:									//挂断键
			//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			PostMessage( hWnd, WM_CLOSE, 0, 0 );
			break;
		/*	
		case VK_F5://第五向键
			PostMessage(hFrameWnd, WM_KEYDOWN, VK_F5, NULL);
			break;
		*/	
		default:
			return (PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam));
			break;
		}
		break;
/*		if( bEverEnter )
		{
			if( wParam == VK_RETURN ) //第五向键
				PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_CONFIRM, NULL), NULL);
			if (wParam == VK_F10)
			{
				PostMessage(hWnd, WM_CLOSE,0 ,0);
			}
			break;
		}
*/


		switch( wParam )
		{
		case VK_UP:
			if (0==nSelMenuItem)
			{
				nSelMenuItem = 1;
				
			}
			else
				nSelMenuItem =0;
			InvalidateRect(hWnd,NULL,TRUE);
			break;

		case VK_DOWN:

			if (0==nSelMenuItem)
			{
				nSelMenuItem = 1;
				
			}
			else
				nSelMenuItem =0;
			InvalidateRect(hWnd,NULL,TRUE);
			break;

		case VK_RETURN:
		//case VK_F5://第五向键
			PostMessage(hWnd, WM_COMMAND, MAKEWPARAM(IDC_CONFIRM, NULL), NULL);
			break;			


		
		case VK_F10:									//挂断键
			//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			PostMessage( hWnd, WM_CLOSE, 0, 0 );
			break;
			
		default:
			return (PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam));
			break;
		}
		break;
#endif

	case WM_COMMAND:
        switch (LOWORD(wParam))
        {
		case IDC_CONFIRM:
			PostMessage(hWnd,WM_COMMAND,IDM_START,NULL);

			break;
		case IDM_START:
			ShowPokerGameWnd();
			break;

		case IDM_HELP:
			ShowPokerHelpWnd();
			break;
		case IDC_EXIT:
			/*
			if( bEverEnter )
			{
				bEverEnter = FALSE;
				//InvalidateRect( hWnd, NULL, TRUE );
			}
			else
			{
				bEverEnter = FALSE;
				//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage( hWnd, WM_CLOSE, 0, 0 );
			}
			*/
			bEverEnter = FALSE;
			//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			PostMessage( hWnd, WM_CLOSE, 0, 0 );
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;

        }
        break;

    case WM_CLOSE:

    	DestroyWindow(hWnd);
	
		break;

    case WM_DESTROY :

        if( hBmpFace )
			DeleteObject(hBmpFace);
        if( hbmpCvMenu )
			DeleteObject(hbmpCvMenu);
        if( hbmpCvCurs )
			DeleteObject(hbmpCvCurs);
		if (hPokerMenu)
		{
			DestroyMenu(hPokerMenu);
		}

		hBmpFace = NULL;
		hwndApp = NULL;
		//hFrameWnd = NULL;
		UnregisterClass( "PokerClass", NULL );
//		DlmNotify(PES_STCQUIT, (long)POKER_AppControl);
		DlmNotify((WPARAM)PES_APP_QUIT, (LPARAM)hInstance);

			
#ifdef _MODULE_DEBUG_
  EndObjectDebug();
#endif
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }

    return lResult;
}

BOOL ShowPokerHelpWnd()
{

	return CallGameHelpWnd(hwndApp, IDS_POKER_HELP);

}

BOOL ShowPokerGameWnd()
{
	WNDCLASS wc;
//	RECT	rf;

	wc.style         = 0;
	wc.lpfnWndProc   = PokerGameWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName  = "PokerPlayClass";

	if (!RegisterClass(&wc))
		return FALSE;

	//GetClientRect(hFrameWnd, &rf);
	hwndGame = CreateWindow(
		"PokerPlayClass",
		ML("Card"), 
        WS_VISIBLE|PWS_STATICBAR,
		/*
		rf.left,
		rf.top,
		rf.right - rf.left,
		rf.bottom - rf.top,
		hFrameWnd,
		*/
		PLX_WIN_POSITION,
		hwndApp,
		NULL,
		NULL,
		NULL
		);
	SendMessage(hwndGame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025, 0), (LPARAM)IDS_BACK);//"退出" 按纽
	SendMessage(hwndGame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025, 1), (LPARAM)STR_START);// "揭底"按纽STR_DRAW
	SendMessage(hwndGame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

	SetRect(&rcAllPoker, 0,0,176,147);

	strcpy( strmoney, "" );
	strlasthand[0] = '\0';
//	InvalidateRect( hWnd, &MoneyRect, TRUE );
//	DrawTipText("500 $", "");
	
	ShowWindow(hwndGame, SW_SHOW);
	UpdateWindow(hwndGame);

	SetFocus( hwndGame );

	return TRUE;
}

//************************************************************************
//          Internal functions
//************************************************************************

static LRESULT CALLBACK PokerGameWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{

    LONG lResult;    
    HDC hDC;
	int i;
	int x, y;
//	static int nTimer;
//	static  HBITMAP  hAppIcon;
	HPEN hPen,hOldPen;
    lResult = (LRESULT)TRUE;
    
    switch (wMsgCmd)
    {
    case WM_CREATE :

		PokerState = 0;
		bEverEnter = FALSE;
        for(i=0;i<5;i++)
		    PValue[i] = 0xff;
        Money = MONEY;
	    WinFlag = 0xff;
        srand( GetTickCount());
        BeginState = 1;

		SetTimer(hWnd, 1, 100, NULL);
		
		hBmpArray[0]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, Icon2);
		hBmpArray[1]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, Icon3);
		hBmpArray[2]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, Icon4);
		hBmpArray[3]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, Icon5);
		hBmpArray[4]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, Icon6);
		hBmpArray[5]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, Icon7);
		hBmpArray[6]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, Icon8);
		hBmpArray[7]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, Icon9);
		hBmpArray[8]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, Icon10);
		hBmpArray[9]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, IconJ);
		hBmpArray[10] = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, IconQ);
		hBmpArray[11] = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, IconK);
		hBmpArray[12] = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, IconA);

		

		hBmprArray[0]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIcon2);
		hBmprArray[1]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIcon3);
		hBmprArray[2]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIcon4);
		hBmprArray[3]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIcon5);
		hBmprArray[4]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIcon6);
		hBmprArray[5]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIcon7);
		hBmprArray[6]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIcon8);
		hBmprArray[7]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIcon9);
		hBmprArray[8]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIcon10);
		hBmprArray[9]  = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIconJ);
		hBmprArray[10] = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIconQ);
		hBmprArray[11] = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIconK);
		hBmprArray[12] = CreateBitmap(BMPWIDTH, BMPHEIGHT, 1, 1, rIconA);

		{
			COLORREF Color;
			BOOL bTran;
			HDC hdc;

			hdc = GetDC(hWnd);

			hBmpBegin = CreateBitmapFromImageFile( hdc, IMG_CARDBK, &Color, &bTran);
			hBmpBackGroud = CreateBitmapFromImageFile( hdc, IMG_GAMEBG, &Color, &bTran);
			hbmpCardFace = CreateBitmapFromImageFile( hdc, IMG_CARD, &Color, &bTran );

			hBmpfArray[0] = CreateBitmapFromImageFile( hdc, IMG_SPADE, &Color, &bTran );
			hBmpfArray[1] = CreateBitmapFromImageFile( hdc, IMG_HEART, &Color, &bTran);
			hBmpfArray[2] = CreateBitmapFromImageFile( hdc, IMG_DIAMD, &Color, &bTran);
			hBmpfArray[3] = CreateBitmapFromImageFile( hdc, IMG_CLUB, &Color, &bTran);

			ReleaseDC(hWnd,hdc);
		}
	//	hBmpBegin = LoadImage(NULL,IMG_CARDBK, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	//	hBmpBackGroud = LoadImage(NULL,IMG_GAMEBG, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
		InitPokerRect();	//计算牌的区域矩形		
		break;				//窗口创建后无焦点，刷新才有
//		
//	case PWM_SHOWWINDOW:
//		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));//"退出" 按纽
//		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_START);// "揭底"按纽STR_DRAW
//		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
//		SetFocus(hwndGame);

		break;
	case WM_ACTIVATE:
		if (wParam == WA_ACTIVE)
		{
			InvalidateRect( hWnd, &MoneyRect, TRUE );
			SetFocus(hWnd);
		}
		
		break;

	case WM_ERASEBKGND:
		break;

    case WM_PAINT:
		hDC = BeginPaint(hWnd, NULL);
		BitBlt(hDC, 0, 0, 176, 182, (HDC)hBmpBackGroud, 1, 0, ROP_SRC);
		for(i=0;i<5;i++)
		{
			DispPokersI(i,BeginState);
		}
		EndPaint(hWnd, NULL);
		//====================================================================
		DrawTipText(strmoney, strlasthand);
		//====================================================================
		break;

	case WM_PENDOWN:
		if(BeginState==0 && NewStep == 1)
		{
			{
				int ret;
				x = LOWORD(lParam);
				y = HIWORD(lParam);
				ret = PtInPoker(x,y);	//若笔点区域不是牌的区域，则保留原来的PokerID值		
				
				
				if(ret>=0 && ret < 5)
				{
					PokerID = ret;	
//					printf("PokerID = %d\r\n",PokerID);
					PokerProcess(PokerID);	//对牌作选中标记,并且加入到PValue中
					DrawPokerRectI(PokerID);
					return TRUE;
				}
				
				return FALSE;
			}
		}
		break;
 /*--------------------------ryx add 0130----------------------*/  
	case WM_KEYDOWN:
		hDC = GetDC(hWnd);	
		switch( wParam )
			{
			case VK_LEFT:
			case VK_UP:
				InvalidateRect(hWnd, &rcAllPoker,0);
				UpdateWindow(hWnd);
				if(BeginState==0 && NewStep == 1)
				{
					hPen = GetStockObject(WHITE_PEN);
					SelectObject(hDC,hPen);
			//		PokerDrawSelItem(hDC);
					PokerID--;			
					if (PokerID<0)
						PokerID=0;
					hPen = CreatePen(PS_SOLID, 1, RGB(255,51,0));
					SelectObject(hDC, (HGDIOBJ)hPen);
					PokerDrawSelItem(hDC);
					DeleteObject(hPen);	
				}
				break;
			case VK_RIGHT:
			case VK_DOWN:
				InvalidateRect(hWnd, &rcAllPoker,0);
				UpdateWindow(hWnd);
				if(BeginState==0 && NewStep == 1)
				{
					//hPen = GetStockObject(WHITE_PEN);
					//SelectObject(hDC,hPen);
	//				PokerDrawSelItem(hDC);
					PokerID++;
					if (PokerID>4)
						PokerID=4;
					hPen = CreatePen(PS_SOLID, 1, RGB(255,51,0));
					hOldPen = SelectObject(hDC, (HGDIOBJ)hPen);
					PokerDrawSelItem(hDC);
					SelectObject(hDC, hOldPen);
					DeleteObject(hPen);	
				}
				break;	
			case VK_F5://
//				printf("call vk_return\n");
				if(BeginState==0 && NewStep == 1)
				{
					hPen = CreatePen(PS_SOLID, 1, RGB(255,51,0));
					hOldPen = SelectObject(hDC, (HGDIOBJ)hPen);
					PokerDrawSelItem(hDC);
					PokerProcess(PokerID);	//
					DrawPokerRectI(PokerID);
					SelectObject(hDC, hOldPen);
					DeleteObject(hPen);
				}
				else
				{
					POSTCOMMAND( IDC_BUTTON_DRAW, 0 );
				}
				break;			


			case VK_F10:				
				PLXConfirmWinEx(NULL,hWnd, IDS_GAME_PROMPT, Notify_Request, ML("POKER"), IDS_CONFIRM, IDS_CANCEL, IDRM_RMSG);
				
				break;
			case VK_RETURN:
				PokerID = 0;
				
				OnButtonDraw(hWnd);				
				InvalidateRect( hWnd, &MoneyRect, TRUE );
				break;

			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
		}
		ReleaseDC(hWnd, hDC);
		break;

	case IDRM_RMSG:
		if (lParam == 1) {
			CLOSEWINDOW(hWnd);
		}
		break;


	case IDRM_GAMEOVER:
		CLOSEWINDOW(hWnd);
		break;

    case WM_CLOSE:
		//SendMessage(hFrameWnd, PWM_CLOSEWINDOW, (LPARAM)hWnd, 0);
    	DestroyWindow(hWnd);
		
		break;

    case WM_DESTROY:
		KillTimer(hWnd,1);
		UnregisterClass("PokerPlayClass", NULL);
		for(i=0;i<13;i++)
		{
			DeleteObject(hBmpArray[i]);
			hBmpArray[i] = NULL;
			DeleteObject(hBmprArray[i]);
			hBmprArray[i] = NULL;
		}
		for(i=0;i<4;i++)
		{
			DeleteObject(hBmpfArray[i]);
			hBmpfArray[i] = NULL;
		}

		for(i=0;i<7;i++)
		{
			DeleteObject(hBmplfArray[i]);
			hBmplfArray[i] = NULL;
		}

		if( hBmpBegin )
			DeleteObject( hBmpBegin );
		if( hBmpBackGroud )
			DeleteObject( hBmpBackGroud );
		if( hbmpCardFace )
			DeleteObject( hbmpCardFace );

		hBmpBegin = NULL;
		hBmpBackGroud = NULL;

		for(i=0; i<5;i++)
			Poker[i].State = 0;
/*
#ifndef SEF_SMARTPHONE
		DlmNotify(PES_APP_QUIT, (int)hInstance);
		hInstance = NULL;
#else
		DlmNotify(PES_STCQUIT, (long)POKER_AppControl);
#endif*/
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;

}


//----------------------------------------------------------------------------
// 函数名：OnButtonDraw()
// 功  能：点击Draw按钮时的处理
// 参  数：
//----------------------------------------------------------------------------
void OnButtonDraw(HWND hWnd) 
{
    int i;
	if(Money<=0)
	{
		if (!bEverEnter)
		{
			bEverEnter = TRUE;
		//	MsgWin(STR_LOSE, GetString(STR_CARD_CAPTION),WAITTIMEOUT);

			PLXTipsWin(NULL, hWnd, IDRM_GAMEOVER, STR_GAMEOVER,ML("Poker"),Notify_Info, ML("OK") ,NULL,WAITTIMEOUT);
		
			//CLOSEWINDOW( hWnd );
		}
		return ;
	}

    if(BeginState == 1)
        BeginState = 0;

	//a new game
    if(PokerState ==0 || PokerState ==2)
	{
		char pTemp[30];
		for(i=0; i<5;i++)
		{
			Poker[i].State = 0;
			PValue[i] = 0xff;
		}
		PostPokers();
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_DRAW);
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		sprintf(pTemp,"%d",Money);

		strcpy( strmoney, pTemp );
		strlasthand[0] = '\0';
		InvalidateRect( hWnd, &MoneyRect, TRUE );
//		DrawTipText(pTemp, "");
//		SetWindowText( hLastEdit, "200"); 
		NewStep = 1;
		InvalidateRect(hWnd, &rcAllPoker,0);
	}
	//end a game
	else if(PokerState ==1)
	{
		ReCreatePokers();
		
		CalcPokers();
		DispResult();  
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_STEP);
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		for(i=0; i<5;i++)
			Poker[i].State = 0;
		NewStep = 0;
		InvalidateRect(hWnd, &rcAllPoker,0);
	}

	PokerState ++;
	if(PokerState>2)
		PokerState =1;

    UpdateWindow(hWnd);
}

//----------------------------------------------------------------------------
// 函数名：Poker_random()
// 功  能：产生所需的随机数
// 参  数：
//----------------------------------------------------------------------------
static BYTE Poker_random()
{
	BYTE Value, Face;
	int i;

	Value = rand() % 13;
	i = rand();							//RAND()出来的都是偶数，原因待查
	Face = ( i / 2 )% 4;

	return(( Value << 4 )+Face );
	
//	random = (rand() & 0x0f);
//	if(random > 12)
//		random = 12;
/*	while((random = (rand() & 0x0f))>12);
	Value = (random<<4);
	random = (rand() & 0x03);
	Value += random;
	return Value;*/

}

//----------------------------------------------------------------------------
// 函数名：PostPokers()
// 功  能：产生牌
// 参  数：
//----------------------------------------------------------------------------
void PostPokers()
{
	BYTE Value;
	BOOL bSame;
    int i,j;

	i = GetTickCount();
	srand( i );

	for(i=0; i<5;i++)
	{
	//	if(Poker[i].State)
	//		continue;
		Value = Poker_random();
		bSame = 0;
		for(j=0;j<i;j++)
		{
			if(Value == Poker[j].Value)
			{
				bSame=1;
				break;
			}
		}
		if(bSame)
		{
			i--;
			continue;
		}
//printf( "********* %s\r\n%d: %x\r\n", __FILE__, __LINE__, ( long )Value );

		Poker[i].Value = Value;	
//printf( "********* %s\r\n%d: %x\r\n", __FILE__, __LINE__, ( long )Poker[i].Value );
		}
}

//----------------------------------------------------------------------------
// 函数名：ReCreatePokers()
// 功  能：产生牌
// 参  数：
//----------------------------------------------------------------------------
void ReCreatePokers()
{
    int i,j;
	BYTE Value;
	BOOL bSame;

	i = GetTickCount();
	srand( i );


	for(i=0; i<5;i++)
	{
		
		if(Poker[i].State)
			continue;
		Value = Poker_random();
		bSame = 0;
		for(j=0;j<5;j++)
		{
			if(Value ==	PValue[j])
			{
				bSame=1;
				break;
			}
		}
		if(bSame)
		{
			i--;
			continue;
		}
//printf( "********* %s\r\n%d: %x\r\n", __FILE__, __LINE__, ( long )Value );
		PValue[i] = Value;
		Poker[i].Value = Value;
//printf( "********* %s\r\n%d: %x\r\n", __FILE__, __LINE__, ( long )Poker[i].Value );
 	}
}

//----------------------------------------------------------------------------
// 函数名：DispResult()
// 功  能：显示结果
// 参  数：
//----------------------------------------------------------------------------
void DispResult()
{
	if(WinFlag <=9)
        Money += WinState[WinFlag].MoneyNum;


	else
		Money -= V_MONEY;


	if(WinFlag <=9)
    {
        sprintf(strmoney,"%d",Money);
		sprintf(strlasthand,"%s +%d",ML(WinState[WinFlag].stridState),WinState[WinFlag].MoneyNum); //ryx del 1205
    }
	else
    {
		if (Money < 0) {
			Money = 0;
		}
        sprintf(strmoney,"%d",Money);
        //sprintf(strlasthand,"%s +%d",STR_PITY, 0);
		sprintf(strlasthand,"%s -%d",STR_PITY, V_MONEY);
    }

//	DrawTipText(strmoney, strlasthand);
}

//----------------------------------------------------------------------------
// 函数名：DispPokersI()
// 功  能：显示牌面位图
// 参  数：RECT rc 牌的大小和位置.
//			int i 牌的序号，
//			BYTE sat 显示状态（即要显示的是开始时的牌面,还是正常的牌面）
//----------------------------------------------------------------------------
void DispPokersI(int i, BYTE sat)
{
	HDC hDC;
    BYTE value;
    BYTE face;
//    BYTE rop;
	RECT rc;
//	HBRUSH hBrush;

	hDC = GetDC(hwndGame);
//	CopyRect(&rc, &(PokerRect[i])); 

	rc.left = PokerRect[i].left;
	rc.top = PokerRect[i].top;
	rc.right = PokerRect[i].right;
	rc.bottom = PokerRect[i].bottom;
//printf( "********* %s\r\n%d: ( %d ) %d, %d, %d, %d\r\n", __FILE__, __LINE__, ( long )&rc,
//	   rc.left, rc.top, rc.right, rc.bottom );

    if(sat == 1)
    {//绘制牌的背面
        DispBeginBmp( hDC, PokerRect[i], FALSE );
    }
	else 
    {//正常牌面显示

		int OldMode = SetBkMode( hDC, OPAQUE );

        DispBeginBmp( hDC, PokerRect[i], TRUE );

		SetBkMode( hDC, OldMode );
//		SetBkMode( hDC, TRANSPARENT );
        value = ( BYTE )((Poker[i].Value & 0xf0)>>4 );//
        face = ( BYTE )( Poker[i].Value & 0x03 );//

//printf( "********* %s\r\n%d: %x - %x\t\t( %d ) %d, %d, %d, %d\r\n", __FILE__, __LINE__,
//	   ( long )Poker[i].Value,( long )value, ( long )&rc,
//	   rc.left, rc.top, rc.right, rc.bottom );
        DispFaceBmp( hDC, &rc, face );
		DispValueBmp( hDC, &rc, value );
//        DispFaceBmp( hDC, PokerRect[i], face );
//		DispValueBmp( hDC, PokerRect[i], value );
		if(Poker[i].State && PokerState ==1)
		{
			InvertRect(hDC,&PokerRect[i]);
		}

		SetBkMode( hDC, OldMode );
    }	
    ReleaseDC(hwndGame, hDC);
}

//----------------------------------------------------------------------------
// 函数名：DiapFaceBmp()
// 功  能：显示牌的花色的位图
// 参  数：HDC，输出位置的坐标，牌的花色，ROP
//----------------------------------------------------------------------------
static void DispFaceBmp(HDC hDC, const RECT* prc, BYTE face)
{
//	int x,y,x1, y1, x2, y2 ;
	int x,y;//,x1, y1, x2, y2 ;
	x = prc->left + (prc->right- prc->left)/2- 5;
	y = prc->top +  (prc->bottom- prc->top)/2- 5;
//	x1 = rc.left + 6;
//	y1 = rc.top + 2 + BMPWIDTH;
//	x2 = rc.right - BMPWIDTH+2;
//	y2 = rc.bottom - 2 - BMPWIDTH - LBMPWIDTH;

//printf( "********* %s\r\n%d: ( %d ) %d, %d, %d, %d\r\n", __FILE__, __LINE__, ( long )prc,
//	   prc->left, prc->top, prc->right, prc->bottom );

//	BitBlt(hDC,x,y,BMPWIDTH,BMPWIDTH,(HDC)hBmpfArray[face],0,0,ROP);
	BitBlt(hDC,x,y,10,11,(HDC)hBmpfArray[face],0,0,ROP_SRC);
//	BitBlt(hDC,x1,y1,LBMPWIDTH,LBMPWIDTH,(HDC)hBmplfArray[face],0,0,ROP);
//	BitBlt(hDC,x2,y2,LBMPWIDTH,LBMPWIDTH,(HDC)hBmplfArray[face + 4],0,0,ROP);

    
}

//----------------------------------------------------------------------------
// 函数名：DispValueBmp()
// 功  能：显示牌的大小的位图
// 参  数：HDC，输出位置的坐标，牌的大小,ROP
//----------------------------------------------------------------------------
static void DispValueBmp(HDC hDC, const RECT* prc, BYTE value)
{
	int x,y,x1, y1 ;
	
	x = prc->left + 2;
	y = prc->top + 2;
	x1 = prc->right - 2 - BMPWIDTH1;//2
	y1 = prc->bottom - 2 - BMPWIDTH1;//2


	BitBlt(hDC,x,y,BMPWIDTH1,BMPWIDTH1,(HDC)hBmpArray[value],0,0,ROP_SRC);
	BitBlt(hDC,x1,y1,BMPWIDTH1,BMPWIDTH1,(HDC)hBmprArray[value],0,0,ROP_SRC);

}

//----------------------------------------------------------------------------
// 函数名：DispBeginBmp()
// 功  能：显示开始时的牌面
// 参  数：HDC，输出位置的坐标，bFace表示正面
//----------------------------------------------------------------------------
void DispBeginBmp( HDC hDC, RECT rc, BOOL bFace )
{
	if( bFace )
		BitBlt(hDC,rc.left, rc.top, CARDWIDTH, CARDHEIGHT, (HDC)hbmpCardFace, 0, 0, ROP_SRC);
	else
		BitBlt(hDC,rc.left, rc.top, CARDWIDTH, CARDHEIGHT, (HDC)hBmpBegin, 0, 0, ROP_SRC);
}
//----------------------------------------------------------------------------
// 函数名：InitPokerRect()
// 功  能：计算牌的区域矩形
// 参  数：
//----------------------------------------------------------------------------

void InitPokerRect()
{
	RECT   rect;
	int i;
	for(i=0;i<5;i++)
	{
		if(i<3)
		{
			SetRect(&rect, CARDWIDTH*i+CARDLEFT*(i+1), CARDTOP, 
				CARDWIDTH*(i+1)+CARDLEFT*(i+1), CARDTOP+CARDHEIGHT);
		}else 
		{
			SetRect(&rect, CARDWIDTH*(i-3)+CARDLEFT*(i-2), CARDHEIGHT+CARDTOP+3,
				CARDWIDTH*(i-2)+CARDLEFT*(i-2), CARDHEIGHT*2 + CARDTOP*2 -4);
		}
		CopyRect(&(PokerRect[i]),&rect);
	}
}

//----------------------------------------------------------------------------
// 函数名：DrawPokerRect()
// 功  能：画牌的矩形
// 参  数：HDC
//----------------------------------------------------------------------------

void DrawPokerRect(HDC hdc)
{
	int i;
	HBRUSH hBrush;
	hBrush = GetStockObject(WHITE_BRUSH);
	for(i=0;i<5;i++)
	{
		DrawRectI(hdc, &PokerRect[i],i);
		DrawLine(hdc,PokerRect[i].right,PokerRect[i].top,PokerRect[i].right,PokerRect[i].bottom);
		DrawLine(hdc,PokerRect[i].left,PokerRect[i].bottom,PokerRect[i].right,PokerRect[i].bottom);
		//FillRect(hdc, &PokerRect[i], hBrush);
		
	}
}
//----------------------------------------------------------------------------
// 函数名：DrawPokerRectI()	
// 功  能：画某张牌并反转
// 参  数：牌的标号
//----------------------------------------------------------------------------
void DrawPokerRectI(int i)		
{
	RECT  rect;
	CopyRect(&rect, &(PokerRect[i]));
	InvalidateRect(hwndGame,&rect, 1);
}

//----------------------------------------------------------------------------
// 函数名：DrawRectI()
// 功  能：画个矩形
// 参  数：HDC，矩形结构指针，矩形标号
//----------------------------------------------------------------------------
void DrawRectI(HDC hdc, const RECT* pRect, int i)
{

	HPEN hpenOld, hpenDraw;
	
	if(Poker[i].State)	
		hpenDraw = CreatePen(PS_SOLID, 1, COLOR_WHITE);
	else
		hpenDraw = CreatePen(PS_SOLID, 1, COLOR_BLACK);

	hpenOld = SelectObject(hdc, hpenDraw);   

	DrawLine(hdc,(*pRect).left-1,(*pRect).top-1,(*pRect).right,(*pRect).top-1);
	DrawLine(hdc,(*pRect).right,(*pRect).top-1,(*pRect).right,(*pRect).bottom);
	DrawLine(hdc,(*pRect).right,(*pRect).bottom,(*pRect).left-1,(*pRect).bottom);
	DrawLine(hdc,(*pRect).left-1,(*pRect).bottom,(*pRect).left-1,(*pRect).top-1);

	SelectObject(hdc, hpenOld);
	DeleteObject(hpenDraw);	

}

//----------------------------------------------------------------------------
// 函数名：CalcPokers()
// 功  能：对牌面进行计算
// 参  数：
//----------------------------------------------------------------------------
void CalcPokers()
{
    int i;
	for(i=0; i<5; i++)
	{
		PValue[i] = Poker[i].Value;
	}
	SortValue();
	AnlsValue();
}

//----------------------------------------------------------------------------
// 函数名：AnlsValue()
// 功  能：对牌面进行分析
// 参  数：
//----------------------------------------------------------------------------
void AnlsValue()          
{
    int i,j;
    BYTE f_straight= 0  , f_3kind = 0 , f_2pairs;
	BYTE pair[2];
	f_2pairs = 0;

	WinFlag = 0xff;

	if(HI(PValue[1]) == (HI(PValue[0]) + 0x10) &&
		HI(PValue[2]) == (HI(PValue[1]) + 0x10) &&
		HI(PValue[3]) == (HI(PValue[2]) + 0x10) &&
		HI(PValue[4]) == (HI(PValue[3]) + 0x10) 
		)

	{
		f_straight = 1;         //顺子
		WinFlag = STRAIGHT;
	}
	else if(HI(PValue[0]) == HI(PValue[3]) ||
			HI(PValue[1]) == HI(PValue[4]) )
	{
		WinFlag = FOURKINDS;    //四张相同
		return ;
	}
	else if(HI(PValue[0]) == HI(PValue[2]) ||
			HI(PValue[1]) == HI(PValue[3]) ||
			HI(PValue[2]) == HI(PValue[4]))
	{
		f_3kind = 1;            //三张相同
		WinFlag = THREEKINDS;
	}
	else
	{
		for(j=0; j<4; j++)
		{
			if(HI(PValue[j]) == HI(PValue[j+1]))
			{
				pair[f_2pairs] = j;
				f_2pairs ++;
			}
		}
		if(f_2pairs == 1)
		{
			if( HI(PValue[pair[0]]) >= 0x90 )
			{
				WinFlag = HIJACK;       //一个大于J的对
				return ;
			}
		}
		else if(f_2pairs == 2)
		{
			WinFlag = TWOPAIRS;         //两个对
			return ;
		}
	}
	if(f_3kind )
	{
		if(( HI(PValue[0]) == HI(PValue[1]) && HI(PValue[2]) == HI(PValue[4])) ||
			( HI(PValue[0]) == HI(PValue[2]) && HI(PValue[3]) == HI(PValue[4])))
			WinFlag = FULL_HOUSE;       //三张相同和二张相同的牌

		return ;
	}

	//花色判断
	for(i=0; i<4; i++)
	{
		if(LO(PValue[i]) != LO(PValue[i+1]))
			return ;
	}

	if(f_straight)
	{
		WinFlag = STRAIGHT_FLUSH;       //同花顺
	}
	else
		WinFlag = FLUSH;                //同花
}

//----------------------------------------------------------------------------
// 函数名：SortValue()
// 功  能：对牌进行排序
// 参  数：
//----------------------------------------------------------------------------
void SortValue()
{
    int i,j;
	BYTE temp;
	for(i=4; i >= 0; i--)
	{
		for(j=0; j<i; j++)
		{
			if(PValue[j]>PValue[j+1])
			{
				temp		= PValue[j];
				PValue[j]   = PValue[j+1];
				PValue[j+1] = temp;
			}
		}
	}
}

//----------------------------------------------------------------------------
// 函数名：PtInPoker()
// 功  能：判断点在哪张牌里
// 参  数：点的坐标
// 输  出：牌的标号
//----------------------------------------------------------------------------
int PtInPoker(int x, int y)
{
	int i;
	for(i=0;i<5;i++)
	{
		if(PtInRectXY(&PokerRect[i], x, y))
				return i;
	}
	return(-1);
}

//----------------------------------------------------------------------------
// 函数名：PokerProcess()
// 功  能：点击牌面时的处理
// 参  数：牌的序号
//----------------------------------------------------------------------------
void PokerProcess(int id)
{
	if(id>=0 && id <5)
	{
		Poker[id].State = !Poker[id].State;
		if(Poker[id].State)
			PValue[id] = Poker[id].Value;
		else
			PValue[id] = 0xff;
	}
}


//------------------------------------------------------------------------------
// The follow is some functions about pseudorandom number .
//------------------------------------------------------------------------------

/*/Sets a random starting point
static void srand ( unsigned int seed)
{
	holdrand = (long)seed;
}*/

/*
static void	OnMenuCmdMsg( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{	
	switch (LOWORD(wParam))
	{
	case IDM_FILEEXIT:
		PostMessage( hWnd, WM_CLOSE, 0, 0 );
		break;
	case IDM_ABOUTHLP:
//		DlmNotify( PS_HELP, (LPARAM)"ROM:帮助.EBK\\纸牌");

		break;
	case IDM_ABOUTVER:
//		DialogBoxIndirect ( NULL, (LPDLGTEMPLATE)&VersionDlg, NULL, VERDlgProc);

		break;
		
	}
	return;
}
*/
static void PokerDrawSelItem(HDC hdc)
{
	POINT	Pt;

	MoveTo(hdc, (PokerRect[PokerID].left -2), (PokerRect[PokerID].top-2), &Pt);
	LineTo(hdc, (PokerRect[PokerID].right+1), (PokerRect[PokerID].top-2));
	LineTo(hdc, (PokerRect[PokerID].right+1), (PokerRect[PokerID].bottom+1));
	LineTo(hdc, (PokerRect[PokerID].left - 2), (PokerRect[PokerID].bottom+1));
	LineTo(hdc, (PokerRect[PokerID].left - 2), (PokerRect[PokerID].top-2));
}

void DrawTipText(char* pMoney, char* pTip)
{
	HDC hdc;
	int nOldMode;
	COLORREF OldColor;
	HPEN	hPen, hOldPen;
	HBRUSH  hBrush, hOldBrush;

	hdc = GetDC(hwndGame);
	
	nOldMode = SetBkMode(hdc, TRANSPARENT);
	OldColor = SetTextColor(hdc, RGB(255,255,255));

	hPen = CreatePen(PS_SOLID, 1, RGB(0,102,204));
	hBrush = CreateBrush(BS_SOLID,RGB(0,102,204), 0);
	hOldPen = SelectObject(hdc, hPen);
	hOldBrush = SelectObject(hdc, hBrush);
	Rectangle(hdc, MoneyRect.left, MoneyRect.top, MoneyRect.right, MoneyRect.bottom);
	DrawText(hdc, pMoney, strlen(pMoney), &MoneyRect, DT_CENTER|DT_VCENTER);

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);

	hPen = CreatePen(PS_SOLID, 1, RGB(0,0,64));
	hBrush = CreateBrush(BS_SOLID,RGB(0,0,64), 0);
	hOldPen = SelectObject(hdc, hPen);
	hOldBrush = SelectObject(hdc, hBrush);
	Rectangle(hdc, TipRect.left, TipRect.top, TipRect.right, TipRect.bottom);
	DrawText(hdc, pTip, strlen(pTip), &TipRect, DT_CENTER|DT_VCENTER);

	SelectObject(hdc, hOldPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hPen);
	DeleteObject(hBrush);

	SetTextColor(hdc, OldColor);
	SetBkMode(hdc, nOldMode);

	ReleaseDC(hwndGame, hdc);

	return;
}
/*********************************************************************\
* Function	   DrawMenuItem
* Purpose      画菜单项
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL DrawMenuItem(HDC hdc, int nSel)
{
#ifdef _MXLINCOMMENT_

	COLORREF	OldBkClr,OldClr,OldTextClr;
	int			OldStyle;

	if(( 0>nSel )||( MENU_ITEM_NUM<=nSel ))
	{
		return FALSE;
	}
	OldBkClr = SetBkColor( hdc, CoverBGClr );
	OldStyle = SetBkMode(hdc, NEWTRANSPARENT);

	BitBlt( hdc, MENU_POS, ( HDC )hbmpCvMenu, 0, 0, ROP_SRC );			//画菜单
	BitBlt( hdc, CURS_X, CURS_Y + CURS_DY * nSel, CURS_CX, CURS_CY,		//画光标
			( HDC )hbmpCvCurs, 0, 0, ROP_SRC );

	SetBkColor( hdc, OldBkClr );
	SetBkMode(hdc, OldStyle);

	rect.left = 0;
	rect.right = 240;
	rect.top = MENUITEM_TOP + nSel*MENUITEM_INTERVAL;
	rect.bottom = MENUITEM_TOP + (nSel+1)*MENUITEM_INTERVAL;

	hPen = CreatePen(PS_SOLID,1,SEL_COLOR);
	hBrush = CreateBrush(BS_SOLID,SEL_COLOR,NULL);

	hOldPen = SelectObject(hdc,hPen);
	hOldBrush = SelectObject(hdc,hBrush);
	Rectangle(hdc,rect.left,rect.top,rect.right,rect.bottom);
	SelectObject(hdc,hOldPen);
	SelectObject(hdc,hOldBrush);

	DeleteObject(hPen);
	DeleteObject(hBrush);

	OldClr = SetBkColor(hdc,COLOR_WHITE);
	OldStyle = SetBkMode(hdc, TRANSPARENT);
	OldTextClr = SetTextColor(hdc,COLOR_WHITE);

	for (i=0; i<MENU_ITEM_NUM; i++)
	{
		rect.left = 0;
		rect.right = 240;
		rect.top = MENUITEM_TOP + i*MENUITEM_INTERVAL;
		rect.bottom = MENUITEM_TOP + (i+1)*MENUITEM_INTERVAL;
		DrawText(hdc, MenuItemText[i], -1, &rect, DT_VCENTER | DT_HCENTER);
	}

	SetTextColor(hdc,OldTextClr);

	SetBkColor(hdc, OldClr);
	SetBkMode(hdc, OldStyle);

	#endif
	return TRUE;
}

int SelOnMenuRect(POINT* pPoint)
{
	RECT rect;
	int i;

	rect.left = 0;
	rect.right = 240;
	for (i=0; i<MENU_ITEM_NUM; i++)
	{
		rect.top = MENUITEM_TOP + i*MENUITEM_INTERVAL;
		rect.bottom = MENUITEM_TOP + (i+1)*MENUITEM_INTERVAL;
		if (PtInRect(&rect, *pPoint))
			return i;
	}
	return -1;
}

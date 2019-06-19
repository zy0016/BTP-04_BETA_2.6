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
#define __MODULE__ "GOBANG"
#include "window.h"
#include "winpda.h"
#include "string.h"
#include "malloc.h"
#include "stdlib.h"
#include "stdio.h"
//#include "fapi.h"
#include "plx_pdaex.h"
#include "hpimage.h"
#if defined SCP_SMARTPHONE
#include "MsgWin.h"
#endif 
#include "public/GameHelp.h"
#include "super5.h"
//左上角图标
#ifdef SCP_SMARTPHONE
#include "hp_icon.h"
#define	DIRICON		"/rom/game/gobang/gobang_8.ico"//GAME_RUSSIA_ICON//"ROM:hrd.ico"
//#else
//#define DIRICON     "ROM:GOBAND_GOBAND.gif"
#endif
#include "mullang.h"

#define WM_ROBOTPLAY (WM_USER + 200)

#define ID_NEW			WM_USER+ 101
#define ID_UNDO			WM_USER+ 102
#define ID_QUIT			WM_USER+ 103
#define ID_SETTING		WM_USER+ 104
#define ID_HUMAN		WM_USER+ 105
#define ID_ROBOT		WM_USER+ 106
#define ID_HELP			WM_USER+ 107
#define	ID_SURE			WM_USER+ 108
#define	IDRM_RMSG		(WM_USER+ 100)

#define	IDM_MANVSCOM	(WM_USER+110)
#define	IDM_MANVSMAN	(WM_USER+111)

#define	IDS_MANVSCOM	ML("Human Vs Computer")
#define	IDS_MANVSMAN	ML("Human Vs Human")

//static BOOL CreateGobangSetting(HWND hWnd);
static BOOL CreateGobangGame(HWND hWnd);
static LRESULT CALLBACK SettingsWndProc(HWND hDlg, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT message, WPARAM wParam,LPARAM lParam);
static	BOOL	ShowArrowState(HWND hWnd,int nSel);
static BOOL	SetMenuItemRectLocal();
static	int	GetAreaByPos(int x,int y);
static BOOL MyDrawMenuItem(HDC hdc,int i,int nSel);
static DWORD	OnGameHelp(HWND hWnd);
static void DrawMsg(HDC hdc);
static void DrawToGoSide(HDC hdc,int color);
static void DrawWonFigure(HDC hdc, int color);
static void UpdateStatus(HWND hWnd);

static HINSTANCE	hInstance;		//实例句柄
static HWND			hHopenApp;		//主窗口句柄
static HWND			hWndGobangSettings;	//设置窗口句柄
static HWND			hBtnHumen;		//人机对战按钮
static HWND			hBtnRobot;		//猪八戒按钮
static HBITMAP		hCoverBg;		//封面背景图句柄
static HBITMAP		hBmpSel;		//选中图标句柄
/*static HBITMAP		hBmpMenu;*/
static HBITMAP      hText;			//菜单文字句柄
static int			nSelMenuItem;	//选中菜单项
static HWND			hWndHelp;		//帮助窗口句柄
static HBITMAP		hBmpToGoCol;
static HCURSOR 		hHandCursor;
static COORDINATEPOINT	curcoordinate;
static	HMENU		hMainMenu	=	NULL;


//#if defined SCP_NOKEYBOARD

#define	X_COVERBG				0
#define	Y_COVERBG				0
#define	COVERBG_WIDTH			176
#define	COVERBG_HEIGHT			196
#define	X_LT_MENUITEM			40
#define	Y_LT_MENUITEM			86
#define	Y_MENUITEM_INTERVAL		23//24
#define	MENUITEM_WIDTH			110
#define	MENUITEM_HEIGHT			18
#define	X_LT_SELITEM			23//20
#define	Y_LT_SELITEM			85//91
#define	SELITEM_WIDTH			11
#define	SELITEM_HEIGHT			11
#define X_ALLMENU				40
#define Y_ALLMENU				86
#define ALLMENU_WIDTH			110
#define ALLMENU_HEIGHT			65

#define	X_TOGO					10
#define	Y_TOGO					170//0
#define	TOGO_WIDTH				12
#define	TOGO_HEIGHT				11
#define X_WON					50
#define Y_WON					180//0
//#endif


#define CreateCapButton(hwnd, lowparam, hiwparam, lparam) \
	SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(lowparam,hiwparam), (LPARAM)lparam)
#define MENU_ITEM_NUM	3
#define MSG_SIDE		"该%s下棋..."
#define MSG_ROBOT		"猪八戒思考中..."

//#ifdef	SCP_SMARTPHONE
#include	"str_public.h"
#include	"str_plx.h"

#define	IDS_OK	ML("Ok")

#define	IDS_GOBANG		ML("Gobang")	//"五子棋"
//#define	IDS_NEWGAME		ML("NEW GAME")		//"新局"
//#define	IDS_UNDO		ML("UNDO")		//"悔棋"
#define	IDS_NEWGAME		"New"//ML("New")
#define	IDS_UNDO		"Undo"//ML("Undo")
#define	IDS_SETTING		ML("Setting")	//"设置"
#define IDS_CONFIRM		ML("Ok")		//"确定"
#define	IDS_EXIT		ML("Back")		//"退出"
#define IDS_HELP		ML("Help")		//"帮助"
#define IDS_BACK		ML("Back")		//"返回"
#define	IDS_SURE		ML("Sure")

#define	IDS_HELPTEXT	ML("Gobanghelp")
//#define IDS_HELPTEXT	("Object:\nThe goal of the game is to make an unbroken row of five stones whether vertically, horizontally, or diagonally.\nHint:\nPlayers(White and Black) alternate to put a stone on the intersections. Both the player vs computer mode and the player vs player mode are supported.")

#define	IDS_DRAW		ML("K.O.\nDraw in Chess")
#define IDS_PROMPT		ML("Prompt")	//"提示"
#define MSG_WON			"%s\nWON"	//"%s赢！"
#define MSG_BLACK		ML("Black")		//"黑方"
#define MSG_WHITE		ML("White")		//"白方"
#define	IDS_GAME_PROMPT	ML("Are you sure?")		//"是否确定退出？"
//#define	IDS_GAME_PROMPT	"是否确定退出?"	
//#endif

static char msg[50];
static int game_stopped;
static int robot_side, robot_new;
static char robot_name[20];
static int last_x, last_y, last_color;
static	int flicker=1;
static HMENU	hMenu = NULL;		//窗口句柄
static RECT		MenuItemRect[MENU_ITEM_NUM];
static RECT		SelItemRect[MENU_ITEM_NUM];
static BOOL	bGobangSettingsClass = FALSE;
static BOOL	bSUPER5Class = FALSE;
static BOOL bUndoPermit;
static	int				lastcoordinatex = -1;
static	int				lastcoordinatey = -1;
static RECT rcMsg =
{
	0, 165, 176, 180
};
/*
//菜单模版
#ifndef	SCP_SMARTPHONE
static MENUITEMTEMPLATE GoBangGameMenu[] =
{
	{ MF_STRING, ID_NEW, NULL, NULL},
	{ MF_STRING, ID_UNDO, NULL, NULL },
//	{ MF_STRING, ID_SETTING, NULL, NULL },
    { MF_END, 0, NULL, NULL }
};
#else
*/
static MENUITEMTEMPLATE GoBangMainMenu[] =
{
	{ MF_STRING, IDM_MANVSCOM, IDS_NEWGAME, NULL},
	{ MF_STRING, IDM_MANVSMAN, IDS_UNDO, NULL },
	{ MF_STRING, ID_HELP, IDS_UNDO, NULL },
    { MF_END, 0, NULL, NULL }
};
//#endif

static const MENUTEMPLATE GoBangMainMenuTemplate =
{
    0,
    GoBangMainMenu
};

static MENUITEMTEMPLATE GoBangGameMenu[] =
{
	{ MF_STRING, ID_NEW, IDS_NEWGAME, NULL},
	{ MF_STRING, ID_UNDO, IDS_UNDO, NULL },
//	{ MF_STRING, ID_SETTING, IDS_SETTING, NULL },
    { MF_END, 0, NULL, NULL }
};
//#endif

static const MENUTEMPLATE GoBangGameMenuTemplate =
{
    0,
    GoBangGameMenu
};

/*********************************************************************\
* Function	   GoBang_AppControl
* Purpose      五子棋入口点
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
//int DlmMain(unsigned long nCode, void* hInst, WPARAM wParam,  LPARAM lParam)
DWORD GoBang_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    WNDCLASS wc;
    DWORD dwRet;
	
	switch (nCode) 
    {
    case APP_INIT :

		hInstance = (HINSTANCE)pInstance;
		  
		
        break;
		
    case APP_ACTIVE :
		if (!bGobangSettingsClass)
		{
			wc.style         = 0;
			wc.lpfnWndProc   = SettingsWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground = GetStockObject(NULL_BRUSH);//BLACK_BRUSH);//WHITE_BRUSH) ;
			wc.lpszMenuName  = NULL;
			wc.lpszClassName  = "GobangSettingsClass";
			
			if (!RegisterClass(&wc))
				return FALSE;
			
			bGobangSettingsClass = TRUE;
		}

        if (hWndGobangSettings)
		{
			ShowWindow(hWndGobangSettings, SW_SHOW);
			ShowOwnedPopups(hWndGobangSettings, TRUE);
			UpdateWindow(hWndGobangSettings);
        }
        else
        {
#ifdef _MODULE_DEBUG_
			StartObjectDebug();
#endif
			hMainMenu = LoadMenuIndirect(&GoBangMainMenuTemplate);

			ModifyMenu(hMainMenu, IDM_MANVSCOM, MF_BYCOMMAND, IDM_MANVSCOM, IDS_MANVSCOM);
			ModifyMenu(hMainMenu, IDM_MANVSMAN, MF_BYCOMMAND, IDM_MANVSMAN, IDS_MANVSMAN);
			ModifyMenu(hMainMenu, ID_HELP, MF_BYCOMMAND, ID_HELP, IDS_HELP);
			
			hWndGobangSettings = CreateWindow(
				"GobangSettingsClass", 
				IDS_GOBANG, 
				WS_VISIBLE|PWS_STATICBAR|WS_CAPTION,//|WS_BORDER,
				PLX_WIN_POSITION,
				/*
				WND_X, 
				TOPSPACE, 
				WND_WIDTH, 
				WND_HEIGHT,
				*/
				NULL, 
				hMainMenu,
				NULL,
				NULL
				);
			
			if(!hWndGobangSettings)
			{
				return FALSE;
			}
			SendMessage(hWndGobangSettings, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			ShowWindow(hWndGobangSettings, SW_SHOW);
			UpdateWindow(hWndGobangSettings);
        }

        break;	   	
		
    case APP_INACTIVE :
		ShowOwnedPopups(hWndGobangSettings, FALSE);
		ShowWindow(hWndGobangSettings,SW_HIDE);
        break;
		
    case APP_GETOPTION :
		
		if (wParam == AS_STATE)
		{
			if(hHopenApp)
			{
				if (IsWindowVisible(hHopenApp))
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
			dwRet =(DWORD) hWndGobangSettings;
			break;
		
		break;

    default :
        break;
    }
    return TRUE;
}

/*********************************************************************\
* Function	   CreateGobangGame
* Purpose      创建游戏窗口
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL CreateGobangGame(HWND hWnd)
{
	WNDCLASS wc;
	
	if (!bSUPER5Class)
	{
		wc.style         = 0;
		wc.lpfnWndProc   = AppWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;//LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = GetStockObject (NULL_BRUSH);//BLACK_BRUSH);//WHITE_BRUSH) ;
		wc.lpszMenuName  = NULL;
		wc.lpszClassName  = "SUPER5Class";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bSUPER5Class = TRUE;
	}
	
	if (hHopenApp)
	{
		ShowWindow(hHopenApp, SW_SHOW);
		UpdateWindow(hHopenApp);
	}
	else
	{
		// load menus from the menu teamplate.
		//hMenu = LoadMenuIndirect((PMENUTEMPLATE)&AppMenuTemplate);
		hMenu = LoadMenuIndirect(&GoBangGameMenuTemplate);
		hHopenApp = CreateWindow(
			"SUPER5Class", 
			"Gobang", 
			WS_VISIBLE|PWS_STATICBAR,//|WS_BORDER,//WS_CAPTION|
			PLX_WIN_POSITION,
			/*
			WND_X, 
			TOPSPACE, 
			WND_WIDTH, 
			WND_HEIGHT,
			*/
			hWnd, 
			(HMENU)hMenu,
			NULL,
			NULL
			);
		
		if(!hHopenApp)
		{
			DestroyMenu(hMenu);
			return FALSE;
		}

		hMenu = GetMenu(hHopenApp);
		ModifyMenu(hMenu,ID_NEW,MF_BYCOMMAND,ID_NEW,IDS_NEWGAME);
		ModifyMenu(hMenu,ID_UNDO,MF_BYCOMMAND,ID_UNDO,IDS_UNDO);
	
		ShowWindow(hHopenApp, SW_SHOW);
		UpdateWindow(hHopenApp);
	}

	return TRUE;
}

/*********************************************************************\
* Function	   SettingsWndProc
* Purpose      设置界面窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK SettingsWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	HDC		hdc, hdcBP;
    RECT    rcClient;
    HBITMAP hBmp, hBmpOld;

	lResult = TRUE;    

    switch (wMsgCmd)
    {
	case WM_CREATE:

#ifdef SCP_SMARTPHONE
		SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
#endif
		{
			COLORREF Color;
			BOOL	bTran;
			hdc = GetDC(hWnd);
			hCoverBg = CreateBitmapFromImageFile(hdc, COVERBG, &Color,&bTran);
            /*
#ifndef SCP_NOKEYBOARD
            hText = CreateBitmapFromImageFile(hdc, 
                GetCurrentLanguage() ? TEXTCH : TEXTEN, &Color, &bTran);
#endif
                */
            hBmpSel = CreateBitmapFromImageFile(hdc,PATH_GOBANGVICTORY,&Color,&bTran);
		//	hBmpMenu = CreateBitmapFromImageFile(hdc,PATH_MENU,&Color,&bTran);
			ReleaseDC(hWnd,hdc);
		}
//#endif
		SetMenuItemRectLocal();
//		nSelMenuItem = 0;
//		ShowArrowState(hWnd,nSelMenuItem);

		CreateCapButton(hWnd, IDCANCEL, 0, IDS_EXIT);
		CreateCapButton(hWnd, IDOK, 1, ML("New"));
		
		
		break;

	case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
			case IDM_MANVSCOM:
			case IDOK:
				robot_side = black;
				PostMessage(hWnd,WM_COMMAND,ID_NEW,NULL);
				break;
				
			case IDM_MANVSMAN:
				robot_side = empty;
				PostMessage(hWnd,WM_COMMAND,ID_NEW,NULL);
				break;
				
#ifdef _MXLINCOMMENT_

				switch(nSelMenuItem)
				{
				case 0:
					robot_side = black;
					PostMessage(hWnd,WM_COMMAND,ID_NEW,NULL);
					break;
					
				case 1:
					robot_side = empty;
					PostMessage(hWnd,WM_COMMAND,ID_NEW,NULL);
					break;
					
				case 2:
					PostMessage(hWnd,WM_COMMAND,ID_HELP,NULL);
					break;
					
				default:
					nSelMenuItem = 0;
					robot_side = black;
					PostMessage(hWnd,WM_COMMAND,ID_NEW,NULL);
					break;
				}
				break;
#endif
				
				
				case ID_NEW:
					CreateGobangGame(hWnd);
					//PostMessage(hHopenApp, WM_COMMAND, ID_NEW, 0);
					break;
					
				case ID_HELP:
					OnGameHelp(hWnd);
					break;
					
				case IDCANCEL:
					DestroyWindow(hWnd);
					break;
			}
		}
		
		break;

	case WM_PENDOWN:
		{
			int i;
			int x,y;

			x = LOWORD(lParam);
			y = HIWORD(lParam);
			i = GetAreaByPos(x,y);
			if (-1 != i)
			{
				switch(i)
				{
				case 0:
					robot_side = black;
					PostMessage(hWnd,WM_COMMAND,ID_NEW,NULL);
					break;

				case 1:
					robot_side = empty;
					PostMessage(hWnd,WM_COMMAND,ID_NEW,NULL);
					break;

				case 2:
					PostMessage(hWnd,WM_COMMAND,ID_HELP,NULL);
					break;

				default:
					i = 0;
					robot_side = black;
					PostMessage(hWnd,WM_COMMAND,ID_NEW,NULL);
					break;
				}
				nSelMenuItem = i;
				ShowArrowState(hWnd,nSelMenuItem);
				InvalidateRect(hWnd,NULL,TRUE);
			}
		}
		break;

	case WM_PAINT:
		hdcBP = BeginPaint(hWnd,NULL);
        GetClientRect(hWnd, &rcClient);
        hdc = CreateCompatibleDC(hdcBP);
        hBmp = CreateCompatibleBitmap(hdcBP, rcClient.right,
            rcClient.bottom);
        hBmpOld = (HBITMAP)SelectObject(hdc, (HGDIOBJ)hBmp);
		BitBlt(hdc, X_COVERBG,Y_COVERBG,
				COVERBG_WIDTH , COVERBG_HEIGHT , 
			   (HDC)hCoverBg, 0, 0, ROP_SRC);
		
        {
            COLORREF	OldClr;
            int			OldStyle;
            
            if ((0>nSelMenuItem)||(MENU_ITEM_NUM<=nSelMenuItem))
            {
                break;
            }
            
            OldClr = SetBkColor(hdc,RGB(0,0,255));
            OldStyle = SetBkMode(hdc, NEWTRANSPARENT);
           /*@**#---2005-04-27 15:26:17 (mxlin)---#**@
 
            BitBlt(
                hdc,
                X_ALLMENU,
                Y_ALLMENU,
                ALLMENU_WIDTH,
                ALLMENU_HEIGHT,
                (HDC)hBmpMenu,
                0,0,
                ROP_SRC);
            */
#ifdef SCP_NOKEYBOARD
            SetBkColor(hdc, OldClr);
            OldClr = SetBkColor(hdc, COLOR_WHITE);
#endif
			/*
            BitBlt(
                hdc,
                SelItemRect[nSelMenuItem].left,
                SelItemRect[nSelMenuItem].top,
                SelItemRect[nSelMenuItem].right - SelItemRect[nSelMenuItem].left,
                SelItemRect[nSelMenuItem].bottom - SelItemRect[nSelMenuItem].top,
                (HDC)hBmpSel,
                0,0,
                ROP_SRC);
				*/
            SetBkColor(hdc, OldClr);
            SetBkMode(hdc, OldStyle);
            /*
            for (i=0;i<MENU_ITEM_NUM;i++)
            {
            MyDrawMenuItem(hdc,i,nSelMenuItem);
            }
            */
        }

		BitBlt(hdcBP, 0, 0, rcClient.right, rcClient.bottom,
			   (HDC)hdc, 0, 0, ROP_SRC);
        SelectObject(hdc, hBmpOld);
        DeleteObject(hBmp);
        DeleteDC(hdc);

        EndPaint(hWnd,NULL);
        break;
		
#ifdef _MXLINCOMMENT_
		
	case WM_KEYDOWN:
		{
			switch(LOWORD(wParam))
			{
				
			case VK_UP:
				
				if(nSelMenuItem <= 0)
					nSelMenuItem = MENU_ITEM_NUM;
				
				//				if (0<nSelMenuItem)
				{
					nSelMenuItem--;
					ShowArrowState(hWnd,nSelMenuItem);
					InvalidateRect(hWnd,NULL,TRUE);
				}
				break;
				
			case VK_DOWN:
				
				if(nSelMenuItem >= MENU_ITEM_NUM-1)
					nSelMenuItem = -1;
				
				//				if (MENU_ITEM_NUM-1>nSelMenuItem)
				{
					nSelMenuItem++;
					ShowArrowState(hWnd,nSelMenuItem);
					InvalidateRect(hWnd,NULL,TRUE);
				}
				break;
				
				//			case VK_F5:
				//				PostMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDOK,1),NULL);
				//				break;
				
			default:
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
		}
		break;
#endif
	case WM_ACTIVATE:
		if (WA_ACTIVE == wParam)
		{
			SetFocus(hWnd);
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		UnregisterClass("GobangSettingsClass",NULL);
		break;
		
	case WM_DESTROY:

//#if defined SCP_NOKEYBOARD
		if (hCoverBg)
		{
			DeleteObject(hCoverBg);
			hCoverBg = NULL;
		}

		if (hBmpSel)
		{
			DeleteObject(hBmpSel);
			hBmpSel = NULL;
		}
/*@**#---2005-04-27 15:26:27 (mxlin)---#**@

		if (hBmpMenu)
		{
			DeleteObject(hBmpMenu);
			hBmpMenu = NULL;
		}*/
//#endif

		hWndGobangSettings = NULL;
		

		DlmNotify((WPARAM)PES_APP_QUIT, (LPARAM)hInstance);

#ifdef _MODULE_DEBUG_
		EndObjectDebug();
#endif
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    
    return lResult;

}

/*********************************************************************\
* Function	   OnGameHelp
* Purpose      启动帮助窗口
* Params	   
* Return	 	   
* Remarks	   注册类并创建窗口
**********************************************************************/
static DWORD	OnGameHelp(HWND hWnd)
{
    DWORD dwRet;

    dwRet = TRUE;

	CallGameHelpWnd(hWnd, IDS_HELPTEXT);

	return dwRet;
}

/*********************************************************************\
* Function	   MyDrawMenuItem
* Purpose      画菜单项
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL MyDrawMenuItem(HDC hdc,int i,int nSel)
{
	COLORREF	OldClr;
	int			OldStyle;

	if ((0>i)||(MENU_ITEM_NUM<=i))
	{
		return FALSE;
	}
	if ((0>nSel)||(MENU_ITEM_NUM<=nSel))
	{
		return FALSE;
	}

	OldClr = SetBkColor(hdc,RGB(0,0,255));
	OldStyle = SetBkMode(hdc, NEWTRANSPARENT);

	/*@**#---2005-04-27 15:26:32 (mxlin)---#**@
BitBlt(
		hdc,
		X_ALLMENU,
		Y_ALLMENU,
		ALLMENU_WIDTH,
		ALLMENU_HEIGHT,
		(HDC)hBmpMenu,
		0,0,
		ROP_SRC);*/

#ifdef SCP_NOKEYBOARD
    SetBkColor(hdc, OldClr);
	OldClr = SetBkColor(hdc, COLOR_WHITE);
#endif
//	BitBlt(
//		hdc,
//		SelItemRect[nSel].left,
//		SelItemRect[nSel].top,
//		SelItemRect[nSel].right - SelItemRect[nSel].left,
//		SelItemRect[nSel].bottom - SelItemRect[nSel].top,
//		(HDC)hBmpSel,
//		0,0,
//		ROP_SRC);
    SetBkColor(hdc, OldClr);
	SetBkMode(hdc, OldStyle);

	return TRUE;
}

/*********************************************************************\
* Function	   GetAreaByPos
* Purpose      得点中区域
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	int	GetAreaByPos(int x,int y)
{
	int i;
	for (i=0;i<MENU_ITEM_NUM;i++)
	{
		if ((MenuItemRect[i].top<y)
			&&(MenuItemRect[i].bottom>y)
			&&(MenuItemRect[i].left<x)
			&&(MenuItemRect[i].right>x))
			return i;
	}
	return -1;
}

/*********************************************************************\
* Function	   ShowArrowState
* Purpose      主界面箭头显示
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	BOOL	ShowArrowState(HWND hWnd,int nSel)
{
	SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLUP|SCROLLDOWN, MASKUP|MASKDOWN);

/*	if (MENU_ITEM_NUM-1 == nSel)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKDOWN);
	if (0 == nSel)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKUP);*/

	SendMessage(hWnd,PWM_SETSCROLLSTATE,SCROLLMIDDLE,MASKMIDDLE);

	return TRUE;
}

/*********************************************************************\
* Function	   SetMenuItemRectLocal
* Purpose      设定菜单项区域
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	SetMenuItemRectLocal()
{
	int i;
	int x1,y1,x2,y2;

	for (i=0;i<MENU_ITEM_NUM;i++)
	{
		x1 = X_LT_MENUITEM;
		y1 = Y_LT_MENUITEM + i * Y_MENUITEM_INTERVAL;
		x2 = x1 + MENUITEM_WIDTH;
		y2 = y1 + MENUITEM_HEIGHT;
		SetRect(&(MenuItemRect[i]),x1,y1,x2,y2);
		x1 = X_LT_SELITEM;
		y1 = Y_LT_SELITEM + i * Y_MENUITEM_INTERVAL;
		x2 = x1 + SELITEM_WIDTH;
		y2 = y1 + SELITEM_HEIGHT;
		SetRect(&(SelItemRect[i]),x1,y1,x2,y2);
	}

	return TRUE;
}

static void DrawMsg(HDC hdc)
{
	int c;
	
	c = Game5_Won();
	if(empty != c)
	{
		DrawWonFigure( hdc , c );
	}
	else
	{
		c = Game5_Side();
		
		if (empty != c)
		{
			DrawToGoSide( hdc , c );
		}
	}
	
	return ;
}

/*********************************************************************\
* Function	   UpdateStatus
* Purpose      刷新状态
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void UpdateStatus(HWND hWnd)
{
	int c;
	HDC hdc;

	hdc = GetDC(hWnd);
	c = Game5_Won();
	if(c!=empty)
	{
		sprintf(msg, MSG_WON, c==black? MSG_BLACK : MSG_WHITE);
		//if(PLXConfirmWin(msg, NULL, IDS_GOBANG, ML("OK"), ML("NO")))
//		MsgWin(msg,IDS_PROMPT,WAITTIMEOUT);
		PLXTipsWin(NULL, hWnd, 0, msg,IDS_GOBANG,Notify_Info,ML("Ok"),NULL,WAITTIMEOUT);
		//DrawWonFigure(hdc,c);
		game_stopped = 1;
	}
	else 
	{
		c = Game5_Side();
		DrawToGoSide(hdc , c);
		//sprintf(msg, MSG_SIDE, c==black?MSG_BLACK:MSG_WHITE);
		//if(c == robot_side)
		//	strcat(msg, MSG_ROBOT);
		game_stopped = 0;
	}
	ReleaseDC(hWnd,hdc);
	flicker = 1;
}

/*********************************************************************\
* Function	   AppWndProc
* Purpose      五子棋窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT message, WPARAM wParam, 
                                   LPARAM lParam)
{
    LONG			lResult;
	HDC				hdc, hdcBP;
    RECT            rcClient;
    HBITMAP         hBmp, hBmpOld;
	int				x,y, c, r, color;
	
    lResult = (LRESULT)TRUE;
    
    switch (message)
    {
	case WM_CREATE:
        bUndoPermit = FALSE;
//#if defined SCP_NOKEYBOARD
		{
			HDC hdc;
			COLORREF Color;
			BOOL	bTran;
			
			hdc = GetDC(hWnd);
			hBmpToGoCol = CreateBitmapFromImageFile(hdc,PATH_GOBANGMAN,&Color,&bTran);
			ReleaseDC(hWnd,hdc);
		}
//#endif
		PaintInit(hWnd, 6, 11);
		CreateCapButton(hWnd, ID_QUIT, 0, IDS_BACK);
		CreateCapButton(hWnd, ID_SURE, 1, IDS_SURE);
		SendMessage(hWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
		SetTimer(hWnd, 1, 500, 0);
		game_stopped = 1;
		last_x = last_y = -1;
		//robot_side = empty;
		Game5_RobotBreak();
		Game5_New();
		
		curcoordinate.x=0;
		curcoordinate.y=0;
		
		
		game_stopped = 0;
		last_x = last_y = -1;
		if(robot_side == Game5_Side())
			PostMessage(hWnd, WM_ROBOTPLAY, 0, 0);
		UpdateStatus(hWnd);
		InvalidateRect(hWnd, NULL, TRUE);
		
		//if(robot_side == Game5_Side()) PostMessage(hWnd, WM_ROBOTPLAY, 0, 0);
		//UpdateStatus(hWnd);
		break;

    case WM_INITMENU:
		if (!bUndoPermit)
		{
			if (GetMenuState(hMenu, ID_UNDO, MF_BYCOMMAND) != -1) {
				RemoveMenu(hMenu,ID_UNDO,MF_BYCOMMAND);
			}
			
		}
		else
		{
			if (GetMenuState(hMenu, ID_UNDO, MF_BYCOMMAND) == -1) {
				AppendMenu(hMenu, MF_STRING, ID_UNDO, IDS_UNDO);
			}
		}
        break;

	case WM_ACTIVATE:
		if (WA_ACTIVE == wParam)
		{
			SetFocus(hWnd);
			//InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_NEW:
			Game5_RobotBreak();
			Game5_New();
			curcoordinate.x=0;
			curcoordinate.y=0;
            		bUndoPermit = FALSE;
			game_stopped = 0;
			last_x = last_y = -1;
			if(robot_side == Game5_Side())
                PostMessage(hWnd, WM_ROBOTPLAY, 0, 0);
			UpdateStatus(hWnd);
			InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_UNDO:
			if (bUndoPermit)
			{
                int step;
				Game5_RobotBreak();
				last_x = last_y = -1;
				step = Game5_Undo(robot_side == empty ? 1 : (robot_side == Game5_Side()?1 : 2));
				if(robot_side == Game5_Side())
                    PostMessage(hWnd, WM_ROBOTPLAY, 0, 0);
				if((nSelMenuItem == 0 && step == 1) || step == 0)
                    bUndoPermit = FALSE;
				UpdateStatus(hWnd);
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;

		case ID_QUIT:
			if (!game_stopped)
			{
				PLXConfirmWinEx(NULL, hWnd, IDS_GAME_PROMPT, Notify_Request, IDS_GOBANG, IDS_CONFIRM, ML("Cancel"), IDRM_RMSG);
							
			}
			else
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;

		}
		break;

	case IDRM_RMSG:
		if (lParam)
		{
			game_stopped = 1;
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

	case WM_SELCANCEL:
		break;

	case WM_SELOK:
		game_stopped = 1;
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case WM_PENDOWN:
		x = LOWORD(lParam);
        y = HIWORD(lParam);

		break;

	case WM_PENUP:
		if(game_stopped || robot_side == Game5_Side())
			break;
		x = LOWORD(lParam);
        y = HIWORD(lParam);
		if(Board_HitTest(x, y, &c, &r))
		{
			color = Game5_Put(c, r);
			if(color!=empty)
			{
				hdc = GetDC(hWnd);
				if(last_x>=0 && last_y>=0) PaintChess(hdc, last_x, last_y, last_color);
				last_x = c;
				last_y = r;
				last_color = color;
				PaintChess(hdc, c, r, color);
				UpdateStatus(hWnd);
				DrawMsg(hdc);
				ReleaseDC(hWnd, hdc);
                bUndoPermit = TRUE;
			}
			if(robot_side == Game5_Side()) PostMessage(hWnd, WM_ROBOTPLAY, 0, 0);
		}
		break;

	case WM_KEYUP:
		{
			int xtmpidentify=0;
			int ytmpidentify=0;
			switch(LOWORD(wParam))
			{
			case VK_LEFT:
				xtmpidentify= curcoordinate.x;
				ytmpidentify= curcoordinate.y;
				hdc = GetDC(hWnd);
				do{
					curcoordinate.x--;
					if(curcoordinate.x<0)
					{
						curcoordinate.x = BOARD_SIZE-1;
					}
					if(curcoordinate.x==xtmpidentify)
					{
						curcoordinate.y++;
						if(curcoordinate.y>BOARD_SIZE-1)
							curcoordinate.y=0;
						if(curcoordinate.y == ytmpidentify)
						{
							//todo:棋盘满后如何处理(自动结束)
							PLXTipsWin(NULL, hWnd,0, IDS_DRAW,IDS_GOBANG,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
							break;
						}
						
					}
				}while(BB_Test(game5.bb[2], curcoordinate.x, curcoordinate.y));
				PaintChess(hdc, curcoordinate.x, curcoordinate.y, Game5_Side());
				if(lastcoordinatex!=last_x||lastcoordinatey!=last_y)
				{
					UnPaintChess(hdc, lastcoordinatex, lastcoordinatey);
				}
				//PaintChess(hdc, curcoordinate.x, curcoordinate.y, Game5_Side());
				ReleaseDC(hWnd, hdc);
				lastcoordinatex = curcoordinate.x;
				lastcoordinatey = curcoordinate.y;
				break;
			case VK_RIGHT:
				xtmpidentify= curcoordinate.x;
				ytmpidentify= curcoordinate.y;
				hdc = GetDC(hWnd);
				do{
					curcoordinate.x++;
					if(curcoordinate.x>BOARD_SIZE-1)
					{
						curcoordinate.x = 0;
					}
					if(curcoordinate.x==xtmpidentify)
					{
						curcoordinate.y++;
						if(curcoordinate.y>BOARD_SIZE-1)
							curcoordinate.y=0;
						if(curcoordinate.y == ytmpidentify)
						{
							//todo:棋盘满后如何处理(自动结束)
							PLXTipsWin(NULL, hWnd, 0, IDS_DRAW,IDS_GOBANG,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
							break;
						}
						
					}
				}while(BB_Test(game5.bb[2], curcoordinate.x, curcoordinate.y));
				PaintChess(hdc, curcoordinate.x, curcoordinate.y, Game5_Side());
				if(lastcoordinatex!=last_x||lastcoordinatey!=last_y)
				{
					UnPaintChess(hdc, lastcoordinatex, lastcoordinatey);
				}
				ReleaseDC(hWnd, hdc);
				lastcoordinatex = curcoordinate.x;
				lastcoordinatey = curcoordinate.y;
				//InvalidateRect(hWnd, NULL, TRUE);
				break;
			case VK_UP:
				xtmpidentify= curcoordinate.x;
				ytmpidentify= curcoordinate.y;
				hdc = GetDC(hWnd);
				do{
					curcoordinate.y--;
					if(curcoordinate.y<0)
					{
						curcoordinate.y = BOARD_SIZE-1;
					}
					if(curcoordinate.y==ytmpidentify)
					{
						curcoordinate.x++;
						if(curcoordinate.x>BOARD_SIZE-1)
							curcoordinate.x=0;
						if(curcoordinate.x == xtmpidentify)
						{
							//todo:棋盘满后如何处理(自动结束)
							PLXTipsWin(NULL, hWnd, 0, IDS_DRAW,IDS_GOBANG,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
							break;
						}
						
					}
				}while(BB_Test(game5.bb[2], curcoordinate.x, curcoordinate.y));
				PaintChess(hdc, curcoordinate.x, curcoordinate.y, Game5_Side());
				if(lastcoordinatex!=last_x||lastcoordinatey!=last_y)
				{
					UnPaintChess(hdc, lastcoordinatex, lastcoordinatey);
				}
				lastcoordinatex = curcoordinate.x;
				lastcoordinatey = curcoordinate.y;
				ReleaseDC(hWnd, hdc);
				//InvalidateRect(hWnd, NULL, TRUE);
				break;
			case VK_DOWN:
				xtmpidentify= curcoordinate.x;
				ytmpidentify= curcoordinate.y;
				hdc = GetDC(hWnd);
				do{
					curcoordinate.y++;
					if(curcoordinate.y>BOARD_SIZE-1)
					{
						curcoordinate.y = 0;
					}
					if(curcoordinate.y==ytmpidentify)
					{
						curcoordinate.x++;
						if(curcoordinate.x>BOARD_SIZE-1)
							curcoordinate.x=0;
						if(curcoordinate.x == xtmpidentify)
						{
							//todo:棋盘满后如何处理(自动结束)
							PLXTipsWin(NULL, hWnd, 0, IDS_DRAW,IDS_GOBANG,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
							break;
						}
						
					}
				}while(BB_Test(game5.bb[2], curcoordinate.x, curcoordinate.y));
				PaintChess(hdc, curcoordinate.x, curcoordinate.y, Game5_Side());
				if(lastcoordinatex!=last_x||lastcoordinatey!=last_y)
				{
					UnPaintChess(hdc, lastcoordinatex, lastcoordinatey);
				}
				lastcoordinatex = curcoordinate.x;
				lastcoordinatey = curcoordinate.y;
				ReleaseDC(hWnd, hdc);
				//InvalidateRect(hWnd, NULL, TRUE);
				break;
			case VK_RETURN:
				if(game_stopped || robot_side == Game5_Side())
					break;
				color = Game5_Put(curcoordinate.x, curcoordinate.y);
				if(color!=empty)
				{
					hdc = GetDC(hWnd);
					if(last_x>=0 && last_y>=0) PaintChess(hdc, last_x, last_y, last_color);
					last_x = curcoordinate.x;
					last_y = curcoordinate.y;
					last_color = color;
					PaintChess(hdc, curcoordinate.x, curcoordinate.y, color);
					UpdateStatus(hWnd);
					DrawMsg(hdc);
					ReleaseDC(hWnd, hdc);
					bUndoPermit = TRUE;
					/*
					curcoordinate.x = 0;
					curcoordinate.y = 0;
					*/
					do{
						curcoordinate.x++;
						if(curcoordinate.x>BOARD_SIZE-1)
							curcoordinate.x=0;
						if(curcoordinate.x == last_x)
						{
							curcoordinate.y++;
							if(curcoordinate.y>BOARD_SIZE-1)
								curcoordinate.y=0;
							if(curcoordinate.y==last_y)
							{
								//todo:棋盘满后如何处理(自动结束)
								PLXTipsWin(NULL, hWnd, 0, IDS_DRAW,IDS_GOBANG,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
								break;
							}
						}
					}while(BB_Test(game5.bb[2], curcoordinate.x, curcoordinate.y));
					lastcoordinatex = -1;
					lastcoordinatey = -1;
					InvalidateRect(hWnd, NULL, TRUE);
				}
				if(robot_side == Game5_Side()) PostMessage(hWnd, WM_ROBOTPLAY, 0, 0);
				break;
			default:
				PDADefWindowProc(hWnd, message, wParam, lParam);
				break;
			}
		}
		break;

	case WM_ROBOTPLAY:
		if(!game_stopped && Game5_RobotPlay(&c, &r))
		{
			color = Game5_Put(c, r);
			if(color!=empty)
			{
				hdc = GetDC(hWnd);
				if(last_x>=0 && last_y>=0) PaintChess(hdc, last_x, last_y, last_color);
				last_x = c;
				last_y = r;
				last_color = color;
				PaintChess(hdc, c, r, color);
				UpdateStatus(hWnd);
				DrawMsg(hdc);
				ReleaseDC(hWnd, hdc);
                //bUndoPermit = TRUE;
			}
		}
		break;
	case WM_TIMER:
		hdc = GetDC(hWnd);
		if(flicker % 5)
		{
			DrawMsg(hdc);
		}
		else
		{
			//FillRect(hdc, &rcMsg, GetStockObject(WHITE_BRUSH));
		}
		if(flicker % 3)
			PaintChess(hdc, last_x, last_y, last_color);
		else
			UnPaintChess(hdc, last_x, last_y);

		flicker ++;
		ReleaseDC(hWnd, hdc);
		break;

	case WM_PAINT : 
		hdcBP = BeginPaint(hWnd, NULL);
        GetClientRect(hWnd, &rcClient);
        hdc = CreateCompatibleDC(hdcBP);
        hBmp = CreateCompatibleBitmap(hdcBP, rcClient.right,
            rcClient.bottom);
        hBmpOld = (HBITMAP)SelectObject(hdc, (HGDIOBJ)hBmp);

        PaintBoard(hdc);
		DrawMsg(hdc);

        BitBlt(hdcBP, 0, 0, rcClient.right, rcClient.bottom,
			   (HDC)hdc, 0, 0, ROP_SRC);
        SelectObject(hdc, hBmpOld);
        DeleteObject(hBmp);
        DeleteDC(hdc);
		EndPaint(hWnd, NULL);
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:
		
		if (hBmpToGoCol)
		{
			DeleteObject(hBmpToGoCol);
			hBmpToGoCol = NULL;
		}
		
		Game5_RobotBreak();
		KillTimer(hWnd, 1);
		PaintDestroy();
		DestroyMenu(hMenu);
		//DestroyWindow(hWnd);
		
		//如果调用LocalCreateHeap来创建局部堆,则需要调用LocalDestroyHeap来释放局部堆
		//LocalDestroyHeap();
		hHopenApp = NULL;
		//UnregisterClass("SUPER5Class", NULL); 

		break;
		
	default :
		lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
	} 
	return lResult;
}

/*********************************************************************\
* Function	   DrawToGoSide
* Purpose      画要走棋的一方的提示标志
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawToGoSide(HDC hdc,int color)
{
	COLORREF	OldColor;
	int			OldStyle;
	int			xofbmp;
	
	if (black == color)
	{
		xofbmp = 12;
	}
	else
	{
		xofbmp = 0;
	}

	OldColor = SetBkColor(hdc, RGB(0,0,255));
	OldStyle = SetBkMode(hdc, NEWTRANSPARENT);

	BitBlt(hdc,
		X_TOGO,
		Y_TOGO,
		TOGO_WIDTH,
		TOGO_HEIGHT,
		(HDC)hBmpToGoCol,
		xofbmp,
		0,
		ROP_SRC);

	SetBkColor(hdc, OldColor);
	SetBkMode(hdc, OldStyle);

	return ;
}

/*********************************************************************\
* Function	   DrawWonFigure
* Purpose      画胜利一方和标志
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DrawWonFigure(HDC hdc, int color)
{
	COLORREF	OldColor;
	int			OldStyle;
	
	DrawToGoSide(hdc ,color );

#ifndef SCP_NOKEYBOARD
    OldColor = SetBkColor(hdc, RGB(0,0,255));
#else
    OldColor = SetBkColor(hdc, RGB(255,255,255));
#endif
	OldStyle = SetBkMode(hdc, NEWTRANSPARENT);

	BitBlt(hdc,
		X_WON,
		Y_WON,
		SELITEM_WIDTH,
		SELITEM_HEIGHT,
		(HDC)hBmpSel,
		0,
		0,
		ROP_SRC);

	SetBkColor(hdc, OldColor);
	SetBkMode(hdc, OldStyle);

	return ;
}

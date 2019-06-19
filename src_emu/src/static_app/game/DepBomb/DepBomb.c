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
#define __MODULE__ "DEPBOMB"
#include "DepBomb.h"
#include "imesys.h"
//#include "MSGWIN.h"



static BOOL	bBombMainWndClass = FALSE;
static BOOL	bBombSetupWndClass = FALSE;
static BOOL	bBombGameWndClass = FALSE;
static BOOL	bBombHeroWndClass = FALSE;
static BOOL	bBombSetHeroWndClass = FALSE;

/*****************************************************************************\
* Funcation: DepthBomb_AppControl
*
* Purpose  : 应用程序入口
*
* Explain  : 
*                      
\*****************************************************************************/
DWORD DepthBomb_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
//------------------------------------------------------------------------
    WNDCLASS	wc;
    DWORD		dwRet;
//------------------------------------------------------------------------

    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:

		hBombInstance = (HINSTANCE)pInstance;
		
		
        
        break;

   case APP_GETOPTION:
		
		switch( wParam )
		{
		
		case AS_ICON:

			dwRet = (DWORD)AppIconData;
			
			break;

		case AS_APPWND:
        	//返回创建窗口句柄，没有值返回空。
			dwRet = (DWORD)hWnd_BombMain;
			break;
		}

		break;

    case APP_ACTIVE :
		if (!bBombMainWndClass)
		{
			// 注册深水炸弹主窗口类
			wc.style         = 0;
			wc.lpfnWndProc   = BombMainWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName  = NULL;
			wc.lpszClassName  = "BombMainWndClass";
			
			if (!RegisterClass(&wc))
				return FALSE;
			
			bBombMainWndClass = TRUE;
		}

		if (IsWindow(hWnd_BombMain))
		{
	   		ShowWindow(hWnd_BombMain, SW_SHOW);
			ShowOwnedPopups(hWnd_BombMain, TRUE);
			UpdateWindow(hWnd_BombMain);
		}
        
        else
        {
#ifdef _MODULE_DEBUG_
			StartObjectDebug();
#endif
			hMainMenu	=	LoadMenuIndirect(&BombMainMenuTemplate);

			ModifyMenu(hMainMenu, IDM_NEWGAME, MF_BYCOMMAND, IDM_NEWGAME, IDS_NEWGAME);
			ModifyMenu(hMainMenu, IDM_SETUP, MF_BYCOMMAND, IDM_SETUP, IDS_SETUP);
			ModifyMenu(hMainMenu, IDM_HEROBOARD,MF_BYCOMMAND,  IDM_HEROBOARD, IDS_SCORES);
			ModifyMenu(hMainMenu, IDM_HELP, MF_BYCOMMAND, IDM_HELP, IDS_HELP);
			
			hWnd_BombMain = CreateWindow(
                "BombMainWndClass", 
                IDS_DEPTHBOMB, 
				WS_VISIBLE|PWS_STATICBAR|WS_CAPTION,
				PLX_WIN_POSITION,
/*
                WND_X, 
                WND_Y+TOPSPACE, 
                WND_WIDTH, 
                WND_HEIGHT,
*/				
                NULL, 
                hMainMenu,
                NULL, 
                NULL
                );
			if (!hWnd_BombMain)
				return (FALSE);

			SendMessage(hWnd_BombMain, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			ShowWindow(hWnd_BombMain, SW_SHOW);
			UpdateWindow(hWnd_BombMain);
		}

		return dwRet;

    case APP_INACTIVE :
		ShowOwnedPopups(hWnd_BombMain, FALSE);
		ShowWindow(hWnd_BombMain, SW_HIDE);
		
        break;
    }

    return dwRet;
}

/*****************************************************************************\
* Funcation: BombMainWndProc
*
* Purpose  : 主窗口窗口过程处理函数
*
* Explain  : 主窗口显示菜单        
*           
\*****************************************************************************/
static LRESULT CALLBACK BombMainWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam)
{
//------------------------------------------------------------------------
	LRESULT			lResult;
	HDC				hdc;
//--------------------------------------------------------------------------

	switch (wMsgCmd)	
    {
	case WM_CREATE:
#if defined(SCP_SMARTPHONE)
		SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
#endif
		{
			HDC hdc;
			COLORREF	Color;
			BOOL	bTran;
			
			hdc = GetDC(hWnd_BombMain);
			hCoverBg = CreateBitmapFromImageFile(hdc,COVERBG, &Color,&bTran);
//			hMenuBg	 = CreateBitmapFromImageFile(hdc,IDS_MENUFILENAME, &Color,&bTran);
			//hBmpCovSel = CreateBitmapFromImageFile(hdc,BOMB_COVER_SELECT, &Color,&bTran);
			ReleaseDC(hWnd_BombMain,hdc);
		}

		nSelMenuItem = 0;
		ShowArrowState(hWnd,nSelMenuItem);
		/*
		strcpy(MenuItemText[0],IDS_NEWGAME);
		strcpy(MenuItemText[1],IDS_SETUP);
		strcpy(MenuItemText[2],IDS_HEROBOARD);
		strcpy(MenuItemText[3],IDS_HELP);
		*/
		SetMenuItemRectLocal();

		CreateCapButton(hWnd, IDM_EXIT, 0, IDS_EXIT);
		CreateCapButton(hWnd, IDM_CONFIRM, 1, IDS_NEW);

		if (!ReadBombHero())
		{
			InitBombHero();
		}
		if (!ReadBombSetup())
		{
			nBombNum = BASIC_BOMB_NUM;
			bPrizePermit = TRUE;
		}
		if (!(BASIC_BOMB_NUM <= nBombNum && DEPTHBOMB_NUM >= nBombNum))
		{
			nBombNum = BASIC_BOMB_NUM;
		}

		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDM_CONFIRM:
			BombNewGameProc(hWnd);
#if 0
			switch(nSelMenuItem)
			{
			case 0:
				PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
				break;

			case 1:
				PostMessage(hWnd,WM_COMMAND,IDM_SETUP,NULL);
				break;

			case 2:
				PostMessage(hWnd,WM_COMMAND,IDM_HEROBOARD,NULL);
				break;

			case 3:
				PostMessage(hWnd,WM_COMMAND,IDM_HELP,NULL);
				break;

			default:
				nSelMenuItem = 0;
				PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
				break;
			}
#endif
			break;

		case	IDM_NEWGAME:
			BombNewGameProc(hWnd);
			break;
		case	IDM_SETUP:
			BombMainSetup(hWnd);
			break;
		case	IDM_HEROBOARD:
			BombHeroProc(hWnd);
			break;
		case IDM_HELP:
			OnGameHelp(hWnd);
			break;
		case	IDM_EXIT:
			DestroyWindow(hWnd);
			break;
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
					PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
					break;

				case 1:
					PostMessage(hWnd,WM_COMMAND,IDM_SETUP,NULL);
					break;

				case 2:
					PostMessage(hWnd,WM_COMMAND,IDM_HEROBOARD,NULL);
					break;

				case 3:
					PostMessage(hWnd,WM_COMMAND,IDM_HELP,NULL);
					break;

				default:
					i = 0;
					PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
					break;
				}
				nSelMenuItem = i;
				//ShowArrowState(hWnd,nSelMenuItem);
				InvalidateRect(hWnd,NULL,TRUE);
			}
		}
		break;

	case WM_PAINT:
		{
			COLORREF		OldColor;
			int				OldStyle;				
			hdc = BeginPaint(hWnd,NULL);
						
			/* 设置背景色和显示模式 */
			OldColor = SetBkColor(hdc,RGB(255,255,255));
			OldStyle = SetBkMode(hdc,NEWTRANSPARENT);
			BitBlt(hdc, X_COVERBG,Y_COVERBG,
				COVERBG_WIDTH , COVERBG_HEIGHT , 
				(HDC)hCoverBg, 0, 0, ROP_SRC);
			/*
			BitBlt(hdc, X_MENUITEM, Y_MENUITEM,
				COVERBG_WIDTH , COVERBG_HEIGHT  , 
				(HDC)hMenuBg, 0, 0, ROP_SRC);
			*/
			
//			{
//				int i;
//				for (i=0;i<MENU_ITEM_NUM;i++)
//				{
//					MyDrawMenuItem(hdc,i,nSelMenuItem);
//				}
//			}
			
			SetBkColor(hdc,OldColor);
			SetBkMode(hdc,OldStyle);
			EndPaint(hWnd,NULL);
			break;
		}

#ifdef _MXLINCOMMENT_
	
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
//#ifndef SCP_SMARTPHONE
//		case VK_F2:
//			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
//            break;
//#endif
		case VK_UP:
			/*if (0<nSelMenuItem)*/
			{
				COLORREF	OldClr;
				int OldStyle;
				HDC	hdc;

				hdc = GetDC (hWnd);

				OldClr = SetBkColor(hdc,SEL_COLOR);
				OldStyle = SetBkMode(hdc, NEWTRANSPARENT);

				BitBlt(hdc,
					SelItemRect[nSelMenuItem].left,
					SelItemRect[nSelMenuItem].top,
					SELITEM_WIDTH,
					SELITEM_HEIGHT,
					(HDC)hCoverBg,
					SelItemRect[nSelMenuItem].left,
					SelItemRect[nSelMenuItem].top,ROP_SRC);

				if ( nSelMenuItem == 0)
					nSelMenuItem = MENU_ITEM_NUM - 1;
				else
					nSelMenuItem = ( nSelMenuItem - 1 ) % MENU_ITEM_NUM;
#ifndef	SCPTS2
				//ShowArrowState(hWnd,nSelMenuItem);
#endif				
				BitBlt(hdc,
					SelItemRect[nSelMenuItem].left,
					SelItemRect[nSelMenuItem].top,
					SELITEM_WIDTH,
					SELITEM_HEIGHT,
					(HDC)hBmpCovSel,
					0,0,ROP_SRC);

				ReleaseDC (hWnd, hdc);
				
				SetBkColor(hdc, OldClr);
				SetBkMode(hdc, OldStyle);

			}
			break;

		case VK_DOWN:
			/*if (MENU_ITEM_NUM-1>nSelMenuItem)*/
			{
				COLORREF	OldClr;
				int OldStyle;
				HDC	hdc;
				
				hdc = GetDC (hWnd);

				OldClr = SetBkColor(hdc,SEL_COLOR);
				OldStyle = SetBkMode(hdc, NEWTRANSPARENT);

				BitBlt(hdc,
					SelItemRect[nSelMenuItem].left,
					SelItemRect[nSelMenuItem].top,
					SELITEM_WIDTH,
					SELITEM_HEIGHT,
					(HDC)hCoverBg,
					SelItemRect[nSelMenuItem].left,
					SelItemRect[nSelMenuItem].top,ROP_SRC);

				nSelMenuItem = ( nSelMenuItem + 1 ) % MENU_ITEM_NUM;
#ifndef	SCPTS2
				//ShowArrowState(hWnd,nSelMenuItem);
#endif

				BitBlt(hdc,
					SelItemRect[nSelMenuItem].left,
					SelItemRect[nSelMenuItem].top,
					SELITEM_WIDTH,
					SELITEM_HEIGHT,
					(HDC)hBmpCovSel,
					0,0,ROP_SRC);

				SetBkColor(hdc, OldClr);
				SetBkMode(hdc, OldStyle);

				ReleaseDC (hWnd, hdc);
			}
			break;

//		case VK_F5:
//			PostMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_CONFIRM,1),NULL);
//			break;

		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;
#endif
	case WM_DESTROY:

		WriteBombSetup();
		WriteBombHero();

		if (NULL != hCoverBg)
		{
			DeleteObject(hCoverBg);
			hCoverBg = NULL;
		}
/*
		if (NULL != hMenuBg)
		{
			DeleteObject(hMenuBg);
			hMenuBg = NULL;
		}
*/
		if (hBmpCovSel)
		{
			DeleteObject(hBmpCovSel);
			hBmpCovSel = NULL;
		}

		hWnd = NULL;
#if defined SCP_SMARTPHONE
		DlmNotify(PES_STCQUIT, (long)DepthBomb_AppControl);
		//DlmNotify((WPARAM)PES_APP_QUIT, (LPARAM)hBombInstance);
#elif defined	SEF_SMARTPHONE
		DlmNotify(PES_STCQUIT, (long)DepthBomb_AppControl);
#else
		DlmNotify((WPARAM)PES_APP_QUIT, (LPARAM)hBombInstance);
#endif
		
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
/*
	if (MENU_ITEM_NUM-1 == nSel)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKDOWN);
	if (0 == nSel)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKUP);
*/
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
		x1 = X_LT_SELITEM;
		y1 = Y_LT_SELITEM + i * Y_MENUITEM_INTERVAL;
		x2 = x1 + SELITEM_WIDTH;
		y2 = y1 + SELITEM_HEIGHT;
		SetRect(&(SelItemRect[i]),x1,y1,x2,y2);
		/*
		x1 = X_LT_MENUITEM;
		y1 = Y_LT_MENUITEM + i * Y_MENUITEM_INTERVAL;
		x2 = x1 + MENUITEM_WIDTH;
		y2 = y1 + MENUITEM_HEIGHT;
		SetRect(&(MenuItemRect[i]),x1,y1,x2,y2);
		*/
	}

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
		if (PtInRectXY(&(MenuItemRect[i]),x,y))
			return i;
	}
	return -1;
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
	if (i != nSel)
	{
		return FALSE;
	}

	OldClr = SetBkColor(hdc,SEL_COLOR);
	OldStyle = SetBkMode(hdc, NEWTRANSPARENT);

	BitBlt(hdc,
		SelItemRect[i].left,
		SelItemRect[i].top,
		SELITEM_WIDTH,
		SELITEM_HEIGHT,
		(HDC)hBmpCovSel,
		0,0,ROP_SRC);

	SetBkColor(hdc, OldClr);
	SetBkMode(hdc, OldStyle);

	return TRUE;
}

/*********************************************************************\
* Function	   BombMainSetup
* Purpose      启动设置窗口
* Params	   
* Return	 	   
* Remarks	   注册类并创建窗口
**********************************************************************/
static DWORD	BombMainSetup(HWND hWnd)
{
	WNDCLASS wc;
    DWORD dwRet;

    dwRet = TRUE;
	
	if (!bBombSetupWndClass)
	{
		wc.style         = CS_OWNDC;
		wc.lpfnWndProc   = BombSetupWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName  = "BombSetupWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bBombSetupWndClass = TRUE;
	}

	if (!IsWindow(hWndBombSetup))
	{
		hWndBombSetup = CreateWindow(
			"BombSetupWndClass", 
			IDS_SETUP, 
			WS_VISIBLE|WS_CAPTION|PWS_STATICBAR,
			PLX_WIN_POSITION,
/*	
			WND_X, 
			WND_Y+TOPSPACE, 
			WND_WIDTH, 
			WND_HEIGHT, 
*/			
			hWnd, 
			NULL,
			NULL, 
			NULL
			);
		
		if ( hWndBombSetup == NULL )
			return FALSE;
	}

	ShowWindow(hWndBombSetup,SW_SHOW);
	UpdateWindow(hWndBombSetup);

	return dwRet;
}

/*********************************************************************\
* Function	   BombSetupWndProc
* Purpose      设置窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK BombSetupWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam)
{
	LRESULT		lResult;
//	HDC			hdc;
	int			index;
//	RECT		txtRect;

	lResult = TRUE;

	switch(wMsgCmd)
	{
	case WM_CREATE:

#if defined(SCP_SMARTPHONE)
		SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
#endif
		CreateCapButton(hWnd, IDM_EXIT, 0, IDS_BACK);
		CreateCapButton(hWnd, IDM_CONFIRM,  1, IDS_CONFIRM);
		SendMessage(hWnd, PWM_SETBUTTONTEXT,2 , (LPARAM)"Select");

		hWnd_Max_Num = CreateWindow("SPINBOXEX",
									IDS_MAX_BOMB_NUM,
									WS_CHILD | WS_VISIBLE  |WS_TABSTOP |SSBS_CENTER,
									0,35,
									176,50,
									hWnd,
									(HMENU)IDC_SPINMAX,
									NULL,
									NULL);

		SendMessage(hWnd_Max_Num, SSBM_ADDSTRING, NULL, (LPARAM)IDS_THREE);
		SendMessage(hWnd_Max_Num, SSBM_ADDSTRING, NULL, (LPARAM)IDS_FOUR);
		SendMessage(hWnd_Max_Num, SSBM_ADDSTRING, NULL, (LPARAM)IDS_FIVE);

		if (!(BASIC_BOMB_NUM <= nBombNum && DEPTHBOMB_NUM >= nBombNum))
		{
			nBombNum = BASIC_BOMB_NUM;
		}
		SendMessage(hWnd_Max_Num, SSBM_SETCURSEL, (WPARAM)(nBombNum - BASIC_BOMB_NUM), NULL);

		hWnd_Prize = CreateWindow(
								"BUTTON",
								IDS_SPECIEL_EFFECT,
								BS_AUTOCHECKBOX|WS_VISIBLE|WS_CHILD|WS_TABSTOP,
								X_BTN_PRIZE-5,
								Y_BTN_PRIZE,
								BTN_PRIZE_WIDTH,
								BTN_PRIZE_HEIGHT,
								hWnd,
								(HMENU)IDC_BTN_PRIZE,
								NULL,
								NULL);
		if (bPrizePermit)
			SendMessage(hWnd_Prize,BM_SETCHECK,BST_CHECKED,NULL);
		else
			SendMessage(hWnd_Prize,BM_SETCHECK,BST_UNCHECKED,NULL);

		break;

	case WM_ACTIVATE:
		if (WA_ACTIVE == wParam)
		{
			SetFocus(hWnd_Max_Num);
		}
		break;
/*
	case WM_PAINT:
		hdc = BeginPaint(hWnd,NULL);
//		DrawImageFromFile(hdc,"/rom/progman/grpbg.bmp",0,0,SRCCOPY);
		txtRect.left = 0;
		txtRect.top	=	Y_MAX_NUM-5;
		txtRect.right = 176;
		txtRect.bottom	=	Y_MAX_NUM + 25-5;
		//TextOut(hdc,X_MAX_NUM,Y_MAX_NUM,IDS_MAX_BOMB_NUM,-1);
		DrawText(hdc, IDS_MAX_BOMB_NUM, -1, &txtRect, DT_CENTER|DT_VCENTER);
		EndPaint(hWnd,NULL);
		break;
*/
	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_RETURN:
			if (-1 != (index = SendMessage(hWnd_Max_Num,SSBM_GETCURSEL,NULL,NULL)))
			{
				nBombNum = index + BASIC_BOMB_NUM;
			}

			if (BST_CHECKED==SendMessage(hWnd_Prize,BM_GETCHECK,NULL,NULL))
			{
				bPrizePermit = TRUE;
			}
			else
			{
				bPrizePermit = FALSE;
			}

			DestroyWindow(hWnd);
			break;

		case VK_F5:
			if (hWnd_Prize == GetFocus()) {
				
				if (BST_CHECKED==SendMessage(hWnd_Prize,BM_GETCHECK,NULL,NULL))
				{
					bPrizePermit=FALSE;
					SendMessage(hWnd_Prize, BM_SETCHECK, BST_UNCHECKED, NULL);
					SendMessage(hWnd_Prize, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_PUSHDOWN),0);
				}
				else
				{
					bPrizePermit=TRUE;
					SendMessage(hWnd_Prize, BM_SETCHECK, BST_CHECKED, NULL);
					SendMessage(hWnd_Prize, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_DISABLE),0);
				}
				UpdateWindow(hWnd);
			}
			break;
		case VK_F10:
			DestroyWindow(hWnd);
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;
#if 0
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_CONFIRM:

			if (-1 != (index = SendMessage(hWnd_Max_Num,SSBM_GETCURSEL,NULL,NULL)))
			{
				nBombNum = index + BASIC_BOMB_NUM;
			}

			if (BST_CHECKED==SendMessage(hWnd_Prize,BM_GETCHECK,NULL,NULL))
			{
				bPrizePermit = TRUE;
			}
			else
			{
				bPrizePermit = FALSE;
			}

			DestroyWindow(hWnd);

			break;
//		case IDM_

		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;


		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;
#endif
	case WM_DESTROY:
		hWndBombSetup = NULL;
		UnregisterClass("BombSetupWndClass",NULL);
		bBombSetupWndClass = FALSE;
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

/*********************************************************************\
* Function	   BombNewGameProc
* Purpose      启动游戏窗口
* Params	   
* Return	 	   
* Remarks	   注册类并创建窗口
**********************************************************************/
static DWORD	BombNewGameProc(HWND hWnd)
{
	WNDCLASS wc;
    DWORD dwRet;

    dwRet = TRUE;

	if (!bBombGameWndClass)
	{
		wc.style         = CS_OWNDC;
		wc.lpfnWndProc   = BombGameWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName  = "BombGameWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bBombGameWndClass = TRUE;
	}

	if (!IsWindow(hWndBombGame))
	{
		hWndBombGame = CreateWindow(
			"BombGameWndClass", 
			IDS_NEWGAME, 
			WS_VISIBLE|PWS_STATICBAR,
			PLX_WIN_POSITION,
/*			
			WND_X, 
			WND_Y+TOPSPACE, 
			WND_WIDTH, 
			WND_HEIGHT, 
*/
			hWnd, 
			NULL,
			NULL, 
			NULL
			);
		
		if ( hWndBombGame == NULL )
			return FALSE;
		SendMessage(hWndBombGame, PWM_SETSCROLLSTATE, SCROLLLEFT|SCROLLRIGHT, MASKLEFT|MASKRIGHT);
		SendMessage(hWndBombGame,PWM_SETSCROLLSTATE,SCROLLMIDDLE,MASKMIDDLE);
	}

	ShowWindow(hWndBombGame,SW_SHOW);
	UpdateWindow(hWndBombGame);

	return dwRet;
}

/*********************************************************************\
* Function	   BombGameWndProc
* Purpose      游戏窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK BombGameWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam)
{
	LRESULT		lResult;
	int			i;

	lResult = TRUE;

	switch(wMsgCmd)
	{
	case WM_CREATE:

		nScore = 0;
		nOldScore = 0;
		srand(GetTickCount());
		CreateCapButton(hWnd, IDM_EXIT, 0, IDS_BACK);
		CreateCapButton(hWnd, IDM_CONFIRM,  1, IDS_BEGIN);
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

		bGameStart	= FALSE;
		bGamePause	= FALSE;
		bLeftDown	= FALSE;
		bRightDown	= FALSE;
		bGameOver = FALSE;
		SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_BEGIN);
//		hBmpSeaSurFace	= LoadImage(NULL,SEASURFACE_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
		{
			HDC hdc;
			COLORREF	Color;
			BOOL	bTran;
			
			hdc = GetDC(hWnd_BombMain);
			hBmpGunShip		= CreateBitmapFromImageFile(hdc,GUNSHIP_PATH,&Color,&bTran);
			hBmpMinGunShip	= CreateBitmapFromImageFile(hdc,MINSHIP_PATH,&Color,&bTran);
			hBmpGameBack	= CreateBitmapFromImageFile(hdc,BOMBGAMEBACK,&Color,&bTran);
			hBmpDepthSub	= CreateBitmapFromImageFile(hdc,DEPTHSUB_PATH,&Color,&bTran);
			hShipBombAndLife = CreateBitmapFromImageFile(hdc,DEPTHBOMBSIGN,&Color,&bTran);
			hBmpTorpedo		= CreateBitmapFromImageFile(hdc,DEPTHSUBSHOT_PATH,&Color,&bTran);
			hBmpPrizeAndBomb = CreateBitmapFromImageFile(hdc,DEPTHSUBPRIZE_PATH,&Color,&bTran);
			hBmpGameScoreText = CreateBitmapFromImageFile(hdc, BOMBGTEXT,&Color,&bTran);
			ReleaseDC(hWnd_BombMain,hdc);
		}

//		hDirLeftSub		= LoadImage(NULL,SUBTOL_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hDirRightSub	= LoadImage(NULL,SUBTOR_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hDepthBomb		= LoadImage(NULL,DEPBOMB_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hAvailableBomb	= LoadImage(NULL,AVAILBOMB_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hLSubDying[0]	= LoadImage(NULL,LSUBDYING0_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hLSubDying[1]	= LoadImage(NULL,LSUBDYING1_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hLSubDying[2]	= LoadImage(NULL,LSUBDYING2_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hRSubDying[0]	= LoadImage(NULL,RSUBDYING0_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hRSubDying[1]	= LoadImage(NULL,RSUBDYING1_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hRSubDying[2]	= LoadImage(NULL,RSUBDYING2_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hSubPrize[0]	= LoadImage(NULL,PRIZE_MIN_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hSubPrize[1]	= LoadImage(NULL,PRIZE_KILLALL_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hSubPrize[2]	= LoadImage(NULL,PRIZE_NOENEMY_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hTorpedo		= LoadImage(NULL,TORPEDO_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hMissil			= LoadImage(NULL,MISSIL_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		hShipLive		= LoadImage(NULL,SHIPLIVE_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);
//		bBmpNoEnemy		= LoadImage(NULL,NOENEMY_PATH,IMAGE_BITMAP,0,0,LR_LOADFROMFILE);

		InitSubmarine();
		InitDepthBomb();
		InitPrizeStete();
		InitTorpedoState();
		nGunShipLive = 0;
		nWaveFrame = 0;
		InitGunShip();
		break;

	case WM_ACTIVATE:
		if (WA_ACTIVE == wParam)
		{
			SetFocus(hWnd);
		}
		else if (WA_INACTIVE == wParam)
		{
			if (bGameStart && !bGamePause)
			{
				bLeftDown = FALSE;
				bRightDown = FALSE;
				PostMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_CONFIRM,1),NULL);
			}
		}
		break;
	case WM_ERASEBKGND:
		break;

	case WM_PAINT:
/*		if (bGameStart)
		{
			lResult = OnDepthBombGamePaint(hWnd,wParam,lParam);
		}
		else
		{
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}*/
		lResult = OnDepthBombGamePaint(hWnd,wParam,lParam);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_LEFT:
			bLeftDown = TRUE;
			
			if ((GUNSHIP_STATE_LIVE == nGunShipState || GUNSHIP_STATE_DYING == nGunShipState)&&(!bGamePause))
			{
				nShipMoveDir = GetShipDir(bLeftDown,bRightDown);
				switch (nShipMoveDir)
				{
				case DIR_NONE:
					break;
					
				case DIR_LEFT:
					xGunShip -= X_GUNSHIP_MOVE_INTERVAL;
					if (0 > xGunShip)
					{
						xGunShip = 0;
					}
					break;
					
				case DIR_RIGHT:
					xGunShip += X_GUNSHIP_MOVE_INTERVAL;
					if (SEASURFACE_WIDTH < xGunShip+nGunShipWidth)
					{
						xGunShip = SEASURFACE_WIDTH - nGunShipWidth;
					}
					break;
					
				default:
					nShipMoveDir = DIR_NONE;
					break;
				}
				InvalidateRect(hWnd,NULL,TRUE);
				//OS_UpdateScreen(0,0,0,0);
			}			
			
			break;
			
		case VK_RIGHT:
			bRightDown = TRUE;
			if ((GUNSHIP_STATE_LIVE == nGunShipState || GUNSHIP_STATE_DYING == nGunShipState)&&(!bGamePause))
			{
				nShipMoveDir = GetShipDir(bLeftDown,bRightDown);
				switch (nShipMoveDir)
				{
				case DIR_NONE:
					break;
					
				case DIR_LEFT:
					xGunShip -= X_GUNSHIP_MOVE_INTERVAL;
					if (0 > xGunShip)
					{
						xGunShip = 0;
					}
					break;
					
				case DIR_RIGHT:
					xGunShip += X_GUNSHIP_MOVE_INTERVAL;
					if (SEASURFACE_WIDTH < xGunShip+nGunShipWidth)
					{
						xGunShip = SEASURFACE_WIDTH - nGunShipWidth;
					}
					break;
					
				default:
					nShipMoveDir = DIR_NONE;
					break;
				}
				InvalidateRect(hWnd,NULL,TRUE);
				OS_UpdateScreen(0,0,0,0);
			}		
			
			
			break;

		case VK_F5:
			if (bGameStart&&!bGamePause)
			{
				i = FindFirstBombByState(STATE_AVAILABLE);
				if (-1 != i)
				{
					ShotBomb(i);
				}
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	case WM_KEYUP:
		bLeftDown = FALSE;
		bRightDown = FALSE;
		switch (wParam)
		{
		case VK_LEFT:
			bLeftDown = FALSE;
			break;

		case VK_RIGHT:
			bRightDown = FALSE;
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	case WM_TIMER:

		if (bGameStart&&!bGamePause)
		{
			switch (wParam)
			{
			case GAME_TIMER_ID:
				i = GetFirstDiedSubInd();
				if (-1 != i)
				{
					ProduceSub(i);
				}
				if (0 < nNoEnemyTimeLeft)
				{
					nNoEnemyTimeLeft --;
				}
				if (0 < nMinTimeLeft)
				{
					nMinTimeLeft --;
					if (0 >= nMinTimeLeft)
					{
						nGunShipWidth = NORMAL_GUNSHIP_WIDTH;
						MakeGunShipInRect();
					}
				}
				break;

			//case WAVE_MOVE_TIMER_ID:
			case SHIP_MOVE_TIMER_ID:
				
			/*				if (MAX_WAVE_FRAME-1 > nWaveFrame)
			{
			nWaveFrame++;
			}
			else
			{
			nWaveFrame = 0;
			}*/
				if ((nScore/SCORE_PER_PLANE)-(nOldScore/SCORE_PER_PLANE))
				{
					ActiveSubPlane();
				}
				nOldScore = nScore;
				MoveSubPlane();
				MovePlaneBomb();
				MoveSubmarine();
				DropDepthBomb();
				DepBombKillSub();
				ChangeSubDyingFrame();
				PrizeAscend();
				ShotTorpedo();
				TorpetoAscend();
				
				i = FindFirstBombByState(STATE_INAVAILABLE);
				if (-1 != i)
				{
					DepthBomb[i].state = STATE_AVAILABLE;
				}
				//InvalidateRect(hWnd,NULL,TRUE);
				//break;
				
				if (GUNSHIP_STATE_DYING == nGunShipState)
				{
					if (0 < nGunShipLive)
					{
						nGunShipDieFrame ++;
						if (GUNSHIP_DIE_FRAME_NUM <= nGunShipDieFrame)
						{
							nGunShipState = GUNSHIP_STATE_LIVE;
						}
					}
					else
					{
						//EndGameDepthBomb(hWnd);
						PLXTipsWin(NULL, hWnd, WM_SELOK, IDS_GAMEOVER, IDS_DEPTHBOMB, Notify_Info, IDS_CONFIRM, NULL, WAITTIMEOUT);
//						if (nScore > BombHero[BOMB_HERO_NUM-1].nScore)
//						{
//							SetHeroProc(hWnd);
//						}
					}
				}	
				
				if (GUNSHIP_STATE_LIVE == nGunShipState || GUNSHIP_STATE_DYING == nGunShipState)
				{
					nShipMoveDir = GetShipDir(bLeftDown,bRightDown);
					switch (nShipMoveDir)
					{
					case DIR_NONE:
						break;
						
					case DIR_LEFT:
						xGunShip -= X_GUNSHIP_MOVE_INTERVAL;
						if (0 > xGunShip)
						{
							xGunShip = 0;
						}
						break;
						
					case DIR_RIGHT:
						xGunShip += X_GUNSHIP_MOVE_INTERVAL;
						if (SEASURFACE_WIDTH < xGunShip+nGunShipWidth)
						{
							xGunShip = SEASURFACE_WIDTH - nGunShipWidth;
						}
						break;
						
					default:
						nShipMoveDir = DIR_NONE;
						break;
					}
					
				}
				InvalidateRect(hWnd,NULL,TRUE);
				OS_UpdateScreen(0,0,0,0);
				break;
			}
		}

		break;
/*
	case UPSHIP_MOVE_TIMER_ID:
		if (GUNSHIP_STATE_LIVE == nGunShipState || GUNSHIP_STATE_DYING == nGunShipState)
		{
			nShipMoveDir = GetShipDir(bLeftDown,bRightDown);
			switch (nShipMoveDir)
			{
			case DIR_NONE:
				break;
				
			case DIR_LEFT:
				xGunShip -= X_GUNSHIP_MOVE_INTERVAL;
				if (0 > xGunShip)
				{
					xGunShip = 0;
				}
				break;
				
			case DIR_RIGHT:
				xGunShip += X_GUNSHIP_MOVE_INTERVAL;
				if (SEASURFACE_WIDTH < xGunShip+nGunShipWidth)
				{
					xGunShip = SEASURFACE_WIDTH - nGunShipWidth;
				}
				break;
				
			default:
				nShipMoveDir = DIR_NONE;
				break;
			}
			InvalidateRect(hWnd,NULL,TRUE);
			OS_UpdateScreen(0,0,0,0);
		}			
		break;
*/
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_CONFIRM:
			if (!bGameStart)
			{
				nScore = 0;
				nOldScore = 0;
				bLeftDown = FALSE;
				bRightDown = FALSE;;
				nGunShipLive = GUNSHIP_START_LIVE;
				bGameStart = TRUE;
				bGamePause = FALSE;
				bGameOver = FALSE;
				SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_PAUSE);
				//nGameTime = GAME_TIME(0);
				InitSubmarine();
				InitDepthBomb();
				InitPrizeStete();
				InitTorpedoState();
				InitPlaneBomb();
				SetTimer(hWnd,GAME_TIMER_ID,1000,NULL);
				nWaveFrame = 0;
				//SetTimer(hWnd,WAVE_MOVE_TIMER_ID,WAVE_MOVE_TIMER_ELAPSE,NULL);
				InitGunShip();
				InitSubPlane();
				SetTimer(hWnd,SHIP_MOVE_TIMER_ID,SHIP_MOVE_ELAPSE,NULL);
			//	SetTimer(hWnd, UPSHIP_MOVE_TIMER_ID, UPSHIP_MOVE_ELAPSE, NULL);
				InvalidateRect(hWnd,NULL,TRUE);
			}
			else
			{
				if (!bGamePause)
				{
					bGamePause = TRUE;
					SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_CONTINUE);
				}
				else
				{
					bGamePause = FALSE;
					SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_PAUSE);
				}
			}
			break;

		case IDM_EXIT:
			if (bGameStart)
			{
				PLXConfirmWinEx(NULL, hWnd, IDS_GAME_PROMPT, Notify_Request, IDS_DEPTHBOMB, IDS_CONFIRM, ML("Cancel"), IDRM_RMSG);
				
				
				//lmx modify
				//MsgSelWin(hWnd,IDS_GAME_PROMPT,IDS_DEPTHBOMB);
			}
			else
			{
				DestroyWindow(hWnd);
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	case WM_SELCANCEL:
		break;

	case IDRM_RMSG:
		if (lParam)
			PostMessage(hWnd, WM_SELOK, 0, 0);	
		break;

	case WM_SELOK:
		EndGameDepthBomb(hWnd);
		if (nScore > BombHero[BOMB_HERO_NUM-1].nScore)
		{
			SetHeroProc(hWnd_BombMain);
		}
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY:

		OnGameDestroy(hWnd,wParam,lParam);

		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;
}

/*********************************************************************\
* Function		OnDepthBombGamePaint
* Purpose		游戏窗口paint过程  
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	OnDepthBombGamePaint(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	#if 1
	HDC hdc;
	HDC hdcMem;
	PAINTSTRUCT	ps;
	COLORREF	OldClr,OldTextClr;
	int			OldStyle;
	char		buf[16];
	RECT rcClient;
	HFONT	hSmall,hold;

	sprintf(buf,SCORE_FORMAT,nScore);

	hdc = BeginPaint(hWnd,&ps);
	
	GetClientRect(hWnd, &rcClient);
	hdcMem = CreateMemoryDC(rcClient.right, rcClient.bottom);
	SetClipRect(hdcMem, &ps.rcPaint);

	BitBlt(hdcMem,X_BMP_GAMEBACK,Y_BMP_GAMEBACK,
		GAMEBACK_WIDTH,GAMEBACK_HEIGHT,
		(HDC)hBmpGameBack,0,0,ROP_SRC);

	BitBlt(hdcMem,X_GAMESCORETEXT,Y_GAMESCORETEXT,
		GAMESCORETEXT_WIDTH,GAMESCORETEXT_HEIGHT,
		(HDC)hBmpGameScoreText,0,0,ROP_SRC);


	OldClr = SetBkColor(hdcMem,COLOR_WHITE);
	OldStyle = SetBkMode(hdcMem,TRANSPARENT);
	OldTextClr = SetTextColor(hdcMem,RGB(254,254,254));
//	TextOut(hdcMem,X_GAMESCORETEXT,Y_GAMESCORETEXT, IDS_GAMESCORETEXT,strlen(IDS_GAMESCORETEXT));
	GetFontHandle(&hSmall,SMALL_FONT);
	hold = SelectObject(hdcMem,hSmall);
	TextOut(hdcMem,X_GAMESCORE,Y_GAMESCORE,buf,strlen(buf));
	SelectObject(hdcMem,hold);
	SetTextColor(hdcMem,OldTextClr);
	SetBkColor(hdcMem,RGB(254,254,254));
	SetBkMode(hdcMem,NEWTRANSPARENT);

	DrawSubPlane(hdcMem);
	DrawPlaneBomb(hdcMem);
	DrawGunShip(hdcMem);
	//DrawWave(hdcMem);
	DrawSubmarine(hdcMem);
	DrawShotBomb(hdcMem);
	DrawAvailableBomb(hdcMem);
	DrawPrize(hdcMem);
	DrawTorpedo(hdcMem);
	DrawShipLive(hdcMem);
	
	SetBkColor(hdcMem,OldClr);
	SetBkMode(hdcMem,OldStyle);

	if (bGameOver)
	{
		DrawImageFromFile(hdcMem,PATH_GAMEOVER,X_GAMEOVER,Y_GAMEOVER,ROP_SRC);
	}

	BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, ROP_SRC);

	DeleteDC(hdcMem);

	EndPaint(hWnd,&ps);
#else
	HDC hdc;
	PAINTSTRUCT	ps;
	COLORREF	OldClr,OldTextClr;
	int			OldStyle;
	char		buf[16];

	sprintf(buf,SCORE_FORMAT,nScore);
	hdc = BeginPaint(hWnd,&ps);

	BitBlt(hdc,X_BMP_GAMEBACK,Y_BMP_GAMEBACK,
		GAMEBACK_WIDTH,GAMEBACK_HEIGHT,
		(HDC)hBmpGameBack,0,0,ROP_SRC);

	OldClr = SetBkColor(hdc,COLOR_WHITE);
	OldStyle = SetBkMode(hdc,TRANSPARENT);
	OldTextClr = SetTextColor(hdc,RGB(254,254,254));
//	TextOut(hdc,X_GAMESCORETEXT,Y_GAMESCORETEXT, IDS_GAMESCORETEXT,strlen(IDS_GAMESCORETEXT));
	TextOut(hdc,X_GAMESCORE,Y_GAMESCORE,buf,strlen(buf));
	SetTextColor(hdc,OldTextClr);
	SetBkColor(hdc,RGB(254,254,254));
	SetBkMode(hdc,NEWTRANSPARENT);

	DrawSubPlane(hdc);
	DrawPlaneBomb(hdc);
	DrawGunShip(hdc);
	//DrawWave(hdc);
	DrawSubmarine(hdc);
	DrawShotBomb(hdc);
	DrawAvailableBomb(hdc);
	DrawPrize(hdc);
	DrawTorpedo(hdc);
	DrawShipLive(hdc);
	
	SetBkColor(hdc,OldClr);
	SetBkMode(hdc,OldStyle);

	if (bGameOver)
	{
		DrawImageFromFile(hdc,PATH_GAMEOVER,X_GAMEOVER,Y_GAMEOVER,ROP_SRC);
	}

	EndPaint(hWnd,&ps);

#endif


	return TRUE;
}

/*********************************************************************\
* Function	   DrawWave
* Purpose      画浪
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*static BOOL	DrawWave(HDC hdc)
{
	int xWave;

	for (xWave = X_WAVE_LEFT_MAX + nWaveFrame * X_WAVE_INTERVAL;
	xWave < SEASURFACE_WIDTH;xWave += WAVE_WIDTH)
	{
		BitBlt(hdc, xWave,Y_WAVE_TOP,
				WAVE_WIDTH , WAVE_HEIGHT , 
			   (HDC)hBmpSeaSurFace, 0, 0, ROP_SRC);
	}

	return TRUE;
}*/

/*********************************************************************\
* Function	   InitSubPlane
* Purpose      初始化飞机
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void InitSubPlane()
{
	SubPlane.nState = PLANE_STATE_DIED;
	hBmpPlane = NULL;

	return ;
}

/*********************************************************************\
* Function	   ActiveSubPlane
* Purpose      激活飞机
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	ActiveSubPlane()
{
	if (PLANE_STATE_ALIVE == SubPlane.nState)
	{
		return FALSE;
	}

	SubPlane.nState = PLANE_STATE_ALIVE;
	SubPlane.x		= X_PLANE_START;
	SubPlane.y		= Y_PLANE_START;

	if (!hBmpPlane)
	{
		HDC hdc;
		COLORREF	Color;
		BOOL	bTran;

		hdc = GetDC(hWnd_BombMain);
		hBmpPlane = CreateBitmapFromImageFile(hdc,DEPTHBOMBPLANE, &Color,&bTran);
		ReleaseDC(hWnd_BombMain,hdc);
	}

	return TRUE;
}

/*********************************************************************\
* Function	   KillSubPlane
* Purpose      杀飞机
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	KillSubPlane()
{
	SubPlane.nState = PLANE_STATE_DIED;

	if (hBmpPlane)
	{
		DeleteObject(hBmpPlane);
		hBmpPlane = NULL;
	}

	return TRUE;
}

/*********************************************************************\
* Function	   MoveSubPlane
* Purpose      飞机飞行
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	MoveSubPlane()
{
	if (PLANE_STATE_DIED == SubPlane.nState)
	{
		return FALSE;
	}

	SubPlane.x -= PLANE_MOVE_INTERVAL;

	if (SubPlane.x <= 0-PLANE_WIDTH)
	{
		KillSubPlane();
		return FALSE;
	}

	ShotPlaneBomb();

	return TRUE;
}

/*********************************************************************\
* Function	   DrawSubPlane
* Purpose      画飞机
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DrawSubPlane(HDC hdc)
{
	COLORREF	OldClr;

	if (PLANE_STATE_DIED == SubPlane.nState)
	{
		return FALSE;
	}

	OldClr = SetBkColor(hdc,RGB(255,255,255));
	BitBlt(hdc, SubPlane.x,SubPlane.y,
			PLANE_WIDTH , PLANE_HEIGHT , 
		   (HDC)hBmpPlane, 0, 0, ROP_SRC);
	SetBkColor(hdc,OldClr);

	return TRUE;
}

/*********************************************************************\
* Function	   InitPlaneBomb
* Purpose      初始化飞机炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL InitPlaneBomb()
{
	int i;

	for (i=0;i<PLANE_BOMB_NUM;i++)
	{
		PlaneBomb[i].state = STATE_AVAILABLE;
	}

	return TRUE;
}

/*********************************************************************\
* Function	   PlaneBombDispear
* Purpose      飞机炸弹消失
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	PlaneBombDispear(int i)
{
	RECT rect;

	if (i<0 || i>=PLANE_BOMB_NUM)
	{
		return FALSE;
	}

	rect.left = X_BMP_GAMEBACK;
	rect.right = X_BMP_GAMEBACK + GAMEBACK_WIDTH;
	rect.top = Y_PLANE_START;
	rect.bottom = Y_LT_SEASURFACE + SEA_HEIGHT;

	if (PtInRectXY(&rect,PlaneBomb[i].x_Bomb,PlaneBomb[i].y_Bomb))
	{
		return FALSE;
	}

	PlaneBomb[i].state = STATE_AVAILABLE;

	return TRUE;
}

/*********************************************************************\
* Function	   GetFirstAvailablePlaneBomb
* Purpose      得第一个可用的飞机炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int GetFirstAvailablePlaneBomb()
{
	int i;

	for (i=0;i<PLANE_BOMB_NUM;i++)
	{
		if (STATE_AVAILABLE == PlaneBomb[i].state)
		{
			return i;
		}
	}

	return -1;
}

/*********************************************************************\
* Function	   ShotPlaneBomb
* Purpose      发射飞机炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	ShotPlaneBomb()
{
	int i;

	i = GetFirstAvailablePlaneBomb();

	if (i<0 || i>=PLANE_BOMB_NUM)
	{
		return FALSE;
	}

	PlaneBomb[i].state = STATE_SHOTTED;
	PlaneBomb[i].x_Bomb = SubPlane.x;
	PlaneBomb[i].y_Bomb = SubPlane.y + PLANE_HEIGHT;

	return TRUE;
}

/*********************************************************************\
* Function		MovePlaneBomb
* Purpose		移动飞机炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	MovePlaneBomb()
{
	int i;

	for (i=0;i<PLANE_BOMB_NUM;i++)
	{
		if (STATE_SHOTTED == PlaneBomb[i].state)
		{
			PlaneBomb[i].y_Bomb += BOMB_DROP_STEP;
			PlaneBombDispear(i);
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   DrawPlaneBomb
* Purpose      画飞机的炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DrawPlaneBomb(HDC hdc)
{
	int i;

	for (i=0;i<PLANE_BOMB_NUM;i++)
	{
		if (STATE_SHOTTED == PlaneBomb[i].state)
		{
			BitBlt(hdc, PlaneBomb[i].x_Bomb,PlaneBomb[i].y_Bomb,
					BOMB_SHOT_WIDTH , BOMB_SHOT_HEIGHT , 
				   (HDC)hBmpPrizeAndBomb, X_BOMB_SHOT, 0, ROP_SRC);
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   InitGunShip
* Purpose      初始化炮船
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void	InitGunShip()
{
	nGunShipWidth	= NORMAL_GUNSHIP_WIDTH;
	nGunShipHeight	= NORMAL_GUNSHIP_HEIGHT;
	xGunShip		= (SEASURFACE_WIDTH - NORMAL_GUNSHIP_WIDTH)/2;
	nShipMoveDir = DIR_NONE;
	nGunShipState = GUNSHIP_STATE_LIVE;
	nNoEnemyTimeLeft = 0;
	nMinTimeLeft = 0;

	return ;
}

/*********************************************************************\
* Function	  DrawGunShip 
* Purpose     画炮船 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DrawGunShip(HDC hdc)
{
	MakeGunShipInRect();

	if (GUNSHIP_STATE_LIVE == nGunShipState)
	{
		if (0 < nMinTimeLeft)
		{
			BitBlt(hdc, xGunShip,Y_GUNSHIP_TOP,
					nGunShipWidth , nGunShipHeight , 
				   (HDC)hBmpMinGunShip, 0, 0, ROP_SRC);
		}
		else
		{
			BitBlt(hdc, xGunShip,Y_GUNSHIP_TOP,
					nGunShipWidth , nGunShipHeight , 
				   (HDC)hBmpGunShip, 0, 0, ROP_SRC);
		}

		if (0 < nNoEnemyTimeLeft)
		{
			BitBlt(hdc, xGunShip,Y_GUNSHIP_TOP,
					NOENEMY_WIDTH , NOENEMY_HEIGHT , 
				   (HDC)hBmpPrizeAndBomb, X_NOENEMY, 0, ROP_SRC);
		}
	}
	else if (GUNSHIP_STATE_DYING == nGunShipState)
	{
		switch (nGunShipDieFrame)
		{
		case 0:
		case 2:
		case 4:
		case 6:
		case 8:
		case 10:
			break;
			
		case 1:
		case 3:
		case 5:
		case 7:
		case 9:
		case 11:
			if (0 < nMinTimeLeft)
			{
				BitBlt(hdc, xGunShip,Y_GUNSHIP_TOP,
						nGunShipWidth , nGunShipHeight , 
					   (HDC)hBmpMinGunShip, 0, 0, ROP_SRC);
			}
			else
			{
				BitBlt(hdc, xGunShip,Y_GUNSHIP_TOP,
						nGunShipWidth , nGunShipHeight , 
					   (HDC)hBmpGunShip, 0, 0, ROP_SRC);
			}
			break;
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   MakeGunShipInRect
* Purpose      使炮船在游戏区域
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void MakeGunShipInRect()
{
	if (0 > xGunShip)
	{
		xGunShip = 0;
	}

	if (SEASURFACE_WIDTH < xGunShip+nGunShipWidth)
	{
		xGunShip = SEASURFACE_WIDTH - nGunShipWidth;
	}
	return ;
}

/*********************************************************************\
* Function	   GetShipDir
* Purpose      得炮船移动的方向
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int	GetShipDir(BOOL bLeft,BOOL bRight)
{
	int dir;

	dir = DIR_NONE;
	if (bLeft)
		dir = DIR_LEFT;
	if (bRight)
		dir = DIR_RIGHT;
	if (bLeft && bRight)
		dir = DIR_NONE;

	return dir;
}

/*********************************************************************\
* Function	   EndGameDepthBomb
* Purpose      结束游戏
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	EndGameDepthBomb(HWND hWnd)
{
	bGameStart = FALSE;
	bGamePause = FALSE;
	bGameOver = TRUE;
	KillSubPlane();
	InitPlaneBomb();
	SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_BEGIN);
	KillTimer(hWnd,GAME_TIMER_ID);
	//KillTimer(hWnd,WAVE_MOVE_TIMER_ID);
	KillTimer(hWnd,SHIP_MOVE_TIMER_ID);
//	KillTimer(hWnd,UPSHIP_MOVE_TIMER_ID);

	return TRUE;
}

/*********************************************************************\
* Function	  random 
* Purpose     得0到n-1的随机数 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int random(int n)
{
	return (rand()%n);
}

/*********************************************************************\
* Function	   InitSubmarine
* Purpose      初始化全部潜艇
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	InitSubmarine()
{
	int i;

	for (i=0;i<MAX_SUBNUM;i++)
	{
		SubMarine[i].pNext = NULL;
		SubMarine[i].state = STATE_DIED;
	}

	return TRUE;
}

/*********************************************************************\
* Function	   GetFirstDiedSubInd
* Purpose      得第一个未用潜艇
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int	GetFirstDiedSubInd()
{
	int index;

	for (index=0;index<MAX_SUBNUM;index++)
	{
		if (STATE_DIED == SubMarine[index].state)
		{
			return index;
		}
	}

	return -1;
}

/*********************************************************************\
* Function	   ProduceSub
* Purpose      填充潜艇结构
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	ProduceSub(const int i)
{
	int k,n;

	if (0>i || MAX_SUBNUM<=i)
	{
		return FALSE;
	}

	for (n=0;n<8;n++)
	{
		SubMarine[i].nDirect = random(2);
		if (DIR_LEFT == SubMarine[i].nDirect)
		{
			SubMarine[i].x_Sub = X_LT_SEASURFACE + SEASURFACE_WIDTH;
		}
		else if (DIR_RIGHT == SubMarine[i].nDirect)
		{
			SubMarine[i].x_Sub = X_LT_SEASURFACE - SUBMARINE_WIDTH;
		}

		k = random(SUB_TIERNUM);
		SubMarine[i].y_Sub = SUB_Y(k);

		if (-1 == LayOverSub(i))
			break;
	}

	if (8 <= n)
	{
		return FALSE;
	}

	if (SUB_TIERNUM-1 == k)
	{
		SubMarine[i].SubShotStyle = SHOT_MISSIL;
	}
	else
	{
		SubMarine[i].SubShotStyle = SHOT_NORMAL;
	}

	if (bPrizePermit)
	{
		if (0 == random(SUBNUM_PERPRIZE))
		{
			SubMarine[i].Prize = PRIZE_MIN + random(PRIZE_NUM);
		}
		else
		{
			SubMarine[i].Prize = NO_PRIZE;
		}
	}
	else
	{
		SubMarine[i].Prize = NO_PRIZE;
	}

	SubMarine[i].state = STATE_ALIVE;

	return TRUE;
}

/*********************************************************************\
* Function	   MoveSubmarine
* Purpose      移动活的潜艇
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	MoveSubmarine()
{
	int i;

	for (i=0;i<MAX_SUBNUM;i++)
	{
		if (STATE_ALIVE == SubMarine[i].state)
		{
			switch(SubMarine[i].nDirect)
			{
			case DIR_LEFT:
				SubMarine[i].x_Sub -= SUB_MOVE_STEP;
				break;

			case DIR_RIGHT:
				SubMarine[i].x_Sub += SUB_MOVE_STEP;
				break;
			}
		}
	}

	ScanSubDispear();

	return TRUE;
}

/*********************************************************************\
* Function	   ScanSubDispear
* Purpose      扫描，使消失的潜艇死亡
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	ScanSubDispear()
{
	int i;

	for (i=0;i<MAX_SUBNUM;i++)
	{
		if (STATE_ALIVE == SubMarine[i].state)
		{
			switch(SubMarine[i].nDirect)
			{
			case DIR_LEFT:
				if (X_LT_SEASURFACE - SUBMARINE_WIDTH > SubMarine[i].x_Sub)
				{
					SubMarine[i].state = STATE_DIED;
				}
				break;

			case DIR_RIGHT:
				if (X_LT_SEASURFACE + SEASURFACE_WIDTH < SubMarine[i].x_Sub)
				{
					SubMarine[i].state = STATE_DIED;
				}
				break;
			}
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   LayOverSub
* Purpose      判断第i个潜艇是否与其他活着的潜艇重叠
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int	LayOverSub(const int i)
{
	int n;

	for (n=0;n<MAX_SUBNUM;n++)
	{
		if (i == n)
			continue;

		if (STATE_ALIVE != SubMarine[n].state)
			continue;

		if (SubMarine[i].y_Sub == SubMarine[n].y_Sub)
		{
			if (SubMarine[i].nDirect == SubMarine[n].nDirect)
			{
				if (SUBMARINE_WIDTH+BOMB_SHOT_WIDTH > abs(SubMarine[i].x_Sub-SubMarine[n].x_Sub))
				{
					return n;
				}
			}
		}
	}

	return -1;
}

/*********************************************************************\
* Function	   DrawSubmarine
* Purpose      画潜艇
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DrawSubmarine(HDC hdc)
{
	int i;
	int	x_;

	for (i=0;i<MAX_SUBNUM;i++)
	{
		if (STATE_ALIVE == SubMarine[i].state)
		{
			if (DIR_LEFT == SubMarine[i].nDirect)
			{
				BitBlt(hdc, SubMarine[i].x_Sub,SubMarine[i].y_Sub,
						SUBMARINE_WIDTH , SUBMARINE_HEIGHT , 
					   (HDC)hBmpDepthSub, X_SUBMARINE_TOL, Y_SUBMARINE, ROP_SRC);
			}
			else if (DIR_RIGHT == SubMarine[i].nDirect)
			{
				BitBlt(hdc, SubMarine[i].x_Sub,SubMarine[i].y_Sub,
						SUBMARINE_WIDTH , SUBMARINE_HEIGHT , 
					   (HDC)hBmpDepthSub, X_SUBMARINE_TOR, Y_SUBMARINE, ROP_SRC);
			}
		}
		else if (STATE_DYING == SubMarine[i].state)
		{
			if (DIR_LEFT == SubMarine[i].nDirect)
			{
				x_ = X_SUBTOL_DIEFRAME(SubMarine[i].DieFrame);
				if (((SUBMARINE_WIDTH*5) <= x_) && ((SUBMARINE_WIDTH*7) >= x_))
				{
					BitBlt(hdc, SubMarine[i].x_Sub,SubMarine[i].y_Sub,
							SUBMARINE_WIDTH , SUBMARINE_HEIGHT , 
						   (HDC)hBmpDepthSub,
						   x_, Y_SUBMARINE, ROP_SRC);
				}
			}
			else if (DIR_RIGHT == SubMarine[i].nDirect)
			{
				x_ = X_SUBTOR_DIEFRAME(SubMarine[i].DieFrame);
				if (((SUBMARINE_WIDTH*2) <= x_) && ((SUBMARINE_WIDTH*4) >= x_))
				{
					BitBlt(hdc, SubMarine[i].x_Sub,SubMarine[i].y_Sub,
							SUBMARINE_WIDTH , SUBMARINE_HEIGHT , 
						   (HDC)hBmpDepthSub, 
						   x_, Y_SUBMARINE, ROP_SRC);
				}
			}
		}
	}
	return TRUE;
}

/*********************************************************************\
* Function	   FindFirstBombByState
* Purpose      找第一个指定状态的炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int	FindFirstBombByState(const int state)
{
	int i;

	for (i=0;i<nBombNum;i++)
	{
		if (state == DepthBomb[i].state)
		{
			return i;
		}
	}

	return -1;
}

/*********************************************************************\
* Function	   ShotBomb
* Purpose      发射炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	ShotBomb(int i)
{
	if (0 > i ||nBombNum <= i)
	{
		return FALSE;
	}

	DepthBomb[i].x_Bomb = xGunShip + (nGunShipWidth - BOMB_SHOT_WIDTH) / 2;
	DepthBomb[i].y_Bomb = Y_GUNSHIP_TOP + nGunShipHeight;
	if (LayOverDepBomb(i))
	{
		return FALSE;
	}
	DepthBomb[i].state = STATE_SHOTTED;

	return TRUE;
}

/*********************************************************************\
* Function	   LayOverDepBomb
* Purpose      判断第i个炸弹是否覆盖其他炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL LayOverDepBomb(const int i)
{
	int n;

	for (n=0;n<nBombNum;n++)
	{
		if (i == n)
			continue;

		if (STATE_SHOTTED != DepthBomb[n].state)
			continue;

		if (BOMB_SHOT_WIDTH > abs(DepthBomb[n].x_Bomb - DepthBomb[i].x_Bomb))
		{
			if (BOMB_SHOT_HEIGHT > abs(DepthBomb[n].y_Bomb - DepthBomb[i].y_Bomb))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*********************************************************************\
* Function	   OnGameDestroy
* Purpose      游戏界面销毁的处理
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	OnGameDestroy(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
//	int i;

/*	if (hBmpSeaSurFace)
	{
		DeleteObject(hBmpSeaSurFace);
		hBmpSeaSurFace = NULL;
	}*/

	KillSubPlane();

	if (hBmpPrizeAndBomb)
	{
		DeleteObject(hBmpPrizeAndBomb);
		hBmpPrizeAndBomb = NULL;
	}

	if (hBmpTorpedo)
	{
		DeleteObject(hBmpTorpedo);
		hBmpTorpedo = NULL;
	}

	if (hBmpGameBack)
	{
		DeleteObject(hBmpGameBack);
		hBmpGameBack = NULL;
	}

	if (hBmpGameScoreText)
	{
		DeleteObject(hBmpGameScoreText);
		hBmpGameScoreText = NULL;
	}
	

	if (hBmpDepthSub)
	{
		DeleteObject(hBmpDepthSub);
		hBmpDepthSub = NULL;
	}

	if (hShipBombAndLife)
	{
		DeleteObject(hShipBombAndLife);
		hShipBombAndLife = NULL;
	}

/*	if (hShipLive)
	{
		DeleteObject(hShipLive);
		hShipLive = NULL;
	}*/

	if (hBmpMinGunShip)
	{
		DeleteObject(hBmpMinGunShip);
		hBmpMinGunShip = NULL;
	}

	if (hBmpGunShip)
	{
		DeleteObject(hBmpGunShip);
		hBmpGunShip = NULL;
	}

/*	if (hDirLeftSub)
	{
		DeleteObject(hDirLeftSub);
		hDirLeftSub = NULL;
	}*/

/*	if (hDirRightSub)
	{
		DeleteObject(hDirRightSub);
		hDirRightSub = NULL;
	}*/

/*	if (hDepthBomb)
	{
		DeleteObject(hDepthBomb);
		hDepthBomb = NULL;
	}*/

/*	if (hAvailableBomb)
	{
		DeleteObject(hAvailableBomb);
		hAvailableBomb = NULL;
	}*/

/*	if (hTorpedo)
	{
		DeleteObject(hTorpedo);
		hTorpedo = NULL;
	}*/

/*	if (hMissil)
	{
		DeleteObject(hMissil);
		hMissil = NULL;
	}*/

/*	if (bBmpNoEnemy)
	{
		DeleteObject(bBmpNoEnemy);
		bBmpNoEnemy = NULL;
	}*/

/*	for (i=0;i<DIEFRAMENUM;i++)
	{
		if (hLSubDying[i])
		{
			DeleteObject(hLSubDying[i]);
			hLSubDying[i] = NULL;
		}
		if (hRSubDying[i])
		{
			DeleteObject(hRSubDying[i]);
			hRSubDying[i] = NULL;
		}
	}*/

/*	for (i=0;i<PRIZE_NUM;i++)
	{
		if (hSubPrize[i])
		{
			DeleteObject(hSubPrize[i]);
			hSubPrize[i] = NULL;
		}
	}*/

	KillTimer(hWnd,GAME_TIMER_ID);
	KillTimer(hWnd,WAVE_MOVE_TIMER_ID);
	KillTimer(hWnd,SHIP_MOVE_TIMER_ID);
//	KillTimer(hWnd,UPSHIP_MOVE_TIMER_ID);

	hWndBombGame = NULL;
	UnregisterClass("BombGameWndClass",NULL);
	bBombGameWndClass = FALSE;

	return TRUE;
}

/*********************************************************************\
* Function	   DropDepthBomb
* Purpose      使发射的炸弹下落
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DropDepthBomb()
{
	int i;

	for (i=0;i<nBombNum;i++)
	{
		if (STATE_SHOTTED == DepthBomb[i].state)
		{
			DepthBomb[i].y_Bomb += BOMB_DROP_STEP;
			if (JudgeBombDispear(i))
			{
				DepthBomb[i].state = STATE_INAVAILABLE;
			}
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   JudgeBombDispear
* Purpose      判断第i个炸弹是否消失
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	JudgeBombDispear(const int i)
{
	if (0 > i ||nBombNum <= i)
	{
		return FALSE;
	}

	if (Y_LT_SEASURFACE + SEA_HEIGHT < DepthBomb[i].y_Bomb)
	{
		return TRUE;
	}

	return FALSE;
}

/*********************************************************************\
* Function	   DrawShotBomb
* Purpose      画发射的炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DrawShotBomb(HDC hdc)
{
	int i;

	for (i=0;i<nBombNum;i++)
	{
		if (STATE_SHOTTED == DepthBomb[i].state)
		{
			BitBlt(hdc, DepthBomb[i].x_Bomb,DepthBomb[i].y_Bomb,
					BOMB_SHOT_WIDTH , BOMB_SHOT_HEIGHT , 
				   (HDC)hBmpPrizeAndBomb, X_BOMB_SHOT, 0, ROP_SRC);
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   InitDepthBomb
* Purpose      初始化炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	InitDepthBomb()
{
	int i;

	for (i=0;i<nBombNum;i++)
	{
		DepthBomb[i].state = STATE_AVAILABLE;
	}

	return TRUE;
}

/*********************************************************************\
* Function	   DrawAvailableBomb
* Purpose      画待投的炸弹
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL DrawAvailableBomb(HDC hdc)
{
	int i,n;

	n = 0;
	for (i=0;i<nBombNum;i++)
	{
		if (STATE_AVAILABLE == DepthBomb[i].state)
		{
			BitBlt(hdc, X_LTFIRSTAVAILBOMB+n*(AVAILBOMB_WIDTH+X_AVAILBOMB_INTERVAL),
				Y_LTFIRSTAVAILBOMB,
					 AVAILBOMB_WIDTH,  AVAILBOMB_HEIGHT, 
				   (HDC)hShipBombAndLife, X_AVAILBOMB_OFBMP, 0, ROP_SRC);
			n++;
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	  TwoRectLayOver 
* Purpose     判断两个矩形是否重叠 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	TwoRectLayOver(RECT rect1,RECT rect2)
{
	if (PtInRectXY(&rect1,rect2.left,rect2.top))
	{
		return TRUE;
	}

	if (PtInRectXY(&rect1,rect2.left,rect2.bottom))
	{
		return TRUE;
	}

	if (PtInRectXY(&rect1,rect2.right,rect2.top))
	{
		return TRUE;
	}

	if (PtInRectXY(&rect1,rect2.right,rect2.bottom))
	{
		return TRUE;
	}

	if (PtInRectXY(&rect2,rect1.left,rect1.top))
	{
		return TRUE;
	}

	if (PtInRectXY(&rect2,rect1.left,rect1.bottom))
	{
		return TRUE;
	}

	if (PtInRectXY(&rect2,rect1.right,rect1.top))
	{
		return TRUE;
	}

	if (PtInRectXY(&rect2,rect1.right,rect1.bottom))
	{
		return TRUE;
	}

	return FALSE;
}

/*********************************************************************\
* Function	   DepBombKillSub
* Purpose      炸弹炸潜艇
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DepBombKillSub()
{
	BOOL brtn;
	RECT RectSub,RectBomb;
	int i,j;

	brtn = FALSE;
	for (i=0;i<nBombNum;i++)
	{
		if (STATE_SHOTTED == DepthBomb[i].state)
		{
			RectBomb.left	= DepthBomb[i].x_Bomb;
			RectBomb.top	= DepthBomb[i].y_Bomb;
			RectBomb.right	= RectBomb.left + BOMB_SHOT_WIDTH;
			RectBomb.bottom = RectBomb.top + BOMB_SHOT_HEIGHT;
			for (j=0;j<MAX_SUBNUM;j++)
			{
				if (STATE_ALIVE == SubMarine[j].state)
				{
					RectSub.left	= SubMarine[j].x_Sub;
					RectSub.top		= SubMarine[j].y_Sub;
					RectSub.right	= RectSub.left + SUBMARINE_WIDTH;
					RectSub.bottom	= RectSub.top + SUBMARINE_HEIGHT;
					if (TwoRectLayOver(RectSub,RectBomb))
					{
						DepthBomb[i].state = STATE_INAVAILABLE;
						SubMarine[j].state = STATE_DYING;
						if (NO_PRIZE != SubMarine[j].Prize)
						{
							ShotPrize(j);
						}
						SubMarine[j].DieFrame = 0;
						nScore += SCORE_PER_SUB;
						brtn = TRUE;
					}
				}
			}
		}
	}

	for (i=0;i<PLANE_BOMB_NUM;i++)
	{
		if (STATE_SHOTTED == PlaneBomb[i].state)
		{
			RectBomb.left	= PlaneBomb[i].x_Bomb;
			RectBomb.top	= PlaneBomb[i].y_Bomb;
			RectBomb.right	= RectBomb.left + BOMB_SHOT_WIDTH;
			RectBomb.bottom = RectBomb.top + BOMB_SHOT_HEIGHT;
			for (j=0;j<MAX_SUBNUM;j++)
			{
				if (STATE_ALIVE == SubMarine[j].state)
				{
					RectSub.left	= SubMarine[j].x_Sub;
					RectSub.top		= SubMarine[j].y_Sub;
					RectSub.right	= RectSub.left + SUBMARINE_WIDTH;
					RectSub.bottom	= RectSub.top + SUBMARINE_HEIGHT;
					if (TwoRectLayOver(RectSub,RectBomb))
					{
						PlaneBomb[i].state = STATE_AVAILABLE;
						SubMarine[j].state = STATE_DYING;
						if (NO_PRIZE != SubMarine[j].Prize)
						{
							ShotPrize(j);
						}
						SubMarine[j].DieFrame = 0;
						nScore += SCORE_PER_SUB;
						brtn = TRUE;
					}
				}
			}
		}
	}

	return brtn;
}

/*********************************************************************\
* Function	   ChangeSubDyingFrame
* Purpose      改变潜艇死亡的针
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	ChangeSubDyingFrame()
{
	int i;

	for (i=0;i<MAX_SUBNUM;i++)
	{
		if (STATE_DYING == SubMarine[i].state)
		{
			if (0 <= SubMarine[i].DieFrame && DIEFRAMENUM > SubMarine[i].DieFrame)
			{
				SubMarine[i].DieFrame ++;
				if (DIEFRAMENUM <= SubMarine[i].DieFrame)
				{
					SubMarine[i].state = STATE_DIED;
				}
			}
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   InitPrizeStete
* Purpose      初始化奖励的状态
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	InitPrizeStete()
{
	int i;

	for (i=0;i<SUBPRIZE_NUM;i++)
	{
		SubPrize[i].PrizeState = PRIZESTATE_DIED;
	}

	return TRUE;
}

/*********************************************************************\
* Function	   FindFirstDiedPrize
* Purpose      寻找第一个未用的奖励
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int	FindFirstDiedPrize()
{
	int i;

	for (i=0;i<SUBPRIZE_NUM;i++)
	{
		if (PRIZESTATE_DIED == SubPrize[i].PrizeState)
		{
			return i;
		}
	}

	return -1;
}

/*********************************************************************\
* Function	   ShotPrize
* Purpose      发射奖励
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	ShotPrize(int nSub)
{
	int nFirstPrize;

	if (0 > nSub || MAX_SUBNUM <= nSub)
	{
		return FALSE;
	}

	nFirstPrize = FindFirstDiedPrize();
	if (-1 == nFirstPrize)
	{
		return FALSE;
	}

	if (NO_PRIZE != SubMarine[nSub].Prize)
	{
		SubPrize[nFirstPrize].PrizeStyle = SubMarine[nSub].Prize;
	}
	else
	{
		return FALSE;
	}
	SubPrize[nFirstPrize].x_Prize = SubMarine[nSub].x_Sub;
	SubPrize[nFirstPrize].y_Prize = SubMarine[nSub].y_Sub;
	if (X_LT_SEASURFACE > SubPrize[nFirstPrize].x_Prize)
	{
		SubPrize[nFirstPrize].x_Prize = X_LT_SEASURFACE;
	}
	if (X_LT_SEASURFACE+SEASURFACE_WIDTH-PRIZE_WIDTH < SubPrize[nFirstPrize].x_Prize)
	{
		SubPrize[nFirstPrize].x_Prize = X_LT_SEASURFACE+SEASURFACE_WIDTH-PRIZE_WIDTH;
	}
	if (!LayOverPrize(nFirstPrize))
	{
		SubPrize[nFirstPrize].PrizeState = PRIZESTATE_ALIVE;
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

/*********************************************************************\
* Function	   LayOverPrize
* Purpose      判断奖励是否重叠
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	LayOverPrize(const int nPrize)
{
	int i;
	RECT rect1,rect2;

	rect1.left		= SubPrize[nPrize].x_Prize;
	rect1.right		= rect1.left + PRIZE_WIDTH;
	rect1.top		= SubPrize[nPrize].y_Prize;
	rect1.bottom	= rect1.top + PRIZE_HEIGHT;
	for (i=0;i<SUBPRIZE_NUM;i++)
	{
		if (i == nPrize)
		{
			continue;
		}

		if (PRIZESTATE_ALIVE == SubPrize[i].PrizeState)
		{
			rect2.left		= SubPrize[i].x_Prize;
			rect2.right		= rect1.left + PRIZE_WIDTH;
			rect2.top		= SubPrize[i].y_Prize;
			rect2.bottom	= rect1.top + PRIZE_HEIGHT;
			if (TwoRectLayOver(rect1,rect2))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*********************************************************************\
* Function	   PrizeAscend
* Purpose      奖励上升
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	PrizeAscend()
{
	BOOL brtn;
	int i;

	brtn = FALSE;
	for (i=0;i<SUBPRIZE_NUM;i++)
	{
		if (PRIZESTATE_ALIVE == SubPrize[i].PrizeState)
		{
			SubPrize[i].y_Prize -= PRIZE_ASCEND_INTERVAL;
			ShipEatPrize(i);
			DispearPrize(i);
			brtn = TRUE;
		}
	}
	return brtn;
}

/*********************************************************************\
* Function	   ShipEatPrize
* Purpose      吃奖励
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	ShipEatPrize(int nPrize)
{
	RECT rect1,rect2;
/*
	if (GUNSHIP_STATE_DYING == nGunShipState)
	{
		return FALSE;
	}

	if (PRIZESTATE_DIED == SubPrize[nPrize].PrizeState)
	{
		return FALSE;
	}
*/
	rect1.left		= SubPrize[nPrize].x_Prize;
	rect1.right		= rect1.left + PRIZE_WIDTH;
	rect1.top		= SubPrize[nPrize].y_Prize;
	rect1.bottom	= rect1.top + PRIZE_HEIGHT;

	rect2.left		= xGunShip;
	rect2.right		= rect2.left + nGunShipWidth;
	rect2.top		= Y_GUNSHIP_TOP;
	rect2.bottom	= rect2.top + nGunShipHeight;

	if (TwoRectLayOver(rect1,rect2))
	{
		switch (SubPrize[nPrize].PrizeStyle)
		{
		case PRIZE_MIN:
			nMinTimeLeft = SHIP_MIN_TIME;
			nGunShipWidth = MIN_GUNSHIP_WIDTH;
			break;

		case PRIZE_KILLALL:
			KillAllSubmarine();
			break;

		case PRIZE_NOENEMY:
			nNoEnemyTimeLeft = NO_ENEMY_TIME;
			break;
		}
		SubPrize[nPrize].PrizeState = PRIZESTATE_DIED;
		return TRUE;
	}

	return FALSE;
}

/*********************************************************************\
* Function	   DispearPrize
* Purpose      奖励出水后消失
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DispearPrize(int nPrize)
{
	if (PRIZESTATE_DIED == SubPrize[nPrize].PrizeState)
	{
		return TRUE;
	}

	if (Y_LT_SEASURFACE > SubPrize[nPrize].y_Prize)
	{
		SubPrize[nPrize].PrizeState = PRIZESTATE_DIED;
		return TRUE;
	}

	return FALSE;
}

/*********************************************************************\
* Function	   DrawPrize
* Purpose      画奖励
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DrawPrize(HDC hdc)
{
	int i;
	
	for (i=0;i<SUBPRIZE_NUM;i++)
	{
		if (PRIZESTATE_ALIVE == SubPrize[i].PrizeState)
		{
			BitBlt(hdc, SubPrize[i].x_Prize,SubPrize[i].y_Prize,
					PRIZE_WIDTH , PRIZE_HEIGHT , 
				   (HDC)hBmpPrizeAndBomb, 
				   X_SUBPRIZE(SubPrize[i].PrizeStyle), 0, ROP_SRC);
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   InitTorpedoState
* Purpose      初始化鱼雷状态
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	InitTorpedoState()
{
	int i;

	for (i=0;i<MAX_TORPEDO_NUM;i++)
	{
		Torpedo[i].nState = TORPEDO_STATE_DIED;
	}

	return TRUE;
}

/*********************************************************************\
* Function	   FindFirstDiedTorpedo
* Purpose      找第一个未用鱼雷
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int FindFirstDiedTorpedo()
{
	int i;

	for (i=0;i<MAX_TORPEDO_NUM;i++)
	{
		if (TORPEDO_STATE_DIED == Torpedo[i].nState)
		{
			return i;
		}
	}

	return -1;
}

/*********************************************************************\
* Function	   DispearTorpedo
* Purpose      判断指定鱼雷消失
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DispearTorpedo(const int nTorpedo)
{
	if (0 > nTorpedo || MAX_TORPEDO_NUM <= nTorpedo)
	{
		return FALSE;
	}
	if (TORPEDO_STATE_ALIVE == Torpedo[nTorpedo].nState)
	{
		if (Y_LT_SEASURFACE > Torpedo[nTorpedo].y)
		{
			Torpedo[nTorpedo].nState = TORPEDO_STATE_DIED;
			return TRUE;
		}
	}

	return FALSE;
}

/*********************************************************************\
* Function	   ShotTorpedo
* Purpose      发射鱼雷
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	ShotTorpedo()
{
	int nTorpedo;
	int i;

	nTorpedo = FindFirstDiedTorpedo();
	if (-1 == nTorpedo)
	{
		return FALSE;
	}

	for (i=0;i<MAX_SUBNUM;i++)
	{
		if (STATE_ALIVE == SubMarine[i].state)
		{
			if (0 == random(TIMES_PER_TORPETO))
			{
				Torpedo[nTorpedo].x	= SubMarine[i].x_Sub;
				Torpedo[nTorpedo].y	= SubMarine[i].y_Sub;
				Torpedo[nTorpedo].x_save	= 0;
				Torpedo[nTorpedo].y_save	= 0;
				Torpedo[nTorpedo].nStyle	= SubMarine[i].SubShotStyle;
				if (X_LT_SEASURFACE > Torpedo[nTorpedo].x)
				{
					Torpedo[nTorpedo].x = X_LT_SEASURFACE;
				}
				if (X_LT_SEASURFACE + SEASURFACE_WIDTH - TORPEDO_WIDTH < Torpedo[nTorpedo].x)
				{
					Torpedo[nTorpedo].x = X_LT_SEASURFACE + SEASURFACE_WIDTH - TORPEDO_WIDTH;
				}
				if (!LayOverTorpedo(nTorpedo))
				{
					Torpedo[nTorpedo].nState = TORPEDO_STATE_ALIVE;
				}
			}
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   LayOverTorpedo
* Purpose      判断鱼雷是否重叠
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	LayOverTorpedo(const int nTorpedo)
{
	int i;
	RECT rect1,rect2;

	rect1.left		= Torpedo[nTorpedo].x;
	rect1.right		= rect1.left + TORPEDO_WIDTH;
	rect1.top		= Torpedo[nTorpedo].y;
	rect1.bottom	= rect1.top + TORPEDO_HEIGHT;
	for (i=0;i<MAX_TORPEDO_NUM;i++)
	{
		if (i == nTorpedo)
		{
			continue;
		}

		if (TORPEDO_STATE_ALIVE == Torpedo[i].nState)
		{
			rect2.left		= Torpedo[i].x;
			rect2.right		= rect1.left + TORPEDO_WIDTH;
			rect2.top		= Torpedo[i].y;
			rect2.bottom	= rect1.top + TORPEDO_HEIGHT;
			if (TwoRectLayOver(rect1,rect2))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*********************************************************************\
* Function	   TorpetoAscend
* Purpose      鱼雷上升
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	TorpetoAscend()
{
	int i;
	double xDisToShip,yDisToShip,DisToShip;
	double x_add,y_add;

	for (i=0;i<MAX_TORPEDO_NUM;i++)
	{
		if (TORPEDO_STATE_ALIVE == Torpedo[i].nState)
		{
			switch (Torpedo[i].nStyle)
			{
			case SHOT_NORMAL:
				Torpedo[i].y -= TORPEDO_MOVE_STEP;
				break;

			case SHOT_MISSIL:
				if (Y_MISSIL_VALID < Torpedo[i].y)
				{
					xDisToShip = xGunShip - Torpedo[i].x;
					yDisToShip = Y_LT_SEASURFACE - Torpedo[i].y;
					DisToShip = sqrt(xDisToShip*xDisToShip + yDisToShip*yDisToShip);
					x_add = xDisToShip/DisToShip*(double)TORPEDO_MOVE_STEP+Torpedo[i].x_save;
					y_add = yDisToShip/DisToShip*(double)TORPEDO_MOVE_STEP+Torpedo[i].y_save;
					Torpedo[i].x += (int)x_add;
					Torpedo[i].y += (int)y_add;
					Torpedo[i].x_save = x_add - (int)x_add;
					Torpedo[i].y_save = y_add - (int)y_add;
				}
				else
				{
					Torpedo[i].y -= TORPEDO_MOVE_STEP;
				}
				break;
			}
			TorpedoKillShip(i);
			DispearTorpedo(i);
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   DrawTorpedo
* Purpose      画鱼雷（含导弹）
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DrawTorpedo(HDC hdc)
{
	int i;

	for (i=0;i<MAX_TORPEDO_NUM;i++)
	{
		if (TORPEDO_STATE_ALIVE == Torpedo[i].nState)
		{
			switch (Torpedo[i].nStyle)
			{
			case SHOT_NORMAL:
				BitBlt(hdc, Torpedo[i].x,Torpedo[i].y,
						TORPEDO_WIDTH , TORPEDO_HEIGHT , 
					   (HDC)hBmpTorpedo, X_TORPEDO, 0, ROP_SRC);
				break;

			case SHOT_MISSIL:
				BitBlt(hdc, Torpedo[i].x,Torpedo[i].y,
						TORPEDO_WIDTH , TORPEDO_HEIGHT , 
					   (HDC)hBmpTorpedo, X_MISSIL, 0, ROP_SRC);
				break;
			}
		}
	}

	return TRUE;
}

/*********************************************************************\
* Function	   TorpedoKillShip
* Purpose      鱼雷杀船
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	TorpedoKillShip(const int nTorpedo)
{
	RECT rect1,rect2;

	if (0 < nNoEnemyTimeLeft)
	{
		return FALSE;
	}
	if (GUNSHIP_STATE_DYING == nGunShipState)
	{
		return FALSE;
	}
	if (0 > nTorpedo || MAX_TORPEDO_NUM <= nTorpedo)
	{
		return FALSE;
	}
	rect1.left		= Torpedo[nTorpedo].x;
	rect1.right		= rect1.left + TORPEDO_WIDTH;
	rect1.top		= Torpedo[nTorpedo].y;
	rect1.bottom	= rect1.top + TORPEDO_HEIGHT;

	rect2.left		= xGunShip;
	rect2.right		= rect2.left + nGunShipWidth;
	rect2.top		= Y_GUNSHIP_TOP;
	rect2.bottom	= rect2.top + nGunShipHeight;

	if (TwoRectLayOver(rect1,rect2))
	{
		Torpedo[nTorpedo].nState = TORPEDO_STATE_DIED;
		RetrieveGunShip();
		return TRUE;
	}

	return FALSE;
}

/*********************************************************************\
* Function	   RetrieveGunShip
* Purpose      重新得到炮船
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	RetrieveGunShip()
{
	nGunShipWidth	= NORMAL_GUNSHIP_WIDTH;
	nGunShipHeight	= NORMAL_GUNSHIP_HEIGHT;
	xGunShip		= (SEASURFACE_WIDTH - NORMAL_GUNSHIP_WIDTH)/2;
	nShipMoveDir = DIR_NONE;
	nNoEnemyTimeLeft = 0;
	nMinTimeLeft = 0;
	nGunShipState = GUNSHIP_STATE_DYING;
	nGunShipDieFrame = 0;
	nGunShipLive --;

	return TRUE;
}

/*********************************************************************\
* Function	   DrawShipLive
* Purpose      画船的命
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DrawShipLive(HDC hdc)
{
	int i;

	for (i=0;i<nGunShipLive;i++)
	{
		BitBlt(hdc, X_LTFIRSTSHIPLIVE+i*(SHIPLIVE_WIDTH+X_SHIPLIVE_INTERVAL),
			Y_LTFIRSTSHIPLIVE,
				 SHIPLIVE_WIDTH,  SHIPLIVE_HEIGHT, 
			   (HDC)hShipBombAndLife, X_SHIPLIFE_OFBMP, 0, ROP_SRC);
	}

	return TRUE;
}

/*********************************************************************\
* Function	   KillAllSubmarine
* Purpose      杀掉所有活着的潜艇
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	KillAllSubmarine()
{
	int i;
	BOOL brtn;

	brtn = FALSE;

	for (i=0;i<MAX_SUBNUM;i++)
	{
		if (STATE_ALIVE == SubMarine[i].state)
		{
			SubMarine[i].state = STATE_DYING;
			if (NO_PRIZE != SubMarine[i].Prize)
			{
				ShotPrize(i);
			}
			SubMarine[i].DieFrame = 0;
			nScore += SCORE_PER_SUB;
			brtn = TRUE;
		}
	}

	return brtn;
}

/*********************************************************************\
* Function	   ReadBombHero
* Purpose      读深水炸弹的英雄
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	BOOL	ReadBombHero()
{
	int	hFile;
	int i;

	if ((hFile=(open(BOMB_HERO_FILE,O_RDONLY)))==-1)
		return FALSE;
	if (lseek(hFile,0,SEEK_SET)==-1)
	{
		close(hFile);
		return FALSE;
	}
	if ((read(hFile,BombHero,sizeof(BOMBHERO)*BOMB_HERO_NUM))==-1)
	{
		close(hFile);
		return FALSE;
	}
	close(hFile);
	for (i=0;i<BOMB_HERO_NUM;i++)
	{
		if (0 == BombHero[i].nScore)
			strcpy(BombHero[i].Name,IDS_ANYMOUS);
	}

	return TRUE;
}

/*********************************************************************\
* Function	   InitBombHero
* Purpose      初始化英雄信息
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	void	InitBombHero()
{
	int i;

	for (i=0;i<BOMB_HERO_NUM;i++)
	{
		strcpy(BombHero[i].Name,IDS_ANYMOUS);
		BombHero[i].nScore = 0;
	}

	return;
}

/*********************************************************************\
* Function	   WriteBombHero
* Purpose      写英雄信息
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	BOOL	WriteBombHero()
{
	int	hFile;
	if (-1==(hFile=(open(BOMB_HERO_FILE,O_WRONLY|O_TRUNC))))
		if (-1==(hFile=open(BOMB_HERO_FILE,O_WRONLY|O_CREAT,0666)))
			return	FALSE;
	if (-1==lseek(hFile,0,SEEK_SET))
	{
		close(hFile);
		return FALSE;
	}
	if (-1==write(hFile,BombHero,sizeof(BOMBHERO)*BOMB_HERO_NUM))
	{
		close(hFile);
		return	FALSE;
	}

	close(hFile);
	return TRUE;
}

/*********************************************************************\
* Function	   BombHeroProc
* Purpose      英雄榜入口
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	DWORD	BombHeroProc(HWND hWnd)
{
	WNDCLASS wc;
    DWORD dwRet;

    dwRet = TRUE;

	if (!bBombHeroWndClass)
	{
		wc.style         = CS_OWNDC;
		wc.lpfnWndProc   = BombHeroWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName  = "BombHeroWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bBombHeroWndClass = TRUE;
	}

	hWndBombHero = CreateWindow(
		"BombHeroWndClass", 
		IDS_HEROBOARD, 
		WS_VISIBLE|WS_CAPTION|PWS_STATICBAR,
		PLX_WIN_POSITION,
/*
		WND_X, 
		WND_Y+TOPSPACE,//TOPSPACE, 
		WND_WIDTH, 
		WND_HEIGHT,// - TOPSPACE, 
*/		
		hWnd, 
		NULL,
		NULL, 
		NULL
		);
	
	if ( hWndBombHero == NULL )
		return FALSE;

	CreateCapButton(hWndBombHero, IDM_CONFIRM , 1, IDS_CLEAR);
	CreateCapButton(hWndBombHero, IDM_EXIT,  0, IDS_BACK);
	ShowWindow(hWndBombHero,SW_SHOW);
	UpdateWindow(hWndBombHero);

	return dwRet;
}

/*********************************************************************\
* Function	   BombHeroWndProc
* Purpose      英雄榜窗口过程函数
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK BombHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT			lResult;
	int		i;
	char	str[30];
	HDC		hdc;
	int OldStyle;
	RECT	rcClient;

	switch (wMsgCmd)
	{
		/*
	case WM_CREATE:
		CreateCapButton(hWnd, IDM_CONFIRM , 1, IDS_CLEAR);
		CreateCapButton(hWnd, IDM_EXIT,  0, IDS_BACK);

		break;
*/
	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDM_EXIT:			

			DestroyWindow(hWnd);

			break;
		case IDM_CONFIRM:	

			InitBombHero();
			WriteBombHero();
			GetClientRect(hWnd, &rcClient);
			InvalidateRect(hWnd,&rcClient,TRUE);


//			DestroyWindow(hWnd);

			break;
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		
		break;

	case WM_ACTIVATE:
		if (wParam==WA_ACTIVE)
			SetFocus(hWnd);
		break;

	case WM_PAINT:
		{
			HFONT	hSmall,hold;
			char	NumBuf[3];
			GetFontHandle(&hSmall,SMALL_FONT);
			hdc=BeginPaint(hWnd,NULL);
			hold = SelectObject(hdc,hSmall);
			//DrawImageFromFile(hdc,PATH_GAMEBG0,0,0,ROP_SRC); "/rom/game/pao/score_bg.gif"
			DrawImageFromFile(hdc,"/rom/game/pao/score_bg.gif",0,0,ROP_SRC);
			OldStyle = SetBkMode(hdc,TRANSPARENT);
			for (i = 0; i < BOMB_HERO_NUM; i ++)
			{
				itoa(i+1,NumBuf,10);
				sprintf(str, "%s", BombHero[i].Name);
				TextOut(hdc, X_HERO_REC, HERO_REC_SPACE * i + Y_HERO_REC, NumBuf, strlen(NumBuf));
				TextOut(hdc, X_HERO_REC+20, HERO_REC_SPACE * i + Y_HERO_REC, str, strlen(str));
				sprintf(str, "%d", BombHero[i].nScore);
				TextOut(hdc, X_HERO_REC+100, HERO_REC_SPACE * i + Y_HERO_REC, str, strlen(str));
			}
			SelectObject(hdc,hold);
			SetBkMode(hdc,OldStyle);
			EndPaint(hWnd,NULL);
		}
		break;
/*
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
#ifndef SCP_SMARTPHONE
		case VK_F2:
			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
            break;
#endif
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;
*/
	case WM_DESTROY:
		hWndBombHero = NULL;
		UnregisterClass("BombHeroWndClass",NULL);
		bBombHeroWndClass = FALSE;
		break;

	default :

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        
		break;
	}
	return lResult;
}

/*********************************************************************\
* Function	   ReadBombSetup
* Purpose      读深水炸弹设置
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	BOOL	ReadBombSetup()
{
	int	hFile;
	if (-1==(hFile=(open(BOMB_SETUP_FILE,O_RDONLY))))
		return FALSE;
	if (-1==(read(hFile,&nBombNum,sizeof(int))))
	{
		close(hFile);
		return FALSE;
	}
	if (-1==(read(hFile,&bPrizePermit,sizeof(BOOL))))
	{
		close(hFile);
		return FALSE;
	}
	close(hFile);
	return TRUE;
}

/*********************************************************************\
* Function	   WriteBombSetup
* Purpose      写深水炸弹设置
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	BOOL	WriteBombSetup()
{
	int	hFile;

	if (-1==(hFile=(open(BOMB_SETUP_FILE,O_WRONLY|O_TRUNC))))
	{
		if (-1==(hFile=open(BOMB_SETUP_FILE,O_WRONLY|O_CREAT,0666)))
		{
			return	FALSE;
		}
	}

	if (-1==write(hFile,&nBombNum,sizeof(int)))
	{
		close(hFile);
		return	FALSE;
	}
	if (-1==write(hFile,&bPrizePermit,sizeof(BOOL)))
	{
		close(hFile);
		return	FALSE;
	}

	close(hFile);
	return TRUE;
}

/*********************************************************************\
* Function	   SetHeroProc
* Purpose      英雄留名入口
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	DWORD   SetHeroProc(HWND hWnd)
{
	WNDCLASS wc;
    DWORD dwRet;

    dwRet = TRUE;

	if (!bBombSetHeroWndClass)
	{
		wc.style         = CS_OWNDC;
		wc.lpfnWndProc   = BombSetHeroWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName  = "BombSetHeroWndClass";		
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bBombSetHeroWndClass = TRUE;
	}
	
    hGameSetHero = CreateWindow(
        "BombSetHeroWndClass", 
        IDS_SETNAME, 
        WS_VISIBLE|WS_POPUP|WS_CAPTION|PWS_STATICBAR,
		PLX_WIN_POSITION,
		/*
        WND_X,
        WND_Y+TOPSPACE,
        WND_WIDTH,
        WND_HEIGHT, 
		*/
        hWnd,
        NULL,
		NULL,
		NULL
	);

	if ( hGameSetHero == NULL )
			return FALSE;

	return	dwRet;
}

/*********************************************************************\
* Function	   BombSetHeroWndProc
* Purpose      英雄留名窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK BombSetHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	LRESULT lResult;
	IMEEDIT ime;

	memset(&ime, 0, sizeof(IMEEDIT));
	ime.hwndNotify	= (HWND)hWnd;    // 可以指定为应用程序主窗口句柄
	ime.dwAttrib	= NULL; // 调用通用输入界面
	ime.pszImeName	= NULL;            // 指定使用默认输入法
//	ie.auBtnStrID[0] = STR_IME_BTNEXIT;
//	ie.auBtnStrID[1] = STR_IME_BTNDONE;
	ime.uMsgSetText = 0;//IME_MSG_NAMEEXIT;
	ime.pszTitle = "";
	switch (wMsgCmd)	
    {
    case WM_CREATE:
#if defined(SCP_SMARTPHONE)
		SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
#endif
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, IDM_EXIT, (LPARAM)IDS_BACK);
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)IDS_CONFIRM);

		//创建“姓名”编辑框控件

		hBombHeroName= CreateWindow(
			"IMEEDIT",//"INPUTEDIT", 
			IDS_ANYMOUS, 
			WS_VISIBLE|WS_CHILD|ES_CENTER|WS_TABSTOP|ES_NOHIDESEL,
			NameEditRect.left , 
			NameEditRect.top , 
			NameEditRect.right- NameEditRect.left, 
			NameEditRect.bottom-NameEditRect.top, 
			hWnd, 
			(HMENU)IDC_HERONAME,
			NULL, 
			(PVOID)&ime
			);
		
		SendMessage(hBombHeroName, EM_LIMITTEXT, 8, NULL);
//		InputEditNode.lStyle = GetWindowLong(hBombHeroName, GWL_STYLE);
//		InputEditNode.nKeyBoardType = GetImeHandle ("全屏手写");//"英数"
//		strcpy(InputEditNode.szName, IDS_ANYMOUS);
//		SetWindowLong(hBombHeroName, GWL_USERDATA, (LONG)&InputEditNode);
		SendMessage(hBombHeroName,EM_SETSEL,-1,-1);
		break;

	case WM_DESTROY:

		hGameSetHero = NULL;
		//UnregisterClass("BombSetHeroWndClass",NULL);
		break;

	case WM_ACTIVATE:
		if (wParam == WA_ACTIVE)
			SetFocus(hBombHeroName);
		break;

	case WM_COMMAND:

		switch (LOWORD(wParam))
		{
		case IDM_CONFIRM:
			{
				char szTempName[15];
				
				GetWindowText(hBombHeroName, szTempName, 9);
				if ( szTempName[0] == '\0' )
				{
					//MsgWin(GetString(STR_INPUT_NOTICE),GetString(STR_PET_PROMPT),MSGWIN_TIMEOUT);
					MsgWin(NULL, NULL, 0, ML("error input"),IDS_DEPTHBOMB,Notify_Info, ML("OK") ,NULL,WAITTIMEOUT);
					break;
				}
				
				strcpy((BombHero+BOMB_HERO_NUM-1)->Name, szTempName);
				(BombHero+BOMB_HERO_NUM-1)->nScore = nScore;
				SortRecord();
				WriteBombSetup();
				DestroyWindow(hWnd);
			}
			break;

		case IDM_EXIT:
			{
				int nCaret_Start,nCaret_End;

				SendMessage(hBombHeroName,EM_GETSEL,(WPARAM)&nCaret_Start,(LPARAM)&nCaret_End);

				if (nCaret_Start||nCaret_End)
				{
					SendMessage(hBombHeroName, WM_KEYDOWN, VK_BACK, 0);
					SendMessage(hBombHeroName, WM_CHAR, VK_BACK, 0);
				}
				else
				{
					DestroyWindow(hWnd);
				}
			}

			break;

		}

		break;

/*	case WM_EDITEXIT:

		SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"退出");
		
		break;

	case WM_EDITCLEAR:

		SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"清除");

		break;
*/
	case WM_SETRBTNTEXT:
		if(strcmp((char *)lParam, IDS_EXIT) == 0)
            SendMessage (hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
        else
            SendMessage (hWnd, PWM_SETBUTTONTEXT, 0, lParam);
		break;

	case WM_CLOSE:

		DestroyWindow(hWnd);

		break;
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{


		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	case WM_PAINT:
		{
			RECT	rc;
			int		OldBkMode;
			hdc = BeginPaint(hWnd, NULL);
			OldBkMode = SetBkMode(hdc,TRANSPARENT);

			DrawText(hdc,IDS_CHEER,-1, &TitleRect,DT_HCENTER|DT_VCENTER);
			rc.top = TitleRect.top +25;
			rc.bottom = TitleRect.bottom +25;
			rc.left	= TitleRect.left;
			rc.right = TitleRect.right;
			DrawText(hdc, IDS_PLSETNAME, -1, &rc, DT_HCENTER|DT_VCENTER);
			SetBkMode(hdc, OldBkMode);
			EndPaint(hWnd, NULL);
		}
		break;

	default:

	    lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        break;
    }

    return lResult;
}

/*********************************************************************\
* Function	   SortRecord
* Purpose      重排英雄数组
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void	SortRecord()
{
	BOMBHERO	temp;
	int i;

	for (i=(BOMB_HERO_NUM-1);i>0;i--)
	{
		if ((BombHero+i)->nScore>((BombHero+i-1)->nScore))
		{
			strcpy(temp.Name,(BombHero+i-1)->Name);
			temp.nScore=(BombHero+i-1)->nScore;
			strcpy((BombHero+i-1)->Name,(BombHero+i)->Name);
			(BombHero+i-1)->nScore=(BombHero+i)->nScore;
			strcpy((BombHero+i)->Name,temp.Name);
			(BombHero+i)->nScore=temp.nScore;
		}
	}

	return;
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

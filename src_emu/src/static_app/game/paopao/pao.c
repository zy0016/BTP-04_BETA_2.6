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
#define __MODULE__ "PAO"
#include "pao.h"
#include "imesys.h"
//extern char Pao_ver	    [16];

static MENUITEMTEMPLATE PushBoxMenu[] =
{
	{ MF_STRING, IDM_NEWGAME, "New game", NULL},
	{ MF_STRING, IDM_SETUP, "Select", NULL },
	{ MF_STRING, IDM_HEROBOARD, "go on", NULL },
	{ MF_STRING, IDM_GAMESHOW, "help", NULL },
	//	{ MF_STRING, ID_SETTING, IDS_SETTING, NULL },
    { MF_END, 0, NULL, NULL }
};
//#endif

static const MENUTEMPLATE PaoMainMenuTemplate =
{
    0,
		PushBoxMenu
};
/*****************************************************************************\
* Funcation: GamePao_AppControl
*
* Purpose  : 应用程序入口
*
* Explain  : 
*                      
\*****************************************************************************/
DWORD GamePao_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	//------------------------------------------------------------------------
    WNDCLASS	wc;
    DWORD		dwRet;
	//------------------------------------------------------------------------
    dwRet = TRUE;
	
    switch (nCode)
    {
    case APP_INIT:
		
		//CopyFile("ROM:PaoMap.Dat","Flash2:PaoMap.dat");
		hPaoInstance = (HINSTANCE)pInstance;
		
		
		//        strcpy(Pao_ver,  "Va1.0.0");
		
        break;
		
	case APP_GETOPTION:
		
		switch( wParam )
		{
			
		case AS_ICON:
			
			dwRet = (DWORD)AppIconData;
			
			break;
			
		case AS_APPWND:
			//返回创建窗口句柄，没有值返回空。
			dwRet = (DWORD)hWnd_PaoMain;
			break;
		}
		
		break;
		
		case APP_ACTIVE :
			// 注册泡泡龙主窗口类
			
			
			if (IsWindow(hWnd_PaoMain))
			{
				ShowWindow(hWnd_PaoMain, SW_SHOW); 
				ShowOwnedPopups(hWnd_PaoMain, TRUE);
				UpdateWindow(hWnd_PaoMain);
			}
			
			else
			{
#ifdef _MODULE_DEBUG_
				StartObjectDebug();
#endif
				wc.style         = 0;
				wc.lpfnWndProc   = PaoMainWndProc;
				wc.cbClsExtra    = 0;
				wc.cbWndExtra    = 0;
				wc.hInstance     = NULL;
				wc.hIcon         = NULL;
				wc.hCursor       = NULL;
				wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
				wc.lpszMenuName  = NULL;
				wc.lpszClassName  = "PaoWndClass";
				
				if (!RegisterClass(&wc))
					return FALSE;
				hMainMenu = LoadMenuIndirect((PMENUTEMPLATE)&PaoMainMenuTemplate);
				ModifyMenu(hMainMenu, IDM_NEWGAME, MF_BYCOMMAND, IDM_NEWGAME, IDS_NEWGAME);
				ModifyMenu(hMainMenu, IDM_SETUP, MF_BYCOMMAND, IDM_SETUP, IDS_SETUP);
				ModifyMenu(hMainMenu, IDM_HEROBOARD,MF_BYCOMMAND,  IDM_HEROBOARD, IDS_SCORE);
				ModifyMenu(hMainMenu, IDM_GAMESHOW, MF_BYCOMMAND, IDM_GAMESHOW, IDS_GAMESHOW);
				hWnd_PaoMain = CreateWindow(
					"PaoWndClass", 
					IDS_PAO, 
					WS_VISIBLE|PWS_STATICBAR|WS_CAPTION,//|WS_BORDER,
					PLX_WIN_POSITION,
					/*
					WND_X, 
					WND_Y,//TOPSPACE, 
					WND_WIDTH, 
					WND_HEIGHT,// - TOPSPACE, 
					*/
					NULL, 
					hMainMenu,
					NULL, 
					NULL
					);
				if (!hWnd_PaoMain)
					return (FALSE);
				
				CreateCapButton(hWnd_PaoMain, IDM_CONFIRM, 1, IDS_NEWGAME);
				CreateCapButton(hWnd_PaoMain, IDM_EXIT, 0, IDS_EXIT);
				SendMessage(hWnd_PaoMain, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
				
				
				ShowWindow(hWnd_PaoMain, SW_SHOW);
				UpdateWindow(hWnd_PaoMain);
			}
			
			return dwRet;
			
			break;
			
		case APP_INACTIVE :
			
			ShowOwnedPopups(hWnd_PaoMain, FALSE);
			ShowWindow(hWnd_PaoMain, SW_HIDE);
			break;
    }
	
    return dwRet;
}

/*****************************************************************************\
* Funcation: PaoMainWndProc
*
* Purpose  : 主窗口窗口过程处理函数
*
* Explain  : 主窗口显示菜单        
*           
\*****************************************************************************/
LRESULT PaoMainWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam)
{
	//------------------------------------------------------------------------
	LRESULT			lResult;
	HDC				hdc;
	//--------------------------------------------------------------------------
	
	switch (wMsgCmd)	
    {
	case WM_CREATE:
		{
			COLORREF	clr;
			
#if defined(SCP_SMARTPHONE)
			SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
#endif
			hdc = GetDC(hWnd);
			hCoverBg = CreateBitmapFromImageFile(hdc, COVERBG, &clr, &bclarity);
			//hCovSel	 = CreateBitmapFromImageFile(hdc, PAOCOVSEL, &clr, &bclarity);
			nSelMenuItem = 0;
			ShowArrowState(hWnd,nSelMenuItem);
			InitSelItem();
			/*
			strcpy(MenuItemText[0],IDS_NEWGAME);
			strcpy(MenuItemText[1],IDS_SETUP);
			strcpy(MenuItemText[2],IDS_HEROBOARD);
			strcpy(MenuItemText[3],IDS_GAMESHOW);
			*/
			//SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLMIDDLE, MASKMIDDLE);
			
			if (!ReadPaoSetup())
			{
				if (!bFirst)
				{
					bFirst = FALSE;
					//	MessageBox(hWnd,IDS_READ_SETUP_ERROR,IDS_PROMPT,MB_OK);
				}
				bSetupPress = FALSE;		
				//				bSetupMove = FALSE;	
			}
			
			if (!ReadPaoHero())
			{
				if (!bFirst)
				{
					bFirst = FALSE;
					//	MessageBox(hWnd,IDS_READ_HERO_ERROR,IDS_PROMPT,MB_OK);
				}
				InitPaoHero();
			}
			ReleaseDC(hWnd,hdc);
		}
		
		break;
		
	case WM_ERASEBKGND:
		break;
		
	case WM_COMMAND:
		
		switch (LOWORD(wParam))
		{
		case IDM_CONFIRM:
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
				PostMessage(hWnd,WM_COMMAND,IDM_GAMESHOW,NULL);
				break;
				
			default:
				nSelMenuItem = 0;
				PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
				break;
			}
			break;
			
			case IDM_NEWGAME:
				PaoGameProc(hWnd);
				break;
				
			case IDM_SETUP:
				PaoSetupProc(hWnd);
				break;
				
			case IDM_HEROBOARD:
				PaoHeroProc(hWnd);
				break;
				
			case IDM_GAMESHOW:
				PaoGameShowProc(hWnd);
				break;
				
			case IDM_EXIT:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
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
						PostMessage(hWnd,WM_COMMAND,IDM_GAMESHOW,NULL);
						break;
						
					default:
						i = 0;
						PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
						break;
					}
					nSelMenuItem = i;
					ShowArrowState(hWnd,nSelMenuItem);
					//				InvalidateRect(hWnd,NULL,TRUE);
					//				UpdateWindow(hWnd);
					{
						HDC hdc;	
						int i;
						
						hdc = GetDC(hWnd);
						
						for (i=0;i<MENU_ITEM_NUM;i++)
						{
							MyDrawMenuItem(hdc,i,nSelMenuItem);
						}
						
						ReleaseDC(hWnd,hdc);
					}
				}
			}
			break;
			
		case WM_PAINT:
			hdc = BeginPaint(hWnd,NULL);
			BitBlt(hdc, X_COVERBG,Y_COVERBG,
				COVERBG_WIDTH , COVERBG_HEIGHT , 
				(HDC)hCoverBg, 0, 1, ROP_SRC);
//			{
//				int i;
//				for (i=0;i<MENU_ITEM_NUM;i++)
//				{
//					MyDrawMenuItem(hdc,i,nSelMenuItem);
//				}
//			}
			EndPaint(hWnd,NULL);
			break;
			
#ifdef _MXLINCOMMENT_
			
		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{
				
			case VK_UP:
				
				if(nSelMenuItem <= 0)
					nSelMenuItem = MENU_ITEM_NUM;
				
				//			if (0<nSelMenuItem)
				{
					nSelMenuItem--;
					ShowArrowState(hWnd,nSelMenuItem);
					InvalidateRect(hWnd,NULL,TRUE);
					//				UpdateWindow(hWnd);
					{
						HDC hdc;	
						int i;
						
						hdc = GetDC(hWnd);
						
						for (i=0;i<MENU_ITEM_NUM;i++)
						{
							MyDrawMenuItem(hdc,i,nSelMenuItem);
						}
						
						ReleaseDC(hWnd,hdc);
					}
				}
				break;
				
			case VK_DOWN:
				
				if(nSelMenuItem >= MENU_ITEM_NUM-1)
					nSelMenuItem = -1;
				
				//			if (MENU_ITEM_NUM-1>nSelMenuItem)
				{
					nSelMenuItem++;
					ShowArrowState(hWnd,nSelMenuItem);
					InvalidateRect(hWnd,NULL,TRUE);
					//				UpdateWindow(hWnd);
					{
						HDC hdc;	
						int i;
						
						hdc = GetDC(hWnd);
						
						for (i=0;i<MENU_ITEM_NUM;i++)
						{
							MyDrawMenuItem(hdc,i,nSelMenuItem);
						}
						
						ReleaseDC(hWnd,hdc);
					}
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
			case WM_CLOSE:
				DestroyWindow(hWnd);
				break;
				
			case WM_DESTROY:
				if (NULL != hCoverBg)
				{
					DeleteObject(hCoverBg);
					hCoverBg = NULL;
				}
				//		if (hCovSel) {
				//			DeleteObject(hCovSel);
				//		}
				DestroyMenu(hMainMenu);
				hMainMenu = NULL;
				
				WritePaoSetup();
				
				UnregisterClass("PaoWndClass",NULL);
				
				DlmNotify((WPARAM)PES_APP_QUIT, (LPARAM)hPaoInstance);
				
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
* Function	   MyDrawMenuItem
* Purpose      画菜单项
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL MyDrawMenuItem(HDC hdc,int i,int nSel)
{
	
	
//	COLORREF	OldClr;
//	int			OldStyle;
	
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
	return TRUE;
	//	OldClr = SetBkColor(hdc,SEL_COLOR);
	//	OldStyle = SetBkMode(hdc, NEWTRANSPARENT);
	//
	//	BitBlt(hdc,
	//		SELRECT[i].left,
	//		SELRECT[i].top,
	//		SEL_WIDTH,
	//		SEL_HEIGHT,
	//		(HDC)hCovSel,
	//		0,0,ROP_SRC);
	//
	//	SetBkColor(hdc, OldClr);
	//	SetBkMode(hdc, OldStyle);
	
	
	/*
	COLORREF	OldClr,OldTextClr;
	int			OldStyle;
	RECT		rect;
	
	  if ((0>i)||(MENU_ITEM_NUM<=i))
	  {
	  return FALSE;
	  }
	  if ((0>nSel)||(MENU_ITEM_NUM<=nSel))
	  {
	  return FALSE;
	  }
	  
		if(i%2)
		{
		rect.left = X_FIRSTRD_MENUITEM;//偶数行菜单项左上角x的坐标
		rect.right = X_FIRSTRD_MENUITEM+X_MENUITEM_INTERVAL;//偶数行菜单项右下角x的坐标
		rect.top = Y_FIRSTLU_MENUITEM + i*Y_MENUITEM_INTERVAL-1;//偶数行菜单项左上角y的坐标
		rect.bottom = Y_FIRSTRD_MENUITEM + i*Y_MENUITEM_INTERVAL-1;//偶数行菜单项右下角y的坐标
		}
		else
		{
		rect.left = X_FIRSTLU_MENUITEM;//奇数行菜单项左上角x的坐标
		rect.right = X_FIRSTLU_MENUITEM+X_MENUITEM_INTERVAL;//奇数行菜单项右下角x的坐标
		rect.top = Y_FIRSTLU_MENUITEM + i*Y_MENUITEM_INTERVAL-1;//奇数行菜单项左上角y的坐标
		rect.bottom = Y_FIRSTRD_MENUITEM + i*Y_MENUITEM_INTERVAL-1;//奇数行菜单项右下角y的坐标
		}
		
		  OldClr = SetBkColor(hdc,COLOR_WHITE);
		  OldStyle = SetBkMode(hdc, TRANSPARENT);
		  
			if (i == nSel)
			OldTextClr = SetTextColor(hdc,RGB(254,240,192));//RGB(135,143,252)
			
			  else
			  OldTextClr = SetTextColor(hdc,RGB(251,173,63));
			  
				//	DrawText(hdc, MenuItemText[i], -1, &rect, DT_VCENTER | DT_HCENTER);
				
				  SetTextColor(hdc,OldTextClr);
				  
					SetBkColor(hdc, OldClr);
					SetBkMode(hdc, OldStyle);
					
					  return TRUE;
	*/
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
		if(i%2)
		{
			if ((X_FIRSTRD_MENUITEM)<x && x<(X_FIRSTRD_MENUITEM+X_MENUITEM_INTERVAL) && y>(Y_FIRSTLU_MENUITEM + i*Y_MENUITEM_INTERVAL) && y<(Y_FIRSTRD_MENUITEM + i*Y_MENUITEM_INTERVAL))
				return i;
		}
		else
		{
			if ((X_FIRSTLU_MENUITEM)<x && x<(X_FIRSTLU_MENUITEM+X_MENUITEM_INTERVAL) && y>(Y_FIRSTLU_MENUITEM + i*Y_MENUITEM_INTERVAL) && y<(Y_FIRSTRD_MENUITEM + i*Y_MENUITEM_INTERVAL))
				return i;
		}
	}
	return -1;
}

/*****************************************************************************\
* Funcation: PaoSetupProc
*
* Purpose  : 建立设置窗口
*
* Explain  : 注册类并显示窗口        
*           
\*****************************************************************************/
static DWORD	PaoSetupProc(HWND hWnd)
{
	WNDCLASS wc;
    DWORD dwRet;
	
    dwRet = TRUE;
	
	if(!bRSetupWnd)
	{
		wc.style         = CS_OWNDC;
		wc.lpfnWndProc   = PaoSetupWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName  = "PaoSetupWndClass";
		
		if (!RegisterClass(&wc))
		{
			bRSetupWnd = FALSE;
			return FALSE;
		}
		else
			bRSetupWnd = TRUE;
	}
	
	hWndPaoSetup = CreateWindow(
		"PaoSetupWndClass", 
		IDS_SETUP, 
		WS_VISIBLE|WS_CAPTION|PWS_STATICBAR,//|WS_BORDER,
		PLX_WIN_POSITION,
		/*
		WND_X, 
		WND_Y,//TOPSPACE, 
		WND_WIDTH, 
		WND_HEIGHT,// - TOPSPACE,
		*/
		hWnd, 
		NULL,
		NULL, 
		NULL
		);
	
	if ( hWndPaoSetup == NULL )
		return FALSE;
	
	ShowWindow(hWndPaoSetup,SW_SHOW);
	UpdateWindow(hWndPaoSetup);
	
	return dwRet;
}

/*****************************************************************************\
* Funcation: PaoSetupWndProc
*
* Purpose  : 设置窗口的过程函数
*
* Explain  : 处理设置窗口的消息
\*****************************************************************************/
LRESULT PaoSetupWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT			lResult;
	HDC		hdc;
	
	switch (wMsgCmd)
	{
	case WM_CREATE:
		
#if defined(SCP_SMARTPHONE)
		SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
#endif
		
		hWnd_SetPress = CreateWindow(
			"BUTTON",
			IDS_PRESS,
			BS_AUTOCHECKBOX|WS_VISIBLE|WS_CHILD|WS_TABSTOP,//|WS_BITMAP,//|WS_BORDER,
			X_SETPRESSBTN,
			Y_SETPRESSBTN,
			SETPRESSBTN_WIDTH,
			SETPRESSBTN_HEIGHT,
			hWnd,
			(HMENU)IDC_SETPRESS,
			NULL,
			NULL);
		if (bSetupPress)
		{
			SendMessage(hWnd_SetPress,BM_SETCHECK,BST_CHECKED,NULL);
			//		SendMessage(hWnd_SetPress, BM_SETCHECK, BST_CHECKED, NULL);
			SendMessage(hWnd_SetPress, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_DISABLE),0);
		}
		else
		{
			SendMessage(hWnd_SetPress,BM_SETCHECK,BST_UNCHECKED,NULL);
			SendMessage(hWnd_SetPress, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_PUSHDOWN),0);
		}
		/*		hWnd_SetMove = CreateWindow(
		"BUTTON",
		IDS_MOVE,
		BS_AUTOCHECKBOX|WS_VISIBLE|WS_CHILD|WS_TABSTOP,//|WS_BITMAP,//|WS_BORDER
		X_SETMOVEBTN,
		Y_SETMOVEBTN,
		SETMOVEBTN_WIDTH,
		SETMOVEBTN_HEIGHT,
		hWnd,
		(HMENU)IDC_SETMOVE,
		NULL,
		NULL);*/
		/*		if (bSetupMove)
		SendMessage(hWnd_SetMove,BM_SETCHECK,BST_CHECKED,NULL);
		else*/
		//		SendMessage(hWnd_SetMove,BM_SETCHECK,BST_UNCHECKED,NULL);
		
		SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLUP|SCROLLDOWN, MASKUP|MASKDOWN);
		CreateCapButton(hWnd, IDM_EXIT, 0, IDS_BACK);
		CreateCapButton(hWnd, IDM_CONFIRM,  1, IDS_CONFIRM);
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		
		break;
		
	case WM_SETRBTNTEXT:
		if(strcmp((char *)lParam, IDS_EXIT) == 0)
            SendMessage (hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
        else
            SendMessage (hWnd, PWM_SETBUTTONTEXT, 0, lParam);
		break;
		
	case WM_COMMAND:
		
		switch (LOWORD(wParam))
		{
		case IDM_CONFIRM:
			if (BST_CHECKED==SendMessage(hWnd_SetPress,BM_GETCHECK,NULL,NULL))
				bSetupPress=TRUE;
			else
				bSetupPress=FALSE;
			
				/*			if (BST_CHECKED==SendMessage(hWnd_SetMove,BM_GETCHECK,NULL,NULL))
				bSetupMove=TRUE;
				else
			bSetupMove=FALSE;*/
			
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		case IDM_EXIT:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		
		break;
		
		case WM_ACTIVATE:
			if (WA_ACTIVE == wParam)
				SetFocus(hWnd_SetPress);
			break;
			
		case WM_PAINT:
			hdc=BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
			break;
			
		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{
				//#ifndef SCP_SMARTPHONE
				//		case VK_F2:
				//			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
				//            break;
				//#endif
			case VK_F5:
				if (BST_CHECKED==SendMessage(hWnd_SetPress,BM_GETCHECK,NULL,NULL))
				{
					bSetupPress=FALSE;
					SendMessage(hWnd_SetPress, BM_SETCHECK, BST_UNCHECKED, NULL);
					SendMessage(hWnd_SetPress, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_PUSHDOWN),0);
				}
				else
				{
					bSetupPress=TRUE;
					SendMessage(hWnd_SetPress, BM_SETCHECK, BST_CHECKED, NULL);
					SendMessage(hWnd_SetPress, BM_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP,IMAGE_DISABLE),0);
				}
				UpdateWindow(hWnd);
				
				
				
				
				//			PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
				/*
				case VK_F10:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
				*/
			default:
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
			break;
			
			case WM_CLOSE:
				
				DestroyWindow(hWnd);
				UnregisterClass("PaoSetupWndClass",NULL);
				bRSetupWnd = FALSE;
				
				break;	
				
			case WM_DESTROY:
				hWndPaoSetup=NULL;
				//		UnregisterClass("PaoSetupWndClass",NULL);
				break;
				
			default :
				
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				
				break;
	}
	return lResult;
}

/*****************************************************************************\
* Funcation: ReadPaoSetup
*
* Purpose  : 读设置文件
*
* Explain  : 读设置文件
\*****************************************************************************/
static	BOOL	ReadPaoSetup()
{
	int	hFile;
	
	if (-1==(hFile=(open(PAO_SETUP_FILE,O_RDONLY))))
	{
		bFirst = TRUE;
		return FALSE;
	}
	if (-1==lseek(hFile,0,SEEK_SET))
	{
		close(hFile);
		return FALSE;
	}
	if (-1==(read(hFile,&bSetupPress,sizeof(BOOL))))
	{
		close(hFile);
		return FALSE;
	}
	/*	if (-1==(read(hFile,&bSetupMove,sizeof(BOOL))))
	{
	close(hFile);
	return FALSE;
}*/
	close(hFile);
	return TRUE;
}

/*****************************************************************************\
* Funcation: WritePaoSetup
*
* Purpose  : 保存设置信息
*
* Explain  : 保存设置信息
\*****************************************************************************/
static	BOOL	WritePaoSetup()
{
	int	hFile;
	int		i;
	//	int		j;
	
	if (-1==(hFile=(open(PAO_SETUP_FILE,O_WRONLY|O_TRUNC))))
		if (-1==(hFile=open(PAO_SETUP_FILE,O_WRONLY|O_CREAT)))
			return	FALSE;
		
			/*	if (-1==write(hFile,&bSetupPress,sizeof(BOOL)))
			{
			close(hFile);
			return FALSE;
}*/
		i = (int)(write(hFile,&bSetupPress,sizeof(BOOL)));
		{
			if(i==-1)
			{
				close(hFile);
				unlink(PAO_SETUP_FILE);
				return	FALSE;
			}
			if((i>0)&&(i<sizeof(BOOL)))
			{
				close(hFile);
				unlink(PAO_SETUP_FILE);
				MsgWin(NULL, NULL, 0, IDS_FILE_PROMPT,IDS_PAO,Notify_Info, ML("OK") ,NULL,WAITTIMEOUT);
				return	FALSE;
			}
		}
		
		/*	if (-1==write(hFile,&bSetupMove,sizeof(BOOL)))
		{
		close(hFile);
		return FALSE;
}*/
/*	j = (int)(write(hFile,&bSetupMove,sizeof(BOOL)));
{
if(j==-1)
{
close(hFile);
return	FALSE;
}
if((j>0)&&(j<sizeof(BOOL)))
{
close(hFile);
DeleteFile(PAO_SETUP_FILE);
MsgWin(IDS_FILE_PROMPT,IDS_FILE_NOTICE,WAITTIMEOUT);
return	FALSE;
}
}
		*/
		close(hFile);
		return TRUE;
}

/*****************************************************************************\
* Funcation: PaoGameProc
*
* Purpose  : 进入游戏界面
*
* Explain  : 注册类并创建窗口
\*****************************************************************************/
static	DWORD	PaoGameProc(HWND hWnd)
{
	WNDCLASS wc;
    DWORD dwRet;
	
    dwRet = TRUE;
	
	if(!bRGameWnd)
	{
		wc.style         = CS_OWNDC;
		wc.lpfnWndProc   = PaoGameWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName  = "PaoGameWndClass";
		
		if (!RegisterClass(&wc))
		{
			bRGameWnd = FALSE;
			return FALSE;
		}
		else
			bRGameWnd = TRUE;
	}
	
	if (hWndPaoGame) {
		ShowWindow(hWndPaoGame, SW_SHOW);
		ShowOwnedPopups(hWndPaoGame, TRUE);
		return dwRet;
	}
	else
	{
		hWndPaoGame = CreateWindow(
			"PaoGameWndClass", 
			IDS_NEWGAME, 
			WS_VISIBLE|PWS_STATICBAR,//|WS_BORDER,//|WS_CAPTION
			PLX_WIN_POSITION,
			/*
			WND_X, 
			WND_Y,//TOPSPACE, 
			WND_WIDTH, 
			WND_HEIGHT,//TOPSPACE, 
			*/
			hWnd_PaoMain, 
			NULL,
			NULL, 
			NULL
			);
		
		if ( hWndPaoGame == NULL )
			return FALSE;
		
		//	SendMessage(hWndPaoGame, PWM_SETSCROLLSTATE, SCROLLLEFT|SCROLLRIGHT|SCROLLUP, MASKLEFT|MASKRIGHT|MASKUP);
		
		ShowWindow(hWndPaoGame,SW_SHOW);
		UpdateWindow(hWndPaoGame);
	}
	return dwRet;
}

/*****************************************************************************\
* Funcation: PaoGameWndProc
*
* Purpose  : 游戏界面窗口过程
*
* Explain  : 游戏界面窗口过程
\*****************************************************************************/
LRESULT PaoGameWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT			lResult;
	HDC		hdc;
	
	switch (wMsgCmd)
	{
	case WM_CREATE:
		{
			COLORREF	clr;
			
#if defined(SCP_SMARTPHONE)
			SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
#endif
			CreateCapButton(hWnd, IDM_EXIT, 0, IDS_BACK);
			CreateCapButton(hWnd, IDM_GAMESTART,  1, IDS_GAMESTART);
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			
			srand(GetTickCount());
			hdc = GetDC(hWnd);
			DropLine = 0;
			nScore = 0;
			hbmpBall[0]	= CreateBitmapFromImageFile(hdc, BALLPATH0, &clr1, &bclarity);
			hbmpBall[1]	= CreateBitmapFromImageFile(hdc, BALLPATH1, &clr2, &bclarity);
			hbmpBall[2]	= CreateBitmapFromImageFile(hdc, BALLPATH2, &clr2, &bclarity);
			hbmpBall[3]	= CreateBitmapFromImageFile(hdc, BALLPATH3, &clr2, &bclarity);
			hbmpBall[4]	= CreateBitmapFromImageFile(hdc, BALLPATH4, &clr2, &bclarity);
			hbmpBall[5]	= CreateBitmapFromImageFile(hdc, BALLPATH5, &clr2, &bclarity);
			hbmpBall[6]	= CreateBitmapFromImageFile(hdc, BALLPATH6, &clr2, &bclarity);
			hbmpBall[7]	= CreateBitmapFromImageFile(hdc, BALLPATH7, &clr2, &bclarity);
			hbmpBall[8]	= CreateBitmapFromImageFile(hdc, BALLPATH8, &clr2, &bclarity);
			
			hPaoBack = CreateBitmapFromImageFile(hdc, PAOBACK_FILE, &clr, &bclarity);
			hPaoGun = CreateBitmapFromImageFile(hdc, PAOGUN_FILE, &clr6, &bclarity);
			hPaoWall = CreateBitmapFromImageFile(hdc, PAOWALL, &clr, &bclarity);
			hDragen[LEFT_DRAGEN][0] = CreateBitmapFromImageFile(hdc, PAO_LDRANGE1, &clr3, &bclarity);
			hDragen[LEFT_DRAGEN][1] = CreateBitmapFromImageFile(hdc, PAO_LDRANGE2, &clr3, &bclarity);
			//			hDragen[RIGHT_DRAGEN][0] = CreateBitmapFromImageFile(hdc, PAO_RDRANGE1, &clr3, &bclarity);
			//			hDragen[RIGHT_DRAGEN][1] = CreateBitmapFromImageFile(hdc, PAO_RDRANGE2, &clr3, &bclarity);
			hRoundBack = CreateBitmapFromImageFile(hdc, PAOROUNDBACK, &clr, &bclarity);
			hRoundNum = CreateBitmapFromImageFile(hdc, PAOROUNDNUM, &clr4, &bclarity);
			//			hPaoScore = CreateBitmapFromImageFile(hdc, PAOSCORE, &clr5, &bclarity);
			
			InitDragenI();
			bGameStart = FALSE;
			bGamePause = FALSE;
			bLeftDown	= FALSE;
			bRightDown	= FALSE;
			bRoundBN = FALSE;
			bDownWallTID = FALSE;
			bMoveTID = FALSE;
			bDelTID = FALSE;
			bPressTID = FALSE;
			//			bGunTID = FALSE;
			bMoveGunTID = FALSE;
			bRoundTID = FALSE;
			IsDead = FALSE;
			//SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_GAMESTART);
			nDelBall = 0;
			nDropBall = 0;
			InitStaticBall();
			SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLMIDDLE, MASKMIDDLE);
			ReleaseDC(hWnd, hdc);
		}
		
		break;
		
	case WM_COMMAND:
		
		switch (LOWORD(wParam))
		{
		case IDM_GAMESTART:
			
			if (FALSE == bGameStart)
			{
				bGameStart = TRUE;
				bRoundBN = TRUE;
				bLeftDown = FALSE;
				bRightDown = FALSE;
				SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_PAUSE);
				ShowArrowInGameState(hWnd);
				InitPaoGame(hWnd);
				MoveBall.color = -1;
				IsDead = FALSE;
				if(!bDownWallTID)
				{
					bDownWallTID = TRUE;
					SetTimer(hWnd, DOWNWALL_TIMER_ID, Interval_Wall, NULL);
				}
				if(!bMoveGunTID)
				{
					bMoveGunTID = TRUE;
					SetTimer(hWnd, MOVE_GUN_ID, Interval_Gun,NULL);
				}
				if(!bRoundTID)
				{
					bRoundTID = TRUE;
					SetTimer(hWnd, ROUND_TIMER_ID, Interval_Round, NULL);
				}
				InvalidateRect(hWnd,NULL,TRUE);
				UpdateWindow(hWnd);
			}
			else
			{
				//EndPaoGame(hWnd);
				if (!bGamePause)	//-->暂停游戏
				{
					bGamePause = TRUE;
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_CONTINUE);
				}
				else				//-->继续游戏
				{
					bGamePause = FALSE;
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PAUSE);
				}
			}
			break;
			
		case IDM_EXIT:
			
			if(bGameStart){
				PLXConfirmWinEx(NULL, hWnd, IDS_GAME_PROMPT, Notify_Request, IDS_PAO, ML("OK"), ML("Cancel"), IDRM_RMSG);
				
				
				//MsgSelWin( hWnd, IDS_GAME_PROMPT, IDS_PAO );
			}
			else
			{
				bGameStart = FALSE;
				ShowArrowInGameState(hWnd);
				//				DestroyWindow(hWnd);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			
			break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}		
		break;
		
		case IDRM_RMSG:
			if (lParam)
				PostMessage(hWnd, WM_SELOK, 0, 0);
			break;
			
		case WM_SELOK:
			
			bGameStart = FALSE;
			ShowArrowInGameState(hWnd);
			nScore = nDelBall*SCORE_PER_BALL+nDropBall*SCORE_PER_DROP;
			if(nScore>PaoRecord[TOP_REC_NUM-1].nScore)
				SetPaoHeroProc(hWnd_PaoMain);
			//		DestroyWindow(hWndPaoGame);
			PostMessage(hWndPaoGame, WM_CLOSE, 0, 0);
			
			break;
			
		case WM_SELCANCEL:
			
			bGamePause = TRUE;
			
			break;

		case IDRM_GAMEOVER:
			PostMessage(hWnd, WM_SELOK, 0, 0);
			break;
			
			
		case WM_TIMER:
			
			switch (wParam)
			{
			case MOVE_TIMER_ID:
				
				if (!bGamePause)
				{
					BOOL bShouldMove;
					bShouldMove = TRUE;
					
					if(bMoveTID)
					{
						bMoveTID = FALSE;
						KillTimer(hWnd,MOVE_TIMER_ID);
					}
					
					if (ShouldMove())
					{
						CalcNewPos();
						InvalidateRect(hWnd,&Rect,TRUE);
						UpdateWindow(hWnd);
						bShouldMove = ShouldMove();
					}
					else 
						bShouldMove = FALSE;
					
					if (bShouldMove)
					{
						bMoveBallDraw = TRUE;
						//					CalcNewPos();
						if(!bMoveTID)
						{
							bMoveTID = TRUE;
							SetTimer(hWnd,MOVE_TIMER_ID,MOVE_TIMER_ELAPSE,NULL);
						}
						//					InvalidateRect(hWnd,&Rect,TRUE);
						//					UpdateWindow(hWnd);
					}
					else 
					{
						int x,y;
						x = MoveBall.lt.x;
						y = MoveBall.lt.y;
						
						if (-1!=StaticBall[MoveBall.i][MoveBall.j].color)
							AdjustBall();
						StaticBall[MoveBall.i][MoveBall.j].color = MoveBall.color;
						MoveBall.lt.x = StaticBall[MoveBall.i][MoveBall.j].lt.x;
						MoveBall.lt.y = StaticBall[MoveBall.i][MoveBall.j].lt.y+DropLine*Y_BALL_INTERVAL;
						bMoveBallDraw = FALSE;
						//
						if(x>MoveBall.lt.x)
						{
							Rect.left = MoveBall.lt.x;
							Rect.right = x + 2*RADIUS_BALL-1;
							Rect.top = ((y>MoveBall.lt.y)?MoveBall.lt.y:y);
							Rect.bottom = ((y>MoveBall.lt.y)?y:MoveBall.lt.y)+2*RADIUS_BALL-1;
						}
						else
						{
							Rect.left = x;
							Rect.right = MoveBall.lt.x + 2*RADIUS_BALL-1;
							Rect.top = ((y>MoveBall.lt.y)?MoveBall.lt.y:y);
							Rect.bottom = ((y>MoveBall.lt.y)?y:MoveBall.lt.y)+2*RADIUS_BALL-1;
						}
						
						InvalidateRect(hWnd,&Rect,TRUE);
						UpdateWindow(hWnd);
						
						if(!bDelTID)
						{
							bDelTID = TRUE;
							SetTimer(hWnd,DEL_TIMER_ID,DEL_TIMER_ELAPSE,NULL);
						}
					}
					//OS_UpdateScreen(0,0,0,0);
				}
				break;
				
			case DOWNWALL_TIMER_ID:
				{
					if(!bGamePause && !bRoundBN )
					{
						if(nCurFrame != 0)
						{
							bWallShouldDown = TRUE;
							break;
						}
						DropLine++;
						
						Rect.left = X_DARKBACK;
						Rect.top = Y_DARKBACK;
						Rect.right = X_DARKBACK+DARKBACK_WIDTH;
						Rect.bottom = Y_DARKBACK+DARKBACK_HEIGHT;
						
						InvalidateRect(hWnd, &Rect, FALSE);
						UpdateWindow(hWnd);
						
						if (IsDeadPao())
						{
							//	InvalidateRect(hWnd,NULL,TRUE);
							EndPaoGame(hWnd);
							Rect.left = X_GAMEBACK;
							Rect.top = Y_GAMEBACK;
							Rect.right = X_GAMEBACK + GAMEBACK_WIDTH;
							Rect.bottom = Y_GAMEBACK+GAMEBACK_HEIGHT;
							if (nScore>PaoRecord[TOP_REC_NUM-1].nScore)
							{
								SetPaoHeroProc(hWnd);
							}
							else
							{
								//PaoGameOverProc();
								PLXTipsWin(NULL, hWnd, IDRM_GAMEOVER, IDS_GAMEOVER, IDS_PAO, Notify_Info, IDS_OK, NULL,WAITTIMEOUT);
							} 
						}
					}
				}
				break;
				
			case MOVE_GUN_ID:
				
				if(bGameStart && !bGamePause && !bRoundBN)
				{
					if(bLeftDown && !bRightDown)
					{
						HDC	hdc;
						hdc = GetDC(hWnd);
						
						if (MIN_GUN_DEGREE<Gun_Degree)
							Gun_Degree -= D_INTERVAL;
						else
							Gun_Degree = MIN_GUN_DEGREE;
						DrawGun(hdc);
						ReleaseDC(hWnd,hdc);
					}
					
					if(!bLeftDown && bRightDown)
					{
						HDC	hdc;
						hdc = GetDC(hWnd);
						
						if (MAX_GUN_DEGREE>Gun_Degree)
							Gun_Degree += D_INTERVAL;
						else
							Gun_Degree = MAX_GUN_DEGREE;
						DrawGun(hdc);
						ReleaseDC(hWnd,hdc);
					}
					
					if(bLeftDown && bRightDown)
					{
						if(LastKeyDown == 'L')
						{
							HDC	hdc;
							hdc = GetDC(hWnd);
							
							if (MIN_GUN_DEGREE<Gun_Degree)
								Gun_Degree -= D_INTERVAL;
							else
								Gun_Degree = MIN_GUN_DEGREE;
							DrawGun(hdc);
							ReleaseDC(hWnd,hdc);
						}
						else if(LastKeyDown == 'R')
						{
							HDC	hdc;
							hdc = GetDC(hWnd);
							
							if (MAX_GUN_DEGREE>Gun_Degree)
								Gun_Degree += D_INTERVAL;
							else
								Gun_Degree = MAX_GUN_DEGREE;
							DrawGun(hdc);
							ReleaseDC(hWnd,hdc);
						}
						
					}
					break;
					
					if(!bLeftDown && !bRightDown)
						break;
				}
				
				break;
				
			case DEL_TIMER_ID:
				
				if(!bGamePause && !bRoundBN)
				{
					OnDelTimer(hWnd);
					OS_UpdateScreen(0,0,0,0);
				}
				
				break;
				
			case PRESS_TIMER_ID:
				
				if(!bGamePause && !bRoundBN)
				{
					if ((-1 != ToMoveBall.color)&&bGameStart)
					{
						ShootBall(hWnd);
					}
					InvalidateRect(hWnd,&Rect,TRUE);
					UpdateWindow(hWnd);
				}
				break;
				
				/*		case GUN_TIMER_ID:
				
				  if(!bGamePause && !bRoundBN)
				  {
				  OnGunMove(hWnd);
				  }
				break;*/
				
			case DRAGEN_TIMER_ID:
				
				if(!bGamePause && !bRoundBN)
				{
					//				if (!bSetupMove)
					//				{
					ChangeDragenI();
					InvalidateRect(hWnd,&Rect,TRUE);
					UpdateWindow(hWnd);
					//				}
				}
				break;
				
			case ROUND_TIMER_ID:
				
				if(!bGamePause)
				{
					if(bRoundBN)
					{	
						bRoundBN = FALSE;
						
						Rect.left = X_GAMEBACK;
						Rect.top = Y_GAMEBACK;
						Rect.right = X_GAMEBACK + GAMEBACK_WIDTH;
						Rect.bottom = Y_GAMEBACK + GAMEBACK_HEIGHT;
						
						InvalidateRect(hWnd,&Rect,TRUE);
						UpdateWindow(hWnd);
					}
					if(bRoundTID)
					{
						bRoundTID = FALSE;
						KillTimer(hWnd,ROUND_TIMER_ID);
					}
				}
				break;
		}
		break;
		
		/*	case WM_CHAR:
		if (-1==OnPaoChar(hWnd,wParam,lParam))
		return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;*/
		
	case WM_KEYDOWN:
		
		switch(LOWORD(wParam))
		{
			//#ifndef SCP_SMARTPHONE
			//		case VK_F2:
			//			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
			//            break;
			//#endif
		default:
			if (bGameStart&&!bGamePause&&!bRoundBN)
			{
				if (-1==OnPaoKeyDown(hWnd,wParam,lParam))
					return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			}
			else 
			{
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			}
			break;
		}
		break;
		
		case WM_KEYUP:
			{
				bLeftDown = FALSE;
				bRightDown = FALSE;
				
				if (bDragenTimer && !bLeftDown && !bRightDown)
				{
					bDragenTimer = FALSE;
					KillTimer(hWnd,DRAGEN_TIMER_ID);
				}
				
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			}
			
			break;
			
		case WM_ACTIVATE:
			
			if (wParam == WA_ACTIVE)
			{
				SetFocus(hWnd);
				Rect.top = 0;
				Rect.left = 0;
				Rect.right = WND_WIDTH;
				Rect.bottom = WND_HEIGHT;
			}
			
			else if (wParam == WA_INACTIVE)
			{
				if (bGameStart)
				{
					bGamePause = TRUE;
					bLeftDown = FALSE;
					bRightDown = FALSE;
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_CONTINUE);
				}
			}
			
			break;
			
		case WM_ERASEBKGND:
			break;
			
		case WM_PAINT:
			{
				hdc=BeginPaint(hWnd,NULL);
				OnPaoGamePaint(hdc);
				EndPaint(hWnd,NULL);
			}
			break;
			
		case WM_CLOSE:
			
			DestroyWindow(hWnd);
			UnregisterClass("PaoGameWndClass",NULL);
			bRGameWnd = FALSE;
			
			break;
			
		case WM_DESTROY:
			
			EndPaoGame(hWnd);
			DeleteObject(hbmpBall[0]);
			DeleteObject(hbmpBall[1]);
			DeleteObject(hbmpBall[2]);
			DeleteObject(hbmpBall[3]);
			DeleteObject(hbmpBall[4]);
			DeleteObject(hbmpBall[5]);
			DeleteObject(hbmpBall[6]);
			DeleteObject(hbmpBall[7]);
			DeleteObject(hbmpBall[8]);
			
			DeleteObject(hPaoWall);
			if (NULL != hPaoBack)
			{
				DeleteObject(hPaoBack);
				hPaoBack = NULL;
			}
			if (NULL != hPaoGun)
			{
				DeleteObject(hPaoGun);
				hPaoGun = NULL;
			}
			DeleteObject(hDragen[LEFT_DRAGEN][0]);
			DeleteObject(hDragen[LEFT_DRAGEN][1]);
			//		DeleteObject(hDragen[RIGHT_DRAGEN][0]);
			//		DeleteObject(hDragen[RIGHT_DRAGEN][1]);
			DeleteObject(hRoundBack);
			DeleteObject(hRoundNum);
			//		DeleteObject(hPaoScore);
			hWndPaoGame=NULL;
			//		UnregisterClass("PaoGameWndClass",NULL);
			break;
			
		default :
			
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			
			break;
	}
	return lResult;
}


/*****************************************************************************\
* Funcation: OnPaoGamePaint
*
* Purpose  : 游戏界面绘图过程
*
* Explain  : 游戏界面绘图过程
\*****************************************************************************/
static	void	OnPaoGamePaint(HDC hdc)
{
	COLORREF	OldClr,OldTextClr;
	int			OldStyle;
	char		cScore[6];
	//	int			i;
	RECT        rect;
	HFONT		hSmall, hLarge;
	
	//	Rectangle(hdc,GameFrameRect.left,GameFrameRect.top,
	//		GameFrameRect.right,GameFrameRect.bottom);
	
	BitBlt(
		hdc,
		X_COVERBG,							//X_GAMEBACK,
		Y_COVERBG,							//Y_GAMEBACK,
		COVERBG_WIDTH,						//GAMEBACK_WIDTH,
		COVERBG_HEIGHT,						//GAMEBACK_HEIGHT,
		(HDC)hPaoBack,
		0,0,
		ROP_SRC);
	
	OldClr = SetBkColor(hdc,clr5);
	OldStyle = SetBkMode(hdc, NEWTRANSPARENT);
	/*	
	BitBlt(
	hdc,
	Pao_SCORE_X,
	Pao_SCORE_Y,
	Pao_SCORE_WIDTH,
	Pao_SCORE_HEIGHT,
	(HDC)hPaoScore,
	0,0,
	ROP_SRC);
	*/
	SetBkColor(hdc,COLOR_WHITE);//COLOR_WHITE
	SetBkMode(hdc, TRANSPARENT);
	OldTextClr = SetTextColor(hdc,RGB(0,255,0));
	//	Rectangle(hdc,GameFrameRect.left,GameFrameRect.top,
	//		GameFrameRect.right,GameFrameRect.bottom);
	/*	if (bGameStart)
	{
	MoveToEx(hdc,GameFrameRect.left,GameFrameRect.top,NULL);
	LineTo(hdc,GameFrameRect.right,GameFrameRect.top);
	LineTo(hdc,GameFrameRect.right,GameFrameRect.bottom);
	LineTo(hdc,GameFrameRect.left,GameFrameRect.bottom);
	LineTo(hdc,GameFrameRect.left,GameFrameRect.top);
	}
	*/
	
	rect.left = X_LU_SCORENUM;
	rect.top = Y_LU_SCORENUM;
	rect.right = X_RD_SCORENUM;
	rect.bottom = Y_RD_SCORENUM;
	
	sprintf(cScore,"%d",nScore);
	GetFontHandle(&hSmall, SMALL_FONT);
	hLarge = SelectObject(hdc, hSmall);
	DrawText(hdc,cScore,-1,&rect,DT_RIGHT);
	SelectObject(hdc, hLarge);
	SetTextColor(hdc,OldTextClr);
	
	SetBkColor(hdc,clr6);
	SetBkMode(hdc, NEWTRANSPARENT);
	
	if (bGameStart)
	{
		if (-1!=nGunOffset)
		{
			HPEN hPen;
			HPEN hOldPen;
			hPen = (HPEN)CreatePen(PS_SOLID, 1, RGB(255,255,255));
			hOldPen = SelectObject(hdc,hPen);
			
			MoveTo(hdc,From.x,From.y,NULL);
			LineTo(hdc,To.x,To.y);
			SelectObject(hdc,hOldPen);
			DeleteObject(hPen);
			
			BitBlt(
				hdc,
				nGunOffset+MIN_X_LEFT_GUN-1,
				Y_GUN,
				RADIUS_WIDTH_GUN*2-1,
				RADIUS_HEIGHT_GUN*2-1,
				(HDC)hPaoGun,
				0,0,
				ROP_SRC);//ROP_SRC_AND_DST
			
			InitDragenI();
			
			if ((int)(MAX_X_GUN_OFFSET/2)<=nGunOffset)
			{
				Dragen.Dir = LEFT_DRAGEN;
			}
			else
			{
				Dragen.Dir = RIGHT_DRAGEN;
			}
			CalcDragenLT();
			SetBkColor(hdc,clr3);
			SetBkMode(hdc, NEWTRANSPARENT);
			BitBlt(
				hdc,
				Dragen.lt.x,
				Dragen.lt.y,
				DRAGEN_WIDTH,
				DRAGEN_HEIGHT,
				(HDC)hDragen[Dragen.Dir][Dragen.i],
				0,0,
				ROP_SRC);//ROP_SRC_AND_DST
		}
		
		SetBkColor(hdc,clr1);
		SetBkMode(hdc, NEWTRANSPARENT);
		
		if(ToMoveBall.color != -1)
		{
			BitBlt(
				hdc,
				ToMoveBall.lt.x+1,
				ToMoveBall.lt.y,
				RADIUS_BALL*2-1,
				RADIUS_BALL*2-1,
				(HDC)hbmpBall[0],
				(ToMoveBall.color)*(RADIUS_BALL*2-1),0,
				ROP_SRC);//ROP_SRC_AND_DST
		}
		
		if ((!bRoundBN)&&(-1!=Ball_x_Offset)&&(-1!=MoveBall.color)&&bGameStart&&(0==nCurFrame)&&bMoveBallDraw)
		{
			BitBlt(
				hdc,
				MoveBall.lt.x,
				MoveBall.lt.y,
				RADIUS_BALL*2-1,
				RADIUS_BALL*2-1,
				(HDC)hbmpBall[0],
				(MoveBall.color)*(RADIUS_BALL*2-1),0,
				ROP_SRC);//ROP_SRC_AND_DST
		}
	}
	
	DrawStaticBall(hdc);
	
	//	for(i=1; i<=DropLine;i++)
	if(DropLine > 0)
	{
		BitBlt(
			hdc,
			X_GAMEBACK,
			Y_GAMEBACK,
			GAMEBACK_WIDTH,
			Y_BALL_INTERVAL*DropLine,
			(HDC)hPaoWall,
			0,DARKBACK_HEIGHT-Y_BALL_INTERVAL*DropLine,
			ROP_SRC);
	}
	
	if(bRoundBN)
	{
		BitBlt(
			hdc,
			X_ROUNDBACK,
			Y_ROUNDBACK,
			ROUNDBACK_WIDTH,
			ROUNDBACK_HEIGHT,
			(HDC)hRoundBack,
			0,0,
			ROP_SRC);
		
		SetBkColor(hdc,clr4);
		SetBkMode(hdc, NEWTRANSPARENT);
		
		ShowRoundNum(hdc);
	}
	
	SetBkColor(hdc, OldClr);
    SetBkMode(hdc, OldStyle);
	
//	if(IsDead)
//	{
//		DrawImageFromFile(hdc, PAOGAMEOVER, X_GAMEOVER, Y_GAMEOVER, ROP_SRC);
//	}
	
	
	return ;
}


/*****************************************************************************\
* Funcation: ShowArrowInGameState
*
* Purpose  : 在游戏界面中正确显示箭头
*
* Explain  : 在游戏界面中正确显示箭头
\*****************************************************************************/
static	void	ShowArrowInGameState(HWND hWnd)
{
	if(bGameStart)
	{
		SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLLEFT|SCROLLRIGHT, MASKLEFT|MASKRIGHT);
		SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLMIDDLE, MASKMIDDLE);
	}
	else
	{
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKALL);
	}
}


/*****************************************************************************\
* Funcation: ShowRoundNum
*
* Purpose  : 显示当前关数
*
* Explain  : 显示当前关数
\*****************************************************************************/
static	void	ShowRoundNum(HDC hdc)
{
	int Num_x1;
	int Num_x2;
	Num_x1 = (int)nCurMap/10;
	Num_x2 = nCurMap%10;
	
	Num_x1 = 13*Num_x1;
	Num_x2 = 13*Num_x2;
	
	BitBlt(
		hdc,
		X_ROUNDNUM_FIRST,
		Y_ROUNDNUM,
		ROUNDNUM_WIDTH,
		ROUNDNUM_HEIGHT,
		(HDC)hRoundNum,
		Num_x1,0,
		ROP_SRC);
	
	BitBlt(
		hdc,
		X_ROUNDNUM_SECOND,
		Y_ROUNDNUM,
		ROUNDNUM_HEIGHT,
		ROUNDNUM_HEIGHT,
		(HDC)hRoundNum,
		Num_x2,0,
		ROP_SRC);
}


/*****************************************************************************\
* Funcation: InitPaoGame
*
* Purpose  : 初始化游戏界面
*
* Explain  : 初始化游戏界面
\*****************************************************************************/
static	void	InitPaoGame(HWND hWnd)
{
	nGunOffset = (int)(MAX_X_GUN_OFFSET/2);
	nScore = 0;
	nDelBall = 0;
	nDropBall = 0;
	nCurFrame = 0;
	DropLine = 0;
	//	nSecFrame = 0;
	Ball_x_Offset = nGunOffset+(RADIUS_WIDTH_GUN);
	Ball_y_Offset = MAX_Y_BALL_OFFSET;
	
	InitBall(hWnd);
	Rect.top = 0;
	Rect.left = 0;
	Rect.right = WND_WIDTH;
	Rect.bottom = WND_HEIGHT;
	Gun_Degree = NORMAL_DEGREE;
	
	From.x = nGunOffset+MIN_X_LEFT_GUN+RADIUS_WIDTH_GUN-1;
	From.y = Y_GUN+RADIUS_HEIGHT_GUN-1;
	To.x = DTOI(From.x-(GUN_LENTH+RADIUS_HEIGHT_GUN)*cos(DTOR(Gun_Degree)));
	To.y = DTOI(From.y-(GUN_LENTH+RADIUS_HEIGHT_GUN)*sin(DTOR(Gun_Degree)));
	From.x = DTOI(From.x-RADIUS_HEIGHT_GUN*cos(DTOR(Gun_Degree)));
	From.y = DTOI(From.y-RADIUS_HEIGHT_GUN*sin(DTOR(Gun_Degree)));
	
	Move_Degree = NORMAL_DEGREE;
	DeadLine = 	NORMAL_DEADLINE;
	GameFrameRect.top = Y_GAMEBACK-1+Y_BALL_INTERVAL*(NORMAL_DEADLINE-DeadLine);
	/*if (ReadMapNum())
	{
	nCurMap = 0;
	if (ReadMap(nCurMap))
	{
	nCurMap = 0;
	}
	else
	{
	DeleteFile(PAO_MAP_FILE);
	nCurMap = -1;
	nMapNum = 0;
	InitStaticBall();
	}
	}
	else
	{
	DeleteFile(PAO_MAP_FILE);
	nCurMap = -1;
	nMapNum = 0;
	InitStaticBall();
}*/
	nCurMap = 1;
	if (!ReadMapNew(nCurMap))
	{
		InitStaticBall();
	}
	
	/*	if (bSetupMove)
	{
	if(!bGunTID)
	{
	bGunTID = TRUE;
	SetTimer(hWnd,GUN_TIMER_ID,GUN_TIMER_ELAPSE,NULL);
	}
}*/
	InitDragenI();
	if(bDragenTimer)
	{
		bDragenTimer = FALSE;
		KillTimer(hWnd,DRAGEN_TIMER_ID);
	}
	return ;
}

/*****************************************************************************\
* Funcation: InitBall
*
* Purpose  : 初始化球
*
* Explain  : 初始化球
\*****************************************************************************/
static	void	InitBall(HWND hWnd)
{
	Ball_x_Offset = nGunOffset+(RADIUS_WIDTH_GUN);
	Ball_y_Offset = MAX_Y_BALL_OFFSET;
	nCurFrame = 0;
	
	ToMoveBall.i = -1;
	ToMoveBall.j = -1;
	ToMoveBall.color = GetColor();
	ToMoveBall.lt.x = X_GAMEBACK+Ball_x_Offset;
	ToMoveBall.lt.y = Y_GAMEBACK+Ball_y_Offset;
	
	if (bSetupPress)
		if(!bPressTID)
		{
			bPressTID = TRUE;
			SetTimer(hWnd,PRESS_TIMER_ID,PRESS_TIMER_ELAPSE,NULL);
		}
		
		return ;
}

/*****************************************************************************\
* Funcation: GetColor
*
* Purpose  : 得球颜色
*
* Explain  : 得球颜色
\*****************************************************************************/
static	int		GetColor()
{
	static int	nColor;
	nColor = rand()%Pao_Ball_Num;
	return (nColor);
}

/*****************************************************************************\
* Funcation: OnPaoKeyDown
*
* Purpose  : 处理游戏的KEYDOWN消息
*
* Explain  : 处理游戏的KEYDOWN消息
\*****************************************************************************/
static	int		OnPaoKeyDown(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
	RECT	rcGun;
	SetRect(&rcGun,nGunOffset+MIN_X_LEFT_GUN-1,
		Y_GUN,
		nGunOffset+MIN_X_LEFT_GUN-1 +RADIUS_WIDTH_GUN*2-1,
		Y_GUN+RADIUS_HEIGHT_GUN*2-1);
	switch(LOWORD(wParam))
	{
	case VK_LEFT:
		if (!bLeftDown)
		{
			HDC	hdc;
			hdc = GetDC(hWnd);
			
			bLeftDown = TRUE;
			LastKeyDown = 'L';
			if (!bRightDown)
			{
				if (MIN_GUN_DEGREE<Gun_Degree)
					Gun_Degree -= D_INTERVAL;
				else
					Gun_Degree = MIN_GUN_DEGREE;
				
				DrawGun(hdc);
			}
			ReleaseDC(hWnd,hdc);
			
		}
		else
		{
			if (!bDragenTimer)
			{
				bDragenTimer = TRUE;
				SetTimer(hWnd,DRAGEN_TIMER_ID,DRAGEN_TIMER_ELAPSE,NULL);
			}
		}
		//InvalidateRect(hWnd,&rcGun,TRUE);
		//UpdateWindow(hWnd);
		break;
		
	case VK_RIGHT:
		
		if (!bRightDown)
		{
			HDC	hdc;
			hdc = GetDC(hWnd);
			
			bRightDown = TRUE;
			LastKeyDown = 'R';
			
			if (!bLeftDown)
			{
				if (MAX_GUN_DEGREE>Gun_Degree)
					Gun_Degree += D_INTERVAL;
				else
					Gun_Degree = MAX_GUN_DEGREE;
				
				DrawGun(hdc);
			}
			
			ReleaseDC(hWnd,hdc);
		}
		else
		{
			if (!bDragenTimer)
			{
				bDragenTimer = TRUE;
				SetTimer(hWnd,DRAGEN_TIMER_ID,DRAGEN_TIMER_ELAPSE,NULL);
			}
		}
		//InvalidateRect(hWnd,&rcGun,TRUE);
		//UpdateWindow(hWnd);
		break;
		
		//	case VK_UP:
	case VK_F5:
		if ((-1 != ToMoveBall.color)&&bGameStart)
		{
			ShootBall(hWnd);
			
			Rect.left = X_GAMEBACK;
			Rect.top = Y_GAMEBACK+Y_BALL_INTERVAL*NORMAL_DEADLINE+8;
			Rect.right = X_GAMEBACK+GAMEBACK_WIDTH;
			Rect.bottom = Y_GAMEBACK+GAMEBACK_HEIGHT;
			
			InvalidateRect(hWnd,&Rect,TRUE);
			UpdateWindow(hWnd);
		}
		break;
	default:
		return	-1;
		break;
	}
	//	InvalidateRect(hWnd,NULL,TRUE);
	return	1;
}

/*****************************************************************************\
* Funcation: DrawGun
*
* Purpose  : 画枪口
*
* Explain  : 画枪口
\*****************************************************************************/
static	void	DrawGun(HDC hdc)
{
	HPEN	hPen,hOldPen;
	
	hPen = (HPEN)CreatePen(PS_SOLID, 1, RGB(107,9,9));
	hOldPen = SelectObject(hdc,hPen);
	
	MoveTo(hdc,From.x,From.y,NULL);
	LineTo(hdc,To.x,To.y);
	
	SelectObject(hdc,hOldPen);
	DeleteObject(hPen);
	
	From.x = nGunOffset+MIN_X_LEFT_GUN+RADIUS_WIDTH_GUN-1;
	From.y = Y_GUN+RADIUS_HEIGHT_GUN-1;
	To.x = DTOI(From.x-(GUN_LENTH+RADIUS_HEIGHT_GUN)*cos(DTOR(Gun_Degree)));
	To.y = DTOI(From.y-(GUN_LENTH+RADIUS_HEIGHT_GUN)*sin(DTOR(Gun_Degree)));
	From.x = DTOI(From.x-RADIUS_HEIGHT_GUN*cos(DTOR(Gun_Degree)));
	From.y = DTOI(From.y-RADIUS_HEIGHT_GUN*sin(DTOR(Gun_Degree)));
	
	hPen = (HPEN)CreatePen(PS_SOLID, 1, RGB(255,255,255));
	hOldPen = SelectObject(hdc,hPen);
	
	MoveTo(hdc,From.x,From.y,NULL);
	LineTo(hdc,To.x,To.y);
	SelectObject(hdc,hOldPen);
	DeleteObject(hPen);
	
	return;
}

/*****************************************************************************\
* Funcation: DTOR
*
* Purpose  : 角度变弧度
*
* Explain  : 角度变弧度
\*****************************************************************************/
static	double DTOR(int Degree)
{
	return ((double)Degree*PI/180);
}

/*****************************************************************************\
* Funcation: DTOI
*
* Purpose  : 四舍五入
*
* Explain  : 四舍五入
\*****************************************************************************/
static	int		DTOI(double	a)
{
	double	temp;
	temp = a-(int)a;
	if (temp<0.5)
		return	(int)a;
	else
		return	((int)a+1);
	return	(int)a;
}

/*****************************************************************************\
* Funcation: InitStaticBall
*
* Purpose  : 初始化静态球
*
* Explain  : 初始化静态球
\*****************************************************************************/
static	void InitStaticBall()
{
	int	i,j;
	POINT	first;
	
	bWallShouldDown = FALSE;
	for (i=0;i<=NORMAL_DEADLINE;i++)
	{
		if (i%2)
		{
			first.x = RADIUS_BALL+X_GAMEBACK;
		}
		else
		{
			first.x = X_GAMEBACK;
		}
		first.y = i*Y_BALL_INTERVAL+Y_GAMEBACK;
		
		for (j=0;j<BALL_PER_LINE;j++)
		{
			StaticBall[i][j].color = -1;
			StaticBall[i][j].i = i;
			StaticBall[i][j].j = j;
			StaticBall[i][j].lt.x = first.x+j*X_BALL_INTERVAL;
			StaticBall[i][j].lt.y = first.y;
		}
	}
	return	;
}

/*****************************************************************************\
* Funcation: DrawStaticBall
*
* Purpose  : 画静态球
*
* Explain  : 画静态球
\*****************************************************************************/
static	void	DrawStaticBall(HDC hdc)
{
	int	i,j;
	
	int xmin, xmax;
	int ymin, ymax;
	xmin = 0;
	xmax = BALL_PER_LINE;
	ymin = 0;
	ymax = DeadLine-DropLine;
	
	ymin = (int)((Rect.top-Y_GAMEBACK-DropLine*Y_BALL_INTERVAL)/Y_BALL_INTERVAL)-1;
	ymax = (int)((Rect.bottom-Y_GAMEBACK-DropLine*Y_BALL_INTERVAL+Y_BALL_INTERVAL/2)/Y_BALL_INTERVAL)+1;
	xmin = (int)((Rect.left-X_GAMEBACK)/(2*RADIUS_BALL-1))-1;
	xmax = (int)((Rect.right-X_GAMEBACK+RADIUS_BALL)/(2*RADIUS_BALL-1))+1;
	
	xmin = (xmin<0)?0:xmin;
	xmax = (xmax>BALL_PER_LINE)?BALL_PER_LINE:xmax;
	ymin = (ymin<0)?0:ymin;
	ymax = (ymax>(DeadLine-DropLine))?(DeadLine-DropLine):ymax;
	
	for (i=ymin;i<=ymax;i++)
	{
		for (j=xmin;j<xmax;j++)
		{
			if (-1!=StaticBall[i][j].color)
			{	
				if(((StaticBall[i][j].color) >= 0) && ((StaticBall[i][j].color) <= 3))
				{
					SetBkColor(hdc,clr1);
					SetBkMode(hdc, NEWTRANSPARENT);
					
					BitBlt(
						hdc,
						StaticBall[i][j].lt.x,
						StaticBall[i][j].lt.y+DropLine*Y_BALL_INTERVAL,
						RADIUS_BALL*2-1,
						RADIUS_BALL*2-1,
						(HDC)hbmpBall[0],
						(StaticBall[i][j].color)*(RADIUS_BALL*2-1),0,
						ROP_SRC);//ROP_SRC_AND_DST
				}
				else
				{
					SetBkColor(hdc,clr2);
					SetBkMode(hdc, NEWTRANSPARENT);
					
					BitBlt(
						hdc,
						StaticBall[i][j].lt.x,
						StaticBall[i][j].lt.y+DropLine*Y_BALL_INTERVAL,
						RADIUS_BALL*2-1,
						RADIUS_BALL*2-1,
						(HDC)hbmpBall[StaticBall[i][j].color-3],
						0,0,
						ROP_SRC);
				}
			}
		}
	}
	return ;
}

/*****************************************************************************\
* Funcation: ShootBall
*
* Purpose  : 发射
*
* Explain  : 发射
\*****************************************************************************/
static	void ShootBall(HWND	hWnd)
{
	if (-1 != MoveBall.color)
		return;
	
	if(bPressTID)
	{
		bPressTID = FALSE;
		KillTimer(hWnd,PRESS_TIMER_ID);
	}
	MoveBall.i = -2;
	Move_Degree = Gun_Degree;
	Plus_x = 0;
	Plus_y = 0;
	MoveBall = ToMoveBall;
	InitBall(hWnd);
	CalcNewPos();
	if(!bMoveTID)
	{
		bMoveTID = TRUE;
		SetTimer(hWnd,MOVE_TIMER_ID,MOVE_TIMER_ELAPSE,NULL);
	}
	return;
}

/*****************************************************************************\
* Funcation: CalcNewPos
*
* Purpose  : 计算新位置
*
* Explain  : 计算新位置
\*****************************************************************************/
static void CalcNewPos()
{
	POINT	delta;
	
	if ((0 >= Ball_x_Offset)||(MAX_X_BALL_OFFSET <= Ball_x_Offset))
	{
		Move_Degree = 180-Move_Degree;
		Plus_x = -Plus_x;
	}
	
	delta.x = DTOI(BALL_STEP*cos(DTOR(Move_Degree)));
	Plus_x	+= BALL_STEP*cos(DTOR(Move_Degree))-delta.x;
	delta.y = DTOI(BALL_STEP*sin(DTOR(Move_Degree)));
	Plus_y	+= BALL_STEP*sin(DTOR(Move_Degree))-delta.y;
	
	if (1<=Plus_x)
	{
		Plus_x -= 1;
		delta.x += 1;
	}
	else if (-1>=Plus_x)
	{
		Plus_x += 1;
		delta.x -= 1;
	}
	
	if (1<=Plus_y)
	{
		Plus_y -= 1;
		delta.y += 1;
	}
	else if (-1>=Plus_y)
	{
		Plus_y += 1;
		delta.y -= 1;
	}
	
	/*	if (0>(Ball_y_Offset+delta.y))
	{
	delta.x = DTOI(delta.x*(double)(0-Ball_y_Offset)/delta.y);
	Ball_x_Offset += delta.x;
	Ball_y_Offset = 0;
}*/
	
	if (0>(Ball_x_Offset+delta.x))
	{
		delta.y = DTOI(delta.y*(double)(0-Ball_x_Offset)/delta.x);
		Ball_x_Offset = 0;
		Ball_y_Offset += delta.y;
	}
	else if (MAX_X_BALL_OFFSET<(Ball_x_Offset+delta.x))
	{
		delta.y = DTOI(delta.y*(double)(MAX_X_BALL_OFFSET-Ball_x_Offset)/delta.x);
		Ball_x_Offset = MAX_X_BALL_OFFSET;
		Ball_y_Offset += delta.y;
	}
	else
	{
		Ball_x_Offset += delta.x;
		Ball_y_Offset += delta.y;
	}
	
	GetPosFromOffset(delta.x);
	
	return;
}

/*****************************************************************************\
* Funcation: GetPosFromOffset
*
* Purpose  : 从偏移量得位置
*
* Explain  : 从偏移量得位置
\*****************************************************************************/
static void GetPosFromOffset(int k)
{
/*	MoveBall.lt.x = X_GAMEBACK+Ball_x_Offset;
MoveBall.lt.y = Y_GAMEBACK+Ball_y_Offset;
	return ;*/
	int		i;
	int		j;
	
	i = MoveBall.lt.x;
	j = MoveBall.lt.y;
	
	MoveBall.lt.x = X_GAMEBACK+Ball_x_Offset;
	MoveBall.lt.y = Y_GAMEBACK+Ball_y_Offset;
	
	if(k<0)//左上
	{
		if((j+(RADIUS_BALL*2-1))<Y_GAMEBACK+Y_BALL_INTERVAL*NORMAL_DEADLINE)
		{
			Rect.left = MoveBall.lt.x;
			Rect.top = MoveBall.lt.y;
			Rect.right = i+RADIUS_BALL*2;
			Rect.bottom = j+(RADIUS_BALL*2-1);
		}
		else
		{
			Rect.left = MoveBall.lt.x-RADIUS_BALL;
			Rect.top = MoveBall.lt.y-RADIUS_BALL;
			Rect.right = i+RADIUS_BALL*2;
			Rect.bottom = j+(RADIUS_BALL*2-1);	
		}
	}
	else//右上
	{
		if((j+(RADIUS_BALL*2-1))<Y_GAMEBACK+Y_BALL_INTERVAL*NORMAL_DEADLINE)
		{
			Rect.left = i;
			Rect.top = MoveBall.lt.y;
			Rect.right = MoveBall.lt.x+RADIUS_BALL*2;
			Rect.bottom = j+RADIUS_BALL*2-1;
		}
		else
		{
			Rect.left = i;
			Rect.top = MoveBall.lt.y-RADIUS_BALL;
			Rect.right = MoveBall.lt.x+RADIUS_BALL*2+RADIUS_BALL;
			Rect.bottom = j+RADIUS_BALL*2-1;
		}
	}
	return ;
}

/*****************************************************************************\
* Funcation: ShouldMove
*
* Purpose  : 是否应该继续移动
*
* Explain  : 是否应该继续移动
\*****************************************************************************/
static BOOL ShouldMove()
{
	int i,j;
	int SP = -1;		//平方和
	SP = -1;
	if (Y_DEADLINE>MoveBall.lt.y)
	{
		for (i=0;i<=DeadLine-DropLine;i++)
		{
			for (j=0;j<BALL_PER_LINE;j++)
			{
				if (-1 == SP)
				{
					SP = SquarePlus(i,j);
					MoveBall.i = i;
					MoveBall.j = j;
				}
				else if (SquarePlus(i,j)<SP)
				{
					SP = SquarePlus(i,j);
					MoveBall.i = i;
					MoveBall.j = j;
				}
			}
		}
		
		if (0==MoveBall.i)
		{
			return FALSE;
		}
		else if (NeighborBall(MoveBall.i,MoveBall.j))
		{
			return	FALSE;
		}
	}
	
	return	TRUE;
	/*
	if (Y_DEADLINE>MoveBall.lt.y)
	{
	MoveBall.i = (int)((MoveBall.lt.y-Y_GAMEBACK-DropLine*Y_BALL_INTERVAL+Y_BALL_INTERVAL/2)/Y_BALL_INTERVAL);
	
	  if(MoveBall.i%2)
	  MoveBall.j = (int)((MoveBall.lt.x-X_GAMEBACK)/(2*RADIUS_BALL-1));
	  else
	  MoveBall.j = (int)((MoveBall.lt.x-X_GAMEBACK+RADIUS_BALL)/(2*RADIUS_BALL-1));
	  
		if(MoveBall.j<0)
		MoveBall.j = 0;
		else if(MoveBall.j>BALL_PER_LINE-1)
		MoveBall.j = BALL_PER_LINE-1;
		if (0==MoveBall.i)
		{
		return FALSE;
		}
		else if (NeighborBall(MoveBall.i,MoveBall.j))
		{
		return	FALSE;
		}
		}
		
	return	TRUE;*/
}

/*****************************************************************************\
* Funcation: SquarePlus
*
* Purpose  : 求平方和
*
* Explain  : 求平方和
\*****************************************************************************/
static int SquarePlus(int i,int j)
{
	return (Square(StaticBall[i][j].lt.x-MoveBall.lt.x)
		+Square(StaticBall[i][j].lt.y+DropLine*Y_BALL_INTERVAL-MoveBall.lt.y));
}

/*****************************************************************************\
* Funcation: Square
*
* Purpose  : 求平方
*
* Explain  : 求平方
\*****************************************************************************/
static int Square(int a)
{
	return	(a*a);
}

/*****************************************************************************\
* Funcation: NeighborBall
*
* Purpose  : 判断是否与球相临
*
* Explain  : 判断是否与球相临
\*****************************************************************************/
static BOOL NeighborBall(int i,int j)
{
	if (i%2)
	{
		if (IsBall(i,j-1)
			||IsBall(i,j+1)
			||IsBall(i-1,j)
			||IsBall(i+1,j)
			||IsBall(i-1,j+1)
			||IsBall(i+1,j+1)
			||IsBall(i,j))
			return TRUE;
	}
	else
	{
		if (IsBall(i,j-1)
			||IsBall(i,j+1)
			||IsBall(i-1,j)
			||IsBall(i+1,j)
			||IsBall(i-1,j-1)
			||IsBall(i+1,j-1)
			||IsBall(i,j))
			return TRUE;
	}
	return FALSE;
}

/*****************************************************************************\
* Funcation: IsBall
*
* Purpose  : 判断是否是球
*
* Explain  : 判断是否是球
\*****************************************************************************/
static BOOL IsBall(int i,int j)
{
	if ((0<=i)&&(DeadLine>=(i+DropLine))
		&&(0<=j)&&(BALL_PER_LINE>j))
	{
		if (-1!=StaticBall[i][j].color)
		{
			if (Square(RADIUS_BALL*2)>SquarePlus(i,j))
				return TRUE;
		}
	}
	return FALSE;
}

/*****************************************************************************\
* Funcation: IsABall
*
* Purpose  : 判断是否是球
*
* Explain  : 判断是否是球
\*****************************************************************************/
static BOOL IsABall(int i,int j)
{
	if ((0<=i)&&(DeadLine>=(i+DropLine))
		&&(0<=j)&&(BALL_PER_LINE>j))
	{
		if (-1!=StaticBall[i][j].color)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*****************************************************************************\
* Funcation: CanDropPao
*
* Purpose  : 判断是否掉下来
*
* Explain  : 判断是否掉下来
\*****************************************************************************/
static BOOL CanDropPao(int i,int j)
{
	int m,n;
	
	if (0>i||DeadLine<i||0>j||BALL_PER_LINE<j)
		return	FALSE;
	
	bChecked[i][j] = TRUE;
	if (0 == i)
		return FALSE;
	
	if (i%2)
	{
		for (m=i-1;m<=i+1;m++)
		{
			for (n=j;n<=j+1;n++)
			{
				if (IsABall(m,n)&&(!bChecked[m][n]))
				{
					if (!CanDropPao(m,n))
						return	FALSE;
				}
			}
		}
		if (IsABall(i,j-1)&&(!bChecked[i][j-1]))
		{
			if (!CanDropPao(i,j-1))
				return	FALSE;
		}
	}
	else
	{
		for (m=i-1;m<=i+1;m++)
		{
			for (n=j-1;n<=j;n++)
			{
				if (IsABall(m,n)&&(!bChecked[m][n]))
				{
					if (!CanDropPao(m,n))
						return	FALSE;
				}
			}
		}
		if (IsABall(i,j+1)&&(!bChecked[i][j+1]))
		{
			if (!CanDropPao(i,j+1))
				return	FALSE;
		}
	}
	
	return TRUE;
}

/*****************************************************************************\
* Funcation: DropPao
*
* Purpose  : 球掉下来
*
* Explain  : 球掉下来
\*****************************************************************************/
static void DropPao(int i,int j)
{
	//	int m,n;
	bChecked[i][j] = TRUE;
	if (nCurFrame%DEL_FRAME_NUM)
	{
		StaticBall[i][j].color = (StaticBall[i][j].color%Pao_Ball_Num)+(nCurFrame%DEL_FRAME_NUM)*Pao_Ball_Num;
	}
	else
	{
		StaticBall[i][j].color = -1;
		nDropBall++;
	}
	
	Rect.left = StaticBall[i][j].lt.x;
	Rect.top = StaticBall[i][j].lt.y+DropLine*Y_BALL_INTERVAL;
	Rect.right = StaticBall[i][j].lt.x + 2*RADIUS_BALL-1;
	Rect.bottom = StaticBall[i][j].lt.y +DropLine*Y_BALL_INTERVAL + 2*RADIUS_BALL-1;
	InvalidateRect(hWndPaoGame,&Rect,TRUE);
	UpdateWindow(hWndPaoGame);
	
	
	/*	if (i%2)
	{
	for (m=i-1;m<=i+1;m++)
	{
	for (n=j;n<=j+1;n++)
	{
				if (IsABall(m,n)&&(!bChecked[m][n]))
				DropPao(m,n);
				
				  }
				  }
				  if (IsABall(i,j-1)&&(!bChecked[i][j-1]))
				  DropPao(i,j-1);
				  }
				  else
				  {
				  for (m=i-1;m<=i+1;m++)
				  {
				  for (n=j-1;n<=j;n++)
				  {
				  if (IsABall(m,n)&&(!bChecked[m][n]))
				  DropPao(m,n);
				  }
				  }
				  if (IsABall(i,j+1)&&(!bChecked[i][j+1]))
				  DropPao(i,j+1);
}*/
	
	return;
}

/*****************************************************************************\
* Funcation: IsDeadPao
*
* Purpose  : 判断是否死了
*
* Explain  : 判断是否死了
\*****************************************************************************/
static	BOOL	IsDeadPao()
{
	int j;
	for (j=0;j<BALL_PER_LINE;j++)
	{
		if (-1!=StaticBall[DeadLine-DropLine][j].color)
		{
			IsDead = TRUE;
			return	TRUE;
		}
	}
	return	FALSE;
}

/*****************************************************************************\
* Funcation: SetPaoHeroProc
*
* Purpose  : 显示英雄留名界面
*
* Explain  : 注册类并创建窗口
\*****************************************************************************/
static	DWORD	SetPaoHeroProc(HWND hWnd)
{
    DWORD dwRet;
	WNDCLASS	wc;
	static HWND	hGameSetHero;
	
    dwRet = TRUE;
	
	if(!bRSetHeroWnd)
	{
		wc.style         = CS_OWNDC;
		wc.lpfnWndProc   = PaoSetHeroWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName  = "PaoSetHeroWndClass";		
		
		if (!RegisterClass(&wc))
		{
			bRSetHeroWnd = FALSE;
			return FALSE;
		}
		else
			bRSetHeroWnd = TRUE;
	}
	
    hGameSetHero = CreateWindow(
        "PaoSetHeroWndClass", 
        IDS_SETNAME, 
        WS_VISIBLE|WS_POPUP|WS_CAPTION|PWS_STATICBAR,//|WS_BORDER
        PLX_WIN_POSITION,
		/*
		WND_X,
        WND_Y,//TOPSPACE,
        WND_WIDTH,
        WND_HEIGHT,//-TOPSPACE, 
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

/*****************************************************************************\
* Funcation: PaoSetHeroWndProc
*
* Purpose  : 英雄留名界面过程函数
*
* Explain  : 英雄留名界面过程函数
\*****************************************************************************/
LRESULT PaoSetHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	static HDC hdc;
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
		
		//创建“姓名”编辑框控件
		
		hPaoHeroName= CreateWindow(
			"IMEEDIT", 
			IDS_ANYMOUS, 
			WS_VISIBLE|WS_CHILD|ES_CENTER|WS_TABSTOP,
			NameEditRect.left , 
			NameEditRect.top , 
			NameEditRect.right- NameEditRect.left, 
			NameEditRect.bottom-NameEditRect.top, 
			hWnd, 
			(HMENU)IDC_HERONAME,
			NULL, 
			(PVOID)&ime
			);
		
		SendMessage(hPaoHeroName, EM_LIMITTEXT, 8, NULL);
		//		InputEditNode.lStyle = GetWindowLong(hPaoHeroName, GWL_STYLE);
		//		InputEditNode.nKeyBoardType = GetImeHandle ("全屏手写");
		//		strcpy(InputEditNode.szName, IDS_ANYMOUS);
		//		SetWindowLong(hPaoHeroName, GWL_USERDATA, (LONG)&InputEditNode);
		SendMessage(hPaoHeroName,EM_SETSEL,-1,-1);
		//SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, IDM_EXIT, (LPARAM)IDS_BACK);
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)IDS_CONFIRM);
		
		break;
		
	case WM_DESTROY:
		
		hWnd = NULL;
		//		UnregisterClass("PaoSetHeroWndClass",NULL);
		break;
		
	case WM_ACTIVATE:
		if (wParam == WA_ACTIVE)
			SetFocus(hPaoHeroName);
		break;
		
	case WM_COMMAND:
		
		switch (LOWORD(wParam))
		{
		case IDM_CONFIRM:
			{
				char s[20];
				
				GetWindowText(hPaoHeroName, s, 9);//(PaoRecord+TOP_REC_NUM-1)->Name
				if(0 == strcmp(s,""))//(PaoRecord+TOP_REC_NUM-1)->Name
				{
					MsgWin(NULL, NULL, 0, IDS_INPUT_NOTICE,IDS_PAO,Notify_Info, ML("OK") ,NULL,WAITTIMEOUT);
					break;
				}
				GetWindowText(hPaoHeroName, (PaoRecord+TOP_REC_NUM-1)->Name, 9);
				(PaoRecord+TOP_REC_NUM-1)->nScore = nScore;
				SortRecord();
				WritePaoHero();
				
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;
			
		case IDM_EXIT:
			{
				int nCaret_Start,nCaret_End;
				
				SendMessage(hPaoHeroName,EM_GETSEL,(WPARAM)&nCaret_Start,(LPARAM)&nCaret_End);
				
				if (nCaret_Start||nCaret_End)
				{
					SendMessage(hPaoHeroName, WM_KEYDOWN, VK_BACK, 0);
					SendMessage(hPaoHeroName, WM_CHAR, VK_BACK, 0);
				}
				else
				{
					//					DestroyWindow(hWnd);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
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
			UnregisterClass("PaoSetHeroWndClass",NULL);
			bRSetHeroWnd = FALSE;
			
			break;
			
		case WM_PAINT:
			{
				RECT	rc;
				int		OldStyle;
				hdc = BeginPaint(hWnd, NULL);
				
				//		TextOut(hdc,TitleRect.left+7, TitleRect.top , IDS_CHEER, strlen(IDS_CHEER));
				//		TextOut(hdc, TitleRect.left,  TitleRect.top+16,IDS_PLSETNAME, strlen(IDS_PLSETNAME));
				OldStyle = SetBkMode(hdc,TRANSPARENT);
				DrawText(hdc, IDS_CHEER, -1, &TitleRect, DT_HCENTER|DT_VCENTER);
				rc.top = TitleRect.top +25;
				rc.bottom = TitleRect.bottom +25;
				rc.left	= TitleRect.left;
				rc.right = TitleRect.right;
				DrawText(hdc, IDS_PLSETNAME, -1, &rc, DT_HCENTER|DT_VCENTER);
				SetBkMode(hdc,OldStyle);
				EndPaint(hWnd, NULL);
			}
			break;
			
		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{
				//#ifndef SCP_SMARTPHONE
				//		case VK_F2:
				//			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
				//            break;
				//#endif
			default:
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				break;
			}
			break;
			
			default:
				
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				
				break;
    }
	
    return lResult;
}

/*****************************************************************************\
* Funcation: EndPaoGame
*
* Purpose  : 一局结束的操作
*
* Explain  : 一局结束的操作
\*****************************************************************************/
static void EndPaoGame(HWND hWnd)
{
	bGameStart = FALSE;
	ShowArrowInGameState(hWnd);
	nScore = nDelBall*SCORE_PER_BALL+nDropBall*SCORE_PER_DROP;
	if(bMoveTID)
	{
		bMoveTID = FALSE;
		KillTimer(hWnd,MOVE_TIMER_ID);
	}
	if(bDelTID)
	{
		bDelTID = FALSE;
		KillTimer(hWnd,DEL_TIMER_ID);
	}
	if(bPressTID)
	{
		bPressTID = FALSE;
		KillTimer(hWnd,PRESS_TIMER_ID);
	}
	/*	if(bGunTID)
	{
	bGunTID = FALSE;
	KillTimer(hWnd,GUN_TIMER_ID);
}*/
	if(bDownWallTID)
	{
		bDownWallTID = FALSE;
		KillTimer(hWnd,DOWNWALL_TIMER_ID);
	}
	if(bMoveGunTID)
	{
		bMoveGunTID = FALSE;
		KillTimer(hWnd,MOVE_GUN_ID);
	}
	if(bRoundTID)
	{
		bRoundTID = FALSE;
		KillTimer(hWnd,ROUND_TIMER_ID);
	}
	SendMessage(hWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_GAMESTART);
	
	return;
}

/*****************************************************************************\
* Funcation: CanDelPao
*
* Purpose  : 判断有否三个以上相同球
*
* Explain  : 判断有否三个以上相同球
\*****************************************************************************/
static int CanDelPao(int m,int n)
{
	int nSameNum = 0;
	int i,j;
	
	nSameNum = 1;
	
	if (bChecked[m][n])
		return 0;
	
	bChecked[m][n] = TRUE;
	
	if (m%2)
	{
		for (i=m-1;i<=m+1;i++)
		{
			for (j=n;j<=n+1;j++)
			{
				if (IsSameBall(i,j))
				{
					nSameNum+=CanDelPao(i,j);
				}
			}
		}
		if (IsSameBall(m,n-1))
		{
			nSameNum+=CanDelPao(m,n-1);
		}
	}
	else
	{
		for (i=m-1;i<=m+1;i++)
		{
			for (j=n-1;j<=n;j++)
			{
				if (IsSameBall(i,j))
				{
					nSameNum+=CanDelPao(i,j);
				}
			}
		}
		if (IsSameBall(m,n+1))
		{
			nSameNum+=CanDelPao(m,n+1);
		}
	}
	
	return nSameNum;
}

/*****************************************************************************\
* Funcation: DelPao
*
* Purpose  : 删除三个以上相同球
*
* Explain  : 删除三个以上相同球
\*****************************************************************************/
static void DelPao(int i,int j)
{
	int m,n;
	bChecked[i][j] = TRUE;
	
	if (nCurFrame%DEL_FRAME_NUM)
	{
		StaticBall[i][j].color = (StaticBall[i][j].color%Pao_Ball_Num)+(nCurFrame%DEL_FRAME_NUM)*Pao_Ball_Num;
	}
	else
	{
		StaticBall[i][j].color = -1;
		nDelBall++;
	}
	
	Rect.left = StaticBall[i][j].lt.x;
	Rect.top = StaticBall[i][j].lt.y+DropLine*Y_BALL_INTERVAL;
	Rect.right = StaticBall[i][j].lt.x + 2*RADIUS_BALL-1;
	Rect.bottom = StaticBall[i][j].lt.y +DropLine*Y_BALL_INTERVAL + 2*RADIUS_BALL-1;
	InvalidateRect(hWndPaoGame,&Rect,TRUE);
	UpdateWindow(hWndPaoGame);
	
	if (i%2)
	{
		for (m=i-1;m<=i+1;m++)
		{
			for (n=j;n<=j+1;n++)
			{
				if (IsSameBall(m,n)&&(!bChecked[m][n]))
					DelPao(m,n);
			}
		}
		if (IsSameBall(i,j-1)&&(!bChecked[i][j-1]))
			DelPao(i,j-1);
	}
	else
	{
		for (m=i-1;m<=i+1;m++)
		{
			for (n=j-1;n<=j;n++)
			{
				if (IsSameBall(m,n)&&(!bChecked[m][n]))
					DelPao(m,n);
			}
		}
		if (IsSameBall(i,j+1)&&(!bChecked[i][j+1]))
			DelPao(i,j+1);
	}
	return;
}

/*****************************************************************************\
* Funcation: IsSameBall
*
* Purpose  : 判断(i,j)点是否与MoveBall颜色相同
*
* Explain  : 判断(i,j)点是否与MoveBall颜色相同
\*****************************************************************************/
static BOOL IsSameBall(int i,int j)
{
	if ((0<=i)&&(DeadLine>=i)
		&&(0<=j)&&(BALL_PER_LINE>j))
	{
		if ((MoveBall.color == StaticBall[i][j].color%Pao_Ball_Num))
			return TRUE;
	}
	
	return FALSE;
}

/*****************************************************************************\
* Funcation: ReadPaoHero
*
* Purpose  : 读英雄信息
*
* Explain  : 读英雄信息
\*****************************************************************************/
static	BOOL	ReadPaoHero()
{
	int	hFile;
	int i;
	if (-1==(hFile=(open(PAO_HERO_FILE,O_RDONLY))))
	{
		bFirst = TRUE;
		return FALSE;
	}
	if (-1==lseek(hFile,0,SEEK_SET))
	{
		close(hFile);
		return FALSE;
	}
	if (-1==(read(hFile,PaoRecord,sizeof(PAOHERO)*TOP_REC_NUM)))
	{
		close(hFile);
		return FALSE;
	}
	close(hFile);
	for (i=0;i<TOP_REC_NUM;i++)
	{
		if (0 == PaoRecord[i].nScore)
			strcpy(PaoRecord[i].Name,IDS_ANYMOUS);
	}
	return TRUE;
}

/*****************************************************************************\
* Funcation: InitPaoHero
*
* Purpose  : 初始化英雄信息
*
* Explain  : 初始化英雄信息
\*****************************************************************************/
static	void	InitPaoHero()
{
	int i;
	for (i=0;i<TOP_REC_NUM;i++)
	{
		strcpy(PaoRecord[i].Name,IDS_ANYMOUS);
		PaoRecord[i].nScore = 0;
	}
	return;
}

/*****************************************************************************\
* Funcation: PaoHeroProc
*
* Purpose  : 显示英雄榜界面
*
* Explain  : 注册并显示窗口
\*****************************************************************************/
static	DWORD	PaoHeroProc(HWND hWnd)
{
	WNDCLASS wc;
    DWORD dwRet;
	
    dwRet = TRUE;
	
	if(!bRHeroWnd)
	{
		wc.style         = CS_OWNDC;
		wc.lpfnWndProc   = PaoHeroWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName  = "PaoHeroWndClass";
		
		if (!RegisterClass(&wc))
		{
			bRHeroWnd = FALSE;
			return FALSE;
		}
		else
			bRHeroWnd = TRUE;
	}
	
	hWndPaoHero = CreateWindow(
		"PaoHeroWndClass", 
		IDS_HEROBOARD, 
		WS_VISIBLE|WS_CAPTION|PWS_STATICBAR,//|WS_BORDER,
		PLX_WIN_POSITION,
		/*
		WND_X, 
		WND_Y,//TOPSPACE, 
		WND_WIDTH, 
		WND_HEIGHT,// - TOPSPACE, 
		*/
		hWnd, 
		NULL,
		NULL, 
		NULL
		);
	
	if ( hWndPaoHero == NULL )
		return FALSE;
	
	CreateCapButton(hWndPaoHero, IDM_EXIT, 1, IDS_CLEAR);//左
	CreateCapButton(hWndPaoHero, IDM_CONFIRM,  0, IDS_BACK);
	
	ShowWindow(hWndPaoHero,SW_SHOW);
	UpdateWindow(hWndPaoHero);
	
	return dwRet;
}

/*****************************************************************************\
* Funcation: PaoHeroWndProc
*
* Purpose  : 英雄榜界面过程函数
*
* Explain  : 英雄榜界面过程函数
\*****************************************************************************/
LRESULT PaoHeroWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT	lResult;
	int		i;
	//	char	str[30];
	HDC		hdc;
	
	switch (wMsgCmd)
	{
	case WM_CREATE:
#if defined(SCP_SMARTPHONE)
		SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
#endif
		break;
		/*
		case WM_COMMAND:
		
		  switch (LOWORD(wParam))
		  {
		  case IDM_CONFIRM:			//-->清空，退出排行榜
		  
			InitPaoHero();
			WritePaoHero();
			
			  //			DestroyWindow(hWnd);
			  //			PostMessage(hWnd, WM_CLOSE, 0, 0);
			  
				break;
				case IDM_EXIT:		//退出排行榜
				
				  //			DestroyWindow(hWnd);
				  PostMessage(hWnd, WM_CLOSE, 0, 0);
				  
					break;
					}
					
					  break;
		*/
	case WM_ACTIVATE:
		if (wParam==WA_ACTIVE)
			SetFocus(hWnd);
		break;
		
	case WM_PAINT:
		{
			int		OldStyle;
			HFONT	hSmall,hold;
			char	NumBuf[3];
			char	strname[10];
			char	strScore[8];
			memset(NumBuf,0,3);
			hdc=BeginPaint(hWnd,NULL);
			GetFontHandle(&hSmall,SMALL_FONT);
			DrawImageFromFile(hdc, PAOBACKGROUND, X_GAMEOVERBACK, X_GAMEOVERBACK, ROP_SRC);
			OldStyle = SetBkMode(hdc, TRANSPARENT);
			hold = SelectObject(hdc,hSmall);
			for (i = 0; i < TOP_REC_NUM; i ++)
			{
				itoa(i+1, NumBuf, 10);
				sprintf(strname, "%s", PaoRecord[i].Name);
				sprintf(strScore, "%d", PaoRecord[i].nScore);
				TextOut(hdc, X_HERO_REC, HERO_REC_SPACE * i + Y_HERO_REC, NumBuf, strlen(NumBuf));
				TextOut(hdc, X_HERO_REC+20, HERO_REC_SPACE * i + Y_HERO_REC, strname, strlen(strname));
				TextOut(hdc, X_HERO_REC+100, HERO_REC_SPACE * i + Y_HERO_REC, strScore, strlen(strScore));
			}
			SelectObject(hdc,hold);
			SetBkMode(hdc, OldStyle);
			EndPaint(hWnd,NULL);
		}
		break;
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_RETURN:
			InitPaoHero();
			WritePaoHero();
			InvalidateRect(hWnd, NULL, TRUE);
			break;
			
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,NULL,NULL);
            break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;
		
		case WM_CLOSE:
			
			DestroyWindow(hWnd);
			UnregisterClass("PaoHeroWndClass",NULL);
			bRHeroWnd = FALSE;
			hWndPaoHero = NULL;
			
			break;
			/*
			case WM_DESTROY:
			hWnd=NULL;
			//		UnregisterClass("PaoHeroWndClass",NULL);
			break;
			*/
		default :
			
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			
			break;
	}
	return lResult;
}


/*****************************************************************************\
* Funcation: PaoGameShowProc
*
* Purpose  : 显示游戏说明界面
*
* Explain  : 注册并显示窗口
\*****************************************************************************/
static  DWORD   PaoGameShowProc(HWND hWnd)
{
    DWORD dwRet;
    
	dwRet = TRUE;
	
	CallGameHelpWnd(hWnd, IDS_HELPTEXT);
	
	return dwRet;
}

/*****************************************************************************\
* Funcation: WritePaoHero
*
* Purpose  : 写英雄信息
*
* Explain  : 写英雄信息
\*****************************************************************************/
static	BOOL	WritePaoHero()
{
	int	hFile;
	int		i,k;
	
	if (-1==(hFile=(open(PAO_HERO_FILE,O_WRONLY|O_TRUNC))))
		if (-1==(hFile=open(PAO_HERO_FILE,O_WRONLY|O_CREAT,0x666)))
			return	FALSE;
		if (-1==lseek(hFile,0,SEEK_SET))
		{
			close(hFile);
			return FALSE;
		}
		/*	if (-1==write(hFile,PaoRecord,sizeof(PAOHERO)*TOP_REC_NUM))
		{
		close(hFile);
		return	FALSE;
}*/
		for (k=0; k<TOP_REC_NUM; k++) 
		{
			i = (int)write(hFile,&PaoRecord[k],sizeof(PAOHERO));
			{
				if(i==-1)
				{
					perror("write file failure!\t");
					close(hFile);
					return FALSE;
				}
				if(i>0 && i<sizeof(PAOHERO))
				{
					close(hFile);
					unlink(PAO_HERO_FILE);
					MsgWin(NULL, NULL, 0 ,IDS_FILE_PROMPT,IDS_PAO,Notify_Info, ML("OK") ,NULL,WAITTIMEOUT);
					return	FALSE;
					
				}
			}
		}
		
		close(hFile);
		return TRUE;
}

/*****************************************************************************\
* Funcation: SortRecord
*
* Purpose  : 重排英雄数组
*
* Explain  : 重排英雄数组
\*****************************************************************************/
static void	SortRecord()
{
	PAOHERO	temp;
	int i;
	for (i=(TOP_REC_NUM-1);i>0;i--)
	{
		if ((PaoRecord+i)->nScore>((PaoRecord+i-1)->nScore))
		{
			strcpy(temp.Name,(PaoRecord+i-1)->Name);
			temp.nScore=(PaoRecord+i-1)->nScore;
			strcpy((PaoRecord+i-1)->Name,(PaoRecord+i)->Name);
			(PaoRecord+i-1)->nScore=(PaoRecord+i)->nScore;
			strcpy((PaoRecord+i)->Name,temp.Name);
			(PaoRecord+i)->nScore=temp.nScore;
		}
	}
	return;
}

/*****************************************************************************\
* Funcation: OnPaoChar
*
* Purpose  : 处理WM_CHAR消息
*
* Explain  : 处理WM_CHAR消息
\*****************************************************************************/
/*static int OnPaoChar(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
switch (wParam)
{
case '1':
if ('2'==CurChar)
CurChar = '1';
break;
case '2':
if ('3'==CurChar)
CurChar = '2';
break;
case '3':
CurChar = '3';
break;
case '6':
if ('1'==CurChar)
CurChar = '6';
break;
case '7':
if ('6'==CurChar)
{
CurChar = '7';
nMapNum++;
if (WriteMapNum())
{
if (!WriteMap())
nMapNum--;
}
else
{
nMapNum--;
}
}
break;
default:
return -1;
break;
}
return 1;
}
*/
/*****************************************************************************\
* Funcation: ReadMapNum
*
* Purpose  : 读地图数
*
* Explain  : 读地图数
\*****************************************************************************/
/*static BOOL	ReadMapNum()
{
HANDLE	hFile;
if (-1==(hFile = open(PAO_MAP_FILE,O_RDONLY)))
return	FALSE;
if (-1==lseek(hFile,0,SEEK_SET))
{
close(hFile);
return FALSE;
}
if (-1==(read(hFile,&nMapNum,sizeof(int))))
{
close(hFile);
return FALSE;
}
close(hFile);
return	TRUE;
}
*/
/*****************************************************************************\
* Funcation: InitMapNum
*
* Purpose  : 初始化地图数
*
* Explain  : 初始化地图数
\*****************************************************************************/
/*static	void	InitMapNum()
{
nMapNum = 0;
return;
}*/

/*****************************************************************************\
* Funcation: WriteMapNum
*
* Purpose  : 写地图数
*
* Explain  : 写地图数
\*****************************************************************************/
/*static BOOL	WriteMapNum()
{
HANDLE	hFile;
if (-1==(hFile=(open(PAO_MAP_FILE,O_WRONLY))))
if (-1==(hFile=open(PAO_MAP_FILE,O_WRONLY|O_CREAT,
FILE_ATTRIBUTE_NORMAL)))
return	FALSE;
if (-1==lseek(hFile,0,SEEK_SET))
{
close(hFile);
return FALSE;
}
write(hFile,&nMapNum,sizeof(int));

  close(hFile);
  return	TRUE;
  }
*/
/*****************************************************************************\
* Funcation: WriteMap
*
* Purpose  : 写地图
*
* Explain  : 写地图
\*****************************************************************************/
/*static	BOOL	WriteMap()
{
HANDLE	hFile;
int i,j;
if (-1==(hFile=(open(PAO_MAP_FILE,O_WRONLY))))
if (-1==(hFile=open(PAO_MAP_FILE,O_WRONLY|O_CREAT,
FILE_ATTRIBUTE_NORMAL)))
return	FALSE;
if (-1==lseek(hFile,0,SEEK_END))
{
close(hFile);
return FALSE;
}
for (i=0;i<=NORMAL_DEADLINE;i++)
{
for (j=0;j<BALL_PER_LINE;j++)
{
if (-1==(write(hFile,&(StaticBall[i][j]),sizeof(THEBALL))))
{
close(hFile);
return FALSE;
}
}
}

  close(hFile);
  return	TRUE;
  }
*/
/*****************************************************************************\
* Funcation: ReadMap
*
* Purpose  : 读地图
*
* Explain  : 读地图
\*****************************************************************************/
/*static BOOL	ReadMap(int n)
{
HANDLE	hFile;
int i,j;
if (-1==(hFile = open(PAO_MAP_FILE,O_RDONLY)))
return	FALSE;
if (-1==lseek(hFile,sizeof(int),SEEK_SET))
{
close(hFile);
return FALSE;
}
if (-1==lseek(hFile,sizeof(THEBALL)*(NORMAL_DEADLINE+1)*BALL_PER_LINE*n,SEEK_CUR))
{
close(hFile);
return FALSE;
}
for (i=0;i<=NORMAL_DEADLINE;i++)
{
for (j=0;j<BALL_PER_LINE;j++)
{
if (-1==(read(hFile,&(StaticBall[i][j]),sizeof(THEBALL))))
{
close(hFile);
return FALSE;
}
}
}
close(hFile);
return	TRUE;
}
*/
/*****************************************************************************\
* Funcation: UncheckBall
*
* Purpose  : 置Check数组为FALSE
*
* Explain  : 置Check数组为FALSE
\*****************************************************************************/
static void	UncheckBall()
{
	int i,j;
	for (i=0;i<=DeadLine;i++)
	{
		for (j=0;j<BALL_PER_LINE;j++)
		{
			bChecked[i][j] = FALSE;
		}
	}
	return;
}

/*****************************************************************************\
* Funcation: AdjustBall
*
* Purpose  : 调整停球位置
*
* Explain  : 调整停球位置
\*****************************************************************************/
static void AdjustBall()
{
	int sp;
	int m,n;			//记录变量
	int i,j;			//循环变量
	sp = Square(RADIUS_BALL*3);
	//最好保留！	m = MoveBall.i;
	//	n = MoveBall.j;
	if (MoveBall.i%2)
	{
		for (i=MoveBall.i-1;i<=MoveBall.i+1;i++)
		{
			for (j=MoveBall.j;j<=MoveBall.j+1;j++)
			{
				if (IsVoid(i,j))
					if (sp>SquarePlus(i,j))
					{
						sp = SquarePlus(i,j);
						m = i;
						n = j;
					}
			}
		}
		if (IsVoid(MoveBall.i,MoveBall.j-1))
			if (sp>SquarePlus(MoveBall.i,MoveBall.j-1))
			{
				sp = SquarePlus(MoveBall.i,MoveBall.j-1);
				m = MoveBall.i;
				n = MoveBall.j-1;
			}
	}
	else
	{
		for (i=MoveBall.i-1;i<=MoveBall.i+1;i++)
		{
			for (j=MoveBall.j-1;j<=MoveBall.j;j++)
			{
				if (IsVoid(i,j))
					if (sp>SquarePlus(i,j))
					{
						sp = SquarePlus(i,j);
						m = i;
						n = j;
					}
			}
		}
		if (IsVoid(MoveBall.i,MoveBall.j+1))
			if (sp>SquarePlus(MoveBall.i,MoveBall.j+1))
			{
				sp = SquarePlus(MoveBall.i,MoveBall.j+1);
				m = MoveBall.i;
				n = MoveBall.j+1;
			}
	}
	MoveBall.i = m;
	MoveBall.j = n;
	return;
}

/*****************************************************************************\
* Funcation: IsVoid
*
* Purpose  : 是否是空
*
* Explain  : 是否是空
\*****************************************************************************/
static BOOL IsVoid(int i,int j)
{
	if ((0<=i)&&(DeadLine>=(i+DropLine))
		&&(0<=j)&&(BALL_PER_LINE>j))
	{
		if (-1==StaticBall[i][j].color)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/*****************************************************************************\
* Funcation: IsSetOver
*
* Purpose  : 一局结束的判断
*
* Explain  : 一局结束的判断
\*****************************************************************************/
static BOOL IsSetOver()
{
	int i,j;
	for (i=0;i<=DeadLine-DropLine;i++)
	{
		for (j=0;j<BALL_PER_LINE;j++)
		{
			if (!IsVoid(i,j))
				return FALSE;
		}
	}
	return TRUE;
}

/*****************************************************************************\
* Funcation: OnDelTimer
*
* Purpose  : 消去球的处理
*
* Explain  : 消去球的处理
\*****************************************************************************/
static void OnDelTimer(HWND hWnd)
{
	if(bDelTID)
	{
		bDelTID = FALSE;
		KillTimer(hWnd,DEL_TIMER_ID);
	}
	
	if(nCurFrame == 0)
	{
		bWallShouldDown = FALSE;
	}
	nCurFrame++;
	
	{
		int i,j;
		UncheckBall();
		if(nCurFrame==1)
		{
			if (CanDelPao(MoveBall.i,MoveBall.j)<3)
			{
				nCurFrame = 0;
				MoveBall.color = -1;
				if (IsDeadPao())
				{
					//	InvalidateRect(hWnd,NULL,TRUE);
					EndPaoGame(hWnd);
					Rect.left = X_GAMEBACK;
					Rect.top = Y_GAMEBACK;
					Rect.right = X_GAMEBACK + GAMEBACK_WIDTH;
					Rect.bottom = Y_GAMEBACK+GAMEBACK_HEIGHT;
					if (nScore>PaoRecord[TOP_REC_NUM-1].nScore)
					{
						SetPaoHeroProc(hWnd);
					}
					else
					{
						//PaoGameOverProc();
						PLXTipsWin(NULL, hWnd, IDRM_GAMEOVER, IDS_GAMEOVER, IDS_PAO, Notify_Info, IDS_OK, NULL,WAITTIMEOUT);
					} 
				}
				
				return;
			}
		}
		if (nCurFrame <= DEL_FRAME_NUM) 
		{
			UncheckBall();
			DelPao(MoveBall.i,MoveBall.j);
		}
		if (nCurFrame>DEL_FRAME_NUM)
		{
			for (i=0;i<=DeadLine-DropLine;i++)
			{
				for (j=0;j<BALL_PER_LINE;j++)
				{
					if (-1 != StaticBall[i][j].color)
					{
						UncheckBall();
						if (CanDropPao(i,j))
						{
							UncheckBall();
							DropPao(i,j);
						}
					}
				}
			}
		}
		
		/*		Rect.left = X_GAMEBACK;
		Rect.top = Y_GAMEBACK+Y_BALL_INTERVAL*DropLine;
		Rect.right = X_GAMEBACK+GAMEBACK_WIDTH;
		Rect.bottom = Y_GAMEBACK+Y_BALL_INTERVAL*NORMAL_DEADLINE+8;
		
		InvalidateRect(hWnd,&Rect,TRUE);*/
		
		//		UpdateWindow(hWnd);
	}
	if(!bDelTID)
	{
		bDelTID = TRUE;
		SetTimer(hWnd,DEL_TIMER_ID,DEL_TIMER_ELAPSE,NULL);
	}
	
	if (!(nCurFrame%(DEL_FRAME_NUM*2)))
	{
		nCurFrame = 0;
		if(bDelTID)
		{
			bDelTID = FALSE;
			KillTimer(hWnd,DEL_TIMER_ID);
		}
		nScore = nDelBall*SCORE_PER_BALL+nDropBall*SCORE_PER_DROP;
		
		Rect.left = X_LU_SCORENUM;
		Rect.top = Y_LU_SCORENUM;
		Rect.right = X_RD_SCORENUM;
		Rect.bottom = Y_RD_SCORENUM;
		
		InvalidateRect(hWnd,&Rect,TRUE);
		UpdateWindow(hWnd);
		
		if (IsSetOver())
		{
			DropLine = 0;
			if(bDownWallTID)
			{
				bDownWallTID = FALSE;
				KillTimer(hWnd,DOWNWALL_TIMER_ID);
			}
			nCurMap++;
			if(nCurMap > nMapNum)
				nCurMap = 1;
			ReadMapNew(nCurMap);
			
			DeadLine = 	NORMAL_DEADLINE;
			nGunOffset = (int)(MAX_X_GUN_OFFSET/2);
			nCurFrame = 0;
			DropLine = 0;
			
			Ball_x_Offset = nGunOffset+(RADIUS_WIDTH_GUN);
			Ball_y_Offset = MAX_Y_BALL_OFFSET;
			
			InitBall(hWnd);
			
			Gun_Degree = NORMAL_DEGREE;
			Move_Degree = NORMAL_DEGREE;
			GameFrameRect.top = Y_GAMEBACK-1+Y_BALL_INTERVAL*(NORMAL_DEADLINE-DeadLine);
			if(!bDownWallTID)
			{
				bDownWallTID = TRUE;
				SetTimer(hWnd, DOWNWALL_TIMER_ID, Interval_Wall, NULL);
			}
			InitDragenI();
			if(bDragenTimer)
			{
				bDragenTimer = FALSE;
				KillTimer(hWnd,DRAGEN_TIMER_ID);
			}
			bRoundBN = TRUE;
			if(!bRoundTID)
			{
				bRoundTID = TRUE;
				SetTimer(hWnd, ROUND_TIMER_ID, Interval_Round, NULL);
			}
			
			Rect.left = X_GAMEBACK;
			Rect.top = Y_GAMEBACK;
			Rect.right = X_GAMEBACK + GAMEBACK_WIDTH;
			Rect.bottom = Y_GAMEBACK+GAMEBACK_HEIGHT;
			
			InvalidateRect(hWnd,&Rect,TRUE);
			UpdateWindow(hWnd);
		}
		if (IsDeadPao())
		{
			//	InvalidateRect(hWnd,NULL,TRUE);
			EndPaoGame(hWnd);
			Rect.left = X_GAMEBACK;
			Rect.top = Y_GAMEBACK;
			Rect.right = X_GAMEBACK + GAMEBACK_WIDTH;
			Rect.bottom = Y_GAMEBACK+GAMEBACK_HEIGHT;
			if (nScore>PaoRecord[TOP_REC_NUM-1].nScore)
			{
				SetPaoHeroProc(hWnd);
			}
			else
			{
				//PaoGameOverProc();
				PLXTipsWin(NULL, hWnd, IDRM_GAMEOVER, IDS_GAMEOVER, IDS_PAO, Notify_Info, IDS_OK, NULL,WAITTIMEOUT);
			} 
			bWallShouldDown = FALSE;
		}
		else
		{
			if(bWallShouldDown)
			{
				SendMessage(hWnd, WM_TIMER, DOWNWALL_TIMER_ID, NULL);
				bWallShouldDown = FALSE;
			}
			MoveBall.color = -1;
			
			/*			Rect.left = X_DARKBACK;
			Rect.top = Y_DARKBACK;
			Rect.right = X_DARKBACK+DARKBACK_WIDTH;
			Rect.bottom = Y_DARKBACK+DARKBACK_HEIGHT;
			
			  InvalidateRect(hWnd,&Rect,TRUE);
			UpdateWindow(hWnd);*/
		}
	}
	
	return;
}

/*****************************************************************************\
* Funcation: OnGunMove
*
* Purpose  : 枪移动的处理
*
* Explain  : 枪移动的处理
\*****************************************************************************/
/*static void OnGunMove(HWND hWnd)
{
static int GunDirect = 1;
nGunOffset += GunDirect*GUN_MOVE_INTERVAL;
ToMoveBall.lt.x += GunDirect*GUN_MOVE_INTERVAL;
if (ToMoveBall.color == -1)
Ball_x_Offset += GunDirect*GUN_MOVE_INTERVAL;
if (0>=nGunOffset)
{
GunDirect = 1;
nGunOffset += GunDirect*GUN_MOVE_INTERVAL;
ToMoveBall.lt.x += GunDirect*GUN_MOVE_INTERVAL;
Ball_x_Offset += GunDirect*GUN_MOVE_INTERVAL;
}
else if (MAX_X_GUN_OFFSET<=nGunOffset)
{
GunDirect = -1;
nGunOffset += GunDirect*GUN_MOVE_INTERVAL;
ToMoveBall.lt.x += GunDirect*GUN_MOVE_INTERVAL;
Ball_x_Offset += GunDirect*GUN_MOVE_INTERVAL;
}
ChangeDragenI();

  Rect.left = X_GAMEBACK;
  Rect.top = Y_GAMEBACK+Y_BALL_INTERVAL*NORMAL_DEADLINE+8;
  Rect.right = X_GAMEBACK+GAMEBACK_WIDTH;
  Rect.bottom = Y_GAMEBACK+GAMEBACK_HEIGHT;
  
	InvalidateRect(hWnd,&Rect,TRUE);
	UpdateWindow(hWnd);
	return;
	}
	*/
	/*****************************************************************************\
	* Funcation: GameOverProc
	*
	* Purpose  : 结束的处理
	*
	* Explain  : 结束的处理
	\*****************************************************************************/
	DWORD PaoGameOverProc()
	{
		WNDCLASS wc;
		DWORD dwRet;
		
		dwRet = TRUE;
		
		if(!bRGameOverWnd)
		{
			wc.style         = CS_OWNDC;
			wc.lpfnWndProc   = PaoGameOverWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName  = NULL;
			wc.lpszClassName  = "PaoGameOverWndClass";		
			
			if (!RegisterClass(&wc))
			{
				bRGameOverWnd = FALSE;
				return FALSE;
			}
			else
				bRGameOverWnd = TRUE;
		}
		
		hPaoGameOver = CreateWindow(
			"PaoGameOverWndClass", 
			IDS_GAMEOVER, 
			WS_CAPTION|PWS_STATICBAR,//|WS_BORDER,
			PLX_WIN_POSITION,
			/*
			WND_X, 
			WND_Y,//TOPSPACE, 
			WND_WIDTH, 
			WND_HEIGHT,//-TOPSPACE,
			*/
			hWndPaoGame, 
			NULL,
			NULL,
			NULL
			);
		if (hPaoGameOver == NULL)	
			return FALSE;
		
		SendMessage(hPaoGameOver, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_OVER_RETURN, 0), (LPARAM)IDS_BACK);
		
		ShowWindow(hPaoGameOver,SW_SHOW);
		UpdateWindow(hPaoGameOver);
		
		return dwRet;
	}
	
	/*****************************************************************************\
	* Funcation: PaoGameOverWndProc
	*
	* Purpose  : 结束的窗口过程
	*
	* Explain  : 结束的窗口过程
	\*****************************************************************************/
	LRESULT PaoGameOverWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
	{
		//------------------+------------------------------------------------------------------------------------
		LRESULT lResult;
		char temp[20];
		HFONT hFont;
		HFONT hOldFont;
		LOGFONT lf;
		HDC	hdc;
		//------------------+------------------------------------------------------------------------------------
		
		switch (wMsgCmd)
		{
		case WM_CREATE:
			
#ifdef SCP_SMARTPHONE
			SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
#endif
			break;
			
		case WM_CLOSE:
			
			DestroyWindow(hWnd);
			UnregisterClass("PaoGameOverWndClass",NULL);
			bRGameOverWnd = FALSE;
			
			break;
			
		case WM_DESTROY:
			
			hPaoGameOver = NULL;
			//		UnregisterClass("PaoGameOverWndClass",NULL);
			
			break;
			
		case WM_COMMAND:
			
			switch (LOWORD(wParam))
			{
				
			case ID_OVER_RETURN:
				
				//			DestroyWindow(hWnd);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				
				break;
			}
			break;
			
			case WM_PAINT :
				{
					int		OldStyle;
					
					lf.lfHeight	= 20; 
					lf.lfWidth	= 20; 
					lf.lfEscapement = 0; 
					lf.lfOrientation= 0; 
					lf.lfWeight	= FW_BLACK;  
					lf.lfItalic = 0;  
					lf.lfUnderline = 0;  
					lf.lfStrikeOut = 0;  
					lf.lfCharSet = 0;  
					lf.lfOutPrecision = 0;  
					lf.lfClipPrecision = 0;  
					lf.lfQuality = 0;  
					lf.lfPitchAndFamily = 0; 
					
					hFont = CreateFontIndirect( &lf ); 
					
					hdc = BeginPaint(hWnd, NULL);
					
					DrawImageFromFile(hdc, PAOBACKGROUND, X_GAMEOVERBACK, Y_GAMEOVERBACK, ROP_SRC);
					
					hOldFont = SelectObject(hdc, hFont);
					
					OldStyle = SetBkMode(hdc, TRANSPARENT);
					//界面调整
					//TextOut(hdc, OVER_TEXT_X, OVER_TEXT_Y, IDS_GAMEOVER, strlen(IDS_GAMEOVER));
					sprintf(temp, "%d", nScore);
					TextOut(hdc, SCORE_TEXT_X, SCORE_TEXT_Y, IDS_SCORE, strlen(IDS_SCORE));
					TextOut(hdc, SCORE_NUM_X, SCORE_NUM_Y, temp, strlen(temp));
					
					SelectObject(hdc, hOldFont);
					/*
					sprintf(temp, "%d", nScore);
					TextOut(hdc, SCORE_TEXT_X, SCORE_TEXT_Y, IDS_SCORE, strlen(IDS_SCORE));
					TextOut(hdc, SCORE_NUM_X, SCORE_NUM_Y, temp, strlen(temp));
					*/
					DeleteObject(hFont);
					
					SetBkMode(hdc, OldStyle);
					
					EndPaint(hWnd, NULL);
				}
				
				break;
				
			default:
				
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}		
		return lResult;
}

/*********************************************************************\
* Function	  InitDragenI 
* Purpose     初始化龙的变量i 
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	void	InitDragenI()
{
	if ((0!=Dragen.i)&&(1!=Dragen.i))
	{
		Dragen.i = 0;
	}
	return;
}

/*********************************************************************\
* Function	   CalcDragenLT
* Purpose      计算龙的位置
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	void	CalcDragenLT()
{
	Dragen.lt.y = Y_DRAGEN;
	if (LEFT_DRAGEN == Dragen.Dir)
	{
		Dragen.lt.x = nGunOffset+MIN_X_LEFT_GUN-DRAGEN_WIDTH+6;
	}
	else if (RIGHT_DRAGEN == Dragen.Dir)
	{
		Dragen.lt.x = nGunOffset+MIN_X_LEFT_GUN+2*RADIUS_WIDTH_GUN-4;
	}
	return;
}

/*********************************************************************\
* Function	   ChangeDragenI
* Purpose      改变龙的动画
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	void	ChangeDragenI()
{
	if (0 == Dragen.i)
	{
		Dragen.i = 1;
	}
	else if (1 == Dragen.i)
	{
		Dragen.i = 0;
	}
	else 
	{
		Dragen.i = 0;
	}
	
	/*	Rect.left = X_GAMEBACK;
	Rect.top = Y_GAMEBACK+Y_BALL_INTERVAL*NORMAL_DEADLINE+8;
	Rect.right = X_GAMEBACK+GAMEBACK_WIDTH;
	Rect.bottom = Y_GAMEBACK+GAMEBACK_HEIGHT;*/
	
	CalcDragenLT();
	
	Rect.left = Dragen.lt.x;
	Rect.top = Dragen.lt.y;
	Rect.right = Dragen.lt.x+DRAGEN_WIDTH;
	Rect.bottom = Dragen.lt.y+DRAGEN_HEIGHT;
	
	return;
}

/*********************************************************************\
* Function	   StrToInt
* Purpose      把字符型变量转化成整形
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int StrToInt ( char * pString)
{
	int count;
	int i;     
	count = 0;
	i = 0;
	while( pString[i] != '\0')
	{
		count = count * 10 + pString[i] - 48 ;  //pString[i] - '0'为字符pString[i]对应 的数字。
		i++;
	}  
	return count;
}

/*********************************************************************\
* Function	   ReadMapNum
* Purpose      读地图的总关数
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL ReadMapNum(int fp)
{
	char Temp[100];
	int i;
	int j;
	i = 0;
	j = read(fp, &Temp[i], sizeof(char));
	
	while(Temp[i] != 'G')
	{
		i++;
		read(fp, &Temp[i], sizeof(char));
	}
	//	i++;
	//	read(fp, &temp[i], sizeof(char));
	Temp[i] = '\0';
	lseek(fp, 2*sizeof(char), SEEK_CUR);
	nMapNum = StrToInt(Temp);
	return TRUE;
}

/*********************************************************************\
* Function	   FileReadLine
* Purpose      按行读文件
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void FileReadLine(int fp, char** myString)
{
	char temp[100];
	int i;
	int j;
	i = 0;
	j = read(fp, &temp[i], sizeof(char));
	if(j == -1)
	{
		*myString = NULL;
		return;
	}
	while(temp[i] != 'E')
	{
		i++;
		read(fp, &temp[i], sizeof(char));
	}
	i++;
	//	read(fp, &temp[i], sizeof(char));
	temp[i] = '\0';
	lseek(fp,2*sizeof(char),SEEK_CUR);
	if(myString == NULL)
		return;
	*myString = (char*)malloc(i*sizeof(char)+8);
	strcpy(*myString, temp);
}

/*********************************************************************\
* Function	   ReadMapNew
* Purpose      读当前关的地图
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL ReadMapNew(int n)
{
	int	fp;
	int i,j;
	int curPos;
	char* pString;
	if (-1 == (fp = open(PAOMAP,O_RDONLY)))
		return	FALSE;
	
	//read(fp, &nMapNum, sizeof(char));
	if(!ReadMapNum(fp))
		return FALSE;
	
	for(i=0; i<n-1; i++)
		FileReadLine(fp, NULL);
	
	FileReadLine(fp, &pString);
	
	if(pString == NULL)
	{
		close(fp);
		free(pString);
		return	FALSE;
	}
	
	InitStaticBall();
	curPos = 0;
	for(j = 0; j<8; j++)
		StaticBall[0][j].color = pString[j] - 48;
	
	curPos = 8; 
	i = 1;
	while(pString[curPos] != 'E')
	{
		for(j = 0; j<8; j++)
			StaticBall[i][j].color = pString[curPos+1+j]-48;
		i++;
		curPos = curPos+9;
	}
	
	free(pString);
	close(fp);
	return	TRUE;
}


static	void	InitSelItem()
{
	int i;
	for(i=0; i<MENU_ITEM_NUM; i++)
	{
		switch(i) {
		case 0:
			SetRect(&SELRECT[i],X_LEFT_ITEM,Y_UP_ITEM,X_LEFT_ITEM + SEL_WIDTH,Y_UP_ITEM + SEL_HEIGHT);
			break;
		case 1:
			SetRect(&SELRECT[i],X_LEFT_ITEM,Y_DOWN_ITEM,X_LEFT_ITEM + SEL_WIDTH,Y_DOWN_ITEM + SEL_HEIGHT);
			
			break;
		case 2:
			SetRect(&SELRECT[i],X_RIGHT_ITEM,Y_UP_ITEM,X_RIGHT_ITEM + SEL_WIDTH,Y_UP_ITEM + SEL_HEIGHT);
			break;
		case 3:
			SetRect(&SELRECT[i],X_RIGHT_ITEM,Y_DOWN_ITEM,X_RIGHT_ITEM + SEL_WIDTH,Y_DOWN_ITEM + SEL_HEIGHT);
			break;
		default:
			break;
		}
	}
	
}

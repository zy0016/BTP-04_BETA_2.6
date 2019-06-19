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

#define __MODULE__ "PUSHBOX"
#include "PushBox.h"

static MENUITEMTEMPLATE PushBoxMenu[] =
{
	{ MF_STRING, IDM_NEWGAME, "New game", NULL},
	{ MF_STRING, IDM_SELECT, "Select", NULL },
	{ MF_STRING, IDM_GOONGAME, "go on", NULL },
	{ MF_STRING, IDM_HELP, "help", NULL },
//	{ MF_STRING, ID_SETTING, IDS_SETTING, NULL },
    { MF_END, 0, NULL, NULL }
};
//#endif

static const MENUTEMPLATE PushBoxMenuTemplate =
{
    0,
    PushBoxMenu
};
/*****************************************************************************\
* Funcation: PushBox_AppControl
*
* Purpose  : 应用程序入口函数
*
* Explain  : 
*                      
\*****************************************************************************/
DWORD GamePushBox_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
//------------------------------------------------------------------------
    WNDCLASS	wc;
	DWORD		dwRet;
//------------------------------------------------------------------------

	dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:

		hInst = (HINSTANCE)pInstance;
	
        break;

    

   case APP_GETOPTION:
		
	
		if(wParam == AS_APPWND)

			dwRet = (DWORD)hPushBoxWnd;


		break;


    case APP_ACTIVE :

		if (IsWindow(hPushBoxWnd))
		{
			ShowWindow(hPushBoxWnd, SW_SHOW);
			ShowOwnedPopups(hPushBoxWnd, TRUE);
			UpdateWindow(hPushBoxWnd);
		}		
        else
        {
#ifdef _MODULE_DEBUG_
			StartObjectDebug();
#endif
			hPushBoxMenu = LoadMenuIndirect(&PushBoxMenuTemplate);

			ModifyMenu(hPushBoxMenu, IDM_NEWGAME, MF_BYCOMMAND, IDM_NEWGAME, IDS_NEW);
			ModifyMenu(hPushBoxMenu, IDM_SELECT, MF_BYCOMMAND, IDM_SELECT, IDS_SELECT);
			ModifyMenu(hPushBoxMenu, IDM_GOONGAME,MF_BYCOMMAND,  IDM_GOONGAME, IDS_GOONGAME);
			ModifyMenu(hPushBoxMenu, IDM_HELP, MF_BYCOMMAND, IDM_HELP, IDS_HELP);
			//register window class			
			wc.style			= CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc	    = (WNDPROC)PushBoxMainWndProc;
			wc.cbClsExtra		= 0;
			wc.cbWndExtra		= 0;
			wc.hInstance		= NULL;
			wc.hIcon			= NULL;
			wc.hbrBackground	= NULL;//GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName	    = NULL;
			wc.lpszClassName	= "PushboxMainWndClass";
			
			RegisterClass(&wc);   
			hPushBoxWnd = CreateWindow("PushboxMainWndClass", 
				IDS_PUSHBOX,
				WS_VISIBLE|PWS_STATICBAR|WS_CAPTION,
				PLX_WIN_POSITION,
				NULL, 
				hPushBoxMenu,
				NULL,
				NULL);
			
			if (!hPushBoxWnd)
				return FALSE;
			SendMessage(hPushBoxWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
			ShowWindow(hPushBoxWnd, SW_SHOW);
			UpdateWindow(hPushBoxWnd);
		}
		//		return dwRet;		
		
		break;

    case APP_INACTIVE :
		ShowOwnedPopups(hPushBoxWnd, SW_HIDE);
		ShowWindow(hPushBoxWnd, SW_HIDE);
        break;

   
    }

    return dwRet;
}


/*********************************************************************\
* Function：游戏的主界面窗口过程的消息处理函数	   
* Purpose ：游戏主界面     
* Params ：	hWnd, message, wParam, lParam   
* Return ： LRESULT	 	   
* Remarks ：	   
**********************************************************************/
LRESULT	PushBoxMainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT			lResult;
	HDC				hdc;
	PAINTSTRUCT		ps;
	
	switch (message)
	{
	case WM_CREATE:
		{			
			hdc = GetDC(hWnd);

			SendMessage(hWndHelp, PWM_SETAPPICON, 0, (LPARAM)DIRICON);

			hPushBoxCover = CreateBitmapFromImageFile(hdc, PushBoxCover, &clrCover, &bclarity);

			//hPushBoxCoverWords = CreateBitmapFromImageFile(hdc, ML("/rom/game/pushbox/pb_cowe.gif"), &clr4, &bclarity);

			//hPushBoxCoverMan = CreateBitmapFromImageFile(hdc, PushBoxCoverMan, &clrCoverMan, &bclarity);

			nSelMenuItem = 0;
			
			ShowArrowState(hWnd,nSelMenuItem);

			SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLMIDDLE, MASKMIDDLE);
			
			ReadLevelFromFile();
			ReadSuccess = ReadMapFromFile();
			
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0), (LPARAM)IDS_EXIT);// 按纽
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)IDS_NEW);// 按纽
			
			ReleaseDC(hWnd,hdc);
		}
		break;
		
	case WM_COMMAND:
			
		switch (LOWORD(wParam))
		{			
		case IDM_EXIT:	

			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
			
/*
		case IDM_CONFIRM:

			switch(nSelMenuItem)
			{
			case 0:
				PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
				break;

			case 1:
				PostMessage(hWnd,WM_COMMAND,IDM_SELECT,NULL);
				break;

			case 2:
				PostMessage(hWnd,WM_COMMAND,IDM_GOONGAME,NULL);
				break;

			case 3:
				PostMessage(hWnd,WM_COMMAND,IDM_HELP,NULL);
				break;

			default:
				nSelMenuItem = 0;
				PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);

				break;
			}
			break;
*/
		case IDM_CONFIRM:
		case IDM_NEWGAME:

			curnLevel = 1;
			ReadBoxMap(curnLevel);
			FreeList(&ppList);
			Distance = -1;
			BackCount = 0;
//			MoveNum = 0;

			if( !GameCreate )
			{
				if(!PushBoxGameProc(hWnd))
					return FALSE;
			}
			else
			    ShowGameWindowAnew(hPushBoxGameWnd);

			break;

		case IDM_SELECT:
				
			if(ReadSuccess)
			{
				if( !LevelSetupCreate )
				{
					if(!PushBoxOptionSetupProc(hWnd))
						return FALSE;
				}
			}
			break;

		case IDM_GOONGAME:
			
			if(ReadSuccess)
			{
				if(GameCreate)
					ShowGameWindowAnew(hWnd);
				else
				{
					ReadBoxMap(curnLevel);
					if(!PushBoxGameProc(hWnd))
						return FALSE;
				}
			}
			break;

		case IDM_HELP:
			
			PushBoxGameHelpProc(hWnd);
			
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
					PostMessage(hWnd,WM_COMMAND,IDM_SELECT,NULL);
					break;
					
				case 2:
					PostMessage(hWnd,WM_COMMAND,IDM_GOONGAME,NULL);
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
				ShowArrowState(hWnd,nSelMenuItem);
				InvalidateRect(hWnd,NULL,FALSE);
			}
		}
		break;
/*
	case WM_KEYDOWN:
		
		switch(LOWORD(wParam))
		{

		case VK_F10:

			//DestroyWindow(hWnd);
			PostMessage(hWnd, WM_CLOSE, 0, 0);

            
			break;

		//	PostMessage(hWnd, WM_CLOSE, 0, 0);

		case VK_UP:
			if (0 == nSelMenuItem) {
				nSelMenuItem = MENU_ITEM_NUM-1;
			}
			else if (0<nSelMenuItem)
			{				
				nSelMenuItem--;				
			}
			
			ShowArrowState(hWnd,nSelMenuItem);
			InvalidateRect(hWnd,NULL,FALSE);
			break;

		case VK_DOWN:
			
			
			if (MENU_ITEM_NUM-1 == nSelMenuItem) {
				nSelMenuItem = 0;
			}
			else if (MENU_ITEM_NUM-1>nSelMenuItem)
			{
				nSelMenuItem++;
				
			}
			ShowArrowState(hWnd,nSelMenuItem);
			InvalidateRect(hWnd,NULL, FALSE);
			break;
			//
//		case VK_F5:
//
//			PostMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_CONFIRM,1),NULL);
//			
//			break;
//	
	
		case VK_RETURN:

			switch(nSelMenuItem)
			{
			case 0:
				PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
				break;
				
			case 1:
				PostMessage(hWnd,WM_COMMAND,IDM_SELECT,NULL);
				break;
				
			case 2:
				PostMessage(hWnd,WM_COMMAND,IDM_GOONGAME,NULL);
				break;
				
			case 3:
				PostMessage(hWnd,WM_COMMAND,IDM_HELP,NULL);
				break;
			}
			break;

		default:

			return PDADefWindowProc(hWnd, message, wParam, lParam);
			
			break;
		}
		
		break;
*/			
	case WM_PAINT:
		{
			COLORREF	OldClr;
			int			OldStyle;
			int			COVERMAN_X;
			int			COVERMAN_Y;
			HDC         hMemDC;
			
			hdc = BeginPaint(hWnd, &ps);

			hMemDC = CreateMemoryDC(ps.rcPaint.right - ps.rcPaint.left,
				ps.rcPaint.bottom-ps.rcPaint.top);
			
			
			if(ReadSuccess)
			{
				BitBlt(hMemDC, WND_X,WND_Y,
					COVER_WIDTH , COVER_HEIGHT , 
					(HDC)hPushBoxCover, 0, 0, ROP_SRC);
				
				OldClr = SetBkColor(hMemDC,clr4);
				OldStyle = SetBkMode(hMemDC, NEWTRANSPARENT);
				
				/*
					BitBlt(hMemDC, 
										COVERWORDS_X + DIFFER_X,
										COVERWORDS_Y + DIFFER_Y,
										COVERWORDS_WIDTH,
										COVERWORDS_HEIGHT, 
										(HDC)hPushBoxCoverWords,
										0, 0,
										ROP_SRC);*/
				/*
				BitBlt(hMemDC, 
					COVERWORDS_X + DIFFER_X,
					COVERWORDS_Y + DIFFER_Y,
					COVERWORDS_WIDTH,
					COVERWORDS_HEIGHT, 
					(HDC)hPushBoxCoverWords,
					0, 0,
					ROP_SRC);
					
				*/
				switch(nSelMenuItem)
				{
/*
									case 0:
										COVERMAN_X = COVERFIRSTMAN_X;
										COVERMAN_Y = COVERFIRSTMAN_Y;
										break;
					
									case 1: 
										COVERMAN_X = COVERFIRSTMAN_X + MENU_WORDSIZE;
										COVERMAN_Y = COVERFIRSTMAN_Y + nSelMenuItem*MENU_WORDSIZE;
										break;
					
									case 2: 
										COVERMAN_X = COVERFIRSTMAN_X;
										COVERMAN_Y = COVERFIRSTMAN_Y + nSelMenuItem*MENU_WORDSIZE;
										break;
					
									case 3: 
										COVERMAN_X = COVERFIRSTMAN_X + MENU_WORDSIZE;
										COVERMAN_Y = COVERFIRSTMAN_Y + nSelMenuItem*MENU_WORDSIZE;
										break;*/
				case 0:
					COVERMAN_X = COVERFIRSTMAN_X;
					COVERMAN_Y = COVERFIRSTMAN_Y;
					break;
					
				case 1: 
					COVERMAN_X = COVERFIRSTMAN_X + MENU_WORDSIZE;
					COVERMAN_Y = COVERFIRSTMAN_Y + nSelMenuItem*MENU_WORDSIZE;
					break;
					
				case 2: 
					COVERMAN_X = COVERFIRSTMAN_X;
					COVERMAN_Y = COVERFIRSTMAN_Y + nSelMenuItem*MENU_WORDSIZE;
					break;
					
				case 3: 
					COVERMAN_X = COVERFIRSTMAN_X + MENU_WORDSIZE;
					COVERMAN_Y = COVERFIRSTMAN_Y + nSelMenuItem*MENU_WORDSIZE;
					break;

				}			
				
				SetBkColor(hMemDC, clrCover);
				SetBkMode(hMemDC, NEWTRANSPARENT);
				
/*
				BitBlt(hMemDC, 
					COVERMAN_X,
					COVERMAN_Y,
					COVERMAN_WIDTH,
					COVERMAN_HEIGHT, 
					(HDC)hPushBoxCoverMan,
					0, 0,
					ROP_SRC); 
*/
				SetBkColor(hMemDC,OldClr);
				SetBkMode(hMemDC,OldStyle);
			}			
			else
			{
				RECT rect;
				char s[20] = "Failed to read map!";
				GetClientRect(hWnd, &rect);
				OldStyle = SetBkMode(hMemDC, TRANSPARENT);
				DrawText(hMemDC, s, -1, &rect, DT_CENTER|DT_VCENTER);
				SetBkMode(hMemDC, OldStyle);
			}

			BitBlt(hdc, 0, 0, ps.rcPaint.right - ps.rcPaint.left, 
				ps.rcPaint.bottom-ps.rcPaint.top, hMemDC, 0, 0, ROP_SRC);

			DeleteDC(hMemDC);
			
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_CLOSE:
		
		//-->退出应用程序
		DestroyWindow(hWnd);
		//UnregisterClass("PushboxMainWndClass", NULL);
#ifdef _MODULE_DEBUG_
  EndObjectDebug();
#endif
		break;
		
	case WM_DESTROY:
		
		if (NULL != hPushBoxCover)
		{
			DeleteObject(hPushBoxCover);
			hPushBoxCover = NULL;
		}

		//DeleteObject(hPushBoxCoverWords);
		//DeleteObject(hPushBoxCoverMan);

		WriteLevelToFile();
		free(pChar);
		hPushBoxWnd = NULL;
		UnregisterClass("PushboxMainWndClass", NULL);
		DlmNotify(PES_STCQUIT, (long)hInst);
/*
#ifdef	SCP_SMARTPHONE
		DlmNotify(PES_STCQUIT, (long)GamePushBox_AppControl);
#else
		DlmNotify((WPARAM)PES_APP_QUIT, (LPARAM)hInst);
#endif
*/		
		break;
		
	default:
		
		lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
		
		break;
	}
		
	return lResult;
		
}


/*********************************************************************\
* Function：游戏的设置界面窗口过程	   
* Purpose ：游戏设置界面     
* Params ：	ParenthWnd   
* Return ： DWORD	 	   
* Remarks ：	   
**********************************************************************/
static	DWORD	PushBoxOptionSetupProc(HWND ParenthWnd)
{
	DWORD dwRet;
	WNDCLASS wc;

    dwRet = TRUE;

	if ( !bRSetupCLA )
	{
		wc.style		  = CS_OWNDC;//CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc	  = (WNDPROC)PushBoxOptionSetupWndProc;
		wc.cbClsExtra	  = 0;
		wc.cbWndExtra	  = 0;
		wc.hInstance	  = NULL;
		wc.hIcon		  = NULL;
		wc.hbrBackground  = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName	  = NULL;
		wc.lpszClassName  = "PushBoxOptionSetupWndClass";		

		if (!RegisterClass(&wc))
		{
			bRSetupCLA = FALSE;
			return FALSE;
		}
		else
			bRSetupCLA = TRUE;
	}
	
	    hPushBoxOptionSetupWnd = CreateWindow(
        "PushBoxOptionSetupWndClass", 
        IDS_SELECT, 
        WS_VISIBLE|WS_CAPTION|PWS_STATICBAR,
/*        WND_BORDER_X,
		WND_BORDER_Y,
		WND_BORDER_LEN,
		WND_BORDER_HEIGHT,
  */    PLX_WIN_POSITION,  
		ParenthWnd, 
		NULL,
		NULL,
		NULL
	);

	if ( hPushBoxOptionSetupWnd == NULL )
	{
		LevelSetupCreate = FALSE;
		return FALSE;
	}
	else
		LevelSetupCreate = TRUE;

	ShowWindow(hPushBoxOptionSetupWnd, SW_SHOW);
	UpdateWindow(hPushBoxOptionSetupWnd);

    return dwRet;
}


/*********************************************************************\
* Function：游戏的设置界面窗口过程的消息处理函数	   
* Purpose ：游戏设置界面     
* Params ：	hWnd, message, wParam, lParam   
* Return ： LRESULT	 	   
* Remarks ：	   
**********************************************************************/
LRESULT PushBoxOptionSetupWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	int index;
//	RECT	txtRect;

	switch (message)
	{

	case WM_CREATE:
		{
			int i;
			
			//#if defined(SCP_SMARTPHONE)
			SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
			//#endif
			
			//游戏选关设置
			hLevelCombo= CreateWindow(
				"SPINBOXEX", 
				IDS_LEVEL_SPEED, 
				WS_CHILD | WS_VISIBLE| WS_TABSTOP|SSBS_ARROWRIGHT|SSBS_CENTER,
				0,
				50,
				176,
				50,
				hWnd,
				(HMENU)ID_LEVEL_COMBO,
				NULL, 
				NULL
				);
			
			for (i = 0; i < LEVEL_COUNT; i++) //共十局
			{
				/*
				strcpy(LevelString[i],GetString(STR_PUSHBOX_NO1+i));
				SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
				*/
				switch(i)
				{
				case 0:
					strcpy(LevelString[i],ML("Level1"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				case 1:
					strcpy(LevelString[i],ML("Level2"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				case 2:
					strcpy(LevelString[i],ML("Level3"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				case 3:
					strcpy(LevelString[i],ML("Level4"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				case 4:
					strcpy(LevelString[i],ML("Level5"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				case 5:
					strcpy(LevelString[i],ML("Level6"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				case 6:
					strcpy(LevelString[i],ML("Level7"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				case 7:
					strcpy(LevelString[i],ML("Level8"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				case 8:
					strcpy(LevelString[i],ML("Level9"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				case 9:
					strcpy(LevelString[i],ML("Level10"));
					SendMessage(hLevelCombo, SSBM_ADDSTRING, NULL, (LPARAM)LevelString[i]);
					break;
				default:
					break;
				}

			}
			
			//SendMessage(hLevelCombo, SSBM_SETCURSEL, (WPARAM)(curnLevel-1), NULL);
			SendMessage(hLevelCombo, SSBM_SETTEXT, (WPARAM)LevelString[curnLevel-1], NULL);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)IDS_CONFIRM);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0), (LPARAM)IDS_RETURN);
			
		}
		
		break;

	case WM_COMMAND:

		switch(LOWORD(wParam))
		{
		case ID_LEVEL_COMBO:

			SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKUP|MASKDOWN);

			break;

		case IDM_CONFIRM:

			hdc = GetDC(hWnd);
			index = SendMessage(hLevelCombo, SSBM_GETCURSEL, NULL, NULL);
			curnLevel = index+1;
			ReadBoxMap(curnLevel);
			FreeList(&ppList);
			Distance = -1;
			BackCount = 0;
//			MoveNum = 0;
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);

			if( !GameCreate )
			{
				if(!PushBoxGameProc(hPushBoxOptionSetupWnd))
					return FALSE;
			}
			else
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			    ShowGameWindowAnew(hPushBoxGameWnd);
			}

			ReleaseDC(hWnd, hdc);
			
			break;

		case IDM_EXIT:

			PostMessage(hWnd, WM_CLOSE, 0, 0);

			break;
		}

		break;

	case WM_ACTIVATE:

		/*if (wParam == WA_ACTIVE)
			SetFocus(hLevelCombo);
		else
			SetFocus((HWND)lParam);*/
		if (wParam == WA_ACTIVE)
			SetFocus(hLevelCombo);
		break;
/*			
	case WM_PAINT:
		
		SetRect(&txtRect,30,50,130,75);

		hdc = BeginPaint(hWnd, NULL);
		//TextOut(hdc,LEVEL_TEXT_X + DIFFER_X, LEVEL_TEXT_Y + DIFFER_Y, IDS_LEVEL_SPEED,12);
		DrawText(hdc, IDS_LEVEL_SPEED, -1, &txtRect, DT_VCENTER|DT_CENTER);
		EndPaint(hWnd, NULL);

		break;
*/			
	case WM_CLOSE:

		DestroyWindow(hWnd);
        UnregisterClass("PushBoxOptionSetupWndClass",NULL);
		bRSetupCLA = FALSE;

		break;
			
	case WM_DESTROY:

		hWnd = NULL;
//		UnregisterClass("PushBoxOptionSetupWndClass", NULL);
		LevelSetupCreate = FALSE;

		break;
		
	case WM_KEYDOWN:

		switch (wParam)
		{
//		case VK_F2:
//
////			DestroyWindow(hWnd);
//			PostMessage(hWnd, WM_CLOSE, 0, 0);
//
//			break;
		
		case VK_F10:

			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case VK_RETURN:
			{
				hdc = GetDC(hWnd);
				index = SendMessage(hLevelCombo, SSBM_GETCURSEL, NULL, NULL);
				curnLevel = index+1;
				ReadBoxMap(curnLevel);
				FreeList(&ppList);
				Distance = -1;
				BackCount = 0;
				//			MoveNum = 0;
				
				if( !GameCreate )
				{
					if(!PushBoxGameProc(hPushBoxOptionSetupWnd))
						return FALSE;
				}
				else
				{
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					ShowGameWindowAnew(hPushBoxGameWnd);
				}
				
				ReleaseDC(hWnd, hdc);
			}
			
			break;
			
		default:
			return PDADefWindowProc(hWnd, message, wParam, lParam);
		}
		
		break;

	default:
		return PDADefWindowProc(hWnd, message, wParam, lParam);

		break;
	}

	return 0;
	
}


/*********************************************************************\
* Function：游戏界面窗口过程	   
* Purpose ：游戏界面     
* Params ：	ParenthWnd   
* Return ： DWORD	 	   
* Remarks ：	   
**********************************************************************/
static	DWORD	PushBoxGameProc(HWND ParenthWnd)
{
	DWORD dwRet;
	WNDCLASS wc;

    dwRet = TRUE;

	if ( !bRGameCLA )
	{
		wc.style		  = CS_OWNDC;
		wc.lpfnWndProc	  = (WNDPROC)PushBoxGameWndProc;
		wc.cbClsExtra	  = 0;
		wc.cbWndExtra	  = 0;
		wc.hInstance	  = NULL;
		wc.hIcon		  = NULL;
		wc.hbrBackground  = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName	  = NULL;
		wc.lpszClassName  = "PushBoxGameWndClass";		

		if (!RegisterClass(&wc))
		{
			bRGameCLA = FALSE;
			return FALSE;
		}
		else
			bRGameCLA = TRUE;
	}

    //hMenu = LoadMenuIndirect((PMENUTEMPLATE)&PushBoxGameMenuTemplate);


	hPushBoxGameWnd = CreateWindow("PushBoxGameWndClass",
	IDS_PUSHBOX,
	WS_VISIBLE|PWS_STATICBAR,
	PLX_WIN_POSITION,
	ParenthWnd,
	NULL,//(HMENU)hMenu, 
	NULL, 
	NULL
	);
	
	if (!hPushBoxGameWnd)
	{
		GameCreate = FALSE;
		return FALSE;
	}
	else
		GameCreate = TRUE;

//#ifdef SCP_SMARTPHONE
	/*hMenu = GetMenu(hPushBoxGameWnd);
	ModifyMenu(hMenu,ID_MENU_BACK,MF_BYCOMMAND|MF_STRING,ID_MENU_BACK,IDS_MENU_BACK);
	ModifyMenu(hMenu,ID_MENU_REPLAY,MF_BYCOMMAND|MF_STRING,ID_MENU_REPLAY,IDS_MENU_REPLAY);*/
//#endif

	ShowWindow(hPushBoxGameWnd, SW_SHOW);
	UpdateWindow(hPushBoxGameWnd);

	return dwRet;

}


/*********************************************************************\
* Function：游戏的游戏界面窗口过程的消息处理函数	   
* Purpose ：游戏界面     
* Params ：	hWnd, message, wParam, lParam   
* Return ： LRESULT	 	   
* Remarks ：	   
**********************************************************************/
LRESULT PushBoxGameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message) 
	{
	case WM_CREATE:
		{
			COLORREF	clr;
			

			SendMessage(hWndHelp, PWM_SETAPPICON, 0, (LPARAM)DIRICON);

			hdc = GetDC(hWnd);
			bTimerAlready = FALSE;
		    FreeList(&ppList);
			Distance = -1;
			BackCount = 0;
//			MoveNum = 0;
		    FindMan();//找到小人儿的当前位置

			hPushBoxbg = CreateBitmapFromImageFile(hdc, PushBoxbg, &clr, &bclarity);
			hPushBoxArea = CreateBitmapFromImageFile(hdc, PushBoxArea, &clr, &bclarity);
			hPushBoxBox = CreateBitmapFromImageFile(hdc, PushBoxBox, &clr, &bclarity);
			hPushBoxWall = CreateBitmapFromImageFile(hdc, PushBoxWall, &clr, &bclarity);
			hPushBoxTemini = CreateBitmapFromImageFile(hdc, PushBoxTemini, &clr, &bclarity);
			hPushBoxMan1 = CreateBitmapFromImageFile(hdc, PushBoxMan1, &clr1, &bclarity);
//			hPushBoxMan2 = CreateBitmapFromImageFile(hdc, PushBoxMan2, &clr1, &bclarity);

		    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0), (LPARAM)IDS_RETURN);// 退出按钮
		    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CANCEL,1), (LPARAM)IDS_BACK);// 退回按钮
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_REPLAY,2), (LPARAM)IDS_REPLAY); //重玩按钮
			
			ReleaseDC(hWnd,hdc);
		}	
		break;

//	case WM_INITMENU:
//		{
//			if(BackCount == 0)
//				ModifyMenu(hMenu,ID_MENU_BACK,MF_BYCOMMAND|MF_GRAYED|MF_STRING,ID_MENU_BACK,IDS_MENU_BACK);
//			else
//				ModifyMenu(hMenu,ID_MENU_BACK,MF_BYCOMMAND|MF_ENABLED|MF_STRING,ID_MENU_BACK,IDS_MENU_BACK);
//		}
//		break;

	case WM_PAINT://显示地图
		{
			hdc = BeginPaint(hWnd, &ps);

			DrawBackGround(hdc);

			CenterShow(hWnd, hdc);
			ShowMap(hdc);//根据每次刷新后的地图，显示地图
			IsWhatAroundMan(hWnd);
			EndPaint(hWnd, &ps);
		}
		
		break;

	case IDRM_GAMEOVER:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case IDRM_SELNEXT:
		if (lParam == 1)
		{
			curnLevel++;
			ShowGameWindowAnew(hPushBoxGameWnd);
		}
		else
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case IDRM_RMSG:
		if (lParam == 1)
		{
			if(hLevelCombo)
				SendMessage(hLevelCombo, SSBM_SETCURSEL, (WPARAM)(curnLevel-1), NULL);						
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{		
		case VK_F10:
			 PLXConfirmWinEx(NULL, hWnd, IDS_GAME_PROMPT, Notify_Request, IDS_PUSHBOX, IDS_CONFIRM, ML("Cancel"), IDRM_RMSG);
			
			break;
			
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			{
				if(Distance >= 0)
					break;
				hdc = GetDC(hWnd);
				PushBoxControlMoveK(hWnd, hdc, wParam);//推箱子游戏中响应键盘消息的控制函数
				IsWhatAroundMan(hWnd);
				ReleaseDC(hWnd, hdc);
				
				if (IsGameOver())
				{
					if( curnLevel< LEVEL_COUNT)
					{
						if( !SuccessCreate )
						{
//							if( !PushBoxSuccessProc(hPushBoxGameWnd) )
//								return FALSE;
							PLXConfirmWinEx(NULL, hWnd, IDS_NEXMISSON, Notify_Request, IDS_PUSHBOX, ML("Yes"), ML("No"), IDRM_SELNEXT);
						}
					}
					else
					{
						if( !EndGameCreate )
						{
//							if( !PushBoxEndGameProc(hPushBoxGameWnd) )
//								return FALSE;
							PLXTipsWin(NULL, hWnd, IDRM_GAMEOVER, IDS_GAMEOVER, IDS_PUSHBOX, Notify_Info, ML("Ok"), NULL, WAITTIMEOUT);
						}
					}
					
					
				}
			}
				
			break;


		default:
			{
				return PDADefWindowProc(hWnd, message, wParam, lParam);
			}
			
			break;

		}

		break;
	
	case WM_PENDOWN:
		{
			if(PushBoxControlMoveP(hWnd, lParam) == 1)
				break;

			if (IsGameOver())
			{
				if( curnLevel< LEVEL_COUNT)
				{
					if( !SuccessCreate )
					{
						if( !PushBoxSuccessProc(hPushBoxGameWnd) )
							return FALSE;
					}
				}
				else
				{
					if( !EndGameCreate )
					{
						if( !PushBoxEndGameProc(hPushBoxGameWnd) )
							return FALSE;
					}
				}
			}
		}

		break;
	
	case WM_TIMER:
		{
			switch(wParam)
			{
			case TIMER_IDM://从起点到终点把已找到的最短路径一步一步地显示出来
				{
					short x, y;
					HDC hdc;
					
					hdc = GetDC(hWnd);
					
					if(Distance >= 0)
					{
						y = ShortPath[Distance] / line;
						x = ShortPath[Distance] % line;
						DrawAutoMoveMan(hdc, x, y);
						Distance--;
					}
					if(Distance == -1)
					{
						KillTimer(hWnd, TIMER_IDM);
						bTimerAlready = FALSE;
					}
					
					ReleaseDC(hWnd, hdc);
				}
				break;
			}
		}

		break;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDM_CANCEL:
				{
					ListUnit *p;
					p = ppList;
					hdc = GetDC(hWnd);
					if(ppList)
						ComeBack(hdc, p);
					ReleaseDC(hWnd, hdc);
			
				}

				break;
			case IDM_REPLAY:
				{
					hdc = GetDC(hWnd);
					
					FreeList(&ppList);
					Distance = -1;
					BackCount = 0;
					//				MoveNum = 0;
					
					ReadBoxMap(curnLevel);
					FindMan();
					
					DrawBackGround(hdc);
					
					CenterShow(hWnd, hdc);
					
					ReleaseDC(hWnd, hdc);
					
					InvalidateRect(hWnd, NULL, FALSE);
				}
				
				break;

			case IDM_EXIT:
				{
					 PLXConfirmWinEx(NULL, hWnd, IDS_GAME_PROMPT, Notify_Request, IDS_PUSHBOX, IDS_CONFIRM, 
						ML("Cancel"), IDRM_RMSG);


				}

/*				DestroyWindow(hWnd);

				if(hLevelCombo)
					SendMessage(hLevelCombo, SSBM_SETCURSEL, (WPARAM)(curnLevel-1), NULL);
*/
				break;
			
			}
		}
		break;

	case WM_SELOK:
		{
			if(hLevelCombo)
				SendMessage(hLevelCombo, SSBM_SETCURSEL, (WPARAM)(curnLevel-1), NULL);
			
			//		DestroyWindow(hWnd);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}

		break;

	case WM_SELCANCEL:
	
		break;

	case WM_CLOSE:
		
		{
			DestroyWindow(hWnd);
			UnregisterClass("PushBoxGameWndClass",NULL);
			bRGameCLA = FALSE;	
			hPushBoxGameWnd = NULL;
		}

		break;

	case WM_DESTROY:
		{
			DeleteObject(hPushBoxbg);
			DeleteObject(hPushBoxArea);
			DeleteObject(hPushBoxBox);
			DeleteObject(hPushBoxWall);
			DeleteObject(hPushBoxTemini);
			DeleteObject(hPushBoxMan1);
			//DestroyMenu(hMenu);
			//		DeleteObject(hPushBoxMan2);
			
			FreeList(&ppList);
			Distance = -1;
			BackCount = 0;
			//		MoveNum = 0;
			
			hWnd = NULL;
			//		UnregisterClass("PushBoxGameWndClass", NULL);
			GameCreate = FALSE;
			
		}

		break;

	default:
		{
			return PDADefWindowProc(hWnd, message, wParam, lParam);
		}
		

		break;
	}

	return 0;
}


/*********************************************************************\
* Function：游戏过关界面窗口过程	   
* Purpose ：游戏过关界面     
* Params ：	ParenthWnd   
* Return ： DWORD	 	   
* Remarks ：	   
**********************************************************************/
static	DWORD	PushBoxSuccessProc(HWND ParenthWnd)
{
	DWORD dwRet;
	WNDCLASS wc;

    dwRet = TRUE;

	if ( !bRSuccessCLA )
	{
		wc.style		  = CS_OWNDC;
		wc.lpfnWndProc	  = (WNDPROC)PushBoxSuccessWndProc;
		wc.cbClsExtra	  = 0;
		wc.cbWndExtra	  = 0;
		wc.hInstance	  = NULL;
		wc.hIcon		  = NULL;
		wc.hbrBackground  = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName	  = NULL;
		wc.lpszClassName  = "PushBoxSuccessWndClass";		

		if (!RegisterClass(&wc))
		{
			bRSuccessCLA = FALSE;
			return FALSE;
		}
		else
			bRSuccessCLA = TRUE;
	}

	if (IsWindow(hPushBoxSuccessWnd)) {
		ShowWindow(hPushBoxSuccessWnd, SW_SHOW);
		SetFocus(hPushBoxSuccessWnd);
		return TRUE;
	}
	hPushBoxSuccessWnd = CreateWindow("PushBoxSuccessWndClass",
	NULL,
	WS_VISIBLE|PWS_STATICBAR,//|WS_CAPTION
/*
	WND_BORDER_X,
	WND_BORDER_Y,
	WND_BORDER_LEN,
	WND_BORDER_HEIGHT,
*/PLX_WIN_POSITION,
	ParenthWnd,
	NULL, 
	NULL, 
	NULL
	);
	
	if (!hPushBoxSuccessWnd)
	{
		SuccessCreate = FALSE;
		return FALSE;
	}
	else
		SuccessCreate = TRUE;
	
	SendMessage(hPushBoxSuccessWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)IDS_CONFIRM);//(LPARAM)IDS_EXIT);//"退出" 按纽
	SendMessage(hPushBoxSuccessWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0), (LPARAM)IDS_RETURN);//(LPARAM)IDS_MENU_OK);//确定 按纽

	ShowWindow(hPushBoxSuccessWnd,SW_SHOW);
	UpdateWindow(hPushBoxSuccessWnd);

	return dwRet;

}


/*********************************************************************\
* Function：游戏的过关界面窗口过程的消息处理函数	   
* Purpose ：游戏过关界面     
* Params ：	hWnd, message, wParam, lParam   
* Return ： LRESULT	 	   
* Remarks ：	   
**********************************************************************/
LRESULT PushBoxSuccessWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
//	RECT rect;
	PAINTSTRUCT ps;
//	char s[100];
	
	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(hWnd);

//#if defined(SCP_SMARTPHONE)
		//SendMessage(hWndHelp, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
//#endif

//#ifdef	SCP_SMARTPHONE
			hPushBoxPassWords = CreateBitmapFromImageFile(hdc, ML("/rom/game/pushbox/pb_passwe.gif"), &clr2, &bclarity);
/*else
			hPushBoxPassWords = CreateBitmapFromImageFile(hdc, PushBoxPassWords, &clr2, &bclarity);
#endif*/
			ReleaseDC(hWnd,hdc);
		}
		break;

	case WM_PAINT:
		{
			COLORREF	OldClr;
			int			OldStyle;
			
			hdc = BeginPaint(hWnd, &ps);
			
			DrawBackGround(hdc);
			
			OldClr = SetBkColor(hdc,clr2);
			OldStyle = SetBkMode(hdc, NEWTRANSPARENT);
			
			BitBlt(hdc, 
				WND_X ,
				WND_Y ,
				WND_WIDTH,  
				WND_HEIGHT, 
				(HDC)hPushBoxPassWords, 
				0, 0, 
				ROP_SRC);

			SetBkColor(hdc,OldClr);
			SetBkMode(hdc,OldStyle);
			
			EndPaint(hWnd, &ps);
		}

		break;
		
	case WM_KEYDOWN:

		switch(LOWORD(wParam))
		{
		case VK_F10:
//		case VK_F2:

//			DestroyWindow(hWnd);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
//			DestroyWindow(hPushBoxGameWnd);
			PostMessage(hPushBoxGameWnd, WM_CLOSE, 0, 0);
			
			if(!LevelSetupCreate)
			{
				if(!PushBoxOptionSetupProc(hPushBoxWnd))
					return FALSE;
			}
			
            break;

		
		

		case VK_RETURN:
			
			{
				curnLevel++;
				
				if(hLevelCombo)
					SendMessage(hLevelCombo, SSBM_SETCURSEL, (WPARAM)(curnLevel-1), NULL);
				
				//					DestroyWindow(hWnd);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				if(curnLevel <= LEVEL_COUNT)
					ShowGameWindowAnew(hPushBoxGameWnd);
			}
						
			break;

		default:
			return PDADefWindowProc(hWnd, message, wParam, lParam);
		
			break;
		}

		break;
/*
	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDM_CONFIRM:
				{
					curnLevel++;
					
					if(hLevelCombo)
						SendMessage(hLevelCombo, SSBM_SETCURSEL, (WPARAM)(curnLevel-1), NULL);
					
//					DestroyWindow(hWnd);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					if(curnLevel <= LEVEL_COUNT)
						ShowGameWindowAnew(hPushBoxGameWnd);
				}

				break;
				
			case IDM_EXIT:

//				DestroyWindow(hWnd);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
//				DestroyWindow(hPushBoxGameWnd);
				PostMessage(hPushBoxGameWnd, WM_CLOSE, 0, 0);

				if(LevelSetupCreate)
				{
//					DestroyWindow(hPushBoxOptionSetupWnd);
					PostMessage(hPushBoxOptionSetupWnd, WM_CLOSE, 0, 0);
				}
				if(!LevelSetupCreate)
				{
					if(!PushBoxOptionSetupProc(hPushBoxWnd))
						return FALSE;
				}
				
				break;
			}
			
		}
		break;
*/
	case WM_CLOSE:

		DestroyWindow(hWnd);
        UnregisterClass("PushBoxSuccessWndClass",NULL);
		bRSuccessCLA = FALSE;

		break;
		
	case WM_DESTROY: 

		DeleteObject(hPushBoxPassWords);

		hWnd = NULL;
//		UnregisterClass("PushBoxSuccessWndClass", NULL);
		SuccessCreate = FALSE;
			
		break;
		
	default:
		return PDADefWindowProc(hWnd, message, wParam, lParam);
		
		break;
	}
	return 0;
}


/*********************************************************************\
* Function：游戏关底界面窗口过程	   
* Purpose ：游戏关底界面     
* Params ：	ParenthWnd   
* Return ： DWORD	 	   
* Remarks ：	   
**********************************************************************/
static	DWORD	PushBoxEndGameProc(HWND ParenthWnd)
{
	DWORD dwRet;
	WNDCLASS wc;

    dwRet = TRUE;

	if ( !bREndgameCLA )
	{
		wc.style		  = CS_OWNDC;
		wc.lpfnWndProc	  = (WNDPROC)PushBoxEndGameWndProc;
		wc.cbClsExtra	  = 0;
		wc.cbWndExtra	  = 0;
		wc.hInstance	  = NULL;
		wc.hIcon		  = NULL;
		wc.hbrBackground  = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName	  = NULL;
		wc.lpszClassName  = "PushBoxEndGameWndClass";		

		if (!RegisterClass(&wc))
		{
			bREndgameCLA = FALSE;
			return FALSE;
		}
		else
			bREndgameCLA = TRUE;
	}

	hPushBoxEndGameWnd = CreateWindow("PushBoxEndGameWndClass",
	IDS_PUSHBOX,
	WS_VISIBLE|PWS_STATICBAR|WS_CAPTION,
/*
	WND_BORDER_X,
	WND_BORDER_Y,
	WND_BORDER_LEN,
	WND_BORDER_HEIGHT,
*/
	PLX_WIN_POSITION,
	ParenthWnd,
	NULL, 
	NULL, 
	NULL
	);
	
	if (!hPushBoxEndGameWnd)
	{
		EndGameCreate = FALSE;
		return FALSE;
	}
	else
		EndGameCreate = TRUE;
	
	SendMessage(hPushBoxEndGameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)IDS_CONFIRM);//(LPARAM)IDS_EXIT);//"退出" 按纽
	SendMessage(hPushBoxEndGameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0), (LPARAM)IDS_RETURN);//(LPARAM)IDS_MENU_OK);//确定 按纽

	ShowWindow(hPushBoxEndGameWnd,SW_SHOW);
	UpdateWindow(hPushBoxEndGameWnd);

	return dwRet;

}


/*********************************************************************\
* Function：游戏的关底界面窗口过程的消息处理函数	   
* Purpose ：游戏关底界面     
* Params ：	hWnd, message, wParam, lParam   
* Return ： LRESULT	 	   
* Remarks ：	   
**********************************************************************/
LRESULT PushBoxEndGameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
	case WM_CREATE:
		{
			hdc = GetDC(hWnd);

//#if defined(SCP_SMARTPHONE)
		SendMessage(hWndHelp, PWM_SETAPPICON, 0, (LPARAM)DIRICON);
//#endif

//#ifdef	SCP_SMARTPHONE
			hPushBoxEndGameWords = CreateBitmapFromImageFile(hdc, ML("/rom/game/pushbox/pb_endwe.gif"), &clr3, &bclarity);
//#else
//			hPushBoxEndGameWords = CreateBitmapFromImageFile(hdc, PushBoxEndGameWords, &clr3, &bclarity);
//#endif

			ReleaseDC(hWnd,hdc);
		}
		break;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case IDM_CONFIRM:
				{
//					DestroyWindow(hWnd);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					
					curnLevel = 1;
					
					if(hLevelCombo)
						SendMessage(hLevelCombo, SSBM_SETCURSEL, (WPARAM)(curnLevel-1), NULL);
					
					ShowGameWindowAnew(hPushBoxGameWnd);
				}
				break;

			case IDM_EXIT://退出该窗口和游戏窗口 

//				DestroyWindow(hWnd);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
//				DestroyWindow(hPushBoxGameWnd);	
				PostMessage(hPushBoxGameWnd, WM_CLOSE, 0, 0);
				
				if( LevelSetupCreate )
				{
//					DestroyWindow(hPushBoxOptionSetupWnd);
					PostMessage(hPushBoxOptionSetupWnd, WM_CLOSE, 0, 0);
				}
				
				break;
			}
		}
		
		break;
/*		
	case WM_KEYDOWN:
		
		switch(LOWORD(wParam))
		{
		case VK_F2:
			
//			DestroyWindow(hWnd);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
//			DestroyWindow(hPushBoxGameWnd);
			PostMessage(hPushBoxGameWnd, WM_CLOSE, 0, 0);
			
			if( LevelSetupCreate )
			{
//				DestroyWindow(hPushBoxOptionSetupWnd);
				PostMessage(hPushBoxOptionSetupWnd, WM_CLOSE, 0, 0);
			}
			
            break;
			
		default:
			return PDADefWindowProc(hWnd, message, wParam, lParam);
			
			break;
		}
		
		break;
*/		
	case WM_PAINT:
		{
			COLORREF	OldClr;
			int			OldStyle;
			
			hdc = BeginPaint(hWnd, &ps);
			
			DrawBackGround(hdc);
			
			OldClr = SetBkColor(hdc,clr3);
			OldStyle = SetBkMode(hdc, NEWTRANSPARENT);
			
			BitBlt(
				hdc, 
				WND_X,
				WND_Y,
				WND_WIDTH,
				WND_HEIGHT, 
				(HDC)hPushBoxEndGameWords, 
				0, 0, 
				ROP_SRC);
			
			SetBkColor(hdc,OldClr);
			SetBkMode(hdc,OldStyle);
			
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_CLOSE:

		DestroyWindow(hWnd);
        UnregisterClass("PushBoxEndGameWndClass",NULL);
		bREndgameCLA = FALSE;

		break;

	case WM_DESTROY: 
		
		DeleteObject(hPushBoxEndGameWords);

		hWnd = NULL;
//		UnregisterClass("PushBoxEndGameWndClass", NULL);
		EndGameCreate = FALSE;
		
		break;
		
	default:
		return PDADefWindowProc(hWnd, message, wParam, lParam);
		
		break;
		
	}
	return 0;
	
}


/*****************************************************************************\
* Funcation: MouseGameHelpProc
*
* Purpose  : 游戏说明界面
*
* Explain  : 注册并建立窗口
\*****************************************************************************/
static	DWORD	PushBoxGameHelpProc(HWND hWnd)
{
	DWORD dwRet;
    
	dwRet = TRUE;
	
	CallGameHelpWnd(hWnd, IDS_HELPTEXT);
	return dwRet;
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
	SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLLEFT|SCROLLRIGHT, MASKLEFT|MASKRIGHT);

	if (MENU_ITEM_NUM-1 == nSel)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKRIGHT);
	if (0 == nSel)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKLEFT);

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
			if (    x > (FIRSTMENU_X + MENU_WORDSIZE)
				&&  x < (FIRSTMENU_X + COVERWORDS_WIDTH)
				&&  y > (FIRSTMENU_Y + i * MENU_WORDSIZE)
				&&  y < (FIRSTMENU_Y + (i+1) * MENU_WORDSIZE)
				)
				return i;
		}
		else
		{			
			if (   x > FIRSTMENU_X
				&& x < (FIRSTMENU_X + COVERWORDS_WIDTH - MENU_WORDSIZE)
				&& y > (FIRSTMENU_Y + i * MENU_WORDSIZE)
				&& y < (FIRSTMENU_Y + (i+1)*MENU_WORDSIZE)
				)
				return i;
		}
	}
	return -1;
/*	if ((X_FIRSTLU)<x && x<(X_FIRSTRD) && y>(Y_FIRSTLU) && y<(Y_FIRSTRD)) 
		return 0;
	else if ((X_SECONDLU)<x && x<(X_SECONDRD) && y>(Y_SECONDLU) && y<(Y_SECONDRD)) 
		return 1;
	else if ((X_THIRDLU)<x && x<(X_THIRDRD) && y>(Y_THIRDLU) && y<(Y_THIRDRD)) 
		return 2;
	else if ((X_FOURTHLU)<x && x<(X_FOURTHRD) && y>(Y_FOURTHLU) && y<(Y_FOURTHRD)) 
		return 3;
	else
		return -1;
*/
}


/*********************************************************************\
* Function	   MyDrawMenuItem
* Purpose      画菜单项
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
/*static BOOL MyDrawMenuItem(HDC hdc,int i,int nSel)
{
#define	SEL_COLOR	RGB(0,0,255)

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
		rect.left = X_FIRSTRD_MENUITEM;
		rect.right = X_FIRSTRD_MENUITEM+X_MENUITEM_INTERVAL;
		rect.top = Y_FIRSTLU_MENUITEM + i*Y_MENUITEM_INTERVAL;
		rect.bottom = Y_FIRSTRD_MENUITEM + i*Y_MENUITEM_INTERVAL;
	}
	else
	{
		rect.left = X_FIRSTLU_MENUITEM;
		rect.right = X_FIRSTLU_MENUITEM+X_MENUITEM_INTERVAL;
		rect.top = Y_FIRSTLU_MENUITEM + i*Y_MENUITEM_INTERVAL;
		rect.bottom = Y_FIRSTRD_MENUITEM + i*Y_MENUITEM_INTERVAL;
	}

	OldClr = SetBkColor(hdc,COLOR_WHITE);
	OldStyle = SetBkMode(hdc, TRANSPARENT);

	if (i == nSel)
		OldTextClr = SetTextColor(hdc,RGB(255,0,0));//RGB(135,143,252)
	
	else
		OldTextClr = SetTextColor(hdc,RGB(0,0,255));

	DrawText(hdc, MenuItemText[i], -1, &rect, DT_VCENTER | DT_HCENTER);

	SetTextColor(hdc,OldTextClr);

	SetBkColor(hdc, OldClr);
	SetBkMode(hdc, OldStyle);

	return TRUE;
}
*/

/*********************************************************************\
* Function ：负责箱子的移动，根据不同方向移动一格	   
* Purpose ：移动模块的实现    
* Params ：Direction（方向）， j（坐标x值），  i（坐标y值） 	   
* Return ：POINT	 	   
* Remarks ：	   
**********************************************************************/
static	POINT	MoveBox(char Direction, int j, int i)
{
	POINT curPoint;

	if (Direction == 'U')
	{
		curPoint.x = j;
		curPoint.y = i-1;
	}

	else if (Direction == 'D')
	{
		curPoint.x = j;
		curPoint.y = i+1;
	}

	else if (Direction == 'L')
	{
		curPoint.x = j-1;
		curPoint.y = i;
	}

	else if (Direction == 'R')
	{
		curPoint.x = j+1;
		curPoint.y = i;
	}

	return curPoint;
}

/*********************************************************************\
* Function ：负责小人儿的移动，根据不同方向移动一格	   
* Purpose ：移动模块的实现    
* Params ：Direction（方向）， j（坐标x值），  i（坐标y值） 	   
* Return ：POINT	 	   
* Remarks ：	   
**********************************************************************/
static	POINT	MoveMan(char Direction, int j, int i)
{
	POINT curPoint;

	if (Direction == 'U')
	{
		curPoint.x = j;
		curPoint.y = i-1;
	}

	else if (Direction == 'D')
	{

		curPoint.x = j;
		curPoint.y = i+1;
	}

	else if (Direction == 'L')
	{
		curPoint.x = j-1;
		curPoint.y = i;
	}

	else if (Direction == 'R')
	{
		curPoint.x = j+1;
		curPoint.y = i;
	}
	gamestate.pCurManState = GameMap[curPoint.y][curPoint.x];
	return curPoint;
}


/*********************************************************************\
* Function ：设定方向键	   
* Purpose ：相应键盘消息     
* Params ：wParam	   
* Return ：char	 	   
* Remarks ：	   
**********************************************************************/
static	char	PushBoxDirectionKeyDown(WPARAM wParam)
{
	char Direction = NULL;

	switch(wParam)
	{
	case VK_UP://上箭头键表示方向为上
// 	case VK_2://2键表示方向为上

		Direction = 'U';

		break;

	case VK_DOWN://下箭头键表示方向为下
// 	case VK_8://8键表示方向为下

		Direction = 'D';

		break;
		
	case VK_LEFT://左箭头键表示方向为左
// 	case VK_4://4键表示方向为左

		Direction= 'L';

		break;
		
	case VK_RIGHT://右箭头键表示方向为右
// 	case VK_6://6键表示方向为右

		Direction= 'R';

		break;
	}
	return Direction;
}


/*********************************************************************\
* Function ：找到小人的位置	   
* Purpose ：确定小人儿的位置  
* Params ：无	   
* Return ：POINT	 	   
* Remarks ：	   
**********************************************************************/
static	POINT	FindMan(void)
{
	int i,j;

	for(i=0; i<row; i++)//找到小人
	{
		for(j=0; j<line; j++)
		{
			if((GameMap[i][j] == 3) || (GameMap[i][j] == 6))
			{
				gamestate.Man.x = j;
				gamestate.Man.y = i;

			}
		}
	}
	return gamestate.Man;
}


/*********************************************************************\
* Function ：得到下一点的逻辑坐标	   
* Purpose ：找到下一点的位置     
* Params ：prePoint（当前点的坐标），Direction（移动方向）	   
* Return ：POINT	 	   
* Remarks ：	   
**********************************************************************/
static	POINT	GetNextPoint(POINT prePoint, char Direction)
{
	POINT NextPoint;

	if(Direction == 'L')//向左移动
	{
		NextPoint.x = prePoint.x-1;
	    NextPoint.y = prePoint.y;
	}
	
	else if(Direction == 'R')//向右移动
	{
		NextPoint.x = prePoint.x+1;
	    NextPoint.y = prePoint.y;
	}

	else if(Direction == 'U')//向上移动
	{
		NextPoint.x = prePoint.x;
		NextPoint.y = prePoint.y-1;
	}

	else if(Direction == 'D')//向下移动
	{
		NextPoint.x = prePoint.x;
		NextPoint.y = prePoint.y+1;
	}
	else
	{
		NextPoint.x = prePoint.x;
		NextPoint.y = prePoint.y;
	}

	return NextPoint;

}


/*********************************************************************\
* Function ：判断这一点是什么	   
* Purpose ：得到这一点地图内容     
* Params ：curPoint（该点位置坐标）	   
* Return ：char	 	   
* Remarks ：	   
**********************************************************************/
static	char	GetStateByPoint(POINT curPoint)
{
	return GameMap[curPoint.y][curPoint.x];
}


/*********************************************************************\
* Function ：测试函数	   
* Purpose ：得到下一点的坐标，并判断它是什么      
* Params ：curPoint（当前的位置坐标），IsWhat（地图内容）， Direction（移动方向） 	   
* Return ：char	 	   
* Remarks ：	   
**********************************************************************/
static	char	TestPosition(POINT curPoint, char IsWhat, char Direction)
{
	curPoint = GetNextPoint(curPoint, Direction);
	IsWhat = GetStateByPoint(curPoint);

	return IsWhat;
}


/*********************************************************************\
* Function ：设置标识量	   
* Purpose ：移动模块里所有标识量的初始化      
* Params ：Direction（移动方向）	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	SettingSign(char Direction)
{
	POINT curPoint;
	char IsWhat;
	
	//标识量初始化	
    gamestate.IsAreaOnBox = FALSE;
	gamestate.IsAreaOnMan = FALSE;
	gamestate.IsBoxOnBox = FALSE;
	gamestate.IsBoxOnMan = FALSE;
	gamestate.IsTerminiOnBox = FALSE;
	gamestate.IsTerminiOnMan = FALSE;
	gamestate.IsWallOnBox = FALSE;
	gamestate.IsWallOnMan = FALSE;
	
	curPoint = GetNextPoint(gamestate.Man, Direction);//得到小人儿的下一点坐标
    IsWhat = GetStateByPoint(curPoint);//判断这一点是什么
	
	switch(IsWhat)//根据判断的结果设置标识量
	{
	case 1://小人儿前是墙

		gamestate.IsWallOnMan = TRUE;

		break;
		
	case 5://小人儿是空地

		gamestate.IsAreaOnMan = TRUE;

		break;
		
	case 4://小人儿前是目的地

		gamestate.IsTerminiOnMan = TRUE;

		break;
		
	case 2://小人儿前是箱子
	case 7://小人儿前是箱子在目的地上

		gamestate.IsBoxOnMan = TRUE;
		IsWhat = TestPosition(curPoint, IsWhat, Direction);//判断箱子前是什么
		
		switch(IsWhat)
		{
		case 2://是箱子
		case 7://或是箱子在目的地上

			gamestate.IsBoxOnBox = TRUE;

			break;
			
		case 1://是墙

			gamestate.IsWallOnBox = TRUE;

			break;
			
		case 4://是目的地

			gamestate.IsTerminiOnBox = TRUE;

			break;
			
		case 5://是空地

			gamestate.IsAreaOnBox = TRUE;

			break;
		}
		break;
	}
	
}


/*********************************************************************\
* Function ：计算地图里箱子不在目的地上的个数	   
* Purpose ：得到地图里箱子不在目的地上的个数      
* Params ：无	   
* Return ：char	 	   
* Remarks ：	   
**********************************************************************/
static	char	GetBoxNum(void)
{
	int i,j;

	char TerminiNum = 0;

	for(i=0; i<row; i++)
	{
		for(j=0; j<line; j++)
		{
			if(GameMap[i][j] == 2)//箱子
				TerminiNum++;
		}
	}
	return TerminiNum;

}


/*********************************************************************\
* Function ：判断游戏是否结束	   
* Purpose ：判断游戏是否结束      
* Params ：无	   
* Return ：BOOL	 	   
* Remarks ：	   
**********************************************************************/
static	BOOL	IsGameOver(void)
{
	//判断目的地上是否都有箱子了
	if(0 == GetBoxNum())
	{
//		MoveNum = 0;
		return TRUE;
	}

	return FALSE;
}


/*********************************************************************\
* Function ：画移动后的小人，并擦掉原来的小人	   
* Purpose ：实现移动      
* Params ：hdc， Direction（移动方向）	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	DrawMoveMan(HDC hdc, char Direction)
{
	POINT curPoint;
	curPoint = MoveMan(Direction, gamestate.Man.x, gamestate.Man.y);//得到移动后的小人儿的坐标
	
	if(GameMap[gamestate.Man.y][gamestate.Man.x] == 6)//如果移动前小人儿是在目的地上
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 4;//更新地图，把小人移动前的位置置为目的地
	}

	else//否则移动前小人儿是在空地上
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 5;//更新地图，把小人移动前的位置置为空地
	}

	ShowMapBlock(hdc, gamestate.Man.x, gamestate.Man.y);

	if(GameMap[curPoint.y][curPoint.x] == 4)//如果移动后的位置原来是目的地
		GameMap[curPoint.y][curPoint.x] = 6;//把改点置为小人儿在目的地上

	else
		GameMap[curPoint.y][curPoint.x] = 3;//否则移动后的位置原来只能是空地，更新地图，把它置为小人儿

	ShowMapBlock(hdc, curPoint.x, curPoint.y);

	gamestate.Man.x = curPoint.x;//把小人儿移动后的逻辑坐标即当前位置，保存在全局变量中
	gamestate.Man.y = curPoint.y;
}


/*********************************************************************\
* Function ：画移动后的箱子和小人，并擦掉原来的箱子和小人	   
* Purpose ：实现移动      
* Params ：hdc， Direction（移动方向）	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	DrawMoveBoxAndMan(HDC hdc, char Direction)
{
	POINT prePoint;
	POINT curPoint;
	prePoint = MoveMan(Direction, gamestate.Man.x, gamestate.Man.y);//得到小人儿移动后的坐标

	if(GameMap[gamestate.Man.y][gamestate.Man.x] == 6)//判断小人儿移动前是否在目的地上，如果是
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 4;//更新地图，把该为置置为目的地
	}
	
	else//如果不是，即为空地
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 5;//更新地图，把该为置置为空地
	}

	ShowMapBlock(hdc, gamestate.Man.x, gamestate.Man.y);

	curPoint.x = prePoint.x;//保存小人儿移动后的坐标
	curPoint.y = prePoint.y;

	if(GameMap[curPoint.y][curPoint.x] == 7)//判断小人儿移动后的位置原来是否为箱子在目的地，如果是
		GameMap[curPoint.y][curPoint.x] = 6;//更新地图，把该为置置为人在目的地上

	else//如果不是，该位置原来一定是空地
		GameMap[curPoint.y][curPoint.x] = 3;//更新地图，把该位置置为小人儿

	ShowMapBlock(hdc, curPoint.x, curPoint.y);

	prePoint = MoveBox(Direction, prePoint.x, prePoint.y);//得到箱子移动后的坐标

	if(GameMap[prePoint.y][prePoint.x] == 4)//判断箱子移动后是不是在目的地上，如果是
		GameMap[prePoint.y][prePoint.x] = 7;//更新地图，把该位置置为箱子在目的地上

	else//如果不是，箱子移动后一定在空地上
		GameMap[prePoint.y][prePoint.x] = 2;//更新地图，把当前位置置为箱子

	ShowMapBlock(hdc, prePoint.x, prePoint.y);

	gamestate.Man.x = curPoint.x;//把移动后的小人儿的逻辑坐标保存在全局变量中
	gamestate.Man.y = curPoint.y;
}


/*********************************************************************\
* Function ：得到移动前的点的坐标，存入链表（用于PENDOWN消息）	   
* Purpose ：保存后退信息      
* Params ：无	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	GetFormerPointP(POINT curPoint, POINT prePoint, POINT LastPoint)
{
	pList.ManPoint = curPoint;//?
	pList.TheLastPoint = prePoint;
	pList.AndTheLastPoint = LastPoint;
}
/*********************************************************************\
* Function ：得到移动前的点原来显示的是什么，存入链表（用于PENDOWN消息）	   
* Purpose ：保存后退信息      
* Params ：Direction（移动方向）	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	GetFormerPlaceIsWhatP(void)
{
	pList.TheFirstPointIsWhat = GameMap[pList.ManPoint.y][pList.ManPoint.x];
	pList.TheSecondPointIsWhat = GameMap[pList.TheLastPoint.y][pList.TheLastPoint.x];
	pList.TheThirdPointIsWhat = GameMap[pList.AndTheLastPoint.y][pList.AndTheLastPoint.x];
}

/*********************************************************************\
* Function ：得到移动前的点的坐标，存入链表（用于KEYDOWN消息）	   
* Purpose ：保存后退信息      
* Params ：Direction（移动方向）	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	GetFormerPointK(char Direction)
{
	pList.ManPoint = FindMan();//找到当前的小人位置
	pList.TheLastPoint = GetNextPoint(pList.ManPoint, Direction);
	pList.AndTheLastPoint = GetNextPoint(pList.TheLastPoint, Direction);
}


/*********************************************************************\
* Function ：得到移动前的点原来显示的是什么，存入链表（用于KEYDOWN消息）	   
* Purpose ：保存后退信息      
* Params ：Direction（移动方向）	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	GetFormerPlaceIsWhatK()
{
	pList.TheFirstPointIsWhat = GameMap[pList.ManPoint.y][pList.ManPoint.x];
	pList.TheSecondPointIsWhat = GameMap[pList.TheLastPoint.y][pList.TheLastPoint.x];
	pList.TheThirdPointIsWhat = GameMap[pList.AndTheLastPoint.y][pList.AndTheLastPoint.x];
}


/*********************************************************************\
* Function ：实现后退一步并刷新地图	   
* Purpose ：实现后退功能      
* Params ：hdc， *q（存放后退信息的链表中的节点）	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	ComeBack(HDC hdc, ListUnit *q)
{
	GameMap[gamestate.Man.y][gamestate.Man.x] = gamestate.pCurManState;
	ShowMapBlock(hdc, gamestate.Man.x, gamestate.Man.y);

	GameMap[q->TheLastPoint.y][q->TheLastPoint.x] = q->TheSecondPointIsWhat;
	GameMap[q->AndTheLastPoint.y][q->AndTheLastPoint.x] = q->TheThirdPointIsWhat;
	ShowMapBlock(hdc, q->TheLastPoint.x, q->TheLastPoint.y);
	ShowMapBlock(hdc, q->AndTheLastPoint.x, q->AndTheLastPoint.y);

	if(q->pNext == NULL)
	{
		GameMap[q->ManPoint.y][q->ManPoint.x] = q->TheFirstPointIsWhat;
		ShowMapBlock(hdc, q->ManPoint.x, q->ManPoint.y);
	}
	else 
	{
		GameMap[q->pNext->TheLastPoint.y][q->pNext->TheLastPoint.x] = q->pNext->TheStateManAfter;
		ShowMapBlock(hdc, q->pNext->TheLastPoint.x, q->pNext->TheLastPoint.y);
	}
	gamestate.Man = FindMan();
	gamestate.pCurManState = 0;
	
	DeleteHead(&ppList);
	BackCount--;
//	MoveNum--;
}

	
/*********************************************************************\
* Function ：根据标志量显示方向箭头	   
* Purpose ：根据标志量显示方向箭头     
* Params ：hWnd，Disappear（根据小人周围是什么而设置的标志量） 	   
* Return ：void	 	   
* Remarks ：	   
**********************************************************************/
static	void	ShowArrowHead(HWND hWnd, int Disappear)
{
	
	if (Disappear == 1)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKUP);
	
	else if (Disappear == 2)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKDOWN);
	
	else if (Disappear == 3)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKLEFT);
	
	else if (Disappear == 4)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKRIGHT);
	
}


/*********************************************************************\
* Function ：判断小人周围是什么	   
* Purpose ：控制显示的箭头     
* Params ：Direction	   
* Return ：void	 	   
* Remarks ：	   
**********************************************************************/
static	void	IsWhatAroundMan(HWND hWnd)
{
	char Direction;
	int Disappear;

	SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLUP|SCROLLDOWN|SCROLLLEFT|SCROLLRIGHT, MASKUP|MASKDOWN|MASKLEFT|MASKRIGHT);
	FindMan();
	Direction = 'U';
	{
		SettingSign(Direction);
		if(gamestate.IsWallOnMan||gamestate.IsWallOnBox||gamestate.IsBoxOnBox)
			Disappear = 1;
		ShowArrowHead(hWnd, Disappear);
	}

	Direction = 'D';
	{
		SettingSign(Direction);
		if(gamestate.IsWallOnMan||gamestate.IsWallOnBox||gamestate.IsBoxOnBox)
			Disappear = 2;
		ShowArrowHead(hWnd, Disappear);
	}

	Direction = 'L';
	{
		SettingSign(Direction);
		if(gamestate.IsWallOnMan||gamestate.IsWallOnBox||gamestate.IsBoxOnBox)
			Disappear = 3;
		ShowArrowHead(hWnd, Disappear);
	}

	Direction = 'R';
	{
		SettingSign(Direction);
		if(gamestate.IsWallOnMan||gamestate.IsWallOnBox||gamestate.IsBoxOnBox)
			Disappear = 4;
		ShowArrowHead(hWnd, Disappear);
	}
}


/*********************************************************************\
* Function ：控制移动的实现	   
* Purpose ：根据状态，实现小人儿和箱子移动的函数(用于KEYDOWN消息)      
* Params ：hdc， wParam	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	PushBoxControlMoveK(HWND hWnd, HDC hdc, WPARAM wParam)
{
	char Direction;
	ListUnit* p;
	Direction = PushBoxDirectionKeyDown(wParam);//通过响应键盘消息得到移动方向
	SettingSign(Direction);//设置标识量
	
    //根据标识量决定移动情况
	if(gamestate.IsWallOnMan)
		return;

	if((gamestate.IsBoxOnBox)||(gamestate.IsWallOnBox))
		return;

	else
	{
		GetFormerPointK(Direction);

		if(gamestate.IsAreaOnMan == TRUE)//小人儿前是空地
		{
			GetFormerPlaceIsWhatK();
			DrawMoveMan(hdc, Direction);//显示移动的小人儿
		}
		
		else if(gamestate.IsTerminiOnMan == TRUE)//如果小人儿前是目的地
		{
			GetFormerPlaceIsWhatK();
			DrawMoveMan(hdc, Direction);//显示移动的小人儿
		}
		
		else if(gamestate.IsBoxOnMan == TRUE)//如果小人儿前是箱子
		{
			if(gamestate.IsAreaOnBox == TRUE)//并且箱子前是空地
			{
				GetFormerPlaceIsWhatK();
				DrawMoveBoxAndMan(hdc, Direction);//显示移动的箱子和小人儿
			}
			
			else if(gamestate.IsTerminiOnBox == TRUE)//并且箱子前是目的地
			{
				GetFormerPlaceIsWhatK();
				DrawMoveBoxAndMan(hdc, Direction);//显示移动的箱子和小人儿
			}
			pList.TheStateManAfter = GameMap[gamestate.Man.y][gamestate.Man.x];
			p = (ListUnit*)malloc(sizeof(ListUnit));
			p->ManPoint = pList.ManPoint;				
			p->TheLastPoint = pList.TheLastPoint;			
			p->AndTheLastPoint = pList.AndTheLastPoint;		
			p->TheFirstPointIsWhat = pList.TheFirstPointIsWhat;
			p->TheSecondPointIsWhat = pList.TheSecondPointIsWhat;
			p->TheThirdPointIsWhat = pList.TheThirdPointIsWhat;
			p->TheStateManAfter = pList.TheStateManAfter;
			InsertDataToList(&ppList, p);
			BackCount = BackCount+1;

			if(BackCount>15)
			{
				DeleteTail(&p);
				BackCount--;
			}
		}
	}
		
}


/*********************************************************************\
* Function ：控制移动的实现	   
* Purpose ：根据状态，实现小人儿和箱子移动的函数(用于PENDOWN消息)      
* Params ：hdc， wParam	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	int		PushBoxControlMoveP(HWND hWnd, LPARAM lParam)
{
	POINT prePoint;
	POINT curPoint;
	POINT LastPoint;
	char IsWhat;
	char Direction;
	ListUnit* p;
	HDC hdc;
	RECT rect;
	int tempDist;
	
	GetClientRect(hWnd, &rect);
	
	MyPen.x = LOWORD(lParam)-(rect.right-line*RECT_W)/2;
	MyPen.y = HIWORD(lParam)-(rect.bottom-row*RECT_W)/2;
	
	prePoint = FindMyPenInMap();//记录笔点下的位置坐标
	if(prePoint.x == -100)
		return 1;
	
	IsWhat = GetStateByPoint(prePoint);//判断笔点下的位置是什么
	if(IsWhat == 0)					   //点下的是背景的话也退出。
		return 1;
	hdc = GetDC(hWnd);
	
	if(IsWhat == 5 || IsWhat == 4)//如果点下的位置是空地或目的地
	{
		curPoint = FindMan();
		tempDist = SearchPath(curPoint, prePoint);
		
		if(tempDist != 0)//设定定时器
		{
			Distance = tempDist;
			if (!bTimerAlready)
			{
				SetTimer(hWnd, TIMER_IDM, INTERVAL, NULL);
				bTimerAlready = TRUE;
			}
			else
			{
				KillTimer(hWnd, TIMER_IDM);
				SetTimer(hWnd, TIMER_IDM, INTERVAL, NULL);
				bTimerAlready = TRUE;
			}
		}
		return 1;
	}

	if(Distance>=0)
		return 1;

	if((IsWhat == 2) || (IsWhat == 7))//如果点下的位置是箱子，判断小人是否在箱子周围，在哪个方向
	{
		int IsBoxMove;
		IsBoxMove = 0;
		curPoint = FindMan();
		Distance = -1;
		if((prePoint.y == curPoint.y-1) && (prePoint.x == curPoint.x))//如果箱子在小人的上边
		{
			Direction = 'U';
			
			LastPoint = GetNextPoint(prePoint, Direction);
			IsWhat = GetStateByPoint(LastPoint);//判断箱子的上边是什么
			if(IsWhat == 5 || IsWhat ==4)//如果是空地或目的地
			{
				GetFormerPointP(curPoint, prePoint, LastPoint);
				GetFormerPlaceIsWhatP();
				DrawMoveBoxAndMan(hdc, Direction);//箱子向上移动
				IsBoxMove = 1;
			}
		}
		
		else if((prePoint.y == curPoint.y+1) && (prePoint.x == curPoint.x))//如果箱子在小人的下边
		{
			Direction = 'D';
			LastPoint = GetNextPoint(prePoint, Direction);
			IsWhat = GetStateByPoint(LastPoint);//判断箱子的下边是什么
			if(IsWhat == 5 || IsWhat ==4)//如果是空地或目的地
			{
				GetFormerPointP(curPoint, prePoint, LastPoint);
				GetFormerPlaceIsWhatP();
				DrawMoveBoxAndMan(hdc, Direction);//箱子向下移动
				IsBoxMove = 1;
			}
		}
		
		else if((prePoint.y == curPoint.y) && (prePoint.x == curPoint.x-1))//如果箱子在小人的左边
		{
			Direction = 'L';
			LastPoint = GetNextPoint(prePoint, Direction);
			IsWhat = GetStateByPoint(LastPoint);//判断箱子的左边是什么
			if(IsWhat == 5 || IsWhat ==4)//如果是空地或目的地
			{
				GetFormerPointP(curPoint, prePoint, LastPoint);
				GetFormerPlaceIsWhatP();
				DrawMoveBoxAndMan(hdc, Direction);//箱子向左移动
				IsBoxMove = 1;
			}
		}
		
		else if((prePoint.y == curPoint.y) && (prePoint.x == curPoint.x+1))//如果箱子在小人的右边
		{
			Direction = 'R';
			LastPoint = GetNextPoint(prePoint, Direction);
			IsWhat = GetStateByPoint(LastPoint);//判断箱子的右边是什么
			if(IsWhat == 5 || IsWhat ==4)//如果是空地或目的地
			{
				GetFormerPointP(curPoint, prePoint, LastPoint);
				GetFormerPlaceIsWhatP();
				DrawMoveBoxAndMan(hdc, Direction);//箱子向右移动
				IsBoxMove = 1;
			}
		}
		if(IsBoxMove == 1)
		{
			pList.TheStateManAfter = GameMap[gamestate.Man.y][gamestate.Man.x];
			p = (ListUnit*)malloc(sizeof(ListUnit));
			p->ManPoint = pList.ManPoint;				
			p->TheLastPoint = pList.TheLastPoint;			
			p->AndTheLastPoint = pList.AndTheLastPoint;		
			p->TheFirstPointIsWhat = pList.TheFirstPointIsWhat;
			p->TheSecondPointIsWhat = pList.TheSecondPointIsWhat;
			p->TheThirdPointIsWhat = pList.TheThirdPointIsWhat;
			p->TheStateManAfter = pList.TheStateManAfter;
			InsertDataToList(&ppList, p);
			BackCount = BackCount+1;
			
			if(BackCount>15)
			{
				DeleteTail(&p);
				BackCount--;
			}
		}
	}
	ReleaseDC(hWnd, hdc);
	return 0;
}
/*********************************************************************\
* Function ：释放资源，重新显示游戏窗口	   
* Purpose ：进入下一关游戏      
* Params ：hWnd	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
//释放资源，重新显示游戏窗口
static	void	ShowGameWindowAnew(HWND hWnd)
{
	FreeList(&ppList);
	Distance = -1;
	BackCount = 0;
//	MoveNum = 0;

	ReadBoxMap(curnLevel);
	
	FindMan();

//	ShowWindow(hWnd, SW_SHOW);
	InvalidateRect(hWnd, NULL, FALSE);
	ShowOwnedPopups(hWnd, TRUE);
//	UpdateWindow(hWnd);
}


/*********************************************************************\
* Function ：找到笔的位置	   
* Purpose ：在地图里确定笔的位置  
* Params ：无	   
* Return ：POINT	 	   
* Remarks ：	   
**********************************************************************/
static	POINT	FindMyPenInMap(void)
{
	int i,j;
	POINT Point;

	Point.x = -100;
	Point.y = -100;
	for(i=0; i<row; i++)
	{
		for(j=0; j<line; j++)
		{
			if((MyPen.x)>=(j*RECT_W) && (MyPen.x)<((j+1)*RECT_W) && (MyPen.y)>=(i*RECT_W) && (MyPen.y)<((i+1)*RECT_W))
			{
				Point.x = j;	
				Point.y = i;
			}
		}
	}
	return Point;
}


/*********************************************************************\
* 函数名   ：	FindNextAdj
* 功  能   ：	寻找给定点的下一个邻接点
* 参数说明 ：	NextPt:		定点的坐标
* 返回值   ：	1:	找到邻接点		0:	没找到邻接点
**********************************************************************/
static	short	FindNextAdj(int IsNew, POINT Pt, POINT * NextPt)
{
	static short i;
	if(IsNew)
		i = 1;
	
	if(i == 1)
	{
		i++;
		NextPt->x = Pt.x;
		NextPt->y = Pt.y - 1;
		if((GameMap[NextPt->y][NextPt->x] == 4)||(GameMap[NextPt->y][NextPt->x] == 5))
			return 1;
	}
	if(i == 2)
	{
		i++;
		NextPt->x = Pt.x + 1;
		NextPt->y = Pt.y;
		if((GameMap[NextPt->y][NextPt->x] == 4)||(GameMap[NextPt->y][NextPt->x] == 5))
			return 1;
	}
	if(i == 3)
	{
		i++;
		NextPt->x = Pt.x;
		NextPt->y = Pt.y + 1;
		if((GameMap[NextPt->y][NextPt->x] == 4)||(GameMap[NextPt->y][NextPt->x] == 5))
			return 1;
	}
	if(i == 4)
	{
		i = 0;
		NextPt->x = Pt.x - 1;
		NextPt->y = Pt.y;
		if((GameMap[NextPt->y][NextPt->x] == 4)||(GameMap[NextPt->y][NextPt->x] == 5))
			return 1;
	}
	i++;
	return 0;
}


/*********************************************************************************************
* 函数名   ：	SearchPath
* 功  能   ：	寻找从起始点到终止点的路径
* 参数说明 ：	ResPt:	起始点的坐标
				DesPt:	终止点的坐标
				Path:	纪录从起始点到终止点的路径
* 返回值   ：	从起始点到终止点的距离，返回0表示没找到路径
**********************************************************************************************/
static	short	SearchPath(POINT ResPt, POINT DesPt)
{
	short ii = 0, j, k = 0, m = 0;
	short Queue[9*12], Path[9*12][2];
	POINT Pt, NextPt;
	//short Visited[9][12];
	short Visited[12][9];
	int isnew;
	short t1,t2;

	for(t1 = 0; t1 < 9; t1++)
	for(t2 = 0; t2 <12; t2++)
		Visited[t1][t2] = 0;

	Visited[ResPt.y][ResPt.x] = 1;
	Queue[ii++] = (short)(ResPt.y * line + ResPt.x);
	Path[k][0] = (short)(ResPt.y * line + ResPt.x);
	Path[k][1] = m ++;
	k ++;

	while (ii != 0)
	{
		ii --;
		Pt.y = Queue[0] / line;
		Pt.x = Queue[0] % line;
		for (j = 0; j < ii; j ++)
			Queue[j] = Queue[j + 1];

		isnew = 1;
		while (FindNextAdj(isnew, Pt, &NextPt) != 0)//寻找下一个邻接点
		{
			isnew = 0;
			if (DesPt.x == NextPt.x && DesPt.y == NextPt.y)
			{
				Path[k][0] = (short)(NextPt.y * line + NextPt.x);
				Path[k][1] = m;
				j = 0;
				do
				{
					ShortPath[j ++] = Path[k][0];
					k = Path[k][1] - 1;
				} while (k != 0);
				ShortPath[j] = Path[0][0];
				return j;
			}
			if (Visited[NextPt.y][NextPt.x] != 1 )
			{
				Visited[NextPt.y][NextPt.x] = 1;
				Queue[ii ++] = (short)(NextPt.y * line + NextPt.x);
				Path[k][0] = (short)(NextPt.y * line + NextPt.x);
				Path[k][1] = m;
				k ++;
			}
		}
		m ++;
	}

	return 0;
}


/*********************************************************************\
* Function ：小人自己移动时，画移动后的小人，并擦掉原来的小人	   
* Purpose ：实现小人的智能移动      
* Params ：hdc， j,i（移动坐标）	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	DrawAutoMoveMan(HDC hdc, short j, short i)
{
	//POINT curPoint;
	//curPoint = MoveMan(Direction, gamestate.Man.x, gamestate.Man.y);//得到移动后的小人儿的坐标
	FindMan();
	
	if(GameMap[gamestate.Man.y][gamestate.Man.x] == 6)//如果移动前小人儿是在目的地上
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 4;//更新地图，把小人移动前的位置置为目的地
		DrawTermini(hdc, gamestate.Man.x, gamestate.Man.y);//显示目的地
	}

	else//否则移动前小人儿是在空地上
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 5;//更新地图，把小人移动前的位置置为空地
	    DrawArea(hdc, gamestate.Man.x, gamestate.Man.y);//显示空地
	}

	gamestate.pCurManState = GameMap[i][j];
	if(GameMap[i][j] == 4)//如果移动后的位置原来是目的地
		GameMap[i][j] = 6;//把改点置为小人儿在目的地上

	else
		GameMap[i][j] = 3;//否则移动后的位置原来只能是空地，更新地图，把它置为小人儿

	DrawMan(hdc, j, i);//把移动后的位置显示为小人儿

	gamestate.Man.x = j;//把小人儿移动后的逻辑坐标即当前位置，保存在全局变量中
	gamestate.Man.y = i;
}


/*********************************************************************\
* Function ：从链表头插入链表	   
* Purpose ：输入后退信息     
* Params ：**ppList（链表头节点）， *pList（需要插入的节点）
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	InsertDataToList( ListUnit **ppList, ListUnit *pList)
{
	ListUnit *p;

	/*不存在链表的情况，自己创建链表头,并插入链表头*/
	if (*ppList == NULL)
	{
		*ppList = pList;
		(*ppList)->pNext = NULL;
		return ;
	}

	/*如果存在*/
	p = pList;
   	p->pNext = *ppList;
	*ppList = p;
}


/*********************************************************************\
* Function ：删除链表的表尾，将倒数第二个点作为新的表尾	   
* Purpose ：链表里只存放最近的15个节点的信息，把以前的信息随着链表的插入逐一删除     
* Params ：**ppList（链表头节点）
* Return ：int	 	   
* Remarks ：	   
**********************************************************************/
static	int		DeleteTail(ListUnit **ppList)
{
	ListUnit *pPre;
   	ListUnit *qCur;

	if(*ppList == NULL)
		return 0;

	pPre = *ppList;

	if(pPre->pNext == NULL)
	{
		free(pPre);
		*ppList = NULL;
		return 1;
	}

	while(pPre->pNext->pNext)
		pPre = pPre->pNext;
	
	qCur = pPre->pNext;
	pPre->pNext = NULL;

	free(qCur);

    return 1;
}


/*********************************************************************\
* Function ：删除链表的表头	   
* Purpose ：每后退一步后，把该点信息删除
* Params ：**ppList（链表头节点）
* Return ：int	 	   
* Remarks ：	   
**********************************************************************/
static	int		DeleteHead(ListUnit **ppList)
{
	ListUnit *p;
	if(*ppList == NULL)
		return 0;

	p = *ppList;
	*ppList = (*ppList)->pNext;

	free(p);
	return 1;
}


/*********************************************************************\
* Function ：释放链表	   
* Purpose ：每一关游戏结束释放资源
* Params ：**ppList（链表头节点）
* Return ：int	 	   
* Remarks ：	   
**********************************************************************/
static	int		FreeList(ListUnit **ppList)
{
	ListUnit *p;

	if(!*ppList)
		return 0;

	p = *ppList;
	while (*ppList)
	{
		*ppList = (*ppList)->pNext;
		free(p);
		p = *ppList;
	}

	return 1;
}


/*********************************************************************\
* Function ：刷背景	   
* Purpose ：刷背景      
* Params ：hdc，rgb，hWnd  	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
/*void PAINTBK_COLOR(HDC hdc, COLORREF rgb, HWND hWnd)
{
	RECT rect;
	HBRUSH hbrush;

	SetViewportOrgEx(hdc, 0, 0, NULL);
	GetClientRect(hWnd, &rect);
	hbrush = CreateSolidBrush(rgb);	
	FillRect(hdc, &rect, hbrush);

	DeleteObject(hbrush);
}
*/

/*********************************************************************\
* Function ：画地图里的背景	   
* Purpose ：画地图里的背景      
* Params ：hdc 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	DrawBackGround(HDC hdc)
{
	SetViewportOrgEx(hdc, 0, 0, NULL);
	BitBlt(
		hdc, 
		WND_X,
		WND_Y,
		COVER_WIDTH,
		COVER_HEIGHT,
		(HDC)hPushBoxbg,
		0, 0, 
		ROP_SRC );
}
/*{
	RECT rect;
	HBRUSH hbrush,holdbrush;

	rect.left = j*RECT_W;
	rect.top = i*RECT_W;
	rect.right = (j+1)*RECT_W;
	rect.bottom = (i+1)*RECT_W;
	hbrush = CreateSolidBrush(RGB(0, 255, 0));
	holdbrush = (HBRUSH)SelectObject(hdc, hbrush);
	FillRect(hdc, &rect, hbrush);

	SelectObject(hdc, holdbrush);
	DeleteObject(hbrush);
}
*/

/*********************************************************************\
* Function ：画地图里的空地	   
* Purpose ：画地图里的空地      
* Params ：hdc，j（坐标x值），i （坐标x值） 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	DrawArea(HDC hdc, int j, int i)
{
	BitBlt(
		hdc, 
		j*RECT_W,
		i*RECT_W,
		RECT_W,
		RECT_W,
		(HDC)hPushBoxArea,
		0, 0, 
		ROP_SRC );
}
/*{
	HPEN hpen,holdpen;
	HBRUSH hbrush,holdbrush;

	hpen = CreatePen(PS_SOLID, 1, RGB(0 , 0, 0));
	hbrush = CreateSolidBrush(RGB(0, 0, 255));
	holdpen = (HPEN)SelectObject(hdc, hpen);
	holdbrush = (HBRUSH)SelectObject(hdc, hbrush);
	Rectangle(hdc, j*RECT_W, i*RECT_W, (j+1)*RECT_W, (i+1)*RECT_W);
	
	SelectObject(hdc, holdpen);
	SelectObject(hdc, holdbrush);
	DeleteObject(hpen);
	DeleteObject(hbrush);
}
*/

/*********************************************************************\
* Function ：画地图里的墙	   
* Purpose ：画地图里的墙      
* Params ：hdc，j（坐标x值），i （坐标x值） 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	DrawWall(HDC hdc, int j, int i)
{

	BitBlt(
		hdc, 
		j*RECT_W,
		i*RECT_W,
		RECT_W ,
		RECT_W ,
		(HDC)hPushBoxWall,
		0, 0, 
		ROP_SRC );


	//StretchBlt(hdc, j*RECT_W, i*RECT_W, RECT_W + 25, RECT_W + 25, hPushBoxWall, 0, 0, RECT_W, RECT_W, ROP_SRC);
}
/*{
	HPEN hpen,holdpen;
	HBRUSH hbrush,holdbrush;

	hbrush = CreateSolidBrush(RGB(192, 192, 192));
	holdbrush = (HBRUSH)SelectObject(hdc, hbrush);
	Rectangle(hdc, j*RECT_W, i*RECT_W, (j+1)*RECT_W, (i+1)*RECT_W);

	hpen = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
	holdpen = (HPEN)SelectObject(hdc, hpen);
	
	MoveToEx(hdc, j*RECT_W+1, (i+1)*RECT_W-2, NULL);
	LineTo(hdc, j*RECT_W+1, i*RECT_W);//i*RECT_W-1
	MoveToEx(hdc, j*RECT_W+1, i*RECT_W+1, NULL);
	LineTo(hdc, (j+1)*RECT_W-1, i*RECT_W+1);
	
	SelectObject(hdc, holdpen);
	DeleteObject(hpen);
	SelectObject(hdc, holdbrush);
	DeleteObject(hbrush);

	hpen = CreatePen(PS_SOLID, 1, RGB(100, 100, 100));
	holdpen = (HPEN)SelectObject(hdc, hpen);

	MoveToEx(hdc, j*RECT_W, (i+1)*RECT_W-1, NULL);
	LineTo(hdc, (j+1)*RECT_W-1, (i+1)*RECT_W-1);
	LineTo(hdc, (j+1)*RECT_W-1, i*RECT_W-1);
	
	MoveToEx(hdc, j*RECT_W+1, (i+1)*RECT_W-2, NULL);
	LineTo(hdc, (j+1)*RECT_W-2, (i+1)*RECT_W-2);
	LineTo(hdc, (j+1)*RECT_W-2, i*RECT_W);

	SelectObject(hdc, holdpen);
	DeleteObject(hpen);
}*/
	

/*********************************************************************\
* Function ：画地图里的小人儿	   
* Purpose ：画地图里的小人儿      
* Params ：hdc，j（坐标x值），i （坐标x值） 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	DrawMan(HDC hdc, int j, int i)
{
	COLORREF	OldClr;
	int			OldStyle;

	OldClr = SetBkColor(hdc,clr1);
	OldStyle = SetBkMode(hdc, NEWTRANSPARENT);
	
//	MoveNum++;

//	if(MoveNum%2)
//	{
	BitBlt(
		hdc, 
		j*RECT_W,
		i*RECT_W,
		RECT_W,
		RECT_W,
		(HDC)hPushBoxMan1,
		0, 0, 
		ROP_SRC );
//	}
/*	else
	{
		BitBlt(
			hdc, 
			j*RECT_W,
			i*RECT_W,
			RECT_W,
			RECT_W,
			(HDC)hPushBoxMan2,
			0, 0, 
			ROP_SRC );
	}
*/
	SetBkColor(hdc,OldClr);
	SetBkMode(hdc,OldStyle);
}
/*{
	HPEN hpen = NULL;
	HPEN holdpen = NULL;
	HBRUSH hbrush,holdbrush;

	hbrush = CreateSolidBrush(RGB(0, 0, 0));
	holdbrush = (HBRUSH)SelectObject(hdc, hbrush);
    Rectangle(hdc, j*RECT_W, i*RECT_W, (j+1)*RECT_W, (i+1)*RECT_W);

	SelectObject(hdc, holdbrush);
	DeleteObject(hbrush);

	hpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hbrush = CreateSolidBrush(RGB(255, 0, 0));
	holdpen = (HPEN)SelectObject(hdc, hpen);
	holdbrush = (HBRUSH)SelectObject(hdc, hbrush);
	
	DrawCircle(hdc, j*RECT_W+RECT_W/2, i*RECT_W+RECT_W/2, RECT_W/2-1);

	SelectObject(hdc, holdpen);
	SelectObject(hdc, holdbrush);
	DeleteObject(hpen);
	DeleteObject(hbrush);
}
*/

/*********************************************************************\
* Function ：画地图里的目的地	   
* Purpose ：画地图里的目的地      
* Params ：hdc，j（坐标x值），i （坐标x值） 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	DrawTermini(HDC hdc, int j, int i)
{
	BitBlt(
		hdc, 
		j*RECT_W,
		i*RECT_W,
		RECT_W,
		RECT_W,
		(HDC)hPushBoxTemini,
		0, 0, 
		ROP_SRC );
}
/*{
	HPEN hpen,holdpen;
	HBRUSH hbrush,holdbrush;

	hpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hbrush = CreateSolidBrush(RGB(255, 255, 0));
	holdpen = (HPEN)SelectObject(hdc, hpen);
	holdbrush = (HBRUSH)SelectObject(hdc, hbrush);

	Rectangle(hdc, j*RECT_W, i*RECT_W, (j+1)*RECT_W, (i+1)*RECT_W);
	
	SelectObject(hdc, holdpen);
	SelectObject(hdc, holdbrush);
	
	DeleteObject(hpen);
	DeleteObject(hbrush);
}
*/

/*********************************************************************\
* Function ：画地图里的箱子	   
* Purpose ：画地图里的箱子      
* Params ：hdc，j（坐标x值），i （坐标x值） 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	DrawBox(HDC hdc, int j, int i)
{
	BitBlt(
		hdc, 
		j*RECT_W,
		i*RECT_W,
		RECT_W,
		RECT_W,
		(HDC)hPushBoxBox,
		0, 0, 
		ROP_SRC );
}
/*{
	HPEN hpen,holdpen;
	HBRUSH hbrush,holdbrush;

	hpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	hbrush = CreateSolidBrush(RGB(255, 0, 255));
	holdpen = (HPEN)SelectObject(hdc, hpen);
	holdbrush = (HBRUSH)SelectObject(hdc, hbrush);

	Rectangle(hdc, j*RECT_W, i*RECT_W, (j+1)*RECT_W, (i+1)*RECT_W);

	SelectObject(hdc, holdpen);
	SelectObject(hdc, holdbrush);
	DeleteObject(hpen);
	DeleteObject(hbrush);
}
*/

/*********************************************************************\
* Function ：根据内容画一块地图	   
* Purpose ：实现地图的显示     
* Params ：hdc，j（坐标x值），i （坐标x值） 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	ShowMapBlock(HDC hdc, int j, int i)
{
	if(GameMap[i][j] == 1)
		DrawWall(hdc, j, i);
		
	else if (GameMap[i][j] == 3)
	{
		DrawArea(hdc, j, i);
		DrawMan(hdc, j, i);
	}

	else if (GameMap[i][j] == 6)
	{
		DrawTermini(hdc, j, i);
		DrawMan(hdc, j, i);
	}
		
	else if (GameMap[i][j] == 4)
		DrawTermini(hdc, j, i);
		
	else if (GameMap[i][j] == 2)
	{
		DrawArea(hdc, j, i);
		DrawBox(hdc, j, i);
	}

	else if (GameMap[i][j] == 7)
	{
		DrawTermini(hdc, j, i);
		DrawBox(hdc, j, i);
	}
		
	else if (GameMap[i][j] == 5)
		DrawArea(hdc, j, i);
		
/*	else if (GameMap[i][j] == 0)
		DrawBackGround(hdc, j, i);
*/
}


/*********************************************************************\
* Function ：居中输出	   
* Purpose ：显示地图时，为了输出美观，居中显示    
* Params ：hWnd， hdc 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	CenterShow(HWND hWnd, HDC hdc)
{
	POINT Point;
	RECT rect;	

	Point.x = 0;
	Point.y = 0;

	GetClientRect(hWnd, &rect);
	SetViewportOrgEx(hdc, (rect.right-line*RECT_W)/2, (rect.bottom-row*RECT_W)/2, &Point);
}


/*********************************************************************\
* Function ：显示地图	   
* Purpose ：实现地图的显示     
* Params ：hdc 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	ShowMap(HDC hdc)
{
	int i, j;
	
	for(i=0; i<row; i++)
	{
		for(j=0; j<line; j++)
		{
			ShowMapBlock(hdc, j, i);
		}
	}
}


/*********************************************************************\
* Function ：把当前关数写入文件	   
* Purpose ：实现“继续上次游戏的功能”      
* Params ：无 	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	WriteLevelToFile(void)
{
	//打开文件，如果不存在创建文件
	int		fp;
	int		i;

/*	fp = CreateFile("FLASH2:BoxLevel.dat", ACCESS_WRITE, 0);
	if(fp == -1)
		fp = CreateFile("FLASH2:BoxLevel.dat",CREATE_OVERWRITE,0);
*/	
	if (-1==(fp=(open(PushBoxLevel,O_WRONLY|O_TRUNC))))
		if (-1==(fp=creat(PushBoxLevel,0x666)))
			return;

//	write(fp, &curnLevel, 1);

	i = (int)(write(fp,&curnLevel,1));
	{
		if(i==-1)
		{
			close(fp);
			return;
		}
		if((i>0)&&(i<1))
		{
			close(fp);
			unlink(PushBoxLevel);
			//MsgWin(IDS_FILE_PROMPT,IDS_FILE_NOTICE,WAITTIMEOUT);
			PLXTipsWin(NULL, NULL, 0, IDS_FILE_PROMPT,ML("PushBox"),Notify_Info, ML("OK") ,NULL, WAITTIMEOUT);
			return;
		}
	}
	close(fp);
	return;
}


/*********************************************************************\
* Function ：从文件中读出游戏关数	   
* Purpose ：实现“继续上次游戏的功能”      
* Params ：无 	   
* Return ：int	 	   
* Remarks ：	   
**********************************************************************/
static	void	ReadLevelFromFile(void)
{
	int	fp;
	//int curnlevel;

	fp = open(PushBoxLevel, O_RDONLY|O_CREAT,0666);//打开文件

	read(fp, &curnLevel, 1);

	close(fp);
	//return curnlevel;
}


/*********************************************************************\
* Function ：将int型变量转化成char型变量	   
* Purpose ：实现地图存储      
* Params ：*pStr（字符串），num（int型变量）  	   
* Return ：无	 	   
* Remarks ：	   
**********************************************************************/
static	void	MyIntToStr (char *pStr, int num) 
{
	int n, i = 0;
	BOOL hiBit = 0;
	
	n = num / 1000;
	if(n > 0)
	{
		pStr[i++] = n + 48;
		hiBit = 1;
	}
	
	num = num % 1000;
	n = num / 100;
	if( (n > 0) || (hiBit == 1) )
	{
		pStr[i++] = n + 48;
		hiBit = 1;
	}

	num = num % 100;
	n = num / 10;
	if( (n > 0) || (hiBit == 1) )
	{
		pStr[i++] = n + 48;
		hiBit = 1;
	}
	
	n = num % 10;
	pStr[i++] = n + 48;
	pStr[i] = 0;
}


/*********************************************************************\
* Function ：把地图文件读入内存	   
* Purpose ：实现地图存储      
* Params ：无  	   
* Return ：BOOL	 	   
* Remarks ：	   
* Extral :it seems that function open and fstat can't work well together,
		  I modify the code of function ReadMapFromFile to do the same 
		  thing in 04-05 2005.
**********************************************************************/
static	BOOL	ReadMapFromFile()
{

	int	fp;
	int size;
	//struct stat *fd_stat=NULL;
	//fp = CreateFile("Rom:MyMap.txt", O_RDONLY, 0);//打开文件

	fp = open(PushBoxMap, O_RDONLY);//打开文件
	if(fp == -1){
		return FALSE;
	}
	pChar = (char *)malloc(4096*sizeof(char));
	memset(pChar, 0, sizeof(char)*4096);
	
	size = read(fp,pChar,4096);
	if(size>=4096)
	{
		printf("only get a part data of the map\n");
		//TODO:add code  to deal with the case;
		//realloc the memory for pChar so that it could contain whole data when the size
		//of the map file is greater than 4096. 
	}
	else if(size<0)
	{
		printf("failed to read date from file %s",PushBoxMap);
		close(fp);
		return FALSE;
	}
	else
	{
		pChar = realloc(pChar,size);
		//printf("%s\n",pChar);
	}
/*		
	fstat(fp,fd_stat);

    size = fd_stat->st_size;
	
	pChar = (char *)malloc(sizeof(char)*size);
	memset(pChar, 0, sizeof(char)*size ) ;

	if(pChar == NULL)
	{
		close(fp);
		return FALSE;
	}

	if(!read(fp, pChar, size))
	{
		close(fp);
		return FALSE;
	}
	*/
	close(fp);

	return TRUE;

}


/*********************************************************************\
* Function ：从内存里读取地图数据	   
* Purpose ：实现读取地图      
* Params ：nlevel（当前关数）  	   
* Return ：无 	 	   
* Remarks ：	   
**********************************************************************/
static	void	ReadBoxMap(int nlevel)
{
	char pp[15] = "nLevel_";
	char* ptemp;
	char p[5];
	int ntemp;
	int i, j;

	MyIntToStr(p, nlevel);
	strcat(pp, p);
	ptemp = strstr(pChar, pp);
	ntemp = ptemp - pChar + strlen(pp) + 4;

	row = (pChar[ntemp]-48)*10 + (pChar[ntemp+1]-48);
	line = (pChar[ntemp+2]-48)*10 + (pChar[ntemp+3]-48);
	ntemp = ntemp + 4;
	
	for(i=0; i<row; i++)
	{
		for(j=0; j<line; j++)
		{
			GameMap[i][j] = (pChar[ntemp]-48);
			ntemp++;
		}
		ntemp++;
	}
	ntemp = ntemp + 3;
}

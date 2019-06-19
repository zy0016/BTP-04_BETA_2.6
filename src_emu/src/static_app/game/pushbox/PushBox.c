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
* Purpose  : Ӧ�ó�����ں���
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
* Function����Ϸ�������洰�ڹ��̵���Ϣ������	   
* Purpose ����Ϸ������     
* Params ��	hWnd, message, wParam, lParam   
* Return �� LRESULT	 	   
* Remarks ��	   
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
			
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0), (LPARAM)IDS_EXIT);// ��Ŧ
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)IDS_NEW);// ��Ŧ
			
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
		
		//-->�˳�Ӧ�ó���
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
* Function����Ϸ�����ý��洰�ڹ���	   
* Purpose ����Ϸ���ý���     
* Params ��	ParenthWnd   
* Return �� DWORD	 	   
* Remarks ��	   
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
* Function����Ϸ�����ý��洰�ڹ��̵���Ϣ������	   
* Purpose ����Ϸ���ý���     
* Params ��	hWnd, message, wParam, lParam   
* Return �� LRESULT	 	   
* Remarks ��	   
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
			
			//��Ϸѡ������
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
			
			for (i = 0; i < LEVEL_COUNT; i++) //��ʮ��
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
* Function����Ϸ���洰�ڹ���	   
* Purpose ����Ϸ����     
* Params ��	ParenthWnd   
* Return �� DWORD	 	   
* Remarks ��	   
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
* Function����Ϸ����Ϸ���洰�ڹ��̵���Ϣ������	   
* Purpose ����Ϸ����     
* Params ��	hWnd, message, wParam, lParam   
* Return �� LRESULT	 	   
* Remarks ��	   
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
		    FindMan();//�ҵ�С�˶��ĵ�ǰλ��

			hPushBoxbg = CreateBitmapFromImageFile(hdc, PushBoxbg, &clr, &bclarity);
			hPushBoxArea = CreateBitmapFromImageFile(hdc, PushBoxArea, &clr, &bclarity);
			hPushBoxBox = CreateBitmapFromImageFile(hdc, PushBoxBox, &clr, &bclarity);
			hPushBoxWall = CreateBitmapFromImageFile(hdc, PushBoxWall, &clr, &bclarity);
			hPushBoxTemini = CreateBitmapFromImageFile(hdc, PushBoxTemini, &clr, &bclarity);
			hPushBoxMan1 = CreateBitmapFromImageFile(hdc, PushBoxMan1, &clr1, &bclarity);
//			hPushBoxMan2 = CreateBitmapFromImageFile(hdc, PushBoxMan2, &clr1, &bclarity);

		    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0), (LPARAM)IDS_RETURN);// �˳���ť
		    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CANCEL,1), (LPARAM)IDS_BACK);// �˻ذ�ť
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_REPLAY,2), (LPARAM)IDS_REPLAY); //���水ť
			
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

	case WM_PAINT://��ʾ��ͼ
		{
			hdc = BeginPaint(hWnd, &ps);

			DrawBackGround(hdc);

			CenterShow(hWnd, hdc);
			ShowMap(hdc);//����ÿ��ˢ�º�ĵ�ͼ����ʾ��ͼ
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
				PushBoxControlMoveK(hWnd, hdc, wParam);//��������Ϸ����Ӧ������Ϣ�Ŀ��ƺ���
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
			case TIMER_IDM://����㵽�յ�����ҵ������·��һ��һ������ʾ����
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
* Function����Ϸ���ؽ��洰�ڹ���	   
* Purpose ����Ϸ���ؽ���     
* Params ��	ParenthWnd   
* Return �� DWORD	 	   
* Remarks ��	   
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
	
	SendMessage(hPushBoxSuccessWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)IDS_CONFIRM);//(LPARAM)IDS_EXIT);//"�˳�" ��Ŧ
	SendMessage(hPushBoxSuccessWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0), (LPARAM)IDS_RETURN);//(LPARAM)IDS_MENU_OK);//ȷ�� ��Ŧ

	ShowWindow(hPushBoxSuccessWnd,SW_SHOW);
	UpdateWindow(hPushBoxSuccessWnd);

	return dwRet;

}


/*********************************************************************\
* Function����Ϸ�Ĺ��ؽ��洰�ڹ��̵���Ϣ������	   
* Purpose ����Ϸ���ؽ���     
* Params ��	hWnd, message, wParam, lParam   
* Return �� LRESULT	 	   
* Remarks ��	   
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
* Function����Ϸ�ص׽��洰�ڹ���	   
* Purpose ����Ϸ�ص׽���     
* Params ��	ParenthWnd   
* Return �� DWORD	 	   
* Remarks ��	   
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
	
	SendMessage(hPushBoxEndGameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)IDS_CONFIRM);//(LPARAM)IDS_EXIT);//"�˳�" ��Ŧ
	SendMessage(hPushBoxEndGameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_EXIT,0), (LPARAM)IDS_RETURN);//(LPARAM)IDS_MENU_OK);//ȷ�� ��Ŧ

	ShowWindow(hPushBoxEndGameWnd,SW_SHOW);
	UpdateWindow(hPushBoxEndGameWnd);

	return dwRet;

}


/*********************************************************************\
* Function����Ϸ�Ĺص׽��洰�ڹ��̵���Ϣ������	   
* Purpose ����Ϸ�ص׽���     
* Params ��	hWnd, message, wParam, lParam   
* Return �� LRESULT	 	   
* Remarks ��	   
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

			case IDM_EXIT://�˳��ô��ں���Ϸ���� 

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
* Purpose  : ��Ϸ˵������
*
* Explain  : ע�Ტ��������
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
* Purpose      �������ͷ��ʾ
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
* Purpose      �õ�������
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
* Purpose      ���˵���
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
* Function ���������ӵ��ƶ������ݲ�ͬ�����ƶ�һ��	   
* Purpose ���ƶ�ģ���ʵ��    
* Params ��Direction�����򣩣� j������xֵ����  i������yֵ�� 	   
* Return ��POINT	 	   
* Remarks ��	   
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
* Function ������С�˶����ƶ������ݲ�ͬ�����ƶ�һ��	   
* Purpose ���ƶ�ģ���ʵ��    
* Params ��Direction�����򣩣� j������xֵ����  i������yֵ�� 	   
* Return ��POINT	 	   
* Remarks ��	   
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
* Function ���趨�����	   
* Purpose ����Ӧ������Ϣ     
* Params ��wParam	   
* Return ��char	 	   
* Remarks ��	   
**********************************************************************/
static	char	PushBoxDirectionKeyDown(WPARAM wParam)
{
	char Direction = NULL;

	switch(wParam)
	{
	case VK_UP://�ϼ�ͷ����ʾ����Ϊ��
// 	case VK_2://2����ʾ����Ϊ��

		Direction = 'U';

		break;

	case VK_DOWN://�¼�ͷ����ʾ����Ϊ��
// 	case VK_8://8����ʾ����Ϊ��

		Direction = 'D';

		break;
		
	case VK_LEFT://���ͷ����ʾ����Ϊ��
// 	case VK_4://4����ʾ����Ϊ��

		Direction= 'L';

		break;
		
	case VK_RIGHT://�Ҽ�ͷ����ʾ����Ϊ��
// 	case VK_6://6����ʾ����Ϊ��

		Direction= 'R';

		break;
	}
	return Direction;
}


/*********************************************************************\
* Function ���ҵ�С�˵�λ��	   
* Purpose ��ȷ��С�˶���λ��  
* Params ����	   
* Return ��POINT	 	   
* Remarks ��	   
**********************************************************************/
static	POINT	FindMan(void)
{
	int i,j;

	for(i=0; i<row; i++)//�ҵ�С��
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
* Function ���õ���һ����߼�����	   
* Purpose ���ҵ���һ���λ��     
* Params ��prePoint����ǰ������꣩��Direction���ƶ�����	   
* Return ��POINT	 	   
* Remarks ��	   
**********************************************************************/
static	POINT	GetNextPoint(POINT prePoint, char Direction)
{
	POINT NextPoint;

	if(Direction == 'L')//�����ƶ�
	{
		NextPoint.x = prePoint.x-1;
	    NextPoint.y = prePoint.y;
	}
	
	else if(Direction == 'R')//�����ƶ�
	{
		NextPoint.x = prePoint.x+1;
	    NextPoint.y = prePoint.y;
	}

	else if(Direction == 'U')//�����ƶ�
	{
		NextPoint.x = prePoint.x;
		NextPoint.y = prePoint.y-1;
	}

	else if(Direction == 'D')//�����ƶ�
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
* Function ���ж���һ����ʲô	   
* Purpose ���õ���һ���ͼ����     
* Params ��curPoint���õ�λ�����꣩	   
* Return ��char	 	   
* Remarks ��	   
**********************************************************************/
static	char	GetStateByPoint(POINT curPoint)
{
	return GameMap[curPoint.y][curPoint.x];
}


/*********************************************************************\
* Function �����Ժ���	   
* Purpose ���õ���һ������꣬���ж�����ʲô      
* Params ��curPoint����ǰ��λ�����꣩��IsWhat����ͼ���ݣ��� Direction���ƶ����� 	   
* Return ��char	 	   
* Remarks ��	   
**********************************************************************/
static	char	TestPosition(POINT curPoint, char IsWhat, char Direction)
{
	curPoint = GetNextPoint(curPoint, Direction);
	IsWhat = GetStateByPoint(curPoint);

	return IsWhat;
}


/*********************************************************************\
* Function �����ñ�ʶ��	   
* Purpose ���ƶ�ģ�������б�ʶ���ĳ�ʼ��      
* Params ��Direction���ƶ�����	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	SettingSign(char Direction)
{
	POINT curPoint;
	char IsWhat;
	
	//��ʶ����ʼ��	
    gamestate.IsAreaOnBox = FALSE;
	gamestate.IsAreaOnMan = FALSE;
	gamestate.IsBoxOnBox = FALSE;
	gamestate.IsBoxOnMan = FALSE;
	gamestate.IsTerminiOnBox = FALSE;
	gamestate.IsTerminiOnMan = FALSE;
	gamestate.IsWallOnBox = FALSE;
	gamestate.IsWallOnMan = FALSE;
	
	curPoint = GetNextPoint(gamestate.Man, Direction);//�õ�С�˶�����һ������
    IsWhat = GetStateByPoint(curPoint);//�ж���һ����ʲô
	
	switch(IsWhat)//�����жϵĽ�����ñ�ʶ��
	{
	case 1://С�˶�ǰ��ǽ

		gamestate.IsWallOnMan = TRUE;

		break;
		
	case 5://С�˶��ǿյ�

		gamestate.IsAreaOnMan = TRUE;

		break;
		
	case 4://С�˶�ǰ��Ŀ�ĵ�

		gamestate.IsTerminiOnMan = TRUE;

		break;
		
	case 2://С�˶�ǰ������
	case 7://С�˶�ǰ��������Ŀ�ĵ���

		gamestate.IsBoxOnMan = TRUE;
		IsWhat = TestPosition(curPoint, IsWhat, Direction);//�ж�����ǰ��ʲô
		
		switch(IsWhat)
		{
		case 2://������
		case 7://����������Ŀ�ĵ���

			gamestate.IsBoxOnBox = TRUE;

			break;
			
		case 1://��ǽ

			gamestate.IsWallOnBox = TRUE;

			break;
			
		case 4://��Ŀ�ĵ�

			gamestate.IsTerminiOnBox = TRUE;

			break;
			
		case 5://�ǿյ�

			gamestate.IsAreaOnBox = TRUE;

			break;
		}
		break;
	}
	
}


/*********************************************************************\
* Function �������ͼ�����Ӳ���Ŀ�ĵ��ϵĸ���	   
* Purpose ���õ���ͼ�����Ӳ���Ŀ�ĵ��ϵĸ���      
* Params ����	   
* Return ��char	 	   
* Remarks ��	   
**********************************************************************/
static	char	GetBoxNum(void)
{
	int i,j;

	char TerminiNum = 0;

	for(i=0; i<row; i++)
	{
		for(j=0; j<line; j++)
		{
			if(GameMap[i][j] == 2)//����
				TerminiNum++;
		}
	}
	return TerminiNum;

}


/*********************************************************************\
* Function ���ж���Ϸ�Ƿ����	   
* Purpose ���ж���Ϸ�Ƿ����      
* Params ����	   
* Return ��BOOL	 	   
* Remarks ��	   
**********************************************************************/
static	BOOL	IsGameOver(void)
{
	//�ж�Ŀ�ĵ����Ƿ���������
	if(0 == GetBoxNum())
	{
//		MoveNum = 0;
		return TRUE;
	}

	return FALSE;
}


/*********************************************************************\
* Function �����ƶ����С�ˣ�������ԭ����С��	   
* Purpose ��ʵ���ƶ�      
* Params ��hdc�� Direction���ƶ�����	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	DrawMoveMan(HDC hdc, char Direction)
{
	POINT curPoint;
	curPoint = MoveMan(Direction, gamestate.Man.x, gamestate.Man.y);//�õ��ƶ����С�˶�������
	
	if(GameMap[gamestate.Man.y][gamestate.Man.x] == 6)//����ƶ�ǰС�˶�����Ŀ�ĵ���
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 4;//���µ�ͼ����С���ƶ�ǰ��λ����ΪĿ�ĵ�
	}

	else//�����ƶ�ǰС�˶����ڿյ���
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 5;//���µ�ͼ����С���ƶ�ǰ��λ����Ϊ�յ�
	}

	ShowMapBlock(hdc, gamestate.Man.x, gamestate.Man.y);

	if(GameMap[curPoint.y][curPoint.x] == 4)//����ƶ����λ��ԭ����Ŀ�ĵ�
		GameMap[curPoint.y][curPoint.x] = 6;//�Ѹĵ���ΪС�˶���Ŀ�ĵ���

	else
		GameMap[curPoint.y][curPoint.x] = 3;//�����ƶ����λ��ԭ��ֻ���ǿյأ����µ�ͼ��������ΪС�˶�

	ShowMapBlock(hdc, curPoint.x, curPoint.y);

	gamestate.Man.x = curPoint.x;//��С�˶��ƶ�����߼����꼴��ǰλ�ã�������ȫ�ֱ�����
	gamestate.Man.y = curPoint.y;
}


/*********************************************************************\
* Function �����ƶ�������Ӻ�С�ˣ�������ԭ�������Ӻ�С��	   
* Purpose ��ʵ���ƶ�      
* Params ��hdc�� Direction���ƶ�����	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	DrawMoveBoxAndMan(HDC hdc, char Direction)
{
	POINT prePoint;
	POINT curPoint;
	prePoint = MoveMan(Direction, gamestate.Man.x, gamestate.Man.y);//�õ�С�˶��ƶ��������

	if(GameMap[gamestate.Man.y][gamestate.Man.x] == 6)//�ж�С�˶��ƶ�ǰ�Ƿ���Ŀ�ĵ��ϣ������
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 4;//���µ�ͼ���Ѹ�Ϊ����ΪĿ�ĵ�
	}
	
	else//������ǣ���Ϊ�յ�
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 5;//���µ�ͼ���Ѹ�Ϊ����Ϊ�յ�
	}

	ShowMapBlock(hdc, gamestate.Man.x, gamestate.Man.y);

	curPoint.x = prePoint.x;//����С�˶��ƶ��������
	curPoint.y = prePoint.y;

	if(GameMap[curPoint.y][curPoint.x] == 7)//�ж�С�˶��ƶ����λ��ԭ���Ƿ�Ϊ������Ŀ�ĵأ������
		GameMap[curPoint.y][curPoint.x] = 6;//���µ�ͼ���Ѹ�Ϊ����Ϊ����Ŀ�ĵ���

	else//������ǣ���λ��ԭ��һ���ǿյ�
		GameMap[curPoint.y][curPoint.x] = 3;//���µ�ͼ���Ѹ�λ����ΪС�˶�

	ShowMapBlock(hdc, curPoint.x, curPoint.y);

	prePoint = MoveBox(Direction, prePoint.x, prePoint.y);//�õ������ƶ��������

	if(GameMap[prePoint.y][prePoint.x] == 4)//�ж������ƶ����ǲ�����Ŀ�ĵ��ϣ������
		GameMap[prePoint.y][prePoint.x] = 7;//���µ�ͼ���Ѹ�λ����Ϊ������Ŀ�ĵ���

	else//������ǣ������ƶ���һ���ڿյ���
		GameMap[prePoint.y][prePoint.x] = 2;//���µ�ͼ���ѵ�ǰλ����Ϊ����

	ShowMapBlock(hdc, prePoint.x, prePoint.y);

	gamestate.Man.x = curPoint.x;//���ƶ����С�˶����߼����걣����ȫ�ֱ�����
	gamestate.Man.y = curPoint.y;
}


/*********************************************************************\
* Function ���õ��ƶ�ǰ�ĵ�����꣬������������PENDOWN��Ϣ��	   
* Purpose �����������Ϣ      
* Params ����	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	GetFormerPointP(POINT curPoint, POINT prePoint, POINT LastPoint)
{
	pList.ManPoint = curPoint;//?
	pList.TheLastPoint = prePoint;
	pList.AndTheLastPoint = LastPoint;
}
/*********************************************************************\
* Function ���õ��ƶ�ǰ�ĵ�ԭ����ʾ����ʲô��������������PENDOWN��Ϣ��	   
* Purpose �����������Ϣ      
* Params ��Direction���ƶ�����	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	GetFormerPlaceIsWhatP(void)
{
	pList.TheFirstPointIsWhat = GameMap[pList.ManPoint.y][pList.ManPoint.x];
	pList.TheSecondPointIsWhat = GameMap[pList.TheLastPoint.y][pList.TheLastPoint.x];
	pList.TheThirdPointIsWhat = GameMap[pList.AndTheLastPoint.y][pList.AndTheLastPoint.x];
}

/*********************************************************************\
* Function ���õ��ƶ�ǰ�ĵ�����꣬������������KEYDOWN��Ϣ��	   
* Purpose �����������Ϣ      
* Params ��Direction���ƶ�����	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	GetFormerPointK(char Direction)
{
	pList.ManPoint = FindMan();//�ҵ���ǰ��С��λ��
	pList.TheLastPoint = GetNextPoint(pList.ManPoint, Direction);
	pList.AndTheLastPoint = GetNextPoint(pList.TheLastPoint, Direction);
}


/*********************************************************************\
* Function ���õ��ƶ�ǰ�ĵ�ԭ����ʾ����ʲô��������������KEYDOWN��Ϣ��	   
* Purpose �����������Ϣ      
* Params ��Direction���ƶ�����	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	GetFormerPlaceIsWhatK()
{
	pList.TheFirstPointIsWhat = GameMap[pList.ManPoint.y][pList.ManPoint.x];
	pList.TheSecondPointIsWhat = GameMap[pList.TheLastPoint.y][pList.TheLastPoint.x];
	pList.TheThirdPointIsWhat = GameMap[pList.AndTheLastPoint.y][pList.AndTheLastPoint.x];
}


/*********************************************************************\
* Function ��ʵ�ֺ���һ����ˢ�µ�ͼ	   
* Purpose ��ʵ�ֺ��˹���      
* Params ��hdc�� *q����ź�����Ϣ�������еĽڵ㣩	   
* Return ����	 	   
* Remarks ��	   
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
* Function �����ݱ�־����ʾ�����ͷ	   
* Purpose �����ݱ�־����ʾ�����ͷ     
* Params ��hWnd��Disappear������С����Χ��ʲô�����õı�־���� 	   
* Return ��void	 	   
* Remarks ��	   
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
* Function ���ж�С����Χ��ʲô	   
* Purpose ��������ʾ�ļ�ͷ     
* Params ��Direction	   
* Return ��void	 	   
* Remarks ��	   
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
* Function �������ƶ���ʵ��	   
* Purpose ������״̬��ʵ��С�˶��������ƶ��ĺ���(����KEYDOWN��Ϣ)      
* Params ��hdc�� wParam	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	PushBoxControlMoveK(HWND hWnd, HDC hdc, WPARAM wParam)
{
	char Direction;
	ListUnit* p;
	Direction = PushBoxDirectionKeyDown(wParam);//ͨ����Ӧ������Ϣ�õ��ƶ�����
	SettingSign(Direction);//���ñ�ʶ��
	
    //���ݱ�ʶ�������ƶ����
	if(gamestate.IsWallOnMan)
		return;

	if((gamestate.IsBoxOnBox)||(gamestate.IsWallOnBox))
		return;

	else
	{
		GetFormerPointK(Direction);

		if(gamestate.IsAreaOnMan == TRUE)//С�˶�ǰ�ǿյ�
		{
			GetFormerPlaceIsWhatK();
			DrawMoveMan(hdc, Direction);//��ʾ�ƶ���С�˶�
		}
		
		else if(gamestate.IsTerminiOnMan == TRUE)//���С�˶�ǰ��Ŀ�ĵ�
		{
			GetFormerPlaceIsWhatK();
			DrawMoveMan(hdc, Direction);//��ʾ�ƶ���С�˶�
		}
		
		else if(gamestate.IsBoxOnMan == TRUE)//���С�˶�ǰ������
		{
			if(gamestate.IsAreaOnBox == TRUE)//��������ǰ�ǿյ�
			{
				GetFormerPlaceIsWhatK();
				DrawMoveBoxAndMan(hdc, Direction);//��ʾ�ƶ������Ӻ�С�˶�
			}
			
			else if(gamestate.IsTerminiOnBox == TRUE)//��������ǰ��Ŀ�ĵ�
			{
				GetFormerPlaceIsWhatK();
				DrawMoveBoxAndMan(hdc, Direction);//��ʾ�ƶ������Ӻ�С�˶�
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
* Function �������ƶ���ʵ��	   
* Purpose ������״̬��ʵ��С�˶��������ƶ��ĺ���(����PENDOWN��Ϣ)      
* Params ��hdc�� wParam	   
* Return ����	 	   
* Remarks ��	   
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
	
	prePoint = FindMyPenInMap();//��¼�ʵ��µ�λ������
	if(prePoint.x == -100)
		return 1;
	
	IsWhat = GetStateByPoint(prePoint);//�жϱʵ��µ�λ����ʲô
	if(IsWhat == 0)					   //���µ��Ǳ����Ļ�Ҳ�˳���
		return 1;
	hdc = GetDC(hWnd);
	
	if(IsWhat == 5 || IsWhat == 4)//������µ�λ���ǿյػ�Ŀ�ĵ�
	{
		curPoint = FindMan();
		tempDist = SearchPath(curPoint, prePoint);
		
		if(tempDist != 0)//�趨��ʱ��
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

	if((IsWhat == 2) || (IsWhat == 7))//������µ�λ�������ӣ��ж�С���Ƿ���������Χ�����ĸ�����
	{
		int IsBoxMove;
		IsBoxMove = 0;
		curPoint = FindMan();
		Distance = -1;
		if((prePoint.y == curPoint.y-1) && (prePoint.x == curPoint.x))//���������С�˵��ϱ�
		{
			Direction = 'U';
			
			LastPoint = GetNextPoint(prePoint, Direction);
			IsWhat = GetStateByPoint(LastPoint);//�ж����ӵ��ϱ���ʲô
			if(IsWhat == 5 || IsWhat ==4)//����ǿյػ�Ŀ�ĵ�
			{
				GetFormerPointP(curPoint, prePoint, LastPoint);
				GetFormerPlaceIsWhatP();
				DrawMoveBoxAndMan(hdc, Direction);//���������ƶ�
				IsBoxMove = 1;
			}
		}
		
		else if((prePoint.y == curPoint.y+1) && (prePoint.x == curPoint.x))//���������С�˵��±�
		{
			Direction = 'D';
			LastPoint = GetNextPoint(prePoint, Direction);
			IsWhat = GetStateByPoint(LastPoint);//�ж����ӵ��±���ʲô
			if(IsWhat == 5 || IsWhat ==4)//����ǿյػ�Ŀ�ĵ�
			{
				GetFormerPointP(curPoint, prePoint, LastPoint);
				GetFormerPlaceIsWhatP();
				DrawMoveBoxAndMan(hdc, Direction);//���������ƶ�
				IsBoxMove = 1;
			}
		}
		
		else if((prePoint.y == curPoint.y) && (prePoint.x == curPoint.x-1))//���������С�˵����
		{
			Direction = 'L';
			LastPoint = GetNextPoint(prePoint, Direction);
			IsWhat = GetStateByPoint(LastPoint);//�ж����ӵ������ʲô
			if(IsWhat == 5 || IsWhat ==4)//����ǿյػ�Ŀ�ĵ�
			{
				GetFormerPointP(curPoint, prePoint, LastPoint);
				GetFormerPlaceIsWhatP();
				DrawMoveBoxAndMan(hdc, Direction);//���������ƶ�
				IsBoxMove = 1;
			}
		}
		
		else if((prePoint.y == curPoint.y) && (prePoint.x == curPoint.x+1))//���������С�˵��ұ�
		{
			Direction = 'R';
			LastPoint = GetNextPoint(prePoint, Direction);
			IsWhat = GetStateByPoint(LastPoint);//�ж����ӵ��ұ���ʲô
			if(IsWhat == 5 || IsWhat ==4)//����ǿյػ�Ŀ�ĵ�
			{
				GetFormerPointP(curPoint, prePoint, LastPoint);
				GetFormerPlaceIsWhatP();
				DrawMoveBoxAndMan(hdc, Direction);//���������ƶ�
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
* Function ���ͷ���Դ��������ʾ��Ϸ����	   
* Purpose ��������һ����Ϸ      
* Params ��hWnd	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
//�ͷ���Դ��������ʾ��Ϸ����
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
* Function ���ҵ��ʵ�λ��	   
* Purpose ���ڵ�ͼ��ȷ���ʵ�λ��  
* Params ����	   
* Return ��POINT	 	   
* Remarks ��	   
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
* ������   ��	FindNextAdj
* ��  ��   ��	Ѱ�Ҹ��������һ���ڽӵ�
* ����˵�� ��	NextPt:		���������
* ����ֵ   ��	1:	�ҵ��ڽӵ�		0:	û�ҵ��ڽӵ�
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
* ������   ��	SearchPath
* ��  ��   ��	Ѱ�Ҵ���ʼ�㵽��ֹ���·��
* ����˵�� ��	ResPt:	��ʼ�������
				DesPt:	��ֹ�������
				Path:	��¼����ʼ�㵽��ֹ���·��
* ����ֵ   ��	����ʼ�㵽��ֹ��ľ��룬����0��ʾû�ҵ�·��
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
		while (FindNextAdj(isnew, Pt, &NextPt) != 0)//Ѱ����һ���ڽӵ�
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
* Function ��С���Լ��ƶ�ʱ�����ƶ����С�ˣ�������ԭ����С��	   
* Purpose ��ʵ��С�˵������ƶ�      
* Params ��hdc�� j,i���ƶ����꣩	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	DrawAutoMoveMan(HDC hdc, short j, short i)
{
	//POINT curPoint;
	//curPoint = MoveMan(Direction, gamestate.Man.x, gamestate.Man.y);//�õ��ƶ����С�˶�������
	FindMan();
	
	if(GameMap[gamestate.Man.y][gamestate.Man.x] == 6)//����ƶ�ǰС�˶�����Ŀ�ĵ���
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 4;//���µ�ͼ����С���ƶ�ǰ��λ����ΪĿ�ĵ�
		DrawTermini(hdc, gamestate.Man.x, gamestate.Man.y);//��ʾĿ�ĵ�
	}

	else//�����ƶ�ǰС�˶����ڿյ���
	{
		GameMap[gamestate.Man.y][gamestate.Man.x] = 5;//���µ�ͼ����С���ƶ�ǰ��λ����Ϊ�յ�
	    DrawArea(hdc, gamestate.Man.x, gamestate.Man.y);//��ʾ�յ�
	}

	gamestate.pCurManState = GameMap[i][j];
	if(GameMap[i][j] == 4)//����ƶ����λ��ԭ����Ŀ�ĵ�
		GameMap[i][j] = 6;//�Ѹĵ���ΪС�˶���Ŀ�ĵ���

	else
		GameMap[i][j] = 3;//�����ƶ����λ��ԭ��ֻ���ǿյأ����µ�ͼ��������ΪС�˶�

	DrawMan(hdc, j, i);//���ƶ����λ����ʾΪС�˶�

	gamestate.Man.x = j;//��С�˶��ƶ�����߼����꼴��ǰλ�ã�������ȫ�ֱ�����
	gamestate.Man.y = i;
}


/*********************************************************************\
* Function ��������ͷ��������	   
* Purpose �����������Ϣ     
* Params ��**ppList������ͷ�ڵ㣩�� *pList����Ҫ����Ľڵ㣩
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	InsertDataToList( ListUnit **ppList, ListUnit *pList)
{
	ListUnit *p;

	/*�����������������Լ���������ͷ,����������ͷ*/
	if (*ppList == NULL)
	{
		*ppList = pList;
		(*ppList)->pNext = NULL;
		return ;
	}

	/*�������*/
	p = pList;
   	p->pNext = *ppList;
	*ppList = p;
}


/*********************************************************************\
* Function ��ɾ������ı�β���������ڶ�������Ϊ�µı�β	   
* Purpose ��������ֻ��������15���ڵ����Ϣ������ǰ����Ϣ��������Ĳ�����һɾ��     
* Params ��**ppList������ͷ�ڵ㣩
* Return ��int	 	   
* Remarks ��	   
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
* Function ��ɾ������ı�ͷ	   
* Purpose ��ÿ����һ���󣬰Ѹõ���Ϣɾ��
* Params ��**ppList������ͷ�ڵ㣩
* Return ��int	 	   
* Remarks ��	   
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
* Function ���ͷ�����	   
* Purpose ��ÿһ����Ϸ�����ͷ���Դ
* Params ��**ppList������ͷ�ڵ㣩
* Return ��int	 	   
* Remarks ��	   
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
* Function ��ˢ����	   
* Purpose ��ˢ����      
* Params ��hdc��rgb��hWnd  	   
* Return ����	 	   
* Remarks ��	   
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
* Function ������ͼ��ı���	   
* Purpose ������ͼ��ı���      
* Params ��hdc 	   
* Return ����	 	   
* Remarks ��	   
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
* Function ������ͼ��Ŀյ�	   
* Purpose ������ͼ��Ŀյ�      
* Params ��hdc��j������xֵ����i ������xֵ�� 	   
* Return ����	 	   
* Remarks ��	   
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
* Function ������ͼ���ǽ	   
* Purpose ������ͼ���ǽ      
* Params ��hdc��j������xֵ����i ������xֵ�� 	   
* Return ����	 	   
* Remarks ��	   
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
* Function ������ͼ���С�˶�	   
* Purpose ������ͼ���С�˶�      
* Params ��hdc��j������xֵ����i ������xֵ�� 	   
* Return ����	 	   
* Remarks ��	   
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
* Function ������ͼ���Ŀ�ĵ�	   
* Purpose ������ͼ���Ŀ�ĵ�      
* Params ��hdc��j������xֵ����i ������xֵ�� 	   
* Return ����	 	   
* Remarks ��	   
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
* Function ������ͼ�������	   
* Purpose ������ͼ�������      
* Params ��hdc��j������xֵ����i ������xֵ�� 	   
* Return ����	 	   
* Remarks ��	   
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
* Function ���������ݻ�һ���ͼ	   
* Purpose ��ʵ�ֵ�ͼ����ʾ     
* Params ��hdc��j������xֵ����i ������xֵ�� 	   
* Return ����	 	   
* Remarks ��	   
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
* Function ���������	   
* Purpose ����ʾ��ͼʱ��Ϊ��������ۣ�������ʾ    
* Params ��hWnd�� hdc 	   
* Return ����	 	   
* Remarks ��	   
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
* Function ����ʾ��ͼ	   
* Purpose ��ʵ�ֵ�ͼ����ʾ     
* Params ��hdc 	   
* Return ����	 	   
* Remarks ��	   
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
* Function ���ѵ�ǰ����д���ļ�	   
* Purpose ��ʵ�֡������ϴ���Ϸ�Ĺ��ܡ�      
* Params ���� 	   
* Return ����	 	   
* Remarks ��	   
**********************************************************************/
static	void	WriteLevelToFile(void)
{
	//���ļ�����������ڴ����ļ�
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
* Function �����ļ��ж�����Ϸ����	   
* Purpose ��ʵ�֡������ϴ���Ϸ�Ĺ��ܡ�      
* Params ���� 	   
* Return ��int	 	   
* Remarks ��	   
**********************************************************************/
static	void	ReadLevelFromFile(void)
{
	int	fp;
	//int curnlevel;

	fp = open(PushBoxLevel, O_RDONLY|O_CREAT,0666);//���ļ�

	read(fp, &curnLevel, 1);

	close(fp);
	//return curnlevel;
}


/*********************************************************************\
* Function ����int�ͱ���ת����char�ͱ���	   
* Purpose ��ʵ�ֵ�ͼ�洢      
* Params ��*pStr���ַ�������num��int�ͱ�����  	   
* Return ����	 	   
* Remarks ��	   
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
* Function ���ѵ�ͼ�ļ������ڴ�	   
* Purpose ��ʵ�ֵ�ͼ�洢      
* Params ����  	   
* Return ��BOOL	 	   
* Remarks ��	   
* Extral :it seems that function open and fstat can't work well together,
		  I modify the code of function ReadMapFromFile to do the same 
		  thing in 04-05 2005.
**********************************************************************/
static	BOOL	ReadMapFromFile()
{

	int	fp;
	int size;
	//struct stat *fd_stat=NULL;
	//fp = CreateFile("Rom:MyMap.txt", O_RDONLY, 0);//���ļ�

	fp = open(PushBoxMap, O_RDONLY);//���ļ�
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
* Function �����ڴ����ȡ��ͼ����	   
* Purpose ��ʵ�ֶ�ȡ��ͼ      
* Params ��nlevel����ǰ������  	   
* Return ���� 	 	   
* Remarks ��	   
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

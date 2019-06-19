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
#define __MODULE__ "Diamond:"

#include "Diamond.h"
#include "fcntl.h"
#include "unistd.h"


static BOOL	bDiamondWndClass = FALSE;
static BOOL	bDiamondMainWndClass = FALSE;
static	HMENU		hDiamondMenu	= NULL;

static MENUITEMTEMPLATE DiamondMenu[] =
{
	{ MF_STRING, IDM_NEWGAME, "New", NULL},
	{ MF_STRING, IDM_HELP, "help", NULL },
//	{ MF_STRING, ID_SETTING, IDS_SETTING, NULL },
    { MF_END, 0, NULL, NULL }
};
//#endif

static const MENUTEMPLATE DiamondMenuTemplate =
{
    0,
    DiamondMenu
};

//extern char Diamond_ver    [16];

/*
**  Function : File Manager_AppControl
**  Purpose  :
**      Application control funciton.
*/
DWORD GameDiamond_AppControl(int nCode, void* pInstance, WPARAM wParam,  LPARAM lParam)
{
    WNDCLASS wc;
    int		dwRet;

	dwRet = TRUE;

	switch (nCode)
    {
	
    case APP_INIT :
        hDiamondInstance = (HINSTANCE)pInstance;

        break;

    case APP_GETOPTION :

		if (wParam == AS_ICON)
			return NULL;		

		if (wParam == AS_STATE)
		{
			if(hWnd_Main)
			{
			   if (IsWindowVisible(hWnd_Main))
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
			dwRet = (DWORD)hWnd_Main;
		break;	

    case APP_ACTIVE :

        if (hWnd_Main)
        {
   			ShowWindow(hWnd_Main, SW_SHOW);
			ShowOwnedPopups(hWnd_Main, TRUE);
            bShow = TRUE; //?       

        }
        else
        {
#ifdef _MODULE_DEBUG_
			StartObjectDebug();
#endif
			if (!bDiamondMainWndClass)
			{
				// Register Diamond window class
				wc.style         = 0;
				wc.lpfnWndProc   = AppMainWndProc;
				wc.cbClsExtra    = 0;
				wc.cbWndExtra    = 0;
				wc.hInstance     = NULL;//hInstance;
				wc.hIcon         = NULL;
				wc.hCursor       = NULL;
				wc.hbrBackground = GetStockObject(BLACK_BRUSH);
				wc.lpszMenuName  = NULL;
				wc.lpszClassName = "DiamondMainWndClass";
				
				if (!RegisterClass(&wc))
					return FALSE;
				
				bDiamondMainWndClass = TRUE;
			}
			hDiamondMenu = LoadMenuIndirect(&DiamondMenuTemplate);
			ModifyMenu(hDiamondMenu, IDM_NEWGAME, MF_BYCOMMAND, IDM_NEWGAME, IDS_NEWGAME);
			ModifyMenu(hDiamondMenu, IDM_HELP, MF_BYCOMMAND, IDM_HELP, IDS_HELP);

            // Creates setup window
            hWnd_Main = CreateWindow(
                "DiamondMainWndClass", 
                IDS_DIAMOND, 
                WS_VISIBLE|PWS_STATICBAR|WS_CAPTION, 
                PLX_WIN_POSITION,
                NULL, 
                hDiamondMenu,
                NULL, 
                NULL
                );
			SendMessage(hWnd_Main, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
            ShowWindow(hWnd_Main, SW_SHOW);
			UpdateWindow(hWnd_Main);
			SetFocus(hWnd_Main);
            bShow = TRUE;
        }

		return (DWORD)hWnd_Main;

    case APP_INACTIVE :
		ShowWindow(hWnd_Main, SW_HIDE);
		ShowOwnedPopups(hWnd_Main, FALSE);

        break;

	
    default :

        break;
    }

    return dwRet;
}

/*****************************************************************************\
* Funcation: AppMainWndProc
*
* Purpose  : 主窗口窗口过程处理函数
*
* Explain  : 主窗口显示菜单        
*           
\*****************************************************************************/
static LRESULT CALLBACK AppMainWndProc (HWND hWnd, UINT wMsgCmd, WPARAM wParam,LPARAM lParam)
{
//------------------------------------------------------------------------
	LRESULT			lResult;
	HDC				hdc;
//--------------------------------------------------------------------------

	switch (wMsgCmd)	
    {
	case WM_CREATE:		
		{
			COLORREF Color;
			BOOL	bTran;
			hdc = GetDC(hWnd);
			hCoverBg = CreateBitmapFromImageFile(hdc,COVERBG,&Color,&bTran);
			DiamondBmp = CreateBitmapFromImageFile(hdc,DIAMONDS_PATH,&Color,&bTran);
			//hBmpMenu = CreateBitmapFromImageFile(hdc, PATH_MENU, &Color, &bTran);
			ReleaseDC(hWnd,hdc);
			
//			SetMenuItemRectLocal();
//			nSelMenuItem = 0;
//			ShowArrowState(hWnd,nSelMenuItem);
//			strcpy(MenuItemText[0],IDS_NEWGAME);
//			strcpy(MenuItemText[1],IDS_HELP);
			
			CreateCapButton(hWnd, IDM_EXIT, 0, IDS_EXIT);
			CreateCapButton(hWnd, IDM_CONFIRM, 1, IDS_NEWGAME);
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
				PostMessage(hWnd,WM_COMMAND,IDM_HELP,NULL);
				break;

			default:
				nSelMenuItem = 0;
				PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
				break;
			}
			break;

		case	IDM_NEWGAME:
			if (CfgFileExist())
			{
				PLXConfirmWinEx(NULL, hWnd, IDS_LOADRECORD, Notify_Request, IDS_DIAMOND, IDS_CONFIRM, IDS_CANCEL, WM_SELOK);
			}
			else
			{
				bLoadData = FALSE;
				DiamondGameProc(hWnd);
			}
			break;

		case IDM_HELP:
			OnGameHelp(hWnd);
			break;

		case	IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		}
		
		break;

	case WM_SELCANCEL:
		bLoadData = FALSE;
		DiamondGameProc(hWnd);
		break;

	case WM_SELOK:
		if (lParam == 1) {
			bLoadData = TRUE;
		}
		else
		{
			bLoadData = FALSE;
		}		
		DiamondGameProc(hWnd);
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
/*#ifndef _EMULATE_
					TouchAlert();
#endif//_EMULATE_*/
					PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
					break;

				case 1:
/*#ifndef _EMULATE_
					TouchAlert();
#endif//_EMULATE_*/
					PostMessage(hWnd,WM_COMMAND,IDM_HELP,NULL);
					break;

				default:
					i = 0;
					PostMessage(hWnd,WM_COMMAND,IDM_NEWGAME,NULL);
					break;
				}
				nSelMenuItem = i;
				ShowArrowState(hWnd,nSelMenuItem);
				InvalidateRect(hWnd,NULL,TRUE);
			}
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd,NULL);
		BitBlt(hdc, X_COVERBG,Y_COVERBG,
				COVERBG_WIDTH , COVERBG_HEIGHT , 
			   (HDC)hCoverBg, 0, 0, ROP_SRC);
//		{
//			int i;
//			for (i=0;i<MENU_ITEM_NUM;i++)
//			{
//				MyDrawMenuItem(hdc,i,nSelMenuItem);
//			}
//		}
		EndPaint(hWnd,NULL);
		break;
#ifdef _MXLINCOMMENT_

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{

		case VK_UP:
			if (0<nSelMenuItem)
			{
				nSelMenuItem--;				
			}
			else
				nSelMenuItem = MENU_ITEM_NUM-1;
			ShowArrowState(hWnd,nSelMenuItem);
			InvalidateRect(hWnd,NULL,TRUE);
			break;

		case VK_DOWN:
			if (MENU_ITEM_NUM-1>nSelMenuItem)
			{
				nSelMenuItem++;				
			}
			else
				nSelMenuItem = 0;

			ShowArrowState(hWnd,nSelMenuItem);
				InvalidateRect(hWnd,NULL,TRUE);
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

		if (NULL != hCoverBg)
		{
			DeleteObject(hCoverBg);
			hCoverBg = NULL;
		}

		if (NULL != DiamondBmp)
		{
			DeleteObject(DiamondBmp);
			DiamondBmp = NULL;
		}


//		if (hBmpMenu)
//		{
//			DeleteObject(hBmpMenu);
//			hBmpMenu = NULL;
//		}


		hWnd_Main = NULL;

		DlmNotify(PES_STCQUIT, (LPARAM)hDiamondInstance);
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

	CallGameHelpWnd(hWnd,IDS_HELPTEXT);

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
	if (i != nSel)
	{
		return FALSE;
	}

	OldClr = SetBkColor(hdc,RGB(255,255,255));
	OldStyle = SetBkMode(hdc, NEWTRANSPARENT);

//	BitBlt(hdc,
//		X_LT_MENUITEM,
//		Y_LT_MENUITEM,
//		ALLMENU_WIDTH,
//		ALLMENUHEIGHT,
//		(HDC)hBmpMenu,
//		0,0,ROP_SRC);

	SetBkColor(hdc,RGB(0,0,0));

	BitBlt(hdc,
		SelItemRect[i].left,
		SelItemRect[i].top,
		SELITEM_WIDTH,
		SELITEM_HEIGHT,
		(HDC)DiamondBmp,
		4*DIAMOND_WIDTH,0,ROP_SRC);

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
		if (PtInRectXY(&(MenuItemRect[i]),x,y))
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

	if (MENU_ITEM_NUM-1 == nSel)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKDOWN);
	if (0 == nSel)
		SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKUP);

	SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLMIDDLE, MASKMIDDLE);

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
		x1 = X_LT_MENUITEM;
		y1 = Y_LT_MENUITEM + i * Y_MENUITEM_INTERVAL;
		x2 = x1 + MENUITEM_WIDTH;
		y2 = y1 + MENUITEM_HEIGHT;
		SetRect(&(MenuItemRect[i]),x1,y1,x2,y2);
	}

	return TRUE;
}

/*
delay given time
*/
/**********************************************************/
//static void DrawColorBorder(HDC hdc, RECT *rc, int In);
/**********************************************************/
static void DrawColorBorder(HDC hdc, RECT *rc, int Color)
{
	HPEN hPen;
	hPen = CreatePen(PS_SOLID, 1, Color);
	SelectObject(hdc, hPen);
	DrawLine( hdc, rc->left, rc->top, rc->left, rc->bottom);
	DrawLine( hdc, rc->left, rc->top, rc->right, rc->top);
	DrawLine( hdc, rc->right, rc->top, rc->right, rc->bottom);
	DrawLine( hdc, rc->left, rc->bottom, rc->right, rc->bottom);
	DeleteObject(hPen);

	return;
}

/*********************************************************************\
* Function	   Bonus
* Purpose      计算奖分
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static int Bonus(int x)
{
	return x*x-3*x+4;
}

/*********************************************************************\
* Function	   Delaying
* Purpose      延迟
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void Delaying(int n)
{
/*   	
	char str[32];
	int i;
	for(i=0;i<n*SPEED;i++)
	{
		sprintf(str, " (%f), [%d]", (double)nMergeDiamondNum, TotalMergeDiamondNum);
#ifndef EMULATION 
		KickDog();
#endif
	}
*/ 
	return;
	
}

/*
load diamond bitmap handle 

*/
static void  InitDiamondBMP()
{
	
	//	DiamondBmp[0]=LoadImage(NULL,"ROM:diamond0.bmp", IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	//	DiamondBmp[1]=LoadImage(NULL,"ROM:diamond1.bmp", IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	//	DiamondBmp[2]=LoadImage(NULL,"ROM:diamond2.bmp", IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	//	DiamondBmp[3]=LoadImage(NULL,"ROM:diamond3.bmp", IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	//	DiamondBmp[4]=LoadImage(NULL,"ROM:diamond4.bmp", IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	//	DiamondBmp[5]=LoadImage(NULL,"ROM:diamond5.bmp", IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	//	DiamondBmp[6]=LoadImage(NULL,"ROM:diamond6.bmp", IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	
#if defined SCP_NOKEYBOARD
	{
		HDC hdc;
		COLORREF	Color;
		BOOL	bTran;

		hdc = GetDC(hWnd_Main);
		hNumBmp = CreateBitmapFromImageFile(hdc,DIAMOND_DIGITAL_PATH,&Color,&bTran);
		hGameBack = CreateBitmapFromImageFile(hdc,DIAMOND_BACK_PATH,&Color,&bTran);
		ReleaseDC(hWnd_Main,hdc);
	}
#else
	{
		HDC hdc;
		COLORREF	Color;
		BOOL	bTran;
		hdc = GetDC(hWnd_Main);

		hNumBmp = CreateBitmapFromImageFile(hdc,DIAMOND_DIGITAL_PATH,&Color,&bTran);
		hGameBack = CreateBitmapFromImageFile(hdc,DIAMOND_BACK_PATH,&Color,&bTran);
		hLittleBall = CreateBitmapFromImageFile(hdc,LITTLEBALL_PATH,&Color,&bTran);

		ReleaseDC(hWnd_Main,hdc);
		//	hNumBmp = LoadImage(NULL,DIAMOND_DIGITAL_PATH, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
		//	hGameBack = LoadImage(NULL,DIAMOND_BACK_PATH, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	}
#endif
	
	return;
}
/*
delete diamond bitmap handle
*/
static void  DeleteDiamondBMP()
{
/*	int i;
	for (i=0; i<7; i++)
	{
		DeleteObject(DiamondBmp[i]);
	}*/
	if (hNumBmp)
	{
		DeleteObject(hNumBmp);
		hNumBmp = NULL;
	}

	if (hGameBack)
	{
		DeleteObject(hGameBack);
		hGameBack = NULL;
	}

	if (hLittleBall)
	{
		DeleteObject(hLittleBall);
		hLittleBall = NULL;
	}

	return ;
}
/*
generate a new random number, value from 0--6, 
because rand() function return value from 0---32767, 
after 12 bits right shift, we get 0--7, then subtract 1 equle -1 to 6 ;
if we get -1 , regenerate random value again,untile the value into 0--6.   
*/
static int NewDiamond()
{
	static int ret ;
	ret = rand();
	//ret = ((DWORD)ret>>12)-1;
	ret = ret%7;
	if(ret<0)
	{
		return NewDiamond(); 
	}
	return ret;
}
/*
refresh Diamond 
*/
static void RedrawDiamond(int i, int j)
{
	RECT rcFocus;
	SetRect(&rcFocus, X_DIAMOND_OFFSET + i * DIAMOND_WIDTH-1, Y_DIAMOND_OFFSET + j * DIAMOND_WIDTH-1, 
		X_DIAMOND_OFFSET + (i+1) * DIAMOND_WIDTH+1, Y_DIAMOND_OFFSET + (j+1) * DIAMOND_WIDTH+1); 
	InvalidateRect(hwndApp, &rcFocus, 0);
}
/*
start a new scence animately
*/
static void DiamondDrop(HDC hdc)
{
	//animate step, we want the diamond Diamonds step drop 
	//from orignal position to new position.
	int		i,y,STEP;
	int		k, h, start, sum;
	int		nDiamShowHeight;
		
	start = 7;
	sum = 8;
	STEP = sum * 3;
	for(i=0;i<8;i++)
	{
		for(h=1;h<=STEP;h++)
		{
			for(k=0;k<=start;k++)
			{
				y = (start - k -sum)* DIAMOND_WIDTH + DIAMOND_WIDTH*h/3; 
//				BitBlt(hdc,  i * DIAMOND_WIDTH , y,	DIAMOND_WIDTH , DIAMOND_WIDTH , 
//				   (HDC)DiamondBmp[Diamond[i][start - k ]], 0, 0, ROP_SRC);
				if (-DIAMOND_WIDTH < y)
				{
					if (0 < y)
					{
						nDiamShowHeight = DIAMOND_WIDTH;
					}
					else
					{
						nDiamShowHeight = DIAMOND_WIDTH + y;
						y = 0;
					}
					BitBlt(hdc, X_DIAMOND_OFFSET + i * DIAMOND_WIDTH , Y_DIAMOND_OFFSET + y,	DIAMOND_WIDTH , nDiamShowHeight , 
					   (HDC)DiamondBmp, GET_X_DIAMONDS(Diamond[i][start - k ]), (DIAMOND_WIDTH - nDiamShowHeight), ROP_SRC);
				}
				else
				{
					nDiamShowHeight = 0;
				}
			}
#ifndef EMULATION 
//			KickDog();
#endif
			//wait a monment. the value should adjust according to machine's CPU speed 
		//	OS_UpdateScreen(0,0,0,0);
			//Delaying(1);

		}
	}
}
/*
Draw number's special font.
right aligned 
*/
static void DrawNumber(HDC hdc, int x, int y, int Number)
{
	int i,temp ;
	int num = Number;
	i=0;
	while(num/10)
	{
		temp = num % 10;
		i++;
		BitBlt(hdc,  (x- (i) * NUMBER_WIDTH) , y,	NUMBER_WIDTH , NUMBER_HEIGHT , 
				   (HDC)hNumBmp, temp*NUMBER_WIDTH, 0, ROP_SRC);
		num = num/10;
	}
	BitBlt(hdc,  (x - (i+1) * NUMBER_WIDTH) , y, NUMBER_WIDTH , NUMBER_HEIGHT , 
				   (HDC)hNumBmp, num*NUMBER_WIDTH, 0, ROP_SRC);

	return;
}
/*
paint back ground whth red brush steply.
*/
static void RefreshScence(HDC hdc)
{
	int i, j;
	RECT rc;
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			SetRect(&rc, X_DIAMOND_OFFSET + i * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + j * DIAMOND_WIDTH, 
				X_DIAMOND_OFFSET + (i+1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (j+1) * DIAMOND_WIDTH); 

			ClearRect(hdc,&rc,RGB( 255, 55, 55));
			//OS_UpdateScreen(0,0,0,0);
			Delaying(10);
		}
	}
	OS_UpdateScreen(0,0,0,0);

//show level in center of screen
	{
		char str[32];
		int oldBkMode;
		int oldTxtCor;

		oldBkMode = SetBkMode(hdc,TRANSPARENT);
		oldTxtCor = SetTextColor(hdc,0x00ffffff);
		sprintf(str, "level %d",	GameLevel);
		TextOut(hdc, X_TEXTLEVEL , Y_TEXTLEVEL , str, -1);
		SetTextColor(hdc,oldTxtCor);
		SetBkMode(hdc,oldBkMode);
		
		OS_UpdateScreen(0,0,0,0);
		Delaying(300);
	}

	return;
}

/*
draw diamond to screen
*/

static void DrawDiamond(HDC hdc)
{
	int i, j;
	RECT rcFocus;
	HPEN hPen;
	HPEN hOldPen;

	//DrawNumber(hdc, 50, 130, SPEED);


	//draw tip button
	//BitBlt(hdc,  54 , 130, 12 , 14 , 
	//			   (HDC)hNumBmp, 110, 0, ROP_SRC);
	DrawNumber(hdc, X_GAMELEVEL, Y_GAMELEVEL, GameLevel);
	DrawNumber(hdc, X_TOTALMERGE, Y_TOTALMERGE, TotalMergeDiamondNum*10);

	{
		//draw process bar
/*		hPen = CreatePen(PS_SOLID, 1, RGB( 5, 150, 150 ));
		hOldPen = SelectObject(hdc, hPen);
		RoundRect(hdc, rcBar.left-3, rcBar.top, rcBar.right+3, rcBar.bottom+1, 1, 1);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);*/
		
		hPen = CreatePen(PS_SOLID, 1, RGB( 160, 255, 160 ));
		hOldPen = SelectObject(hdc, hPen);
		DrawLine( hdc, BAR_LEFT-1, BAR_TOP+2, BAR_LEFT+LevelMergeDiamondNum*BAR_WIDTH/MAXPRICE, BAR_TOP+2);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
		
		hPen = CreatePen(PS_SOLID, 1, RGB( 100, 255, 100 ));
		hOldPen = SelectObject(hdc, hPen);
		DrawLine( hdc, BAR_LEFT, BAR_TOP+1, BAR_LEFT+LevelMergeDiamondNum*BAR_WIDTH/MAXPRICE, BAR_TOP+1);
		DrawLine( hdc, BAR_LEFT-1, BAR_TOP+3, BAR_LEFT+LevelMergeDiamondNum*BAR_WIDTH/MAXPRICE, BAR_TOP+3);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);

		hPen = CreatePen(PS_SOLID, 1, RGB( 50, 200, 50 ));
		hOldPen = SelectObject(hdc, hPen);
		DrawLine( hdc, BAR_LEFT-2, BAR_TOP+4, BAR_LEFT+LevelMergeDiamondNum*BAR_WIDTH/MAXPRICE, BAR_TOP+4);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
        
		hPen = CreatePen(PS_SOLID, 1, RGB( 50, 128, 50 ));
		hOldPen = SelectObject(hdc, hPen);
		DrawLine( hdc, BAR_LEFT-1, BAR_TOP+5, BAR_LEFT+LevelMergeDiamondNum*BAR_WIDTH/MAXPRICE, BAR_TOP+5);
		DrawLine( hdc, BAR_LEFT-1, BAR_TOP+6, BAR_LEFT+LevelMergeDiamondNum*BAR_WIDTH/MAXPRICE, BAR_TOP+6);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);
		//LHBHIDE  04-02-27
		/*
		hPen = CreatePen(PS_SOLID, 1, RGB( 20, 96, 20 ));
		hOldPen = SelectObject(hdc, hPen);
		DrawLine( hdc, BAR_LEFT, BAR_TOP+7, BAR_LEFT+LevelMergeDiamondNum*BAR_WIDTH/MAXPRICE, BAR_TOP+7);
		SelectObject(hdc, hOldPen);
		DeleteObject(hPen);*/
	}

	if(!bBegin && !bLeave)
	{
//		RefreshScence(hdc);
		DiamondDrop(hdc);
	}
	{
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j++)
			{
//				BitBlt(hdc,  i * DIAMOND_WIDTH , j * DIAMOND_WIDTH,
//					DIAMOND_WIDTH , DIAMOND_WIDTH , 
//				   (HDC)DiamondBmp[Diamond[i][j]], 0, 0, ROP_SRC);
				BitBlt(hdc, X_DIAMOND_OFFSET + i * DIAMOND_WIDTH , Y_DIAMOND_OFFSET + j * DIAMOND_WIDTH,
					DIAMOND_WIDTH , DIAMOND_WIDTH , 
				   (HDC)DiamondBmp, GET_X_DIAMONDS(Diamond[i][j]), 0, ROP_SRC);
			}
#ifndef EMULATION 
//			KickDog();
#endif

		}
	}

	//high light the focus Diamond
	SetRect(&rcFocus, X_DIAMOND_OFFSET + iFocus * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + jFocus * DIAMOND_WIDTH, 
				X_DIAMOND_OFFSET + (iFocus+1) * DIAMOND_WIDTH-1, Y_DIAMOND_OFFSET + (jFocus+1) * DIAMOND_WIDTH-1); 
	DiamondDraw3DBorder(hdc, &rcFocus, 0);
/*
	if(bTip)
	{
		SetRect(&rcFocus, iTip * DIAMOND_WIDTH, jTip * DIAMOND_WIDTH, 
					(iTip+1) * DIAMOND_WIDTH, (jTip+1) * DIAMOND_WIDTH); 
		DrawColorBorder(hdc, &rcFocus, RGB(255, 255, 255));
	}
*/
	return;
}


/*
merge a vertical identical diamond line 
i: vertical line number
end: end row number of identical diamond value   
sum : number of identical  diamonds  
*/
static void Merge(int i, int end, int sum)
{
	int		y,STEP;
	int		k, h;
	int		nDiamShowHeight;
	RECT	rc;
	HDC		hdc;


	for(k=0;k<=end;k++)
	{
		if(end - sum - k < 0 )
			//
			Diamond[i][end - k] = NewDiamond();
		else
			Diamond[i][end - k] = Diamond[i][end- sum - k];
	}
	//if a new game start , the animation is no use.
	if(!bBegin)
		return;
	
	//animate step, we want the diamond Diamonds step drop 
	//from original position to new position.

	hdc = GetDC(hwndApp);
	STEP = sum * 3;
	for(h=1;h<=STEP;h++)
	{
		for(k=0;k<=end;k++)
		{
			y = (end - k -sum)* DIAMOND_WIDTH + DIAMOND_WIDTH*h/3;
			
			if (-DIAMOND_WIDTH < y)
			{
				if (0 < y)
				{
					nDiamShowHeight = DIAMOND_WIDTH;
				}
				else
				{
					nDiamShowHeight = DIAMOND_WIDTH + y;
					y = 0;
				}
				BitBlt(hdc, X_DIAMOND_OFFSET + i * DIAMOND_WIDTH , Y_DIAMOND_OFFSET+y,	DIAMOND_WIDTH , nDiamShowHeight , 
				   (HDC)DiamondBmp, GET_X_DIAMONDS(Diamond[i][end - k ]), (DIAMOND_WIDTH - nDiamShowHeight), ROP_SRC);
			}
			else
			{
				nDiamShowHeight = 0;
			}
		//	OS_UpdateScreen(0,0,0,0);
		}
		//wait a monment. the value should adjust according to machine's CPU speed 
		OS_UpdateScreen(0,0,0,0);
		Delaying(10);

	}

	//flush rectangle area which need redraw
	SetRect(&rc, X_DIAMOND_OFFSET + i * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + 0, 
		X_DIAMOND_OFFSET + (i+1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (end+1) * DIAMOND_WIDTH); 
	InvalidateRect(hwndApp, &rc, 0);
	
	SetRect(&rc, X_DIAMOND_OFFSET + 0, Y_DIAMOND_OFFSET + 8*DIAMOND_WIDTH, 
		X_DIAMOND_OFFSET + 8 * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + 10 * DIAMOND_WIDTH); 
	InvalidateRect(hwndApp, &rc, 0);
	//
	OS_UpdateScreen(0,0,0,0);
	Delaying(20);

	ReleaseDC( hwndApp, hdc );
	return;
}
//check all diamonds , wether there are over 3 same diamonds live a line  
//

static void CheckBoard()
{
	int i, j, k;
	int sum;
	int start, seed ;

	int bMerging ;
	
	bMerging = 0;
	for(i=0;i<8;i++)
	{
		seed = Diamond[i][7];
		sum = 0;
		start = 7;
		for(j=6;j>=0;j--)
		{
			if(seed != Diamond[i][j])
			{
				if(sum>1)
				{
					Merge(i, start, sum+1);
					nMergeDiamondNum+=Bonus(sum+1);
					bMerging = 1;
					i--;
					break;
				}
				sum = 0;
				start = j;
				seed = Diamond[i][j];
			}
			else
			{
				sum ++;
			}
			
		}
		if(sum>1 && j==-1)
		{
			Merge(i, start, sum+1);
			nMergeDiamondNum+=Bonus(sum+1);
			i--;
		}

	}
#ifndef EMULATION 
//	KickDog();
#endif

	for(j=7; j>=0; j--)
	{
		seed = Diamond[0][j];
		sum = 0;
		start = 0;
		for(i=1; i<8; i++)
		{
			if(seed != Diamond[i][j])
			{
				if(sum>1)
				{
					for(k=0;k<(sum+1);k++)
						Merge(k + start, j, 1);
					nMergeDiamondNum+=Bonus(sum+1);
					bMerging = 1;
					j--;
					break;
				}
				sum = 0;
				start = i;
				seed = Diamond[i][j];
			}
			else
			{
				sum ++;
			}
			
		}
		if(sum>1 && i==8)
		{
			for(k=0;k<sum+1;k++)
				Merge(k + start, j, 1);
			nMergeDiamondNum+=Bonus(sum+1);
			bMerging = 1;
			j--;
		}

	}
	if(bMerging)
		CheckBoard();
#ifndef EMULATION 
//	KickDog();
#endif

	return;
}

/********************************************************************************/
//check is there a way, if any , give the tip
//-----------------------------------------

//     *
//  ---@*
//     *
static int CheckNeighborLeft(int i,int j, int seed)
{
	if(i<0 || i>7 || j<0 || j>7  )
		return 0;
	if(j>0 && Diamond[i][j-1]==seed)
		return 1;
	if(j<7 && Diamond[i][j+1]==seed)
		return 1;
	if(i>0 && Diamond[i-1][j]==seed)
		return 1;
	return 2;
}
//     *
//    *@----
//     *
static int CheckNeighborRight(int i,int j, int seed)
{
	if(i<0 || i>7 || j<0 || j>7  )
		return 0;
	if(j>0 && Diamond[i][j-1]==seed)
		return 1;
	if(j<7 && Diamond[i][j+1]==seed)
		return 1;
	if(i<7 && Diamond[i+1][j]==seed)
		return 1;
	return 2;
}
//     *
//    *@*
//     |
//     |

static int CheckNeighborUp(int i,int j, int seed)
{
	if(i<0 || i>7 || j<0 || j>7  )
		return 0;
	if(j>0 && Diamond[i][j-1]==seed)
		return 1;
	if(i>0 && Diamond[i-1][j]==seed)
		return 1;
	if(i<7 && Diamond[i+1][j]==seed)
		return 1;
	return 2;
}

//     |
//     |
//    *@*
//     *

static int CheckNeighborDown(int i,int j, int seed)
{
	if(i<0 || i>7 || j<0 || j>7  )
		return 0;
	if(j<7 && Diamond[i][j+1]==seed)
		return 1;
	if(i>0 && Diamond[i-1][j]==seed)
		return 1;
	if(i<7 && Diamond[i+1][j]==seed)
		return 1;
	return 2;
}
//     |
//     *
//  --#@#--
//     *
//     |

static int CheckNeighborUpDown(int i,int j, int seed)
{
	if(i<0 || i>7 || j<0 || j>7  )
		return 0;
	if(j>0 && Diamond[i][j-1]==seed)
		return 1;
	if(j<7 && Diamond[i][j+1]==seed)
		return 1;
	return 2;
}
//     |
//     |
//  --*@*--
//     |
//     |
static int CheckNeighborLeftRight(int i,int j, int seed)
{
	if(i<0 || i>7 || j<0 || j>7  )
		return 0;
	if(i>0 && Diamond[i-1][j]==seed)
		return 1;
	if(i<7 && Diamond[i+1][j]==seed)
		return 1;
	return 2;
}

//check there is a way

static int CheckWay()
{
	int i, j;
	int sum, ret;
	int start, end, seed ;
	//03
	for(j=0;j<8;j++)
	{
		seed = Diamond[0][j];
		sum  = 1;
		start = 0;
		for(i=1;i<8;i++)
		{
			if(seed == Diamond[i][j])
			{//if same , sum
				sum ++;
				if(i==7)
				{//same last one
					ret = CheckNeighborLeft(start-1, j,  seed);
					if(ret==1)
					{
						iTip = start -1;
						jTip = j;
						return 0;
					}
				}
			}
			else
			{
				if(sum>1)
				{//over one same Diamond, check the Diamond close to start and end  
					end = i;
					ret = CheckNeighborLeft(start-1, j, seed);
					if(ret==1)
					{
						iTip = start-1;
						jTip = j;
						return 0;
					}
					ret = CheckNeighborRight(end, j, seed);
					if(ret==1)
					{
						iTip = end;
						jTip = j;
						return 0;
					}
				}
				seed = Diamond[i][j];
				sum  = 1;
				start = i;
			}
		}				
	}
#ifndef EMULATION 
//	KickDog();
#endif
	//00
	for(i=0;i<8;i++)
	{
		seed = Diamond[i][0];
		for(j=1;j<7;j++)
		{
			if(seed != Diamond[i][j])
			{
				if(seed == Diamond[i][j+1])
				{
					ret = CheckNeighborLeftRight(i,j, seed);
					if(ret==1)
					{
						iTip = i;
						jTip = j;
						return 0;
					}
				}
				seed = Diamond[i][j];
			}
		}				
	}
#ifndef EMULATION 
//	KickDog();
#endif

	//01
	for(j=0;j<8;j++)
	{
		seed = Diamond[0][j];
		for(i=1;i<7;i++)
		{
			if(seed != Diamond[i][j])
			{
				if(seed == Diamond[i+1][j])
				{
					ret = CheckNeighborUpDown(i,j, seed);
					if(ret==1)
					{
						iTip = i;
						jTip = j;
						return 0;
					}
				}
				seed = Diamond[i][j];
			}				
		}
	}
#ifndef EMULATION 
//	KickDog();
#endif
	//02

	
	for(i=0;i<8;i++)
	{
		seed = Diamond[i][0];
		sum  = 1;
		start = 0;
		for(j=1;j<8;j++)
		{
			if(seed == Diamond[i][j])
			{//if same , sum
				sum ++;
				if(j==7)
				{//same last one//02
					ret = CheckNeighborUp(i,start-1, seed);
					if(ret==1)
					{
						iTip = i;
						jTip = start-1;
						return 0;
					}
				}
			}
			else
			{
				if(sum>1)
				{//over one same Diamond, check the Diamond close to start and end  
					end = j;//02
					ret = CheckNeighborUp(i,start-1, seed);
					if(ret==1)
					{
						iTip = i;
						jTip = start-1;
						return 0;
					}
					ret = CheckNeighborDown(i,end, seed);
					if(ret==1)
					{
						iTip = i;
						jTip = end;
						return 0;
					}
				}
				seed = Diamond[i][j];
				sum  = 1;
				start = j;
			}
		}				
	}

#ifndef EMULATION 
//	KickDog();
#endif

	return 1;
}
/*
give new value to all the Diamonds
  */
static void InitDiamond()
{
	int i, j;
	
	srand(GetTickCount());
	for(i=0;i<8;i++)
	{
		for(j=0;j<8;j++)
		{
			Diamond[i][j] = NewDiamond();
		}
	}
#ifndef EMULATION 
//	KickDog();
#endif

	CheckBoard();

	return;
}

/*********************************************************************\
* Function	   SwapDiamond
* Purpose      交换钻石
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void SwapDiamond()
{
	int temp;
	HDC hdc = GetDC(hwndApp);

	temp = Diamond[bi][bj] ;
	Diamond[bi][bj] = Diamond[ai][aj] ;
	Diamond[ai][aj] = temp;
	BitBlt(hdc, X_DIAMOND_OFFSET + ai * DIAMOND_WIDTH , Y_DIAMOND_OFFSET + aj * DIAMOND_WIDTH,
					DIAMOND_WIDTH , DIAMOND_WIDTH , 
				   (HDC)DiamondBmp, GET_X_DIAMONDS(Diamond[ai][aj]), 0, ROP_SRC);
	BitBlt(hdc,  X_DIAMOND_OFFSET + bi * DIAMOND_WIDTH , Y_DIAMOND_OFFSET + bj * DIAMOND_WIDTH,
					DIAMOND_WIDTH , DIAMOND_WIDTH , 
				   (HDC)DiamondBmp, GET_X_DIAMONDS(Diamond[bi][bj]), 0, ROP_SRC);
	OS_UpdateScreen(0,0,0,0);

	ReleaseDC( hwndApp, hdc );
	return;
}

/*
swap select Diamond and check there is a a line 
*/

static void SwapAndCalc()
{
	nMergeDiamondNum = 0;
	SwapDiamond();
	CheckBoard();
	if(nMergeDiamondNum==0)
	{
		//cancle this swap
		SwapDiamond();
		RedrawDiamond(ai, aj);
		RedrawDiamond(bi, bj);
	}
	else
	{
		//add this operation value to total price 
		LevelMergeDiamondNum += nMergeDiamondNum;
		TotalMergeDiamondNum += nMergeDiamondNum;
		iTimerCouter = WAITTIMES;
		bBallFlash = FALSE;
	}
	
	return;
}
/*********************************************************************\
* Function	   DiamondGameOver
* Purpose      游戏结束
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static void DiamondGameOver(HDC hdc)
{
	HBITMAP hBmp;
	hBmp=LoadImage(NULL,"ROM:diagameover.bmp", IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
	BitBlt(hdc,  0 , 0,	120, 160 , 
				 (HDC)hBmp, 0, 0, ROP_SRC);
	DeleteObject(hBmp);

	return;
}

/*
Diamond select process
*/
static void SelectOneDiamond(int ni, int nj)
{
	if(ni<0 || ni>7 || nj<0 || nj>7)
		return;

	nHit ++;
	if(nHit==1)
	{//first select,just high light the Diamond
		ai = ni;
		aj = nj;
		RedrawDiamond(iFocus , jFocus );//recover last select Diamond

		//high light the Diamond
		RedrawDiamond(ai, aj);
	}
	iFocus = ni;
	jFocus = nj;

	if(nHit==2)
	{
		bi = ni;
		bj = nj;
		nHit=0;
		if(   (bj==aj+1 && bi==ai) ||(bj==aj-1 && bi==ai)
			||(bi==ai+1 && bj==aj) ||(bi==ai-1 && bj==aj) )
		{//if the selected Diamonds adjacent
			SwapAndCalc();
		}
		else
		{//just recover
			RedrawDiamond(ai, aj);
			RedrawDiamond(bi, bj);
		}
		
		//check whether this level  is over?
		if(CheckWay() || LevelMergeDiamondNum  >= MAXPRICE)
		{
			if(GameLevel<99)
			{
				GameLevel ++;
				bLeave = FALSE;
				InitDiamond();
				while(CheckWay())
					InitDiamond();
				LevelMergeDiamondNum = MAXPRICE/2;
				bBegin = 0; 

				InvalidateRect(hwndApp, NULL, 0);
				OS_UpdateScreen(0,0,0,0);
			}
			else
			{//MAX 99 level 
//				bGameOver = 1;
				PLXConfirmWinEx(NULL, hwndApp,IDS_AGAIN,Notify_Request, IDS_PROMPT, IDS_CONFIRM, IDS_CANCEL, WM_MSGWINOK);
/*				bGameOver = TRUE;
				GameLevel=1; 
				bBegin = 0;
				bGamePause = FALSE;
				nMergeDiamondNum = 0;
				TotalMergeDiamondNum = 0;
				LevelMergeDiamondNum = MAXPRICE/2;
				bGameOver = 0;
				iFocus = jFocus = 0;
				ai = aj = bi = bj = 0;

				bLeave = FALSE;
				InitDiamond();
				while(CheckWay())
					InitDiamond();
				InvalidateRect(hWnd,NULL,TRUE);
				OS_UpdateScreen(0,0,0,0);*/
			}
		}
	}

	return;
}

/*
save game status data into file
*/
static void DiamondSaveData()
{
	int hFile;

	//----------------------------------------------------------	判断FLASH空间
	if( 0 == IsFlashAvailable( 200 ))
	{																//不足200K
		PLXTipsWin(NULL, NULL, 0, ML("No memory"), IDS_DIAMOND, Notify_Alert, IDS_CONFIRM, IDS_CANCEL, WAITTIMEOUT );
		return;
	}

	hFile = open(DIAMOND_CFG_FILE, O_WRONLY|O_TRUNC);
	if (hFile == -1)
	{
		hFile = creat(DIAMOND_CFG_FILE, 666);
	}
	//Total Bonus
	write(hFile, &TotalMergeDiamondNum, sizeof(int));
	//this level bonus
	write(hFile, &LevelMergeDiamondNum, sizeof(int));
	//level
	write(hFile, &GameLevel, sizeof(DWORD));
	//Diamond data
	write(hFile, Diamond, 64*sizeof(int));
	//high scores should save later
	//
	close(hFile);

	return;
}
/*
read game status data from file
*/
static int DiamondLoadData()
{
	int hFile;
//	hFile = CreateFile(DIAMOND_CFG_FILE, ACCESS_READ | SHARE_READ, 0);
	hFile = open(DIAMOND_CFG_FILE, O_RDONLY);
	if (hFile == -1)
	{
		GameLevel=1; 
		bBegin = 0;
		nMergeDiamondNum = 0;
		TotalMergeDiamondNum = 0;
		LevelMergeDiamondNum = MAXPRICE/2;
		bGameOver = 0;
		iFocus = jFocus = 0;
		return 0;
	}
	//Total Bonus
	read(hFile, &TotalMergeDiamondNum, sizeof(TotalMergeDiamondNum));
	//this level bonus
	read(hFile, &LevelMergeDiamondNum, sizeof(LevelMergeDiamondNum));
	//level
	read(hFile, &GameLevel, sizeof(GameLevel));
	//Diamond data
	read(hFile, Diamond, sizeof(Diamond));
	//high scores should save later
	//
	close(hFile);
	return 1;
}

//************************************************************************
//          Internal functions
//************************************************************************
static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{

	HDC hdc;
	
	WORD wID;
	int px,py, ni, nj;
	int ret;
	
	static RECT rcFocus;
    
	
	LRESULT lResult;

	HDC hdcMem;
//	PAINTSTRUCT	ps;
	RECT rcClient;

	lResult = (LRESULT)TRUE;

	switch (wMsgCmd)
    {
	case WM_ACTIVATE:
		if (WA_ACTIVE == wParam)
		{
			bGamePause = FALSE;
			SetFocus(hWnd);
		}
		else if (WA_INACTIVE == wParam)
		{
			bGamePause = TRUE;
			bLeave = TRUE;
		}
		break;

    case WM_CREATE :
		bLeave = FALSE;
		SetTimer(hWnd,GAME_TIMER_ID,GAME_TIMER_ELAPSE,NULL);
		iTimerCouter = WAITTIMES;
		bBallFlash = FALSE;

		SetRect(&rcBar, BAR_LEFT, BAR_TOP, BAR_LEFT+BAR_WIDTH, BAR_TOP + BAR_HEIGHT);  

   	    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
            (WPARAM)ID_FILEMGR_QUIT, (LPARAM)ML("Back"));
        
        //左键文字
        SendMessage(hWnd , PWM_CREATECAPTIONBUTTON, 
            MAKEWPARAM(ID_FILEMGR_OK,1), (LPARAM)IDS_PROMPT);

		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

		InitDiamondBMP();
		if (bLoadData)
		{
			ret = DiamondLoadData();
			if(ret==1)
			{
				CheckWay();
				break;
			}
		}
		GameLevel=1; 
		bBegin = 0;
		bGamePause = FALSE;
		nMergeDiamondNum = 0;
		TotalMergeDiamondNum = 0;
		LevelMergeDiamondNum = MAXPRICE/2;
		bGameOver = 0;
		iFocus = jFocus = 0;
		ai = aj = bi = bj = 0;

		InitDiamond();
		while(CheckWay())
			InitDiamond();
		break;

    case WM_DESTROY :

		KillTimer(hWnd,GAME_TIMER_ID);
		if (bSaveData)
		{
			DiamondSaveData();
		}
		
		DeleteDiamondBMP();
        hwndApp = NULL;
		UnregisterClass("DiamondWndClass",NULL);
		bDiamondWndClass = FALSE;

        break;

	case WM_TIMER:
		if (bGamePause)
			break;
		if (!bBegin)
			break;
		if (bGameOver)
			break;
		if( --iTimerCouter )
		{
			InvalidateRect( hWnd, &INFORECT, FALSE );
			break;
		}
		bBallFlash = TRUE;

		iTimerCouter = WAITTIMES;
		LevelMergeDiamondNum --;
		if (0 >= LevelMergeDiamondNum)
		{
			//MsgWinEx(hwndApp,IDS_AGAIN,IDS_PROMPT,WAITTIMEOUT);
			PLXConfirmWinEx(NULL, hwndApp,IDS_AGAIN,Notify_Request, IDS_PROMPT, IDS_CONFIRM, IDS_CANCEL, WM_MSGWINOK);
/*			bGameOver = TRUE;
			GameLevel=1; 
			bBegin = 0;
			bGamePause = FALSE;
			nMergeDiamondNum = 0;
			TotalMergeDiamondNum = 0;
			LevelMergeDiamondNum = MAXPRICE/2;
			bGameOver = 0;
			iFocus = jFocus = 0;
			ai = aj = bi = bj = 0;

			bLeave = FALSE;
			InitDiamond();
			while(CheckWay())
				InitDiamond();*/
		}
		InvalidateRect( hWnd, &INFORECT, FALSE );
	//	OS_UpdateScreen(0,0,0,0);
		break;

	case WM_MSGWINOK:
		if (lParam ==1) {
		
		bGameOver = TRUE;
		GameLevel=1; 
		bBegin = 0;
		bGamePause = FALSE;
		nMergeDiamondNum = 0;
		TotalMergeDiamondNum = 0;
		LevelMergeDiamondNum = MAXPRICE/2;
		bGameOver = 0;
		iFocus = jFocus = 0;
		ai = aj = bi = bj = 0;

		bLeave = FALSE;
		InitDiamond();
		while(CheckWay())
			InitDiamond();
		InvalidateRect(hWnd,NULL,TRUE);
		}
		
		break;

	case WM_KEYDOWN:
		//bTip = 0;
		switch(LOWORD(wParam))
		{
		case VK_UP:
			if(!bBegin)
			{
				bBegin = 1; 
			}
		
			if(bGameOver)
				break;

			if(jFocus==0)
				break;

			jFocus--;
			SetRect(&rcFocus, X_DIAMOND_OFFSET + iFocus * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + jFocus * DIAMOND_WIDTH, 
				X_DIAMOND_OFFSET + (iFocus+1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jFocus+2) * DIAMOND_WIDTH); 
			InvalidateRect(hwndApp, &rcFocus, 0);
            break;
		case VK_DOWN:
			if(!bBegin)
			{
				bBegin = 1; 
			}
			if(bGameOver)
				break;

			if(jFocus==7)
				break;

			jFocus++;
			SetRect(&rcFocus, X_DIAMOND_OFFSET + iFocus * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jFocus-1) * DIAMOND_WIDTH, 
				X_DIAMOND_OFFSET + (iFocus+1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jFocus+1) * DIAMOND_WIDTH); 
			InvalidateRect(hwndApp, &rcFocus, 0);
            break;
		case VK_LEFT:
			if(!bBegin)
			{
				bBegin = 1; 
			}
			if(bGameOver)
				break;

			if(iFocus==0)
				break;

			iFocus--;
			SetRect(&rcFocus, X_DIAMOND_OFFSET + iFocus * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + jFocus * DIAMOND_WIDTH, 
				X_DIAMOND_OFFSET + (iFocus+2) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jFocus+1) * DIAMOND_WIDTH); 
			InvalidateRect(hwndApp, &rcFocus, 0);
            break;
		case VK_RIGHT:
			if(!bBegin)
			{
				bBegin = 1; 
			}
			if(bGameOver)
				break;

			if(iFocus==7)
				break;

			iFocus++;
			SetRect(&rcFocus, X_DIAMOND_OFFSET + (iFocus-1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jFocus) * DIAMOND_WIDTH, 
				X_DIAMOND_OFFSET + (iFocus+1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jFocus+1) * DIAMOND_WIDTH); 
			InvalidateRect(hwndApp, &rcFocus, 0);
            break;
		
		case VK_F5 :
			if(!bBegin)
			{
				bBegin = 1; 
			}
			if(bGameOver)
				break;

			SelectOneDiamond(iFocus, jFocus);
			break;

		default:
			lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
			break;
		}
		break;
    case WM_PENDOWN :
		//bTip = 0;
		if(bGameOver)
			break;
		if(!bBegin)
		{
			bBegin = 1; 
		}
		px  = LOWORD(lParam);  
		py  = HIWORD(lParam);  
		ni	= (px - X_DIAMOND_OFFSET)/DIAMOND_WIDTH;
		nj	= (py - Y_DIAMOND_OFFSET)/DIAMOND_WIDTH;
		if (0 <= ni && 8 > ni && 0 <= nj && 8 > nj)
		{

			SelectOneDiamond(ni, nj);
			if (0 == nHit)
			{
				SelectOneDiamond(ni, nj);
			}
		}
		//for test speed
/*		if(px>30 && px < 40 && py >130 && py < 144 )
		{
			RECT rc;
			SPEED--;
			if(SPEED<1)
				SPEED = 1;
			SetRect(&rc, 0, 8*DIAMOND_WIDTH, 8 * DIAMOND_WIDTH, 10 * DIAMOND_WIDTH); 
			InvalidateRect(hwndApp, &rc, 0);
			OS_UpdateScreen(0,0,0,0);

			break;
		}
		if(px>40 && px < 50 && py >130 && py < 144 )
		{
			RECT rc;
			SPEED++;
			if(SPEED>99)
				SPEED = 99;
			SetRect(&rc, 0, 8*DIAMOND_WIDTH, 8 * DIAMOND_WIDTH, 10 * DIAMOND_WIDTH); 
			InvalidateRect(hwndApp, &rc, 0);
			OS_UpdateScreen(0,0,0,0);
			break;
		}*/
		//this code show put in menu action
		//just for test
		//give tip , and move focus to tip Diamond position
		if(px>TIP_LEFT && px < TIP_RIGHT && py >TIP_UP && py < TIP_DOWN )
		{
			RECT rc;
			//bTip = 1;

			SetRect(&rc, X_DIAMOND_OFFSET + iFocus * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + jFocus * DIAMOND_WIDTH, 
				X_DIAMOND_OFFSET + (iFocus+1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jFocus+1) * DIAMOND_WIDTH);
			InvalidateRect(hwndApp, &rc, 0);

			iFocus = iTip ;
			jFocus = jTip ;

			SetRect(&rc, X_DIAMOND_OFFSET + iTip * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + jTip * DIAMOND_WIDTH, 
				X_DIAMOND_OFFSET + (iTip+1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jTip+1) * DIAMOND_WIDTH);
			InvalidateRect(hwndApp, &rc, 0);

			break;
		}

        break;
        
	case WM_COMMAND:
		
		wID = LOWORD(wParam);//wParam低字节中包含控制的标识符，高字节中包含本消息的代码
		switch(wID)
		{
		case ID_FILEMGR_OK:
			if(!bBegin)
			{
				bBegin = 1; 
			}
			if(bGameOver)
				break;
			{
				RECT rc;
				//bTip = 1;
				SetRect(&rc, X_DIAMOND_OFFSET + iFocus * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + jFocus * DIAMOND_WIDTH, 
					X_DIAMOND_OFFSET + (iFocus+1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jFocus+1) * DIAMOND_WIDTH);
				InvalidateRect(hwndApp, &rc, 0);

				iFocus = iTip ;
				jFocus = jTip ;

				SetRect(&rc, X_DIAMOND_OFFSET + iTip * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + jTip * DIAMOND_WIDTH, 
					X_DIAMOND_OFFSET + (iTip+1) * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + (jTip+1) * DIAMOND_WIDTH);
				InvalidateRect(hwndApp, &rc, 0);

				break;
			}
			break;

        case ID_FILEMGR_QUIT:
			//MsgSelWin(hWnd,IDS_SAVERECORD,IDS_DIAMOND);
			PLXConfirmWinEx(NULL, hWnd, IDS_SAVERECORD, Notify_Request, IDS_DIAMOND, IDS_CONFIRM, IDS_CANCEL, WM_SELOK);
            break;

		default:
			lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
			break;
		}//end WM_COMMAND
		break;

	case WM_SELCANCEL:
		bSaveData = FALSE;
		DestroyWindow(hWnd);
		break;

	case WM_SELOK:
		if(lParam == 1)
			bSaveData = TRUE;
		else
			bSaveData = FALSE;
		DestroyWindow(hWnd);
		break;

	case WM_PAINT :

		{
#if 1



	GetClientRect(hWnd, &rcClient);
	hdcMem = CreateMemoryDC(rcClient.right, rcClient.bottom);
	SetClipRect(hdcMem, NULL);

	BitBlt(hdcMem, X_GAMEBACK,Y_GAMEBACK,
				GAMEBACK_WIDTH , GAMEBACK_HEIGHT , 
			   (HDC)hGameBack, 0, 0, ROP_SRC);

	if(( !bBallFlash )||( iTimerCouter % 2 ))
	{
		int OldStyle;
		COLORREF OldClr;

		OldClr = SetBkColor(hdcMem,RGB(255,255,255));
		OldStyle = SetBkMode( hdcMem, NEWTRANSPARENT );

		BitBlt(hdcMem, X_LBALL, Y_LBALL,
					WIDTH_LBALL, HEIGHT_LBALL, 
				   (HDC)hLittleBall, 0, 0, ROP_SRC);

		SetBkMode( hdcMem, OldStyle );
		SetBkColor( hdcMem, OldClr );
	}

		if(bGameOver)
			DiamondGameOver(hdcMem);
		else
		{
			DrawDiamond(hdcMem);
			
			if(nHit==1)
			{
				SetRect(&rcFocus, X_DIAMOND_OFFSET + ai * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + aj * DIAMOND_WIDTH, 
					X_DIAMOND_OFFSET + (ai+1) * DIAMOND_WIDTH-1, Y_DIAMOND_OFFSET + (aj+1) * DIAMOND_WIDTH-1); 
				DiamondDraw3DBorder(hdcMem, &rcFocus, 1);
			}
		}
		/////////////////////////////////////////////////
	hdc=BeginPaint(hWnd,NULL);

	BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, ROP_SRC);

	DeleteDC(hdcMem);

	EndPaint(hWnd,NULL);
	DrawStaticBarArrowState(hWnd);

#else
		
		hdc = BeginPaint(hWnd, NULL);

		BitBlt(hdc, X_GAMEBACK,Y_GAMEBACK,
				GAMEBACK_WIDTH , GAMEBACK_HEIGHT , 
			   (HDC)hGameBack, 0, 0, ROP_SRC);

		if(bGameOver)
			DiamondGameOver(hdc);
		else
		{
			DrawDiamond(hdc);
			
			if(nHit==1)
			{
				SetRect(&rcFocus, X_DIAMOND_OFFSET + ai * DIAMOND_WIDTH, Y_DIAMOND_OFFSET + aj * DIAMOND_WIDTH, 
					X_DIAMOND_OFFSET + (ai+1) * DIAMOND_WIDTH-1, Y_DIAMOND_OFFSET + (aj+1) * DIAMOND_WIDTH-1); 
				DiamondDraw3DBorder(hdc, &rcFocus, 1);
			}
		}
		EndPaint(hWnd, NULL);

		DrawStaticBarArrowState(hWnd);
#endif

        break;
		}
	
	default :

        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );//为窗口函数不处理的消息提供缺省处理。
        break;
    }
    
    return lResult;
}

/**********************************************************/
//void DiamondDraw3DBorder(HDC hdc, RECT *rc, int In);
//画3维边框
//In :凹下还是凸起 1：凹下。 0： 凸起
/**********************************************************/
static void DiamondDraw3DBorder(HDC hdc, RECT *rc, int In)
{
	if(In)
	{
		//gray
		SelectObject(hdc, GetStockObject(GRAY_PEN));
		DrawLine( hdc, rc->left, rc->top, rc->left, rc->bottom);
		DrawLine( hdc, rc->left, rc->top, rc->right, rc->top);
		//black
		SelectObject(hdc, GetStockObject(DKGRAY_PEN));
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
		//light gray
		SelectObject(hdc, GetStockObject(LTGRAY_PEN));
		DrawLine( hdc, rc->left, rc->top, rc->left, rc->bottom);
		DrawLine( hdc, rc->left, rc->top, rc->right, rc->top);
		//white
		SelectObject(hdc, GetStockObject(WHITE_PEN));
		DrawLine( hdc, rc->left+1, rc->top+1, rc->left+1, rc->bottom-1);
		DrawLine( hdc, rc->left+1, rc->top+1, rc->right-1, rc->top+1);
		//gray
		SelectObject(hdc, GetStockObject(GRAY_PEN));
		DrawLine( hdc, rc->right-1, rc->top+1, rc->right-1, rc->bottom);
		DrawLine( hdc, rc->left+1, rc->bottom-1, rc->right-1, rc->bottom-1);

		//black
		SelectObject(hdc, GetStockObject(DKGRAY_PEN));
		DrawLine( hdc, rc->right, rc->top, rc->right, rc->bottom);
		DrawLine( hdc, rc->left, rc->bottom, rc->right+1, rc->bottom);
	}

	return;
}

/*********************************************************************\
* Function	   DiamondGameProc
* Purpose      调用游戏界面
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DiamondGameProc(HWND hWnd)
{
	WNDCLASS	wc;
	
	if (!bDiamondWndClass)
	{
		wc.style         = 0;
		wc.lpfnWndProc   = AppWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;//hInstance;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "DiamondWndClass";
		
		if (!RegisterClass(&wc))
			return FALSE;
		
		bDiamondWndClass = TRUE;
	}

    if (hwndApp)
    {
   		ShowWindow(hwndApp, SW_SHOW);
		MoveWindowToTop(hwndApp);
        bShow = TRUE;        

    }
    else
    {
        // Creates Game window
        hwndApp = CreateWindow(
            "DiamondWndClass", 
            "Diamond", 
            WS_VISIBLE|PWS_STATICBAR, 
            PLX_WIN_POSITION,
            hWnd, 
            NULL,
            NULL, 
            NULL
            );
        ShowWindow(hwndApp, SW_SHOW);
		UpdateWindow(hwndApp);
        bShow = TRUE;
    }

	return TRUE;
}

/*********************************************************************\
* Function	   CfgFileExist
* Purpose      判断配置文件是否存在
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	CfgFileExist()
{
	int		hFile;
	BOOL	bRtn;

	//hFile = CreateFile(DIAMOND_CFG_FILE, ACCESS_READ, 0);
	hFile = open(DIAMOND_CFG_FILE, O_RDONLY);
	if (-1 == hFile)
	{
		bRtn = FALSE;
	}
	else
	{
		bRtn = TRUE;
		close(hFile);
	}

	return bRtn;
}

/*********************************************************************\
* Function	   DrawStaticBarArrowState
* Purpose      画状态条的箭头
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL	DrawStaticBarArrowState(HWND hWnd)
{
	if (bGameOver)
	{
		return FALSE;
	}

	PostMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLUP|SCROLLDOWN|SCROLLLEFT|SCROLLRIGHT, MASKALL);
	
	if (0==iFocus)
		PostMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKLEFT);
	if (0==jFocus)
		PostMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKUP);
	if ((8-1)==iFocus)
		PostMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKRIGHT);
	if ((8-1)==jFocus)
		PostMessage(hWnd, PWM_SETSCROLLSTATE, NULL, MASKDOWN);

	PostMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLMIDDLE, MASKMIDDLE);

	return TRUE;
}

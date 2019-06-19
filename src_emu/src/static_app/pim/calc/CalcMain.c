 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Calculator
 *
 * Purpose  : main window
 *            
\**************************************************************************/

#include "window.h"
#include "string.h"
#include "winpda.h"
#include "malloc.h"
#include "plx_pdaex.h"

#include "CalcMain.h"

typedef double OPRAND;

#ifdef _EMULATE_
//	BOOL Beep(DWORD, DWORD);
#else
    BOOL PlayBack(WORD *wFreq, WORD *wDuration, int num);
#endif


static BOOL bShow = FALSE;			
static HINSTANCE hInstance = NULL;	
static HWND	Calc_hwndApp = NULL;
//static RECT rcBtns[BTN_COUNT];		
static char DispBuf[33];
static int nCurSign = -1;
static int nCurBtn = 5;
static BOOL bKeyUp = TRUE;
static BOOL bKeyLong = FALSE;
static char KeydownChar;
static int TimerId;

static HBITMAP *phbmp[BTN_COUNT], hSign_M, hSign_Add, hSign_Sub, hSign_Multiply, hSign_Divide;
extern OPRAND CM_mem;

/**********************************************************************
* Function	 CALC_AppControl
* Purpose    application main function
* Params	
* Return	
* Remarks
**********************************************************************/
DWORD Calc_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	WNDCLASS	    wc;
	DWORD		    dwRet;

    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:
        hInstance = (HINSTANCE)pInstance;
        break;

   case APP_GETOPTION:
		switch (wParam)
		{
		case AS_STATE:
			break;
		case AS_HELP:
			break;
		case AS_APPWND:
			dwRet = (DWORD)Calc_hwndApp;
			break;
		}
		break;

	case APP_SETOPTION:
		break;

    case APP_ACTIVE:

		if(IsWindow(Calc_hwndApp))
		{
			ShowWindow(Calc_hwndApp, SW_SHOW);
			UpdateWindow(Calc_hwndApp);
		}
		else
		{
			wc.style         = 0;
			wc.lpfnWndProc   = CalcMainWndProc;
			wc.cbClsExtra    = 0;
			wc.cbWndExtra    = 0;
			wc.hInstance     = NULL;
			wc.hIcon         = NULL;
			wc.hCursor       = NULL;
			wc.hbrBackground = GetStockObject(WHITE_BRUSH);
			wc.lpszMenuName  = NULL;
			wc.lpszClassName = "CalcWndClass";
			if (!RegisterClass(&wc))
			{
				UnregisterClass("CalcWndClass", NULL);
				return FALSE;
			}
			Calc_hwndApp = CreateWindow(
				"CalcWndClass",
				IDS_APPNAME,
				WS_VISIBLE | WS_CAPTION | PWS_STATICBAR,
				PLX_WIN_POSITION,
				NULL, NULL, NULL, NULL);

			if (!Calc_hwndApp)
				return (FALSE);

			SendMessage(Calc_hwndApp, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EQU, 1), (LPARAM)IDS_EQU);
			SendMessage(Calc_hwndApp, PWM_CREATECAPTIONBUTTON, (WPARAM)ID_EXIT, (LPARAM)IDS_EXIT);
			SendMessage(Calc_hwndApp, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

			ShowWindow(Calc_hwndApp, SW_SHOW);
			UpdateWindow(Calc_hwndApp);
		}
        break;

    case APP_INACTIVE:
		ShowWindow(Calc_hwndApp, SW_HIDE);
		
        break;

    default:
        break;
    }

    return dwRet;
}

/**********************************************************************
* Function	CalcMainWndProc
* Purpose   main window function
* Params
* Return	
* Remarks
**********************************************************************/
LRESULT CalcMainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT		lResult = TRUE;

	switch (wMsgCmd)
    {
    case WM_CREATE:
		Calc_InitValiable();
		Calc_OnCreate(hWnd);
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
        UnregisterClass("CalcWndClass", NULL);
		break;

    case WM_DESTROY:
		Calc_OnDestroy(hWnd);
		break;

//	case WM_COMMAND:
//		Calc_OnCommand(hWnd, wParam, lParam);
//		break;

	case WM_PAINT:
		Calc_OnPaint(hWnd);
		break;

//	case WM_PENDOWN:
//		Calc_OnPenDown(hWnd, LOWORD(lParam), HIWORD(lParam));
//		break;

//	case WM_PENUP:
//		Calc_OnPenUp(hWnd);
//		break;

	case WM_CHAR:
		KeydownChar = (char)wParam;
//		Calc_OnChar(hWnd, (char)wParam);
		break;

	case WM_KEYDOWN:
		bKeyUp = FALSE; 
		Calc_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);
		break;
	case WM_KEYUP:
		bKeyUp = TRUE;
		KillTimer(hWnd, TimerId);
		if(!bKeyLong)
		{
			Calc_OnChar(hWnd, KeydownChar);
		}
		KeydownChar = 0;
		bKeyLong =FALSE;
		
		break;
	case WM_TIMER:
//		KillTimer(hWnd, TimerId);
		if(!bKeyUp)//³¤°´
		{
			bKeyLong =TRUE;
			Calc_ConfirmInput(nCurBtn);
			InvalidateRect(hWnd, NULL, FALSE);
		}

		break;

	default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}

/**********************************************************************
* Function	  Calc_OnCreate
* Purpose     deal WM_CREATE message in main window
* Params
* Return
* Remarks
**********************************************************************/
BOOL Calc_OnCreate(HWND hWnd)
{
	int i;
	static HDC	hdc;
	COLORREF Color;
	BOOL bTrans;
	
	hdc = GetDC(hWnd);

	hDispNum = CreateBitmapFromImageFile(hdc, NUM_PATH, &Color, &bTrans);
	hSign_M = CreateBitmapFromImageFile(hdc, M_PATH, &Color, &bTrans);
	hSign_Add = CreateBitmapFromImageFile(hdc, ADD_PATH, &Color, &bTrans);
	hSign_Sub = CreateBitmapFromImageFile(hdc, SUB_PATH, &Color, &bTrans);
	hSign_Multiply = CreateBitmapFromImageFile(hdc, MULTIPLY_PATH, &Color, &bTrans);
	hSign_Divide = CreateBitmapFromImageFile(hdc, DIVIDE_PATH, &Color, &bTrans);
	for(i = 0; i < BTN_COUNT; i++)
		phbmp[i] = (HBITMAP *)CreateBitmapFromImageFile(hdc, pBmpPath[i], &Color, &bTrans);

	ReleaseDC(hWnd, hdc);
	InitMachine();


	return TRUE;
}

/**********************************************************************
* Function	  Calc_OnPaint
* Purpose     deal WM_PAINT message in main window
* Params
* Return
* Remarks
**********************************************************************/
void Calc_OnPaint(HWND hWnd)
{
	HDC hdc;
	int i;

	hdc = BeginPaint(hWnd, NULL);
	DrawRect(hdc, &ShowBarRect);
	for (i = 0; i < BTN_COUNT; i++)
	{
		DrawRect(hdc, &g_btns[i]);
		BitBlt(hdc, g_btns[i].left + 6, g_btns[i].top + 6, 
			g_btns[i].right - g_btns[i].left,
			g_btns[i].bottom - g_btns[i].top, 
			(HDC)phbmp[i], 0, 0, ROP_SRC);
	}

	BitBlt(hdc, g_btns[nCurBtn].left, g_btns[nCurBtn].top, 
	g_btns[nCurBtn].right - g_btns[nCurBtn].left,
	g_btns[nCurBtn].bottom - g_btns[nCurBtn].top,
	hdc,
	g_btns[nCurBtn].left,
	g_btns[nCurBtn].top,
	ROP_NSRC);

	if(ShowResult(hdc))
		ShowOprationSign(hdc, nCurSign);

//	nCurSign = -1;
	DeleteDC(hdc);
	EndPaint(hWnd, NULL);

	return;
}

/**********************************************************************
* Function	Calc_OnPenDown
* Purpose   deal WM_PENDOWN message in main window
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Calc_OnPenDown(HWND hWnd, int x, int y)
{
	POINT ptPen;
	int i;

	ptPen.x = x;
	ptPen.y = y;

	nCurSign = -1;
	for (i = 0; i < BTN_COUNT; i++)
	{
		if ( PtInRect(&(g_btns[i]), ptPen) )
			break;
	}
	if ( i == BTN_COUNT )
		return;
	nCurBtn = i;
	Calc_ConfirmInput(nCurBtn);
	InvalidateRect(hWnd, NULL, FALSE);
	return;
}

/**********************************************************************
* Function	Calc_OnChar
* Purpose   deal WM_CHAR message in main window
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Calc_OnChar(HWND hWnd, char c)
{
//	HDC hdc;
	int i;

	i = c - '0';		

	if (i < 0 || i > 9)
		return;		
	
	if (Calc_Input(i) < 0)
		SOUND;

	InvalidateRect(hWnd, NULL, FALSE);

	return;
}
/**********************************************************************
* Function	   Calc_OnKeyDown
* Purpose      deal WM_KEYDOWN message in main window
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void Calc_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{

	switch (nKey)
	{
	case VK_UP:
		Calc_ChangeFocus(hWnd, UP_DIRECTION);
		break;

	case VK_DOWN:
		Calc_ChangeFocus(hWnd, DOWN_DIRECTION);
		break;

	case VK_LEFT:
		Calc_ChangeFocus(hWnd, LEFT_DIRECTION);
		break;

	case VK_RIGHT:
		Calc_ChangeFocus(hWnd, RIGHT_DIRECTION);
		break;
	case VK_1:
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}
		nCurBtn = KEY_1;
		TimerId = KEY_1;
		SetTimer(hWnd, TimerId, 600, NULL);
		break;
	case VK_2:
	
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}
			
		nCurBtn = KEY_2;
		TimerId = KEY_2;
		SetTimer(hWnd, TimerId, 600, NULL);
		break;
	case VK_3:
	
			if (nCurSign > -1)
				{
					nCurSign = -1;
				}
		
		nCurBtn = KEY_3;
		TimerId = KEY_3;
		SetTimer(hWnd, TimerId, 600, NULL);
		break;
	case VK_4:
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}
		nCurBtn = KEY_4;
		TimerId = KEY_4;
		SetTimer(hWnd, TimerId, 600, NULL);
		break;
	case VK_5:
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}
		nCurBtn = KEY_5;
		TimerId = KEY_5;
		SetTimer(hWnd, TimerId, 600, NULL);
		break;
	case VK_6:
		
		if (nCurSign > -1)
				{
					nCurSign = -1;
				}
		
		nCurBtn = KEY_6;
		TimerId = KEY_6;
		SetTimer(hWnd, TimerId, 600, NULL);
		break;
	case VK_7:
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}
		nCurBtn = KEY_7;
		TimerId = KEY_7;
		SetTimer(hWnd, TimerId, 600, NULL);
		break;
	case VK_8:
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}
		nCurBtn = KEY_8;
		TimerId = KEY_8;
		SetTimer(hWnd, TimerId, 600, NULL);
		break;
	case VK_9:
		
		if (nCurSign > -1)
				{
					nCurSign = -1;
				}
		
		nCurBtn = KEY_9;
		TimerId = KEY_9;
		SetTimer(hWnd, TimerId, 600, NULL);
		break;
	case VK_0:
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}
		nCurBtn = KEY_0;
		TimerId = KEY_0 + 10;
		SetTimer(hWnd, TimerId, 600, NULL);
		
		break;
	case VK_F3:
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}
		nCurBtn = KEY_DOT;
		Calc_ConfirmInput(nCurBtn);
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case VK_F4:
		/*
		if (nCurSign > -1)
				{
					nCurSign = -1;
				}*/
		
		nCurBtn = KEY_MADD;
		Calc_ConfirmInput(nCurBtn);
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case VK_F5:
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}
		nCurSign = nCurBtn;
		if(nCurBtn >= 0 && nCurBtn <= 9)
			Calc_ConfirmInput(nCurBtn + 10);
		else
			Calc_ConfirmInput(nCurBtn);
		InvalidateRect(hWnd, &ShowBarRect, FALSE);
		break;

	case VK_RETURN:
		if (nCurSign > -1)
		{
			nCurSign = -1;
		}

		if ( Calc_Input(KEY_EQU) < 0 )
			SOUND;
		InvalidateRect(hWnd, &ShowBarRect, FALSE);
		break;

	case VK_F10:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
		break;
	}

	return;
}

/**********************************************************************
* Function	  Calc_OnDestroy
* Purpose     deal WM_DESTROY message in main window
* Params
* Return
* Remarks
**********************************************************************/
void Calc_OnDestroy(HWND hWnd)
{
	int i;

	DeleteObject(hDispNum);
	DeleteObject(hSign_M);
	DeleteObject(hSign_Add);
	DeleteObject(hSign_Sub);
	DeleteObject(hSign_Multiply);
	DeleteObject(hSign_Divide);

	for(i = 0; i < BTN_COUNT; i++)
		DeleteObject(phbmp[i]);
	Calc_hwndApp = NULL;
    DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);	

    Calc_hwndApp = NULL;

	return;
}

/**********************************************************************
* Function	ShowResult
* Purpose   counting result is showed in box
* Params	hdc
* Return
* Remarks
**********************************************************************/
BOOL ShowResult(HDC hdc)
{
	BOOL ret;
	int i;
	int len;
	HFONT hFont, hOld;
    
	ret = Calc_GetResult(DispBuf);
	len = strlen(DispBuf);

	if (!ret)
	{
		SetTextAlign(hdc, TA_RIGHT);
		GetFontHandle(&hFont, SMALL_FONT);
		hOld = SelectObject(hdc,hFont);
		TextOut(hdc, RES_DSP, RES_Y - 1, DispBuf, -1);
		SelectObject(hdc, hOld);
		return ret;
	}

	for (i=0; i<len; i++)
	{
		if ((DispBuf[i]-'0') >= 0 && (DispBuf[i]-'0') <= 9)
		{
			BitBlt(hdc, RES_DSP-(len-i-1)*RES_NUMW, RES_Y, RES_NUMW, RES_CY, (HDC)hDispNum, (DispBuf[i]-'0')*RES_NUMW, 0, ROP_SRC);
		}
		else if (DispBuf[i] == '-')
		{
			BitBlt(hdc, RES_DSP-(len-i-1)*RES_NUMW, RES_Y, RES_NUMW, RES_CY, (HDC)hDispNum, 13 * RES_NUMW, 0, ROP_SRC);
		}
		else if (DispBuf[i] == '.')
		{
			BitBlt(hdc, RES_DSP-(len-i-1)*RES_NUMW, RES_Y, RES_NUMW, RES_CY, (HDC)hDispNum, 10 * RES_NUMW, 0, ROP_SRC);
		}
		else
		{
			TextOut(hdc, RES_X+RES_CX-8, RES_Y+4, DispBuf, -1);
		}
	}

	return ret;
}

void ShowOprationSign(HDC hdc, int nSign)
{
	if(CM_mem == 0)
	{
		switch (nSign)
		{
			case 2:
				BitBlt(hdc, RES_NUMW, RES_Y, RES_NUMW, RES_CY, (HDC)hDispNum, 14*RES_NUMW, 0, ROP_SRC);
				break;

			case 5:
				BitBlt(hdc, RES_NUMW, RES_Y, RES_NUMW, RES_CY, (HDC)hDispNum, 13*RES_NUMW, 0, ROP_SRC);
				break;

			case 8:
				BitBlt(hdc, RES_NUMW, RES_Y, RES_NUMW, RES_CY, (HDC)hDispNum, 12*RES_NUMW, 0, ROP_SRC);
				break;

			case 0:
				BitBlt(hdc, RES_NUMW, RES_Y, RES_NUMW, RES_CY, (HDC)hDispNum, 11*RES_NUMW, 0, ROP_SRC);
				break;
			default:
				break;
		}
	}
	else
	{		
		BitBlt(hdc, SignRect0.left, SignRect0.top, 
					SignRect0.right - SignRect0.left, SignRect0.bottom - SignRect0.top,
					(HDC)hSign_M, 0, 0, ROP_SRC);
		switch (nSign)
		{
			case 2:
				BitBlt(hdc, SignRect1.left, SignRect1.top, 
					SignRect1.right - SignRect1.left, SignRect1.bottom - SignRect1.top,
					(HDC)hSign_Add, 0, 0, ROP_SRC);

//				DrawText(hdc, "M+", 5, &SignRect, DT_LEFT);
				break;
			case 5:
				BitBlt(hdc, SignRect1.left, SignRect1.top, 
					SignRect1.right - SignRect1.left, SignRect1.bottom - SignRect1.top,
					(HDC)hSign_Sub, 0, 0, ROP_SRC);
//				DrawText(hdc, "M-", 5, &SignRect, DT_LEFT);
				break;
			case 8:
				BitBlt(hdc, SignRect1.left, SignRect1.top, 
					SignRect1.right - SignRect1.left, SignRect1.bottom - SignRect1.top,
					(HDC)hSign_Multiply, 0, 0, ROP_SRC);
//				DrawText(hdc, "M*", 5, &SignRect, DT_LEFT);
				break;
			case 0:
				BitBlt(hdc, SignRect1.left, SignRect1.top, 
					SignRect1.right - SignRect1.left, SignRect1.bottom - SignRect1.top,
					(HDC)hSign_Divide, 0, 0, ROP_SRC);
//				DrawText(hdc, "M¡Â", 5, &SignRect, DT_LEFT);
				break;
			default:
//				DrawText(hdc, "M", 5, &SignRect, DT_LEFT);
				break;
		}

	}
	

	return;
}


/**=======================================================================
* function  : Calc_MoveFocus
* purpose  £ºmove focus
* params  £º
* return   : 
*
**=======================================================================*/
void Calc_ChangeFocus(HWND hWnd, int nDirection)
{
	int BtnNum;
	switch (nDirection)
	{
	case UP_DIRECTION:
		BtnNum = FocusPostion[nCurBtn].up;
		break;

	case DOWN_DIRECTION:
		BtnNum = FocusPostion[nCurBtn].down;
		break;

	case LEFT_DIRECTION:
		BtnNum = FocusPostion[nCurBtn].left;
		break;

	case RIGHT_DIRECTION:
		BtnNum = FocusPostion[nCurBtn].right;
		break;
	}

	nCurBtn = BtnNum;
	InvalidateRect(hWnd, &BtnsRect, FALSE);

	return;
}

/***=======================================================================
* function : MakePointInRect
* purpose :
* params : 
* return : 
*
**=======================================================================*/
POINT MakePointInRect()
{
	POINT pt;

	pt.x = g_btns[nCurBtn].left + 1;
	pt.y = g_btns[nCurBtn].top + 1;

	return pt;
}
/***=======================================================================
* function : InitValiable
* purpose :
* params : 
* return : 
*
**=======================================================================*/
void Calc_InitValiable()
{			
	nCurSign = -1;
	nCurBtn = 5;
}
/***=======================================================================
* function : Calc_ConfirmInput()
* purpose :
* params : 
* return : 
*
**=======================================================================*/
void Calc_ConfirmInput(int i)
{	
	if(bKeyLong && nCurBtn >= KEY_0 && nCurBtn <= KEY_9)
	{
		i = i + 10;
		if (CM_mem == 0 && nCurBtn != KEY_3 && nCurBtn !=KEY_6 && nCurBtn !=KEY_9 )
		{
			nCurSign = nCurBtn;	
		}

		if (CM_mem != 0) 
		{
			nCurSign = nCurBtn;
		}
		
	}

	if(nCurBtn == KEY_DOT || nCurBtn == KEY_MADD)
		i = i + 10;
	
	if ( Calc_Input(i) < 0 )
		SOUND;

	return;
}

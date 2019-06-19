/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 控件
 *
 * Purpose  : 游戏帮助的控件
 *            
\**************************************************************************/

#include "GameHelp.h"
#include "winpda.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"


#include "plx_pdaex.h"
//#ifdef	SCP_SMARTPHONE
#ifndef	_STR_PUBLIC_CTR_
#define	_STR_PUBLIC_CTR_
#include	"str_public.h"
#include	"str_plx.h"
#endif
//#endif
#include "mullang.h"
#include "pubapp.h"
//#ifdef SCP_SMARTPHONE
#define	IDS_BACK		ML("Back")//GetString(STR_WINDOW_BACK)
#define IDS_HELP ML("Help")
/*#else
#define	IDS_BACK		"返回"
#endif*/

#define	Y_SCROLLBAR				1
#define	LINE_PER_PAGE			9	

//控件ID
#define	IDC_SCROLLBAR		WM_USER+1001
#define	ID_EXIT				WM_USER+1002
#define	IDC_MYEDIT			WM_USER+1003

//Control.h中的相关定义

static LRESULT CALLBACK GameHelp_WndProc(HWND hWnd, UINT message,
										 WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK NewWnd_WndProc(HWND hWnd, UINT message,
										 WPARAM wParam, LPARAM lParam);
static BOOL SetSTBarState(HWND hWnd,int nFirstLine,int nLastLine,int nCurLen,int nLenVisual);
//static int	GetLineVisualNum(HWND hWnd);

//static HINSTANCE    hInstance;

/*********************************************************************
* Function	   GAMEHELP_RegisterClass
* Purpose      创建窗口类
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL GAMEHELP_RegisterClass(void* hInst)
{
    WNDCLASS wc;
//    hInstance = (HINSTANCE)hInst;
    
    wc.style            = NULL;
    wc.lpfnWndProc      = GameHelp_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = sizeof(STRU_GAMEHELP);
    wc.hInstance        = NULL;//(HINSTANCE)hInst;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = WC_GAMEHELP;
    
    if (!RegisterClass(&wc))
        return FALSE;
    
    return TRUE;
}

BOOL	CallGameHelpWnd(HWND hParent, PCSTR HelpText)
{
	HWND	hWndHelp;
	STRU_GAMEHELP GameHelp;
	
	memset(&GameHelp, 0, sizeof(STRU_GAMEHELP));
	GameHelp.pHelpText = malloc(strlen(HelpText)+1);
	strcpy(GameHelp.pHelpText, HelpText);
	GameHelp.hParentWnd = hParent;
	GameHelp.nCurLen = strlen(HelpText);
	
	GAMEHELP_RegisterClass(NULL);	
	
	hWndHelp = CreateWindow(
		WC_GAMEHELP, 
		IDS_HELP, 
		WS_VISIBLE|WS_CAPTION|PWS_STATICBAR,
		PLX_WIN_POSITION,
		hParent, 
		NULL,
		NULL, 
		(PVOID)&GameHelp);
	if (!hWndHelp) {
		return FALSE;
	}
	SetFocus(hWndHelp);
	return TRUE;
}
/*********************************************************************
* Function	   GameHelp_WndProc
* Purpose      游戏帮助的窗口过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static LRESULT CALLBACK GameHelp_WndProc(HWND hWnd, UINT message, 
                                           WPARAM wParam, LPARAM lParam)
{
    PSTRU_GAMEHELP  pGameHelp;
    LPCREATESTRUCT  pCreateStruct;
    LRESULT         lResult;
//	RECT			rcClient;
    
    pGameHelp = (PSTRU_GAMEHELP)GetUserData(hWnd);
    lResult = TRUE;
    
    switch(message)
    {
    case WM_CREATE:
        pCreateStruct			= (PCREATESTRUCT)lParam;
		memcpy(pGameHelp, (PSTRU_GAMEHELP)pCreateStruct->lpCreateParams, sizeof(STRU_GAMEHELP));
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT, 0), (LPARAM)IDS_BACK);
		pGameHelp->hTextView = PlxTextView(NULL, hWnd, pGameHelp->pHelpText, pGameHelp->nCurLen, 
			FALSE, NULL, NULL, 0);
        break;
/*
	case WM_ACTIVATE:
		SetFocus(hWnd);
		break;
*/
	case WM_KEYDOWN:
	
		switch(wParam)
		{
		case VK_F10:
			//SendMessage(pGameHelp->hParentWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		default:
			lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
		break;

	case WM_CLOSE:

		if (pGameHelp->pHelpText) {
			free(pGameHelp->pHelpText);
		}
		DestroyWindow(hWnd);
        UnregisterClass(WC_GAMEHELP,NULL);//(HINSTANCE)hInstance

		break;

       
    case WM_SETFOCUS:
		SetFocus(pGameHelp->hTextView);
        break;

        
    default :
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
    }

    return lResult;
}


void ProcessListBoxScroll(HWND hParent, HWND hList, int nItemNum)
{
	int index;

	SendMessage(hParent, PWM_SETSCROLLSTATE, SCROLLUP|SCROLLDOWN, MASKUP|MASKDOWN);
	index = SendMessage(hList, LB_GETCURSEL, 0, 0);
	
	if (nItemNum == 0)
		SendMessage(hParent, PWM_SETSCROLLSTATE, NULL, MASKUP|MASKDOWN);
	if (index == -1)
		SendMessage(hParent, PWM_SETSCROLLSTATE, NULL, MASKUP|MASKDOWN);

	if (index == nItemNum-1)
		SendMessage(hParent, PWM_SETSCROLLSTATE, NULL, MASKDOWN);
	if (index == 0)
		SendMessage(hParent, PWM_SETSCROLLSTATE, NULL, MASKUP);
	
}

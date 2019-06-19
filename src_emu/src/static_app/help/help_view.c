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
#include "winpda.h"

#include "plx_pdaex.h"
#include "string.h"
#include "pubapp.h"
#include "help.h"


static HWND hWndView;
static HWND hWndViewEditCtrl;
static char cContent[ContentMaxLength];
static char cKey[KeyMaxLength];

static LRESULT ViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void GetContent(char *pKey);
extern long Help_GetPrivateProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						const char *lpDefault,  /* points to default string */
						char *lpReturnedString,  /* points to destination buffer */
						unsigned long nSize,				/* size of destination buffer */
						char *lpFileName  /* points to initialization filename */
);


DWORD Help_View(char *pKey)
{
	WNDCLASS wc;
//	HFONT hFont, hOld;
//	HDC hdc;
	RECT rClient;
//	
	if(IsWindow(hWndView))
	{
		ShowWindow(hHelpFrameWnd, SW_SHOW);
		UpdateWindow(hHelpFrameWnd);
	}
	else
	{
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = GetStockObject(WHITE_BRUSH);
		wc.hCursor = NULL;
		wc.hIcon = NULL;
		wc.hInstance = NULL;
		wc.lpfnWndProc = ViewWndProc;
		wc.lpszClassName = "Help_ViewWndClass";
		wc.lpszMenuName = NULL;
		wc.style = 0;

		if(!RegisterClass(&wc))
			return (FALSE);
		strcpy(cKey , pKey);
		GetContent(pKey);

		GetClientRect(hHelpFrameWnd,&rClient);
		hWndView = CreateWindow(
			"Help_ViewWndClass",
			"",//ML(pKey),
			WS_VISIBLE | WS_CHILD ,//|WS_VSCROLL,
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hHelpFrameWnd,
			NULL,
			NULL,
			NULL);
		if(NULL == hWndView)
			return (FALSE);
		
	/*
		hdc = GetDC(hWndView);
			GetFontHandle(&hFont, SMALL_FONT);
			hOld = SelectObject(hdc,hFont);	
			SetWindowText(hWndViewEditCtrl, cContent);
	 		SelectObject(hdc, hOld);
			ReleaseDC(hWndView , hdc);*/
	
		
		ShowWindow(hHelpFrameWnd, SW_SHOW);
		UpdateWindow(hHelpFrameWnd);
	}

	return (TRUE);
}

//////////////////////////////////////////////////////////////////////////////
// Function	NotepadViewWinProc
// Purpose	
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static LRESULT ViewWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;
	RECT rcClient;
	static HWND hViewCtrl;

	switch(wMsgCmd)
	{
	case WM_CREATE:
		{
			
			GetClientRect(hWnd, &rcClient);
	/*
						hWndViewEditCtrl = CreateWindow("EDIT",
								"",
								WS_CHILD | WS_VISIBLE | WS_TABSTOP |ES_MULTILINE | ES_READONLY | ES_AUTOVSCROLL,
								rcClient.left, rcClient.top,
								rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
								hWnd,
								NULL,
								NULL,
								NULL);*/
			if (!strlen(cContent))
				strcpy(cContent, "have nothing");
			hViewCtrl = PlxTextView(hHelpFrameWnd, hWnd, cContent, strlen(cContent), FALSE, NULL, NULL, 0);
			SetFocus(hViewCtrl);
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
			SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
			if (strlen(cKey) > 20) 
			{
				cKey[21] = '.';
				cKey[22] = '.';
				cKey[23] = '.';
				cKey[24] = '\0';
			}
			SetWindowText(hHelpFrameWnd, ML(cKey));
		}
	
		break;
/*
			case WM_ACTIVATE:
				if (wParam == WA_ACTIVE)
					SetFocus(hWndViewEditCtrl);
				break;*/
	case PWM_SHOWWINDOW:
		SetFocus(hViewCtrl);
		SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
		SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
			SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
		SetWindowText(hHelpFrameWnd, ML(cKey));
		break;
		
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_RETURN:
			SendMessage(hWnd,WM_COMMAND, ID_OK, 0);
			break;
		default:
			lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_OK:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			SendMessage(hHelpFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			break;
		}
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		UnregisterClass("Help_ViewWndClass", NULL);
		hWndView = NULL;
		break;
	default:
		lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return (lRet);
}
static void GetContent(char *pKey)
{
	char cSection[TOPIC_NAMELENGTH];

	memset(cSection, 0, TOPIC_NAMELENGTH);
	strcpy(cSection, GetActiveLanguage());
	if (strlen(cSection) == 0) 
	{
		strcpy(cSection, "english");
	}

    Help_GetPrivateProfileString(cSection, pKey, ML("has nothing"), cContent, ContentMaxLength, ContentFilePath );

	
}


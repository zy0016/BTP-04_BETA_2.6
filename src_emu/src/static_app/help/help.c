/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Help application
 *
 * Purpose  : help user to use a mobilephone
 *            
\**************************************************************************/

#include "plx_pdaex.h"
#include "imesys.h"
#include "string.h"
#include "winpda.h"
#include "pubapp.h"
#include "stdlib.h"

#include "help.h"

extern DWORD Help_SubWindow(char *pSection, char *pAllKey[LIST_MAXNUM]);
extern long Help_GetAllKeyNamesInSection(						
						char *lpAppName,  /* points to section name */
						char *lpString[],		/* pointer to string to add */
						int  nSize,
						char *lpFileName  /* points to initialization filename */
);
extern int Help_HasKeyOrNoInSection(const char *lpAppName, char *lpFileName );
extern DWORD Help_View(char *pKey);
extern DWORD Help_Search();

static LRESULT MainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
void Help_CreateCtls(HWND hWndParent);
void Help_InitList(char *topic[], HWND hWnd);
static void Search(HWND hWnd);

static HINSTANCE hInstance = NULL;
HWND Help_MainWndApp;
static HWND hWndList;
static HWND hSearchEdit;
static HWND hErrEdit;
static HMENU hMenu;
static HBITMAP	hBmpFolder, hBmpDoc;//hBmpRightArrow;


/**********************************************************************
* Function	 Help_AppControl
* Purpose    application main function
* Params	
* Return	
* Remarks
**********************************************************************/
DWORD Help_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	WNDCLASS	    wc;
	DWORD		    dwRet;
	RECT			rClient;
    dwRet = TRUE;

    switch (nCode)
    {
		case APP_INIT:
			hInstance = (HINSTANCE)pInstance;
			break;

		case APP_GETOPTION:
			switch (wParam)
			{
			case AS_APPWND:
				dwRet = (DWORD)Help_MainWndApp;
				break;
			}
			break;

		case APP_ACTIVE:

			if(IsWindow(hHelpFrameWnd))
			{
				ShowWindow(hHelpFrameWnd, SW_SHOW);
				ShowOwnedPopups(hHelpFrameWnd, SW_SHOW);
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
				wc.lpfnWndProc = MainWndProc;
				wc.lpszClassName = "Help_MainWndClass";
				wc.lpszMenuName = NULL;
				wc.style = 0;
				if(!RegisterClass(&wc))
					return FALSE;
				
				hHelpFrameWnd = CreateFrameWindow( WS_CAPTION | PWS_STATICBAR);
				GetClientRect(hHelpFrameWnd,&rClient);
				Help_MainWndApp = CreateWindow(
					"Help_MainWndClass",
					"",//IDS_HELP,
					WS_VISIBLE | WS_CHILD,
					rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
					hHelpFrameWnd,
					NULL,
					NULL,
					NULL
					);

				
				ShowWindow(Help_MainWndApp, SW_SHOW);
				UpdateWindow(Help_MainWndApp);

			}
			break;

		case APP_INACTIVE:
			ShowOwnedPopups(hHelpFrameWnd, SW_HIDE);
			ShowWindow(hHelpFrameWnd,SW_HIDE); 
			break;

		default:
			break;
    }

    return dwRet;
}

/**********************************************************************
* Function	 Help_MainWndProc
* Purpose     main application function
* Params	
* Return	
* Remarks
**********************************************************************/
static LRESULT MainWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;
	int index;
	
	switch (wMsgCmd)
    {
		case WM_CREATE:
			Help_CreateCtls(hWnd);
			Help_InitList(cTopic, hWndList);
			SendMessage(hHelpFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT, 0), (LPARAM)IDS_EXIT);
			SendMessage(hHelpFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_OPEN, 1), (LPARAM)"");
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)ICON_RIGHTARROW);
			SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
			SetWindowText(hHelpFrameWnd, IDS_HELP);
			SetFocus(hWndList);
			break;

	/*
				case WM_ACTIVATE:
						if (wParam == WA_ACTIVE)
							SetFocus(hWndList);
						break;*/
		case PWM_SHOWWINDOW:
			SetFocus(hWndList);
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON),(LPARAM)ICON_RIGHTARROW);
			SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
			SetWindowText(hHelpFrameWnd, IDS_HELP);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case ID_OPEN:
					{
						int lenth;
						char *pAllKey[LIST_MAXNUM];
						char *pSection;
						int i;
						
						index = SendMessage(hWndList, LB_GETCURSEL, NULL, NULL);
						lenth = SendMessage(hWndList, LB_GETTEXTLEN, index, NULL);
						pSection = (char *)malloc((lenth+1)*sizeof(char));
						if (NULL == pSection)
							break;
						for(i = 0; i< LIST_MAXNUM; i ++)
						{
							pAllKey[i] = (char *)malloc((KeyMaxLength + 1) * sizeof(char));
							if (NULL ==pAllKey[i])
								break;
						}

						SendMessage(hWndList, LB_GETTEXT, index, (long)pSection);
						if(SendMessage(hWndList, LB_GETITEMDATA, index, 0))
						{
							Help_GetAllKeyNamesInSection(pSection, pAllKey, KeyMaxLength, TopicFilePath);
							Help_SubWindow(pSection, pAllKey);
						}
						else
							Help_View(pSection);
						for(i = 0; i< LIST_MAXNUM; i ++)
							free(pAllKey[i]);

						free(pSection);

						break;

					}
				default:
					break;
					
			}

		/*
				switch (HIWORD(wParam))
						{
						case EN_CHANGE:
							Help_Search(hWnd);
							break;
						}*/
			break;
		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{
				case VK_F10:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					SendMessage(hHelpFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					break;
				case VK_F5:
					SendMessage(hWnd, WM_COMMAND, ID_OPEN, NULL);
					break;
				case VK_RIGHT:
					Help_Search();
					break;
				default:
					lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
					break;
			}
			break;
		case WM_PAINT:
			BeginPaint(hWnd, NULL);
			EndPaint(hWnd, NULL);
			break;
			
		case WM_CLOSE:
			DestroyWindow(hWnd);
			DeleteObject(hBmpFolder);
			DeleteObject(hBmpDoc);
			//DeleteObject(hBmpRightArrow);
			UnregisterClass("Help_MainWndClass", NULL);
			UnregisterClass("Help_SubWndClass", NULL);
			SendMessage(hHelpFrameWnd, PWM_CLOSEWINDOW, 0, 0);
			DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance); 

			break;
		default:
			lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
	}

	return lRet;
}
/**********************************************************************
* Function	 Help_CreatCtls
* Purpose     create listbox and edit in main window
* Params	hWnd : parent window
* Return	
* Remarks
**********************************************************************/
void Help_CreateCtls(HWND hWndParent)
{
	RECT rc;
	HDC hdc;
//	COLORREF Color;
//	BOOL bTrans;
	
	hdc = GetDC(hWndParent);
	//hBmpFolder = CreateBitmapFromImageFile(hdc, ICON_FOLDER, &Color, &bTrans);
	//hBmpDoc = CreateBitmapFromImageFile(hdc, ICON_DOC, &Color, &bTrans);
	hBmpFolder = LoadImage(NULL, ICON_FOLDER, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
	hBmpDoc = LoadImage(NULL, ICON_DOC, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
//	hBmpRightArrow = CreateBitmapFromImageFile(hdc, ICON_RIGHTARROW, &Color, &bTrans);
	ReleaseDC(hWndParent, hdc);

	GetClientRect(hWndParent, &rc);
	hWndList = CreateWindow(
		"LISTBOX", 
		"",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_SORT | LBS_BITMAP,
		rc.left, rc.top,
		rc.right - rc.left, 
		rc.bottom - rc.top,
		hWndParent,
		NULL,
		NULL,
		NULL);
	
/*
	memset(&ie, 0, sizeof(IMEEDIT));
	ie.hwndNotify = hWnd;
	ie.dwAttrib = 0;
	ie.dwAscTextMax = 0;
	ie.dwUniTextMax = 0;
	ie.pszImeName ="Ó¢Êý";
	ie.pszCharSet = NULL;
	ie.pszTitle	= NULL;
	ie.uMsgSetText	= 0;

    *phEdit = CreateWindow (      
        "IMEEDIT","", 
        WS_CHILD | WS_VISIBLE| WS_BORDER | ES_LEFT ,
        SearchEditRect.left, SearchEditRect.top,
		SearchEditRect.right - SearchEditRect.left, 
		SearchEditRect.bottom - SearchEditRect.top,
        hWnd,
		(HMENU)ID_SEARCHEdit,
		NULL,
		(PVOID)&ie);
*/

	return;
}
/**********************************************************************
* Function	 Help_AddTopic
* Purpose    add topic to lisbox
* Params	
* Return	
* Remarks
**********************************************************************/
void Help_InitList(char *cTopic[], HWND hWnd)
{
	int i = 0;
	int index = 0;
	
	i = 0;
	while (cTopic[i] != NULL)
	{
		index = SendMessage(hWnd, LB_ADDSTRING,	NULL, (LPARAM)ML(cTopic[i]));

		if(Help_HasKeyOrNoInSection(ML(cTopic[i]),TopicFilePath) > 0)
		{
			SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hBmpFolder);
			SendMessage(hWnd, LB_SETITEMDATA, index, 1);
		}	
		else
		{
			SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hBmpDoc);
			SendMessage(hWnd, LB_SETITEMDATA, index, 0);
		}
		i++;
	}
	
	SendMessage(hWnd, LB_SETCURSEL, 0, NULL);
	return;
}


/**********************************************************************
* Function	 Help_Search
* Purpose    search position in listbox by characters in editbox 
* Params	
* Return	
* Remarks
**********************************************************************/
/*void Help_Search(HWND hWnd)
{
	char *buff = NULL;
	int index;


	if(GetWindowTextLength(hSearchEdit) == 0)
	{
		if(hErrEdit)
		{
			DestroyWindow(hErrEdit);
			hErrEdit = NULL;
			InvalidateRect(hWnd, NULL, NULL);
		}
		SendMessage(hMainList, LB_SETCURSEL, 0, NULL);
		return;
	}
	buff = malloc(TOPIC_NAMELENGTH+1);
	memset(buff,0,TOPIC_NAMELENGTH+1);
	GetWindowText(hSearchEdit, buff, TOPIC_NAMELENGTH+1);
	index = SendMessage(hMainList, LB_FINDSTRING, 1, (char) buff);
	if(index !=LB_ERR && hErrEdit)
	{	
		DestroyWindow(hErrEdit);
		hErrEdit = NULL;
		InvalidateRect(hWnd, NULL, NULL);	
	}
	if (index == LB_ERR && hErrEdit)
		return;

	if(index == LB_ERR && hErrEdit == NULL)
	{
		hErrEdit = CreateWindow("EDIT",
			"",
			WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY | ES_CENTER,
			HelpListRect.left, HelpListRect.top,
			HelpListRect.right - HelpListRect.left, 
			HelpListRect.bottom - HelpListRect.top,
			hWnd,
			NULL,
			NULL,
			NULL);
		SetWindowText(hErrEdit, HELP_ERROR);
		InvalidateRect(hWnd, NULL, NULL);
		return;
	}
	SendMessage(hMainList, LB_SETCURSEL, index, NULL);

	return;
}*/

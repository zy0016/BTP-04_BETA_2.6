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
#include "stdlib.h"
#include "mullang.h"
#include "pubapp.h"
#include "malloc.h"

#include "help.h"

typedef struct tagHelpWndGlobalData
{
	HWND hWndList;
	char *pAllKey[LIST_MAXNUM];
	char *pTopic;
}HELPWNDGLOBALDATA;

static HBITMAP	hBmpFolder, hBmpDoc;

static const char *GetCurrentSection();
static LRESULT SubWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
//extern void Help_CreateCtls(HWND hWnd);
static void Help_CreateCtls(HELPWNDGLOBALDATA *pData, HWND hWndParent);
static void Help_InitList(char *topic[], HWND hWnd);

extern DWORD Help_View(char *pKey);
extern int Help_HasKeyOrNoInSection(const char *lpAppName, char *lpFileName );

extern long Help_GetPrivateProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpDefault,  /* points to default string */
						char *lpReturnedString,  /* points to destination buffer */
						unsigned long nSize,				/* size of destination buffer */
						char *lpFileName  /* points to initialization filename */
);


//extern long FS_GetAllSectionNames( char *lpReturn, 
//						unsigned long nSize,  char *lpFileName );
extern long Help_GetAllKeyNamesInSection(						
						char *lpAppName,  /* points to section name */
						char *lpString[],		/* pointer to string to add */
						int  nSize,
						char *lpFileName  /* points to initialization filename */
);




DWORD Help_SubWindow(char *pSection, char *pKey[LIST_MAXNUM])
{
	WNDCLASS	    wc;
	HELPWNDGLOBALDATA Data;
	int i;
	RECT rClient;

	memset(&Data, 0, sizeof(HELPWNDGLOBALDATA));
	for(i = 0; i< LIST_MAXNUM; i++)
		Data.pAllKey[i] = pKey[i];

	Data.pTopic = (char *)malloc(strlen(pSection)+1);
	if (NULL == Data.pTopic)
		return FALSE;
	strcpy(Data.pTopic, pSection);
	
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(HELPWNDGLOBALDATA);
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hInstance = NULL;
	wc.lpfnWndProc = SubWndProc;
	wc.lpszClassName = "Help_SubWndClass";
	wc.lpszMenuName = NULL;
	wc.style = 0;
			
	RegisterClass(&wc);
	GetClientRect(hHelpFrameWnd,&rClient);
	
	CreateWindow(
		"Help_SubWndClass",
		"",//ML(pSection),
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hHelpFrameWnd,
		NULL,
		NULL,
		(PVOID)&Data
		);
	
//	free(Data.pTopic);
	ShowWindow(hHelpFrameWnd, SW_SHOW);
	UpdateWindow(hHelpFrameWnd);

	return TRUE;
}
/**********************************************************************
* Function	 Help_MainWndProc
* Purpose     main application function
* Params	
* Return	
* Remarks
**********************************************************************/
static LRESULT SubWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;
	HELPWNDGLOBALDATA *pData;

	pData = GetUserData(hWnd);
	
	switch (wMsgCmd)
    {
		case WM_CREATE:
			{
				LPCREATESTRUCT lpCreateStruct;
				lpCreateStruct = (LPCREATESTRUCT)lParam;
				memcpy(pData, lpCreateStruct->lpCreateParams, sizeof(HELPWNDGLOBALDATA));
				Help_CreateCtls(pData , hWnd);
				Help_InitList(pData->pAllKey, pData->hWndList);
				SetFocus(pData->hWndList);
				SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
				SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
			    SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
				SetWindowText(hHelpFrameWnd,ML(pData->pTopic));
			}
			break;
/*
					case WM_ACTIVATE:
						if (wParam == WA_ACTIVE)
							SetFocus(pData->hWndList);
						break;*/
		case PWM_SHOWWINDOW:
 			SetFocus(pData->hWndList);
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
			SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
			SetWindowText(hHelpFrameWnd,ML(pData->pTopic));
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
					int index;
					
					index = SendMessage(pData->hWndList, LB_GETCURSEL, NULL, NULL);
					lenth = SendMessage(pData->hWndList, LB_GETTEXTLEN, index, NULL);
					pSection = (char *)malloc((lenth+1)*sizeof(char));
					if (NULL ==pSection)
						break;
					for(i = 0; i< LIST_MAXNUM; i ++)
					{
						pAllKey[i] = (char *)malloc((KeyMaxLength + 1) * sizeof(char));
						if (NULL == pAllKey[i])
							break;
					}
					
					SendMessage(pData->hWndList, LB_GETTEXT, index, (long)pSection);
					if(SendMessage(pData->hWndList, LB_GETITEMDATA, index, 0))
					{
						Help_GetAllKeyNamesInSection(pSection, pAllKey, KeyMaxLength, TopicFilePath);
						Help_SubWindow(pSection, pAllKey);
					}
					else
					{
						Help_View(pSection);
					}

					for(i = 0; i< LIST_MAXNUM; i ++)
						free(pAllKey[i]);
					
					free(pSection);
					
				}
				break;
			case ID_BACK:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				SendMessage(hHelpFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				break;
			}

		case WM_PAINT:
			BeginPaint(hWnd, NULL);
			EndPaint(hWnd, NULL);
			break;
		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{
				case VK_F10:
					SendMessage(hWnd, WM_COMMAND, ID_BACK, NULL);
					break;
				case VK_F5:
					SendMessage(hWnd, WM_COMMAND, ID_OPEN, NULL);
					break;
				default:
					lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
					break;
			}
			break;
		case WM_CLOSE:
			if(pData->pTopic != NULL)
				free(pData->pTopic);
			DestroyWindow(hWnd);
			DeleteObject(hBmpFolder);
			DeleteObject(hBmpDoc);
			break;
		case WM_DESTROY:
		//	UnregisterClass("Help_SubWndClass", NULL);
			break;
		default:
			lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
	}

	return lRet;
}
static const char *GetCurrentSection()
{
	return GetActiveLanguage();
}
static void Help_InitList(char *cTopic[], HWND hWnd)
{
	int i = 0;
	int index = 0;
	
	i = 0;
	while (cTopic[i] != NULL && ML(cTopic[i]) != NULL)
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

static void Help_CreateCtls(HELPWNDGLOBALDATA *pData, HWND hWndParent)
{
	RECT rc;
	HDC hdc;
//	COLORREF Color;
//	BOOL bTrans;
	
	hdc = GetDC(hWndParent);
//	hBmpFolder = CreateBitmapFromImageFile(hdc, ICON_FOLDER, &Color, &bTrans);
//	hBmpDoc = CreateBitmapFromImageFile(hdc, ICON_DOC, &Color, &bTrans);
	hBmpFolder = LoadImage(NULL, ICON_FOLDER, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
	hBmpDoc = LoadImage(NULL, ICON_DOC, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
	ReleaseDC(hWndParent, hdc);

	GetClientRect(hWndParent, &rc);
	pData->hWndList = CreateWindow(
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
}


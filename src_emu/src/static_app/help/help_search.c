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


static LRESULT SearchWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void CreateCtls(HWND hWnd);
static void InitList(char *topic[], HWND hWnd);
static void Search(HWND hWnd);
extern DWORD Help_View(char *pKey);
extern long Help_GetAllKeyNamesInSection(						
						char *lpAppName,  /* points to section name */
						char *lpString[],		/* pointer to string to add */
						int  nSize,
						char *lpFileName  /* points to initialization filename */
);

static HINSTANCE hInstance = NULL;
static HWND hWndSearch;
static HWND hWndList;
static HWND hWndEdit;
static HBITMAP hBmpSearch, hBmpDoc;//hBmpLeftArrow

extern HWND Help_MainWndApp;

DWORD Help_Search()
{
	WNDCLASS	    wc;
	DWORD		    dwRet;
	RECT			rClient;
    dwRet = TRUE;


	if(IsWindow(hWndSearch))
	{
		ShowWindow(hHelpFrameWnd, SW_SHOW);
		UpdateWindow(hHelpFrameWnd);
	}
	else
	{
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = 0;//GetStockObject(WHITE_BRUSH);
		wc.hCursor = NULL;
		wc.hIcon = NULL;
		wc.hInstance = NULL;
		wc.lpfnWndProc = SearchWndProc;
		wc.lpszClassName = "Help_SearchWndClass";
		wc.lpszMenuName = NULL;
		wc.style = 0;
		if(!RegisterClass(&wc))
			return FALSE;

		GetClientRect(hHelpFrameWnd, &rClient);
		hWndSearch = CreateWindow(
			"Help_SearchWndClass",
			"",//IDS_HELPINDEX,
			WS_VISIBLE| WS_CHILD,
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hHelpFrameWnd,
			NULL,
			NULL,
			NULL
			);
		
	
		ShowWindow(hHelpFrameWnd, SW_SHOW);
		UpdateWindow(hHelpFrameWnd);
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
static LRESULT SearchWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;

	switch (wMsgCmd)
    {
		case WM_CREATE:
			{
				HDC hdc;
	//			COLORREF Color;
	//			BOOL bTrans;

				CreateCtls(hWnd);
				SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
				SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SetWindowText(hHelpFrameWnd,IDS_HELPINDEX);
				SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT, 0), (LPARAM)IDS_EXIT);
				SetFocus(hWndEdit);
				hdc = GetDC(hWnd);
				hBmpDoc = LoadImage(NULL, ICON_DOC, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
//				hBmpLeftArrow = CreateBitmapFromImageFile(hdc, ICON_LEFTARROW, &Color, &bTrans);
				ReleaseDC(hWnd, hdc);
				SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
				SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM) ICON_LEFTARROW);
			}
			break;

/*
					case WM_ACTIVATE:
						if (wParam == WA_ACTIVE)
							SetFocus(hWndEdit);
						break;*/
		case PWM_SHOWWINDOW:
			{
				int iCount;
				SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_EXIT);
				SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);
				SendMessage(hHelpFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON),(LPARAM)ICON_LEFTARROW);

				iCount = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
				if (iCount > 0)
				{
					SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}
				else
				{
					SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				}
				
				SetWindowText(hHelpFrameWnd,IDS_HELPINDEX);
				SetFocus(hWndEdit);
			}
			break;
		
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case ID_OPEN:
					{
						int lenth;
						char *pKey;
						int index;
						
						index = SendMessage(hWndList, LB_GETCURSEL, NULL, NULL);
						lenth = SendMessage(hWndList, LB_GETTEXTLEN, index, NULL);
						pKey = (char *)malloc((lenth+1)*sizeof(char));
						if (NULL ==pKey)
							break;
						SendMessage(hWndList, LB_GETTEXT, index, (long)pKey);
						Help_View(pKey);
					}
					
					break;
				case ID_EXIT:
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					SendMessage(hHelpFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					break;
			}

			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
				SetTimer(hWnd, ID_TIMER, 1000, NULL);
				break;
			}
			break;
		case WM_TIMER:
			{
				int iCount;
				
				KillTimer(hWnd, ID_TIMER);
				Search(hWnd);
				iCount = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
				if (iCount > 0)
				{
					SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}
				else
				{
					SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				}
			}
			break;

		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{
				case VK_F10:
					SendMessage(hWnd, WM_COMMAND, ID_EXIT , 0);
					SendMessage(Help_MainWndApp, WM_KEYDOWN, wParam, lParam);
					break;
				case VK_LEFT:
					SendMessage(hWnd, WM_COMMAND, ID_EXIT , 0);
					break;
				case VK_UP:
					{
						int iCurSel;
						int iCount;

						iCount = SendMessage(hWndList, LB_GETCOUNT, 0, 0);

						iCurSel = SendMessage(hWndList, LB_GETCURSEL, 0, 0);

						if (iCurSel > 0)
						{
							SendMessage(hWndList, LB_SETCURSEL, iCurSel -1, NULL);
						}
						else
						{
							SendMessage(hWndList, LB_SETCURSEL, iCount -1, NULL);
						}
						
					}
					break;
				case VK_DOWN:
					{
						int iCurSel;
						int iCount;

						iCount = SendMessage(hWndList, LB_GETCOUNT, 0, 0);
						
						iCurSel = SendMessage(hWndList, LB_GETCURSEL, 0, 0);
						if (iCurSel < iCount -1)
						{
							SendMessage(hWndList, LB_SETCURSEL, iCurSel +1, NULL);
						}
						else
						{
							SendMessage(hWndList, LB_SETCURSEL, 0, NULL);
						}
						
					}
					break;
				case VK_1:
				case VK_2:
				case VK_3:
				case VK_4:
				case VK_5:
				case VK_6:
				case VK_7:
				case VK_8:
				case VK_9:
				case VK_0:
					SetFocus(hWndEdit);
					break;
				case VK_F5:
					{
						int lenth;
						char *pSection;
						int index;
						
						if (SendMessage(hWndList, LB_GETCOUNT, 0, 0) == 0)
							break;
						else
						{
							index = SendMessage(hWndList, LB_GETCURSEL, NULL, NULL);
							lenth = SendMessage(hWndList, LB_GETTEXTLEN, index, NULL);
							pSection = (char *)malloc((lenth+1)*sizeof(char));
							if (NULL == pSection)
								break;
							SendMessage(hWndList, LB_GETTEXT, index, (long)pSection);
							/*
							if (strlen(pSection) > 20) 
							{
								pSection[21] = '.';
								pSection[22] = '.';
								pSection[23] = '.';
								pSection[24] = '\0';
							}*/
							
							Help_View(pSection);
						}
						
					}
					break;
				default:
					lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
					break;
			}
			break;
		case WM_PAINT:
			{
				HDC hdc;
				int nBkMode;
				COLORREF BkColor;
				
				hdc = BeginPaint(hWnd, NULL);
				nBkMode = SetBkMode(hdc, BM_NEWTRANSPARENT);
				BkColor = SetBkColor(hdc, COLOR_BLACK);
				BitBlt( hdc, SearchBmpRect.left, SearchBmpRect.top, 
					BMP_WIDTH1, BMP_HIGTH, (HDC)hBmpSearch, 0, 0, ROP_SRC);
				SetBkColor(hdc, BkColor);
				SetBkMode(hdc, nBkMode);
				DeleteDC(hdc);
				EndPaint(hWnd, NULL);
			}
			break;
		case WM_SETRBTNTEXT:
			SendMessage(hHelpFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)lParam);
			break;
		case WM_CLOSE:
			DestroyWindow(hWnd);
			DeleteObject(hBmpSearch);
			DeleteObject(hBmpDoc);
			//DeleteObject(hBmpLeftArrow);
			UnregisterClass("Help_SearchWndClass", NULL);

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
static void CreateCtls(HWND hWnd)
{
	IMEEDIT ie;
//	COLORREF Color;
//	BOOL bTrans;
	HDC hdc;

	hdc = GetDC(hWnd);
//	hBmpSearch = CreateBitmapFromImageFile(hdc, ICON_SEARCH, &Color, &bTrans);
	hBmpSearch = LoadImage(NULL, ICON_SEARCH, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
	ReleaseDC(hWnd, hdc);

	hWndList = CreateWindow(
		"LISTBOX", "",
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_SORT | LBS_BITMAP,
		HelpListRect.left, HelpListRect.top,
		HelpListRect.right - HelpListRect.left, 
		HelpListRect.bottom - HelpListRect.top,
		hWnd,
		NULL,
		NULL,
		NULL);
	
	memset(&ie, 0, sizeof(IMEEDIT));
	ie.hwndNotify = hWnd;
	ie.dwAttrib = 0;
	ie.dwAscTextMax = 0;
	ie.dwUniTextMax = 0;
	ie.pszImeName = NULL;//"Ó¢Êý"
	ie.pszCharSet = NULL;
	ie.pszTitle	= NULL;
	ie.uMsgSetText	= 0;

    hWndEdit = CreateWindow (      
        "IMEEDIT","", 
        WS_CHILD | WS_VISIBLE| ES_LEFT | CS_NOSYSCTRL ,
        SearchEditRect.left, SearchEditRect.top,
		SearchEditRect.right - SearchEditRect.left, 
		SearchEditRect.bottom - SearchEditRect.top,
        hWnd,
		NULL,
		NULL,
		(PVOID)&ie);

	return;
}
/**********************************************************************
* Function	 Help_AddTopic
* Purpose    add topic to lisbox
* Params	
* Return	
* Remarks
/*
********************************************************************** /
void Help_InitList(char *topic[], HWND hWnd)
{
	int i = 0;
	int index = 0;
	
	while (topic[i] != NULL)
	{
		index = SendMessage(hWnd, LB_ADDSTRING,	NULL, (LPARAM)topic[i]);
		++i;
	}

	SendMessage(hWnd, LB_SETCURSEL, 0, NULL);
	return;
}*/

/**********************************************************************
* Function	 Search
* Purpose    search position in listbox by characters in editbox 
* Params	
* Return	
* Remarks
**********************************************************************/
static void Search(HWND hWnd)
{
	char *buff = NULL;
	int iNum = 0;
	char cSection[SectionMaxLength];
	char *pAllKey[LIST_MAXNUM];
	int i, iIndex;
	
	if(SendMessage(hWndList, LB_GETCOUNT, 0, 0))
		SendMessage(hWndList, LB_RESETCONTENT, 0, 0);
	buff = (char *)malloc(TOPIC_NAMELENGTH+1);
	if (NULL == buff)
		return;
	memset(buff,0,TOPIC_NAMELENGTH+1);
	GetWindowText(hWndEdit, buff, TOPIC_NAMELENGTH+1);
	if (strlen(buff) == 0)
	{
		free(buff);
		return;
	}
    memset(cSection, 0, SectionMaxLength);
	strcpy(cSection, GetActiveLanguage());
	if (strlen(cSection) == 0) 
	{
		strcpy(cSection, "english");
	}
	for(i = 0; i< LIST_MAXNUM; i ++)
	{
		pAllKey[i] = (char *)malloc((KeyMaxLength + 1) * sizeof(char));
		if (NULL ==pAllKey[i])
			break;
	}
	Help_GetAllKeyNamesInSection(cSection, pAllKey, KeyMaxLength, ContentFilePath);

	for(i = 0; pAllKey[i] != NULL ; i++)
	{
		if(strstr(pAllKey[i], buff) != NULL)
		{
			iIndex = SendMessage(hWndList, LB_ADDSTRING, NULL, (long)pAllKey[i]);
			SendMessage(hWndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, iIndex), (LPARAM)hBmpDoc);
			iNum ++;
		}
	}
	
	if (iNum)
		SendMessage(hWndList, LB_SETCURSEL, 0, NULL);
	free(buff);
	for(i = 0; i< LIST_MAXNUM; i ++)
		free(pAllKey[i]);
	return;
}


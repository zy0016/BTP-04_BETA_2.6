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

#include "window.h"
#include "string.h"
#include "malloc.h"
#include "wappush.h"
#include "unistd.h"
#include "sys\stat.h"
#include "sys\types.h"
#include "fcntl.h"
#include "stdio.h"
#include "wUipub.h"
#include "winpda.h"
#include "mullang.h"
#include "pwbe.h"
#include "wappushapi.h"
#include "pubapp.h"
#include "wie.h"
#include "plx_pdaex.h"
#include "prioman.h"

#define WIE_ID_PUSH_LIST        (WM_USER+101)
#define IDM_WML_PUSH_OPEN (WM_USER+201)
#define IDM_WML_PUSH_DETAIL (WM_USER+202)
#define IDM_WML_PUSH_DELETE	(WM_USER+203)
#define IDM_WML_PUSH_DELETEALL	(WM_USER+204)
#define IDM_WML_PUSH_DELETECONFIRM (WM_USER+205)
#define IDM_WML_PUSH_DELETEALLCONFIRM  (WM_USER+206)
#define EACHLINE	28

static HMENU hpushMenu;

int nTotalPush = 0;
int nUnread = 0;
HWND hPushbox;
RECT rClient;
HWND hPushlist = NULL;
HWND hPushDetail = NULL;
char *DetailTitle, *DetailUrl;
int lines = 0;
int distance = 0, keylines = 0, movekeylines = 0;
int startpos = 28;
BOOL bmove = FALSE;
int nPushMaxWidth = 0;
RECT rPushClient;
int nPushHeight = 0;
BOOL bPushGoto = FALSE;
HBITMAP hreadpushbmp = NULL;
HBITMAP hunreadpushbmp = NULL;

extern HWND hwndIEApp;
extern int PushMsgHandle[MAX_PUSH_NUM];
extern BOOL bPushenter;
extern BOOL bPushexit;
extern HWND hWapFrame;
extern HFONT hViewFont;

extern int WP_GetTotalCount(void);

static LRESULT WMLPushMessageProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT PushDetailsWNDProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL WP_OnPushDetail(HWND hParent, PUSHFILERECORD record);



int WAP_PushMessage(HWND hWapFrame)
{
	WNDCLASS wc;            
	
	wc.style         = 0;
	wc.lpfnWndProc   = WMLPushMessageProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "WMLPushClass";
	
	if (!RegisterClass(&wc))
		return 0;
	
	GetClientRect(hwndIEApp, &rClient);

	hpushMenu = CreateMenu();
	hpushMenu = CreateMenu();
	InsertMenu(hpushMenu, 0, MF_BYPOSITION, IDM_WML_PUSH_OPEN, WML_PUSH_OPEN); 
	InsertMenu(hpushMenu, 1, MF_BYPOSITION, IDM_WML_PUSH_DETAIL, WML_PUSH_DETAIL);
	InsertMenu(hpushMenu, 2, MF_BYPOSITION, IDM_WML_PUSH_DELETE, WML_PUSH_DELETE);
	InsertMenu(hpushMenu, 3, MF_BYPOSITION, IDM_WML_PUSH_DELETEALL, WML_PUSH_DELETEALL);
    	
	hPushbox = CreateWindow(
		"WMLPushClass", 
		"", 
		WS_VISIBLE|WS_CHILD,
		rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
		GetWAPFrameWindow(), 
		NULL,
		NULL, 
		NULL
		);	
	
	if ( hPushbox == NULL ) 
    {
        UnregisterClass("WMLPushClass",NULL);
        return 0;
    }   
    ShowWindow(hPushbox, SW_SHOW);   
    UpdateWindow(hPushbox);
    
    return 1;
}


static LRESULT WMLPushMessageProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;
	HDC hdc;

	switch (wMsgCmd)
		{
		case WM_CREATE:
			{
				hPushlist = CreateWindow("LISTBOX","",
					WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_VSCROLL|LBS_BITMAP|LBS_MULTILINE,
					rClient.left, rClient.top, 
					rClient.right-rClient.left, 
					rClient.bottom-rClient.top,            
					hWnd,
					(HMENU)WIE_ID_PUSH_LIST, 
					NULL,NULL);   
				
				SetFocus(hPushlist);
				
				SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_PUSHCAPTION);	
				if(!bPushenter)
					SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
				else
					SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_EXIT);
				
				nTotalPush = WP_GetTotalCount();

				if(nTotalPush == 0)
				{
					PDASetMenu(GetWAPFrameWindow(), NULL);
//					SendMessage(hPushlist, LB_RESETCONTENT, NULL, NULL);
//					SendMessage(hPushlist, LB_ADDSTRING, 0, (LPARAM)WML_NOPUSHMESSAGE);
//					SendMessage(hPushlist, LB_SETCURSEL, 0, 0);
					SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
					SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
					SetFocus(GetWAPFrameWindow());
					ShowWindow(hPushlist, SW_HIDE);
					SetFocus(hWnd);
					
				}
				else
				{
					PDASetMenu(GetWAPFrameWindow(), hpushMenu);
					SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
					SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_PUSH_OPEN);
					ShowWindow(hPushlist, SW_SHOW);
				}

				hreadpushbmp = LoadImage(NULL, WIE_IMG_READPUSH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				hunreadpushbmp = LoadImage(NULL, WIE_IMG_UNREADPUSH, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
				WP_InitMsgHandle();
				if(WP_GetHandleReady(hPushlist))
					SendMessage(hPushlist, LB_SETCURSEL, 0, 0);

				SetFocus(hPushlist);
			}			
			break;

		case PWM_SHOWWINDOW:
			if(nTotalPush == 0)
			{
				PDASetMenu(GetWAPFrameWindow(), NULL);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
//				SendMessage(hPushlist, LB_RESETCONTENT, NULL, NULL);
//				SendMessage(hPushlist, LB_ADDSTRING, 0, (LPARAM)WML_NOPUSHMESSAGE);
//				SendMessage(hPushlist, LB_SETCURSEL, 0, 0);
				SetFocus(GetWAPFrameWindow());
				ShowWindow(hPushlist, SW_HIDE);
				SetFocus(hWnd);
			}
			else
			{
				PDASetMenu(GetWAPFrameWindow(), hpushMenu);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_PUSH_OPEN);
				ShowWindow(hPushlist, SW_SHOW);
			}
			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_PUSHCAPTION);	
			if(!bPushenter)
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
			else
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_EXIT);
			SetFocus(hPushlist);
			break;

		case WM_SETFOCUS:
			SetFocus(hPushlist);
			break;

		case IDM_WML_PUSH_DELETECONFIRM:
			if(lParam == 1)
			{
				int index;
				
				index = SendMessage(hPushlist, LB_GETCURSEL, NULL, NULL);
				if(WP_DeleteRecord(PushMsgHandle[index]))
					PLXTipsWin(NULL, NULL, NULL, WML_DELETED,NULL,
					Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
				WP_GetHandleReady(hPushlist);
				
				SendMessage(hPushlist, LB_SETCURSEL, 0, 0);
				SetFocus(hPushlist);
			}
			nTotalPush = WP_GetTotalCount();
			break;

		case IDM_WML_PUSH_DELETEALLCONFIRM:
			if(lParam == 1)
			{
				int i;

				remove(FILE_PUSH_WAP);
				for(i = 0; i < nTotalPush; i++)
					PushMsgHandle[i] = -1;
				nTotalPush = 0;
				nUnread = 0;
				
				PDASetMenu(GetWAPFrameWindow(), NULL);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SetFocus(GetWAPFrameWindow());
				ShowWindow(hPushlist, SW_HIDE);
				SetFocus(hWnd);
			}
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
			case VK_F10:
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;

			case VK_F5:
				return PDADefWindowProc(GetWAPFrameWindow(), wMsgCmd, wParam, lParam);
				break;

			case VK_RETURN:
				if(nTotalPush != 0)
					SendMessage(hWnd, WM_COMMAND, IDM_WML_PUSH_OPEN, NULL);
				break;
				
			default:
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);            
			}
			break;
		case WM_COMMAND:
			{
				int ID;

				ID = LOWORD(wParam);

				switch (ID)
				{
				case IDM_WML_PUSH_OPEN:
					{
						int index;
						PUSHFILERECORD record;
	
						index = SendMessage(hPushlist, LB_GETCURSEL, NULL, NULL);

						WP_GetRecord(PushMsgHandle[index], &record);
						if((record.pszUri != NULL) && (strlen(record.pszUri) != 0))
						{
							SetCurentPage(record.pszUri, URL_REMOTE);
							if(((hWapFrame != NULL) && (IsWindow(hWapFrame))))
							{
								UI_Message(UM_URLGO, (unsigned long)record.pszUri, URL_REMOTE);
							}
							bPushGoto = TRUE;
							WP_ResetFlag(PushMsgHandle[index], WP_PUSH_READ);
						}
						PostMessage(hWnd, WM_CLOSE, NULL, NULL);
					}
					break;

				case IDM_WML_PUSH_DETAIL:
					{
						int index;
						PUSHFILERECORD record;

						index = SendMessage(hPushlist, LB_GETCURSEL, NULL, NULL);

						WP_GetRecord(PushMsgHandle[index], &record);
						WP_OnPushDetail(GetWAPFrameWindow(), record);
					}
					
					break;

				case IDM_WML_PUSH_DELETE:
					PLXConfirmWinEx(NULL, hWnd, WML_DELETEPUSHONE, Notify_Request, (char *)NULL, 
						IDS_YES, IDS_NO, IDM_WML_PUSH_DELETECONFIRM);
					break;

				case IDM_WML_PUSH_DELETEALL:
					PLXConfirmWinEx(NULL, hWnd, WML_DELETEPUSHALL, Notify_Request, (char *)NULL, 
						IDS_YES, IDS_NO, IDM_WML_PUSH_DELETEALLCONFIRM);
					break;
				}
			}
			break;

		case WM_ACTIVATE:
			if (WA_ACTIVE == LOWORD(wParam))
				SetFocus(hPushbox);//激活后设置焦点
			break;

		case WM_PAINT:
			{
				RECT rtClient;
				int bkmodeold;
				
				hdc = BeginPaint(hWnd, NULL);
				GetClientRect(hWnd, &rtClient);
				bkmodeold = SetBkMode(hdc, TRANSPARENT);
				if(nTotalPush == 0)
				{
					DrawText(hdc, WML_NOPUSHMESSAGE, -1, &rtClient, DT_VCENTER|DT_HCENTER);
				}
				SetBkMode(hdc, bkmodeold);
				EndPaint(hWnd, NULL);  
			}      
			break;
			
		case WM_CLOSE:
			SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			DestroyMenu(hpushMenu);
			DestroyWindow(hWnd);
			break;

		case WM_DESTROY:
			if(bPushenter)
			{
				if(bPushexit)
				{
					bPushexit = FALSE;
					if(!bPushGoto)
					{
						bPushGoto = FALSE;
						SendMessage(hwndIEApp, WM_SELOK, NULL, NULL);
					}
				}
				bPushenter = FALSE;
				bPushGoto = FALSE;	
			}
			UnregisterClass("WMLPushClass", NULL);
			DeleteObject(hreadpushbmp);
			DeleteObject(hunreadpushbmp);
			hPushlist = NULL;
			break;

		default:        
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}

		return lResult; 
}

WB_BOOL WP_PushSIData(void *data)
{
	SIINFORECORD *SIdata;
	PUSHFILERECORD WPrecord;
	int titlelen, urllen;
	char *tmp;
	int tmplen;

	nTotalPush++;

	SIdata = (SIINFORECORD *)data;
	
	WPrecord.status = WP_PUSH_UNREAD;
	WPrecord.type = CONTENT_TYPE_WAPSIC;
	titlelen = strlen(SIdata->pszTitle);
	urllen = strlen(SIdata->pszUri);
	WPrecord.TitleLen = titlelen;
	WPrecord.UrlLen = urllen;
	WPrecord.pszTitle = (char *)malloc(titlelen + 1);
	memset(WPrecord.pszTitle, 0, titlelen + 1);
	WPrecord.pszUri = (char *)malloc(urllen + 1);
	memset(WPrecord.pszUri, 0, urllen + 1);

	tmplen = UTF8ToMultiByte(CP_ACP, 0, SIdata->pszTitle, -1, NULL, 0, NULL, NULL);
	tmp = (char *)malloc(tmplen + 1);
	memset(tmp, 0, tmplen + 1);
	UTF8ToMultiByte(CP_ACP, 0, SIdata->pszTitle, -1, tmp, tmplen, NULL, NULL);
	tmp[tmplen] = '\0';

	strcpy(WPrecord.pszTitle, tmp);
	free(tmp);
	tmp = NULL;
	
	tmplen = UTF8ToMultiByte(CP_ACP, 0, SIdata->pszUri, -1, NULL, 0, NULL, NULL);
	tmp = (char *)malloc(tmplen + 1);
	memset(tmp, 0, tmplen + 1);
	UTF8ToMultiByte(CP_ACP, 0, SIdata->pszUri, -1, tmp, tmplen, NULL, NULL);
	tmp[tmplen] = '\0';

	strcpy(WPrecord.pszUri, tmp);
	free(tmp);
	tmp = NULL;

	WP_InsertRecord(&WPrecord);
	
	printf("\r\n+++++++++++++++++++ Wap Get a Push Message from Heaven!!! ++++++++++++++++\r\n");
	if(hPushlist != NULL)
	{
		nTotalPush = WP_GetTotalCount();
		{
			PDASetMenu(GetWAPFrameWindow(), hpushMenu);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_PUSH_OPEN);
			ShowWindow(hPushlist, SW_SHOW);
		}
		
		WP_InitMsgHandle();
		if(WP_GetHandleReady(hPushlist))
			SendMessage(hPushlist, LB_SETCURSEL, 0, 0);
		
		SetFocus(hPushlist);
	}

	free(WPrecord.pszTitle);
	free(WPrecord.pszUri);
	DlmNotify(PS_NEWMSG, 3);
	DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);	//turn on back light
	PrioMan_CallMusicEx(PRIOMAN_PRIORITY_PUSHINFO, 3000);
//	DlmNotify(PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);	//back light can be off after 20s
	return TRUE;
}

HWND hPushDetails;
BOOL WP_OnPushDetail(HWND hParent, PUSHFILERECORD record)
{
	WNDCLASS wc;

	GetClientRect(hParent, &rPushClient); 
    
    wc.style         = 0;
    wc.lpfnWndProc   = PushDetailsWNDProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WP_PushDetail";
    
	RegisterClass(&wc); 
	
	DetailTitle = (char *)malloc(record.TitleLen + 1);
	memset(DetailTitle, 0, record.TitleLen + 1);
	DetailUrl = (char *)malloc(record.UrlLen + 1);
	memset(DetailUrl, 0, record.UrlLen + 1);
	strcpy(DetailTitle, record.pszTitle);
	strcpy(DetailUrl, record.pszUri);
	free(record.pszTitle);
	free(record.pszUri);
	
	nPushMaxWidth = rPushClient.right-rPushClient.left - 6;
    
    hPushDetails = CreateWindow(
        "WP_PushDetail", 
        "", 
        WS_VISIBLE|WS_CHILD|WS_VSCROLL,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        NULL
        );
    if (hPushDetails == NULL) 
    {
        UnregisterClass("WP_PushDetail", NULL);
        return FALSE;
    }
    
    ShowWindow(hPushDetails, SW_SHOW);            
    UpdateWindow(hPushDetails);
    
    return TRUE;
}

LRESULT PushDetailsWNDProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_DETAILS);
			PDASetMenu(GetWAPFrameWindow(), NULL);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK); 
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			RedrawScrollBar(hWnd, 6, 0, 6);
			SetFocus(hWnd);
		}
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_DETAILS);
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK); 
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		break;  
        
    case WM_PAINT:        
		{
			int bkmodeold;
			int i = 0, j = 0, k = 0;
			char tmpstring[50];
			int titlelen, urllen, linesurl = 0, linestitle = 0, movelen = 0;
//			char *szMulti;
//			int nLenMulti;
			int nLinechalen = 0;
			HFONT hBig;

			hdc = BeginPaint(hWnd, NULL);
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			titlelen = strlen(DetailTitle);
			urllen = strlen(DetailUrl);

			//Url Detail
			hBig = SelectObject(hdc, hViewFont);
			TextOut(hdc, 0, startpos - 23, STRWMLDETAILPUSHURL, -1);
			SelectObject(hdc, hBig);

				memset(tmpstring, 0, 50);
				urllen = strlen(DetailUrl);

				do {
					memset(tmpstring, 0, 50);
					GetTextExtentExPoint(hdc, &DetailUrl[j], urllen, nPushMaxWidth, &nLinechalen, NULL, NULL);
					strncpy(tmpstring, &DetailUrl[j], nLinechalen);
					urllen -= nLinechalen;
					if(urllen > 0)
					{
						j += nLinechalen;
						
//						nLenMulti = UTF8ToMultiByte(CP_ACP, 0, tmpstring, -1, NULL, 0, NULL, NULL);
//						szMulti = (char *)malloc(nLenMulti + 1);
//						memset(szMulti, 0, nLenMulti + 1);
//						UTF8ToMultiByte(CP_ACP, 0, tmpstring, -1, szMulti, nLenMulti, NULL, NULL);
//						szMulti[nLenMulti] = '\0';
						
						TextOut(hdc, 0, startpos + linesurl*EACHLINE, tmpstring, nLinechalen);
						linesurl++;
//						free(szMulti);
					}
					else
					{
//						nLenMulti = UTF8ToMultiByte(CP_ACP, 0, tmpstring, -1, NULL, 0, NULL, NULL);
//						szMulti = (char *)malloc(nLenMulti + 1);
//						memset(szMulti, 0, nLenMulti + 1);
//						UTF8ToMultiByte(CP_ACP, 0, tmpstring, -1, szMulti, nLenMulti, NULL, NULL);
//						szMulti[nLenMulti] = '\0';
						
						TextOut(hdc, 0, startpos + linesurl*EACHLINE, tmpstring, nLinechalen);
						urllen = 0;
//						free(szMulti);
					}
				} while(urllen > 0);

				j = 0;

			movelen = startpos + (linesurl + 1)*EACHLINE + 7;
			//Title Detail
			hBig = SelectObject(hdc, hViewFont);
			TextOut(hdc, 0, movelen, STRWMLDETAILPUSHTITLE, -1);	
			SelectObject(hdc, hBig);
			
			memset(tmpstring, 0, 16);
			titlelen = strlen(DetailTitle);

			do {
				memset(tmpstring, 0, 50);
				GetTextExtentExPoint(hdc, &DetailTitle[j], titlelen, nPushMaxWidth, &nLinechalen, NULL, NULL);
				strncpy(tmpstring, &DetailTitle[j], nLinechalen);
				titlelen -= nLinechalen;
				if(titlelen > 0)
				{
					j += nLinechalen;
					
//					nLenMulti = UTF8ToMultiByte(CP_ACP, 0, tmpstring, -1, NULL, 0, NULL, NULL);
//					szMulti = (char *)malloc(nLenMulti + 1);
//					memset(szMulti, 0, nLenMulti + 1);
//					UTF8ToMultiByte(CP_ACP, 0, tmpstring, -1, szMulti, nLenMulti, NULL, NULL);
//					szMulti[nLenMulti] = '\0';
					
					TextOut(hdc, 0, (movelen + EACHLINE + linestitle*EACHLINE), tmpstring, nLinechalen);
					linestitle++;
//					free(szMulti);
				}
				else
				{
//					nLenMulti = UTF8ToMultiByte(CP_ACP, 0, tmpstring, -1, NULL, 0, NULL, NULL);
//					szMulti = (char *)malloc(nLenMulti + 1);
//					memset(szMulti, 0, nLenMulti + 1);
//					UTF8ToMultiByte(CP_ACP, 0, tmpstring, -1, szMulti, nLenMulti, NULL, NULL);
//					szMulti[nLenMulti] = '\0';
					
					TextOut(hdc, 0, (movelen + EACHLINE + linestitle*EACHLINE), tmpstring, nLinechalen);
					titlelen = 0;
//					free(szMulti);
				}
			} while(titlelen > 0);

			lines = linesurl + linestitle + 4;
		
			if((lines * EACHLINE) < (rClient.bottom-rClient.top))
				nPushHeight = rClient.bottom-rClient.top;
			else
			{
				nPushHeight = lines * EACHLINE;
				distance = (lines * EACHLINE) - (rClient.bottom - rClient.top);
				if(!distance%EACHLINE)
					keylines = distance/EACHLINE + 1;
				else if(distance < EACHLINE)
					keylines = 1;
				else
					keylines = distance/EACHLINE;
				
				bmove = TRUE;
			}
			RedrawScrollBar(hWnd, lines + 1, movekeylines, 6);
			SetBkMode(hdc, bkmodeold);
			EndPaint(hWnd, NULL);
		}
        break;
        
    case WM_KEYDOWN:        
        switch (wParam)
		{
		case VK_UP:
			{
				if(bmove)
				{
					if(movekeylines > 0)
					{
						movekeylines--;
						startpos += EACHLINE;
						InvalidateRect(hWnd, NULL, TRUE);
						RedrawScrollBar(hWnd, lines + 1, movekeylines, 6);
					}
				}
			}
			break;
			
		case VK_DOWN:
			{
				if(bmove)
				{
					if(movekeylines <= keylines)
					{
						movekeylines++;
						startpos -= EACHLINE;
						InvalidateRect(hWnd, NULL, TRUE);
						RedrawScrollBar(hWnd, lines + 1, movekeylines, 6);
					}
				}
			}
			break;
			
		case VK_LEFT:
			break;
			
		case VK_RIGHT:
			break;
			
		case VK_F10:
			break;

		case VK_RETURN:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
			
		case VK_F5:
			return PDADefWindowProc(GetWAPFrameWindow(), wMsgCmd, wParam, lParam);
			break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);            
		}
        break;
        
    case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        bmove = FALSE;
		movekeylines = 0;
		keylines = 0;
		distance = 0;
		startpos = 28;
		free(DetailTitle);
		free(DetailUrl);
        DestroyWindow(hWnd);
		UnregisterClass("WP_PushDetail", NULL);
        break;
        
    default:            
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }

	return lResult;
}

void WML_PushCloseWindow(void)
{
	PostMessage(hPushbox, WM_CLOSE, NULL, NULL);
	PostMessage(hPushDetails, WM_CLOSE, NULL, NULL);
}




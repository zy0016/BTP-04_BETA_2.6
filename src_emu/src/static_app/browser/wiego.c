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
#include "winpda.h"
#include "string.h"
#include "malloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "plx_pdaex.h"
#include "setting.h"
#include "pubapp.h"
#include "imesys.h"
#include "dirent.h"
#include "unistd.h"

#include "wiego.h"
#include "wUipub.h"
#include "WIEBK.h"
#include "WIE.h"
#include "PWBE.h"

#define IDM_WML_BOOKMARK_SELECT   (WM_USER+1000)
#define IDM_WML_BKDELALL          (WM_USER+1001)
#define IDC_PAGE_DELSELECT        (WM_USER+1002)

extern HBROWSER  pUIBrowser;
extern HFONT hViewFont;

#define  WIE_DOCDIR_SOURCE             "/mnt/flash/wapbrowser/mydata/"
#define  WIE_IME_TXTCHANGE             1

static MENUITEMTEMPLATE WMLLFMenu[] = 
{
    {MF_STRING, WIE_IDM_LOCAL_OK, "打开", NULL},
    {MF_STRING, WIE_IDM_LOCAL_DEL, "删除", NULL},
    {MF_END, 0, NULL, NULL }
};

static const MENUTEMPLATE WMLLFMENUTEMPLATE =
{
    0,
    WMLLFMenu
};
static HWND HLocalFileList;
static HMENU hLFMenu;
static BOOL bAbleEdit = FALSE;       //在执行“跳转”和“当前连接”的时候，此
//标记用于判断是否可以对inputedit进行编辑
static HWND hEdurl, hwndCurf;
static char szUrlTmpFile[URLNAMELEN] = "", szTmp[URLNAMELEN] = "";
static char szSelFile[WIESAVEAS_MAXFNAME_LEN] = "";
static char szLFTmp[WIESAVEAS_MAXFNAME_LEN+40] = WIE_DOCDIR_SOURCE;
static int  nFileSel;
static int  ntotal = 0;
static int  nCount = 0;
static HBITMAP hsavenotactive;


static LRESULT IEUrlWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT IELFWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void LocalFile(HWND hFileListbox, BOOL add, BOOL bShowpic);
static void LocalOneTypeFile(char *pfext, HWND hFileListbox, BOOL delornot, BOOL addornot, BOOL bShow);
static BOOL On_BKSelect_Del(HWND hParent);
static LRESULT IELFDELSELECTWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static BOOL WmlIEUrl_OnCreate(HWND hWnd)
{         
    DWORD dInputStyle;
	IMEEDIT InputURL;
	char *szMulti = NULL;
	int nLenMulti = 0;
	RECT rtClient;
	
	GetClientRect(hWnd, &rtClient);

	memset(&InputURL, 0, sizeof(IMEEDIT));	 
	InputURL.hwndNotify   = (HWND)hWnd;
	InputURL.dwAscTextMax = 0;
	InputURL.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
	InputURL.dwUniTextMax = 0;
	InputURL.pszCharSet   = NULL;
	InputURL.pszImeName   = NULL;//"字母";
	InputURL.pszTitle     = NULL;
	InputURL.uMsgSetText  = 0;
	InputURL.wPageMax     = 0;
	
	SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_GOTOURL);
    SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_CANCEL);
	SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
        
    if (bAbleEdit)
    {
		dInputStyle = WS_CHILD|WS_TABSTOP|WS_VISIBLE|ES_AUTOHSCROLL|ES_TITLE;
        hEdurl =  CreateWindow(
            "IMEEDIT", 
            "", 
            dInputStyle,
            rtClient.left + CX_FITIN, 
			rtClient.top + CY_FITIN, 
			rtClient.right - rtClient.left - 2 * CX_FITIN, 
			(rtClient.bottom - rtClient.top - CY_FITIN)/3,  
            hWnd, 
            (HMENU)NULL,
            NULL, 
            (PVOID)&InputURL
            ); 
        if (hEdurl == NULL) 
            return FALSE;

        SendMessage(hEdurl, EM_LIMITTEXT, URLNAMELEN -1, NULL); 
		SendMessage(hEdurl, EM_SETTITLE, 0, (LPARAM)WML_URLADD); 
    }
    else
    {
        dInputStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_UNDERLINE|ES_MULTILINE|WS_VSCROLL|WS_BORDER;
        hEdurl =  CreateWindow(
            "EDIT", 
            "", 
            dInputStyle,
            5,
            5,
            WIE_LEDIT_WIDTH,
            WIE_MEDIT_HEIGHT+20,
            hWnd, 
            (HMENU)NULL,
            NULL, 
            NULL
            ); 
        if (hEdurl == NULL) 
            return FALSE;
        SendMessage(hEdurl, EM_LIMITTEXT, URLNAMELEN -1, NULL);                        
    }    
    
	strcpy(szUrlTmpFile, "");
	
	nLenMulti = UTF8ToMultiByte(CP_ACP, 0, szUrlTmpFile, -1, NULL, 0, NULL, NULL);
	szMulti = (char *)malloc(nLenMulti + 1);
	memset(szMulti, 0, nLenMulti + 1);
	nLenMulti = UTF8ToMultiByte(CP_ACP, 0, szUrlTmpFile, -1, szMulti, nLenMulti, NULL, NULL);
	szMulti[nLenMulti] = '\0';

    SetWindowText(hEdurl, szMulti);
    SetFocus(hEdurl);
    hwndCurf = hEdurl;

	free(szMulti);
    return TRUE;
}
HWND hIeUrl;
BOOL On_IDM_REMOTE(HWND hParent, BOOL bEdit, RECT rClient)
{
    WNDCLASS wc;
    
    bAbleEdit = bEdit;
    
    wc.style         = 0;
    wc.lpfnWndProc   = IEUrlWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLIeUrlWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
    
    hIeUrl = CreateWindow(
        "WMLIeUrlWndClass", 
        WML_URL, 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top,
        hParent, 
        NULL,
        NULL, 
        NULL
        );
    if (hIeUrl == NULL)    
    {
        UnregisterClass("WMLIeUrlWndClass", NULL);
        return FALSE;
    }  
	
    SendMessage(hEdurl, EM_SETSEL, -1, -1);
    ShowWindow(hIeUrl, SW_SHOW);
    UpdateWindow(hIeUrl);

    return TRUE;    
}

/*********************************************************************\
* Function       IEUrlWndProc
* Purpose      REMOTE URL WINDOW PROC
* Params       
* Return            
* Remarks       
**********************************************************************/
static LRESULT IEUrlWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    WORD mID;
    WORD msgBox;  
    int ncurtype;    
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
        lResult = (LRESULT)WmlIEUrl_OnCreate(hWnd);        
        break;

	case PWM_SHOWWINDOW:
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_GOTOURL);
	    SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_CANCEL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
		break;
		
    case WM_SETRBTNTEXT:
        if (strcmp((char *)lParam, (LPCSTR)WML_CANCEL) == 0)
		{
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_CANCEL);
		}
        else
		{
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, lParam);
		}
        break;

    case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);

        DestroyWindow(hWnd);
        UnregisterClass("WMLIeUrlWndClass", NULL);		
        break;
        
    case WM_COMMAND:        
        mID    = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {
        case WIE_IDM_REMOTE_CLOSE:                
            Wml_ONCancelExit(hWnd);
            break;
            
        case WIE_IDM_REMOTE_OK:
			{
				char *szMulti;
				int nLenMulti;

				strcpy(szTmp, (char *)lParam);

				nLenMulti = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szTmp, -1, NULL, 0);
				szMulti = (char *)malloc(nLenMulti + 1);
				memset(szMulti, 0, nLenMulti + 1);
				nLenMulti = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szTmp, -1, szMulti, nLenMulti);
				szMulti[nLenMulti] = '\0';

				{		
					if(strncasecmp(szMulti, "file", 4) == 0)
					{
						ncurtype = URL_LOCAL;
						SetCurentPage(szMulti, ncurtype);
						UI_Message(UM_URLGO, (unsigned long)szMulti, ncurtype);
					}
					else if(strncasecmp(szMulti, "http://", strlen("http://")) != 0)
					{
						int len;
						char *tmpurl;
						
						len = strlen(szMulti);
						tmpurl = (char *)malloc((len + 9));
						memset(tmpurl, 0, (len + 9));
						strncpy(tmpurl, "http://", 7);
						strcat(tmpurl, szMulti);	
						ncurtype = URL_REMOTE;
						SetCurentPage(tmpurl, ncurtype);
						UI_Message(UM_URLGO, (unsigned long)tmpurl, ncurtype);
						free(tmpurl);
					}
					else
					{
						ncurtype = URL_REMOTE;
						SetCurentPage(szMulti, ncurtype);
						UI_Message(UM_URLGO, (unsigned long)szMulti, ncurtype);
					}
					PostMessage(hWnd, WM_CLOSE, NULL, NULL);           
				}
			}
            break;   		
		
        default:
            break;                
        }
        break;
		
	case WM_ACTIVATE:
		if (WA_INACTIVE == LOWORD(wParam))
			hwndCurf = GetFocus();
        else
            SetFocus(hwndCurf);
        break;

    case WM_PAINT:            
		{
			int bkmodeold;
			hdc = BeginPaint(hWnd, NULL);
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc, hViewFont);
			SetBkMode(hdc, bkmodeold);
			EndPaint(hWnd, NULL);     
		}
        break;

	case WM_KEYDOWN:
		switch (wParam)
		{				
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		case VK_RETURN:
			{
				char sztmpurl[URLNAMELEN];
				
				GetWindowText(hEdurl, sztmpurl, URLNAMELEN);
				if((GetWindowTextLength(hEdurl) != 0) && (WML_DeleteSpace(sztmpurl)))
					SendMessage(hWnd, WM_COMMAND, WIE_IDM_REMOTE_OK, (LPARAM)sztmpurl);
				else
					PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFURL, NULL,
								Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
			}
			break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);			
		}
		break;

    default:            
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);                
        break;
    }
    
    return lResult;    
}

static int fcurUrlType;
static HWND hwndIELocalApp;
void On_IDM_LF(HWND hParent, RECT rClient)
{
    Create_LF_WND(hParent, rClient);
}

void Destroy_LF_WND()
{    
    DestroyWindow(hwndIELocalApp);
    UnregisterClass("WMLIeLFWndClass", NULL);
}

HMENU hDelMenu;
BOOL Create_LF_WND(HWND hParent, RECT rClient)
{
    WNDCLASS wc;
    
    wc.style         = 0;
    wc.lpfnWndProc   = IELFWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLIeLFWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE; 
    
    fcurUrlType = URL_LOCAL;
    
	hLFMenu = CreateMenu();
    hDelMenu = CreateMenu();

    InsertMenu(hLFMenu, 0, MF_BYPOSITION, WIE_IDM_LOCAL_OK, WML_OPEN);
	InsertMenu(hLFMenu, 1, MF_BYPOSITION|MF_POPUP, (DWORD)hDelMenu, WML_DELETE);    
    	
	InsertMenu(hDelMenu, 0, MF_BYPOSITION, IDM_WML_BOOKMARK_SELECT, MENU_SELECT);
	InsertMenu(hDelMenu, 1, MF_BYPOSITION, IDM_WML_BKDELALL, WML_ALL);

    hwndIELocalApp = CreateWindow(
        "WMLIeLFWndClass", 
        WML_MENU_LOCAL, 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        NULL
        );
    
    if (hwndIELocalApp == NULL ) 
    {
        UnregisterClass("WMLIeLFWndClass", NULL);
        return FALSE;
    }
   
    ShowWindow(hwndIELocalApp,SW_SHOW);
    UpdateWindow(hwndIELocalApp);    
    
    return TRUE;    
}

static void WmlIELF_OnCommand(HWND hWnd,  UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{              
    switch (LOWORD(wParam))
    {
    case WIE_IDM_FILE_LIST:
        if (HIWORD(wParam) == LBN_SELCHANGE)
        {
            nFileSel = SendMessage(HLocalFileList, LB_GETCURSEL, NULL, NULL);
            if (nFileSel ==LB_ERR)
            {                
                EnableMenuItem(hLFMenu, WIE_IDM_LOCAL_OK, MF_BYCOMMAND|MF_GRAYED);
                EnableMenuItem(hLFMenu, WIE_IDM_LOCAL_DEL, MF_BYCOMMAND|MF_GRAYED);                            
            }
            else
            {                
                EnableMenuItem(hLFMenu, WIE_IDM_LOCAL_OK, MF_BYCOMMAND|MF_ENABLED);
                EnableMenuItem(hLFMenu, WIE_IDM_LOCAL_DEL, MF_BYCOMMAND|MF_ENABLED);                            
            }            
        }
        else if (HIWORD(wParam) == LBN_DBLCLK)
        {
            SendMessage(hWnd, WM_KEYDOWN, VK_RETURN, NULL);
        }
        break;

	case IDM_WML_BOOKMARK_SELECT:
		On_BKSelect_Del(GetParent(hWnd));
		break;

	case IDM_WML_BKDELALL:
		PLXConfirmWinEx(NULL, hWnd, WML_DELALLORNOT, Notify_Request, (char *)NULL, 
				IDS_YES, IDS_NO, WIE_IDM_LOCAL_DELALLCONFIRM);
		break;

    case WIE_IDM_LOCAL_DEL:
        nFileSel = SendMessage(HLocalFileList, LB_GETCURSEL, NULL, NULL);
        if (nFileSel != LB_ERR)
        {
            SendMessage(HLocalFileList, LB_GETTEXT, (WPARAM)nFileSel, (LPARAM) szSelFile);                    
            sprintf(szLFTmp, "%s\r\n%s?", WML_MENU_2, szSelFile);            
            PLXConfirmWinEx(NULL, hWnd, szLFTmp, Notify_Request, (char *)NULL, 
				IDS_YES, IDS_NO, WIE_IDM_LOCAL_DELCONFIRM);
        }        
        break;
		
    case WIE_IDM_LOCAL_OK:
        nFileSel = SendMessage(HLocalFileList, LB_GETCURSEL, NULL, NULL);
        if (nFileSel != LB_ERR)
        {
            char szTargetUrl[WIESAVEAS_MAXFNAME_LEN+40] = "";

            SendMessage(HLocalFileList, LB_GETTEXT, (WPARAM)nFileSel, (LPARAM)szSelFile);           
            strcpy(szTargetUrl, "file://");
            strcat(szTargetUrl, WIE_DOCDIR_SOURCE);                    
            strcat(szTargetUrl, szSelFile);            
            SetCurentPage(szTargetUrl, URL_LOCAL);
			UI_Message(UM_URLGO, (unsigned long)szTargetUrl, URL_LOCAL);
            PostMessage(hWnd, WM_CLOSE, NULL, NULL);                    
        }
        break;

    case WIE_IDM_LOCAL_CLOSE:        
        PostMessage(hWnd, WM_CLOSE, NULL, NULL);
        break;

    default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }        
}

static LRESULT IELFWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    LRESULT lResult = 0;
    HDC hdc;
    
    switch (wMsgCmd)
    {
    case WM_CREATE:        
        SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_PAGESAVED);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);	
		hsavenotactive = LoadImage(NULL, WIE_IMG_NOTACTIVE, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

        HLocalFileList = CreateWindow(
            "LISTBOX", 
            "", 
            WS_VISIBLE|WS_VSCROLL|WS_CHILD|WS_CLIPCHILDREN|WS_TABSTOP|LBS_SORT|LBS_BITMAP,/*|WS_HSCROLL*/
            0, 0, 176, 150,
            hWnd, 
            (HMENU)WIE_IDM_FILE_LIST,
            NULL, 
            NULL
            );
        
        LocalFile(HLocalFileList, TRUE, TRUE);        
		SetFocus(HLocalFileList);

		if(ntotal == 0)
		{
			PDASetMenu(GetWAPFrameWindow(), NULL);
//			SendMessage(HLocalFileList, LB_RESETCONTENT, NULL, NULL);
//			SendMessage(HLocalFileList, LB_ADDSTRING, 0, (long)(LPSTR)(WML_NOSAVEPAGE));
//			SendMessage(HLocalFileList, LB_SETCURSEL, 0, 0);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SetFocus(GetWAPFrameWindow());
			ShowWindow(HLocalFileList, SW_HIDE);
			SetFocus(hWnd);
		}
		else
		{
			PDASetMenu(GetWAPFrameWindow(), hLFMenu);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_OPEN);
            SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
			ShowWindow(HLocalFileList, SW_SHOW);
		}
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_PAGESAVED);		
		if(ntotal == 0)
		{
			PDASetMenu(GetWAPFrameWindow(), NULL);
//			SendMessage(HLocalFileList, LB_RESETCONTENT, NULL, NULL);
//			SendMessage(HLocalFileList, LB_ADDSTRING, 0, (long)(LPSTR)(WML_NOSAVEPAGE));
//			SendMessage(HLocalFileList, LB_SETCURSEL, 0, 0);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SetFocus(GetWAPFrameWindow());
			ShowWindow(HLocalFileList, SW_HIDE);
			SetFocus(hWnd);
		}
		else
		{
			PDASetMenu(GetWAPFrameWindow(), hLFMenu);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_OPEN);
            SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION); 
			ShowWindow(HLocalFileList, SW_SHOW);
		}
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
		break;

	case WM_SETFOCUS:
		SetFocus(HLocalFileList);
		break;

	case WIE_IDM_LOCAL_DELALLCONFIRM:
		if(lParam == 1)
		{
			LocalOneTypeFile("wml", HLocalFileList, TRUE, FALSE, FALSE);
			LocalOneTypeFile("wmlc", HLocalFileList, TRUE, FALSE, FALSE);    
			LocalOneTypeFile("xhtml", HLocalFileList, TRUE, FALSE, FALSE);
			LocalOneTypeFile("html", HLocalFileList, TRUE, FALSE, FALSE);
			LocalOneTypeFile("wml2", HLocalFileList, TRUE, FALSE, FALSE);
			ntotal = 0;
//			SendMessage(HLocalFileList, LB_RESETCONTENT, NULL, NULL);
//			SendMessage(HLocalFileList, LB_ADDSTRING, 0, (long)(LPSTR)(WML_NOSAVEPAGE));
//			SendMessage(HLocalFileList, LB_SETCURSEL, 0, 0);
			SetFocus(GetWAPFrameWindow());
			ShowWindow(HLocalFileList, SW_HIDE);
			SetFocus(hWnd);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)""); 
		}
		break;

	case WIE_IDM_LOCAL_DELCONFIRM:
		if(lParam == 1)
			SendMessage(hWnd, WM_SELOK, NULL, NULL);
		else
			SendMessage(hWnd, WM_SELCANCEL, NULL, NULL);
		break;

    case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_RETURN:
				if(ntotal != 0)
					PostMessage(hWnd, WM_COMMAND, WIE_IDM_LOCAL_OK, NULL);
                break;

			case VK_F10:
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;

			case VK_F5:
				if(ntotal != 0)
				{
					nFileSel = SendMessage(HLocalFileList, LB_GETCURSEL, NULL, NULL);
					if (nFileSel != LB_ERR)
						return PDADefWindowProc(GetWAPFrameWindow(), wMsgCmd, wParam, lParam);
					else
						break;
				}
				else
					break;

            default:
                return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }            
        }
        break;
		
    case WM_ACTIVATE:
        if (WA_ACTIVE == LOWORD(wParam))
            SetFocus(HLocalFileList);
        break;        
        
    case WM_PAINT:        
        {
			RECT rtClient;
			int bkmodeold;

			hdc = BeginPaint(hWnd, NULL);
			GetClientRect(hWnd, &rtClient);
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			if(ntotal == 0)
			{
				DrawText(hdc, WML_NOSAVEPAGE, -1, &rtClient, DT_VCENTER|DT_HCENTER);
			}
			SetBkMode(hdc, bkmodeold);
			EndPaint(hWnd, NULL);        
		}
        break;

    case WM_SELOK:
        szLFTmp[0] = 0;
        strcpy(szLFTmp, WIE_DOCDIR_SOURCE);
        strcat(szLFTmp, szSelFile);
        remove(szLFTmp);
        PWBE_DeleteSavedSource(pUIBrowser, szLFTmp);
        nFileSel = SendMessage(HLocalFileList, LB_GETCURSEL, NULL, NULL);
        if (nFileSel != LB_ERR)
        {
            SendMessage(HLocalFileList, LB_DELETESTRING, (WPARAM)nFileSel, NULL);
            if (nFileSel > 0)
                SendMessage(HLocalFileList, LB_SETCURSEL, (WPARAM)nFileSel-1, NULL);
            else 
                SendMessage(HLocalFileList, LB_SETCURSEL, (WPARAM)nFileSel, NULL);
            nFileSel = SendMessage(HLocalFileList, LB_GETCURSEL, NULL, NULL);
            if (nFileSel == LB_ERR)
            {                
                SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");                           
            }
            else
            {                
                SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_OPEN);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);                            
            }            
        }
		ntotal--;
		if(ntotal == 0)
		{
//			SendMessage(HLocalFileList, LB_RESETCONTENT, NULL, NULL);
//			SendMessage(HLocalFileList, LB_ADDSTRING, 0, (long)(LPSTR)(WML_NOSAVEPAGE));
//			SendMessage(HLocalFileList, LB_SETCURSEL, 0, 0);
			SetFocus(GetWAPFrameWindow());
			ShowWindow(HLocalFileList, SW_HIDE);
			SetFocus(hWnd);
		}
        break;
        
    case WM_COMMAND:
        WmlIELF_OnCommand(hWnd, wMsgCmd, wParam, lParam);
        break;
        
    case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		ntotal = 0;
		DeleteObject(hsavenotactive);
		DestroyMenu(hLFMenu);
		DestroyMenu(hDelMenu);
        DestroyWindow(hWnd);
        UnregisterClass("WMLIeLFWndClass", NULL);
        break;
        
    default:        
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
       
    return lResult;
}

static void LocalOneTypeFile(char *pfext, HWND hFileListbox, BOOL delornot, BOOL addornot, BOOL bShow)
{
	char szkey[10] = "*.";
    char str[WIESAVEAS_MAXFNAME_LEN];
    int  nListID, i, fixlen;
    DIR *dp;
	struct dirent *dirp;
	BOOL bcmptrue = FALSE;
	char * cmpfix;
	int ftlen;
	
    strcat(szkey, pfext);
    if ((dp = opendir(WIE_DOCDIR_SOURCE)) == NULL)
		return;
	if ((dirp = readdir(dp)) == NULL)
	{
		closedir(dp);
		return;
    }

	do {
		if (strcmp(dirp->d_name, "..") == 0)
			break;
	} while ((dirp = readdir(dp)) != NULL);	
	
	if ((dirp = readdir(dp)) == NULL)
	{
		closedir(dp);
		return ;
	}
	
	do 
	{
		cmpfix = strstr(dirp->d_name, ".");
		if (cmpfix == NULL)
			continue;
		ftlen = strlen(cmpfix) - 1;
		fixlen = strlen(pfext);

		if(ftlen != fixlen)
			continue;
		
		for (i = 0; i < fixlen; i++)
		{
			if (cmpfix[i + 1] != pfext[i])
			{
				bcmptrue = FALSE;
				break;
			}
			else if (i == fixlen - 1)
				bcmptrue = TRUE;
		}

		if (bcmptrue)
		{
			strcpy(str, dirp->d_name);
			if(strcmp(str, "") != 0)
			{
				if (delornot)
				{
					strcpy(szLFTmp, WIE_DOCDIR_SOURCE);
					strcat(szLFTmp, str);
					remove(szLFTmp);

					PWBE_DeleteSavedSource(pUIBrowser, szLFTmp);
					memset(szLFTmp, 0, (WIESAVEAS_MAXFNAME_LEN+40));
				}
				else
				{
					nListID = SendMessage(hFileListbox, LB_ADDSTRING, NULL, (LPARAM)str);
					if(bShow)
						SendMessage(hFileListbox, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nListID), (LPARAM)hsavenotactive);
					if(addornot)	
						ntotal++;
				}
			}
		}
	} while ((dirp = readdir(dp)) != NULL);
    
    closedir(dp);    
}

static void LocalFile(HWND hFileListbox, BOOL add, BOOL bShowpic)
{
	char OldPath[_MAX_PATH];
	
    getcwd(OldPath, _MAX_PATH);
    chdir(WIE_DOCDIR_SOURCE);
    //find wml file
    LocalOneTypeFile("wml", hFileListbox, FALSE, add, bShowpic);
    //find wmlc file
    LocalOneTypeFile("wmlc", hFileListbox, FALSE, add, bShowpic);    
    LocalOneTypeFile("xhtml", hFileListbox, FALSE, add, bShowpic);
    LocalOneTypeFile("html", hFileListbox, FALSE, add, bShowpic);
	LocalOneTypeFile("wml2", hFileListbox, FALSE, add, bShowpic);
	
    chdir(OldPath);
    SendMessage(hFileListbox, LB_SETCURSEL, 0, NULL);
}

static int CanbeLocalFileName(char *szUrl)
{
    char *ndot;
    
    if (!szUrl || strlen(szUrl) ==0 || strlen(szUrl) > (9+1+4))
        return -1;
    
    ndot = strchr(szUrl, '.');    
    if (ndot)
    {
        if (strcasecmp(ndot+1,"wml") == 0 || strcasecmp(ndot+1,"wmlc") == 0 )
            return 0;
    }
    else 
    {
        if (strlen(szUrl) <= 9) 
            return 1;
    }
    
    return -1;    
}

BOOL isLocalFile(char *szUrl)
{
    char szkey[20] ;
    int isLegalName;
    int nlnum;
    
    
    isLegalName = CanbeLocalFileName(szUrl);
    if (isLegalName == -1) 
        return FALSE;
    
    strcpy(szkey,szUrl);
    nlnum = SendMessage(HLocalFileList, LB_FINDSTRINGEXACT, 0, (LPARAM)strlwr(szkey));    
    if (nlnum != LB_ERR) 
        return TRUE;

    return FALSE;
}

/*********************************************************************\
* Function     On_BKSelect_Del
* Purpose      create public config window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
HWND hbkselect;
static BOOL On_BKSelect_Del(HWND hParent)
{
	WNDCLASS wc;
	RECT rClient;

	GetClientRect(hParent, &rClient); 
    
    wc.style         = 0;
    wc.lpfnWndProc   = IELFDELSELECTWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLDELSelectWndClass";
    
	RegisterClass(&wc);        
    
    hbkselect = CreateWindow(
        "WMLDELSelectWndClass", 
        "", 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        NULL
        );
    if (hbkselect == NULL) 
    {
        return FALSE;
    }
    
    ShowWindow(hbkselect, SW_SHOW);            
    UpdateWindow(hbkselect);
    
    return TRUE;
}

static LRESULT IELFDELSELECTWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    WORD mID;
    WORD msgBox;    
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			HWND hwndList;
		
			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_SELPAGES);
			PDASetMenu(GetWAPFrameWindow(), NULL);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_DELETE); 
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);

			hwndList = CreateWindow(
				"MULTILISTBOX",
				"Item1",
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				0, 0, 176, 150,
				hWnd,
				(HMENU)IDC_PAGE_DELSELECT,
				NULL,
				NULL);
			
			LocalFile(hwndList, FALSE, FALSE);
			SendMessage(hwndList, LB_ENDINIT, 0, 0);
			SendMessage(hwndList, LB_SETCURSEL, 0, NULL);		

			SetFocus(hwndList);
		}
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_SELPAGES);
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_DELETE); 
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
		break;  
        
    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
        break;

	case WIE_IDM_LOCAL_DELSELCONFIRM:
		if(lParam == 1)
		{
			int i;
			HWND hDelList = GetDlgItem(hWnd, IDC_PAGE_DELSELECT);
			
			for (i = 0; i < nCount; i++)
			{
				if (SendMessage(hDelList, LB_GETSEL, (WPARAM)i, 0))
				{
					SendMessage(hDelList, LB_GETTEXT, (WPARAM)i, (LPARAM)szSelFile);
					szLFTmp[0] = 0;
					strcpy(szLFTmp, WIE_DOCDIR_SOURCE);
					strcat(szLFTmp, szSelFile);
					if(!remove(szLFTmp))
						PLXTipsWin(NULL, NULL, NULL, WML_DELETED, NULL,
						Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
					PWBE_DeleteSavedSource(pUIBrowser, szLFTmp);
					memset(szLFTmp, 0, (WIESAVEAS_MAXFNAME_LEN+40));
					ntotal--;
				}
			}
		}
		SendMessage(HLocalFileList, LB_RESETCONTENT, NULL, NULL);
		LocalFile(HLocalFileList, FALSE, TRUE);
		PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		break;
        
    case WM_COMMAND:        
        mID    = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {
        default:
            break;                
        }
        break;
        
    case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        
        DestroyWindow(hWnd);
		UnregisterClass("WMLDELSelectWndClass", NULL);
        break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		case VK_RETURN: //delete selected items
			{
				HWND hDelList = GetDlgItem(hWnd, IDC_PAGE_DELSELECT);
				int i, nDelnum = 0;
				BOOL selected = FALSE;
				char promptchar[40];

				nCount = SendMessage(hDelList, LB_GETCOUNT, 0, 0);
				if (nCount > 0)
				{	
					for (i = 0; i < nCount; i++)
					{
						if (SendMessage(hDelList, LB_GETSEL, (WPARAM)i, 0))
						{
							selected = TRUE;							
							nDelnum++;
						}
					}
				}

				if (!selected)
					PLXTipsWin(NULL , NULL, NULL, WML_NOPAGES, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
				else
				{
					if(nDelnum == 1)
						strcpy(promptchar, WML_DELSELECTSIGORNOT);
					else
						strcpy(promptchar, WML_DELSELECTMULORNOT);
					PLXConfirmWinEx(NULL, hWnd, promptchar, Notify_Request, (char *)NULL, 
						IDS_YES, IDS_NO, WIE_IDM_LOCAL_DELSELCONFIRM);
				}
			}			
			break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			
		}
		break;
        
    default:            
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }

	return lResult;
}

void WML_GotoCloseWindow(void)
{
	PostMessage(hIeUrl, WM_CLOSE, NULL, NULL);
	PostMessage(hbkselect, WM_CLOSE, NULL, NULL);
	PostMessage(hwndIELocalApp, WM_CLOSE, NULL, NULL);
}





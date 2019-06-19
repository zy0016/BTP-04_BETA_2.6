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
#include "sys\stat.h"

#include "wiego.h"
#include "wUipub.h"
#include "WIEBK.h"
#include "WIE.h"
#include "PWBE.h"

#define WIESAVEAS_OK             (WM_USER+2)
#define WIESAVEAS_CLOSE          (WM_USER+1)
#define WIE_IDM_SAVE_OVERWRITE   (WM_USER+101)

#define  WIESAVEAS_TEXT_X        2
#define  WIESAVEAS_TEXT_Y        0
#define  WIESAVEAS_TEXT_WIDTH    160
#define  WIESAVEAS_TEXT_HEIGHT   24

#define  WIESAVEAS_EDIT_X        2
#define  WIESAVEAS_EDIT_Y        28
#define  WIESAVEAS_EDIT_WIDTH    130
#define  WIESAVEAS_EDIT_HEIGHT   28
#define  WIESAVEAS_TAIL_X        (WIESAVEAS_EDIT_X + WIESAVEAS_EDIT_WIDTH + 1)
#define  WIESAVEAS_TAIL_Y        WIESAVEAS_EDIT_Y

#define  WIESAVEAS_DOCMAXSIZE    10*1024
#define  WIESAVEAS_IMGMAXSIZE    50*1024

static DIR *pdir;
static HWND hWndSaveAs;
static char szFileClass[WIESAVEAS_MAXFTAIL_LEN];
static char szFileName[WIESAVEAS_MAXFNAME_LEN];
static HBROWSER pSaveBrowser;
static HWND hEdFileName;

extern HFONT hViewFont;

static LRESULT WIESaveAsWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL WIE_CheckFileName(const char *FileName, unsigned int nNameLen);
BOOL CheckFileExist(const char* DIR, const char * FILENAME);

extern BOOL WML_DeleteSpace( char* s );

BOOL On_IDM_SaveAs(HWND hParent, HBROWSER pSave, RECT rClient)
{
    WNDCLASS wc;    
    
    if (pSave == NULL)
        return FALSE;
    
    pSaveBrowser = pSave;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WIESaveAsWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WIESaveAs";

    if (!RegisterClass(&wc))
        return FALSE;
    
    hWndSaveAs = CreateWindow(
        "WIESaveAs", 
        WML_MENU_SAVESOURCE,
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top,
        hParent, 
        NULL,
        NULL, 
        NULL);
    
    if (hWndSaveAs == NULL) 
    {
        UnregisterClass("WIESaveAs", NULL);
        return FALSE;
    }
	SendMessage(hEdFileName, EM_SETSEL, -1, -1);    
    ShowWindow(hWndSaveAs, SW_SHOW);            
    UpdateWindow(hWndSaveAs);
    
    return TRUE;    
}

static LRESULT WIESaveAsWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    LRESULT lResult = TRUE;
    static HWND hfocus;
    static char szTail[20];
	IMEEDIT InputEditFileName;    
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
        {    
            const char *PFILENAME;
			char path[_MAX_PATH];
			RECT rtClient;
	
			GetClientRect(hWnd, &rtClient);
			memset(path, 0, sizeof(path));
			getcwd(path, _MAX_PATH);			
			
			if (chdir(WIE_DOCDIR) == -1)
			{
				int ret = mkdir("/mnt/flash/wapbrowser/", 0);
				ret = mkdir(WIE_DOCDIR, 0);
			}
			chdir(WIE_DOCDIR);
            
            PDASetMenu(GetWAPFrameWindow(), NULL);
			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_SAVEPAGE);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_SAVES);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            
            memset(&InputEditFileName, 0, sizeof(IMEEDIT));			
			InputEditFileName.hwndNotify   = (HWND)hWnd;
			InputEditFileName.dwAscTextMax = 0;
			InputEditFileName.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
			InputEditFileName.dwUniTextMax = 0;
			InputEditFileName.pszCharSet   = NULL;
			InputEditFileName.pszImeName   = NULL;//"字母";
			InputEditFileName.pszTitle     = NULL;
			InputEditFileName.uMsgSetText  = 0;
			InputEditFileName.wPageMax     = 0;			
            hEdFileName = CreateWindow("IMEEDIT", 
                NULL, 
				WS_CHILD|WS_TABSTOP|WS_VISIBLE|ES_AUTOHSCROLL|ES_TITLE,
                rtClient.left + CX_FITIN,//WIESAVEAS_EDIT_X,
                rtClient.top + CY_FITIN,//WIESAVEAS_EDIT_Y,
                rtClient.right - rtClient.left - 2 * CX_FITIN,//WIESAVEAS_EDIT_WIDTH,// - 20,
				(rtClient.bottom - rtClient.top - CY_FITIN)/3,//WIESAVEAS_EDIT_HEIGHT,
                hWnd, 
                NULL,
                NULL, 
                (PVOID)&InputEditFileName
                );
            if (hEdFileName == NULL) 
                return FALSE;

            SendMessage(hEdFileName, EM_LIMITTEXT, WIESAVEAS_EDITLIMIT, NULL);
			SendMessage(hEdFileName, EM_SETTITLE, 0, (LPARAM)WML_DLFNAME);

            PFILENAME = PWBE_GetCurURLName(pSaveBrowser);            
            hfocus = hEdFileName;     
            
            switch (PWBE_GetSourceType(pSaveBrowser))
			{
			case MMT_WMLC:
				strcpy(szTail, ".wmlc");
				break;

			case MMT_WML1:
				strcpy(szTail, ".wml");
				break;

			case MMT_WML2:
				strcpy(szTail, ".wml2");
				break;

			case MMT_XHTMLMP:
				strcpy(szTail, ".xhtml");
				break;

			case MMT_HTML:
				strcpy(szTail, ".html");
				break;

			default:
				strcpy(szTail, ".tmp");
				break;
			}
        }
		printf("\r\n@@@@@@@@@@@@@@@@@ szTail = %s ####################\r\n", szTail);
		SetFocus(hfocus);
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_SAVEPAGE);		
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_SAVES);
		break;

	case WM_SETRBTNTEXT:
		if (strcmp((char *)lParam, (LPCSTR)WML_MENU_BACK) == 0)
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
        else
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, lParam);
        break;
        
    case WM_ACTIVATE:  
        if (WA_INACTIVE == LOWORD(wParam))
            hfocus = GetFocus();//取得焦点所在子窗口的句柄
        else
            SetFocus(hfocus);//激活后设置焦点
        break; 
        
    case WM_PAINT:
        {
            HDC hdc;  
			int bkmodeold;
			
            hdc = BeginPaint(hWnd, NULL);            
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc, hViewFont);
			SetBkMode(hdc, bkmodeold);
            EndPaint(hWnd, NULL);
        }
        break;

	case WIE_IDM_SAVE_OVERWRITE:
		if(lParam == 1)
		{
			char szFullfn[256];
			
			strcpy(szFullfn, WIE_DOCDIR);
			strcat(szFullfn, szFileName);
			
			if (PWBE_SaveSource(pSaveBrowser, szFullfn,
				WIESAVEAS_DOCMAXSIZE, WIESAVEAS_IMGMAXSIZE))
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			else
				PLXTipsWin(NULL, NULL, NULL, WML_MEMOUT,WML_SAVEFAILED,
				Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);                    
		}
		else
			return FALSE;
		break;
        
    case WM_COMMAND:
        {
            WORD mID;
            WORD msgBox;
            
            mID    = LOWORD(wParam);
            msgBox = HIWORD(wParam);
            
            switch (mID)
            {                
            case WIESAVEAS_OK:    
                {
                    char szTemp[WIESAVEAS_MAXFNAME_LEN];
					char szFullfn[256];

                    GetWindowText(hEdFileName, szFileName, WIESAVEAS_MAXFNAME_LEN);
                    
                    if (strcmp(szFileName, "") == 0)
                    {                        
                        PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFNAME, NULL, 
							Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
                        return FALSE;
                    }
                    
                    if (!WML_DeleteSpace(szFileName))
                    {
                        PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFNAME, NULL,
							Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
                        return FALSE;
                    }

                    if (!WIE_CheckFileName(szFileName, strlen(szFileName)))
                    {                        
                        PLXTipsWin(NULL, NULL, NULL, WML_DL_BADFNAME_ERR, NULL, 
							Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
                        return FALSE;
                    }
 
                    szTemp[0] = 0;
                    strcpy(szTemp, szFileName);
                    strcat(szTemp, szTail);

                    if (CheckFileExist(WIE_DOCDIR, szTemp) == TRUE)
                    {
						PLXConfirmWinEx(NULL, hWnd, WML_NAMEUSEDOVER, Notify_Request, (char *)NULL, 
							IDS_YES, IDS_NO, WIE_IDM_SAVE_OVERWRITE);
                    }
					else
					{
						strcpy(szFullfn, WIE_DOCDIR);
						strcat(szFullfn, szFileName);
						
						printf("\r\n@@@@@@@@@@@@@@@@@@@@ save source enter####################\r\n");
						printf("\r\n@@@@@@@@@@@@ szFileName = %s ###############\r\n", szFileName);
						if (PWBE_SaveSource(pSaveBrowser, szFullfn,
							WIESAVEAS_DOCMAXSIZE, WIESAVEAS_IMGMAXSIZE))
						{
							PostMessage(hWnd, WM_CLOSE, 0, 0);
						}
						else
							PLXTipsWin(NULL, NULL, NULL, WML_MEMOUT,WML_SAVEFAILED,
							Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);                    
					}
                }
                break;
                
            case WIESAVEAS_CLOSE:  
				Wml_ONCancelExit(hWnd);
                break;

            default:
                break;
            }
        }
        break;
        
    case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		
        DestroyWindow(hWnd);
        UnregisterClass("WIESaveAs", NULL);            
        break;

	case WM_KEYDOWN:
		switch (wParam)
		{				
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
			
		case VK_RETURN:
			PostMessage(hWnd, WM_COMMAND, WIESAVEAS_OK, NULL);
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

BOOL WIE_CheckFileName(const char *FILENAME, unsigned int nNameLen)
{
    unsigned int i;
    char c;

    if (strlen(FILENAME) != nNameLen)
        return FALSE;

    if (nNameLen == 0)
        return FALSE;

    if (FILENAME[0] == ' ')
        return FALSE;
    
    i = 0;
    while (i < nNameLen && FILENAME[i] != '\0')
    {
        c = FILENAME[i];
        /*if  ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || (c == ' ') || (c == '$') ||
            (c == '%') || (c == '_') || ( c == 0x1c ) ||
            (c >= 0xa0 && c <= 0xfe) || c < 0)*/
		if  ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || (c == 0x11))
            i++;
        else
            return FALSE;       
    }

    return TRUE;    
}

BOOL CheckFileExist(const char* DIR, const char * FILENAME)
{
	char szCurPath[_MAX_PATH];
	BOOL ibexist;	
	struct dirent *dirp;	
		
	getcwd(szCurPath, _MAX_PATH);
	
	if ((pdir = opendir(DIR)) == NULL)
	{
		ibexist = FALSE;
		return ibexist;
	}

	if ((dirp = readdir(pdir)) == NULL)
	{
		ibexist = FALSE;
		closedir(pdir);
		return ibexist;
	}
	do
	{
		if ((strcmp(FILENAME, dirp->d_name)) == 0)
		{
			ibexist = TRUE;
			break;
		}
		else
			ibexist = FALSE;
	} while ((dirp = readdir(pdir)) != NULL);
	
	closedir(pdir);	
	chdir(szCurPath);

	return ibexist;
}


void WML_SaveCloseWindow(void)
{
	PostMessage(hWndSaveAs, WM_CLOSE, NULL, NULL);
}

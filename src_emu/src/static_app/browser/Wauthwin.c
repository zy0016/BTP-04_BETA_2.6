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
#include "plx_pdaex.h"
#include "winpda.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "malloc.h"
#include "pubapp.h"
#include "imesys.h"
#include "wUipub.h"

static HWND hCallAuthWnd;
static UINT uCallAuthMsg;
static HWND hEditUser, hEditPwd;
static int nEditX = 15, nEditY = 55, nW = 125, nH = 25, nG = 60;
static HWND hFocus;

extern HFONT hViewFont;

#define MAX_AUTHLEN          51
#define IDM_AUTH_CANCEL      (WM_USER+102)
#define IDM_AUTH_CONFIRM     (WM_USER+101)

static BOOL WmlAuth_OnCreate(HWND hWnd)
{
	IMEEDIT InputUser, InputPws;	
	
	memset(&InputUser, 0, sizeof(IMEEDIT));
	memset(&InputPws, 0, sizeof(IMEEDIT));	
	InputUser.hwndNotify   = (HWND)hWnd;
	InputUser.dwAscTextMax = 0;
	InputUser.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
	InputUser.dwUniTextMax = 0;
	InputUser.pszCharSet   = NULL;
	InputUser.pszImeName   = NULL;//"字母";
	InputUser.pszTitle     = NULL;
	InputUser.uMsgSetText  = 0;
	InputUser.wPageMax     = 0;

	InputPws.hwndNotify   = (HWND)hWnd;
	InputPws.dwAscTextMax = 0;
	InputPws.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
	InputPws.dwUniTextMax = 0;
	InputPws.pszCharSet   = NULL;
	InputPws.pszImeName   = NULL;//"字母";
	InputPws.pszTitle     = NULL;
	InputPws.uMsgSetText  = 0;
	InputPws.wPageMax     = 0;

    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, IDM_AUTH_CANCEL, (LPARAM)STR_WAPWINDOW_BACK);
    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_AUTH_CONFIRM,1), (LPARAM)STR_WAPWINDOW_OK);
        
    hEditUser = CreateWindow(
        "IMEEDIT",
        NULL, WS_CHILD|WS_VISIBLE|ES_UNDERLINE|ES_MULTILINE|WS_TABSTOP|ES_AUTOVSCROLL|ES_NOHIDESEL|WS_VSCROLL,
        nEditX, nEditY, nW, nH,
        hWnd, (HMENU)NULL ,
        NULL, (PVOID)&InputUser);
    if (hEditUser == NULL) 
        return FALSE;
    SendMessage(hEditUser, EM_LIMITTEXT, MAX_AUTHLEN-1, 0);	
    
    hEditPwd = CreateWindow (                 
        "IMEEDIT",
        NULL, WS_CHILD|WS_VISIBLE|ES_UNDERLINE|ES_MULTILINE|WS_TABSTOP|ES_AUTOVSCROLL|ES_NOHIDESEL|ES_PASSWORD|ES_NOHIDESEL,
        nEditX, nEditY + nG, nW, nH,
        hWnd, (HMENU)NULL ,
        NULL, (PVOID)&InputUser);
    if (hEditPwd == NULL) 
        return FALSE;
    SendMessage(hEditPwd, EM_LIMITTEXT, MAX_AUTHLEN-1, 0);
    hFocus = hEditUser;
    
    return TRUE;
}
/*********************************************************************\
* Function     Jam_CallAuthenticProc 
* Purpose    要求用户输入用户名密码等
鉴权信息的窗口的窗口过程  
* Params       
* Return            
* Remarks       
**********************************************************************/
static LRESULT APP_Wap_CallAuthenticProc(HWND hWnd, UINT wMsg, WPARAM wParam,LPARAM lParam)
{
    LRESULT lResult=TRUE;
    HDC hdc;
    int nTxtX = 10, nTxtY = 30;
    
    char cAuthUser[MAX_AUTHLEN], cAuthPwd[MAX_AUTHLEN];
    
    switch (wMsg)
    {
    case WM_CREATE:
        lResult = (LRESULT)WmlAuth_OnCreate(hWnd);
        break;
        
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
            hFocus = GetFocus();
        else
            SetFocus(hFocus);
        break;
        
    case WM_SETRBTNTEXT:
        if (strcmp((char *)lParam, STR_WAPWINDOW_EXIT) == 0)
            SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);
        else
            SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, lParam);
        break;
        
    case WM_PAINT:
		{
			int bkmodeold;
        hdc = BeginPaint(hWnd, NULL);
		bkmodeold = SetBkMode(hdc, TRANSPARENT);
		SelectObject(hdc, hViewFont);
        TextOut(hdc, 5, 5, WML_AUTH_REQLABEL, -1);
        TextOut(hdc, nTxtX, nTxtY, WML_AUTH_USER, -1);
        TextOut(hdc, nTxtX, nTxtY + nG, WML_AUTH_PWS, -1);
		SetBkMode(hdc, bkmodeold);
        EndPaint(hWnd, NULL);
		}
        break;
        
    case WM_CLOSE:
        SendMessage(hCallAuthWnd, uCallAuthMsg, (WPARAM)0, (LPARAM)0);
        UnregisterClass("APP_WAP_CallAuthenticWndClass", NULL);
        DestroyWindow(hWnd);
        break;
        
    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDM_AUTH_CANCEL:
            PostMessage(hWnd, WM_CLOSE, NULL, NULL);
            break;
            
        case IDM_AUTH_CONFIRM:            
            {
                int nLen1, nLen2;
                nLen1 = GetWindowTextLength(hEditUser);
                nLen2 = GetWindowTextLength(hEditPwd);
                if (nLen1 == 0 || nLen2 == 0)
                {
                    PLXTipsWin(NULL, NULL, NULL, WML_AUTH_NOTALL, NULL, 
						Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
                    if (nLen1 == 0) 
                        SetFocus(hEditUser);
                    else 
						SetFocus(hEditPwd);
                    break;
                }
                //检查用户输入,这里只能检查每一EDIT是否输入
                GetWindowText(hEditUser, cAuthUser, MAX_AUTHLEN);
                GetWindowText(hEditPwd, cAuthPwd, MAX_AUTHLEN);
                SendMessage(hCallAuthWnd, uCallAuthMsg, 
                    (WPARAM)cAuthUser, (LPARAM) cAuthPwd);
                UnregisterClass("APP_WAP_CallAuthenticWndClass", NULL);
                DestroyWindow(hWnd);
            }
            break;

        default:
            break;
        }
        break;
        
    case WM_DESTROY:
        hCallAuthWnd = NULL;
        uCallAuthMsg = 0;
        break;        
        
    default:
        return PDADefWindowProc(hWnd, wMsg, wParam, lParam);
    }

    return lResult;    
}

/*********************************************************************\
* Function       
* Purpose    调用提示用户验证的窗口 
* Params       
* Return            
* Remarks       
**********************************************************************/
BOOL APP_Wap_CallAuthenticWnd(HWND hParent, UINT uMsg)
{
    HWND hwnd;
    WNDCLASS wc;
    
    hCallAuthWnd = hParent;
    uCallAuthMsg = uMsg;
    
    wc.style         = 0;
    wc.lpfnWndProc   = APP_Wap_CallAuthenticProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = (HINSTANCE)NULL;
    wc.hIcon         = (HICON)NULL;
    wc.hCursor       = (HCURSOR)NULL;
    wc.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = (LPCSTR)NULL;
    wc.lpszClassName = "APP_WAP_CallAuthenticWndClass";
    
    if( !RegisterClass(&wc) )
        return FALSE;
    
    hwnd = CreateWindow(
        "APP_WAP_CallAuthenticWndClass",
        WML_AUTH_CAPTION, 
        WS_BORDER |PWS_STATICBAR|WS_CAPTION, 
        PLX_WIN_POSITION, 
        hParent, NULL, NULL, NULL);
    
    if (!hwnd)
        return FALSE;

    SendMessage(hEditPwd, EM_SETSEL, -1, -1);
	SendMessage(hEditUser, EM_SETSEL, -1, -1);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);    
    
    return TRUE;
}

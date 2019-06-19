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
#include "string.h"
#include "imesys.h"
#include "me_wnd.h"


#define		PROJECTATCLASS			"Project_ATWndClass"

#define		ATCOMMANDMAXLEN			100
#define     IDC_CALLBACK_AT         WM_USER + 100

#define     ERROR2                  ML("Allocate Space Error")
#define     ERROR3                  ML("Time Out")
#define     ERROR4                  ML("Implement Error")
#define     ERROR5                  ML("Other Error")

#define     IDC_SEND				0x101
#define     IDC_BACK				0x102

static BOOL	    CreateControl(HWND hParentWnd,HWND *hATEdit,HWND *hResultEdit);
static LRESULT  ProjectATWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );


extern HWND		GetProjectModeFrameWnd();
extern int		GetSIMState();

static	HWND hProjectModeFrame;

BOOL    CallProjectATWindow()
{
    HWND hwnd;
    WNDCLASS wc;
	RECT rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = ProjectATWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;	//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = PROJECTATCLASS;
    
    if( !RegisterClass(&wc) )
    {
        return FALSE;
    }

	hProjectModeFrame = GetProjectModeFrameWnd();
	GetClientRect(hProjectModeFrame,&rClient);

    hwnd = CreateWindow(
        PROJECTATCLASS,
        ML("AT COMMAND"), 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top, 
        hProjectModeFrame, NULL, NULL, NULL);
            
    if (!hwnd)
        return FALSE;
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd); 

    return TRUE;
}

static LRESULT ProjectATWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hFocus;
	static HWND hATEdit, hResultEdit;
	static char at_command[ATCOMMANDMAXLEN+1];
	static char at_result[1000];

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			SendMessage(hProjectModeFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK,0), (LPARAM)ML("BACK"));
			SendMessage(hProjectModeFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SEND,1), (LPARAM)ML("Send")); 

			SetWindowText(hProjectModeFrame,ML("AT COMMAND"));
			CreateControl(hWnd,&hATEdit,&hResultEdit);

			hFocus = hATEdit;	
			SetFocus(hFocus);
		}
        break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;

	case PWM_SHOWWINDOW:
		SendMessage(hProjectModeFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("BACK")); 
		SendMessage(hProjectModeFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
		SendMessage(hProjectModeFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		
		SetWindowText(hProjectModeFrame,ML("AT COMMAND"));
		SetFocus(hFocus);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass(PROJECTATCLASS,NULL);
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hProjectModeFrame, PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;

        case VK_RETURN:
			{
				memset( at_command, 0x00, ATCOMMANDMAXLEN + 1 );
				GetWindowText(hATEdit,at_command,ATCOMMANDMAXLEN);
				ME_SendATCmd(hWnd,IDC_CALLBACK_AT,at_command,20000);
				SetWindowText(hResultEdit,ML("Requesting..."));
			}
            break;
			
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	case WM_COMMAND:
		switch( LOWORD( wParam ))
		{

		}
		break;

	case IDC_CALLBACK_AT:
		SetWindowText(hResultEdit,"");
		switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(at_result,sizeof(at_result));
            SetWindowText(hResultEdit,at_result);
            break;

        case ME_RS_FAILURE:
            SetWindowText(hResultEdit,ERROR4);
            break;

        case ME_RS_TIMEOUT:
            SetWindowText(hResultEdit,ERROR3);
            break;
			
        default:
            SetWindowText(hResultEdit,ERROR5);
            break;
        }
		break;

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;
}

static BOOL CreateControl(HWND hParentWnd,HWND *hATEdit,HWND *hResultEdit)
{
	int     xzero=0,yzero=0;
	RECT    rClient;
	IMEEDIT ie;

    GetClientRect(hParentWnd, &rClient);
	
	memset(&ie, 0, sizeof(IMEEDIT));
	ie.hwndNotify	= (HWND)hParentWnd;    
	ie.dwAttrib	    = 0;                
	ie.dwAscTextMax	= 0;
	ie.dwUniTextMax	= 0;
	ie.wPageMax	    = 0;        
	ie.pszCharSet	= NULL;
	ie.pszTitle	    = NULL;
	ie.pszImeName	= NULL;

    * hATEdit= CreateWindow( "IMEEDIT", "", 
        WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE,
        rClient.left,
		rClient.top,
		rClient.right-rClient.left,
		(rClient.bottom-rClient.top)/3,
        hParentWnd, (HMENU)NULL, NULL, (PVOID)&ie);
	
	SetWindowText(*hATEdit,"AT^");
	SendMessage(*hATEdit, EM_SETTITLE, 0, (LPARAM)ML("Input AT Command:"));

    if (* hATEdit == NULL)
        return FALSE;
	
    * hResultEdit = CreateWindow( "EDIT", "", 
        WS_BORDER | WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_MULTILINE | ES_UNDERLINE | ES_READONLY,
        rClient.left,
		rClient.top + (rClient.bottom-rClient.top)/3,
		rClient.right-rClient.left,
		2 * (rClient.bottom - rClient.top) / 3,
        hParentWnd, (HMENU)NULL, NULL, NULL);
	
    if (* hResultEdit == NULL)
        return FALSE;
	
	return TRUE;   
}

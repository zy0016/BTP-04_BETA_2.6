#include    "winpda.h"
#include    "plx_pdaex.h"
#include    "stdlib.h"
#include    "mullang.h"
#include    "MBPublic.h"
#include   "MB_control.h"

#define     IDC_SEND                3
#define     IDC_BACK                4
#define     IDC_EDIT                5
#define     MAX_EDITREPLY           183

#define     IDS_SEND  ML("Send")
#define     IDS_BACK  ML("Back")
#define     CAP_NETWORKSERVICE  ML("Network service")
#define     CAP_EDIT_ENTERCOMMAND   ML("Enter command:")

static HWND  hNotifyReplyWnd;
static UINT  wNotifyReplyMsg;
static const char * pReplyControlClassName = "MBReplyControlWndClass";
static HINSTANCE    hInstance;
static char       * pReplyResult;
static HWND       hReplyWnd;
static HWND  hReplyEdit;
static HWND  hFocus;
extern void CloseUSSDShowWnd();

static LRESULT ReplyControlAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

BOOL    CallReplyControlWindow(HWND hPWnd)
{
    WNDCLASS    wc;
	RECT   rClient;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = ReplyControlAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pReplyControlClassName;

    if (!RegisterClass(&wc))
        return FALSE;

	if (hPWnd != NULL) 
	{
		GetClientRect(hPWnd, &rClient);
		hReplyWnd = CreateWindow(pReplyControlClassName, CAP_NETWORKSERVICE,
			PWS_STATICBAR|WS_CAPTION|WS_VSCROLL,
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
			hPWnd, NULL, NULL, NULL);
	}
	else
	{
		hReplyWnd = CreateWindow(pReplyControlClassName, CAP_NETWORKSERVICE,
			PWS_STATICBAR|WS_CAPTION|WS_VSCROLL|PWS_NOTSHOWPI, 
			PLX_WIN_POSITION,
			hPWnd, NULL, NULL, NULL);
	}

    if (NULL == hReplyWnd)
    {
        UnregisterClass(pReplyControlClassName,NULL);
        return FALSE;
    }
	CALLWND_InsertDataByHandle(pGlobalCallListBuf, hReplyWnd);
    ShowWindow(hReplyWnd, SW_SHOW);
    UpdateWindow(hReplyWnd); 
/*
			if (hReplyEdit != NULL)
				SetFocus(hReplyEdit);
			else
				SetFocus(hReplyWnd);*/
		
    return (TRUE);
}
static LRESULT ReplyControlAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  IMEEDIT   IReply;
    int         nEnd = 0,nPos = 0;
    int         iLen;
    HWND        hCurWin = 0;
    LRESULT     lResult;
	DWORD       dwStyle;
	RECT rClient;
			
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		memset(&IReply, 0, sizeof(IMEEDIT));
		IReply.hwndNotify = hWnd;
		IReply.dwAttrib = 0;
		IReply.dwAscTextMax = 0;
		IReply.dwUniTextMax = 0;
		IReply.pszImeName ="Phone";//"ºÅÂë";
		IReply.pszCharSet = "0123456789*#";
		IReply.pszTitle	= NULL;
		IReply.uMsgSetText	= 0;

		dwStyle = WS_VISIBLE | WS_CHILD | WS_VSCROLL |  ES_TITLE | ES_MULTILINE | ES_NUMBER;
		GetClientRect(hWnd, &rClient);
        hReplyEdit = CreateWindow ("IMEEDIT", NULL, 
			dwStyle,
			rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
            hWnd,(HMENU)IDC_EDIT,NULL,(PVOID)&IReply);

        if (hReplyEdit == NULL)
            return FALSE;
		hFocus = hReplyEdit;
        SendMessage(hReplyEdit, EM_LIMITTEXT, (WPARAM)MAX_EDITREPLY, NULL);        
        SendMessage(hReplyEdit, EM_SETTITLE, 0, (LPARAM)CAP_EDIT_ENTERCOMMAND);    
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_BACK, 0), (LPARAM)IDS_BACK);
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_SEND, 1), (LPARAM)IDS_SEND);
		SetWindowText(hWnd, CAP_NETWORKSERVICE);
        break;

/*
            case WM_SETFOCUS:
                SetFocus(GetDlgItem(hWnd,IDC_EDIT));
                break;*/
        

/*
    case WM_SETRBTNTEXT:
        SendMessage (hWnd, PWM_SETBUTTONTEXT, 0, lParam);
        break;*/


    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
			SetFocus(hFocus);
		else
			hFocus = GetFocus();

/*
        if (WA_INACTIVE == LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_EDIT));

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
*/
        break;

    case WM_CLOSE: 
        DestroyWindow(hWnd);
        break;

    case WM_DESTROY :
		CALLWND_DeleteDataByHandle(pGlobalCallListBuf, hReplyWnd);
		if (pReplyResult != NULL && strlen(pReplyResult) != 0) 
			free(pReplyResult);
		pReplyResult = NULL;
		hReplyWnd = NULL;
		hReplyEdit = NULL;
		hFocus = NULL;
        UnregisterClass(pReplyControlClassName,NULL);
        break;

/*
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
            DestroyWindow(hWnd);
            break;
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;

*/
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
		case IDC_SEND:
            iLen  = GetWindowTextLength(hReplyEdit);
            if (0 == iLen)
			{
				PLXTipsWin(NULL, NULL, NULL, ML("Please enter\ncommand"), "", Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);             
				break;
			}
            pReplyResult = (char *)malloc(iLen + 1);
            if (NULL == pReplyResult)
				break;
            memset(pReplyResult,0x00,iLen + 1);
            GetWindowText(hReplyEdit,pReplyResult,iLen + 1);
			PostMessage(hNotifyReplyWnd, wNotifyReplyMsg, (WPARAM)pReplyResult, (LPARAM)iLen);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			CloseUSSDShowWnd();
			break;

        case IDC_BACK:
			PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

void SetUSSDReplyNotify(HWND hWnd, UINT wMsg)
{
	hNotifyReplyWnd = hWnd;
	wNotifyReplyMsg = wMsg;
}

void CloseUSSDReplyWnd()
{
	PostMessage(hReplyWnd, WM_CLOSE, 0,0);
}

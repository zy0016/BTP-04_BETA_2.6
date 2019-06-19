       #include "stdio.h"
#include "string.h"
#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "network.h"
#include "imesys.h"
#include "pubapp.h"
#include "me_wnd.h"

#define   IDC_ENTERCOMMANDLIST		0x101
#define   IDC_ENTERCOMMANDEDIT		0x102

#define   LIMIT_NUM		183

#define   ME_USSD_MSG		(WM_USER+101)

#define CAP_NETWORKSERVICE ML("Network service")

static LRESULT ServiceCommandProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void    ShowUSSDInfoWin_NoReply(void *info);
static LRESULT ShowUSSDInfoWin_NoReplyProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void    ShowUSSDInfoWin_Reply(void *info);
static LRESULT ShowUSSDInfoWin_ReplyProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void USSDReplyWin();
static LRESULT USSDReplyWinProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static  BOOL    USSDDealRespose(HWND hWnd,WPARAM wParam,LPARAM lParam);
static  BOOL    GetUSSDInfoAndShow(HWND hWnd, BOOL isre);

static BOOL ShowUSSDInfoWin(HWND hPWnd, USSD_INFO * ussd_info, BOOL IsResponse);
static LRESULT ShowUSSDInfoProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);

static HWND hEdit;
static HWND hList;
static HWND hUSSDInfo_NoReplyWnd;
static HWND hUSSDInfo_ReplyWnd;
static void *hUSSD;

void ServiceCommandWin()
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = ServiceCommandProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ServiceCommand";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("ServiceCommand","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	
	{
		int i; 
		RECT rListItem;
		IMEEDIT ie;
		DWORD dwStyle;

		i=SendMessage(hList,LB_GETCURSEL,0,0);

		SendMessage(hList,LB_GETITEMRECT,(WPARAM)0,(LPARAM)&rListItem); 
		memset(&ie, 0, sizeof(IMEEDIT));
		ie.hwndNotify	= (HWND)hWnd;    
		ie.dwAttrib	    = 0;                
		ie.dwAscTextMax	= 0;
		ie.dwUniTextMax	= 0;
		ie.wPageMax	    = 0;        
		ie.pszCharSet	= NULL;
//		ie.pszCharSet	= "01234567890#*";
		ie.pszTitle	    = NULL;
		dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE | CS_NOSYSCTRL;
		ie.pszImeName	= "Phone";
		
		hEdit = CreateWindow("IMEEDIT","",dwStyle,
            rListItem.left,rListItem.top,rListItem.right - rListItem.left,rListItem.bottom - rListItem.top,
            hWnd,
            (HMENU)IDC_ENTERCOMMANDEDIT,
            NULL,
            (PVOID)&ie);
		SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)LIMIT_NUM, (LPARAM)NULL); //bug fixed
		SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)ML("service command:"));
	}

	SetFocus(hWnd);
	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
	
}

static LRESULT ServiceCommandProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			GetClientRect(hWnd,&rClient);
			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP|LBS_MULTILINE,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_ENTERCOMMANDLIST,NULL,NULL);
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("service command:"));
			SendMessage(hList,LB_SETCURSEL,0,0);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hNetworkFrame,ML("network services"));

			ME_USSD_SetNotify(hWnd,ME_USSD_MSG);
			
		}
        break;
	case ME_USSD_MSG:
		
		WaitWindowStateEx(hNetworkFrame,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL);
		switch (wParam)
		{
		case ME_USSD_ABORT:
			printf("ME_USSD_ABORT\r\n");
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,(LPARAM)NULL);
			return TRUE;
			
		case ME_USSD_REFUSE:
			PLXTipsWin(NULL,NULL,0,ML("Unknown subscriber"), ML("Network service"), Notify_Failure, ML("Ok"), NULL, -1);
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,(LPARAM)NULL); //exit
			break;
			
		case ME_USSD_NOTIFY:
			{
				USSD_INFO   ussd_info;
				char strUSSDInfo[160];
				
				memset(&ussd_info,0x00,sizeof(USSD_INFO));
				if (-1 == ME_GetUSSDInfo(&ussd_info))
					return FALSE;
				
				if (ussd_info.DataLen == 0)
					return FALSE;

				if (CBS_ALPHA_UCS2 == ussd_info.Code)
				{
					WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)ussd_info.Data,
						ussd_info.DataLen/2, (LPSTR)strUSSDInfo, sizeof(strUSSDInfo), 0, 0);			
				}
				else
				{
					memcpy(strUSSDInfo, ussd_info.Data,ussd_info.DataLen);
				}		
				
				memset(ussd_info.Data,0,sizeof(ussd_info.Data));
				strncpy(ussd_info.Data,strUSSDInfo,strlen(strUSSDInfo));
				ussd_info.DataLen=strlen(strUSSDInfo);
				
				ShowUSSDInfoWin_NoReply(&ussd_info);
				return TRUE;
				
			}
			break;
			
		case ME_USSD_REQUEST:
			{
				USSD_INFO   ussd_info;
				char strUSSDInfo[160];

				memset(strUSSDInfo,0x00,sizeof(strUSSDInfo));
				memset(&ussd_info,0x00,sizeof(USSD_INFO));
				if (-1 == ME_GetUSSDInfo(&ussd_info))
					return FALSE;
				
				if (ussd_info.DataLen == 0)
					return FALSE;
				
				if (CBS_ALPHA_UCS2 == ussd_info.Code)
				{
					WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)ussd_info.Data,
						ussd_info.DataLen/2, (LPSTR)strUSSDInfo, sizeof(strUSSDInfo), 0, 0);			
				}
				else
				{
					memcpy(strUSSDInfo, ussd_info.Data,ussd_info.DataLen);
				}		
				
				memset(ussd_info.Data,0,sizeof(ussd_info.Data));
				strncpy(ussd_info.Data,strUSSDInfo,strlen(strUSSDInfo));
				ussd_info.DataLen=strlen(strUSSDInfo);
				
//				ShowUSSDInfoWin_NoReply(&ussd_info);
				ShowUSSDInfoWin_Reply(&ussd_info);

				return TRUE;
				
			}
			break;
			
		case ME_USSD_DISCONNECT:
			if(hUSSDInfo_ReplyWnd)
				PostMessage(hUSSDInfo_ReplyWnd,WM_CLOSE,0,0);
			if(hUSSDInfo_NoReplyWnd)
				PostMessage(hUSSDInfo_NoReplyWnd,WM_CLOSE,0,0);

			PLXTipsWin(NULL,NULL,0,ML("Operation terminated\nby network"), CAP_NETWORKSERVICE, Notify_Alert, ML("Ok"), NULL, -1);
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,(LPARAM)NULL);
			break;
		default:
			printf("ME response error\r\n");
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,(WPARAM)hWnd,(LPARAM)NULL);
			break;
		}
		break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_ENTERCOMMANDLIST:
			{
				switch(HIWORD(wParam))
				{
				case LBN_SETFONT:
					{
						HFONT hFont = NULL;
						GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
						return (LRESULT)hFont;
					}
				case LBN_SETFOCUS:
					SetFocus(GetDlgItem(hWnd, IDC_ENTERCOMMANDLIST));
					break;
    			default:
					break;
					
				}
				
			}
			
		case IDC_ENTERCOMMANDEDIT:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				char buffer[LIMIT_NUM];
				GetWindowText(hEdit,buffer,100);
				
			}
			break;
		default:
			break;
		}
		break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hEdit);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		SetWindowText(hNetworkFrame,ML("network services"));
		SetFocus(hEdit);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("ServiceCommand",NULL);
        break;
		
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	
			
		case VK_RETURN:
			{
				char buffer[LIMIT_NUM];
				GetWindowText(hEdit,buffer,LIMIT_NUM);
				if(strlen(buffer)==0)
				{
					PLXTipsWin(NULL,NULL,0,ML("Please enter\ncommand"),ML("Service command"),
						Notify_Alert,ML("OK"),NULL,20);
				}
				else
				{
					WaitWindowStateEx(hNetworkFrame,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL);
					hUSSD=ME_USSD_Request(buffer);
				}
				break;
			}
			break;
			
			
			
		}

	default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			
			return lResult;
	}
	return lResult;
}


static void ShowUSSDInfoWin_NoReply(void *info)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = ShowUSSDInfoWin_NoReplyProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ShowUSSDInfoWin_NoReply";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("ShowUSSDInfoWin_NoReply","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)info);

	hUSSDInfo_NoReplyWnd=hWnd;
	
	SetFocus(hWnd);
	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}
static LRESULT ShowUSSDInfoWin_NoReplyProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hTextView;
	static USSD_INFO ussd_info;
	HWND hPrevWin;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			LPCREATESTRUCT	pCreateStruct;
			

			pCreateStruct = (PCREATESTRUCT)lParam;
			
			memcpy(&ussd_info,pCreateStruct->lpCreateParams,sizeof(ussd_info));  //Get user parameter transfered from CreateWindow;

			GetClientRect(hWnd,&rClient);

			hTextView=PlxTextView(hNetworkFrame,hWnd,ussd_info.Data,ussd_info.DataLen,FALSE,NULL,NULL,0);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hNetworkFrame,ML("Network service"));
			SetFocus(hTextView);

		}
        break;
	

	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		
		
		}
		break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hTextView);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		SetWindowText(hNetworkFrame,ML("Network service"));
		SetFocus(hTextView);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("ShowUSSDInfoWin_NoReply",NULL);
		hUSSDInfo_NoReplyWnd=NULL;
        break;
		
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{

		case VK_RETURN:
			{
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				PostMessage(hWnd,WM_CLOSE,0,0);
				//"Enter command" UI should be also closeed according the SPECS:
				// "After that return to Application view (cf.4)" ----Cited from SPECS
				PostMessage(hPrevWin,WM_CLOSE,0,0);  
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
			}
			break;
		}

	default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			
			return lResult;
	}
	return lResult;
}

static void ShowUSSDInfoWin_Reply(void *info)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = ShowUSSDInfoWin_ReplyProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ShowUSSDInfoWin_Reply";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("ShowUSSDInfoWin_Reply","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)info);
	hUSSDInfo_ReplyWnd=hWnd;
	
	SetFocus(hWnd);
	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}
static LRESULT ShowUSSDInfoWin_ReplyProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hTextView;
	static USSD_INFO ussd_info;
	HWND hPrevWin;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			LPCREATESTRUCT	pCreateStruct;
			
			pCreateStruct = (PCREATESTRUCT)lParam;
			
			memcpy(&ussd_info,pCreateStruct->lpCreateParams,sizeof(ussd_info)); 

			hTextView=PlxTextView(hNetworkFrame,hWnd,ussd_info.Data,ussd_info.DataLen,FALSE,NULL,NULL,0);
			SetFocus(hTextView);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Reply"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hNetworkFrame,ML("Network service"));
			
		}
        break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
			
	case WM_SETFOCUS:
		SetFocus(hTextView);
		break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Reply"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		SetWindowText(hNetworkFrame,ML("Network service"));
		SetFocus(hEdit);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("ShowUSSDInfoWin_Reply",NULL);
		hUSSDInfo_ReplyWnd=NULL;
        break;
		
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
			PostMessage(hWnd,WM_CLOSE,0,0);
			//"Enter command" UI should be also closeed according the SPECS:
			// "After that return to Application view (cf.4)" ----Cited from SPECS
			PostMessage(hPrevWin,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
			break;	
			
		case VK_RETURN:
			{
				USSDReplyWin();
				return TRUE;
			}
			break;
		}

	default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			
			return lResult;
	}
	return lResult;
}
static void USSDReplyWin()
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = USSDReplyWinProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "USSDReplyWin";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("USSDReplyWin","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	
	SetFocus(hWnd);
	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}

static LRESULT USSDReplyWinProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hEdit;
	HWND hPrevWin;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			IMEEDIT ie;
			DWORD dwStyle;

			GetClientRect(hWnd,&rClient);
			
			memset(&ie, 0, sizeof(IMEEDIT));
			ie.hwndNotify	= (HWND)hWnd;    
			ie.dwAttrib	    = 0;                
			ie.dwAscTextMax	= 0;
			ie.dwUniTextMax	= 0;
			ie.wPageMax	    = 0;        
			//ie.pszCharSet	= NULL;
			ie.pszCharSet	= "01234567890#*";
			ie.pszTitle	    = NULL;
			dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE | CS_NOSYSCTRL;
			ie.pszImeName	= "Phone";
			
			hEdit=CreateWindow("IMEEDIT","",dwStyle,
				rClient.left,rClient.top,rClient.right-rClient.left,(rClient.bottom-rClient.top)/3,
				hWnd,NULL,NULL,(PVOID)&ie);


			SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)ML("service command:"));
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hNetworkFrame,ML("Network service"));
			
		}
        break;
		
		case WM_PAINT:
			{
				BeginPaint(hWnd,NULL);
				EndPaint(hWnd,NULL);
			}
			break;
			
		case WM_SETFOCUS:
        SetFocus(hEdit);
        break;
		
		case PWM_SHOWWINDOW:
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hNetworkFrame,ML("Network service"));
			SetFocus(hEdit);
			break;
			
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("USSDReplyWin",NULL);
		hUSSDInfo_ReplyWnd=NULL;
        break;
		
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
 		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	

		case VK_RETURN:
			{
				char buffer[LIMIT_NUM];
				memset(buffer,0,sizeof(buffer));
				GetWindowText(hEdit,buffer,LIMIT_NUM);
				if(strlen(buffer)==0)
					PLXTipsWin(NULL,NULL,0,ML("Please enter\ncommand"),ML("Network service"),Notify_Alert,ML("Ok"),NULL,20);
				else
				{
					ME_USSD_Response(hUSSD,buffer);
					hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
					PostMessage(hWnd,WM_CLOSE,0,0);
					PostMessage(hPrevWin,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM)hPrevWin, (LPARAM)0);
					WaitWindowStateEx(hNetworkFrame,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL);
				}
				
			}
			break;
		}

	default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			
			return lResult;
	}
	return lResult;
}

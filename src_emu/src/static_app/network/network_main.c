#include "stdio.h"
#include "string.h"
#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "me_wnd.h"
#include "pubapp.h"
#include "public/PreBrowHead.h"
#include "network.h"

//#define  __MODULE__		"Network Service Module:"
#include "plxdebug.h"

//#define NETWORKS_OBJDUMP

#define  IDC_CHECK		0x200
#define  IDC_SELECT		0x201
#define  IDC_EXIT        0x202

#define  IDC_CALLDIVERTS			0x301
#define  IDC_CALLBARRINGS		0x302
#define  IDC_CALLWAITING			0X303
#define  IDC_REVEALOWNNR			0x304
#define  IDC_SERVICECOMMAND		0x305
#define  IDC_NETWORKS			0x306

#define  WM_OPENING_CANCEL		0x307

#define  IDM_CHECKSTATUS		0x401
#define  IDM_SET				0x402
#define  IDM_CLEAR				0x403

#define  ET_REPEAT_LATER		100
#define  ET_REPEAT_FIRST		300

#define  IDS_CHECK		(LPARAM)ML("Check")
#define  IDS_SELECT		(LPARAM)ML("Select")
#define  IDS_EXIT		(LPARAM)ML("Exit")

#define  IDS_CHECKSTATUS    ML("Check status")
#define  IDS_SET			ML("Set")
#define  IDS_CLEAR			ML("Clear")

#define  IDS_NETWORK_CLASSNAME   "Network Main"


//Call waiting ME callback message
#define  ME_MSG_CALLWAITING			(WM_USER+103)
#define  ME_MSG_GETCLIRSTATUS		(WM_USER+104)
#define  TIMER_ID					(WM_USER+105)

#define  REQUEST_CALLWAITING_SET			0x0
#define  REQUEST_CALLWAITING_CLEAR			0x1
#define  REQUEST_CALLWAITING_CHECKSTATUS	0x2

typedef struct tagCALLWAITINFOS 
{
	CALLWAIT_INFO waitinfo[3];
} CALLWAITINFOS, *PCALLWAITINFOS;

static HWND hNetworkFrame = NULL;
static HWND hNetworkMain = NULL;
static HWND hNetworkInit = NULL;
static HINSTANCE    hNetworkInstance=NULL;
static int  nCurFocus;

static unsigned int  RepeatFlag = 0;
static WORD wKeyCode;

static BOOL Network_WaitSMSInit = FALSE;		//network 是否还是需要等待短信初始
static BOOL Network_WaitABInit = FALSE;			//network 是否还是需要等待联系人初始

static int  iReveal;

static LRESULT NetworkMainProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CreateSettingListControl(HWND hParentWnd,HWND *hCallDivert,HWND *hCallBarring,HWND *hCallWaiting,HWND *hRevealNum,
									 HWND *hServiceCommand,HWND *hNetwork);
static void DoVScroll(HWND hWnd,WPARAM wParam);
static void InitVScrollBar(HWND hWnd,int iItemNum);
static void ShowCallWaitingStatus(void *info);
static LRESULT ShowCallWaitingStatusProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void NetWork_OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);

int  ReadFromNetWorkFile(void);
BOOL WriteToNetWorkFile(int i);

BOOL NetworkInit(void);
void SetABInitOK(BOOL b);

extern BOOL SMS_IsInitOK(void);

extern void CallDivertWin(BOOL);
extern void CallBarringWin();
extern void RevealOwnNumWin();
extern void ServiceCommandWin();
extern void NetworksWin();
extern int  GetSIMState();
extern int  GetCurrentAltWin();
extern void SetShortCut(BOOL shortcut2);

DWORD Network_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	
    switch (nCode)
    {
    case APP_INIT :
        hNetworkInstance = pInstance;
		GetCurrentAltWin();	//when init the app init the window of GetCurrentAltWin
        break;

	case APP_GETOPTION:
		switch(wParam)
		{
		case AS_APPWND:
			dwRet = (DWORD)hNetworkInstance;
			break;
		}
		break;
		
    case APP_ACTIVE :
		if ( NetworkInit() )
		{
			if(IsWindow(hNetworkFrame))		// the network app exist.
			{
				SetShortCut(FALSE);
				ShowWindow(hNetworkFrame, SW_SHOW);
				ShowOwnedPopups(hNetworkFrame, SW_SHOW);
				UpdateWindow(hNetworkFrame);
			}
			else	// create a new network frame.
			{

				wc.style         = 0; 
				wc.lpfnWndProc   = NetworkMainProc;
				wc.cbClsExtra    = 0;
				wc.cbWndExtra    = 0;
				wc.hInstance     = NULL;
				wc.hIcon         = NULL;
				wc.hCursor       = NULL;
				wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
				wc.lpszMenuName  = NULL;
				wc.lpszClassName = IDS_NETWORK_CLASSNAME;

				if(!RegisterClass(&wc))
					return FALSE;

#ifdef NETWORKS_OBJDUMP			
				StartObjectDebug(); //debug
#endif

				hNetworkFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR /*|WS_VSCROLL*/);
				GetClientRect(hNetworkFrame,&rClient);
		
				hNetworkMain = CreateWindow(
					IDS_NETWORK_CLASSNAME,
					"",
					WS_VISIBLE | WS_CHILD |WS_VSCROLL,
					rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
					hNetworkFrame,
					NULL,
					NULL,
					NULL
					);

				if ( lParam == 1 )
				{
					CallDivertWin(TRUE);
					break;
				}

				ShowWindow(hNetworkFrame, SW_SHOW); 
				UpdateWindow(hNetworkFrame);  
				SetFocus(hNetworkMain); 
			}	
		}
        break;
		
    case APP_INACTIVE :
		ShowOwnedPopups(hNetworkFrame, SW_HIDE);
        ShowWindow(hNetworkFrame,SW_HIDE); 
        break;
		
    default :
        break;
    }
    return dwRet;
}

static LRESULT NetworkMainProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hCallDivert,hCallBarring,hCallWaiting,hRevealNum,hServiceCommand,hNetwork;
	HWND hWndFocus;
	static HWND hFocus;
	static HMENU hMenu;
	static int request;
	static int callwaiting_state;
	static int i;
		   int status;


    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			int  iItemNum=6;

			SendMessage(hNetworkFrame,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_EXIT,0),(LPARAM)IDS_EXIT);
			SendMessage(hNetworkFrame,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_CHECK,1),(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hNetworkFrame,ML("Network services"));
			CreateSettingListControl(hWnd,&hCallDivert,&hCallBarring,&hCallWaiting,&hRevealNum,&hServiceCommand,&hNetwork);
			InitVScrollBar(hWnd,iItemNum);
			
			hMenu=CreateMenu();
			AppendMenu(hMenu,MF_ENABLED, IDM_CHECKSTATUS, IDS_CHECKSTATUS);
			AppendMenu(hMenu,MF_ENABLED, IDM_SET, IDS_SET);
			AppendMenu(hMenu,MF_ENABLED, IDM_CLEAR, IDS_CLEAR);
			PDASetMenu(hNetworkFrame,hMenu);

			status = ReadFromNetWorkFile();

			SendMessage(hRevealNum,SSBM_SETCURSEL,(WPARAM)status,(LPARAM)0);
			
			hFocus = hCallDivert;
		}
        break;

	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,IDS_EXIT);
		SetWindowText(hNetworkFrame,ML("Network services"));
		if(hFocus==hCallWaiting)
		{
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,IDS_CHECK);
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
			
			PDASetMenu(hNetworkFrame,hMenu);
		}
		else
		{
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		}
		SetFocus(hFocus);//SetFocus(hWnd);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd); 

#ifdef NETWORKS_OBJDUMP
 		EndObjectDebug();  //Debug
#endif
        break;

	case WM_OPENING_CANCEL:
		DestroyWindow(hNetworkFrame);
		break;

    case WM_DESTROY :
        UnregisterClass(IDS_NETWORK_CLASSNAME,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hNetworkInstance );
        break;

	case WM_VSCROLL:
		DoVScroll(hWnd,wParam);
		break;

	case WM_REVEALOWNNR_CHANGED:
		SendMessage(hRevealNum,SSBM_SETCURSEL,wParam,(LPARAM)0);
		break;

	case WM_TIMER:
        NetWork_OnTimeProcess(hWnd, wParam, hFocus);
        break;

	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
		break;

    case WM_KEYUP:
		RepeatFlag = 0;
        switch(LOWORD(wParam))
        {
        case VK_UP:
        case VK_DOWN:
			KillTimer(hWnd, TIMER_ID);
            break;
        default:
            break;
        }        
        break;

	case WM_KEYDOWN:
		if (RepeatFlag > 0 && wKeyCode != LOWORD(wParam))
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
		}
		
		wKeyCode = LOWORD(wParam);
		RepeatFlag++;

		switch(LOWORD(wParam))
		{
		case VK_F10:
			{
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			}
			break;	
		case VK_F5:
			{
				if(hFocus==hCallDivert)
					CallDivertWin(FALSE);
				else if(hFocus==hCallBarring)
					CallBarringWin();
				else if(hFocus==hCallWaiting)
				{
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Network services"),Notify_Alert,ML("Ok"),NULL,20);
						return FALSE;
					}

					PDASetMenu(hNetworkFrame,hMenu);
					
					SendMessage(hNetworkFrame,WM_KEYDOWN,wParam,lParam); 
					break;	
				}
				else if(hFocus==hRevealNum)
				{
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Network services"),Notify_Alert,ML("Ok"),NULL,20);
						return FALSE;
					}
					
					iReveal = SendMessage(hRevealNum, SSBM_GETCURSEL, 0, 0);
					RevealOwnNumWin(&iReveal);

				}
				else if(hFocus==hServiceCommand)
				{
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Service Command"),Notify_Alert,ML("Ok"),NULL,20);
						return FALSE;
					}

					ServiceCommandWin();
				}
				else if(hFocus==hNetwork)
				{
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Network services"),Notify_Alert,ML("Ok"),NULL,20);
						return FALSE;
					}
					NetworksWin();
				}
			}
			break;

        case VK_RETURN:
			{
				if(hFocus==hCallWaiting)
				{
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Network services"),Notify_Alert,ML("Ok"),NULL,20);
						return FALSE;
					}
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_CHECKSTATUS,0),(LPARAM)0);
				}
			}
            break;
			
		case VK_DOWN:
			{
				hWndFocus= GetFocus();
				
				while(GetParent(hWndFocus) != hWnd)
				{
					hWndFocus = GetParent(hWndFocus);
				}
				
				hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, FALSE);

				SetFocus(hWndFocus);
				hFocus=hWndFocus;

				if(hWndFocus==hCallWaiting)
				{
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,IDS_CHECK);
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");

					PDASetMenu(hNetworkFrame,hMenu);
				}
				else
				{
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
				}

				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)NULL);

				if ( 1 == RepeatFlag )
				{
					SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				}
			}
			break;
			
		case VK_UP:
			{
				hWndFocus = GetFocus();
				
				while(GetParent(hWndFocus) != hWnd)
				{
					hWndFocus = GetParent(hWndFocus);
				}
				
				hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, TRUE);
				SetFocus(hWndFocus);
				hFocus=hWndFocus;

				if(hWndFocus==hCallWaiting)
				{
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,IDS_CHECK);
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
				}
				else
				{
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
				}

				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, (LPARAM)NULL);

				if ( 1 == RepeatFlag )
				{
					SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				}
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;        

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
		case IDC_REVEALOWNNR:
			{
				if(HIWORD( wParam ) == SSBN_CHANGE)
				{
					iReveal = SendMessage(hRevealNum,SSBM_GETCURSEL,0,0);
					WriteToNetWorkFile(iReveal);
				}
			}
			break;

		//The Menu valid only if the current item is "Call Waiting"
		case IDM_CHECKSTATUS:
			{
				if(ME_GetCallWaitingStatus(hWnd,ME_MSG_CALLWAITING,CLASS_ALL)>0)
				{
					WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL);
					request=REQUEST_CALLWAITING_CHECKSTATUS;
				}
				else 
					return FALSE;
			}
			break;
		case IDM_SET:
			{
				if(ME_SetCallWaitingStatus(hWnd,ME_MSG_CALLWAITING,TRUE,CLASS_ALL)>0)
				{
					WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL);
					request=REQUEST_CALLWAITING_SET;
				}
				else
					return FALSE;
				
			}

			break;
		case IDM_CLEAR:
			{
				if(ME_SetCallWaitingStatus(hWnd,ME_MSG_CALLWAITING,FALSE,CLASS_ALL)>0)
				{
					WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL);
					request=REQUEST_CALLWAITING_CLEAR;
				}
				else
					return FALSE;

			}
			break;

        }
        break;

	case ME_MSG_CALLWAITING:	
		
		if(ME_RS_SUCCESS==wParam)
		{
			if(request==REQUEST_CALLWAITING_CHECKSTATUS)	//CHECK 3 KIND 
			{
				CALLWAITINFOS status;

				
				ME_GetResult(&status.waitinfo,3 * sizeof(CALLWAIT_INFO));
				WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL);
				ME_GetResult(&status,sizeof(BOOL));

				if( status.waitinfo[0].Enable || status.waitinfo[1].Enable || status.waitinfo[2].Enable )
					ShowCallWaitingStatus(&status);
				else
					PLXTipsWin(NULL,NULL,0,ML("Call waiting\nnot active"),ML("Network service"),Notify_Info,
					ML("OK"),NULL,0);
					
			}
			else if(request==REQUEST_CALLWAITING_SET)
			{
				WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL);
				PLXTipsWin(NULL,NULL,0,ML("Call waiting\nset"),ML("Network service"),Notify_Success,ML("Ok"),NULL,20);
				break;
			}
			else if(request==REQUEST_CALLWAITING_CLEAR)
			{
				WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL);
				PLXTipsWin(NULL,NULL,0,ML("Call waiting\nCleared"),ML("Network service"),Notify_Success,ML("Ok"),NULL,20);
				break;	
			}
		}
		else
		{
			if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
				WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL);
		}
		break;

	case ME_MSG_GETLASTERROR:
		{
			ME_EXERROR LastError;
			char ErrorInfo[50];
			
			//KillTimer(hWnd,ID_REQUEST_TIMER);
			WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
				NULL,NULL);	
			if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
			{
				GetErrorString(LastError,ErrorInfo);
				PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,0);
			}
		}
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;

}

static BOOL CreateSettingListControl(HWND hParentWnd,HWND *hCallDivert,HWND *hCallBarring,HWND *hCallWaiting,
									 HWND *hRevealNum,HWND *hServiceCommand,HWND *hNetwork)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;
    GetClientRect(hParentWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hCallDivert = CreateWindow( "SPINBOXEX", ML("Call diverts"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_CALLDIVERTS, NULL, NULL);

    if (* hCallDivert == NULL)
        return FALSE;

    * hCallBarring = CreateWindow( "SPINBOXEX", ML("Call barrings"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_CALLBARRINGS, NULL, NULL);

    if (* hCallBarring == NULL)
        return FALSE;
    * hCallWaiting = CreateWindow( "SPINBOXEX", ML("Call waiting"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_CALLWAITING, NULL, NULL);

    if (* hCallWaiting == NULL)
        return FALSE;

     * hRevealNum = CreateWindow( "SPINBOXEX", ML("Reveal own nr"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*3, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_REVEALOWNNR, NULL, NULL);

    if (* hRevealNum == NULL)
        return FALSE;

	SendMessage(* hRevealNum,SSBM_ADDSTRING,0,(LPARAM)ML("Decided by network"));
	SendMessage(* hRevealNum,SSBM_ADDSTRING,0,(LPARAM)ML("Yes"));
	SendMessage(* hRevealNum,SSBM_ADDSTRING,0,(LPARAM)ML("No"));
	SendMessage(* hRevealNum,SSBM_SETCURSEL,0,0);
    
     
    * hServiceCommand = CreateWindow( "SPINBOXEX", ML("Service command"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*4, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_SERVICECOMMAND, NULL, NULL);

    if (* hServiceCommand == NULL)
        return FALSE;
    
    * hNetwork = CreateWindow( "SPINBOXEX", ML("Networks"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*5, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_NETWORKS, NULL, NULL);

    if (* hNetwork == NULL)
        return FALSE;

	return TRUE;
    
}

static void InitVScrollBar(HWND hWnd,int iItemNum)
{
    SCROLLINFO   vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (iItemNum-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nCurFocus =0;
	return;
}

static void DoVScroll(HWND hWnd,WPARAM wParam)
{
	int  nY;
    RECT rcClient;
    SCROLLINFO      vsi;

    switch(wParam)
    {
    case SB_LINEDOWN:
		memset(&vsi, 0, sizeof(SCROLLINFO));
		
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus++;
		
        if(nCurFocus > vsi.nMax)	
        {
            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);	
            UpdateWindow(hWnd);
            nCurFocus = 0;
			vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
	
        if(((int)(vsi.nPos + vsi.nPage - 1) <= nCurFocus) && nCurFocus != vsi.nMax)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,-nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
		break;
		
    case SB_LINEUP:
		
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
		
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus--;
		
        if(nCurFocus < vsi.nMin)	
        {
            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
			
            UpdateWindow(hWnd);
			nCurFocus = vsi.nMax;
		    vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;	
        }
		
        if((int)vsi.nPos == nCurFocus && nCurFocus != vsi.nMin)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   
            break;	
        }
		break;

    case SB_PAGEDOWN:
        break;

    case SB_PAGEUP:
        break;

    default:
        break;	
    }	

}

HWND GetNetworkFrameWnd()
{
	return hNetworkFrame;
}
/*******************************************************************************/
/*						Call Waiting check status							   */
/*******************************************************************************/
static void ShowCallWaitingStatus(void *info)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = ShowCallWaitingStatusProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ShowCallWaitingStatus";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();
	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("ShowCallWaitingStatus","",WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)info);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);

}

static LRESULT ShowCallWaitingStatusProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND  hNetworkFrame;
	static HWND hFormView;
	static CALLWAITINFOS ActiveCallWaiting;

    lResult = (LRESULT)TRUE;
	hNetworkFrame=GetNetworkFrameWnd();

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;

			//Create Form viewr
			LISTBUF ListData;

			memcpy(&ActiveCallWaiting,pCreateStruct->lpCreateParams,sizeof(CALLWAITINFOS));
			GetClientRect(hNetworkFrame,&rClient);
			PREBROW_InitListBuf(&ListData);
			
			if(ActiveCallWaiting.waitinfo[0].Enable)
				PREBROW_AddData(&ListData,ML("Service:"),ML("Voice"));

			if(ActiveCallWaiting.waitinfo[1].Enable)
				PREBROW_AddData(&ListData,ML("Service:"),ML("Data"));

			if(ActiveCallWaiting.waitinfo[2].Enable)
				PREBROW_AddData(&ListData,ML("Service:"),ML("Fax"));

			hFormView=CreateWindow(WC_FORMVIEW,NULL,WS_VISIBLE|WS_CHILD|WS_VSCROLL,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,NULL,NULL,(PVOID)&ListData);

			if(!hFormView)
				return FALSE;

			SetFocus(hFormView);
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hNetworkFrame,ML("Call waiting active"));
		}
        break;

	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;
		
    case WM_SETFOCUS:
        //SetFocus(hList);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		SetWindowText(hNetworkFrame,ML("Call waiting active"));
		SetFocus(hFormView);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("ShowCallWaitingStatus",NULL);
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_RETURN:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
			break;
			
		default:
			break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;

}

static void NetWork_OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus)
{
	switch(wParam)
	{
	case TIMER_ID:
		if (1 == RepeatFlag)
		{
			KillTimer(hWnd, TIMER_ID);
			SetTimer(hWnd, TIMER_ID, ET_REPEAT_LATER, NULL);
		}
		keybd_event(wKeyCode, 0, 0, 0);
		break;
	default:
		KillTimer(hWnd, wParam);
		break;
	}
}

BOOL WriteToNetWorkFile(int i)
{
	FILE * fp;
	
	if ( ( fp = fopen("/mnt/flash/calllogs/divert.dat","w+b") ) == NULL )
	{
		fp = fopen("/mnt/flash/calllogs/divert.dat","w+b");
		if ( fp == NULL)
			return FALSE;
	}

	fwrite(&i,sizeof(int),1,fp);
	fclose(fp);
	return TRUE;
}

int  ReadFromNetWorkFile(void)
{
	FILE * fp;
	int i;

	if ( ( fp = fopen("/mnt/flash/calllogs/divert.dat","rb") ) == NULL )
	{
		fp = fopen("/mnt/flash/calllogs/divert.dat","rb");

		if ( fp == NULL)
			return 0;
	}
	
	fread(&i,sizeof(int),1,fp);
	fclose(fp);
	return i;
}


BOOL NetworkInit(void)
{
	MSG msg;
	
    if( GetSIMState() )
    {	
		if ( ( SMS_IsInitOK() == FALSE ) || !Network_WaitABInit )
			WaitWin(NULL,TRUE,ML("Opening..."),NULL,NULL,ML("Cancel"),0);  

        while (GetMessage(&msg, NULL, 0, 0))
        {	
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            
            if( msg.message == WM_KEYDOWN && msg.wParam == VK_F10 )
            {
                    WaitWin(NULL,FALSE,ML("Opening..."),NULL,NULL,ML("Cancel"),0);  
                    
                    return FALSE;
            }
            else
            {
                if( SMS_IsInitOK() && Network_WaitABInit )
                {
                    WaitWin(NULL,FALSE,ML("Opening..."),NULL,NULL,ML("Cancel"),0);  
                    
                    return TRUE;
                }
            }
        }
        return FALSE;
    }
    else
        return TRUE;
}

void SetABInitOK(BOOL b)
{
	Network_WaitABInit = b;
}

HWND GetMainWnd(void)
{
	return hNetworkMain;
}

HINSTANCE GetFrameWnd(void)
{
	return hNetworkInstance;
}



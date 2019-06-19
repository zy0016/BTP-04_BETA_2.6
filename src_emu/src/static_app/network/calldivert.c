#include "stdio.h"
#include "string.h"
#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "pubapp.h"
#include "network.h"
#include "imesys.h"
#include "me_wnd.h"
#include "pubapp.h"
#include "public/PreBrowHead.h"
#include "ab/PhonebookExt.h"
#include "callapp/MBPublic.h"
#include "recipient.h"


#define   IDC_BACK		0x101
#define   IDC_CHECK		0x102

#define   IDM_CHECKSTATUS		0x201
#define   IDM_SET				0x202
#define   IDM_SETFORSERVICE		0x203
#define   IDM_CLEAR				0x204
#define   IDM_VOICECALLS		0x205
#define   IDM_DATACALLS			0x206
#define   IDM_FAXCALLS			0x207
#define   WM_CLEARALL			0x208

#define   IDC_ALLCALLS			0x301
#define   IDC_IFBUSY			0x302
#define   IDC_IFUNANSWERED		0x303
#define   IDC_IFNOTREACHABLE	0x304
#define   IDC_CLEARALL			0x305

#define   ME_MSG_CLAERCHECK		0x306

#define   IDC_DELAYTIME			0x401

#define   IDC_DELAYTIME_LIST	0x501

//the folloing ID used by DivertToNumber_NotInIfUnanswered_Proc's Form Editor
#define   IDC_TONUMBER_LIST		0x601
#define   IDC_TONUMBER_EDIT		0x602

#define   IDC_TONUMBER_LIST2    0x603
#define   IDC_TONUMBER_SPINBOX	0x604


#define   IDS_BACK			ML("Back")
#define   IDS_CHECK			ML("Check")

#define   IDS_CHECKSTATUS	ML("Check status")
#define   IDS_SET			ML("Set")
#define   IDS_SETFORSERVICE	ML("Set for service")
#define	  IDS_CLEAR			ML("Clear")
#define   IDS_VOICECALLS	ML("Voice calls")
#define   IDS_DATACALLS		ML("Data calls")
#define   IDS_FAXCALLS		ML("Fax calls")

#define   DIVERT_LEFT_ICON		"/rom/public/arrow_left.ico"
#define   DIVERT_RIGHT_ICON		"/rom/public/arrow_right.ico"

#define   ME_MSG_CALL_DIVERT_CHECK		(WM_USER+101)
#define   ME_MSG_CALL_DIVERT_SET		(WM_USER+102)
#define   ME_MSG_DIVERT_IFUNANSWERED	(WM_USER+103)
#define   ME_MSG_CALL_DIVERT			(WM_USER+104)
#define   WM_SPINBOX_CHANGE				(WM_USER+105)
#define   ME_MSG_TONUMBER_IFUNANSWERED	(WM_USER+106)
#define   ME_MSG_CLEARALL				(WM_USER+107)
#define   ME_MSG_CLEAR					(WM_USER+108)
#define   TIMER_ID					    (WM_USER+109)

#define   WM_GET_CONTACTPHONENUM		(WM_USER+120)


#define   ID_REQUEST_TIMER		0x100

#define   ME_TIMEOUT			20

#define   ET_REPEAT_LATER		100
#define   ET_REPEAT_FIRST		300

typedef enum {DIVERT_ALLCALLS,DIVERT_IFBUSY,DIVERT_IFUNANSWERED,DIVERT_IFNOTREACHABLE}DIVERT_TYPE;
typedef enum {VOICECALL,DATACALL,FAXCALL,DIVERTALLTYPE}SERVICE_TYPE;

typedef struct tag_DIVERTCHECKINFO
{
	DIVERT_TYPE divert_type;
	CALLFWD_INFO calldivert_info[3];
}DIVERTCHECK_INFO;

typedef struct tag_DivertSetInfo
{
	DIVERT_TYPE  divert_type;
	SERVICE_TYPE service_type;
	char phone_num[ME_PHONENUM_LEN];
//	int DelayTime;
}DIVERT_SET_INFO;

static int  nCurFocus;

static HWND hEdit;

static unsigned int  RepeatFlag = 0;
static WORD wKeyCode;

static DIVERTCHECK_INFO DivertCheckInfo;
static BOOL bTimeOut;
DIVERT_SET_INFO SetInfo;
static CALL_FWD CallFwd;
static int state;

static BOOL clear_checkstatus[3];

static BOOL shortcut = FALSE;
static const char * cSecond = "seconds";

static LRESULT CallDivertWinProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL CallDivert_CreateSettingListControl(HWND hParentWnd,HWND *hAllCalls,HWND *hIfBusy,HWND *hIfUnanswered,
									 HWND *hIfNotReachable,HWND *hClearAll);
static void InitVScrollBar (HWND hWnd,int iItemNum);
static void DoVScroll(HWND hWnd,WPARAM wParam);
static void ShowCallDivertCheckStatus(DIVERTCHECK_INFO *Info);
static LRESULT ShowCallDivertCheckStatusProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void CallDivert_Set(DIVERT_SET_INFO *pSetInfo);
static LRESULT CallDivert_SetWinProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void ChangeDelayTimeWin(DIVERT_SET_INFO *info);
static LRESULT ChangeDelayTimeProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void DelaySelectionList(int i);
static LRESULT DelaySelectionListProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void DivertToNumber_NotInIfUnanswered(DIVERT_SET_INFO *pSetInfo);
static LRESULT DivertToNumber_NotInIfUnanswered_Proc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void DivertToNumber_IfUnanswered(DIVERT_SET_INFO *pSetInfo);
static LRESULT DivertToNumber_IfUnanswered_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static void CallDivert_OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);

extern void WaitWin(HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption,PCSTR szOk,PCSTR szCancel, UINT rMsg);
extern BOOL GetVoiceMailBox(char *PhoneNumber,int iPhoneNumberLen);
extern HWND GetMainWnd(void);
extern HINSTANCE GetFrameWnd(void);

void CallDivertWin(BOOL bShortCut)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	
	wc.style         = 0; 
	wc.lpfnWndProc   = CallDivertWinProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "CallDivert";

	shortcut = bShortCut;
	
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("CallDivert","",WS_VISIBLE | WS_CHILD |WS_VSCROLL,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
	
}

static LRESULT CallDivertWinProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hAllCalls,hIfBusy,hIfUnanswered,hIfNotReachable,hClearAll;
	HWND hWndFocus;
	static HWND hFocus;
	static HMENU hMenu,hSubMenu;
	HWND hNetworkFrame;
//	static int check_state;

	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			int  iItemNum=5;
			
			SendMessage(hNetworkFrame,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BACK,0),(LPARAM)IDS_BACK);
			SendMessage(hNetworkFrame,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_CHECK,1),(LPARAM)IDS_CHECK);
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
			SetWindowText(hNetworkFrame,ML("Call diverts"));
			
			CallDivert_CreateSettingListControl(hWnd,&hAllCalls,&hIfBusy,&hIfUnanswered,&hIfNotReachable,&hClearAll);
			InitVScrollBar(hWnd,iItemNum);
			
			hMenu=CreateMenu();
			hSubMenu=CreateMenu();
			
			InsertMenu(hMenu,0,MF_BYPOSITION,IDM_CHECKSTATUS,(LPCSTR)IDS_CHECKSTATUS); 
			InsertMenu(hMenu,1,MF_BYPOSITION,IDM_SET,(LPCSTR)IDS_SET);
			InsertMenu(hMenu,2,MF_BYPOSITION|MF_POPUP,(DWORD)hSubMenu,(LPCSTR)IDS_SETFORSERVICE);
			InsertMenu(hSubMenu,0,MF_BYPOSITION,IDM_VOICECALLS,(LPCSTR)IDS_VOICECALLS);
			InsertMenu(hSubMenu,1,MF_BYPOSITION,IDM_DATACALLS,(LPCSTR)IDS_DATACALLS);
			InsertMenu(hSubMenu,2,MF_BYPOSITION,IDM_FAXCALLS,(LPCSTR)IDS_FAXCALLS);
			InsertMenu(hMenu,3,MF_BYPOSITION,IDM_CLEAR,IDS_CLEAR);

			PDASetMenu(hNetworkFrame,hMenu);
			
			hFocus = hAllCalls;
			
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
		
	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
		break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDS_BACK);
		SetWindowText(hNetworkFrame,ML("Call diverts"));

		if(hFocus==hClearAll)
		{
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		}
		else
		{
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_CHECK);
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
		}
		PDASetMenu(hNetworkFrame,hMenu);
		SetFocus(hFocus);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("CallDivert",NULL);

		if ( shortcut )
		{
//			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM)GetMainWnd, 0);
			// add after frame modifyed
//			SendMessage(hNetworkFrame, WM_CLOSE, 0,0);
			DestroyWindow(GetMainWnd());
			DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)GetFrameWnd() );
		}

        break;
		
	case WM_VSCROLL:
		DoVScroll(hWnd,wParam);
		break;

	case WM_CLEARALL:
		if (lParam == 1)
		{
			//CALL_FWD CallFwd;
			CallFwd.bEnable=FALSE;
			CallFwd.Class=CLASS_ALL;
			CallFwd.Reason=FWD_ALL;
			
			if(ME_SetCallForwardStatus(hWnd,ME_MSG_CLEARALL,&CallFwd)<0)
				return FALSE;
			
			bTimeOut=FALSE;
			SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
			WaitWin(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL,(UINT)0);
		}
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
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			
			break;	
		case VK_F5:
			{
				if(!GetSIMState())
				{
					PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Call diverts"),Notify_Alert,ML("Ok"),NULL,WAITTIMEOUT);
					return FALSE;
				}

				if(hFocus!=hClearAll)
				{
					SendMessage(hNetworkFrame,WM_KEYDOWN,wParam,lParam); 
					break;
				}
				else	//--// send command for "clear all"
				{
					PLXConfirmWinEx(hNetworkFrame,hWnd,ML("Clear all diverts?"),Notify_Request,ML("Call diverts"),
						ML("Yes"),ML("No"),WM_CLEARALL);
/*
					{
						//CALL_FWD CallFwd;
						CallFwd.bEnable=FALSE;
						CallFwd.Class=CLASS_ALL;
						CallFwd.Reason=FWD_ALL;

						if(ME_SetCallForwardStatus(hWnd,ME_MSG_CLEARALL,&CallFwd)<0)
							return FALSE;
						
						bTimeOut=FALSE;
						SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
						WaitWin(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL,(UINT)0);
					}

					else
						return FALSE;
*/				}	
			}
			break;

        case VK_RETURN:
			{
				if(!GetSIMState())
				{
					PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Call diverts"),Notify_Alert,ML("Ok"),NULL,WAITTIMEOUT);
					return FALSE;
				}
				
				if(hFocus != hClearAll)
					SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_CHECKSTATUS,0),0);
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
				
				if(hFocus==hClearAll)
				{
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
				}
				else
				{
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_CHECK);
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
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
				
				if(hFocus==hClearAll)
				{
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
				}
				else
				{
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDS_CHECK);
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
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
			break;
		}
		break; 

		case ME_MSG_CLAERCHECK:
			if(bTimeOut)  //if the timer be killed by KillTimer,ignore the ME_MSG_CALL_DIVERT_CHECK message
				break;
			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{
					int i;
					
					ME_GetResult(&DivertCheckInfo.calldivert_info,3 * sizeof(CALLFWD_INFO));
					
					for ( i = 0; i != 3; i++ )
					{
						if (DivertCheckInfo.calldivert_info[i].Enable)
							clear_checkstatus[i] = TRUE;
					}
					
					CallFwd.bEnable = FALSE;

					if ( clear_checkstatus[0] && clear_checkstatus[1] && clear_checkstatus[2] )
					{
						CallFwd.Class = CLASS_ALL;
						state = 2;
					}
					else if (clear_checkstatus[0])
					{
						CallFwd.Class = CLASS_VOICE;
						if (clear_checkstatus[1] || clear_checkstatus[2])
							state = 0;
						else
							state = 2;
					}
					else if (clear_checkstatus[1])
					{
						CallFwd.Class = CLASS_DATA;
						if (clear_checkstatus[2])
							state = 1;
						else
							state = 2;
					}
					else if (clear_checkstatus[2])
					{
						CallFwd.Class = CLASS_FAX;
						state = 2;
					}
					else
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
						PLXTipsWin(NULL,NULL,0,ML("Call divert\ncleared"),ML("Call diverts"),Notify_Success,ML("Ok"),NULL,WAITTIMEOUT);
						return FALSE;
					}
					
					if(ME_SetCallForwardStatus(hWnd,ME_MSG_CLEAR,&CallFwd)<0)
						return FALSE;
					
				}
				break;
				
			default: 
				printf("clear Check divert status failure\r\n");
				if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
				{
					KillTimer(hWnd,ID_REQUEST_TIMER);
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
				}
				
				break;
			}
			break;


		case ME_MSG_CLEAR:		// the return msg of "clear"			
			{
				if(bTimeOut)  
					break;
				
				switch(wParam) 
				{
				case ME_RS_SUCCESS:
					
					if(CallFwd.Class==CLASS_VOICE || CallFwd.Class == CLASS_ALL)
					{
						if(CallFwd.Reason==FWD_UNCDTIONAL)
						{
							SetForward_Uncdtional(FALSE);
							DlmNotify(PS_SETCALLDIVERT, ICON_CANCEL);  //xjdan fixed
						}
						else if(CallFwd.Reason==FWD_BUSY)
							SetForward_Busy(FALSE);
						else if(CallFwd.Reason==FWD_NOREACH)
							SetForward_NoReach(FALSE);
						else if(CallFwd.Reason==FWD_NOREPLY)
							SetForward_NoReply(FALSE);
					}
					
					if(state==0)
					{
						if (clear_checkstatus[1])
						{
							CallFwd.Class=CLASS_DATA;
							state = 1;
						}
						else if (clear_checkstatus[2])
						{
							CallFwd.Class=CLASS_FAX;
							state = 2;
						}
						
						if(ME_SetCallForwardStatus(hWnd,ME_MSG_CLEAR,&CallFwd)<0)
							return FALSE;
					}
					else if(state==1)
					{
						if (clear_checkstatus[2])
						{
							CallFwd.Class=CLASS_FAX;
							state = 2;
						}
						
						if(ME_SetCallForwardStatus(hWnd,ME_MSG_CLEAR,&CallFwd)<0)
							return FALSE;
						state=2;
					}
					else if(state==2)
					{
						{
							clear_checkstatus[0] = FALSE;
							clear_checkstatus[1] = FALSE;
							clear_checkstatus[2] = FALSE;
						}
					
						WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);

						PLXTipsWin(NULL,NULL,0,ML("Call divert\ncleared"),ML("Call diverts"),Notify_Success,ML("Ok"),NULL,WAITTIMEOUT);
						KillTimer(hWnd,ID_REQUEST_TIMER);
					}
					
					break;

				default:
					printf("Clear the divert failure\r\n");
					if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
					{
						WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
						KillTimer(hWnd,ID_REQUEST_TIMER);
					}
					break;
				}
			}
			break;
			
		case ME_MSG_CLEARALL:		//the return msg of "clear all"
			{
				if(bTimeOut)  
					break;

				SetForward_All(FALSE);

				switch(wParam) 
				{
				case ME_RS_SUCCESS:
					{
						WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
						PLXTipsWin(NULL,NULL,0,ML("Call divert\ncleared"),ML("Network service"),Notify_Success,ML("Ok"),NULL,WAITTIMEOUT);
						KillTimer(hWnd,ID_REQUEST_TIMER);
						DlmNotify(PS_SETCALLDIVERT, ICON_CANCEL);
					}
					break;
				default:
					printf("Clear All the divert failure\r\n");
					if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
					{
						WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
//						PLXTipsWin(NULL,NULL,0,ML("Call divert\ncleared"),ML("Network service"),Notify_Info,ML("Ok"),NULL,WAITTIMEOUT);
						KillTimer(hWnd,ID_REQUEST_TIMER);
					}
					break;
				}
			}
		break;
		
		case ME_MSG_CALL_DIVERT_CHECK:		//--// check status msg need 3 AT to 1 AT
			if(bTimeOut)  //if the timer be killed by KillTimer,ignore the ME_MSG_CALL_DIVERT_CHECK message
				break;
			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{
					int len;
					
					KillTimer(hWnd,ID_REQUEST_TIMER);
					len=ME_GetResult(&DivertCheckInfo.calldivert_info,3 * sizeof(CALLFWD_INFO));
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					if(!DivertCheckInfo.calldivert_info[0].Enable && !DivertCheckInfo.calldivert_info[1].Enable 
						&&!DivertCheckInfo.calldivert_info[2].Enable)
						PLXTipsWin(NULL,NULL,0,ML("Divert not active"),ML("Network service"),Notify_Info,ML("Ok"),NULL,WAITTIMEOUT);
					else
					{
						ShowCallDivertCheckStatus(&DivertCheckInfo);
					}
				break;
					
				}
			break;
			
			default: 
				printf("Check divert status failure\r\n");
				if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
				{
					KillTimer(hWnd,ID_REQUEST_TIMER);
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
				}
				
				break;
			}
			break;

		case ME_MSG_GETLASTERROR:
			{
				ME_EXERROR LastError;
				char ErrorInfo[50];
				if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
				{
					KillTimer(hWnd,ID_REQUEST_TIMER);
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					GetErrorString(LastError,ErrorInfo);
					PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
				}
			}
			break;
		
		case WM_COMMAND:
			switch( LOWORD( wParam ))
			{
			case IDM_CHECKSTATUS:			//--// check differ option
				{
					if(hFocus==hAllCalls)
					{
						DivertCheckInfo.divert_type=DIVERT_ALLCALLS;
//						MsgOut ("SETTING OUT: We begin GetFWD hwnd[%x] msg[%d]", hWnd, ME_MSG_CALL_DIVERT_CHECK);
						if(ME_GetCallForwardStatus(hWnd,ME_MSG_CALL_DIVERT_CHECK,FWD_UNCDTIONAL,CLASS_ALL)<0)
							return FALSE;
					}
					else if(hFocus==hIfBusy)
					{
						DivertCheckInfo.divert_type=DIVERT_IFBUSY;
						if(ME_GetCallForwardStatus(hWnd,ME_MSG_CALL_DIVERT_CHECK,FWD_BUSY,CLASS_ALL)<0)
							return FALSE;
					}
					else if(hFocus==hIfUnanswered)
					{
						DivertCheckInfo.divert_type=DIVERT_IFUNANSWERED;
						if(ME_GetCallForwardStatus(hWnd,ME_MSG_CALL_DIVERT_CHECK,FWD_NOREPLY,CLASS_ALL)<0)
							return FALSE;
					}
					else if(hFocus==hIfNotReachable)
					{
						DivertCheckInfo.divert_type=DIVERT_IFNOTREACHABLE;
						if(ME_GetCallForwardStatus(hWnd,ME_MSG_CALL_DIVERT_CHECK,FWD_NOREACH,CLASS_ALL)<0)
							return FALSE;
					}
					else  //If in "Clear All" Item. Do not send Check divert status 
					{
						return FALSE;
					}
					bTimeOut=FALSE;
					SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
					WaitWin(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					
				}
				break;

			case IDM_SET:		//--// set 
				{
					if(hFocus==hAllCalls)
					{
						SetInfo.divert_type=DIVERT_ALLCALLS;
						SetInfo.service_type=DIVERTALLTYPE;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfBusy)
					{
						SetInfo.divert_type=DIVERT_IFBUSY;
						SetInfo.service_type=DIVERTALLTYPE;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfUnanswered)
					{
						
						SetInfo.divert_type=DIVERT_IFUNANSWERED;
						SetInfo.service_type=DIVERTALLTYPE;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfNotReachable)
					{
						SetInfo.divert_type=DIVERT_IFNOTREACHABLE;
						SetInfo.service_type=DIVERTALLTYPE;
						CallDivert_Set(&SetInfo);
					}
											

				}
				break;

//---//	Set for service :
			//voice 
			case IDM_VOICECALLS:
				{
					if(hFocus==hAllCalls)
					{
						SetInfo.divert_type=DIVERT_ALLCALLS;
						SetInfo.service_type=VOICECALL;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfBusy)
					{
						SetInfo.divert_type=DIVERT_IFBUSY;
						SetInfo.service_type=VOICECALL;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfUnanswered)
					{
						
						SetInfo.divert_type=DIVERT_IFUNANSWERED;
						SetInfo.service_type=VOICECALL;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfNotReachable)
					{
						SetInfo.divert_type=DIVERT_IFNOTREACHABLE;
						SetInfo.service_type=VOICECALL;
						CallDivert_Set(&SetInfo);
					}
											
				}
				break;
			//data
			case IDM_DATACALLS:
				{
					if(hFocus==hAllCalls)
					{
						SetInfo.divert_type=DIVERT_ALLCALLS;
						SetInfo.service_type=DATACALL;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfBusy)
					{
						SetInfo.divert_type=DIVERT_IFBUSY;
						SetInfo.service_type=DATACALL;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfUnanswered)
					{
						
						SetInfo.divert_type=DIVERT_IFUNANSWERED;
						SetInfo.service_type=DATACALL;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfNotReachable)
					{
						SetInfo.divert_type=DIVERT_IFNOTREACHABLE;
						SetInfo.service_type=DATACALL;
						CallDivert_Set(&SetInfo);
					}
					
				}
				break;
			//fax
			case IDM_FAXCALLS:
				{
					if(hFocus==hAllCalls)
					{
						SetInfo.divert_type=DIVERT_ALLCALLS;
						SetInfo.service_type=FAXCALL;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfBusy)
					{
						SetInfo.divert_type=DIVERT_IFBUSY;
						SetInfo.service_type=FAXCALL;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfUnanswered)
					{
						SetInfo.divert_type=DIVERT_IFUNANSWERED;
						SetInfo.service_type=FAXCALL;
						CallDivert_Set(&SetInfo);
					}
					else if(hFocus==hIfNotReachable)
					{
						SetInfo.divert_type=DIVERT_IFNOTREACHABLE;
						SetInfo.service_type=FAXCALL;
						CallDivert_Set(&SetInfo);
					}
					
				}
				break;
//--//clear option
			case IDM_CLEAR:
				{
					//CALL_FWD CallFwd;
					
					if(hFocus==hAllCalls)
						CallFwd.Reason=FWD_UNCDTIONAL;
					else if(hFocus==hIfBusy)
						CallFwd.Reason=FWD_BUSY;
					else if(hFocus==hIfUnanswered)
						CallFwd.Reason=FWD_NOREPLY;
					else if(hFocus==hIfNotReachable)
						CallFwd.Reason=FWD_NOREACH;
					
					if (ME_GetCallForwardStatus(hWnd,ME_MSG_CLAERCHECK,CallFwd.Reason,CLASS_ALL)<0)
						return FALSE;

					bTimeOut=FALSE;
					SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
					state=0;
					WaitWin(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					
				}
				break;	
			}
			break; //End of WM_COMMAND
			
		case WM_TIMER:
			switch(wParam) 
			{
			case ID_REQUEST_TIMER:
				KillTimer(hWnd,ID_REQUEST_TIMER);
				WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);	
//				PLXTipsWin(NULL,NULL,0,ML("Other error"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
				bTimeOut=TRUE;
				break;
				
			case TIMER_ID:
				CallDivert_OnTimeProcess(hWnd, wParam, hFocus);
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

static BOOL CallDivert_CreateSettingListControl(HWND hParentWnd,HWND *hAllCalls,HWND *hIfBusy,HWND *hIfUnanswered,
									 HWND *hIfNotReachable,HWND *hClearAll)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;
    GetClientRect(hParentWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hAllCalls = CreateWindow( "SPINBOXEX", ML("All calls"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_ALLCALLS, NULL, NULL);

    if (* hAllCalls == NULL)
        return FALSE;

    * hIfBusy = CreateWindow( "SPINBOXEX", ML("If busy"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_IFBUSY, NULL, NULL);

    if (* hIfBusy == NULL)
        return FALSE;
    * hIfUnanswered = CreateWindow( "SPINBOXEX", ML("If unanswered"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_IFUNANSWERED, NULL, NULL);

    if (* hIfUnanswered == NULL)
        return FALSE;

     * hIfNotReachable= CreateWindow( "SPINBOXEX", ML("If not reachable"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*3, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_IFNOTREACHABLE, NULL, NULL);

    if (* hIfNotReachable == NULL)
        return FALSE;

     
    * hClearAll = CreateWindow( "SPINBOXEX", ML("Clear all"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*4, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_CLEARALL, NULL, NULL);

    if (* hClearAll == NULL)
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

/**********************************************************************************
*
*					Check Status Menu Item Process
*
***********************************************************************************/
static void ShowCallDivertCheckStatus(DIVERTCHECK_INFO *Info)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	
	wc.style         = 0; 
	wc.lpfnWndProc   = ShowCallDivertCheckStatusProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ShowCallDivertCheckStatus";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("ShowCallDivertCheckStatus","",WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)Info);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}

static LRESULT ShowCallDivertCheckStatusProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static DIVERTCHECK_INFO info;
	static HWND hFormView[3];
	static int  iIndex = 0;
	static int  index = 0;
	static int  iInfo = 0;
	static int  iPage = 0;
	static SIZE lsize,rsize;

	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			//Create Form viewr
			LISTBUF ListData[3];
			PCREATESTRUCT pCreateData;
			char strDivertType[50];
			char bs[3][100];
			char PhoneNumber[3][120];
			char DelayTime[3][30];
			int len;
			RECT rClient;

			GetClientRect(hNetworkFrame,&rClient);
			
			pCreateData=(PCREATESTRUCT)lParam;
			memcpy(&info,pCreateData->lpCreateParams,sizeof(info));

			if(info.divert_type==DIVERT_ALLCALLS)
				strcpy(strDivertType,ML("All calls"));
			else if(info.divert_type==DIVERT_IFBUSY)
				strcpy(strDivertType,ML("If busy"));
			else if(info.divert_type==DIVERT_IFUNANSWERED)
				strcpy(strDivertType,ML("If unanswered"));
			else if(info.divert_type==DIVERT_IFNOTREACHABLE)
				strcpy(strDivertType,ML("If not reachable"));
			
			iIndex = 0;
			iPage = 0;
			for ( index = 0; index != 3; index++)
			{
				if ( info.calldivert_info[index].Enable )
					iIndex++;
			}

			for ( index = 0; index != iIndex; index++ )
			{
				PREBROW_InitListBuf(&ListData[index]);

				PREBROW_AddData(&ListData[index],ML("Type:"),strDivertType);
			}
			
			memset(bs,0,sizeof(bs));
			memset(PhoneNumber,0,sizeof(PhoneNumber));
			memset(DelayTime,0,sizeof(DelayTime));

			len = 0;
			iInfo = 0;
			if(info.calldivert_info[0].Enable)
			{
				strcat(bs[iInfo],ML("Voice"));
				strcat(bs[iInfo],"\n");
				strcat(PhoneNumber[iInfo],info.calldivert_info[0].PhoneNum);
				strcat(PhoneNumber[iInfo],"\n");
				/*len=*/
				sprintf(DelayTime[iInfo],"%d %s\n",info.calldivert_info[0].Time,cSecond);
				iInfo++;
			}

			if(info.calldivert_info[1].Enable)
			{
				strcat(bs[iInfo],ML("All data"));
				strcat(bs[iInfo],"\n");
				strcat(PhoneNumber[iInfo],info.calldivert_info[1].PhoneNum);
				strcat(PhoneNumber[iInfo],"\n");
				/*len+=*/
				sprintf(DelayTime[iInfo],"%d %s\n",info.calldivert_info[1].Time,cSecond);
				iInfo++;
			}

			if(info.calldivert_info[2].Enable)
			{
				strcat(bs[iInfo],ML("Fax"));
				strcat(bs[iInfo],"\n");
				strcat(PhoneNumber[iInfo],info.calldivert_info[2].PhoneNum);
				strcat(PhoneNumber[iInfo],"\n");
				sprintf(DelayTime[iInfo],"%d %s\n",info.calldivert_info[2].Time,cSecond);
			}


			for ( index = 0; index != iIndex; index++ )
			{
				PREBROW_AddData(&ListData[index],ML("Service:"),bs[index]);
				PREBROW_AddData(&ListData[index],ML("Number:"),PhoneNumber[index]);
			}

			if(info.divert_type==DIVERT_IFUNANSWERED)
			{
				for ( index = 0; index != iIndex; index++ )
				{
					PREBROW_AddData(&ListData[index],ML("Delay:"),DelayTime[index]);
				}
			}

			FormView_Register();

			for ( index = 0; index != iIndex; index++ )
			{
				hFormView[index] = CreateWindow(WC_FORMVIEW,NULL,WS_VISIBLE|WS_CHILD|WS_VSCROLL,
					rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
					hWnd,NULL,NULL,(PVOID)&ListData[index]);
			}

			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hNetworkFrame,ML("Call divert active"));

			SetFocus(hFormView[0]);

			ShowWindow(hFormView[0], SW_SHOW);
			for ( index = 1; index != iIndex; index++ )
			{
				ShowWindow(hFormView[index],SW_HIDE);
			}
		}
        break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			if ( iIndex == 1 )
			{
				SendMessage(hNetworkFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
				SendMessage(hNetworkFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
			}
			else if ( iPage == 0 )
			{
				SendMessage(hNetworkFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
				SendMessage(hNetworkFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)DIVERT_RIGHT_ICON);
			}
			else if ( iPage == ( iIndex - 1 ) ) 
			{
				SendMessage(hNetworkFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)DIVERT_LEFT_ICON);
				SendMessage(hNetworkFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
			}
			else
			{
				SendMessage(hNetworkFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)DIVERT_LEFT_ICON);
				SendMessage(hNetworkFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)DIVERT_RIGHT_ICON);
			}
			EndPaint(hWnd,NULL);
		}
		break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		SetWindowText(hNetworkFrame,ML("Call divert active"));
		SetFocus(hFormView[iPage]);
		break;
		
	case WM_SETFOCUS:
		SetFocus(hFormView[iPage]);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("ShowCallDivertCheckStatus",NULL);
        break;
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_LEFT:
			printf("switch formview item\r\n");
			if ( iPage != 0 )
			{
				SetFocus(hFormView[iPage - 1]);
				ShowWindow(hFormView[iPage],SW_HIDE);
				iPage--;
				ShowWindow(hFormView[iPage],SW_SHOW);
			}

			break;

		case VK_RIGHT:
			printf("switch formview item\r\n");
			if ( iPage != ( iIndex - 1 ) )
			{
				SetFocus(hFormView[iPage + 1]);
				ShowWindow(hFormView[iPage],SW_HIDE);
				iPage++;
				ShowWindow(hFormView[iPage],SW_SHOW);
			}
			break;
	
		case VK_UP:
			printf("receive the VK_UP message!\r\n");
			break;

		case VK_DOWN:
			printf("receive the VK_DOWN message!\r\n");
			break;

        case VK_RETURN:
			{
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}
            break;
		
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break; 

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;
}

/////////////////////////////////////////////////////////////////////////////////////
//////////////////////////  Menu Item "Set" 's process   ////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
#define  IDC_SET_LIST	0x401


static void CallDivert_Set(DIVERT_SET_INFO *pSetInfo)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	

	wc.style         = 0; 
	wc.lpfnWndProc   = CallDivert_SetWinProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "CallDivert_Set";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("CallDivert_Set","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)pSetInfo);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);

}

//"Set" Menu Item process
static LRESULT CallDivert_SetWinProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hList;
	static HBITMAP hRadioOn,hRadioOff;
	static DIVERT_SET_INFO SetInfo;
	static int SetState;
	static BOOL bTimeOut;
	static CALL_FWD CallFwd;
	HWND hPrevWin;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			HDC hDC=NULL;
			PCREATESTRUCT pCreateStruct;
			pCreateStruct=(PCREATESTRUCT)lParam;
			memcpy(&SetInfo,pCreateStruct->lpCreateParams,sizeof(DIVERT_SET_INFO));
			GetClientRect(hWnd,&rClient);

			hDC=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_on.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_off.bmp",NULL,NULL);
			ReleaseDC(hWnd,hDC);

			hList=CreateWindow("LISTBOX","",WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_SET_LIST,NULL,NULL);
			if(hList==0)
				return 0;
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("To voice mailbox"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("To number"));
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)0),(LPARAM)hRadioOff);
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)1),(LPARAM)hRadioOff);
			SendMessage(hList,LB_SETCURSEL,0,0);
		
			SendMessage(hNetworkFrame,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(IDC_BACK,0),(LPARAM)IDS_BACK);
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hNetworkFrame,ML("Set call divert"));
			
		}
        break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		
		break;
    case WM_SETFOCUS:
        SetFocus(hList);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDS_BACK);
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hNetworkFrame,ML("Set call divert"));
		SetFocus(hList);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("CallDivert_Set",NULL);
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;
		
	case ME_MSG_CALL_DIVERT_SET:  
		{
			if(bTimeOut==TRUE)
				return FALSE;

			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{
					printf("\r\nGet the Success return message of ME function! \r\n");
					if(CallFwd.Class==CLASS_VOICE || CallFwd.Class==CLASS_ALL)
					{
						if(CallFwd.Reason==FWD_UNCDTIONAL)
						{
							SetForward_Uncdtional(TRUE);
							DlmNotify(PS_SETCALLDIVERT,ICON_SET); //xjdan fixed
						}
						else if(CallFwd.Reason==FWD_BUSY)
							SetForward_Busy(TRUE);
						else if(CallFwd.Reason==FWD_NOREACH)
							SetForward_NoReach(TRUE);
					}

					KillTimer(hWnd,ID_REQUEST_TIMER);
					printf("\r\nSuccess Set Call Forward (All Type)!!\r\n");
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);

					PLXTipsWin(NULL,NULL,0,ML("Call divert set"),ML("Network service"),Notify_Success,ML("Ok"),NULL,WAITTIMEOUT);

					PostMessage(hWnd,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
				}
				break;
			
			default:
				if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0) //Check which error it is
				{
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					KillTimer(hWnd,ID_REQUEST_TIMER);				
					PostMessage(hWnd,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
				}
			}
		}
		break;

	case ME_MSG_GETLASTERROR:  //Get detailed voice mailbox error
			{
				ME_EXERROR LastError;
				char ErrorInfo[50];
				if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
				{
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					KillTimer(hWnd,ID_REQUEST_TIMER);
					GetErrorString(LastError,ErrorInfo);
					PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
				}
				
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
			}
			break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	

		case VK_F5:
			{
				int iCurrentItem;
				iCurrentItem=SendMessage(hList,LB_GETCURSEL,0,0);
				if(iCurrentItem==0)  //To Voice mailbox: 1---show failure info 2--ChangeDelayTime 3---Send Divert Request
				{
					char PhoneNum[50];

					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Call divert"),Notify_Alert,ML("Ok"),NULL,WAITTIMEOUT);
						return FALSE;
					}

					if(!GetVoiceMailBox(PhoneNum,sizeof(PhoneNum))) //SPECS Page13
					{
						PLXTipsWin(NULL,NULL,0,ML("Voice mailbox\nnumber not set"),ML("Set call divert"),Notify_Failure,ML("OK"),NULL,WAITTIMEOUT);
						return TRUE;
					}

					if(strlen(PhoneNum)==0)
					{
						PLXTipsWin(NULL,NULL,0,ML("Voice mailbox\nnumber not set"),ML("Set call divert"),Notify_Failure,ML("OK"),NULL,WAITTIMEOUT);
						return TRUE;
					}

					switch(SetInfo.divert_type)
					{
						//For other case,it is unnecessary copy the voice mailbox to SetInfo.
						//Because in other case it is another window to send request
					case DIVERT_IFUNANSWERED: 
						memset(SetInfo.phone_num,0,sizeof(SetInfo.phone_num));
						strncpy(SetInfo.phone_num,PhoneNum,min(strlen(PhoneNum),sizeof(SetInfo.phone_num)-1));
						ChangeDelayTimeWin(&SetInfo);   //Special case,return directly.
						return TRUE;

					case DIVERT_ALLCALLS: 
						CallFwd.Reason =FWD_UNCDTIONAL;
						break;

					case DIVERT_IFBUSY:
						CallFwd.Reason=FWD_BUSY;
						break;
						
					case DIVERT_IFNOTREACHABLE:
						CallFwd.Reason=FWD_NOREACH;
						break;
					}

					CallFwd.bEnable=TRUE;
					memset(CallFwd.PhoneNum,0,sizeof(CallFwd.PhoneNum));
					strncpy(CallFwd.PhoneNum,PhoneNum,min(strlen(PhoneNum),sizeof(CallFwd.PhoneNum)-1));
					
					switch(SetInfo.service_type)
					{
					case DIVERTALLTYPE:
						CallFwd.Class=CLASS_ALL;
						break;
					case VOICECALL:
						CallFwd.Class=CLASS_VOICE;
						break;
					case DATACALL:
						CallFwd.Class=CLASS_DATA;
						break;
					case FAXCALL:
						CallFwd.Class=CLASS_FAX;
						break;
					}
					
					if(ME_SetCallForwardStatus(hWnd,ME_MSG_CALL_DIVERT_SET,&CallFwd)<0)
						return FALSE;
					bTimeOut=FALSE;
					SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
					WaitWin(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
				}  //End of iCurrentItem==0
				else if(iCurrentItem==1)  // To Number
				{
					if(SetInfo.divert_type==DIVERT_IFUNANSWERED)
					{
 						DivertToNumber_IfUnanswered(&SetInfo);
					}
					else
					{
 						DivertToNumber_NotInIfUnanswered(&SetInfo);
					}
				}
				
			} //End VK_F5
			break;
      
	
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		} //End WM_KEYDOWN
		break;        
		
		case WM_TIMER:
			KillTimer(hWnd,ID_REQUEST_TIMER);
			bTimeOut=TRUE;
			WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
//			PLXTipsWin(NULL,NULL,0,ML("Other error"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);		
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);

			
		default :
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
    }
    return lResult;
}


//SendDivertRequest(){}

///////////////////////////////////////////////////////////////////////////////////////////
/////////////////		Set---->To Voice mailbox----->Change Delay Time  //////////////////
///////////////////////////////////////////////////////////////////////////////////////////
static void ChangeDelayTimeWin(DIVERT_SET_INFO *info)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = ChangeDelayTimeProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ChangeDelayTime";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("ChangeDelayTime","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)info);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}

static LRESULT ChangeDelayTimeProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hDelayTime;
	static DIVERT_SET_INFO SetInfo;
	static CALL_FWD CallFwd;
	static int state;
	static int delaytime;
	static BOOL bTimeOut;
	HWND hPrevWin;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			int    iControlH,iControlW;
			PCREATESTRUCT pCreateData;

			pCreateData=(PCREATESTRUCT)lParam;
			memcpy(&SetInfo,pCreateData->lpCreateParams,sizeof(SetInfo));
			
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"Send");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hNetworkFrame,ML("Divert if unanswered"));
			
			GetClientRect(hWnd,&rClient);
			iControlH = rClient.bottom/3;
			iControlW = rClient.right;
			
			hDelayTime = CreateWindow( "SPINBOXEX", ML("Delay:"), 
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |  SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
				0, 0, iControlW, iControlH, 
				hWnd, (HMENU)IDC_DELAYTIME, NULL, NULL);
			
			SendMessage( hDelayTime,SSBM_ADDSTRING,0,(LPARAM)ML("5 seconds"));
			SendMessage( hDelayTime,SSBM_ADDSTRING,0,(LPARAM)ML("10 seconds"));
			SendMessage( hDelayTime,SSBM_ADDSTRING,0,(LPARAM)ML("15 seconds"));
			SendMessage( hDelayTime,SSBM_ADDSTRING,0,(LPARAM)ML("20 seconds"));
			SendMessage( hDelayTime,SSBM_ADDSTRING,0,(LPARAM)ML("25 seconds"));
			SendMessage( hDelayTime,SSBM_ADDSTRING,0,(LPARAM)ML("30 seconds"));
			
			SendMessage( hDelayTime,SSBM_ADDSTRING,0,(LPARAM)ML("Network default"));
		}
        break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
		// 	SetFocus(hList);
			EndPaint(hWnd,NULL);
		}
		
		break;
    case WM_SETFOCUS:
        SetFocus(hDelayTime);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"Send");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hNetworkFrame,ML("Divert if unanswered"));
		SetFocus(hDelayTime);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("ChangeDelayTime",NULL);
        break;
		
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			
			break;	
		case VK_F5:
			{
				int i;
				i=SendMessage(hDelayTime,SSBM_GETCURSEL,0,0);
				if(i==6)
					i=0;
				else
					i=i+1;
				DelaySelectionList(i);
				
			}
			break;
		case VK_RETURN:
			{
				int i;
				i=SendMessage(hDelayTime,SSBM_GETCURSEL,0,0);
				switch(i)
				{
				case 0:
					delaytime=5;
					break;
				case 1:
					delaytime=10;
					break;
				case 2:
					delaytime=15;
					break;
				case 3:
					delaytime=20;
					break;
				case 4:
					delaytime=25;
					break;
				case 5:
					delaytime=30;
					break;
				case 6:
					delaytime=10;
					break;
				}
				
				CallFwd.Time=delaytime;
				CallFwd.bEnable=TRUE;

				switch(SetInfo.service_type)
				{
				case VOICECALL:
					CallFwd.Class=CLASS_VOICE;
					break;
				case DATACALL:
					CallFwd.Class=CLASS_DATA;
					break;
				case FAXCALL:
					CallFwd.Class=CLASS_FAX;
					break;
				case DIVERTALLTYPE:
					CallFwd.Class=CLASS_ALL;
					break;
				}

				CallFwd.Reason=FWD_NOREPLY;
				memset(CallFwd.PhoneNum,0,sizeof(CallFwd.PhoneNum));
				memcpy(CallFwd.PhoneNum,SetInfo.phone_num,sizeof(CallFwd.PhoneNum));
				
				if(ME_SetCallForwardStatus(hWnd,ME_MSG_DIVERT_IFUNANSWERED,&CallFwd)<0)
					return FALSE;
				
				bTimeOut=FALSE;
				SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
				WaitWin(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
				
			}
			break;
		}
		break;  //End of WM_KEYDOWN

		case ME_MSG_DIVERT_IFUNANSWERED:
			if(bTimeOut)
				return FALSE;
			
			switch(wParam) 
			{
			case ME_RS_SUCCESS:

				if(CallFwd.Class==CLASS_VOICE || CallFwd.Class==CLASS_ALL)
					SetForward_NoReply(TRUE);

				printf("Divert if unanswered Set success\r\n");
				WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
			
				KillTimer(hWnd,ID_REQUEST_TIMER);

				PLXTipsWin(NULL,NULL,0,ML("Call divert set"),ML("Network service"),Notify_Success,ML("Ok"),NULL,WAITTIMEOUT);

				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				PostMessage(hWnd,WM_CLOSE,0,0);
				//SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
				PostMessage(hPrevWin,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
				break;
				
			default:
				printf("Set divert if unanswered failure\r\n");
				if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0) //Check which error it is
				{
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					KillTimer(hWnd,ID_REQUEST_TIMER);
					
					hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
					PostMessage(hWnd,WM_CLOSE,0,0);
					PostMessage(hPrevWin,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
				}
				break;
			}
			break;

		case ME_MSG_GETLASTERROR:
			{
				ME_EXERROR LastError;
				char ErrorInfo[50];
				if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
				{
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					
					KillTimer(hWnd,ID_REQUEST_TIMER);
					GetErrorString(LastError,ErrorInfo);
					PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
				}
				
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				PostMessage(hWnd,WM_CLOSE,0,0);
				PostMessage(hPrevWin,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
			}
			break;

		case WM_TIMER:
			KillTimer(hWnd,ID_REQUEST_TIMER);
			WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
			bTimeOut=TRUE;

//			PLXTipsWin(NULL,NULL,0,ML("Other error"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);

			hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
			PostMessage(hWnd,WM_CLOSE,0,0);
			//SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			PostMessage(hPrevWin,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
			
		case WM_SPINBOX_CHANGE:
			SendMessage(hDelayTime,SSBM_SETCURSEL,wParam,(LPARAM)NULL);
			break;
			
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}
///////////////////////////////////////////////////////////////////////////
////    Delay Selction List ---SPECS Page15-Page16
/////////////////////////////////////////////////////////////////////////
static void DelaySelectionList(int i)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = DelaySelectionListProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "DelaySelectionList";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("DelaySelectionList","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)&i );
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
	
}

static LRESULT DelaySelectionListProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	HDC hdc;
	static HWND hList;
	static HBITMAP hRadioOn,hRadioOff;
	static int iSelected;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			int i;
			PCREATESTRUCT pCreateData=(PCREATESTRUCT)lParam;
			int focus_item=*(int *)(pCreateData->lpCreateParams);

			GetClientRect(hWnd,&rClient);
			
			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_DELAYTIME_LIST,NULL,NULL);
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("Network default"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("5 seconds"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("10 seconds"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("15 seconds"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("20 seconds"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("25 seconds"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("30 seconds"));
			
			hdc=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hdc,"/rom/network/radio_button_on.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hdc,"/rom/network/radio_button_off.bmp",NULL,NULL);
			ReleaseDC(hWnd,hdc);

			for(i=0;i<7;i++)
				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOff);
			
			SendMessage(hList,LB_SETCURSEL,focus_item,0);
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)focus_item),(LPARAM)hRadioOn);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Delay"));

			iSelected=focus_item;  //No item has been selected.
		}
        break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		
		break;
    case WM_SETFOCUS:
        SetFocus(hList);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
		SetWindowText(hNetworkFrame,ML("Delay"));
		SetFocus(hList);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("DelaySelectionList",NULL);
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;
		
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			
			break;	
		case VK_F5:
			{
				int i;
				HWND hPrevWnd;

				i=SendMessage(hList,LB_GETCURSEL,0,0); 
				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOn);

				if(iSelected!=-1)
					SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,iSelected),(LPARAM)hRadioOff);
				iSelected=i;
				
				hPrevWnd=GetWindow(hWnd,GW_HWNDNEXT);

				if(i==0)
					SendMessage(hPrevWnd,WM_SPINBOX_CHANGE,6,(LPARAM)NULL);
				else
					SendMessage(hPrevWnd,WM_SPINBOX_CHANGE,i-1,(LPARAM)NULL);
				
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}
			break;
		}
		break;
		
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}

///////////////////////////////////////////////////////////////////////////////////
///////			Set ---->To number										//////////
//////																	/////////
/////////////////////////////////////////////////////////////////////////////////
//#define IDC_DIVERTTONUMBER_EDIT		0x501
#define  MAX_PHONECODE_LENGTH		40

static HWND hEdit;
static void DivertToNumber_NotInIfUnanswered(DIVERT_SET_INFO *pSetInfo)
{
	HWND hNetworkFrame,hWnd,hList;
	RECT rClient;
	WNDCLASS wc;
	int i;
	RECT rListItem;
//	IMEEDIT ie;
//	DWORD dwStyle;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = DivertToNumber_NotInIfUnanswered_Proc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "DivertToNumber_NotInIfUnanswered";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("DivertToNumber_NotInIfUnanswered","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)pSetInfo);
	
	hList=GetDlgItem(hWnd,IDC_TONUMBER_LIST);

	i=SendMessage(hList,LB_GETCURSEL,0,0);
	SendMessage(hList,LB_GETITEMRECT,(WPARAM)0,(LPARAM)&rListItem); //!!!!why I can't get the RECT to rListItem

/*
	memset(&ie, 0, sizeof(IMEEDIT));
	ie.hwndNotify	= (HWND)hWnd;    
	ie.dwAttrib	    = 0;                
	ie.dwAscTextMax	= 0;
	ie.dwUniTextMax	= 0;
	ie.wPageMax	    = 0;        
	ie.pszCharSet	= NULL;
	ie.pszTitle	    = NULL;
	dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE | CS_NOSYSCTRL;
	ie.pszImeName	= "ºÅÂë";

	hEdit = CreateWindow("IMEEDIT","",dwStyle,
		rListItem.left,rListItem.top,rListItem.right - rListItem.left,rListItem.bottom - rListItem.top,
		hWnd,
		(HMENU)IDC_TONUMBER_EDIT,
		NULL,
		(PVOID)&ie);
	
	SendMessage(hEdit, EM_LIMITTEXT, (WPARAM)MAX_PHONECODE_LENGTH, (LPARAM)NULL); //bug fixed
*/

	hEdit = CreateWindow(WC_RECIPIENT,"",
		WS_CHILD | WS_VISIBLE | WS_TABSTOP | CS_NOSYSCTRL,
		rClient.left,rClient.top,rClient.right-rClient.left,(rClient.bottom-rClient.top)/3,
		hWnd,
		NULL,
		NULL,
		NULL);
	
	SendMessage(hEdit, REC_SETMAXREC, 1, (LPARAM)NULL);
	SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)ML("Number:"));
	
	SetFocus(hEdit);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
	
}

static LRESULT DivertToNumber_NotInIfUnanswered_Proc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hList;
	static DIVERT_SET_INFO SetInfo;
	static CALL_FWD CallFwd;
	static int state;
	static BOOL bTimeOut;
//	char buffer[MAX_PHONECODE_LENGTH+1];
//	int len;
	HWND hPrevWin;
	static ABNAMEOREMAIL ContactInfo;
	static BOOL bContactValidate;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;

			memcpy(&SetInfo,pCreateStruct->lpCreateParams,sizeof(DIVERT_SET_INFO));

			GetClientRect(hWnd,&rClient);
			
			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP|LBS_MULTILINE,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_TONUMBER_LIST,NULL,NULL);
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("Number:"));
			SendMessage(hList,LB_SETCURSEL,0,0);

	  		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
 			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Call divert"));
			bContactValidate=FALSE;
		}
        break;


	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_TONUMBER_LIST:
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
					SetFocus(GetDlgItem(hWnd, IDC_TONUMBER_EDIT));
					break;
    			default:
					break;
					
				}
				
			}
			break;
			
		case IDC_TONUMBER_EDIT:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				bContactValidate=FALSE;
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
	  		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel")); //BUG ???
 			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Call divert"));
			SetFocus(hList);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("DivertToNumber_NotInIfUnanswered",NULL);
        break;
		
	case WM_GET_CONTACTPHONENUM:
		{
			int		num = 0;
			int		i = 0;
			PABNAMEOREMAIL  pArray = NULL;
//			HWND            hWndPhone = 0;
			
			pArray = (PABNAMEOREMAIL)lParam;
			
			if (LOWORD(wParam) == FALSE)
			{
				if (pArray)
					free(pArray);
				return 0;
			}
			
			SendMessage(hEdit, REC_CLEAR, 0, 0);
			
			num = HIWORD(wParam);
			
			for (i = 0; i < num; i++)
			{
				SendMessage(hEdit, GHP_ADDREC, 0, (LPARAM)&pArray[i]);
				bContactValidate = TRUE;
			}
			if (pArray)
				free(pArray);
/*
			if(wParam==TRUE)
			{
				memcpy(&ContactInfo,(PVOID)lParam,sizeof(ContactInfo));
				SendMessage(hEdit, GHP_ADDREC, 0, (LPARAM)&ContactInfo);
				bContactValidate=TRUE;
			}
*/
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	

		case VK_F5:
			{
				int		i = 0, k = 0;
				HWND	hWndPhone;
				RECIPIENTLISTBUF	RecList;
				PABNAMEOREMAIL		pArray;
				PRECIPIENTLISTNODE   pTemp = NULL;

				RECIPIENT_InitListBuf(&RecList);
				memset(&RecList,0,sizeof(RECIPIENTLISTBUF));

				SendMessage(hEdit, GHP_GETREC, 0, (LPARAM)&RecList);
				hWndPhone = GetDlgItem(hWnd,IDC_TONUMBER_LIST);
				pArray = malloc(sizeof(ABNAMEOREMAIL) );

				pTemp = RecList.pDataHead;

				for(i = 0; i < RecList.nDataNum ; i++)
				{
					if(pTemp->bExistInAB)
					{
						strcpy(pArray[k].szName, pTemp->szShowName);
						strcpy(pArray[k].szTelOrEmail, pTemp->szPhoneNum);

						pArray[k].nType = AB_NUMBER;
						
						k++;
					}
					pTemp = pTemp->pNext;
				}

				APP_GetMultiPhoneOrEmail(hNetworkFrame, hWnd, WM_GET_CONTACTPHONENUM, 
					PICK_NUMBERANDEMAIL, pArray,k,1);


//			RecList.pDataHead->szPhoneNum;

/*
				HWND            hWndPhone;
				PABNAMEOREMAIL  pArray;
				RECIPIENTLISTBUF Recipient;	
				PRECIPIENTLISTNODE pTemp = NULL;	
				int i = 0, k=0;
				
				hWndPhone = GetDlgItem(hWnd, ID_PHONENUM);
				pArray = MMS_malloc(sizeof(ABNAMEOREMAIL) * MAX_ADDR_NUM);
				memset(pArray, 0, sizeof(ABNAMEOREMAIL) * MAX_ADDR_NUM);
				
				memset(&Recipient,0,sizeof(RECIPIENTLISTBUF));
				
				SendMessage(hWndPhone,GHP_GETREC,0,(LPARAM)&Recipient);
				
				pTemp = Recipient.pDataHead;
				
				for(i = 0; i < Recipient.nDataNum ; i++)
				{
					if(pTemp->bExistInAB)
					{
						strcpy(pArray[k].szName, pTemp->szShowName);
						strcpy(pArray[k].szTelOrEmail, pTemp->szPhoneNum);
						if(MMS_IsEmail(pArray[k].szTelOrEmail))
							pArray[k].nType = AB_EMAIL;
						else
							pArray[k].nType = AB_NUMBER;
						
						k++;
					}
					pTemp = pTemp->pNext;
				}
				// call phonebook, need to ...
				APP_GetMultiPhoneOrEmail(hWndFrame, hWnd, MMS_ADD_RECIPIENT, 
					PICK_NUMBERANDEMAIL, pArray,k,20 - (Recipient.nDataNum - k));
*/
///				if(!APP_GetOnePhoneOrEmail(hNetworkFrame,hWnd,WM_GET_CONTACTPHONENUM,PICK_NUMBER))
///						return FALSE;
			}
			break;
			
		case VK_RETURN:
			{
				RECIPIENTLISTBUF	RecList;
				RECIPIENT_InitListBuf(&RecList);

				if (GetWindowTextLength(hEdit) == 0)
				{
					PLXTipsWin(NULL,NULL,0,ML("please def number"),ML("Call divert"),Notify_Alert,ML("Ok"),NULL,WAITTIMEOUT);					
					return FALSE;
				}

				SendMessage(hEdit, GHP_GETREC, 0, (LPARAM)&RecList);
				//GetWindowText(hEdit,buffer,MAX_PHONECODE_LENGTH);
				
				//len=sizeof(CallFwd.PhoneNum);
				memset(&CallFwd,0,sizeof(CallFwd));
				//memcpy(CallFwd.PhoneNum,(void*)buffer,len);
				strcpy(CallFwd.PhoneNum, RecList.pDataHead->szPhoneNum);
				//CallFwd.PhoneNum[len-1]='\0';
				
				CallFwd.bEnable=TRUE;

				switch(SetInfo.divert_type) 
				{
				case DIVERT_ALLCALLS:
					CallFwd.Reason=FWD_UNCDTIONAL;
					break;
				case DIVERT_IFBUSY:
					CallFwd.Reason=FWD_BUSY;
					break;
				case DIVERT_IFNOTREACHABLE:
					CallFwd.Reason=FWD_NOREACH;
					break;
				default:
					return FALSE;
				}

				switch(SetInfo.service_type)
				{
				case VOICECALL:
					CallFwd.Class=CLASS_VOICE;
					break;
				case DATACALL:
					CallFwd.Class=CLASS_DATA;
					break;
				case FAXCALL:
					CallFwd.Class=CLASS_FAX;
					break;
				case DIVERTALLTYPE:
					CallFwd.Class=CLASS_ALL;
					break;
				}
				//Send the request of set 

				if(ME_SetCallForwardStatus(hWnd,ME_MSG_CALL_DIVERT,&CallFwd)<0)
					return FALSE;
				SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
				bTimeOut=FALSE;
				WaitWin(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
				
			}
			break;
		} //WM_KEYDOWN
		break;
		
		case ME_MSG_CALL_DIVERT:
			if(bTimeOut)
				return FALSE;

			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{
					if(CallFwd.Class==CLASS_VOICE || CallFwd.Class==CLASS_ALL)
					{
						if(CallFwd.Reason==FWD_UNCDTIONAL)
						{
							SetForward_Uncdtional(TRUE);
							DlmNotify(PS_SETCALLDIVERT, ICON_SET);	//xjdan fixed
						}
						else if(CallFwd.Reason==FWD_BUSY)
							SetForward_Busy(TRUE);
						else if(CallFwd.Reason==FWD_NOREACH)
							SetForward_NoReach(TRUE);
					}

					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					KillTimer(hWnd,ID_REQUEST_TIMER);
					PLXTipsWin(NULL,NULL,0,ML("Call divert set"),ML("Network service"),Notify_Success,ML("Ok"),NULL,WAITTIMEOUT);

					hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
					PostMessage(hWnd,WM_CLOSE,0,0);
					PostMessage(hPrevWin,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
				}
				break;

			default:
				printf("call divert failure\r\n");
				if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
				{
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
					KillTimer(hWnd,ID_REQUEST_TIMER);

					hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
					PostMessage(hWnd,WM_CLOSE,0,0);
					PostMessage(hPrevWin,WM_CLOSE,0,0);
					SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
				}
				break;
			}
			break;

		case ME_MSG_GETLASTERROR:
			{
				ME_EXERROR LastError;
				char ErrorInfo[50];
				if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
				{
					WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);

					KillTimer(hWnd,ID_REQUEST_TIMER);
					GetErrorString(LastError,ErrorInfo);
					PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
				}
				
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				PostMessage(hWnd,WM_CLOSE,0,0);
				PostMessage(hPrevWin,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
			}
			break;

		case  WM_TIMER:
			KillTimer(hWnd,ID_REQUEST_TIMER);
			bTimeOut=TRUE;
			WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);

//			PLXTipsWin(NULL,NULL,0,ML("Other error"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);

			hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
			PostMessage(hWnd,WM_CLOSE,0,0);
			PostMessage(hPrevWin,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}

static void DivertToNumber_IfUnanswered(DIVERT_SET_INFO *pSetInfo)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = DivertToNumber_IfUnanswered_Proc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "DivertToNumber_IfUnanswered";
	if(!RegisterClass(&wc))
		return ;
	
	hNetworkFrame=GetNetworkFrameWnd();
	
	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("DivertToNumber_IfUnanswered","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)pSetInfo);
	

	SetFocus(hWnd);
	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}

static LRESULT DivertToNumber_IfUnanswered_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hList,hSpinBox;
	static int iSelected;
	static char DivertNum[100];
	static DIVERT_SET_INFO SetInfo;
	static HWND hFocus;
	static int state;
	static BOOL bTimeOut;
	static CALL_FWD CallFwd;
	HWND hPrevWin;
	static ABNAMEOREMAIL ContactInfo;
	static BOOL bContactValidate;

	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			int i;
			RECT rListItem;
			int height;
			PCREATESTRUCT pCreateData=(PCREATESTRUCT)lParam;

			memcpy(&SetInfo,pCreateData->lpCreateParams,sizeof(DIVERT_SET_INFO));
			iSelected=0;

			GetClientRect(hWnd,&rClient);
			height=(rClient.bottom-rClient.top)/3;
			
			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP|LBS_MULTILINE,
				rClient.left,rClient.top,rClient.right-rClient.left,height,
				hWnd,(HMENU)IDC_TONUMBER_LIST,NULL,NULL);
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("Number:"));
			SendMessage(hList,LB_SETCURSEL,0,0);

			i=SendMessage(hList,LB_GETCURSEL,0,0);
			SendMessage(hList,LB_GETITEMRECT,(WPARAM)0,(LPARAM)&rListItem); //Here got value is error

			hEdit = CreateWindow(WC_RECIPIENT,"",
				WS_CHILD | WS_VISIBLE | WS_TABSTOP | CS_NOSYSCTRL,
				rClient.left,rClient.top,rClient.right-rClient.left,(rClient.bottom-rClient.top)/3,
				hWnd,
				NULL,
				NULL,
				NULL);

			SendMessage(hEdit, REC_SETMAXREC, 1, (LPARAM)NULL);			
			SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)ML("Number:"));
	
		
			//Create SpinBox
			hSpinBox = CreateWindow( "SPINBOXEX", ML("Delay:"), 
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
				rClient.left,rClient.top+height,rClient.right-rClient.left,height, 
				hWnd, (HMENU)IDC_TONUMBER_SPINBOX, NULL, NULL);
			
			
			SendMessage(hSpinBox,SSBM_ADDSTRING,0,(LPARAM)ML("5 seconds"));
			SendMessage(hSpinBox,SSBM_ADDSTRING,0,(LPARAM)ML("10 seconds"));
			SendMessage(hSpinBox,SSBM_ADDSTRING,0,(LPARAM)ML("15 seconds"));
			SendMessage(hSpinBox,SSBM_ADDSTRING,0,(LPARAM)ML("20 seconds"));
			SendMessage(hSpinBox,SSBM_ADDSTRING,0,(LPARAM)ML("25 seconds"));
			SendMessage(hSpinBox,SSBM_ADDSTRING,0,(LPARAM)ML("30 seconds"));
			SendMessage(hSpinBox,SSBM_ADDSTRING,0,(LPARAM)ML("Network default"));
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
 			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Divert if unanswered"));

			iSelected=0;
			hFocus=hEdit;
			bContactValidate=FALSE;
		}
        break;
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_TONUMBER_LIST:
			{
				switch(HIWORD(wParam))
				{
				case LBN_SETFONT:
					{
						HFONT hFont = NULL;
						GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
						return (LRESULT)hFont;
					}
					break;

				case LBN_SETFOCUS:
					SetFocus(GetDlgItem(hWnd, IDC_TONUMBER_EDIT));
					break;
					
    			default:
					break;
				}
				
			}
			
		case IDC_TONUMBER_EDIT:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				bContactValidate=FALSE;
				
			}
			break;
		}//WM_COMMAND
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
//	  		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
 			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Divert if unanswered"));
			SetFocus(hFocus);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("DivertToNumber_IfUnanswered",NULL);
        break;
		
	case WM_SPINBOX_CHANGE:
		SendMessage(hSpinBox,SSBM_SETCURSEL,wParam,(LPARAM)NULL);
		break;

	case WM_TIMER:
		WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
		KillTimer(hWnd,ID_REQUEST_TIMER);
		bTimeOut=TRUE;

//		PLXTipsWin(NULL,NULL,0,ML("Other error"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
			
		hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		PostMessage(hPrevWin,WM_CLOSE,0,0);
		SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
		break;

	case ME_MSG_TONUMBER_IFUNANSWERED:
		if(bTimeOut)
			return FALSE;

		switch(wParam) 
		{
		case ME_RS_SUCCESS:
			{
				printf("\r\nGet the return message of ME function! \r\n");
				if(CallFwd.Class==CLASS_VOICE || CallFwd.Class==CLASS_ALL)
				{
					SetForward_NoReply(TRUE);			///??
//					return TRUE;
				}
				//To use less code line,We set CallFwd variable to static,so it can keep the last value.
				//Here we just change the CallFwd.Class.
				KillTimer(hWnd,ID_REQUEST_TIMER);
				WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
				PLXTipsWin(NULL,NULL,0,ML("Call divert set"),ML("Network service"),Notify_Success,ML("Ok"),NULL,WAITTIMEOUT);

				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
				PostMessage(hPrevWin,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
			}
			break;
			
		default:
			printf("Divert to Number,If Unanswered failure\r\n"); 
			if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0) //Check which error it is
			{
				WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
				KillTimer(hWnd,ID_REQUEST_TIMER);
				
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				PostMessage(hWnd,WM_CLOSE,0,0);
				PostMessage(hPrevWin,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hPrevWin, (LPARAM)0);
			}
			break;
		}
		break;

	case ME_MSG_GETLASTERROR: //Get detailed error
		{
			ME_EXERROR LastError;
			char ErrorInfo[50];
			if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
			{
				WaitWin(hWnd,FALSE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
				KillTimer(hWnd,ID_REQUEST_TIMER);
				GetErrorString(LastError,ErrorInfo);
				PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
			}
			hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
			PostMessage(hWnd,WM_CLOSE,0,0);
			PostMessage(hPrevWin,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW, (WPARAM) hPrevWin, (LPARAM)0);
		}
		break;

	case WM_GET_CONTACTPHONENUM:
		{
			int		num = 0;
			int		i = 0;
			PABNAMEOREMAIL  pArray = NULL;
			
			pArray = (PABNAMEOREMAIL)lParam;
			
			if (LOWORD(wParam) == FALSE)
			{
				if (pArray)
					free(pArray);
				return 0;
			}
			
			SendMessage(hEdit, REC_CLEAR, 0, 0);
			
			num = HIWORD(wParam);
			
			for (i = 0; i < num; i++)
			{
				SendMessage(hEdit, GHP_ADDREC, 0, (LPARAM)&pArray[i]);
				bContactValidate = TRUE;
			}
			if (pArray)
				free(pArray);
/*
			if(wParam==TRUE)
			{
				memcpy(&ContactInfo,(PVOID)lParam,sizeof(ContactInfo));
				SendMessage(hEdit, GHP_ADDREC, 0, (LPARAM)&ContactInfo);
				bContactValidate=TRUE;
			}
*/
				
		}
		break;
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	

		case VK_F5:
			{
				if(iSelected != 1)
				{
					int		i = 0;
					int     k = 0;
					RECIPIENTLISTBUF	RecList;
					PABNAMEOREMAIL		pArray;
					PRECIPIENTLISTNODE   pTemp = NULL;
					
					RECIPIENT_InitListBuf(&RecList);
					memset(&RecList,0,sizeof(RECIPIENTLISTBUF));
					
					SendMessage(hEdit, GHP_GETREC, 0, (LPARAM)&RecList);

					pArray = malloc(sizeof(ABNAMEOREMAIL) );
					
					pTemp = RecList.pDataHead;
					
					for(i = 0; i < RecList.nDataNum ; i++)
					{
						if(pTemp->bExistInAB)
						{
							strcpy(pArray[k].szName, pTemp->szShowName);
							strcpy(pArray[k].szTelOrEmail, pTemp->szPhoneNum);
							
							pArray[k].nType = AB_NUMBER;
							
							k++;
						}
						pTemp = pTemp->pNext;
					}
					
					APP_GetMultiPhoneOrEmail(hNetworkFrame, hWnd, WM_GET_CONTACTPHONENUM, 
						PICK_NUMBERANDEMAIL, pArray,k,1);
/*
					if(!APP_GetOnePhoneOrEmail(hNetworkFrame,hWnd,WM_GET_CONTACTPHONENUM,PICK_NUMBER))
						return FALSE;
*/
				}
				else
				{
					int i;
					i=SendMessage(hSpinBox,SSBM_GETCURSEL,0,0);
					if(i==6)
						i=0;
					else
						i=i+1;
					DelaySelectionList(i);
				}

			}
			break;

		case VK_RETURN:
			{
				int i;
				RECIPIENTLISTBUF	RecList;
				RECIPIENT_InitListBuf(&RecList);

				if (GetWindowTextLength(hEdit) == 0)
				{
					PLXTipsWin(NULL,NULL,0,ML("please def number"),ML("divert if unanswered"),Notify_Alert,ML("Ok"),NULL,WAITTIMEOUT);
					return FALSE;
				}
								
				CallFwd.bEnable=TRUE;

				SendMessage(hEdit, GHP_GETREC, 0, (LPARAM)&RecList);

				strcpy(CallFwd.PhoneNum, RecList.pDataHead->szPhoneNum);

				switch(SetInfo.service_type) 
				{
				case VOICECALL:
					CallFwd.Class=CLASS_VOICE;
					break;
				case DATACALL:
					CallFwd.Class=CLASS_DATA;
					break;
				case FAXCALL:
					CallFwd.Class=CLASS_FAX;
					break;
				case DIVERTALLTYPE:
					CallFwd.Class=CLASS_ALL;
					break;
				default:
					return FALSE;
				}

				CallFwd.Reason=FWD_NOREPLY;

				i=SendMessage(hSpinBox,SSBM_GETCURSEL,0,0);
				if(i==6)
					CallFwd.Time=10;
				else
					CallFwd.Time=(i+1)*5;
//Send the request of set 
				printf("\r\nNetwork Report: run ME function to ME_MSG_TONUMBER_IFUNANSWERED! \r\n");
				if(ME_SetCallForwardStatus(hWnd,ME_MSG_TONUMBER_IFUNANSWERED,&CallFwd)<0)
					return FALSE;

				SetTimer(hWnd,ID_REQUEST_TIMER,	ME_TIMEOUT*1000,NULL);
				bTimeOut=FALSE;
				WaitWin(hWnd,TRUE,ML("Requesting..."),ML("Network service"),NULL,NULL,0);
			}
			break;

	
		case VK_UP:
			if(iSelected==0)
			{
				iSelected=1;

				SetFocus(hList); //to Hide Editor ShowWindow(hEdit,SW_HIDE);
				SetFocus(hSpinBox);
				hFocus=hSpinBox;
			}
			else
			{
				iSelected=0;

				SetFocus(hEdit);
				hFocus=hEdit;
			}
			break;



		case VK_DOWN:
			if(iSelected==1)
			{	
				iSelected=0;

				SetFocus(hEdit);
				hFocus=hEdit;
			}
			else
			{
				iSelected=1;
				SetFocus(hList);
				SetFocus(hSpinBox);
				hFocus=hSpinBox;	
			}

			break;
		}
		break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}


static void CallDivert_OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus)
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

void SetShortCut(BOOL shortcut2)
{
	shortcut = shortcut2;
}

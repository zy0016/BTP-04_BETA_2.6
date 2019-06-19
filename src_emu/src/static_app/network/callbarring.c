  #include "stdio.h"
#include "string.h"
#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "me_wnd.h"
#include "imesys.h"
#include "network.h"
#include "pubapp.h"
#include "public/PreBrowHead.h"

#define   ME_TIMEOUT			20    //ME timer time out

#define	  ET_REPEAT_LATER		100
#define   ET_REPEAT_FIRST		300

#define   IDC_BAROUTGING		0x101
#define   IDC_BARINCOMING		0x102
#define   IDC_CLEARALL			0x103
#define   IDC_CHANGECODE		0x104

#define   IDM_BAROUTGOING_CHECKSTATUS	0x201
#define   IDM_BAROUTGOING_SET			0x202
#define   IDM_BAROUTGOING_CLEAR			0x203

//Control ID and Menu for Bar incoming
#define   IDC_BARINCOMINGALL			0x301
#define   IDC_BARINCOMINGWHENROAMING	0x302
#define   IDM_BARINCOMING_CHECKSTATUS	0x303
#define   IDM_BARINCOMING_SET			0x304
#define   IDM_BARINCOMING_CLEAR			0x305

//Control ID for Change Code
#define   IDC_CHANGECODELIST			0x401
#define   IDC_CHANGECODEEDIT			0x402
#define   IDC_CHANGECODEEDIT1			0x403
#define   IDC_CHANGECODEEDIT2			0x404

//Menu for Bar outgoing
#define   IDS_BAROUTGOING_CHECKSTATUS	ML("Check status")
#define   IDS_BAROUTGOING_SET			ML("Set")
#define   IDS_BAROUTGOING_CLEAR			ML("Clear")

//Menu for Bar incoming
#define   IDS_BARINCOMING_CHECKSTATUS	ML("Check status")
#define   IDS_BARINCOMING_SET			ML("Set")
#define   IDS_BARINCOMING_CLEAR			ML("Clear")

#define   ME_MSG_CHECK_BAR		(WM_USER+101)
#define	  ME_MSG_BAR_SET        (WM_USER+102)
#define   ME_MSG_BAR_CLEAR		(WM_USER+103)
#define   ME_MSG_CLEARALLBAR	(WM_USER+104)
#define   ME_MSG_CHANGEPASSWORD (WM_USER+105)

#define	  TIMER_ID				(WM_USER+106)

#define   ID_REQUEST_TIMER		0x101

#define   MAX_PASSWORD		0x4

static LRESULT CallBarringMainProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL CreateSettingListControl(HWND hParentWnd,HWND *hBarOutgoing,HWND *hBarIncoming,
									 HWND *hClearAll,HWND *hChangeCode);
static void InitVScrollBar(HWND hWnd,int iItemNum);
static void DoVScroll(HWND hWnd,WPARAM wParam);


static void BarOutgoingWin();
static LRESULT BarOutgoingProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL CreateBarOutgoingList(HWND hParentWnd,HWND *hBarAll,HWND *hBarInternational,HWND *hBarInterExceptHome);
static void BarOutgoingSet(HWND hParentWnd);

static void BarIncomingWin();
static LRESULT BarIncomingProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void BarIncomingClear();
static void BarIncomingSet(HWND hParentWnd);

static void ShowCallBarringStatus(void *Info);
static LRESULT ShowCallBarringStatusProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static void BarChangeCodeWin();
static LRESULT BarChangeCodeProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static HWND CreateListEdit(HWND hParentWnd,HWND hList,int iListItem);

static void CallBarring_OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);

extern int GetSIMState (void);

enum BARTYPE{OUT_ALL,OUT_INTERNATIONAL,OUT_HOME,IN_ALL,IN_ROAMING};
enum BSTYPE{BS_DATA,BS_VOICE,BS_FAX};


/*
typedef struct tagCallBarInfo {
	int BarType; 
	int style;            
	BOOL bEnable;		  //only used for Set
	BOOL bVoice;		  //only used for check
	BOOL bData;           //only used for check
	BOOL bFax;			  //only used for check
}BARINFO;
*/

typedef struct tagBarSet
{
	int style;			 //CALLBAR_BAOC,CALLBAR_BOIC,etc.
	BOOL bEnable;        //True mena Set,False mean clear
	char Password[20];
}BARSET; //for Barring set and clear

typedef enum 
{
	VOICECALL,
	DATACALL,
	FAXCALL,
	DIVERTALLTYPE
}SERVICE_TYPE;

typedef struct tag_CALLBARCHECKINFO
{
	int			 callbar_type;
	CALLBAR_INFO callbar_info[3];
}CALLBARCHECK_INFO;

typedef struct tag_CALLBarSetInfo
{
	int			  callbar_type;
	SERVICE_TYPE  service_type;
}CALLBAR_SET_INFO;

static unsigned int  RepeatFlag = 0;
static WORD wKeyCode;

static int nCurFocus;

void CallBarringWin()
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = CallBarringMainProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "CallBarringMain";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("CallBarringMain","",WS_VISIBLE | WS_CHILD |WS_VSCROLL ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}

static LRESULT CallBarringMainProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hBarOutgoing,hBarIncoming,hBarClearAll,hBarChangeCode;
	static HWND hFocus;
	static BARSET BarSet;
	static BOOL bTimeOut;
	static int state;
	
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			int  iItemNum=4;
			CreateSettingListControl(hWnd,&hBarOutgoing,&hBarIncoming,&hBarClearAll,&hBarChangeCode);
			InitVScrollBar(hWnd,iItemNum);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
			SetWindowText(hNetworkFrame,ML("Call barrings"));

			hFocus=hBarOutgoing;
		}
        break;
		
	case WM_VSCROLL:
		DoVScroll(hWnd,wParam);
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
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Select"));
		SetWindowText(hNetworkFrame,ML("Call barrings"));
		SetFocus(hFocus);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("CallBarringMain",NULL);
        break;

	case WM_TIMER:
		switch(wParam) 
		{
		case ID_REQUEST_TIMER:
			KillTimer(hWnd,ID_REQUEST_TIMER);
			WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
				NULL,NULL);	
			PLXTipsWin(NULL,NULL,0,ML("Other error"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
			bTimeOut=TRUE;
			break;

		case TIMER_ID:
			CallBarring_OnTimeProcess(hWnd, wParam, hFocus);
			break;

		default:
			break;
		}
		break;

	case ME_MSG_CLEARALLBAR:		//--/clear all 
		switch(wParam)
		{
			if(bTimeOut)
				return FALSE;

		case ME_RS_SUCCESS:
			{
				KillTimer(hWnd,ID_REQUEST_TIMER);
				WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
								NULL,NULL);
				PLXTipsWin(NULL,NULL,0,ML("Call barring cleared"),ML("Network service"),Notify_Success,ML("Ok"),NULL,WAITTIMEOUT);
			}
			break;
			
		default:
			if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
			{
				KillTimer(hWnd,ID_REQUEST_TIMER);
				WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
					NULL,NULL);													
			}
			break;
		}
		break;

	case ME_MSG_GETLASTERROR:
		{
			ME_EXERROR LastError;
			char ErrorInfo[50];
			
			KillTimer(hWnd,ID_REQUEST_TIMER);
			WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
				NULL,NULL);	
			if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
			{
				GetErrorString(LastError,ErrorInfo);
				PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
			}
		}
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
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;
			
		case VK_F5:
			{
				if(hFocus==hBarOutgoing)
					BarOutgoingWin();
				else if(hFocus==hBarIncoming)
					BarIncomingWin();
				else if(hFocus==hBarClearAll) //Clear All barring
				{
					char passwd[40];
					BOOL bVerifyPassword;
					
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Call barrings"),Notify_Alert,ML("Ok"),NULL,WAITTIMEOUT);
						return FALSE;
					}

/* ////*/			bVerifyPassword=SSPLXVerifyPassword(hWnd,NULL,ML("Enter\nbarring code:"),passwd,MAKEWPARAM(4,4),
						ML("Send"),ML("Cancel"),-1);
					if(!bVerifyPassword)
						return FALSE;
					strcpy(BarSet.Password,passwd);
					BarSet.style=CALLBAR_UAB;
					BarSet.bEnable=FALSE;

					if(ME_SetCallBarringStatus(hWnd,ME_MSG_CLEARALLBAR,BarSet.style,BarSet.bEnable,BarSet.Password,CLASS_ALL)<0)
						return FALSE;
					state=0;
					bTimeOut=FALSE;
					SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
					WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),
								NULL,NULL);
					
				}
				else if(hFocus==hBarChangeCode)
				{
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Call barrings"),Notify_Alert,ML("Ok"),NULL,20);
						return FALSE;
					}

					BarChangeCodeWin();
				}
			}
			break;
			
		case VK_DOWN:
			{
				hFocus= GetFocus();
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, FALSE);
				SetFocus(hFocus);
				
				SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, (LPARAM)NULL);
				if ( 1 == RepeatFlag )
				{
					SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				}
			}
			break;
			
		case VK_UP:
			{
				hFocus = GetFocus();
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, TRUE);
				SetFocus(hFocus);
				
				SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, (LPARAM)NULL);
				if ( 1 == RepeatFlag )
				{
					SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				}
			}
			break;
		}
		break;
		
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}

static BOOL CreateSettingListControl(HWND hParentWnd,HWND *hBarOutgoing,HWND *hBarIncoming,
									 HWND *hBarClearAll,HWND *hBarChangeCode)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;
    GetClientRect(hParentWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hBarOutgoing = CreateWindow( "SPINBOXEX", ML("Bar outgoing"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_BAROUTGING, NULL, NULL);

    if (* hBarOutgoing == NULL)
        return FALSE;

    * hBarIncoming = CreateWindow( "SPINBOXEX", ML("Bar incoming"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_BARINCOMING, NULL, NULL);

    if (* hBarIncoming == NULL)
        return FALSE;
	
    * hBarClearAll = CreateWindow( "SPINBOXEX", ML("Clear all"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_CLEARALL, NULL, NULL);

    if (* hBarClearAll == NULL)
        return FALSE;

     * hBarChangeCode = CreateWindow( "SPINBOXEX", ML("Change code"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*3, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_CHANGECODE, NULL, NULL);

    if (* hBarChangeCode == NULL)
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

/*--------------------------------------------------------------------------*/
/*----------------------------    Bar outgoing     -------------------------*/
/*--------------------------------------------------------------------------*/
static void BarOutgoingWin()
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = BarOutgoingProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "BarOutgoing";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("BarOutgoing","",WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/ ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}

static LRESULT BarOutgoingProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hBarAll,hBarInternational,hBarInterExceptHome;
	static HWND hFocus;
	static HMENU hMenu;
	static int state;
	static BOOL bTimeOut;
	static char passwd[30];
	static BOOL bVerifyPassword;
	static BOOL bInPassWin;
	static BARSET BarSet;
	static CALLBAR_INFO BarInfo;
	static CALLBARCHECK_INFO BarCheckInfo;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			CreateBarOutgoingList(hWnd,&hBarAll,&hBarInternational,&hBarInterExceptHome);
			
			hMenu=CreateMenu();
			AppendMenu(hMenu,MF_ENABLED, IDM_BAROUTGOING_CHECKSTATUS, IDS_BAROUTGOING_CHECKSTATUS);
			AppendMenu(hMenu,MF_ENABLED, IDM_BAROUTGOING_SET, IDS_BAROUTGOING_SET);
			AppendMenu(hMenu,MF_ENABLED, IDM_BAROUTGOING_CLEAR, IDS_BAROUTGOING_CLEAR);
			PDASetMenu(hNetworkFrame,hMenu);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Check"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Options"));
			SetWindowText(hNetworkFrame,ML("Bar outgoing"));

			hFocus=hBarAll;
			bInPassWin=FALSE;
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
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Check"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Options"));
		SetWindowText(hNetworkFrame,ML("Bar outgoing"));
		PDASetMenu(hNetworkFrame,hMenu);
		SetFocus(hFocus);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("BarOutgoing",NULL);
        break;

	case WM_TIMER:
		KillTimer(hWnd,ID_REQUEST_TIMER);
		bTimeOut=TRUE;
		WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
						NULL,NULL);
		PLXTipsWin(NULL,NULL,0,ML("Other error"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
		break;
		
		
	case WM_KEYDOWN:
		if(bInPassWin)
			break;
		
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	

		case VK_F5:
			if(!GetSIMState())
			{
				PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Bar outgoing"),Notify_Alert,ML("Ok"),NULL,20);
				return FALSE;
			}
			SendMessage(hNetworkFrame,WM_KEYDOWN,wParam,lParam);
			break;

		case VK_RETURN:
			if(!GetSIMState())
			{
				PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Bar outgoing"),Notify_Alert,ML("Ok"),NULL,20);
				return FALSE;
			}
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_BAROUTGOING_CHECKSTATUS,0),0);
			break;
			
		case VK_DOWN:
			{
				hFocus= GetFocus();
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, FALSE);
				SetFocus(hFocus);
				
			}
			break;
			
		case VK_UP:
			{
				hFocus = GetFocus();
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, TRUE);
				SetFocus(hFocus);
				
				//SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, (LPARAM)NULL);
			}
			break;
		}  //End of WM_KEYDOWN
		break;
		
		case ME_MSG_CHECK_BAR:	//--//check 
			{
				if(bTimeOut)
					return FALSE;
				
				switch(wParam)
				{	
				case ME_RS_SUCCESS:
					{
						ME_GetResult(BarCheckInfo.callbar_info,3 * sizeof(CALLBAR_INFO));
							
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);
							
						if(!BarCheckInfo.callbar_info[0].Enable && !BarCheckInfo.callbar_info[1].Enable 
							&& !BarCheckInfo.callbar_info[2].Enable)
							PLXTipsWin(NULL,NULL,0,ML("Call barring\nnot active"),ML("Network service"),Notify_Info,ML("Ok"),NULL,WAITTIMEOUT);
						else
							ShowCallBarringStatus(&BarCheckInfo);
					}
					break;

				default:
					printf("Check Bar status failure\r\n");

					if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);													
					}
					break;
				}
				break;
			}	//End of ME_MSG_CHECK_BAR
			break;

		case ME_MSG_BAR_CLEAR:			//--//clear
			{
				if(bTimeOut)
					return FALSE;
				
				switch(wParam)
				{
					
				case ME_RS_SUCCESS:
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);
							
						PLXTipsWin(NULL,NULL,0,ML("Call barring cleared"),ML("Network service"),Notify_Success,ML("Ok"),NULL,20);
					}
					
					break;
				default:
					printf("Bar clear failure\r\n");
					if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);													
					}
					break;
				}
			} //end of ME_MSG_BAR_CLEAR
			break;

		case ME_MSG_BAR_SET:
			{
				if(bTimeOut)
					return FALSE;
				
				switch(wParam)
				{
				case ME_RS_SUCCESS:
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);
							
						PLXTipsWin(NULL,NULL,0,ML("Call barring\nset"),ML("Network service"),Notify_Success,ML("Ok"),NULL,20);
					}
					break;

				default:
					printf("Bar set failure\r\n");
					if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);													
					}
					break;
				}
			} //end of ME_MSG_BAR_SET
			break;

			case ME_MSG_GETLASTERROR:
			{
				ME_EXERROR LastError;
				char ErrorInfo[50];

				KillTimer(hWnd,ID_REQUEST_TIMER);
				WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
					NULL,NULL);	
				if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
				{
					GetErrorString(LastError,ErrorInfo);
					PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
				}
			}
			break;
			
			case WM_COMMAND:
				switch( LOWORD( wParam ))
				{
				case IDM_BAROUTGOING_CHECKSTATUS:
					{
						if(hFocus==hBarAll)
						{
							if(ME_GetCallBarringStatus(hWnd,ME_MSG_CHECK_BAR,CALLBAR_BAOC,CLASS_ALL,NULL)<0)
								return FALSE;
							BarCheckInfo.callbar_type = CALLBAR_BAOC;
						}
						else if(hFocus==hBarInternational)
						{
							if(ME_GetCallBarringStatus(hWnd,ME_MSG_CHECK_BAR,CALLBAR_BOIC,CLASS_ALL,NULL)<0)
								return FALSE;
							BarCheckInfo.callbar_type=CALLBAR_BOIC;
						}
						else if(hFocus==hBarInterExceptHome)
						{
							if(ME_GetCallBarringStatus(hWnd,ME_MSG_CHECK_BAR,CALLBAR_BOIC_EXHC,CLASS_ALL,NULL)<0)
								return FALSE;
							BarCheckInfo.callbar_type=CALLBAR_BOIC_EXHC;
						}
						
						  BarCheckInfo.callbar_info[0].Enable 
						= BarCheckInfo.callbar_info[1].Enable
						= BarCheckInfo.callbar_info[2].Enable
						= FALSE;
						
						SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
						bTimeOut=FALSE;
						WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);
						
					}
					break;

				case IDM_BAROUTGOING_SET:
					{
						bInPassWin=TRUE;
						bVerifyPassword=SSPLXVerifyPassword(hWnd,NULL,ML("Enter\nbarring code:"),passwd,MAKEWPARAM(4,4),
							ML("Send"),ML("Cancel"),-1);
						bInPassWin=FALSE;
						if(!bVerifyPassword)
							return FALSE;
						strcpy(BarSet.Password,passwd);
						
						BarSet.bEnable=TRUE;
						
						if(hFocus==hBarAll)
							BarSet.style=CALLBAR_BAOC;
						else if(hFocus==hBarInternational)
							BarSet.style=CALLBAR_BOIC;
						else if(hFocus==hBarInterExceptHome)
							BarSet.style=CALLBAR_BOIC_EXHC;
						
						if(ME_SetCallBarringStatus(hWnd,ME_MSG_BAR_SET,BarSet.style,BarSet.bEnable,BarSet.Password,CLASS_ALL)<0)
							return FALSE;
						
						SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
						bTimeOut=FALSE;
						WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);
					}
					break;
					
				case IDM_BAROUTGOING_CLEAR:
					{
						bInPassWin=TRUE;
						bVerifyPassword=SSPLXVerifyPassword(hWnd,NULL,ML("Enter\nbarring code:"),passwd,MAKEWPARAM(4,4),
							ML("Send"),ML("Cancel"),-1);
						bInPassWin=FALSE;
						if(!bVerifyPassword)
							return FALSE;
						strcpy(BarSet.Password,passwd);
						
						BarSet.bEnable=FALSE;
						state=0;
						
						if(hFocus==hBarAll)
							BarSet.style=CALLBAR_BAOC;
						else if(hFocus==hBarInternational)
							BarSet.style=CALLBAR_BOIC;
						else if(hFocus==hBarInterExceptHome)
							BarSet.style=CALLBAR_BOIC_EXHC;
						
						if(ME_SetCallBarringStatus(hWnd,ME_MSG_BAR_CLEAR,BarSet.style,BarSet.bEnable,BarSet.Password,CLASS_ALL)<0)
							return FALSE;
						
						SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
						bTimeOut=FALSE;
						WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);
								
					}
					break;
				} //End of WM_COMMAND
				break;
				
			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
    return lResult;
}

static BOOL CreateBarOutgoingList(HWND hParentWnd,HWND *hBarAll,HWND *hBarInternational,HWND *hBarInterExceptHome)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;
    GetClientRect(hParentWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hBarAll = CreateWindow( "SPINBOXEX", ML("All"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_BAROUTGING, NULL, NULL);

    if (* hBarAll == NULL)
        return FALSE;

    * hBarInternational = CreateWindow( "SPINBOXEX", ML("International"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_BARINCOMING, NULL, NULL);

    if (* hBarInternational == NULL)
        return FALSE;
	
    * hBarInterExceptHome = CreateWindow( "SPINBOXEX", ML("Int\'l except home"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_CLEARALL, NULL, NULL);

    if (* hBarInterExceptHome == NULL)
        return FALSE;

	return TRUE;

}

static void ShowCallBarringStatus(void *Info)
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	
	wc.style         = 0; 
	wc.lpfnWndProc   = ShowCallBarringStatusProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ShowCallBarringStatus";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("ShowCallBarringStatus","",WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		(PVOID)Info);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);

}

static LRESULT ShowCallBarringStatusProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hFormView;
	static CALLBARCHECK_INFO BarInfo;

	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			//Create Form viewr
			LISTBUF ListData;
			PCREATESTRUCT pCreateData;
			char BarType[100];
			char bs[100];
			
			RECT rClient;
			GetClientRect(hNetworkFrame,&rClient);
			pCreateData=(PCREATESTRUCT)lParam;
			memcpy(&BarInfo,pCreateData->lpCreateParams,sizeof(CALLBARCHECK_INFO));
			
			memset(BarType,0,sizeof(BarType));
			switch(BarInfo.callbar_type)
			{
			case CALLBAR_BAOC:
				strcpy(BarType,ML("All outgoing"));
				break;
			case CALLBAR_BOIC:
				strcpy(BarType,ML("All outgoing int'l"));
				break;
			case CALLBAR_BOIC_EXHC:
				strcpy(BarType,ML("All outgoing int'l except home"));
				break;
			case CALLBAR_BAIC:
				strcpy(BarType,ML("All incoming"));
				break;
			case CALLBAR_BIC_ROAM:
				strcpy(BarType,ML("All incoming when roaming"));
				break;
			default:
				break;
			}
			
			PREBROW_InitListBuf(&ListData);
			PREBROW_AddData(&ListData,ML("Type:"),BarType);

			memset(bs,0,sizeof(bs));
			if(BarInfo.callbar_info[0].Enable)
				strcat(bs,"Voice\n");
			if(BarInfo.callbar_info[1].Enable)
				strcat(bs,"Data\n");
			if(BarInfo.callbar_info[2].Enable)
				strcat(bs,"Fax\n");
			PREBROW_AddData(&ListData,ML("Service:"),bs);

			hFormView=CreateWindow(WC_FORMVIEW,NULL,WS_VISIBLE|WS_CHILD|WS_VSCROLL,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,NULL,NULL,(PVOID)&ListData);

			if(!hFormView)
				return FALSE;

			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hNetworkFrame,ML("Call barring active"));

			SetFocus(hFormView);
		}
        break;
		
	case WM_PAINT:
		{
			BeginPaint(hWnd,NULL);
			EndPaint(hWnd,NULL);
		}
		break;

    case WM_SETFOCUS:
		SetFocus(hFormView);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		SetWindowText(hNetworkFrame,ML("Call barring active"));
		SetFocus(hFormView);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("ShowCallBarringStatus",NULL);
        break;
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_LEFT:
			printf("switch formview item\r\n");
			break;

		case VK_RIGHT:
			printf("switch formview item\r\n");
			break;
	
        case VK_RETURN:
			{
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}
            break;
		
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break; 

		default :
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
    }
    return lResult;
	
}


/*--------------------------------------------------------------------------*/
/*----------------------------    Bar Incoming    -------------------------*/
/*--------------------------------------------------------------------------*/
static void BarIncomingWin()
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = BarIncomingProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; 
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "BarIncoming";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("BarIncoming","",WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/ ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
}

static LRESULT BarIncomingProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hBarIncomingAll,hBarIncomingWhenRoaming;
	static HWND hFocus;
	static HMENU hMenu;
	static CALLBARCHECK_INFO BarCheckInfo;
	static BARSET  BarSet;
	static BOOL bTimeOut;
	static int state;
	static BOOL bVerifyPassword;
	static BOOL bInPassWin;
	static CALLBAR_INFO barinfo;
	
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			{
				int     xzero=0,yzero=0,iControlH,iControlW;
				RECT    rect;
				GetClientRect(hWnd, &rect);
				iControlH = rect.bottom/3;
				iControlW = rect.right;
				
				hBarIncomingAll = CreateWindow( "SPINBOXEX", ML("All"), 
					WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
					xzero, yzero, iControlW, iControlH, 
					hWnd, (HMENU)IDC_BARINCOMINGALL, NULL, NULL);
				
				if (hBarIncomingAll == NULL)
					return FALSE;
				
				hBarIncomingWhenRoaming = CreateWindow( "SPINBOXEX", ML("When roaming"), 
					WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
					xzero, yzero+iControlH, iControlW, iControlH, 
					hWnd, (HMENU)IDC_BARINCOMINGWHENROAMING, NULL, NULL);
				
				if (hBarIncomingWhenRoaming == NULL)
					return FALSE;
				bInPassWin=FALSE;
				
			}

			hMenu=CreateMenu();
			AppendMenu(hMenu,MF_ENABLED, IDM_BARINCOMING_CHECKSTATUS, IDS_BARINCOMING_CHECKSTATUS);
			AppendMenu(hMenu,MF_ENABLED, IDM_BARINCOMING_SET, IDS_BARINCOMING_SET);
			AppendMenu(hMenu,MF_ENABLED, IDM_BARINCOMING_CLEAR, IDS_BARINCOMING_CLEAR);
			PDASetMenu(hNetworkFrame,hMenu);
			
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Check"));
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Options"));
			SetWindowText(hNetworkFrame,ML("Bar incoming"));
			hFocus=hBarIncomingAll;
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
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Check"));
		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)ML("Options"));
		SetWindowText(hNetworkFrame,ML("Bar incoming"));
		PDASetMenu(hNetworkFrame,hMenu);
		SetFocus(hFocus);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("BarIncoming",NULL);
        break;
		
	case WM_TIMER:
		KillTimer(hWnd,ID_REQUEST_TIMER);
		WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
			NULL,NULL);
		PLXTipsWin(NULL,NULL,0,ML("Other error"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
		bTimeOut=TRUE;
		break;
		
	case WM_KEYDOWN:
		if(bInPassWin)
			break;
		switch(LOWORD(wParam))
		{
		case VK_RETURN:
			if(!GetSIMState())
			{
				PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Bar incoming"),Notify_Alert,ML("Ok"),NULL,20);
				return FALSE;
			}
			SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(IDM_BARINCOMING_CHECKSTATUS,0),(LPARAM)NULL);
			break;
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			
			break;	
		case VK_F5:
			{
				if(!GetSIMState())
				{
					PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Bar incoming"),Notify_Alert,ML("Ok"),NULL,20);
					return FALSE;
				}
				SendMessage(hNetworkFrame,WM_KEYDOWN,wParam,lParam);
			}
			break;

		case VK_DOWN:
			{
				hFocus= GetFocus();
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, FALSE);
				SetFocus(hFocus);
				
			}
			break;
			
		case VK_UP:
			{
				hFocus = GetFocus();
				while(GetParent(hFocus) != hWnd)
				{
					hFocus = GetParent(hFocus);
				}
				hFocus = GetNextDlgTabItem(hWnd, hFocus, TRUE);
				SetFocus(hFocus);
				
			}
			break;
		}  //End WM_KEYDOWN
		break;

		case ME_MSG_CHECK_BAR:		//--// check
			{
				if(bTimeOut)
					return FALSE;
				
				switch(wParam)
				{
					
				case ME_RS_SUCCESS:
					{
						ME_GetResult(&BarCheckInfo.callbar_info,3 * sizeof(CALLBAR_INFO));

						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);
							
						if(	   !BarCheckInfo.callbar_info[0].Enable 
							&& !BarCheckInfo.callbar_info[1].Enable 
							&& !BarCheckInfo.callbar_info[2].Enable)
							PLXTipsWin(NULL,NULL,0,ML("Call barring\nnot active"),ML("Network service"),Notify_Info,ML("Ok"),NULL,WAITTIMEOUT);
						else
							ShowCallBarringStatus(&BarCheckInfo);
					}
					
					break;
				default:
					printf("Check Bar status failure\r\n");
					if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);													
					}
					break;
				}
			}
			break;

		case ME_MSG_BAR_CLEAR:		//--//clear
			{
				if(bTimeOut)
					return FALSE;
				
				switch(wParam)
				{
				case ME_RS_SUCCESS:
					{				
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);
							
						PLXTipsWin(NULL,NULL,0,ML("Call barring cleared"),ML("Network service"),Notify_Success,ML("Ok"),NULL,20);
					}
					
					break;
				default:
					printf("Bar clear failure\r\n");
					if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);													
					}
					break;
				}
			} //end of ME_MSG_BAR_CLEAR
			break;


		case ME_MSG_BAR_SET:		//--// set
			{
				if(bTimeOut)
					return FALSE;
				
				switch(wParam)
				{
				case ME_RS_SUCCESS:
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);
							
						PLXTipsWin(NULL,NULL,0,ML("Call barring\nset"),ML("Network service"),Notify_Success,ML("Ok"),NULL,20);
					}
					
					break;
				default:
					printf("Bar set failure\r\n");
					if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
					{
						KillTimer(hWnd,ID_REQUEST_TIMER);
						WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
							NULL,NULL);													
					}
					break;
				}
			} //end of ME_MSG_BAR_SET
			
			break;

		case ME_MSG_GETLASTERROR:
			{
				ME_EXERROR LastError;
				char ErrorInfo[50];

				KillTimer(hWnd,ID_REQUEST_TIMER);
				WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
					NULL,NULL);	
				if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
				{
					GetErrorString(LastError,ErrorInfo);
					PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
				}
			}
			break;
	

		case WM_COMMAND:
			switch( LOWORD( wParam ))
			{
			case IDM_BARINCOMING_CHECKSTATUS:
				{
					if(hFocus==hBarIncomingAll)
						BarCheckInfo.callbar_type=CALLBAR_BAIC;
					else if(hFocus==hBarIncomingWhenRoaming)
						BarCheckInfo.callbar_type=CALLBAR_BIC_ROAM;
					
					BarCheckInfo.callbar_info[0].Enable 
					= BarCheckInfo.callbar_info[1].Enable
					= BarCheckInfo.callbar_info[2].Enable
					= FALSE;

					if(ME_GetCallBarringStatus(hWnd,ME_MSG_CHECK_BAR,BarCheckInfo.callbar_type,CLASS_ALL,NULL)<0)
						return FALSE;
					
					SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
					bTimeOut=FALSE;
					WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),
						NULL,NULL);
				}
				break;

			case IDM_BARINCOMING_SET:
				{
					char passwd[40];
					bInPassWin=TRUE;
					bVerifyPassword=SSPLXVerifyPassword(hWnd,NULL,ML("Enter\nbarring code:"),passwd,MAKEWPARAM(4,4),
						ML("Send"),ML("Cancel"),-1);
					bInPassWin=FALSE;
					if(!bVerifyPassword)
						return FALSE;
					strcpy(BarSet.Password,passwd);
					
					BarSet.bEnable=TRUE;
					
					if(hFocus==hBarIncomingAll)
						BarSet.style=CALLBAR_BAIC;
					else if(hFocus==hBarIncomingWhenRoaming)
						BarSet.style=CALLBAR_BIC_ROAM;
					
					if(ME_SetCallBarringStatus(hWnd,ME_MSG_BAR_SET,BarSet.style,BarSet.bEnable,BarSet.Password,CLASS_ALL)<0)
						return FALSE;
					
					SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
					bTimeOut=FALSE;
					WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),
						NULL,NULL);
				}
				break;
				
			case IDM_BARINCOMING_CLEAR:
				{
					char passwd[40];
					bInPassWin=TRUE;
					bVerifyPassword=SSPLXVerifyPassword(hWnd,NULL,ML("Enter\nbarring code:"),passwd,MAKEWPARAM(4,4),
						ML("Send"),ML("Cancel"),-1);
					bInPassWin=FALSE;
					if(!bVerifyPassword)
						return FALSE;
					strcpy(BarSet.Password,passwd);
					
					BarSet.bEnable=FALSE;
					state=0;
					
					if(hFocus==hBarIncomingAll)
						BarSet.style=CALLBAR_BAIC;
					else if(hFocus==hBarIncomingWhenRoaming)
						BarSet.style=CALLBAR_BIC_ROAM;
					
					if(ME_SetCallBarringStatus(hWnd,ME_MSG_BAR_CLEAR,BarSet.style,BarSet.bEnable,BarSet.Password,CLASS_ALL)<0)
						return FALSE;
					
					SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*1000,NULL);
					bTimeOut=FALSE;
					WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),
						NULL,NULL);
				}
				break;
			}
			break;

			default:
				lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}


/*-----------------------------------------------------------------------*/
/*--------------		Change code In Bar			---------------------*/
/*-----------------------------------------------------------------------*/
static void BarChangeCodeWin()
{
	HWND hNetworkFrame,hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = BarChangeCodeProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "BarChangeCode";
	if(!RegisterClass(&wc))
		return ;

	hNetworkFrame=GetNetworkFrameWnd();

	GetClientRect(hNetworkFrame,&rClient);
	hWnd=CreateWindow("BarChangeCode","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hNetworkFrame,
		NULL,
		NULL,
		NULL);
	SetFocus(hWnd);

	ShowWindow(hNetworkFrame,SW_SHOW);
	UpdateWindow(hNetworkFrame);
	
}


static LRESULT BarChangeCodeProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HWND hNetworkFrame;
	static HWND hList;
	static HWND hEdit[3];
	static int iSelect;
	static char OldPass[50],NewPass[50],ConfirmPass[50];
	static BOOL bTimeOut;
	static int state;
	
	hNetworkFrame=GetNetworkFrameWnd();
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
 			int i;
			RECT rListItem;
			IMEEDIT ie;
			DWORD dwStyle;
			int Height;

			GetClientRect(hWnd,&rClient);
			
			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP|LBS_MULTILINE,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_CHANGECODELIST,NULL,NULL);
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("Old code:"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("New code:"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("Confirm new code:"));
			SendMessage(hList,LB_SETCURSEL,0,0);

//			//create Edit,and overwrite the listbox.
			SendMessage(hList,LB_GETITEMRECT,(WPARAM)0,(LPARAM)&rListItem); //!!!!why I can't get the RECT to rListItem
			memset(&ie, 0, sizeof(IMEEDIT));
			ie.hwndNotify	= (HWND)hWnd;    
			ie.dwAttrib	    = 0;                
			ie.dwAscTextMax	= 0;
			ie.dwUniTextMax	= 0;
			ie.wPageMax	    = 0;        
			ie.pszCharSet	= NULL;
			ie.pszTitle	    = NULL;
			dwStyle = WS_VISIBLE | WS_CHILD|ES_AUTOHSCROLL|WS_TABSTOP|ES_TITLE|CS_NOSYSCTRL|ES_NUMBER|ES_PASSWORD;
            ie.pszImeName	= "Digit";

			Height=(rClient.bottom-rClient.top)/3;
			
			for(i=0;i<3;i++)
			{
				hEdit[i] = CreateWindow("IMEEDIT","",dwStyle,
					rClient.left,rClient.top+i*Height,rClient.right-rClient.left,Height,
					hWnd,
					(HMENU)IDC_CHANGECODEEDIT,
					NULL,
					(PVOID)&ie);
				SendMessage(hEdit[i], EM_LIMITTEXT, (WPARAM)MAX_PASSWORD, (LPARAM)NULL); //bug fixed
			}
			
			SendMessage(hEdit[0], EM_SETTITLE, 0, (LPARAM)ML("Old code:"));
			SendMessage(hEdit[1], EM_SETTITLE, 0, (LPARAM)ML("New code:"));
			SendMessage(hEdit[2], EM_SETTITLE, 0, (LPARAM)ML("Confirm new code:"));

			iSelect=0; //Set Focus Item is set to 0
	  		SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
 			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hNetworkFrame,ML("Barring code"));
		}
        break;
	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_CHANGECODELIST:
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
					SetFocus(GetDlgItem(hWnd, IDC_CHANGECODEEDIT));
					break;

    			default:
					break;	
				}
				
			}
			break;
			
		case IDC_CHANGECODEEDIT:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				if(GetWindowTextLength(hEdit[0])>=4 && GetWindowTextLength(hEdit[1])>=4 && GetWindowTextLength(hEdit[2])>=4)
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
				else				
					SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
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
        SetFocus(hEdit[iSelect]);
        break;
		
	case PWM_SHOWWINDOW:
		if(GetWindowTextLength(hEdit[0])>=4 && GetWindowTextLength(hEdit[1])>=4 && GetWindowTextLength(hEdit[2])>=4)
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Send"));
		else				
			SendMessage(hNetworkFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");

		SetWindowText(hNetworkFrame,ML("Set call divert"));
		SetFocus(hEdit[iSelect]);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("BarChangeCode",NULL);
        break;

	case WM_TIMER:
		KillTimer(hWnd,ID_REQUEST_TIMER);
		bTimeOut=TRUE;
		WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
						NULL,NULL);
		PLXTipsWin(NULL,NULL,0,ML("Other error"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
		break;
		
	case ME_MSG_CHANGEPASSWORD:
		if(bTimeOut)
			return FALSE;

		switch(wParam) 
		{
		case ME_RS_SUCCESS:
			printf("Change password success\r\n");
			KillTimer(hWnd,ID_REQUEST_TIMER);
			WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
				NULL,NULL);
			PLXTipsWin(NULL,NULL,0,ML("Barring code\nchanged"),ML("Network service"),Notify_Success,ML("Ok"),NULL,WAITTIMEOUT);		
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;
			
		default:
			printf("Change code failure\r\n");

			if(ME_GetLastError(hWnd,ME_MSG_GETLASTERROR)<0)
			{
				KillTimer(hWnd,ID_REQUEST_TIMER);
				WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
					NULL,NULL);		
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}
			break;
		}
		break;

	case ME_MSG_GETLASTERROR:
		{
			ME_EXERROR LastError;
			char ErrorInfo[50];
			
			KillTimer(hWnd,ID_REQUEST_TIMER);
			WaitWindowStateEx(hWnd,FALSE,ML("Requesting..."),ML("Network service"),
				NULL,NULL);	
			if(ME_GetResult(&LastError,sizeof(ME_EXERROR))!=-1)
			{
				GetErrorString(LastError,ErrorInfo);
				PLXTipsWin(NULL,NULL,0,ErrorInfo,ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
			}
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hNetworkFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
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
			}
			break;

		case VK_RETURN:
			{
				if(GetWindowTextLength(hEdit[0])>=4 && GetWindowTextLength(hEdit[1])>=4 && GetWindowTextLength(hEdit[2])>=4)
				{
					GetWindowText(hEdit[0],OldPass,sizeof(OldPass));
					GetWindowText(hEdit[1],NewPass,sizeof(NewPass));
					GetWindowText(hEdit[2],ConfirmPass,sizeof(ConfirmPass));
					if(strcmp(NewPass,ConfirmPass)!=0)
					{
						PLXTipsWin(NULL,NULL,0,ML("Codes do not match"),ML("Network service"),Notify_Failure,ML("Ok"),NULL,WAITTIMEOUT);
						return FALSE;
					}

					if(ME_ChangeLockPassword(hWnd,ME_MSG_CHANGEPASSWORD,CALLBAR_BOIC,OldPass,NewPass)<0)
						return FALSE;
					bTimeOut=FALSE;
					//state=0;
					SetTimer(hWnd,ID_REQUEST_TIMER,ME_TIMEOUT*10*1000,NULL);
					WaitWindowStateEx(hWnd,TRUE,ML("Requesting..."),ML("Network service"),
						NULL,NULL);

				}
				
			}
			break;
		case VK_DOWN:
			{
				char buff[100];
				int i,len;
				
				GetWindowText(hEdit[iSelect],buff,100);
				len=strlen(buff);
				for(i=0;i<len;i++)
					buff[i]='*';
				SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(iSelect,-1),(LPARAM)buff);
				

				if(iSelect==2)
					iSelect=0;
				else
					iSelect++;
				
				SendMessage(hList,LB_SETCURSEL,iSelect,(LPARAM)0);

				SetFocus(hEdit[iSelect]);
				InvalidateRect(hWnd,NULL,FALSE);	
				
			}
			break;
		case VK_UP:
			{
				char buff[100];
				int i,len;

				GetWindowText(hEdit[iSelect],buff,100);
				len=strlen(buff);
				for(i=0;i<len;i++)
					buff[i]='*';
				SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(iSelect,-1),(LPARAM)buff);

				if(iSelect==0)
					iSelect=2;
				else
					iSelect--;
				SendMessage(hList,LB_SETCURSEL,iSelect,(LPARAM)0);
				
				SetFocus(hEdit[iSelect]);
				InvalidateRect(hWnd,NULL,FALSE);
			}
			break;
		}
		break;
		
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}

static void CallBarring_OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus)
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


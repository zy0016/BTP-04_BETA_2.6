  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setup
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "stdio.h"
#include "string.h"
#include "window.h"
#include "winpda.h"
#include "plx_pdaex.h"
#include "me_wnd.h"
#include "pubapp.h"
#include "public/PreBrowHead.h"
#include "imesys.h"
//#include "public/pubapp.h"

//#define  __MODULE__  "CallSetting Module"
#include "plxdebug.h"
//#define   CALLSETTING_OBJDUMP

#define  WM_DURINGCALL_CHANGED		(WM_USER+101)
#define  WM_AFTERCALL_CHANGED		(WM_USER+102)
#define  WM_ANYKEYANSWER_CHANGED	(WM_USER+103)
#define  WM_LINENAME_CHANGED        (WM_USER+104)
#define  WM_ACTIVELINE_CHANGED		(WM_USER+105)
#define  WM_MAXCOST_CHANGED			(WM_USER+106)
#define  WM_UNITCOST_CHANGED		(WM_USER+107)
#define  ME_MSG_ALS					(WM_USER+108)
#define  ME_MSG_CHECK_PASSWORD		(WM_USER+109)
#define  ME_MSG_GETUNITCOST			(WM_USER+110)
#define  ME_MSG_GETMAXCOST			(WM_USER+111)
#define  ME_MSG_SETUNITCOST			(WM_USER+112)
#define  ME_MSG_SETMAXCOST			(WM_USER+113)
#define  ME_MSG_SELECT_LINE			(WM_USER+114)

#define  ME_MSG_ISPIN2BLOCKED		(WM_USER+115)
#define  ME_MSG_ISPUK2BLOCKED		(WM_USER+116)
#define  ME_MSG_ISPUK2INPUTRIGHT	(WM_USER+117)
#define  ME_MSG_ISPIN2CHANGED		(WM_USER+118)
#define  ME_MSG_VERIFYPUK2			(WM_USER+119)
#define  ME_MSG_GETPASSWORDSTATUS	(WM_USER+120)

#define  TIMER_ID					(WM_USER+121)

#define  IDC_KEYANSWER				(WM_USER+122)
#define  ME_MSG_PIN2CHECK			(WM_USER+123)

#define  WM_CHECKPIN2				(WM_USER+150)
#define  WM_USERSELECTOK			(WM_USER+151)
#define  WM_PUK2ERROR				(WM_USER+152)
#define  WM_PIN2CHANGEDOK			(WM_USER+153)

#define  ME_MSG_CHECKCURRENTLINE1	(WM_USER+154) //check and save the result
#define  ME_MSG_CHECKCURRENTLINE2   (WM_USER+155) //check and compare it with line2
#define  ME_MSG_SETLINE2			(WM_USER+156) //set other line and to check 
#define  ME_MSG_SETLINE1			(WM_USER+157)

#define	 ME_MSG_GETCURRENTLINE		(WM_USER+0x100)

#define  IDC_UNITPRICE			0x101
#define  IDC_CURRENCY			0x102
#define  IDC_UNITCOSTLIST		0x103

#define  IDC_MAXCOST			0x201

#define  IDC_LINENAMEEDIT		0x301

#define  ET_REPEAT_LATER		100
#define  ET_REPEAT_FIRST		300


static LRESULT CallSettingProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void InitVScrollBar(HWND hWnd,int iItemNum);
static BOOL CreateSettingListControl(HWND hParentWnd,HWND *hCallTimer,HWND *hAOC,HWND *hAnyKeyAnswer,
									 HWND *hAltLine);
static void DoVScroll(HWND hWnd,WPARAM wParam);
static LRESULT CallTimerProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void CallTimerWin(void);
static void DuringCallWin(void *info);
static LRESULT DuringCallProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void AfterCallWin(void *info);
static LRESULT AfterCallProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void AdviceOfChargeWin(void *info);
static LRESULT AdviceOfChargeProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void UnitCostWin(void *CostInfo);
static LRESULT UnitCostProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void MaxCostWin(void *CostInfo);
static LRESULT MaxCostProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void AnyKeyAnswerWin(void *info);
static LRESULT AnyKeyAnswerProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void OutgoingLineWin(void *info);
static LRESULT OutgoingLineProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void ALSWin(void *ALSInfo);
static LRESULT ALSProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
void LineWin(void *info);
static LRESULT LineProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
BOOL ReadMobileConfigFile(MBConfig * pConfig);
static BOOL SaveMobileConfigFile(const MBConfig * pConfig);

extern void GetSimStatus_AOC(SimService *PSimSer);
extern int GetSIMState();

BOOL CheckALSWin(void);
LRESULT CheckALSWinProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

int GetCurrentAltWin (void);
void CheckPin2Blocked(HWND hFrameWin,HWND hParentWnd,UINT message,const char *szCaption);
static LRESULT CheckPin2BlockedProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static void InputPin2Code(HWND hFrameWin,HWND hParentWnd,void*info);

static void MBCall_OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);

int GetCurrentAltLine();
BOOL SetCurrentAltLine(int);

static int iOutLine;		//the outline now
static int iAnyKey;

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static char buffer1[10],buffer2[10];

static char OldLine1Name[MAX_LINENAME];
static char OldLine2Name[MAX_LINENAME];

static BOOL bALS = FALSE;			// if the sim card is ALS
static BOOL bInit = FALSE;			// if finish check the result 

static BOOL bPasswordOK = FALSE;
static char Pin2[8];

typedef struct tagALS 
{
	int iCurrentLine;
	char Line1Name[MAX_LINENAME];
	char Line2Name[MAX_LINENAME];
}LINEINFO;

typedef struct tagAOCINFO 
{
	ME_PUCINFO UnitCost;
	int MaxCost;
	char PIN2[50];
	char PUK2[50];
}AOCINFO;


/////////////////////////////////////////////////////////////////////////
typedef enum tag_PIN2_PUK2_STATE
{
	FSM_INIT=0,PIN2_NONBLOCKED,PIN2_BLOCKED,PUK2_NONBLOCKED,PUK2_BLOCKED,
	PUK2_INPUT_RIGHT,PUK2_INPUT_ERROR,PIN2_CHANGED,PUK2_INPUT_EXIT
}PIN2_PUK2_FSM;
typedef struct tag_PUK2_PIN2_CODE
{
	char PUK2[20];
	char PIN2[20];
}PUK2_PIN2_CODE;
//////////////////////////////////////////////////////////////////////////
typedef struct tagCHECKPIN2INFO
{
	HWND hFrameWin;
	HWND hParentWnd;
	UINT message;
	char szCaption[50];
	PUK2_PIN2_CODE Puk2Pin2Code;
}CHECKPIN2INFO;

static PUK2_PIN2_CODE Puk2Pin2Code;

static HWND hSettingFrame;
static int  nCurFocus;

BOOL CallSettingList(HWND hFrameWnd)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = CallSettingProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "CallSetting";
	if(!RegisterClass(&wc))
		return FALSE;

#ifdef CALLSETTING_OBJDUMP
	StartObjectDebug();
#endif

	GetClientRect(hFrameWnd,&rClient);
	
	hSettingFrame=hFrameWnd;

	hWnd = CreateWindow(
		"CallSetting",
		"",
		WS_VISIBLE | WS_CHILD |WS_VSCROLL,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hFrameWnd,
		NULL,
		NULL,
		NULL
		);
	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  

	return TRUE;
	
}

static LRESULT CallSettingProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hCallTimer,hAOC,hAnyKeyAnswer,hAltLine;
	static HWND hFocus;
	static HMENU hMenu;
	static MBConfig CallConfig;
	static AOCINFO AocInfo;
		   int i;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			int  iItemNum=4;
			
			ReadMobileConfigFile(&CallConfig);
//			WaitWin(NULL,TRUE,ML("Opening"),ML("Call Setting"),NULL,ML("Cancel"),0);
//			CheckALSWin(hSettingFrame);
			
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hSettingFrame,ML("Call"));
			CreateSettingListControl(hWnd,&hCallTimer,&hAOC,&hAnyKeyAnswer,&hAltLine);
			if(CallConfig.bModeAnyKey)
				SendMessage(hAnyKeyAnswer,SSBM_SETCURSEL,0,0);
			else
				SendMessage(hAnyKeyAnswer,SSBM_SETCURSEL,1,0);
			
			InitVScrollBar(hWnd,iItemNum);
			
			hFocus = hCallTimer;
		}
        break;
		
	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
		break;
		
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hSettingFrame,ML("Call"));
		SetFocus(hFocus);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);  

#ifdef CALLSETTING_OBJDUMP
		EndObjectDebug();
#endif
        break;
	
	case WM_TIMER:
		MBCall_OnTimeProcess(hWnd, wParam, hFocus);
		break;

    case WM_DESTROY :
        UnregisterClass("CallSetting",NULL);
        break;

	case WM_VSCROLL:
		DoVScroll(hWnd,wParam);
		break;

	case WM_ANYKEYANSWER_CHANGED:
		SendMessage(hAnyKeyAnswer,SSBM_SETCURSEL,wParam,0);
		i = (int)wParam;
		if((i==0 && CallConfig.bModeAnyKey==FALSE) |(i==1 &&CallConfig.bModeAnyKey==TRUE))
		{
			CallConfig.bModeAnyKey=(i==0)?TRUE:FALSE;
			SaveMobileConfigFile(&CallConfig);
		}
		break;

	case WM_COMMAND:
		switch(LOWORD( wParam )) 
		{
		case IDC_KEYANSWER:
			if(HIWORD( wParam ) == SSBN_CHANGE)
			{
				iAnyKey = SendMessage(hAnyKeyAnswer,SSBM_GETCURSEL,0,0);
				CallConfig.bModeAnyKey=(iAnyKey==0)?TRUE:FALSE;
				SaveMobileConfigFile(&CallConfig);
			}
			break;

		default:
			break;
		}
		break;

	case ME_MSG_ALS:
		switch(wParam) 
		{
		case ME_RS_SUCCESS:
			{
				int result;
				ME_GetResult(&result,sizeof(int));
				printf("ME_GetResult value show : %d \r\n",result);
//				result = GetCurrentAltLine();
				printf("GetCurrentAltLine() show the line is %d \r\n",GetCurrentAltLine());
				
				if(result==1) //current  line is 1
				{
					LINEINFO LineInfo;
					memset(&LineInfo,0,sizeof(LineInfo));
					LineInfo.iCurrentLine=ALS_LINE1;
					ALSWin(&LineInfo);
					iOutLine = result;
				}
				else  if(result==2) //current line is 2
				{
					LINEINFO LineInfo;
					memset(&LineInfo,0,sizeof(LineInfo));
					LineInfo.iCurrentLine=ALS_LINE2;
					ALSWin(&LineInfo);
					iOutLine = result;
				}
			}
			break;
		default:
			break;
		}
		break;
	
	//Check if there is a SIM card
//	case ME_MSG_CHECK_PASSWORD:
//		switch(wParam) {
////		case RS_SUCCESS:
////			break;
//		case ME_RS_NOSIMCARD:
//			PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Advice of charge"),Notify_Alert,ML("Ok"),NULL,20);
//			break;
//		default:
//			{
//				SimService sim;
//				GetSimStatus_AOC(&sim);
//				if(sim.SupportStatus!=Allocated)
//				{
//					PLXTipsWin(NULL,NULL,0,ML("Feature not\navailable on SIM"),ML("Advice of charge"),
//					Notify_Failure,ML("Ok"),NULL,20);
//					return FALSE;
//				}
//				ME_GetPricePerUnit(hWnd,ME_MSG_GETUNITCOST);
//			}
//			
//			break;
//		}
//		break;

	case ME_MSG_GETUNITCOST:
		switch(wParam) 
		{
		case ME_RS_SUCCESS:
			ME_GetResult(&AocInfo.UnitCost,sizeof(AocInfo.UnitCost));
			ME_GetACMmax(hWnd,ME_MSG_GETMAXCOST);	
			break;

		default:
			break;
		}
		break;

	case ME_MSG_GETMAXCOST:
		switch(wParam) 
		{
		case ME_RS_SUCCESS:
			ME_GetResult(&AocInfo.MaxCost,sizeof(AocInfo.MaxCost));
			AdviceOfChargeWin(&AocInfo);
			break;
			
		default:
			break;
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
			{
				int i;
				i=SendMessage(hAnyKeyAnswer,SSBM_GETCURSEL,0,0);
				if((i==0 && CallConfig.bModeAnyKey==FALSE) |(i==1 &&CallConfig.bModeAnyKey==TRUE))
				{
					CallConfig.bModeAnyKey=(i==0)?TRUE:FALSE;
					SaveMobileConfigFile(&CallConfig);
				}

				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}
			break;
				
		case VK_F5:
			{
				if(hFocus==hCallTimer)
					CallTimerWin();
				else if (hFocus==hAOC)
				{
					//Check if there is a SIM card
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Call"),Notify_Alert,ML("Ok"),NULL,20);
						return FALSE;
					}

					{
						SimService sim;
						GetSimStatus_AOC(&sim);
						if(sim.SupportStatus != Allocated || sim.CurrentStatus != 1 )	
						{
							PLXTipsWin(NULL,NULL,0,ML("Feature not\navailable on SIM"),ML("Advice of charge"),
								Notify_Failure,ML("Ok"),NULL,20);
							return FALSE;
						}
						//two status are one-> service is available.
						ME_GetPricePerUnit(hWnd,ME_MSG_GETUNITCOST);
					}
				}
				else if(hFocus==hAnyKeyAnswer)
				{
					SendMessage(hWnd,WM_COMMAND,IDC_KEYANSWER,0);

					iAnyKey = SendMessage(hAnyKeyAnswer,SSBM_GETCURSEL,0,0);
					AnyKeyAnswerWin(&iAnyKey);
				}
				else if(hFocus==hAltLine)
				{
					if(!GetSIMState())
					{
						PLXTipsWin(NULL,NULL,0,ML("No SIM card"),ML("Call"),Notify_Alert,ML("Ok"),NULL,20);
						return FALSE;
					}

					if(ME_GetSALSstatus(hWnd,ME_MSG_ALS)<0)
						return FALSE;
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
				break;

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
				break;

			}
			break;
	
		}
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;

}

static BOOL CreateSettingListControl(HWND hParentWnd,HWND *hCallTimer,HWND *hAOC,HWND *hAnyKeyAnswer,
									 HWND *hAltLine)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;
    GetClientRect(hParentWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;

    * hCallTimer = CreateWindow( "SPINBOXEX", ML("Call timers"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hParentWnd, (HMENU)NULL, NULL, NULL);

    if (* hCallTimer == NULL)
        return FALSE;

    * hAOC = CreateWindow( "SPINBOXEX", ML("Advice of charge"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hParentWnd, (HMENU)NULL, NULL, NULL);

    if (* hAOC == NULL)
        return FALSE;
    * hAnyKeyAnswer = CreateWindow( "SPINBOXEX", ML("Any key answer"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hParentWnd, (HMENU)IDC_KEYANSWER, NULL, NULL);
	
	SendMessage(* hAnyKeyAnswer,SSBM_ADDSTRING,0,(LPARAM)ML("On"));
	SendMessage(* hAnyKeyAnswer,SSBM_ADDSTRING,0,(LPARAM)ML("Off"));
	SendMessage(* hAnyKeyAnswer,SSBM_SETCURSEL,1,(LPARAM)NULL);

    if (* hAnyKeyAnswer == NULL)
        return FALSE;

     * hAltLine = CreateWindow( "SPINBOXEX", ML("Alt.line service"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*3, iControlW, iControlH, 
        hParentWnd, (HMENU)NULL, NULL, NULL);

    if (* hAltLine == NULL)
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


/*********************************************************************
*
*				2.1.1  Call timers
*
*********************************************************************/
static void CallTimerWin(void)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = CallTimerProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "CallTimer";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"CallTimer",
		"",
		WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		NULL
		);
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
	SetFocus(hWnd); 
}

static LRESULT CallTimerProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hDuringCall,hAfterCall;
	static HWND hFocus;
	int i,j;
	static MBConfig CallConfig;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			
			int     xzero=0,yzero=0,iControlH,iControlW;
			RECT    rect;
			GetClientRect(hWnd, &rect);
			iControlH = rect.bottom/3;
			iControlW = rect.right;

			ReadMobileConfigFile(&CallConfig);
			
			hDuringCall = CreateWindow( "SPINBOXEX", ML("During call"), 
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
				xzero, yzero, iControlW, iControlH, 
				hWnd, (HMENU)NULL, NULL, NULL);
			SendMessage(hDuringCall,SSBM_ADDSTRING,0,(LPARAM)ML("On"));
			SendMessage(hDuringCall,SSBM_ADDSTRING,0,(LPARAM)ML("Off"));

			if(CallConfig.bShowCallDuration)
				SendMessage(hDuringCall,SSBM_SETCURSEL,0,(LPARAM)NULL);
			else
				SendMessage(hDuringCall,SSBM_SETCURSEL,1,(LPARAM)NULL);
			
			hAfterCall = CreateWindow( "SPINBOXEX", ML("After call"), 
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT | CS_NOSYSCTRL,
				xzero, yzero+iControlH, iControlW, iControlH, 
				hWnd, (HMENU)NULL, NULL, NULL);

			SendMessage(hAfterCall,SSBM_ADDSTRING,0,(LPARAM)ML("On"));
			SendMessage(hAfterCall,SSBM_ADDSTRING,0,(LPARAM)ML("Off"));

			if(CallConfig.bCallSummary)
				SendMessage(hAfterCall,SSBM_SETCURSEL,0,(LPARAM)NULL);
			else
				SendMessage(hAfterCall,SSBM_SETCURSEL,1,(LPARAM)NULL);
			
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hSettingFrame,ML("Call timers"));

			hFocus = hDuringCall;
		}
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hSettingFrame,ML("Call timers"));
		SetFocus(hFocus);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("CallTimer",NULL);
        break;

	case WM_VSCROLL:
		DoVScroll(hWnd,wParam);
		break;

	case  WM_DURINGCALL_CHANGED:
		SendMessage(hDuringCall,SSBM_SETCURSEL,wParam,0);
		break;

	case WM_AFTERCALL_CHANGED:
		SendMessage(hAfterCall,SSBM_SETCURSEL,wParam,0);
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			{
				BOOL bDuringCallChanged=TRUE,bAfterCallChanged=TRUE;

				i=SendMessage(hDuringCall,SSBM_GETCURSEL,0,0);
				if( (i==0 && CallConfig.bShowCallDuration ==TRUE)|| (i==1 &&CallConfig.bShowCallDuration==FALSE) )
					bDuringCallChanged=FALSE;
			
				j=SendMessage(hAfterCall,SSBM_GETCURSEL,0,0);
				if( (j==0 &&CallConfig.bCallSummary==TRUE) || (j==1 &&CallConfig.bCallSummary==FALSE))
					bAfterCallChanged=FALSE;
				
				if(bDuringCallChanged || bAfterCallChanged)
				{
					CallConfig.bShowCallDuration=(i==0)?TRUE:FALSE;
					CallConfig.bCallSummary=(j==0)?TRUE:FALSE;
					SaveMobileConfigFile(&CallConfig);
				}

				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}
			break;	
		case VK_F5:
			{
				if(hFocus==hDuringCall)
				{
					i=SendMessage(hDuringCall,SSBM_GETCURSEL,0,0);
					DuringCallWin(&i);
				}
				else if (hFocus==hAfterCall)
				{
					i=SendMessage(hAfterCall,SSBM_GETCURSEL,0,0);
					AfterCallWin(&i);
				}
			}
			break;

		case VK_DOWN:
			{
				if ( hFocus == hDuringCall )
				{
					hFocus = hAfterCall;
					SetFocus(hFocus);
				}
				else
				{
					hFocus = hDuringCall;
					SetFocus(hFocus);
				}
			}
			break;
			
		case VK_UP:
			{
				if ( hFocus == hDuringCall )
				{
					hFocus = hAfterCall;
					SetFocus(hFocus);
				}
				else
				{
					hFocus = hDuringCall;
					SetFocus(hFocus);
				}
			}
			break;
	
		}
		break;        

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static void DuringCallWin(void *info)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = DuringCallProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "DuringCall";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"DuringCall",
		"",
		WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		(PVOID)info
		);
	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
}

static LRESULT DuringCallProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hList;
	static HBITMAP hRadioOn,hRadioOff;
	static int iSelect;
	int iCurrentItem;
	HWND hPrevWin;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			
			RECT rClient;
			HDC hDC=NULL;
			PCREATESTRUCT pCreateData=(PCREATESTRUCT)lParam;
			int i,j;

			memcpy(&i,pCreateData->lpCreateParams,sizeof(int));
			
			GetClientRect(hSettingFrame,&rClient);
			hDC=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_on.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_off.bmp",NULL,NULL);
			ReleaseDC(hWnd,hDC);

			hList=CreateWindow("LISTBOX","",WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)NULL,NULL,NULL);

			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("On"));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Off"));
			for(j=0;j<2;j++)
				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)j),(LPARAM)hRadioOff);

			SendMessage(hList,LB_SETCURSEL,i,0);
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOn);
			
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hSettingFrame,ML("Timer during call"));

			iSelect=i;

		}
        break;

    case WM_SETFOCUS:
        SetFocus(hList);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hSettingFrame,ML("Timer during call"));
		SetFocus(hList);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("DuringCall",NULL);
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	

		case VK_F5:
			{
				iCurrentItem=SendMessage(hList,LB_GETCURSEL,0,0);
				if(iCurrentItem!=iSelect)
				{
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iSelect),(LPARAM)hRadioOff);
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iCurrentItem),(LPARAM)hRadioOn);
					iSelect=iCurrentItem;
				}
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				SendMessage(hPrevWin,WM_DURINGCALL_CHANGED,iSelect,0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);

			}
			break;
		}
		break;        

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static void AfterCallWin(void *info)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = AfterCallProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "AfterCall";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"AfterCall",
		"",
		WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		(PVOID)info
		);
	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
}

static LRESULT AfterCallProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HBITMAP hRadioOn,hRadioOff;
	static HWND hList;
	static int iSelect;
	int iCurrentItem;
	HWND hPrevWin;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			
			RECT rClient;
			HDC hDC=NULL;
			PCREATESTRUCT pCreateData=(PCREATESTRUCT)lParam;
			int i,j;

			memcpy(&i,pCreateData->lpCreateParams,sizeof(int));
			
			hDC=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_on.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_off.bmp",NULL,NULL);
			ReleaseDC(hWnd,hDC);

			GetClientRect(hSettingFrame,&rClient);
			
			hList=CreateWindow("LISTBOX","",WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)NULL,NULL,NULL);
			
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("On"));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Off"));
			SendMessage(hList,LB_SETCURSEL,i,0);
			
			for(j=0;j<2;j++)
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)j),(LPARAM)hRadioOff);
			
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOn);
			iSelect=i;
			
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hSettingFrame,ML("Summary after call"));
			
		}
        break;
		
    case WM_SETFOCUS:
        SetFocus(hList);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hSettingFrame,ML("Summary after call"));
		SetFocus(hList);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("AfterCall",NULL);
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;


	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	
			
		case VK_F5:
			{
				iCurrentItem=SendMessage(hList,LB_GETCURSEL,0,0);
				if(iCurrentItem!=iSelect)
				{
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iSelect),(LPARAM)hRadioOff);
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iCurrentItem),(LPARAM)hRadioOn);
					iSelect=iCurrentItem;
				}
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				SendMessage(hPrevWin,WM_AFTERCALL_CHANGED,iSelect,0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}
			break;
		}
		break;


    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

/***************************************************************************************
*
*							2.1.2 Advice of charge
*
***************************************************************************************/
static void AdviceOfChargeWin(void *info)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = AdviceOfChargeProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "AdviceOfCharge";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"AdviceOfCharge",
		"",
		WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		(PVOID)info
		);
	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
}

static LRESULT AdviceOfChargeProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hList;
	static HWND hFocus;
	int i;
	static AOCINFO AocInfo;
	char strMaxCost[20];

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;

			memcpy(&AocInfo,pCreateStruct->lpCreateParams,sizeof(AocInfo));
			
			GetClientRect(hSettingFrame,&rClient);
			hList=CreateWindow("LISTBOX","",WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP|LBS_MULTILINE,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)NULL,NULL,NULL);

			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Unit cost"));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Maximum cost"));
			SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(0,-1),(LPARAM)AocInfo.UnitCost.ppu);
			sprintf(strMaxCost,"%d",AocInfo.MaxCost);
			SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(1,-1),(LPARAM)strMaxCost);
			SendMessage(hList,LB_SETCURSEL,0,0);
			
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hSettingFrame,ML("Advice of charge"));
		}
        break;

    case WM_SETFOCUS:
        SetFocus(hList);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hSettingFrame,ML("Advice of charge"));
		SetFocus(hList);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("AdviceOfCharge",NULL);
        break;

	case WM_MAXCOST_CHANGED:
		SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(1,-1),lParam);
		break;

	case  WM_UNITCOST_CHANGED: //refresh AocInfo from Unit cost window or max cost window
		SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(0,-1),(LPARAM)wParam);
		break;

	case ME_MSG_PIN2CHECK:
		switch(wParam) 
		{
		case ME_RS_SUCCESS:

			memset(Pin2,0x00,sizeof(Pin2));
			strncpy(Pin2,AocInfo.PIN2,sizeof(AocInfo.PIN2));


			WaitWin(NULL, FALSE, ML("Requesting..."),ML("Advice of charge"),NULL,NULL,0);
			
			printf("\r\nCall Settting : finish the check of PIN2!\r\n");
			bPasswordOK = TRUE;

			i=SendMessage(hList,LB_GETCURSEL,0,0);
			if(i==0)
			{
				printf("\r\nCall Setting : Come into the UnitCostWin! \r\n");
				UnitCostWin(&AocInfo);
			}
			else
				MaxCostWin(&AocInfo);
			break;
			
		default: //Check whether the PIN2 code is error. If true,prompt user input PIN2 again
			//ME_GetCurWaitingPassword(hWnd,ME_MSG_CHECK_PASSWORD);
			WaitWin(NULL,FALSE,ML("Requesting..."),ML("Advice of charge"),NULL,NULL,0);
			PLXTipsWin(hSettingFrame, hWnd, 0, ML("PIN2 code\r\nwrong"), ML("Advice of charge"), Notify_Failure, ML("Ok"), NULL, WAITTIMEOUT);
			break;
		}
		break;

	case WM_CHECKPIN2:
		{
 			switch(wParam) 
			{
			case PIN2_NONBLOCKED:
			case PIN2_CHANGED:
				{
					const char pin_null[8] = "";
					//Firstly,the user have to input an PIN2 code

/*
					if (    ( strcmp(Pin2,pin_null) != 0 ) 
						 && ( strcmp(AocInfo.PIN2,Pin2) == 0 ) )
					{
						PostMessage(hWnd, ME_MSG_PIN2CHECK,ME_RS_SUCCESS,0);
						WaitWin(NULL, TRUE,ML("Requesting..."),ML("Advice of charge"),NULL,NULL,0);
						break;
					}
*/
					
					if ( !bPasswordOK )
					{
						if(!SSPLXVerifyPassword(hWnd,ML("Advice of charge"),ML("Enter PIN2 code:"),AocInfo.PIN2,MAKEWPARAM(4,10),ML("Ok"),ML("Cancel"),-1))
							return FALSE;
						
						ME_PasswordValidation(hWnd, ME_MSG_PIN2CHECK, PIN2, AocInfo.PIN2, NULL);
						WaitWin(NULL, TRUE,ML("Requesting..."),ML("Advice of charge"),NULL,NULL,0);
					}			
					else
					{
						i=SendMessage(hList,LB_GETCURSEL,0,0);
						if(i==0)
						{
							UnitCostWin(&AocInfo);
						}
						else
							MaxCostWin(&AocInfo);
					}
				}
				break;

			case PUK2_BLOCKED:
				break;

			case PUK2_INPUT_EXIT:
				printf("user exit when input PUK2 code\r\n");
				break;

			default:
				break;
 			}
			
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	
			
		case VK_F5:
			{
				//check if PIN2/PUK2 is blocked       BUG FIXED
				CheckPin2Blocked(hSettingFrame,hWnd,WM_CHECKPIN2,ML("Advice of charge"));
				break;
			}
			break;

		}
		break;        

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static void UnitCostWin(void *CostInfo)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = UnitCostProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "UnitCost";

	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"UnitCost",
		"",
		WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		(PVOID)CostInfo
		);

	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
}

static LRESULT UnitCostProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hList;
	static HWND hEdit[2];
	static HWND hFocus;
	static int iSelect;
	HWND hPrevWin;
	static AOCINFO AocInfo;
	static ME_PUCINFO puc;


    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			IMEEDIT ie;
			DWORD dwStyle;
			int Height;
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;

			printf("\r\n Call Setting : start WM_CREATE \r\n");
			printf("\r\n Call Setting : pCreateStruct is %08x \r\n",pCreateStruct);
			
			memcpy(&AocInfo,pCreateStruct->lpCreateParams,sizeof(AocInfo));
			GetClientRect(hWnd,&rClient);
			
			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP|LBS_MULTILINE,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)IDC_UNITCOSTLIST,NULL,NULL);

			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("Cost/unit:"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("Currency:"));
			SendMessage(hList,LB_SETCURSEL,0,0);

			memset(&ie, 0, sizeof(IMEEDIT));
			ie.hwndNotify	= (HWND)hWnd;    
			ie.dwAttrib	    = 0;                
			ie.dwAscTextMax	= 0;
			ie.dwUniTextMax	= 0;
			ie.wPageMax	    = 0;        
			ie.pszCharSet	=NULL; // "1234567890.";
			ie.pszTitle	    = NULL;
			dwStyle = WS_VISIBLE | WS_CHILD|ES_AUTOHSCROLL|WS_TABSTOP|ES_TITLE|CS_NOSYSCTRL;
            ie.pszImeName	= "Digit";

			Height=(rClient.bottom-rClient.top)/3;

			hEdit[0] = CreateWindow("IMEEDIT","",dwStyle,
					rClient.left,rClient.top,rClient.right-rClient.left,Height,
					hWnd,
					(HMENU)IDC_UNITPRICE,
					NULL,
					(PVOID)&ie);

			dwStyle = WS_VISIBLE | WS_CHILD|ES_AUTOHSCROLL|WS_TABSTOP|ES_TITLE|CS_NOSYSCTRL;
            ie.pszImeName	= "ABC";

			hEdit[1] = CreateWindow("IMEEDIT","",dwStyle,
					rClient.left,rClient.top+Height,rClient.right-rClient.left,Height,
					hWnd,
					(HMENU)IDC_CURRENCY,
					NULL,
					(PVOID)&ie);
			
			SendMessage(hEdit[0], EM_SETTITLE, 0, (LPARAM)ML("Cost/Unit:"));
			SendMessage(hEdit[1], EM_SETTITLE, 0, (LPARAM)ML("Currency:"));
			SendMessage(hEdit[0],EM_LIMITTEXT,11,(LPARAM)NULL);
			SendMessage(hEdit[1],EM_LIMITTEXT,3,(LPARAM)NULL);

			iSelect=0; //Set Focus Item is set to 0
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Save"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hSettingFrame,ML("Advice of charge"));
		
		}
        break;

    case WM_SETFOCUS:
        SetFocus(hEdit[iSelect]);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Save"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		SetWindowText(hSettingFrame,ML("Advice of charge"));
		SetFocus(hEdit[iSelect]);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("UnitCost",NULL);
        break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_UNITPRICE:
			{
				switch(HIWORD(wParam))
				{
				case LBN_SETFONT:
					{	
						HFONT hFont = NULL;
						GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
						return (LRESULT)hFont;
					}
				}
			}
			break;

		case IDC_CURRENCY:
			{
				switch(HIWORD(wParam))
				{
				case LBN_SETFONT:
					{
						HFONT hFont = NULL;
						GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
						return (LRESULT)hFont;
					}
				}
			}
			break;

		case IDC_UNITCOSTLIST:
			break;
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			{
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
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
				

				if(iSelect==1)
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
				int /*i,*/len;

				GetWindowText(hEdit[iSelect],buff,sizeof(buff));
				len=strlen(buff);

				if(len != 0)
					memset(buff,'*',len-1);	 //xjdan fixed
//				for(i=0;i<len;i++)
//					buff[i]='*';
				SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(iSelect,-1),(LPARAM)buff);

				if(iSelect==0)
					iSelect=1;
				else
					iSelect--;
				SendMessage(hList,LB_SETCURSEL,iSelect,(LPARAM)0);
				
				SetFocus(hEdit[iSelect]);
				InvalidateRect(hWnd,NULL,FALSE);
			}
			break;
			
		case VK_RETURN:
			{
				char UnitPrice[20];
				char Currency[10];
				
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				GetWindowText(hEdit[0],UnitPrice,sizeof(UnitPrice));
				GetWindowText(hEdit[1],Currency,sizeof(Currency));
				if(strlen(UnitPrice)==0 || atof(UnitPrice)<0.0004095 || atof(UnitPrice)>40950000000)
				{
					PLXTipsWin(NULL,NULL,0,ML("Please define\nvalid cost/unit value"),ML("Advice of charge"),Notify_Alert,ML("Ok"),NULL,20);
					return FALSE;
				}
				
				strncpy(puc.currency,Currency,sizeof(puc.currency));
				strncpy(puc.ppu,UnitPrice,sizeof(puc.ppu));
				//SendMessage(hPrevWin,WM_UNITCOST_CHANGED,(WPARAM)UnitPrice,(LPARAM)Currency);
				if ( bPasswordOK )
				{
					if ( ME_SetPricePerUnit(hWnd,ME_MSG_SETUNITCOST,&puc,Pin2) < 0 )
					{
						PostMessage(hWnd,WM_CLOSE,0,0);
						SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
						return FALSE;
					}
				}
				else
				{
					if(ME_SetPricePerUnit(hWnd,ME_MSG_SETUNITCOST,&puc,AocInfo.PIN2)<0)
					{
						PostMessage(hWnd,WM_CLOSE,0,0);
						SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
						return FALSE;
					}
				}
				
			}
//			PostMessage(hWnd,WM_CLOSE,0,0);
//			SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;
		}
		break;  
		
	case ME_MSG_SETUNITCOST:
		switch(wParam) 
		{
		case ME_RS_SUCCESS:
			hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
			SendMessage(hPrevWin,WM_UNITCOST_CHANGED,(WPARAM)puc.ppu,(LPARAM)puc.currency);
			printf("Set Unit cost success\r\n");
			break;

		default: //Check whether the PIN2 code is error. If true,prompt user input PIN2 again
			//ME_GetCurWaitingPassword(hWnd,ME_MSG_CHECK_PASSWORD);
			printf("Set Unit cost failure\r\n");
			break;
		}
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;

	default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static void MaxCostWin(void *info)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = MaxCostProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; // GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "MaxCost";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"MaxCost",
		"",
		WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		(PVOID)info
		);
	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
}

static LRESULT MaxCostProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hEdit;
	static AOCINFO AocInfo;
	HWND hPrevWin;
	static char strMaxCost[9];

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			IMEEDIT ie;
			DWORD dwStyle;
			int Height;
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;

			memcpy(&AocInfo,pCreateStruct->lpCreateParams,sizeof(AocInfo));

			GetClientRect(hWnd,&rClient);
			
			memset(&ie, 0, sizeof(IMEEDIT));
			ie.hwndNotify	= (HWND)hWnd;    
			ie.dwAttrib	    = 0;                
			ie.dwAscTextMax	= 0;
			ie.dwUniTextMax	= 0;
			ie.wPageMax	    = 0;        
			ie.pszCharSet	=NULL; // "1234567890.";
			ie.pszTitle	    = NULL;
			dwStyle = WS_VISIBLE | WS_CHILD|ES_AUTOHSCROLL|WS_TABSTOP|ES_TITLE|CS_NOSYSCTRL|ES_NUMBER;
            ie.pszImeName	= "Digit";

			Height=(rClient.bottom-rClient.top)/3;


			hEdit = CreateWindow("IMEEDIT","",dwStyle,
					rClient.left,rClient.top,rClient.right-rClient.left,Height,
					hWnd,
					(HMENU)IDC_MAXCOST,
					NULL,
					(PVOID)&ie);
		
			SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)ML("Maximum cost (EUR):"));
			SendMessage(hEdit, EM_LIMITTEXT,8,(LPARAM)NULL);

			//iSelect=0; //Set Focus Item is set to 0
	  		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
 			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Save"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hSettingFrame,ML("Advice of charge"));
		
		}
        break;

    case WM_SETFOCUS:
        SetFocus(hEdit);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Save"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		SetWindowText(hSettingFrame,ML("Advice of charge"));
		SetFocus(hEdit);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("MaxCost",NULL);
        break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_MAXCOST:
			{
				switch(HIWORD(wParam))
				{
				case LBN_SETFONT:
					{	
						HFONT hFont = NULL;
						GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
						return (LRESULT)hFont;
					}

				}
			}
			break;
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		 	break;	

		case VK_RETURN:
			{
				GetWindowText(hEdit,strMaxCost,sizeof(strMaxCost));
				if ( ( strlen(strMaxCost) == 0 ) || ( atoi(strMaxCost) > 16777214 ) )
				{
					PLXTipsWin(NULL,NULL,0,ML("Please define valid limit value"),NULL,Notify_Alert,
						ML("Ok"),NULL,20);
					return FALSE;
				}
				
				if ( bPasswordOK )
				{
					if ( ME_SetACMmax(hWnd,ME_MSG_SETMAXCOST,atoi(strMaxCost),Pin2) < 0 )
					{
						PostMessage(hWnd,WM_CLOSE,0,0);
						SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
						return FALSE;
					}
				}
				else
				{
					if(ME_SetACMmax(hWnd,ME_MSG_SETMAXCOST,atoi(strMaxCost),AocInfo.PIN2)<0)
					{
						PostMessage(hWnd,WM_CLOSE,0,0);
						SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
						return FALSE;
					}
				}
			}
			break;
		}
		break;  

	case ME_MSG_SETMAXCOST:
		switch(wParam) 
		{
		case ME_RS_SUCCESS:
			printf("Set Max Cost succss\r\n");
			hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
			SendMessage(hPrevWin,WM_MAXCOST_CHANGED,0,(LPARAM)strMaxCost);
			break;

		default:
			printf("Set Max Cost failure\r\n");
			break;
		}
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static void AnyKeyAnswerWin(void *info)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = AnyKeyAnswerProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "AnyKeyAnswer";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"AnyKeyAnswer",
		"",
		WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		(PVOID)info
		);
	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
}

static LRESULT AnyKeyAnswerProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hList;
	static HBITMAP hRadioOn,hRadioOff;
	static int iSelect;
	int iCurrentItem;
	HWND hPrevWin;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			
			RECT rClient;
			HDC hDC=NULL;
			PCREATESTRUCT pCreateData=(PCREATESTRUCT)lParam;
			int i,j;

			memcpy(&i,pCreateData->lpCreateParams,sizeof(int));
			
			GetClientRect(hSettingFrame,&rClient);
			
			hDC=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_on.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_off.bmp",NULL,NULL);
			ReleaseDC(hWnd,hDC);

			hList=CreateWindow("LISTBOX","",WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)NULL,NULL,NULL);

			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("On"));
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Off"));
			for(j=0;j<2;j++)
				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)j),(LPARAM)hRadioOff);

			SendMessage(hList,LB_SETCURSEL,i,0);
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)i),(LPARAM)hRadioOn);
			
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hSettingFrame,ML("Any key answer"));

			iSelect=i;
		}
        break;

    case WM_SETFOCUS:
        SetFocus(hList);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hSettingFrame,ML("Any key answer"));
		SetFocus(hList);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("AnyKeyAnswer",NULL);
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;
			
		case VK_F5:
			{
				iCurrentItem=SendMessage(hList,LB_GETCURSEL,0,0);
				if(iCurrentItem!=iSelect)
				{
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iSelect),(LPARAM)hRadioOff);
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iCurrentItem),(LPARAM)hRadioOn);
					iSelect=iCurrentItem;
				}
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				SendMessage(hPrevWin,WM_ANYKEYANSWER_CHANGED,iSelect,0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);

			}
			break;
		}
		break;        

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static void ALSWin(void *ALSInfo)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = ALSProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "ALS";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"ALS",
		"",
		WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		(PVOID)ALSInfo
		);
	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
}

static LRESULT ALSProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hOutgoing,hLine1,hLine2;
	static HWND hFocus;
	int i;
	static LINEINFO LineInfo;
	static MBConfig CallConfig;
	
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			int     xzero=0,yzero=0,iControlH,iControlW;
			RECT    rect;
			PCREATESTRUCT pCreateData=(PCREATESTRUCT)lParam;
			//char Line1Name[100],Line2Name[100];

			GetClientRect(hWnd, &rect);
			iControlH = rect.bottom/3;
			iControlW = rect.right;
			
			ReadMobileConfigFile(&CallConfig);

			//Read Data from SIM card.
			memcpy(&LineInfo,pCreateData->lpCreateParams,sizeof(LineInfo));
			
			if(LineInfo.iCurrentLine !=CallConfig.ALS)
			{
				CallConfig.ALS=LineInfo.iCurrentLine; //Active line read from ME if it not equal to config,
				SaveMobileConfigFile(&CallConfig);    //update the ALS in config file
			}

			strncpy(LineInfo.Line1Name,CallConfig.Line1Name,MAX_LINENAME);
			strncpy(LineInfo.Line2Name,CallConfig.Line2Name,MAX_LINENAME);

			hOutgoing = CreateWindow( "SPINBOXEX", ML("Outgoing line"), 
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST |/* WS_BORDER |*/ WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
				xzero, yzero, iControlW, iControlH, 
				hWnd, (HMENU)NULL, NULL, NULL);
			SendMessage(hOutgoing,SSBM_ADDSTRING,0,(LPARAM)LineInfo.Line1Name);
			SendMessage(hOutgoing,SSBM_ADDSTRING,0,(LPARAM)LineInfo.Line2Name);
			if(CallConfig.ALS==ALS_LINE1)
				SendMessage(hOutgoing,SSBM_SETCURSEL,0,(LPARAM)NULL);
			else
				SendMessage(hOutgoing,SSBM_SETCURSEL,1,(LPARAM)NULL);
			
			hLine1 = CreateWindow( "SPINBOXEX", ML("Line 1 name"), 
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
				xzero, yzero+iControlH, iControlW, iControlH, 
				hWnd, (HMENU)NULL, NULL, NULL);
			SendMessage(hLine1,SSBM_ADDSTRING,0,(LPARAM)LineInfo.Line1Name);
			SendMessage(hLine1,SSBM_SETCURSEL,0,(LPARAM)NULL);

			
			hLine2 = CreateWindow( "SPINBOXEX", ML("Line 2 name"), 
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | /*WS_BORDER | */WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
				xzero, yzero+2*iControlH, iControlW, iControlH, 
				hWnd, (HMENU)NULL, NULL, NULL);
			SendMessage(hLine2,SSBM_ADDSTRING,0,(LPARAM)LineInfo.Line2Name);
			SendMessage(hLine2,SSBM_SETCURSEL,0,(LPARAM)NULL);
			
			
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hSettingFrame,ML("Alt. line service"));

			hFocus = hOutgoing;
//			InvalidateRect(hWnd,&rect,FALSE);
		}
        break;
		
    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Back"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hSettingFrame,ML("Alt. line service"));
		SetFocus(hFocus);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("ALS",NULL);
        break;

	case ME_MSG_GETCURRENTLINE:
		switch(wParam)
		{
		case ME_RS_SUCCESS:
			{
				int result;
				ME_GetResult(&result,sizeof(int));
				
				if(result!=1 && result !=2)
					result=1;
				
				iOutLine = result;
			}
			break;

		default:
			break;
		}
		break;


	case  WM_ACTIVELINE_CHANGED:
		{
			SendMessage(hOutgoing,SSBM_SETCURSEL,wParam,0);
			SetCurrentAltLine((int)wParam + 1);
		}
		break;

	case  WM_LINENAME_CHANGED:
		{
			LINEINFO *pReturnInfo=(LINEINFO*)lParam;
			if(pReturnInfo->iCurrentLine==0)
			{
				strncpy(CallConfig.Line1Name,pReturnInfo->Line1Name,MAX_LINENAME);
				strncpy(LineInfo.Line1Name,pReturnInfo->Line1Name,MAX_LINENAME);
				SendMessage(hLine1,SSBM_SETTEXT,0,(LPARAM)pReturnInfo->Line1Name);
				SendMessage(hOutgoing,SSBM_SETTEXT,0,(LPARAM)pReturnInfo->Line1Name);
				SaveMobileConfigFile(&CallConfig);
			}
			else
			{
				strncpy(CallConfig.Line2Name,pReturnInfo->Line2Name,MAX_LINENAME);
				strncpy(LineInfo.Line2Name,pReturnInfo->Line2Name,MAX_LINENAME);
				SendMessage(hLine2,SSBM_SETTEXT,0,(LPARAM)pReturnInfo->Line2Name);
				SendMessage(hOutgoing,SSBM_SETTEXT,1,(LPARAM)pReturnInfo->Line2Name);
				SaveMobileConfigFile(&CallConfig);
			}

		}
		break;

	case ME_MSG_SELECT_LINE:
		switch(wParam) 
		{
		case ME_RS_SUCCESS:
			printf("Select outgoing line success\r\n");
			if(ME_GetSALSstatus(hWnd,ME_MSG_GETCURRENTLINE)<0)
			{
				return FALSE;
			}
			i=SendMessage(hOutgoing,SSBM_GETCURSEL,0,0);
			CallConfig.ALS = i;
			SaveMobileConfigFile(&CallConfig);
			break;

		default:
			break;
		}
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			{
				i=SendMessage(hOutgoing,SSBM_GETCURSEL,0,0);
				if( i!=CallConfig.ALS)
				{
					if(CallConfig.ALS==ALS_LINE1)
					{
						if(ME_SelectOutLine(hWnd,ME_MSG_SELECT_LINE,2,0)<0)
						{
							PostMessage(hWnd,WM_CLOSE,0,0);
							SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
						}
					}
					else
					{
						if(ME_SelectOutLine(hWnd,ME_MSG_SELECT_LINE,1,0)<0)
						{
							PostMessage(hWnd,WM_CLOSE,0,0);
							SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
						}
					}

					SetCurrentAltLine(i+1);
					return TRUE;
				}
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}		
			break;	

		case VK_F5:
			{
				if(hFocus==hOutgoing)
				{
					i=SendMessage(hOutgoing,SSBM_GETCURSEL,0,0);
					LineInfo.iCurrentLine=i;
					OutgoingLineWin(&LineInfo);
				}
				else if (hFocus==hLine1)
				{
					i=SendMessage(hLine1,SSBM_GETCURSEL,0,0);
					LineInfo.iCurrentLine=0;
					LineWin(&LineInfo);
				}
				else if (hFocus==hLine2)
				{
					i=SendMessage(hLine2,SSBM_GETCURSEL,0,0);
					LineInfo.iCurrentLine=1;
					LineWin(&LineInfo);

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
			}
			break;
	
		}
		break;        

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static void OutgoingLineWin(void *info)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = OutgoingLineProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "OutgoinLine";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"OutgoinLine",
		"",
		WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		(PVOID)info
		);
	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
}

static LRESULT OutgoingLineProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hList;
	static HBITMAP hRadioOn,hRadioOff;
	static int iSelect;
	int iCurrentItem;
	HWND hPrevWin;
	static LINEINFO LineInfo;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			
			RECT rClient;
			HDC hDC=NULL;
			PCREATESTRUCT pCreateData=(PCREATESTRUCT)lParam;
			int /*i,*/j;

			memcpy(&LineInfo,pCreateData->lpCreateParams,sizeof(LineInfo));
			
			GetClientRect(hSettingFrame,&rClient);
			hDC=GetDC(hWnd);
			hRadioOn=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_on.bmp",NULL,NULL);
			hRadioOff=CreateBitmapFromImageFile(hDC,"/rom/network/radio_button_off.bmp",NULL,NULL);
			ReleaseDC(hWnd,hDC);

			hList=CreateWindow("LISTBOX","",WS_VISIBLE|WS_CHILD|/*WS_BORDER|*/WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hWnd,(HMENU)NULL,NULL,NULL);

			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)LineInfo.Line1Name);
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)LineInfo.Line2Name);
			for(j=0;j<2;j++)
				SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)j),(LPARAM)hRadioOff);

			SendMessage(hList,LB_SETCURSEL,LineInfo.iCurrentLine,0);
//			SendMessage(hList,LB_SETCURSEL,LineInfo.iCurrentLine,0);
			SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)LineInfo.iCurrentLine),(LPARAM)hRadioOn);
			
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			SetWindowText(hSettingFrame,ML("Outgoing line"));

			iSelect=LineInfo.iCurrentLine;

		}
        break;

    case WM_SETFOCUS:
        SetFocus(hList);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hSettingFrame,ML("Outgoing line"));
		SetFocus(hList);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
		DeleteObject(hRadioOn);
		DeleteObject(hRadioOff);
        break;

    case WM_DESTROY :
        UnregisterClass("OutgoinLine",NULL);
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;	
		case VK_F5:
			{
				iCurrentItem=SendMessage(hList,LB_GETCURSEL,0,0);
				if(iCurrentItem!=iSelect)
				{
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iSelect),(LPARAM)hRadioOff);
					SendMessage(hList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)iCurrentItem),(LPARAM)hRadioOn);
					iSelect=iCurrentItem;
				}
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				SendMessage(hPrevWin,WM_ACTIVELINE_CHANGED,iSelect,0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);

			}
			break;

		}
		break;        

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

void LineWin(void *info)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = LineProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH); Changed for transparent
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "Lines";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hSettingFrame,&rClient);
	
	hWnd = CreateWindow(
		"Lines",
		"",
		WS_VISIBLE | WS_CHILD /*|WS_VSCROLL*/,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hSettingFrame,
		NULL,
		NULL,
		(PVOID)info
		);
	SetFocus(hWnd); 
	ShowWindow(hWnd, SW_SHOW); 
	UpdateWindow(hWnd);  
}

static LRESULT LineProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	static HWND hEdit;
	static LINEINFO LineInfo;
	static HWND hPrevWin;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			IMEEDIT ie;
			DWORD dwStyle;
			int Height;
			PCREATESTRUCT pCreateData=(PCREATESTRUCT)lParam;
			
			memcpy(&LineInfo,pCreateData->lpCreateParams,sizeof(LINEINFO));

			GetClientRect(hWnd,&rClient);
			
			memset(&ie, 0, sizeof(IMEEDIT));
			ie.hwndNotify	= (HWND)hWnd;    
			ie.dwAttrib	    = 0;                
			ie.dwAscTextMax	= 0;
			ie.dwUniTextMax	= 0;
			ie.wPageMax	    = 0;        
			ie.pszCharSet	= NULL; // "1234567890.";
			ie.pszTitle	    = NULL;
			dwStyle = WS_VISIBLE | WS_CHILD|ES_AUTOHSCROLL|WS_TABSTOP|ES_TITLE|CS_NOSYSCTRL;
            ie.pszImeName	= NULL;

			Height=(rClient.bottom-rClient.top)/3;
			hEdit = CreateWindow("IMEEDIT","",dwStyle,
					rClient.left,rClient.top,rClient.right-rClient.left,Height,
					hWnd,
					(HMENU)IDC_LINENAMEEDIT,
					NULL,
					(PVOID)&ie);

			if(LineInfo.iCurrentLine==0)
			{
				memset(OldLine1Name, 0x00, MAX_LINENAME);
				SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)ML("Line1 name:"));
				SetWindowText(hEdit,LineInfo.Line1Name);
				strcpy(OldLine1Name,LineInfo.Line1Name);
				SendMessage(hEdit, EM_SETSEL, -1, -1);
			}
			else
			{
				memset(OldLine2Name, 0x00, MAX_LINENAME);
				SendMessage(hEdit, EM_SETTITLE, 0, (LPARAM)ML("Line2 name:"));
				SetWindowText(hEdit,LineInfo.Line2Name);
				strcpy(OldLine2Name,LineInfo.Line2Name);
				SendMessage(hEdit, EM_SETSEL, -1, -1);
			}

			SendMessage(hEdit,EM_LIMITTEXT,sizeof(LineInfo.Line1Name)-1,(LPARAM)NULL);

	  		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
 			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Save"));
			SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hSettingFrame,ML("Alt.line service"));
		}
        break;

		
    case WM_SETFOCUS:
        SetFocus(hEdit);
        break;

	case PWM_SHOWWINDOW:
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Save"));
		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		SetWindowText(hSettingFrame,ML("Alt.line service"));
		SetFocus(hEdit);
		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("Lines",NULL);
        break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_LINENAMEEDIT:
			{
				switch(HIWORD(wParam))
				{
				case LBN_SETFONT:
					{	
						HFONT hFont = NULL;
						GetFontHandle(&hFont, (lParam == 0) ? SMALL_FONT : LARGE_FONT);
						return (LRESULT)hFont;
					}

				}
			}
			break;
		}
		break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;
			
		case VK_F5:
			break;

		case VK_RETURN:
			{
				char buffer[100];
				GetWindowText(hEdit,buffer,sizeof(buffer));

				if ( strcmp(buffer, "") == 0 )
				{
					if(LineInfo.iCurrentLine==0)
						//strncpy(LineInfo.Line1Name, OldLine1Name, sizeof(LineInfo.Line1Name));
						strncpy(LineInfo.Line1Name, ML("Line 1"), sizeof(LineInfo.Line1Name));
					else
						//strncpy(LineInfo.Line2Name, OldLine2Name, sizeof(LineInfo.Line2Name));
						strncpy(LineInfo.Line2Name, ML("Line 2"), sizeof(LineInfo.Line2Name));
				}
				else
				{
					if(LineInfo.iCurrentLine==0)
						strncpy(LineInfo.Line1Name,buffer,sizeof(LineInfo.Line1Name));
					else
						strncpy(LineInfo.Line2Name,buffer,sizeof(LineInfo.Line2Name));
				}

				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				SendMessage(hPrevWin,WM_LINENAME_CHANGED,0,(LPARAM)&LineInfo);

			}
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hSettingFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;

		}
		break;        

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;

}
/**************************************************************************************
*
*							extern interface for xge
*
***************************************************************************************/
#define  ME_MSG_GETCURRENTLINE		(WM_USER+0x100)

static LRESULT GetCurrentAltLineProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

BOOL SetCurrentAltLine(int iOutline2)
{
	if ( iOutline2 != 1 && iOutline2 != 2 )
		return FALSE;

	iOutLine = iOutline2;

	GetCurrentAltWin();
	return TRUE;
}

int GetCurrentAltLine()
{
	return iOutLine;
}

int GetCurrentAltWin(void)
{
//	int result;
    WNDCLASS wc;
	HWND	hWnd;
	
    wc.style            = CS_OWNDC;
    wc.lpfnWndProc      = GetCurrentAltLineProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "GetCurrentAltLine";
    wc.hbrBackground    = NULL;

    if (!RegisterClass(&wc))
        return 1;

    hWnd = CreateWindow("GetCurrentAltLine","",
		       0/*WS_POPUP*/,
			   0,0,0,0,
		       NULL,
			   NULL,
			   NULL,
			   NULL);

	if (!hWnd) 
	{
		return 1;
	}

	//MessageLoop.
/*
    while (TRUE)
    {
        GetMessage(&msg, 0, 0, 0);

        if (msg.hwnd == hWnd && (msg.message == WM_CLOSE))
		{
			result=msg.wParam;
			DestroyWindow(hWnd);
            break;
		}
		
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
*/

/*
			if(result!=1 && result !=2)
				result=1;
*/
		
	// result=1------line 1,  result=2------line 2
	if ( iOutLine != 1 && iOutLine != 2 )
		return 1;
	
	return iOutLine; 
}

static LRESULT GetCurrentAltLineProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	int lResult;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			if(ME_GetSALSstatus(hWnd,ME_MSG_GETCURRENTLINE)<0)
			{
				PostMessage(hWnd,WM_CLOSE,-1,0);
				return FALSE;
			}
		}
        break;

	case ME_MSG_GETCURRENTLINE:
		switch(wParam)
		{
		case ME_RS_SUCCESS:
			{
				int result;
				ME_GetResult(&result,sizeof(int));

				if(result!=1 && result !=2)
					result=1;
				
				iOutLine = result;
				PostMessage(hWnd,WM_CLOSE,result,0);
			}
			break;

		default:
			PostMessage(hWnd,WM_CLOSE,-1,0);
			break;
		}
		break;

	case WM_CLOSE:
		UnregisterClass("GetCurrentAltLine",NULL);
		DestroyWindow(hWnd);
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/*********************************************************************************
*
*			CheckPin2Blocked   --interface provided to lanlan and axli
*
*********************************************************************************/
void CheckPin2Blocked(HWND hFrameWin,HWND hParentWnd,UINT message,const char *szCaption)
{
	WNDCLASS wc;
 	RECT rClient;
    DWORD dwRet = TRUE;
	HWND hWnd;
	CHECKPIN2INFO CheckPin2Info;

	CheckPin2Info.hFrameWin=hFrameWin;
	CheckPin2Info.hParentWnd=hParentWnd;
	CheckPin2Info.message =message;
	strncpy(CheckPin2Info.szCaption,szCaption,sizeof(CheckPin2Info.szCaption));
	
	
	wc.style         = 0; 
	wc.lpfnWndProc   = CheckPin2BlockedProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(CHECKPIN2INFO);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; 
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "CheckPin2Blocked";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hFrameWin,&rClient);
	
	hWnd = CreateWindow(
		"CheckPin2Blocked",
		"",
		/*WS_VISIBLE |*/ WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hFrameWin,
		NULL,
		NULL,
		(PVOID)&CheckPin2Info
		);
	SetFocus(hWnd); 
//	ShowWindow(hWnd, SW_SHOW); 
//	UpdateWindow(hWnd);  
}


static LRESULT CheckPin2BlockedProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	CHECKPIN2INFO *pCheckPin2Info;
	static int iInputPUK2Num;
	static BOOL bInputPUK2;
	static PIN2_PUK2_FSM FSM;
	static BOOL bFirstCheckPUK2;

    lResult = (LRESULT)TRUE;
	pCheckPin2Info=GetUserData(hWnd);

    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;

			memcpy(pCheckPin2Info,pCreateStruct->lpCreateParams,sizeof(CHECKPIN2INFO));

			FSM=FSM_INIT;
			bFirstCheckPUK2=TRUE;
			ME_GetCurPasswordStatus(hWnd,ME_MSG_GETPASSWORDSTATUS);

		}
        break;
	
	case ME_MSG_GETPASSWORDSTATUS:
		{
			int result;
			ME_GetResult(&result,sizeof(result));
			if(result==PUK2)
			{	
				printf("PIN2 blocked\r\n");
				if(ME_GetPassInputLimit(hWnd,ME_MSG_ISPUK2BLOCKED,PUK2)<0)
				{
					PostMessage(hWnd,WM_CLOSE,0,0);
					SendMessage(pCheckPin2Info->hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
					return FALSE;
				}
			}

			else
			{
				printf("PIN2 not blocked\r\n");
				PostMessage(pCheckPin2Info->hParentWnd,pCheckPin2Info->message,PIN2_NONBLOCKED,0); //The parent window will go on process
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(pCheckPin2Info->hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
			}
			
		}
		break;


	case ME_MSG_ISPUK2BLOCKED:
		switch(wParam)
		{
		case ME_RS_SUCCESS:
			{
// 				int result;

				ME_GetResult(/*&result*/&iInputPUK2Num,sizeof(/*result*/iInputPUK2Num));

				if(/*result*/iInputPUK2Num==0) //PUK2 block
				{
					FSM=PUK2_BLOCKED;
					PLXTipsWin(NULL,NULL,0,ML("PUK2 blocked"),NULL/*pCheckPin2Info->szCaption*/,Notify_Failure,
						ML("Ok"),NULL,0);

					PostMessage(pCheckPin2Info->hParentWnd,pCheckPin2Info->message,PUK2_BLOCKED,0); //wParam means PUK2 blocked
					PostMessage(hWnd,WM_CLOSE,0,0);
					SendMessage(pCheckPin2Info->hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
				}
				else
				{
					FSM=PIN2_BLOCKED;
					if(bFirstCheckPUK2)
					{
						bFirstCheckPUK2=FALSE;
						PLXTipsWin(GetParent(hWnd),hWnd,WM_USERSELECTOK,ML("PIN2 blocked"),NULL/*pCheckPin2Info->szCaption*/,Notify_Failure,
						ML("Ok"),NULL,0);
					}
					else
						PostMessage(hWnd,WM_USERSELECTOK,0,0);
				}

			}
			break;
		default:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(pCheckPin2Info->hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
			break;
		}
		break;

		case WM_USERSELECTOK:

		if(SSPLXVerifyPassword(hWnd,NULL,ML("Enter PUK2 code:"),Puk2Pin2Code.PUK2,MAKEWPARAM(4,8),ML("Ok"),ML("Cancel"),/*-1*/iInputPUK2Num))
		{
			memcpy(pCheckPin2Info->Puk2Pin2Code.PUK2,&Puk2Pin2Code,sizeof(Puk2Pin2Code));
			InputPin2Code(pCheckPin2Info->hFrameWin,hWnd,/*&Puk2Pin2Code*/pCheckPin2Info);
		}
		else
		{
			PostMessage(pCheckPin2Info->hParentWnd,pCheckPin2Info->message,PUK2_INPUT_EXIT,0);  //user exit when inputing PUK2
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(pCheckPin2Info->hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
			
		}
		break;

		case WM_PUK2ERROR: //User input PUK2 error,input again or PUK2 block
			if(ME_GetPassInputLimit(hWnd,ME_MSG_ISPUK2BLOCKED,PUK2)<0)
			{
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(pCheckPin2Info->hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0);
				return FALSE;
			}
			break;
			
    case WM_SETFOCUS:
        //SetFocus(hEdit);
        break;

//	case PWM_SHOWWINDOW:
//		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
//		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Save"));
//		SendMessage(hSettingFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
//		SetWindowText(hSettingFrame,ML("Alt.line service"));
//		SetFocus(hEdit);
//		break;

    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;

    case WM_DESTROY :
        UnregisterClass("CheckPin2Blocked",NULL);
        break;


//	case WM_KEYDOWN:
//		switch(LOWORD(wParam))
//		{
//
//		}
//		break;        

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

#define  MAX_PIN2_LENGTH   8
#define  MIN_PIN2_LENGTH   4
#define  IDC_NEWPIN2CODE_LIST		0x200
#define  IDC_NEWCODE_EDIT			0x201
#define  IDC_REPEATNEWCODE_EDIT		0x202
static LRESULT InputPin2CodeProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

static void InputPin2Code(HWND hFrameWin,HWND hParentWnd,void *info)
{

	HWND hWnd;
	RECT rClient;
	WNDCLASS wc;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = InputPin2CodeProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "InputPin2Code";
	if(!RegisterClass(&wc))
		return ;
	
	GetClientRect(hFrameWin,&rClient);
	hWnd=CreateWindow("InputPin2Code","",WS_VISIBLE | WS_CHILD ,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
        hFrameWin,
		NULL,
		NULL,
		(PVOID)info);
	

	SetFocus(hWnd);
	ShowWindow(hFrameWin,SW_SHOW);
	UpdateWindow(hFrameWin);
}

static LRESULT InputPin2CodeProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;
	HWND hFrameWin;
	static HWND hList,hEdit[2];
	static int iSelected;
	static HWND hFocus;
	static CHECKPIN2INFO CheckPin2Info;

	hFrameWin=GetParent(hWnd);
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			RECT rClient;
			IMEEDIT ie;
			DWORD dwStyle;
			int height;
			PCREATESTRUCT pCreateStruct=(PCREATESTRUCT)lParam;

			memcpy(&CheckPin2Info,pCreateStruct->lpCreateParams,sizeof(CheckPin2Info));

			iSelected=0;

			GetClientRect(hWnd,&rClient);
			height=(rClient.bottom-rClient.top)/3;
			
			hList=CreateWindow("LISTBOX","",
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP|LBS_MULTILINE,
				rClient.left,rClient.top,rClient.right-rClient.left,height,
				hWnd,(HMENU)IDC_NEWPIN2CODE_LIST,NULL,NULL);

			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("Enter new code"));
			SendMessage(hList,LB_ADDSTRING,-1,(LPARAM)ML("Repeat new code"));
			SendMessage(hList,LB_SETCURSEL,0,0);

			memset(&ie, 0, sizeof(IMEEDIT));
			ie.hwndNotify	= (HWND)hWnd;    
			ie.dwAttrib	    = 0;                
			ie.dwAscTextMax	= 0;
			ie.dwUniTextMax	= 0;
			ie.wPageMax	    = 0;        
			ie.pszCharSet	= NULL;
			ie.pszTitle	    = NULL;
			dwStyle = WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | WS_TABSTOP | ES_TITLE | CS_NOSYSCTRL | ES_NUMBER |ES_PASSWORD;
			ie.pszImeName	= "Phone";
			
			hEdit[0] = CreateWindow("IMEEDIT","",dwStyle,
				rClient.left,rClient.top,rClient.right-rClient.left,(rClient.bottom-rClient.top)/3,
				hWnd,
				(HMENU)IDC_NEWCODE_EDIT,
				NULL,
				(PVOID)&ie);
			SendMessage(hEdit[0], EM_LIMITTEXT, (WPARAM)MAX_PIN2_LENGTH, (LPARAM)NULL); 
			SendMessage(hEdit[0], EM_SETTITLE, 0, (LPARAM)ML("Enter new code"));	///fixed by xjdan  BUG3-- 88

			hEdit[1] = CreateWindow("IMEEDIT","",dwStyle,
				rClient.left,rClient.top+height,rClient.right-rClient.left,(rClient.bottom-rClient.top)/3,
				hWnd,
				(HMENU)IDC_REPEATNEWCODE_EDIT,
				NULL,
				(PVOID)&ie);
			SendMessage(hEdit[1], EM_LIMITTEXT, (WPARAM)MAX_PIN2_LENGTH, (LPARAM)NULL); 
			SendMessage(hEdit[1], EM_SETTITLE, 0, (LPARAM)ML("Repeat new code"));	///fixed by xjdan  BUG3-- 88
			
			SendMessage(hFrameWin,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
 			SendMessage(hFrameWin,PWM_SETBUTTONTEXT,1,(LPARAM)ML(""));
			SendMessage(hFrameWin,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hFrameWin,ML("PIN2 code"));

			iSelected=0;
			hFocus=hEdit[0];
		}
        break;

	case WM_COMMAND:
		switch(LOWORD(wParam)) 
		{
		case IDC_NEWPIN2CODE_LIST:
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
					SetFocus(GetDlgItem(hWnd, IDC_NEWPIN2CODE_LIST));
					break;

    			default:
					break;
					
				}	
			}
			
		case IDC_NEWCODE_EDIT:
		case IDC_REPEATNEWCODE_EDIT:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				GetWindowText(hEdit[0],buffer1,sizeof(buffer1));
				GetWindowText(hEdit[1],buffer2,sizeof(buffer2));
				if(strlen(buffer1)>=4 &&strlen(buffer2)>=4)
					SendMessage(hFrameWin,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
				else
					SendMessage(hFrameWin,PWM_SETBUTTONTEXT,1,(LPARAM)"");

			}
			break;
		}//WM_COMMAND
		break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;
		
	case PWM_SHOWWINDOW:
	  		SendMessage(hFrameWin,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
			{
				char buffer1[10],buffer2[10];
				GetWindowText(hEdit[0],buffer1,sizeof(buffer1));
				GetWindowText(hEdit[1],buffer2,sizeof(buffer2));
				if(strlen(buffer1)>=4 &&strlen(buffer2)>=4)
					SendMessage(hFrameWin,PWM_SETBUTTONTEXT,1,(LPARAM)ML("Ok"));
				else
					SendMessage(hFrameWin,PWM_SETBUTTONTEXT,1,(LPARAM)ML(""));
			}
			
			SendMessage(hFrameWin,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			SetWindowText(hFrameWin,ML("PIN2 code"));
			SetFocus(hFocus);
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass("InputPin2Code",NULL);
        break;
		
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			{
				HWND hPrevWin;
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				PostMessage(hWnd,WM_CLOSE,0,0);
				PostMessage(hPrevWin,WM_CLOSE,0,0);
				SendMessage(hFrameWin, PWM_CLOSEWINDOW, (WPARAM) hPrevWin, (LPARAM)0);
			}
			break;	

		case VK_RETURN:
			{
//				char buffer1[10],buffer2[10];
				GetWindowText(hEdit[0],buffer1,sizeof(buffer1));
				GetWindowText(hEdit[1],buffer2,sizeof(buffer2));

				if(strlen(buffer1)<4 || strlen(buffer2)<4)
					return FALSE;
				
				if(strcmp(buffer1,buffer2)!=0)
					PLXTipsWin(NULL,NULL,0,ML("Codes do\nnot match"),ML("PIN2 code"),Notify_Failure,ML("Ok"),NULL,0);
				else
				{
					//strncpy(Puk2_Pin2_code.PIN2,buffer1,sizeof(Puk2_Pin2_code.PIN2));
					ME_PasswordValidation(hWnd,ME_MSG_VERIFYPUK2,PUK2,CheckPin2Info.Puk2Pin2Code.PUK2,buffer1); 
				}
			}
			break;
			
		case VK_UP:
			if(iSelected==0)
			{
				char buffer[20];
				GetWindowText(hEdit[0],buffer,sizeof(buffer));
				memset(buffer,'*',strlen(buffer));
				SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(0,-1),(LPARAM)buffer);
				iSelected=1;
				SetFocus(hList); //to Hide Editor ShowWindow(hEdit,SW_HIDE);
				SetFocus(hEdit[1]);
				hFocus=hEdit[1];
			}
			else
			{
				iSelected=0;
				SetFocus(hEdit[0]);
				hFocus=hEdit[0];
			}
			break;

		case VK_DOWN:
			if(iSelected==1)
			{	
				iSelected=0;
				SetFocus(hEdit[0]);
				hFocus=hEdit[0];
			}
			else
			{
				char buffer[20];
				
				iSelected=1;
				GetWindowText(hEdit[0],buffer,sizeof(buffer));
				memset(buffer,'*',strlen(buffer));
				SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(0,-1),(LPARAM)buffer);
				SetFocus(hList);
				SetFocus(hEdit[1]);
				hFocus=hEdit[1];
				
			}
			break;
		}
		
		break;

		case WM_PIN2CHANGEDOK:
			{	
				HWND hPrevWin/*,hPrevPrevWin*/;
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
//				hPrevPrevWin=GetWindow(hPrevWin,GW_HWNDNEXT);
				PostMessage(hWnd,WM_CLOSE,0,0);
				PostMessage(hPrevWin,WM_CLOSE,0,0);
				PostMessage(CheckPin2Info.hParentWnd,CheckPin2Info.message,PIN2_CHANGED,0);
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}
			break;
		case WM_PUK2ERROR:
			{
				HWND hPrevWin;
				hPrevWin=GetWindow(hWnd,GW_HWNDNEXT);
				PostMessage(hPrevWin,WM_PUK2ERROR,0,0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,(LPARAM)0);
				
			}
			break;

		case ME_MSG_VERIFYPUK2:
			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{		
					printf("PUK2 right,PIN2 changed\r\n");
					PLXTipsWin(CheckPin2Info.hFrameWin,hWnd,WM_PIN2CHANGEDOK,ML("pin2codechanged"),NULL,Notify_Success,ML("Ok"),NULL,20);
				}
				break;

			default:
				{
					printf("PUK2 error,PIN2 not changed,input again or PUK2 block\r\n");
					PLXTipsWin(CheckPin2Info.hFrameWin,hWnd,WM_PUK2ERROR,ML("PUK2codewrong"),NULL,Notify_Failure,ML("Ok"),NULL,0);
				}
				break;
			}
			break;
			

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	
    return lResult;
}



static const char * pClassName          = "MBCCallServWndClass";
static const char * PMBConfig_File      = "/mnt/flash/setup/CallSetLog.cfg";
static const int    iAutoRedialNum      = 5;//自动重拨的次数
/*********************************************************************\
* Function     ReadMobileConfigFile
* Purpose      读取配置文件中的数据
* Params       MBConfig ：保存数据的结构
* Return       成功失败    
* Remarks      
**********************************************************************/
BOOL ReadMobileConfigFile(MBConfig * pConfig)
{
    int     hFile;
    DWORD   dfsize = 0;
	struct stat *buf =NULL;

    hFile = open(PMBConfig_File, O_RDONLY);  
    if (hFile == -1)
    {
        hFile = open(PMBConfig_File, O_CREAT|O_RDWR, S_IRWXU);
        if (hFile == -1)
            return FALSE;

        memset(pConfig, 0, sizeof(MBConfig));
        pConfig->iAutoRedial = iAutoRedialNum;   //自动重拨次数的默认值

		//Show call duration and call summary by defatult
		pConfig->bShowCallDuration=TRUE;      
		pConfig->bCallSummary=TRUE;
		strcpy(pConfig->Line1Name,ML("Line 1"));
		strcpy(pConfig->Line2Name,ML("Line 2"));

        if (-1 == write(hFile, pConfig, sizeof(MBConfig)))
            goto Error;

        close(hFile);
        return TRUE;
    }
	buf = malloc(sizeof(struct stat));

    if(buf == NULL)
    {
        close(hFile);
        return FALSE;
    }

    memset(buf, 0, sizeof(struct stat));
    stat(PMBConfig_File, buf);

    dfsize = buf->st_size;

    free(buf);
    buf = NULL;
	
	if (sizeof(MBConfig) != dfsize)//文件长度非法
        goto Error;

    if (-1 == read(hFile, pConfig, sizeof(MBConfig)))
        goto Error;

    pConfig->iAutoRedial = iAutoRedialNum;   //自动重拨次数的默认值

    close(hFile);
    return TRUE;

Error:
    close(hFile);
    unlink(PMBConfig_File);
    return FALSE;
}
/*********************************************************************\
* Function     SaveMobileConfigFile
* Purpose      存储配置文件中的数据
* Params       MBConfig ：保存数据的结构
* Return       成功失败    
* Remarks      
**********************************************************************/
static BOOL SaveMobileConfigFile(const MBConfig * pConfig)
{
    int hFile;

    hFile = open(PMBConfig_File, O_WRONLY);
    
    if (hFile == -1)
    {
        hFile = open(PMBConfig_File, O_RDWR|O_CREAT, S_IRWXU);
        if (hFile == -1)
            return FALSE;
    }
    if (-1 == write(hFile, (MBConfig *)pConfig, sizeof(MBConfig)))
    {
        close(hFile);
        return FALSE;
    }
    close(hFile);
    return TRUE;
}

BOOL ChangeAutoAnswer(BOOL bAuto)
{
	MBConfig pConfig;
	
	if ( ReadMobileConfigFile(&pConfig) )
	{
		pConfig.bModeAuto = bAuto;
		if ( SaveMobileConfigFile(&pConfig) )
			return TRUE;
		else
			return FALSE;
	}
	
	return FALSE;
}

static void MBCall_OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus)
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

BOOL CheckALSWin(void)
{
	WNDCLASS wc;
	HWND hWnd;
	
	wc.style         = 0; 
	wc.lpfnWndProc   = CheckALSWinProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL; //GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "CheckALSWin";

	if(!RegisterClass(&wc))
		return FALSE;
	
	hWnd = CreateWindow(
		"CheckALSWin",
		"",
		WS_CHILD,
		0,0,0,0,
		NULL,
		NULL,
		NULL,
		NULL
		);
	
	return TRUE;
}

LRESULT CheckALSWinProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	
	lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
//			WaitWin(NULL,TRUE,ML("Opening..."),ML("Call Setting"),NULL,ML("Cancel"),0);
			if(ME_GetSALSstatus(hWnd,ME_MSG_CHECKCURRENTLINE1)<0)
			{
//				WaitWin(NULL,FALSE,ML("Opening..."),ML("Call Setting"),NULL,ML("Cancel"),0);
				return FALSE;
			}
		}
        break;

	case ME_MSG_CHECKCURRENTLINE1:	//check the init status of ALS
		{
			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{
					int result;
					ME_GetResult(&result,sizeof(int));
					if ( result == 2 )
					{
						bALS = TRUE;
						bInit = TRUE;
						iOutLine = result;
//						WaitWin(NULL,FALSE,ML("Opening..."),ML("Call Setting"),NULL,ML("Cancel"),0);
						PostMessage(hWnd,WM_CLOSE,0,0);
						break;
					}

					iOutLine = result;
					if( ME_SelectOutLine(hWnd,ME_MSG_SETLINE1,2,0) < 0 )
						return FALSE;
				}
				break;

			default:
				break;
			}
		}
		break;

	case ME_MSG_SETLINE1:	//set to second number 
		{
			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{
					if(ME_GetSALSstatus(hWnd,ME_MSG_CHECKCURRENTLINE2)<0)
					{
						return FALSE;
					}
				}
				break;

			default:
				break;
			}
		}
		break;

	case ME_MSG_CHECKCURRENTLINE2:	//after set to second number and check 
		{
			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{
					int result;
					ME_GetResult(&result,sizeof(int));

					if ( result == 2 )
					{
						bALS = TRUE;
						if( ME_SelectOutLine(hWnd,ME_MSG_SETLINE2,1,0) < 0 )
							return FALSE;
						break;
					}
					else
					{
						bALS = FALSE;
//						WaitWin(NULL,FALSE,ML("Opening"),ML("Call Setting"),NULL,ML("Cancel"),0);
						break;
					}
				}
				break;

			default:
				break;
			}
		}
		break;

	case ME_MSG_SETLINE2:	//if bALS == TRUE, set to init number (line 1)
		{
			switch(wParam) 
			{
			case ME_RS_SUCCESS:
				{
					iOutLine = 1;
					bInit = TRUE;
//					WaitWin(NULL,FALSE,ML("Opening"),ML("Call Setting"),NULL,ML("Cancel"),0);
					PostMessage(hWnd,WM_CLOSE,0,0);
				}
				break;

			default:
				break;
			}

		}
		break;
	
    case WM_CLOSE:
		DestroyWindow(hWnd);  
		break;
		
    case WM_DESTROY :
        UnregisterClass("CheckALSWin",NULL);
        break;

	default:
		break;

	}
	
	return FALSE;
}






















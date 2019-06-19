#include "winpda.h"  
#include "string.h"
#include "malloc.h"
#include "stdlib.h"
#include "stdio.h" 
#include "MBPublic.h"
#include "callpub.h"
#include "RcCommon.h"
#include "MBRecordList.h"
#include "log.h"
#include "plx_pdaex.h"
#include "mullang.h"
#include "me_wnd.h"
#include "..\BlueTooth\BtHandfreeHeadset.h"

#define DEBUG_MBPSTMSG

#define TIMER_CALLING_TIMEOUT       3000
#define TIMER_RINGING_TIMEOUT       3000

#define WM_URC_NOCARRIER            WM_USER + 200
#define WM_URC_CSSU                 WM_USER + 201
#define WM_URC_WIND                 WM_USER + 202
#define WM_URC_CLIP                 WM_USER + 203
#define WM_URC_CCWA                 WM_USER + 204
#define WM_URC_RING                 WM_USER + 205
#define WM_URC_CRING                WM_USER + 213
#define WM_URC_CCCM                 WM_USER + 214
#define WM_LIST_CALLS_NOCAR_CCSU    WM_USER + 206 
#define WM_LIST_CALLS_CALLING       WM_USER + 207
#define WM_LIST_CALLS_CCWA_CLIP     WM_USER + 209
#define WM_LIST_CALLS_RING          WM_USER + 210
#define WM_LIST_CALLS_RINGING       WM_USER + 211
#define WM_LIST_CALLS_CALLWAITOVER  WM_USER + 212

#define IDC_GETCALLWAIT             WM_USER + 110
#define IDC_SETCALLWAIT             WM_USER + 120

#define IDC_BEGIN_SEARCH_CF         WM_USER + 180
#define IDC_GETFORWAR_DUNCDTIONAL   WM_USER + 130
#define IDC_GETFORWAR_BUSY          WM_USER + 140
#define IDC_GETFORWAR_NOREPLY       WM_USER + 150
#define IDC_GETFORWAR_NOREACH       WM_USER + 160
#define IDC_GETLASTERROR            WM_USER + 170
#define IDC_GETSINGALINFO           WM_USER + 300

#define CALLING_TIMEROUT_TIMER_ID   WM_USER + 1
#define RINGING_TIMEROUT_TIMER_ID   WM_USER + 2
//////////////////////////////////////////////////////////////////////////
//蓝牙部分
#define IDC_BT_ANSWER_CALL          WM_USER + 400
#define IDC_BT_CANCEL_CALL          WM_USER + 410
#define IDC_BT_VOL_UP               WM_USER + 420
#define IDC_BT_VOL_DOWN             WM_USER + 430
#define IDC_BT_HFHS_CONNECTED       WM_USER + 440
#define IDC_BT_HFHS_DISCONNECTED    WM_USER + 450
#define IDC_BT_HFHS_AUDIO_CONNECTED WM_USER + 460
#define IDC_BT_HFHS_AUDIO_DISCONNECTED  WM_USER + 470
#define IDC_BT_HF_DAIL_CALL         WM_USER + 480

#define IDC_BT_SWITCHAUTO_TODIGITAL WM_USER + 500//切换到数字通道
#define IDC_BT_SWITCHAUTO_TOANALOG  WM_USER + 505//切换到模拟通道
#define IDC_GETVOLUMEUP             WM_USER + 510
#define IDC_GETVOLUMEDOWN           WM_USER + 520
#define IDC_SETVOLUME               WM_USER + 530
#define IDC_READPHONEBOOK			WM_USER + 540
typedef struct 
{
    int iBTCallBack;
    int iBTMessage;
}BTCALLBACKINFO;
static const BTCALLBACKINFO BtCallBackInfo[] =
{
    {BT_ANSWER_CALL             ,IDC_BT_ANSWER_CALL},
    {BT_CANCEL_CALL             ,IDC_BT_CANCEL_CALL},
    {BT_VOL_UP                  ,IDC_BT_VOL_UP},
    {BT_VOL_DOWN                ,IDC_BT_VOL_DOWN},
    {BT_HFHS_CONNECTED          ,IDC_BT_HFHS_CONNECTED},
    {BT_HFHS_DISCONNECTED       ,IDC_BT_HFHS_DISCONNECTED},
    {BT_HFHS_AUDIO_CONNECTED    ,IDC_BT_HFHS_AUDIO_CONNECTED},
    {BT_HFHS_AUDIO_DISCONNECTED ,IDC_BT_HFHS_AUDIO_DISCONNECTED},
    {BT_HF_DAIL_CALL            ,IDC_BT_HF_DAIL_CALL},
    {-1                         ,-1}
};
//////////////////////////////////////////////////////////////////////////
#ifdef SIEMENS_TC35
#define PHONE_URCMSG_NUM            6
static int  URCKeyWord[] = 
{
    ME_URC_CCCM,
    ME_URC_CRING,       
    ME_URC_RING,        
    ME_URC_NOCARRIER,   
    ME_URC_CSSU,        
    ME_URC_CCWA         
};
static int      URCMsg[] =
{
    WM_URC_CCCM,
    WM_URC_CRING,       
    WM_URC_RING,        
    WM_URC_NOCARRIER,   
    WM_URC_CSSU,        
    WM_URC_CCWA         
};
#endif

enum keytype{
	PM_LONGKEY = 0x01,
	PM_SHORTKEY = 0x02,
	PM_INSTANTKEY =0x04,
	PM_REPETEDKEY = 0x08,
	PM_MULTITAPKEY =0x10
};

#define ET_REPEAT_FIRST  300
#define ET_REPEAT_LATER  100
#define ET_MULTITAP      400
#define ET_LONG          600

typedef struct tagKeyEventData{
	enum keytype  nType;
	DWORD   dKicktime;
	unsigned int nTimerId;     
    long    nkeyCode;
	struct tagKeyEventData *pNext;
}KeyEventData, *PKEYEVENTDATA;
/*-------------------------------------------------------------*/
static	char *SOSNUM[]=
{
	"112",
	"911",
    "999",
    "08",
	""
};
#define	SOSNUMMAXLEN	10
static	char	SosNumStack[11];

int		Matchstr(char **src,char *dec);
static	BOOL	CoordinateNumStack(char * numstack, char chKeyCode);
/*-------------------------------------------------------------*/

static const char * pClassName = "MBPostMsgWndClass";
static HWND         MB_hWndPostMsg;
static int          hWndPostMsg[PHONE_URCMSG_NUM];
static CALLS_INFO   calls_info[MAX_COUNT + 1];

#ifndef _EMULATE_
static BOOL         bInitOver = FALSE;
#else
static BOOL         bInitOver = FALSE ;
#endif
static HHOOK        call_hook; 
static BOOL         bSearchCF = FALSE;
static BOOL         bCalling;
static BOOL         bRinging;
static BOOL         bCFClew = FALSE;

static PKEYEVENTDATA pIdleKeyListHead = NULL;
static BOOL			bLock = FALSE; 
static BOOL			bEmerge;
static BOOL        bEmergyCall = FALSE; 
static WORD        v_nRepeats;	
static PKEYEVENTDATA	pCurKeydata =NULL;//hot key data
static WORD            v_HotKey;
static BOOL			   bChangedLock = FALSE;
static AudioInterfaceType   BTAudioInterfaceType = ME_AUDIO_ANALOG;//当前通道类型

static BT_STATUS BT_status = BT_AUDIO_DISCONNECTED;
static BT_CONNECTED_STATUS BT_Connected_status = BT_DISCONNECTED;
static char cPhoneNumber[PHONENUMMAXLEN + 1];
static BOOL bBTAnswer = FALSE;
static BOOL bBTDialOut = FALSE;

static LRESULT CALLBACK MBPostMsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static LRESULT CALLBACK MBRecPhoneCall_KeyBoardProc(int code,WPARAM wParam,LPARAM lParam);
static BOOL    OnCreate(HWND hWnd);
static void    OnDestroy(HWND hWnd);
static BOOL    InitIdleKeyEvent(void);
static PKEYEVENTDATA  GetKeyTypeByCode(long vkCode);
static WORD PM_GetHotKeyType(WPARAM wParam, LPARAM lParam);
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime);
static BOOL DialKeyProc(WORD vkType, int nCode, WPARAM wParam, LPARAM lParam);

static  void    OnListCallsInTimerCalling(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  void    OnListCallsInWind(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  void    OnListCallsInCcwaAndClip(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  void    OnListCallsInRing(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  void    OnListCallsInTimerRinging(HWND hWnd, WPARAM wParam, LPARAM lParam);
static  BOOL    bEmergencyFirstChar(char ch);
extern	BOOL	IsIdleState();
extern	BOOL	PM_GetkeyLockStatus(void);
extern	BOOL	PM_GetPhoneLockStatus(void);
extern	BOOL	CallAppEntry(BYTE CallType);
extern	HWND    GetPhoneLockWnd();
extern void SetShowSummaryFlag(BOOL f);
extern BOOL GetShowSummaryFlag();
extern void    SendMBDialLinkDialInfo(int iType);
extern void NotifyEmergency(void);
BOOL InitMBPostMsgHook(void)
{
    //call_hook = SetWindowsHookEx(WH_KEYBOARD,MBRecPhoneCall_KeyBoardProc,(WPARAM)0,(LPARAM)0);
    InitMBPostMsg();
    return TRUE;
}
BOOL InitMBPostMsg(void)
{
    WNDCLASS wc;

    if (IsWindow(MB_hWndPostMsg))
        return TRUE;

    wc.style         = 0;
    wc.lpfnWndProc   = MBPostMsgWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	InitIdleKeyEvent();//init idle key for call app

    MB_hWndPostMsg = CreateWindow(pClassName,"",WS_CAPTION,0,0,100,100,NULL,NULL,NULL,NULL);
    if (MB_hWndPostMsg == NULL)
    {
        return FALSE;
    }
    return TRUE;
}

static LRESULT CALLBACK MBPostMsgWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT         lResult;
    CALLFWD_INFO    callfwd_info;
    ME_EXERROR      lLastError;
    int             iVolume;

    lResult = (LRESULT)TRUE;

    switch (message)
    {
    case WM_CREATE:
        bRinging = FALSE;
        lResult = (LRESULT)OnCreate(hWnd);
        if (call_hook == NULL)
		    call_hook = SetWindowsHookEx(WH_KEYBOARD,MBRecPhoneCall_KeyBoardProc,
            (WPARAM)0,(LPARAM)0);

        CreateMBGSMWindow();
        InitMBCalling();    
//        bInitMBDialup();    
//        bInitMBLink();      
        ME_GetCallWaitingStatus(hWnd,IDC_GETCALLWAIT,CLASS_VOICE);
        break;

    case WM_DESTROY:
        OnDestroy(hWnd);
        break;

    case WM_URC_NOCARRIER:
#if defined(SIEMENS_TC35)

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive NOCARRIER,get the reason of interrruption\n");
#endif

		bBTAnswer = FALSE;
        if (IsWindow_DialLink())
            SendMBDialLinkDialInfo(ME_URC_NOCARRIER);
        else
            ME_GetLastError(hWnd,IDC_GETLASTERROR);
#endif
        break;

    case WM_URC_CSSU:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive CSSU!\n");
#endif
        if (IsWindow_MBRing() && (!bCFClew))
        {
            ME_CSSU_ITEM me_cssu_item;
            if (-1 != ME_GetResult(&me_cssu_item,sizeof(ME_CSSU_ITEM)))
            {
                if (me_cssu_item.Action == 0)
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                    printf("mb incoming forwarded calling\r\n");
#endif
                    MessageMBRing(IDC_FORWARDING,NULL,NULL);
                    //AppMessageBox(NULL,ML("Callforwarding"),"",WAITTIMEOUT);
                    bCFClew = TRUE;
                    break;
                }
            }
        }
#if defined(SIEMENS_TC35)
        SetListCurCalls(TRUE);
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_NOCAR_CCSU))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive CSSU==-1!\n");
#endif
        }
#endif
        break;

    //case WM_URC_CCCM:
    case WM_URC_CCWA:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive CCWA!\n");
#endif
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_CCWA_CLIP))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive CCWA==-1!\n");
#endif
        }
        break;

    case WM_URC_CRING:
    case WM_URC_RING:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive RING!bRinging:%d\r\n",bRinging);
#endif
        //蓝牙部分////////////////////////////////////////////////////////////////////////
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\r\n MB Copy Ring to Headset\r\n");
#endif
		if (strlen(cPhoneNumber) != 0)
			BT_CopyRing(cPhoneNumber);//BT_CopyRingToBtDevice(cPhoneNumber);

        BT_status = BT_WAITING;//来电

        if (!bRinging)
        {
            bBTAnswer = FALSE;
        }
        //if (!bRinging)
        {
            bRinging = TRUE;
			//bBTAnswer = FALSE;
            if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_RING))
            {
                bRinging = FALSE;
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("\nMB ME_ListCurCalls==-1\r\n");
#endif
            }
        }
        break;

    case WM_URC_CLIP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB Phone receive CLIP!\n");
#endif
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_CCWA_CLIP))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive CLIP==-1!\n");
#endif
        }
        break;

    case WM_LIST_CALLS_RINGING:
        OnListCallsInTimerRinging(hWnd, wParam, lParam);
        break;

    case WM_LIST_CALLS_RING:
        OnListCallsInRing(hWnd, wParam, lParam);
        break;

    case WM_LIST_CALLS_NOCAR_CCSU:
        OnListCallsInWind(hWnd, wParam, lParam);
        break;

    case WM_TIMER:
        switch (wParam)
        {
        case CALLING_TIMEROUT_TIMER_ID:
            if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_CALLING))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("\nMB CALLING_TIMEROUT_TIMER_ID == -1!\n");
#endif
            }
            break;
        case RINGING_TIMEROUT_TIMER_ID:
            if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_RINGING))
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("\nMB RINGING_TIMEROUT_TIMER_ID == -1!\n");
#endif
            }
            break;
        }   
        break;

    case WM_LIST_CALLS_CALLING:
        OnListCallsInTimerCalling(hWnd, wParam, lParam);
        break;

    case WM_LIST_CALLS_CCWA_CLIP:
        OnListCallsInCcwaAndClip(hWnd, wParam, lParam);
        break;

    case WM_CALLING_HANGUP:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB MBPstMsg receive WM_CALLING_HANGUP\n");
#endif
        KillTimer(hWnd, RINGING_TIMEROUT_TIMER_ID);
        KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
        bCalling = FALSE;
        bRinging = FALSE;

        bCFClew  = FALSE;
		memset(cPhoneNumber,0x00,sizeof(cPhoneNumber));
        break;

    case IDC_GETLASTERROR:
        ME_GetResult(&lLastError,sizeof(PME_EXERROR));
		
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("MB get reason of call interruption,wParam:%ld,lParam:%ld,lLastError:%lx\r\n",wParam,lParam,lLastError);
#endif
		if ((lLastError.LocationID == 8) && (lLastError.Reason == 68))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("MB extra cost\r\n");
#endif
            SetCostLimit(TRUE);
        }
        else if ((lLastError.LocationID == 0) && (lLastError.Reason == 0) && 
            (lLastError.SSrelease == 0))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\r\n MB MBPstMsg LocationID:0,Reason:0,SSrelease:0");
#endif
            ME_GetSignalInfo(hWnd,IDC_GETSINGALINFO);//查询信号量
            break;
        }

        SetListCurCalls(TRUE);
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_NOCAR_CCSU))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone receive NOCARRIER==-1\n");
#endif
        }
        break;

    case IDC_GETSINGALINFO:
        if (wParam == ME_RS_SUCCESS)
        {
            int iSingal;
            ME_GetResult(&iSingal,sizeof(int));
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\r\n MB MBPstMsg iSingal:%d",iSingal);
#endif
            if (iSingal == 0)
            {
                SetHangupSingalInfo(TRUE);
            }
        }
        SetListCurCalls(TRUE);
        if (-1 == ME_ListCurCalls(hWnd,WM_LIST_CALLS_NOCAR_CCSU))
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB Phone IDC_GETSINGALINFO ME_ListCurCalls==-1\n");
#endif
        }
        break;

	case IDC_GETCALLWAIT:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			{
				CALLWAIT_INFO bCallWait;
				if (ME_GetResult(&bCallWait,sizeof(CALLWAIT_INFO)) != -1)
				{
					ME_SetCallWaitingStatus(hWnd,IDC_SETCALLWAIT,bCallWait.Enable,
                        CLASS_VOICE);
				}
                else
                {
                    ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_DUNCDTIONAL,
                        FWD_UNCDTIONAL,CLASS_VOICE);
                }
			}
			break;
        default:
            //ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_DUNCDTIONAL,
            //    FWD_UNCDTIONAL,CLASS_VOICE);
            break;
		}
		break;

    case IDC_SETCALLWAIT:
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_DUNCDTIONAL,FWD_UNCDTIONAL,CLASS_VOICE);
        break;        
    
    case IDC_BEGIN_SEARCH_CF: 
        if (!bSearchCF)
        {
            ME_CREG_URC me_creg_urc;
            ME_GetResult(&me_creg_urc,sizeof(ME_CREG_URC));

            if ((me_creg_urc.Stat == CREG_HOMEREG) || 
                (me_creg_urc.Stat == CREG_ROAMING))
                ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_DUNCDTIONAL,FWD_UNCDTIONAL,CLASS_VOICE);

            bSearchCF = TRUE;
        }
        break;

    case IDC_GETFORWAR_DUNCDTIONAL:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_Uncdtional(callfwd_info.Enable);

            break;
        }
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_BUSY,FWD_BUSY,CLASS_VOICE);
        break;

    case IDC_GETFORWAR_BUSY:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_Busy(callfwd_info.Enable);

            break;
        }
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_NOREPLY,FWD_NOREPLY,CLASS_VOICE);
        break;

    case IDC_GETFORWAR_NOREPLY:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_NoReply(callfwd_info.Enable);

            break;
        }
        ME_GetCallForwardStatus(hWnd,IDC_GETFORWAR_NOREACH,FWD_NOREACH,CLASS_VOICE);
        break;

    case IDC_GETFORWAR_NOREACH:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            if (-1 != ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
                SetForward_NoReach(callfwd_info.Enable);

            break;
        }
        bInitOver = TRUE;
        break;
    //蓝牙部分////////////////////////////////////////////////////////////////////////
    case IDC_BT_ANSWER_CALL://选择蓝牙设备接听
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\r\n MB BT_ANSWER_CALL 蓝牙接听\r\n");
#endif
        BTAnswerHangup(1);
		bBTAnswer = TRUE;
        break;

    case IDC_BT_CANCEL_CALL://蓝牙挂断
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\r\n MB BT_CANCEL_CALL 蓝牙挂断\r\n");
#endif
        if (IsWindow_MBRing())//当前存在来电界面
        {
            BTAnswerHangup(2);//挂断来电
        }
        else if (IsWindow_DialLink())//当前存在连接界面
        {
            DialLink_Hangup();//挂断拨号
        }
        else if (IsCallingWnd())//当前存在通话界面
        {
            MBCalling_Shortcut(CUT_ALL,"");//挂断通话
        }
        break;

    case IDC_BT_VOL_UP://增加音量
        ME_GetSpeakerVolume(hWnd,IDC_GETVOLUMEUP);//首先获得当前音量
        break;
    case IDC_BT_VOL_DOWN://减小音量
        ME_GetSpeakerVolume(hWnd,IDC_GETVOLUMEDOWN);//首先获得当前音量
        break;

    case IDC_BT_HFHS_CONNECTED://蓝牙设备连接
        BT_Connected_status = BT_CONNECTED;
		BTAudioInterfaceType = ME_AUDIO_ANALOG;
		printf("\r\n IDC_BT_HFHS_CONNECTED 蓝牙设备连接");
        if (IsCallingWnd())
        {
			printf("\r\n IDC_BT_HFHS_CONNECTED 蓝牙设备连接 IsCallingWnd()\r\n");
            SetBlueToothStatus();//检测到蓝牙设备,通知通话界面设置右softkey文字
        }
        break;
    case IDC_BT_HFHS_DISCONNECTED://蓝牙设备断连
		printf("\r\n IDC_BT_HFHS_DISCONNECTED 蓝牙设备断连");
        BT_Connected_status = BT_DISCONNECTED;
		BTAudioInterfaceType = ME_AUDIO_ANALOG;
		if (IsCallingWnd())
		{
			ME_SwitchAudioInterface(hWnd,IDC_BT_SWITCHAUTO_TOANALOG,
				ME_AUDIO_ANALOG);//切换成模拟通道
			SetBlueToothClear();
		}
        break;

    case IDC_BT_HFHS_AUDIO_CONNECTED:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\r\n MB BT_HFHS_AUDIO_CONNECTED 蓝牙AUDIO_CONNECTED\r\n");
#endif
        BT_status = BT_AUDIO_CONNECTED;
		bBTDialOut = FALSE;
        if (IsCallingWnd())//手机接听进入本流程
        {
			if (bBTAnswer)
			{
				printf("\r\n BT_HFHS_AUDIO_CONNECTED 切换成数字通道\r\n");
				ME_SwitchAudioInterface(hWnd,IDC_BT_SWITCHAUTO_TODIGITAL,
                    ME_AUDIO_DIGITAL);//切换成数字通道
				break;
			}
            if (BTAudioInterfaceType == ME_AUDIO_DIGITAL)
            {
				printf("\r\n BT_HFHS_AUDIO_CONNECTED 切换成数字通道\r\n");
                ME_SwitchAudioInterface(hWnd,IDC_BT_SWITCHAUTO_TODIGITAL,
                    ME_AUDIO_DIGITAL);//切换成数字通道
            }
            else
            {
				printf("\r\n BT_HFHS_AUDIO_CONNECTED 切换成模拟通道\r\n");
                ME_SwitchAudioInterface(hWnd,IDC_BT_SWITCHAUTO_TOANALOG,
                    ME_AUDIO_ANALOG);//切换成模拟通道
            }
        }
        else if (IsWindow_MBRing())//蓝牙接听进入本流程
        {
			printf("\r\n IDC_BT_HFHS_AUDIO_CONNECTED 蓝牙接听进入本流程\r\n");
            ME_SwitchAudioInterface(hWnd,IDC_BT_SWITCHAUTO_TODIGITAL,
                ME_AUDIO_DIGITAL);//切换成数字通道
        }
		else if (IsWindow_DialLink()) //手机呼出
		{
			printf("\r\n IDC_BT_HFHS_AUDIO_CONNECTED 手机呼出\r\n");
			ME_SwitchAudioInterface(hWnd,IDC_BT_SWITCHAUTO_TODIGITAL,
                ME_AUDIO_DIGITAL);//切换成数字通道

			bBTDialOut = TRUE;
		}
        break;

    case IDC_BT_HFHS_AUDIO_DISCONNECTED:
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\r\n MB BT_HFHS_AUDIO_DISCONNECTED 蓝牙断连\r\n");
#endif
        BT_status = BT_AUDIO_DISCONNECTED;
        if (IsCallingWnd())
        {
            ME_SwitchAudioInterface(hWnd,IDC_BT_SWITCHAUTO_TOANALOG,
                ME_AUDIO_ANALOG);//切换成模拟通道
        }
        if (IsWindow_DialLink())
        {
            DialLink_Hangup();//挂断拨号
			ME_SwitchAudioInterface(hWnd,IDC_BT_SWITCHAUTO_TOANALOG,
                ME_AUDIO_ANALOG);//切换成模拟通道
        }
        break;

    case IDC_BT_SWITCHAUTO_TODIGITAL://切换到数字通道
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\r\n MB 切换到数字通道 wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
        switch (wParam)
        {
        case ME_RS_SUCCESS://切换成功
            if (IsCallingWnd())//通话过程中切换
            {
                SetBlueToothButtonInfo(ME_AUDIO_DIGITAL);
            }
            break;
        default://切换失败
            break;
        }
        break;

    case IDC_BT_SWITCHAUTO_TOANALOG://切换到模拟通道
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\r\n MB 切换到模拟通道 wParam:%ld,lParam:%ld\r\n",wParam,lParam);
#endif
		if (BT_Connected_status == BT_DISCONNECTED)
		{
			printf("\r\n 切换到模拟通道 蓝牙断连\r\n");
			break;
		}
        switch (wParam)
        {
        case ME_RS_SUCCESS://切换成功
            SetBlueToothButtonInfo(ME_AUDIO_ANALOG);
            break;
        default://切换失败
            break;
        }
        break;

	case IDC_BT_HF_DAIL_CALL:
		switch (HIWORD(wParam))
		{
		case 0://number
			strncpy(cPhoneNumber,(char*)lParam,sizeof(cPhoneNumber) - 1);
			printf("\r\n IDC_BT_HF_DAIL_CALL cPhoneNumber:%s\r\n",cPhoneNumber);
			APP_CallPhoneNumber(cPhoneNumber);
			break;
		case 1://dial number in the sim card
			printf("\r\n IDC_BT_HF_DAIL_CALL dial number in the sim card:%d\r\n",
				LOWORD(wParam));
			ME_ReadPhonebook(hWnd,IDC_READPHONEBOOK,PBOOK_SM,LOWORD(wParam),0);
			break;
		case 2://redial
			{
				CallRecord BT_Record;
				printf("\r\n IDC_BT_HF_DAIL_CALL redial\r\n");
				if (-1 != GetMBRecord(&BT_Record,sizeof(CallRecord),DIALED_REC,0))
				{
					printf("\r\n IDC_BT_HF_DAIL_CALL BT_Record.PhoneNo:%s\r\n",BT_Record.PhoneNo);
					APP_CallPhoneNumber(BT_Record.PhoneNo);
				}
			}
			break;
		}
		break;

	case IDC_READPHONEBOOK:
		switch (wParam)
		{
		case ME_RS_SUCCESS:
			{
				PHONEBOOK BT_phonebook;
				ME_GetResult(&BT_phonebook,sizeof(PHONEBOOK));
				printf("\r\n IDC_READPHONEBOOK number:%s\r\n",BT_phonebook.PhoneNum);
				APP_CallPhoneNumber(BT_phonebook.PhoneNum);
			}
			break;
		default:
			break;
		}
		break;

    case IDC_GETVOLUMEUP:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&iVolume,sizeof(iVolume));
            ME_SetSpeakerVolume(hWnd,IDC_SETVOLUME,++iVolume);//增加音量
            break;
        default:
            break;
        }
        break;
    case IDC_GETVOLUMEDOWN:
        switch (wParam)
        {
        case ME_RS_SUCCESS:
            ME_GetResult(&iVolume,sizeof(iVolume));
            ME_SetSpeakerVolume(hWnd,IDC_SETVOLUME,--iVolume);//减小音量
            break;
        default:
            break;
        }
        break;
    //////////////////////////////////////////////////////////////////////////
    default :
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    } 
    return lResult;
}

static BOOL OnCreate(HWND hWnd)
{
    int i;

    for (i = 0; i < PHONE_URCMSG_NUM; i++)
    {
        hWndPostMsg[i] = ME_RegisterIndication(URCKeyWord[i],IND_ORDINAL,hWnd,URCMsg[i]);
        if (hWndPostMsg[i] == -1)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB ME_RegisterIndication is %d fail\n",hWndPostMsg[i]);
#endif
            return FALSE;
        }
    }
    ME_RegisterIndication(ME_URC_CREG,IND_ORDINAL,hWnd,IDC_BEGIN_SEARCH_CF);
    //////////////////////////////////////////////////////////////////////////
    //关于兰牙部分
#ifndef _EMULATE_
    i = 0;
    while (BtCallBackInfo[i].iBTCallBack != -1)
    {
        BT_RegisterNotifyMsg(BtCallBackInfo[i].iBTCallBack,hWnd,BtCallBackInfo[i].iBTMessage);
        i++;
    }
#endif
    return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void    BT_AnswerCall(void)//通过来电界面接听
{
    if ((GetUsedItemNum() == 0) && (BT_Connected_status == BT_CONNECTED))
    {
#ifndef _EMULATE_
        BT_MobileAnswerCall();//通知蓝牙设备接听
#endif
    }
}
void    BT_CopyRing(const char * pNumber)
{
    if ((GetUsedItemNum() == 0) && (BT_Connected_status == BT_CONNECTED))
    {
#ifndef _EMULATE_
        BT_CopyRingToBtDevice(pNumber);
#endif
    }
}
void    BT_Hangup(void)//蓝牙挂断
{
    int iUsed = GetUsedItemNum();
    if (((iUsed == 0) || (iUsed == 1)) && (BT_Connected_status == BT_CONNECTED))
    {
#ifndef _EMULATE_
        BT_MobileHangUpCall();
#endif
    }
}
void	BT_DialOut(void)//
{
    if ((GetUsedItemNum() == 0) && (BT_Connected_status == BT_CONNECTED))
    {
#ifndef _EMULATE_
	    BT_DialCall();
#endif
    }
}
BOOL	AnswerCallFromBT(void)
{
	return bBTAnswer;
}
void	SetAnswerCallFromBT(BOOL b)
{
	bBTAnswer = b;
}
void	SetBTDialout(BOOL b)
{
	bBTDialOut = b;
}
BOOL	GetBTDialout(void)
{
	return bBTDialOut;
}
BT_CONNECTED_STATUS    BT_EquipmentConnected(void)//蓝牙设备是否已经连接
{
	if (BT_Connected_status == BT_CONNECTED)
		printf("\r\n BT_Connected_status:BT_CONNECTED\r\n");
	else
		printf("\r\n BT_Connected_status:BT_DISCONNECTED\r\n");

    return BT_Connected_status;
}
BT_STATUS	BT_GetStatus(void)
{
	return BT_status;
}

void    BT_SwitchChannel(AudioInterfaceType AudioType)//切换通道类型
{
    BTAudioInterfaceType = AudioType;//记录需要切换成什么类型
	printf("\r\n BT_SwitchChannel BTAudioInterfaceType:%d\r\n",BTAudioInterfaceType);
    if ((GetUsedItemNum() == 1) && (BT_Connected_status == BT_CONNECTED))
    {
#ifndef _EMULATE_
        BT_SwitchVoiceChannel();
#endif
    }
}
void    BT_RemoveCall(void)//来电自行挂断调用函数
{
    if ((GetUsedItemNum() == 0) && (BT_Connected_status == BT_CONNECTED))
    {
#ifndef _EMULATE_
        BT_RemoteTerminateCall();
#endif
    }
}
void    BT_HangupNetwork(void)//蓝牙无信号挂断调用
{
    int iUsed = GetUsedItemNum();
    if (((iUsed == 0) || (iUsed == 1)) && (BT_Connected_status == BT_CONNECTED))
    {
#ifndef _EMULATE_
        BT_NetworkUnavailable();
#endif
    }
}
void    BT_Terminal(void)//远端忙
{
    if ((GetUsedItemNum() == 0) && (BT_Connected_status == BT_CONNECTED))
    {
#ifndef _EMULATE_
        BT_RemoteBusy();
#endif
    }
}
void    BT_RemoteAnswer(void)//远端接听
{
    int iUsed = GetUsedItemNum();
    if (((iUsed == 0) || (iUsed == 1)) && (BT_Connected_status == BT_CONNECTED))
    {
#ifndef _EMULATE_
        BT_RemoteAnswerCall();
#endif
    }
}
//////////////////////////////////////////////////////////////////////////
static void OnDestroy(HWND hWnd)
{
    int i;
    for (i = 0; i < PHONE_URCMSG_NUM; i++)
    {
        ME_UnRegisterIndication(hWndPostMsg[i]);
    }
    MB_hWndPostMsg = NULL;
    UnregisterClass(pClassName, NULL);
}

static void OnListCallsInCcwaAndClip(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int i,iDataLen;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info, sizeof(calls_info));
    if (iDataLen > 0)
    {
        iDataLen = iDataLen / sizeof(CALLS_INFO);
    }
    else
    {
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);
    }
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInCcwaAndClip,ME_GetResult return:%d\n",iDataLen);
#endif
    if (iDataLen > 0)
    {
        for (i = 0; i < iDataLen; i++)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB  calls_info[%d].Stat=%d\n",i,calls_info[i].Stat);
#endif
            if ((calls_info[i].Stat == CALL_STAT_INCOMING) ||   
                (calls_info[i].Stat == CALL_STAT_WAITING))      
            {
                //if ((iDataLen > MAX_COUNT) || (GetPhoneGroupNum()== 2)) 
				if (iDataLen > MAX_COUNT)
                {   
                    if (-1 == ME_SwitchCallStatus(hWnd,WM_LIST_CALLS_CALLWAITOVER,CUT_HELD,0))
                    {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                        printf("MB MBPstMsg OnListCallsInCcwaAndClip ME_SwitchCallStatus=-1\r\n");
#endif
                    }
                    return;
                }
//                MessageMBDialUp(WM_CLOSE,0,0);
                if (MBPhoneRing(&calls_info[i]))
				{
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
					printf("\nMB OnListCallsInCcwaAndClip,i:%d,phonenumber:%s,index:%d\n",
						i,calls_info[i].PhoneNum,calls_info[i].index);
#endif
					bCalling = TRUE;
					SetTimer(hWnd,CALLING_TIMEROUT_TIMER_ID,
						TIMER_CALLING_TIMEOUT,NULL);
				}

                break;
            }
        }
    }
}

static void OnListCallsInRing(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int i,iDataLen;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info, MAX_COUNT * sizeof(CALLS_INFO));
    if (iDataLen > 0)
    {
        iDataLen = iDataLen / sizeof(CALLS_INFO);
    }
    else
    {
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);
    }

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInRing,ME_GetResult return:%d\n",iDataLen);
#endif
    if (iDataLen > 0)
    {
        for (i = 0; i < iDataLen; i++)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB calls_info[%d].Stat=%d\n",i,calls_info[i].Stat);
#endif
            if ((calls_info[i].Stat == CALL_STAT_INCOMING) ||   
                (calls_info[i].Stat == CALL_STAT_WAITING))      
            {
//                MessageMBDialUp(WM_CLOSE,0,0);
                if (MBPhoneRing(&calls_info[i]))
                {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                    printf("\nMB OnListCallsInRing,i:%d,phonenumber:%s,index:%d\n",
                        i,calls_info[i].PhoneNum,calls_info[i].index);
#endif
                    bCalling = TRUE;
                    SetTimer(hWnd,RINGING_TIMEROUT_TIMER_ID,
                        TIMER_RINGING_TIMEOUT, NULL);
					strcpy(cPhoneNumber,calls_info[i].PhoneNum);
                }
                return;
            }
        }
    }
}

static void OnListCallsInWind(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int         i,iDataLen;
    CALLS_INFO  newcall_info;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info, MAX_COUNT * sizeof(CALLS_INFO));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInWind,iDataLen:%d\r\n",iDataLen);
#endif

    if (iDataLen == -1)
    {
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);
//        MessageMBDialUp(WM_CLOSE,0,0);
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);
        MessageMBCallingOne(WM_VOICE_HANGUPONE,ME_RS_SUCCESS,0);
        if (IsCallingWnd())
        {
            BT_RemoveCall();
        }
        bCalling = FALSE;
    }
    else
    {
        MBListCurCalls(LISTCURCALLS_REPEAT,wParam,lParam);

        if (IsWindow_MBRing())
        {
            GetNewItemInfo(&newcall_info);

            iDataLen = iDataLen / sizeof(CALLS_INFO);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("MB OnListCallsInWind,iDataLen:%d,phonenumber:%s,index:%d\r\n",
                iDataLen,newcall_info.PhoneNum,newcall_info.index);
#endif
            for (i = 0; i < iDataLen; i++)
            {
                if (calls_info[i].index == newcall_info.index)
                    break;
            }
            if (i == iDataLen)
            {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
                printf("MB OnListCallsInWind,(i == iDataLen) phonenumber:%s,index:%d\r\n",
                    newcall_info.PhoneNum,newcall_info.index);
#endif
                KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
                bCalling = FALSE;
                MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);
            }
        }
    }
}

static void OnListCallsInTimerCalling(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int         i,iDataLen;
    CALLS_INFO  NewCall_info;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info, MAX_COUNT * sizeof(CALLS_INFO));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInTimerCalling,ME_GetResult return:%d,bCalling:%d\n",iDataLen,bCalling);
#endif
    if ((iDataLen == -1) && (bCalling))
    {
        PostMessage(MB_hWndPostMsg,WM_CALLING_HANGUP,0,0);
        bCalling = FALSE;
        KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);
    }
    else if (bCalling)
    {
        iDataLen = iDataLen / sizeof(CALLS_INFO);
        GetNewItemInfo(&NewCall_info);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB OnListCallsInTimerCalling,iDataLen:%d,phonenumber:%s,index:%d\n",
            iDataLen,NewCall_info.PhoneNum,NewCall_info.index);
#endif

        for (i = 0; i < iDataLen; i++)
        {
            if (calls_info[i].index == NewCall_info.index)
                break;
        }
        if (i == iDataLen)
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB OnListCallsInTimerCalling,i == iDataLen,i:%d,phonenumber:%s,index:%d\n",
                i,NewCall_info.PhoneNum,NewCall_info.index);
#endif

            KillTimer(hWnd, CALLING_TIMEROUT_TIMER_ID);
            bCalling = FALSE;
            MessageMBRing(WM_CALLING_HANGUP,NULL,NULL);
        }
    }
}

static void OnListCallsInTimerRinging(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    int         i,iDataLen;
    CALLS_INFO  newcall_info;

    memset(calls_info,0x00,sizeof(calls_info));
    iDataLen = ME_GetResult(calls_info,MAX_COUNT * sizeof(CALLS_INFO));

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
    printf("\nMB OnListCallsInTimerRinging,ME_GetResult return:%d,bRinging:%d\n",iDataLen,bRinging);
#endif
    if ((iDataLen == -1) && (bRinging))
    {
        bRinging = FALSE;
        bCalling = FALSE;
        KillTimer(hWnd, RINGING_TIMEROUT_TIMER_ID);
        MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);
    }
    else if (bRinging)
    {
        iDataLen = iDataLen / sizeof(CALLS_INFO);
        GetNewItemInfo(&newcall_info);

#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
        printf("\nMB OnListCallsInTimerRinging,iDataLen:%d,phonenumber:%s,index:%d\n",
            iDataLen,newcall_info.PhoneNum,newcall_info.index);
#endif

        for (i = 0; i < iDataLen; i++)
        {
            if (calls_info[i].index == newcall_info.index)
            {
                if (calls_info[i].Stat == CALL_STAT_ACTIVE)//电话已经接通
                {
                    KillTimer(hWnd,RINGING_TIMEROUT_TIMER_ID);
                    MBRingAnswerCall();
                }
                return;
            }
        }
        if ((i == iDataLen) && (bCalling)) 
        {
#if defined(SHOWDEBUGINFO) && defined(DEBUG_MBPSTMSG)
            printf("\nMB OnListCallsInTimerRinging,i == iDataLen,i:%d,phonenumber:%s,index:%d\n",
                i,newcall_info.PhoneNum,newcall_info.index);
#endif

            KillTimer(hWnd,RINGING_TIMEROUT_TIMER_ID);
            bRinging = FALSE;
            bCalling = FALSE;
            MessageMBRing(WM_CALLING_HANGUP, NULL, NULL);
        }
    }
}
void    MessageMBPostMsg(int iCallMsg,WPARAM wParam, LPARAM lParam)
{
    SendMessage(MB_hWndPostMsg,iCallMsg,wParam,lParam);
}

void    MessageMBPostMsgCallForward(void)
{
    ME_GetCallForwardStatus(MB_hWndPostMsg,IDC_GETFORWAR_DUNCDTIONAL,FWD_UNCDTIONAL,CLASS_VOICE);
}

calliftype GetCallIf()
{
	if (IsRingWnd()) 
		return ringif;
	if (IsWindow_Dialup()) 
		return dialupif;
	if (IsWindow_DialLink())
		return diallinkif;
	if (IsCallingWnd())
		return callingif;
	if (IsCallEndWnd())
		return callendif;
	if (IsGSMWndShow())
		return gsmshowif;
	return initif;
}

BOOL IsCallAPP(void)
{
	return (IsRingWnd()||IsWindow_Dialup()||IsWindow_DialLink()||IsCallingWnd()||IsCallEndWnd() );
}

#define MAX_IDLEKEYNUM  14
static WORD nKeyWord = 0;
extern BOOL IfSetConfirm(void);

static  LRESULT CALLBACK MBRecPhoneCall_KeyBoardProc(int ncode,WPARAM wParam,LPARAM lParam)
{
	WORD vkType = 0;
	WORD nKeyCode = LOWORD(wParam);
	int  i=0;
	BOOL bEnable;
	HMENU hMenu;
	
	long vkParam[] = 
	{
	 		VK_F2,VK_F1,VK_F3,VK_F4,
			VK_0, VK_1, VK_2, 
			VK_3, VK_4, VK_5, 
			VK_6, VK_7, VK_8, VK_9,0
	};

    if (ncode < 0 )  
        return CallNextHookEx(call_hook,ncode,wParam,lParam);

//	if (LOWORD(wParam) == VK_1 || bEmerge == TRUE)//emergy call
//		bEmerge = TRUE;
//	else
	//		bEmerge = FALSE;

	if (PM_GetkeyLockStatus() || PM_GetPhoneLockStatus())
	{
		calliftype callif;
		bLock = TRUE;
		if (LOWORD(wParam) == VK_F2)
		{			
			callif = GetCallIf();
			if (callif == dialupif) {
				hMenu = (HMENU)GetCapture();
				SendMessage((HWND)hMenu, WM_CANCELMODE, 0, 0);
				if (IsCallingWnd())
					CloseCallingWnd();
				CloseDialupWnd();
				return TRUE;
			}
			if (callif == callingif) {
				hMenu = (HMENU)GetCapture();
				SendMessage((HWND)hMenu, WM_CANCELMODE, 0, 0);
				CloseCallingWnd();
				return TRUE;
			}
			if (callif == diallinkif) {
				hMenu = (HMENU)GetCapture();
				SendMessage((HWND)hMenu, WM_CANCELMODE, 0, 0);
				CloseDialLinkWnd();
				return TRUE;
			}
			if (callif == ringif) {
				hMenu = (HMENU)GetCapture();
				SendMessage((HWND)hMenu, WM_CANCELMODE, 0, 0);
				CloseRingWnd();
				return TRUE;
			}
/*
			if (callif == callendif) {
				CloseCallEndWnd();
				return TRUE;
			}
			if (callif == gsmshowif) {
				HideGSMWnd();
				return TRUE;
			}
*/
			return CallNextHookEx(call_hook, ncode, wParam, lParam);
		}
        //else //if (bEmerge == FALSE) 
		//	return CallNextHookEx(call_hook, ncode, wParam, lParam);
		else
		{
			if(lParam & 0xc0000000)//keyup
				;
			else
			{
				calliftype callif;
				callif = GetCallIf();
				
			//	if(PM_GetPhoneLockStatus() /*&& LOWORD(wParam) == VK_F10*/)
			//		return CallNextHookEx(call_hook, ncode, wParam, lParam);				
				/*else */if(callif != ringif && callif != callendif && callif != callingif && PM_GetkeyLockStatus())
				{
					if(nKeyCode ==VK_EARPHONE_OUT || nKeyCode == VK_EARPHONE_IN|| nKeyCode ==VK_CHARGER_IN || nKeyWord == VK_CHARGER_OUT 
						|| nKeyCode == VK_USB_IN || nKeyCode == VK_USB_OUT)
						;
					{
						if(IfSetConfirm() && ((nKeyCode == VK_RETURN) || (nKeyCode== VK_F10)))
                        {
                            return CallNextHookEx(call_hook, ncode, wParam, lParam);
                        }
						else
                        {
                            if (bEmergencyFirstChar((char)nKeyCode) && !IsRingWnd() && !IsCallingWnd())
                            {
                            }
                            else
                            {
                                printf("\r\n ^^^^^^^^^^^^^^ Keypad locked ^^^^^^^^^^^^^^^^^^^^^^^^\r\n");
                                PLXTipsWin(NULL, NULL, NULL, ML("Keypad locked"), NULL,Notify_Info, NULL, ML("Unlock"),20);
                            }
                        }
					}
				}
			}
		}
	}
	else
		bLock = FALSE;

	for(i=0; i < MAX_IDLEKEYNUM; i++)
	{
		if(vkParam[i] == LOWORD(wParam))
		{
		    break;
		}
	}
	
	if(i == MAX_IDLEKEYNUM )
		return CallNextHookEx(call_hook, ncode, wParam, lParam);
	
	vkType = PM_GetHotKeyType(wParam, lParam);


//	if(PM_GetPhoneLockStatus() && GetFocus() != GetPhoneLockWnd())
//		bEnable = FALSE;
//	else
		bEnable = bLock;

	if((IsIdleState() || bEnable) && ((nKeyCode >= VK_0 && nKeyCode <= VK_9) 
		||(nKeyCode == VK_F3) ||(nKeyCode == VK_F4) || (nKeyCode == VK_F1))) 
	{
		if(DialKeyProc(vkType, ncode, wParam, lParam))
			return TRUE;
	}

	if(nKeyCode == VK_F2)
	{
		//if(lParam & 0xc0000000) //keyup
		{
			calliftype callif;
			callif = GetCallIf();
			if (callif == dialupif) {
				hMenu = (HMENU)GetCapture();
				SendMessage((HWND)hMenu, WM_CANCELMODE, 0, 0);
				if (IsCallingWnd())
                {
                    if (!(lParam & 0xc0000000))
                        CloseCallingWnd();
                }
				if(bChangedLock)
				{
					bChangedLock = FALSE;
					DlmNotify(PS_LOCK_ENABLE, TRUE);
				}
				CloseDialupWnd();
                return CallNextHookEx(call_hook, ncode, wParam, lParam);
			}
			if (callif == callingif) {
				hMenu = (HMENU)GetCapture();
				SendMessage((HWND)hMenu, WM_CANCELMODE, 0, 0);

				if (!(lParam & 0xc0000000))
                {
                    CloseCallingWnd();
                }
				return TRUE;
			}
			if (callif == diallinkif) {
				hMenu = (HMENU)GetCapture();
				SendMessage((HWND)hMenu, WM_CANCELMODE, 0, 0);
				CloseDialLinkWnd();
				if(bChangedLock)
				{
					bChangedLock = FALSE;
					DlmNotify(PS_LOCK_ENABLE, TRUE);
				}
				return TRUE;
			}
			if (callif == ringif && IsCallingWnd())
			{
				hMenu = (HMENU)GetCapture();
				SendMessage((HWND)hMenu, WM_CANCELMODE, 0, 0);
				SetShowSummaryFlag(TRUE);
				if (!(lParam & 0xc0000000))
                {
                    CloseCallingWnd();
                }
				return TRUE;
			}
			if (callif == ringif) {
				hMenu = (HMENU)GetCapture();
				SendMessage((HWND)hMenu, WM_CANCELMODE, 0, 0);
				CloseRingWnd();
				return TRUE;
			}
/*
			if (callif == callendif) {
				CloseCallEndWnd();
				return TRUE;
			}
			if (callif == gsmshowif) {
				HideGSMWnd();
				return TRUE;
			}*/

			else
			{
				return CallNextHookEx(call_hook, ncode, wParam, lParam);
			}
		}
	}

    return CallNextHookEx(call_hook, ncode, wParam, lParam);
}
/********************************************************************
* Function   DialKeyProc  
* Purpose    handle digital key 
* Params     vkType: means key type(short, long, instant...)
* Return     
* Remarks      
**********************************************************************/
static BOOL DialKeyProc(WORD vkType, int nCode, WPARAM wParam, LPARAM lParam)
{
	static char cEmergenum[4];
	static  const char  cCh_Well     = '#';
    static  const char  cCh_Asterisk = '*';
	char cNum [2];
	static BOOL bFirstOne = TRUE;
	WORD nKeyCode = LOWORD(wParam);

	if(vkType == PM_SHORTKEY)//begin if
	   {
		//for dial number
		if (bLock == FALSE)
		{
			//for dial number
			switch (nKeyCode)
			{
			case VK_0:  //0
			case VK_1:  //1
			case VK_2:  //2
			case VK_3:  //3
			case VK_4:  //4
			case VK_5:  //5
			case VK_6:  //6
			case VK_7:  //7
			case VK_8:  //8
			case VK_9:  //9
				cNum[0] = (char)nKeyCode;
				cNum[1] = 0;
                APP_CallPhone(cNum);
				return TRUE;
			case VK_F4://#
				cNum[0] = cCh_Well;//'#';
				cNum[1] = 0;
                APP_CallPhone(cNum);
				return TRUE;
			}
		}
		else
		{
			switch (nKeyCode)
			{
			case VK_0:  //0
			case VK_1:  //1
			case VK_2:  //2
			case VK_3:  //3
			case VK_4:  //4
			case VK_5:  //5
			case VK_6:  //6
			case VK_7:  //7
			case VK_8:  //8
			case VK_9:  //9
				if(CoordinateNumStack(SosNumStack, (char)nKeyCode))
				{
					if(bLock)
					{
						DlmNotify(PS_LOCK_ENABLE, FALSE);
						bChangedLock = TRUE;
					}
                    NotifyEmergency();
                    APP_CallPhone(SosNumStack);
                    memset(SosNumStack,0x00,sizeof(SosNumStack));
				}
                else if (bEmergencyFirstChar((char)nKeyCode) && !IsRingWnd() && !IsCallingWnd())
                {
                    if(bLock)
					{
						DlmNotify(PS_LOCK_ENABLE, FALSE);
						bChangedLock = TRUE;
					}
                    NotifyEmergency();
                    APP_CallPhone(SosNumStack);
                    memset(SosNumStack,0x00,sizeof(SosNumStack));
                }
				else
				{
					if(PM_GetkeyLockStatus())
					{
						if(lParam & 0xc0000000)//keyup
							;
						else
						{
							if(nKeyCode ==VK_EARPHONE_OUT || nKeyCode == VK_EARPHONE_IN|| nKeyCode ==VK_CHARGER_IN || nKeyWord == VK_CHARGER_OUT 
								|| nKeyCode == VK_USB_IN || nKeyCode == VK_USB_OUT)
                            {
                                ;
                            }
							else
                            {
                                //printf("\r\n @@@@@@@@@@@@@@@ Keypad locked @@@@@@@@@@@@@@@@@@@@@@@\r\n");
                                PLXTipsWin(NULL, NULL, NULL, ML("Keypad locked"), NULL,Notify_Info, NULL, ML("Unlock"),20);
                            }
						}
					}
				}
				return TRUE;				
			}
			/*
			if ( nKeyCode == VK_1 && bFirstOne == TRUE)
			{
			if(PM_GetkeyLockStatus())
			PLXTipsWin(NULL, NULL, NULL, ML("Keypad locked"), NULL,Notify_Info, NULL, ML("Unlock"),20);
			cEmergenum[0]= (char)nKeyCode;
			cEmergenum[1] = 0;
			bFirstOne = FALSE;
			bEmerge = TRUE;
			return TRUE;
			}
			else if ( nKeyCode == VK_1 && bFirstOne == FALSE && bEmerge == TRUE && strcmp(cEmergenum, "1") == 0) 
			{
			if(PM_GetkeyLockStatus())
			PLXTipsWin(NULL, NULL, NULL,ML("Keypad locked"), NULL,Notify_Info, NULL, ML("Unlock"),20);
			cEmergenum[1]= (char)nKeyCode;
			cEmergenum[2] = 0;
			bEmerge = TRUE;
			return TRUE;
			}
			else if (nKeyCode == VK_2 && bFirstOne == FALSE && bEmerge == TRUE && strcmp(cEmergenum, "11") == 0) 
			{
			cEmergenum[2]= (char)nKeyCode;
			cEmergenum[3] = 0;
			bEmerge = TRUE;
			bFirstOne = TRUE;
			bEmergyCall = TRUE;
			//call emergecall
			if(bLock)
			{
			DlmNotify(PS_LOCK_ENABLE, FALSE);
			bChangedLock = TRUE;
			}
			
			  APP_CallPhone(cEmergenum);
			  return TRUE;
			  }
			  else
			  {
			  if(PM_GetkeyLockStatus())
			  PLXTipsWin(NULL, NULL, NULL, ML("Keypad locked"), NULL,Notify_Info, NULL, ML("Unlock"),20);
			  
				cEmergenum[0] = 0;
				bFirstOne = TRUE;
				bEmerge = FALSE;
				return TRUE;
				}
			*/
		}
	   }//end if

	   if (bLock && nKeyCode == VK_F1)
	   {
		   calliftype callif;
		   callif = GetCallIf();
		   if (callif == ringif)
		   {
			   DlmNotify(PS_LOCK_ENABLE, FALSE);
			   bChangedLock = TRUE;
			   return CallNextHookEx(call_hook, nCode, wParam, lParam);
		   }
	   }
	   if(bLock &&  nKeyCode == VK_F1 && bEmergyCall)
	   {
		   bEmergyCall = FALSE;
		   return CallNextHookEx(call_hook, nCode, wParam, lParam);
	   }
	   
	   if(vkType == PM_INSTANTKEY && nKeyCode == VK_F1 && IsIdleState())
	   {
		   CallAppEntry(DIALED_CALL);
		   return TRUE;
	   }
	   
	   if(vkType == PM_LONGKEY && nKeyCode == VK_1&& IsIdleState())
	   {
		   keybd_event(VK_1, 0, 0, 0); //call voice mail
	   }
	   
	   if(vkType == PM_LONGKEY &&(nKeyCode >= VK_2 && nKeyCode <= VK_9)&& IsIdleState())
	   {
		   keybd_event(nKeyCode, 0, 0, 0); //quick dial
	   }
	   
	   if(vkType == PM_LONGKEY && nKeyCode == VK_0 && IsIdleState())
	   {
		   keybd_event(VK_0, 0, 0, 0);//call wap browser
	   }
	   
	   if(nKeyCode == VK_F3 && IsIdleState())
	   {
		   if(vkType == PM_INSTANTKEY/*PM_MULTITAPKEY*/)
		   {
			   cNum[0] = cCh_Asterisk;//'*';
			   cNum[1] = 0;
               APP_CallPhone(cNum);
			   return TRUE;//dialing 
		   }
		   else if(vkType == PM_LONGKEY)
		   {
			   //characters grid 
		   }
	   }
	   
	   if(vkType == PM_INSTANTKEY && nKeyCode == VK_F4 && IsIdleState())
	   {
		   //dialing
	   }
	   return FALSE;
}
/********************************************************************
* Function   PM_GetHotKeyType  
* Purpose    Get key Type(short, long, instant...)
* Params     
* Return     
* Remarks      
**********************************************************************/
static WORD PM_GetHotKeyType(WPARAM wParam, LPARAM lParam)
{
	PKEYEVENTDATA pKeyData = NULL;
	DWORD	dwTickNow = 0;
    WORD    wKeyType = 0;
    BYTE    byKeyEvent = 0;
    int     nInterval = 0;
	WORD    vkType = 0;

	
	pKeyData = GetKeyTypeByCode(LOWORD(wParam));
	
	if(pKeyData == NULL)
		return -1;
	
	pCurKeydata = pKeyData;
	
	if(lParam & 0xc0000000) //keyup
	{
		if(pCurKeydata->nType & PM_LONGKEY)
		{
			if(pCurKeydata->nTimerId !=0)
			{
				KillTimer(NULL, pCurKeydata->nTimerId);
				pCurKeydata->nTimerId = 0;
			}
//			else
//			{
//				vkType= PM_LONGKEY;
//			}
		}
		
		if (pCurKeydata->nType & PM_SHORTKEY)
		{
			dwTickNow = GetTickCount();
			printf("\r\n dwTickNow = %x", dwTickNow);

			nInterval = dwTickNow - pCurKeydata->dKicktime;
			if (nInterval < 0)
				nInterval = -nInterval;
			printf("\r\n interval = %d", nInterval);
			
			if (nInterval <= ET_LONG)
			{
				vkType = PM_SHORTKEY;
			}
		}
		v_nRepeats=0;
	}
	else //keydown
	{
		v_nRepeats++;
		
		if(pCurKeydata->nType & PM_LONGKEY)
		{
			if(pCurKeydata->nTimerId != 0)
			{
				KillTimer(NULL, pCurKeydata->nTimerId);
			}
			pCurKeydata->nTimerId = SetTimer(NULL, 0,  ET_LONG, f_TimerProc);
		}

		if(pCurKeydata->nType & PM_INSTANTKEY)
		{
			vkType = PM_INSTANTKEY;
		}
		if(pCurKeydata->nType & PM_SHORTKEY || pCurKeydata->nType & PM_MULTITAPKEY)
		{
			dwTickNow = GetTickCount();
			nInterval = dwTickNow - pCurKeydata->dKicktime;
			if (nInterval < 0)
				nInterval = -nInterval;
			
			if (pCurKeydata->nType & PM_MULTITAPKEY)
			{
				if ((nInterval <= ET_MULTITAP) && (LOWORD(wParam) == v_HotKey))
				{
					vkType= PM_MULTITAPKEY;
				}
				
				// 1.Press down during the MULTITAP interval,
				//   but the pressing key changed
				// 2.Although exceeds the MULTITAP time limit,
				//   the timer haven't arrived
				if (((nInterval <= ET_MULTITAP) && (LOWORD(wParam) != v_HotKey))
					|| ((nInterval > ET_MULTITAP) && (pCurKeydata->nTimerId != 0)))
				{
					KillTimer(NULL, pCurKeydata->nTimerId);
					pCurKeydata->nTimerId = 0;
				}
			}
			
			pCurKeydata->dKicktime = dwTickNow;
			printf("\r\n CurKey kick time = %x", dwTickNow);
			
			if ( pCurKeydata->nType & PM_MULTITAPKEY)
			{
				if (pCurKeydata->nTimerId != 0)
				{
					KillTimer(NULL, pCurKeydata->nTimerId);
				}
				pCurKeydata->nTimerId = SetTimer(NULL, 0, ET_MULTITAP,
					f_TimerProc);
			}
		}

		if(pCurKeydata->nType & PM_MULTITAPKEY)
		{
			vkType = PM_MULTITAPKEY;
		}
		v_HotKey  = LOWORD(wParam);
	}
	return vkType;
}
/*********************************************************************\
* Function     InitIdleKeyEvent
* Purpose      
* Params      
* Return       
**********************************************************************/
#define  MAX_KEYLISTITEM  MAX_IDLEKEYNUM - 1 //f2
static BOOL bKeyEnable;

static BOOL InitIdleKeyEvent(void)
{
	int i;
	PKEYEVENTDATA pKeyData = NULL, p = NULL;
    long vkParam[][2]= 
    {
		{VK_1, PM_LONGKEY|PM_SHORTKEY}, {VK_2,PM_LONGKEY|PM_SHORTKEY},
		{VK_3, PM_LONGKEY|PM_SHORTKEY}, {VK_4,PM_LONGKEY|PM_SHORTKEY},
		{VK_5, PM_LONGKEY|PM_SHORTKEY}, {VK_6,PM_LONGKEY|PM_SHORTKEY}, 
		{VK_7, PM_LONGKEY|PM_SHORTKEY}, {VK_8,PM_LONGKEY|PM_SHORTKEY}, 
		{VK_9, PM_LONGKEY|PM_SHORTKEY}, {VK_0,PM_LONGKEY|PM_SHORTKEY},
		{VK_F1, PM_INSTANTKEY},
		{VK_F3, /*PM_SHORTKEY|*/PM_LONGKEY|PM_INSTANTKEY/*PM_MULTITAPKEY*/},
		{VK_F4, PM_SHORTKEY|PM_INSTANTKEY},0
    };

	for(i = 0;i< MAX_KEYLISTITEM;i++)
	{
		pKeyData = malloc(sizeof(KeyEventData));

		if(pKeyData == NULL)
			return FALSE;
		pKeyData->dKicktime = 0;
		pKeyData->nkeyCode = vkParam[i][0];
		pKeyData->nType = vkParam[i][1];
		pKeyData->nTimerId =0;
		pKeyData->pNext = NULL;

		if(pIdleKeyListHead == NULL)
			pIdleKeyListHead = pKeyData;
		else
		{
			p = pIdleKeyListHead;
			while(p->pNext)
			{
				p = p->pNext;
			}
			p->pNext = pKeyData;
		}
	}
	return TRUE;
}
/*********************************************************************\
* Function     GetKeyTypeByCode
* Purpose      
* Params      
* Return       
**********************************************************************/
static PKEYEVENTDATA  GetKeyTypeByCode(long vkCode)
{
   PKEYEVENTDATA pKeyData = NULL;

   if(pIdleKeyListHead == NULL)
	   return NULL;
   
   pKeyData = pIdleKeyListHead;

   while(pKeyData)
   {
	   if(pKeyData->nkeyCode == vkCode)
		   return pKeyData;
	   pKeyData = pKeyData->pNext;
   }
   return NULL;
}
/********************************************************************
* Function   f_TimerProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if(pCurKeydata->nTimerId != 0)
	{
		KillTimer(hWnd, idEvent);
		
		switch(pCurKeydata->nkeyCode) 
		{
		case VK_0:
		case VK_1:
		case VK_2:
		case VK_3:
		case VK_4:
		case VK_5:
		case VK_6:
		case VK_7:
		case VK_8:
		case VK_9:
			{
				if(IsIdleState())
					keybd_event(pCurKeydata->nkeyCode, 0, 0, 0); 
			}
			break;
		}

	}
	idEvent = 0;
	pCurKeydata->nTimerId = 0;
}
/********************************************************************
* Function   GetChangeLockStatus  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL    GetChangeLockStatus(void)
{
	return bChangedLock;
}
void   SetChangeLockStatus(void)
{
	bChangedLock = FALSE;
}

BOOL    GetInitState(void)
{
    return bInitOver;
}
/*
 *	
 */
int Matchstr(char **src,char *dec)
{
	int	i,nResult;
	char	**tmp = src;
	nResult = -1;
	//与ＳＯＳ号码表完全不匹配
	while ((i = strncmp(*tmp,dec,strlen(dec)))!=0)
	{
		*tmp++;
		if ((strlen(*tmp))<=0) {			
			return nResult;
		}
	}
	nResult = 0;
	//前面几个数字匹配，可能还没有输入完。
	while ((i = strcmp(*tmp,dec))!=0) {
		*tmp++;
		if ((strlen(*tmp))<=0) {			
			return nResult;
		}
	}
	//匹配成功，输入的号码存在于SOS表中
	nResult = 1;
	return nResult;
}

static BOOL bEmergencyFirstChar(char ch)
{
    char ** p = SOSNUM;
    while (**p)
    {
        if (** p == ch)
            return TRUE;
        p++;
    }
    return FALSE;
}

static	BOOL	CoordinateNumStack(char * numstack, char chKeyCode)
{
	char	tmp[10] = "";
//	char	ch;
	int		i,lResult,nLen;

	//put key in stack;
	nLen = strlen(numstack);
	if (nLen < SOSNUMMAXLEN)
	{
		numstack[nLen] = chKeyCode;		
	}
	else
	{
		for (i=0; i<nLen-1; i++) 
		{
			numstack[i]=numstack[i+1];
		}
		numstack[nLen-1] = chKeyCode;
	}
	lResult = Matchstr(SOSNUM,numstack);
	if (lResult ==1) {
		return TRUE;
	}
	if (lResult ==0) {
		return FALSE;
	}
	//optimize assording to some special status from sos table;
//	while(numstack[0]!='1'||(lResult < 0)) {
	while(lResult < 0) {
/*		nLen = strlen(numstack);
		if (nLen == 0) {
			return lResult;
		}
*/		
		nLen = strlen(numstack);
		for (i=0; i<nLen-1; i++) 
		{
			numstack[i]=numstack[i+1];
		}
		numstack[nLen-1] = '\0';
		nLen = strlen(numstack);
		if (nLen == 0) {
			return FALSE;
		}
		/*
		if (nLen == 1) {
			return -1;
		}
		*/
		lResult = Matchstr(SOSNUM,numstack);
	}
	if (lResult<1) {
		return FALSE;
	}
	else{
		return TRUE;
	}
	
}
BOOL GetAppCallInfo(void)
{
    return (IsWindow_MBRing() || IsCallingWnd() || IsWindow_DialLink());
}

void PostRingMessage(void)
{
    PostMessage(MB_hWndPostMsg,WM_URC_RING,0,0);
}

void HangupIncomingOrCalling(void)
{
    if (IsRingWnd())
    {
        BTAnswerHangup(2);
    }
    else if (IsCallingWnd())
    {
        CloseCallingWnd();
    }
}

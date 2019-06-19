        /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman.lib
 *
 * Purpose  : handle idle window (that is screen save window)
 *            status window
 *            
\**************************************************************************/

#include "progman.h"
#include "string.h"
#include "stdlib.h"
#include "unistd.h"

#include "plx_pdaex.h"
//#include "Ring.h"
#include "appdirs.h"
#include "appfile.h"
#include "me_wnd.h"
#include "pmi.h"
#include "setup.h"
#include "pubapp.h"
#include "callpub.h"
#include "pmalarm.h"
#include "PhoneBookExt.h"
#include "LocApi.h"
#include "MBPublic.h"
#include "browser\func.h"

#define MAX_TIMESNUM			10 //max left remains when find network 
#define EDITMAXLEN              8
#define EDITMINLEN              4

#define GPRS_SUSPEND      0
#define GPRS_TRANSFERING  1
#define GPRS_ATTACH       2
#define GPRS_UNATTACH     3
#define GPRS_UNAVAIL      4


#define	PMA_HAS_CHARGEUP		0x0001 //means charger attached

//#define TIMER_ID_PHONELOCK		1
#define TIMER_ID_FRESH			2 //update caption time show 
#define TIMER_ID_IDLETXT		3//when idle mode text is above five lines auto scroll

#define IDLE_TIMEOUT			15000 //auto keylock  timeout in idle
#define APP_TIMEOUT				30000 //auto keylock timeout in app 
#define IDLETXT_TIMEOUT			5000 //when idle mode text is above five lines auto scroll  

#define  MAX_IDLETXTLEN			5 //idle mode text max lines

#define  LINETXT_Y				20
#define  LINETXT_HIGHT			18
#define  LINE1_Y				130
#define  LINE2_Y				113
#define  LINEHIGH				15

#define CODE_WRONG				"Code wrong"
#define CODE_RIGHT				"Code right"
#define IDS_OK					"Ok"
#define IDS_CANCEL				"Cancel"
#define PHONELOCK				"Enter phone\r\nlock code:"


extern	PAPPADM		pAppMain;		
extern	PROGDESKIDLE  ProgDeskIdle; 

static  BOOL     bGPRS = FALSE, bGPRSGet = FALSE, bSimLockBlocked = FALSE;
static	DWORD	 g_nAttrib;//system status parameter

static  HICON    hIcon[MAX_ICONID];//record idle icon info

static  BOOL     bFirstTip = TRUE;//first tip for no enough battery
static  NOTIFYICONDATA BatData;
static  BOOL     bQuestPin;// whether pin request
static  HWND     hPhonelockWnd;//phonelock window handle
static  BOOL     bSearchNetwork, bInitNet =TRUE, bMe_Init = FALSE;
static  BOOL     bGetSimLock, bSPCodeGet;
static  int      iOpsLimit = 0;
   
/*****************************************/
//function statement
static	LRESULT	DesktopWndProc(HWND, UINT, WPARAM, LPARAM);
static	LRESULT	AppLaunchWndProc(HWND, UINT, WPARAM, LPARAM);
static	LRESULT IdleWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static  void    HandleBattery(HWND hWnd);
static  void    HandleSignal(HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL bURC);
static  BOOL    GetIconRect(int id, RECT *rc);

static	void	DrawIdleText(HWND hwnd, HDC hdc, BOOL bCellIdUp);
static	BOOL	IsTimeChanged(void);

		int		PM_atoi(char* nptr);
		BOOL	PM_isdigit(int ch);
		BOOL	PM_GetkeyLockStatus(void);
		BOOL	PM_GetPhoneLockStatus(void);
		BOOL	IsIdleState();
		BOOL	IsMissedState();

static void CALLBACK f_KeylockTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);
static void CALLBACK f_PhonelockTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);

		void	f_ResetDelayKeyLock();
		void	f_ResetDelayPhoneLock();
		void	f_ResetDelayKeyLockEx();
		void	f_ResetDelayPhoneLockEx();

extern  BOOL    Desk_ShowGrp( struct appMain * pAdm);
extern  BOOL    PlayPowerOff(BOOL bPlay);
extern  void    SIMVerify(WPARAM wParam, LPARAM lParam);
extern  void    SetSimState(int i);
extern  BOOL    PasswordWindow(int type);
extern	BOOL	CallCommunicateWindow(HWND hwndCall,int icallmsg);
extern  BOOL    APP_ViewUnanswered(void);
extern  BOOL    AlarmInterfaceForProgman(void);
extern  PCURAPPLIST *AppFileGetListHead( struct appMain * pAdm );
extern	void	dlm_UnoadModule ();
extern	void	SetPin1Status(BOOL bState);
extern	BOOL	IsResetPowerOn(void);
extern  BOOL    SetSPSimLockData(PSPSIMLOCKCODE pSPSimlockCode);
extern  int		ReadSPSimLockDate(PSPSIMLOCKCODE pSPLC);
extern	BOOL    SetNetSimLockData(PNSLC pNetSimlockCode, int n);
extern  BOOL	CallAppEntryEx(PSTR pAppName, WPARAM wparam, LPARAM lparam);
extern  int		ReadNetSimLockData(PNSLC pNetSimlockCode);
extern  BOOL	IsSimLockCheck(void);
extern  BOOL    ClearSPSimLockCode(void);
extern  BOOL    ClearSPSimLockData(void);
extern  BOOL    ClearNetSimLockCode(void);
extern  BOOL	ClearNetSimLockData(void);
extern  HWND	GetPowerOnWnd(void);
extern	void	RegisterNotification(HWND hWnd);
extern	void	RequestLocalTime(HWND hWnd);
extern  void    EndMobileInit(void);
extern	BOOL	PhoneCodeWnd(void);
extern  BOOL	IfMeInitRate(void);
extern  BOOL	PM_IsPowerOff(void);

/********************************************************************
* Function	 AppDeskCreate  
* Purpose    hide window to recieve me message    
* Params	   
* Return	 	   
* Remarks	  
**********************************************************************/
BOOL	AppDeskCreate( struct appMain * pAdm )
{
    HWND	hWnd = NULL;	
    WNDCLASS wc;
    
    wc.style         = 0;
    wc.lpfnWndProc   = DesktopWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "ProgmanWindowClass";
    if (!RegisterClass(&wc))
        return FALSE;
    
    hWnd = CreateWindow("ProgmanWindowClass",
        "BackDesk",
        WS_BORDER,
        0,	
        0,
        0,	
        0,
        NULL, NULL,	NULL,	NULL);					
    
    RegisterDesktopWindow(hWnd);
    pAppMain->hDeskWnd = hWnd;
    
    return TRUE;
}
/*************************************************************************
* Function	f_OnMainChangeTime
* Purpose	set moduel time
* Params	
* Return		   
* Remarks	  
**************************************************************************/
static	int	g_nTimerID;//timer id for finding network
static  SimFunctions SimFun;//record sim status
static  int  iSimlockCheckRemains;//simlock check remains
static  LONG   bSimState = ME_RS_NOSIMCARD;
static  int    Contentlen, Contentlen_Lan;
static  char   strLan[3 + 1]/*, strNetSimLockData[6]*/;
static  BOOL   bReadFileType = FALSE, bReadFileType_lan =FALSE;
static  BOOL   bMailBoxAct = FALSE, bSimInfoAct =FALSE;
static  BOOL   bRoaming;
static	void   ST_ParseStatus(unsigned char *strCode, int iconter);
static  BOOL   CreateSimLockCodeWnd(HWND hWnd);
static	void   PM_HandleSetLan(void);
static  void   PM_HadleReadGil(void);

		void   SetSimStk(BOOL bStatus);
		void   SetSTKResult(int info);
extern	void   f_HandSetInlineInitialize ( void );
extern  void   SetGSMRadioPartMode(SWITCHTYPE OnOrOff);
extern  BOOL   GetPS_Open(void);
extern  void   SetPSCode(char* pPSCode);
extern  void   SetPSData(char*pPsData);
extern  BOOL   GetNCK(char* pNck);
extern  BOOL   GetSPCK(char* pSpck);
extern  void   PM_CancelPoweroff(void);
static	BOOL   f_OnMainChangeTime(HWND hWnd )
{
    SYSTEMTIME  aTime;
    char		aStr[32] = "\0";	
	int        nYear = 0;
    
    if ( ME_GetResult ( aStr, ME_SMS_TIME_LEN ) < 0 )
	{
		printf("\r\n Get ME_GetResult Timer Fail!");
        return	FALSE;
	}

	printf("\r\n Get Me Time Success!");

	memset(&aTime, 0, sizeof(SYSTEMTIME));
    
    aStr[2]	= 0;

	nYear = atoi(aStr);

	if(nYear > 50)
		aTime.wYear	= (WORD)(atoi( aStr ) + 1900);
	else
		aTime.wYear	= (WORD)(atoi( aStr ) + 2000);
    
    aStr[5]	= 0;
    aTime.wMonth= (WORD)atoi( &aStr[3] );
    
    aStr[8]	= 0;
    aTime.wDay	= (WORD)atoi( &aStr[6] );
    
    aStr[11]= 0;
    aTime.wHour	= (WORD)atoi( &aStr[9] );
    
    aStr[14]= 0;
    aTime.wMinute = (WORD)atoi( &aStr[12] );
    
    aStr[17]= 0;
    aTime.wSecond = (WORD)atoi( &aStr[15] );
    
    DHI_SetRTC_( &aTime );
    
    InvalidateRect ( hWnd, NULL, FALSE );
    
    return	TRUE;
}
//if GPS time is hot and set gps time to wireless moduel 
static BOOL   f_SetMeTime(HWND hWnd, SYSTEMTIME* st)
{
    char		cdate[ME_SMS_TIME_LEN] = "\0";	
	int        nYear = 0;
	char       pTimeStr[] = "%02d/%02d/%02d,%02d:%02d:00";
 
	if(!st)
		return FALSE;
	
	sprintf(cdate,pTimeStr,//"%02d/%02d/%02d,%02d:%02d:00"
		st->wYear%100,st->wMonth,st->wDay,st->wHour,st->wMinute);
	
	ME_SetClockTime(hWnd,WM_ME_SET_TIME,cdate);//set the wierless module first
    
    
    return	TRUE;
}
extern BOOL GetSIMInfoAct(void)
{
	return bSimInfoAct;
}
extern BOOL GetMailNumAct(void)
{
	return bMailBoxAct;
}
#define HEADSETVOLUM  "/mnt/flash/handvolum.ini"
#define SN_VOL       "volum"
#define KN_VOL       "kn_vol"
void SetHeadSetVolum(int value)
{
	FS_WritePrivateProfileInt(SN_VOL,KN_VOL,value,HEADSETVOLUM);

}
int GetHeadSetVolum(void)
{
	int volum = 0;

	volum = FS_GetPrivateProfileInt(SN_VOL,KN_VOL,volum,HEADSETVOLUM);
	return volum;
}
extern void    SetHeadSetStatus(BOOL bAttach);
void SetHandSet(HWND hWnd)
{
	RTC_Check();
	
	if(DHI_ReadHeadSetStatus() > 0)			// check headset ( earphone )
	{
		DlmNotify(PS_SETHEADSET, ICON_SET);		// set headset icon
		SetHeadSetStatus(TRUE);
		
		if(ME_SelectAudioMode(hWnd, WM_AUDIO_HEADSET, ME_AUDIO_HEADSET)<0)
		{
			printf("\r\n ME_SelectAudioMode failure!");
		}
		if(ME_SelectAudioModeEx(hWnd, WM_AUDIO_HEADSET, ME_AUDIO_HEADSET)<0)
		{
			printf("\r\n ME_SelectAudioModeEx failure!");
		}
		
	}
	else
	{
		DlmNotify(PS_SETHEADSET, ICON_CANCEL);
		SetHeadSetStatus(FALSE);
		
		if(ME_SelectAudioMode(hWnd, WM_AUDIO_HANDSET, ME_AUDIO_HANDSET)<0)
		{
			printf("\r\n ME_SelectAudioMode failure!");
		}
		if(ME_SelectAudioModeEx(hWnd, WM_AUDIO_HANDSET, ME_AUDIO_HANDSET)<0)
		{
			printf("\r\n ME_SelectAudioModeEx failure!");
		}	
	}

}
BOOL GetSimLockCheckReady(void)
{
	return(bSPCodeGet && bGetSimLock); 
}
void Me_InitFunc(HWND hWnd)
{
	
	if(!bSPCodeGet)
	{
		if(ME_GetCPHSInfo(hWnd, WM_READGIL1)<0)
		{
			printf("\r\n ME_GetCPHSInfo failure!");
		}
	}

	
	if(ME_InitSTK(hWnd, WM_USER + 623)<0)
	{
		printf("\r\n ME_InitSTK failure!");
	}

}
extern int	OpenedFileNumber(void); 
extern int	OpenedDirNumber(void); 
extern BOOL PdaSvr_Open(BYTE connect_type);
extern void PdaSvr_Close(void);
extern void	MobileInit3(void);
extern void MobileInit4(HWND hWnd);
extern BOOL InitProcessEnd(void);

void SetHandSet(HWND hWnd);
static BOOL bHaveInitMux;
static LRESULT DesktopWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT      lResult  = TRUE;	
    static	int	s_nTimes = 0;
    
    switch (wMsgCmd)
    {
    case WM_TIMER:

		if(g_nTimerID !=0)//find operator
		{
			KillTimer ( hWnd, g_nTimerID );
			g_nTimerID = 0;
		}
		if (wParam == 1) //180s  prompt no enough battery
        {
            Elect_PowerOn(TRUE);
            break;
        }
        
        if (wParam == 2) //charging
        {
            char       IconName[64] = "", ind[4] = "";
            static int i = 0;
            
            if ( !(g_nAttrib & PMA_HAS_CHARGEUP) )
                break;
            
			if(i == 4)
			{
				strcpy(IconName, "/rom/progman/battery/battery_full.ico");
			}
			else if(i == 0)
			{
				strcpy(IconName, "/rom/progman/battery/battery_0%.ico");
			}
			else
			{
				strcpy(IconName, "/rom/progman/battery/battery_");    
				sprintf(ind, "%d%c", i*25,0x25);
				strcat(IconName, ind);	
				strcat(IconName, ".ico");	
			}
			
			SendMessage(pAppMain->hIdleWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
		
			SendMessage(pAppMain->hIdleWnd, PWM_SETSIGNAL, 
				MAKEWPARAM(IMAGE_BITMAP, POWERICON),(LPARAM)IconName);

            i ++;
            if (i > 4)
                i =0;
            break;
        }
        if (wParam == 3) // query singnal and battery voltage
        {
            ME_GetSignalInfo(hWnd, WM_PROG_SINGEL);
			SendMessage(hWnd, WM_PROG_BATTERY, 3, 0);
            
            if (ME_GetBatteryInfo(hWnd, WM_PROG_BATTERY) < 0)
            {
                PLXPrintf("battery error\r\n");
                break;
            }
			OpenedFileNumber(); 
			OpenedDirNumber(); 
			
            break;
        }		
		if(wParam == 4) //delay query status
		{
			if(ME_GetCurOprator(hWnd, WM_PROG_COPSINFO, FORMAT_LONGALPHA)<0)
				;
			else
				KillTimer(hWnd, 4);
		}
        break;
	case WM_RTCTIMER:
		{
			if(wParam == 1)
			{
				if (ME_GetBatteryInfo(hWnd, WM_PROG_BATTERY) < 0)
				{
					PLXPrintf("battery error\r\n");
					break;
				}	
			}
		}
		break;

	case CALLBACK_CANCEL_OFF:
		{
			PM_CancelPoweroff();
			f_sleep_unregister(INPOWEROFF);
		}
		break;

	case WM_AUDIO_HANDSET:
		printf("\r\n set audio chanel to handset!");
		break;

	case WM_AUDIO_HEADSET:
		printf("\r\n set audio chanel to headset!");
		break;
        
    case WM_PROG_SINGEL:
        HandleSignal(hWnd,wParam, lParam, FALSE);
        break;
		     
    case WM_PROG_BATTERY: 

		if(wParam == 3)//user define
		{
			char          IconName[64];
			static BOOL  bInit;

			if(bInit)
				break;

			bInit = TRUE;

			IconName[0] = 0;

			strcpy(IconName, "/rom/progman/battery/battery_50%.ico");

			SendMessage(pAppMain->hIdleWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
			
			SendMessage(pAppMain->hIdleWnd, PWM_SETSIGNAL, 
				MAKEWPARAM(IMAGE_BITMAP, POWERICON),(LPARAM)IconName);
			
			if(PM_GetPhoneLockStatus())
			{
				SendMessage(GetActiveWindow(), PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
				SendMessage(GetActiveWindow(), PWM_SETSIGNAL, 
					MAKEWPARAM(IMAGE_BITMAP, POWERICON),(LPARAM)IconName);
			}
		}
		else
			HandleBattery(hWnd);
        break;
		
	case WM_ME_BATTERYCHARGECONTROL:
		if (ME_RS_SUCCESS==wParam)
			printf("\r\nAT^SBC return OK!\r\n");
		else
			printf("\r\nAT^SBC return error!\r\n");
		
		break;

	case WM_PROG_BTCOM:
		{
			if(wParam == 1)
				PdaSvr_Open(2);//TFTP_CONNECT_BT
			else
				PdaSvr_Close();
		}
		break;

	case WM_SIMLOCKBLOCK://simlock blocked after simlock check fail ten times 
		printf("\r\n simlockblocked and enter emergency call!");
		bSimLockBlocked  =TRUE;
		f_sleep_unregister(INIT_FLAG);
		break;

	case WM_MESLEEPMODE://me enter sleep mode
		break;

	case MYWM_SETTITLE:
        
        switch(wParam)
        {
        case MYWM_SETBAT:		//plug in charger or not
            
            if((int)lParam == 1)
            {
                bFirstTip = TRUE;
                KillTimer(hWnd, 1); //over "no enough battery " prompt
                Elect_PowerOn(FALSE);  //display charging gif
                
                SetTimer(hWnd, 2, 1000, NULL);
                
                g_nAttrib |= PMA_HAS_CHARGEUP;
            }
            else
            {					
                g_nAttrib &= ~PMA_HAS_CHARGEUP;
                KillTimer(hWnd, 2);
                HandleBattery(hWnd);				
            }
            
            break;
            
        case MYWM_SETSGN://query singal
            {                
				SetTimer(hWnd, 3, 25000,NULL);
				break;					
            }
        }
        break;
        
    case WM_CREATE:
        PROG_InitDeskIdle(&ProgDeskIdle);//initialize idle info
        break;

	case WM_COMMAND://recieve gps time changed info
		{
			if(wParam == LOC_TIME_UPDATING && LOWORD( lParam ) == LOC_OK)
			{
				UTCTIME aUTCTimePtr;
				SYSTEMTIME  aTime;

				memset(&aUTCTimePtr, 0, sizeof(UTCTIME));
				memset(&aTime, 0, sizeof(SYSTEMTIME));

				if(LocGetUTCTimeAndDate( &aUTCTimePtr )< 0)
					break;

				aTime.wYear = aUTCTimePtr.UTCYear;
				aTime.wMonth = aUTCTimePtr.UTCMonth;
				aTime.wDay = aUTCTimePtr.UTCDay;
				aTime.wHour = aUTCTimePtr.UTCHour;
				aTime.wMinute = aUTCTimePtr.UTCMinute;
				aTime.wSecond = (WORD)aUTCTimePtr.UTCSecond;

				SetLocalTime( &aTime );

				f_SetMeTime(hWnd, &aTime);//set gps time to wireless moduel
		
				InvalidateRect ( hWnd, NULL, FALSE );
			}
		}
		break;
		
	case PM_DLMEXIT://for dlm app edit
		{
            PAPPNODE pNode =NULL;

			PLXPrintf("\r\n recieve PM_DLMEXIT message !");
			pNode = pAppMain->pFileCache + LOWORD(wParam);
			PLXPrintf("\r\n %d", LOWORD(wParam));
			PLXPrintf("\r\n %x ", pNode);
			if(pNode != NULL)
			{
				PLXPrintf("\r\n %x, %x", pNode->nType, pNode->appcontrol);
			}
			if(pNode && pNode->nType& DLM_APP && pNode->appcontrol != NULL)
			{
				
				PLXPrintf("\r\n send app_exit message !");
				pNode->appcontrol(APP_EXIT, 0, 0, 0);
			}

			PLXPrintf("\r\n call dlm unload !");
			dlm_UnoadModule();
		}
		break;
        
    case WM_ME_COMINIT:	
		{
			bSimState = wParam;
			
			if(bSimState == ME_RS_NOSIMCARD)
			{
				printf("\r\n recieve ME_RS_NOSIMCARD!");
			//	MobileInit3();
				SetHandSet(hWnd);//set default volume

				PLXTipsWinOem(NULL, GetPowerOnWnd(), NOSIM_ALART, ML("Insert SIM card"),ML("SIM"), Notify_Alert,
					NULL,NULL, 20);

			}
			else
			{
				pm_sim_cop_status = pm_cops_link;
				SetSimState(1);

				printf("\r\n WM_ME_COMINIT!!!");

				printf("\r\n WM_ME_COMINIT Mobile Init3!!");

			//	MobileInit3();
			
				ME_GetCurWaitingPassword( hWnd, WM_PROG_WHICHPASS);
			}

		}
		
		break;

	case WM_HOMEZONE://query homezone
		{
			int info = -1;
			if (ME_GetResult(&info, sizeof(ME_CGREG_URC)) < 0)
                break;

			if(info == 0)
				DlmNotify(PS_SETHOMEZONE, ICON_CANCEL);
			else if(info == 1)
				DlmNotify(PS_SETHOMEZONE, ICON_SET);;
		}
		break;
	case WM_PORG_STK:
		{
			int info = 0;

			ME_GetResult(&info,sizeof(int));

			if((info== 37) || (info == 33))
			{
				SetSimStk(TRUE);
				SetSTKResult(info);
			}
		}
		break;

	case WM_PORG_CELLID:
		{
			ME_CREG_URC info;
			
			memset(&info, 0, sizeof(ME_CREG_URC));
			
			if(ME_GetResult(&info, sizeof(ME_CREG_URC))<0)
				break;

			PLXPrintf("\r\n WM_PORG_CELLID info.Stat = %d", info.Stat );

			if(info.Stat == CREG_ROAMING || info.Stat == CREG_HOMEREG)
			{
				if(info.Stat == CREG_ROAMING)
				{
					DlmNotify(PS_SETROAMING, ICON_SET);
					bRoaming = TRUE;
				}
				ME_GetSignalInfo (hWnd, WM_PROG_SINGEL);
				ME_GetBatteryInfo(hWnd, WM_PROG_BATTERY);
				if(InitProcessEnd())
					ME_GetCurOprator(hWnd, WM_PROG_COPSINFO, FORMAT_LONGALPHA);
				if(!bMe_Init)
				{
					bMe_Init = TRUE;
					Me_InitFunc(hWnd);
					break;
				}		
			}

			if(info.Stat == CREG_DENIED || info.Stat == CREG_NOREG)
			{
				DlmNotify(PS_SETROAMING, ICON_CANCEL);
				bRoaming = FALSE;
				if(GetSIMState() == 0)
					pm_sim_cop_status = pm_no_sim;
				else if(bSimLockBlocked)
					pm_sim_cop_status = pm_cops_emergencycallonly;
				else
					pm_sim_cop_status = pm_cops_rejectsim;
				if(pAppMain && pAppMain->hIdleWnd)
					PostMessage(pAppMain->hIdleWnd, WM_IDLEOPRCHANGED, 0, 0);
				DlmNotify(PS_SETGPRS,GPRS_UNAVAIL);
			
			}
			if (info.Stat == CREG_SEARCHING)
            {
				ME_GetSignalInfo (hWnd, WM_PROG_SINGEL);
				ME_GetBatteryInfo(hWnd, WM_PROG_BATTERY);
            }
		
		}
		break;
	case WM_PROG_SBC:
		{
			Ex_Indicator  exIndicator;

			memset(&exIndicator, 0, sizeof(int));

			if(ME_GetResult(&exIndicator, sizeof(int)) < 0)
				break;
			
			if(exIndicator == Battchg)
				ME_GetBatteryInfo(hWnd, WM_PROG_BATTERY);
		}
		break;

	case WM_PROG_VOLTAGELOW://Battery voltage low tips
		{
			if(PM_IsPowerOff())
				break;
			
			PLXTipsWinOem(NULL, NULL, NULL, ML("Battery voltage low"), ML("Battery"), Notify_Info,
				NULL, NULL, WAITTIMEOUT);
			
			if ( !(g_nAttrib & PMA_HAS_CHARGEUP) )
			{
				printf("\r\n low voltage power off!\r\n");
				PlayPowerOff(TRUE); 
			}

		}
		break;

	case WM_PROG_BATTEMPOUTRANGE://query battery status
		{
			ChargeCtl_t i ;

			memset(&i, 0, sizeof(ChargeCtl_t));
			
			if(ME_GetResult(&i, sizeof(ChargeCtl_t))<0)
				break;

			if(i.ChargeStatus == CHG_HIGHTMP)
			{
				PLXTipsWin(NULL, hWnd, NULL,ML("Battery temperature out of rang"),ML("Battery"),Notify_Alert,
				NULL, NULL, WAITTIMEOUT);
			}
			if(i.ChargeStatus == CHG_ERROR)
			{
				PLXTipsWin(NULL, hWnd, NULL,ML("Charging error"),ML("Battery"),Notify_Alert,
				NULL, NULL, WAITTIMEOUT);
			}
			if(i.ChargeStatus == CHG_FINISH)
			{
				PLXTipsWin(NULL, hWnd, NULL, ML("Charging finished"), ML("Battery"), Notify_Info, 
					NULL,NULL,WAITTIMEOUT);
                KillTimer( pAppMain->hDeskWnd, 2 );
			}
			
		}
	
		break;
	case WM_CALLDIVERT://query call divert
		{
			CALLFWD_INFO callfwd_info;

			memset(&callfwd_info, 0, sizeof(CALLFWD_INFO));

			switch (wParam)
			{
			case ME_RS_SUCCESS:
				if (-1 == ME_GetResult(&callfwd_info,sizeof(CALLFWD_INFO)))
				{
					break;
				}
				else
				{
					if (callfwd_info.Enable)
					{
						DlmNotify(PS_SETCALLDIVERT, ICON_SET);
					}
					else
					{
						DlmNotify(PS_SETCALLDIVERT, ICON_CANCEL);
					}
				}
				break;
				
			}
			break;

		}
		break;
	case WM_ALSLINE://query als line info
		{
			int iLine = 0;

			if (ME_GetResult(&iLine, sizeof(int)) < 0)
				break;

			if(iLine == 1)
			{
				DlmNotify(PS_SETACTIVELINE2, ICON_CANCEL);
				DlmNotify(PS_SETACTIVELINE1, ICON_SET);
			}
			else
			{
				DlmNotify(PS_SETACTIVELINE1, ICON_CANCEL);
				DlmNotify(PS_SETACTIVELINE2, ICON_SET);
			}
		}
		break;
		
	case WM_SIMREAD: //read from sim card
		{
			CPHS_struct result;
			
			memset(&result, 0, sizeof(CPHS_struct));
			
			memset(&SimFun, 0x00, sizeof(SimFunctions));	
			
			if(ME_GetResult(&result, sizeof(CPHS_struct)) < 0)
			{
				printf("\r\n ME_GetCPHSparameters Getresult failure! ");
				if(!bGPRSGet)
				{
					if(ME_GPRS_Monitor(hWnd, WM_GPRSAVAIL)<0)
					{
						printf("\r\n ME_GPRS_Monitor failure!");
					}
				}
				break;
			}
			
			if(result.Type == FieldNotFound)	
				break;
			else if(result.Type == RecordContent)
			{
				;
			}
			else if(result.Type == FileContent)	
			{
				bReadFileType = TRUE;
				Contentlen = 2 * result.Content_Len;
				
				if(ME_ReadCPHSFile( hWnd, WM_READFILE, 
					0x6F38, result.Content_Len )<0)//result.Content_Len unsigned char	
				{
					printf("\r\n ME_ReadCPHSFile failure!");
				}
				
			}

		}
		break;
	case WM_READFILE://read from sim card
		{
			unsigned char *result = NULL;
			
			BYTE    istatus =0;
			
			result = (unsigned char *)malloc(Contentlen *sizeof(unsigned char));
			
			if(result == NULL)
			{
				printf("\r\n WM_READFILE malloc failure!");
				break;
			}
			
			memset(result, 0, Contentlen *sizeof(unsigned char));
			
			if(bReadFileType)
			{
				if(ME_GetResult(result, Contentlen* sizeof(unsigned char))<0)
				{
					printf("\r\n WM_READFILE ME_GetResult failure!");
					if(!bGPRSGet)
					{
						if(ME_GPRS_Monitor(hWnd, WM_GPRSAVAIL)<0)
						{
							printf("\r\n ME_GPRS_Monitor failure!");
						}
					}
					
					if(result)
					{
						free(result);
						result = NULL;
					}
					break;
				}
				ST_ParseStatus(result,Contentlen);
			}
			
			if(result)
			{
				free(result);
				result = NULL;
			}
		
			if(!bGPRSGet)
			{
				if(ME_GPRS_Monitor(hWnd, WM_GPRSAVAIL)<0)
				{
					printf("\r\n ME_GPRS_Monitor failure!");
				}
			}

			
		}
		break;
    case WM_PROG_COPS:
        {
            ME_INIT_GETVALUE value;         						
            
             memset(&value, 0, sizeof(ME_INIT_GETVALUE));
			 			
            if (ME_GetResult(&value, sizeof(ME_INIT_GETVALUE)) < 0)
            {
				if(!bSearchNetwork)
				{
					f_HandSetInlineInitialize ();	
				}
                break;
            }
			if(!bMe_Init)
			{
				bMe_Init = TRUE;
				Me_InitFunc(hWnd);
			}	
			
            PLXPrintf("\r\n WM_PROG_COPS CREG value = %d", value.CREG );

			if(value.CREG == CREG_ROAMING)
			{
				DlmNotify(PS_SETROAMING, ICON_SET);
				bRoaming = TRUE;
			}
			else
			{
				DlmNotify(PS_SETROAMING, ICON_CANCEL);
				bRoaming = FALSE;
			}

			if(value.CREG == CREG_ROAMING || value.CREG == CREG_HOMEREG)
			{
				if(ME_GetCurOprator(hWnd, WM_PROG_COPSINFO, FORMAT_LONGALPHA)< 0)
					SetTimer(hWnd, 4, 5000, NULL);
			}	
			if(value.CREG == CREG_DENIED || value.CREG == CREG_NOREG)
			{
				if(GetSIMState() == 0)
					pm_sim_cop_status = pm_no_sim;
				else if(bSimLockBlocked)
					pm_sim_cop_status = pm_cops_emergencycallonly;
				else
					pm_sim_cop_status = pm_cops_rejectsim;
				
				if(pAppMain && pAppMain->hIdleWnd)
					PostMessage(pAppMain->hIdleWnd, WM_IDLEOPRCHANGED, 0, 0);
				DlmNotify(PS_SETGPRS,GPRS_UNAVAIL);
				if(!bSearchNetwork)
				{
					f_HandSetInlineInitialize ();	
				}
				break;
			}
					
			if (value.CREG == CREG_SEARCHING)
			{
				iOpsLimit ++;
				if(iOpsLimit < MAX_TIMESNUM)
				{
					if(ME_GSMInit_Module_3(pAppMain->hDeskWnd, WM_PROG_COPS) <0)
					{
						printf("\r\n ME_GSMInit_Module_3 failure!");
					}
					if(pm_sim_cop_status != pm_cops_searching)
					{
						bSearchNetwork = TRUE;
						pm_sim_cop_status = pm_cops_searching;
						if(pAppMain && pAppMain->hIdleWnd)
							PostMessage(pAppMain->hIdleWnd, WM_IDLEOPRCHANGED, 0, 0);
					}
				
					DlmNotify(PS_SETGPRS,GPRS_UNAVAIL);

					f_HandSetInlineInitialize ();	
				
				}
				else
				{
					pm_sim_cop_status = pm_cops_noserivce;
					if(pAppMain && pAppMain->hIdleWnd)
						PostMessage(pAppMain->hIdleWnd, WM_IDLEOPRCHANGED, 0, 0);
					iOpsLimit = 0;
					bInitNet = FALSE;
				}
			
			}	
		}
        break;
	case WM_HEADSETVOLUM:
		{
			int volum = 0;

		    printf("\r\n set handset volum!");

			SetHandSet(hWnd);
		}
		break;
	case CALLBACK_MESSAGE:
		{
			CPHS_struct result;    
			memset(&result, 0, sizeof(CPHS_struct));

			if(ME_GetResult(&result, sizeof(CPHS_struct))<0)
				break;

			if( result.Type != RecordContent || result.Content_Len == 0)
				bSimInfoAct = FALSE;//not surpport information numbers
			else
				bSimInfoAct = TRUE;

		}
		break;
	case WM_MAILBOXNUM:
		{
			CPHS_struct result;    

			memset(&result, 0, sizeof(CPHS_struct));

			if(ME_GetResult(&result, sizeof(CPHS_struct)) < 0)
				break;

			if( result.Type != RecordContent || result.Content_Len == 0)
				bMailBoxAct = FALSE;//not surpport mailbox numbers
			else
				bMailBoxAct = TRUE;
			
		}
		break;

	case WM_SETLAN://set default language
		{
			PM_HandleSetLan();
			if(ME_GetExIndicator( hWnd, WM_PROG_VOICEMAIL1, Vmwait1) < 0)
			{
				printf("\r\n ME_GetExIndicator voicemail1 failure!");
			}
		}
		break;

	case WM_READGIL1://read GIL1 file from sim card for SP simlock
		{
			PM_HadleReadGil();
		}
		break;

	case WM_READLAU:
		{
			CPHS_struct result;
			
			memset(&result, 0, sizeof(CPHS_struct));
					
			if(ME_GetResult(&result, sizeof(CPHS_struct)) < 0)
			{
				break;
			}
			
			if(result.Type == FieldNotFound)	
				break;
			else if(result.Type == RecordContent)
			{
		
			}
			else if(result.Type == FileContent)	
			{
				bReadFileType_lan = TRUE;
				Contentlen_Lan = 2 * result.Content_Len;
				
				ME_ReadCPHSFile( hWnd, WM_READLAN, 
					0x2F05, result.Content_Len );//result.Content_Len unsigned char				
			}

		}
		break;
	case WM_READLAN:
		{
			unsigned char *result = NULL;
			
			BYTE    istatus =0;
			
			result = (unsigned char *)malloc(Contentlen *sizeof(unsigned char));
			
			if(result == NULL)
			{
				break;;
			}
			
			memset(result, 0, Contentlen *sizeof(unsigned char));
			
			if(bReadFileType_lan)
			{
				if(ME_GetResult(result, Contentlen* sizeof(unsigned char))<0)
				{
					
					if(result)
					{
						free(result);
						result = NULL;
					}
					break;
				}
				
			}
			
			if(result)
			{
				free(result);
				result = NULL;
			}
			
		}
		break;
    case WM_PROG_VOICEMAIL1:
		{
			int Info = 0;
			
			if(ME_GetResult(&Info, ME_RESPONSE_INT ) < 0)
			{
				printf("\r\nME_GetExIndicator  WM_PROG_VOICEMAIL1 getresult failure!"); 
				if(ME_GetExIndicator( hWnd, WM_PROG_VOICEMAIL2, Vmwait2) < 0)
				{
					printf("\r\n ME_GetExIndicator voicemail2 failure!");
				}
				break;
			}
		
			if(Info ==1)
				DlmNotify(PS_SETVOICEMAIL1, ICON_SET);
			else
				DlmNotify(PS_SETVOICEMAIL1, ICON_CANCEL);

			if(ME_GetExIndicator( hWnd, WM_PROG_VOICEMAIL2, Vmwait2) < 0)
			{
				printf("\r\n ME_GetExIndicator voicemail2 failure!");
			}
		}
		break;
	case WM_PROG_VOICEMAIL2:
		  {
			  int Info = 0;	

			 if(ME_GetResult(&Info, ME_RESPONSE_INT ) < 0)
			 {
				 printf("\r\nME_GetExIndicator WM_PROG_VOICEMAIL2 getresult failure!"); 
				 if(ME_GetExIndicator( hWnd, WM_PROG_CIPHERING, Ciphcall)<0)
				 {
					 printf("\r\n ME_GetExIndicator ciphcall failure!");
				 }
				 break;
			 }
			  
			  if(Info ==1)
				  DlmNotify(PS_SETVOICEMAIL2, ICON_SET);
			  else
				  DlmNotify(PS_SETVOICEMAIL2, ICON_CANCEL);

			  if(ME_GetExIndicator( hWnd, WM_PROG_CIPHERING, Ciphcall)<0)
			  {
				  printf("\r\n ME_GetExIndicator ciphcall failure!");
			  }
		  }
		  break;
		  
    case  WM_GPRSAVAIL:
		{
			MONITORINFO  monitorInfo;
			int value = GetHeadSetVolum();

			memset(&monitorInfo, 0, sizeof(MONITORINFO));

			if(ME_GetResult(&monitorInfo, sizeof(MONITORINFO))<0)
			{
				printf("\r\n WM_GPRSAVAIL ME_GetResult!");
				if(value == 0)
				{
					value = 3;
					SetHeadSetVolum(value);
				}
				
				if(ME_SetSpeakerVolumeSNFV(hWnd, WM_HEADSETVOLUM, value)<0)
				{
					printf("\r\n ME_SetSpeakerVolumeSNFV failure!");
				}
				break;
			}

			if(monitorInfo.BCCH[0] != 0 && stricmp(monitorInfo.BCCH, MONITORSEARCHING) != NULL)
			{
				if(monitorInfo.G[0] != 0 && stricmp(monitorInfo.G, "1") == NULL)
				{
					bGPRS = TRUE;
					bGPRSGet = TRUE;
					if(!bSimLockBlocked)
						DlmNotify(PS_SETGPRS,GPRS_UNATTACH);
				}
				else
					DlmNotify(PS_SETGPRS,GPRS_UNAVAIL);
			}
			else
				DlmNotify(PS_SETGPRS,GPRS_UNAVAIL);
			

			if(value == 0)
			{
				value = 3;
				SetHeadSetVolum(value);
			}
			
			if(ME_SetSpeakerVolumeSNFV(hWnd, WM_HEADSETVOLUM, value)<0)
			{
				printf("\r\n ME_SetSpeakerVolumeSNFV failure!");
			}
		}
		break;
   
	case WM_PROG_CIPHERING://query ciphering info for idle display
		{
			int Info = 0;

			if(ME_GetResult(&Info, ME_RESPONSE_INT ))
			{
				printf("\r\n WM_PROG_CIPHERING ME_GetResult failure!");
				if(ME_GetCPHSparameters( hWnd, WM_SIMREAD, 0x6F38 )<0)
				{
					printf("\r\n ME_GetCPHSparameters failure!");
				}
				break;		
			}
			
			if(Info ==1)
				DlmNotify(PS_SETCIPHER, ICON_CANCEL);
			else
				DlmNotify(PS_SETCIPHER, ICON_SET);


			if(ME_GetCPHSparameters( hWnd, WM_SIMREAD, 0x6F38 )<0)
			{
				printf("\r\n ME_GetCPHSparameters failure!");
			}
		}
		break;

    case WM_PROG_COPSINFO:
        {					
            CUROPRATOR cops;
            
            memset(&cops, 0, sizeof(CUROPRATOR));
            
            if (ME_GetResult(&cops, sizeof(CUROPRATOR)) < 0)
            {
				if(!bSearchNetwork)
				{
					if(bInitNet)
						bInitNet = FALSE;
					f_HandSetInlineInitialize ();
				}
			   break;
            }
            
            if(cops.Name[0] !=0)		
           	{
			    bSearchNetwork  = FALSE;
               	strcpy(ProgDeskIdle.cOperatorLName, cops.Name);
				pm_sim_cop_status = pm_cops_link;

				if(pAppMain && pAppMain->hIdleWnd)
					PostMessage(pAppMain->hIdleWnd, WM_IDLEOPRCHANGED, 0, 0);
				bInitNet = FALSE;
            }
						
			if(!bSearchNetwork)
			{
				PLXPrintf("\r\n copsinfo success,enter init !\r\n");
				f_HandSetInlineInitialize ();
			}           
        }
        break;
		
	case WM_PORG_COPSINFONUM://query operator info for number format
        {					
            CUROPRATOR cops;
            
            memset(&cops, 0, sizeof(CUROPRATOR));
            
            if (ME_GetResult(&cops, sizeof(CUROPRATOR)) < 0)
                break;
           
            if (cops.Name[0] !=0)		
            {
               	strcpy(ProgDeskIdle.cOpenratorSName, cops.Name);
            }
			
		}
        break;

    case WM_ME_TIME://query wireless time
        return	f_OnMainChangeTime ( hWnd );

	case WM_ME_RATE://set flow rate control not for emulate 
		if ( 0==wParam )// OK
		{
			int fd;
			unsigned char startup_flags = 0xBB;
			fd = open(PROG_FILE_NAME,O_RDONLY);
			if ( fd<0 )
			{
				// file is not exist , create it
				fd = open(PROG_FILE_NAME,O_WRONLY|O_CREAT,S_IRWXU);
				if ( fd<0 )
				{
					printf("\r\nError create %s!!!\r\n",PROG_FILE_NAME);
					break;
				}
				write(fd,&startup_flags,1);
				close(fd);
			}
			else
			{
				// file has already existed , we should not receive this message
				close(fd);
				printf("\r\n!!!Call ME_InitRate twice !!!\r\n");
				printf("!!!Please Check Progman and Me !!!\r\n");
			}

			printf("\r\n WM_ME_RATE Mobile Init4!!");
			MobileInit3();
			MobileInit4(hWnd);
		}
		else
		{
			printf("\r\nME_InitRate error.\r\n");
		}
		break;

    case WM_PROG_WHICHPASS://query pin code 
        {
            int nType = 0;
            
            switch ( wParam ) 
            {
            case ME_RS_SUCCESS:
#ifndef _EMULATE_
				if(IsResetPowerOn())
				{
					PLXPrintf("\r\n Is reset power on!\r\n");
					SetSimState(1);
					PLXPrintf("=================run ME_ClearSMSindication==============\r\n");
					ME_ClearSMSindication(pAppMain->hDeskWnd, WM_USER+610);
					ME_GSMInit_Module_3(pAppMain->hDeskWnd, WM_PROG_COPS);
					return TRUE;
				}
#endif
                break;

			default:
				{
					PLXPrintf("=================run ME_ClearSMSindication==============\r\n");
					ME_ClearSMSindication(pAppMain->hDeskWnd, WM_USER+610);
					f_HandSetInlineInitialize ();
					return	TRUE;
				}		
          
            }		
            
            if (ME_GetResult(&nType, sizeof(int)) < 0)
            {
				f_HandSetInlineInitialize ();
                
                return	TRUE;
            }

			DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 15);
			DlmNotify (PMM_NEWS_ENABLE,PMF_MAIN_BKLIGHT|PMF_AUTO_SHUT_MAIN_LIGHT);
                       
            if (nType == NOPIN)
            {
                SetSimState(1);
				SetPin1Status(FALSE);
				bQuestPin = FALSE;
				PLXPrintf("=================run ME_ClearSMSindication==============\r\n");
				ME_ClearSMSindication(pAppMain->hDeskWnd, WM_USER+610);
				ME_GSMInit_Module_3(pAppMain->hDeskWnd, WM_PROG_COPS);
			
                break;
            }
            else if (nType == PIN1)//enter input pin1 code window
            {
				SetPin1Status(TRUE);
				bQuestPin =TRUE;
                PasswordWindow(PIN1);
			    break;
            }
            else if (nType == PUK1)//enter input puk1 code window
            {
				bQuestPin = TRUE;
                PasswordWindow(PUK1);
			    break;
            }
			else
				ME_GetCurWaitingPassword( pAppMain->hDeskWnd, WM_PROG_WHICHPASS);
        }
        break;
	
    default : 
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
//get desk window (hide & revieve msg)
HWND GetDeskWnd(void)
{
	return pAppMain->hDeskWnd;
}

#define NCKCODE   "12345678"
#define SPCKCODE  "12345678"

extern void   SetSPCKRemains(char* strRemains);
extern void   SetNCKRemains(char* strRemains);
extern BOOL	  IsPhoncodeCheck(void);
extern void   AfterPhoneCode(void);
extern BOOL	  PhoneCodeWnd(void);
extern BOOL	  GetPSData(char* pPSCode);
extern BOOL   GetPSCode(char* pPSCode);
extern void   SetPhoncodeCheck(BOOL bSet);

static void PM_HandleSetLan(void)
{
	char value[ME_MBID_LEN], temp[ME_MBID_LEN];
	char netSimlockCode[6];
	int i;
	
	for(i = 0; i< ME_MBID_LEN;i++)
	{
		value[i] = 0;
		temp[i] = 0;
	}
	
 	netSimlockCode[0] = 0;
	
	if(GetPS_Open())
	{
		SetPSData("");
	}
	else
	{
		SetPSCode("");
	}
	
	if(ME_GetResult(&value, sizeof(unsigned char)*ME_MBID_LEN)<0)
	{
		printf("\r\n Setlan ME_GetResult failure!");
		if(!bSPCodeGet)
		{
			if(ME_GetExIndicator( pAppMain->hDeskWnd, WM_PROG_VOICEMAIL1, Vmwait1) < 0)
			{
				printf("\r\n ME_GetExIndicator voicemail1 failure!");
			}
		}
		return;
	}
	
	if(value[0] != 0)
	{
		PNSLC pCode = NULL;
		PSPSIMLOCKCODE pSPCode = NULL;

		bGetSimLock = TRUE;
		
		strncpy(strLan, value, 3);
		strLan[3] = 0;
		
		strncpy(netSimlockCode, value, 5);
		netSimlockCode[5] = 0;
		
		pCode = malloc(sizeof(NSLC));
		if(pCode == NULL)
			return;
		
		memset(pCode, 0, sizeof(NSLC));
		strncpy(pCode->strMCC, value, 3);
		pCode->strMCC[3] =0;
		strncpy(pCode->strMNC, &value[3], 2);
		pCode->strMNC[2] = 0;
		
		if(GetNETSimLock_Open())//check simlock active or not
		{
			SetNetSimLockData(pCode, 1);
		}
			
		free(pCode);
		pSPCode = malloc(sizeof(SPSIMLOCKCODE));
		if(pSPCode == NULL)
			return;
		
		memset(pSPCode, 0 , sizeof(SPSIMLOCKCODE));
	
		if(GetSPSimLock_Open())
		{
			ReadSPSimLockDate(pSPCode);
			strcpy(pSPCode->strMCC, strLan);
			strcpy(pSPCode->strMNC, &netSimlockCode[3]);
			SetSPSimLockData(pSPCode);
		}
	
		free(pSPCode);	
		
		if(GetPS_Open())//check if sim changed avtive or not
			SetPSData(value);
		else
			SetPSCode(value);

		if(IsPhoncodeCheck())
		{	
			char strPscode[31];
			char strPsData[31];

			SetPhoncodeCheck(FALSE);
			
			memset(strPscode, 0, 31);
			memset(strPsData, 0, 31);
			GetPSData(strPsData);
			GetPSCode(strPscode);
			if(stricmp(strPscode, strPsData) != NULL)
			{
				PhoneCodeWnd();
			}
			else
				AfterPhoneCode();
			
		}
		
		if(IsSimLockCheck())//already simlock check
		{
			PNSLC pNetSimlockCode = NULL, pData = NULL, p = NULL, pTemp =NULL;
			int num = 0, count = 0, counter = 0;
			BOOL bReject = TRUE;
			
			
			if(GetSPSimLock_Open())
			{
				PSPSIMLOCKCODE pSPLCData = NULL,pSPLC = NULL;
				
				pSPLCData = malloc(sizeof(SPSIMLOCKCODE));
				if(pSPLCData == NULL)
				{
					EndMobileInit();
					return;
				}
				
				pSPLC = malloc(sizeof(SPSIMLOCKCODE));
				if(pSPLC == NULL)
				{
					free(pSPLCData);
					pSPLCData = NULL;
					EndMobileInit();
					return;
				}
				
				ReadSPSimLockDate(pSPLCData);
				ReadSPSimLock(pSPLC);
				
				if(stricmp(pSPLCData->strMCC, "001") == NULL&&
					stricmp(pSPLCData->strMNC, "01") == NULL)
				{
					if(pSPLCData != NULL)
						free(pSPLCData);
					
					if(pSPLC != NULL)
						free(pSPLC);
					
					EndMobileInit();
					return;
				}

				if(pSPLCData->strGID1[0] == 0)
				{
					EndMobileInit();
					return;
				}

				EndMobileInit();
				
				if( stricmp(pSPLCData->strMCC, pSPLC->strMCC) != NULL ||
					stricmp(pSPLCData->strMNC, pSPLC->strMNC) != NULL||
					stricmp(pSPLCData->strGID1, pSPLC->strGID1) != NULL)
				{
					PLXTipsWin(NULL, pAppMain->hIdleWnd, SIMLOCK_FAIL, ML("SIM rejected"),
						ML("SP simlock"), Notify_Failure, ML("Ok"), NULL, 0);
				}
				
				if(pSPLCData != NULL)
					free(pSPLCData);
				
				if(pSPLC != NULL)
					free(pSPLC);

				return;
				
			}
			
			if(GetNETSimLock_Open())
			{
				count = ReadNetSimLock(NULL);
				num =ReadNetSimLockData(NULL);
				
				if(0 == count)
				{
					EndMobileInit();
					PLXTipsWin(NULL, pAppMain->hIdleWnd, SIMLOCK_FAIL, ML("SIM rejected"),
						ML("Network Simlock"), Notify_Failure, ML("Ok"), NULL, 0);
					return;
				}
				
				pNetSimlockCode = malloc(sizeof(NSLC)*num);
				if(pNetSimlockCode == NULL)
				{
					EndMobileInit();
					return; 
				}
				pData = malloc(sizeof(NSLC)*num);
				
				if(pData == NULL)
				{
					free(pNetSimlockCode);
					EndMobileInit();
					return;
				}
				memset(pNetSimlockCode, 0, sizeof(NSLC)*num);
				memset(pData, 0, sizeof(NSLC)*count);
				
				ReadNetSimLockData(pNetSimlockCode);
				ReadNetSimLock(pData);
				
				p = pNetSimlockCode;
				pTemp = pData;
				counter = count;

				if(stricmp(p->strMCC, "001") == NULL &&
					stricmp(p->strMNC, "01") == NULL)
				{
					if(pData != NULL)
						free(pData);
					
					if(pNetSimlockCode != NULL)
						free(pNetSimlockCode);
					
					EndMobileInit();
					return;
				}
				
				p = pNetSimlockCode;
				pTemp = pData;
				
				while (p && pTemp &&(counter > 0))
				{
					
					if((stricmp(p->strMCC, pTemp->strMCC) == 0)
						||(stricmp(p->strMNC, pTemp->strMNC) == 0))
					{
						
						bReject = FALSE;
					}
					counter--;
					pTemp ++;
				}
				
				if(pData != NULL)
					free(pData);
				
				if(pNetSimlockCode != NULL)
					free(pNetSimlockCode);

				if(bReject)
				{
					EndMobileInit();
					PLXTipsWin(NULL, pAppMain->hIdleWnd, SIMLOCK_FAIL, ML("SIM rejected"),
						ML("Network simlock"), Notify_Failure, ML("Ok"), NULL, 0);
					return;
				}
				
			}
			
		}
		
}

}
static void PM_HadleReadGil(void)
{
	unsigned char value[ME_CPHSINFOMAXLEN];
	int i;
	PSPSIMLOCKCODE pSPCode = NULL;
	
	for(i = 0; i< ME_CPHSINFOMAXLEN;i++)
		value[i] = 0;
	
	if(ME_GetResult(&value, sizeof(unsigned char)*ME_CPHSINFOMAXLEN)<0)
	{
		printf("\r\n ME_GetCPHSInfo GetResult failure!");
		if(!bGetSimLock)
		{
			if(ME_GetSubscriberId (pAppMain->hDeskWnd, WM_SETLAN)<0)
			{
				printf("\r\n ME_GetSubscriberId failure!");
			}
		}
		
		return;
	}
	
	SetSPCK(SPCKCODE);
	SetNCK(NCKCODE);
	
	if(GetNETSimLock_Open()||GetSPSimLock_Open())
	{
		if(GetNETSimLock_Open())
		{
			ClearNetSimLockData();
			SetSPCKRemains("10");
		}
		else
		{
			ClearSPSimLockData();
			SetNCKRemains("10");
		}
	}
	else
	{
		SetSPCKRemains("10");
		SetNCKRemains("10");
	}
	
	
	bSPCodeGet = TRUE;
	
	pSPCode = malloc(sizeof(SPSIMLOCKCODE));
	if(pSPCode == NULL)
	{
		printf("\r\n WM_READGIL1 malloc failure!");
		return;
	}
	
	memset(pSPCode, 0 , sizeof(SPSIMLOCKCODE));
	
	if(GetSPSimLock_Open())//check simlock active or not
	{
		ReadSPSimLockDate(pSPCode);
		strncpy(pSPCode->strGID1,&value[6],2);
		pSPCode->strGID1[2] = 0;
		SetSPSimLockData(pSPCode);
	}
	else
	{
		ReadSPSimLock(pSPCode);
		strncpy(pSPCode->strGID1,&value[6],2);
		pSPCode->strGID1[2] = 0;
		
		SetSPSimLockCode(pSPCode);
	}
	free(pSPCode);
	
	if(!bGetSimLock)
	{
		if(ME_GetSubscriberId (pAppMain->hDeskWnd, WM_SETLAN)<0)
		{
			printf("\r\n ME_GetSubscriberId failure!");
		}
	}

}

#define  IDC_SIMLOCK_QUIT   801
#define  IDC_SIMLOCK_SAVE   802
#define  IDC_SIMLOCKCODE    803
#define  MAX_TIMES          10
#define  SIMLOCK_BLOCKED    WM_USER + 102

static HWND hSimlockWnd  = NULL,hSimlockFocusWnd = NULL;
static int remain = 10;

static BOOL CreateSimlockControl(HWND hWnd);
static LRESULT AppSimlockWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

//simlock unlock code request view
BOOL  CreateSimLockCodeWnd(HWND hWnd)
{
	WNDCLASS wc;
	char strTitle[20];

	memset(strTitle, 0, 20);

	if(GetNETSimLock_Open())
	{
		char strRemains[4];

		memset(strRemains, 0, 4);
		strcpy(strTitle, "Network Simlock");
		GetNCKRemains(strRemains);
		if(strRemains[0] != 0)
			remain = atoi(strRemains);
	}
	else
	{
		char strRemains[4];

		memset(strRemains, 0, 4);
		strcpy(strTitle, "SP Simlock");
		GetSPCKRemains(strRemains);
		if(strRemains[0] != 0)
			remain = atoi(strRemains);
	}
	
	wc.style         = CS_OWNDC;
	wc.lpfnWndProc   = AppSimlockWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "AppSimlockWndClass";
	
	if(IsWindow(hSimlockWnd))
	{
		ShowWindow(hSimlockWnd, SW_SHOW);
		UpdateWindow(hSimlockWnd);	
		return TRUE;
	}
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	
	hSimlockWnd = CreateWindow(
		"AppSimlockWndClass", 
		ML(strTitle),
		WS_CAPTION |/* WS_BORDER|*/ PWS_STATICBAR, 
		PLX_WIN_POSITION,
		NULL, 
		NULL,
		NULL, 
		NULL);
	
	
	if (NULL == hSimlockWnd)
	{
		UnregisterClass("AppSimlockWndClass",NULL);
		return FALSE;
	}
	
	ShowWindow(hSimlockWnd, SW_SHOW);
	UpdateWindow(hSimlockWnd);

	return TRUE;

}
/********************************************************************
* Function   AppSimlockWndProc 
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static LRESULT AppSimlockWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT lResult;
	HDC hdc;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        CreateSimlockControl(hWnd);
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
		{
			SetFocus(hSimlockFocusWnd);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
				MAKEWPARAM(IDC_SIMLOCK_SAVE,1),(LPARAM)ML("") );
			
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
				(WPARAM)IDC_SIMLOCK_QUIT,(LPARAM)ML("Cancel") );
		}
        break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

   	case WM_PAINT:
		hdc = BeginPaint( hWnd, NULL);
        EndPaint(hWnd, NULL);
		break;

    case WM_DESTROY :
		hSimlockFocusWnd = NULL;
		hSimlockWnd = NULL;
		EndMobileInit();
		if ( NULL != GetPowerOnWnd() )
		{
			PostMessage( GetPowerOnWnd(), WM_CLOSE, 0, 0 );
		}
        UnregisterClass("AppSimlockWndClass",NULL);
		
        break;
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_F10:
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				ME_SetOpratorMode( pAppMain->hDeskWnd, WM_SIMLOCKBLOCK, 2, NULL);
				bSimLockBlocked  =TRUE;
				pm_sim_cop_status = pm_cops_emergencycallonly;
				if(pAppMain && pAppMain->hIdleWnd)
					PostMessage(pAppMain->hIdleWnd, WM_IDLEOPRCHANGED, 0, 0);
				DlmNotify(PS_SETGPRS,GPRS_UNAVAIL);	
			}
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
	case SIMLOCK_BLOCKED:
		SendMessage(hWnd, WM_COMMAND, IDC_SIMLOCK_QUIT, 0);			
		break;

    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {	
		case IDC_SIMLOCKCODE:
			if(HIWORD(wParam) == EN_CHANGE ||HIWORD(wParam) == EN_UPDATE )
			{
				char pText[8+1];
				int  i = 0;
				
				pText[0] = 0;
				
				GetWindowText(GetDlgItem(hWnd, IDC_SIMLOCKCODE), pText,
					GetWindowTextLength(GetDlgItem(hWnd, IDC_SIMLOCKCODE)) +1);
				
				if(strlen(pText) == 8)
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Ok"));
				else
					SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)NULL);
			}
			break;
		case IDC_SIMLOCK_SAVE:
			{
				char pText[8+1];
				char pUnlockCode[8+1];
				int  i = 0;
				char strFailConter[2];
				
				pText[0] = 0;
				
				GetWindowText(GetDlgItem(hWnd, IDC_SIMLOCKCODE), pText,
					GetWindowTextLength(GetDlgItem(hWnd, IDC_SIMLOCKCODE)) +1);
				
				if(strlen(pText) < 8)
					break;
				
				if(GetNETSimLock_Open())
					GetNCK(pUnlockCode);
				else
					GetSPCK(pUnlockCode);
				
				if(stricmp(pUnlockCode, pText) != NULL)
				{
					remain--;
					memset(strFailConter, 0, 2);
					itoa(remain, strFailConter, 10);

					if(strFailConter[0] != 0)
					{
						if(GetNETSimLock_Open())
						{
							SetNCKRemains(strFailConter);
						}
						else
						{
							SetSPCKRemains(strFailConter);
						}
					}
					
					if(remain <= 0)
					{
						if((GetNETSimLock_Open()))
							PLXTipsWin(NULL, hWnd, SIMLOCK_BLOCKED, ML("Code blocked"), ML("Network Simlock"),
							Notify_Failure, ML("Ok"), NULL, 0);
						else
							PLXTipsWin(NULL, hWnd, SIMLOCK_BLOCKED, ML("Code blocked"), ML("SP Simlock"),
							Notify_Failure, ML("Ok"), NULL, 0);
						break;
					}
					SetWindowText(GetDlgItem(hWnd, IDC_SIMLOCKCODE), "");

					if((GetNETSimLock_Open()))
						PLXTipsWin(NULL, hWnd, NULL, ML("Wrong code"), ML("Network Simlock"),Notify_Failure,
						NULL, NULL, 20);
					else
						PLXTipsWin(NULL, hWnd, NULL, ML("Wrong code"), ML("SP Simlock"),Notify_Failure,
						NULL, NULL, 20);
				}
				else
				{
					if(GetNETSimLock_Open())
					{
						SetNCKRemains("10");
					}
					else
					{
						SetSPCKRemains("10");
					}
					
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				}
			}
			break;
					
		case IDC_SIMLOCK_QUIT:	
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			ME_SetOpratorMode( pAppMain->hDeskWnd, WM_SIMLOCKBLOCK, 2, NULL);
			bSimLockBlocked  =TRUE;
			pm_sim_cop_status = pm_cops_emergencycallonly;
			if(pAppMain && pAppMain->hIdleWnd)
				PostMessage(pAppMain->hIdleWnd, WM_IDLEOPRCHANGED, 0, 0);
			DlmNotify(PS_SETGPRS,GPRS_UNAVAIL);
			if(GetNETSimLock_Open())
				SetNetSimLockBlock(TRUE);
			else
				SetSPSimLockBlock(TRUE);
			break;
		}
		break;
		
		default :
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
	}
	return lResult;
}
/********************************************************************
* Function   CreateRenameFolderControl  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL CreateSimlockControl(HWND hWnd)
{
	
	HWND hSimLockCode= NULL;
	IMEEDIT ie_Name;

	int num = 0;
	
	SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
		MAKEWPARAM(IDC_SIMLOCK_SAVE,1),(LPARAM)ML("") );
	
	SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
		(WPARAM)IDC_SIMLOCK_QUIT,(LPARAM)ML("Cancel") );

	
	memset(&ie_Name, 0, sizeof(IMEEDIT));
    
    ie_Name.hwndNotify   = hWnd;
	ie_Name.dwAttrib |= 0;

    ie_Name.dwAscTextMax = 0;
    ie_Name.dwUniTextMax = 0;
    
    ie_Name.pszImeName = "Digit";
    ie_Name.pszTitle = NULL;

	
	hSimLockCode = CreateWindow(
		"IMEEDIT",
		NULL,
		WS_VISIBLE|WS_CHILD|WS_TABSTOP|ES_TITLE|ES_AUTOHSCROLL,
		0,0, PLX_WIN_WIDTH -2, 70,                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		hWnd,
		(HMENU)IDC_SIMLOCKCODE,
		NULL,
		(PVOID)&ie_Name);
	if (NULL == IDC_SIMLOCK_SAVE)
		return FALSE;

	SendMessage(hSimLockCode, EM_SETTITLE, 0, (LPARAM)ML("Enter unlock code:"));
	SendMessage(hSimLockCode, EM_LIMITTEXT, 8, 0);
	SendMessage(hSimLockCode, EM_SETSEL, -1, -1);

	hSimlockFocusWnd = hSimLockCode ;
	
	return TRUE;
}
/********************************************************************
* Function   ST_ParseStatus  
* Purpose    parse the sim file content
* Params     
* Return     
* Remarks      
**********************************************************************/
static void   ST_ParseStatus(unsigned char *strCode, int iconter)
{
	#define IFALLOCATED	0x01
	#define IFACTIVATED	0x02	
		
	unsigned char *temp = NULL;
	unsigned char CharTempH = NULL;
	unsigned char CharTempL = NULL;
	unsigned char *str = NULL, i = 0;
	BOOL bSet= FALSE;
	int  j = 0;

	temp = (unsigned char *)malloc((iconter/2)*sizeof(unsigned char));
	if(temp == NULL)
		return;	
	
	for(j=0; j<(iconter/2); j++)
	{
		CharTempH = *(strCode+2*j);	
		if(CharTempH >= '0' && CharTempH <= '9')
			CharTempH = CharTempH - '0';
		else if(CharTempH >= 'a' && CharTempH <= 'f')
			CharTempH = CharTempH - 'a' + 10;	
		else if(CharTempH >= 'A' && CharTempH <= 'F')	 	
			CharTempH = CharTempH - 'A' + 10;
		else 
		{
			//something wrong
			free(temp);
			return;	
		}	
		
		CharTempL = *(strCode+2*j+1);	
		if(CharTempL >= '0' && CharTempL <= '9')
			CharTempL = CharTempL - '0';
		else if(CharTempL >= 'a' && CharTempL <= 'f')
			CharTempL = CharTempL - 'a' + 10;	
		else if(CharTempL >= 'A' && CharTempL <= 'F')	 	
			CharTempL = CharTempL - 'A' + 10;
		else 
		{
			//something wrong
			free(temp);
			return;	
		}		
		
		//convert the service string
		temp[j] = CharTempL + (CharTempH<<4);
	}
	
	//FDN service
	if((temp[0] >> 4) & IFALLOCATED)
	{
		SimFun.FDN.SupportStatus = Allocated;
		if((temp[0] >> 4) & IFACTIVATED)
			SimFun.FDN.CurrentStatus = Activated;
		else
			SimFun.FDN.CurrentStatus = Inactivated;	
	}
	else
		SimFun.FDN.SupportStatus = NotAllocated;
		
	//AOC service
	if((temp[1]) & IFALLOCATED)
	{
		SimFun.AOC.SupportStatus = Allocated;
		if((temp[1]) & IFACTIVATED)
			SimFun.AOC.CurrentStatus = Activated;
		else
			SimFun.AOC.CurrentStatus = Inactivated;	
	}
	else
		SimFun.AOC.SupportStatus = NotAllocated;			

	//CHV1 service
	if((temp[0]) & IFALLOCATED)
	{
		SimFun.CHV1.SupportStatus = Allocated;
		if((temp[0]) & IFACTIVATED)
			SimFun.CHV1.CurrentStatus = Activated;
		else
			SimFun.CHV1.CurrentStatus = Inactivated;	
	}
	else
		SimFun.CHV1.SupportStatus = NotAllocated;

	printf("\r\n SimFun.CHV1.SupportStatus = %d,SimFun.CHV1.CurrentStatus = %d ", 
		SimFun.CHV1.SupportStatus,SimFun.CHV1.CurrentStatus);
	
	free(temp);
	temp = NULL;
}
/********************************************************************\
* Function     GetSimStatus_FDN
* Purpose      for fixed dial
* Params      
* Return       
**********************************************************************/
void GetSimStatus_FDN(SimService *PSimSer)
{
	PSimSer->SupportStatus = SimFun.FDN.SupportStatus;
	PSimSer->CurrentStatus = SimFun.FDN.CurrentStatus;	
}
void SetSimStatus_FDN(SimService *PSimSer)
{
	 SimFun.FDN.SupportStatus = PSimSer->SupportStatus;
	 SimFun.FDN.CurrentStatus = PSimSer->CurrentStatus;
}
/********************************************************************\
* Function     GetSimStatus_AOC
* Purpose      for call divert
* Params      
* Return       
**********************************************************************/
void GetSimStatus_AOC(SimService *PSimSer)
{
	PSimSer->SupportStatus = SimFun.AOC.SupportStatus;
	PSimSer->CurrentStatus = SimFun.AOC.CurrentStatus;	
}
void SetSimStatus_AOC(SimService *PSimSer)
{
	SimFun.AOC.SupportStatus = PSimSer->SupportStatus;
	SimFun.AOC.CurrentStatus = PSimSer->CurrentStatus;
}
/********************************************************************\
* Function     GetSimStatus_CHV1
* Purpose      for pin1 status 
* Params      
* Return       
**********************************************************************/
void GetSimStatus_CHV1(SimService *PSimSer)
{
	PSimSer->SupportStatus = SimFun.CHV1.SupportStatus;
	PSimSer->CurrentStatus = SimFun.CHV1.CurrentStatus;	
}
/********************************************************************\
* Function     GetFavoriteLan
* Purpose      for default language
* Params      
* Return       
**********************************************************************/
void GetFavoriteLan(char* str)
{
	if(strLan[0] != 0)
		strcpy(str, strLan);
	else
		str = NULL;
}

/********************************************************************\
* Function     PM_GetNetWorkStatus
* Purpose      for mms to get status :home or roaming
* Params      
* Return       0, no network;
               1,home
			   2,roaming
			   3,reject sim card
			   4,no service
			   5,emergency call only
**********************************************************************/
#define NONETWORK   0
#define HOMEZONE    1
#define ROAMING     2
#define REJECTSIMCARD 3
#define NOSERVICE   4
#define ONLY_EMERGENCYCALL 5
int PM_GetNetWorkStatus()
{
	if(bRoaming)
		return ROAMING;
	if(pm_sim_cop_status == pm_cops_rejectsim)
		return REJECTSIMCARD;
	if(pm_sim_cop_status == pm_cops_noserivce)
		return NOSERVICE;
	if(pm_sim_cop_status == pm_cops_emergencycallonly)
		return ONLY_EMERGENCYCALL;
	if(pm_sim_cop_status == pm_cops_link)
		return HOMEZONE;

	return NONETWORK;
}

/*********************************************************************\
* Function     DrawNotifyIconEx
* Purpose      
* Params      
* Return       
**********************************************************************/
typedef struct NotifyIconParam
{
    BOOL bSet[5]; 
	int  iPriority[5];
	HANDLE hIcon[5];
    char aIconName[5][128];  
}NOTIFYICONPARAM;

static NOTIFYICONPARAM  NotifyIconInfo[MAX_ICONID];
static void  DrawNotifyIconEx(HDC hdc)
{
    
	int i = 0, j= 0, num =0;
	RECT  rcIcon;

	while (i< MAX_ICONID && num < 9)
	{
		for(j = 0; j<5; j++)
		{
			if(NotifyIconInfo[i].bSet[j])
			{
				num++;
				if (!GetIconRect(num, &rcIcon))
					return ;

				DrawBitmapFromFile(hdc,rcIcon.left ,rcIcon.top , NotifyIconInfo[i].aIconName[j], SRCCOPY);			
				break;		
			}
		}
		i++;
	}
}
/*********************************************************************\
* Function     DrawNotifyIcon
* Purpose      draw idle icon in the device mode indicators area
* Params      
* Return       
**********************************************************************/
void DrawNotifyIcon(WPARAM wParam, LPARAM lParam)
{

	int id,i = 0, j= 0, num =0;
	RECT rcIcon;


    switch (wParam)
    {
	case PS_SETCALLDIVERT://all lines call divert
		{
			id = ICONID_CALLDIVERT;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_divert_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}
		break;

	case PS_CALLDIVERTLINE1://call divert line1
		{
			id = ICONID_CALLDIVERT;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[1] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[1], "/rom/progman/icon/idle_d_divert1_15x15.bmp");
				NotifyIconInfo[id].iPriority[1] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[1] = FALSE;
			}
		}
		break;

	case PS_CALLDIVERTLINE2://call divert line2
		{
			id = ICONID_CALLDIVERT;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[2] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[2], "/rom/progman/icon/idle_d_divert2_15x15.bmp");
				NotifyIconInfo[id].iPriority[2] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[2] = FALSE;
			}
		}
		break;

	case PS_SETROAMING://roaming
		{
			id = ICONID_ROAMING;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_roaming_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}
		break;
		
	case PS_SETCIPHER:
		{
			id = ICONID_CIPHERING;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_ciphering_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}
		break;

	case PS_SETHOMEZONE://homezone
		{
			id = ICONID_HOMEZONE;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_homezone_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}
		break;

	case PS_SETACTIVELINE1://active line1
		{
			id = ICONID_ACTIVELINE;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_line1_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}
		break;

	case PS_SETACTIVELINE2://active line2
		{
			id = ICONID_ACTIVELINE;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[1] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[1], "/rom/progman/icon/idle_d_line2_15x15.bmp");
				NotifyIconInfo[id].iPriority[1] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[1] = FALSE;
			}
		}
		break;
		
	case PS_SETBLUETOOTHON://set bluetooth on
		{
			id = ICONID_BLUETOOTH;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_bton_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}
		break;

	case PS_SETBLUETOOTHOFF://set bluetooth off
		{
			id = ICONID_BLUETOOTH;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[1] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[1], "/rom/progman/icon/idle_d_btactive_15x15.bmp");
				NotifyIconInfo[id].iPriority[1] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[1] = FALSE;
			}
		}
		break;

	case PS_SETHEADSET://headset
		{
			id = ICONID_HEADSET;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_headset_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}
		break;

	case PS_SETCARKIT://carkit
		{
			id = ICONID_CARKIT;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_carkit_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}
		break;

	
	case PS_MSGUNREAD:
		{
			id = ICONID_MSG;

			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[1] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[1], "/rom/progman/icon/idle_d_unreadmsg_15x15.bmp");
				NotifyIconInfo[id].iPriority[1] = 2;
			}
			else
			{
				NotifyIconInfo[id].bSet[1] = FALSE;
			}
		}
		
		break;
	case PS_MSGOVERFLOW:
		{
			id = ICONID_MSG;

			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_msgoverflow_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 3;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;	
			}
		}
		break;
	case PS_MSGOUTBOX:
		{
			id = ICONID_MSG;

			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[4] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[4], "/rom/progman/icon/idle_d_outbox_15x15.bmp");
				NotifyIconInfo[id].iPriority[4] = 1;
			}
			else
			{
				NotifyIconInfo[id].bSet[4] = FALSE;	
			}
		}
	
		break;
	case PS_SETVOICEMAIL:
		{
			id = ICONID_VOICEMAIL;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_voicemail_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;	
			}
		}
		break;

	case PS_SETVOICEMAIL1:
		{
			id = ICONID_VOICEMAIL;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[1] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[1], "/rom/progman/icon/idle_d_voicemail1_15x15.bmp");
				NotifyIconInfo[id].iPriority[1] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[1] = FALSE;	
			}
		}
		break;
	case PS_SETVOICEMAIL2:
		{
			id = ICONID_VOICEMAIL;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[2] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[2], "/rom/progman/icon/idle_d_voicemail2_15x15.bmp");
				NotifyIconInfo[id].iPriority[2] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[2] = FALSE;	
			}
		}
		break;
		
	case PS_KEYLOCK:
		{
			id = ICONID_KEYLOCK;

			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_keylock_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}	
		}
		break;
	case PS_ALARMCLOCK:
		{
			id = ICONID_ALARMCLK;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_alarmclock_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}
		break;
	case PS_SILENCE:
		{
			id = ICONID_SOUNDPROFILE;

			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_silentprofile_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
		}

		break;

	case PS_SPROHTER:
		{
			id = ICONID_SOUNDPROFILE;

			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[1] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[1], "/rom/progman/icon/idle_d_profile_15x15.bmp");
				NotifyIconInfo[id].iPriority[1] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[1] = FALSE;
			}
		}
		break;

	case PS_POWERON:
		
		{
			id = ICONID_POWER;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[0] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[0], "/rom/progman/icon/idle_d_autoswitchon_15x15.bmp");
				NotifyIconInfo[id].iPriority[0] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[0] = FALSE;
			}
			
		}
		break;

	case PS_POWEROFF:
		{
			id = ICONID_POWER;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[1] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[1], "/rom/progman/icon/idle_d_autoswitchoff_15x15.bmp");
				NotifyIconInfo[id].iPriority[1] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[1] = FALSE;
			}
			
		}
		
		break;

	case PS_POWERONOFF:
		{
			id = ICONID_POWER;
			
			if(lParam == ICON_SET)
			{
				NotifyIconInfo[id].bSet[2] = TRUE;
				strcpy(NotifyIconInfo[id].aIconName[2], "/rom/progman/icon/idle_d_switchonoff_15x15.bmp");
				NotifyIconInfo[id].iPriority[2] = 0;
			}
			else
			{
				NotifyIconInfo[id].bSet[2] = FALSE;
			}
			
		}
		
		break;
	}
	
	for(i = 1; i <= 9 ; i++)
	{
		GetIconRect(i, &rcIcon);
		
		if(pAppMain && pAppMain->hIdleWnd)
			InvalidateRect(pAppMain->hIdleWnd, &rcIcon, TRUE);
		if(hPhonelockWnd)
			InvalidateRect(hPhonelockWnd, &rcIcon, TRUE);
	}
	
}
/*********************************************************************\
* Function     HandleSignal
* Purpose      query signal
* Params      
* Return       
**********************************************************************/
#define DEFAULT_DELATTIME  10
static BOOL   bGprsChange;
static 	int    bGprsStatus;
static int    iSignel = 0, iDelay = 0;
static void HandleSignal(HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL bURC)
{			

    NOTIFYICONDATA data;
    char   IconName[64];
	BOOL    bEnable = FALSE;

	memset(IconName, 0 , 64);
	

	if(wParam == 3)//gprs 
	{
		bGprsChange = TRUE;
		bGprsStatus = lParam;
		bURC = TRUE;
	}

	// get singal , result is 0£­4£¬
    // 0 means singal is unknowed
	if(!bURC)
	{
		if(ME_GetResult(&iSignel, sizeof(int)) < 0)
		{
			iSignel = 0;
		}
	}

    if( iSignel > 4)
        iSignel = 4;
    
    if ( iSignel < 0) 
	{
        iSignel = 0;
	}

	bEnable = FALSE;

	if(iSignel == 0 && pm_sim_cop_status == pm_cops_noserivce)
	{
	     ;
	}
	else
	{
		if(iSignel == 0 || pm_sim_cop_status == pm_cops_noserivce)
		{
			bEnable = TRUE;
		}
	}
	
	if(!bInitNet && !bSearchNetwork &&  bEnable && (wParam != 3))
	{
		DlmNotify(PS_SETGPRS,GPRS_UNAVAIL);
		DlmNotify(PS_SETROAMING, ICON_CANCEL);
		bRoaming = FALSE;

		bSearchNetwork = TRUE;
		pm_sim_cop_status = pm_cops_searching;
		if(pAppMain && pAppMain->hIdleWnd)
			PostMessage(pAppMain->hIdleWnd, WM_IDLEOPRCHANGED, 0, 0);
		ME_GSMInit_Module_3(pAppMain->hDeskWnd, WM_PROG_COPS);
		
		ME_GPRS_Monitor(pAppMain->hDeskWnd, WM_GPRSAVAIL);
		
	}

	
	printf("\r\n singal = %d", iSignel);
    
    data.dwSize = sizeof(NOTIFYICONDATA);
    data.hWnd = hWnd;    
    data.uID = ICONID_SGN;

	if(iSignel == 4)
	{
		if (bGprsChange)
		{
			switch(bGprsStatus)
			{
			case GPRS_SUSPEND:
				strcpy(IconName, "/rom/progman/icon/gprs_suspended_full.ico");	
				break;
			case GPRS_TRANSFERING:
				{
					BOOL bBlink= FALSE;
					if(bBlink)
						strcpy(IconName, "/rom/progman/icon/gprs_attached_transfer_full.ico");
					else
					{
						strcpy(IconName, "/rom/progman/icon/gprs_available_unattachfull.ico");
					}
					bBlink = !bBlink;
				}
				break;
			case GPRS_ATTACH:
				strcpy(IconName, "/rom/progman/icon/gprs_attached_transfer_full.ico");
				break;
			case GPRS_UNATTACH:
				strcpy(IconName, "/rom/progman/icon/gprs_available_unattachfull.ico");
				break;
			case GPRS_UNAVAIL:
				strcpy(IconName, "/rom/progman/icon/gsm_available_full_bars.ico");	
				break;
			}
		}
		else
			strcpy(IconName, "/rom/progman/icon/gsm_available_full_bars.ico");	
	}
	else
	{	
		if (bGprsChange)
		{
			switch(bGprsStatus)
			{
			case GPRS_SUSPEND:
				sprintf(IconName, "/rom/progman/icon/gprs_suspended_%d%c.ico", iSignel*25,0x25);
				break;
			case GPRS_TRANSFERING:
				{
					BOOL bBlink = FALSE;
					if(bBlink)
						sprintf(IconName, "/rom/progman/icon/gprs_attached_transfer_%d%c.ico", iSignel*25,0x25);
					else
						sprintf(IconName, "/rom/progman/icon/gprs_available_unattach%d%c.ico", iSignel*25,0x25);
					bBlink = !bBlink;
				}
				
				break;
			case GPRS_ATTACH:
				sprintf(IconName, "/rom/progman/icon/gprs_attached_transfer_%d%c.ico", iSignel*25,0x25);
				break;
			case GPRS_UNATTACH:
				sprintf(IconName, "/rom/progman/icon/gprs_available_unattach%d%c.ico", iSignel*25,0x25);
				break;
			
			default:
				sprintf(IconName, "/rom/progman/icon/gsm_available_%d%c_bars.ico", iSignel*25,0x25);
				break;
			}
		}
		else
			sprintf(IconName, "/rom/progman/icon/gsm_available_%d%c_bars.ico", iSignel*25,0x25);
	}
    
    strcpy(data.IconName, IconName);
	
	SendMessage(pAppMain->hIdleWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
 	SendMessage(pAppMain->hIdleWnd, PWM_SETSIGNAL, 
		MAKEWPARAM(IMAGE_BITMAP, SIGNALICON),(LPARAM)data.IconName);

	if(PM_GetPhoneLockStatus())
	{
		SendMessage(GetActiveWindow(), PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
		SendMessage(GetActiveWindow(), PWM_SETSIGNAL, 
		MAKEWPARAM(IMAGE_BITMAP, SIGNALICON),(LPARAM)data.IconName);
	}  
}
/**************************************************************************
* Function		: HandleBattery
* Description	: handle battery info
*				: HWND hWnd
* Return type	: static void 
**************************************************************************/
static  void    SetBatValue(int BCValue);

#define PMS_BAT_NORMAL_VALUE        3700
#define PMS_BAT_WARN_VALUE			3600
#define PMS_BAT_SHUT_VALUE			3500
void SetTipsLowPower(BOOL bSet)
{
	bFirstTip = bSet;
}
BOOL GetTipsLowPower(void)
{
	return bFirstTip;
}
static  void    HandleBattery(HWND hWnd)
{	
    BATTERY_INFO  bInfo;
    char          IconName[64], ind[4];
    int 		  iBat, aBat[6] = {0,20, 40, 60, 80, 100};
	    
    memset(&bInfo, 0x00, sizeof(BATTERY_INFO));
    
	if (ME_GetResult(&bInfo, sizeof(BATTERY_INFO)) < 0)
    {
        return;
    }
    
    for ( iBat = 0; iBat < 5; iBat++ )
        if ( aBat[iBat] >= bInfo.BCLever )
            break;
        
		SetBatValue(bInfo.BCValue);

		if(bInfo.BCValue <= PMS_BAT_WARN_VALUE)
		{
			DlmNotify ( PMM_NEWS_ENABLE, PMF_KEYLIGHT|PMF_MAIN_BKLIGHT );
            
            if (!PM_IsPowerOff()/* && bFirstTip*/)
            {
				PLXTipsWinOem(NULL, NULL, NULL, ML("Battery voltage low,\r\nPlease charging now..."), ML("Battery"),Notify_Info,
					NULL,NULL, WAITTIMEOUT );
                SetRTCTimer(hWnd, 1, 60 * 1000, NULL);
                bFirstTip = FALSE;
            }

		}
		if(bInfo.BCValue >= PMS_BAT_NORMAL_VALUE)
		{
			if(!bFirstTip)
			{
				KillRTCTimer(hWnd, 1);
				bFirstTip = TRUE;
			}
		}
         
        if (!PM_IsPowerOff() && (bInfo.BCValue<= PMS_BAT_SHUT_VALUE))
		{
			DlmNotify ( PMM_NEWS_ENABLE, PMF_KEYLIGHT|PMF_MAIN_BKLIGHT );
			
			PLXTipsWinOem(NULL, NULL, NULL, ML("Battery voltage low"), NULL,Notify_Info,NULL,NULL, WAITTIMEOUT );		
			
			if ( !(g_nAttrib & PMA_HAS_CHARGEUP) )
			{
				printf("\r\n low voltage power off!\r\n");
				PlayPowerOff(TRUE); 
			}
			return;
		}
        
        if ( g_nAttrib & PMA_HAS_CHARGEUP )
        {
            if (iBat == 5)
			{
				PLXTipsWin(NULL, hWnd, NULL, ML("Charging finished"), ML("Battery"), Notify_Info, 
					NULL,NULL,WAITTIMEOUT);
                KillTimer( hWnd, 2 );
			}
            else 
                SetTimer(hWnd, 2, 1000, NULL);
        }   

        if(iBat == 5)
            iBat--;         //draw bat4.ico
        
		if(iBat == 4)
		{
			strcpy(IconName, "/rom/progman/battery/battery_full.ico");
		}
		else if(iBat == 0)
		{
			strcpy(IconName, "/rom/progman/battery/battery_0%.ico");
		}
		else
		{
			strcpy(IconName, "/rom/progman/battery/battery_");    
			sprintf(ind, "%d%c", iBat*25, 0x25);
			strcat(IconName, ind);	
			strcat(IconName, ".ico");
		}
    
		SendMessage(pAppMain->hIdleWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);

		SendMessage(pAppMain->hIdleWnd, PWM_SETSIGNAL, 
			MAKEWPARAM(IMAGE_BITMAP, POWERICON),(LPARAM)IconName);
	
		if(PM_GetPhoneLockStatus())
		{
			SendMessage(GetActiveWindow(), PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
			SendMessage(GetActiveWindow(), PWM_SETSIGNAL, 
				MAKEWPARAM(IMAGE_BITMAP, POWERICON),(LPARAM)IconName);
		}
}

/*********************************************************************\
* Function     GetIconRect
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL GetIconRect(int id, RECT *rc)
{  	
    RECT	rcRgn[9] = 
    {
        {4, 2, 19, 17},
        {23, 2, 38, 17},
        {42, 2, 57, 17},

        {61, 2, 76, 17},
        {80, 2, 95, 17},
        {99, 2, 113, 17},
		{117, 2, 133, 17},

		{137, 2, 152, 17},
		{156, 2, 171, 17}	
    };
    id --;
    
    if ( rc == NULL )
        return FALSE;
    if (id < 0 || id > MAX_ICONID)
        return FALSE;
    
    rc->top = rcRgn[id].top;
    rc->bottom = rcRgn[id].bottom;
    rc->left = rcRgn[id].left;
    rc->right = rcRgn[id].right;
    
    return TRUE;
}

/*********************************************************************
* Function	   PROG_InitDeskIdle
* Purpose    initial idle struct
* Params	 PPROGDESKIDLE  &pDeskIdle:
* Return	 none 	   
* Remarks	  
**********************************************************************/
void PROG_InitDeskIdle(PROGDESKIDLE  *pDeskIdle)
{

    pDeskIdle->count = 0;           //per 5 senconds¡£
    pDeskIdle->Idle_limit  = 3;     //default 15 second 
    pDeskIdle->cPlmnStr[0] = 0;  
	pDeskIdle->cOperatorLName[0] = 0;
	pDeskIdle->cOpenratorSName[0] = 0;
	pDeskIdle->cOperatorCode[0] = 0;
	pDeskIdle->cCountryCode[0] = 0;
    pDeskIdle->cIdleModeTxt[0] = 0;
	pDeskIdle->cSPN[0] = 0;
	pDeskIdle->strCellId[0] = 0;
	strcpy(pDeskIdle->cBgImage, "/rom/progman/desk1.bmp");    
    pDeskIdle->bHideText = 0;       //default show idle text
	pDeskIdle->cDeskStr[0] = 0;
    SetRect(&(pDeskIdle->rBg) , 0, STATUSBAR_HEIGHT, DESK_WIDTH, DESK_HEIGHT - STATUSBAR_HEIGHT);      
    SetRect(&(pDeskIdle->rMNO), 0, LINE1_Y, 88, LINE1_Y + LINEHIGH);
    SetRect(&(pDeskIdle->rSPN), 88,LINE1_Y , DESK_WIDTH,  LINE1_Y + LINEHIGH);
	SetRect(&(pDeskIdle->rSPNMNO), 0, LINE1_Y, DESK_WIDTH, LINE1_Y + LINEHIGH);
	SetRect(&(pDeskIdle->rCellId), 0, LINE2_Y, DESK_WIDTH, LINE2_Y +LINEHIGH);
    SetRect(&(pDeskIdle->rIdleTxt), 0, TITLEBAR_HEIGHT + CAPTION_HEIGHT, DESK_WIDTH, LINE1_Y + 48);
}
void GetIdleOprName(char * strOprName)
{
	if(!bRoaming)
		strcpy(strOprName, ProgDeskIdle.cOperatorLName);
}
/*********************************************************************\
* Function     InitIdleKeyEvent
* Purpose      init idle handle key 
* Params      
* Return       
**********************************************************************/
#define  MAX_KEYLISTITEM  8
static PKEYEVENTDATA  pIdleKeyListHead;
static BOOL bKeyEnable;

BOOL InitIdleKeyEvent(void)
{
	int i;
	PKEYEVENTDATA pKeyData = NULL, p = NULL;
    long vkParam[][2]= 
    {
		{VK_F9, PM_LONGKEY|PM_SHORTKEY}, 
		{VK_UP, PM_SHORTKEY|PM_LONGKEY|PM_REPETEDKEY}, 
		{VK_DOWN, PM_SHORTKEY|PM_LONGKEY|PM_REPETEDKEY},
		{VK_LEFT, PM_INSTANTKEY|PM_REPETEDKEY},  
		{VK_RIGHT,PM_INSTANTKEY|PM_REPETEDKEY}, 
		{VK_F6, PM_LONGKEY},
		{VK_F7, PM_INSTANTKEY},
		{VK_F8, PM_INSTANTKEY},

	0
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
* Purpose      Getkeytype:long short ...
* Params      
* Return       
**********************************************************************/
PKEYEVENTDATA  GetKeyTypeByCode(long vkCode)
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
/*********************************************************************\
* Function     Desk_ShowIdle
* Purpose      
* Params      
* Return       
**********************************************************************/
#define  ENTER_UP       0  //press up key to enter quick menu
#define  ENTER_DOWN     1//press down key to enter quick menu

#define  MAX_QUICKMENUITEM  4 //quick menu max item numbers
#define  LISTITEMHIGH  37
#define  TEXTHIGH  20
#define  LISTNUM   4

#define  IDC_GPS_LIST  201
#define  IDC_SOUND_LIST 202

#define  IDC_MENULIST   203
#define  IDM_LOCKKEY    204
#define  IDM_LOCKPHONE  205
#define  IDM_GPS        206
#define  IDM_SOUND      207

#define  PORICON_WIDTH  13
#define  PROICON_HEIGHT 13
#define  MAX_PROFILELEN  40

#define  IDC_MISSEDEVENTS_LIST 600

#define  IDC_LEFT  201
#define  IDC_RIGHT 202

#define MAX_SHORTCUTKEY 4//shortcut key 

#define PM_SETPROFILE     WM_USER + 101
#define PM_QUCIKDIAL      WM_USER +	102
#define PM_SETQUICKDIAL   WM_USER + 103
typedef struct tagProfileDATA
{
    char pProfileName[MAX_PROFILELEN];
	char pIconName[128];
	HANDLE hIcon;
	BOOL  bActive;
	struct tagProfileDATA *pNext;
}ProfileDATA, *PProfileDATA;

extern BOOL    bLockEnable;//lock enable or not
static HBITMAP hIldeBgBmp;//idle backgroud 
static WORD  nPreKeyCode;//handle for key up and down
static PKEYEVENTDATA pIdleCurKeydata;//record idle cur keydata
	   BOOL CreateQuickMenu(struct appMain * pAdm, int nEnterTpye);
static int  OnIdleKeyDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static int  OnIdleKeyUp(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void OnShortCutKey(WORD nKeyCode);

static void CALLBACK f_IdleTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);

static int  OnQuickMenuDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam, HWND *hList);
static BOOL GetTextRect(int num , PRECT prcTxt);
static BOOL OpenMenuList(HWND hWnd,HWND *hList);
	   BOOL CreateKeylockWnd(HWND hWnd);
static BOOL CreatePhonelockWnd(HWND hWnd);
BOOL CreateGPSprofileWnd(HWND hWnd);
static BOOL CreateSoundprofileWnd(HWND hWnd);
static BOOL CreateGpsprofileList(HWND hWnd, HWND* phList);
static BOOL CreateSoundprofileList(HWND hWnd, HWND* phList);
static BOOL CreateQuickMenuList(HWND hWnd, HWND *hList);
static BOOL InitGPSData(void);
static BOOL FreeGpsData(void);
static BOOL InitSoundData(void);
static BOOL FreeSoundData(void);

static LRESULT SoundprofileWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static LRESULT GPSprofileWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static LRESULT PhonelockWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                             LPARAM lParam);
static LRESULT QuickMenuWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static	LRESULT	CALLBACK Prog_KeyBoardProc(int , WPARAM , LPARAM );

static BOOL			bKeyLock, bPhoneLock;//lock flag 
static BOOL         bGPSprofileActivate, bSoundprofileActivate;//gps and sound profile active flag
static UINT			g_nKeyLockTimerId, g_nPhoneLockTimerId;//for keylock and phonelock
static HHOOK		prog_kbHook;//keylock hook 
static PProfileDATA pGpsProfileListHead, pSoundPorfileListHead;//profile data
static HWND			hGpsList, hSoundList;//profile listbox
   
	   HWND			hList;
static HICON		hActIcon_Sound;//active icon handle
//quick menu list item
static int  ListID[]=
{
	IDM_LOCKKEY,
		IDM_LOCKPHONE,
		IDM_GPS,
		IDM_SOUND
};

typedef struct tagMISSEDDATA
{
    int  number;
	BYTE nType; //call, msg, email...
	struct tagMISSEDDATA *pNext;
}MISSEDDATA, *PMISSEDDATA;


static char ShortCutKey[MAX_SHORTCUTKEY][PMNODE_NAMELEN];
static int  ShortCutKeyId[MAX_SHORTCUTKEY];
static HWND hMissedEventsList;
static MISSEDDATA *pMissedEnventsHead = NULL;
static int  nIdleTxtLine = 0, iIdleTxtPos; 
static IDLESHOW idleshow;
static unsigned int  iRepeatKeyTimerId = 0;
static WORD wDIndex = 0;
static HWND hQuickDial;//quick dial window handle
static BOOL bShowIdle = FALSE;

static LRESULT MissedEventsWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
static BOOL CreateMissedEventsList(HWND hWnd, HWND* hList);
static int  OnMissedEventsDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void InitMissedEventsNumEx(int number, BYTE nType);
static void ChangeMissedDataList(BYTE nType, BYTE flag);
static void FreeMissedData(void);
static BOOL PMView_Layout(HWND hWnd, char* pTxt);
static int  PMView_GetLine(void);
static void InitShortCutKey(HWND hWnd);
	   HWND GetMissedEventWnd(void);
extern const TCHAR * GetShortCutAppName(SHORTCUTKEY keyName, int* fID);
extern BOOL APP_CallPhone(const char * pPhoneNumber);
extern BOOL APP_CallPhoneNumber(const char * pPhoneNumber);
extern BOOL GetPhoneLock_Open(void);
extern BOOL SetPhone_Open(BOOL bAuto);

BOOL InitMissedEventsNum();
BOOL CreateMissedEventsWnd(HWND hWnd);
HWND GetPhoneLockWnd();

extern IDLESHOW GetIdleShowInfo(void);
BOOL Desk_ShowIdle( struct appMain * pAdm)
{
    WNDCLASS wc;
    HWND hWnd;
	
	InitShortCutKey(NULL);

	bShowIdle = TRUE;

    wc.style         = 0;
    wc.lpfnWndProc   = IdleWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "IdleWindowClass";

    if (!RegisterClass(&wc))
        return FALSE;
	
	hWnd = CreateWindow("IdleWindowClass",
        "",
        PWS_STATICBAR|WS_CAPTION|WS_VISIBLE,
		0,	
		TITLEBAR_HEIGHT,
		DESK_WIDTH,	
		DESK_HEIGHT- TITLEBAR_HEIGHT,
        
        NULL,NULL,	NULL,	NULL);
	
    if(hWnd == NULL)
    {
        UnregisterClass("IdleWindowClass", NULL);
        return FALSE;
    }
	
	pAdm->hIdleWnd = hWnd;
	
	if(GetPhoneLock_Open())//if phone lock to enter lock window
	{
		CreatePhonelockWnd(hWnd);
		return TRUE;
	}
	return TRUE;
}
/*********************************************************************\
* Function     IdleWndProc
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL bCancelMissed;
extern WORD GetLockKeyCode(void);
extern void SetLockKeyCode(WORD nKeyCode);
BOOL GetEnterInbox(void);
BOOL GetEnterMail(void);
BOOL GetEnterPush(void);
extern BOOL IfPressEndKey(void);
extern void SetPressEndKey(BOOL bEnable);
void  PM_GetCellIdFunc(char *strCellId)
{
	if(!strCellId)
		return;

	if(strlen(strCellId)> 74)
	{
		strncpy(ProgDeskIdle.strCellId, strCellId, 74);
		ProgDeskIdle.strCellId[74] = 0;
	}
	else
		strcpy(ProgDeskIdle.strCellId, strCellId);
}
static LRESULT IdleWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
						   LPARAM lParam)
{
    LRESULT      lResult  = TRUE;	
    char  Screen[PREBROW_MAXFULLNAMELEN + 1];
	HDC          hdc;
	COLORREF Color;
	BOOL	bTran;
	
    switch(wMsgCmd) 
    {
		
	case WM_IDLEOPRCHANGED://changed operator info
		{
			InvalidateRect(hWnd, &ProgDeskIdle.rSPNMNO, TRUE);
			UpdateWindow(hWnd);
		}
		break;
	case CALLBACK_POWEROFF:
		{
			DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_KEYLIGHT), 5);
			DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 10);
			DlmNotify (PMM_NEWS_ENABLE,PMF_KEYLIGHT|PMF_MAIN_BKLIGHT|
				PMF_AUTO_SHUT_KEYLIGHT|PMF_AUTO_SHUT_MAIN_LIGHT);
			WaitWin(pAppMain->hDeskWnd, TRUE, ML("Switching off power..."), ML("Auto switch off"), 0,
				ML("Cancel"), CALLBACK_CANCEL_OFF);
		}
		break;

	case SIMLOCK_FAIL://enter unlock simlock view 
		CreateSimLockCodeWnd(hWnd);
		break;
		
    case WM_CREATE:

		memset(Screen, 0, PREBROW_MAXFULLNAMELEN +1);
		GetBKPicture(Screen);
		if(Screen[0] != 0 && stricmp(Screen, "Default") != 0)
		{
			strcpy(ProgDeskIdle.cBgImage, Screen);
		}

		if(ShortCutKey[0] !=0 )
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_LEFT, 1), 
			(LPARAM)ML(ShortCutKey[0]));
	
		if(ShortCutKey[1] !=0)
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_RIGHT, 0),
			(LPARAM)ML(ShortCutKey[1]));
	
		
		hdc = GetDC(hWnd);
		hIldeBgBmp = CreateBitmapFromImageFile(hdc,ProgDeskIdle.cBgImage,&Color ,&bTran);
		if(hIldeBgBmp == NULL)
		{
			hIldeBgBmp =  CreateBitmapFromImageFile(hdc,"/rom/progman/desk1.bmp",&Color ,&bTran);
			strcpy(ProgDeskIdle.cBgImage, "/rom/progman/desk1.bmp");
		}
		ReleaseDC(hWnd, hdc);
		idleshow = GetIdleShowInfo();
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Home");
		
		break;
  
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
			HWND hCurWnd = NULL;

			if(iRepeatKeyTimerId != 0)//kill repeat key timer when inactive
			{
				KillTimer(NULL, iRepeatKeyTimerId);
				iRepeatKeyTimerId = 0;
			}

			if(nIdleTxtLine > MAX_IDLETXTLEN)//kill idle mode text display timer
			{
				iIdleTxtPos = 0;
				KillTimer(hWnd, TIMER_ID_IDLETXT);
			}
		//	hCurWnd = GetActiveWindow();
			
	//		if(hCurWnd != NULL)//close missed events window
	//		{	
//				if(hCurWnd == pAppMain->hQuickMenuhWnd ||
//					hCurWnd == GetMissedEventWnd())
//					SendMessage(hCurWnd, WM_CLOSE, 0, 0);
	//		}
			bCancelMissed =FALSE;
        }
        else
        {      
			int nAutoLockTime= 0;
			char pAutoLockTime[10];
			calliftype callstate = GetCallIf();

			SetFocus(hWnd);
			
			InitShortCutKey(hWnd);

			if(ShortCutKey[0] !=0 )
				SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_LEFT, 1), 
				(LPARAM)ML(ShortCutKey[0]));
			
			if(ShortCutKey[1] !=0)
				SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDC_RIGHT, 0),
				(LPARAM)ML(ShortCutKey[1]));
			
			
			if(callstate == diallinkif || callstate == ringif || callstate == callingif)
				;//if in call status not auto lock
			else
			{
				if(GetKeyLockMode() != KL_MANUAL)//if autokeylock
				{
					if(g_nKeyLockTimerId)
					{
						KillTimer(NULL, g_nKeyLockTimerId);
						g_nKeyLockTimerId = 0;
					}
					g_nKeyLockTimerId = SetTimer(NULL, 0, IDLE_TIMEOUT, f_KeylockTimerProc);
				}
				else
				{
					if(g_nKeyLockTimerId)
					{
						KillTimer(NULL, g_nKeyLockTimerId);
						g_nKeyLockTimerId = 0;
					}

				}
			}

			if(nIdleTxtLine > MAX_IDLETXTLEN)
			{
				SetTimer(hWnd, TIMER_ID_IDLETXT, IDLETXT_TIMEOUT, NULL);
			}

            if(!ProgDeskIdle.bHideText)
            {
                InvalidateRect(hWnd, NULL, FALSE);
                SetTimer(hWnd, TIMER_ID_FRESH, 5000, NULL);//frash time
            }
			//set auto phonelock timer
			if(!PM_GetPhoneLockStatus())
			{
				pAutoLockTime[0] = 0;
				GetLockCodePeriod((char*)pAutoLockTime);
				if(pAutoLockTime[0] != 0)
				{
					if(strcmp(pAutoLockTime, "Off") ==0 ||
						strcmp(pAutoLockTime,"0") == 0)
						nAutoLockTime = 0;
					else
					{
						pAutoLockTime[strlen(pAutoLockTime) -4] = 0;
						nAutoLockTime = PM_atoi(pAutoLockTime);
					}
					
				}
				if(nAutoLockTime != 0)
				{
					if(g_nPhoneLockTimerId)
					{
						KillTimer(NULL, g_nPhoneLockTimerId);
						g_nPhoneLockTimerId = 0;
					}
					g_nPhoneLockTimerId = SetTimer(NULL, NULL,nAutoLockTime*60*1000, f_PhonelockTimerProc);
				}
				else
				{
					if(g_nPhoneLockTimerId)
					{
						KillTimer(NULL, g_nPhoneLockTimerId);
						g_nPhoneLockTimerId = 0;
					}
				}
			}

 			if(InitMissedEventsNum() && !bCancelMissed && !IfPressEndKey())//if have missed events to enter missed events window
				CreateMissedEventsWnd(hWnd);
			if(bCancelMissed)
				bCancelMissed =FALSE;
			if(IfPressEndKey())
				SetPressEndKey(FALSE);
        }
        break;
    case WM_PAINT:
       	{
			BLENDFUNCTION blendFunction;
			int           nMode;
			HDC           hdcMem, hdcTans;
			PAINTSTRUCT ps;
			RECT        rect;
			BOOL        ret, bCellIdUp = FALSE;
			HFONT       hFont, hOldFont;
			int         nLine = 0;
			int         nClientWidth = 0, len = 0, nFit = 0;
			
			hdcTans = BeginPaint(hWnd, &ps);

					
			idleshow = GetIdleShowInfo();
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Home");
			
			GetFontHandle(&hFont, SMALL_FONT);
			
			BitBlt(hdcTans, 0, 0, DESK_WIDTH, DESK_HEIGHT-STATUSBAR_HEIGHT, (HDC)hIldeBgBmp, 0, 0, ROP_SRC);

			nMode = SetBkMode(hdcTans, NEWTRANSPARENT );
			SetBkColor(hdcTans,0x000000);
			DrawNotifyIconEx(hdcTans);

			hdcMem = CreateMemoryDC(DESK_WIDTH, LINE1_Y + LINEHIGH);
			SetBkColor(hdcTans,0x000000);
			
			//rect for operator & service provider
			SetRect(&rect, 0, LINE1_Y, DESK_WIDTH, LINE1_Y + LINEHIGH );
			ClearRect( hdcMem, &rect, 0xffffff);
			SetBkMode(hdcTans, BM_NEWALPHATRANSPARENT);
			
			blendFunction.AlphaFormat	= 0;
			blendFunction.BlendFlags	= 0;
			blendFunction.BlendOp		= AC_SRC_OVER;
			blendFunction.SourceConstantAlpha = 160;
			
			ret = AlphaBlend(hdcTans, 0,LINE1_Y , DESK_WIDTH , LINE1_Y + LINEHIGH, 
				hdcMem, 0, LINE1_Y , DESK_WIDTH , LINE1_Y + LINEHIGH, blendFunction );
			
			DeleteDC(hdcMem);
			
			//rect for cell id & data
			idleshow = GetIdleShowInfo();
#if 0
			idleshow = IS_CELLID;
			strcpy(ProgDeskIdle.strCellId, "shfihjfjsljlfjlsjflkjfsjfsjfjsjfjfjfsfjsjf");

#endif
			if((idleshow == IS_CELLID && ProgDeskIdle.strCellId[0] != 0) || idleshow == IS_DATE)
			{
				hdcMem = CreateMemoryDC(DESK_WIDTH, LINE2_Y+LINEHIGH);
				SetBkColor(hdcTans,0x000000);

				if(idleshow == IS_CELLID)
				{
					nClientWidth = DESK_WIDTH;
					len = strlen(ProgDeskIdle.strCellId);
					GetTextExtentExPoint(hdcTans, ProgDeskIdle.strCellId, len, nClientWidth, &nFit, NULL, NULL);
					if(nFit < len)
					{
						SetRect(&rect, 0, LINE2_Y-LINEHIGH, DESK_WIDTH, LINE2_Y+LINEHIGH);
						ClearRect(hdcMem, &rect, 0xffffff);
						
						ret = AlphaBlend(hdcTans, 0, LINE2_Y- LINEHIGH, DESK_WIDTH, LINE2_Y+LINEHIGH,
							hdcMem, 0, LINE2_Y- LINEHIGH, DESK_WIDTH, LINE2_Y+LINEHIGH, blendFunction);
						
						SetRect(&(ProgDeskIdle.rCellId), 0, LINE2_Y-LINEHIGH, DESK_WIDTH, LINE2_Y +LINEHIGH
							);
						bCellIdUp = TRUE;
					}
					else
					{
						SetRect(&rect, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH);
						ClearRect(hdcMem, &rect, 0xffffff);
						
						ret = AlphaBlend(hdcTans, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH,
							hdcMem, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH, blendFunction);

					}
				}
				else
				{
					SetRect(&rect, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH);
					ClearRect(hdcMem, &rect, 0xffffff);
					
					ret = AlphaBlend(hdcTans, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH,
						hdcMem, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH, blendFunction);
				}
				
				DeleteDC(hdcMem);
				
			}
			
#if 0
	
    strcpy(ProgDeskIdle.cIdleModeTxt , "1234567890 \r\nI am a good person,if there is no bad wether ,did you?\r1\n2\r\r3\n\n4\r\n5\n\r6abcdefghijklmnopqrstuvwsyz12345678900987654321 error\r\n");
	
#endif
			//rect for idle mode text
			PMView_Layout(hWnd, ProgDeskIdle.cIdleModeTxt);
			nIdleTxtLine = PMView_GetLine();
			
			if(nIdleTxtLine > MAX_IDLETXTLEN)
			{
				SetTimer(hWnd, TIMER_ID_IDLETXT, IDLETXT_TIMEOUT, NULL);
				nLine = MAX_IDLETXTLEN;
			}
			else
				nLine = nIdleTxtLine;
			
			if(nLine == MAX_IDLETXTLEN && bCellIdUp)
				nLine--;
			
			SetRect(&ProgDeskIdle.rIdleTxt, 0,  LINETXT_Y, PLX_WIN_WIDTH,
				LINETXT_Y + nLine * LINETXT_HIGHT);
		
			hdcMem = CreateMemoryDC(DESK_WIDTH, LINETXT_Y + nLine * LINETXT_HIGHT);
			SetBkColor(hdcTans,0x000000);
			
			SetRect(&rect, 0, LINETXT_Y, DESK_WIDTH, LINETXT_Y + nLine * LINETXT_HIGHT );
			ClearRect( hdcMem, &rect, 0xffffff);

			ret = AlphaBlend(hdcTans, 0,LINETXT_Y, DESK_WIDTH , LINETXT_Y + nLine * LINETXT_HIGHT, 
				hdcMem, 0, LINETXT_Y , DESK_WIDTH , LINETXT_Y + nLine * LINETXT_HIGHT, blendFunction );
			
			DeleteDC(hdcMem);

			SetBkMode(hdcTans, nMode);
			
			GetClientRect(hWnd, &rect);
			
			hdcMem = CreateMemoryDC(rect.right, rect.bottom );
			
			BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdcTans, 0,0,SRCCOPY);
			
			hOldFont = SelectObject ( hdcMem, hFont );
			
			DrawIdleText(hWnd,hdcMem, bCellIdUp);
			
			SelectObject(hdcMem, hOldFont);
			
			BitBlt(hdcTans, 0, 0, rect.right, rect.bottom,
				hdcMem, 0, 0, SRCCOPY);
			
			DeleteDC(hdcMem);
			hdcMem = NULL;
	
			EndPaint(hWnd, NULL);
		}
        break;

	case PM_QUCIKDIAL://shortcut enter quick dial
		{
			if(lParam == 1)
				hQuickDial = APP_SetQuickDial(hWnd, PM_SETQUICKDIAL, wDIndex);
		}
		break;
		
	case PM_SETQUICKDIAL:
		{
			ABNAMEOREMAIL *ABName;

			if(wParam == FALSE)
				break;
			else
			{
				ABName = (ABNAMEOREMAIL*)lParam;
			}
            hQuickDial = NULL;
		//	APP_CallPhoneNumber(ABName->szTelOrEmail);	
		}
		break;
		
    case WM_KEYDOWN://handle idle key
		{
			WORD mId;
			int  iIndex= 0;

			mId = LOWORD(wParam);
			
			if(LOWORD(wParam) == VK_F5)
			{
				if(pAppMain && pAppMain->hGrpWnd)
				{
					ShowWindow(pAppMain->hIdleWnd, SW_HIDE);

					if(IsWindowVisible(pAppMain->hGrpWnd))
						BringWindowToTop(pAppMain->hGrpWnd);	
					else
						ShowWindow(pAppMain->hGrpWnd, SW_SHOW);
				}
			
			}
			else if(LOWORD(wParam) == VK_F10)
			{
				OnShortCutKey(VK_F10);
			}
			else if(LOWORD(wParam) == VK_RETURN)
			{
				OnShortCutKey(VK_RETURN);
			}
			else if(LOWORD(wParam) == VK_0)
			{
				if(lParam != 1)
					break;

				CallAppEntryEx("WAP", 0, 0);
			}
			else if(LOWORD(wParam) == VK_1)
			{
				ABNAMEOREMAIL ABName;

				wDIndex = LOWORD(wParam) - 0x30;
				
				if(lParam != 1)
					break;

				memset(&ABName, 0, sizeof(ABNAMEOREMAIL));

				if(APP_GetQuickDial(wDIndex, &ABName) == FALSE)//quick dial
				{
					PLXTipsWin(NULL, hWnd, PM_QUCIKDIAL, 
						ML("Voice mailbox\r\nnr not set.\r\nSet now?"), NULL, Notify_Request,
						ML("Yes"), ML("No"), 0);
				}
				else
				{
					APP_CallPhoneNumber(ABName.szTelOrEmail);
				}
				break;//call voice mail
			}
			else if((LOWORD(wParam) >= VK_2)&&(LOWORD(wParam) <= VK_9))
			{
				ABNAMEOREMAIL ABName;
				
				wDIndex = LOWORD(wParam) - 0x30;
				
				if(lParam != 1)
					break;

				memset(&ABName, 0, sizeof(ABNAMEOREMAIL));
				
				if(GetSIMState() == 0)
				{
					PLXTipsWin(NULL, hWnd, NULL, ML("Insert SIM card"), NULL, Notify_Alert,ML("Ok"),
					NULL, WAITTIMEOUT);
					break;
				}

				if(APP_GetQuickDial(wDIndex, &ABName) == FALSE)//quick dial
				{	
					PLXTipsWin(NULL, hWnd, PM_QUCIKDIAL, 
						ML("Quick dialling nr not set. Set now?"), NULL, Notify_Request,
						ML("Yes"), ML("No"), 0);
				}
				else
				{
					APP_CallPhoneNumber(ABName.szTelOrEmail);
				}
			}
			else
				OnIdleKeyDown(hWnd, wMsgCmd, wParam, lParam);
			
		}
	    break;
	case WM_KEYUP:
		OnIdleKeyUp(hWnd,wMsgCmd, wParam, lParam);
		break;
		
	case WM_COMMAND:
		break;

    case WM_TIMER:
	
		if(wParam == TIMER_ID_FRESH)
        {
            //update time
            if(ProgDeskIdle.bHideText)
                break;

            if(IsTimeChanged())
            {
                //RECT rect;
				SYSTEMTIME	SystemTime;
				char		aTimeBuf[25], aDateBuf[25];
				
				memset(&SystemTime, 0, sizeof(SYSTEMTIME));
				memset(aTimeBuf, 0, 25);
				memset(aDateBuf, 0, 25);
				
				GetLocalTime(&SystemTime);
				
				GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
						
				if(!hPhonelockWnd)
					SetWindowText(hWnd, aTimeBuf);
							
                InvalidateRect(hWnd, &ProgDeskIdle.rCellId, TRUE);
	         }
			if(GetLockKeyCode() == VK_WAKEUP_KEY ||
				GetLockKeyCode() == VK_WAKEUP_POWERKEY
				||GetLockKeyCode() == VK_WAKEUP_EMERGENCYKEY
				||GetLockKeyCode() == VK_WAKEUP_EXTERNAL)
			{
				SetLockKeyCode(VK_RINGIN);
				f_sleep_unregister(KEYEVENTS);
				DlmNotify(PMM_NEWS_ENABLE, 	PMF_KEYLIGHT|PMF_MAIN_BKLIGHT);
				DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 15);
			}		
        }
		else if(wParam == TIMER_ID_IDLETXT)
		{
			iIdleTxtPos++;
			KillTimer(hWnd, TIMER_ID_IDLETXT);
			SetTimer(hWnd, TIMER_ID_IDLETXT, IDLETXT_TIMEOUT, NULL);
			InvalidateRect(hWnd, &ProgDeskIdle.rIdleTxt, TRUE);
		}
        break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

    case WM_DESTROY:
		{
			if(hIldeBgBmp)
			{
				DeleteObject(hIldeBgBmp);
				hIldeBgBmp = NULL;
			}
			UnregisterClass("IdleWindowClass", NULL);
			pAppMain->hIdleWnd = NULL;
		}
    	break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}
/*********************************************************************\
* Function     CallIdleBkPicChange
* Purpose      
* Params      
* Return       
**********************************************************************/
extern PAPPADM	 GetAppMain(void);
void CallIdleBkPicChange(void)
{

	char  Screen[PREBROW_MAXFULLNAMELEN + 1];
	HDC hdc;
	PAPPADM pappMain = NULL;
	COLORREF Color;
	BOOL	bTran;

	pappMain = GetAppMain();

	hdc = GetDC(pappMain->hIdleWnd);
	
	memset(Screen, 0, PREBROW_MAXFULLNAMELEN +1);
	GetBKPicture(Screen);

	if(hIldeBgBmp)
	{
		DeleteObject(hIldeBgBmp);
		hIldeBgBmp = NULL;
	}


	if(Screen[0] != 0 && stricmp(Screen, "Default") != 0)
	{
		//if(stricmp(ProgDeskIdle.cBgImage,Screen) != NULL)
		{
			strcpy(ProgDeskIdle.cBgImage, Screen);
			hIldeBgBmp = CreateBitmapFromImageFile(hdc,ProgDeskIdle.cBgImage,
				&Color ,&bTran);
			if(!hIldeBgBmp)
			{
				hIldeBgBmp =  CreateBitmapFromImageFile(hdc,"/rom/progman/desk1.bmp",&Color ,&bTran);
				strcpy(ProgDeskIdle.cBgImage, "/rom/progman/desk1.bmp");
			}
		}
	}
	else
	{
		hIldeBgBmp =  CreateBitmapFromImageFile(hdc,"/rom/progman/desk1.bmp",&Color ,&bTran);
		strcpy(ProgDeskIdle.cBgImage, "/rom/progman/desk1.bmp");
	}

	ReleaseDC(pappMain->hIdleWnd, hdc);

}

/*********************************************************************\
* Function     OnIdleKeyDown
* Purpose      
* Params      
* Return       
**********************************************************************/
static int  OnIdleKeyDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	PKEYEVENTDATA pKeyData = NULL;

	
	nPreKeyCode = LOWORD(wParam);

    pKeyData = GetKeyTypeByCode(LOWORD(wParam));

	if(pKeyData == NULL)
	{
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		return 0;
	}

	pIdleCurKeydata = pKeyData;

	if(pKeyData->nType & PM_LONGKEY)
	{
		pKeyData->nTimerId = SetTimer(NULL, 0,  ET_LONG, f_IdleTimerProc);
		iRepeatKeyTimerId = pKeyData->nTimerId;
	}

	if(pKeyData->nType& PM_INSTANTKEY)
	{
		bKeyEnable = TRUE;
	}
	
	if(pKeyData->nType & PM_SHORTKEY)
	{
		pKeyData->dKicktime = GetTickCount();
	}
	
    if(bKeyEnable && (LOWORD(wParam) == VK_LEFT || LOWORD(wParam) == VK_RIGHT))
	{
		OnShortCutKey(LOWORD(wParam));
	}
	
	return 0;
}
/*********************************************************************\
* Function     OnShortCutKey
* Purpose      
* Params      
* Return       
**********************************************************************/
static void OnShortCutKey(WORD nKeyCode)
{
	char strKeyText[PMNODE_NAMELEN];
	int  keyId = 0;
	
	strKeyText[0] = 0;

	switch(nKeyCode) 
	{
	case VK_LEFT:
		strcpy(strKeyText, ShortCutKey[2]);
		keyId = ShortCutKeyId[2];
		break;

	case VK_RIGHT:
		strcpy(strKeyText, ShortCutKey[3]);
		keyId = ShortCutKeyId[3];
		break;

	case VK_RETURN:
		strcpy(strKeyText, ShortCutKey[0]);
		keyId = ShortCutKeyId[0];
		break;

	case VK_F10:
		strcpy(strKeyText, ShortCutKey[1]);
		keyId = ShortCutKeyId[1];
		break;
	
	}
   CallAppEntryEx(strKeyText, 0, keyId);

}
/*********************************************************************\
* Function     OnIdleKeyUp
* Purpose      
* Params      
* Return       
**********************************************************************/
static int  OnIdleKeyUp(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	PKEYEVENTDATA pKeyData = NULL;
	long  vkType;
	DWORD	dwTickNow = 0;
    int     nInterval = 0;


	if(nPreKeyCode == 0)//only get keyup
	{
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		return 0;
	}
	if(nPreKeyCode != LOWORD(wParam))//keyup and keydown not match
	{
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		return 0;
	}
	else
		nPreKeyCode = 0;

    pKeyData = GetKeyTypeByCode(LOWORD(wParam));

	if(pKeyData == NULL)
	{
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		return 0;
	}

	if(pKeyData->nType & PM_LONGKEY)
	{
		if(pKeyData->nTimerId !=0)
		{
			KillTimer(hWnd, pKeyData->nTimerId);
			pKeyData->nTimerId = 0;
		}
	}

	if(pKeyData->nType & PM_SHORTKEY)
	{

		dwTickNow = GetTickCount();
	
		nInterval = dwTickNow - pKeyData->dKicktime;
		if (nInterval < 0)
			nInterval = -nInterval;
	
		if (nInterval <= ET_LONG)
		{
			vkType = PM_SHORTKEY;
		}
	}


	switch(wParam)
	{
	case VK_DOWN:
		if(vkType == PM_SHORTKEY)
			CreateQuickMenu(pAppMain, ENTER_UP);
		break;
		
	case VK_UP:
		if(vkType == PM_SHORTKEY)
			CreateQuickMenu(pAppMain, ENTER_DOWN);
		break;
		
	}

	return 0;
}
/*********************************************************************\
* Function     f_IdleTimerProc
* Purpose      
* Params      
* Return       
**********************************************************************/
static void CALLBACK f_IdleTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
	if(pIdleCurKeydata->nTimerId != 0)
	{
		KillTimer(hWnd, idEvent);
		if(pIdleCurKeydata->nType & PM_LONGKEY)
		{
			switch(pIdleCurKeydata->nkeyCode)
			{
			case VK_DOWN:
				CreateKeylockWnd(pAppMain->hIdleWnd);
				break;
				
			case VK_UP:
				CreateSoundprofileWnd(pAppMain->hIdleWnd);
				break;
			}
		}
	}
   idEvent = 0;
}
/*********************************************************************\
* Function     DrawIdleText
* Purpose      
* Params      
* Return       
**********************************************************************/
extern BOOL GetTimeDisplay(SYSTEMTIME st, char* cTime, char* cDate);
static void PMDrawIdleModeTxt(HWND hWnd, HDC hdc, BOOL bCellIdUp);
static void PMDrawIdleCellId(HWND hWnd, HDC hdc);

static void DrawIdleText(HWND hWnd, HDC hdc, BOOL bCellIdUp)
{
    int bkoldMode;
    SYSTEMTIME	SystemTime;
    char		aTimeBuf[25], aDateBuf[25];
	char  cOperatorInfo[30];

    if(ProgDeskIdle.bHideText)
        return;

	switch(pm_sim_cop_status)
    {
    
    case pm_cops_searching:
        strcpy(ProgDeskIdle.cPlmnStr, ML("Searching for networks"));
        break;
	case pm_cops_roaming:
		strcpy(ProgDeskIdle.cPlmnStr, ML("Roaming"));
		break;
	case pm_cops_rejectsim:
		strcpy(ProgDeskIdle.cPlmnStr, ML("Change SIM card"));
		break;
	case pm_cops_emergencycallonly:
		strcpy(ProgDeskIdle.cPlmnStr, ML("Emergency calls only"));
		break;
	case pm_cops_noserivce:
		strcpy(ProgDeskIdle.cPlmnStr, ML("No service"));
		break;
    case pm_cops_link:
		break;
	case pm_cops_gsmoff:
		strcpy(ProgDeskIdle.cPlmnStr, ML("GSM radio part off"));
		break;
    default:
        break;
	}

    bkoldMode = SetBkMode(hdc, BM_TRANSPARENT);

	memset(&SystemTime, 0, sizeof(SYSTEMTIME));
	memset(cOperatorInfo, 0, 30);
	memset(aTimeBuf, 0, 25);
	memset(aDateBuf, 0, 25);
	
    GetLocalTime(&SystemTime);

	GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);

#if 1	
	if(ProgDeskIdle.cIdleModeTxt[0] != 0)
	{
		PMDrawIdleModeTxt(hWnd, hdc, bCellIdUp);
	}
	
#endif


	if(pm_sim_cop_status == pm_cops_link)
	{
		if(ProgDeskIdle.cOperatorLName[0] != 0)
		{
			if(strlen(ProgDeskIdle.cOperatorLName) <= 30)
				strcpy(cOperatorInfo, ProgDeskIdle.cOperatorLName);
			else if(ProgDeskIdle.cOpenratorSName[0] !=0)
				strcpy(cOperatorInfo, ProgDeskIdle.cOpenratorSName);
		}
		else
		{
			if(ProgDeskIdle.cCountryCode[0] != 0 && ProgDeskIdle.cOperatorCode[0] !=0)
			{
				sprintf(cOperatorInfo, "%s/%s", ProgDeskIdle.cCountryCode, ProgDeskIdle.cOperatorCode);
			}
		}
	}
	else
		strcpy(cOperatorInfo, ProgDeskIdle.cPlmnStr);
	 
	 if(ProgDeskIdle.cSPN[0] != 0 && cOperatorInfo[0] !=0)
	 {
		 DrawText(hdc, ML(ProgDeskIdle.cSPN), -1, &ProgDeskIdle.rSPN, DT_CENTER | DT_VCENTER);
		 DrawText(hdc, ML(cOperatorInfo), -1, &ProgDeskIdle.rMNO, DT_CENTER | DT_VCENTER);
	 }
	 else if(ProgDeskIdle.cSPN[0] != 0)
	 {
		 DrawText(hdc, ML(ProgDeskIdle.cSPN), -1, &ProgDeskIdle.rSPNMNO, DT_CENTER | DT_VCENTER);
	 }
	 else if(cOperatorInfo[0] != 0)
	 {
		 DrawText(hdc, ML(cOperatorInfo), -1, &ProgDeskIdle.rSPNMNO, DT_CENTER | DT_VCENTER);
	 }

	 if(ProgDeskIdle.strCellId[0] != 0&& idleshow == IS_CELLID)
	 {
		 if(bCellIdUp)
		 {
			 PMDrawIdleCellId(hWnd, hdc);
		 }
		 else
			 DrawText(hdc, ProgDeskIdle.strCellId, -1, &ProgDeskIdle.rCellId, DT_CENTER);
	 }
	 else if(aDateBuf[0] != 0 && idleshow == IS_DATE)
		 DrawText(hdc, aDateBuf, -1, &ProgDeskIdle.rCellId, DT_CENTER|DT_VCENTER);

	 if(!hPhonelockWnd)
		 SetWindowText(hWnd, aTimeBuf);
	 
	 SetBkMode(hdc, bkoldMode);
	 
	 return;
}
/*********************************************************************\
* Function     IsTimeChanged
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL IsTimeChanged(void)
{
    static SYSTEMTIME  oldTime;
	SYSTEMTIME	SystemTime;

	GetLocalTime(&SystemTime);

    if (SystemTime.wYear == oldTime.wYear && 
        SystemTime.wMonth == oldTime.wMonth &&
        SystemTime.wDay == oldTime.wDay &&
        SystemTime.wHour == oldTime.wHour &&
        SystemTime.wMinute == oldTime.wMinute)
    {
        return FALSE;
    }
    else
    {
        oldTime = SystemTime;     
    }

    return TRUE;
}
/*********************************************************************\
* Function     InitShortCutKey
* Purpose      
* Params      
* Return       
**********************************************************************/
static void InitShortCutKey(HWND hWnd)
{
	int i;
	for(i = 0; i < MAX_SHORTCUTKEY; i++)
	{
		memset(ShortCutKey[i], 0, PMNODE_NAMELEN);
		ShortCutKeyId[i] =0;
	}


	strcpy(ShortCutKey[0],GetShortCutAppName(SCK_LSK, &ShortCutKeyId[0]));//left softkey
	strcpy(ShortCutKey[1],GetShortCutAppName(SCK_RSK,&ShortCutKeyId[1]));//right softkey
	strcpy(ShortCutKey[2],GetShortCutAppName(SCK_LAK,&ShortCutKeyId[2]));//left arrowkey
	strcpy(ShortCutKey[3],GetShortCutAppName(SCK_RAK,&ShortCutKeyId[3]));//right arrowkey

	if(hWnd != NULL)
	{
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML(ShortCutKey[0]));
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML(ShortCutKey[1]));
	}
}
/*********************************************************************\
* Function     CreateAppLaunch
* Purpose      app swapper view
* Params      
* Return       
**********************************************************************/
#define  ITEMHIG  59
#define  ITEMWIN  57
#define  ICONHIG  28
#define  ICONWIN  43
#define  TEXTHIN  16
#define  TEXTWIN  53

#define  ET_SLIDE 100
#define  IDC_SLIDE 101
#define  CY_SLIDESTART 80
#define  DCY_SLIDEONCE (-9)

static  int nFocus = 1, nCurAppNum, iPos = 0, iOldpos;
static PAPPNODE pCurAppNode;
static char strCap[PMNODE_NAMELEN +1];
static HBITMAP hBmpLeftArrow, hBmpRightArrow;
static int nFileNum;
static HBITMAP hGrpBgBmp;
static int v_nRepeats;
static PKEYEVENTDATA pCurKeydata;
static HBRUSH hLaunchBgBrush;
DWORD	dwTickNow = 0;
int     nInterval = 0;
static int cySlide, y, cy,yFinal; //move window
static void LaunchGetRect(RECT *rcRect, RECT *rcText, int iCurr, int iStyle);
static void PM_MakeHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam);
static int  OnLaunchKeyDown(HWND hWnd, int KeyCode, WORD vkType);
static void LaunchGetGifRect(RECT *rcRect, int iCurr, int iStyle);
extern BOOL	GetDisplayName(char* pDisplayName, char *str, unsigned int len);
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);
static void CALLBACK f_RpTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);

static BOOL AppLaunchList(HWND hWnd, HDC hdc, struct appMain *pAdm);
static BOOL AppSwapperCap(HWND hWnd, HDC hdc, struct appMain *pAdm);
static int  GetBackgroudAppNum(struct appMain *pAdm);
BOOL CreateAppLaunch(struct appMain * pAdm, HWND hParentWnd)
{
	WNDCLASS wc;
    HWND hWnd;
	
	GetBackgroudAppNum(pAdm);
	iPos = 0;

    wc.style         = 0;
    wc.lpfnWndProc   = AppLaunchWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "applaunchWindowClass";

    if (!RegisterClass(&wc))
	{
		UnregisterClass("applaunchWindowClass", NULL);
		if(!RegisterClass(&wc))
			return FALSE;
	}

	y = 197 - CY_SLIDESTART;
	cy = CY_SLIDESTART +24;
	cySlide = CY_SLIDESTART;
	yFinal = CAPTION_HEIGHT + TITLEBAR_HEIGHT;
	
    hWnd = CreateWindow("applaunchWindowClass",
        "",
        WS_VISIBLE| PWS_STATICBAR,
		0,	
		y,
		DESK_WIDTH,	
		cy, 
        hParentWnd,NULL,	NULL,	NULL);

    if(hWnd == NULL)
    {
        UnregisterClass("applaunchWindowClass", NULL);
        return FALSE;
    }

	SetTimer(hWnd, IDC_SLIDE, ET_SLIDE, NULL);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    pAdm->hLaunchWnd = hWnd;

    return TRUE;

}
/*********************************************************************\
* Function     AppLaunchWndProc
* Purpose      
* Params      
* Return       
**********************************************************************/
void AppLaunchKillFocus();
extern void PMShowIdle(void);
static LRESULT AppLaunchWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT      lResult  = TRUE;	
    HDC hdc;
    RECT rect;

    switch(wMsgCmd) 
    {
    case WM_CREATE:
	
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
			(LPARAM)ML(IDS_CANCEL));

		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		
		hLaunchBgBrush = CreateSolidBrush(RGB(158,177,230));
        
		hBmpLeftArrow = LoadImage(NULL, "/rom/progman/app/leftarrow.bmp", IMAGE_BITMAP,
			13,11, LR_LOADFROMFILE);
		hBmpRightArrow = LoadImage(NULL, "/rom/progman/app/rightarrow.bmp", IMAGE_BITMAP,
			13, 11, LR_LOADFROMFILE);
	
    	break;

	case WM_TIMER://slide into view
		{
			if(wParam == IDC_SLIDE)
			{
				cySlide += DCY_SLIDEONCE;

				if (cySlide < -DCY_SLIDEONCE)
				{
					cySlide = -DCY_SLIDEONCE;
				}
				y -= cySlide;
				cy += cySlide;
				if (y <= yFinal)
					
				{
					y  = yFinal;
					cy = DESK_HEIGHT- TITLEBAR_HEIGHT -CAPTION_HEIGHT;
					KillTimer(hWnd, wParam);
					cySlide = CY_SLIDESTART;
				}
				MoveWindow(hWnd, 0, y, DESK_WIDTH, cy, FALSE);
			}
		}
		break;
  
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
			AppLaunchKillFocus();
         }
        else
        {
            SetFocus(hWnd);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
				(LPARAM)ML(IDS_CANCEL));
         }
        break;
	case WM_KILLFOCUS:
		{
			AppLaunchKillFocus();			
		}
		break;
	case WM_QUERYSYSEXIT:
        return SYSEXIT_CONTINUE;
        
    case WM_SYSEXIT:
        SendMessage(hWnd, WM_CLOSE, 0, 0);
        break;

	case WM_ERASEBKGND:
		break;

    case WM_PAINT:
        {
            HDC hScreendc=NULL, hMemdc = NULL;
            PAINTSTRUCT ps;
			HFONT       hFont= NULL, hOldFont = NULL;
			HBRUSH    hOldBrush = NULL;
			RECT     rcBg;
						
			strCap[0] = 0;
			
            hdc = BeginPaint(hWnd, &ps);
			
			hOldBrush = SelectObject(hdc, hLaunchBgBrush);
			SetRect(&rcBg, 0,0, 176, 30);
			FillRect(hdc, &rcBg, hLaunchBgBrush);

            GetClientRect(hWnd, &rect);
			rect.top += 30;
			ClearRect(hdc, &rect, COLOR_WHITE);
			rect.top -= 30;
			
			GetFontHandle(&hFont,SMALL_FONT);

			hMemdc = CreateMemoryDC(rect.right, rect.bottom);
			BitBlt(hMemdc, 0, 0, rect.right, rect.bottom, hdc, 0,0,SRCCOPY);
			
			hOldFont = SelectObject ( hMemdc, hFont );
			
			AppLaunchList(hWnd, hMemdc, pAppMain);

            BitBlt(hdc, 0, 0, rect.right, rect.bottom,
                hMemdc, 0, 0, SRCCOPY);
			
			DeleteDC(hMemdc);
			hMemdc = NULL;
					
			EndPaint(hWnd, NULL);
        }
        break;
	case WM_CLOSE:

		DestroyWindow(hWnd);
		break;

    case WM_KEYDOWN:

		switch(LOWORD(wParam)) 
		{
		case VK_F10 :
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
		case VK_F5:
			{
				if((strcmp(pCurAppNode->aDisplayName,"Call") == NULL)
					&&(!IsCallAPP()))
				{
					PMShowIdle();
				}
				else
					AppFileOpen(pAppMain,pCurAppNode->appId, TRUE, 0);
				PostMessage(hWnd,WM_CLOSE, NULL, NULL);
			}
			
			break;
		case VK_LEFT:
		case VK_RIGHT:
		case VK_F9:
			PM_MakeHotKey(hWnd, wParam, lParam);
			break;
		default:
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		
        break;
	case WM_KEYUP:
		{
			AppLaunchKillFocus();
			if (pCurKeydata && pCurKeydata->nType & PM_SHORTKEY)
			{
				dwTickNow = GetTickCount();
				PLXPrintf("\r\n dwTickNow = %x", dwTickNow);
				
				nInterval = dwTickNow - pCurKeydata->dKicktime;
				if (nInterval < 0)
					nInterval = -nInterval;
				PLXPrintf("\r\n interval = %d", nInterval);
				
				if (nInterval <= ET_LONG)
				{
					OnLaunchKeyDown(hWnd, LOWORD(wParam), PM_SHORTKEY);
				}
				
			}
		}
		break;

    case WM_DESTROY:
      
		if(hLaunchBgBrush)
		{
			DeleteObject(hLaunchBgBrush);
			hLaunchBgBrush = NULL;
		}
		if(hBmpLeftArrow)
		{
			DeleteObject(hBmpLeftArrow);
			hBmpLeftArrow = NULL;
		}
		if(hBmpRightArrow)
		{
			DeleteObject(hBmpRightArrow);
			hBmpRightArrow =NULL;
		}
        UnregisterClass("applaunchWindowClass", NULL);
	    pAppMain->hLaunchWnd = NULL;
    	break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}
void AppLaunchKillFocus()
{
	if(pCurKeydata == NULL)
		return;
	
	if(pCurKeydata->nType & PM_LONGKEY)
	{
		if(pCurKeydata->nTimerId !=0)
		{
			KillTimer(NULL, pCurKeydata->nTimerId);
			pCurKeydata->nTimerId = 0;
		}
	}
	
	if(pCurKeydata->nType & PM_REPETEDKEY)
	{
		if(pCurKeydata->nTimerId !=0)
		{
			KillTimer(NULL, pCurKeydata->nTimerId);
			pCurKeydata->nTimerId = 0;
		}
		v_nRepeats = 0;
	}	
}
/*********************************************************************\
* Function     AppLaunchList
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL AppLaunchList(HWND hWnd, HDC hdc, struct appMain *pAdm)
{
    PCURAPPLIST *pHead, pFile = NULL ;
    PAPPNODE  pAppNode = NULL;
	int        nCur =0, nListhighth, i= 0, nDawnitem = 0,nOldmode;
	char      strDirName[PMNODE_NAMELEN + 1];
	RECT      rcIcon, rcText, rcFocus;
	COLORREF  oldTxtColor;
	short     fileID = 0;
	BOOL      bDlmEnable = FALSE;
	
	nOldmode = GetBkMode(hdc);
	oldTxtColor = SetTextColor(hdc, RGB(0,0,0));

	fileID = AppGetIdFromIconName(pAppMain, "/rom/progman/app/mcall_43x28.bmp");
	pAppNode = pAppMain->pFileCache + fileID;
	
	pHead = AppFileGetListHead(pAdm);
    if ( !pHead )
		return FALSE;
	
	pFile = *pHead;
	
	while(pFile)
	{
		pFile= pFile->pNext;
		i++;
	}
    nCurAppNum = i;
	
	if(nCurAppNum >3)
		nListhighth = 3;
	else
		nListhighth = nCurAppNum;

	//draw call app
	if(nCur >-iPos + nListhighth || nCur < -iPos)
	{
		nCur ++;
	}
	else
	{
		if(pAppNode)
		{
			LaunchGetRect(&rcIcon, &rcText, nDawnitem,nCurAppNum );
			
			if(nFocus != nCur)
			{
				SetTextColor(hdc, RGB(0,0,0));
				
				SetBkMode(hdc, ALPHATRANSPARENT );
				if(!IsCallAPP())
				{
					DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mhome_43x28.bmp",SRCCOPY);
					SetBkMode(hdc, TRANSPARENT );
					DrawText(hdc, ML("Home"),-1, &rcText, DT_VCENTER | DT_CENTER);
				}
				else
				{
					DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, pAppNode->cIconName,SRCCOPY);
				
					memset(strDirName, 0 ,DISPLAY_NAMELEN + 1);
					
					GetDisplayName(pAppNode->aTitleName, strDirName,DISPLAY_NAMELEN);
					SetBkMode(hdc, TRANSPARENT );
					DrawText(hdc, ML(strDirName),-1, &rcText, DT_VCENTER | DT_CENTER);
				}
				
			}
			else
			{
				char strIconName[PMICON_NAMELEN];
				
				pCurAppNode = pAppNode;
				
				SetRect(&rcFocus, rcIcon.left -7, rcIcon.top - 8,
					rcIcon.right+7, rcText.bottom+ 5);
				
				DrawImageFromFile(hdc,"/rom/progman/select.gif", rcFocus.left, rcFocus.top, SRCCOPY);
				
				if(!IsCallAPP())
				{
					DrawBitmapFromFile(hdc, rcIcon.left-2, rcIcon.top-2, "/rom/progman/app/mghome_43x28.bmp",SRCCOPY);
					SetBkMode(hdc, TRANSPARENT );
					SetTextColor(hdc, RGB(255,255,255));
					DrawText(hdc, ML("Home"),-1, &rcText, DT_VCENTER|DT_CENTER);
					strcpy(strCap, "Home");
				}
				else
				{
					memset(strIconName, 0, PMICON_NAMELEN);
					
					SetBkMode(hdc, ALPHATRANSPARENT );
					
					sprintf(strIconName, "%s%s%s", PMPIC_PATH, "mg", &pAppNode->cIconName[PMPICPATHLEN +1]);
									
					DrawBitmapFromFile(hdc, rcIcon.left-2, rcIcon.top-2, strIconName,SRCCOPY);
				
					memset(strDirName, 0 ,DISPLAY_NAMELEN + 1);
					GetDisplayName(pAppNode->aDisplayName, strDirName, DISPLAY_NAMELEN);
					
					SetBkMode(hdc, TRANSPARENT );
					SetTextColor(hdc, RGB(255,255,255));
					DrawText(hdc, ML(strDirName),-1, &rcText, DT_VCENTER|DT_CENTER);
					
					strDirName[0] = 0;
					GetDisplayName(pAppNode->aTitleName, strDirName, DISPLAY_NAMELEN + 8);
					strcpy(strCap, strDirName);
				}
			}
		}
		nCur++;
		nDawnitem++;
	}

  	pFile = *pHead;

	while(pFile)
	{
		if(pFile->flags)
		{
			pAppNode = pAdm->pFileCache + pFile->appId;

			if(stricmp(pAppNode->aDisplayName, "Call") == NULL)
			{
				pFile =pFile->pNext;
			//	nCur ++;
				continue;
			}
			if(pAppNode->nType & HIDE_APP)
			{
				pFile =pFile->pNext;
				nCurAppNum --;
				continue;
			}
			LaunchGetRect(&rcIcon, &rcText, nDawnitem,nCurAppNum );
			
			if(nDawnitem > nListhighth)
				break;

			if(nCur >-iPos + nListhighth || nCur < -iPos)
			{
				pFile =pFile->pNext;
				nCur ++;
				continue;
			}

			if(nFocus != nCur)
			{
				bDlmEnable = FALSE;
				
				SetTextColor(hdc, RGB(0,0,0));

				SetBkMode(hdc, ALPHATRANSPARENT );
				
				bDlmEnable = DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, pAppNode->cIconName,SRCCOPY);
				
				printf("\r\n DrawBitmapFromFile name = %s, bDlmEnable = %d", pAppNode->cIconName,bDlmEnable);
				if(!bDlmEnable && pAppNode->sFlag & APP_NODE_DLM)
				{
					DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mdefaultapp_43x28.bmp",SRCCOPY);
				}
				if(!bDlmEnable && pAppNode->sFlag & APP_NODE_WAP)
				{
					DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, "/rom/progman/app/mdefaultshortcut_43x28.bmp",SRCCOPY);
				}
				memset(strDirName, 0 ,DISPLAY_NAMELEN + 1);
				
				GetDisplayName(pAppNode->aTitleName, strDirName,DISPLAY_NAMELEN);
				SetBkMode(hdc, TRANSPARENT );
				DrawText(hdc, ML(strDirName),-1, &rcText, DT_VCENTER | DT_CENTER);
			}
			else
			{
				char strIconName[PMICON_NAMELEN];
				
				bDlmEnable = FALSE;
				pCurAppNode = pAppNode;

				SetRect(&rcFocus, rcIcon.left -7, rcIcon.top - 8,
					rcIcon.right+7, rcText.bottom+ 5);
				
				DrawImageFromFile(hdc,"/rom/progman/select.gif", rcFocus.left, rcFocus.top, SRCCOPY);
				memset(strIconName, 0, PMICON_NAMELEN);
				
				SetBkMode(hdc, ALPHATRANSPARENT );
				
				sprintf(strIconName, "%s%s%s", PMPIC_PATH, "mg", &pAppNode->cIconName[PMPICPATHLEN +1]);
				
				bDlmEnable = DrawBitmapFromFile(hdc, rcIcon.left-2, rcIcon.top-2, strIconName,SRCCOPY);

				printf("\r\n DrawBitmapFromFile name = %s, bDlmEnable = %d", strIconName,bDlmEnable);
				if(!bDlmEnable && pAppNode->sFlag & APP_NODE_WAP)
				{
					DrawBitmapFromFile(hdc, rcIcon.left-2, rcIcon.top -2, "/rom/progman/app/mgdefaultshortcut_43x28.bmp",SRCCOPY);
				}
				memset(strDirName, 0 ,DISPLAY_NAMELEN + 1);
				GetDisplayName(pAppNode->aDisplayName, strDirName, DISPLAY_NAMELEN);
				
				SetBkMode(hdc, TRANSPARENT );
				SetTextColor(hdc, RGB(255,255,255));
				DrawText(hdc, ML(strDirName),-1, &rcText, DT_VCENTER|DT_CENTER);
				
				strDirName[0] = 0;
				GetDisplayName(pAppNode->aTitleName, strDirName, DISPLAY_NAMELEN + 8);
				strcpy(strCap, strDirName);
			
			}
			nDawnitem ++;
		}

		pFile =pFile->pNext;
		nCur ++;
	}

	SetTextColor(hdc, RGB(0,0,0));
	AppSwapperCap(hWnd, hdc, pAdm);
	
	SetTextColor(hdc, oldTxtColor);
	SetBkMode(hdc, nOldmode);
	return TRUE;

}
/*********************************************************************\
* Function     AppSwapperCap
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL AppSwapperCap(HWND hWnd, HDC hdc, struct appMain *pAdm)
{
	HBRUSH   hOldBrush = NULL;
	RECT     rcTextCap, rcBg;
	int      nOldmode = 0;
	BOOL     ret = FALSE;
	HFONT    hFont = NULL, hOldFont = NULL;
	COLORREF  OldColor;

	hOldBrush = SelectObject(hdc, hLaunchBgBrush);

	GetFontHandle(&hFont, LARGE_FONT);

	hOldFont = SelectObject(hdc, hFont);

	SetRect(&rcBg, 0,0, 176, 30);
	FillRect(hdc, &rcBg, hLaunchBgBrush);

	nOldmode = GetBkMode(hdc);
	OldColor = SetBkColor(hdc, RGB(0, 0, 0));
    SetBkMode(hdc, NEWTRANSPARENT);

	BitBlt(hdc, 2, 13, 13,11,(HDC)hBmpLeftArrow,0,0 ,ROP_SRC);

	BitBlt(hdc,162, 13, 13,11,(HDC)hBmpRightArrow,0,0 ,ROP_SRC);

	SetBkMode(hdc, TRANSPARENT);
	SetRect(&rcTextCap, 17,5, 159, 23);
	DrawText(hdc, ML(strCap), -1, &rcTextCap, DT_HCENTER|DT_VCENTER);

	SetBkColor(hdc, OldColor);
	SetBkMode(hdc, nOldmode);
	SelectObject(hdc, hOldBrush);
	SelectObject(hdc, hOldFont);

	return TRUE;
}
/*********************************************************************\
* Function     LaunchGetRect
* Purpose      
* Params      
* Return       
**********************************************************************/
static void LaunchGetRect(RECT *rcRect, RECT *rcText, int iCurr, int iStyle)
{	
    POINT Pos; 

	if(iCurr < 0)
		return;

	if(iStyle >= 3)
	{
		Pos.x = iCurr*ITEMWIN;
		Pos.y = 74;
		
		SetRect(rcRect, Pos.x + 10, Pos.y, 
			Pos.x + 10 +ICONWIN, Pos.y + ICONHIG);
		
		SetRect(rcText,Pos.x +5, Pos.y+ 23,
			Pos.x+5  + TEXTWIN , Pos.y + 23 + TEXTHIN);
	}
	else if(iStyle == 2)
	{
		Pos.x = iCurr * ITEMWIN;
		Pos.y = 74;

		SetRect(rcRect, Pos.x + 38, Pos.y, Pos.x + 38 + ICONWIN,
			Pos.y + ICONHIG);
		SetRect(rcText, Pos.x + 33, Pos.y + 23,
			Pos.x + 33 +TEXTWIN, Pos.y + 23 + TEXTHIN);
	}
	else if(iStyle == 1)
	{
		Pos.x = 0;
		Pos.y = 74;
		
		SetRect(rcRect, Pos.x + 66, Pos.y, Pos.x + 66 + ICONWIN,
			Pos.y + ICONHIG);
		SetRect(rcText, Pos.x + 61, Pos.y + 23,
			Pos.x + 61 +TEXTWIN, Pos.y + 23 + TEXTHIN);

	}

}
/*********************************************************************\
* Function     PM_MakeHotKey
* Purpose      
* Params      
* Return       
**********************************************************************/
static void PM_MakeHotKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	PKEYEVENTDATA pKeyData = NULL;
	int i;
	long vkParam[] = 
    {
        VK_F9,VK_DOWN,VK_LEFT,VK_RIGHT, 0
    };
	
	for(i=0; i < 3;i++)
	{
		if(vkParam[i] == LOWORD(wParam))
			break;
	}
	
	pKeyData = GetKeyTypeByCode(LOWORD(wParam));

	if(pKeyData == NULL)
	{
		return;
	}
	
	pCurKeydata = pKeyData;
	
	
	if(pCurKeydata->nType & PM_LONGKEY)
	{
		if(pCurKeydata->nTimerId != 0)
		{
			KillTimer(NULL, pCurKeydata->nTimerId);
		}
		pCurKeydata->nTimerId = SetTimer(NULL, 0,  ET_LONG, f_TimerProc);
		
	}
				
	if(pCurKeydata->nType & PM_SHORTKEY )
	{
		dwTickNow = GetTickCount();
		nInterval = dwTickNow - pCurKeydata->dKicktime;
		if (nInterval < 0)
			nInterval = -nInterval;
		
		pCurKeydata->dKicktime = dwTickNow;
		
	}
	
	if(pKeyData->nType & PM_REPETEDKEY)
	{
		v_nRepeats++;
		if(v_nRepeats == 1)
		{
			pKeyData->nTimerId = SetTimer(NULL, 0,  ET_REPEAT_FIRST, f_RpTimerProc);
			pCurKeydata = pKeyData;
			OnLaunchKeyDown(hWnd, LOWORD(wParam),PM_REPETEDKEY);
		}
	}
}
/*********************************************************************\
* Function     f_TimerProc
* Purpose      
* Params      
* Return       
**********************************************************************/
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
	if(pCurKeydata->nTimerId != 0)
	{
		KillTimer(hWnd, idEvent);
		if(pCurKeydata->nType & PM_LONGKEY)
		{
			OnLaunchKeyDown(pAppMain->hLaunchWnd,  pCurKeydata->nkeyCode, PM_LONGKEY);
		}
	}
	idEvent = 0;
	pCurKeydata->nTimerId = 0;
}
/*********************************************************************\
* Function     f_RpTimerProc
* Purpose      
* Params      
* Return       
**********************************************************************/
static void CALLBACK f_RpTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{

	if(v_nRepeats == 1)
	{
		KillTimer(NULL, pCurKeydata->nTimerId);
		pCurKeydata->nTimerId = SetTimer(NULL,0,  ET_REPEAT_LATER,f_RpTimerProc );
	}
	OnLaunchKeyDown(pAppMain->hLaunchWnd, pCurKeydata->nkeyCode, PM_REPETEDKEY);

}
/*********************************************************************\
* Function     OnLaunchKeyDown
* Purpose      
* Params      
* Return       
**********************************************************************/
static int  OnLaunchKeyDown(HWND hWnd, int KeyCode, WORD vkType)
{
	int nOldFocus = nFocus;
	
    switch(KeyCode)
    {

    case VK_LEFT:
		{
			if(nCurAppNum <= 3)
			{
				if (nFocus > 0)
					nFocus --;
				else if(nFocus == 0)
				{
					nFocus = nCurAppNum -1;
				}
			}
			else
			{
				if(nFocus == 1)
				{
					nFocus--;
				}
				else if(nFocus == 0)
				{
					nFocus = nCurAppNum -1;
					iPos -= nCurAppNum - 3; 
				}
				else if(nFocus == nCurAppNum - 1)
				{
					nFocus--;
				}
				else if(nFocus > 1)
				{
					nFocus --;
					iPos ++;
				}
			}
			
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				LaunchGetGifRect(&rc1, nOldFocus, nCurAppNum);
				LaunchGetGifRect(&rc2, nFocus, nCurAppNum);	
				
				InvalidateRect(hWnd, NULL, TRUE);
				UpdateWindow(hWnd);
			}
			
		}
		break;
	case VK_RIGHT:
		{
			PLXPrintf("\r\n enter right key!");
			if(nCurAppNum <=3)
			{
				if (nFocus < nCurAppNum - 1)
					nFocus ++;
				else if(nFocus == nCurAppNum - 1)
					nFocus = 0;
			}
			else
			{
				if(nFocus +1 == nCurAppNum -1)
				{
					nFocus ++;
				}
				else if(nFocus == nCurAppNum -1)
				{
					nFocus = 0;
					iPos += nCurAppNum -3; 

				}
				else if(nFocus == 0)
				{
					nFocus++;
				}
				else if(nFocus < nCurAppNum - 1)
				{
					nFocus ++;
					iPos --;
				}
			}
			if (nOldFocus != nFocus)
			{
				RECT rc1, rc2;        
				
				LaunchGetGifRect(&rc1, nOldFocus, nCurAppNum);
				LaunchGetGifRect(&rc2, nFocus,nCurAppNum);	
				
				InvalidateRect(hWnd, NULL,TRUE);
				UpdateWindow(hWnd);
			}
			
		}
		break;
	case VK_F9:
		{
			if(vkType == PM_SHORTKEY)
			{
				{
					if(nCurAppNum <=3)
					{
						if (nFocus < nCurAppNum - 1)
							nFocus ++;
						else if(nFocus == nCurAppNum - 1)
							nFocus = 0;
					}
					else
					{
						if(nFocus +1 == nCurAppNum -1)
						{
							nFocus ++;
						}
						else if(nFocus == nCurAppNum -1)
						{
							nFocus = 0;
							iPos += nCurAppNum -3; 
							
						}
						else if(nFocus == 0)
						{
							nFocus++;
						}
						else if(nFocus < nCurAppNum - 1)
						{
							nFocus ++;
							iPos --;
						}
					}
					if (nOldFocus != nFocus)
					{
						RECT rc1, rc2;        
						
						LaunchGetGifRect(&rc1, nOldFocus, nCurAppNum);
						LaunchGetGifRect(&rc2, nFocus,nCurAppNum);	
						
						InvalidateRect(hWnd, NULL,TRUE);
						UpdateWindow(hWnd);
					}
					
				}
			}
			else if(vkType == PM_LONGKEY)
			{
				if((strcmp(pCurAppNode->aDisplayName,"Call") == NULL)
					&&(!IsCallAPP()))
				{
					PMShowIdle();
				}
				else
					AppFileOpen(pAppMain,pCurAppNode->appId, TRUE, 0);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
		}
		break;

    }
	return 0;
}
/*********************************************************************\
* Function     LaunchGetGifRect
* Purpose      
* Params      
* Return       
**********************************************************************/
static void LaunchGetGifRect(RECT *rcRect, int iCurr, int iStyle)
{
	RECT rcText;

	LaunchGetRect(rcRect, &rcText, iCurr, iStyle);

	//adjust for gif
	rcRect->top = rcRect->top -8;
	rcRect->left = rcRect->left -7;
	rcRect->bottom = rcText.bottom + 3;
	rcRect->right = rcRect->right +7;
}
/*********************************************************************\
* Function     GetBackgroudAppNum
* Purpose      get app number in background
* Params      
* Return       
**********************************************************************/
static int GetBackgroudAppNum(struct appMain *pAdm)
{
	PCURAPPLIST *pHead = NULL, pFile = NULL;
	int i  =0;

	pHead = AppFileGetListHead(pAdm);
    if ( !pHead )
		return FALSE;
	
	pFile = *pHead;
	
	while(pFile)
	{
		pFile= pFile->pNext;
		i++;
	}
    nFileNum = i;
	if(nFileNum == 1)
		nFocus = 0;
	else 
		nFocus = 1;
	
	return TRUE;
}
/*********************************************************************\
* Function     CreateQuickMenu
* Purpose      quick menu view
* Params      
* Return       
**********************************************************************/
#define REQUEST_CODE  WM_USER +100
#define CODERIGHT    WM_USER + 101
#define IDC_QUCIKMENU  700
#define ET_SLIDEQ        100
static BOOL bLockRequst;
static int y_qmenu, cy_qmenu,cySlide_qmenu, yFinal_qmenu;
BOOL CreateQuickMenu(struct appMain * pAdm,int nEnterTpye)
{
	WNDCLASS wc;
    HWND hWnd;
	
    wc.style         = 0;
    wc.lpfnWndProc   = QuickMenuWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "quickmenuWindowClass";

    if (!RegisterClass(&wc))
        return FALSE;

	y_qmenu = 197 - CY_SLIDESTART;
	cy_qmenu = CY_SLIDESTART+24;
	cySlide_qmenu = CY_SLIDESTART;
	yFinal_qmenu = CAPTION_HEIGHT+TITLEBAR_HEIGHT;

    hWnd = CreateWindow("quickmenuWindowClass",
        "",
        WS_VISIBLE| PWS_STATICBAR,
		0,	
		y_qmenu ,
		DESK_WIDTH,	
		cy_qmenu,  
        pAdm->hIdleWnd,NULL,	NULL,	NULL);

    if(hWnd == NULL)
    {
        UnregisterClass("quickmenuWindowClass", NULL);
        return FALSE;
    }
	SetTimer(hWnd, IDC_QUCIKMENU, ET_SLIDEQ, NULL);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    pAdm->hQuickMenuhWnd = hWnd;

	if(nEnterTpye == ENTER_UP)
		SendMessage(hList, LB_SETCURSEL, 0, 0);
	else 
		SendMessage(hList, LB_SETCURSEL, MAX_QUICKMENUITEM -1, 0);

    return TRUE;

}
/*********************************************************************\
* Function     QuickMenuWndProc
* Purpose      
* Params      
* Return       
**********************************************************************/
static LRESULT QuickMenuWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT      lResult  = TRUE;	
    HDC hdc;


    switch(wMsgCmd) 
    {
    case WM_CREATE:
	
       	  SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
			(LPARAM)ML("Back"));

		  SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

		  CreateQuickMenuList(hWnd, &hList);
		
		  break;


	case WM_CTLCOLORLISTBOX ://set listbox background white
		
        SetBkColor((HDC)wParam, COLOR_TRANSBK);
        SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
		
        return (LRESULT)(COLOR_MENU + 1);

	case WM_TIMER://slide into view
		{
			if(wParam == IDC_QUCIKMENU)
			{
				cySlide_qmenu += DCY_SLIDEONCE;
				
				if (cySlide_qmenu < -DCY_SLIDEONCE)
				{
					cySlide_qmenu = -DCY_SLIDEONCE;
				}
				y_qmenu -= cySlide_qmenu;
				cy_qmenu += cySlide_qmenu;
				if (y_qmenu <= yFinal_qmenu)
					
				{
					y_qmenu  = yFinal_qmenu;
					cy_qmenu = DESK_HEIGHT- TITLEBAR_HEIGHT -CAPTION_HEIGHT;
					KillTimer(hWnd, wParam);
					cySlide_qmenu = CY_SLIDESTART;
				}
				MoveWindow(hWnd, 0, y_qmenu, DESK_WIDTH, cy_qmenu, FALSE);
			}
		}
		break;
	case WM_SETFOCUS:
		SetFocus(hList);
		break;
		
	case REQUEST_CODE://for phone lock code
		{
			char strCode[EDITMAXLEN+1];
			char cLockcode[EDITMAXLEN +1];

			memset(strCode, 0, EDITMAXLEN + 1);
			memset(cLockcode, 0, EDITMAXLEN + 1);
			
			if( SSPLXVerifyPassword(hWnd, NULL, ML(PHONELOCK), 
				strCode, MAKEWPARAM(EDITMINLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
			{
				GetSecurity_code(cLockcode);
				
				if (strCode[0] != 0 && cLockcode[0] !=0)
				{
					if(strcmp(strCode,cLockcode) != 0)
					{
						PLXTipsWin(NULL, hWnd, REQUEST_CODE,ML(CODE_WRONG), NULL, Notify_Failure, ML(IDS_OK),
							NULL, WAITTIMEOUT);														
					}
					else
					{
						bLockRequst = FALSE;
						PLXTipsWin(NULL, hWnd, CODERIGHT, ML(CODE_RIGHT), NULL, Notify_Success, ML(IDS_OK),
							NULL, WAITTIMEOUT);
						
						
					}
				}
				
			}
		}
		break;
	case CODERIGHT:
		{
			HWND hParentWnd = NULL;

			hParentWnd = GetParent(hWnd);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			CreatePhonelockWnd(hParentWnd);
		}
		break;
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
			;
         }
        else
        {
            SetFocus(hWnd);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
			(LPARAM)ML("Back"));
         }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
			RECT rect;
									
			strCap[0] = 0;
			
            hdc = BeginPaint(hWnd, &ps);
		    GetClientRect(hWnd, &rect);
			ClearRect(hdc, &rect, COLOR_WHITE);
			EndPaint(hWnd, NULL);
        }
        break;
	case WM_CLOSE:

		DestroyWindow(hWnd);
		break;

    case WM_KEYDOWN:
		if(LOWORD(wParam) == VK_F10 )
		{
			if(!bKeyLock && !bPhoneLock)
			{
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;
			}
		}
		else
			OnQuickMenuDown(hWnd,wMsgCmd, wParam, lParam, &hList);
			
        break;

    case WM_DESTROY:
      
		pAppMain->hQuickMenuhWnd  =NULL;
        UnregisterClass("quickmenuWindowClass", NULL);
    	break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}
/*********************************************************************\
* Function     CreateQuickMenuList
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL CreateQuickMenuList(HWND hWnd, HWND* hList)
{
	char* ListMenu[] =
	{
		"Lock keypad",
		"Lock phone",
		"Set GPS profile",
		"Set sound profile"
	};

	int i;
	RECT rect;
	
	GetWindowRect(hWnd, &rect);

    *hList = CreateWindow("LISTBOX", "", 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP |WS_VSCROLL,
        rect.left, 0 , rect.right, rect.bottom,
        hWnd, (HMENU)IDC_MENULIST, NULL, NULL);
    
    if ( *hList == NULL )
        return FALSE;

	SendMessage(*hList, LB_RESETCONTENT, 0, 0);
 
	for(i = 0; i< MAX_QUICKMENUITEM ;i++)
	{
        SendMessage(*hList,LB_ADDSTRING,i,(LPARAM)ML(ListMenu[i]));
	}
	return TRUE;
}
/*********************************************************************\
* Function     OnQuickMenuDown
* Purpose      
* Params      
* Return       
**********************************************************************/
static int  OnQuickMenuDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam, HWND*hList)
{

	int nFocusList = -1;

    switch(wParam)
    {
    case VK_F5:    
		{
			OpenMenuList(hWnd, hList);
		}
		
        break;
	case VK_RETURN:
			break;
	default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
	return 0;

}
/*********************************************************************\
* Function     OpenMenuList
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL OpenMenuList(HWND hWnd,HWND *hList)
{
	int nFocusId = 0;

	nFocusId = SendMessage(*hList, LB_GETCURSEL, 0, 0);

	if(nFocusId == -1)
		return FALSE;
	
	switch(nFocusId)
	{
	case 0:
		{	
			HWND hParentWnd = NULL;
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			CreateKeylockWnd(pAppMain->hIdleWnd);
		}
		
		break;
	case 1:
		{
			char strCode[EDITMAXLEN+1];
			char cLockcode[EDITMAXLEN +1];
			char pAutoLockTime[10];

			pAutoLockTime[0] = 0;
			GetLockCodePeriod((char*)pAutoLockTime);
			if(pAutoLockTime[0] != 0 && (strcmp(pAutoLockTime, "Off") !=0
				&& strcmp(pAutoLockTime,"0") != 0))
			{
				HWND hParentWnd = NULL;
				
				hParentWnd = GetParent(hWnd);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				CreatePhonelockWnd(hParentWnd);
				break;
			}

			memset(strCode, 0, EDITMAXLEN + 1);
			memset(cLockcode, 0, EDITMAXLEN + 1);
			
			if( SSPLXVerifyPassword(hWnd, NULL, ML(PHONELOCK), 
				strCode, MAKEWPARAM(EDITMINLEN,EDITMAXLEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
			{
				GetSecurity_code(cLockcode);
				
				if (strCode[0] != 0 && cLockcode[0] !=0)
				{
					if(strcmp(strCode,cLockcode) != 0)
					{
						PLXTipsWin(NULL, hWnd, REQUEST_CODE,ML(CODE_WRONG), NULL, Notify_Failure, ML(IDS_OK),
							NULL, WAITTIMEOUT);														
					}
					else
					{
						bLockRequst = FALSE;
						PLXTipsWin(NULL, hWnd, CODERIGHT, ML(CODE_RIGHT), NULL, Notify_Success, ML(IDS_OK),
							NULL, WAITTIMEOUT);
					}
				}

			}

		}
		break;
	case 2:
		CreateGPSprofileWnd(hWnd);//for GPS profile view
		break;
	case 3:
		CreateSoundprofileWnd(hWnd);//for sound profile view
		break;

	default:
		break;
	}
	return TRUE;
}
/*********************************************************************\
* Function     CreateKeylockWnd
* Purpose      enter key lock window
* Params      
* Return       
**********************************************************************/
#define  PM_KEYLOCK  WM_USER + 300
static HWND hParentWnd, hKeyLockWnd;
static BOOL bSetConfirm = TRUE, bHaveSetConfirm;
static WORD nKeyWord = 0;
static BOOL bKeyLockHide = FALSE, bTipWnd =FALSE;

HWND GetKeyLockWnd(void)
{
	return hKeyLockWnd;
}
HWND GetKeyLockParentWnd(void)
{
	return hParentWnd;
}
BOOL IfSetConfirm(void)
{
	return bSetConfirm;
}
BOOL IfTipWndLock(void)
{
	return bTipWnd;
}

static LRESULT KeylockWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                             LPARAM lParam);

BOOL CreateKeylockWnd(HWND hWnd)
{
	WNDCLASS wc;

	DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_KEYLIGHT), 10);
	DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 10);
	DlmNotify (PMM_NEWS_ENABLE,PMF_KEYLIGHT|PMF_MAIN_BKLIGHT
		|PMF_AUTO_SHUT_KEYLIGHT|PMF_AUTO_SHUT_MAIN_LIGHT);

    wc.style         = 0;
    wc.lpfnWndProc   = KeylockWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "KeylockWindowClass";
	
    if (!RegisterClass(&wc))
        return FALSE;
	
    hKeyLockWnd = CreateWindow("KeylockWindowClass",
        "",
        WS_VISIBLE|PWS_STATICBAR,
		0,	
		196,
		176,	
		24, 
        NULL, NULL,	NULL,	NULL);
	
    if(hKeyLockWnd == NULL)
    {
        UnregisterClass("PhonelockWindowClass", NULL);
        return FALSE;
    }
	
    ShowWindow(hKeyLockWnd, SW_SHOW);
    UpdateWindow(hKeyLockWnd);


 	hParentWnd = hWnd;
	return TRUE;
}
/*********************************************************************\
* Function     Prog_KeyBoardProc
* Purpose      
* Params      
* Return       
**********************************************************************/
void PM_SetKeyLockStatus(BOOL bStatus);
static	LRESULT	CALLBACK Prog_KeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{	
  
	calliftype callstate = GetCallIf();
	

	if(LOWORD(wParam) == VK_F10 )
	{
		printf("\r\n enter unlock key once!\r\n");
		if(!bLockEnable)
		{
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
		}
		if(callstate == callingif || callstate == ringif || callstate == callendif)
		{
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
		}
		
		bSetConfirm = TRUE;
		if(bSetConfirm)
		{
			if(lParam & 0xc0000000)
			{
				if(nKeyWord != LOWORD(wParam))
				{
					nKeyWord = 0;
					return FALSE;
				}
				nKeyWord = 0;
			}
			else
			{
				nKeyWord = LOWORD(wParam);
			}
			if(lParam & 0xc0000000)//keyup
			{
			//	UnhookWindowsHookEx(prog_kbHook);
			//	prog_kbHook = NULL;
				bLockEnable = FALSE;
			//	PM_SetKeyLockStatus(FALSE);
			//	bKeyLockHide = FALSE;
				if(!bHaveSetConfirm)
				{
					bHaveSetConfirm = TRUE;
					PLXTipsWin(NULL, hKeyLockWnd, PM_KEYLOCK,ML("Unlock keypad?"),NULL,Notify_Request,
						ML("Yes"), ML("No"),50);
				}
			}
		}
	
	}
	else
	{
		if(!bLockEnable)
		{
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
		}
		if(callstate == diallinkif || callstate == ringif || callstate == callingif)
		{
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
		}
		if((hParentWnd == pAppMain->hIdleWnd) && !bSetConfirm)
		{
			if(lParam & 0xc0000000)//key up
				;
			else
			;//	PLXTipsWin(NULL, NULL, NULL,ML("Keypad locked"), NULL,Notify_Info, NULL, ML("Unlock"),WAITTIMEOUT);
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
		}

		bKeyLock = TRUE;
		
		if(bSetConfirm)
		{
			if(lParam & 0xc0000000)	//key up
			{
				if(nKeyWord != LOWORD(wParam))
				{
					nKeyWord = 0;
					return FALSE;
				}
				nKeyWord = 0;
			}
			else
			{
				nKeyWord = LOWORD(wParam);
			}
			if(lParam & 0xc0000000)//key up
				;
			else
			{
				if(nKeyWord ==VK_EARPHONE_OUT || nKeyWord == VK_EARPHONE_IN|| nKeyWord ==VK_CHARGER_IN || nKeyWord == VK_CHARGER_OUT 
			|| nKeyWord == VK_USB_IN || nKeyWord == VK_USB_OUT)
					;
				else
					PLXTipsWin(NULL, NULL, NULL,ML("Keypad locked"), NULL,Notify_Info, NULL, ML("Unlock"),WAITTIMEOUT);
			}
			bSetConfirm = FALSE;
		}
		else
		{
			if(lParam & 0xc0000000)//key up
				;
			else
			{
				if(nKeyWord ==VK_EARPHONE_OUT || nKeyWord == VK_EARPHONE_IN|| nKeyWord ==VK_CHARGER_IN || nKeyWord == VK_CHARGER_OUT 
					|| nKeyWord == VK_USB_IN || nKeyWord == VK_USB_OUT)
					;
				else
					PLXTipsWin(NULL, NULL, NULL,ML("Keypad locked"), NULL,Notify_Info, NULL, ML("Unlock"),WAITTIMEOUT);
			}
			
		}
	}

	return TRUE;
}
/*********************************************************************\
* Function     KeylockWndProc
* Purpose      
* Params      
* Return       
**********************************************************************/
static LRESULT KeylockWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                             LPARAM lParam)
{
    LRESULT      lResult  = TRUE;	
    HDC hdc;


    switch(wMsgCmd) 
    {
    case WM_CREATE:
		
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
			(LPARAM)ML("Unlock"));

		bKeyLock = TRUE;
		bLockEnable = TRUE;
		DlmNotify(PS_KEYLOCK, ICON_SET);
		prog_kbHook = SetWindowsHookEx(WH_KEYBOARD, Prog_KeyBoardProc, 0, 0);
			
		break;
	case PM_KEYLOCK:
		if(LOWORD(lParam) == 1)
		{
			
			bKeyLock = FALSE;
			UnhookWindowsHookEx(prog_kbHook);
			prog_kbHook = NULL;
	
			DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 15);
			DlmNotify (PMM_NEWS_ENABLE,PMF_MAIN_BKLIGHT|PMF_AUTO_SHUT_MAIN_LIGHT);

			PM_SetKeyLockStatus(FALSE);
			bKeyLockHide = FALSE;
			
			DlmNotify(PS_KEYLOCK, ICON_CANCEL);	
			PLXTipsWin(NULL, hParentWnd, NULL, ML("Keypad unlocked"), NULL,Notify_Success,
				ML("Ok"), NULL, WAITTIMEOUT);
			bHaveSetConfirm = FALSE;

			PostMessage(hKeyLockWnd, WM_CLOSE, NULL, NULL);
		}
		else
		{
		//	prog_kbHook = SetWindowsHookEx(WH_KEYBOARD, Prog_KeyBoardProc, 0, 0);
			bSetConfirm = FALSE;
			bHaveSetConfirm = FALSE;
			bLockEnable = TRUE;
		//	bKeyLockHide  =TRUE;
		//	PM_SetKeyLockStatus(TRUE);
		}
		
		break;
  
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
			if(!bHaveSetConfirm && TopWndIsNotification())
			{
				bTipWnd =TRUE;
				bKeyLock = FALSE;
				bLockEnable = FALSE;
				UnhookWindowsHookEx(prog_kbHook);
				prog_kbHook = NULL;
			}
         }
        else
        {
            SetFocus(hWnd);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
				(LPARAM)ML("Unlock"));
			if(bTipWnd)
			{
				bKeyLock = TRUE;
				bLockEnable = TRUE;
				if(!prog_kbHook)
					prog_kbHook = SetWindowsHookEx(WH_KEYBOARD, Prog_KeyBoardProc, 0, 0);
				bTipWnd = FALSE;
			}
		}
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
									
            hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, NULL);
        }
        break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

 	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDB_EXIT:
			
			break;
	
		}
		break;
	case WM_QUERYSYSEXIT:
		return SYSEXIT_CONTINUE;
			
	case WM_SYSEXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

    case WM_DESTROY:
        hKeyLockWnd = NULL;
        UnregisterClass("KeylockWindowClass", NULL);
    	break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
	
    return lResult;
}
/*********************************************************************\
* Function     CreatePhonelockWnd
* Purpose      enter phone lock window
* Params      
* Return       
**********************************************************************/
#define  IDB_LOCK_OK  500
#define  IDS_OK      "Ok"
#define  IDS_UNLOCK  "Unlock"
#define  IDS_ENTERPHONECODE "Enter phone\r\nlock code:"
#define  IDS_PHONECODE "Phone lock"
#define  IDS_CODERIGHT "Code right"
#define  IDS_CODEWRONG "Code wrong"
#define  MAX_CODELEN  8
#define  MIN_CODELEN  4
static HWND hActiveWnd;
static HWND hPhonelockList ;
static HBRUSH hPhonelockBgBrush;
static BOOL bPhoneLockHide = FALSE;
void  PM_SetPhoneLockStatus(BOOL bStatus);
BOOL InitMissedEventsMemory();

static BOOL CreatePhonelockWnd(HWND hWnd)
{
		
	WNDCLASS wc;
	
    wc.style         = 0;
    wc.lpfnWndProc   = PhonelockWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "PhonelockWindowClass";
	
    if (!RegisterClass(&wc))
        return FALSE;
	
    hPhonelockWnd = CreateWindow("PhonelockWindowClass",
        ML("Phone locked"),
        WS_VISIBLE| PWS_STATICBAR|WS_CAPTION,
		0,	
		TITLEBAR_HEIGHT,
		DESK_WIDTH,	
		DESK_HEIGHT- TITLEBAR_HEIGHT, 
        NULL,NULL,	NULL,	NULL);
	
    if(hPhonelockWnd == NULL)
    {
        UnregisterClass("PhonelockWindowClass", NULL);
        return FALSE;
    }
	
    ShowWindow(hPhonelockWnd, SW_SHOW);
    UpdateWindow(hPhonelockWnd);

	return TRUE;
}
static LRESULT PhonelockWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                             LPARAM lParam)
{
    LRESULT      lResult  = TRUE;	


    switch(wMsgCmd) 
    {
    case WM_CREATE:
		
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_LOCK_OK, 0), 
			(LPARAM)ML(IDS_UNLOCK));

		SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
		SendMessage(hWnd, PWM_SETAPPICON,MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
		bPhoneLock = TRUE;
		SetPhone_Open(TRUE);
		if(InitMissedEventsNum())
		{
			CreateMissedEventsList(hWnd, &hPhonelockList);
			hPhonelockBgBrush = CreateSolidBrush(RGB(158,177,230));
		}

    	break;
  
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
         }
        else
        {
            SetFocus(hWnd);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_LOCK_OK, 0), 
			(LPARAM)ML(IDS_UNLOCK));
         }
        break;

	case WM_CTLCOLORLISTBOX :
		
        SetBkColor((HDC)wParam, COLOR_TRANSBK);
        SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));
		
        return (LRESULT)(COLOR_MENU + 1);
			  
	
    case WM_PAINT:
        {
			BLENDFUNCTION blendFunction;
			int           nMode;
			HDC           hdcMem, hdcTans;
			PAINTSTRUCT ps;
			RECT        rect;
			BOOL        ret, bCellIdUp = FALSE;
			HFONT       hFont, hOldFont;
			int         nLine = 0, len = 0, nFit = 0,nClientWidth = 0;
			HBRUSH   hOldBrush = NULL;
			RECT     rcTextCap, rcBg;
			int      nOldmode = 0;
						
			hdcTans = BeginPaint(hWnd, &ps);
			
			if(InitMissedEventsMemory())//if have missed events
			{
				hdcMem = CreateMemoryDC(176, 31);
				BitBlt(hdcMem, 0, 0, 176, 31, hdcTans, 0,0,SRCCOPY);
				
				hOldBrush = SelectObject(hdcMem, hPhonelockBgBrush);
				
				GetFontHandle(&hFont, LARGE_FONT);
				
				hOldFont = SelectObject(hdcMem, hFont);
				
				SetRect(&rcBg, 0,1, 176, 31);
				FillRect(hdcMem, &rcBg, hPhonelockBgBrush);
				
				nOldmode = GetBkMode(hdcMem);
				
				SetBkMode(hdcMem, TRANSPARENT);
				SetRect(&rcTextCap, 18,5, 142, 23);
				DrawText(hdcMem, ML("Missed"), -1, &rcTextCap, DT_CENTER);
				
				SetBkMode(hdcMem, nOldmode);
				SelectObject(hdcMem, hOldBrush);
				SelectObject(hdcMem, hOldFont);
				
				BitBlt(hdcTans, 0, 0, 176, 31,  hdcMem, 0, 0, SRCCOPY);
				
				DeleteDC(hdcMem);
				
				if(hPhonelockList == NULL)
				{
					CreateMissedEventsList(hWnd, &hPhonelockList);
					if(hPhonelockBgBrush)
					{
						DeleteObject(hPhonelockBgBrush);
						hPhonelockBgBrush = NULL;
					}
					hPhonelockBgBrush = CreateSolidBrush(RGB(158,177,230));
				}
				else
				{
					MISSEDDATA *p = NULL;
					int i;
					char cNum[6];
					
					SendMessage(hPhonelockList, LB_RESETCONTENT,0,0);  
					p = pMissedEnventsHead;
					
					for(p = pMissedEnventsHead, i =0;p != NULL;i++)
					{
						char str[20];
						
						cNum[0] = 0;
						str[0] = 0;
						
						sprintf(cNum, "(%d)", p->number);
						
						switch(p->nType)
						{
						case CALL_APP:
							sprintf(str, "%s %s", ML("Calls"), cNum); 
							break;
							
						case MSG_APP:
							sprintf(str, "%s %s", ML("Messages"), cNum); 
							break;
							
						case MAIL_APP:
							sprintf(str, "%s %s", ML("E-mails"), cNum);
							break;
						case  PUSHMSG:
							sprintf(str, "%s %s", ML("Push message"), cNum);
							break;
						}
						
						SendMessage(hPhonelockList,LB_ADDSTRING,i,(LPARAM)str);
						SendMessage(hPhonelockList, LB_SETITEMDATA, i, (LPARAM)p->nType);
						p = p->pNext;
					}
					
					SendMessage(hPhonelockList,LB_SETCURSEL,0,0);
					
					
				}
				EndPaint(hWnd, NULL);
				break;
			}
			
			GetFontHandle(&hFont, SMALL_FONT);
			
			BitBlt(hdcTans, 0, 0, DESK_WIDTH, DESK_HEIGHT-STATUSBAR_HEIGHT, (HDC)hIldeBgBmp, 0, 0, ROP_SRC);
			
			nMode = SetBkMode(hdcTans, NEWTRANSPARENT );
			SetBkColor(hdcTans,0x000000);
			DrawNotifyIconEx(hdcTans);

			hdcMem = CreateMemoryDC(DESK_WIDTH, LINE1_Y + LINEHIGH);
			SetBkColor(hdcTans,0x000000);
			
			//rect for operator & service provider
			SetRect(&rect, 0, LINE1_Y, DESK_WIDTH, LINE1_Y + LINEHIGH );
			ClearRect( hdcMem, &rect, 0xffffff);
			SetBkMode(hdcTans, BM_NEWALPHATRANSPARENT );
			
			blendFunction.AlphaFormat	= 0;
			blendFunction.BlendFlags	= 0;
			blendFunction.BlendOp		= AC_SRC_OVER;
			blendFunction.SourceConstantAlpha = 160;
			
			ret = AlphaBlend(hdcTans, 0,LINE1_Y , DESK_WIDTH , LINE1_Y + LINEHIGH, 
				hdcMem, 0, LINE1_Y , DESK_WIDTH , LINE1_Y + LINEHIGH, blendFunction );
			
			DeleteDC(hdcMem);

			//rect for cell id & data
			idleshow = GetIdleShowInfo();

			if((idleshow == IS_CELLID && ProgDeskIdle.strCellId[0] != 0)|| idleshow == IS_DATE)
			{
				hdcMem = CreateMemoryDC(DESK_WIDTH, LINE2_Y+LINEHIGH);
				SetBkColor(hdcTans,0x000000);

				if(idleshow == IS_CELLID)
				{
					nClientWidth = DESK_WIDTH;
					len = strlen(ProgDeskIdle.strCellId);
					GetTextExtentExPoint(hdcTans, ProgDeskIdle.strCellId, len, nClientWidth, &nFit, NULL, NULL);
					if(nFit < len)
					{
						SetRect(&rect, 0, LINE2_Y-LINEHIGH, DESK_WIDTH, LINE2_Y+LINEHIGH);
						ClearRect(hdcMem, &rect, 0xffffff);
						
						ret = AlphaBlend(hdcTans, 0, LINE2_Y- LINEHIGH, DESK_WIDTH, LINE2_Y+LINEHIGH,
							hdcMem, 0, LINE2_Y- LINEHIGH, DESK_WIDTH, LINE2_Y+LINEHIGH, blendFunction);
						
						SetRect(&(ProgDeskIdle.rCellId), 0, LINE2_Y-LINEHIGH, DESK_WIDTH, LINE2_Y +LINEHIGH
							);
						bCellIdUp = TRUE;
					}
					else
					{
						SetRect(&rect, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH);
						ClearRect(hdcMem, &rect, 0xffffff);
						
						ret = AlphaBlend(hdcTans, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH,
							hdcMem, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH, blendFunction);
						
					}
				}
				else
				{
					SetRect(&rect, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH);
					ClearRect(hdcMem, &rect, 0xffffff);
					
					ret = AlphaBlend(hdcTans, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH,
						hdcMem, 0, LINE2_Y, DESK_WIDTH, LINE2_Y+LINEHIGH, blendFunction);
				}
				
				DeleteDC(hdcMem);
			}

			//rect for idle text
			PMView_Layout(hWnd, ProgDeskIdle.cIdleModeTxt);
			nIdleTxtLine = PMView_GetLine();
			
			if(nIdleTxtLine > MAX_IDLETXTLEN)
			{
				SetTimer(hWnd, TIMER_ID_IDLETXT, IDLETXT_TIMEOUT, NULL);
				nLine = MAX_IDLETXTLEN;
			}
			else
				nLine = nIdleTxtLine;
			
			if(nLine == MAX_IDLETXTLEN && bCellIdUp)
				nLine--;
			
			SetRect(&ProgDeskIdle.rIdleTxt, 0,  LINETXT_Y, PLX_WIN_WIDTH,
				LINETXT_Y + nLine * LINETXT_HIGHT);
			
			hdcMem = CreateMemoryDC(DESK_WIDTH, LINETXT_Y + nLine * LINETXT_HIGHT);
			SetBkColor(hdcTans,0x000000);
			
			SetRect(&rect, 0, LINETXT_Y, DESK_WIDTH, LINETXT_Y + nLine * LINETXT_HIGHT );
			ClearRect( hdcMem, &rect, 0xffffff);
			
			ret = AlphaBlend(hdcTans, 0,LINETXT_Y, DESK_WIDTH , LINETXT_Y + nLine * LINETXT_HIGHT, 
				hdcMem, 0, LINETXT_Y , DESK_WIDTH , LINETXT_Y + nLine * LINETXT_HIGHT, blendFunction );
			
			DeleteDC(hdcMem);
			
			//draw idle text
			SetBkMode(hdcTans, nMode);
			
			GetClientRect(hWnd, &rect);
			
			hdcMem = CreateMemoryDC(rect.right, rect.bottom );
			
			BitBlt(hdcMem, 0, 0, rect.right, rect.bottom, hdcTans, 0,0,SRCCOPY);
			
			hOldFont = SelectObject ( hdcMem, hFont );
			
			DrawIdleText(hWnd,hdcMem, bCellIdUp);
			
			SelectObject(hdcMem, hOldFont);
			
			BitBlt(hdcTans, 0, 0, rect.right, rect.bottom,
				hdcMem, 0, 0, SRCCOPY);
			
			DeleteDC(hdcMem);
			hdcMem = NULL;
			
			EndPaint(hWnd, NULL);
        }
        break;
		
	case WM_CLOSE:
		bPhoneLock  = FALSE;
		SetPhone_Open(FALSE);
		DestroyWindow(hWnd);
		break;

	case CODERIGHT:
		PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		break;

	case REQUEST_CODE:
		{
			char strCode[MAX_CODELEN + 1];
			char cLockcode[MAX_CODELEN + 1];
			
			memset(strCode, 0, MAX_CODELEN + 1);
			memset(cLockcode, 0, MAX_CODELEN + 1);
	
			bPhoneLockHide = TRUE;

			if(SSPLXVerifyPassword(hWnd, NULL, ML(IDS_ENTERPHONECODE), 
						strCode, MAKEWPARAM(MIN_CODELEN, MAX_CODELEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
			{
				GetSecurity_code(cLockcode);
				
				if (strCode[0] != 0 && cLockcode[0] !=0)
				{
					if(strcmp(strCode,cLockcode) != 0)
					{
						PLXTipsWin(NULL , hWnd, REQUEST_CODE, ML(IDS_CODEWRONG), ML(IDS_PHONECODE), Notify_Failure, ML("Ok"),
							NULL, WAITTIMEOUT);	
					}
					else
					{
						bPhoneLockHide = FALSE;
						PLXTipsWin(NULL, hWnd, CODERIGHT, ML(IDS_CODERIGHT), ML(IDS_PHONECODE), Notify_Success, ML(IDS_OK),
							NULL, WAITTIMEOUT);	
					}
				}
			}
			else
			{
				bPhoneLockHide = FALSE;
				PM_SetPhoneLockStatus(TRUE);
			}
		}
		break;
		
    case WM_KEYDOWN:
		
        if(LOWORD(wParam) == VK_F10)
		{
	
			char strCode[MAX_CODELEN + 1];
			char cLockcode[MAX_CODELEN + 1];
			
			memset(strCode, 0, MAX_CODELEN + 1);
			memset(cLockcode, 0, MAX_CODELEN + 1);
	
			bPhoneLockHide = TRUE;
			PM_SetPhoneLockStatus(FALSE);

			if(SSPLXVerifyPassword(hWnd, NULL, ML(IDS_ENTERPHONECODE), 
						strCode, MAKEWPARAM(MIN_CODELEN, MAX_CODELEN), ML(IDS_OK),ML(IDS_CANCEL), -1))
			{
				GetSecurity_code(cLockcode);
				
				if (strCode[0] != 0 && cLockcode[0] !=0)
				{
					if(strcmp(strCode,cLockcode) != 0)
					{
						PLXTipsWin(NULL , hWnd, REQUEST_CODE, ML(IDS_CODEWRONG), ML(IDS_PHONECODE), Notify_Failure, ML("Ok"),
							NULL, WAITTIMEOUT);	
					}
					else
					{
						bPhoneLockHide = FALSE;
						PLXTipsWin(NULL, hWnd, CODERIGHT, ML(IDS_CODERIGHT), ML(IDS_PHONECODE), Notify_Success, ML(IDS_OK),
							NULL, WAITTIMEOUT);	
					}
				}
			}
			else
			{
				PM_SetPhoneLockStatus(TRUE);
				bPhoneLockHide = FALSE;
			}
		}
		else
		{
			if(!bPhoneLockHide)
			{
				PM_SetPhoneLockStatus(TRUE);
			}
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		
        break;

    case WM_DESTROY:
		if(hActiveWnd)
			ShowWindow(hActiveWnd, SW_SHOW);
		if(hPhonelockBgBrush)
		{
			DeleteObject(hPhonelockBgBrush);
			hPhonelockBgBrush = NULL;
		}
		hPhonelockWnd = NULL;
		hActiveWnd  = NULL;
		hPhonelockList = NULL;
	//	if(IsTimeChanged())
		{
			SYSTEMTIME	SystemTime;
			char		aTimeBuf[25], aDateBuf[25];
			
			memset(&SystemTime, 0, sizeof(SYSTEMTIME));
			memset(aTimeBuf, 0, 25);
			memset(aDateBuf, 0, 25);
			
			GetLocalTime(&SystemTime);
			
			GetTimeDisplay(SystemTime, aTimeBuf, aDateBuf);
			
			SetWindowText(pAppMain->hIdleWnd, aTimeBuf);
			
			InvalidateRect(pAppMain->hIdleWnd, &ProgDeskIdle.rCellId, TRUE);
		}
        UnregisterClass("PhonelockWindowClass", NULL);

		break;
		
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
	
    return lResult;
}
/*********************************************************************\
* Function     GetPhoneLockWnd
* Purpose      
* Params      
* Return       
**********************************************************************/
HWND    GetPhoneLockWnd()
{
	return hPhonelockWnd;
}
/*********************************************************************\
* Function     CreateGPSprofileWnd
* Purpose      
* Params      
* Return       
**********************************************************************/
static HBITMAP hGpsOffIcon;
BOOL CreateGPSprofileWnd(HWND hParentWnd)
{
	WNDCLASS wc;
	HWND hWnd;
	
    wc.style         = 0;
    wc.lpfnWndProc   = GPSprofileWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "GPSprofileWindowClass";
	
    if (!RegisterClass(&wc))
        return FALSE;
	
	pGpsProfileListHead = NULL;

	InitGPSData();
	
    hWnd = CreateWindow("GPSprofileWindowClass",
        ML("GPS profiles"),
        WS_VISIBLE| PWS_STATICBAR|WS_CAPTION,
		0,	
		TITLEBAR_HEIGHT,
		DESK_WIDTH,	
		DESK_HEIGHT- TITLEBAR_HEIGHT, 
        hParentWnd,NULL,	NULL,	NULL);
	
    if(hWnd == NULL)
    {
        UnregisterClass("GPSprofileWindowClass", NULL);
        return FALSE;
    }
	
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
		
	return TRUE;
}
static LRESULT GPSprofileWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT      lResult  = TRUE;	
    HDC hdc;


    switch(wMsgCmd) 
    {
    case WM_CREATE:
	
		SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
			(LPARAM)ML("Back"));
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		hActIcon_Sound  =  LoadImage(NULL, "/rom/progman/icon/sett_active_20x16.bmp", IMAGE_BITMAP,
			PORICON_WIDTH, PROICON_HEIGHT, LR_LOADFROMFILE);
		CreateGpsprofileList(hWnd, &hGpsList);
    	break;
  
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
			;
		}
        else
        {
            SetFocus(hGpsList);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
				(LPARAM)ML("Back"));
		}
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
									
            hdc = BeginPaint(hWnd, &ps);
			
			EndPaint(hWnd, NULL);
        }
        break;
	case WM_QUERYSYSEXIT:
		return SYSEXIT_CONTINUE;
			
	case WM_SYSEXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case PM_SETPROFILE:
		{
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			if(pAppMain->hQuickMenuhWnd)
				PostMessage(pAppMain->hQuickMenuhWnd, WM_CLOSE, NULL, NULL);
		}
		break;

    case WM_KEYDOWN:
		if(LOWORD(wParam) == VK_F10)
		{
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
            break;
		}
	    else if(LOWORD(wParam) == VK_F5)
		{
			int nFocusItem;
			char strName[MAX_PROFILELEN];
			char *p = NULL;
	
			nFocusItem = SendMessage(hGpsList, LB_GETCURSEL, 0, 0);

			if(nFocusItem < 0)
				break;

			if(nFocusItem == 0)
			{
				SetActiveGPSProfile(-1);
				PLXTipsWin(NULL, hWnd, PM_SETPROFILE, ML("GPS switched off"), ML("GPS profiles"), Notify_Success, 
				ML(IDS_OK),NULL, WAITTIMEOUT);
			}
			else
			{
				memset(strName, 0,MAX_PROFILELEN);
				p = (char*)SendMessage(hGpsList, LB_GETITEMDATA, nFocusItem, 0);
				//call set gps profle
				sprintf(strName, "%s:\r\nActivated", p);
				SetActiveGPSProfile(nFocusItem -1);
				bGPSprofileActivate = TRUE;
				PLXTipsWin(NULL, hWnd, PM_SETPROFILE, strName, ML("GPS profiles"), Notify_Success,  ML(IDS_OK), NULL , 20);
			}

			DlmNotify(PS_FRASHGPS, TRUE);
			
			break;
		}
		else
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
		
    case WM_DESTROY:
		FreeGpsData();
        if(hActIcon_Sound)
		{
			DeleteObject(hActIcon_Sound);
			hActIcon_Sound = NULL;
		}
		if(hGpsOffIcon)
		{
			DeleteObject(hGpsOffIcon);
			hGpsOffIcon = NULL;
		}
        UnregisterClass("GPSprofileWindowClass", NULL);
    	break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
	
    return lResult;
}
/*********************************************************************\
* Function     CreateGpsprofileList
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL CreateGpsprofileList(HWND hWnd,HWND* hList)
{
	int i =1;
	DWORD dWord;
	PProfileDATA pProfile =NULL;
	RECT rect;
	
	int iAvc =  GetActiveGPSProfile();
		 
	//read gps profiles

	GetClientRect(hWnd, &rect);
	*hList = CreateWindow("LISTBOX","", 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP |WS_VSCROLL,
		rect.left, rect.top , rect.right, rect.bottom,
        hWnd, (HMENU)IDC_GPS_LIST, NULL, NULL );

	if(*hList == NULL)
		return FALSE;


	if(pGpsProfileListHead != NULL)
		pProfile = pGpsProfileListHead;

	SendMessage(*hList, LB_RESETCONTENT, 0, 0);

	SendMessage(*hList,LB_ADDSTRING,0,(LPARAM)ML("GPS off"));

	hGpsOffIcon = LoadImage(NULL, "/rom/setup/gps/gps_off_ex.bmp", IMAGE_BITMAP,
		PORICON_WIDTH, PROICON_HEIGHT, LR_LOADFROMFILE);
	dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)0);
	SendMessage(*hList, LB_SETIMAGE, (WPARAM)dWord, (LPARAM)hGpsOffIcon);    

	if(iAvc == -1)
	{
		dWord = MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), 0);	
		SendMessage(*hList, LB_SETIMAGE, (WPARAM)dWord, (LPARAM)
			hActIcon_Sound);
	}

	while(pProfile != NULL)
	{
		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)ML(pProfile->pProfileName));

		if(pProfile->hIcon == NULL)
			pProfile->hIcon = LoadImage(NULL, pProfile->pIconName, IMAGE_BITMAP,
			PORICON_WIDTH, PROICON_HEIGHT, LR_LOADFROMFILE);

		printf("\r\n pProfile iconname = %s", pProfile->pIconName);
		dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
		SendMessage(*hList, LB_SETIMAGE, (WPARAM)dWord, (LPARAM)pProfile->hIcon);

		dWord = MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), i);
		if(pProfile->bActive)
			SendMessage(*hList, LB_SETIMAGE, (WPARAM)dWord, (LPARAM)hActIcon_Sound);
		
		SendMessage(*hList, LB_SETITEMDATA, i, (LPARAM)pProfile->pProfileName);

		pProfile = pProfile->pNext;
		i++;
	}
	
	SendMessage(*hList,LB_SETCURSEL,0,0);

	return TRUE;

}
/*********************************************************************\
* Function     InitGPSData
* Purpose     
* Params      
* Return       
**********************************************************************/
extern GetGPSprofileNum();
static void GetGPSIconBmp(char* IconName, char* BmpName)
{
	char* cTmp = NULL;
	
	if(IconName == NULL || BmpName == NULL)
		return;

	strcpy(BmpName, IconName);

	cTmp = strstr(BmpName, ".ico");

	if (cTmp != NULL)
	{
		strcpy(cTmp, "_Ex.bmp");
	}

}
static BOOL InitGPSData(void)
{
	PProfileDATA pProfile = NULL, p =NULL;
	GPSMODE gm;
	int i, num = GetGPSprofileNum(), iAvc =  GetActiveGPSProfile() ;
	
	for(i =0; i< num;i++)
	{
		GetGPSprofile(&gm, i);
		pProfile = malloc(sizeof(ProfileDATA));
		if(pProfile == NULL)
			return FALSE;
		pProfile->hIcon = NULL;
		strcpy(pProfile->pProfileName , gm.cGPSModeName);
		strcpy(pProfile->pIconName, gm.cGPSIconName);
		if(pProfile->pProfileName[0] !=0)
		{
			char strBmpName[128];

			strBmpName[0] = 0;

			GetGPSIconBmp(pProfile->pIconName, strBmpName);
			strcpy(pProfile->pIconName, strBmpName);	
		}
		else
			pProfile->pIconName[0] = 0;
		
		if(iAvc == i)
			pProfile->bActive = TRUE;
		else
			pProfile->bActive = FALSE;
		
		pProfile->pNext = NULL;
		if(pGpsProfileListHead)
		{
			p = pGpsProfileListHead;
			while(p->pNext)
			{
				p = p->pNext;
			}
			p->pNext = pProfile;
		}
		else
			pGpsProfileListHead = pProfile;
		
	}
	
	return TRUE;
}
/*********************************************************************\
* Function     FreeGpsData
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL FreeGpsData(void)
{
	PProfileDATA p, pProfile;

	if(pGpsProfileListHead == NULL)
		return TRUE;

	p = pGpsProfileListHead;

	while(p)
	{
		pProfile = p;
		p =p->pNext;
		if(pProfile->hIcon!= NULL)
		{
			DeleteObject(pProfile->hIcon);
			pProfile->hIcon  = NULL;
		}
		free(pProfile);
	}
	pGpsProfileListHead = NULL;

	return TRUE;
}
/*********************************************************************\
* Function     CreateSoundprofileWnd
* Purpose      
* Params      
* Return       
**********************************************************************/
static HWND hSoundProfileWnd;
HWND GetSoundPorfileWnd(void)
{
	return hSoundProfileWnd;
}
static BOOL CreateSoundprofileWnd(HWND hParentWnd)
{
	WNDCLASS wc;
	HWND hWnd;
	
    wc.style         = 0;
    wc.lpfnWndProc   = SoundprofileWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "SoundprofileWindowClass";
	
    if (!RegisterClass(&wc))
        return FALSE;
	
	InitSoundData();

	if(hParentWnd == pAppMain->hIdleWnd)
		hParentWnd = NULL;

    hWnd = CreateWindow("SoundprofileWindowClass",
        ML("Sound profiles"),
        WS_VISIBLE| PWS_STATICBAR|WS_CAPTION,
		0,	
		TITLEBAR_HEIGHT,
		DESK_WIDTH,	
		DESK_HEIGHT- TITLEBAR_HEIGHT, 
        hParentWnd,NULL,	NULL,	NULL);
	
    if(hWnd == NULL)
    {
        UnregisterClass("SoundprofileWindowClass", NULL);
        return FALSE;
    }
	if(hParentWnd == NULL)
		hSoundProfileWnd = hWnd;
	
    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);
	
	return TRUE;
}
static LRESULT SoundprofileWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT      lResult  = TRUE;	
    HDC hdc;


    switch(wMsgCmd) 
    {
    case WM_CREATE:
	
       	  SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
			  (LPARAM)ML("Back"));
		  SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		  hActIcon_Sound  =  LoadImage(NULL, "/rom/progman/icon/sett_active_20x16.bmp", IMAGE_BITMAP,
			PORICON_WIDTH, PROICON_HEIGHT, LR_LOADFROMFILE);
		  CreateSoundprofileList(hWnd, &hSoundList);
		
    	break;
  
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
			;
		}
        else
        {
            SetFocus(hSoundList);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
				(LPARAM)ML("Back"));
		}
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
									
            hdc = BeginPaint(hWnd, &ps);
			
			EndPaint(hWnd, NULL);
        }
        break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case PM_SETPROFILE:
		{
			SCENEMODE scenemode;
			int nFocusItem;
			
			nFocusItem = SendMessage(hSoundList, LB_GETCURSEL, 0, 0);
			
			if(nFocusItem < 0)
				break;

			GetSM(&scenemode, nFocusItem);
			SetSM(&scenemode,nFocusItem);
			SetCurSceneMode(nFocusItem);
		
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			
			if(pAppMain->hQuickMenuhWnd)
				PostMessage(pAppMain->hQuickMenuhWnd, WM_CLOSE, NULL, NULL);
		}
		break;

    case WM_KEYDOWN:
		if(LOWORD(wParam) == VK_F10)
		{
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
            break;				
		}
		else if(LOWORD(wParam) == VK_F5)
		{
			int nFocusItem;
			char strName[MAX_PROFILELEN];
			char *p =NULL;
		
			
			nFocusItem = SendMessage(hSoundList, LB_GETCURSEL, 0, 0);
			
			if(nFocusItem < 0)
				break;
			
			memset(strName, 0, MAX_PROFILELEN);
			p = strName;
			p = (char*)SendMessage(hSoundList, LB_GETITEMDATA, nFocusItem, 0);
			sprintf(strName, "%s:\r\nActivated", p);
			//call set sound profle
			bSoundprofileActivate = TRUE;

			if(stricmp(p, "Normal") == NULL)
			{
				DlmNotify(PS_SPROHTER, ICON_CANCEL);
				DlmNotify(PS_SILENCE, ICON_CANCEL);
			}
			else
			{
				if(stricmp(p, "Silent") == NULL)
				{
					DlmNotify(PS_SPROHTER, ICON_CANCEL);
					DlmNotify(PS_SILENCE, ICON_SET);
				}
				else 
				{
					DlmNotify(PS_SILENCE, ICON_CANCEL);
					DlmNotify(PS_SPROHTER, ICON_SET);
				}
			}
		
			PLXTipsWin(NULL, hWnd, PM_SETPROFILE, strName, ML("Sound profiles"),Notify_Success,ML(IDS_OK), NULL , 20);
	
			break;
		}
		else
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
				
        break;
	case WM_QUERYSYSEXIT:
		return SYSEXIT_CONTINUE;
			
	case WM_SYSEXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

    case WM_DESTROY:
		FreeSoundData();
        if(hActIcon_Sound)
		{
			DeleteObject(hActIcon_Sound);
			hActIcon_Sound = NULL;
		}
		hSoundProfileWnd = NULL;
        UnregisterClass("SoundprofileWindowClass", NULL);
    	break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }
	
    return lResult;
}
/*********************************************************************\
* Function     CreateSoundprofileList
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL CreateSoundprofileList(HWND hWnd, HWND *hList)
{

	int i =0;
	DWORD dWord;
	PProfileDATA pProfile =NULL;
	RECT rect;
		 

	GetClientRect(hWnd, &rect);
	*hList = CreateWindow("LISTBOX","", 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP |WS_VSCROLL,
        rect.left, rect.top , rect.right, rect.bottom,
        hWnd, (HMENU)IDC_SOUND_LIST, NULL, NULL );

	if(*hList == NULL)
		return FALSE;
	

	if(pSoundPorfileListHead != NULL)
		pProfile = pSoundPorfileListHead;

	while(pProfile != NULL)
	{
		SendMessage(*hList,LB_ADDSTRING,NULL,(LPARAM)ML(pProfile->pProfileName));

		if(stricmp(pProfile->pProfileName, "Normal") != NULL)
		{
			if(pProfile->hIcon == NULL)
				pProfile->hIcon = LoadImage(NULL, pProfile->pIconName, IMAGE_BITMAP,
				PORICON_WIDTH, PROICON_HEIGHT, LR_LOADFROMFILE);
			dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
			
			SendMessage(*hList, LB_SETIMAGE, (WPARAM)dWord, (LPARAM)pProfile->hIcon);
		}

		dWord = MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), i);
		if(pProfile->bActive)
			SendMessage(*hList, LB_SETIMAGE, (WPARAM)dWord, (LPARAM)hActIcon_Sound);
		
		SendMessage(*hList, LB_SETITEMDATA, i, (LPARAM)pProfile->pProfileName);
		
		pProfile = pProfile->pNext;
		i++;
	}
	
	SendMessage(*hList,LB_SETCURSEL,0,0);

	return TRUE;
}
/*********************************************************************\
* Function     InitSoundData
* Purpose      
* Params      
* Return       
**********************************************************************/
extern int GetUserProfileNo();
extern BOOL GetSM(SCENEMODE * scenemode,int iscenemode);
extern int  GetCurSceneMode(void);//get the NO. of current sound profle

static BOOL InitSoundData(void)
{
	PProfileDATA pProfile = NULL, p =NULL;
	int i, num =4 + GetUserProfileNo(), iAvc = GetCurSceneMode();//4 means default
	SCENEMODE scenemode;

    for(i =0; i< num;i++)
	{
		GetSM(&scenemode,i);
		pProfile = malloc(sizeof(ProfileDATA));
		if(pProfile == NULL)
			return FALSE;
		pProfile->hIcon = NULL;
		strcpy(pProfile->pProfileName , scenemode.cModeName);
		if(pProfile->pProfileName[0] !=0)
		{
			if(!strcmp(pProfile->pProfileName, "Silent"))
				strcpy(pProfile->pIconName, "/rom/progman/icon/idle_d_silentprofile_15x15.bmp");
			else 
				strcpy(pProfile->pIconName, "/rom/progman/icon/idle_d_profile_15x15.bmp");
		
		}
		else
			pProfile->pIconName[0] = 0;

		if(i == iAvc)
			pProfile->bActive = TRUE;
		else
			pProfile->bActive = FALSE;

		pProfile->pNext = NULL;
		if(pSoundPorfileListHead)
		{
			p = pSoundPorfileListHead;
			while(p->pNext)
			{
				p = p->pNext;
			}
			p->pNext = pProfile;
		}
		else
			pSoundPorfileListHead = pProfile;
		
	}
	
	return TRUE;
}
/*********************************************************************\
* Function     FreeSoundData
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL FreeSoundData(void)
{
	PProfileDATA p, pProfile;

	if(pSoundPorfileListHead == NULL)
		return TRUE;

	p = pSoundPorfileListHead;

	while(p)
	{
		pProfile = p;
		p =p->pNext;
	
		if(pProfile->hIcon!= NULL)
		{
			DeleteObject(pProfile->hIcon);
			pProfile->hIcon  = NULL;
		}
		free(pProfile);
	}
	pSoundPorfileListHead = NULL;

	return TRUE;
}
/*********************************************************************\
* Function     PM_GetkeyLockStatus
* Purpose      
* Params      
* Return       
**********************************************************************/
BOOL PM_GetkeyLockStatus(void)
{
	return bKeyLock;
}
/*********************************************************************\
* Function     PM_GetPhoneLockStatus
* Purpose      
* Params      
* Return       
**********************************************************************/
BOOL PM_GetPhoneLockStatus(void)
{
	return bPhoneLock;
}
/*********************************************************************\
* Function     PM_SetkeyLockStatus
* Purpose      
* Params      
* Return       
**********************************************************************/
void PM_SetKeyLockStatus(BOOL bStatus)
{
	bKeyLock = bStatus;
	bKeyLockHide = !bStatus;
}
/*********************************************************************\
* Function     PM_SetPhoneLockStatus
* Purpose      
* Params      
* Return       
**********************************************************************/
void  PM_SetPhoneLockStatus(BOOL bStatus)
{
	bPhoneLock = bStatus;
	bPhoneLockHide = !bStatus;
}
/*********************************************************************\
* Function     PM_GetPhoneLockHideStatus
* Purpose      
* Params      
* Return       
**********************************************************************/
BOOL PM_GetPhoneLockHideStatus(void)
{
	return bPhoneLockHide;
}
/*********************************************************************\
* Function     PM_GetKeyLockHideStatus
* Purpose      
* Params      
* Return       
**********************************************************************/
BOOL PM_GetKeyLockHideStatus(void)
{
	return bKeyLockHide;
//	return 0;
}
/*********************************************************************\
* Function     PM_atoi
* Purpose      
* Params      
* Return       
**********************************************************************/
int PM_atoi(char* nptr)
{
	int c;              /* current char */
	long total;         /* current total */
	int sign;           /* if '-', then negative, otherwise positive */
	
	/* skip whitespace */
	while ((int)(unsigned char)*nptr == 0x20) 
		++nptr;
	
	c = (int)(unsigned char)*nptr++;
	sign = c;           /* save sign indication */
	if (c == '-' || c == '+')
		c = (int)(unsigned char)*nptr++;    /* skip sign */
	
	total = 0;
	
	while (PM_isdigit(c)) {
		total = 10 * total + (c - '0');     /* accumulate digit */
		c = (int)(unsigned char)*nptr++;    /* get next char */
	}
	
	if (sign == '-')
		return -total;
	else
		return total;   /* return result, negated if necessary */
	
}
/*********************************************************************\
* Function     PM_isdigit
* Purpose      
* Params      
* Return       
**********************************************************************/
BOOL PM_isdigit(int ch)
{
	if(ch >= '0' && ch <= '9')
		return TRUE;
	else
		return FALSE;
}
static BOOL bSupportStk;
static int i_STKInfo;
void SetSimStk(BOOL bStatus)
{
	printf("\r\n SetSimStk value = %d", bStatus);
	bSupportStk = bStatus;
}
BOOL GetSimStk(void)
{
	return bSupportStk;
}
int GetSTKResult(void)
{
	return i_STKInfo;
}
void SetSTKResult(int info)
{
	i_STKInfo = info;
	
}
/*********************************************************************\
* Function     f_ResetDelayPhoneLock
* Purpose      for auto phone lock (if press key delay phonelock timer)
* Params      
* Return       
**********************************************************************/
void f_ResetDelayPhoneLock()
{
	calliftype callstate = GetCallIf();
	
	if(PM_GetPhoneLockStatus())
		return;
	
	if(callstate == diallinkif || callstate == ringif || callstate == callingif)
		return ;
	else
	{
		if(g_nPhoneLockTimerId)
		{
			KillTimer(NULL,g_nPhoneLockTimerId);
			g_nPhoneLockTimerId = 0;
		}
	}

}
/*********************************************************************\
* Function     f_ResetDelayPhoneLockEx
* Purpose      for auto phone lock (if press key delay phonelock timer)
* Params      
* Return       
**********************************************************************/
void f_ResetDelayPhoneLockEx()
{
	char pAutoLockTime[10];
	int  nAutoLockTime;
	
	calliftype callstate = GetCallIf();
	
	if(PM_GetPhoneLockStatus())
		return;
	
	if(callstate == diallinkif || callstate == ringif || callstate == callingif)
		return ;
	else
	{
		pAutoLockTime[0] = 0;
		GetLockCodePeriod((char*)pAutoLockTime);
		if(pAutoLockTime[0] != 0)
		{
			if(strcmp(pAutoLockTime, "Off") ==0)
				nAutoLockTime = 0;
			else
			{
				pAutoLockTime[strlen(pAutoLockTime) -4] = 0;
				nAutoLockTime = PM_atoi(pAutoLockTime);
			}
			
		}
		
		if(nAutoLockTime != 0)
		{
			if(g_nPhoneLockTimerId != 0)
			{
				KillTimer(NULL, g_nPhoneLockTimerId);
				g_nPhoneLockTimerId = 0;
			}
			g_nPhoneLockTimerId = SetTimer(NULL, 0, 
			nAutoLockTime*60*1000, f_PhonelockTimerProc);
		}
	}

}
/*********************************************************************\
* Function     f_ResetDelayKeyLock
* Purpose      for auto key lock (if press key delay keylock timer)
* Params      
* Return       
**********************************************************************/
void f_ResetDelayKeyLock()
{
	if(g_nKeyLockTimerId)
	{
		KillTimer(NULL,g_nKeyLockTimerId);
		g_nKeyLockTimerId = 0;
	}
	return;
}
/*********************************************************************\
* Function     f_ResetDelayKeyLockEx
* Purpose      for auto key lock (if press key delay keylock timer)
* Params      
* Return       
**********************************************************************/
void f_ResetDelayKeyLockEx()
{
	calliftype callstate = GetCallIf();

	if(PM_GetPhoneLockStatus() || PM_GetkeyLockStatus())
	{
		if(g_nKeyLockTimerId)
		{
			KillTimer(NULL,g_nKeyLockTimerId);
			g_nKeyLockTimerId = 0;
		}
		return;
	}

	if(GetKeyLockMode() == KL_MANUAL)
	{
		if(g_nKeyLockTimerId)
		{
			KillTimer(NULL,g_nKeyLockTimerId);
			g_nKeyLockTimerId = 0;
		}
		return;
	}

	if(callstate == diallinkif || callstate == ringif || callstate == callingif)
	{
		if(g_nKeyLockTimerId)
		{
			KillTimer(NULL,g_nKeyLockTimerId);
			g_nKeyLockTimerId = 0;
		}
		return ;
	}
	else
	{
		if(IsIdleState() || IsMissedState())
		{
			if(g_nKeyLockTimerId)
			{
				KillTimer(NULL,g_nKeyLockTimerId);
				g_nKeyLockTimerId = 0;
			}
			g_nKeyLockTimerId = SetTimer(NULL, 0, 
				IDLE_TIMEOUT, f_KeylockTimerProc);
		}
		else
		{
			if(g_nKeyLockTimerId)
			{
				KillTimer(NULL,g_nKeyLockTimerId);
				g_nKeyLockTimerId = 0;
			}
			g_nKeyLockTimerId = SetTimer(NULL, 0, 
				APP_TIMEOUT, f_KeylockTimerProc);
		}
	}
}
/*********************************************************************\
* Function     f_KeylockTimerProc
* Purpose      
* Params      
* Return       
**********************************************************************/
static void CALLBACK f_KeylockTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,                                 DWORD dwTime)
{
	HWND hAvtWnd =  NULL;
	calliftype callstate = GetCallIf();

	hAvtWnd = GetActiveWindow();

	if(g_nKeyLockTimerId)
	{
		KillTimer(NULL, g_nKeyLockTimerId);
		g_nKeyLockTimerId = 0;
	}

	if(pAppMain->hIdleWnd == NULL)
		return;
	
	if(GetKeyLockMode() == KL_MANUAL)
		return;

	if(PM_GetPhoneLockStatus() || PM_GetkeyLockStatus())
		return;

	if(TopWndIsNotification())//if confirmwnd 
		return ;

	if(callstate == diallinkif || callstate == ringif || callstate == callingif)
		return;

	if(hAvtWnd)
	{
		SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);//remove menu
		CreateKeylockWnd(hAvtWnd);
	}
}
/*********************************************************************\
* Function     f_PhonelockTimerProc
* Purpose      
* Params      
* Return       
**********************************************************************/
static void CALLBACK f_PhonelockTimerProc(HWND hWnd, UINT uMsg, UINT idEvent,                                 DWORD dwTime)
{
	calliftype callstate = GetCallIf();
	BOOL  bHideKey = FALSE;		

	if(g_nPhoneLockTimerId)
	{
		KillTimer(NULL, g_nPhoneLockTimerId);
		g_nPhoneLockTimerId = 0;
	}
	
	if(PM_GetkeyLockStatus()|| PM_GetKeyLockHideStatus())
	{
		if(PM_GetkeyLockStatus())
		{
			bKeyLock = FALSE;
		}
		else
		{
			bKeyLockHide = FALSE;
			bHideKey = TRUE;
		}

		UnhookWindowsHookEx(prog_kbHook);
		
		DlmNotify(PS_KEYLOCK, ICON_CANCEL);	
		
		PostMessage(hKeyLockWnd, WM_CLOSE, NULL, NULL);
	}

	if(callstate == diallinkif || callstate == ringif || callstate == callingif)
		return;

	if(PM_GetPhoneLockStatus())
		return;

	hActiveWnd = GetActiveWindow(); 

	if(TopWndIsNotification())//tipswin not hide
		PostMessage(hActiveWnd, WM_CLOSE, 0, 0);

	SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);

	CreatePhonelockWnd(hActiveWnd);

	if(bHideKey)
		PM_SetPhoneLockStatus(FALSE);
}
/*********************************************************************\
* Function     CreateMissedEventsWnd
* Purpose      missed events view
* Params      
* Return       
**********************************************************************/
static HWND hMissedEventWnd;
static BOOL bPressMissList;
static int  nTimerIdMiss;
static void CallMsgMissedEvents(BYTE CallType);
extern int Mu_GetUnReadMsg(void);
extern int Mu_GetUnReadEmail(void);
extern void GetRecentMissedCall(int * pcount); 
/********************************************************************
* Function   pMissTimeWndPorc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void CALLBACK pMissTimeWndPorc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
	if(bPressMissList)
	{
		KillRTCTimer(NULL, nTimerIdMiss);
		PrioMan_EndCallMusic(PRIOMAN_PRIORITY_BEEPSOUND, TRUE);
		nTimerIdMiss = 0;
	}
	else
	{
		int value = 0;

		PrioMan_CallMusic(PRIOMAN_PRIORITY_BEEPSOUND, 1);

		DHI_GetBkLightVolume(&value);
		
		if(value  == 0)
		{
			DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
			DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 1);	
			DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);
		}
		else
		{
			KillTimer(NULL, nTimerIdMiss);
			nTimerIdMiss = 0;
			bPressMissList = 0;
		}
	
	}
}
BOOL CreateMissedEventsWnd(HWND hWnd)
{
	WNDCLASS wc;
	SCENEMODE scenemode;
	int iInterval[6] = {0,30, 60, 120, 300, 600};

	memset(&scenemode, 0, sizeof(SCENEMODE));

	if(!bShowIdle)//first init idle
		return FALSE;
	
	if (hMissedEventWnd)
	{
		SetFocus(hMissedEventsList);
		return FALSE;
	}
	
	wc.style         = 0;
	wc.lpfnWndProc   = MissedEventsWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground =GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "MissedEventsWindowClass";
	RegisterClass(&wc);
	

	GetSM(&scenemode, GetCurSceneMode());
	
	if(scenemode.mEvents.iEventVibra != 0)
	{
		nTimerIdMiss = SetRTCTimer(NULL, 0, 
			iInterval[scenemode.mEvents.iTimeInterval], pMissTimeWndPorc);
	}	
	
	bPressMissList = FALSE;
	
    hMissedEventWnd = CreateWindow("MissedEventsWindowClass",
        "",
        WS_VISIBLE| PWS_STATICBAR,
		0,	
		TITLEBAR_HEIGHT+CAPTION_HEIGHT,
		DESK_WIDTH,	
		DESK_HEIGHT- TITLEBAR_HEIGHT-CAPTION_HEIGHT, 
        hWnd,NULL,	NULL,	NULL);
	
    if(hMissedEventWnd == NULL)
    {
        return FALSE;
    }


    ShowWindow(hMissedEventWnd, SW_SHOW);
    UpdateWindow(hMissedEventWnd);
	
    return TRUE;
}

static LRESULT MissedEventsWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT      lResult  = TRUE;	
    HDC hdc;
	static HBRUSH hMissedBgBrush;

    switch(wMsgCmd) 
    {
    case WM_CREATE:
	
       	  SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
			  (LPARAM)ML("Cancel"));
		  
		  SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		  
		  CreateMissedEventsList(hWnd, &hMissedEventsList);

		  hMissedBgBrush = CreateSolidBrush(RGB(158,177,230));
		
    	break;
	case WM_SETFOCUS:
		SetFocus(hMissedEventsList);
		break;
		
	case WM_QUERYSYSEXIT:
	    return SYSEXIT_CONTINUE;

	case WM_CTLCOLORLISTBOX :
		
        SetBkColor((HDC)wParam, COLOR_TRANSBK);
        SetTextColor((HDC)wParam, GetSysColor(COLOR_WINDOWTEXT));

        return (LRESULT)(COLOR_MENU + 1);
			  
	case WM_SYSEXIT:
	    SendMessage(hWnd, WM_CLOSE, 0, 0);
	    break;

	case WM_PROG_MISSCHANGE:
		{
			SCENEMODE scenemode;
			int iInterval[6] = {0,30, 60, 120, 300, 600};
			
			memset(&scenemode, 0, sizeof(SCENEMODE));
			if(scenemode.mEvents.iEventVibra != 0)
			{
				if(nTimerIdMiss != 0)
				{
					KillRTCTimer(NULL, nTimerIdMiss);
					PrioMan_EndCallMusic(PRIOMAN_PRIORITY_BEEPSOUND, TRUE);
					nTimerIdMiss = 0;
				}
				bPressMissList = FALSE;
				nTimerIdMiss = SetRTCTimer(NULL, 0, 
					iInterval[scenemode.mEvents.iTimeInterval], pMissTimeWndPorc);
			}	
			
		}
		break;

	case PM_MISSEVENTCHANGE:
		{
			ChangeMissedDataList((BYTE)LOWORD(lParam), (BYTE)HIWORD(lParam));	
		}
		break;

    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
        {
			if(nTimerIdMiss)
			{
				KillRTCTimer(NULL, nTimerIdMiss);
				PrioMan_EndCallMusic(PRIOMAN_PRIORITY_BEEPSOUND, TRUE);
				nTimerIdMiss = 0;
			}
		}
        else
        {
            SetFocus(hMissedEventsList);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDB_EXIT, 0), 
				(LPARAM)ML("Cancel"));
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
			MISSEDDATA *p = NULL;
			int i;
			char cNum[6];
			HBRUSH   hOldBrush = NULL;
			RECT     rcTextCap, rcBg;
			int      nOldmode = 0;
			BOOL     ret = FALSE;
			HFONT    hFont = NULL, hOldFont = NULL;
			HDC hMemdc;
			
            hdc = BeginPaint(hWnd, &ps);

			hMemdc = CreateMemoryDC(176, 31);
			BitBlt(hMemdc, 0, 0, 176, 31, hdc, 0,0,SRCCOPY);

			hOldBrush = SelectObject(hMemdc, hMissedBgBrush);
			
			GetFontHandle(&hFont, LARGE_FONT);
			
			hOldFont = SelectObject(hMemdc, hFont);
			
			SetRect(&rcBg, 0,1, 176, 31);
			FillRect(hMemdc, &rcBg, hMissedBgBrush);
			
			nOldmode = GetBkMode(hMemdc);
					
			SetBkMode(hMemdc, TRANSPARENT);
			SetRect(&rcTextCap, 18,5, 142, 23);
			DrawText(hMemdc, ML("Missed"), -1, &rcTextCap, DT_CENTER);
			
			SetBkMode(hMemdc, nOldmode);
			SelectObject(hMemdc, hOldBrush);
			SelectObject(hMemdc, hOldFont);

			BitBlt(hdc, 0, 0, 176, 31,  hMemdc, 0, 0, SRCCOPY);
			
			DeleteDC(hMemdc);
			
			SendMessage(hMissedEventsList, LB_RESETCONTENT,0,0);  

			p = pMissedEnventsHead;
			
			for(p = pMissedEnventsHead, i =0;p != NULL;i++)
			{
				char str[20];
				
				cNum[0] = 0;
				str[0] = 0;
				
				sprintf(cNum, "(%d)", p->number);
				
				switch(p->nType)
				{
				case CALL_APP:
					sprintf(str, "%s %s", ML("Calls"), cNum); 
					break;
					
				case MSG_APP:
					sprintf(str, "%s %s", ML("Messages"), cNum); 
					break;
					
				case MAIL_APP:
					sprintf(str, "%s %s", ML("E-mails"), cNum);
					break;
				case PUSHMSG:
					sprintf(str, "%s %s", ML("Push message"), cNum);
					break;
				}
				
				SendMessage(hMissedEventsList,LB_ADDSTRING,i,(LPARAM)str);
				SendMessage(hMissedEventsList, LB_SETITEMDATA, i, (LPARAM)p->nType);
				p = p->pNext;
			}
			
			SendMessage(hMissedEventsList,LB_SETCURSEL,0,0);

			EndPaint(hWnd, NULL);
        }
        break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

    case WM_KEYDOWN:
		
		bPressMissList =TRUE;
		if(LOWORD(wParam) == VK_F10 )
		{
			if(!bKeyLock && !bPhoneLock)
			{
				bCancelMissed = TRUE;
				FreeMissedData();
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;
			}
		}
		else
			OnMissedEventsDown(hWnd,wMsgCmd, wParam, lParam);
			
        break;

    case WM_DESTROY:
      
		if(hMissedBgBrush)
		{
			DeleteObject(hMissedBgBrush);
			hMissedBgBrush = NULL;
		}
		hMissedEventWnd = NULL;
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
    }

    return lResult;
}
/*********************************************************************\
* Function     CreateMissedEventsList
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL CreateMissedEventsList(HWND hWnd, HWND* hList)
{
	RECT rect;
	char cNum[6];
	MISSEDDATA *p = NULL;
	int i;

	GetClientRect(hWnd, &rect);

	*hList = CreateWindow("LISTBOX","", 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP|WS_VSCROLL,
        0, 31 , DESK_WIDTH, rect.bottom -rect.top-31,
        hWnd, (HMENU)IDC_MISSEDEVENTS_LIST, NULL, NULL );
	
	if(*hList == NULL)
		return FALSE;

	SendMessage(*hList, LB_RESETCONTENT,0,0);  
	p = pMissedEnventsHead;
	    		
	for(p = pMissedEnventsHead, i =0;p != NULL;i++)
	{
		char str[20];

		cNum[0] = 0;
		str[0] = 0;
		
		sprintf(cNum, "(%d)", p->number);

		switch(p->nType)
		{
		case CALL_APP:
			sprintf(str, "%s %s", ML("Calls"), cNum); 
			break;

		case MSG_APP:
			sprintf(str, "%s %s", ML("Messages"), cNum); 
			break;

		case MAIL_APP:
			sprintf(str, "%s %s", ML("E-mails"), cNum);
			break;
		case PUSHMSG:
			sprintf(str, "%s %s", ML("Push message"), cNum);
		}

		SendMessage(*hList,LB_ADDSTRING,i,(LPARAM)str);
		SendMessage(*hList, LB_SETITEMDATA, i, (LPARAM)p->nType);
		p = p->pNext;
	}

	SendMessage(*hList,LB_SETCURSEL,0,0);

	return TRUE;
}
/*********************************************************************\
* Function     OnMissedEventsDown
* Purpose      
* Params      
* Return       
**********************************************************************/
static int  OnMissedEventsDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{

	int nFocusId = 0;
	BYTE nType = 0;

    switch(wParam)
    {
    case VK_F5:    
		{
			nFocusId = SendMessage(hMissedEventsList, LB_GETCURSEL, 0, 0);
			
			nType = (BYTE)SendMessage(hMissedEventsList, LB_GETITEMDATA, nFocusId, 0);
			
			CallMsgMissedEvents(nType);
            FreeMissedData();
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
        break;
   
	default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
	return 0;

}
/*********************************************************************\
* Function     InitMissedEventsNum
* Purpose      
* Params      
* Return       
**********************************************************************/
BOOL InitMissedEventsMemory()
{
	int iMissedCallNum = 0, iUnreadMsg = 0, iUnreadmail = 0, iUnreadPush = 0;

	GetRecentMissedCall(&iMissedCallNum);
	if(GetEnterInbox())
		iUnreadMsg = 0;
	else
		iUnreadMsg = Mu_GetUnReadMsg();
	if(GetEnterMail())
		iUnreadmail = 0;
	else
		iUnreadmail = Mu_GetUnReadEmail();
	if(GetEnterPush())
		iUnreadPush = 0;
	else
		iUnreadPush = WP_GetUnReadCount();

	if((iMissedCallNum != 0) ||(iUnreadMsg != 0) || (iUnreadmail != 0) ||
		(iUnreadPush != 0))
		return TRUE;
	else
		return FALSE;
}
BOOL InitMissedEventsNum()
{
	int iMissedCallNum = 0, iUnreadMsg = 0, iUnreadmail = 0, iUnreadPush = 0;

	FreeMissedData();

	pMissedEnventsHead = NULL;

	GetRecentMissedCall(&iMissedCallNum);
	InitMissedEventsNumEx(iMissedCallNum, CALL_APP);
	
	if(GetEnterInbox())
		InitMissedEventsNumEx(0, MSG_APP);
	else
		InitMissedEventsNumEx(Mu_GetUnReadMsg(), MSG_APP);
	
	if(GetEnterMail())
		InitMissedEventsNumEx(0, MAIL_APP);
	else
		InitMissedEventsNumEx(Mu_GetUnReadEmail(), MAIL_APP);

	if(GetEnterPush())
		InitMissedEventsNumEx(0, PUSHMSG);
	else
		InitMissedEventsNumEx(WP_GetUnReadCount(), PUSHMSG);

	if(pMissedEnventsHead == NULL)
		return FALSE;
	else
		return TRUE;
}
/*********************************************************************\
* Function     InitMissedEventsNumEx
* Purpose      
* Params      
* Return       
**********************************************************************/
static void InitMissedEventsNumEx(int number, BYTE nType)
{
	MISSEDDATA *pData = NULL;

	if(number <= 0)
		return;

	pData = malloc(sizeof(MISSEDDATA));
	memset(pData, 0, sizeof(MISSEDDATA));
	pData->number = number;
	pData->nType = nType;
	pData->pNext = NULL;
	
	if(pMissedEnventsHead == NULL)
		pMissedEnventsHead = pData;
	else
	{
		MISSEDDATA *p = NULL;
		p = pMissedEnventsHead;
		while(p->pNext)
		{
			p = p->pNext;
		}
		p->pNext = pData;
	}

}
/*********************************************************************\
* Function     ChangeMissedDataList
* Purpose      
* Params      
* Return       
**********************************************************************/
static void ChangeMissedDataList(BYTE nType, BYTE flag)
{
	MISSEDDATA *pData = NULL, *p = NULL;
	char cNum[6];
	int i = 0;

	if(flag == DEC_COUNT)
	{
		if(pMissedEnventsHead == NULL)
			return;

		p = pMissedEnventsHead;

		while(p)
		{
			if(p->nType == nType)
			{
				p->number--;
				if(p->number == 0)
				{
					pData = p;
					p = p->pNext;
					free(pData);
					pData = NULL;
				}
				break;
			}
		}

	}
	else if(flag == ADD_COUNT)
	{
		p = pMissedEnventsHead;
		
		while (p->pNext)
		{
			if(p->nType == nType)
			{
				p->number++;
				break;
			}
			p = p->pNext;
		}
		if(p->pNext == NULL)
		{
			pData = malloc(sizeof(MISSEDDATA));
			memset(pData, 0, sizeof(MISSEDDATA));
			pData->nType = nType;
			pData->number = 1;
			pData->pNext = NULL;
			p->pNext = pData;
		}
	}

	SendMessage(hMissedEventsList, LB_RESETCONTENT,0,0);
	p = pMissedEnventsHead;

	while(p)
	{
		char str[20];
		
		cNum[0] = 0;
		str[0] = 0;
		
		sprintf(cNum, "(%d)", p->number);
		
		switch(p->nType)
		{
		case CALL_APP:
			sprintf(str, "%s %s", ML("Calls"), cNum); 
			break;
			
		case MSG_APP:
			sprintf(str, "%s %s", ML("Messages"), cNum); 
			break;
			
		case MAIL_APP:
			sprintf(str, "%s %s", ML("E-mails"), cNum);
			break;
		case PUSHMSG:
			sprintf(str, "%s %s", ML("Push message"), cNum);
		}
		
		SendMessage(hMissedEventsList,LB_ADDSTRING,i,(LPARAM)str);
		SendMessage(hMissedEventsList, LB_SETITEMDATA, i, (LPARAM)p->nType);
		i++;
	}
	SendMessage(hMissedEventsList,LB_SETCURSEL,0,0);
}
/*********************************************************************\
* Function     FreeMissedData
* Purpose      
* Params      
* Return       
**********************************************************************/
static void FreeMissedData(void)
{
	MISSEDDATA *p = NULL, *pData = NULL;

	if(pMissedEnventsHead == NULL)
		return;

	p = pMissedEnventsHead;

	while (p)
	{
		pData = p;
		p = p->pNext;
		free(pData);
		pData = NULL;
	}

	pMissedEnventsHead = NULL;
}
/*********************************************************************\
* Function     IsMissedState
* Purpose      
* Params      
* Return       
**********************************************************************/
BOOL IsMissedState()
{
    if(hMissedEventWnd)
        return TRUE;

    return FALSE;
}
/*********************************************************************\
* Function     GetMissedEventWnd
* Purpose      
* Params      
* Return       
**********************************************************************/
HWND GetMissedEventWnd(void)
{
	if(IsMissedState())
		return hMissedEventWnd;
	else
		return NULL;
}
/*********************************************************************\
* Function     CallMsgMissedEvents
* Purpose      
* Params      
* Return       
**********************************************************************/
static BOOL bEnterInbox, bEnterPush, bEnterEmail;
BOOL GetEnterInbox(void)
{
	return bEnterInbox;
}
void SetEnterInbox(BOOL bSet)
{
	bEnterInbox = bSet;
}
BOOL GetEnterPush(void)
{
	return bEnterPush;
}
void SetEnterPush(BOOL bSet)
{
	bEnterPush = bSet;
}
BOOL GetEnterMail(void)
{
	return bEnterEmail;
}
void SetEnterMail(BOOL bSet)
{
	bEnterEmail = bSet;
}

static void CallMsgMissedEvents(BYTE CallType)
{
	char   AppName[PMNODE_NAMELEN + 1];

	AppName[0] = 0;

	switch(CallType)
	{
	case CALL_APP:
		strcpy(AppName, "Logs");
		CallAppEntryEx(AppName, CallType,UNANSWER_CALL);

		break;
	case MSG_APP:
		SetEnterInbox(TRUE);
	case MAIL_APP:
		SetEnterMail(TRUE);
		strcpy(AppName, "Messaging");
		CallAppEntryEx(AppName, CallType, 0);
		break;
	case PUSHMSG:
		SetEnterPush(TRUE);
		strcpy(AppName, "WAP");
		CallAppEntryEx(AppName, CallType, 0);
		break;
	}

}
/*********************************************************************\
* Function     PMView_Layout
* Purpose      
* Params      
* Return       
**********************************************************************/
#define ISNEWLINE(p)  \
		(*p == '\r' || *p == '\n' )

#define ISENTERNEWLINE(p) \
        (*p == '\r' && *(p+1) && *(p+1) == '\n')

typedef struct tagPMView_Layout
{
    char* pszFragment;
    int   nLen;
    struct tagPMView_Layout *pNext;
}PMVIEW_LAYOUT, *PPMVIEW_LAYOUT;

static PMVIEW_LAYOUT *pLayoutData;

static BOOL PMView_Layout(HWND hWnd, char *pTxt)
{

    char *p = NULL;

    int  len = 0,nFit = 0,nClientWidth = 0,i = 0;
	
	PMVIEW_LAYOUT *pLayout = NULL,*pTmpLayout;
	
	BOOL bBreak = FALSE; 
	
	HFONT hFontBig = NULL,hFontOld = NULL;
	
	HDC  hdc;
	
	char* pString = NULL;
	
	RECT rcClient;


	len = strlen(pTxt);

    if (len <= 0 || pTxt == NULL)
		
        return FALSE;
    
    p = pTxt;

    pLayout = (PMVIEW_LAYOUT*)malloc(sizeof(PMVIEW_LAYOUT));

    if(pLayout == NULL)// need to free memory
        return FALSE;

    memset(pLayout,0,sizeof(PMVIEW_LAYOUT));

	pLayoutData = pLayout;
	
    pLayout->pszFragment = pTxt;

    while(*p)
    {

        if(ISENTERNEWLINE(p))
        {
            pTmpLayout = pLayout;

            if(pTmpLayout->nLen == 0)
                pTmpLayout->pszFragment = NULL;

             pTmpLayout->pNext = (PMVIEW_LAYOUT*)malloc(sizeof(PMVIEW_LAYOUT));

            if(pTmpLayout->pNext == NULL)// need to free memory
                return FALSE;

            memset(pTmpLayout->pNext,0,sizeof(PMVIEW_LAYOUT));

            pLayout = pTmpLayout->pNext;

            pLayout->pszFragment = p+2;

            p += 2;

        }

        else if(ISNEWLINE(p))
        {

            pTmpLayout = pLayout;

            if(pTmpLayout->nLen == 0)
                pTmpLayout->pszFragment = NULL;

            pTmpLayout->pNext = (PMVIEW_LAYOUT*)malloc(sizeof(PMVIEW_LAYOUT));

            if(pTmpLayout->pNext == NULL)// need to free memory
                return FALSE;

            memset(pTmpLayout->pNext,0,sizeof(PMVIEW_LAYOUT));

            pLayout = pTmpLayout->pNext;

            pLayout->pszFragment = p+1;

            p++;

        }
        else 
        {
            pLayout->nLen++;
            p++;
        }
    }
	
	pLayout = pLayoutData;

	GetClientRect(hWnd,&rcClient);
	
    nClientWidth = rcClient.right - rcClient.left;

	
	hdc = GetDC(hWnd);
    while(pLayout != NULL)
    {
		
        pString = pLayout->pszFragment;
		
        len = pLayout->nLen;
		
        while(len > 0)
        {
			
            GetTextExtentExPoint(hdc, pString, len, nClientWidth, &nFit, NULL, NULL);
			
            if(nFit >= len)
                break;
			
            pTmpLayout = (PMVIEW_LAYOUT*)malloc(sizeof(PMVIEW_LAYOUT));
			
            if(pTmpLayout == NULL)// need to free memory
                return FALSE;
			
            memset(pTmpLayout,0,sizeof(PMVIEW_LAYOUT));
			
            for(i=nFit-1 ; i >0 ; i--)
            {
				
                bBreak = FALSE;
				
                if(pString[i] == ' ')
                {
					
                    len -= (i+1);
					
                    pString += (i+1);
					
                    pLayout->nLen = (i+1);
					
                    pTmpLayout->pNext = pLayout->pNext; 
					
                    pLayout->pNext = pTmpLayout; 
					
                    pTmpLayout->pszFragment = pString;
					
                    pTmpLayout->nLen = len;
					
                    pLayout = pTmpLayout;
					
                    bBreak = TRUE;
					
                    break;
                }
            }
			
            if(bBreak == FALSE)
            {
				
                len -= nFit;
				
                pString += nFit;
				
                pLayout->nLen = nFit;
				
                pTmpLayout->pNext = pLayout->pNext; 
				
                pLayout->pNext = pTmpLayout; 
				
                pTmpLayout->pszFragment = pString;
				
                pTmpLayout->nLen = len;
				
                pLayout = pTmpLayout;
				
                continue;
            }
			
        }        
		
        pLayout = pLayout->pNext;
		
    }
	ReleaseDC(hWnd,hdc);
    return TRUE;
}
/*********************************************************************\
* Function     PMDrawIdleModeTxt
* Purpose      
* Params      
* Return       
**********************************************************************/
static void PMDrawIdleModeTxt(HWND hWnd, HDC hdc, BOOL bCellIdUp)
{

	int  nLine, i = 0, iMax = MAX_IDLETXTLEN;
	PMVIEW_LAYOUT *pLayout = NULL;
	RECT  rIdleTxt[] =
	{
		{ 0, 20, 176, 36},
		{0, 38, 176, 54},
		{0, 56, 176, 72},
		{0, 74, 176, 90},
		{0, 92, 176, 108},
	};

	pLayout = pLayoutData;

	nLine = 0;

	if(bCellIdUp)
		iMax = MAX_IDLETXTLEN - 1;

	if(nIdleTxtLine <= iMax+ iIdleTxtPos)
	{
		iIdleTxtPos = 0;
		pLayout = pLayoutData;
	}

	while(pLayout && iIdleTxtPos > nLine)
	{
		pLayout = pLayout->pNext;
		nLine++;
	}
	
  	while(pLayout && i< iMax)
	{
		if(pLayout->nLen && pLayout->pszFragment)
			DrawText(hdc,pLayout->pszFragment,pLayout->nLen, &rIdleTxt[i],DT_VCENTER|DT_LEFT);
		
		pLayout = pLayout->pNext;  
		i++;
	}

}
/*********************************************************************\
* Function     PMDrawIdleCellId
* Purpose      
* Params      
* Return       
**********************************************************************/
static void PMDrawIdleCellId(HWND hWnd, HDC hdc)
{

	int  i = 0, iMax = 2, nLen = 0;
	int  nLineNum = 0;
	char *p = ProgDeskIdle.strCellId;

	RECT  rIdleCellId[] =
	{
		{ 0, 98, 176, 113},
		{0, 113, 176, 128}
	};

	nLen = strlen(ProgDeskIdle.strCellId);

	GetTextExtentExPoint(hdc, ProgDeskIdle.strCellId, nLen,	DESK_WIDTH, &nLineNum, NULL, NULL);
		
  	while(nLen > 0 && i< iMax)
	{
		if(nLen < nLineNum)
			DrawText(hdc, p, nLen, &rIdleCellId[i],DT_VCENTER|DT_LEFT);
		else
			DrawText(hdc, p, nLineNum, &rIdleCellId[i],DT_VCENTER|DT_LEFT);

		i++;
		
		if(nLen < nLineNum)
		{
			nLen = 0;
		}
		else
		{
			nLen -= nLineNum;
			p += nLineNum;
		}
	}

}
/*********************************************************************\
* Function     PMView_GetLine
* Purpose      
* Params      
* Return       
**********************************************************************/
static int PMView_GetLine(void)
{
    PMVIEW_LAYOUT *pLayout;
    int nLine = 0;
	
	pLayout = pLayoutData;

    while(pLayout != NULL)
    {
        nLine++;
        pLayout = pLayout->pNext;
    }
    return nLine;
}
/********************************************************************
* Function   PMHideIdle  
* Purpose    show idle view(for call app)
* Params     
* Return     
* Remarks      
**********************************************************************/
void PMHideIdle(void)
{
	HWND hCurWnd = GetActiveWindow();
	
	if(pAppMain->hIdleWnd)
	{
		if(IsWindowVisible(pAppMain->hIdleWnd))//show or hide
		{
			if(hCurWnd != NULL && hCurWnd != pAppMain->hIdleWnd)
			{
				if(hCurWnd == pAppMain->hQuickMenuhWnd/* ||
					hCurWnd == hMissedEventWnd*/)
				{
					SendMessage(hCurWnd, WM_CLOSE, 0, 0);
					ShowWindow(pAppMain->hIdleWnd, SW_HIDE);
				}
			}
			else
				ShowWindow(pAppMain->hIdleWnd, SW_HIDE);
		}
	}

}
/********************************************************************
* Function   GetBatValue  
* Purpose    for call app to display battery info
* Params     
* Return     
* Remarks      
**********************************************************************/
static int bVoltValue = 0;
int GetBatValue(void)
{
	return bVoltValue;
}
/********************************************************************
* Function   SetBatValue  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static  void    SetBatValue(int BCValue)
{
	bVoltValue = BCValue;
}
/********************************************************************
* Function   GetQuickDialWnd  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
HWND GetQuickDialWnd(void)
{
	return hQuickDial;
}

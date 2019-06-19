           /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman
 *
 * Purpose  : implement intercept special key
 *            
\**************************************************************************/

#include "window.h"
#include "me_wnd.h"
#include "plx_pdaex.h"
#include "setting.h"
#include "progman.h"
#include "APPFile.h"
//#include "plxmm.h"
#include "pmi.h"
#include "phonebookext.h"
#include "callpub.h"
#include "MBRecordList.h"
#include "RcCommon.h"
#include "pubapp.h"
#include "setup.h"
#include "mullang.h"
#include "mbpublic.h"

#define  MAX_IDLEKEYNUM   6 //idle handle key number
#define  MAX_KEYNUM       25 //all key number
#define  nTimeID_HOOK     300

static	HHOOK			prog_kbHook;  //registe idle and call hook
static  HHOOK			prog_sysHook;//registe system hook (delay keylock and phone lock timer)

static  PKEYEVENTDATA	pCurKeydata =NULL;//hot key data
static  WORD            v_HotKey;
static  WORD            v_nRepeats;	
static  BOOL            bEmergyCall; 
static	BOOL			bEmerge = FALSE;//enter emergency call or not
static	BOOL			bLock = FALSE; //lock or not
static  int             nTimeId_hook;
RECT    g_rcSound = {0,0,0,0};

static	LRESULT	CALLBACK Prog_KeyBoardProc(int , WPARAM , LPARAM );
static  LRESULT CALLBACK Prog_SystemProc(int ,WPARAM, LPARAM);

BOOL            IsIdleState();
BOOL            IsMissedSate();
BOOL            PM_GetkeyLockStatus(void);
BOOL            PM_GetPhoneLockStatus(void);
BOOL			PM_GetPhoneLockHideStatus(void);
BOOL			PM_GetKeyLockHideStatus(void);

static WORD		PM_GetHotKeyType(WPARAM wParam, LPARAM lParam);
static BOOL		IsCurWndApp(void); //curren app exit
	   BOOL		CallAppEntry(BYTE CallType);
static BOOL     MenuKeyProc(WORD vkType, LPARAM lParam);
static BOOL     EndKeyProc(int nCode, WPARAM wParam, LPARAM lParam);
static void     PowerKeyProc(void);
static BOOL		CallLevelAdjuster(HWND hcallWnd, int nlevel);

extern	BOOL    PlayPowerOff(BOOL bPlay);
extern  void    f_CheckPowerOff(void);
extern	void	f_ReSetDelayStart( WORD nIndex );
extern	BOOL	AppCurFileHide(struct appMain * pAdm);
extern PCURAPPLIST *AppFileGetListHead( struct appMain * pAdm );
    
extern  BOOL		  CreateAppLaunch(struct appMain * pAdm, HWND hWnd);
extern PKEYEVENTDATA  GetKeyTypeByCode(long vkCode);
extern int GetHeadSetVolum(void);

//call app functions
extern  calliftype GetCallIf();

extern	void	f_ResetDelayKeyLock();
extern	void	f_ResetDelayPhoneLock();
extern	void	f_ResetDelayKeyLockEx();
extern	void	f_ResetDelayPhoneLockEx();
extern  HWND	GetMissedEventWnd(void);
extern  HWND	GetQuickDialWnd(void);
extern  void	ReadRingVolume(unsigned long * ivalue);
extern  BOOL	CreateMissedEventsWnd(HWND hWnd);
extern  BOOL	InitMissedEventsNum();
extern	BOOL	Clock_IsAlarming(void);//clock alarm, not hide 
extern  BOOL	CALE_IsAlarmWnd(void);//calendar alarm ,not hide
extern  HWND	GetElectPowerOffWnd(void);
extern  void	PowerOff();
extern  BOOL	IfStkMainMenu(void);
extern HWND		GetSoundPorfileWnd(void);
static	HWND	hAppWnd;
/********************************************************************
* Function   Prog_InitHook  
* Purpose    init hook(system and idle)
* Params     
* Return     
* Remarks      
**********************************************************************/
void Prog_InitHook(void)
{
	prog_sysHook = SetWindowsHookEx(WH_KEYBOARD|0x8000, Prog_SystemProc, 0, 0);
    prog_kbHook = SetWindowsHookEx(WH_KEYBOARD, Prog_KeyBoardProc, 0, 0);
}
/********************************************************************
* Function   Prog_SystemProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static WORD nLockKeycode;
extern void SetHeadSetStatus(BOOL bAttach);
void PostRingMessage(void);
extern void f_SetBkLight(BOOL bEnable);
extern BOOL PM_IsPowerOff(void);
extern BOOL IsAlarmPowerOn(void);
extern BOOL MobileInit2(void);
extern BOOL Sett_BritAdjusting(LEVELVALUE level);
extern void SetTipsLowPower(BOOL bSet);
extern BOOL GetTipsLowPower(void);
static void CALLBACK pKeyTimerFunc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime);
#define BKLIGHT_TIMEOUT   15
#define KEYLIGHT_TIMEOUT  10
static	LRESULT	CALLBACK Prog_SystemProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	WORD nKeyCode;
	int handle = -1, i;
	long vkParam[] = 
    {
        VK_F9, VK_F2,VK_RETURN,VK_F5,VK_F3,VK_F4,VK_F1,
		VK_0, VK_1, VK_2, VK_3, VK_4, VK_5,VK_6, 
		VK_7, VK_8, VK_9, VK_F6, VK_F7,VK_F8,
		VK_LEFT,VK_RIGHT, VK_UP, VK_DOWN,VK_F10
    };

	PLXPrintf("\r\n input nCode = %d, nKeycode = %x, lparam = %x, wparam = %x", nCode, LOWORD(wParam),
		lParam, wParam);

	if(nCode <0 )
		return CallNextHookEx(prog_sysHook, nCode, wParam, lParam);

	if(PM_IsPowerOff())
		return CallNextHookEx(prog_sysHook, nCode, wParam, lParam);

	nKeyCode = LOWORD(wParam);
	nLockKeycode =nKeyCode;
	
	if(nKeyCode == VK_RINGIN)
	{
		printf("\r\nPM recieve VK_RINGIN message!");
		PostRingMessage();
		return TRUE;
	}

	if(nKeyCode == VK_WAKEUP_RINGIN)
		return TRUE;

	if(nKeyCode == VK_WAKEUP_KEY|| nKeyCode == VK_WAKEUP_POWERKEY|| nKeyCode == VK_WAKEUP_EMERGENCYKEY
		|| nKeyCode == VK_WAKEUP_EXTERNAL)
	{
		printf("\r\n recieve vk_wakeup !");
		f_sleep_register(KEYEVENTS);

#define TIMER_ID_FRESH 2
		SendMessage(pAppMain->hIdleWnd,WM_TIMER,TIMER_ID_FRESH, 0);
		SendMessage(pAppMain->hDeskWnd, WM_TIMER, 3, 0);
		if(DHI_ReadHeadSetStatus() > 0)			// check headset ( earphone )
		{
			DlmNotify(PS_SETHEADSET, ICON_SET);		// set headset icon
			SetHeadSetStatus(TRUE);
		}
		else
		{
			DlmNotify(PS_SETHEADSET, ICON_CANCEL);
			SetHeadSetStatus(FALSE);
		}
		return TRUE;

	}

	if(nKeyCode == VK_EARPHONE_IN || nKeyCode == VK_EARPHONE_OUT|| nKeyCode == VK_USB_IN 
		|| nKeyCode == VK_USB_OUT || nKeyCode == VK_CHARGER_IN || nKeyCode == VK_CHARGER_OUT
		|| nKeyCode == VK_LOWPOWER)
	{

		f_sleep_register(KEYEVENTS);
		
		DlmNotify(PMM_NEWS_ENABLE, 	PMF_MAIN_BKLIGHT);
		DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), BKLIGHT_TIMEOUT);
		DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT);
		if(nTimeId_hook)
		{
			KillTimer(NULL, nTimeId_hook);
			nTimeId_hook = 0;
		}
		nTimeId_hook = SetTimer(NULL, nTimeID_HOOK, 5000, pKeyTimerFunc);

		if(nKeyCode == VK_LOWPOWER)
		{
			if (!PM_IsPowerOff() /*&& GetTipsLowPower()*/)
            {
				PLXTipsWinOem(NULL, NULL, NULL, ML("Battery voltage low,\r\nPlease charging now..."), ML("Battery"),Notify_Info,
					NULL,NULL, WAITTIMEOUT );
                SetRTCTimer(pAppMain->hDeskWnd, 1, 60 * 1000, NULL);
                SetTipsLowPower(FALSE);
            }	
			return TRUE;
		}
		else
			return CallNextHookEx(prog_sysHook, nCode, wParam, lParam);
	}

	for(i=0; i < sizeof(vkParam)/sizeof(long); i++)
	{
		if(vkParam[i] == LOWORD(wParam))
		{
			if(lParam & 0xc0000000)
				;
			else
				PrioMan_CallMusic(PRIOMAN_PRIORITY_KEYBOARD, 1);//call keypress sound
			break;
		}
	}

	if(lParam & 0xc0000000)
	{
		printf("\r\n enter keyup!");
		if(nTimeId_hook)
		{
			KillTimer(NULL, nTimeId_hook);
			nTimeId_hook = 0;
		}
		nTimeId_hook = 	SetTimer(NULL, nTimeID_HOOK, 5000, pKeyTimerFunc);
		DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), BKLIGHT_TIMEOUT);	
		DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_KEYLIGHT), KEYLIGHT_TIMEOUT);
		DlmNotify (PMM_NEWS_ENABLE,PMF_AUTO_SHUT_MAIN_LIGHT|PMF_AUTO_SHUT_KEYLIGHT);
	
		f_ResetDelayPhoneLockEx();
		f_ResetDelayKeyLockEx();
	}
	else
	{
		printf("\r\n enter keydown!");
		f_sleep_register(KEYEVENTS);
		DlmNotify(PMM_NEWS_ENABLE, 	PMF_KEYLIGHT|PMF_MAIN_BKLIGHT);
		DlmNotify (PMM_NEWS_DISABLE,PMF_AUTO_SHUT_KEYLIGHT|PMF_AUTO_SHUT_MAIN_LIGHT);
		
		f_ResetDelayPhoneLock();
		f_ResetDelayKeyLock();
	}

	return CallNextHookEx(prog_sysHook, nCode, wParam, lParam);

}
WORD GetLockKeyCode(void)
{
	return nLockKeycode;
}
void SetLockKeyCode(WORD nKeyCode)
{
	nLockKeycode = nKeyCode;
}
/********************************************************************
* Function   pKeyTimerFunc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void CALLBACK pKeyTimerFunc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
	f_sleep_unregister(KEYEVENTS);
	if(nTimeId_hook)
	{
		KillTimer(NULL, nTimeId_hook);
		nTimeId_hook = 0;
	}
}
/********************************************************************
* Function   f_TimerProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static void CALLBACK f_TimerProc(HWND hWnd, UINT uMsg, UINT idEvent,
                                 DWORD dwTime)
{
	if(pCurKeydata->nTimerId != 0)
	{
		KillTimer(hWnd, idEvent);
		if(pCurKeydata->nType & PM_LONGKEY)
		{
			switch(pCurKeydata->nkeyCode)
			{
			case VK_F9://menu key
				MenuKeyProc(PM_LONGKEY, pCurKeydata->nkeyCode);
				break;
			case VK_F6://power key
				PowerKeyProc();
				break;
			}

		}
	}
	idEvent = 0;
	pCurKeydata->nTimerId = 0;
}

/********************************************************************
* Function   Prog_KeyBoardProc  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
extern BOOL CallAppEntryEx(PSTR pAppName, WPARAM wparam, LPARAM lparam);
extern BOOL IsAlarmPowerOn(void);
extern BOOL PdaSvr_Open(BYTE connect_type);
extern void PdaSvr_Close(void);
extern BOOL DHI_ChangeAudioRoute(void);
void SetHeadSetVolum(int value);
static	LRESULT	CALLBACK Prog_KeyBoardProc(int nCode, WPARAM wParam, LPARAM lParam)
{	
	WORD vkType = 0;
	WORD nKeyCode = LOWORD(wParam);
	static WORD nPreKeyCode ;
	int i =0;

    long vkParam[] = 
    {
        VK_F9, VK_F2,VK_F1,
		VK_F6, VK_F7,VK_F8,0
    };

	printf("\r\n keycode = %d", nKeyCode);
    if (nCode < 0 )
        return CallNextHookEx(prog_kbHook, nCode, wParam, lParam); 
	
	if(PM_IsPowerOff())
		return CallNextHookEx(prog_kbHook, nCode, wParam, lParam); 

	if(IsAlarmPowerOn())
	{
		if(nKeyCode == VK_F6 || nKeyCode == VK_RETURN || nKeyCode == VK_F10)
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
//		else
//			return TRUE;
	}

	if(LOWORD(wParam) == VK_F9 && pAppMain->hLaunchWnd)//app swapper
		return CallNextHookEx(prog_kbHook, nCode, wParam, lParam); 

	switch(nKeyCode)
	{
	case VK_EARPHONE_OUT: 
		printf("\r\n earphone out!");
		DlmNotify(PS_SETHEADSET, ICON_CANCEL);
		SetHeadSetStatus(FALSE);
		ME_SelectAudioMode(pAppMain->hDeskWnd, WM_AUDIO_HANDSET, ME_AUDIO_HANDSET);
		ME_SelectAudioModeEx(pAppMain->hDeskWnd, WM_AUDIO_HANDSET, ME_AUDIO_HANDSET);
		DHI_ChangeAudioRoute();
		return TRUE;
		
	case VK_EARPHONE_IN:  			
		printf("\r\n earphone in!");
		DlmNotify(PS_SETHEADSET, ICON_SET);
		SetHeadSetStatus(TRUE);
		ME_SelectAudioMode(pAppMain->hDeskWnd,WM_AUDIO_HEADSET, ME_AUDIO_HEADSET );
		ME_SelectAudioModeEx(pAppMain->hDeskWnd,WM_AUDIO_HEADSET, ME_AUDIO_HEADSET );
		DHI_ChangeAudioRoute();
		return TRUE;

	case VK_CHARGER_IN: //plug into charger
		printf("\r\n charger attatch!");
		DlmNotify(PS_SETBAT, 1);
		DHI_SetChargerStatus(TRUE);
		return TRUE;
		
	case VK_CHARGER_OUT:
		{
			printf("\r\n charger out!");
			if(GetElectPowerOffWnd() != NULL)
			{
				PostMessage(GetElectPowerOffWnd(), WM_CLOSE, 0, 0);
				PowerOff();
				return TRUE;
			}
		}
		DlmNotify(PS_SETBAT, 0);// plug off charger
		f_CheckPowerOff();
		DHI_SetChargerStatus(FALSE);
		return TRUE;	
   
	case VK_USB_IN:
		{
			if(IsAlarmPowerOn())
			{
				MobileInit2();
			}
			CallAppEntryEx("UsbModem", 0, USB_IN);
			PdaSvr_Open(1);//TFTP_CONNECT_USB
		}
		
		return TRUE;
		
	case VK_USB_OUT:
		CallAppEntryEx("UsbModem", 0, USB_OUT);
		PdaSvr_Close();
		return TRUE;
	default:
		break;
	}
	
	for(i=0; i < MAX_IDLEKEYNUM; i++)
	{
		if(vkParam[i] == LOWORD(wParam))
		{
			// Call_PlayMusic(RING_KEY,1);//call keypress sound
		    break;
		}
	}
	
	if(i == MAX_IDLEKEYNUM )
	{
		if(LOWORD(wParam) ==VK_EARPHONE_OUT || LOWORD(wParam) == VK_EARPHONE_IN
			|| LOWORD(wParam) ==VK_CHARGER_IN || LOWORD(wParam) == VK_CHARGER_OUT 
			|| LOWORD(wParam) == VK_USB_IN || LOWORD(wParam) == VK_USB_OUT)
			;
		else
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
	}

	if ((PM_GetkeyLockStatus() || PM_GetPhoneLockStatus()) 
		|| PM_GetPhoneLockHideStatus() || PM_GetKeyLockHideStatus())
	{
		if(LOWORD(wParam) == VK_F6)//power off
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
		else
		{
			if(PM_GetkeyLockStatus() && !PM_GetKeyLockHideStatus())
				;//PLXTipsWin(NULL, NULL, NULL,ML("Keypad locked"), NULL,Notify_Info, NULL, ML("Unlock"),WAITTIMEOUT);
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
		}
	}
	
	vkType = PM_GetHotKeyType(wParam, lParam);
	
	if(lParam & 0xc0000000) //keyup
	{
		if(nPreKeyCode == 0)//only get keyup
		{
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
		}
		if(nPreKeyCode != LOWORD(wParam))//keyup and keydown not match
		{
			return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
		}
		else
			nPreKeyCode = 0;
	}
	else
	{
		nPreKeyCode = LOWORD(wParam);
	}
	
	f_ReSetDelayStart (PMS_DELAY_SHUT_KEYLIGHT);
	f_ReSetDelayStart (PMS_DELAY_SHUT_LIGHT_MAIN);
	
	if(!IfStkMainMenu())
		return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
	
	switch(nKeyCode)
	{
	case VK_F9://application lauch;
		return MenuKeyProc(vkType, nKeyCode);
		break;
		
	case VK_F2://return idledesk,don't close app window
		return EndKeyProc(nCode, wParam, lParam);
		break;
		   
   case VK_F6://Power key
//	   EndObjectDebug();
	   if(IsAlarmPowerOn())
		   return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
	   return TRUE;
   case VK_F7:
	   {
		   unsigned long value = 0;
		   calliftype callstate = GetCallIf();
		   value = GetHeadSetVolum();
		   PLXPrintf("\r\n hAppWnd = %d", hAppWnd);

		   if(TopWndIsNotification())//tipswin not hide
			   return TRUE;
		   
		   if(lParam & 0xc0000000) //keyup
		   {
			   return TRUE;
		   }
		   else
		   {
			   if(callstate == callingif||callstate == diallinkif || callstate == ringif ||callstate == callendif)
				   return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
			   if(hAppWnd)
				   return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
			   else
			   {
				   CallLevelAdjuster(pAppMain->hIdleWnd,value);
				   return TRUE;
			   }	   
		   }
		  
	   }
	   break;

   case VK_F8:
	   {
		   unsigned long value = 0;
		   calliftype callstate = GetCallIf();
		   
		   value = GetHeadSetVolum();
		   PLXPrintf("\r\n hAppWnd = %d", hAppWnd);
		   
		   if(TopWndIsNotification())//tipswin not hide
			   return TRUE;

		   if(lParam & 0xc0000000) //keyup
		   {
			   return TRUE;
		   }
		   else
		   {
			   if(callstate == callingif||callstate == diallinkif || callstate == ringif ||callstate == callendif)
				   return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
			   if(hAppWnd)
				   return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
			   else
			   {
				   CallLevelAdjuster(pAppMain->hIdleWnd,value);
				   return TRUE;
			   }

		   }
		   
	   }
	   break;

   default: 
	   break;
    }
    return CallNextHookEx(prog_kbHook, nCode, wParam, lParam);
}
/********************************************************************
* Function   IsIdleState  
* Purpose    provide for mbdial hook to get whether the sate is in idle
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL IsIdleState()
{
    if(pAppMain == NULL)
        return FALSE;

    if(pAppMain->hIdleWnd == NULL)
        return FALSE;

    if(GetFocus() == pAppMain->hIdleWnd)
        return TRUE;

    return FALSE;
}
/********************************************************************
* Function   IsCurWndApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL IsCurWndApp(void)
{
	HWND hActWnd = GetActiveWindow();

	if(!hActWnd)
		return FALSE;

	if(hActWnd == pAppMain->hAppWnd || hActWnd == pAppMain->hGrpWnd 
		|| hActWnd == pAppMain->hIdleWnd || hActWnd == pAppMain->hLaunchWnd
	     || hActWnd == GetMissedEventWnd()||hActWnd == GetQuickDialWnd()
		||(pAppMain->hQuickMenuhWnd != 0)|| hActWnd == GetSoundPorfileWnd()/*|| hActWnd == hAppWnd*/) //not include phone lock and keylock view
		return FALSE;
	
	return TRUE;
}
/********************************************************************
* Function   CallAppEntry  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL CallAppEntryEx(PSTR pAppName, WPARAM wparam, LPARAM lparam);
BOOL CallAppEntry(BYTE CallType)
{
	return CallAppEntryEx("Logs",NULL, CallType );
}
/********************************************************************
* Function   GetEmergyCallStatus  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL GetEmergyCallStatus(void)
{
	return bEmergyCall;
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
		}
		
		if (pCurKeydata->nType & PM_SHORTKEY)
		{
			dwTickNow = GetTickCount();
			PLXPrintf("\r\n dwTickNow = %x", dwTickNow);

			nInterval = dwTickNow - pCurKeydata->dKicktime;
			if (nInterval < 0)
				nInterval = -nInterval;
			PLXPrintf("\r\n interval = %d", nInterval);
			
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
			pCurKeydata->nTimerId = SetTimer(NULL, 1,  ET_LONG, f_TimerProc);

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
			PLXPrintf("\r\n CurKey kick time = %x", dwTickNow);
			
			if ( pCurKeydata->nType & PM_MULTITAPKEY)
			{
				if (pCurKeydata->nTimerId != 0)
				{
					KillTimer(NULL, pCurKeydata->nTimerId);
				}
				pCurKeydata->nTimerId = SetTimer(NULL, 2, ET_MULTITAP,
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
/********************************************************************
* Function   MenuKeyProc  
* Purpose    handle menu key(VK_F9)
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL bPressEndKey;
BOOL IfPressEndKey(void)
{
	return bPressEndKey;
}
void SetPressEndKey(BOOL bEnable)
{
	bPressEndKey = bEnable;
}
static BOOL MenuKeyProc(WORD vkType, LPARAM lParam)
{
	calliftype callstate = GetCallIf();
	   
	if(callstate == diallinkif || callstate == ringif ||callstate == callendif/*|| callstate == callingif*/)
		return TRUE;

	if(Clock_IsAlarming() || CALE_IsAlarmWnd())//clock and calendar alarm, not hide 
		return TRUE;
	
	if(TopWndIsNotification())
		return TRUE;

	if(vkType == PM_LONGKEY)
	{
		HWND hCurWnd = NULL;
	
		hCurWnd = GetActiveWindow();
		
		if(IsCurWndApp())
		{
			if(hCurWnd == hAppWnd)
			{
				SendMessage(hCurWnd, WM_CLOSE, 0, 0);
				if(GetMissedEventWnd())
					SendMessage(GetMissedEventWnd(), WM_CLOSE, 0, 0);
				if(GetQuickDialWnd())
					SendMessage(GetQuickDialWnd(), WM_CLOSE, 0, 0);
				if(GetSoundPorfileWnd())
					SendMessage(GetSoundPorfileWnd(), WM_CLOSE, 0, 0);
				if(pAppMain->hLaunchWnd)
					SendMessage(pAppMain->hLaunchWnd, WM_CLOSE, 0, 0);
			}
			SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
			AppCurFileHide(pAppMain);//inactive cur app
		}
		else
		{
			if(hCurWnd != NULL)
			{	
				if(hCurWnd == GetMissedEventWnd() || hCurWnd == hAppWnd 
					|| hCurWnd == GetQuickDialWnd() || hCurWnd == GetSoundPorfileWnd())
					SendMessage(hCurWnd, WM_CLOSE, 0, 0);
				if(pAppMain->hQuickMenuhWnd != 0)
					SendMessage(pAppMain->hQuickMenuhWnd, WM_CLOSE, 0, 0);
			}
		}
		CreateAppLaunch(pAppMain, GetActiveWindow());

		return TRUE;
		
	}
	else if(vkType == PM_SHORTKEY)
	{
		PLXPrintf("press f2 key!\r\n");
		PLXPrintf("\r\nlowerparam =%d, highlparam = %d ", LOWORD(lParam), HIWORD(lParam));
		
		if(pAppMain->hGrpWnd)
		{
			if(IsWindowVisible(pAppMain->hGrpWnd))
			{
				if( GetActiveWindow() == pAppMain->hGrpWnd)
				{
					ShowWindow(pAppMain->hGrpWnd, SW_HIDE);
					
					if(IsWindowVisible(pAppMain->hIdleWnd))
					{
						BringWindowToTop(pAppMain->hIdleWnd);
					}
					else
					{
						ShowWindow(pAppMain->hIdleWnd, SW_SHOW);
					}
				}
				
				else
				{
					HWND hCurWnd = NULL;
					hCurWnd = GetActiveWindow();
					
					if(IsCurWndApp())
					{
						if(hCurWnd == hAppWnd)
						{
							SendMessage(hCurWnd, WM_CLOSE, 0, 0);
							if(GetMissedEventWnd())
								SendMessage(GetMissedEventWnd(), WM_CLOSE, 0, 0);
							if(GetQuickDialWnd())
								SendMessage(GetQuickDialWnd(), WM_CLOSE, 0, 0);
							if(GetSoundPorfileWnd())
								SendMessage(GetSoundPorfileWnd(), WM_CLOSE, 0, 0);
							if(pAppMain->hLaunchWnd)
								SendMessage(pAppMain->hLaunchWnd, WM_CLOSE, 0, 0);
						}
						SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
						AppCurFileHide(pAppMain);//inactive cur app
					}
					else
					{
						if(hCurWnd != NULL)
						{	
							if(hCurWnd == GetMissedEventWnd()
								|| hCurWnd == hAppWnd||hCurWnd == GetQuickDialWnd()
								||hCurWnd == GetSoundPorfileWnd())
								SendMessage(hCurWnd, WM_CLOSE, 0, 0);
							if(pAppMain->hQuickMenuhWnd != 0)
								SendMessage(pAppMain->hQuickMenuhWnd, WM_CLOSE, 0, 0);
						}
					}
					BringWindowToTop(pAppMain->hGrpWnd);
				}
			}
			else 
			{
				HWND hCurWnd = NULL;
				hCurWnd = GetActiveWindow();
				
				if(IsCurWndApp())
				{
					if(hCurWnd == hAppWnd)
					{
						SendMessage(hCurWnd, WM_CLOSE, 0, 0);
						if(GetMissedEventWnd())
							SendMessage(GetMissedEventWnd(), WM_CLOSE, 0, 0);
						if(GetQuickDialWnd())
							SendMessage(GetQuickDialWnd(), WM_CLOSE, 0, 0);
						if(GetSoundPorfileWnd())
							SendMessage(GetSoundPorfileWnd(), WM_CLOSE, 0, 0);
						if(pAppMain->hLaunchWnd)
							SendMessage(pAppMain->hLaunchWnd, WM_CLOSE, 0, 0);
					}
					SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
					AppCurFileHide(pAppMain);//inactive cur app
				}
				else
				{
					if(hCurWnd != NULL)
					{	
						if(hCurWnd == GetMissedEventWnd()
							|| hCurWnd == hAppWnd || hCurWnd == GetQuickDialWnd()
							|| hCurWnd == GetSoundPorfileWnd())
							SendMessage(hCurWnd, WM_CLOSE, 0, 0);
						if(pAppMain->hQuickMenuhWnd != 0)
							SendMessage(pAppMain->hQuickMenuhWnd, WM_CLOSE, 0, 0);
						else
							ShowWindow(GetActiveWindow(), SW_HIDE);
					}
				}
				
				ShowWindow(pAppMain->hGrpWnd, SW_SHOW);	
			}
		}
		return TRUE;
	}
	
	return TRUE;
	
}
/********************************************************************
* Function   EndKeyProc  
* Purpose    handle End key(return to idle)(VK_F2)
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL EndKeyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	
	HWND hCurWnd = NULL;
	calliftype callstate = GetCallIf();

	if((lParam & 0xc0000000)) //keyup
		return TRUE;
	
	if(TopWndIsNotification())//tipswin not hide
		return TRUE;
	
	if(callstate == diallinkif || callstate == ringif ||callstate == callendif/*|| callstate == callingif*/)
		return TRUE;
	
	if(Clock_IsAlarming() || CALE_IsAlarmWnd())//clock and calendar alarm, not hide 
		return TRUE;

	if(!pAppMain || !pAppMain->hIdleWnd)
		return (CallNextHookEx(prog_kbHook, nCode, wParam, lParam));

	hCurWnd = GetActiveWindow();

	if(hCurWnd == GetMissedEventWnd())
		return TRUE;
	
	if(IsCurWndApp())
	{
		if(hCurWnd == hAppWnd)
		{
			SendMessage(hCurWnd, WM_CLOSE, 0, 0);
			if(GetMissedEventWnd())
				SendMessage(GetMissedEventWnd(), WM_CLOSE, 0, 0);
			if(GetQuickDialWnd())
				SendMessage(GetQuickDialWnd(), WM_CLOSE, 0, 0);
			if(GetSoundPorfileWnd())
				SendMessage(GetSoundPorfileWnd(), WM_CLOSE, 0, 0);
			if(pAppMain->hLaunchWnd)
				SendMessage(pAppMain->hLaunchWnd, WM_CLOSE, 0, 0);
		}
		SendMessage(GetCapture(), WM_CANCELMODE, 0, 0);
		AppCurFileHide(pAppMain);//inactive cur app
	}
	else
	{
//		if(hCurWnd == pAppMain->hIdleWnd)
//			bPressEndKey = TRUE;
		if(hCurWnd != NULL)
		{	
			if(hCurWnd == pAppMain->hLaunchWnd ||hCurWnd == GetMissedEventWnd()
				|| hCurWnd == hAppWnd|| hCurWnd == GetQuickDialWnd()
				|| hCurWnd == GetSoundPorfileWnd())
				SendMessage(hCurWnd, WM_CLOSE, 0, 0);
			if(pAppMain->hQuickMenuhWnd != 0)
				SendMessage(pAppMain->hQuickMenuhWnd, WM_CLOSE, 0, 0);
			else if(hCurWnd != pAppMain->hIdleWnd && hCurWnd == GetMissedEventWnd())
				ShowWindow(hCurWnd, SW_HIDE);
		}
	}

	if(pAppMain->hIdleWnd)
	{
		if(IsWindowVisible(pAppMain->hIdleWnd))//show or hide
		{
			if(GetActiveWindow() != GetMissedEventWnd()&&
				GetActiveWindow() != pAppMain->hIdleWnd)
			{
				BringWindowToTop(pAppMain->hIdleWnd);
			}
		}
		else //hide
		{
			ShowWindow(pAppMain->hIdleWnd, SW_SHOW);
		}
	}
	
	return TRUE;
}

/********************************************************************
* Function   PowerKeyProc  
* Purpose    handle power key(vk_f6)
* Params     
* Return     
* Remarks      
**********************************************************************/
extern BOOL IfTipWndLock(void);
static void PowerKeyProc(void)
{
	HWND hWndAct = NULL, hWndFind = NULL, hWndNext = NULL;
	int iResult = TRUE ;

	if(IfTipWndLock())
		return;

	PlayPowerOff (TRUE);
	return;
}
/********************************************************************
* Function   PMShowIdle  
* Purpose    show idle view(for call app)
* Params     
* Return     
* Remarks      
**********************************************************************/
void PMShowIdle(void)
{
	HWND hCurWnd = NULL;
	
	if(!pAppMain->hIdleWnd)
		return ;
	
	hCurWnd = GetActiveWindow();

	if(hCurWnd != NULL)
	{	
		if(hCurWnd == pAppMain->hQuickMenuhWnd 
			|| hCurWnd == pAppMain->hLaunchWnd ||hCurWnd == GetMissedEventWnd()
			|| hCurWnd == GetQuickDialWnd())
			SendMessage(hCurWnd, WM_CLOSE, 0, 0);
		else
			ShowWindow(hCurWnd, SW_HIDE);
	}
	
	if(pAppMain->hIdleWnd)
	{
		if(IsWindowVisible(pAppMain->hIdleWnd))//show or hide
		{
			if(GetActiveWindow() != pAppMain->hIdleWnd)
			{
				BringWindowToTop(pAppMain->hIdleWnd);
			}
		}
		else //hide
		{
			ShowWindow(pAppMain->hIdleWnd, SW_SHOW);
		}
	}
}
#define pClassName "LeveAdjClass"
#define ICONVOL "/rom/setup/sett_volume_19x19.bmp"
//#define ICONLIT "/rom/setup/sett_brightness_19x19.bmp"
#define ICONIND "/rom/setup/icon_levind_26x46.bmp"
#define ICONINDEX "/rom/setup/icon_levind_ex_26x46.bmp"

#define ICON_IND_WIDTH      26
#define ICON_IND_HEIGHT     46    

#define ICON_INDEX_WIDTH    26
#define ICON_INDEX_HEIGHT   31

#define ICON_IND_POS_INIT_X 17
#define ICON_IND_POS_INIT_Y 102
#define ICON_IND_POS_INIT_W 26
#define ICON_IND_POS_INIT_H 26

#define ICON_IND_X_GAP      28
#define ICON_IND_Y_GAP      5


#define TITLE_ICON_WIDTH    19
#define TITLE_ICON_HEIGHT   19

#define TITLE_AREA_HEIGHT   30

#define TITLE_ICON_X        79
#define TITLE_ICON_Y        6

#define TITLEBK_COLOR  (RGB(0, 0, 255))
#define IDC_BUTTON_QUIT 101
#define IDC_BUTTON_SET  102

#define WM_SETVOLUME  WM_USER + 200

#define CR_ICON_IND	  (RGB(51,152,204))

#define TIMERID_ADJVOLUME     301

static int  nLevel;
static HBITMAP hBitmap;
static HBITMAP hLevInd;
static HBITMAP hLevIndEX;
static HBRUSH hBgBrush;
static int    TimerId_Adjvolume;
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void OnCreate(HWND hWnd);
static void OnKeyDown(HWND hWnd, UINT wMsgCmd,WPARAM wParam, LPARAM lParam);
static void OnPaint(HWND hWnd);

static BOOL CallLevelAdjuster(HWND hcallWnd, int nlevel)
{
    WNDCLASS wc;

    nLevel = nlevel;

    if(nLevel < 0 || nLevel > 5)
        return FALSE;
   
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    hAppWnd = CreateWindow(pClassName,ML("Speaker volume"), 
                WS_VISIBLE |WS_CAPTION|PWS_STATICBAR,                    
                PLX_WIN_POSITION,
                NULL,
                NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

	SendMessage(hAppWnd, PWM_CREATECAPTIONBUTTON, // set the right soft key
                (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("") );

    SendMessage(hAppWnd, PWM_CREATECAPTIONBUTTON, //set the left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)ML("") ) ;
    
    SendMessage(hAppWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SetFocus(hAppWnd);

    return TRUE;
}

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT     lResult;   
    lResult = TRUE;
    switch(wMsgCmd)
    {
    case WM_CREATE:
        OnCreate(hWnd);
		TimerId_Adjvolume = SetTimer(hWnd, TIMERID_ADJVOLUME, 2000, NULL);
    	break;

	case WM_ACTIVATE:
        if (WA_INACTIVE != LOWORD(wParam))
		{
			SetFocus(hWnd);
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, // set the right soft key
                (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("") );
			
			SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, //set the left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)ML("") ) ;
		}
		else
		{
			if(TimerId_Adjvolume)
			{
				KillTimer(hWnd, TIMERID_ADJVOLUME);
				TimerId_Adjvolume = 0;
			}
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
        break;
	case WM_TIMER:
		{
			if(TimerId_Adjvolume)
			{
				KillTimer(hWnd, TIMERID_ADJVOLUME);
				TimerId_Adjvolume = 0;
			}
			{
				unsigned int value = 0;
				
				
				if(nLevel > 4)
					value = 4;
				else
					value = nLevel;
				
				if(ME_SetSpeakerVolumeSNFV(hWnd, WM_SETVOLUME, value) < 0)
				{
					printf("\r\n ME_SetSpeakerVolumeSNFV failure!!");
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				}
			}
		//	PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

    case WM_PAINT:
        OnPaint(hWnd);
    	break;

	case WM_SETVOLUME:
		{
			SetHeadSetVolum(nLevel);
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;

    case WM_KEYDOWN:
		{
			if(TimerId_Adjvolume)
			{
				KillTimer(hWnd, TIMERID_ADJVOLUME);
				TimerId_Adjvolume = 0;
			}
			OnKeyDown(hWnd,wMsgCmd,wParam,lParam);
		}
        break;
	case WM_KEYUP:
		{
			TimerId_Adjvolume = SetTimer(hWnd, TIMERID_ADJVOLUME, 2000, NULL);
		}
		break;
		
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
    case WM_DESTROY:
        DeleteObject(hBitmap);
		hBitmap = NULL;
         DeleteObject(hLevInd);
		hLevInd = NULL;
        DeleteObject(hLevIndEX);
		hLevIndEX = NULL;
		DeleteObject(hBgBrush);
		hBgBrush = NULL;
		hBgBrush = NULL;
        hAppWnd = NULL;
        UnregisterClass(pClassName,NULL);
        break;
    case WM_COMMAND:
        break;
    default:
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    }
    return lResult;
}
static void OnCreate(HWND hWnd)
{
	HDC hdc;

	hdc = GetDC(hWnd);

	hBgBrush = CreateSolidBrush(RGB(158,177,230));

	hBitmap = LoadImageEx(hdc,NULL,ICONVOL,IMAGE_BITMAP,TITLE_ICON_WIDTH,TITLE_ICON_HEIGHT,LR_LOADFROMFILE);
	
    hLevInd = LoadImageEx(hdc,NULL,ICONIND,IMAGE_BITMAP,ICON_IND_WIDTH,ICON_IND_HEIGHT,LR_LOADFROMFILE);   
	
    hLevIndEX = LoadImageEx(hdc,NULL,ICONINDEX,IMAGE_BITMAP,ICON_INDEX_WIDTH, ICON_INDEX_HEIGHT,LR_LOADFROMFILE);

	ReleaseDC(hWnd, hdc);
	
}
static void OnPaint(HWND hWnd)
{
    HDC           hdc = NULL;
    PAINTSTRUCT   ps;
    RECT          rc;
    RECT          rcTitle;
    int i,iIndPos, iIndWidth, iIndHeight, iIndY;
    int iBKIndNum = 5;
	int	nBkMode;
	COLORREF nBkColor;
	HBRUSH hOldBrush,hIndBrush;
	HPEN hOldPen;
	
    hdc = BeginPaint(hWnd, &ps);
    
    GetClientRect(hWnd, &rc);
    rcTitle.bottom = TITLE_AREA_HEIGHT;
    rcTitle.right = rc.right;
    rcTitle.left = 0;
    rcTitle.top = rc.top;
	
	hOldBrush = SelectObject(hdc, hBgBrush);
	FillRect(hdc, &rcTitle, hBgBrush);

	SelectObject(hdc, hOldBrush);
	DeleteObject(hBgBrush);
	nBkMode = SetBkMode(hdc, BM_NEWTRANSPARENT);
	nBkColor = SetBkColor(hdc, COLOR_BLACK);
    BitBlt(hdc, TITLE_ICON_X, TITLE_ICON_Y, TITLE_ICON_WIDTH, TITLE_ICON_HEIGHT, (HDC)hBitmap, 0, 0, ROP_SRC);
	SetBkMode(hdc, nBkMode);
	SetBkColor(hdc, nBkColor);
	iIndWidth = ICON_IND_POS_INIT_W - 2;
    iIndHeight = ICON_IND_POS_INIT_H - 2;
    iIndY = ICON_IND_POS_INIT_Y + 1;
    iIndPos=ICON_IND_POS_INIT_X + 1;

	hIndBrush = CreateBrush(BS_SOLID,CR_ICON_IND,NULL);
	hOldBrush = SelectObject(hdc,hIndBrush);
	hOldPen = SelectObject(hdc, GetStockObject(BLACK_PEN));
    
	for(i=0;i<nLevel;i++)
    {
		RoundRect(hdc, iIndPos, iIndY, iIndPos + iIndWidth, iIndY + iIndHeight, 6, 6);
        iIndPos += ICON_IND_X_GAP;    
        iIndHeight += ICON_IND_Y_GAP;
        iIndY -= ICON_IND_Y_GAP;
    }
	hIndBrush = SelectObject(hdc,hOldBrush);
	SelectObject(hdc,hOldPen);
	DeleteObject(hIndBrush);
    

	if (i<iBKIndNum)
	{
		hOldPen = SelectObject(hdc, GetStockObject(BLACK_PEN));
		for(;i<iBKIndNum;i++)
		{
			RoundRect(hdc, iIndPos, iIndY, iIndPos + iIndWidth, iIndY + iIndHeight, 6, 6);
			iIndPos += ICON_IND_X_GAP;    
			iIndHeight += ICON_IND_Y_GAP;
			iIndY -= ICON_IND_Y_GAP;
		}
		SelectObject(hdc,hOldPen);
	}

    EndPaint(hWnd, &ps);
}

static void OnKeyDown(HWND hWnd, UINT wMsgCmd,WPARAM wParam, LPARAM lParam)
{

	RECT refRect;

    switch (wParam)
    {
    case VK_LEFT:
	case VK_F7:
		refRect.left = ICON_IND_POS_INIT_X + (nLevel - 1)*ICON_IND_X_GAP;
		refRect.top = ICON_IND_POS_INIT_Y - (nLevel - 1)*ICON_IND_Y_GAP;
		refRect.right = refRect.left + ICON_IND_POS_INIT_H;
		refRect.bottom = refRect.top + ICON_IND_POS_INIT_W + (nLevel - 1)* ICON_IND_Y_GAP;
       	if(nLevel != 0)
		{
			nLevel -= 1;
			InvalidateRect(hWnd, &refRect, TRUE);
		}
        break;
        
    case VK_RIGHT:
	case VK_F8:
        if(nLevel != 5)
		{
			nLevel += 1;
			
			refRect.left = ICON_IND_POS_INIT_X + (nLevel- 1)*ICON_IND_X_GAP;
			refRect.top = ICON_IND_POS_INIT_Y - (nLevel - 1)*ICON_IND_Y_GAP;
			refRect.right = refRect.left + ICON_IND_POS_INIT_H;
			refRect.bottom = refRect.top + ICON_IND_POS_INIT_W + (nLevel - 1)* ICON_IND_Y_GAP;
			InvalidateRect(hWnd, &refRect, TRUE);
		}
        break;

    default:
        PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
}

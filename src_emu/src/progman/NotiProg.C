         /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman
 *
 * Purpose  : implement interface for application to progman
 *            
\**************************************************************************/

#include "progman.h"
#include "string.h"
#include "plx_pdaex.h"
#include "app.h"
#include "appdirs.h"
#include "appfile.h"
#include "pmalarm.h"
#include "PMI.h"
#include "stk/StkEx.h"

extern	PROGDESKIDLE  ProgDeskIdle;
extern	BOOL	g_bUseScrSave;
extern  BOOL    GetDIALState();
extern  void    AppDirsChangeIcon ( struct appMain * pAdm, int iType);

extern  PAPPADM		pAppMain;
static  BOOL bOnline = FALSE;
extern	BOOL	PlayPowerOff(BOOL bPlay);
extern BOOL f_ChangePowerMode ( BOOL bLimit );
extern HWND GetMissedEventWnd(void);
extern void PM_SetKeyLockStatus(BOOL bStatus);
extern void PM_SetPhoneLockStatus(BOOL bStatus);
extern BOOL PM_GetPhoneLockHideStatus(void);
extern BOOL PM_GetKeyLockHideStatus(void);
extern BOOL	PM_GetkeyLockStatus(void);
extern BOOL	PM_GetPhoneLockStatus(void);
extern BOOL f_sleep_unregister(int handle);
extern void f_SetBkLight(BOOL bEnable);
BOOL    bLockEnable;
static int g_nPoweroffTimeID;

static	VOID    CALLBACK 
f_KLightTimerProc ( HWND hWnd, UINT uMsg, UINT nID, DWORD dwTime );
static	VOID    CALLBACK 
f_BLightTimerProc ( HWND hWnd, UINT uMsg, UINT nID, DWORD dwTime );
static	VOID    CALLBACK 
pPowerOffWndProc ( HWND hWnd, UINT uMsg, UINT nID, DWORD dwTime );
/*************************************************************************
* Function	GetDIALState
* Purpose	
* Params
* Return	 	   
* Remarks	  
**************************************************************************/
extern  BOOL    GetDIALState()
{
    return bOnline;
}

/*************************************************************************
* Function	f_ChangeSimCopsInfo
* Purpose	
* Params
* Return	 	   
* Remarks	  
**************************************************************************/
static BOOL f_ChangeSimCopsInfo(void)
{    
    switch(pm_sim_cop_status)
    {
    case pm_no_sim:
        strcpy(ProgDeskIdle.cPlmnStr,	ML("Insert SIM card"));
        break;
    case pm_sim_destroy:
        strcpy(ProgDeskIdle.cPlmnStr, ML("SIM card is destroied"));
        break;
	case pm_cops_searching:
        strcpy(ProgDeskIdle.cPlmnStr, ML("Searching"));
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
    default:
        return FALSE;
    }
    
    return TRUE;
}
/*************************************************************************
* Function	f_ReSetDelayStart
* Purpose	
* Params
* Return	 	   
* Remarks	  
**************************************************************************/
#define PMS_DELAY_MAX	3

static	DWORD	g_aStart[PMS_DELAY_MAX];
static	WORD	g_aDelay[PMS_DELAY_MAX];
void	f_ReSetDelayStart( WORD nIndex )
{
    WORD	i;	
    
    switch( nIndex ) 
    {
    case PMS_DELAY_SHUT_KEYLIGHT:		
        
        i = 0;
        break;
    case PMS_DELAY_SHUT_LIGHT_MAIN:
        
        i = 1;
        break;
    case PMS_DELAY_SHUT_CPU:
        
        i = 2;
        break;
    default:
        
        return;
    }
    g_aStart[i] = GetTickCount();
}
/********************************************************************
* Function	   f_GetStart
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	DWORD	f_GetStart( WORD nIndex )
{
    WORD	i;	
    
    switch( nIndex ) 
    {
    case PMS_DELAY_SHUT_KEYLIGHT:
        
        i = 0;
        break;
    case PMS_DELAY_SHUT_LIGHT_MAIN:
        
        i = 1;
        break;
    case PMS_DELAY_SHUT_CPU:
        
        i = 2;
        break;
    default:
        
        return 0;
    }
    return	g_aStart[i];
}
/********************************************************************
* Function	   f_SetDelay
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	DWORD	f_SetDelay( WORD nIndex, DWORD nTimer )
{
    WORD	nRet;
    WORD	i;
    
    if (nTimer < 1|| nTimer > 0xffff )
        return	0;
    
    switch( nIndex ) 
    {
    case PMS_DELAY_SHUT_KEYLIGHT:
        
        i = 0;
        break;
    case PMS_DELAY_SHUT_LIGHT_MAIN:
        
        i = 1;
        break;
    case PMS_DELAY_SHUT_CPU:
        
        i = 2;
        break;
    default:
        
        return	0;
    }
    nRet = g_aDelay[i];
    g_aDelay[i] = (WORD)nTimer;
    
    return	nRet;
}

/*************************************************************************
* Function	f_GetDelay( HWND hWnd, WORD nIndex, PMUINT32* pTimer )
* Purpose	
* Params	hWnd	: window handle
nIndex	:
pTimer	:
* Return	Process return TRUE 	   
* Remarks	  
**************************************************************************/
static	BOOL	f_GetDelay( WORD nIndex, DWORD* pTimer )
{
    WORD	i;
    
    if ( NULL == pTimer )
        return	FALSE;
    
    switch( nIndex ) 
    {
    case PMS_DELAY_SHUT_KEYLIGHT:
        
        i = 0;
        break;
    case PMS_DELAY_SHUT_LIGHT_MAIN:
        
        i = 1;
        break;
    case PMS_DELAY_SHUT_CPU:
        
        i = 2;
        break;
    default:
        
        return	FALSE;
    }
    *pTimer = g_aDelay[i];
    
    return	TRUE;
}
/********************************************************************
* Function	   f_CheckSleepMode
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	WORD	g_nflags = PMF_AUTO_SHUT_CPU;
static	BYTE	g_nTimer;
static	BYTE	g_nAttrib;
static	UINT	g_nTimeID_KLight,g_nTimeID_BLight;

#define	PMA_HAS_KEYLIGHT_ON			0x01
#define PMA_HAS_MAIN_LIGHT_ON		0x02
#define PMA_HAS_SHUT_DOWN			0x04

void    f_CheckSleepMode(void)
{
//    if ( g_nAttrib & PMA_HAS_KEYLIGHT_ON )
//        return;
    
    if ( g_nflags & PMF_AUTO_SHUT_CPU )
        f_ChangePowerMode( TRUE );
}
/********************************************************************
* Function	   f_Enable
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
extern void f_SetKeyLight(BOOL bOn);
extern WORD GetLockKeyCode(void);
static	DWORD	f_Enable ( BOOL bEnable, DWORD nflags )
{
    DWORD	nRet = 0;
    WORD	aflags[] =
    {
        PMF_KEYLIGHT	,		PMF_MAIN_BKLIGHT,	
            PMF_AUTO_SHUT_KEYLIGHT,		PMF_AUTO_SHUT_MAIN_LIGHT,
            PMF_AUTO_SHUT_CPU,			PMF_AUTO_SHUT_DOWN,0
            
    }, i = 0;
    
    while ( 0 != aflags[i] )
    {
        //	IN WHILE LOOP
        if ( (!(aflags[i] & nflags)) ||							
            ( bEnable && (g_nflags & aflags[i]) ) ||		
            ( !bEnable && !(g_nflags & aflags[i] ) ) )	
        {
			if( bEnable && (g_nflags & aflags[i]) )
			{
				if((aflags[i] & PMF_MAIN_BKLIGHT) &&(nflags & PMF_MAIN_BKLIGHT) 
					&& (PM_GetkeyLockStatus() || PM_GetPhoneLockStatus()))
				{
					if((GetLockKeyCode() != VK_RIGHT )&&
						(GetLockKeyCode() != VK_F6))
						break;
				}		
			}
		
			i++;
			continue;
		
        }
        switch ( aflags[i] )
        {
        case PMF_AUTO_SHUT_DOWN:
            
			printf("\r\n auto shut down!\r\n");
            if ( bEnable && (g_nAttrib & PMA_HAS_SHUT_DOWN) )
                PlayPowerOff (TRUE);
            break;
        case PMF_KEYLIGHT:

			f_SetKeyLight(bEnable);

            if ( bEnable )
            {
                if ( g_nflags & PMF_AUTO_SHUT_KEYLIGHT )
                    f_ReSetDelayStart ( PMS_DELAY_SHUT_KEYLIGHT );
                
                g_nAttrib |= PMA_HAS_KEYLIGHT_ON;
            }
            else
            {
                g_nAttrib &= ~PMA_HAS_KEYLIGHT_ON;
            }			
            break;
	
        case PMF_MAIN_BKLIGHT:
            
			f_SetBkLight(bEnable);
			
            if ( bEnable )
            {
                f_ReSetDelayStart ( PMS_DELAY_SHUT_LIGHT_MAIN );
                
                g_nAttrib |= PMA_HAS_MAIN_LIGHT_ON;
            }
            else
            {
                g_nAttrib &= ~PMA_HAS_MAIN_LIGHT_ON;
            }
            break;
        case PMF_AUTO_SHUT_CPU:
            if (bEnable)
            {
                g_nflags |= PMF_AUTO_SHUT_CPU;                 
            }
            else
            {
                g_nflags &= ~PMF_AUTO_SHUT_CPU;                 
            }
            break;
            
            break;
        case PMF_AUTO_SHUT_KEYLIGHT	:
            
            if ( bEnable )
            {
                f_ReSetDelayStart ( PMS_DELAY_SHUT_KEYLIGHT );				
				g_nTimeID_KLight = SetTimer( NULL, 1, 3000, f_KLightTimerProc ); 
			
            }
            else
            {
				if(g_nTimeID_KLight)
				{
					KillTimer( NULL, g_nTimeID_KLight );
					g_nTimeID_KLight = 0;
				}
            }
            break;
        case PMF_AUTO_SHUT_MAIN_LIGHT:
            
            if ( bEnable )
            {
                f_ReSetDelayStart ( PMS_DELAY_SHUT_LIGHT_MAIN );
				g_nTimeID_BLight = SetTimer( NULL, 2, 10000, f_BLightTimerProc ); 
            }
            else
            {	
				if(g_nTimeID_BLight)
				{
					KillTimer( NULL, g_nTimeID_BLight );
					g_nTimeID_BLight = 0;   
				}
            }
            break;
        }		
        if ( bEnable )
            g_nflags |= aflags[i];
        else
            g_nflags &= ~aflags[i];
        i++;
        
        nRet |= aflags[i];
    }
    return	nRet;
}
/********************************************************************
* Function	   f_KLightTimerProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	VOID    CALLBACK 
f_KLightTimerProc ( HWND hWnd, UINT uMsg, UINT nID, DWORD dwTime )
{
    DWORD	nTime, nStart, nDelay;
    
    nTime = GetTickCount();
    
    if ( (g_nAttrib & PMA_HAS_KEYLIGHT_ON) && 
        ( g_nflags & PMF_AUTO_SHUT_KEYLIGHT ) )
    {
        nStart = f_GetStart (PMS_DELAY_SHUT_KEYLIGHT);
        f_GetDelay ( PMS_DELAY_SHUT_KEYLIGHT, &nDelay );
        if ( nTime > nStart + nDelay*1000 )
        {
            f_Enable ( FALSE, PMF_KEYLIGHT );
     
            if (g_nflags & PMF_AUTO_SHUT_CPU)
                f_ChangePowerMode( TRUE );
        }
    }
    
  }
/********************************************************************
* Function	   f_BLightTimerProc
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static	VOID    CALLBACK 
f_BLightTimerProc ( HWND hWnd, UINT uMsg, UINT nID, DWORD dwTime )
{
    DWORD	nTime, nStart, nDelay;
    
    nTime = GetTickCount();
	
      
    if ( (g_nAttrib & PMA_HAS_MAIN_LIGHT_ON) && 
        ( g_nflags & PMF_AUTO_SHUT_MAIN_LIGHT ) )
    {
        nStart = f_GetStart (PMS_DELAY_SHUT_LIGHT_MAIN);
        f_GetDelay ( PMS_DELAY_SHUT_LIGHT_MAIN, &nDelay );
		if((nDelay == 1)&& (nTime > nStart + nDelay*100))
		{
            f_Enable ( FALSE, PMF_MAIN_BKLIGHT );
		}
		else
		{
			if ( nTime > nStart + nDelay*1000 )
			{
				f_Enable ( FALSE, PMF_MAIN_BKLIGHT );
			}
		}
    }
}

/*************************************************************************
* Function		: f_CallApp
* Description	: 
* Argument		: HWND hWnd
*				: HPMCALLAPP hEntry
* Return type	: 
**************************************************************************/
static	BOOL	f_CallApp( HPMCALLAPP hEntry )
{
    APPENTRY    aEntry;    
    
    aEntry.hApp     = NULL;
    aEntry.wParam   = hEntry->v_wParam;
    aEntry.lParam   = hEntry->v_lParam;
    aEntry.nCode    = hEntry->v_nCode;
    aEntry.pAppName = hEntry->v_pPath;
    
    return  CallAppFile( pAppMain, &aEntry );
}

//extern  RECT    g_rcSound;

/*************************************************************************
* Function		: DlmNotify
* Description	: 
* Argument		: WPARAM wParam
*				: LPARAM lParam
* Return type	: DWORD 
**************************************************************************/
static int gSaver_TimerID;
static BOOL bAutoPowerOff;
extern HWND GetGPSWnd(void);
extern BOOL NotifyAppFile(int nCode);
extern BOOL CreateMissedEventsWnd(HWND hWnd);
extern BOOL	InitMissedEventsNum();
extern BOOL MobileInit2(void);
extern void PowerOff();
extern void	DHI_Reset(void);
extern BOOL IsAlarmPowerOn(void);
extern BOOL IfPoweroffEnable(void);
BOOL IfInAutoPowerOff(void)
{
	return bAutoPowerOff;
}
static	VOID    CALLBACK 
pPowerOffWndProc ( HWND hWnd, UINT uMsg, UINT nID, DWORD dwTime )
{

	printf("\r\npoweroff timerout!");
	WaitWin(pAppMain->hIdleWnd, FALSE, ML("Switching off power..."), ML("Auto switch off"), 0,
		ML("Cancel"), CALLBACK_CANCEL_OFF);
	if(g_nPoweroffTimeID)
	{
		KillTimer(NULL, g_nPoweroffTimeID);
		g_nPoweroffTimeID = 0;
		printf("\r\n play power off!");
		PlayPowerOff (TRUE);
	}
	f_sleep_unregister(INPOWEROFF);
}
void PM_CancelPoweroff(void)
{
	if(g_nPoweroffTimeID)
	{
		KillTimer(NULL, g_nPoweroffTimeID);
		g_nPoweroffTimeID = 0;
	}
}
extern HWND GetKeyLockParentWnd(void);
extern HWND GetKeyLockWnd(void);
extern HWND GetPhoneLockWnd(void);
extern void SetEnterInbox(BOOL bSet);
extern void SetEnterMail(BOOL bSet);
extern void SetEnterPush(BOOL bSet);
DWORD DlmNotify( WPARAM wParam, LPARAM lParam )
{
    DWORD   nRet = 0;
    
    switch( LOWORD(wParam) )
    {
    case PMM_NEWS_SHUT_DOWN:

		printf("\r\n auto shuttle down !\r\n");
		f_sleep_register(INPOWEROFF);
        
        if ( g_nflags & PMF_AUTO_SHUT_DOWN )
		{
			if(IfPoweroffEnable())
			{
				
				printf("\r\n enable power off!");
				bAutoPowerOff = FALSE;
				if(g_nPoweroffTimeID)
				{
					KillTimer(NULL, g_nPoweroffTimeID);
					g_nPoweroffTimeID = 0;
				}
				g_nPoweroffTimeID = SetTimer(NULL,0, 10000, pPowerOffWndProc);
				printf("\r\n g_nPoweroffId = %d", g_nPoweroffTimeID);
				PostMessage(pAppMain->hIdleWnd, CALLBACK_POWEROFF, 0, 0);	
			}
			else
			{
				printf("\r\n disable power off!");
				bAutoPowerOff = TRUE;
			}
		}
        else
            g_nAttrib |= PMA_HAS_SHUT_DOWN;
        
        return	TRUE;
    case PMM_NEWS_CALL_APP:
        
        return	f_CallApp ( (HPMCALLAPP)lParam );
    case PMM_NEWS_GET_FLAGS:
        
        return  g_nflags;
    case PMM_NEWS_ENABLE:
        
        return	f_Enable ( TRUE, lParam );
    case PMM_NEWS_DISABLE:
        
        return	f_Enable ( FALSE, lParam );
    case PMM_NEWS_SET_DELAY:

        return	f_SetDelay ( HIWORD(wParam), (DWORD)lParam );
    case PMM_NEWS_GET_DELAY:
        
        return	f_GetDelay ( HIWORD(wParam), (DWORD*)lParam );

	case PMM_OPERATOR_CHANGE:
		ME_GetCurOprator(pAppMain->hDeskWnd, WM_PROG_COPSINFO, FORMAT_LONGALPHA);
		break;
	case PS_ALARMMODE:
		if(lParam == TRUE)
		{
			//normal power on
			if(IsAlarmPowerOn())
				MobileInit2();
		}
		else
		{
			if (DHI_CheckChargerStatus())
			{
				printf("\r\ncharger is connected, enter charger power off...\r\n");
				
				// we should reset the phone when the charger is connected at this time
#ifndef	_EMULATE_
				DHI_Reset();
#else
				exit(0);
#endif
			}
			else
				PlayPowerOff(FALSE);

		}
		break;

	case PMM_NEWS_ENABLE_SCRSAVE:	
		return  nRet;

    case PS_SETCALLDIVERT:
	case PS_CALLDIVERTLINE1:
	case PS_CALLDIVERTLINE2:
	case PS_SETROAMING:
	case PS_SETCIPHER:
	case PS_SETHOMEZONE:
	case PS_SETACTIVELINE1:
	case PS_SETACTIVELINE2:
	case PS_SETBLUETOOTHON:
	case PS_SETBLUETOOTHOFF:
	case PS_SETHEADSET:
	case PS_SETCARKIT:
	case PS_SETMISSEDCALL:
	case PS_SETMSG:
	case PS_MSGUNREAD:
	case PS_MSGUNREAD1:
	case PS_MSGUNREAD2:
	case PS_MSGOVERFLOW:
	case PS_MSGOUTBOX:
	case PS_SETVOICEMAIL:
	case PS_SETVOICEMAIL1:
	case PS_SETVOICEMAIL2:
	case PS_SILENCE:
	case PS_SPROHTER:
	case PS_KEYLOCK:
	case PS_ALARMCLOCK:
	case PS_POWERON:
	case PS_POWEROFF:
    case PS_POWERONOFF:
	case PS_USB:
	case PS_SECURITYCON:
        DrawNotifyIcon(wParam, lParam);
        break;
	case PS_SETSGN:
        SendMessage(pAppMain->hDeskWnd,MYWM_SETTITLE,MYWM_SETSGN,lParam);
        break;
    case PS_SETBAT:
        SendMessage(pAppMain->hDeskWnd,MYWM_SETTITLE,MYWM_SETBAT,lParam);
        break;
	case PS_AUTOPOWERON:
		break;
	case PS_FRASHGPS:
		PostMessage(GetGPSWnd(), WM_GPSFRASH, 0, 0);

		break;
    case PS_SETTIME:
		{
			SendMessage(pAppMain->hIdleWnd, WM_TIMER, (WPARAM)2, NULL);
		}
        break;

	case PS_LOCK_ENABLE://for call & alarm app
		if(lParam == 1)
			bLockEnable = TRUE;
		else
			bLockEnable = FALSE;

		if(PM_GetkeyLockStatus() || PM_GetKeyLockHideStatus())
			PM_SetKeyLockStatus(bLockEnable);

		if(PM_GetPhoneLockStatus() || PM_GetPhoneLockHideStatus())
			PM_SetPhoneLockStatus(bLockEnable);
		
		break;
	case PS_NEWMSG://update missed events
		{
			HWND hCurwnd = NULL;

			if(lParam == 1)
				SetEnterInbox(FALSE);
			if(lParam == 2)
				SetEnterMail(FALSE);
			if(lParam == 3)
				SetEnterPush(FALSE);
			
			if(GetMissedEventWnd() != NULL)
			{
				PostMessage(GetMissedEventWnd(), WM_PROG_MISSCHANGE, 0, 0);
			}
			
			hCurwnd = GetActiveWindow();
			if(hCurwnd == pAppMain->hIdleWnd)
			{
				if(InitMissedEventsNum() && GetMissedEventWnd() == NULL)//if have missed events to enter missed events window
					CreateMissedEventsWnd(hCurwnd);
			}
			else 
			{
				if(GetMissedEventWnd() != NULL)
				{
					InitMissedEventsNum();
					InvalidateRect(GetMissedEventWnd(), NULL, TRUE);
				}
				
				if(GetPhoneLockWnd())
				{
					InitMissedEventsNum();
					InvalidateRect(GetPhoneLockWnd(), NULL, TRUE);
				}
				
				if(GetKeyLockWnd() && GetKeyLockParentWnd() == pAppMain->hIdleWnd)
				{
					ShowWindow(GetKeyLockWnd(), SW_HIDE);
					if(InitMissedEventsNum())//if have missed events to enter missed events window
					{
						if(GetMissedEventWnd() != NULL)
							UpdateWindow(GetKeyLockParentWnd());
						else
						{
							UpdateWindow(GetKeyLockParentWnd());
							CreateMissedEventsWnd(pAppMain->hIdleWnd);
						}
					}
					ShowWindow(GetKeyLockWnd(),SW_SHOW);
				}
			}
		}
		break;
	case PS_IDLEMODETXT:
		{
			
		   char strIdleTxt[513];
		
		   strIdleTxt[0] =0 ;
			
		   memset(ProgDeskIdle.cIdleModeTxt, 0, MAX_IDLEMODETETSIZE +1);
		   STK_GetIdleModeText(strIdleTxt);
		   if(strIdleTxt[0] != 0)
		   {
			   strncpy(ProgDeskIdle.cIdleModeTxt,strIdleTxt ,MAX_IDLEMODETETSIZE);
			   ProgDeskIdle.cIdleModeTxt[MAX_IDLEMODETETSIZE] = 0;
			   InvalidateRect(pAppMain->hIdleWnd, NULL, TRUE);
		   }
		   
		}
		break;
        
    case PS_SETLANGUAGE:
        f_ChangeSimCopsInfo();
        break;

	case PM_TIMECHANGED:
		NotifyAppFile(APP_TIMECHANGED);
		break;
        
    case PS_SETDESKTOP:           
	//	GetScreenPhrase(ProgDeskIdle.cDeskStr );	//get deskon phrase
        break;

	case PS_SETGPRS:
		{
			SendMessage(pAppMain->hDeskWnd, WM_PROG_SINGEL, 3, lParam);
		}
		break;
        
    case PS_SETCOLOR:
        break;
        
    case PS_DISAUTOSHUTCPU:
        if (g_nflags & PMF_AUTO_SHUT_CPU  && lParam)
        {
            g_nflags &= ~PMF_AUTO_SHUT_CPU; 
            f_ChangePowerMode( FALSE );
        }
        if (!(g_nflags & PMF_AUTO_SHUT_CPU) && !lParam)
        {
            g_nflags |= PMF_AUTO_SHUT_CPU; 
            f_ChangePowerMode( TRUE );
        }
        break;
  
    case PES_APP_QUIT:	
    
        return AppFileClose( pAppMain,  (HINSTANCE)lParam);
        
    case PES_APP_SHOW:		
        return ShowActiveFile( pAppMain, (HINSTANCE)lParam );
        
    case PES_APP_HIDE:									
        return AppFileHide ( pAppMain, (HINSTANCE)lParam );	
        
    case PES_APP_CALL:		
        return (DWORD)CallActiveFile( pAppMain,(PAPPENTRY)lParam );
        
//    case PES_APP_CREATE:
//        return (DWORD)ActiveAppFile(pAppMain, LOWORD(wParam), (LPCSTR) lParam );
        
    }
    return	nRet;
}

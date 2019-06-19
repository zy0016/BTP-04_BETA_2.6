/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : Automatic switch off
 *            
\**************************************************************************/

#include    "winpda.h"
#include    "str_plx.h"
#include    "setting.h"
#include    "str_public.h"
#include    "plx_pdaex.h"
#include    "dialer.h"
#include    "stdio.h"
#include    "stdlib.h"
#include    "window.h"
#include    "setup.h"
#include    "MBPublic.h"
#include	"PMI.h"
#define     IDC_BUTTON_QUIT         3 
#define     IDC_BUTTON_SET          4 
#define     ID_TIMER1               WM_USER+131
#define     ID_TIMER2               WM_USER+132
#define     ID_TIMER3               WM_USER+133
#define     CALLBACK_CANCEL_OFF     WM_USER+134

#define     IDC_TIMERSCAN           50000// 10 second
#define     IDC_TIMERCOUNT          1000 // 1 second

static const char * pClassName       = "AutoCloseClewWndClass";
static int          iAutoClost_Delay = 50;
static int          iSecond60        = 60;
static int          iTxtX            = 10;
static int          iTxtY            = 10;

static HWND hFrameWin;
static HWND hAutoClewWin;
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    JustCloseTimeS30(void);
static  BOOL    APP_GetMobileState(void);

//********************extern function********************
extern  BOOL    SetAutoClose_Execute(void);
extern  BOOL    MMSPro_IsIdle(void);//Get the state of MMS, return FALSE: it is retrieving MMS;
extern  BOOL    TS_IsProcessing(void);
//extern  int     GetUsedItemNum();//mbcalling.c 
extern  BOOL	Clock_IsAlarming(void);
extern	BOOL	MAIL_IsNetWork(void);
extern	BOOL	CALE_IsAlarmWnd(void);

extern  void	FetchAutoOCState(AUTO_CLOSE  *asOC);

//*********************************************************
BOOL    CallAutoCloseClewWindow(void)
{
    WNDCLASS    wc;
    RECT rClient;
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    
    hFrameWin = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
            
    GetClientRect(hFrameWin, &rClient);

    hAutoClewWin = CreateWindow(pClassName,NULL,
        WS_CHILD, 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hAutoClewWin)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  int         iTimerCount;
//            int         iDialState;
            BOOL        bState;
            LRESULT     lResult;
            char        cClew[50] = "";

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :        
        //SetTimer(hWnd,ID_TIMER1,IDC_TIMERSCAN,0); //Timer 1, check every 10 second,
                                                  //to see if 10 sec later is auto
		                                          //switch off time;    
		SetRTCTimer(hWnd, ID_TIMER1, IDC_TIMERSCAN, 0);
        iTimerCount = 10;//10 sec before power off;
        break;


    case WM_RTCTIMER://WM_TIMER:
        switch(wParam)
        {
        case ID_TIMER1:
            if (JustCloseTimeS30())
            {
                bState = TRUE;

				if ( IsCallAPP() )						//Phone call state
				{
					bState = FALSE;
					printf("\r\nAuto switch off ---> Call is on going/r/n");
				}
				

                if ( !MMSPro_IsIdle() )					//MMS state
				{
					bState = FALSE;
					printf("\r\nAuto switch off ---> MMS is on going/r/n");
				}

                if ( TS_IsProcessing() )				//PC sync state
				{
					bState = FALSE; 
					printf("\r\nAuto switch off ---> PC sync is on going/r/n");
				}
                
				if ( Clock_IsAlarming() )				//Alarm state
				{
					bState = FALSE;
					printf("\r\nAuto switch off ---> Clock Alarm is on going/r/n");
				}

				if ( MAIL_IsNetWork() )					//Email state
				{
					bState = FALSE;
					printf("\r\nAuto switch off ---> Email is on going/r/n");
				}

				if ( CALE_IsAlarmWnd() )				//Calendar alarm state
				{
					bState = FALSE;
					printf("\r\nAuto switch off ---> Calendar alarm is on going/r/n");
				}
				
                KillTimer(hWnd,ID_TIMER1);

                if (!bState)//if state is not free, start timer 2 to check the state.
                {
                    /*SetTimer*/SetRTCTimer(hWnd,ID_TIMER2,1000,0);
                }
                else
                {
					printf("\r\nAuto switch off ---> Free now! switch off after 10 seconds!!/r/n");
                    ShowWindow(hFrameWin,SW_SHOW);
                    UpdateWindow(hFrameWin);
                    SetFocus(hWnd);
                    WaitWin(hWnd, TRUE, ML("Switching off powerthreedot"), ML("Auto switch off"), 0,
                            ML("Cancel"), CALLBACK_CANCEL_OFF);
                    /*SetTimer*/SetRTCTimer(hWnd,ID_TIMER3,IDC_TIMERCOUNT,0);
                }
            }
            break;
        case ID_TIMER2:
			if ( IsCallAPP() )						//Phone call state
                return TRUE;

            if (!MMSPro_IsIdle())					//MMS state
                return TRUE;

            if ( TS_IsProcessing() )				//pc sync
                return TRUE; 

			if ( Clock_IsAlarming() )				//Alarm state
                return TRUE; 
			
			if ( MAIL_IsNetWork() )					//Email state
                return TRUE; 
			
			if ( CALE_IsAlarmWnd() )				//Calendar alarm state
                return TRUE; 
			
            KillTimer(hWnd,ID_TIMER2);

            ShowWindow(hFrameWin,SW_SHOW);
            UpdateWindow(hFrameWin);
            SetFocus(hWnd);
            WaitWin(hWnd, TRUE, ML("Switching off powerthreedot"), ML("Auto switch off"), 0,
                    ML("Cancel"), CALLBACK_CANCEL_OFF);
            /*SetTimer*/SetRTCTimer(hWnd,ID_TIMER3,IDC_TIMERCOUNT,0);
            break;
        case ID_TIMER3://power off after 10 second
            iTimerCount--;
            if (0 == iTimerCount)
            {
                /*KillTimer*/KillRTCTimer(hWnd,ID_TIMER1);
                /*KillTimer*/KillRTCTimer(hWnd,ID_TIMER2);
                /*KillTimer*/KillRTCTimer(hWnd,ID_TIMER3);
                DlmNotify(PMM_NEWS_ENABLE,PMF_AUTO_SHUT_DOWN);
                DlmNotify(PMM_NEWS_SHUT_DOWN,(LPARAM)0);    //power off
                WaitWin(hWnd, FALSE, NULL, NULL, NULL,NULL, NULL); 
                DestroyWindow(hWnd);
            }
            break;
        }
        break;

    case WM_SETFOCUS:
        SetFocus(hWnd);
        break;

    case WM_CLOSE:
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY :
        hAutoClewWin = NULL;
        UnregisterClass(pClassName,NULL);
        break;

    case CALLBACK_CANCEL_OFF:
        /*KillTimer*/KillRTCTimer(hWnd,ID_TIMER3);
        /*SetTimer*/SetRTCTimer(hWnd,ID_TIMER1,IDC_TIMERSCAN,0);//re-start the timer for auto switch off
        ShowWindow(hFrameWin,SW_HIDE);
        break;
        
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

/***************************************************************
* Function     JustCloseTimeS30
* Purpose      To see if there is just 10 second left 
*                           to the auto switch off time
* Params 
* Return        TURE: yes, FLASE: no
* Remarks
***************************************************************/
static  BOOL    JustCloseTimeS30(void)
{
    SYSTEMTIME  systemtime;
    AUTO_CLOSE  autoclose;

    FetchAutoOCState(&autoclose);

    if (autoclose.CloseState == SWITCH_OFF)//Auto switch off has been cancel
        return FALSE;

	//ignore auto switch off if its time is same as auto switch on time
	if ((autoclose.OpenHour == autoclose.CloseHour) && (autoclose.OpenMin == autoclose.CloseMin) && (autoclose.OpenState == SWITCH_ON))
		return FALSE;

    GetLocalTime(&systemtime);

    if (systemtime.wSecond + iAutoClost_Delay >= iSecond60)//it is next minute after 10 second
    {
        if (systemtime.wMinute + 1 == iSecond60)//it is next hour after next minute
        {
            if (systemtime.wHour + 1 == 24)//it is the next day after next hour
            {
                systemtime.wHour    = 0;
                systemtime.wMinute  = 0;
                systemtime.wSecond  = (systemtime.wSecond + iAutoClost_Delay) % iSecond60;
            }
            else
            {
                systemtime.wHour++;
                systemtime.wMinute  = 0;
                systemtime.wSecond  = (systemtime.wSecond + iAutoClost_Delay) % iSecond60;
            }
        }
        else
        {
            systemtime.wMinute++;
            systemtime.wSecond = (systemtime.wSecond + iAutoClost_Delay) % iSecond60;
        }
    }
    else
    {
        systemtime.wSecond += iAutoClost_Delay;
    }
    return ((systemtime.wHour == autoclose.CloseHour) && (systemtime.wMinute == autoclose.CloseMin));
}
static  BOOL    APP_GetMobileState(void)
{
    //return (GetUsedItemNum() != 0);
	return 0; 
}

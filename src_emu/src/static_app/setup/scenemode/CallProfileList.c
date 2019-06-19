/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : call window of profile list
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "plx_pdaex.h"
#include	"fcntl.h"
#include    "setting.h"
#include    "pubapp.h"
#include    "setup.h"
#include    "mullang.h"
#include    "CallProfileList.h"
	
#define SETT_VIBRA_TIME 300
static  SCENEMODE sm;

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static int      iCurMode;
static int      iContentMode;
static int      nCurVol;
static int      nCurKeyVol;
static int      nCurFocus;
static BOOL     CreateNewFlag;
static HWND     hAppWnd = NULL;
static HWND     hFrameWin = NULL;
static HWND     hFather = NULL;
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL		CreateControl (HWND hWnd, HWND *hName,HWND *hIncomeCall,HWND *hMessage,HWND *hTMessage,HWND *hAClock,HWND *hCalendar,HWND *hNotify,HWND *hMissEvent,HWND *hKTone,HWND *hVibra,HWND *hAlertGrp);
static void     ProfileSet_InitVScrolls(HWND hWnd,int iItemNum);
static void     ProfileSet_OnVScroll(HWND hWnd,  UINT wParam);
static void     SetProfile_Vibra(int iCurMode,SWITCHTYPE iCurIndex);

extern BOOL     CallProfileName(HWND hWndApp,int icurMode);
extern BOOL     CallRingSetWindow(HWND hwndApp,int icurMode,RINGFOR iringFor);
extern BOOL     CallMissedEvent(HWND hcallApp,int icurmode);
extern BOOL     CallAlertGrp(HWND hcallApp,int icurmode);
extern BOOL     CallVibraSettingWnd(HWND hParentWnd, int icurmode);
extern BOOL     CallLevelAdjuster(HWND hFrame, HWND hcallWnd, ADJTYPE adjType, int nlevel,UINT wCallbackMsg, const char * szTitle);
extern void     IntToString( int nValue, char *buf );

//BOOL Sett_VolumeAdjusting(LEVELVALUE level);
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);
void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);

BOOL CallProfileList(HWND hwndCall,HWND hFatherWnd, int icurmode)//edit the exist scenemode
{

	WNDCLASS wc;
    RECT rClient;

    hFrameWin = hwndCall;
    iCurMode = icurmode;
    hFather = hFatherWnd;
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    GetClientRect(hFrameWin, &rClient);
    
    GetSM(&sm,iCurMode);
    
    hAppWnd = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD | WS_VSCROLL,          
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Back"));//RSK
        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");

	if (strcmp(sm.cModeName, "Normal") == 0)
		SetWindowText(hFrameWin, ML("Normal"));
	else if (strcmp(sm.cModeName, "Silent") == 0)
		SetWindowText(hFrameWin, ML("Silent"));
	else if (strcmp(sm.cModeName, "Noisy") == 0)
		SetWindowText(hFrameWin, ML("Noisy"));
	else if (strcmp(sm.cModeName, "Discreet") == 0)
		SetWindowText(hFrameWin, ML("Discreet"));
	else 
		SetWindowText(hFrameWin, sm.cModeName);
    SetFocus(hAppWnd);

    return TRUE;

}

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT     lResult;   
	static HWND hName,hIncomeCall,hMessage,hTMessage,hAClock,hCalendar,hNotify,hMissEvent,hKTone,hVibra,hAlertGrp;
    static HWND hFocus = 0;
    static int  iButtonJust,iCurIndex;
    static HWND hWndFocus;
    static char     cCurModeName[21] = "";
    int iControlH;
	RECT    rect;    
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    lResult = (LRESULT)TRUE;
    
    switch ( wMsgCmd )
	{
	     case WM_CREATE:
			  CreateControl (hWnd,&hName,&hIncomeCall,&hMessage,&hTMessage,&hAClock,&hCalendar,&hNotify,&hMissEvent,&hKTone,&hVibra,&hAlertGrp);
              hFocus = hName;
              ProfileSet_InitVScrolls(hWnd,MAX_ITEMNUM);
              
              nCurVol = sm.iNotification;
              nCurKeyVol = sm.iKeyVolume;    
			  if (strcmp(sm.cModeName, "Normal") == 0)
			      SendMessage(hName,SSBM_ADDSTRING,0,(LPARAM)ML("Normal"));
              else if (strcmp(sm.cModeName, "Silent") == 0)
		          SendMessage(hName,SSBM_ADDSTRING,0,(LPARAM)ML("Silent"));
              else if (strcmp(sm.cModeName, "Noisy") == 0)
		          SendMessage(hName,SSBM_ADDSTRING,0,(LPARAM)ML("Noisy"));
              else if (strcmp(sm.cModeName, "Discreet") == 0)
		          SendMessage(hName,SSBM_ADDSTRING,0,(LPARAM)ML("Discreet"));
              else 
		          SendMessage(hName,SSBM_ADDSTRING,0,(LPARAM)sm.cModeName);


              SendMessage(hNotify,SSBM_SETCURSEL,nCurVol,0);              
              SendMessage(hKTone,SSBM_SETCURSEL,nCurKeyVol,0);              
              SendMessage(hVibra, SSBM_SETCURSEL, sm.iVibraSwitch, 0);

              if(sm.iAlertFor == ALERTALL)
                  SendMessage(hAlertGrp, SSBM_ADDSTRING, 0, (LPARAM)ML("All"));
              if(sm.iAlertFor == ALERTSELECT)
                  SendMessage(hAlertGrp, SSBM_ADDSTRING, 0, (LPARAM)ML("Selected"));
              if(sm.iAlertFor == ALERTNO)
                  SendMessage(hAlertGrp, SSBM_ADDSTRING, 0, (LPARAM)ML("None"));              

              strncpy(cCurModeName,sm.cModeName,sizeof(cCurModeName));
			  break;
         case WM_SETFOCUS:
              SetFocus(hFocus);
              break;
	  	 case PWM_ACTIVATE:
			 if (wParam == WA_INACTIVE)
			 {
				 KillTimer(hWnd, TIMER_ID);
				 RepeatFlag = 0;
				 wKeyCode   = 0;
			 }
			 break;
		 case WM_DESTROY ://
              hAppWnd = NULL;
              UnregisterClass(pClassName,NULL);
              break;
        case  WM_VSCROLL:
	    	  ProfileSet_OnVScroll(hWnd,wParam);
		      break;

        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            GetSM(&sm,iCurMode);
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            

			if (strcmp(sm.cModeName, "Normal") == 0)
			{
				SendMessage(hName,SSBM_SETTEXT,0,(LPARAM)ML("Normal"));
				SetWindowText(hFrameWin, ML("Normal"));
			}
			else if (strcmp(sm.cModeName, "Silent") == 0)
			{
				SendMessage(hName,SSBM_SETTEXT,0,(LPARAM)ML("Silent"));
				SetWindowText(hFrameWin, ML("Silent"));
			}
			else if (strcmp(sm.cModeName, "Noisy") == 0)
			{
				SendMessage(hName,SSBM_SETTEXT,0,(LPARAM)ML("Noisy"));
				SetWindowText(hFrameWin, ML("Noisy"));
			}
			else if (strcmp(sm.cModeName, "Discreet") == 0)
			{
				SendMessage(hName,SSBM_SETTEXT,0,(LPARAM)ML("Discreet"));
				SetWindowText(hFrameWin, ML("Discreet"));
			}
			else 
			{
				SendMessage(hName,SSBM_SETTEXT,0,(LPARAM)sm.cModeName);
				SetWindowText(hFrameWin, sm.cModeName);
			}

            
			SendMessage(hVibra, SSBM_SETCURSEL, sm.iVibraSwitch, 0);
            if(sm.iAlertFor == ALERTALL)
                SendMessage(hAlertGrp, SSBM_SETTEXT, 0, (LPARAM)ML("All"));
            if(sm.iAlertFor == ALERTSELECT)
                SendMessage(hAlertGrp, SSBM_SETTEXT, 0, (LPARAM)ML("Selected"));
            if(sm.iAlertFor == ALERTNO)
                SendMessage(hAlertGrp, SSBM_SETTEXT, 0, (LPARAM)ML("None"));          
                        
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Back"));//RSK
            
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            
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
                 if(hFather != NULL)
                 {
                    PostMessage(hFrameWin, PWM_CLOSEWINDOW, (WPARAM)hFather, 0);
                    PostMessage(hFather, WM_CLOSE, 0, 0);
                 }
                 SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                 PostMessage(hWnd, WM_CLOSE, 0, 0);
                 break;
             case VK_F5:
                 SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_OK,0);
                 break;
             case VK_DOWN:
                 SettListProcessKeyDown(hWnd, &hFocus);            
                 
                 SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
				 if ( 1 == RepeatFlag )
				 {
					 SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				 }
                 break;
                 
             case VK_UP:
                 SettListProcessKeyUp(hWnd, &hFocus);            
                 
                 SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
				 if ( 1 == RepeatFlag )
				 {
					 SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
				 }
                 break;
                 
             default:
                 lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
                 break;
             }
             break;  
        case WM_TIMER:
            OnTimeProcess(hWnd, wParam, hFocus);
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
		
		 case WM_COMMAND:
			 switch(LOWORD(wParam))
			 {
			 case IDC_BUTTON_CANCEL:
				 DestroyWindow(hWnd);
				 break;
			 case IDC_BUTTON_OK:
				 switch(iButtonJust)
				 {
			      case IDC_NAME:
                       if(iCurMode < 4)
                       {
                           PLXTipsWin(hFrameWin, hWnd, NULL,ML("Not allowedsnfor preset profile"),NULL,Notify_Alert,ML("Ok"),0,WAITTIMEOUT);
                       }
                       else
                           CallProfileName(hFrameWin,iCurMode);
				       break;
				  case IDC_INCOMECALL:
					   CallRingSetWindow(hFrameWin,iCurMode,INCOMCALL);//edit the ring for incoming call
					   break;
				  case IDC_MESSAGE:
					   CallRingSetWindow(hFrameWin,iCurMode,MESSAGES);//edit the ring for message
					   break;
				  case IDC_TMESSAGE:
					   CallRingSetWindow(hFrameWin,iCurMode,TMESSAGES);//edit the ring for telematic message
				       break;
				  case IDC_ACLOCK:
					   CallRingSetWindow(hFrameWin,iCurMode,ALARMCLK);//edit the ring of alarm clock
					   break;
				  case IDC_CALENDAR:
					   CallRingSetWindow(hFrameWin,iCurMode,CALENDAR);//edit the ring of calendar alarm
					   break;
				  case IDC_NOTIFY:
                       CallLevelAdjuster(hFrameWin, hWnd,LAS_NOTIFY,nCurVol,CALLBACK_SETVOL, ML("Notifications"));
					   break;
				  case IDC_MISSEVENT:
					   CallMissedEvent(hFrameWin,iCurMode);
					   break;
				  case IDC_KTONE:
                       CallLevelAdjuster(hFrameWin, hWnd,LAS_KEY,nCurKeyVol,CALLBACK_SETKEYVOL, ML("Key tones"));
					   break;
				  case IDC_VIBRA:
                       CallVibraSettingWnd(hFrameWin,iCurMode);
					   break;
				  case IDC_ALERTGRP:
                       CallAlertGrp(hFrameWin,iCurMode);
					   break;
				 }
				 break;
			 case IDC_NAME:
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
				    iButtonJust = IDC_NAME;
				 break;
			 case IDC_INCOMECALL:
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
				    iButtonJust = IDC_INCOMECALL;
				 break;
			 case IDC_MESSAGE:
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
				    iButtonJust = IDC_MESSAGE;
				 break;
			 case IDC_TMESSAGE:
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
				    iButtonJust = IDC_TMESSAGE;
				 break;
			 case IDC_ACLOCK:
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
    				 iButtonJust = IDC_ACLOCK;
				 break;
			 case IDC_CALENDAR:
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
    				 iButtonJust = IDC_CALENDAR;
				 break;
			 case IDC_NOTIFY:
			    if(HIWORD( wParam ) == SSBN_CHANGE)
                {
				    nCurVol = SendMessage(hNotify,SSBM_GETCURSEL,0,0);
                    sm.iNotification = nCurVol;
                    SetSM(&sm, iCurMode);
                }
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
    				 iButtonJust = IDC_NOTIFY;
				 break;
			 case IDC_MISSEVENT:
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
    				 iButtonJust = IDC_MISSEVENT;
				 break;
			 case IDC_KTONE:
			    if(HIWORD( wParam ) == SSBN_CHANGE)
                {
				    nCurKeyVol = SendMessage(hKTone,SSBM_GETCURSEL,0,0);
                    sm.iKeyVolume = nCurKeyVol;
                    SetSM(&sm, iCurMode);
                }
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
    				 iButtonJust = IDC_KTONE;
				 break;
			 case IDC_VIBRA:
			    if(HIWORD( wParam ) == SSBN_CHANGE)
			    {
				    iCurIndex = SendMessage(hVibra,SSBM_GETCURSEL,0,0);
                    SetProfile_Vibra(iCurMode, iCurIndex);
					if (iCurIndex == 0)//vibra on
					{
/*
						Sett_MakeVibra(1);
						SetTimer(hWnd, TIMER_VIBRA, SETT_VIBRA_TIME, NULL);
*/
					}
                }
				 iButtonJust = IDC_VIBRA;
				 break;
			 case IDC_ALERTGRP:
                 if(HIWORD(wParam) == SSBN_SETFOCUS)
    				 iButtonJust = IDC_ALERTGRP;
				 break;
			 }
		     break;	
         case CALLBACK_SETVOL:
             if(1 == wParam)            //save and return 
             {
                 nCurVol = lParam;
                 sm.iNotification = nCurVol;
                 SetSM(&sm,iCurMode);
                 SendMessage(hNotify,SSBM_SETCURSEL,nCurVol,0);
             }
             else
             {
                 nCurVol = SendMessage(hNotify, SSBM_GETCURSEL, 0, 0);
             }
             break;
         case CALLBACK_SETKEYVOL:
             if(1 == wParam)
             {
                 nCurKeyVol = lParam;
                 sm.iKeyVolume = nCurKeyVol;
                 SetSM(&sm,iCurMode);
                 SendMessage(hKTone,SSBM_SETCURSEL,nCurKeyVol,0);
             }
             break;
             
         default :
              lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
              break;
	}
return lResult;
}
static void SetProfile_Vibra(int iMode,SWITCHTYPE iCurIndex)
{
    sm.iVibraSwitch = (SWITCHTYPE)iCurIndex;
    SetSM(&sm, iMode);
}

static BOOL		CreateControl (HWND hWnd, HWND *hName,HWND *hIncomeCall,HWND *hMessage,HWND *hTMessage,HWND *hAClock,HWND *hCalendar,HWND *hNotify,HWND *hMissEvent,HWND *hKTone,HWND *hVibra,HWND *hAlertGrp)
{
    int iControlH,iControlW;
	int xzero=0,yzero=0;

	RECT    rect;
    GetClientRect(hWnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
/************************************************************************/

	* hName = CreateWindow( "SPINBOXEX", ML("Name"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hWnd, (HMENU)IDC_NAME, NULL, NULL);

    if (* hName == NULL)
        return FALSE;  
	
	* hIncomeCall = CreateWindow( "SPINBOXEX", ML("Incoming call"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 1, iControlW, iControlH, 
        hWnd, (HMENU)IDC_INCOMECALL, NULL, NULL);

    if (* hIncomeCall == NULL)
        return FALSE;  

	* hMessage = CreateWindow( "SPINBOXEX", ML("Messages"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 2, iControlW, iControlH, 
        hWnd, (HMENU)IDC_MESSAGE, NULL, NULL);

    if (* hMessage == NULL)
        return FALSE;  

	* hTMessage = CreateWindow( "SPINBOXEX", ML("Telematic messages"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 3, iControlW, iControlH, 
        hWnd, (HMENU)IDC_TMESSAGE, NULL, NULL);

    if (* hTMessage == NULL)
        return FALSE;  

	* hAClock = CreateWindow( "SPINBOXEX", ML("Alarm clock"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 4, iControlW, iControlH, 
        hWnd, (HMENU)IDC_ACLOCK, NULL, NULL);

    if (* hAClock == NULL)
        return FALSE;  

	* hCalendar = CreateWindow( "SPINBOXEX", ML("Calendar"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 5, iControlW, iControlH, 
        hWnd, (HMENU)IDC_CALENDAR, NULL, NULL);

    if (* hCalendar == NULL)
        return FALSE;  

	* hNotify = CreateWindow( "LEVIND", ML("Notifications"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 6, iControlW, iControlH, 
        hWnd, (HMENU)IDC_NOTIFY, NULL, (PVOID)LAS_VOL);

    if (* hNotify == NULL)
        return FALSE;  

	* hMissEvent = CreateWindow( "SPINBOXEX", ML("Missed events"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 7, iControlW, iControlH, 
        hWnd, (HMENU)IDC_MISSEVENT, NULL, NULL);

    if (* hMissEvent == NULL)
        return FALSE;  

	* hKTone = CreateWindow( "LEVIND", ML("Key tones"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 8, iControlW, iControlH, 
        hWnd, (HMENU)IDC_KTONE, NULL,(PVOID)LAS_VOL);

    if (* hKTone == NULL)
        return FALSE;  

	* hVibra = CreateWindow( "SPINBOXEX", ML("Vibration"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 9, iControlW, iControlH, 
        hWnd, (HMENU)IDC_VIBRA, NULL, NULL);

    if (* hVibra == NULL)
        return FALSE;  

    SendMessage(* hVibra, SSBM_ADDSTRING, 0, (LPARAM)ML("On"));
    SendMessage(* hVibra, SSBM_ADDSTRING, 0, (LPARAM)ML("Off"));

	* hAlertGrp = CreateWindow( "SPINBOXEX", ML("Alerting groups"),
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 10, iControlW, iControlH, 
        hWnd, (HMENU)IDC_ALERTGRP, NULL, NULL);

    if (* hAlertGrp == NULL)
        return FALSE;  


/************************************************************************/
  return TRUE;
}

/*********************************************************************\
* Function        ProfileSet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void ProfileSet_InitVScrolls(HWND hWnd,int iItemNum)
{
    static SCROLLINFO   vsi;

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
/*********************************************************************\
* Function        ProfileSet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/

static void ProfileSet_OnVScroll(HWND hWnd,  UINT wParam)
{
    static int  nY;
    static RECT rcClient;
    static SCROLLINFO      vsi;

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
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus)
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
//		case TIMER_VIBRA:
//			KillTimer(hWnd, TIMER_VIBRA);
//			Sett_MakeVibra(0);
//			break;
        default:
			KillTimer(hWnd, wParam);
            break;
		}

}

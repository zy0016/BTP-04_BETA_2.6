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
 * Autor    :
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallSelZoneUsedWindow.h"
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
#include    "me_wnd.h"
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "setting.h"
#include    "setup.h"
#include    "mullang.h"

static HWND hFrameWin;
static HWND hZoneSel;
static HWND hHomeZone;
static HWND hVisitZone;

static char * pClassName = "TimeZoneSelectClass";
static char * pHZClassName = "HomeZoneEditClass";
static char * pVZClassName = "VisitZoneEditClass";
static const char* pTimeStr = "%02d/%02d/%02d,%02d:%02d:00";
static LRESULT  ZS_AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  HZ_AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  VZ_AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

static BOOL CreateControl (HWND hWnd, HWND * hTimeZone);
static BOOL HZCreateControl (HWND hWnd, HWND * hHomeZone);
static BOOL VZCreateControl (HWND hWnd, HWND * hVisitZone);

extern void TimeZoneStrToSysTime(SYSTEMTIME * st, const char * szTimeZone);
extern const char* GetHomeZoneStr(void);
extern const char* GetVisitZoneStr(void);
extern void SaveHomeZoneStr(const char* szHomeStr);//szHomeStr : "UTC +/- XX:XX"
extern void SaveVisitZoneStr(const char* szVisitStr); //szVisitStr : "UTC +/- XX:XX"
void SetUsingTimeZone(TIMEZONE tz, SYSTEMTIME *st);
void GetNewZoneTime(SYSTEMTIME *st_old,const char *szNewZone,const char *szOldZone);

BOOL CallSelZoneUsedWindow(HWND hFrame)
{
	WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hFrame;
    
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = ZS_AppWndProc;
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

    hZoneSel = CreateWindow(pClassName,
        NULL, 
        WS_VISIBLE | WS_CHILD,                    
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin,
        NULL, NULL, NULL);

    if (NULL == hZoneSel)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");      

    SetWindowText(hFrameWin, ML("Time zone in use"));
    SetFocus(hZoneSel);


    return (TRUE);    
}

static LRESULT  ZS_AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
	static  int     iIndex=0,i;
    static  HWND    hTimeZone;
    static int iCurIndex;    
    static  SYSTEMTIME  tzs_st;
    int meRet;
    char cdate[20] = "";
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {
    case WM_CREATE :
        CreateControl (hWnd, &hTimeZone);
        
        hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        ReleaseDC(hWnd,hdc);
        
        iIndex = GetUsingTimeZone();
        SendMessage(hTimeZone,LB_SETCURSEL,iIndex,0);
        Load_Icon_SetupList(hTimeZone,hIconNormal,hIconSel,2,iIndex);
        break;
        
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");   
        SetWindowText(hFrameWin, ML("Time zone in use"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        break;
        
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_TIMEZONE));
        break;
        
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {            
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_OK,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;  
        
    case WM_DESTROY : 
        DeleteObject(hIconNormal);
        DeleteObject(hIconSel);
        hZoneSel = NULL;
        UnregisterClass(pClassName,NULL);
        break;
            
    case WM_COMMAND :
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_OK:   
            iIndex = SendMessage(hTimeZone,LB_GETCURSEL,0,0);//get the current selected item number
            Load_Icon_SetupList(hTimeZone,hIconNormal,hIconSel,2,iIndex);//change the radio button of the current selected item		
            SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
            break;
        }
        break;
        
    case WM_TIMER:
        switch(wParam)
        {
        case TIMER_ASURE:
            KillTimer(hWnd, TIMER_ASURE);

			if (GetUsingTimeZone() == iIndex)		//re-saving the same setting info;
			{
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			}
            memset(&tzs_st,0x00,sizeof(SYSTEMTIME));
            
            GetLocalTime(&tzs_st);
            SetUsingTimeZone(iIndex,&tzs_st);
            sprintf(cdate,pTimeStr,//"%02d/%02d/%02d,%02d:%02d:00"
                tzs_st.wYear%100,tzs_st.wMonth,tzs_st.wDay,tzs_st.wHour,tzs_st.wMinute);

            meRet = ME_SetClockTime(hWnd,CALLBACK_SETDATE,cdate);//set the wierless module first
            if(meRet < 0)
			{
				//PostMessage(hWnd,CALLBACK_SETDATE,0,0);// only for test in emulator
				PLXTipsWin(NULL, NULL, NULL, "Failed to set time", NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
			}
            break;
        }
        break;
        
    case CALLBACK_SETDATE:        
        SetLocalTime (&tzs_st);
        DlmNotify((WPARAM)PS_SETTIME, 0);        
        SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        
        break;
    
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    }
    return lResult;
    
}
static BOOL CreateControl (HWND hWnd, HWND * hTimeZone)
{

    RECT rect;
    GetClientRect(hWnd, &rect);

    * hTimeZone = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right - rect.left, rect.bottom - rect.top,
        hWnd, (HMENU)IDC_TIMEZONE, NULL, NULL);
    if (* hTimeZone == NULL )
        return FALSE;

	SendMessage(* hTimeZone,LB_ADDSTRING,TF_24,(LPARAM)ML("Home"));
	SendMessage(* hTimeZone,LB_ADDSTRING,TF_12,(LPARAM)ML("Visiting"));

   return TRUE;
    
}







BOOL CallEditHomeZoneWindow(HWND hFrame)
{
	WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hFrame;
    
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = HZ_AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pHZClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hHomeZone = CreateWindow(pHZClassName,
        NULL, 
        WS_VISIBLE | WS_CHILD,                    
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin,
        NULL, NULL, NULL);

    if (NULL == hHomeZone)
    {
        UnregisterClass(pHZClassName,NULL);
        return FALSE;
    }
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, // set the right soft key
                (WPARAM)IDC_BUTTON_QUIT, (LPARAM)ML("Cancel"));
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, //set the left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1), (LPARAM)ML("Save")) ;
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SetWindowText(hFrameWin, ML("Home"));
    SetFocus(hHomeZone);


    return (TRUE);    

}
static LRESULT HZ_AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HWND    hFocus,hHome;
    int     nHour, nMin;
    static  SYSTEMTIME  st, st_Time;
	char	cdate [20] = "";
    static  char    szHomeZone[11] = "";      
    HWND    hCurWin = 0;
    LRESULT lResult;
    int meRet;
	lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
    case WM_CREATE :
        HZCreateControl(hWnd, &hHome);
		memset(szHomeZone, 0, sizeof(szHomeZone));
        hFocus = hHome;
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Cancel"));           
        
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)ML("Save"));//set the left soft key
            
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        
        SetWindowText(hFrameWin, ML("Home"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        break; 

    case WM_DESTROY ://application quit;
        hHomeZone = NULL;
        UnregisterClass(pHZClassName,NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {       
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;  
        
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET://save the time

            SendMessage(hHome, UEM_GETDATA, 0, (LPARAM)&st);
            nHour = (SHORT)st.wHour;
            nMin  = (SHORT)st.wMinute;
printf("\r\n Get the Hour & Minutes from editor is %02d:%02d", nHour, nMin);            
            if ( (nHour >= 0) && (nMin >= 0) )
            {
				memset(szHomeZone, 0, sizeof(szHomeZone));
                sprintf(szHomeZone,"UTC+%02d:%02d",nHour,nMin);      
            }            
            
            if ( (nHour < 0) || (nMin < 0) )
            {
				memset(szHomeZone, 0, sizeof(szHomeZone));
                sprintf(szHomeZone,"UTC-%02d:%02d",abs(nHour), abs(nMin)); 
            }   
			
			if (GetUsingTimeZone() == TZ_HOME)
			{
				GetLocalTime(&st_Time);
				GetNewZoneTime(&st_Time, szHomeZone, GetHomeZoneStr());	
  			    sprintf(cdate,pTimeStr,//"%02d/%02d/%02d,%02d:%02d:00"
                           st_Time.wYear%100,st_Time.wMonth,st_Time.wDay,st_Time.wHour,st_Time.wMinute);
				meRet = ME_SetClockTime(hWnd,CALLBACK_SETDATE,cdate);//set the wierless module first
				if(meRet < 0)
				{
					PLXTipsWin(NULL, NULL, NULL, "Failed to set time", NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
				}
				break;
			}
            SaveHomeZoneStr(szHomeZone);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }  
        break;

	case CALLBACK_SETDATE:        
		SaveHomeZoneStr(szHomeZone);
        SetLocalTime (&st_Time);
        DlmNotify((WPARAM)PS_SETTIME, 0);        
        SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
        PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
        
    default :        
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static BOOL HZCreateControl(HWND hWnd, HWND * hHome)
{
    SYSTEMTIME st;

    memset((void*)&st, 0, sizeof(SYSTEMTIME));

    TimeZoneStrToSysTime(&st, GetHomeZoneStr());
    
    * hHome = CreateWindow(
        "UNITEDIT",
        ML("Home zonecolon"),
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | UES_TIMEZONE | UES_TITLE,
        1,
        3,
        169,
        47,
        hWnd,
        (HMENU)ID_HOMEZONE,
        NULL,
        (LPVOID)&st);

    if(* hHome == NULL)
        return FALSE;

    return TRUE;
    
}






BOOL CallEditVisitZoneWindow(HWND hFrame)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hFrame;
    
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = VZ_AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pVZClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    hVisitZone = CreateWindow(pVZClassName,
        NULL, 
        WS_VISIBLE | WS_CHILD,                    
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin,
        NULL, NULL, NULL);

    if (NULL == hVisitZone)
    {
        UnregisterClass(pVZClassName,NULL);
        return FALSE;
    }
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, // set the right soft key
                (WPARAM)IDC_BUTTON_QUIT, (LPARAM)ML("Cancel"));
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, //set the left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1), (LPARAM)ML("Save")) ;
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SetWindowText(hFrameWin, ML("Visiting"));
    SetFocus(hVisitZone);


    return (TRUE);    

}
static LRESULT VZ_AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HWND    hFocus,hVisit;
    static  int  meRet;
    int nMin,nHour;
    static  SYSTEMTIME  st,st_Time;
    static  char    szVisitZone[20] = "";      
    char cdate[20] = "";
    HWND    hCurWin = 0;
    LRESULT lResult;
  
	lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
    case WM_CREATE :
        VZCreateControl(hWnd, &hVisit);
		memset(szVisitZone, 0, sizeof(szVisitZone));
        hFocus = hVisit;
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Cancel"));           
        
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)ML("Save"));//set the left soft key
            
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        
        SetWindowText(hFrameWin, ML("Visiting"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        break; 

    case WM_DESTROY ://application quit;
        hVisitZone = NULL;
        UnregisterClass(pVZClassName,NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {       
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_RETURN:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;  
        
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET://save the time
            SendMessage(hVisit, UEM_GETDATA, 0, (LPARAM)&st);
            nHour = (SHORT)st.wHour;
            nMin  = (SHORT)st.wMinute;
            
            if ( (nHour >= 0) && (nMin >= 0) )
            {
                sprintf(szVisitZone,"UTC+%02d:%02d",nHour,nMin);      
            }            
            
            if ( (nHour < 0) || (nMin < 0) )
            {
                sprintf(szVisitZone,"UTC-%02d:%02d", abs(nHour), abs(nMin)); 
            }       

			if (GetUsingTimeZone() == TZ_VISIT)
			{
				GetLocalTime(&st_Time);
				GetNewZoneTime(&st_Time, szVisitZone, GetVisitZoneStr());	
  			    sprintf(cdate,pTimeStr,//"%02d/%02d/%02d,%02d:%02d:00"
                           st_Time.wYear%100,st_Time.wMonth,st_Time.wDay,st_Time.wHour,st_Time.wMinute);
				meRet = ME_SetClockTime(hWnd,CALLBACK_SETDATE,cdate);//set the wierless module first
				if(meRet < 0)
				{
					PLXTipsWin(NULL, NULL, NULL, "Failed to set time", NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
				}
				break;
			}

            SaveVisitZoneStr(szVisitZone);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        }  
        break;

	case CALLBACK_SETDATE:        
		SaveVisitZoneStr(szVisitZone);
        SetLocalTime (&st_Time);
        DlmNotify((WPARAM)PS_SETTIME, 0);        
        SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
        PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
        
    default :        
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
static BOOL VZCreateControl(HWND hWnd, HWND * hVisit)
{
    SYSTEMTIME st;

    memset((void*)&st, 0, sizeof(SYSTEMTIME));

    TimeZoneStrToSysTime(&st, GetVisitZoneStr());

    * hVisit = CreateWindow(
        "UNITEDIT",
        ML("Visiting zonecolon"),
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | UES_TIMEZONE | UES_TITLE,
        1,
        3,
        169,
        47,
        hWnd,
        (HMENU)ID_VISITZONE,
        NULL,
        (LPVOID)&st);

    if(* hVisit == NULL)
        return FALSE;

    return TRUE;
    
}

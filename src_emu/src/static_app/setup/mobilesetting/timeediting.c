 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : time setting / edit time
 *
 * Purpose  : create the date and time edit window
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "timeediting.h"
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

static const char * pTimeStr   = "%02d/%02d/%02d,%02d:%02d:00";

#define SETT_MAX_YEAR 2037
#define SETT_MIN_YEAR 2000

static  LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static  LRESULT DateAppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static  BOOL    TimeCreateControl(HWND hwnd,HWND * hTime);
static  BOOL    DateCreateControl(HWND hwnd,HWND * hDate);
static	int		GetDay(int iYear,int iMonth);

static  HWND    hFrameWin = NULL;

static  HWND    hTimeWnd = NULL;
static  HWND    hDateWnd = NULL;
/***************************************************************
* Function  TimeEditing
* Purpose   register the class and create the main window     
* Params
* Return    TRUE: Success; FALSE: Failed
* Remarks
***************************************************************/
BOOL TimeEditing(HWND hwndCall)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hwndCall;

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
    
    hTimeWnd = CreateWindow(
        pClassName, 
        NULL,
        WS_VISIBLE | WS_CHILD,                    
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin,
        NULL,
        NULL, 
        NULL);
		
		if (NULL == hTimeWnd)
		{
			UnregisterClass(pClassName,NULL);
			return FALSE;
		}
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, // set the right soft key
                (WPARAM)IDC_BUTTON_QUIT, (LPARAM)ML("Cancel"));
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, //set the left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1), (LPARAM)ML("Save")) ;
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    SetWindowText(hFrameWin, ML("Date and time"));
    SetFocus(hTimeWnd);
    return (TRUE);
}
/***************************************************************
* Function  application window processing
* Purpose        
* Params 
* Return    
* Remarks
***************************************************************/
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HWND    hFocus,hTime;
    static  int  meRet;
    static  SYSTEMTIME  st;
	static  SYSTEMTIME  systime;
	int iret;
    char    cdate[20] = "";      
    HWND    hCurWin = 0;
    LRESULT lResult;
  
	lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
    case WM_CREATE :
        TimeCreateControl(hWnd, &hTime);
        hFocus = hTime;
        break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Cancel"));
            
        
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)ML("Save"));//set the left soft key
            
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
        
        SetWindowText(hFrameWin, ML("Date and time"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        break; 

    case WM_DESTROY ://application quit;
		KillTimer(hWnd, TIMER_ID);
        hTimeWnd = NULL;
        UnregisterClass(pClassName,NULL);
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
        }
        break;  
        
	    case WM_COMMAND:
             switch( LOWORD( wParam ))
			 {
              case IDC_BUTTON_SET://save the time
                   memset(&systime,0x00,sizeof(SYSTEMTIME));
				   memset(&st,0x00,sizeof(SYSTEMTIME));

				   iret = SendMessage(hTime, TEM_GETTIME, 0, (LPARAM)&st);
				   if (!iret)
					   break;
                   GetLocalTime(&systime);
				   st.wDay = systime.wDay;
				   st.wMonth = systime.wMonth;
				   st.wYear = systime.wYear;

				   sprintf(cdate,pTimeStr,//"%02d/%02d/%02d,%02d:%02d:00"
                           st.wYear%100,st.wMonth,st.wDay,st.wHour,st.wMinute);
			       SetTimer(hWnd, TIMER_ID, ME_SETCLK_TIMEOUT, NULL);
                   printf("\r\nSettings----> Start to set ME time...%s\r\n", cdate);
				   meRet = ME_SetClockTime(hWnd,CALLBACK_SETDATE,cdate);//set the wierless module first
				   printf("\r\n ME_SetClockTime return value is %d", meRet);	   
				
                   if(meRet < 0)
				   {
					  ZC_PLXTipsWin(NULL, NULL, NULL, "Failed to set time", NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
					  SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
					  PostMessage(hWnd, WM_CLOSE, 0, 0);
				   }
				   break;
			  case IDC_BUTTON_QUIT://quit
                   DestroyWindow( hWnd );  
                   DestroyWindow(hFocus); 
				   break; 
			 }  
			 break;

		case CALLBACK_SETDATE:
			KillTimer(hWnd, TIMER_ID);
			SetLocalTime (&st);
            DlmNotify((WPARAM)PS_SETTIME, 0);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;

	    case WM_TIMER:
			switch(wParam)
			{
			case TIMER_ID:
				KillTimer(hWnd, TIMER_ID);
				ZC_PLXTipsWin(NULL, NULL, NULL, "Set ME timeout", NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			default:
				break;
			}
			break;

/*
		case PWM_ACTIVATE:
			if (wParam == WA_INACTIVE)
			{
				KillTimer(hWnd, TIMER_ID);
			}
			break;
*/

         default :        
			 lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
             break;
	}
    return lResult;
}
/***************************************************************
* Function  TimeCreateControl
* Purpose   create the control components for time edit window    
* Params
* Return    TRUE; FALSE  
* Remarks
***************************************************************/
static  BOOL    TimeCreateControl(HWND hwnd,HWND * hTime)
{
	SYSTEMTIME st;
    int iControlH;
	RECT    rect;
	int     xzero=0,yzero=0;
    
	memset((void*)&st, 0, sizeof(SYSTEMTIME));

    GetClientRect(hwnd, &rect);
	iControlH = rect.bottom/3;

	GetLocalTime(&st);
    if (TF_24 == GetTimeFormt() )
    {
        * hTime = CreateWindow (           //create the 24HR "time" editor;  
            "TIMEEDIT",ML("Timecolon"), 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_TIME_24HR | TES_TITLE,
            xzero, yzero, rect.right - rect.left, iControlH,
            hwnd,(HMENU)IDC_TIME,NULL,(LPVOID)&st);
    }
    
    else
    {
        * hTime = CreateWindow (           //create the 12HR "time" editor;  
            "TIMEEDIT",ML("Timecolon"), 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_TIME_12HR | TES_TITLE,
            xzero, yzero, rect.right - rect.left, iControlH,
            hwnd,(HMENU)IDC_TIME,NULL,(LPVOID)&st);
    }

     if (* hTime == NULL)
         return FALSE;
	return TRUE;
}
/***************************************************************
* Function  DateEditing
* Purpose   register the class and create the main window     
* Params
* Return    TRUE: Success; FALSE: Failed
* Remarks
***************************************************************/
BOOL DateEditing(HWND hwndCall)
{
    WNDCLASS    wc;
    RECT rClient;

    hFrameWin = hwndCall;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = DateAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pDateClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);
    
    hDateWnd = CreateWindow(
			pDateClassName, 
			NULL,
            WS_VISIBLE | WS_CHILD,                    
            rClient.left,
            rClient.top,
            rClient.right-rClient.left,
            rClient.bottom-rClient.top,
            hFrameWin,
			NULL,
			NULL, 
			NULL
			);
		
    if (NULL == hDateWnd)
    {
        UnregisterClass(pDateClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, // set the right soft key
        (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
    
    SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, //set the left soft key
        MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)ML("Save") ) ;
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");   
    
    SetWindowText(hFrameWin, ML("Date and time"));
    SetFocus(hDateWnd);   

    return (TRUE);
}
/***************************************************************
* Function  application window processing
* Purpose        
* Params
* Return    
* Remarks
***************************************************************/
static LRESULT DateAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  HWND    hFocus,hDate;
    static  int meRet;
    static  SYSTEMTIME  st;
	static  SYSTEMTIME  systime;
    char    cdate[20] = "";      
    HWND    hCurWin = 0;
	int iret;
    LRESULT lResult;
  
	lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
		case WM_CREATE :
			 DateCreateControl(hWnd, &hDate);
             hFocus = hDate;
		     break;
        case WM_SETFOCUS:
             SetFocus(hFocus);
             break;
        
        case PWM_SHOWWINDOW:
            SetFocus(hWnd);   
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, // set the right soft key
                (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );
            
            SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, //set the left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)ML("Save") ) ;
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");   
            
            SetWindowText(hFrameWin, ML("Date and time"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            break;
        case WM_DESTROY ://application quit;
			 KillTimer(hWnd, TIMER_ID);
			 hDateWnd = NULL;
		     UnregisterClass(pDateClassName,NULL);
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
                   memset(&systime,0x00,sizeof(SYSTEMTIME));
				   memset(&st,0x00,sizeof(SYSTEMTIME));

				   iret = SendMessage(hDate, TEM_GETTIME, 0, (LPARAM)&st);
				   if (!iret)
				   {
					   PLXTipsWin(NULL, NULL, NULL, ML("Not valid date"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
					   break;
				   }
                   if (st.wYear > SETT_MAX_YEAR || st.wYear < SETT_MIN_YEAR)
				   {
					   PLXTipsWin(NULL, NULL, NULL, ML("Please reset year"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
					   break;
				   }
                   GetLocalTime(&systime);
                   st.wHour = systime.wHour;
                   st.wMinute = systime.wMinute;


				   sprintf(cdate,pTimeStr,//"%02d/%02d/%02d,%02d:%02d:00"
                           st.wYear%100,st.wMonth,st.wDay,st.wHour,st.wMinute);
					
			       SetTimer(hWnd, TIMER_ID, ME_SETCLK_TIMEOUT, NULL);
                   printf("\r\nSettings----> Start to set ME time...%s\r\n", cdate);
				   meRet = ME_SetClockTime(hWnd,CALLBACK_SETDATE,cdate);//to set the wireless module first
				   printf("\r\n ME_SetClockTime return value is %d", meRet);	   
                   if(meRet < 0) //Me set time failed
                   {
					   ZC_PLXTipsWin(NULL, NULL, NULL, "Failed to set time", NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
					   SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
					   PostMessage(hWnd, WM_CLOSE, 0, 0);
				   }

				   break;
			  case IDC_BUTTON_QUIT://quit
                   DestroyWindow( hWnd );  
                   DestroyWindow(hFocus); 
				   break; 
			 }  
			 break;
        case CALLBACK_SETDATE:
			KillTimer(hWnd, TIMER_ID);
            SetLocalTime (&st);
            DlmNotify((WPARAM)PS_SETTIME, 0);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
	    case WM_TIMER:
			switch(wParam)
			{
			case TIMER_ID:
				KillTimer(hWnd, TIMER_ID);
				ZC_PLXTipsWin(NULL, NULL, NULL, "Set ME timeout", NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
				PostMessage(hWnd, WM_CLOSE, 0, 0);
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
/***************************************************************
* Function  DateCreateControl
* Purpose   create the control components for date edit window    
* Params
* Return    TRUE; FALSE  
* Remarks
***************************************************************/
static  BOOL    DateCreateControl(HWND hwnd,HWND * hDate)
{
	SYSTEMTIME st;
    int iControlH;
	RECT    rect;
	int     xzero=0,yzero=0;
    DATEFORMAT dateformat;
	memset((void*)&st, 0, sizeof(SYSTEMTIME));

    GetClientRect(hwnd, &rect);
	iControlH = rect.bottom/3;


	SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, // right soft key
                (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );

    SendMessage(hwnd , PWM_CREATECAPTIONBUTTON, //left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)ML("Save") ) ;
	dateformat = GetDateFormt();
	GetLocalTime(&st);
    if( dateformat == DF_DMY_DOT || dateformat == DF_DMY_SLD || dateformat == DF_DMY_DSH )
     * hDate = CreateWindow (           //create "dd mm yyyy" date editor;  
        "TIMEEDIT",ML("Datecolon"), 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_DATE_DMY | TES_TITLE,
        xzero, yzero, rect.right - rect.left, iControlH,
        hwnd,(HMENU)IDC_DATE,NULL,(LPVOID)&st);

    if( dateformat == DF_MDY_DOT || dateformat == DF_MDY_SLD || dateformat == DF_MDY_DSH )
     * hDate = CreateWindow (           //create "mm dd yyyy" date editor;
        "TIMEEDIT",ML("Datecolon"), 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_DATE_MDY | TES_TITLE,
        xzero, yzero, rect.right - rect.left, iControlH,
        hwnd,(HMENU)IDC_DATE,NULL,(LPVOID)&st);

    if( dateformat == DF_YMD_DOT || dateformat == DF_YMD_SLD || dateformat == DF_YMD_DSH )
     * hDate = CreateWindow (           //create "yyyy mm dd" date editor;
        "TIMEEDIT",ML("Datecolon"), 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | TES_DATE_YMD | TES_TITLE,
        xzero, yzero, rect.right - rect.left, iControlH,
        hwnd,(HMENU)IDC_DATE,NULL,(LPVOID)&st);

      if (* hDate == NULL)
          return FALSE;

	return TRUE;
}
/*==========================================================================
*   Function:  static int GetDay ( int iYear, int iMonth )
*   Purpose :  Get the number of days of the specified month of year;
*   Argument:  int iYear, int iMonth.
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/

static int GetDay(int iYear,int iMonth)
{
    int iMaxDay;

    if(iMonth == 4 || iMonth == 6 || iMonth == 9 || iMonth == 11) 
        iMaxDay = 30;
    else if(iMonth != 2)
        iMaxDay = 31;
    else//iMonth == 2
    {
        if((iYear % 4 == 0 && iYear % 100 != 0) || (iYear % 400 == 0)) 
            iMaxDay = 29;
        else
            iMaxDay = 28;
    }
    return iMaxDay;
}

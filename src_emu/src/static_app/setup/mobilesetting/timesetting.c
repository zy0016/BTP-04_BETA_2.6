/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting / setting time format
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
#include    "timesetting.h"
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
#include	"assert.h"


#define SETT_REFRESH_TIME	60000
static const char * pClassName = "TimeSettingWndClass";
static  HWND    hWndApp=NULL;
static  HWND    hFrameWin=NULL;
static SYSTEMTIME st;
static int  nCurFocus;
static BOOL iRefFlag = TRUE;

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static int iTimeSrcFlag;


static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hwnd,HWND * hDateOn,HWND * hDateForm,HWND * hTimeForm,HWND * hDateSepa,HWND * hTimeSepa, HWND * hGPS);
static  void    TimeSet_InitVScrolls(HWND hWnd,int iItemNum);
static  void    TimeSet_OnVScroll(HWND hWnd,  UINT wParam);

BOOL   SetDateFormt(DATEFORMAT df_format);
BOOL   SetTimeFormt(TIMEFORMAT tf_format);
BOOL   SetTimeSource(TIMESOURCE timesource);

extern BOOL CallDateOnOptions(HWND hwndCall);
extern BOOL CallDateFormat(HWND hwndCall);
extern BOOL CallTimeFormat(HWND hwndCall);
extern BOOL TimeEditing(HWND hwndCall);
extern BOOL DateEditing(HWND hwndCall);
extern BOOL CallTimeZone(HWND hwndApp);

extern const char* GetHomeZoneStr(void);
extern const char* GetVisitZoneStr(void);

void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);


BOOL TimeSetting(HWND hwndCall)
{
    WNDCLASS    wc;
    RECT rClient;
    hFrameWin = hwndCall;

	if (hWndApp)
		SendMessage(hWndApp, WM_CLOSE, 0, 0);

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

    hWndApp = CreateWindow(pClassName,NULL, 
                WS_VISIBLE | WS_CHILD | WS_VSCROLL,                    
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin,
                NULL, NULL, NULL);

    if (NULL == hWndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
	
	SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
	SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);   
	SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Back") );                
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SetWindowText(hFrameWin, ML("Date and time"));
    SetFocus(hWndApp);

    return (TRUE);
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HWND        hTimeSource, hTime, hTimeZone, hTimeFormat, hDate, hDateFormat;
    static      HWND        hFocus;
    static      int         iButtonJust;
    static      int         iIndex,iCurIndex;
    static      char        szTimeZone[50]="";
	LRESULT     lResult;
    static      HWND hWndFocus;
	static char szDate[11];
	static char szTime[6];
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
	{
		case WM_CREATE :
			memset(szDate, 0, sizeof(szDate));
			memset(szTime, 0, sizeof(szTime));
            CreateControl(hWnd, &hTimeSource,&hTime, &hTimeZone, &hTimeFormat, &hDate, &hDateFormat);
			hFocus=hTimeSource;
			
            iIndex = GetDateFormt();
			SendMessage(hDateFormat,SSBM_SETCURSEL,(WPARAM)(iIndex),0);
			iIndex = GetTimeFormt();
			SendMessage(hTimeFormat,SSBM_SETCURSEL,(WPARAM)(iIndex),0);
            iIndex = GetTimeSource();
			SendMessage(hTimeSource,SSBM_SETCURSEL,(WPARAM)(iIndex),0);
            iIndex = GetUsingTimeZone();

			memset(szTimeZone, 0, sizeof(szTimeZone));
            if(iIndex == TZ_HOME)
                sprintf(szTimeZone,"%s(%s)",ML("Home"), GetHomeZoneStr());
            else if(iIndex == TZ_VISIT)
                sprintf(szTimeZone,"%s(%s)",ML("Visiting"), GetVisitZoneStr());

            SendMessage(hTimeZone,SSBM_ADDSTRING,0,(LPARAM)szTimeZone);
 
            GetLocalTime(&st);
            GetTimeDisplay(st,szTime,szDate);
            SendMessage(hDate,SSBM_ADDSTRING,0,(LPARAM)szDate);
            SendMessage(hTime,SSBM_ADDSTRING,0,(LPARAM)szTime);

            SetTimer(hWnd, TIMER_REFRESH, SETT_REFRESH_TIME, NULL);
			TimeSet_InitVScrolls(hWnd,MAX_ITEMNUM);
        	break;

        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),0);   
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Back"));                
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            SetWindowText(hFrameWin, ML("Date and time"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
			
            iIndex = GetDateFormt();
			SendMessage(hDateFormat,SSBM_SETCURSEL,(WPARAM)(iIndex),0);
			iIndex = GetTimeFormt();
			SendMessage(hTimeFormat,SSBM_SETCURSEL,(WPARAM)(iIndex),0);
            iIndex = GetTimeSource();
			SendMessage(hTimeSource,SSBM_SETCURSEL,(WPARAM)(iIndex),0);
            
            iIndex = GetUsingTimeZone();
            SendMessage(hTimeZone,SSBM_DELETESTRING,0,0);
			memset(szTimeZone, 0, sizeof(szTimeZone));
            if(iIndex == TZ_HOME)
                sprintf(szTimeZone,"%s(%s)",ML("Home"), GetHomeZoneStr());
            else if(iIndex == TZ_VISIT)
                sprintf(szTimeZone,"%s(%s)",ML("Visiting"), GetVisitZoneStr());

            SendMessage(hTimeZone,SSBM_ADDSTRING,0,(LPARAM)szTimeZone);
 
            GetLocalTime(&st);
            GetTimeDisplay(st,szTime,szDate);
            SendMessage(hDate,SSBM_DELETESTRING,0,0);
            SendMessage(hTime,SSBM_DELETESTRING,0,0);
            SendMessage(hDate,SSBM_ADDSTRING,0,(LPARAM)szDate);
            SendMessage(hTime,SSBM_ADDSTRING,0,(LPARAM)szTime);
            iRefFlag = TRUE;
            break;

		case WM_SETFOCUS:
            SetFocus(hFocus);
            break;

        case WM_VSCROLL:
	    	TimeSet_OnVScroll(hWnd,wParam);
		    break;

	    case WM_DESTROY : 
            hWndApp = NULL;
            KillTimer(hWnd, TIMER_REFRESH);
            UnregisterClass(pClassName,NULL);
            break;
            
    	case WM_TIMER:
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
			
			case TIMER_REFRESH:
                if(iRefFlag)
                {
					KillTimer(hWnd, TIMER_REFRESH);
                    GetLocalTime(&st);
                    GetTimeDisplay(st,szTime,szDate);
                    SendMessage(hDate,SSBM_SETTEXT,0,(LPARAM)szDate);
                    SendMessage(hTime,SSBM_SETTEXT,0,(LPARAM)szTime);  
					SetTimer(hWnd, TIMER_REFRESH, SETT_REFRESH_TIME, NULL);
                }
                break;
	
			default:
				KillTimer(hWnd, wParam);
				break;
		    }
		    break;

			
		case PWM_ACTIVATE:
			if (wParam == WA_INACTIVE)
			{
				KillTimer(hWnd, TIMER_ID);
				RepeatFlag = 0;
				wKeyCode   = 0;
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
                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            case VK_F5:
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
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
            switch( LOWORD( wParam ))
		    {
            case IDC_BUTTON_SET:
                if(hFocus == hTimeSource)
                {
                    iRefFlag = FALSE;
                    CallDateOnOptions(hFrameWin);
                    break;
                }
                if(hFocus == hTime) 
                {
                    if( USER_DEFINED == GetTimeSource() )
                    {
                        iRefFlag = FALSE;
                        TimeEditing(hFrameWin);                              
                    }
                    else
                        PLXTipsWin(NULL, NULL, NULL,ML("Cannot editsnGPS time"),ML("Date and time"),Notify_Alert,ML("Ok"),0,WAITTIMEOUT);                    
                    break;
                }
                if(hFocus == hTimeZone)
                {
                    iRefFlag = FALSE;
                    CallTimeZone(hFrameWin);
                    break;
                }
                if(hFocus == hTimeFormat) 
                { 
                    iRefFlag = FALSE;
                    CallTimeFormat(hFrameWin);
                    break;
                }  
                if(hFocus == hDate)
                {
                    if( USER_DEFINED == GetTimeSource() )
                    {
                        iRefFlag = FALSE;
                        DateEditing(hFrameWin);                   
                    }
                    else
                        PLXTipsWin(NULL, NULL, NULL,ML("Cannot editsnGPS time"),ML("Date and time"),Notify_Failure,ML("Ok"),0,WAITTIMEOUT);                    
                    break;
                }
                if(hFocus == hDateFormat) 
                {
                    iRefFlag = FALSE;
                    CallDateFormat(hFrameWin);
                    break;                    
                }
                break;
            case IDC_TIMESRC:
                if(HIWORD( wParam ) == SSBN_CHANGE)
                {
                    iCurIndex = SendMessage(hTimeSource,SSBM_GETCURSEL,0,0);
                    SetTimeSource(iCurIndex);
                }   
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_TIMESRC;
                break;
                
            case IDC_TIME:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_TIME;
                break;
                
            case IDC_TIMEZONE:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_TIMEZONE;
                break;
                    
            case IDC_TIMEFORM:
                if(HIWORD( wParam ) == SSBN_CHANGE)
                {
                    iCurIndex = SendMessage(hTimeFormat,SSBM_GETCURSEL,0,0);
                    SetTimeFormt(iCurIndex);
                    GetLocalTime(&st);
                    GetTimeDisplay(st,szTime,szDate);
                    SendMessage(hTime,SSBM_SETTEXT,0,(LPARAM)szTime);  
                }
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_TIMEFORM;
                break;
                
            case IDC_DATE:
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_DATE;
                break;
                
            case IDC_DATEFORM:
                if(HIWORD( wParam ) == SSBN_CHANGE)
                {
                    iCurIndex = SendMessage(hDateFormat,SSBM_GETCURSEL,0,0);
                    SetDateFormt(iCurIndex);
                    GetLocalTime(&st);
                    GetTimeDisplay(st,szTime,szDate);
                    SendMessage(hDate,SSBM_SETTEXT,0,(LPARAM)szDate);
                }
                if(HIWORD(wParam) == SSBN_SETFOCUS)
                    iButtonJust = IDC_DATEFORM;
                break;
            }
		    break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}
	return lResult;
}

static  BOOL    CreateControl(HWND hwnd,HWND * hTimeSource,HWND * hTime,HWND * hTimeZone,HWND * hTimeFormat,HWND * hDate, HWND * hDateFormat)
{
	int     xzero=0,yzero=0,i;
    int     iControlH,iControlW;
	RECT    rect;
	const char * szTimeSource[2] =
	{
		ML("GPS"),
		ML("User defined")
	};
	const char * szTimeFormat[2] =
	{
		ML("12tohour"),
		ML("24tohour")
	};
    const char * szDateFormat[9] =
    {
        ML("dd.mm.yyyy"),
        ML("mm.dd.yyyy"),
        ML("yyyy.mm.dd"),
        ML("ddslmmslyyyy"),
        ML("mmslddslyyyy"),
        ML("yyyyslmmsldd"),
        ML("ddtommtoyyyy"),
        ML("mmtoddtoyyyy"),
        ML("yyyytommtodd")
    };
    GetClientRect(hwnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;


    * hTimeSource = CreateWindow( "SPINBOXEX", ML("Time source"), //
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hwnd, (HMENU)IDC_TIMESRC, NULL, NULL);

    if (* hTimeSource == NULL)
        return FALSE;

    * hTime = CreateWindow( "SPINBOXEX", ML("Time"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER */| WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH), iControlW, iControlH, 
        hwnd, (HMENU)IDC_TIME, NULL, NULL);

    if (* hTime == NULL)
        return FALSE;
   
	* hTimeZone = CreateWindow( "SPINBOXEX", ML("Time zone"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH)*2, iControlW, iControlH, 
        hwnd, (HMENU)IDC_TIMEZONE, NULL, NULL);

    if (* hTimeZone == NULL)
        return FALSE;

    * hTimeFormat = CreateWindow( "SPINBOXEX", ML("Time format"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 3, iControlW, iControlH, 
        hwnd, (HMENU)IDC_TIMEFORM, NULL, NULL);

    if (* hTimeFormat == NULL)
        return FALSE;

	* hDate = CreateWindow( "SPINBOXEX", ML("Date"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 4, iControlW, iControlH, 
        hwnd, (HMENU)IDC_DATE, NULL, NULL);

    if (* hDate == NULL)
        return FALSE;

	* hDateFormat = CreateWindow( "SPINBOXEX", ML("Date format"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+(iControlH) * 5, iControlW, iControlH, 
        hwnd, (HMENU)IDC_DATEFORM, NULL, NULL);

    if (* hDateFormat == NULL)
        return FALSE;    
    
	for(i=0; i<2; i++)
		{
		SendMessage(*hTimeSource, SSBM_ADDSTRING, i,(LPARAM)(szTimeSource[i]));
		SendMessage(*hTimeFormat, SSBM_ADDSTRING, i,(LPARAM)(szTimeFormat[i]));
		}
    for(i=0; i<9; i++)
        SendMessage(*hDateFormat, SSBM_ADDSTRING, 0,(LPARAM)(szDateFormat[i]));

    return TRUE; 
}

/***************************************************************
* Function  SetTimeSource
* Purpose   set the time source    
* Params
* Return    
* Remarks
***************************************************************/
BOOL SetTimeSource(TIMESOURCE timesource)
{
	iTimeSrcFlag = timesource;
    FS_WritePrivateProfileInt(SN_TIMEMODE,KN_TIMESRC,timesource,SETUPFILENAME);
    return TRUE;
}
/***************************************************************
* Function  GetTimeSource
* Purpose   Get the time source    
* Params
* Return    
* Remarks
***************************************************************/
TIMESOURCE   GetTimeSource(void)
{
	iTimeSrcFlag = FS_GetPrivateProfileInt(SN_TIMEMODE, KN_TIMESRC,0,SETUPFILENAME);
    return iTimeSrcFlag;
}

TIMESOURCE GetTimeSourceFlag(void)
{
	return iTimeSrcFlag;
}

void Sett_RestoreRamTimeSrc(void)
{
	iTimeSrcFlag = 0;
}
/***************************************************************
* Function  GetTimeDisplay
* Purpose       
* Params    before call this function, init the time structure "st",
*           then after the function, "cTime" will be the character
*           string of time in the format of current setting, and 
*           "cDate" will be the character string of date in the 
*           current format. 
* Return    
* Remarks   when the value of year in "st" is "0", "cDate" will just
*           display the month and date.
***************************************************************/

BOOL GetTimeDisplay(SYSTEMTIME st, char* cTime, char* cDate)
{
    static const char* pdString0 = "%02d.%02d";
    static const char* pdString1 = "%02d/%02d";
    static const char* pdString2 = "%02d-%02d";
    static const char* pdString3 = "%02d:%02d";
    static const char* pdamString = "%02d:%02dAM";
    static const char* pdpmString = "%02d:%02dPM";

    static const char* pdyString0 = "%02d.%02d.%04d";
    static const char* pydString0 = "%04d.%02d.%02d";
    static const char* pdyString1 = "%02d/%02d/%04d";
    static const char* pydString1 = "%04d/%02d/%02d";
    static const char* pdyString2 = "%02d-%02d-%04d";
    static const char* pydString2 = "%04d-%02d-%02d";
    
	static const int iTime12 = 12;		
  
    if(cDate != NULL)
    {
		switch(GetDateFormt())
		{
		case DF_DMY_DOT:
			if(st.wYear == 0)
				sprintf(cDate,pdString0,st.wDay,st.wMonth);
			else
				sprintf(cDate,pdyString0,st.wDay,st.wMonth,st.wYear);
			break;
		case DF_MDY_DOT:
			if(st.wYear == 0)
				sprintf(cDate,pdString0,st.wMonth,st.wDay);
			else
				sprintf(cDate,pdyString0,st.wMonth,st.wDay,st.wYear);
			break;
		case DF_YMD_DOT:
			if (st.wYear == 0)
				sprintf(cDate,pdString0,st.wMonth,st.wDay);
			else
				sprintf(cDate,pydString0,st.wYear,st.wMonth,st.wDay);
			break;
		case DF_DMY_SLD:
			if (st.wYear == 0)
				sprintf(cDate,pdString1,st.wDay,st.wMonth);
			else
				sprintf(cDate,pdyString1,st.wDay,st.wMonth,st.wYear);
			break;
		case DF_MDY_SLD:
			if (st.wYear == 0)
				sprintf(cDate,pdString1,st.wMonth,st.wDay);
			else
				sprintf(cDate,pdyString1,st.wMonth,st.wDay,st.wYear);
			break;
		case DF_YMD_SLD:
			if (st.wYear == 0)
				sprintf(cDate,pdString1,st.wMonth,st.wDay);
			else
				sprintf(cDate,pydString1,st.wYear,st.wMonth,st.wDay);
			break;
		case DF_DMY_DSH:
			if (st.wYear == 0)
				sprintf(cDate,pdString2,st.wDay,st.wMonth);
			else
				sprintf(cDate,pdyString2,st.wDay,st.wMonth,st.wYear);
			break;
		case DF_MDY_DSH:
			if (st.wYear == 0)
				sprintf(cDate,pdString2,st.wMonth,st.wDay);
			else
				sprintf(cDate,pdyString2,st.wMonth,st.wDay,st.wYear);
			break;
		case DF_YMD_DSH:
			if (st.wYear == 0)
				sprintf(cDate,pdString2,st.wMonth,st.wDay);
			else
				sprintf(cDate,pydString2,st.wYear,st.wMonth,st.wDay);
			break;
		default:
			return FALSE;
		}
    }
    
    if(cTime != NULL)
    {
	    if(GetTimeFormt()==TF_24)
		    sprintf(cTime,pdString3,st.wHour,st.wMinute);
	    else
	    {          
		    if ((st.wHour >= 0) && (st.wHour <= 11))//AM
		    {
		        sprintf(cTime,pdamString,st.wHour == 0 ? iTime12 : st.wHour,st.wMinute);
		    }
            else if ((st.wHour >= iTime12) && (st.wHour <= 23))//PM
            {
                if (st.wHour == iTime12)
		            sprintf(cTime,pdpmString,st.wHour,st.wMinute);
                else
		            sprintf(cTime,pdpmString,st.wHour - iTime12,st.wMinute);
            }
	    }     
    }
	return TRUE;
}
/*********************************************************************\
* Function        MobileSet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void TimeSet_InitVScrolls(HWND hWnd,int iItemNum)
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
* Function        TimeSet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/

static void TimeSet_OnVScroll(HWND hWnd,  UINT wParam)
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

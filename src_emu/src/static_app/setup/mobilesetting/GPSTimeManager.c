/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : GPS time update window
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
#include    "LocApi.h"

#define     IDC_BUTTON_QUIT         3 
#define     IDC_BUTTON_SET          4 
#define     CALLBACK_SETGPSTIME     WM_USER+132

extern void SetUsingTimeZone(TIMEZONE tz, SYSTEMTIME* st);
extern char* GetHomeZoneStr(void);
extern char* GetVisitZoneStr(void);
extern void TimeZoneStrToSysTime(SYSTEMTIME * st, const char * szTimeZone);
extern void tzs_DecreaseDay(SYSTEMTIME *lpTD);
extern void tzs_IncreaseDay(SYSTEMTIME *lpTD);
extern float tzs_TimeStrToFloat(const char *szTimeStr);

static void TZStrToSysTime( int* aHour, int* aMin, const char * szTimeZone );

static const char * pClassName       = "GPSTimeManagerClass";
static const char* pTimeStr = "%02d/%02d/%02d,%02d:%02d:%02d";
static HWND hTimeMgr;
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

TIMESOURCE GetTimeSourceFlag(void);

BOOL    GPSTimeManager(void)
{
    WNDCLASS    wc;
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

    hTimeMgr = CreateWindow(pClassName,NULL,
        0, 
		0,
		0,
		0,
		0,
        NULL,NULL,NULL,NULL);

    if (NULL == hTimeMgr)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    return (TRUE);
}

static LRESULT AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT				lResult;
	UTCTIME				GPSTime;
	static SYSTEMTIME	st;
	int					meRet;
	char				cdate[ 20 ] = "";

    lResult = ( LRESULT )TRUE;
    switch( wMsgCmd )
    {
		case WM_CREATE :     
			LocRegister( hWnd, NAV_DATA_MODE_FIX_ONLY );
		break;

		case WM_CLOSE:
			DestroyWindow( hWnd );
		break;
			
		case WM_COMMAND: // KAo 28.09.05
			switch( wParam )
			{
				case LOC_NAV_DATA_UPDATE:
				{
					static TIMESOURCE oldTimeSource = USER_DEFINED;
					static TIMESOURCE ts;
					static WORD		  oldDay = 0;
					static WORD		  oldMonth;
					BOOL              tryToUpdate = FALSE;
					int				  tzHour;
					int				  tzMin;
					int				  mins;

					ts = GetTimeSourceFlag();
					if( oldTimeSource != ts ) // switched to GPS_BASED?
					{
						oldTimeSource = ts;
						tryToUpdate = TRUE;
					}
					if( ts != GPS_BASED ) break;

					if( LocGetUTCTimeAndDate( &GPSTime ) == 0 ) //no error 
					{
//								memset( &st, 0, sizeof( SYSTEMTIME ) );
						st.wDay    = GPSTime.UTCDay;
						st.wMonth  = GPSTime.UTCMonth;
						st.wYear   = GPSTime.UTCYear;
						st.wHour   = GPSTime.UTCHour;
						st.wMinute = GPSTime.UTCMinute;
						st.wSecond = GPSTime.UTCSecond;
					}
					else
					{
//						printf( "\r\nGPSTIME ERROR\n\r" );
						break;
					}

					if( st.wDay != oldDay || st.wMonth != oldMonth ) // if 24 h from last updating
					{
//						printf( "\r\nGPSTIME 3:%02d-%02d %02d-%02d\r\n", st.wMonth, oldMonth, st.wDay, oldDay );
						oldDay   = st.wDay;
						oldMonth = st.wMonth;
						tryToUpdate = TRUE;
					}

					if( tryToUpdate )
					{
						if( GetUsingTimeZone() == TZ_HOME )
						{
							TZStrToSysTime( &tzHour, &tzMin, GetHomeZoneStr() );
						}
						else /* if( GetUsingTimeZone() == TZ_VISIT )*/
						{
							TZStrToSysTime( &tzHour, &tzMin, GetVisitZoneStr() );
						}

						mins = ( ( int )st.wHour + tzHour ) * 60 + ( int )st.wMinute + tzMin;
						printf( "\r\nTIMEZONE1:%02dh %02dmin,%02dh %02dmin, %02dmins\r\n", st.wHour, st.wMinute, tzHour, tzMin, mins );

						if( tzMin == 0 || tzHour >= 0 && tzMin > 0 )
						{
							if( st.wMinute + tzMin >= 60 )
							{
								st.wHour = ( 25 + ( int )st.wHour + tzHour ) % 24;
							}
							else
							{
								st.wHour = ( 24 + ( int )st.wHour + tzHour ) % 24;
							}
						}
						else
						{
							if( st.wMinute + tzMin >= 0 )
							{
								st.wHour = ( 24 + ( int )st.wHour + tzHour ) % 24;
							}
							else
							{
								st.wHour = ( 23 + ( int )st.wHour + tzHour ) % 24;
							}
						}
						st.wMinute = ( 60 + st.wMinute + tzMin ) % 60;

						if( mins >= 24 * 60 )
						{
							tzs_IncreaseDay( &st );
						}
						else if( mins < 0 )
						{
							tzs_DecreaseDay( &st );
						}

						sprintf( cdate, pTimeStr,//"%02d/%02d/%02d,%02d:%02d:%02"
								 st.wYear%100, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond );
						printf( "\r\nGPSTIME+ZONE: %s\n\r", cdate );

					    meRet = ME_SetClockTime( hWnd, CALLBACK_SETGPSTIME, cdate );//to set the wireless module first
						   
					    if( meRet < 0 ) //Me set time failed
						{
//							printf( "\r\nGPSTIME MODEM ERROR\n\r" );
							PostMessage( hWnd, CALLBACK_SETGPSTIME, 0, 0 ); //set local time anyway
						}
					}
				}
				break;
				default:
				break;
			}
		break;

		case CALLBACK_SETGPSTIME:
			SetLocalTime( &st );
//			printf( "\r\nGPSTIME CALLBACK:%02d,%02d,%02d\r\n", st.wYear%100, st.wMonth, st.wDay );
			DlmNotify((WPARAM)PS_SETTIME, 0);
		break;

		case WM_DESTROY :
			hTimeMgr = NULL;
			UnregisterClass(pClassName,NULL);
		break;

		default :
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;
}

static void TZStrToSysTime( int* aHourPtr, int* aMinPtr, const char * szTimeZone )
{
    float timevalue;
    int hour, minute;
    timevalue = tzs_TimeStrToFloat( szTimeZone );
    hour = ( int )( timevalue );
    minute = abs( ( int )( ( timevalue - hour ) * 60 ) );

    if( ( hour == 0 ) && ( timevalue < 0 ) )
    {
        *aHourPtr = hour;
        *aMinPtr  = -minute;
    }
    else
    {
        *aHourPtr = hour;
		if( hour < 0 )
		{
			*aMinPtr  = -minute;
		}
		else
		{
			*aMinPtr  = minute;
		}
    }    
}

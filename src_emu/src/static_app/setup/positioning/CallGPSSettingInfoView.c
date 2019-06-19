 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting\GPS Profile
 *
 * Purpose  : GPS profile form view window
 *            
\**************************************************************************/
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
#include	"prebrowhead.h"

#define IDC_BUTTON_OK	 100
#define IDC_GPS_FORMVIEW 101
#define LEFT_ICON		"/rom/mbrecord/left.bmp"
#define RIGHT_ICON		"/rom/mbrecord/right.bmp"



static HWND hFrameWin = NULL;
static HWND hWndApp = NULL;
static HWND hFormView = NULL;
static int iCurGPSIndex;
static char * pClassName = "GPSProfileFormViewerClass";
static GPSMODE  Global_gm;
/*
static HANDLE hLeftBmp = NULL;
static HANDLE hRightBmp = NULL;
*/
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void CallGPSFormView(HWND hFrame, HWND hWnd, int iCurGPSIndex, int nGPSNum);


void Sett_GPS_ConvertIconName(char* szIconName);

int GetGPSprofileNum (void);
BOOL CallGPSSettingInfoView(HWND hFrame, int iIndex)
{
    RECT rClient;
    WNDCLASS    wc;

    hFrameWin = hFrame;
    
    iCurGPSIndex = iIndex;
    GetGPSprofile(&Global_gm, iCurGPSIndex);

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
        WS_VISIBLE | WS_CHILD, 
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
	

    SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON,//LSK
            MAKEWPARAM((WORD)1, (WORD)IDC_BUTTON_OK), (LPARAM)ML("Ok"));
        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT,0, (LPARAM)"");//RSK   
            
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    SetFocus(hWndApp);
	return (TRUE);
	
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT     lResult;
	static int nGPSProfileNum;
    lResult = (LRESULT)TRUE;
	
    switch ( wMsgCmd )
	{
		case WM_CREATE :
			nGPSProfileNum = GetGPSprofileNum();
			CallGPSFormView(hFrameWin, hWnd, iCurGPSIndex, nGPSProfileNum);		
			break;
		
		case PWM_SHOWWINDOW:
			if (iCurGPSIndex == 0)
			{
				SetWindowText(hFrameWin, ML("Full power"));
			}
			else if (iCurGPSIndex == 1)
			{
				SetWindowText(hFrameWin, ML("Economy"));
			}
			else
			{
				SetWindowText(hFrameWin, Global_gm.cGPSModeName);
			}
			
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Ok"));//LSK
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)"");//RSK   
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");		
			SetFocus(hWndApp);
			break;
		
		case WM_KEYDOWN:
			switch(LOWORD(wParam))
			{       
			case VK_RETURN:
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				break;
			case VK_UP:
				PostMessage(hFormView, WM_KEYDOWN, VK_UP, 0);
				break;
			case VK_DOWN:
				PostMessage(hFormView, WM_KEYDOWN, VK_DOWN, 0);
				break;
			case VK_LEFT:
				if ( 0 == iCurGPSIndex )
				{
					break;
				}
				iCurGPSIndex--;
				DestroyWindow(hFormView);
				CallGPSFormView(hFrameWin, hWnd, iCurGPSIndex, nGPSProfileNum);
				break;
			case VK_RIGHT:
				if ( iCurGPSIndex == nGPSProfileNum - 1)
				{
					break;
				}
				iCurGPSIndex++;
				DestroyWindow(hFormView);
				CallGPSFormView(hFrameWin, hWnd, iCurGPSIndex, nGPSProfileNum);
				break;
			default:
				break;
			}
			break;  
    case WM_DESTROY : //Quit the application;
		SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
		SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
        hFormView = NULL;
        hWndApp = NULL;
        UnregisterClass(pClassName,NULL);
        break;
	    
		default :
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
	}
	return lResult;

}

static void CallGPSFormView(HWND hFrame, HWND hWnd, int iCurGPSIndex, int nGPSNum)
{
	LISTBUF		Listtmp;
    RECT rcClient;
	char *Tmp;
	char TmpName [GPSICONNAMEMAXLEN+1];
    GetGPSprofile(&Global_gm, iCurGPSIndex);
	
	PREBROW_InitListBuf (&Listtmp);
	if (iCurGPSIndex == 0)
	{
		PREBROW_AddData(&Listtmp, ML("Name"), ML("Full power"));
		SetWindowText(hFrame, ML("Full power"));
	}
	else if (iCurGPSIndex == 1)
	{
		PREBROW_AddData(&Listtmp, ML("Name"), ML("Economy"));
		SetWindowText(hFrame, ML("Economy"));
	}
	else
	{
		PREBROW_AddData(&Listtmp, ML("Name"), Global_gm.cGPSModeName);
		SetWindowText(hFrame, Global_gm.cGPSModeName);
	}
	
//	if (iCurGPSIndex == 0 || iCurGPSIndex == 1)
//	{
//		Sett_GPS_ConvertIconName(Global_gm.cGPSIconName);
//		UTF8ToMultiByte(CP_ACP,0,Global_gm.cGPSIconName,GPSICONNAMEMAXLEN + 1,Global_gm.cGPSIconName,GPSICONNAMEMAXLEN+ 1,NULL,NULL);
//		PREBROW_AddData(&Listtmp, ML("Icon"), Global_gm.cGPSIconName);			
//	}
//	else
//	{
		Tmp = strrchr(Global_gm.cGPSIconName, '/');	
		if(Tmp != 0)
		{
			Tmp++;
			memset(TmpName, 0, GPSICONNAMEMAXLEN+1);
			Sett_GPS_ConvertIconName(Tmp);
			UTF8ToMultiByte(CP_ACP,0,Tmp,strlen(Tmp),TmpName,GPSICONNAMEMAXLEN+1,NULL,NULL);
			PREBROW_AddData(&Listtmp, ML("Icon"), TmpName);		
		}
		else
		{
			PREBROW_AddData(&Listtmp, ML("Icon"), "");		
		}
//	}
	
	switch(Global_gm.iRefIntervl)
	{
	case GPSRI_OFF:
		PREBROW_AddData(&Listtmp, ML("Refresh interval"), ML("Off"));
		break;
	case GPSRI_ONESEC:
		PREBROW_AddData(&Listtmp, ML("Refresh interval"), ML("GPSRefInt1s"));
		break;
	case GPSRI_THREESEC:
		PREBROW_AddData(&Listtmp, ML("Refresh interval"), ML("GPSRefInt3s"));
		break;
	case GPSRI_TENSEC:
		PREBROW_AddData(&Listtmp, ML("Refresh interval"), ML("GPSRefInt10s"));
		break;
	case GPSRI_THIRTYMIN:
		PREBROW_AddData(&Listtmp, ML("Refresh interval"), ML("GPSRefInt30min"));
		break;
	default:
		break;
	}

	switch(Global_gm.iRefMode)
	{
	case GPSRIC_CON:
		PREBROW_AddData(&Listtmp, ML("Ref. int. charging"), ML("Continuous"));
		break;
	case GPSRIC_KIPINT:
		PREBROW_AddData(&Listtmp, ML("Ref. int. charging"), ML("Keep interval"));
		break;
	case GPSRIC_OFF:
		PREBROW_AddData(&Listtmp, ML("Ref. int. charging"), ML("Off"));
		break;
	default:
		break;
	}

	switch(Global_gm.iOptMode)
	{
	case GPS_ACCURACY:
		PREBROW_AddData(&Listtmp, ML("Optimized for"), ML("Accuracy"));
		break;
		
	case GPS_SENSITI:
		PREBROW_AddData(&Listtmp, ML("Optimized for"), ML("Sensitivity"));
		break;
	
	default:
		break;
	}

	switch(Global_gm.iNMEAoutput)
	{
	case GPSNMEA_OFF:
		PREBROW_AddData(&Listtmp, ML("NMEA output"), ML("Off"));
		break;
	case GPSNMEA_FOUREIGHTOO:
		PREBROW_AddData(&Listtmp, ML("NMEA output"), ML("4800 bps"));
		break;
	case GPSNMEA_NINESIXOO:
		PREBROW_AddData(&Listtmp, ML("NMEA output"), ML("9600 bps"));
		break;
	default:
		break;
	}
	
	GetClientRect (hFrame, &rcClient);
	
	hFormView = CreateWindow(FORMVIEWER,NULL,
		WS_VISIBLE  |WS_CHILD | WS_VSCROLL,
		rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		hWnd, (HMENU)IDC_GPS_FORMVIEW, NULL, (PVOID)&Listtmp);
	
	if (!hFormView) 
	{
		return;
	}	
	if ( 0 == iCurGPSIndex)
	{
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)"");
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)SETT_RIGHT_ICON);
	}
	else if (iCurGPSIndex == nGPSNum - 1)
	{
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)SETT_LEFT_ICON);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)"");
	}
	else
	{
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, LEFTICON), (LPARAM)SETT_LEFT_ICON);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_ICON, RIGHTICON), (LPARAM)SETT_RIGHT_ICON);
	}		
	
}
void Sett_GPS_ConvertIconName(char* szIconName)
{
	if (strcmp(szIconName, "GPS_profile_6.ico") == 0)
	{
		memset(szIconName, 0, sizeof(szIconName));
		strcpy(szIconName, "GPS_rabbit.ico");
	}
	else if (strcmp(szIconName, "GPS_profile_5.ico") == 0)
	{
		memset(szIconName, 0, sizeof(szIconName));
		strcpy(szIconName, "GPS_turtle.ico");
	}
}

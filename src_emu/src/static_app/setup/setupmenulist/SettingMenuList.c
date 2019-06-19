/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting
 *
 * Purpose  : main window
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/

#include "SettingMenuList.h"

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
#include    "pubapp.h"
#include    "unistd.h "
#include    "stat.h"
#include    "LocApi.h"
#include	"log.h"
#include    "PMALARM.h"
#define     szPrompt  ML("Restore defaults.snEnter phone sn lock codecolon")

static      HINSTANCE    hInstance;
static      HWND         hWndApp = NULL;
static      BOOL         bShow; 
static      HWND         hFrameWin=NULL;
static      const   char     *SetupFileDir = "/mnt/flash/setup";
static int LangChangeFlag = 0;
static BOOL		CreateControl (HWND hWnd, HWND *hSettingList);
static void     Load_SettingList(HWND hwnd,HWND hSettingList);
static BOOL     Load_SettingListIcon(HWND hList,HBITMAP *hIcon);
static BOOL     RestoreSettingInfo(void);
static void		OnAppInitProcess(HINSTANCE pInstance);
static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void	Sett_RestoreTime(HWND hWnd);
static BOOL CallSceneModeShortCutWin(HWND hFrame);
static BOOL Sett_CreateSettMainView(HWND hFrame);


extern BOOL     CallPhoneSettingList(HWND hWnd);
extern BOOL     CreateSecuritysettingWnd(HWND hParentwnd );
extern BOOL     PositioningSetting(HWND hwndCall);
extern BOOL     CallSceneModeWindow (HWND hwndCall);
extern BOOL     CallConnectSetting(HWND hFatherWnd);
extern BOOL     AccessorySetting(HWND hParentWnd);
extern BOOL     RestoreGPSProfile(void);
extern BOOL     RestoreSceneMode(void);
extern BOOL     CallAutoCloseClewWindow(void);
extern BOOL     CallSettingList(HWND hFrameWnd);
extern int      CreateFullPathDir (const char * pDir);
extern int      ME_FactoryDefaultSetting( HWND hWnd, int Msg );
extern void		Sett_SaveSettInfo(void);
extern BOOL		SYS_InitGPSProfile(void);
extern int		SYS_GetCurSceneMode(void);
extern void		SYS_GetActiveGPSProfile(void);
extern void		SetBenefonActGPS(GPSMODE *gpsPollexUiProfile);
extern BOOL		DHI_ResumeMeStart(void);


BOOL GPSTimeManager(void);
BOOL SetSecurity_code(char * cpassword);
BOOL RstCommnSettInfo(void);
void ResetFavorite(void);
void ResetGame(void);
void Sett_RestoreRamSettingInfo(void);
BOOL Sett_BritAdjusting(LEVELVALUE level);

BOOL    CallSceneModeWindow(HWND hwndCall);
BOOL LEVADJ_RegisterClass(void);
BOOL WML_SetToDefault(void);
int	OpenedFileNumber(void);

DWORD   Settings_AppControl (int nCode, void* pInstance, WPARAM wParam, LPARAM lParam )
{
    DWORD dwRet = TRUE;
    switch (nCode)
    {
    case APP_INIT :
		OnAppInitProcess(pInstance);
        break;

    case APP_ACTIVE :

#ifdef _SETTING_DEBUG_
  StartObjectDebug();
#endif
SETT_PRINT("\r\nZC FILE HANDLE NUMBER NOW is ===========>%d \r\n", Sett_GetFileHandleNum());
		if (lParam != 0)
		{
			if(IsWindow(hFrameWin))
			{
/*
				DestroyWindow(hFrameWin);
				hFrameWin = NULL;
*/
				ShowWindow(hFrameWin,SW_SHOW);
				ShowOwnedPopups(hFrameWin,SW_SHOW);
				UpdateWindow(hFrameWin);
				break;
			}
            hFrameWin = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
			
			if (!CallSceneModeShortCutWin(hFrameWin))
				return FALSE;

            ShowWindow(hFrameWin,SW_SHOW);
            UpdateWindow(hFrameWin);
			break;
		}
		else if(IsWindow(hFrameWin))
        {
            ShowWindow(hFrameWin,SW_SHOW);
            ShowOwnedPopups(hFrameWin,SW_SHOW);
            UpdateWindow(hFrameWin);
        }
        else
        {
            hFrameWin = CreateFrameWindow(WS_CAPTION | PWS_STATICBAR);
			Sett_CreateSettMainView(hFrameWin);            			

            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDC_BUTTON_EXIT,(LPARAM)ML("Exit"));
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
                        MAKEWPARAM(IDC_BUTTON_SET,1), (LPARAM)"") ;//set the left soft key
            SetWindowText(hFrameWin,ML("Settings"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif


            ShowWindow(hFrameWin,SW_SHOW);
            UpdateWindow(hFrameWin);
            SetFocus(hWndApp);          
        }    
        break;

    case APP_INACTIVE :
		ShowOwnedPopups(hFrameWin, SW_HIDE);
        ShowWindow(hFrameWin,SW_HIDE); 
        break;

	case APP_DESTROY:
		Sett_SaveSettInfo();
		break;
	case APP_ALARM:
		if (wParam == ALMN_SETERROR)
			SETT_PRINT("\r\n Alarm_Set auto switch off faild......\r\n");
		break;
    }
    return dwRet;
}


static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  int     iIndex=0,i;
	int iret;
    static  HWND    hSettingList,hFocus;
    BOOL PassRet;
    static char szPwd[9] = "";
    static char szSetPwd[9] = "";
    LRESULT lResult;
    HDC     hdc;
    static      HBITMAP hIcon[8];
    lResult = TRUE;   

    switch ( wMsgCmd )
    {

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0,(LPARAM)ML("Exit"));
        SetWindowText(hFrameWin,ML("Settings"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
		if (LangChangeFlag == 1)
		{
			iIndex = SendMessage(hSettingList, LB_GETCURSEL, 0 ,0);
			Load_SettingList(hWnd,hSettingList);
			Load_SettingListIcon(hSettingList,hIcon);
			SendMessage(hSettingList, LB_SETCURSEL, iIndex, 0);
			LangChangeFlag = 0;
		}
        SetFocus(hWnd);
        break;

    case WM_CREATE :
        CreateControl (hWnd, &hSettingList);  
		Load_SettingList(hWnd,hSettingList);
   		hdc         = GetDC(hWnd);
        hIcon[0]    = LoadImageEx(hdc,NULL,ICONPHONE,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hIcon[1]    = LoadImageEx(hdc,NULL,ICONCALL,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hIcon[2]    = LoadImageEx(hdc,NULL,ICONCONNECT,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hIcon[3]    = LoadImageEx(hdc,NULL,ICONPROFILE,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hIcon[4]    = LoadImageEx(hdc,NULL,ICONSECURE,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hIcon[5]    = LoadImageEx(hdc,NULL,ICONPOSIT,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hIcon[6]    = LoadImageEx(hdc,NULL,ICONACCESS,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hIcon[7]    = LoadImageEx(hdc,NULL,ICONRESTORE,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);

        Load_SettingListIcon(hSettingList,hIcon);
		SendMessage(hSettingList,LB_SETCURSEL,0,0);
		break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_SETTINGLIST));
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

   case WM_COMMAND :

       iIndex = SendMessage(hSettingList,LB_GETCURSEL,0,0);              //get the current selected item number
        
       switch (LOWORD(wParam))
        {
        case IDC_BUTTON_EXIT:
            DestroyWindow(hWnd);
            break;
        case IDC_BUTTON_OK:
			switch(iIndex)
			{
			case 0:
				CallPhoneSettingList(hFrameWin);
				break;
            case 1:
                CallSettingList(hFrameWin); //CallSetting Entry.
                break;
            case 2:
                CallConnectSetting(hFrameWin);
                break;
			case 3:
				CallSceneModeWindow(hFrameWin);
				break;
			case 4:
				CreateSecuritysettingWnd(hFrameWin);
				break;
            case 5:
		 		PositioningSetting(hFrameWin);//      ShowGPSSettingsView();               
                break;
            case 6:
                AccessorySetting(hFrameWin);
                break;
            case 7: 
				PostMessage(hWnd, CALLBACK_REINPUT_PASSWD, 0, 0);
                break;
			default:
				break;		
			}		
        }
        break;

	   case CALLBACK_REINPUT_PASSWD:
		   PassRet = SSPLXVerifyPassword(hWnd, NULL, szPrompt, szPwd, MAKEWPARAM(4,8),ML("Ok"), ML("Cancel"), -1); //int nSize, 
		   if(PassRet)
		   {
			   GetSecurity_code(szSetPwd);
			   if(strcmp(szPwd, szSetPwd) == 0)
			   {
				   WaitWin(hWnd, TRUE, ML("Restoring defaults..."), ML("Settings"), ML("Ok"), 0, NULL);
				   
				   iret = ME_FactoryDefaultSetting(hWnd, CALLBACK_ME_RESTORE);
				   if (iret < 0)
				   {
					   PostMessage(hWnd, CALLBACK_ME_RESTORE, 0, 0);
					   SETT_PRINT("\r\n ME restored failed\r\n");
				   }
			   }
			   else
			   {
				   PLXTipsWin(hFrameWin, hWnd, CALLBACK_REINPUT_PASSWD,ML("Phone locksncode wrong"),ML("Settings"),Notify_Failure,ML("Ok"),0,WAITTIMEOUT);                       
			   }
		   }
		   break;
    case CALLBACK_ME_RESTORE:
        RestoreSettingInfo();
		if (ME_GetClockTime ( hWnd, SETT_GET_ME_TIME ) < 0)
		{
			SETT_PRINT("\r\n Get time from ME failed\r\n");
			PostMessage(hWnd, SETT_GET_ME_TIME, 0, 0);
			WaitWin(hWnd, FALSE, NULL, NULL, NULL,NULL, NULL); 
		}
        break;

	case SETT_GET_ME_TIME:
		Sett_RestoreTime(hWnd);
		WaitWin(hWnd, FALSE, NULL, NULL, NULL,NULL, NULL); 
		PLXTipsWin(hFrameWin, hWnd, NULL,ML("Default values restored"),ML("Settings"),Notify_Success,ML("Ok"),0,WAITTIMEOUT);
		break;
		
    case WM_DESTROY :
         for(i=0;i<8;i++)
    	    DeleteObject(hIcon[i]);

         hWndApp = NULL;
		 Sett_SaveSettInfo();
		 WaitWin(hWnd, FALSE, NULL, NULL, NULL,NULL, NULL); 
         UnregisterClass(pClassName,NULL);
         DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);

#ifdef _SETTING_DEBUG_
  EndObjectDebug();
#endif
SETT_PRINT("\r\nZC FILE HANDLE NUMBER Now is ===========>%d \r\n", Sett_GetFileHandleNum());
         break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    } 
    return lResult;
}
/*==========================================================================
*   Function:  static void	OnAppInitProcess();
*   Purpose :  
*   Argument:  HWND hWnd
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/
static void  OnAppInitProcess(HINSTANCE pInstance)
{
    AUTO_CLOSE Auto_State;
    int iCurSoundProfile;
	int iGPSState;
	GPSPROFILE gpsLocModUiProfile;
	GPSMODE gpsPollexUiProfile;
	RTCTIME rtcTime;
	SYSTEMTIME stime;

	hInstance = pInstance;	

	/********************************************************************/	
	mkdir(SetupFileDir, 0);	// check if the directory of setup is exist
	/******************Set phone language********************************/	

	if (GetAutoLangState())
	{
		SetAutoLanguage(SWITCH_ON);
	}
	else //
	{
		SetActiveLanguage(GetActiveLanguage());
	}
	
	/********************************************************************/	


	SYS_InitGPSProfile();
	SYS_GetActiveGPSProfile();
	/************************Set the auto switch on/off icon**************/	
	GetAutoOSOption(&Auto_State);
	
	if(Auto_State.OpenState == SWITCH_ON && Auto_State.CloseState == SWITCH_OFF)
        DlmNotify(PS_POWERON,ICON_SET);
	else if(Auto_State.OpenState == SWITCH_OFF && Auto_State.CloseState == SWITCH_ON)
        DlmNotify(PS_POWEROFF,ICON_SET);
	else if(Auto_State.OpenState == SWITCH_ON  && Auto_State.CloseState == SWITCH_ON)
        DlmNotify(PS_POWERONOFF,ICON_SET);

	if (Auto_State.CloseState == SWITCH_ON)
	{
		memset(&rtcTime, 0, sizeof(RTCTIME));
		GetLocalTime(&stime);
		rtcTime.v_nDay = (u_INT1)(stime.wDay);
		rtcTime.v_nMonth = (u_INT1)(stime.wMonth);
		rtcTime.v_nYear = (u_INT2)(stime.wYear);
		rtcTime.v_nHour = (u_INT1)(Auto_State.CloseHour);
		rtcTime.v_nMinute = (u_INT1)(Auto_State.CloseMin);
		rtcTime.v_nSecond = 30;
		RTC_SetAlarms(AUTO_POWER_OFF, ALM_ID_AUTO_POWER_OFF, ALMF_REPLACE|ALMF_AUTONEXTDAY, AUTO_POWER_OFF, &rtcTime);
	}
	/*********************Set the sound profile icon***********************/	
	iCurSoundProfile = SYS_GetCurSceneMode();
	if(iCurSoundProfile == 1)
		DlmNotify(PS_SILENCE, ICON_SET);
	else if(iCurSoundProfile != 0)
		DlmNotify(PS_SPROHTER, ICON_SET);
	
	/*****************Sync the GPS profile*********************************/	
	if ( 0 == LocCreateServer() )
	{
		iGPSState = GetActiveGPSProfile();
		if (iGPSState > 0) //GPS is on
		{
			GetGPSprofile( &gpsPollexUiProfile, iGPSState);
			SetBenefonActGPS(&gpsPollexUiProfile);
		}
		else if (-1 == iGPSState)
		{
			gpsLocModUiProfile.fixRate = FIX_RATE_OFF;
			gpsLocModUiProfile.fixRateWhileCharging = FIX_RATE_WHILE_CHARGING_OFF;
			gpsLocModUiProfile.handle = NULL;
			gpsLocModUiProfile.nmeaOutput = GPS_NMEA_OUTPUT_OFF;
			gpsLocModUiProfile.operatingMode = GPS_OPERATING_MODE_ACCURATE;
			LocSetUIGPSProfile( &gpsLocModUiProfile );			
		}
	}
	
	/********************Start the GPS time manager*************************/	
	GPSTimeManager(); //  
	
	/***********************************************************************/	
//	CallAutoCloseClewWindow(); //create auto close window. 
	LEVADJ_RegisterClass();

}
/*==========================================================================
*   Function:  static void CreateControl ( HWND hWnd )
*   Purpose :  
*   Argument:  HWND hWnd
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/
static BOOL CreateControl (HWND hWnd, HWND *hSettingList)
{
	RECT rect;

	GetClientRect(hWnd, &rect);

    * hSettingList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | /*WS_TABSTOP |*/LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0, 0, 
        rect.right-rect.left,
        rect.bottom-rect.top,
        hWnd, (HMENU)IDC_SETTINGLIST, NULL, NULL);
    if (* hSettingList == NULL )
        return FALSE;
    return TRUE;
}
/*==========================================================================
*   Function:  static void Load_SettingList ( HWND hWnd ,HWND hSettingList)
*   Purpose :  
*   Argument:  HWND hWnd
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/

static  void   Load_SettingList(HWND hwnd,HWND hSettingList)
{
	int i,nChoice = 8;
    const char *cChoiceName[] =
	{ 
	    ML("Phone"),
        ML("Call"),
		ML("Connectivity"),
		ML("Sound profiles"),
		ML("Security"),
		ML("GPS profiles"),
		ML("Accessories"),
		ML("Restore defaults"),
	};

	SendMessage(hSettingList, LB_RESETCONTENT,0,0);  
    		
	for(i=0;i<nChoice;i++)
	{
		SendMessage(hSettingList,LB_ADDSTRING,0,(LPARAM)cChoiceName[i]);
	}

}

/*==========================================================================
*   Function:  static BOOL Load_SettingListIcon (HWND hList,HBITMAP *hIcon)
*   Purpose :  
*   Argument:  HWND hWnd
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/

static BOOL Load_SettingListIcon(HWND hList,HBITMAP *hIcon)
{
    DWORD dWord;
	int i;
		    
	for(i=0;i<8;i++)
	{
		dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
		SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hIcon[i]);
	}
    return TRUE;
}
/*==========================================================================
*   Function:  static BOOL RestoreSettingInfo (void)
*   Purpose :  
*   Argument:  void
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/

static BOOL RestoreSettingInfo(void)
{
    if ( !RstCommnSettInfo() ) //restore the common setting info;
        return FALSE;
	
	Sett_RestoreRamSettingInfo();//restore the setting info in the ram;

    if ( !RestoreGPSProfile() ) //restore the GPS setting info;
        return FALSE;
    if ( !RestoreSceneMode() ) //restore the sound profile setting info;
        return FALSE;
	
	ResetFavorite();
	
	ResetGame();

	ResetLogSetting();

	if (!WML_SetToDefault())
	{
		printf("/r/n Restore wap setting failed/r/n");
	}
	if (!DHI_ResumeMeStart())
	{
		printf("/r/n Restore==> DHI_ResumeMeStart return false!!/r/n");
	}

	Sett_BritAdjusting(LEV_THREE); //reset the default brightness of the screen;

//	SetSecurity_code(PASSWORD_DEFAULT); //reset phone password
    return TRUE;
}
/*==========================================================================
*   Function:  HWND GetSetupAppFrameWnd(void)
*   Purpose :  
*   Argument:  void
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/

HWND GetSetupAppFrameWnd(void)
{
    return hFrameWin;
}
/*==========================================================================
*   Function:  BOOL RstCommnSettInfo(void)
*   Purpose :  
*   Argument:  void
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/

BOOL RstCommnSettInfo(void)
{
    if ( Sett_FileIfExist(SETUPFILENAME) )
	{
		if( 0 == unlink(SETUPFILENAME))
			return TRUE ;
		else
			return FALSE;
	}
    return TRUE;
}

static void	Sett_RestoreTime(HWND hWnd)
{
    SYSTEMTIME  aTime;
    char		aStr[32];	
	int        nYear = 0;
    
	memset(aStr, 0, sizeof(aStr));
    if ( ME_GetResult ( aStr, sizeof(aStr) ) < 0 )
        return;
    
    GetLocalTime( &aTime );
    
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
    
    SetLocalTime( &aTime );
}

/*==========================================================================
*   Function:  BOOL Sett_FileIfExist(const char * pFileName)
*   Purpose :  
*   Argument:  void
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/

BOOL    Sett_FileIfExist(const char * pFileName)
{
    int     iLen;
	FILE * file;


    iLen = strlen(pFileName);
    if (iLen == 0)
        return FALSE;

	file = fopen( pFileName, "r");
	if (file == NULL) 
		return FALSE;
	else
	{
		fclose(file);
		return TRUE;
	}

}
/*==========================================================================
*   Function:  BOOL CallSceneModeShortCutWin(HWND hFrame)
*   Purpose :  
*   Argument:  
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/
static BOOL CallSceneModeShortCutWin(HWND hFrame)
{
	//create tree to get into scene mode;
	if (Sett_CreateSettMainView(hFrame))
	{
		if (CallSceneModeWindow(hFrame))
		{
			return TRUE;
		}
		else
			return FALSE;	
	}
	else
		return FALSE;

}
/*==========================================================================
*   Function:  BOOL Sett_CreateSettMainView(HWND hFrame)
*   Purpose :  
*   Argument:  
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/
static BOOL Sett_CreateSettMainView(HWND hFrame)
{
    RECT rClient;
    WNDCLASS wc;

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

	GetClientRect(hFrame, &rClient);
	
	hWndApp = CreateWindow(
		pClassName, 
		NULL, 
		WS_VISIBLE | WS_CHILD, 
		rClient.left,
		rClient.top,
		rClient.right-rClient.left,
		rClient.bottom-rClient.top,
		hFrame, 
		NULL,
		NULL, 
		NULL
		);

	if (hWndApp == NULL)
		return FALSE;

	return TRUE;
}
/*==========================================================================
*   Function:  void InitDisplayColor(void)
*   Purpose :  
*   Argument:  void
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/
static HDC hDCBack;

void InitDisplayColor(void)
{
    hDCBack = CreateDC("DISPLAY_BACK", NULL, NULL, NULL);
    DrawBitmapFromFile(hDCBack, 0, 0, SETT_BKGRD_PIC, SRCCOPY);    
}

HDC GetBackDisplayhDC(void)
{
	return hDCBack;
}
void	Sett_ProcessLangChange(void)
{
	LangChangeFlag = 1;
}

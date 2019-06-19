           /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : program manager $ version 3.0
 *
 * Purpose  : Implements  program for PDA .
 *            
\**************************************************************************/
#include	<hopen/ipmc.h>
#include	<sys/task.h>
#include	"progman.h"
#include    "string.h"
#include    "malloc.h"
#include	"stdio.h"

#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	 "unistd.h" 

#include	"version.h"
#include    "appfile.h"
#include    "appdirs.h"

#include	 "smsdptch.h"
#include     "me_wnd.h"
#include     "dialmgr.h"

#include	"plx_pdaex.h"
#include    "pmalarm.h"
#include    "pmi.h"
#include    "setting.h"
#include	"setup.h"

#include	"imesys.h"
#include	"LocApi.h"
#include    "MB_control.h"

#include	<project/prioman.h>
#include	<project/compatable.h>

#include	"MBpublic.h"

#define     PMS_MODE_CHARGE     0x0001
#define     PMS_MODE_NORMAL     0x0002
#define     PMS_MODE_RESET      0x0004
#define     PMS_MODE_ALARM      0x0008
#define     PMS_MODE_USB        0x0010

extern	void	TCPIP_Init( void* hWnd );
extern	BOOL	InitMBPostMsg(void);
extern	BOOL	MENULIST_RegisterClass(void);
extern	BOOL	WSCTRLEX_Init(void);
extern	void	PROG_InitDeskIdle(PPROGDESKIDLE  pDeskIdle);
extern	BOOL	CreateAppPathInFlash();
extern	BOOL	MMC_CheckCardStatus();
extern	void	PowerOff();
extern	int		ML_Init(void);
extern	int		DHI_OpenRTC(void);
extern	BOOL	Init_ProfileSetting(void);
extern	BOOL	ML_Write(TCHAR* pszLanguage);
extern  void	BeginAlarm();
extern	int		SetCurrentLanguage(int lang);
extern	void	InitDisplayColor(void);
extern  BOOL	ReadChargerStatus(void);
extern  BOOL    ImmInitialize();
extern	BOOL    ImeCtrlInit(HINSTANCE hInst);
extern	int		MUX_Init(void);					// start multiplexer
extern	int		CMUX_ClosedownFlow(void);		// close multiplexer
extern	int		MAT_AutoSetMeBaudRate(void);
extern	int		RTC_AlarmPowerOn(void);
extern	void	ChargeOnlyState(void);
extern	HWND	GetElectPowerOffWnd(void);
extern	BOOL	Clock_IsAlarming(void);
extern	HWND	GetAlarmTopWnd();
extern	LIGHTMODE	GetDisplayBackLitMode(void); //return value: LS_AUTO;  LS_KEEP_ON
extern	LIGHTMODE	GetKeypadLitMode(void); //return value: LS_AUTO; LS_KEEP_ON; LS_KEEP_OFF
extern  void	RegisterBtComNotifyMsg(HWND hWnd,UINT msg);

static	void	MobileInit1(void);
BOOL			MobileInit2( void );
		void	MobileInit3(void);
		void	MobileInit4(HWND hWnd);
void	f_HandSetInlineInitialize ( void );
void	f_HandSetInlineInitializeEx(void);
BOOL	GetFontHandle(HFONT *hFont, int nType);
BOOL	DeleteFont(void);
void    SetHeadSetStatus(BOOL bAttach);

static	void	f_SetLockTime();
static	void	ColorScheme_init(void);
static	void	RequestLocalTime(HWND hWnd);
static	BOOL	InitFontForApp(void);
static	LRESULT	PowerOnWndProc(HWND, UINT, WPARAM, LPARAM);
        void	SetDefaultLan(void);
extern  BOOL    InitMBPostMsgHook(void);

static APPADM	appManger;
PROGDESKIDLE	ProgDeskIdle;
static HFONT	hFontlarge = NULL, hFontsmall = NULL;
static  int		g_nPowerMode;

/********************************************************************
* Function   InitProgman  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL InitProgman(void)
{
    MSG         msg;

    long	    nRet;
	int         iPoweronStatus = 0;

	DHI_OpenPmu();
	DHI_OpenMixer();

	// get status how the phone is powered on
	g_nPowerMode = PMS_MODE_NORMAL;
	if(DHI_ReadResetStatus() == 0)
	{
		g_nPowerMode = PMS_MODE_RESET;			// watchdog reset
	}
	else
	{											// other reset
		iPoweronStatus = DHI_ReadPowerOnStatus();
		DHI_SetChargerStatus(FALSE);
		
		switch(iPoweronStatus)
		{
		case 0:
			g_nPowerMode = PMS_MODE_NORMAL;			// nomal power on
			break;
		case 1:
			g_nPowerMode = PMS_MODE_ALARM;			// alarm power on
			break;
		case 2:
			g_nPowerMode = PMS_MODE_USB;			// usb power on
			break;
		case 3:
			g_nPowerMode = PMS_MODE_CHARGE;			// charger power on
			DHI_SetChargerStatus(TRUE);
			break;
		default:
			break;
		}

	}
#ifdef _EMULATE_
	// emulator will restart with MC55 still work, so we have to set as reset mode
	g_nPowerMode = PMS_MODE_RESET;
#endif
//	g_nPowerMode = PMS_MODE_ALARM;

	if ( -1==DHI_OpenRTC() )
		printf("\r\nOpen RTC error.\r\n");
	f_SetLockTime();			// set a default time
	ColorScheme_init();
	if ( !InitFontForApp() )
		printf("\r\nInit Font error.\r\n");

	SetDefaultLan();
//	if ( !ML_Write("English") )
	//		printf("\r\nSet default language error.\r\n");
	if ( !ML_Init() )
		printf("\r\nMultiLanguage init error.\r\n");

	InitDisplayColor();
    
    //register public control class
    if ( !MENULIST_RegisterClass() )
		printf("\r\nMenuList register error.\r\n");
	if ( !WSCTRLEX_Init() )
		printf("\r\nWidget initialize error.\r\n");

	//progman initialization
    PROG_InitDeskIdle(&ProgDeskIdle);	
    AppMainConstruct (&appManger );		
    appManger.pVtbl->CreateDeskWnd( &appManger );	//create desk window
   	AppGPSCreate();//create GPS window
	
	RegisterBtComNotifyMsg(appManger.hDeskWnd,WM_PROG_BTCOM);
	
    Prog_InitHook();
	InitMBPostMsgHook();

	// input method initialization
   	if ( !ImmInitialize() )
		printf("\r\nImm Initialize error.\r\n");
    if ( !ImeCtrlInit(NULL) )
		printf("\r\nIme Control Init error.\r\n");
    PLXPrintf("after Ime_Create\r\n");
    
	if ( !Recipient_RegisterClass() )
		printf("\r\nRecipient Class registe error.\r\n");
	if ( !FormView_Register() )
		printf("\r\nFormView Class registe error.\r\n");
    if ( !Init_WaitWindow() )
		printf("\r\nWaitWindow init error.\r\n");
    
    if ( !CreateAppPathInFlash() )
		printf("\r\nCreate path in Flash error.\r\n");

	if ( !MMC_CheckCardStatus() )
		printf("\r\nCreate path in SD card error.\r\n");

	Init_ProfileSetting();
	
	printf("\r\nCreate Audio priority manager.\r\n");
	PrioMan_Initialize();
    printf("\r\nCreate Audio priority manager over.\r\n");

	if ( g_nPowerMode == PMS_MODE_RESET )	// watchdog reset close multiplexer
	{
		printf("\r\nClose multiplexer due to watchdog reset.\r\n");
		CMUX_ClosedownFlow();
	}

	MobileInit1();

	printf("\r\nMobileInit1 over.\r\n");

	// Initialize pmalarm module
	nRet = RTC_Create();

	printf("\r\nRTC_Create over.\r\n");
	
	if (0 == nRet)
	{
		printf("\r\nRTC_release begin!!!n");
		RTC_Release();
		printf("\r\n!!!pmalarm module init error!!!\r\n");
	}

	if ( -1==ME_SetBatteryChargeControl(appManger.hDeskWnd,WM_ME_BATTERYCHARGECONTROL,150) )
		printf("\r\ncall ME_SetBatteryChargeControl error!\r\n");

	printf("\r\nPOWER on Status = %d\r\n",g_nPowerMode);

    if( PMS_MODE_ALARM==g_nPowerMode || PMS_MODE_CHARGE==g_nPowerMode )
	{
		printf("\r\nEnter Alarm Mode & Charge-only Mode...\r\n");

		MAT_AutoSetMeBaudRate();

		// get time from MC55 or GPS
		RequestLocalTime(appManger.hDeskWnd);

		if ( PMS_MODE_ALARM == g_nPowerMode )
		{
			int reason;
			
			printf("\r\n enter alarm power on\r\n");
			reason = RTC_AlarmPowerOn();
			// we should delete the first alarm item after power on by alarm
			if ( 0>reason )
			{
				// error, just power on as normal
				MobileInit2();
			}
			else
				;// alarm power on, we will notify application in pmalarm window proc, do nothing here
		}
		else
		{
			printf("\r\n enter charger power on\r\n");
			ChargeOnlyState();
		}
	}
	else if(g_nPowerMode == PMS_MODE_NORMAL || g_nPowerMode == PMS_MODE_RESET
		|| g_nPowerMode == PMS_MODE_USB)
	{
		printf("\r\nEnter Normal Mode...\r\n");
		MobileInit2();
		printf("After MobileInit2 init!\r\n");
	}
	else
	{
		printf("\r\nEnter Default Mode...\r\n");
		g_nPowerMode = PMS_MODE_NORMAL;
		MobileInit2();
	}

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
	    
    return TRUE;
}
void SetDefaultLan(void)
{
	if (GetAutoLangState() ||!GetActiveLanguage())
	{
#ifdef LANGUAGE_CHN
		ML_Write("Chinese");
#else
		ML_Write("English");
#endif	
	}
	else 
	{	
		ML_Write((TCHAR*)GetActiveLanguage());	
	}
}
/********************************************************************
* Function   EndMobileInit  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
extern void InitAutoPowerOff();
extern  BOOL    Desk_ShowIdle( struct appMain * pAdm);
void EndMobileInit(void)
{
	Desk_ShowGrp(pAppMain);
	Desk_ShowIdle(pAppMain);
#ifndef _EMULATE_
				ME_SleepModule(pAppMain->hDeskWnd, WM_MESLEEPMODE);
#endif
	DlmNotify (MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_KEYLIGHT), 10);
	DlmNotify(MAKELONG(PMM_NEWS_SET_DELAY,PMS_DELAY_SHUT_LIGHT_MAIN), 15);
	DlmNotify (PMM_NEWS_ENABLE,PMF_KEYLIGHT|PMF_MAIN_BKLIGHT|
					PMF_AUTO_SHUT_KEYLIGHT|PMF_AUTO_SHUT_MAIN_LIGHT);
				
	f_sleep_unregister(INIT_FLAG);
	InitAutoPowerOff();
	DlmNotify(PMM_NEWS_ENABLE_SCRSAVE, TRUE);
}
/********************************************************************
* Function   ColorScheme_init  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void ColorScheme_init(void)
{    	    
	COLORSCHEME scheme;
	memset(&scheme, 0x00, sizeof(COLORSCHEME));
	strcpy(scheme.CapBmpName, "/usr/local/lib/gui/resources/capbmp.bmp");
	strcpy(scheme.BarBmpName, "/usr/local/lib/gui/resources/staticbar.bmp");
	strcpy(scheme.BarMenuDownBmpName, "/usr/local/lib/gui/resources/menu_down.bmp");
	strcpy(scheme.BarMenuUpBmpName, "/usr/local/lib/gui/resources/menu_up.bmp");
	scheme.MenuFocusColor = RGB(204,204,204);		//hight light color in menu
	scheme.MenuBorderColor = RGB(0,0,0);			//color of border in menu
	scheme.MenuBKColor = 0xffffff;					//color of background in menu
	strcpy(scheme.ScrollUp, "scup.bmp");			//up arrow in scroll bar
	strcpy(scheme.ScrollDown,"scdn.bmp");			//down arrow in scroll bar
	strcpy(scheme.ScrollLeft,"scle.bmp");			//left arrow in scroll bar
	strcpy(scheme.ScrollRight,"scri.bmp");			//right arrow in scroll bar
	strcpy(scheme.ScrollDisUp,"scdisup.bmp");			//disabled up arrow in scroll bar
	strcpy(scheme.ScrollDisDown,"scdisdn.bmp");			//disabled down arrow in scroll bar
	strcpy(scheme.ScrollDisLeft,"scdisle.bmp");			//disabled left arrow in scroll bar
	strcpy(scheme.ScrollDisRight,"scdisri.bmp");		//disabled right arrow in scroll bar
	scheme.ScrollBorderColor = RGB(122,20,20);			//color of border in scroll bar
    scheme.ScrollBgColor = RGB(248,171,173);            //color of background in scroll bar
	scheme.ScrollThumbColor = RGB(237,27,47);			//color of thumb in scroll bar	
	scheme.ScrollThuBorColor1 = RGB(0,0,0);				//color of thumb's border in scroll bar
	scheme.ScrollThuBorColor2 = RGB(0,0,0);				//color of thumb's border in scroll bar
    scheme.RealFocusColor = RGB(204,204,204);			//color of Real focus
	scheme.FocusColor = RGB(204,204,204);				//color of focus

	/* staticbar bitmap */
	strcpy(scheme.BarBtnDownBmpName, "/usr/local/lib/gui/resources/staticbarbtn_down.bmp");
    strcpy(scheme.BarBtnUpBmpName, "/usr/local/lib/gui/resources/staticbarbtn_up.bmp");
	strcpy(scheme.BarMenuDownBmpName, "/usr/local/lib/gui/resources/menu_down.ico");
	strcpy(scheme.BarMenuUpBmpName, "/usr/local/lib/gui/resources/menu.ico");
	strcpy(scheme.BarBmpName, "/usr/local/lib/gui/resources/capbmp.bmp");
    strcpy(scheme.BarCopsBmpName, "/usr/local/lib/gui/resources/capbmp.bmp");
	strcpy(scheme.BarBtnDownBmpName, "/usr/local/lib/gui/resources/menu_home.ico");
	
	SetColorScheme(&scheme);
}

static HGIFANIMATE hOnGif;
static	int     nStartupTimer;
static	HWND    hWndPowerOn;
/********************************************************************
* Function   f_CheckPowerOff  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
HWND GetPowerOnWnd(void)
{
	return hWndPowerOn;
}
/********************************************************************
* Function   f_CheckPowerOff  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void    f_CheckPowerOff(void)
{
    if ( PMS_MODE_CHARGE != g_nPowerMode )
        return;
    
    DlmNotify (PMM_NEWS_DISABLE,PMF_KEYLIGHT|PMF_MAIN_BKLIGHT);
    
    pAppMain->pVtbl->Destroy(pAppMain, 0);        
    PowerOff();
}
#define TIMERID_POWERON  300
#define TIMERID_STARTUP  301
static	LRESULT	PowerOnWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam)
{
    LRESULT lResult = FALSE;
    HDC     hdc;    
    SIZE    size;
    int     x=0, y=0;
    HGIFANIMATE hOffGif = NULL;
 
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			GetImageDimensionFromFile("/rom/progman/start up.gif",&size);
			if (size.cx < DESK_WIDTH)
				x = (DESK_WIDTH - size.cx)/2;
			if (size.cy < DESK_HEIGHT - TITLEBAR_HEIGHT)
				y = (DESK_HEIGHT- TITLEBAR_HEIGHT - size.cy)/2;
			
			hOnGif = DrawGIFFromFileTime(hWnd, "/rom/progman/start up.gif", x, y, 0,1);
			
			InvalidateRect(hWnd,NULL, TRUE);
			
			PrioMan_CallMusic(PRIOMAN_PRIORITY_POWERON, 1);
		}
        break;
	case WM_ACTIVATE:
		{
			if (WA_INACTIVE == LOWORD(wParam))
			{
				PrioMan_EndCallMusic(PRIOMAN_PRIORITY_POWERON, TRUE);
			}
			else
			{
				SetFocus(hWnd);
			}
		}
		break;
	case WM_KEYDOWN:
		{
			if(LOWORD(wParam) == VK_F2)
			{
				PrioMan_EndCallMusic(PRIOMAN_PRIORITY_POWERON, TRUE);
			}
			else
				PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;
	case NOSIM_ALART:
		{
			f_HandSetInlineInitialize ();
			PostMessage( hWndPowerOn, WM_CLOSE, 0, 0 );
		}
		break;

	case WM_ERASEBKGND:
		break;
		
    case WM_PAINT :           
        hdc= BeginPaint(hWnd, NULL);
		if(hOnGif)
			PaintAnimatedGIF(hdc, hOnGif);
        EndPaint(hWnd, NULL);  
        break;
        
  	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
        
    case WM_DESTROY:
		
		PrioMan_EndCallMusic(PRIOMAN_PRIORITY_POWERON, TRUE);

        hWndPowerOn = NULL;
        UnregisterClass("PlayPowerOnWindowClass", NULL);
        break;
        
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}    
/********************************************************************
* Function   StartupWnd(void)  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static HWND hStartupWnd;
static	LRESULT	StartupWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam);
BOOL StartupWnd(void)
{
    WNDCLASS wc;
	
	wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    
    // poweron poweroff window
    wc.lpfnWndProc   = StartupWndProc;
    wc.lpszClassName = "StartupWindowClass";   
	if(!RegisterClass(&wc))
	{
	     return FALSE;
	}     
    
    hStartupWnd = CreateWindow("StartupWindowClass",
        "",
        WS_VISIBLE|WS_BORDER,
        0,	
        0,
        DESK_WIDTH,	
        DESK_HEIGHT,
        NULL, NULL,	NULL, NULL);
	
	ShowWindow(hStartupWnd, SW_SHOW);
	UpdateWindow(hStartupWnd);
	
    return TRUE;
}
static	LRESULT	StartupWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam)
{
    LRESULT lResult = FALSE;
    HDC     hdc;    
	SIZE    size;
	int     x=0, y=0;
	static STARTUP_EFFECT istartup = SE_DEF;
	char    DeskPhrase[MAXSTARTUPTXTLEN +1];
	char    DeskShowPhrase[61];
	char    StartPic[PREBROW_MAXFULLNAMELEN +1];
 
    
    switch (wMsgCmd)
    {
    case WM_CREATE:	
		
		istartup = GetStartupEffect();
		nStartupTimer = SetTimer(hWnd, TIMERID_STARTUP, 2000, NULL); 
        break;
		
	case WM_KEYDOWN:
		{
			if(LOWORD(wParam) == VK_F2)
			{
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				f_HandSetInlineInitializeEx();
			}
			else
				PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		}
		break;

	case WM_ERASEBKGND:
		break;
		
    case WM_PAINT :           
		{
			RECT rect;

			hdc= BeginPaint(hWnd, NULL);
			SetRect(&rect, 0, 0, DESK_WIDTH, DESK_HEIGHT);
			ClearRect(hdc, &rect, 0xffffff);

			if(istartup == SE_TXT)
			{
				memset(DeskPhrase, 0, MAXSTARTUPTXTLEN + 1);
				memset(DeskShowPhrase, 0, 61);
				GetGreetingTxt(DeskPhrase);

				FormatStr(DeskShowPhrase,DeskPhrase);

				if (DeskShowPhrase[0] != '\0')
					DrawText(hdc, DeskShowPhrase,-1, &rect, DT_CENTER|DT_HCENTER|DT_VCENTER);
				else
					DrawText(hdc, "Welcome",-1, &rect, DT_CENTER|DT_HCENTER|DT_VCENTER);
			}
			else if(istartup == SE_PIC)
			{
				memset(StartPic, 0, PREBROW_MAXFULLNAMELEN +1);
				GetStartupPic(StartPic);
				if(StartPic[0] != 0)
				{
					GetImageDimensionFromFile(StartPic,&size);
					if (size.cx < DESK_WIDTH)
						x = (DESK_WIDTH - size.cx)/2;
					if (size.cy < DESK_HEIGHT - TITLEBAR_HEIGHT)
						y = (DESK_HEIGHT- TITLEBAR_HEIGHT - size.cy)/2;
					DrawImageFromFile(hdc, StartPic, x,y, SRCCOPY);
				}

			}
			EndPaint(hWnd, NULL);  
		}
        break;
        
    case WM_TIMER:     

		if(nStartupTimer != 0 && (istartup == SE_PIC || istartup == SE_TXT))
		{
			KillTimer(hWnd, TIMERID_STARTUP);
			nStartupTimer = 0;
		}
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		f_HandSetInlineInitializeEx();
		break;

	case WM_CLOSE:
		if(nStartupTimer != 0)
		{
			KillTimer(hWnd, TIMERID_STARTUP);
			nStartupTimer = 0;
		}
		DestroyWindow(hWnd);
		break;
        
    case WM_DESTROY:

        UnregisterClass("StartupWindowClass", NULL);
        break;
        
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
void AfterPhoneCode(void)
{
	static STARTUP_EFFECT istartup = SE_DEF;
	
	istartup = GetStartupEffect();
	if(istartup == SE_DEF)
		f_HandSetInlineInitializeEx();
	else//enter startup window
	{
		if(hWndPowerOn)
			PostMessage( hWndPowerOn, WM_CLOSE, 0, 0 );
		StartupWnd();
	}

	//	alarm power on and auto power on happen sequencely,
	//	we should check if there is alarm window shown and bring it to top
	if ( Clock_IsAlarming() )
		BringWindowToTop( GetAlarmTopWnd() );
}    
/********************************************************************
* Function   PhoneCodeWnd(void)  
* Purpose    /
* Params     
* Return     
* Remarks      
**********************************************************************/
static char	*SOSPhoneNum[]=
{
	"112",
//	"110",
//	"119",
//	"114",
//	"120",
	"911",
	"08",
	""
};
#define PHONECODE_CALL   WM_USER + 200
#define REQUEST_CODERIGHT WM_USER + 201
#define REQUEST_CODE      WM_USER + 202
#define REQUEST_NOSIM     WM_USER + 203
#define MIN_CODELEN      4
#define MAX_CODELEN      8
static HWND hPhoneCodeWnd;
static	LRESULT	PhoneCodeWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam);
BOOL PhoneCodeWnd(void)
{
    WNDCLASS wc;
	char strCode[MAX_CODELEN +1];
	
	wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    
    // poweron poweroff window
    wc.lpfnWndProc   = PhoneCodeWndProc;
    wc.lpszClassName = "PhoneCodeWndClass";   
	if(!RegisterClass(&wc))
	{
	     return FALSE;
	}     
    
    hPhoneCodeWnd = CreateWindow("PhoneCodeWndClass",
        "",
        WS_VISIBLE,
        0,	
        0,
        DESK_WIDTH,	
        DESK_HEIGHT,
        NULL, NULL,	NULL, NULL);
	
	ShowWindow(hPhoneCodeWnd, SW_SHOW);
	UpdateWindow(hPhoneCodeWnd);

	memset(strCode, 0, MAX_CODELEN + 1);
	PLXVerifyPassword(hPhoneCodeWnd, ML("Security"), ML("Enter Phone\r\nlock code:"), 
		strCode, MAKEWPARAM(MIN_CODELEN, MAX_CODELEN), ML("Ok"),NULL, -1, PHONECODE_CALL);
	
    return TRUE;
}
static	LRESULT	PhoneCodeWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                               LPARAM lParam)
{
    LRESULT lResult = FALSE;
    HDC     hdc;    
 
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		
        break;
	case REQUEST_NOSIM:
		{
			char strCode[MAX_CODELEN +1];
			
			memset(strCode, 0, MAX_CODELEN + 1);
			PLXVerifyPassword(hPhoneCodeWnd, ML("Security"), ML("Enter Phone\r\nlock code:"), 
				strCode, MAKEWPARAM(MIN_CODELEN, MAX_CODELEN), ML("Ok"),NULL, -1, PHONECODE_CALL);
		}
		break;

	case REQUEST_CODE:
		{
			 
			char strCode[MAX_CODELEN+1];
			
			memset(strCode, 0, MAX_CODELEN + 1);
			
			PLXVerifyPassword(hWnd, ML("Security"), ML("Enter Phone\r\nlock code:"), 
				strCode, MAKEWPARAM(4, 8), ML("Ok"),NULL, -1, PHONECODE_CALL);
		}
		break;
		
	case REQUEST_CODERIGHT:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	case PHONECODE_CALL:
		{
			 char cLockcode[MAX_CODELEN + 1];
			 char strCode[MAX_CODELEN +1];

			 if(lParam == NULL)
				 break;
			 
			 memset(cLockcode,0, MAX_CODELEN + 1);
			 memset(strCode, 0, MAX_CODELEN + 1);
			 
			 GetSecurity_code(cLockcode);
			 strcpy(strCode, (char*)lParam);
			 
			 if (cLockcode[0] !=0)
			 {
				 if(strcmp(strCode,cLockcode) != 0)
				 {
					 PLXTipsWinOem(NULL,hWnd, REQUEST_CODE, ML("Phone lock\r\ncode wrong"), ML("Security"), Notify_Failure, NULL,
						 NULL, WAITTIMEOUT );
				 }
				 else
				 {
					 PLXTipsWinOem(NULL, hWnd, REQUEST_CODERIGHT, ML("Phone lock\r\ncode right"), ML("Security"), Notify_Success, NULL,
						 NULL, WAITTIMEOUT);
					 
				 }
			 }
		}
		break;
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F1:
		case VK_RETURN:
			{
				char strPhoneNumber[MAX_CODELEN+1];
				
				if(lParam == NULL)
					break;
				
				memset(strPhoneNumber, 0, MAX_CODELEN + 1);
				strcpy(strPhoneNumber, (char*)lParam);
				if(findstr(SOSPhoneNum, strPhoneNumber))
				{
					//	ME_VoiceDial(hWnd,IDC_TEST,"112",NULL);
					APP_CallPhoneNumber(strPhoneNumber);
				}
				
			}
			break;
		default:
			PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;	
		
	case WM_ERASEBKGND:
		break;
		
    case WM_PAINT :           
		{
			hdc= BeginPaint(hWnd, NULL);
		
			EndPaint(hWnd, NULL);  
		}
        break;     
   
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
        
    case WM_DESTROY:
		AfterPhoneCode();
        UnregisterClass("PhoneCodeWndClass", NULL);
        break;
        
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}    
/********************************************************************
* Function   PlayPowerOn  
* Purpose    play power on aniamte gif and sound
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL PlayPowerOn(BOOL bOpen)
{
    WNDCLASS wc;

    wc.style         = 0;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    
    // poweron poweroff window
    wc.lpfnWndProc   = PowerOnWndProc;
    wc.lpszClassName = "PlayPowerOnWindowClass";   
	if(!RegisterClass(&wc))
	{
		if(IsWindow(hWndPowerOn))
		{
			PostMessage(hWndPowerOn, WM_CLOSE, 0, 0);
		}
		RegisterClass(&wc);
	}     
    
    hWndPowerOn = CreateWindow("PlayPowerOnWindowClass",
        "",
        WS_VISIBLE|WS_BORDER,
        0,	
        0,
        DESK_WIDTH,	
        DESK_HEIGHT,
        NULL, NULL,	NULL, NULL);

    return TRUE;
}
/********************************************************************
* Function   MobileInit2  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/

void RegisterNotification(HWND hWnd)
{
	ME_GetHomeZone			(hWnd, WM_HOMEZONE);
	ME_RegisterIndication	(ME_URC_CREG, IND_PRIORITY, hWnd, WM_PORG_CELLID);
	ME_RegisterIndication	(ME_URC_UNDERVOLTAGE, IND_PRIORITY, hWnd, WM_PROG_VOLTAGELOW);
	ME_RegisterIndication	(ME_URC_SSTN, IND_PRIORITY, hWnd, WM_PORG_STK);
	ME_RegisterIndication	(ME_URC_CIEV, IND_PRIORITY, hWnd, WM_PROG_SBC);
	ME_GetChargeStatus		(hWnd, WM_PROG_BATTEMPOUTRANGE);
	ME_GetCallForwardStatus	(hWnd, WM_CALLDIVERT, FWD_UNCDTIONAL, CLASS_VOICE);
	ME_GetSALSstatus		(hWnd, WM_ALSLINE);
	ME_InitSTK(hWnd, WM_USER + 623);
	ME_GetCPHSparameters(hWnd, CALLBACK_MESSAGE, 0x6F19);
	ME_GetCPHSparameters(hWnd, WM_MAILBOXNUM, 0x6F17);
}

/*
 *	Get time from Me or GPS server
 */
static void RequestLocalTime(HWND hWnd)
{
	printf("\r\nGet time from Me...\r\n");
	ME_GetClockTime ( hWnd, WM_ME_TIME );
}

/*
 *	Just initialize Me database and alarm mode and charge-only mode need this
 */
static void MobileInit1(void)
{
	if ( ME_Initialize(MOD_MUX)<0 )
		PLXPrintf("\r\n me initialize fail!\r\n");
}
static BOOL bMeInitRate;
BOOL  IfMeInitRate(void)
{
	return bMeInitRate;
}
BOOL MobileInit2(void)
{    
	int fd;
	HWND hWnd;
	unsigned char starttimes=0;
    pm_sim_cop_status = pm_no_sim;		// default setting

	// we should igt MC55 when exit from charge-only and alarm mode
	if( PMS_MODE_ALARM==g_nPowerMode || PMS_MODE_CHARGE==g_nPowerMode )
	{
		printf("\r\nIgt MC55...\r\n");
		DHI_IgniteWirelessModule();
		DHI_Vibration(TRUE);
		Sleep(500);
		DHI_Vibration(FALSE);
		g_nPowerMode = PMS_MODE_NORMAL;
	}
    
	// if the charge only window exist, we should close it
	hWnd = GetElectPowerOffWnd();
	if ( hWnd )
		DestroyWindow(hWnd);
    
    //start play power on cartoon
	if(g_nPowerMode != PMS_MODE_RESET)
		PlayPowerOn(FALSE);

	//init call app
	{
		pGlobalCallListBuf = malloc(sizeof(CALLWNDLISTBUF));
		CALLWND_InitListBuf(pGlobalCallListBuf);
	}
	
	DlmNotify(PS_SETSGN, 0);	// set timer to check signal and battary

	// hardware support flow control, so we have to seperate these codes
	fd = open(PROG_FILE_NAME,O_RDONLY);
	if ( fd<0 )
	{
		// file is not exist, first time we start the phone
		// pmstart.ini will be created in AppDesk.c			
		goto FirstTimeStartPhone;
	}
	else
	{
		read(fd,&starttimes,1);
		close(fd);
		if ( 0xBB != starttimes )
			goto FirstTimeStartPhone;
		else
			goto PhoneStartAfterFirstTime;
	}
FirstTimeStartPhone:
#ifndef _EMULATE_
	bMeInitRate = TRUE;
	if(ME_InitRate(appManger.hDeskWnd, WM_ME_RATE) < 0)
	{
		MobileInit3();
		f_HandSetInlineInitialize();
		return TRUE;
	}
#endif
PhoneStartAfterFirstTime:
	if(!bMeInitRate)
	{
		MobileInit3();
		MobileInit4(appManger.hDeskWnd);
	}
	EnableWatchDog();
	return TRUE;
}
static BOOL bInit = FALSE;
void MobileInit3(void)
{
	if(bInit)
		return;
	
	bInit = TRUE;

	printf("\r\nstart multiplexer.\r\n");
	MUX_Init();					// open multiplexer

	TCPIP_Init( NULL );
	DIALMGR_Initialize();
	SMSDISPATCH_Init();
}
void MobileInit4(HWND hWnd)
{
	
	// get time from MC55
	RequestLocalTime(hWnd);

	//SIM module initialization
	if(ME_GSMInit_Module((HWND)hWnd, WM_ME_COMINIT ) < 0)
	{
	//	MobileInit3();
		f_HandSetInlineInitialize ();
	}
	else
	{
		RegisterNotification(hWnd);
	}
}

extern  BOOL    SMS_Init(void);
extern  BOOL    InitIdleKeyEvent(void);
extern  HWND    GetGPSWnd(void);
extern  int		ReadNetSimLockData(PNSLC pNetSimlockCode);
extern  int		ReadSPSimLockDate(PSPSIMLOCKCODE pSPLC);
extern  int		ReadSPSimLock(PSPSIMLOCKCODE pSPLC);
extern  BOOL    CreateSimLockCodeWnd(HWND hWnd);
extern  void    SetNCK(char* pNck);
extern  void    SetSPCK(char* pSpck);
extern  BOOL    GetPS_Open(void);
static  BOOL    bInitialize = FALSE, bSimLockCheck = FALSE;

/********************************************************************
* Function   f_HandSetInlineInitialize  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
extern BOOL GetSimLockCheckReady(void);
void f_HandSetInlineInitializeEx(void)
{
	
	LIGHTMODE KeyLightMode, DisplayLightMode;

	DisplayLightMode = GetDisplayBackLitMode();
	KeyLightMode = GetKeypadLitMode();
	
	switch(DisplayLightMode) 
	{
	case LS_KEEP_ON:
		break;
	case LS_KEEP_OFF:
		break;
	case LS_AUTO:
		break;
	}
	
	switch(KeyLightMode) 
	{
	case LS_KEEP_ON:
		break;
	case LS_KEEP_OFF:
		break;
	case LS_AUTO:
		break;
	}
	
    InitMBPostMsg();
    
    SMS_Init();	

	//init language
	if (GetAutoLangState())
	{
		SetAutoLanguage(SWITCH_ON);
	}
	else
	{
		SetActiveLanguage(GetActiveLanguage());
	}

	printf("\r\n APPFileInitialize Begin!\r\n");
    AppFileInitialize( pAppMain);
	printf("\r\n APPFileInitialize Over!\r\n");

	printf("\r\n AppDirsInitialize Begin!\r\n");
    AppDirsInitialize( pAppMain);
	printf("\r\n AppDirsInitialize Over!\r\n");

	if(hWndPowerOn)
	{
		PostMessage( hWndPowerOn, WM_CLOSE, 0, 0 );
	}

	ShowWindow(GetGPSWnd(), SW_SHOW);
	MoveWindowToTop(GetGPSWnd()); 
	InitIdleKeyEvent();
	
    if (pm_sim_cop_status != pm_no_me)
        SendMessage(pAppMain->hDeskWnd, WM_TIMER, 3, 0);  

	//simlock check
	if(!GetSIMState())
	{
		EndMobileInit();
		return;
	}
	
	SetNetSimLockEnableStatus(TRUE);
	SetSPSimLockEnableStatus(TRUE);
	SetNCK("12345678");
	SetSPCK("12345678");

	if(GetNetSimLockBlock() || GetSPSimLockBlock())
	{
		EndMobileInit();
		DlmNotify(PS_SETGPRS,4);//GPRS_UNAVAIL
		if(pAppMain && pAppMain->hDeskWnd)
			ME_SetOpratorMode( pAppMain->hDeskWnd, WM_SIMLOCKBLOCK, 2, NULL);
		pm_sim_cop_status = pm_cops_emergencycallonly;
		if(pAppMain && pAppMain->hIdleWnd)
			PostMessage(pAppMain->hIdleWnd, WM_IDLEOPRCHANGED, 0, 0);
	}
	else
	{
	
		if(GetNETSimLock_Open()||GetSPSimLock_Open())
		{
			char strCode[6],netSimlockCode[6];
			PNSLC pNetSimlockCode = NULL, pData = NULL, p = NULL, pTemp =NULL;
			int num = 0, count = 0,counter = 0;
			BOOL bReject = TRUE;
			
			strCode[0] = 0;
			netSimlockCode[0] =0 ;

			if(!GetSimLockCheckReady())
			{
				bSimLockCheck = TRUE;
				return;
			}
		
			if(GetSPSimLock_Open())
			{
				PSPSIMLOCKCODE pSPLCData = NULL,pSPLC = NULL;

				pSPLCData = malloc(sizeof(SPSIMLOCKCODE));
				if(pSPLCData == NULL)
				{
					EndMobileInit();
					return;
				}
				
				pSPLC = malloc(sizeof(SPSIMLOCKCODE));
				if(pSPLC == NULL)
				{
					free(pSPLCData);
					pSPLCData = NULL;
					EndMobileInit();
					return;
				}

				memset(pSPLCData, 0, sizeof(SPSIMLOCKCODE));
				memset(pSPLC, 0, sizeof(SPSIMLOCKCODE));

				num = ReadSPSimLockDate(pSPLCData);
				if(num == -1)
				{
					bSimLockCheck = TRUE;
					return;
				}
				ReadSPSimLock(pSPLC);

				if(stricmp(pSPLCData->strMCC, "001") == NULL&&
					stricmp(pSPLCData->strMNC, "01") == NULL)
				{
					if(pSPLCData != NULL)
						free(pSPLCData);
					
					if(pSPLC != NULL)
						free(pSPLC);

					EndMobileInit();
					return;
				}

				if(pSPLCData->strGID1[0] == 0)
				{
					EndMobileInit();
					return;
				}

				EndMobileInit();

				if(stricmp(pSPLCData->strMCC, pSPLC->strMCC) != NULL||
					stricmp(pSPLCData->strMNC, pSPLC->strMNC) != NULL ||
					stricmp(pSPLCData->strGID1, pSPLC->strGID1) != NULL)
				{
					PLXTipsWin(NULL, pAppMain->hIdleWnd, SIMLOCK_FAIL, ML("SIM rejected"),
						ML("SP simlock"), Notify_Failure, ML("Ok"), NULL, 0);
				}

				if(pSPLCData != NULL)
					free(pSPLCData);
				
				if(pSPLC != NULL)
					free(pSPLC);
				
				return;
			}

			if(GetNETSimLock_Open())
			{
				num =ReadNetSimLockData(NULL);
				if(num == -1)
				{
					bSimLockCheck = TRUE;
					return;
				}
				count = ReadNetSimLock(NULL);
				
				if(0 == count)
				{
					EndMobileInit();
					PLXTipsWin(NULL, pAppMain->hIdleWnd, SIMLOCK_FAIL, ML("SIM rejected"),
						ML("Network simlock"), Notify_Failure, ML("Ok"), NULL, 0);
					return;
				}
				
				pNetSimlockCode = malloc(sizeof(NSLC)*num);
				if(pNetSimlockCode == NULL)
				{
					EndMobileInit();
					return ;
				}
				pData = malloc(sizeof(NSLC)*count);
				
				if(pData == NULL)
				{
					free(pNetSimlockCode);
					EndMobileInit();
					return;
				}
				memset(pNetSimlockCode, 0, sizeof(NSLC)*num);
				memset(pData, 0, sizeof(NSLC)*count);
				
				ReadNetSimLockData(pNetSimlockCode);
				ReadNetSimLock(pData);
				
				p = pNetSimlockCode;
				pTemp = pData;
				counter = count;
				
				if(stricmp(p->strMCC, "001") == NULL &&
					stricmp(p->strMNC, "01") == NULL)
				{
					if(pData != NULL)
						free(pData);
					
					if(pNetSimlockCode != NULL)
						free(pNetSimlockCode);
					
					EndMobileInit();
					return;
				}
				
				p = pNetSimlockCode;
				pTemp = pData;
			
				
				while (p && pTemp &&(counter > 0))
				{
					
					if((stricmp(p->strMCC, pTemp->strMCC) == 0)
						||(stricmp(p->strMNC, pTemp->strMNC) == 0))
					{
						
						bReject = FALSE;
					}
					counter--;
					//p ++;
					pTemp ++;
				}
				
				if(pData != NULL)
					free(pData);
				
				if(pNetSimlockCode != NULL)
					free(pNetSimlockCode);

				if(bReject)
				{
					EndMobileInit();
					PLXTipsWin(NULL, pAppMain->hIdleWnd, SIMLOCK_FAIL, ML("SIM rejected"),
						ML("Network simlock"), Notify_Failure, ML("Ok"), NULL, 0);
					return;
				}
			
			}	
			
		}
		
	}
	EndMobileInit();
}
/********************************************************************
* Function   f_HandSetInlineInitialize  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static  BOOL   bPhonecodeCheck;
extern  BOOL   GetPS_Open(void);
extern  BOOL   GetPSData(char* pPSCode);
extern  BOOL   GetPSCode(char* pPSCode);
extern  BOOL   HavePhonecodeCheck(void);
extern  HWND   GetPhoneLockWnd();
BOOL IsPhoncodeCheck(void)
{
	return bPhonecodeCheck;
}
void SetPhoncodeCheck(BOOL bSet)
{
	bPhonecodeCheck = bSet;
}
void	f_HandSetInlineInitialize ( void )
{
	
	if(bInitialize)
		return;
	
	
	bInitialize = TRUE;

    switch(pm_sim_cop_status)
    {
    case pm_no_me:
        {
            strcpy(ProgDeskIdle.cPlmnStr, ML("no ME module"));         
            break;
        }
    case pm_no_sim:
        strcpy(ProgDeskIdle.cPlmnStr, ML("Insert SIM card"));
        break;
    case pm_sim_destroy:
        strcpy(ProgDeskIdle.cPlmnStr, ML("SIM card is destroied"));
        break;
    case pm_cops_searching:
        strcpy(ProgDeskIdle.cPlmnStr, ML("Searching for networks"));
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
    case pm_cops_link:
		break;
	case pm_cops_gsmoff:
		strcpy(ProgDeskIdle.cPlmnStr, ML("GSM radio part off"));
		break;
    default:
        break;
    }

	if(GetPS_Open()== TRUE)
	{
		if(GetSIMState() == 0)
			PhoneCodeWnd();	
		else
		{
			if(HavePhonecodeCheck())//after pin code request
				AfterPhoneCode();
			else
			{
				bPhonecodeCheck = TRUE;//need to check phonecode
				ME_GetSubscriberId(pAppMain->hDeskWnd, WM_SETLAN);
			}
		}
	}
    else
	{
	   AfterPhoneCode();
	}
	
}
/********************************************************************
* Function   InitFontForApp  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
#ifdef LANGUAGE_CHN
#define  LARGESIZE   16
#define  SMALLSIZE   12
#else
#define  LARGESIZE   23
#define  SMALLSIZE   16
#endif
static BOOL InitFontForApp(void)
{
	hFontlarge = CreateFont(LARGESIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
				DEFAULT_QUALITY, DEFAULT_PITCH, "large" );
	hFontsmall = CreateFont(SMALLSIZE, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
				DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, 
				DEFAULT_QUALITY, DEFAULT_PITCH, "small" );

	if(!hFontlarge || !hFontsmall)
		return FALSE;

	return TRUE;
}
/********************************************************************
* Function   GetFontHandle  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL GetFontHandle(HFONT *hFont, int nType)
{
	if(!hFontlarge || !hFontsmall)
		return FALSE;

	if(nType == LARGE_FONT)
		*hFont = hFontlarge;
	else if(nType == SMALL_FONT)
		*hFont = hFontsmall;

	return TRUE;
}
/********************************************************************
* Function   DeleteFont  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL DeleteFont()
{
	if(!hFontlarge)
		return FALSE;

	DeleteObject(hFontlarge);
	hFontlarge = NULL;

	if(!hFontsmall)
		return FALSE;

	DeleteObject(hFontsmall);
	hFontsmall = NULL;

	return TRUE;
}
/********************************************************************
* Function   IsResetPOn  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL IsResetPowerOn(void)
{
	if(g_nPowerMode == PMS_MODE_RESET)	// watchdog reset
		return TRUE;
	else
		return FALSE;
}

BOOL IsAlarmPowerOn(void)
{
	if ( PMS_MODE_ALARM==g_nPowerMode || PMS_MODE_CHARGE==g_nPowerMode )
		return TRUE;
	return FALSE;
}
/********************************************************************
* Function   IsSimLockCheck 
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
BOOL IsSimLockCheck(void)
{
	return bSimLockCheck;
}
/********************************************************************
* Function   f_SetLockTime 
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
void f_SetLockTime()
{
	SYSTEMTIME Time;    
	
	//if there is no me module, set system default time
	memset(&Time, 0, sizeof(SYSTEMTIME));
	Time.wYear	= 2000;
	Time.wMonth = 1;
	Time.wDay	= 1;
	Time.wHour	= 0;
	Time.wMinute = 0;
	Time.wSecond = 0;
	
	DHI_SetRTC((PSYSTEMTIME)&Time);
	printf("\r\nSet system default time:\r\n");
	printf("Year:%4d  Month:%2d  Day:%2d\r\n",Time.wYear,Time.wMonth,Time.wDay);
	printf("Hour:%4d  Minut:%2d  Sec:%2d\r\n",Time.wHour,Time.wMinute,Time.wSecond);
}
/*
*	file over
*	if you find there has some shortcoming please modify and signed your name
*/

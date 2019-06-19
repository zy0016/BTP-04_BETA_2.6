 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _PLXPDAEX_H

#define _PLXPDAEX_H

//#define  HP30

#include "window.h"
#include "winpda.h"
#include "windebug.h"
#include "hpimage.h"
#include "app.h"
#include "mullang.h"

#ifndef DIAL_MODULE 
#include "plxdebug.h"
#endif


#define _WAVECOM

#define WAITTIMEOUT	      20 
//window position

//#define PLX_WIN_POSITION   0,18,176,202   
////
//#define PLX_WIN_TOP_X    0
//#define PLX_WIN_TOP_Y    18
//#define PLX_WIN_WIDTH    176
//#define PLX_WIN_HIGH     202 

#define PLX_WIN_POSITION   0,15,176,205   
//
#define PLX_WIN_TOP_X    0
#define PLX_WIN_TOP_Y    15
#define PLX_WIN_WIDTH    176
#define PLX_WIN_HIGH     205 


/*********************  progman app need message ID *************************/
/*********************  remain the same interface parameter with original code *************************/

/***	PMI ***/

#define		WM_PROGRAM		 WM_USER+50
#define		WM_TOOLBAR		(WM_PROGRAM+1)
#define		WM_PENWND_QUIT	(WM_PROGRAM+3)
#define		WM_PASSWND_QUIT	(WM_PROGRAM+4)
#define		WM_BATTERY      (WM_PROGRAM+5)
#define		WM_TRANSFER		(WM_PROGRAM+7)
#define		WM_APPNOTIFY	(WM_PROGRAM+8)
#define		WM_RTCTIMER     (WM_PROGRAM+9)


//exit 
#define     WM_QUERYSYSEXIT  (WM_PROGRAM+20)
#define     WM_SYSEXIT       (WM_PROGRAM+21)

#define     SYSEXIT_CONTINUE      100
#define     SYSEXIT_ABORT         101
#define     SYSEXIT_WAITREPLAY    102

//Set Caption and Staticbar colors
#define CAPTIONCOLOR RGB(128,255,255)//RGB(170, 220, 200)//RGB(189, 249, 164)
#define STATICBARCOLOR RGB(0,128,255)//(172, 220, 220)//RGB(189, 249, 164)
#define FOCUSCOLOR  RGB(0,0,128)

/***	app use message ***/

#define		PES_APP_QUIT		2	//	app exit
#define		PES_APP_HIDE		3	//	hide app
#define		PES_APP_SHOW		4	//	show app 
#define		PES_APP_CALL		5	//	call app entry
#define		PES_APP_CREATE		6	//	create an appointed app
#define     PES_SYN_QUIT        7
#define     PES_IME_REGISTER    10  //  register ime
#define     PES_IME_UNREGISTER  11  //  inregiste ime

#define		PES_STCQUIT			PES_APP_QUIT
#define     PES_DLMQUIT        	PES_APP_QUIT							
#define		PES_APPRUN			PES_APP_CALL
#define		PES_APPHIDE			PES_APP_HIDE
#define		PES_APPSHOW			PES_APP_SHOW
#define     PES_APPSTART		PES_APP_CREATE

//for call app
#define CALL_DIAL		0x01
#define CALL_GSMURC		0x02
#define CALL_INCOMING	0x04
BOOL AppActiveCallApp(BYTE CallType); 
void PMShowIdle(void); //show idle view
void PMHideIdle(void); //hide idle view


//define newwork status
#define NONETWORK   0
#define HOMEZONE    1
#define ROAMING     2
#define REJECTSIMCARD 3
#define NOSERVICE   4
#define ONLY_EMERGENCYCALL 5
int PM_GetNetWorkStatus();

/***	program entry parameter ***/
#define  STATIC_APP   0x0001
#define  DLM_APP      0x0002
#define  JAVA_APP     0x0004
#define  HIDE_APP     0x0008
#define  WAP_SHORTCUT 0x0010
#define  SHORTCUT     0x0011//for edit sms mms and email

/***   program parameter  ***/
#define		APP_SYSTEM			2000
#define		APP_INIT            0	//	app initialize		
#define		APP_INITINSTANCE    1	//	app initialize hinstance used in dynamic
#define		APP_ACTIVE          2	//	app active and show app interface
#define		APP_INACTIVE        3	//	app hide interface
#define     APP_INACTIVE_NETCAT    1
#define     APP_GETOPTION       4
#define     APP_SETOPTION       5
#define     APP_TIMECHANGED     11
#define		APP_DESTROY			16	//	app destroy	,call by other app
#define     APP_EXIT            10  //  dlm message 
#define		APP_ALARM			100
#define		APP_TIMER			101

#define     DLL_INIT			6
#define     DLL_INITINSTANCE	7
#define     DLL_GETFUNADD		8
#define     DLL_EXIT			9

#define		APP_GETICON			(APP_SYSTEM+1)	//	app get info
#define		APP_GETVERSION		(APP_SYSTEM+2)	//	app get version
#define		APP_GETDATA			(APP_SYSTEM+3)	//	app get data
#define		APP_SETDATA			(APP_SYSTEM+4)	//	app Set data
#define		APP_GETHELP			(APP_SYSTEM+5)	//	app get info

#define AS_NOTRUNNING       0
#define AS_INACTIVE         1
#define AS_ACTIVE           2
#define AS_HELP				3
#define AS_ICON				4
#define AS_STATE			5
#define AS_IMEINFO          6  //ime define

#define AS_APPWND           7

#define WM_RINGMSG    WM_USER+1001
#define DIRECT_DIAL   WM_USER+1002
#define PM_DLMEXIT    WM_USER+1003
#define WM_PROG_BTCOM WM_USER+1004

/********************  end define ************************************************/

#define	_WAVECOM

//for ignor switching focus of control in wml browser
#define CS_NOSYSCTRL		0x20000000L

HWND    GetDesktopWindow(void);
BOOL	Init_WaitWindow(void);
//void	WaitWindowState(BOOL bShow);
void	WaitWindowState(HWND hParent, BOOL bShow);
BOOL	IsWaitWindowShow();
//add by mxlin
#define WC_RECIPIENT   "PBRECIPIENTWNDCLASS"
BOOL	Recipient_RegisterClass(void);
#define	WC_FORMVIEW	"FORMVIEWWNDCLASS"
#define	FORMVIEWER	WC_FORMVIEW
BOOL	FormView_Register(void);

void Play(char *pName);

void EnableFastKey( void );
void DisableFastKey( void );
void EnableTouchpen( void );
void DisableTouchpen ( void );


#define WM_CHECKCURSOR	WM_USER + 700

#define WM_ME_INIT		WM_USER + 800
#define ME_SUCCESS    0
#define ME_ERROR      1

/*----- Program Manager interface function declare ---------*/

typedef struct tagAPPENTRY{
	DWORD	    	nCode;	
	WPARAM		    wParam;
	LPARAM		    lParam;
	HINSTANCE		hApp;
	char*		    pAppName;
} APPENTRY, *PAPPENTRY;

typedef struct pmcallapp_s
{
    LPSTR	v_pPath;
    DWORD	v_nCode;	
    WPARAM	v_wParam;
    LPARAM	v_lParam;
    
}	PMCALLAPP, *HPMCALLAPP;


DWORD DlmNotify( WPARAM wParam, LPARAM lParam );
extern  DWORD RMonitorNotify(DWORD Param1, DWORD Param2);
extern  BOOL GetDIALState();
extern	BOOL GetMultState();

#define MSG_NO_CARRIER			(10013)

#define PS_SIGNALSTATE      30       //singnal states
#define PS_BATTERY_CHARGE	31      //battery states
#define PS_MAIL				32      //mail symbol
#define PS_SINFO			33      //small information symbol
#define PS_SETDESKTOP       34      //set desktop
#define PS_SETTIME          35
#define PS_CALLDIALCON      37
#define PS_RUNDIALCON       38
#define PS_STOPDIALCON      39
#define PS_REGRINGWND       40
#define PS_GETREGRINGWND    41
#define PS_BATTERY			42
#define PS_AUTOPOWERON      43


/* for PS_SINGASTATE*/
#define SS_UNTOUCH          0
#define SS_LOW              1
#define SS_HIGH             2
#define SS_FULL             3

#define SMSNONE      0
#define SMSIN        1
#define SMSFULL      2
#define SMSINFULL    3 

/* for PS_BATTERY_CHARGE */
#define BATTERY_EMPTY		0		//lParam
#define BCHARGE_END			1
#define BCHARGE_S			2
#define BCHARGE_2			3
#define BCHARGE_3			4
#define BCHARGE_4			5
#define BCHARGE_5			6

/* for PS_MAIL */
#define MAIL_NEW			0		//lParam
#define MAIL_DISAPPEAR		1

/* for PS_SINFO */
#define SI_NEW		        0		//lParam
#define SI_DISAPPEAR		1

#define WM_SETUPUPDATE		WM_USER+10

//Window message to change right button text for edit 
#define WM_SETLBTNTEXT  (WM_USER + 501)
#define WM_SETRBTNTEXT	(WM_USER + 502)


//add  special messagebox for MPDA
int WINAPI TipMessageBox(HWND hWnd, PCSTR pszText);

DWORD	PDA_InitAppGlobalVarBlock(DWORD nSize);
DWORD	PDA_SetAppGlobalVar(DWORD nType, DWORD nValue);
DWORD	PDA_GetAppGlobalVar(DWORD nType);

typedef struct tagSMSData
{
	char* pSour;		// scr address£¬sender number
	char* pDest;		//des address£¬reciever number£¨sms centrl£©
	unsigned long datalen;		//data length
	unsigned char* pdata;		//data point
}SMSData;

typedef unsigned long  (*PMSG_CALLBACK)(unsigned long  wMsg, \
										unsigned long  Param1, \
										unsigned long  Param2);
BOOL	    PMSG_MessageInit(void);
HANDLE      PMSG_RegisteWnd( DWORD nType, HWND hWnd , PMSG_CALLBACK msg_call);
BOOL        PMSG_UnRegisteWnd(DWORD nType, HANDLE handle);
BOOL	    PMSG_MessageSend(DWORD nType, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

#define		PDA_ACTION_POWERON	0	
#define		PDA_ACTION_SOFTKBD	1	
#define		PDA_ACTION_EXTCARD	2	
#define		PDA_ACTION_FILE		3	
#define		PHONE_SMS_IN	    4

void *		GetResourceSection(HINSTANCE hInstance, DWORD * pSize);
HWND		GetDesktopWindow( void );
BOOL WINAPI	RegisterDesktopWindow( HWND hWnd );

//new added progman definement
typedef enum {
    PROG_IDLETIME,
    PROG_IDLEPLNM,
    PROG_IDLEPROMPT,
    PROG_IDLEDYNM
};
extern void Prog_SetDeskIdle(int iType, int iOffsetX, int iOffsetY);

/*
**	dlm interface
*/
typedef		BOOL (*DLMFILEPROC)( LPCTSTR lpszFileName, long lUserData );
BOOL		EnumDlmFiles(DLMFILEPROC lpfnDlmFile, long lUserData);

DWORD		RunModule(DWORD type, HINSTANCE hInstance, DWORD param1, DWORD param2);
void		UnloadModule(HINSTANCE hInstance, HINSTANCE hModuleInst);
HINSTANCE   DllLoadLibrary(LPCSTR lpName);
DWORD       GetFuncAddress(HINSTANCE hInstance, LPCSTR lpFuncName);
void        DllFreeLibrary(HINSTANCE hInstance);

#define		HEAP_EXPANDABLE		0x10		/* Heap expandable */
//int			LocalCreateHeap (unsigned long heap_size, int flags); /* This is in static library */
//void		LocalDestroyHeap (void);
//int			CRT_InitInstance(HINSTANCE hInstance);

#ifdef	_EMULATE_

#define		DLMEXPORT	_declspec(dllexport)

#else

#define		DLMEXPORT

#endif //_EMULATE_

#define		SHGFI_ICON		0x55
#define		NAPP_NAME_PATH	12

typedef	struct	_SHFILEINFO{//	filenode struct

//	unsigned long	hBitmap;					//	fileicon
	HICON	 hIcon;	
	char	 szName[NAPP_NAME_PATH];			//	file displayname	1:appname

} SHFILEINFO;

DWORD WINAPI SHGetFileInfo(LPCTSTR pszPath, DWORD dwFileAttributes, 
						   SHFILEINFO *psfi, UINT cbFileInfo, UINT uFlags);

HINSTANCE	LoadModule(HINSTANCE hInstance, LPCSTR pName);


/**********music play interface********************************************************/
int PlayMusic ( char * pFileName, int StartPoint, int PlayMode);
int PlayStopAll (int seqHd);

extern void MsgSelWin( HWND hCall, char *szPrompt, char *szCaption ,UINT msg);
#define			WM_SELCANCEL				WM_USER + 551
#define			WM_SELOK					WM_USER + 552

//color control
#define WM_COLOR_SELECT	(WM_USER+12345)
extern  int ReadDefaultColorFile();
extern  void WriteDefaultColorFile();
extern HWND InitColorWin(HWND hWndB);

//return space status
#define SPACE_CHECKERROR     0
#define SPACE_AVAILABLE      1
#define SPACE_NEEDCLEAR      2
#define SPACE_NEEDDELFILE    3
int IsFlashAvailable( int nSize );

/************ notify device mode icon  ********/
#define ICON_SET            50
#define ICON_CANCEL         51

#define PS_SETCALLDIVERT    60
#define PS_CALLDIVERTLINE1  61
#define PS_CALLDIVERTLINE2  62
#define PS_SETROAMING       63
#define PS_SETCIPHER	    64
#define PS_SETHOMEZONE      65
//#define PS_SETACTIVELINE    66
#define PS_SETACTIVELINE1   67
#define PS_SETACTIVELINE2   68
#define PS_SETBLUETOOTHON   69
#define PS_SETBLUETOOTHOFF  70
#define PS_SETHEADSET       71
#define PS_SETCARKIT        72  
#define PS_SETMISSEDCALL    73
#define PS_SETMSG			74
#define PS_MSGUNREAD        75
#define PS_MSGUNREAD1       76
#define PS_MSGUNREAD2       77
#define PS_MSGOVERFLOW		78
#define PS_MSGOUTBOX        79 
#define PS_SETVOICEMAIL     80
#define PS_SETVOICEMAIL1    81
#define PS_SETVOICEMAIL2    82
#define PS_SILENCE			83
#define PS_SPROHTER			84

#define PS_KEYLOCK          86
#define PS_ALARMCLOCK       87
#define PS_SETSGN           88
#define PS_SETBAT			89
#define PS_POWEROFF		    90
#define PS_POWERON          91
#define PS_POWERONOFF       92
#define PS_USB              93
#define PS_SECURITYCON      94

//setting parameters
#define PS_SETLANGUAGE      42
#define PS_SETSTYLE         43
#define PS_SETCOLOR         44
#define PS_DISAUTOSHUTCPU   45
#define PS_MISSEDEVENTSCAHNGE 46
#define PS_LOCK_ENABLE        47
#define PS_LOCK_DISENABLE     48 
#define PS_SETGPRS          95
#define PM_TIMECHANGED      96

#define PS_FRASHGPS         97
#define PMM_OPERATOR_CHANGE  98

#define PS_NEWMSG           99
#define PS_IDLEMODETXT      100
#define PS_ALARMMODE         101

#define PMM_NEWS_ENABLE_SCRSAVE	0x0101

#define PMM_NEWS					0x1000
#define PMM_NEWS_ENABLE				(PMM_NEWS+1)	
//	enable some functions//	lParam:	 below//	return falure or success
#define PMM_NEWS_DISABLE			(PMM_NEWS+2)
//	disable some functions//	lParam:	below
#define PMF_KEYLIGHT			    0x0010
#define PMF_MAIN_BKLIGHT			0x0020
#define PMF_AUTO_SHUT_DOWN			0x0100
#define PMF_AUTO_SHUT_KEYLIGHT		0x0200
#define PMF_AUTO_SHUT_MAIN_LIGHT	0x1000
#define PMF_AUTO_SHUT_CPU			0x2000

#define PMM_NEWS_SHUT_DOWN			(PMM_NEWS+3)
#define PMM_NEWS_SET_DELAY			(PMM_NEWS+26)

//	HIWORD wParam: below£¬lParam  seconds
#define PMS_DELAY_SHUT_KEYLIGHT		0x01
#define PMS_DELAY_SHUT_LIGHT_MAIN	0x03
#define PMS_DELAY_SHUT_CPU			0x05


#define PMM_NEWS_GET_DELAY			(PMM_NEWS+27)
#define PMM_NEWS_INVALID_SOUND_REGN (PMM_NEWS+28)
#define PMM_NEWS_CALL_APP			(PMM_NEWS+29)
#define PMM_NEWS_GET_FLAGS          (PMM_NEWS+30)


#define  WM_SETVALUE        WM_USER +200
#define  WM_SETARROW        WM_USER +201

DWORD DlmNotify( WPARAM wParam, LPARAM lParam );

//create font
#define  SMALL_FONT 0
#define  LARGE_FONT 1
extern BOOL GetFontHandle(HFONT *hFont, int nType);

#define COLOR_TRANSBK        RGB(255, 0, 255)

//notify missed events number changed
#define CALL_APP   0x0001
#define MSG_APP    0x0002
#define MAIL_APP   0x0004
#define ADBOOK_APP 0x0008
#define PUSHMSG    0x0010

#define DEC_COUNT  0
#define ADD_COUNT  1 

//notify usb in or out
#define USB_IN  0x0001
#define USB_OUT 0x0002

//define for call app indirectly
#define DIALED_CALL  0x0004  
#define UNANSWER_CALL 0x0001

#define SHORTCUTFORWAP  0x0001//for wap shortcur wparam

void f_EnableKeyRing(BOOL bEnable);// enable or disable key ring

//wparam
#define PS_GPRSGSM 200    
//lparam
//define for GPRS & GSM indicator
#define TRANFERING_GPRS     0x0001 //gprs tranfering
#define SUSPENDER_GPRS		0x0002 //gprs suspended
#define ATTACHED_GPRS		0x0004 //gprs attached 
#define AVAILABLEUNATTACHED_GPRS     0x0008//gprs available but unattached 
#define AVAILABLE_GSM       0x0010//GSM available
#define OFF_GSM             0x0020//GSM off

#define    MECPHSINFOMAXLEN    60//ME_CPHSINFOMAXLEN

typedef struct tagNSLC
{
	char strMCC[3 +1];
	char strMNC[2 + 1];
	struct tagNSLC *pNext;
}NSLC, *PNSLC;

typedef struct tagSPSIMLOCKCODE
{
	char strMCC[3 +1];
	char strMNC[2 + 1];
	char strGID1[MECPHSINFOMAXLEN + 1];
}SPSIMLOCKCODE, *PSPSIMLOCKCODE;


#define NETWORK_SC  1
#define SP_SC       2

BOOL IsSimLockContradict(int nType, char* strMCC, char*strMNC);
//Network simlock
// block
BOOL GetNetSimLockBlock(void);
void SetNetSimLockBlock(BOOL bStatus);
//active/inactive
BOOL    GetNETSimLock_Open(void);
BOOL    SetNETSimLock_Open(BOOL bAuto);

int ReadNetSimLock(PNSLC pNetSimlockCode);
BOOL   SetNetSimLockCode(PNSLC pNetSimlockCode, int n);

//NCK validate
BOOL NCKValidate(char* strNck);
//set NCK
void   SetNCK(char* pNck);
BOOL   GetNCK(char* pNck);

//SP simlock
BOOL    GetSPSimLock_Open(void);
BOOL    SetSPSimLock_Open(BOOL bAuto);

//active or inactive
BOOL GetSPSimLockBlock(void);
void SetSPSimLockBlock(BOOL bStatus);

//SPCK validate
BOOL SPCKValidate(char* strNck);

int ReadSPSimLock(PSPSIMLOCKCODE pSPLC);
BOOL   SetSPSimLockCode(PSPSIMLOCKCODE pSPSimlockCode);

//set SPCK
void   SetSPCK(char* pNck);
BOOL   GetSPCK(char* pSpck);

//check status
int ReadSPSimLock(PSPSIMLOCKCODE pSPLC);
int ReadNetSimLock(PNSLC pNetSimlockCode);

//check simlock disable
BOOL    GetNetSimLockEnableStatus(void);
BOOL    SetNetSimLockEnableStatus(BOOL bAuto);
BOOL    GetSPSimLockEnableStatus(void);
BOOL    SetSPSimLockEnableStatus(BOOL bAuto);

//remains
BOOL GetNCKRemains(char* strRemains);
void SetNCKRemains(char* strRemains);
BOOL GetSPCKRemains(char* strRemains);
void SetSPCKRemains(char* strRemains);

#define GPRS_CONNECT   0//gprs connected
#define CALL_DATA      1//in call 
#define ME_WAITRESP    2//me wait for response
#define PSYNC_TANSFER  3//psync data transfering
#define USB_ATTACHED   4//usb attached
#define SIM_READ       5//read sim file
#define BLUETOOTH_DATA 6  //bluetooth data transfer
#define CAL_ALARM      7//calendar alarm
#define CLOCK_ALARM    8//clock alarm
#define BKLIGHT        9
#define INIT_FLAG      10
#define ENGINEERINGMODE	11	//engineering mode, only used by engineering application
#define KEYLIGHT        12
#define INPOWEROFF		13 // power off 
#define KEYEVENTS       14
#define SOUNDPLAY       15//play sound
#define PICBROWSE       16//browser picture
#define GPSDATA         17//used for GPS

BOOL  f_sleep_register(int handle );
BOOL f_sleep_unregister(int handle);

//Get cellid 
void  PM_GetCellIdFunc(char* strCellId);

// volume level 
#define  LEVEL_ZERO       0x00	// -21	db
#define  LEVEL_ONE   	  0x2B	// 0	db
#define  LEVEL_TWO   	  0x2F	// 6	db
#define  LEVEL_THREE 	  0x32	// 10.5	db
#define  LEVEL_FOUR  	  0x35	// 15	db
#define  LEVEL_FIVE  	  0x39	// 21	db

#define DEFAULT_LEV LEVEL_TWO

enum  disable_poweroff{
INCALL, 
RETRIEVMMS,
RETRIEVMAIL,
PCSYNC,
CALENDARALARM,
CLOCKALARM
};

BOOL f_DisablePowerOff(int handle);
BOOL f_EnablePowerOff(int handle);

//gps api
//GPS position status
#define GPS_PS_VALID  0x0001 //green GPS has a fresh and valid position fix.
//yellow GPS is attempting to calculate a position fix but doesnot have a fresh and valid fix.
#define GPS_PS_NOVALID 0x0002  
#define GPS_PS_SLEEP   0x0004//blue GPS is sleeping between attempts at calculating a position fix.
#define GPS_PS_OFF     0x0008//grey Gps is off
#define GPS_PS_ERROR   0x0010 //red GPS has encountered an error.

//GPS Profile
#define GPS_PROFILE_USED  0x0001 //This icon identifies the currently used GPS profile.
#define GPS_PROFILE_OFF  0x0002 // The GPS module is off
//The icon is shown when the device uses parameters
//differing from those defined by the current GPS profile.
#define GPS_PROFILE_OTHERS 0x0004 

//Position quality
#define QUALITY_LEVEL1  1
#define QUALITY_LEVEL2  2
#define QUALITY_LEVEL3  3
#define QUALITY_LEVEL4  4 

//GPS status
#define GPS_VALID  0x0001
#define GPS_SLEEP  0x0002
#define GPS_ERROR  0x0004

BOOL SetGpsPStatus(BYTE iPstatus);
BOOL SetGpsProfile(BYTE  iGPSProfile);
BOOL SetGpsQuality(int   iQuality);
BOOL SetGpsStatus(BYTE iGpsStatus);
BOOL SetGpsBoostStatus(BOOL bOn);

BOOL CreateGPSprofileWnd(HWND hWnd);


/**********************************************************************
 * Function     GetExtentFittedText
 * Purpose      
 * Params       hdc, lpszSrc, cchSrc, lpszDst,
 *              cchDst, nMaxExtent, chSuffix, nRepeat
 * Return       
 * Remarks      
 **********************************************************************/

BOOL GetExtentFittedText(HDC hdc, LPCSTR lpszSrc, int cchSrc,
                         LPSTR lpszDst, int cchDst, int nMaxExtent,
                         char chSuffix, int nRepeat);

extern void OS_UpdateScreen(int x1,int y1,int x2,int y2);

#endif // _PLXPDAEX_H

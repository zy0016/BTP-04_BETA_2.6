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

#ifndef _PROGMAN_H_
#define _PROGMAN_H_

#include    "window.h"
#include    "winpda.h"
#include    "app.h"
//#include    "MEdrv.h"
#include	"appmain.h"
#include    "hpimage.h"
#include    "setting.h"
#include	"setup.h"
#include    "mullang.h"
#include    "imesys.h"
#include	"pubapp.h"
#include	"stdio.h"
#include    "PreBrowHead.h"
#include	"plxdebug.h"
#include "stk/StkEx.h"

//#define printf MsgOut

#define PROG_PATH "/rom/progman/"
#define PROG_FILE_NAME	"/mnt/flash/pmstart.ini"
#define _NEW_ME
#define _MONITOR_NEW

#define MYMESSAGE_RIGHTDOWN MAKEWPARAM(WM_USER+11 ,0)
#define MYMESSAGE_LEFTDOWN  MAKEWPARAM(WM_USER+12 ,1)

#define MYWM_SETTITLE  WM_USER + 1101

#define MYWM_NOTIFYMSG WM_USER + 1002
#define WM_SMS_MMS     WM_USER + 1003
#define WM_DEL_SMS     WM_USER + 1004
#define WM_MMS_OK      WM_USER + 1005
#define WM_WAP_CUT     WM_USER + 1006
#define WM_SHUTDOWN    WM_USER + 1010

#define WM_DLMNODEADD  WM_USER + 1011 //add dlm icon 
#define WM_DLMNODEDEL  WM_USER + 1012//del dlm icon
#define NOSIM_ALART	   WM_USER +  1013// no sim card


#define IDB_EXIT	   10001
#define IDB_OK         10002
#define	ID_APPLIST	   10003

#define PM_MISSEVENTCHANGE   401//idle missed events number change



//#define MYWM_SETSMS  1
//#define MYWM_SETTEL  2
#define MYWM_SETBAT  3
#define MYWM_SETSGN  4

//#define SMSMASK 0x0300
//#define BATMASK 0x00f0
//#define SGNMASK 0x000f

//#define SCREEN_MAIN     0
//#define SCREEN_SECOND   1

/* set desk shape */
#define DESK_WIDTH			(GetSystemMetrics(SM_CXSCREEN))  
#define DESK_HEIGHT			(GetSystemMetrics(SM_CYSCREEN))
#define DESK2_WIDTH                 96
#define DESK2_HEIGHT                64

#define STATUSBAR_HEIGHT            24//18
#define SHORTCUTBAR_HEIGHT          28
#define CAPTION_HEIGHT              31
#define TITLEBAR_HEIGHT             15
#define DESK2_STATUSBAR_HEIGHT      18

/* define grouping settings */
#define GRP_ICON_WIDTH				93
#define GRP_ICON_HEIGHT				93

/* define app settings */
#define APP_ICON_WIDTH				25
#define APP_ICON_HEIGHT				25


#define DAT_TIM_UDT     10000

/*
**	program comunication message module macro define
*/
#define		APP_HWND_REVMSG 8		//	permit receive windows number
#define		APP_MSG_KIND	8		//	must be 2 index: 2 4 8 16

#define  MAX_IDLEMODETETSIZE 240

extern BOOL GetSIMInfoAct(void);
extern BOOL GetMailNumAct(void);

//*********************************************************************
// Progman module function prototypes


DWORD WINAPI SHGetFileInfo(LPCTSTR pszPath, DWORD dwFileAttributes, 
						   SHFILEINFO *psfi, UINT cbFileInfo, UINT uFlags);

extern long FS_GetPrivateProfileInt
				( char *lpAppName,  /* address of section name */
				char *lpKeyName,  /* address of key name */
 				unsigned long nDefault,   		/* return value if key name is not found */
 				char *lpFileName  /* address of initialization filename */
);
extern long FS_GetPrivateProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpDefault,  /* points to default string */
						char *lpReturnedString,  /* points to destination buffer */
						unsigned long nSize,				/* size of destination buffer */
						char *lpFileName  /* points to initialization filename */
);
extern long FS_WritePrivateProfileString( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						char *lpString,		/* pointer to string to add */
						char *lpFileName  /* points to initialization filename */
);
extern long FS_WritePrivateProfileInt( 
						char *lpAppName,  /* points to section name */
						char *lpKeyName,  /* points to key name */
						long	 nValue,			/* pointer to string to add */
						char *lpFileName  /* points to initialization filename */
);

/* Create keyboard */
////////////////extern  HWND	Ime_Create(void);

/* Pen calibration window proc function, implemented in penwnd.c */
extern BOOL WINAPI RegisterDesktopWindow(HWND hWnd);
extern BOOL    AppDirGetRegIme(PAPPADM pAdm);

extern  PAPPADM	pAppMain;

//mux handle
//int Mux_UartHandle;
 
//define initial me message
#define WM_ME_COMINIT				(WM_USER+501)
#define WM_ME_TIME					(WM_USER+502)
#define WM_ME_COMINITEX             (WM_USER+503)

#define WM_PROG_SINGEL              (WM_USER+510)
#define WM_PROG_BATTERY             (WM_USER+511)

#define WM_PROG_WHICHPASS           (WM_USER+512)
#define WM_PROG_COPS                (WM_USER+513)
#define WM_PROG_COPSINFO            (WM_USER+514)
#define WM_PROG_GPRS                (WM_USER+515)
#define WM_PORG_CELLID              (WM_USER+516)
#define WM_PORG_COPSINFONUM         (WM_USER+517)
#define WM_PROG_VOICEMAIL1          (WM_USER+518)
#define WM_PROG_VOICEMAIL2          (WM_USER+519)
#define WM_PROG_CIPHERING           (WM_USER+520)
#define WM_PROG_VOLTAGELOW          (WM_USER+521)
#define WM_PROG_BATTEMPOUTRANGE     (WM_USER+522)
#define WM_PROG_ALARM               (WM_USER+523)
#define WM_CALLDIVERT               (WM_USER+524)
#define WM_ALSLINE					(WM_USER+525)
#define WM_SIMREAD					(WM_USER+526)
#define WM_READFILE					(WM_USER+527)
#define WM_READLAU                  (WM_USER+528)
#define WM_READLAN					(WM_USER+529)
//#define WM_PROG_FACDEFSET           (WM_USER+522)

#define WM_PIN_REMAIN               (WM_USER+530)
#define WM_PIN_QUERY                (WM_USER+531)
#define WM_PIN_CONFIRM              (WM_USER+532)

#define WM_GPRSSTATUS               (WM_USER+533)
#define WM_SETLAN					(WM_USER+534)
#define WM_READGIL1					(WM_USER+535)

#define WM_MESLEEPMODE              (WM_USER+536)

#define WM_PROG_RETURNIDLE          (WM_USER+602)
#define WM_CHANGE_GROUP             (WM_USER+603)
#define WM_PROG_CHGTIME             (WM_USER+604)

#define SIMLOCK_FAIL				(WM_USER+605)
#define REQUEST_SIMLOCKCODE         (WM_USER+606)
#define WM_SIMLOCKBLOCK             (WM_USER+607)

#define WM_IDLEOPRCHANGED           (WM_USER+608)
#define WM_ME_RATE                  (WM_USER+609)
#define WM_ME_SET_TIME              (WM_USER+610)

#define WM_GPSFRASH                  (WM_USER+611)

#define WM_HOMEZONE					(WM_USER + 612)
#define WM_PROG_SINGELCHN           (WM_USER + 613)

#define WM_AUDIO_HEADSET			(WM_USER + 614)
#define WM_AUDIO_HANDSET			(WM_USER + 615)   

#define CALLBACK_MESSAGE			(WM_USER + 616)
#define WM_MAILBOXNUM				(WM_USER + 617)

#define WM_HEADSETVOLUM             (WM_USER + 618)
#define WM_GPRSAVAIL				(WM_USER + 619)

#define WM_ME_BATTERYCHARGECONTROL	(WM_USER + 620)
#define WM_PORG_STK					(WM_USER + 621)
#define WM_PROG_SBC					(WM_USER + 622)
#define WM_PROG_MISSCHANGE          (WM_USER + 623)

#define APP_ALARM		100


//#define     IMAGE_TOOLBAR_NAME          "/rom/progman/b_ind_bk.bmp"         
//#define     FOCUSE_IMAGE_FILE           "/rom/progman/p_msgwin_bar.gif"	
//#define     BG_APP_LIST			        "/rom/progman/timer_bg.gif"         

//ME URC handle
//HANDLE   hProgUrcClip;
//HANDLE   hProgUrcCmti;

//idle struct
typedef struct
{
    RECT rBg;
    RECT rMNO;
	RECT rSPN;
	RECT rSPNMNO;
	RECT rCellId;
	RECT rIdleTxt;
    int  count;                             //idle count
    int  Idle_limit;                        //idle count limit£¬every 5 seconds scroll idle mode text
    int  bHideText;                         //hide text
    char cPlmnStr[20];						// roaming/ insert SIM card
	                                        //change SIM card/Emergency calls only
										    //Searching for networks/No service
	char cDeskStr[20];
	char cOperatorLName[20];                 //long version of operator name
	char cOpenratorSName[10];					//short version of operator name
	char cCountryCode[6];                    //country code
	char cOperatorCode[6];                   //operator code
	char cSPN[20];                          //SPN(Service provider name)                   
	char strCellId[75];                     //cell ID
	char cIdleModeTxt[MAX_IDLEMODETETSIZE+1];    //idle mode text
    char cBgImage[PREBROW_MAXFULLNAMELEN+1];
 
}PROGDESKIDLE, *PPROGDESKIDLE;

void PROG_InitDeskIdle(PROGDESKIDLE  *pDeskIdle);

// enum corporation information or sim card status
enum pm_cops{
 	pm_cops_link,
    pm_cops_searching, // searching cops
	pm_cops_rejectsim, //reject the sim card
	pm_cops_roaming, //roaming
	pm_cops_noserivce, //network is lost
	pm_cops_emergencycallonly, // emergency call only
    pm_no_sim,        // no sim card
    pm_sim_destroy,   // sim card is destroied
	pm_cops_gsmoff, //GSM radio off
    pm_no_me          // no me module
}pm_sim_cop_status;

/* MSG get Status*/
//#define ST_GETSMS     7
//#define PS_SETLXR			60

//#define MIN_ICONID    1
#define ICONID_SGN    21
#define ICONID_GPRS   22
#define ICONID_BAT    23

#define ICONID_CALLDIVERT 0
#define ICONID_ROAMING    1
#define ICONID_CIPHERING  2
#define ICONID_HOMEZONE   3
#define ICONID_ACTIVELINE 4
#define ICONID_BLUETOOTH  5
#define ICONID_HEADSET    6
#define ICONID_CARKIT     7
#define ICONID_MISSEDCALLS 8
#define ICONID_MSG         9
#define ICONID_VOICEMAIL		10
#define ICONID_SOUNDPROFILE		11
#define ICONID_KEYLOCK			12
#define ICONID_ALARMCLK         13
#define ICONID_POWER            14
#define ICONID_USB              15
#define ICONID_SECURECON        16

#define MAX_ICONID    20

#define CALLBACK_CANCEL_OFF   WM_USER + 200
#define CALLBACK_POWEROFF     WM_USER + 201

//#define   NIM_ADD      1
//#define   NIM_DELETE   2
//#define   NIM_MODIFY   3

typedef struct _NOTIFYICONDATA { 
    DWORD dwSize; 
    HWND  hWnd;
	UINT  udcNo;
    UINT  uID; 
    UINT  uFlags; 
    UINT  uCallbackMessage;	
	char  IconName[64];
} NOTIFYICONDATA, *PNOTIFYICONDATA; 

enum keytype{
	PM_LONGKEY = 0x01,
	PM_SHORTKEY = 0x02,
	PM_INSTANTKEY =0x04,
	PM_REPETEDKEY = 0x08,
	PM_MULTITAPKEY =0x10
};
#define ET_REPEAT_FIRST  300
#define ET_REPEAT_LATER  100
#define ET_MULTITAP      400
#define ET_LONG          600

typedef struct tagKeyEventData{
	enum keytype  nType;
	DWORD   dKicktime;
	unsigned int nTimerId;     
    long    nkeyCode;
	struct tagKeyEventData *pNext;
}KeyEventData, *PKEYEVENTDATA;
 
void  DrawNotifyIcon(WPARAM wParam, LPARAM lParam);

void Prog_InitHook(void);
BOOL RegisterElectClass(void);
void CreateElectWindow(BOOL bPower, BOOL bTip);
void Elect_PowerOn(BOOL bTip);
void Elect_PowerOff(void);
BOOL PlayPowerOn(BOOL);
BOOL Desk_ShowGrp( struct appMain * pAdm);
int  GetSIMState(void);
void SetSimState(int i);
void SetStyle(int id);
void ChangeAppStyle(int id);
void ChangeGroupStyle(int id);
int  GetDirChildNum(struct appMain * pAdm, short dirId);
BOOL AppFileMoveEx( struct appMain * pAdm, short fileID ,short  srcIndex,short destIndex);

#endif //_PROGMAN_H_

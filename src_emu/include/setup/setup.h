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
#ifndef _SETUP_H
#define _SETUP_H

#include "pubapp.h"
#include "string.h"
#include "setting.h"
#include "me_wnd.h"
#include    "plxdebug.h"
#include "compatable.h"
//#define _SETTING_DEBUG_
//#define _CPHS_DEBUG_

#define SETUPFILENAME       "/mnt/flash/setup/Setting.ini"
#define SETDIALFILE			"/mnt/flash/setup/SetDialID.inf"

//保存各个情景模式信息的文件
#define SETUP_SCENEMODEFN   "/mnt/flash/setup/smseting.ini" 

//保存GPS情景模式信息的文件
#define SETUP_GPSMODE       "/mnt/flash/setup/GPSmodesetting.ini" 
//*************Radio button for single selection***************************
#define SETT_SHOW_TITLE_ICON 0
#define SETT_SIGSEL_TIMEOUT 500 //duration time after press ok and before window closed of single selection list;
BOOL Load_Icon_SetupList(HWND hSetupList,HBITMAP hIconNormal,HBITMAP hIconSel,int iItemNum, int iSelIndex);

#define		SETT_PRINT				/*printf*/
#define		ZC_PLXTipsWin    		PLXTipsWin

#define     ICONORMAL_WIDTH          17
#define     ICONORMAL_HEIGHT         16
#define     ICONORMAL               "ROM:network/radio_button_off.bmp"    
#define     ICONSEL                 "ROM:network/radio_button_on.bmp"      
#define     ICON_CHECK_BTN          "ROM:setup/language/RB_Checked.bmp"     
#define     ICON_NOCHECK_BTN        "ROM:setup/language/RB_NotChecked.bmp"  

#define     ICONPHONE               "ROM:setup/Sett_Phone_22x16.bmp"
#define     ICONCALL                "ROM:setup/Sett_Call_22x16.bmp"
#define     ICONCONNECT             "ROM:setup/Sett_Connectivity_22x16.bmp"
#define     ICONPROFILE             "ROM:setup/Sett_SoundProfile_22x16.bmp"
#define     ICONSECURE              "ROM:setup/Sett_Security_22x16.bmp"
#define     ICONPOSIT               "ROM:setup/Sett_GPS_22x16.bmp"
#define     ICONACCESS              "ROM:setup/Sett_Accessories_22x16.bmp"
#define     ICONRESTORE             "ROM:setup/Sett_RestoreDefaults_22x16.bmp"

#define     ICON_PROFILE_NEW        "ROM:setup/Sett_NewSoundProfile_22x16.bmp"
#define     ICON_PROFILE_SILENT     "ROM:setup/Idle_D_silentprofile_15x15.bmp"
#define     ICON_PROFILE_OTHERS     "ROM:setup/Idle_D_profile_15x15.bmp"
#define     ICON_PROFILE_ACTIVE     "ROM:setup/Sett_Active_20x16.bmp"

#define     ICON_GPS_NEW            "ROM:setup/Sett_NewGPSProfile_22x16.bmp"
#define     ICON_GPS_OFF            "ROM:setup/gps/gps_off_ex.bmp"
#define     ICON_GPS_PRESET_ONE     "ROM:setup/gps/gps_profile_6_ex.bmp"
#define     ICON_GPS_PRESET_TWO     "ROM:setup/gps/gps_profile_5_ex.bmp"

#define     ICON_CONNECTION_NEW     "ROM:setup/Sett_NewConnection_22x16.bmp"
#define     ICON_CONNECTION_GPRS    "ROM:setup/Sett_GPRSConnection_22x16.bmp"
#define     ICON_CONNECTION_CSD     "ROM:setup/Sett_CSDConnection_22x16.bmp"

#define     SETT_BKGRD_PIC          "ROM:public/background_areas.bmp"
#define     SETT_BKGRD_PIC_1        "ROM:public/background_1.bmp"
#define     SETT_BKGRD_PIC_2        "ROM:public/background_2.bmp"
#define     SETT_BKGRD_PIC_3        "ROM:public/background_3.bmp"


#define		SETT_LEFT_ICON			"ROM:public/arrow_left.ico"
#define		SETT_RIGHT_ICON			"ROM:public/arrow_right.ico"
//******************************************************************
long    FS_WritePrivateProfileString(char * ,char * ,char * ,char * );
long    FS_GetPrivateProfileString(char *, char *,char *,char *,unsigned long , char *);                                                                                                         
long    FS_WritePrivateProfileInt(char *,char *, long ,char * );
long    FS_GetPrivateProfileInt( char *,char *,unsigned long ,char *);
//******************************************************************
int Sett_SpecialStrProc(char *str); //eliminate the space at the end and head of string;

//******************************************************************
BOOL    GetSecurity_Open(void);
void    GetSecurity_code(char * cpassword);
BOOL    GetLockCodePeriod(char *pLockPeriodTime);//Get autolock prieod

void Sett_InitSettInfo(void); //init the setting info when power on

void Sett_SaveSettInfo(void); //save the setting info when power off

#define ET_REPEAT_FIRST 300
#define ET_REPEAT_LATER 100
    

typedef enum
{
    SWITCH_ON = 0,
    SWITCH_OFF = 1
}SWITCHTYPE;

typedef enum
{
    LAS_VOL,
    LAS_LIT,
	LAS_RING,
	LAS_KEY,
	LAS_NOTIFY,
	LAS_ICON_SHOW
}ADJTYPE;
//***********************************语言设置所需部分*****************************************
#define     SN_WRILANGUAGE          "Writinglangage"
#define     KN_WRILANGUAGEID        "id"
#define     KN_PREINPUTID           "PIid"
#define     SN_PHONELANG            "PhoneLanguage"
#define     KN_AUTOLANG             "automaticlanguage"

int GetWriLanguage(void);
BOOL GetAutoLangState(void);
TCHAR* GetWriLangStr(void);

SWITCHTYPE GetPreInputSetting(void);
BOOL GetAutoLangName(char * AutoLangName);
void SetAutoLanguage (SWITCHTYPE AutoLang);

//***********************************情景模式设置所需部分*****************************************
#define     FIXEDPROFILENUMBER   4
#define     MODENAMEMAXLEN      20    
#define     GROUPNAMEMAXLEN     30
#define     GPSMODENAMEMAXLEN   16    
#define     GPSICONNAMEMAXLEN   200   

#define     SN_SCENEMODE            "scenemode"             //情景模式的结点
#define     KN_SCENEMODENUM         "scenemodenum"          //用户选择的情景模式的关键字

typedef enum 
{
	NORMAL       = 0,    //the sound type of ring is 'ring'
	ASCENDING    = 1,    //the sound type of ring is 'ring ascending'    
	BEEPFIRST    = 2,    //the sound type of ring is 'beep once'
}RINGTYPE;

typedef struct 
{
	char        cMusicFileName[RINGNAMEMAXLEN+1];  //the name of the music file(including the path)
	int         iRingVolume;                     //the volume of the ring
	RINGTYPE    iRingType;                   
}SCENEMODERING;

typedef struct 
{
	int         iTimeInterval;    // 0: off, 1: 30 sec; 2: 1 min; 3: 2 min; 4: 5 min; 5 10 min;
	int         iEventVolume;
    SWITCHTYPE  iEventVibra;
}MISSEDEVENTS;

typedef enum 
{
	ALERTALL    = 0,
	ALERTSELECT = 1,
	ALERTNO     = 2
}ALERTGROUPS;

#define  SETT_MAX_ABGRP_NUM   50
#define  SOUND_PROFILE_ACT    1
#define  SOUND_PROFILE_INACT  0
typedef struct 
{
	char           cModeName[MODENAMEMAXLEN+1];  //the name of the scenemode
	SCENEMODERING  rIncomeCall;                    //setting for the ring of incoming call
	SCENEMODERING  rMessage;                       //setting for the ring of message(sound type is not available)
	SCENEMODERING  rTelematics;                    //setting for the ring of telematics message(sound type is not available)
    SCENEMODERING  rAlarmClk;                      //setting for the ring of alarm clock
	SCENEMODERING  rCalendar;                      //setting for the ring of calendar alarm
	int            iNotification;                  //setting for the volume of notification 
    MISSEDEVENTS   mEvents;
	int            iKeyVolume;                     //the volume of the key click
	SWITCHTYPE     iVibraSwitch;
	ALERTGROUPS    iAlertFor;                      //Alert for all, selected groups, or None
	int            iAlertGrpID[SETT_MAX_ABGRP_NUM]; //The selected group's ID
    int            iActiveFlag;                     //SOUND_PROFILE_ACT, SOUND_PROFILE_INACT
	int            iHeadSetFlag;
	int			   iCarkitFlag;
}SCENEMODE; 


BOOL    GetSM(SCENEMODE * scenemode, int iscenemode);//读取情景模式的各个参数值
BOOL    SetSM(const SCENEMODE * scenemode,int iscenemode);//设置情景模式的各个参数值
BOOL    DelSM(int iCurMode);
BOOL    AddSM(SCENEMODE sm);
int     GetUserProfileNo(void);//取得用户设定的情景模式数量
BOOL    SetUserProfileNo(int iIndex);
int     GetCurSceneMode(void);//获得当前情景模式的编号
void    SetCurSceneMode(int iscenemode);//把编号为iscenemode的情景模式设为激活的模式

BOOL    Sett_GetActiveSM(SCENEMODE* scenemode);

typedef enum 
{
	INCOMCALL = 0,
    MESSAGES  = 1,
	TMESSAGES = 2,
	ALARMCLK  = 3,
	CALENDAR  = 4
}RINGFOR;
//************************************Color theme setting**************************************************
#define  MAX_CLOR_THEME_NAME  20

typedef struct 
{
    char colorThemeName[MAX_CLOR_THEME_NAME + 1];
    unsigned long colorThemeID;
}COLOR_THEME_INFO;



//************************************Color theme setting**************************************************

//************************************GPS模式设置**************************************************

#define     PRESETGPSPROFILENUM   2

#define     SN_GPSSETTING           "GPSsetting"
#define     KN_GPSMODENUM           "GPSmodenumber"
#define     KN_ACTGPSINDEX          "GPSmodeIndex"
#define     KN_GPSMODEID            "GPSmodeID"

typedef enum
{
    GPSRI_OFF,
    GPSRI_ONESEC,
    GPSRI_THREESEC,
    GPSRI_TENSEC,
    GPSRI_THIRTYMIN
}GPSREFINT;

typedef enum
{
    GPSRIC_CON,
    GPSRIC_KIPINT,
    GPSRIC_OFF
}GPSREFINTCHARG;

typedef enum
{
    GPS_ACCURACY,
    GPS_SENSITI
}GPSOPTIMISE;

typedef enum
{
    GPSNMEA_OFF,
    GPSNMEA_FOUREIGHTOO,
    GPSNMEA_NINESIXOO
}GPSNMEAOP;

typedef struct 
{
    char            cGPSModeName[GPSMODENAMEMAXLEN + 1];
    char            cGPSIconName[GPSICONNAMEMAXLEN + 1];
    GPSREFINT       iRefIntervl;
    GPSREFINTCHARG  iRefMode;    
    GPSOPTIMISE     iOptMode;
    GPSNMEAOP       iNMEAoutput; 
    unsigned long   nGPSID;
	HBITMAP			hGPSIcon;
	int				ActiFlag;
}GPSMODE;

BOOL GetGPSprofile(GPSMODE *gm, int iGPSModeIndex);
int     GetGPSprofileNum(void);

void SetActiveGPSProfile(int iProfileIndex);
int GetActiveGPSProfile(void);
void SetOffGPSProfile(void);


//************************************锁键盘设置**************************************************
#define     SN_PHONESETTING         "phonesetting"
#define     KN_KEYLOCKMODE          "Keylockmode"

typedef enum
{
    KL_MANUAL,
    KL_AUTO
}KEYLOCKMODE;
KEYLOCKMODE GetKeyLockMode(void);

//************************************锁键盘设置**************************************************
//************************************Info line设置**************************************************
#define     KN_IDLESHOWINFO         "IdleShownInfo"


typedef enum
{
    IS_CELLID,
    IS_DATE,
    IS_NONE
}IDLESHOW;
IDLESHOW GetIdleShowInfo(void);
//************************************Info line设置**************************************************

//************************************Accessory setting*************************************************
#define SN_ACCESSORY "AccessorySettingInfo"

#define KN_HEADSETPROFILE "Accessory_HeadsetProfile"
#define KN_HEADSETAUTOANS "Accessory_HeadsetAutoAns"
#define KN_CARKITPROFILE  "Accessory_CarkitProfile"
#define KN_CARKITAUTOANS  "Accessory_CarkitAutoAns"
#define KN_SPEAKERAUTOANS "Accessory_SpeakerAutoAns"


typedef enum
{
    ACC_HEADSET,
    ACC_CARKIT,
    ACC_SPEAKER
}ACCESSORY_TYPE;


typedef struct 
{
    int use_profile; //keep profile: -1, Normal: 0, Silent: 1 ...
    SWITCHTYPE auto_ans;   // On, Off

}ACCESSORY_SETUP;

void GetAccessorySettingInfo(ACCESSORY_SETUP *AccInfo, ACCESSORY_TYPE AccType);

//************************************Accessory setting**************************************************

//************************************快捷键设置所需部分**************************************************

#define SN_SHORTCUT    "shortcutname"

#define KN_LEFTSOFTID  "leftsoftkey"
#define KN_LSFID       "LeftsoftKeyFeatureID"

#define KN_RIGHTSOFTID  "rightsoftkey"
#define KN_RSFID		"RightSoftKeyFeatureID"

#define KN_LEFTARROWID  "leftarrowkey"
#define KN_LAFID		"LeftArrowKeyFeatureID"

#define KN_RIGHTARROWID  "rightarrowkey"
#define KN_RAFID		 "RightArrowKeyFeatureID"

typedef enum
{
    SCK_LAK, //left arrow key
    SCK_LSK, //left soft key
    SCK_RAK, //right arrow key
    SCK_RSK  //right soft key
}SHORTCUTKEY;
const TCHAR * GetShortCutAppName(SHORTCUTKEY keyName, int* fID); //return the application name string of the key

//************************************快捷键设置所需部分**************************************************

//************************************ Display setting related **************************************************
#define     SN_DISPLAY              "SN_DISPLAY"            //display setting related info
#define     KN_BGLIGHT              "KN_bglight"            //背光灯信息关键字名
#define     KN_SCREEN               "KN_bglight_screen"     //屏幕持续显示时间
#define     KN_BRIGHT               "KN_bright"             //背景灯亮度
#define     KN_SYSWAIT              "KN_syswait"            //系统待机时间
#define     KN_SCRNSAVER            "KN_scrnsaver_wait"     //screen saver time
#define     KN_BGLIGHTMODE          "KN_backlight_mode"     //Display backlight mode
#define     KN_KPLIGHTMODE          "KN_keypad_light_mode"     //Display backlight mode
#define     KN_STARTTXT             "KN_Start_Text"         //Greeting text
#define     KN_START_EFFECT         "KN_Start_Effect"         //start up effect
#define     KN_BKPICNAME            "KN_BackgrdPicName"         //start up effect
#define     KN_STARTPICNAME         "KN_StartupPicName"         //start up effect
#define     KN_IND_LED				"KN_IndLedState"
void InitDisplayColor(void);

SWITCHTYPE GetIndicatorLEDStat(void);
void SetIndicatorLEDStat(SWITCHTYPE ind_LED);

// get the value for screen saver time
int GetScrnSaverTime(void); //return value: 0  --   Off
                            //              1  --   1 min
                            //              2  --   2 min
                            //              3  --   5 min
                            //              4  --  10 min
                            //              5  --  15 min
                            //              6  --  30 min
LEVELVALUE GetBrightness(void);

// the setting of light(display backlight & keypad light)
typedef enum
{
    LS_AUTO,
    LS_KEEP_ON,
    LS_KEEP_OFF
}LIGHTMODE;

LIGHTMODE GetDisplayBackLitMode(void); //return value: LS_AUTO;  LS_KEEP_ON

LIGHTMODE GetKeypadLitMode(void); //return value: LS_AUTO; LS_KEEP_ON; LS_KEEP_OFF

BOOL GetBKPicture(char * szPicFile);

#define MAXSTARTUPTXTLEN   40

void GetGreetingTxt(char * szStartupTxt);
BOOL GetStartupPic(char * szPicFile);
typedef enum
{
    SE_DEF, //start up effects is default
    SE_TXT, //start up effects is text
    SE_PIC  //start up effects is picture
}STARTUP_EFFECT;

STARTUP_EFFECT GetStartupEffect(void);

//************************************ Display setting related **************************************************



//************************************ connectivity setting related **************************************************

//******************GSM radio setting******************//
SWITCHTYPE GetGSMRadioPartMode(void);
void SetGSMRadioPartMode(SWITCHTYPE OnOrOff);

//******************GSM radio setting******************//




//************************************ connectivity setting related **************************************************






//************************************自动关机所需部分**************************************************
/*
typedef enum
{
    AC_CLOSE = 0,//不启动
    AC_START = 1 //启动
}AC_STATE;                //自动关机

//Jesson 7.18
typedef enum
{
    AO_CLOSE = 0,
    AO_START = 1
}AO_STATE;              //自动开机
*/

/*
typedef enum
{
    AC_ONLY     = 0,//仅一次
    AC_EVERYDAY = 1 //每日
}AC_TYPE;
*/

typedef struct  //自动关机中保存关机时间，状态，频率的结构
{
    WORD         CloseHour;       //自动关机的小时(24小时制)
    WORD         CloseMin;        //自动关机的分钟
    WORD         OpenHour;        //自动开机的小时(24小时制)  
    WORD         OpenMin;         //自动开机的分钟
    SWITCHTYPE  CloseState;      //自动关机是否打开
    SWITCHTYPE  OpenState;       //自动开机是否打开
}AUTO_CLOSE;


BOOL    GetAutoOSOption(AUTO_CLOSE * os);//读取自动开关机的数据


//*********************************声音播放部分********************
int     SetupHf_PlayMusicInit(SetupHS_PlayMusic * playmusic);//播放音乐初始化
int     SetupHf_PlayAMR(void);          //播放AMR文件
int     SetupHf_PlayWAVE(void);         //播放WAVE文件
int     SetupHf_PlayMIDI(void);         //播放midi文件
BOOL    SetupHF_EndPlayMusic(void);     //停止播放音乐
int     SetupHf_BreakPlayMusic(void);   //播放声音的时候收到中止消息后需要执行的函数
////////////////////////设置部分包的硬件函数//////////////////////////////////////////////////
void    SetupHf_LcdBklight(BOOL bEn);   //开关LCD
void    SetupHf_BgLight(int iBgLight);  //设置亮度
void    SetupHf_Vibration(BOOL bEn);    //开关震动
BOOL    GetEarphone(void);//查询耳机状态
//*********************************其它函数*****************************************
BOOL    FindSIMState(BOOL bShow);
//获得SIM卡的状态
void    LoadHbitmaps_Gif(HWND hWnd,HBITMAP * hbitmap,char ** p);
//加载**P中的位图
void    List_AddString(HWND hList,char ** p,HBITMAP * hIconNormal,BOOL bAppend);
//将相应的文字添加到列表框中
BOOL    AddString(HWND hWnd,char ** p);
//将指针数组**p中的文字添加到控件中
BOOL    CallheadphoneWindow(HWND hFrame, int iHeadphone);

BOOL    CallUnlockPinWindow(void);//pin1码解锁

BOOL    CallCartoon_PreWindow(HWND hwndCall,const char * cfilename);//动画预览

void    BeginVibration(unsigned int iSecond);//实现振动iSecond秒

/*******************************************************/
#define     MAX_CPHS_ENTRY_NAME 40
#define     CPHS_INFO_NUM_LENGTH 20
typedef struct 
{
    UINT iIndexLev;
    char szEntryName[MAX_CPHS_ENTRY_NAME + 1];
    int  nEntryNo;
    BOOL ChargeSign;
    BOOL NetSpeciSign;
	int  iListIndex;	//the index number of item in the UI list
}CPHS_INFONUM_ENTRY;
/**********************************************************/

void GetSimStatus_FDN(SimService *PSimSer);
void GetSimStatus_AOC(SimService *PSimSer);
void GetSimStatus_CHV1(SimService *PSimSer);

/**********************************************************/
BOOL    Sett_FileIfExist(const char * pFileName);
/**********************************************************/
/**********************************************************/

int ZC_Open(const char* pathname, int flags, mode_t mode);

int ZC_Close(int fd);

int Sett_GetFileHandleNum(void);


/**********************************************************/
#endif

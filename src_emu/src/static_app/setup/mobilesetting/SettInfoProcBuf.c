/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setup
 *
 * Purpose  : process setting info in the memory, saving info to flash when 
 *            power off.
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "str_plx.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	"unistd.h" 
#include    "setting.h"
#include    "setup.h"
#include    "mullang.h"
#include    "ioctl.h"
#include	"hopen/ipmc.h"
#include	"pmalarm.h"
#include	"compatable.h"
/********************Global variable**********************/

static int iScrSaverTime = 0;
static int iIdleShowInfo = 0;
static int dateFormat = 0;
static int timeFormat = 1;
static int timeZone = 0;
static int iWriLangIndex = 0;

static TCHAR**     LangName = NULL;
static int nLang = 0;


static LIGHTMODE DisLitMode = 0;
static LIGHTMODE KeyPadMode = 0;
static char szShortcut[PMNODE_NAMELEN];

static ACCESSORY_SETUP HSAccInfo; //global headset accessory setting info;
static ACCESSORY_SETUP CKAccInfo; //global car kit accessory setting info;
static ACCESSORY_SETUP SPAccInfo; //global speaker phone accessory setting info;
/********************Global variable**********************/
void Sett_RestoreRamTimeSrc(void);
void Sett_RestoreRamOCState(void);
void GetNewZoneTime(SYSTEMTIME *st_old,const char *szNewZone,const char *szOldZone);
char* GetHomeZoneStr(void);
char* GetVisitZoneStr(void);
void SetShortCutAppName(char* szAppName, SHORTCUTKEY keyName);
BOOL DHI_SetKeylight(BOOL bOpen);
void InitAutoOCState(void);
int GetAppIndexByName(char* appName);
void DHI_SetLEDLight(BOOL bEnable);
BOOL APP_ShowCellInfo(BOOL bShow);
void	Sett_ProcessLangChange(void);
BOOL ChangeAutoAnswer(BOOL bAuto);
void SYS_SetCurSceneMode();
void SYS_GetAccessorySettingInfo(void);
void SYS_SetActiveGPSProfile(void);
void GetShortcutDisplayName(char *szName, int fID);

/********************internal function**********************/
static void SYS_Sett_GetUnit(void);
static void SYS_Sett_SetUnit(void);
static void	SYS_GetIdleShowInfo(void);
static void SYS_SetIdleShowInfo(void);
static void SYS_GetScrnSaverTime(void);
static void SYS_GetDateFormt(void);
static void SYS_GetTimeFormt(void);
static void SYS_GetKeyLockMode(void);
static void SYS_SetShortCutAppName(void);
static void SYS_GetShortCutAppName(void);
static void SYS_GetIndicatorLEDStat(void);
static void SYS_GetUsingTimeZone(void);
static void SYS_GetDisplayBackLitMode(void);
static void SYS_SetDisplayBackLitMode(void);
static void SYS_SetKeypadLitMode(void);
static void SYS_GetKeypadLitMode(void);
static void SYS_GetWriLanguage(void);
static BOOL SYS_SetWriLanguage(void);
static void SYS_SaveAutoAnsState(void);
static void SYS_SaveAccessoryProfileInfo(void);


static void GetAppNameForPM(char* szName, int* fID);

/********************internal function**********************/


void Sett_InitSettInfo(void)
{
	SYS_GetDateFormt();
	SYS_GetTimeFormt();
	SYS_GetKeyLockMode();
	SYS_GetShortCutAppName();
	SYS_Sett_GetUnit();
	SYS_GetIdleShowInfo();
	SYS_GetScrnSaverTime();
	SYS_GetIndicatorLEDStat();
	SYS_GetUsingTimeZone();
	SYS_GetDisplayBackLitMode();
	SYS_GetKeypadLitMode();
	InitAutoOCState();
	SYS_GetWriLanguage();
}


void Sett_SaveSettInfo(void)
{
	SYS_SetShortCutAppName();
	SYS_Sett_SetUnit();
	SYS_SetIdleShowInfo();
	SYS_SetDisplayBackLitMode();
	SYS_SetKeypadLitMode();
	SYS_SetWriLanguage();
	SYS_SaveAutoAnsState();
	SYS_SetCurSceneMode();
	SYS_SaveAccessoryProfileInfo();
	SYS_SetActiveGPSProfile();
}

static void SYS_GetScrnSaverTime(void)
{
//	iScrSaverTime = FS_GetPrivateProfileInt(SN_DISPLAY,KN_SCRNSAVER,2,SETUPFILENAME);
}
/***************************************************************
* Function  GetScrnSaverTime
* Purpose   get the screen saver time
* Params
* Return
* Remarks
***************************************************************/

int GetScrnSaverTime(void)
{
    return  iScrSaverTime;//default 2 min
}
/***************************************************************
* Function  SetScrnSaverTime
* Purpose
* Params
* Return
* Remarks
***************************************************************/
void SetScrnSaverTime(int iTime)
{    
//	if (iTime != iScrSaverTime)
//    {
//		iScrSaverTime = iTime;
//		FS_WritePrivateProfileInt(SN_DISPLAY,KN_SCRNSAVER,iTime,SETUPFILENAME);
//		DlmNotify(PMM_NEWS_ENABLE_SCRSAVE, TRUE);
//	}
}

/***************************************************************
* Function  SetKeyLockMode
* Purpose   set the key lock mode    
* Params
* Return    
* Remarks
***************************************************************/
static int keyLockMode = 0;


BOOL SetKeyLockMode(KEYLOCKMODE iKeylockmode)
{
	if (keyLockMode == iKeylockmode)
		return TRUE;

	keyLockMode = iKeylockmode;
    FS_WritePrivateProfileInt(SN_PHONESETTING,KN_KEYLOCKMODE,iKeylockmode,SETUPFILENAME);
    return TRUE;
}
/***************************************************************
* Function  GetKeyLockMode
* Purpose   set the key lock mode    
* Params
* Return    
* Remarks
***************************************************************/
KEYLOCKMODE GetKeyLockMode(void)
{
    return keyLockMode;
}

static void SYS_GetKeyLockMode(void)
{
	keyLockMode = FS_GetPrivateProfileInt(SN_PHONESETTING,KN_KEYLOCKMODE,0,SETUPFILENAME);
}

/***************************************************************
* Function  GetIdleShowInfo
* Purpose   get the info shown in Idle    
* Params
* Return    IS_CELLID: show cell id; 
            IS_DATE:   show date;
            IS_NONE:   show nothing
* Remarks
***************************************************************/
IDLESHOW GetIdleShowInfo(void)
{
    return iIdleShowInfo;
}

static void SYS_GetIdleShowInfo(void)
{
	iIdleShowInfo = FS_GetPrivateProfileInt(SN_PHONESETTING,KN_IDLESHOWINFO,0,SETUPFILENAME);
}
/***************************************************************
* Function  SetIdleShowInfo
* Purpose   set the info shown in Idle    
* Params
* Return    
* Remarks
***************************************************************/
BOOL SetIdleShowInfo(IDLESHOW idleShow)
{
	if (idleShow == IS_CELLID && iIdleShowInfo != idleShow)
	{
		APP_ShowCellInfo(TRUE);
	}
	
	if (idleShow != IS_CELLID && iIdleShowInfo == IS_CELLID)
	{
		APP_ShowCellInfo(FALSE);
	}
	iIdleShowInfo = idleShow;
    return TRUE;
}
static void SYS_SetIdleShowInfo(void)
{
    FS_WritePrivateProfileInt(SN_PHONESETTING,KN_IDLESHOWINFO,iIdleShowInfo,SETUPFILENAME);
}

/***************************************************************
* Function  GetTimeFormt
* Purpose   get the time format    
* Params
* Return    TF_24: 24-hours mode;   TF_12:  12-hours mode
* Remarks
***************************************************************/

TIMEFORMAT     GetTimeFormt(void)
{
    return   timeFormat;
}

void SYS_GetTimeFormt(void)
{
	timeFormat = FS_GetPrivateProfileInt(SN_TIMEMODE, KN_TIMEMODE,TF_24,SETUPFILENAME); 
}
/***************************************************************
* Function  SetTimeFormt
* Purpose   set the time format    
* Params
* Return    
* Remarks
***************************************************************/

BOOL  SetTimeFormt(TIMEFORMAT tf_format)
{
	if (tf_format != timeFormat)
	{
		timeFormat = tf_format;
		FS_WritePrivateProfileInt(SN_TIMEMODE, KN_TIMEMODE,tf_format,SETUPFILENAME);
	}
    return TRUE;
}
/***************************************************************
* Function  GetDateFormt
* Purpose   get the date format    
* Params
* Return    DF_MDY: mm dd yyyy; DF_DMY: dd mm yyyy; DF_YMD: yyyy mm dd
* Remarks
***************************************************************/

DATEFORMAT      GetDateFormt(void)
{
    return dateFormat;
}

void SYS_GetDateFormt(void)
{
	dateFormat = FS_GetPrivateProfileInt(SN_TIMEMODE, KN_DATEMODE,0,SETUPFILENAME);
}
/***************************************************************
* Function  SetDateFormt
* Purpose   set the time format    
* Params
* Return    
* Remarks
***************************************************************/

BOOL   SetDateFormt(DATEFORMAT df_format)
{   
	if (df_format != dateFormat)
	{
		dateFormat = df_format;
		FS_WritePrivateProfileInt(SN_TIMEMODE, KN_DATEMODE,df_format,SETUPFILENAME);
	}
    return TRUE;  
}
/***************************************************************
* Function  unit setting info
* Purpose   
* Params
* Return    
* Remarks
***************************************************************/
static int g_DisUnit;
static int g_SpeedUnit;
static int g_EleUnit;

void SYS_Sett_GetUnit(void)
{
	g_DisUnit = FS_GetPrivateProfileInt(SN_UNIT, KN_UNIT_DISTANCE, DIS_METERS, SETUPFILENAME);
	g_SpeedUnit = FS_GetPrivateProfileInt(SN_UNIT, KN_UNIT_SPEED, KM_PER_HOUR, SETUPFILENAME);
	g_EleUnit = FS_GetPrivateProfileInt(SN_UNIT, KN_UNIT_ELEVATION, ELE_METERS, SETUPFILENAME);
}

void SYS_Sett_SetUnit(void)
{
    FS_WritePrivateProfileInt(SN_UNIT, KN_UNIT_DISTANCE, g_DisUnit, SETUPFILENAME);
    FS_WritePrivateProfileInt(SN_UNIT, KN_UNIT_SPEED, g_SpeedUnit, SETUPFILENAME);
    FS_WritePrivateProfileInt(SN_UNIT, KN_UNIT_ELEVATION, g_EleUnit, SETUPFILENAME);
}

void SetUnitDistance(UNIT_DISTANCE DisUnit)
{
	g_DisUnit = DisUnit;
}
void SetUnitSpeed(UNIT_SPEED SpeedUnit)
{
	g_SpeedUnit = SpeedUnit;
}
void SetUnitElevation(UNIT_ELEVATION EleUnit)
{
	g_EleUnit = EleUnit;
}

UNIT_DISTANCE GetUnitDistance(void)
{
   return g_DisUnit;
}

UNIT_SPEED GetUnitSpeed(void)
{
   return g_SpeedUnit;
}

UNIT_ELEVATION GetUnitElevation(void)
{
   return g_EleUnit;
}


/***************************************************************
* Function  shortcut setting info
* Purpose   
* Params
* Return    
* Remarks
***************************************************************/

static TCHAR szLSK_AppName[PMNODE_NAMELEN];
static int   LSKF_ID = 0;

static TCHAR szRSK_AppName[PMNODE_NAMELEN];
static int   RSKF_ID = 0;

static TCHAR szLAK_AppName[PMNODE_NAMELEN];
static int   LAKF_ID = 0;

static TCHAR szRAK_AppName[PMNODE_NAMELEN];
static int   RAKF_ID = 0;
const TCHAR* Sett_GetShortCutAppName(SHORTCUTKEY keyName, int* fID)
{
    if(keyName == SCK_LAK)        
	{
		*fID = LAKF_ID;
		return szLAK_AppName;
	}

    if(keyName == SCK_LSK)              
	{
		*fID = LSKF_ID;
        return szLSK_AppName;
	}

    if(keyName == SCK_RAK)
	{
		*fID = RAKF_ID;
        return szRAK_AppName;
	}

    if(keyName == SCK_RSK)
	{
		*fID = RSKF_ID;
        return szRSK_AppName;        
	}

    return (TCHAR*)FALSE;
}
const TCHAR* GetShortCutAppName(SHORTCUTKEY keyName, int* fID)//to program manager
{
	memset(szShortcut, 0, sizeof(szShortcut));
	strcpy(szShortcut, Sett_GetShortCutAppName(keyName, fID));
	GetAppNameForPM(szShortcut, fID);
	return szShortcut;
}
const TCHAR* Sett_GetAppIdleDis(SHORTCUTKEY keyName)
{
	int fID;
	memset(szShortcut, 0, sizeof(szShortcut));
	strcpy(szShortcut, Sett_GetShortCutAppName(keyName, &fID));
	GetShortcutDisplayName(szShortcut, fID);
	return szShortcut;
	
}
static void SYS_GetShortCutAppName(void)
{
	FS_GetPrivateProfileString(SN_SHORTCUT,KN_LEFTARROWID,"Contacts",
            szLAK_AppName,PMNODE_NAMELEN,SETUPFILENAME);
   
	FS_GetPrivateProfileString(SN_SHORTCUT,KN_LEFTSOFTID,"Messaging",
            szLSK_AppName,PMNODE_NAMELEN,SETUPFILENAME);
        
	FS_GetPrivateProfileString(SN_SHORTCUT,KN_RIGHTARROWID,"Calendar",
            szRAK_AppName,PMNODE_NAMELEN,SETUPFILENAME);
        
	FS_GetPrivateProfileString(SN_SHORTCUT,KN_RIGHTSOFTID, "Navigation",
            szRSK_AppName,PMNODE_NAMELEN,SETUPFILENAME);
 	
	LAKF_ID = FS_GetPrivateProfileInt(SN_SHORTCUT, KN_LAFID, 0, SETUPFILENAME);
	LSKF_ID = FS_GetPrivateProfileInt(SN_SHORTCUT, KN_LSFID, 3, SETUPFILENAME);
	RAKF_ID = FS_GetPrivateProfileInt(SN_SHORTCUT, KN_RAFID, 1, SETUPFILENAME);
	RSKF_ID = FS_GetPrivateProfileInt(SN_SHORTCUT, KN_RSFID, 2, SETUPFILENAME);
}

void ConfigShortCutKey(SHORTCUTKEY keyName, char* szAppName, int fID)
{
	SetShortCutAppName(szAppName, keyName);
	    
	switch(keyName)
    {
    case SCK_LAK:
		LAKF_ID = fID;
    	break;
    case SCK_LSK:
		LSKF_ID = fID;
    	break;
    case SCK_RAK:
		RAKF_ID = fID;
        break;
    case SCK_RSK:
		RSKF_ID = fID;
        break;
    }
}

void SetShortCutAppName(char* szAppName, SHORTCUTKEY keyName)
{
    switch(keyName)
    {
    case SCK_LAK:
        strncpy(szLAK_AppName, szAppName, sizeof(szLAK_AppName));
    	break;
    case SCK_LSK:
        strncpy(szLSK_AppName, szAppName, sizeof(szLSK_AppName));
    	break;
    case SCK_RAK:
        strncpy(szRAK_AppName, szAppName, sizeof(szRAK_AppName));
        break;
    case SCK_RSK:
        strncpy(szRSK_AppName, szAppName, sizeof(szRSK_AppName));
        break;
    }
}

static void SYS_SetShortCutAppName(void)
{
	FS_WritePrivateProfileString(SN_SHORTCUT,KN_LEFTARROWID,szLAK_AppName,SETUPFILENAME);
	FS_WritePrivateProfileInt(SN_SHORTCUT, KN_LAFID, LAKF_ID, SETUPFILENAME);

	FS_WritePrivateProfileString(SN_SHORTCUT,KN_LEFTSOFTID,szLSK_AppName,SETUPFILENAME);
	FS_WritePrivateProfileInt(SN_SHORTCUT, KN_LSFID, LSKF_ID, SETUPFILENAME);

	FS_WritePrivateProfileString(SN_SHORTCUT,KN_RIGHTARROWID,szRAK_AppName,SETUPFILENAME);
	FS_WritePrivateProfileInt(SN_SHORTCUT, KN_RAFID, RAKF_ID, SETUPFILENAME);

	FS_WritePrivateProfileString(SN_SHORTCUT,KN_RIGHTSOFTID,szRSK_AppName,SETUPFILENAME);
	FS_WritePrivateProfileInt(SN_SHORTCUT, KN_RSFID, RSKF_ID, SETUPFILENAME);
}
/***************************************************************
* Function  shortcut setting info
* Purpose   
* Params
* Return    
* Remarks
***************************************************************/
static SWITCHTYPE Ind_LED_State = SWITCH_OFF;

SWITCHTYPE GetIndicatorLEDStat(void)
{
	return Ind_LED_State;
}

void SetIndicatorLEDStat(SWITCHTYPE ind_LED)
{
	if (ind_LED != Ind_LED_State)
	{
		Ind_LED_State = ind_LED;
		
		FS_WritePrivateProfileInt(SN_DISPLAY, KN_IND_LED, ind_LED, SETUPFILENAME);
	}
}

static void SYS_GetIndicatorLEDStat(void)
{
	Ind_LED_State = FS_GetPrivateProfileInt(SN_DISPLAY, KN_IND_LED, SWITCH_OFF, SETUPFILENAME);
}
/***************************************************************
* Function  GetUsingTimeZone()
* Purpose  
* Params
* Return    
* Remarks
***************************************************************/
TIMEZONE GetUsingTimeZone(void)
{
	return timeZone;
}
static void SYS_GetUsingTimeZone(void)
{
    timeZone = FS_GetPrivateProfileInt(SN_TIMEZONE,KN_TIMEZONE,TZ_HOME,SETUPFILENAME);
}


void SetUsingTimeZone(TIMEZONE tz, SYSTEMTIME *st)
{ 
	if (timeZone == tz)
		return;
	
    if(tz == TZ_HOME)
    {
    GetNewZoneTime(st,GetHomeZoneStr(),GetVisitZoneStr());
    FS_WritePrivateProfileInt(SN_TIMEZONE,KN_TIMEZONE,tz,SETUPFILENAME);
    }
    if(tz == TZ_VISIT)
    {
    GetNewZoneTime(st,GetVisitZoneStr(),GetHomeZoneStr());
    FS_WritePrivateProfileInt(SN_TIMEZONE,KN_TIMEZONE,tz,SETUPFILENAME);
    }
	timeZone = tz;
}

/***************************************************************
* Function  SetDisplayBackLitMode
* Purpose   save the back light state
* Params    litMode: auto, or keep on
* Return
* Remarks
***************************************************************/
void SetDisplayBackLitMode(LIGHTMODE litMode)
{
	if (litMode != DisLitMode)
	{
		if (litMode == LS_KEEP_ON)
		{
			f_sleep_register(BKLIGHT);
		}
		else
		{
			f_sleep_unregister(BKLIGHT);
		}
		DisLitMode = litMode;
	}
}
static void SYS_SetDisplayBackLitMode(void)
{
    FS_WritePrivateProfileInt(SN_DISPLAY,KN_BGLIGHTMODE,DisLitMode,SETUPFILENAME);

}
/***************************************************************
* Function  GetDisplayBackLitMode
* Purpose
* Params
* Return     LS_AUTO;  LS_KEEP_ON
* Remarks   default value of display back light: automatic
***************************************************************/
LIGHTMODE GetDisplayBackLitMode(void)
{
	return DisLitMode;
}
static void SYS_GetDisplayBackLitMode(void)
{
    int ret = FS_GetPrivateProfileInt(SN_DISPLAY,KN_BGLIGHTMODE,0,SETUPFILENAME);
        
	DisLitMode = (LIGHTMODE)ret;
}

/***************************************************************
* Function  SetKeypadLitMode
* Purpose
* Params
* Return
* Remarks
***************************************************************/
void SetKeypadLitMode(LIGHTMODE litMode)
{
	if (litMode != KeyPadMode)
	{
		if (litMode == LS_KEEP_ON)
		{
			DHI_SetKeylight(TRUE);
		}
		else if (litMode == LS_KEEP_OFF)
		{
			DHI_SetKeylight(FALSE);
		}
		
		if (litMode == LS_KEEP_ON)
		{
			f_sleep_register(KEYLIGHT);
		}
		else
		{
			f_sleep_unregister(KEYLIGHT);
		}
	}
	KeyPadMode = litMode;
}
static void SYS_SetKeypadLitMode(void)
{
    FS_WritePrivateProfileInt(SN_DISPLAY,KN_KPLIGHTMODE,KeyPadMode,SETUPFILENAME);
}
/***************************************************************
* Function  GetKeypadLitMode
* Purpose
* Params
* Return    LS_AUTO; LS_KEEP_ON; LS_KEEP_OFF
* Remarks
***************************************************************/
LIGHTMODE GetKeypadLitMode(void)
{
	return KeyPadMode;
}
static void SYS_GetKeypadLitMode(void)
{
    int ret = FS_GetPrivateProfileInt(SN_DISPLAY,KN_KPLIGHTMODE,0,SETUPFILENAME);
	KeyPadMode = ret;
}
/***************************************************************
* Function  Set\GetWriLangugae
* Purpose
* Params
* Return    
* Remarks
***************************************************************/
static BOOL SYS_SetWriLanguage(void)
{	   
    FS_WritePrivateProfileInt(SN_WRILANGUAGE,KN_WRILANGUAGEID,iWriLangIndex,SETUPFILENAME);
    return TRUE;
}
BOOL SetWriLanguage(int iIndex)
{
	iWriLangIndex = iIndex;
	return TRUE;
}
int GetWriLanguage(void)
{
	return iWriLangIndex;
}

static void SYS_GetWriLanguage(void)
{
    iWriLangIndex = FS_GetPrivateProfileInt(SN_WRILANGUAGE,KN_WRILANGUAGEID,0,SETUPFILENAME);    
}

TCHAR* GetWriLangStr(void)
{
	int i;
	int WritingLang = GetWriLanguage();
	
	if(!GetLanguageInfo(NULL,&nLang)) //get the number of language
		return NULL;
	
	if(!GetLanguageInfo(&LangName,NULL)) //get language string
		return NULL;

	for (i=0; i<nLang; i++)
	{
		if (i == WritingLang)
		{
			return LangName[i];
		}
	}

	return NULL;
}



/***************************************************************
* Function  Accessory setting related
* Purpose  
* Params
* Return    
* Remarks
***************************************************************/
void SYS_GetAccessorySettingInfo(void)
{
	HSAccInfo.use_profile = FS_GetPrivateProfileInt(SN_ACCESSORY,KN_HEADSETPROFILE,-1,SETUPFILENAME);
	HSAccInfo.auto_ans = FS_GetPrivateProfileInt(SN_ACCESSORY,KN_HEADSETAUTOANS,SWITCH_OFF,SETUPFILENAME);
	
	CKAccInfo.use_profile = FS_GetPrivateProfileInt(SN_ACCESSORY,KN_CARKITPROFILE,-1,SETUPFILENAME);
	CKAccInfo.auto_ans = FS_GetPrivateProfileInt(SN_ACCESSORY,KN_CARKITAUTOANS,SWITCH_OFF,SETUPFILENAME);
	
	SPAccInfo.auto_ans = FS_GetPrivateProfileInt(SN_ACCESSORY,KN_SPEAKERAUTOANS,SWITCH_OFF,SETUPFILENAME);
}

void GetAccessorySettingInfo(ACCESSORY_SETUP *AccInfo, ACCESSORY_TYPE AccType)
{
    switch(AccType)
    {
    case ACC_HEADSET:
        AccInfo->use_profile = HSAccInfo.use_profile;
        AccInfo->auto_ans = HSAccInfo.auto_ans;
        break;
    case ACC_CARKIT:
        AccInfo->use_profile = CKAccInfo.use_profile;
        AccInfo->auto_ans = CKAccInfo.auto_ans;
        break;
    case ACC_SPEAKER:
        AccInfo->auto_ans = SPAccInfo.auto_ans;
        break;
    }
}

static void SYS_SaveAutoAnsState(void)
{
	FS_WritePrivateProfileInt(SN_ACCESSORY,KN_HEADSETAUTOANS,HSAccInfo.auto_ans,SETUPFILENAME);
	FS_WritePrivateProfileInt(SN_ACCESSORY,KN_CARKITAUTOANS,CKAccInfo.auto_ans,SETUPFILENAME);
	FS_WritePrivateProfileInt(SN_ACCESSORY,KN_SPEAKERAUTOANS,SPAccInfo.auto_ans,SETUPFILENAME);
}

void SaveAutoAnsState(ACCESSORY_TYPE AccType, SWITCHTYPE AutoAnsState)
{
    switch(AccType)
    {
    case ACC_HEADSET:
        HSAccInfo.auto_ans = AutoAnsState;
        break;
    case ACC_CARKIT:
        CKAccInfo.auto_ans = AutoAnsState;
        break;
    case ACC_SPEAKER:
		SPAccInfo.auto_ans = AutoAnsState;
        break;
    default:
        break;
    }
}
static void SYS_SaveAccessoryProfileInfo(void)
{
	FS_WritePrivateProfileInt(SN_ACCESSORY,KN_HEADSETPROFILE,HSAccInfo.use_profile,SETUPFILENAME);
	FS_WritePrivateProfileInt(SN_ACCESSORY,KN_CARKITPROFILE,CKAccInfo.use_profile,SETUPFILENAME);
}


void SaveAccessoryProfileInfo(ACCESSORY_TYPE AccType, int iProfileIndex)
{
    switch(AccType)
    {
    case ACC_HEADSET:
		HSAccInfo.use_profile = iProfileIndex;
        break;
    case ACC_CARKIT:
		CKAccInfo.use_profile = iProfileIndex;
        break;
    default:
        break;
    }
}

/***************************************************************
* Function  Sett_RestoreRamSettingInfo()
* Purpose  
* Params
* Return    
* Remarks
***************************************************************/
void Sett_RestoreRamSettingInfo(void)
{
	iScrSaverTime = 2; //default value of screen saver time;
	iIdleShowInfo = 0; //default value of idle shown info;
	dateFormat = 0; //default value of date format;
	timeFormat = 1; //default value of time format;
	keyLockMode = 0; //default value of keybad lock mode;
	Ind_LED_State = SWITCH_OFF;//default value of indicator led state;
	DisLitMode = LS_AUTO;
	KeyPadMode = LS_AUTO;
	//restore the default value of unit;
	g_DisUnit = DIS_METERS;
	g_SpeedUnit = KM_PER_HOUR;
	g_EleUnit = ELE_METERS;
	timeZone = TZ_HOME;
	iWriLangIndex = 0;

	HSAccInfo.use_profile = -1;
	HSAccInfo.auto_ans = SWITCH_OFF;
	
	CKAccInfo.use_profile = -1;
	CKAccInfo.auto_ans = SWITCH_OFF;
	
	SPAccInfo.auto_ans = SWITCH_OFF;

	//shortcut setting info restore;
	memset(szLSK_AppName, 0, PMNODE_NAMELEN);
	memset(szRSK_AppName, 0, PMNODE_NAMELEN);
	memset(szLAK_AppName, 0, PMNODE_NAMELEN);
	memset(szRAK_AppName, 0, PMNODE_NAMELEN);
	strcpy(szLSK_AppName, "Messaging");
	strcpy(szRSK_AppName, "Navigation");
	strcpy(szLAK_AppName, "Contacts");
	strcpy(szRAK_AppName, "Calendar");
	LSKF_ID = 3;
	RSKF_ID = 2;
	LAKF_ID = 0;
	RAKF_ID = 1;

	Sett_RestoreRamTimeSrc();
	Sett_RestoreRamOCState();


	/****************reset auto switch on/off********************************/
    DlmNotify(PS_POWEROFF,ICON_CANCEL);
    DlmNotify(PS_POWERON,ICON_CANCEL);
    DlmNotify(PS_POWERONOFF,ICON_CANCEL);
	RTC_KillAlarms(AUTO_POWER_ON, ALM_ID_AUTO_POWER_ON);
	RTC_KillAlarms(AUTO_POWER_OFF, ALM_ID_AUTO_POWER_OFF);

	
	if (Sett_FileIfExist(PMS_FILE_NAME))
		unlink(PMS_FILE_NAME);

	/************************************************************************/

	SetAutoLanguage(SWITCH_ON);
	Sett_ProcessLangChange();

}
/*===================================================================
* Function  Sett_SpecialStrProc()
* Purpose  
* Params
* Return    
* Remarks	erase the space at the head or end of string. sucess return 1;
*           fail return 0;
*           if all character of string are space, return -1;
*
====================================================================*/
int Sett_SpecialStrProc(char *str)
{
	int nLen, nSrcLen;
	int i;
	int HeadSpcNum = 0, EndSpcNum = 0;
	char *newStr;
	BYTE tmp;

	if (str == NULL)
	{
		return 0;
	}
	nSrcLen = strlen(str);

	nLen = nSrcLen;
	
	for (i=0; i<nLen; i++)
	{
		tmp = *(str + i);
		if (tmp == ' ')
		{
			*(str+i) = '\0';
			HeadSpcNum++;
		}
		else
			break;
	}

	if (HeadSpcNum == nLen)
		return -1;

	newStr = str + HeadSpcNum;

	nLen -= HeadSpcNum;
	
	while ( *(newStr + nLen - 1) == ' ')
	{
		*(newStr + nLen - 1) = '\0';
		nLen--;
		EndSpcNum++;
	}

	strncpy(str, newStr, nLen);

	
	for (i=0; i<nSrcLen - nLen; i++)
	{
		*(str + nLen + i) = '\0';
	}

	return 1;
}

static void GetAppNameForPM(char* szName, int* fID)
{
	int iIndex;

	if (szName == NULL)
		return;

	iIndex = GetAppIndexByName(szName);
	switch(iIndex)
	{
/*
	case 0://Bluetooth
		if ( 1 == fID )
		{
			strncpy(szName, ML("Bluetooth ON/OFF"), PMNODE_NAMELEN);
		}
		else if ( 2 == fID )
		{
			strncpy(szName, ML("Search for new devices"), PMNODE_NAMELEN);			
		}
		break;
	case 1://Calculator
		break;
	case 2://Calendar
		if ( 1 == fID )
		{
			strncpy(szName, ML("Calendar entrylist for current date"), PMNODE_NAMELEN);
		}
		break;
	case 3://Clock
		if ( 1 == fID )
		{
			strncpy(szName, ML("Set alarm"), PMNODE_NAMELEN);			
		}
		break;
	case 4://Contacts
		break;
	case 5://Currency converter
		break;
	case 6://GPS monitor
		switch(fID)
		{
		case 1:
			strncpy(szName, ML("Satellite view"), PMNODE_NAMELEN);			
			break;
		case 2:
			strncpy(szName, ML("Position refresh"), PMNODE_NAMELEN);			
			break;
		case 3:
			strncpy(szName, ML("Position view"), PMNODE_NAMELEN);			
			break;
		case 4:
			strncpy(szName, ML("Compass view"), PMNODE_NAMELEN);			
			break;
		default:
			break;
		}
		break;
	case 7://Help
		break;
	case 8://Logs
		switch(fID)
		{
		case 1:
			strncpy(szName, ML("Missed calls"), PMNODE_NAMELEN);			
			break;
		case 2:
			strncpy(szName, ML("Call counters"), PMNODE_NAMELEN);			
			break;
		case 3:
			strncpy(szName, ML("Communications"), PMNODE_NAMELEN);			
			break;
		default:
			break;
		}
		break;*/
	
	case 9://Memory management
		strcpy(szName, "Memory");
		*fID = 0;
		break;

	case 10://Messaging
		switch(*fID)
		{
/*
		case 1:
			strncpy(szName, ML("Inbox"), PMNODE_NAMELEN);			
			break;
		case 2:
			strncpy(szName, ML("Inbox of default mailbox"), PMNODE_NAMELEN);			
			break;
*/
		case 3:
			strncpy(szName, "Edit SMS", PMNODE_NAMELEN);
			*fID = 0;
			break;
		case 4:
			strncpy(szName, "Edit Email", PMNODE_NAMELEN);			
			*fID = 0;
			break;
		case 5:
			strncpy(szName, "Edit MMS", PMNODE_NAMELEN);			
			*fID = 0;
			break;
		default:
			break;
		}
		break;
	
/*
	case 11: //Navigation client
		if ( 1 == fID )
		{
			strncpy(szName, ML("Map base view"), PMNODE_NAMELEN);			
		}
		else if ( 2 == fID )
		{
			strncpy(szName, ML("Navigate to"), PMNODE_NAMELEN);		
		}
		
		break;
	case 12://Network services
		if ( 1 == fID )
		{
			strncpy(szName, ML("Call forwarding"), PMNODE_NAMELEN);			
		}
		break;
	case 13://Notepad
		if ( 1 == fID )
		{
			strncpy(szName, ML("Write new note"), PMNODE_NAMELEN);			
		}
		break;
	case 14://Pictures
		break;
*/	case 15://Program manager
		strncpy(szName, "Manager", PMNODE_NAMELEN);			
		break;
/*	
	case 16://Settings
		if ( 1 == fID )
		{
			strncpy(szName, ML("Sound profiles"), PMNODE_NAMELEN);			
		}
		break;
	case 17://sounds
		break;
	case 18://Wap browser
		if ( 1 == fID )
		{
			strncpy(szName, ML("Bookmarks"), PMNODE_NAMELEN);			
		}
		break;
	case 19: //games
	case 20: //games
	case 21: //games
	case 22: //games
	case 23: //games
	case 24: //games*/

	default:
		break;
	}

}

static int Setting_FILE_HANDLE = 0;

int ZC_Open(const char* pathname, int flags, mode_t mode)
{
	int fd;

	if (mode == 0)
	{
		fd = open(pathname, flags);
	}
	else
	{
		fd = open(pathname, flags, mode);
	}

	if (fd < 0)
		return -1;
	else //if (fd>=0)
	{
		Setting_FILE_HANDLE++;
SETT_PRINT("\r\n(Just open a file) File handle number of settings is %d\r\n", Setting_FILE_HANDLE);
		return fd;
	}
}

int ZC_Close(int fd)
{
	int iret;

	if(fd < 0)
	{
		iret = -1;
		SETT_PRINT("\r\n file handle is not valid ====> %d \r\n", fd);
	}
	else
	{
		iret = close(fd);
		if (iret == 0)
		{
			Setting_FILE_HANDLE--;
SETT_PRINT("\r\n(Just close a file) File handle number of settings is %d\r\n", Setting_FILE_HANDLE);		
		}
		else
		{
		SETT_PRINT("\r\n close file failed, return ===> %d \r\n", iret);			
		}
	}

	return iret;
}

int Sett_GetFileHandleNum(void)
{
	return Setting_FILE_HANDLE;
}

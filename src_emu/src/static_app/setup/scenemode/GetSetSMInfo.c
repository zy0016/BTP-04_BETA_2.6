/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : file operation functions for sound profile
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
static int nUserProfileNum;
static BOOL iGetCurModeFlag = FALSE; //flag for if the current active profile index is read
static SCENEMODE Global_Scenemode, Global_HeadsetSM, Global_CarkitSM;
static int Global_CurScene = 0;
static int iActiveMode = 0;


extern  int     GetMaxRingVolume(void);
extern  int     GetMinRingVolume(void);
extern  int     GetMaxHeadphoneVolume(void);
extern  int     GetMinHeadphoneVolume(void);
extern  char *  GetDefaultRing(void);   
extern  char *  GetDefaultSmsRing(void);
extern  char *  GetDefaultMmsRing(void);
extern  char *  GetDefKeyTouch(int iRingNum);
extern  char *  GetNoneMusic(void);
extern  char *  GetNoneKeyTouch(void);
extern  BOOL FileIfExist (const char * pFileName);
extern  int  IspIsNum(void);
extern  TIMESOURCE GetTimeSource(void);
extern  void GetAccessorySettingInfo(ACCESSORY_SETUP *AccInfo, ACCESSORY_TYPE AccType);

extern  void SaveAccessoryProfileInfo(ACCESSORY_TYPE AccType, int iProfileIndex);
extern  void SYS_GetAccessorySettingInfo(void);

extern  BOOL GetHeadSetStatus(void);

void Sett_InitSettInfo(void);
int  GetDefaultRingEx(char *pFilePath);
BOOL Sett_FileIfExist(const char * pFileName);

static int iSceneModeNum;


#define     KN_UPNOID               "userpflnbrid"
#define     KN_UPNAME               "userpflname"
/***************************************************************************************/
#define  SN_PROFILESETT   "Sett_sound_profile"
#define  KN_PROFILEID	  "Sett_sound_profile_id"

int SYS_GetCurSceneMode(void)
{
    Global_CurScene = FS_GetPrivateProfileInt(SN_PROFILESETT,KN_PROFILEID,0,SETDIALFILE);
	return Global_CurScene;
}
void SYS_SetCurSceneMode()
{
	FS_WritePrivateProfileInt(SN_PROFILESETT,KN_PROFILEID,Global_CurScene,SETDIALFILE);
}

BOOL SettGetCarkitStatus(void)
{
	return FALSE;
}
/***************************************************************************************/

BOOL    GetSceneModeCall(void)
{
    return FALSE;
}

static    char szFixedProfileNormal[MODENAMEMAXLEN];
static    char szFixedProfileSilent[MODENAMEMAXLEN];
static    char szFixedProfileNoisy[MODENAMEMAXLEN];
static    char szFixedProfileDiscreet[MODENAMEMAXLEN];

static SCENEMODE smdefault[] = 
{
    {"",{"",3,NORMAL},{"",2,NORMAL},{"",2,NORMAL},{"",3,NORMAL},{"",3,NORMAL},3,{2,3,SWITCH_OFF},1,SWITCH_OFF,ALERTALL,{0}, 1},//
    {"",{"",0,NORMAL},{"",0,NORMAL},{"",0,NORMAL},{"",0,NORMAL},{"",0,NORMAL},0,{0,0,SWITCH_OFF},0,SWITCH_OFF,ALERTALL,{0}, 0},//
    {"",{"",5,NORMAL},{"",5,NORMAL},{"",5,NORMAL},{"",5,NORMAL},{"",5,NORMAL},5,{2,5,SWITCH_OFF},5,SWITCH_ON, ALERTALL,{0}, 0},//
    {"",{"",2,NORMAL},{"",2,NORMAL},{"",2,NORMAL},{"",2,NORMAL},{"",2,NORMAL},0,{2,2,SWITCH_OFF},0,SWITCH_ON, ALERTALL,{0}, 0},//
};

static void InitDefaultNameOfSP(void)
{
    strcpy(smdefault[0].cModeName, "Normal");
    strcpy(smdefault[1].cModeName, "Silent");
    strcpy(smdefault[2].cModeName, "Noisy");
    strcpy(smdefault[3].cModeName, "Discreet");    
}
static BOOL    Sett_SetActiveSM(const SCENEMODE* scenemode) //providing for other applications
{
	if (scenemode == NULL)
		return FALSE;

	memcpy(&Global_Scenemode, scenemode, sizeof(SCENEMODE));
	return TRUE;
}

BOOL    RestoreSceneMode(void)
{
    int i;
	SetCurSceneMode(0);
    for(i=0;i<FIXEDPROFILENUMBER;i++)
    {
		SetSM(&smdefault[i], i);
	}
    return TRUE;
}
/*********************************************************************\
* Function   GetSM  
* Purpose    
* Params     scenemode:
* Return     
* Remarks      
**********************************************************************/

BOOL    GetSM(SCENEMODE * scenemode,int iscenemode)
{
    int     i;
    int		hf;

	char DefRingName[RINGNAMEMAXLEN+1];

	memset(DefRingName, 0, sizeof(DefRingName));
  
	GetDefaultRingEx(DefRingName);
    
    iSceneModeNum = FIXEDPROFILENUMBER+GetUserProfileNo();

    if ((iscenemode < 0) || (iscenemode > iSceneModeNum - 1))
        iscenemode = 0;

    if(-1 == (hf = ZC_Open(SETUP_SCENEMODEFN, O_RDONLY,0)))
    {
        if (-1 == (hf = ZC_Open(SETUP_SCENEMODEFN, O_RDWR|O_CREAT, S_IRWXU)))
        {
			unlink(SETUP_SCENEMODEFN);
			memcpy(scenemode,&smdefault[iscenemode],sizeof(SCENEMODE));
			return FALSE;
        }
        if (-1 == lseek(hf, 0L, SEEK_SET))
		{
			ZC_Close(hf);
			unlink(SETUP_SCENEMODEFN);
			memcpy(scenemode,&smdefault[iscenemode],sizeof(SCENEMODE));
			return FALSE;
		}

        InitDefaultNameOfSP();

        for (i = 0;i < FIXEDPROFILENUMBER;i++)
        {
			if (!Sett_FileIfExist(smdefault[i].rIncomeCall.cMusicFileName))
			{
				strcpy(smdefault[i].rIncomeCall.cMusicFileName, DefRingName);
			}
			if (!Sett_FileIfExist(smdefault[i].rMessage.cMusicFileName))
			{
				strcpy(smdefault[i].rMessage.cMusicFileName, DefRingName);
			}
			if (!Sett_FileIfExist(smdefault[i].rTelematics.cMusicFileName))
			{
				strcpy(smdefault[i].rTelematics.cMusicFileName, DefRingName);
			}
			if (!Sett_FileIfExist(smdefault[i].rCalendar.cMusicFileName))
			{
				strcpy(smdefault[i].rCalendar.cMusicFileName, DefRingName);
			}
			if (!Sett_FileIfExist(smdefault[i].rAlarmClk.cMusicFileName))
			{
				strcpy(smdefault[i].rAlarmClk.cMusicFileName, DefRingName);
			}
            if (-1 == write(hf, &smdefault[i], sizeof(SCENEMODE)))
            {
				ZC_Close(hf);
				unlink(SETUP_SCENEMODEFN);
				memcpy(scenemode,&smdefault[iscenemode],sizeof(SCENEMODE));
				return FALSE;
			}
        }
        ZC_Close(hf);
        memcpy(scenemode,&smdefault[iscenemode],sizeof(SCENEMODE));
        return FALSE;
    }
    else
    {
        DWORD   dfsize = 0;
        char * pMusic = NULL,* pKeyTouch = NULL;
		struct stat *buf = NULL;
		
		buf = (struct stat*)malloc(sizeof(struct stat));
		
		if(buf == NULL)
		{
			ZC_Close(hf);
			return FALSE;
		}
		
		memset(buf, 0, sizeof(struct stat));
		stat(SETUP_SCENEMODEFN, buf);
		
		dfsize = buf->st_size;
		
		free(buf);
		buf = NULL;

		if (sizeof(SCENEMODE) * iSceneModeNum != dfsize)
		{
			ZC_Close(hf);
			unlink(SETUP_SCENEMODEFN);
			memcpy(scenemode,&smdefault[iscenemode],sizeof(SCENEMODE));
			return FALSE;
		}

        if (-1 == lseek(hf, sizeof(SCENEMODE) * iscenemode, SEEK_CUR))
		{
			ZC_Close(hf);
			unlink(SETUP_SCENEMODEFN);
			memcpy(scenemode,&smdefault[iscenemode],sizeof(SCENEMODE));
			return FALSE;
		}

        if (-1 == read(hf,scenemode,sizeof(SCENEMODE)))
		{
			ZC_Close(hf);
			unlink(SETUP_SCENEMODEFN);
			memcpy(scenemode,&smdefault[iscenemode],sizeof(SCENEMODE));
			return FALSE;
		}

        if(iscenemode >= 0 && iscenemode < FIXEDPROFILENUMBER)
        {
            strcpy(scenemode->cModeName, smdefault[iscenemode].cModeName);
        }
		
		if (!Sett_FileIfExist(scenemode->rIncomeCall.cMusicFileName))
		{
			strcpy(scenemode->rIncomeCall.cMusicFileName, DefRingName);
		}
		if (!Sett_FileIfExist(scenemode->rMessage.cMusicFileName))
		{
			strcpy(scenemode->rMessage.cMusicFileName, DefRingName);
		}
		if (!Sett_FileIfExist(scenemode->rTelematics.cMusicFileName))
		{
			strcpy(scenemode->rTelematics.cMusicFileName, DefRingName);
		}
		if (!Sett_FileIfExist(scenemode->rCalendar.cMusicFileName))
		{
			strcpy(scenemode->rCalendar.cMusicFileName, DefRingName);
		}
		if (!Sett_FileIfExist(scenemode->rAlarmClk.cMusicFileName))
		{
			strcpy(scenemode->rAlarmClk.cMusicFileName, DefRingName);
		}
		
        ZC_Close(hf);

        return TRUE;

    }
}

/*********************************************************************\
* Function   SetSM  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/

BOOL    SetSM(const SCENEMODE * scenemode,int iscenemode)
{
    int     hf;
    long    lfseek = 0;
	ACCESSORY_SETUP accType;
        
	iSceneModeNum = FIXEDPROFILENUMBER+GetUserProfileNo();

    if ((iscenemode < 0) || (iscenemode > iSceneModeNum))
        return FALSE;
    
    if(-1 == (hf = ZC_Open(SETUP_SCENEMODEFN, O_WRONLY,0)))
    {
        if (-1 == (hf = ZC_Open(SETUP_SCENEMODEFN, O_RDWR|O_CREAT, S_IRWXU)))
         {
            return FALSE;
         }
    }
    lfseek = iscenemode * sizeof(SCENEMODE);
    
    if (-1 == lseek(hf, lfseek, SEEK_CUR))
    {
        ZC_Close(hf);
        return FALSE;
    }
    if (-1 != write(hf, (PVOID)scenemode, sizeof(SCENEMODE)))
    {
        ZC_Close(hf);
/********************** modify global value if necessary **********************************/
		if (iscenemode == GetCurSceneMode())
		{
			Sett_SetActiveSM(scenemode);
		}

		GetAccessorySettingInfo(&accType, ACC_HEADSET);

		if (iscenemode == accType.use_profile)
		{
			memcpy(&Global_HeadsetSM, scenemode, sizeof(SCENEMODE));
		}
		
		GetAccessorySettingInfo(&accType, ACC_CARKIT);

		if (iscenemode == accType.use_profile)
		{
			memcpy(&Global_CarkitSM, scenemode, sizeof(SCENEMODE));
		}
/********************** modify global value if necessary **********************************/
        return TRUE;
    }
    else
    {
        ZC_Close(hf);
        return FALSE;
    }
}

BOOL AddSM(SCENEMODE sm)
{
  int iNum;
 
  if (IsFlashAvailable(0) == 0) 
  {
	  printf("\r\n not enough flash space\r\n");
	  return FALSE;
  }

  iNum = GetUserProfileNo();
  sprintf(sm.cModeName,"%s %d",ML("Profile"), FIXEDPROFILENUMBER + iNum + 1);
  sm.iActiveFlag = 0;   
  if(!SetSM(&sm,FIXEDPROFILENUMBER + iNum))
  {
	  return FALSE;
  }
  SetUserProfileNo(iNum+1);
  return TRUE;
}

BOOL DelSM(int iCurMode)
{
    int     hf;
    long    lfseek = 0;
    int     nProfileLeftNum;
    SCENEMODE *tmSM = NULL;
	ACCESSORY_SETUP accProfile;
 	if(iCurMode<FIXEDPROFILENUMBER)
		return FALSE;
    
    nProfileLeftNum = FIXEDPROFILENUMBER + nUserProfileNum - iCurMode - 1;

    if (nProfileLeftNum < 0)
        return FALSE;
    
    if (nProfileLeftNum > 0)
    {
        tmSM = (SCENEMODE*)malloc(nProfileLeftNum * sizeof(SCENEMODE));
    
        if( NULL == tmSM)
            return FALSE;

        if(-1 == (hf = ZC_Open(SETUP_SCENEMODEFN, O_RDWR,0)))
        {
            return FALSE;
        }
        
        lfseek = (iCurMode + 1) * sizeof(SCENEMODE); 
        
        if (-1 == lseek(hf, lfseek, SEEK_SET))
        {
            ZC_Close(hf);
            return FALSE;
        }
        
        if (-1 == read(hf, tmSM, nProfileLeftNum * sizeof(SCENEMODE)))
		{
			ZC_Close(hf);
			return FALSE;
		}
        
        lfseek = iCurMode * sizeof(SCENEMODE); 

        if (-1 == lseek(hf, lfseek, SEEK_SET))
        {
            ZC_Close(hf);
            return FALSE;
        }
        
        if (-1 == write(hf,tmSM,nProfileLeftNum * sizeof(SCENEMODE)))
        {
            ZC_Close(hf);
            return FALSE;
        }
        
        ZC_Close(hf);
        free(tmSM);
        tmSM = NULL;      
    }
    
	if(-1==truncate(SETUP_SCENEMODEFN,sizeof(SCENEMODE)*(nUserProfileNum+FIXEDPROFILENUMBER-1)))
		return FALSE;

    SetUserProfileNo(nUserProfileNum - 1);

	if (Global_CurScene > iCurMode)
	{
		Global_CurScene--;
	}
	else if (iCurMode == Global_CurScene)
	{
		SetCurSceneMode(0);
	}

	GetAccessorySettingInfo(&accProfile, ACC_HEADSET);
	
	if (accProfile.use_profile > iCurMode)
	{
		accProfile.use_profile--;
		SaveAccessoryProfileInfo(ACC_HEADSET, accProfile.use_profile);		
	}
	else if(iCurMode == accProfile.use_profile)
	{
		SaveAccessoryProfileInfo(ACC_HEADSET, -1);	
	}

	GetAccessorySettingInfo(&accProfile, ACC_CARKIT);

	if (accProfile.use_profile > iCurMode)
	{
		accProfile.use_profile--;
		SaveAccessoryProfileInfo(ACC_CARKIT, accProfile.use_profile);		
	}
	else if(iCurMode == accProfile.use_profile)
	{
		SaveAccessoryProfileInfo(ACC_CARKIT, -1);	
	}
    
	return TRUE;
}

int     GetCurrentVolume(void)
{
    return Global_Scenemode.rIncomeCall.iRingVolume;
}


int SYS_GetUserProfileNo()
{
//	return FS_GetPrivateProfileInt(SN_SCENEMODE, KN_UPNOID,0,SETUPFILENAME);
    DWORD fsize = 0,ProfileNo;
    WORD  smSize;
    struct stat *buf = NULL;
		
    buf = (struct stat*)malloc(sizeof(struct stat));
		
	if(buf == NULL)
		{
			return -1;
		}
		
    memset(buf, 0, sizeof(struct stat));
    stat(SETUP_SCENEMODEFN, buf);
		
    fsize = buf->st_size;

		
    free(buf);
    buf = NULL;
    
    if(0 == fsize)
        return 0;

    smSize = sizeof(SCENEMODE);
    ProfileNo = fsize / smSize;
    return ProfileNo - FIXEDPROFILENUMBER;    
}

int GetUserProfileNo(void)
{
     if(iGetCurModeFlag)
        return nUserProfileNum;
     else
        return SYS_GetUserProfileNo();
}

BOOL  SetUserProfileNo(int iIndex)
{
    nUserProfileNum = iIndex;
	return TRUE;
}

void    SetCurSceneMode(int iscenemode)
{
	SCENEMODE smtmp;

    if(iscenemode >= 0 && Global_CurScene != iscenemode)
    {
        if(Global_CurScene == 1)        
            DlmNotify(PS_SILENCE, ICON_CANCEL);
		else
            DlmNotify(PS_SPROHTER, ICON_CANCEL);

        Global_CurScene = iscenemode;    

        if(iscenemode == 1)
            DlmNotify(PS_SILENCE, ICON_SET);
		else if(iscenemode != 0)
            DlmNotify(PS_SPROHTER, ICON_SET);

		if (GetSM(&smtmp, Global_CurScene))
		{
			Sett_SetActiveSM(&smtmp);
		}
    }
    else
        return;   
}

int GetCurSceneMode(void)
{
    return Global_CurScene;
}

BOOL    Sett_GetActiveSM(SCENEMODE* scenemode) //providing for other applications
{
	ACCESSORY_SETUP accProfile;

	if (scenemode == NULL)
		return FALSE;

	if(GetHeadSetStatus())//headset state
	{
		GetAccessorySettingInfo(&accProfile, ACC_HEADSET);
		if (accProfile.use_profile > -1)
		{
			memcpy(scenemode, &Global_HeadsetSM, sizeof(SCENEMODE));
			return TRUE;
		}
	}

	if(SettGetCarkitStatus())
	{
		GetAccessorySettingInfo(&accProfile, ACC_CARKIT);
		if (accProfile.use_profile > -1)
		{
			memcpy(scenemode, &Global_CarkitSM, sizeof(SCENEMODE));
			return TRUE;
		}
	}

	memcpy(scenemode, &Global_Scenemode, sizeof(SCENEMODE));
	return TRUE;
}

void Sett_SetActiveAccSM(ACCESSORY_TYPE AccType, int iProfileIndex)
{
	switch(AccType)
	{
	case ACC_HEADSET:
		GetSM(&Global_HeadsetSM, iProfileIndex);
		break;
	case ACC_CARKIT:		
		GetSM(&Global_CarkitSM, iProfileIndex);
		break;
	default:
		break;
	}
}
/*******************************************************************************/

BOOL Init_ProfileSetting(void)
{
	ACCESSORY_SETUP headsetProfile;
	ACCESSORY_SETUP carkitProfile;


    InitDefaultNameOfSP();

    if((nUserProfileNum = SYS_GetUserProfileNo()) < 0)
        return FALSE;
   
    iGetCurModeFlag = TRUE;
	SYS_GetAccessorySettingInfo();
	SYS_GetCurSceneMode();	
	
	GetAccessorySettingInfo(&headsetProfile, ACC_HEADSET);

	if (headsetProfile.use_profile > -1)
	{		
		GetSM(&Global_HeadsetSM, headsetProfile.use_profile);
	}
	GetAccessorySettingInfo(&carkitProfile, ACC_CARKIT);

	if (carkitProfile.use_profile > -1)
	{		
		GetSM(&Global_CarkitSM, carkitProfile.use_profile);
	}
	
	GetSM(&Global_Scenemode, GetCurSceneMode());

	Sett_InitSettInfo();
    
//    SYS_InitGPSProfile();
    
    SetUp_SetDialCfg();//initialise the connection setting info.          
    
    IspIsNum();
	
	GetTimeSource();    
    return TRUE;
}

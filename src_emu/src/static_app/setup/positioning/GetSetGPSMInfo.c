/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : GPS profile
 *
 * Purpose  : file operation functions of GPS profile
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
#include    "PositioningSetting.h"
#include    "locapi.h"

static int nGPSProfileNum;
static  GPSMODE * Global_gm;
static int iActGPSIndex;
extern BOOL SetGPSprofile(GPSMODE *gm, int iGPSModeIndex);
extern void SetBenefonActGPS(GPSMODE *gpsPollexUiProfile);
extern GPSMODE* Sett_GetGPSModeList(void);

static GPSMODE  GPSsmdefault[] = //default value
{
    {"Full power","ROM:setup/gps/GPS_profile_6.ico",GPSRI_ONESEC,GPSRIC_KIPINT,GPS_SENSITI,GPSNMEA_FOUREIGHTOO, 1, 0},
    {"Economy","ROM:setup/gps/GPS_profile_5.ico",GPSRI_THREESEC,GPSRIC_KIPINT,GPS_SENSITI,GPSNMEA_OFF, 2, 0}
};

BOOL    RestoreGPSProfile(void)//
{
    int i;
    SetOffGPSProfile();
	for(i=0;i<PRESETGPSPROFILENUM;i++)
    {
		SetGPSprofile(&GPSsmdefault[i], i);
	}
    return TRUE;
}

#define DEFAULT_GPS_ICON "ROM:setup/gps/GPS_profile_1.ico"
static GPSMODE GPSsmNew =
{"", DEFAULT_GPS_ICON,  GPSRI_ONESEC, GPSRIC_CON, GPS_SENSITI, GPSNMEA_OFF, 0, 0};

int SYS_GetGPSprofileNum(void)
{
    DWORD fsize = 0,ProfileNo;
    WORD  smSize;
    struct stat *buf = NULL;
		
    buf = (struct stat*)malloc(sizeof(struct stat));
		
	if(buf == NULL)
    {
	    return -1;
	}
		
    memset(buf, 0, sizeof(struct stat));
    stat(SETUP_GPSMODE, buf);
		
    fsize = buf->st_size;

		
    free(buf);
    buf = NULL;
    
    if(0 == fsize)
        return 0;

    smSize = sizeof(GPSMODE);
    ProfileNo = fsize / smSize;
    nGPSProfileNum = ProfileNo;   

    return ProfileNo;
    
}

int GetGPSprofileNum(void)
{
    return nGPSProfileNum;
}

BOOL SYS_InitGPSProfile(void)
{
    int f;
    int i;
    f = ZC_Open(SETUP_GPSMODE, O_RDONLY,0);
    if(-1 == f) //GPS setting file not found
    {
        f = ZC_Open(SETUP_GPSMODE, O_RDWR|O_CREAT, S_IRWXU); //create new GPS setting file
        if(-1 == f)
        {
            unlink(SETUP_GPSMODE);
            return FALSE;
        }
        if (-1 == lseek(f, 0L, SEEK_SET))
        {
            ZC_Close(f);
            unlink(SETUP_GPSMODE);
            return FALSE;
        }
	
        
		for(i=0;i<PRESETGPSPROFILENUM;i++)
        {
            if (-1 == write(f, &GPSsmdefault[i], sizeof(GPSMODE))) //write the preset GPS file
            {
                ZC_Close(f);
                unlink(SETUP_GPSMODE);
                return FALSE;
            }
        }
        ZC_Close(f);
        
        nGPSProfileNum = PRESETGPSPROFILENUM;

        return TRUE;
    }
    else //file found
    {
        ZC_Close(f);
        SYS_GetGPSprofileNum(); //get the number of GPS profile
        return TRUE;
    }
}

BOOL SetGPSprofile(GPSMODE *gm, int iGPSModeIndex)
{
    int     hf;
    long    lfseek = 0;
        
    if ((iGPSModeIndex < 0) || (iGPSModeIndex > nGPSProfileNum))
        return FALSE;
    
    if(-1 == (hf = ZC_Open(SETUP_GPSMODE, O_WRONLY,0)))
    {
        if (-1 == (hf = ZC_Open(SETUP_GPSMODE, O_RDWR|O_CREAT, S_IRWXU)))
         {
            return FALSE;
         }
    }
    lfseek = iGPSModeIndex * sizeof(GPSMODE);
    
    if (-1 == lseek(hf, lfseek, SEEK_CUR))//
    {
        ZC_Close(hf);
        return FALSE;
    }
    if (-1 == write(hf, (PVOID)gm, sizeof(GPSMODE)))//
    {
        ZC_Close(hf);
        return FALSE;
    }
    else//
    {
        ZC_Close(hf);
		
		if(iGPSModeIndex == iActGPSIndex) //set current active profile
		{
			SetBenefonActGPS(gm);
		}
        
		return TRUE;
    }
    
}
BOOL GetGPSprofile(GPSMODE *gm, int iGPSModeIndex)
{
    int f;
    f = ZC_Open(SETUP_GPSMODE,O_RDONLY,0);
    if(-1 == f)//file not found
    {
        return FALSE;    
    }
    else
    {
		struct stat *buf = NULL;
        DWORD   dfsize = 0;
		
		buf = (struct stat*)malloc(sizeof(struct stat));
		
		if(buf == NULL)
		{
			ZC_Close(f);
			return FALSE;
		}
		
		memset(buf, 0, sizeof(struct stat));
		stat(SETUP_GPSMODE, buf);
		
		dfsize = buf->st_size;
		
		free(buf);
		buf = NULL;

		if (sizeof(GPSMODE) * nGPSProfileNum != dfsize)//
        {
            ZC_Close(f);
            unlink(SETUP_GPSMODE);
            return FALSE;
        }

        if (-1 == lseek(f, sizeof(GPSMODE) * iGPSModeIndex, SEEK_CUR))//
        {
            ZC_Close(f);
            unlink(SETUP_GPSMODE);
            return FALSE;
        }
        if (-1 == read(f,gm,sizeof(GPSMODE)))//
        {
            ZC_Close(f);
            unlink(SETUP_GPSMODE);
            return FALSE;
        }

        ZC_Close(f);
        return TRUE;
    }
    return FALSE;
}

BOOL CreateNewGPSProfile(void)
{
	GPSMODE* ptmpGPSmode = NULL;
	int nTmp = 0, i;

	if (IsFlashAvailable(0) == 0)
	{
		printf("\r\n not enough flash space\r\n");
		return FALSE;
	}
	
	ptmpGPSmode = Sett_GetGPSModeList();
	
	if (ptmpGPSmode == NULL)
		return FALSE;

	do 
	{
		nTmp++;
		sprintf(GPSsmNew.cGPSModeName, "%s %d", ML("Profile"), nTmp);
		for (i=0; i<nGPSProfileNum; i++)
		{
			if (strcmp(ptmpGPSmode[i].cGPSModeName, GPSsmNew.cGPSModeName) == 0)
				break;
		}
	}while(i<nGPSProfileNum);


    SetGPSprofile(&GPSsmNew, nGPSProfileNum);
    nGPSProfileNum++;
    
    return TRUE;
}

BOOL DelGPSProfile(int iCurMode)//
{
	GPSMODE gm;
	int i;
    static int iActIndex;

	if(iCurMode<PRESETGPSPROFILENUM)
		return FALSE;

    
	for(i=1;i<nGPSProfileNum-iCurMode;i++)
	{
		if(GetGPSprofile(&gm,iCurMode + i))
		SetGPSprofile(&gm,iCurMode+i-1);
		else break;
	}
    
	if(-1==truncate(SETUP_GPSMODE,sizeof(GPSMODE)*(nGPSProfileNum-1)))
		return FALSE;

	if (iCurMode < iActGPSIndex) //delete the profile before the current active profile
	{
		iActGPSIndex--;
	}

    nGPSProfileNum--;
	return TRUE;
}

void SetActiveGPSProfile(int iProfileIndex)
{
	GPSPROFILE gpsLocModUiProfile;
	GPSMODE    gpsPollexUiProfile;
	
	iActGPSIndex = iProfileIndex;
	
    if ( (iProfileIndex < nGPSProfileNum) && (iProfileIndex >= 0) )
    {
		GetGPSprofile(&gpsPollexUiProfile, iProfileIndex);

		SetBenefonActGPS(&gpsPollexUiProfile);
    }
    else if ( -1 == iProfileIndex )//set off
    {
		gpsLocModUiProfile.fixRate = FIX_RATE_OFF;
		gpsLocModUiProfile.fixRateWhileCharging = FIX_RATE_WHILE_CHARGING_OFF;
		gpsLocModUiProfile.handle = NULL;
		gpsLocModUiProfile.nmeaOutput = GPS_NMEA_OUTPUT_OFF;
		gpsLocModUiProfile.operatingMode = GPS_OPERATING_MODE_ACCURATE;
		LocSetUIGPSProfile( &gpsLocModUiProfile );
    }

	DlmNotify(PS_FRASHGPS, TRUE);
}



int GetActiveGPSProfile(void)
{
	return iActGPSIndex;
}

void SetOffGPSProfile(void)
{
    SetActiveGPSProfile(-1);
}

void SetBenefonActGPS(GPSMODE *gpsPollexUiProfile)
{
	GPSPROFILE gpsLocModUiProfile;
	
	switch( gpsPollexUiProfile->iRefIntervl )
	{
	case GPSRI_OFF:
		gpsLocModUiProfile.fixRate = FIX_RATE_OFF;
		break;
	case GPSRI_ONESEC:
		gpsLocModUiProfile.fixRate = FIX_RATE_1S_CONSTANT;
		break;
	case GPSRI_THREESEC:
		gpsLocModUiProfile.fixRate = FIX_RATE_3S;
		break;
	case GPSRI_TENSEC:
		gpsLocModUiProfile.fixRate = FIX_RATE_10S;
		break;
	case GPSRI_THIRTYMIN:
		gpsLocModUiProfile.fixRate = FIX_RATE_30MIN;
		break;
	default:
		gpsLocModUiProfile.fixRate = FIX_RATE_1S_CONSTANT;
		break;
	}
	gpsLocModUiProfile.fixRateWhileCharging = gpsPollexUiProfile->iRefMode;
	gpsLocModUiProfile.handle = NULL;
	gpsLocModUiProfile.nmeaOutput = gpsPollexUiProfile->iNMEAoutput;
	gpsLocModUiProfile.operatingMode = gpsPollexUiProfile->iOptMode;
	LocSetUIGPSProfile( &gpsLocModUiProfile );        		
}

void SYS_GetActiveGPSProfile(void)
{
    GPSMODE tmpGPS;
	int i;

    for (i=0; i<nGPSProfileNum; i++)
    {
        GetGPSprofile(&tmpGPS, i);
        if ( 1 == tmpGPS.ActiFlag )
		{
			iActGPSIndex = i;
			return;
		}
    }

    iActGPSIndex = -1; //GPS switched off;	
}
void SYS_SetActiveGPSProfile(void)
{
	int i;
	GPSMODE tmpGPS;

	for (i=0; i<nGPSProfileNum; i++)
	{
		GetGPSprofile(&tmpGPS, i);
		if (iActGPSIndex == i)
		{
			if (tmpGPS.ActiFlag != 1) //
			{
				tmpGPS.ActiFlag = 1;
				SetGPSprofile(&tmpGPS, i);
			}			
		}
		else// iActGPSIndex != i
		{
			if (tmpGPS.ActiFlag != 0)
			{
				tmpGPS.ActiFlag = 0;
				SetGPSprofile(&tmpGPS, i);
			}
		}
	}
}

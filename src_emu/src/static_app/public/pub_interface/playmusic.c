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
#include "window.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "setting.h"
#include "winpda.h"
#include "hopen/ipmc.h"
#include "fcntl.h"
#include "unistd.h"


extern int DHI_Vibration(BOOL bOn);

BOOL    SetupHf_PlayMusicFromFile(const char * pFilename,BYTE byPlayType,
                                  BYTE byPlayCount,void(*callbackproc)(BYTE))
{
    BOOL bRes = TRUE;
#ifndef _EMULATE_
    bRes = SEF_PreviewRingFromData(pFilename,byPlayType,byPlayCount,callbackproc);
#endif
    return bRes;
}
BOOL    SetupHf_PlayMusicFromData(BYTE byRingType,BYTE byPlayTime,BYTE *pBuf,
                                  unsigned long DataSize, void(*callbackproc)(BYTE))
{
    BOOL bRes = TRUE;
#ifndef _EMULATE_
    bRes = SEF_PreviewRingFromData(byRingType,byPlayTime,pBuf,DataSize,callbackproc);
#endif
    return bRes;
}
BOOL    SetupHF_EndPlayMusic(void)
{
#ifndef _EMULATE_
    AS_StopPlay();
#endif
    return TRUE;
}
/*<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
<> Function Setup_GetMusicData
<> Purpose  给播放BUffer提供数据
<> Params   
<> Return   
<> Remarks  
\*<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>*/
//int     Setup_GetMusicData(LPWAVEHDR phdr)
//{
//    return 1;
//}
/*<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
<> Function Setup_WriteMusicData
<> Purpose  给播放BUffer提供数据
<> Params   
<> Return   
<> Remarks  
\*<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>*/
//int     Setup_WriteMusicData(LPWAVEHDR lpHdr)
//{
//    return 1;
//}
//////////////////////////以上为声音播放部分,以下为其它硬件函数///////////////////////////////////////////////
void    SetupHf_LcdBklight(BOOL bEn)//开关LCD的背光灯
{
}
void    SetupHf_Lcd(BOOL bEn)//开关LCD
{
}
void    SetupHf_BgLight(int iBgLight)//亮度
{
}
void    SetupHf_Vibration(BOOL bEn)//振动
{
	DHI_Vibration(bEn);
	/*
	int	fd;
	fd = open("/dev/pmu",O_RDWR);
	ioctl(fd, IPMC_IOCTL_VIBRATORON, bEn);  //开震动
	//ioctl(fd, IPMC_IOCTL_VIBRATORON, 0);  //关震动
	close(fd);
	*/
}
BOOL    GetEarphone(void)//查询耳机状态
{
    return (FALSE);
}
static  char *  strlwr(char * str)
{
    char * pAddress = str;
    if (str == NULL)
        return NULL;
    while (* str)
    {
        if (('A' <= * str) && (* str <= 'Z'))
        {
            * str += 32;
        }
        str++;
    }
    return pAddress;
}
int     SetupHf_OpenDev(void)
{
    return 1;//(open("/dev/mixer",O_RDWR));
}
void    SetupHf_CloseDev(int fd)
{
    //close(fd);
}
int     SetupHf_GetIPGA(int fd)
{
    int iPGA = 0;
    //ioctl(fd,SOUND_MIXER_READ_MIC,&iPGA);
    return (iPGA);
}
BOOL    SetupHf_SetIPGA(int iPGA,int fd)
{
    int ires = 0;
    //ires = ioctl(fd,SOUND_MIXER_WRITE_MIC,&iPGA);
    return (ires < 0 ? FALSE : TRUE);
}
BOOL    SetupHf_Set20DB(int i20DB,int fd)
{
    int ires = 0;
    //ires = ioctl(fd,EN_MIC_20DB,&i20DB);
    return (ires < 0 ? FALSE : TRUE);
}
BOOL    SetupHf_SetMOGN(int iMOGN,int fd)
{
    int ires = 0;
    //ires = ioctl(fd,EN_MIC_MOGN,&iMOGN);
    return (ires < 0 ? FALSE : TRUE);
}
BOOL    SetupHf_SetATTS(int iATTS,int fd)
{
    int ires = 0;
    //ires = ioctl(fd,SET_ATTS,&iATTS);
    return (ires < 0 ? FALSE : TRUE);
}
int     SetupHf_GetATTS(int fd)
{
    int iATTS = 0;
    //ioctl(fd,GET_ATTS,&iATTS);
    return (iATTS);
}

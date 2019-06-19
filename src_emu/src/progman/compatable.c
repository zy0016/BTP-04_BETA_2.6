  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : progman
 *
 * Purpose  : implement charge
 *            
\**************************************************************************/

#include <fcntl.h>
#include <unistd.h>
#include <hopen/lcd.h>
#include <hopen/Ioctl.h>
#include <hopen/pmioctl.h>
#include <hopen/devmajor.h>
#include <hpdef.h>
#include <hopen/dock.h>
#include <hopen/soundcard.h>
#include <stdio.h>
#include <hopen/ipmc.h>
#include <sys/task.h>
#include <sys/mount.h>
#include <hopen/pm.h>
#include "window.h"
#include <project/compatable.h>
#include "progman.h"
#include "pmi.h"
#include "callpub.h"

#define PES_SLEEP_MAX  32
#define ET_SLEEP   6000

extern LEVELVALUE GetBrightness(void);
extern BOOL IfInAutoPowerOff(void);
extern SWITCHTYPE GetIndicatorLEDStat (void);
extern WORD GetLockKeyCode(void);

// device handle
static	int pmuhandle = -1;
static	int mixerhandle = -1;

// charger status
static BOOL bPlugCharger = FALSE;

// sleep flags
static unsigned int g_nAutoPoweroff[PES_SLEEP_MAX];
static unsigned int nTimeId_Sleep = 0;

/*******************************************************************
 PMU device DHI interface
 ******************************************************************/
/*
 *	open pmu device
 */
BOOL	DHI_OpenPmu(void)
{
	int fd;
	
	fd = open("/dev/pmu",O_RDWR);
	if ( fd<0 )
	{
		printf("\r\nOpen pmu error, please chek it.\r\n");
		return	FALSE;
	}
	pmuhandle = fd;

	return TRUE;

}
/*
 *	close pmu device
 */
void DHI_ClosePmu(void)
{
	if ( pmuhandle<0 )
		printf("\r\npmu device is already closed.\r\n");
	else
		printf("\r\n!!! pmu device should never be closed!!!\r\n");
}

/*
 *	reset the cpu
 */
void	DHI_Reset(void)
{
	if ( pmuhandle<0 )
		printf("\r\n!!!can't reset the phone, pmu is not opened, please check pmu device!!!\r\n");

	// we should disable sleep mode when reset
	f_sleep_register(INIT_FLAG);
	ioctl(pmuhandle,IPMC_IOCTL_CPU_RESET);
}

/*
 *	shut down MC55
 */
void	DHI_ShutDownWirelessModule(void)
{
	if ( pmuhandle<0 )
		printf("\r\n!!!can't shutdown MC55 compulsively,please check pmu device!!!\r\n");
	ioctl(pmuhandle,IPMC_IOCTL_SHUTDOWN_GSM);
}

/*
 *	ignite MC55
 */
void	DHI_IgniteWirelessModule(void)
{
	if ( pmuhandle<0 )
		printf("\r\nIgt MC55 error!\r\n");
	else
	{
		ioctl(pmuhandle,IPMC_IOCTL_IGNITE_GSM);
		Sleep(2500);	// wait for 1 second
		printf("\r\nIgt MC55 ok!\r\n");
	}
}

/*
 *	set sleep interval time
 */
void	DHI_SetSleepIntervalTime(int seconds)
{
	if ( pmuhandle<0 )
	{
		printf("\r\npmu device is not opened, please check it!\r\n");
		return;
	}
	ioctl(pmuhandle,SET_SLEEP_INTERVAL,seconds);
	printf("\r\nSet sleep interval ok.\r\n");
	return;
}

/*
 *	power off the phone
 */
void PowerOff()
{

	if ( pmuhandle<0 )
		printf("\r\n!!!can't power off the phone, please check pmu device!!!\r\n");

	printf("\r\nphone is shutdown.\r\n");
	ioctl(pmuhandle,PM_CPU_OFF);
}

/*
 *
 */
BOOL	f_ChangePowerMode ( BOOL bLimit )
{
#ifdef PM_IOC_SUSPEND
	if ( bLimit )
	{
		int nTmp;

		if ( pmuhandle < 0 )
			return	FALSE;

		nTmp = 3;
		nTmp = ioctl(pmuhandle, PM_IOC_SUSPEND, &nTmp );

		return TRUE;
	}
#endif
	return	FALSE;
}
/*
 * Return value:    1:alarm power on 	2:usb power on  3:charger power on
 */
int DHI_ReadPowerOnStatus(void)
{
	unsigned long arg = 0;
	
	if( pmuhandle < 0)
	{
		PLXPrintf("\r\npmu device is not be opened, please check it.\r\n");
		return -1;
	}

	ioctl(pmuhandle, IPMC_IOCTL_GET_POWERON_REASON, &arg);

	printf("\r\n ReadPowerOnStatus is %x", arg);
	
	if(arg & 1)		    
	{
		PLXPrintf("power key on\r\n");
		return 0;
	}
	if(arg & (1<<1))
	{
		PLXPrintf("alarm on\r\n");
		return 1;
	}
	if(arg & (1<<2))
	{
		PLXPrintf("usb on\r\n");
		return 2;
	}
	if(arg & (1<<3))
	{
		PLXPrintf("charge detected\r\n");
		return 3;
	}

	return -1;
}
static BOOL bEnableKeyRing;
BOOL IfEnableKeyRing(void)
{
	return bEnableKeyRing;
}
void f_EnableKeyRing(BOOL bEnable)
{
	bEnableKeyRing = bEnable;
}
/********************************************************************
* Function   ReadResetStatus  
* Purpose    
* Params     
* Return     1:normal  ; 0:reset
* Remarks      
**********************************************************************/
int  DHI_ReadResetStatus(void)
{
	unsigned long arg = 0;

	if( pmuhandle < 0 )
	{
		PLXPrintf("\r\npmu device is not opened , please check it.\r\n");
		return -1;
	}

	ioctl(pmuhandle, IPMC_IOCTL_NORMAL_POWEROFF, &arg);


	if(arg)
	{
		PLXPrintf("normal power on\r\n");
		return 1;
	}
	else
	{
		PLXPrintf("unnormal power off \r\n");
		return 0;
	}
}
/*
 * Return     0: no headset; 1:have headset
 */
int  DHI_ReadHeadSetStatus(void)
{
	unsigned long arg = 0;
	
	if( pmuhandle < 0 )
	{
		PLXPrintf("\r\npmu device is not opened , please check it.\r\n");
		return -1;
	}

	ioctl(pmuhandle, IPMC_IOCTL_GET_ERPSTATUS, &arg);
	
	if(arg)
	{
		PLXPrintf("arphone detected\r\n");
		return 1;
	}
	else
	{
		PLXPrintf("arphone undetected \r\n");
		return 0;
	}

}
/*
 *	get the reason of wakeup
 */
int  DHI_ReadWeekupStatus(int * value)
{
	unsigned long arg = 0;
	
	if(pmuhandle < 0)
	{
		PLXPrintf("\r\npmu device is not opened , please check it.\r\n");
		return -1;
	}

	ioctl(pmuhandle, IPMC_IOCTL_GET_WAKEUP_REASON, &arg);

	*value = arg;

	return 1;
}

/*
 *	prevent cpu goes in to sleep by some reason
 */
BOOL  f_sleep_register(int handle )
{
	int i = 0;

	if(handle < 0 || handle > PES_SLEEP_MAX)
		return FALSE;

	if(pmuhandle < 0)
	{
		PLXPrintf("\r\npmu device is not opened , please check it.\r\n");
		return FALSE;
	}	
	ioctl(pmuhandle, IPMC_IOCTL_SETFLAG, handle); 

	return TRUE;	
}
/*
 *	clear sleep flag
 */
BOOL f_sleep_unregister(int handle)
{
	int i = 0;

	if(handle == -1 || handle >= PES_SLEEP_MAX)
		return FALSE;

	if(pmuhandle < 0)
	{
		PLXPrintf("\r\npmu device is not opened , please check it.\r\n");
		return FALSE;
	}	
	ioctl(pmuhandle, IPMC_IOCTL_CLRANFLAG, handle); 

	PLXPrintf("\r\n unregister handle = %d", handle);

	return TRUE;
}

/*
 *	set keypad light
 */
BOOL DHI_SetKeylight(BOOL bOpen)
{
	int ioret = -1;
	
	if(pmuhandle < 0)
	{
		PLXPrintf("\r\npmu device is not opened , please check it.\r\n");
		return FALSE;
	}
	if(bOpen)
		ioret = ioctl(pmuhandle,IPMC_IOCTL_SET_KEYBL, 1);
	else
		ioret = ioctl(pmuhandle,IPMC_IOCTL_SET_KEYBL, 0);
	
    if (-1 == ioret)
    {
		printf("keylight ioctl error\r\n");	
        return FALSE;
    }
    
	return TRUE;
}

/*
 *	vibrate phone
 */
int DHI_Vibration(BOOL bOn)
{
	if ( pmuhandle<0 )
	{
		PLXPrintf("\r\npmu device is not opened , please check it.\r\n");
		return -1;
	}
	ioctl(pmuhandle,IPMC_IOCTL_VIBRATORON,bOn);
	return 0;
}

/*
 *	turn on/off led
 */
void f_SetLEDLight(BOOL bEnable)
{
	if(pmuhandle < 0)
	{
		PLXPrintf("\r\npmu device is not opened , please check it.\r\n");
		return;
	}
	if(bEnable)
		ioctl(pmuhandle, IPMC_IOCTL_SET_INFLED, 1);//open LED
	else
		ioctl(pmuhandle, IPMC_IOCTL_SET_INFLED, 0);//close LED
}
void DHI_SetLEDLight(BOOL bEnable)
{
	f_SetLEDLight(bEnable);
}

/*******************************************************************
 Mixer device DHI interface
 ******************************************************************/
/*
 *	open mixer device
 */
BOOL DHI_OpenMixer(void)
{
	int fd;

	fd = open("/dev/mixer",O_WRONLY);
	if ( fd<0 )
	{
		PLXPrintf("\r\nopen mixer failed!\r\n");
		return FALSE;
	}
	mixerhandle = fd;
	return TRUE;
}

/*
 *	close mixer device
 */
void DHI_CloseMixer(void)
{
	if ( mixerhandle<0 )
		printf("\r\nmixer device is already closed.\r\n");
	else
		printf("\r\nmixer device should not be closed!\r\n");
}

/*
 * get the value of current volume
 */
BOOL DHI_GetRingVolume(unsigned long * ivalue)
{
	unsigned long volume = 0;

	if( mixerhandle < 0)
	{
		PLXPrintf("\r\nmixer device is not opened, please check it.\r\n");
		return FALSE;
	}
	ioctl(mixerhandle, SOUND_MIXER_READ_VOLUME, &volume); 
	PLXPrintf("\r\ncurrent volume = %d\r\n",volume);

	*ivalue = volume;
	return TRUE;
}
/*
 * set a new volume value to device
 */
BOOL DHI_SetRingVolume(int ivalue)
{
	if( mixerhandle < 0)
	{
		PLXPrintf("\r\nmixer device is not opened, please check it.\r\n");
		return FALSE;
	}
	ioctl(mixerhandle, SOUND_MIXER_WRITE_VOLUME, ivalue); 

	PLXPrintf("\r\ncurrent volume = %d\r\n",ivalue);
	return TRUE;
}
/*
 *	ajust volume when hand free mode
 */
void AdjustVolumeForHandFree(BOOL bHandFree)
{
	int iAdjust = 0;

	if(bHandFree)
	{
		iAdjust = LEVEL_FIVE;
	}
	else
	{
       iAdjust = DEFAULT_LEV;
	}

	DHI_SetRingVolume(iAdjust);
}

/********************************************************************
* Function   GetHeadSetStatus()  
* Purpose    
* Params     
* Return     
* Remarks      
**********************************************************************/
static BOOL bHeadsetAttach;
BOOL GetHeadSetStatus(void)
{
	return bHeadsetAttach;
}
void SetHeadSetStatus(BOOL bAttach)
{
	bHeadsetAttach = bAttach;
}
/*******************************************************************
 Sound device DHI interface
 ******************************************************************/
static	int soundfd;
/*
 *	DHI_OpenAudioRoute and DHI_CloseAudioRoute should be a pair
 */
BOOL DHI_OpenAudioRoute(void)
{
	soundfd = open("/dev/sound",O_RDWR);
	if ( soundfd<0 )
	{
		printf("\r\nOpenAudioRoute error: can't open sound device.\r\n");
		return FALSE;
	}

	if ( GetHeadSetStatus() )
	{
		printf("\r\nAudio route is set to external speaker.\r\n");
		ioctl(soundfd,SNDCTL_SET_EXTERNAL_CALL);
	}
	else
	{
		printf("\r\nAudio route is set to internal speaker.\r\n");
		ioctl(soundfd,SNDCTL_SET_INTERNAL_CALL);
	}
	return TRUE;
}
void DHI_CloseAudioRoute(void)
{
	if ( soundfd>0 )
	{
		close(soundfd);
		soundfd = -1;
	}
	else
		printf("\r\nCloseAudioRoute error: sound device is not opened.\r\n");
}
/*
 *	called after sound device is opened then earphone is connected
 */
BOOL DHI_ChangeAudioRoute(void)
{
	if ( soundfd<0 )
	{
		printf("\r\nChangeAudioRoute error: sound device is not opened.\r\n");
		return FALSE;
	}
	if ( GetHeadSetStatus() )
	{
		printf("\r\nChange audio route to external speaker.\r\n");
		ioctl(soundfd,SNDCTL_SET_EXTERNAL_CALL);
	}
	else
	{
		printf("\r\nChange audio route to internal speaker.\r\n");
		ioctl(soundfd,SNDCTL_SET_INTERNAL_CALL);
	}
	return TRUE;
}

/*******************************************************************
 LCD device DHI interface
 ******************************************************************/
/*
 * get backlight volume
 */
BOOL DHI_GetBkLightVolume(int * ivalue)
{
	int getlevel;
	int ret;
	int fd;

	fd = open("/dev/lcd",O_RDWR);	
	if(fd < 0)
	{
		PLXPrintf("open lcd error\r\n");	
		return FALSE;
	}
	ret = ioctl(fd,LCD_IOC_GETBKLIGHT, &getlevel );		
	close(fd);

	if(ret == -1)
	{
		PLXPrintf("\r\nget backlight error!\r\n");
		return FALSE;
	}
	*ivalue = getlevel;

	return TRUE;
}

/*
 *	set backlight volume
 */
BOOL DHI_SetBkLightVolume(LEVELVALUE level)
{
	int ioret;
	int fd;

	fd = open("/dev/lcd",O_RDWR);
	if ( fd<0 )
	{
		PLXPrintf("\r\nopen lcd error!\r\n");
		return FALSE;
	}

	ioret = ioctl(fd,LCD_IOC_SETBKLIGHT, level);
	close(fd);

	if (-1 == ioret)
    {
		printf("\r\nLCD ioctl error!\r\n");	
        return FALSE;
    }
    
	return TRUE;
}

/*******************************************************************
 Status of charger
 ******************************************************************/
/*
 *	get the status of charger
 */
BOOL DHI_CheckChargerStatus(void)
{
	return bPlugCharger;
}

/*
 *	recorde status of charger, now we get the status of charger by message
 */
void DHI_SetChargerStatus(BOOL bPlugin)
{
	bPlugCharger = bPlugin;
}

/*******************************************************************
 Application interface for sleep enable , not device interface
 refer to f_sleep_register and f_sleep_unregister for device sleep interface
 ******************************************************************/
/*
 *	initialize sleep flag
 */
void InitAutoPowerOff()
{
	int i;
	for(i = 0; i < PES_SLEEP_MAX; i++)
		g_nAutoPoweroff[i] = 0;
}

BOOL f_DisablePowerOff(int handle)
{
		
	if(handle < 0 || handle > PES_SLEEP_MAX)
		return FALSE;

	g_nAutoPoweroff[handle] = 1;

	return TRUE;
}
BOOL f_EnablePowerOff(int handle)
{

	if(handle < 0 || handle >= PES_SLEEP_MAX)
		return FALSE;

	g_nAutoPoweroff[handle] = 0;

	if(IfInAutoPowerOff())
	{
		printf("\r\n unregister power off! ");
		DlmNotify(PMM_NEWS_ENABLE, PMF_AUTO_SHUT_DOWN);
		DlmNotify(PMM_NEWS_SHUT_DOWN, (LPARAM)0);
	}

	return TRUE;
}
BOOL IfPoweroffEnable(void)
{
	int i = 0;

	for(i = 0; i < PES_SLEEP_MAX; i++)
	{
		if(g_nAutoPoweroff[i] == 1)
			return FALSE;
	}

	return TRUE;
}

/*
 *	turn on/off keypad light according to parameter and user's settings
 */
void f_SetKeyLight(BOOL bOn)
{
	int value = 0;

	if(!bOn)
	{
		if(GetKeypadLitMode() == LS_KEEP_ON)
		{
			// always turn on keypad light when user has set
			DHI_SetKeylight(TRUE);
			return;
		}
		printf("\r\nturn on keypad light.\r\n");
		DHI_SetKeylight(bOn);
		f_sleep_unregister(KEYLIGHT);	// enable sleep when bOn is FALSE
	}
	else
	{
		if(GetKeypadLitMode() == LS_KEEP_OFF)
		{
			// always turn off keypad light when user has set
			DHI_SetKeylight(FALSE);
			return;
		}
		printf("\r\nturn off keypad light.\r\n");
		DHI_SetKeylight(bOn);
		f_sleep_register(KEYLIGHT);		// disable sleep when bOn is TRUE
	}
}

/*
 *	turn on/off backlight and set level of backlight 
 *	according to parameter and user's settings
 */
BOOL IfSetConfirm(void);
BOOL IfKeyCode(WORD nKeyCode)
{
	long vkParam[] = 
    {
        VK_F9, VK_F2,VK_RETURN,VK_F5,VK_F3,VK_F4,VK_F1,
		VK_0, VK_1, VK_2, VK_3, VK_4, VK_5,VK_6, 
		VK_7, VK_8, VK_9,  VK_F7,VK_F8,
		VK_LEFT,VK_RIGHT, VK_UP, VK_DOWN
    };
	int i;

	for(i =0; i < sizeof(vkParam)/sizeof(long); i++)
	{
		if(nKeyCode == vkParam[i])
			return TRUE;
	}

	return FALSE;
}
void f_SetBkLight(BOOL bEnable)
{
	int value = 0;
	WORD	lockKeyCode;
	calliftype callstate = GetCallIf();

	if(bEnable == FALSE)
	{
		// turn off backlight

		// if user set always turn on backlight, just return
		if(GetDisplayBackLitMode() == LS_KEEP_ON)
			return;

		DHI_SetBkLightVolume(0);	// turn off backlight
		PLXPrintf("\r\n bklight enable sleep!\r\n");
		f_sleep_unregister(BKLIGHT);// enable backlight sleep

		// turn on led when it is off
		if(GetIndicatorLEDStat() == SWITCH_ON)
			f_SetLEDLight(TRUE);
	}
	else
	{
		// turn on backlight
		value = GetBrightness();
		printf("\r\n bklight value = %d\r\n", value);

		f_sleep_register(BKLIGHT);	// disable backlight sleep

		if(callstate == ringif )
		{
			printf("\r\n setbklight = %d\r\n", value);
			// set backlight as defined in setting
			DHI_SetBkLightVolume(value);
		}
		else
		{
			if(PM_GetkeyLockStatus() || PM_GetPhoneLockStatus())
			{
				// keypad is locked
				lockKeyCode = GetLockKeyCode();
				
				if( IfKeyCode(lockKeyCode))	
				{
					if(GetDisplayBackLitMode() == LS_KEEP_ON)
						DHI_SetBkLightVolume(value);

					if(IfSetConfirm() && lockKeyCode ==VK_RETURN)
						DHI_SetBkLightVolume(value);
					else
					{
						// other key is pressed
						printf("\r\n nkeycode = %d\r\n",lockKeyCode);
						printf("\r\n setbklight = %d \r\n", LEV_ONE);
						// set backlight to level 1
						DHI_SetBkLightVolume(LEV_ONE);
					}
				}
				else
				{
					// power key and unlock key is pressed
					printf("\r\n setbklight = %d\r\n", value);
					// set backlight as defined in setting
					DHI_SetBkLightVolume(value);
				}
			}
			else
				// keypad is not locked, set the backlight as defined in setting
				DHI_SetBkLightVolume(value);

		}
		// we turn off led when the backlight is on
		if(GetIndicatorLEDStat() == SWITCH_ON)
			f_SetLEDLight(FALSE);
	}
}

/*
 *	mount SD card
 */
BOOL CheckMMCCheck(void)
{
	int iRet = 0;

	iRet = mount("/dev/mmc1","/mnt/fat16","fatfs",0,NULL);
	
	if(iRet == 0)
	{
		PLXPrintf("\r\n<Mount SDCard Ok>\r\t\n");
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL DHI_ResumeMeStart(void)
{
	int fd;
	fd = open(PROG_FILE_NAME,O_RDONLY);
	if ( fd<0 )
	{
		// file is not exist , create it
		PLXPrintf("\r\n<pmstart.ini does not exit!>\r\t\n");
		return FALSE;
	}
	else
	{
		close(fd);
		remove(PROG_FILE_NAME);
		PLXPrintf("\r\n<pmstart.ini is deleted!>\r\t\n");
	}
	return TRUE;
}






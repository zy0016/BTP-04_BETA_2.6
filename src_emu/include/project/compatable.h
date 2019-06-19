
#ifndef _COMPATABLE_H_
#define _COMPATABLE_H_

typedef enum
{
    LEV_ZERO,
    LEV_ONE,
    LEV_TWO,
    LEV_THREE,
    LEV_FOUR,
    LEV_FIVE
}LEVELVALUE;

/*******************************************************************
 PMU device DHI interface
 ******************************************************************/
/*
 *	open and close pmu device , should be called at startup
 */
extern BOOL	DHI_OpenPmu(void);
extern void DHI_ClosePmu(void);

/*
 *	reset cpu
 */
extern void	DHI_Reset(void);

/*
 *	shut down wireless module
 */
extern void	DHI_ShutDownWirelessModule(void);

/*
 *	IGT: ignite wireless module
 */
extern void	DHI_IgniteWirelessModule(void);

/*
 *	set sleep interval time
 */
extern void	DHI_SetSleepIntervalTime(int seconds);

/*
 *	get the reason of power on
 *  1:alarm power on 	2:usb power on  3:charger power on
 */
extern int DHI_ReadPowerOnStatus(void);

/*
 *	check if cpu is reset
 *  1:normal  ; 0:reset
 */
extern int  DHI_ReadResetStatus(void);

/*
 *	0: no headset(earphone) ; 1:have headset(earphone)
 */
extern int  DHI_ReadHeadSetStatus(void);

/*
 *	value:      1: rtc; 2:keydown 3 :charger
 */
extern int  DHI_ReadWeekupStatus(int * value);

/*
 *	Vibration interface
 *	bOn = FALSE : turn off vibration
 *	bOn = TRUE  : turn on  vibration
 */
extern int DHI_Vibration(BOOL bOn);

/*
 *	bEnable = TRUE  : turn on  led
 *	bEnable = FALSE : turn off led
 */
extern void DHI_SetLEDLight(BOOL bEnable);


/*******************************************************************
 Mixer device DHI interface
 ******************************************************************/
/*
 *	open and close mixer device
 */
extern BOOL DHI_OpenMixer(void);
extern void DHI_CloseMixer(void);

/*
 *	set and get ring volume
 */
BOOL DHI_GetRingVolume(unsigned long * ivalue);
BOOL DHI_SetRingVolume(int ivalue);

/*******************************************************************
 LCD device DHI interface
 ******************************************************************/
extern BOOL DHI_GetBkLightVolume(int * ivalue);
extern BOOL DHI_SetBkLightVolume(LEVELVALUE level);

/*******************************************************************
 Status of charger
 ******************************************************************/
extern BOOL DHI_CheckChargerStatus(void);
extern void DHI_SetChargerStatus(BOOL bPlugin);

extern BOOL DHI_ResumeMeStart(void);

#endif

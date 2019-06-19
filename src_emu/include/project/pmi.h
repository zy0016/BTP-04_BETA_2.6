/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : $ version 3.0
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef HAVE_PMI_H
#define HAVE_PMI_H

#include "plx_pdaex.h"
#ifdef __cplusplus
extern "C" {
#endif
//	TYPE DEFINE HERE
typedef struct tagAlarmTime
{
    WORD wYear;
    WORD wMonth;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;

}   ALARMTIME, *PALARMTIME;
//	MACRO DEFINE HERE
//	OUTPUT FUNCTION DECLARE
BOOL    DHI_SetRTC(PSYSTEMTIME pSystemTime);
BOOL    DHI_SetRTC_(PSYSTEMTIME pSystemTime);
BOOL    DHI_GetRTC(PSYSTEMTIME pSystemTime);
BOOL    DHI_GetRTC_(PSYSTEMTIME pSystemTime);
BOOL    DHI_CheckAlarm(void);
BOOL    DHI_SetAlarm(HWND hWnd, UINT uMsgCmd, PALARMTIME  pAlarmTime);

UINT	SetRTCTimer(HWND hWnd, UINT uIDEvent, DWORD uElapse, TIMERPROC pTimerFunc);
BOOL	KillRTCTimer(HWND hWnd, UINT uIDEvent);

int     DHI_OpenRTC();
void    DHI_CloseRTC();
BOOL    IsIdleState();// for mb to get idle state
BOOL	PM_GetkeyLockStatus(void);
BOOL	PM_GetPhoneLockStatus(void);
BOOL	AddDlmApp(PAPPGROUP pAppInfo);
void	SetPin1Status(BOOL bState);
BOOL    GetPin1Status(void);
void	KickDog(void);
void	EnableWatchDog(void);
void	DisableWatchDog(void);
BOOL	GetWatchDogState(void);
#ifdef __cplusplus
}
#endif

#endif //HAVE_PMI_H

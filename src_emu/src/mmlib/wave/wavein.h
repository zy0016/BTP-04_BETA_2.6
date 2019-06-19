/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : wavein.h
 *
 * Purpose  : Declare interface for program designer
 *
\**************************************************************************/

#ifndef HAVE_WAVEIN_H
#define HAVE_WAVEIN_H

/*********************************************************************\
*
* WAVEIN API PROCESS HEADER SECTION
*
**********************************************************************/

/**
** PROC HEADER
** PURPOSE	:	BASE TYPE DEFINE
*/
DECLARE_HANDLE(HWAVEIN);

typedef	HWAVEIN		FAR *LPHWAVEIN;

/**
** PROC HEADER
** PURPOSE	:	BASE MACRO DEFINE
*/
#define MM_WIM_OPEN     0x3BE
#define MM_WIM_CLOSE    0x3BF
#define MM_WIM_DATA     0x3C0

#define WIM_OPEN        MM_WIM_OPEN
#define WIM_CLOSE       MM_WIM_CLOSE
#define WIM_DATA        MM_WIM_DATA

/*
** PROC HEADER
** PURPOSE	:	WAVEFORM INPUT DEVICE CAPABILITIES STRUCTURE
*/
#define MAXPNAMELEN	32				// max product name length (including NULL)

typedef struct tagWAVEINCAPSA
{
    WORD    wMid;                    // manufacturer ID
    WORD    wPid;                    // product ID
    MMVERSION vDriverVersion;        // version of the driver
    CHAR    szPname[MAXPNAMELEN];    // product name (NULL terminated string)
    DWORD   dwFormats;               // formats supported
    WORD    wChannels;               // number of channels supported
    WORD    wReserved1;              // structure packing

} WAVEINCAPSA, *PWAVEINCAPSA, *NPWAVEINCAPSA, *LPWAVEINCAPSA;

typedef WAVEINCAPSA		WAVEINCAPS;
typedef PWAVEINCAPSA	PWAVEINCAPS;
typedef NPWAVEINCAPSA	NPWAVEINCAPS;
typedef LPWAVEINCAPSA	LPWAVEINCAPS;

/**
** PROC HEADER
** PURPOSE	:	WAVEFORM AUDIO INPUT FUNCTIONS DECLARE
*/
#ifdef __cplusplus
extern "C"{
#endif

WINMMAPI UINT WINAPI waveInGetNumDevs(void);
WINMMAPI WINAPI waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);

WINMMAPI MMRESULT WINAPI waveInOpen(LPHWAVEIN phwi, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);

WINMMAPI MMRESULT WINAPI waveInClose(HWAVEIN hwi);
WINMMAPI MMRESULT WINAPI waveInPrepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
WINMMAPI MMRESULT WINAPI waveInUnprepareHeader(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
WINMMAPI MMRESULT WINAPI waveInAddBuffer(HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh);
WINMMAPI MMRESULT WINAPI waveInStart(HWAVEIN hwi);
WINMMAPI MMRESULT WINAPI waveInStop(HWAVEIN hwi);
WINMMAPI MMRESULT WINAPI waveInReset(HWAVEIN hwi);
WINMMAPI MMRESULT WINAPI waveInGetPosition(HWAVEIN hwi, LPMMTIME pmmt, UINT cbmmt);
WINMMAPI MMRESULT WINAPI waveInGetID(HWAVEIN hwi, LPUINT puDeviceID);

WINMMAPI MMRESULT WINAPI waveInSetChannel(HWAVEIN hwi, BOOL bSet, DWORD nflags);

#ifdef __cplusplus
}
#endif

#endif // HAVE_WAVEIN_H

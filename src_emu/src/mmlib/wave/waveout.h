/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : waveout.h
 *
 * Purpose  : Declare interface for program designer
 *
\**************************************************************************/

#ifndef HAVE_WAVEOUT_H
#define HAVE_WAVEOUT_H

/*********************************************************************\
*
* WAVEOUT API PROCESS HEADER SECTION
*
**********************************************************************/

/*
** PROC HEADER
** PURPOSE	:	BASE TYPE DEFINE
*/
DECLARE_HANDLE(HWAVEOUT);

typedef	HWAVEOUT	FAR *LPHWAVEOUT;

/**
** PROC HEADER
** PURPOSE	:	WAVE CALLBACK MESSAGES 
*/
#define MM_WOM_OPEN         0x3BB           // waveform output
#define MM_WOM_CLOSE        0x3BC
#define MM_WOM_DONE         0x3BD
#define MM_WOM_PAUSE		0x390

#define WOM_OPEN			MM_WOM_OPEN		// waveform output
#define WOM_CLOSE			MM_WOM_CLOSE
#define WOM_DONE			MM_WOM_DONE
#define WOM_PAUSE			MM_WOM_PAUSE

/*
** PROC HEADER
** PURPOSE	:	WAVEFORM OUTPUT DEVICE CAPABILITIES STRUCTURE
*/
#define MAXPNAMELEN		32			// max product name length (including NULL)

typedef struct tagWAVEOUTCAPSA {
    WORD    wMid;                  // manufacturer ID
    WORD    wPid;                  // product ID
    MMVERSION vDriverVersion;      // version of the driver
    CHAR    szPname[MAXPNAMELEN];  // product name (NULL terminated string)
    DWORD   dwFormats;             // formats supported
    WORD    wChannels;             // number of sources supported
    WORD    wReserved1;            // packing
    DWORD   dwSupport;             // functionality supported by driver
} WAVEOUTCAPSA, *PWAVEOUTCAPSA, *NPWAVEOUTCAPSA, *LPWAVEOUTCAPSA;

typedef WAVEOUTCAPSA	WAVEOUTCAPS;
typedef PWAVEOUTCAPSA	PWAVEOUTCAPS;
typedef NPWAVEOUTCAPSA	NPWAVEOUTCAPS;
typedef LPWAVEOUTCAPSA	LPWAVEOUTCAPS;

/**
** PROC HEADER
** PURPOSE	:	WAVEFORM AUDIO OUT PUT FUNCTIONS DECLARE
*/
#ifdef __cplusplus
extern "C"{
#endif

WINMMAPI UINT WINAPI waveOutGetNumDevs(void);
WINMMAPI MMRESULT WINAPI waveOutGetDevCaps(UINT uDeviceID, LPWAVEOUTCAPS pwoc, UINT cbwoc);
WINMMAPI MMRESULT WINAPI waveOutGetVolume(HWAVEOUT hwo, LPDWORD pdwVolume);
WINMMAPI MMRESULT WINAPI waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);

WINMMAPI MMRESULT WINAPI waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
    LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);

WINMMAPI MMRESULT WINAPI waveOutClose(HWAVEOUT hwo);
WINMMAPI MMRESULT WINAPI waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
WINMMAPI MMRESULT WINAPI waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
WINMMAPI MMRESULT WINAPI waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
WINMMAPI MMRESULT WINAPI waveOutPause(HWAVEOUT hwo);
WINMMAPI MMRESULT WINAPI waveOutRestart(HWAVEOUT hwo);
WINMMAPI MMRESULT WINAPI waveOutReset(HWAVEOUT hwo);
WINMMAPI MMRESULT WINAPI waveOutBreakLoop(HWAVEOUT hwo);
WINMMAPI MMRESULT WINAPI waveOutGetPosition(HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt);
WINMMAPI MMRESULT WINAPI waveOutGetPitch(HWAVEOUT hwo, LPDWORD pdwPitch);
WINMMAPI MMRESULT WINAPI waveOutSetPitch(HWAVEOUT hwo, DWORD dwPitch);
WINMMAPI MMRESULT WINAPI waveOutGetPlaybackRate(HWAVEOUT hwo, LPDWORD pdwRate);
WINMMAPI MMRESULT WINAPI waveOutSetPlaybackRate(HWAVEOUT hwo, DWORD dwRate);
WINMMAPI MMRESULT WINAPI waveOutGetID(HWAVEOUT hwo, LPUINT puDeviceID);

WINMMAPI MMRESULT WINAPI waveOutSetChannel(HWAVEOUT hwo, BOOL bSet, DWORD nflags);

#ifdef __cplusplus
}
#endif

#endif // HAVE_WAVEOUT_H

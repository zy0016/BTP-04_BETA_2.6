/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : plxmm.h
 *
 * Purpose  : Declare function interface for applications
 *
\**************************************************************************/

#ifndef __PLXMM_H
#define __PLXMM_H

/*********************************************************************\
*
* DECODER AND ENCODER PROCESS HEADER SECTION
*
**********************************************************************/

#define DECODER_TYPE_MP3 0
#define DECODER_TYPE_AMR 1

#define ENCODER_TYPE_MP3 0
#define ENCODER_TYPE_AMR 1

#define DECODER_ATTRIB_RANDOM 0x0001

typedef void *HDECODER;
typedef void *HENCODER;

typedef struct tagMMDataSource
{
    void*   (*mm_malloc)(unsigned int size);
    void    (*mm_free)(void *memblock);
    void*   (*mm_realloc)(void *memblock, unsigned int size);
    void*   (*mm_memset)(void *dest, int c, unsigned int count);
    void*   (*mm_memcpy)(void *dest, const void *src, unsigned int count);
    void*   (*mm_memmove)(void *dest, const void *src, unsigned int count);

    int     (*mm_read)(unsigned char *buffer, int size, int offset, void *pParam);
    int     (*mm_write)(unsigned char *buffer, int size, void *pParam);
} MMDataSource, *PMMDataSource;

typedef struct tagAudioInfo
{
    int nChannel;
    int nSampleBits;
    int nSampleRate;
} AUDIOINFO, *PAUDIOINFO;

#ifdef __cplusplus
extern "C"
{
#endif

// decoder interface
HDECODER decoderCreate(int type, int random, PMMDataSource pSource,
                       int nSourceSize, int nPreferBufSize,
                       void *pParam);

int decoderDestroy(HDECODER hDecoder);

int decoderSeek(HDECODER hDecoder, int nSeconds);

int decoderGetTotalTime(HDECODER hDecoder);

int decoderGetData(HDECODER hDecoder, unsigned char *pBuf, int nReqSize);

int decoderGetAudioInfo(HDECODER hDecoder, PAUDIOINFO pInfo);

// encoder interface
#define ENCODEBEGIN     0x0001
#define ENCODEPROGRESS  0x0002
#define ENCODEEND       0x0004

HENCODER encoderCreate(int type, PMMDataSource pSource,
                       int nPreferBufSize, void *pParam);
int encoderDestroy(HENCODER hEncoder);
int encoderSetData(HENCODER hEncoder, unsigned char *pData,
                       int nDataSize, int flag);

#ifdef __cplusplus
}
#endif

/*********************************************************************\
*
* WAVE API PROCESS HEADER SECTION
*
**********************************************************************/

#include "hpdef.h"

/*
**	PROC HEADER
**	PURPOSE	:	WAVEFORM AUDIO DATA TYPES DEFINE
*/
#ifndef	CALLBACK
#define	CALLBACK
#endif

#ifndef	WINAPI
#define	WINAPI
#endif

#ifndef	WINMMAPI
#define	WINMMAPI
#endif

typedef char            CHAR;
typedef unsigned short  WCHAR;
typedef UINT            MMVERSION;  // major (high byte), minor (low byte)
typedef UINT            MMRESULT;   // error return code, 0 means no error

typedef	void (CALLBACK DRVCALLBACK)(HANDLE hdrvr, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
typedef	DRVCALLBACK		FAR *LPDRVCALLBACK;
typedef	DRVCALLBACK		WAVECALLBACK;
typedef	WAVECALLBACK	FAR *LPWAVECALLBACK;

/*
** PROC HEADER
** PURPOSE	:	DEVICE ID FOR WAVE DEVICE MAPPER
*/
#define WAVE_MAPPER     ((UINT)-1)

/*
** PROC HEADER
** PURPOSE	:	FLAGS FOR WFORMATTAG FIELD OF WAVEFORMAT
*/
#define WAVE_FORMAT_PCM     1

#define	WAVE_CHANNEL_EARPHONE	0x0001
#define WAVE_CHANNEL_ONLINE		0x0002
#define WAVE_CHANNEL_MASK		(WAVE_CHANNEL_EARPHONE | WAVE_CHANNEL_ONLINE)

/*
** PROC HEADER
** PURPOSE	:	FLAGS FOR DWSUPPORT FIELD OF WAVEOUTCAPS
*/
#define WAVECAPS_PITCH          0x0001   // supports pitch control
#define WAVECAPS_PLAYBACKRATE   0x0002   // supports playback rate control
#define WAVECAPS_VOLUME         0x0004   // supports volume control
#define WAVECAPS_LRVOLUME       0x0008   // separate left-right volume control
#define WAVECAPS_SYNC           0x0010
#define WAVECAPS_SAMPLEACCURATE 0x0020
#define WAVECAPS_DIRECTSOUND    0x0040

/*
** PROC HEADER
** PURPOSE	:	DEFINES FOR DWFORMAT FIELD OF WAVEINCAPS AND WAVEOUTCAPS
*/
#define WAVE_INVALIDFORMAT     0x00000000       //invalid format
#define WAVE_FORMAT_1M08       0x00000001       //11.025 kHz, Mono,   8-bit
#define WAVE_FORMAT_1S08       0x00000002       //11.025 kHz, Stereo, 8-bit
#define WAVE_FORMAT_1M16       0x00000004       //11.025 kHz, Mono,   16-bit
#define WAVE_FORMAT_1S16       0x00000008       //11.025 kHz, Stereo, 16-bit
#define WAVE_FORMAT_2M08       0x00000010       //22.05  kHz, Mono,   8-bit
#define WAVE_FORMAT_2S08       0x00000020       //22.05  kHz, Stereo, 8-bit
#define WAVE_FORMAT_2M16       0x00000040       //22.05  kHz, Mono,   16-bit
#define WAVE_FORMAT_2S16       0x00000080       //22.05  kHz, Stereo, 16-bit
#define WAVE_FORMAT_4M08       0x00000100       //44.1   kHz, Mono,   8-bit
#define WAVE_FORMAT_4S08       0x00000200       //44.1   kHz, Stereo, 8-bit
#define WAVE_FORMAT_4M16       0x00000400       //44.1   kHz, Mono,   16-bit
#define WAVE_FORMAT_4S16       0x00000800       //44.1   kHz, Stereo, 16-bit

/*
** PROC HEADER
** PURPOSE	:	DRIVER CALLBACK SUPPORT
** REMARK	:	FLAGS USED WITH WAVEOUTOPEN(), WAVEINOPEN(), MIDIINOPEN(), AND
**				MIDIOUTOPEN() TO SPECIFY THE TYPE OF THE DWCALLBACK PARAMETER.
*/
#define	CALLBACK_TYPEMASK      0x00070000l    //callback type mask
#define	CALLBACK_NULL          0x00000000l    //no callback
#define	CALLBACK_WINDOW        0x00010000l    //dwCallback is a HWND
#define	CALLBACK_TASK          0x00020000l    //dwCallback is a HTASK
#define	CALLBACK_FUNCTION      0x00030000l    //dwCallback is a FARPROC

/*
** PROC HEADER
** PURPOSE	:	MULTI OPENED HANDLE AND PRIORITY SUPPORT
** REMARK	:	CURRENTLY DIVIDED INTO 6 LEVELS
*/
#define WAVEOUT_PRIO_SHIFT     24
#define WAVEOUT_PRIO_MASK      0x0f000000l

#define WAVEOUT_PRIO_LOW            0x01000000l
#define WAVEOUT_PRIO_BELOWNORMAL    0x02000000l
#define WAVEOUT_PRIO_NORMAL         0x03000000l
#define WAVEOUT_PRIO_ABOVENORMAL    0x04000000l
#define WAVEOUT_PRIO_BELOWHIGH		0x05000000l
#define WAVEOUT_PRIO_HIGH           0x06000000l
#define WAVEOUT_PRIO_LOWEST         0x00000000l
#define WAVEOUT_PRIO_HIGHEST        0x0f000000l

#define WAVEOUT_WAITING        0x80000000l

/*
** PROC HEADER
** PURPOSE	:	MMTIME DATA STRUCTURE
*/
typedef struct mmtime_tag
{
    UINT		wType;      // indicates the contents of the union

    union
    {
	DWORD       ms;         // milliseconds
	DWORD       sample;     // samples
	DWORD       cb;         // byte count
	DWORD       ticks;      // ticks in MIDI stream

	struct		// SMPTE
	{
	    BYTE    hour;       // hours
	    BYTE    min;        // minutes
	    BYTE    sec;        // seconds
	    BYTE    frame;      // frames
	    BYTE    fps;        // frames per second
	    BYTE    dummy;      // pad
#ifdef _WIN32
	    BYTE    pad[2];
#endif
	} smpte;

	struct		// MIDI
	{
	    DWORD songptrpos;   // song pointer position
	} midi;

	} u;

} MMTIME, *PMMTIME, NEAR *NPMMTIME, FAR *LPMMTIME;

/* types for wType field in MMTIME struct */
#define TIME_MS         0x0001  /* time in milliseconds */
#define TIME_SAMPLES    0x0002  /* number of wave samples */
#define TIME_BYTES      0x0004  /* current byte offset */
#define TIME_SMPTE      0x0008  /* SMPTE time */
#define TIME_MIDI       0x0010  /* MIDI time */
#define TIME_TICKS      0x0020  /* Ticks within MIDI stream */

/*
** PROC HEADER
** PURPOSE	:	EXTENDED WAVEFORM FORMAT STRUCTURE USED FOR ALL NON-PCM FORMATS.
**				THIS STRUCTURE IS COMMON TO ALL NON-PCM FORMATS.
*/
typedef struct tWAVEFORMATEX
{
    WORD        wFormatTag;         // format type
    WORD        nChannels;          // number of channels (i.e. mono, stereo...)
    DWORD       nSamplesPerSec;     // sample rate
    DWORD       nAvgBytesPerSec;    // for buffer estimation
    WORD        nBlockAlign;        // block size of data
    WORD        wBitsPerSample;     // number of bits per sample of mono data
    WORD        cbSize;             // the count in bytes of the size of

} WAVEFORMATEX, *PWAVEFORMATEX, NEAR *NPWAVEFORMATEX, FAR *LPWAVEFORMATEX;

typedef const WAVEFORMATEX FAR *LPCWAVEFORMATEX;

/*
** PROC HEADER
** PURPOSE	: WAVE DATA BLOCK HEADER
*/
typedef struct wavehdr_tag
{
    LPSTR       lpData;                 // pointer to locked data buffer
    DWORD       dwBufferLength;         // length of data buffer
    DWORD       dwBytesRecorded;        // used for input only
    DWORD       dwUser;                 // for client's use
    DWORD       dwFlags;                // assorted flags (see defines)
    DWORD       dwLoops;                // loop control counter
    struct wavehdr_tag FAR *lpNext;     // reserved for driver
    DWORD       reserved;               // reserved for driver

} WAVEHDR, *PWAVEHDR, NEAR *NPWAVEHDR, FAR *LPWAVEHDR;

/* 
** PROC HEADER
** PURPOSE	:	FLAGS FOR DWFLAGS FIELD OF WAVEHDR
*/
#define WHDR_DONE			0x00000001	// done bit
#define WHDR_PREPARED		0x00000002	// set if this header has been prepared
#define WHDR_BEGINLOOP		0x00000004	// loop start block
#define WHDR_ENDLOOP		0x00000008	// loop end block
#define WHDR_INQUEUE		0x00000010	// reserved for driver

/*
** PROC HEADER
** PURPOSE	:	FUNCTION RETURN VALUE DEFINE
** DESTRIBE	:	GENERAL ERROR RETURN VALUES
*/
#define MMSYSERR_BASE         0
#define MMSYSERR_NOERROR      0                    // no error
#define MMSYSERR_ERROR        (MMSYSERR_BASE + 1)  // unspecified error
#define MMSYSERR_BADDEVICEID  (MMSYSERR_BASE + 2)  // device ID out of range
#define MMSYSERR_NOTENABLED   (MMSYSERR_BASE + 3)  // driver failed enable
#define MMSYSERR_ALLOCATED    (MMSYSERR_BASE + 4)  // device already allocated
#define MMSYSERR_INVALHANDLE  (MMSYSERR_BASE + 5)  // device handle is invalid
#define MMSYSERR_NODRIVER     (MMSYSERR_BASE + 6)  // no device driver present
#define MMSYSERR_NOMEM        (MMSYSERR_BASE + 7)  // memory allocation error
#define MMSYSERR_NOTSUPPORTED (MMSYSERR_BASE + 8)  // function isn't supported
#define MMSYSERR_BADERRNUM    (MMSYSERR_BASE + 9)  // error value out of range
#define MMSYSERR_INVALFLAG    (MMSYSERR_BASE + 10) // invalid flag passed
#define MMSYSERR_INVALPARAM   (MMSYSERR_BASE + 11) // invalid parameter passed
#define MMSYSERR_HANDLEBUSY   (MMSYSERR_BASE + 12) // handle being used

/*
** PROC HEADER
** PURPOSE	:	SIMULTANEOUSLY ON ANOTHER THREAD (EG CALLBACK) 
*/
#define MMSYSERR_INVALIDALIAS (MMSYSERR_BASE + 13) // specified alias not found
#define MMSYSERR_BADDB        (MMSYSERR_BASE + 14) // bad registry database
#define MMSYSERR_KEYNOTFOUND  (MMSYSERR_BASE + 15) // registry key not found
#define MMSYSERR_READERROR    (MMSYSERR_BASE + 16) // registry read error
#define MMSYSERR_WRITEERROR   (MMSYSERR_BASE + 17) // registry write error
#define MMSYSERR_DELETEERROR  (MMSYSERR_BASE + 18) // registry delete error
#define MMSYSERR_VALNOTFOUND  (MMSYSERR_BASE + 19) // registry value not found
#define MMSYSERR_NODRIVERCB   (MMSYSERR_BASE + 20) // driver does not call DriverCallback
#define MMSYSERR_LASTERROR    (MMSYSERR_BASE + 20) // last error in range

/*
** PROC HEADER
** PURPOSE	:	CUSTOMIZED ERROR CODE
*/
#define MMSYSERR_LOWPRIORITY  (MMSYSERR_BASE + 500)/* refused for lower
                                                    * priority, usually
                                                    * due to confliction
                                                    * with a player with
                                                    * higher priority.
                                                    */
/*
** PROC HEADER
** PURPOSE	:	WAVEFORM AUDIO ERROR RETURN VALUES
*/
#define WAVERR_BASE            32
#define WAVERR_BADFORMAT      (WAVERR_BASE + 0)    // unsupported wave format
#define WAVERR_STILLPLAYING   (WAVERR_BASE + 1)    // still something playing
#define WAVERR_UNPREPARED     (WAVERR_BASE + 2)    // header not prepared
#define WAVERR_SYNC           (WAVERR_BASE + 3)    // device is synchronous
#define WAVERR_LASTERROR      (WAVERR_BASE + 3)    // last error in range

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
MMRESULT WINAPI waveInGetDevCaps(UINT uDeviceID, LPWAVEINCAPS pwic, UINT cbwic);

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

#endif // __PLXMM_H

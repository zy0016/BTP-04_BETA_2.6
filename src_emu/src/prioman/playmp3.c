/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : playmp3.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "plx_pdaex.h"
#include "string.h"
#include "stdio.h"
#include "malloc.h"

#include "prioman.h"
#include "plxmm.h"

#define	WAVE_BUF_NUM    4
#define	WAVE_BUF_SIZE   4096
#define	PERFERSIZE      2048

/////////////////////////////////////////////////////////////////
static FILE *pMP3File;
static HDECODER hDecoder;
static HWAVEOUT hWaveOut;
static WAVEHDR WaveHDR[WAVE_BUF_NUM];
static BYTE WaveBuf[WAVE_BUF_NUM][WAVE_BUF_SIZE];
static BOOL bPlayOver;
static int nAudPrio;

/////////////////////////////////////////////////////////////////
HDECODER mp3DecoderCreate(PMMDataSource pSource, int nSourceSize, int nPreferBufSize, void *pParam);
int mp3DecoderDestroy(HDECODER hDecoder);
int mp3DecoderGetTotalTime(HDECODER hDecoder);
int mp3DecoderCurrentTime(HDECODER hDecoder, int* pSeconds);
int mp3DecoderSeek(HDECODER hDecoder, int nSeconds);
int mp3DecoderGetData(HDECODER hDecoder, unsigned char *pBuf, int nReqSize);
int mp3DecoderGetAudioInfo(HDECODER hDecoder, PAUDIOINFO pInfo);

/////////////////////////////////////////////////////////////////
#ifdef _EMULATE_
void FAR __stdcall mp3DecoderCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
#else
void CALLBACK mp3DecoderCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
#endif // _EMULATE_

void *MP3_Malloc(unsigned int size);
void  MP3_Free(void *memblock);
void *MP3_Realloc(void *memblock, unsigned int size);
void *MP3_Memset(void *dest, int c, unsigned int count);
void *MP3_Memcpy(void *dest, const void *src, unsigned int count);
void *MP3_Memmove(void *dest, const void *src, unsigned int count);
int   MP3_Read(unsigned char *pBuf, int nSize, int nOffset, void *pParam);

/*********************************************************************
* Function	MP3DecodeBegin
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int MP3DecodeBegin(HWND hWnd, LPSTR pstrName, int nPriority)
{
	MMDataSource ds;
	AUDIOINFO ai;
	WAVEFORMATEX wfx;
	int nResult, i, size;

	ds.mm_malloc = MP3_Malloc;
	ds.mm_free = MP3_Free;
	ds.mm_read = MP3_Read;
	ds.mm_realloc = MP3_Realloc;
	ds.mm_memcpy = MP3_Memcpy;
	ds.mm_memmove = MP3_Memmove;
	ds.mm_memset = MP3_Memset;

	pMP3File = fopen(pstrName, "rb");
	if (pMP3File == NULL)
		return PRIOMAN_ERROR_OPENFAIL;

	fseek(pMP3File, 0, SEEK_END);
	size = ftell(pMP3File);

	hDecoder = mp3DecoderCreate(&ds, size, PERFERSIZE, pMP3File);
	if (hDecoder == NULL)
		return PRIOMAN_ERROR_DECORDER;

	nResult = mp3DecoderGetTotalTime(hDecoder);
	if (nResult <= 0)
		return PRIOMAN_ERROR_GETPLAYTIME;

	nResult = mp3DecoderGetAudioInfo(hDecoder, &ai);
	if (nResult < 0)
		return PRIOMAN_ERROR_GETAUDIOINFO;

	wfx.cbSize = 0;
	wfx.wFormatTag = WAVE_FORMAT_PCM;
	wfx.nChannels = ai.nChannel;
	wfx.wBitsPerSample = ai.nSampleBits;
	wfx.nSamplesPerSec = ai.nSampleRate;
	wfx.nBlockAlign = wfx.nChannels*wfx.wBitsPerSample/8;
	wfx.nAvgBytesPerSec = wfx.nSamplesPerSec*wfx.nBlockAlign;

	bPlayOver = FALSE;
	nAudPrio = nPriority;
	nResult = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD)mp3DecoderCallback, 0, CALLBACK_FUNCTION);
	if (nResult != MMSYSERR_NOERROR)
		return PRIOMAN_ERROR_WAVEOPEN;

	for (i=0; i<WAVE_BUF_NUM; i++)
	{
		memset(&WaveHDR[i], 0, sizeof(WAVEHDR));
		WaveHDR[i].lpData = (char *)WaveBuf[i];
		WaveHDR[i].dwBufferLength = WAVE_BUF_SIZE;
		WaveHDR[i].dwUser = (DWORD)hWnd;
		WaveHDR[i].dwFlags = WHDR_DONE;

		nResult = waveOutPrepareHeader(hWaveOut, &WaveHDR[i], sizeof(WAVEHDR));
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_PREPARE;
	}

	for (i=0; i<WAVE_BUF_NUM; i++)
	{
		nResult = mp3DecoderGetData(hDecoder, WaveBuf[i], WAVE_BUF_SIZE);
		if (nResult <= 0)
			return PRIOMAN_ERROR_GETDECDATA;
	}

	for (i=0; i<WAVE_BUF_NUM; i++)
	{
		nResult = waveOutWrite(hWaveOut, &WaveHDR[i], sizeof(WAVEHDR));
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_WRITEOUT;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	MP3DecodeEnd
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int MP3DecodeEnd(void)
{
	int nResult, i;

	if (pMP3File == NULL)
		return PRIOMAN_ERROR_SUCCESS;

	fclose(pMP3File);
	pMP3File = NULL;

	if (hWaveOut != NULL)
	{
		nResult = waveOutReset(hWaveOut);
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_RESET;

		for (i=0; i<WAVE_BUF_NUM; i++)
		{
			nResult = waveOutUnprepareHeader(hWaveOut, &WaveHDR[i], sizeof(WAVEHDR));
			if (nResult != MMSYSERR_NOERROR)
				return PRIOMAN_ERROR_PREPARE;
		}

		nResult = waveOutClose(hWaveOut);
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_WAVECLOSE;

		// !!!!!!
		hWaveOut = NULL;
	}

	if (hDecoder != NULL)
	{
		mp3DecoderDestroy(hDecoder);
		hDecoder= NULL;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	MP3ReadData
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void MP3ReadData(WPARAM wParam, LPARAM lParam)
{
	LPWAVEHDR pHDR;
	int nReadLen;

	pHDR = (LPWAVEHDR)lParam;
	if (pHDR == NULL)
		return;

	if (hDecoder == NULL)
		return;

	nReadLen = mp3DecoderGetData(hDecoder, (unsigned char *)pHDR->lpData, WAVE_BUF_SIZE);
	if (nReadLen <= 0)
	{
		bPlayOver = TRUE;
		SendMessage((HWND)pHDR->dwUser, PRIOMAN_MESSAGE_PLAYOVER, NULL, NULL);
		return;
	}

	PostMessage((HWND)pHDR->dwUser, PRIOMAN_MESSAGE_WRITEDATA, nAudPrio, (LPARAM)pHDR);
}

/*********************************************************************
* Function	MP3WriteData
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void MP3WriteData(WPARAM wParam, LPARAM lParam)
{
	LPWAVEHDR pHDR;
	int nWriteLen;

	pHDR = (LPWAVEHDR)lParam;
	if (pHDR == NULL)
		return;

	if (hWaveOut == NULL)
		return;

	nWriteLen = waveOutWrite(hWaveOut, pHDR, sizeof(WAVEHDR));
	if (nWriteLen != MMSYSERR_NOERROR)
		return;
}

/*********************************************************************
* Function	MP3Pause
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int MP3Pause(void)
{
	return waveOutPause(hWaveOut);
}

/*********************************************************************
* Function	MP3Restart
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int MP3Restart(void)
{
	return waveOutRestart(hWaveOut);
}

/*********************************************************************
* Function	MP3GetTotalTime
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int MP3GetTotalTime(void)
{
	if (hDecoder == NULL)
		return -1;

	return mp3DecoderGetTotalTime(hDecoder);
}

/*********************************************************************
* Function	MP3CurrentTime
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int MP3CurrentTime(int* pSeconds)
{
	if (hDecoder == NULL)
		return -1;

	return mp3DecoderCurrentTime(hDecoder, pSeconds);
}

/*********************************************************************
* Function	MP3Seek
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int MP3Seek(int nSeconds)
{
	if (hDecoder == NULL)
		return -1;

	return mp3DecoderSeek(hDecoder, nSeconds);
}

/*********************************************************************
* Function	MP3SetVolume
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int MP3SetVolume(int nVolume)
{
	DWORD dwVolume;
	WORD wLeft, wRight;

	if (nVolume<PRIOMAN_VOLUME_LOWLEVEL || nVolume>PRIOMAN_VOLUME_HIGHLEVEL)
		nVolume = PRIOMAN_VOLUME_DEFAULT;

	wLeft = 0xFFFF / (PRIOMAN_VOLUME_HIGHLEVEL - PRIOMAN_VOLUME_LOWLEVEL) * nVolume;
	wRight = wLeft;
	dwVolume = (wLeft << 16) + wRight;

	return waveOutSetVolume(hWaveOut, dwVolume);
}

/*********************************************************************
* Function	mp3DecoderCallback
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
#ifdef _EMULATE_
void FAR __stdcall mp3DecoderCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
#else
void CALLBACK mp3DecoderCallback(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
#endif // _EMULATE_
{
	LPWAVEHDR pHDR;

	if (uMsg == WOM_OPEN)
	{
		bPlayOver = FALSE;
		return;
	}

	if (uMsg == WOM_CLOSE)
		return;

	if (uMsg == WOM_DONE)
	{
		pHDR = (LPWAVEHDR)dwParam1;
		if (pHDR == NULL)
			return;

		if (!bPlayOver)
			PostMessage((HWND)pHDR->dwUser, PRIOMAN_MESSAGE_READDATA, nAudPrio, (LPARAM)pHDR);
	}
}

/*********************************************************************
* Function	MP3_Malloc
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *MP3_Malloc(unsigned int size)
{
	void *memblock;

	memblock = malloc(size);
	if (memblock == NULL)
		return NULL;

	return memblock;
}

/*********************************************************************
* Function	MP3_Free
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void MP3_Free(void *memblock)
{
	free(memblock);
}

/*********************************************************************
* Function	MP3_Realloc
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *MP3_Realloc(void *memblock, unsigned int size)
{
	return realloc(memblock, size);
}

/*********************************************************************
* Function	MP3_Memset
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *MP3_Memset(void *dest, int c, unsigned int count)
{
	return memset(dest, c, count);
}

/*********************************************************************
* Function	MP3_Memcpy
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *MP3_Memcpy(void *dest, const void *src, unsigned int count)
{
	return memcpy(dest, src, count);
}

/*********************************************************************
* Function	MP3_Memmove
* Purpose   
* Parameter	
* Return	void *
* Remarks	
**********************************************************************/
void *MP3_Memmove(void *dest, const void *src, unsigned int count)
{
	return memmove(dest, src, count);
}

/*********************************************************************
* Function	MP3_Read
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int MP3_Read(unsigned char *pBuf, int nSize, int nOffset, void *pParam)
{
	FILE* pFile;

	pFile = (FILE *)pParam;
	if (pFile == NULL)
		return -1;

	fseek(pFile, nOffset, SEEK_SET);

	return fread(pBuf, sizeof(unsigned char), nSize, pFile);
}

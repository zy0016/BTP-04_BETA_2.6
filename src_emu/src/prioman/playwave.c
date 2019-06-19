/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : playwave.c
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

#define KILOSIZE        1024
#define BUFNUM          3

/////////////////////////////////////////////////////////////////
static FILE *pWaveFile;
static HWAVEOUT hWaveOut;
static WAVEHDR WaveHDR[BUFNUM];
static unsigned char* WaveBuf[BUFNUM];
static int nMallocLen;
static BOOL bPlayData, bMMSDataEnd;
static unsigned char* pMMSDataBuf, * pFreeDataBuf;
static int nMMSDataLen;
static BOOL bPlayOver;
static int nAudPrio;

/////////////////////////////////////////////////////////////////
#ifdef _EMULATE_
void FAR __stdcall waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
#else
void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
#endif // _EMULATE_

/*********************************************************************
* Function	WaveDecodeBegin
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int WaveDecodeBegin(HWND hWnd, LPSTR pstrName, unsigned char* pDataBuf, int nDataLen, int nPriority)
{
	WAVEFORMATEX wfx;
	int nFileLen, nReadLen;
	int nResult, i;

	if (pstrName != NULL)
	{
		pWaveFile = fopen(pstrName, "rb");
		if (pWaveFile == NULL)
			return PRIOMAN_ERROR_OPENFAIL;

		// Music file
		bPlayData = FALSE;

		wfx.cbSize = 0;
		wfx.wFormatTag = WAVE_FORMAT_PCM;
		fseek(pWaveFile, 0x16, SEEK_SET);
		fread(&wfx.nChannels, 1, sizeof(WORD), pWaveFile);
		fread(&wfx.nSamplesPerSec, 1, sizeof(DWORD), pWaveFile);
		fseek(pWaveFile, 0x22, SEEK_SET);
		fread(&wfx.wBitsPerSample, 1, sizeof(WORD), pWaveFile);
		wfx.nBlockAlign = wfx.nChannels*wfx.wBitsPerSample/8;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec*wfx.nBlockAlign;

		if (wfx.wBitsPerSample!=8 && wfx.wBitsPerSample!=16)
			return PRIOMAN_ERROR_WAVEFORMAT;

		bPlayOver = FALSE;
		nAudPrio = nPriority;
		nResult = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD)waveOutProc, 0, CALLBACK_FUNCTION);
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_WAVEOPEN;

		fseek(pWaveFile, 0, SEEK_END);
		nFileLen = ftell(pWaveFile);
		if (nFileLen > 16*KILOSIZE)
			nMallocLen = 4*KILOSIZE;
		else if (nFileLen>4*KILOSIZE && nFileLen<=16*KILOSIZE)
			nMallocLen = KILOSIZE;
		else if (nFileLen>KILOSIZE && nFileLen<=4*KILOSIZE)
			nMallocLen = KILOSIZE/4;
		else
			nMallocLen = KILOSIZE/16;

		// 文件指针移动至数据区
		fseek(pWaveFile, 0x2C, SEEK_SET);
	}
	else
	{
		unsigned char* p;

		// MMS Data
		bPlayData = TRUE;
		bMMSDataEnd = FALSE;
		nMMSDataLen = nDataLen;

		pMMSDataBuf = (unsigned char*)malloc(nMMSDataLen+1);
		if (pMMSDataBuf == NULL)
			return PRIOMAN_ERROR_ALLOCMEM;
		memcpy(pMMSDataBuf, pDataBuf, nMMSDataLen);
		pFreeDataBuf = pMMSDataBuf;

		memset(&wfx, 0, sizeof(WAVEFORMATEX));
		wfx.cbSize = 0;
		wfx.wFormatTag = WAVE_FORMAT_PCM;

		p = pMMSDataBuf;
		p += 0x16;
		memcpy(&wfx.nChannels, p, sizeof(WORD));
		memcpy(&wfx.nSamplesPerSec, p+2, sizeof(DWORD));

		p = pMMSDataBuf;
		p += 0x22;
		memcpy(&wfx.wBitsPerSample, p, sizeof(WORD));

		wfx.nBlockAlign = wfx.nChannels*wfx.wBitsPerSample/8;
		wfx.nAvgBytesPerSec = wfx.nSamplesPerSec*wfx.nBlockAlign;

		if (wfx.wBitsPerSample!=8 && wfx.wBitsPerSample!=16)
			return PRIOMAN_ERROR_WAVEFORMAT;

		bPlayOver = FALSE;
		nAudPrio = nPriority;
		nResult = waveOutOpen(&hWaveOut, WAVE_MAPPER, &wfx, (DWORD)waveOutProc, 0, CALLBACK_FUNCTION);
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_WAVEOPEN;

		nFileLen = nMMSDataLen;
		if (nFileLen > 16*KILOSIZE)
			nMallocLen = 4*KILOSIZE;
		else if (nFileLen>4*KILOSIZE && nFileLen<=16*KILOSIZE)
			nMallocLen = KILOSIZE;
		else if (nFileLen>KILOSIZE && nFileLen<=4*KILOSIZE)
			nMallocLen = KILOSIZE/4;
		else
			nMallocLen = KILOSIZE/16;

		// 文件指针移动至数据区
		pMMSDataBuf += 0x2C;
		nMMSDataLen -= 0x2C;
	}

	for (i=0; i<BUFNUM; i++)
	{
		memset(&WaveHDR[i], 0, sizeof(WAVEHDR));
		WaveBuf[i] = (unsigned char*)malloc(nMallocLen);
		WaveHDR[i].lpData = (char *)WaveBuf[i];
		WaveHDR[i].dwBufferLength = nMallocLen;
		WaveHDR[i].dwUser = (DWORD)hWnd;
		WaveHDR[i].dwFlags = WHDR_DONE;

		nResult = waveOutPrepareHeader(hWaveOut, &WaveHDR[i], sizeof(WAVEHDR));
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_PREPARE;
	}

	for (i=0; i<BUFNUM; i++)
	{
		if (!bPlayData)
		{
			nReadLen = fread(WaveBuf[i], 1, nMallocLen, pWaveFile);
			if (nReadLen <= 0)
				return PRIOMAN_ERROR_GETDECDATA;
		}
		else
		{
			if (nMMSDataLen > nMallocLen)
			{
				nReadLen = nMallocLen;
				memcpy(WaveBuf[i], pMMSDataBuf, nReadLen);
				pMMSDataBuf += nMallocLen;
				nMMSDataLen -= nMallocLen;
			}
			else
			{
				nReadLen = nMMSDataLen;
				memcpy(WaveBuf[i], pMMSDataBuf, nReadLen);
				pMMSDataBuf = NULL;
				nMMSDataLen = 0;
			}
			if (nReadLen <= 0)
				return PRIOMAN_ERROR_GETDECDATA;
		}
	}

	for (i=0; i<BUFNUM; i++)
	{
		nResult = waveOutWrite(hWaveOut, &WaveHDR[i], sizeof(WAVEHDR));
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_WRITEOUT;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	WaveDecodeEnd
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int WaveDecodeEnd(void)
{
	int nResult, i;

	if (!bPlayData)
	{
		if (pWaveFile == NULL)
			return PRIOMAN_ERROR_SUCCESS;

		fclose(pWaveFile);
		pWaveFile = NULL;
	}
	else
	{
		if (bMMSDataEnd)
			return PRIOMAN_ERROR_SUCCESS;

		free(pFreeDataBuf);
		pFreeDataBuf = NULL;
		bMMSDataEnd = TRUE;
	}

	if (hWaveOut != NULL)
	{
		nResult = waveOutReset(hWaveOut);
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_RESET;

		for (i=0; i<BUFNUM; i++)
		{
			nResult = waveOutUnprepareHeader(hWaveOut, &WaveHDR[i], sizeof(WAVEHDR));
			if (nResult != MMSYSERR_NOERROR)
				return PRIOMAN_ERROR_PREPARE;

			if (WaveBuf[i] != NULL)
			{
				free(WaveBuf[i]);
				WaveBuf[i] = NULL;
			}
		}

		nResult = waveOutClose(hWaveOut);
		if (nResult != MMSYSERR_NOERROR)
			return PRIOMAN_ERROR_WAVECLOSE;

		// !!!!!!
		hWaveOut = NULL;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	WaveReadData
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void WaveReadData(WPARAM wParam, LPARAM lParam)
{
	LPWAVEHDR pHDR;
	int nReadLen;

	pHDR = (LPWAVEHDR)lParam;
	if (pHDR == NULL)
		return;

	if (!bPlayData)
	{
		if (pWaveFile == NULL)
			return;
	}
	else
	{
		if (bMMSDataEnd)
			return;
	}

	if (!bPlayData)
	{
		nReadLen = fread(pHDR->lpData, 1, nMallocLen, pWaveFile);
	}
	else
	{
		if (nMMSDataLen > nMallocLen)
		{
			nReadLen = nMallocLen;
			memcpy(pHDR->lpData, pMMSDataBuf, nReadLen);
			pMMSDataBuf += nMallocLen;
			nMMSDataLen -= nMallocLen;
		}
		else
		{
			nReadLen = nMMSDataLen;
			if (nReadLen > 0)
			memcpy(pHDR->lpData, pMMSDataBuf, nReadLen);
			pMMSDataBuf = NULL;
			nMMSDataLen = 0;
		}
	}

	if (nReadLen <= 0)
	{
		bPlayOver = TRUE;
		SendMessage((HWND)pHDR->dwUser, PRIOMAN_MESSAGE_PLAYOVER, NULL, NULL);
		return;
	}

	PostMessage((HWND)pHDR->dwUser, PRIOMAN_MESSAGE_WRITEDATA, nAudPrio, (LPARAM)pHDR);
}

/*********************************************************************
* Function	WaveWriteData
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void WaveWriteData(WPARAM wParam, LPARAM lParam)
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
* Function	waveOutProc
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
#ifdef _EMULATE_
void FAR __stdcall waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
#else
void CALLBACK waveOutProc(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
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

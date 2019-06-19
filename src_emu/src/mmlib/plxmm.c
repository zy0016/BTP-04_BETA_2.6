/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : mmlib.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "pthread.h"

#include "plxmm.h"
#include "sys/ipc.h"

typedef struct tagCALLBACKPARAMS
{
    HANDLE      hwo;
    UINT        uMsg;
    DWORD       dwInstance;
    DWORD       dwParam1;
    DWORD       dwParam2;
} CALLBACKPARAMS, *PCALLBACKPARAMS;

#define QUE_MAX_LEN		8

typedef struct tagWOMQUEUE
{
    CALLBACKPARAMS  aParams[QUE_MAX_LEN];
    BOOL            abInQueue[QUE_MAX_LEN];
    BYTE            iRead;
    BYTE            iWrite;
} WOMQUEUE, *PWOMQUEUE;

MMRESULT _EMU_waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
	LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen);
MMRESULT _EMU_waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume);
MMRESULT _EMU_waveOutClose(HWAVEOUT hwo);
MMRESULT _EMU_waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT _EMU_waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT _EMU_waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh);
MMRESULT _EMU_waveOutPause(HWAVEOUT hwo);
MMRESULT _EMU_waveOutRestart(HWAVEOUT hwo);
MMRESULT _EMU_waveOutReset(HWAVEOUT hwo);

HANDLE _EMU_CreateEvent();
BOOL _EMU_CloseHandle(HANDLE hHandle);
DWORD _EMU_WaitForSingleObject(HANDLE hHandle);
BOOL _EMU_ResetEvent(HANDLE hEvent);
BOOL _EMU_SetEvent(HANDLE hEvent);

static DWORD g_dwCallback;
static DWORD g_fdwOpen;
static DWORD g_fdwUsed;

static HANDLE g_hEvent;
static pthread_t g_pThread;
static WOMQUEUE g_womQue;

static int IsWomEmpty();
static void WomEnqueue(PCALLBACKPARAMS pParams);
static void WomDequeue();
static void WomDestroyQueue();

static void	FAR __stdcall waveOutProc(HWAVEOUT hwo, UINT uMsg,
    DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);
static void ThreadProc(void *pVoid);

MMRESULT waveOutOpen(LPHWAVEOUT phwo, UINT uDeviceID,
	LPCWAVEFORMATEX pwfx, DWORD dwCallback, DWORD dwInstance, DWORD fdwOpen)
{
	if (g_fdwUsed == 1)
		return MMSYSERR_ALLOCATED;

	g_dwCallback = dwCallback;
	g_fdwOpen = fdwOpen;
	g_fdwUsed = 1;

	g_hEvent = _EMU_CreateEvent();
	pthread_create(&g_pThread, 0, (void *)&ThreadProc, NULL);

	return _EMU_waveOutOpen(phwo, uDeviceID, pwfx, (DWORD)waveOutProc, dwInstance, fdwOpen);
}

MMRESULT waveOutSetVolume(HWAVEOUT hwo, DWORD dwVolume)
{
	return _EMU_waveOutSetVolume(hwo, dwVolume);
}

MMRESULT waveOutClose(HWAVEOUT hwo)
{
	MMRESULT mmresult;

	mmresult = _EMU_waveOutClose(hwo);

	pthread_join(g_pThread, NULL);
	_EMU_CloseHandle(g_hEvent);
	WomDestroyQueue();
	g_fdwUsed = 0;

	return mmresult;
}

MMRESULT waveOutPrepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	return _EMU_waveOutPrepareHeader(hwo, pwh, cbwh);
}

MMRESULT waveOutUnprepareHeader(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	return _EMU_waveOutUnprepareHeader(hwo, pwh, cbwh);
}

MMRESULT waveOutWrite(HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh)
{
	return _EMU_waveOutWrite(hwo, pwh, cbwh);
}

MMRESULT waveOutPause(HWAVEOUT hwo)
{
	return _EMU_waveOutPause(hwo);
}

MMRESULT waveOutRestart(HWAVEOUT hwo)
{
	return _EMU_waveOutRestart(hwo);
}

MMRESULT waveOutReset(HWAVEOUT hwo)
{
	return _EMU_waveOutReset(hwo);
}

MMRESULT waveOutSetChannel(HWAVEOUT hwo, BOOL bSet, DWORD nflags)
{
	return MMSYSERR_NOTSUPPORTED;
}

static void FAR __stdcall waveOutProc(HWAVEOUT hwo, UINT uMsg,
    DWORD dwInstance, DWORD dwParam1, DWORD dwParam2)
{
	CALLBACKPARAMS params;

	params.hwo          = (HANDLE)hwo;
	params.uMsg         = uMsg;
	params.dwInstance   = dwInstance;
	params.dwParam1     = dwParam1;
	params.dwParam2     = dwParam2;

	WomEnqueue(&params);
	_EMU_SetEvent(g_hEvent);
}

static int IsWomEmpty()
{
	if (g_womQue.abInQueue[g_womQue.iRead])
		return 0;

	return 1;
}

static void WomEnqueue(PCALLBACKPARAMS pParams)
{
	if (g_womQue.abInQueue[g_womQue.iWrite])
		return;

	g_womQue.aParams[g_womQue.iWrite].hwo        = pParams->hwo;
	g_womQue.aParams[g_womQue.iWrite].uMsg       = pParams->uMsg;
	g_womQue.aParams[g_womQue.iWrite].dwInstance = pParams->dwInstance;
	g_womQue.aParams[g_womQue.iWrite].dwParam1   = pParams->dwParam1;
	g_womQue.aParams[g_womQue.iWrite].dwParam2   = pParams->dwParam2;

	g_womQue.abInQueue[g_womQue.iWrite] = TRUE;
	g_womQue.iWrite = (g_womQue.iWrite < QUE_MAX_LEN - 1) ?
		g_womQue.iWrite + 1 : 0;
}

static void WomDequeue()
{
	g_womQue.abInQueue[g_womQue.iRead] = FALSE;
	g_womQue.iRead = (g_womQue.iRead < QUE_MAX_LEN - 1) ?
		g_womQue.iRead + 1 : 0;
}

static void WomDestroyQueue()
{
	int i;

	for (i = 0; i < QUE_MAX_LEN; i ++)
		g_womQue.abInQueue[i] = FALSE;
	g_womQue.iRead = g_womQue.iWrite = 0;
}

typedef void (FAR __stdcall * mystdcall)(HWAVEOUT hwo, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD dwParam2);

static void ThreadProc(void *pVoid)
{
	PCALLBACKPARAMS pParams;
	int i;

	for (i = 0; i < QUE_MAX_LEN; i ++)
		g_womQue.abInQueue[i] = FALSE;
	g_womQue.iRead = g_womQue.iWrite = 0;

	while (1)
	{
		if (IsWomEmpty())
		{
			_EMU_WaitForSingleObject(g_hEvent);
			continue;
		}

		pParams = &(g_womQue.aParams[g_womQue.iRead]);

		switch (g_fdwOpen)
		{
		case CALLBACK_FUNCTION:
			((mystdcall)g_dwCallback)(pParams->hwo,
				pParams->uMsg, pParams->dwInstance, pParams->dwParam1,
				pParams->dwParam2);
			break;

		case CALLBACK_WINDOW:
			PostMessage((HWND)g_dwCallback, pParams->uMsg,
				(WPARAM)pParams->hwo, (LPARAM)pParams->dwParam1);
			break;

		default:
			break;
		}

		WomDequeue();

		if (pParams->uMsg == WOM_CLOSE)
			break;
	}
}

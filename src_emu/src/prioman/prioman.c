/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : prioman.c
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "plx_pdaex.h"
#include "winpda.h"
#include "string.h"
#include "stdio.h"

#include "playwave.h"
#include "playmidi.h"
#include "playamr.h"
#include "playmp3.h"

#include "prioman.h"
#include "setup.h"
#include "MBPublic.h"
#include "PhonebookExt.h"

#include "sys/ipc.h"
#include "hopen/ipmc.h"
#include "hopen/soundcard.h"
#include "compatable.h"
#include "romdata.h"

#define MUSICNUM        3
#define CALLNUM         2

/////////////////////////////////////////////////////////////////
static HWND hPrioMan_HideWnd;
static int nPrioMan_Priority;
static SCENEMODE sPrioMan_Profile;
static ABINFO sPrioMan_ABInfo;
static PM_PlayMusic sPrioMan_PlayMusic;
static PM_PlayMusic sPrioMan_CallMusic;
static PM_MusicList sPrioMan_MusicList[MUSICNUM];
static PM_CallList sPrioMan_CallList[CALLNUM];
static PM_RAMFILE sPrioMan_RAMFILE;
static int hPrioMan_Event;
static int hPrioMan_Sem;

/////////////////////////////////////////////////////////////////
BOOL PrioMan_CreateHideWindow(void);
LRESULT CALLBACK PrioMan_HideWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static int  PrioMan_ReadWriteRAM(PPM_RAMFILE pRAM);
static int  PrioMan_ResetRAM(int nPrio);
static BOOL PrioMan_IsPlaying(int nPrio);

static int  PrioMan_GetPriority(void);
static void PrioMan_SetPriority(int nPrio);
static void PrioMan_ResetPriority(void);
static int  PrioMan_GetType(char* pMusicName, unsigned char* pDataBuf, int nDataBuf);
static int  PrioMan_GetExp(char* pMusicName);

static int  PrioMan_PlayWAVE(PPM_PlayMusic pPM);
static int  PrioMan_PlayMIDI(PPM_PlayMusic pPM);
static int  PrioMan_PlayMMF(PPM_PlayMusic pPM);
static int  PrioMan_PlayAMR(PPM_PlayMusic pPM);
static int  PrioMan_PlayMP3(PPM_PlayMusic pPM);

static void PrioMan_ReadCall(WPARAM wParam, LPARAM lParam);
static void PrioMan_WriteCall(WPARAM wParam, LPARAM lParam);

static int  PrioMan_EndPlayWAVE(int nPrio, BOOL bCall, BOOL bBreak);
static int  PrioMan_EndPlayMIDI(int nPrio, BOOL bCall, BOOL bBreak);
static int  PrioMan_EndPlayMMF(int nPrio, BOOL bCall, BOOL bBreak);
static int  PrioMan_EndPlayAMR(int nPrio, BOOL bCall, BOOL bBreak);
static int  PrioMan_EndPlayMP3(int nPrio, BOOL bCall, BOOL bBreak);

static int  PrioMan_PauseWAVE(void);
static int  PrioMan_PauseMIDI(void);
static int  PrioMan_PauseMMF(void);
static int  PrioMan_PauseAMR(void);
static int  PrioMan_PauseMP3(void);

static int  PrioMan_ResumeWAVE(void);
static int  PrioMan_ResumeMIDI(void);
static int  PrioMan_ResumeMMF(void);
static int  PrioMan_ResumeAMR(void);
static int  PrioMan_ResumeMP3(void);

/*********************************************************************
* Function	PrioMan_Initialize
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL PrioMan_Initialize(void)
{
	// �����¼����ź���
	hPrioMan_Event = CreateEvent(IPC_EVENT|0x9270, 0);
	hPrioMan_Sem = CreateSemaphore(IPC_SEM|0x8810, 1);

	// RAM���ݸ�λ
	sPrioMan_RAMFILE.hCallWnd = NULL;
	sPrioMan_RAMFILE.nPriority = PRIOMAN_PRIORITY_ZERO;
	nPrioMan_Priority = PRIOMAN_PRIORITY_ZERO;

	return PrioMan_CreateHideWindow();
}

/*********************************************************************
* Function	PrioMan_Uninitialize
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL PrioMan_Uninitialize(void)
{
	// �����¼����ź���
	close(hPrioMan_Event);
	close(hPrioMan_Sem);

	return TRUE;
}

/*********************************************************************
* Function	PrioMan_CreateHideWindow
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL PrioMan_CreateHideWindow(void)
{
	WNDCLASS wc;

	wc.style         = 0;
	wc.lpfnWndProc   = PrioMan_HideWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "PXMMWndClass";
	if (!RegisterClass(&wc))
		return FALSE;

	hPrioMan_HideWnd = CreateWindow("PXMMWndClass",
		"PXMM",
		0,
		0,0,0,0,
		NULL, NULL, NULL, NULL);
	if (hPrioMan_HideWnd == NULL)
		return FALSE;

	return TRUE;
}

/*********************************************************************
* Function	PrioMan_HideWndProc
* Purpose   
* Parameter	
* Return	LRESULT
* Remarks	
**********************************************************************/
LRESULT CALLBACK PrioMan_HideWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult;

	lResult = (LRESULT)TRUE;

	switch (message)
	{
	case WM_CREATE:
		break;

	case WM_DESTROY:
		UnregisterClass("PXMMWndClass", NULL);
		hPrioMan_HideWnd = NULL;
		break;

	case WM_PAINT:
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_TIMER:
		switch (wParam)
		{
		case PRIOMAN_TIMER_CALLMUSIC:
			// �رղ��Ŷ�ʱ��
			KillTimer(hWnd, PRIOMAN_TIMER_CALLMUSIC);

			// ��ʼ�µĲ���
			switch (sPrioMan_CallMusic.nRingType)
			{
			case PRIOMAN_RINGTYPE_WAVE:
				PrioMan_PlayWAVE(&sPrioMan_CallMusic);
				break;
			case PRIOMAN_RINGTYPE_MIDI:
				PrioMan_PlayMIDI(&sPrioMan_CallMusic);
				break;
			case PRIOMAN_RINGTYPE_MMF:
				PrioMan_PlayMMF(&sPrioMan_CallMusic);
				break;
			case PRIOMAN_RINGTYPE_AMR:
				PrioMan_PlayAMR(&sPrioMan_CallMusic);
				break;
			case PRIOMAN_RINGTYPE_MP3:
				PrioMan_PlayMP3(&sPrioMan_CallMusic);
				break;
			default:
				break;
			}
			break;
		case PRIOMAN_TIMER_ENDCALL:
			// �رճ�ʱ��ʱ��
			KillTimer(hWnd, PRIOMAN_TIMER_ENDCALL);

			// �������β���
			PrioMan_EndCallMusic(nPrioMan_Priority, FALSE);

			// ��λ���ȼ�
			PrioMan_ResetRAM(nPrioMan_Priority);
			break;
		case PRIOMAN_TIMER_VIBRATION:
			PrioMan_Vibration(0);
			break;
		default:
			break;
		}
		break;

	case PRIOMAN_MESSAGE_READDATA:
		// ��ȡ��������
		PrioMan_ReadCall(wParam, lParam);
		break;

	case PRIOMAN_MESSAGE_WRITEDATA:
		// д����������
		PrioMan_WriteCall(wParam, lParam);
		break;

	case PRIOMAN_MESSAGE_PLAYOVER:
		// �رճ�ʱ��ʱ��
		KillTimer(hWnd, PRIOMAN_TIMER_ENDCALL);

		// �������β���
		PrioMan_EndCallMusic(nPrioMan_Priority, FALSE);

		// �Ƿ���Ҫѭ�����ţ�
		if (sPrioMan_CallMusic.nRepeat == -1)
		{
			// ���ò��Ŷ�ʱ��
			SetTimer(hWnd, PRIOMAN_TIMER_CALLMUSIC, 100, NULL);
		}
		else if (sPrioMan_CallMusic.nRepeat > 0)
		{
			// ���ò��Ŷ�ʱ��
			sPrioMan_CallMusic.nRepeat--;
			SetTimer(hWnd, PRIOMAN_TIMER_CALLMUSIC, 100, NULL);
		}
		else
		{
			// �رղ��Ŷ�ʱ��
			KillTimer(hWnd, PRIOMAN_TIMER_CALLMUSIC);

			// ��λ���ȼ�
			PrioMan_ResetRAM(nPrioMan_Priority);
		}
		break;

	case PRIOMAN_MESSAGE_BREAKOFF:
		// �رճ�ʱ��ʱ��
		KillTimer(hWnd, PRIOMAN_TIMER_ENDCALL);

		// �رղ��Ŷ�ʱ��
		KillTimer(hWnd, PRIOMAN_TIMER_CALLMUSIC);

		// ��ϵ�ǰ����
		PrioMan_BreakOffCall(nPrioMan_Priority);
		break;

	default:
		lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return lResult;
}

/*********************************************************************
* Function	PrioMan_PlayMusic
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int PrioMan_PlayMusic(PPM_PlayMusic pPM)
{
	HWND hCallWnd;
	int nPriority, nRingType, nVolume, nRepeat;
	char* pMusicName;
	unsigned char* pDataBuf;
	int nDataLen;
	PM_RAMFILE RAMFILE;
	int nResult;

	// ��ȡ�龰ģʽ����
	Sett_GetActiveSM(&sPrioMan_Profile);
	if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
	{
		printf("\r\nPRIOMAN: ����ģʽ\r\n");
		return PRIOMAN_ERROR_MUTEMODE;
	}

	// �����������
	if (pPM == NULL || pPM->hCallWnd == NULL)
	{
		printf("\r\nPRIOMAN: �����������\r\n");
		return PRIOMAN_ERROR_PARAMETER;
	}

	// �����������
	hCallWnd = pPM->hCallWnd;
	nPriority = pPM->nPriority;
	nRingType = pPM->nRingType;
	nVolume = pPM->nVolume;
	nRepeat = pPM->nRepeat;
	pMusicName = pPM->pMusicName;
	pDataBuf = pPM->pDataBuf;
	nDataLen = pPM->nDataLen;

	// ��������������ȼ�
	switch (nPriority)
	{
	case PRIOMAN_PRIORITY_PHONE:		// ����
	case PRIOMAN_PRIORITY_ALARM:		// ����
	case PRIOMAN_PRIORITY_CALENDAR:		// �ճ�
	case PRIOMAN_PRIORITY_POWERON:		// ����
	case PRIOMAN_PRIORITY_POWEROFF:		// �ػ�
	case PRIOMAN_PRIORITY_BEEPSOUND:	// Beep Sound
	case PRIOMAN_PRIORITY_SMSAPP:		// ����Ϣ
	case PRIOMAN_PRIORITY_MMSAPP:		// ����
	case PRIOMAN_PRIORITY_EMAIL:		// E-Mail
	case PRIOMAN_PRIORITY_BLUETOOTH:	// ����
	case PRIOMAN_PRIORITY_PUSHINFO:		// PUSH��Ϣ
	case PRIOMAN_PRIORITY_SIMTOOLKIT:	// STK
	case PRIOMAN_PRIORITY_WAPAPP:		// �����
	case PRIOMAN_PRIORITY_MP3APP:		// MP3����
	case PRIOMAN_PRIORITY_MUSICMAN:		// ���ֹ�����
	case PRIOMAN_PRIORITY_CAMERA:		// �����
	case PRIOMAN_PRIORITY_KJAVA:		// KJAVA
	case PRIOMAN_PRIORITY_PUBLIC:		// ����Ԥ��
	case PRIOMAN_PRIORITY_SETTING:		// ��������
	case PRIOMAN_PRIORITY_CONNECT:		// ��ͨ��ʾ��
	case PRIOMAN_PRIORITY_50TIPS:		// 50����ʾ��
	case PRIOMAN_PRIORITY_WARNING:		// ������
	case PRIOMAN_PRIORITY_KEYBOARD:		// ������
	case PRIOMAN_PRIORITY_TSCREEN:		// ������
		if (nVolume < PRIOMAN_VOLUME_LOWLEVEL || nVolume > PRIOMAN_VOLUME_HIGHLEVEL)
		{
			printf("\r\nPRIOMAN: ����������\r\n");
			return PRIOMAN_ERROR_VOLUME;
		}
		break;
	case PRIOMAN_PRIORITY_ZERO:			// �����ȼ�
		printf("\r\nPRIOMAN: �����������ȼ�̫��\r\n");
		return PRIOMAN_ERROR_LOWPRIO;
	default:
		printf("\r\nPRIOMAN: δ֪�������������ȼ�\r\n");
		return PRIOMAN_ERROR_UNKNOWNPRIO;
	}

	if (pMusicName == NULL)
	{
		// ����һ������
		if (pDataBuf == NULL || nDataLen == 0)
		{
			printf("\r\nPRIOMAN: �����������\r\n");
			return PRIOMAN_ERROR_PARAMETER;
		}

		// �����������
		nRingType = PrioMan_GetType(NULL, pDataBuf, nDataLen);
		if (nRingType != PRIOMAN_RINGTYPE_WAVE && nRingType != PRIOMAN_RINGTYPE_AMR)
		{
			printf("\r\nPRIOMAN: �������ʹ���\r\n");
			return PRIOMAN_ERROR_RINGTYPE;
		}

		// ��дRAM����...
		RAMFILE.hCallWnd = hCallWnd;
		RAMFILE.nPriority = nPriority;
		nResult = PrioMan_ReadWriteRAM(&RAMFILE);
		if (nResult == PRIOMAN_ERROR_BREAKOFF)
		{
			// �����ȼ��ر����
			nPrioMan_Priority = RAMFILE.nPriority;
		}
		else
		{
			if (nResult != PRIOMAN_ERROR_SUCCESS)
				return nResult;

			// ��ǰ�޲���
			nPrioMan_Priority = RAMFILE.nPriority;
		}

		// ��������������ȼ�
		memset(&sPrioMan_PlayMusic, 0, sizeof(PM_PlayMusic));
		sPrioMan_PlayMusic.hCallWnd = hCallWnd;
		sPrioMan_PlayMusic.nPriority = nPriority;
		sPrioMan_PlayMusic.nRingType = nRingType;
		sPrioMan_PlayMusic.nVolume = nVolume;
		sPrioMan_PlayMusic.nRepeat = nRepeat;
		sPrioMan_PlayMusic.pMusicName = pMusicName;
		sPrioMan_PlayMusic.pDataBuf = pDataBuf;
		sPrioMan_PlayMusic.nDataLen = nDataLen;

		// �������ž���^_^
		switch (nRingType)
		{
		case PRIOMAN_RINGTYPE_WAVE:
			nResult = PrioMan_PlayWAVE(&sPrioMan_PlayMusic);
			break;
		case PRIOMAN_RINGTYPE_MIDI:
			nResult = PrioMan_PlayMIDI(&sPrioMan_PlayMusic);
			break;
		case PRIOMAN_RINGTYPE_MMF:
			nResult = PrioMan_PlayMMF(&sPrioMan_PlayMusic);
			break;
		case PRIOMAN_RINGTYPE_AMR:
			nResult = PrioMan_PlayAMR(&sPrioMan_PlayMusic);
			break;
		case PRIOMAN_RINGTYPE_MP3:
			nResult = PrioMan_PlayMP3(&sPrioMan_PlayMusic);
			break;
		default:
			nResult = PRIOMAN_ERROR_RINGTYPE;
			break;
		}

		if (nResult != PRIOMAN_ERROR_SUCCESS)
		{
			// RAM���ݸ�λ
			sPrioMan_RAMFILE.hCallWnd = NULL;
			sPrioMan_RAMFILE.nPriority = PRIOMAN_PRIORITY_ZERO;
			nPrioMan_Priority = PRIOMAN_PRIORITY_ZERO;
		}

		return nResult;
	}
	else
	{
		// ���������ļ�
		printf("\r\nPRIOMAN: pMusicName = %s\r\n", pMusicName);
		if (pDataBuf != NULL || nDataLen != 0)
		{
			printf("\r\nPRIOMAN: �����������\r\n");
			return PRIOMAN_ERROR_PARAMETER;
		}

		// �����������
		if (nRingType == PRIOMAN_RINGTYPE_MP3)
		{
			// MP3����
			nRingType = PRIOMAN_RINGTYPE_MP3;
		}
		else
		{
			// ��������
			nRingType = PrioMan_GetType(pMusicName, NULL, 0);
			if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
			{
				printf("\r\nPRIOMAN: �������ʹ���\r\n");
				return PRIOMAN_ERROR_RINGTYPE;
			}
		}

		// ��дRAM����...
		RAMFILE.hCallWnd = hCallWnd;
		RAMFILE.nPriority = nPriority;
		nResult = PrioMan_ReadWriteRAM(&RAMFILE);
		if (nResult == PRIOMAN_ERROR_BREAKOFF)
		{
			// �����ȼ��ر����
			nPrioMan_Priority = RAMFILE.nPriority;
		}
		else
		{
			if (nResult != PRIOMAN_ERROR_SUCCESS)
				return nResult;

			// ��ǰ�޲���
			nPrioMan_Priority = RAMFILE.nPriority;
		}

		// ��������������ȼ�
		memset(&sPrioMan_PlayMusic, 0, sizeof(PM_PlayMusic));
		sPrioMan_PlayMusic.hCallWnd = hCallWnd;
		sPrioMan_PlayMusic.nPriority = nPriority;
		sPrioMan_PlayMusic.nRingType = nRingType;
		sPrioMan_PlayMusic.nVolume = nVolume;
		sPrioMan_PlayMusic.nRepeat = nRepeat;
		sPrioMan_PlayMusic.pMusicName = pMusicName;
		sPrioMan_PlayMusic.pDataBuf = pDataBuf;
		sPrioMan_PlayMusic.nDataLen = nDataLen;

		// �������ž���^_^
		switch (nRingType)
		{
		case PRIOMAN_RINGTYPE_WAVE:
			nResult = PrioMan_PlayWAVE(&sPrioMan_PlayMusic);
			break;
		case PRIOMAN_RINGTYPE_MIDI:
			nResult = PrioMan_PlayMIDI(&sPrioMan_PlayMusic);
			break;
		case PRIOMAN_RINGTYPE_MMF:
			nResult = PrioMan_PlayMMF(&sPrioMan_PlayMusic);
			break;
		case PRIOMAN_RINGTYPE_AMR:
			nResult = PrioMan_PlayAMR(&sPrioMan_PlayMusic);
			break;
		case PRIOMAN_RINGTYPE_MP3:
			nResult = PrioMan_PlayMP3(&sPrioMan_PlayMusic);
			break;
		default:
			nResult = PRIOMAN_ERROR_RINGTYPE;
			break;
		}

		if (nResult != PRIOMAN_ERROR_SUCCESS)
		{
			// RAM���ݸ�λ
			sPrioMan_RAMFILE.hCallWnd = NULL;
			sPrioMan_RAMFILE.nPriority = PRIOMAN_PRIORITY_ZERO;
			nPrioMan_Priority = PRIOMAN_PRIORITY_ZERO;
		}

		return nResult;
	}

	return PRIOMAN_ERROR_UNKNOWN;
}

/*********************************************************************
* Function	PrioMan_ReadData
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void PrioMan_ReadData(WPARAM wParam, LPARAM lParam)
{
	int nRingType;

	// ��ȡ��������
	nRingType = sPrioMan_PlayMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		if (wParam == nPrioMan_Priority)
			WaveReadData(wParam, lParam);
		break;
	case PRIOMAN_RINGTYPE_MIDI:
		break;
	case PRIOMAN_RINGTYPE_MMF:
		break;
	case PRIOMAN_RINGTYPE_AMR:
		if (wParam == nPrioMan_Priority)
			AMRReadData(wParam, lParam);
		break;
	case PRIOMAN_RINGTYPE_MP3:
		if (wParam == nPrioMan_Priority)
			MP3ReadData(wParam, lParam);
		break;
	default:
		break;
	}
}

/*********************************************************************
* Function	PrioMan_WriteData
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
void PrioMan_WriteData(WPARAM wParam, LPARAM lParam)
{
	int nRingType;

	// д����������
	nRingType = sPrioMan_PlayMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		if (wParam == nPrioMan_Priority)
			WaveWriteData(wParam, lParam);
		break;
	case PRIOMAN_RINGTYPE_MIDI:
		break;
	case PRIOMAN_RINGTYPE_MMF:
		break;
	case PRIOMAN_RINGTYPE_AMR:
		if (wParam == nPrioMan_Priority)
			AMRWriteData(wParam, lParam);
		break;
	case PRIOMAN_RINGTYPE_MP3:
		if (wParam == nPrioMan_Priority)
			MP3WriteData(wParam, lParam);
		break;
	default:
		break;
	}
}

/*********************************************************************
* Function	PrioMan_EndPlayMusic
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int PrioMan_EndPlayMusic(int nPrio)
{
	int nRingType;

	// ��鵱ǰ�Ƿ��н������ڲ���
	if (!PrioMan_IsPlaying(nPrio))
	{
		printf("\r\nPRIOMAN: ��ǰ�н������ڲ���\r\n");
		return PRIOMAN_ERROR_ISPLAYING;
	}

	// ������������^_^
	nRingType = sPrioMan_PlayMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		return PrioMan_EndPlayWAVE(nPrio, FALSE, FALSE);
	case PRIOMAN_RINGTYPE_MIDI:
		return PrioMan_EndPlayMIDI(nPrio, FALSE, FALSE);
	case PRIOMAN_RINGTYPE_MMF:
		return PrioMan_EndPlayMMF(nPrio, FALSE, FALSE);
	case PRIOMAN_RINGTYPE_AMR:
		return PrioMan_EndPlayAMR(nPrio, FALSE, FALSE);
	case PRIOMAN_RINGTYPE_MP3:
		return PrioMan_EndPlayMP3(nPrio, FALSE, FALSE);
	default:
		break;
	}

	return PRIOMAN_ERROR_RINGTYPE;
}

/*********************************************************************
* Function	PrioMan_BreakOffMusic
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int PrioMan_BreakOffMusic(int nPrio)
{
	int nRingType;

	// ������������^_^
	nRingType = sPrioMan_PlayMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		return PrioMan_EndPlayWAVE(nPrio, FALSE, TRUE);
	case PRIOMAN_RINGTYPE_MIDI:
		return PrioMan_EndPlayMIDI(nPrio, FALSE, TRUE);
	case PRIOMAN_RINGTYPE_MMF:
		return PrioMan_EndPlayMMF(nPrio, FALSE, TRUE);
	case PRIOMAN_RINGTYPE_AMR:
		return PrioMan_EndPlayAMR(nPrio, FALSE, TRUE);
	case PRIOMAN_RINGTYPE_MP3:
		return PrioMan_EndPlayMP3(nPrio, FALSE, TRUE);
	default:
		break;
	}

	return PRIOMAN_ERROR_RINGTYPE;
}

/*********************************************************************
* Function	PrioMan_PauseMusic
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int PrioMan_PauseMusic(void)
{
	int nRingType;

	// ��ͣ��������^_^
	nRingType = sPrioMan_PlayMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		return PrioMan_PauseWAVE();
	case PRIOMAN_RINGTYPE_MIDI:
		return PrioMan_PauseMIDI();
	case PRIOMAN_RINGTYPE_MMF:
		return PrioMan_PauseMMF();
	case PRIOMAN_RINGTYPE_AMR:
		return PrioMan_PauseAMR();
	case PRIOMAN_RINGTYPE_MP3:
		return PrioMan_PauseMP3();
	default:
		break;
	}

	return PRIOMAN_ERROR_RINGTYPE;
}

/*********************************************************************
* Function	PrioMan_ResumeMusic
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int PrioMan_ResumeMusic(void)
{
	int nRingType;

	// ������������^_^
	nRingType = sPrioMan_PlayMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		return PrioMan_ResumeWAVE();
	case PRIOMAN_RINGTYPE_MIDI:
		return PrioMan_ResumeMIDI();
	case PRIOMAN_RINGTYPE_MMF:
		return PrioMan_ResumeMMF();
	case PRIOMAN_RINGTYPE_AMR:
		return PrioMan_ResumeAMR();
	case PRIOMAN_RINGTYPE_MP3:
		return PrioMan_ResumeMP3();
	default:
		break;
	}

	return PRIOMAN_ERROR_RINGTYPE;
}

/*********************************************************************
* Function	PrioMan_CallMusic
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL PrioMan_CallMusic(int nPrio, int nRep)
{
	HWND hCallWnd;
	int nPriority, nRingType, nVolume, nRepeat;
	char* pMusicName;
	unsigned char* pDataBuf;
	int nDataLen;
	char cPhoneNum[PHONENUMMAXLEN + 1];
	PM_RAMFILE RAMFILE;
	int nResult;

	// ��ȡ�龰ģʽ����
	Sett_GetActiveSM(&sPrioMan_Profile);

	// �����������
	hCallWnd = hPrioMan_HideWnd;
	nPriority = nPrio;
	nRepeat = nRep;
	if (nRepeat == 0)
		nRepeat = -1;
	else
		nRepeat--;
	pDataBuf = NULL;
	nDataLen = 0;

	// ��������������ȼ�
	switch (nPriority)
	{
	case PRIOMAN_PRIORITY_PHONE:		// ����
		if (sPrioMan_Profile.iVibraSwitch == SWITCH_ON)
		{
			printf("\r\nPRIOMAN: �ֻ���5��\r\n");
			PrioMan_Vibration(5);
		}
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		switch (sPrioMan_Profile.rIncomeCall.iRingType)
		{
		case NORMAL:
		case ASCENDING:
		case BEEPFIRST:
			GetCurrentPhoneNum(cPhoneNum, sizeof(cPhoneNum));
			if (APP_GetInfoByPhone(cPhoneNum, &sPrioMan_ABInfo))
			{
				printf("\r\nPRIOMAN: ��ϵ���ļ� = %s\r\n", sPrioMan_ABInfo.szRing);
				if (!FileIfExist(sPrioMan_ABInfo.szRing))
				{
					pMusicName = sPrioMan_Profile.rIncomeCall.cMusicFileName;
					printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
				}
				else
				{
					pMusicName = sPrioMan_ABInfo.szRing;
					printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
				}
			}
			else
			{
				pMusicName = sPrioMan_Profile.rIncomeCall.cMusicFileName;
				printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
			}
			nRingType = PrioMan_GetExp(pMusicName);
			if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
			{
				printf("\r\nPRIOMAN: �������ʹ���\r\n");
				return FALSE;
			}
			nVolume = sPrioMan_Profile.rIncomeCall.iRingVolume;
			if (nVolume == 0)
				return FALSE;
			break;
		default:
			printf("\r\nPRIOMAN: ��ȡ�龰ģʽ���ݴ���\r\n");
			return FALSE;
		}
		break;
	case PRIOMAN_PRIORITY_ALARM:		// ����
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		switch (sPrioMan_Profile.rAlarmClk.iRingType)
		{
		case NORMAL:
		case ASCENDING:
		case BEEPFIRST:
			pMusicName = sPrioMan_Profile.rAlarmClk.cMusicFileName;
			printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
			nRingType = PrioMan_GetExp(pMusicName);
			if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
			{
				printf("\r\nPRIOMAN: �������ʹ���\r\n");
				return FALSE;
			}
			nVolume = sPrioMan_Profile.rAlarmClk.iRingVolume;
			if (nVolume == 0)
				return FALSE;
			break;
		default:
			printf("\r\nPRIOMAN: ��ȡ�龰ģʽ���ݴ���\r\n");
			return FALSE;
		}
		break;
	case PRIOMAN_PRIORITY_CALENDAR:		// �ճ�
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		switch (sPrioMan_Profile.rCalendar.iRingType)
		{
		case NORMAL:
		case ASCENDING:
		case BEEPFIRST:
			pMusicName = sPrioMan_Profile.rCalendar.cMusicFileName;
			printf("\r\nPRIOMAN: �ճ��ļ� = %s\r\n", pMusicName);
			nRingType = PrioMan_GetExp(pMusicName);
			if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
			{
				printf("\r\nPRIOMAN: �������ʹ���\r\n");
				return FALSE;
			}
			nVolume = sPrioMan_Profile.rCalendar.iRingVolume;
			if (nVolume == 0)
				return FALSE;
			break;
		default:
			printf("\r\nPRIOMAN: ��ȡ�龰ģʽ���ݴ���\r\n");
			return FALSE;
		}
		break;
	case PRIOMAN_PRIORITY_POWERON:		// ����
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = PRIOMAN_RINGFILE_POWERON;
		printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_AMR;
		nVolume = PRIOMAN_VOLUME_DEFAULT;
		break;
	case PRIOMAN_PRIORITY_POWEROFF:		// �ػ�
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = PRIOMAN_RINGFILE_POWEROFF;
		printf("\r\nPRIOMAN: �ػ��ļ� = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_AMR;
		nVolume = PRIOMAN_VOLUME_DEFAULT;
		break;
	case PRIOMAN_PRIORITY_BEEPSOUND:	// Beep Sound
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = PRIOMAN_RINGFILE_BEEPSOUND;
		printf("\r\nPRIOMAN: Beep Sound = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_WAVE;
		nVolume = PRIOMAN_VOLUME_DEFAULT;
		break;
	case PRIOMAN_PRIORITY_SMSAPP:		// ����Ϣ
	case PRIOMAN_PRIORITY_MMSAPP:		// ����
	case PRIOMAN_PRIORITY_EMAIL:		// E-Mail
	case PRIOMAN_PRIORITY_BLUETOOTH:	// ����
	case PRIOMAN_PRIORITY_PUSHINFO:		// PUSH��Ϣ
		if (sPrioMan_Profile.iVibraSwitch == SWITCH_ON)
		{
			printf("\r\nPRIOMAN: �ֻ���3��\r\n");
			PrioMan_Vibration(3);
		}
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		switch (sPrioMan_Profile.rMessage.iRingType)
		{
		case NORMAL:
		case ASCENDING:
		case BEEPFIRST:
			pMusicName = sPrioMan_Profile.rMessage.cMusicFileName;
			printf("\r\nPRIOMAN: ��Ϣ�ļ� = %s\r\n", pMusicName);
			nRingType = PrioMan_GetExp(pMusicName);
			if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
			{
				printf("\r\nPRIOMAN: �������ʹ���\r\n");
				return FALSE;
			}
			nVolume = sPrioMan_Profile.rMessage.iRingVolume;
			if (nVolume == 0)
				return FALSE;
			break;
		default:
			printf("\r\nPRIOMAN: ��ȡ�龰ģʽ���ݴ���\r\n");
			return FALSE;
		}
		break;
	case PRIOMAN_PRIORITY_SIMTOOLKIT:	// STK
		printf("\r\nPRIOMAN: ��ʱ��δʵ��\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_WAPAPP:		// �����
	case PRIOMAN_PRIORITY_MP3APP:		// MP3����
	case PRIOMAN_PRIORITY_MUSICMAN:		// ���ֹ�����
		printf("\r\nPRIOMAN: ����õ�һ�׽ӿ�\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_CAMERA:		// �����
	case PRIOMAN_PRIORITY_KJAVA:		// KJAVA
	case PRIOMAN_PRIORITY_PUBLIC:		// ����Ԥ��
	case PRIOMAN_PRIORITY_SETTING:		// ��������
		printf("\r\nPRIOMAN: ����õ�һ�׽ӿ�\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_CONNECT:		// ��ͨ��ʾ��
	case PRIOMAN_PRIORITY_50TIPS:		// 50����ʾ��
		printf("\r\nPRIOMAN: ��ʾ����Ŀ��֧��\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_WARNING:		// ������
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = NULL;
		pDataBuf = (unsigned char *)KeyWaveDataBuf;
		nDataLen = KEY_WAVE_DATA_LEN;
		printf("\r\nPRIOMAN: �������ļ� = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_WAVE;
		nVolume = sPrioMan_Profile.iNotification;
		if (nVolume == 0)
			return FALSE;
		break;
	case PRIOMAN_PRIORITY_KEYBOARD:		// ������
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = NULL;
		pDataBuf = (unsigned char *)KeyWaveDataBuf;
		nDataLen = KEY_WAVE_DATA_LEN;
		printf("\r\nPRIOMAN: �������ļ� = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_WAVE;
		nVolume = sPrioMan_Profile.iKeyVolume;
		if (nVolume == 0)
			return FALSE;
		break;
	case PRIOMAN_PRIORITY_TSCREEN:		// ������
		printf("\r\nPRIOMAN: ��������Ŀ��֧��\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_ZERO:			// �����ȼ�
		printf("\r\nPRIOMAN: �����������ȼ�̫��\r\n");
		return FALSE;
	default:
		printf("\r\nPRIOMAN: δ֪�������������ȼ�\r\n");
		return FALSE;
	}

	// ��дRAM����...
	RAMFILE.hCallWnd = hCallWnd;
	RAMFILE.nPriority = nPriority;
	nResult = PrioMan_ReadWriteRAM(&RAMFILE);
	if (nResult == PRIOMAN_ERROR_BREAKOFF)
	{
		// �����ȼ��ر����
		nPrioMan_Priority = RAMFILE.nPriority;
	}
	else
	{
		if (nResult != PRIOMAN_ERROR_SUCCESS)
			return FALSE;

		// ��ǰ�޲���
		nPrioMan_Priority = RAMFILE.nPriority;
	}

	// ��������������ȼ�
	memset(&sPrioMan_CallMusic, 0, sizeof(PM_PlayMusic));
	sPrioMan_CallMusic.hCallWnd = hCallWnd;
	sPrioMan_CallMusic.nPriority = nPriority;
	sPrioMan_CallMusic.nRingType = nRingType;
	sPrioMan_CallMusic.nVolume = nVolume;
	sPrioMan_CallMusic.nRepeat = nRepeat;
	sPrioMan_CallMusic.pMusicName = pMusicName;
	sPrioMan_CallMusic.pDataBuf = pDataBuf;
	sPrioMan_CallMusic.nDataLen = nDataLen;

	// �������ž���^_^
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		nResult = PrioMan_PlayWAVE(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_MIDI:
		nResult = PrioMan_PlayMIDI(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_MMF:
		nResult = PrioMan_PlayMMF(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_AMR:
		nResult = PrioMan_PlayAMR(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_MP3:
		nResult = PrioMan_PlayMP3(&sPrioMan_CallMusic);
		break;
	default:
		nResult = PRIOMAN_ERROR_RINGTYPE;
		break;
	}

	if (nResult != PRIOMAN_ERROR_SUCCESS)
	{
		// RAM���ݸ�λ
		sPrioMan_RAMFILE.hCallWnd = NULL;
		sPrioMan_RAMFILE.nPriority = PRIOMAN_PRIORITY_ZERO;
		nPrioMan_Priority = PRIOMAN_PRIORITY_ZERO;

		return FALSE;
	}

	return TRUE;
}

/*********************************************************************
* Function	PrioMan_CallMusicEx
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL PrioMan_CallMusicEx(int nPrio, int nTimeOut)
{
	HWND hCallWnd;
	int nPriority, nRingType, nVolume, nRepeat;
	int nPlayTime;
	char* pMusicName;
	char cPhoneNum[PHONENUMMAXLEN + 1];
	PM_RAMFILE RAMFILE;
	int nResult;

	// ��ȡ�龰ģʽ����
	Sett_GetActiveSM(&sPrioMan_Profile);

	// �����������
	hCallWnd = hPrioMan_HideWnd;
	nPriority = nPrio;
	nRepeat = 0;

	// ���ų�ʱʱ��
	nPlayTime = nTimeOut;
	if (nPlayTime == 0)
		nPlayTime = 3000;

	// ��������������ȼ�
	switch (nPriority)
	{
	case PRIOMAN_PRIORITY_PHONE:		// ����
		if (sPrioMan_Profile.iVibraSwitch == SWITCH_ON)
		{
			printf("\r\nPRIOMAN: �ֻ���5��\r\n");
			PrioMan_Vibration(5);
		}
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		switch (sPrioMan_Profile.rIncomeCall.iRingType)
		{
		case NORMAL:
		case ASCENDING:
		case BEEPFIRST:
			GetCurrentPhoneNum(cPhoneNum, sizeof(cPhoneNum));
			if (APP_GetInfoByPhone(cPhoneNum, &sPrioMan_ABInfo))
			{
				printf("\r\nPRIOMAN: ��ϵ���ļ� = %s\r\n", sPrioMan_ABInfo.szRing);
				if (!FileIfExist(sPrioMan_ABInfo.szRing))
				{
					pMusicName = sPrioMan_Profile.rIncomeCall.cMusicFileName;
					printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
				}
				else
				{
					pMusicName = sPrioMan_ABInfo.szRing;
					printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
				}
			}
			else
			{
				pMusicName = sPrioMan_Profile.rIncomeCall.cMusicFileName;
				printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
			}
			nRingType = PrioMan_GetExp(pMusicName);
			if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
			{
				printf("\r\nPRIOMAN: �������ʹ���\r\n");
				return FALSE;
			}
			nVolume = sPrioMan_Profile.rIncomeCall.iRingVolume;
			if (nVolume == 0)
				return FALSE;
			break;
		default:
			printf("\r\nPRIOMAN: ��ȡ�龰ģʽ���ݴ���\r\n");
			return FALSE;
		}
		break;
	case PRIOMAN_PRIORITY_ALARM:		// ����
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		switch (sPrioMan_Profile.rAlarmClk.iRingType)
		{
		case NORMAL:
		case ASCENDING:
		case BEEPFIRST:
			pMusicName = sPrioMan_Profile.rAlarmClk.cMusicFileName;
			printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
			nRingType = PrioMan_GetExp(pMusicName);
			if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
			{
				printf("\r\nPRIOMAN: �������ʹ���\r\n");
				return FALSE;
			}
			nVolume = sPrioMan_Profile.rAlarmClk.iRingVolume;
			if (nVolume == 0)
				return FALSE;
			break;
		default:
			printf("\r\nPRIOMAN: ��ȡ�龰ģʽ���ݴ���\r\n");
			return FALSE;
		}
		break;
	case PRIOMAN_PRIORITY_CALENDAR:		// �ճ�
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		switch (sPrioMan_Profile.rCalendar.iRingType)
		{
		case NORMAL:
		case ASCENDING:
		case BEEPFIRST:
			pMusicName = sPrioMan_Profile.rCalendar.cMusicFileName;
			printf("\r\nPRIOMAN: �ճ��ļ� = %s\r\n", pMusicName);
			nRingType = PrioMan_GetExp(pMusicName);
			if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
			{
				printf("\r\nPRIOMAN: �������ʹ���\r\n");
				return FALSE;
			}
			nVolume = sPrioMan_Profile.rCalendar.iRingVolume;
			if (nVolume == 0)
				return FALSE;
			break;
		default:
			printf("\r\nPRIOMAN: ��ȡ�龰ģʽ���ݴ���\r\n");
			return FALSE;
		}
		break;
	case PRIOMAN_PRIORITY_POWERON:		// ����
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = PRIOMAN_RINGFILE_POWERON;
		printf("\r\nPRIOMAN: �����ļ� = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_AMR;
		nVolume = PRIOMAN_VOLUME_DEFAULT;
		break;
	case PRIOMAN_PRIORITY_POWEROFF:		// �ػ�
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = PRIOMAN_RINGFILE_POWEROFF;
		printf("\r\nPRIOMAN: �ػ��ļ� = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_AMR;
		nVolume = PRIOMAN_VOLUME_DEFAULT;
		break;
	case PRIOMAN_PRIORITY_BEEPSOUND:	// Beep Sound
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = PRIOMAN_RINGFILE_BEEPSOUND;
		printf("\r\nPRIOMAN: Beep Sound = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_WAVE;
		nVolume = PRIOMAN_VOLUME_DEFAULT;
		break;
	case PRIOMAN_PRIORITY_SMSAPP:		// ����Ϣ
	case PRIOMAN_PRIORITY_MMSAPP:		// ����
	case PRIOMAN_PRIORITY_EMAIL:		// E-Mail
	case PRIOMAN_PRIORITY_BLUETOOTH:	// ����
	case PRIOMAN_PRIORITY_PUSHINFO:		// PUSH��Ϣ
		if (sPrioMan_Profile.iVibraSwitch == SWITCH_ON)
		{
			printf("\r\nPRIOMAN: �ֻ���3��\r\n");
			PrioMan_Vibration(3);
		}
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		switch (sPrioMan_Profile.rMessage.iRingType)
		{
		case NORMAL:
		case ASCENDING:
		case BEEPFIRST:
			pMusicName = sPrioMan_Profile.rMessage.cMusicFileName;
			printf("\r\nPRIOMAN: ��Ϣ�ļ� = %s\r\n", pMusicName);
			nRingType = PrioMan_GetExp(pMusicName);
			if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
			{
				printf("\r\nPRIOMAN: �������ʹ���\r\n");
				return FALSE;
			}
			nVolume = sPrioMan_Profile.rMessage.iRingVolume;
			if (nVolume == 0)
				return FALSE;
			break;
		default:
			printf("\r\nPRIOMAN: ��ȡ�龰ģʽ���ݴ���\r\n");
			return FALSE;
		}
		break;
	case PRIOMAN_PRIORITY_SIMTOOLKIT:	// STK
		printf("\r\nPRIOMAN: ��ʱ��δʵ��\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_WAPAPP:		// �����
	case PRIOMAN_PRIORITY_MP3APP:		// MP3����
	case PRIOMAN_PRIORITY_MUSICMAN:		// ���ֹ�����
		printf("\r\nPRIOMAN: ����õ�һ�׽ӿ�\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_CAMERA:		// �����
	case PRIOMAN_PRIORITY_KJAVA:		// KJAVA
	case PRIOMAN_PRIORITY_PUBLIC:		// ����Ԥ��
	case PRIOMAN_PRIORITY_SETTING:		// ��������
		printf("\r\nPRIOMAN: ����õ�һ�׽ӿ�\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_CONNECT:		// ��ͨ��ʾ��
	case PRIOMAN_PRIORITY_50TIPS:		// 50����ʾ��
		printf("\r\nPRIOMAN: ��ʾ����Ŀ��֧��\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_WARNING:		// ������
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = PRIOMAN_RINGFILE_WARNING;
		printf("\r\nPRIOMAN: �������ļ� = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_WAVE;
		nVolume = sPrioMan_Profile.iNotification;
		if (nVolume == 0)
			return FALSE;
		break;
	case PRIOMAN_PRIORITY_KEYBOARD:		// ������
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		pMusicName = PRIOMAN_RINGFILE_KEYBOARD;
		printf("\r\nPRIOMAN: �������ļ� = %s\r\n", pMusicName);
		nRingType = PRIOMAN_RINGTYPE_WAVE;
		nVolume = sPrioMan_Profile.iKeyVolume;
		if (nVolume == 0)
			return FALSE;
		break;
	case PRIOMAN_PRIORITY_TSCREEN:		// ������
		printf("\r\nPRIOMAN: ��������Ŀ��֧��\r\n");
		return FALSE;
	case PRIOMAN_PRIORITY_ZERO:			// �����ȼ�
		printf("\r\nPRIOMAN: �����������ȼ�̫��\r\n");
		return FALSE;
	default:
		printf("\r\nPRIOMAN: δ֪�������������ȼ�\r\n");
		return FALSE;
	}

	// ��дRAM����...
	RAMFILE.hCallWnd = hCallWnd;
	RAMFILE.nPriority = nPriority;
	nResult = PrioMan_ReadWriteRAM(&RAMFILE);
	if (nResult == PRIOMAN_ERROR_BREAKOFF)
	{
		// �����ȼ��ر����
		nPrioMan_Priority = RAMFILE.nPriority;
	}
	else
	{
		if (nResult != PRIOMAN_ERROR_SUCCESS)
			return FALSE;

		// ��ǰ�޲���
		nPrioMan_Priority = RAMFILE.nPriority;
	}

	// ��������������ȼ�
	memset(&sPrioMan_CallMusic, 0, sizeof(PM_PlayMusic));
	sPrioMan_CallMusic.hCallWnd = hCallWnd;
	sPrioMan_CallMusic.nPriority = nPriority;
	sPrioMan_CallMusic.nRingType = nRingType;
	sPrioMan_CallMusic.nVolume = nVolume;
	sPrioMan_CallMusic.nRepeat = nRepeat;
	sPrioMan_CallMusic.pMusicName = pMusicName;
	sPrioMan_CallMusic.pDataBuf = NULL;
	sPrioMan_CallMusic.nDataLen = 0;

	// ���ó�ʱ��ʱ��
	SetTimer(hPrioMan_HideWnd, PRIOMAN_TIMER_ENDCALL, nPlayTime, NULL);

	// �������ž���^_^
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		nResult = PrioMan_PlayWAVE(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_MIDI:
		nResult = PrioMan_PlayMIDI(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_MMF:
		nResult = PrioMan_PlayMMF(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_AMR:
		nResult = PrioMan_PlayAMR(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_MP3:
		nResult = PrioMan_PlayMP3(&sPrioMan_CallMusic);
		break;
	default:
		nResult = PRIOMAN_ERROR_RINGTYPE;
		break;
	}

	if (nResult != PRIOMAN_ERROR_SUCCESS)
	{
		// �رճ�ʱ��ʱ��
		KillTimer(hPrioMan_HideWnd, PRIOMAN_TIMER_ENDCALL);

		// RAM���ݸ�λ
		sPrioMan_RAMFILE.hCallWnd = NULL;
		sPrioMan_RAMFILE.nPriority = PRIOMAN_PRIORITY_ZERO;
		nPrioMan_Priority = PRIOMAN_PRIORITY_ZERO;

		return FALSE;
	}

	return TRUE;
}

/*********************************************************************
* Function	PrioMan_CallMusicFile
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL PrioMan_CallMusicFile(char* pMusicName, int nPrio, int nRep, int nVol)
{
	HWND hCallWnd;
	int nPriority, nRingType, nVolume, nRepeat;
	PM_RAMFILE RAMFILE;
	int nResult;

	// ��ȡ�龰ģʽ����
	Sett_GetActiveSM(&sPrioMan_Profile);

	// �����������
	hCallWnd = hPrioMan_HideWnd;
	nPriority = nPrio;
	nRepeat = nRep;
	if (nRepeat == 0)
		nRepeat = -1;
	else
		nRepeat--;

	// ��������������ȼ�
	switch (nPriority)
	{
	case PRIOMAN_PRIORITY_PHONE:		// ����
	case PRIOMAN_PRIORITY_ALARM:		// ����
	case PRIOMAN_PRIORITY_CALENDAR:		// �ճ�
	case PRIOMAN_PRIORITY_POWERON:		// ����
	case PRIOMAN_PRIORITY_POWEROFF:		// �ػ�
	case PRIOMAN_PRIORITY_BEEPSOUND:	// Beep Sound
	case PRIOMAN_PRIORITY_SMSAPP:		// ����Ϣ
	case PRIOMAN_PRIORITY_MMSAPP:		// ����
	case PRIOMAN_PRIORITY_EMAIL:		// E-Mail
	case PRIOMAN_PRIORITY_BLUETOOTH:	// ����
	case PRIOMAN_PRIORITY_PUSHINFO:		// PUSH��Ϣ
	case PRIOMAN_PRIORITY_SIMTOOLKIT:	// STK
	case PRIOMAN_PRIORITY_WAPAPP:		// �����
	case PRIOMAN_PRIORITY_MP3APP:		// MP3����
	case PRIOMAN_PRIORITY_MUSICMAN:		// ���ֹ�����
	case PRIOMAN_PRIORITY_CAMERA:		// �����
	case PRIOMAN_PRIORITY_KJAVA:		// KJAVA
	case PRIOMAN_PRIORITY_PUBLIC:		// ����Ԥ��
	case PRIOMAN_PRIORITY_SETTING:		// ��������
	case PRIOMAN_PRIORITY_CONNECT:		// ��ͨ��ʾ��
	case PRIOMAN_PRIORITY_50TIPS:		// 50����ʾ��
	case PRIOMAN_PRIORITY_WARNING:		// ������
	case PRIOMAN_PRIORITY_KEYBOARD:		// ������
	case PRIOMAN_PRIORITY_TSCREEN:		// ������
		if (strcmp(sPrioMan_Profile.cModeName, "Silent") == 0)
		{
			printf("\r\nPRIOMAN: ����ģʽ\r\n");
			return FALSE;
		}
		printf("\r\nPRIOMAN: pMusicName = %s\r\n", pMusicName);
		nRingType = PrioMan_GetExp(pMusicName);
		if (nRingType == PRIOMAN_RINGTYPE_UNKNOWN)
		{
			printf("\r\nPRIOMAN: �������ʹ���\r\n");
			return FALSE;
		}
		nVolume = nVol;
		break;
	case PRIOMAN_PRIORITY_ZERO:			// �����ȼ�
		printf("\r\nPRIOMAN: �����������ȼ�̫��\r\n");
		return FALSE;
	default:
		printf("\r\nPRIOMAN: δ֪�������������ȼ�\r\n");
		return FALSE;
	}

	// ��дRAM����...
	RAMFILE.hCallWnd = hCallWnd;
	RAMFILE.nPriority = nPriority;
	nResult = PrioMan_ReadWriteRAM(&RAMFILE);
	if (nResult == PRIOMAN_ERROR_BREAKOFF)
	{
		// �����ȼ��ر����
		nPrioMan_Priority = RAMFILE.nPriority;
	}
	else
	{
		if (nResult != PRIOMAN_ERROR_SUCCESS)
			return FALSE;

		// ��ǰ�޲���
		nPrioMan_Priority = RAMFILE.nPriority;
	}

	// ��������������ȼ�
	memset(&sPrioMan_CallMusic, 0, sizeof(PM_PlayMusic));
	sPrioMan_CallMusic.hCallWnd = hCallWnd;
	sPrioMan_CallMusic.nPriority = nPriority;
	sPrioMan_CallMusic.nRingType = nRingType;
	sPrioMan_CallMusic.nVolume = nVolume;
	sPrioMan_CallMusic.nRepeat = nRepeat;
	sPrioMan_CallMusic.pMusicName = pMusicName;
	sPrioMan_CallMusic.pDataBuf = NULL;
	sPrioMan_CallMusic.nDataLen = 0;

	// �������ž���^_^
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		nResult = PrioMan_PlayWAVE(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_MIDI:
		nResult = PrioMan_PlayMIDI(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_MMF:
		nResult = PrioMan_PlayMMF(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_AMR:
		nResult = PrioMan_PlayAMR(&sPrioMan_CallMusic);
		break;
	case PRIOMAN_RINGTYPE_MP3:
		nResult = PrioMan_PlayMP3(&sPrioMan_CallMusic);
		break;
	default:
		nResult = PRIOMAN_ERROR_RINGTYPE;
		break;
	}

	if (nResult != PRIOMAN_ERROR_SUCCESS)
	{
		// RAM���ݸ�λ
		sPrioMan_RAMFILE.hCallWnd = NULL;
		sPrioMan_RAMFILE.nPriority = PRIOMAN_PRIORITY_ZERO;
		nPrioMan_Priority = PRIOMAN_PRIORITY_ZERO;

		return FALSE;
	}

	return TRUE;
}

/*********************************************************************
* Function	PrioMan_EndCallMusic
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL PrioMan_EndCallMusic(int nPrio, BOOL bStop)
{
	int nRingType;

	if (bStop)
	{
		// �رղ��Ŷ�ʱ��
		KillTimer(hPrioMan_HideWnd, PRIOMAN_TIMER_CALLMUSIC);

		// ��λ���ȼ�
		PrioMan_ResetRAM(nPrio);
	}

	// ��鵱ǰ�Ƿ��н������ڲ���
	if (!PrioMan_IsPlaying(nPrio))
	{
		printf("\r\nPRIOMAN: ��ǰ�н������ڲ���\r\n");
		return PRIOMAN_ERROR_ISPLAYING;
	}

	// ������������^_^
	nRingType = sPrioMan_CallMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		return (PrioMan_EndPlayWAVE(nPrio, TRUE, FALSE) == PRIOMAN_ERROR_SUCCESS);
	case PRIOMAN_RINGTYPE_MIDI:
		return (PrioMan_EndPlayMIDI(nPrio, TRUE, FALSE) == PRIOMAN_ERROR_SUCCESS);
	case PRIOMAN_RINGTYPE_MMF:
		return (PrioMan_EndPlayMMF(nPrio, TRUE, FALSE) == PRIOMAN_ERROR_SUCCESS);
	case PRIOMAN_RINGTYPE_AMR:
		return (PrioMan_EndPlayAMR(nPrio, TRUE, FALSE) == PRIOMAN_ERROR_SUCCESS);
	case PRIOMAN_RINGTYPE_MP3:
		return (PrioMan_EndPlayMP3(nPrio, TRUE, FALSE) == PRIOMAN_ERROR_SUCCESS);
	default:
		break;
	}

	return FALSE;
}

/*********************************************************************
* Function	PrioMan_BreakOffCall
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
BOOL PrioMan_BreakOffCall(int nPrio)
{
	int nRingType;

	// ������������^_^
	nRingType = sPrioMan_CallMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		return (PrioMan_EndPlayWAVE(nPrio, TRUE, TRUE) == PRIOMAN_ERROR_SUCCESS);
	case PRIOMAN_RINGTYPE_MIDI:
		return (PrioMan_EndPlayMIDI(nPrio, TRUE, TRUE) == PRIOMAN_ERROR_SUCCESS);
	case PRIOMAN_RINGTYPE_MMF:
		return (PrioMan_EndPlayMMF(nPrio, TRUE, TRUE) == PRIOMAN_ERROR_SUCCESS);
	case PRIOMAN_RINGTYPE_AMR:
		return (PrioMan_EndPlayAMR(nPrio, TRUE, TRUE) == PRIOMAN_ERROR_SUCCESS);
	case PRIOMAN_RINGTYPE_MP3:
		return (PrioMan_EndPlayMP3(nPrio, TRUE, TRUE) == PRIOMAN_ERROR_SUCCESS);
	default:
		break;
	}

	return FALSE;
}

/*********************************************************************
* Function	PrioMan_MP3GetTotalTime
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int PrioMan_MP3GetTotalTime(void)
{
	return MP3GetTotalTime();
}

/*********************************************************************
* Function	PrioMan_MP3GetTotalTime
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int PrioMan_MP3CurrentTime(int* pSeconds)
{
	return MP3CurrentTime(pSeconds);
}

/*********************************************************************
* Function	PrioMan_MP3Seek
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int PrioMan_MP3Seek(int nSeconds)
{
	return MP3Seek(nSeconds);
}

/*********************************************************************
* Function	PrioMan_MP3SetVolume
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
int PrioMan_MP3SetVolume(int nVolume)
{
	return MP3SetVolume(nVolume);
}

/*********************************************************************
* Function	PrioMan_VolumeAdjusting
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/

#ifndef _EMULATE_
BOOL DHI_SetRingVolume(int ivalue);
#endif

int PrioMan_VolumeAdjusting(int nVolume)
{
#ifndef _EMULATE_

	// ���������Χ
	if (nVolume < PRIOMAN_VOLUME_LOWLEVEL || nVolume > PRIOMAN_VOLUME_HIGHLEVEL)
	{
		printf("\r\nPRIOMAN: ����������\r\n");
		return PRIOMAN_ERROR_VOLUME;
	}

	// ����������С
	switch (nVolume)
	{
	case PRIOMAN_VOLUME_LEVEL0:
		printf("\r\nPRIOMAN: ����������С = 0x%02X\r\n", LEVEL_ZERO);
		DHI_SetRingVolume(LEVEL_ZERO);
		break;
	case PRIOMAN_VOLUME_LEVEL1:
		printf("\r\nPRIOMAN: ����������С = 0x%02X\r\n", LEVEL_ONE);
		DHI_SetRingVolume(LEVEL_ONE);
		break;
	case PRIOMAN_VOLUME_LEVEL2:
		printf("\r\nPRIOMAN: ����������С = 0x%02X\r\n", LEVEL_TWO);
		DHI_SetRingVolume(LEVEL_TWO);
		break;
	case PRIOMAN_VOLUME_LEVEL3:
		printf("\r\nPRIOMAN: ����������С = 0x%02X\r\n", LEVEL_THREE);
		DHI_SetRingVolume(LEVEL_THREE);
		break;
	case PRIOMAN_VOLUME_LEVEL4:
		printf("\r\nPRIOMAN: ����������С = 0x%02X\r\n", LEVEL_FOUR);
		DHI_SetRingVolume(LEVEL_FOUR);
		break;
	case PRIOMAN_VOLUME_LEVEL5:
		printf("\r\nPRIOMAN: ����������С = 0x%02X\r\n", LEVEL_FIVE);
		DHI_SetRingVolume(LEVEL_FIVE);
		break;
	default:
		break;
	}

#endif

	return MMSYSERR_NOERROR;
}

/*********************************************************************
* Function	PrioMan_Vibration
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/

#ifndef _EMULATE_
extern int DHI_Vibration(BOOL bOn);
#endif

void PrioMan_Vibration(int nSec)
{
#ifndef _EMULATE_

	if (nSec > 0)
	{
		if (DHI_Vibration(TRUE) == -1)
		{
			printf("\r\nPRIOMAN: �����豸����\r\n");
			return;
		}

		// �����񶯶�ʱ��
		SetTimer(hPrioMan_HideWnd, PRIOMAN_TIMER_VIBRATION, nSec*1000, NULL);
	}
	else
	{
		// �ر��񶯶�ʱ��
		KillTimer(hPrioMan_HideWnd, PRIOMAN_TIMER_VIBRATION);

		DHI_Vibration(FALSE);
	}

#endif
}

/*********************************************************************
* Function	PrioMan_ReadWriteRAM
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_ReadWriteRAM(PPM_RAMFILE pRAM)
{
	int nPrioed/*��ǰ������*/, nPrioing/*��δ����*/;

	// �����������
	nPrioing = pRAM->nPriority;

	// ��鵱ǰ���ȼ�
	nPrioed = sPrioMan_RAMFILE.nPriority;
	if (nPrioed == PRIOMAN_PRIORITY_ZERO)
	{
		HWND hCallWnd;
		hCallWnd = pRAM->hCallWnd;

		// ��ǰ���������ڲ���
		sPrioMan_RAMFILE.hCallWnd = hCallWnd;
		sPrioMan_RAMFILE.nPriority = nPrioing;
		pRAM->nPriority = nPrioing;

		printf("\r\nPRIOMAN: ���ȼ���֤ͨ��\r\n");
		return PRIOMAN_ERROR_SUCCESS;
	}
	else if (nPrioed < nPrioing)
	{
		HWND hCalledWnd, hCallingWnd;
		hCalledWnd = sPrioMan_RAMFILE.hCallWnd;
		hCallingWnd = pRAM->hCallWnd;

		// ��ǰ�е����ȼ��������ڲ���
		sPrioMan_RAMFILE.hCallWnd = hCallingWnd;
		sPrioMan_RAMFILE.nPriority = nPrioing;
		pRAM->nPriority = nPrioing;

		// �������ȼ������Ƿ����
		if (hCalledWnd == NULL)
		{
			printf("\r\nPRIOMAN: FINDWINDOW����\r\n");
			return PRIOMAN_ERROR_FINDWINDOW;
		}

		// ����Ϣ�������ȼ�����
		SendMessage(hCalledWnd, PRIOMAN_MESSAGE_BREAKOFF, NULL, NULL);

		printf("\r\nPRIOMAN: �رյ����ȼ�\r\n");
		return PRIOMAN_ERROR_BREAKOFF;
	}

	printf("\r\nPRIOMAN: �����������ȼ�̫��\r\n");
	return PRIOMAN_ERROR_LOWPRIO;
}

/*********************************************************************
* Function	PrioMan_ResetRAM
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_ResetRAM(int nPrio)
{
	int nPrioed;

	// RAM���ݸ�λ
	nPrioed = sPrioMan_RAMFILE.nPriority;
	if (nPrioed == nPrio)
	{
		sPrioMan_RAMFILE.hCallWnd = NULL;
		sPrioMan_RAMFILE.nPriority = PRIOMAN_PRIORITY_ZERO;
		nPrioMan_Priority = PRIOMAN_PRIORITY_ZERO;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_IsPlaying
* Purpose   
* Parameter	
* Return	BOOL
* Remarks	
**********************************************************************/
static BOOL PrioMan_IsPlaying(int nPrio)
{
	int nPrioed;

	// ����Ƿ�����������
	nPrioed = sPrioMan_RAMFILE.nPriority;
	if (nPrioed > PRIOMAN_PRIORITY_ZERO && nPrioed != nPrio)
	{
		// ������������ڲ���
		return FALSE;
	}

	// ����������
	return TRUE;
}

/*********************************************************************
* Function	PrioMan_GetPriority
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_GetPriority(void)
{
	int nPrio;

	nPrio = nPrioMan_Priority;

	return nPrio;
}

/*********************************************************************
* Function	PrioMan_SetPriority
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
static void PrioMan_SetPriority(int nPrio)
{
	nPrioMan_Priority = nPrio;
}

/*********************************************************************
* Function	PrioMan_ResetPriority
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
static void PrioMan_ResetPriority(void)
{
	nPrioMan_Priority = PRIOMAN_PRIORITY_ZERO;
}

/*********************************************************************
* Function	PrioMan_GetType
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_GetType(char* pMusicName, unsigned char* pDataBuf, int nDataLen)
{
	unsigned char AudioType[9];
	FILE* pMusicFile;
	int nDataSize;

	if (pMusicName != NULL)
	{
		pMusicFile = fopen(pMusicName, "rb");
		if (pMusicFile == NULL)
			return PRIOMAN_RINGTYPE_UNKNOWN;

		fseek(pMusicFile, 0, SEEK_END);
		nDataSize = ftell(pMusicFile);
		if (nDataSize < 9)
		{
			fclose(pMusicFile);
			return PRIOMAN_RINGTYPE_UNKNOWN;
		}

		fseek(pMusicFile, 0, SEEK_SET);
		fread(AudioType, 1, sizeof(AudioType), pMusicFile);
		fclose(pMusicFile);
	}
	else if (pDataBuf != NULL && nDataLen != 0)
	{
		unsigned char* p;
		int i;

		nDataSize = nDataLen;
		if (nDataSize < 9)
			return PRIOMAN_RINGTYPE_UNKNOWN;

		p = pDataBuf;
		for (i=0; i<9; i++)
			AudioType[i] = *p++;
	}

	if (AudioType[0]=='R' && AudioType[1]=='I' &&
		AudioType[2]=='F' && AudioType[3]=='F')
	{
		if (nDataSize < 0x2C)
			return PRIOMAN_RINGTYPE_UNKNOWN;
		return PRIOMAN_RINGTYPE_WAVE;
	}
	else if (AudioType[0]=='M' && AudioType[1]=='T' &&
		AudioType[2]=='h' && AudioType[3]=='d')
	{
		return PRIOMAN_RINGTYPE_MIDI;
	}
	else if (AudioType[0]=='M' && AudioType[1]=='M' &&
		AudioType[2]=='M' && AudioType[3]=='D')
	{
		return PRIOMAN_RINGTYPE_MMF;
	}
	else if (AudioType[0]=='#' && AudioType[1]=='!' &&
		AudioType[2]=='A' && AudioType[3]=='M' &&
		AudioType[4]=='R' && AudioType[5]=='\n')
	{
		return PRIOMAN_RINGTYPE_AMR;
	}
	else if (AudioType[0]=='#' && AudioType[1]=='!' &&
		AudioType[2]=='A' && AudioType[3]=='M' &&
		AudioType[4]=='R' && AudioType[5]=='-' &&
		AudioType[6]=='W' && AudioType[7]=='B' &&
		AudioType[8]=='\n')
	{
		return PRIOMAN_RINGTYPE_AMR;
	}

	return PRIOMAN_RINGTYPE_UNKNOWN;
}

/*********************************************************************
* Function	PrioMan_GetExp
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_GetExp(char* pMusicName)
{
	int nLen;
	char* p;

	nLen = strlen(pMusicName);
	p = pMusicName + nLen;
	while ('.' != *p && p > pMusicName) {
		p--;
	}

	if (stricmp(p, ".wav") == 0)
		return PRIOMAN_RINGTYPE_WAVE;
	if (stricmp(p, ".mid") == 0)
		return PRIOMAN_RINGTYPE_MIDI;
	if (stricmp(p, ".midi") == 0)
		return PRIOMAN_RINGTYPE_MIDI;
	if (stricmp(p, ".mmf") == 0)
		return PRIOMAN_RINGTYPE_MMF;
	if (stricmp(p, ".amr") == 0)
		return PRIOMAN_RINGTYPE_AMR;
	if (stricmp(p, ".mp3") == 0)
		return PRIOMAN_RINGTYPE_MP3;

	return PRIOMAN_RINGTYPE_UNKNOWN;
}

/*********************************************************************
* Function	PrioMan_PlayWAVE
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PlayWAVE(PPM_PlayMusic pPM)
{
	int nResult;

	// ��ֹ˯��
	f_sleep_register(SOUNDPLAY);

	// ��������
	PrioMan_VolumeAdjusting(pPM->nVolume);

	// ˭���ڲ����У�
	printf("\r\nPRIOMAN: �����������ȼ� = %d\r\n", pPM->nPriority);

	nResult = WaveDecodeBegin(pPM->hCallWnd, pPM->pMusicName, pPM->pDataBuf, pPM->nDataLen, pPM->nPriority);
	if (nResult != PRIOMAN_ERROR_SUCCESS)
	{
		// Multimedia LIB must be responsible for playing...
		printf("\r\nPRIOMAN: ����WAVE�ļ�����\r\n");

		// Wave decorder end
		WaveDecodeEnd();

		// ����˯��
		f_sleep_unregister(SOUNDPLAY);

		return nResult;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_PlayMIDI
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PlayMIDI(PPM_PlayMusic pPM)
{
#ifdef _EMULATE_

	AllocBuffers(pPM->hCallWnd);
	AttemptFileOpen(pPM->hCallWnd, pPM->pMusicName);
	if (PrerollAndWait(pPM->hCallWnd))
		Start();

	return PRIOMAN_ERROR_SUCCESS;

#endif // _EMULATE_

	return PRIOMAN_ERROR_RINGTYPE;
}

/*********************************************************************
* Function	PrioMan_PlayMMF
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PlayMMF(PPM_PlayMusic pPM)
{
	return PRIOMAN_ERROR_RINGTYPE;
}

/*********************************************************************
* Function	PrioMan_PlayAMR
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PlayAMR(PPM_PlayMusic pPM)
{
	int nResult;

	// ��ֹ˯��
	f_sleep_register(SOUNDPLAY);

	// ��������
	PrioMan_VolumeAdjusting(pPM->nVolume);

	// ˭���ڲ����У�
	printf("\r\nPRIOMAN: �����������ȼ� = %d\r\n", pPM->nPriority);

	nResult = AMRDecodeBegin(pPM->hCallWnd, pPM->pMusicName, pPM->pDataBuf, pPM->nDataLen, pPM->nPriority);
	if (nResult != PRIOMAN_ERROR_SUCCESS)
	{
		// Multimedia LIB must be responsible for playing...
		printf("\r\nPRIOMAN: ����AMR�ļ�����\r\n");

		// AMR decorder end
		AMRDecodeEnd();

		// ����˯��
		f_sleep_unregister(SOUNDPLAY);

		return nResult;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_PlayMP3
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PlayMP3(PPM_PlayMusic pPM)
{
	int nResult;

	// ��ֹ˯��
	f_sleep_register(SOUNDPLAY);

	// ��������
	PrioMan_VolumeAdjusting(pPM->nVolume);

	// ˭���ڲ����У�
	printf("\r\nPRIOMAN: �����������ȼ� = %d\r\n", pPM->nPriority);

	nResult = MP3DecodeBegin(pPM->hCallWnd, pPM->pMusicName, pPM->nPriority);
	if (nResult != PRIOMAN_ERROR_SUCCESS)
	{
		// Multimedia LIB must be responsible for playing...
		printf("\r\nPRIOMAN: ����MP3�ļ�����\r\n");

		// MP3 decorder end
		MP3DecodeEnd();

		// ����˯��
		f_sleep_unregister(SOUNDPLAY);

		return nResult;
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_ReadCall
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
static void PrioMan_ReadCall(WPARAM wParam, LPARAM lParam)
{
	int nRingType;

	// ��ȡ��������
	nRingType = sPrioMan_CallMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		if (wParam == nPrioMan_Priority)
			WaveReadData(wParam, lParam);
		break;
	case PRIOMAN_RINGTYPE_MIDI:
		break;
	case PRIOMAN_RINGTYPE_MMF:
		break;
	case PRIOMAN_RINGTYPE_AMR:
		if (wParam == nPrioMan_Priority)
			AMRReadData(wParam, lParam);
		break;
	case PRIOMAN_RINGTYPE_MP3:
		if (wParam == nPrioMan_Priority)
			MP3ReadData(wParam, lParam);
		break;
	default:
		break;
	}
}

/*********************************************************************
* Function	PrioMan_WriteCall
* Purpose   
* Parameter	
* Return	void
* Remarks	
**********************************************************************/
static void PrioMan_WriteCall(WPARAM wParam, LPARAM lParam)
{
	int nRingType;

	// д����������
	nRingType = sPrioMan_CallMusic.nRingType;
	switch (nRingType)
	{
	case PRIOMAN_RINGTYPE_WAVE:
		if (wParam == nPrioMan_Priority)
			WaveWriteData(wParam, lParam);
		break;
	case PRIOMAN_RINGTYPE_MIDI:
		break;
	case PRIOMAN_RINGTYPE_MMF:
		break;
	case PRIOMAN_RINGTYPE_AMR:
		if (wParam == nPrioMan_Priority)
			AMRWriteData(wParam, lParam);
		break;
	case PRIOMAN_RINGTYPE_MP3:
		if (wParam == nPrioMan_Priority)
			MP3WriteData(wParam, lParam);
		break;
	default:
		break;
	}
}

/*********************************************************************
* Function	PrioMan_EndPlayWAVE
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_EndPlayWAVE(int nPrio, BOOL bCall, BOOL bBreak)
{
	if (!bCall)
	{
		// ��λ���ȼ�
		PrioMan_ResetRAM(nPrio);
	}

	// Wave decorder end
	WaveDecodeEnd();

	if (bBreak)
	{
		// �ȴ�һ��ʱ��
	}
	else
	{
		// ����˯��
		f_sleep_unregister(SOUNDPLAY);
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_EndPlayMIDI
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_EndPlayMIDI(int nPrio, BOOL bCall, BOOL bBreak)
{
#ifdef _EMULATE_

	if (!bCall)
	{
		// ��λ���ȼ�
		PrioMan_ResetRAM(nPrio);
	}

	// Midi decorder end
	Stop();
	FreeBuffers();

	if (bBreak)
	{
		// �ȴ�һ��ʱ��
	}
	else
	{
		// ����˯��
		f_sleep_unregister(SOUNDPLAY);
	}

	return PRIOMAN_ERROR_SUCCESS;

#endif // _EMULATE_

	return PRIOMAN_ERROR_RINGTYPE;
}

/*********************************************************************
* Function	PrioMan_EndPlayMMF
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_EndPlayMMF(int nPrio, BOOL bCall, BOOL bBreak)
{
	return PRIOMAN_ERROR_RINGTYPE;
}

/*********************************************************************
* Function	PrioMan_EndPlayAMR
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_EndPlayAMR(int nPrio, BOOL bCall, BOOL bBreak)
{
	if (!bCall)
	{
		// ��λ���ȼ�
		PrioMan_ResetRAM(nPrio);
	}

	// AMR decorder end
	AMRDecodeEnd();

	if (bBreak)
	{
		// �ȴ�һ��ʱ��
	}
	else
	{
		// ����˯��
		f_sleep_unregister(SOUNDPLAY);
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_EndPlayMP3
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_EndPlayMP3(int nPrio, BOOL bCall, BOOL bBreak)
{
	if (!bCall)
	{
		// ��λ���ȼ�
		PrioMan_ResetRAM(nPrio);
	}

	// MP3 decorder end
	MP3DecodeEnd();

	if (bBreak)
	{
		// �ȴ�һ��ʱ��
	}
	else
	{
		// ����˯��
		f_sleep_unregister(SOUNDPLAY);
	}

	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_PauseWAVE
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PauseWAVE(void)
{
	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_PauseMIDI
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PauseMIDI(void)
{
	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_PauseMMF
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PauseMMF(void)
{
	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_PauseAMR
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PauseAMR(void)
{
	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_PauseMP3
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_PauseMP3(void)
{
	return MP3Pause();
}

/*********************************************************************
* Function	PrioMan_ResumeWAVE
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_ResumeWAVE(void)
{
	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_ResumeMIDI
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_ResumeMIDI(void)
{
	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_ResumeMMF
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_ResumeMMF(void)
{
	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_ResumeAMR
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_ResumeAMR(void)
{
	return PRIOMAN_ERROR_SUCCESS;
}

/*********************************************************************
* Function	PrioMan_ResumeMP3
* Purpose   
* Parameter	
* Return	int
* Remarks	
**********************************************************************/
static int PrioMan_ResumeMP3(void)
{
	return MP3Restart();
}

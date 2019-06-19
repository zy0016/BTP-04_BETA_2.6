/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : prioman.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _PRIOMAN_H_
#define _PRIOMAN_H_

// �����������ȼ�
#define PRIOMAN_PRIORITY_PHONE		100		// ����
#define PRIOMAN_PRIORITY_ALARM		95		// ����
#define PRIOMAN_PRIORITY_CALENDAR	94		// �ճ�
#define PRIOMAN_PRIORITY_POWERON	90		// ����
#define PRIOMAN_PRIORITY_POWEROFF	89		// �ػ�
#define PRIOMAN_PRIORITY_BEEPSOUND	88		// Beep Sound
#define PRIOMAN_PRIORITY_SMSAPP		85		// ����Ϣ
#define PRIOMAN_PRIORITY_MMSAPP		84		// ����
#define PRIOMAN_PRIORITY_EMAIL		83		// E-Mail
#define PRIOMAN_PRIORITY_BLUETOOTH	82		// ����
#define PRIOMAN_PRIORITY_PUSHINFO	81		// PUSH��Ϣ
#define PRIOMAN_PRIORITY_SIMTOOLKIT	75		// STK
#define PRIOMAN_PRIORITY_WAPAPP		70		// �����
#define PRIOMAN_PRIORITY_MP3APP		65		// MP3����
#define PRIOMAN_PRIORITY_MUSICMAN	60		// ���ֹ�����
#define PRIOMAN_PRIORITY_CAMERA		55		// �����
#define PRIOMAN_PRIORITY_KJAVA		50		// KJAVA
#define PRIOMAN_PRIORITY_PUBLIC		45		// ����Ԥ��
#define PRIOMAN_PRIORITY_SETTING	40		// ��������
#define PRIOMAN_PRIORITY_CONNECT	35		// ��ͨ��ʾ��
#define PRIOMAN_PRIORITY_50TIPS		34		// 50����ʾ��
#define PRIOMAN_PRIORITY_WARNING	33		// ������
#define PRIOMAN_PRIORITY_KEYBOARD	32		// ������
#define PRIOMAN_PRIORITY_TSCREEN	31		// ������
#define PRIOMAN_PRIORITY_ZERO		0		// �����ȼ�

// ��������
#define PRIOMAN_RINGTYPE_WAVE		1		// Wave
#define PRIOMAN_RINGTYPE_MIDI		2		// Midi
#define PRIOMAN_RINGTYPE_MMF		3		// MMF
#define PRIOMAN_RINGTYPE_AMR		4		// AMR
#define PRIOMAN_RINGTYPE_MP3		5		// MP3
#define PRIOMAN_RINGTYPE_UNKNOWN	6		// Unknown

// ��������
#define PRIOMAN_VOLUME_DEFAULT		3		// 3��
#define PRIOMAN_VOLUME_MUTE			0		// 0��
#define PRIOMAN_VOLUME_LOWLEVEL		1		// 1��
#define PRIOMAN_VOLUME_HIGHLEVEL	5		// 5��

#define PRIOMAN_VOLUME_LEVEL0		0		// 0��
#define PRIOMAN_VOLUME_LEVEL1		1		// 1��
#define PRIOMAN_VOLUME_LEVEL2		2		// 2��
#define PRIOMAN_VOLUME_LEVEL3		3		// 3��
#define PRIOMAN_VOLUME_LEVEL4		4		// 4��
#define PRIOMAN_VOLUME_LEVEL5		5		// 5��

// �����ļ�
#define PRIOMAN_RINGFILE_POWERON	"/rom/prioman/startup.amr"
#define PRIOMAN_RINGFILE_POWEROFF	"/rom/prioman/shutdown.amr"
#define PRIOMAN_RINGFILE_BEEPSOUND	"/rom/prioman/beep.wav"
#define PRIOMAN_RINGFILE_WARNING	"/rom/prioman/ding.wav"
#define PRIOMAN_RINGFILE_KEYBOARD	"/rom/prioman/key.wav"
#define PRIOMAN_RINGFILE_TSCREEN	"/rom/prioman/touch.wav"

// ��Ϣ����
#define PRIOMAN_MESSAGE				WM_USER+2881
#define PRIOMAN_MESSAGE_READDATA	PRIOMAN_MESSAGE+1		// ������
#define PRIOMAN_MESSAGE_WRITEDATA	PRIOMAN_MESSAGE+2		// д����
#define PRIOMAN_MESSAGE_PLAYOVER	PRIOMAN_MESSAGE+3		// ����
#define PRIOMAN_MESSAGE_BREAKOFF	PRIOMAN_MESSAGE+4		// �ж�

// ��ʱ������
#define PRIOMAN_TIMER				927
#define PRIOMAN_TIMER_CALLMUSIC		PRIOMAN_TIMER+1			// ��������
#define PRIOMAN_TIMER_ENDCALL		PRIOMAN_TIMER+2			// ��������
#define PRIOMAN_TIMER_VIBRATION		PRIOMAN_TIMER+3			// ��

// ������Ϣ
#define PRIOMAN_ERROR_SUCCESS		1		// �ɹ�
#define PRIOMAN_ERROR_PARAMETER		2		// �����������
#define PRIOMAN_ERROR_LOWPRIO		3		// �����������ȼ�̫��
#define PRIOMAN_ERROR_UNKNOWNPRIO	4		// δ֪�������������ȼ�
#define PRIOMAN_ERROR_RINGTYPE		5		// �������ʹ���
#define PRIOMAN_ERROR_OPENFAIL		6		// ���ļ�����
#define PRIOMAN_ERROR_ALLOCMEM		7		// �����ڴ����
#define PRIOMAN_ERROR_DECORDER		8		// ��������������
#define PRIOMAN_ERROR_GETPLAYTIME	9		// ��ȡ����ʱ�����
#define PRIOMAN_ERROR_GETAUDIOINFO	10		// ��ȡ������Ϣ����
#define PRIOMAN_ERROR_GETDECDATA	11		// ��ȡ�������ݴ���
#define PRIOMAN_ERROR_WAVEFORMAT	12		// WAVE��ʽ����
#define PRIOMAN_ERROR_WAVEOPEN		13		// WAVE�򿪴���
#define PRIOMAN_ERROR_WAVECLOSE		14		// WAVE�رմ���
#define PRIOMAN_ERROR_SETVOLUME		15		// ������������
#define PRIOMAN_ERROR_PREPARE		16		// ׼�����Ŵ���
#define PRIOMAN_ERROR_WRITEOUT		17		// д�豸����
#define PRIOMAN_ERROR_RESET			18		// RESET�豸����
#define PRIOMAN_ERROR_CREATERAM		19		// ����RAM�ļ�����
#define PRIOMAN_ERROR_READRAM		20		// ��ȡRAM�ļ�����
#define PRIOMAN_ERROR_WRITERAM		21		// д��RAM�ļ�����
#define PRIOMAN_ERROR_BREAKOFF		22		// ��ϴ���
#define PRIOMAN_ERROR_FINDWINDOW	23		// FINDWINDOW����
#define PRIOMAN_ERROR_ISPLAYING		24		// ��ǰ�н������ڲ���
#define PRIOMAN_ERROR_VOLUME		25		// ����������
#define PRIOMAN_ERROR_MUTEMODE		26		// ����ģʽ
#define PRIOMAN_ERROR_UNKNOWN		0		// δ֪�Ĵ���

#define PRIOMAN_PLAYMUSIC			1		// �ӿ�1
#define PRIOMAN_CALLMUSIC			2		// �ӿ�2

// �������Žṹ
typedef struct tagPM_PlayMusic
{
	HWND hCallWnd;
	int nPriority;
	int nRingType;
	int nVolume;
	int nRepeat;
	char* pMusicName;
	unsigned char* pDataBuf;
	int nDataLen;
}PM_PlayMusic, *PPM_PlayMusic;

// �ӿ�1�ļ��б�
typedef struct tagPM_MusicList {
	char* pMusicName;
	int nPriority;
	int nRingType;
	int nVolume;
}PM_MusicList, *PPM_MusicList;

// �ӿ�2�ļ��б�
typedef struct tagPM_CallList {
	int nPriority;
	int nRepeat;
	int nRingType;
	int nVolume;
}PM_CallList, *PPM_CallList;

// RAM���ݽṹ
typedef struct tagPM_RAMFILE {
	HWND hCallWnd;
	int nPriority;
}PM_RAMFILE, *PPM_RAMFILE;

////////////////////////////////////////////////////////////////////////////
// �Գ���������ṩ�Ľӿ�
////////////////////////////////////////////////////////////////////////////

/*
�ӿ�˵����
���� --�������ȼ��¼����źŵƲ�����
���� �ޡ�
����ֵ������ɹ�������TRUE�����ʧ�ܣ�����FALSE��
*/
BOOL PrioMan_Initialize(void);

/*
�ӿ�˵����
���� --�ͷ����ȼ��¼����źŵƲ�����
���� �ޡ�
����ֵ������ɹ�������TRUE�����ʧ�ܣ�����FALSE��
*/
BOOL PrioMan_Uninitialize(void);

////////////////////////////////////////////////////////////////////////////
// �����ṩ�Ľӿ�1
////////////////////////////////////////////////////////////////////////////

/*
�ӿ�˵����
���� --��������׼����
���� pPM�������������Ϣ�ṹ���������ݽṹһ�ڲ鿴��ϸ��Ϣ��
����ֵ������ɹ������� PRIOMAN_ERROR_SUCCESS�����ں궨��һ�ڲ鿴��ϸ��Ϣ��
*/
int  PrioMan_PlayMusic(PPM_PlayMusic pPM);

/*
�ӿ�˵����
���� --��ȡ�������ݡ�
���� wParam��δʹ�ã�������
	 lParam�����ݿ顣
����ֵ���ޡ�
*/
void PrioMan_ReadData(WPARAM wParam, LPARAM lParam);

/*
�ӿ�˵����
���� --д���������ݡ�
���� wParam��δʹ�ã�������
	 lParam�����ݿ顣
����ֵ���ޡ�
*/
void PrioMan_WriteData(WPARAM wParam, LPARAM lParam);

/*
�ӿ�˵����
���� --�����������š�
���� nPrio��App���ȼ�����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
����ֵ������ɹ������� PRIOMAN_ERROR_SUCCESS�����ں궨��һ�ڲ鿴��ϸ��Ϣ��
*/
int  PrioMan_EndPlayMusic(int nPrio);

/*
�ӿ�˵����
���� --����������š�
���� nPrio��App���ȼ�����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
����ֵ������ɹ������� PRIOMAN_ERROR_SUCCESS�����ں궨��һ�ڲ鿴��ϸ��Ϣ��
*/
int  PrioMan_BreakOffMusic(int nPrio);

/*
�ӿ�˵����
���� --��ͣ�������š�
���� �ޡ�
����ֵ���ɹ�����0��
*/
int  PrioMan_PauseMusic(void);

/*
�ӿ�˵����
���� --�����������š�
���� �ޡ�
����ֵ���ɹ�����0��
*/
int  PrioMan_ResumeMusic(void);

////////////////////////////////////////////////////////////////////////////
// �����ṩ�Ľӿ�2
////////////////////////////////////////////////////////////////////////////

/*
�ӿ�˵����
���� --ֱ���������š�
���� nPrio��App���ȼ�����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
     nRep��ѭ��������0Ϊ����ѭ����
����ֵ������ɹ�������TRUE�����ʧ�ܣ�����FALSE��
*/
BOOL PrioMan_CallMusic(int nPrio, int nRep);

/*
�ӿ�˵����
���� --ֱ������������չ��
���� nPrio��App���ȼ�����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
     nTimeOut����ʱʱ�䣬����ơ�
����ֵ������ɹ�������TRUE�����ʧ�ܣ�����FALSE��
*/
BOOL PrioMan_CallMusicEx(int nPrio, int nTimeOut);

/*
�ӿ�˵����
���� --ֱ���������š�
���� pMusicName�������ļ���
     nPrio��App���ȼ�����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
     nRep��ѭ��������0Ϊ����ѭ����
     nVol��������С����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
����ֵ������ɹ�������TRUE�����ʧ�ܣ�����FALSE��
*/
BOOL PrioMan_CallMusicFile(char* pMusicName, int nPrio, int nRep, int nVol);

/*
�ӿ�˵����
���� --����ֱ�Ӳ��š�
���� nPrio��App���ȼ�����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
     bStop���Ƿ���ȫֹͣ���ţ���ΪFALSEʱֻ��������ѭ����
����ֵ������ɹ�������TRUE�����ʧ�ܣ�����FALSE��
*/
BOOL PrioMan_EndCallMusic(int nPrio, BOOL bStop);

/*
�ӿ�˵����
���� --���ֱ�Ӳ��š�
���� nPrio��App���ȼ�����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
����ֵ������ɹ�������TRUE�����ʧ�ܣ�����FALSE��
*/
BOOL PrioMan_BreakOffCall(int nPrio);

////////////////////////////////////////////////////////////////////////////
// ��MP3Ӧ���ṩ�Ľӿ�
////////////////////////////////////////////////////////////////////////////

/*
�ӿ�˵����
���� --��ȡMP3�ܲ���ʱ�䡣
���� �ޡ�
����ֵ���ɹ�����>0������ʧ�ܷ���-1��
*/
int  PrioMan_MP3GetTotalTime(void);

/*
�ӿ�˵����
���� --��ȡMP3��ǰʱ�䡣
���� pSeconds��������
����ֵ���ɹ�����0��ʧ�ܷ���-1��
*/
int  PrioMan_MP3CurrentTime(int* pSeconds);

/*
�ӿ�˵����
���� --��λMP3���Ž��ȡ�
���� nSeconds��������
����ֵ�� �ɹ�����0��ʧ�ܷ���-1��
*/
int  PrioMan_MP3Seek(int nSeconds);

/*
�ӿ�˵����
���� --����MP3������
���� nVolume��������С����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
����ֵ���ɹ�����0��
*/
int  PrioMan_MP3SetVolume(int nVolume);

////////////////////////////////////////////////////////////////////////////
// ����
////////////////////////////////////////////////////////////////////////////

/*
�ӿ�˵����
���� --����������
���� nVolume��������С����priomanͳһ���塣���ں궨��һ�ڲ鿴��ϸ��Ϣ��
����ֵ���ɹ�����0��
*/
int  PrioMan_VolumeAdjusting(int nVolume);

/*
�ӿ�˵����
���� --�ֻ��񶯡�
���� nSec����������Ϊ0ʱ�����񶯡�
����ֵ���ޡ�
*/
void PrioMan_Vibration(int nSec);

#endif // _PRIOMAN_H_

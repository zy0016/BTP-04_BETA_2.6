
#ifndef _VAUDIOUIFUNC_H_
#define _VAUDIOUIFUNC_H_

typedef void(AUDIOMSG_PROCESSFUNC)(AUDIOMSG_STRUCT *AudioMsg);

BOOL VAUDIO_Init();
void VAUDIO_Done();
void VAUDIO_RegCallback4AudioMsg(AUDIOMSG_PROCESSFUNC* pCallback);
void VAUDIO_UnRegCallback4AudioMsg(AUDIOMSG_PROCESSFUNC* pCallback);
void VAUDIO_OpenStreamComplete(void);
void VAUDIO_CloseStreamComplete(void);
void VAUDIO_BluetoothStreamState(BOOL     	iscapturestream,  BOOL ispresent);
AUDIOMSG_STRUCT* VAUDIO_AllocateCaptureDataBuff(DWORD length);
void VAUDIO_FreeCaptureDataBuff(AUDIOMSG_STRUCT*          p);
BOOL VAUDIO_SendCaptureStreamData(
	AUDIOMSG_STRUCT* 	paudiomsg, 
	DWORD 				length, 
	DWORD* 				nwritten);
void VAUDIO_EnableReportStreamState(void);
void ProcessAudioMsg(AUDIOMSG_STRUCT *AudioMsg);
BOOL  VAUDIO_SetBlueletAudio2DefaultPBDevice(void);
void  VAUDIO_RestoreDefaultPBDevice(void);
BOOL  VAUDIO_SetBlueletAudio2DefaultRecordDevice(void);
void  VAUDIO_RestoreDefaultRecordDevice(void);
BOOL  VAUDIO_SetPreferredOnly(void);
void  VAUDIO_RestorePreferredOnly(void);
UCHAR vaudio_SwitchUSBAudio(char *pDeviceName);

#ifndef MAXPNAMELEN
#define MAXPNAMELEN      32     /* max product name length (including NULL) */
#endif

typedef struct VAUDIO_RegInfo{
	char 	oldplaydevice[MAXPNAMELEN];
	char 	oldrecoderdevice[MAXPNAMELEN];
	DWORD 	oldpreferonly;
	BOOL	bsafeExit_PB;	 	//0 not safeexit, 1 safe exit; playback device
	BOOL      bsafeExit_RC;	 	//0 not safeexit, 1 safe exit; recoder device
	UCHAR	Cd_digital_saftexit;	//0 not safeexit&not set. 1 not safeexit&set. other(>1) safeexit
}VAUDIO_RegInfoStru;

typedef struct _VAUDIO_INSTANCE{
	BOOL		g_audioexitthread;
	HANDLE		g_audiohDevice;
	HANDLE		g_audiohEvent;
	HANDLE		g_hexitRevmsg;

	SHAREDBUFFER_STRUCT*	g_sharebuffer;
	BOOL					g_isnewdriver;
	UINT					g_period ;
	VAUDIO_RegInfoStru		g_VaudioRegInfo;

	int				g_vaudio_refcount;
	struct BtList* 	g_vaudio_callback_list;
	char g_BlueAudioDeviceName[60];
} VAUDIO_INSTANCE,* PVAUDIO_INSTANCE;

BOOL VAUDIO_Init_New(PVAUDIO_INSTANCE pVAudioInst);
void ProcessAudioMsg_New(AUDIOMSG_STRUCT *AudioMsg,PVAUDIO_INSTANCE pVAudioInst);
void VAUDIO_Done_New(PVAUDIO_INSTANCE pVAudioInst);
void VAUDIO_RegCallback4AudioMsg_New(PVAUDIO_INSTANCE pVAudioInst, AUDIOMSG_PROCESSFUNC* pCallback);
void VAUDIO_UnRegCallback4AudioMsg_New(PVAUDIO_INSTANCE pVAudioInst, AUDIOMSG_PROCESSFUNC* pCallback);
void VAUDIO_OpenStreamComplete_New(PVAUDIO_INSTANCE pVAudioInst);
void VAUDIO_CloseStreamComplete_New(PVAUDIO_INSTANCE pVAudioInst);
void VAUDIO_BluetoothStreamState_New(PVAUDIO_INSTANCE pVAudioInst, BOOL     	iscapturestream,  BOOL ispresent);
BOOL VAUDIO_SendCaptureStreamData_New(
	PVAUDIO_INSTANCE pVAudioInst,
	AUDIOMSG_STRUCT* 	paudiomsg, 
	DWORD 				length, 
	DWORD* 				nwritten);
void VAUDIO_EnableReportStreamState_New(PVAUDIO_INSTANCE pVAudioInst);
BOOL  VAUDIO_SetBlueletAudio2DefaultPBDevice_New(PVAUDIO_INSTANCE pVAudioInst);
void  VAUDIO_RestoreDefaultPBDevice_New(PVAUDIO_INSTANCE pVAudioInst);
BOOL  VAUDIO_SetBlueletAudio2DefaultRecordDevice_New(PVAUDIO_INSTANCE pVAudioInst);
void  VAUDIO_RestoreDefaultRecordDevice_New(PVAUDIO_INSTANCE pVAudioInst);
BOOL  VAUDIO_SetPreferredOnly_New(PVAUDIO_INSTANCE pVAudioInst);
void  VAUDIO_RestorePreferredOnly_New(PVAUDIO_INSTANCE pVAudioInst);

#endif  // _VAUDIOUIFUNC_H_

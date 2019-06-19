#ifndef VAUDIOSYSDEP_H
#define VAUDIOSYSDEP_H

BOOL VAUDIO_InitSysInfo(void);
void VAUDIO_DoneSysInfo(void);
BOOL VAUDIO_SendMsg2Drv( 
   PVOID lpInBuffer,             /*input data buffer*/
   DWORD nInBufferSize           /*size of input data buffer*/
);
BOOL VAUDIO_SendData2Drv( 
   PVOID lpInBuffer,             /*input data buffer*/
   DWORD nInBufferSize,           /*size of input data buffer*/
   DWORD* ntransfered);

BOOL  SetBlueletAudio2DefaultPBDevice(void);
void  RestoreDefaultPBDevice(void);
BOOL  SetBlueletAudio2DefaultRecordDevice(void);
void  RestoreDefaultRecordDevice(void);
BOOL  SetPreferredOnly(void);
void  RestorePreferredOnly(void);

UCHAR SwitchUSBAudio(char *pDeviceName);


BOOL VAUDIO_InitSysInfo_New(PVAUDIO_INSTANCE pVAudioInst);
void VAUDIO_DoneSysInfo_New(PVAUDIO_INSTANCE pVAudioInst);
BOOL VAUDIO_SendMsg2Drv_New( 
   PVAUDIO_INSTANCE pVAudioInst,
   PVOID lpInBuffer,             /*input data buffer*/
   DWORD nInBufferSize           /*size of input data buffer*/
);

BOOL VAUDIO_SendData2Drv_New( 
   PVAUDIO_INSTANCE pVAudioInst,
   PVOID lpInBuffer,             /*input data buffer*/
   DWORD nInBufferSize,           /*size of input data buffer*/
   DWORD* ntransfered);
BOOL  SetBlueletAudio2DefaultPBDevice_New(PVAUDIO_INSTANCE pVAudioInst);
BOOL  SetBlueletAudio2DefaultRecordDevice_New(PVAUDIO_INSTANCE pVAudioInst);
BOOL  SetPreferredOnly_New(PVAUDIO_INSTANCE pVAudioInst);
void  RestorePreferredOnly_New(PVAUDIO_INSTANCE pVAudioInst);

#endif

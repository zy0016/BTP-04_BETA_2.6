#ifndef SWRAPSYSDEP_H
#define SWRAPSYSDEP_H

#undef  SWRAP_DEP_H
#define SWRAP_DEP_H

HANDLE InitSysInfo(void);
void DoneSysInfo(HANDLE h);

BOOL IOCTL(HANDLE hDevice,DWORD dwIoControlCode,PVOID lpInBuffer,DWORD nInBufferSize);

BOOL Search_Comport(void);
void ReceiveMsgFromVcomm(void);
void OpenProcessing(UCHAR com_index);
void CloseProcessing(UCHAR com_index);

BOOL ParseLAPData( UCHAR com_index,UCHAR *buf,WORD datalen);

#endif

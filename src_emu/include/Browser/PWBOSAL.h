/**************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   :  wap browser
 *
 * Purpose  : Ϊ���������C�⺯��
 *
\**************************************************************************/

#ifndef WB_OSFILE_H
#define WB_OSFILE_H

#ifndef _WBHUGE_
#ifdef _C166
#define _WBHUGE_  _huge
#else
#define _WBHUGE_ 
#endif
#else
#endif

typedef void* WB_HFILE;

#ifndef NULL
#  define NULL  0
#endif


#define INVALID_WBHFILE  ( (WB_HFILE)-1 )

enum FileOpenMode
{
	FOM_READ   = 0x01,  //����ʽ���ļ�
	FOM_WRITE  = 0x02,  //д��ʽ���ļ�
	FOM_CREATE = 0x04   //������ʽ��
};


enum FileSeekMode
{
	WB_SEEK_SET,    //��λ����ʼ
	WB_SEEK_CUR,    //��λ�ڵ�ǰλ��
	WB_SEEK_END     //��λ���ļ�ĩβ
};

enum
{
	WB_SPACE_NOTAVAILABLE = 0,
	WB_SPACE_AVAILABLE = 1
};

#ifdef __cplusplus
extern "C"{
#endif


WB_HFILE WB_OpenFile(char *filename, unsigned char mode);
void WB_CloseFile(WB_HFILE hFile);

unsigned long WB_SeekFile(WB_HFILE hFile, long lDistance, unsigned int origin);

unsigned long WB_ReadFile(WB_HFILE hFile, void _WBHUGE_ * buffer, unsigned long nBytes);
unsigned long WB_WriteFile(WB_HFILE hFile, void _WBHUGE_ * buffer, unsigned long nBytes);

//unsigned long WB_ReadFileToBuffer(char *filename, void _WBHUGE_ * *buffer);
unsigned long WB_WriteBufferToFile(char *filename, void _WBHUGE_ * buf, unsigned long buflen);

unsigned long WB_GetFileSize(WB_HFILE hFile);

int WB_IsFlashAvailable(unsigned long lSize);

int WB_DeleteFile(char *filename);

int WB_CreateDirectory(char *dir);
//remove the dir and it's sub files
int WB_RemoveDirectory(char *path);



/*****************************************************************
*
*����Ϊ�ַ���ת����صĺ����ӿ�,����ʵ����WBDrawDev.c��
*
*****************************************************************/

enum WB_CODEPAGE
{
    WB_CP_ACP,    
    WB_CP_UTF8 
};

int		WB_MultiByteToUTF8(int CodePage, unsigned long dwFlags, const char* lpMultiByteStr, 
					    int cchMultiByte, char* lpUtf8CharStr,  int nUtf8Len);

int		WB_UTF8ToMultiByte(int CodePage, unsigned long dwFlags, 
						   const char *lpUtf8CharStr, int cUtf8Char, 
					       char *lpMultiByteStr, int cchMultiByte, 
						   const char *lpDefaultChar, int *lpUsedDefaultChar);


int     WB_WideCharToMultiByte(int CodePage, unsigned long dwFlags, const unsigned short* lpWideCharStr, 
						int cchWideChar, char* lpMultiByteStr, int cchMultiByte,
						const char* lpDefaultChar, int *lpUsedDefaultChar);



#define WBTRANS_INVALIDETIMER        ( (unsigned long)-1 )

typedef void (* WB_DevTIMERPROC)(unsigned long TimerID, void* param1, void* param2);
typedef void (*ECHO_TIMER)(unsigned long TimerID, void* param1, void* param2);

int WB_DevInitTimer(void);
unsigned long WB_DevSetTimer(int uElapse,  WB_DevTIMERPROC pTimerFunc, void* param1, void* param2);
int WB_DevKillTimer(unsigned long  uIDEvent);
int WB_DevClearTimer(void);

#ifdef _DEBUG

void WB_MsgOut(char *str);
void WB_MsgOut1( char * format, unsigned long param1 );
void WB_MsgOut2( char *format, unsigned long param1, unsigned long param2);
void WB_MsgOut3( char *format, unsigned long param1, unsigned long param2, unsigned long param3);

void WB_DataOut(const unsigned char *pData, unsigned short Length,
						  const unsigned char *pName);

#else

#define WB_MsgOut
#define WB_MsgOut1
#define WB_MsgOut2
#define WB_MsgOut3

#define WB_DataOut

#endif

void WB_InitLog(void);
void WB_EndLog(void);

//��ɾ������(((

void*   WB_GetMemoryDC(void *hdc, int width, int height);
void    WB_FreeMemDC(void* hdc);
int     WB_BitBlt(void* hdcDest, int nXDest, int nYDest, int nWidth, int nHeight, 
			  void* hdcSrc, int nXSrc, int nYSrc);
//))))��ɾ������

/*****************************************************************
*
*����Ϊ�ַ���ת����صĺ����ӿ�,����ʵ����WBDrawDev.c��
*
*****************************************************************/
/*
enum WB_CODEPAGE{
    WB_CP_ACP,    
    WB_CP_UTF8 
};
*/
int		WB_MultiByteToUTF8(int CodePage, unsigned long dwFlags, const char* lpMultiByteStr, 
					    int cchMultiByte, char* lpUtf8CharStr,  int nUtf8Len);

int		WB_UTF8ToMultiByte(int CodePage, unsigned long dwFlags, 
						   const char *lpUtf8CharStr, int cUtf8Char, 
					       char *lpMultiByteStr, int cchMultiByte, 
						   const char *lpDefaultChar, int *lpUsedDefaultChar);


int     WB_WideCharToMultiByte(int CodePage, unsigned long dwFlags, const unsigned short* lpWideCharStr, 
						int cchWideChar, char* lpMultiByteStr, int cchMultiByte,
						const char* lpDefaultChar, int *lpUsedDefaultChar);

#ifdef __cplusplus
}
#endif


#endif


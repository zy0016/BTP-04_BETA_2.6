    /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "window.h"

#include "hpimage.h"
#include "string.h"
#include "winpda.h"
#include "setting.h"
#include "dialer.h"
#include "str_plx.h"
#include "PhonebookExt.h"
//#include "vcaglb.h"
#include "stdio.h"
#include "stdlib.h"



void  APP_CreatePBEnv(void)
{
    return;
}

BOOL	APP_PBCallPhone()
{
    return TRUE;
}

//void PositioningSetting(){}


void AlarmInterfaceForProgman()
{
}

void CreateDinputSoftKeyboard()
{
}


void _assert()
{
}
void ASSERT()
{
}

/*******************************************************************************/
// Implementation of PLXPrintf
typedef char * charptr;
typedef int (*func_ptr)(void *, char *, int);
extern int _uprint( func_ptr out_char, void * where, charptr ctrl, va_list argp );

static int _fputs (void * fp, char * str, int count)
{
	fwrite (str, count, 1, fp);
	return count;
}
void PLXPrintf( const char *fmt, ... )
{
    va_list varg;
    ASSERT(fmt != NULL);
	
	va_start( varg, fmt );
	/* 2 for stderr */
	_uprint(_fputs, stdout, (/*charptr*/char*)fmt, varg);
	va_end( varg );
	/* Send 0 for flush the buffer */
	fflush( stdout );
}
/*********************************************************************************/
void APP_SendMailAsBody()
{
}

/*
void msg_registerrefreshmailbox()
{
}
*/


void EndPicBrowser()
{}
/*
void CallMMSEditWnd()
{}*/

BOOL APP_SaveToPhoneBook(HWND hParent, const char* pPhoneNo)
{
	return 0;
}
Test_AppControl(){}
void SaveFile()
{
}
void CallPenCalibrate()
{
}
/*
void GetLanguage()
{
}*/

MYear_FormatLunar()
{}

T9PhnImeWinMain(){}
//APP_EditMMS(){}
//CallheadphoneWindow(){}
GetDefaultRing(){}
void    GetReplyData(char * ringname,LPARAM lParam ,unsigned int buflen){}
void    GetRingShowName(const char * ringname,char * cshowname){}
BOOL    CallRingSelect(const FUNCRINGSELECT * ringselect)
{
	return 1;
}
//CallCreateNewMode(){}
//CallRingSetWindow(){}
//CallProfileName(){}
#ifndef  _EMULATE_
//KickDog(){}
PlayBack(){}
SEF_PreviewRingFromData(){}
AS_StopPlay(){}
GetWinDebugInfo(){}
Setup_ShowPenCalib(){return 1;}
ShowPenCalibWindow(){}
#endif


mp3DecoderCreate(){}
mp3DecoderDestroy(){}
mp3DecoderSeek(){}
mp3DecoderGetTotalTime(){}
mp3DecoderGetData(){}
p3DecoderGetAudioInfo(){}
mp3DecoderGetAudioInfo(){}

//BOOL SSPLXVerifyPassword(HWND hParentHwnd, PCSTR szCaption, PCSTR szPrompt, PSTR szPwd, DWORD dwSizeRange, //int nSize, 
//                      PCSTR szOk, PCSTR szCancel)
//{
//	return 1;
//}

// SMS_OnRecvPort(){}


//void APP_GetNameByPhone(){}


_dirps(){}
_restore(){}

//8-17
/*
BOOL Get_DeviceLocalAddr( char * addr)
{
	return FALSE;  
}*/

//PLXConfirmWinEx(){}

/*
#ifndef _EMULATE_
int vCard_Reader ( unsigned char* pData, int Datalen, vCard_Obj** ppObj )
{
	*ppObj = NULL;
}

int vCal_Reader ( unsigned char * pData, int Datalen, vCal_Obj ** ppObj )
{
	*ppObj = NULL;
}
int vCard_Clear ( int type, void* pbuf )
{
	return 0;
}

int vCal_Clear ( int type, void* pbuf )
{
	return 0;
}

int vCard_Writer( vCard_Obj* pObj, unsigned char** ppbuf, int *buflen )
{
	
	*ppbuf = NULL;
	
	*buflen = 0;
	
	return 1;
}

#endif*/

/*
BOOL WAP_Public_Setting(HWND hParent)
{
	return FALSE;
}
*/
void JAM_Install_Finalize()
{
}

void JAM_Install_Init()
{
}

void Jam_SendDialerStatus()
{
}

/*
void CallGameHelpWnd(HWND hwnd, char* str)
{
}
*/
//mp3DecoderCreate(){}
//mp3DecoderDestroy(){}
//mp3DecoderGetTotalTime(){}
mp3DecoderCurrentTime(){}
//mp3DecoderSeek(){}
//mp3DecoderGetData(){}
//mp3DecoderGetAudioInfo(){}
#ifndef __NETWORK_H__
#define __NETWORK_H__
#include "me_wnd.h"

#define  WM_REVEALOWNNR_CHANGED		(WM_USER+200)  //This message used in RealOwnNum window and Network service main Window
#define  ME_MSG_GETLASTERROR		(WM_USER+201)  //Used for get last ME's error
extern HWND GetNetworkFrameWnd();

typedef struct tagErrorInfo
{
	int iErrorCode;
	char strErrorInfo[50];
}ERRORINFO;

void GetErrorString(ME_EXERROR ME_Error,char *strErrorInfo);

#endif

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : StkMain.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _STK_MAIN_H_
#define _STK_MAIN_H_

#define	IDC_OK			100
#define	IDC_BACK		101
#define	IDC_TIMEOUT		102
#define	IDC_ENDSESSION	103

#define IDC_STK_MENU		200
#define IDC_STK_TEXT		201
#define IDC_STK_EDIT		202
#define IDC_STK_NUMBER		203
#define IDC_STK_HIDENUMBER	204
#define IDC_STK_HELP		205

#define WM_STK_INDICATION		WM_USER+100
#define WM_STK_PACINFO_RECV		WM_USER+101
#define WM_STK_RESP_RECV		WM_USER+102
#define WM_STK_RESP_ACK			WM_USER+103
#define WM_STK_CONFIRMATION		WM_USER+104
#define WM_STK_CALLSETUP		WM_USER+105
#define WM_STK_SENDDTMF			WM_USER+106
#define WM_STK_PLAYTONE			WM_USER+107
#define WM_STK_DISPALYTEXT		WM_USER+108
#define WM_STK_TIMEOUT			WM_USER+109
#define WM_STK_IMECHANGE		WM_USER+110

#define STK_TIMER_MESSAGE_DELAY		1
#define STK_TIMER_INKEY_TIMEOUT		2
#define STK_TIMER_INPUT_TIMEOUT		3
#define STK_TIMER_SELECT_TIMEOUT	4
#define STK_INPUT_LIMIT_TEXT		255

#define STK_WAITTIME_4S			40
#define STK_WAITTIME_5S			50
#define STK_WAITTIME_60S		600
#define STK_WAITTIME_3MIN		1800
#define STK_WAITTIME_NONE		0

#define IDS_STK_REQUESTING		"/rom/stk/requesting.gif"

typedef struct tagINTPUTEDIT{
	long lStyle;
	HWND nKeyBoardType;
	char szName[20];
}INPUTEDIT;

#endif // _STK_MAIN_H_

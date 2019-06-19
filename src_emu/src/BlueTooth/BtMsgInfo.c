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
#include "string.h"
#include "mullang.h"
#include "winpda.h"
#include "prebrowhead.h"
#include "msgunibox.h"

#include "BtMsg.h"
#include "BtString.h"

typedef struct tagBTMSGINFODATA
{	
	HWND       hWndFrame;
	HWND       hBtMsgInfoForm;
	PBTMSGHANDLENAME pBtMsghandle;
}BTMSGINFODATA,*PBTMSGINFODATA;

static LRESULT BtMsgInfoWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL BtMsgInfo_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void BtMsgInfo_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);

BOOL BtMsgInfoWindow(HWND hWndFrame, HWND hWnd, PBTMSGHANDLENAME pBtMsghandle)
{
    WNDCLASS	wc;

	BTMSGINFODATA  BtMsgInfoData;
	HWND  hBtMsgInfoWnd;
	RECT  rClient;
	
	memset(&BtMsgInfoData, 0, sizeof(BTMSGINFODATA));
	
	BtMsgInfoData.hWndFrame=hWndFrame;
	BtMsgInfoData.pBtMsghandle=pBtMsghandle;
	
	wc.style         = 0;
	wc.lpfnWndProc   = BtMsgInfoWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = sizeof(BTMSGINFODATA);
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "BtMsgInfoWndClass";    
	wc.hbrBackground = NULL;
	
	if (!RegisterClass(&wc))
	{
		return FALSE;
	}
	
	GetClientRect(hWndFrame, &rClient);
	
	hBtMsgInfoWnd = CreateWindow(
		"BtMsgInfoWndClass", 
		"",
		WS_VISIBLE | WS_CHILD | WS_VSCROLL,
		rClient.left, rClient.top, rClient.right-rClient.left, rClient.bottom-rClient.top,
		hWndFrame, 
		NULL, 
		NULL, 
		(PVOID)&BtMsgInfoData);
    
	if (NULL==hBtMsgInfoWnd)
	{
		UnregisterClass("BtMsgInfoWndClass",NULL);
		return FALSE;
	}
	
	SetFocus(hBtMsgInfoWnd);
	
	ShowWindow(hBtMsgInfoWnd,SW_SHOW);
	UpdateWindow(hBtMsgInfoWnd);
	
	return TRUE;	
}

static LRESULT BtMsgInfoWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT lResult=TRUE;
	PBTMSGINFODATA pBtMsgInfoData=NULL;
	
    pBtMsgInfoData=GetUserData(hWnd);
	
	switch (message)
    {
    case WM_CREATE :
		lResult = BtMsgInfo_OnCreate(hWnd,(LPCREATESTRUCT)(lParam));
        break;
		
	case WM_SETFOCUS:
		SetFocus(pBtMsgInfoData->hBtMsgInfoForm);
        break;
		
    case PWM_SHOWWINDOW:
		{			
			SendMessage(pBtMsgInfoData->hWndFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
			SendMessage(pBtMsgInfoData->hWndFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_OK);
			SendMessage(pBtMsgInfoData->hWndFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			
			SetWindowText(pBtMsgInfoData->hWndFrame,IDP_BT_STRING_MSGINFO);
			
			SetFocus(pBtMsgInfoData->hBtMsgInfoForm);
		}
        break;
		
	case WM_KEYDOWN:
		BtMsgInfo_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);
		break;		
		
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
		
    case WM_DESTROY :
        UnregisterClass("BtMsgInfoWndClass", NULL);
        break;
		
    default:    
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;		
}

static BOOL BtMsgInfo_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct)
{
	int nCurBtMsgType;
	UINT BtMsgSize;
 	int nBtKilobyte = 0;
    HWND hBtMsgInfoForm;
    LISTBUF		Listtmp;
	RECT rClient;
	char szBtMsgDate[32],szBtMsgTime[32],szBtMsgSize[32];
	BTMSGTIME BtMsgtime;
	SYSTEMTIME BtMsgSystemTime;

	PBTMSGINFODATA pBtMsgInfoData=NULL;
	HWND hBtMsgWndFrame = NULL;

	szBtMsgDate[0] = 0;
	szBtMsgTime[0] = 0;

	pBtMsgInfoData=GetUserData(hWnd);
	memcpy(pBtMsgInfoData,lpCreateStruct->lpCreateParams,sizeof(BTMSGINFODATA));

	GetClientRect(hWnd,&rClient);
    hBtMsgWndFrame = pBtMsgInfoData->hWndFrame;

	nCurBtMsgType=pBtMsgInfoData->pBtMsghandle->nBtMsgType;

	PREBROW_InitListBuf(&Listtmp);

	PREBROW_AddData(&Listtmp, IDP_BT_STRING_FROM, IDP_BT_TITLE_BLUETOOTH);

	chdir(BTRECV_FILEPATH);
    
	BtGetMsgTime(pBtMsgInfoData->pBtMsghandle->szBtMsgRealName, &BtMsgtime);
	
	BtFTtoST(&BtMsgtime, &BtMsgSystemTime);
	
	GetTimeDisplay(BtMsgSystemTime, szBtMsgTime, szBtMsgDate);
	
	PREBROW_AddData(&Listtmp, IDP_BT_STRING_DATE, szBtMsgDate);
	PREBROW_AddData(&Listtmp, IDP_BT_STRING_TIME, szBtMsgTime);

	switch(nCurBtMsgType)
	{
	case MU_MSG_BT_VCARD:
        PREBROW_AddData(&Listtmp, IDP_BT_STRING_TYPE, IDP_BT_TITLE_BUSINESSCARD);
		break;
	case MU_MSG_BT_VCAL:
        PREBROW_AddData(&Listtmp, IDP_BT_STRING_TYPE, IDP_BT_TITLE_CALENDARENTRY);
		break;		
	case MU_MSG_BT_PICTURE:
		PREBROW_AddData(&Listtmp, IDP_BT_STRING_TYPE, IDP_BT_STRING_PICTURE);
		break;
	case MU_MSG_BT_NOTEPAD:
        PREBROW_AddData(&Listtmp, IDP_BT_STRING_TYPE, IDP_BT_TITLE_NOTE);
		break;
	case MU_MSG_BT:
		PREBROW_AddData(&Listtmp, IDP_BT_STRING_TYPE, IDP_BT_STRING_UNKNOWN);
		break;
	default:		
		break;
	}
	
	szBtMsgSize[0] = 0;
	
	BtMsgSize=BtGetMsgSize(pBtMsgInfoData->pBtMsghandle->szBtMsgRealName);

	nBtKilobyte = BtMsgSize/1024;
	if(nBtKilobyte >= 1)
		sprintf(szBtMsgSize,"%d KB",nBtKilobyte);
	else
	{
		nBtKilobyte = BtMsgSize * 10 /1024;
		if(nBtKilobyte >=1)
			sprintf(szBtMsgSize,"0.%d KB",nBtKilobyte);
		else
		{
			nBtKilobyte = BtMsgSize * 100 /1024;
			sprintf(szBtMsgSize,"0.0%d KB",nBtKilobyte);
		}
	}
	
	PREBROW_AddData(&Listtmp, IDP_BT_STRING_SIZE, szBtMsgSize);

	hBtMsgInfoForm=CreateWindow(
			FORMVIEWER,
			"",
			WS_VISIBLE  |WS_CHILD | WS_VSCROLL,
			rClient.left,
			rClient.top,
			rClient.right - rClient.left,
			rClient.bottom - rClient.top,
			hWnd,
			NULL,
			NULL, 
			(PVOID)&Listtmp);
	
    pBtMsgInfoData->hBtMsgInfoForm=hBtMsgInfoForm;

	SendMessage(pBtMsgInfoData->hWndFrame,PWM_SETBUTTONTEXT,0,(LPARAM)"");
	SendMessage(pBtMsgInfoData->hWndFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_OK);
	SendMessage(pBtMsgInfoData->hWndFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");

	SetWindowText(hBtMsgWndFrame,IDP_BT_STRING_MSGINFO);
	
	return TRUE;	
}

static void BtMsgInfo_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{
	PBTMSGINFODATA pBtMsgInfoData = NULL;
    pBtMsgInfoData=GetUserData(hWnd);
	
	switch (nKey)
	{	
    case VK_RETURN:
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(pBtMsgInfoData->hWndFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;
		
	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
		break;
	}
}

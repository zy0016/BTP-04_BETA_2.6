/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Bluetooth module
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "plx_pdaex.h"
#include "mullang.h"
#include "pubapp.h"
#include "imesys.h"
#include "string.h"

#include "BtMain.h"
#include "BtString.h"

#define ID_PHONENAME              (WM_USER+0x200)

#define PHONAME_LENGTH_LIMIT           30

static HWND hPhoneNameWnd;
static HWND hPhoneNameEdit;
static HWND hFocus;

extern HWND hBtSettingsPhoneName;   //全局变量，Settings窗口蓝牙设备名SpinBox控件句柄
extern HWND hBtSettingsWnd;         //全局变量，Settings窗口句柄

extern BOOL  bBluetoothStatus;  //全局变量，存放Bluetooth当前状态，默认值为FALSE
// extern BOOL  bVisibilityStatus; //全局变量，存放Visibility当前状态，默认值为FALSE
extern char  szPhoneName[MAX_PHONENAME_LEN];   //全局变量，存放当前手机名，默认值为My Benefon

extern HWND GetBluetoothFrameWnd();

static LRESULT BtPhoneNameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL BtPhoneName_OnCreate(HWND hWnd);
static void BtPhoneName_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);

BOOL BtPhoneNameWindow()
{
	HWND hBluetoothFrame;
	RECT rClient;
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = BtPhoneNameWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "PhoneNameWndClass";

	if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);

    hPhoneNameWnd = CreateWindow(
		"PhoneNameWndClass",
		"", 
        WS_VISIBLE | WS_CHILD,
        rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hBluetoothFrame, 
		NULL, 
		NULL, 
		NULL);

    if (NULL ==  hPhoneNameWnd)
    {
        UnregisterClass("PhoneNameWndClass",NULL);
        return FALSE;
    }

	SetFocus(hPhoneNameWnd);
	
	ShowWindow(hPhoneNameWnd,SW_SHOW);
	UpdateWindow(hPhoneNameWnd);

	SendMessage(hPhoneNameEdit, EM_SETSEL, -1, -1);

    return (TRUE);
}

static LRESULT BtPhoneNameWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam) 
{
    LRESULT lRet = TRUE;
	HWND hBluetoothFrame;
	
	hBluetoothFrame=GetBluetoothFrameWnd();

	switch(wMsgCmd){		
	      case WM_CREATE:
		       BtPhoneName_OnCreate(hWnd);
		       break;
		
	      case WM_SETFOCUS:
		       SetFocus(hPhoneNameEdit);
               break;
			   
		  case PWM_SHOWWINDOW:
			  SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
			  SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_SAVE);
			  SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");

			  SetWindowText(hBluetoothFrame,IDP_BT_TITLE_SETTINGS);
			  SetFocus(hPhoneNameEdit);
			  break;
			   
	      case WM_KEYDOWN:
			   BtPhoneName_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);
			   break;
			  
	      case WM_CLOSE:
			   DestroyWindow(hWnd);
			   break;
	      case WM_DESTROY:
			   UnregisterClass("PhoneNameWndClass", NULL);
			   break;
	      default:
			   lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			   break;
	}
	return lRet;
}

static BOOL BtPhoneName_OnCreate(HWND hWnd)
{
	IMEEDIT ie;
	RECT rClient;
	HWND hBluetoothFrame;
	
	memset((void*)&ie, 0, sizeof(IMEEDIT));
	
	ie.hwndNotify   = hWnd;
	GetClientRect(hWnd, &rClient);
	hBluetoothFrame=GetBluetoothFrameWnd();

	hPhoneNameEdit = CreateWindow(
        "IMEEDIT",
        "",
        WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | ES_TITLE,
        rClient.left, rClient.top,
		rClient.right - rClient.left, rClient.bottom - rClient.top - 47*2,
        hWnd,
        (HMENU)ID_PHONENAME,
        NULL,
        &ie);
	
	SendMessage(hPhoneNameEdit, EM_LIMITTEXT, PHONAME_LENGTH_LIMIT, 0);
	SendMessage(hPhoneNameEdit, EM_SETTITLE, 0, (LPARAM)IDP_BT_STRING_PHONENAMEINPUT);
    	
	//SendMessage(hPhoneNameEdit, WM_SETTEXT, 0, (LPARAM)szPhoneName);
	SetWindowText(hPhoneNameEdit, szPhoneName);
    
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_SAVE);
    SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");

	SetWindowText(hBluetoothFrame,IDP_BT_TITLE_SETTINGS);
    
	SetFocus(hPhoneNameEdit);

	return TRUE;
}

static void BtPhoneName_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{
	char tmpBuff[30];
	HWND hBluetoothFrame;
	
    hBluetoothFrame=GetBluetoothFrameWnd();

	switch (nKey)
	{
	  case VK_F10:
		  PostMessage(hWnd,WM_CLOSE,0,0);
		  SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		  break;
		
	  case VK_RETURN:
		  {
			memset(tmpBuff,0,30);
			GetWindowText(hPhoneNameEdit,tmpBuff,30);
			
			if(strlen(tmpBuff)==0)	
				PLXTipsWin(hBluetoothFrame, hWnd, 0,IDP_BT_STRING_DEFINENAME, IDP_BT_TITLE_SETTINGS,
					Notify_Alert, IDP_BT_BUTTON_OK, NULL,20);
			else
			{
				memset(szPhoneName,0,sizeof(szPhoneName));
				strcpy(szPhoneName,tmpBuff);
#ifndef _EMULATE_				
				Btsdk_SetLocalName((BTUINT8 *)szPhoneName,(BTUINT16)(sizeof(szPhoneName)));
#endif
				SendMessage(hBtSettingsPhoneName, SSBM_SETTEXT, 
					0, (LPARAM)szPhoneName);

				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			}
		  }				
		break;
		
	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
		break;         
	}
}

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

#include "BtMain.h"
#include "BtString.h"

#define ID_BLUETOOTHVISIBILITYLIST	   (WM_USER+0x200)

static HWND hBtVisibilityWnd;
static HWND hBtVisibilityList;
static HBITMAP hBtVisibilityOn,hBtVisibilityOff; 

extern HWND hBtSettingsVisibility;  //全局变量，Settings窗口蓝牙可见性SpinBox控件句柄
extern HWND hBtSettingsWnd;         //全局变量，Settings窗口句柄

extern BOOL  bBluetoothStatus;  //全局变量，存放Bluetooth当前状态，默认值为FALSE
extern BOOL  bVisibilityStatus; //全局变量，存放Visibility当前状态，默认值为FALSE

extern HWND GetBluetoothFrameWnd();
static LRESULT BtVisibilityWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL BtVisibility_OnCreate(HWND hWnd);
static void BtVisibility_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);

BOOL BtVisibilityWindow(HWND hWnd)
{
	WNDCLASS wc;
	RECT rClient;
	HWND hBluetoothFrame;
	
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = NULL;
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hInstance = NULL;
	wc.lpfnWndProc = BtVisibilityWndProc;
	wc.lpszClassName = "BtVisibilityClass";
	wc.lpszMenuName = NULL;
	wc.style = 0;
	
	if(!RegisterClass(&wc))
		return FALSE;
	
	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);
	
	hBtVisibilityWnd = CreateWindow(
		"BtVisibilityClass",
		"",
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hBluetoothFrame,
		NULL, 
		NULL,
		NULL
		);

	SetFocus(hBtVisibilityWnd);
	
	ShowWindow(hBtVisibilityWnd,SW_SHOW);
	UpdateWindow(hBtVisibilityWnd);
		
	return TRUE;	
}

static LRESULT BtVisibilityWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;
	HWND hBluetoothFrame;
	
	hBluetoothFrame=GetBluetoothFrameWnd();

	switch(wMsgCmd){		
	     case WM_CREATE:
		     BtVisibility_OnCreate(hWnd);
		     break;
	
	     case WM_SETFOCUS:
		     SetFocus(hBtVisibilityList);
             break;
		
		 case PWM_SHOWWINDOW:
			 SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
			 SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			 SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			 
			 SetWindowText(hBluetoothFrame,IDP_BT_TITLE_VISIBILITY);
			 
			 SetFocus(hBtVisibilityList);
			 break;
		
	     case WM_KEYDOWN:
			 BtVisibility_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);		
			 break;
			 
	     case WM_CLOSE:
			DestroyWindow(hWnd);
			break;

	     case WM_DESTROY:
			UnregisterClass("BtVisibilityClass", NULL);

			if(hBtVisibilityOn)
				DeleteObject(hBtVisibilityOn);
			if(hBtVisibilityOff)
				DeleteObject(hBtVisibilityOff);
			break;

	     default:
			lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
	}
	return lRet;
}

static BOOL BtVisibility_OnCreate(HWND hWnd)
{
	RECT rClient;
	HDC hDC=NULL;
	HWND hBluetoothFrame;
	
	hBluetoothFrame=GetBluetoothFrameWnd();
	
	GetClientRect(hWnd,&rClient);
	
	hDC=GetDC(hWnd);
	hBtVisibilityOn=CreateBitmapFromImageFile(hDC,"/rom/ime/radioon.bmp",NULL,NULL);
	hBtVisibilityOff=CreateBitmapFromImageFile(hDC,"/rom/ime/radiooff.bmp",NULL,NULL);
	ReleaseDC(hWnd,hDC);
	
	hBtVisibilityList=CreateWindow(
		"LISTBOX",
		"",
		WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY | LBS_BITMAP,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hWnd,
		(HMENU)ID_BLUETOOTHVISIBILITYLIST,
		NULL,
		NULL);
	
	if(NULL==hBtVisibilityList)
		return FALSE;
	
	SendMessage(hBtVisibilityList,LB_ADDSTRING,-1,(LPARAM)IDP_BT_STRING_ON);
	SendMessage(hBtVisibilityList,LB_ADDSTRING,-1,(LPARAM)IDP_BT_STRING_OFF);
	
	if(bVisibilityStatus)
	{
		SendMessage(hBtVisibilityList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)0),(LPARAM)hBtVisibilityOn);
		SendMessage(hBtVisibilityList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)1),(LPARAM)hBtVisibilityOff);
	}
	else
	{
		SendMessage(hBtVisibilityList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)0),(LPARAM)hBtVisibilityOff);
		SendMessage(hBtVisibilityList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)1),(LPARAM)hBtVisibilityOn);
	}
	
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
	
	SetWindowText(hBluetoothFrame,IDP_BT_TITLE_VISIBILITY);
	
	if(!bVisibilityStatus)
	   SendMessage(hBtVisibilityList,LB_SETCURSEL,1,0);
	else
	   SendMessage(hBtVisibilityList,LB_SETCURSEL,0,0);

	return TRUE;
}

static void BtVisibility_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{
	LONG curSel;
	HWND hBluetoothFrame;
	
    hBluetoothFrame=GetBluetoothFrameWnd();
	
	switch (nKey)
	{
	case VK_F5:
			curSel = SendMessage(hBtVisibilityList, LB_GETCURSEL, 0, 0);
			
			if(0==curSel)
			{
#ifndef _EMULATE_
				Btsdk_SetDiscoveryMode(BTSDK_DISCOVERY_DEFAULT_MODE);
#endif

				bVisibilityStatus=TRUE;
			}
			else
			{  
#ifndef _EMULATE_
				Btsdk_SetDiscoveryMode(BTSDK_LIMITED_DISCOVERABLE);
#endif

				bVisibilityStatus=FALSE;
			}
			SendMessage(hBtSettingsWnd, WM_SETVISIBILITYSTATE, bVisibilityStatus, 0);

			SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			PostMessage(hWnd,WM_CLOSE,0,0);			
			
		break;
	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;
		
	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
		break;         
	}
	
}

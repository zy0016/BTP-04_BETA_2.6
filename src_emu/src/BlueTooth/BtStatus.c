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
#include "BtThread.h"
#include "BtString.h"
#include "BtMain.h"

#define ID_BLUETOOTHSTATUSLIST	          (WM_USER+0x200)

#define CONFIRM_SWITCHOFFBT               (WM_USER+0x301)
#define TIPS_OK                           (WM_USER+0x302)

#define  BT_MSG_DEVICE_OPENED			  (WM_USER+0x400)
#define  BT_MSG_DEVICE_CLOSED			  (WM_USER+0x401)
#define  BT_MSG_DEVICE_DISCONNECT_CLOSED  (WM_USER+0x402)

#define BT_SWITCH_ON_OFF_TIMEOUT			10
#define TIMER_ID_SWITCH_OFF					0x1
#define TIMER_ID_SWITCH_ON					0x2
static  BOOL bTimeOut=FALSE;

static HWND hBtChangeStatusWnd;
static HWND hBtChangeStatusList;
static HBITMAP hBtStatusOn,hBtStatusOff; 
static HWND hFocus;

extern HWND hBtSettingsStatus;   //全局变量，Settings窗口设置蓝牙打开关闭SpinBox控件句柄
extern HWND hBtSettingsWnd;      //全局变量，Settings窗口句柄

extern BOOL  bBluetoothStatus;  //全局变量，存放Bluetooth当前状态，出厂默认值为FALSE

static BOOL bOpenDeviceFinished=TRUE;
static BOOL bCloseDeviceFinished=TRUE;

extern HWND GetBluetoothFrameWnd();
static LRESULT BtChangeStatusWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL BtChangeStatus_OnCreate(HWND hWnd);
static void BtChangeStatus_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);

BOOL BtChangeStatusWindow()
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
	wc.lpfnWndProc = BtChangeStatusWndProc;
	wc.lpszClassName = "BtChangeStatusWndClass";
	wc.lpszMenuName = NULL;
	wc.style = 0;
	
	if(!RegisterClass(&wc))
		return FALSE;

	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);
	
	hBtChangeStatusWnd = CreateWindow(
		"BtChangeStatusWndClass",
		"",
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hBluetoothFrame,
		NULL, 
		NULL,
		NULL);
	
	SetFocus(hBtChangeStatusWnd);
	
	ShowWindow(hBtChangeStatusWnd,SW_SHOW);
	UpdateWindow(hBtChangeStatusWnd);

	return TRUE;
}

static LRESULT BtChangeStatusWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;	
	HWND hBluetoothFrame;

	hBluetoothFrame=GetBluetoothFrameWnd();
    
	switch(wMsgCmd){		
	case WM_CREATE:
	    BtChangeStatus_OnCreate(hWnd);
		bTimeOut=FALSE;
		break;   
		
	case WM_SETFOCUS:
		SetFocus(hBtChangeStatusList);
        break;
		
	case PWM_SHOWWINDOW:
		SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
		SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		
		SetWindowText(hBluetoothFrame,IDP_BT_TITLE_BLUETOOTH);
		
		SetFocus(hBtChangeStatusList);
        break;
		
	case WM_KEYDOWN:
		BtChangeStatus_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);
		break;

	case WM_TIMER:
		switch(wParam) {
		case TIMER_ID_SWITCH_OFF:
			DEBUG("\r\nBluetooth:Switch off Timeout\r\n");
			bTimeOut=TRUE;
			WaitWin(hWnd,FALSE,ML("Switch off..."),ML("Bluetooth"),NULL,NULL,0);
			break;
		case TIMER_ID_SWITCH_ON:
			DEBUG("\r\nBluetooth:Switch on Timeout\r\n");
			bTimeOut=TRUE;
			WaitWin(hWnd,FALSE,ML("Switch on..."),ML("Bluetooth"),NULL,NULL,0);
			break;
		default:
			break;
		}
		break;

	case BT_MSG_DEVICE_OPENED:
		{
			printf("\r\nreceive BT_MSG_DEVICE_OPENED\r\n");
			bBluetoothStatus=TRUE;	
			
			DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //通知程序管理器蓝牙打开
			SendMessage(hBtSettingsWnd, WM_SETBTSTATE, bBluetoothStatus, 0);
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			bOpenDeviceFinished=TRUE;
			WaitWin(hWnd,FALSE,ML("Switch on..."),ML("Bluetooth"),NULL,NULL,0);
			printf("\r\nBT switch on finished\r\n");
			
		}
		break;
	case BT_MSG_DEVICE_CLOSED:
		{
			printf("\r\nReceive BT_MSG_DEVICE_CLOSED\r\n");
			bBluetoothStatus=FALSE;
			bCloseDeviceFinished=TRUE;
			DlmNotify(PS_SETBLUETOOTHON,ICON_CANCEL);  //通知程序管理器蓝牙关闭
			SendMessage(hBtSettingsWnd, WM_SETBTSTATE, bBluetoothStatus, 0);
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			WaitWin(hWnd,FALSE,ML("Switch off..."),ML("Bluetooth"),NULL,NULL,0);

		}
		break;
	case BT_MSG_DEVICE_DISCONNECT_CLOSED:
		{
			bCloseDeviceFinished=TRUE;
			bBluetoothStatus=FALSE;
			DlmNotify(PS_SETBLUETOOTHON,ICON_CANCEL);  //通知程序管理器蓝牙关闭
			SendMessage(hBtSettingsWnd, WM_SETBTSTATE, bBluetoothStatus, 0);
			WaitWin(hWnd,FALSE,ML("Switch off..."),ML("Bluetooth"),NULL,NULL,0);
			PLXTipsWin(hBluetoothFrame, hWnd, TIPS_OK, IDP_BT_STRING_BTSWITCHOFFNOTIF,IDP_BT_TITLE_BLUETOOTH, 
				Notify_Success, IDP_BT_BUTTON_OK, NULL,20);	
		}
		break;
        
	case CONFIRM_SWITCHOFFBT:
		if (lParam)
		{
			WaitWin(hWnd,TRUE,ML("Switch off..."),ML("Bluetooth"),NULL,NULL,0);
			bCloseDeviceFinished=FALSE;
			BT_SendRequest(hWnd,BT_MSG_DEVICE_DISCONNECT_CLOSED,RT_BT_SWITCHOFF,NULL,NULL,0);
		}
		else
		{
			PostMessage(hWnd,WM_CLOSE,0,0);
		    SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		}	
		break;

	case TIPS_OK:		
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;
		
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		UnregisterClass("BtChangeStatusWndClass", NULL);
		
        if(hBtStatusOn)
			DeleteObject(hBtStatusOn);
		if(hBtStatusOff)
			DeleteObject(hBtStatusOff);
		break;
	default:
		lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lRet;
}

static BOOL BtChangeStatus_OnCreate(HWND hWnd)
{
	RECT rClient;
	HDC hDC;
	HWND hBluetoothFrame;

	hBluetoothFrame=GetBluetoothFrameWnd();

	GetClientRect(hWnd,&rClient);
	
	hDC=GetDC(hWnd);
	hBtStatusOn=CreateBitmapFromImageFile(hDC,"/rom/ime/radioon.bmp",NULL,NULL);
	hBtStatusOff=CreateBitmapFromImageFile(hDC,"/rom/ime/radiooff.bmp",NULL,NULL);
	ReleaseDC(hWnd,hDC);

	hBtChangeStatusList=CreateWindow(
		"LISTBOX",
		"",
		WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_NOTIFY | LBS_BITMAP,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hWnd,
		(HMENU)ID_BLUETOOTHSTATUSLIST,
		NULL,
		NULL);

	if(NULL==hBtChangeStatusList)
		return FALSE;
	
	SendMessage(hBtChangeStatusList,LB_ADDSTRING,-1,(LPARAM)IDP_BT_STRING_ON);
	SendMessage(hBtChangeStatusList,LB_ADDSTRING,-1,(LPARAM)IDP_BT_STRING_OFF);

	if(bBluetoothStatus)
	{
		SendMessage(hBtChangeStatusList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)0),(LPARAM)hBtStatusOn);
		SendMessage(hBtChangeStatusList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)1),(LPARAM)hBtStatusOff);
	}
	else
	{
		SendMessage(hBtChangeStatusList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)0),(LPARAM)hBtStatusOff);
		SendMessage(hBtChangeStatusList,LB_SETIMAGE,(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)1),(LPARAM)hBtStatusOn);
	}
	
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
	
	SetWindowText(hBluetoothFrame,IDP_BT_TITLE_BLUETOOTH);

    if(!bBluetoothStatus)
	   SendMessage(hBtChangeStatusList,LB_SETCURSEL,1,0);
	else
	   SendMessage(hBtChangeStatusList,LB_SETCURSEL,0,0);
	
	return TRUE;
}

static void BtChangeStatus_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{
	LONG curSel;
	HWND hBluetoothFrame;
	
    hBluetoothFrame=GetBluetoothFrameWnd();

     switch (nKey)
	 {
	 case VK_F5:
		 curSel = SendMessage(hBtChangeStatusList, LB_GETCURSEL, 0, 0);

		 if(1==curSel && bBluetoothStatus) //选择Off,并且当前状态是一个激活的连接					 			 
		 {

			 if(!bOpenDeviceFinished || !bCloseDeviceFinished) 
			 {
				 DEBUG("\r\nBluetooth:Open/closed not finished,but Off selected\r\n");
				 return ;
			 }

			 DEBUG("\r\nBluetooth:check BtIsActiveConnect when switch off BT device\r\n");
			if(BtIsActiveConnect())
				PLXConfirmWinEx(hBluetoothFrame,hWnd,IDP_BT_STRING_BTSWITCHOFFPROMPT, Notify_Request, 
				   IDP_BT_TITLE_BLUETOOTH, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_SWITCHOFFBT);
			else
			{

				DEBUG("\r\nBluetooth:WaitWin in switch off device\r\n");
				WaitWin(hWnd,TRUE,ML("Switch off..."),ML("Bluetooth"),NULL,NULL,0);
				SetTimer(hWnd,TIMER_ID_SWITCH_OFF,BT_SWITCH_ON_OFF_TIMEOUT*1000,NULL);
				bCloseDeviceFinished=FALSE;
                BT_SendRequest(hWnd,BT_MSG_DEVICE_CLOSED,RT_BT_SWITCHOFF,NULL,NULL,0);
			}
            
		 }		 
		 else if(0==curSel && !bBluetoothStatus)    //选择On,并且当前状态是一个未激活的连接
		 {
			 if(!bOpenDeviceFinished||!bCloseDeviceFinished)
			 {
				 DEBUG("\r\nBlueTooth:Open not finished last time,but user open again\r\n");
				 return;
			 }

			 bOpenDeviceFinished=FALSE;
			 WaitWin(hWnd,TRUE,ML("Switch on..."),ML("Bluetooth"),NULL,NULL,0);
			 SetTimer(hWnd,TIMER_ID_SWITCH_ON,BT_SWITCH_ON_OFF_TIMEOUT*1000,NULL);

           	 BT_SendRequest(hWnd,BT_MSG_DEVICE_OPENED,RT_BT_SWITCHON,0,NULL,0);
//#if 0
//			 {
//				 int k;
//				 int i;
//				 printf("\r\n=========begin to test\r\n");
//				 for(k=0;k<=100;k++)
//				 {
//					 printf("\r\n==========k=%d\r\n",k);
//					 SwitchOnBlueTooth();
//
//					 for(i=0;i<0x10000;i++)
//						 NULL;
//
//					 Sleep(5*1000);
//					 SwitchOffBlueTooth();
//					 
//				 }
//				 printf("\r\n================Test finished\r\n");
//				 return;
//			 }
//#endif			 
		 }		
		 else //选择off并且当前是未激活连接和选择on当前是激活的连接
		 {
			 PostMessage(hWnd,WM_CLOSE,0,0);
			 SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		 }
		 
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

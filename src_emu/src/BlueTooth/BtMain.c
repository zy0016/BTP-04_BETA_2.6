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

#include "sdk_def.h"
#include "sdk_opp.h"

#include "window.h"
#include "plx_pdaex.h"
#include "winpda.h"
#include "mullang.h"

#include "BtMain.h"
#include "BtThread.h"
#include "BtString.h"
#include "BtFileOper.h"

#define ID_EXIT		                   (WM_USER+0x200)
#define ID_OK                          (WM_USER+0x201)

#define ID_BLUETOOTHMAINLIST	       (WM_USER+0x202)
#define ID_SETTINGS                    (WM_USER+0x203)
#define ID_PAIREDDEVICES               (WM_USER+0x204)

#define CONFIRM_SWITCHONBTSEARCH       (WM_USER+0x300)
#define TIPS_SEARCH                    (WM_USER+0x301)

static HWND hBluetoothFrame=NULL;
static HWND hBtMainWnd=NULL;
static HINSTANCE hBluetoothInstance=NULL;

static HWND hBtControlList=NULL;
static HBITMAP hSettingsBitmap,hPairedDeviceBitmap;

extern pthread_cond_t  BtRecvData_cond;

static LRESULT BluetoothMainWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//底层回调函数需要弹出的窗口过程
static LRESULT BluetoothMSGWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

static	BOOL	RegisterBTMainWnd();

static BOOL Bluetooth_OnCreate(HWND hWnd);
static void Bluetooth_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void Bluetooth_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);
static void Bluetooth_OnDestroy();

extern BOOL CreateSettingsWindow();
extern BOOL CreatePairedDevicesWindow();
extern BOOL PM_GetkeyLockStatus (void);
extern BOOL PM_GetPhoneLockStatus (void);


extern BOOL BtChangeStatusWindow();
extern BOOL SearchDevicesWindow();

HWND GetBluetoothFrameWnd();

HWND hBtMsgWnd;   //底层回调函数需要弹出的窗口句柄
static BOOL bChangedLock=FALSE;
static int  iLockNum=0;

extern HWND  hBtSettingsWnd;
extern HWND hSearchDevicesWnd;    //关闭和打开配对设备的WaitWin需要的外部变量
extern char szWaitWinComPairing[64];
extern int iActivePair;

BOOL	bCallByShortcur	=	FALSE;
/*********************************************************************\
* Function  : Bluetooth_AppControl
* Purpose   : Bluetooth application main entry function
* Params    :    
* Return    :    
* Remarks   :
**********************************************************************/
DWORD Bluetooth_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	WNDCLASS wc;
    DWORD  dwRet;
	RECT rClient;

    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT :   
		wc.style         = 0;
		wc.lpfnWndProc   = BluetoothMSGWinProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "BLUETOOTHMSGWND";
		RegisterClass(&wc);
		
		hBtMsgWnd = CreateWindow(      //创建一个隐藏窗口,实现蓝牙底层函数的回调
			"BLUETOOTHMSGWND", 
			NULL, 
			WS_POPUP, 
			0, 0, 50, 50,
			NULL, 
			NULL,
			pInstance,
			NULL);

        hBluetoothInstance=(HINSTANCE)pInstance;

		BT_Init();         //初始化蓝牙协议栈

		InitBtThread();    //初始化处理蓝牙比较耗时操作的线程
		
        break;

	case APP_GETOPTION:
			switch(wParam){
			case AS_APPWND:
				dwRet = (DWORD)hBluetoothInstance;
				break;
			}
			break;

    case APP_ACTIVE:

		if (lParam == 1)         //favourite & shortcut for Bluetooth on/off
		{
			if (hBluetoothFrame)
			{
				DestroyWindow(hBluetoothFrame);
			}
			hBluetoothFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR);
			
			SendMessage(hBluetoothFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT,0), (LPARAM)IDP_BT_BUTTON_EXIT);
			SendMessage(hBluetoothFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_OK,1), (LPARAM)"");
			bCallByShortcur	=	TRUE;
			CreateSettingsWindow();
		}
		if (lParam == 2)       //favourite & shortcut for Searching for new devices
		{
			if (hBluetoothFrame) 
			{
				DestroyWindow(hBluetoothFrame);
			}
			hBluetoothFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR);

			SendMessage(hBluetoothFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT,0), (LPARAM)IDP_BT_BUTTON_EXIT);
			SendMessage(hBluetoothFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_OK,1), (LPARAM)"");	
			
			bCallByShortcur	=	TRUE;
			
			if(bBluetoothStatus)
				SearchDevicesWindow();
			else
				PLXConfirmWinEx(hBluetoothFrame,hBtMsgWnd,IDP_BT_STRING_BTSWITCHONPROMPT, Notify_Request, 
				    IDP_BT_TITLE_BLUETOOTH, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_SWITCHONBTSEARCH);	
		}


		if(IsWindow(hBluetoothFrame))
		{
			ShowWindow(hBluetoothFrame, SW_SHOW);
			ShowOwnedPopups(hBluetoothFrame, SW_SHOW);
			UpdateWindow(hBluetoothFrame);
		}
		
		else
		{
	        wc.style         = 0;
		    wc.lpfnWndProc   = BluetoothMainWinProc;
		    wc.cbClsExtra    = 0;
		    wc.cbWndExtra    = 0;
		    wc.hInstance     = NULL;
		    wc.hIcon         = NULL;
		    wc.hCursor       = NULL;
		    wc.hbrBackground = NULL;
		    wc.lpszMenuName  = NULL;
		    wc.lpszClassName = "BluetoothWndClass";
		
		    if (!RegisterClass(&wc))
			{
                UnregisterClass("BluetoothWndClass", NULL);
			    return FALSE;
            }

			hBluetoothFrame=CreateFrameWindow( WS_CAPTION |PWS_STATICBAR);
			GetClientRect(hBluetoothFrame,&rClient);
			bCallByShortcur = FALSE;
            hBtMainWnd=CreateWindow(
				"BlueToothWndClass",
				"",
				WS_VISIBLE | WS_CHILD,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
				hBluetoothFrame, 
				NULL, 
				NULL, 
				NULL);
		    
			SetFocus(hBtMainWnd); 
			ShowWindow(hBtMainWnd, SW_SHOW); 
			UpdateWindow(hBtMainWnd); 
						
		}
		break;

    case APP_INACTIVE :
		ShowOwnedPopups(hBluetoothFrame, SW_HIDE);
        ShowWindow(hBluetoothFrame,SW_HIDE); 
        break;

    default :
        break;
    }

    return dwRet;

}

static	BOOL	RegisterBTMainWnd()
{
	WNDCLASS	wc;

	wc.style         = 0;
	wc.lpfnWndProc   = BluetoothMainWinProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "BluetoothWndClass";
	
	if (!RegisterClass(&wc))
	{
		//UnregisterClass("BluetoothWndClass", NULL);
		return FALSE;
	}
	return TRUE;
}

BOOL	CallBTMainWnd(HWND hParent)
{
	RECT	rClient;
	
	if (!bCallByShortcur)
	{
		return TRUE;
	}
	else
		bCallByShortcur = FALSE;

	RegisterBTMainWnd();

	GetClientRect(hParent,&rClient);
			
	hBtMainWnd=CreateWindow(
		"BlueToothWndClass",
		"",
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hParent, 
		NULL, 
		NULL, 
		NULL);
	if (!hBtMainWnd)
	{
		return FALSE;
	}
	
	SetFocus(hBtMainWnd); 
	ShowWindow(hBtMainWnd, SW_SHOW); 
	UpdateWindow(hBtMainWnd); 
	return TRUE;
}
/*********************************************************************\
* Function  : Bluetooth_AppControl
* Purpose   : Bluetooth application main window process function
* Params    :    
* Return    :    
* Remarks   :
**********************************************************************/

static LRESULT BluetoothMainWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    LRESULT lResult=TRUE;
    
	switch (message)
    {
    case WM_CREATE :
		Bluetooth_OnCreate(hWnd);
        break;

	case WM_SETFOCUS:
		SetFocus(hBtControlList);
		break;

	case PWM_SHOWWINDOW:
		SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_EXIT);
		SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		SetWindowText(hBluetoothFrame,IDP_BT_TITLE_BLUETOOTH);
        SetFocus(hBtControlList);
		break;

	case WM_COMMAND:
		Bluetooth_OnCommand(hWnd, wParam, lParam);
		break;

    case WM_KEYDOWN:
		Bluetooth_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
		
    case WM_DESTROY :
        UnregisterClass("BlueToothWndClass", NULL);
		DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hBluetoothInstance);

		if(hSettingsBitmap)
			DeleteObject(hSettingsBitmap);
		if(hPairedDeviceBitmap)
			DeleteObject(hPairedDeviceBitmap);
        break;

    default:    
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}

/*********************************************************************\
* Function  : BlueTooth_OnCreate
* Purpose   : 
* Params    :    
* Return    :    
* Remarks   :
**********************************************************************/

static BOOL Bluetooth_OnCreate(HWND hWnd)
{
	int curIndex;
	RECT rClient;

	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);

    hBtControlList=CreateWindow(
		"LISTBOX",
		"",
		WS_VISIBLE | WS_CHILD | LBS_BITMAP,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hWnd,
		(HMENU)ID_BLUETOOTHMAINLIST,
		NULL,
		NULL);
	if(NULL == hBtControlList)
		return FALSE;

	hSettingsBitmap = LoadImage(NULL, BTSETTINGSICON, 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	
	if(!hSettingsBitmap)
		return FALSE;

    curIndex=SendMessage(hBtControlList, LB_ADDSTRING, 0, (LPARAM)IDP_BT_TITLE_SETTINGS);
	SendMessage(hBtControlList, LB_SETIMAGE, 
		(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)hSettingsBitmap);
    
	hPairedDeviceBitmap = LoadImage(NULL, BTPAIRDEVICEICON, 
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if(!hPairedDeviceBitmap)
		return FALSE;

    curIndex=SendMessage(hBtControlList, LB_ADDSTRING, 0, (LPARAM)IDP_BT_TITLE_PAIREDDEVICES);
	SendMessage(hBtControlList, LB_SETIMAGE, 
		(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)hPairedDeviceBitmap);

	SendMessage(hBtControlList, LB_SETCURSEL, 0, 0);

    SendMessage(hBluetoothFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_EXIT,0), (LPARAM)IDP_BT_BUTTON_EXIT);
	SendMessage(hBluetoothFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_OK,1), (LPARAM)"");

	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
	
	SetWindowText(hBluetoothFrame,IDP_BT_TITLE_BLUETOOTH);

	return TRUE;
}

/*********************************************************************\
* Function  : BlueTooth_OnCommand
* Purpose   : 
* Params    :    
* Return    :    
* Remarks   :
**********************************************************************/

static void Bluetooth_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
	case ID_SETTINGS:
		CreateSettingsWindow();
		break;
    case ID_PAIREDDEVICES:
		CreatePairedDevicesWindow();
		break;
	}
}

/*********************************************************************\
* Function  : BlueTooth_OnKeyDown
* Purpose   : 
* Params    :    
* Return    :    
* Remarks   :
**********************************************************************/

static void Bluetooth_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{
	LONG curSel;

	switch (nKey)
	{
	case VK_F5:
		curSel = SendMessage(hBtControlList, LB_GETCURSEL, 0, 0);

	    if (0 == curSel) 
			SendMessage(hWnd, WM_COMMAND, ID_SETTINGS, (LPARAM)NULL);
		else
            SendMessage(hWnd, WM_COMMAND, ID_PAIREDDEVICES, (LPARAM)NULL);

		break;

	case VK_F10:
//      WriteStatusFile(BTSTATUSFILENAME);
// 		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, 0);
		PostMessage(hWnd,WM_CLOSE,0,0);
		break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
		break;
	}
}

HWND GetBluetoothFrameWnd()
{
	return hBluetoothFrame;
}

static LRESULT BluetoothMSGWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult=TRUE;

	static BTSVCHDL svc_hdl;  //接受其它蓝牙设备连接时的服务handle
	static BTDEVHDL dev_hdl;  //接受其它蓝牙设备连接时的设备handle
	static BTUINT8 dev_connname[BTSDK_DEVNAME_LEN]; //接受其它蓝牙设备连接时的远程设备名

    static PBtSdkFileTransferReqStru pRecvFileInfo; //接受其它蓝牙设备发送某一具体文件的文件结构
    static BTUINT8 dev_recvname[BTSDK_DEVNAME_LEN]; //接受其它蓝牙设备发送某一具体文件时的远程设备名

	switch (message)
    {
	case CONFIRM_SWITCHONBTSEARCH:
		{
			BT_SendRequest(hWnd,0,RT_BT_SWITCHON,0,NULL,0);
			
			bBluetoothStatus=TRUE;	
			
			DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //通知程序管理器蓝牙打开
			
			SendMessage(hBtSettingsWnd, WM_SETBTSTATE, bBluetoothStatus, 0);
			
			PLXTipsWin(hBluetoothFrame, hWnd, TIPS_SEARCH,IDP_BT_STRING_BTSWITCHONPROMPT, 
				IDP_BT_TITLE_BLUETOOTH, Notify_Success, IDP_BT_BUTTON_OK, NULL,20);
		}
		break;

	case TIPS_SEARCH:
		SearchDevicesWindow();
		break;
		
	case WM_GETPASSWORD:
		{
			DEBUG("\r\nBluetooth:Receive WM_GETPASSWORD message\r\n");
			
			if(iActivePair==1)
			{
				WaitWin(hSearchDevicesWnd, FALSE, szWaitWinComPairing,
						IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_CANCEL,CANCELPAIRING);
			}
			else
			{
				iLockNum++;     //to resolve re-enter problem
				DEBUG("\r\nBluetooth:iLockNum=%d\r\n",iLockNum);
				if(PM_GetkeyLockStatus() ||  PM_GetPhoneLockStatus())
				{
					DlmNotify(PS_LOCK_ENABLE, FALSE);
					bChangedLock = TRUE;
					DEBUG("\r\nBluetooth:Unlock Key\r\n");
				}
			}
			
			BtEnterPinCode((BTDEVHDL)lParam);    //回调函数
			
			if(iActivePair==1)
			{
				WaitWin(hSearchDevicesWnd, TRUE, szWaitWinComPairing,
				IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_CANCEL,CANCELPAIRING);
			}
			else
			{
				iLockNum--;
				DEBUG("\r\nBluetooth:iLockNum=%d\r\n",iLockNum);
				if(iLockNum==0 && bChangedLock)
				{
					bChangedLock = FALSE;
					DlmNotify(PS_LOCK_ENABLE, TRUE);
					DEBUG("\r\nBluetooth:LOCK KEY again\r\n");
				}
				
			}
        }
		break;

    case WM_CONFIRMCONNECT:
		{			
			BTUINT16 DevNameLen;
			char szComstr1[128];
			
            svc_hdl=(BTSVCHDL)wParam;
			dev_hdl=(BTDEVHDL)lParam;
			
			memset(dev_connname,0,BTSDK_DEVNAME_LEN);
			memset(szComstr1,0,128);
			
			DevNameLen=BTSDK_DEVNAME_LEN;
			
			if(Btsdk_GetRemoteDeviceName(dev_hdl, dev_connname, &DevNameLen)!=BTSDK_OK)
			{
				Btsdk_UpdateRemoteDeviceName(dev_hdl, dev_connname, &DevNameLen);
			}
			
			strcpy(szComstr1,dev_connname);
			strcat(szComstr1,IDP_BT_STRING_ACCEPTCONNPROMPT);
			
			iLockNum++;
			DEBUG("\r\nBluetooth:iLockNum=%d\r\n",iLockNum);
			if(PM_GetkeyLockStatus() ||  PM_GetPhoneLockStatus())
			{
				DlmNotify(PS_LOCK_ENABLE, FALSE);
				bChangedLock = TRUE;
				DEBUG("\r\nBluetooth:Unlock Key\r\n");
			}

			PLXConfirmWinEx(NULL,hWnd,szComstr1, Notify_Request, 
				IDP_BT_TITLE_BLUETOOTH, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO,WM_DEALCONNECT);

		}
		break;

    case WM_DEALCONNECT:
		{
			char szComstr1[128];
			char szComstr2[128];

            memset(szComstr1,0,128);
			memset(szComstr2,0,128);

			iLockNum--;
			DEBUG("\r\nBluetooth:iLockNum=%d\r\n",iLockNum);
			if(iLockNum==0 && bChangedLock)
			{
				bChangedLock = FALSE;
				DlmNotify(PS_LOCK_ENABLE, TRUE);
				DEBUG("\r\nBluetooth:Lock Key again\r\n");
			}

			if (lParam)
			{
				AuthoriseDevService(svc_hdl,dev_hdl);

				strcpy(szComstr1,dev_connname);
				strcat(szComstr1,IDP_BT_STRING_CONNECTEDNOTIF);
				PLXTipsWin(NULL,hWnd,0,szComstr1,
					IDP_BT_TITLE_BLUETOOTH,Notify_Success,IDP_BT_BUTTON_OK,NULL,20);
			}
			else
			{
                UnauthoriseDevService(svc_hdl, dev_hdl);

				strcpy(szComstr2,dev_connname);
				strcat(szComstr2,IDP_BT_STRING_CONNDENIEDNOTIF);
				PLXTipsWin(NULL,hWnd,0,szComstr2,
					IDP_BT_TITLE_BLUETOOTH,Notify_Info,IDP_BT_BUTTON_OK,NULL,20);
			}
		}
        break;

	case WM_CONFIRMRECVDATA:
		{			
			BTUINT16 DevNameLen;
			char szComstr1[128];

            pRecvFileInfo=(PBtSdkFileTransferReqStru)lParam;

			memset(szComstr1,0,128);
			
			memset(dev_recvname,0,BTSDK_DEVNAME_LEN);
			
				DevNameLen=BTSDK_DEVNAME_LEN;
				
				if(Btsdk_GetRemoteDeviceName(pRecvFileInfo->dev_hdl, dev_recvname, &DevNameLen)!=BTSDK_OK)
				{
					Btsdk_UpdateRemoteDeviceName(pRecvFileInfo->dev_hdl, dev_recvname, &DevNameLen);
				}
				
				strcpy(szComstr1,dev_recvname);
				strcat(szComstr1,IDP_BT_STRING_RECEIVEDATAPROMPT);
				
				iLockNum++;
				DEBUG("\r\nBluetooth:iLockNum=%d\r\n",iLockNum);
				if(PM_GetkeyLockStatus() ||  PM_GetPhoneLockStatus())
				{
					DlmNotify(PS_LOCK_ENABLE, FALSE);
					bChangedLock = TRUE;
					DEBUG("\r\nBluetooth:Unlock Key\r\n");
				}
				
                PLXConfirmWinEx(NULL,hWnd,szComstr1, Notify_Request, 
					IDP_BT_TITLE_BLUETOOTH, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO,WM_DEALRECVDATA);

				
			
		}
		break;

	case WM_DEALRECVDATA:
		{
			char OldFileName[64];
			char NewFileName[64];
			
			char szComstr1[128];
			char szComstr2[128];
			
			int ret;

			iLockNum--;
			DEBUG("\r\nBluetooth:iLockNum=%d\r\n",iLockNum);
			if(iLockNum==0 && bChangedLock)
			{
				bChangedLock = FALSE;
				DlmNotify(PS_LOCK_ENABLE, TRUE);
				DEBUG("\r\nBluetooth:Lock Key again\r\n");
			}

			memset(OldFileName,0,64);
            memset(NewFileName,0,64);

			memset(szComstr1,0,128);
			memset(szComstr2,0,128);

            strcpy(OldFileName,pRecvFileInfo->file_name);
            
			if(lParam)
			{				
				ret= IsFlashAvailable(GetBtPreAcceptFileSize() / 1024);

				if(ret == SPACE_AVAILABLE || ret == SPACE_CHECKERROR)
				{
					RenameFileName(OldFileName,NewFileName);  //需要改变原来的文件名
					
					strcpy(pRecvFileInfo->file_name,NewFileName);
					
					strcpy(szComstr1,dev_recvname);
					strcat(szComstr1,IDP_BT_STRING_DATARECVSTARTEDNOTIF);
					
					pthread_cond_signal(&BtRecvData_cond);   //唤醒阻塞的线程
					
					PLXTipsWin(NULL,hWnd,0,szComstr1,IDP_BT_TITLE_BLUETOOTH,Notify_Info,
						IDP_BT_BUTTON_OK,NULL,20);
				}
				else
				{
					pRecvFileInfo->flag =BTSDK_ER_FORBIDDEN;
					
					strcpy(szComstr2,dev_recvname);
					strcat(szComstr2,IDP_BT_STRING_DATARECVDENIEDNOTIF);
					
					pthread_cond_signal(&BtRecvData_cond);   //唤醒阻塞的线程
					
					PLXTipsWin(NULL,hWnd,0,szComstr2,IDP_BT_TITLE_BLUETOOTH,Notify_Info,
						IDP_BT_BUTTON_OK,NULL,20);
				}
			}
			else
			{
				pRecvFileInfo->flag =BTSDK_ER_FORBIDDEN;
				
				strcpy(szComstr2,dev_recvname);
				strcat(szComstr2,IDP_BT_STRING_DATARECVDENIEDNOTIF);

                pthread_cond_signal(&BtRecvData_cond);   //唤醒阻塞的线程
				
				PLXTipsWin(NULL,hWnd,0,szComstr2,IDP_BT_TITLE_BLUETOOTH,Notify_Info,
					IDP_BT_BUTTON_OK,NULL,20);
			}
			
// 			pthread_cond_signal(&BtRecvData_cond);   //唤醒阻塞的线程
		}
		break;

	case WM_PINCODEFAILED:
        BtPincodeFailed((BTDEVHDL)lParam);
		break;

	case WM_PINCODESUCCESS:
		BtPincodeSuccess((BTDEVHDL)lParam);
		break;
		
	default:    
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}

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
#include "imesys.h"
#include "pubapp.h"

#include "BtMain.h"
#include "BtThread.h"
#include "BtString.h"

#define ID_VISIBILITYSPINBOX   (WM_USER+0x200)

#define CONFIRM_VISIBILITYSWITCHONBT   (WM_USER+0x300)
#define CONFIRM_PHONENAMESWITCHONBT    (WM_USER+0x301)
 
HWND hBtSettingsStatus;            //全局变量，Settings窗口中Bluetooth是否打开的SpinBox控件句柄
HWND hBtSettingsVisibility;        //全局变量，Settings窗口中Bluetooth是否可见的SpinBox控件句柄
HWND hBtSettingsPhoneName;         //全局变量，Settings窗口中Bluetooth设备名的SpinBox控件句柄

HWND hBtSettingsWnd;               //全局变量，Settings窗口句柄

extern BOOL  bBluetoothStatus;
extern BOOL  bVisibilityStatus;
extern char  szPhoneName[MAX_PHONENAME_LEN];

extern	BOOL	bCallByShortcur;

static LRESULT SettingsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

extern HWND GetBluetoothFrameWnd();

extern BOOL BtChangeStatusWindow();
extern BOOL BtVisibilityWindow();
extern BOOL BtPhoneNameWindow();
extern BOOL	CallBTMainWnd(HWND hParent);

BOOL CreateSettingsWindow()
{
	HWND hBluetoothFrame;
	RECT rClient;
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = SettingsWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "SettingsWndClass";

	if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);

    hBtSettingsWnd = CreateWindow(
		"SettingsWndClass",
		"", 
        WS_VISIBLE | WS_CHILD,
        rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hBluetoothFrame, 
		NULL, 
		NULL, 
		NULL);

    if (NULL ==  hBtSettingsWnd)
    {
        UnregisterClass("SettingsWndClass",NULL);
        return FALSE;
    }
    
	SetFocus(hBtSettingsWnd);
	
	ShowWindow(hBtSettingsWnd,SW_SHOW);
	UpdateWindow(hBtSettingsWnd);

    return (TRUE);
}

static LRESULT SettingsWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
    LRESULT lResult=TRUE;
	HWND hBluetoothFrame;
	
    HWND hWndFocus;
	static HWND hFocus;

	static BOOL bOldBluetoothStatus;  //没有改变前蓝牙的状态

	hBluetoothFrame=GetBluetoothFrameWnd();
    	
	switch (message)
    {
    case WM_CREATE :
		{
			int  xzero=0,yzero=0,iControlH,iControlW;
			RECT rClient;
			
			GetClientRect(hWnd, &rClient);
			
			iControlH = rClient.bottom/3;
			iControlW = rClient.right;
			
			hBtSettingsStatus = CreateWindow(      //蓝牙打开关闭状态的spinbox
				"SPINBOXEX", 
				IDP_BT_TITLE_BLUETOOTH,
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP 
				| SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
				xzero,yzero, 
				iControlW, iControlH, 
				hWnd, 
				NULL,
				NULL, 
				NULL);
			
			if(NULL==hBtSettingsStatus)
				return FALSE;
			
			if(bBluetoothStatus)                             //保存原来Bluetooth设置的状态
				SendMessage(hBtSettingsStatus, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_ON);
			else
				SendMessage(hBtSettingsStatus, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_OFF);
			
			hBtSettingsVisibility = CreateWindow(      //蓝牙可见性状态的spinbox
				"SPINBOXEX", 
				IDP_BT_TITLE_VISIBILITY,
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP 
				| SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
				xzero,yzero+iControlH, 
				iControlW, iControlH, 
				hWnd, 
				(HMENU)ID_VISIBILITYSPINBOX,
				NULL, 
				NULL);
			
			if(NULL==hBtSettingsVisibility)
				return FALSE;
			
			if(bBluetoothStatus)
			{
				SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_ON);
				SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_OFF);
				
				if(bVisibilityStatus)                            //保存原来Visibility设置的状态
					SendMessage(hBtSettingsVisibility,SSBM_SETCURSEL,0,0);
				else
					SendMessage(hBtSettingsVisibility,SSBM_SETCURSEL,1,0);
			}
			else
			{
				if(bVisibilityStatus) 
					SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_ON);
				else
					SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_OFF);
			}
			
			hBtSettingsPhoneName = CreateWindow(      //设置蓝牙名字的spinbox
				"SPINBOXEX", 
				IDP_BT_STRING_PHONENAME,
				WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_TABSTOP 
				| SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
				xzero,yzero+iControlH*2, 
				iControlW, iControlH, 
				hWnd, 
				NULL,
				NULL, 
				NULL);
			
			if(NULL==hBtSettingsPhoneName)
				return FALSE;
			
			SendMessage(hBtSettingsPhoneName, SSBM_ADDSTRING,0,(LPARAM)(szPhoneName));
			
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			
			SetWindowText(hBluetoothFrame,IDP_BT_TITLE_SETTINGS);
		
		    hFocus = hBtSettingsStatus;   //默认的焦点放在设置蓝牙打开和关闭的选择项上
		}
        break;
		
	case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case PWM_SHOWWINDOW:
        {
			LONG OldSel;

			printf("\n#########In BTSetttings PWM_SHOWWINDOW##########\n");
			
            if(!bOldBluetoothStatus && bBluetoothStatus)   //蓝牙的状态从关闭到打开
				                                           //设置蓝牙可见性相应的状态
			{ 
				SendMessage(hBtSettingsVisibility,SSBM_DELETESTRING,0,0);

				SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_ON);
				SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_OFF);
                
				if(bVisibilityStatus)
				   SendMessage(hBtSettingsVisibility, SSBM_SETTEXT,0, (LPARAM)IDP_BT_STRING_ON); 
				else
				   SendMessage(hBtSettingsVisibility, SSBM_SETTEXT,0, (LPARAM)IDP_BT_STRING_OFF); 
			}

			if(bOldBluetoothStatus && !bBluetoothStatus)  //蓝牙的状态从打开到关闭
				                                          //设置蓝牙可见性相应的状态
			{
				OldSel = SendMessage(hBtSettingsVisibility, SSBM_GETCURSEL, 0, 0);

                SendMessage(hBtSettingsVisibility,SSBM_DELETESTRING,1,0);   //先删第二项
				SendMessage(hBtSettingsVisibility,SSBM_DELETESTRING,0,0);   //再删第一项

                if(OldSel==0)
                   SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_ON);
				else
                   SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_OFF);
			}

			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			
			SetWindowText(hBluetoothFrame,IDP_BT_TITLE_SETTINGS);
			SetFocus(hFocus);
		}
        break;

	case WM_KEYDOWN:
		{			
			switch (LOWORD(wParam))
			{
			case VK_F5:		

				if(hFocus==hBtSettingsStatus)
				{
                    bOldBluetoothStatus=bBluetoothStatus;   //保存原来蓝牙的状态

					BtChangeStatusWindow();
				}
				else if(hFocus==hBtSettingsVisibility)
				{
					if(!bBluetoothStatus)
					{
						PLXConfirmWinEx(hBluetoothFrame,hWnd,IDP_BT_STRING_BTSWITCHONPROMPT, Notify_Request, 
							IDP_BT_TITLE_BLUETOOTH, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO,CONFIRM_VISIBILITYSWITCHONBT);
					}
                    else
					{
					   bOldBluetoothStatus=bBluetoothStatus;

					   BtVisibilityWindow();
					}
				}
				else
				{
                    if(!bBluetoothStatus)
                    {
						PLXConfirmWinEx(hBluetoothFrame,hWnd,IDP_BT_STRING_BTSWITCHONPROMPT, Notify_Request, 
							IDP_BT_TITLE_BLUETOOTH, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO,CONFIRM_PHONENAMESWITCHONBT);
                    }
                    else
					{
                        bOldBluetoothStatus=bBluetoothStatus;

					    BtPhoneNameWindow();
					}
				}
				break;
				
			case VK_F10:
				WriteStatusFile(BTSTATUSFILENAME);   //退出时把当前的蓝牙设置信息写入文件
				CallBTMainWnd(hBluetoothFrame);
				SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				
				break;
		    
			case VK_DOWN:
				{
					hWndFocus= GetFocus();
					
					while(GetParent(hWndFocus) != hWnd)
					{
						hWndFocus = GetParent(hWndFocus);
					}
					
					hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, FALSE);
					
					SetFocus(hWndFocus);
					hFocus=hWndFocus;
				}
				break;
				
			case VK_UP:
				{
					hWndFocus = GetFocus();
					
					while(GetParent(hWndFocus) != hWnd)
					{
						hWndFocus = GetParent(hWndFocus);
					}
					
					hWndFocus = GetNextDlgTabItem(hWnd, hWndFocus, TRUE);
					SetFocus(hWndFocus);
					hFocus=hWndFocus;
				}
			    break;	
				
			default:
				lResult = PDADefWindowProc(hWnd, WM_KEYDOWN, wParam, lParam);
				break;
			}
		}
		break;
		
	case CONFIRM_VISIBILITYSWITCHONBT:
		if (lParam)
		{
#ifndef _EMULATE_
			BT_SendRequest(hWnd,0,RT_BT_SWITCHON,0,NULL,0);
#endif			
			bBluetoothStatus=TRUE;	
			
            DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //通知程序管理器蓝牙打开
			
			SendMessage(hBtSettingsWnd, WM_SETBTSTATE, bBluetoothStatus, 0);
			
			PLXTipsWin(hBluetoothFrame, hWnd, 0,IDP_BT_STRING_BTSWITCHONNOTIF, IDP_BT_TITLE_BLUETOOTH,
				Notify_Success, IDP_BT_BUTTON_OK, NULL,20);

			bOldBluetoothStatus=bBluetoothStatus;
            
			BtVisibilityWindow();
			
		}
		break;

	case CONFIRM_PHONENAMESWITCHONBT:
		if (lParam)
		{
#ifndef _EMULATE_
            BT_SendRequest(hWnd,0,RT_BT_SWITCHON,0,NULL,0);
#endif			
			bBluetoothStatus=TRUE;	
			
            DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //通知程序管理器蓝牙打开
			
			SendMessage(hBtSettingsWnd, WM_SETBTSTATE, bBluetoothStatus, 0);
			
			PLXTipsWin(hBluetoothFrame, hWnd, 0,IDP_BT_STRING_BTSWITCHONNOTIF, IDP_BT_TITLE_BLUETOOTH,
				Notify_Success, IDP_BT_BUTTON_OK, NULL,20);
			
			bOldBluetoothStatus=bBluetoothStatus;

            BtPhoneNameWindow();
		}
		break;

	case WM_SETBTSTATE:	
		if( LOWORD(wParam) )
			SendMessage(hBtSettingsStatus, SSBM_SETTEXT, 
			0, (LPARAM)IDP_BT_STRING_ON);
		else
			SendMessage(hBtSettingsStatus, SSBM_SETTEXT, 
			0, (LPARAM)IDP_BT_STRING_OFF);
		break;
		
	case WM_SETVISIBILITYSTATE:
        if( wParam==TRUE )
			SendMessage(hBtSettingsVisibility, SSBM_SETTEXT, 
			0, (LPARAM)IDP_BT_STRING_ON);
		else
			SendMessage(hBtSettingsVisibility, SSBM_SETTEXT, 
			0, (LPARAM)IDP_BT_STRING_OFF);		
		break;
    
	case WM_COMMAND:
		{   
			LONG curSel;
			int nVisibilityCount;
			
			switch(LOWORD(wParam))
			{
			case ID_VISIBILITYSPINBOX:
				switch(HIWORD(wParam))
				{
				case SSBN_CHANGE:

					nVisibilityCount=SendMessage(hBtSettingsVisibility,SSBM_GETCOUNT,0,0);
					
					if(nVisibilityCount>1)
					{
						curSel = SendMessage(hBtSettingsVisibility,SSBM_GETCURSEL,0,0);
						
						if(curSel==0)
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
					}
                    
					break;
				default:
					break;
				}
				default:
					break;
			}
		}
		break;
		
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
		
    case WM_DESTROY :
        UnregisterClass("SettingsWndClass", NULL);
        break;

    default:    
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}

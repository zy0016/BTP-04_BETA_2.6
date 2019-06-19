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
#include "stdio.h"
#include "plx_pdaex.h"
#include "mullang.h"
#include "pubapp.h"
#include "imesys.h"

#include <stdlib.h>
#include <string.h>

#include "BtMain.h"
#include "BtDeviceNode.h"
#include "BtThread.h"
#include "BtPairedDevNode.h"
#include "BtString.h"

#define ID_SEARCHDEVICESLIST          (WM_USER+0x200)
#define ID_SEARCH_TIMER				  (WM_USER+0x201)

#define STOPDISCOVERDEV               (WM_USER+0x300)
#define TIPS_PAIR                     (WM_USER+0x302)
#define BT_MSG_PAIR					  (WM_USER+0x303)

HWND hSearchDevicesWnd;  
char szWaitWinComPairing[64];

int  iActivePair=0;   //是否是主动配对

extern BTINT32 g_rmt_dev_num;
extern BOOL bStopSearchDev;
// extern pthread_mutex_t BtStopSearch_mutex;
 
static HWND hSearchDeviceslist;

DeviceNodeLink FoundDevListHead=NULL;   //全局变量，存放找到的设备的链表头
extern PPAIREDDEVNODE pPairedDevHead; 
extern pthread_mutex_t BtDeviceNodeListMutex;

extern HWND GetBluetoothFrameWnd();
extern	BOOL CallPairedDevicesWnd();
static LRESULT SearchDevicesWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void SearchDevices_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void SearchDevices_OnPaint(HWND hWnd);

BOOL SearchDevicesWindow()
{
    WNDCLASS    wc;
	HWND hBluetoothFrame;
	RECT rClient;

    wc.style         = 0;
    wc.lpfnWndProc   = SearchDevicesWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "SearchDevicesWndClass";

	if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);

    hSearchDevicesWnd = CreateWindow(
		"SearchDevicesWndClass",
		"", 
        WS_VISIBLE | WS_CHILD,
        rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hBluetoothFrame, 
		NULL, 
		NULL, 
		NULL);

	if (NULL ==  hSearchDevicesWnd)
    {
        UnregisterClass("SearchDevicesWndClass",NULL);
        return FALSE;
    }
	
    SetFocus(hSearchDevicesWnd);
	
	ShowWindow(hSearchDevicesWnd,SW_SHOW);
	UpdateWindow(hSearchDevicesWnd);

    return (TRUE);
}

static LRESULT SearchDevicesWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{    
	HWND hBluetoothFrame;
	LRESULT lResult=TRUE;
	LONG curSel;

	static char TempBuff[256];   //存放提示找到的设备

	static int iIDSearchDevice,iIDPairDevice;

	static HBITMAP SearchDevBitmap,MobilePhoneBitmap,HandheldBitmap;
	static HBITMAP PCBitmap,HeadsetBitmap,OtherBitmap;

	hBluetoothFrame=GetBluetoothFrameWnd();
	
    switch (message)
    {
    case WM_CREATE :	
		{
			int curIndex;
			DeviceNodeLink Pointer;	
			RECT rClient;
			HBITMAP CurFoundBitmap;

			SearchDevBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_search.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			
			MobilePhoneBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory1.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			HandheldBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory2.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			PCBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory3.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			HeadsetBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory4-b.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			OtherBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory4-a.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			
			GetClientRect(hWnd, &rClient);
			
			hSearchDeviceslist=CreateWindow(
				"LISTBOX",
				"",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL| LBS_BITMAP,
				rClient.left, rClient.top,
				rClient.right-rClient.left, rClient.bottom-rClient.top,
				hWnd,
				(HMENU)ID_SEARCHDEVICESLIST,
				NULL,
				NULL);
			
			if(NULL == hSearchDeviceslist)
				return FALSE;

			curIndex=SendMessage(hSearchDeviceslist, LB_ADDSTRING, 0, (LPARAM)IDP_BT_STRING_SEARCHFORDEVICES);
			SendMessage(hSearchDeviceslist, LB_SETIMAGE, 
					(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)SearchDevBitmap); 
				
			Pointer=FoundDevListHead;
				
			while(Pointer!=NULL)
			{	
				switch(DistinguishDevCls(Pointer->DevClass))     //得到对应设备的图标
				{
				case MOBILEPHONE:
					CurFoundBitmap=MobilePhoneBitmap;
					break;
				case HANDHELD:
					CurFoundBitmap=HandheldBitmap;
					break;
				case PC:
					CurFoundBitmap=PCBitmap;
					break;
				case HEADSET:
					CurFoundBitmap=HeadsetBitmap;
					break;
				default:
					CurFoundBitmap=OtherBitmap;
					break;
				}
									
				curIndex=SendMessage(hSearchDeviceslist, LB_INSERTSTRING, 1, (LPARAM)Pointer->DevName);
				SendMessage(hSearchDeviceslist, LB_SETIMAGE, 
						(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)CurFoundBitmap);

				SendMessage(hSearchDeviceslist,LB_SETITEMDATA,
					(WPARAM)curIndex,(LPARAM)Pointer->DevHandle);
					
				Pointer=Pointer->Next;
			}
						
			SendMessage(hSearchDeviceslist, LB_SETCURSEL, 0, 0);
			
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			
			SetWindowText(hBluetoothFrame,IDP_BT_TITLE_NEWDEVICE);
		}
        break;
    
	case WM_SETFOCUS:
        SetFocus(hSearchDeviceslist);
        break;
		
    case PWM_SHOWWINDOW:
		{
			int i, nCount;
			DeviceNodeLink Pointer;
			int curIndex;					
			HBITMAP CurFoundBitmap;

            DEBUG("\r\nBluetooth:PWM_SHOWWINDOW when finishing search device\r\n");

            nCount=SendMessage(hSearchDeviceslist,LB_GETCOUNT,0,0);
			
			if(nCount>1)
			{
				for (i = nCount - 1; i > 0 ; i--)
				{
					SendMessage(hSearchDeviceslist,LB_DELETESTRING,(WPARAM)i,0);
				}
			}
			
			Pointer=FoundDevListHead;
			
			SendMessage(hSearchDeviceslist, LB_SETCURSEL, 0, 0);
			
			while(Pointer!=NULL)
			{	
				switch(DistinguishDevCls(Pointer->DevClass))     //得到对应设备的图标
				{
				case MOBILEPHONE:
					CurFoundBitmap=MobilePhoneBitmap;
					break;
				case HANDHELD:
					CurFoundBitmap=HandheldBitmap;
					break;
				case PC:
					CurFoundBitmap=PCBitmap;
					break;
				case HEADSET:
					CurFoundBitmap=HeadsetBitmap;
					break;
				default:
					CurFoundBitmap=OtherBitmap;
					break;
				}
				
				curIndex=SendMessage(hSearchDeviceslist, LB_INSERTSTRING, 1, (LPARAM)Pointer->DevName);
				
				SendMessage(hSearchDeviceslist, LB_SETIMAGE, 
					(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)CurFoundBitmap);
				
				SendMessage(hSearchDeviceslist,LB_SETITEMDATA,
					(WPARAM)curIndex,(LPARAM)Pointer->DevHandle);
				
				Pointer=Pointer->Next;
			}
		   
 		   curSel = SendMessage(hSearchDeviceslist, LB_GETCURSEL, 0, 0);
		   SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
		   
		   if(0==curSel)
		   {
		       SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		       SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
		   }
		   else			
		   {
			   SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_PAIR);
			   SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");
		   }	

		     SetWindowText(hBluetoothFrame,IDP_BT_TITLE_NEWDEVICE);
		     SetFocus(hSearchDeviceslist);
		}
        break;
    
	case WM_COMMAND:
		SearchDevices_OnCommand(hWnd, wParam, lParam);         
 		break;

	case WM_GETREMOTEDEVINFO:
		{
			DEBUG("\r\nBluetooth:receive WM_GETREMOTEDEVINFO message from callback\r\n");
			if(bStopSearchDev)
			{
				DEBUG("\r\nBluetooth:receive WM_GETREMOTEDEVINFO message After stop,discard it \r\n");
				return FALSE;
			}

			BT_SendRequest(NULL,0,RT_BT_GETREMOTEDEVINFO,&lParam,NULL,0);
		}
		break;
		
	case WM_UPDATEREMOTEDEVINFO:
		{
			DEBUG("\r\nBluetooth:receive WM_UPDATEREMOTEDEVINFO message from callback\r\n");
			if(bStopSearchDev)
			{
				DEBUG("\r\nBluetooth:receive WM_UPDATEREMOTEDEVINFO message After stop,discard it \r\n");
				return FALSE;
			}

			if(g_rmt_dev_num==0) //if there is no device found
			{
				DEBUG("\r\nBluetooth:WM_UPDATEREMOTEDEVINFO,There is no device found,stack timeout\r\n");
				PostMessage(hWnd,WM_UPDATEREMOTEDEVCOMPLETED,0,0);
				return FALSE;
			}
			
			BT_SendRequest(hWnd,0,RT_BT_UPDATEREMOTEDEVINFO,NULL,NULL,0);
		}
		break;

	case TIPS_PAIR:
        PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;

	case STOPDISCOVERDEV:    //停止查找设备
		{
			printf("\n$$$$$$$$$$$$$In STOPDISCOVERDEV$$$$$$$$$$$$$$$$$\n");

		    StopDiscoverDevice();
           
            bStopSearchDev=TRUE;
		}
		break;

	case CANCELPAIRING:
		{
		  iActivePair=0;
          //StopPairDevice(pCurPointer->DevHandle);
		  BT_CancelRequest(iIDPairDevice);
		}
		break;

	case WM_UPDATEREMOTEDEVCOMPLETED:
		{			
			if((int)lParam>0)
			{
				DEBUG("\r\nBluetooth:WM_UPDATEREMOTEDEVCOMPLETED:There is some device found,close waitwin\r\n");
				
				WaitWin(hWnd, FALSE, IDP_BT_STRING_SEARCHFORDEVICESPROMPT,
					IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_STOP,STOPDISCOVERDEV);
				
			}	
			else
			{	
                DEBUG("\r\nBluetooth:WM_UPDATEREMOTEDEVCOMPLETED:There is no device found,stack timeout,close waitwin\r\n");
				
				WaitWin(hWnd, FALSE, IDP_BT_STRING_SEARCHFORDEVICESPROMPT,
					IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_STOP,STOPDISCOVERDEV);
				
				PLXTipsWin(hBluetoothFrame, hWnd, 0,IDP_BT_STRING_NODEVICESFOUND, IDP_BT_TITLE_NEWDEVICE,
					Notify_Failure, IDP_BT_BUTTON_OK, NULL,20);
			}
		}
		break;
   
	case BT_MSG_PAIR:
		{
			char szCurSelDevice[64];
			char szMsgComPairing[64];

			curSel = SendMessage(hSearchDeviceslist, LB_GETCURSEL, 0, 0);

			SendMessage(hSearchDeviceslist,LB_GETTEXT,
				(WPARAM)curSel,(LPARAM)(LPCTSTR)szCurSelDevice);
			
			strcpy(szMsgComPairing,szCurSelDevice);
			strcat(szMsgComPairing,IDP_BT_STRING_PAIRING);	
			
		    switch(wParam) 
			{
		      case BTSDK_OK:
				{
			      char szComSuccess[64];
				  
                  iActivePair=0;   

			      WaitWin(hWnd, FALSE, szMsgComPairing,
				       IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_CANCEL,CANCELPAIRING);

				  strcpy(szComSuccess,szCurSelDevice);
				  strcat(szComSuccess,IDP_BT_STRING_PAIRINGSUCCESSFUL);

				  printf("====PLXTipsWin\r\n");
				  
				  PLXTipsWin(hBluetoothFrame, hWnd,TIPS_PAIR ,szComSuccess, IDP_BT_TITLE_NEWDEVICE,
					  Notify_Success, IDP_BT_BUTTON_OK, NULL,20);			 
          
				}	
			   break;
		    default:
				{
					char szComFailed[64];

					iActivePair=0; 

                    WaitWin(hWnd, FALSE, szMsgComPairing,
						IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_CANCEL,CANCELPAIRING);
					
					strcpy(szComFailed,szCurSelDevice);
					strcat(szComFailed,IDP_BT_STRING_PAIRINGFAILED);
					
					PLXTipsWin(hBluetoothFrame, hWnd,TIPS_PAIR ,szComFailed, IDP_BT_TITLE_NEWDEVICE,
						Notify_Failure, IDP_BT_BUTTON_OK, NULL,20);

				}
			}
		}
		break;

	case WM_FOUNDDEVICE:
		{
		   DEBUG("\r\nBluetooth:Remote Device Name updated succss\r\n");
	       
		   strcat(TempBuff,(char *)lParam);
           strcat(TempBuff,"\n");
 		
		   UpdateWaitwinContext(TempBuff);
		   InvalidateRect(NULL, NULL, TRUE);
		   UpdateScreen();
		   
		}
		break;
    	
	case WM_KEYDOWN:	
		{
			LONG curSel;
			char szCurSelDevice[64];         //当前所选的设备
			
			BTDEVHDL CurDevHdl;
			HWND hBluetoothFrame;
			
			hBluetoothFrame=GetBluetoothFrameWnd();
			
			switch (LOWORD(wParam))
			{
			case VK_F5:	
				
				curSel = SendMessage(hSearchDeviceslist, LB_GETCURSEL, 0, 0);
				
				if (0 == curSel) 
				{					
					WaitWin(hWnd, TRUE, IDP_BT_STRING_SEARCHFORDEVICESPROMPT,
						IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_STOP,STOPDISCOVERDEV);
// 			        UpdateScreen();
					
					memset(TempBuff,0,256);

					strcpy(TempBuff,IDP_BT_STRING_SEARCHDEVICESFOUND);

					iIDSearchDevice=BT_SendRequest(hWnd,0,RT_BT_SEARCHDEVICE,0,NULL,0);  
					//向蓝牙辅线程助发出搜索设备的请求

// 					pthread_mutex_lock(&BtStopSearch_mutex);
					bStopSearchDev=FALSE;
// 					pthread_mutex_unlock(&BtStopSearch_mutex);
					
				}
				
				break;
				
			case VK_RETURN:
				curSel=SendMessage(hSearchDeviceslist, LB_GETCURSEL, 0, 0);
				
				if(curSel!=0)
				{			
					SendMessage(hSearchDeviceslist, LB_GETTEXT, (WPARAM)curSel, (LPARAM) (LPCTSTR)szCurSelDevice);

					CurDevHdl=SendMessage(hSearchDeviceslist,LB_GETITEMDATA,
						                         (WPARAM)curSel,0);
										
					strcpy(szWaitWinComPairing,szCurSelDevice);
					strcat(szWaitWinComPairing,IDP_BT_STRING_PAIRING);
					
					WaitWin(hWnd, TRUE, szWaitWinComPairing,
						IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_CANCEL,CANCELPAIRING);
					
					iIDPairDevice=BT_SendRequest(hWnd,BT_MSG_PAIR,RT_BT_PAIR,&CurDevHdl,NULL,0);
					//向蓝牙辅线程助发出配对设备的请求

					iActivePair=1;    //1 表示本地蓝牙设备主动配对其它蓝牙设备
					                  //0 表示其它蓝牙设备配对本地蓝牙设备
				}
				break;
				
			case VK_F10:
				CallPairedDevicesWnd();
				SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
				PostMessage(hWnd,WM_CLOSE,0,0);
				
				break;
			default:
				PDADefWindowProc(hWnd, WM_KEYDOWN, wParam, lParam);
				break;
			}
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);        
		break;
		
    case WM_DESTROY :
		{
			UnregisterClass("SearchDevicesWndClass", NULL);
			
			if(SearchDevBitmap)                    //释放图标占用的内存
				DeleteObject(SearchDevBitmap);
			if(MobilePhoneBitmap)
				DeleteObject(MobilePhoneBitmap);
			if(HandheldBitmap)
				DeleteObject(HandheldBitmap);
			if(PCBitmap)
				DeleteObject(PCBitmap);
			if(HeadsetBitmap)
				DeleteObject(HeadsetBitmap);
			if(OtherBitmap)
				DeleteObject(OtherBitmap);
		}
        break;

    default:    
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}


static void SearchDevices_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LONG curSel; 
	HWND hBluetoothFrame;
	
    hBluetoothFrame=GetBluetoothFrameWnd();

	switch(LOWORD(wParam))
	{
	case ID_SEARCHDEVICESLIST:
		
		switch(HIWORD(wParam))
		{
		case LBN_SELCHANGE:
			{
				curSel = SendMessage(hSearchDeviceslist, LB_GETCURSEL, 0, 0);
				if(curSel==0)
				{
					SendMessage(hBluetoothFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
					SendMessage(hBluetoothFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}   
				else
				{
					SendMessage(hBluetoothFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_BT_BUTTON_PAIR);
					SendMessage(hBluetoothFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");   
				}

			}				
			break;
		}
	break;
	}
	
}

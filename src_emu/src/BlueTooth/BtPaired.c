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
#include "hpdef.h"

#include <stdlib.h>
#include <string.h>

#include "BtMain.h"
#include "BtDeviceNode.h"
#include "BtThread.h"
#include "BtPairedDevNode.h"
#include "BtString.h"

#define ID_PAIREDBLUETOOTHLIST          (WM_USER+0x200)
#define ID_CONNECT                      (WM_USER+0x201)  
#define ID_DISCONNECT                   (WM_USER+0x202)
#define ID_RENAME                       (WM_USER+0x203)
#define ID_AUTHORISE                    (WM_USER+0x204)
#define ID_UNAUTHORISE                  (WM_USER+0x205)
#define ID_DELETE                       (WM_USER+0x206)
#define ID_DELETESELECT                 (WM_USER+0x207)
#define ID_DELETEALL                    (WM_USER+0x208)

#define CONFIRM_SWITCHONBTSEARCH 	    (WM_USER+0x300)
#define CONFIRM_SWITCHONBTCONNECT       (WM_USER+0x301)
#define CONFIRM_DISCONNECT              (WM_USER+0x302)
#define CONFIRM_AUTHORISE               (WM_USER+0x303)
#define CONFIRM_DELPAIR                 (WM_USER+0x304)
#define CONFIRM_DELALLPAIR              (WM_USER+0x305)
#define CANCELCONNECTING                (WM_USER+0x306)

#define BT_MSG_CONNECT_HEADSET          (WM_USER+0x307)

#define TIPS_SEARCH                     (WM_USER+0x400)
#define TIPS_CONNECT                    (WM_USER+0x401)

HWND hPairedWnd;  
HWND hPairedlist;    

PPAIREDDEVNODE pPairedDevHead=NULL;

static HMENU hPairedMainMenu;
static HMENU hPairedDelManyMenu;

extern BOOL  bBluetoothStatus; 
extern HWND  hBtSettingsWnd;

static LRESULT PairedWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void Paired_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);
static void Paired_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

static void Paired_Connecting(HWND hWnd,LONG curSel);
static void Paired_Disconnect(HWND hWnd,LONG curSel);
static void Paired_Authorise(HWND hWnd,LONG curSel);
static void Paired_Unauthorise(HWND hWnd,LONG curSel);
static void Paired_Delete(HWND hWnd,LONG curSel);

static void DelAllPaired(HWND hWnd);

static BOOL IsSomeDevConnected();

extern HWND GetBluetoothFrameWnd();

extern BOOL SearchDevicesWindow();
extern BOOL PairedRenameWindow(LONG curSel);
extern BOOL DelMulSelPairedWindow();
extern	BOOL	CallBTMainWnd(HWND hParent);

static void CreatePairedOptionsMenu();
static void PairedChangeMenu();

BOOL CreatePairedDevicesWindow()
{
	HWND hBluetoothFrame;
	RECT rClient;
    WNDCLASS    wc;

    wc.style         = 0;
    wc.lpfnWndProc   = PairedWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "PairedWndClass";

	if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);

    hPairedWnd = CreateWindow(
		"PairedWndClass",
		"", 
        WS_VISIBLE | WS_CHILD,
        rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hBluetoothFrame, 
		NULL, 
		NULL, 
		NULL);

	if (NULL ==  hPairedWnd)
    {
        UnregisterClass("PairedWndClass",NULL);
        return FALSE;
    }
	 
   	SetFocus(hPairedWnd);
	
	ShowWindow(hPairedWnd,SW_SHOW);
	UpdateWindow(hPairedWnd);
	
    return (TRUE);
}


BOOL CallPairedDevicesWnd()
{
	HWND hBluetoothFrame;
	RECT rClient;
    WNDCLASS    wc;

	if (hPairedWnd)
	{
		return TRUE;
	}
    wc.style         = 0;
    wc.lpfnWndProc   = PairedWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "PairedWndClass";

	if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);

    hPairedWnd = CreateWindow(
		"PairedWndClass",
		"", 
        WS_VISIBLE | WS_CHILD,
        rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hBluetoothFrame, 
		NULL, 
		NULL, 
		NULL);

	if (NULL ==  hPairedWnd)
    {
        UnregisterClass("PairedWndClass",NULL);
        return FALSE;
    }
	 
   	SetFocus(hPairedWnd);
	
	ShowWindow(hPairedWnd,SW_SHOW);
	UpdateWindow(hPairedWnd);
	
    return TRUE;
}


static LRESULT PairedWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	LONG curSel;
	LRESULT lResult=TRUE;
	HWND hBluetoothFrame;
    static HBITMAP NewDevBitmap,MobilePhoneBitmap,HandheldBitmap;
	static HBITMAP PCBitmap,HeadsetBitmap,OtherBitmap;
	static HBITMAP AuthorBitmap;

	char curSelPairedName[32];
	char StrDisconnected[64];
	char StrAuthorised[64];

	static BTCONNHDL CurConn_hdl;    //本次建立起来连接的handle;

	hBluetoothFrame=GetBluetoothFrameWnd();

    switch (message)
    {
    case WM_CREATE :
		{
			int curIndex;
			RECT rClient;
			HBITMAP CurPairedBitmap;
			
			PPAIREDDEVNODE pCurPairedPointer;
			BTUINT32 dev_class=0;		
			
 			static BOOL bFirstOpenFile=TRUE;

			NewDevBitmap = LoadImage(NULL, "/rom/bluetooth/bluetooth_newdevice.bmp", 
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
			AuthorBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_authorised.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			
			GetClientRect(hWnd, &rClient);

			CreatePairedOptionsMenu();

			PDASetMenu(hBluetoothFrame,hPairedMainMenu);
			
			hPairedlist=CreateWindow(
				"LISTBOX",
				"",
				WS_VISIBLE | WS_CHILD | WS_VSCROLL| LBS_BITMAP,
				rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom - rClient.top,
				hWnd,
				(HMENU)ID_PAIREDBLUETOOTHLIST,
				NULL,
				NULL);
			
			if(NULL == hPairedlist)
				return FALSE;
			
			curIndex=SendMessage(hPairedlist, LB_ADDSTRING, 0, (LPARAM)IDP_BT_TITLE_NEWDEVICE);
			
			SendMessage(hPairedlist, LB_SETIMAGE, 
				(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)NewDevBitmap);
			
			if(bFirstOpenFile)
			{			
				pPairedDevHead=GetPairedDevInfoFromFile();
 				bFirstOpenFile=FALSE;
 			}
			
			pCurPairedPointer=pPairedDevHead;
			
			while(pCurPairedPointer!=NULL)
			{	
				GetDeviceClass(pCurPairedPointer->PairedDevHandle, &dev_class);     //得到已配对的设备类

				switch(DistinguishDevCls(dev_class))
				{
				case MOBILEPHONE:
					CurPairedBitmap=MobilePhoneBitmap;
					break;
				case HANDHELD:
					CurPairedBitmap=HandheldBitmap;
					break;
				case PC:
					CurPairedBitmap=PCBitmap;
					break;
				case HEADSET:
					CurPairedBitmap=HeadsetBitmap;
					break;
				default:
					CurPairedBitmap=OtherBitmap;
					break;
				}
								
				if(strlen(pCurPairedPointer->PairedDevAliasName)!=0)
					curIndex=SendMessage(hPairedlist, LB_INSERTSTRING, 1, 
					              (LPARAM)pCurPairedPointer->PairedDevAliasName);
				else
				    curIndex=SendMessage(hPairedlist, LB_INSERTSTRING, 1, 
					              (LPARAM)pCurPairedPointer->PairedDevName);
				
				if(pCurPairedPointer->PairedDevAuthor)  	//如果原来关机前设置为授权设备
				{
					SetAuthoriseDevice(pCurPairedPointer->PairedDevHandle);  
						
					SendMessage(hPairedlist, LB_SETIMAGE, 
						(WPARAM)MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(curIndex,1)),(LPARAM)AuthorBitmap);
				}
				
				SendMessage(hPairedlist, LB_SETIMAGE, 
					(WPARAM)MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(curIndex,0)),(LPARAM)CurPairedBitmap);
				
				SendMessage(hPairedlist,LB_SETITEMDATA,
					(WPARAM)curIndex,(LPARAM)pCurPairedPointer->PairedDevHandle);
				
				pCurPairedPointer=pCurPairedPointer->Next;
			}
			
			SendMessage(hPairedlist, LB_SETCURSEL, 0, 0);
			
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
			
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");  
			
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");

			SetWindowText(hBluetoothFrame,IDP_BT_TITLE_PAIREDDEVICES);
		}
		
        break;

	case WM_SETFOCUS:
         SetFocus(hPairedlist);
        break;
		
    case PWM_SHOWWINDOW:
		{
			int curIndex;
			int i;
			int nCount;

			HBITMAP CurPairedBitmap;

			PPAIREDDEVNODE pCurPairedPointer;
			BTDEVHDL CurDevHdl;
            PPAIREDDEVNODE pListPointer;

			BTUINT32 dev_class=0;	
            BTUINT32 dev_cls=0;

			curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);   //保存原来窗口Listbox的焦点
            		
			nCount=SendMessage(hPairedlist,LB_GETCOUNT,0,0);
            
			if(nCount>1)
			{
				for (i = nCount - 1; i > 0 ; i--)
				{
					SendMessage(hPairedlist,LB_DELETESTRING,(WPARAM)i,0);
				}
			}
			
			pCurPairedPointer=pPairedDevHead;
			
			while(pCurPairedPointer!=NULL)
			{						
				GetDeviceClass(pCurPairedPointer->PairedDevHandle, &dev_class);     //得到已配对的设备类

                switch(DistinguishDevCls(dev_class))
				{
				case MOBILEPHONE:
					CurPairedBitmap=MobilePhoneBitmap;
					break;
				case HANDHELD:
					CurPairedBitmap=HandheldBitmap;
					break;
				case PC:
					CurPairedBitmap=PCBitmap;
					break;
				case HEADSET:
					CurPairedBitmap=HeadsetBitmap;
					break;
				default:
					CurPairedBitmap=OtherBitmap;
					break;
				}
				
				if(strlen(pCurPairedPointer->PairedDevAliasName)!=0)
					curIndex=SendMessage(hPairedlist, LB_INSERTSTRING, 1, 
					        (LPARAM)pCurPairedPointer->PairedDevAliasName);
				else
					curIndex=SendMessage(hPairedlist, LB_INSERTSTRING, 1, 
					        (LPARAM)pCurPairedPointer->PairedDevName);
				
				if(pCurPairedPointer->PairedDevAuthor)
				{
                    SetAuthoriseDevice(pCurPairedPointer->PairedDevHandle);

					SendMessage(hPairedlist, LB_SETIMAGE, 
					   (WPARAM)MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(curIndex,1)),(LPARAM)AuthorBitmap);
				}
				
				SendMessage(hPairedlist, LB_SETIMAGE, 
					(WPARAM)MAKEWPARAM(IMAGE_BITMAP,MAKEWORD(curIndex,0)),(LPARAM)CurPairedBitmap);

                SendMessage(hPairedlist,LB_SETITEMDATA,
					      (WPARAM)curIndex,(LPARAM)pCurPairedPointer->PairedDevHandle);
				
				pCurPairedPointer=pCurPairedPointer->Next;
			}
			
			
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
			
			if(0==curSel)
			{
				SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
				SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			}
			else
			{
				CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
					                             (WPARAM)curSel,0);
				
				GetDeviceClass(CurDevHdl, &dev_cls); 

				if(DistinguishDevCls(dev_cls)==HEADSET)  
				{				
					pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);
					
					if(pListPointer->PairedDevConnhandle==0)
						SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_CONNECT);
					else
						SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_DISCONNECT);
				}
				else
                     SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");

				SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
			}
			
			SetWindowText(hBluetoothFrame,IDP_BT_TITLE_PAIREDDEVICES);
			
			PDASetMenu(hBluetoothFrame,hPairedMainMenu);
			
			SetFocus(hPairedlist);

			SendMessage(hPairedlist, LB_SETCURSEL, curSel, 0);  //设回原来窗口Listbox的焦点

		}
        break;

	case WM_COMMAND:
		Paired_OnCommand(hWnd, wParam, lParam);		
		break;
		
	case WM_KEYDOWN:
		Paired_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);
		break;

	case CONFIRM_SWITCHONBTSEARCH:
		if (lParam)
		{			
// 			SwitchOnBlueTooth();
			
			BT_SendRequest(hWnd,0,RT_BT_SWITCHON,0,NULL,0);
			
			bBluetoothStatus=TRUE;	
			
			DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //通知程序管理器蓝牙打开

			SendMessage(hBtSettingsWnd, WM_SETBTSTATE, bBluetoothStatus, 0);

			PLXTipsWin(hBluetoothFrame, hWnd, TIPS_SEARCH,IDP_BT_STRING_BTSWITCHONNOTIF, IDP_BT_TITLE_BLUETOOTH,
				Notify_Success, IDP_BT_BUTTON_OK, NULL,20);	
		}
        break;

	case TIPS_SEARCH:
		 SearchDevicesWindow();	 
			
		 break;

	case CONFIRM_SWITCHONBTCONNECT:
		if (lParam)
		{
// 		   SwitchOnBlueTooth();	
			
           BT_SendRequest(hWnd,0,RT_BT_SWITCHON,0,NULL,0);
		   
		   bBluetoothStatus=TRUE;
		   
		   DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //通知程序管理器蓝牙打开

		   SendMessage(hBtSettingsWnd, WM_SETBTSTATE, bBluetoothStatus, 0);

           PLXTipsWin(hBluetoothFrame, hWnd, TIPS_CONNECT,IDP_BT_STRING_BTSWITCHONNOTIF, IDP_BT_TITLE_BLUETOOTH,
			   Notify_Success, IDP_BT_BUTTON_OK, NULL,20);	
		}
		break;
    
	case TIPS_CONNECT:
		{
		  curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
		  Paired_Connecting(hWnd,curSel);
		}	        
		break;

	case CONFIRM_DISCONNECT:
		if (lParam)
		{	
			BTDEVHDL CurDevHdl;
            PPAIREDDEVNODE pListPointer;

			memset(curSelPairedName,0,32);
			curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
			SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);
			
			DisConnection(CurConn_hdl);   //断开当前连接

			CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
				(WPARAM)curSel,0);
			pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);

            pListPointer->PairedDevConnhandle=0;
						
			strcpy(StrDisconnected,curSelPairedName);
			strcat(StrDisconnected,IDP_BT_STRING_DISCONNECTEDNOTIF);
			
			PLXTipsWin(hBluetoothFrame, hWnd, 0,StrDisconnected, IDP_BT_TITLE_PAIREDDEVICES,
				Notify_Success, IDP_BT_BUTTON_OK, NULL,20);
		}
		break;

	case CONFIRM_AUTHORISE:
        if (lParam)
		{	
			BTDEVHDL CurDevHdl;
            PPAIREDDEVNODE pListPointer;
			
			memset(curSelPairedName,0,32);
			curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
			SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);

            CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
				(WPARAM)curSel,0);
			
			pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);
            
 			SetAuthoriseDevice(CurDevHdl);   //设置为授权设备

			pListPointer->PairedDevAuthor=TRUE;

            SetPairedDevInfoToFile(pPairedDevHead);   //存储已配对的设备到文件中
			
			strcpy(StrAuthorised,curSelPairedName);
			strcat(StrAuthorised,IDP_BT_STRING_AUTHORISENOTIF);
			
			PLXTipsWin(hBluetoothFrame, hWnd, 0,StrAuthorised, IDP_BT_TITLE_PAIREDDEVICES,
				Notify_Success, IDP_BT_BUTTON_OK, NULL,20);
		}
		break;
    case CONFIRM_DELPAIR:
		if (lParam)
		{
			BTDEVHDL CurDevHdl;
            PPAIREDDEVNODE pListPointer;
			
			memset(curSelPairedName,0,32);
			curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
			SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);
			
			CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
				(WPARAM)curSel,0);
			pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);

     		DelRemoteDevice(CurDevHdl);

			pPairedDevHead=DeletePairedDevNode(pPairedDevHead,CurDevHdl);

            SetPairedDevInfoToFile(pPairedDevHead);   //存储已配对的设备到文件中

			SendMessage(hPairedlist, LB_SETCURSEL, 0, 0);

			PLXTipsWin(hBluetoothFrame, hWnd, 0,IDP_BT_STRING_DELETEDNOTIF, IDP_BT_TITLE_PAIREDDEVICES,
				Notify_Success, IDP_BT_BUTTON_OK, NULL,20);			
		}
		break;
		
    case CONFIRM_DELALLPAIR:
        if (lParam)
		{
			PPAIREDDEVNODE pCurPairedPointer;
			
			pCurPairedPointer=pPairedDevHead;
            
			WaitWin(hWnd, TRUE, IDP_BT_STRING_DELETING,
			   IDP_BT_TITLE_SELECTPAIREDDEVICES, NULL, NULL,0);	

            while(pCurPairedPointer!=NULL)
			{
				DelRemoteDevice(pCurPairedPointer->PairedDevHandle);
                pPairedDevHead=DeletePairedDevNode(pPairedDevHead,pCurPairedPointer->PairedDevHandle);
                
				pCurPairedPointer=pPairedDevHead;
// 				pCurPairedPointer=pCurPairedPointer->Next;
			}

			WaitWin(hWnd, FALSE,IDP_BT_STRING_DELETING,
			    IDP_BT_TITLE_SELECTPAIREDDEVICES, NULL, NULL,0);

			SetPairedDevInfoToFile(pPairedDevHead);   //存储已配对的设备到文件中

            SendMessage(hPairedlist, LB_SETCURSEL, 0, 0);
			
			PLXTipsWin(hBluetoothFrame, hWnd, 0,IDP_BT_STRING_DELETEDNOTIF,
				IDP_BT_TITLE_PAIREDDEVICES,Notify_Success, IDP_BT_BUTTON_OK, NULL,20);	
		}
		break;

    case CANCELCONNECTING:
		{
		   BTDEVHDL CurDevHdl;
           PPAIREDDEVNODE pListPointer;

		   curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
		   
		   CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
			   (WPARAM)curSel,0);
		   pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);
		   
           DisConnection(CurConn_hdl);

           pListPointer->PairedDevConnhandle=0;
		   
		}
		break;

	case BT_MSG_CONNECT_HEADSET:
		{			  
			char StrConnecting[64];

			BTDEVHDL CurDevHdl;
            PPAIREDDEVNODE pListPointer;
			
			curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
			SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);
            
			strcpy(StrConnecting,curSelPairedName);
			strcat(StrConnecting,IDP_BT_STRING_CONNECTING);

			switch(wParam) 
			{
			case BTSDK_OK:
				{
					WaitWin(hWnd, FALSE, StrConnecting,
						IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_CANCEL,CANCELCONNECTING);

					CurConn_hdl=(BTCONNHDL)lParam;

					CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
						(WPARAM)curSel,0);
					
					pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);

					pListPointer->PairedDevConnhandle=CurConn_hdl;

					PLXTipsWin(hBluetoothFrame, hWnd, 0,IDP_BT_STRING_CONNECTED, IDP_BT_TITLE_PAIREDDEVICES,
						Notify_Success, IDP_BT_BUTTON_OK, NULL,20);

				}
				break;
			default:
				{
					WaitWin(hWnd, FALSE, StrConnecting,
						IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_CANCEL,CANCELCONNECTING);

                    PLXTipsWin(hBluetoothFrame, hWnd, 0,IDP_BT_STRING_CONNECTIONFAILED, IDP_BT_TITLE_PAIREDDEVICES,
						Notify_Failure, IDP_BT_BUTTON_OK, NULL,20);
				}
				break;
			}
		}
		break;

	case WM_CLOSE:		
		DestroyWindow(hWnd);
        hPairedWnd = NULL;
		break;
		
    case WM_DESTROY :
		DestroyMenu(hPairedDelManyMenu);
        DestroyMenu(hPairedMainMenu);
        UnregisterClass("PairedWndClass", NULL);

		if(NewDevBitmap)                    //释放图标占用的内存
			DeleteObject(NewDevBitmap);
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
		if(AuthorBitmap)
			DeleteObject(AuthorBitmap);
        break;
		
	default:    
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}

void Paired_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{
	LONG curSel;
	BTDEVHDL CurDevHdl;
	PPAIREDDEVNODE pListPointer;
	BTUINT32 dev_class;

	HWND hBluetoothFrame;
	hBluetoothFrame=GetBluetoothFrameWnd();

	switch (nKey)
	{
	case VK_F5:	
		{
			curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);		
			if (0 == curSel)
			{
				if(bBluetoothStatus)
					SearchDevicesWindow();
				else
					PLXConfirmWinEx(hBluetoothFrame,hWnd,IDP_BT_STRING_BTSWITCHONPROMPT, Notify_Request, 
					      IDP_BT_TITLE_BLUETOOTH, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_SWITCHONBTSEARCH);
			}
			else
			{
				PairedChangeMenu();     //根据不同情况，改变菜单的值
				
				SendMessage(GetParent(hWnd), WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
			}
			
		}
		break;

	case VK_F10:
		{
			SetPairedDevInfoToFile(pPairedDevHead);   //存储已配对的设备到文件中

			CallBTMainWnd(hBluetoothFrame);
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		}
		break;

	case VK_RETURN:
		{
			curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);	

            CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
				(WPARAM)curSel,0);

			GetDeviceClass(CurDevHdl, &dev_class);

			if(DistinguishDevCls(dev_class)==HEADSET)  
			{				
				pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);
				
				if(pListPointer->PairedDevConnhandle==0)
					Paired_Connecting(hWnd,curSel);
				else
					Paired_Disconnect(hWnd,curSel);
			}
		}
		break;

	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
		break;
	}
	return;
}

void Paired_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LONG curSel;

	BTDEVHDL CurDevHdl;
	BTUINT32 dev_class;
	PPAIREDDEVNODE pListPointer;

	HWND hBluetoothFrame;	
    hBluetoothFrame=GetBluetoothFrameWnd();
    
	switch(LOWORD(wParam))
	{
	case ID_PAIREDBLUETOOTHLIST:
		switch(HIWORD(wParam))
		{
		case LBN_SELCHANGE:
			curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
			if(curSel==0)
			{
				SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");	
				SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			}
			else
			{
				CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
					(WPARAM)curSel,0);
				
                GetDeviceClass(CurDevHdl,&dev_class);
				
				if(DistinguishDevCls(dev_class)==HEADSET)  
				{		
					pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);
					
					if(pListPointer->PairedDevConnhandle==0)
						SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_CONNECT);
					else
						SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_DISCONNECT);
                }
				else
                    SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)"");
				
                SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Options");
            }
			break;
		default:
			break;
		}
		break;
	case ID_CONNECT:
		curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
		Paired_Connecting(hWnd,curSel);
		break;
	case ID_DISCONNECT:
		curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
		Paired_Disconnect(hWnd,curSel);
		break;
	case ID_RENAME:
		curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
		PairedRenameWindow(curSel);

        SetPairedDevInfoToFile(pPairedDevHead);   //存储已配对的设备到文件中
		break;
	case ID_AUTHORISE:
		curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
        Paired_Authorise(hWnd,curSel);
		break;
	case ID_UNAUTHORISE:
		curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
		Paired_Unauthorise(hWnd,curSel);
		break;
	case ID_DELETE:		
		curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);
		Paired_Delete(hWnd,curSel);
		break;
	case ID_DELETESELECT:
		DelMulSelPairedWindow();
		SendMessage(hPairedlist, LB_SETCURSEL, 0, 0);
		break;
	case ID_DELETEALL:
        DelAllPaired(hWnd);
		break;
	default:		
	    break;
	}
}

static void CreatePairedOptionsMenu()
{	
	hPairedMainMenu = CreateMenu();   //创建主菜单句柄

	hPairedDelManyMenu = CreateMenu(); //创建子菜单句柄
	
	AppendMenu(hPairedDelManyMenu, MF_STRING, (UINT_PTR)ID_DELETESELECT, IDP_BT_STRING_SELECT);
	AppendMenu(hPairedDelManyMenu, MF_STRING, (UINT_PTR)ID_DELETEALL, IDP_BT_STRING_ALL);
} 

static void PairedChangeMenu()
{
	LONG curSel;
	
	HWND hBluetoothFrame;
	int iPairedDevMenuCount;

	BTDEVHDL CurDevHdl;
	BTUINT32 cls;
	PPAIREDDEVNODE pListPointer;
	
    hBluetoothFrame=GetBluetoothFrameWnd();
	curSel = SendMessage(hPairedlist, LB_GETCURSEL, 0, 0);

	CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
		(WPARAM)curSel,0);
    
	GetDeviceClass(CurDevHdl,&cls);  //得到当前的设备类

	if(DistinguishDevCls(cls)==HEADSET)  
    {
		iPairedDevMenuCount = GetMenuItemCount(hPairedMainMenu);
		while(iPairedDevMenuCount-- > 0)
		{
			RemoveMenu(hPairedMainMenu, iPairedDevMenuCount, MF_BYPOSITION);
		}
		
		pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);
        
        if(pListPointer->PairedDevConnhandle!=0)			
		    AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_DISCONNECT, IDP_BT_BUTTON_DISCONNECT);
		else
            AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_CONNECT, IDP_BT_BUTTON_CONNECT);

		AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_RENAME, IDP_BT_STRING_RENAME);

		if(pListPointer->PairedDevAuthor)
		    AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_UNAUTHORISE, IDP_BT_STRING_UNAUTHORISE);
		else
            AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_AUTHORISE, IDP_BT_STRING_AUTHORISE);
		
		AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_DELETE, IDP_BT_BUTTON_DELETE);

		AppendMenu(hPairedMainMenu, MF_STRING | MF_POPUP, (UINT_PTR)hPairedDelManyMenu, IDP_BT_STRING_DELETEMANY);		
	}
	else
	{
		iPairedDevMenuCount = GetMenuItemCount(hPairedMainMenu);
		while(iPairedDevMenuCount-- > 0)
		{
			RemoveMenu(hPairedMainMenu, iPairedDevMenuCount, MF_BYPOSITION);
		}

		AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_RENAME, IDP_BT_STRING_RENAME);
		
        pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);
		
		if(pListPointer->PairedDevAuthor)
			AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_UNAUTHORISE, IDP_BT_STRING_UNAUTHORISE);
		else
            AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_AUTHORISE, IDP_BT_STRING_AUTHORISE);
		
		AppendMenu(hPairedMainMenu, MF_STRING, (UINT_PTR)ID_DELETE, IDP_BT_BUTTON_DELETE);
		
		AppendMenu(hPairedMainMenu, MF_STRING | MF_POPUP, (UINT_PTR)hPairedDelManyMenu, IDP_BT_STRING_DELETEMANY);
	}
	
}

void Paired_Connecting(HWND hWnd,LONG curSel)  //配对过的设备开始连接
{	
	  char curSelPairedName[32];
	  HWND hBluetoothFrame;
	  char StrConnecting[64];
	  BTDEVHDL CurDevHdl;
	  PPAIREDDEVNODE pListPointer;

	  hBluetoothFrame=GetBluetoothFrameWnd();
	  memset(curSelPairedName,0,32);	  
	  
	  if(!bBluetoothStatus)
			PLXConfirmWinEx(hBluetoothFrame,hWnd,IDP_BT_STRING_BTSWITCHONPROMPT, Notify_Request, 
				IDP_BT_TITLE_BLUETOOTH, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_SWITCHONBTCONNECT);	
	  else
	  {
		  SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);
		  
		  strcpy(StrConnecting,curSelPairedName);
		  strcat(StrConnecting,IDP_BT_STRING_CONNECTING);

		  CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
			  (WPARAM)curSel,0);
		  pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);

		  WaitWin(hWnd, TRUE, StrConnecting,
			  IDP_BT_TITLE_NEWDEVICE, NULL, IDP_BT_BUTTON_CANCEL,CANCELCONNECTING);

		  BT_SendRequest(hWnd,BT_MSG_CONNECT_HEADSET,RT_BT_CONNECT_HEADSET,&pListPointer->PairedDevHandle,NULL,0);
	  }
		  
}
		
void Paired_Disconnect(HWND hWnd,LONG curSel)
{
	  HWND hBluetoothFrame;
	  char curSelPairedName[32];
	  char StrDisconnect[64];

	  hBluetoothFrame=GetBluetoothFrameWnd();
	  memset(curSelPairedName,0,32);

	  SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);
	  
	  strcpy(StrDisconnect,curSelPairedName);
	  strcat(StrDisconnect,IDP_BT_STRING_DISCONNECTPROMPT);

	  PLXConfirmWinEx(hBluetoothFrame,hWnd,StrDisconnect, Notify_Request, 
		  IDP_BT_TITLE_PAIREDDEVICES, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_DISCONNECT);	
    
}

void Paired_Authorise(HWND hWnd,LONG curSel)
{
	char curSelPairedName[32];
    HWND hBluetoothFrame;
	char StrAuthorise[64];

	hBluetoothFrame=GetBluetoothFrameWnd();

    SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);
	
	strcpy(StrAuthorise,curSelPairedName);
	strcat(StrAuthorise,IDP_BT_STRING_AUTHORISEPROMPT);

	PLXConfirmWinEx(hBluetoothFrame,hWnd,StrAuthorise, Notify_Request, 
		IDP_BT_TITLE_PAIREDDEVICES, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_AUTHORISE);
}

void Paired_Unauthorise(HWND hWnd,LONG curSel)
{  
	char curSelPairedName[32];
	HWND hBluetoothFrame;
	char StrUnauthorised[64];
    BTDEVHDL CurDevHdl;
	PPAIREDDEVNODE pListPointer;

	hBluetoothFrame=GetBluetoothFrameWnd();
	
	SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);

	CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
		(WPARAM)curSel,0);
    pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);

	SetUnauthoriseDevice(CurDevHdl);
	
	pListPointer->PairedDevAuthor=FALSE;

	SetPairedDevInfoToFile(pPairedDevHead);   //存储已配对的设备到文件中
	
	strcpy(StrUnauthorised,curSelPairedName);
	strcat(StrUnauthorised,IDP_BT_STRING_UNAUTHORISENOTIF);
	
	PLXTipsWin(hBluetoothFrame, hWnd, 0,StrUnauthorised, IDP_BT_TITLE_PAIREDDEVICES,
		Notify_Success, IDP_BT_BUTTON_OK, NULL,20);
}

void Paired_Delete(HWND hWnd,LONG curSel)
{
	char curSelPairedName[32];
	char StrDelConn[128];
	char StrDelPair[128];
	HWND hBluetoothFrame;
	BTDEVHDL CurDevHdl;
	PPAIREDDEVNODE pListPointer;

	memset(StrDelConn,0,128);
	memset(StrDelPair,0,128);
    
	memset(curSelPairedName,0,32);
	
	hBluetoothFrame=GetBluetoothFrameWnd();

	SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);
    CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
		(WPARAM)curSel,0);
    pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);
	
	if(pListPointer->PairedDevConnhandle!=0)
	{
		strcpy(StrDelConn,curSelPairedName);
		strcat(StrDelConn,IDP_BT_STRING_SINGLEDEVDISCONNDELPrompt);
		
		PLXConfirmWinEx(hBluetoothFrame,hWnd,StrDelConn, Notify_Request, 
			IDP_BT_TITLE_PAIREDDEVICES, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_DELPAIR);
	}
	else
	{
		strcpy(StrDelPair,curSelPairedName);
		strcat(StrDelPair,IDP_BT_STRING_DELPAIRINGPROMPT);
		
		PLXConfirmWinEx(hBluetoothFrame,hWnd,StrDelPair, Notify_Request, 
			IDP_BT_TITLE_PAIREDDEVICES, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_DELPAIR);			
	}
		
}

void DelAllPaired(HWND hWnd)
{
	HWND hBluetoothFrame;

	hBluetoothFrame=GetBluetoothFrameWnd();
	
    if(IsSomeDevConnected())
	{
		PLXConfirmWinEx(hBluetoothFrame,hWnd,
			 IDP_BT_STRING_SOMEDEVDISCONNDELPROMPT,Notify_Request, IDP_BT_TITLE_PAIREDDEVICES, 
			 IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_DELALLPAIR);
			
	}
	else
	{
		PLXConfirmWinEx(hBluetoothFrame,hWnd,
			IDP_BT_STRING_DELALLPAIRINGSPROMPT,Notify_Request, IDP_BT_TITLE_PAIREDDEVICES, 
			IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_DELALLPAIR);
	}
	
}

static BOOL IsSomeDevConnected()
{
	PPAIREDDEVNODE pCurPairedPointer;
	
	pCurPairedPointer=pPairedDevHead;
	
	while(pCurPairedPointer!=NULL)
	{
       if(pCurPairedPointer->PairedDevConnhandle!=0)
		   return TRUE;
       else
		   pCurPairedPointer=pCurPairedPointer->Next;
	}
    
       return FALSE;
}

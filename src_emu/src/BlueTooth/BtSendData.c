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

#include "BtThread.h"
#include "BtLowCall.h"
#include "BtDeviceNode.h"
#include "BtSendData.h"
#include "BtString.h"

#define ID_SENDDATALIST              (WM_USER+0x200)

#define CONFIRM_SWITCHONBLUETOOTH    (WM_USER+0x300)
#define STOPDISCOVERDEV              (WM_USER+0x301)
#define CANCELCONNECTING             (WM_USER+0x302)
#define CANCELSENDFTPFILE            (WM_USER+0x303)
#define CANCELSENDOPPFILE            (WM_USER+0x304)

#define TIPS_SEND                    (WM_USER+0x305)
#define TIPS_OK                      (WM_USER+0x306)

#define BT_MSG_CONNECT_FTP           (WM_USER+0x307)
#define BT_MSG_CONNECT_OPP           (WM_USER+0x308)
#define BT_MSG_SENDFILE              (WM_USER+0x309)

#define MAX_APPPATHNAME_LEN          256
#define MAX_APPVIEWNAME_LEN          64

typedef struct tagBTCREATEDATA
{
    HWND  hCurAppFrameWnd;
	HWND  hBtSendDatalist;
    char* szAppPathName;
	char* szViewFileName;
	int   iFiletype;
}BTCREATEDATA,*PBTCREATEDATA;

extern DeviceNodeLink FoundDevListHead;
extern BOOL  bBluetoothStatus; 
extern HWND  hBtSettingsWnd;

extern BOOL bStopSearchDev;
//extern pthread_mutex_t BtStopSearch_mutex;

static LRESULT BtSendDataWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void BtSendData_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam);

BOOL BtSendData(HWND hAppFrameWnd,char* szAppPathName,char* szViewFileName,int iFileType)
{
	BTCREATEDATA BtCreateData;

    WNDCLASS    wc;
	RECT rClient;

	HWND hBtSendDataWnd=NULL;   

	memset(&BtCreateData,0,sizeof(BTCREATEDATA));

    wc.style         = 0;
    wc.lpfnWndProc   = BtSendDataWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = sizeof(BTCREATEDATA);
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "BtSendDataWndClass";

	if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    
    BtCreateData.hCurAppFrameWnd=hAppFrameWnd;
    BtCreateData.iFiletype=iFileType;

	if(iFileType == BTMULTIPICTURE)
       BtCreateData.szAppPathName=szAppPathName;
	else
	{
		BtCreateData.szAppPathName=(char *)malloc(MAX_APPPATHNAME_LEN);	
		strcpy(BtCreateData.szAppPathName,szAppPathName);
		
		if(szViewFileName!=NULL)
		{
			BtCreateData.szViewFileName=(char *)malloc(MAX_APPVIEWNAME_LEN);
			strcpy(BtCreateData.szViewFileName,szViewFileName);
		}
    }
    
    GetClientRect(hAppFrameWnd,&rClient);

    hBtSendDataWnd = CreateWindow(
		"BtSendDataWndClass",
		"", 
        WS_VISIBLE | WS_CHILD,
        rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hAppFrameWnd, 
		NULL, 
		NULL, 
		(LPVOID)&BtCreateData);

    // SetFocus(hBtSendDataWnd);

	//去掉上一个窗口的左右小箭头
    SendMessage(hAppFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON),(LPARAM)NULL);
	SendMessage(hAppFrameWnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON),(LPARAM)NULL);	
	
	ShowWindow(hBtSendDataWnd,SW_SHOW);
	UpdateWindow(hBtSendDataWnd);

    return (TRUE);
}

static LRESULT BtSendDataWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	LONG curSel;
	BTCREATEDATA *pBtCreateData;

	static HBITMAP SearchDevBitmap,MobilePhoneBitmap,HandheldBitmap;
	static HBITMAP PCBitmap,HeadsetBitmap,OtherBitmap;
    
	static BTCONNHDL CurConn_hdl;    //本次建立起来连接的handle;

    static int iIDSendDataSearchDev,iIDBtFileTransfer,iIDBtOppTransfer;

	static char TempBuff[256];   //存放提示找到的设备

    LRESULT lResult=TRUE;
	
	pBtCreateData = (BTCREATEDATA*)GetUserData(hWnd);

    switch (message)
    {
    case WM_CREATE :
		{				
			memcpy(pBtCreateData,(PBTCREATEDATA)(((LPCREATESTRUCT)lParam)->lpCreateParams),
				sizeof(BTCREATEDATA));

			if(bBluetoothStatus)
			{
                int curIndex;
				RECT rc;
				DeviceNodeLink Pointer;

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
				
				GetClientRect(hWnd, &rc);
				
				pBtCreateData->hBtSendDatalist=CreateWindow(
					"LISTBOX",
					"",
					WS_CHILD | WS_VISIBLE | WS_VSCROLL| LBS_BITMAP,
					rc.left, rc.top,
					rc.right-rc.left, rc.bottom-rc.top,
					hWnd,
					(HMENU)ID_SENDDATALIST,
					NULL,
					NULL);
				
				if(NULL == pBtCreateData->hBtSendDatalist)
					return FALSE;
				

				curIndex=SendMessage(pBtCreateData->hBtSendDatalist, LB_ADDSTRING, 0, 
					(LPARAM)IDP_BT_STRING_SEARCHFORDEVICES);
				
				SendMessage(pBtCreateData->hBtSendDatalist, LB_SETIMAGE, 
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
						
					curIndex=SendMessage(pBtCreateData->hBtSendDatalist, LB_INSERTSTRING, 1, 
						(LPARAM)Pointer->DevName);

					SendMessage(pBtCreateData->hBtSendDatalist, LB_SETIMAGE, 
							(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)CurFoundBitmap);

					SendMessage(pBtCreateData->hBtSendDatalist,LB_SETITEMDATA,
						(WPARAM)curIndex,(LPARAM)Pointer->DevHandle);
						
					Pointer=Pointer->Next;
				}
				
				SendMessage(pBtCreateData->hBtSendDatalist, LB_SETCURSEL, 0, 0);
				
				SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
				SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
				SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
				
				SetWindowText(pBtCreateData->hCurAppFrameWnd,IDP_BT_TITLE_BLUETOOTH);
				
				SetFocus(pBtCreateData->hBtSendDatalist);
				
			}
			else
			{	
				PLXConfirmWinEx(pBtCreateData->hCurAppFrameWnd,hWnd,IDP_BT_STRING_BTSWITCHONPROMPT,
					Notify_Request,IDP_BT_TITLE_BLUETOOTH, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_SWITCHONBLUETOOTH);
			}
		}
        break;

    case WM_SETFOCUS:			
		    SetFocus(pBtCreateData->hBtSendDatalist);
		break;

	case PWM_SHOWWINDOW:
		{
			int i, nCount;
			DeviceNodeLink Pointer;
			int curIndex;					
			HBITMAP CurFoundBitmap;
			
            nCount=SendMessage(pBtCreateData->hBtSendDatalist,LB_GETCOUNT,0,0);
			
			if(nCount>1)
			{
				for (i = nCount - 1; i > 0 ; i--)
				{
					SendMessage(pBtCreateData->hBtSendDatalist,LB_DELETESTRING,(WPARAM)i,0);
				}
			}
			
			Pointer=FoundDevListHead;
			
			SendMessage(pBtCreateData->hBtSendDatalist, LB_SETCURSEL, 0, 0);
			
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
				
				curIndex=SendMessage(pBtCreateData->hBtSendDatalist, LB_INSERTSTRING, 1, 
					           (LPARAM)Pointer->DevName);
				
				SendMessage(pBtCreateData->hBtSendDatalist, LB_SETIMAGE, 
					(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)CurFoundBitmap);
				
				SendMessage(pBtCreateData->hBtSendDatalist,LB_SETITEMDATA,
					(WPARAM)curIndex,(LPARAM)Pointer->DevHandle);
				
				Pointer=Pointer->Next;
			}

			curSel = SendMessage(pBtCreateData->hBtSendDatalist, LB_GETCURSEL, 0, 0);
            
			SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
			if(0==curSel)
			{		    
		      SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
		      SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			}
			else
			{
				SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_CONNECT);
				SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"");
			}
		
	        SetWindowText(pBtCreateData->hCurAppFrameWnd,IDP_BT_TITLE_BLUETOOTH);

		    SetFocus(pBtCreateData->hBtSendDatalist);
		}
        break;

 	case WM_COMMAND:
 	    BtSendData_OnCommand(hWnd, wParam, lParam);         
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
			
			BT_SendRequest(hWnd,0,RT_BT_UPDATEREMOTEDEVINFO,NULL,NULL,0);
		}
		break;

	case CONFIRM_SWITCHONBLUETOOTH:
        if (lParam)
		{			
// 			SwitchOnBlueTooth();
			BT_SendRequest(hWnd,0,RT_BT_SWITCHON,0,NULL,0);

            bBluetoothStatus=TRUE;   //蓝牙已打开
            SendMessage(hBtSettingsWnd, WM_SETBTSTATE, bBluetoothStatus, 0);
			
			DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //通知程序管理器蓝牙打开

			PLXTipsWin(pBtCreateData->hCurAppFrameWnd, hWnd, TIPS_SEND,
				IDP_BT_STRING_BTSWITCHONNOTIF, IDP_BT_TITLE_BLUETOOTH,
				Notify_Success, IDP_BT_BUTTON_OK, NULL,20);
		}
		else
		{
            PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		}
		break;

	case WM_UPDATEREMOTEDEVCOMPLETED:
		{
			if(lParam>0) 
				{					
					WaitWin(hWnd, FALSE, IDP_BT_STRING_SEARCHFORDEVICESPROMPT,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_STOP,STOPDISCOVERDEV);

				}		
			else
				{	
					WaitWin(hWnd, FALSE, IDP_BT_STRING_SEARCHFORDEVICESPROMPT,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_STOP,STOPDISCOVERDEV);
					
					PLXTipsWin(pBtCreateData->hCurAppFrameWnd, hWnd, 0,	
						IDP_BT_STRING_NODEVICESFOUND, IDP_BT_TITLE_BLUETOOTH,Notify_Failure, IDP_BT_BUTTON_OK, NULL,20);
				}
			}
		break;

    case WM_FOUNDDEVICE:
		{
			printf("\n#######Current DevName=%s\n",(char *)lParam);
			
			strcat(TempBuff,(char *)lParam);
			strcat(TempBuff,"\n");
			
			UpdateWaitwinContext(TempBuff);
			InvalidateRect(NULL, NULL, TRUE);
			UpdateScreen();	
		
		}
		break;

	case BT_MSG_CONNECT_FTP:
		{
			char curSelDevName[32];
 			char szConnectFailed[64];    
			char szConnecting[64];
			char szSending[64];
			BTCONNFILENAME BtConnFileName; 
			BTCONNLIST     BtConnList;
		
			curSel = SendMessage(pBtCreateData->hBtSendDatalist, LB_GETCURSEL, 0, 0);
			SendMessage(pBtCreateData->hBtSendDatalist,LB_GETTEXT,
				     (WPARAM)curSel,(LPARAM)(LPCTSTR)curSelDevName);
            
			strcpy(szConnecting,curSelDevName);
			strcat(szConnecting,IDP_BT_STRING_CONNECTING);
			
			switch(wParam) 
			{
			case BTSDK_OK:
				{
					WaitWin(hWnd, FALSE, szConnecting,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_CANCEL,CANCELCONNECTING);
										
					CurConn_hdl=(BTCONNHDL)lParam;
					
					if(pBtCreateData->iFiletype == BTMULTIPICTURE)
					{
                        memset(&BtConnList,0,sizeof(BTCONNLIST));

                        BtConnList.conn_hdl=(BTCONNHDL)lParam;
                        
                        BtConnList.iFiletype=pBtCreateData->iFiletype;
						BtConnList.pListHead=pBtCreateData->szAppPathName;

                        iIDBtFileTransfer=BT_SendRequest(hWnd,BT_MSG_SENDFILE,RT_BT_MULPICFILETRANSFER,&BtConnList,NULL,0);
					}
					else
					{
                        memset(&BtConnFileName,0,sizeof(BTCONNFILENAME));
						
						BtConnFileName.conn_hdl=(BTCONNHDL)lParam;

						strcpy(BtConnFileName.szFileName,pBtCreateData->szAppPathName);
						BtConnFileName.iFiletype=pBtCreateData->iFiletype;
						
						if(pBtCreateData->szViewFileName!=NULL)
							strcpy(BtConnFileName.szViewFileName,pBtCreateData->szViewFileName);
						
						iIDBtFileTransfer=BT_SendRequest(hWnd,BT_MSG_SENDFILE,RT_BT_FILETRANSFER,&BtConnFileName,NULL,0);
					//发出文件传输请求(FTP)
                    }
					
					strcpy(szSending,curSelDevName);
					strcat(szSending,IDP_BT_STRING_SENDING);
					
					WaitWin(hWnd, TRUE, szSending,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_CANCEL,CANCELSENDFTPFILE);
					
				}
				break;
			default:
				{
					WaitWin(hWnd, FALSE, szConnecting,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_CANCEL,CANCELCONNECTING);

					strcpy(szConnectFailed,curSelDevName);
					strcat(szConnectFailed,IDP_BT_STRING_CONNECTIONFAILED);
					
                    PLXTipsWin(pBtCreateData->hCurAppFrameWnd, hWnd, 0,szConnectFailed, IDP_BT_TITLE_BLUETOOTH,
						Notify_Failure, IDP_BT_BUTTON_OK, NULL,20);
				}
				break;
			}
		}
		break;

	case BT_MSG_CONNECT_OPP:
		{
            char curSelDevName[32];
 			char szConnectFailed[64];    
			char szConnecting[64];
			char szSending[64];
			BTCONNFILENAME BtConnFileName; 

			memset(&BtConnFileName,0,sizeof(BTCONNFILENAME));
		
			curSel = SendMessage(pBtCreateData->hBtSendDatalist, LB_GETCURSEL, 0, 0);
			SendMessage(pBtCreateData->hBtSendDatalist,LB_GETTEXT,
				     (WPARAM)curSel,(LPARAM)(LPCTSTR)curSelDevName);
            
			strcpy(szConnecting,curSelDevName);
			strcat(szConnecting,IDP_BT_STRING_CONNECTING);
			
			switch(wParam) 
			{
			case BTSDK_OK:
				{
					WaitWin(hWnd, FALSE, szConnecting,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_CANCEL,CANCELCONNECTING);
					
					/*
					strcpy(szConnected,curSelDevName);
					strcat(szConnected,":\r\nConnected");*/
										
					BtConnFileName.conn_hdl=(BTCONNHDL)lParam;
					CurConn_hdl=(BTCONNHDL)lParam;
                    strcpy(BtConnFileName.szFileName,pBtCreateData->szAppPathName);
                    BtConnFileName.iFiletype=pBtCreateData->iFiletype;

					if(pBtCreateData->szViewFileName!=NULL)
					   strcpy(BtConnFileName.szViewFileName,pBtCreateData->szViewFileName);
					
                    iIDBtOppTransfer=BT_SendRequest(hWnd,BT_MSG_SENDFILE,RT_BT_OBJECTTRANSFER,&BtConnFileName,NULL,0);
					//发出文件传输请求(OPP)

					strcpy(szSending,curSelDevName);
					strcat(szSending,IDP_BT_STRING_SENDING);
					
					WaitWin(hWnd, TRUE, szSending,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_CANCEL,CANCELSENDOPPFILE);
					
				}
				break;
			default:
				{
					WaitWin(hWnd, FALSE, szConnecting,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_CANCEL,CANCELCONNECTING);

					strcpy(szConnectFailed,curSelDevName);
					strcat(szConnectFailed,IDP_BT_STRING_CONNECTIONFAILED);
					
                    PLXTipsWin(pBtCreateData->hCurAppFrameWnd, hWnd, 0,szConnectFailed, 
						IDP_BT_TITLE_BLUETOOTH, Notify_Failure, IDP_BT_BUTTON_OK, NULL,20);
				}
				break;
			}
		}
        break;

	case BT_MSG_SENDFILE:
		{
			char curSelDevName[32];
			char szSending[64];
			char szSendFailed[64];
			char szSent[64];

			curSel = SendMessage(pBtCreateData->hBtSendDatalist, LB_GETCURSEL, 0, 0);
			SendMessage(pBtCreateData->hBtSendDatalist,LB_GETTEXT,
				(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelDevName);
            
			strcpy(szSending,curSelDevName);
			strcat(szSending,IDP_BT_STRING_SENDING);

			switch(wParam) 
			{
			case BTSDK_OK:
				{
					WaitWin(hWnd, FALSE, szSending,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_CANCEL,CANCELSENDFTPFILE);

					strcpy(szSent,curSelDevName);
					strcat(szSent,IDP_BT_STRING_SENT);
					
					PLXTipsWin(pBtCreateData->hCurAppFrameWnd, hWnd, TIPS_OK,
						szSent, IDP_BT_TITLE_BLUETOOTH,   Notify_Success, IDP_BT_BUTTON_OK, NULL,20);

				}
				break;
			default:
				{
					WaitWin(hWnd, FALSE, szSending,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_CANCEL,CANCELSENDFTPFILE);	
					
					strcpy(szSendFailed,curSelDevName);
					strcat(szSendFailed,IDP_BT_STRING_SENDINGFAILED);
					
					PLXTipsWin(pBtCreateData->hCurAppFrameWnd, hWnd, TIPS_OK,
						szSendFailed, IDP_BT_TITLE_BLUETOOTH,   Notify_Failure, IDP_BT_BUTTON_OK, NULL,20);
				}
		        break;
			}	
		}
		break;
				
	case TIPS_SEND:
		{
			int curIndex;
			RECT rc;
			DeviceNodeLink Pointer;
			
			HBITMAP CurFoundBitmap;				
			
			SearchDevBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_search.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			
			MobilePhoneBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory1.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			HandheldBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory2.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			PCBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory3.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			HeadsetBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory4-a.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			OtherBitmap=LoadImage(NULL, "/rom/bluetooth/bluetooth_devicecategory4-b.bmp", 
				IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			
			GetClientRect(hWnd, &rc);
			
			pBtCreateData->hBtSendDatalist=CreateWindow(
				"LISTBOX",
				"",
				WS_CHILD | WS_VISIBLE | WS_VSCROLL| LBS_BITMAP,
				rc.left, rc.top,
				rc.right-rc.left, rc.bottom-rc.top,
				hWnd,
				(HMENU)ID_SENDDATALIST,
				NULL,
				NULL);
			
			if(NULL == pBtCreateData->hBtSendDatalist)
				return FALSE;
						
			curIndex=SendMessage(pBtCreateData->hBtSendDatalist, LB_ADDSTRING, 0, 
				   (LPARAM)IDP_BT_STRING_SEARCHFORDEVICES);
			
			SendMessage(pBtCreateData->hBtSendDatalist, LB_SETIMAGE, 
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
				
				curIndex=SendMessage(pBtCreateData->hBtSendDatalist, LB_INSERTSTRING, 1, 
					(LPARAM)Pointer->DevName);
				
				SendMessage(pBtCreateData->hBtSendDatalist, LB_SETIMAGE, 
					(WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)curIndex),(LPARAM)CurFoundBitmap);
				
				Pointer=Pointer->Next;
			}
			
			SendMessage(pBtCreateData->hBtSendDatalist, LB_SETCURSEL, 0, 0);
			
			SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_BACK);
			SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,1,(LPARAM)"");
			SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			
			SetWindowText(pBtCreateData->hCurAppFrameWnd,IDP_BT_TITLE_BLUETOOTH);
			
			SetFocus(pBtCreateData->hBtSendDatalist);
		}

		break;

	case TIPS_OK:
		{
			BTCREATEDATA *pBtCreateData;
			
			pBtCreateData = (BTCREATEDATA*)GetUserData(hWnd);
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		}
		break;

	case STOPDISCOVERDEV:
	{
		printf("\n$$$$$$$$$$$$$In STOPDISCOVERDEV$$$$$$$$$$$$$$$$$\n");

		    StopDiscoverDevice();
            
// 			pthread_mutex_lock(&BtStopSearch_mutex);
            bStopSearchDev=TRUE;
// 			pthread_mutex_unlock(&BtStopSearch_mutex);
         }
		break;

	case CANCELCONNECTING:
        DisConnection(CurConn_hdl);       
		break;

	case CANCELSENDFTPFILE:
	    BT_CancelRequest(iIDBtFileTransfer);
		break;

	case CANCELSENDOPPFILE:
        BT_CancelRequest(iIDBtOppTransfer);
		break;
		
	case WM_KEYDOWN:	
		{
			LONG curSel;
			
			char szCurSelDevName[64];
			char szComConnecting[128];
			
			BTDEVHDL CurDevHdl;
			
			switch (LOWORD(wParam))
			{
			case VK_F5:	
				
				curSel = SendMessage(pBtCreateData->hBtSendDatalist, LB_GETCURSEL, 0, 0);
				
				if (0 == curSel) 
				{								
					WaitWin(hWnd, TRUE, IDP_BT_STRING_SEARCHFORDEVICESPROMPT,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_STOP,STOPDISCOVERDEV);
					
					UpdateScreen();

					memset(TempBuff,0,256);
					
					strcpy(TempBuff,IDP_BT_STRING_SEARCHDEVICESFOUND);
										
					iIDSendDataSearchDev=BT_SendRequest(hWnd,0,RT_BT_SEARCHDEVICE,0,NULL,0);
					//发出搜索设备请求

//					pthread_mutex_lock(&BtStopSearch_mutex);
					bStopSearchDev=FALSE;
//					pthread_mutex_unlock(&BtStopSearch_mutex);
				}
				
				break;
				
			case VK_RETURN:
				curSel = SendMessage(pBtCreateData->hBtSendDatalist, LB_GETCURSEL, 0, 0);
				
				if(curSel>0)
				{
					SendMessage(pBtCreateData->hBtSendDatalist,LB_GETTEXT,curSel,
						(LPARAM)szCurSelDevName);
					
					CurDevHdl=SendMessage(pBtCreateData->hBtSendDatalist,LB_GETITEMDATA,
						(WPARAM)curSel,0);
					
					strcpy(szComConnecting,szCurSelDevName);
					strcat(szComConnecting,IDP_BT_STRING_CONNECTING);
					
					WaitWin(hWnd, TRUE, szComConnecting,
						IDP_BT_TITLE_BLUETOOTH, NULL, IDP_BT_BUTTON_CANCEL, CANCELCONNECTING);
					
					if(pBtCreateData->iFiletype==BTNOTE || pBtCreateData->iFiletype==BTPICTURE
						    || pBtCreateData->iFiletype==BTMULTIPICTURE)
						BT_SendRequest(hWnd,BT_MSG_CONNECT_FTP,RT_BT_CONNECT_FTP,&CurDevHdl,NULL,0);
					//发出连接远程设备请求(FTP)
					else
						BT_SendRequest(hWnd,BT_MSG_CONNECT_OPP,RT_BT_CONNECT_OPP,&CurDevHdl,NULL,0);
					//发出连接远程设备请求(OPP)
				}
				
				break;
				
			case VK_F10:
				PostMessage(hWnd,WM_CLOSE,0,0);
				SendMessage(pBtCreateData->hCurAppFrameWnd,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
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
			if(pBtCreateData->iFiletype!=BTMULTIPICTURE)
			    free(pBtCreateData->szAppPathName);
			
			if(pBtCreateData->szViewFileName!=NULL)
				free(pBtCreateData->szViewFileName);
			
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
			
			UnregisterClass("BtSendDataWndClass", NULL);
		}
        break;

    default:    
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}

void BtSendData_OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	LONG curSel; 

	BTCREATEDATA *pBtCreateData;
	
	pBtCreateData = (BTCREATEDATA*)GetUserData(hWnd);

	switch(LOWORD(wParam))
	{
	case ID_SENDDATALIST:
		
		switch(HIWORD(wParam))
		{
		case LBN_SELCHANGE:
			curSel = SendMessage(pBtCreateData->hBtSendDatalist, LB_GETCURSEL, 0, 0);
			if(curSel==0)
			{
			    SendMessage(pBtCreateData->hCurAppFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(pBtCreateData->hCurAppFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			}
			else
			{
				SendMessage(pBtCreateData->hCurAppFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDP_BT_BUTTON_CONNECT);
				SendMessage(pBtCreateData->hCurAppFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			}
			break;
		}
	break;
	}
	
}

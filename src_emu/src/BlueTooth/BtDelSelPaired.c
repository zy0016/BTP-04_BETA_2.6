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
#include "BtPairedDevNode.h"
#include "BtString.h"

#define ID_DELMULSELPAIREDLIST     (WM_USER+0x200)

#define CONFIRM_DELSELPAIR         (WM_USER+0x300)
#define TIPS_DELDEVSUCCESS         (WM_USER+0x301)

extern PPAIREDDEVNODE pPairedDevHead;     //已配对设备链表头
 
static HWND hDelMulSelPairedList;      
static HWND hDelMulSelPairedWnd;               

static LRESULT DelMulSelPairedWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static void DelMulSelPaired_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);
static BOOL DelMulSelPaired_OnCreate(HWND hWnd);
static void DelMulSelPaired_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);
static void DelMulSelPaired(HWND hWnd);

static BOOL IsSomeSelDevConnected();

extern HWND GetBluetoothFrameWnd();

extern BOOL DelMulSelPairedWindow()
{
    WNDCLASS    wc;
	RECT rClient;
	HWND hBluetoothFrame;

    wc.style         = 0;
    wc.lpfnWndProc   = DelMulSelPairedWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "DelMulSelPairedWndClass";

	if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);

    hDelMulSelPairedWnd= CreateWindow(
		"DelMulSelPairedWndClass",
		"", 
        WS_VISIBLE | WS_CHILD,
         rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hBluetoothFrame, 
		NULL, 
		NULL, 
		NULL);

    if (NULL ==  hDelMulSelPairedWnd)
    {
        UnregisterClass("DelMulSelPairedWndClass",NULL);
        return FALSE;
    }
	
	SetFocus(hDelMulSelPairedWnd);
	
	ShowWindow(hDelMulSelPairedWnd,SW_SHOW);
	UpdateWindow(hDelMulSelPairedWnd);

    return (TRUE);
}

LRESULT DelMulSelPairedWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 
{
	HWND hBluetoothFrame;
	
	LRESULT lResult=TRUE;
	hBluetoothFrame=GetBluetoothFrameWnd();
    	
	switch (message)
    {
    case WM_CREATE :
		DelMulSelPaired_OnCreate(hWnd);	
        break;

	case WM_SETFOCUS:
        SetFocus(hDelMulSelPairedList);
        break;
		
    case PWM_SHOWWINDOW:
		{
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_DELETE);
			SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
			
			SetWindowText(hBluetoothFrame,IDP_BT_TITLE_SELECTPAIREDDEVICES);
			SetFocus(hDelMulSelPairedList);
		}
        break;

	case CONFIRM_DELSELPAIR:
		if (lParam)
		{
			int nCount;
			int i;
			BTDEVHDL CurDevHdl;
			

			nCount = SendMessage(hDelMulSelPairedList, LB_GETSELCOUNT, 0, 0);
			//得到当前用户所选的要删除已配对设备的个数
			
			if(nCount > 0)
			{	
				
				WaitWin(hWnd, TRUE, IDP_BT_STRING_DELETING,
					IDP_BT_TITLE_SELECTPAIREDDEVICES, NULL, NULL,0);
				
				for(i = 0; i < nCount; i++)
				{
					if(SendMessage(hDelMulSelPairedList, LB_GETSEL, (WPARAM)i, 0)>0)
					{
						CurDevHdl=SendMessage(hDelMulSelPairedList,LB_GETITEMDATA,
							(WPARAM)i,0);
						DelRemoteDevice(CurDevHdl);
						pPairedDevHead=DeletePairedDevNode(pPairedDevHead,CurDevHdl);
					}
				}
								
				WaitWin(hWnd, FALSE, IDP_BT_STRING_DELETING,
					IDP_BT_TITLE_SELECTPAIREDDEVICES, NULL, NULL,0);

                SetPairedDevInfoToFile(pPairedDevHead);   //存储已配对的设备到文件中

				PLXTipsWin(hBluetoothFrame, hWnd, TIPS_DELDEVSUCCESS,IDP_BT_STRING_DELETEDNOTIF,
					IDP_BT_TITLE_PAIREDDEVICES,Notify_Success, IDP_BT_BUTTON_OK, NULL,20);				
			}
		}
		else
		{
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		}
		break;

	case TIPS_DELDEVSUCCESS:
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;
		
	case WM_KEYDOWN:
		DelMulSelPaired_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
		
    case WM_DESTROY :
        UnregisterClass("DelMulSelPairedWndClass", NULL);
        break;
		
    default:    
        lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
        break;
    }
    
    return lResult;
}

BOOL DelMulSelPaired_OnCreate(HWND hWnd)
{
	int curIndex;
	RECT rClient;
	HWND hBluetoothFrame;
    PPAIREDDEVNODE pCurPairedPointer;
    
	GetClientRect(hWnd, &rClient);
	hBluetoothFrame=GetBluetoothFrameWnd();
	
	hDelMulSelPairedList=CreateWindow(
        "MULTILISTBOX",
        "",
        WS_VISIBLE | WS_CHILD | WS_VSCROLL | LBS_SORT| LBS_BITMAP,
        rClient.left,rClient.top,
		rClient.right-rClient.left,
        rClient.bottom - rClient.top,
        hWnd,
        (HMENU)ID_DELMULSELPAIREDLIST,
        NULL,
        NULL);
	
	if(NULL==hDelMulSelPairedList)
		return FALSE;

	pCurPairedPointer=pPairedDevHead;
	
	while(pCurPairedPointer!=NULL)
	{
		if(strlen(pCurPairedPointer->PairedDevAliasName)!=0)
			curIndex=SendMessage(hDelMulSelPairedList, LB_INSERTSTRING, 0, 
			        (LPARAM)pCurPairedPointer->PairedDevAliasName);
		else
            curIndex=SendMessage(hDelMulSelPairedList, LB_INSERTSTRING, 0, 
			        (LPARAM)pCurPairedPointer->PairedDevName);
		
		SendMessage(hDelMulSelPairedList,LB_SETITEMDATA,
			(WPARAM)curIndex,(LPARAM)pCurPairedPointer->PairedDevHandle);

	    pCurPairedPointer=pCurPairedPointer->Next;
	}
		  	
	SendMessage(hDelMulSelPairedList, LB_SETCURSEL, 0, 0);
	SendMessage(hDelMulSelPairedList, LB_ENDINIT, 0, 0);

    SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_DELETE);
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"Select");
	
	SetWindowText(hBluetoothFrame,IDP_BT_TITLE_SELECTPAIREDDEVICES);
    
	SendMessage(hDelMulSelPairedList, LB_SETCURSEL,0,0);
	
	return TRUE;	
}

static void DelMulSelPaired_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{
	HWND hBluetoothFrame;
	
    hBluetoothFrame=GetBluetoothFrameWnd();
	
	switch(nKey)
	{
	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;
	case VK_RETURN:
        DelMulSelPaired(hWnd);
		break;
	}
}

void DelMulSelPaired(HWND hWnd)
{ 
	int nCount;
	HWND hBluetoothFrame;
	
	hBluetoothFrame=GetBluetoothFrameWnd();

    nCount = SendMessage(hDelMulSelPairedList, LB_GETSELCOUNT, 0, 0);
	//得到当前用户所选的要删除已配对设备的个数

	if(nCount==0)
	{
		PLXTipsWin(hBluetoothFrame, hWnd, 0,
			IDP_BT_STRING_NOPAIREDDEVSELPROMPT, IDP_BT_TITLE_SELECTPAIREDDEVICES,
			Notify_Alert, IDP_BT_BUTTON_OK, NULL,20);	
	}
    else
	{
		if(IsSomeSelDevConnected())
		{
			PLXConfirmWinEx(hBluetoothFrame,hWnd,
				IDP_BT_STRING_SOMEDEVDISCONNDELPROMPT,
				Notify_Request, IDP_BT_TITLE_PAIREDDEVICES, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_DELSELPAIR);
		}
		else
		{
			PLXConfirmWinEx(hBluetoothFrame,hWnd,
				IDP_BT_STRING_DelSelPairingsPrompt,
				Notify_Request, IDP_BT_TITLE_PAIREDDEVICES, IDP_BT_BUTTON_YES, IDP_BT_BUTTON_NO, CONFIRM_DELSELPAIR);
		}
    }
}

BOOL IsSomeSelDevConnected()
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

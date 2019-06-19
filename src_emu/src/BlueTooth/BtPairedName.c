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
#include "BtPairedDevNode.h"
#include "BtString.h"

#define ID_PAIREDNAME                  (WM_USER+0x200)

#define ALIAS_LENGTH_LIMIT                  30

static HWND hPairedRenameWnd;
static HWND hPairedRenameEdit;

extern HWND hPairedlist;  //Paired主窗口里面的ListBox句柄
extern HWND hPairedWnd;   //Paired主窗口句柄
extern PPAIREDDEVNODE pPairedDevHead; 

// static char szPairedName[30];

static LRESULT PairedRenameWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
static BOOL PairedRename_OnCreate(HWND hWnd,LPARAM lParam);
static void PairedRename_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags);

extern HWND GetBluetoothFrameWnd();

BOOL PairedRenameWindow(LONG curSel)
{
    WNDCLASS    wc;
	HWND hBluetoothFrame;
	RECT rClient;

    wc.style         = 0;
    wc.lpfnWndProc   = PairedRenameWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "PairedRenameWndClass";

	if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	hBluetoothFrame=GetBluetoothFrameWnd();
	GetClientRect(hBluetoothFrame,&rClient);

    hPairedRenameWnd = CreateWindow(
		"PairedRenameWndClass",
		"", 
        WS_VISIBLE | WS_CHILD,
         rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		hBluetoothFrame, 
		NULL, 
		NULL, 
		(LPVOID)curSel);

    if (NULL ==  hPairedRenameWnd)
    {
        UnregisterClass("PairedRenameWndClass",NULL);
        return FALSE;
    }
	
	SetFocus(hPairedRenameWnd);
	
	ShowWindow(hPairedRenameWnd,SW_SHOW);
	UpdateWindow(hPairedRenameWnd);

	SendMessage(hPairedRenameEdit, EM_SETSEL, -1, -1);

    return (TRUE);
}

LRESULT PairedRenameWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam) 
{
	HWND hBluetoothFrame;	
	LRESULT lRet = TRUE;

	hBluetoothFrame=GetBluetoothFrameWnd();

	switch(wMsgCmd)
	{		
	   case WM_CREATE:
		   PairedRename_OnCreate(hWnd,lParam);
		   break;
		   
	   case WM_SETFOCUS:
		   SetFocus(hPairedRenameEdit);
		   break;
		   
	   case PWM_SHOWWINDOW:
		   SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
		   SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_SAVE);
		   SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");

		   SetWindowText(hBluetoothFrame,IDP_BT_TITLE_RENAMEDEVICE);
		   SetFocus(hPairedRenameEdit);
		   break;
		   
	   case WM_KEYDOWN:
		    PairedRename_OnKeyDown(hWnd, (UINT)wParam, (UINT)lParam);
			break;
	
	   case WM_CLOSE:
			DestroyWindow(hWnd);
			break;
	   case WM_DESTROY:
			UnregisterClass("PairedRenameWndClass", NULL);
			break;
	   default:
			lRet = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
	}
	return lRet;
}

BOOL PairedRename_OnCreate(HWND hWnd,LPARAM lParam)
{
	LONG curSel;
	IMEEDIT ie;
	RECT rClient;
	HWND hBluetoothFrame;
	char curSelPairedName[30];

	memset((void*)&ie, 0, sizeof(IMEEDIT));
	memset(curSelPairedName,0,30);

    curSel=(LONG)((LPCREATESTRUCT) lParam)->lpCreateParams;  
    SendMessage(hPairedlist,LB_GETTEXT,(WPARAM)curSel,(LPARAM)(LPCTSTR)curSelPairedName);
	//得到paired窗口传送过来的Paired过的设备名
	
	hBluetoothFrame=GetBluetoothFrameWnd();
	
	ie.hwndNotify   = hWnd;
	GetClientRect(hWnd, &rClient);

	hPairedRenameEdit = CreateWindow(
        "IMEEDIT",
        "",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | ES_AUTOVSCROLL | ES_MULTILINE | ES_TITLE,
        rClient.left, rClient.top,
		rClient.right - rClient.left, rClient.bottom - rClient.top - 47*2,
        hWnd,
        (HMENU)ID_PAIREDNAME,
        NULL,
        &ie);
	
	SendMessage(hPairedRenameEdit, EM_LIMITTEXT, ALIAS_LENGTH_LIMIT, 0);
	SendMessage(hPairedRenameEdit, EM_SETTITLE, 0, (LPARAM)IDP_BT_STRING_DEVICENAME);
    
	SetWindowText(hPairedRenameEdit, curSelPairedName);
    
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,0,(LPARAM)IDP_BT_BUTTON_CANCEL);
	SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,1,(LPARAM)IDP_BT_BUTTON_SAVE);
    SendMessage(hBluetoothFrame,PWM_SETBUTTONTEXT,2,(LPARAM)"");

	SetWindowText(hBluetoothFrame,IDP_BT_TITLE_RENAMEDEVICE);
			
	SetFocus(hPairedRenameEdit);

	return TRUE;
}

static void PairedRename_OnKeyDown(HWND hWnd, UINT nKey, UINT nFlags)
{
	LONG curSel;
	char tmpBuff[30];
    char szReNamePaired[30];

	HWND hBluetoothFrame;
	BTDEVHDL CurDevHdl;
	PPAIREDDEVNODE pListPointer;

	memset(tmpBuff,0,30);
	memset(szReNamePaired,0,30);
	
    hBluetoothFrame=GetBluetoothFrameWnd();
	
	switch(nKey)
	{
	case VK_F10:
		PostMessage(hWnd,WM_CLOSE,0,0);
		SendMessage(hBluetoothFrame,PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
		break;

	case VK_RETURN:
		
		GetWindowText(hPairedRenameEdit,tmpBuff,30);
		
		if(strlen(tmpBuff)==0)
		{
			PLXTipsWin(hBluetoothFrame, hWnd, 0,IDP_BT_STRING_DEFINENAME, IDP_BT_TITLE_RENAMEDEVICE,
				Notify_Alert, IDP_BT_BUTTON_OK, NULL,20);	
		}
		else			
		{
			strcpy(szReNamePaired,tmpBuff);
			
			curSel=SendMessage(hPairedlist,LB_GETCURSEL,0,0);

			CurDevHdl=SendMessage(hPairedlist,LB_GETITEMDATA,
				(WPARAM)curSel,0);
			pListPointer=GetDeviceNodePointer(pPairedDevHead,CurDevHdl);
			
			SendMessage(hPairedlist,LB_SETTEXT,(WPARAM)curSel,(LPARAM)szReNamePaired);
			
			strcpy(pListPointer->PairedDevAliasName,szReNamePaired);
			
			SendMessage(hWnd, WM_CLOSE, 0, 0);
		}							
		break;
	default:
		PDADefWindowProc(hWnd, WM_KEYDOWN, (WPARAM)nKey, (LPARAM)nFlags);
		break;  
	}
	
}



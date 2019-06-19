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
 
HWND hBtSettingsStatus;            //ȫ�ֱ�����Settings������Bluetooth�Ƿ�򿪵�SpinBox�ؼ����
HWND hBtSettingsVisibility;        //ȫ�ֱ�����Settings������Bluetooth�Ƿ�ɼ���SpinBox�ؼ����
HWND hBtSettingsPhoneName;         //ȫ�ֱ�����Settings������Bluetooth�豸����SpinBox�ؼ����

HWND hBtSettingsWnd;               //ȫ�ֱ�����Settings���ھ��

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

	static BOOL bOldBluetoothStatus;  //û�иı�ǰ������״̬

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
			
			hBtSettingsStatus = CreateWindow(      //�����򿪹ر�״̬��spinbox
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
			
			if(bBluetoothStatus)                             //����ԭ��Bluetooth���õ�״̬
				SendMessage(hBtSettingsStatus, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_ON);
			else
				SendMessage(hBtSettingsStatus, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_OFF);
			
			hBtSettingsVisibility = CreateWindow(      //�����ɼ���״̬��spinbox
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
				
				if(bVisibilityStatus)                            //����ԭ��Visibility���õ�״̬
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
			
			hBtSettingsPhoneName = CreateWindow(      //�����������ֵ�spinbox
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
		
		    hFocus = hBtSettingsStatus;   //Ĭ�ϵĽ���������������򿪺͹رյ�ѡ������
		}
        break;
		
	case WM_SETFOCUS:
        SetFocus(hFocus);
        break;

    case PWM_SHOWWINDOW:
        {
			LONG OldSel;

			printf("\n#########In BTSetttings PWM_SHOWWINDOW##########\n");
			
            if(!bOldBluetoothStatus && bBluetoothStatus)   //������״̬�ӹرյ���
				                                           //���������ɼ�����Ӧ��״̬
			{ 
				SendMessage(hBtSettingsVisibility,SSBM_DELETESTRING,0,0);

				SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_ON);
				SendMessage(hBtSettingsVisibility, SSBM_ADDSTRING,0,(LPARAM)IDP_BT_STRING_OFF);
                
				if(bVisibilityStatus)
				   SendMessage(hBtSettingsVisibility, SSBM_SETTEXT,0, (LPARAM)IDP_BT_STRING_ON); 
				else
				   SendMessage(hBtSettingsVisibility, SSBM_SETTEXT,0, (LPARAM)IDP_BT_STRING_OFF); 
			}

			if(bOldBluetoothStatus && !bBluetoothStatus)  //������״̬�Ӵ򿪵��ر�
				                                          //���������ɼ�����Ӧ��״̬
			{
				OldSel = SendMessage(hBtSettingsVisibility, SSBM_GETCURSEL, 0, 0);

                SendMessage(hBtSettingsVisibility,SSBM_DELETESTRING,1,0);   //��ɾ�ڶ���
				SendMessage(hBtSettingsVisibility,SSBM_DELETESTRING,0,0);   //��ɾ��һ��

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
                    bOldBluetoothStatus=bBluetoothStatus;   //����ԭ��������״̬

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
				WriteStatusFile(BTSTATUSFILENAME);   //�˳�ʱ�ѵ�ǰ������������Ϣд���ļ�
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
			
            DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //֪ͨ���������������
			
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
			
            DlmNotify(PS_SETBLUETOOTHON,ICON_SET);  //֪ͨ���������������
			
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

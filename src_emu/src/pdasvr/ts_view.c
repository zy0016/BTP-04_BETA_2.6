/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : TFTP Server Module
 *
 * Purpose  : TFTP Main Procedure
 *            
\**************************************************************************/


#include "window.h"    
#include "device.h"  
#include "fapi.h"
#include "plxdebug.h" 
#include "hpimage.h"
#include "winpda.h"
#include "plx_pdaex.h" 
#include "pubapp.h"
#include "di_uart.h"
#include "tsmain.h"


extern char Server_ver	[16];


BOOL Tftp_IsConnected = FALSE;
static HINSTANCE hSvr_Instance;
static HWND m_hwnd;
static BOOL IsIniOK;
static BYTE	Tftp_connect_type;


#define WM_TFTP_DATAIN	WM_USER+100
#define IDC_EXIT		10

HWND	SVR_hwndApp = NULL;


static LRESULT	PdaSvrWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,  LPARAM lParam);


/*--------------------------------------------------------
 *
 * TFTP Server View App
 *
 *-------------------------------------------------------*/

void PdaSvr_Close(void)
{

	SendMessage(m_hwnd, WM_CLOSE, NULL, NULL);
	
	PCFTP_Dest();	//�رշ�����

	//����˯��
	f_sleep_unregister(PSYNC_TANSFER);

	//����ػ�
	f_DisablePowerOff(PCSYNC);

	IsIniOK = FALSE;
	Tftp_IsConnected = FALSE;
}

BOOL PdaSvr_Open(int connect_type)
{
	
	WNDCLASS	wc;
	DWORD		dwRet = TRUE;

	// �ж��Ƿ��Ѿ�������, ����Ѿ�����������ֱ�ӷ���
	if (Tftp_IsConnected) 
		return FALSE;

	// ���ֻ�ǳ�ʼ���ɹ�
	if (IsIniOK) 
	{
		// �����������BT��ֱ�ӷ���
		if ((TFTP_CONNECT_BT == connect_type)) 
			return FALSE;

		// ��������ӷ�ʽ�Ѿ���usb�򷵻�
		if ((TFTP_CONNECT_USB == connect_type) 
			&& (TFTP_CONNECT_USB == Tftp_connect_type)) 
			return FALSE;

		// ���ԭ�����������ĳ�ʼ��(��û������), ���½�������������usb �������,����usb
		if ((TFTP_CONNECT_USB == connect_type)
			&& (TFTP_CONNECT_BT == Tftp_connect_type)) 
			PCFTP_Dest();	//�رշ�����(����)			
	}

	IsIniOK =  FALSE;

	wc.style         = 0;
	wc.lpfnWndProc   = PdaSvrWndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = NULL;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.hbrBackground = GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = "PdaSvrWndClass";

	RegisterClass(&wc);

	SVR_hwndApp = CreateWindow("PdaSvrWndClass", 
							   "PC Sync", 
							   WS_CAPTION,
							   PLX_WIN_POSITION,
							   NULL, NULL, NULL, NULL);


	switch(connect_type) 
	{
	case TFTP_CONNECT_USB:	// usb
			if(PCFTP_Init("DEV:UART5", 32, 57600) != 0)
				return FALSE;

			Tftp_connect_type = TFTP_CONNECT_USB;	//	��¼�����ͱ�
			IsIniOK = TRUE;
		break;
		
	case TFTP_CONNECT_BT:	//	BT
			if(PCFTP_Init("DEV:UART6", 32, 57600) != 0)
				return FALSE;

			Tftp_connect_type = TFTP_CONNECT_BT;	//	��¼�����ͱ�
			IsIniOK = TRUE;
		break;

	default:
		return FALSE;
		break;
	}

	PCFTP_Register(SVR_hwndApp, WM_TFTP_DATAIN, TS_OP_OPTION);
	PCFTP_Register(SVR_hwndApp, WM_TFTP_DATAIN, TS_OP_DIR);	

	//��ֹ˯��
	f_sleep_register(PSYNC_TANSFER);

	//��ֹ�ػ�
	f_EnablePowerOff(PCSYNC);

	return TRUE;
}



/*--------------------------------------------------------
 *
 * ��������PdaSvrWndProc
 * ��  �ܣ��ļ�������������Ϣ������
 *
 *-------------------------------------------------------*/
static LRESULT PdaSvrWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,  LPARAM lParam)                        
{
	static int		pTimes = 0;

	switch (wMsgCmd)
	{
	case WM_CREATE:
		StartObjectDebug();
		m_hwnd = hWnd;
		break;

	case WM_SELOK:
		DestroyWindow( hWnd );
		break;

	case WM_COMMAND :
		switch (LOWORD(wParam))
		{
		case IDC_EXIT:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_CLOSE:		
		DestroyWindow(hWnd);
		break;

	case WM_TFTP_DATAIN:
		PCFTP_Readdata();
		break;

	case WM_DESTROY:
		SVR_hwndApp = NULL;
		DlmNotify(PES_STCQUIT, (int)PdaSvr_Open);
		EndObjectDebug();
		break;

	default :
		return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);			
    }

    return 0;
}

// End of Program

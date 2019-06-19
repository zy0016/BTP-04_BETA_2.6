/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : SmartPhone Version_NOWIN
 *            
\**************************************************************************/

#include "window.h"
#include "debug.h"
#include "string.h"

#include "mac_proc.h"

#include "dialer.h"
#include "dialmain.h"

#define  _DIALER_DEBUG_

DIALERBLOCK		dialer;
DIALINFO		DialInfo;
DIALINFO		Dial_MMSInfo;

extern int DIALER_Initialize( void );
extern int DIALER_SelConnect( HWND, UINT, int, void* );
extern int DIALER_HangUp( void );
extern int DIALER_DialCancel( void );
extern int DIALER_GetLineState( void );


/*-------------------------------------------------------------------
 *
 * ����ģ��ϵͳ��ʼ��
 *
 * ������hWnd		������ͼ���ڵ��������ھ��
 *
 * ����ֵ��0 �ɹ�,  -1 ʧ��
 *
 *--------------------------------------------------------------------*/

extern LRESULT CALLBACK dial_Ctl_Proc( HWND , UINT , WPARAM , LPARAM );

int DIALER_Initialize( void )
{
	WNDCLASS	Dial_WndClass;

	/* ����Dial���� */
	Dial_WndClass.style         = 0;
	Dial_WndClass.lpfnWndProc   = dial_Ctl_Proc;
	Dial_WndClass.cbClsExtra    = 0;
	Dial_WndClass.cbWndExtra    = 0;
	Dial_WndClass.hInstance     = NULL;
	Dial_WndClass.hIcon         = NULL;
	Dial_WndClass.hCursor       = NULL;
	Dial_WndClass.hbrBackground = GetStockObject(WHITE_BRUSH);
	Dial_WndClass.lpszMenuName  = NULL;
	Dial_WndClass.lpszClassName = "Dial_CtlWnd";
	if (!RegisterClass(&Dial_WndClass))
		return -1;

	dialer.hDialWnd = CreateWindow( "Dial_CtlWnd" ,
									"Dial_Ctl" ,
									0,
									0, 0, 0, 0,
									NULL, NULL, NULL, NULL
									);
	if (dialer.hDialWnd == NULL)
		return -1;

	DialInfo.Valid = 0;
	Dial_MMSInfo.Valid = 0;

	/* ����MACʵ�壬����������MACʵ�����ϵ */
	MAC_Instance( dialer.hDialWnd, WM_DIAL_MACCALLBACK );

	return 0;
}

/*------------------------------------------------------------------
 *
 * ��ʼ���ţ������������������û�ѡ��UDB���ݿ��е�ISP��Ϣ���в���
 *
 * ������	hWnd		���ô��ھ��
 *			msg			���ô�����Ϣ
 *			SelectType	���ŵ�������
 *			pSrcInfo	���Ӳ�����Ϣ
 *
 * ����ֵ��		DIALER_REQ_ACCEPT ���ܹҶ�����
 *				DIALER_REQ_REFUSE �ܾ��Ҷ�����
 *				DIALER_MCH_FAILURE
 *				DIALER_INVALID_ISP
 *
 *------------------------------------------------------------------*/

extern void Sock_StopDial();

int DIALER_SelConnect( HWND hWnd, UINT msg, int SelectType, void *pSrcInfo )
{
	if( dial_RunStage() != DIALER_INVALIDSTAGE )
	{
		Sock_StopDial();
		return DIALER_REQ_REFUSE;
	}

	// Open Uart Channel
	if( (dialer.MacHandle = MAC_Open( (char*)DIAL_DEFCOMNAME )) <0 )
	{
#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: MAC_Open() return is %d \r\n", dialer.MacHandle);
#endif

		Sock_StopDial();
		MAC_AbortNetDial();
		return DIALER_MCH_FAILURE;
	}

	// Get DialInfo
	if ( dial_GetEnvelope (SelectType, pSrcInfo, &DialInfo) == -1 )
	{
		Sock_StopDial();
		MAC_AbortNetDial();
		MAC_Close( dialer.MacHandle );
#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: MAC_Close() return is %d \r\n", dialer.MacHandle);
#endif
		return DIALER_INVALID_ISP;
	}

	dialer.CallerWnd = hWnd;
	dialer.CallBackMsg = msg;
	dialer.SelectISP = SelectType;
	dialer.CallManner = DM_VIDEO;

	dialer.HangupWnd = NULL;
	dialer.HangupMsg = 0;

	PostMessage( dialer.hDialWnd, WM_DIAL_START, 0, 0L );

	return DIALER_REQ_ACCEPT;
}


/*------------------------------------------------------------------
 *
 * �Ҷ���������
 *
 * ����ֵ��		DIALER_REQ_ACCEPT ���ܹҶ�����
 *				DIALER_REQ_REFUSE �ܾ��Ҷ�����
 *
 *------------------------------------------------------------------*/

int DIALER_HangUp( void )
{
	if( dial_RunStage() == DIALER_NETWORKSTAGE )
	{
		PostMessage( dialer.hDialWnd, WM_DIAL_HANGUP, 0, 0L );
		return DIALER_REQ_ACCEPT;
	}

	if( dial_RunStage() == DIALER_CONNECTSTAGE 
		|| dial_RunStage() == DIALER_PPPSTAGE )
	{
		// Terminate Dialer Connect
		PostMessage( dialer.hDialWnd, WM_DIAL_RUNCANCEL, 0, 0L );
		return DIALER_REQ_ACCEPT;
	}

	return DIALER_REQ_REFUSE;
}

/*
int DIALER_HangUp( void )
{
	if( dial_RunStage() != DIALER_NETWORKSTAGE )
		return DIALER_REQ_REFUSE;

	PostMessage( dialer.hDialWnd, WM_DIAL_HANGUP, 0, 0L );
	return DIALER_REQ_ACCEPT;
}*/


/*------------------------------------------------------------------
 *
 * ��ֹ��������
 *
 * ����ֵ��		DIALER_REQ_ACCEPT ������ֹ����
 *				DIALER_REQ_REFUSE �ܾ���ֹ����
 *
 *------------------------------------------------------------------*/

int DIALER_DialCancel( void )
{
	if( dial_RunStage() == DIALER_NETWORKSTAGE )
	{
		PostMessage( dialer.hDialWnd, WM_DIAL_HANGUP, 0, 0L );
		return DIALER_REQ_ACCEPT;
	}

	if( dial_RunStage() == DIALER_CONNECTSTAGE 
		|| dial_RunStage() == DIALER_PPPSTAGE )
	{
		// Terminate Dialer Connect
		PostMessage( dialer.hDialWnd, WM_DIAL_RUNCANCEL, 0, 0L );
		return DIALER_REQ_ACCEPT;
	}

	return DIALER_REQ_REFUSE;
}

/*
int DIALER_DialCancel( void )
{
	if( dial_RunStage() != DIALER_CONNECTSTAGE 
		&& dial_RunStage() != DIALER_PPPSTAGE )
		return DIALER_REQ_REFUSE;

	// Terminate Dialer Connect
	PostMessage( dialer.hDialWnd, WM_DIAL_RUNCANCEL, 0, 0L );
	return DIALER_REQ_ACCEPT;
}
*/


/*-------------------------------------------------------------------
 *
 * ��ò���ģ�鵱ǰ��״̬
 *
 * ����ֵ������ģ�鵱ǰ��״̬
 *				DIALER_INVALIDSTAGE		����״̬
 *				DIALER_CONNECTSTAGE		����״̬
 *				DIALER_NETWORKSTAGE		����״̬
 *				DIALER_PPPSTAGE			PPPЭ��״̬
 *				DIALER_HANGUPSTAGE		�Ҷ�״̬
 *
 *--------------------------------------------------------------------*/

int DIALER_GetLineState( void )
{
	return dial_RunStage();
}


/*--------------------------------------------------------------------
 *
 * Notify Dialer Running Message
 *
 *--------------------------------------------------------------------*/

int DIALER_NotifyRegister( HWND hWnd, int msg )
{
	if( hWnd != dialer.CallerWnd || msg != dialer.CallBackMsg )
	{
		dialer.HangupWnd = hWnd;
		dialer.HangupMsg = msg;
	}
	return 0;
}

int DIALER_NotifyUNRegister( HWND hWnd, int msg )
{
	if( hWnd == dialer.HangupWnd && msg == dialer.HangupMsg )
	{
		dialer.HangupWnd = NULL;
		dialer.HangupMsg = 0;
	}
	return 0;
}

/*--------------------------------------------------------------------
 * 1:��ͬ
 * 0:��ͬ
 * -1:��Ч����
 *--------------------------------------------------------------------*/

int DIALER_CompareInfo( int SelectType, void *pSrcInfo )
{
	dial_GetEnvelope (SelectType, pSrcInfo, &Dial_MMSInfo);

	if( !DialInfo.Valid || !Dial_MMSInfo.Valid )
		return -1;

	if( Dial_MMSInfo.SelMode == DialInfo.SelMode
		&& strcmp( Dial_MMSInfo.PhoneNum, DialInfo.PhoneNum ) == 0
		&& strcmp( Dial_MMSInfo.UserName, DialInfo.UserName ) == 0
		&& strcmp( Dial_MMSInfo.PassWord, DialInfo.PassWord ) == 0 )
		return 1;

	return 0;
}
/*--------------------------------------------------------------------
 * �ɹ�����MAC handle
 * ʧ�ܷ���-1
 *--------------------------------------------------------------------*/

int Get_MAC_Handle()
{
	if (dialer.MacHandle >= 0)
		return dialer.MacHandle;
	else 
		return -1;
}

/* End Of program */

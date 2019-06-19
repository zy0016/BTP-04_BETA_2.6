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
#include "stdio.h"
#include "mac_proc.h"
#include "plx_pdaex.h"
//#include "hp_dhi.h"

#include "dialer.h"
#include "dialmain.h"

#define _DIALER_DEBUG_

extern void Sock_StopDial();
extern void Sock_DialLineUp();

extern DIALERBLOCK	dialer;
extern DIALINFO		DialInfo;

RUNINFO	Run_Info;


/*----------------------ʵ�ֺ���---------------------------------*/

extern LRESULT CALLBACK dial_Ctl_Proc( HWND , UINT , WPARAM , LPARAM );

static int dial_Ctl_Operate( int );

static int dial_Ctl_ScrPar_DailEnd( void );
static int dial_Ctl_ScrPar_LoginEnd( void );
static int dial_Ctl_ScrPar_HangupEnd( void );
static int dial_Ctl_NegotiateEnd( int );

static int dial_Ctl_AutoLineDown( void );
static int dial_Ctl_MacLineStatus( int );

extern int dial_RunStage( void );
static int Dialer_SetError( int );
extern int Dialer_GetError( void );

static int dial_Ctl_StopNetwork( void );
//extern int  f_sleep_register(void );
//extern int f_sleep_unregister(int handle); 
//static int sleephandle = 0;

/*----------------------------------------------------------------
 *
 * Dial���ڵ����̣���������Ŷ���
 *
 *---------------------------------------------------------------*/

extern LRESULT CALLBACK dial_Ctl_Proc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                   LPARAM lParam )
{
	switch ( wMsgCmd )
	{
	case WM_DIAL_MACCALLBACK:
		switch( LOBYTE(wParam) )
		{
/*		case NDIS_ADAPTER:
			switch (HIBYTE(wParam))
			{
			case ADPTOR_OPEN:
				if( dial_ReadyForStart() ==-1)
					return -1;
				dialer.CallManner = DM_TCPIP;
				dial_GetEnvelope();
				return 0;
			case ADPTOR_CLOSE:
				if( Run_Info.run_state != DS_NET )
					return -1;
				PostMessage(dialer.hDialWnd, WM_DIAL_HANGUP, 0, 0L);
				return 0;
			}
*/
		case EXPLAIN_END:
			Run_Info.ScrParse_Ret = (int)lParam;
			switch ( Run_Info.run_state )
			{
			case DS_DIAL:
				dial_Ctl_ScrPar_DailEnd();
				return 0;
			case DS_LOGIN:
				dial_Ctl_ScrPar_LoginEnd();
				return 0;
			case DS_HANGUP:
				dial_Ctl_ScrPar_HangupEnd();
				return 0;
			}
			return 0;

		case PPP_NEGOTIATE:
			dial_Ctl_NegotiateEnd( (int)HIBYTE(wParam) );
			return 0;

		case LINE_EVENT:
			dial_Ctl_MacLineStatus( (int)HIBYTE(wParam) );
			return 0;
		}
		return 0;

	case WM_DIAL_START:

#ifdef SMARTPHONE2
		DHI_SetNotSleepFlag(DIAL_FLAG);
#endif

		// Set Dialer configuration
		MAC_SetOption( dialer.MacHandle, OPT_CALLWND, (int)dialer.hDialWnd, sizeof(HWND) );
		MAC_SetOption( dialer.MacHandle, OPT_CALLMSG, WM_DIAL_MACCALLBACK, sizeof(int) );

		MAC_SetOption( dialer.MacHandle, OPT_PHONENUM, 
			(int)DialInfo.PhoneNum, strlen(DialInfo.PhoneNum) );
		MAC_SetOption( dialer.MacHandle, OPT_USERNAME, 
			(int)DialInfo.UserName, strlen(DialInfo.UserName) );
		MAC_SetOption( dialer.MacHandle, OPT_PASSWORD, 
			(int)DialInfo.PassWord, strlen(DialInfo.PassWord) );

		MAC_SetOption(dialer.MacHandle, OPT_DNS1, (int)DialInfo.Dns1, strlen(DialInfo.Dns1));
		MAC_SetOption(dialer.MacHandle, OPT_DNS2, (int)DialInfo.Dns2, strlen(DialInfo.Dns2));

		if( DialInfo.SelMode == DIALER_RUNMODE_DATA )
		{
			// DATA Dialer
			MAC_SetOption( dialer.MacHandle, OPT_SCRIPTNAME, (int)DIAL_DATA_SCRIPT, strlen(DIAL_DATA_SCRIPT) );
		}
		else if( DialInfo.SelMode == DIALER_RUNMODE_GPRS )
		{
			// GPRS Dialer
			MAC_SetOption( dialer.MacHandle, OPT_SCRIPTNAME, (int)DIAL_GPRS_SCRIPT, strlen(DIAL_GPRS_SCRIPT) );
		}

		// Start DialNet
		dial_Ctl_Operate( DS_DIAL );
		return 0;

	case WM_DIAL_HANGUP:
		dial_Ctl_StopNetwork();
		//dial_Ctl_Operate( DS_HANGUP );

		return 0;

	case WM_DIAL_RUNCANCEL:
		Sock_StopDial();

		if( Run_Info.run_state == DS_DIAL && Run_Info.rep_times > 0 )
			KillTimer( dialer.hDialWnd, DIAL_BUSYTIMER );

		switch( Run_Info.run_state )
		{
		case DS_DIAL:
		case DS_LOGIN:
			dial_Ctl_Operate( DS_STOP );
			return 0;

		case DS_PPP:
		case DS_PAP:
			dial_Ctl_StopNetwork();
			//dial_Ctl_Operate( DS_HANGUP );

			return 0;
		}
		return 0;

	case WM_TIMER:
		switch( (int)wParam )
		{
		case DIAL_TESTTIMER:
			dial_Ctl_AutoLineDown();
			return 0;

		case DIAL_BUSYTIMER:
			KillTimer( dialer.hDialWnd, DIAL_BUSYTIMER );
			dial_Ctl_Operate( DS_DIAL );
			return 0;
			
		case DIAL_HANGUPTIMER:
			KillTimer( dialer.hDialWnd, DIAL_HANGUPTIMER );
			dial_Ctl_ScrPar_HangupEnd();
			return 0;

		case DIAL_STOPNETWORKTIMER:
			KillTimer( dialer.hDialWnd, DIAL_STOPNETWORKTIMER );

			//StopNetWork Failure, Call NetWorkDown to Force PPP Stop
			MAC_NetworkDown( dialer.MacHandle );
			dial_Ctl_Operate( DS_HANGUP );
			return 0;
		}

	case WM_CREATE:
		// Run_Info Init
		Run_Info.run_state = DS_INVALID;
		Run_Info.Mac_LineStatus = LINE_BROKEN;
		return 0;

	default:
		return DefWindowProc (hWnd, wMsgCmd, wParam, lParam);
	}
	return 0;
}


/*---------------------------------------------------------------
 *
 * ������ͨ�����ŹҶϵĿ�ʼ����
 *
 * ����stage:	DS_DIAL		��ʼ������ͨ	
 *				DS_HANGUP	��ʼ�Ҷ�
 *
 *--------------------------------------------------------------*/

static int dial_Ctl_Operate( int stage )
{
	switch ( stage )
	{
	case DS_DIAL:
		//DlmNotify(PS_SETGPRS, 2); //֪ͨ�����������ʼ����
		//sleephandle = f_sleep_register();
		f_sleep_register(GPRS_CONNECT);
		if( Run_Info.stg_track != DT_NOTPASS )
			Run_Info.rep_times = 0;
		Run_Info.run_state = DS_DIAL;
		Run_Info.stg_track = DT_BEGIN;
		Run_Info.ReNegFlag = 0;

		MAC_SetOption( dialer.MacHandle, OPT_CALLSTATE, SCRPAR_CALL_DIAL, sizeof(int) );
		if( MAC_ScriptExplain( dialer.MacHandle, SCR_START ) == RTN_WOULDBLOCK )
		{
			REPORTBACK( DialInfo.SelMode, DIALER_RUN_DIALING );
#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: ���ڲ���..." );
#endif
			return 0;
		}

		Run_Info.run_state = DS_INVALID;

		MAC_AbortNetDial();
		MAC_Close( dialer.MacHandle );
#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: MAC_Close() return is %d \r\n", dialer.MacHandle);
#endif

		Dialer_SetError( DIALER_RUN_SCREXP_ERROR );
		REPORTBACK( DialInfo.SelMode, DIALER_RUN_NET_ABNORMAL );

#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: �ű�����������" );
#endif
		return -1;

	case DS_HANGUP:
		//DlmNotify(PS_SETGPRS, 3); //֪ͨ�����������ʼ�Ҷ�
		//f_sleep_unregister(sleephandle);
		f_sleep_unregister(GPRS_CONNECT);
		if( Run_Info.run_state == DS_DIAL || Run_Info.run_state == DS_LOGIN )
			MAC_AbortNetDial();
		else
			MAC_NetworkDown( dialer.MacHandle );
//			MAC_StopNetwork( dialer.MacHandle );

		if( DialInfo.AutoHangupTimeout > 0 )
			KillTimer( dialer.hDialWnd, DIAL_TESTTIMER );

		Run_Info.stg_track = DT_BEGIN;
		Run_Info.run_state = DS_HANGUP;

		/* ���нű������� */
		if( Run_Info.Mac_LineStatus == LINE_CONNECTED )
			MAC_SetOption( dialer.MacHandle, OPT_CALLSTATE, SCRPAR_CALL_HANGUP, sizeof(int) );
		else if( Run_Info.Mac_LineStatus == LINE_BROKEN )
			MAC_SetOption( dialer.MacHandle, OPT_CALLSTATE, SCRPAR_CALL_STOP, sizeof(int) );

//		if( DialInfo.SelMode == DIALER_RUNMODE_GPRS )
			MAC_SetOption(dialer.MacHandle, OPT_SWITCH_MODE, 0, 0);

		if( MAC_ScriptExplain( dialer.MacHandle, SCR_START ) == RTN_WOULDBLOCK )
		{
			REPORTBACK( DialInfo.SelMode, DIALER_RUN_HANGUPING );
#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: �Ҷ���..." );
#endif
			return 0;
		}

		Run_Info.run_state = DS_INVALID;
		Dialer_SetError( DIALER_RUN_SCREXP_ERROR );
		REPORTBACK( DialInfo.SelMode, DIALER_RUN_NET_ABNORMAL );

#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: �ű�����������" );
#endif
		return -1;

	/* �ű���������������ʱ���жϣ�ִ�йҶϵ����⴦�� */
	case DS_STOP:
		//DlmNotify(PS_SETGPRS, 3); //֪ͨ�����������ʼ�Ҷ�
//		f_sleep_unregister(sleephandle);
		f_sleep_unregister(GPRS_CONNECT);
		if( MAC_StopExplain() == RTN_WOULDBLOCK )
		{
			Run_Info.stg_track = DT_BEGIN;
			Run_Info.run_state = DS_HANGUP;

			MAC_AbortNetDial();
			REPORTBACK( DialInfo.SelMode, DIALER_RUN_HANGUPING );

#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: �Ҷ���..." );
#endif
			return 0;
		}
		return -1;
	}
	return -1;
}


static int dial_Ctl_StopNetwork( void )
{
	MAC_StopNetwork( dialer.MacHandle );
	SetTimer(dialer.hDialWnd, DIAL_STOPNETWORKTIMER, DIALER_STOPWAIT, NULL);
	return 0;
}


/*---------------------------------------------------------------
 *
 * MAC�����ݴ�����·״̬�仯ʱ����������й�һ���¼�
 *
 * ����lineevent:	LINE_UP		��·��ͨ
 *					LINE_DOWN	��·����
 *
 *--------------------------------------------------------------*/

static int dial_Ctl_MacLineStatus( int lineevent )
{
	switch( lineevent )
	{
	case LINE_UP:
		Run_Info.Mac_LineStatus = LINE_CONNECTED;
		return 0;

	case LINE_DOWN:
		Run_Info.Mac_LineStatus = LINE_BROKEN;
		switch( Run_Info.run_state )
		{
		case DS_PPP:
		case DS_PAP:
		case DS_NET:
			KillTimer( dialer.hDialWnd, DIAL_STOPNETWORKTIMER );

			if( DialInfo.AutoHangupTimeout > 0 )
				KillTimer( dialer.hDialWnd, DIAL_TESTTIMER );

			///MAC_NetworkDown( dialer.MacHandle );
			//MAC_StopNetwork( dialer.MacHandle );

			dial_Ctl_ScrPar_HangupEnd();
			return 0;

		case DS_HANGUP:
			if( Run_Info.stg_track == DT_NOTPASS )
			{
				KillTimer( dialer.hDialWnd, DIAL_HANGUPTIMER );
				dial_Ctl_ScrPar_HangupEnd();
			}
			return 0;
		}
		return 0;

	}
	return 0;
}


/*---------------------------------------------------------------
 *
 * ���Ž׶ε��ýű���������ɺ󣬸��ݽű�����ֵ������Ӧ����
 *
 *--------------------------------------------------------------*/

static int dial_Ctl_ScrPar_DailEnd( void )
{
	if( Run_Info.ScrParse_Ret >= SCR_ERRORCODE )
	{
		Run_Info.run_state = DS_INVALID;
		Run_Info.ScrParse_Ret = DP_INVALID;

		MAC_AbortNetDial();
		MAC_Close( dialer.MacHandle );
#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: MAC_Close() return is %d \r\n", dialer.MacHandle);
#endif

		Dialer_SetError( DIALER_RUN_SCREXP_ERROR );
		REPORTBACK( DialInfo.SelMode, DIALER_RUN_NET_ABNORMAL );

#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: �ű�����������" );
#endif
		return -1;
	}

	Run_Info.Mac_LineStatus = LINE_CONNECTED;//��ʷԭ����ǰ���ж�������Mac����·�����������Ϊִ�нű����Ӻ�Mac����ͨ

	switch( Run_Info.ScrParse_Ret )
	{
	case DP_SUCCESS:
		if( Run_Info.Mac_LineStatus == LINE_BROKEN )
		{
			Run_Info.ScrParse_Ret = DP_INVALID;

			Dialer_SetError( DIALER_RUN_SIGNAL_ERROR );

			/* ����RS232�ź��쳣ʱ�ĹҶ� */
			MAC_AbortNetDial();
			Run_Info.stg_track = DT_BEGIN;
			Run_Info.run_state = DS_HANGUP;

			/* ���нű������� */
			MAC_SetOption( dialer.MacHandle, OPT_CALLSTATE, SCRPAR_CALL_HANGUP, sizeof(int) );
			if( MAC_ScriptExplain( dialer.MacHandle, SCR_START ) == RTN_WOULDBLOCK )
			{
				REPORTBACK( DialInfo.SelMode, DIALER_RUN_HANGUPING );

#ifdef _DIALER_DEBUG_
				MsgOut("\r\nDialer Report: �Ҷ���..." );
#endif
				return -1;
			}

			Run_Info.run_state = DS_INVALID;

			Dialer_SetError( DIALER_RUN_GENERAL_ERROR );
			REPORTBACK( DialInfo.SelMode, DIALER_RUN_NET_ABNORMAL );

#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: �ű�����������" );
#endif
			return -1;
		}

		Run_Info.stg_track = DT_INVALID;

		// Login Continue
		if( DialInfo.SelMode == 0 )
		{
			Run_Info.run_state = DS_LOGIN;
			MAC_SetOption( dialer.MacHandle, OPT_CALLSTATE, SCRPAR_CALL_LOGIN, sizeof(int) );
			if( MAC_ScriptExplain( dialer.MacHandle, SCR_START ) == RTN_WOULDBLOCK )
			{
				REPORTBACK( DialInfo.SelMode, DIALER_RUN_LOGINING );

#ifdef _DIALER_DEBUG_
				MsgOut("\r\nDialer Report: ��¼��..." );
#endif
				return 0;
			}

			Run_Info.run_state = DS_INVALID;

			MAC_AbortNetDial();
			MAC_Close( dialer.MacHandle );
#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: MAC_Close() return is %d \r\n", dialer.MacHandle);
#endif

			Dialer_SetError( DIALER_RUN_SCREXP_ERROR );
			REPORTBACK( DialInfo.SelMode, DIALER_RUN_NET_ABNORMAL );

#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: �ű�����������" );
#endif
			return -1;
		}

		// PPP Negotiate Continue
		else if( DialInfo.SelMode == 1 )
		{
			Run_Info.ScrParse_Ret = DP_SUCCESS;
			dial_Ctl_ScrPar_LoginEnd();
			return 0;
		}
		return 0;

	case DP_BUSY:
		Run_Info.stg_track = DT_NOTPASS;
		if (++Run_Info.rep_times <= DialInfo.RepeatDialTimes)
		{
			/* �趨ռ���ز���ʱ�� */
			SetTimer(dialer.hDialWnd, DIAL_BUSYTIMER, DIAL_BUSYWAIT, NULL);
			REPORTBACK( DialInfo.SelMode, DIALER_RUN_BUSYRETRY );

#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: ռ���ز�..." );
#endif
		}
		/* �������ز� */
		else
		{
			Dialer_SetError( DIALER_RUN_BUSYFAIL );
			dial_Ctl_Operate( DS_HANGUP );
		}
		return 0;

	/* case DP_NO_DIALTONE, DP_MODEM_DUMB,DP_GENERAL_FAILURE: */
	default:
		if ( Run_Info.ScrParse_Ret == DP_MODEM_DUMB )
			Dialer_SetError( DIALER_RUN_MODEMDUMB );

		if ( Run_Info.ScrParse_Ret == DP_NO_DIALTONE )
			Dialer_SetError( DIALER_RUN_NODIALTONE );

		if ( Run_Info.ScrParse_Ret == DP_NO_CARRIER )
			Dialer_SetError( DIALER_RUN_NOCARRIER );

		if ( Run_Info.ScrParse_Ret == DP_GENERAL_FAILURE )
			Dialer_SetError( DIALER_RUN_DIALFAIL );

		Run_Info.stg_track = DT_NOTPASS;
		dial_Ctl_Operate( DS_HANGUP );
		return 0;
	}
}


/*---------------------------------------------------------------
 *
 * ��¼�׶ε��ýű���������ɺ󣬸��ݽű�����ֵ������Ӧ����
 *
 *--------------------------------------------------------------*/

static int dial_Ctl_ScrPar_LoginEnd( void )
{
	if( Run_Info.ScrParse_Ret >= SCR_ERRORCODE )
	{
		Run_Info.run_state = DS_INVALID;
		Run_Info.ScrParse_Ret = DP_INVALID;

		MAC_AbortNetDial();
		MAC_Close( dialer.MacHandle );
#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: MAC_Close() return is %d \r\n", dialer.MacHandle);
#endif

		Dialer_SetError( DIALER_RUN_SCREXP_ERROR );
		REPORTBACK( DialInfo.SelMode, DIALER_RUN_NET_ABNORMAL );

#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: �ű�����������" );
#endif
		return -1;
	}

	switch( Run_Info.ScrParse_Ret )
	{
	case DP_UNSUPPORTED_LOGIN:
		if( Run_Info.Mac_LineStatus == LINE_BROKEN )
		{
			Dialer_SetError( DIALER_RUN_LOGINFAIL );
			dial_Ctl_Operate( DS_HANGUP );
			return 0;
		}
	case DP_SUCCESS:

		Run_Info.run_state = DS_PPP;

		/* �����Զ����߹���,�����Զ����߶�ʱ�� */
		if( DialInfo.AutoHangupTimeout > 0 )
			SetTimer( dialer.hDialWnd, DIAL_TESTTIMER, DIAL_TESTWAIT, NULL);

		REPORTBACK( DialInfo.SelMode, DIALER_RUN_NEGOTIATING );

#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: ����Э����..." );
#endif

		MAC_StartNetwork( dialer.MacHandle );
		return 0;

	case DP_GENERAL_FAILURE:

		Dialer_SetError( DIALER_RUN_LOGINFAIL );
		dial_Ctl_Operate( DS_HANGUP );
		return  0;
	}
	return -1;
}


/*---------------------------------------------------------------
 *
 * ����Э����ɣ�����Э�̽������Ӧ����
 *
 *--------------------------------------------------------------*/

static int dial_Ctl_NegotiateEnd( int NetEvent )
{
	switch( NetEvent )
	{
	case PPP_CLOSED:
		KillTimer( dialer.hDialWnd, DIAL_STOPNETWORKTIMER );

		dial_Ctl_Operate( DS_HANGUP );
		break;

	case PPP_RE_NEGO:
		Run_Info.ReNegFlag = 1;
		REPORTBACK( DialInfo.SelMode, DIALER_RUN_NETPAUSE );

#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: �����ж�״̬" );
#endif
		return 0;

	case PPP_SUCCESS:
		if( Run_Info.ReNegFlag == 1 )
		{
			Run_Info.ReNegFlag = 0;
			REPORTBACK( DialInfo.SelMode, DIALER_RUN_NETRESUME );

#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: ����ָ�״̬" );
#endif
			return 0;
		}

		Run_Info.run_state = DS_NET;
		Sock_DialLineUp();
		REPORTBACK( DialInfo.SelMode, DIALER_RUN_NETCONNECT );

#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: ������ͨ״̬" );
#endif
		return 0;

	case PAP_FAILURE:
		Run_Info.run_state = DS_PAP;
		Dialer_SetError( DIALER_RUN_PAPFAIL );

	case PPP_FAILURE:
		if(	Run_Info.run_state == DS_PPP )
			Dialer_SetError( DIALER_RUN_PPPFAIL );

		dial_Ctl_Operate( DS_HANGUP );

		if( Run_Info.ReNegFlag == 1 )
			Run_Info.ReNegFlag = 0;
		return 0;
	}
	return -1;
}


/*---------------------------------------------------------------
 *
 * �ҶϽ׶ε��ýű���������ɺ�����Ӧ����
 * ע����������ű����ͽ���޹أ���Ҫ�鿴������Ϣ
 *
 *--------------------------------------------------------------*/

static int dial_Ctl_ScrPar_HangupEnd( void )
{
	if( Run_Info.ScrParse_Ret >= SCR_ERRORCODE )
		Run_Info.ScrParse_Ret = DP_INVALID;

	Run_Info.Mac_LineStatus = LINE_BROKEN; //����Ҳ��ֱ����Ϊ�ű�������ִ�йҶϵ�ʱ�� MAC��ͶϿ���

	/* �����·�Ѷ�����˵���Ҷϳɹ� */
	if( Run_Info.Mac_LineStatus == LINE_BROKEN )
	{
		/* Run_Info��Ч */
		Run_Info.stg_track = DT_INVALID;
		Run_Info.run_state = DS_INVALID;
		Run_Info.ReNegFlag = 0;

		MAC_Close( dialer.MacHandle );
#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: MAC_Close() return is %d \r\n", dialer.MacHandle);
#endif

		REPORTBACK( DialInfo.SelMode, DIALER_RUN_NETBROKEN );
#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: �������״̬" );
#endif

#ifdef	SMARTPHONE2
		DHI_ClearNotSleepFlag(DIAL_FLAG);
#endif
		return 0;
	}

	if( Run_Info.Mac_LineStatus == LINE_CONNECTED )
	{
		/* �ҶϺ���·����ͨ���趨�Ҷ϶�ʱ���Ա��ٴμ�� */
		if( Run_Info.stg_track == DT_BEGIN )
		{
			Run_Info.stg_track = DT_NOTPASS;
			SetTimer(dialer.hDialWnd, DIAL_HANGUPTIMER, DIAL_HANGUPWAIT, NULL);
			return 0;
		}
		/* �ײ���·δ����,�Ҷ�ʧ��,���ش��� */
		if( Run_Info.stg_track == DT_NOTPASS )
		{
			Run_Info.run_state = DS_NET;
			Dialer_SetError( DIALER_RUN_HANGUPFAIL );
			REPORTBACK( DialInfo.SelMode, DIALER_RUN_NETCONNECT );

#ifdef _DIALER_DEBUG_
			MsgOut("\r\nDialer Report: ������ͨ״̬" );
#endif
			return 0;
		}
	}
	return 0;
}


/*---------------------------------------------------------------
 *
 * ���ݼ�ⶨʱ���ص�����
 *
 *--------------------------------------------------------------*/

static int dial_Ctl_AutoLineDown( void )
{
	int	dataflag;
	static int timercount = 0;
	/* ���û�������ʱȡ�����Զ��ҶϹ��ܣ���ô�����Զ��Ҷ�*/
	if( DialInfo.AutoHangupTimeout <= 0 )
	{
		timercount = 0;
		KillTimer( dialer.hDialWnd, DIAL_TESTTIMER );
		return 0;
	}

	MAC_GetOption( dialer.MacHandle, OPT_TESTDATA, (void*)&dataflag, sizeof(int));
	/* �����ݴ��䣬��ռ�ʱ�� */
	if( dataflag == DATA_ACTIVE )
	{
		timercount = 0;
		DlmNotify(PS_SETGPRS, 0);//֪ͨ���������������
	}

	else if( dataflag == DATA_SILENCE )
	{
		DlmNotify(PS_SETGPRS, 1);//֪ͨ���������������
		/* �����涨ʱ�䣬�����Զ����� */
		if( ++timercount >= (DialInfo.AutoHangupTimeout*DIAL_TESTPARA) )
		{
			timercount = 0;
			KillTimer( dialer.hDialWnd, DIAL_TESTTIMER );
			dial_Ctl_StopNetwork();
			//dial_Ctl_Operate( DS_HANGUP );
		}
	}
	return 0;
}


/*---------------------------------------------------------------
 *
 * Error Value Operate
 *
 *-------------------------------------------------------------*/

static int Dialer_SetError( int ErrorVal )
{
	if( ErrorVal != DIALER_RUN_HANGUPFAIL && ErrorVal != DIALER_RUN_GENERAL_ERROR )
		Sock_StopDial();

	Run_Info.Dialer_ErrorVal = ErrorVal;
	return 0;
}

extern int Dialer_GetError( void )
{
	int Val = Run_Info.Dialer_ErrorVal;
	Run_Info.Dialer_ErrorVal = DIALER_RUN_NO_ERROR;
	return Val;
}


/*---------------------------------------------------------------
 *
 * ģ����ͨ�ú���,�жϲ������н׶�
 *
 * ����ֵ������ģ�鵱ǰ��״̬
 *				DIALER_INVALIDSTAGE		����״̬
 *				DIALER_CONNECTSTAGE		����״̬
 *				DIALER_NETWORKSTAGE		����״̬
 *				DIALER_PPPSTAGE			PPPЭ��״̬
 *				DIALER_HANGUPSTAGE		�Ҷ�״̬
 *				-1	�쳣
 *
 *--------------------------------------------------------------*/

extern int dial_RunStage( void )
{
#ifdef _DIALER_DEBUG_
	printf("\r\nDialer Report: dial_RunStage() == %d", Run_Info.run_state);
#endif

	if( Run_Info.run_state == DS_INVALID )
	{
#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: dial_RunStage() ==DIALER_INVALIDSTAGE" );
#endif
		return DIALER_INVALIDSTAGE;
	}

	if( Run_Info.run_state == DS_DIAL || Run_Info.run_state == DS_LOGIN )
		return DIALER_CONNECTSTAGE;

	if( Run_Info.run_state == DS_PPP || Run_Info.run_state == DS_PAP )
		return DIALER_PPPSTAGE;

	if( Run_Info.run_state == DS_NET )
		return DIALER_NETWORKSTAGE;

	if( Run_Info.run_state == DS_HANGUP )
	{
#ifdef _DIALER_DEBUG_
		MsgOut("\r\nDialer Report: dial_RunStage() ==DIALER_HANGUPSTAGE" );
#endif
		return DIALER_HANGUPSTAGE;
	}

	return -1;
}



/* End of program */

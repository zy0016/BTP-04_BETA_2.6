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

#ifndef _DIAL_MAIN_H_
#define _DIAL_MAIN_H_

#if (_HOPEN_VER >= 200)
#define M_GlobalAlloc			LocalAlloc
#define M_GlobalFree			LocalFree
#define M_GetSysClock			OS_GetSysClock
#define M_GetTicksPerSecond		OS_GetTicksPerSecond

#else
#define M_GlobalAlloc			OS_GlobalAlloc
#define M_GlobalFree			OS_GlobalFree
#define M_GetSysClock			OS_GetSysClock
#define M_GetTicksPerSecond		OS_GetTicksPerSecond

#endif

/*----------------------���ú궨��----------------------------*/

#define DIAL_DEFCOMNAME			"MUX-SIEMENS-DLC1"		/* ָ����ͨ������	*/


#define DIAL_DATA_SCRIPT		"ROM:scptDATA.txt"
#define DIAL_GPRS_SCRIPT		"ROM:scptGPRS.txt"


#define DIAL_STRINGLEN			30				/* �ִ�����󳤶�	*/

#define		REPORTBACK( mode, message )	\
	{	\
		if( dialer.CallerWnd != NULL )	\
			PostMessage( dialer.CallerWnd, dialer.CallBackMsg, (mode), (message));	\
		if( dialer.HangupWnd != NULL )	\
			PostMessage( dialer.HangupWnd, dialer.HangupMsg, (mode), (message));	\
	}


/*--------------------��ʱ��-----------------------*/


/* �����������ݼ��Ӷ�ʱ�� */
#define DIAL_TESTTIMER			1000		/* ���ݼ�����ID		*/
#define DIAL_TESTWAIT			20000		/* ���ݼ��20��		*/
#define DIAL_TESTPARA			3			/* ��ʱ�����˹�ϵ	*/

/* �����ز��ȴ���ʱ�� */
#define DIAL_BUSYTIMER			2000		/* �ز��ȴ���ʱ��ID	*/
#define DIAL_BUSYWAIT			1000		/* �ز��ȴ�1��		*/

/* ���ŹҶϵȴ���ʱ�� */
#define DIAL_HANGUPTIMER		3000		/* �Ҷ϶�ʱ��ID		*/
#define DIAL_HANGUPWAIT			2000		/* �Ҷϵȴ�2��		*/

// Dialer StopNetwork Timer
#define DIAL_STOPNETWORKTIMER	4000
#define DIALER_STOPWAIT			5000


/*--------------- ����ű����͵������� ----------------*/

#define SCRPAR_CALL_DIAL			1			/* ����ִ�в��� */
#define SCRPAR_CALL_LOGIN			2			/* ����ִ�е�¼ */
#define SCRPAR_CALL_HANGUP			3			/* ����ִ�йҶ� */
#define SCRPAR_CALL_STOP			4			/* ����ֹͣ���� */




/*-------------------����ͨ����Ϣ-------------------*/

/* Dial���ڴ�����Ϣ */
#define WM_DIAL_MACCALLBACK		WM_USER+1
#define WM_DIAL_START			WM_USER+2		/* ��ʼ����			*/
#define WM_DIAL_HANGUP			WM_USER+3		/* ��ʼ�Ҷ�			*/
#define WM_DIAL_RUNCERTAIN		WM_USER+5		/* ��Ӧ������Ϣ		*/
#define WM_DIAL_RUNCANCEL		WM_USER+6		/* ��Ӧȡ������		*/



/*------------------����״̬�����ݽṹ��Ϣ---------------------*/

/* ��ǰģ������״̬ DS_ */
#define DS_INVALID			0			/* ��Ч״̬			*/
#define DS_DIAL				1			/* ����״̬			*/					
#define DS_LOGIN			2			/* ��¼״̬			*/
#define DS_PPP				3			/* PPPЭ��״̬		*/
#define DS_PAP				4			/* PAPЭ��״̬		*/
#define DS_NET				5			/* ��������״̬		*/
#define DS_HANGUP			6			/* �Ҷ�״̬			*/
#define DS_STOP				7			/* ȡ������������	*/

/* ���н׶ν��ȸ��� DT_ */
#define DT_INVALID			0			/* ������		*/
#define DT_BEGIN			1			/* ����			*/
#define DT_PASS				2			/* �ɹ�			*/
#define DT_NOTPASS			3			/* ʧ��			*/

/* ���ͽű�����ֵ DP_ */
#define DP_SUCCESS				0			/* �ɹ���ɽ���		*/
#define DP_GENERAL_FAILURE		1			/* ����δ��ͨ���	*/
#define DP_MODEM_DUMB			2			/* Modem�޷�Ӧ		*/
#define DP_NO_DIALTONE			3			/* �޲�����			*/
#define DP_BUSY					4			/* ռ��				*/
#define DP_NO_CARRIER			5			/* ���ز��ź�		*/
#define DP_UNSUPPORTED_LOGIN	6			/* ��֧�����еĵ�¼��ʽ,����PAPЭ��	*/
#define DP_INVALID				-1

/* MAC����·��� */
#define LINE_BROKEN			0
#define LINE_CONNECTED		1

/* ���ݽṹ */
typedef struct{
	int			run_state;			/* ��ǰģ������״̬		*/
	int			stg_track;			/* ���н׶ν��ȸ�����	*/
	int			rep_times;			/* �ز�����				*/

	int			ScrParse_Ret;		/* �ű����ͷ���ֵ		*/
	int			Mac_LineStatus;		/* MAC����·���		*/
	int			ReNegFlag;			/* ����Э�̱�־			*/

	int			Dialer_ErrorVal;

}RUNINFO, *PRUNINFO;



/*--------------------DIALERBLOCK���ݽṹ��Ϣ----------------------*/

/* ���ŵ��÷�ʽ DM_ */
#define DM_INVILID			0			// �޵���
#define DM_COMMON			1			// һ�㷽ʽ
#define DM_AUTO				2			// �Զ�����
#define DM_VIDEO			3			// ָ����ͨ
#define DM_TCPIP			5			// TCPIP�������ݵ���

/* ���ݽṹ���� */
typedef struct{
	int				CallManner;			// ���ŵ��÷�ʽ
	HWND			hDialWnd;			// ���ſ��ƴ��ھ��

	int				SelectISP;			// ѡ��ISP��ʽ
	HWND			CallerWnd;			// ���ô��ھ��
	int				CallBackMsg;		// ���ô�����Ϣ
	HWND			HangupWnd;			// �Ҷ�֪ͨ����
	int				HangupMsg;			// �Ҷ�֪ͨ��Ϣ

	int				MacHandle;			// MAC����þ��

} DIALERBLOCK;



/*---------------------DIALINFO���ݽṹ��Ϣ------------------------*/

#define DIAL_DEFAUTORETRY	5		// ȱʡ���Զ��ز�����
#define DIAL_DEFAUTOHANGUP	30		// ȱʡ���Զ�����ʱ��

typedef struct{
	// ISP info
	char	ISPName[DIAL_MAXSTRLEN +1];		// ISP����
	char	PhoneNum[DIAL_MAXSTRLEN +1];	// �绰����
	char	UserName[DIAL_MAXSTRLEN +1];	// �û���
	char	PassWord[DIAL_MAXSTRLEN   +1];	// ����
	char	Dns1[DIAL_MAXSTRLEN + 1];		// DNS��Ϣ1
	char	Dns2[DIAL_MAXSTRLEN + 1];		// DNS��Ϣ2

	// system setting
	int		RepeatDialTimes;				// 1 - 10  �ز�����
	int		AutoHangupTimeout;				// 0 - 60m �Զ�����

	int		SelMode;
	int		Valid;							// 0:��Ч 1:��Ч

}DIALINFO, *PDIALINFO;



/*-------------------- ģ����ͨ�ú��� --------------------*/
/* dialctrl.c */
extern int dial_RunStage( void );

/* dialinfo.c */
extern int dial_GetEnvelope( int, void*, PDIALINFO );

#endif /* _DIAL_MAIN_H_ */

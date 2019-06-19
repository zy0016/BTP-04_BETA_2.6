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

#ifndef _DIAL_VIEW_H_
#define _DIAL_VIEW_H_

// dialview.c
char Dialer_Run_Msg_String[][30]={

	"���ܵ�������"		,	//SPECDIAL_REQ_ACCEPT
	"�����Ѵ���"		,	//DIALER_REQ_EXIST
	"�ܾ���������"		,	//DIALER_REQ_REFUSE
	"��ͨ���������ʧ��",	//DIALER_MCH_FAILURE
	"ISP��Ϣ��Ч"		,	//DIALER_INVALID_ISP

	"���ڽ��в���..."	,	//DIALER_RUN_DIALING
	"ռ��,�����ز�..."	,	//DIALER_RUN_BUSYRETRY
	"��֤�û����..."	,	//DIALER_RUN_LOGINING
	"���ڵ�¼����..."	,	//DIALER_RUN_NEGOTIATING
	"���ڹҶ�..."		,	//DIALER_RUN_HANGUPING

	""					,	//DIALER_RUN_NETCONNECT
	""					,	//DIALER_RUN_NETBROKEN
	""					,	//DIALER_RUN_NET_ABNORMAL
	""					,	//DIALER_RUN_NETPAUSE
	""					,	//DIALER_RUN_NETRESUME

	"��������ʧ��"		,	//DIALER_RUN_DIALFAIL
	"���ƽ�����޷�Ӧ"	,	//DIALER_RUN_MODEMDUMB
	"ռ��"				,	//DIALER_RUN_BUSYFAIL
	"�޲�����"			,	//DIALER_RUN_NODIALTONE
	"���ز��ź�"		,	//DIALER_RUN_NOCARRIER

	"�û���¼ʧ��"		,	//DIALER_RUN_LOGINFAIL
	"����Э��ʧ��"		,	//DIALER_RUN_PPPFAIL
	"�û���֤ʧ��"		,	//DIALER_RUN_PAPFAIL
	"�Ҷ�ʧ��"			,	//DIALER_RUN_HANGUPFAIL

	"�ű�����������"	,	//DIALER_RUN_SCREXP_ERROR
	"��·�ź��쳣"		,	//DIALER_RUN_SIGNAL_ERROR
	"ģ�������쳣����"	,	//DIALER_RUN_GENERAL_ERROR

};


#endif /* _DIAL_VIEW_H_ */

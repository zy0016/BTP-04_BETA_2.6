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

	"接受调用请求"		,	//SPECDIAL_REQ_ACCEPT
	"连接已存在"		,	//DIALER_REQ_EXIST
	"拒绝调用请求"		,	//DIALER_REQ_REFUSE
	"多通道句柄申请失败",	//DIALER_MCH_FAILURE
	"ISP信息无效"		,	//DIALER_INVALID_ISP

	"正在进行拨号..."	,	//DIALER_RUN_DIALING
	"占线,进行重拨..."	,	//DIALER_RUN_BUSYRETRY
	"验证用户身份..."	,	//DIALER_RUN_LOGINING
	"正在登录网络..."	,	//DIALER_RUN_NEGOTIATING
	"正在挂断..."		,	//DIALER_RUN_HANGUPING

	""					,	//DIALER_RUN_NETCONNECT
	""					,	//DIALER_RUN_NETBROKEN
	""					,	//DIALER_RUN_NET_ABNORMAL
	""					,	//DIALER_RUN_NETPAUSE
	""					,	//DIALER_RUN_NETRESUME

	"拨号连接失败"		,	//DIALER_RUN_DIALFAIL
	"调制解调器无反应"	,	//DIALER_RUN_MODEMDUMB
	"占线"				,	//DIALER_RUN_BUSYFAIL
	"无拨号音"			,	//DIALER_RUN_NODIALTONE
	"无载波信号"		,	//DIALER_RUN_NOCARRIER

	"用户登录失败"		,	//DIALER_RUN_LOGINFAIL
	"网络协商失败"		,	//DIALER_RUN_PPPFAIL
	"用户验证失败"		,	//DIALER_RUN_PAPFAIL
	"挂断失败"			,	//DIALER_RUN_HANGUPFAIL

	"脚本解释器错误"	,	//DIALER_RUN_SCREXP_ERROR
	"线路信号异常"		,	//DIALER_RUN_SIGNAL_ERROR
	"模块运行异常错误"	,	//DIALER_RUN_GENERAL_ERROR

};


#endif /* _DIAL_VIEW_H_ */

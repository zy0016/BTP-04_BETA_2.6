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


#include "hp_modcfg.h"
#if defined(SIEMENS_TC35)|| defined(TI_MODULE)


#ifndef _DIAL_DIALER_H_
#define _DIAL_DIALER_H_


/*--- int DIALER_SelConnect( HWND, UINT, int SelectType, void *pDialInfo ); ---*/

// SelectType:
#define		DIALDEF_WAP			1
#define		DIALDEF_MAIL		2
#define		DIALDEF_SELFGPRS	3
#define		DIALDEF_SELFDATA	4

#define DIAL_MAXSTRLEN		31		// ISP名字的最大长度,需要对应setting.h

// when SelectType = DIALDEF_SELFGPRS
typedef struct TAGDIALER_GPRSINFO{
	char	APN[DIAL_MAXSTRLEN +1];			// APN
	char	UserID[DIAL_MAXSTRLEN +1];		// 用户ID
	char	PassWord[DIAL_MAXSTRLEN +1];	// 密码

}DIALER_GPRSINFO, *PDIALER_GPRSINFO;

// when SelectType = DIALDEF_SELFDATA
typedef struct TAGDIALER_DATAINFO{
	char	PhoneNum[DIAL_MAXSTRLEN +1];	// 电话号码
	char	UserName[DIAL_MAXSTRLEN +1];	// 用户名
	char	PassWord[DIAL_MAXSTRLEN +1];	// 口令

}DIALER_DATAINFO, *PDIALER_DATAINFO;



/*---------------- DIALER_GetLineState() return value ------------------*/

#define DIALER_INVALIDSTAGE		0		// 空闲状态
#define DIALER_CONNECTSTAGE		1		// 连接状态
#define DIALER_PPPSTAGE			2		// PPP协商
#define DIALER_NETWORKSTAGE		3		// 网络状态
#define DIALER_HANGUPSTAGE		4		// 挂断状态



/*------------ 拨号运行情况通知消息 PostMessage() -------------*/

// 拨号方式 wParam
#define DIALER_RUNMODE_DATA		0		//数据拨号
#define DIALER_RUNMODE_GPRS		1		//GPRS方式

//lParam
enum Dialer_Run_MsgNotice {

// Function Return Value, not message
	DIALER_REQ_ACCEPT =0	,		// 接受调用
	DIALER_REQ_EXIST		,		// 已存在
	DIALER_REQ_REFUSE		,		// 拒绝调用
	DIALER_MCH_FAILURE		,		// 多通道句柄申请失败
	DIALER_INVALID_ISP		,		// 无效ISP

// Normal Stage Message
	DIALER_RUN_DIALING		,		// Dial Stage
	DIALER_RUN_BUSYRETRY	,		// ReDial when Busy
	DIALER_RUN_LOGINING		,		// Login Stage
	DIALER_RUN_NEGOTIATING	,		// Negotiate Stage
	DIALER_RUN_HANGUPING	,		// Hangup Stage

// Result Message
	DIALER_RUN_NETCONNECT	,
	DIALER_RUN_NETBROKEN	,
	DIALER_RUN_NET_ABNORMAL	,
	DIALER_RUN_NETPAUSE		,
	DIALER_RUN_NETRESUME	,

// Error Code, Use Function:Dialer_GetError()
	DIALER_RUN_DIALFAIL		,		// 拨号失败
	DIALER_RUN_MODEMDUMB	,		// 调制解调器无反应
	DIALER_RUN_BUSYFAIL		,		// 占线
	DIALER_RUN_NODIALTONE	,		// 无拨号音
	DIALER_RUN_NOCARRIER	,		// 无载波信号

	DIALER_RUN_LOGINFAIL	,		// 登录失败
	DIALER_RUN_PPPFAIL		,		// PPP协商失败
	DIALER_RUN_PAPFAIL		,		// PAP协商失败
	DIALER_RUN_HANGUPFAIL	,		// 线路挂断失败

	DIALER_RUN_SCREXP_ERROR	,		// 脚本解释器问题
	DIALER_RUN_SIGNAL_ERROR	,		// 线路信号异常
	DIALER_RUN_GENERAL_ERROR,		// 模块运行异常错误

	DIALER_RUN_NO_ERROR,

};


/*--------------- 接口函数声明 ------------------------*/

int DIALER_Initialize( void );
int DIALER_SelConnect( HWND hWnd, UINT msg, int SelectType, void *pDialInfo );
int DIALER_HangUp( void );
int DIALER_DialCancel( void );
int DIALER_GetLineState( void );
int Dialer_GetError( void );


int DIALER_NotifyRegister( HWND hWnd, int msg );
int DIALER_NotifyUNRegister( HWND hWnd, int msg );
int DIALER_CompareInfo( int SelectType, void *pSrcInfo );


#endif /* _DIAL_DIALER_H_ */


#elif defined WAVECOM_2C

#ifndef _DIAL_DIALER_H_
#define _DIAL_DIALER_H_


/*--- int DIALER_SelConnect( HWND, UINT, int SelectType, void *pDialInfo ); ---*/

// SelectType:
#define		DIALDEF_WAP			1
#define		DIALDEF_MAIL		2
#define		DIALDEF_SELFGPRS	3
#define		DIALDEF_SELFDATA	4

#define DIAL_MAXSTRLEN		31		// ISP名字的最大长度,需要对应setting.h

// when SelectType = DIALDEF_SELFGPRS
typedef struct TAGDIALER_GPRSINFO{
	char	APN[DIAL_MAXSTRLEN +1];			// APN
	char	UserID[DIAL_MAXSTRLEN +1];		// 用户ID
	char	PassWord[DIAL_MAXSTRLEN +1];	// 密码

}DIALER_GPRSINFO, *PDIALER_GPRSINFO;

// when SelectType = DIALDEF_SELFDATA
typedef struct TAGDIALER_DATAINFO{
	char	PhoneNum[DIAL_MAXSTRLEN +1];	// 电话号码
	char	UserName[DIAL_MAXSTRLEN +1];	// 用户名
	char	PassWord[DIAL_MAXSTRLEN +1];	// 口令

}DIALER_DATAINFO, *PDIALER_DATAINFO;



/*---------------- DIALER_GetLineState() return value ------------------*/

#define DIALER_INVALIDSTAGE		0		// 空闲状态
#define DIALER_CONNECTSTAGE		1		// 连接状态
#define DIALER_PPPSTAGE			2		// PPP协商
#define DIALER_NETWORKSTAGE		3		// 网络状态
#define DIALER_HANGUPSTAGE		4		// 挂断状态



/*------------ 拨号运行情况通知消息 PostMessage() -------------*/

// 拨号方式 wParam
#define DIALER_RUNMODE_DATA		0		//数据拨号
#define DIALER_RUNMODE_GPRS		1		//GPRS方式

//lParam
enum Dialer_Run_MsgNotice {

// Function Return Value, not message
	DIALER_REQ_ACCEPT =0	,		// 接受调用
	DIALER_REQ_EXIST		,		// 已存在
	DIALER_REQ_REFUSE		,		// 拒绝调用
	DIALER_MCH_FAILURE		,		// 多通道句柄申请失败
	DIALER_INVALID_ISP		,		// 无效ISP

// Normal Stage Message
	DIALER_RUN_DIALING		,		// Dial Stage
	DIALER_RUN_BUSYRETRY	,		// ReDial when Busy
	//DIALER_RUN_LOGINING	,		 Login Stage
	DIALER_RUN_NEGOTIATING	,		// Negotiate Stage
	DIALER_RUN_HANGUPING	,		// Hangup Stage
	DIALER_RUN_CANCELING	,		// Try to Stop Dialing

// Result Message
	DIALER_RUN_NETCONNECT	,
	DIALER_RUN_NETBROKEN	,
	DIALER_RUN_NET_ABNORMAL	,
	DIALER_RUN_NETPAUSE		,
	DIALER_RUN_NETRESUME	,

// Error Code, Use Function:Dialer_GetError()
	DIALER_RUN_DIALFAIL		,		// 拨号失败
	DIALER_RUN_MODEMDUMB	,		// 调制解调器无反应
	DIALER_RUN_BUSYFAIL		,		// 占线
	DIALER_RUN_NODIALTONE	,		// 无拨号音
	DIALER_RUN_NOCARRIER	,		// 无载波信号

	//DIALER_RUN_LOGINFAIL	,		 登录失败
	DIALER_RUN_PPPFAIL		,		// PPP协商失败
	DIALER_RUN_PAPFAIL		,		// PAP协商失败
	DIALER_RUN_HANGUPFAIL	,		// 线路挂断失败

	DIALER_RUN_USERCANCEL	,		// 用户取消操作
	DIALER_RUN_SCREXP_ERROR	,		// 脚本解释器问题
	DIALER_RUN_SIGNAL_ERROR	,		// 线路信号异常
	DIALER_RUN_GENERAL_ERROR,		// 模块运行异常错误

	DIALER_RUN_NO_ERROR,

};


/*--------------- 接口函数声明 ------------------------*/

int DIALER_Initialize( void );
int DIALER_SelConnect( HWND hWnd, UINT msg, int SelectType, void *pDialInfo );
int DIALER_HangUp( void );
int DIALER_DialCancel( void );
int DIALER_GetLineState( void );
int Dialer_GetError( void );


int DIALER_NotifyRegister( HWND hWnd, int msg );
int DIALER_NotifyUNRegister( HWND hWnd, int msg );
int DIALER_CompareInfo( int SelectType, void *pSrcInfo );

/* For ColorMMS
int MMS_DialerStart( HWND hWnd, UINT msg,  int SelectType, void *pSrcInfo );
int MMS_DialerRecvMessage( HWND hWnd, UINT dialMsg, WPARAM wParam, LPARAM lParam );
*/


#endif /* _DIAL_DIALER_H_ */


#endif

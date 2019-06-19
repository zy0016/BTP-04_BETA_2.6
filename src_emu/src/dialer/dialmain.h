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

/*----------------------公用宏定义----------------------------*/

#define DIAL_DEFCOMNAME			"MUX-SIEMENS-DLC1"		/* 指定多通道名称	*/


#define DIAL_DATA_SCRIPT		"ROM:scptDATA.txt"
#define DIAL_GPRS_SCRIPT		"ROM:scptGPRS.txt"


#define DIAL_STRINGLEN			30				/* 字串的最大长度	*/

#define		REPORTBACK( mode, message )	\
	{	\
		if( dialer.CallerWnd != NULL )	\
			PostMessage( dialer.CallerWnd, dialer.CallBackMsg, (mode), (message));	\
		if( dialer.HangupWnd != NULL )	\
			PostMessage( dialer.HangupWnd, dialer.HangupMsg, (mode), (message));	\
	}


/*--------------------定时器-----------------------*/


/* 拨号上网数据监视定时器 */
#define DIAL_TESTTIMER			1000		/* 数据监视器ID		*/
#define DIAL_TESTWAIT			20000		/* 数据检测20秒		*/
#define DIAL_TESTPARA			3			/* 定时器倍乘关系	*/

/* 拨号重拨等待定时器 */
#define DIAL_BUSYTIMER			2000		/* 重拨等待定时器ID	*/
#define DIAL_BUSYWAIT			1000		/* 重拨等待1秒		*/

/* 拨号挂断等待定时器 */
#define DIAL_HANGUPTIMER		3000		/* 挂断定时器ID		*/
#define DIAL_HANGUPWAIT			2000		/* 挂断等待2秒		*/

// Dialer StopNetwork Timer
#define DIAL_STOPNETWORKTIMER	4000
#define DIALER_STOPWAIT			5000


/*--------------- 定义脚本解释调用类型 ----------------*/

#define SCRPAR_CALL_DIAL			1			/* 解释执行拨号 */
#define SCRPAR_CALL_LOGIN			2			/* 解释执行登录 */
#define SCRPAR_CALL_HANGUP			3			/* 解释执行挂断 */
#define SCRPAR_CALL_STOP			4			/* 解释停止拨号 */




/*-------------------窗口通用消息-------------------*/

/* Dial窗口处理消息 */
#define WM_DIAL_MACCALLBACK		WM_USER+1
#define WM_DIAL_START			WM_USER+2		/* 开始拨号			*/
#define WM_DIAL_HANGUP			WM_USER+3		/* 开始挂断			*/
#define WM_DIAL_RUNCERTAIN		WM_USER+5		/* 响应错误信息		*/
#define WM_DIAL_RUNCANCEL		WM_USER+6		/* 响应取消操作		*/



/*------------------拨号状态集数据结构信息---------------------*/

/* 当前模块运行状态 DS_ */
#define DS_INVALID			0			/* 无效状态			*/
#define DS_DIAL				1			/* 拨号状态			*/					
#define DS_LOGIN			2			/* 登录状态			*/
#define DS_PPP				3			/* PPP协商状态		*/
#define DS_PAP				4			/* PAP协商状态		*/
#define DS_NET				5			/* 网络运行状态		*/
#define DS_HANGUP			6			/* 挂断状态			*/
#define DS_STOP				7			/* 取消解释器运行	*/

/* 进行阶段进度跟踪 DT_ */
#define DT_INVALID			0			/* 无意义		*/
#define DT_BEGIN			1			/* 启动			*/
#define DT_PASS				2			/* 成功			*/
#define DT_NOTPASS			3			/* 失败			*/

/* 解释脚本返回值 DP_ */
#define DP_SUCCESS				0			/* 成功完成解释		*/
#define DP_GENERAL_FAILURE		1			/* 其他未连通情况	*/
#define DP_MODEM_DUMB			2			/* Modem无反应		*/
#define DP_NO_DIALTONE			3			/* 无拨号音			*/
#define DP_BUSY					4			/* 占线				*/
#define DP_NO_CARRIER			5			/* 无载波信号		*/
#define DP_UNSUPPORTED_LOGIN	6			/* 不支持现有的登录方式,进行PAP协商	*/
#define DP_INVALID				-1

/* MAC层链路情况 */
#define LINE_BROKEN			0
#define LINE_CONNECTED		1

/* 数据结构 */
typedef struct{
	int			run_state;			/* 当前模块运行状态		*/
	int			stg_track;			/* 进行阶段进度跟踪器	*/
	int			rep_times;			/* 重拨次数				*/

	int			ScrParse_Ret;		/* 脚本解释返回值		*/
	int			Mac_LineStatus;		/* MAC层链路情况		*/
	int			ReNegFlag;			/* 重新协商标志			*/

	int			Dialer_ErrorVal;

}RUNINFO, *PRUNINFO;



/*--------------------DIALERBLOCK数据结构信息----------------------*/

/* 拨号调用方式 DM_ */
#define DM_INVILID			0			// 无调用
#define DM_COMMON			1			// 一般方式
#define DM_AUTO				2			// 自动调用
#define DM_VIDEO			3			// 指定连通
#define DM_TCPIP			5			// TCPIP传输数据调用

/* 数据结构描述 */
typedef struct{
	int				CallManner;			// 拨号调用方式
	HWND			hDialWnd;			// 拨号控制窗口句柄

	int				SelectISP;			// 选择ISP方式
	HWND			CallerWnd;			// 调用窗口句柄
	int				CallBackMsg;		// 调用窗口消息
	HWND			HangupWnd;			// 挂断通知窗口
	int				HangupMsg;			// 挂断通知消息

	int				MacHandle;			// MAC层调用句柄

} DIALERBLOCK;



/*---------------------DIALINFO数据结构信息------------------------*/

#define DIAL_DEFAUTORETRY	5		// 缺省的自动重拨次数
#define DIAL_DEFAUTOHANGUP	30		// 缺省的自动断线时间

typedef struct{
	// ISP info
	char	ISPName[DIAL_MAXSTRLEN +1];		// ISP名字
	char	PhoneNum[DIAL_MAXSTRLEN +1];	// 电话号码
	char	UserName[DIAL_MAXSTRLEN +1];	// 用户名
	char	PassWord[DIAL_MAXSTRLEN   +1];	// 口令
	char	Dns1[DIAL_MAXSTRLEN + 1];		// DNS信息1
	char	Dns2[DIAL_MAXSTRLEN + 1];		// DNS信息2

	// system setting
	int		RepeatDialTimes;				// 1 - 10  重拨次数
	int		AutoHangupTimeout;				// 0 - 60m 自动断线

	int		SelMode;
	int		Valid;							// 0:无效 1:有效

}DIALINFO, *PDIALINFO;



/*-------------------- 模块内通用函数 --------------------*/
/* dialctrl.c */
extern int dial_RunStage( void );

/* dialinfo.c */
extern int dial_GetEnvelope( int, void*, PDIALINFO );

#endif /* _DIAL_MAIN_H_ */

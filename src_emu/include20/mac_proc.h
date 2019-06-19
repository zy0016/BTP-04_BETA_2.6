/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _MAC_PROC_H_
#define _MAC_PROC_H_

#include "DI_Uart.h"
#include "hp_kern.h"

/*************************************************************************\
*
*	常量定义
*
\*************************************************************************/

/* 定义消息类型 */
#define EXPLAIN_END			1		/* 脚本解释完毕		*/
#define NDIS_ADAPTER		2		/* NDIS设备请求		*/
#define PPP_NEGOTIATE		3		/* PPP协商结果		*/
#define LINE_EVENT			4		/* 线路事件			*/
#define MAC_CLOSED			5		/* MAC设备成功关闭  */
#define SCR_CLOSED			(MAC_CLOSED)

/* 定义NDIS设备请求 */
#define ADPTOR_OPEN			6		/* NDIS OpenAdpter	*/
#define ADPTOR_CLOSE		7		/* NDIS CloseAdpter	*/

/* 定义PPP协商结果 */
#define PPP_SUCCESS			8		/* PPP协商成功		*/
#define PPP_FAILURE			9		/* PPP协商失败		*/
#define PAP_FAILURE			10		/* PAP协商失败		*/
#define PPP_RE_NEGO			11		/* PPP重新协商		*/
#define PPP_CLOSED			12

/* 定义线路事件 */
#define LINE_UP				13		/* 线路连通			*/
#define LINE_DOWN			14		/* 线路断连			*/
#define LINE_DATAIN			15		/* 有数据到来		*/
#define LINE_DATAOUT		16		/* 可以发送数据		*/
#define LINE_STATE			17		/* 线路状态			*/
#define LINE_POLLING		18		/* 轮询检测线路		*/
#define LINE_RING			19		/* 振铃信号			*/

/* 定义脚本解释目的 */
#define SCR_START			1		/* 开始脚本解释		*/
#define SCR_STOP			2		/* 终止脚本解释		*/

/* 定义数据往来与否 */
#define DATA_ACTIVE			1		/* 有数据传输		*/
#define DATA_SILENCE		0		/* 无数据传输		*/

/* 定义选项 */
#define OPT_ISPNAME			2		/* ISP 名字	*/
#define OPT_PHONENUM		3		/* 电话号码	*/
#define OPT_USERNAME		4		/* 用户帐号 */
#define OPT_PASSWORD		5		/* 用户口令 */
#define OPT_TRANSRATE		6		/* 传输速率 */
#define OPT_CALLSTATE		7		/* 脚本调用状态 */
#define OPT_DIALMODE		8		/* 脚本拨号方式 */
#define OPT_DNS1			9		/* 主域名服务器地址	*/
#define OPT_DNS2			10		/* 辅域名服务器地址	*/

#ifndef _TCPIP_NOWIN_
#define OPT_CALLWND			11		/* 消息反馈窗口 */
#define OPT_CALLMSG			12		/* 反馈窗口消息 */
#else
#define OPT_CALLBACK		11		/* 回调函数     */
#endif

#define OPT_TESTDATA		13		/* 检测数据往来 */
#define OPT_SCRIPTNAME		14		/* 脚本文件名称 */
#define OPT_SELDIALTONE		15		/* 检查拨号音   */
#define OPT_PHONEBOOK		16		/* 电话本数据   */
#define OPT_CONTEXT			17		/* SMS内容信息  */
#define OPT_INTEGERVALUE	18		/* 整型数据		*/
#define OPT_STRINGVALUE		19		/* 字串数据		*/
#define OPT_COMCONFIG		20		/* 串口配置		*/
#define OPT_SWITCH_MODE		21		/* 将数据模式转换为AT模式 */


/* 定义返回值 */
#define	RTN_SUCCESS				0		/* 成功 */
#define RTN_FAILURE				(-1)	/* 失败 */
#define ERR_MAC_OCCUPIED		(-2)	/* 没有空闲MAC */
#define ERR_INVALID_COM_PORT	(-3)	/* 串口无效 */
#define ERR_INVALID_NDIS_MAC	(-4)	/* NDIS无效 */
#define ERR_INVALID_LINE		(-6)	/* 线路无效 */
#define RTN_WOULDBLOCK			(-7)	/* 阻塞		*/
#define ERR_LINE_CONNECTED		(-8)	/* 线路仍然连通 */

#define MAC_MAXSTRINGLEN		52		/* 字符串的最大长度	*/
#define MAX_QUEUE_LEN			30		/* 事件队列长度 */
#define MAX_DEVICE_NUM			1		/* 定义设备数量 */

#define SCR_USERSTOP			1000	/* 用户终止脚本返回值 */
#define SCR_ERRORCODE			SCR_USERSTOP
#define SCR_INVALID_VERSION		(SCR_USERSTOP + 1)		/* 版本信息错误 */

/* 定义唤醒原因代码 */
#define SCRIPT_TIMEOUT			1		/* 脚本解释超时	*/
#define SCRIPT_DATAIN			2		/* 脚本数据到来 */
#define SCRIPT_DATAOUT			3		/* 脚本数据可写 */
#define SCRIPT_STOP				4		/* 脚本解释终止 */
#define SCRIPT_START			5		/* 脚本解释启动 */

/* 定义GSM AT Script Explain支持的参数常量 */
#define CONTEXT_LEN				360
#define INT_VAR_NUM				5
#define STR_VAR_NUM				5

/* 定义选项组合码 */
#define MAKEOPTION(opt, id)		((id << 16) | opt & 0xffff)

/*************************************************************************\
*
*	数据结构及数据类型定义
*
\*************************************************************************/

/* 定义通用请求原语入口类型 */
typedef int (*COM_ENTRY)(int, int, int, int, long);

/* 定义文本数据结构 */
typedef struct _Textent
{
	int LineLength;
	int LineNumber;
	char** TextPaper;
}	TEXTENT;

/* 定义系统信息结构 */
typedef struct _SysInfo
{	
	int			MAC_Task;			/* MAC异步处理线程  */
	int			SexpTask;			/* 脚本解释线程ID号	*/
	OSHANDLE	MAC_Event;			/* MAC等待事件		*/
	OSHANDLE	SexpEvent;			/* 脚本解释事件		*/
	
#if (_HOPEN_VER >= 200)
	MUTEX*		MutexLock;			/* MAC互斥锁		*/
	MUTEX		tempMutexLock;
#else
	OSHANDLE	MutexLock;			/* MAC互斥锁		*/
#endif
	COM_ENTRY	Request;			/* 通用请求原语入口 */

}	SYSINFO;

/* 定义事件队列结构 */
typedef struct EventQueue
{
	char	item[MAX_QUEUE_LEN];	/* 事件队列 */
	int		head;					/* 队列头部 */
	int		tail;					/* 队列尾部 */

}	EVENTQUEUE;

/* 定义拨号信息结构 */
typedef struct _MacInfo
{
#ifndef _TCPIP_NOWIN_
	HWND	call_wnd;		/* 消息回调窗口 */
	int		call_msg;		/* 窗口回调消息 */
#else
	int		(*callback)(short, long);
#endif
	char	state;			/* 状态			*/
	char	linestate;		/* 连接状态		*/
	int		dev_handle;		/* 串口句柄		*/
	int		retval;			/* 脚本返回值	*/
	char	callstate;		/* 调用类型		*/
	char	bTone;			/* 音频拨号		*/
	char	CheckTone;		/* 是否检测拨号音 */
	char	devicename	[MAC_MAXSTRINGLEN + 1];	/* 串口设备名称 */	
	char	scriptname	[MAC_MAXSTRINGLEN + 1];	/* 脚本文件名称 */
	char	ISPname		[MAC_MAXSTRINGLEN + 1];	/* ISP 名称		*/
	char	phonenum	[MAC_MAXSTRINGLEN + 1];	/* 电话号码		*/
	char	username	[MAC_MAXSTRINGLEN + 1];	/* 用户名		*/
	char	password	[MAC_MAXSTRINGLEN + 1];	/* 用户口令		*/
	char	tranrate	[MAC_MAXSTRINGLEN + 1];	/* 传输速率		*/
	char	dns1		[MAC_MAXSTRINGLEN + 1];	/* 主域名服务器	*/
	char	dns2		[MAC_MAXSTRINGLEN + 1];	/* 辅域名服务器	*/

}	MACINFO;

/* 定义脚本解释器的数据交换区 */
typedef struct _DataInfo
{
	char* PhoneBook;
	int	  LineLength;
	int   LineNumber;
	char  Context  [CONTEXT_LEN + 1];
	short IntVarArr[INT_VAR_NUM];
	char  StrVarArr[STR_VAR_NUM][MAC_MAXSTRINGLEN + 1];
}	DATAINFO;

/* 定义MAC控制结构 */
typedef struct _MacCtrlBlock
{
#ifndef _TCPIP_NOWIN_
	HWND		DialWnd;		/* 拨号应用窗口句柄 */
	int			DialMsg;		/* 拨号应用窗口消息 */
#else
	int			(*callback)(short, long);
#endif
	int			OpenAdaptorTag;	/* NDIS OpenAdpter标记 */
	SYSINFO		SysInfo;		/* 系统信息 */
	MACINFO		MacInfo[MAX_DEVICE_NUM];	/* MAC对象 */
	DATAINFO	DataInfo;
	EVENTQUEUE	UartQueue;		/* 中断事件队列 */
	EVENTQUEUE	MesgQueue;		/* 异步事件队列 */
		
}	MACCTRLBLOCK;


/*************************************************************************\
*
*	函数声明
*
\*************************************************************************/

/* 系统接口函数 */
int MAC_Initial	(void);					/* for system task : socklib.c */
int MAC_Destroy	(void);					/* for system task */
#ifndef _TCPIP_NOWIN_
int MAC_Instance(HWND, int);			/* for dial application */
#else
int MAC_Instance(int (*)(short, long));	/* for dial application */
#endif
int MAC_AbortNetDial(void);				/* for dial application */

/* 外部接口函数 */
int MAC_Open		(char*);
int MAC_Open2		(char*, int);
int MAC_Close		(int);
int MAC_SetOption	(int, unsigned, int, int);
int MAC_GetOption	(int, unsigned, char*, int);
int MAC_ScriptExplain(int, int);
int MAC_StopExplain	(void);
int MAC_StartNetwork(int);
int MAC_NetworkDown	(int);
int MAC_StopNetwork	(int);
int MAC_Read		(int, char*, int);
int MAC_Write		(int, char*, int);

/* 脚本解释器对外接口 */
int SCR_Open(int devhandle);
int SCR_Close(int handle);
int SCR_ScriptExplain(int handle, int call);
int SCR_SetOption(int handle, unsigned OptionID, int value, int len);
int SCR_GetOption(int handle, unsigned OptionID, char* buf, int buflen);
int SCR_ScriptDatain(int handle);
int SCR_ScriptDataout(int handle);

/* 内部接口函数 */
int MAC_GetDns		(char*, char*, int);	/* socklib.c */
int MAC_AppendEvent	(int);

/************* End Of Head File ******************************************/
#endif	/* _MAC_PROC_H_ */

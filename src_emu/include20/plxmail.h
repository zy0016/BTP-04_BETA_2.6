/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Pollex E-mail System Program
 *
 * Purpose  : implement e-mail management    
 *            
\**************************************************************************/

#ifndef _PLXMAIL_H_
#define _PLXMAIL_H_

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************\
*
*	常量定义
*
\*************************************************************************/

/* 定义信件状态 */
#define MS_FREE				0		/* 无效状态 */
#define MS_OPEN				1		/* 打开状态，该信件存在于本地及服务器 */
#define MS_OPENLOCAL		2		/* 打开状态，该信件存在于本地		  */
#define MS_OPENERROR		3		/* 打开状态，该信件发送失败			  */
#define MS_READY			4		/* 就位状态，该信件存在于本地及服务器 */
#define MS_REMOTE			5		/* 远地状态，该信件存在于服务器		  */
#define MS_LOCAL			6		/* 本地状态，该信件存在于本地		  */
#define MS_RECVING			7		/* 正在接收状态 */
#define MS_DELEING			8		/* 正在删除状态 */
#define MS_SENDING			9		/* 正在发送状态 */
#define MS_SENDERROR		10		/* 发送出错状态 */

/* 定义消息结果和函数返回 */
#define MAIL_SUCCESS		0		/* 成功 (同步) */
#define MAIL_FAILURE		1		/* 失败 (同步) */
#define MAIL_ERROR			2		/* 错误		   */
#define MAIL_BLOCKED		3		/* 阻塞 (异步) */	
#define MAIL_BROKEN			4		/* 断连 */
#define MAIL_TIMEOUT		5		/* 操作超时 */
#define MAIL_EXIST          6       /* 邮件列表信息已经由上层应用保存 */
    
/* 定义MAIL消息 */
#define MSG_MAIL_LIST		1		/* 邮件列表消息 */
#define MSG_MAIL_DELE		2		/* 删除邮件消息 */
#define MSG_MAIL_RECV		3		/* 接收邮件消息 */
#define MSG_MAIL_SEND		4		/* 发送邮件消息 */
#define MSG_MAIL_GETHOST	5		/* 域名查询消息 */
#define MSG_MAIL_CONNECT	6		/* SOCKET连接消息 */
#define MSG_MAIL_QUIT		7		/* 结束消息 */
#define MSG_MAIL_BROKEN		8		/* SOCKET断连消息 */
#define MSG_MAIL_SENDDATA	9		/* 发送数据消息 */
#define MSG_MAIL_RECVDATA	10		/* 接收数据消息 */
#define MSG_MAIL_LOGIN		11		/* 邮箱登录消息 */
#define MSG_MAIL_LISTINFO	12		/* 接收列表信息进度消息 */
#define MSG_MAIL_REFRESH	13		/* 重新刷新列表 */
#define MSG_MAIL_UPDATE		14		/* 更新邮件列表消息 */

/* 定义邮件箱的状态 */
#define BS_INVALID			0		/* 无效状态 */
#define BS_INITIAL			1		/* 初始状态 */
#define BS_LOGIN			2		/* 登录状态 */
#define BS_LIST1ST			3		/* 第一次取邮件列表 */
#define BS_LISTERR			4		/* 第一次取邮件列表失败 */
#define BS_READY			5		/* 服务器就绪 */
#define BS_LIST				6		/* 正在刷新邮件列表 */
#define BS_RETRIEVE			7		/* 正在取邮件 */
#define BS_DELETE			8		/* 正在删邮件 */
#define BS_SENDMAIL			9		/* 正在发邮件 */
#define BS_QUIT				10		/* 正在退出   */

/* 定义信箱出错代码 */
#define ERR_CODE_NULL		0		/* 没有错误 */

/* 定义信息字符串长度 */
#define TO_LEN				64
#define FROM_LEN			64	
#define DATE_LEN			40
#define SUBJ_LEN			30
#define MID_LEN				40
#define UID_LEN				40


/* 定义邮件列表的长度 */
#ifdef _PRO_BENEFON_
#define MAILLIST_LEN		(1024*1)
#else
#define MAILLIST_LEN		30
#endif

/* 定义RFC822信件头域名代码 */
#define FIELD_FROM			0		/* 发件人 */
#define FIELD_TO			1		/* 收件人 */
#define FIELD_CC			2		/* 抄送人 */
#define FIELD_MESSAGEID		3		/* 邮件ID */
#define FIELD_DATE			4		/* 邮件日期 */
#define FIELD_SUBJECT		5		/* 邮件主题 */
#define FIELD_REPLYTO		6		/* 回复地址 */

/*************************************************************************\
*
*	数据结构及数据类型定义
*
\*************************************************************************/

/* 定义邮件列表 */
typedef struct
{
	int		state;					/* 邮件的状态 */
	int		id;						/* 标识号 */
	int		seq;					/* 邮件的序号 */
	unsigned long	size;			/* 邮件的大小 */
	char	to     [TO_LEN + 1];	/* 收件人信息 */
	char	from   [FROM_LEN + 1];	/* 发送人信息 */
	char	subject[SUBJ_LEN + 1];	/* 邮件的标题 */
	char	date   [DATE_LEN + 1];	/* 邮件的时间 */
	char	uidl   [UID_LEN + 1];	/* 唯一性标识1 */
	char	msgid  [MID_LEN + 1];	/* 唯一性标识2 */
}	MAILLIST;

/* 定义帐号类数据结构 */
typedef struct
{
	char* username;					/* 用户名称 */
	char* password;					/* 用户密码 */
	char* svr_ip;					/* 服务地址 */
	unsigned short svr_port;		/* 服务端口 */
	char* retaddr;					/* 回信地址 */
}	ACCOUNT;

/* 定义附件表 */
typedef struct 
{
	char*	name;					/* 附件的名字 */
	char*	charset;				/* 附件的字符集 */
	char*	type;					/* 附件的类型 */
	char*	filename;				/* 附件的文件名 */
}	ATTACHMENT;

/*************************************************************************\
*
*	函数声明
*
\*************************************************************************/

/* 模块初始化函数 */
int		MAIL_Initial(void);

/* 定义基类函数 */
int		MAIL_GetState(int handle);
int		MAIL_GetError(int handle);
int		MAIL_GetMailCount(int handle);
int		MAIL_GetMailState(int handle, int id);
int		MAIL_GetMailListItem(int handle, int index, MAILLIST* list);
int		MAIL_RemoveMail(int handle, int id);
int		MAIL_CancelAsync(int handle);

/* 收件箱类函数 */
int		MAIL_CreateInbox(HWND hwnd, int msg, ACCOUNT* account, char* protocol);
int		MAIL_DestroyInbox(int handle);
int     MAIL_ImportMailList(int handle,char* uidl);
int		MAIL_CheckMailList(int handle, int start, int num);
int		MAIL_StopCheckMailList(int handle);
int		MAIL_RetrieveMail(int handle, int id, char* option);
int		MAIL_DeleteMail(int handle, int id);
int		MAIL_QuitInboxSession(int handle);
int		MAIL_RemoteMailNum(int handle);

/* 发件箱类函数 */
int		MAIL_CreateOutQueue(HWND hwnd, int msg, ACCOUNT* account, char* protocol);
int		MAIL_DestroyOutQueue(int handle);
int		MAIL_SendMail(int handle);
int		MAIL_StopSend(int handle);
int		MAIL_AddMailToOutQueue(int handle, int fh, char* rcpts);
int		MAIL_QuitOutQueueSession(int handle);

/* 邮件夹类函数 */
int		MAIL_CreateFolder(char* path);
int		MAIL_DestroyFolder(int handle);
int		MAIL_AddMailToFolder(int handle, int fh);
int		MAIL_ImportFolder(int handle, int hDestBox, int id);

/* 信件类函数 */
int		MAIL_CreateMailObject(int boxhandle, int id);
int		MAIL_DestroyMailObject(int handle);
char*	MAIL_GetMailHeader(int handle, int Field);
int		MAIL_GetMailTextLength(int handle);
int		MAIL_GetMailText(int handle, int offset, char* buf, int buflen);
char*	MAIL_GetMailTextInfo(int handle, char* key, void* param);
int		MAIL_GetAttachmentCount(int handle);
int		MAIL_GetAttachmentName(int handle, int index, char* buf, int buflen);
int		MAIL_GetAttachmentType(int handle, int index, char* buf, int buflen);
int		MAIL_GetAttachmentFileName(int handle, int index, char* buf, int buflen);
int		MAIL_GetAttachmentInfo(int handle, int i, void* buf, int buflen);

#ifdef __cplusplus
} //extern "C" {
#endif

/************* End Of Head File ******************************************/
#endif	/* _PLXMAIL_H_ */

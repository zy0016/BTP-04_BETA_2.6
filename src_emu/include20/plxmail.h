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
*	��������
*
\*************************************************************************/

/* �����ż�״̬ */
#define MS_FREE				0		/* ��Ч״̬ */
#define MS_OPEN				1		/* ��״̬�����ż������ڱ��ؼ������� */
#define MS_OPENLOCAL		2		/* ��״̬�����ż������ڱ���		  */
#define MS_OPENERROR		3		/* ��״̬�����ż�����ʧ��			  */
#define MS_READY			4		/* ��λ״̬�����ż������ڱ��ؼ������� */
#define MS_REMOTE			5		/* Զ��״̬�����ż������ڷ�����		  */
#define MS_LOCAL			6		/* ����״̬�����ż������ڱ���		  */
#define MS_RECVING			7		/* ���ڽ���״̬ */
#define MS_DELEING			8		/* ����ɾ��״̬ */
#define MS_SENDING			9		/* ���ڷ���״̬ */
#define MS_SENDERROR		10		/* ���ͳ���״̬ */

/* ������Ϣ����ͺ������� */
#define MAIL_SUCCESS		0		/* �ɹ� (ͬ��) */
#define MAIL_FAILURE		1		/* ʧ�� (ͬ��) */
#define MAIL_ERROR			2		/* ����		   */
#define MAIL_BLOCKED		3		/* ���� (�첽) */	
#define MAIL_BROKEN			4		/* ���� */
#define MAIL_TIMEOUT		5		/* ������ʱ */
#define MAIL_EXIST          6       /* �ʼ��б���Ϣ�Ѿ����ϲ�Ӧ�ñ��� */
    
/* ����MAIL��Ϣ */
#define MSG_MAIL_LIST		1		/* �ʼ��б���Ϣ */
#define MSG_MAIL_DELE		2		/* ɾ���ʼ���Ϣ */
#define MSG_MAIL_RECV		3		/* �����ʼ���Ϣ */
#define MSG_MAIL_SEND		4		/* �����ʼ���Ϣ */
#define MSG_MAIL_GETHOST	5		/* ������ѯ��Ϣ */
#define MSG_MAIL_CONNECT	6		/* SOCKET������Ϣ */
#define MSG_MAIL_QUIT		7		/* ������Ϣ */
#define MSG_MAIL_BROKEN		8		/* SOCKET������Ϣ */
#define MSG_MAIL_SENDDATA	9		/* ����������Ϣ */
#define MSG_MAIL_RECVDATA	10		/* ����������Ϣ */
#define MSG_MAIL_LOGIN		11		/* �����¼��Ϣ */
#define MSG_MAIL_LISTINFO	12		/* �����б���Ϣ������Ϣ */
#define MSG_MAIL_REFRESH	13		/* ����ˢ���б� */
#define MSG_MAIL_UPDATE		14		/* �����ʼ��б���Ϣ */

/* �����ʼ����״̬ */
#define BS_INVALID			0		/* ��Ч״̬ */
#define BS_INITIAL			1		/* ��ʼ״̬ */
#define BS_LOGIN			2		/* ��¼״̬ */
#define BS_LIST1ST			3		/* ��һ��ȡ�ʼ��б� */
#define BS_LISTERR			4		/* ��һ��ȡ�ʼ��б�ʧ�� */
#define BS_READY			5		/* ���������� */
#define BS_LIST				6		/* ����ˢ���ʼ��б� */
#define BS_RETRIEVE			7		/* ����ȡ�ʼ� */
#define BS_DELETE			8		/* ����ɾ�ʼ� */
#define BS_SENDMAIL			9		/* ���ڷ��ʼ� */
#define BS_QUIT				10		/* �����˳�   */

/* �������������� */
#define ERR_CODE_NULL		0		/* û�д��� */

/* ������Ϣ�ַ������� */
#define TO_LEN				64
#define FROM_LEN			64	
#define DATE_LEN			40
#define SUBJ_LEN			30
#define MID_LEN				40
#define UID_LEN				40


/* �����ʼ��б�ĳ��� */
#ifdef _PRO_BENEFON_
#define MAILLIST_LEN		(1024*1)
#else
#define MAILLIST_LEN		30
#endif

/* ����RFC822�ż�ͷ�������� */
#define FIELD_FROM			0		/* ������ */
#define FIELD_TO			1		/* �ռ��� */
#define FIELD_CC			2		/* ������ */
#define FIELD_MESSAGEID		3		/* �ʼ�ID */
#define FIELD_DATE			4		/* �ʼ����� */
#define FIELD_SUBJECT		5		/* �ʼ����� */
#define FIELD_REPLYTO		6		/* �ظ���ַ */

/*************************************************************************\
*
*	���ݽṹ���������Ͷ���
*
\*************************************************************************/

/* �����ʼ��б� */
typedef struct
{
	int		state;					/* �ʼ���״̬ */
	int		id;						/* ��ʶ�� */
	int		seq;					/* �ʼ������ */
	unsigned long	size;			/* �ʼ��Ĵ�С */
	char	to     [TO_LEN + 1];	/* �ռ�����Ϣ */
	char	from   [FROM_LEN + 1];	/* ��������Ϣ */
	char	subject[SUBJ_LEN + 1];	/* �ʼ��ı��� */
	char	date   [DATE_LEN + 1];	/* �ʼ���ʱ�� */
	char	uidl   [UID_LEN + 1];	/* Ψһ�Ա�ʶ1 */
	char	msgid  [MID_LEN + 1];	/* Ψһ�Ա�ʶ2 */
}	MAILLIST;

/* �����ʺ������ݽṹ */
typedef struct
{
	char* username;					/* �û����� */
	char* password;					/* �û����� */
	char* svr_ip;					/* �����ַ */
	unsigned short svr_port;		/* ����˿� */
	char* retaddr;					/* ���ŵ�ַ */
}	ACCOUNT;

/* ���帽���� */
typedef struct 
{
	char*	name;					/* ���������� */
	char*	charset;				/* �������ַ��� */
	char*	type;					/* ���������� */
	char*	filename;				/* �������ļ��� */
}	ATTACHMENT;

/*************************************************************************\
*
*	��������
*
\*************************************************************************/

/* ģ���ʼ������ */
int		MAIL_Initial(void);

/* ������ຯ�� */
int		MAIL_GetState(int handle);
int		MAIL_GetError(int handle);
int		MAIL_GetMailCount(int handle);
int		MAIL_GetMailState(int handle, int id);
int		MAIL_GetMailListItem(int handle, int index, MAILLIST* list);
int		MAIL_RemoveMail(int handle, int id);
int		MAIL_CancelAsync(int handle);

/* �ռ����ຯ�� */
int		MAIL_CreateInbox(HWND hwnd, int msg, ACCOUNT* account, char* protocol);
int		MAIL_DestroyInbox(int handle);
int     MAIL_ImportMailList(int handle,char* uidl);
int		MAIL_CheckMailList(int handle, int start, int num);
int		MAIL_StopCheckMailList(int handle);
int		MAIL_RetrieveMail(int handle, int id, char* option);
int		MAIL_DeleteMail(int handle, int id);
int		MAIL_QuitInboxSession(int handle);
int		MAIL_RemoteMailNum(int handle);

/* �������ຯ�� */
int		MAIL_CreateOutQueue(HWND hwnd, int msg, ACCOUNT* account, char* protocol);
int		MAIL_DestroyOutQueue(int handle);
int		MAIL_SendMail(int handle);
int		MAIL_StopSend(int handle);
int		MAIL_AddMailToOutQueue(int handle, int fh, char* rcpts);
int		MAIL_QuitOutQueueSession(int handle);

/* �ʼ����ຯ�� */
int		MAIL_CreateFolder(char* path);
int		MAIL_DestroyFolder(int handle);
int		MAIL_AddMailToFolder(int handle, int fh);
int		MAIL_ImportFolder(int handle, int hDestBox, int id);

/* �ż��ຯ�� */
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

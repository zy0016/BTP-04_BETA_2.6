/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailInterface.h
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

//unibox
BOOL mail_init();
BOOL mail_get_messages(HWND hwndmu, int folder);
BOOL mail_get_one_message(MU_MsgNode *msgnode);
BOOL mail_release_message(HWND hwndmu);
BOOL mail_read_message(HWND hwndmu, DWORD msghandle, BOOL bPre, BOOL bNext);
BOOL mail_delete_message(HWND hwndmu, DWORD msghandle);
BOOL mail_delete_multi_messages(HWND hwndmu, DWORD handle[], int i);
BOOL mail_delete_all_messages(HWND hwndmu, int folder, MU_DELALL_TYPE ntype);
BOOL mail_new_message(HWND hwndmu);
BOOL mail_setting(HWND hwndmu);
BOOL mail_get_detail(HWND hwndmu, DWORD handle);
BOOL mail_newmsg_continue(void);
BOOL mail_forward(HWND hwndmu, DWORD handle);
BOOL mail_reply_from(HWND hwndmu, DWORD handle);
BOOL mail_reply_all(HWND hwndmu, DWORD handle);
BOOL mail_get_number(HWND hwndmu, DWORD handle);
BOOL mail_resend_message(HWND hwndmu, DWORD handle);
BOOL mail_defer2send_message(HWND hwndmu, DWORD handle);
BOOL mail_move_message(HWND hwndmu, DWORD handle, int nFolder);
BOOL mail_copy_message(HWND hwndmu, DWORD handle, int nFolder);
BOOL mail_destroy(void);
BOOL mail_get_count(int folder, int *pnunread, int* pncount);
BOOL mail_new_folder(int folder);
//mailbox
static BOOL mailbox_get_mailboxinfo(MU_EMAILBOXINFO *pEmailBoxInfo, int* nMailboxCount);
static BOOL mailbox_read(HWND hwndmu, MU_EMAILBOXINFO *pEmailBoxInfo);
static BOOL mailbox_get_count(MU_EMAILBOXINFO *pEmailBoxInfo, int* pnunread, int* pncount);

static MU_IMessage mu_mail = 
{
        mail_init,
        mail_get_messages,
        mail_get_one_message,
        mail_release_message,
        mail_read_message,
        mail_delete_message,
        mail_delete_multi_messages,
        mail_delete_all_messages,
        mail_new_message,
        mail_setting,
        mail_get_detail,
        mail_newmsg_continue,    
        mail_forward,
        mail_reply_from,
        mail_reply_all,
        mail_get_number,
        mail_resend_message,
        mail_defer2send_message,
        mail_move_message,
        mail_copy_message,
        mail_destroy,
        mail_get_count,
        mail_new_folder
};

static MU_IEmailbox mu_mailbox = 
{
        mailbox_get_mailboxinfo,
        mailbox_read,
        mailbox_get_count
};

//individual
static BOOL MAIL_GetBoxFileList(MailListHead *pHead, int folder);
BOOL MAIL_FileTypeMatch(char* szBoxSavePath, char* szBoxFindPath);
static void MAIL_ReleaseBoxFileList(MailListHead *pHead);
void MAIL_AddBoxFileNode(MailListHead *pHead, MailListNode *pTempNode);
void MAIL_DelBoxFileNode(MailListHead *pHead, int nTempNode);
static BOOL MAIL_SearchBoxFileNode(MailListHead *pHead, MailListNode **pFileNode, int nTempNode);
int MAIL_GetNodeByHandle(MailListHead *pHead, MailListNode **pDelListNode, char *msghandle);
int mail_GetRecNum(HWND hwndmu, DWORD msghandle);

//global count
static void MAIL_CountMailList(void);
static void MAIL_AddDefCountList(CountMailListHead *pHead);
void MAIL_AddCountNode(CountMailListHead *pHead, int MailNum, BOOL UnRead);
void MAIL_DelCountNode(CountMailListHead *pHead, int MailNum, BOOL UnRead);
void MAIL_AddUnreadNode(CountMailListHead *pHead, int MailNum);
void MAIL_DelUnreadNode(CountMailListHead *pHead, int MailNum);
void MAIL_ModifyCountNode(CountMailListHead *pHead, int MailNum, int NewFol, BOOL Unread);
void MAIL_DelCountFolder(CountMailListHead *pHead, int MailNum);
static void MAIL_InitMailBoxCount(MailConfigHeader *pHead);
void MAIL_ModifyMailBoxCount(char *MailboxName, int Unread, int boxnum);
void MAIL_ReinstallMailBoxCount(char *MailboxName, int Unread, int boxnum);
void MAIL_InstallMailBoxCount(char *MailboxName);
BOOL msg_registerrefreshmailbox(HWND hWnd, UINT wMsgCmd);
LONG APP_GetEmailSize(void);

BOOL mailbox_read_default(HWND hwndmu);
BOOL mailbox_get_all_count(int* pnunread, int* pncount);

//extern
extern BOOL CreateMailSendWnd();
extern BOOL CreateMailGetWnd();
extern BOOL CreateMailDialWnd();
extern BOOL MAIL_CreateSetWnd(HWND hParent);
extern BOOL CreateMailInBoxWnd(HWND hParent, const char *SZMAILBOXNAME);
extern BOOL CreateMailDetailWnd(HWND hParent, MailFile *szOutFile);

extern BOOL RegisterMailInBoxClass(void);
extern BOOL RegisterMailViewClass(void);
extern BOOL RegisterMailEditClass(void);

extern void MAIL_DeleteSendNode(MailSendNode *pMailSendNode);
extern void MAIL_DelAllSendMail(void);

extern void MAIL_ReleaseDisList(MailAttachmentHead *pHead);
extern void MAIL_AddDisNode(MailAttachmentHead *pHead, MailAttachmentNode *pTempNode);
extern void MAIL_DelAttachmentNode(MailAttachmentHead *pHead, MailAttachmentNode *pNode);

extern BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
                              const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT, 
                                const SYSTEMTIME *PTIME, DWORD handle, int folderid);
extern BOOL MAIL_CreateViewWnd(HWND hParent, const char *pszOutFile, DWORD handle, int folderid, BOOL bPre, BOOL bNext);

extern void MAIL_ReleaseConfigList(MailConfigHeader *pHead);
extern void MAIL_AddConfigNode(MailConfigHeader *pHead, MailConfigNode *pNewNode);
extern BOOL MAIL_GetConfigNodebyInt(MailConfigHeader *pHead, MailConfigNode *pConfigNode, int iTmp);
extern int MAIL_GetSendNodebyHandle(MailSendNode **pMailSendNode, char *SendHandle);
extern BOOL MAIL_IsNodeSending(char *NodeHandle);

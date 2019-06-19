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
#ifndef _MSGUNIBOX_H
#define _MSGUNIBOX_H

#include "window.h"
#include "string.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "ctype.h"
#include "fcntl.h"
#include "malloc.h"
#include "stdlib.h"
#include "winpda.h" 
#include "stdio.h" 
#include "plx_pdaex.h"

#define MU_WINDOW_MESSAGE_BASE      200

/* when get messages */
#define PWM_MSG_MU_GETMSGS_RESP     (WM_USER + MU_WINDOW_MESSAGE_BASE + 1)
/* when delete one message */
#define PWM_MSG_MU_DELETED_RESP     (WM_USER + MU_WINDOW_MESSAGE_BASE + 2)
/* when delete multi messages */
#define PWM_MSG_MU_DELMULTI_RESP    (WM_USER + MU_WINDOW_MESSAGE_BASE + 3)
/* when delete all messages */
#define PWM_MSG_MU_DELALL_RESP      (WM_USER + MU_WINDOW_MESSAGE_BASE + 4)
/* when change the content of messages */
#define PWM_MSG_MU_MODIFIED         (WM_USER + MU_WINDOW_MESSAGE_BASE + 5)
/* when new message coming */
#define PWM_MSG_MU_NEWMTMSG         (WM_USER + MU_WINDOW_MESSAGE_BASE + 6)
/* when the message change */
#define PWM_MSG_MU_DAEMON           (WM_USER + MU_WINDOW_MESSAGE_BASE + 7)
/* when user pick-up the number of the message to address book,unibox need to update showing info */
#define PWM_MSG_MU_PICKUP 			(WM_USER + MU_WINDOW_MESSAGE_BASE + 8)
/* when retreive e-mail */
#define PWM_MSG_MU_MAILBOX_REFRESH  (WM_USER + MU_WINDOW_MESSAGE_BASE + 9)
/* sub model initiative request to delete msg */
#define PWM_MSG_MU_DELETE           (WM_USER + MU_WINDOW_MESSAGE_BASE + 10)
/*force any child window to close*/
#define PWM_MSG_FORCE_CLOSE			(WM_USER + MU_WINDOW_MESSAGE_BASE + 100)
/**/
#define PWM_MSG_MU_NEXT				(WM_USER + MU_WINDOW_MESSAGE_BASE + 101)
#define PWM_MSG_MU_PRE				(WM_USER + MU_WINDOW_MESSAGE_BASE + 102)
/*---------- Constant / Macro Definitions -----------------------------------*/
typedef enum
{
    /* Only two error types are provided here, successful or failed. */
    MU_ERR_SUCC = 0,
    MU_ERR_FAILED
}MU_ERR_TYPE;

typedef enum
{
    MU_STORE_IN_FLASH = 0,    /* For MMS/SMS/EMAIL/Wap Push , FLASH is default value. */
	MU_STOREAGE_TYPE_MAX = MU_STORE_IN_FLASH
}MU_STORAGE_TYPE;

typedef enum
{
    /* For each message type, the icon displayed in folder list control is individual. */
    MU_MSG_SMS = 0,
    MU_MSG_STATUS_REPORT,   /* deliver report of SMS */
    MU_MSG_MMS,    /* A normal MMS or notification. Notification is ONLY stored in inbox , and identified by status  MU_RECEIVING or MU_UNRECEIVED. */
    MU_MSG_MMS_DELIVER_REPORT,
    MU_MSG_EMAIL,
    MU_MSG_EMAILHEADER,
	MU_MSG_PUSH,
    MU_MSG_FOLDER,
    MU_MSG_VCARD,					//sms
    MU_MSG_VCAL,					//sms
	MU_MSG_BT,
	MU_MSG_BT_VCARD,
	MU_MSG_BT_VCAL,
	MU_MSG_BT_NOTEPAD,
	MU_MSG_BT_PICTURE,
    MU_MSG_TYPE_MAX = MU_MSG_BT_PICTURE
}MU_MSG_TYPE;

typedef enum
{
    /* Four module type are supported currently. Note these are different to the message type. */
    MU_MDU_SMS = 0,
    MU_MDU_MMS,
    MU_MDU_EMAIL,
	MU_MDU_BT,
    MU_MDU_FOLDER,
	MU_MDU_PUSH,
    MU_MDU_TYPE_MAX = MU_MDU_PUSH , 
    MU_MDU_INVALID_TYPE = 0xFF
}MU_MDU_TYPE;

typedef enum
{
    MU_STU_UNRECEIVED = 0,    /* A MMS notification which has not been received. */
	MU_STU_RECEIVING,   /* A MMS notification receiving now. */
	MU_STU_UNREAD,    /* A message(SMS/Wap Push/MMS) which has not been read. */	
    MU_STU_READ   /* A message(SMS/Wap Push/MMS) which has been read. */
}MU_INBOX_STAUS;

typedef enum
{
	MU_STU_SENDING = 4,    /* The message is sending now. */
    MU_STU_SUSPENDED,       /* The message was be delayed to send */
    MU_STU_DEFERMENT,   /* The message was be sent at next connection */
	MU_STU_UNSENT,      /* A message stored in outbox waitting for sending or having been sent unsuccessfully. */
    MU_STU_WAITINGSEND,/* A message stored in outbox waitting for sending*/
}MU_OUTBOX_STATUS;

typedef enum
{
    MU_STU_DRAFT = 9,
}MU_DRAFT_STATUS;

typedef enum
{
    MU_STU_SENT = 10,
}MU_SENT_STATUS;

typedef enum
{
    MU_STU_WAITING = 11,
    MU_STU_CONFIRM,
	MU_STU_FAILED,
}MU_REPORTBOX_STATUS;

typedef enum
{   
    MU_STU_PHRASE = 14, 
    MU_STU_DIR,
}MU_MYFOLDER_STATUS;

typedef enum
{
    MU_STU_UNRECEIVEDUNREAD = 16, 
    MU_STU_RECEIVEDUNREAD, 
    MU_STU_UNRECEIVEDREAD, 
    MU_STU_RECEIVEDREAD,
    MU_STU_DELBUTUNREFER,
}MU_MAILBOX_STATUS;

#define MU_MAX_STATUS    20

typedef enum
{
    MU_INBOX = 0,
    MU_MYFOLDER,
    MU_DRAFT,
    MU_SENT,
    MU_OUTBOX,
    MU_REPORT,
}MU_FOLDER_TYPE;

typedef enum
{
    MU_SEND_START = 1,      /* start sending the message. */
    MU_SEND_WAITING,      
    MU_SEND_TIMING,      	 
    MU_SEND_DEFERMENT,  
	MU_SEND_SUCCESS,        /* the message has been sent successfully. */
	MU_SEND_FAILURE,        /* send the message unsuccessfully. */
	MU_RECV_START,          /* start receiving the message. */
	MU_RECV_SUCCESS,        /* receive the message successfully. */
	MU_RECV_FAILURE,        /* receive the message unsuccessfully. */
	MU_CONN_START ,         /* start connecting. We should enable the "disconnect" item in the folder menu. */
	MU_DISCONNETED,         /* having disconnected. We should disable the "disconnect" item in the folder menu. */
	MU_SEND_SUSPENDED,
}MU_DAEMON_MODIFY_TYPE;


typedef struct tagMU_Daemon_Notify
{
    MU_DAEMON_MODIFY_TYPE type;
	DWORD handle;
}MU_Daemon_Notify;

typedef enum
{
    MU_ATTACHMENT_NOTHING = 0,
    MU_ATTACHMENT_EXIST,
}MU_EMAIL_ATTACHMENT;

/*---------- Type Declarations ----------------------------------------------*/

#define MU_SUBJECT_DIS_LEN  20  /* The max length of the subject displayed in the folder list control. */
#define MU_ADDR_MAX_LEN     100  /* The max length of the address field displayed in the folder list control. */


/*---------- MSG Node ----------------------------------------------*/
typedef struct tagMU_MsgNode
{
	BYTE	msgtype;	 /* NOT module type. *///MU_MSG_TYPE
	BYTE	status;       
	BYTE	storage_type;
	BYTE	attachment;
	DWORD	maskdate;	  /* For MT message, means receiving date; For MO message, means writing date. */
	DWORD	handle;  /* It is the individual identifier of the message, it maybe a id number or a pointer. */
	char	subject[MU_SUBJECT_DIS_LEN + 1];
	char	addr[MU_ADDR_MAX_LEN + 1];
}MU_MsgNode;

typedef enum
{
    MU_DELALL_FOLDER = 0,
    MU_DELALL_FILE,
}MU_DELALL_TYPE;

typedef enum
{
	MU_SMS_NOTIFY = 0,
	MU_MMS_NOTIFY,
	MU_MAIL_NOTIFY,
	MU_BT_NOTIFY,
}MU_NOTIFY_TYPE;

typedef struct tagMU_IMessage
{
    BOOL (*msg_init)();
	BOOL (*msg_get_messages)(HWND hwndmu , int folder);
	BOOL (*msg_get_one_message)(MU_MsgNode *msgnode);
	BOOL (*msg_release_messages)(HWND hwndmu);
	BOOL (*msg_read_message)(HWND hwndmu , DWORD handle, BOOL bPre, BOOL bNext);
	BOOL (*msg_delete_message)(HWND hwndmu , DWORD handle);
	BOOL (*msg_delete_multi_messages)(HWND hwndmu , DWORD handle[],int nCount);
	BOOL (*msg_delete_all_messages)(HWND hwndmu , int folder , MU_DELALL_TYPE ntype);
	BOOL (*msg_new_message)(HWND hwndmu);
	BOOL (*msg_setting)(HWND hwndmu);
	BOOL (*msg_get_detail)(HWND hwndmu,DWORD handle);
	BOOL (*msg_newmsg_continue)();
    BOOL (*msg_forward)(HWND hwndmu, DWORD handle);
    BOOL (*msg_reply_from)(HWND hwndmu, DWORD handle);
    BOOL (*msg_reply_all)(HWND hwndmu, DWORD handle);
    BOOL (*msg_get_number)(HWND hwndmu, DWORD handle);
	BOOL (*msg_resend_message)(HWND hwndmu, DWORD handle);
	BOOL (*msg_defer2send_message)(HWND hwndmu, DWORD handle);
	BOOL (*msg_move_message)(HWND hwndmu, DWORD handle , int nFolder);
	BOOL (*msg_copy_message)(HWND hwndmu, DWORD handle , int nFolder);        //only for email
	BOOL (*msg_destroy)();
    BOOL (*msg_get_count)(int folder,int* pnunread,int* pncount);
    BOOL (*msg_new_folder)(int folder);
}MU_IMessage;

typedef struct tagMU_ISMSEx
{
    BOOL (*sms_sim_create)(HWND hwndmu);
    BOOL (*sms_voicemail_call)(HWND hwndmu);
	BOOL (*sms_cb_create)(HWND hwndmu);
	int  (*sms_get_simcounter)(void);
	int  (*sms_get_cbcounter)(void);
}MU_ISMSEx;

#define MU_EMAILBOX_NAME_LEN  40  /* The max length of the e-mailbox name. for temporary */

typedef struct tagMU_EmailBoxInfo
{
    char szEmailBoxName[MU_EMAILBOX_NAME_LEN];
}MU_EMAILBOXINFO;

typedef struct tagMU_IEmailbox
{
    BOOL (*email_get_mailboxinfo)(MU_EMAILBOXINFO *pEmailBoxInfo,int* nMailboxCount);//if pEmailBoxInfo = NULL , nMailboxCount 
    BOOL (*email_read)(HWND hwndmu, MU_EMAILBOXINFO *pEmailBoxInfo);
    BOOL (*email_get_count)(MU_EMAILBOXINFO *pEmailBoxInfo,int* pnunread,int* pncount);
}MU_IEmailbox;

#define MU_FOLDER_NAME_LEN  30
typedef struct tagMU_FolderInfo
{
#define FOLDER_USED         1
#define FOLDER_UNUSED       0
    BOOL    bUsed;
    int     nFolderID;
    char    szFolderName[MU_FOLDER_NAME_LEN+1];
}MU_FOLDERINFO,*PMU_FOLDERINFO;

/*---------- function Declarations ------------------------------------------*/

extern BOOL mms_register(MU_IMessage **Imsg);
extern BOOL sms_register(MU_IMessage **Imsg);
extern BOOL email_register(MU_IMessage **Imsg);
extern BOOL push_register(MU_IMessage **Imsg);
extern BOOL bt_register (MU_IMessage **Imsg);
extern BOOL smsex_register(MU_ISMSEx **Ismsex);
extern BOOL emailbox_register(MU_IEmailbox **Iemailbox);

/*---------- provide for SMS/MMS/EMAIL/WAPPUSH ------------------------------------------*/
extern BOOL MU_FolderSelection(HWND hParent, HWND hWnd, UINT wMsgCmd, int nFolderType);
extern BOOL MU_EmialboxFolderSelection(HWND hParent, HWND hWnd, UINT wMsgCmd, char* pszEmailboxName);
extern BOOL MU_GetFolderInfo(PMU_FOLDERINFO pInfo,int* pnCount);
extern BOOL MU_IsWindow(void);
extern void APP_SHOWINBOX(void);
extern int  MU_GetCurFolderType(void);
extern const char* MU_GetCurEmailboxName(void);
extern BOOL MU_NewMsgArrival(MU_NOTIFY_TYPE nType);

extern BOOL msg_registerrefreshmailbox(HWND hWnd,UINT wMsgCmd);
extern void MsgNotify(int nType, BOOL bFull, int unRead, int unSend);

extern HWND MuGetFrame(void);

#endif//_MSGUNIBOX_H

/******************************************************************************
																		  
   SEF CONFIDENTIAL AND PROPRIETARY 									  
																		  
   This source is the sole property of SEF Inc. Reproduction or 		  
   utilization of this source in whole or in part is forbidden without	  
   the written consent of SEF Inc.										  
																		  
   (c) Copyright SEF Inc.,		   2003.  All Rights Reserved.			  
																		  
*******************************************************************************
																		  
   mu_main.h	:The header file of mu_main.c.
																		  
*******************************************************************************
   Date 	  Author		   Reference
   ========   =============    ==========================
   03-05-13   xuzc			   CR - BJC000264
			  Create the module.
			  --Initial created

   03-06-13   xuzc             PR - BJP000603
              Modify the interface between message unibox and SMS/EMS/MMS.

   03-07-08   xuzc             PR - BJP000833
              Extend to 20 items in voicego.
              --Add "MMSService".

   03-07-21   xuzc             PR - BJP000959
              Some issues about UI feature.
              --Add two status.

   03-07-30   xuzc             PR - BJP001079
              UI related,refer to content.
              --When having new messages to read , idle window would set the left softkey as "view". 
                A interace is provided here.

/*****************************************************************************/
#ifndef __MU_MAIN_H__
#define __MU_MAIN_H__

#ifdef _cplusplus
extern "C" {
#endif

/*---------- Dependencies-------- -------------------------------------------*/
#include "window.h"
#include "string.h"
#include "fapi.h"
#include "malloc.h"
#include "stdlib.h"
#include "winpda.h" 
#include "stdio.h" 
#include "hp_pdaex.h"

/*---------- Global Definitions and Declarations ----------------------------*/

/*---------- Constant / Macro Definitions -----------------------------------*/

/*---------------------- Constant / Macro Definitions ------------------*/
#define MU_MMS_MESSAGE_MAX_COUNT 200
#define MU_PUSH_MESSAGE_MAX_COUNT 100
#define MU_SUBJECT_DIS_LEN    16    /* The max length of the subject displayed in the folder list control. */
#define MU_ADDR_MAX_LEN       512   /* The max length of the address field displayed in the folder list control. */
#define MMS_SUBJECT_LEN 100

#define MU_WINDOW_MESSAGE_BASE       200
#define PWM_MSG_MU_GETMSGS_RESP      (WM_USER + MU_WINDOW_MESSAGE_BASE + 1)
#define PWM_MSG_MU_DELETED           (WM_USER + MU_WINDOW_MESSAGE_BASE + 2)
#define PWM_MSG_MU_MODIFIED          (WM_USER + MU_WINDOW_MESSAGE_BASE + 3)
#define PWM_MSG_MU_DETALL_RESP       (WM_USER + MU_WINDOW_MESSAGE_BASE + 4)
#define PWM_MSG_MU_STORAGEINFO_RESP  (WM_USER + MU_WINDOW_MESSAGE_BASE + 6)
#define PWM_MSG_MU_NEWMTMSG          (WM_USER + MU_WINDOW_MESSAGE_BASE + 7)
#define PWM_MSG_MU_DAEMON            (WM_USER + MU_WINDOW_MESSAGE_BASE + 8)
#define PWM_MSG_MU_NEWMOMSG 	     (WM_USER + MU_WINDOW_MESSAGE_BASE + 9)
#define PWM_MSG_MU_MOVED             (WM_USER + MU_WINDOW_MESSAGE_BASE + 10)
#define PWM_MSG_MU_REVERTED          (WM_USER + MU_WINDOW_MESSAGE_BASE + 11)

#define MAIL_OUTBOX_FOLDER			1	//发件箱文件夹
#define MAIL_DRAFTBOX_FOLDER		2   //草稿箱文件夹
#define MAIL_SENTBOX_FOLDER			3	//已发送文件夹
#define MAIL_INBOX_FOLDER			4	//收件箱文件夹
/*---------------------------- Type Declarations -----------------------*/

typedef enum
{
    /* Only two error types are provided here, successful or failed. */
    MU_ERR_SUCC = 0,
    MU_ERR_FAILED
}MU_ERR_TYPE;

typedef enum
{
    MU_STORE_IN_FLASH,    /* For MMS/EMS , FLASH is default value. */
	MU_STORE_IN_SIM,
	MU_STOREAGE_TYPE_MAX = MU_STORE_IN_SIM
}MU_STORAGE_TYPE;

typedef enum
{
    /* For each message type, the icon displayed in folder list control is individual. */
    MU_MSG_SMS = 0,
    MU_MSG_STATUS_REPORT,
    MU_MSG_EMS,
    MU_MSG_MMS,    /* A normal MMS or notification. Notification is ONLY stored in inbox , and identified by status  MU_RECEIVING or MU_UNRECEIVED. */
    MU_MSG_MMS_DELIVER_REPORT,
    MU_MSG_CELL_BROADCAST,
    MU_MSG_PUSH,  //push消息类型定义
    MU_MSG_TYPE_MAX = MU_MSG_PUSH
}MU_MSG_TYPE;

typedef enum
{
    /* Four module type are supported currently. Note these are different to the message type. */
    MU_MDU_SMS = 0,
    MU_MDU_EMS,
    MU_MDU_MMS,
    MU_MDU_PUSH,  //push模块类型定义
    MU_MDU_TYPE_MAX = MU_MDU_PUSH , 
    MU_MDU_INVALID_TYPE = 0xFF
}MU_MDU_TYPE;

typedef enum
{
    MU_STU_UNRECEIVED = 0,    /* A MMS notification which has not been received. */
	MU_STU_RECEIVING,   /* A MMS notification receiving now. */
	MU_STU_UNREAD,    /* A message(SMS/EMS/MMS) which has not been read. */	
    MU_STU_READ   /* A message(SMS/EMS/MMS) which has been read. */
}MU_INBOX_STAUS;

typedef enum
{
	MU_STU_SENDING = 4,    /* The message is sending now. */
	MU_STU_UNSENT      /* A message stored in outbox waitting for sending or having been sent unsuccessfully. */
}MU_OUTBOX_STATUS;

typedef enum
{
    MU_STU_DRAFT = 6,
}MU_DRAFT_STATUS;

typedef enum
{
    MU_STU_SENT = 7
}MU_SENT_STATUS;

#define MU_MAX_STATUS    8

typedef enum
{
    MU_INBOX = 0,
    MU_DRAFT,
    MU_OUTBOX,
    MU_SENT,
    MU_FAVORITE, //精彩信息
    MU_FOLDER_TYPE_MAX = MU_FAVORITE,
    MU_FOLDER_COUNT
}MU_FOLDER_TYPE;


typedef enum
{
    MU_SEND_START = 1,   /* start sending the message. */
    MU_SEND_SUCCESS,       /* The message has been sent successfully. */
    MU_SEND_FAILURE,      /* Send the message unsuccessfully. */
    MU_SEND_READY,
    MU_RECV_START = 5,     /* start receiving the message. */
    MU_RECV_SUCCESS,      /* receive the message successfully. */
    MU_RECV_FAILURE,       /* receive the message unsuccessfully. */
    MU_CONN_START ,      /* Start connecting. We should enable the "disconnect" item in the folder menu. */
    MU_DISCONNETED       /* Having disconnected. We should disable the "disconnect" item in the folder menu. */
}MU_DAEMON_MODIFY_TYPE;

/*---------- Type Declarations ----------------------------------------------*/
typedef struct tagMU_MsgNode
{
	BYTE msgtype;	 /* NOT module type. */
	BYTE status;
	BYTE storage_type;
	UINT maskdate;	  /* For MT message, means receiving date; For MO message, means writing date. */
	int handle;  /* It is the individual identifier of the message, it maybe a id number or a pointer. */
	char subject[MU_SUBJECT_DIS_LEN + 1];
	char addr[MU_ADDR_MAX_LEN + 1];

    BYTE byValidate;
}MU_MsgNode;

typedef struct tagMU_IMessage
{
    BOOL (*msg_init)();
	BOOL (*msg_get_messages)(HWND hwndmu , int folder);
	BOOL (*msg_get_one_message)(MU_MsgNode *msgnode);
	BOOL (*msg_release_messages)(HWND hwndmu);
	BOOL (*msg_read_message)(HWND hwndmu , int handle);
	BOOL (*msg_delete_message)(HWND hwndmu , int handle);
	BOOL (*msg_delete_all_messages)(HWND hwndmu , int folder);
	BOOL (*msg_new_message)(HWND hwndmu);
	BOOL (*msg_setting)();
	BOOL (*msg_get_storageinfo)(HWND hwndmu);
	BOOL (*msg_newmsg_continue)();
	BOOL (*msg_destroy)();
}MU_IMessage;

typedef struct tagMU_ISMSEx
{
    BOOL (*sms_voicemail_create)();
	BOOL (*sms_cannedmessage_create)(HWND hWndParent, UINT wCallBackMsg);
	BOOL (*sms_cb_create)();
}MU_ISMSEx;

typedef struct tagMU_Daemon_Notify
{
    int type;
	int handle;
}MU_Daemon_Notify;

typedef struct tagMmsInformation {
	int handle;             // mms 的 handle
    char szNumber[PB_FLASH_ENTRY_EMAIL_MAXLEN + 1];      // 发件人号码
    char szTimeString[32];  // 格式为 2004-04-30 10:25 或其它你方便的格式
    char szSubject[MMS_SUBJECT_LEN + 1];
}MmsInformation;

typedef enum
{
	EMAIL_INBOX,
	EMAIL_OUTBOX,
	EMAIL_DRAFT,
	EMAIL_SENTBOX
} Email_Folder_Type;

/*---------- Type Declarations ----------------------------------------------*/

/*---------- Variable Declarations ------------------------------------------*/

/*---------- function Declarations ------------------------------------------*/

/*---------- Global function Declarations -----------------------------------*/
DWORD MU_NewAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_InboxAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_DraftAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_OutboxAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_SentAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_MMSServiceAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

MU_IMessage *MU_GetMduInterface(int mdutype);
MU_ISMSEx *MU_GetSMSExInterface();
void mu_newmsg_ring(char * pNumber);

BOOL mms_set_mu_interface(MU_IMessage *Imsg);
void mms_set_indbar(int all_unread_count , int unread_count , int unreceived_count);

BOOL MU_NeedViewUnread();
void MU_ClearUnreadField();
void MU_IdleOnLanguageChange();

extern void email_daemon_change(int , int );
extern void email_set_indbar(int );
extern void mms_daemon_change(int , int );
extern void mms_set_indbar(int , int , int );
extern void mms_is_full(BOOL );
extern void mu_newmsg_ring(char *);

#ifdef _cplusplus
}
#endif

#endif /* End of #ifndef __MU_MAIN_H__ */

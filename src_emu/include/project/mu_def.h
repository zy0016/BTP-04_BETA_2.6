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

#include "window.h"
#include "string.h"
//#include "fapi.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "ctype.h"
#include "fcntl.h"

#include "malloc.h"
#include "stdlib.h"
#include "winpda.h" 
#include "stdio.h" 
#include "plx_pdaex.h"
//#include "Progman.h"
//#include "msgwin.h"
#include "str_public.h"

/*---------- Global Definitions and Declarations ----------------------------*/
#define MU_MESSAGE_MAXCOUNT 600
#define MU_PUSH_MESSAGE_MAX_COUNT 100 //push

/*---------- Constant / Macro Definitions -----------------------------------*/
typedef enum
{
    /* Only two error types are provided here, successful or failed. */
    MU_ERR_SUCC = 0,
    MU_ERR_FAILED
}MU_ERR_TYPE;

#define MU_SUBJECT_DIS_LEN  20 //16/* The max length of the subject displayed in the folder list control. */
#define MU_ADDR_MAX_LEN     41    /* The max length of the address field displayed in the folder list control. */

typedef enum
{
    MU_STORE_IN_FLASH,    /* For MMS/EMS , FLASH is default value. */
	MU_STORE_IN_SIM,
    MU_STORE_IN_ROM,
	MU_STOREAGE_TYPE_MAX = MU_STORE_IN_ROM
}MU_STORAGE_TYPE;

typedef enum
{
    /* For each message type, the icon displayed in folder list control is individual. */
    MU_MSG_SMS = 0,
    MU_MSG_STATUS_REPORT,
    MU_MSG_MMS,    /* A normal MMS or notification. Notification is ONLY stored in inbox , and identified by status  MU_RECEIVING or MU_UNRECEIVED. */
    MU_MSG_MMS_DELIVER_REPORT,
    MU_MSG_CELL_BROADCAST,
	MU_MSG_PUSH,
    MU_MSG_TYPE_MAX = MU_MSG_PUSH
}MU_MSG_TYPE;

typedef enum
{
    /* Four module type are supported currently. Note these are different to the message type. */
    MU_MDU_SMS = 0,
    MU_MDU_EMS,
    MU_MDU_MMS,
	MU_MDU_PUSH,
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
    MU_TEMPLATE,
    MU_REPORT,
    MU_FOLDER_TYPE_MAX = MU_REPORT
}MU_FOLDER_TYPE;

typedef enum
{
    MU_SEND_START = 1,   /* start sending the message. */
	MU_SEND_SUCCESS,       /* The message has been sent successfully. */
	MU_SEND_FAILURE,      /* Send the message unsuccessfully. */
	MU_RECV_START = 5,     /* start receiving the message. */
	MU_RECV_SUCCESS,      /* receive the message successfully. */
	MU_RECV_FAILURE,       /* receive the message unsuccessfully. */
	MU_CONN_START ,      /* Start connecting. We should enable the "disconnect" item in the folder menu. */
	MU_DISCONNETED,       /* Having disconnected. We should disable the "disconnect" item in the folder menu. */
	MU_SMS_READED
}MU_DAEMON_MODIFY_TYPE;

#define MU_WINDOW_MESSAGE_BASE       200
#define PWM_MSG_MU_GETMSGS_RESP      (WM_USER + MU_WINDOW_MESSAGE_BASE + 1)
#define PWM_MSG_MU_DELETED      (WM_USER + MU_WINDOW_MESSAGE_BASE + 2)
#define PWM_MSG_MU_MODIFIED      (WM_USER + MU_WINDOW_MESSAGE_BASE + 3)
#define PWM_MSG_MU_DETALL_RESP      (WM_USER + MU_WINDOW_MESSAGE_BASE + 4)
#define PWM_MSG_MU_STORAGEINFO_RESP       (WM_USER + MU_WINDOW_MESSAGE_BASE + 6)
#define PWM_MSG_MU_NEWMTMSG        (WM_USER + MU_WINDOW_MESSAGE_BASE + 7)
#define PWM_MSG_MU_DAEMON         (WM_USER + MU_WINDOW_MESSAGE_BASE + 8)
#define PWM_MSG_MU_NEWMOMSG 	   (WM_USER + MU_WINDOW_MESSAGE_BASE + 9)
#define PWM_MSG_MU_UPDATE 			(WM_USER + MU_WINDOW_MESSAGE_BASE + 10)

#define	MU_IME_SAVEMMSTEMP      WM_USER + 30//save temp ime callback msg

/*---------- Type Declarations ----------------------------------------------*/
typedef struct tagMU_MsgNode
{
	BYTE	msgtype;	 /* NOT module type. */
	BYTE	status;
	BYTE	storage_type;
	BYTE	unused;
	DWORD	maskdate;	  /* For MT message, means receiving date; For MO message, means writing date. */
	DWORD	handle;  /* It is the individual identifier of the message, it maybe a id number or a pointer. */
	char	subject[MU_SUBJECT_DIS_LEN + 1];
	char	addr[MU_ADDR_MAX_LEN + 1];
}MU_MsgNode;

typedef struct tagMU_IMessage
{
    BOOL (*msg_init)();
	BOOL (*msg_get_messages)(HWND hwndmu , int folder);
	BOOL (*msg_get_one_message)(MU_MsgNode *msgnode);
	BOOL (*msg_release_messages)(HWND hwndmu);
	BOOL (*msg_read_message)(HWND hwndmu , DWORD handle);
	BOOL (*msg_delete_message)(HWND hwndmu , DWORD handle);
	BOOL (*msg_delete_all_messages)(HWND hwndmu , int folder);
	BOOL (*msg_new_message)(HWND hwndmu);
	BOOL (*msg_setting)();
	BOOL (*msg_get_storageinfo)(HWND hwndmu);
	BOOL (*msg_newmsg_continue)();
    BOOL (*msg_forward)(HWND hwndmu, DWORD handle);
    BOOL (*msg_reply_from)(HWND hwndmu, DWORD handle);
    BOOL (*msg_reply_all)(HWND hwndmu, DWORD handle);
    BOOL (*msg_get_number)(HWND hwndmu, DWORD handle);
    BOOL (*msg_save_template)(HWND hwndmu, DWORD handle);
    BOOL (*msg_template_message)(HWND hwndmu, DWORD handle);
	BOOL (*msg_edittemp_message)(HWND hwndmu, DWORD handle);
	BOOL (*msg_resend_message)(HWND hwndmu, DWORD handle);
	BOOL (*msg_destroy)();
}MU_IMessage;

typedef struct tagMU_ISMSEx
{
    BOOL (*sms_voicemail_create)();
	BOOL (*sms_cb_create)();
}MU_ISMSEx;

typedef struct tagMU_Daemon_Notify
{
    int type;
	DWORD handle;
}MU_Daemon_Notify;


/*---------- Variable Declarations ------------------------------------------*/

/*---------- function Declarations ------------------------------------------*/

/*---------- Global function Declarations -----------------------------------
DWORD MU_NewAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_InboxAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_DraftAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_OutboxAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_SentAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);
DWORD MU_MMSServiceAppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam);

MU_IMessage *MU_GetMduInterface(int mdutype);
MU_ISMSEx *MU_GetSMSExInterface();
void mu_newmsg_ring(int type);

void mms_set_indbar(int all_unread_count , int unread_count , int unreceived_count);

BOOL MU_NeedViewUnread();
void MU_ClearUnreadField();
void MU_IdleOnLanguageChange();
*/
extern BOOL mms_set_mu_interface(MU_IMessage **Imsg);
extern BOOL sms_set_mu_interface(MU_IMessage **Imsg);
extern BOOL smsex_set_mu_interface(MU_ISMSEx **Ismsex);

typedef enum
{
	MU_ICON_NEW= 0,
	MU_ICON_IN,
	MU_ICON_DRAFT,
    MU_ICON_TEMPLATE,
	MU_ICON_PHRASE,
	MU_ICON_SETUP,
	MU_ICON_OUT,
	MU_ICON_SENT,
	MU_ICON_REPORT,
	MU_ICON_VOICE,
	MU_ICON_BROADCAST,
	MU_ICON_NEW_SMS,
	MU_ICON_NEW_MMS,
	MU_ICON_SETUP_SMS,
	MU_ICON_SETUP_MMS,
    MU_ICON_SPACE,
	MU_ICON_VIEW_SMS,
	MU_ICON_VIEW_MMS,
	MU_ICON_BC_SUBJECT,
	MU_ICON_BC_LANGUAGE,
	MU_ICON_BC_OFF,
	MU_ICON_VOICE_CONNECT,
	MU_ICON_VOICE_SETUP,
}MU_MENULIST_ICON_TYPE;

extern const char* const mu_MenuListIcon[];

extern void MMSNotify(int nMMS,BOOL bFull);
extern void SMSNotify(int nSMS,BOOL bFull);

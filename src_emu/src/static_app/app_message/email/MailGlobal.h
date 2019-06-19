/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MailGlobal.h
 *
 * Purpose  : 
 *
\**************************************************************************/

static const char *nMonthEnString[12] = 
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", 
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" 
};

static const int nMonthDaString[12] = 
{
    1, 2, 3, 4, 5, 6, 
        7, 8, 9, 10, 11, 12
};

static char *nWeekEnString[7] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *MAIL_AFFIX_PIC_EXTNAME[MAX_MAIL_PIC_ATT_KIND + 1]=
{
    "BMP", "JPG", "GIF", "WBMP", "JPEG", NULL
};


static const char *MAIL_AFFIX_RING_EXTNAME[MAX_MAIL_SOUND_ATT_KIND + 1] =
{
    "WAV", "MID","AMR", NULL
};

//attachment flag
typedef enum
{
    MAIL_ATTFLAG_NOEXIST = 0,
    MAIL_ATTFLAG_EXIST,
    MAIL_ATTFLAG_NOUSED,
}MAIL_ATTFLAG;

//send status
typedef enum
{
    MAIL_SEND_SENDING = 0,             //sending now
    MAIL_SEND_DELAY,                   //be delayed to send
    MAIL_SEND_DEFERMENT,               //be sent at next connection
    MAIL_SEND_UNSENT,                  //stored in outbox waitting for sending or having been sent 
                                       //unsuccessfully
    MAIL_SEND_SUCCESS,                 //be sent successfully
}MAIL_SEND_STATUS;

//box type
typedef enum
{
    MAIL_INBOX_TYPE = 0,
    MAIL_OUTBOX_TYPE,
    MAIL_DRAFTBOX_TYPE,
    MAIL_SENDBOX_TYPE,
    MAIL_MAILBOX_TYPE,
    MAIL_FOLDER_TYPE,
}MAIL_BOX_BYPE;

//CPU byte order
typedef enum
{
    MAIL_CPU_REVERSE = 0,              //reverse btye order
    MAIL_CPU_OBVERSE,                  //normal btye order     
}MAIL_CPU_ORDER;

//read flag
typedef enum
{
	UNREAD = 0,                        //unread mail
	READ,                       	   //readed mail
    RETRIEVE,
}MAIL_READ_FLAG;

//get status
typedef enum
{
    UNDOWN = 0,
    DOWNSER,
    DOWNLOCAL,
}MAIL_DOWNLOAD_FLAG;

//delete from serverflag
typedef enum
{
    DELSERVER = 0,
    UNDELSERVER,
}MAIL_DELETE_SERVER_FLAG;

//delete from local file
typedef enum
{
    DELLOCAL = 0,
    UNDELLOCAL,
}MAIL_DELETE_LOCAL_FLAG;

//call HwndMaildial and call MailServer opreation
typedef enum
{
    MAIL_GET_NO_OPREATION,
    MAIL_GET_HEADER,
    MAIL_GET_DOWN,
    MAIL_GET_STOP_DOWN,
    MAIL_GET_DELETE,
}MAIL_GET_OPREATION;

//connect style
typedef enum
{
    MAIL_GET_CONNECT = 0,
    MAIL_GET_DISCONNECT,
}MAIL_GET_CONNECT_TYPE;

//delete style
typedef enum
{
    MAIL_DEL_LOCAL = 0,
    MAIL_DEL_ALL,
}MAIL_DEL_TYPE;

//default mailbox style
typedef enum
{
    MAIL_DEF_UNSELECTED = 0,
    MAIL_DEF_SELECTED,
}MAIL_DEF_MAILBOX;

//message sending
typedef enum
{
    MAIL_CONNECT_AUTO = 0,
    MAIL_CONNECT_WHEN,
}MAIL_MSG_SENDING;

//mailbox type
typedef enum
{
    MAIL_TYPE_POP3 = 0,
    MAIL_TYPE_IMAP4,
}MAIL_BOX_TYPE;

//autoretrieval
typedef enum
{
    MAIL_AUTORET_OFF = 0,
    MAIL_AUTORET_10MIN,
    MAIL_AUTORET_30MIN,
    MAIL_AUTORET_1H,
    MAIL_AUTORET_2H,
    MAIL_AUTORET_6H,
    MAIL_AUTORET_12H,
    MAIL_AUTORET_24H,
}MAIL_AUTO_RETRIEVAL;

//retrieve headers
typedef enum
{
    MAIL_RET_HEADER_ALL = 0,
    MAIL_RET_HEADER_5,
    MAIL_RET_HEADER_10,
    MAIL_RET_HEADER_30,
    MAIL_RET_HEADER_50,
    MAIL_RET_HEADER_100,
}MAIL_RETRIEVE_HEADER;

//compare result
typedef enum
{
    Early = 0,
    Equality,
    Late,
}MAIL_COMPARE_RESULT;

//confirm style
typedef enum
{
    Con_None = 0,
    Con_Create,                 //when create window whether to connect
    Con_Open,                   //when open whether to connect
    Con_Delete,                 //whether delete
    Con_DeleteSel,              //whether delete select
    Con_DeleteAll,              //whether delete all
    Con_DelSelect,              //delete local or all(local and server)
    Con_DelSelectSel,           //delete local or all(local and server)
    Con_DelSelectAll,           //delete local or all(local and server)
    Con_DisConnect,             //whether disconnect
    Con_Close,                  //when close disconnect the mailbox
    Con_MoveOne,                //move one mail
    Con_MoveSelect,             //move selected mail
    Con_MoveAll,                //move all mail
}MAIL_CONFIRM_STYLE;

//remove node style
typedef enum
{
    REMOVE_SEL = 0,
    REMOVE_DEL,
    REMOVE_GET,
}MAIL_REMOVE_STYLE;

//select sub window
typedef enum
{
    DELETE_SELECT = 0,
    MOVE_SELECT,
}MAIL_SELECT_TYPE;

//insert attachment type
typedef enum
{
    INSERT_PIC = 0,
    INSERT_RING,
    INSERT_NODE,
    INSERT_VCARD,
    INSERT_VCAL,
    INSERT_TEMLATE,
}MAIL_INSERT_TYPE;

//convert path type
typedef enum
{
    FLASH_COLON_TO_MNT = 0,
    FLASH_MNT_TO_COLON,
    RAM_COLON_TO_MNT,
    RAM_MNT_TO_COLON,
}MAIL_CONVERT_TYPE;

//mailbox connect status
typedef enum
{
    MAILBOX_CON_IDLE = 0,
    MAILBOX_CON_DIAL,
    MAILBOX_CON_CONNECT,
    MAILBOX_CON_ONLINE,
    MAILBOX_CON_RETRIEVE,
    MAILBOX_CON_SEND,
    MAILBOX_CON_DELETE,
    MAILBOX_CON_DISCONNECT,
}MAILBOX_CONNECT_STATUS;

//mailbox connect type
typedef enum
{
    MAILBOX_CON_NONE = 0,
    MAILBOX_CON_HIDE,
    MAILBOX_CON_SHOW,
}MAILBOX_CON_TYPE;

/* Callback function when caret position changing */
typedef void (CALLBACK *CARETPROC)(const RECT*);
extern CARETPROC WINAPI SetCaretProc(CARETPROC pNewProc);

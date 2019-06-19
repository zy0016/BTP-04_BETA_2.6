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
#ifndef _SMS_CONST_H_
#define _SMS_CONST_H_

#define IDC_OK			    100
#define IDC_CANCEL			200
#define IDC_CLASS0_VIEW     300
#define IDC_CB_MAIN         400
#define IDC_CB_VIEW         500
#define IDC_SMS_SIM         600

#define WM_EXIT             WM_USER+600
#define WM_SMS_SETBC        WM_USER+150

#define WM_EDITPROFILENAME  WM_USER+1
#define WM_EDITCENTER       WM_USER+2
#define WM_FRESHSETTINGLIS  WM_USER+3
#define WM_SETCENTERNR      WM_USER+4
#define WM_DELETEPROFILE    WM_USER+5
#define WM_SELECTLANGUAGE   WM_USER+6
#define WM_ADDRECIPIENT		WM_USER+7
#define WM_SAVETODRAFT		WM_USER+8
#define WM_SUREFRAGSEND		WM_USER+9
#define WM_SAVEACTIVATE		WM_USER+10
#define WM_SIMDELALL		WM_USER+11
#define WM_SIMDEL			WM_USER+12
#define WM_SURETODIAL       WM_USER+13
#define WM_COPYSIM          WM_USER+14
#define WM_SIMDELSEL        WM_USER+15
#define WM_SURETODELSEL     WM_USER+16
#define WM_ADDTOPIC         WM_USER+17
#define WM_SENDWITHOPTIONS  WM_USER+18
#define WM_INSERTTEMPLATE   WM_USER+19
#define WM_SAVETOAB         WM_USER+20
#define WM_CANCELCOPY       WM_USER+21
#define WM_CANCELDEL        WM_USER+22
#define WM_SUREDELETE       WM_USER+23
#define WM_MODIFYTOPIC      WM_USER+24
#define WM_CBMSG_PRE        WM_USER+25
#define WM_CBMSG_NEXT       WM_USER+26
#define WM_INITSELECT       WM_USER+27
#define WM_CBMSG_REFRESH    WM_USER+28
#define WM_SELECTFOLDER     WM_USER+30
#define WM_SMSREPLACESEL    WM_USER+31
#define WM_SETCENTERNRFORSEND   WM_USER+32
#define WM_SETNRRESULT      WM_USER+33

#define WM_SIM_CANCEL           WM_USER+100
#define WM_SIM_OK               WM_USER+101
#define	WM_SMS_PDU_LIST			WM_USER+102
#define	WM_SMS_PDU_DELETE		WM_USER+103
#define	WM_SMS_PDU_READ			WM_USER+104	
#define WM_SMS_MEMCOUNT			WM_USER+105
#define WM_SMS_CHANGEMODE		WM_USER+106
#define MSG_SIM_DELETE          WM_USER+107
#define MSG_SIM_DELETE_SUCC     WM_USER+108
#define MSG_SIM_DELETE_FAIL     WM_USER+109


#define SMS_MALLOC_LEN      ((2*1024)-1)

#define MAX_TXT_SIZE        100*134

#define SMS_FREE(p)         do { if(p) free(p); p=NULL;} while(0)            

#define SMS_FILE_POSTFIX    ".sms"

#define PATH_MAXLEN         256
#define PATH_DIR_SMS        "/mnt/flash/sms"

#define SMS_FILENAME_MAXLEN (15+1)

#define SMS_FILENAME_INBOX      "inbox.sms"
#define SMS_FILENAME_MYFOLDER   "myfolder.sms"
#define SMS_FILENAME_SENT       "sent.sms"
#define SMS_FILENAME_REPORT     "report.sms"

//#define SMS_FILENAME_SCA        "smssca.inf"
#define SMS_FILENAME_SETTING    "smssetting.inf"
#define SMS_FILENAME_CBTOPIC    "topic.cb"
#define SMS_FILENAME_CBSETTING  "setting.cb"
#define SMS_FIMENAME_TEMP       "reset.tmp"
#define SMS_FILENAME_VMN        "vmn.inf"
#define SMS_FILENAME_ACTIVATEID "activateID.inf"
#define SMS_FILENAME_MESTOREINFO    "mestore.inf"
#define SMS_FILENAME_CELLINFO    "cellinfo.inf"


#define ICON_WIDTH              22
#define ICON_HEIGHT             16

#define IMG_READ_BMP            "/rom/message/unibox/sms_read.bmp"
#define IMG_UNREAD_BMP          "/rom/message/unibox/sms_unre.bmp"
#define SMS_ICO_ARROWLEFT       "/rom/public/arrow_left.ico"
#define SMS_ICO_ARROWRIGHT      "/rom/public/arrow_right.ico"
#define SMS_BMP_ADDTOPIC        "/rom/message/sms/addtopic.bmp"
#define SMS_BMP_TOPIC           "/rom/message/sms/topic.bmp"
#define SMS_BMP_SELECT          "/rom/message/sms/rb_select.bmp"
#define SMS_BMP_NORMAL          "/rom/message/sms/rb_normal.bmp"
#define SMS_BMP_ACTIVE          "/rom/message/sms/active.bmp"
#define SMS_BMP_NEWSMSPROFILE   "/rom/message/sms/newsmsprofile.bmp"
#define SMS_BMP_SMSPROFILE      "/rom/message/sms/smsprofile.bmp"

#define SMS_FILE_DRAFT_MARK     '2'
#define SMS_FILE_OUTBOX_MARK    '4'
#define SMS_FILE_FOLDER_MARK    "A"
#define SMS_FILE_INBOX_MARK     '0'


#define TEXT_WIDTH              150
#define TEXTDETAIL_WIDTH        160
#define OMIT_SUFFIX             '.'
#define SUFFIX_REPEAT           3

#define SMS_VCARD_PORTFIX       ".vcf"
#define SMS_VCAL_PORTFIX        ".rtf"

#define KEY_SHORT_TIME          600

#define SMS_EDIT_SPACE          6

typedef enum
{
    STORE_FILE = 0,
    STORE_OFFSET,
    STORE_INDEX,
};
#define SMS_USED                 1
#define SMS_DEL                  0

#define SMS_SIMINDEX_NULL        -1

#define SMS_SIM                  -1
#define SMS_CB                   -2
#define SMS_REPORT               -3

typedef enum
{
    SETTING_METHOD_EDIT = 0,
    SETTING_METHOD_SETTING,
};

typedef enum
{
    SMS_COUNT_UNREAD = 0,
    SMS_COUNT_ALL,   
    SMS_COUNT_DEL,     
}SMS_COUNT_TYPE;

#define SMS_RESET_MAX  20


#define ISNEWLINE(p)  \
		(*p == '\r' || *p == '\n' )

#define ISENTERNEWLINE(p) \
        (*p == '\r' && *(p+1) && *(p+1) == '\n')


#define SMS_PHONE_LEN       (41+1)

#define ICON_OPTIONS        "Options"
#define ICON_SELECT         "Select"

#define SMS_RECMAX          20


typedef enum
{
    CB_DELETE = 0,
    CB_ADD,   
    CB_MODIFY,     
}CB_OPTION;


#define CB_LANGUAGE_MAX     15

#define CB_MAX              1000
#define CB_SUBSCRIBE_MAX    5


#endif // _SMS_CONST_H_

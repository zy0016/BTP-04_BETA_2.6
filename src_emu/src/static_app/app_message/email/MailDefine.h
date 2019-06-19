/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailDefine.h
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

#define     _BENEFON_MAIL_
#undef      _NOKIA_MAIL_

#define     MAX_PROTOCOL_FILE           MAILLIST_LEN
#define     MAIL_DIAL_HANDUP_NOW        1
#define     WAITTIMEOUT                 20
#define     MIN_FREE_SPACE		        200 * 1024
#define     MU_BOXMAIL                  1000
#define     MAIL_CHAR_AT                ((char)0xfd)
#define     MAX_YEAR                    2050

/***************** WM_USER defination ************************/
//mail message
#define     WM_MAIL_MSG                 (WM_USER + 301)   //mail message from mail-protocol module
#define     WM_MAIL_BEGIN_SEND          (WM_USER + 302)   //ask to send mail
#define     WM_MAIL_TERMINATE_SEND      (WM_USER + 303)   //terminate mail sending

#define     WM_MAIL_DIAL_AGAIN          (WM_USER + 304)   //need dial message
#define     WM_MAIL_DIAL_HANDUP         (WM_USER + 305)   //allow dial hang up
#define     WM_MAIL_DIAL_DIALUP         (WM_USER + 306)

#define     WM_MAIL_DIAL_SUCCESS        (WM_USER + 307)   //dial succeed message
#define     WM_MAIL_DIAL_CANCEL         (WM_USER + 308)   //dial failure message

#define     WM_MAIL_BOX_RETURN          (WM_USER + 309)   //mailbox return to setting

#define     WM_MAIL_DELETE_RETURN       (WM_USER + 310)   //delete confirm return
#define     WM_MAIL_FOLDER_RETURN       (WM_USER + 311)   //move to folder return
#define     WM_MAIL_ADDRESS_RETURN      (WM_USER + 312)   //add recipient return
#define     WM_MAIL_CANCEL_RETURN       (WM_USER + 313)   //cancel connecting return
#define     WM_MAIL_INSERT_RETURN       (WM_USER + 314)   //insert attachment return
#define     WM_MAIL_DIS_RETURN          (WM_USER + 315)   //display window return
#define     WM_MAIL_TIPS_RETURN         (WM_USER + 316)   //tips win return
#define     WM_MAIL_SAVEOBJ_RETURN      (WM_USER + 317)   //save object return
#define     WM_MAIL_REPLACE_SEL         (WM_USER + 318)   //after insert template replace cursel 

#define     ID_MAILBOX_SELECT_RET       (WM_USER + 320)   //mailbox selection return
                                                          //lparam is string pointer
#define     ID_MAILBOX_MODIFY           (WM_USER + 321)
#define     ID_MAILBOX_NEW              (WM_USER + 322)
#define     ID_MAIL_LIST_REFRESH        (WM_USER + 323)

//mail command function
#define     IDM_MAIL_BUTTON_EXIT        (WM_USER + 330)   //exit window
#define     IDM_MAIL_BUTTON_SEND        (WM_USER + 331)
#define     IDM_MAIL_BUTTON_OPTION      (WM_USER + 332)
#define     IDM_MAIL_BUTTON_SAVE        (WM_USER + 333)
#define     IDM_MAIL_BUTTON_ADD         (WM_USER + 334)
#define     IDM_MAIL_BUTTON_INSERT      (WM_USER + 335)
#define     IDM_MAIL_BUTTON_ATTACHMENT  (WM_USER + 336)
#define     IDM_MAIL_BUTTON_INFO        (WM_USER + 337)
#define     IDM_MAIL_BUTTON_OK          (WM_USER + 338)
#define     IDM_MAIL_BUTTON_EDIT        (WM_USER + 339)
#define     IDM_MAIL_BUTTON_ACTI        (WM_USER + 340)
#define     IDM_MAIL_BUTTON_DELETE      (WM_USER + 341)

#define     IDM_MAIL_BUTTON_OPEN        (WM_USER + 342)
#define     IDM_MAIL_BUTTON_REPLY       (WM_USER + 343)
#define     IDM_MAIL_BUTTON_FORWARD     (WM_USER + 344)
#define     IDM_MAIL_BUTTON_MOVE        (WM_USER + 345)
#define     IDM_MAIL_BUTTON_MOVEMANY    (WM_USER + 346)
#define     IDM_MAIL_BUTTON_CONNECT     (WM_USER + 347)
#define     IDM_MAIL_BUTTON_DISCONNECT  (WM_USER + 348)
#define     IDM_MAIL_BUTTON_RETRIEVE    (WM_USER + 349)
#define     IDM_MAIL_BUTTON_MARK        (WM_USER + 350)

#define     IDM_MAIL_INSERT_PICTURE     (WM_USER + 351)
#define     IDM_MAIL_INSERT_SOUND       (WM_USER + 352)
#define     IDM_MAIL_INSERT_NOTE        (WM_USER + 353)
#define     IDM_MAIL_INSERT_TEMPLATE    (WM_USER + 354)

#define     IDM_MAIL_REPLY_TOSENDER     (WM_USER + 355)
#define     IDM_MAIL_REPLY_TOALL        (WM_USER + 356)

#define     IDM_MAIL_DELETE_SELECT      (WM_USER + 357)
#define     IDM_MAIL_DELETE_ALL         (WM_USER + 358)

#define     IDM_MAIL_MOVE_SELECT        (WM_USER + 500)
#define     IDM_MAIL_MOVE_ALL           (WM_USER + 501)

#define     IDM_MAIL_VIEW_GOTOURL               (WM_USER + 550)
#define     IDM_MAIL_VIEW_ADDTOBM               (WM_USER + 551)
#define     IDM_MAIL_VIEW_VIEWONMAP             (WM_USER + 552)
#define     IDM_MAIL_VIEW_SETASDESTINATION      (WM_USER + 553)
#define     IDM_MAIL_VIEW_SAVEASWAYPOINT        (WM_USER + 554)
#define     IDM_MAIL_VIEW_SEND                  (WM_USER + 555)
#define     IDM_MAIL_VIEW_SEND_SMS              (WM_USER + 556)
#define     IDM_MAIL_VIEW_SEND_MMS              (WM_USER + 557)
#define     IDM_MAIL_VIEW_SEND_EMAIL            (WM_USER + 558)
#define     IDM_MAIL_VIEW_ATTACHMENTS           (WM_USER + 559)
#define     IDM_MAIL_VIEW_REPLY                 (WM_USER + 560)
#define     IDM_MAIL_VIEW_REPLYMANY             (WM_USER + 561)
#define     IDM_MAIL_VIEW_FORWARD               (WM_USER + 562)
#define     IDM_MAIL_VIEW_ADDTOCONTACTS         (WM_USER + 563)
#define     IDM_MAIL_VIEW_ADDTOCONTACTS_UPDATE  (WM_USER + 564)
#define     IDM_MAIL_VIEW_ADDTOCONTACTS_CREATE  (WM_USER + 565)
#define     IDM_MAIL_VIEW_FIND                  (WM_USER + 566)
#define     IDM_MAIL_VIEW_FIND_PHONE            (WM_USER + 567)
#define     IDM_MAIL_VIEW_FIND_EMAIL            (WM_USER + 568)
#define     IDM_MAIL_VIEW_FIND_WEB              (WM_USER + 569)
#define     IDM_MAIL_VIEW_FIND_COORDINATE       (WM_USER + 570)
#define     IDM_MAIL_VIEW_HIDEFOUNDITEMS        (WM_USER + 571)
#define     IDM_MAIL_VIEW_MOVETOFOLDER          (WM_USER + 572)
#define     IDM_MAIL_VIEW_MESSAGEINFO           (WM_USER + 573)
#define     IDM_MAIL_VIEW_DELETE                (WM_USER + 574)

//mail edit ID
#define     IDC_MAIL_EDIT_RECIPIENT     (WM_USER + 360)
#define     IDC_MAIL_EDIT_RECIPIENT2    (WM_USER + 361)
#define     IDC_MAIL_EDIT_TITLE         (WM_USER + 362)
#define     IDC_MAIL_EDIT_CONTENT       (WM_USER + 363)
#define     IDC_MAIL_VIEW_LIST          (WM_USER + 364)
#define     IDC_MAIL_SETTING_LIST       (WM_USER + 365)
#define     IDC_MAIL_BOXMAIL_LIST       (WM_USER + 700)
#define     IDC_MAIL_SELECT_LIST        (WM_USER + 701)

#define     IDC_MAIL_BOX_MAILNAME       (WM_USER + 366)
#define     IDC_MAIL_BOX_CONNECT        (WM_USER + 367)
#define     IDC_MAIL_BOX_MAILADDR       (WM_USER + 368)
#define     IDC_MAIL_BOX_OUTSERVER      (WM_USER + 369)
#define     IDC_MAIL_BOX_SENDMSG        (WM_USER + 370)
#define     IDC_MAIL_BOX_CC             (WM_USER + 371)
#define     IDC_MAIL_BOX_SIGNATURE      (WM_USER + 372)
#define     IDC_MAIL_BOX_LOGIN          (WM_USER + 373)
#define     IDC_MAIL_BOX_PASSWORD       (WM_USER + 374)
#define     IDC_MAIL_BOX_INSERVER       (WM_USER + 375)
#define     IDC_MAIL_BOX_MAILTYPE       (WM_USER + 376)
#define     IDC_MAIL_BOX_AUTORET        (WM_USER + 377)
#define     IDC_MAIL_BOX_SECURITY       (WM_USER + 378)
#define     IDC_MAIL_BOX_SECURELOGIN    (WM_USER + 379)
#define     IDC_MAIL_BOX_RETRATT        (WM_USER + 380)
#define     IDC_MAIL_BOX_RETRHEAD       (WM_USER + 381)

#define     IDC_MAIL_SELECT_PUBLIC      (WM_USER + 382)
#define     MAIL_TIMER_ID               (WM_USER + 801)

//mail get function
/*
#define     WP_MAIL_GET_REFRESH         WM_USER + 400   //refresh
#define     WP_MAIL_GET_HEADER          WM_USER + 401   //connect,call protocol fuctions to get mail header
#define     WP_MAIL_GET_QUIT            WM_USER + 402   //cut connect
#define     WP_MAIL_GET_DELETE          WM_USER + 403   //delete mail on server
#define     WP_MAIL_GET_DOWNLOAD        WM_USER + 404   //download mail from server
#define     WP_MAIL_GET_STOP_DOWN       WM_USER + 405   //stop download from server*/

/***************** middle icon defination ************************/
//middle button icon
#define     ICON_OPTIONS                "Options"
#define     ICON_SELECT                 "Select"

/***************** length and size defination ************************/
//mail config define
#define		MAX_MAIL_BOXNAME_LEN	    30				//!! maximum mailbox name length
#define     MAX_MAIL_CONNECTION_LEN     128             //!! maximum access point in use length
#define		MAX_MAIL_MAILADDR_LEN		128 			//!! maximum my e-mail address length
#define     MAX_MAIL_OUTGOSERVER_LEN    64              //!! maximum outgoing mail server length
#define		MAX_MAIL_SIGNATURE_LEN		128				//!! maximum include signature length
#define     MAX_MAIL_LOGIN_LEN          128             //!! maximum user name length
#define		MAX_MAIL_PASSWORD_LEN		128				//!! maximum password length
#define     MAX_MAIL_INCOMSERVER_LEN    64              //!! maximum incoming mail server length

//MailFile define
#define     MAX_MAIL_ADDRESS_SIZE       (MAX_MAIL_RECIEVESOLE_LEN * 20)		//!! maximum reciever length supported (To field)
#define     MAX_MAIL_MESSAGEID_LEN      512             //!! Message-Id length			
#define     MAX_MAIL_FROM_LEN           MAX_MAIL_ADDRESS_SIZE
#define     MAX_MAIL_TO_LEN             MAX_MAIL_ADDRESS_SIZE
#define     MAX_MAIL_DATE_LEN           40              //!! Standard DateTime length
#define     MAX_MAIL_SUBJECE_LEN        512				//!! maximum subject length supported
#define     MAX_MAIL_LOCALDATE_LEN      20              //!! Local DateTime length		2003-09-06 15:12:13

//display
#define     MAX_MAIL_RECIPIENT_LEN      (MAX_MAIL_RECIEVESOLE_LEN * 20)      //recipient length
#define     MAX_MAIL_RECIEVESOLE_LEN    110
#define     MAX_MAIL_RECIPIENT_NUM      20              //recipient number

//attachment file define
#define     MAX_MAIL_SHOW_FILENAME_LEN  80				//!! attachment showing name (as: nihaohahaha.jpg )
#define     MAX_MAIL_SAVE_FILENAME_LEN  16				//!! file saving name length (saved in flash: $ml0xxxx.ml$)
#define     MAX_MAIL_SAVE_FULLNAME_LEN  80
#define     MAX_MAIL_PIC_ATT_KIND       5
#define     MAX_MAIL_SOUND_ATT_KIND     3
#define     MAX_MAIL_NODE_ATT_KIND      1

//get mail define
#define     MAX_MAIL_UID_LEN            40
#define     MAX_MAIL_MSGID_LEN          512

//other
#define     MAX_MAIL_FILE_PATH_LEN      512             //!! full path file name length

//mail size
#define     MAX_MAIL_SIZE               (220 * 1024)    //maximum mail total size(roughly)
#define     MAX_MAIL_BODY_SIZE          (20 * 1024)     //maximum mail body(text) size
#define     MAX_MAIL_ATT_SIZE           (200 * 1024)     //maximum mail attachment size
#define     MAX_MAIL_SIZE_K             (100 + 50)      //minimum mail system (unit K)

//server(mailbox) number
#define     MAX_MAIL_SERVER_NUM         10              //maximum account
#define     MAX_MAIL_TOTAL_MAIL_NUM     100             //maximum mail items in mail application

//menu status
#define     MAIL_MENUSTATUS_SEND        0x01
#define     MAIL_MENUSTATUS_OPTION      0x02
#define     MAIL_MENUSTATUS_SAVE        0x04
#define     MAIL_MENUSTATUS_ADD         0x08
#define     MAIL_MENUSTATUS_INSERT      0x10
#define     MAIL_MENUSTATUS_ATTACHMENT  0x20

//connect type
#define     MAIL_CON_TYPE_SEND          0x01
#define     MAIL_CON_TYPE_HIDEREC       0x02
#define     MAIL_CON_TYPE_SHOWREC       0x04

//space size
#define     MAIL_MALLOC_LEN             MAX_MAIL_ADDRESS_SIZE
#define     MAIL_EDIT_SPACE             6               //content edit space
#define     PATH_MAXLEN                 256             //old path length

//sub window
#define     PARTWND_X               0               //display window area(edit\timeedit\spinbox)
#define     PARTWND_Y               0
#define     PARTWND_WIDTH           176
#define     PARTWND_HEIGHT          50
#define     RECIWND_HEIGHT          60
#define     LR_ICON_WIDTH           20
#define     LR_ICON_HEIGHT          28

//extern define
#define     META_OTHER				0
#define     META_AUDIO				2
#define     META_IMG				3
#define     META_TEXT               5

/***************** string defination ************************/
#define     MAIL_FILE_PATH              "/mnt/flash/email"
#define     MAIL_FILE_PATH_DIR          "/mnt/flash/email/"
#define     MAIL_FILE_PIC_PATH          "/mnt/flash/picture/"
#define     MAIL_FILE_AUDIO_PATH        "/mnt/flash/audio/"
#define     MAIL_FILE_TEXT_PATH         "/mnt/flash/notepad/"

#define     MAIL_PATH_FLASH_COLON       "FLASH2:/"
#define     MAIL_PATH_FLASH_MNT         "/mnt/flash/"
#define     MAIL_PATH_RAM_COLON         "RAM:"
#define     MAIL_PATH_RAM_MNT           "/mnt/ram/"

#define     MAIL_IN_FILE                "ML000000%x.ml" //inbox
//#define     MAIL_MAIL_FILE              "ML01%4d%x.ml"  //mailbox
#define     MAIL_MAIL_FILE              "ML010000%x.ml"  //mailbox
#define     MAIL_DRAFT_FILE             "ML020000%x.ml" //draftbox
#define     MAIL_SEND_FILE              "ML030000%x.ml" //sendbox
#define     MAIL_OUT_FILE               "ML040000%x.ml" //outbox
#define     MAIL_FOLDER_FILE            "ML05%04d%x.ml"  //folder
#define     MAIL_CONFIG_FILE            "ML060000%x.ml" //config

#define     MAIL_FOLDER_FILE_HEAD       "ML05%04d"
#define     MAIL_FOLDER_FILE_TAIL       "%x.ml"

#define     MAIL_FILE_GET_LIST          "MLgetlist%x.ml"   //save get file list header
#define     MAIL_FILE_GET_TMPSAVE       "MLgetlisttmp.ml"  //save get file list header temp

#define     MAIL_IN_FILE_MATCH          "ML000000.ml"   //inbox match
#define     MAIL_MAIL_FILE_MATCH        "ML010000.ml"   //mailbox match
#define     MAIL_DRAFT_FILE_MATCH       "ML020000.ml"   //draftbox match
#define     MAIL_SEND_FILE_MATCH        "ML030000.ml"   //sendbox match
#define     MAIL_OUT_FILE_MATCH         "ML040000.ml"   //outbox match
#define     MAIL_FOLDER_FILE_MATCH      "ML05%04d.ml"    //folder match
#define     MAIL_CONFIG_FILE_MATCH      "ML060000.ml"   //config match

#define     MAIL_FILE_SEND_NAME         "MLsendenname.ml"   //generate encodeed mail file
                                                            //when one mail is to be sent
#define     MAIL_FILE_SEND_SIZE         "MLsendensize.ml"   //temp file for get mail size
                                                            //after mail encoded

#define     MAIL_CHARSET_UTF8           "UTF-8"
#define     WC_RECIPIENT                "PBRECIPIENTWNDCLASS"
#define     MAIL_ATTACH_FILE_NAME_HEAD  "mlgary" //"MLGARY"
#define     MAIL_ATTA_SAVE_EXTNAME      ".tmp"

//pictue and icon
#define     MAIL_ICON_NEW               "/rom/setup/sett_newmailbox_22x16.bmp"
#define     MAIL_ICON_ICON              "/rom/setup/sett_mailbox_22x16.bmp"
#define     MAIL_ICON_ACTI              "/rom/setup/sett_active_20x16.bmp"

#define     MAIL_ICON_SELECT            ICONSEL//"/usr/local/lib/gui/resources/select.bmp"
#define     MAIL_ICON_UNSELECT          ICONORMAL//"/usr/local/lib/gui/resources/unselect.bmp"

#define     MAIL_ICON_READ              "/rom/message/email/email_read.bmp"
#define     MAIL_ICON_READATT           "/rom/message/email/email_readatt.bmp"
#define     MAIL_ICON_UNREAD            "/rom/message/email/email_unread.bmp"
#define     MAIL_ICON_UNREADATT         "/rom/message/email/email_unreadatt.bmp"
#define     MAIL_ICON_UNRETRREAD        "/rom/message/email/email_unretrievedread.bmp"
#define     MAIL_ICON_UNRETR            "/rom/message/email/email_unretrieved.bmp"
#define     MAIL_ICON_UNRETRATT         "/rom/message/email/email_unretrievedatt.bmp"
#define     MAIL_ICON_DELETE            "/rom/message/email/email_delete.bmp"

#define     MAIL_ICON_PICTURE           "/rom/message/email/messaging_attach_jpg.bmp"
#define     MAIL_ICON_SOUND             "/rom/message/email/messaging_attach_wav.bmp"
#define     MAIL_ICON_NODE              "/rom/message/email/messaging_attach_txt.bmp"
#define     MAIL_ICON_VCAL              "/rom/pim/calendar/meeting.bmp"
#define     MAIL_ICON_OTHER             "/rom/message/email/messaging_attach_other.bmp"

#define     MAIL_ICON_LEFT              "/rom/public/arrow_left.ico"
#define     MAIL_ICON_RIGHT             "/rom/public/arrow_right.ico"

#define     GARY_FREE(p)                do { if(p) free(p); p=NULL;} while(0)

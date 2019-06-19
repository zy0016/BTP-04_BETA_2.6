/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : define the global const and struct etc.
 *            
\**************************************************************************/

#ifndef   MMSGLOBAL_
#define	  MMSGLOBAL_

#define  MMS_NOKEYBOARD
#define  LILY_DEBUG		0

#include "window.h"
#include "winpda.h"
#include "string.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "sys/statfs.h"
#include "fcntl.h"
#include "unistd.h"

#include "malloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "plx_pdaex.h"
#include "dialmgr.h"
#include "setting.h"
#include "mms/mmsuser.h"
#include "hpimage.h"

#include "imesys.h"
#include "phonebookext.h"
#include "pubapp.h"
#include "MmsCommon.h"
#include "msgunibox.h"
#include "dirent.h"
#include "mullang.h"
#include "PreBrowhead.h"
#include "plxdebug.h"
#include "sndmgr.h"
#include "plxmm.h"
#include "prioman.h"
#include "calendar/Calendar.h"
#include "PreBrow.h"

/******************************************************************** 
*
*			definination
*	   
**********************************************************************/
#ifndef STR_MMS_SNDFAILED
#define STR_MMS_SNDFAILED STR_MMS_EDITAPP
#endif

#define		MMSMSGWIN			printf//NullMsg 
#define		MsgWin				PLXTipsWin
//#define		MsgWin
#define     ASSERT(f)          ((void)0)
#define		GetMEIsSim			GetSIMState

#define		WM_MMS_NOTIFY		WM_USER + 10
#define		MMS_REMOVE_IMG      WM_USER + 34
#define		MMS_REMOVE_AUDIO    WM_USER + 35
#define     MMS_REMOVE_TEXT     WM_USER + 36
#define     MMS_REMOVE_OBJ      WM_USER + 37

#define		MSG_DIAL_RETURN		WM_USER + 100
#define		MAX_LISTNUM         6
#define		MAX_MSG_NUM         100
#define     MAX_MMS_LEN     1024*90             //change mms-size to 100K 2004.10.21
#define     MAX_MMS_SIZE    1024*100

// the max number of mms template
#define     MAX_TEMP_NUM        10


#define  INFO_ACCEPT            0
#define  INFO_HEAD              1
#define  INFO_REPORT            2
#define  INFO_RECEIVE           3
#define  INFO_SENT              4
#define  INFO_RETRIEVE          5

#define  EDIT_VIEW              0
#define  OTHER_VIEW             1
#define  RECEIVE_VIEW           2

#define  DISP_CONTENT_TOP		0   // 30		//Display 's top
#define  INTERVAL_BTN_TXT_IMG	2	//interval between text and pic

#define	 TOPSPACE				18
#define	 SCREEN_WIDTH			176
#define	 SCREEN_HEIGHT			150
#define  LIST_HEIGHT		    163

#define  SCROLL_WIDTH           20      //scroll width

#define ENG_FONT_WIDTH			7//8	//engish char 16 * 8
    
#define LIST_X					0//-1
#define LIST_Y					0//-1
#define LIST_WIDTH				SCREEN_WIDTH

#define  REPORTFILE             "/mnt/flash/message/dreport.mms"

#define	 MmsSetFile				"/mnt/flash/message/setting.mms"
#define  FILTER_NUM				20
#define  MMSFILTER_FILE			"/mnt/flash/message/3mmsfilt.dat"
#define  MMSDELAY_FILE			"/mnt/flash/message/mmsdelay.dat"

#define  RECVLEN				50
#define  MMSIDLEN				50


#define  MMS_WAITTIME           WAITTIMEOUT

// default timeout for per slide
#define  DEFAULT_DUR            2000  //ms

#define  MAX_SENDER_NUM         20
// ab name max length
#define  MAX_PBNAME_LEN        (AB_MAXLEN_NAME + 1)

#define  MAX_NUM_LEN           AB_MAXLEN_NAME
#define  MAX_MAIL_LEN          AB_MAXLEN_NAME

#define  MAX_ADDR_NUM           20//(SIZE_1/MAX_NUM_LEN)
#define  MAX_MAIL_NUM           (SIZE_1/MAX_MAIL_LEN)

#define  MAX_PHRASE_LEN         512

#define  FILE_TYPE_NUM          17

//file name types

//sent box
#define	 TYPE_SENDED            0x41      //send accomplishly			== A
//out box
#define	 TYPE_UNSEND            0x42      //send failure				== B

//in box
#define  TYPE_READED            0x43      //readed						== C
#define	 TYPE_UNREAD            0x44      //unreaded					== D

//draft box
#define  TYPE_DRAFT             0x45      //draft						== E

#define  TYPE_UNRECV            0x46      //receive failure				== F
#define  TYPE_DELAYRECV         0x47      //delay to receive			== G
#define  TYPE_READREPLY         0x48      //auto reply to the sender	== H
#define  TYPE_WAITINGSEND       0x49      //template					== I
#define  TYPE_SENDING           0x4A      //be sending					== J 

#define  TYPE_RECVING           0x4B      //be receiving				== K 
#define  TYPE_REPORTED          0x4C      //unreaded send report		== L 

#define  TYPE_DEFERMENT         0x4D      //readed send report			== M
#define  TYPE_FIRSTAOTORECV     0x4E          //auto receive at first time    ==N
#define  TYPE_DELAYSEND         0x4F       //delay send ==O
#define  TYPE_REPORTING			0x50		//pending report			==O
#define  TYPE_UNREPORT			0x51		//failed  report			==P

#define  MMS_TYPE_POS				1          //the positon of file name indicates the file type
#define  MMS_FOLDER_POS				2
#define  MMS_RESEND_POS				6

#define  MAX_FOLDERNUM          4
//file content masks; meaning just as above 
#define	 MMFT_SENDED            0x0001      
#define	 MMFT_UNSEND            0x0002  
#define  MMFT_READED            0x0004 
#define  MMFT_UNREAD            0x0008    

#define  MMFT_DRAFT             0x0010
#define  MMFT_UNRECV            0x0020      
#define  MMFT_DELAYRECV         0x0040     
#define  MMFT_READREPLY			0x0080  
#define	 MMFT_WAITINGSEND       0x0100     /* A message stored in outbox waitting for sending*/          
#define  MMFT_SENDING           0x0200   
#define  MMFT_RECVING           0x0400  
#define  MMFT_REPORTED			0x0800 
#define  MMFT_REPORTING			0x1000 
#define  MMFT_UNREPORT			0x2000
#define  MMFT_SUSPEND           0x4000    
#define  MMFT_FIRSTAOTORECV     0x8000 
#define  MMFT_DELAYSEND         0x10000    

#define  MMS_FILE_NAME          "*.*"      // MMS 

#define  MMS_HEADER             "%ZXL"      // mms head sign
#define  MMS_HEADER_LEN         5
#define  MMS_HEADER_SIZE        6

//define reciever address seperator
#define  MMS_ASEPSTR            ","
#define  MMS_ASEPSTRF			";"        
#define  MMS_ASEPCHR            ','
#define  MMS_ASEPCHRF           ';'


// define pic size 
#define  IMGLIMIT_X             320
#define  IMGLIMIT_Y             240

//image types:
#define  MAX_IMGTYPE_NUM        6

#define  IMG_UNKNOWN			0	//unknow image type
#define  IMG_GIF 				1	//gif
#define  IMG_JPEG				2	//jpeg
#define  IMG_BMP				3	//bmp
#define  IMG_WBMP				4	//wbmp
#define  IMG_PNG                5   //png

#define  MAX_TXTTYPE_NUM        4
#define  TEXT_UNKNOWN           0
#define  TEXT_PLAIN             1
#define  TEXT_VCALE             2
#define  TEXT_VCARD             3

#define  MAX_AUDIOTYPE_NUM      5

#define  AUDIO_UNKNOWN			0	//un know audio
#define  AUDIO_AMR				1	//amr
#define  AUDIO_MIDI				2	//midi (SMF)
#define  AUDIO_WAV				3	//wav
#define  AUDIO_MMF				4	//mmf (SMAF)

#define META_OTHER				0
#define META_AUDIO				2
#define META_AUDIO_AMR			21
#define META_AUDIO_MIDI			22
#define META_AUDIO_WAV			23
#define META_AUDIO_MMF			24
#define META_AUDIO_MP3			25
#define META_AUDIO_IMY			26//I_Melody
#define META_IMG				3
#define META_IMG_GIF			31
#define META_IMG_JPG			32
#define META_IMG_BMP			33
#define META_IMG_WBMP			34  
#define META_IMG_PNG			35  
/*
#define META_TEXT				4
#define META_TEXT_PLAIN			41
#define META_TEXT_VCALE			42  //"text/x-vCalendar" 0x06,    
#define META_TEXT_VCARD			43  //"text/x-vCard"     0x07,
#define META_REF				5
#define META_SMIL				6*/
#define META_REF            4
#define META_TEXT           5
#define META_TEXT_PLAIN     51
#define META_TEXT_VCALE     52  //"text/x-vCalendar" 0x06,    
#define META_TEXT_VCARD     53  //"text/x-vCard"     0x07,
#define META_SMIL           6



//charsets:
#define  CHARSET_ASCII			0
#define  CHARSET_UCS2			1
#define  CHAESET_SHIFTJI		2
#define  CHARSET_BIG5   		3
#define  CHARSET_ISO88591       4
#define  CHARSET_ISO88592       5
#define  CHARSET_ISO88593		6
#define  CHARSET_ISO88594		7
#define  CHARSET_ISO88595		8
#define  CHAESET_ISO88596		9
#define  CHARSET_ISO88597		10
#define  CHARSET_ISO88598		11
#define  CHARSET_ISO88599		12
#define  CHARSET_UTF8			13
#define  CHARSET_UNKNOWN		14

#define  SIZE_1                 513//512
#define  SIZE_2                 32//30
#define  SIZE_3                 10
#define  SIZE_4                  5
#define  SIZE_5                 256
#define  SIZE_6                 1980 //65*30 +29(;)
#define  SIZE_SUBJECT           40  //subject length

/* list type macro define */
#define  MMS_LIST_RECV			0	//inbox list
#define  MMS_LIST_SEND			1	//outbox list
#define  MMS_LIST_SENDED        2   //sent list
#define  MMS_LIST_DRAFT			3	//draft list
#define  MMS_LIST_REPORT        4   //delivery report list

#define  MMS_MAXNUM				20	//mms max number of Slide
#define  LMAXLEN				(SCREEN_WIDTH/ENG_FONT_WIDTH - 3)	//character number by per line
#define  LMAX_18030             (LMAXLEN * 2)
#define  TXTSPACE				18  //the pixel height of every line 18= 16(height) + 2(space)   

/* operation type */
#define  MMS_CALLEDIT_NEW		0	//new 
#define  MMS_CALLEDIT_OPEN		1	//edit draft
#define  MMS_CALLEDIT_REPLY		2	//reply mms
#define  MMS_CALLEDIT_REPLYALL	3   //reply all 
#define  MMS_CALLEDIT_TRAN		4	//forward mms
#define  MMS_CALLEDIT_IMAGE     5   //send image as mms
#define  MMS_CALLEDIT_SOUND     6   //send audio as mms
#define  MMS_CALLEDIT_TEXT      7   //send text as mms's text
#define  MMS_CALLEDIT_MOBIL     8   //mobile num as mms's receiver
#define  NOTI_CALLEDIT_REPLY	9	//回复notification
//#define  MMS_CALLEDIT_TEMPLATE  10  //send template
#define  MMS_CALLEDIT_TEMPOPEN  11  //edit template
#define  MMS_CALLEDIT_QTEXT     12   
#define  MMS_CALLEDIT_VCARD     13
#define  MMS_CALLEDIT_VCAL		14
#define  MMS_CALLEDIT_MULTIIMAGE 15

#define  MAX_PATH_LEN           256
#define  MAX_FILENAME			60
#define  MMS_FILEPATH			"/mnt/flash/mms/"
#define  MMSSET_FILEPATH		"/mnt/flash/mms/set"
#define  FLASHPATH              MMS_FILEPATH
#define  MMS_SETFILE            "mmsset.inf"
//there are only templates strored in rom
//they also canbe stored in flash
#define  ROMPATH				"/rom/"

#define  FLASH_IMGPATH          "/mnt/flash/picture/"
//"/mnt/flash/pim/image/"
#define  FLASH_AUDIOPATH        "/mnt/flash/audio/"
#define  FLASH_TEXTPATH         "/mnt/flash/notepad/"

#define  MMS_GATEWAY_IP			"10.0.0.172"
#define  MMS_MMSC_URL			"http://mmsc.monternet.com"
#define  MMS_PORT                9201

// string
#define STR_MMS_SPACE			ML("Space")
//MMS prompt info
#define  STR_MMS_MMS			ML("MMS")
#define  STR_MMS_CANCEL			ML("Cancel")
#define  STR_MMS_OK				ML("Ok")
#define  STR_MMS_YES            ML("Yes")
#define  STR_MMS_NO             ML("No")
#define  STR_MMS_BIG		    ML("The file is too large, please update")
#define  STR_MMS_NEGH			ML("Not enough space")
#define  STR_MMS_DIAL			ML("Dialing...")
#define  STR_MMS_RECVOK			ML("Receive succeed")
#define  STR_MMS_OVERNUM	    ML("The Number overflows")
#define  STR_MMS_EXIST			ML("The phrase has already existed")
#define  STR_MMS_PNUM			ML("Input Dest. address!")
#define  STR_MMS_ADDR			ML("Please reinput Dest. address")
#define  STR_MMS_SETIP			ML("Please config gateway")
#define  STR_MMS_SETURL			ML("Please setup MMSC URL")
#define  STR_MMS_SENDFAI		ML("Sending failed.")
#define  STR_MMS_SENDOK			ML("Message sent")
#define  STR_MMS_DELIVERY		ML("%s has already sent")
#define  STR_MMS_SELSAVE	    ML("Save to Drafts?")
#define  STR_MMS_DEL			ML("Delete surely")
#define  STR_MMS_RECVFAI		ML("Retrieving failed")
#define  STR_MMS_SAVEOK			ML("Saved to drafts")
#define  STR_MMS_CONTENTNULL	ML("The content is null!")
#define  STR_MMS_EXPIRED		ML("MMS is empired!")
#define  STR_MMS_OVERSIZE		ML("MMS too large. Please remove objects.")
#define  STR_MMS_NOSIM			ML("No SIM card")
#define  STR_MMS_FULLING		ML("The space of mms if fulling, please delete some")
#define  STR_MMS_IMGBIG			ML("The image is too large")
#define  STR_MMS_CANNOTSPACE	ML("Please input filename")
#define  STR_MMS_ERRFORMAT		ML("Error Format")
#define  STR_MMS_RENAME			ML("No file's name or this name is existed,please input again!")
#define  STR_MMS_FAILDRAW       ML("The picture is too big to show!")
#define  STR_MMS_INPUT          ML("Please input filename")
#define  STR_MMS_REPORT         ML("Report")
#define  STR_MMS_OK             ML("Ok")
#define  STR_MMS_EXIT           ML("Exit")
#define  STR_MMS_SLIDENEXT      ML("Next")
#define  STR_MMS_PLAYSLIDE      ML("Shuffle")
#define  STR_MMS_SAVEOBJECT     ML("Save Object")
#define  STR_MMS_STOP           ML("Stop")
#define  STR_MMS_PLAY           ML("Play") 
#define  STR_MMS_IMAGE          ML("Image")
#define  STR_MMS_WAITING        ML("Waiting...")
#define  STR_MMS_OPEN           ML("Open")
#define  STR_MMS_SAVE           ML("Save")
#define  STR_MMS_OBJECT         ML("MMS Objects")
#define  STR_MMS_DELETE         ML("Delete")
#define  STR_MMS_ATTATCH        ML("Attatch")
#define  STR_MMS_PREVIEW        ML("Preview")
#define  STR_MMS_INFO			ML("Details")
#define  STR_MMS_INSERT         ML("Insert")
#define  STR_MMS_PAILIE         ML("Arrange")
#define  STR_MMS_FRONTTXT       ML("Text first")
#define  STR_MMS_FRONTIMG       ML("Imgae first")
#define  STR_MMS_SOUND          ML("Sound")
#define  STR_MMS_DRAFTAPP		ML("Draft")
#define  STR_MMS_SLIDE			ML("Slide")
#define  STR_MMS_ADD            ML("Add")
#define  STR_MMS_SLIDEPRE       ML("Prev")
#define  STR_MMS_SLIDENEXT      ML("Next")
#define  STR_MMS_SEND           ML("Send")
#define  STR_MMS_CLEAR          ML("Clear")
#define  STR_MMS_SUBJECT        ML("Subject")
#define  STR_MMS_SELECT         ML("Select")
#define  STR_MMS_SPHONENUM      ML("Phone")
#define  STR_MMS_EMAIL          ML("Email")
#define  STR_MMS_CONTENTDEF		ML("<Content>")
#define  STR_MMS_DATE           ML( "Date:")
#define  STR_MMS_STATUS         ML("Status Report:")
#define  STR_MMS_STUEXPIRED     ML("Expired")
#define  STR_MMS_STURETRIEV     ML("Retrieved")
#define  STR_MMS_STUREJECT		ML("Rejected")
#define  STR_MMS_STUDEFER		ML("Deffered")
#define  STR_MMS_STUUNREC		ML("Unrecognized")
#define  STR_MMS_REPLYMMS		ML("Your message\r\nTo:%s\r\nSent:\r\n%04d/%02d/%02d %02d:%02d\r\nwas read")
#define  STR_MMS_CANANONY       ML("Hide")
#define  STR_MMS_SETNET         ML("Net Setting")
#define  STR_MMS_SETSEND		ML("Send Setting")
#define  STR_MMS_SETRECV		ML("Receive Setting")
#define  STR_MMS_SETFILTER      ML("Filter")
#define  STR_MMS_CANCEL         ML("Cancel")
#define  STR_MMS_OVERSENDNUM    ML("Over the number of address, please edit again!")
#define  STR_MMS_NULL           ML("The MMS is space!")
#define  STR_MMS_ATTEXIST		ML("This attatch is exist.")
#define  STR_MMS_TEXT           ML("Text")
#define  STR_MMS_REMOVECONFIRM  ML("Remove object?")
#define  STR_MMS_UNSUPPORT      ML("Unsupported file type.Object not added.")
#define  STR_MMS_SENDING        ML("Sending...")
#define  STR_MMS_EXPIRED		ML("MMS is empired!")
#define  STR_MMS_SUREREPLY      ML("Are you sure to send Read-Reply?")
#define  STR_MMS_RECCONFIRM     ML("New MMS available. Receive it now?")
#define  STR_MMS_SEND_NOSIM		ML("Sending failed. No SIM card")
#define  STR_MMS_OPENNING		ML("opening...")
#define  STR_MMS_IMAGESAVED		ML("Image saved")
#define  STR_MMS_PHONEMEMORY	ML("Phone memory")
#define  STR_MEMORY_CARD		ML("Memory card")
#define  STR_SELECT_DISTINATION	ML("Select destination")
#define  STR_SELECT_FOLDER	    ML("Select folder")
#define  STR_SAVING_FAILED		ML("Saving failed")
#define  STR_SAVED				ML("Saved")
#define  STR_RENAME_OBJECT		ML("Rename object")
#define  STR_MMS_SAVING			ML("Saving...")
#define  STR_MMS_PENDING		ML("Pending")
#define  STR_MMS_DELIVERED		ML("Delivered")
#define  STR_MMS_FAILED			ML("Failed")

#define ICON_OPTIONS        "Options"
#define ICON_SELECT         "Select"

//string end

#define MSG_CLASS_PERSONAL	"PERSONAL"
#define MSG_CLASS_AD		"ADVERTISEMENT"
#define MSG_CLASS_INFO		"INFORMATIONAL"
#define MSG_CLASS_AUTO		"AUTO"

#define MAX_MSGID_LEN       50
#define MAX_CLASS_NUM       8

// define result  
#define RETURN_FAI          0       // failure
#define RETURN_OK           1       // success

#define ERR_NOTFOUND        2       // not find the file
#define ERR_NOTMMS          3       // not MMS file
#define ERR_MALLOCFAI       4       // malloc failure
#define ERR_WRITEFILE       5       // write fail because of no enough space 

#define EDIT_CALL           5       //relative to mubox

#define UNREAD_ZERO         0       
#define UNREAD_DEC          1       
#define UNREAD_ADD          2       

#define ADDR_UNKNOWN        0
#define ADDR_MOBIL          1
#define ADDR_EMAIL          2

#define COUNT_DEC           1       
#define COUNT_ADD           2       
#define COUNT_SET           3       // set

#define MMS_NUM_TIP         10

/******************************************************************** 
*
*			定义数据结构
*	   
**********************************************************************/
//窗口对象和类名结构
typedef struct tagMmsWndClass
{
    WNDPROC  wndProc;
    char     szClassName[SIZE_2];
} MmsWndClass;

typedef struct tagMMSMSGINFO
{
    HWND    hWnd;
    UINT    msg;
    int    nType;
} MMSMSGINFO,*PMMSMSGINFO;


// handle到name的对应链表
typedef struct tagMSGHANDLENAME
{
    int  nStatus;		// mmft...
    BOOL isRom;
    char msgname[MAX_FILENAME];
	struct tagMSGHANDLENAME *pPre,*pNext;
} MSGHANDLENAME,*PMSGHANDLENAME;

// the struct of storageinfo, it will be sent to Unibox
typedef struct tagSTORAGEINFO
{
    int nCount;
    int nTotalSize;
}STORAGEINFO, *PSTORAGEINFO;

// the structure of all kinds of count
typedef struct tagMMSCOUNT
{
    int  nTotalnum;            // total mms number
    int  nUnread;              // unread mms number
    int  nUnreceive;           // unreceive mms number
    int  nUnconfirmed;         // unconfirm mms number
//    int  nInboxnum;            // inbox number
//    int  nReport;              // report number
//    int  nDraftnum;            // draft number
//    int  nOutboxnum;           // outbox number
//    int  nSentnum;             // sent nubmer
//    int  nMyFoldernum;            // my folder count
//    int  nFavorite;            // 
} MMSCOUNT;

typedef struct tagDREPORT
{
    char fileName[MAX_FILENAME];
    int  MsgType;
    char Version[SIZE_4];
    char msgId[MAX_MSGID_LEN];
    char to[SIZE_1];
    char Status[SIZE_2];
    SYSTEMTIME  getReportTime;
} DREPORT,*PDREPORT;

typedef struct NAMENODE{
    char  filename[MAX_FILENAME];
    struct NAMENODE *pNext;
} NAMENODE;

typedef struct
{
	int  homerec;		//mms reception 0 == never ; 1 == receive at once；2 == confirm
    int  visitrec;      //same as before
	char szIp[SIZE_2];	//网关IP地址
	char szUrl[SIZE_1*2];	//mmsc url 地址
	
    int nAnonymity;		//Allow anonymous		0 == no; 1 == yes
	int	nAd;			//Receive addverts		0 == no; 1 == yes
    int nReportRec;     //report reception      0 == off;1 == on
    int nReportSend;    //report sending        0 == off;1 == on
    int indexExpire;	//Message validity		0 == max;1 == 1h; 2 == 6h; 3 == 24h;4 == 1week
	int nImageSize;     //image size			0 == small; 1 == Big

	int indexReply;		//是否要求答复			0 == no reply;	1 == reply yes	
	int	nPriority;		//优先级				0 == high ; 1 == low; 2 == normal;

	unsigned long       ISPID;                      /*  ISP ID           */
    unsigned long       ProxyFlag;                  /*    */
    unsigned long       ProxyID;                    /*  proxy ID            */
    unsigned long       DtType;                     /*  GPRS/CSD    GPRS: 1, CSD: 0*/

    char    ISPName     [ UDB_K_NAME_SIZE + 1 ];    /*  ISP name               */
    char    ISPUserName [ UDB_K_NAME_SIZE + 1 ];    /*  ISP username       */
    char    ISPPassword [ UDB_K_PWD_SIZE  + 1 ];    /*  ISP password         */    
    char    ISPPhoneNum1[ UDB_K_TEL_SIZE+1 ];       /*  phonenumber   */    
    unsigned short  port;
}MMSSETTING;

typedef struct tagACCEPTINFO
{
    int         infoType;
    int         MsgType;
    char        TranId[SIZE_1];
    char        Version[SIZE_4];
    char        From[SIZE_1];
    char        To[SIZE_1];
    char        Subject[SIZE_1];
    char        MsgClass[SIZE_2];
    int         MsgSize/*[SIZE_3]*/;
    char        ExpireTime[SIZE_2];
    char        ConLocation[SIZE_1];
    SYSTEMTIME  acceptTime;
} ACCEPTINFO,*PACCEPTINFO;

//wsp head
typedef struct tagWSPHEAD
{
    char        ConType[SIZE_1];        //content-type
    char        from[SIZE_1];           
    char        to[SIZE_1];             
    char        cc[SIZE_1];             
    char        Bcc[SIZE_1];            
    char        subject[SIZE_1];        
    BOOL        bReport;                
    BOOL        bReply;                 
    int         expire;                 
    int         Priority;               
    int         nMmsSize;               
    SYSTEMTIME  date;                   
    int         nSlidnum;               
} WSPHead,*PWSPHead;

struct meta_node;
typedef struct meta_node MetaNode;

typedef struct meta_node
{    
    int  refCount;
    int  MetaType;  
    int  txtcharset;
    char *Content_Id;   
    char *Content_Type;
    char *Content_Location ;
    int  Metalen;       
    int  inSlide;            // in whick slide,if -1,then it is attatchment
    char *Metadata; 
    struct meta_node *pnext;
} mmsMetaNode;

typedef struct tagSLIDE
{
    int     dur;               
    
    mmsMetaNode *pImage;
    char    imgAlt[SIZE_2];    
    POINT   imgPoint;          
    SIZE    imgSize;           
    
    mmsMetaNode *pText;
    POINT   txtPoint;          
    SIZE    txtSize;           
    
    mmsMetaNode  *pAudio;    
} SLIDE,*PSLIDE;

typedef struct  tagMULTIMMS
{
    WSPHead     wspHead;            
    SLIDE       slide[MMS_MAXNUM];  
} MultiMMS,*PMultiMMS;

typedef struct  tagTOTALMMS
{
    MultiMMS    mms;
    mmsMetaNode *pAttatch;
} TotalMMS,*PTotalMMS;

typedef struct tagFOLDERINFO
{
	int nFolderID;
	int *FileType;
	int nMmsNum;
	int nUnreadNum;
    struct tagFOLDERINFO* pNext;
}FolderInfo, *PFolderInfo;

typedef void (CALLBACK *CARETPROC)(const RECT*);
extern CARETPROC WINAPI SetCaretProc(CARETPROC pNewProc);

typedef struct tagMMS_InfoData
{	
	HWND       hWndFrame;
	BOOL       bPre;
	BOOL       bNext;
	MMSMSGINFO MsgInfo;
	ACCEPTINFO AcceptInfo;
	PMSGHANDLENAME pmsgNode;
}MMS_INFODATA,*PMMS_INFODATA;

/******************************************************************** 
*
*			函数定义
*	   
**********************************************************************/
int sprintf(char *buffer, const char *format, ...);

int CalTxtSize(PCSTR pTxt, SIZE * pSize);

void CallDisplayWnd( TotalMMS * pmms, HWND hWndFrame, HWND hWndPre, UINT msg, UINT nType, 
					PMSGHANDLENAME pHandle, BOOL bPre, BOOL bNext);
BOOL CallMMSEditWnd(char *fileName, HWND hWndFrame, HWND hwnd, UINT msg, 
					UINT nKind, PSTR name, HINSTANCE hInst);
BOOL MMS_CreateSettingWnd(HWND hWndFrame, HWND hParent);
BOOL CreateListWnd(int nList);
void CallDisplayInfo(HWND hWndFrame, HWND hwnd, UINT msghandle, ACCEPTINFO AcceptInfo, 
					 BOOL bPre, BOOL bNext);

int MMS_GetSetting(MMSSETTING *mset);

BOOL MMSPro_Initial(void);
int MMSPro_PackSend(char* MmsSendName, TotalMMS tmms, BOOL bTran);
int MMSPro_Send(const char *mmsDraftFile);
BOOL MMSPro_IsInRSTable(const char *szFilename);
BOOL MMSPro_RecvDelay(const char *pName);
void MMSPro_ReadReply(const char *pName);
void MMSPro_ModifySetting(MMSSETTING *nset);
BOOL MMSPro_IsReply(void);
void MMSPro_RegisterMsg(HWND hWnd, UINT msg, UINT nType);
void MMSPro_UnRegisterMsg();
void MMSPro_Cancel(void);
BOOL MMSPro_IsIdle(void);
BOOL MMSPro_IsInRSChain(const char *szFilename);


BOOL IsFailureTableFull(void);
void GetQueFront(char *filename);
HWND MMSPro_GetHandle();
void DelQueFront();
void MMS_ParseAccept(const char *pAcceptBuf, int bufLen, PACCEPTINFO pAcceptInfo);
int ReadAndParse(const char* pszFileName,TotalMMS *dispMMS);
BOOL MMS_ReadMsg(HWND hWnd, PMSGHANDLENAME  pMsgNode, BOOL bPre, BOOL bNext);

//int strnicmp(char *first, char *last, unsigned int count);
int GetMsgType(int nStatus);
PMSGHANDLENAME GetNodeByHandle(DWORD msghandle);
void FreeMsgHandle(DWORD msghandle);
DWORD GetHandleByName(const char *filename);
DWORD AllocMsgHandle(const char *filename, int iStatus, BOOL isRom);
int GetCurFolder(void);
void ModifyMsgNode(DWORD msghandle, const char* filename, int iStatus);
BOOL FillMsgNode(MU_MsgNode* pmsgnode, BYTE msgtype, int iStatus, SYSTEMTIME date, 
                 DWORD  handle, PCSTR subject, PCSTR address, int storeType);
int MMS_GetRecNum(HWND hwndmu, DWORD msghandle);
extern BOOL CanMoveToFolder(void);

/*********************************************************************\
* Function     MMSC_InitCount
* Purpose      search all mms files
* Params       void
* Return       void
**********************************************************************/
void MMSC_InitCount(void);
/******************************************************************** 
* Function	   MMSPro_IsTempFull
* Purpose      
* Params	   void
* Return	   bool
**********************************************************************/
BOOL MMSC_IsTempFull(void);
/*********************************************************************\
* Function     MMSC_IsTotalFull
* Purpose      
* Params       void
* Return       bool
**********************************************************************/
BOOL MMSC_IsTotalFull(void);
/*********************************************************************\
* Function     MMSC_ModifyTotal
* Purpose      
* Params       nsign
* Return       void
**********************************************************************/
void MMSC_ModifyTotal(int nSign, int nCount);
/*********************************************************************\
* Function     MMSC_ModifyUnread
* Purpose      
* Params       int
* Return       void
**********************************************************************/
void MMSC_ModifyUnread(int nSign, int nCount);
/*********************************************************************\
* Function     MMSC_ModifyUnreceive
* Purpose      
* Params       int
* Return       void
**********************************************************************/
void MMSC_ModifyUnreceive(int nSign, int nCount);
/*********************************************************************\
* Function     MMSC_ModifyUnconfirmed
* Purpose      
* Params       int
* Return       void
**********************************************************************/
void MMSC_ModifyUnconfirmed(int nSign, int nCount);
/*********************************************************************\
* Function     MMSC_ModifyUnconfirmed
* Purpose      
* Params       int
* Return       void
**********************************************************************/
void MMSC_ModifyReportUnread(int nSign, int nCount);
/*********************************************************************\
* Function     MMSC_ModifyMsgCount
* Purpose      
* Params       nFolder: the id of the box
               nSign: how to modify
               nCount: if (nSign == COUNT_SET), this param is useful
* Return       void
**********************************************************************/
void MMSC_ModifyMsgCount(int nFolder, int nSign, int nCount);
/******************************************************************** 
* Function	   MMSPro_ModifyTempnum
* Purpose      
* Params	   nsign
* Return	   void
**********************************************************************/
void MMSC_ModifyTempnum(int nSign, int nCount);
/*********************************************************************\
* Function     mms_get_unconfirmed
* Purpose      
* Params       void
* Return        int 
**********************************************************************/
int mms_get_unconfirmed();
/*********************************************************************\
* Function     mms_get_unreceived
* Purpose      
* Params       void
* Return        int
**********************************************************************/
int mms_get_unreceived();
/*********************************************************************\
* Function     int mms_get_unread_msg_count()
* Purpose      
* Params       void
* Return        int
**********************************************************************/
int mms_get_unread_msg_count();
/*********************************************************************\
* Function     mms_get_message_count(int folder)
* Purpose      
* Params       void
* Return        int
**********************************************************************/
int mms_get_message_count(int folder);
/*********************************************************************\
* Function     mms_clear_unconfirmed
* Purpose      
* Params       void
* Return       void   
**********************************************************************/
void mms_clear_unconfirmed();
/*********************************************************************\
* Function     mms_clear_unreceived
* Purpose      
* Params       void
* Return       void
**********************************************************************/
void mms_clear_unreceived();
int mms_get_total_count(void);

BOOL mms_newmsg_continue(void);

#ifdef _PHOSPHOR_
    extern int GetSIMState();
#else
    extern int GetMEIsSim();
#endif

/* conditional free memory */
#define COND_FREE(p)  do { if(p) free(p); } while(0)

#if LILY_DEBUG
    void * __cdecl mymalloc( char* file, int line, size_t size);
    void   __cdecl myfree(void * p);
    void checknum();
    
    #define MMS_malloc(size_t)    mymalloc(__FILE__, __LINE__, size_t)
    #define MMS_free              myfree
#else
    #define MMS_malloc(size_t)    malloc(size_t)
    #define MMS_free(p)           COND_FREE(p)
#endif
	
#define mms_offsetof(type, member)  (UINT)(&((type * )0)->member)
//#define MMS_DEBUG
//#include "hpdebug.h"

#define MMS_SUBJECT
#endif

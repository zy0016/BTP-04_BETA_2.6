/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : define some consts in mmsdisplay.c
 *            
\**************************************************************************/

#ifndef   MMSDISPLAY_
#define   MMSDISPLAY_

#define  MUSIC_DEBUG            0

#define WM_BEGINDRAWIMG         WM_USER + 701       //begin to draw gif
#define WM_NEWNAME              WM_USER + 702
#define WM_IMGBIG               WM_USER + 703
#define WM_BEGINDRAWIMGOBJ      WM_USER + 704
#define WM_PLAYDELAY            WM_USER + 705

#define NEWNAME_IMAGE           0
#define NEWNAME_AUDIO           1
#define NEWNAME_TEXT            2
#define NEWNAME_TEXT_VCARD      3
#define NEWNAME_TEXT_VCALE      4
#define NEWNAME_OTHER           5

#define IDS_DISPLAY_EXIT        ML("Exit")
#define MMS_DISPLAY_APP         ML("MMS preview")
#define MMS_IDS_DISPLAY_BROKE	ML("Play stop")

#define MENU_DISPLAY_EXIT       701
#define IDB_EXIT                702
#define IDC_EDIT_NAME           703
#define IDC_DESTINATION			704
#define IDC_FOLDER				705
#define IDC_FORMVIEW_LIST		706

// main menu
#define IDM_GOTO_URL            711
#define IDM_ADDTO_BK            712
#define IDM_VIEW_MAP            713
#define IDM_SET_DEST            714
#define IDM_SAVE_POINT          715
#define IDM_WRITE_MSG           716
#define IDM_PLAY_SOUND          717
#define IDM_VIEW_IMG            718
#define IDM_MMS_REPLY           719
#define IDM_MMS_FORWARD         720
#define IDM_SAVE_CONTACT        721
#define IDM_MMS_OBJECT          722
#define IDM_MMS_FIND            723
#define IDM_MMS_HIDE            724
#define IDM_MOVETO_FOLD         725
#define IDM_MMS_INFO            726
#define IDM_MMS_DELETE          727

// write message...
#define IDM_WRITE_SMS           731
#define IDM_WRITE_MMS           732
#define IDM_WRITE_EMAIL         733

// reply...
#define IDM_REPLY_SENDER        741
#define IDM_REPLY_ALL           742
#define IDM_REPLY_SMS           743

// save to contacts...
#define IDM_SAVECONTACT_NEW     751
#define IDM_SAVECONTACT_ADD     752

// find...
#define IDM_FIND_PHONE          761
#define IDM_FIND_EMAIL          762
#define IDM_FIND_URL            763
#define IDM_FIND_COORD          764


#define NEWNAME_LEN             40

#define SCROLL_UP               -1      
#define SCROLL_DOWN             1       

#define IMGCLK_PATH_ENG         "/rom/message/unibox/click.bmp"
#define IMGCLK_PATH_CHN         "/rom/message/unibox/click_chn.bmp"
#define MMS_OBJIMG_PATH         "/rom/message/unibox/messaging_object_jpg.bmp"
#define MMS_OBJTXT_PATH         "/rom/message/unibox/messaging_object_txt.bmp"
#define MMS_OBJWAV_PATH         "/rom/message/unibox/messaging_object_wav.bmp"
#define MMS_LEFTARROW_PATH      "/rom/public/arrow_left.ico"
#define MMS_RIGHTARROW_PATH     "/rom/public/arrow_right.ico"
#define MMS_RIGHT2_PATH         "/rom/message/mms/right2.ico" 
#define MMS_RIGHTAUDIO_PATH     "/rom/message/mms/rightaudio.ico"
#define MMS_PHONE_MEMORY_PATH	"/rom/message/mms/mgm_phone_22x16 - a.bmp"
#define MMS_MEMORY_CARD_PATH	"/rom/message/mms/mgm_memorycard_22x16.bmp"
#define MMS_OPEN_FOLDER_PATH	"/rom/message/unibox/folder.bmp"
#define MMS_CHILD_FOLDER_PATH	"/rom/message/unibox/folder.bmp"

#define IMGCLK_WIDTH            56
#define IMGCLK_HEIGHT           64

#define  VIEW_CALLOBJ  0
#define  EDIT_CALLOBJ  1

#define  TIMER_PLAY_MULTI_SLIDE		1
#define  TIMER_PLAY_AUDIO			2
/******************************************************************** 
* 
*		data structure definition
* 
**********************************************************************/
typedef struct tagMMS_VIEWCreateData
{
	HWND       hWndFrame;
	PMSGHANDLENAME  pMsgHandle;
	HMENU      hMenu;
	HWND       hWndTextView;
	TotalMMS * pTmms;
	MMSMSGINFO DispMsgInfo;

	int        curSlide;
	BOOL	   bMusicPlay;

	int        bPre;
	int        bNext;
	HBITMAP    hBmpLeft;
	HBITMAP    hBmpRight;
	HBITMAP    hBmpRight2;
	HBITMAP    hBmpAudio;
}MMS_VIEWCREATEDATA,*PMMS_VIEWCREATEDATA;

typedef struct tagMMS_OBJCreateData
{
	HWND     hWndFrame;
	HBITMAP  hBmpObj[3];
    mmsMetaNode *pObj; 
	HMENU    hMenuObj;
    char     oldObjName[MAX_FILENAME];    
    BOOL     bPlayMusic;
}MMS_OBJCREATEDATA,*PMMS_OBJCREATEDATA;

typedef struct tagMMS_VIEWIMGCreateData
{
	HWND     hWndFrame;
    HWND     hWndCall;
	HMENU    hMenuImg;
    mmsMetaNode *pImg;
	HGIFANIMATE hGif;
	char     imgname[MAX_FILENAME];
}MMS_VIEWIMGCREATEDATA,*PMMS_VIEWIMGCREATEDATA;

typedef struct tagMMS_OBJOPENCreateData
{
    mmsMetaNode *pNode;
	HWND        hWndOpen;
	HWND        hWndFrame;
    
}MMS_OBJOPENCREATEDATA,*PMMS_OBJOPENCREATEDATA;

typedef struct tagMMS_InputCreateData
{
	HWND        hWndFrame;
	HWND        hWndCall;
	int         msg;
	char        oldName[MAX_FILENAME];
	char        nameShow[MAX_FILENAME];
	char        title[32];
    PCSTR       data;
	int         datalen;
}MMS_INPUTCREATEDATA,*PMMS_INPUTCREATEDATA;

typedef struct tagMMS_DirTree
{
	char    szName[256];
	struct tagMMS_DirTree* pChild;
	struct tagMMS_DirTree* pParent;
	struct tagMMS_DirTree* pNext;
	struct tagMMS_DirTree* pPre;
}MMS_DIRTREE, *PMMS_DIRTREE;

typedef struct tagMMS_DestinationCreateData
{
	HWND        hWndFrame;
	HWND		hWnd;
	int			msg;
	const char*		pData;
	int			len;
	HBITMAP		hBitmapPhone;
	HBITMAP		hBitmapMemory;
	int			mediaType;
}MMS_DESTINATIONCREATEDATA,*PMMS_DESTINATIONCREATEDATA;

typedef struct tagMMS_FolderCreateData
{
	HWND        hWndFrame;
	const char*		pData;
	int			len;
	HBITMAP		hBitmapOpen;
	HBITMAP		hBitmapChild;
	PVOID		pRoot;
}MMS_FOLDERCREATEDATA,*PMMS_FOLDERCREATEDATA;

/******************************************************************** 
* 
*		FUNC definition
* 
**********************************************************************/
BOOL CallMMSObjList(HWND hWndFrame, HWND hWndParent, mmsMetaNode *pObjMeta, int nCallType);

extern int CalTxtSize(PCSTR pTxt, SIZE * pSize);
extern BOOL DeleteAttatchByIndex(HWND hwnd, mmsMetaNode *p);
extern BOOL MMC_CheckCardStatus(void);
//extern BOOL SaveVcardToPhone(PB_RDB_ENTRY *pb_entry);
//extern BOOL SaveVcalendarToPhone(DateInfo *info);
#endif MMSDISPLAY_

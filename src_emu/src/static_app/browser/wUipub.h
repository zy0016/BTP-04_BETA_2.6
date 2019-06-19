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

#ifndef WIE_PUBLIC_H
#define WIE_PUBLIC_H

#include "PWBE.h"
#include "window.h"
#include "whtpuser.h"

#define MENU_SELECT                   ML("Select")
#define MENU_OPTION                   ML("Options")

#define IDS_OK                        ML("Ok")
#define IDS_YES                       ML("Yes")
#define IDS_NO                        ML("No")
#define STR_WAPWINDOW_CANCEL          ML("Cancel")
#define STR_WAPWINDOW_BACK            ML("Back")
#define STR_WAPWINDOW_SAVE            ML("Save")
#define STR_WAPWINDOW_EXIT            ML("Exit") 
#define STR_WAPWINDOW_OK              ML("Ok")

#define WML_GOTOURL               ML("Go to URL")
#define WML_REFRESH               ML("Refresh")
#define WML_SAVES                 ML("Save")
#define WML_PAGES                 ML("Pages")
#define WML_SETTINGS              ML("Settings")
#define WML_DISCONNECTACTTION     ML("Disconnect")
#define WML_DISCONNECTSUCCESS	  ML("Disconnected!")
#define WML_CONNECTACTTION        ML("Connect")
#define WML_PUSH				  ML("Push messages")
#define WML_EXITAPP               ML("Exit")
#define WML_BOOKMARK              ML("Bookmark")
#define WML_PAGE                  ML("Page")
#define WML_FILE                  ML("File")
#define WML_SHORTCUT              ML("Shortcut")
#define WML_GOHOMEPAGE            ML("Go to homepage")
#define WML_BOOKMARKS             ML("Bookmarks")
#define WML_PAGESAVED             ML("Saved pages")
#define WML_HISTORY               ML("History")
#define WML_CANCEL                ML("Cancel")
#define WML_CLEAR                 ML("Clear")
#define WML_GOTO                  ML("Go to")
#define WML_URLADD                ML("URL address:")
#define WML_BMSAVEFAILURE         ML("Cannot save:")
//#define WML_BMFULL                ML("Bookmark is full.")
//#define WML_BMFULL                ML("All bookmark have been used!")
#define WML_BK_NAME               ML("Title:")
#define WML_BK_NEWBOOKMARK        ML("New bookmark")
#define WML_EDIT                  ML("Edit")
#define WML_DELETE                ML("Delete")
#define WML_DELETED               ML("Deleted")
#define WML_REMOVE                ML("Remove")
#define WML_REMOVEALL             ML("Remove all")
#define WML_SEND                  ML("Send")
#define WML_SMS                   ML("Via SMS")
#define WML_MMS                   ML("Via MMS")
#define WML_MAIL                  ML("Via e-mail")
#define WML_SAVEPAGE              ML("Save page")
#define WML_DLFNAME               ML("Filename:")
#define WML_SAVEFILE              ML("Save file")
#define WML_NEWBOOKMARK           ML("New bookmark")
#define WML_OPEN                  ML("Open")
#define WML_HISTORY               ML("History")
#define WML_WAP                   ML("WAP")
#define WML_CONNECTORNOT          ML("Connect?")
#define WML_DISCONNECTORNOT       ML("Disconnect?")
#define WML_WAITING               ML("Waiting")
#define WML_FAILCONNECT           ML("Connecting failed")
#define WML_DETAILS               ML("Details") 
#define WML_CONNECTION            ML("Connection")
#define WML_CACHESIZE             ML("Cache size")
#define WML_ACCEPTCOOKIE          ML("Accept cookies")
#define WML_DISPLAYPIC            ML("Display pictures")
#define WML_PLAYSOUND             ML("Play sounds")
#define WML_OFF                   ML("Off")
#define WML_ON                    ML("On")
#define WML_ALL                   ML("All")
#define WML_DELALLORNOT           ML("Delete all saved pages?")
#define WML_SELPAGES              ML("Select pages")
#define WML_NOPAGES               ML("No pages selected")
#define WML_HISTORYTITLE          ML("History title:")
#define WML_URLADDRESS            ML("URL address:")
#define WML_REMOVEHISTORY         ML("Remove from history?")



#define WML_BK_ADDRESS            ML("Address")
#define STR_WAPWML_CAPTION            ML("WAP browser")
#define WML_ERRORWINDOW           ML("Create window error")
#define WML_CAPTION_REMOTE        ML("URL")
#define WML_CAPTION_HIS           ML("History")
#define WML_URL                   ML("Address")
#define WML_BACK                  ML("Back")
#define WML_STOP                  ML("Stop")
#define WML_HOMEPAGE              ML("Home page")
#define WML_BK_NULLDIS            ML("Not used")
#define WML_OFFLINE               ML("Offline")
#define WML_DIALING               ML("Dialing...")
#define WML_DIALSUC               ML("Online")
#define WML_DELHIS                ML("Delete all")
#define WML_CURRENT               ML("Current")
#define WML_MENU_1                ML("Edit")
#define WML_MENU_2                ML("Delete")
#define WML_MENU_3                ML("Browse")
#define WML_MENU_4                ML("Bookmark")
#define WML_MENU_5                ML("Setting")
#define WML_SCROLL_1              ML("Focus")
#define WML_SCROLL_2              ML("Link")
#define WML_SCROLL_3              ML("Link Background")
#define WML_SCROLL_4              ML("Active link")
#define WML_SCROLL_5              ML("Active background")
#define WML_SET_RESET             ML("Reset")
#define WML_SAVE                  ML("Save")
#define WML_CC                    ML("Cancel")
#define WML_MENU_BACK             ML("Back")
#define WML_GATEWAYFOUND          ML("Connecting...")
#define WML_CONNECT               ML("Connect OK")
#define WML_DATAEND               ML("Ready")
#define WML_GATEWAYNOTFOUND       ML("Gateway not found")
#define WML_SEARCHING             ML("Searching...")
#define WML_PROMPT                ML("Prompt")
#define WML_PROMPT_NOSIM          ML("No SIM card")
#define WML_PROMPT_EXIT           ML("Are you sure to exit?")
#define WML_ADDTOBM               ML("Add to bookmark")
#define WML_SET_CHANGECOLOR       ML("Change color")
#define WML_COLORL_TEXT           ML("Color Setup")
#define WML_COLOR_FOCUS           ML("Active link")
#define WML_SET_HPBTN             ML("Home page")
#define WML_SET_COLORBTN          ML("Colors")
#define WML_EXITDIALFAIL          ML("Disconnecting...")
#define WML_DL_FNAME_INPUT        ML("File name")
#define WML_DL_NOFNAME_ERR        ML("Please input file name.")
#define WML_DL_BADFNAME_ERR       ML("Invalid file name!")
#define WML_DL_USEDFNAME_ERR      ML("This name has been used.")
#define WML_DL_WRITEFILE_ERR      ML("Write file fail!")
#define WML_DL_TEMPFILE_ERR       ML("Data error")
#define WML_DL_UNSUPPORTFILE_ERR  ML("File type not supported")
#define WML_DL_FLASHFULLFILE_ERR  ML("No space to save this file")
#define WML_MENU_LOCAL            ML("File")
#define WML_MENU_SAVESOURCE       ML("Save page")
#define WML_AUTH_CAPTION          ML("Authentication")
#define WML_AUTH_REQLABEL         ML("Please input")
#define WML_AUTH_USER             ML("Username")
#define WML_AUTH_PWS              ML("PWD")
#define WML_AUTH_NOTALL           ML("Fill in all the information")
#define WML_DL_WRITEFILE_SUCCESS  ML("Save to My Doc")
#define WML_DEFAULT               ML("Default")
#define WML_INBOX_DELETE          ML("Delete")
#define WML_INBOX_INBOX           ML("Inbox")
#define WML_INBOX_DELETE_OK       ML("OK")
#define WML_INBOX_EXIT            ML("Exit")
#define WML_BK_PROMPT2            ML("Bookmark name is used")
#define WML_PROMPT_SET            ML("Setting is available after restart.")
#define WML_DL_TYPEFULLFILE_ERR   ML("Can't save this type of file any more!")
//#define WML_BMFULL                ML("All bookmark have been used!")
#define WML_BK_PROMPT1            ML("Please input name and address.")
#define WML_BK_PROMPT4            ML("Reserved,use another one please")
#define WML_SET_PROMPT1           ML("Reserved,use another one please")
#define WML_MEMOUT                ML("Can't save, memory insufficient!")
#define WML_OPTION                ML("Option")
#define WML_USECACHE              ML("Enable Cache")
#define WML_CACHSIZE              ML("Cache Pages")
#define WML_USECOOKIE             ML("Enable Cookie")
#define WML_COOKIESIZE            ML("Cookie Size")
#define WML_COOKIENUM             ML("Cookie Count")
#define WML_SET_ADVANCED          ML("Advance")
#define WML_DATAERROR             ML("Transfer Error")
#define WML_SHOWIMG               ML("Allow Showing Image")
#define WML_PLAYRING              ML("Allow Play Ring")
#define WML_SET_SHOW              ML("Shower Setting")
#define WML_SET_CACHE             ML("Cache Setting")
#define WML_SET_COOKIE            ML("Cookie Setting")
#define WML_PREVIEWCERT           ML("Preview Cert")
#define WML_DISCONNECT            ML("Hangup")
#define WML_SET_GPRS              ML("GPRS")
#define WML_SET_GPRSUP            ML("Data Sent:")
#define WML_SET_GPRSDOWN          ML("Data Receive:")
#define WML_SET_GPRS0             ML("Reset")
#define WML_BK_ERRURL             ML("URL is too long!")
#define WML_DL_NOSUPPORT_ERR      ML("Data form not supported!")
#define WML_ERR_UNKNOWNTYPE       ML("Unsupport file type.")
#define WML_ERR_UNKNOWNCHARSET    ML("Unsupport character set.")
#define WML_LOADING               ML("Loading...The left is %d.")
#define WML_LEFT                  ML("%2.2f K,waiting for %d secs.")
#define WML_ERR_FORMATERROR       ML("Format error.")
#define WML_ERR_ACCESSDENY        ML("Deny to access.")
#define WML_ERR_DEFAULT           ML("Unknown error.")
#define WML_LOAD                  ML("Loading...")
#define WML_ERR_SCRIPT            ML("Script is error.")
#define WML_ERR_PAGETOOLARGE      ML("The page is too large.")
#define WML_MENU_BROWSER          ML("Browse")
#define WML_BK_NONAME             ML("Please input name.")
#define WML_BK_NOURL              ML("Please input address.")
#define WML_CANNOTSAVE			  ML("Cannot save:")
#define WML_NOTDEFALLDATA		  ML("Please define all data")
#define WML_NAMEUSEDOVER		  ML("Filename already exists. Overwrite?")
#define WML_BMSAVESUCCESS		  ML("Save successful!")
#define WML_NOHISTORY			  ML("No history")
#define WML_EMPTYHISTORY		  ML("Remove history?")
#define WML_SCSAVETOMENU		  ML("Save shortcut to Menu?")
#define WML_INPUTEDIT			  ML("Input Edit")
#define WML_BMNAMEUSED			  ML("Bookmark already exists. Overwrite?")
#define WML_PUSHCAPTION			  ML("Push messages")
#define WML_PUSH_OPEN			  ML("Open")
#define WML_PUSH_DETAIL			  ML("Details")
#define WML_PUSH_DELETE			  ML("Delete")
#define WML_PUSH_DELETEALL		  ML("Delete all")
#define WML_NOSAVEPAGE			  ML("No saved pages")
#define WML_NOPUSHMESSAGE		  ML("No push messages")
#define WML_DELETEPUSHONE		  ML("Delete message?")
#define WML_DELETEPUSHALL		  ML("Delete all messages?")
#define STRWMLDETAILPUSHURL			  ML("URL address")
#define STRWMLDETAILPUSHTITLE		  ML("Title")
#define STRWMLNOCONNECT				  ML("None")
#define WML_NOTDEFTITLE			  ML("Please define title")
#define WML_NOTDEFURL			  ML("Please define URL address")
#define WML_NOTDEFNAME			  ML("Please define name")
#define WML_CLEARINPUT			  ML("Clear")
#define WML_WRONGTYPE			  ML("Wrong file type")
#define WML_DELSELECTSIGORNOT        ML("Delete selected page?")
#define WML_DELSELECTMULORNOT        ML("Delete selected pages?")
#define WML_SAVEFAILED				ML("Saving failed")
#define WML_BMFULL					ML("Too many bookmarks")
#define WML_ROMFULL					ML("Not enough memory")
#define WML_REMOVED					ML("Removed")
#define WML_DISCONNECTING			ML("Disconnecting...")
#define WML_STFULL					ML("Too many shortcut")




#define _MAX_PATH                     256
#define WML_DEBUG
#define SMARTPHONE

/* URL类型 */
#define		URL_NULL		  0      //空类型
#define		URL_LOCAL		  1      //本地访问
#define		URL_REMOTE		  2      //远程请求
#define		URL_EMPTY_COMMAND 4 

/**************************************************************
位置宏
**************************************************************/
#define URLNAMELEN                    (512*2)
#define WIESAVEAS_EDITLIMIT           40              //文件名称长度
#define WIESAVEAS_MAXFTAIL_LEN        10             //文件尾长度
#define WIESAVEAS_MAXFNAME_LEN        (WIESAVEAS_EDITLIMIT+WIESAVEAS_MAXFTAIL_LEN+1)
#define CLT_HEIGHT                    150  
#define CLT_WIDTH                     PLX_WIN_WIDTH
#define WIE_LISTBOX_POS               0, 0, 172, 146

#define WIE_MEDIT_WIDTH               (CLT_WIDTH-40)
#define WIE_MEDIT_HEIGHT			  120
#define WGO_BTMBTNY                   128
#define WIE_SEDIT_HEIGHT              20
#define WIE_SEDIT_WIDTH               (CLT_WIDTH-60)
#define WIE_LEDIT_WIDTH               176
#define WIE_GWEDIT_WIDTH              WIE_MEDIT_WIDTH
#define WIE_MBTN_HEIGHT               26
#define WIE_MBTN_WIDTH                80
#define WIE_LBTN_WIDTH				  154
#define WIE_LBTN_HEIGHT               20
#define WIE_SBTN_HEIGHT				  20
#define WIE_SBTN_WIDTH				  50    
#define WIE_LSPIN_WIDTH               160

#define WML_SAVESOURCE

//#ifdef _EMULATE_
//#define FLASHPATH                     "FLASH2:\\"
//#define FLASHNAME                     "FLASH2"
//#define WIECONFIG_FILENAME            "FLASH2:\\WIECFG3.cfg"
//#define WIE_BM_FILENAME               "FLASH2:\\WIEBMark.wbm"
//#define WIE_HIS_FILENAME              "FLASH2:\\WIEHIS.wht"
//#define LOCALPATH                     "file://flash2"
//#else

#define FLASHPATH                     "/mnt/flash/wapbrowser/"
#define FLASHNAME                     "FLASH"
#define WIECONFIG_FILENAME            "/mnt/flash/wapbrowser/WIECFG3.cfg"
#define WIE_BM_FILENAME               "/mnt/flash/wapbrowser/WIEBMark.wbm"
#define WIE_HIS_FILENAME              "/mnt/flash/wapbrowser/WIEHIS.wht"
#define LOCALPATH                     "file://flash2"
#define FILE_PUSH_WAP				  "/mnt/flash/wapbrowser/WIEPush.wpm"
#define FILE_PUSH_BACKUP			  "/mnt/flash/wapbrowser/WIEPushbk.wpm"
#define  WIE_DOCDIR					  "/mnt/flash/wapbrowser/mydata/"
//short cut
#define WIE_CUTDIR					  "/mnt/flash/wapbrowser/shortcut/"
#define WIE_IMG_NOTACTIVE			  "/rom/wml/not_available.bmp"
//push message
#define WIE_IMG_READPUSH			  "/rom/wml/push_read.bmp"
#define WIE_IMG_UNREADPUSH			  "/rom/wml/push_unread.bmp"
//#endif

#define NULL_PORT                     0
#define MAX_PORT                      65535

#define CN_NONE                       -1
#define CN_NOCONNECT                  0    
#define CN_CONNECT                    1

//////////////////////////////////////////////////////////////////////////

#define		MAX_FILE_NUM							500
#define		DATA_FILE_NUM						200

//////////////////////////////////////////////////////////////////////////

#define		CX_FITIN	1
#define		CY_FITIN	3

//////////////////////////////////////////////////////////////////////////

/**
 * @ Save File To Flash Max Value
 */

#define		MAX_MOVIE_NUM						5
#define		MAX_RING_NUM							100
#define		MAX_PICTURE_NUM					200
#define		MAX_PHOTO_NUM						200
#define		MAX_PORTRAIT_NUM					500
#define		MAX_RECORD_NUM						10
#define		MAX_DOCUMENT_NUM					30
#define		MAX_MP3_NUM							50
#define		MAX_PUSH_NUM						100

/**
 * @ Other Define 
 */

#define		FILE_NAME_LEN						100
#define		FILE_NAME_WITHOUT_PATH			40		

#define		Dim(x)									(sizeof(x)/sizeof(x[0]))

/**
 * @ 	Medium Type
 */
typedef	enum	MediumType
{
	enRom,
	enFlash,
	enSDCard,
	enUDisk,
}MEDIUM_TYPE;

/**
 * @ 	Data Type
 */

typedef	enum	DataType
{
	enMovie,
	enRing,
	enPicture,
	enPhoto,
	enPortrait,
	enRecord,
	enDocument,
	enMP3,
	enAllSupport,
}DATA_TYPE;

/**
 * @ 	File Or Dir But Now It's no using in Appollo 
 */

enum	FileOrDir
{
	enDir = 1,
	enFile,
};

/**
 * @ Save Type
 */

typedef	enum	SaveType
{
	enSaveFile = 0,
	enSaveBuf,

}SAVE_TYPE;

/**
 * @ 	File Type For Individual Part Such as Movie,Ring,Picture etc.
 */

typedef	struct	FileType
{
	DATA_TYPE	DataType;
	int			iMaxNum;
	char			*szFileType[6];
}FILE_TYPE;

/**
 * @ 	File Property
 */

typedef	struct	FileProperty
{
	char	szName[FILE_NAME_LEN];
	char	szDate[30];
	char	szTime[20];
	char	szSize[20];
	char	szFormat[10];
	char	szWidth[10];
	char	szHeight[10];
	BOOL	bForward;

}FILE_PROPERTY;

/**
 * @ 	File Struct
 */

typedef	struct	FilePath
{
	DATA_TYPE	DataType;
	char			*szFilePath;
}FILE_PATH;

/**
 * @ 	Medium Struct
 */

typedef	struct	MediumPath
{
	MEDIUM_TYPE		DataType;
	char				*szMediumPath;
}MEDIUM_PATH;

/**
 * @ 	iProperty: Indicator is file or directory
 * @ NameWithPath:File Name inlcue directory				
 */
typedef	struct	DataFile
{
	int		iProperty;			
	char		NameWithPath[FILE_NAME_LEN];
}DATA_FILE;

/**
 * @ 	For Searching Directory
 */

typedef	struct	SearchDirectory
{
	BOOL		flag;
	char		SearchDir[100];
}SEARCH_DIRECTOEY;

/**
 * @ 	Save File Struct
 */

typedef	struct	SaveFile
{
	DATA_TYPE	DataType;
	SAVE_TYPE	SaveType;
	long 			nLen;
	char			szFileName[60];
	BYTE			*pDataBuf;
}SAVE_FILE;

//////////////////////////////////////////////////////////////////////////


extern char szHomePage[URLNAMELEN];
extern int nHomepageType;
extern int curUrlType;
extern unsigned short nCPort;
extern int DialSuccessFlag;
extern HBITMAP hwmlnsbmp, hwmlnsbmpfs, hwmlnsbmpps;

typedef struct tagWMLCFGFILE
{
    char sHomepage[URLNAMELEN];    //主页;
    int nhomepagetype;    
    Color_Scheme uColorCfg;  //色彩配置    
    int nCacheSize;          //缓存大小
    CACHEMODE CacheMode;     //缓存模式    
    int nCookieMode;         //Cookie模式
    int nCookieCount;        //Cookie最大数目
    int nCookieSize;         //Cookie最大空间    
    BOOL bShowImg;           //是否显示图片
    BOOL bPlayRing;          //是否播放背景音乐
	unsigned long ISPID;	 //ISP的ID号
	char nPort[5];			 //连接的端口号
	char sGateWay[32];			 //连接的IP地址
	char szConnectionname[32];	//连接方式的名字
} WMLCFGFILE;

HWND GetWAPFrameWindow();
HMENU GetWAPMainMenu();

int SetCurentPage(char *, int ntype);
int GetCurentPage(char *, int *ptype);
int GetConnectType(int);
BOOL GetDialSucFlag();
int LocalFileUrlStd(char *szFileUrl);
void Wml_ONCancelExit(HWND hParent);
BOOL JudgetoStandardUrl(char *dst, char *src, int *type);
BOOL isLocalFile(char *szUrl);
char* StrDup(const char* SrcStr);
void WAP_GetCurConfig(WMLCFGFILE * curconfig);
void WAP_SetCurConfig(WMLCFGFILE * curconfig);
void WCFG_Close_Window();
void WML_BackOrExit(int bPrev, BOOL bHis);
void RedrawScrollBar(HWND hWnd, int nTotalLines, int nLinePos, int nLinesPerPage);
BOOL WIE_CheckFileName(const char *FILENAME, unsigned int nNameLen);
BOOL CheckFileExist(const char* DIR, const char * FILENAME);
BOOL WML_DeleteSpace( char* s );
int UI_Message(unsigned int MsgID, unsigned long Param1, unsigned long Param2);
void WML_GotoCloseWindow(void);
void WML_SaveCloseWindow(void);
void WML_HisCloseWindow(void);
void WML_SetCloseWindow(void);
void WML_PushCloseWindow(void);

#endif

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PIM
 *
 * Purpose  : preview pictures
 *            
\**************************************************************************/
#ifndef _PICBREOWSERHEADFILE_
#define	_PICBREOWSERHEADFILE_

#include	"PreBrow.h"
#include	"hpimage.h"
#include	"PreBrowHead.h"
#include	"callapp/MBPublic.h"
#include	"mullang.h"
#include	"pubapp.h"
#include	"MmsUi.h"
#include "setup/setup.h"
#include "log.h"
#include "plx_pdaex.h"
#include "time.h"
#include "pmi.h"
#include "bluetooth\BtSendData.h"
#include "assert.h"


//#define TRACE       printf
/************************************************************************/
/* 消息参数宏                                                           */
/************************************************************************/

#define		IDC_NULL			(WM_USER+99)
#define		IDC_PREPIC_LIST		(WM_USER+100)
#define		IDC_OK				(WM_USER+101)
#define		IDC_BACK			(WM_USER+102)
#define		IDC_OPEN			(WM_USER+103)
#define		IDC_SEND			(WM_USER+104)
#define		IDC_SENDMANY		(WM_USER+105)
#define		IDC_DELETE			(WM_USER+106)
#define		IDC_DELETEMANY		(WM_USER+107)
#define		IDC_RENAME			(WM_USER+108)
#define		IDC_MOVETOFOLDER	(WM_USER+109)
#define		IDC_MOVEMANY		(WM_USER+110)
#define		IDC_CREATEFOLDER	(WM_USER+111)
#define		IDC_WALLPAPER		(WM_USER+112)
#define		IDC_SHOWDETAIL		(WM_USER+113)
#define		IDC_EXIT			(WM_USER+114)
#define		IDC_PROCESSDEL		(WM_USER+115)

#define		IDC_VIAMMS			(WM_USER+116)
#define		IDC_VIABLUETOOTH	(WM_USER+117)
#define		IDC_VIAEMAIL		(WM_USER+118)


#define		IDC_ROTATE			(WM_USER+119)
#define		IDC_ZOOMIN			(WM_USER+120)
#define		IDC_ZOOMOUT			(WM_USER+121)
#define		IDC_FULLSCREEN		(WM_USER+122)
#define		IDC_NORMALSCREEN	(WM_USER+123)

#define		IDC_SENDSELECT		(WM_USER+124)
#define		IDC_DELSELECT		(WM_USER+125)
#define		IDC_MOVESELECT		(WM_USER+126)

#define		IDC_SENDALL			(WM_USER+127)
#define		IDC_DELALL			(WM_USER+128)
#define		IDC_MOVEALL			(WM_USER+129)

#define		IDC_CLOCKWISE		(WM_USER+130)
#define		IDC_COUNTERCLKWISE	(WM_USER+131)

#define		IDC_SET				(WM_USER+132)
#define		IDC_CLEAR			(WM_USER+133)
#define		IDC_COPY			(WM_USER+151)
#define		IDC_COPYMANY		(WM_USER+152)
#define		IDC_COPYALL			(WM_USER+153)
#define		IDC_COPYSELECT		(WM_USER+154)

#define		IDRM_DELALL			(WM_USER+134)
#define		IDRM_DELSEL			(WM_USER+135)
#define		IDRM_MOVEALL		(WM_USER+136)
#define		IDRM_OPENING		(WM_USER+137)
#define		IDRM_EDITMMS		(WM_USER+138)
#define		IDRM_MOTOFOLDER		(WM_USER+140)
#define		IDRM_COPYTOFOLER	(WM_USER+142)
#define		IDRM_DEFOTHERNAME	(WM_USER+143)
#define		IDRM_COPYALL		(WM_USER+144)

#define		IDRM_NOMEMORY		(WM_USER+150)

#define		IDRM_CLOSEWINDOW	(WM_USER+141)
#define		IDM_DEFNAMESU		(WM_USER+151)

#define		PIC_LOADBMP			(WM_USER+152)

#define		IDT_RADIOBUTTON		1

#define		RBTIMEOUT			500
/************************************************************************/
/* Control ID                                                           */
/************************************************************************/
#define		ID_MULTISLELIST		100
/*************************************************
	字符串定义宏
*************************************************/
#define		IDS_OPENING		ML("Opening...")
#define		IDS_CAPTION		ML("Pictures") //GetString(STR_PREPIC_CAPTION)
#define		IDS_BACK		ML("Back")//GetString(STR_WINDOW_BACK)
#define		IDS_EXIT		ML("Exit")
#define		IDS_CANCEL		ML("Cancel")
#define		IDS_NOPIC		ML("No pictures")
#define		IDS_OPEN		ML("Open")
#define		IDS_SEND		ML("Send")
#define		IDS_SENDMANY	ML("Send many")
#define		IDS_DELETE		ML("Delete")
#define		IDS_DELETEMANY	ML("Delete many")
#define		IDS_RENAME		ML("Rename")
#define		IDS_ROTATE		ML("Rotate")
#define		IDS_MOVETOFOLDER	ML("Move to folder")
#define		IDS_MOVEMANY		ML("Move many")
#define		IDS_CREATEFOLDER	ML("Create folder")
#define		IDS_WALLPAPER		ML("Wallpaper")
#define		IDS_SHOWDETAIL		ML("Details")
#define		IDS_RNMNOTIFY		ML("Please define name")
#define		IDS_FOLDERNAME		ML("Folder name:")
#define		IDS_NEWNAME			ML("Picture name:")
#define		IDS_YES				ML("Yes")
#define		IDS_NO				ML("No")
#define		IDS_SAVE			ML("Save")
#define		IDS_OK				ML("Ok")
#define		IDS_VIAMMS			ML("Via MMS")
#define		IDS_VIABLUETOOTH	ML("Via Bluetooth")
#define		IDS_VIAEMAIL		ML("Via e-mail")
#define		IDS_FULLSCREEN		ML("Full screen")
#define		IDS_NORMALSCREEN	ML("Normal screen")
#define		IDS_ZOOMIN			ML("Zoom in")
#define		IDS_ZOOMOUT			ML("Zoom out")
#define		IDS_DELSELPIC		ML("Delete selected pictures?")
#define		IDS_SELECT			ML("Select")
#define		IDS_ALLINFOLDER		ML("All in folder")
#define		IDS_CLKWISE			ML("Clockwise")
#define		IDS_COUCLKWISE		ML("Counterclockwise")
#define		IDS_SET				ML("Set")
#define		IDS_CLEAR			ML("Clear")
#define		IDS_DELFOLDER		ML("Delete folder and contents?")
#define		IDS_DELETING		ML("Deleting...")
#define		IDS_DELETED			ML("Deleted")
#define		IDS_SELPIC			ML("Please select pictures")
#define		IDS_MOVE			ML("Move")
#define		IDS_DEFOTHERNAME	ML("Please define another name")
#define		IDS_SELPICCAPTION	ML("Select pictures")
#define		IDS_MEMORYFULL		ML("Not enough memory")
#define		IDS_WALLPAPERSET	ML("Wallpaper set")
#define		IDS_WALLPAPERCLEAR	ML("Wallpaper cleared")
#define		IDS_RENAMEPICTITLE	ML("Rename picture")
#define		IDS_COPY			ML("Copy")
#define		IDS_COPYMANY		ML("Copy many")
#define		IDS_SELDEST			ML("Select destination")
#define		IDS_PHONEMEM		ML("Phone memory")
#define		IDS_MMCMEM			ML("Memory card")
#define		IDS_COPING			ML("Copying...")
#define		IDS_MOVING			ML("Moving...")
#define		IDS_SELECTFOLDER	ML("Select folder")
#define		IDS_MOVED			ML("Moved")
#define		IDS_SUBFOLDER		ML("Folder: ")
#define		IDS_PICNUM			ML("Pictures: ")

#define		RIGHTARROWICON	"/rom/public/arrow_right.ico"
#define		LEFTARROWICON	"/rom/public/arrow_left.ico"
#define		FOLDERPATH	"/rom/progman/app/mdefaultfolder_43x28.bmp"

#define		SUBFOLDER	"/rom/message/unibox/folder.bmp"	//***********// Messaging_F_Folder_22x16.bmp
#define		OPENFOLDER	"/rom/message/unibox/newfolder.bmp"	//**********// Messaging_Open_Folder_22x16.bmp
#define		MMCICON		"/rom/pim/mgm_memorycard_22x16.bmp"
#define		PHONEICON	"/rom/pim/mgm_phone_22x16.bmp"
#define		DEFAULTPICTURE	"/rom/pim/miniature.bmp"//"/rom/progman/desk1.bmp"
//#define		PICMMCICON	"/rom/progman/app/mgmemorymngt_43x28.bmp"
#define		PICMMCICON	"/rom/pim/na_43x28.bmp"

//#define RADIO_OFF   "ROM:ime/radiooff.bmp"
//#define RADIO_ON    "ROM:ime/radioon.bmp"

#define RADIO_OFF   "/rom/public/radiooff.bmp"
#define RADIO_ON    "/rom/public/radioon.bmp"


/*************************************************
	全局变量宏
*************************************************/
#ifndef _EMULATE_
extern BOOL		MMC_CheckCardStatus();
#define		PHONEPICFILEDIR		"/mnt/flash/pictures"
#define		MMCPICFOLDERPATH	"/mnt/fat16/pictures"
#define		MMCPICFOLDERPATHEX	"/mnt/fat16/pictures/"
#else
#define		MMC_CheckCardStatus()	1
#define		PHONEPICFILEDIR		"/mnt/flash/pictures"
#define		MMCPICFOLDERPATH	"/mnt/flash/mmc/pictures"
#define		MMCPICFOLDERPATHEX	"/mnt/flash/mmc/pictures/"
#endif

#define BUF_SIZE 1024
#define	KICKDOGSIZE	100


//"/mnt/flash/mmc/picture"//"/mnt/fat16/picture/"

/*@**#---2005-07-21 14:25:59 (mxlin)---#**@
#define		PREBROW_MAX_GRPNAMELEN	50
#define		PREPIC_MAXGRP			4

// #define		PREPIC_DIRGRP0		"ROM:/prefab/image/"
#define		PREPIC_DIRGRP0		"/mnt/flash/picture/"
#define		PREPIC_DIRGRP1		"/rom/game/pao/"
#define		PREPIC_DIRGRP2		"/mnt/flash/pim/image/"
#define		PREPIC_DIRGRP3		"/mnt/flash/image/"*/

/***********
Function declare
*********************************************/
HBITMAP	AdjustBmpToScreen(HBITMAP	hSrcBmp, int x, int y, int width, int height, PCSTR pFileName);
BOOL	DefAnotherName(HWND hFrame, HWND hRecWnd, PSTR pOldName, PSTR pNewName, PSTR pOnlyName,UINT uMsg, DWORD dReserved);
VOID	GetLastPath(PSTR	szPath, PSTR pNewPath);
BOOL	IsBigFileFromSD(PCSTR pFile);
int		GetSubFolderNum(PCSTR szFolderPath);
BOOL	loadBitmapForList(HWND hPicList, int nPos, PLISTBUF pBuf);
BOOL	PicSetWallPaper(HWND hWnd);
BOOL	PicCopyFile(PCSTR pOldFileName, PCSTR pNewFileName);
BOOL	PicMoveFile(PCSTR pOldFileName, PCSTR pNewFileName);
long	lmxDifftime(unsigned long tm1, unsigned long tm2);
BOOL	SetWallPaper(HWND	hWnd);
BOOL	DeleteDirectory(PCSTR	pPath, BOOL b);
BOOL	ListFolder(HWND hRecmsg, UINT rMsg, BOOL bPhone);
BOOL	PicBrowser_Edit(HWND	hParentWnd, PSTR szName, PSTR szFullName, BOOL bFolder);
BOOL	ShowPicDetail(HWND hFrame, PCSTR cFileName, PCSTR cFileFullName, int	nFileType, BOOL bPopup);
BOOL	GetInfoFromListByIndex(HWND hList, PLISTBUF ListBuffer, char* cFileName, char* szFullFileName, int *nFileType, int index);
BOOL	ProcessMultiSel(HWND hFrame, HWND hMsg, PLISTBUF pList, DWORD wID, HWND hList);
int		AddFolderToList(HWND hList, PCSTR pFilePath, HBITMAP hOpen, HBITMAP	hNormal, PLISTBUF FolderList);
BOOL	PicMoveToFolder(HWND hRecmsg, UINT rMsg);
BOOL	SavePictureFromHandle(HDC hdc, HBITMAP hBmp, PSTR pFileName);
BOOL	BrowsePicture(HWND hList, HWND hParentWnd, PVOID pImgData, DWORD dwDataSize, PSTR pFileFullName, PLISTBUF pListBuf, BOOL bFullScreen, DWORD dFileType, HBITMAP hBmp);
extern BOOL BtSendData(HWND hAppFrameWnd,char* szAppPathName,char* szViewFileName,int iFileType);
extern BOOL SetBKPicture(char* szPicFile);
extern BOOL MAIL_CreateEditInterface(HWND hFrameWindow, char *PSZRECIPIENT, char *PSZATTENMENT, char *PSZATTNAME, BOOL bAtt);
extern BOOL GetMMCName(char *name);
extern	void CallIdleBkPicChange(void);
BOOL	AddMultiSelPicToList(PLISTATTNODE *pHead, PCSTR pName, PCSTR pPath);
void	FreeMultiSelList(PLISTATTNODE *pHead);
BOOL	Image_LoadMiniature(int	nTopindex, int nPos, HWND hPicList, PLISTBUF pbuffer);
BOOL	IsValidFileorFolderName(char * fName);

#endif

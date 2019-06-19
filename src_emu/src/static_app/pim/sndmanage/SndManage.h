/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PIM
 *
 * Purpose  : sound application
 *            
\**************************************************************************/
#ifndef _SOUND_APP_H_
#define	_SOUND_APP_H_
/*
#ifndef _SOUND_SYSICONS_
#define _SOUND_SYSICONS_	// signal and power icon displayed
#endif
//*/
#undef  _SOUND_SYSICONS_	// no system icons displayed

#include	"pmi.h"
#include	"PreBrow.h"
#include	"PreBrowHead.h"
#include	"hpimage.h"
#include	"mullang.h"
#include	"imesys.h"
#include	"pubapp.h"
#include	"setup/setup.h"
#include	"plx_pdaex.h"
#include	"time.h"
#include	"sndmgr.h"
#include	"SndUsrApi.h"
#include	"SndString.h"
#include	"prioman.h"

#define ISSOUNDFILE(s)		(PREBROW_FILETYPE_WAV==(s)||PREBROW_FILETYPE_AMR==(s))


#define MAXFULLNAMELEN 512
#define MAXFILENAMELEN 64

/************************************************************************/
/* message IDs                                                          */
/************************************************************************/

#define		IDC_NULL			(WM_USER+1099)
#define		IDC_PREPIC_LIST		(WM_USER+1100)
#define		IDC_OK				(WM_USER+1101)
#define		IDC_BACK			(WM_USER+1102)
#define		IDC_PLAY			(WM_USER+1103)
#define		IDC_STOP			(WM_USER+1203)
#define		IDC_SEND			(WM_USER+1104)
#define		IDC_SENDMANY		(WM_USER+1105)
#define		IDC_DELETE			(WM_USER+1106)
#define		IDC_DELETEMANY		(WM_USER+1107)
#define		IDC_RENAME			(WM_USER+1108)
#define		IDC_MOVETOFOLDER	(WM_USER+1109)
#define		IDC_MOVEMANY		(WM_USER+1110)
#define		IDC_CREATEFOLDER	(WM_USER+1111)
#define		IDC_WALLPAPER		(WM_USER+1112)
#define		IDC_SHOWDETAIL		(WM_USER+1113)
#define		IDC_EXIT			(WM_USER+1114)
#define		IDC_PROCESSDEL		(WM_USER+1115)

#define		IDC_COPYSELECT		(WM_USER+1124)
#define		IDC_DELSELECT		(WM_USER+1125)
#define		IDC_MOVESELECT		(WM_USER+1126)

#define		IDC_COPYALL			(WM_USER+1127)
#define		IDC_DELALL			(WM_USER+1128)
#define		IDC_MOVEALL			(WM_USER+1129)

#define		IDC_CPTOFOLDER		(WM_USER+1131)
#define		IDC_COPYSEL			(WM_USER+1132)

#define		IDM_AUTOSTOP		(WM_USER+1133)

#define		IDRM_DELALL			(WM_USER+1134)
#define		IDRM_DELSEL			(WM_USER+1135)
#define		IDRM_MOVEALL		(WM_USER+1136)
#define		IDRM_OPENING		(WM_USER+1137)
#define		IDRM_MOTOFOLDER		(WM_USER+1140)
#define		IDRM_COPYALL		(WM_USER+1141)
#define		IDRM_CPTOFOLDER		(WM_USER+1142)

/*************************************************
	File path
*************************************************/

#ifdef _SOUND_SYSICONS_
#define SND_LEFTICON	SIGNALICON
#define SND_RIGHTICON	POWERICON
#else
#define SND_LEFTICON	NULL
#define SND_RIGHTICON	NULL
#endif

//#define		RIGHT_ICON	"/rom/mbrecord/right.bmp"
//#define		LEFT_ICON "/rom/mbrecord/left.bmp"

#define		STATIC_ICON		"/rom/pim/notepad/note.bmp"	//***********// Sounds_I_Playing_22x16.bmp
#define		PLAYING_ICON	"/rom/pim/notepad/newnote.bmp"		//***********// Sounds_I_Normal_22x16.bmp
#define		BENESND_ICON	"/rom/message/unibox/sms_unre.bmp"	//*************
#define		BENESNDPL_ICON	"/rom/message/unibox/sms_read.bmp"	//*************

#define		PHONEMEM_ICON	"/rom/message/unibox/mu_out.bmp"	//****************// Mgm_Phone_22x16.bmp
#define		MMCFOLDER_ICON	"/rom/message/unibox/mu_draft.bmp"	//***********// Mgm_MemoryCard_22x16.bmp
#define		SNDFOLDER_ICON	"/rom/message/unibox/folder.bmp"	//***********// Messaging_F_Folder_22x16.bmp
#define		OPENFOLDER_ICON	"/rom/message/unibox/newfolder.bmp"	//**********// Messaging_Open_Folder_22x16.bmp


#ifndef _SOUND_LOCAL_TEST_
#define		BENESNDFILEDIR		"/rom/audio/"
#define		PHONESNDFILEDIR		"/mnt/flash/audio/"
#define		MMCSNDFOLDERPATH	"/mnt/fat16/audio/"
#else
#define		BENESNDFILEDIR		"/rom/audio/"
#define		PHONESNDFILEDIR		"/rom/audio/"	//**********
#define		MMCSNDFOLDERPATH	"/mnt/flash/audio/"	//*********
#endif

#define		SOUND_FILETYPE_MMCCARD	128		// this value MUST be larger than 13

#define		FOLDERTYPE_UNKNOWN	0	// invalid folder
#define		FOLDERTYPE_ROOT		1	// PHONESNDFILEDIR
#define		FOLDERTYPE_SUB		2	// sub folders of ROOT
#define		FOLDERTYPE_MMCROOT	3	// MMCSNDFOLDERPATH
#define		FOLDERTYPE_MMCSUB	4	// sub folders of MMCROOT
#define		FOLDERTYPE_BENE		5	// bene sound dir

#define		SNDEDIT_RENAMEFILE	1
#define		SNDEDIT_RENAMEDIR	2
#define		SNDEDIT_CREATEDIR	3

#define		TIMERID_KEYDOWN		2204
#define		TIMEREL_KEYDOWN		300

/***********
Function declare
*********************************************/
BOOL	SND_DeleteDirectory(PCSTR	pPath);
BOOL	SND_DeleteAllInFolder(PCSTR	pPath);

BOOL	IsSound(PSTR pFileName, int *nItemData);
BOOL	RealtimeUpdateSound();
BOOL	SndBrowser_Edit(HWND hParentWnd, PSTR szName, PSTR szFullName, short nType);
BOOL	ShowSndDetail(HWND hFrame, PCSTR cFileName, PCSTR cFileFullName, int nFileType);
BOOL	SndMultiSel(HWND hFramewnd, PLISTBUF pList, DWORD wID);
int		SND_AddFolderToList(HWND hList, PCSTR pFilePath, HBITMAP	hNormal, PLISTBUF FolderList);
int		SND_GetFolderType (const char *szPath);
BOOL	SndMoveToFolder(HWND hRecmsg, UINT rMsg);
int		SND_GetFreeSpace(const char *szDstPath);
long	GetSndFileNumInDir(PCSTR pPath);
BOOL	AddSndFileToList(const char* pFilePath, LISTBUF* ListBuffer, BOOL bShowSubDir);
BOOL	SND_GetMMCStatus();
int		SearchSndData(PLISTBUF pListBuf, const char *szFullName, int nItemType);
BOOL	SND_IsMoveAvailable(PCSTR pSrc, PCSTR pDst);
BOOL	SND_GetMMCName(char *szName);
BOOL	SND_GotoParentFolder(char *szPath);

BOOL SND_GetInfoFromList(int nIndex, PLISTBUF pListBuffer, char* cFileName, char* szFullFileName, int *nFileType);
PLISTDATA PREBROW_AddSndData (PLISTBUF pListBuf, const char* szData, const char* szFullData, unsigned long atime);
PLISTDATA PREBROW_InsertSndData (PLISTBUF pListBuf, int nItemData, char* szData, char* szFullData, unsigned long atime);

extern BOOL GetMMCName(char *szName);

#endif	// _SOUND_APP_H_

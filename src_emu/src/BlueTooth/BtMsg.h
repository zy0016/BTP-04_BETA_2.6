/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Bluetooth module
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _BTMSG_H_
#define _BTMSG_H_

#include "window.h"

#define BTRECV_FILEPATH     "/mnt/flash/bluetooth/inbox"
#define BTMSGINFO_FILEPATH  "/mnt/flash/bluetooth"
#define BTMSGINFOFILENAME   "/mnt/flash/bluetooth/btmsginfo.txt"

#define MAX_BTFILENAME       64
#define MAX_BTPATHNAME       256

// handle到name的对应链表
typedef struct tagBTMSGHANDLENAME
{
    int  nCurFolder;
    int  nBtMsgStatus;		
    int  nBtMsgType;
	char szBtMsgRealName[MAX_BTFILENAME];
    char szBtMsgViewName[MAX_BTFILENAME];
    struct tagBTMSGHANDLENAME *Next;
} BTMSGHANDLENAME,*PBTMSGHANDLENAME;

typedef struct tagBTFOLDERINFO
{
	int nBtFolderID;
	int nBtTotalNum;
	int nBtUnreadNum;
    struct tagBTFOLDERINFO *Next;
}BTFOLDERINFO, *PBTFOLDERINFO;

typedef struct tagBTMSGFILEINFO
{
    int     nBtMsgDeleted;   //标志位,没有被删除是1，删除是0
	char    szBtMsgViewName[MAX_BTFILENAME];  //界面显示的文件名
	char    szBtMsgRealName[MAX_BTFILENAME];  //真实的文件名
	int     nUnread;     //已读未读标志,未读是1，已读是0
    int     nFolderid;   //当前的文件夹的ID
}BTMSGFILEINFO,*PBTMSGFILEINFO;

typedef struct tagBTMSGTIME {	
    DWORD		dwLowDateTime;	
    DWORD		dwHighDateTime;	
}BTMSGTIME, *PBTMSGTIME;

UINT BtGetMsgSize(char* filename);
BOOL BtGetMsgTime(char *filename, PBTMSGTIME pBtMsgTime);
void BtFTtoST(PBTMSGTIME pBtMsgTime, SYSTEMTIME* pBtMsgSysTime);
void BtSTtoFT(SYSTEMTIME* pBtMsgSysTime, PBTMSGTIME pBtMsgTime);
BOOL BtCopyFile(PCSTR pOldFileName, PCSTR pNewFileName);
BOOL BtMoveFile(PCSTR pOldFileName, PCSTR pNewFileName);

#endif

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

// handle��name�Ķ�Ӧ����
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
    int     nBtMsgDeleted;   //��־λ,û�б�ɾ����1��ɾ����0
	char    szBtMsgViewName[MAX_BTFILENAME];  //������ʾ���ļ���
	char    szBtMsgRealName[MAX_BTFILENAME];  //��ʵ���ļ���
	int     nUnread;     //�Ѷ�δ����־,δ����1���Ѷ���0
    int     nFolderid;   //��ǰ���ļ��е�ID
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

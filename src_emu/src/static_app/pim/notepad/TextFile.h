/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : TextFile.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _TEXTFILEHEAD_
#define _TEXTFILEHEAD_

#include "unistd.h"
#include "dirent.h"
#include "types.h"
#include "stat.h"
#include "unistd.h"
#include "vfs.h"
#include "fcntl.h"


#define SAVE_MODE_NEW			1	
#define SAVE_MODE_OVERWRITE		2	
#define SAVE_MODE_APPEND		3

#define _MAX_PATH  256
#define NOTEPAD_MAX_CONTENT     10*1024
#define TEXT_NAME_LENGTH		15
#define TEXT_SUFFIX_LENGTH		3
#define TEXT_FILENAME_LENGTH	64
#define TEXT_FILE_SUFFIX		"txt"
#define TEXT_FILE_TYPE			".txt"
#define TEXT_PATH_FLASH				"/mnt/flash/notepad/"

#define TEXT_LIST_FROMAT		"%-13s"
#define TEXT_LIST_FROMAT2		 "%-13.13s..."



typedef struct  tagTextInfo{
		char szTextName[TEXT_NAME_LENGTH + 1];
	char szFileName[TEXT_FILENAME_LENGTH];
	DWORD dwTextLength;
	unsigned long stTime;
}TEXTINFO, * PTEXTINFO;

typedef struct tagTextInfoNode{
	TEXTINFO TextInfo;
	struct tagTextInfoNode * pPreInfo;
	struct tagTextInfoNode * pNextInfo;
}TEXTINFONODE, * PTEXTINFONODE;

PTEXTINFO SaveText(PTEXTINFO pInfo, PCSTR szContent, char *pNewFileName, char* pDesPath, DWORD dwMode);
int ReadText(PTEXTINFO pInfo, char * szContent, DWORD dwReadLen);
int DeleteText(PTEXTINFO pInfo);
int GetAllTextNode(char* pNotePath);

void ReleaseAllTextInfo();
int AddTextNode(PTEXTINFO pText);
int MoveToFirstTextNode();
int MoveToNextTextNode();

PTEXTINFO GetCurTextInfo();
int IsEnoughFreeSpaceToSaveText(int nlength);
void GetTimeStr(SYSTEMTIME FileSysTime, char *cResult);
int GetTextFileInfo(PCSTR szFilename, struct stat *buff);
int GetTextFileTime(PCSTR szFilename, PSYSTEMTIME pSysTime);
int GetTextFileLength(PCSTR szFilename);
int Notepad_MemManage(void);
int IsWantedFile(char * FileName, char * pWantedFile);

#endif	//_TEXTFILEHEAD_




 

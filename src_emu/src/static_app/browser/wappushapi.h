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
#ifndef _WAPPUSHAPI_H_
#define _WAPPUSHAPI_H_

#include "stdio.h"
#include "PushStruct.h"

#define DATETIMELEN	21
#define TIMELEN	30
#define WP_PUSH_READ	0
#define WP_PUSH_UNREAD	1

typedef struct tagSIINFORecord
{
	char *pszTitle;
	char *pszUri;
	char *pszId;
	char szCreateTime[DATETIMELEN];
	char szExTime[DATETIMELEN];
	WORD nActionType;
	WORD nRead;		//0ÎªÒÑ¶Á£¬1ÎªÎ´¶Á    
    char szTime[TIMELEN];
}SIINFORECORD,*PSIINFORECORD;


int WP_AddToTail(FILE* pFile, PPUSHFILERECORD pRecord, int offset);
int WP_ChangeOffset(char* data, int datalen, int change);
BOOL WP_DeleteRecord(int offset);
BOOL WP_FormatMessage(int MsgHandle);
BOOL WP_FreeNode(int MsgHandle, int offset);
BOOL WP_GetRecord(int offset, PPUSHFILERECORD pRecord);
int WP_GetTotalCount(void);
int WP_GetUnReadCount(void);
void WP_InitMsgHandle(void);
int WP_InsertRecord(PPUSHFILERECORD pRecord);
int WP_MallocNode(void);
void WP_NewPushArrival(int MsgHandle);
BOOL WP_ResetFlag(int offset, BOOL nFlag);
BOOL WP_GetHandleReady(HWND hList);
void TestPush(void);


#endif		//_WAPPUSHAPI_H_

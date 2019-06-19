/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : MMS
 *
 * Purpose  : define interface with protocol
 *            
\**************************************************************************/

#ifndef   MMSPROTOCOL_
#define   MMSPROTOCOL_

#define  MMS_REC_ATONCE    0
#define  MMS_REC_CONFIRM   1
#define  MMS_REC_NEVER     2

#define PB_FLASH_ENTRY_NUMBER_MAXLEN    43
#define PB_FLASH_ENTRY_EMAIL_MAXLEN     65
#define PB_FLASH_ENTRY_NAME_MAXLEN      31

BOOL IsFailureTableFull(void);
void GetQueFront(char *filename);
HWND MMSPro_GetHandle();
void DelQueFront();
int PackMmsBody(const char *pFileName, TotalMMS tmms);
BOOL MMSPro_IsInRSChain(const char *szFilename);
static BOOL Try_StartDialing(void);
extern BOOL MMS_SaveSetting(MMSSETTING * mSet);
extern void WaitWinWithTimer (HWND hParent, BOOL bShow, PCSTR cPrompt, PCSTR szCaption, PCSTR szOk,PCSTR szCancel, UINT rMsg, UINT uTimer);

typedef struct mmspendingrec
{
	char  pFileName[256];
	struct mmspendingrec* pNext;
}MMSPENDINGREC, *PMMSPENDINGREC;

#endif MMSPROTOCOL_

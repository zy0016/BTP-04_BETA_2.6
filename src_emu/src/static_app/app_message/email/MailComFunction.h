/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailComFunction.h
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

void MAIL_GetLocalDateString(char * szLocalDate);
int MAIL_DateCmpChar(char *FirstChar, char *SecondChar);
void MAIL_SpecialChar(char *CharDate, SYSTEMTIME *SysDate);
void MAIL_CharToSystem(char *CharDate, SYSTEMTIME *SysDate);
void MAIL_SystemToChar(SYSTEMTIME *SysDate, char *CharDate);
void MAIL_GetNewFileName(char *szNewFilePath, int len);
BOOL MAIL_IsFileExist(char *szFileName);
void MAIL_JudgeCPUType(void);
BOOL MAIL_IsSpaceEnough(void);
int MAIL_IsFlashEnough();
static int MAIL_IsFlashAvailable(int nSize);
int MAIL_GetMailTotalFile(void);
int MAIL_StringCut(char *pStr, unsigned int nCutLen);
void MAIL_UpperCase(char *pDest);
void MAIL_LowerCase(char *pDest);
int MAIL_GetFileSuffix(void);
void MAIL_TrimString(char * pstr);
int MAIL_GetMailAttachmentNum(char *AttName);
BOOL MAIL_GetMailAttachmentSaveName(char *AttAllName, char *AttName, int num);
BOOL MAIL_GetMailAttachmentShowName(char *AttAllName, char *AttName, int num);
int MAIL_GetAllMailAttSize(char *AttAll, int Num);
int MAIL_GetMailAttachmentSize(char *AttName);
char*  MAIL_GetFileType(char *FileName);
BOOL MAIL_IsAttSupported(char *szAffixName, char *tmpType, int *type);
void MAIL_GetFileNameFromPath(char *szFullName, char*szFileName);
void MAIL_GetNewAffixFileName(char *pNewName);
BOOL MAIL_DeleteAttachment(char *DelAtt);
BOOL MAIL_DeleteFile(char *DeleteFile);
BOOL MAIL_CopyMail(char *szDes, char *szSour);
BOOL MAIL_CopyFile(char *pSour, char *pDes);
BOOL MAIL_CopyAttachment(char *szDes, char *szSour, int Num);
int MAIL_AnalyseMailAddr(char * strAddr);
BOOL MAIL_GetMailAddress(char *strAddr, char *MailAddr, int number);
int MAIL_JudgeEdit();
BOOL MAIL_ConvertPath(char *Des, char *Sour, int type);
BOOL MAIL_CombineReplyRec(char **ReturnName, char *WholeName, char *IndiName);
void MAIL_CommaToSemicolon(char *WholeName);
void MAIL_InitConfigNode(MailConfigHeader *pHead, MailConfigNode *pNode, MailConfig *pFile);
BOOL MAIL_AddDialWaitNode(MailConfigHeader *pHead, MailConfigNode *pNode, BYTE WaitType);
void MAIL_DelDialWaitNode(MailConfigHeader *pHead, MailConWaitNode *pNode);
void MAIL_DealNoReturnConnect();
BOOL MAIL_DealReturnConnect();
void MAIL_DealNextConnect();
void MAIL_TruncateRec(char *chDes, char *chSour);
BOOL MAIL_PlayAudio (HWND hWnd, char *audioFileName, int audioType, 
                     const char *audioBody, int audioSize);
void MAIL_StopPlay(void);
int GARY_open(const char *filename, int FirstArg, int SecondArg);
int GARY_close(int hFile);
DIR* GARY_opendir(const char* szPath);
int GARY_closedir(DIR* dir);
BOOL MAIL_DayIncrease(SYSTEMTIME *psystime, unsigned int day);
int MAIL_AlmDaysOfMonth(SYSTEMTIME *pDate);
BOOL MAIL_IsLeapYear(WORD wYear);

extern void MAIL_GenerateMailID(char *szName, char *szMailID);
extern BOOL MAIL_ConnectServer(HWND hWnd, BYTE bConType);
extern UINT	SetRTCTimer(HWND hWnd, UINT uIDEvent, DWORD uElapse, TIMERPROC pTimerFunc);



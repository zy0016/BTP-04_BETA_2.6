/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailMailBox.c
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

//mailbox
BOOL RegisterMailInBoxClass(void);
void UnRegisterMailInBoxClass(void);
BOOL CreateMailInBoxWnd(HWND hParent, const char *SZMAILBOXNAME);
static LRESULT MailInBoxWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MailInBox_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MailInBox_OnActivate(HWND hWnd, UINT state);
static void MailInBox_OnInitmenu(HWND hWnd);
static void MailInBox_OnPaint(HWND hWnd);
static void MailInBox_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailInBox_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MailInBox_OnDestroy(HWND hWnd);
static void MailInBox_OnClose(HWND hWnd);
static BOOL MailInBox_OnBoxReturn(HWND hWnd, WPARAM wParam, LPARAM lParam);
void MailInBox_OnFolderReturn(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MailInBox_OnOpenPre(HWND hWnd, LPARAM lParam);
static void MailInBox_OnOpenNext(HWND hWnd, LPARAM lParam);
static void MailInBox_OnDeleteAtt(HWND hWnd);

//select sub window
static void MAIL_SelectMsg(HWND hFrameWnd, HWND hParent, BOOL bMove, BOOL bDel);
static LRESULT MailSelWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL MailSel_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MailSel_OnActivate(HWND hWnd, UINT state);
static void MailSel_OnPaint(HWND hWnd);
static void MailSel_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailSel_OnCommand(HWND hWnd, int id, UINT codeNotify);
static void MailSel_OnDestroy(HWND hWnd);
static void MailSel_OnClose(HWND hWnd);
static void MailSel_OnDelete(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MailSel_OnFolder(HWND hWnd, WPARAM wParam, LPARAM lParam);

//Get List and Get Node
static BOOL MAIL_GetGetList(HWND hWnd, MailGetListHead *pHead, char *pTmp);
static void MAIL_AddNodeToList(HWND hList, MAIL_INBOXCREATEDATA *pCreateData);
static void MAIL_SetImage(HWND hList, MAIL_INBOXCREATEDATA *pCreateData, MailGetListNode *pTemp, int curIndex);

BOOL MAIL_ConnectServer(HWND hWnd, BYTE bConType);
BOOL MAIL_GetHeaderFromServer(HWND hWnd, int start, int num);
void MAIL_GetServerHead();
static BOOL MAIL_DelHeaderFromServer(HWND hWnd);
void MAIL_DelServerHead();
static BOOL MAIL_RetriHeaderFromServer(HWND hWnd);
void MAIL_RetrieveServerHead(HWND hWnd);
void MAIL_RetrieveHideHead();
void MAIL_QuitServerHead();
void MAIL_RetrieveDigest(BOOL bSucc);
static BOOL MAIL_DecodeDigest(int hMailObj, char **chDigest);

void MAIL_ReleaseGetList(MailGetListHead *pHead);
void MAIL_AddGetListNode(MailGetListHead *pHead, MailGetListNode *pTmpNode, int style);
void MAIL_DelGetListNode(MailGetListHead *pHead, MailGetListNode *pTmpNode);
void MAIL_RemoveGetListNode(MailGetListHead *pHead, MailGetListNode *pTmpNode, int style);
int MAIL_GetGetNodeInt(MailGetListHead *pHead, MailGetListNode *pNode);
BOOL MAIL_GetGetNodebyInt(MailGetListHead *pHead, MailGetListNode **pListNode, int iTmp);
BOOL MAIL_GetGetNodebyHandle(MailGetListHead *pHead, MailGetListNode **pListNode, char *msghandle);
BOOL MAIL_GetGetNodebyID(MailGetListHead *pHead, MailGetListNode **pListNode, int msg);

static void MAIL_GetSaveSaveNode(HWND hWnd, MailGetListNode *pTmpNode);
static void MAIL_DelSaveSaveNode(HWND hWnd, MailGetListNode *pTmpNode);
void MAIL_DelConfigSaveList(char *SaveName);
static void MAIL_GetSaveAllFile(HWND hWnd);

void MAIL_PLXTipsWin(const char *cTips, NOTIFYTYPE Tipspic);
//extern
extern BOOL MAIL_CreateViewWnd(HWND hParent, const char *pszOutFile, DWORD handle, int folderid, BOOL bPre, BOOL bNext);

extern int MAIL_GetConfigNodebyName(MailConfigHeader *pHead, MailConfigNode *pConfigNode, const char *szTmpName);
extern BOOL MAIL_SaveMailContent(int hMailObj, char *DownMailName, char *BoxName, int iReadFileSize, BOOL *bAtt);

extern BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
                              const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT, 
                              const SYSTEMTIME *PTIME, DWORD handle, int folderid);

extern BOOL CreateMailDetailWnd(HWND hParent, MailFile *szOutFile);
extern char *Mail_UTF8DecodeOnString(char *pSrc, int nSize);
extern char *Mail_MultiDecodeOnString (char *pSrc, int nSize);

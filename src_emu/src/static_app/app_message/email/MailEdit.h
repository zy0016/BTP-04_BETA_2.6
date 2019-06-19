/***************************************************************************
*
*                      Pollex Mobile Platform
*
* Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
*                       All Rights Reserved 
*
* Module   : MailEdit.h
*
* Purpose  : 
*
\**************************************************************************/

#include "MailHeader.h"

//insert submenu
MENUITEMTEMPLATE MailInsertMenuItem[] =
{
    {MF_STRING, IDM_MAIL_INSERT_PICTURE, NULL, NULL},
    {MF_STRING, IDM_MAIL_INSERT_SOUND, NULL, NULL},
    {MF_STRING, IDM_MAIL_INSERT_NOTE, NULL, NULL},
    {MF_STRING, IDM_MAIL_INSERT_TEMPLATE, NULL, NULL},
    {MF_END, 0, NULL, NULL}
};

const MENUTEMPLATE MailInsertMenuTemplate =
{
    0,
        MailInsertMenuItem
};

//global
BOOL RegisterMailEditClass(void);
void UnRegisterMailEditClass(void);
BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
                       const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT, 
                       const SYSTEMTIME *PTIME, DWORD handle, int folderid);
static LRESULT MailEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL MAIL_CreateEditInterface(HWND hFrameWindow, char *PSZRECIPIENT, 
                              char *PSZATTENMENT, char *PSZATTNAME, BOOL bAtt);

//message
static BOOL MailEdit_OnCreate(HWND hWnd, LPCREATESTRUCT lpCreateStruct);
static void MailEdit_OnActivate(HWND hWnd, UINT state);
static void MailEdit_OnInitmenu(HWND hWnd);
static void MailEdit_OnPaint(HWND hWnd);
static void MailEdit_OnKey(HWND hWnd, UINT vk, int cRepeat, UINT flags);
static void MailEdit_OnCommand(HWND hWnd, int id, UINT codeNotify, LPARAM lParam);
static void MailEdit_OnSetLBtnText(HWND hWnd, int nID, BOOL bEmpty, LPSTR pszText);
static void MailEdit_OnMoveWindow(HWND hWnd,WPARAM wParam);
static void MailEdit_OnConfirm(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MailEdit_OnDestroy(HWND hWnd);
static void MailEdit_OnClose(HWND hWnd);
static void MailEdit_OnAddress(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MailEdit_OnInsertAtt(HWND hWnd, WPARAM wParam, LPARAM lParam);
static void MailEdit_OnReplaceSel(HWND hWnd);
static void MailEdit_OnSetRightIcon(HWND hWnd, WPARAM wParam, LPARAM lParam);

//local
static BOOL MAIL_SaveToBox(HWND hWnd, int BoxStyle);
static BOOL MAIL_SaveFile(HWND hWnd, char* szFileName, int BoxStyle, MailFile *SaveFl);
void MAIL_GenerateMailID(char *szName, char *szMailID);
BOOL MAIL_CheckReceiverValid(char *szReceiver);
BOOL MAIL_CheckMultiAddr(char *strMailAddr);
static void MAIL_EditInitVScroll(HWND hWnd);
void MAIL_CaretProc(const RECT* rc);
static void MAIL_RecipientGetText(HWND hEdt, PSTR pszText, int nMaxCount);
static int MAIL_RecipientGetTextLength(HWND hEdt);

static void MAIL_SetMemIndicator(HWND hWnd, int nFrag);
static void MAIL_SetAttIndicator(HWND hWnd, int nFrag);
void MAIL_SetNumber(HWND hWnd, int leftNum, int rightNum);

//extern
extern void MAIL_GetNewFileName(char *szNewFilePath, int len);
extern void MAIL_TrimString(char * pstr);
extern int MAIL_GetMailAttachmentNum(char *AttName);
extern BOOL MAIL_GetMailAttachmentSaveName(char *AttAllName, char *AttName, int num);
extern MAIL_GetMailAttachmentShowName(char *AttAllName, char *AttName, int num);
extern void MAIL_GetNewAffixFileName(char *pNewName);
extern BOOL MAIL_CopyFile(char *pSour, char *pDes);
extern int MAIL_GetMailAttachmentSize(char *AttName);
extern BOOL MAIL_AddSendMailNode(HWND hWnd, char *pMailSendName, char *MsgId);
extern BOOL CreateMailDetailWnd(HWND hParent, MailFile *szOutFile);
extern BOOL MAIL_CreateSetWnd(HWND hParent);

extern BOOL CreateMailDisplayWnd(HWND hFrameWnd, HWND hParent, char *pAttHead, int AttNum, int nAttSize, BOOL bEdit);
extern BOOL CallAppEntryEx(PSTR pAppName, WPARAM wparam, LPARAM lparam);

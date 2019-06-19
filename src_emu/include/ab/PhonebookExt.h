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

#ifndef _ADDRESSBOOKEXT_H
#define _ADDRESSBOOKEXT_H

#include <me/me_wnd.h>

#define AB_MAXLEN_NAME                 102
#define AB_MAXLEN_TEL                  42
#define AB_MAXLEN_EMAILADDR            51
#define AB_MAXLEN_RING                 256
#define AB_MAXLEN_ICON                 256
#define AB_MAXLEN_GROUPNAME            51

typedef enum{
    PICK_NUMBER = 0,
    PICK_EMAIL,
    PICK_NUMBERANDEMAIL,
}AB_PICK_TYPE;

typedef enum{
    AB_NEW = 0,
    AB_UPDATE,
}AB_SAVE_MODE;

typedef enum{
    AB_NUMBER = 0,
    AB_EMAIL,
    AB_URL,
}AB_SAVE_TYPE;


typedef struct tagABInfo
{ 
    int         nGroupID;
    char        szName[AB_MAXLEN_NAME];
    char        szTel[AB_MAXLEN_TEL];
    char        szRing[AB_MAXLEN_RING];
    char        szIcon[AB_MAXLEN_ICON];
}ABINFO,*PABINFO;

typedef struct tagGroup_Info
{
    int    nDel;
    int    nGroupID;
    char   szGroupName[AB_MAXLEN_GROUPNAME];
}GROUP_INFO,*PGROUP_INFO;

typedef struct tagABNameOrEmail
{ 
    AB_SAVE_TYPE    nType;
    char            szName[AB_MAXLEN_NAME];
    char            szTelOrEmail[AB_MAXLEN_EMAILADDR];
}ABNAMEOREMAIL,*PABNAMEOREMAIL;

BOOL APP_GetInfoByPhone(const char* pPhone, ABINFO* pInfo);

BOOL APP_GetNameByPhone(const char* pPhone, ABNAMEOREMAIL* pABName);

BOOL APP_GetNameByEmail(const char* pEmailAddr, ABNAMEOREMAIL* pABName);

BOOL APP_GetQuickDial(WORD wDDIdx, ABNAMEOREMAIL* pABName);

BOOL APP_GetGroupInfo(GROUP_INFO* pGroup_Info, int* pnGroupCounter);

#define AB_GETSIMREC_SUCC       0
#define AB_GETSIMREC_NOTFOUNT   1
#define AB_GETSIMREC_INIT       2

int APP_GetSIMRecByOrder(int nIndex, PHONEBOOK* pPB);

BOOL APP_AddApi(DWORD* pid,LPBYTE pData,LONG nDataLen);

BOOL APP_ModifyApi(DWORD id,LPBYTE pData,int nDataLen);

BOOL APP_DeleteApi(DWORD id);

/*
* uMsgCmd 
* wParam = TURE means pick up one; wParam = FLASE means cancel
* lParam = (ABNAMEOREMAIL*)
*/
//exist UI
BOOL APP_GetOnePhoneOrEmail(HWND hFrameWnd, HWND hMsgWnd, UINT uMsgCmd, AB_PICK_TYPE nType);

/*
* uMsgCmd 
* wParam = number of the selected
* lParam = (ABNAMEOREMAIL*) Array
*/
//exist UI
BOOL APP_GetMultiPhoneOrEmail(HWND hFrameWnd, HWND hMsgWnd, UINT uMsgCmd, 
                              AB_PICK_TYPE nType,ABNAMEOREMAIL* pArray,int nCount,int nMax);

/*
* uMsgCmd 
* wParam = TURE means save ok; wParam = FLASE means cancel or failed
* lParam = (ABNAMEOREMAIL*)
*/
BOOL APP_SaveToAddressBook(HWND hFrameWnd, HWND hMsgWnd, UINT uMsgCmd, ABNAMEOREMAIL* pABName,AB_SAVE_MODE nMode);

/*
* uMsgCmd 
* wParam = TURE means save ok; wParam = FLASE means cancel or failed
* lParam = (ABNAMEOREMAIL*)
*/
HWND APP_SetQuickDial(HWND hMsgWnd, UINT uMsgCmd,WORD wDDIdx);
#endif

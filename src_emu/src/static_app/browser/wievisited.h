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
#ifndef WIE_VISITED_H
#define WIE_VISITED_H

#include "wUipub.h"

#define WIE_IDM_HIS_OK            301
#define WIE_IDM_HIS_CLOSE         302
#define WIE_IDM_DETAILS           307
#define WIE_IDM_HIS_DEL           308
#define WIE_IDM_EMPTYHIS          309
#define WIE_IDM_HIS_EMPTYCONFIRM  306
#define WIE_IDM_HIS_DELCONFIRM    305

#define WIE_HIS_MAXNUM            20

static MENUITEMTEMPLATE WMLhisMenu[] = 
{
    {MF_STRING, WIE_IDM_HIS_OK, "goto", NULL},
    {MF_STRING, WIE_IDM_DETAILS, "details", NULL},
	{MF_STRING, WIE_IDM_HIS_DEL, "remove", NULL},
    {MF_STRING, WIE_IDM_EMPTYHIS, "remove all", NULL},
    {MF_END, 0, NULL, NULL}
};

static const MENUTEMPLATE WMLHISMENUTEMPLATE =
{
    0,
    WMLhisMenu
};

#define WML_HISTORY_NAME_LEN      100
typedef struct tagWIEHistoryItem
{
    int nUrltype;       //保存URL名字的ID号;
	char szName[100];    //保存URL名字;
    char *szURLName;    //保存URL;  
} WIEHistoryItem;

BOOL WAP_Public_Setting(HWND hParent);
BOOL On_IDM_Visited(HWND hParent, RECT rClient);
int LoadHistory();
BOOL WIE_Savehistory();
void WIE_EmptyHistory();
int WIE_AddHisItem(int nUrltype, char *szUrl, char *szName);
extern char* Wml_StrDup(const char*);

#endif

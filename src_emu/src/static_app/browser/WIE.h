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

#ifndef WML_WIE_H
#define WML_WIE_H

#include "wUipub.h"

#define HOMEPAGE_START

#define MSG_DIALUSER				(WM_USER+50)
#define WM_CLOSE_PROMPT				(WM_USER+40)
#define MSG_DIAL_SUCCESS			(MSG_DIALUSER+1)
#define MSG_DIAL_FAILURE			(MSG_DIALUSER+2)
#define MSG_CALL_REFUSE				(MSG_DIALUSER+3)
#define WM_USERDIAL                 (WM_USER+345)
#define WIE_USER_CALL               0x0f0f0f0f
#define WIE_USER_CALLDEF            0x0f0f0f0E
#define WIE_ID_COMBOBOX             (WM_USER+101)
#define WIE_ID_LISTBOX              (WM_USER+102)
#define WIE_ID_STATIC               (WM_USER+103)
#define WIE_IDM_REMOTE              (WM_USER+104)
#define WIE_IDM_LOCAL               (WM_USER+105)
#define WIE_IDM_SAVEAS              (WM_USER+106)
#define WIE_IDM_HISTORY             (WM_USER+107)
#define WIE_IDM_BOOKMARK            (WM_USER+108)
#define WIE_IDM_SETTING             (WM_USER+109)
#define WIE_IDM_MENU_EXIT           (WM_USER+110)
#define WIE_IDM_SAVEIMG             (WM_USER+119)
#define WIE_IDM_DIAL                (WM_USER+111)
#define WIE_IDM_EXIT                (WM_USER+300)
#define WIE_IDM_REFRESH             (WM_USER+301)
#define WIE_IDM_DISCONNECT          (WM_USER+302)
#define WIE_IDM_GO                  (WM_USER+111)
#define WIE_IDM_HOMEPAGE            (WM_USER+113)
#define WIE_IDM_BACK                (WM_USER+114)
#define WIE_IDM_FORWARD             (WM_USER+115)
#define WIE_IDM_STOP                (WM_USER+116)
#define WIE_IDM_DO_OK               (WM_USER+119)
#define WIE_IDM_DO_CC               (WM_USER+118)
#define WIE_IDM_OPTION              (WM_USER+201)
#define WIE_IDM_JUMP                (WM_USER+202)
#define WIE_IDM_INBOX               (WM_USER+131)
#define WML_IDM_EXIT_OK             (WM_USER+121)
#define WML_IDM_EXIT_CC             (WM_USER+120)
#define WM_BROHISTORY               (WM_USER+30)
#define WML_DISABLEBF               (WM_USER+31)          
#define WML_ENABLEBACK              (WM_USER+32)
#define WML_ENABLEFORWORD           (WM_USER+33)
#define WML_ENABLEBF                (WM_USER+34)
#define UM_URLGO                    (WM_USER+225)
#define UM_FILESAVED                (WM_USER+226)
#define UM_STATUSRESET              (WM_USER+240)
#define UM_STATUS                   (WM_USER+241)
#define WIE_IDM_MENU_OK				(WM_USER+111)
#define WIE_SHORT_OK				(WM_USER+122)
#define WIE_SHORT_CLOSE				(WM_USER+123)

#define IDM_WML_GOTOURL             (WM_USER+601)
#define IDM_WML_REFRESH             (WM_USER+602)
#define IDM_WML_SAVES               (WM_USER+603)
#define IDM_WML_PAGES               (WM_USER+604)
#define IDM_WML_SETTINGS            (WM_USER+605)
#define IDM_WML_DISCONNECTACTTION   (WM_USER+606)
#define IDM_WML_CONNECTACTTION      (WM_USER+607)
#define IDM_WML_EXITAPP             (WM_USER+608)
#define IDM_WML_BOOKMARK            (WM_USER+609)
#define IDM_WML_PAGE                (WM_USER+610)
#define IDM_WML_FILE                (WM_USER+611)
#define IDM_WML_SHORTCUT            (WM_USER+612)
#define IDM_WML_GOHOMEPAGE          (WM_USER+613)
#define IDM_WML_BOOKMARKS           (WM_USER+614)
#define IDM_WML_PAGESAVED           (WM_USER+615)
#define IDM_WML_HISTORY             (WM_USER+616)
#define IDM_WML_GOTO                (WM_USER+617)
#define IDM_WML_EDIT                (WM_USER+618)
#define IDM_WML_DELETE              (WM_USER+619)
#define IDM_WML_SEND                (WM_USER+620)
#define IDM_WML_SMS                 (WM_USER+621)
#define IDM_WML_MMS                 (WM_USER+622)
#define IDM_WML_MAIL                (WM_USER+623)
#define IDM_WML_WAIT_CONNECT		(WM_USER+624)
#define IDM_WML_WAIT_DISCONNECT		(WM_USER+631)
#define IDM_WML_PUSH				(WM_USER+625)
#define IDM_WML_DISCONFIRM			(WM_USER+626)
#define IDM_WML_CONCONFIRM			(WM_USER+627)
#define IDM_WML_SHORTCONFIRM		(WM_USER+628)
#define IDM_WML_CONNECTCONFIRM		(WM_USER+629)
#define IDM_WML_DISCONNECTCONFIRM   (WM_USER+630)

typedef enum{
    AB_FIND_NAME = 0,
    AB_FIND_TEL,
    AB_FIND_EMAIL,
    AB_FIND_ID,
}AB_FIND_TYPE;


void Wml_Go();

#endif

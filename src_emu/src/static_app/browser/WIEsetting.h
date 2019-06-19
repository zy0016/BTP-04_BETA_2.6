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
#ifndef WIESETTING_H
#define WIESETTING_H

#include "wUipub.h"
#include "PWBE.h"

#define WIE_IDM_BTNPAGE             0
#define WIE_IDM_BTNCOLOR            1
#define WIE_IDM_BTNADVANCECFG       2
#define WIE_IDM_BTNGPRS             3
#define WIE_IDM_BTNCERTIFICATECFG   4
#define WIE_IDM_BTNCACHECFG         0
#define WIE_IDM_BTNCOOKIECFG        1
#define WIE_IDM_BTNSHOWCFG          2
#define WIE_IDM_CHANGECOLOR         301
#define WIE_IDM_RESET				304
#define WIE_IDM_ITEM				305
#define WIE_IDM_OPAGE               307    
#define WIE_IDM_CURPAGE             308
#define WIE_IDM_EDITPAGE            309
#define WIE_IDM_CFG0				505
#define WIE_IDM_CFG1				501    
#define WIE_IDM_CFG2				502
#define WIE_IDM_CFG3				503
#define WIE_IDM_CFG4				504
#define WIE_IDM_SET_CHANGE_ITEM     507
#define WIE_IDM_SET_OK              508
#define WIE_IDM_SET_CLOSE           509
#define COLORBN_KILLFOCUS           510
#define WIE_IDM_GW_EDIT             510
#define WIE_IDM_GW_DEL              511
#define WIE_IDM_GW_ACTIVE           512
#define WIE_IDM_PRO_WSP             550
#define WIE_IDM_PRO_WHTTP           551

#define  DEFTEXTCOLOR            RGB(0, 0, 0) // href text color: gray
#define  DEFHREFCOLOR            RGB(0, 0, 255) //href text color: gray
#define  DEFWNDBACKCOLOR         RGB(255, 255, 255) //href text color: gray
#define  DEFHREFFOCUSCOLOR       RGB(255,255 ,255)  // href text focus color: red 
#define  DEFWNDBACKFOCUSCOLOR    RGB(0, 0, 255) //href text color: gray

#define RES_RADIO_OFF   "/rom/wml/radio_button_off.bmp"
#define RES_RADIO_ON    "/rom/wml/radio_button_on.bmp"

typedef struct _WMLSet
{
	HWND hCacheSize;
	HWND hCookie;
	HWND hDisplay;
	HWND hSound; 
	BOOL bDownMove;
	BOOL bUpMove;
	int nFocus;
} WMLSETTING, *PWMLSETTING;

typedef struct _WMLSetSub
{
	HWND hWindow;
	HBITMAP hbmpRadioON;
	HBITMAP hbmpRadioOFF;
} WMLSETSUB, *PWMLSETSUB;

HWND On_IDM_Setting(HWND hParent, RECT rClient);
void WIE_Set_Systemcfg(void);
BOOL WAP_PubSet_Select(HWND hParent);
int Loadconfig(WMLCFGFILE *pCurrentCfg);
Color_Scheme WSET_GetColorCfg();
BOOL On_WAP_CACHE_Setting(HWND hParent, PWMLSETTING pSet);
BOOL On_WAP_COOKIE_Setting(HWND hParent, PWMLSETTING pSet);
BOOL On_WAP_DISPLAY_Setting(HWND hParent, PWMLSETTING pSet);
BOOL On_WAP_SOUND_Setting(HWND hParent, PWMLSETTING pSet);
BOOL On_WAP_CONNECT_Setting(HWND hParent);

#endif 

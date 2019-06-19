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
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
//#include    "fapi.h"
#include	"sys/types.h"
#include	"sys/stat.h"
#include	"fcntl.h"
#include	"unistd.h" 
#include    "setting.h"
#include    "str_public.h"
#include    "plx_pdaex.h"
#include    "setup.h"
#include    "mullang.h"
#include    "imesys.h"
#include    "PreBrowHead.h"
#include    "PreBrow.h"

#define     pClassName              "DisplaySettingClass"
#define     pSSClassName            "ScreenSaverSettingClass"
#define     pSEClassName            "startupsettingClass"
#define     pWSTClassName           "EditStartupTextClass"
#define     pBLClassName            "BacklightSettingClass"
#define     pDIS_BK_LIT_ClassName   "DisBackLitSettingClass"
#define     pDIS_KP_LIT_ClassName   "KeypadLitSettingClass"
#define     pInfo_ClassName         "InfoLineSettingClass"
#define     pIndLED_ClassName       "IndLedSettingClass"


#define     IDC_BUTTON_SEL          101
#define     IDC_BUTTON_BACK         102
#define     IDC_BUTTON_QUIT         103
#define     IDC_BUTTON_SET          104
#define     IDC_INDICATOR_LED       105
#define     IDC_COLORTHEME          106
#define     IDC_BRIGHTNESS          107
#define     IDC_STARTUP             108
#define     IDC_WALLPAPER	        109
#define     IDC_SAVERLIST           110
#define     IDC_BUTTON_OK           111
#define     IDC_LIGHTS              113
#define     IDC_INFOLINE            118
#define     IDC_SELLIST             120
#define     IDC_BUTTON_SAVE         121
#define     ID_EDITTXT              122
#define     IDC_BUTTON_CANCEL       123
#define     IDC_DISBKLIT            124
#define     IDC_KEYPADBKLIT         125
#define     IDC_LIST                126
#define     IDC_CANCEL              127
#define     IDC_OK                  128
#define     IDC_KPLIST              129
#define     IDC_INFOLIST            130
#define     IDC_INDLED_LIST         131

#define     TIMER_ASURE             WM_USER + 100
#define     CALLBACK_SETLIT         WM_USER + 101
#define     IME_MSG_SETTEXT         WM_USER + 102
#define     CALLBACK_SETBKPIC       WM_USER + 103
#define     CALLBACK_SET_START_PIC  WM_USER + 104
#define     TIMER_ID     WM_USER + 105

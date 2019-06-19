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
#include    "plx_pdaex.h"
#include    "setting.h"
#include    "str_public.h"
//#include    "hpime.h"
#include	"imesys.h"
#include    "setup.h"
#include	"pmalarm.h"
#define     IDC_BUTTON_QUIT         3//quit, back
#define     IDC_BUTTON_SET          4//ok
#define     IDC_BUTTON_SAVE         5//save

#define     IDC_AUTOON              100
#define     IDC_AUTOOFF             101
#define     IDC_AUTOSWITCHON        102
#define     IDC_SWITCHONTIME        103
#define     IDC_AUTOONOFFLIST       104
#define     IDC_TIME                105


#define     CALLBACK_SETAUTOON      WM_USER+100
#define     TIMER_ASURE             WM_USER+101
#define		TIMER_ID				WM_USER + 105

#define     SN_AUTO_CLOSE           "auto_close_open"        //auto close & open
#define     KN_AUTO_CLOSE_HOUR      "auto_close_hour"   //the hour of auto close
#define     KN_AUTO_CLOSE_MINUTE    "auto_close_minute" //the minute of auto close
#define     KN_AUTO_CLOSE_STATE     "auto_close_state"  //the state of auto close
#define     KN_AUTO_OPEN_HOUR       "auto_open_hour"   //the hour of auto open
#define     KN_AUTO_OPEN_MINUTE     "auto_open_minute" //the minute of auto open
#define     KN_AUTO_OPEN_STATE      "auto_open_state"  //the state of auto open

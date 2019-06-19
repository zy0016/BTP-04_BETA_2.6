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

#ifndef __ADDRESSBROWSER_H
#define __ADDRESSBROWSER_H

#ifdef __cplusplus
extern "C" {
#endif


#define     PB_SETGROUP     (WM_USER+91)
#define     PB_SETFIELD     (WM_USER+92)
#define     PB_SETSORT      (WM_USER+93)

#define     PBN_DESTROY     (WM_USER+94)

#define     PB_GETSELECT    (WM_USER+95)

#define     PB_SETNAME      (WM_USER+96)  // set name
#define     PB_SETTELPHONE  (WM_USER+97)  // set phonenumber
#define     PB_SETEMAIL     (WM_USER+98)  // set email

#define     SELECT_NUMBER      0  // select phone number 
#define     SELECT_NAME        1  // select name
#define     SELECT_EMAIL       2  // select email

#define     SETPHONE_MOBILE      0  // phone 1
#define     SETPHONE_WORK        1  // phone 2
#define     SETPHONE_HOME        2  // phone 3

#define     GRP_SIM             "SIM"
#define     GRP_FAMILY          "family"
#define     GRP_FRIEND          "friend"
#define     GRP_COLLEAGUE       "colleague"
#define     GRP_BUSINESS        "business"
#define     GRP_FREE1           "free1"
#define     GRP_FREE2           "free2"
#define     GRP_FREE3           "free3"
#define     GRP_STRANGE         "strange"

HWND CreateAddressBrowser(HWND hWndOwner);
HWND SaveAsPB(HWND hWndOwner);

#ifdef __cplusplus
}
#endif

#endif  // __ADDRESSBROWSER_H
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
#ifndef _MU_CONST_H_
#define _MU_CONST_H_

#define PATH_MAXLEN         256

//#define PATH_DIR_MU         "/mnt/flash/message/mu"
//#define PATH_DIR_MSG        "/mnt/flash/message"
//
//#define PATH_FILE_FOLDER    "/mnt/flash/message/mu/folder.inf"
//#define PATH_FILE_TEMP      "/mnt/flash/message/mu/tmp.inf"
//#define ICON_MU_MAIN		"ROM:/message/unibox/mu_main.ico"

#define PATH_DIR_MU			"/mnt/flash/unibox"
#define PATH_DIR_MSG        "/mnt/flash/unibox"
#define PATH_FILE_FOLDER    "/mnt/flash/unibox/folder.inf"
#define PATH_FILE_TEMP      "/mnt/flash/unibox/tmp.inf"
#define ICON_MU_MAIN		"ROM:/message/unibox/mu_main.ico"

#define MU_BMP_SELECT           "/rom/contact/rb_select.bmp"
#define MU_BMP_NORMAL           "/rom/contact/rb_normal.bmp"

#define MSG_SUCCEED         0
#define MSG_ERR_LOWMEMORY   -100
#define MSG_ERR_EMAIL       -101
#define MSG_ERR_SMS         -102
#define MSG_ERR_MMS         -103
#define MSG_ERR_PUSH        -104

#define ICON_OPTIONS        "Options"
#define ICON_SELECT         "Select"

typedef BOOL  (*CMPFUNC)( DWORD index1, DWORD index2 );

#endif // _MU_CONST_H_

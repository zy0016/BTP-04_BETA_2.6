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

#ifndef WIE_REMOTE_H
#define WIE_REMOTE_H

#include "wUipub.h"

BOOL  On_IDM_REMOTE(HWND hParent, BOOL bEdit, RECT rClient);
void  Destroy_LF_WND();
BOOL  Create_LF_WND(HWND hParent, RECT rClient);
void  On_IDM_LF(HWND hParent, RECT rClient);

#define WIE_IDM_ADDTOBM          (WM_USER+112)
#define WIE_IDM_REMOTE_OK        (WM_USER+401) 
#define WIE_IDM_REMOTE_CLOSE     (WM_USER+402)
#define WIE_IDM_LOCAL_OK         (WM_USER+201)
#define WIE_IDM_LOCAL_CLOSE      (WM_USER+202)
#define WIE_IDM_LOCAL_DEL        (WM_USER+200)
#define WIE_IDM_FILE_LIST        (WM_USER+100)
#define WIE_IDM_LOCAL_DELCONFIRM (WM_USER+101)
#define WIE_IDM_LOCAL_DELALLCONFIRM (WM_USER+102)
#define WIE_IDM_LOCAL_DELSELCONFIRM (WM_USER+103)

extern char* strlwr(char*);

#endif

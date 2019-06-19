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

#ifndef _MMSCOTRL_H
#define _MMSCOTRL_H 

#include "window.h"
#define MMS_SETADDRESS	(WM_USER + 2000)
#define MMS_ADDCONTENT	(WM_USER + 2001)
#define MMSN_DESTROY	(WM_USER + 2002)

#define CTRL_CONTENT_TEXT	1
#define CTRL_CONTENT_AUDIO	2
#define CTRL_CONTENT_IMAGE	3

//wParam		invalide , must be 0
//lParam		pointer to a struct defined as following
typedef struct tagCTRLMMCOTNECT 
{
	int		mode;			// 0 : means date in file, 1 : means data in memory
	int     ContentType;	// content type such as text , audio or image
	long	length;			// when mode is 1, this field is memory lenght
	char*   pData;			// when mode is 0, pData is file name
							// when mode is 1, pData is memory pointer
}CTRLMMCONTENT, *PCTRLMMCONTENT;

void* CreateMMSEdit (void* hOwner);

#endif
/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : ¿Õ¼ä²é¿´
 *            
\**************************************************************************/
#ifndef _FAVORITESHORTCUR_
#define	_FAVORITESHORTCUR_

#include "window.h"
#include "plx_pdaex.h"
#include "winpda.h"
#include "pubapp.h"
#include "mullang.h"

#include "stdio.h"
#include "stdlib.h"

#include "sys/ioctl.h"
#include "sys/statfs.h"
#include "sys/stat.h"
#include "fcntl.h"

#include "callapp/MBPublic.h"
#include "setup/setup.h"

#define	FAVORITEWC	"FAVORATEWNDCLASS"
#define	ADDSHORTWC	"ADDSHORTCURWNDCLS"
#define	RENAMEWCLS	"RENAMEWNDCLASS"
#define	ADDFEATURE	"ADDFEATUREWNDCLASS"

#define	IDC_ADD		(WM_USER+1000)
#define	IDC_MOVE	(WM_USER+1001)
#define	IDC_RENAME	(WM_USER+1002)
#define	IDC_REMOVE	(WM_USER+1003)
#define	IDC_OPEN	(WM_USER+1004)

#define	IDC_EXIT	(WM_USER+1005)
//#define	IDC_ADDOPEN	(WM_USER+1006)
#define	IDC_CANCEL	(WM_USER+1007)
#define	IDC_OK		(WM_USER+1008)
#define	IDRM_OPENING	(WM_USER+1013)
#define	IDRM_ADDFEATURE	(WM_USER+1014)


#define	IDC_LIST	1//(WM_USER+1010)
#define IDC_ADDLIST	2//(WM_USER+1011)
#define	IDC_RNMEDIT	3//(WM_USER+1012)


#define	ID_RBTIMER		4
#define	IDC_FEATURELIST	5


#define	IDS_YES		ML("Yes")
#define	IDS_NO		ML("No")
#define	IDS_ADD		ML("Add")
#define	IDS_MOVE	ML("Move")
#define	IDS_RENAME	ML("Rename")
#define	IDS_REMOVE	ML("Remove")
#define	IDS_OPEN	ML("Open")
#define	IDS_EXIT	ML("Exit")
#define	IDS_CANCEL	ML("Cancel")
#define	IDS_ADDPROMT	ML("Shortcut added")
#define	IDS_OK		ML("OK")
#define	IDS_SAVE	ML("Save")
#define	IDS_OPENING	ML("Opening")
#define	IDS_NEWSHORTCUT	ML("New shortcut")

#define	IDS_FAVOR		ML("Favorites")
#define	IDS_ADDCAPTION	ML("New shortcut")
#define	IDS_RNMCAPTION	ML("Rename shortcut")
#define	IDS_MOVECAPTION	ML("Move shortcut")
#define	IDS_RNMNOTIFY	ML("Please define name")
#define	IDS_NAME		ML("Shortcut Name:")
#define	IDS_REMOVESHORTCUT	ML("Remove?")
#define	IDS_SCREMOVED		ML("Removed")
#define	IDS_NOSHORTCUT	ML("No shortcuts")

#define	FAVORCONF	"/mnt/flash/favorite/favor.ini"
#define RADIO_OFF   "/rom/public/radiooff.bmp"//"ROM:ime/radiooff.bmp"
#define RADIO_ON    "/rom/public/radioon.bmp"//"ROM:ime/radioon.bmp"
#define	SHORTCURICON	"/rom/pim/favorites_category2.bmp"
#define	NEWSHORTCURICON	"/rom/pim/favorites_category1.bmp"


#endif

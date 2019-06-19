/**************************************************************************\
 *
 *                      Pollex Software System
 *
 * Copyright (c) Pollex Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
\**************************************************************************/
#ifndef _DLMUI_H_
#define _DLMUI_H_

#include "str_public.h"
#include "str_plx.h"

#define WIN_TITLE		ML("MANGER_WIN_TITLE")		//"Manager"

#define DLM_DIR_CARD			"/mnt/fat16/dlm/"
#define DLM_DIR_MOBILE			"/mnt/flash/dlm/"
#define DLM_DEV_FLASH			"/mnt/flash"
#define DLM_DEV_CARD			"/mnt/fat16"

#define DLM_EXT			".plx"

#define DLM_DEFAULT_ICON1		"/rom/progman/app/mdefaultapp_43x28.bmp"
#define DLM_DEFAULT_ICON2		"/rom/progman/app/mgdefaultapp_43x28.bmp"


#define IDS_TOTALFILE	ML("MANGER_IDS_TOTALFILE")	//"Total %d file(s)."
#define IDS_NOFILE		ML("MANGER_IDS_NOFILE")		//		"No files"
#define IDS_CLASSNAME	"Dlm_MainWndClass"
#define IDS_DEVICEMC	"fat16"

#define IDS_SELECT		ML("MANGER_IDS_SELECT")		//"Select"
#define IDS_OPTION		ML("MANGER_IDS_OPTION")		//"Options"

#define IDS_INSTALL		ML("MANGER_IDS_INSTALL")		//"Install"

#define IDS_MMC			ML("MANGER_IDS_MMC")		//"Memory card"
#define IDS_FLASH		ML("MANGER_IDS_FLASH")		//"Phone memory"

#define IDS_BACK		ML("MANGER_IDS_BACK")		//"Back"
#define IDS_EXIT		ML("MANGER_IDS_EXIT")		//"Exit"
#define IDS_ENTER		ML("MANGER_IDS_ENTER")		//"Enter"
#define IDS_RUN			ML("MANGER_IDS_RUN")		//"Run"
#define IDS_INSTALL		ML("MANGER_IDS_INSTALL")		//"Install"
#define IDS_DETAIL		ML("MANGER_IDS_DETAIL")		//"Details"
#define IDS_VIAEMAIL	ML("MANGER_IDS_VIAEMAIL")		//"Via e-mail"
#define IDS_VIABT		ML("MANGER_IDS_VIABT")		//"Via Bluetooth"
#define IDS_SEND		ML("MANGER_IDS_SEND")		//"Send"
#define IDS_DELETE		ML("MANGER_IDS_DELETE")		//"Delete"
#define	IDS_CANCEL		ML("MANGER_IDS_CANCEL")		//"Cancel"
#define IDS_OK			ML("MANGER_IDS_OK")		//"Ok"
#define IDS_DELETED		ML("MANGER_IDS_DELETED")		//"Deleted"
#define IDS_DETAILS		ML("MANGER_IDS_DETAILS")		//"Details"

#define IDS_INF_FILENAME "$$$PLX$$$.inf"

#define IDS_CARD		ML("MANGER_IDS_CARD")		//"Memory card"
#define IDS_MOBILE		ML("MANGER_IDS_MOBILE")		//"Phone memory"
#define IDS_INSTALLING  ML("MANGER_IDS_INSTALLING")		//"Installing..."
#define IDS_DELETING	ML("MANGER_IDS_DELETING")		//"Deleting..."
#define IDS_LACK_OF_MEMORY ML("MANGER_IDS_LACK_OF_MEMORY")		//"Not enough memory"
#define IDS_COMPLETE	ML("MANGER_IDS_COMPLETE")		//"Complete"
#define IDS_INSTALL_FAILED ML("MANGER_IDS_INSTALL_FAILED")		//"Installing failed"
#define IDS_NOT_PLXFILE	ML("MANGER_IDS_NOT_PLXFILE")		//"Not a PLX file."
#define IDS_YES				ML("MANGER_IDS_YES")		//"Yes"
#define IDS_NO				ML("MANGER_IDS_NO")		//"No"
#define IDS_CANNOTRUN	ML("MANGER_IDS_CANNOTRUN")		//"Cannot perform.\nToo many applications running."
#define IDS_NOCARD		ML("MANGER_IDS_NOCARD")		//"No memory card"

#define IDS_ASKDELETE	ML("MANGER_IDS_ASKDELETE")	//"Delete?"
#define IDS_NAME		ML("MANGER_IDS_NAME")	//"Name:"
#define IDS_VERSION		ML("MANGER_IDS_VERSION")	//"Version:"
#define IDS_COPYRIGHT	ML("MANGER_IDS_COPYRIGHT")	//"Copyright:"
#define IDS_FILETYPE	ML("MANGER_IDS_FILETYPE")	//"Type:"
#define IDS_FLASHMEM	ML("MANGER_IDS_FLASHMEM")	//"FLASH"
#define IDS_MMCCARD		ML("MANGER_IDS_MMCCARD")	//"Memory card"	
#define IDS_LOCATION	ML("MANGER_IDS_LOCATION")	//"Location:"
#define ID_EXIT			100
#define ID_RUN			(WM_USER + 300)
#define ID_INSTALL		(WM_USER + 301)
#define ID_ENTER		(WM_USER + 302)
#define ID_BACK			(WM_USER + 303)
#define ID_DETAIL		(WM_USER + 304)
#define ID_VIAEMAIL		(WM_USER + 305)
#define ID_VIABT		(WM_USER + 306)
#define ID_DELETE		(WM_USER + 307)
#define ID_INSTALLTO	(WM_USER + 308)

#define ID_LISTBOX		(WM_USER + 354)
#define ID_RADIOFLS		(WM_USER + 355)
#define ID_RADIOMC		(WM_USER + 356)
#define ID_INSTALL_CANCEL (WM_USER + 357)
#define ID_INSTALL_OK (WM_USER + 358)
#define ID_INSTALL_COMPLETE (WM_USER + 359)
#define ID_LACK_OF_MEMORY (WM_USER+360)
#define ID_DELETE_COFIRM (WM_USER+361)
#define ID_DELETED		(WM_USER+362)
#define ID_NOT_PLXFILE	(WM_USER+363)
#define ID_CANNOTRUN	(WM_USER+364)
#define ID_NOCARD		(WM_USER+365)

#define ID_NEEDNOT		(WM_USER+750)


extern int dlm_CanQuit () ;
extern int dlm_LoadModule (unsigned char *fn) ;

#define ITEM_DIR	0x1
#define ITEM_FILE   0x2

#define ICON_MMC "/rom/pim/notepad/newnote.bmp"
#define ICON_FLS "/rom/pim/notepad/note.bmp"

#define ICON_INSTALLED "/rom/pim/notepad/newnote.bmp"
#define ICON_UNINSTALLED "/rom/pim/notepad/note.bmp"

#define ICON_EMPTY "/rom/pim/notepad/newnote.bmp"

#define LEFT_ICON "/rom/mbrecord/left.bmp"
#define RIGHT_ICON "/rom/mbrecord/right.bmp"
#define SELECT_ICON "/rom/pim/notepad/newnote.bmp"
#define UNSELECT_ICON "/rom/pim/notepad/note.bmp"


// define install stated
#define INSTALL_OK					0 
#define INSTALL_CANCELED			-1
#define INSTALL_FAILED				-2



#define SECOND(n) (n*10)
#endif



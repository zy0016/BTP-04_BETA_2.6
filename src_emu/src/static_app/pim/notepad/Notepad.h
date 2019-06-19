/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Notepad.h
 *
 * Purpose  : 
 *
\**************************************************************************/

#ifndef _NOTEPADHEAD_
#define _NOTEPADHEAD_

#include "str_public.h"
#include "str_plx.h"
#include "mullang.h"

#define IDS_NOTEPAD			ML("Notepad")
#define IDS_EXIT			ML("Exit")				
#define IDS_NEW				ML("New")					
#define IDS_OPEN			ML("Open")							
#define IDS_DELETE			ML("Delete")					
#define IDS_SEND			ML("Send")				
#define IDS_SMS				ML("Via SMS")					
#define IDS_MMS				ML("Via MMS")			
#define IDS_MAIL			ML("Via e-mail")
#define IDS_BLUETOOTH		ML("Via Bluetooth")
#define IDS_EDIT			ML("Edit")									
#define IDS_CONFIRM			ML("Ok")				
#define IDS_BACK			ML("Back")					
#define IDS_CANCEL			ML("Cancel")				
#define IDS_NOFREESPACE		ML("Memory full\nCannot save note.")
#define IDS_DELCONFIRM		ML("Delete selected notes?")
#define IDS_DELSUCCESS		ML("Deleted")
#define IDS_DELFAIL			ML("Fail to\ndelete")
#define IDS_DELALLCONFIRM	ML("Delete all notes?")
#define IDS_INVALIDCHAR		ML("Invalid file name!")				
#define IDS_SAVENBOOK		ML("Save note?")	
#define IDS_YES				ML("Yes")
#define IDS_NO				ML("No")				
#define IDS_NEWNOTE			ML("New note")
#define IDS_DELEMANY		ML("Delete many")
#define IDS_DELE_SELECT		ML("Selected")
#define IDS_DELE_ALL		ML("All")
#define IDS_VIEWTITLE		ML("Note")
#define IDS_MODIFYBOOK		ML("Note")
#define IDS_OK				ML("Ok")
#define IDS_WRITE			ML("Please write note")
#define IDS_NULTISELECT		ML("Select notes")
#define IDS_NOSELECT		ML("No notes selected")
#define IDS_SAVE			ML("Save")

#define EDIT_CANCEL				-1
#define EDIT_CHANGED			1
#define EDIT_OVERWRITE			2
#define EDIT_NOVERWRITE			3
#define EDIT_EMPTY				0

#define ID_MENU				99
#define ID_EXIT				100
#define ID_EDIT				101
#define ID_OPEN				102
#define ID_RENAME			103
#define ID_DELETE			104
#define ID_DELEMANY			105
#define ID_DELE_SELECT		106
#define ID_DELE_ALL			107
#define ID_SEND				201
#define ID_SEND_SMS			202
#define ID_SEND_MMS			203
#define ID_SEND_MAIL		204
#define ID_SEND_BLUETOOTH	205
#define ID_NEW				206
#define ID_CANCEL			207
#define ID_SAVE				208
#define CONFIRM_DELETE		WM_USER + 300
#define CONFIRM_DELETEALL	WM_USER + 301
#define CONFIRM_SAVE		WM_USER + 302
#define CONFIRM_DELETE_OK	WM_USER + 303
#define CONFIRM_NEW			WM_USER + 304
#define CONFIRM_DELETED		WM_USER + 305
#define CONFIRM_INPUT		WM_USER + 306
#define EDIT_SAVE_OK		WM_USER + 307
#define NEW_SAVE_OK			WM_USER + 308
#define VIEW_SAVE_OK     WM_USER + 309
#define ID_CONFIRM			101
#define ID_BACK				102
#define ID_MODIFY			201
#define ID_MAINLIST			301
#define ID_DELETELIST		302
#define IME_MSG_TEXTEXIT	303
#define IME_MSG_NAMEEXIT	304
#define IME_CLOSE			305

	

//#define ICON_PATH				"/rom/pim/icon_clock.gif"
#define ICON_NEWNOTE			"/rom/pim/notepad/newnote.bmp"
#define ICON_NOTE			"/rom/pim/notepad/note.bmp"
#define NOTEPAD_ICON_ARROWRIGHT	"/rom/public/arrow_right.ico"
#define NOTEPAD_ICON_ARROWLEFT	"/rom/public/arrow_left.ico"
#endif // _NOTEPADHEAD_

typedef struct  tagEditWnd{
	char *pContent;
	int  lenth;
	BOOL bIsViewWnd;
	BOOL bIsNewNote;
}EDITWNDUSED;

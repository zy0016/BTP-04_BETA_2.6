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
#include "mullang.h"

#include "window.h"

#define IDS_HELP		ML("Help")
#define IDS_HELPINDEX	ML("Help index")
#define IDS_MENU		ML("Menu")
#define IDS_OPEN		ML("Open")	
#define IDS_CATEGORY	ML("Category List")	
#define IDS_SEARCH		ML("Search by keyword")
#define IDS_EXIT		ML("Exit")
#define IDS_BACK		ML("Back")
#define IDS_OK			ML("Ok")
#define HELP_ERROR		ML("No Find")
#define HELP_NOSPACE	ML("No enough space!")
#define HELP_NOCONTENT  ML("Have nothing")


#define ID_MENU			300	
#define ID_OPEN			301
#define ID_CATEGORY		302
#define ID_SEARCH		303
#define ID_EXIT			304
#define ID_MAINLIST		305
#define ID_SEARCHEdit	306
#define ID_SUBMENU			307	
#define ID_SUBOPEN			308
#define ID_SUBCATEGORY		309
#define ID_SUBSEARCH		310
#define ID_BACK				311
#define ID_TIMER			312
#define ID_OK				313

#define BMP_WIDTH1			22
#define BMP_WIDTH2			18
#define BMP_HIGTH			16

static RECT HelpListRect = {0, 0, 174, 90};
static RECT SearchBmpRect = {80, 97, 80+BMP_WIDTH2, 97 + BMP_HIGTH};
static RECT SearchEditRect = {0, 121, 174, 150};


HWND hHelpFrameWnd;

#define TOPIC_MAX			30
#define LIST_MAXNUM			30
#define TOPIC_NAMELENGTH	30	
#define IsTopic				1
#define IsText				0
#define ContentMaxLength	512
#define KeyMaxLength		50
#define SectionMaxLength    50

#define TopicFilePath        "/rom/help/help_list.ini" 
#define ContentFilePath      "/rom/help/help_content.ini"

#define ICON_FOLDER			"/rom/help/folder.bmp"
#define ICON_DOC			"/rom/help/document.bmp"
#define ICON_SEARCH			"/rom/help/search.bmp"
#define ICON_RIGHTARROW		"/rom/public/arrow_right.ico"
#define ICON_LEFTARROW		"/rom/public/arrow_left.ico"

/*
typedef struct  tagRecordInfo{
	int iPlaceInArray;
	char **pArray_Addr;
}RECORDINFO;*/



static char *cTopic[] =
{
	"messaging",
	"extras",
	"fun",
	"contacts",
	"logs",
	"tools",
	"wap",
	"favorites",
//	"Profiles",
	0,
};



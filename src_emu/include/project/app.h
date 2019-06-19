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

#ifndef __APP_H
#define __APP_H

#ifndef _WINDEF_
#include "hpdef.h"
#endif


typedef DWORD (*APPCONTROL)(int nCode, void* pInstance,
										WPARAM wParam, LPARAM lParam );

#define PMNODE_NAMELEN		40
#define PMICON_NAMELEN		256//128
#define DIR_TYPE            0
#define APP_TYPE            1

// Define struct for a subapplication 
typedef struct tagAPPLICATION
{
    char  achName[PMNODE_NAMELEN];	 // Name of the application
	char  achTitleName[PMNODE_NAMELEN]; //as title to display
	char  strIconName[PMICON_NAMELEN];	// Name of the application Icon file
 	short attrib;                 
    APPCONTROL AppControl;		// Application control entry
} APPLICATION, *PAPPLICATION;

extern const APPLICATION* AppTable[];

typedef struct tagAPPGROUP
{
    char  achName[PMNODE_NAMELEN];	// Name of the application
	char  achTitleName[PMNODE_NAMELEN]; //as title to display
	char  strIconName[PMICON_NAMELEN];	// Name of the application Icon file
	short  nType;                   //1:app; 0;dir   node type
	short  attrib; 
	APPLICATION **AppTable;  
	APPCONTROL AppControl;
	
} APPGROUP, *PAPPGROUP;

extern const APPGROUP* GrpTable[];


#endif

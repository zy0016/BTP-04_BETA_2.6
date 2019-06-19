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

#ifndef __PATH_H
#define __PATH_H

#ifndef _WINDEF_
#include "hpdef.h"
#endif


#define APP_NAMELEN		32

// Define struct for a appliction path
typedef struct tagAPPPATH
{
    char achName[APP_NAMELEN];	// Name of the path
	char **strChildName;
} APPPATH, *PAPPPATH;

extern const APPPATH* AppPath[], *AppPathMmc[];

#endif

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

#ifndef _HMAP_ANSIC_H_
#define _HMAP_ANSIC_H_

#if defined(WIN32) || defined(HMAP_DLL)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef  isblank 
#define  isblank    isspace
#endif

#endif

#ifdef LINUX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef  isblank 
#define  isblank    isspace
#endif

extern char * strupr(char * str);
#define stricmp	    strcasecmp
#define strnicmp    strncasecmp

#endif

#ifdef HP30
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef  isblank 
#define  isblank    isspace
#endif

extern char * strupr(char * str);
#endif

#endif /* _HMAP_ANSIC_H_ */

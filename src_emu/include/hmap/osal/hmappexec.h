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

#ifndef __HMAPPEXEC_H
#define __HMAPPEXEC_H

//#if ( defined _SP || defined COMBIND_APP )

typedef int (*AppEntry)(int argc, char ** argv);

struct app_table {
		AppEntry		appEntry;
		char			appName[16];
};

//#endif

#ifdef __cplusplus
extern "C" {
#endif

int AppExec ( const char * appName, const char * cmdLine, int mode );

#ifdef __cplusplus
}
#endif

#endif  // __HMAPPEXEC_H
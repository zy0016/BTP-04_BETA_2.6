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

#ifndef _HMFIND_H_
#define _HMFIND_H_

typedef struct hm_findentry {
	int	filetype;
    unsigned long filesize;
    unsigned long fileatime;
    unsigned long filemtime;
    unsigned long filectime;
	const char * filename;
} HMFINDENTRY, * PHMFINDENTRY;

typedef struct hm_findhandle {
	HMFINDENTRY	find;
	/* other fields not obvious to user */
} * HMFINDHANDLE;

#define HMFT_FILE       0x0001
#define HMFT_DIR		0x0002
#define HMFT_OTHER      0x0003

#ifdef __cplusplus
extern "C" {
#endif

HMFINDHANDLE hm_FindFirstFile (const char * filename);
int hm_FindNextFile (HMFINDHANDLE findHandle);
void hm_FindClose (HMFINDHANDLE findHandle);

#ifdef __cplusplus
}
#endif

#endif  //_HMFIND_H_





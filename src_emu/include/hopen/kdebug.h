/**************************************************************************\
 *
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :
 *
 * Purpose :     
 *  
 * Author  :     WangXinshe
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/kdebug.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:58:59 $
 * 
\**************************************************************************/

#ifndef _HOPEN_KDEBUG_H_
#define _HOPEN_KDEBUG_H_

enum arg_type { ARG_STRING, ARG_NUMBER };

struct cmdarg {
	int	type;
	unsigned long value;
	char * pstr;
};

#define MAX_CMDARGS	10

extern int k_sprintf( char * pStr, const char *fmt, ... );

extern void debug_cmd_help (void);
extern void debug_cmd_invalid (void);

extern void debug_cmd_dm (int argc, struct cmdarg * argp);
extern void debug_cmd_lt (int argc, struct cmdarg * argp);

extern void (*debug_out)(const char * str);

#endif // _HOPEN_KDEBUG_H_

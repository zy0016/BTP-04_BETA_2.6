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
 * $Source: /cvs/hopencvs/src/include/hopen/gdbstub.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:58:59 $
 * 
\**************************************************************************/

#ifndef _HOPEN_GDBSTUB_H_
#define	_HOPEN_GDBDTUB_H_

struct gdb_sysinfo
{
	unsigned short	os_version;
	unsigned char	cpu_type;
	unsigned char	mach_type;
	void *			k_data;
	unsigned short	k_data_size;
};

extern int gdbstub_init (void);

#endif  // _GDBSTUB_H_
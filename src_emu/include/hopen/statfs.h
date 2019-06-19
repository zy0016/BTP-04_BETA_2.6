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
 * $Source: /cvs/hopencvs/src/include/hopen/statfs.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _HOPEN_STATFS_H
#define _HOPEN_STATFS_H

#ifndef __STRUCT_STATFS
#define __STRUCT_STATFS
struct statfs {
	long	f_type;
	long	f_bsize;
	long	f_blocks;
	long	f_bfree;
	long	f_bavail;
	long	f_files;
	long	f_ffree;
	long	f_fsid;
	long	f_namelen;
	long	f_spare[6];
};
#endif	//__STRUCT_STATFS

#endif

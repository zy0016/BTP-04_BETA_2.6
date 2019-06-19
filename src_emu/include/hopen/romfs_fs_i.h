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
 * $Source: /cvs/hopencvs/src/include/hopen/romfs_fs_i.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _HOPEN_ROMFS_FS_I
#define _HOPEN_ROMFS_FS_I

/* inode in-kernel data */

struct romfs_inode_info {
	unsigned long i_metasize;	/* size of non-data area */
	unsigned long i_dataoffset;	/* from the start of fs */
};

#endif

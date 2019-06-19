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
 * $Source: /cvs/hopencvs/src/include/hopen/romfs_fs.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 * 
\**************************************************************************/

#ifndef _HOPEN_ROMFS_FS_H
#define _HOPEN_ROMFS_FS_H

/* The basic structures of the romfs filesystem */

#define ROMBSIZE	BLOCK_SIZE
#define ROMBSBITS	BLOCK_SIZE_BITS
#define ROMBMASK	(ROMBSIZE-1)
#define ROMFS_MAGIC	0x7275

#define ROMFS_MAXFN 128

/* On-disk "super block" */

struct romfs_super_block {
	unsigned long word0;
	unsigned long word1;
	unsigned long size;
	unsigned long checksum;
//	char name[0];		/* volume name */
};

/* On disk inode */

struct romfs_inode {
	unsigned long next;	/* low 4 bits see ROMFH_ */
	unsigned long spec;
	unsigned long size;
	unsigned long checksum;
//	char name[0];
};

#define ROMFH_TYPE  7
#define ROMFH_HRD   0
#define ROMFH_DIR   1
#define ROMFH_REG   2
#define ROMFH_SYM   3
#define ROMFH_BLK   4
#define ROMFH_CHR   5
#define ROMFH_SCK   6
#define ROMFH_FIF   7
#define ROMFH_EXEC  8

/* Alignment */

#define ROMFH_SIZE 16
#define ROMFH_PAD (ROMFH_SIZE-1)
#define ROMFH_MASK ((unsigned)~ROMFH_PAD)

#include <hopen/romfs_fs_i.h>
#include <hopen/romfs_fs_sb.h>

extern int init_romfs_fs(void);

#endif

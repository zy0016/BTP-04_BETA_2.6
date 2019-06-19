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
 * Author  :     zhaohong
 *
 *-------------------------------------------------------------------------
 *
 * $Source: /cvs/hopencvs/src/include/hopen/devfs_fs.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/02/04 08:45:06 $
 * 
\**************************************************************************/


#ifndef _HOPEN_DEVFS_FS_H
#define _HOPEN_DEVFS_FS_H

#define FIRST_INODE 1
#define DEVFS_SUPER_MAGIC                0x1373

#define MODE_DIR (S_IFDIR | S_IWUSR | S_IRUGO | S_IXUGO)

#define DEVFS_FL_NONE           0x000 /* This helps to make code more readable
				       */
#define DEVFS_FL_AUTO_OWNER     0x001 /* When a closed inode is opened the
					 ownerships are set to the opening
					 process and the protection is set to
					 that given in <<mode>>. When the inode
					 is closed, ownership reverts back to
					 <<uid>> and <<gid>> and the protection
					 is set to read-write for all        */
#define DEVFS_FL_HIDE           0x002 /* Do not show entry in directory list */
#define DEVFS_FL_AUTO_DEVNUM    0x004 /* Automatically generate device number
				       */ 
#define DEVFS_FL_REMOVABLE      0x010 /* This is a removable media device    */ 
#define DEVFS_FL_CURRENT_OWNER  0x040 /* Set initial ownership to current    */
#define DEVFS_FL_DEFAULT        DEVFS_FL_NONE


#define DEVFS_SPECIAL_CHR     0
#define DEVFS_SPECIAL_BLK     1

typedef struct devfs_entry * devfs_handle_t;

extern int devfs_alloc_major (char type);
extern void devfs_dealloc_major (char type, int major);

#endif /* _HOPEN_DEVFS_FS_H */

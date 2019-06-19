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
 * $Source: /cvs/hopencvs/src/include/hopen/mount.h,v $
 * $Name:  $
 *
 * $Revision: 1.2 $     $Date: 2004/12/13 06:19:23 $
 * 
\**************************************************************************/

#ifndef _HOPEN_MOUNT_H
#define _HOPEN_MOUNT_H

struct dentry;
struct super_block;

extern struct dentry * find_mount_dir(struct dentry *);
extern struct super_block * find_mount_super(int dev);

/* Find device according to mounting entry */
extern int find_mount_dev (struct dentry *);

#endif //_HOPEN_MOUNT_H

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
 * $Source: /cvs/hopencvs/src/include/sys/mount.h,v $
 * $Name:  $
 *
 * $Revision: 1.3 $     $Date: 2003/03/26 06:22:34 $
 * 
\**************************************************************************/

#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

enum
{
  MS_RDONLY = 1,		/* Mount read-only.  */
#define MS_RDONLY	MS_RDONLY
  MS_NOSUID = 2,		/* Ignore suid and sgid bits.  */
#define MS_NOSUID	MS_NOSUID
  MS_NODEV = 4,			/* Disallow access to device special files.  */
#define MS_NODEV	MS_NODEV
  MS_NOEXEC = 8,		/* Disallow program execution.  */
#define MS_NOEXEC	MS_NOEXEC
  MS_SYNCHRONOUS = 16,		/* Writes are synced at once.  */
#define MS_SYNCHRONOUS	MS_SYNCHRONOUS
  MS_REMOUNT = 32,		/* Alter flags of a mounted FS.  */
#define MS_REMOUNT	MS_REMOUNT
  MS_MANDLOCK = 64,		/* Allow mandatory locks on an FS.  */
#define MS_MANDLOCK	MS_MANDLOCK
  S_QUOTA = 128,		
#define S_QUOTA		S_QUOTA
  S_APPEND = 256,		/* Append-only file.  */
#define S_APPEND	S_APPEND
  S_IMMUTABLE = 512,		/* Immutable file.  */
#define S_IMMUTABLE	S_IMMUTABLE
  MS_NOATIME = 1024,		/* Do not update access times.  */
#define MS_NOATIME	MS_NOATIME
  MS_NODIRATIME = 2048,		/* Do not update directory access times.  */
#define MS_NODIRATIME	MS_NODIRATIME
};

long mount(char * dev_name, char * dir_name, char * fstype, unsigned flags, void *data);
long umount(const char * dir_name);

#ifdef __cplusplus
}
#endif

#endif

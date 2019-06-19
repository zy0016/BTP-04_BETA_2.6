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
 * $Source: /cvs/hopencvs/src/include/sys/utsname.h,v $
 * $Name:  $
 *
 * $Revision: 1.1.1.1 $     $Date: 2002/12/26 07:59:00 $
 *
\**************************************************************************/

#ifndef	_SYS_UTSNAME_H
#define	_SYS_UTSNAME_H

#define _UTSNAME_LENGTH			32
#define _UTSNAME_NODENAME_LENGTH	32

/* Structure describing the system and machine.  */
struct utsname {
    /* Name of the implementation of the operating system.  */
    char sysname[_UTSNAME_LENGTH];

    /* Name of this node on the network.  */
    char nodename[_UTSNAME_NODENAME_LENGTH];

    /* Current release level of this implementation.  */
    char release[_UTSNAME_LENGTH];

    /* Current version level of this release.  */
    char version[_UTSNAME_LENGTH];

    /* Name of the hardware type the system is running on.  */
    char machine[_UTSNAME_LENGTH];
};

#ifdef __cplusplus
extern "C" {
#endif

/* Put information about the system in NAME.  */
int uname (struct utsname *__name);

#ifdef __cplusplus
}
#endif

#endif /* sys/utsname.h  */

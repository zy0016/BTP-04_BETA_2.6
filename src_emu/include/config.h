/**************************************************************************\
**                                                                        **
**                         HOPEN OS Program                               **
**                                                                        **
**  Copyright (C) 1998 by CASS Corptation & Software Engineering Center   **
**                       All Rights Reserved                              **
**                                                                        **
** The contents of this file are subject to the HOPEN License;            **
**                                                                        **
** Software distributed under this packet is distributed on an "AS IS"    **
** basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.        **
**                                                                        **
** Duplicate this file without HOPEN license is illegal.                  **
**                                                                        **
\**************************************************************************/

/***************************************************************************
 *
 * Sysytm header file
 *
 * $Name:  $
 *
 * $Revision: 1.1 $  $Date: 2004/02/13 07:04:41 $  $State: Exp $
 *
 * $Author: LiChun_k $
 *
 ***************************************************************************
 */

#ifndef _ARM_CONFIG_H
#define _ARM_CONFIG_H

//platforms
#define DRAGONBALL		1	//MX1
#define INTERGRATOR		2	//arm integrator
#define NMS7210SDB		3	//MiMagic 3(NMS7210) System Development Board
#define DBPXA26X		4	//Intel DBPXA26x Development platform
#define BULVERDE		5	//Intel Bulverde Development platform
//cpus
#define ARM920 	1
#define ARM720	2
#define XSCALE	3

#define __BYTE_ORDER	__LITTLE_ENDIAN

#define	__KERNEL__
#define	__WITH_CONSOLE__
#define  __WITH_IPC__
//#define	__WITH_NET__
#undef  __WITH_NETDEBUG__
#define  __WITH_BLKDEV__
#define __RTC_TIME__

#undef  __NO_MALLOC__
#define __NO_MEMCHK__
#undef  __NO_SIGNAL__

#undef  __NO_SYSTIMER__

#define CONFIG_PM

#define	MAX_TASKS		100
#define	MAX_PROCS		15
#define MAX_LIBS		10

#define	TICKS_PERSECOND	 100

#define MAX_POLL		32

#define	MAX_IPCNODES		200
#define PIPE_BUF		4096

#define PATH_MAX		1023
#define NAME_MAX		255

#define	MAX_FILESYS		10
#define MAX_MOUNTS		8
#define	MAX_BLKDEV		16
#define	MAX_CHRDEV		32

#define MAX_DENTRIES		128
#define MAX_INODES		128

#define	MAX_FDS			64
#define	MAX_FILES		128

#define	SLEEP_HASH_TBLSIZE	32

#define CPU_ARCH		BULVERDE

#define USER_STACK_LIMIT	(1024*1024)

/* We use MMU for ARM architecture */
#ifndef __MMU__
#define __MMU__
#endif


#define	PAGE_OFFSET		0xC0000000
#define VMALLOC_START	0xD0000000
#define VMALLOC_SIZE	0x08000000
#define VMALLOC_END     0xD8000000

#if CPU_ARCH == BULVERDE

//#define CONFIG_MAINSTONE

#undef 	CPU_TYPE
#define	CPU_TYPE	XSCALE

#define PHYS_OFFSET		0xA0000000
#define RAM_SIZE		0x01c00000	/* 28M SDRAM*/
#define ROMMAP_BASE		0xf0000000
#define FLASHMAP_BASE	0xf2000000
#define FB_RAM_MAP_BASE	0xf4100000
#define FB_RAM_BASE		0xa1d00000

// ram fs max size
#undef  RAMFS_MAX_PAGE
#define RAMFS_MAX_PAGE	2048	/*RAM FS max size = 8M, 4k per page*/

// flash chip configs
#define CONFIG_MTD_CFI_GEOMETRY
#define CONFIG_MTD_CFI
#define CONFIG_MTD_CFI_B2
#define CONFIG_MTD_CFI_I1
#define CONFIG_MTD_CFI_INTELEXT
#define CONFIG_MTD_GEN_PROBE


#define CONFIG_NUM_IRQS 128

#else

#error platform not supported

#endif

#undef __KERNEL_GDB_DEBUG
//#define CONFIG_MMC_DEBUG
//#define CONFIG_JFFS2_FS_DEBUG 1

#endif /* _ARM_CONFIG_H */

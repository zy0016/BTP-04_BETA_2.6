/**************************************************************************\
 *                      Hopen Software System
 *
 * Copyright (c) 1999-2000 by Hopen Software Engineering Co., Ltd. 
 *                       All Rights Reserved
 *
 * Model   :	HOPEN Device Drive
 *
 * Purpose :	Device Drive
 *  
 * Author  :	Di Ling
 *
 *-------------------------------------------------------------------------
 *
 * $Archive:: /hopen2.0/include/Sysctl.h							$
 * $Workfile::	Sysctl.h											$
 * $Revision:: 4  $     $Date:: 03-04-26 3:28p						$
 *
\**************************************************************************/

/***************************************************************************
 *
 * Hopen Device Driver Interface
 *
 * $Revision: 2 $     $Date: 00-03-24 9:21 $    
 *
 * $Log: /Hopen/include/ddi/Sysctl.h $
 * 
 * 2     00-03-24 9:21 Dling
 * bak for kernel 2.0
 * 
 * 1     99-11-08 14:21 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _SYS_CONTROL_H
#define _SYS_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************************************************************
 * 
 * system control function definition:
 *
 ***************************************************************************/

#define	SYS_SETCALLBACK		0x0001	/*  function 0001H: set call backfuncion: 
										param1:		call back function entry.
										param2:		a user defined data, used when call back func is called,
													usually a pointer to a structure. */

#define	SYS_GETCALLBACK		0x8001	/*	function 8001H:	get call back fucntion:
										param1:		a pointer to retrieve call back function
										param2:		a pointer to user data */

#define SYS_GETEVENTMASK	0x0002	/*  function */
#define SYS_SETEVENTMASK	0x8002	/*  function */

#define SYS_SETCACHEMODE	0x0003	/*	function 0002H:	set cache mode 
										param1:		cache mode
										param2:		not used */

#define SYS_GETCACHEMODE	0x8003	/*	function 8002H:	get cache mode 
										param1:		pointer to retrieve catch mode
										param2:		not used */

#define SYS_DUMPCACHE		0x0004	/*	function 0003H: dump cache
										param1:		work mode
										param2:		not used */

#define SYS_WRITECACHE		0x0005	/*	function 0004H: write cache blocks
										param1:
										param2:	*/

#define SYS_CACHESTATUS		0x0006	/*	function 0005H: cache status
										param1:
										param2:	*/

#define SYS_GETDEVATTRIB	0x0007	/*	funtion 0001H: get device attribute 
										param1:	pointer to a DEVICEHEADER structure
										param2:	buffer size
									*/

/* Call back funtion type */
typedef UINT (*_CALLBACK)(LONG lUserData, DWORD dwEventMask, DWORD param1, LONG param2);

/* callback data */
typedef struct CALLBACKDATA {
	_CALLBACK	callback;
	LONG		callback_para;
	DWORD		event_mask;
}CALLBACKDATA;


/* device attribute header */
typedef struct tagDEVICEHEADER {
	DWORD	size;
	char	name[8];		/*	device name */
	char	oem[8];			/*	original equipment manufacturer */
	DWORD	type;			/*	device type */
	DWORD	attrib;			/*	device attribute */
	WORD	status;			/*	bit0:	opened
								bit1:	½Úµç×´Ì¬	*/
	WORD	SysRes;			/*	bit0 ~ bit1:	IRQs ( max = 2 );
								bit2 ~ bit3:	DMAs ( max = 2 );
								bit4 ~ bit6:	AddrSegs( max = 4 ); 
								bit7 ~ bit15:	unused or private	*/
	BYTE	IRQ[2];			/*	used IRQ  */
	BYTE	DMA[2];			/*	used DMA  */
	WORD	AddrSeg[4][2];	/*	used address segments */
}DEVICEHEADER;

/* char device attribute: */
typedef struct tagCHARDEVICE{
	DEVICEHEADER	hdr;
	PVOID	pStatisticsTbl;
	DWORD	MaxSpeed;
	WORD	MaxReadCount;
	WORD	MaxWriteCount;
}CHARDEVICE;

/* block device attribute: */
typedef struct tagBLOCKDEVICE{
	DEVICEHEADER	hdr;
	PVOID	pStatisticsTbl;
	DWORD	blocks;
	WORD	BlockSize;
	WORD	MaxSpeed;
	WORD	SeekSpeed;
	WORD	WriteBackDelay;
	WORD	cylinders;
	BYTE	heads;
	BYTE	sectors;
}BLOCKDEVICE;

#ifdef _EMU_DEV
int _EMU_Callback(_CALLBACK lpfnCallback, LONG lUserData, DWORD dwEventMask, 
				  DWORD param1, LONG param2);
#endif

#ifdef __cplusplus
} /* extern C */
#endif

#endif // _SYS_CONTROL_H

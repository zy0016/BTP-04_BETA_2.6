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
 * $Archive:: /hopen2.0/include/Devman.h							$
 * $Workfile::	Devman.h											$
 * $Revision:: 4  $     $Date:: 03-04-26 3:28p						$
 *
\**************************************************************************/

/***************************************************************************
 *
 * Hopen Device Driver Interface
 *
 * $Revision: 4 $     $Date: 00-02-22 9:05 $    
 *
 * $Log: /Hopen/include/ddi/Devman.h $
 * 
 * 4     00-02-22 9:05 Dling
 * modify declaration of RegisterDevice and UnregisterDevice
 * 
 * 3     00-01-27 10:38 Dling
 * bak for adding fat16
 * 
 * 2     99-11-25 15:33 Dling
 * update by ling di
 * 
 * 1     99-11-08 14:20 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _HOPEN_DEVMAN_H
#define _HOPEN_DEVMAN_H

#include "stype.h"

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************
 *
 * value of dwReason when calling DriverEntryPoint
 *
 **************************************************************************************/

#define LOAD_DRIVER		1
#define FREE_DRIVER		2

/**************************************************************************************
 *
 *  device entry:
 *
 **************************************************************************************/

typedef int (*DEVENTRY)(const char* pDevName, void** ppvObj);

/**************************************************************************************
 *
 *  device properties:
 *
 **************************************************************************************/

#define DEVNAME_LEN		16

typedef struct DEVPROP{
	DWORD		size;
	char		name[DEVNAME_LEN];
	DWORD		type;
	DWORD		attrib;
	DWORD		hModule;
	DEVENTRY	DevEntry;
}DEVPROP, *PDEVPROP;

/* device type:	*/
#define DEV_USB			1
#define DEV_PCMCIABUS	2
#define DEV_SERIAL		3
#define DEV_PARALLEL	4
#define DEV_KEYBOARD	5
#define DEV_MOUSE		6
#define DEV_TOUCHSCREEN	7
#define DEV_DISPLAY		8
#define DEV_HARDDISK	9
#define DEV_FLOPPY		10
#define DEV_CDROM		11
#define DEV_ZIP			12
#define DEV_INFRARED	13

/* divice attribute: */
#define DA_BUS				0x00000800
#define DA_MEDIACHANGABLE	0x00001000
#define DA_SLOWIO			0x00002000
#define DA_VIRTUAL			0x00004000
#define DA_BLOCKIO			0x00008000
#define DA_READONLY			0x00010000
#define DA_REMOTE			0x00020000
#define DA_USECACHE			0x40000000
#define DA_DYNAMIC			0x80000000

/* initialization of device manager, must be called before using device man */
int InitDeviceManager(void);
int ExitDeviceManager(void);

/* register/unregister device */
#ifdef _EMU_DEV

#define RegisterDevice		_RegisterDevice
#define UnregisterDevice	_UnregisterDevice

#endif // _EMU_DEV

#ifdef _MSC_VER
_declspec(dllexport) 
#endif
int RegisterDevice(const char* pDevName, DWORD type, DWORD attrib, 
				   DWORD hModule, DEVENTRY entry);
#ifdef _MSC_VER
_declspec(dllexport) 
#endif
int UnregisterDevice(const char* pDevName);

/* get/set device entry */
int GetDeviceEntry(const char* pDevName, DEVENTRY* pEntry);
int SetDeviceEntry(const char* pDevName, DEVENTRY entry);

/* find device */
int FindDeviceByName(const char* pDevName, void** ppvDevProp);
int FindDeviceByType(DWORD type, DWORD attr_mask, int index, void** ppvDevProp);

/* get device interface */
int	GetDeviceInterface(const char* pDevName, void** ppvObj);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif // _HOPEN_DEVMAN_H

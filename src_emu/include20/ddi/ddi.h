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
 * $Archive:: /hopen2.0/include/Ddi.h				                $
 * $Workfile:: Ddil.h                                               $
 * $Revision:: 4  $     $Date:: 03-04-26 3:28p                      $
 *
\**************************************************************************/

/***************************************************************************
 *
 * Hopen Device Driver Interface
 *
 * $Revision: 2 $     $Date: 99-12-09 10:24 $    
 *
 * $Log: /Hopen/include/ddi/Ddi.h $
 * 
 * 2     99-12-09 10:24 Dling
 * modify IDevice_Write
 * 
 * 1     99-11-08 14:20 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _HOPEN_DDI_H
#define _HOPEN_DDI_H

#include "stype.h"
#include "ioctl.h"
#include "sysctl.h"

#ifdef __cplusplus
extern "C"{
#endif

/**************************************************************************************
 *
 *  interface definition
 *
 **************************************************************************************/

typedef _INTERFACE IDevice IDevice, *PDEVICE;

#define DDIARGLIST_Open	
#define DDIARGLIST_Close
#define DDIARGLIST_Read			PVOID pBuf, DWORD nBytes
#define DDIARGLIST_Write		PVOID pBuf, DWORD nBytes
#define DDIARGLIST_ReadBlock	DWORD lBlockNo, PVOID pBuf, DWORD nBlocks
#define DDIARGLIST_WriteBlock	DWORD lBlockNo, PVOID pBuf, DWORD nBlocks
#define DDIARGLIST_IoControl	UINT iCtlCode, PVOID pBuf, DWORD size, DWORD dwEventId
#define DDIARGLIST_SysControl	UINT iCtlCode, DWORD param1, LONG param2

#if defined(__cplusplus) && !defined(DEV_CINTERFACE)	/* C++ style interface */

//#define _STDMETHOD(method)	virtual UINT _STD_CALL method
#define THIS_DEV			void
#define THIS_DEV_
#define PURE				= 0

_INTERFACE IDevice {
public:
	_STDMETHOD(Release) (THIS_DEV) PURE;
	_STDMETHOD(Open) (THIS_DEV) PURE;
	_STDMETHOD(Close) (THIS_DEV) PURE;
	_STDMETHOD(Read) (THIS_DEV_ DDIARGLIST_Read) PURE;
	_STDMETHOD(Write) (THIS_DEV_ DDIARGLIST_Write) PURE;
	_STDMETHOD(ReadBlock) (THIS_DEV_ DDIARGLIST_ReadBlock) PURE;
	_STDMETHOD(WriteBlock) (THIS_DEV_ DDIARGLIST_WriteBlock) PURE;
	_STDMETHOD(IoControl) (THIS_DEV_ DDIARGLIST_IoControl) PURE;
	_STDMETHOD(SysControl) (THIS_DEV_ DDIARGLIST_SysControl) PURE;
};

#else /* C style interface */

//#define _STDMETHOD(method)	UINT (_STD_CALL *method)
#define THIS_DEV			IDevice * This
#define THIS_DEV_			IDevice * This,
#define PURE

typedef struct IDeviceVtbl{
	_STDMETHOD(Release) (THIS_DEV) PURE;
	_STDMETHOD(Open) (THIS_DEV) PURE;
	_STDMETHOD(Close) (THIS_DEV) PURE;
	_STDMETHOD(Read) (THIS_DEV_ DDIARGLIST_Read) PURE;
	_STDMETHOD(Write) (THIS_DEV_ DDIARGLIST_Write) PURE;
	_STDMETHOD(ReadBlock) (THIS_DEV_ DDIARGLIST_ReadBlock) PURE;
	_STDMETHOD(WriteBlock) (THIS_DEV_ DDIARGLIST_WriteBlock) PURE;
	_STDMETHOD(IoControl) (THIS_DEV_ DDIARGLIST_IoControl) PURE;
	_STDMETHOD(SysControl) (THIS_DEV_ DDIARGLIST_SysControl) PURE;
}IDeviceVtbl;

_INTERFACE IDevice
{
	const IDeviceVtbl *lpVtbl;
};

/* define macro to call interface */
#define IDevice_Release(This) \
				(This)->lpVtbl->Release(This)

#define IDevice_Open(This)	\
				(This)->lpVtbl->Open(This)

#define IDevice_Close(This)	(This)->lpVtbl->Close(This)

#define IDevice_Read(This,pBuf,nBytes)	\
				(This)->lpVtbl->Read(This,pBuf,nBytes)

#define IDevice_Write(This,pBuf,nBytes)	\
				(This)->lpVtbl->Write(This,pBuf,nBytes)

#define IDevice_ReadBlock(This,lBlockNo,pBuf,nBlocks)	\
				(This)->lpVtbl->ReadBlock(This,lBlockNo,pBuf,nBlocks)

#define IDevice_WriteBlock(This,lBlockNo,pBuf,nBlocks)	\
				(This)->lpVtbl->WriteBlock(This,lBlockNo,pBuf,nBlocks)

#define IDevice_IoControl(This,iCtlCode,pBuf,size,dwEventId)	\
				(This)->lpVtbl->IoControl(This,iCtlCode,pBuf,size,dwEventId)

#define IDevice_SysControl(This,iCtlCode,param1,param2)	\
				(This)->lpVtbl->SysControl(This,iCtlCode,param1,param2)

#endif /* C style interface */


/**************************************************************************************
 *
 *  macro used in implementation
 *
 **************************************************************************************/

#if defined(__cplusplus) && !defined(DEV_CINTERFACE)	/* C++ style interface */

#define DECLARE_DDI_IMPL(dev_class) \
	_STDMETHOD(Release) (THIS_DEV) ;\
	_STDMETHOD(Open) (THIS_DEV) ;\
	_STDMETHOD(Close) (THIS_DEV) ;\
	_STDMETHOD(Read) (THIS_DEV_ DDIARGLIST_Read) ;\
	_STDMETHOD(Write) (THIS_DEV_ DDIARGLIST_Write) ;\
	_STDMETHOD(ReadBlock) (THIS_DEV_ DDIARGLIST_ReadBlock) ;\
	_STDMETHOD(WriteBlock) (THIS_DEV_ DDIARGLIST_WriteBlock) ;\
	_STDMETHOD(IoControl) (THIS_DEV_ DDIARGLIST_IoControl) ;\
	_STDMETHOD(SysControl) (THIS_DEV_ DDIARGLIST_SysControl) ;


#define DDIMETHOD_DEFIMPLV(dev_class, method)\
	UINT dev_class::method() { return 0; }

#define DDIMETHOD_DEFIMPLP(dev_class, method)\
	UINT dev_class::method(DDIARGLIST##_##method) { return 0; }

#else /* ANSI C style */	

/* define local ddi method type */

#define LOCAL_DDIMETHOD(method)	static UINT _STD_CALL method

#define BEGIN_DDI_IMPL(dev_class)	\
	typedef struct dev_class dev_class;	\
	LOCAL_DDIMETHOD(Release) (dev_class* This);	\
	LOCAL_DDIMETHOD(Open) (dev_class* This);	\
	LOCAL_DDIMETHOD(Close) (dev_class* This);	\
	LOCAL_DDIMETHOD(Read) (dev_class* This, DDIARGLIST_Read);	\
	LOCAL_DDIMETHOD(Write) (dev_class* This, DDIARGLIST_Write);	\
	LOCAL_DDIMETHOD(ReadBlock) (dev_class* This, DDIARGLIST_ReadBlock);\
	LOCAL_DDIMETHOD(WriteBlock) (dev_class* This, DDIARGLIST_WriteBlock);\
	LOCAL_DDIMETHOD(IoControl) (dev_class* This, DDIARGLIST_IoControl);\
	LOCAL_DDIMETHOD(SysControl) (dev_class* This, DDIARGLIST_SysControl);\
	const static IDeviceVtbl theVtbl = {\
		(UINT (_STD_CALL *)(THIS_DEV))Release,\
		(UINT (_STD_CALL *)(THIS_DEV))Open,	\
		(UINT (_STD_CALL *)(THIS_DEV))Close,\
		(UINT (_STD_CALL *)(THIS_DEV_ DDIARGLIST_Read))Read,\
		(UINT (_STD_CALL *)(THIS_DEV_ DDIARGLIST_Write))Write,\
		(UINT (_STD_CALL *)(THIS_DEV_ DDIARGLIST_ReadBlock))ReadBlock,\
		(UINT (_STD_CALL *)(THIS_DEV_ DDIARGLIST_WriteBlock))WriteBlock,\
		(UINT (_STD_CALL *)(THIS_DEV_ DDIARGLIST_IoControl))IoControl,\
		(UINT (_STD_CALL *)(THIS_DEV_ DDIARGLIST_SysControl))SysControl \
	};	struct dev_class{	\
		const struct IDeviceVtbl * lpVtbl;

#define END_DDI_IMPL(dev_class)	\
	};

#define DDIMETHOD_DEFIMPLV(dev_class, method)\
	LOCAL_DDIMETHOD(method)(dev_class* This) { return 0; }

#define DDIMETHOD_DEFIMPLP(dev_class, method)\
	LOCAL_DDIMETHOD(method)(dev_class* This, DDIARGLIST##_##method) {\
		return 0; }

#endif /* C style interface */

/***************************************************************************
 * 
 * return value by device operations:
 *
 ***************************************************************************/

#define DI_IOPENDING	-1

 /***************************************************************************
 * 
 * device status flags
 *
 ***************************************************************************/

#define DF_OPEN			0x80000000
#define DF_LOCK			0x40000000


#ifdef __cplusplus
} /* extern c */
#endif

#endif /* _HOPEN_DDI_H */

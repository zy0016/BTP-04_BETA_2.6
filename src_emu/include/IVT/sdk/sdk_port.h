/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:	sdk_port.h 
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/


#ifndef _SDK_PORT_H
#define _SDK_PORT_H

/* the mask for setting Btsdk_StoreLocalDevice() */
#define BTSDK_STORE_LOC_DEV_NAME			0x0001
#define BTSDK_STORE_LOC_DISCOVERY_MODE		0x0002
#define BTSDK_STORE_LOC_FIXED_PIN_CODE		0x0004
#define BTSDK_STORE_LOC_DEV_CLASS			0x0008
#define BTSDK_STORE_LOC_SEC_MODE			0x0010
#define BTSDK_STORE_LOC_DEV_ALL				0x0FFF

/* the mask for setting Btsdk_StoreRemoteDeviceInfo() */
#define BTSDK_STORE_RMT_DEV_NAME			0x0001
#define BTSDK_STORE_RMT_DEV_CLASS			0x0002
#define BTSDK_STORE_RMT_LINK_KEY			0x0004
#define BTSDK_STORE_RMT_DEV_ALL				0x0FFF

/* the mask for setting Btsdk_StoreLocalService() */
#define BTSDK_STORE_LOC_SVC_NAME			0x0001
#define BTSDK_STORE_LOC_SVC_SECLEVEL		0x0002
#define BTSDK_STORE_LOC_SVC_AUTHORMETHOD	0x0004
#define BTSDK_STORE_LOC_SVC_EXTATTR			0x0008
#define BTSDK_STORE_LOC_SVC_CLASS			0x0020
#define BTSDK_STORE_LOC_SVC_STATUS			0x0040
#define BTSDK_STORE_LOC_SVC_ALL				0x00FF

/* the mask for setting Btsdk_StoreShortCutInfo() */
#define BTSDK_STORE_SHC_NAME				0x0001
#define BTSDK_STORE_SHC_BDADDR				0x0002
#define BTSDK_STORE_SHC_DEVCLASS			0x0004
#define BTSDK_STORE_SHC_SVCCLASS			0x0008
#define BTSDK_STORE_SHC_IS_DEFAULT			0x0010
#define BTSDK_STORE_SHC_SECLEVEL			0x0020
#define BTSDK_STORE_SHC_SVCNAME				0x0040
#define BTSDK_STORE_SHC_DEVNAME				0x0080
#define BTSDK_STORE_SHC_ATTRIB				0x0100
#define BTSDK_STORE_SHC_ATTRIB_LEN			0x0200
#define BTSDK_STORE_SHC_ALL					0x0FFF


#ifdef CONFIG_BTSDK_STORE_INFO

BTINT32 Btsdk_StoreLocalDevice(LocalDeviceStru *local, BTUINT16 mask);
BTINT32 Btsdk_RecoverLocalDevice(LocalDeviceStru *local);
BTINT32 Btsdk_StoreRemoteDeviceInfo(RemoteDeviceStru *dev, BTUINT16 mask);
BTINT32 Btsdk_RecoverRemoteDeviceInfo(RemoteDeviceStru *dev);
BTINT32 Btsdk_DeleteRemoteDeviceInfo(RemoteDeviceStru *dev);
BTINT32 Btsdk_StoreAllRemoteDeviceInfo(struct BtList *dev_list);
BTINT32 Btsdk_RecoverAllRemoteDeviceInfo(struct BtList *dev_list);
BTINT32 Btsdk_StoreLocalServiceAttr(PLocalServiceStru loc_svc, BTUINT16 mask);
BTINT32 Btsdk_RecoverLocalService(LocalServiceStru *svc);
BTINT32 Btsdk_RecoverAllLocalService();
BTINT32 Btsdk_DeleteLocalService(BTSVCHDL svc_hdl);
BTINT32 Btsdk_StoreLocSerTrustedDevList(BTSVCHDL svc_hdl);
BTINT32 Btsdk_StoreLocSerOneTrustedDev(BTSVCHDL svc_hdl, BTUINT8* addr, BTBOOL bIsStore);
BTINT32 Btsdk_RecoverLocSerTrustedDev(struct BtList* dev_list, BTUINT8 *psSectionName);
BTINT32 Btsdk_RenameShortcutSection(BTUINT8 *old_sec, BTUINT8 *new_sec);
BTINT32 Btsdk_StoreShortCutInfo(ShortCutManagerStru *psch, BTUINT16 mask);
BTINT32 Btsdk_RecoverShortCutInfo(ShortCutManagerStru *psch);
BTINT32 Btsdk_DeleteShortCutInfo(ShortCutManagerStru *psch);
BTINT32 Btsdk_DeleteAllShortCutInfo(void);
BTINT32 Btsdk_StoreAllShortCutInfo(struct BtList *psch_list);
BTINT32 Btsdk_RecoverAllShortCutInfo(struct BtList *psch_list);

#else

#define Btsdk_StoreLocalDevice(local, mask)							BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_RecoverLocalDevice(local)								BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_StoreRemoteDeviceInfo(dev, mask)						BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_RecoverRemoteDeviceInfo(dev)							BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_DeleteRemoteDeviceInfo(dev)							BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_StoreAllRemoteDeviceInfo(dev_list)					BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_RecoverAllRemoteDeviceInfo(dev_list)					BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_StoreLocalServiceAttr(loc_svc, mask)					BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_RecoverLocalService(svc)								BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_RecoverAllLocalService()								BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_DeleteLocalService(svc_hdl)							BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_StoreLocSerTrustedDevList(svc_hdl)					BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_StoreLocSerOneTrustedDev(svc_hdl, addr, bIsStore)		BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_RecoverLocSerTrustedDev(dev_list, psSectionName)		BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_RenameShortcutSection(old_sec, new_sec)				BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_StoreShortCutInfo(psch, mask)							BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_RecoverShortCutInfo(psch)								BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_DeleteShortCutInfo(psch)								BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_DeleteAllShortCutInfo()								BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_StoreAllShortCutInfo(psch_list)						BTSDK_ER_FUNCTION_NOTSUPPORT
#define Btsdk_RecoverAllShortCutInfo(psch_list)						BTSDK_ER_FUNCTION_NOTSUPPORT

#endif

#endif

/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_rmt.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _SDK_RMT_H
#define _SDK_RMT_H

#define	DEVICE_CLASS_MASK		0x1FFC
#define IS_SAME_TYPE_DEVICE_CLASS(a, b)	(((a) & DEVICE_CLASS_MASK) == ((b) & DEVICE_CLASS_MASK))

/* Default role of local device when creating a new ACL connection. */
#define BTSDK_MASTER_ROLE						0x00
#define BTSDK_SLAVE_ROLE						0x01

/* Possible power mode of an ACL link */
#define BTSDK_LPM_ACTIVE_MODE	0
#define BTSDK_LPM_HOLD_MODE		1
#define BTSDK_LPM_SNIFF_MODE	2
#define BTSDK_LPM_PARK_MODE		3


typedef struct  _BtSdkRemoteLMPInfoStru
{
	BTUINT8 lmp_feature[8];				/* LMP features */
	BTUINT16 manuf_name; 				/* the name of the manufacturer */
	BTUINT16 lmp_subversion;			/* the sub version of the LMP firmware */
	BTUINT8 lmp_version; 				/* the main version of the LMP firmware */
}BtSdkRemoteLMPInfoStru, *PBtSdkRemoteLMPInfoStru;

typedef struct  _BtSdkRemoteDevicePropertyStru
{
	BTDEVHDL dev_hdl;							/*Handle assigned to the device record*/
	BTUINT8 bd_addr[BTSDK_BDADDR_LEN];			/*BT address of the device record*/
	BTUINT8 name[BTSDK_DEVNAME_LEN];				/*Name of the device record, must be in UTF-8*/
	BTUINT32 dev_class;							/*Device class*/
	BtSdkRemoteLMPInfoStru lmp_info;			/* LMP info */
}BtSdkRemoteDevicePropertyStru;  
typedef BtSdkRemoteDevicePropertyStru * PBtSdkRemoteDevicePropertyStru;

typedef struct _BtSdkHoldModeStru {
	BTUINT16 conn_hdl;					/* reserved, set it to 0. */
	BTUINT16 max;						/* Hold mode max interval. */
	BTUINT16 min;						/* Hold mode min interval. */
} BtSdkHoldModeStru;
typedef BtSdkHoldModeStru * PBtSdkHoldModeStru;

typedef struct _BtSdkSniffModeStru {
	BTUINT16 conn_hdl;					/* reserved, set it to 0. */
	BTUINT16 max;						/* Sniff mode max interval. */
	BTUINT16 min;						/* Sniff mode min interval. */
	BTUINT16 attempt;					/* Sniff mode attempt value. */
	BTUINT16 timeout;					/* Sniff mode timeout value. */
} BtSdkSniffModeStru;
typedef BtSdkSniffModeStru * PBtSdkSniffModeStru;

typedef struct _BtSdkParkModeStru {
	BTUINT16 conn_hdl;					/* reserved, set it to 0. */
	BTUINT16 max;						/* Beacon max interval. */
	BTUINT16 min;						/* Beacon min interval. */
} BtSdkParkModeStru;
typedef BtSdkParkModeStru * PBtSdkParkModeStru;


/*************** Function Prototype ******************/
/* Remote Device Manager */
FUNC_EXPORT BTINT32 Btsdk_GetRemoteDeviceBDAddr (BTDEVHDL dev_hdl, BTUINT8 *bd_addr);
FUNC_EXPORT BTDEVHDL Btsdk_GetDeviceHandle(BTUINT8 *bd_addr);
FUNC_EXPORT BTINT32 Btsdk_DeleteRemoteDeviceByHandle(BTDEVHDL dev_hdl);
FUNC_EXPORT BTINT32 Btsdk_DeleleRemoteDeviceByDeviceClass(BTUINT32 device_class);
FUNC_EXPORT BTUINT32 Btsdk_GetKnownDevicesByClass(BTUINT32 dev_class, BTDEVHDL *pdev_hdl, BTUINT32 max_dev_num);
FUNC_EXPORT BTINT32 Btsdk_GetPairedRemoteDevice(BTDEVHDL *pdev_hdl, BTUINT32 max_dev_num);
FUNC_EXPORT BTINT32 Btsdk_GetInquiredRemoteDevice(BTDEVHDL *pdev_hdl, BTUINT32 max_dev_num);
FUNC_EXPORT BTINT32 Btsdk_StartDiscoverDevice(BTUINT32 device_class, BTUINT16 max_num, BTUINT16 max_seconds);
FUNC_EXPORT BTINT32 Btsdk_StopDiscoverDevice(void);
FUNC_EXPORT BTINT32 Btsdk_StartPeriodicDiscoverDevice(BTUINT16 max_seconds);
FUNC_EXPORT BTINT32 Btsdk_StopPeriodicDiscoverDevice(void);
FUNC_EXPORT BTINT32 Btsdk_PairDevice(BTDEVHDL dev_hdl);
FUNC_EXPORT BTINT32 Btsdk_UnPairDevice(BTDEVHDL dev_hdl);
FUNC_EXPORT BTINT32 Btsdk_IsDevicePaired(BTDEVHDL dev_hdl, BTBOOL *pis_paired);
FUNC_EXPORT BTINT32 Btsdk_SetRemoteDevicePinCode(BTDEVHDL dev_hdl, BTUINT8* pin_code, BTUINT16 size);
FUNC_EXPORT BTINT32 Btsdk_SetRemoteDeviceLinkKey(BTDEVHDL dev_hdl, BTUINT8* link_key);
FUNC_EXPORT BTINT32 Btsdk_GetRemoteDeviceName(BTDEVHDL dev_hdl, BTUINT8 *name, BTUINT16 *plen);
FUNC_EXPORT BTINT32 Btsdk_UpdateRemoteDeviceName(BTDEVHDL dev_hdl, BTUINT8 *name, BTUINT16 *plen);

FUNC_EXPORT BTINT32 Btsdk_GetRemoteDeviceRole(BTDEVHDL dev_hdl, BTUINT16 *prole);
FUNC_EXPORT BTINT32 Btsdk_GetRemoteDeviceClass(BTDEVHDL dev_hdl, BTUINT32 *pdevice_class);
FUNC_EXPORT BTINT32 Btsdk_UpdateRemoteDeviceClass(BTDEVHDL dev_hdl, BTUINT32 *pdevice_class);
FUNC_EXPORT BTINT32 Btsdk_GetRemoteLMPInfo(BTDEVHDL dev_hdl, BtSdkRemoteLMPInfoStru *info);
FUNC_EXPORT BTINT32 Btsdk_GetRemoteRSSI(BTDEVHDL dev_hdl, BTUINT16 *prssi);
FUNC_EXPORT BTINT32 Btsdk_GetRemoteLinkQuality(BTDEVHDL dev_hdl, BTUINT16 *plink_quality);
FUNC_EXPORT BTINT32 Btsdk_GetSupervisonTimeout(BTDEVHDL dev_hdl, BTUINT16 *ptimeout);
FUNC_EXPORT BTINT32 Btsdk_SetSupervisonTimeout(BTDEVHDL dev_hdl, BTUINT16 timeout);
FUNC_EXPORT BTINT32 Btsdk_SetRemoteDeviceParam(BTDEVHDL dev_hdl, BTUINT32 app_param);
FUNC_EXPORT BTINT32 Btsdk_GetRemoteDeviceParam(BTDEVHDL dev_hdl, BTUINT32 *papp_param);
FUNC_EXPORT BTINT32 Btsdk_GetRemoteDeviceProperty(BTDEVHDL dev_hdl, PBtSdkRemoteDevicePropertyStru rmt_dev_prop);
FUNC_EXPORT BTINT32 Btsdk_RejAclConn(BTDEVHDL dev_hdl);
FUNC_EXPORT BTBOOL Btsdk_IsDeviceConnected(BTDEVHDL dev_hdl);

FUNC_EXPORT BTINT32 Btsdk_GetCurrentLinkMode(BTDEVHDL dev_hdl, BTUINT8* link_mode);
FUNC_EXPORT BTINT32 Btsdk_SetLinkMode(BTDEVHDL dev_hdl, BTUINT8 link_mode, BTUINT8* param);
/* BTINT32 Btsdk_ActivateACLLink(BTDEVHDL dev_hdl) */
#define Btsdk_ActivateACLLink(dev_hdl)		 Btsdk_SetLinkMode(dev_hdl, BTSDK_LPM_ACTIVE_MDOE, NULL)	
/* BTINT32 Btsdk_EnterHoldMode(BTDEVHDL dev_hdl, PBtSdkHoldModeStru param) */
#define Btsdk_EnterHoldMode(dev_hdl, param)	 Btsdk_SetLinkMode(dev_hdl, BTSDK_LPM_HOLD_MODE, (BTUINT8*)param)
/* BTINT32 Btsdk_EnterSniffMode(BTDEVHDL dev_hdl, PBtSdkSniffModeStru param) */
#define Btsdk_EnterSniffMode(dev_hdl, param) Btsdk_SetLinkMode(dev_hdl, BTSDK_LPM_SNIFF_MODE, (BTUINT8*)param)
/* BTINT32 Btsdk_EnterSniffMode(BTDEVHDL dev_hdl, PBtSdkParkModeStru param) */
#define Btsdk_EnterParkMode(dev_hdl, param)	 Btsdk_SetLinkMode(dev_hdl, BTSDK_LPM_PARK_MODE, (BTUINT8*)param)

FUNC_EXPORT BTINT32 Btsdk_RecoverRemoteDeviceLinkKey(BTDEVHDL dev_hdl, BTUINT8* link_key);

FUNC_EXPORT BTINT32 Btsdk_RemoteDeviceFlowStatistic(BTDEVHDL dev_hdl, BTUINT32* rx_bytes, BTUINT32* tx_bytes);


#endif

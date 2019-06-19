/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_rmt.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _SDK_SHC_H
#define _SDK_SHC_H


typedef struct  _BtSdkShortCutPropertyStru
{
	BTSHCHDL shc_hdl;						/* handle assigned to the shortcut instance*/
	BTCONNHDL conn_hdl;						/* handle assigned to the connection instance*/
	BTCONNHDL dev_hdl;						/* handle assigned to the device instance */
	BTSVCHDL svc_hdl;						/* handle assigned to the service instance */
	BTBOOL	by_dev_hdl;						/* BTSDK_TRUE: Specify device by dev_hdl. 
											   Otherwise by bd_addr. */
	BTBOOL	by_svc_hdl;						/* BTSDK_TRUE: Specify service type by svc_hdl. 
											   Otherwise by svc_class. */
	BTUINT8 bd_addr[BTSDK_BDADDR_LEN];		/* bt address of local device */
	BTUINT16 svc_class;						/* service class */
	BTUINT16 mask;							/* Specified which member is to be set or get. */ 
	BTUINT8 shc_name[BTSDK_SHORTCUT_NAME_LEN];	/* name of the shortcut, must in UTF-8 */
	BTUINT8 dev_name[BTSDK_DEVNAME_LEN];	/* Name of the device record, must be in UTF-8 */
	BTUINT8 svc_name[BTSDK_SERVICENAME_MAXLENGTH];	/*must in UTF-8*/
	BTUINT32 dev_class;						/* device class of the remote device*/
	BTBOOL is_default;						/* is default shortcut */
	BTUINT8 sec_level;						/* Security level of this shortcut. Authentication/Encryption. */
	BTUINT16 shc_attrib_len;				/* the length of shortcut attribute */
	BTUINT8 *pshc_attrib;					/* shortcut attribute */
}BtSdkShortCutPropertyStru;

typedef BtSdkShortCutPropertyStru * PBtSdkShortCutPropertyStru;

/* Possible flags for member 'mask' in _BtSdkShortCutPropertyStru */
#define BTSDK_SCPM_SHCNAME					0x0001
#define BTSDK_SCPM_DEVNAME					0x0002
#define BTSDK_SCPM_SVCNAME					0x0004
#define BTSDK_SCPM_DEVCLASS					0x0008
#define BTSDK_SCPM_ISDEFAULT				0x0010
#define BTSDK_SCPM_SECLEVEL					0x0020
#define BTSDK_SCPM_SHCATTR					0x0040
#define BTSDK_SCPM_ALL						0x0FFF


/*************** Function Prototype ******************/
FUNC_EXPORT BTINT32 Btsdk_CreateShortCut(BTUINT8 *shc_name, BTCONNHDL conn_hdl, BTSHCHDL *pshc_hdl);
FUNC_EXPORT BTINT32 Btsdk_CreateShortCutEx(PBtSdkShortCutPropertyStru shc_prop);
FUNC_EXPORT BTINT32 Btsdk_StartShortCutEx(BTSHCHDL shc_hdl, BTUINT32 lParam, BTCONNHDL *conn_hdl);
FUNC_EXPORT BTINT32 Btsdk_StartShortCut(BTSHCHDL shc_hdl);
FUNC_EXPORT BTINT32 Btsdk_StopShortCut(BTSHCHDL shc_hdl);
FUNC_EXPORT BTINT32 Btsdk_DeleteShortCut(BTSHCHDL shc_hdl);

FUNC_EXPORT BTINT32 Btsdk_GetShortCutProperty(PBtSdkShortCutPropertyStru pshc_prop);
FUNC_EXPORT BTINT32 Btsdk_SetShortCutProperty(PBtSdkShortCutPropertyStru pshc_prop);
FUNC_EXPORT BTUINT32 Btsdk_GetShortCutByDeviceHandle(BTDEVHDL dev_hdl, BTUINT16 service_class, 
										 BTSHCHDL *pshc_hdl, BTUINT32 max_shc_num);
FUNC_EXPORT BTUINT32 Btsdk_GetShortCutByServiceHandle(BTDEVHDL dev_hdl, BTSVCHDL svc_hdl, BTSHCHDL *pshc_hdl, 
										  BTUINT32 max_shc_num);
FUNC_EXPORT BTUINT32 Btsdk_GetShortCutByConnectionHandle(BTCONNHDL conn_hdl, BTSHCHDL *pshc_hdl, BTUINT32 max_shc_num);
FUNC_EXPORT BTSHCHDL Btsdk_GetShortCutByName(BTUINT8 *short_name);
FUNC_EXPORT BTSHCHDL Btsdk_GetDefaultShortCut(BTUINT32 service_class);
FUNC_EXPORT BTUINT32 Btsdk_GetShortCutByServiceClass(BTUINT32 service_class, BTSHCHDL *pshc_hdl, BTUINT32 max_shc_num);
FUNC_EXPORT BTUINT32 Btsdk_GetShortCutByDeviceClass(BTUINT32 device_class, BTSHCHDL *pshc_hdl, BTUINT32 max_shc_num);
FUNC_EXPORT BTUINT32 Btsdk_GetAllShortCut(BTSHCHDL *pshc_hdl, BTUINT32 max_shc_num);

#endif

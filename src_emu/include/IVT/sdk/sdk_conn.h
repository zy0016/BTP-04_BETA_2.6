/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_conn.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/



#ifndef _SDK_CONN_H
#define _SDK_CONN_H


/*************** Structure Definition ******************/
typedef struct _BtSdkConnectionPropertyStru
{
	BTUINT32 role : 2;
	BTUINT32 reserved : 30;
	BTDEVHDL device_handle;
	BTSVCHDL service_handle;
	BTUINT16 service_class;
	BTUINT32 duration;
	BTUINT32 received_bytes;
	BTUINT32 sent_bytes;
} BtSdkConnectionPropertyStru, *PBtSdkConnectionPropertyStru;

/* Possible roles for member 'role' in _BtSdkConnectionPropertyStru */
#define BTSDK_CONNROLE_CLIENT					0x2
#define BTSDK_CONNROLE_SERVER					0x1


/*
		lParam of Btsdk_StartClient, Btsdk_StartClientEx and Btsdk_CreateShortCutEx
*/

/*
	lParam for SPP, LAP, DUN is the index of the serial port to be opened to access
	the Bluetooth connection. If zero value is given, SDK will select an idle port
	automatically.
*/

/*
	lParam for OPP.
*/
typedef struct _BtSdkOPPClientParamStru
{
	BTUINT32 size;									/*Size of this structure, use for verification and versioning.*/
	BTUINT8 inbox_path[BTSDK_PATH_MAXLENGTH];		/*must in UTF-8*/
	BTUINT8 outbox_path[BTSDK_PATH_MAXLENGTH];		/*must in UTF-8*/
	BTUINT8 own_card[BTSDK_CARDNAME_MAXLENGTH]; 	/*must in UTF-8*/
} BtSdkOPPClientParamStru, *PBtSdkOPPClientParamStru;


/*************** Function Prototype ******************/
/* Connection Manager */
FUNC_EXPORT BTINT32 Btsdk_StartClient(BTSVCHDL svc_hdl, BTUINT32 lParam, BTCONNHDL *conn_hdl);
FUNC_EXPORT BTINT32 Btsdk_StartClientEx(BTDEVHDL dev_hdl, BTUINT16 service_class, BTUINT32 lParam, BTCONNHDL *conn_hdl);

FUNC_EXPORT BTINT32 Btsdk_DisconnectConnection(BTCONNHDL handle);
FUNC_EXPORT BTUINT32 Btsdk_GetAllIncomingConnections(BTCONNHDL *conn_hdl, BTUINT32 count);
FUNC_EXPORT BTUINT32 Btsdk_GetAllOutgoingConnections(BTCONNHDL *conn_hdl, BTUINT32 count);
FUNC_EXPORT BTINT32 Btsdk_GetConnectionProperty(BTCONNHDL conn_hdl, PBtSdkConnectionPropertyStru conn_prop);
FUNC_EXPORT BTINT16 Btsdk_GetClientPort(BTCONNHDL conn_hdl);
FUNC_EXPORT BTINT16 Btsdk_GetServerPort(BTSVCHDL svc_hdl);

#endif

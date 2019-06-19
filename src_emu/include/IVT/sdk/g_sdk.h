/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		g_sdk.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/



#ifndef _G_SDK_H
#define _G_SDK_H


#include "global.h"
#include "btinit.h"

/* THIS SHOULD BE MERGED INTO GLOBAL.H */
//#define PROT_SDK								50

#define BTSDK_DBG_PRINT_0(f)  DBG_PRINT( "(%s, %d) "##f, __FILE__, __LINE__);
#define BTSDK_DBG_PRINT_1(f, s1)  DBG_PRINT( "(%s, %d) "##f, __FILE__, __LINE__, s1);
#define BTSDK_DBG_PRINT_2(f, s1, s2)  DBG_PRINT( "(%s, %d) "##f, __FILE__, __LINE__, s1, s2);
#define BTSDK_DBG_PRINT_3(f, s1, s2, s3)  DBG_PRINT( "(%s, %d) "##f, __FILE__, __LINE__, s1, s2, s3);
#define BTSDK_DBG_PRINT_4(f, s1, s2, s3, s4)  DBG_PRINT( "(%s, %d) "##f, __FILE__, __LINE__, s1, s2, s3, s4);
#define BTSDK_DBG_PRINT_5(f, s1, s2, s3, s4, s5)  DBG_PRINT( "(%s, %d) "##f, __FILE__, __LINE__, s1, s2, s3, s4, s5);


#define BTSDK_IS_NEW_MEM_OK(ptr) \
		{ \
			if ((ptr) == NULL) \
			{ \
				BTSDK_DBG_PRINT_0("BTSDK:  No memory allocated!\n"); \
				DBG_LEAVE(PROT_SDK); \
				return BTSDK_ER_NO_MEMORY; \
			} \
		}

#define BTSDK_IS_POINTER_OK(ptr) \
		{ \
			if ((ptr) == NULL) \
			{ \
				BTSDK_DBG_PRINT_0("BTSDK:  The parameter is a NULL pointer!\n"); \
				DBG_LEAVE(PROT_SDK); \
				return BTSDK_ER_NULL_POINTER; \
			} \
		}

#define BTSDK_IS_DEV_HDL_OK(rmt_ptr, dev_hdl) \
		{ \
			rmt_ptr = (PRemoteDeviceStru)NewListFind(g_rmt_dev_list, (BTUINT8 *)&dev_hdl, sizeof(BTDEVHDL)); \
			if (rmt_ptr == NULL) \
			{ \
				 LEAVE_CRITICAL(g_rmt_dev_list->lock); \
 				 BTSDK_DBG_PRINT_0("BTSDK:  Not a VALID Device Pointer!\n"); \
				 DBG_LEAVE(PROT_SDK); \
				 return BTSDK_ER_HANDLE_NOT_EXIST; \
			} \
		}

#define BTSDK_CHECK_RETURN_CODE(func) \
			{ \
				BTUINT32 result; \
				result = (func); \
				Btsdk_PrintErrorMessage(result, BTSDK_TRUE); \
			}

#define BTSDK_IS_SHC_HDL_OK(shc_ptr, shc_hdl) \
		{ \
			shc_ptr = (PShortCutManagerStru)NewListFind(g_shc_list, (BTUINT8 *)&shc_hdl, sizeof(BTSHCHDL)); \
			if (shc_ptr == NULL) \
			{ \
				 LEAVE_CRITICAL(g_shc_list->lock); \
 				 BTSDK_DBG_PRINT_0("BTSDK:  Not a VALID Shortcut Pointer!\n"); \
				 DBG_LEAVE(PROT_SDK); \
				 return BTSDK_ER_HANDLE_NOT_EXIST; \
			} \
		}

#define BTSDK_IS_SAME_SERVICE_CLASS(svc_class_a, svc_class_b) \
			((svc_class_a) == (svc_class_b)  \
			|| ((svc_class_a) == BTSDK_CLS_HEADSET_AG && (svc_class_b) == BTSDK_CLS_HEADSET) \
			|| ((svc_class_b) == BTSDK_CLS_HEADSET_AG && (svc_class_a) == BTSDK_CLS_HEADSET) \
			|| ((svc_class_a) == BTSDK_CLS_HANDSFREE_AG && (svc_class_b) == BTSDK_CLS_HANDSFREE) \
			|| ((svc_class_b) == BTSDK_CLS_HANDSFREE_AG && (svc_class_a) == BTSDK_CLS_HANDSFREE))

#define BTSDK_MAX_EVENT_NUM			(BTSDK_SHORTCUT_EVENT_IND + 1)


/* for HCI Error Indication */
#define BTSDK_ERR_IND_TIMEOUT				200
#define PROT_SDK_BASE						((PROT_SDK - 1)*MAX_DEFINE_PER_PROTOCOL)
#define BTSDK_ERR_IND_HDL					PROT_SDK_BASE + 1

/* the flag of adding to connection list for the local service*/
#define BTSDK_SERVER_CONN_FLAG					0x8000

/*  Status of Local Device */
#define ST_LOC_DEV_FIXED_PIN					0x01
#define ST_LOC_DEV_NAME_READY					0x02
#define ST_LOC_DEV_CLASS_READY					0x04
#define ST_LOC_DEV_INQUIRING					0x08

/* Status of Remote Device */
#define ST_RMT_DEV_PAIRED						0x01
#define ST_RMT_DEV_CONNECTED					0x02
#define ST_RMT_DEV_INQUIRED						0x04
#define ST_RMT_DEV_NAME_READY					0x08
#define ST_RMT_DEV_CLASS_READY					0x10
#define ST_RMT_DEV_TRUSTED						0x20	/* This device is allowed to access all local service. */

/* Status of Local Service */
#define ST_LOC_SVC_STARTED						0x01
#define ST_LOC_SVC_STOPPED						0x02
#define ST_LOC_SVC_CONNECTED					0x03
#define ST_LOC_SVC_PAUSED						0x04

/* Status of Connection */
#define ST_CONN_SERVER							0x01
#define ST_CONN_CLIENT							0x02

/* Status of Remote Service */
#define ST_RMT_SVC_IDLE							0x00
#define ST_RMT_SVC_CONNECTED					0x01

/* Status of Shortcut */
#define ST_SHC_CONNECTED					0x01

/*************** Internal Structure ******************/

typedef struct  _RemoteDeviceStru
{
	BTDEVHDL dev_hdl;							/*Handle assigned to the device record*/
	BTUINT8 bd_addr[BTSDK_BDADDR_LEN];			/*BT address of the device record*/
	BTUINT8 name[BTSDK_DEVNAME_LEN];				/*Name of the device record, must be in UTF-8*/
	BTUINT32 dev_class;							/*Device class*/
	BTUINT8 link_key[BTSDK_LINK_KEY_LENGTH];	/*Link key of the device record if it is paired*/
	struct BtList * svc_list;					/*List of service records available on the device*/
	BTUINT16 status;							/*Paired, Connected, Inquired*/	
	BTUINT16 ref_count;							/*Refrence to this item*/
	BTUINT32 app_param;							/*User defined parameters*/
}RemoteDeviceStru;  
typedef RemoteDeviceStru * PRemoteDeviceStru;

typedef struct  _LocalDeviceStru
{
	BTUINT8 bd_addr[BTSDK_BDADDR_LEN];			/*BT address of local device*/
	BTUINT32 dev_class;							/*Device class of local device*/
	BTUINT8 name[BTSDK_DEVNAME_LEN];				/*Name of local device, must in UTF-8 */
	BTUINT16 pin_code_len;						/*Length of the fixed PIN if sets*/
	BTUINT8 pin_code[BTSDK_PIN_CODE_LEN];		/*Fixed PIN if sets*/	
	BTUINT16 sec_mode;							/* security mode */
	BTUINT16 discovery_mode;					/* discovery mode */
	struct BtList *svc_list;					/*Local service records list*/
	BTUINT32 status;							/*visible mode, security level, fixed PIN or not, Inquiry*/
	BTUINT32 app_param;							/*User defined parameters*/
	SYSTEM_LOCK	lock;							/* lock for  the structure*/
}LocalDeviceStru; 
typedef LocalDeviceStru * PLocalDeviceStru;

typedef struct  _LocalServiceStru
{
	BTSVCHDL svc_hdl;					/*Handle assigned to the service record*/
	HDLTYPE   real_hdl;					/*This value is returned from XXX_SvrStart, so it is meaningful 
   only when this service record is started*/
	BTUINT16 svc_class;					/*Service type*/
	BTUINT8 svc_name[BTSDK_SERVICENAME_MAXLENGTH];	/*must in UTF-8*/
	BTLPVOID ext_attrib;				/*Profile specific attributes*/
	struct BtList *trusted_list;				/*List of trusted devices for this service record*/
	BTUINT16 channel;					/*Server channel to check security*/
	BTUINT16 prot_id;						/*Protocol type to check security*/
	BTUINT16 sec_level;					/*Authorization, Authentication, Encryption, None*/
	BTUINT16 author_method;					/*Accept, Prompt, Reject (untrusted device), combined with security level "Authorization"*/
	BTUINT32 status;					/*Start, Stop, Connected*/
	BTUINT32 app_param;					/*User defined parameter*/
}LocalServiceStru;
typedef LocalServiceStru * PLocalServiceStru;

typedef struct  _RemoteServiceStru
{
	BTSVCHDL svc_hdl;					/*Handle assigned to the service record*/
	BTUINT16 status;					/*Not defined yet*/
	BTUINT16 svc_class;					/*Service type*/
	BTUINT32 sdp_hdl;					/*SDP service record handle*/
	BTUINT32 attrib_size;				/*Size of the buffer pointed by svc_attrib*/
	BTLPVOID svc_attrib;				/*Profile specific attributes*/
	BTUINT32 app_param;					/*User defined parameter*/
}RemoteServiceStru;
typedef RemoteServiceStru * PRemoteServiceStru;

typedef struct  _ConnectionManagerStru
{
	BTCONNHDL conn_hdl;					/*Handle assigned to the connection instance*/
	BTSVCHDL svc_hdl;					/*Handle to the service record pointed by this connection*/
	BTDEVHDL dev_hdl;					/*Handle to the remote device*/
	BTUINT32 duration;					/*Sytem tick value when this connection is created*/
	BTUINT32 bytes_sent;				/*Total bytes sent for this connection*/	
	BTUINT32 bytes_rcv;					/*Total bytes received for this connection*/	
	BTUINT16 flag;						/*Server, Client*/
	BTUINT16 svc_class;					/*Service type of this connection, to identify connection whose svc_hdl is invalid. */
	BTUINT32 app_param;					/*User defined parameter*/	
}ConnectionManagerStru;
typedef ConnectionManagerStru * PConnectionManagerStru;

/* the bd_addr and svc_class determined one unique shortcut */
typedef struct  _ShortCutManagerStru
{
	BTSHCHDL shc_hdl;						/* handle assigned to the shortcut instance*/
	BTUINT8 shc_name[BTSDK_SHORTCUT_NAME_LEN];	/* name of the shortcut, must in UTF-8 */
	BTUINT8 bd_addr[BTSDK_BDADDR_LEN];		/* bt address of local device */
	BTUINT16 svc_class;						/* service class */
	BTCONNHDL conn_hdl;						/* handle assigned to the connection instance*/
	BTDEVHDL dev_hdl;						/* device handle */
	BTDEVHDL svc_hdl;						/* service handle */
	BTUINT32 dev_class;						/* device class of the remote device*/
	BTBOOL is_default;						/* is default shortcut */
	BTUINT8 sec_level;						/* Security level. */
	BTUINT8 *svc_name;						/* Service name. */
	BTUINT8 *dev_name;						/* Device name. */
	BTUINT16 shc_attrib_len;				/* the length of shortcut attribute */
	BTUINT8 *pshc_attrib;					/* shortcut attribute */
	BTUINT16 status;						/* the status */
	BTUINT16 ref_count;						/*Refrence to this item*/
	BTUINT32 app_param;						/*User defined parameter*/	
}ShortCutManagerStru;
typedef ShortCutManagerStru *PShortCutManagerStru;

typedef struct _AuthorReqIndStru
{
	BTUINT8 bd_addr[BTSDK_BDADDR_LEN];
	BTUINT16 channel;
	BTUINT8 proto;
}AuthorReqIndStru, *PAuthorReqIndStru;

/*************** Internal Global Variables ******************/
extern struct BtList* g_rmt_dev_list;			/*List of remote devices*/
extern LocalDeviceStru  g_loc_dev;				/*Local device attributes*/
extern struct BtList* g_conn_list;				/*List of connections*/
extern BtSdkCallBackStru * g_sdk_cb;			/*Callback function*/
extern struct BtList* g_shc_list;				/* List of short cut */

#endif

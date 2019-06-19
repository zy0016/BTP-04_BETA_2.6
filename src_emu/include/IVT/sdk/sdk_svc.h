/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_svc.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/



#ifndef _SDK_SVC_H
#define _SDK_SVC_H



/* Type of Data Element */
#define BTSDK_DETYPE_NULL						0x00
#define BTSDK_DETYPE_UINT						0x01
#define BTSDK_DETYPE_INT						0x02
#define BTSDK_DETYPE_UUID						0x03
#define BTSDK_DETYPE_STRING						0x04
#define BTSDK_DETYPE_BOOL						0x05
#define BTSDK_DETYPE_DESEQ						0x06
#define BTSDK_DETYPE_DEALT						0x07
#define BTSDK_DETYPE_URL						0x08

/* Server Status */
#define BTSDK_SERVER_STARTED					0x01
#define BTSDK_SERVER_STOPPED					0x02
#define BTSDK_SERVER_CONNECTED					0x03

/* Status of Remote Service */
#define BTSDK_RMTSVC_IDLE						0x00
#define BTSDK_RMTSVC_CONNECTED					0x01

/*************** Structure Definition ******************/

/*
		Basic SDP Element
*/
typedef struct _BtSdkUUIDStru
{
    BTUINT32 Data1;
    BTUINT16 Data2;
    BTUINT16 Data3;
    BTUINT8  Data4[8];
} BtSdkUUIDStru, PBtSdkUUIDStru;

typedef struct _BtSdkUINT64Stru
{
	BTUINT32 higher_4bytes;
	BTUINT32 lower_4bytes;
} BtSdkUInt64Stru, PBtSdkUInt64Stru;

typedef struct _BtSdkDataElementStru
{
	BTUINT8	alignt_bytes;						/*alignt_bytes+value_size is the actual size of the buffer*/
	BTUINT8 type;								/*Type of the data element*/
	BTUINT16 sub_element_number;				/*If this element is of sequence type, it is the number of sub elements in the sequence*/
	BTUINT32 value_size;						/*Size of the actual value in bytes*/
	BTUINT8 de_value[1];						/*Value of this data element*/
} BtSdkDataElementStru, *PBtSdkDataElementStru;

typedef struct _BtSdkSDPAttributeStru
{
	BTUINT32 size;
	BTUINT16 attr_id;
	BTUINT16 reserved;
	BtSdkDataElementStru attr_value;
} BtSdkSDPAttributeStru, *PBtSdkSDPAttributeStru;

typedef struct _BtSdkSDPSearchPatternStru
{
	BTUINT32 mask;					/*Specifies the valid bytes in the uuid*/
	BtSdkUUIDStru uuid;				/*UUID value*/
} BtSdkSDPSearchPatternStru, *PBtSdkSDPSearchPatternStru;

/* Possible flags for member 'mask' in _BtSdkSDPSearchPatternStru */
#define BTSDK_SSPM_UUID16					0x0001
#define BTSDK_SSPM_UUID32					0x0002
#define BTSDK_SSPM_UUID128					0x0004


/*
		Local server attributes
*/
typedef struct  _BtSdkLocalServerAttrStru
{
	BTUINT16 mask;					/*Decide which parameter to be modified or retrieved*/
	BTUINT16 service_class;			/*Service class, 16bit UUID*/
	BTUINT8 svc_name[BTSDK_SERVICENAME_MAXLENGTH];	/*must in UTF-8*/
	BTUINT16 security_level;		/*Authorization, Authentication, Encryption, None*/
	BTUINT16 author_method;			/*Accept, Prompt, Reject (untrusted device), combined with security level "Authorization"*/
	BTLPVOID ext_attributes;		/*Profile specific attributes*/
	BTUINT32 app_param;				/*User defined parameters*/
} BtSdkLocalServerAttrStru, *PBtSdkLocalServerAttrStru;

/* Possible flags for member 'mask' in _BtSdkLocalServerAttrStru */
#define BTSDK_LSAM_SERVICENAME					0x0001
#define BTSDK_LSAM_SECURITYLEVEL				0x0002
#define BTSDK_LSAM_AUTHORMETHOD					0x0004
#define BTSDK_LSAM_EXTATTRIBUTES				0x0008
#define BTSDK_LSAM_APPPARAM						0x0010


typedef struct _BtSdkLocalSPPServerAttrStru 
{
	BTUINT32 size;
	BTUINT16 mask;
	BTUINT8 com_index;
} BtSdkLocalSPPServerAttrStru, *PBtSdkLocalSPPServerAttrStru;

/* Possible flags for member 'mask' in _BtSdkLocalSPPServerAttrStru */
#define BTSDK_LSPPSAM_COMINDEX					0x0001


typedef struct  _BtSdkLocalFTPServerAttrStru
{
	BTUINT32 size;
	BTUINT16 mask;
	BTUINT16 desired_access;
	BTUINT8 root_dir[BTSDK_PATH_MAXLENGTH];
} BtSdkLocalFTPServerAttrStru, *PBtSdkLocalFTPServerAttrStru;

/* Possible flags for member 'mask' in _BtSdkLocalFTPServerAttrStru */
#define BTSDK_LFTPSAM_DESIREDACCESS				0x0001
#define BTSDK_LFTPSAM_ROOTDIR					0x0002
#define BTSDK_LFTPSAM_ALL						0x0003

/* Possible values for member 'desired_access' in _BtSdkLocalFTPServerAttrStru */
#define BTSDK_FTPDA_NOACCESS					0x0000
#define BTSDK_FTPDA_READWRITE					0x0001
#define BTSDK_FTPDA_READONLY					0x0002


typedef struct  _BtSdkLocalOPPServerAttrStru
{
	BTUINT32 size;									/*Size of this structure, use for verification and versioning.*/
	BTUINT16 mask; 									/*Decide which parameter to be modified or retrieved*/
	BTUINT16 vcard_support;							/*Specify vCard version supported and operation allowed*/
	BTUINT16 vcal_support;							/*Specify vCal version supported and operation allowed*/
	BTUINT16 vnote_support;							/*Specify operation on vNote allowed*/
	BTUINT16 vmessage_support;						/*Specify operation on vMessage allowed*/
	BTUINT8 inbox_path[BTSDK_PATH_MAXLENGTH];		/*must in UTF-8*/
	BTUINT8 outbox_path[BTSDK_PATH_MAXLENGTH];		/*must in UTF-8*/
	BTUINT8 own_card[BTSDK_CARDNAME_MAXLENGTH]; 	/*must in UTF-8*/
} BtSdkLocalOPPServerAttrStru, *PBtSdkLocalOPPServerAttrStru;

/* Possible flags for member 'mask' in _BtSdkLocalOPPServerAttrStru */
#define BTSDK_LOPPSAM_VCARDSUPPORT				0x0001
#define BTSDK_LOPPSAM_VCALSUPPORT				0x0002
#define BTSDK_LOPPSAM_VNOTESUPPORT				0x0004
#define BTSDK_LOPPSAM_VMESSAGESUPPORT			0x0008
#define BTSDK_LOPPSAM_INBOXPATH					0x0010
#define BTSDK_LOPPSAM_OUTBOXPATH				0x0020
#define BTSDK_LOPPSAM_OWNCARD					0x0040
#define BTSDK_LOPPSAM_ALL						0x007f

/* Possible operation flags for members '****_support' in _BtSdkLocalOPPServerAttrStru */
#define BTSDK_OBJ_ACCEPT_PUSH					0x0100
#define BTSDK_OBJ_ACCEPT_PULL					0x0200

/* Possible version flags for members 'vcard_support' in _BtSdkLocalOPPServerAttrStru */
#define BTSDK_OPPVCARD_21						0x0001
#define BTSDK_OPPVCARD_30						0x0002

/* Possible version flags for members 'vcal_support' in _BtSdkLocalOPPServerAttrStru */
#define BTSDK_OPPVCAL_10						0x0001
#define BTSDK_OPPICAL_20						0x0002


typedef struct  _BtSdkLocalNAPServerAttrStru
{
	BTUINT32 size;
	BTUINT16 mask;
	BTUINT16 security_description;
	BTUINT32 max_access_rate;
	BTUINT16 net_access_type;
	BTUINT16 packet_type_num;
	BTUINT16 packet_type_list[BTSDK_PACKETTYPE_MAXNUM];
} BtSdkLocalNAPServerAttrStru, *PBtSdkLocalNAPServerAttrStru;

/* Possible flags for member 'mask' in _BtSdkLocalNAPServerAttrStru */
#define BTSDK_LNAPSAM_SECURITYDESCRIPTION		0x0001
#define BTSDK_LNAPSAM_MAXACCESSRATE				0x0002
#define BTSDK_LNAPSAM_NETACCESSTYPE				0x0004
#define BTSDK_LNAPSAM_PACKETTYPE				0x0008
#define BTSDK_LNAPSAM_ALL						0x000f


typedef struct  _BtSdkLocalGNServerAttrStru
{
	BTUINT32 size;
	BTUINT16 mask;
	BTUINT16 security_description;
	BTUINT16 packet_type_num;
	BTUINT16 packet_type_list[BTSDK_PACKETTYPE_MAXNUM];
} BtSdkLocalGNServerAttrStru, *PBtSdkLocalGNServerAttrStru;

/* Possible flags for member 'mask' in _BtSdkLocalGNServerAttrStru */
#define BTSDK_LGNSAM_SECURITYDESCRIPTION		0x0001
#define BTSDK_LGNSAM_PACKETTYPE					0x0002
#define BTSDK_LGNSAM_ALL						0x0003


typedef struct  _BtSdkLocalPANUServerAttrStru
{
	BTUINT32 size;
	BTUINT16 mask;
	BTUINT16 security_description;
	BTUINT16 packet_type_num;
	BTUINT16 packet_type_list[BTSDK_PACKETTYPE_MAXNUM];
} BtSdkLocalPANUServerAttrStru, *PBtSdkLocalPANUServerAttrStru;

/* Possible flags for member 'mask' in _BtSdkLocalPANUServerAttrStru */
#define BTSDK_LPANUSAM_SECURITYDESCRIPTION		0x0001
#define BTSDK_LPANUSAM_PACKETTYPE				0x0002
#define BTSDK_LPANUSAM_ALL						0x0003


/*
		Remote service record attributes
*/
typedef struct _BtSdkRemoteServiceAttrStru
{
	BTUINT16 mask;									/*Decide which parameter to be retrieved*/
	BTUINT16 svc_class;								/*Type of this service record*/
	BTUINT8 svc_name[BTSDK_SERVICENAME_MAXLENGTH];	/*In UTF-8*/
	BTLPVOID ext_attributes;						/*Free by the APP*/
	BTUINT16 status;
} BtSdkRemoteServiceAttrStru, *PBtSdkRemoteServiceAttrStru;

/* Possible flags for member 'mask' in _BtSdkRemoteServiceAttrStru */
#define BTSDK_RSAM_SERVICENAME					0x0001
#define BTSDK_RSAM_EXTATTRIBUTES				0x0002


typedef struct _BtSdkRmtSPPSvcExtAttrStru 
{
	BTUINT32 size;						/*Size of BtSdkRmtSPPSvcExtAttrStru*/
	BTUINT8  server_channel;			/*Server channel value of this SPP service record*/
} BtSdkRmtSPPSvcExtAttrStru, *PBtSdkRmtSPPSvcExtAttrStru;


typedef struct _BtSdkRmtHIDSvcExtAttrStru
{
	BTUINT32 size;						/* Size of BtSdkRmtSPPSvcExtAttrStru */
	BTUINT16 mask;						/* Specifies whether an optional attribute value is available */
	BTUINT16 release_number;			/* Value of HIDDeviceReleaseNumber attribute */
	BTUINT16 parser_version;			/* Value of HIDParserVersion attribute */
	BTUINT8  sub_class;					/* Value of HIDDeviceSubclass attribute */
	BTUINT8  country_code;				/* Value of HIDCountryCode attribute */
	BTBOOL	 virtual_cable;				/* Value of HIDVirtualCable attribute */
	BTBOOL   reconnect_initiate;		/* Value of HIDReconnectInitiate attribute */
	BTBOOL	 sdp_disable;				/* Value of HIDSDPDisable attribute */
	BTBOOL	 battery_power;				/* Value of HIDBatteryPower attribute */
	BTBOOL	 remote_wake;				/* Value of HIDRemoteWake attribute */
	BTBOOL	 boot_device;				/* Value of HIDBootDevice attribute */
	BTUINT16 supervision_timeout;		/* Value of HIDSupervisionTimeout attribute */
	BTBOOL	 normally_connectable;		/* Value of HIDNormallyConnectable attribute */
	BTUINT16 desc_list_size;			/* Total size of the descriptor list. */
	BTUINT8	 descriptor_list[1];		/* List of HID class descriptor. */
} BtSdkRmtHIDSvcExtAttrStru, *PBtSdkRmtHIDSvcExtAttrStru;

/* Possible flags for member 'mask' in _BtSdkRmtHIDSvcExtAttrStru */
#define BTSDK_HIDM_SDPDISABLE			0x0001
#define BTSDK_HIDM_BATTERYPWR			0x0002
#define BTSDK_HIDM_RMTWAKE				0x0004
#define BTSDK_HIDM_SUPERTO				0x0008
#define BTSDK_HIDM_NORMCONN				0x0010


typedef struct _BtSdkRmtDISvcExtAttrStru
{
	BTUINT32 size;						/* Size of BtSdkRmtSPPSvcExtAttrStru */
	BTUINT16 mask;						/* Specifies whether an optional attribute value is available */
	BTUINT16 spec_id;					/* Value of SpecificationID attribute */
	BTUINT16 vendor_id;					/* Value of VendorID attriubte attribute */
	BTUINT16 product_id;				/* Value of ProductID attribute attribute */
	BTUINT16 version;					/* Value of Version attribute attribute */
	BTBOOL	 primary_record;			/* Value of PrimaryRecord attribute */
	BTUINT16 vendor_id_source;			/* Value of VendorIDSource attribute */
	BTUINT16 list_size;					/* Size of the text string list */
	BTUINT8  str_url_list[1];			/* List of ClientExecutableURL, DocumentationURL and 
										   ServiceDescription attributes. */
} BtSdkRmtDISvcExtAttrStru, *PBtSdkRmtDISvcExtAttrStru;

/* Possible flags for member 'mask' in _BtSdkRmtHIDSvcExtAttrStru */
#define BTSDK_DIM_CLIENTEXEURL				0x0001
#define BTSDK_DIM_DOCURL					0x0002


/*************** Function Prototype ******************/
/* Service Manager */
BTSVCHDL Btsdk_AddServer(BtSdkLocalServerAttrStru *attribute);
BTINT32 Btsdk_RemoveServer(BTSVCHDL handle);
BTINT32 Btsdk_UpdateServerAttributes(BTSVCHDL handle, BtSdkLocalServerAttrStru *attribute);
BTINT32 Btsdk_StartServer(BTSVCHDL handle);
BTINT32 Btsdk_StopServer(BTSVCHDL handle);
BTINT32 Btsdk_GetServerStatus(BTSVCHDL handle, BTUINT16 *status);
BTINT32 Btsdk_GetServerAttributes(BTSVCHDL handle, BtSdkLocalServerAttrStru *attribute);
BTINT32 Btsdk_GetLocalServiceList(BTSVCHDL * svc_hdl, BTUINT32 * svc_count);

/* Service Discovery */
BTINT32 Btsdk_BrowseRemoteServicesEx(BTDEVHDL dev_hdl, PBtSdkSDPSearchPatternStru psch_ptn, 
									 BTUINT32 ptn_num, BTSVCHDL *svc_hdl, BTUINT32 *svc_count);
BTINT32 Btsdk_BrowseRemoteServices(BTDEVHDL dev_hdl, BTSVCHDL *svc_hdl, BTUINT32 *svc_count);
BTINT32 Btsdk_GetRemoteServiceListEx(BTDEVHDL dev_hdl, PBtSdkSDPSearchPatternStru psch_ptn, 
									 BTUINT32 ptn_num, BTSVCHDL *svc_hdl, BTUINT32 *svc_count);
BTINT32 Btsdk_GetRemoteServiceList(BTDEVHDL dev_hdl, BTSVCHDL *svc_hdl, BTUINT32 *svc_count);
BTINT32 Btsdk_GetRemoteServiceAttributes(BTSVCHDL svc_hdl, BtSdkRemoteServiceAttrStru *attribute);
BTINT32 Btsdk_RefreshRemoteServiceAttributes(BTSVCHDL svc_hdl, BtSdkRemoteServiceAttrStru *attribute);
BTINT32 Btsdk_SetRemoteServiceParam(BTSVCHDL svc_hdl, BTUINT32 app_param);
BTINT32 Btsdk_GetRemoteServiceParam(BTSVCHDL svc_hdl, BTUINT32 *papp_param);

#endif

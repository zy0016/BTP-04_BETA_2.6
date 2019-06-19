/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_rmt.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _SDK_LOC_H
#define _SDK_LOC_H

#define BTSDK_LMP_FEATURES_LEN					8
#define BTSDK_LMP_FEATURES_LEN					8

/* Discovery Mode for Btsdk_SetDiscoveryMode() and Btsdk_GetDiscoveryMode() */
#define BTSDK_GENERAL_DISCOVERABLE				0x01
#define BTSDK_LIMITED_DISCOVERABLE				0x02
#define BTSDK_DISCOVERABLE						BTSDK_GENERAL_DISCOVERABLE
#define BTSDK_CONNECTABLE						0x04
#define BTSDK_PAIRABLE							0x08
#define BTSDK_DISCOVERY_DEFAULT_MODE			(BTSDK_DISCOVERABLE | BTSDK_CONNECTABLE | BTSDK_PAIRABLE)


typedef struct _BtSdkLocalLMPInfoStru
{
	BTUINT8 lmp_feature[8];				/* LMP features */
	BTUINT16 manuf_name;				/* the name of the manufacturer */
	BTUINT16 lmp_subversion;			/* the sub version of the LMP firmware */
	BTUINT8 lmp_version;				/* the main version of the LMP firmware */
	BTUINT8 hci_version;				/* HCI version */
	BTUINT16 hci_revision;				/* HCI revision */
	BTUINT8 country_code;				/* country code */
} BtSdkLocalLMPInfoStru, *PBtSdkLocalLMPInfoStru;

typedef struct _BtSdkVendorCmdStru
{
	BTUINT16 ocf;			/* OCF Range (10 bits): 0x0000-0x03FF */
	BTUINT8	 param_len;		/* length of param in bytes */
	BTUINT8	 param[1];		/* Parameters to be packed in the vendor command. Little endian is adopted. */
} BtSdkVendorCmdStru, *PBtSdkVendorCmdStru;

typedef struct _BtSdkEventParamStru
{
	BTUINT8 param_len;		/* length of param in bytes */
	BTUINT8 param[1];		/* Event parameters. */
} BtSdkEventParamStru, *PBtSdkEventParamStru;

typedef struct _BtSdkCommSettingStru
{
	BTUINT32 baud_rate;		/* Baud rate at which the communications device operates. */
	BTUINT8  byte_size;		/* Number of bits in the bytes transmitted and received. */
	BTUINT8  parity;		/* Parity scheme to be used. */
	BTUINT8  stop_bits;		/* Number of stop bits to be used. */
	BTUINT8  flow_control;	/* Flow control scheme to be used. */
} BtSdkCommSettingStru, *PBtSdkCommSettingStru;

/* Possible values of 'parity' member in BtSdkCommSettingStru: */
#define BTSDK_PARITY_NONE	0x00
#define BTSDK_PARITY_ODD	0x01
#define BTSDK_PARITY_EVEN	0x02

/* Possible values of 'stop_bits' member in BtSdkCommSettingStru: */
#define BTSDK_STOPBIT_ONE	0x00
#define BTSDK_STOPBITS_ONE5	0x01
#define BTSDK_STOPBITS_TWO	0x02

/* Possible values of 'flow_control' member in BtSdkCommSettingStru: */
#define BTSDK_FC_NONE         0x00
#define BTSDK_FC_DTRDSR       0x01
#define BTSDK_FC_RTSCTS       0x02
#define BTSDK_FC_XONXOFF      0x04


/*************** Function Prototype ******************/
/* Local Device Manager */
BTINT32 Btsdk_SetLocalName(BTUINT8* name, BTUINT16 len);
BTINT32 Btsdk_GetLocalName(BTUINT8* name, BTUINT16 *len);
BTINT32 Btsdk_SetDiscoveryMode (BTUINT16 mode);
BTINT32 Btsdk_GetDiscoveryMode(BTUINT16 *mode);
BTINT32 Btsdk_SetFixedPinCode(BTUINT8* pin_code_code, BTUINT16 size);
BTINT32 Btsdk_GetLocalPinCode(BTUINT8* pin_code, BTUINT16 *size);
BTINT32 Btsdk_GetLocalClassDevice(BTUINT32 *device_class);
BTINT32 Btsdk_SetLocalClassDevice(BTUINT32 device_class);
BTINT32 Btsdk_GetLocalBDAddr(BTUINT8 *bd_addr);
BTINT32 Btsdk_GetLocalLMPInfo(BtSdkLocalLMPInfoStru *lmp_info);
BTINT32 Btsdk_SetDefaultMode(void);
BTINT32 Btsdk_SetLocalDeviceParam(BTUINT32 app_param);
BTINT32 Btsdk_GetLocalDeviceParam(BTUINT32 *app_param);
BTINT32 Btsdk_VendorCommand(BTUINT32 ev_flag, PBtSdkVendorCmdStru in_cmd, PBtSdkEventParamStru out_ev);

BTUINT32 Btsdk_GetLocalComPortHandle(void);
BTINT32 Btsdk_SetDefaultCommSettings(PBtSdkCommSettingStru pcs);

#endif

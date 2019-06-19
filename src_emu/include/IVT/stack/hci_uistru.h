/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
* Copyright (c) 2000 IVT Corporation
*
* All rights reserved.
* 
---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    hci_uistru.h
Abstract:
	The module defines the structures of the user interface
	
Author:
    Hong Lin
Revision History:
4.5		change interface of ReadLocalInfo and WriteLocalInfo
4.11	change structures of ReadLocalInfo and WriteLocalInfo
4.30	add field to the structure WriteLocalInfoParamStru
---------------------------------------------------------------------------*/



#ifndef _HCI_UISTRU_H
#define _HCI_UISTRU_H

/* sort of data sent or received */
#define HCI_COMMAND_DATA		1
#define HCI_ACL_DATA			2
#define HCI_SCO_DATA			3
#define	HCI_EVENT_DATA			4

/* length of baseband address */
#define BD_ADDR_LEN				6
/*length of class of device */
#define CLS_DEV_LEN				3
/* length of LAP */
#define LAP_LEN					3
/* length of LMP features */
#define LMP_FEATURES_LEN		8
/* length of bluetooh name */
#define BT_NAME_LEN				248
/* maxinimum length of PIN code */
#define MAX_PIN_CODE_LEN		16
/* maxinimum length of Link key */
#define MAX_LINK_KEY_LEN		16

/*------------------------------------------STRUCTURE------------------------------------------*/

struct QosType{
	UCHAR flags;				/* flags reserved */
	UCHAR svc_type;			/* service type */
	DWORD token_rate;		/* tokeb rate */
	DWORD peak_bdwid;		/* peak bandwidth */
	DWORD latency;			/* latency */
	DWORD delay_var;		/* delay variation */
};

/* structure of link mode for hold, sniff, park */
struct HoldStru{
	WORD max_interval;				/* maximum interval in hold mode */
	WORD min_interval;				/* minimum interval in hold mode */
};

struct SniffStru{
	WORD max_interval;				/* maximum interval in sniff mode */
	WORD min_interval;				/* minimum interval in sniff mode */
	WORD attempt;					/* attempt in sniff mode */
	WORD timeout;					/* timeout in sniff mode */
};

struct ParkStru{
	WORD max_interval;				/* maximum interval in park mode */
	WORD min_interval;				/* minimum interval in park mode */
};

/* structure of ChangeConnParam */
struct ChangeConnParamStru{
	WORD pkt_type;						/* packet type */
	UCHAR encry_enable;					/* encryption enble/disable */
};

/* structure for inquiry command */
struct InquiryScanStru{
	char lap[LAP_LEN];					/* LAP */
	UCHAR inq_len;						/* inquiry length */
    UCHAR num_rsp;						/* number of responses */
};

/* structure for inquiry result */
struct InquiryResultStru 
{
	UCHAR bd_addr[BD_ADDR_LEN];				/* baseband address */
	char class_of_dev[CLS_DEV_LEN];			/* class of device */
};

/* structure for periodic inquiry command */
struct PerInquiryScanStru{
	WORD max_len;						/* maximum period length */
	WORD min_len;						/* minimum period length */
	char lap[LAP_LEN];					/* LAP */
	UCHAR inq_len;						/* inquiry length */
    UCHAR num_rsp;						/* number of responses */
};

/* information of the remote device */
struct RemDevInfoStru{
	char rem_name[BT_NAME_LEN];			/* name requested */
	char lmp_feature[LMP_FEATURES_LEN];	/* LMP features */
	UCHAR lmp_ver;						/* LMP version */
	WORD manuf_name;					/* manufacturer name */
	WORD lmp_subver;					/* LMP subversion */
	WORD clk_off;						/* clock offset */
};

/* ACL data structure for L2CAP */
struct DataTransStru{
	UCHAR bd_addr[BD_ADDR_LEN];					/* baseband address */
	WORD len;							/* length of packet */
	UCHAR *packet;						/* buffer of packet */
};

/* SCO data structure for AUDIO data */
struct ScoDataTransStru{
	WORD conn_hdl;						/* connection handle */
	WORD len;							/* length of packet */
	UCHAR *packet;						/* buffer of packet */
};


/* one structure for condition parameter in command Set_Event_Filter */
struct SetEventFilterStru{
	UCHAR type;							/* filter type */
	UCHAR cond_type;						/* filter condition type */
	char cls_dev[CLS_DEV_LEN];				/* class of device */
	char cls_dev_mask[CLS_DEV_LEN];			/* class of device mask */
	UCHAR bd_addr[BD_ADDR_LEN];					/* baseband address */
	UCHAR auto_accept;					/* automatic accept */
};

/* pin code input from user */
struct PinCodeStru {
	UCHAR bd_addr[BD_ADDR_LEN];					/* baseband address */
    UCHAR pin_len;						/* length of pin code */
	char pin[MAX_PIN_CODE_LEN];			/* PIN code */
};

/* link key input from user */
struct LinkKeyStru {
	UCHAR bd_addr[BD_ADDR_LEN];					/* baseband address */
	char linkey[MAX_LINK_KEY_LEN];		/* link key */
};


/* a big structure for writing Local Info */
struct WriteLocalInfoParamStru{
	struct{
		WORD conn_hdl;					/* connection handle */
		WORD link_policy_settings;		/* link policy settings */
	}link_policy;						/* link policy settings */
	UCHAR pin_type;						/* pin type */
	WORD conn_accept_timeout;			/* connection accept timeout */
	WORD page_timeout;					/* page timeout */
	UCHAR scan_enable;					/* scan enable */
	struct{
		WORD page_scan_interval;			/* page scan interval */
		WORD page_scan_window;			/* page scan window */
	}page_scan_act;						/* page scan activity */
	UCHAR sco_fc_enable;					/* sco flow control enable */
	struct{
		WORD inquiry_scan_interval;		/* inquiry scan interval */
		WORD inquiry_scan_window;		/* inquiry scan window */
	}inquiry_scan_act;					/* inquiry scan activity */
	UCHAR authentication_enable;		/* authentication enable */
	UCHAR encryption_mode;				/* encryption mode */
	char class_of_device[CLS_DEV_LEN];	/* class of device */
	WORD voice_channel_setting;			/* voice channel setting */
	struct{
		WORD conn_hdl;					/* connection handle */
		WORD flush_timeout;				/* flush timeout */
	}auto_flush_timeout;				/* automatic flush timeout */
	UCHAR num_broadcast_retran;			/* number of broadcast retransimissions */
	UCHAR hold_mode_activity;			/* hold mode activity */
	struct{
		WORD conn_hdl;					/* connection handle */
		UCHAR power_level;				/* power level */
	}transmit_power_level;				/* transmit power level */
	struct{
		WORD conn_hdl;					/* connection handle */
		WORD link_supervision_timeout;	/* link supervision timeout*/
	}link_supervision_timeout;			/* link supervision timeout*/
	UCHAR num_support_iac;				/* number of supported iac */
	struct{
		UCHAR num_current_iac;			/* number of current IAC */
		UCHAR reservered[3];			/* reservered */
		char iac_lap[250];				/* IAC LAP */
	}current_iac_lap;					/* current IAC LAP */
	UCHAR page_scan_period_mode;			/* page scan period mode */
	UCHAR page_scan_mode;				/* page scan mode */
	struct{
		UCHAR hci_version;				/* HCI version */
		WORD hci_revsion;				/* HCI revision */
		UCHAR lmp_version;				/* lMP version */
		WORD manufacture_name;			/* manufacturer name */
		WORD lmp_subversion;			/* LMP subversion */
	}local_ver_info;					/* local version information */
	char lmp_features[LMP_FEATURES_LEN];/* LMP features */
	UCHAR country_code;					/* country code */
	UCHAR bd_addr[BD_ADDR_LEN];					/* baseband address */
	struct{
		WORD conn_hdl;					/* connection handle */
		UCHAR failed_contact_counter;	/* failed contact counter */
	}failed_counter;					/* falied contact counter */
	WORD conn_hdl;						/* connection handle */
	struct{
		WORD conn_hdl;					/* connection handle */
		UCHAR link_quality;				/* link quality */
	}link_quality;						/* link quality */
	struct{
		WORD conn_hdl;					/* connection handle */
		UCHAR rssi;						/* received signal strength indication */
	}rssi;								/* received signal strength indication */
	UCHAR loopback_mode;					/* loopback mode */
};

/* returned parameres of reading/writing local information */

struct ReadLinkPolicyReturnStru{
	UCHAR status;						/* status */
	WORD conn_hdl;						/* connection handle */
	WORD link_policy_settings;			/* link policy settings */
};
struct WriteLinkPolicyReturnStru{
   	UCHAR status;						/* status */
	WORD conn_hdl;						/* connection handle */
};
struct ReadPinTypeReturnStru{
	UCHAR status;						/* status */
	UCHAR pin_type;						/* pin type */
};
struct ReadLocalNameReturnStru{
	UCHAR status;						/* status */
	char local_name[BT_NAME_LEN];		/* local name */
};
struct  ReadConnAcceptTimeoutReturnStru{
	UCHAR status;						/* status */
	WORD conn_accept_timeout;			/* connection accept timeout */
};
struct ReadPageTimeoutReturnStru{
	UCHAR status;						/* status */
	WORD page_timeout;					/* page timeout */
};
struct ReadScanEnableReturnStru{
	UCHAR status;						/* status */
	UCHAR scan_enable;					/* scan enable */
};
struct ReadPageScanActReturnStru{
	UCHAR status;						/* status */
	WORD page_scan_interval;			/* page scan interval */
	WORD page_scan_window;				/* page scan window */
};
struct ReadInquiryScanActReturnStru{
	UCHAR status;						/* status */
	WORD inq_scan_interval;				/* inquiry scan interval */
	WORD inq_scan_window;				/* inquiry scan window */
};
struct ReadAuthEnableReturnStru{
	UCHAR status;						/* status */
	UCHAR auth_enable;					/* authentication enable */
};
struct ReadEncryModeReturnStru{
	UCHAR status;						/* status */
	UCHAR encry_mode;					/* encryption mode */
};
struct ReadDevClassReturnStru{
    UCHAR status;						/* status */
	char class_of_device[CLS_DEV_LEN];	/* class of device */
};
struct ReadVoiceSettingReturnStru{
	UCHAR status;						/* status */
	WORD voice_channel_setting;			/* voice channel setting */
};
struct ReadAutoFlushTimeoutReturnStru{
	UCHAR status;						/* status */
	WORD conn_hdl;						/* connection handle */
	WORD flush_timeout;					/* flush timeout */
};
struct WriteAutoFlushTimeoutReturnStru{
	UCHAR status;						/* status */
	WORD conn_hdl;						
};
struct ReadNumBroadcastRetranReturnStru{
	UCHAR status;						/* status */
	UCHAR num_broadcast_retran;			/* number of broadcast retransimissions */
};
struct ReadHoldModeActReturnStru{
	UCHAR status;						/* status */
	UCHAR hold_mode_activity;			/* hold mode activity */
};
struct ReadTransPoweLevelReturnStru{
	UCHAR status;						/* status */
	WORD conn_hdl;
	UCHAR power_level;
};
struct WriteScoFlowControlEnableReturnStru{
	UCHAR status;						/* status */
	UCHAR sco_fc_enable;
};
struct ReadLinkSupervisionTimeoutReturnStru{
	UCHAR status;						/* status */
	WORD conn_hdl;						/* connection handle */
	WORD link_supervision_timeout;		/* link supervision timeout*/
};
struct WriteLinkSupervisionTimeoutReturnStru{
	UCHAR status;						/* status */
	WORD conn_hdl;
};
struct ReadNumSupportIACReturnStru{
	UCHAR status;						/* status */
	UCHAR num_support_iac;				/* number of supported iac */
};
struct ReadCurrIACReturnStru{
	UCHAR status;						/* status */
	UCHAR num_current_iac;				/* number of current IAC */
	char iac_lap[254];					/* IAC LAP */
};
struct ReadPageScanPeriodModeReturnStru{
	UCHAR status;						/* status */
	UCHAR page_scan_period_mode;			/* page scan period mode */
};
struct ReadPageScanModeReturnStru{
	UCHAR status;						/* status */
	UCHAR page_scan_mode;				/* page scan mode */
};
struct ReadLocalVerReturnStru{
	UCHAR status;						/* status */
	UCHAR hci_version;					/* HCI version */
	WORD hci_revsion;					/* HCI revision */
	UCHAR lmp_version;					/* lMP version */
	WORD manufacture_name;				/* manufacturer name */
	WORD lmp_subversion;				/* LMP subversion */
};
struct ReadLocalSupportReturnStru{
	UCHAR status;						/* status */
	char lmp_features[LMP_FEATURES_LEN];/* LMP features */
};
struct ReadCountryCodeReturnStru{
	UCHAR status;						/* status */
	UCHAR country_code;					/* country code */
};
struct ReadBDAddrReturnStru{ 
	UCHAR status;						/* status */
	UCHAR bd_addr[BD_ADDR_LEN];					/* baseband address */
};
struct ReadWriteFailedContactCountReturnStru{
    UCHAR status;						/* status */
	WORD conn_hdl;						/* connection handle */
	UCHAR failed_contact_counter;		/* failed contact counter */
};
struct ReadLinkQualityReturnStru{
	UCHAR status;						/* status */
	WORD conn_hdl;						/* connection handle */
	UCHAR link_quality;					/* link quality */
};
struct ReadRSSIReturnStru{
	UCHAR status;						/* status */
	WORD conn_hdl;						/* connection handle */
	CHAR rssi;							/* received signal strength indication */
};
struct ReadLoopbackModeReturnStru{
	UCHAR status;						/* status */
	UCHAR loopback_mode;					/* loopback mode */
};

/* parameters buffer of ReadLocalInfo */
struct ReadLocalInfoParamStru{
	WORD conn_hdl;						/* connection hadle */
	UCHAR pl_type;						/* power level type */
};

/** Added by He gang ************/
#define MAXDEVNAMELEN 64

#endif

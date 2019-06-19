#ifndef GAP_UI_H
#define GAP_UI_H
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
	gap_ui.h
    
Abstract:
	The module defines the indexes of operation codes and event codes
Author:
    Wang Bin
Revision History:
	2003.12.23 Zhang Tao 
	modified with bluetooth specification 1.2 version

---------------------------------------------------------------------------*/

/******************************************************************************
 *																			  *
 *					GAP_ExecuteCommand function manual					 	  *
 *																			  *
 ******************************************************************************
I have implemented all of HCI commands(see Part H:1 of Bluetooth 11 Specifications
book for each command in detail) in one GAP function, GAP_ExecuteCommand(). This 
function is a synchronous one. That means the function will wait for the proper event
or timeout event to occur before it returns.The prototype of this function is 

		UCHAR GAP_ExecuteCommand(UCHAR idx, UCHAR *inp, UCHAR *outp);

idx		idx is a UCHAR type parameter, which specifies the command index you want
		to execute.
inp		inp is the pointer to the address of input parameters, and NULL is valid.
outp 	outp is the pointer to the address of output parameters.

The content of input parameters and outp parameters is  command  specified.  
Part  H:1 of Bluetooth 11 Specifications book has detailed definiation for each 
command's parameter. 

I write a table below to describe each command index with its corresponding input
and output paramter. You can use this table as a quick manual to access the service
provided by GAP. 

&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
index							input							output
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

***Link control commands***
IDX_INQ							struct GapInquiryStru			UCHAR status
IDX_INQ_CANCEL					NULL							UCHAR status
IDX_PERIODIC_INQ_MODE			struct GapPeriodicInqModeStru	UCHAR status
IDX_EXIT_PERIODIC_INQ_MODE		NULL							UCHAR status
IDX_CREATE_CONN					struct GapCreateConnStru		struct GapConnCompEvStru
IDX_DISC						struct GapDisconnStru			struct GapDisconnCompEvStru
IDX_ADD_SCO_CONN				struct GapAddScoConnStru		struct GapConnCompEvStru
IDX_ACC_CONN_REQ				struct GapAcceptConnReqStru		struct GapConnCompEvStru
IDX_REJ_CONN_REQ				struct GapRejectConnReqStru		struct GapConnCompEvStru
IDX_LINK_KEY_REQ_REPLY			struct GapLinkKeyStru			struct GapLinkKeyOutStru
IDX_LINK_KEY_REQ_NEG_REPLY		struct GapLinkKeyNegStru		struct GapLinkKeyOutStru
IDX_PIN_CODE_REQ_REPLY			struct GapPinCodeStru			struct GapPinCodeOutStru
IDX_PIN_CODE_REQ_NEG_REPLY		struct GapPinCodeNegStru		struct GapPinCodeOutStru
IDX_CHANGE_CONN_PKT_TYPE		struct GapChangePacketTypeStru	struct GapPacketChangeEvStru
IDX_AUTH_REQ					WORD conn_hdl					struct GapAuthCompEvStru
IDX_SET_CONN_ENCRY				struct GapSetConnEncryStru		struct GapEncryChangeEvStru
IDX_CHANGE_CONN_LINK_KEY		WORD conn_hdl					struct GapChangeLinkKeyEvStru
IDX_MASTER_LINKKEY				struct GapMasterLinkKeyStru		struct GapMasterLinkKeyEvStru
IDX_REM_NAME_REQ				struct GapRemNameReqStru		struct GapRemNameReqCompEvStru
IDX_READ_REM_SUP_FEATURES		WORD conn_hdl					struct GapReadRemFeatureCompEvStru
IDX_READ_REM_VER_INFO			WORD conn_hdl					struct GapReadRemVerInfoCompEvStru
IDX_READ_CLK_OFF				WORD conn_hdl					struct GapReadClkOffsetCompEvStru
							
****Link policy commands****
IDX_HOLD_MODE					struct GapHoldModeStru			struct GapModeChangeEvStru
IDX_SNIFF_MODE					struct GapSniffModeStru			struct GapModeChangeEvStru
IDX_EXIT_SNIFF_MODE				WORD conn_hdl					struct GapModeChangeEvStru
IDX_PARK_MODE					struct GapParkModeStru			struct GapModeChangeEvStru
IDX_EXIT_PARK_MODE				WORD conn_hdl					struct GapModeChangeEvStru
IDX_QOS_SETUP					struct GapQosSetupStru			struct GapQosSetupCompEvStru
IDX_ROLE_DCV					WORD conn_hdl					struct GapRoleDiscoveryOutStru
IDX_SW_ROLE						struct GapSwitchRoleStru		struct GapRoleChangeEvStru
IDX_READ_LINK_POLICY			WORD conn_hdl					struct GapReadLPSettingOutStru
IDX_WRITE_LINK_POLICY			struct GapWriteLPSettingStru	struct GapWriteLPSettingOutStru

***Controller & baseband commands***
IDX_SET_EV_MASK					struct GapSetEvMaskStru			UCHAR status
IDX_RESET						NULL							UCHAR status	
IDX_SET_EV_FILTER				struct GapSetEvFilterStru		UCHAR status
IDX_FLUSH						WORD conn_hdl					struct GapFlushOutStru
IDX_READ_PIN_TYPE				NULL							struct GapReadPinTypeOutStru	
IDX_WRITE_PIN_TYPE				struct GapWritePinTypeStru		UCHAR status
IDX_CREATE_NEW_UNIT_KEY			NULL							UCHAR status	
IDX_READ_STORED_LINK_KEY		struct GapReadStoredKeyStru		struct GapReadStoredKeyOutStru
IDX_WRITE_STORED_LINK_KEY		struct GapWriteStoredKeyStru	struct GapWriteStoredKeyOutStru
IDX_DEL_STORED_LINK_KEY			struct GapDelStoredKeyStru		struct GapDelStoredKeyOutStru
IDX_CHANGE_LOCAL_NAME			struct GapChangeNameStru		UCHAR status
IDX_READ_LOCAL_NAME				NULL							struct GapReadNameOutStru
IDX_READ_CONN_ACC_TIMEOUT		NULL							struct GapReadConnTimeOutStru	
IDX_WRITE_CONN_ACC_TIMEOUT		struct GapWriteConnTimeStru		UCHAR status
IDX_READ_PAGE_TIMEOUT			NULL							struct GapReadPageTimeOutStru
IDX_WRITE_PAGE_TIMEOUT			struct GapWritePageTimeStru		UCHAR status	
IDX_READ_SCAN_EN				NULL							struct GapReadScanEnOutStru	
IDX_WRITE_SCAN_EN				struct GapWriteScanEnStru		UCHAR status	
IDX_READ_PGSCAN_ACT				NULL							struct GapReadPageScanActOutStru	
IDX_WRITE_PGSCAN_ACT			struct GapWritePageScanActStru	UCHAR status
IDX_READ_INQ_SCAN_ACT			NULL							struct GapReadInqScanActOutStru	
IDX_WRITE_INQ_SCAN_ACT			struct GapWriteInqScanActStru	UCHAR status	
IDX_READ_AUTH_EN				NULL							struct GapReadAuthEnOutStru
IDX_WRITE_AUTH_EN				struct GapWriteAuthEnStru		UCHAR status
IDX_READ_ENCRY_MODE				NULL							struct GapReadEncryModeOutStru	
IDX_WRITE_ENCRY_MODE			struct GapWriteEncryModeStru	UCHAR status
IDX_READ_DEV_CLASS				NULL							struct GapReadClassOfDevOutStru	
IDX_WRITE_DEV_CLASS				struct GapWriteClassOfDevStru	UCHAR status
IDX_READ_VOICE_SETTING			NULL							struct GapReadVoiceSettingOutStru	
IDX_WRITE_VOICE_SETTING			struct GapWriteVoiceSettingStru	UCHAR status
IDX_READ_AUTO_FLUSH_TIMEOUT		WORD conn_hdl					struct GapReadAutoFlushOutStru
IDX_WRITE_AUTO_FLUSH_TIMEOUT	struct GapWriteAutoFlushStru	struct GapWriteAutoFlushOutStru
IDX_READ_NUM_BROAD_RETRANS		NULL							struct GapReadNumBCRetranOutStru
IDX_WRITE_NUM_BROAD_RETRANS		struct GapWriteNumBCRetranStru	UCHAR status
IDX_READ_HOLD_MODE_ACT			NULL							struct GapReadHoldModeActOutStru
IDX_WRITE_HOLD_MODE_ACT			struct GapWriteHoldModeActStru	UCHAR status	
IDX_READ_TRANS_POWER_LEVEL		struct GapReadTransPowerStru	struct GapReadTransPowerOutStru
IDX_READ_SCO_FLOW_CTRL_EN		NULL							struct GapReadSCOFlowCtlOutStru
IDX_WRITE_SCO_FLOW_CTRL_EN		struct GapWriteSCOFlowCtlStru	UCHAR status
IDX_SET_HC_TO_HOST_FLOW_CTRL 	struct GapSetHCToHostFlowStru	UCHAR status
IDX_HOST_BUF_SIZE				struct GapHostBuffSizeStru		UCHAR status
IDX_HOST_NUM_COMP_PKTS			struct GapHostNumCompPacketStru	UCHAR status
IDX_READ_LINK_SUPER_TIMEOUT		WORD conn_hdl					struct GapReadSuperTimeOutStru
IDX_WRITE_LINK_SUPER_TIMEOUT	struct GapWriteSuperTimeStru	struct GapWriteSuperTimeOutStru
IDX_READ_NUM_SUP_IAC			NULL							struct GapReadIACOutStru
IDX_READ_CUR_IAC_LAP			NULL							struct GapReadCurrLAPOutStru
IDX_WRITE_CUR_IAC_LAP			struct GapWriteCurrLAPStru		UCHAR status
IDX_READ_PGSCAN_PERIOD_MODE		NULL							struct GapReadScanPeriodModeOutStru
IDX_WRITE_PGSCAN_PERIOD_MODE	struct GapWriteScanPerModeStru	UCHAR status
IDX_READ_PGSCAN_MODE			NULL							struct GapReadPageScanModeOutStru
IDX_WRITE_PGSCAN_MODE			struct GapWritePageScanModeStru	UCHAR status

**Informational Parameters***
IDX_READ_LOCAL_VER_INFO			NULL							struct GapReadLocalVerInfoOutStru
IDX_READ_LOCAL_SUP_FEATURES		NULL							struct GapReadLocalFeatureOutStru
IDX_READ_BUF_SIZE				NULL							struct GapReadBufSizeOutStru
IDX_READ_COUNTRY_CODE			NULL							struct GapReadCountryCodeOutStru
IDX_READ_BDADDR					NULL							struct GapReadBDAddrOutStru

**Status Parameters***
IDX_READ_FAILED_CONTACT_CNT		WORD conn_hdl					struct GapReadFailedCountOutStru
IDX_RESET_FAILED_CONTACT_CNT	WORD conn_hdl					struct GapResetFailedCountOutStru
IDX_GET_LINK_QUAILTY			WORD conn_hdl					struct GapGetLinkQualityOutStru
IDX_READ_RSSI					WORD conn_hdl					struct GapReadRSSIOutStru

***Testing Commands***
IDX_READ_LPBK_MODE				NULL							struct GapReadLBModeOutStru
IDX_WRITE_LPBK_MODE				struct GapWriteLBModeStru		UCHAR status
IDX_EN_DEV_UNDER_TEST_MODE		NULL							UCHAR status

***1.2 command begin***
IDX_CREATE_CONN_CANCEL				struct GapCreateConnCancelStru				struct GapCreateConnCancelOutStru
IDX_REM_NAME_REQ_CANCEL				struct GapRemNameReqCancelStru				struct GapRemNameReqCancelOutStru
IDX_READ_REM_EX_FEATURES			struct GapReadRemExFeaturesStru				struct GapReadRemExFeaturesCompEvStru

IDX_READ_LMP_HANDLE					WORD conn_hdl								struct GapReadLMPHdlOutStru
// create synchronous connetiion 
IDX_SETUP_SYNC_CONN					struct GapSetupSyncConnStru					struct GapSyncConnCompEvStru
// modify parameter of an existing synchronous connection 
IDX_SETUP_SYNC_CONN					struct GapSetupSyncConnStru					struct GapSyncConnChangeEvStru
IDX_ACC_SYNC_CONN_REQ				struct GapAccSyncConnReqStru				struct GapSyncConnCompEvStru
IDX_REJ_SYNC_CONN_REQ				struct GapRejSyncConnReqStru				struct GapSyncConnCompEvStru
IDX_READ_DEF_LINK_POLICY_SETTING	NULL										GapReadDefLinkPolicySettingOutStru
IDX_WRITE_DEF_LINK_POLICY_SETTING	struct GapWriteDefLinkPolicySettingStru		UCHAR status
IDX_FLOW_SPEC						struct GapFlowSpecStru						struct GapFlowSpecCompEvStru
IDX_SET_AFH_HOST_CHNL_CLS			struct GapSetAfhHostChnlClsStru				UCHAR status
IDX_READ_INQ_SCAN_TYPE				NULL										GapReadInqScanTypeOutStru
IDX_WRITE_INQ_SCAN_TYPE				struct GapWriteInqScanTypeStru				UCHAR status
IDX_READ_INQ_MODE					NULL										struct GapReadInqModeOutStru
IDX_WRITE_INQ_MODE					struct GapWriteInqModeStru					UCHAR status
IDX_READ_PGSCAN_TYPE				NULL										struct GapReadPgScanTypeOutStru
IDX_WRITE_PGSCAN_TYPE				struct GapWritePgScanTypeStru				UCHAR status
IDX_READ_AFH_CHNL_ASSESS_MODE		NULL										struct GapReadAfhChnlAssessModeOutStru
IDX_WRITE_AFH_CHNL_ASSESS_MODE		struct GapWriteAfhChnlAssessModeStru		UCHAR status
IDX_READ_LOCAL_SUP_COMMANDS			NULL										struct GapReadLocalSupCmdOutStru
IDX_READ_LOCAL_EX_FEATURES			struct GapReadLocalExFeaturesStru			struct GapReadLocalExFeaturesOutStru
IDX_READ_AFH_CHNL_MAP				WORD conn_hdl								struct GapReadAfhChnlMapOutStru
IDX_READ_CLOCK						struct GapReadClockStru						struct GapReadClockOutStru

***vendor command***
IDX_VENDOR_CMD						struct GapVendorStru						UCHAR status
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
index								input										output
&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&*/

#include "sm.h"

/******************************************************************************
 *																			  *
 *								MACRO DEFINATION					 		  *
 *																			  *
 ******************************************************************************/
/*index of Gap commands begins*/
/**********Link control commands************/
#define IDX_INQ								0
#define IDX_INQ_CANCEL						1
#define IDX_PERIODIC_INQ_MODE				2
#define IDX_EXIT_PERIODIC_INQ_MODE			3
#define IDX_CREATE_CONN						4
#define IDX_DISC							5
#define IDX_ADD_SCO_CONN					6
#define IDX_ACC_CONN_REQ					7
#define IDX_REJ_CONN_REQ					8
#define IDX_LINK_KEY_REQ_REPLY				9
#define IDX_LINK_KEY_REQ_NEG_REPLY			10
#define IDX_PIN_CODE_REQ_REPLY				11
#define IDX_PIN_CODE_REQ_NEG_REPLY			12
#define IDX_CHANGE_CONN_PKT_TYPE			13
#define IDX_AUTH_REQ						14
#define IDX_SET_CONN_ENCRY					15
#define IDX_CHANGE_CONN_LINK_KEY			16
#define IDX_MASTER_LINKKEY					17
#define IDX_REM_NAME_REQ					18
#define IDX_READ_REM_SUP_FEATURES			19
#define IDX_READ_REM_VER_INFO				20
#define IDX_READ_CLK_OFF					21
/************Link policy commands************/
#define IDX_HOLD_MODE						22
#define IDX_SNIFF_MODE						23
#define IDX_EXIT_SNIFF_MODE					24
#define IDX_PARK_MODE						25
#define IDX_EXIT_PARK_MODE					26
#define IDX_QOS_SETUP						27
#define IDX_ROLE_DCV						28
#define IDX_SW_ROLE							29
#define IDX_READ_LINK_POLICY				30
#define IDX_WRITE_LINK_POLICY				31
/*******Link control & baseband commands*****/
#define IDX_SET_EV_MASK						32
#define IDX_RESET							33
#define IDX_SET_EV_FILTER					34
#define IDX_FLUSH							35
#define IDX_READ_PIN_TYPE					36
#define IDX_WRITE_PIN_TYPE					37
#define IDX_CREATE_NEW_UNIT_KEY				38
#define IDX_READ_STORED_LINK_KEY			39
#define IDX_WRITE_STORED_LINK_KEY			40
#define IDX_DEL_STORED_LINK_KEY				41
#define IDX_CHANGE_LOCAL_NAME				42
#define IDX_READ_LOCAL_NAME					43
#define IDX_READ_CONN_ACC_TIMEOUT			44
#define IDX_WRITE_CONN_ACC_TIMEOUT			45
#define IDX_READ_PAGE_TIMEOUT				46
#define IDX_WRITE_PAGE_TIMEOUT				47
#define IDX_READ_SCAN_EN					48
#define IDX_WRITE_SCAN_EN					49
#define IDX_READ_PGSCAN_ACT					50
#define IDX_WRITE_PGSCAN_ACT				51
#define IDX_READ_INQ_SCAN_ACT				52
#define IDX_WRITE_INQ_SCAN_ACT				53
#define IDX_READ_AUTH_EN					54
#define IDX_WRITE_AUTH_EN					55
#define IDX_READ_ENCRY_MODE					56
#define IDX_WRITE_ENCRY_MODE				57
#define IDX_READ_DEV_CLASS					58
#define IDX_WRITE_DEV_CLASS					59
#define IDX_READ_VOICE_SETTING				60
#define IDX_WRITE_VOICE_SETTING				61
#define IDX_READ_AUTO_FLUSH_TIMEOUT			62
#define IDX_WRITE_AUTO_FLUSH_TIMEOUT		63
#define IDX_READ_NUM_BROAD_RETRANS			64
#define IDX_WRITE_NUM_BROAD_RETRANS			65
#define IDX_READ_HOLD_MODE_ACT				66
#define IDX_WRITE_HOLD_MODE_ACT				67
#define IDX_READ_TRANS_POWER_LEVEL			68
#define IDX_READ_SCO_FLOW_CTRL_EN			69
#define IDX_WRITE_SCO_FLOW_CTRL_EN			70
#define IDX_SET_HC_TO_HOST_FLOW_CTRL 		71
#define IDX_HOST_BUF_SIZE					72
#define IDX_HOST_NUM_COMP_PKTS				73
#define IDX_READ_LINK_SUPER_TIMEOUT			74
#define IDX_WRITE_LINK_SUPER_TIMEOUT		75
#define IDX_READ_NUM_SUP_IAC				76
#define IDX_READ_CUR_IAC_LAP				77
#define IDX_WRITE_CUR_IAC_LAP				78
#define IDX_READ_PGSCAN_PERIOD_MODE			79
#define IDX_WRITE_PGSCAN_PERIOD_MODE		80
#define IDX_READ_PGSCAN_MODE				81
#define IDX_WRITE_PGSCAN_MODE				82
/*********Informational Parameters***********/
#define IDX_READ_LOCAL_VER_INFO				83
#define IDX_READ_LOCAL_SUP_FEATURES			84
#define IDX_READ_BUF_SIZE					85
#define IDX_READ_COUNTRY_CODE				86
#define IDX_READ_BDADDR						87
/************Status Parameters***************/
#define IDX_READ_FAILED_CONTACT_CNT			88
#define IDX_RESET_FAILED_CONTACT_CNT		89
#define IDX_GET_LINK_QUAILTY				90
#define IDX_READ_RSSI						91
/************Testing Commands****************/
#define IDX_READ_LPBK_MODE					92
#define IDX_WRITE_LPBK_MODE					93
#define IDX_EN_DEV_UNDER_TEST_MODE			94
#ifdef CONFIG_HCI_12
/************HCI 1.2 Commands****************/
#define IDX_READ_REM_EX_FEATURES			95
#define IDX_READ_LMP_HANDLE					96
#define IDX_SETUP_SYNC_CONN					97
#define IDX_READ_DEF_LINK_POLICY_SETTING	98
#define IDX_WRITE_DEF_LINK_POLICY_SETTING	99
#define IDX_FLOW_SPEC						100
#define IDX_SET_AFH_HOST_CHNL_CLS			101
#define IDX_READ_INQ_SCAN_TYPE				102
#define IDX_WRITE_INQ_SCAN_TYPE				103
#define IDX_READ_INQ_MODE					104
#define IDX_WRITE_INQ_MODE					105
#define IDX_READ_PGSCAN_TYPE				106
#define IDX_WRITE_PGSCAN_TYPE				107
#define IDX_READ_AFH_CHNL_ASSESS_MODE		108
#define IDX_WRITE_AFH_CHNL_ASSESS_MODE		109
#define IDX_READ_LOCAL_EX_FEATURES			110
#define IDX_READ_AFH_CHNL_MAP				111
#define IDX_READ_CLOCK						112
#define IDX_ACC_SYNC_CONN_REQ				113
#define IDX_REJ_SYNC_CONN_REQ				114
#define IDX_CREATE_CONN_CANCEL				115
#define IDX_REM_NAME_REQ_CANCEL				116
#define IDX_READ_LOCAL_SUP_COMMANDS			117
/************Vendor Commands****************/
#define IDX_VENDOR_CMD						118
#else
/************Vendor Commands****************/
#define IDX_VENDOR_CMD						95
#endif


#define BD_ADDR_LEN				6			/* bluetooth address length */
#define CLS_DEV_LEN				3			/* class of device length */
#define LMP_FEATURE_LEN			8			/* length of LMP features */
#define MAX_PIN_CODE_LEN		16			/* maxinimum length of PIN code */
#define MAX_LINK_KEY_LEN		16			/* maxinimum length of Link key */

#define ROLE_MASTER			        0x0		/* local device must be the master of the connection */		
#define ROLE_SLAVE			        0x1		/* local device must be the slave of the connection */
#define ROLE_MASTER_SLAVE	        0x2		/* local device's role is free, which is the default value */

#define NO_SCAN_ENABLE				0x0		/* no scans enabled */
#define INQUIRY_SCAN_ENABLE			0x1		/* inquiry scan enabled */
#define PAGE_SCAN_ENABLE			0x2		/* page scan enabled */
#define INQUIRY_PAGE_SCAN_ENABLE	0x3		/* inquiry and page scan enable */


/* filter type in command Set_Event_Filter */
#define FT_CLR_ALL_FLT				0x0		/* clear all filters */
#define FT_INQ_RSLT					0x1		/* inquiry result */
#define FT_CONN_SETUP				0x2		/* connection setup */

/* filter condition type for inquiry */
#define FCT_INQ_NEW_DEV					0x0		/* inquiry new device */
#define FCT_INQ_CLS_DEV					0x1		/* a specific class of device */
#define FCT_INQ_BD_ADDR					0X2		/* a specific bd_addr */

/* filter condition type for inquiry */
#define FCT_CONN_ALL_DEV					0x0		/* allow connection from all devices */
#define FCT_CONN_CLS_DEV					0x1		/* a specific class of device */
#define FCT_CONN_BD_ADDR					0X2		/* a specific bd_addr */

#define FCT_NOT_AUTO_ACCEPT			0x1		/*not auto-accept the connection */
#define FCT_AUTO_ACCEPT				0x2		/* auto-accept the connection */

#define CMD_LEN_EV_FILTER_CLR_ALL	0x1	/* The parameter length of set event filter with filter_type==0x00*/
#define CMD_LEN_EV_FILTER_INQ_NEW	0x2	/* The parameter length of set event filter with filter_type==0x01 & cond_type==0x00*/
#define CMD_LEN_EV_FILTER_INQ_CLS	0x8	/* The parameter length of set event filter with filter_type==0x01 & cond_type==0x01*/
#define CMD_LEN_EV_FILTER_INQ_ADDR	0x8	/* The parameter length of set event filter with filter_type==0x01 & cond_type==0x02*/
#define CMD_LEN_EV_FILTER_CONN_ALL	0x2	/* The parameter length of set event filter with filter_type==0x02 & cond_type==0x00*/
#define CMD_LEN_EV_FILTER_CONN_CLS	0x8	/* The parameter length of set event filter with filter_type==0x02 & cond_type==0x01*/
#define CMD_LEN_EV_FILTER_CONN_ADDR	0x8	/* The parameter length of set event filter with filter_type==0x02 & cond_type==0x02*/

#define EV_MASK_LEN			8
#define MAX_KEYS_NUM		10
#define MAX_CONN_COMP		10
#define NUM_IAC_LAP			10
#define IAC_LAP_LEN			3
#define CONN_HDL_LEN		2
#define MAX_IACLAP_NUM		10

#define INQUIRY_GENERAL				0x00000000
#define INQUIRY_LIMITED				0x00000001
#define NAME_REQUIRED				0x00000002
#define COD_SRVCLS					0xFFE000
#define COD_DEVCLS					0x001F00
#define COD_MINDEVCLS				(COD_DEVCLS | 0x0000FC)	

/*major service classes*/
#define SRVCLS_LDM					0x002000
#define SRVCLS_POSITION				0x010000
#define SRVCLS_NETWORK				0x020000
#define SRVCLS_RENDER				0x040000
#define SRVCLS_CAPTURE				0x080000
#define SRVCLS_OBJECT				0x100000
#define SRVCLS_AUDIO				0x200000
#define SRVCLS_TELEPHONE			0x400000
#define SRVCLS_INFOR				0x800000
#define SRVCLS_MASK(a)				(((DWORD)(a) >> 13) & 0x7FF)

/*major device classes*/			                                    
#define DEVCLS_MISC					0x000000
#define DEVCLS_COMPUTER				0x000100
#define DEVCLS_PHONE				0x000200
//#define DEVCLS_PHONE				0x001000
#define DEVCLS_LAP					0x000300
#define DEVCLS_AUDIO				0x000400
#define DEVCLS_PERIPHERAL			0x000500
#define DEVCLS_IMAGE              	0x000600
#define DEVCLS_UNCLASSIFIED			0x001F00
#define DEVCLS_MASK(a)				(((DWORD)(a) >> 8) & 0x1F)
#define MINDEVCLS_MASK(a)			(((DWORD)(a) >> 2) & 0x3F)

/*the minor device class field - computer major class */
#define COMPCLS_UNCLASSIFIED     	(DEVCLS_COMPUTER | 0x000000) 
#define COMPCLS_DESKTOP          	(DEVCLS_COMPUTER | 0x000004)
#define COMPCLS_SERVER            	(DEVCLS_COMPUTER | 0x000008)
#define COMPCLS_LAPTOP            	(DEVCLS_COMPUTER | 0x00000C)
#define COMPCLS_HANDHELD        	(DEVCLS_COMPUTER | 0x000010)
#define COMPCLS_PALMSIZED        	(DEVCLS_COMPUTER | 0x000014)
#define COMPCLS_WEARABLE        	(DEVCLS_COMPUTER | 0x000018)

/*the minor device class field - phone major class*/
#define PHONECLS_UNCLASSIFIED   	(DEVCLS_PHONE | 0x000000) 
#define PHONECLS_CELLULAR         	(DEVCLS_PHONE | 0x000004)
#define PHONECLS_CORDLESS        	(DEVCLS_PHONE | 0x000008)
#define PHONECLS_SMARTPHONE     	(DEVCLS_PHONE | 0x00000C)
#define PHONECLS_WIREDMODEM    		(DEVCLS_PHONE | 0x000010)
#define PHONECLS_COMMONISDNACCESS	(DEVCLS_PHONE | 0x000014)
#define PHONECLS_SIMCARDREADER		(DEVCLS_PHONE | 0x000018)

/*the minor device class field - LAN/Network access point major class*/
#define LAP_Fully                  	(DEVCLS_LAP | 0x000000)
#define LAP_17                     	(DEVCLS_LAP | 0x000002)
#define LAP_33                     	(DEVCLS_LAP | 0x000004)
#define LAP_50                     	(DEVCLS_LAP | 0x000006)
#define LAP_67                     	(DEVCLS_LAP | 0x000008)
#define LAP_83                     	(DEVCLS_LAP | 0x00000A)
#define LAP_99                     	(DEVCLS_LAP | 0x00000C)
#define LAP_NOSRV                 	(DEVCLS_LAP | 0x00000D)

/*the minor device class field - Audio/Video major class*/
#define AV_UNCLASSIFIED           	(DEVCLS_AUDIO | 0x000000)
#define AV_HEADSET                	(DEVCLS_AUDIO | 0x000004)
#define AV_HANDSFREE             	(DEVCLS_AUDIO | 0x000008)
#define AV_HEADANDHAND           	(DEVCLS_AUDIO | 0x00000C)
#define AV_MICROPHONE            	(DEVCLS_AUDIO | 0x000010) 
#define AV_LOUDSPEAKER           	(DEVCLS_AUDIO | 0x000014)
#define AV_HEADPHONES            	(DEVCLS_AUDIO | 0x000018)
#define AV_PORTABLEAUDIO         	(DEVCLS_AUDIO | 0x00001C)
#define AV_CARAUDIO               	(DEVCLS_AUDIO | 0x000020)
#define AV_SETTOPBOX              	(DEVCLS_AUDIO | 0x000024)
#define AV_HIFIAUDIO               	(DEVCLS_AUDIO | 0x000028)
#define AV_VCR                     	(DEVCLS_AUDIO | 0x00002C)
#define AV_VIDEOCAMERA           	(DEVCLS_AUDIO | 0x000030)
#define AV_CAMCORDER             	(DEVCLS_AUDIO | 0x000034)
#define AV_VIDEOMONITOR          	(DEVCLS_AUDIO | 0x000038)
#define AV_VIDEODISPANDLOUDSPK   	(DEVCLS_AUDIO | 0x00003C) 
#define AV_VIDEOCONFERENCE       	(DEVCLS_AUDIO | 0x000040)
#define AV_GAMEORTOY             	(DEVCLS_AUDIO | 0x000048)

/*the minor device class field - peripheral major class*/
#define PERIPHERAL_KEYBOARD      	(DEVCLS_PERIPHERAL | 0x000040)    
#define PERIPHERAL_POINT           	(DEVCLS_PERIPHERAL | 0x000080)
#define PERIPHERAL_KEYORPOINT    	(DEVCLS_PERIPHERAL | 0x0000C0)
#define PERIPHERAL_UNCLASSIFIED  	(DEVCLS_PERIPHERAL | 0x000000) 
#define PERIPHERAL_JOYSTICK       	(DEVCLS_PERIPHERAL | 0x000004)
#define PERIPHERAL_GAMEPAD       	(DEVCLS_PERIPHERAL | 0x000008)
#define PERIPHERAL_REMCONTROL    	(DEVCLS_PERIPHERAL | 0x00000C)
#define PERIPHERAL_SENSE          	(DEVCLS_PERIPHERAL | 0x000010)

/*the minor device class field - imaging major class*/
#define IMAGE_DISPLAY             	(DEVCLS_IMAGE | 0x000010)
#define IMAGE_CAMERA             	(DEVCLS_IMAGE | 0x000020)
#define IMAGE_SCANNER            	(DEVCLS_IMAGE | 0x000040)
#define IMAGE_PRINTER              	(DEVCLS_IMAGE | 0x000080)

/*Used in the functions GAP_GetLocalDevInfo and GAP_SetLocalDevInfo*/
#define LOC_NAME_AVAILABLE			     0x00000001
#define LOC_ADDR_AVAILABLE			     0x00000002
#define LOC_CLS_AVAILABLE		         0x00000004
#define LOC_ROLE                         0x00000008
#define LOC_SEC_MODE					 0x00000010
#define LOC_VISIBLE_MODE                 0x00000020
#define LOC_DEV_MASKALL				     0x0000003f
#define LOC_DEV_DEFAULT_SETMASK     (LOC_ROLE|LOC_SEC_MODE|LOC_VISIBLE_MODE)
#define LOC_DEV_DEFAULT_GETMASK     (LOC_NAME_AVAILABLE|LOC_ADDR_AVAILABLE|LOC_CLS_AVAILABLE)

#define SM_MODE_1		            0x1			
#define SM_MODE_2		            0x2			
#define SM_MODE_3		            0x3	

#define VM_DISABLE			        0x0000 
#define VM_LIMIT_DISCOV		        0x0002		
#define VM_GENERAL_DISCOV       	0x0003		
#define VM_PAGE				        0x0004		
#define VM_PAIR				        0x0008		
#define VM_ENCRYPT			        0x0010		
#define VM_DISCOV			        VM_GENERAL_DISCOV 
#define VM_DEFAULT			        (VM_GENERAL_DISCOV | VM_PAGE | VM_PAIR) 

/*Link key management mode*/
#define LK_APP_SAVE					0x0
#define LK_HCI_SAVE					0x1

/* used for hci 1.1 add sco connection */
#define SCO_PKT_HV1			 0x20
#define SCO_PKT_HV2			 0x40
#define SCO_PKT_HV3			 0x80
#define SCO_PKT_DV			 0x100

/*Used in the functions GAP_EnterLinkMode and GAP_ExitLinkMode */
#define ACTIVE_MODE				0
#define HOLD_MODE				1
#define SNIFF_MODE				2
#define PARK_MODE				3

#ifdef CONFIG_HCI_12
/*Inquiry Mode*/
#define INQ_RESULT_STD			0
#define INQ_RESULT_RSSI			1

/*AFH Channel Assessment Mode*/
#define CHNL_ASSESS_DIS			0
#define CHNL_ASSESS_EN			1

/*Inquiry Scan Type*/
#define INQ_SCAN_STD			0
#define INQ_SCAN_INTERLACED		1

/*Page Scan Type*/
#define PAGE_SCAN_STD			0
#define PAGE_SCAN_INTERLACED	1

/*Read Clock*/
#define LOCAL_CLOCK				0
#define PICONET_CLOCK			1

/*Setup Synchronous Connection, Accept Synchronous Connection*/
#define RETRANS_EFFORT_ONE_POWER	0x01		/* At least one retransmission, optimize for power consumption */
#define RETRANS_EFFORT_ONE_LINK		0x02		/* At least one retransmission, optimize for link quality */
#define RETRANS_EFFORT_NOT_CARE		0xFF		/* don't care */
#define MAX_LATENCY_NOT_CARE		0xFFFF		/* don't care */
#define SYNC_PKT_HV1			0x01
#define SYNC_PKT_HV2			0x02
#define SYNC_PKT_HV3			0x04
#define SYNC_PKT_EV3			0x08
#define SYNC_PKT_EV4			0x10
#define SYNC_PKT_EV5			0x20
#endif

#define EV_INQ_RESULT_IND				0x20
#ifdef CONFIG_HCI_12
typedef void(* GAP_InqResultFunc)(UCHAR *bd_addr, UCHAR *dev_class, UCHAR *rssi);
#else
typedef void(* GAP_InqResultFunc)(UCHAR *bd_addr, UCHAR *dev_class);
#endif
#define EV_AUTHORIZATION_IND			0x24
typedef void(* GAP_AuthorIndFunc)(UCHAR *bd_addr, WORD channel);

#define EV_PIN_CODE_REQ_IND				0x28
typedef void(* GAP_PinCodeReqFunc)(UCHAR *bd_addr, WORD delay); 

#define EV_LINK_KEY_REQ_IND				0x29
typedef void(* GAP_LinkKeyReqFunc)(UCHAR *bd_addr);

#define EV_LINK_KEY_NOTIF_IND			0x2A
typedef void(* GAP_LinkKeyNotifFunc)(UCHAR *bd_addr, UCHAR *link_key);

#define EV_ERROR_IND					0x2B
typedef void(* GAP_ErrorFunc)(UCHAR code);

#define EV_AUTHEN_FAILED_IND			0x2D
typedef void(* GAP_AuthFailFunc)(UCHAR *bd_addr);

#define EV_CLK_OFFSET_CFM				0x31
typedef void(* GAP_ClockOffsetCfm)(UCHAR *bd_addr, WORD clk_offset, UCHAR result);

#define EV_LINK_LOSS_IND				0x32
typedef void(* GAP_LinkLossFunc)(UCHAR *bd_addr);

#define EV_LOOPBACK_IND					0x25
typedef void(* GAP_LoopBackFunc)(UCHAR *buff, UCHAR len);

#ifdef CONFIG_HCI_12
#define EV_SYNC_CONN_CFM				0x26
typedef void (GAP_SyncConnCfm)(UCHAR *bd_addr, WORD hdl, UCHAR link_type, UCHAR result);

#define EV_SYNC_DISC_IND				0x27
typedef void (GAP_SyncDiscInd)(WORD conn_hdl);

#define EV_SYNC_CONN_CHANGE_IND			0x35
typedef void (GAP_SyncConnChangeInd)(WORD conn_hdl);

#endif

/*add by xufei for jepico*/
#define EV_INQ_RESULT_IND_EX			0x3A
typedef void(* GAP_InqResultFuncEx)(UCHAR *bd_addr, UCHAR *dev_class, UCHAR rssi, WORD clk_offset,UCHAR pg_scan_period_mode,UCHAR pg_scan_rep_mode);


//added by lhb
#define EV_SCO_DATA_IND					0x40
typedef void (* GAP_ScoDataInd)(WORD conn_hdl, WORD len, UCHAR *packet);

#define EV_CONN_REQ_IND				0x2E
typedef UCHAR (*GAP_ConnReqInd)(UCHAR *bd_addr, UCHAR *dev_class, UCHAR link_type);

#define EV_CONN_COMP_CFM			0x37
typedef UCHAR (* GAP_ConnCompCfm)(UCHAR *bd_addr);

/******************************************************************************
 *																   	     	  *
 *								STRUCT DEFINATION							  *
 *																			  *
 ******************************************************************************/
struct GapInquiryStru {
	UCHAR lap[3];
	UCHAR inq_len;
    UCHAR num_rsp;
};

struct GapPeriodicInqModeStru{
	WORD max_len;						/* maximum period length */
	WORD min_len;						/* minimum period length */
	char lap[3];						/* LAP */
	UCHAR inq_len;						/* inquiry length */
    UCHAR num_rsp;						/* number of responses */
};

struct GapCreateConnStru {
	UCHAR bd_addr[BD_ADDR_LEN];
    WORD pkt_type;
    UCHAR pgscan_rep_mode;
    UCHAR pgscan_mode;
    WORD clk_off;
    UCHAR role_sw; 
};

struct GapConnCompEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR link_type;
	UCHAR encry_mode;
};

struct GapDisconnStru {
	WORD conn_hdl;
	UCHAR reason;
};

struct GapDisconnCompEvStru {
	UCHAR status;
    WORD conn_hdl;
	UCHAR reason;
};

struct GapAddScoConnStru {
	WORD conn_hdl;
	WORD pkt_type;
};

struct GapAcceptConnReqStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR role;
};

struct GapRejectConnReqStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR reason;
};

struct GapLinkKeyStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR link_key[MAX_LINK_KEY_LEN];
};

struct GapLinkKeyNegStru {
	UCHAR bd_addr[BD_ADDR_LEN];
};

struct GapLinkKeyOutStru {
	UCHAR status;
	UCHAR bd_addr[BD_ADDR_LEN];
};

struct GapPinCodeStru {
	UCHAR bd_addr[BD_ADDR_LEN];
    UCHAR pin_len;
	UCHAR pin[MAX_PIN_CODE_LEN];
};

struct GapPinCodeNegStru {
	UCHAR bd_addr[BD_ADDR_LEN];
};

struct GapPinCodeOutStru {
	UCHAR status;
	UCHAR bd_addr[BD_ADDR_LEN];
};

struct GapChangePacketTypeStru {
	WORD conn_hdl;
	WORD pkt_type;
};

struct GapPacketChangeEvStru {
	UCHAR status;
	WORD conn_hdl;
	WORD pkt_type;
};

struct GapAuthCompEvStru {
	UCHAR status;
	WORD  conn_hdl;
};

struct GapSetConnEncryStru {
	WORD conn_hdl;
	UCHAR encry;
};

struct GapEncryChangeEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR encry;
};

struct GapChangeLinkKeyEvStru {
	UCHAR status;
	WORD conn_hdl;
};

struct GapMasterLinkKeyStru {
	UCHAR key_flag;
};

struct GapMasterLinkKeyEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR key_flag;
};

struct GapRemNameReqStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR scan_rep_mode;
    UCHAR scan_mode;
	WORD clk_off;
};

struct GapRemNameReqCompEvStru {
    UCHAR status;
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR rem_name[MAX_NAME_LEN];
};

struct GapReadRemFeatureCompEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR feature[LMP_FEATURE_LEN];
};

struct GapReadRemVerInfoCompEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR lmp_ver;
	WORD manu_name;
	WORD lmp_subver;
};

struct GapReadClkOffsetCompEvStru {
	UCHAR status;
	WORD conn_hdl;
	WORD offset;
};

struct GapHoldModeStru {
	WORD conn_hdl;
	WORD max;
	WORD min;
};

struct GapSniffModeStru {
	WORD conn_hdl;
	WORD max;
	WORD min;
	WORD attempt;
	WORD timeout;
};

struct GapParkModeStru {
	WORD conn_hdl;
	WORD max;
	WORD min;
};

struct GapModeChangeEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR current_mode;
	WORD interval;
};

struct GapQosSetupStru {
	WORD conn_hdl;
	UCHAR flags;
	UCHAR service_type;
	DWORD token_rate;
	DWORD peak_bandwidth;
	DWORD latency;
	DWORD delay_variation;
};

struct GapQosSetupCompEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR flags;
	UCHAR service_type;
	DWORD token_rate;
	DWORD peak_bandwidth;
	DWORD latency;
	DWORD delay_variation;
};

struct GapRoleDiscoveryOutStru {
	UCHAR status;
	WORD conn_hdl;					/* acl connection handle */
	UCHAR current_role;				/* local device role for this acl connection: ROLE_MASTER or ROLE_SLAVE*/
};

struct GapSwitchRoleStru {
	UCHAR bd_addr[BD_ADDR_LEN];		/* remote device baseband address */
	UCHAR role;						/* ROLE_SLAVE or ROLE_MASTER that local device wants to be */
};

struct GapRoleChangeEvStru {
	UCHAR status;
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR role;
};

struct GapReadLPSettingOutStru {
	UCHAR status;
	WORD conn_hdl;
	WORD setting;
};

struct GapWriteLPSettingStru {
	WORD conn_hdl;
	WORD setting;
};

struct GapWriteLPSettingOutStru {
	UCHAR status;
	WORD conn_hdl;
};

struct GapSetEvMaskStru {
	UCHAR mask[EV_MASK_LEN];
};

struct GapSetEvFilterStru{
	UCHAR type;						
	UCHAR cond_type;				
	UCHAR cls_dev[CLS_DEV_LEN];		
	UCHAR cls_dev_mask[CLS_DEV_LEN];	
	UCHAR bd_addr[BD_ADDR_LEN];		
	UCHAR auto_accept;				
};

struct GapFlushOutStru {
	UCHAR status;
	WORD conn_hdl;
};

struct GapReadPinTypeOutStru {
	UCHAR status;
	UCHAR pin_type;
};

struct GapWritePinTypeStru {
	UCHAR pin_type;
};

struct GapReadStoredKeyStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR flag;
};

struct GapReadStoredKeyOutStru {
	UCHAR status;
	WORD max_num_keys;
	WORD num_keys_read;
};

struct StoredKeyStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR link_key[MAX_LINK_KEY_LEN];
};

struct GapWriteStoredKeyStru {
	DWORD num;
	struct StoredKeyStru keys[MAX_KEYS_NUM];
};

struct GapWriteStoredKeyOutStru {
	UCHAR status;
	UCHAR num_written;
};

struct GapDelStoredKeyStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR flag;
};

struct GapDelStoredKeyOutStru {
	UCHAR status;
	WORD num;
};

struct GapChangeNameStru {
	UCHAR name[MAX_NAME_LEN];
};

struct GapReadNameOutStru {
	UCHAR status;
	UCHAR name[MAX_NAME_LEN];
};

struct GapReadConnTimeOutStru {
	UCHAR status;
	WORD timeout;
};

struct GapWriteConnTimeStru {
	WORD timeout;
};

struct GapReadPageTimeOutStru {
	UCHAR status;
	WORD timeout;
};

struct GapWritePageTimeStru {
	WORD timeout;
};

struct GapReadScanEnOutStru {
	UCHAR status;
	UCHAR enable;
};

struct GapWriteScanEnStru {
	UCHAR enable;
};

struct GapReadPageScanActOutStru {
	UCHAR status;
	WORD interval;
	WORD windows;
};

struct GapWritePageScanActStru {
	WORD interval;
	WORD window;
};

struct GapReadInqScanActOutStru {
	UCHAR status;
	WORD interval;
	WORD window;
};

struct GapWriteInqScanActStru {
	WORD interval;
	WORD window;
};

struct GapReadAuthEnOutStru {
	UCHAR status;
	UCHAR enable;
};

struct GapWriteAuthEnStru {
	UCHAR enable;
};

struct GapReadEncryModeOutStru {
	UCHAR status;
	UCHAR mode;
};

struct GapWriteEncryModeStru {
	UCHAR mode;
};

struct GapReadClassOfDevOutStru {
	UCHAR status;
	UCHAR dev_class[CLS_DEV_LEN];
};

struct GapWriteClassOfDevStru {
	UCHAR dev_class[CLS_DEV_LEN];
};

struct GapReadVoiceSettingOutStru {
	UCHAR status;
	WORD setting;
};

struct GapWriteVoiceSettingStru {
	WORD setting;
};

struct GapReadAutoFlushOutStru {
	UCHAR status;
	WORD conn_hdl;
	WORD timeout;
};

struct GapWriteAutoFlushStru {
	WORD conn_hdl;
	WORD timeout;
};

struct GapWriteAutoFlushOutStru {
	UCHAR status;
	WORD conn_hdl;
};

struct GapReadNumBCRetranOutStru {
	UCHAR status;
	UCHAR num;
};

struct GapWriteNumBCRetranStru {
	UCHAR num;
};

struct GapReadHoldModeActOutStru {
	UCHAR status;
	UCHAR activity;
};

struct GapWriteHoldModeActStru {
	UCHAR activity;
};

struct GapReadTransPowerStru {
	WORD conn_hdl;
	UCHAR type;
};

struct GapReadTransPowerOutStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR level;
};

struct GapReadSCOFlowCtlOutStru {
	UCHAR status;
	UCHAR enable;
};

struct GapWriteSCOFlowCtlStru {
	UCHAR enable;
};

struct GapSetHCToHostFlowStru {
	UCHAR fc;
};

struct GapHostBuffSizeStru {
	WORD acl_len;
	UCHAR sco_len;
	WORD acl_num;
	WORD sco_num;
};

struct ConnCompStru {
	WORD conn_hdl;
	WORD num;
};

struct GapHostNumCompPacketStru {
	DWORD conn_num;
	struct  ConnCompStru conn_comp[MAX_CONN_COMP];
};

struct GapReadSuperTimeOutStru {
	UCHAR status;
	WORD conn_hdl;
	WORD timeout;
};

struct GapWriteSuperTimeStru {
	WORD conn_hdl;
	WORD timeout;
};

struct GapWriteSuperTimeOutStru {
	UCHAR status;
	WORD conn_hdl;
};

struct GapReadIACOutStru {
	UCHAR status;
	UCHAR num;
};

struct IacLapStru {
	UCHAR lap[IAC_LAP_LEN];
};

struct GapReadCurrLAPOutStru {
	UCHAR status;
	UCHAR num;
	struct IacLapStru lap[NUM_IAC_LAP];
};

struct GapWriteCurrLAPStru {
	DWORD num;
	struct IacLapStru lap[NUM_IAC_LAP];
};

struct GapReadScanPeriodModeOutStru {
	UCHAR status;
	UCHAR mode;
};

struct GapWriteScanPerModeStru {
	UCHAR mode;
};

struct GapReadPageScanModeOutStru {
	UCHAR status;
	UCHAR mode;
};

struct GapWritePageScanModeStru {
	UCHAR mode;
};

struct GapReadLocalVerInfoOutStru {
	UCHAR status;
	UCHAR hci_version;
	WORD hci_revision;
	UCHAR lmp_version;
	WORD manu_name;
	WORD lmp_subversion;
};

struct GapReadLocalFeatureOutStru {
	UCHAR status;
	UCHAR feature[LMP_FEATURE_LEN];
};

struct GapReadBufSizeOutStru {
	UCHAR status;
	WORD acl_len;
	UCHAR sco_len;
	WORD acl_num;
	WORD sco_num;
};

struct GapReadCountryCodeOutStru {
	UCHAR status;
	UCHAR country;
};

struct GapReadBDAddrOutStru {
	UCHAR status;
	UCHAR bd_addr[BD_ADDR_LEN];
};

struct GapReadFailedCountOutStru {
	UCHAR status;
	WORD conn_hdl;
	WORD counter;
};

struct GapResetFailedCountOutStru {
	UCHAR status;
	WORD conn_hdl;
};

struct GapGetLinkQualityOutStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR quality;
};

struct GapReadRSSIOutStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR rssi;
};

struct GapReadLBModeOutStru {
	UCHAR status;
	UCHAR mode;
};

struct GapWriteLBModeStru {
	UCHAR mode;
};

struct GapCmdInfoStru {
	WORD idx;
	UCHAR type;
	WORD ilen;
};

struct GapEnumDevInfoStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR dev_class[CLS_DEV_LEN];
	UCHAR name[MAX_NAME_LEN];	
};

struct GAP_LocDevInfoStru {
    DWORD mask;
    UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR name[MAX_NAME_LEN];
	UCHAR cls[CLS_DEV_LEN];
	UCHAR local_role;
	UCHAR sec_mode;
	WORD vis_mode;
};

struct GAP_EnumDevOldStru {
	WORD num;
	struct GAP_LocDevInfoStru *dev_buf;
};

#ifdef CONFIG_HCI_12
struct GapCreateConnCancelStru {
    UCHAR bd_addr[BD_ADDR_LEN];
};

struct GapCreateConnCancelOutStru {
	UCHAR status;
    UCHAR bd_addr[BD_ADDR_LEN];
};

struct GapRemNameReqCancelStru {
    UCHAR bd_addr[BD_ADDR_LEN];
};

struct GapRemNameReqCancelOutStru {
	UCHAR status;
    UCHAR bd_addr[BD_ADDR_LEN];
};

struct GapReadRemExFeaturesStru {
	WORD conn_hdl;
	UCHAR num_page;
};

struct GapReadRemExFeaturesCompEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR pg_num;
	UCHAR max_pg_num;
	UCHAR ex_lmp_fea[8];
};

struct GapReadLMPHdlOutStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR lmp_hdl;
	DWORD rsv;
};

struct GapSetupSyncConnStru {
	WORD conn_hdl;
	DWORD trans_bandwidth;
	DWORD rcv_bandwidth;
	WORD max_latency;
	WORD voice_setting;
	UCHAR retrans_effort;
	WORD packet_type;
};

struct GapAccSyncConnReqStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	DWORD trans_bandwidth;
	DWORD rcv_bandwidth;
	WORD max_latency;
	WORD content_format;
	UCHAR retrans_effort;
	WORD packet_type;
};

struct GapRejSyncConnReqStru {
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR reason;
};

struct GapSyncConnCompEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR bd_addr[BD_ADDR_LEN];
	UCHAR link_type;
	UCHAR trans_interval;
	UCHAR retrans_window;
	WORD rx_packet_len;
	WORD tx_packet_len;
	UCHAR air_mode;
};

struct GapSyncConnChangeEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR trans_interval;
	UCHAR retrans_window;
	WORD rx_packet_len;
	WORD tx_packet_len;
};

struct GapReadDefLinkPolicySettingOutStru {
	UCHAR status;
	WORD setting;
};

struct GapWriteDefLinkPolicySettingStru {
	WORD setting;
};

struct GapFlowSpecStru {
	WORD conn_hdl;
	UCHAR flags;
	UCHAR flow_direction;
	UCHAR srv_type;
	DWORD token_rate;
	DWORD token_bucket_size;
	DWORD peak_bandwidth;
	DWORD access_latency;
};

struct GapFlowSpecCompEvStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR flags;
	UCHAR flow_direction;
	UCHAR srv_type;
	DWORD token_rate;
	DWORD token_bucket_size;
	DWORD peak_bandwidth;
	DWORD access_latency;
};

struct GapSetAfhHostChnlClsStru {
	UCHAR afh_host_chnl_cls[10];
};

struct GapReadInqScanTypeOutStru {
	UCHAR status;
	UCHAR scan_type;
};

struct GapWriteInqScanTypeStru {
	UCHAR scan_type;
};

struct GapReadInqModeOutStru {
	UCHAR status;
	UCHAR inq_mode;
};

struct GapWriteInqModeStru {
	UCHAR inq_mode;
};

struct GapReadPgScanTypeOutStru {
	UCHAR status;
	UCHAR scan_type;
};

struct GapWritePgScanTypeStru {
	UCHAR scan_type;
};

struct GapReadAfhChnlAssessModeOutStru {
	UCHAR status;
	UCHAR afh_chnl_assess_mode;
};

struct GapWriteAfhChnlAssessModeStru {
	UCHAR afh_chnl_assess_mode;
};

struct GapReadLocalExFeaturesStru {
	UCHAR page_num;
};

struct GapReadLocalSupCmdOutStru {
	UCHAR status;
	UCHAR sup_cmd[64];
};

struct GapReadLocalExFeaturesOutStru {
	UCHAR status;
	UCHAR page_num;
	UCHAR max_page_num;
	UCHAR ex_lmp_features[8];
};

struct GapReadAfhChnlMapOutStru {
	UCHAR status;
	WORD conn_hdl;
	UCHAR afh_mode;
	UCHAR afh_chnl_map[10];
};

struct GapReadClockStru {
	UCHAR which_clock;
	WORD conn_hdl;
};

struct GapReadClockOutStru {
	UCHAR status;
	WORD conn_hdl;
	DWORD clock;
	WORD accuracy;
};

#endif

struct GapVendorStru {
	UCHAR ocf;
	UCHAR len;
	UCHAR param[1];
};

/********************************************************************************
 *																				*
 *								FUNCATION DECLARATION							*
 *																				*
 ********************************************************************************/

UCHAR GAP_ResetHardware(void);
UCHAR GAP_CreateScoConn(UCHAR *bd_addr, WORD pkt_type, WORD *conn_hdl);
#ifdef CONFIG_HCI_12
UCHAR GAP_SetupSyncConn(struct GapSetupSyncConnStru *in, struct GapSyncConnCompEvStru *out);
UCHAR GAP_AcceptSyncConn(struct GapAccSyncConnReqStru *in, struct GapSyncConnCompEvStru *out);
UCHAR GAP_RejectSyncConn(struct GapRejSyncConnReqStru *in, struct GapSyncConnCompEvStru *out);
#endif
UCHAR GAP_ReleaseConn(WORD conn_hdl);
#define GAP_ReleaseScoConn(hdl)		GAP_ReleaseConn(hdl)
UCHAR GAP_EnumDev(DWORD mode, DWORD cod, UCHAR duration, DWORD *size, struct GAP_LocDevInfoStru *dev_buf);
UCHAR GAP_EnumDev2(DWORD mode, UCHAR duration, UCHAR size);
UCHAR GAP_ExecuteCommand(WORD idx, UCHAR *inp, UCHAR *outp);
UCHAR GAP_RegIndCbk(WORD ev, UCHAR *cbk, UCHAR *param);
UCHAR GAP_UnRegIndCbk(WORD ev);
UCHAR GAP_UnRegIndCbk2(WORD ev, UCHAR *cbk, UCHAR *param);
UCHAR GAP_GetLocalDevInfo(struct GAP_LocDevInfoStru *dev_info);
UCHAR GAP_SetLocalDevInfo(struct GAP_LocDevInfoStru *dev_set);
UCHAR GapEnableDevice(WORD flag);
UCHAR GAP_PairRemoteDev(UCHAR *bd_addr);
UCHAR GAP_GetLocalRole(UCHAR *role);
UCHAR GAP_SetLocalRole(UCHAR role);
UCHAR GAP_ScoConnRsp(UCHAR *bd_addr, UCHAR resp, WORD *conn_hdl);
UCHAR GAP_ExitLinkMode(UCHAR *bd_addr, UCHAR mode);
UCHAR GAP_SetHostBuffSize(struct GapHostBuffSizeStru * bufsize);
UCHAR GAP_SetHc2HostFC(UCHAR fc);
UCHAR GAP_EnterLinkMode(UCHAR *bd_addr, UCHAR mode, UCHAR *param);
UCHAR GAP_VendorCommand(UCHAR *inp, UCHAR *outp);
UCHAR GAP_Done(void);

/*The following API is used to read/write info that restored by HCI*/
UCHAR GAP_SetLinkKeyManageMode(UCHAR mode);
UCHAR GAP_GetConnHandle(UCHAR *bd_addr, WORD *conn_hdl);
UCHAR GAP_GetClockOffset(UCHAR *bd_addr, WORD *offset);
UCHAR GAP_SetClockOffset(UCHAR *bd_addr, WORD *offset);
UCHAR GAP_GetLinkMode(UCHAR *bd_addr, UCHAR *mode);
UCHAR GAP_GetEncryptInfo(UCHAR *bd_addr, UCHAR *encrypt);
UCHAR GAP_GetTestMode(void);
UCHAR GAP_GetDevClass(UCHAR *bd_addr, UCHAR *dev_class);


UCHAR GAP_GetTransDataSize(UCHAR *bd_addr, DWORD *receive_data, DWORD *send_data);

void GAP_SecuReg(struct SMServInfoStru *arg);
void GAP_SecuUnReg(struct SMSecUnRegStru *arg);
void GAP_SetTrusted(UCHAR *arg);
void GAP_ClearTrusted(UCHAR *arg);
UCHAR GAP_ComCompleteCbk(WORD idx,UCHAR plen,UCHAR* outp);




void GAP_TransScoData(WORD conn_hdl, WORD len, UCHAR *packet);
#ifdef CONFIG_OS_WIN32
INT16 GAP_WriteScoDataToInt(WORD conn_hdl, WORD len, UCHAR *data);
#endif


#endif

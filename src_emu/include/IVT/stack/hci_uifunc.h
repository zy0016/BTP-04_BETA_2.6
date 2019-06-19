/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
* Copyright (c) 2000 IVT Corporation
*
* All rights reserved.
* 
---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
	hci_uifunc.h
    
Abstract:
	The module defines the functions of user interface
	
Author:
    Hong Lin
Revision History:
4.5		change interface of ReadLocalInfo and WriteLocalInfo
4.11	change structures of ReadLocalInfo and WriteLocalInfo
4.30	add field to the structure WriteLocalInfoParamStru
---------------------------------------------------------------------------*/



#ifndef _HCI_UIFUNC_H
#define _HCI_UIFUNC_H

#include "sec_mgr.h"

/*----------------------------------MACRO & DEFINITION-----------------------------------------*/

/* user interface functions */

/* maxinimum length of command parameters */
#define MAX_PARAM_LEN			255

/* op code of read or write local info */
#define OP_LINK_POLICY			0
#define OP_PIN_TYPE				1		
#define OP_LOCAL_NAME			2
#define OP_CONN_ACC_TIMEOUT		3	
#define OP_PAGE_TIMEOUT			4	
#define OP_SCAN_EN				5
#define OP_PGSCAN_ACT			6	
#define OP_INQ_SCAN_ACT			7	
#define OP_AUTH_EN				8
#define OP_ENCRY_MODE			9
#define OP_DEV_CLASS			10	
#define OP_VOICE_SETTING		11	
#define OP_AUTO_FLUSH_TIMEOUT	12	
#define OP_NUM_BROAD_RETRANS	13
#define OP_HOLD_MODE_ACT		14
#define OP_TRANS_POWER_LEVEL	15
#define OP_SCO_FLOW_CTRL_EN		16	
#define OP_LINK_SUPER_TIMEOUT	17
#define OP_NUM_SUP_IAC			18
#define OP_CUR_IAC_LAP			19
#define OP_PGSCAN_PERIOD_MODE	20
#define OP_PGSCAN_MODE			21
#define OP_LOCAL_VER_INFO		22			
#define OP_LOCAL_SUP_FEATURES	23
#define OP_COUNTRY_CODE			24
#define OP_BDADDR				25
#define OP_FAILED_CONTACT_CNT	26
#define OP_GET_LINK_QUAILTY		27		
#define OP_RSSI					28
#define OP_LPBK_MODE			29

/* operation code of read information of the remote device */
#define OP_REM_NAME				0x01
#define OP_REM_FEATURE			0x02
#define OP_REM_VER				0x04
#define OP_REM_CLKOFF			0x08

/*operation code of changing connection parameters */
#define OP_CHANGE_CONN_PKT_TYPE		0x1
#define OP_AUTH_REQ					0x2
#define OP_SET_CONN_ENCRY			0x4
#define OP_CHANGE_CONN_LINK_KEY		0x8

/* different mode of link policy */
#define ACTIVE_MODE 0
#define HOLD_MODE	1
#define SNIFF_MODE	2
#define PARK_MODE	3


/* acl or sco link mode */
#define ACL_LINK_MODE 1
#define SCO_LINK_MODE 0

/* some common parameters of link policy commands */
#define HOLD_MAX_INTERVAL	100
#define HOLD_MIN_INTERVAL	1
#define SNIFF_MAX_INTERVAL	100
#define SNIFF_MIN_INTERVAL	1
#define SNIFF_ATMPT			40
#define SNIFF_TMOUT			100
#define BCN_MAX_INTERVAL	100
#define BCN_MIN_INTERVAL	1

/* some common parameters of inquiry commands */
extern const char GIAC[];
extern const char LIAC[];
#define INQ_LEN				4
#define INQ_MAX_PER_LEN		10	
#define INQ_MIN_PER_LEN		5
#define INQ_NUM_RESP		10

/* definitions of link type */
/* packet type  for ACL connection */
#ifndef HCIDEFS_H 
#define PKT_TYPE_DM1		0x8
#define PKT_TYPE_DH1		0x10
#define PKT_TYPE_AUX1		0x200
#define PKT_TYPE_DM3		0x400
#define PKT_TYPE_DH3		0x800
#define PKT_TYPE_DM5		0x4000
#define PKT_TYPE_DH5		0x8000

/* packet type for SCO connection */
#define PKT_TYPE_HV1		0x20
#define PKT_TYPE_HV2		0x40
#define PKT_TYPE_HV3		0x80
#define PKT_TYPE_DV			0x100
#endif

/* link policy settings */
#define LPS_DISABLE_ALL				0x0		/* disable all modes */
#define LPS_MASTER_SLAVE_SWITCH		0x1		/* enable master/slave switch */
#define LPS_HOLD_MODE				0x2		/* enable hold mode */
#define LPS_SNIFF_MODE				0x4		/* enable sniff mode */
#define LPS_PARK_MODE				0x8		/* enable park mode */

/* encryption enable/disable */
#define ENCRY_ON			0x01
#define ENCRY_OFF			0x00

/* master/slave role */
#define MASTER_ROLE			0x0
#define SLAVE_ROLE			0x1

/* key flag */
#define SEMI_LINK_KEY		0x0
#define TEMP_LINK_KEY		0x1


/* event mask */
#define ALL_EVENT_ENABLED					(0xffffffff)
#define SEM_INQ_COMP_EV						(1L<<0)
#define SEM_INQ_RESULT_EV					(1L<<1)				
#define SEM_CONN_COMP_EV					(1L<<2)
#define SEM_CONN_REQ_EV						(1L<<3)				
#define SEM_DISC_COMP_EV   					(1L<<4)
#define SEM_AUTH_COMP_EV   					(1L<<5)
#define SEM_REM_NAME_REQ_COMP_EV   			(1L<<6)
#define SEM_ENCRYCHANGE_EV   				(1L<<7)	
#define SEM_CHANGECONNLINKKEYCOMP_EV   		(1L<<8)	
#define SEM_MASTERLINKKEYCOMP_EV   			(1L<<9)		
#define SEM_READ_REM_SUP_FEATURES_COMP_EV	(1L<<10)   	
#define SEM_READ_REM_VER_INFO_COMP_EV		(1L<<11)		
#define SEM_QOS_SETUP_COMP_EV   			(1L<<12)		
#define SEM_COMM_COMP_EV   					(1L<<13)	
#define SEM_COMM_STATUS_EV   				(1L<<14)		
#define SEM_HARD_ERR_EV   					(1L<<15)	
#define SEM_FLUSHED_OCCURRED_EV   			(1L<<16)
#define SEM_ROLE_CHANGE_EV   				(1L<<17)	
#define SEM_NUM_COMP_PKTS_EV   				(1L<<18)
#define SEM_MODE_CHANGE_EV   				(1L<<19)	
#define SEM_RET_LINK_KEYS_EV   				(1L<<20)	
#define SEM_PIN_CODE_REQ_EV   				(1L<<21)	
#define SEM_LINK_KEY_REQ_EV   				(1L<<22)	
#define SEM_LINK_KEY_NOT_EV   				(1L<<23)	
#define SEM_LPBK_COMM_EV   					(1L<<24)
#define SEM_DATA_BUF_OVERFLOW_EV   			(1L<<25)
#define SEM_MAX_SLOTS_CHANGE_EV   			(1L<<26)
#define SEM_READ_CLK_OFF_COMP_EV   			(1L<<27)
#define SEM_CONN_PKT_TYPE_CHANGED_EV   		(1L<<28)
#define SEM_QOS_VIOLA_EV   					(1L<<29)	
#define SEM_PGSCAN_MODE_CHANGE_EV   		(1L<<30)
#define SEM_PGSCAN_REP_MODE_CHANGE_EV   	(1L<<31)	

/* filter type in command Set_Event_Filter */
#define FT_CLR_ALL_FLT				0x0		/* clear all filters */
#define FT_INQ_RSLT					0x1		/* inquiry result */
#define FT_CONN_SETUP				0x2		/* connection setup */

/* filter condition type in command Set_Event_Filter */
#define FCT_ALL_DEV					0x0		/* all devices to response */
#define FCT_CLS_DEV					0x1		/* a specific class of device */
#define FCT_BD_ADDR					0X2		/* a specific bd_addr */

#define FCT_NOT_AUTO_ACCEPT			0x1		/*not auto-accept the connection */
#define FCT_AUTO_ACCEPT				0x2		/* auto-accept the connection */

#define VARIABLE_PIN				0x0		/* variable PIN */
#define FIXED_PIN					0x1		/* fixed PIN */

#define NO_SCAN_ENABLE				0x0		/* no scans enabled */
#define INQUIRY_SCAN_ENABLE			0x1		/* inquiry scan enabled */
#define PAGE_SCAN_ENABLE			0x2		/* page scan enabled */
#define INQUIRY_PAGE_SCAN_ENABLE	0x3		/* inquiry and page scan enable */

#define AUTHEN_DISABLE				0x0		/* authentication disabled */
#define AUTHEN_ENABLE				0x1		/* authentication enabled */

#define ENCRYPT_DISABLE				0x0		/* encryption disabled */
#define ENCRYPT_PTP					0x1		/* encryption only for point-to-point packets */
#define ENCRYPT_PTP_BRDCST			0x2		/* encryption for both point-to-point and broadcast packets */

#define INPUT_CODE_LINEAR			0x0		/* input coding: linear */
#define INPUT_CODE_M_LAW			0x100	/* input coding: ¦Ì-law input coding */
#define INPUT_CODE_A_LAW			0x200	/* input Coding: a-law input coding */
#define INPUT_CODE_RESERVED			0x300	/* reserved for future use */
#define INPUT_DATA_1_COMPLEMENT		0x0		/* input data format: 1¡¯s complement */
#define INPUT_DATA_2_COMPLEMENT		0x40	/* input data format: 2¡¯s complement */
#define INPUT_DATA_SIGN_MAG			0x80	/* input data format: sign-magnitude */
#define INPUT_DATA_RESERVED			0xc0	/* reserved for future use */
#define INPUT_SIZE_8				0x0		/* input sample size: 8 bit (only for liner PCM) */
#define INPUT_SIZE_16				0x10	/* input sample size: 16 bit (only for liner PCM) */
#define AIR_CODE_CVSD				0x0		/* air coding format: CVSD */
#define AIR_CODE_M_LAW				0x1		/* air coding format: ¦Ì-law */
#define AIR_CODE_A_LAW				0x2		/* air coding format: a-law */
#define AIR_CODE_RESERVED			0x3		/* reserved */
#define AIR_CODE_DEFAULT			0x60	/* default condition */

#define HOLD_MAINTAIN				0x00	/* maintain current power state */
#define HOLD_PAGE_SCAN				0x01	/* suspend page scan */
#define HOLD_INQUIRY_SCAN			0x02	/* suspend inquiry scan */
#define HOLD_PER_INQUIRY			0x04	/* suspend periodic inquiries */

#define CURRENT_TPL					0x00	/* read current transmit power level */
#define MAX_TPL						0x01	/* read maximum transmit power level */

#define SCO_FLOW_CONTROL_DISABLE	0x00	/* SCO flow control disable */
#define SCO_FLOW_CONTROL_ENABLE		0x01	/* SCO flow control enable */

#define HOST_FLOW_CONTROL_DISABLE		0x0	/* host controller flow control disable */
#define HOST_ACL_FLOW_CONTROL_ENABLE	0x1	/* host controller ACL flow control enable */
#define HOST_SCO_FLOW_CONTROL_ENABLE	0x2		/* host controller SCO flow control enable */
#define HOST_FLOW_CONTROL_ENABLE		0x3		/* host controller ACL and SCO flow control enable */

#define PAGE_SCAN_PER_MODE_P0		0x0		/* page scan_period_mode P0 */
#define PAGE_SCAN_PER_MODE_P1		0x1		/* page scan_period_mode P1 */
#define PAGE_SCAN_PER_MODE_P2		0x2		/* page scan_period_mode P2 */

#define PAGE_SCAN_MODE_MAN			0x0		/* mandatory page scan mode */
#define PAGE_SCAN_MODE_I			0x1		/* optional page scan mode i */
#define PAGE_SCAN_MODE_II			0x2		/* optional page scan mode ii */
#define PAGE_SCAN_MODE_III			0x3		/* optional page scan mode iii */

#define NO_LOOPBACK					0x0		/* no loopback mode enabled */
#define LOCAL_LOOPBACK				0x1		/* enable local loopback */
#define REMOTE_LOOPBACK				0x2		/* enable remote loopback */


/* bit set of device mode used in HciEnableDevice(WORD flag) */
#define VM_DISABLE			0x0000		/* non-discoverable mode */
#define VM_LIMIT_DISCOV		0x0002		/* limited discoverable mode */
#define VM_GENERAL_DISCOV	0x0003		/* general discoverable mode */
#define VM_PAGE				0x0004		/* connectable mode */
#define VM_PAIR				0x0008		/* pairable mode */
#define VM_ENCRYPT			0x0010		/* encryption enable */
#define VM_DISCOV			VM_GENERAL_DISCOV
#define VM_DEFAULT			(VM_GENERAL_DISCOV | VM_PAGE | VM_PAIR) /* default local mode */

/* bit set of role of local device */
#define SLR_MASTER			0x0				/* the local device must be master */
#define SLR_SLAVE			0x1				/* the local device must be slave */
#define SLR_MASTER_SLAVE	0x3				/* the local device may be master or slave */

/* definitions of security level of services */
#define SL_AUTHORISATION_IN		0x1				/* authorisation required for incoming connection */
#define SL_AUTHENTICATION_IN	0x2				/* authentication required for incoming connection */
#define SL_ENCRYPTION_IN		0x4				/* encryption required for incoming connection */
#define SL_AUTHORISATION_OUT	0x8				/* authorisation required for outgoing connection */
#define SL_AUTHENTICATION_OUT	0x10			/* authentication required for outgoing connection */
#define SL_ENCRYPTION_OUT		0x20			/* encryption required for outgoing connection */
#define SL_CONNECTIONLESS		0x40			/* reception of connectionless packets allowed */
#define SL_NO_SECURITY			0x0				/* no security required */
#define SL_DEFAULT				SL_AUTHORISATION_IN | SL_AUTHENTICATION_IN | SL_AUTHENTICATION_OUT		/* default secuiry level */

/* definitions of security mode */
#define SM_MODE_1		0x0			/* security mode 1 */
#define SM_MODE_2		0x1			/* security mode 2 */
#define SM_MODE_3		0x2			/* security mode 3 */

/* definition of management mode of link key */
#define LK_NO_SAVE		0x0			/* not save link key when pairing */
#define LK_SAVE			0x1			/* save link key and not mark trusted when pairing */ 
#define LK_SAVE_TRUSTED	0x2			/* save link key and mark trusted when pairing */ 

/* definitions of key types */
#define COMBINATION_KEY	0x0			/* key type combination key */
#define LOCAL_UNIT_KEY	0x1			/* key type local unit key */
#define REMOTE_UNIT_KEY	0x2			/* key tyep remote unit key */

#define FCT_AUTO_ACCEPT_ROLE_SWITCH 0x3 /* auto-accept the connection and role switch */

#define BT_VERSION_10B	0x0			/* version 1.0B */
#define BT_VERSION_11	0X1			/* version 1.1 */

/* indication of the event, used for HciRegister */
#define EV_INQUIRY_IND				0x20			/* inquiry event indication */
#define EV_ROLE_CHANGE_IND			0x21			/* role change event indication */
#define EV_LINK_MODE_CHANGE_IND		0x22			/* link mode change event indication */
#define EV_ENCRY_MODE_CHANGE_IND	0x23			/* encryption mode change event indication */
#define EV_AUTHORIZED_IND			0x24			/* authorized event indication */
#define EV_SCO_CONNECT_IND			0x25			/* SCO connection indication */
#define EV_SCO_CONNECT_CFM			0x26			/* SCO connection confirmation */
#define EV_SCO_DISCONNECT_IND		0x27			/* SCO disconnection indication */
#define EV_PIN_CODE_REQUEST_IND		0x28			/* pin code request event indication */
#define EV_LINK_KEY_REQUEST_IND		0x29			/* link key request event indication */
#define EV_LINK_KEY_IND				0x2A			/* link key event indication */	
#define EV_ERROR_IND				0x2B			/* error code indication */
#define EV_BUFFER_STATUS_IND		0x2C			/* buffer status indication */
#define EV_AUTHEN_FAILED_IND		0x2D            /* authentication failed indication */
#define EV_CONN_REQ_IND				0x2E           /*connection request indication */
#define EV_CLASS_DEV_IND			0x2F           /*class of device indication */
#define EV_HARD_ERR_IND				0X30			/*hardware error indication*/

/* current status of hci data sending buffer */
#define HCI_OVERFLOW                0x1                 /* no buffer available */ 
#define HCI_FULL                     0x2                 /* more than 2/3 of total buffer has been used */ 
#define HCI_AVAILABLE                0x3                  /* less than 1/3 of total buffer has been used */

/* definitions of error code*/
#define  ER_UNKNOWN_HCI_CMD            0x01            /*Unknown HCI Command*/
#define  ER_NO_CONNECT                 0x02            /*No Connection*/
#define  ER_HARDWARE_FAILURE           0x03            /*Hardware Failure*/ 
#define  ER_PAGE_TIMEOUT               0x04            /*Page Timeout*/
#define  ER_AUTH_FAILURE               0x05            /*Authentication Failure*/
#define  ER_KEY_MISSING		           0x06            /*Key Missing*/
#define  ER_MEMORY_FULL                0x07            /*Memory Full*/
#define  ER_CONN_TIMEOUT               0x08            /*Connection Timeout*/
#define  ER_MAX_NUM_CONN               0x09            /*Max Number Of Connections*/
#define  ER_Max_NUM_SCO_CONN           0x0A            /*Max Number Of SCO Connections To A Device*/
#define  ER_ACL_CONN_EXIST             0x0B            /*ACL connection already exists*/
#define  ER_COMMAND_DISALLOW           0x0C            /*Command Disallowed*/
#define  ER_HOST_REJ_lIMI_RESOURCE     0x0D            /*Host Rejected due to limited resources*/
#define  ER_HOST_REJ_SECU_REASON       0x0E            /*Host Rejected due to security reasons*/
#define  ER_HOST_REJ_REMOTE_PERS_DEV   0x0F            /*Host Rejected due to remote device is only a personal device*/ 
#define  ER_HOST_TIMEOUT               0x10            /*Host Timeout*/
#define  ER_UNSUPP_FEAT_OR_PARA_VALUE  0x11            /*Unsupported Feature or Parameter Value*/
#define  ER_INVALID_HCI_COMM_PARA      0x12            /*Invalid HCI Command Parameters*/
#define  ER_OTHER_END_USER_END_CONN    0x13            /*Other End Terminated Connection: User Ended Connection*/
#define  ER_OTHER_END_LOW_RESOURCE     0x14            /*Other End Terminated Connection: Low Resources*/
#define  ER_OTHER_END_POWER_OFF        0x15            /*Other End Terminated Connection: About to Power Off*/
#define  ER_LOCAL_HOST_TERMI_CONN      0x16            /*Connection Terminated by Local Host*/
#define  ER_REPEATED_ATTEMPT           0x17            /* Repeated Attempts*/
#define  ER_PAIRING_NOT_ALLOW          0x18            /*Pairing Not Allowed*/
#define  ER_UNKNOWN_LMP_PDU            0x19            /*Unknown LMP PDU*/
#define  ER_UNSUPP_REMOTE_FEATURE      0x1A            /*Unsupported Remote Feature*/
#define  ER_SCO_OFF_REJECT             0x1B            /*SCO Offset Rejected*/
#define  ER_SCO_INTERVAL_REJECT        0x1C            /*SCO Interval Rejected*/
#define  ER_SCO_AIR_MODE_REJECT        0x1D            /*SCO Air Mode Rejected*/
#define  ER_INVALID_LMP_PARA           0x1E            /*Invalid LMP Parameters*/
#define  ER_UNSPEC_ERROR               0x1F            /*Unspecified Error*/
#define  ER_UNSUPP_LMP_PARA_VALUE      0x20            /*Unsupported LMP Parameter Value*/
#define  ER_ROLE_CHANGE_NOT_ALLOW      0x21            /*Role Change Not Allowed*/
#define  ER_LMP_RESPONSE_TIMEOUT       0x22            /*LMP Response Timeout*/
#define  ER_LMP_ERROR_TRANS_COLLISION  0x23            /*LMP Error Transaction Collision*/
#define  ER_LMP_PDU_NOT_ALLOW          0x24            /*LMP PDU Not Allowed*/
#define  ER_ENCRYPT_MODE_NOT_ACCEPT    0x25            /*Encryption Mode Not Acceptable*/
#define  ER_UNIT_KEY_USED              0x26            /*Unit Key Used*/
#define  ER_QOS_NOT_SUPPORT            0x27            /*QoS is Not Supported*/
#define  ER_INSTANT_PASSED             0x28            /*Instant Passed*/
#define  ER_UNIT_KEY_PAIR_NOT_SUPPORT  0x29            /*Pairing with Unit Key Not Supported*/

/*---------------------------------------FUNCTION DECLARATION----------------------------------*/

/* some functions  upper layers may use */
UCHAR HciInit(void);
void HciDone(void);

FUNC_EXPORT UCHAR HciReadLocalInfo(UCHAR op, UCHAR *inp, 
								 UCHAR *outp);
FUNC_EXPORT UCHAR HciWriteLocalInfo(UCHAR op, UCHAR *inp,
								  UCHAR *outp);
FUNC_EXPORT UCHAR HciReadLinkPolicy(UCHAR *bd_addr, WORD *lp);
FUNC_EXPORT UCHAR HciWriteLinkPolicy(UCHAR *bd_addr, WORD lp);
FUNC_EXPORT UCHAR HciReadAutoFlushTimeout(UCHAR *bd_addr, WORD *tm);
FUNC_EXPORT UCHAR HciWriteAutoFlushTimeout(UCHAR *bd_addr, WORD tm);
FUNC_EXPORT UCHAR HciReadSupervisionTimeout(UCHAR *bd_addr, WORD *tm);
FUNC_EXPORT UCHAR HciWriteSupervisionTimeout(UCHAR *bd_addr, WORD tm);
FUNC_EXPORT UCHAR HciGetLinkHandle(UCHAR *bd_addr, WORD *conn_hdl);
FUNC_EXPORT UCHAR HciGetLinkMode(UCHAR *bd_addr, WORD *lnk_mode);
FUNC_EXPORT UCHAR HciGetLinkKey(UCHAR *bd_addr, UCHAR *ln_key);
FUNC_EXPORT UCHAR HciConnect(UCHAR *bd_addr);
FUNC_EXPORT UCHAR HciConnectRsp(UCHAR *bd_addr, UCHAR resp);
FUNC_EXPORT UCHAR HciQoS(UCHAR *bd_addr, struct QosType * qos);
FUNC_EXPORT UCHAR HciDisconnect(UCHAR *bd_addr);
FUNC_EXPORT UCHAR HciConnectSco(UCHAR *bd_addr, WORD pkt_type, WORD *conn_hdl);
FUNC_EXPORT UCHAR HciConnectScoRsp(UCHAR *bd_addr, UCHAR resp, WORD *conn_hdl);
FUNC_EXPORT UCHAR HciDisconnectSco(WORD conn_hdl);
FUNC_EXPORT UCHAR HciChangeLinkMode(UCHAR *bd_addr, UCHAR mode, UCHAR *param);
FUNC_EXPORT UCHAR HciExitCurrMode(UCHAR *bd_addr, UCHAR mode);
FUNC_EXPORT UCHAR HciChangeConnParam(UCHAR *bd_addr, UCHAR op_mask, 
								   struct ChangeConnParamStru *param);
FUNC_EXPORT UCHAR HciReadRemDevInfo(UCHAR *bd_addr, UCHAR op_mask, 
								  struct RemDevInfoStru *dev_info);
FUNC_EXPORT UCHAR HciSwRole(UCHAR *bd_addr, UCHAR role);
FUNC_EXPORT UCHAR HciRoleDcv(UCHAR *bd_addr, UCHAR *role);
FUNC_EXPORT UCHAR HciMasterLinkKey(UCHAR key_flag);
FUNC_EXPORT UCHAR HciInquiry(struct InquiryScanStru *param, DWORD *num_rsp, struct InquiryResultStru *result);
FUNC_EXPORT UCHAR HciInquiry2(struct InquiryScanStru *param);
FUNC_EXPORT UCHAR HciInquiryCancel(void);
FUNC_EXPORT UCHAR HciPerInqMode(struct PerInquiryScanStru *param, DWORD *num_rsp, struct InquiryResultStru *result);
FUNC_EXPORT UCHAR HciPerInqModeCancel(void);
FUNC_EXPORT UCHAR HciReset(char *port_name);
FUNC_EXPORT UCHAR HciResetHardware(char *port_name);
struct DataTransStru;
FUNC_EXPORT INT16 HciTransData(struct DataTransStru *data);
#ifndef CONFIG_HCI_NO_SCO_DATA
struct ScoDataTransStru;
FUNC_EXPORT INT16 HciTransScoData(struct ScoDataTransStru *data);
#endif
FUNC_EXPORT UCHAR HciSetEvMask(DWORD ev_mask);
FUNC_EXPORT UCHAR HciSetEventFilter(struct SetEventFilterStru *ev_flt);
FUNC_EXPORT UCHAR HciGetLinkQuality(UCHAR *bd_addr, UCHAR *quality);
FUNC_EXPORT UCHAR HciReadRssi(UCHAR *bd_addr, CHAR *rssi);
FUNC_EXPORT UCHAR HciGetTransDataSize(UCHAR *bd_addr, DWORD *receive_data, DWORD *send_data);
struct EventIndInStru;
FUNC_EXPORT UCHAR HciGetCurrLinkMode(UCHAR *bd_addr, WORD *mode, WORD *interval);
FUNC_EXPORT UCHAR HciRegister(struct EventIndInStru *ev_ind);
FUNC_EXPORT UCHAR HciUnregister(WORD event);
FUNC_EXPORT UCHAR HciGetClockOffset(UCHAR *bd_addr);
FUNC_EXPORT UCHAR HciPinCode(struct PinCodeStru *pin);
FUNC_EXPORT UCHAR HciSendLinkKeyNeg(UCHAR *bd_addr);
FUNC_EXPORT UCHAR HciSendLinkKey(struct LinkKeyStru *linkkey);
FUNC_EXPORT UCHAR HciSendPinCode(struct PinCodeStru *pin);
FUNC_EXPORT UCHAR HciLinkKey(struct LinkKeyStru *lk);
FUNC_EXPORT UCHAR HciClrLinkKey(UCHAR *bd_addr);
FUNC_EXPORT UCHAR HciEnableDevice(WORD flag);
FUNC_EXPORT UCHAR HciSetLocalDeviceRole(UCHAR role);
FUNC_EXPORT UCHAR HciGetLocalDeviceRole(UCHAR *role);
FUNC_EXPORT UCHAR HciSetLocalClkOffset(UCHAR *bd_addr, WORD clk_off);
FUNC_EXPORT UCHAR HciReverseRole(UCHAR *bd_addr);
FUNC_EXPORT UCHAR HciGetRemoteDeviceClass(UCHAR *bd_addr, UCHAR *dev_class);
FUNC_EXPORT UCHAR HciGetLocalDeviceClass(UCHAR *dev_class);
FUNC_EXPORT UCHAR HciSetLocalDeviceClass(UCHAR *dev_class);
FUNC_EXPORT UCHAR HciCreateNewUnitKey(void);

/*---------------------------------------CALLBACK FUNCTION DECLARATION----------------------*/

void HciConnectInd(UCHAR *bd_addr);
void HciConnectCfm(UCHAR *bd_addr, UCHAR result);
void HciScoConnectCfm(UCHAR *bd_addr, WORD hdl, UCHAR result);
void HciScoConnectInd(UCHAR *bd_addr, WORD conn_hdl, UCHAR is_connected);
void HciDisconnectInd(UCHAR *bd_addr);
void HciScoDisconnectInd(WORD conn_hdl);
void HciQosViolationInd(UCHAR *bd_addr);
void HciDataInd(struct DataTransStru  *packet);
void HciSecurityInd(UCHAR s, UCHAR *param);
#ifndef CONFIG_HCI_NO_SCO_DATA
void HciScoDataInd(struct ScoDataTransStru  *packet);
#endif
void HciConnEncryChangeInd(UCHAR *bd_addr, UCHAR encry_mode);
void HciRoleChangeInd(UCHAR *bd_addr, UCHAR role);
void HciLinkModeChangeInd(UCHAR *bd_addr, WORD mode, WORD interval);
UCHAR HciAuthorizeInd(UCHAR *bd_addr, struct AppSecRegStru * as);
void HciInquiryInd(UCHAR *bd_addr, UCHAR *dev_class);
void HciPinCodeRequestInd(UCHAR *bd_addr, WORD delay);
void HciLinkKeyRequestInd(UCHAR *bd_addr);
void HciLinkKeyInd(UCHAR* bd_addr,UCHAR *link_key);
void HciErrorInd(UCHAR code);
void HciBufferStatusInd(UCHAR status);
void HciAuthenFailedInd(UCHAR *bd_addr);
UCHAR HciConnReqInd(UCHAR *bd_addr);
UCHAR HciClassDevInd(UCHAR *bd_addr,UCHAR *class_of_device);
UCHAR HciHardErrInd(UCHAR err_code);
UCHAR HciAuthRemDev(UCHAR *bd_addr);

#ifdef CONFIG_OS_WIN32

void HciUSBUnavailable(void);
#endif


/*---------------------------------------SECURITY FUNCTION DECLARATION-------------------------*/

FUNC_EXPORT UCHAR HciSetLinkKeyManageMode(UCHAR *bd_addr, UCHAR lk_manage);
FUNC_EXPORT UCHAR HciSetSecurityMode(UCHAR sec_mode);
FUNC_EXPORT UCHAR HciGetSecurityMode(UCHAR *sec_mode);
FUNC_EXPORT UCHAR HciGetVisibleMode(UCHAR *vis_mode);
/*---------------------------------------------------------------------------------------------*/

#endif

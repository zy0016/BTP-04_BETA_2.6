#ifndef G_HCI_UI_H
#define G_HCI_UI_H

#define SEMI_LINK_KEY		0x0
#define TEMP_LINK_KEY		0x1

//#define MAX_NAME_LEN			248	/* length of bluetooh name */
#define MAX_PIN_CODE_LEN		16	/* maxinimum length of PIN code */
#define MAX_LINK_KEY_LEN		16	/* maxinimum length of Link key */
#define BD_ADDR_LEN				6	/* bluetooth address length */
#define CLS_DEV_LEN				3	/* class of device length */

#define EV_INQUIRY_IND				0x20			/* inquiry event indication */
#define EV_ROLE_CHANGE_IND			0x21			/* role change event indication */
#define EV_LINK_MODE_CHANGE_IND		0x22			/* link mode change event indication */
#define EV_ENCRY_MODE_CHANGE_IND	0x23			/* encryption mode change event indication */
#define EV_AUTHORIZATION_IND		0x24			/* authorized event indication */
#define EV_PIN_CODE_REQUEST_IND		0x28			/* pin code request event indication */
#define EV_LINK_KEY_REQUEST_IND		0x29			/* link key request event indication */
#define EV_LINK_KEY_IND				0x2A			/* link key event indication */	
#define EV_ERROR_IND				0x2B			/* error code indication */
#define EV_BUFFER_STATUS_IND		0x2C			/* buffer status indication */
#define EV_AUTHEN_FAILED_IND		0x2D            /* authentication failed indication */
#define EV_CONN_REQ_IND				0x2E           	/* connection request indication:acl,sco or esco */
#define EV_CLASS_DEV_IND			0x2F           	/* class of device indication */
#define EV_HARD_ERR_IND				0x30			/* hardware error indication*/
#define EV_CLK_OFFSET_CFM			0x31			/* read clock offset confirm */
#define EV_LINK_LOSS_IND			0x32			/* link loss indication */
#define EV_GET_LINKKEY_CFM			0x33			/* get link key confirm */
#define EV_CHANGE_LOCNAME_IND		0x34			/* change local name indication */
#define EV_LOOPBACK_IND				0x25			/* loopback command indication */
#define EV_INQ_IND_EX				0x3A			/*add by xufei for jepico*/

#define EV_SYNC_CONN_CFM			0x26			/* synchronous connection confirmation */
#define EV_SYNC_DISC_IND			0x27			/* SCO and eSCO disconnection indication */
#define EV_SCO_DISCONNECT_IND		EV_SYNC_DISC_IND
#define EV_SCO_CONNECT_CFM			EV_SYNC_CONN_CFM
#define EV_CONN_COMP_CFM			0x37			/* acl connection establishment conform*/

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

#ifdef CONFIG_HCI_12
#define EV_SYNC_CONN_CHANGE_IND		0x35			/* synchronous connection change indication */
#define EV_FLOW_SPEC_CFM			0x36			/* flow specification confirmation */
#endif

#define EV_SCO_DATA_IND				0x40	

/* the return value of EV_CONN_REQ_IND callback function */
#define	 DO_NOT_CARE				0x00	/* upper layer do not care */
#define  UP_LAYER_DO				0x01	/* upper layer will process (answer or reject) the request by itself */
#define	 HCI_ACCEPT					0x02	/* upper layer want hci to accept the connection request indication */
#define	 HCI_REJECT					0x03	/* upper layer want hci to reject the connection request indication (except acl) */

/* callback function registration structure */
struct EventIndInStru {
	WORD event;
	UCHAR* callback;
	UCHAR* param;
};

/* Command buffer */
struct CommandStru{
	WORD idx;							/* command index*/	
	WORD len;							/* length of param*/
	UCHAR param[1];						/* buffer of command */
};

/* pin code input from user */
struct PinCodeStru {
	UCHAR bd_addr[BDLENGTH];			/* baseband address */
    UCHAR pin_len;						/* length of pin code */
	char pin[MAX_PIN_CODE_LEN];			/* PIN code */
};

/* link key input from user */
struct LinkKeyStru {
	UCHAR bd_addr[BDLENGTH];			/* baseband address */
	char linkey[MAX_LINK_KEY_LEN];		/* link key */
};

/* Qos param from l2cap */
struct QosType{
	UCHAR flags;				/* flags reserved */
	UCHAR svc_type;			/* service type */
	DWORD token_rate;		/* tokeb rate */
	DWORD peak_bdwid;		/* peak bandwidth */
	DWORD latency;			/* latency */
	DWORD delay_var;		/* delay variation */
};

#define ERR_INVALID_COMMAND		0xFF
#define ERR_TIMEOUT_1			0xFE
#define ERR_TIMEOUT_2			0xFD
#define ERR_INVALID_STATE		0xFC

UCHAR HciInit(void);
void HciDone(void);
UCHAR HciRegister(struct EventIndInStru *ev_ind);
UCHAR HciUnregister(WORD event);
UCHAR HciUnregister2(struct EventIndInStru *ev_ind);
FUNC_EXPORT UCHAR HciQoS(UCHAR *bd_addr, struct QosType * qos);
void HciAuthenReq(UCHAR *bd_addr);
void HciAuthorReq(UCHAR *bd_addr, UCHAR proto, WORD channel);
void HciEncryReq(UCHAR *bd_addr);
void HciConnect(UCHAR *bd_addr);
void HciDisconnect(UCHAR *bd_addr);
void HciConnectSco(UCHAR *bd_addr, WORD pkt_type);
void HciDisconnectSco(WORD conn_hdl);
void HciConnectRsp(UCHAR *bd_addr, UCHAR result);
#ifdef CONFIG_HCI_12
void HciFlowSpec(UCHAR *par);
void HciSetupSyncConn(UCHAR *par);
void HciSetupSyncConnRsp(UCHAR *par,UCHAR result);
void HciCreateConnCancel(UCHAR *bd_addr);
void HciRmtNameReqCancel(UCHAR *bd_addr);
#endif
void HciMasterLinkKey(UCHAR key_flag);
void HciLinkKeyReqReply(UCHAR *bd_addr, UCHAR *link_key);
void HciLinkKeyReqNegReply(UCHAR *bd_addr);
void HciPinCodeReqReply(UCHAR *bd_addr, UCHAR len, UCHAR *pin);
void HciPinCodeReqNegReply(UCHAR *bd_addr);
UCHAR HciGetLinkKey(UCHAR *bd_addr, UCHAR *link_key);
UCHAR HciSetLinkKey(UCHAR *bd_addr, UCHAR *link_key);
UCHAR HciGetRemoteClockOffset(UCHAR *bd_addr);
UCHAR HciSetRemoteClockOffset(UCHAR *bd_addr, WORD clock);
void HciSendSyncCommand(struct CommandStru * cmd);
void HciSendAsyncCommand(struct CommandStru * cmd);
void HciTransData(struct BuffStru *data);
void HciTransSyncData(struct BuffStru *data);
#define HciTransScoData(data)	HciTransSyncData(data)
UCHAR HciWriteScanEnable(UCHAR scan_enable);
UCHAR HciReadLocalAddress(UCHAR *bd_addr);
UCHAR HciGetRmtBdaddr(WORD handle,UCHAR *bd_addr);
UCHAR Hci_GetTestMode(void);

UCHAR ReadLocalInfo(DWORD idx, UCHAR *data);
UCHAR WriteLocalInfo(DWORD idx, UCHAR *data);
UCHAR WriteRemoteInfo(UCHAR *bd_addr, WORD op_idx, UCHAR *data);
UCHAR ReadRemoteInfo(UCHAR* bd_addr, WORD op_idx, UCHAR *data);

#define HciReadRmtLinkKey(bd,info)			ReadRemoteInfo(bd,REMOTE_LINK_KEY,info)
#define HciReadRmtPinCode(bd,info)			ReadRemoteInfo(bd,REMOTE_PIN_CODE,info)
#define HciReadRmtSentSize(bd,info)			ReadRemoteInfo(bd,REMOTE_SENT_SIZE,info)
#define HciReadRmtRevSize(bd,info)			ReadRemoteInfo(bd,REMOTE_RCV_SIZE,info)
#define HciReadRmtClkOff(bd,info)			ReadRemoteInfo(bd,REMOTE_CLK_OFF,info)
#define HciReadRmtRole(bd,info)				ReadRemoteInfo(bd,REMOTE_ROLE,info)
#define HciReadRmtTrust(bd,info)			ReadRemoteInfo(bd,REMOTE_TRUSTED,info)
#define HciReadConnHdl(bd,info)				ReadRemoteInfo(bd,REMOTE_CONNHDL,info)
#define HciReadRmtLinkMode(bd,info)			ReadRemoteInfo(bd,REMOTE_LINK_MODE,info)
#define HciReadRmtDevClass(bd,info)			ReadRemoteInfo(bd,REMOTE_DEV_CLASS,info)
#define HciReadRmtEncryption(bd,info)       ReadRemoteInfo(bd,REMOTE_ENCRYPT,info)

#define HciWriteRmtLinkKey(bd,info)			WriteRemoteInfo(bd,REMOTE_LINK_KEY,info)
#define HciWriteRmtPinCode(bd,info)			WriteRemoteInfo(bd,REMOTE_PIN_CODE,info)
#define HciWriteRmtSentSize(bd,info)		WriteRemoteInfo(bd,REMOTE_SENT_SIZE,info)
#define HciWriteRmtRevSize(bd,info)			WriteRemoteInfo(bd,REMOTE_RCV_SIZE,info)
#define HciWriteRmtClkOff(bd,info)			WriteRemoteInfo(bd,REMOTE_CLK_OFF,info)
#define HciWriteRmtRole(bd,info)			WriteRemoteInfo(bd,REMOTE_ROLE,info)
#define HciWriteRmtTrust(bd,info)			WriteRemoteInfo(bd,REMOTE_TRUSTED,info)

#endif

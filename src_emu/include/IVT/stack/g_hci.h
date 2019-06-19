/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	
* Copyright (c) 2000 IVT Corporation
*
* All rights reserved.
* 
---------------------------------------------------------------------------*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
File Name:
    g_hci.h
Abstract:
	The header defines global macros, definintions, structures and viariables
	which are mainly used in internal manner. 
Author:
    Hong Lin
Revision History:
4.6		add buffer header macro
4.10	fix timer out definition
4.13	add memory null macro
5.10	adjust layout of header
---------------------------------------------------------------------------*/


#ifndef G_HCI_H
#define G_HCI_H

#include "global.h"

#ifdef CONFIG_HCI_12
#define CONFIG_HCI_11
#endif

/*----------------------------------------MACRO------------------------------------------------*/
#define MAX_SCO_HDL				3
#define HCI_ER_BASE				200

#define MAX_PARAM_LEN			255
/*the number of bytes that used to restore support cmd info, ervery bit stands for one command
the meaning of every bit is list after the defination of LocalInfoStru.*/
#define BYTE_STORE_SUP_CMD      16     

#define HCI_COMM_OFF			2		/* reservered bytes for command packet */
#define HCI_COMM_HEADER			3		/* header bytes of command packet */
#define HCI_COMM_RESERVE		(HCI_COMM_OFF+HCI_COMM_HEADER)		/* header bytes of command packet */
#define HCI_ACL_OFF				2		/* reservered bytes for ACL data packet */
#define HCI_ACL_HEADER			4		/* header bytes of ACL data packet */
#define HCI_SCO_OFF				2		/* reservered bytes for SCO data packet */
#define HCI_SCO_HEADER			3		/* header bytes of SCO data packet */
#define HCI_EVT_OFF				2		/* reservered bytes for event packet */
#define HCI_EVT_HEADER			2		/* header bytes of event packet */
#define MAX_COMM_PKT_LEN		255		/* maxinimum length of one command packet, wangbin*/
#define LMP_FEATURES_LEN		8

/* sort of data sent or received*/
#define HCI_COMMAND_DATA		1
#define HCI_ACL_DATA			2
#define HCI_SCO_DATA			3
#define	HCI_EVENT_DATA			4

/*pairable*/
#define PAIR_DISABLE			0
#define PAIR_ENABLE				1

/* different mode of link policy */
#define ACTIVE_MODE				0
#define HOLD_MODE				1
#define SNIFF_MODE				2
#define PARK_MODE				3

#define BT_UNIT_TIME			0.625	/* unit time of command parameters */
/*class id of event handle */
#define HCI_BD_HDL				(PROT_HCI_BASE + 1)	/* bd address specified */
#define HCI_CH_HDL				(PROT_HCI_BASE + 2)	/* connection handle specified */
#define HCI_OP_HDL				(PROT_HCI_BASE + 3)	/* command index specified */
#define HCI_STOP_HDL			(PROT_HCI_BASE + 4)	/* status command complete specified */
#define HCI_ACL_BSY_HDL			(PROT_HCI_BASE + 5)	/* acl data busy specified */
#define HCI_SCO_BSY_HDL			(PROT_HCI_BASE + 6)	/* sco data busy specified */
#define HCI_COMM_BSY_HDL		(PROT_HCI_BASE + 7)	/* command sending busy specified */
#define HCI_DBG_BSY_HDL			(PROT_HCI_BASE + 8)	/* debug handle */
#define HCI_DATA_BSY_HDL		(PROT_HCI_BASE + 9)	/* data transmit busy */

/* error in status field of event*/
#define ER_HOST_REJECTED		0x0D	/* host rejects connection request */
#define ER_TERM_CONN			0x13	/* host terminates the connection */

#define ROLE_MASTER				0x0		/* local device must be the master of the connection */		
#define ROLE_SLAVE				0x1		/* local device must be the slave of the connection */
#define ROLE_MASTER_SLAVE		0x2		/* local device's role is free, which is the default value */
#define ROLE_UNDECIDED			0x3		/* local role is undecided. */

#ifdef CONFIG_HCI_12
#define MAX_OP_CODE				119		/* max number of command code  */
#define MAX_EV_CODE 			45		/* max number of event code  */
#define MAX_LI_OP				30		/* maxinimum operation code of reading/writing local information */
#else
#define MAX_OP_CODE				96		/* max number of command code  */
#define MAX_EV_CODE 			32		/* max number of event code  */
#define MAX_LI_OP				30		/* maxinimum operation code of reading/writing local information */
#endif

#define VENDER_EV				0xFF

#define MAX_TIMEOUT_INDEX		(IDX_REM_NAME_REQ + 1)	/* maximum index of dynamic timeout */
#define MAX_BDADDRINFO_NUM		15
#define MAX_DISCARD_NUM         5

#define LAP_LEN					3		/* length of LAP */
#define LMP_FEATURE_LEN			8		/* length of LMP features */
#define BT_MAX_NAME_LEN			248

#define TRUSTED					0x01
#define DISTRUSTED				0x02

#define DELAY_PINCODE_INPUT		50000  

typedef UCHAR (FuncEvent)(INT32, UCHAR *Parameters);	/* function type */

enum {
	LOCAL_BD_ADDR,
	LOCAL_IS_HC_RESET,
	LOCAL_ROLE,	
	LOCAL_SEC_MODE,
	LOCAL_AUTO_ACCEPT,	
	LOCAL_PARABLE,		
	LOCAL_VISIBLE,		
	LOCAL_KEYMANAGE,
	LOCAL_HC_ACL_PKT_LEN,	
	LOCAL_HC_SCO_PKT_LEN,	
	LOCAL_HC_ACL_PKT_NUM,
	LOCAL_HC_SCO_PKT_NUM,
	LOCAL_HC_CUR_ACL_NUM,
	LOCAL_HC_CUR_SCO_NUM,
	LOCAL_HOST_ACL_PKT_LEN,	
	LOCAL_HOST_SCO_PKT_LEN,	
	LOCAL_HOST_ACL_PKT_NUM,	
	LOCAL_HOST_SCO_PKT_NUM,	
	LOCAL_HOST_CUR_ACL_NUM,
	LOCAL_HOST_CUR_SCO_NUM,	
	LOCAL_HOST_FLOW_CTRL,
	LOCAL_HOST_ACL_LEN,
	LOCAL_HOST_SCO_LEN,
	LOCAL_LMP_FEATURE,
	LOCAL_SENT_SIZE,
	LOCAL_RCV_SIZE,
	LOCAL_VERSION,
	LOCAL_HCI_SUP_CMD
};

enum {
	REMOTE_LINK_KEY,	
	REMOTE_PIN_CODE,	
	REMOTE_SENT_SIZE,
	REMOTE_RCV_SIZE,	
	REMOTE_CLK_OFF,	
	REMOTE_ROLE,		
	REMOTE_TRUSTED,
	REMOTE_CONNHDL,
	REMOTE_LINK_MODE,
	REMOTE_DEV_CLASS,
	REMOTE_ENCRYPT
};

/*error number*/
#define HCI_ER_BASE				200
#define ER_NO_WAIT_HDL			(HCI_ER_BASE+0)		/* no waiting handle */
#define ER_SEND_COMM_FAILED		(HCI_ER_BASE+1)		/* fail to send command */
#define ER_COMM_STATUS_TIMEOUT	(HCI_ER_BASE+2)		/* command status event timeout */
#define ER_COMM_TIMEOUT			(HCI_ER_BASE+3)		/* command timeout */
#define ER_NOT_EQUAL_LEN		(HCI_ER_BASE+4)		/* not equal to the length of the event */
#define ER_WRONG_EV				(HCI_ER_BASE+5)		/* wrong event index*/
#define ER_NO_MEM				(HCI_ER_BASE+6)		/* no memory */
#define ER_OPCODE				(HCI_ER_BASE+7)		/* wrong operation code */
#define ER_ACL_EXISTED			(HCI_ER_BASE+8)		/* an acl link has established tetween two devices*/
#define ER_NO_ACL_LINK			(HCI_ER_BASE+9)		/* no acl link found*/
#define ER_INVALID_PARAM		(HCI_ER_BASE+10) 	/* invalid parameter*/

/* link type */
#define SCO_LINK_MODE				0x00
#define ACL_LINK_MODE				0x01
#define ESCO_LINK_MODE				0x02

/* link key management mode */
#define LK_APP_SAVE					0x0
#define LK_HCI_SAVE					0x1

/* controller to host flow control flags */
#define HC2HOST_FC_OFF				0
#define HC2HOST_FC_ACL				1
#define HC2HOST_FC_SCO				2
#define HC2HOST_FC_BOTH				3

/* g_hci_fsminst */
#define HCI_USER_DATA		((struct HciFsmUserDataStru *)(g_hci_fsminst->user_data))
#define HCI_REM_DEV_LIST	(((struct HciFsmUserDataStru *)(g_hci_fsminst->user_data))->remote_dev_list)
#define HCI_CALLBACK_LIST	(((struct HciFsmUserDataStru *)(g_hci_fsminst->user_data))->callback_list)
#define HCI_HOST_INFO		(((struct HciFsmUserDataStru *)(g_hci_fsminst->user_data))->info)
#define HCI_HOST_POLICY		(((struct HciFsmUserDataStru *)(g_hci_fsminst->user_data))->policy)

#define SET_FIRST(buf)		(*(DWORD*)(buf->data) |= 0x01000000)
#define SET_CONTINUE(buf)	(*(DWORD*)(buf->data) &= 0x00FFFFFF)
#define IS_FIRST(buf)		(*(DWORD*)(buf->data) &  0x01000000)

/*----------------------------------GLOBAL STRUCTURE-------------------------------------------*/
struct LocalInfoStru {
	UCHAR bd_addr[6];					/* local bluetooth address */
	UCHAR is_hc_reset;					/* host controller has been reseted. */
										/* Host to the Host Controller */
	INT16 hc_acl_pkt_len;		  		/* ACL data packet length */ 
	INT16 hc_sco_pkt_len;	        	/* SCO data packet length */
	UCHAR hc_acl_total_pkt;          	/* the number of ACL data packet buffer in hc */
	UCHAR hc_sco_total_pkt;				/* the number of SCO data packet buffer in hc */
										/* Host Controller to the Host */
	INT16 host_acl_pkt_len;				/* ACL data packet length */ 
	INT16 host_sco_pkt_len;				/* SCO data packet length */
	INT16 host_acl_pkt_num;				/* the number of ACL data packet buffer in host */
	INT16 host_sco_pkt_num;          	/* the number of SCO data packet buffer in host */
										/* the length of data which allowed to be buffered in HCI */
	INT32 host_acl_len;               	/* the total length of ACL data buffer */
	INT32 host_sco_len;					/* the total length of SCO data buffer */
										/* LMP features */
	UCHAR hc_lmp_features[LMP_FEATURES_LEN];

	UCHAR hci_version;                  /*HCI_Version: 0-HCI 1.0B;1-HCI 1.1;2-HCI 1.2; 3-HCI 2.0*/
	WORD  hci_revision;                 /*lgh not clear the meaning of this param*/
	UCHAR LMP_version;
	WORD Manufacture_name;
	WORD LMP_Subvesion;

/*The support hci cmd, if the correspond bit is set to 1, then the cmd is supported.
the map of idx and correpond cmd is ni following table*/
#ifdef CONFIG_HCI_12
	UCHAR support_cmd[BYTE_STORE_SUP_CMD];
#endif
	
	DWORD total_size_send_data;			/* size of sent data since the local device is up*/ 
	DWORD total_size_rcv_data;			/* size of receive data since the local device is up*/ 
	WORD comm_port_hdl;					/* communication port handle */
	WORD default_pkt_type;				/* default packet type used to build acl connection */
	UCHAR loopback_mode;				/* test mode 0:normal 1:local 2:remote 3:local ready 4:remote ready */
	UCHAR tl_type;						/* transport layer type: USB/COM/BCSP*/
	UCHAR host_fc;						/* Host controller to host flow control flag */
	CHAR tl_port_name[20];				/* port name of transport layer */
};

/*
Octet    bit     command
          0      IDX_INQ
          1      IDX_INQ_CANCEL
          2      IDX_PERIODIC_INQ_MODE
0        3      IDX_EXIT_PERIODIC_INQ_MODE
          4      IDX_CREATE_CONN
          5      IDX_DISC
          6      IDX_ADD_SCO_CONN
          7      IDX_ACC_CONN_REQ
          0      IDX_REJ_CONN_REQ
          1      IDX_LINK_KEY_REQ_REPLY
          2      IDX_LINK_KEY_REQ_NEG_REPLY
1        3      IDX_PIN_CODE_REQ_REPLY
          4      IDX_PIN_CODE_REQ_NEG_REPLY
          5      IDX_CHANGE_CONN_PKT_TYPE
          6      IDX_AUTH_REQ
          7      IDX_SET_CONN_ENCRY
          0      IDX_CHANGE_CONN_LINK_KEY
          1      IDX_MASTER_LINKKEY
          2      IDX_REM_NAME_REQ
2        3      IDX_READ_REM_SUP_FEATURES
          4      IDX_READ_REM_VER_INFO
          5      IDX_READ_CLK_OFF
          6      IDX_HOLD_MODE
          7      IDX_SNIFF_MODE
          0      IDX_EXIT_SNIFF_MODE
          1      IDX_PARK_MODE
          2      IDX_EXIT_PARK_MODE
3        3      IDX_QOS_SETUP
          4      IDX_ROLE_DCV
          5      IDX_SW_ROLE
          6      IDX_READ_LINK_POLICY
          7      IDX_WRITE_LINK_POLICY
          0      IDX_SET_EV_MASK
          1      IDX_RESET
          2      IDX_SET_EV_FILTER
4        3      IDX_FLUSH
          4      IDX_READ_PIN_TYPE
          5      IDX_WRITE_PIN_TYPE
          6      IDX_CREATE_NEW_UNI_TKEY
          7      IDX_READ_STORED_LINK_KEY
          0      IDX_WRITE_STORED_LINK_KEY
          1      IDX_DEL_STORED_LINK_KEY
          2      IDX_CHANGE_LOCAL_NAME
5        3      IDX_READ_LOCAL_NAME
          4      IDX_READ_CONN_ACC_TIMEOUT
          5      IDX_WRITE_CONN_ACC_TIMEOUT
          6      IDX_READ_PAGE_TIMEOUT
          7      IDX_WRITE_PAGE_TIMEOUT
          0      IDX_READ_SCAN_EN
          1      IDX_WRITE_SCAN_EN
          2      IDX_READ_PGSCAN_ACT
6        3      IDX_WRITE_PGSCAN_ACT
          4      IDX_READ_INQ_SCAN_ACT
          5      IDX_WRITE_INQ_SCAN_ACT
          6      IDX_READ_AUTH_EN
          7      IDX_WRITE_AUTH_EN
          0      IDX_READ_ENCRY_MODE
          1      IDX_WRITE_ENCRY_MODE
          2      IDX_READ_DEV_CLASS
7        3      IDX_WRITE_DEV_CLASS
          4      IDX_READ_VOICE_SETTING
          5      IDX_WRITE_VOICE_SETTING
          6      IDX_READ_AUTO_FLUSH_TIMEOUT
          7      IDX_WRITE_AUTO_FLUSH_TIMEOUT
          0      IDX_READ_NUM_BROAD_RETRANS
          1      IDX_WRITE_NUM_BROAD_RETRANS
          2      IDX_READ_HOLD_MODE_ACT
8        3      IDX_WRITE_HOLD_MODE_ACT
          4      IDX_READ_TRANS_POWER_LEVEL
          5      IDX_READ_SCO_FLOW_CTRL_EN
          6      IDX_WRITE_SCO_FLOW_CTRL_EN
          7      IDX_SET_HOST_CTRL_TO_HOST_FLOW_CTRL
          0      IDX_HOST_BUF_SIZE
          1      IDX_HOST_NUM_COMP_PKTS
          2      IDX_READ_LINK_SUPER_TIMEOUT
9        3      IDX_WRITE_LINK_SUPER_TIMEOUT
          4      IDX_READ_NUM_SUP_IAC
          5      IDX_READ_CUR_IAC_LAP
          6      IDX_WRITE_CUR_IAC_LAP
          7      IDX_READ_PGSCAN_PERIOD_MODE
          0      IDX_WRITE_PGSCAN_PERIOD_MODE
          1      IDX_READ_PGSCAN_MODE                      //deprecated in hci 1.2
          2      IDX_WRITE_PGSCAN_MODE                    //deprecated in hci 1.2
10       3      IDX_READ_LOCAL_VER_INFO
          4      IDX_READ_LOCAL_SUP_FEATURES
          5      IDX_READ_BUF_SIZE
          6      IDX_READ_COUNTRY_CODE                  //deprecated in hci 1.2
          7      IDX_READ_BDADDR
          0      IDX_READ_FAILED_CONTACT_CNT
          1      IDX_RESET_FAILED_CONTACT_CNT
          2      IDX_GET_LINK_QUAILTY
11       3      IDX_READ_RSSI
          4      IDX_READ_LPBK_MODE
          5      IDX_WRITE_LPBK_MODE
          6      IDX_EN_DEV_UNDER_TEST_MODE
#ifdef CONFIG_HCI_12
          7      IDX_READ_REM_EX_FEATURES
          0      IDX_READ_LMP_HANDLE
          1      IDX_SETUP_SYNC_CONN
          2      IDX_READ_DEF_LINK_POLICY_SETTING
12      3      IDX_WRITE_DEF_LINK_POLICY_SETTING
          4      IDX_FLOW_SPEC
          5      IDX_SET_AFH_HOST_CHNL_CLS
          6      IDX_READ_INQ_SCAN_TYPE
          7      IDX_WRITE_INQ_SCAN_TYPE
          0      IDX_READ_INQ_MODE
          1      IDX_WRITE_INQ_MODE
          2      IDX_READ_PGSCAN_TYPE
13        3      IDX_WRITE_PGSCAN_TYPE
          4      IDX_READ_AFH_CHNL_ASSESS_MODE
          5      IDX_WRITE_AFH_CHNL_ASSESS_MODE
          6      IDX_READ_LOCAL_EX_FEATURES
          7      IDX_READ_AFH_CHNL_MAP
          0      IDX_READ_CLOCK
          1      IDX_ACC_SYNC_CONN_REQ
          2      IDX_REJ_SYNC_CONN_REQ
14        3      IDX_CREATE_CONN_CANCEL
          4      IDX_REM_NAME_REQ_CANCEL
          5      IDX_READ_LOCAL_SUP_COMMANDS
#endif
          6      IDX_VENDOR_CMD
          7      NULL
          0      NULL
          1      NULL
          2      NULL
15        3      NULL
          4      NULL
          5      NULL
          6      NULL
          7      NULL
*/
struct LocalPolicyStru {
	UCHAR local_role:2;					/*SLR_MASTER£¬SLR_SLAVE or SLR_MASTER_SLAVE*/
	UCHAR sec_mode:2;					/*security mode 1, 2, 3*/
	UCHAR auto_accept:1;				/*auto-accept remote baseband connection request*/
	UCHAR pairable:1;					/*parable or unpairable*/
	UCHAR visible:1;					/*visible or unvisible*/
	UCHAR lk_manage:1;
};

struct ConnInfoStru {
	DWORD size_send_data;         		/* size of sent data */ 
	DWORD size_rcv_data;       			/* size of received data */
	struct BuffStru *rcv_buf;
	WORD  conn_hdl[MAX_SCO_HDL + 1];	/* conn_hdl[0] is acl connection handle, the others are sco handles */
	WORD  pkt_type;
	UCHAR hdl_num;						/* number of connection handles*/
	UCHAR link_mode;
	UCHAR acl_sending;					/* number of sending acl packets */
	UCHAR sco_sending;					/* number of sending sco packets */
};

struct BTSecuInfoStru{
	UCHAR lk_type;
	UCHAR link_key[16];
	UCHAR pin_len;
	UCHAR pin_code[16];	
};

struct RemoteDevInfoStru {
	UCHAR bd_addr[6];
	WORD  clk_off;
	struct BTSecuInfoStru *sec_info;
	struct ConnInfoStru *conn_info;
	UCHAR dev_class[3];
	UCHAR scan_rep_mode:2;
	UCHAR scan_mode:2;
	UCHAR role:1;
	UCHAR trusted:1;
	UCHAR encry_enable:1;
};

/* structure of command information */
struct CommInfoStru{
	WORD op_code;						/*command code */
	DWORD timeout;						/* command time out_param */
	DWORD en_mask;						/* encode parameter mask */
	DWORD de_mask;						/* decode parameter mask */
	UCHAR de_len;						/* decode parameter length */
	UCHAR hdl_sort;						/* sort of event handle: 00-index 01- index+bdaddr 10-index+handle */
	WORD hdl_mask;						/* mask of event handle */
	UCHAR is_cc;						/* if returned with Command Complete event: 0x0e*/
};

/* structure of event map */
struct EventMapStru{
	UCHAR type;							/* 0: sync cmd event; 1: async cmd event; 2: indication */
	FuncEvent *func;					/* pointer to function which handles the event */
};

/*structure of FSM user_data*/
struct HciFsmUserDataStru {
	struct LocalInfoStru info;
	struct LocalPolicyStru policy;
	struct BtList *remote_dev_list;
	struct BtList* callback_list;		/* callback list */

	INT32 host_acl_cur_len;      		/* current available ACL data buffer in host data buffer */
	INT32 host_sco_cur_len;      		/* current available SCO data buffer in host data buffer */

	UCHAR host_acl_cur_num;      		/* current ACL data packet buffer number in host*/
	UCHAR host_sco_cur_num;      		/* current SCO data packet buffer number in host*/

	UCHAR hc_acl_cur_num;      			/* current ACL data packet buffer number in hc*/
	UCHAR hc_sco_cur_num;      			/* current SCO data packet buffer number in hc*/

	UCHAR hc_cmd_cur_num;				/* current command buffer number in hc*/
	struct BtList *acl_sbuf_list;		/* ACL data sending buffer list */
	struct BtList *sco_sbuf_list;		/* SCO data sending buffer list */
	struct BtList *cmd_sbuf_list;		/* Buffer hci command, when fsm is busy */
	struct CommandStru * async_cmd;
	struct CommandStru * sync_cmd;
	struct BuffStru *uncertain_data;	/*pointer to the data received before connection complete*/
	WORD sync_idx;
};

struct BDWParamStru{
	UCHAR bd[6];
	WORD wparam;
};

struct AuthorParamStru{
	UCHAR bd[6];
	WORD channel;
	UCHAR proto;
};

struct BdLkStru {
	UCHAR bd[6];
	UCHAR lk[16];
};
/*----------------------------------GLOBAL VARIABLES-------------------------------------------*/

extern const struct CommInfoStru comm_info[MAX_OP_CODE];	/* all command information */
extern struct FsmInst *g_hci_fsminst;
extern const struct EventMapStru ev_map[MAX_EV_CODE] ;		/* event map */


/*
UCHAR WriteLocalInfo(DWORD idx, UCHAR *data);
UCHAR ReadLocalInfo(DWORD idx, UCHAR *data);

UCHAR WriteRemoteInfo(UCHAR *bd, WORD idx, UCHAR *data);
UCHAR ReadRemoteInfo(UCHAR* bd, WORD idx, UCHAR *data);
*/

/*----------------------------------------INCLUDING--------------------------------------------*/

#include "gap_ui.h"
#include "hci_ui.h"
#include "hci_ev.h"
#include "sm.h"
#include "hci_idx.h"
#include "hci_stru.h"
#include "hci_hlp.h"
#include "hci_data.h"
#include "hci_tl.h"
#include "hci_fsm.h"
#include "l2capui.h"

#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
*
* Copyright (c) 1999-2002 IVT Corporation
*
* All rights reserved.
* 
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    hid_ui.h

Abstract:
	This file defines all the UI data structures and UI funtions.

Author:
    Yongping Zeng

Revision History:
2002.3
	create the initiator version
2003.3
	adapted for SOC stack by luo xiqiong
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef HID_UI_H
#define HID_UI_H

#ifndef BDADDRLEN
#define BDADDRLEN 6
#endif

#define HID_CONTROL_PSM	0x0011
#define HID_INTERRUPT_PSM 0x0013

/*marcos for indication and confirmation definitions*/
#define HID_CTRLCFM			1
#define HID_GETREPCFM		4
#define HID_SETREPCFM		5
#define HID_GETPROTCFM		6
#define HID_SETPROTCFM		7
#define HID_GETIDLECFM		8
#define HID_SETIDLECFM		9
#define HID_DATAIND			10
#define HID_CONNIND			12
#define HID_DISCIND			13
#define HID_RECONNIND		14
#define HID_UNPLUGIND		15
#define HID_CONNREQIND		16
#define HID_QOSIND			17
#define HID_CONNCFM			18
#define HID_DISCCFM			19

#define HIDCONTROL	((struct HID_CtrlInstStru*)(g_hid_fsm_inst->user_data))

#define HID_CCH		0
#define HID_ICH		1
#define HID_NONECH		0xFF

/* HID Transaction Type */
#define HID_HANDSHAKE		0x0
#define HID_CONTROL			0x1
#define HID_GET_REPORT		0x4
#define HID_SET_REPORT		0x5
#define HID_GET_PROTOCOL	0x6
#define HID_SET_PROTOCOL	0x7
#define HID_GET_IDLE		0x8
#define HID_SET_IDLE		0x9
#define HID_DATA			0xA
#define HID_DATC			0xB

/*HID Req Type*/
#define HID_DISC_REQ		0xC

/* result code for request operation */
#define HID_SUCCESSFUL					0x0
#define HID_ERR_NOT_READY				0x1
#define HID_ERR_INVALID_REPORT_ID		0x2
#define HID_ERR_UNSUPPORTED_REQUEST		0x3
#define HID_ERR_INVALID_PARAMETER		0x4
#define HID_ERR_UNKNOWN					0xE
#define HID_ERR_FATAL					0xF
#define HID_ERR_REQ_TIMEOUT				0x10

/* control operation code */
#define HID_NOP						0x0
#define HID_HARD_RESET				0x1
#define HID_SOFT_RESET				0x2
#define HID_SUSPEND					0x3
#define HID_EXIT_SUSPEND			0x4
#define HID_VIRTUAL_CABLE_UNPLUG	0x5

/* report type */
#define HID_OTHER_REPORT		0x0
#define HID_INPUT_REPORT		0x1
#define HID_OUTPUT_REPORT		0x2
#define HID_FEATURE_REPORT		0x3

/* HID protocol device */
#define HID_BOOT_PROTOCOL		0x0
#define HID_REPORT_PROTOCOL		0x1

/* HID report ID */
#define HID_REPORT_ID_KEYBOARD 0x01
#define HID_REPORT_ID_MOUSE 0x02

/* GET_REPORT size */
#define	HID_NO_SIZE				0x0
#define	HID_SPECIFIED_SIZE		0x1

/*error codes*/
#define HID_ERROR_NO_RESOURCE			(PROT_HID_BASE+10)
#define HID_ERROR_NO_CONNECTION			(PROT_HID_BASE+11)
#define HID_ERROR_IN_CONNECTION			(PROT_HID_BASE+12)
#define HID_ERROR_TIMEOUT				(PROT_HID_BASE+13)
#define HID_ERROR_CONNECTION_FAIL		(PROT_HID_BASE+14)

/* HID sub-class definition */
#define HID_SUBCLASS_KEYBOARD			0x40    
#define HID_SUBCLASS_POINT				0x80
#define HID_SUBCLASS_KEYORPOINT			0xC0
#define HID_SUBCLASS_UNCLASSIFIED		0x00 
#define HID_SUBCLASS_JOYSTICK			0x04
#define HID_SUBCLASS_GAMEPAD			0x08
#define HID_SUBCLASS_REMCONTROL			0x0C
#define HID_SUBCLASS_SENSE				0x10


/* HID Transaction header bytes len */
#define HID_THDR_LEN 0x1
#define HID_REPORT_ID_LEN 0x1
#define HID_REPORT_BUF_LEN 0x2
#define HID_IDLE_RATE_LEN 0x1
#define HID_PROT_LEN 0x01
#define HID_REQ_MASK 0xF0

/*reserved buff lenth*/
#define HID_BUF_RES_LEN			( HCI_RESERVE + L2CAP_CO_RESERVE	)

/*define HID buf created Macro*/
#define HIDBUFFNEW(len) BuffNew((WORD)(len+HID_BUF_RES_LEN), HID_BUF_RES_LEN)

/*timer out*/
#define HID_REQ_TIMEOUT 		5000
#define HID_DISC_TIMEOUT		1000
//#define HID_CTL_TIMEOUT			3000

#define HID_TIMEOUT_BASE		(((DWORD)(PROT_HID))<<24)
//#define HID_TIMEOUT_BASE 		(PROT_HID_BASE + 60) 
#define HID_NO_TIMER			(HID_TIMEOUT_BASE + 0)
#define HID_CONN_REQ_TIMER		(HID_TIMEOUT_BASE + 1)
#define HID_DISC_REQ_TIMER		(HID_TIMEOUT_BASE + 2)
#define HID_CTRL_REQ_TIMER		(HID_TIMEOUT_BASE + 3)
#define HID_GET_REP_TIMER		(HID_TIMEOUT_BASE + 4)
#define HID_SET_REP_TIMER 		(HID_TIMEOUT_BASE + 5)
#define HID_GET_PRO_TIMER		(HID_TIMEOUT_BASE + 6)
#define HID_SET_PRO_TIMER		(HID_TIMEOUT_BASE + 7)
#define HID_GET_IDL_TIMER		(HID_TIMEOUT_BASE + 8)
#define HID_SET_IDL_TIMER		(HID_TIMEOUT_BASE + 9)
#define HID_CONN_CLNT_REQ_TIMER		(HID_TIMEOUT_BASE + 10)
#define HID_DISC_CLNT_REQ_TIMER		(HID_TIMEOUT_BASE + 11)

/*define handle name in field id*/
#define HID_H_CTRL			(PROT_HID_BASE + 21) 
#define HID_H_GET_REP		(PROT_HID_BASE + 22)
#define HID_H_SET_REP		(PROT_HID_BASE + 23)
#define HID_H_GET_PRO		(PROT_HID_BASE + 24)
#define HID_H_SET_PRO 		(PROT_HID_BASE + 25)
#define HID_H_GET_IDL  		(PROT_HID_BASE + 26)
#define HID_H_SET_IDL		(PROT_HID_BASE + 27)
#define HID_H_CONN_REQ		(PROT_HID_BASE + 28)
#define HID_H_DISC_REQ		(PROT_HID_BASE + 29)
#define HID_H_CONN_CLNT_REQ		(PROT_HID_BASE + 30)
#define HID_H_DISC_CLNT_REQ 	(PROT_HID_BASE + 31)

/*define handle class id*/
#define HID_CONN_REQ_HDL	(PROT_HID_BASE + 41)
#define HID_DISC_REQ_HDL	(PROT_HID_BASE + 42)
#define HID_CTRL_REQ_HDL	(PROT_HID_BASE + 43)
#define HID_GET_REP_REQ_HDL 	(PROT_HID_BASE + 44)
#define HID_SET_REP_REQ_HDL	 	(PROT_HID_BASE + 45)
#define HID_GET_PRO_REQ_HDL		(PROT_HID_BASE + 46)
#define HID_SET_PRO_REQ_HDL		(PROT_HID_BASE + 47)
#define HID_GET_IDL_REQ_HDL		(PROT_HID_BASE + 48)
#define HID_SET_IDL_REQ_HDL		(PROT_HID_BASE + 49)
#define HID_CONN_CLNT_REQ_HDL	 	(PROT_HID_BASE + 50)
#define HID_DISC_CLNT_REQ_HDL		(PROT_HID_BASE + 51)	


/*msg id for profile callback*/
#define HID_MSG_ID_BASE  0
#define HID_C_CONN_REQ			(HID_MSG_ID_BASE + 1)
#define HID_C_DISC_REQ			(HID_MSG_ID_BASE + 2)
#define HID_C_CTRL_REQ			(HID_MSG_ID_BASE + 3)
#define HID_C_GET_REP_REQ		(HID_MSG_ID_BASE + 4)
#define HID_C_SET_REP_REQ		(HID_MSG_ID_BASE + 5)
#define HID_C_GET_PRO_REQ		(HID_MSG_ID_BASE + 6)
#define HID_C_SET_PRO_REQ		(HID_MSG_ID_BASE + 7)
#define HID_C_GET_IDL_REQ		(HID_MSG_ID_BASE + 8)
#define HID_C_SET_IDL_REQ		(HID_MSG_ID_BASE + 9)
#define HID_C_DATA_REQ			(HID_MSG_ID_BASE + 10)

struct HIDBDHdlFieldStru{		/*defines the message handle structures and macros*/
	WORD handle_name;
	UCHAR bdaddr[BDADDRLEN];
};

struct HIDTHdrHdlFieldStru{
	WORD handle_name;
	DWORD hdl;
};

struct HIDBDPSMHdlFieldStru{
	WORD handle_name;
	UCHAR bdaddr[BDADDRLEN];
	WORD  psm;
};

struct HIDCIDHdlFieldStru{
	WORD handle_name;
	WORD cid;
};

struct HIDPSMHdlFieldStru
{
	WORD handle_name;
	WORD psm;
};


struct HIDConnReqCfmStru{		/*use for conn req's result*/
	DWORD hdl;				/*Server Side: hdl = 0*/
	WORD psm;	
	UCHAR res;	
};

#ifdef CONFIG_HID_CLIENT

struct HID_CtrlInstStru 
{
	struct NewBtList* sessionlist;
};

struct HID_SessionInstStru{
	UCHAR bdaddr[6];
	UCHAR subclass;
	UCHAR reqpending;
	struct ChannelStru
	{
		WORD cid;
		WORD mtu;
	} ctrl_ch, int_ch;	
	
#ifdef CONFIG_HID_SAR	
	struct NewBtList* datalist;
	struct NewBtList* ctrllist;
#endif

	struct  FsmInst *fsm_inst;		/*FSM instance*/
};

struct HID_MsgIndStru{		/*use for hid stack to notify upside msg inds comes*/
	DWORD hdl;
	UCHAR  bdaddr[BDADDRLEN];
};

struct HID_DataIndStru {
	DWORD hdl;
	WORD len;
	UCHAR buf[1];
};

struct HID_DataWriteInStru
{
	DWORD hdl;
	WORD len;
	UCHAR* buf;
};

struct HID_DataWriteOutStru 
{
	WORD result;
	WORD off;	/*data len sent*/
};

struct HID_ConnReqInStru 
{
	UCHAR bdaddr[6];
	UCHAR subclass;
	WORD cur_psm;
};

struct HID_ConnReqOutStru 
{
	UCHAR result;
	DWORD hdl;
};

struct HID_CtrlReqInStru{
	DWORD hdl;
	UCHAR ctrl_code;
};

struct HID_GetReportReqInStru
{
	DWORD hdl;
	UCHAR type;
	UCHAR report_id;
	WORD buf_size;
};

struct HID_SetReportReqInStru
{
	DWORD hdl;
	UCHAR type;
	WORD len;
	UCHAR *buf;
};

struct HID_SetProtReqInStru
{
	DWORD hdl;
	UCHAR prot;
};

struct HID_SetIdleReqInStru
{
	DWORD hdl;
	UCHAR rate;
};

struct HID_CtrlOrSetCfmStru {
	DWORD hdl;
	UCHAR result;
};

struct HID_GetRepCfmStru {
	DWORD hdl;
	UCHAR result;
	WORD len;
	UCHAR buf[1];
};

struct HID_GetProtCfmStru {
	DWORD hdl;
	UCHAR result;
	UCHAR protocol;
};

struct HID_GetIdleCfmStru {
	DWORD hdl;
	UCHAR result;
	UCHAR rate;
};

UCHAR HID_ClntCmdUI(UCHAR msgid, void* arg);
UCHAR HID_SendIndMessage(UCHAR msgid, UCHAR* param);
UCHAR HID_SendIndData(UCHAR msgid, UCHAR* data);
UCHAR HID_ClearStatus(UCHAR* hdl);
extern void HIDP_CallbackDispatch(WORD msgid, UCHAR* param);
extern void HIDP_NotifyActivated(WORD msgid, UCHAR* session, UCHAR* resParam);
extern UCHAR HIDP_NotifyIndInfo(WORD msgid, UCHAR* param);
extern UCHAR HIDP_GetDevsStatus(WORD msgid, UCHAR* param1);
extern UCHAR HIDP_GetHidSubclass(UCHAR* bdaddr);

#endif


/*===============================================
**===============================================
**/

#ifdef CONFIG_HID_SERVER

struct HID_CtrlIndStru {
	UCHAR* hdl;
	UCHAR ctrl_code;
};

struct HID_GetRepIndStru {
	UCHAR* hdl;
	UCHAR type;
	UCHAR report_id;
	WORD buf_size;
};

struct HID_GetRepRespStru {
	UCHAR* hdl;
	UCHAR result;
	UCHAR type;
	WORD len;
	UCHAR* buf[1];
};

struct HID_SetRepIndStru {
	UCHAR* hdl;
	UCHAR type;
	WORD len;
	UCHAR buf[1];
};

struct HID_GetProtRespStru {
	UCHAR* hdl;
	UCHAR result;
	UCHAR protocol;
};

struct HID_SetProtIndStru {
	UCHAR* hdl;
	UCHAR prot;
};

struct HID_GetIdleRespStru {
	UCHAR* hdl;
	UCHAR result;
	UCHAR rate;
};

struct HID_SetIdleIndStru {
	UCHAR* hdl;
	UCHAR rate;
};

extern struct QosStru ctrl_qos;
extern struct QosStru int_qos;

extern UCHAR HID_CtrlOrSet_Handler(UCHAR reqtype, WORD len, UCHAR* buf);
extern UCHAR* HID_Get_Handler(UCHAR reqtype, WORD len, UCHAR* buf);
extern UCHAR HID_Data_Handler(UCHAR* hdl, WORD len, UCHAR* buf);
extern UCHAR HIDP_SvrNotifyIndInfo(WORD msgid, UCHAR* param);

void HID_SendHandshake(WORD cid, UCHAR res);
void HID_SendGetProtResp(WORD cid, UCHAR prot);
void HID_SendGetRateResp(WORD cid, UCHAR rate);

#endif

#endif

#ifndef G_RCP_H
#define G_RCP_H
#include <stdio.h>

#include "global.h"

#include "sdap_ui.h"
#include "g_avctp.h"

#include "avrcpfsm.h"
#include "avrcpstruct.h"
#include "avrcpui.h"
#include "avrcpcode.h"
#include "avrcphlp.h"

/* g_ctype 's defination */
#define UNITINFO	1
#define SUBUNITINFO 2
#define PASSTHR		3
#define VENDORDEP	4

/*default mtu*/
#define AVRCP_DEFAULT_MTU			672

/* Device class defination */
#define DEVCLS_NULL		0x00
#define DEVCLS_CT		0x01
#define DEVCLS_TG		0x02
#define DEVCLS_CT_TG	0x03

/* Profile ID defination */
#define AVRCP_PROFILE_ID 0x110E

/* result of register event call back */
#define AVCT_EVT_REG_SUCCESS 0
#define AVCT_EVT_REG_FAILED -1

#define T101 0x1	/* CT retry timer */
#define T102 0x2	/* TG send interim timer */

/*for device class*/
#define MINOR_CLS_AUDIO_TG		0x00	/* 00000000, bit7-bit2 */
#define MAJOR_CLS_AUDIO			0x04	/* 00000100, bit12-bit8 */
#define IS_AUDIO_TG(p)			(*((UCHAR *)(p))==MINOR_CLS_AUDIO_TG)&&(*((UCHAR *)(p)+1)==MAJOR_CLS_AUDIO)?1:0

#define ALL						0xff
#define IS_TIMER_ACTIVE(p,t)	(p->timer&(t))?1:0
#define RESET_TIMER(p,t)		{p->timer&=~(t);}
#define CLR_TIMER(p,t)			{p->timer&=~(t); if((t)!=ALL)FsmDelTimer(g_avrcp_fsm_inst, t);}

/* for CTP. message type*/
#define AVRCP_CTP_CMDTYPE 0x01
#define AVRCP_CTP_RSPTYPE 0x02
struct AvrcpUserDataStru{

	UCHAR timer; /* bit0: T101; bit1T102 */


	UCHAR ct_trans_lbl;

	UCHAR tg_trans_lbl;
	
	struct AVCTP_DataIndStru *msg; /* point to received message */
	UINT16 msg_len;
	UINT16 pid;	/* profile ID */

	DWORD CT_svr_hdl; /* the handle of SDP CT service */
	DWORD TG_svr_hdl;/* the handle of SDP TG service*/

	AVRC_Cbk *pCbkFun;

	/*for CT */
	UCHAR *pCTCmdRspStru;
	HANDLE hCTUIEvent;  // this is for send msg
	HANDLE hUIConnEvent; //this is for connect and disconnect
//	UCHAR CTEventCall;/*for fsm , if event call function not called, BT_FaIL, */

	UINT16	cmd_status; /* CT; for user interface ,*/
	UCHAR	cmdtype;	/* last commmand type; TG; for vendep cmd rsp when t102 expired */

};


void SET_TIMER(struct AvrcpUserDataStru *p,UCHAR t);

#define INIT_CMD_MSG(msg,length) {			\
	msg->msg_type = AVRCP_CTP_CMDTYPE;					\
	msg->pid = ((struct AvrcpUserDataStru *)g_avrcp_fsm_inst->user_data)->pid;				\
	msg->len = length;						\
}

#define INIT_RSP_MSG(msg,length) {			\
	msg->msg_type = AVRCP_CTP_RSPTYPE;					\
	msg->pid = ((struct AvrcpUserDataStru *)g_avrcp_fsm_inst->user_data)->pid;				\
	msg->len = length;						\
}

extern struct FsmInst*				g_avrcp_fsm_inst;
extern struct AvrcpUserDataStru*	g_data;

#endif

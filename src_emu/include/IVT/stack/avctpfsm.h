/*
***************************************************************************
Module Name:
    avctpfsm.h
Abstract:
	Macro definition and state transition function definition
Author:
    Qian Lixin
Data:
    2001.8.2
**************************************************************************** 
*/

#ifndef  _AVCTP_FSM_H
#define  _AVCTP_FSM_H

/****************************************STATE********************************************/
/*the state(s) of the process dispatcher*/
enum {
	ST_AVCTP_DISP_STATE0
};
#define AVCTP_DISPATCHER_STATE_COUNT (ST_AVCTP_DISP_STATE0+1)


/*the state(s) of the process mh*/
enum {
	ST_AVCTP_MH_READY
};
#define AVCTP_MSGHANDLE_STATE_COUNT (ST_AVCTP_MH_READY+1)

/******************************************EVENT******************************************/
/* the events of the process dispatcher*/
enum {
	/*Uplayer events */
	EV_AVCTP_DISP_CONN_REQ = FSM_EV_BASE,
	EV_AVCTP_DISP_CONN_RSP,
	EV_AVCTP_DISP_DISCONN_REQ,
    EV_AVCTP_DISP_SENDMSG,
    EV_CTP_EVENT_REG,
	
	/*received events from L2CAP*/
	EV_AVCTP_L2CA_CONN_IND,
	EV_AVCTP_L2CA_CONN_CFM,
	EV_AVCTP_L2CA_CONN_COMPLETE,
	EV_AVCTP_L2CA_CONFIG_IND,
	EV_AVCTP_L2CA_DISCONN_IND,
	EV_AVCTP_L2CA_DATA_IND,
	EV_AVCTP_L2CA_DISCONN_CFM

	/*L2CAP channel config timeout*/
	/*EV_CTP_CONFIG_TIMEOUT,	*/
	
};
#define AVCTP_DISPATCHER_EVENT_COUNT (EV_AVCTP_L2CA_DATA_IND+1)
#define AVCTP_DISPATCHER_FN_COUNT (sizeof(avctp_dispatcher_fn_list)/sizeof(struct FsmNode))


/* the events of the process mh*/
enum {
	EV_AVCTP_MH_SENDMSG = FSM_EV_BASE,
	EV_AVCTP_MH_PACKRECIND,			/*packet received indication*/

};
#define AVCTP_MSGHANDLE_EVENT_COUNT (EV_AVCTP_MH_PACKRECIND+1)
#define AVCTP_MSGHANDLE_FN_COUNT (sizeof(avctp_msghandle_fn_list)/sizeof(struct FsmNode))

/*******************************FUNCTIONS DEFINITION****************************************/
/*
 * ------------------------------dispatcher fsm instance ---------------------------------
 */
void DispConnReqInState0(struct FsmInst *fi, UCHAR event, void *arg);
void DispDisconnReqInState0(struct FsmInst *fi, UCHAR event, void *arg);
void DispEventRegReqInState0(struct FsmInst *fi, UCHAR event, void *arg);
void DispConnRspInState0(struct FsmInst *fi, UCHAR event, void *arg);
void DispSendMsgReqInState0(struct FsmInst *fi, UCHAR event, void *arg);

void DispL2CAConnIndInState0(struct FsmInst *fi, UCHAR event, void *arg);
void DispL2CADisconnIndInState0(struct FsmInst *fi, UCHAR event, void *arg);
void DispL2CADataIndInState0(struct FsmInst *fi, UCHAR event, void *arg);
void DispL2CADisconncfmInS0(struct FsmInst *fi, UCHAR event, void *arg);
void DispL2CAConnCpleInS0(struct FsmInst *fi, UCHAR event, void *arg);
void DispL2CAConnCfmInState0(struct FsmInst *fi, UCHAR event, void *arg);

/*void ConfigTimeoutInState0(struct FsmInst *fi, UCHAR event, void *arg);*/
/*
 * ------------------------------msghandle fsm instance ---------------------------------
 */
void MhSendMsgInReady(struct FsmInst *fi, UCHAR event, void *arg);
void MhPackRecInReady(struct FsmInst *fi, UCHAR event, void *arg);


#endif

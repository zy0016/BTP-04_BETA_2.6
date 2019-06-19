/*
***************************************************************************
Module Name:
    avctp.h
Abstract:
	 avctp initialize and done function declaration
Author:
    Qian Lixin
Data:
	2001.7.27
**************************************************************************** 
*/
#ifndef AVCTP_H
#define AVCTP_H


#include "../btcommon/global.h"
#include "../l2cap/g_l2cap.h"
#include "../l2cap/l2capui.h"
#include "../l2cap/l2func.h"

#define AVCTP_OUT_MH_CLOSEMH 0
#define AVCTP_MHINIT_CONN  0
#define AVCTP_MHINIT_IND   1 
#define AVCTP_DECODE_SIG_HEADER   0x09
#define AVCTP_DECODE_START_HEADER 0x25   

/* the state cid info */
#define AVCTP_STATE_NOT_CONNECT 1
#define AVCTP_STATE_CONNECTED 2
#define AVCTP_STATE_CONFIG 3
#define AVCTP_STATE_CONNECTING 4

/* wait handle class id */
#define AVCTP_CLASS_BD_ADDR 1


#define MAXPROFCOUNT 8
#define AVCTP_MAX_INS 7

#define AVCTP_DEFAULT_IN_MTU	672

#include "avctpui.h"


/*callback function provided by uplayer avrcp*/
typedef void (*Callback_ConnectReqInd) (UCHAR*);
typedef void (*Callback_DisconnectInd) (UCHAR*);
typedef void (*Callback_MessageRecInd) (struct AVCTP_DataIndStru *); 
typedef void (*Callback_ReestablishInd) (UCHAR*);
typedef void (*Callback_ConnectReqCfm)(struct AVCTP_ConnCfmStru *);
typedef void (*Callback_DisconnReqCfm)(struct AVCTP_DisconnCfmStru *);
typedef void (*Callback_NeedConnRsp) (UCHAR*);


/*0x09 ---- 0000 1001*/
struct AVCTP_SinglePackHeader {
	UCHAR fb;  /*first byte: bit0(IPID) bit1(C/R) bit2-bit3(packet_type=00) bit4-bit7(trans_lbl)*/
	UINT16  pid;        /*profile id ---two bytes */
};

/*0x25 ---- 0010 0101*/
struct AVCTP_StartPackHeader {
	UCHAR fb;  /*first byte: bit0(IPID) bit1(C/R) bit2-bit3(packet_type=01) bit4-bit7(trans_lbl)*/
	UCHAR num;         /*packet number ---only one byte */   
	UINT16  pid;        /*profile id ---two bytes */
};

struct AVCTP_ProfInfoStru {
	UINT16 pid;
/*	Callback_MessageRecInd 	callback;*/
	Callback_MessageRecInd	msg_ind;
	Callback_ConnectReqInd 	connect_ind;
	Callback_DisconnectInd		disconnect_ind;
	Callback_ReestablishInd	reestablish_ind;
	Callback_ConnectReqCfm	connect_cfm;
	Callback_DisconnReqCfm	disconn_cfm;
	Callback_NeedConnRsp		connect_rsp;
};

/*for message handle instance*/
struct AVCTP_MhUserDataStru {
	WORD cid;			/* cid of this MH */
	UCHAR cur_label;    /* previous transaction label in case more than one transaction cross*/
	UCHAR pack_num;   /* the packet number of fragmented av/c command or response message*/
	UINT16 pid;          /* store profile id*/
	INT16  len;          /* length of received message*/ 
	UCHAR* pMsg;       /* store av/c message*/
	WORD mtu;			/* mtu pertaining to every channel */
};

struct AVCTP_CidInfoStru {
	UCHAR bd_addr[AVCTP_BD_ADDR_LEN]; /*index of cid info list*/
	struct FsmInst* fsm_inst;	/* mh fsm instance */
	UCHAR	state;				/* NOT_CONNECT, CONNECTED, CONNECTING, CONFIG */
	struct BtList* ct_prof_list;     /*connection inititor,not AVRCP CT */
	struct BtList* tg_prof_list;	    /*connection rsponspor , not AVRCP TG*/
};

/* for dispatcher instance */
struct AVCTP_DispatcherUserDataStru {
	struct BtList* 	cid_info_list; 	/* bd_addr table, from bd_addr to find mh instance */
	struct BtList* 	prof_info_list;	/* all event-registered profiles list */
#ifdef CONFIG_CTP_REDUCE_VERSION
	WORD cid;
	WORD mtu;
	struct AVCTP_ProfInfoStru *profile;
	UCHAR state; /* NOT_CONNECT, CONNECTED, CONNECTING, CONFIG */
	UCHAR *pmsg; /*store av/c message*/
	INT16 len;
	UCHAR packetnum;
	UCHAR cur_tl;
	UCHAR bd[AVCTP_BD_ADDR_LEN];
#endif
	UCHAR *pconfig; /* AVCTP_ConnReqInStru from upper layer*/
	UCHAR *p_disconn_req_param;
	UCHAR *p_connect_ind_param;
};

struct AVCTP_ArgConnectIndStru {
	struct L2CAConnIndStru* ind;
	WORD mtu;
};

void AVCT_l2cap_cb(WORD msg,void* arg);

extern struct FsmInst * avctp_fsminst;


#include "avctpcode.h"
#include "avctphlp.h"
#include "avctpfsm.h"


#endif

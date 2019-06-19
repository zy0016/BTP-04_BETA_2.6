/*
***************************************************************************
Module Name:
    g_avctp.h
Abstract:
	Global variable and function declaration 
Author:
    Qian Lixin
Data:
    2001.8.2
**************************************************************************** 
*/

#ifndef _CTP_GLOBAL_H_
#define _CTP_GLOBAL_H_


#define AVCTP_OUT_MH_CLOSEMH 0
#define AVCTP_MHINIT_CONN  0
#define AVCTP_MHINIT_IND   1 
#define AVCTP_DECODE_SIG_HEADER   0x09
#define AVCTP_DECODE_START_HEADER 0x25   


#define MAXPROFCOUNT 8
#define AVCTP_MAX_INS 7
#define AVCT_BUFFER_RESERVE		24




#include "../btcommon/global.h"
#include "../l2cap/g_l2cap.h"
#include "../l2cap/l2capui.h"
#include "../l2cap/l2func.h"


#include "avctp.h"
#include "avctpui.h"
#include "avctpcode.h"
#include "avctphlp.h"


/*0x09 ---- 0000 1001*/
struct SinglePackHeader {
	UCHAR fb;  /*first byte: bit0(IPID) bit1(C/R) bit2-bit3(packet_type=00) bit4-bit7(trans_lbl)*/
	UINT16  pid;        /*profile id ---two bytes */
};

/*0x25 ---- 0010 0101*/
struct StartPackHeader {
	UCHAR fb;  /*first byte: bit0(IPID) bit1(C/R) bit2-bit3(packet_type=01) bit4-bit7(trans_lbl)*/
	UCHAR num;         /*packet number ---only one byte */   
	UINT16  pid;        /*profile id ---two bytes */
};

struct ProfInfoStru {
	UINT16 pid;
	Callback_MessageRecInd callback;     
};

/*for message handle instance*/
struct MhUserDataStru {
	WORD cid;			/* cid of this MH */
	UCHAR cur_label;    /* previous transaction label in case more than one transaction cross*/
	UCHAR pack_num;   /* the packet number of fragmented av/c command or response message*/
	UINT16 pid;          /* store profile id*/
	INT16  len;          /* length of received message*/ 
	UCHAR* pMsg;       /* store av/c message*/
	WORD mtu;			/* mtu pertaining to every channel */
};

struct CidInfoStru {
	UCHAR bd_addr[AVCTP_BD_ADDR_LEN]; /*index of cid info list*/
	struct FsmInst* fsm_inst;	/* mh fsm instance */
	/*UINT16 profcount;*/
};

/* for dispatcher instance */
struct DispatcherUserDataStru {
	struct BtList* cid_info_list; /* bd_addr table, from bd_addr to find mh instance */
};

struct ArgConnectIndStru {
	struct L2CAConnIndStru* ind;
	WORD mtu;
};

extern struct FsmInst * avctp_fsminst;


#include "avctpui.h"
#include "avctpfsm.h"
#include "avctp.h"


#endif

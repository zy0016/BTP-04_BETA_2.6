/*
***************************************************************************
Module Name:
    avctpuistru.h
Abstract:
	avctp ui structure declaration
Author:
    Qian Lixin
Data:
    2001.8.2
**************************************************************************** 
*/
#ifndef _AVCTP_UI_STRU_H
#define _AVCTP_UI_STRU_H

#include "l2capuistru.h"

/*upper layer event defenition*/
#define AVCT_CONN_REQ_CFM					1
#define AVCT_DISCONN_REQ_CFM				2
#define AVCT_SEND_DATA_CFM				3
#define AVCT_CONN_REQ_IND	 				4
#define AVCT_DISCONN_REQ_IND	 			5
#define AVCT_MESSAGE_REC_IND 				6
#define AVCT_ReEstablish_IND 				7
#define AVCT_NEEDCONN_RSP					8

/*AVCT_Connect_Req rsp & AVCT_CONN_CFM ind result macro define*/
#define AVCT_CONNECT_REQ_ACCEPT							1
#define AVCT_CONNECT_REQ_SUCCESS							0
#define AVCT_CONNECT_REQ_CHANNEL_EXIST					2
#define AVCT_CONNECT_REQ_CHANNEL_ESTABLISHING			3
#define AVCT_CONNECT_REQ_EXCEED_MAX						4
#define AVCT_CONNECT_REQ_L2CAP_ERROR						5

/*AVCT_DisconnectReq*/
#define AVCT_DISCONN_REQ_ACCEPT							1
#define AVCT_DISCONN_REQ_SUCCESS							0

/*AVCT_SendMessage macro*/
#define AVCT_SENDMSG_OK									0
#define AVCT_SENDMSG_ERROR								1
#define AVCT_SENDMSG_NOCHANNEL							2
#define AVCT_SENDMSG_PROFILE_NOTREG						3
/*AVCT_connect _rsp macro*/
#define AVCT_CONNRSP_ACCEPT								0
#define AVCT_CONNRSP_ERROR								2
/*Other Reserved for future use*/
#ifndef UINT16
#define UINT16 unsigned short
#endif

#define AVCTP_BD_ADDR_LEN 6

struct AVCTP_EventIndInStru {
	UINT16 	pid;	
	WORD 	event;
	UCHAR	*callback;
};

struct AVCTP_ConfigInStru {
	WORD in_mtu;
	struct QosStru out_flow; 
	WORD flush_to;
	WORD link_to;
};

struct AVCTP_ConnReqInStru {
	UCHAR bd_addr[AVCTP_BD_ADDR_LEN];
	UINT16  pid;
	struct AVCTP_ConfigInStru config_param;	/* only mtu at present */
};

struct AVCTP_ConnReqOutStru {
	UINT16 rsp;
	UINT16 res;
	UINT16 st;
};

struct AVCTP_ConnRspInStru {
	UCHAR bd_addr[AVCTP_BD_ADDR_LEN];
	UINT16 res;				/* no use at present */
	UINT16 st;				/* no use at present */
	struct AVCTP_ConfigInStru config_param;	/* only mtu at present */
};

struct AVCTP_DisconnReqInStru {
	UCHAR bd_addr[AVCTP_BD_ADDR_LEN];
	UINT16  pid;
};


struct AVCTP_MsgInfoStru {
	UCHAR bd_addr[AVCTP_BD_ADDR_LEN];
	UCHAR tl;       /*transaction label*/
	UCHAR msg_type; /*message type: 1 is command, 2 is response*/
	UINT16 pid;     /*profile id*/ 
	UINT16 len;     /*data length*/ 
	UCHAR  data[1]; /*av/c command or response message*/
};

struct AVCTP_DataIndStru {
	UCHAR bd_addr[AVCTP_BD_ADDR_LEN];
	UCHAR tl;
	UCHAR msg_type; /*message type: 1 is command, 2 is response*/
	UINT16 len;
	UCHAR* data;
};

struct AVCTP_ConnCfmStru{
	UCHAR bd[AVCTP_BD_ADDR_LEN];
	WORD conn_result;	//l2cap result
	WORD config_result;	//avctp result
	WORD status;		//no status now. 0
};

struct AVCTP_DisconnCfmStru{
	UCHAR bd[AVCTP_BD_ADDR_LEN];
	WORD result;
};


#endif

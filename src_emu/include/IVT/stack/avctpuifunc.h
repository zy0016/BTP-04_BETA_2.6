/*
***************************************************************************
Module Name:
    avctpuifunc.h
Abstract:
	avctp ui function declaration
Author:
    Qian Lixin
Data:
    2001.8.2
**************************************************************************** 
*/
#ifndef _AVCTP_UI_FUNC_H
#define _AVCTP_UI_FUNC_H

/* Initialization and destruction functions */
void AvctpInit(void);
void AvctpDone(void);

/*It registers the user call back functions that handle the indications. */
FUNC_EXPORT void  AVCT_EventRegistration(struct AVCTP_EventIndInStru* in_param);

/*direct service calls provided to upper layers */
FUNC_EXPORT void AVCT_Connect_Req(struct AVCTP_ConnReqInStru *in);
FUNC_EXPORT void AVCT_ConnectRsp(struct AVCTP_ConnRspInStru *in);
FUNC_EXPORT void AVCT_DisconnectReq(struct AVCTP_DisconnReqInStru *in);
FUNC_EXPORT void AVCT_SendMessage(struct AVCTP_MsgInfoStru *msg);




#endif 

/*
*********************************************************************************************************
*                                                Abstract
*                                          
*		This file declare the user interface function provided for the upper layer
*
* File : rfuifunc.h
* By   : Zhang Tao
*********************************************************************************************************
*/

#ifndef __RFUIFUNC_H
#define __RFUIFUNC_H

/*
*********************************************************************************************************
*                                              for bluelet
*********************************************************************************************************
*/
int  RFInit(void);
void RFDone(void);

/*
*********************************************************************************************************
*                                              for l2cap and hci
*********************************************************************************************************
*/
void RF_L2Cbk(WORD msgid, void *arg);
void RF_SecuCfm(struct SMSecuReqStru *secu_req);

/*
*********************************************************************************************************
*                                              for spp
*********************************************************************************************************
*/
void RF_DlcEstabReq(struct RF_DlcEstabReqStru *par);
void RF_DlcEstabRsp(struct RF_DlcEstabRspStru *par);
void RF_DlcRelReq(struct RF_DlcRelStru *par);
void RF_PnReq(struct RF_PnStru *par);
void RF_PnRsp(struct RF_PnStru *par);
void RF_RpnReq(struct RF_RpnStru *par);
void RF_RpnRsp(struct RF_RpnStru *par);
void RF_CtrlReq(struct RF_CtrlStru *par);
void RF_RlsReq(struct RF_RlsStru *par);
void RF_TestReq(struct RF_TestStru *par);
void RF_Credit(struct RF_CreditStru *par);
void RF_DataReq(struct RF_DataReqStru *par);
#endif

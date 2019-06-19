#ifndef HCRP_UIFUNC_H
#define HCRP_UIFUNC_H

#include "../l2cap/l2capuistru.h"

void HCRP_CtrlChnl_L2Cbk(WORD msgid, void *arg);
void HCRP_DataChnl_L2Cbk(WORD msgid, void *arg);
void HCRP_NotifChnl_L2Cbk(WORD msgid, void *arg);

/*l2cap indication functions*/
void HCRP_L2CAConnInd(struct L2CAConnIndStru *ind);
void HCRP_L2CAConfigInd(struct L2CAConfigIndStru *ind);
void HCRP_L2CADiscInd(WORD *cid);
void HCRP_L2CAQosViaInd(UCHAR dev[]);
void HCRP_L2CADataInd( struct DataIndStru *ind);

/* request functions*/
FUNC_EXPORT void HCRP_ConnReq(struct HCRP_ConnReqInStru* in, struct HCRP_ConnReqOutStru* out);
FUNC_EXPORT void HCRP_DiscReq(WORD* hdl,WORD* result);
FUNC_EXPORT void HCRP_DataReq(struct HCRP_DataWriteInStru* in,struct HCRP_DataWriteOutStru* out);
FUNC_EXPORT void HCRP_CreditGrantReq(struct HCRP_CreditGrantReqInStru* in,WORD* result);
FUNC_EXPORT void HCRP_CreditReq(WORD* hdl, struct HCRP_CreditOutStru *out);
FUNC_EXPORT void HCRP_CreditReturnReq(struct HCRP_CreditReturnReqInStru* in,struct HCRP_CreditOutStru *out);
FUNC_EXPORT void HCRP_CreditQueryReq(WORD* hdl, struct HCRP_CreditOutStru *out);
FUNC_EXPORT void HCRP_LPTStatusReq(WORD* hdl, struct HCRP_LPTStatusReqOutStru *out);
FUNC_EXPORT void HCRP_1284IDReq(struct HCRP_1284IDReqInStru* in, struct HCRP_1284IDReqOutStru *out);
FUNC_EXPORT void HCRP_SoftResetReq(WORD* hdl, WORD* result);
FUNC_EXPORT void HCRP_HardResetReq(WORD* hdl, WORD* result);
FUNC_EXPORT void HCRP_NotifRegReq(struct HCRP_NotifRegReqInStru* in,  struct HCRP_NotifRegReqOutStru* out);
FUNC_EXPORT void HCRP_NotifAliveReq(WORD* hdl, struct HCRP_NotifAliveReqOutStru *out);

/* response functions*/
FUNC_EXPORT void HCRP_CreditRsp(struct HCRP_CreditRspInStru* in, WORD* result);
FUNC_EXPORT void HCRP_CreditReturnRsp(struct HCRP_CreditReturnRspInStru* in, WORD* result);
FUNC_EXPORT void HCRP_LPTStatusRsp(struct HCRP_LPTStatusRspInStru* in, WORD* result);
FUNC_EXPORT void HCRP_1284IDRsp(struct HCRP_1284IDRspInStru* in, WORD* result);

/* notification related functions*/
#ifdef	CONFIG_HCRP_NOTIF
FUNC_EXPORT void HCRP_NotifDiscReq(UCHAR* bdaddr, WORD* result);
FUNC_EXPORT void HCRP_NotifReq(struct HCRP_NotifReqInStru* in, WORD* result);
FUNC_EXPORT void HCRP_NotifAddTimeout(struct HCRP_NotifAddTimeoutStru* in, WORD* result);
#endif

/*init and done functions*/
WORD HCRP_Init(void);
void HCRP_Done(void);
FUNC_EXPORT void HCRP_SetControlParam(struct HCRP_SetControlParamInStru* in);
FUNC_EXPORT WORD HCRP_GetActiveConn(void);
#endif

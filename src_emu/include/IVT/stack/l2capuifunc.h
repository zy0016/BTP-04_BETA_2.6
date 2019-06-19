#ifndef L2CAPUIFUNC_H
#define L2CAPUIFUNC_H

/*L2cap Initialize and done*/
void L2capInit(void);
void L2capDone(void);

/*callback for HCI*/
void HciConnectCfm(WORD hdl, UCHAR* bd_addr,UCHAR result);
void HciConnectInd(UCHAR* bd_addr);
void HciDisconnectInd(UCHAR* bd_addr);
void HciQosViolationInd(UCHAR* bd_addr);
void HciDataInd(struct BuffStru* buf);
void L2capSecCfm(struct SMSecuReqStru* sc);
void HciSecurityInd(struct SMSecuReqStru* sc, UCHAR s);


/*These are exported UI function implementing the l2cap service primitives*/
FUNC_EXPORT void EventIndication(WORD psm,UCHAR* cb);
FUNC_EXPORT void L2CA_ConnectReq(struct L2CAConnReqInStru* in_param);
FUNC_EXPORT UCHAR L2CA_ConnectRsp(struct L2CAConnRspInStru* in_param);
FUNC_EXPORT UCHAR L2CA_ConfigReq(struct L2CAConfigReqInStru* in_param);
FUNC_EXPORT UCHAR L2CA_ConfigRsp(struct L2CAConfigRspInStru* in_param);
FUNC_EXPORT UCHAR L2CA_DisconnectReq(WORD* cid);
FUNC_EXPORT UCHAR L2CA_DataWrite(struct L2CADataWriteInStru* in_param);
FUNC_EXPORT UCHAR L2CA_DataRead(struct L2CADataReadInStru* in_param);
FUNC_EXPORT void L2CA_Ping(struct L2CAPingInStru* in_param);
FUNC_EXPORT void L2CA_GetInfo(struct L2CAGetInfoInStru* in_param);
FUNC_EXPORT void L2CA_GroupCreate(WORD* psm);
FUNC_EXPORT UCHAR L2CA_GroupClose(WORD* cid);
FUNC_EXPORT UCHAR L2CA_GroupAddMember(struct L2CAGroupAddInStru* in_param);
FUNC_EXPORT UCHAR L2CA_GroupRemoveMember(struct L2CAGroupRemoveInStru* in_param);
FUNC_EXPORT UCHAR L2CA_GroupMembership(WORD* cid);
FUNC_EXPORT UCHAR L2CA_DisableCLT(WORD* psm);
FUNC_EXPORT UCHAR L2CA_EnableCLT(WORD* psm);
FUNC_EXPORT void L2CA_DisconnACLLink(UCHAR* bd);

/*additional UI function*/
FUNC_EXPORT void UnRegister(WORD psm);
FUNC_EXPORT UCHAR IVT_DataWrite(struct BuffStru* buf);
FUNC_EXPORT UCHAR IVT_DataWrite_Internal(struct BuffStru* buf);
FUNC_EXPORT void IVT_GetPSM(struct GetPSMStru* in_param, WORD* psm);
FUNC_EXPORT UCHAR IVT_Check_L2caChannel(UCHAR* bd_addr,WORD psm);

/*Reserved functions*/
FUNC_EXPORT void L2_SetTestFlag(WORD* flag,WORD* result);
FUNC_EXPORT void L2CA_DisconnectRsp(WORD* cid,WORD* result);
FUNC_EXPORT void L2_Tester_ErrorPack(struct ErrorStru* err,WORD* result);
FUNC_EXPORT CHAR* GetBlueletVersion(void);


#endif

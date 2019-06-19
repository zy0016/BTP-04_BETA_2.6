/*
***************************************************************************
Module Name:
    avctphlp.h
Abstract:
	Auxiliary function declaration 
Author:
    Qian Lixin
Data:
    2001.8.2
**************************************************************************** 
*/

#ifndef CTP_HLP_H
#define CTP_HLP_H

/* Register CTP callback functions for L2CAP */
void RegisterAVCTPToL2CAP(void);

/*Auxiliary function*/
int AvctpDispInit(void * user_data);
struct AVCTP_CidInfoStru* AvctpMhInit(UCHAR* user_data, UCHAR* bd_addr,UCHAR flag);

struct AVCTP_MhUserDataStru* AVCTP_InitMhUserData(WORD cid, WORD mtu);
void AVCTP_CloseMh(struct FsmInst *fsm_inst, UCHAR* bd_addr, UCHAR flag);
int AVCTP_WriteDataToL2cap(WORD cid,struct BuffStru *buf,UINT16 len);
struct AVCTP_CidInfoStru * AVCTP_FindCidInfo(WORD cid);
UCHAR AVCTP_CalcNum(INT16 len, WORD mtu);
int AVCTP_IsRegistered(UINT16 pid);
void AVCTP_FreeCidInfo(struct AVCTP_CidInfoStru *info);
int AVCTP_AddNewProfile(struct AVCTP_CidInfoStru *cid_info, UCHAR target, UINT16 pid);
int AVCTP_IsConnectRegistered(struct AVCTP_CidInfoStru *cid_info, UINT16 pid);
void AVCTP_DecodeSinglePack(UCHAR *qp,UINT16 *qd);
void AVCTP_DecodeStartPack(UCHAR *qp,UCHAR *qn,UINT16 *qd) ;
#endif

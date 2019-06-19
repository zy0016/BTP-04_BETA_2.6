/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    l2func.h
Abstract:
	This file is not used for L2CAP. It is a common file for SDP,RFCOMM and TCS.
	This file can only be added to the Bluelet project one time.
Author:
    Xu Fei
Revision History:2000.4
---------------------------------------------------------------------------*/

#ifndef L2FUNC_H
#define L2FUNC_H

extern struct BtList* cblist;
struct ChnInfoStru{
	WORD cid;
	WORD in_mtu;
	WORD out_mtu;
	struct FsmTimer* ft;
	struct L2CAConfigReqInStru* cfgreq;
	struct L2CAConfigRspInStru* cfgrsp;
	UCHAR side;
	UCHAR bd[BDLENGTH];
};
struct ConfTOStru{
	struct ChnInfoStru* info;
	WORD psm;
};

struct CBInfoStru{
	WORD psm;
	UCHAR* cb;
};
struct ConnCfmStru{
	WORD result;  
	WORD cid;
	WORD mtu;
	WORD side; 
	WORD psm;
	UCHAR bd[BDLENGTH];
};

#define L2CAP_CHANNEL_INITIATOR	0
#define L2CAP_CHANNEL_RESPONDER	1

#define L2CONN_SUCCESS			0	/*success */
#define L2CONN_PENDING			1	/*pending*/
#define L2CONN_ERR_PSM			2	/*refused because error psm*/
#define L2CONN_SECURITY_FAIL	3	/*refused because security*/
#define L2CONN_TIMEOUT			0xEEEE	/*timeout occurred*/
#define L2CONN_INVALID_PARAM	4	/*invalid parameters*/
#define L2CONN_CONFIGREQ_FAIL	5	/*configreq be rejected*/
#define L2CONN_CONFIGIND_FAIL	6	/*reject the configuration from remote device*/
#define L2CONN_CONFIG_TIMEOUT	7	/*configuration timeout*/

#define CONFIG_TIMEOUT			5000


void L2Func_Init(void);
void L2Func_Done(void);
void L2CAPRegisterCB(WORD psm, UCHAR* func);
void L2CAPConnect(WORD psm,UCHAR* bd);
void L2CAPConfig(WORD cid, UCHAR* bd, struct L2CAConfigReqInStru* creq, struct L2CAConfigRspInStru* crsp, UCHAR side);
void L2capConnCfmMsg(WORD psm,struct L2CAConnReqOutStru* conn,struct L2CAConfigReqInStru* creq,struct L2CAConfigRspInStru* crsp);
void L2capConfigMsg(WORD psm, WORD msgid, void* arg);

void L2capConfigTO(UCHAR* arg);
#if 1
void SendL2CAPConnectCfm(WORD p,WORD c,UCHAR* d,WORD m,WORD r,WORD s) ;
#endif

#if 0
#define SendL2CAPConnectCfm(p,c,d,m,r,s) {\
	struct ConnCfmStru* cfm;\
	struct CBInfoStru* cbmem;\
	cbmem = (struct CBInfoStru*)NewListFind(cblist,(UCHAR*)(&p),sizeof(WORD));\
	if (cbmem) {\
		cfm = (struct ConnCfmStru*)NEW(sizeof(struct ConnCfmStru));\
		cfm->result = r;\
		cfm->cid = c;\
		cfm->mtu = m;\
		memcpy(cfm->bd,d,BDLENGTH);\
		cfm->side = s;\
		cfm->psm = p;\
		(*(FuncL2CAEventCB*)(cbmem->cb))(L2CAP_CONNECTCFM,(UCHAR*)cfm);\
	}\
}
#endif

#define FreeChnlInfo(f){\
	if (f->ft)\
		FsmDelTimer2(f->ft);\
	if (f->cfgreq)\
		FREE((UCHAR*)f->cfgreq);\
	if (f->cfgrsp)\
		FREE((UCHAR*)f->cfgrsp);\
	NewListRemoveAt(linkinfolist,(UCHAR*)f);\
	BtListMemberFree((UCHAR*)f);\
}

#endif

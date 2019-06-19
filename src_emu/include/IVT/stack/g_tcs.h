/*
Contain globle veriables macros and include files for TCS
*/

#ifndef  G_TCS_H
#define  G_TCS_H


#include "global.h"
#include "hci_ui.h"
#include "l2capui.h"
#include "l2func.h"
#include "tcsfsm.h"
#include "tcsui.h"
#include "tcshlp.h"
#include "tcscode.h"

#define TCS_BROADCAST_BD 0
#define TCS_MAX_INS 8
#define INVALID_CID 0

#define T_ALERTING          0
#define T_CALLPROCEED       1
#define T_CONN              2
#define T_CONNACK           3
#define T_PROGRESS          4 
#define T_SETUP             5
#define T_SETUPACK          6
#define T_DISCONN           7
#define T_RELEASE           8
#define T_RELEASECOMP       9
#define T_INFO             10
#define T_START_DTMF		16
#define T_START_DTMF_ACK	17
#define T_START_DTMF_REJ	18
#define T_STOP_DTMF			19
#define T_STOP_DTMF_ACK		20

#define T_INFO_SUGGEST    32
#define T_INFO_ACCEPT     33
#define T_LISTEN_REQUEST  34
#define T_LISTEN_ACCEPT   35 
#define T_LISTEN_SUGGEST  36
#define T_LISTEN_REJECT    37
#define T_RIGHTS_REQ       38
#define T_RIGHTS_ACCEPT    39
#define T_RIGHTS_REJECT    40

#define T_CL_INFO           64

#define TCS_T301_TIMEOUT    180000L
#define TCS_T302_TIMEOUT    15000L/*15000L*/
#define TCS_T303_TIMEOUT    200000L/*20000L*/
#define TCS_T304_TIMEOUT    30000L/*30000*/
#define TCS_T305_TIMEOUT    30000L
#define TCS_T308_TIMEOUT    4000L
#define TCS_T310_TIMEOUT    120000L/*20000*/
#define TCS_T313_TIMEOUT    40000L /* 4000 */  //zzb, 03/02/24,for debug
#define TCS_T401_TIMEOUT	8000L /* 8000 */
#define TCS_T403_TIMEOUT	4000L /* 4000 */
#define TCS_T404_TIMEOUT	2500L /* 3000 */
#define TCS_T405_TIMEOUT	2000L /* 2000 */
#define TCS_T406_TIMEOUT	20000L
/* timer name */
enum{
	T301,
	T302,
      T303,
      T304,
	T305,
	T308,
	T313,
	T310,
	T401,
	T403,
	T404,
	T405,
	T406
};

#define TCS_OUT_CC_CLOSECC	0
#define TCS_IN_CC_CLOSECC	1

/* for CC instance */
struct CcUserDataStru {
//#ifdef CONFIG_TCS_GW
	struct BtList* active_dev;	/* broadcast receiver list */
//#endif
	WORD cid;			/* cid of this cc */
	WORD handle;		/* if bear type is voice, indicates SCO handle; otherwise, served L2Cap logical channel (ACL) identifier */
	UCHAR bd_addr[BDLENGTH];	/* remote baseband address */
	UCHAR flag;			/* bit0: bearstatus, 0: bear cleared; 1: bear connected */
						/* bit1: broadcast or not, 0: not broadcast; 1: broadcast */
						/* bit2: bear_type, 0: data call; 1: voice call */
						/* bit3,4: call class, (1,0):intercom call; (0,0):external call */
	UCHAR pkt_type;
};

#define CLEAR_BROADCAST_FLAG(t) (t->flag&=0xFD)
#define SET_BROADCAST_FLAG(t) (t->flag|=0x02)
#define CLEAR_BEAR(t) (t->flag&=0xFE)
#define CONN_BEAR(t) (t->flag|=0x01)
#define SET_CALL_CLASS(t,a) t->flag=(t->flag&0xE7)|(a<<3)
#define SET_VOICE_CALL(t) (t->flag|=0x04)
#define SET_DATA_CALL(t) (t->flag&=0xFB)
#define IS_BEAR_CONNECTED(flag) (flag&1)
#define IS_BROADCAST(flag) (flag&2)
#define IS_VOICE(flag) (flag&4)
#define CALL_CLASS(flag) ((flag&0x18)>>3)
#define CALL_INTERCOM 1
#define CALL_EXTERNAL 0

struct TcsGCreateReqInfoStru{
	WORD cid;
	UCHAR bd_addr[BDLENGTH];
};

struct TcsScoConnReqInfoStru{
	UCHAR bd[BDLENGTH];
	WORD cid;
	UCHAR progress_flag;
	struct FsmInst *fi;
	struct CompanySpecStru comp;
};

struct TcsClkOffReqInfoStru{
	WORD clk_off;
	UCHAR rsp_flag;
	struct FsmInst * fi;
	struct CompanySpecStru comp;
};

struct CidInfoStru{
	WORD cid;					/* channel identifier */
	struct FsmInst* fsm_inst;	/* cc fsm instance */
	UCHAR scohdl;				/* Sco handle */
};

struct GroupMemInfoStru {
	WORD cid;
	UCHAR bd_addr[BDLENGTH];
};

struct GroupInfoStru {
	UCHAR total;
	UCHAR num;
	struct GroupMemInfoStru cidlist[1];
};
#define CID_BLOCK 3

/* for dispatcher instance */
struct DispatcherUserDataStru {
	WORD gcid; /* group cid */
	struct BtList* cid_info_list; /* cid table, from cid to find cc instance */
//#ifndef CONFIG_TCS_GW
	WORD gatewaycid;	/* the cid connected to the gateway only exist in TL */
	UCHAR gw_bd_addr[BDLENGTH]; /* bd_addr of the gateway */
//#else
	struct GroupInfoStru * group;
//#endif
};

#ifdef CONFIG_TCS_GM
/* for gm unit instance */
struct GmMemberInfoStru{
    char inter_numb[2];
    UCHAR link_key[LINK_KEY_LEN];
//#ifndef CONFIG_TCS_GW
    UCHAR bd_addr[BDLENGTH];
//#else
    WORD cid;
//#endif
};

struct GmUserDataStru{
    UCHAR config;    
    /*gm_config:
        bit0:info_accept;
        others:reserved;
    */
//#ifdef CONFIG_TCS_GW    
    WORD  orig_cid;
    WORD  remote_cid;
    UCHAR InfoIsUpdate;	//If RightsRequest arrive while Gateway is distributing
    			//configuration information, distribution be interrupted,
    			//then information is out of data in some members; 
    			//After response is made to interrrupter, distribution
    			//should be resumed or restarted.
    struct BuffStru* tmp_info_pdu;
    struct GmMemberInfoStru*  distribution_cusor;
//#else
    UCHAR* remote_bd_addr;
    UCHAR  tmp_pgscan_enable;
//#endif
    struct CompanySpecStru tmp_comp;
    struct BtList* member_info_list;
};

struct GmDataIndParamStru{
	WORD cid;
	void* arg;
};
#endif

//#ifdef CONFIG_TCS_GW
struct GroupInfoStru *  AddGroupMember(struct GroupInfoStru * group, WORD cid, UCHAR * bd_addr);
struct GroupInfoStru *  RmGroupMember(struct GroupInfoStru * group, WORD cid);
WORD FindGroupMemberCid(struct GroupInfoStru * group, UCHAR * bd_addr);
UCHAR * FindGroupMemberBdAddr(struct GroupInfoStru * group, WORD cid);

#define CreateGroup(g) \
{ \
	UCHAR i; \
	\
	i=(sizeof(struct GroupInfoStru))+CID_BLOCK*sizeof(struct GroupMemInfoStru);\
	g=NEW(i);\
	memset(g,0,i);\
	g->total = CID_BLOCK;\
}
//#endif

struct CcMsgType{
	WORD  msg_type;				/* the PDU type							*/
	WORD  length;					/* the size of the PDU					*/
    UCHAR  content[1];	/* the content of the PDU				*/
};

extern struct BtList	* Tcs_ChnlConnReq_list;
extern struct TcsClkOffReqInfoStru * g_Tcs_ClkOffReqInfo;

extern struct BtList	* Tcs_ScoConnReq_list;

extern struct TcsGCreateReqInfoStru * g_TcsGCreateReqInfo;

extern struct FsmInst * tcs_fsminst;
extern struct FsmInst* gm_inst;
extern WORD g_sco_hdl ;
extern struct CompanySpecStru*			g_comp_null;
extern FuncPHONE_Callback * g_tcs_callback;

#define TCS_GRP_MGN_CMD     32
#endif



#ifndef L2CAPUISTRU_H
#define L2CAPUISTRU_H

#define DEFAULTMTUcno 672
#define DEFAULTFLUSHTO 0xFFFF
#define DEFAULTFLOWSPEC(QoS) {	memset(&QoS,0,sizeof(struct QosStru));\
								QoS.flags = 0; \
								QoS.service_type = 1; \
								QoS.token_rate = 0; \
								QoS.token_b_size = 0; \
								QoS.peak_bandwidth = 0; \
								QoS.latency = 0xFFFFFFFF; \
								QoS.delay_variation = 0xFFFFFFFF;}

struct QosStru {
	UCHAR flags;
	UCHAR service_type;
	ULONG token_rate;
	ULONG token_b_size;
	ULONG peak_bandwidth;
	ULONG latency;
	ULONG delay_variation;
};

#define L2CAP_L2CA_CONNIND		1
#define L2CAP_L2CA_CONFIGIND	2
#define L2CAP_L2CA_DISCONNIND	3
#define L2CAP_L2CA_QOSVIND		4
#define L2CAP_L2CA_DATAIND			5
#define L2CAP_L2CA_TIMEOUTIND		6
#define L2CAP_L2CA_CONNCFM		7	
#define L2CAP_L2CA_CONNRSPCFM	8
#define L2CAP_L2CA_CONFIGCFM	9
#define L2CAP_L2CA_CONFIGRSPCFM	10
#define L2CAP_L2CA_DISCONNCFM	11
#define L2CAP_L2CA_WRITECFM		12
#define L2CAP_L2CA_READCFM		13
#define L2CAP_L2CA_G_CREATECFM	14
#define L2CAP_L2CA_G_ADDCFM		15
#define L2CAP_L2CA_G_REMOVECFM	16
#define L2CAP_L2CA_G_MEMSHIPCFM	17
#define L2CAP_L2CA_G_CLOSECFM	18
#define L2CAP_L2CA_PINGCFM		19
#define L2CAP_L2CA_INFOCFM		20
#define L2CAP_CONNECTCFM		30


#define L2CAP_TEST				1
#define L2CAP_TEST_CFG_FLAG		2
#define L2CAP_TEST_CFGTO		3
#define L2CAP_TEST_DISRSP		4

#define PEER_DISCONNECT_LINK		1
#define LP_LINK_LOSS				2
#define LOCAL_DISCONNCT				3


struct L2CAConnReqInStru {
	WORD psm_value;
	UCHAR bd_addr[BDLENGTH];
};

struct L2CAConnReqOutStru {
	WORD channel_id;
	WORD result;
	WORD status;
	WORD destination_cid;
	WORD psm;
	UCHAR bd[BDLENGTH];
};

struct L2CAConnRspInStru {
	UCHAR bd_addr[BDLENGTH];
	UCHAR identifier;
	WORD channel_id;
	WORD response;
	WORD status;
};

struct L2CAConnRspOutStru {
	WORD cid;
	WORD result;
};

struct L2CAConnIndStru {
	UCHAR bd_addr[BDLENGTH];
	WORD channel_id;
	WORD psm_value;
	UCHAR identifier;
	WORD destination_cid;
};

struct L2CADisConnOutStru {
	WORD cid;
	WORD result;
};

struct  L2CADisconnIndStru{
	WORD cid;
	WORD reason;
};

struct L2CAConfigReqInStru {
	WORD channel_id;
	WORD in_mtu;
	struct QosStru out_flow; 
	WORD flush_to;
	WORD link_to;
};

struct L2CAConfigReqOutStru {
	WORD result;
	WORD in_mtu;
	struct QosStru out_flow; 
	WORD flush_to;
	WORD cid;
};

struct L2CAConfigRspInStru {
	WORD channel_id;
	WORD out_mtu;
	struct QosStru in_flow; 
};

struct L2CAConfigRspOutStru {
	WORD cid;
	WORD result;
};

struct L2CAConfigIndStru {
	WORD channel_id;
	WORD out_mtu;
	struct QosStru in_flow; 
	WORD flush_to;
};

struct L2CADataWriteInStru {
	WORD channel_id;
	WORD length;
	UCHAR* buf;
};

struct L2CADataWriteOutStru {
	WORD size;
	WORD result;
	WORD cid;
};

struct L2CADataReadInStru {
	WORD channel_id;
	WORD length;
	UCHAR* buf;
};

struct L2CADataReadOutStru {
	WORD result;
	WORD n;
	WORD cid;
};

struct L2CAGroupCreateOutStru{
	WORD psm;
	WORD cid;
};

struct L2CAGroupAddInStru {
	WORD cid;
	UCHAR bd_addr[BDLENGTH];
};

struct L2CAGroupAddOutStru{
	WORD cid;
	WORD result;
};

struct L2CAGroupRemoveInStru {
	WORD cid;
	UCHAR bd_addr[BDLENGTH];
};

struct L2CAGroupRemoveOutStru{
	WORD cid;
	WORD result;
};

struct L2CAGroupMemOutStru {
	WORD cid;
	WORD result;
	WORD n;
	UCHAR* bd_list;
};

struct L2CAGroupDisableOutStru{
	WORD psm;
	WORD result;
};

struct L2CAGroupEnableOutStru{
	WORD psm;
	WORD result;
};


struct L2CAPingInStru {
	WORD psm;
	UCHAR bd[BDLENGTH];
	UCHAR* data;
	WORD length;
};

struct L2CAPingOutStru {
	 WORD result;
	 UCHAR* data;
	 WORD size;
	 UCHAR bd[BDLENGTH];
};

struct L2CAGetInfoInStru {
	WORD psm;
	UCHAR bd[BDLENGTH];
	WORD type;
	
};

struct L2CAGetInfoOutStru {
	 WORD result;
	 UCHAR* data;
	 WORD size;
	 UCHAR bd[BDLENGTH];
};

struct DataIndStru {
	WORD cid;
	WORD length;
	UCHAR* data;
};

struct HciConnCfmStru {
	UCHAR bd[BDLENGTH];
	WORD hci_hdl;
	UCHAR result;
};

/*reserved structure*/

/*#ifdef CONFIG_ANALYZER*/
struct GetPSMStru{
	WORD cid;
	WORD flag;
};
#define DIR_SEND	0
#define DIR_RECEIVE	1
/*#endif*/

/*#ifdef CONFIG_CONFORMANCE_TESTER*/
struct TimeOutIndStru{
	WORD cid;
	WORD t_timer;
};
/*for timeout_ind*/

struct ConfigRspReqStru {
	WORD channel_id;
	WORD out_mtu;
	struct QosStru in_flow; 
	WORD cid;
	WORD in_mtu;
	struct QosStru out_flow; 
	WORD flush_to;
	WORD link_to;
};

struct ErrorStru {
	WORD length;
	WORD type;
	UCHAR bd_addr[BDLENGTH];
	UCHAR* pack;
};

#define ERR_CNODATA 3
/*#endif*/
typedef void (FuncL2CAEventCB) (WORD msg,void* arg);


#endif

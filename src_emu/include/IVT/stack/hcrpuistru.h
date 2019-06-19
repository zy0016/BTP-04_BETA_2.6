#ifndef HCRP_UISTRU_H
#define HCRP_UISTRU_H

/*request functions related structure definitions*/
struct HCRP_SetControlParamInStru {
	WORD cpsm;
	WORD dpsm;
	WORD npsm;
	UCHAR flag;	/*server or client*/
};

struct HCRP_ConnReqInStru {
	WORD  cpsm;
	WORD  dpsm;
	UCHAR bd_addr[6];
};
struct HCRP_ConnReqOutStru {
	WORD result;
	WORD hdl;
	WORD cmtu;
	WORD dmtu;
};

struct HCRP_DataWriteInStru {
	WORD hdl;
	WORD len;
	UCHAR* buf;
};
struct HCRP_DataWriteOutStru {
	WORD result;
	WORD off;	/*data len sent*/
};

struct HCRP_CreditGrantReqInStru {
	WORD hdl;
	DWORD credit;
};
struct HCRP_CreditReturnReqInStru {
	WORD hdl;
	DWORD credit;
};
struct HCRP_CreditOutStru{
	WORD result;
	DWORD credit;
};

struct HCRP_LPTStatusReqOutStru{
	WORD result;
	UCHAR status;
};

struct HCRP_1284IDReqInStru {
	WORD hdl;
	WORD start;
	WORD number;
};
struct HCRP_1284IDReqOutStru{
	WORD result;
	WORD len;
	UCHAR *retbuf;
};

struct HCRP_NotifRegReqInStru {
	WORD hdl;
	UCHAR flag;		/*reg or unreg*/
	DWORD id;		/*call back context id*/
	DWORD cbtmt;	/*call back timeout*/
};
struct HCRP_NotifRegReqOutStru{
	WORD result;
	DWORD timeout;
	DWORD callbacktimeout;
};

struct HCRP_NotifAliveReqOutStru{
	WORD result;
	DWORD timeoutincrement;
};

/*response functions related structure definitions*/
struct HCRP_CreditRspInStru {
	WORD hdl;
	WORD tranid;
	DWORD credit;
	WORD code;		/*status code*/
};

struct HCRP_CreditReturnRspInStru {
	WORD hdl;
	WORD tranid;
	DWORD credit;
	WORD code;
};

struct HCRP_LPTStatusRspInStru {
	WORD hdl;
	WORD tranid;
	WORD code;
	UCHAR status;
};

struct HCRP_1284IDRspInStru {
	WORD hdl;
	WORD tranid;
	WORD code;
	WORD len;
	UCHAR* buf;
};

//notification function related structure definition;
struct HCRP_NotifAddTimeoutStru {
	DWORD tmtadd;
	UCHAR bdaddr[6];
};
struct HCRP_NotifReqInStru {
    DWORD cbcid;
    DWORD conntmt;
    DWORD regtimerid;
    UCHAR bdaddr[6];
    WORD npsm;
};

/*structures definitions for indications */
struct HCRP_ConnIndStru {
	UCHAR bdaddr[6];
	WORD hdl;
	WORD mtu;
	UCHAR flag;
};
struct HCRP_DataIndStru {
	UCHAR* buf;
	WORD hdl;
	WORD len;
	UCHAR role;
};
struct HCRP_DiscIndStru {
	WORD hdl;
	UCHAR role;
	UCHAR active;
};
struct HCRP_CreditGrantIndStru {
	DWORD credit;
	WORD hdl;
};
struct HCRP_CreditReqIndStru {
	WORD hdl;
	WORD tranid;
};
struct HCRP_CreditReturnIndStru {
	DWORD credit;
	WORD hdl;
	WORD tranid;
};
struct HCRP_CreditQueryIndStru {
    DWORD credit;
    WORD hdl;
};
struct HCRP_LPTStatusIndStru {
	WORD hdl;
	WORD tranid;
};
struct HCRP_1284IDIndStru {
	WORD hdl;
	WORD tranid;
	WORD start;
	WORD number;
};
struct HCRP_NotifRegIndStru {
	DWORD cbcid;
	DWORD cbtmt;
	DWORD rtid;
	WORD hdl;
};
struct HCRP_NotifAliveIndStru {
	DWORD tmtadd;
	WORD hdl;
};
struct HCRP_NotifIndStru {
	DWORD id;
	UCHAR bdaddr[6];
};

struct HCRP_ConnCfmStru {
	UCHAR 	bd_addr[BD_ADDR_LEN];	
	WORD	connect_result;
};


#endif

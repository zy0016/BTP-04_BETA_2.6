
#ifndef SM_FSM_H
#define SM_FSM_H

#include "global.h"
/*ui*/



#define SM_MODE_1 0x1
#define SM_MODE_2 0x2
#define SM_MODE_3 0x3

#define SM_AUTHEN			1
#define SM_AUTHOR			2
#define SM_ENCRY			4

/*used by l2cap*/
#define SM_FLAG_NONE		0
#define SM_FLAG_AUTHEN		1
#define	SM_FLAG_AUTHOR		2
#define	SM_FLAG_ENCRY		3

struct SMServInfoStru {
	UCHAR proto_id;
	WORD channel;
	UCHAR sl;
};

struct SMSecUnRegStru {
	UCHAR proto_id;
	WORD channel;
};

struct SMSecuReqStru {
	UCHAR bd_addr[BDLENGTH];
	UCHAR proto_id;
	WORD channel;
	UCHAR is_incoming;
	UCHAR result;
	void* cb;
};

typedef void (FuncSecuCfm)(struct SMSecuReqStru*);

void SM_Init(void);
void SM_Done(void);
void SM_SecuReg(struct SMServInfoStru *secu_reg);
void SM_SecuUnReg(struct SMSecUnRegStru *secu_unreg);
void SM_SecuReq(struct SMSecuReqStru *secu_req);
void SM_SetTrusted(UCHAR *bd_addr);
void SM_ClearTrusted(UCHAR *bd_addr);
void SM_SetSecuMode(UCHAR secu_mode);
void SM_GetSecuMode(UCHAR *secu_mode);
void SM_Complete(UCHAR* bd,UCHAR r,UCHAR flag);
#define SM_RcvAuthenComp(bd_addr,result)		SM_Complete(bd_addr,result,SM_FLAG_AUTHEN)
#define SM_RcvAuthorComp(bd_addr,result)		SM_Complete(bd_addr,result,SM_FLAG_AUTHOR)
#define SM_RcvEncryComp(bd_addr,result)			SM_Complete(bd_addr,result,SM_FLAG_ENCRY)


/*help*/
typedef void (FuncHciAuthenEncryReq)(UCHAR *bd_addr);
typedef void (FuncHciAuthorReq)(UCHAR *bd_addr, UCHAR proto, WORD channel);

struct SecuInfoStru {
	void* func;
	DWORD timer;
	UCHAR str[MAX_TIMER_NAME_LEN];
};

struct SMDevInfoStru {
	UCHAR bd_addr[BDLENGTH];
	UCHAR is_trusted;
};

struct SMStru {
	struct SMSecuReqStru *secu_req;
	struct BtList *req_list;
	UCHAR secu_level;
	UCHAR status;
};

#define SM_WAIT_AUTHEN_TIMEOUT	60000
#define SM_WAIT_AUTHOR_TIMEOUT	30000
#define SM_WAIT_ENCRY_TIMEOUT	15000

#endif

#ifndef SM_UI_H
#define SM_UI_H

#define SM_MODE_1 0x1
#define SM_MODE_2 0x2
#define SM_MODE_3 0x3

#define SL_AUTHEN	0x1
#define SL_AUTHOR	(0x2 | SL_AUTHEN)
#define SL_ENCRY	(0x4 | SL_AUTHEN)

#define SEC_NOPENDING					0/*00000000*/
#define SEC_AUTHENTICATION_PENDING		1/*00000001*/
#define SEC_AUTHORISATION_PENDING		2/*00000010*/

struct SMSecuRegStru {
	UCHAR proto_id;
	WORD channel;
	UCHAR sl;
	UCHAR index;
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
};

#define SM_UPDATE_REG				1
#define SM_UPDATE_UNREG				2
#define SM_UPDATE_SET_TRUSTED		3
#define SM_UPDATE_CLEAR_TRUSTED		4
#define SM_UPDATE_SET_SECU_MODE		5

struct SMUpdateStru {
	UCHAR op;
	void *param;
};

UCHAR SM_Init(void);
UCHAR SM_Done(void);
UCHAR SM_SecuReg(struct SMSecuRegStru *secu_reg);
UCHAR SM_SecuUnReg(struct SMSecUnRegStru *secu_unreg);
UCHAR SM_SecuReq(struct SMSecuReqStru *secu_req);
UCHAR SM_SetTrusted(UCHAR *bd_addr);
UCHAR SM_ClearTrusted(UCHAR *bd_addr);
UCHAR SM_RcvAuthenComp(UCHAR *bd_addr, UCHAR result);
UCHAR SM_RcvAuthorComp(UCHAR *bd_addr, UCHAR result);
UCHAR SM_RcvEncryComp(UCHAR *bd_addr, UCHAR result);
UCHAR SM_SetSecuMode(UCHAR secu_mode);
UCHAR SM_GetSecuMode(UCHAR *secu_mode);

#endif

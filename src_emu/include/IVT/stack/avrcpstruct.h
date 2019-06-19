/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
filename:
	avrcpstruct.h
descprition:
	avrcp struct and macro define.
copywrite:
	IVT Corporation.


$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$*/
#ifndef AVRCP_STRUCT_H
#define AVRCP_STRUCT_H
/*
	new code is added here.
*/
#define AVRCP_CT						0x01/*  controller */
#define AVRCP_TG						0x02/* target   */

/* indications to app */
#define AVRCP_IND_CONN					0x01
#define AVRCP_IND_DISCONN				0x02
#define AVRCP_IND_UNITINFO				0x03
#define AVRCP_IND_SUBUNITINFO			0x04
#define AVRCP_IND_PASSTHROUGH			0x05
#define AVRCP_IND_VENDORDEP			0x06
/*for asyn avrcp ui*/
#define AVRCP_IND_CONN_CFM			0x07

#define AVRCP_RSP_UNITINFO				0x07
#define AVRCP_RSP_SUBUNITINFO			0x08
#define AVRCP_RSP_PASSTHROGH			0x09
#define AVRCP_RSP_VENDORDEP			0x0a

/* error codec */
#define AVRCP_ERROR_BASE				0x10
/*cann't send cmd or rsp when rcp wait for a rsp of last cmd*/
#define AVRCP_ERROR_STATEERROR		AVRCP_ERROR_BASE + 1;

typedef void (AVRC_Cbk)(UCHAR event,UCHAR *param);

struct AVRCP_CallBackStru{
	UINT16 pid;
	UCHAR *con_ind;
	UCHAR *discon_ind;
	UCHAR *reestablish_ind;
	UCHAR *msg_ind;
	UCHAR *conn_cfm;
	UCHAR *disconn_cfm;
	UCHAR *conn_rsp;
};

/* used for encode */
struct EncPassStru{
	UCHAR subunit_id;
	UCHAR state_flag;
	UCHAR op_id;
	UCHAR length; /*of vendor unique */
	UCHAR op_data[1];
};


struct EncVendorStru{
	UINT16 len;
	UCHAR ctype;
	UCHAR company_id[3];
	UCHAR vendor_data[1];
};
/*typedef struct avrcp_publicinfor
{
	
	
}AVRCP_PUBINFO;*/

#ifndef MIN
#define MIN(a,b) (((DWORD)a>(DWORD)b)?b:a)
#endif

#endif

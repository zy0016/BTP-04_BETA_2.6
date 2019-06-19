/******************************************************************************
*	Module Name:
*		avrcpui.h
*	Description:
*		interfaces declaration of struct and functions for avctp and mmi. 
*	Created By:
*		penghui
*	Date
*		August 13th, 2001
******************************************************************************/

#ifndef RCP_UI_H
#define RCP_UI_H
#include "avctpui.h"
#include "avrcpstruct.h"

#define AVRCP_BD_ADDR_LEN 6
#define AVRCP_BD_CLASS_LEN 3


/* AV/C Panel Commands operation_id*/
#define AVC_PANEL_SELECT 						0x00
#define AVC_PANEL_UP					 		0x01
#define AVC_PANEL_DOWN 						0x02
#define AVC_PANEL_LEFT 							0x03
#define AVC_PANEL_RIGHT 						0x04
#define AVC_PANEL_RIGHT_UP						0x05			
#define AVC_PANEL_RIGHT_DOWN					0x06
#define AVC_PANEL_LEFT_UP						0x07
#define AVC_PANEL_LEFT_DOWN					0x08
#define AVC_PANEL_ROOTMENU 					0x09
#define AVC_PANEL_SETUP_MENU					0x0a
#define AVC_PANEL_CONTENTS_MENU				0x0b
#define AVC_PANEL_FAVORITE_MENU				0x0c
#define AVC_PANEL_EXIT							0x0d

#define AVC_PANEL_0								0x20
#define AVC_PANEL_1								0x21
#define AVC_PANEL_2								0x22
#define AVC_PANEL_3								0x23
#define AVC_PANEL_4								0x24
#define AVC_PANEL_5								0x25
#define AVC_PANEL_6								0x26
#define AVC_PANEL_7								0x27
#define AVC_PANEL_8								0x28
#define AVC_PANEL_9								0x29
#define AVC_PANEL_DOT							0x2a
#define AVC_PANEL_ENTER						0x2b
#define AVC_PANEL_CLEAR						0x2c

#define AVC_PANEL_CHANNEL_UP 					0x30
#define AVC_PANEL_CHANNEL_DOWN 				0x31
#define AVC_PANEL_PREVIOUS_CHANNEL			0x32
#define AVC_PANEL_SOUND_SELECT				0x33
#define AVC_PANEL_INPUT_SELECT				0x34
#define AVC_PANEL_DISPLAY_INFORMATION		0x35
#define AVC_PANEL_HELP							0x36
#define AVC_PANEL_PAGE_UP						0x37
#define AVC_PANEL_PAGE_DOWN					0x38

#define AVC_PANEL_POWER 						0x40
#define AVC_PANEL_VOLUME_UP 					0x41
#define AVC_PANEL_VOLUME_DOWN 				0x42
#define AVC_PANEL_MUTE							0x43
#define AVC_PANEL_PLAY 							0x44
#define AVC_PANEL_STOP 							0x45
#define AVC_PANEL_PAUSE 						0x46
#define AVC_PANEL_RECORD 						0x47
#define AVC_PANEL_REWIND 						0x48
#define AVC_PANEL_FAST_FORWARD 				0x49
#define AVC_PANEL_EJECT 						0x4a
#define AVC_PANEL_FORWARD 					0x4b
#define AVC_PANEL_BACKWARD 					0x4c

#define AVC_PANEL_ANGLE						0x50
#define AVC_PANEL_SUBPICTURE					0x51
#define AVC_PANEL_F1							0x52
#define AVC_PANEL_F2							0x53
#define AVC_PANEL_F3							0x54
#define AVC_PANEL_F4							0x55
#define AVC_PANEL_F5							0x56

#define AVC_PANEL_VENDORUNIQUE 				0x7e

/* AV/C command types */
#define AVC_CTYPE_CONTROL 0x00
#define AVC_CTYPE_STATUS 0x01
#define AVC_CTYPE_SPECIFIC_INQUIRY 0x02
#define AVC_CTYPE_NOTIFY 0x03
#define AVC_CTYPE_GERNERAL_INQUIRY 0x04

/* AV/C response type*/
#define AVC_RSP_NOT_IMPLEMENTED 0x08
#define AVC_RSP_ACCEPTED 0x09
#define AVC_RSP_REJECTED 0x0A
#define AVC_RSP_IN_TRANSITION 0x0B
#define AVC_RSP_IMPLEMENTED 0x0C
#define AVC_RSP_CHANGED 0x0D
#define AVC_RSP_INTERIM 0x0F


/* AV/C subunit types */
#define AVC_SUBTYPE_VIDEO_MONITOR 0
#define AVC_SUBTYPE_DISC_RECORDER  3
#define AVC_SUBTYPE_TAPE_RECORDER 4
#define AVC_SUBTYPE_TUNER 5
#define AVC_SUBTYPE_VIDEO_CAMERA 7
#define AVC_SUBTYPE_VENDOR_UNIQUE 0x1C
#define AVC_SUBTYPE_EXTENDED 0x1E
#define AVC_SUBTYPE_UNIT 0x1F
#define AVC_SUBTYPE_PANEL 9

/* AV/C subunit IDs */
#define AVC_SUBID_0 0 
#define AVC_SUBID_1 1
#define AVC_SUBID_2 2 
#define AVC_SUBID_3 3 
#define AVC_SUBID_4 4 
#define AVC_SUBID_EXTENDED 5 
#define AVC_SUBID_IGNORE 7 

/* AV/C Unit commands */
#define AVC_COM_UNIT_INFO 0x30
#define AVC_COM_SUBUNIT_INFO 0x31


/* AV/C Common unit and subunit commands */
#define AVC_COM_OBJECT_NUMBER_SELECT 0x0D
#define AVC_COM_POWER 0xB2
#define AVC_COM_RESERVE 0x01
#define AVC_COM_PLUG_INFO 0x02
#define AVC_COM_VENDOR_DEPENDENT 0

/* AV/C subunit commands */
#define AVC_COM_PASS_THROUGH 0x7C

/*AVRCP ERROR CODE*/
#define AVRC_SERVICE_ERROR_CODE_BASE 				0x02
#define AVRC_ERROR_SDAP_OPENFAIL					AVRC_SERVICE_ERROR_CODE_BASE + 1
#define AVRC_ERROR_SDAP_GETSVCFAIL				AVRC_SERVICE_ERROR_CODE_BASE + 2


/* IVT Corporation vendordep cmd format and ids*/
/* transmit parameter to Cfm and Ind */
struct UnitInfoStru{
	DWORD cbsize;/*must be filled*/
	UCHAR tl;    // // transaction label, both CT & TG; 0 < tl < 0x0F
	UCHAR bd_addr[AVRCP_BD_ADDR_LEN];
	UCHAR response;
	UCHAR unit_type;
	UCHAR company_id[3];
};

struct SubUnitInfoStru{
	DWORD cbsize;/*must be filled*/
	UCHAR tl; // 0 < tl < 0x0F
	UCHAR bd_addr[AVRCP_BD_ADDR_LEN];
	UCHAR response;
	UCHAR page;
	UCHAR page_data[4];	
};

struct PassThrStru{
	DWORD cbsize;/*must be filled, size of total struct , both CT & TG*/
	UCHAR tl;        // transaction label, both CT & TG ; 0 < tl < 0x0F
	UCHAR bd_addr[AVRCP_BD_ADDR_LEN]; // peer device address both CT & TG
	UCHAR response;	 //response of command(such as AVC_RSP_ACCEPTED), only TG 
	UCHAR subunit_id; // sub unit ID, both CT & TG
	UCHAR state_flag;  // button state(0: pressed 1: released)
	UCHAR op_id;        // pass through command ID
	UCHAR length;	// param length
	UCHAR op_data[1]; // param 
};

struct VendorDepStru{
	DWORD cbsize;/* must be filled*/
	UCHAR tl; /*transaction lable , should be  0 < tl < 0x0F*/
	UCHAR bd_addr[AVRCP_BD_ADDR_LEN];
	UCHAR response; /*response type: 1 is interim     0 is others*/
	UCHAR company_id[3];
	UINT16 length;
	UCHAR vendor_data[1];
	
};

/*
#define UnitInfoReqStru bd_addr
*/
struct UnitInfoReqStru{
	UCHAR tl;/*transaction lable , should be  0 < tl < 0x0F*/
	UCHAR bd_addr[AVRCP_BD_ADDR_LEN];
};

struct SubUnitInfoReqStru {
	UCHAR tl;
	UCHAR bd_addr[AVRCP_BD_ADDR_LEN];
	UCHAR req_page;
	
};

#define PassThrReqStru PassThrStru
#define VendDepReqStru VendorDepStru

/*for sync avrcp ui only*/
struct AVRCP_ConnCfmStru{
	UCHAR bd[AVCTP_BD_ADDR_LEN];
	WORD conn_result;	//BT_SUCCESS or BT_FAIL
	WORD res;	
	WORD res1;		
};

//#include "avrcpcode.h"

void AVRC_ConnectReq(UCHAR *bd_addr, UINT16 *result);
void AVRC_DisconnectReq(UCHAR *bd_addr, UINT16 *result);
void AVRC_UnitInfoReq(/*in*/struct UnitInfoReqStru *pinStru, UINT16 *result,struct UnitInfoStru *pOutStru);
void AVRC_SubUnitInfoReq(struct SubUnitInfoReqStru * info_req, UINT16 *result,struct SubUnitInfoStru *pOutStru);
void AVRC_PassThroughReq(struct PassThrReqStru *pass_req, UINT16 *result,struct PassThrStru *pOutStru);
void AVRC_VendorDepReq(struct VendDepReqStru *vend_req, UINT16* result,struct VendorDepStru *pOutStru);



void AVRC_ConnectRsp(UCHAR *bd_addr, UINT16 *result);
void AVRC_UnitInfoRsp(struct UnitInfoStru *info, UINT16 *result);
void AVRC_SubUnitInfoRsp(struct SubUnitInfoStru *info, UINT16 *result);
void AVRC_PassThroughRsp(struct PassThrStru * rsp, UINT16 *result);
void AVRC_VendorDepRsp(struct VendorDepStru *rsp, UINT16 *result);


extern void AVRC_UnitInfoInd(struct UnitInfoStru *ind);
extern void AVRC_SubUnitInfoInd(struct SubUnitInfoStru *ind);
extern void AVRC_PassThroughInd(struct PassThrStru *ind);
extern void AVRC_VendorDepInd(struct VendorDepStru *ind);

extern void AVRC_ConnectInd(UCHAR* bd_addr);

void AVRC_CTPConnectInd(UCHAR *bd_addr);


/* AVCTP indication */
void AVRC_CTPDisconnectInd(UCHAR *bd_addr);
void AVRC_CTPMessageRecInd(struct AVCTP_DataIndStru * msg);
void AVRC_CTPConneCfm(struct AVCTP_ConnCfmStru *pcfm);
void AVRC_CTPReConnInd(UCHAR *bd);
void AVRC_CTPDisconnCfm(struct AVCTP_DisconnCfmStru *pcfm);
void AVRC_CTPNeedConneRsp(UCHAR *bd);

UCHAR AVRC_Start(UCHAR svr_cls);
UCHAR AVRC_Config(void);
UCHAR AVRC_RegSvrToSDP(UCHAR svr_cls);
UCHAR AVRC_UnRegSvr(void);
UCHAR AVRC_Stop(void);
UCHAR AVRC_Init(void);
UCHAR AVRC_Done(void);
UCHAR AVRC_RegCbk(UCHAR *pfun);

void AVRC_DefaultPassThrCmdRsp(struct PassThrStru *ind);
void AVRC_DefaultUnitCmdRsp(struct UnitInfoStru *ind);
void AVRC_DefaultSubUnitCmdRsp(struct SubUnitInfoStru *ind);

UCHAR AVRC_SearchSvrInfo(UCHAR *bd,struct SDAP_AVRCPInfoStru *pinfo);

#endif

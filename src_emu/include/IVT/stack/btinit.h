#ifndef BTINIT_H
#define BTINIT_H

#include "autoconf.h"
#include "datatype.h"

struct AppEvConn {
	WORD clsid;
	UCHAR bd[6];	/* Remote bd address */
	UCHAR* sub_hdl; /*this is for PAN, each PAN client connection's handle*/
};
#define IS_SERVEREV(a)	(a&0x8000)
#define APP_EVENT(a)	(a&0x7fff)
#define SERVER_EV(a)	(a|0x8000)
#define BTAPP_EV_CONN		0x01	/* Connection is setup. Param is a pointer to AppEvConn structure*/
#define BTAPP_EV_DISC		0x02	/* Connection is disconected. Param is not used */
#define BTAPP_EV_START		0x03	/* Start server.  Param is not used */
#define BTAPP_EV_STOP		0x04	/* Stop server. Param is not used */
#define BTAPP_EV_PREV_DISC	0x05
#define BTAPP_EV_SEND		0x05
#define BTAPP_EV_RECEIVE	0x06

/* PAN specific event */
#define BTAPP_EV_PAN_BASE		0x100
#define BTAPP_EV_PAN_CONN		(BTAPP_EV_PAN_BASE+0)
#define BTAPP_EV_PAN_DISC		(BTAPP_EV_PAN_BASE+1)
/* decide if we are IVT NAP, param is not used, if we are not IVT NAP, return 0, else 1 */
#define BTAPP_EV_PAN_IS_IVTNAP	(BTAPP_EV_PAN_BASE+2) 
/* config indication for IVT NAP, param is a pointer to pan_nap_config structure, if we are not IVT nap, return 0, else 1 */
#define BTAPP_EV_PAN_NAP_CONFIG (BTAPP_EV_PAN_BASE+3) 

/* OPP specific event */
#define BTAPP_EV_OPP_BASE       0x200
#define BTAPP_EV_OPP_CONN		(BTAPP_EV_OPP_BASE+0)
#define BTAPP_EV_OPP_DISC		(BTAPP_EV_OPP_BASE+1)
#define BTAPP_EV_OPP_PULL		(BTAPP_EV_OPP_BASE+2)
#define BTAPP_EV_OPP_PUSH		(BTAPP_EV_OPP_BASE+3)
#define BTAPP_EV_OPP_PUSH_CARD	(BTAPP_EV_OPP_BASE+4)
#define BTAPP_EV_OPP_EXCHG		(BTAPP_EV_OPP_BASE+5)

/* FTP specific event */
#define BTAPP_EV_FTP_BASE		0x300
#define BTAPP_EV_FTP_PUT		(BTAPP_EV_FTP_BASE+0)
#define BTAPP_EV_FTP_GET        (BTAPP_EV_FTP_BASE+1)
#define BTAPP_EV_FTP_BROWSE		(BTAPP_EV_FTP_BASE+2)
#define BTAPP_EV_FTP_DEL_FILE   (BTAPP_EV_FTP_BASE+3)
#define BTAPP_EV_FTP_DEL_FODLER	(BTAPP_EV_FTP_BASE+4)	
#define BTAPP_EV_FTP_CREATE		(BTAPP_EV_FTP_BASE+5)
#define BTAPP_EV_FTP_SET_CUR	(BTAPP_EV_FTP_BASE+6)
#define BTAPP_EV_FTP_SET_ROOT	(BTAPP_EV_FTP_BASE+7)
#define BTAPP_EV_FTP_ABORT		(BTAPP_EV_FTP_BASE+8)
#define BTAPP_EV_FTP_BACK		(BTAPP_EV_FTP_BASE+9)
#define BTAPP_EV_FTP_CONN		(BTAPP_EV_FTP_BASE+10)

/* SYNC specific event */
#define BTAPP_EV_SYNC_BASE		0x400
#define BTAPP_EV_SYNC_START		(BTAPP_EV_SYNC_BASE+0)
#define BTAPP_EV_SYNC_CHALOG	(BTAPP_EV_SYNC_BASE+1)
#define BTAPP_EV_SYNC_DEVINFO	(BTAPP_EV_SYNC_BASE+2)
#define BTAPP_EV_SYNC_CC		(BTAPP_EV_SYNC_BASE+3)
#define BTAPP_EV_SYNC_INFOLOG	(BTAPP_EV_SYNC_BASE+4)
#define BTAPP_EV_SYNC_STREAM	(BTAPP_EV_SYNC_BASE+5)
#define BTAPP_EV_SYNC_LUID		(BTAPP_EV_SYNC_BASE+6)
#define BTAPP_EV_SYNC_OK		(BTAPP_EV_SYNC_BASE+7)
#define BTAPP_EV_SYNC_ADD		(BTAPP_EV_SYNC_BASE+8)
#define BTAPP_EV_SYNC_MODIFY	(BTAPP_EV_SYNC_BASE+9)
#define BTAPP_EV_SYNC_DELETE	(BTAPP_EV_SYNC_BASE+10)
#define BTAPP_EV_SYNC_PREV_CHALOG	(BTAPP_EV_SYNC_BASE+11)


/* HCRP specific event */
#define BTAPP_EV_HCRP_BASE			0x500
#define BTAPP_EV_HCRP_NOTIF			(BTAPP_EV_HCRP_BASE+1)
#define BTAPP_EV_HCRP_REG_NOTIF		(BTAPP_EV_HCRP_BASE+2)
#define BTAPP_EV_HCRP_GET_LPTSTAT   (BTAPP_EV_HCRP_BASE+3)
#define BTAPP_EV_HCRP_GET_1284ID    (BTAPP_EV_HCRP_BASE+4)
#define BTAPP_EV_HCRP_SOFT_RESET    (BTAPP_EV_HCRP_BASE+5)
#define BTAPP_EV_HCRP_HARD_RESET    (BTAPP_EV_HCRP_BASE+6)
#define BTAPP_EV_HCRP_DATA          (BTAPP_EV_HCRP_BASE+7)
#define BTAPP_EV_HCRP_RECEIVE_DATA  (BTAPP_EV_HCRP_BASE+8)
#define BTAPP_EV_HCRP_SEND_DATA     (BTAPP_EV_HCRP_BASE+9)


/* HID specific event */
#define BTAPP_EV_HID_BASE 			0x600
#define BTAPP_EV_HID_CONN		(BTAPP_EV_HID_BASE+1)
#define BTAPP_EV_HID_UNPLUG		(BTAPP_EV_HID_BASE+2)

/* BIP specific event */
#define BTAPP_EV_BIP_BASE			0x700
#define BTAPP_EV_BIP_START_ARCHIVE	(BTAPP_EV_BIP_BASE+0)
#define BTAPP_EV_BIP_ADV_PRINT		(BTAPP_EV_BIP_BASE+1)
#define BTAPP_EV_BIP_START_PRINT	(BTAPP_EV_BIP_BASE+2)
#define BTAPP_EV_BIP_GET_MON		(BTAPP_EV_BIP_BASE+3)
#define BTAPP_EV_BIP_STORE_IMG		(BTAPP_EV_BIP_BASE+4)
#define BTAPP_EV_BIP_REM_DISP		(BTAPP_EV_BIP_BASE+5)
#define BTAPP_EV_BIP_CONN			(BTAPP_EV_BIP_BASE+6)
#define BTAPP_EV_BIP_DISCONN		(BTAPP_EV_BIP_BASE+7)
#define BTAPP_EV_BIP_ABORT			(BTAPP_EV_BIP_BASE+8)
#define BTAPP_EV_BIP_NORMAL			(BTAPP_EV_BIP_BASE+9)

#define BTAPP_MAX_EVENT			10000

typedef UCHAR (StatusInd)(WORD event, UCHAR * hdl, UCHAR * param);

void BtInitProfile(void);
void BtDoneProfile(void);
void BtInitEnv(void);
void BtInitStack(void);
void BtDoneStack(void);

void BtInit(void);
void BtDone(void);

typedef UCHAR * HDLTYPE;

#ifdef CONFIG_SDAP
HDLTYPE BtStartServer(WORD svc_cls, UCHAR * local_attrib);
DWORD BtStopServer(WORD svc_cls, HDLTYPE handle);
HDLTYPE BtStartClient(WORD svc_cls, UCHAR * bd, UCHAR * sdp_attrib, UCHAR * local_attrib);
DWORD BtStopClient(WORD svc_cls, HDLTYPE handle);
void BtSvrGetStatus(WORD cls, UCHAR * hdl, UCHAR * status);
#endif

#ifdef CONFIG_GAP
#include "gap_ui.h"
#else
/* GAP does not have other funtion used in bluelet initialize and finish*/
#endif

#ifdef CONFIG_SDAP
#include "sdap_ui.h"
#else
/* SDAP does not have other funtion used in bluelet initialize and finish*/
#endif

#ifdef CONFIG_SPP
#include "spp_ui.h"
#else
#define	SPP_Init()
#define SPP_Done()
#define SPP_SvrStart(a) 0
#define SPP_SvrStop(a)	0
#define SPP_ClntStart(a,b,c) 0
#define SPP_ClntStop(a)	0
#define SPPAPP_RegCbk(a, b) 0
#endif

#ifdef CONFIG_LAP
#include "lap_ui.h"
#else
#define LAP_Init() 
#define LAP_Done() 
#define LAP_SvrStart(a) 0
#define LAP_SvrStop(a)	0
#define LAP_ClntStart(a,b,c) 0
#define LAP_ClntStop(a)	0
#define LAPAPP_RegCbk(a, b) 0
#endif

#ifdef CONFIG_DUN
#include "dun_ui.h"
#else
#define DUN_Init()	0
#define DUN_Done()	0
#define DUN_SvrStart(a) 0
#define DUN_SvrStop(a)	0
#define DUN_ClntStart(a,b,c) 0
#define DUN_ClntStop(a)	0
#define DUNAPP_RegCbk(a, b) 0
#endif

#ifdef CONFIG_FAX
#include "fax_ui.h"
#else
#define FAX_Init()	0
#define FAX_Done()	0
#define FAX_SvrStart(a) 0
#define FAX_SvrStop(a)	0
#define FAX_ClntStart(a,b,c) 0
#define FAX_ClntStop(a)	0
#define FAXAPP_RegCbk(a, b) 0
#endif

#ifdef CONFIG_GOEP
#include "goep_ui.h"
#else
#define	GOEP_Init()
#define	GOEP_Done()
#endif

#ifdef CONFIG_OPP
#include "opp_ui.h"
#else
/* OPP does not have other funtion used in bluelet initialize and finish*/
#define OPP_SvrStart(a)			0
#define OPP_SvrStop(a)			0
#define OPP_ClntStart(a,b,c)	0
#define OPP_ClntStop(a)			0
#endif

#ifdef CONFIG_FTP
#include "ftp_ui.h"
#else
/* FTP does not have other funtion used in bluelet initialize and finish*/
#define FTP_SvrStart(a)			0
#define FTP_SvrStop(a)			0
#define FTP_ClntStart(a,b,c)	0
#define FTP_ClntStop(a)			0
#endif

#ifdef CONFIG_IRMC
#include "sync_ui.h"
#else
/* SYNC does not have other funtion used in bluelet initialize and finish*/
#define SYNC_SvrStart(a)		0
#define SYNC_SvrStop(a)			0
#define SYNC_ClntStart(a,b,c)	0
#define SYNC_ClntStop(a)		0
#endif

#ifdef CONFIG_BIP
#include "img_ui.h"
#else
#define IMG_Init()				0
#define IMG_Done()				0
#define IMG_ClntStart(a,b,c)	0
#define IMG_ClntStop(a)			0
#define IMG_SvrStart(a)			0
#define IMG_SvrStop(a)			0
#endif

#ifdef CONFIG_CTP_ICP
#include "phone_ui.h"
#else
#define Phone_Init()
#define Phone_Done()
#define CTP_SvrStart(a)			0
#define CTP_SvrStop(a)			0
#endif

#ifdef CONFIG_PAN
#include "pan_ui.h"
#else
#define PAN_Init()
#define PAN_Done()
#define PAN_SvrStart(a)			0
#define PAN_SvrStop(a)			0
#define PAN_ClntStart(a,b,c)	0
#define PAN_ClntStop(a)			0
#endif

#ifdef CONFIG_HCRP
#include "hcrp_ui.h"
#else
#define HCRPP_Init()                0
#define HCRPP_Done()
#define HCRPP_ClntStart(a,b,c)	    0
#define HCRPP_Write(a,b,c)          0
#define HCRPP_ClntStop(a)		    0
#define HCRPPAPP_RegCbk(a,b)    
#define HCRPP_GetVersion()          0
#define HCRPP_GetStatus(a)          0
#define HCRPP_GetSvr1284ID(a,b)     0
#define HCRPP_GetSvrLPTStatus(a,b)  0
#define HCRPP_SoftResetSvr(a)       0
#define HCRPP_HardResetSvr(a)       0
#define HCRPP_RegisterNotif(a,b)    0
#define HCRPP_SvrStart(a)           0
#define HCRPP_SvrStop(a)            0
#define HCRPP_Notif()               0
#define HCRPP_SetSvrMode(a)         0
#endif

#ifdef CONFIG_HID
#include "hidp_ui.h"
#else
#define HIDP_Init()		0
#define HIDP_Done()		0
#define HIDP_AppRegCbk()	0
#define HIDP_ClntStart(a,b,c)	0
#define HIDP_ClntStop(a)	0
#define HIDP_SvrInit()		0
#define HIDP_SvrDone()		0
#define HIDP_SvrAppRegCbk(a,b)	0
#define HIDP_SvrStart(a)	0
#define HIDP_SvrStop(a)		0
#endif

#ifdef CONFIG_HEP_AG
#include "hep_ui.h"
#else
#define HEP_AG_ClntStart(a,b,c)	    0
#define HEP_AG_ClntStop(a)			0
#define HEP_AG_SvrStart(a)			0 
#define HEP_AG_SvrStop(a)			0
#define HEP_AG_SendAlert(a)			0
#define HEP_AG_RelScoConn()			0
#define HEP_AG_EstScoConn()			0
#define HEP_AG_MMIAdjMicVol(a,b)	0
#define HEP_AG_MMIAdjSpkVol(a,b)	0
#define HEP_AG_CallbackRegister(a)	0
#endif

#ifdef CONFIG_AVCTP
#include "avctpui.h"
#else
#define AvctpInit()
#define AvctpDone()
#endif

#ifdef CONFIG_AVRCP
#include "avrcpui.h"
#else
#define AVRC_Init()				0
#define AVRC_Done()				0
#define AVRC_Start(a)				0
#define AVRC_Stop()				0
#endif

#ifdef CONFIG_A2DP
#include "a2dputil_ui.h"
#else
#define A2DPAPP_Init()				0
#define A2DPAPP_Done()
#define A2DPAPPSNK_SvrStart(a)		0
#define A2DPAPPSNK_SvrStop(a)		0
#define A2DPAPPSRC_ClntStart(a,b,c)	0
#define A2DPAPPSRC_ClntStop(a)		0
#define A2DPAPP_RegCbk(a,b)			0
#endif

#ifdef CONFIG_HFP
#include "hfp_ui.h"
#else
#define HFP_Init()			0
#define HFP_Done()          0
#endif

#endif

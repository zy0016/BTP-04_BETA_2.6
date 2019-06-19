#ifndef LAP_UI_H
#define LAP_UI_H

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    lap_ui.h
Abstract:
	This file is the header file of exported API from LAP profile.
Author:
    CHEN ZHONGYI
Revision History:2002.2
			
---------------------------------------------------------------------------*/


struct LAP_InfoStru {
	UCHAR	vport;			/* the number of virtual port returned by SPP_AddEntry		*/
	UCHAR	svr_side;		/* the role of LAP, if 1, LAP server; otherwise, LAP client	*/
	UCHAR	svr_chnl;		/* if svrside is not 1, it means the server channel of LAP	*/
	UCHAR	com_index;		/* Windows system's virtual serial port number				*/
							/* service on remote device; otherwise, the server channel	*/
							/* of LAP service on local device							*/
	UCHAR	sent_flag;		/* flag for us to check "CLIENT" and send "CLIENTSERVER"	*/
	DWORD	svc_hdl;		/* the handle of LAP service record on local SDP database	*/
};

struct CbkStru {
	UCHAR	*hdl;			/* The handle returned by LAP_ClntStart() or LAP_SvrStart()	*/
	UCHAR	*cbk;			/* the pointer to a StatusInd callback function				*/
};

BOOL LAP_Init(void);
void LAP_Done(void);
void LAP_CBFunc(WORD event,UCHAR *param);
void LAPAPP_RegCbk(UCHAR *hdl,UCHAR *pfunc);
const char *LAP_GetVersion(void);

#ifdef CONFIG_LAP_CLIENT

UCHAR *LAP_ClntStart(UCHAR *bd,UCHAR *lap_attrib,UCHAR *local_attrib);
UCHAR LAP_ClntStop(UCHAR *lap_info);

#else

#define LAP_ClntStart(a,b,c)	0
#define LAP_ClntStop(a)			0

#endif

#ifdef CONFIG_LAP_SERVER

UCHAR* LAP_SvrStart(UCHAR *local_attrib);
UCHAR LAP_SvrStop(UCHAR *lap_info);

#else

#define LAP_SvrStart(a)			0
#define LAP_SvrStop(a)			0

#endif

#endif

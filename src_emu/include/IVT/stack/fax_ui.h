#ifndef FAX_UI_H
#define FAX_UI_H

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    fax_ui.h
Abstract:
	This file is the header file of exported API from FAX profile.
Author:
    CHEN ZHONGYI
Revision History:2002.2
			
---------------------------------------------------------------------------*/

extern struct BtList *fax_list;

struct FAX_CbkStru {
	UCHAR	*hdl;
	UCHAR	*cbk;
};

struct FAX_DataStru {
	WORD	length;
	UCHAR	*buf;
};

BOOL FAX_Init(void);
void FAX_Done(void);
void FAXAPP_RegCbk(UCHAR *hdl,UCHAR *pfunc);
const char *FAX_GetVersion(void);

#ifdef CONFIG_FAX_CLIENT

struct FAX_ClntInfo {
	UCHAR	vport;		/* the number of virtual port returned by SPP_AddEntry	*/
	UCHAR	svr_chnl;	/* the server channel of FAX service on remote device	*/
	UCHAR	com_index;	/* Windows system's virtual serial port number			*/
};

UCHAR *FAX_ClntStart(UCHAR *bd,UCHAR *fax_attrib,UCHAR *local_attrib);
UCHAR FAX_ClntStop(UCHAR *fax_clntinfo);
void FAX_CBFunc(WORD event,UCHAR *param);

#else

#define FAX_ClntStart(a,b,c) 		0
#define FAX_ClntStop(a) 			0

#endif

#ifdef CONFIG_FAX_SERVER

#define PORTNAME_LENGTH				12
#define DATABUFFER_LENGTH			1024
#define REPEAT_TIMES				5

struct FAX_SvrInfo {
	DWORD	svc_hdl;				/* the handle of FAX service record on local SDP database	*/
	UCHAR	vport;					/* the number of virtual port returned by SPP_AddEntry		*/
	UCHAR	svr_chnl;				/* the server channel of FAX service on local device		*/
	HANDLE	sendevent;				/* the handle of event used to send data to modem			*/
	struct	BtList	*recvdatalist;	/* the list of data receiving from local BT system			*/
	DWORD	mdmport_handle;			/* the handle of serial port attached to a physical modem	*/ 
	UCHAR	mdmport_name[PORTNAME_LENGTH+1];
									/* the name of serial port attached to a physical modem		*/
};

struct FAX_SvrLocalAttr {
	UCHAR		*mdmport_name;
#ifdef CONFIG_SDP_FIXED_DATABASE
	DWORD		svc_hdl;
	UCHAR		svr_chnl;
#endif
};

UCHAR* FAX_SvrStart(UCHAR *local_attrib);
UCHAR FAX_SvrStop(UCHAR *fax_svrinfo);
void FAX_SvrCBFunc(WORD event,UCHAR *param);
void FAX_MdmThread(void);
void FAX_SendThread(void);
DWORD FAX_OpenMdmPort(UCHAR *mdmport_name);
BOOL FAX_ConfigMdmPort(DWORD handle);
BOOL FAX_WriteMdmPort(DWORD handle,UCHAR *buf,DWORD nwrite,DWORD *nwritten);
BOOL FAX_ReadMdmPort(DWORD handle,UCHAR *buf,DWORD nread,DWORD *nreturned);
BOOL FAX_SetMdmEventMask(DWORD handle,DWORD mask );
BOOL FAX_WaitMdmEvent(DWORD handle,DWORD *eventmask);
BOOL FAX_GetMdmStatus(DWORD handle,DWORD *modemstatus);
void FAX_SetDTRmdmPort(DWORD handle);
void FAX_ClrDTRmdmPort(DWORD handle);
void FAX_SetRTSmdmPort(DWORD handle);
void FAX_ClrRTSmdmPort(DWORD handle);
BOOL FAX_CloseMdmPort(DWORD handle);

#else

#define FAX_SvrStart(a)	 			0
#define FAX_SvrStop(a) 				0

#endif

#endif

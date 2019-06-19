#ifndef DUN_UI_H
#define DUN_UI_H

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    dun_ui.h
Abstract:
	This file is the header file of exported API from DUN profile.
Author:
    CHEN ZHONGYI
Revision History:2002.2
			
---------------------------------------------------------------------------*/

extern struct BtList *dun_list;

struct DUN_CbkStru {
	UCHAR	*hdl;							/* The handle returned by DUN_ClntStart() or DUN_SvrStart()	*/
	UCHAR	*cbk;							/* the pointer to a StatusInd callback function				*/
};

BOOL DUN_Init(void);
void DUN_Done(void);
void DUNAPP_RegCbk(UCHAR *hdl,UCHAR *pfunc);
const char * DUN_GetVersion(void);

#ifdef CONFIG_DUN_CLIENT

struct DUN_ClntInfo {
	UCHAR	vport;							/* the number of virtual port returned by SPP_AddEntry		*/
	UCHAR	svr_chnl;						/* the server channel of DUN service on remote device		*/
	UCHAR	com_index;						/* Windows system's virtual serial port number				*/
};

UCHAR * DUN_ClntStart(UCHAR *bd,UCHAR *dun_attrib,UCHAR *local_attrib);
UCHAR DUN_ClntStop(UCHAR *dun_clntinfo);
void DUN_CBFunc(WORD event,UCHAR *param);

#else

#define DUN_ClntStart(a,b,c)	0
#define DUN_ClntStop(a)			0

#endif


/*DUN server side definition*/

#ifdef CONFIG_DUN_SERVER

#define PORTNAME_LENGTH			12
#define DATABUFFER_LENGTH		1024
#define REPEAT_TIMES			5

struct DUN_DataStru {
	WORD	length;
	UCHAR	*buf;
};

struct DUN_SvrInfo {
	UCHAR	vport;							/* the number of virtual port returned by SPP_AddEntry		*/
	UCHAR	svr_chnl;						/* the server channel of DUN service on local device		*/
	DWORD	svc_hdl;						/* the handle of DUN service record on local SDP database	*/
	HANDLE	sendevent;						/* the handle of event used to send data to modem			*/
	struct	BtList *recvdatalist; 			/* the list of data receiving from local BT system			*/
	DWORD	mdmport_handle;					/* the handle of serial port attached to a physical modem	*/ 
	UCHAR	mdmport_name[PORTNAME_LENGTH+1];/* the name of serial port attached to a physical modem		*/
};

struct DUN_CltLocalAttrStru {
	UCHAR	com_index;
};

struct DUN_SvrLocalAttrStru {
	UCHAR	*mdmport_name;
#ifdef CONFIG_SDP_FIXED_DATABASE
	DWORD	svc_hdl;
	UCHAR	svr_chnl;
#endif
};

UCHAR * DUN_SvrStart(UCHAR *local_attrib);
UCHAR DUN_SvrStop(UCHAR *dun_svrinfo);
void DUN_SvrCBFunc(WORD event,UCHAR *param);
void DUN_MdmThread(void);
void DUN_SendThread(void);
DWORD DUN_OpenMdmPort(UCHAR *mdmport_name);
BOOL DUN_ConfigMdmPort(DWORD handle);
BOOL DUN_WriteMdmPort(DWORD handle,UCHAR *buf,DWORD nwrite,DWORD *nwritten);
BOOL DUN_ReadMdmPort(DWORD handle,UCHAR *buf,DWORD nread,DWORD *nreturned);
BOOL DUN_SetMdmEventMask(DWORD handle,DWORD mask);
BOOL DUN_WaitMdmEvent(DWORD handle,DWORD *eventmask);
BOOL DUN_GetMdmStatus(DWORD handle,DWORD *modemstatus);
void DUN_SetDTRmdmPort(DWORD handle);
void DUN_ClrDTRmdmPort(DWORD handle);
void DUN_SetRTSmdmPort(DWORD handle);
void DUN_ClrRTSmdmPort(DWORD handle);
BOOL DUN_CloseMdmPort(DWORD handle);

#ifdef CONFIG_INTER_TESTER
void DUN_SetInterTraceCbK(UCHAR *func);
#endif

#else

#define DUN_SvrStart(a)			0
#define DUN_SvrStop(a)			0

#endif

#endif

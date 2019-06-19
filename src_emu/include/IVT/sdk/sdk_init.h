/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_init.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/



#ifndef _SDK_INIT_H
#define _SDK_INIT_H


/* Type of Callback Indication */
#define BTSDK_PIN_CODE_IND 						0x00
#define BTSDK_LINK_KEY_REQ_IND					0x01
#define BTSDK_LINK_KEY_NOTIF_IND				0x02
#define BTSDK_AUTHENTICATION_FAIL_IND  			0x03
#define BTSDK_INQUIRY_RESULT_IND  				0x04
#define BTSDK_INQUIRY_COMPLETE_IND 				0x05
#define BTSDK_AUTHORIZATION_IND  				0x06
#define BTSDK_CONNECTION_REQUEST_IND			0x07
#define BTSDK_CONNECTION_COMPLETE_IND			0x08
#define BTSDK_CONNECTION_EVENT_IND  			0x09
#define BTSDK_SHORTCUT_EVENT_IND	 			0x0A


/*************** Structure Definition ******************/
/* Callback Function */
typedef void  (Btsdk_Pin_Req_Ind_Func)(BTDEVHDL dev_hdl);
typedef void  (Btsdk_Link_Key_Req_Ind_Func)(BTDEVHDL dev_hdl);
typedef void  (Btsdk_Link_Key_Notif_Ind_Func)(BTDEVHDL dev_hdl, BTUINT8 *link_key);
typedef void  (Btsdk_Authentication_Fail_Ind_Func)(BTDEVHDL dev_hdl);
typedef void  (Btsdk_Inquiry_Result_Ind_Func)(BTDEVHDL dev_hdl);
typedef void  (Btsdk_Inquiry_Complete_Ind_Func)(void);
typedef void  (Btsdk_Authorization_Req_Ind_Func)(BTSVCHDL svc_hdl, BTDEVHDL dev_hdl);
typedef BTUINT8  (Btsdk_Connection_Request_Ind_Func)(BTDEVHDL dev_hdl, BTUINT32 dev_class);
typedef void  (Btsdk_Connection_Complete_Ind_Func)(BTDEVHDL dev_hdl);
typedef void  (Btsdk_Connection_Event_Ind_Func)(BTCONNHDL conn_hdl, BTUINT16 event, BTUINT8 *arg);
typedef void  (Btsdk_Shortcut_Event_Ind_Func)(BTSHCHDL shc_hdl, BTUINT16 event);

typedef struct  _BtSdkCallBackStru
{
	BTUINT16 type;					/*type of callback*/
	void *func;						/*callback function*/
}BtSdkCallBackStru, *PBtSdkCallBackStru;



/* Memory Allocation */
void *Btsdk_MallocMemory(BTUINT32 size);
void Btsdk_FreeMemory(void *memblock);

/* Initialize, Deinitialize and Register Callback */
BTINT32 Btsdk_Init(void);
BTINT32 Btsdk_ResetHardware(void);
BTINT32 Btsdk_Done(void);
BTINT32 Btsdk_RegisterSdkCallBack( BtSdkCallBackStru* call_back);
BTINT32 Btsdk_SetSaveFileRootDir(BTUINT8 *pFilePath);
#endif

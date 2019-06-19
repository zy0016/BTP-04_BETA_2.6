/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_opp.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _SDK_OPP_H
#define _SDK_OPP_H
/*************** Macro Definition ******************/


/*************** Structure Definition ******************/
#ifndef _SDK_FTP_H
typedef struct _BtSdkFileTransferReqStru
{
	BTDEVHDL dev_hdl;		/* Handle to the remote device tries to upload the file */
	BTUINT16 operation;		/* Reserved for future extension. */
	BTUINT16 flag;			/* Flag specifies the current status of uploading. 
							It can be following values:
								BTSDK_ER_CONTINUE: The remote device request to upload the file. 
								BTSDK_ER_SUCCESS: The remote device uploads the file successfully. 
								Other value: Error code specifies the reason of uploading failure. 
							*/
	BTUINT8	 file_name[BTSDK_PATH_MAXLENGTH];	/* the name of the file uploaded or to be uploaded */
} BtSdkFileTransferReqStru, *PBtSdkFileTransferReqStru;
#endif


/*************** Function Prototype ******************/

/* Register a function for transfering file status
	first:		[in]first callback flag 
	last:		[in]last callback flag 
	filename:	[in]file name, only valid when first flag is setted.
	filesize:	[in]total transfer file size, only valid when first flag is setted.
	cursize:	[in]current transfer size
*/
typedef void (Btsdk_OPP_STATUS_INFO_CB)(BTUINT8 first, BTUINT8 last, BTUINT8* filename, BTUINT32 filesize, BTUINT32 cursize);
void Btsdk_OPPRegisterStatusCallback(BTCONNHDL conn_hdl, Btsdk_OPP_STATUS_INFO_CB *func);

/* Register a function for uploading file request from remote device.
   pFileInfo	[in/out] Specify the information of the file uploaded or to be uploaded.
				When input pFileInfo->flag is set to BTSDK_ER_CONTINUE, following operation is allowed:
				(1)If the application wants to save the file using a different name, copy the 
				new file name to pFileInfo->file_name.
				(2)If the application wants to reject the file uploading request, change pFileInfo->flag
				to one of OBEX error code except for BTSDK_ER_CONTINUE and BTSDK_ER_SUCCESS.
				(3)If the application allow to save the file, just keep pFileInfo->flag unchanged.

	return value is ignored.
*/
typedef BTBOOL (BTSDK_OPP_UIDealReceiveFile)(PBtSdkFileTransferReqStru pFileInfo);
void Btsdk_OPPRegisterDealReceiveFileCB(BTSDK_OPP_UIDealReceiveFile* func);


BTINT32 Btsdk_OPPCancelTransfer(BTCONNHDL conn_hdl);

BTINT32 Btsdk_OPPPushObj(BTCONNHDL conn_hdl,BTUINT8 *szPushFilePath);
BTINT32 Btsdk_OPPPullObj(BTCONNHDL conn_hdl,BTUINT8 *szPullFilePath);
BTINT32 Btsdk_OPPExchangeObj(BTCONNHDL conn_hdl,BTUINT8 *szPushFilePath,BTUINT8 *szPullFilePath,BTINT32 *npushError, BTINT32 *npullError);

BTSVCHDL Btsdk_RegisterOPPService(BTUINT8 *inbox_path, BTUINT8 *outbox_path, BTUINT8 *own_card);
BTUINT32 Btsdk_UnregisterOPPService(void);

#endif

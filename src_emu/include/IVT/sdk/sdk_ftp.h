/********************************************************************
  Copyright (C), 2004, IVT Corporation
  Module name:		sdk_ftp.h
  Author:       
  Version:        
  Date: 
  Abstract:   
  Revision History:        

********************************************************************/

#ifndef _SDK_FTP_H
#define _SDK_FTP_H

/*************** Macro Definition ******************/
#define FTP_OP_REFRESH	0
#define FTP_OP_UPDIR	1
#define FTP_OP_NEXT		2


/*************** Structure Definition ******************/
#ifndef _SDK_OPP_H
typedef struct _BtSdkFileTransferReqStru
{
	BTDEVHDL dev_hdl;		/* Handle to the remote device tries to upload/delete the file. */
	BTUINT16 operation;		/* Specify the operation on the file. 
							It can be one of the following values:
								BTSDK_APP_EV_FTP_PUT: The remote device request to upload the file.
								BTSDK_APP_EV_FTP_DEL_FILE: The remote device request to delete the file.
								BTSDK_APP_EV_FTP_DEL_FOLDER: The remote device request to delete the folder. In this case,
															 file_name specify the name of the folder to be deleted.
							*/
	BTUINT16 flag;			/* Flag specifies the current status of uploading/deleting. 
							It can be one of the following values:
								BTSDK_ER_CONTINUE: The remote device request to upload/delete the file. 
								BTSDK_ER_SUCCESS: The remote device uploads/deletes the file successfully. 
								Other value: Error code specifies the reason of uploading/deleting failure. 
							*/
	BTUINT8	 file_name[BTSDK_PATH_MAXLENGTH];	/* the name of the file uploaded/deleted or to be uploaded/deleted */
} BtSdkFileTransferReqStru, *PBtSdkFileTransferReqStru;
#endif

/*************** Function Prototype ******************/

/*Register a function for transfering file status
	first:		[in]first callback flag 
	last:		[in]last callback flag 
	filename:	[in]file name, only valid when first flag is setted.
	filesize:	[in]total transfer file size, only valid when first flag is setted.
	cursize:	[in]current transfer size
*/
typedef void  (Btsdk_FTP_STATUS_INFO_CB)(BTUINT8 first, BTUINT8 last, BTUINT8* filename, BTUINT32 filesize, BTUINT32 cursize);
void Btsdk_FTPRegisterStatusCallback(BTCONNHDL conn_hdl, Btsdk_FTP_STATUS_INFO_CB *func);

/* Register a function for uploading file request from remote device.
   pFileInfo	[in/out] Specify the information of the file uploaded or to be uploaded.
				When input pFileInfo->flag is set to BTSDK_ER_CONTINUE, following operation is allowed:
				(1)If the application wants to save the file using a different name, copy the 
				new file name to pFileInfo->file_name.
				(2)If the application wants to reject the uploading request, change pFileInfo->flag
				to one of OBEX error code except for BTSDK_ER_CONTINUE and BTSDK_ER_SUCCESS.
				(3)If the application allow to save the file, just keep pFileInfo->flag unchanged.

	return value is ignored.
*/
typedef BTBOOL (BTSDK_FTP_UIDealReceiveFile)(PBtSdkFileTransferReqStru pFileInfo);
void Btsdk_FTPRegisterDealReceiveFileCB(BTSDK_FTP_UIDealReceiveFile* func);

/***SYS_FIND_DATA: This structure describes a file found by the FindFirstFile(Depends on the system).****/
typedef void (BTSDK_FTP_UIShowBrowseFile)(BTUINT8* SYS_FIND_DATA);

BTINT32 Btsdk_FTPBrowseFolder(BTCONNHDL conn_hdl, BTUINT8 *szPath, BTSDK_FTP_UIShowBrowseFile* pShowFunc, BTUINT8 op_type);
BTINT32 Btsdk_FTPGetRmtDir(BTCONNHDL conn_hdl,BTUINT8 *szDir);
BTINT32 Btsdk_FTPSetRmtDir(BTCONNHDL conn_hdl,BTUINT8 *szDir);	
BTINT32 Btsdk_FTPCreateDir(BTCONNHDL conn_hdl,BTINT8 *szDir);
BTINT32 Btsdk_FTPDeleteDir(BTCONNHDL conn_hdl,BTINT8 *szDir);
BTINT32 Btsdk_FTPDeleteFile(BTCONNHDL conn_hdl,BTINT8 *szDir);
BTINT32 Btsdk_FTPCancelTransfer(BTCONNHDL conn_hdl);
BTINT32 Btsdk_FTPPutDir(BTCONNHDL conn_hdl, BTUINT8 *loc_dir, BTUINT8 *new_dir);
BTINT32 Btsdk_FTPPutFile(BTCONNHDL conn_hdl, BTUINT8 *loc_file, BTUINT8 *new_file);
BTINT32 Btsdk_FTPGetDir(BTCONNHDL conn_hdl, BTUINT8 *rem_dir, BTUINT8 *new_dir);
BTINT32 Btsdk_FTPGetFile(BTCONNHDL conn_hdl, BTUINT8 *rem_file, BTUINT8 *new_file);
BTBOOL Btsdk_FTPBackDir(BTCONNHDL conn_hdl);
BTSVCHDL Btsdk_RegisterFTPService(BTUINT16 desired_access, BTUINT8 *root_dir);
BTUINT32 Btsdk_UnregisterFTPService(void);

#endif

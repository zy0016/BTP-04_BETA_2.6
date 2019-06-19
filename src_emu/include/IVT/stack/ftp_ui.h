/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    ftp_ui.h
Abstract:
	This module provides all the constants and user interface functions for 
	the upper layer application.
Author:
    Luo hongbo
Revision History:
	2002.5		created
version 2.0.3
	2002.10.30	add a parameter "svr_chnl" in the structure "FTP_SvrInfo".
---------------------------------------------------------------------------*/
#ifndef FTP_UI_H
#define FTP_UI_H

#include "goep_ui.h"
#include "file_dep.h"
#include "ftp_parser.h"

#ifdef CONFIG_MICROSOFT
#define FTP_MAX_PATH	 256								/* the maximum length of the path */
#else
#define FTP_MAX_PATH	 32									/* for the embedded system */
#endif
#define INVALID_FTP_HDL	0									/* invalid ftp handle */
#define IS_VALID_FTP_HDL(hdl) (hdl!=INVALID_FTP_HDL)

typedef UCHAR* FTPHDL;										/* FTP handle */

/* some not support attributes */
#define FTP_BROWSE_NOT_PMT			0x08					/* Browse the sub-folder listing not permitted */
#define FTP_PUSH_FD_NOT_SUP			0x10					/* Push folder not supported */
#define FTP_PULL_FD_NOT_SUP			0x20					/* Pull folder not supported */	
#define FTP_CRT_FD_NOT_SUP			0x40					/* Create folder not supported */
#define FTP_DEL_NON_EMP_FD_NOT_SUP	0x80					/* Delete a non empty folder not supported */

/* the ftp service attribute */
struct FTP_SvrInfo{
	DWORD svc_hdl;
	WORD  user_size;										/* the number of the users */
	UCHAR attrib;											/* authentication and access,and not support attribute */
	UCHAR svr_chnl;											/* the output parameter */
	UCHAR root_dir[FTP_MAX_PATH];							/* the root directory of the server */
#ifdef CONFIG_GOEP_AUTH
	struct GOEPUserInfo user_list[1];						/* the user list */
#endif
};

/* the parameters sent to the upper layer */
struct FTP_ParamInfo{
	UCHAR  code;											/* the code of the operation */
	UCHAR  name[FTP_MAX_PATH];								/* the name of the file of folder being operated */
};

/* the common functions */
const char* FTP_GetVersion(void);							/* Get the latest ftp profile version */
void    FTPAPP_RegCbk(UCHAR* hdl, UCHAR* pfunc);			/* Register the event callback function */
BOOL    FTP_RegCbk(FTPHDL hdl,Status_Cbk status_cbk);		/* Rggister the file transfer progress callback function */

/* Client side functions */
#ifdef CONFIG_FTP_CLIENT
FTPHDL FTP_ClntStart(UCHAR* bd,UCHAR* sdp_attrib,UCHAR* loc_attrib);		/* Start the FTP client to connect */
DWORD  FTP_ClntStop(FTPHDL hdl);											/* Stop  the FTP client to disconnect */
BOOL   FTP_Stop(FTPHDL hdl);												/* Stop the transferring procedure */
BOOL   FTP_DelFile(FTPHDL hdl, const char* file_name);						/* Delete a file or an empty folder */
BOOL   FTP_GetFile(FTPHDL hdl, const char*rem_file, const char* new_file);	/* Get a file from the server */
BOOL   FTP_GetFileEx(FTPHDL hdl, const char*rem_file, FILE_HDL file_hdl);	/* Get a file from the server (exteneded)*/
BOOL   FTP_PutFile(FTPHDL hdl, const char* loc_file, const char* new_file); /* Put a file to the server */
BOOL   FTP_PutDir(FTPHDL hdl, const char* loc_dir, const char*  new_dir);	/* Put a folder to the server */
BOOL   FTP_GetDir(FTPHDL hdl, const char* rem_dir, const char* new_dir);	/* Get a folder from the server */
BOOL   FTP_RemDir(FTPHDL hdl, const char* dir_name);						/* delete a folder */
BOOL   FTP_BackDir(FTPHDL hdl);												/* Backward a level of the directory */
BOOL   FTP_SetCurDir(FTPHDL hdl, const char* dir_name);						/* Enter the current directory */
BOOL   FTP_GetCurDir(FTPHDL hdl, UCHAR* dir_name);							/* Get the current directory */
BOOL   FTP_CreateDir(FTPHDL hdl, const char* dir_name);						/* Create an empty folder on the server */
BOOL   FTP_BrowseObj(FTPHDL hdl,FILE_HDL file_hdl);							/* Browse the folder listing of the server */
WORD   FTP_GetLastErr(FTPHDL hdl);											/* Get the error code of the latest operation */

#ifdef CONFIG_OBEX_ADDITIONAL
BOOL   FTP_PutCancel(FTPHDL hdl);											/* Cancel the put operation */
#endif

#endif

/* Server side functions */
#ifdef CONFIG_FTP_SERVER
FTPHDL FTP_SvrStart(UCHAR* ftp_attrib);						/* Start the FTP service */
DWORD  FTP_SvrStop(FTPHDL hdl);								/* Stop the FTP service */
BOOL   FTP_SetSharedDir(FTPHDL hdl, const char* dir_name);	/* Set the shared dirctory accessed by the client */
BOOL   FTP_GetSharedDir(FTPHDL hdl, UCHAR* dir_name);		/* Get the shared directory accessed by the client */
#endif

#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    goep_ui.h
Abstract:
	This module provides all the constants and user interface functions for 
	the upper layer application.
Author:
    Luo hongbo
Revision History:
	2002.1		created
version 2.1.0
	2002.12.10  
		1) add some constans which specify the corresponding service (line 53);
		2) change the returned value of the callback function GOEP_PullObjCbk()
		   from "void" to "UCHAR";
		3) add two micros GOEP_SetStrHeader() and GOEP_PutCmd() for the upper
		   layer.
version 2.1.4
	2003.3.11	change the parameter type of GOEP_Open().
---------------------------------------------------------------------------*/
#ifndef GOEP_UI_H
#define GOEP_UI_H

#include "obexui.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef MIN
#define MIN(a,b) (((DWORD)a>(DWORD)b)?b:a)
#endif

#ifndef MAX
#define MAX(a,b) (((DWORD)a>(DWORD)b)?a:b)
#endif

#define INVALID_GOEP_HDL		0
#define MAX_USER_ID_LEN			20
#define MAX_PWD_LEN				64

/* some values of the attributes */
/* the first two bits : the access right */
#define	NO_ACCESS				0x00		
#define READ_WRITE_ACCESS		0x01
#define READ_ONLY_ACCESS		0x02
/* the three bits */
#define IS_ANONYMOUS			0x04		/* whether it is anoymous or required authentication */
/* the next five bits : service type*/
#define INBOX_SERVICE			0x00		
#define FOLDER_BROWSE_SERVICE	0x08		
#define IRMC_SERVICE			0x10
#define IMG_PUSH_SERVICE		0x18
#define IMG_PULL_SERVICE		0x20
#define IMG_PRINT_SERVICE		0x28
#define IMG_AUTO_ARCH_SERVICE	0x30
#define IMG_REM_CAM_SERVICE		0x38
#define IMG_REM_DISP_SERVICE	0x40
#define IMG_REM_REF_OBJ			0x48
#define IMG_REM_ARCH_OBJ		0x50

#define STOP_FLAG				0x08		/* the stop flag when transferring */
#define END_CONTENT_FLAG		0x10		/* the end of the object */
#define SEND_BUF_FLAG			0x20		/* the send buffer    */
#define RECV_BUF_FLAG			0x40		/* the receive buffer */
#define AUTO_ALLOC_FLAG			0x80		/* whether it is automatically allocated by the GOEP */	

/* some masks to get the value of the corresponding attribute */
#define BUF_INFO				0x0001
#define MAX_SEND_INFO			0x0002
#define MAX_RECV_INFO			0x0004
#define AUTH_INFO				0x0008
#define ACCESS_INFO				0x0010
#define BUF_FLAG_INFO			0x0020
#define SESSION_ID_INFO			0x0040
#define HEAD_LEN_INFO			0x0080
#define STOP_FLAG_INFO			0x0100
#define STATUS_CBK_INFO			0x0200
#define SPP_HDL_INFO			0x0400

#define SET_CUR_DIR_IND		(VCOMM_CONN_IND+10)
#define CREATE_DIR_IND		(SET_CUR_DIR_IND+1)
#define SET_ROOT_DIR_IND	(SET_CUR_DIR_IND+2)
#define BACK_DIR_IND		(SET_CUR_DIR_IND+3)

typedef UCHAR* GOEPHDL;
typedef UCHAR (*GOEP_Cbk)(UCHAR* goep_hdl,UCHAR event,UCHAR header,UCHAR* param);
typedef void  (*Status_Cbk)(UCHAR first,UCHAR last,UCHAR* filename,DWORD filesize,DWORD cursize);
typedef UCHAR (*GOEP_PullObjCbk)(UCHAR* hdl,UCHAR* data,WORD len,UCHAR is_end);

#define GOEP_IS_AUTH(a)		(a&0x04)
#define GOEP_BUF_FLAG(a)	(a&0xf0)
#define GOEP_ACCESS(a)		(a&0x03)
#define GOEP_IS_STOP(a)		(a&0x08)

struct GOEP_InfoStru{
	Status_Cbk			 status_cbk;
	struct BuffStru* buf;
	DWORD   session_id;
	WORD	max_send_len;
	WORD    max_recv_len;
	UCHAR   spp_hdl;	
	UCHAR   attrib;	
};

#ifdef CONFIG_GOEP_AUTH
struct GOEPUserInfo{
	UCHAR			pwd_only;						/* whether the user id is needed */
	UCHAR			user_id[MAX_USER_ID_LEN+1];		/* the user id string		*/
	UCHAR			pwd[MAX_PWD_LEN+1];				/* the password string		*/
};
#endif

#define GOEP_GetAccess(info)	(info->attrib&3)
struct GOEPConnInfo{
	UCHAR			attrib;							/* the service type and whether it is anoymous */
#ifdef CONFIG_GOEP_AUTH
	UCHAR			user_id[MAX_USER_ID_LEN+1];		/* the user id string		*/
	UCHAR			pwd[MAX_PWD_LEN+1];				/* the password string		*/
#endif
};

DWORD GOEP_Init(void);
void  GOEP_Done(void);
const char* GOEP_GetVersion(void);
void GOEP_Close(GOEPHDL hdl);
GOEPHDL GOEP_Open(UCHAR* bd,UCHAR isserver,WORD uuid,UCHAR obex_type,UCHAR* svrchnl);
WORD GOEP_BuffNew(GOEPHDL hdl,DWORD size,UCHAR flag);
void GOEP_RegCbk(GOEPHDL hdl,UCHAR* cbk_hdl,GOEP_Cbk cbk,GOEP_PullObjCbk pull_cbk);
void GOEP_GetRemoteBd(GOEPHDL hdl,UCHAR* bd);

#define INVALID_CONNECTION_ID               0xffffffff
#define IS_VALID_GOEP_HDL(hdl)				(hdl!=INVALID_GOEP_HDL)
#define GOEP_BuffFree(p)					OBEX_BuffFree(p)
#define GOEP_GetHeader(id,header,buf,len)   OBEX_GetHeader(id,header,buf,len)
#define GOEP_SetHeader(id,header,buf,len)	OBEX_SetHeader(id,header,buf,len)
#define GOEP_SetStrHeader(id,header,buf)	OBEX_SetHeader(id,header,buf,(WORD)(strlen((const char*)buf)+1))
#define GOEP_GetHeaderLen(id,header)		OBEX_GetHeaderLen(id,header)
/*0 is a valid connection ID (By WeiJunping and ZhangJian on 2004-3-1 14:24:55)*/
#define GOEP_SetID(id)                      {if(id!=INVALID_CONNECTION_ID) {OBEX_SetHeader(id,OBEXH_CONNID,(UCHAR*)&id,sizeof(DWORD));}} 
#define GOEP_CloseService(id)				{ WORD ret;{OBEX_CloseService(id,&ret);}}
#define GOEP_PutCmd(id,r)					{ WORD ret;{OBEX_Put_Req(id,END_PUT_REQ_ID,&ret);*r = (UCHAR)ret;}}
#define GOEP_GetCmd(id,r)					{ WORD ret;{OBEX_Get_Req(id,GET_REQ_ID,&ret);*r = (UCHAR)ret;}}

#ifdef CONFIG_GOEP_CLIENT
#define GOEP_PushObject(hdl,obj,obj_len)	GOEP_PushObjectEx(hdl,1,obj,obj_len,NULL)
WORD   GOEP_Connect(GOEPHDL hdl,struct GOEPConnInfo* info);
WORD   GOEP_PushObjectEx(GOEPHDL hdl,UCHAR flag,UCHAR* obj, DWORD obj_len,Status_Cbk pfunc);
WORD   GOEP_PullObject(GOEPHDL hdl,UCHAR* name,Status_Cbk pfunc);
WORD   GOEP_DeleteObject(GOEPHDL hdl,UCHAR* name,WORD name_len,UCHAR* app_com,WORD app_com_len);
WORD   GOEP_Disconnect(GOEPHDL hdl);
WORD   GOEP_SetPath(GOEPHDL hdl,UCHAR flag,UCHAR* name,WORD name_len);

#ifdef CONFIG_OBEX_ADDITIONAL
WORD   GOEP_PushObjectCancel(GOEPHDL hdl);
#endif

#endif

void   GOEP_SetInfo(GOEPHDL hdl,struct GOEP_InfoStru* pinfo,WORD mask);
void   GOEP_GetInfo(GOEPHDL hdl,struct GOEP_InfoStru* pinfo,WORD mask);
#ifdef CONFIG_GOEP_SERVER
#ifdef CONFIG_GOEP_AUTH
int    GOEP_GetUserNum(GOEPHDL hdl);
void   GOEP_AddUser(GOEPHDL hdl,struct GOEPUserInfo* info);
void   GOEP_RemoveUser(GOEPHDL hdl,const char* name);
void   GOEP_ClearUsers(GOEPHDL hdl);
struct GOEPUserInfo* GOEP_FindUser(GOEPHDL hdl,const char* name);
#endif
#endif

#endif

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    obex_ui.h
Abstract:
	This module defines the user interface of the OBEX layer.
Author:
    Luo hongbo
Revision History:
	2000.11		created
version 3.4.0:
	2002.10.9	add two new headers OBEXH_IMGHDL and OBEXH_IMGDESCRIPTOR.
version 3.4.2
	2003.3.11	change the parameter type of OBEX_OpenService().
---------------------------------------------------------------------------*/
#ifndef OBEXUI_H
#define OBEXUI_H

#ifndef FUNC_EXPORT
#define FUNC_EXPORT
#endif

/* OBEX Headers */
#define OBEXH_NO_HEADER					0x00		/* no header is included in the packet */
#define	OBEXH_COUNT						0xc0		/*Number of objects. */
#define	OBEXH_NAME						0x01		/* Name of the object.*/
#define	OBEXH_TYPE						0x42		/* Type of the object (ex. text/html). */
#define	OBEXH_LENGTH					0xc3		/*Length of the object in bytes.*/
#define	OBEXH_TIME_ISO					0x44		/*	ISO 8601 timestamp (preferred format). */
#define	OBEXH_TIME_COMPAT				0xc4		/*	Old timestamp, for compatibility only. */
#define	OBEXH_DESCRIPTION				0x05		/*	Text description of object. */
#define	OBEXH_TARGET					0x46 		/* Name of service that object is targeted for. */
#define	OBEXH_HTTP 						0x47		/* 	An HTTP 1.x header. */
#define	OBEXH_BODY						0x48		/* the start of the body */
#define	OBEXH_END_BODY					0x49		/* the end of the body */
#define	OBEXH_WHO 	 					0x4a		/*	Identifies the service providing the object. */
#define	OBEXH_CONNID					0xcb		/*	Identifies the connection     */
#define	OBEXH_APP_PARMS					0x4c		/* Contains application parameters. */
#define	OBEXH_AUTH_CHAL					0x4d		/* Contains an authentication challenge. */
#define	OBEXH_AUTH_RESP					0x4e		/* Contains the response to an authentication challenge. */
#define	OBEXH_OBJ_CLASS					0x4f		/* Contains  OBEX Object class of object*/
/* user defined header*/
#define OBEXH_RESET						0x31		/* to reset the state */
#define OBEXH_IMGHDL					0x30		/* the image handle : UTF-16 Unicode text */
#define OBEXH_IMGDESCRIPTOR				0x71		/* the image descriptor : byte sequence */

/* Command identifier */
#define CONN_REQ_ID						0x80
#define PUT_REQ_ID						0x02
#define END_PUT_REQ_ID					0x82
#define GET_REQ_ID						0x03
#define END_GET_REQ_ID					0x83
#define DISCONN_REQ_ID					0x81
#define SETPATH_REQ_ID					0x85
#define ABORT_REQ_ID					0xff

/* Response code */
#define  RSP_CODE_CONTINUE				0x90
#define  RSP_CODE_SUCCESS				0xA0
#define  RSP_CODE_CREATED				0xA1
#define  RSP_CODE_ACCEPTED				0xA2
#define  RSP_CODE_NON_AUTH_INFO			0xA3
#define  RSP_CODE_NO_CONTENT			0xA4
#define  RSP_CODE_RESET_CONTENT			0xA5
#define  RSP_CODE_PARTIAL_CONTENT		0xA6
#define  RSP_CODE_MULT_CHOICES			0xB0
#define  RSP_CODE_MOVE_PERM				0xB1
#define  RSP_CODE_MOVE_TEMP				0xB2
#define  RSP_CODE_SEE_OTHER				0xB3
#define  RSP_CODE_NOT_MODIFIED			0xB4
#define  RSP_CODE_USE_PROXY				0xB5
#define  RSP_CODE_BAD_REQ				0xC0
#define  RSP_CODE_UNAUTHORIZED			0xC1
#define  RSP_CODE_PAY_REQ				0xC2
#define  RSP_CODE_FORBIDDEN				0xC3
#define  RSP_CODE_NOTFOUND				0xC4
#define  RSP_CODE_NOT_ALLOWED			0xC5
#define  RSP_CODE_NOT_ACCEPT			0xC6
#define  RSP_CODE_PROXY_AUTH_REQ		0xC7
#define  RSP_CODE_TIME_OUT				0xC8
#define  RSP_CODE_CONFLICT				0xC9
#define  RSP_CODE_GONE					0xCA
#define  RSP_CODE_LEN_REQ				0xCB
#define  RSP_CODE_PREC_FAIL				0xCC
#define  RSP_CODE_ENT_TOO_LARGE			0xCD
#define  RSP_CODE_URL_TOO_LARGE			0xCE
#define  RSP_CODE_NOTSUPPORT			0xCF
#define  RSP_CODE_SVR_ERR				0xD0
#define  RSP_CODE_NOTIMPLEMENTED		0xD1
#define  RSP_CODE_BAD_GATEWAY			0xD2
#define  RSP_CODE_SERVICE_UNAVAILABLE	0xD3
#define  RSP_CODE_GATEWAY_TIMEOUT		0xD4
#define  RSP_CODE_HTTP_NOTSUPPORT		0xD5
#define  RSP_CODE_DATABASE_FULL			0xE0
#define  RSP_CODE_DATABASE_LOCK			0xE1

/* events for the upper layer */
#define PUT_DATA_IND		1
#define GET_DATA_IND		2
#define ABORT_IND			3
#define SETPATH_IND			4
#define CONN_IND			5
#define DISCONN_IND         6
#define VCOMM_RELEASE_IND   7
#define VCOMM_CONN_IND		9

/* structure for the upper layer */
struct OBEXConnReqOutStru{
	DWORD		connid;
	WORD		max_send_len;
	WORD        ret;
};

struct OBEXConnRspOutStru{
	DWORD		connid;
	WORD		ret;
};

/* OBEX layer User Interface Functions */
FUNC_EXPORT void OBEXInit(void);
FUNC_EXPORT void OBEXDone(void);
FUNC_EXPORT void OBEX_Connect_Req(UCHAR handle,WORD max_recv_len, struct OBEXConnReqOutStru* out);
FUNC_EXPORT void OBEX_Connect_Rsp(UCHAR handle,WORD max_pack_len, UCHAR code,struct OBEXConnRspOutStru* out);
FUNC_EXPORT void OBEX_Put_Req(DWORD session_id,UCHAR code, WORD* ret);
FUNC_EXPORT void OBEX_Put_Rsp(DWORD session_id,UCHAR code,WORD* ret);
FUNC_EXPORT void OBEX_Get_Req(DWORD session_id, UCHAR code,WORD* ret);
FUNC_EXPORT void OBEX_Get_Rsp(DWORD session_id, UCHAR code,WORD* ret);
FUNC_EXPORT void OBEX_Setpath_Req(DWORD session_id, UCHAR flag,WORD* ret);
FUNC_EXPORT void OBEX_Setpath_Rsp(DWORD session_id, UCHAR code,WORD* ret);
FUNC_EXPORT void OBEX_Abort_Req(DWORD session_id, WORD* ret);
FUNC_EXPORT void OBEX_Abort_Rsp(DWORD session_id, UCHAR code,WORD* ret);
FUNC_EXPORT void OBEX_Disconnect_Req(DWORD session_id, WORD* ret);
FUNC_EXPORT void OBEX_Disconnect_Rsp(DWORD session_id,UCHAR code,WORD* ret);
FUNC_EXPORT void OBEX_CloseService(DWORD session_id,WORD* ret);
FUNC_EXPORT UCHAR OBEX_OpenService(UCHAR* bd,UCHAR isserver,WORD uuid,UCHAR* svrchnl);
FUNC_EXPORT void OBEX_CloseComm(UCHAR handle);
FUNC_EXPORT int  OBEX_GetHeaderLen(DWORD session_id, UCHAR type);
FUNC_EXPORT int  OBEX_GetHeader(DWORD session_id, UCHAR type, UCHAR* buf, WORD len);
FUNC_EXPORT int  OBEX_SetHeader (DWORD session_id, UCHAR type, UCHAR* buf, WORD len);
FUNC_EXPORT int  OBEX_SetResetHeader(DWORD session_id);
FUNC_EXPORT void OBEX_Put_Reset(DWORD session_id);
#ifdef CONFIG_OBEX_ADDITIONAL
FUNC_EXPORT void OBEX_Put_Cancel(DWORD session_id,WORD* ret);
#endif

/* some constant UUIDs used in the upper layer profiles */
#ifdef CONFIG_GNUMAKE
FUNC_EXPORT extern const UCHAR UUID_IRMC[];
FUNC_EXPORT extern const UCHAR UUID_FOLDER_LISTING[];
FUNC_EXPORT extern const UCHAR UUID_IMG_PUSH[];
FUNC_EXPORT extern const UCHAR UUID_IMG_PULL[];
FUNC_EXPORT extern const UCHAR UUID_IMG_PRINT[];
FUNC_EXPORT extern const UCHAR UUID_IMG_AUTO_ARCH[];
FUNC_EXPORT extern const UCHAR UUID_IMG_REM_CAM[];
FUNC_EXPORT extern const UCHAR UUID_IMG_REM_DISP[];
FUNC_EXPORT extern const UCHAR UUID_IMG_REF_OBJ[];
FUNC_EXPORT extern const UCHAR UUID_IMG_ARCH_OBJ[];
#endif

/* This function is implemented on the upper layer to receive
	the events from OBEX */
void OBEX_SendTaskMessage(UCHAR handle,DWORD wParam,UCHAR* lParam);

#ifdef CONFIG_OS_WIN32
struct BuffStru * OBEX_BuffNew(WORD size, WORD off );
UCHAR OBEX_BuffRes(struct BuffStru * buf, INT16 delta);
#else
#define OBEX_BuffNew BuffNew
#define OBEX_BuffRes BuffRes
#endif

#ifdef CONFIG_OS_WIN32
#define OBEX_BuffFree(p)			{if (p) FREE(p);p=NULL;}
#else
#define OBEX_BuffFree(p)			{if (p) BuffFree(p);p=NULL;}
#endif

#endif

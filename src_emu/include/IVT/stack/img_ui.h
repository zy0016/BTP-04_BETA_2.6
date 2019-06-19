/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    img_ui.h
Abstract:
	This module defines the user interface of the Imaging profile .
Author:
    Luo hongbo
Revision History:
	2002.9.29		created
---------------------------------------------------------------------------*/
#ifndef IMG_UI_H
#define IMG_UI_H

#include "goep_ui.h"
#include "img_dep.h"

/* the supported capabilities */
#define SUP_CAP_GENERIC_IMGING	(1<<0x00)
#define SUP_CAP_CAPTURING		(1<<0x01)
#define SUP_CAP_PRINTING		(1<<0x02)
#define SUP_CAP_DISPLAYING		(1<<0x03)

/* the supported features */
#define SUP_FEA_IMG_PUSH		(1<<0x00)
#define SUP_FEA_IMG_PUSH_STORE	(1<<0x01)
#define SUP_FEA_IMG_PUSH_PRINT	(1<<0x02)
#define SUP_FEA_IMG_PUSH_DISP	(1<<0x03)
#define SUP_FEA_IMG_PULL		(1<<0x04)
#define SUP_FEA_IMG_ADV_PRINT	(1<<0x05)
#define SUP_FEA_AUTO_ARCH		(1<<0x06)
#define SUP_FEA_REM_CAM			(1<<0x07)
#define SUP_FEA_REM_DISP		(1<<0x08)

/* the supported functions */
#define SUP_FUN_GET_CAP			(1<<0x00)	
#define SUP_FUN_PUT_IMG			(1<<0x01)
#define SUP_FUN_PUT_LNK_ATTACH	(1<<0x02)
#define SUP_FUN_PUT_LNK_THUMB	(1<<0x03)
#define SUP_FUN_REM_DISP		(1<<0x04)
#define SUP_FUN_GET_IMG_LIST	(1<<0x05)
#define SUP_FUN_GET_IMG_PROP	(1<<0x06)
#define SUP_FUN_GET_IMG			(1<<0x07)
#define SUP_FUN_GET_LNK_THUMB	(1<<0x08)
#define SUP_FUN_GET_LNK_ATTACH	(1<<0x09)
#define SUP_FUN_DEL_IMG			(1<<0x0a)
#define SUP_FUN_START_PRINT		(1<<0x0b)
#define SUP_FUN_GET_PART_IMG	(1<<0x0c)
#define SUP_FUN_START_ARCH		(1<<0x0d)
#define SUP_FUN_GET_MON_IMG		(1<<0x0e)
#define SUP_FUN_GET_STATUS		(1<<0x10)

/************************	PART 1: all the functions used in the profile ************************/
#ifdef CONFIG_MICROSOFT			
#define IMG_NAME_LEN	256
#define ATTACH_NAME_LEN	256
#else							/* Configuration for SOC */
#define IMG_NAME_LEN	32
#define ATTACH_NAME_LEN	32
#endif
#define IMG_HDL_LEN		7
#define SERVICE_ID_LEN	16

/* the four display style of RemoteDisplay */
#define DISP_NEXT_IMG	0x01
#define DISP_PREV_IMG	0x02
#define DISP_SEL_IMG	0x03
#define DISP_CUR_IMG	0x04

/* the two latest capture tag */
#define LATEST_CAPTURE_SORT		0x01
#define LATEST_CAPTURE_NOT_SORT	0x00

/* the two store flag */
#define MONITOR_STORE			0x01
#define MONITOR_NOT_STORE		0x00

/* the end flag */
#define PARTIAL_IMG_FINAL		0x01
#define PARTIAL_IMG_NOT_FINAL	0x00

typedef UCHAR* BIPHDL;						/* the instance of opening a specified service */
#define INVALID_BIP_HDL	0					/* invalid BIP handle */
#define IS_VALID_BIP_HDL(hdl) (hdl!=INVALID_BIP_HDL)	

struct PutImgStru{
	IMG_FILE_HDL	file_hdl;				/* the file handle of the image */
	UCHAR			name[IMG_NAME_LEN];		/* the name of the image: can be null*/
	WORD			len;					/* the length of the image descriptor object */
	UCHAR*			data;					/* the body of the image descriptor object */
};

struct PutLnkThumbStru{
	IMG_FILE_HDL	file_hdl;				/* the file handle of the thumbnail file */
	UCHAR			img_hdl[IMG_HDL_LEN+1];	/* the image handle string */
};

struct PutLnkAttachStru{
	IMG_FILE_HDL	file_hdl;				/* the file handle of the attachment file */
	UCHAR			img_hdl[IMG_HDL_LEN+1];	/* the image handle string */
	WORD			len;					/* the length of the attachment descriptor object */
	UCHAR*			data;					/* the body of the attachment descriptor object */
};

struct RemDispStru{
	UCHAR			img_hdl[IMG_HDL_LEN+1];	/* the image handle string */
	UCHAR			disp_tag;				/* one of the four display styles */
};

struct GetImgListInStru{
	WORD			num_ret_hdl;			/* NbReturnedHandles */
	WORD			list_start_off;			/* ListStartOffset */
	UCHAR			latest_cap_tag;			/* LatestCaptureImages */
	WORD			len;					/* the length of the Image handles descriptor object */
	UCHAR*			data;					/* the body of the Image handles descriptor object */
};

struct GetImgListOutStru{
	IMG_FILE_HDL	file_hdl;				/* the handle of the images-listing object */
	UCHAR*			data;					/* the body of the Image handles descriptor object */
	WORD			num_ret_hdl;			/* NbReturnedHandles */
	WORD			len;					/* the length of the Image handles descriptor object */	
};

struct GetImgStru{
	UCHAR			img_hdl[IMG_HDL_LEN+1];	/* the image handle string */
	WORD			len;					/* the length of the Image-descriptor object */
	UCHAR*			data;					/* the body of the Image-descriptor object */
};

struct GetLnkAttachStru{
	UCHAR			img_hdl[IMG_HDL_LEN+1];	/* the image handle string */
	UCHAR			name[ATTACH_NAME_LEN];	/* the attachment file name */
};


struct StartPrintStru{
	UCHAR			id[SERVICE_ID_LEN];		/* the ServiceID with length of 16 bytes */
	IMG_FILE_HDL	file_hdl;				/* the handle of the Printer control object */
};

struct GetPartImgInStru{
	DWORD			part_file_len;			/* PartialFileLength */
	DWORD			part_file_start_off;	/* PartialFileStartOffset */
	UCHAR			name[IMG_NAME_LEN];		/* the image file name */
};

struct GetPartImgOutStru{
	DWORD			part_file_len;			/* the actual length of partial file */
	DWORD			total_file_size;		/* TotalFileSize*/
	UCHAR			end_flag;				/* EndFlag*/
	UCHAR*			data;					/* the body of the image subfile */
};

struct GetMonImgOutStru{
	UCHAR			img_hdl[IMG_HDL_LEN+1];	/* the image handle string */
	IMG_FILE_HDL	file_hdl;				/* the body of the Monitoring image object */
};

UCHAR IMG_GetCapabilities(BIPHDL hdl/*[in]*/,IMG_FILE_HDL file_hdl/*[in/out]*/);
UCHAR IMG_PutImage(BIPHDL hdl/*[in]*/,struct PutImgStru* img/*[in]*/,UCHAR img_hdl[IMG_HDL_LEN+1]/*[out]*/);
UCHAR IMG_PutLinkedThumbnail(BIPHDL hdl/*[in]*/,struct PutLnkThumbStru* thumb_img/*[in]*/);
UCHAR IMG_PutLinkedAttachment(BIPHDL hdl/*[in]*/,struct PutLnkAttachStru* attach_img/*[in]*/);
UCHAR IMG_RemoteDisplay(BIPHDL hdl/*[in]*/,struct RemDispStru* rem_disp/*[in]*/,UCHAR img_hdl[IMG_HDL_LEN+1]/*[out]*/);
UCHAR IMG_GetImageList(BIPHDL hdl/*[in]*/,struct GetImgListInStru* img_list_in/*[in]*/,struct GetImgListOutStru* img_list_out/*[out]*/);
UCHAR IMG_GetImageProperties(BIPHDL hdl/*[in]*/,UCHAR* img_hdl/*[in]*/,IMG_FILE_HDL file_hdl/*[in/out]*/);
UCHAR IMG_GetImage(BIPHDL hdl/*[in]*/,struct GetImgStru* img/*[in]*/,IMG_FILE_HDL file_hdl/*[in/out]*/);
UCHAR IMG_GetLinkedThumbnail(BIPHDL hdl/*[in]*/,UCHAR* img_hdl/*[in]*/,IMG_FILE_HDL file_hdl/*[in/out]*/);
UCHAR IMG_GetLinkedAttachment(BIPHDL hdl/*[in]*/,struct GetLnkAttachStru* attach_img/*[in]*/,IMG_FILE_HDL file_hdl/*[in/out]*/);
UCHAR IMG_DeleteImage(BIPHDL hdl/*[in]*/,UCHAR* img_hdl/*[in]*/);
UCHAR IMG_StartPrint(BIPHDL hdl/*[in]*/,struct StartPrintStru* print_img/*[in]*/);
UCHAR IMG_GetPartialImage(BIPHDL hdl/*[in]*/,struct GetPartImgInStru* part_img_in/*[in]*/,struct GetPartImgOutStru* part_img_out/*[out]*/);
UCHAR IMG_StartArchive(BIPHDL hdl/*[in]*/,UCHAR	id[SERVICE_ID_LEN]/*[in]*/);
UCHAR IMG_GetStatus(BIPHDL hdl/*[in]*/);
UCHAR IMG_GetMonitoringImage(BIPHDL hdl/*[in]*/,UCHAR store_flag/*[in]*/,struct GetMonImgOutStru* mon_img_out/*[out]*/);

/* extended UI */
UCHAR IMG_GetImageEx(BIPHDL hdl/*[in]*/,struct GetImgStru* img/*[in]*/,GOEP_PullObjCbk cbk/*[in]*/);

/***********************	PART 2: Service Register and Discovery ******************/
struct BIP_ImgStru{
	DWORD	sup_fun;
	DWORD	max_mem;
	WORD	sup_fea;
	UCHAR	sup_capa;
};

struct BIP_ObjStru{
	UCHAR	id[SERVICE_ID_LEN];
	DWORD	sup_fun;
};

DWORD  IMG_Init(void);
void   IMG_Done(void);
void   IMG_ResetCapa(void);
void   IMG_AddCapa(UCHAR type,UCHAR* attr/*[in]*/,WORD size/*[in]*/);
BIPHDL IMG_SvcStart(WORD svc_type/*[in]*/,UCHAR obex_type/*[in]*/,UCHAR* svc_name/*[in]*/,UCHAR* svc/*[in]*/,UCHAR* chnl/*[out]*/);
DWORD  IMG_SvcStop(BIPHDL hdl/*[in]*/);
BIPHDL IMG_Conn(UCHAR* bd/*[in]*/,UCHAR* sdp_attrib/*[in]*/,WORD svc_type/*[in]*/,UCHAR obex_type/*[in]*/,UCHAR* svc/*[in]*/);	
DWORD  IMG_Disconn(BIPHDL hdl/*[in]*/);
void   IMGAPP_RegCbk(BIPHDL hdl/*[in]*/,UCHAR* ev_cbk/*[in]*/);
BOOL   IMG_RegCbk(BIPHDL hdl/*[in]*/,Status_Cbk status_cbk/*[in]*/);
BOOL   IMG_Stop(BIPHDL hdl/*[in]*/);
const char* IMG_GetVersion(void);	/* Get the latest BIP profile version */
/* User interface for the Bluesoleil */
struct BIP_ClntInfo{
	struct BIP_ImgStru	info;
	UCHAR				obex_type;
};

struct BIP_SvrInfo{
	struct BIP_ImgStru	info;
	UCHAR				obex_type;
	UCHAR				svr_chnl;
	UCHAR				svc_name[64];
};

#define IMG_SvrStart(bip_attrib) IMG_SvcStart(CLS_IMAG_RESPONDER,((struct BIP_SvrInfo*)bip_attrib)->obex_type,((struct BIP_SvrInfo*)bip_attrib)->svc_name,(UCHAR*)(&((struct BIP_SvrInfo*)bip_attrib)->info),(UCHAR*)(&((struct BIP_SvrInfo*)bip_attrib)->svr_chnl))
#define IMG_ClntStart(bd,sdp_attrib,loc_attrib) IMG_Conn(bd,sdp_attrib,CLS_IMAG_RESPONDER,((struct BIP_ClntInfo*)loc_attrib)->obex_type,(UCHAR*)(&((struct BIP_ClntInfo*)loc_attrib)->info))
#define IMG_ClntStop(hdl)	IMG_Disconn(hdl)
#define IMG_SvrStop(hdl)	IMG_SvcStop(hdl)


/************************  PART 3 : XML Reader/Writer ************************/
/* The prototype of all functions are declared in this head file */
#include "img_parser.h"		


/************************  PART 4: DPOF Reader/Writer    ************************/
/************************   PART 5: Image Encoder/Decoder ************************/
/* This two part may be put on the application layer */

/************************   PART 6: Features ************************/
/* four operations in the Image Pull Feature */
#define FEA_PULL_THM	0x00				/* get the thumbnail */
#define FEA_PULL_IMG	0x01				/* get the native image */
#define FEA_PULL_ATT	0x02				/* get the attachment */
#define FEA_PULL_DEL	0x03				/* delete the image */

struct FeaImgPushStru{
	IMG_FILE_HDL				img_file_hdl;			/* the file handle of the image */	
	IMG_FILE_HDL				thm_file_hdl;			/* the file handle of the thumbnail file */
	IMG_FILE_HDL				att_file_hdl;			/* the file handle of the attachment file */
	UCHAR						name[IMG_NAME_LEN];		/* the name of the image: can be null*/
	UCHAR						img_hdl[IMG_HDL_LEN+1];	/* the image handle string [out]*/
	struct XMLElemImgDescStru	img_desc;				/* the description of the image */
	struct XMLElemAttDescStru	att_desc;				/* the description of the attachment */
};

struct FeaImgPullListStru{
	WORD			num_ret_hdl;			/* NbReturnedHandles [in/out] : 0xffff or 0 or others. */
	WORD			list_start_off;			/* ListStartOffset */
	struct XMLElemFiltDescStru* hdl_desc;	/* the filter paramerters of the handle descriptor */
};

struct FeaImgPullObjStru{
	UCHAR			type;					/* the object type to be pulled or deleted */	
	UCHAR			img_hdl[IMG_HDL_LEN+1];	/* the image handle string */
	UCHAR			name[ATTACH_NAME_LEN];	/* the attachment file name,only valid for attachment */
};

struct FeaImgPrintStru{
	UCHAR			name[IMG_NAME_LEN];		/* the full name path of the image to be printed */
	DWORD			len;					/* the maximum length of the image to be printed every time.*/
};

struct FeaImgRemCamStru{
	UCHAR			img_hdl[IMG_HDL_LEN+1];	/* the image handle string [out]*/
	IMG_FILE_HDL	img_file_hdl;			/* the file handle of the stored image [out]*/	
	UCHAR			flag;					/* 0: the native image; 1: the thumbnail image */
};

/*Image Push Feature */
UCHAR IMGFEA_ImgPush(BIPHDL hdl/*[in]*/,struct FeaImgPushStru** img/*[in/out]*/,WORD count/*[in]*/);

/*Image Pull Feature */			
UCHAR IMGFEA_ImgPullList(BIPHDL hdl/*[in]*/,struct FeaImgPullListStru* in/*[in/out]*/,struct XMLElemImgListStru** list/*[out]*/,WORD* count/*[out]*/);
UCHAR IMGFEA_ImgPullProp(BIPHDL hdl/*[in]*/,UCHAR img_hdl[IMG_HDL_LEN+1]/*[in]*/,struct BtList** prop_list/*[out]*/);
UCHAR IMGFEA_ImgPullObj(BIPHDL hdl/*[in]*/,struct FeaImgPullObjStru* obj/*[in]*/,IMG_FILE_HDL file_hdl/*[in/out]*/);

/*Advanced Image Printing Feature */
UCHAR IMGFEA_AdvPrintReq(BIPHDL hdl/*[in]*/,UCHAR* svc_id/*[in]*/,IMG_FILE_HDL file_hdl/*[in]*/);
UCHAR IMGFEA_AdvPrintRsp(BIPHDL hdl/*[in]*/,UCHAR* svc_id/*[in]*/,struct FeaImgPrintStru* obj/*[in]*/,WORD count/*[in]*/);

/*Automatic Archive Feature */
UCHAR IMGFEA_ImgAutoArchReq(BIPHDL hdl/*[in]*/,UCHAR* svc_id/*[in]*/);
UCHAR IMGFEA_ImgAutoArchRsp(BIPHDL hdl/*[in]*/,UCHAR* svc_id/*[in]*/,struct FeaImgPullListStru* in/*[in]*/);

/*Remote Camera Feature */
UCHAR IMGFEA_ImgRemView(BIPHDL hdl/*[in]*/,struct FeaImgRemCamStru* rem/*[in/out]*/);
UCHAR IMGFEA_ImgRemCapture(BIPHDL hdl/*[in]*/);
UCHAR IMGFEA_ImgRemStop(BIPHDL hdl/*[in]*/);

/* some helper functions */
UCHAR* IMGHLP_EncodeDesc(UCHAR desc_type,UCHAR* data,WORD* len);
UCHAR* IMGHLP_DecodeDesc(UCHAR desc_type,UCHAR* data,WORD len);
UCHAR* IMGHLP_DecodeXMLObj(UCHAR xml_type,IMG_FILE_HDL xml_file_hdl,WORD count);

/* the structure of the parameter of the callback function
   for the event BTAPP_EV_BIP_ADV_PRINT*/
struct BIPEvPrint{
	UCHAR			svc_id[SERVICE_ID_LEN];		/* the service id */
	IMG_FILE_HDL	file_hdl;					/* the handle of the print-control object */
};

/* for the event BTAPP_EV_BIP_START_PRINT */
struct BIPEvObj{
	DWORD		total_len;						/* the total length of the image to be printed */
	DWORD		part_len;						/* the length of the current part to be printed */
	UCHAR*		data;							/* the body of the current part */
};

/* for the event BTAPP_EV_BIP_REM_DISP */
struct BIPEvDisp{
	UCHAR		tag;				/* [in] the display style */
	UCHAR*		img_hdl;			/* [out] the handle of the current displayed image*/
};

/* for the other objects, it is the event BTAPP_EV_BIP_NORMAL especially for tester */
struct BIPEvNormal{
	UCHAR*		type;				/* the object type pointer */
	UCHAR		code;				/* the response code */
	UCHAR		is_put;				/* the operation is PUT or GET command */
};

#endif

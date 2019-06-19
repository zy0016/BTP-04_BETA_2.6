/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    img_parser.h
Abstract:
	This module defines all the constants and structures used to parse the XML
	format objects.
Author:
    Luo hongbo
Revision History:
	2002.10.18		created
---------------------------------------------------------------------------*/
#ifndef IMG_PARSER_H
#define IMG_PARSER_H

////////////////  PART 3 : XML Reader/Writer ///////////////////////////////////
typedef DWORD IMG_DATA_HDL ;			/* XML parser handle */
#define INVALID_IMG_DATA_HDL 0			/* invalid XML parser handle */

typedef DWORD IMG_ELEM_HDL ;			/* XML encoding handle */
#define INVALID_IMG_ELEM_HDL 0			/* invalid XML encoding handle */

#ifndef IMG_HDL_LEN
#define IMG_HDL_LEN			7
#endif
#define IMG_TIME_LEN		16
#define IMG_THUMB_WIDTH		160
#define IMG_THUMB_HEIGHT	120

#define IMG_XML_NAME_LEN		256
#define IMG_XML_CONTENT_LEN		20
#define IMG_XML_CHARSET_LEN		20

/* the type of the file to be analyzed */
#define IMG_XML_TYPE_LISTING	0x01	/* Images-Listing */
#define IMG_XML_TYPE_PROP		0x02	/* Image-properties */
#define IMG_XML_TYPE_CAPA		0x03	/* Imaging-capabilities */
#define IMG_XML_TYPE_HDLDESC	0x04	/* Image Handles Descriptor */	
#define IMG_XML_TYPE_IMGDESC	0x05	/* Image Descriptor */
#define IMG_XML_TYPE_ATTACHDESC	0x06	/* Attachment Descriptor */

/* the elements of the XML files */
#define IMG_XML_ELEM_IMGSLIST	0x10	/* "images-listing"*/
#define IMG_XML_ELEM_IMG		0x20	/* "image" */
#define IMG_XML_ELEM_IMGPROP	0x30	/* "image-properties" */
#define IMG_XML_ELEM_NAT		0x40	/* "native"*/
#define IMG_XML_ELEM_VAR		0x50	/* "variant"*/
#define IMG_XML_ELEM_ATT		0x60	/* "attachment"*/
#define IMG_XML_ELEM_IMGCAPA	0x70	/* "imaging-capabilities"*/
#define IMG_XML_ELEM_IMGFMT		0x80	/* "image-formats"*/
#define IMG_XML_ELEM_PREFMT		0x90	/* "preferred-format"*/
#define IMG_XML_ELEM_ATTFMT		0xa0	/* "attachment-formats"*/
#define IMG_XML_ELEM_FILTPARA	0xb0	/* "filtering-parameters"*/
#define IMG_XML_ELEM_DPOFOPT	0xc0	/* "DPOF-options"*/
#define IMG_XML_ELEM_IMGHDLDESC	0xd0	/* "image-handles-descriptor" */
#define IMG_XML_ELEM_IMGDESC	0xe0	/* "image-descriptor" */
#define IMG_XML_ELEM_ATTDESC	0xf0	/* "attachment-descriptor" */

/* the encoding style */
#define IMG_XML_ENCODE_JPG		0x01	/* JPEG */
#define IMG_XML_ENCODE_GIF		0x02	/* GIF */
#define IMG_XML_ENCODE_WBMP		0x03	/* WBMP*/
#define IMG_XML_ENCODE_PNG		0x04	/* PNG */
#define IMG_XML_ENCODE_JPG2000	0x05	/* JPEG2000 */
#define IMG_XML_ENCODE_BMP		0x06	/* BMP */
#define IMG_XML_ENCODE_USRNOKIA	0x07	/* User defined format */

/* the transformation methods */
#define IMG_XML_TRANS_STRETCH	0x01	/* "stretch" */
#define IMG_XML_TRANS_FILL		0x02	/* "fill" */
#define IMG_XML_TRANS_CROP		0x04	/* "crop" */

/* the filtering type */
#define IMG_XML_FILTER_CREAT	0x01	/* filter object based on creation date */
#define IMG_XML_FILTER_MODIFY	0x02	/* filter object based on modification date */

/* DPOF attributes */
#define IMG_XML_DPOF_STDPRINT	0x01	/* support for DPOF stand print */
#define IMG_XML_DPOF_IDXPRINT	0x02	/* support for DPOF index print */
#define IMG_XML_DPOF_MULTPRINT	0x04	/* support for DPOF multiple image print */
#define IMG_XML_DPOF_SPECPRINT	0x08	/* support for DPOF specific size print */
#define IMG_XML_DPOF_QTY		0x10	/* support for the DPOF QTY parameter */
#define IMG_XML_DPOF_DSC		0x20	/* support for the DPOF DSC parameter */
#define IMG_XML_DPOF_TRM		0x40	/* support for the DPOF TRM parameter */

#define PIXEL_SIZE(p)	((DWORD)((WORD)(p>>16)*(WORD)(p&0xffff)))
/* elem: IMG_XML_ELEM_IMG type: IMG_XML_TYPE_LISTING */
struct XMLElemImgListStru{			
	UCHAR		img_hdl[IMG_HDL_LEN+1];			/* the image handle string */
	UCHAR		create_time[IMG_TIME_LEN+1];	/* the created time string */
	UCHAR		modify_time[IMG_TIME_LEN+1];	/* the modified time string */
};

/* elem: IMG_XML_ELEM_IMGPROP */
struct XMLElemPropStru{
	UCHAR		img_hdl[IMG_HDL_LEN+1];			/* the image handle string */
	UCHAR		friend_name[IMG_XML_NAME_LEN];	/* A human readable name for the image */
};

/* elem: IMG_XML_ELEM_NAT */
struct XMLElemNatStru{
	DWORD		lower_pixel;					/* The highest two bytes represent Width, the other two bytes represent Height, 
												   If upper_pixel is 0, lower_pixel represents the only pixel the image supports;
												   else lower_pixel repsents the lower limit of the pixel range.	*/
	DWORD		upper_pixel;					/* The highest two bytes represent Width, the other two byptes represent Height,
												   and it represents the upper limit of the pixel range. its value may be 0	*/
	DWORD		size;							/* the size in bytes of the image file */
	UCHAR		encode;							/* the encoding style */
};

/* elem: IMG_XML_ELEM_VAR */
struct XMLElemVarStru{
	struct XMLElemNatStru var;					/* some information of the variant image */
	UCHAR		transform;						/* the list of supported image transformation methods */
};

/* elem: IMG_XML_ELEM_ATTFMT */
struct XMLElemAttfmtStru{
	UCHAR	content_type[IMG_XML_CONTENT_LEN];	/* the MIME content type of the attachment */	
	UCHAR	charset[IMG_XML_CHARSET_LEN];		/* the MIME character set of the attachment */
};

/* elem: IMG_XML_ELEM_ATT; type: IMG_XML_TYPE_ATTACHDESC */
struct XMLElemAttDescStru{
	DWORD		size;							/* the size in bytes of an attachment */
	struct XMLElemAttfmtStru att;				/* attach information */
	UCHAR		name[IMG_XML_NAME_LEN];			/* the file name of the attachment */
	UCHAR		create_time[IMG_TIME_LEN+1];	/* the created time of the attachment */
};

/* elem: IMG_XML_ELEM_ATT type: IMG_XML_TYPE_PROP */
struct XMLElemAttPropStru{
	struct XMLElemAttDescStru att;				/* attach information */
	UCHAR		modify_time[IMG_TIME_LEN+1];	/* the modified time of the attachment */
};

/* elem: IMG_XML_ELEM_PREFMT */
#define XMLElemPreffmtStru	XMLElemVarStru

/* elem: IMG_XML_ELEM_IMGFMT */
#define XMLElemImgfmtStru	XMLElemNatStru

/* elem: IMG_XML_ELEM_FILTPARA; type: IMG_XML_TYPE_CAPA */
struct XMLElemFiltCapaStru{
	DWORD		lower_pixel;					/* the same meaning with "struct XMLElemNatStru"*/
	DWORD		upper_pixel;					/* the same meaning with "struct XMLElemNatStru"*/
	UCHAR		filter;							/* the filter type */
	UCHAR		encode;							/* the encoding style */
};

/* elem: IMG_XML_ELEM_DPOFOPT */
/* just one byte: UCHAR */

/* elem: IMG_XML_ELEM_FILTPARA;type: IMG_XML_TYPE_HDLDESC */
struct XMLElemFiltDescStru{
	UCHAR		encode;							/* the encoding style */
	UCHAR		reserved[3];					/* reserved for alignment */
	DWORD		lower_pixel;					/* the same meaning with the "struct XMLElemNatStru" */
	DWORD		upper_pixel;					/* the same meaning with the "struct XMLElemNatStru" */
	UCHAR		lower_create[IMG_TIME_LEN+1];	/* the lower value of the range of the created time */
	UCHAR		upper_create[IMG_TIME_LEN+1];	/* the upper value of the range of the created time */
	UCHAR		lower_modify[IMG_TIME_LEN+1];	/* the lower value of the range of the modified time */
	UCHAR		upper_modify[IMG_TIME_LEN+1];	/* the upper value of the range of the modified time */
};

/* elem: IMG_XML_ELEM_IMG; type: IMG_XML_TYPE_IMGDESC */
struct XMLElemImgDescStru{
	struct XMLElemVarStru var;
	DWORD				  maxsize;
};

/* The following three are decoding functions for XML objects */
typedef struct XMLFindDataStru{
	UCHAR	elem_type;
	UCHAR	flag;		/* reserved */
	UCHAR*	data;
}XMLFINDDATA,*PXMLFINDDATA;

#ifdef CONFIG_BIP_XML_PARSER
IMG_DATA_HDL  IMGXML_ParseFirstFileEx(UCHAR* data/*[in]*/,WORD len/*[in]*/,UCHAR file_type/*[in]*/,XMLFINDDATA* find_data/*[in/out]*/);
IMG_DATA_HDL  IMGXML_ParseFirstFile(IMG_FILE_HDL file/*[in]*/,UCHAR file_type/*[in]*/,XMLFINDDATA* find_data/*[in/out]*/);
int IMGXML_ParseNextFile(IMG_DATA_HDL hdl/*[in]*/,XMLFINDDATA* find_data/*[in/out]*/);	/* 0: success; -1: failure. */
void IMGXML_ParseClose(IMG_DATA_HDL hdl/*[in]*/);
#endif

IMG_ELEM_HDL  IMGXML_EncodeFirstElemEx(UCHAR file_type/*[in]*/,UCHAR* data/*[in]*/,UCHAR* buf/*[in/out]*/,WORD len/*[in]*/);
IMG_ELEM_HDL  IMGXML_EncodeFirstElem(UCHAR file_type/*[in]*/,UCHAR* data/*[in]*/,IMG_FILE_HDL file/*[in/out]*/);
WORD IMGXML_EncodeNextElem(IMG_ELEM_HDL hdl/*[in]*/,UCHAR elem_type/*[in]*/,UCHAR* data/*[in]*/);	/* return the length */
WORD IMGXML_EncodeClose(IMG_ELEM_HDL hdl/*[in]*/);	/* return the length */
WORD IMGXML_GetCurElemLen(IMG_ELEM_HDL hdl/*[in]*/);

#endif

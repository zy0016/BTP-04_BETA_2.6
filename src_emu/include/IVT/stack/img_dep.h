/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
Module Name:
    img_dep.h
Abstract:
	This module defines the prototype of all the system depedent functions.
Author:
    Luo hongbo
Revision History:
	2002.10.10		created
---------------------------------------------------------------------------*/
#define IMG_FILE_HDL		DWORD		/* the handle of the image file */
#define INVALID_IMG_FILE_HDL 0			/* the value of invalid image file handle */
#define IMG_FILE_FIND_HDL	DWORD		/* the image file find handle */
#define INVALID_IMG_FILE_FIND_HDL	0	/* invalid image file find handle */

#define TYPE_IMG_FILE		0x00		/* the native or thumbnail or variant image file */
#define TYPE_THM_IMG_FILE	0x01		/* the thumbnail file */
#define TYPE_MON_IMG_FILE	0x02		/* the monitoring image file */
#define TYPE_PART_IMG_FILE	0x03		/* the partial image file */
#define TYPE_ATT_FILE		0x04		/* the attachment of the file */
#define TYPE_TMP_FILE		0x05		/* the temporary file */

#define TYPE_ELEM_NAT		0x00
#define TYPE_ELEM_VAR		0x01
#define TYPE_ELEM_ATT		0x02

int IMGFILE_CreateStore(const char* path);

IMG_FILE_HDL IMGFILE_OpenFile(UCHAR type/*[in]*/,UCHAR* img_id/*[in]*/,UCHAR* prop/*[in]*/);	
IMG_FILE_HDL IMGFILE_OpenFileEx(UCHAR* path/*[in]*/);
IMG_FILE_HDL IMGFILE_CreateFile(UCHAR type/*[in]*/,UCHAR* img_id/*[in]*/,UCHAR* prop/*[in]*/,UCHAR img_hdl[]/*[out]*/);	
IMG_FILE_HDL IMGFILE_CreateFileEx(UCHAR* path/*[in]*/);


WORD  IMGFILE_GetImgProp(UCHAR* img_hdl/*[in]*/,UCHAR elem_type/*[in]*/,UCHAR** prop_list/*[in/out]*/);	
void  IMGFILE_CloseFile(IMG_FILE_HDL hdl/*[in]*/);	
DWORD IMGFILE_WriteFile(IMG_FILE_HDL hdl/*[in]*/,UCHAR* buf/*[in]*/,DWORD len/*[in]*/);	
DWORD IMGFILE_ReadFile(IMG_FILE_HDL hdl/*[in]*/,UCHAR* buf/*[in/out]*/,DWORD len/*[in]*/,UCHAR* is_end/*[out]*/);
DWORD IMGFILE_GetFileSize(IMG_FILE_HDL hdl/*[in]*/);
void  IMGFILE_Rewind(IMG_FILE_HDL hdl/*[in]*/);	
void  IMGFILE_SetFilePos(IMG_FILE_HDL hdl/*[in]*/,DWORD off/*[in]*/);
BOOL  IMGFILE_DelFile(UCHAR* img_hdl/*[in]*/);

IMG_FILE_FIND_HDL IMGFILE_FindFirst(WORD offset/*[in]*/,UCHAR* data/*[in/out]*/);
int	IMGFILE_FindNext(IMG_FILE_FIND_HDL hdl/*[in]*/,UCHAR* data/*[in/out]*/);	/* 0: success; else failure ; */
void IMGFILE_FindClose(IMG_FILE_FIND_HDL hdl/*[in]*/);

#ifndef CONFIG_BIP_XML_PARSER
IMG_FILE_HDL IMGFILE_CreateFileWithProp(UCHAR type/*[in]*/,UCHAR* img_name,UCHAR* prop_data/*[in]*/,WORD prop_len/*[in]*/,UCHAR img_hdl[]/*[out]*/);
#endif

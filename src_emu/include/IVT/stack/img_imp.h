#ifndef IMG_IMP_H
#define IMG_IMP_H

/* some interface to be implemented */
IMG_FILE_HDL IMGIMP_CreateThumbnail(UCHAR* path);
IMG_FILE_HDL IMGIMP_CreateImgByProp(UCHAR* path,UCHAR* prop);
void		 IMGIMP_GetNatImgProp(UCHAR* path,UCHAR* prop);
IMG_FILE_HDL IMGIMP_OpenImageByProp(UCHAR* path,UCHAR* prop);
IMG_FILE_HDL IMGIMP_OpenThumbnail(UCHAR* path);
IMG_FILE_HDL IMGIMP_OpenMonImgFile(void);
UCHAR*       IMGIMP_GetImgVarProp(UCHAR* path,WORD* count);

#endif

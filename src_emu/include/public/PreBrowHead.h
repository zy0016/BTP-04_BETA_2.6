 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Public
 *
 * Purpose  : ����Ԥ��ģ�鹫�õ�List����洢�ṹ
 *            ���õĵز㺯���ӿ�
 *            
\**************************************************************************/

#ifndef	_PREBROWHEAD_H
#define	_PREBROWHEAD_H

#include		"window.h"
#include		"string.h"
#include		"setup/setting.h"
//#include		"fapi.h"
#include		"sys/types.h"
#include		"sys/stat.h"
#include		"fcntl.h"
#include		"unistd.h" 

#include		"dirent.h"

#include		"winpda.h"
#include		"malloc.h"
#include		"stdlib.h"
#include		"stdio.h" 
#include		"plx_pdaex.h"
#include		"str_plx.h"
#include		"str_public.h"
//#include		"hp_icon.h"
#include		"hpimage.h"
#include		"mullang.h"
/************************************************************************/
/* ����λ�ú�                                                           */
/************************************************************************/

#define		PREBROW_WND_X			0
#define		PREBROW_WND_Y			0
#define		PREBROW_WND_WIDTH		176//240
#define		PREBROW_WND_HEIGHT		205//240

#define		PREBROW_MAXFILENAMELEN	51//260//50				/*	����ļ����Ƴ���	*/
#define		PREBROW_MAXFULLNAMELEN	256//350				/*	����ļ�����+·������	*/
#define		PREBROW_MAXPATHLEN		256//300				/*	���·������		*/

#define		PREBROW_IMAGELIMIT_WIDTH		480//240		///���ͼƬ�Ŀ��
#define		PREBROW_IMAGELIMIT_HEIGHT		640//320		///���ͼƬ�ĸ߶�

#define		IDS_TOOBIG			ML("The file is Too big")	//"ͼƬ̫���޷���ʾ"

#define     PREBROW


/*******************************************************************\
	(((ListBoxӳ������ݽṹ�ͽӿ�
\*******************************************************************/
typedef	struct	tagFORMVIEWER{
	char	FVbuf[32];
	struct	tagFORMVIEWER *pNext;
}FORMVEWERDATA, *PFORMVEWERDATA;

typedef	struct	tagLISTADATA{
	int		nIndex;
	int		nFVLine;//the line number to show pFVData 
	int		nVisibleLine;
	char	*szData;//[PREBROW_MAXFILENAMELEN];
	char	*szFullData;//[PREBROW_MAXFULLNAMELEN];
	//char	FVbuf[10][26];
	PFORMVEWERDATA	pFormViewerContent;
//	char	*pFVData;		//	only used for Formviewer

	HBITMAP	hbmp;			//for picture
	unsigned long	atime;

	struct	tagLISTADATA	*pPre;
	struct	tagLISTADATA	*pNext;
}LISTDATA, *PLISTDATA;

typedef	struct	tagLISTBUF{
	int			nDataNum;
	int			nCurrentIndex;

	PLISTDATA	pCurrentData;
	PLISTDATA	pDataHead;
}LISTBUF, *PLISTBUF;


void	PREBROW_InitListBuf	(PLISTBUF pListBuf);
BOOL	PREBROW_AddData (PLISTBUF pListBuf, const char* szData, const char* szFullData);
BOOL	PREBROW_GetData (PLISTBUF pLiSTBuf, int nIndex, char* szGetData, char* szFullData);
BOOL	PREBROW_InsertData (PLISTBUF pLiSTBuf, int nIndex, char* szData, char* szFullData);
BOOL	PREBROW_DelData (PLISTBUF pListBuf, int nIndex);
int		PREBROW_FindData (PLISTBUF pListBuf, char* szFindData);
void	PREBROW_FreeListBuf (PLISTBUF pListBuf);
PLISTDATA	PREBROW_AddPicData (PLISTBUF pListBuf, const char* szData, 
								const char* szFullData, HBITMAP	hbmp, unsigned long atime);
BOOL	PREBROW_InsertPicData (PLISTBUF pListBuf, int nIndex, char* szData, char* szFullData, HBITMAP	hbmp, unsigned long atime);
BOOL	PREBROW_ModifyData (PLISTBUF pListBuf, int nIndex,PSTR szData, PSTR szFullData);
PLISTDATA	PREBROW_GetPDATA(PLISTBUF pListBuf, int nIndex );
BOOL	FormViewerAddData(PFORMVEWERDATA pLinkHead,PSTR pString);
BOOL	JoinAllNodeToString(PFORMVEWERDATA pLinkHead, int nBegin, PSTR pString);
/*******************************************************************\
	ListBoxӳ������ݽṹ�ͽӿ�))))
\*******************************************************************/

/********************************************************************\
\********************************************************************/
#define		PREBROW_FILETYPE_BMP	0
#define		PREBROW_FILETYPE_JPG	1
#define		PREBROW_FILETYPE_JPEG	2
#define		PREBROW_FILETYPE_GIF	3
#define		PREBROW_FILETYPE_WBMP	4
#define		PREBROW_FILETYPE_PNG	12
#define		PREBROW_FILETYPE_ICON	13

#define		PREBROW_FILETYPE_WAV	5
#define		PREBROW_FILETYPE_MID	6
#define		PREBROW_FILETYPE_MMF	7
#define		PREBROW_FILETYPE_AMR	10

#define		PRBROW_FILETYPE_TEXT	8
#define		PREBROW_FILETYPE_FOLDER	11
#define		PREBROW_FILETYPE_MEMEORY	14

#define		FILE_TYPE_UNKNOW		9
/*----------------------------------------------*/
#define		PREBROW_FILETAIL_BMP	".bmp"
#define		PREBROW_FILETAIL_JPG	".jpg"
#define		PREBROW_FILETAIL_JPEG	".jpeg"
#define		PREBROW_FILETAIL_GIF	".gif"
#define		PREBROW_FILETAIL_WBMP	".wbmp"

#define		PREBROW_FILETAIL_WAV	".wav"	
#define		PREBROW_FILETAIL_MID	".mid"
#define		PREBROW_FILETAIL_MMF	".mmf"
#define		PREBROW_FILETAIL_AMR	".amr"

#define		PRBROW_FILETAIL_TEXT	".txt"
/*----------------------------------------------*/
#define		PREBROW_FILEUNIVERSE_BMP	".bmp"
#define		PREBROW_FILEUNIVERSE_JPG	".jpg"
#define		PREBROW_FILEUNIVERSE_JPEG	".jpeg"
#define		PREBROW_FILEUNIVERSE_GIF	".gif"
#define		PREBROW_FILEUNIVERSE_WBMP	".wbmp"
#define		PREBROW_FILEUNIVERSE_PNG	".png"

#define		PREBROW_FILEUNIVERSE_WAV	".wav"	
#define		PREBROW_FILEUNIVERSE_MID	".mid"
#define		PREBROW_FILEUNIVERSE_MMF	".mmf"
#define		PREBROW_FILEUNIVERSE_AMR	".amr"

#define		PRBROW_FILEUNIVERSE_TEXT	".txt"

//��д
#define		PREBROW_FILEUNIVERSE_BMP1	".BMP"
#define		PREBROW_FILEUNIVERSE_JPG1	".JPG"
#define		PREBROW_FILEUNIVERSE_JPEG1	".JPEG"
#define		PREBROW_FILEUNIVERSE_GIF1	".GIF"
#define		PREBROW_FILEUNIVERSE_WBMP1	".WBMP"
#define		PREBROW_FILEUNIVERSE_PNG1	".PNG"

#define		PREBROW_FILEUNIVERSE_WAV1	".WAV"	
#define		PREBROW_FILEUNIVERSE_MID1	".MID"
#define		PREBROW_FILEUNIVERSE_MMF1	".MMF"
#define		PREBROW_FILEUNIVERSE_AMR1	".AMR"

#define		PRBROW_FILEUNIVERSE_TEXT1	".TXT"
/*----------------------------------------------*/


/* ����ĳ���͵��ļ������ļ����Ƽ��뵽hList�б�֮�� */
int	FindFileAndAddToList(HWND hList, const char* pFilePath, const unsigned char* pFileType,int nItemData, 
						 int nImageIndex, LISTBUF* ListBuffer);

//void	InitGrpImage(int nGrpNum, const	char** szImageFile);
//void	FreeGrpImage(int nGrpNum);
/* ��hList�б�֮�еõ��ļ����� */
BOOL	GetFileNameFromList(HWND hList, PLISTBUF ListBuffer, PCSTR pPath, char* cFileName, char* szFullFileName, int *nFileType);

BOOL	PictureBrowPaint(HWND hWnd, HDC hdc, const char *cFileName, int nFileType,
							 int nHdcShowX, int nHdcShowY, int nHdcShowWidth, int nHdcShowHeight);
/*����ͼƬ��С�õ��ȱ�������Ĵ�С*/
double GetImageStrechSize ( int ScreenWidth, int ScreenHeight, int ImageWidth, int ImageHeight,
							 int *Stretchx, int *Stretchy, int *StretchWidth, int *StretchHeight);

int	GetFileTypeByName(LPCTSTR pFileName, BOOL bType);

BOOL	RealtimeUpdatePicture();
#endif

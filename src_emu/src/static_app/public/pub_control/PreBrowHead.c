/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Public
 *
 * Purpose  : 各个预览模块公用的List缓冲存储结构
 *            
\**************************************************************************/

#include	"PreBrowHead.h"
#include "time.h"
#include "pubapp.h"
#define	PREBROW_MAXGRPNUM	6
//HBITMAP	PREBROW_hImageBmp[PREBROW_MAXGRPNUM];


/*********************************************************************\

**********************************************************************/

void PrebrowPaintBkFunc(HGIFANIMATE hGIFAnimate, int x, int y, HDC hdcMem);
static PFORMVEWERDATA	FormViewerFindTailNode(PFORMVEWERDATA pLinkHead);
static	void	FreeFormViewerData(PFORMVEWERDATA pHead);
///////////////////////////////////////////////////////////////////////////

/*********************************************************************\
* Function	   PREBROW_InitListBuf
* Purpose      初始化List缓冲区
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void	PREBROW_InitListBuf	(PLISTBUF pListBuf)
{
	pListBuf->nCurrentIndex	=	-1;
	pListBuf->nDataNum		=	0;
	pListBuf->pCurrentData	=	NULL;
	pListBuf->pDataHead		=	NULL;
}

BOOL	FormViewerAddData(PFORMVEWERDATA pLinkHead,PSTR pString)
{
	PFORMVEWERDATA	pTail, pNew;

	if (!pString) {
		return FALSE;
	}
	pTail = FormViewerFindTailNode(pLinkHead);
	if (strlen(pTail->FVbuf)<=0) {
		strcpy(pLinkHead->FVbuf, pString);
		return TRUE;
	}
	pNew = malloc(sizeof(FORMVEWERDATA));
	if (!pNew) {
		return FALSE;
	}
	memset(pNew, 0, sizeof(FORMVEWERDATA));
	strcpy(pNew->FVbuf,pString);
	pNew->pNext = NULL;
	pTail->pNext = pNew;
	return TRUE;
}
static	void	FreeFormViewerData(PFORMVEWERDATA pHead)
{
	PFORMVEWERDATA	pTemp,p;
	pTemp = p = pHead;
	while (p) {
		pTemp = p->pNext;
		free(p);
		p = pTemp;
	}
}
BOOL	JoinAllNodeToString(PFORMVEWERDATA pLinkHead, int nIndex, PSTR pString)
{
	PFORMVEWERDATA	pTemp;
	int	i=0;
	pTemp = pLinkHead;
	if (!pString) {
		return FALSE;
	}
	while (i < nIndex) {
		pTemp = pTemp->pNext;
		i++;
	}
	while(pTemp)
	{
		strcat(pString, pTemp->FVbuf);
		strcat(pString, "\n");
		pTemp = pTemp->pNext;
	}
	return TRUE;
}

static PFORMVEWERDATA	FormViewerFindTailNode(PFORMVEWERDATA pLinkHead)
{
	PFORMVEWERDATA	pTemp;
	pTemp = pLinkHead;
	while(pTemp->pNext)
	{
		pTemp = pTemp->pNext;
	}
	return pTemp;
}
/*********************************************************************\
* Function	   PREBROW_AddData
* Purpose      添加数据
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL	PREBROW_AddData (PLISTBUF pListBuf, const char* szData, const char* szFullData)
{
	PLISTDATA	tempdata;
	PLISTDATA	pTail;

	if (pListBuf->nDataNum == 0)//Initialize the chain
	{
		if ( NULL == ( pListBuf->pDataHead = malloc ( sizeof (LISTDATA) )) )
			return FALSE;
		if(szData)
		{
			if (NULL == (pListBuf->pDataHead->szData = malloc(strlen(szData)+1))) 
			{
				return FALSE;
			}
		}
		else
		{
			pListBuf->pDataHead->szData = NULL;
		}

		if (szFullData) 
		{			
			if (NULL == (pListBuf->pDataHead->szFullData = malloc(strlen(szFullData)+1))) 
			{
				return FALSE;
			}
		}
		else
		{
			pListBuf->pDataHead->szFullData = NULL;
		}

		pListBuf->pCurrentData	=	pListBuf->pDataHead;

		pListBuf->pCurrentData->nIndex	=	pListBuf->nDataNum;
		if (szData) {
			strcpy( pListBuf->pCurrentData->szData, szData );
		}
		else
			pListBuf->pCurrentData->szData = NULL;
		if (szFullData) {
			strcpy( pListBuf->pCurrentData->szFullData, szFullData );
		}
		else
			pListBuf->pCurrentData->szFullData = NULL;
		
		pListBuf->pCurrentData->hbmp = NULL;
		pListBuf->pCurrentData->pFormViewerContent = NULL;
	//	pListBuf->pCurrentData->pFVData = NULL;

		pListBuf->pCurrentData->pNext =	pListBuf->pDataHead;
		pListBuf->pCurrentData->pPre  = pListBuf->pDataHead;		


		pListBuf->nCurrentIndex	=	pListBuf->pCurrentData->nIndex;
		pListBuf->nDataNum++;
		return TRUE;
	}
	//add node to the chain
	if (NULL == (tempdata = malloc ( sizeof(LISTDATA) ) ) )
		return FALSE;
	if (szData) 
	{		
		if (NULL == (tempdata->szData = malloc(strlen(szData)+1))) {
			return FALSE;
		}
	}
	else
	{
		tempdata->szData = NULL;
	}
	if (szFullData) {
		
		if (NULL == (tempdata->szFullData = malloc(strlen(szFullData)+1))) {
			return FALSE;
		}
	}
	else
	{
		tempdata->szFullData = NULL;
	}
	tempdata->nIndex = pListBuf->nDataNum;

	if(szData){
		strcpy(tempdata->szData , szData);
	}
	if (szFullData) {
		strcpy(tempdata->szFullData , szFullData);
	}
	
	tempdata->hbmp = NULL;
	tempdata->pFormViewerContent = NULL;
	//tempdata->pFVData = NULL;

	pTail = pListBuf->pDataHead->pPre;

	pTail->pNext = tempdata;
	tempdata->pNext = pListBuf->pDataHead;

	tempdata->pPre  = pTail;
	pListBuf->pDataHead->pPre = tempdata;	
	
	pListBuf->nDataNum++;

	return TRUE;
}

/*********************************************************************\
* Function	   PREBROW_GetData
* Purpose      得到缓冲区的内容
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL	PREBROW_GetData (PLISTBUF pListBuf, int nIndex, char* szGetData, char* szFullData)
{
	if ( (pListBuf->pCurrentData = PREBROW_GetPDATA ( pListBuf, nIndex )) == NULL)
	{
		printf("PREBROW_GetPDATA is NULL!\r\n");
		return FALSE;
	}
	strcpy (szGetData, pListBuf->pCurrentData->szData);	
	strcpy (szFullData, pListBuf->pCurrentData->szFullData);	
	return TRUE;
}


/*********************************************************************\
* Function	   PREBROW_FreeListBuf
* Purpose      释放List缓冲区
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
void	PREBROW_FreeListBuf (PLISTBUF pListBuf)
{
	PLISTDATA	temp;

	pListBuf->pCurrentData = pListBuf->pDataHead;
	while ( pListBuf->pCurrentData != NULL )
	{
		pListBuf->pCurrentData->pPre->pNext = NULL;
		temp = pListBuf->pCurrentData;
		if (temp->hbmp) {
			DeleteObject(temp->hbmp);
		}
		if (temp->szData) {
			free(temp->szData);
			temp->szData = NULL;
		}
		if (temp->szFullData) {
			free(temp->szFullData);
			temp->szFullData;
		}
		if (temp->pFormViewerContent) {
			FreeFormViewerData(temp->pFormViewerContent);
		}
//		if (temp->pFVData) {
//			free(temp->pFVData);
//			temp->pFVData = NULL;
//		}
		pListBuf->pCurrentData = pListBuf->pCurrentData->pNext;
		free ( temp );
	}

	pListBuf->nCurrentIndex	=	-1;
	pListBuf->nDataNum		=	0;
	pListBuf->pDataHead		=	NULL;
}

PLISTDATA	PREBROW_GetPDATA (PLISTBUF pListBuf, int nIndex )
{	
	PLISTDATA	pTempData;
	int			i;

	if ( nIndex > pListBuf->nDataNum || nIndex < 0)
	{
		printf("PREBROW_GetPDATA Fail!\r\n");
		return NULL;
	}
		

	pTempData = pListBuf->pDataHead;
	i	=	0;

	while( i != nIndex )
	{
		pTempData = pTempData->pNext;
		i ++;
	}

	return pTempData;
}

/*********************************************************************\
* Function	   PREBROW_DelData
* Purpose      删除缓冲区的一个节点
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

BOOL	PREBROW_DelData (PLISTBUF pListBuf, int nIndex)
{

	PLISTDATA	pTempData;
	PLISTDATA	pPre;
	PLISTDATA	pRepairIndex;
	int			i;

	if ( nIndex >= pListBuf->nDataNum )
		return FALSE;

	if ( pListBuf->nDataNum == 1)
	{
		
		if (pListBuf->pDataHead->hbmp) 
		{
			DeleteObject(pListBuf->pDataHead->hbmp);
		}
		if (pListBuf->pDataHead->szData) 
		{
			free(pListBuf->pDataHead->szData);
			pListBuf->pDataHead->szData = NULL;
		}
		if (pListBuf->pDataHead->szFullData) 
		{
			free(pListBuf->pDataHead->szFullData);
			pListBuf->pDataHead->szFullData = NULL;
		}
//		if (pListBuf->pDataHead->pFVData) {
//			free(pListBuf->pDataHead->pFVData);
//			pListBuf->pDataHead->pFVData = NULL;
//		}
		if (pListBuf->pDataHead->pFormViewerContent) 
		{
			FreeFormViewerData(pListBuf->pDataHead->pFormViewerContent);
		}
		
		free (pListBuf->pDataHead );
		pListBuf->pCurrentData = NULL;
		pListBuf->pDataHead = NULL;
		pListBuf->nDataNum = 0;
		return TRUE;
	}

	if (nIndex == 0)
	{
		pTempData = pListBuf->pDataHead;
		pPre	  = pTempData->pPre;

		pPre->pNext = pTempData->pNext;
		pTempData->pNext->pPre = pPre;

		pRepairIndex = pListBuf->pDataHead = pTempData->pNext;
		//repair the index
		for (i = pRepairIndex->nIndex; i<pListBuf->nDataNum; i++)
		{
			pRepairIndex->nIndex--;
			pRepairIndex = pRepairIndex->pNext;
		}
		if (pTempData->hbmp) {
			DeleteObject(pTempData->hbmp);
		}
		if (pTempData->szData) {
			free(pTempData->szData);
			pTempData->szData = NULL;
		}
		if (pTempData->szFullData) {
			free(pTempData->szFullData);
			pTempData->szFullData = NULL;
		}
		if (pTempData->pFormViewerContent) {
			FreeFormViewerData(pTempData->pFormViewerContent);
		}
//		if (pTempData->pFVData) {
//			free(pTempData->pFVData);
//			pTempData->pFVData = NULL;
//		}
		free (pTempData);

		pListBuf->nDataNum --;
		return TRUE;
	}

	pTempData = pListBuf->pDataHead;	
	i	=	0;

	while( i != nIndex )
	{
		pTempData = pTempData->pNext;
		i++;
	}

	pPre = pTempData->pPre;

	pPre->pNext = pTempData->pNext;
	pTempData->pNext->pPre = pPre;
	//rebuild the index of the chain
	//if the Node is the last,need not to repair index
	if (nIndex != (pListBuf->nDataNum - 1))
	{
		pRepairIndex = pTempData->pNext;
		for (i = pRepairIndex->nIndex; i<pListBuf->nDataNum; i++)
		{
			pRepairIndex->nIndex--;
			pRepairIndex = pRepairIndex->pNext;
		}
	}
	if (pTempData->hbmp) 
	{
		DeleteObject(pTempData->hbmp);
	}
	if (pTempData->szData) 
	{
		free(pTempData->szData);
		pTempData->szData = NULL;
	}
	if (pTempData->szFullData) 
	{
		free(pTempData->szFullData);
		pTempData->szFullData = NULL;
	}
	if (pTempData->pFormViewerContent) 
	{
		FreeFormViewerData(pTempData->pFormViewerContent);
	}

	free(pTempData);
	pListBuf->nDataNum --;

	return TRUE;
}

/*********************************************************************\
* Function	   PREBROW_FindData
* Purpose      找到缓冲区的一个节点，并返回index值
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

int		PREBROW_FindData (PLISTBUF pListBuf, char* szFindData)
{
	int			i;
	PLISTDATA	pTempData;

	if ( pListBuf->nDataNum == 0)
		return -1;

	pTempData = pListBuf->pDataHead;	
	i	=	0;

	while( stricmp (pTempData->szFullData, szFindData) != 0  )
	{		
		pTempData = pTempData->pNext;
		i++;
		if ( i >= pListBuf->nDataNum )
			return -1;
	}

	return i;
}

BOOL	PREBROW_InsertData (PLISTBUF pListBuf, int nIndex, char* szData, char* szFullData)
{
	PLISTDATA	pInsertData;
	PLISTDATA	pRepairIndex;
	PLISTDATA	pCurData;
	int					i;

	if ((nIndex > pListBuf->nDataNum)||(nIndex < 0)) 
	{
		return FALSE;
	}
	if (nIndex == pListBuf->nDataNum) {
		return PREBROW_AddData(pListBuf, szData, szFullData);
	}
	//create inserted node 
	if (NULL == (pInsertData = ((PLISTDATA)malloc(sizeof(LISTDATA)))))
	{
		return FALSE;
	}
	pInsertData->szData = malloc(strlen(szData)+1);
	if (!pInsertData->szData) {
		return FALSE;
	}
	pInsertData->szFullData = malloc(strlen(szFullData)+1);
	if (!pInsertData->szFullData) {
		return FALSE;
	}
	strcpy(pInsertData->szFullData,szFullData);
	strcpy(pInsertData->szData,szData);
	pInsertData->hbmp	=	NULL;
	pInsertData->nIndex	= nIndex;
	pInsertData->pFormViewerContent = NULL;

	//get the original node which index equal to nindex;
	pCurData = PREBROW_GetPDATA(pListBuf,nIndex);
	
	//insert node 
//	pCurData->pPre->pNext = pInsertData;
//	pInsertData->pPre = pCurData->pPre;
//	pCurData->pPre = pInsertData;
//	pInsertData->pNext = pCurData;

	if (nIndex == 0)
	{
		pListBuf->pDataHead = pInsertData;		
	}
	//repair index
	pRepairIndex = pCurData;
	for (i = pCurData->nIndex; i<pListBuf->nDataNum; i++)
	{
		pRepairIndex->nIndex++;
		pRepairIndex = pRepairIndex->pNext;
		if (pRepairIndex == pListBuf->pDataHead)
		{
			break;
		}
	}
	pCurData->pPre->pNext = pInsertData;
	pInsertData->pPre = pCurData->pPre;
	pCurData->pPre = pInsertData;
	pInsertData->pNext = pCurData;
	pListBuf->nDataNum++;
	return TRUE;
}

BOOL	PREBROW_ModifyData (PLISTBUF pListBuf, int nIndex,PSTR szData, PSTR szFullData)
{
	PLISTDATA	pTempData;
	pTempData = PREBROW_GetPDATA(pListBuf,nIndex);
	if (!pTempData)
	{
		return FALSE;
	}
	if (szData) 
	{
		if (strlen(szData) > strlen(pTempData->szData)) 
		{
			pTempData->szData = realloc(pTempData->szData,strlen(szData)+1);
		}
		strcpy(pTempData->szData,szData);
	}
	if (szFullData) 
	{
		if (strlen(szFullData) > strlen(pTempData->szFullData)) 
		{
			pTempData->szFullData = realloc(pTempData->szFullData,strlen(szFullData)+1);
		}
		strcpy(pTempData->szFullData,szFullData);
	}
	return TRUE;
}

/*********************************************************************\
* Function	   GetImageStrechSize
* Purpose      得到图片的拉伸长度和宽度
* Params	   屏幕宽度，屏幕高度，图片高度，图片宽度，拉伸显示位置x，拉伸显示位置y，拉伸宽度，拉伸高度
* Return	 	   
* Remarks	   
**********************************************************************/
double	GetImageStrechSize ( int ScreenWidth, int ScreenHeight, int ImageWidth, int ImageHeight,
							 int *Stretchx, int *Stretchy, int *StretchWidth, int *StretchHeight)
{
	double	StretchRate;	

	/*图片宽度和高度都小于屏幕宽高*/
	if ( ImageWidth <= ScreenWidth && ImageHeight <= ScreenHeight )
	{
		*StretchWidth	=	ImageWidth;
		*StretchHeight	=	ImageHeight;
		*Stretchx		=	(int)((double)(ScreenWidth - *StretchWidth)/(double)2);
		*Stretchy		=	(int)((double)(ScreenHeight - *StretchHeight)/(double)2);
		return 1;
	}

	/*仅图片高度大于屏幕的高度*/
	if ( ImageWidth <= ScreenWidth && ImageHeight > ScreenHeight )
	{
		StretchRate		=	(double)ScreenHeight/(double)ImageHeight;	
	}

	/*仅图片宽度大于屏幕的宽度*/
	if ( ImageWidth > ScreenWidth && ImageHeight <= ScreenHeight )
	{
		StretchRate		=	(double)ScreenWidth/(double)ImageWidth;	
	}

	/*图片的高度和宽度都大于屏幕的宽度和高度*/
	if ( ImageWidth > ScreenWidth && ImageHeight > ScreenHeight )
	{
		double	tempRate;
		StretchRate = (double)ScreenWidth / (double)ImageWidth;
		tempRate = (double)ScreenHeight/(double)ImageHeight;
		if ( StretchRate > tempRate )
		{
			StretchRate = tempRate;
		}
	}

	*StretchWidth	=	(int) (ImageWidth * StretchRate);
	*StretchHeight	=	(int) (ImageHeight * StretchRate);
	*Stretchx		=	(int)((double)(ScreenWidth - *StretchWidth)/(double)2);
	*Stretchy		=	(int)((double)(ScreenHeight - *StretchHeight)/(double)2);
	return StretchRate;
}




/*********************************************************************\
* Function	   FindFileAndAddToList
* Purpose      把某类型文件加入列表
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
int	FindFileAndAddToList(HWND hList, const char* pFilePath, const unsigned char* pFileType,int nItemData, 
						 int nImageIndex, LISTBUF* ListBuffer)
{
//	int		 fhandle;
//	_FIND_DATA FindData;
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	char temp[PREBROW_MAXFILENAMELEN];
//	char FullAuxText[PREBROW_MAXFILENAMELEN];
	BOOL bContinue;
	int	nRtnNum;	
	char path[PREBROW_MAXPATHLEN];
	char PicSize[18];
	float		tmpsize;
	DIR  *dirtemp = NULL;
	HBITMAP	hbiticon;
	

	if (!hList)
	{
		return -1;
	}


	bContinue = TRUE;
	strcpy(temp,pFileType);
	memset(path, 0x0, PREBROW_MAXPATHLEN);

	strcpy(path, pFilePath);

    nRtnNum = 0;

	dirtemp = opendir(path);

	if(dirtemp == NULL)
		return nRtnNum;
	
	dirinfo = readdir(dirtemp);
	hbiticon = LoadImage(NULL, "/rom/message/unibox/mail_aff.bmp", IMAGE_BITMAP, 16, 16, LR_LOADFROMFILE);
	while(dirinfo && dirinfo->d_name[0] && bContinue)
	{
		int index;
		char	FullName[PREBROW_MAXFULLNAMELEN];

		if(strstr(dirinfo->d_name, temp) == 0)
		{
			dirinfo = readdir(dirtemp);
			continue;
		}
		index = SendMessage(hList,LB_ADDSTRING,NULL,(LPARAM)dirinfo->d_name);
		/*//useless code
		if ( PREBROW_hImageBmp[nImageIndex] != NULL )
			SendMessage(hList, LB_SETIMAGE, (WPARAM)MAKEWPARAM(IMAGE_BITMAP,(WORD)index),(LPARAM)PREBROW_hImageBmp[nImageIndex]);	
		*/
		SendMessage(hList,LB_SETIMAGE,MAKEWPARAM(IMAGE_BITMAP,index),(LPARAM)hbiticon);
		strcpy ( FullName, pFilePath );
		strcat ( FullName, dirinfo->d_name);
		//-------get the stat of current file------------------
		stat(FullName,&filestat);
//		SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(index,-1),(LPARAM)ctime(&(filestat.st_ctime)));
		tmpsize = (float)filestat.st_size/128;
		floattoa(tmpsize, PicSize);
		//sprintf(PicSize, "%d Kb", tmpsize);
		SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(index,-1),(LPARAM)PicSize);
		/*-------------------------------------------------------*/
		PREBROW_AddData(ListBuffer, dirinfo->d_name, FullName);
		nRtnNum++;
		if (LB_ERR != index)
		{
			SendMessage(hList,LB_SETITEMDATA,(WPARAM)index,(LPARAM)nItemData);
		}
		dirinfo = readdir(dirtemp);
		if(dirinfo)
			bContinue = TRUE;
	}
	closedir(dirtemp);

	//SetCurrentDirectory(path);
	chdir(path);
	DeleteObject(hbiticon);

	return nRtnNum;
}
/*********************************************************************\
* Function	   GetFileNameFromList
* Purpose      从列表得文件名
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
BOOL	GetFileNameFromList(HWND hList, LISTBUF *ListBuffer, PCSTR cPath, char* cFileName, char* szFullFileName, int *nFileType)
{
	int index;	

	strcpy(cFileName,"");
	strcpy(szFullFileName,"");
	*nFileType = FILE_TYPE_UNKNOW;

	if (LB_ERR != (index = SendMessage(hList,LB_GETCURSEL,NULL,NULL)))
	{
		if (LB_ERR != SendMessage(hList,LB_GETTEXT,(WPARAM)index,(LPARAM)cFileName))
		{
//			printf("GetFileNameFromList is %s\r\n",cFileName);
			PREBROW_GetData(ListBuffer, index, cFileName, szFullFileName);			
			*nFileType = SendMessage(hList,LB_GETITEMDATA,(WPARAM)index,NULL);

			return TRUE;
		}
	}

	return FALSE;
}

/*********************************************************************\
* Function	   PictureBrowPaint
* Purpose      预览图片界面PAINT过程
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

BOOL	PictureBrowPaint(HWND hWnd,HDC hdc, const char *cFileName, int nFileType,
							 int nHdcShowX, int nHdcShowY, int nHdcShowWidth, int nHdcShowHeight)
{
	HBITMAP hBitmap;
	BITMAP bm;
	COLORREF Color, OldColor;
	BOOL	bTran;
	SIZE	Size;
	BOOL	bGet;
	int		hFile;
	RECT	rect;
//	HGIFANIMATE hGif;
	struct stat *buf = NULL;
	int    nFileSize;
	

	int		ScreenWidth, ScreenHeight;
	int		Stretchx, Stretchy, StretchWidth, StretchHeight;


	if ( -1 == (hFile = open (cFileName, O_RDONLY)) )
		return FALSE;

	buf = malloc(sizeof(struct stat));

    if(buf == NULL)
    {
        close(hFile);
        return NULL;
    }

    memset(buf, 0, sizeof(struct stat));
    stat(cFileName, buf);

    nFileSize = buf->st_size;

    free(buf);
    buf = NULL;
	

	if ( nFileSize == 0 )
	{
		close ( hFile );		
		OldColor = SetBkColor( hdc, RGB(255,255,255));
		DrawText(hdc, "", -1, &rect ,DT_VCENTER| DT_CENTER |DT_CLEAR);
		SetBkColor (hdc, OldColor);
		return FALSE;
	}
	close(hFile);

	bGet	=	GetImageDimensionFromFile(cFileName, &Size);
	if (!bGet)
	{
		OldColor = SetBkColor( hdc, RGB(255,255,255));
		DrawText(hdc, ML("Invalid File"), -1, &rect ,DT_VCENTER| DT_CENTER |DT_CLEAR);
		SetBkColor (hdc, OldColor);
		return FALSE;
	}	
		
	if  ( bGet && ( (Size.cx > PREBROW_IMAGELIMIT_WIDTH) || (Size.cy > PREBROW_IMAGELIMIT_HEIGHT) ) )
	{

		
		/*
		rect.left	=	PREPIC_PIC_X ;
		rect.right	=	PREPIC_PIC_X + PREPIC_PIC_WIDTH ;
		rect.top	=	PREPIC_PIC_Y;
		rect.bottom	=	PREPIC_PIC_Y + PREPIC_PIC_HEIGHT;
		*/
		rect.left	=	nHdcShowX ;
		rect.right	=	nHdcShowX + nHdcShowWidth ;
		rect.top	=	nHdcShowY;
		rect.bottom	=	nHdcShowY + nHdcShowHeight;

		OldColor = SetBkColor	( hdc, RGB(255,255,255));
		DrawText(hdc, IDS_TOOBIG, -1, &rect ,DT_VCENTER| DT_CENTER |DT_CLEAR);
		SetBkColor (hdc, OldColor);
		return FALSE;
	}

	ScreenWidth		=	nHdcShowWidth;
	ScreenHeight	=	nHdcShowHeight;
	GetImageStrechSize (ScreenWidth, ScreenHeight, Size.cx, Size.cy,
						&Stretchx, &Stretchy, &StretchWidth, &StretchHeight);

	switch(nFileType)
	{
	case PREBROW_FILETYPE_GIF:
/*
		{
			if(hGif)
			{
				hdc = GetDC(hWnd);
				PaintAnimatedGIF(hdc, hGif);
				ReleaseDC(hWnd, hdc);
			}
			else
			{
				hGif = StartAnimatedGIFFromFile(hWnd,
					cFileName, nHdcShowX, nHdcShowY, DM_NONE);
			}
			
			SetPaintBkFunc(hGif,PrebrowPaintBkFunc);
			
		}
		break;
		*/


	case PREBROW_FILETYPE_BMP:
		/*
		hBitmap = LoadImage( NULL, cFileName, IMAGE_BITMAP,0, 0, LR_LOADFROMFILE);
		GetObject(hBitmap, sizeof(BITMAP), &bm);
		StretchBlt(
			hdc,
			Stretchx,
			nHdcShowY + Stretchy, 
			StretchWidth, StretchHeight, 
			(HDC)hBitmap,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY
			);
		DeleteObject(hBitmap);
		break;*/

	case PREBROW_FILETYPE_JPG:
	case PREBROW_FILETYPE_JPEG:
		hBitmap = CreateBitmapFromImageFile(hdc,cFileName,&Color,&bTran);
		GetObject(hBitmap, sizeof(BITMAP), &bm);
		StretchBlt(
			hdc,
			Stretchx,
			nHdcShowY + Stretchy, 
			StretchWidth, StretchHeight, 
			(HDC)hBitmap,0,0,bm.bmWidth,bm.bmHeight,SRCCOPY
			);
		DeleteObject(hBitmap);
		break;

	case PREBROW_FILETYPE_WBMP:
		StretchWBMP(hdc,cFileName,Stretchx,nHdcShowY + Stretchy,StretchWidth,StretchHeight,SRCCOPY);
		break;

	case FILE_TYPE_UNKNOW:
		break;

	default:
		break;
	}

	return TRUE;
}


int	GetFileTypeByName(LPCTSTR pFileName, BOOL bType)
{
	LPCTSTR cTemp = pFileName;
	char ext[5];  
	
    if(cTemp == NULL)
		return FILE_TYPE_UNKNOW;
	
	ext[4] = '\0';

    if(bType)
	{
		while((*cTemp != '.')&&(*cTemp != '\0'))
			cTemp++;
		
		if(*cTemp == '\0')
			return FILE_TYPE_UNKNOW;
		
		cTemp++;
	}

	strcpy(ext, cTemp);

	if(strcasecmp(ext, "BMP") == 0)
	{
		return PREBROW_FILETYPE_BMP;
	}
	if(strcasecmp(ext, "ICO") == 0)
	{
		return PREBROW_FILETYPE_ICON;
	}
	if(strcasecmp(ext, "GIF") == 0)
	{
		return PREBROW_FILETYPE_GIF;
	}
	if((strcasecmp(ext, "JPG") == 0)||(strcasecmp(ext, "JPEG") == 0))
	{
		return PREBROW_FILETYPE_JPG;
	}
	if(strcasecmp(ext, "WBMP") == 0)
	{
		return PREBROW_FILETYPE_WBMP;
	}
	if(strcasecmp(ext, "TXT") == 0)
	{
		return PRBROW_FILETYPE_TEXT;
	}
//	if (stricmp(ext, "MMF") == 0)
//	{
//		return PREBROW_FILETYPE_MMF;
//	}
	if (strcasecmp(ext, "MID") == 0)
	{
		return PREBROW_FILETYPE_MID;
	}
	if (strcasecmp(ext, "WAV") == 0)
	{
		return PREBROW_FILETYPE_WAV;
	}

	if (strcasecmp(ext, "AMR") == 0)
	{
		return PREBROW_FILETYPE_AMR;
	}
	return FILE_TYPE_UNKNOW;

}

/*********************************************************************\
* Function	   InitGrpImage
* Purpose      初始化List图标的句柄
* Params	   图标个数，图标文件名称数组
* Return	 	   
* Remarks	   
**********************************************************************/
/*
void	InitGrpImage(int nGrpNum, const	char** szImageFile)
{
	int i;
	nMaxGrpNum = nGrpNum;
	for ( i=0; i<nGrpNum; i++)
	{
		PREBROW_hImageBmp[i] = LoadImage(NULL, szImageFile[i], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	}
}
*/
/*********************************************************************\
* Function	   FreeGrpImage
* Purpose      释放List图标的句柄
* Params	   图标个数
* Return	 	   
* Remarks	   
**********************************************************************/
/*
void	FreeGrpImage(int nGrpNum)
{
	int i;
	for ( i=0; i<nGrpNum; i++)
	{
		if ( PREBROW_hImageBmp[i] != NULL)
			DeleteObject (PREBROW_hImageBmp[i]);
	}
}
*/
// 画gif的回调函数，刷新背景
//void PaintBkFunc(HGIFANIMATE hGIFAnimate, BOOL End, int x, int y, HDC hdcMem)
void PrebrowPaintBkFunc(HGIFANIMATE hGIFAnimate, int x, int y, HDC hdcMem)
{
    int             width, height;
    RECT            rect;

    width = GetDeviceCaps(hdcMem, HORZRES);
    height = GetDeviceCaps(hdcMem, VERTRES);

    rect.bottom = height;
    rect.left = 0;
    rect.top = 0;
    rect.right = width;

    ClearRect(hdcMem, &rect, RGB(255, 255, 255));
}

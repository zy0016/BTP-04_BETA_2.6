/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PIM
 *
 * Purpose  : preview pictures
 *            
\**************************************************************************/

#include "PicBrowser.h"

typedef struct tagDETAILSWNDDATA {
	HWND	hFramewnd;
	HWND	hFormViewer;			
	PSTR	szCaption;
	PSTR	cFileFullName;
	char	sPicType[10];
	BOOL	bPopupWnd;
}DETAILSDATA, *PDETAILSDATA;
/*
static	BOOL	RegisterOpenFolderWC();
static	LRESULT	OpenFolderWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

BOOL	OpenFolder(HWND	hParent, PCSTR pPath, BOOL IsSelect)
{
	
}
*/
#define	DETAILWCN	"SHOWDETAILWNDCLASS"

#define	IDC_FORMVIEWER	(WM_USER+10)
#define	IDM_OK			(WM_USER+11)
static	BOOL	RegisterDetailWC();
static	LRESULT	PicDetailsWNDProc(HWND	hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
static	BOOL	GetFormatFromType(char *cFormat, int nFileType);
static	BOOL	Details_OnCreate(HWND hwnd, LPARAM lparam, PDETAILSDATA pDetailData);



BOOL	DeleteDirectory(PCSTR	pPath, BOOL bDelSubFolder)
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR  *dirtemp = NULL;
	int		i=0;
	PSTR	strFullName;
//	int		tmp;
	dirtemp = opendir(pPath);
	dirinfo = readdir(dirtemp);
	while (dirinfo&&dirinfo->d_name[0]) {
		strFullName = malloc(strlen(pPath)+strlen(dirinfo->d_name)+2);
		strcpy ( strFullName, pPath );
		if (pPath[strlen(pPath)-1] != '/') {
			strcat(strFullName,"/");
		}
		strcat ( strFullName, dirinfo->d_name);
		stat(strFullName,&filestat);
		if ((((filestat.st_mode&S_IFMT)!=S_IFLNK)&&((filestat.st_mode&S_IFMT)==S_IFDIR)&&(strcmp(dirinfo->d_name,".")!=0)&&(strcmp(dirinfo->d_name,"..")!=0))) 
		{
			//unlink(strFullName);
			if (bDelSubFolder)
			{
				DeleteDirectory(strFullName, TRUE);
			}
			
		}
		else if(!S_ISDIR(filestat.st_mode))
		{
			unlink(strFullName);
		}
		free(strFullName);
		strFullName = NULL;
		dirinfo = readdir(dirtemp);
	}
	closedir(dirtemp);
	if (bDelSubFolder)
	{
		i = rmdir(pPath);
	}
	
	if (i==0) {
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

static	BOOL	RegisterDetailWC()
{
	WNDCLASS  wc;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = PicDetailsWNDProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(DETAILSDATA);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = NULL;
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = DETAILWCN;
	
	if (!RegisterClass(&wc))
		return FALSE;
	return TRUE;
}

BOOL ShowPicDetail(HWND hFrame, PCSTR cFileName, PCSTR cFileFullName, int	nFileType, BOOL bPopup)
{
	HWND	hDetailswnd;
	DETAILSDATA	dDetailData;
	RECT	rf;
	RegisterDetailWC();

	//init window data
	memset(&dDetailData, 0, sizeof(DETAILSDATA));
	dDetailData.hFramewnd = hFrame;
	dDetailData.szCaption = malloc(strlen(cFileName)+1);
	strcpy(dDetailData.szCaption, cFileName);
	dDetailData.cFileFullName = malloc(strlen(cFileFullName)+1);
	strcpy(dDetailData.cFileFullName, cFileFullName);
	GetFormatFromType(dDetailData.sPicType, nFileType);
	dDetailData.bPopupWnd = bPopup;

	if (!bPopup) {
		
		GetClientRect(hFrame, &rf);
		hDetailswnd = CreateWindow(DETAILWCN, NULL, 
			WS_CHILD|WS_VISIBLE,
			rf.left, rf.top,
			rf.right - rf.left,
			rf.bottom -rf.top,
			hFrame, NULL, NULL, (PVOID)&dDetailData);
		if (!hDetailswnd) {
			return FALSE;
		}
		SetWindowText(hFrame, IDS_SHOWDETAIL);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
		ShowWindow(hFrame, SW_SHOW);
		UpdateWindow(hFrame);
		ShowWindow(hDetailswnd, SW_SHOW);
		SetFocus(hDetailswnd);
	}
	else
	{
		hDetailswnd = CreateWindow(DETAILWCN, IDS_SHOWDETAIL, 
			WS_VISIBLE|WS_CAPTION|PWS_STATICBAR,
			PLX_WIN_POSITION,
			hFrame, NULL, NULL, (PVOID)&dDetailData);
		if (!hDetailswnd) {
			return FALSE;
		}
		SendMessage(hDetailswnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_OK,1), (LPARAM)IDS_OK);
		ShowWindow(hDetailswnd, SW_SHOW);
		UpdateWindow(hDetailswnd);
		
		SetFocus(hDetailswnd);
	}
	return TRUE;

}

static	LRESULT	PicDetailsWNDProc(HWND	hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	LRESULT	lResult;
	PDETAILSDATA pDetailData;
	PCREATESTRUCT	pCreateStru;
	
	pDetailData = GetUserData(hwnd);
	switch(umsg) 
	{
	case WM_CREATE:
		{
			struct stat	filestat;
			char	pResolution[20];
			char	pictime[20];
			char	picdate[40];
			char	picsize[20];
			char	color[5];
			SYSTEMTIME	systime;
			struct tm *StTime;
			LISTBUF	listbuffer;
			//SIZE	size;
			RECT	rc;
			BITMAP	bmp;
			HBITMAP	hBmp;
			
			
			
			pCreateStru = (PCREATESTRUCT)lparam;
			memcpy(pDetailData, (PDETAILSDATA)pCreateStru->lpCreateParams, sizeof(DETAILSDATA));
			stat(pDetailData->cFileFullName, &filestat);
			//LOG_FTtoST(&filestat.st_mtime, &systime);

			StTime = gmtime(&filestat.st_mtime);
			systime.wYear = StTime->tm_year+1900;
			systime.wMonth = StTime->tm_mon+1;
			systime.wDay = StTime->tm_mday;
			systime.wDayOfWeek = StTime->tm_wday;
			systime.wHour = StTime->tm_hour;
			systime.wMinute = StTime->tm_min;
			systime.wSecond = StTime->tm_sec;
			//systime.wMilliseconds = 
			GetTimeDisplay(systime, pictime, picdate);
			//sprintf(picsize, "%d Kb", filestat.st_size/1024);
			floattoa((float)filestat.st_size/1024, picsize);
			strcat(picsize, " kB");
			//GetImageDimensionFromFile(pDetailData->cFileFullName, &size);
			hBmp = CreateBitmapFromImageFile(NULL,pDetailData->cFileFullName,NULL,NULL);
			
			//
			if (!hBmp)
			{
				return FALSE;
			}
			memset(&bmp, 0, sizeof(BITMAP));
			GetObject(hBmp, sizeof(BITMAP), (void*)&bmp);
			sprintf(pResolution, "%d x %d", bmp.bmWidth, bmp.bmHeight);
			if (bmp.bmBitsPixel == 1) {
				sprintf(color, "%d", 2);
			}
			else
				sprintf(color, "%d", 4);
			//
			if (hBmp) {
				DeleteObject(hBmp);
			}

			PREBROW_InitListBuf(&listbuffer);
			PREBROW_AddData(&listbuffer, ML("Name:"), pDetailData->szCaption);
			PREBROW_AddData(&listbuffer, ML("Date:"), picdate);
			PREBROW_AddData(&listbuffer, ML("Time:"), pictime);
			PREBROW_AddData(&listbuffer, ML("Format:"), pDetailData->sPicType);
			PREBROW_AddData(&listbuffer, ML("Size:"), picsize);
			PREBROW_AddData(&listbuffer, ML("Resolution:"), pResolution);
			PREBROW_AddData(&listbuffer, ML("Color:"), color);
			
			GetClientRect(hwnd, &rc);
			FormView_Register();
			pDetailData->hFormViewer = CreateWindow(FORMVIEWER, NULL, WS_CHILD|WS_VISIBLE|WS_VSCROLL,
				rc.left, rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				hwnd, (HMENU)IDC_FORMVIEWER, NULL, (PVOID)&listbuffer);
			if (!pDetailData->hFormViewer) {
				return FALSE;
			}
		}
		break;

	case PWM_SHOWWINDOW:
		SetWindowText(pDetailData->hFramewnd, IDS_SHOWDETAIL);
		SendMessage(pDetailData->hFramewnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
		SendMessage(pDetailData->hFramewnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(pDetailData->hFramewnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		
		SendMessage(pDetailData->hFramewnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
		SendMessage(pDetailData->hFramewnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
		//ShowWindow(pDetailData->hFramewnd, SW_SHOW);
		SetFocus(hwnd);
		break;

	case WM_ACTIVATE:
		if (WA_ACTIVE == LOWORD(wparam)) {
			
			ShowWindow(pDetailData->hFormViewer, SW_SHOW);
			SetFocus(pDetailData->hFormViewer);
		}
		break;

	case WM_SETFOCUS:
		SetFocus(pDetailData->hFormViewer);
		break;

	case WM_KEYDOWN:
		switch(wparam) {
		case VK_RETURN:
			if (!pDetailData->bPopupWnd) {
				SendMessage(pDetailData->hFramewnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
			}
			
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		}
		break;

	case WM_DESTROY:
		if (pDetailData->cFileFullName) {
			free(pDetailData->cFileFullName);
			pDetailData->cFileFullName = NULL;
		}
		if (pDetailData->szCaption) {
			free(pDetailData->szCaption);
			pDetailData->szCaption = NULL;
		}
		UnregisterClass(DETAILWCN, NULL);
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	default:
		lResult = PDADefWindowProc(hwnd, umsg, wparam,lparam);

		break;
	}
	return lResult;
}
static	BOOL	GetFormatFromType(char *cFormat, int nFileType)
{
	switch(nFileType) 
	{
	case PREBROW_FILETYPE_JPG:
		strcpy(cFormat, "JPG");
		break;

	case PREBROW_FILETYPE_JPEG:
		strcpy(cFormat, "JPEG");
		break;

	case PREBROW_FILETYPE_BMP:
		strcpy(cFormat, "BMP");
		break;
		
	case PREBROW_FILETYPE_WBMP:
		strcpy(cFormat, "WBMP");
		break;

	case PREBROW_FILETYPE_GIF:
		strcpy(cFormat, "GIF");
		break;

	case PREBROW_FILETYPE_PNG:
		strcpy(cFormat, "PNG");
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

static	BOOL	Details_OnCreate(HWND hwnd, LPARAM lparam, PDETAILSDATA pDetailData)
{
	struct stat	filestat;
	char	pResolution[20];
	char	pictime[20];
	char	picdate[40];
	char	picsize[20];
	char	color[5];
	SYSTEMTIME	systime;
	PCREATESTRUCT	pCreateStru;
	LISTBUF	listbuffer;
	SIZE	size;
	RECT	rc;
	

	
	pCreateStru = (PCREATESTRUCT)lparam;
	memcpy(pDetailData, (PDETAILSDATA)pCreateStru->lpCreateParams, sizeof(DETAILSDATA));
	stat(pDetailData->cFileFullName, &filestat);
	LOG_FTtoST(&filestat.st_mtime, &systime);
	GetTimeDisplay(systime, pictime, picdate);
	sprintf(picsize, "%s Kb", filestat.st_size/1024);
	GetImageDimensionFromFile(pDetailData->cFileFullName, &size);
	sprintf(pResolution, "%d x %d", size.cx, size.cy);
	//
	sprintf(color, "%d", 4);
	//
	PREBROW_InitListBuf(&listbuffer);
	PREBROW_AddData(&listbuffer, ML("Date:"), picdate);
	PREBROW_AddData(&listbuffer, ML("Time:"), pictime);
	PREBROW_AddData(&listbuffer, ML("Format:"), pDetailData->sPicType);
	PREBROW_AddData(&listbuffer, ML("Size:"), picsize);
	PREBROW_AddData(&listbuffer, ML("Resolution:"), pResolution);
	PREBROW_AddData(&listbuffer, ML("Color:"), color);

	GetClientRect(hwnd, &rc);
	pDetailData->hFormViewer = CreateWindow(FORMVIEWER, NULL, WS_CHILD,
		rc.left, rc.top,
		rc.right - rc.left,
		rc.bottom - rc.top,
		hwnd, (HMENU)IDC_FORMVIEWER, NULL, (PVOID)&listbuffer);
}

BOOL GetInfoFromListByIndex(HWND hList, PLISTBUF ListBuffer, char* cFileName, char* szFullFileName, int *nFileType, int index)
{

	strcpy(cFileName,"");
	strcpy(szFullFileName,"");

	PREBROW_GetData(ListBuffer, index, cFileName, szFullFileName);			
	*nFileType = SendMessage(hList,LB_GETITEMDATA,(WPARAM)index,NULL);		

	return FALSE;
}

int	AddFolderToList(HWND hList, PCSTR pFilePath, HBITMAP hOpen, HBITMAP	hNormal, PLISTBUF pListBuf)
{

	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	char	*szCurFolderName;
	char	bufname[41] = "";
	int	nRtnNum;	
	int index;
	DIR  *dirtemp = NULL;

	if (!hList)
	{
		return -1;
	}
    nRtnNum = 0;

	dirtemp = opendir(pFilePath);

	if(dirtemp == NULL)
		return nRtnNum;
	
	SendMessage(hList, LB_RESETCONTENT, 0, 0);
	PREBROW_InitListBuf(pListBuf);
	chdir(pFilePath);

	if (pFilePath[strlen(pFilePath) - 1] != '/') {
		szCurFolderName = strrchr(pFilePath, '/');
		szCurFolderName++;
	}
	else
	{
		//pFilePath[strlen(pFilePath) - 1] = '/';
		szCurFolderName = (PSTR)pFilePath + strlen(pFilePath) -2;
		while (*szCurFolderName != '/')
		{
			szCurFolderName --;
		}
		szCurFolderName++;
	}
	//current folder
	if ((stricmp(pFilePath, "/mnt/flash/pictures") == 0)||(stricmp(pFilePath, "/mnt/flash/pictures/") == 0))
	{
		index = SendMessage(hList,LB_ADDSTRING,NULL,(LPARAM)"Pictures");
	}
	else if ((stricmp(pFilePath, "/mnt/fat16/pictures") == 0)||(stricmp(pFilePath, "/mnt/fat16/pictures/") == 0))
	{
		GetMMCName(bufname);
		index = SendMessage(hList,LB_ADDSTRING,NULL,(LPARAM)bufname);
	}
	else
	{
		index = SendMessage(hList,LB_ADDSTRING,NULL,(LPARAM)szCurFolderName);
	}
	SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hOpen);
	PREBROW_AddData(pListBuf, szCurFolderName, pFilePath);

	while((dirinfo = readdir(dirtemp))!=NULL)
	{
		
		char	FullName[PREBROW_MAXFULLNAMELEN];
		memset(FullName, 0, PREBROW_MAXFULLNAMELEN);
		strcpy ( FullName, pFilePath );
		if (FullName[strlen(FullName) - 1] != '/') {
			strcat(FullName, "/");
		}
		strcat ( FullName, dirinfo->d_name);
		stat(FullName,&filestat);
		/*--------------if FullName is a dir----------------------------*/
		if ((((filestat.st_mode&S_IFMT)!=S_IFLNK)&&((filestat.st_mode&S_IFMT)==S_IFDIR)&&(strcmp(dirinfo->d_name,".")!=0)&&(strcmp(dirinfo->d_name,"..")!=0))) 
		{			
			//sprintf(FileName,"%s",dirinfo->d_name);
			index = SendMessage(hList,LB_ADDSTRING,NULL,(LPARAM)dirinfo->d_name);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hNormal);
			PREBROW_AddData(pListBuf, dirinfo->d_name, FullName);
		}
		
	}
	closedir(dirtemp);

	

	return nRtnNum;
}

int	GetSubFolderNum(PCSTR pFilePath)
{

	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	int	nRtnNum;
	DIR  *dirtemp = NULL;


    nRtnNum = 0;

	dirtemp = opendir(pFilePath);

	if(dirtemp == NULL)
		return nRtnNum;	

	while((dirinfo = readdir(dirtemp))!=NULL)
	{
		
		char	FullName[PREBROW_MAXFULLNAMELEN];
		memset(FullName, 0, PREBROW_MAXFULLNAMELEN);
		strcpy ( FullName, pFilePath );
		if (FullName[strlen(FullName) - 1] != '/') {
			strcat(FullName, "/");
		}
		strcat ( FullName, dirinfo->d_name);
		stat(FullName,&filestat);
		/*--------------if FullName is a dir----------------------------*/
		if ((((filestat.st_mode&S_IFMT)!=S_IFLNK)&&((filestat.st_mode&S_IFMT)==S_IFDIR)&&(strcmp(dirinfo->d_name,".")!=0)&&(strcmp(dirinfo->d_name,"..")!=0))) 
		{			
			nRtnNum++;
		}
		
	}
	closedir(dirtemp);

	return nRtnNum;
}
BOOL PicCopyFile(PCSTR pOldFileName, PCSTR pNewFileName)
{
	int  hfold,hfnew,iCount;
	char buf[BUF_SIZE];
	DWORD nBytes, dwPointer = 0;
	
	hfold = open(pOldFileName, O_RDONLY);
	
	if(hfold == -1)
		return FALSE;
	
	hfnew = open(pNewFileName, O_RDWR|O_CREAT, 0666);
	
	if(hfnew == -1)
	{
		close(hfold);
		return FALSE;
	}
	nBytes = read(hfold, buf, BUF_SIZE);
	
	while (nBytes != 0 && nBytes != -1)
	{
		int ret;
		
		lseek(hfnew, dwPointer, SEEK_SET);
		ret = write(hfnew, buf, nBytes);
		iCount++;
		if (iCount >= KICKDOGSIZE)
		{
			iCount = 0;
			KickDog();
		}
		if(ret != (int)nBytes)
		{
			close(hfnew);
			close(hfold);
			return 0;
		}
		dwPointer += nBytes; 
		lseek(hfold, dwPointer, SEEK_SET);
		nBytes = read(hfold, buf, BUF_SIZE);
		
	}
	close(hfold);
	close(hfnew);
	return TRUE;
	
}
BOOL PicMoveFile(PCSTR pOldFileName, PCSTR pNewFileName)
{
	int  hfold,hfnew,iCount;
	char buf[BUF_SIZE];
	DWORD nBytes, dwPointer = 0;
	
	hfold = open(pOldFileName, O_RDONLY);
	
	if(hfold == -1)
		return FALSE;
	
	hfnew = open(pNewFileName, O_RDWR|O_CREAT, 0666);
	
	if(hfnew == -1)
	{
		close(hfold);
		return FALSE;
	}
	nBytes = read(hfold, buf, BUF_SIZE);
	
	while (nBytes != 0 && nBytes != -1)
	{
		int ret;
		
		lseek(hfnew, dwPointer, SEEK_SET);
		ret = write(hfnew, buf, nBytes);
		iCount++;
		if (iCount >= KICKDOGSIZE)
		{
			iCount = 0;
			KickDog();
		}
		if(ret != (int)nBytes)
		{
			close(hfnew);
			close(hfold);
			return 0;
		}
		dwPointer += nBytes; 
		lseek(hfold, dwPointer, SEEK_SET);
		nBytes = read(hfold, buf, BUF_SIZE);
		
	}
	close(hfold);
	close(hfnew);

	unlink(pOldFileName);
	return TRUE;
	
}
BOOL	IsBigFileFromSD(PCSTR pFile)
{
	struct stat	fs;

	if (!pFile)
	{
		return FALSE;
	}
	stat(pFile, &fs);
	if ((strncmp(pFile, "/mnt/fat16/pictures", 19) == 0)&&(fs.st_size/1024 > 100))
	{
		return TRUE;
	}
	else
		return FALSE;
}

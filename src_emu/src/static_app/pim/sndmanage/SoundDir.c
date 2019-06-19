/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PIM
 *
 * Purpose  : sound application
 *            
\**************************************************************************/

#include "SndManage.h"

typedef struct tagDETAILSWNDDATA {
	HWND	hFramewnd;
	HWND	hFormViewer;			
	PSTR	szCaption;
	PSTR	cFileFullName;
	char	sSndType[10];
}DETAILSDATA, *PDETAILSDATA;
/*
static	BOOL	RegisterOpenFolderWC();
static	LRESULT	OpenFolderWndProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

BOOL	OpenFolder(HWND	hParent, PCSTR pPath, BOOL IsSelect)
{
	
}
*/
#define	DETAILWCN	"SHOWSNDDETAILWNDCLASS"

#define	IDC_FORMVIEWER	(WM_USER+10)
#define	IDM_OK			(WM_USER+11)
static	BOOL	RegisterDetailWC();
static	LRESULT	SndDetailsWNDProc(HWND	hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);
static	BOOL	GetFormatFromType(char *cFormat, int nFileType);
static	BOOL	Details_OnCreate(HWND hwnd, LPARAM lparam, PDETAILSDATA pDetailData);



BOOL	SND_DeleteDirectory(PCSTR	pPath)
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR  *dirtemp = NULL;
	int		i=0;
	PSTR	strFullName;

	if(NULL == pPath)
		return FALSE;

	dirtemp = opendir(pPath);
	if(NULL == dirtemp)
		return FALSE;

	dirinfo = readdir(dirtemp);
	while (dirinfo&&dirinfo->d_name[0])
	{
		if(ISPRESERVEDPATH(dirinfo->d_name))
		{
			dirinfo = readdir(dirtemp);
			continue;
		}
		
		strFullName = malloc(strlen(pPath)+strlen(dirinfo->d_name)+2);
		strcpy ( strFullName, pPath );
		if (!ISPATHSEPARATOR(pPath[strlen(pPath)-1]))
			strcat(strFullName,"/");

		strcat ( strFullName, dirinfo->d_name);
		stat(strFullName,&filestat);
		if (!S_ISLNK(filestat.st_mode) && S_ISDIR(filestat.st_mode)) 
		{
			//unlink(strFullName);
			SND_DeleteDirectory(strFullName);
		}
		else if(!S_ISDIR(filestat.st_mode))
		{
			unlink(strFullName);
		}
		free(strFullName);
		strFullName = NULL;
		dirinfo = readdir(dirtemp);

		KickDog();	// clear the watch dog
	//	GetMessage(&msg, NULL, 0, 0))
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	}
	i = rmdir(pPath);
	closedir(dirtemp);

	return (BOOL)(i==0);
}

// delete all sound files in the folder
// sub-folder will not be processed
BOOL SND_DeleteAllInFolder(PCSTR pPath)
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR  *dirtemp = NULL;
	int		i=0;
	PSTR	strFullName;

	if(NULL == pPath)
		return FALSE;

	dirtemp = opendir(pPath);
	if(NULL == dirtemp)
		return FALSE;
	
	dirinfo = readdir(dirtemp);
	while (dirinfo&&(dirinfo->d_name[0]))
	{
		if(ISPRESERVEDPATH(dirinfo->d_name))
		{
			dirinfo = readdir(dirtemp);
			continue;
		}
		
		strFullName = malloc(strlen(pPath)+strlen(dirinfo->d_name)+2);
		strcpy ( strFullName, pPath );
		if (pPath[strlen(pPath)-1] != '/')
		{
			strcat(strFullName,"/");
		}
		strcat ( strFullName, dirinfo->d_name);
		stat(strFullName,&filestat);
/*		if ((((filestat.st_mode&S_IFMT)!=S_IFLNK)&&((filestat.st_mode&S_IFMT)==S_IFDIR)) 
		{
			//unlink(strFullName);
			SND_DeleteDirectory(strFullName);
		}
		else //*/
		if(!S_ISDIR(filestat.st_mode))
		{
			if(0 != unlink(strFullName))
			{
				free(strFullName);
				closedir(dirtemp);
				return FALSE;
			}
		}
		free(strFullName);
		strFullName = NULL;
		dirinfo = readdir(dirtemp);

		KickDog();	// clear the watch dog
	//	GetMessage(&msg, NULL, 0, 0))
	//	TranslateMessage(&msg);
	//	DispatchMessage(&msg);
	}

	closedir(dirtemp);
	return TRUE;
}

static	BOOL	RegisterDetailWC()
{
	WNDCLASS  wc;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = SndDetailsWNDProc;
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

BOOL ShowSndDetail(HWND hFrame, PCSTR cFileName, PCSTR cFileFullName, int	nFileType)
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
	GetFormatFromType(dDetailData.sSndType, nFileType);

	GetClientRect(hFrame, &rf);
	hDetailswnd = CreateWindow(DETAILWCN, NULL, 
		WS_CHILD|WS_VISIBLE,
		rf.left, rf.top,
		rf.right - rf.left,
		rf.bottom -rf.top,
		hFrame, NULL, NULL, (PVOID)&dDetailData);
	if (!hDetailswnd)
		return FALSE;
	
	SetWindowText(hFrame, IDS_SHOWDETAIL/*cFileName*/);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_LEFTICON), NULL);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_RIGHTICON), NULL);
	ShowWindow(hFrame, SW_SHOW);
	ShowWindow(hDetailswnd, SW_SHOW);
	UpdateWindow(hFrame);
	SetFocus(hDetailswnd);
	
	return TRUE;

}

static	LRESULT	SndDetailsWNDProc(HWND	hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
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
			char	pictime[20];
			char	picdate[40];
			char	picsize[20];
			char	cName[64];
			SYSTEMTIME	systime;
			struct tm *StTime;
			LISTBUF	listbuffer;
			float	fSize;
			RECT	rc;
			
			
			
			pCreateStru = (PCREATESTRUCT)lparam;
			memcpy(pDetailData, (PDETAILSDATA)pCreateStru->lpCreateParams, sizeof(DETAILSDATA));
			stat(pDetailData->cFileFullName, &filestat);

			StTime = gmtime(&filestat.st_mtime);
			systime.wYear = StTime->tm_year + 1900;
			systime.wMonth = StTime->tm_mon + 1;
			systime.wDay = StTime->tm_mday;
			systime.wDayOfWeek = StTime->tm_wday;
			systime.wHour = StTime->tm_hour;
			systime.wMinute = StTime->tm_min;
			systime.wSecond = StTime->tm_sec;

			GetTimeDisplay(systime, pictime, picdate);

			fSize = (float)(filestat.st_size)/1024;
			if(fSize >= 1)
				sprintf(picsize, "%d kB", (int)fSize);
			else
				sprintf(picsize, "0.%d kB", (int)(10*fSize));

			memset(cName, 0, 64);
			strcpy(cName, pDetailData->szCaption);

			PREBROW_InitListBuf(&listbuffer);
			PREBROW_AddData(&listbuffer, ML("Name:"), cName);
			PREBROW_AddData(&listbuffer, ML("Date:"), picdate);
			PREBROW_AddData(&listbuffer, ML("Time:"), pictime);
			PREBROW_AddData(&listbuffer, ML("Format:"), pDetailData->sSndType);
			PREBROW_AddData(&listbuffer, ML("Size:"), picsize);
			
			GetClientRect(hwnd, &rc);
			FormView_Register();
			pDetailData->hFormViewer = CreateWindow(FORMVIEWER, NULL, WS_CHILD|WS_VISIBLE|WS_VSCROLL,
				rc.left, rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				hwnd, (HMENU)IDC_FORMVIEWER, NULL, (PVOID)&listbuffer);
			if (!pDetailData->hFormViewer)
				return FALSE;
		}
		break;

	case PWM_SHOWWINDOW:
		SetWindowText(pDetailData->hFramewnd, IDS_SHOWDETAIL);
		SendMessage(pDetailData->hFramewnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
		SendMessage(pDetailData->hFramewnd, PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(pDetailData->hFramewnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		
		SendMessage(pDetailData->hFramewnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_LEFTICON), NULL);
		SendMessage(pDetailData->hFramewnd, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_RIGHTICON), NULL);
		SetFocus(hwnd);
		break;

	case WM_ACTIVATE:
		if (WA_ACTIVE == LOWORD(wparam))
		{
			ShowWindow(pDetailData->hFormViewer, SW_SHOW);
			SetFocus(pDetailData->hFormViewer);
		}
		break;

	case WM_SETFOCUS:
		SetFocus(pDetailData->hFormViewer);
		break;

	case WM_KEYDOWN:
		switch(wparam)
		{
		case VK_RETURN:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		}
		break;

	case WM_DESTROY:
		if (pDetailData->cFileFullName)
		{
			free(pDetailData->cFileFullName);
			pDetailData->cFileFullName = NULL;
		}
		if (pDetailData->szCaption)
		{
			free(pDetailData->szCaption);
			pDetailData->szCaption = NULL;
		}
		UnregisterClass(DETAILWCN, NULL);
		break;

	case WM_CLOSE:
		PostMessage(pDetailData->hFramewnd, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
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
	case PREBROW_FILETYPE_AMR:
		strcpy(cFormat, "AMR");
		break;

	case PREBROW_FILETYPE_WAV:
		strcpy(cFormat, "WAV");
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

int	SND_AddFolderToList(HWND hList, PCSTR pFilePath, HBITMAP hNormal, PLISTBUF pListBuf)
{

	struct dirent *dirinfo = NULL;
	struct stat	filestat;

	int		nRtnNum;	
	int		index;
	int		nLen;
	char	FullName[MAXFULLNAMELEN];
	char	FileName[MAXFILENAMELEN];
	
	DIR  *dirtemp = NULL;

	if (!hList)
	{
		return -1;
	}

    nRtnNum = 0;

	dirtemp = opendir(pFilePath);

	if(dirtemp == NULL)
		return nRtnNum;
	
	nLen = strlen(pFilePath);
	memset(FullName, 0, MAXFULLNAMELEN);
	strcpy ( FullName, pFilePath );
	
	if (!ISPATHSEPARATOR(FullName[nLen - 1]))
		strcat(FullName, "/");
	
	nLen = strlen(FullName);

	while((dirinfo = readdir(dirtemp))!=NULL)
	{
		if(ISPRESERVEDPATH(dirinfo->d_name))
			continue;
		
		memset(FileName, 0, MAXFILENAMELEN);
//		memset(FullName, 0, MAXFULLNAMELEN);
//		strcpy ( FullName, pFilePath );
//		if (FullName[strlen(FullName) - 1] != '/')
//			strcat(FullName, "/");
		FullName[nLen] = '\0';

		strcat ( FullName, dirinfo->d_name);
		stat(FullName, &filestat);
		/*--------------if FullName is a dir----------------------------*/
		if (!S_ISLNK(filestat.st_mode) && S_ISDIR(filestat.st_mode)) 
		{			
			sprintf(FileName,"%s",dirinfo->d_name);
		//	index = SendMessage(hList,LB_ADDSTRING,NULL,(LPARAM)FileName);
			index = SND_AddListBoxItem(hList, FileName);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hNormal);
			PREBROW_AddData(pListBuf, FileName, FullName);
		}
	}
	closedir(dirtemp);

	chdir(pFilePath);

	return nRtnNum;
}

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/

#include "window.h"
#include "winpda.h"
#include "string.h"
#include "malloc.h"
#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include "dirent.h"
#include "sys\stat.h"
#include "plx_pdaex.h"
#include "setting.h"
#include "pubapp.h"
#include "hpimage.h"
#include "imesys.h"
#include "wUipub.h"
#include "DownLoad.h"

#define DEFCONTENTNUM    10

typedef struct tagDLContTable
{
    char *szConStr;
    char nContType;
}DLCONTABLE;

static DLCONTABLE DLDefContTable[DEFCONTENTNUM] = {
		".mmf",    MMT_AUDIOMMF,
        ".amr",    MMT_AUDIOAMR,
        ".mid",    MMT_AUDIOMIDI,
		".wav",    MMT_AUDIOWAV,
        ".bmp",    MMT_IMGBMP,
        ".gif",    MMT_IMGGIF,
        ".jpg",    MMT_IMGJPG,
        ".png",    MMT_IMGPNG,
        ".wml",    MMT_WML1,
        ".wmlc",   MMT_WMLC,
};

#define WML_DL_IDM_SAVE    191
#define WML_DL_IDM_CC      192
#define WML_DL_IDM_RING    193
#define WML_DL_IDM_VIEWIMG 194
#define WML_DL_IDM_STOP    195
#define WML_IDM_MMWRITE    196
#define WML_IDM_MMSTOP     197
#define WML_IDM_MMGETDATA  198
#define WML_IDM_DOWN_OVERWRITE  199

#define WLE_MAXDIR         64

#define WIE_INPTEXT_X      2
#define WIE_INPTEXT_Y      0
#define WIE_INPTEXT_WIDTH  176
#define WIE_INPTEXT_HEIGHT 24
#define WIE_SEDIT_X        2
#define WIE_SEDIT_Y        28
#define WIEDL_SEDIT_WIDTH  130
#define WIEDL_SEDIT_HEIGHT 24
#define WIE_STEXT_X        132
#define WIE_STEXT_Y        28
#define WIE_STEXT_WIDTH    176
#define WIE_STEXT_HEIGHT   24
#define WIE_VIEW1_X        10
#define WIE_VIEW1_Y        110
#define WIE_VIEW1_WIDTH    30
#define WIE_VIEW1_HEIGHT   30
#define WIE_VIEW2_X        50
#define WIE_VIEW2_Y        110
#define WIE_VIEW2_WIDTH    30
#define WIE_VIEW2_HEIGHT   30

#define	IDS_BMPAUDIOPLAY	"/rom/wml/wieplay.gif"  
#define	IDS_BMPAUDIOSTOP	"/rom/wml/wiestop.gif"  
#define	IDS_BMPIMGVIEW		"/rom/wml/wieview.gif"  
#define	IDS_BMPAUDIOPLAYD	"/rom/wml/wieplayd.gif"
#define	IDS_BMPAUDIOSTOPD	"/rom/wml/wiestopd.gif"
#define	IDS_BMPIMGVIEWD		"/rom/wml/wieviewd.gif"
#define	IDS_BMPAUDIOPLAYF	"/rom/wml/wieplayf.gif"
#define	IDS_BMPAUDIOSTOPF	"/rom/wml/wiestopf.gif"
#define	IDS_BMPIMGVIEWF		"/rom/wml/wieviewf.gif"

#define IDS_TEMPFILENAME    "/mnt/flash/wapbrowser/wtmptmp.tmp"
#define WIE_FLASH_MYDATA    "/mnt/flash/wapbrowser/mydata/"  
#define WIE_FLASH_IMGPATH   "/mnt/flash/pictures/"  
#define WIE_FLASH_AUDIOPATH "/mnt/flash/audio/" 

static char szFDir[WLE_MAXDIR];
static char szFName[WIESAVEAS_MAXFNAME_LEN];    //文件保存名字(包含后缀)
static char szFNameTail[WIESAVEAS_MAXFTAIL_LEN] = "";
static int nDatabuflen;
static const char* PDATABUF;
static unsigned char cContType;
static char szInputFileName[WIESAVEAS_EDITLIMIT + WIESAVEAS_MAXFTAIL_LEN] = "";
//static HBITMAP hbPlay, hbStop, hbView;
//static HBITMAP hbPlayd, hbStopd, hbViewd;
//static HBITMAP hbPlayf, hbStopf, hbViewf;
//static HWND hbtnplay, hbtnstop, hbtnview;
static HWND hwndDLoad;
static HWND hEdFileName;
//static SAVE_FILE save_file;
static SetupHS_PlayMusic rMusicinfo;
static BOOL IsPlaying = FALSE;

extern HFONT hViewFont;

static void SetPlayState(BOOL ibplay);             //设置播放模式
static void OnRingPreview();                       //预览铃声
static void OnImagePreview();                      //预览图片
static void OnRingStopCallBack(BOOL bRingIsEnd, DWORD X);
static int WLE_GetSaveFileName(unsigned char ContType, const char* szDir,
                               const char* szDefName);
static int WLE_GetSaveDir(char* szDir, int len, const char WLEContType);
static int WLE_SaveFile(const char* Databuf, int nbuflen, const char* szDir,
                        const char* szFullName);
static void GetFileNameFromUrl(int ContType, const char *szUrl, char *szName, char *szExt);
static int GetCIDinArraybyType(int ContType);
static BOOL CheckFileName(char *szName, int nlen);
static LRESULT CALLBACK WMLDLWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

extern BOOL WML_DeleteSpace( char* s );
extern HINSTANCE APP_PreviewDirect(HWND hwnd, LPCTSTR lpOperation, LPCTSTR lpFile, 
								   LPCTSTR lpParameters, LPCTSTR lpDirectory, INT nShowCmd);
extern int FileCanDownloadViaWap(char *pszeExtFilename, int nFileSize);
/*****************************************************************
* 函数介绍：将DataBuf中的数据保存到文件中去
* 传入参数：Databuf： 要保存为文件的数据
nbuflen： 数据长度
ContType：数据要保存的文件类型，此参数可以用来知道文件保存的位置（目录）
szDefName：要保存的缺省文件名称
* 传出参数：无
* 返回    ：1：成功
0：失败
*****************************************************************/
int WLE_DownLoad(const char* DATABUF, long nbuflen, unsigned char ContType, const char *szMIMEName,
                 const char* SZDEFNAME,unsigned char xdrm_type)
{
    char szDir[WLE_MAXDIR];
    char szName[WIESAVEAS_MAXFNAME_LEN];

	printf("\r\n@@@@@@@@@@@@@@@@@@ ContType = %d ######################\r\n", ContType);
    switch(ContType)
    {
		//benefon does not support wav
//    case MMT_AUDIOMIDI:
	case MMT_AUDIOWAV:
    case MMT_AUDIOAMR:
    case MMT_IMGWBMP:
	case MMT_IMGPNG:
    case MMT_IMGGIF:
    case MMT_IMGJPG:
    case MMT_IMGBMP:
        /* 分离带有“http://”等信息的szDefName */
        GetFileNameFromUrl(ContType, SZDEFNAME, szName, szFNameTail);
        
        /*得到要保存的文件路径*/
        WLE_GetSaveDir(szDir, WLE_MAXDIR, ContType);    
        
        PDATABUF    = DATABUF;
        nDatabuflen = nbuflen;
        
        if ( 1 != WLE_SaveFile(DATABUF, nbuflen, NULL, IDS_TEMPFILENAME))
            return 0;
        
        /*得到要保存的文件名称,并保存文件*/
        WLE_GetSaveFileName(ContType, (const char*)szDir, (const char*)szName);
        
        /*保存文件*/
        //if ( 1 == WLE_SaveFile(Databuf, nbuflen, szDir, szFName) )
        //    return 1;
        
        return 1;
        
    case MMT_AUDIOMMF:
    case MMT_UNKNOWN:
    default:
        PLXTipsWin(NULL, NULL, NULL, WML_WRONGTYPE, NULL,
	           Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);		
        return 0;
    }
    printf("\r\n@@@@@@@@@@@@@@@@@@ Download proc over ####################\r\n");
    //end   
}

/*****************************************************************
* 函数介绍：得到用户保存下载文件的路径
* 传入参数：szDir：   用户保存路径Buffer的地址
len：     保存路径buffer的长度
ContType：要保存的文件类型（指导保存路径）             
* 传出参数：szDir：   用户保存路径
* 返回    ：1：成功
0：失败
*****************************************************************/
static int WLE_GetSaveDir(char* SZDIR, int len, const char CONTTYPE)
{
    if (CONTTYPE == MMT_AUDIOMMF  ||
        CONTTYPE == MMT_AUDIOMIDI ||
        CONTTYPE == MMT_AUDIOAMR  ||
		CONTTYPE == MMT_AUDIOWAV)
    {
        strcpy(SZDIR, WIE_FLASH_AUDIOPATH);
    }
    else if (CONTTYPE == MMT_IMGWBMP ||
        CONTTYPE == MMT_IMGGIF ||
        CONTTYPE == MMT_IMGJPG ||
        CONTTYPE == MMT_IMGPNG ||
        CONTTYPE == MMT_IMGBMP )
    {
        strcpy(SZDIR, WIE_FLASH_IMGPATH);
    }
    else
    {
        strcpy(SZDIR, "FLASH2:");
    }    
    return 1;
}
/*****************************************************************
* 函数介绍：将DataBuf中的数据保存到文件中去
* 传入参数：Databuf： 要保存为文件的数据
nbuflen： 数据长度
szDir  ： 保存路径
szName：要保存的文件名称
* 传出参数：无
* 返回    ：1：成功
0：失败
*****************************************************************/
static int WLE_SaveFile(const char* DATABUF, int nbuflen, const char* SZDIR, const char* SZNAME)
{
    FILE*  handle;
    int    len;
    char   szCurPath[_MAX_PATH];
    
    getcwd(szCurPath, _MAX_PATH);
    
    if (SZDIR != NULL)
		chdir(SZDIR);
   
	remove(SZNAME);
    if ((handle = fopen(SZNAME, "wb+")) == NULL)
    {
        PLXTipsWin(NULL, NULL, NULL, WML_DL_TEMPFILE_ERR, NULL,
            Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
        return -1;
    }
    
    len = fwrite((char *)DATABUF, 1, nbuflen, handle);
    fclose(handle);
	printf("\r\n\r\n__________handle close = %d____________________\r\n\r\n",handle);
    
    if (len != nbuflen)
    {        
        PLXTipsWin(NULL, NULL, NULL, WML_DL_TEMPFILE_ERR, NULL,
            Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
        remove(SZNAME);
        chdir(szCurPath);
        return 0;
    }
    
    chdir(szCurPath);
    return 1;    
}

static int GetCIDinArraybyType(int ContType)
{
    int i;
    
    for (i = 0;i < DEFCONTENTNUM; i++)
    {
        if (ContType == DLDefContTable[i].nContType )
        {
            return i;
        }
    }
    
    return -1;
}

static void  GetFileNameFromUrl(int ContType, const char *SZURL,char *szName,char *szExt)
{
#define    WLE_INDEXNAME  "index"
    int nUrlLen, ndot, nbegin;
    int ncpylen;
    int cid;    
    
    cid = GetCIDinArraybyType(ContType);
    ndot = strlen(SZURL);
    nUrlLen = strlen(SZURL);
    
    if (cid != -1) 
        strcpy(szExt, DLDefContTable[cid].szConStr);
    
    //处理首页省略index.xml的情况
    if (SZURL[nUrlLen - 1] == '/')
    {
        strcpy(szName, WLE_INDEXNAME);
        return;
    }    
    
    if (nUrlLen == 0) 
    {
        strcpy(szName, ""); 
        return ;
    }
    
    //扩展名        
    while (ndot >= 0  && (*(SZURL+ndot) != '.'))        
        ndot--;
    
    if (ndot == -1)     
        ndot= strlen(SZURL);
    nbegin = ndot;
    
    while (nbegin >= 0  && (*(SZURL+nbegin) != '/'))        
        nbegin--;
    
    ncpylen = (ndot - nbegin -1) < WIESAVEAS_MAXFNAME_LEN ? 
        (ndot - nbegin - 1) : WIESAVEAS_MAXFNAME_LEN;
    strncpy(szName, SZURL +nbegin + 1, ncpylen);
    *(szName+ncpylen) = '\0';    
}

static BOOL CheckFileName(char *szName, int nlen)
{
    unsigned char c;
    int i;
    
    for (i = 0; i < nlen; i++)
    {
        c = *(szName + i);
//        if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
//            (c >= '0' && c <= '9') || (c == ' ') || (c == '$') ||
//            (c == '%') || (c == '_') ||
//            (c >= 0xa0 && c <= 0xfe)))
		if  (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') || (c == 0x11)))
            return FALSE;
    }
    return TRUE;
}

static BOOL WmlDL_OnCreate(HWND hWnd)
{    
	//DWORD dwStyle = WS_CHILD|ES_UNDERLINE|WS_VISIBLE|ES_AUTOHSCROLL|WS_TABSTOP;	
	DWORD dwStyle = WS_CHILD|WS_TABSTOP|WS_VISIBLE|ES_AUTOHSCROLL|ES_TITLE;
	IMEEDIT ie;
	//HDC hdc;
	//COLORREF Color;
	//BOOL bTran;
	char *szMulti = NULL;
	int nLenMulti = 0;
	RECT rtClient;
	
	GetClientRect(hWnd, &rtClient);
	memset(&ie, 0, sizeof(IMEEDIT));	
	ie.hwndNotify   = (HWND)hWnd;
	ie.dwAscTextMax = 0;
	ie.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
	ie.dwUniTextMax = 0;
	ie.pszCharSet   = NULL;
	ie.pszImeName   = NULL;//"字母";
	ie.pszTitle     = NULL;
	ie.uMsgSetText  = 0;
	ie.wPageMax     = 0;

    hEdFileName = CreateWindow("IMEEDIT", 
        NULL, 
        dwStyle,//|ES_NOHIDESEL,
        rtClient.left + CX_FITIN, 
		rtClient.top + CY_FITIN, //WIE_SEDIT_X,//WIE_SEDIT_Y,
		rtClient.right-rtClient.left - 2 * CX_FITIN,//WIEDL_SEDIT_WIDTH,
		(rtClient.bottom-rtClient.top - CY_FITIN)/3,//WIEDL_SEDIT_HEIGHT,         
        hWnd, 
        NULL,
        NULL, 
        (PVOID)&ie
        );
    if (hEdFileName == NULL) 
        return FALSE;

    SendMessage(hEdFileName, EM_LIMITTEXT, WIESAVEAS_EDITLIMIT, NULL);
	SendMessage(hEdFileName, EM_SETTITLE, 0, (LPARAM)WML_DLFNAME);
			
	nLenMulti = UTF8ToMultiByte(CP_ACP, 0, szFName, -1, NULL, 0, NULL, NULL);
	szMulti = (char *)malloc(sizeof(char) * nLenMulti + 1);
	memset(szMulti, 0, nLenMulti + 1);
	nLenMulti = UTF8ToMultiByte(CP_ACP, 0, szFName, -1, szMulti, nLenMulti, NULL, NULL);
	szMulti[nLenMulti] = '\0';

    SetWindowText(hEdFileName, szMulti);
        
#if 0
	hdc = GetDC(hWnd);
	hbPlay  = CreateBitmapFromImageFile(hdc, IDS_BMPAUDIOPLAY,  &Color, &bTran);
	hbStop  = CreateBitmapFromImageFile(hdc, IDS_BMPAUDIOSTOP,  &Color, &bTran);
	hbView  = CreateBitmapFromImageFile(hdc, IDS_BMPIMGVIEW,    &Color, &bTran);
	hbPlayd = CreateBitmapFromImageFile(hdc, IDS_BMPAUDIOPLAYD, &Color, &bTran);
	hbStopd = CreateBitmapFromImageFile(hdc, IDS_BMPAUDIOSTOPD, &Color, &bTran);
	hbViewd = CreateBitmapFromImageFile(hdc, IDS_BMPIMGVIEWD,   &Color, &bTran);
	hbPlayf = CreateBitmapFromImageFile(hdc, IDS_BMPAUDIOPLAYF, &Color, &bTran);
	hbStopf = CreateBitmapFromImageFile(hdc, IDS_BMPAUDIOSTOPF, &Color, &bTran);
	hbViewf = CreateBitmapFromImageFile(hdc, IDS_BMPIMGVIEWF,   &Color, &bTran);
	ReleaseDC(hWnd, hdc);
	
    memset(&save_file, 0, sizeof(SAVE_FILE));    
    if (cContType == MMT_AUDIOMMF || cContType == MMT_AUDIOMIDI ||
        cContType == MMT_AUDIOAMR || cContType == MMT_AUDIOWAV)
    {
        save_file.DataType = enRing;        
        hbtnplay = CreateWindow("BUTTON", "",
            WS_BITMAP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
            WIE_VIEW1_X,
            WIE_VIEW1_Y,
            WIE_VIEW1_WIDTH,
            WIE_VIEW1_HEIGHT,
            hWnd, (HMENU)WML_DL_IDM_RING, NULL, NULL);
        
        hbtnstop = CreateWindow("BUTTON", "",
            WS_BITMAP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
            WIE_VIEW2_X,
            WIE_VIEW2_Y,
            WIE_VIEW2_WIDTH,
            WIE_VIEW2_HEIGHT,
            hWnd, (HMENU)WML_DL_IDM_STOP, NULL, NULL);
        
        SendMessage(hbtnplay, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbPlay);
        SendMessage(hbtnplay, BM_SETIMAGE, (WPARAM)IMAGE_DISABLE|IMAGE_BITMAP, (LPARAM)hbPlayd);
        SendMessage(hbtnplay, BM_SETIMAGE, (WPARAM)IMAGE_FOCUS|IMAGE_BITMAP, (LPARAM)hbPlayf);
        SendMessage(hbtnplay, BM_SETIMAGE, (WPARAM)IMAGE_PUSHDOWN|IMAGE_BITMAP, (LPARAM)hbPlayd);
        
        SendMessage(hbtnstop, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbStop);
        SendMessage(hbtnstop, BM_SETIMAGE, (WPARAM)IMAGE_PUSHDOWN|IMAGE_BITMAP, (LPARAM)hbStopd);
        SendMessage(hbtnstop, BM_SETIMAGE, (WPARAM)IMAGE_FOCUS|IMAGE_BITMAP, (LPARAM)hbStopf);
        
        SetPlayState(FALSE);                
    }
    else if (cContType == MMT_IMGWBMP || cContType == MMT_IMGGIF || cContType == MMT_IMGJPG ||
             cContType == MMT_IMGPNG || cContType == MMT_IMGBMP)
    {
        save_file.DataType = enPicture;
        hbtnview = CreateWindow("BUTTON", "", WS_BITMAP|WS_CHILD|WS_VISIBLE|WS_TABSTOP,
            WIE_VIEW1_X,
            WIE_VIEW1_Y,
            WIE_VIEW1_WIDTH,
            WIE_VIEW1_HEIGHT,
            hWnd, (HMENU)WML_DL_IDM_VIEWIMG, NULL, NULL);    
        
        SendMessage(hbtnview, BM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hbView);
        SendMessage(hbtnview, BM_SETIMAGE, (WPARAM)IMAGE_PUSHDOWN|IMAGE_BITMAP, (LPARAM)hbViewd);
        SendMessage(hbtnview, BM_SETIMAGE, (WPARAM)IMAGE_FOCUS|IMAGE_BITMAP, (LPARAM)hbViewf);
    }
#endif

	free(szMulti);
    return TRUE;
}

static BOOL SaveDataToFile(char *szFileName)
{
	int tmp;
	BOOL ret;
	char szCurPath[_MAX_PATH];
	char szdstFileName[_MAX_PATH];
	int  countin = 0, countout = 0;	
	DWORD ipointer = 0;
	char promtpch[40];
//	char *szMulti;
//	int nLenMulti;
				
	int retval = mkdir(WIE_FLASH_IMGPATH, 0);
	
	getcwd(szCurPath, _MAX_PATH);
	
	if ((cContType == MMT_IMGGIF) || (cContType == MMT_IMGBMP)
        || (cContType == MMT_IMGJPG) )
	{
		chdir(WIE_FLASH_IMGPATH);
		strcpy(szdstFileName, WIE_FLASH_IMGPATH);
	}
	else if ((cContType == MMT_AUDIOMMF) || (cContType == MMT_AUDIOMIDI)
		|| (cContType == MMT_AUDIOAMR) || (cContType == MMT_AUDIOWAV))
	{
		chdir(WIE_FLASH_AUDIOPATH);
		strcpy(szdstFileName, WIE_FLASH_AUDIOPATH);
	}
	else
	{
		chdir(FLASHPATH);
		strcpy(szdstFileName, FLASHPATH);
	}
	
//	nLenMulti = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szFileName, -1, NULL, 0);
//	szMulti = (char *)malloc(nLenMulti + 1);
//	memset(szMulti, 0, nLenMulti + 1);
//	nLenMulti = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szFileName, -1, szMulti, nLenMulti);
//	szMulti[nLenMulti] = '\0';
				
	strcat(szdstFileName, szFileName);

	tmp = rename(IDS_TEMPFILENAME, szdstFileName);
	if (tmp != 0)
	{
		int nerr;
		
		nerr = GetLastError();
		strcpy(promtpch, WML_SAVEFAILED);
		strcat(promtpch, "\n");
		strcat(promtpch, WML_DL_WRITEFILE_ERR);
		PLXTipsWin(NULL, NULL, NULL, promtpch, NULL,
			Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
		remove(szdstFileName);
	}

	chdir(szCurPath);
	if (tmp == 0)
		ret = TRUE;
	else
		ret = FALSE;
//	free(szMulti);
	return ret;
}

static BOOL CheckDownFileExist(char *szName)
{
	char szCurPath[_MAX_PATH];
	BOOL ibexist;
	DIR *dp;
	struct dirent *dirp;
	
	getcwd(szCurPath, _MAX_PATH);
	
	if ((cContType == MMT_IMGGIF) || (cContType == MMT_IMGBMP)
        || (cContType == MMT_IMGJPG) || (cContType == MMT_IMGPNG))	
	{
		chdir(WIE_FLASH_IMGPATH);
		if((dp = opendir(WIE_FLASH_IMGPATH)) == NULL)
			return FALSE;
	}
	else if ((cContType == MMT_AUDIOMMF) || (cContType == MMT_AUDIOMIDI) 
		|| (cContType == MMT_AUDIOAMR) || (cContType == MMT_AUDIOWAV))
	{
		chdir(WIE_FLASH_AUDIOPATH);
		if ((dp = opendir(WIE_FLASH_AUDIOPATH)) == NULL)
			return FALSE;
	}
	else 
	{
		chdir(FLASHPATH);
		if ((dp = opendir(FLASHPATH)) == NULL)
			return FALSE;
	}
	
	if ((dirp = readdir(dp)) != NULL)
	{
		do
		{
			if (strcmp(szName, dirp->d_name) == 0)
			{
				ibexist = TRUE;
				break;
			}
			else
				ibexist = FALSE;
		} while ((dirp = readdir(dp)) != NULL);
	}
	
	closedir(dp);
	chdir(szCurPath);
	
	return ibexist;
}

void Browser_TrimString(char * pstr)
{
	int len;
	char *pTmp;
	
	len = strlen(pstr);
	//前后没有空格
	if ((pstr[0] != 0x20) && (pstr[len -1] != 0x20))
		return ;
	
	pTmp = pstr;	
	while (*pTmp == 0x20)
	{
		pTmp++;
	}
	
	//去掉前面的空格
	strcpy(pstr, pTmp);
	len = strlen(pstr);
	
	while (pstr[len -1] == 0x20)
	{
		pstr[len - 1] = 0x0;
		len--;
	}
}

static BOOL OnCommandSave(HWND hWnd, HWND hEdFileName)
{
	unsigned int nInputLen = 0;
	int nSEFJudgeRet = 1;

	GetWindowText(hEdFileName, szInputFileName, WIESAVEAS_EDITLIMIT + 1);	
	if (strlen(szInputFileName) == 0)
	{
		PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFNAME,	NULL,
			Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
		return FALSE;
	}

	if (!WML_DeleteSpace(szInputFileName))
	{
		PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFNAME, NULL,
			Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
		return FALSE;
	}
	
	//#ifndef  _EMULATE_
	nSEFJudgeRet = FileCanDownloadViaWap(szFNameTail+1, nDatabuflen);
	//#endif
	if (nSEFJudgeRet == -2)
	{
		PLXTipsWin(NULL, NULL, NULL, WML_WRONGTYPE, NULL,
			Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
		return FALSE;
	}
	else if (nSEFJudgeRet == -1)
	{
		PLXTipsWin(NULL, NULL, NULL, WML_DL_FLASHFULLFILE_ERR, NULL,
			Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
		return FALSE;
	}
	else if (nSEFJudgeRet == 0)
	{
		PLXTipsWin(NULL, NULL, NULL, WML_DL_TYPEFULLFILE_ERR, NULL,
			Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
		return FALSE;
	}
	
	nInputLen = strlen(szInputFileName);
	if (CheckFileName(szInputFileName, nInputLen) == FALSE)
	{
		PLXTipsWin(NULL, NULL, NULL, WML_DL_BADFNAME_ERR, NULL,
			Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
		return FALSE;
	}
	
	strcat(szInputFileName, szFNameTail);
	if (CheckDownFileExist(szInputFileName) == TRUE)
	{
		PLXConfirmWinEx(NULL, hWnd, WML_NAMEUSEDOVER, Notify_Request, (char *)NULL, 
			IDS_YES, IDS_NO, WML_IDM_DOWN_OVERWRITE);
		return FALSE;
	}
	else
		return SaveDataToFile(szInputFileName);
}

static void WmlDL_OnCommand(HWND hWnd,WPARAM wParam,LPARAM lParam)
{        
    switch (LOWORD(wParam))
    {
    case WML_DL_IDM_SAVE:
        {
			if(OnCommandSave(hWnd, hEdFileName) == TRUE)
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
        }
        break;
        
    case WML_DL_IDM_CC:
		Wml_ONCancelExit(hWnd);
        break;        
    
#if 0
    case WML_DL_IDM_RING:
        if (HIWORD(wParam) == BN_CLICKED)
            OnRingPreview();
        break;
		
    case WML_DL_IDM_STOP:
        if (HIWORD(wParam) == BN_CLICKED)
        {
			IsPlaying = FALSE;
#ifdef _EMULATE_
			Setup_EndPlayMusic(RING_OTHER);
#else
			Setup_EndPlayMusic(RING_WAP);
#endif
            SetPlayState(FALSE);
        }
        break;

    case WML_DL_IDM_VIEWIMG:
        if (HIWORD(wParam) == BN_CLICKED)
            OnImagePreview();        
        break;
#endif
		
    default:
        break;
    }
}
/*****************************************************************
* 函数介绍：弹出一个输入窗口，并得到用户输入后的文件名称
* 传入参数：szName：  用户名称保存地址
nbuflen： 用户名称长度
ContType：要保存的文件类型（决定文件名称后缀）
szDir:    保存文件的路径
szDefName：要保存的缺省文件名称
* 传出参数：szFName：  用户名称（为静态的全局变量）
* 返回    ：1：成功
0：失败
*****************************************************************/
static int WLE_GetSaveFileName(unsigned char ContType, 
                               const char* SZDIR,
                               const char* SZDEFNAME)
{
    WNDCLASS wc;
    char szTitle[12] = "";
    
    //保存文件路径和缺省名称
    strcpy(szFDir, SZDIR);
    strcpy(szFName, SZDEFNAME);    
    cContType = ContType;
    
    //根据文件类型得到文件后缀    
    //创建输入文件名称的窗口
    wc.style         = 0;
    wc.lpfnWndProc   = WMLDLWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);    
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLDLClass";
    
    if (!RegisterClass(&wc))
        return -1;
    
    hwndDLoad = CreateWindow(
        "WMLDLClass", 
        "",
        WS_VISIBLE|WS_CHILD,
        0,0,176,150, 
        GetWAPFrameWindow(),
        NULL,
        NULL, 
        NULL
        );
    if (hwndDLoad == NULL) 
        return 0;
    
	SendMessage(hEdFileName, EM_SETSEL, -1, -1);
    ShowWindow(hwndDLoad, SW_SHOW);            
    UpdateWindow(hwndDLoad);
    
    return 1;
}

#if 0
void test_download()
{
	WLE_GetSaveFileName(0, "FLASH2:", "1.wml");
}
#endif

static LRESULT CALLBACK WMLDLWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    HDC hdc;
    LRESULT lResult = TRUE;
    PAINTSTRUCT ps;    
    static HWND hfocus;    
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		SetCanPlayMusic(FALSE, WAVEOUT_PRIO_BELOWNORMAL);
        lResult = (LRESULT)WmlDL_OnCreate(hWnd);
		hfocus = hEdFileName;
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_SAVEFILE);		
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_SAVES);
		SetFocus(hfocus);
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_SAVEFILE);		
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_SAVES);
		SetFocus(hfocus);
		break;
    
	case WM_SETRBTNTEXT:
		if (strcmp((char *)lParam, (LPCSTR)WML_MENU_BACK) == 0)
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
        else
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, lParam);
        break;
        
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
            hfocus = GetFocus();
        else
            SetFocus(hfocus);
        break; 

	case WML_IDM_DOWN_OVERWRITE:
		if(lParam == 1)
		{
			remove(szInputFileName);
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		}
		break;
        
    case WM_COMMAND:
        WmlDL_OnCommand(hWnd, (WPARAM)wParam, (LPARAM)lParam);
        break;  
#if 0		
	case WML_IDM_MMWRITE:
#ifndef _EMULATE_
		//Setup_WriteMusicData((LPWAVEHDR)lParam);
#endif
		break;
		
	case WML_IDM_MMSTOP:
		SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(WML_DL_IDM_STOP, BN_CLICKED), NULL);
		break;
		
	case WML_IDM_MMGETDATA:
#ifndef _EMULATE_
		//Setup_GetMusicData((LPWAVEHDR) lParam);
#endif
		break;
#endif

    case WM_PAINT:    
        {
            int  OldStyle;
            
            hdc = BeginPaint(hWnd, &ps);
            OldStyle = SetBkMode(hdc, TRANSPARENT);   
			SelectObject(hdc, hViewFont);
//            TextOut(hdc, WIE_INPTEXT_X, WIE_INPTEXT_Y, WML_DLFNAME, -1); 
//            TextOut(hdc, WIE_STEXT_X, WIE_STEXT_Y, szFNameTail, -1);            
            SetBkMode(hdc, OldStyle);
            EndPaint(hWnd, &ps);
        }
        break;
        
    case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		
		if (IsPlaying)
		{
			IsPlaying = FALSE;
#ifdef _EMULATE_
			Setup_EndPlayMusic(RING_OTHER);
#else
			Setup_EndPlayMusic(RING_WAP);
#endif
		}
		SetCanPlayMusic(TRUE, 0);
        DestroyWindow(hWnd);
        UnregisterClass("WMLDLClass", NULL);            
        break;
        
    case WM_DESTROY:
#if 0
		if (hbView)
			DeleteObject(hbView);
		if (hbStop)
			DeleteObject(hbStop);
		if (hbPlay)
			DeleteObject(hbPlay);
		if (hbViewf)
			DeleteObject(hbViewf);
		if (hbStopf)
			DeleteObject(hbStopf);
		if (hbPlayf)
			DeleteObject(hbPlayf);
		if (hbViewd)
			DeleteObject(hbViewd);
		if (hbStopd)
			DeleteObject(hbStopd);
		if (hbPlayd)
			DeleteObject(hbPlayd);
		hbView  = NULL;
		hbStop  = NULL;
		hbPlay  = NULL;
		hbViewf = NULL;
		hbStopf = NULL;
		hbPlayf = NULL;
		hbViewd = NULL;
		hbStopd	= NULL;		
		hbPlayd	= NULL;
#endif
        remove(IDS_TEMPFILENAME);
        break;

	case WM_KEYDOWN:
		switch (wParam)
		{				
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		case VK_RETURN:
			PostMessage(hWnd, WM_COMMAND, WML_DL_IDM_SAVE, NULL);
			break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);			
		}
		break;
        
    default:            
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    
    return lResult;        
}

#if 0
static void SetPlayState(BOOL ibplay)
{
    if (ibplay)
    {
        if (GetFocus() == hbtnplay) 
            SetFocus(hbtnstop);

        EnableWindow(hbtnplay, FALSE);
        EnableWindow(hbtnstop, TRUE);
    }
	else
	{
        if (GetFocus() == hbtnstop) 
            SetFocus(hbtnplay);
		
        EnableWindow(hbtnplay, TRUE);
        EnableWindow(hbtnstop, FALSE);
    }
}

static void OnRingPreview()
{
	int nAudiotype = 0;
	
	if (cContType == MMT_AUDIOMMF)
		nAudiotype = MMF_TYPE;
	else if(cContType == MMT_AUDIOMIDI)
		nAudiotype = MIDI_TYPE;
	else if(cContType == MMT_AUDIOAMR)
		nAudiotype = AMR_TYPE;	
    else if(cContType == MMT_AUDIOWAV)
		nAudiotype = WAVE_TYPE;
	
	//#ifndef  _EMULATE_
	rMusicinfo.hWnd          = hwndDLoad;		
	rMusicinfo.iWM_MMWRITE   = WML_IDM_MMWRITE;
	rMusicinfo.iWM_MMSTOP    = WML_IDM_MMSTOP;	
	rMusicinfo.iWM_MMGETDATA = WML_IDM_MMGETDATA;
	rMusicinfo.iVolume       = -1;
	rMusicinfo.iType         = nAudiotype;
#ifdef _EMULATE_
	rMusicinfo.uiringselect  = RING_OTHER;
#else
	rMusicinfo.uiringselect  = RING_WAP;
#endif	
	rMusicinfo.pFileName     = IDS_TEMPFILENAME;
	rMusicinfo.pBuffer       = NULL;
	rMusicinfo.ulBufferSize  = nDatabuflen;
	
	IsPlaying = TRUE;
	SETUP_PlayMusic(&rMusicinfo);
	SetPlayState(TRUE);
}

static void OnImagePreview()
{
    char sfileExt[10] = {'\0'};
    
    if (cContType == MMT_IMGGIF)
    {
        //npictype = PIC_IS_GIF;
        strcpy(sfileExt,"GIF");
    }
    else if (cContType == MMT_IMGBMP)
    {
        //npictype = PIC_IS_BMP;
        strcpy(sfileExt,"BMP");
    }
    else if (cContType == MMT_IMGJPG)
    {
        //npictype = 2;//PIC_IS_JPG;
        strcpy(sfileExt,"JPG");
    }
    else if (cContType == MMT_IMGPNG)
    {
        //npictype = 2;//PIC_IS_JPG;
        strcpy(sfileExt,"PNG");
    }
    
    APP_PreviewDirect(hwndDLoad, NULL, IDS_TEMPFILENAME, sfileExt, 0x00, TRUE);    
}

static void OnRingStopCallBack(BOOL bRingIsEnd, DWORD X)
{
    SetPlayState(FALSE);    
}
#endif


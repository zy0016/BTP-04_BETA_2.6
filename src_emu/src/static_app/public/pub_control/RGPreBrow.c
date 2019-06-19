/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Public
 *
 * Purpose  : 铃声预览
 *            
\**************************************************************************/

#include	"PreBrow.h"
#include	"PreBrowHead.h"
#include    "setting.h"
#include	"mullang.h"
#include	"pubapp.h"

/************************************************************************/
/* 控件位置宏                                                           */
/************************************************************************/



/************************************************************************/
/* 消息参数宏                                                           */
/************************************************************************/
#define		IDS_CANCEL	ML("Cancel")
#define		IDS_PLAY	ML("Play")
#define		IDS_STOP	ML("Stop")
#define		IDS_CAPTION	ML("Select sound")

#define		IDB_CANCEL	(WM_USER+100)


#define		PRERG_RINGFILEDIR	"/rom/audio/"
#define		PRERG_RINGFILEDIR2	"/mnt/flash/audio/"
#define     PRERG_RINGFILEDIR3  "/mnt/fat16/mmc/audio/" 



typedef struct tagRGPREBROW {
	HWND	hWndList;
	HWND	hFrame;
	HWND	hRecMsg;
	UINT	uSmPreReturn;
	PSTR	szCaption;
	char	cMediaFile[200];
	BOOL	bPlaying;
	LISTBUF	ListBuffer;
}RGPREBROWDATA,* PRGPREBROWDATA;
/********************/
/*			*/
/********************/
static	LRESULT	CALLBACK	PRERG_RGPreBrowProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static	BOOL	RegisteRGPREBROW();
int	AddRingFileToList(HWND hList, PLISTBUF pListbuf);


static	BOOL	RegisteRGPREBROW()
{
	WNDCLASS wc;	
	
	
	wc.style            = NULL;
	wc.lpfnWndProc      = PRERG_RGPreBrowProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(RGPREBROWDATA);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = "RGPreBrowWndClass";
	
	if (!RegisterClass(&wc))
		return FALSE;
	
	return TRUE;
	
}
/*********************************************************************
* Function	RGPreBrow   
* Purpose     
* Params	
* Return	
* Remarks	
**********************************************************************/
BOOL	APP_PreviewRing(HWND hFrame, HWND hWnd, UINT returnmsg, PSTR psCaption)
{
	RECT	rf;
	RGPREBROWDATA RgData;
	HWND	hWndMain	=	NULL;

	RegisteRGPREBROW();

	memset(&RgData, 0, sizeof(RGPREBROWDATA));
	RgData.hFrame = hFrame;
	RgData.uSmPreReturn = returnmsg;
	RgData.szCaption = psCaption;
	RgData.hRecMsg = hWnd;

	GetClientRect(hFrame, &rf);
	
	hWndMain = CreateWindow(
		"RGPreBrowWndClass", 
		NULL, 
		WS_VISIBLE|WS_CHILD,		
		rf.left,
		rf.top,
		rf.right - rf.left,
		rf.bottom - rf.top,
		hFrame, 
		NULL,
		NULL, 
		(PVOID)&RgData);
	
	if (!IsWindow(hWndMain))
	{
		return FALSE;
	}
	if (psCaption) {
		SetWindowText(hFrame, psCaption);
	}
	else
	{
		SetWindowText(hFrame, IDS_CAPTION);
	}
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PLAY);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
	UpdateWindow(hFrame);
	ShowWindow(hFrame,SW_SHOW);
	SetFocus(hWndMain);
			
			
	return	TRUE;
}

/*********************************************************************
* Function	PRERG_MainWndProc
* Purpose   
* ParaSM	
* Return	
* Remarks	
**********************************************************************/
static	LRESULT	CALLBACK	PRERG_RGPreBrowProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	PCREATESTRUCT	pCreatestu;
	LRESULT	lResult;
	int		num;
	PRGPREBROWDATA	pRgData;
	RECT	rc;
	char	szTemp[PREBROW_MAXFILENAMELEN];
	char	cFileName[PREBROW_MAXFULLNAMELEN];
	int		nFileType;

	pRgData = GetUserData(hWnd);


	lResult	=	TRUE;	

	switch(wMsgCmd)
	{
	case WM_CREATE:	
		pCreatestu = (PCREATESTRUCT)lParam;
		memcpy(pRgData, pCreatestu->lpCreateParams, sizeof(RGPREBROWDATA));
		pRgData->bPlaying = FALSE;
		PREBROW_InitListBuf (&pRgData->ListBuffer);
		GetClientRect(hWnd, &rc);
		pRgData->hWndList	=	CreateWindow("LISTBOX","",
			WS_CHILD |LBS_HASSTRINGS|WS_VSCROLL|LBS_BITMAP|WS_VISIBLE,
			rc.left,
			rc.top,
			rc.right - rc.left,
			rc.bottom - rc.top,
			hWnd, NULL , NULL, NULL);
		SendMessage(pRgData->hWndList, LB_ADDSTRING, 0, (LPARAM)ML("Default tone"));
		PREBROW_AddData(&pRgData->ListBuffer, ML("Default tone"), "");
		num = AddRingFileToList ( pRgData->hWndList, &pRgData->ListBuffer);
		SendMessage(pRgData->hWndList, LB_SETCURSEL, 0, 0);
		SendMessage(pRgData->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
		SendMessage(pRgData->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
		/*
		if (num!=0) {
			SendMessage(pRgData->hWndList, LB_SETCURSEL, 0, 0);
			ShowWindow(pRgData->hWndList, SW_SHOW);
			GetFileNameFromList(pRgData->hWndList, &pRgData->ListBuffer, NULL, szTemp, cFileName, &nFileType);
			strcpy(pRgData->cMediaFile, cFileName);
			PlaySound(cFileName, 0, 0);
		}
		*/
		break;

	case PWM_SHOWWINDOW:
		SendMessage(pRgData->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 1), NULL);
		SendMessage(pRgData->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, 0), NULL);
		if (pRgData->szCaption) 
		{			
			SetWindowText(pRgData->hFrame, pRgData->szCaption);
		}
		else
		{
			SetWindowText(pRgData->hFrame, IDS_CAPTION);
		}
		SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PLAY);
		
		SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		SetFocus(pRgData->hWndList);
		/*
		if (SendMessage(pRgData->hWndList, LB_GETCOUNT,0,0) <= 0) {
			ShowWindow(pRgData->hWndList, SW_HIDE);
		}
		else
		{
			ShowWindow(pRgData->hWndList, SW_SHOW);
		}
		*/
		break;

	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE) 
		{
			
			if (pRgData->bPlaying) {
				pRgData->bPlaying = FALSE;
//				stopSound(pRgData->cMediaFile,0);
			}
		}
		break;

	case WM_SETFOCUS:
		SetFocus(pRgData->hWndList);
		break;

	case WM_PAINT: 
		{
			HDC		hdc;
			hdc	=	BeginPaint( hWnd, NULL);
			GetClientRect(hWnd, &rc);
			DrawText(hdc, ML("No rings"), -1, &rc, DT_VCENTER|DT_HCENTER);
			EndPaint(hWnd, NULL);		
			
		}
		break;

	case WM_KEYDOWN:
	    switch(wParam)
		{
		case VK_F5:
			szTemp[0] = NULL;
			cFileName[0] = NULL;
			if(GetFileNameFromList(pRgData->hWndList, &pRgData->ListBuffer, NULL, szTemp, cFileName, &nFileType))
			{
				if (pRgData->bPlaying) {
					pRgData->bPlaying = FALSE;
//					stopSound(pRgData->cMediaFile,0);
				}
				strcpy(pRgData->cMediaFile, cFileName);
				SendMessage(pRgData->hRecMsg,pRgData->uSmPreReturn, MAKEWPARAM(strlen(cFileName), RTN_SOUND), (LPARAM)cFileName);
				SendMessage(pRgData->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			else
			{
				SendMessage(pRgData->hRecMsg,pRgData->uSmPreReturn, MAKEWPARAM(0, RTN_NOSELECT), (LPARAM)"");
				SendMessage(pRgData->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
			break;
			
		case VK_F10:
			if (pRgData->bPlaying) {
				pRgData->bPlaying = FALSE;
//				stopSound(pRgData->cMediaFile,0);
			}
			PostMessage(pRgData->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		case VK_RETURN:
			if (pRgData->bPlaying) 
			{
//				stopSound(pRgData->cMediaFile, 0);
				SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PLAY);
				pRgData->bPlaying = FALSE;
			}
			else
			{
				if(GetFileNameFromList(pRgData->hWndList, &pRgData->ListBuffer, NULL, szTemp, cFileName, &nFileType))
				{
					if ((nFileType == PREBROW_FILETYPE_WAV)||(nFileType == PREBROW_FILETYPE_AMR)) 
					{
						strcpy(pRgData->cMediaFile, cFileName);
//						PlaySound(pRgData->cMediaFile, NULL,1<<28);
						
							pRgData->bPlaying = TRUE;
							SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_STOP);
						
						//else
							//stopSound(pRgData->cMediaFile, 0);
					}
					else
						PLXTipsWin(pRgData->hFrame, hWnd, 0, ML("Unknown audio stream"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
				}
			}
			break;
			

		}
		break;

		
	case WM_DESTROY:
		if (pRgData->bPlaying) 
		{
			pRgData->bPlaying = FALSE;
//			stopSound(pRgData->cMediaFile,0);
		}
		PREBROW_FreeListBuf (&pRgData->ListBuffer);
		UnregisterClass ("RGPreBrowWndClass", NULL);
		break;

	case WM_CLOSE:
		DestroyWindow (hWnd);
		
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;	
}



/*********************************************************************
* Function	AddRingFileToList
* Purpose    把不同目录中的文件显示到列表中
* ParaSM	    
* Return		
* Remarks	
**********************************************************************/
int	AddRingFileToList(HWND hList, PLISTBUF pListBuf)
{
	int n;
	int	temp;

	
	temp = FindFileAndAddToList (hList, PRERG_RINGFILEDIR,  PREBROW_FILEUNIVERSE_WAV, PREBROW_FILETYPE_WAV, 0, pListBuf);

	temp = FindFileAndAddToList (hList, PRERG_RINGFILEDIR,  PREBROW_FILEUNIVERSE_WAV1, PREBROW_FILETYPE_WAV, 0, pListBuf);


	temp = FindFileAndAddToList (hList, PRERG_RINGFILEDIR,  PREBROW_FILEUNIVERSE_MID, PREBROW_FILETYPE_MID, 0, pListBuf);

	temp = FindFileAndAddToList (hList, PRERG_RINGFILEDIR,  PREBROW_FILEUNIVERSE_MID1, PREBROW_FILETYPE_MID, 0, pListBuf);

	
	temp = FindFileAndAddToList (hList, PRERG_RINGFILEDIR,  PREBROW_FILEUNIVERSE_MMF, PREBROW_FILETYPE_MMF, 0, pListBuf);


	temp = FindFileAndAddToList (hList, PRERG_RINGFILEDIR,  PREBROW_FILEUNIVERSE_AMR, PREBROW_FILETYPE_AMR, 0, pListBuf);


	temp = FindFileAndAddToList (hList, PRERG_RINGFILEDIR,  PREBROW_FILEUNIVERSE_AMR1, PREBROW_FILETYPE_AMR, 0, pListBuf);

	
	
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR2,  PREBROW_FILEUNIVERSE_WAV, PREBROW_FILETYPE_WAV, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR2,  PREBROW_FILEUNIVERSE_WAV1, PREBROW_FILETYPE_WAV, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR2,  PREBROW_FILEUNIVERSE_MID, PREBROW_FILETYPE_MID, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR2,  PREBROW_FILEUNIVERSE_MID1, PREBROW_FILETYPE_MID, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR2,  PREBROW_FILEUNIVERSE_MMF, PREBROW_FILETYPE_MMF, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR2,  PREBROW_FILEUNIVERSE_AMR, PREBROW_FILETYPE_AMR, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR2,  PREBROW_FILEUNIVERSE_AMR1, PREBROW_FILETYPE_AMR, 0, pListBuf);

	FindFileAndAddToList (hList, PRERG_RINGFILEDIR3,  PREBROW_FILEUNIVERSE_WAV, PREBROW_FILETYPE_WAV, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR3,  PREBROW_FILEUNIVERSE_WAV1, PREBROW_FILETYPE_WAV, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR3,  PREBROW_FILEUNIVERSE_MID, PREBROW_FILETYPE_MID, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR3,  PREBROW_FILEUNIVERSE_MID1, PREBROW_FILETYPE_MID, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR3,  PREBROW_FILEUNIVERSE_MMF, PREBROW_FILETYPE_MMF, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR3,  PREBROW_FILEUNIVERSE_AMR, PREBROW_FILETYPE_AMR, 0, pListBuf);
	FindFileAndAddToList (hList, PRERG_RINGFILEDIR3,  PREBROW_FILEUNIVERSE_AMR1, PREBROW_FILETYPE_AMR, 0, pListBuf);


	n = SendMessage(hList,LB_GETCOUNT,NULL,NULL);
	if (LB_ERR != n)
	{
		SendMessage(hList,LB_SETCURSEL,(WPARAM)0,NULL);
	}

	return n;
}

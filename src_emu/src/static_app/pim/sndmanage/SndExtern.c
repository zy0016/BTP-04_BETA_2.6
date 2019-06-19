/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Sound
 *
 * Purpose  : External services
 *            
\**************************************************************************/

#include	"PreBrow.h"
#include	"PreBrowHead.h"
#include    "setting.h"
#include	"mullang.h"
#include	"pubapp.h"
#include	"SndManage.h"

/************************************************************************/
/* 消息参数宏                                                           */
/************************************************************************/
#define		IDS_CANCEL	ML("Cancel")
#define		IDS_PLAY	ML("Play")
#define		IDS_STOP	ML("Stop")
#define		IDS_EXTCAP	ML("Select sound")

#define		IDB_CANCEL	(WM_USER+100)
/*
#ifndef		_SNDEXT_BACK_	// the RSK is back inside a folder
#define		_SNDEXT_BACK_
#endif
//*/
#undef		_SNDEXT_BACK_	// the RSK is always cancel

#define		ISBENESOUND(s) (strncasecmp(s,BENESNDFILEDIR,strlen(BENESNDFILEDIR))==0)

typedef struct tagSNDEXTDATA {
	HWND	hWndList;
	HWND	hFrame;
	HWND	hRecMsg;
	UINT	uSmPreReturn;
	char	szCaption[64];
//	char	cMediaFile[200];
	char	szNowPath[MAXSNDPATHLENGTH];
	BOOL	bShowPreset;	// show benefon preset sounds or not
	BOOL	bBeneSnd;		// whether the playing is a preset sound
	int		nPlayingItem;	// index of the playing sound
	int		nVolume;
	BOOL	bDefTone;
	PSTR	szDefTone;
	LISTBUF	ListBuffer;
}SNDEXTDATA,* PSNDEXTDATA;
/********************/
/*			*/
/********************/

static HBITMAP hMMCBmp = NULL;
static HBITMAP hFolderBmp = NULL;
static HBITMAP hIconStatic = NULL;
static HBITMAP hIconPlaying = NULL;
static HBITMAP hBeneStatic = NULL;
static HBITMAP hBenePlaying = NULL;

static int nBmpCount = 0;

static LRESULT	CALLBACK	SelectSndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam);
static BOOL	RegisterSndExt();
//extern BOOL SND_GetMMCStatus();
//extern BOOL	IsSound(PSTR pFileName, int *nItemData);
//extern BOOL AddSndFileToList(/*HWND hList, */const char* pFilePath, LISTBUF* ListBuffer, BOOL bShowSubDir);

static BOOL SND_SetEXNCArea(PSNDEXTDATA pData);
static	int	LoadExtViewList(PSNDEXTDATA pData);
static void SNDEX_OnDestroy(PSNDEXTDATA pData);
static int LoadExListFromBuffer(PSNDEXTDATA pData);
static int AddSoundsToExList(PSNDEXTDATA pData, int nFolderType);
static void SNDEX_SetSoundIcon(PSNDEXTDATA pData, BOOL bPlaying);
static int SND_GetFirstSound(const char *pFilePath, char *szPath);
static BOOL SNDEX_LoadDefaultTone(PSNDEXTDATA pData);

static	BOOL	RegisterSndExt()
{
	WNDCLASS wc;	
	
	if(GetClassInfo(NULL, "SelectSndWndClass", &wc))
		return TRUE;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = SelectSndProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(SNDEXTDATA);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = "SelectSndWndClass";
	
	if (!RegisterClass(&wc))
		return FALSE;
	
	return TRUE;
	
}
/*********************************************************************
* Function	PreviewSoundEx   
* Purpose	sounds preview and select
* Params	hFrame			handle of the caller's frame
*			hWnd			handle of the window to receive the return message
*			returnmsg		ID of the return message. 
*							When user decide to select a sound, this message 
*							will be sent to the window pointed by hWnd.
*				wParam		Low word is the length of the path selected, 
*							high word is RTN_SOUND(=1)
*				lParam		Pointer to the full path of the selected sound.
*			psCaption		Caption of the preview window. It can be NULL.
*			bShowPresetSnd	show the benefon preset sounds or not
* Return	return NULL only if new window can't be created. 
*			otherwise, return handle of the preview window.
* Remarks	Leave "psCaption" as NULL if the SPEC doesn't specify it what to be
**********************************************************************/
HWND PreviewSoundEx(HWND hFrame, HWND hWnd, UINT returnmsg, PSTR psCaption, BOOL bShowBeneSnd)
{
	RECT	rf;
	SNDEXTDATA RgData;
	HWND	hWndMain = NULL;

	RegisterSndExt();
	
	memset(&RgData, 0, sizeof(SNDEXTDATA));
	RgData.hFrame = hFrame;
	RgData.uSmPreReturn = returnmsg;
	RgData.bShowPreset = bShowBeneSnd;
	RgData.hRecMsg = hWnd;
	RgData.bDefTone = FALSE;
	RgData.szDefTone =NULL;
	RgData.nVolume = PRIOMAN_VOLUME_DEFAULT;
	strcpy(RgData.szNowPath, PHONESNDFILEDIR);

	if(NULL == psCaption || strlen(psCaption) <= 0)
		strcpy(RgData.szCaption, IDS_EXTCAPTION);
	else
		strcpy(RgData.szCaption, psCaption);

	GetClientRect(hFrame, &rf);
	
	hWndMain = CreateWindow(
		"SelectSndWndClass", 
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
		if(nBmpCount <= 0)
			UnregisterClass ("SelectSndWndClass", NULL);
		return NULL;
	}
	
	
	SetFocus(hWndMain);
	ShowWindow(hFrame,SW_SHOW);
	UpdateWindow(hFrame);
	
	return	hWndMain;
}

/*********************************************************************
* Function	PRERG_MainWndProc
* Purpose   
* ParaSM	
* Return	
* Remarks	
**********************************************************************/
static LRESULT CALLBACK SelectSndProc(HWND hWnd,UINT wMsgCmd,WPARAM wParam,LPARAM lParam)
{
	PCREATESTRUCT	pCreatestu;
	LRESULT	lResult;
	PSNDEXTDATA	pRgData;
	RECT	rc;
	char	szTemp[MAXFILENAMELEN];
	char	cFileName[MAXFULLNAMELEN];
	int		nFileType;
	static	BOOL bKeyDown = FALSE;

	pRgData = GetUserData(hWnd);

	lResult	=	TRUE;	

	switch(wMsgCmd)
	{
	case WM_CREATE:	
		if(nBmpCount <= 0)
		{
			hFolderBmp	 = LoadImage(NULL, SNDFOLDER_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hMMCBmp	 	 = LoadImage(NULL, MMCFOLDER_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hIconStatic  = LoadImage(NULL, STATIC_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hIconPlaying = LoadImage(NULL, PLAYING_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hBeneStatic  = LoadImage(NULL, BENESND_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hBenePlaying = LoadImage(NULL, BENESNDPL_ICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			nBmpCount = 1;
		}
		else
			nBmpCount ++;
		
		bKeyDown = FALSE;
		
		pCreatestu = (PCREATESTRUCT)lParam;
		memcpy(pRgData, pCreatestu->lpCreateParams, sizeof(SNDEXTDATA));

		pRgData->nPlayingItem = -1;
		PREBROW_InitListBuf (&pRgData->ListBuffer);
		GetClientRect(hWnd, &rc);

		pRgData->hWndList = CreateWindow("LISTBOX","",
			WS_CHILD |WS_VISIBLE|LBS_HASSTRINGS|WS_VSCROLL|LBS_BITMAP|LBS_MULTILINE,
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
			hWnd,(HMENU)IDC_PREPIC_LIST,NULL,NULL);
		if (!pRgData->hWndList) 
			return	FALSE;

		LoadExtViewList(pRgData);

		break;

	case PWM_SHOWWINDOW:
		SND_SetEXNCArea(pRgData);
		break;

	case WM_SETFOCUS:
		SND_SetListBoxFocus(pRgData->hWndList);
		break;

	case WM_PAINT: 
		{
			HDC		hdc;
			int		oldbm;
			hdc	=	BeginPaint( hWnd, NULL);

			if(SendMessage(pRgData->hWndList, LB_GETCOUNT, 0, 0) <= 0)
			{
				GetClientRect(hWnd, &rc);
				oldbm = SetBkMode(hdc,TRANSPARENT);
				DrawText(hdc, IDS_NOSND, -1, &rc, DT_CENTER|DT_VCENTER);
				SetBkMode(hdc,oldbm);
			}
			EndPaint(hWnd, NULL);	
		}
		break;

	case WM_KEYDOWN:
	    switch(wParam)
		{
		case VK_F5:
			{
				int index;
				szTemp[0] = NULL;
				cFileName[0] = NULL;
				
				index = SendMessage(pRgData->hWndList, LB_GETCURSEL, 0, 0);
				if(index < 0)
					break;

				if(0 == index && pRgData->bDefTone)
					SendMessage(pRgData->hRecMsg, pRgData->uSmPreReturn, MAKEWPARAM(0, RTN_SOUND), (LPARAM)"");
				else if(SND_GetInfoFromList(index, &pRgData->ListBuffer, NULL, cFileName, &nFileType))
				{
					if(ISSOUNDFILE(nFileType))
					{
						SendMessage(pRgData->hRecMsg, pRgData->uSmPreReturn, MAKEWPARAM(strlen(cFileName), RTN_SOUND), (LPARAM)cFileName);
					}
					else
					{
						break;
					//	strcpy(pRgData->szNowPath, cFileName);
					//	LoadExtViewList(pRgData);
					}
				}
				else
				{
					SendMessage(pRgData->hRecMsg,pRgData->uSmPreReturn, MAKEWPARAM(0, RTN_NOSELECT), (LPARAM)"");
				}
			}
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
			
		case VK_F10:
			{
#ifdef _SNDEXT_BACK_
				int nType;
				char *szOldItem = NULL;
#endif
				
				if (0 <= pRgData->nPlayingItem)
				{
					pRgData->nPlayingItem = -1;
					PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);
				}
#ifdef _SNDEXT_BACK_			
				nType = SND_GetFolderType(pRgData->szNowPath);
				
				if(FOLDERTYPE_ROOT == nType)		// exit
#endif
				{
					PostMessage(hWnd, WM_CLOSE, 0, 0);
					break;
				}

#ifdef _SNDEXT_BACK_
				szOldItem = (char*)malloc(strlen(pRgData->szNowPath)+1);
				if(NULL != szOldItem)
					strcpy(szOldItem, pRgData->szNowPath);

				if(FOLDERTYPE_UNKNOWN == nType)			// to the top level
					strcpy(pRgData->szNowPath, PHONESNDFILEDIR);
				else if(FOLDERTYPE_MMCROOT == nType)	// to the top level
				{
					strcpy(pRgData->szNowPath, PHONESNDFILEDIR);
				}
				else									// to the parent folder
				{
					if(!SND_GotoParentFolder(pRgData->szNowPath))
					{
						if(NULL != szOldItem)
							free(szOldItem);
						break;
					}
				}
				LoadExtViewList(pRgData);

				nType = SearchSndData(&(pRgData->ListBuffer), szOldItem, PREBROW_FILETYPE_FOLDER);
				if(nType >= 0)
				{
					SendMessage(pRgData->hWndList, LB_SETCURSEL, (WPARAM)nType, 0);
					SND_SetEXNCArea(pRgData);
				}

				if(NULL != szOldItem)
					free(szOldItem);
#endif
			}
			break;

		case VK_RETURN:
			if(bKeyDown)
				break;

			bKeyDown = TRUE;
			SetTimer(hWnd, TIMERID_KEYDOWN, TIMEREL_KEYDOWN, NULL);

			if (0 <= pRgData->nPlayingItem) 
			{
				PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);
				SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PLAY);
				SNDEX_SetSoundIcon(pRgData, FALSE);
				pRgData->nPlayingItem = -1;
			}
			else
			{
				if(GetFileNameFromList(pRgData->hWndList, &pRgData->ListBuffer, NULL, szTemp, cFileName, &nFileType))
				{
					if (ISSOUNDFILE(nFileType))
					{
						PM_PlayMusic pm;
					//	strcpy(pRgData->cMediaFile, cFileName);
						
						pm.hCallWnd = hWnd;
						pm.nPriority = PRIOMAN_PRIORITY_MUSICMAN;
						if(PREBROW_FILETYPE_WAV == nFileType)
							pm.nRingType = PRIOMAN_RINGTYPE_WAVE;
						else if(PREBROW_FILETYPE_AMR == nFileType)
							pm.nRingType = PRIOMAN_RINGTYPE_AMR;
						else
							break;
						pm.nVolume = pRgData->nVolume/*PRIOMAN_VOLUME_DEFAULT*/;
						pm.nRepeat = 1;
						pm.pMusicName = cFileName;
						pm.pDataBuf = NULL;
						pm.nDataLen = 0;
						
						printf("\r\n[sound]: before PrioMan_PlayMusic\r\n");
						if(PRIOMAN_ERROR_SUCCESS != PrioMan_PlayMusic(&pm))
						{
							pRgData->nPlayingItem = -1;
							break;
						}
						printf("\r\n[sound]: after PrioMan_PlayMusic\r\n");
						
						pRgData->bBeneSnd = ISBENESOUND(cFileName);
						pRgData->nPlayingItem = SendMessage(pRgData->hWndList, LB_GETCURSEL, 0, 0);
						SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_STOP);
						
						SNDEX_SetSoundIcon(pRgData, TRUE);
					}
					else
					{
					//	PLXTipsWin(pRgData->hFrame, hWnd, 0, ML("Unknown audio stream"), NULL, Notify_Alert, ML("Ok"), NULL, WAITTIMEOUT);
						strcpy(pRgData->szNowPath, cFileName);
						LoadExtViewList(pRgData);
					}
				}
			}
			break;
		}
		break;

	case WM_TIMER:
		KillTimer(hWnd, wParam);
		if(TIMERID_KEYDOWN == wParam)
			bKeyDown = FALSE;
		break;

	case WM_COMMAND:
		if (HIWORD(wParam) != LBN_SELCHANGE)
			break;

		if (pRgData->nPlayingItem >= 0)
		{
			PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);
			SNDEX_SetSoundIcon(pRgData, FALSE);
			pRgData->nPlayingItem = -1;	
		}

		SND_SetEXNCArea(pRgData);		

		break;
			
	case SPM_SETCURSEL:	// set the selected item in the list
		{
			int nDefault;
			
			if((BOOL)wParam)
				nDefault = (int)lParam;
			else
			{
				if(!IsSound((PSTR)lParam, &nFileType))
					return FALSE;
				
				nDefault = SearchSndData(&(pRgData->ListBuffer), (PCSTR)lParam, nFileType);
			}
			
			if(nDefault < 0)
				return FALSE;

			SendMessage(pRgData->hWndList, LB_SETCURSEL, nDefault, 0);
		}
		break;

	case SPM_SETVOLUME:		// set volume
		pRgData->nVolume = (int)lParam;
		break;

	case SPM_SETDEFTONE:	// set default tone
		{
			PSTR pRing = NULL;
			SCENEMODE scenemode;
		//	int nSmode;
			
			if(pRgData->bDefTone)
				return TRUE;
			
		//	nSmode = GetCurSceneMode();		// get the index of current scene mode
		//	if(!GetSM(&scenemode, nSmode))	// get the details of current scene mode
			if(!Sett_GetActiveSM(&scenemode))
				return FALSE;

			pRing = scenemode.rIncomeCall.cMusicFileName;
			if(NULL == pRing)
				return FALSE;

			if(!SND_IsItemExist(pRing, TRUE))
			{
				if(GetDefaultRingEx(pRing) <= 0)
					return FALSE;
			}
			if(NULL != pRgData->szDefTone)
				free(pRgData->szDefTone);
					
			pRgData->szDefTone = (char*)malloc(strlen(pRing)+1);
			strcpy(pRgData->szDefTone, pRing);
			pRgData->bDefTone = TRUE;

			if(SND_GetFolderType(pRgData->szNowPath) == FOLDERTYPE_ROOT)
				SNDEX_LoadDefaultTone(pRgData);

			SendMessage(pRgData->hWndList, LB_SETCURSEL, (WPARAM)0, NULL);
		}
		break;

	case PRIOMAN_MESSAGE_READDATA:
		PrioMan_ReadData(wParam, lParam);
		break;

	case PRIOMAN_MESSAGE_WRITEDATA:
		PrioMan_WriteData(wParam, lParam);
		break;

	case PRIOMAN_MESSAGE_PLAYOVER:
		PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);
		if (0 <= pRgData->nPlayingItem) 
			SNDEX_SetSoundIcon(pRgData, FALSE);

		if(GetWindow(pRgData->hFrame, GW_CHILD/*GW_LASTCHILD*/) == hWnd)
			SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PLAY);

		pRgData->nPlayingItem = -1;
		break;

	case PRIOMAN_MESSAGE_BREAKOFF:
		PrioMan_BreakOffMusic(PRIOMAN_PRIORITY_MUSICMAN);
		if (0 <= pRgData->nPlayingItem) 
			SNDEX_SetSoundIcon(pRgData, FALSE);

		if(GetWindow(pRgData->hFrame, GW_CHILD/*GW_LASTCHILD*/) == hWnd)
			SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PLAY);
		
		pRgData->nPlayingItem = -1;
		break;

	case PWM_ACTIVATE:
		if(!wParam)
		{
			if(0 <= pRgData->nPlayingItem)
			{
				PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);
				SNDEX_SetSoundIcon(pRgData, FALSE);
				
				if(GetWindow(pRgData->hFrame, GW_CHILD/*GW_LASTCHILD*/) == hWnd)
					SendMessage(pRgData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PLAY);
				
				pRgData->nPlayingItem = -1;
			}
			bKeyDown = FALSE;
		}
	//	f_EnableKeyRing(!wParam);
		lResult = PDADefWindowProc(hWnd, PWM_ACTIVATE, wParam, lParam);
		break;

	case WM_CLOSE:
		bKeyDown = FALSE;
		PostMessage(pRgData->hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		DestroyWindow (hWnd);
		break;

	case WM_DESTROY:
		SNDEX_OnDestroy(pRgData);
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}

	return lResult;	
}

static void SNDEX_SetSoundIcon(PSNDEXTDATA pData, BOOL bPlaying)
{
	if(bPlaying)
	{
		if(pData->bBeneSnd)
		{
			SendMessage(pData->hWndList, LB_SETIMAGE,
				MAKEWPARAM(IMAGE_BITMAP, pData->nPlayingItem), (LPARAM)hBenePlaying);
		}
		else
		{
			SendMessage(pData->hWndList, LB_SETIMAGE,
				MAKEWPARAM(IMAGE_BITMAP, pData->nPlayingItem), (LPARAM)hIconPlaying);
		}
	}
	else
	{
		if(pData->bBeneSnd)
		{
			SendMessage(pData->hWndList, LB_SETIMAGE,
				MAKEWPARAM(IMAGE_BITMAP, pData->nPlayingItem), (LPARAM)hBeneStatic);
		}
		else
		{
			SendMessage(pData->hWndList, LB_SETIMAGE,
				MAKEWPARAM(IMAGE_BITMAP, pData->nPlayingItem), (LPARAM)hIconStatic);
		}
	}
}

static void SNDEX_OnDestroy(PSNDEXTDATA pData)
{
	nBmpCount --;
	if(nBmpCount <= 0)
	{
		if (hFolderBmp) 
		{
			DeleteObject(hFolderBmp);
			hFolderBmp = NULL;
		}
		if(hMMCBmp)
		{
			DeleteObject(hMMCBmp);
			hMMCBmp = NULL;
		}
		if(hIconStatic)
		{
			DeleteObject(hIconStatic);
			hIconStatic = NULL;
		}
		if(hIconPlaying)
		{
			DeleteObject(hIconPlaying);
			hIconPlaying = NULL;
		}
		if(hBeneStatic)
		{
			DeleteObject(hIconStatic);
			hBeneStatic = NULL;
		}
		if(hBenePlaying)
		{
			DeleteObject(hIconPlaying);
			hBenePlaying = NULL;
		}
		nBmpCount = 0;
		UnregisterClass ("SelectSndWndClass", NULL);
	}

	if(NULL== pData)
		return;

	if((pData->bDefTone) && (NULL != pData->szDefTone))
	{
		free(pData->szDefTone);
		pData->szDefTone = NULL;
	}
	
	if (0 <= pData->nPlayingItem) 
	{
		pData->nPlayingItem = -1;
		PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_MUSICMAN);
	}
	PREBROW_FreeListBuf (&pData->ListBuffer);
}

// load sounds and folders
static	int	LoadExtViewList(PSNDEXTDATA pData)
{
	int		num;
	BOOL	bMMC = FALSE;
	int		ntype = FOLDERTYPE_UNKNOWN;

	ntype = SND_GetFolderType(pData->szNowPath);

	num = AddSoundsToExList(pData, ntype);

	// add MMC root folder
	if(FOLDERTYPE_ROOT == ntype)
	{
		if(SND_GetMMCStatus())
		{
			int index;
			char pInfo[32];
			long nNum;
			
			nNum = GetSndFileNumInDir(MMCSNDFOLDERPATH);
			
			memset(pInfo, 0, 32);
			SND_GetMMCName(pInfo);
			index = SendMessage(pData->hWndList,LB_ADDSTRING,NULL,(LPARAM)pInfo);
			if (LB_ERR != index)
			{
				PREBROW_AddSndData(&pData->ListBuffer, pInfo, MMCSNDFOLDERPATH, 0);	//********
				if(nNum != -1)
				{
					sprintf(pInfo, "%s%d    %s%d", ML("Folder:"), HIWORD(nNum), ML("Sounds:"), LOWORD(nNum));
					SendMessage(pData->hWndList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)pInfo);
				}
				SendMessage(pData->hWndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hMMCBmp);
				SendMessage(pData->hWndList, LB_SETITEMDATA, (WPARAM)index, (LPARAM)SOUND_FILETYPE_MMCCARD);
			}
			
			num ++;
		}
		SNDEX_LoadDefaultTone(pData);
	}
	
	if (num > 0)
	{
		SendMessage(pData->hWndList, LB_SETCURSEL, (WPARAM)0, NULL);
	}

	SND_ShowListBox(pData->hWndList);

	SND_SetEXNCArea(pData);
	
	return	num;
}

static int AddSoundsToExList(PSNDEXTDATA pData, int nFolderType)
{
	int n;
	
	if(NULL == pData)
		return FALSE;

	SendMessage (pData->hWndList, LB_RESETCONTENT, NULL, NULL);
	PREBROW_FreeListBuf (&pData->ListBuffer);
	
	if((FOLDERTYPE_ROOT == nFolderType) && (pData->bShowPreset))
	{
//		int			nDataNum;
//		PLISTDATA	pHead;
		
		AddSndFileToList(BENESNDFILEDIR, &pData->ListBuffer, FALSE);
		
//		nDataNum = (pData->ListBuffer).nDataNum;
//		pHead = (pData->ListBuffer).pDataHead;
//		
//		(pData->ListBuffer).nDataNum = 0;
//		(pData->ListBuffer).pDataHead = NULL;
//		(pData->ListBuffer).nCurrentIndex = -1;
//		(pData->ListBuffer).pCurrentData = NULL;
		
		AddSndFileToList(PHONESNDFILEDIR, &pData->ListBuffer, TRUE);

//		if(!(pData->ListBuffer).pDataHead)
//		{
//			(pData->ListBuffer).pDataHead = pHead;
//			(pData->ListBuffer).nDataNum = nDataNum;
//			(pData->ListBuffer).nCurrentIndex = (pHead ? 0 : -1);
//			(pData->ListBuffer).pCurrentData = pHead;
//		}
//		else if(NULL != pHead)
//		{
//			PLISTDATA	pH2, pT1, pT2;
//
//			pT1 = pHead->pPre;
//			pH2 = (pData->ListBuffer).pDataHead;
//			pT2 = pH2->pPre;
//
//			pHead->pPre = pT2;
//			pT2->pNext = pHead;
//
//			pT1->pNext = pH2;
//			pH2->pPre = pT1;
//
//			(pData->ListBuffer).pDataHead = pHead;
//			(pData->ListBuffer).nDataNum += nDataNum;
//			(pData->ListBuffer).nCurrentIndex = 0;
//			(pData->ListBuffer).pCurrentData = pHead;
//		}
	}
	else
	{
		AddSndFileToList(pData->szNowPath, 
			&pData->ListBuffer, (FOLDERTYPE_SUB != nFolderType));
	}

	LoadExListFromBuffer(pData);
	
	n = (pData->ListBuffer).nDataNum;
//	if (n > 0)
//	{
//		SendMessage(pData->hWndList, LB_SETCURSEL, (WPARAM)0, NULL);
//	}

	return n;
}

static int LoadExListFromBuffer(PSNDEXTDATA pData)
{
	HBITMAP		hbitbmp = NULL;
	PLISTDATA	pNode = NULL;
    int			nRtnNum = 0;
	int			index = -1;
	struct stat	filestat;
	char		SndSize[18];
	float		tmpsize;
	DWORD		nFileNum;
	char		FileName[PREBROW_MAXFILENAMELEN];
	
	pNode = (pData->ListBuffer).pDataHead;
	if(NULL == pNode)
		return 0;

	do
	{
		if(ISSOUNDFILE(pNode->nFVLine))	// file
		{
		//	index = SendMessage(pData->hWndList, LB_ADDSTRING, 0, (LPARAM)(pNode->szData));
			index = SND_AddListBoxItem(pData->hWndList, pNode->szData);
			if (LB_ERR == index)
				break;
			
			if(ISBENESOUND(pNode->szFullData))
			{
				if(index == pData->nPlayingItem)
					hbitbmp = hBenePlaying;
				else
					hbitbmp = hBeneStatic;
			}
			else
			{
				if(index == pData->nPlayingItem)
					hbitbmp = hIconPlaying;
				else
					hbitbmp = hIconStatic;
				
				//-------get the stat of current file------------------
				stat((pNode->szFullData), &filestat);
				tmpsize = (float)filestat.st_size/1024;
				floattoa(tmpsize, SndSize);
				strcat(SndSize, " kB");

				SendMessage(pData->hWndList,LB_SETAUXTEXT,MAKEWPARAM(index,-1),(LPARAM)SndSize);
			}

			SendMessage(pData->hWndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hbitbmp);
			SendMessage(pData->hWndList, LB_SETITEMDATA, (WPARAM)index, (LPARAM)(pNode->nFVLine));

		}
		else	// folder
		{
			nFileNum = GetSndFileNumInDir(pNode->szFullData);

			index = SND_AddListBoxItem(pData->hWndList, pNode->szData);
			if (LB_ERR == index)
				break;

			if((long)nFileNum != -1)
			{
				sprintf(FileName, "%s%d    %s%d", ML("Folder:"), HIWORD(nFileNum), 
					ML("Sounds:"), LOWORD(nFileNum));	
				SendMessage(pData->hWndList, LB_SETAUXTEXT, MAKEWPARAM(index, -1), (LPARAM)FileName);
			}
			SendMessage(pData->hWndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)hFolderBmp);
			SendMessage(pData->hWndList, LB_SETITEMDATA,(WPARAM)index,(LPARAM)PREBROW_FILETYPE_FOLDER);
		}
		nRtnNum++;
		pNode = pNode->pNext;
	} while(pNode &&(pNode != (pData->ListBuffer).pDataHead));

	return nRtnNum;
}

static BOOL SND_SetEXNCArea(PSNDEXTDATA pData)
{
	int		ntype = 0;
	int		nIndex = -1;
	int		nItemType = FILE_TYPE_UNKNOW;

	if(NULL == pData->hWndList || NULL == pData->szNowPath)
		return FALSE;

	SendMessage(pData->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_LEFTICON), NULL);
	SendMessage(pData->hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_RIGHTICON), NULL);
	PDASetMenu(pData->hFrame, NULL);
	
#ifdef _SNDEXT_BACK_
	ntype = SND_GetFolderType(pData->szNowPath);
	if(FOLDERTYPE_ROOT != ntype)
		SendMessage(pData->hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
	else
#endif
		SendMessage(pData->hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	
//	SetWindowText(pData->hFrame, pData->szCaption);
	SND_SetWindowText(pData->hFrame, pData->szCaption);
	
	if ( SendMessage(pData->hWndList, LB_GETCOUNT, 0, 0) > 0 )
	{
		nIndex = SendMessage(pData->hWndList, LB_GETCURSEL, 0, 0);
		if(LB_ERR == nIndex)
		{
			nIndex = 0;
			SendMessage(pData->hWndList, LB_SETCURSEL, 0, 0);
		}

		nItemType = SendMessage(pData->hWndList, LB_GETITEMDATA, nIndex, 0);
	}

	// set menu and left button text
	switch(nItemType)
	{
	case PREBROW_FILETYPE_FOLDER:
	case SOUND_FILETYPE_MMCCARD:
		SendMessage(pData->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(pData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
		break;

	case PREBROW_FILETYPE_WAV:
	case PREBROW_FILETYPE_AMR:
		
		if(nIndex == pData->nPlayingItem)
			SendMessage(pData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_STOPSND);
		else
			SendMessage(pData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_PLAYSND);

		SendMessage(pData->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		break;

	default:
		SendMessage(pData->hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(pData->hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
	//	ShowWindow(pData->hWndList, SW_HIDE);
		break;
	}

//	SetFocus(pData->hWndList);
	return TRUE;
}	

static BOOL SNDEX_LoadDefaultTone(PSNDEXTDATA pData)
{
	HBITMAP hbitmap;
	int nType, nIndex;

	if(NULL == pData)
		return FALSE;

	if(!(pData->bDefTone) || (NULL == pData->szDefTone))
		return FALSE;

	if(!IsSound(pData->szDefTone, &nType))
		return FALSE;
	
	PREBROW_AddSndData(&(pData->ListBuffer), ML("Default tone"), pData->szDefTone, 0);
	if(pData->ListBuffer.pDataHead && pData->ListBuffer.pDataHead->pPre)
	{
		pData->ListBuffer.pDataHead = pData->ListBuffer.pDataHead->pPre;
		pData->ListBuffer.pDataHead->nFVLine = nType;
		pData->ListBuffer.nDataNum ++;
	}
	else
		return FALSE;

	nIndex = SendMessage(pData->hWndList, LB_INSERTSTRING, 0, (LPARAM)ML("Default tone"));
	if (0 != nIndex)
		return FALSE;
	
	if(pData->nPlayingItem >= 0)
		pData->nPlayingItem ++;

	if(ISBENESOUND(pData->szDefTone))
	{
		if(0 == pData->nPlayingItem)
			hbitmap = hBenePlaying;
		else
			hbitmap = hBeneStatic;
	}
	else
	{
		if(0 == pData->nPlayingItem)
			hbitmap = hIconPlaying;
		else
			hbitmap = hIconStatic;
	}
	
	SendMessage(pData->hWndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hbitmap);
	SendMessage(pData->hWndList, LB_SETITEMDATA, (WPARAM)0, (LPARAM)nType);
	return TRUE;
}

/*********************************************************************\
* Function	SND_GetFirstSound
* Purpose	Get the full path of the first file listed
* Params	const char*	pFilePath	path of the target folder
*			char *		szPath		buffer for the path
* Return	If the function succeeds, the return value is the length, 
*			in characters, of the copied string, not including the 
*			terminating null character.
*			If error occurs, the return value is 0.
* Remarks	only wav and amr files are supported.
*			the muxium length of the path will be less than 512
**********************************************************************/
static int SND_GetFirstSound(const char *pFilePath, char *szPath)
{
	struct dirent *dirinfo = NULL;
	struct stat	filestat;
	DIR		*dirtemp = NULL;
	int		nItemData;
	int		nRet = 0;
	int		pathlen;
	char	FullName[512];
	char	FileName[64] = "";

	if(NULL == pFilePath || NULL == szPath)
		return 0;
	
	dirtemp = opendir(pFilePath);
	if(dirtemp == NULL)
		return 0;
	
	memset(FullName, 0, 512);
	strcpy(FullName, pFilePath);
		
	if (!ISPATHSEPARATOR(FullName[strlen(FullName)-1]) )
		strcat(FullName,"/");
	
	pathlen = strlen(FullName);
	
	while((dirinfo = readdir(dirtemp))!=NULL)
	{
		if(ISPRESERVEDPATH(dirinfo->d_name))
			continue;
		
		FullName[pathlen] = '\0';
		strcat (FullName, dirinfo->d_name);
		stat(FullName, &filestat);
		
		if(S_ISREG(filestat.st_mode))
		{
			if(!IsSound(FullName, &nItemData)) 
			{
				continue;
			}
			if(strcasecmp(FileName, dirinfo->d_name) > 0 || strlen(FileName) <= 0)
			{
				strcpy(FileName, dirinfo->d_name);
				strcpy(szPath, FullName);
				nRet = strlen(FullName);
			}
			
		}
	}
	closedir(dirtemp);
	chdir(pFilePath);

	return nRet;
}

/*********************************************************************\
* Function	GetDefaultRingEx
* Purpose	Get the full path of the first file listed
* Params	char*	pFilePath	address of buffer for text
* Return	If the function succeeds, the return value is the length, 
*			in characters, of the copied string, not including the 
*			terminating null character.
*			If error occurs, the return value is 0.
* Remarks	only wav and amr files are supported.
*			the muxium length of the path will be less than 512
**********************************************************************/
int GetDefaultRingEx(char *pFilePath)
{
	int nRet = 0;

	if(NULL == pFilePath)
		return 0;

	nRet = SND_GetFirstSound(BENESNDFILEDIR, pFilePath);
	if(nRet <= 0)
		nRet = SND_GetFirstSound(PHONESNDFILEDIR, pFilePath);
	else
	{
		int nTmp = 0;
		char tmpname[512];
		nTmp = SND_GetFirstSound(PHONESNDFILEDIR, tmpname);
		if(nTmp > 0 && strcasecmp(pFilePath, tmpname) > 0)
		{
			strcpy(pFilePath, tmpname);
			nRet = nTmp;
		}
	}
	
	return nRet;
}

long SND_StatMMCFolder()
{
	long lRet = StatFolder(MMCSNDFOLDERPATH);
	chdir(MMCSNDFOLDERPATH);
	return lRet;
}

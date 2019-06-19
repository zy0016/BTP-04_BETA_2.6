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

#define	MULTISELWC	"MULTISELECTWNDC"
#define	DEFNAMECWND	"DEFANOTHERNAMEWC"

typedef	struct tagMULTISELPICWND {
	HWND	hSelMultiWnd;
	HWND	hMultiList;
	HWND	hFrame;
	PLISTBUF	pMultiList;
	DWORD	DOMSG;
	char	szCaption[30];
	char	leftButton[30];
}MULTISELWD, *PMULTISELWD;

typedef	struct tagDEFANOTHERNAME {
	HWND	hFrameWnd;
	HWND	hRecMsg;
	HWND	hRenameEdit;
	UINT	uMsg;
	DWORD	dReserved;
	char	pOldName[PREBROW_MAXFULLNAMELEN];
	PSTR	pEditContent;
	PSTR	pOnlyName;
}DEFNAMEWD, *PDEFNAMEWD;

static	HWND	hSelMultiWnd = NULL;
static	HWND	hMultiList	=	NULL;
static	HWND	hFrame;
static	HWND	hListBox = NULL;
static	PLISTBUF	pMultiList;
static	DWORD	DOMSG;
static	char	szCaption[30] = "";
static	char	leftButton[30] = "";
static	BOOL	bContinue = FALSE;

extern	HMENU	hSendSelMenu;
extern	HBITMAP	hSpareMiniature;
static	BOOL	RegisterMultiSelWndClass();
static	LRESULT	MultiSelProc(HWND hwnd, UINT umsg, WPARAM wapram, LPARAM lparam);
static	LRESULT	DefOtherNameProc(HWND hwnd, UINT umsg, WPARAM wapram, LPARAM lparam);
static BOOL PicIsSelected(int i);


static	BOOL	RegisterMultiSelWndClass()
{
	
	WNDCLASS  wc;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = MultiSelProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = 0;
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = NULL;
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = MULTISELWC;
	
	if (!RegisterClass(&wc))
		return FALSE;
	return TRUE;
}

BOOL ProcessMultiSel(HWND hFramewnd, HWND hMsg, PLISTBUF pList, DWORD wID, HWND hList)
{
	RECT	rf;
	RegisterMultiSelWndClass();
	switch(wID) {
	case IDC_DELSELECT:
		//strcpy(szCaption, IDS_DELETEMANY);
		//strcat(szCaption, "...");
		strcpy(leftButton, IDS_DELETE);
		break;

	case IDC_SENDSELECT:
		//strcpy(szCaption, IDS_SENDMANY);
		//strcat(szCaption, "...");
		strcpy(leftButton, IDS_SEND);
		break;

	case IDC_MOVESELECT:
		//strcpy(szCaption, IDS_MOVEMANY);
		//strcat(szCaption, "...");
		strcpy(leftButton, IDS_MOVE);
		break;

	case IDC_COPYSELECT:
		strcpy(leftButton, IDS_COPY);
		break;
	
	}
	DOMSG = wID;
	hFrame = hFramewnd;
	pMultiList = pList;
	hListBox = hList;
	GetClientRect(hFrame, &rf);
	hSelMultiWnd = CreateWindow(MULTISELWC, NULL,
		WS_CHILD|WS_VISIBLE,
		rf.left,
		rf.top,
		rf.right - rf.left, 
		rf.bottom - rf.top,
		hFrame, NULL,NULL,NULL	);
	if (!hSelMultiWnd) {
		return FALSE;
	}
	SetWindowText(hFrame, IDS_SELPICCAPTION);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)leftButton);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
	SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
	PDASetMenu(hFrame, NULL);
	ShowWindow(hFrame, SW_SHOW);
	UpdateWindow(hFrame);
	SetFocus(hSelMultiWnd);
	return TRUE;
}

static	LRESULT	MultiSelProc(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	LRESULT	lResult;
	PLISTDATA	pNode;
	int		nIndex, iCount, AvailSpace;
	char	cFileName[51];
	char	cFullName[256];
	struct	stat	filestat;
	float		tmpsize;
	char	PicSize[18];
	char	*p;
	RECT	rClient;
	FILE	*fp;
	switch(umsg) 
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rClient);
		hMultiList = CreateWindow("PICMULTILISTEX", NULL, WS_VISIBLE|WS_CHILD|LBS_BITMAP|LBS_MULTILINE|WS_VSCROLL, 
			rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
			hwnd, (HMENU)ID_MULTISLELIST, NULL, NULL);
		pNode = pMultiList->pDataHead;
		if (pNode) {
			do 
			{
				if (strchr(pNode->szFullData,'.')) {
				//	memset(&filestat, 0, sizeof(struct stat));
					nIndex = SendMessage(hMultiList, LB_ADDSTRING, 0, (LPARAM)pNode->szData);
					stat(pNode->szFullData, &filestat);
					tmpsize = (float)filestat.st_size/1024;
					//sprintf(PicSize, "%d Kb", tmpsize);
					floattoa(tmpsize, PicSize);
					strcat(PicSize, " kB");
					SendMessage(hMultiList,LB_SETAUXTEXT,MAKEWPARAM(nIndex,-1),(LPARAM)PicSize);
					SendMessage(hMultiList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, MAKEWORD(nIndex, 2)), (LPARAM)pNode->hbmp);
				}
				pNode = pNode->pNext;
			}while (pNode!= pMultiList->pDataHead &&pNode);
		}
		SendMessage(hMultiList, LB_ENDINIT, 0, 0);
		SendMessage(hMultiList,LB_SETCURSEL,0,0);
		
		break;


	case PWM_SHOWWINDOW:
		SetWindowText(hFrame, IDS_SELPICCAPTION);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)leftButton);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		PDASetMenu(hFrame, NULL);
		SetFocus(hSelMultiWnd);
		break;

	case WM_SETFOCUS:
		SetFocus(hMultiList);
		break;

	case WM_KEYDOWN:
		switch(wparam) 
		{
		case VK_F10:
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case VK_RETURN:
			{
				int	i,j, k;
				k=0;
				iCount = SendMessage(hMultiList, LB_GETCOUNT, 0, 0);
				j = SendMessage(hMultiList, LB_GETSELCOUNT, 0, 0);
				if (j<=0) {
					PLXTipsWin(hFrame, hwnd, 0, IDS_SELPIC, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
					break;
				}
				switch(DOMSG) 
				{
				case IDC_DELSELECT:
					if (PLXConfirmWin(hFrame, hwnd, IDS_DELSELPIC, Notify_Request, NULL, IDS_YES, IDS_NO)) 
					{
						WaitWindowStateEx(hFrame, TRUE, IDS_DELETING, NULL, NULL, NULL);
						for (i=iCount-1; i>=0; i--)
						{
							if(PicIsSelected(i))
							{
								
								if(PREBROW_GetData(pMultiList, i, cFileName, cFullName))
								{
									
									SendMessage(hMultiList, LB_DELETESTRING, i, 0);
									SendMessage(hListBox, LB_DELETESTRING, i, 0);
									PREBROW_DelData(pMultiList, i);	
									//k++;
									remove(cFullName);
								}
								
							}
						}
						WaitWindowStateEx(hFrame, FALSE, IDS_DELETING, NULL, NULL, NULL);
						PLXTipsWin(hFrame, hwnd, 0, IDS_DELETED, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
						SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
						PostMessage(hwnd, WM_CLOSE, 0, 0);
					}
					break;

				case IDC_COPYSELECT:
					PicMoveToFolder(hwnd, IDRM_COPYTOFOLER);
					break;

				case IDC_MOVESELECT:
					PicMoveToFolder(hwnd, IDRM_MOTOFOLDER);
					break;
					
				case IDC_SENDSELECT:
					{
						PDASetMenu(hFrame, hSendSelMenu);
						PostMessage(hFrame, WM_KEYDOWN, VK_F5, 0);
					}
					break;
				}
				
				
			}
			
			break;
		}
		break;
		
	case PIC_LOADBMP:
		{
			Image_LoadMiniature(LOWORD(wparam), HIWORD(wparam), hMultiList, pMultiList);
		}
		break;
			
		

	case WM_COMMAND:
		switch(LOWORD(wparam)) 
		{
		case ID_MULTISLELIST:
			switch(HIWORD(wparam)) 
			{
			case LBN_SELCHANGE:
				loadBitmapForList(hMultiList, 2, pMultiList);
				break;
			default:
				lResult = PDADefWindowProc(hwnd, umsg, wparam, lparam);
			}
			break;

		case IDC_VIAMMS:
			{
				int i;
				PLISTATTNODE pHead = NULL;
				iCount = SendMessage(hMultiList, LB_GETCOUNT, 0, 0);
				for (i=0; i<iCount; i++)
				{
					if(PicIsSelected(i))
					{
						
						if(PREBROW_GetData(pMultiList, i, cFileName, cFullName))
						{
							AddMultiSelPicToList(&pHead, cFileName, cFullName);
							//APP_EditMMS(hFrame,hwnd, IDRM_EDITMMS, MMS_CALLEDIT_IMAGE, cFullName);
						}
						
					}
				}
				APP_EditMMS(hFrame,hwnd, IDRM_EDITMMS, MMS_CALLEDIT_MULTIIMAGE, (PSTR)pHead);
				FreeMultiSelList(&pHead);
				pHead = NULL;
				SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;

		case IDC_VIAEMAIL:
			{
				int i;
				PLISTATTNODE pHead = NULL;
				iCount = SendMessage(hMultiList, LB_GETCOUNT, 0, 0);
				for (i=0; i<iCount; i++)
				{
					if(PicIsSelected(i))
					{
						
						if(PREBROW_GetData(pMultiList, i, cFileName, cFullName))
						{
							p = strrchr(cFullName,'/');
							AddMultiSelPicToList(&pHead, p+1, cFullName);
							//MAIL_CreateEditInterface(hFrame, NULL, cFullName, p+1,1);
						}
						
					}
				}
				MAIL_CreateMultiInterface(hFrame, pHead);
				FreeMultiSelList(&pHead);
				pHead = NULL;
				SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;

		case IDC_VIABLUETOOTH:
			{
				int i;
				PLISTATTNODE pHead = NULL;
				iCount = SendMessage(hMultiList, LB_GETCOUNT, 0, 0);
				for (i=0; i<iCount; i++)
				{
					if(PicIsSelected(i))
					{
						
						if(PREBROW_GetData(pMultiList, i, cFileName, cFullName))
						{
							//p = strrchr(cFullName,'/');
							AddMultiSelPicToList(&pHead, cFileName, cFullName);
						}
						
					}
				}
				BtSendData(hFrame,(PSTR)pHead, NULL, BTMULTIPICTURE);
				FreeMultiSelList(&pHead);
				pHead = NULL;
				SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
				PostMessage(hwnd, WM_CLOSE, 0, 0);
				
				
			}
			break;
		default:
			lResult = PDADefWindowProc(hwnd, umsg, wparam, lparam);
			break;
		}
		break;

	case IDRM_COPYTOFOLER:
		{
			int	i, k=0;
			char	NewFullName[PREBROW_MAXFULLNAMELEN];
			char	*name;
			if (lparam > 0) {
				strcpy(NewFullName, (PSTR)lparam);
			}
			else
				break;
			iCount = SendMessage(hMultiList, LB_GETCOUNT, 0, 0);
			WaitWindowStateEx(hwnd, TRUE, IDS_COPING, NULL, NULL, NULL);
			for (i=0; i<iCount; i++)
			{
				if(PicIsSelected(i))
				{
					
					if(PREBROW_GetData(pMultiList, i, cFileName, cFullName))
					{
						
						if (NewFullName[strlen(NewFullName) -1] != '/') {
							strcat(NewFullName,"/");
						}
						name = strrchr(cFullName, '/');
						name++;
						strcat(NewFullName,name);

						
						stat(cFullName, &filestat);
						if (strnicmp(NewFullName, MMCPICFOLDERPATH, strlen(MMCPICFOLDERPATH)) == 0)
						{
							AvailSpace = GetAvailMMCSpace();
						}
						else
						{
							AvailSpace = GetAvailFlashSpace();
						}
						if ((int)filestat.st_size/1024 > AvailSpace)
						{
							printf("%s\n", IDS_MEMORYFULL);
							WaitWindowStateEx(hwnd, FALSE, IDS_COPING, NULL, NULL, IDS_CANCEL);
							PLXTipsWin(hFrame, hwnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
							SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
							DestroyWindow(hwnd);
							return FALSE;
						}
						if ((fp = fopen(NewFullName, "r")) != NULL) 
						{
							fclose(fp);
							//WaitWindowStateEx(hwnd, FALSE, IDS_COPING, NULL, NULL, IDS_CANCEL);
							//PLXTipsWin(hFrame, hwnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							//SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
							//DestroyWindow(hwnd);
							DefAnotherName(NULL, hwnd, cFullName, NewFullName, cFileName, 0, 0);
							//return FALSE;
						}
						printf("begin to Copyfile from %s to %s\n",cFullName, NewFullName);
						if(PicCopyFile(cFullName, NewFullName))
							printf("Success to create file %s\n", NewFullName);
						strcpy(NewFullName, (PSTR)lparam);
					}
					
				}
			}
			WaitWindowStateEx(hwnd, FALSE, IDS_COPING, NULL, NULL, IDS_CANCEL);
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
			DestroyWindow(hwnd);
		}
		break;
		
	case IDRM_MOTOFOLDER:
		{
			int	i, k=0;
			char	NewFullName[PREBROW_MAXFULLNAMELEN];
			char	*name;
			if (lparam > 0) {
				strcpy(NewFullName, (PSTR)lparam);
			}
			else
				break;
			iCount = SendMessage(hMultiList, LB_GETCOUNT, 0, 0);
			WaitWindowStateEx(hwnd, TRUE, IDS_MOVING, NULL, NULL, NULL);
			for (i=0; i<iCount; i++)
			{
				if(PicIsSelected(i))
				{
					
					if(PREBROW_GetData(pMultiList, i-k, cFileName, cFullName))
					{
						
						if (NewFullName[strlen(NewFullName) -1] != '/') {
							strcat(NewFullName,"/");
						}
						name = strrchr(cFullName, '/');
						name++;
						strcat(NewFullName,name);

						
						if (strnicmp(cFullName, NewFullName, 10) != 0)
						{
							stat(cFullName, &filestat);
							if (strnicmp(NewFullName, MMCPICFOLDERPATH, strlen(MMCPICFOLDERPATH)) == 0)
							{
								AvailSpace = GetAvailMMCSpace();
							}
							else
							{
								AvailSpace = GetAvailFlashSpace();
							}
							if ((int)filestat.st_size/1024 > AvailSpace)
							{
								WaitWindowStateEx(hwnd, FALSE, IDS_MOVING, NULL, NULL, IDS_CANCEL);
								PLXTipsWin(hFrame, hwnd, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
								SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
								DestroyWindow(hwnd);
								return FALSE;
							}
						}

						if ((fp = fopen(NewFullName, "r")) != NULL) 
						{
							fclose(fp);
							DefAnotherName(NULL, hwnd, cFullName, NewFullName, cFileName, 0, 0);
//							WaitWindowStateEx(hwnd, FALSE, IDS_MOVING, NULL, NULL, IDS_CANCEL);
//							PLXTipsWin(hFrame, hwnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
//							return FALSE;
						}

						if(PicMoveFile(cFullName, NewFullName))
						{
							SendMessage(hMultiList, LB_DELETESTRING, i-k, 0);
							PREBROW_DelData(pMultiList, i-k);
							k++;
						}
						strcpy(NewFullName, (PSTR)lparam);
					}
					
				}
			}
			WaitWindowStateEx(hwnd, FALSE, IDS_MOVING, NULL, NULL, NULL);
			PLXTipsWin(hFrame, hwnd, 0, IDS_MOVED, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
			DestroyWindow(hwnd);
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		UnregisterClass(MULTISELWC, NULL);
		break;

	default:
		lResult = PDADefWindowProc(hwnd, umsg, wparam, lparam);
		break;
	}
	return lResult;
}
static BOOL PicIsSelected(int i)
{
	if (SendMessage(hMultiList, LB_GETSEL, (WPARAM)i, 0)) 
		return TRUE;
	else
		return FALSE;
}
BOOL	AddMultiSelPicToList(PLISTATTNODE *pHead, PCSTR pName, PCSTR pPath)
{
	PLISTATTNODE	pTailNode, pTmp;
	if (!(*pHead))
	{
		*pHead = malloc(sizeof(LISTATTNODE));
		if (!(*pHead))
		{
			return FALSE;
		}
		if (!pName)
		{
			(*pHead)->AttName = (PSTR)pName;
		}
		else
		{
			(*pHead)->AttName = malloc(strlen(pName)+1);
			if (!((*pHead)->AttName))
			{
				return FALSE;
			}
			strcpy((*pHead)->AttName, pName);
		}
		if (!pPath)
		{
			(*pHead)->AttPath = (PSTR)pPath;
		}
		else
		{
			(*pHead)->AttPath = malloc(strlen(pPath)+1);
			if (!((*pHead)->AttPath))
			{
				return FALSE;
			}
			strcpy((*pHead)->AttPath, pPath);
		}		
		(*pHead)->pNext = NULL;
		return TRUE;
	}
	pTmp = *pHead;
	while (pTmp->pNext)
	{
		pTmp = pTmp->pNext;
	}
	pTailNode = malloc(sizeof(LISTATTNODE));
	if (!pTailNode)
	{
		return FALSE;
	}
	if (!pName)
	{
		pTailNode->AttName = (PSTR)pName;
	}
	else
	{
		pTailNode->AttName = malloc(strlen(pName)+1);
		if (!(pTailNode->AttName))
		{
			return FALSE;
		}
		strcpy(pTailNode->AttName, pName);
	}
	if (!pPath)
	{
		pTailNode->AttPath = (PSTR)pPath;
	}
	else
	{
		pTailNode->AttPath = malloc(strlen(pPath)+1);
		if (!(pTailNode->AttPath))
		{
			return FALSE;
		}
		strcpy(pTailNode->AttPath, pPath);
	}
	pTailNode->pNext = NULL;
	pTmp->pNext = pTailNode;
	return TRUE;
}
void	FreeMultiSelList(PLISTATTNODE *pHead)
{
	PLISTATTNODE	pTailNode, pTmp;
	pTailNode = pTmp = *pHead;
	while (pTmp)
	{
		pTailNode = pTmp->pNext;
		if (pTmp->AttName)
		{
			free(pTmp->AttName);
		}
		if (pTmp->AttPath)
		{
			free(pTmp->AttPath);
		}
		free(pTmp);
		pTmp = pTailNode;
	}

}

static	BOOL	RegisterDefNameCWnd()
{
	
	WNDCLASS  wc;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = DefOtherNameProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = sizeof(DEFNAMEWD);
	wc.hInstance        = NULL;
	wc.hIcon            = NULL;
	wc.hCursor          = NULL;//LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground    = NULL;
	wc.lpszMenuName     = NULL;
	wc.lpszClassName    = DEFNAMECWND;
	
	if (!RegisterClass(&wc))
		return FALSE;
	return TRUE;
}


BOOL	DefAnotherName(HWND hFrame, HWND hRecWnd, PSTR pOldName, PSTR pNewName, PSTR pOnlyName, UINT uMsg, DWORD dReserved)
{
	HWND		hDefNameWnd = NULL;
	DEFNAMEWD	dNWndDate;
	MSG			dMsg;

	memset(&dNWndDate, 0, sizeof(DEFNAMEWD));
	dNWndDate.hFrameWnd = hFrame;
	dNWndDate.hRecMsg = hRecWnd;
	strcpy(dNWndDate.pOldName , pOldName);
	dNWndDate.pEditContent = pNewName;
	dNWndDate.pOnlyName = pOnlyName;
	//strcpy(dNWndDate.pEditContent, pOldName);
	dNWndDate.uMsg = uMsg;
	dNWndDate.dReserved = dReserved;

	RegisterDefNameCWnd();
	
	hDefNameWnd = CreateWindow(DEFNAMECWND,
		IDS_RENAMEPICTITLE,
		WS_VISIBLE | WS_CAPTION|PWS_STATICBAR,
		PLX_WIN_POSITION,
		hRecWnd,
		NULL,
		NULL,
		&dNWndDate);

	if (!hDefNameWnd)
	{
		return FALSE;
	}
	SendMessage(hDefNameWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025, 1),(LPARAM)IDS_SAVE);
	SendMessage(hDefNameWnd,PWM_CREATECAPTIONBUTTON, MAKEWPARAM(1025, 0),(LPARAM)IDS_CANCEL);

	while (GetMessage(&dMsg, NULL, 0, 0))
	{
		if ((dMsg.message == IDM_DEFNAMESU)&&(dMsg.hwnd == hDefNameWnd))
		{
			
			//strcpy(pNewName,dNWndDate.pEditContent);
			DestroyWindow(hDefNameWnd);
			hDefNameWnd = NULL;
			
			return (BOOL)dMsg.lParam;
			//break;
		}
		TranslateMessage(&dMsg);
		DispatchMessage(&dMsg);
		
	}
	return	TRUE;
}
static	LRESULT	DefOtherNameProc(HWND hWnd,UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT	lResult;
	PDEFNAMEWD	pDnwd;
	
	PSTR	ptr;
	int		txtlen;
	char	delprompt[PREBROW_MAXFULLNAMELEN] = "";
	char	*cPath = NULL;
	lResult = TRUE;

	pDnwd = GetUserData(hWnd);

	switch(uMsg) 
	{
	case WM_CREATE:
		{
			IMEEDIT	ie;
			PCREATESTRUCT	pcs;
			RECT	rcClient;
			pcs = (PCREATESTRUCT)lParam;
			memcpy(pDnwd, (PDEFNAMEWD)pcs->lpCreateParams, sizeof(DEFNAMEWD));

			memset(&ie, 0, sizeof(IMEEDIT));
			ie.hwndNotify	= hWnd ;    
			ie.dwAttrib	    = 0;                
			ie.dwAscTextMax	= 0;
			ie.dwUniTextMax	= 0;
			ie.wPageMax	    = 0;        
			ie.pszCharSet	= NULL;
			ie.pszTitle	    = NULL;
			ie.pszImeName	= NULL;
			
			GetClientRect(hWnd,&rcClient);
			PLXTipsWin(NULL, hWnd, IDRM_DEFOTHERNAME, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
			pDnwd->hRenameEdit	=	CreateWindow("IMEEDIT",NULL,
				WS_VISIBLE| WS_CHILD|ES_AUTOHSCROLL|ES_TITLE,
				rcClient.left,
				rcClient.top,
				rcClient.right - rcClient.left,
				52,//rcClient.bottom - rcClient.top,
				//52,
				hWnd,NULL,NULL,(PVOID)&ie);
			SendMessage(pDnwd->hRenameEdit,EM_LIMITTEXT,40,0);
			
			SendMessage(pDnwd->hRenameEdit,EM_SETTITLE,0,(LPARAM)IDS_NEWNAME);
			SendMessage(pDnwd->hRenameEdit, WM_SETTEXT, 0, (LPARAM)pDnwd->pOnlyName);
			SendMessage(pDnwd->hRenameEdit, EM_SETSEL, -1, -1);
		}				
		break;

	case WM_SETFOCUS:
		SetFocus(pDnwd->hRenameEdit);		
		SendMessage(pDnwd->hRenameEdit,EM_SETSEL, -1, -1);
		break;

	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_RETURN:
			{
				char	szPath[PREBROW_MAXFULLNAMELEN] = "";
				char	szSDName[41] = "";
				PSTR	pSuffix = NULL;
				FILE	*fp;
				txtlen	= GetWindowTextLength(pDnwd->hRenameEdit);
				GetWindowText(pDnwd->hRenameEdit, szSDName, 41);

				if (txtlen < 1) 
				{
					PLXTipsWin(NULL, NULL, 0, IDS_RNMNOTIFY,NULL,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
					break;
				}
				else if(!IsValidFileorFolderName(szSDName))
				{
					PLXTipsWin(NULL, NULL, 0, IDS_DEFOTHERNAME, NULL,Notify_Alert,IDS_OK,NULL,WAITTIMEOUT);
					break;
				}
				else
				{					
					ptr		= malloc(txtlen+1);
					GetWindowText(pDnwd->hRenameEdit,ptr,txtlen+1);
					memset(delprompt,0,PREBROW_MAXFULLNAMELEN);
					
					
					cPath = strrchr(pDnwd->pOldName, '/');
					strncpy(szPath, pDnwd->pOldName, cPath - pDnwd->pOldName+1);
					pSuffix = strrchr(pDnwd->pOldName, '.');
					if (!pSuffix)
					{
						free(ptr);
						ptr = NULL;
						PostMessage(hWnd, IDM_DEFNAMESU, 0, 0);
						break;
					}
					//pSuffix++;
					sprintf(delprompt, "%s%s%s", szPath,ptr,pSuffix);
					if (((fp = fopen(delprompt, "r")) == NULL)&&((strlen(szPath) + strlen(ptr)+ strlen(pSuffix)) < PREBROW_MAXFULLNAMELEN ))
					{
						//fclose(fp);
						strcpy(pDnwd->pEditContent,delprompt);
						PostMessage(hWnd, IDM_DEFNAMESU, 0, 1);
					}
					else
					{
						fclose(fp);
						PLXTipsWin(NULL, hWnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
						free(ptr);
						ptr = NULL;
						break;
					}					
					
					if (ptr)
					{
						free(ptr);
					}					
					//PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					//PostMessage(hWnd,WM_CLOSE,0,0);
					//PostMessage(hEditParent, IDRM_CLOSEWINDOW, 0, 0);
					break;
				}
			}
			break;

		case VK_F10:
			PostMessage(hWnd, IDM_DEFNAMESU, 0, 0);
			break;

		default:
			lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
			break;
		}
		break;

//	case WM_CLOSE:
//		DestroyWindow(hWnd);
//		UnregisterClass(RENAMEWCLS,NULL);
//		break;

	case WM_DESTROY:
		UnregisterClass(DEFNAMECWND,NULL);
		break;

	default:
		lResult = PDADefWindowProc(hWnd,uMsg,wParam,lParam);
		break;
	}
	return lResult;
}


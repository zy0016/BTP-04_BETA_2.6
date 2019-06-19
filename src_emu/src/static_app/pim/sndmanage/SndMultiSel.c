/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : PIM
 *
 * Purpose  : sound selection
 *            
\**************************************************************************/

#include "SndManage.h"

#define	MULTISELWC	"SNDMULTISELECTWNDC"
static	HWND	hSelMultiWnd = NULL;
static	HWND	hMultiList	=	NULL;
static	HWND	hFrame;
static	PLISTBUF	pMultiList;
static	DWORD	DOMSG;
static	char	szCaption[30] = "";
static	char	leftButton[30] = "";
static	BOOL	bContinue = FALSE;

//extern	HMENU	hSendSelSndMenu;
static	BOOL	RegisterMultiSelWndClass();
static	LRESULT	MultiSelProc(HWND hwnd, UINT umsg, WPARAM wapram, LPARAM lparam);
static BOOL SndIsSelected(int i);

static	BOOL	RegisterMultiSelWndClass()
{
	
	WNDCLASS  wc;
	
	wc.style            = NULL;
	wc.lpfnWndProc      = MultiSelProc;
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = 256;	//*******
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

BOOL SndMultiSel(HWND hFramewnd, PLISTBUF pList, DWORD wID)
{
	RECT	rf;
	RegisterMultiSelWndClass();
	switch(wID) {
	case IDC_DELSELECT:
		//strcpy(szCaption, IDS_DELETEMANY);
		//strcat(szCaption, "...");
		strcpy(leftButton, IDS_DELETE);
		break;

	case IDC_COPYSELECT:
		//strcpy(szCaption, IDS_SENDMANY);
		//strcat(szCaption, "...");
		strcpy(leftButton, IDS_COPY);
		break;

	case IDC_MOVESELECT:
		//strcpy(szCaption, IDS_MOVEMANY);
		//strcat(szCaption, "...");
		strcpy(leftButton, IDS_MOVE);
		break;
	
	}

	DOMSG = wID;
	hFrame = hFramewnd;
	pMultiList = pList;

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
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_LEFTICON), NULL);
	SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_RIGHTICON), NULL);
	SetWindowText(hFrame, IDS_SELSNDCAPTION);
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
	int		nIndex, iCount;
	char	cFileName[100];
	char	cFullName[200];
	struct	stat	filestat;
	float		tmpsize;
	char	PicSize[18];
	RECT	rClient;
	switch(umsg) 
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rClient);
		hMultiList = CreateWindow("MULTILISTBOX", NULL, WS_VISIBLE|WS_CHILD|LBS_BITMAP|LBS_MULTILINE, 
			rClient.left, rClient.top, rClient.right - rClient.left, rClient.bottom - rClient.top,
			hwnd, NULL, NULL, NULL);
		pNode = pMultiList->pDataHead;
		if (pNode)
		{
			do 
			{
				if(ISSOUNDFILE(pNode->nFVLine))
				{
					nIndex = SND_AddListBoxItem(hMultiList, pNode->szData);
					stat(pNode->szFullData, &filestat);

					tmpsize = (float)filestat.st_size/1024;
					floattoa(tmpsize, PicSize);
					
					strcat(PicSize, "kB");
					SendMessage(hMultiList, LB_SETAUXTEXT, MAKEWPARAM(nIndex, -1), (LPARAM)PicSize);
				}
				pNode = pNode->pNext;
			}while (pNode!= pMultiList->pDataHead &&pNode);
		}
		SendMessage(hMultiList, LB_ENDINIT, 0, 0);
		SendMessage(hMultiList,LB_SETCURSEL,0,0);
		
		break;


	case PWM_SHOWWINDOW:
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_LEFTICON), NULL);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SND_RIGHTICON), NULL);
		SetWindowText(hFrame, IDS_SELSNDCAPTION);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)leftButton);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_CANCEL);
		SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		PDASetMenu(hFrame, NULL);
		SetFocus(hSelMultiWnd);
		break;

	case WM_SETFOCUS:
		SND_SetListBoxFocus(hMultiList);
		break;

	case WM_KEYDOWN:
		switch(wparam) 
		{
		case VK_F10:
			PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;

		case VK_RETURN:
			{
				int	i,j, k;
				k=0;
				iCount = SendMessage(hMultiList, LB_GETCOUNT, 0, 0);
				j = SendMessage(hMultiList, LB_GETSELCOUNT, 0, 0);
				if (j<=0) {
					PLXTipsWin(hFrame, hwnd, 0, IDS_SELSND, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
					break;
				}
				switch(DOMSG) 
				{
				case IDC_DELSELECT:
					if (PLXConfirmWin(hFrame, hwnd, IDS_DELSELSND, Notify_Request, NULL, IDS_YES, IDS_NO)) 
					{
						WaitWindowStateEx(hFrame, TRUE, IDS_DELETING, NULL, NULL, NULL);
						for (i=0; i<iCount; i++)
						{
							if(SndIsSelected(i))
							{
								if(PREBROW_GetData(pMultiList, i-k, cFileName, cFullName))
								{
									if(0 == remove(cFullName))
									{
										SendMessage(hMultiList, LB_DELETESTRING, i-k, 0);
										PREBROW_DelData(pMultiList, i - k);	
										k++;
									}
						
									KickDog();	// clear the watch dog
								}
								
							}
						}
						if(0 < k)
						{
							RealtimeUpdateSound();
							PLXTipsWin(hFrame, hwnd, 0, IDS_DELETED, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
						}
						WaitWindowStateEx(hFrame, FALSE, IDS_DELETING, NULL, NULL, NULL);
						PostMessage(hwnd, WM_CLOSE, 0, 0);
					}
					break;
					
				case IDC_MOVESELECT:
					SndMoveToFolder(hwnd, IDRM_MOTOFOLDER);
					break;
					
				case IDC_COPYSELECT:
					SndMoveToFolder(hwnd, IDRM_CPTOFOLDER);
					break;
				}
				
				
			}
			
			break;
		}
		break;

	case IDRM_CPTOFOLDER:
		{
			int		i/*, k=0*/;
			char	NewFullName[MAXFULLNAMELEN];
			char	*name;
			BOOL	bSucc = TRUE;
			int		nLen = 255;

			if (lparam > 0)
			{
				strcpy(NewFullName, (PSTR)lparam);
				if (NewFullName[strlen(NewFullName) -1] != '/')
					strcat(NewFullName,"/");
				nLen = strlen(NewFullName);
			}
			else
				break;
			iCount = SendMessage(hMultiList, LB_GETCOUNT, 0, 0);

			WaitWindowStateEx(hFrame, TRUE, ML("Copying ..."), NULL, NULL, NULL);			
			for (i=0; i<iCount; i++)
			{
				if(SndIsSelected(i))
				{
					if(PREBROW_GetData(pMultiList, i, cFileName, cFullName))
					{
						NewFullName[nLen] = '\0';
						name = strrchr(cFullName, '/') + 1;
						strcat(NewFullName,name);
						
						if(0 == strcasecmp(cFullName, NewFullName))
						{
							PLXTipsWin(hFrame, hwnd, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							bSucc = FALSE;
							break;
						}
						else if(SND_IsItemExist(NewFullName, TRUE))
						{
							PLXTipsWin(hFrame, NULL, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							bSucc = FALSE;
							break;
						}
						else if(SND_GetFileSize(cFullName) + 500 > SND_GetFreeSpace(NewFullName))
						{
							PLXTipsWin(hFrame, NULL, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
							bSucc = FALSE;
							break;
						}
						else
							SND_CopyFile(cFullName, NewFullName);

						KickDog();	// clear the watch dog
					//	GetMessage(&msg, NULL, 0, 0))
					//	TranslateMessage(&msg);
					//	DispatchMessage(&msg);
					}
				}
			}
			NewFullName[nLen] = '\0';
			if(SearchSndData(pMultiList, NewFullName, PREBROW_FILETYPE_FOLDER) >= 0)
				RealtimeUpdateSound();
			if(bSucc)
				PLXTipsWin(hFrame, NULL, 0, ML("Copied"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			WaitWindowStateEx(hFrame, FALSE, ML("Copying ..."), NULL, NULL, NULL);

			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;

	case IDRM_MOTOFOLDER:
		{
			int		i;
			char	NewFullName[MAXFULLNAMELEN];
			char	*name;
			BOOL	bSucc = TRUE;
			int		nLen = 255;

			if (lparam > 0)
			{
				strcpy(NewFullName, (PSTR)lparam);
				if (NewFullName[strlen(NewFullName) -1] != '/')
					strcat(NewFullName,"/");
				nLen = strlen(NewFullName);
			}
			else
				break;
			iCount = SendMessage(hMultiList, LB_GETCOUNT, 0, 0);

			WaitWindowStateEx(hFrame, TRUE, ML("Moving ..."), NULL, NULL, NULL);
			for (i=iCount-1; i>=0; i--)
			{
				if(SndIsSelected(i))
				{
					if(PREBROW_GetData(pMultiList, i, cFileName, cFullName))
					{
						NewFullName[nLen] = '\0';
						name = strrchr(cFullName, '/') + 1;
						strcat(NewFullName,name);

						if(0 == strcasecmp(cFullName, NewFullName))
						{
							continue;
						}
						else if(!SND_IsMoveAvailable(cFullName, NewFullName))
						{
							PLXTipsWin(hFrame, NULL, 0, IDS_MEMORYFULL, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
						}
						else if(SND_IsItemExist(NewFullName, TRUE))
						{
							PLXTipsWin(hFrame, NULL, 0, IDS_DEFOTHERNAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
							bSucc = FALSE;
							break;
						}
						else if(/*0 == rename*/SND_MoveFile(cFullName, NewFullName))
						{
							SendMessage(hMultiList, LB_DELETESTRING, i, 0);
							PREBROW_DelData(pMultiList, i);
						}

						KickDog();	// clear the watch dog
					//	GetMessage(&msg, NULL, 0, 0))
					//	TranslateMessage(&msg);
					//	DispatchMessage(&msg);
					}
				}
			}
			if(bSucc)
			{
				RealtimeUpdateSound();
				PLXTipsWin(hFrame, NULL, 0, ML("Moved"), NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			}
			WaitWindowStateEx(hFrame, FALSE, ML("Moving ..."), NULL, NULL, NULL);
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		break;
	
	case WM_CLOSE:
		PostMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hwnd, 0);
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
static BOOL SndIsSelected(int i)
{
	if (SendMessage(hMultiList, LB_GETSEL, (WPARAM)i, 0)) 
		return TRUE;
	else
		return FALSE;
}

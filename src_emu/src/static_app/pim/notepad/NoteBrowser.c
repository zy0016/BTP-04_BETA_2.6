#include "pollex.h"
#include "window.h"
#include "stdio.h"
#include "string.h"

#include "plx_pdaex.h"
#include "pubapp.h"
#include "winpda.h"
#include "malloc.h"

#include "NoteBrowser.h"
BOOL NoteBrowserRegisterClass(void)
{
    WNDCLASS wc;
	
   	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(NOTEBROWSERWNDGLOBALDATA);
	wc.hbrBackground = NULL;
	wc.hCursor = NULL;
	wc.hIcon = NULL;
	wc.hInstance = NULL;
	wc.lpfnWndProc = NoteBrowserWinProc;
	wc.lpszClassName = "NoteBrowserWinClass";
	wc.lpszMenuName = NULL;
	wc.style = 0;

    if (!RegisterClass(&wc))
        return FALSE;
	
    return TRUE;
}
/*********************************************************************
* Function	DWORD BrowserNote(HWND hFrameWnd, HWND hReturnWnd, UINT uiReturnMsg, PCSTR szCaption)
* Purpose     
* Params	
* Return	
* Remarks	
**********************************************************************/	 
		 
DWORD BrowserNote(HWND hFrameWnd, HWND hReturnWnd, UINT uiReturnMsg, PSTR szCaption)
{
	
	RECT rClient;
	HWND hNoteBrowserWnd;
	NOTEBROWSERWNDGLOBALDATA NoteBrowserData;

	memset(&NoteBrowserData, 0, sizeof(NoteBrowserData));
	NoteBrowserData.hFrameWnd = hFrameWnd;
	NoteBrowserData.pTextNodeHead = NULL;
	NoteBrowserData.pTextNodeEnd = NULL;
	NoteBrowserData.pTextNodeCur = NULL;
	NoteBrowserData.hView = NULL;
	NoteBrowserData.hReturnWnd = hReturnWnd;
	NoteBrowserData.uiReturnMsg = uiReturnMsg;
	NoteBrowserData.szCaption = szCaption;

	
	GetClientRect(NoteBrowserData.hFrameWnd,&rClient);
	hNoteBrowserWnd = CreateWindow(
		"NoteBrowserWinClass",
		"",
		WS_VISIBLE | WS_CHILD,
		rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top,
		NoteBrowserData.hFrameWnd,
		NULL,
		NULL,
		(PVOID)&NoteBrowserData);	
	
	if(NULL == hNoteBrowserWnd)
		return (FALSE);

	ShowWindow(NoteBrowserData.hFrameWnd,SW_SHOW);
	UpdateWindow(NoteBrowserData.hFrameWnd);
	return (TRUE);
	
}

//////////////////////////////////////////////////////////////////////////////
// Function	NoteBrowserWinProc
// Purpose	
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK NoteBrowserWinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = TRUE;
	RECT rcClient;
	NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData;

	pNoteBrowserData = (NOTEBROWSERWNDGLOBALDATA *)GetUserData(hWnd);
	
	switch(msg)
	{
	case WM_CREATE:
		{
			HDC hdc;
			COLORREF Color;
			BOOL bTrans;
			
			hdc = GetDC(hWnd);		
			hNotePadBmp = CreateBitmapFromImageFile(hdc, ICON_NOTE, &Color, &bTrans);
			
		}
	
		{
			LPCREATESTRUCT lpCreateStruct;
			lpCreateStruct = (LPCREATESTRUCT)lParam;
			memcpy(pNoteBrowserData, lpCreateStruct->lpCreateParams, sizeof(NOTEBROWSERWNDGLOBALDATA));
		}

		GetClientRect(hWnd, &rcClient);
		
		pNoteBrowserData->hNoteList = CreateWindow("LISTBOX",
			"",
			WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_MULTILINE | LBS_BITMAP,
			rcClient.left, rcClient.top,
			rcClient.right - rcClient.left, rcClient.bottom - rcClient.top,
			hWnd,
			NULL,
			NULL,
			NULL);
		
		InitList(pNoteBrowserData);
		SetFocus(pNoteBrowserData->hNoteList);
		if (pNoteBrowserData->pTextNodeHead != NULL) 
		{
			SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
			SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		}
		else
		{
			SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		}
		
		SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);		
	
		if(pNoteBrowserData->szCaption)
			SetWindowText(pNoteBrowserData->hFrameWnd, pNoteBrowserData->szCaption);
		else
			SetWindowText(pNoteBrowserData->hFrameWnd , IDS_NOTEBROWSERTITLE);
		
	
		break;
		
	case PWM_SHOWWINDOW:
		{
			int iCurSel;
			PTEXTINFO pNoteInfo;
			
			if (pNoteBrowserData->hView)
			{
				SetFocus(pNoteBrowserData->hView);
				SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
				SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				
				iCurSel = SendMessage(pNoteBrowserData->hNoteList, LB_GETCURSEL, 0, 0);
				pNoteInfo = (PTEXTINFO)SendMessage(pNoteBrowserData->hNoteList, LB_GETITEMDATA, (WPARAM)iCurSel, 0);
				SetWindowText(pNoteBrowserData->hFrameWnd, ML(pNoteInfo->szTextName));
			}
			else
			{
				SetFocus(pNoteBrowserData->hNoteList);
				if (pNoteBrowserData->pTextNodeHead != NULL)
				{
					SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
					SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				}
				else
				{
					SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
					SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				}
				SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
			
			
				if(pNoteBrowserData->szCaption)
					SetWindowText(pNoteBrowserData->hFrameWnd, pNoteBrowserData->szCaption);
				else
					SetWindowText(pNoteBrowserData->hFrameWnd , IDS_NOTEBROWSERTITLE);
				
			}
		}
		break;
	case WM_PAINT:
		BeginPaint(hWnd, NULL);
		EndPaint(hWnd, NULL);
		break;
	
	case WM_KEYDOWN:
		switch(wParam)
		{	
		case VK_F10:
			if (pNoteBrowserData->hView)
			{
				DestroyWindow(pNoteBrowserData->hView);
				if (NULL != pContent) 
				{
					free(pContent);
					pContent = NULL;
				}	
				pNoteBrowserData->hView = NULL;
				SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OPEN);
				SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
				if(pNoteBrowserData->szCaption)
					SetWindowText(pNoteBrowserData->hFrameWnd, pNoteBrowserData->szCaption);
				else
					SetWindowText(pNoteBrowserData->hFrameWnd , IDS_NOTEBROWSERTITLE);	
			}
			else
				PostMessage(hWnd, WM_CLOSE, 0, 0);		
			
			break;
		case VK_F5:
			{
				int iCurSel;
				PTEXTINFO pNoteInfo;
				char cAccessoryName[TEXT_NAME_LENGTH + 10];
				
				if (pNoteBrowserData->hView)
				{
					break;
				}
				
				if(NULL == pNoteBrowserData->pTextNodeHead)
				{
					SendMessage(pNoteBrowserData->hReturnWnd, pNoteBrowserData->uiReturnMsg, 0, 0);
					break;
				}
				else
				{
					iCurSel = SendMessage(pNoteBrowserData->hNoteList, LB_GETCURSEL, 0, 0);
					pNoteInfo = (PTEXTINFO)SendMessage(pNoteBrowserData->hNoteList, LB_GETITEMDATA, (WPARAM)iCurSel, 0);
					
				
					sprintf(cAccessoryName, "%s.%s", pNoteInfo->szTextName, TEXT_FILE_SUFFIX);
					
					SendMessage(pNoteBrowserData->hReturnWnd, pNoteBrowserData->uiReturnMsg,
						(WPARAM)pNoteInfo->szFileName, (LPARAM)cAccessoryName);
				}			
				
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			
			}
			break;
		case VK_RETURN:
			{
				
				if (NULL == pNoteBrowserData->pTextNodeHead)
				{
					break;
				}
				else
				{
					pNoteBrowserData->hView = ViewNote(pNoteBrowserData, hWnd);
					SetFocus(pNoteBrowserData->hView);
				}

				
			}
			break;
		default:
			lRet =  PDADefWindowProc(hWnd, msg, wParam, lParam);
			break;
		}
		break;

	case WM_CLOSE:
		{			
			DestroyWindow(hWnd);		
		}
		break;
	case WM_DESTROY:
		DeleteObject(hNotePadBmp);
		ReleaseAllNodeInfo(pNoteBrowserData);
		SendMessage(pNoteBrowserData->hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		break;
	default:
		lRet = PDADefWindowProc(hWnd, msg, wParam, lParam);
		break;
	}
	return (lRet);
}

static int InitList(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData)
{
	PTEXTINFO pInfo;
	int res;
	res = GetAllNotesInfo(pNoteBrowserData);
	if( res== 0)
	{
		PLXTipsWin(pNoteBrowserData->hFrameWnd, pNoteBrowserData->hNoteList, 0, 
			ML("There is not notepad directory!"), ML("Note Browser"), Notify_Alert, ML("Ok"), 0, WAITTIMEOUT);
		return 0;
	}
	else if (res == -1)
	{
		SendMessage(pNoteBrowserData->hNoteList, LB_INSERTSTRING, 0, (LPARAM)ML("No note"));
		SendMessage(pNoteBrowserData->hNoteList, LB_SETCURSEL, 0, 0);
		return 1;
	}

	
	if(MoveToFirstNodeInfo(pNoteBrowserData))
	{
		do {	
			pInfo = GetCurNoteInfo(pNoteBrowserData->pTextNodeCur);
			if (pInfo->dwTextLength > TEXT_NAME_LENGTH - TEXT_SUFFIX_LENGTH -1)
				AddNoteToList(pNoteBrowserData->hNoteList, pInfo, 1);
			else 
				AddNoteToList(pNoteBrowserData->hNoteList, pInfo, 0);
		} while(MoveToNextNodeInfo(pNoteBrowserData));
	}

	return 1;
}

static int MoveToFirstNodeInfo(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData)
{
	pNoteBrowserData->pTextNodeCur = pNoteBrowserData->pTextNodeHead;
	
	if(NULL == pNoteBrowserData->pTextNodeCur)
		return 0;
	return 1;
}

static int MoveToNextNodeInfo(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData)
{
	
	if(NULL == pNoteBrowserData->pTextNodeCur)
		return 0;
	if(NULL == pNoteBrowserData->pTextNodeCur->pNextInfo)
		return 0;
	pNoteBrowserData->pTextNodeCur = pNoteBrowserData->pTextNodeCur->pNextInfo;
	return 1;
}


static PTEXTINFO GetCurNoteInfo(PTEXTINFONODE pNodeCur)
{
	return (&pNodeCur->TextInfo);
}

static int GetAllNotesInfo(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData)
{
	TEXTINFO TxtInfo;
	char szFilename[TEXT_FILENAME_LENGTH + 1];
//	char szName[TEXT_NAME_LENGTH];
	struct dirent *pdirent =NULL;
	DIR *pdir = NULL;
	struct stat buff;
	int res = -1;
	
	chdir(TEXT_PATH_FLASH);
	
	pdir = opendir(TEXT_PATH_FLASH);
	if(pdir == NULL)
	{
		res = 0;
		return res;
	}

	pdirent = readdir(pdir);
	if(pdirent == NULL)
	{
		res = 0;
		closedir(pdir);
		return res;
	}

	memset(&TxtInfo, 0, sizeof(TEXTINFO));

	do {
		strcpy(szFilename,TEXT_PATH_FLASH);		
		strcat(szFilename, pdirent->d_name);
		if(IsWantedFile(szFilename, TEXT_FILE_TYPE))
		{		
			res = 1;
			TxtInfo.dwTextLength = GetTextFileLength(szFilename);
			GetTextFileInfo(szFilename, &buff);
			TxtInfo.stTime = buff.st_mtime;
//			SeparateSuffixFromFileName(szFilename, szName, NULL);
			strcpy(TxtInfo.szFileName, szFilename);
			ReadText(&TxtInfo, TxtInfo.szTextName, TEXT_NAME_LENGTH);
			AddNodeInfo(&TxtInfo, pNoteBrowserData);
		}
		pdirent = readdir(pdir);
	}while(pdirent !=NULL );

	closedir(pdir);
	
	return res;
}

static int AddNodeInfo(PTEXTINFO pTxtInfo, NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData)
{
	PTEXTINFONODE pInfoNode;

	pInfoNode = (PTEXTINFONODE)malloc(sizeof(TEXTINFONODE));
	
	if (NULL == pInfoNode) 
		return 0;

	if(NULL == pInfoNode)
		return 0;

	memcpy(&pInfoNode->TextInfo, pTxtInfo, sizeof(TEXTINFO));

	if(NULL == pNoteBrowserData->pTextNodeHead)
	{	
		pNoteBrowserData->pTextNodeHead = pInfoNode;
		pNoteBrowserData->pTextNodeEnd = pInfoNode;
		pInfoNode->pPreInfo = NULL;
		pInfoNode->pNextInfo = NULL;
	}
	else
	{
		pInfoNode->pNextInfo = NULL;
		pInfoNode->pPreInfo = pNoteBrowserData->pTextNodeEnd;
		pNoteBrowserData->pTextNodeEnd->pNextInfo = pInfoNode;
		pNoteBrowserData->pTextNodeEnd = pInfoNode;
	}
	pNoteBrowserData->pTextNodeCur = pInfoNode;

	return 1;
}

static void AddNoteToList(HWND hWnd,PTEXTINFO pInfo,  BOOL MoreLong)
{
	char ItemName[32];
	SYSTEMTIME systemtime;
	int i, ItemNum;
	PTEXTINFO ptmpInfo;
	char ItemTime[30];
	int lenth;



	GetTextFileTime(pInfo->szFileName, &systemtime);
	if(MoreLong)
		sprintf(ItemName, TEXT_LIST_FROMAT2, pInfo->szTextName);
	else
		sprintf(ItemName, TEXT_LIST_FROMAT, pInfo->szTextName);
	
	GetTimeStr(systemtime, ItemTime);
	
	ItemNum = SendMessage(hWnd, LB_GETCOUNT, NULL, NULL);
	if (ItemNum == 0) 
	{
		SendMessage(hWnd, LB_INSERTSTRING, 0, (LPARAM)ItemName);
		lenth = SendMessage(hWnd, LB_GETTEXTLEN , 0, NULL);
		SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hNotePadBmp);
		SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(0, -1), (LPARAM)ItemTime);
		SendMessage(hWnd, LB_SETITEMDATA, 0, (LPARAM)pInfo);
		SendMessage(hWnd, LB_SETCURSEL, 0, 0);
		return;
	}
	for(i = 0; i < ItemNum; i++)
	{ 
		ptmpInfo = (PTEXTINFO)SendMessage(hWnd, LB_GETITEMDATA, i, NULL);
		if(pInfo->stTime >= ptmpInfo->stTime)
		{
			SendMessage(hWnd, LB_INSERTSTRING, i, (LPARAM)ItemName);
			SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, i), (LPARAM)hNotePadBmp);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(i, -1), (LPARAM)ItemTime);
			SendMessage(hWnd, LB_SETITEMDATA, i, (LPARAM)pInfo);
			SendMessage(hWnd, LB_SETCURSEL, i, 0);
			break;
		}
		if (i == ItemNum -1) 
		{
			SendMessage(hWnd, LB_INSERTSTRING, ItemNum, (LPARAM)ItemName);
			SendMessage(hWnd, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, ItemNum), (LPARAM)hNotePadBmp);
			SendMessage(hWnd, LB_SETAUXTEXT, MAKEWPARAM(ItemNum, -1), (LPARAM)ItemTime);
			SendMessage(hWnd, LB_SETITEMDATA, (WPARAM)ItemNum, (LPARAM)pInfo);
			SendMessage(hWnd, LB_SETCURSEL, ItemNum, 0);
			break;
		}
	}
	return;
}

static HWND ViewNote(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData, HWND hWnd)
{

	HWND hView = NULL;
	PTEXTINFO pNoteInfo;
	int iCurSel;

	
	iCurSel = SendMessage(pNoteBrowserData->hNoteList, LB_GETCURSEL, 0, 0);
	pNoteInfo = (PTEXTINFO)SendMessage(pNoteBrowserData->hNoteList, LB_GETITEMDATA, (WPARAM)iCurSel, 0);
	pContent = (char *)malloc((pNoteInfo->dwTextLength +1) * sizeof(char));
	if (NULL == pContent) 
	{
		return NULL;
	}
	ReadText(pNoteInfo, pContent, pNoteInfo->dwTextLength);
	hView = PlxTextView(pNoteBrowserData->hFrameWnd, hWnd, pContent , strlen(pContent), FALSE, NULL, NULL, 0);

	SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
	SendMessage(pNoteBrowserData->hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	if(pNoteBrowserData->szCaption)
		SetWindowText(pNoteBrowserData->hFrameWnd, pNoteBrowserData->szCaption);
	else
		SetWindowText(pNoteBrowserData->hFrameWnd , IDS_NOTEBROWSERTITLE);
			
	return hView;
}
//////////////////////////////////////////////////////////////////////////////
// Function	ReleaseAllTextInfo
// Purpose	release all information about files释放所有文本信息
// Params	
// Return	
// Remarks	
//////////////////////////////////////////////////////////////////////////////
static void ReleaseAllNodeInfo(NOTEBROWSERWNDGLOBALDATA *pNoteBrowserData)
{
	PTEXTINFONODE pTextInfo, pNextTextInfo;
	if(NULL == 	pNoteBrowserData->pTextNodeHead && NULL == pNoteBrowserData->pTextNodeEnd)
		return;
	
	pNextTextInfo = pNoteBrowserData->pTextNodeHead ;
	while (NULL != pNextTextInfo) 
	{
		pTextInfo = pNextTextInfo;
		pNextTextInfo = pTextInfo->pNextInfo;
		free(pTextInfo);
		pTextInfo = NULL;
	}

}

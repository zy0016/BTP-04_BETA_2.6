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
#include "plx_pdaex.h"
#include "pubapp.h"  
#include "imesys.h"
#include "unistd.h"
#include "sys\stat.h" 

#include "WIEBK.h"
#include "WIE.h"
#include "wUipub.h"
#include "func.h"


#define WIE_HSPACE      0
#define WIE_VSPACE      2
#define WIE_BKNAME_POSITION WIE_HSPACE + CX_FITIN, CY_FITIN, 176 - 2*CX_FITIN, 56 - CY_FITIN
#define WIE_BKURL_POSITION  WIE_HSPACE + CX_FITIN, 57, 176 - 2*CX_FITIN, 90

#define	WIE_BM_IDOFFSET		  0

extern BOOL APP_EditSMS(HWND hFrameWnd,const char* PSZRECIPIENT,const char* PSZCONTENT);
extern void APP_EditMMS(HWND hWndFrame, HWND hWndPre, UINT msg, UINT nType,
						PSTR  InsertFileName);
extern BOOL CreateMailEditWnd(HWND hParent, const char *PSZRECIPIENT, const char *PSZRECIPIENT2, 
							  const char *PSZTITLE, const char *PSZCONTENT, const char *PSZATTENMENT, 
							  const SYSTEMTIME *PTIME, DWORD handle, int folderid);

static MENUITEMTEMPLATE WMLBMMenu[] = 
{
    {MF_STRING, WIE_IDM_BM_GO,   "打开", NULL},
    {MF_STRING, WIE_IDM_BM_EDIT, "编辑", NULL},
    {MF_STRING, WIE_IDM_BM_DEL,  "删除", NULL},    
    {MF_END,    0,               NULL,   NULL}
};

static const MENUTEMPLATE WMLBMMENUTEMPLATE =
{
    0,
    WMLBMMenu
};
static HMENU hbmMenu;
static HMENU hSendMenu;
static HWND hwndIEBMViewClo;
static RECT rtBMClient;
static WIEBookMark wBookMark[WIE_BM_MAXNUM];
static HWND hRefList[10];
static BOOL bPushClose = FALSE;
int totalbm = 0;
HWND hwndIEBMEditClo;
HBITMAP hwmlnotactive;

extern int InitBMT;

typedef struct _WMLBM
{
	int EditId;
	HWND hEName;
	HWND hEUrl;
	HWND hList;
	char szBMTmpFile[URLNAMELEN];
	HWND hwndCurf;
	BOOL bNotNew;
	int nEditType;
	int nOverID;
	BOOL bEditName;
} WMLBMStruct, *PWMLBMStruct;

static LRESULT WMLIEBookMarkWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WMLIEBMEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static int BMListInit (WIEBookMark *dBookMark, HWND hList);
static int BMSetValue(PWMLBMStruct pSetting, WIEBookMark *wapBookMark, int index, int itid, char * sztmpname, char * sztmpurl);
static int WBK_OnCommand(HWND hParent, HWND hList, WPARAM wParam, LPARAM lParam);
static int GetAbsURL(char* szURL, char* szAbsURL, int iAbsLen);
static int SaveBookMark(HWND hWnd, WIEBookMark *sBookMark);
static void FreeAllBm(WIEBookMark *fBookMark);
static int LoadBookMark(WIEBookMark *rBookMark);


BOOL On_IDM_BM(HWND hParent, RECT rClient)
{
    WNDCLASS wc;
	HWND hwndIEBMView;

	rtBMClient.bottom = rClient.bottom;
	rtBMClient.left   = rClient.left;
	rtBMClient.right  = rClient.right;
	rtBMClient.top    = rClient.top;
    
	if(!GetClassInfo(NULL, "WMLIEBMViewClass", &wc))
	{
		wc.style         = 0;
		wc.lpfnWndProc   = WMLIEBookMarkWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "WMLIEBMViewClass";
		
		RegisterClass(&wc);


		hbmMenu = CreateMenu();
		hSendMenu = CreateMenu();
		InsertMenu(hbmMenu, 0, MF_BYPOSITION, IDM_WML_GOTO, WML_GOTO); 
		InsertMenu(hbmMenu, 1, MF_BYPOSITION, IDM_WML_EDIT, WML_EDIT);
		InsertMenu(hbmMenu, 2, MF_BYPOSITION, IDM_WML_DELETE, WML_DELETE);
		InsertMenu(hbmMenu, 3, MF_BYPOSITION|MF_POPUP, (DWORD)hSendMenu, WML_SEND);
		
		InsertMenu(hSendMenu, 0, MF_BYPOSITION, IDM_WML_SMS, WML_SMS);
		InsertMenu(hSendMenu, 1, MF_BYPOSITION, IDM_WML_MMS, WML_MMS);
		InsertMenu(hSendMenu, 2, MF_BYPOSITION, IDM_WML_MAIL, WML_MAIL);
		
		PDASetMenu(hParent, NULL);
		SetWindowText(hParent, (LPCTSTR)WML_BOOKMARKS);
		SendMessage(hParent, PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
		SendMessage(hParent, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
		
		hwndIEBMView = CreateWindow(
			"WMLIEBMViewClass", 
			WML_MENU_4,
			WS_VISIBLE|WS_CHILD,
			rtBMClient.left, rtBMClient.top, 
			rtBMClient.right-rtBMClient.left, 
			rtBMClient.bottom-rtBMClient.top,
			hParent, 
			NULL,
			NULL, 
			NULL
			);
		if (hwndIEBMView == NULL) 
		{
			UnregisterClass("WMLIEBMViewClass", NULL);
			return FALSE;
		}
		if(hParent == GetWAPFrameWindow())
			hwndIEBMViewClo = hwndIEBMView;
   	}

    ShowWindow(hwndIEBMView, SW_SHOW);            
    UpdateWindow(hwndIEBMView);
    
    return TRUE;
}

/*************************************************************************\
  书签窗口过程
  增加、编辑、修改、打开
\*************************************************************************/
#define WIEBK_LIST_X      0
#define WIEBK_LIST_Y      0
#define WIEBK_LIST_WIDTH  176
#define WIEBK_LIST_HEIGHT 150

#define WIEBK_EDIT_X      WIEBK_LIST_X
#define WIEBK_EDIT_Y      WIEBK_LIST_HEIGHT + 2
#define WIEBK_EDIT_WIDTH  WIEBK_LIST_WIDTH
#define WIEBK_EDIT_HEIGHT 108

static LRESULT WMLIEBookMarkWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    int nBmID, nListId; 
	HWND hBMlist;
	HWND hFrame;

	hFrame = GetParent(hWnd);
    
    switch (wMsgCmd)
    {
    case WM_CREATE:    
		{
			int i = 0;
			hBMlist = CreateWindow("LISTBOX","",
				WS_CHILD|WS_TABSTOP|WS_VISIBLE|WS_VSCROLL|LBS_BITMAP|LBS_MULTILINE,
				WIEBK_LIST_X,
				WIEBK_LIST_Y,
				WIEBK_LIST_WIDTH,
				WIEBK_LIST_HEIGHT,            
				hWnd,
				(HMENU)WIE_ID_BM_LIST, 
				NULL,NULL);                
			
			for(i = 0; i < 10; i++)
			{
				if(hRefList[i] == NULL)
				{
					hRefList[i] = hBMlist;
					break;
				}
			}
			SetFocus(hBMlist);
			hwmlnotactive = LoadImage(NULL, WIE_IMG_NOTACTIVE, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
			LoadBookMark(wBookMark);	
			BMListInit(wBookMark, hBMlist);  
			if(totalbm == WIE_BM_MAXNUM)
			{
				PDASetMenu(hFrame, hbmMenu);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
			}
			else
			{
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
			}
		}
        break; 
		
	case PWM_SHOWWINDOW:
		{
			HWND hList;
			
			hList = GetDlgItem(hWnd, WIE_ID_BM_LIST);
			SetWindowText(hFrame, (LPCTSTR)WML_BOOKMARKS);
			SetFocus(hList);
			if (totalbm != WIE_BM_MAXNUM)
			{
				int nListId;
				
				nListId = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
				if (nListId != LB_ERR)
				{				
					if (nListId == 0)
					{
						PDASetMenu(hFrame, NULL);
						SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
						SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
					}
					else
					{
						PDASetMenu(hFrame, hbmMenu);
						SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
						SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
					}
				}
			}
			else
			{
				PDASetMenu(hFrame, hbmMenu);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
			}
						
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);	
		}
		break;

	case WM_SETFOCUS:
		{
			HWND hList;

			hList = GetDlgItem(hWnd, WIE_ID_BM_LIST);
			SetFocus(hList);
		}
		break;
        
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_RETURN:
            {
				char LBtninfo[10] = {0};
				HWND hList;

				hList = GetDlgItem(hWnd, WIE_ID_BM_LIST);
				SendMessage(hFrame, PWM_GETBUTTONTEXT, 1, (LPARAM)LBtninfo);
				if (strcmp(LBtninfo, WML_GOTO) == 0)
				{
					nListId = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
					if (nListId != LB_ERR)
					{
	                    nBmID = SendMessage(hList, LB_GETITEMDATA, nListId, NULL);                    
		                if ((nBmID != LB_ERR) && (wBookMark[nBmID].szBMUrl != NULL))  
			                SendMessage(hWnd, WM_COMMAND, IDM_WML_GOTO, NULL);
					}
				}
            }
            break;

		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		case VK_F5:
			if (totalbm != WIE_BM_MAXNUM)
			{
				int nListId;
				HWND hList;

				hList = GetDlgItem(hWnd, WIE_ID_BM_LIST);
				nListId = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
				if (nListId != LB_ERR)
				{				
					if (nListId == 0)
					{						
						printf("\r\n@@@@@@@@@@@@@@@@@@@  enter new bk  \r\n");
						WIE_BM_Edit(-1, hFrame, hList, NULL, rtBMClient, FALSE);
					}
					else
					{
						return PDADefWindowProc(hFrame, wMsgCmd, wParam, lParam);
					}
				}
			}
			else
			{
				return PDADefWindowProc(hFrame, wMsgCmd, wParam, lParam);
			}
			break;

        default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);            
        }
		break;        
        
    case WM_SELOK:
		{
			HWND hList;
			
			hList = GetDlgItem(hWnd, WIE_ID_BM_LIST);
			nListId = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
			if (nListId != LB_ERR)
			{
				nBmID = SendMessage(hList, LB_GETITEMDATA, nListId, NULL);
				if ((nBmID != LB_ERR) && (wBookMark[nBmID].szBMUrl != NULL)) 
				{
					int i;
					char szprompt[100] = {'0'};
					HDC hdc;
					int titlelen, nLinechalen;

					memset(szprompt, 0, 100);
					titlelen = strlen(wBookMark[nBmID].szBMName);
					
					hdc = GetDC(hWnd);
					GetTextExtentExPoint(hdc, wBookMark[nBmID].szBMName, titlelen, 175, &nLinechalen, NULL, NULL);
					ReleaseDC(hWnd, hdc);
					
					if(nLinechalen < titlelen)
					{
						strncpy(szprompt, wBookMark[nBmID].szBMName, nLinechalen - 5);
						strcat(szprompt, "...\r\n");
					}
					else
					{
						strcpy(szprompt, wBookMark[nBmID].szBMName);
						strcat(szprompt, "\r\n");
					}
										
					totalbm--;

					wBookMark[nBmID].ibChange = FALSE;
					strcpy(wBookMark[nBmID].szBMName, "");
					free(wBookMark[nBmID].szBMUrl);
					wBookMark[nBmID].szBMUrl = NULL;
					wBookMark[nBmID].nUrlType = URL_NULL;

					for (i = nBmID; i < totalbm; i++)
					{
						if (strlen(wBookMark[i + 1].szBMName) > 0)
						{
							strcpy(wBookMark[i].szBMName, wBookMark[i+1].szBMName);
							wBookMark[i].szBMUrl = wBookMark[i+1].szBMUrl;
							wBookMark[i].ibChange = wBookMark[i+1].ibChange;
							wBookMark[i].nUrlType = wBookMark[i+1].nUrlType;
						}
						else
							break;						
					}
					strcpy(wBookMark[totalbm].szBMName, "");
					wBookMark[totalbm].szBMUrl = NULL;
					wBookMark[totalbm].ibChange = FALSE;
					wBookMark[totalbm].nUrlType = 0;

					strcat(szprompt, WML_DELETED);
					strcat(szprompt, "!");
					PLXTipsWin(NULL, NULL, NULL, szprompt, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
					
					SaveBookMark(hWnd, wBookMark);
					BMListInit(wBookMark, hList);	
				}
			}
		}
        break;

    case WM_COMMAND:
        {
			HWND hList;

			hList = GetDlgItem(hWnd, WIE_ID_BM_LIST);
			WBK_OnCommand(hWnd, hList, wParam, lParam);
		}
        break;

    case WM_ACTIVATE:
        {
			HWND hList;

			hList = GetDlgItem(hWnd, WIE_ID_BM_LIST);
			if (WA_ACTIVE == LOWORD(wParam))
				SetFocus(hList);
		}
        break;

	case WIE_IDM_BM_DELCONFIRM:
		if(lParam == 1)
			SendMessage (hWnd, WM_SELOK, 0, 0);
		else
			SendMessage (hWnd, WM_SELCANCEL, 0, 0);
		break;

    case WM_PAINT:            
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);            
        break;

    case WM_CLOSE:
		{
			int i;
			HWND hList;
		
			hList = GetDlgItem(hWnd, WIE_ID_BM_LIST);
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			for(i = 0; i < 10; i++)
			{
				if(hRefList[i] == hList)
				{
					hRefList[i] = NULL;
					break;
				}
			}
			DestroyMenu(hbmMenu);
			DeleteObject(hwmlnotactive);
			DestroyWindow(hWnd);
        }
        break;

    case WM_DESTROY:        
		if(!bPushClose)
			SaveBookMark(hWnd, wBookMark);
		UnregisterClass("WMLIEBMViewClass", NULL);
		DestroyMenu(hbmMenu);
		DestroyMenu(hSendMenu);
        break;

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
       }
       
	return lResult;       
}

static int WBK_OnCommand(HWND hParent, HWND hList, WPARAM wParam, LPARAM lParam)
{
    static int nBmID, nListId;
    WORD mID;
    WORD msgBox;    
	HWND hFrame;
    
    mID    = LOWORD(wParam);
    msgBox = HIWORD(wParam);
    
	hFrame = GetParent(hParent);

    switch (mID)
    {     
	case WIE_ID_BM_LIST:
        if (msgBox == LBN_SELCHANGE)
        {
            nListId = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
            if (nListId != LB_ERR)
            {				
				if (totalbm == WIE_BM_MAXNUM)
				{									
				}
				else
				{
					if (nListId == 0)
					{
						PDASetMenu(hFrame, NULL);
						SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
						SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
					}
					else
					{
						PDASetMenu(hFrame, hbmMenu);
						SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
						SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
					}
				}
            }
            else
                nBmID = LB_ERR;
            
        }
        else if (msgBox == LBN_DBLCLK)
        {
            SendMessage(hParent, WM_KEYDOWN, VK_RETURN, NULL);
        }
        break;

	case IDM_WML_EDIT:
        {
			RECT rtClient;
			
			nListId = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
			if (nListId !=LB_ERR)
			{
				nBmID = SendMessage(hList, LB_GETITEMDATA, nListId, NULL);
				if (nBmID != LB_ERR)
				{
					GetClientRect(hFrame, &rtClient);
					WIE_BM_Edit(nBmID, hFrame, hList, NULL, rtClient, TRUE);
				}
			}
		}
        break;
        
	case IDM_WML_DELETE:
        {
			HDC hdc;
			int nLinechalen;
			int namelen, urllen;
			
			nListId = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
			if (nListId !=LB_ERR)
			{
				nBmID = SendMessage(hList, LB_GETITEMDATA, nListId, NULL);
				if ((nBmID != LB_ERR) && (wBookMark[nBmID].szBMUrl != NULL)) 
				{
					char szprompt[100] = {'0'};
					
					hdc = GetDC(hParent);
					namelen = strlen(wBookMark[nBmID].szBMName);
					urllen = strlen(wBookMark[nBmID].szBMUrl);
					GetTextExtentExPoint(hdc, wBookMark[nBmID].szBMName, namelen, 175, &nLinechalen, NULL, NULL);
					
					if (nLinechalen < namelen)
					{
						strncpy(szprompt, wBookMark[nBmID].szBMName, nLinechalen - 5);
						strcat(szprompt, "...\r\n");
					}
					else
					{
						strcpy(szprompt, wBookMark[nBmID].szBMName);
						strcat(szprompt, "\r\n");
					}
					
					GetTextExtentExPoint(hdc, wBookMark[nBmID].szBMUrl, urllen, 175, &nLinechalen, NULL, NULL);
					ReleaseDC(hParent, hdc);
					if (nLinechalen < urllen)
					{
						strncat(szprompt, wBookMark[nBmID].szBMUrl, nLinechalen - 7);
						strcat(szprompt, "... :\r\n");
					}
					else
					{
						strcat(szprompt, wBookMark[nBmID].szBMUrl);
						strcat(szprompt, " :\r\n");
					}
					
					strcat(szprompt, WML_MENU_2);                
					strcat(szprompt, "?");
					
					PLXConfirmWinEx(NULL, hParent, szprompt, Notify_Request, (char *)NULL, 
						IDS_YES, IDS_NO, WIE_IDM_BM_DELCONFIRM);
				}
			}
		}
        break;
		
    case WIE_IDM_BM_CHGNAME:
		nListId = msgBox+1;
        nBmID = SendMessage(hList, LB_GETITEMDATA, nListId, NULL);
		if (nBmID != -1)
        {
			int ret;

			SendMessage(hList, LB_SETTEXT, nListId, (LPARAM)(LPSTR)wBookMark[msgBox].szBMName);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nListId), (LPARAM)hwmlnotactive);
			ret = SendMessage(hList, LB_SETAUXTEXT, (WPARAM)MAKEWPARAM(nListId, -1), (LPARAM)(LPSTR)wBookMark[msgBox].szBMUrl);
			SendMessage(hList, LB_SETITEMDATA, nListId, nBmID);		       
		}
		else
		{
			int nId;

			nId = SendMessage(hList, LB_ADDSTRING, nListId, (long)(LPSTR)(wBookMark[msgBox].szBMName));
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nListId), (LPARAM)hwmlnotactive);
			nId = SendMessage(hList, LB_SETAUXTEXT, (WPARAM)MAKEWPARAM(nListId, -1), (LPARAM)(LPSTR)wBookMark[msgBox].szBMUrl);
			SendMessage(hList, LB_SETITEMDATA, nListId, msgBox);     				
		}
        break;
        
    case WIE_IDM_BM_EXIT:
        PostMessage(hParent, WM_CLOSE, NULL, NULL);
        break;
        
	case IDM_WML_GOTO:
        {
			int tmplen;
			char *tmp;
			
			nListId = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
			if (nListId != LB_ERR)
			{
				nBmID = SendMessage(hList, LB_GETITEMDATA, nListId, NULL);            
				if ((nBmID != LB_ERR) && (wBookMark[nBmID].szBMUrl != NULL))  
				{
					char szAbsURL[URLNAMELEN];
					
					if (0 == GetAbsURL(wBookMark[nBmID].szBMUrl, szAbsURL, URLNAMELEN))
					{
						PLXTipsWin(NULL, NULL, NULL, WML_BK_ERRURL, NULL,
							Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
						return 1;
					}
					
					tmplen = MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szAbsURL, -1, NULL, 0);
					tmp = (char *)malloc(tmplen + 1);
					memset(tmp, 0, tmplen + 1);
					MultiByteToUTF8(CP_ACP, 0, (LPCSTR)szAbsURL, -1, tmp, tmplen);
					tmp[tmplen] = '\0';
					
					UI_Message(UM_URLGO, (unsigned int)tmp, wBookMark[nBmID].nUrlType);	
					PostMessage(hParent, WM_KEYDOWN, (WPARAM)VK_F10, NULL);
				}
				free(tmp);
			}
		}
        break;

	case IDM_WML_SMS:
		{
			int index;
			HWND hList;

			hList = GetDlgItem(hParent, WIE_ID_BM_LIST);
			index = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
			
			if(totalbm == WIE_BM_MAXNUM)
				APP_EditSMS(hFrame, NULL, wBookMark[index].szBMUrl);
			else
				APP_EditSMS(hFrame, NULL, wBookMark[index - 1].szBMUrl);
		}
		
		break;

	case IDM_WML_MMS:
		{
			int index;
			HWND hList;

			hList = GetDlgItem(hParent, WIE_ID_BM_LIST);
			index = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
			
			if(totalbm == WIE_BM_MAXNUM)
				APP_EditMMS(hFrame, hParent, NULL, 12,
				wBookMark[index].szBMUrl);
			else
				APP_EditMMS(hFrame, hParent, NULL, 12,
				wBookMark[index - 1].szBMUrl);
		}
		break;

	case IDM_WML_MAIL:
		{
			int index;
			SYSTEMTIME st;
			HWND hList;

			hList = GetDlgItem(hParent, WIE_ID_BM_LIST);		
			index = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
			GetLocalTime(&st);
			
			if(totalbm == WIE_BM_MAXNUM)
				CreateMailEditWnd(hFrame, NULL, NULL, 
				NULL, wBookMark[index].szBMUrl, NULL, 
				&st, -1, -1);
			else
				CreateMailEditWnd(hFrame, NULL, NULL, 
				NULL, wBookMark[index - 1].szBMUrl, NULL, 
				&st, -1, -1);
		}		
		break;
        
    case WIE_IDM_BM_CLOSE:
        PostMessage(hParent, WM_CLOSE, NULL, NULL);
        break;
        
    default:
        break;
    }

    return 1;    
}

static int EditBM(PWMLBMStruct pSetting, WIEBookMark *Book, int EditID, char *szEditUrl)
{
	int j;
	
	if(pSetting->nOverID != -1)
	{
		free(Book[pSetting->nOverID].szBMUrl);
		Book[pSetting->nOverID].szBMUrl = (char *)malloc(strlen(szEditUrl) + 1);
		memset(Book[pSetting->nOverID].szBMUrl, 0, strlen(szEditUrl) + 1);
		strcpy(Book[pSetting->nOverID].szBMUrl, szEditUrl);        
		Book[pSetting->nOverID].nUrlType = pSetting->nEditType;
		Book[pSetting->nOverID].ibChange = TRUE;
					
		if(EditID == -1)
			return pSetting->nOverID;
	}
	totalbm--;
	for(j = pSetting->EditId; j < totalbm; j++)
	{
		strcpy(Book[j].szBMName, Book[j + 1].szBMName);
		free(Book[j].szBMUrl);
		Book[j].szBMUrl = (char *)malloc(strlen(Book[j + 1].szBMUrl) + 1);
		memset(Book[j].szBMUrl, 0, strlen(Book[j + 1].szBMUrl) + 1);
		strcpy(Book[j].szBMUrl, Book[j + 1].szBMUrl);
		Book[j].ibChange = Book[j + 1].ibChange;
		Book[j].nUrlType = Book[j + 1].nUrlType;
	}
	strcpy(Book[totalbm].szBMName, "");
	free(Book[totalbm].szBMUrl);
	Book[totalbm].szBMUrl = NULL;
	Book[totalbm].ibChange = FALSE;
	Book[totalbm].nUrlType = 0;
				
	return pSetting->nOverID;
}

static int SortNewBM(PWMLBMStruct pSetting, WIEBookMark * Book, char *szEditName, char *szEditUrl)
{
	int i, j, SetId;
	
	totalbm++;
	if(pSetting->nOverID == -1)
	{
		for (i = WIE_BM_IDOFFSET; i < WIE_BM_MAXNUM; i++)
		{
			if(strcmp(Book[i].szBMName, szEditName) > 0)
			{
				int len = 0;
				for(j = totalbm - 1; j > i; j--)
				{
					strcpy(Book[j].szBMName, Book[j - 1].szBMName);
					if((Book[j - 1].szBMUrl != NULL) && ((len = strlen(Book[j - 1].szBMUrl)) != 0))
					{
						if(Book[j].szBMUrl != NULL)
						{
							free(Book[j].szBMUrl);
							Book[j].szBMUrl = NULL;
						}
						Book[j].szBMUrl = (char *)malloc(len + 1);
						memset(Book[j].szBMUrl, 0, len + 1);
					}
					strcpy(Book[j].szBMUrl, Book[j - 1].szBMUrl);
					Book[j].ibChange = Book[j - 1].ibChange;
					Book[j].nUrlType = Book[j - 1].nUrlType;
					pSetting->bNotNew = FALSE;
				}
				SetId = i;
				break;
			}
			else if(i == (totalbm - 1))
			{
				pSetting->bNotNew = FALSE;
				SetId = i;
				break;
			}
			else if(totalbm == 0)
			{
				SetId = i;
				pSetting->bNotNew = FALSE;
				break;
			}				
		}
	}
	else
		SetId = pSetting->nOverID;
	{
		free(Book[SetId].szBMUrl);
		Book[SetId].szBMUrl = NULL;
		Book[SetId].szBMUrl = (char *)malloc(strlen(szEditUrl) + 1);
		memset(Book[SetId].szBMUrl, 0, strlen(szEditUrl) + 1);
		strcpy(Book[SetId].szBMUrl, szEditUrl);        
		strcpy(Book[SetId].szBMName, szEditName);
		Book[SetId].nUrlType = pSetting->nEditType;
		Book[SetId].ibChange = TRUE;
	}
	return SetId;
}
static int BMSetValue(PWMLBMStruct pSetting, WIEBookMark *wapBookMark, int index, 
					  int itid, char * sztmpname, char * sztmpurl)
{    	
	int SetID; 
	int urllen;
		
	urllen = strlen(sztmpurl);
	if(index != 0)
	{
		if(strcmp(wapBookMark[itid].szBMName, sztmpname) == 0)
		{
			GetWindowText(pSetting->hEUrl, sztmpurl, urllen+1);
			free(wapBookMark[itid].szBMUrl);
			wapBookMark[itid].szBMUrl = (char *)malloc(urllen + 1);
			memset(wapBookMark[itid].szBMUrl, 0, urllen + 1);
			strcpy(wapBookMark[itid].szBMUrl, sztmpurl); 
			wapBookMark[itid].nUrlType = pSetting->nEditType;
			wapBookMark[itid].ibChange = TRUE;
			pSetting->bNotNew = TRUE;
			return itid;
		}
	}		
	
	if(pSetting->bNotNew)
	{
		if((SetID = EditBM(pSetting, wapBookMark, itid, sztmpurl)) != -1)
		{
			return SetID;
		}
	}
	SetID = SortNewBM(pSetting, wapBookMark, sztmpname, sztmpurl);
	return SetID; 
}

static int BMListInit(WIEBookMark *dBookMark, HWND hList)
{    
    int i, nId;    
	BOOL bnull = TRUE;
    
    SendMessage(hList, LB_RESETCONTENT, 0, 0);

	if (totalbm == WIE_BM_MAXNUM)
	{
		for (i = 0; i < WIE_BM_MAXNUM; i++)
		{
			if ((dBookMark[i].szBMName != NULL) && (dBookMark[i].szBMUrl != NULL)) 
			{
				if (
					(strlen(dBookMark[i].szBMName) != 0 ) ||
					(
					(dBookMark[i].szBMUrl!=NULL) &&
					(strlen(dBookMark[i].szBMUrl) != 0)
					)
					)
				{
					nId = SendMessage(hList, LB_ADDSTRING, NULL, (long)(LPSTR)(dBookMark[i].szBMName));
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nId), (LPARAM)hwmlnotactive);
					bnull = FALSE;
				}
				if(!bnull)
				{
					SendMessage(hList, LB_SETITEMDATA, nId, i); 
					SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(nId, -1), (LPARAM)dBookMark[i].szBMUrl);
				}
			}
		}  
	}
	else
	{	
		nId = SendMessage(hList, LB_ADDSTRING, NULL, (long)(LPSTR)WML_BK_NEWBOOKMARK);
		SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)hwmlnotactive);
		SendMessage(hList, LB_SETITEMDATA, nId, -1);

		for (i = 0; i < WIE_BM_MAXNUM; i++)
		{
			if ((dBookMark[i].szBMName != NULL) && (dBookMark[i].szBMUrl != NULL)) 
			{
				if (
					(strlen(dBookMark[i].szBMName) != 0 ) ||
					(
					(dBookMark[i].szBMUrl!=NULL) &&
					(strlen(dBookMark[i].szBMUrl) != 0)
					)
					)
				{
					nId = SendMessage(hList, LB_ADDSTRING, NULL, (long)(LPSTR)(dBookMark[i].szBMName));
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nId), (LPARAM)hwmlnotactive);
					bnull = FALSE;
				}
				if(!bnull)
				{
					SendMessage(hList, LB_SETITEMDATA, nId, i);
					SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(nId, -1), (LPARAM)dBookMark[i].szBMUrl); 
				}
			}
		}  
	}

    SendMessage(hList, LB_SETCURSEL, 0, NULL);
	i = SendMessage(hList, LB_GETITEMDATA, 0, NULL);
	
	InitBMT = totalbm;
    return i;    
}

static int LoadBookMark(WIEBookMark *rBookMark)
{
    int nRetFileSize = 0,nRetName = 0, nRetLen = 0, nRetUrl = 0, nRetType = 0;
    int i = 0, nRead = -1, urllen = 0;    
    FILE* fIe;
    int flag = 1;
    char *sztmpurl;
	struct stat st;
	char tmd[50];
	BOOL kkk = FALSE;
    
    i = WIE_BM_IDOFFSET;
    
    fIe = fopen(WIE_BM_FILENAME, "rb+");    
    if (fIe == NULL)
    {        
		strcpy(tmd, WIE_BM_FILENAME);
        fIe = fopen(WIE_BM_FILENAME, "wb+");
        if (fIe == NULL)
            flag=-1;
		else
			kkk = TRUE;
    }
    
    if (flag != -1)
    {
        stat(WIE_BM_FILENAME, &st);
        if (-1 == (nRetFileSize = st.st_size))
        {
            fclose(fIe);
            flag = -1;
        }
        else
        {
            for (i = WIE_BM_IDOFFSET; i < WIE_BM_MAXNUM; i++)
            {
                rBookMark[i].ibChange = FALSE;
                rBookMark[i].szBMUrl = NULL;
                rBookMark[i].szBMName[0] = '\0';
                nRetType = fread(&(rBookMark[i].nUrlType), 1, sizeof(int), fIe);
                nRetName = fread(rBookMark[i].szBMName, 1, WIE_BM_NAMELEN, fIe);
                if (nRetName == 0 || nRetType == 0)
                {
                    flag = -1;
                    break;
                }
                
                nRetLen = fread(&urllen, 1, sizeof(int), fIe);
                if (nRetName == 0 || nRetLen == 0)
                {
                    flag = -1;
                    break;
                }
                
                if (urllen != 0)
                {
                    sztmpurl = malloc(urllen + 1);
					memset(sztmpurl, 0, urllen + 1);
                    if (sztmpurl == NULL)
                    {
                        flag = -1;
                        break;
                    }
					else
                    {                        
                        nRetUrl = fread(sztmpurl, 1, urllen, fIe);
                        if (nRetUrl < urllen)
                        {                            
                            free(sztmpurl);
                            flag = -1;
                            break;
                        }//end if                        
                    }//end else
                    rBookMark[i].szBMUrl = sztmpurl;
                }//end if                
            }//end for
            nRead = i;
            fclose(fIe);
        }//end else        
    }//end if	

	if(kkk)
		fclose(fIe);
	
	if (i != 0)
		totalbm = nRead;
	else
		totalbm = 0;
    
    if (i < WIE_BM_MAXNUM)
    {
        for (i; i < WIE_BM_MAXNUM; i++)
        {
            rBookMark[i].ibChange = FALSE;
            strcpy(rBookMark[i].szBMName, "");
            rBookMark[i].szBMUrl = NULL;
            rBookMark[i].nUrlType = URL_NULL;
        }
    } 	

    return nRead;
}

static void FreeAllBm(WIEBookMark *fBookMark)
{
    int i;
    
    for (i = 0; i <WIE_BM_MAXNUM; i++ )
    {
        if (NULL != fBookMark[i].szBMUrl)    
        {
            free(fBookMark[i].szBMUrl); 
            fBookMark[i].szBMUrl = NULL;
        }
    }
}

static int SaveBookMark(HWND hWnd, WIEBookMark *sBookMark)
{
	int nRetFileSize = 0;
    int i, urllen, nFileLen = 0;
    FILE* fIe;    
    char szCurPath[_MAX_PATH];
    char *szWbuf, *szWtmp;
    
    getcwd(szCurPath, _MAX_PATH);
    chdir(FLASHPATH);
    
    if ((fIe = fopen(WIE_BM_FILENAME, "wb")) == NULL)
    {
		chdir(szCurPath);
		return -1;     
    }    
    for (i = 0; i < WIE_BM_MAXNUM; i++)
    {
		if ((sBookMark[i].szBMName != NULL) && (sBookMark[i].szBMUrl != NULL)) 
		{			
			nFileLen += (2*sizeof(int) +WIE_BM_NAMELEN);
			if (sBookMark[i].szBMUrl)
				nFileLen += (strlen(sBookMark[i].szBMUrl) + 1);        
		}
    }//end for 
    
    szWbuf = malloc(nFileLen + 1);
	memset(szWbuf, 0, nFileLen + 1);
    if (!szWbuf)
    {
		char prompt[100];
		
		strcpy(prompt, WML_SAVEFAILED);
		strcat(prompt, "\r\n");
		strcat(prompt, WML_ROMFULL);
        PLXTipsWin(NULL, NULL, NULL, prompt, NULL,
			Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
        nRetFileSize = -1;
    }
    else
    {        
        szWtmp = szWbuf;
        for (i = 0; i < WIE_BM_MAXNUM; i++)
        {    
			if ((sBookMark[i].szBMName != NULL) && (sBookMark[i].szBMUrl != NULL)) 
			{
				memcpy(szWtmp, &sBookMark[i].nUrlType, sizeof(int));
				szWtmp += sizeof(int);
				memcpy(szWtmp, sBookMark[i].szBMName, WIE_BM_NAMELEN);
				szWtmp += WIE_BM_NAMELEN;
            
				urllen = 0;
				if (sBookMark[i].szBMUrl)        
					urllen = strlen(sBookMark[i].szBMUrl) + 1;
				memcpy(szWtmp, &urllen, sizeof(int));
				szWtmp += sizeof(int);
            
				if (urllen != 0)
				{
					memcpy(szWtmp, sBookMark[i].szBMUrl, urllen);                
					szWtmp += urllen;
				}            
			}
        }//end for 
        
        nRetFileSize = fwrite(szWbuf, 1, nFileLen, fIe);
        free(szWbuf);
        if (nRetFileSize < nFileLen)        
            nRetFileSize = -1;
    }
    
    if (nRetFileSize == -1)
    {
        fclose(fIe);
        chdir(szCurPath);
        return -1;
    }    
    fclose(fIe);
    chdir(szCurPath);
    
    return 1;  
}



void WBK_Close_Window()
{
	bPushClose = TRUE;
    PostMessage(hwndIEBMEditClo, WM_CLOSE, NULL, NULL);
    PostMessage(hwndIEBMViewClo, WM_CLOSE, NULL, NULL);
}

int WBM_ADD_FROMURL(char *szinput, HWND hFrame)
{    
	RECT rbmClient;
	char input[URLNAMELEN];
    
	memset(input, 0, URLNAMELEN);
    LoadBookMark(wBookMark);
	if(totalbm == WIE_BM_MAXNUM)
	{
		char prompt[60];
		
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
			(LPARAM)NULL);
		SendMessage(hFrame, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
			(LPARAM)NULL);
		strcpy(prompt, WML_SAVEFAILED);
		strcat(prompt, "\n");
		strcat(prompt, WML_BMFULL);
		PLXTipsWin(NULL, NULL, NULL, prompt, NULL, 
			Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);

		return -1;
	}
    
	GetClientRect(hFrame, &rbmClient);
	if(szinput != NULL)
	{
		if(strlen(szinput) < URLNAMELEN)
			strcpy(input, szinput);
		else
			strncpy(input, szinput, URLNAMELEN - 1);
	}
	
    WIE_BM_Edit(-1, hFrame, NULL, input, rbmClient, FALSE);

    return 1;
}
/*****************************************************************
 * 函数介绍：判断URL是否包含"http://"。否则就加上"http://"
 * 传入参数：szURL；源串
 * 返回    ：转换后的URL
*****************************************************************/
static int GetAbsURL(char* szURL, char* szAbsURL, int iAbsLen)
{
#define HTTPHEAD "HTTP://"
#define HTTPSHEAD "HTTPS://"
#define FILEHEAD "FILE://"

	if (iAbsLen < (int)strlen(szURL))
		return 0;
	strcpy(szAbsURL, szURL);
	strupr(szAbsURL);

	if (NULL != strstr(szAbsURL, HTTPHEAD) 
        || NULL != strstr(szAbsURL, HTTPSHEAD) 
        || NULL != strstr(szAbsURL, FILEHEAD))
	{
		strcpy(szAbsURL, szURL);
		return 1;
	}

	if (iAbsLen < (int)(strlen(szURL)+strlen(HTTPHEAD)))
		return 0;

	strcpy(szAbsURL, HTTPHEAD);
	strcat(szAbsURL, szURL);

	return 1;
}

int WIE_LoadBookMark()
{
	return LoadBookMark(wBookMark);
}

void WIE_FreeAllBm()
{
	FreeAllBm(wBookMark);
}

int WIE_SaveBookMark(HWND hWnd)
{
	return SaveBookMark(hWnd, wBookMark);
}




static BOOL WmlIEBM_OnCreate(HWND hWnd, PWMLBMStruct pBMCreate)
{
	IMEEDIT	InputName, InputUrl;
	
	memset(&InputName, 0, sizeof(IMEEDIT));
	InputName.hwndNotify   = (HWND)hWnd;
	InputName.dwAscTextMax = 0;
	InputName.dwAttrib     = 0;
	InputName.dwUniTextMax = 0;
	InputName.pszCharSet   = NULL;
	InputName.pszImeName   = NULL;
	InputName.pszTitle     = NULL;
	InputName.uMsgSetText  = 0;
	InputName.wPageMax     = 0;

	memset(&InputUrl, 0, sizeof(IMEEDIT));
	InputUrl.hwndNotify   = (HWND)hWnd;
	InputUrl.dwAscTextMax = 0;
	InputUrl.dwAttrib     = 0;
	InputUrl.dwUniTextMax = 0;
	InputUrl.pszCharSet   = NULL;
	InputUrl.pszImeName   = NULL;
	InputUrl.pszTitle     = NULL;
	InputUrl.uMsgSetText  = 0;
	InputUrl.wPageMax     = 0;
	
    SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
	SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
    SendMessage(hWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_WAPWINDOW_SAVE);
        
    pBMCreate->hEName = CreateWindow("IMEEDIT", "",
        WS_CHILD|WS_TABSTOP|WS_VISIBLE|ES_AUTOHSCROLL|ES_TITLE,
        WIE_BKNAME_POSITION,
        hWnd, (HMENU)WIE_IDM_BM_EDITNAME, NULL, (PVOID)&InputName);
    
    SendMessage(pBMCreate->hEName, EM_LIMITTEXT, WIE_BM_NAMELEN -1, NULL);
	SendMessage(pBMCreate->hEName, EM_SETTITLE, 0, (LPARAM)WML_BK_NAME);  


    pBMCreate->hEUrl= CreateWindow("IMEEDIT", "",
        WS_CHILD|WS_TABSTOP|WS_VISIBLE|ES_MULTILINE|WS_VSCROLL|ES_TITLE,
        WIE_BKURL_POSITION,
        hWnd, (HMENU)WIE_IDM_BM_EDITPAGE, NULL, (PVOID)&InputUrl);
    
    SendMessage(pBMCreate->hEUrl, EM_LIMITTEXT, URLNAMELEN -1, NULL);
	SendMessage(pBMCreate->hEUrl, EM_SETTITLE, 0, (LPARAM)WML_URLADD);
	    
    pBMCreate->hwndCurf = pBMCreate->hEName;        
    
    if (pBMCreate->EditId >= -1  && pBMCreate->EditId < WIE_BM_MAXNUM)
    {
		if (strlen(pBMCreate->szBMTmpFile) > 0)
		{
			char *szMulti = NULL;
			int nLenMulti = 0;
			
			nLenMulti = UTF8ToMultiByte(CP_ACP, 0, pBMCreate->szBMTmpFile, -1, NULL, 0, NULL, NULL);
			szMulti = (char *)malloc(nLenMulti + 1);
			memset(szMulti, 0, nLenMulti + 1);
			nLenMulti = UTF8ToMultiByte(CP_ACP, 0, pBMCreate->szBMTmpFile, -1, szMulti, nLenMulti, NULL, NULL);
			szMulti[nLenMulti] = '\0';
			SetWindowText(pBMCreate->hEUrl, szMulti);
			free(szMulti);
		}
		else
		{
			if(pBMCreate->EditId != -1)
			{
				SetWindowText(pBMCreate->hEName, wBookMark[pBMCreate->EditId].szBMName);
				SetWindowText(pBMCreate->hEUrl, wBookMark[pBMCreate->EditId].szBMUrl);
			}
		}
    }
	
	SetFocus(pBMCreate->hwndCurf);

    return TRUE;
}

static void WmlIEBM_OnCommand(HWND hParent, HWND hWnd, PWMLBMStruct pStruct, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    WORD mID;
    WORD msgBox;     
    
    mID    = LOWORD(wParam);
    msgBox = HIWORD(wParam);
    
    switch (mID)
    {        
    case WIE_IDM_BM_EDITNAME:        
        if (msgBox == EN_CHANGE)
		{
			char tmptitle[42];
			
			GetWindowText(pStruct->hEName, tmptitle, 41);
				SetWindowText(hParent, tmptitle);

			pStruct->bEditName = TRUE;
		}
        break;            
                
    case WIE_IDM_BM_JUDGETOSAVE:    
		{
			int namelen, urllen;
			char sztmpname[WIE_BM_NAMELEN], *sztmpurl;
			int nOverID = -1;
			int index, itid;
			int i = 0;
			int nSetRet;
			BOOL bOver = FALSE;
			BOOL bFailed = FALSE;
			
			if(totalbm == WIE_BM_MAXNUM && !pStruct->bNotNew)
			{
				char prompt[100];
				
				strcpy(prompt, WML_SAVEFAILED);
				strcat(prompt, "\r\n");
				strcat(prompt, WML_BMFULL);
				PLXTipsWin(NULL, NULL, NULL, prompt, NULL, 
					Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
				PostMessage(hWnd, WIE_IDM_BM_NOTSAVE, 0, 0);
				break;
			}
			
			namelen = GetWindowTextLength(pStruct->hEName);
			urllen = GetWindowTextLength(pStruct->hEUrl);
			if (namelen == 0 || urllen == 0)
			{
				if (namelen == 0 && urllen == 0)
					PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFALLDATA, NULL,
					Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
				else if (namelen == 0)
					PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFTITLE, NULL,
					Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
				else if (urllen == 0)
					PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFURL, NULL,
					Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
				PostMessage(hWnd, WIE_IDM_BM_NOTSAVE, 0, 0);
				break;
			}
			else
			{
				if(pStruct->hList != NULL)
				{
					index = SendMessage(pStruct->hList, LB_GETCURSEL, NULL, NULL);
					if(index != 0)
						itid = SendMessage(pStruct->hList, LB_GETITEMDATA, index, NULL);
					else
						itid = -1;
				}
				else
				{
					index = 0;
					itid = -1;
				}
				
				GetWindowText(pStruct->hEName, sztmpname, WIE_BM_NAMELEN);  
				if (!WML_DeleteSpace(sztmpname))
				{
					PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFTITLE,NULL,
						Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
					PostMessage(hWnd, WIE_IDM_BM_NOTSAVE, 0, 0);
					break;	
				}
				
				sztmpurl = malloc(urllen+1);
				memset(sztmpurl, 0, urllen + 1);
				GetWindowText(pStruct->hEUrl, sztmpurl, urllen + 1);
				JudgetoStandardUrl(NULL, sztmpurl, &pStruct->nEditType);
				
				if(totalbm != 0)
				{
					for (i = WIE_BM_IDOFFSET; i < totalbm; i++)
					{
						if ((strcmp(wBookMark[i].szBMName, sztmpname) == 0) && i != itid)
						{
							PLXConfirmWinEx(NULL, hWnd, WML_BMNAMEUSED, Notify_Request, (char *)NULL, 
								IDS_YES, IDS_NO, WIE_IDM_BM_OVERWRITE);
							pStruct->nOverID = i;
							bOver = TRUE;
							break;
						}
						else
							nOverID = -1;
					}
				}
				else
					nOverID = -1;

				if(!bOver)
				{
					pStruct->nOverID = nOverID;
					
					nSetRet = BMSetValue(pStruct, wBookMark, index, itid, sztmpname, sztmpurl);
					SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(WIE_IDM_BME_SAVE, 0), (LPARAM)nSetRet);
				}
				free(sztmpurl);
			}
		}
		break;

	case WIE_IDM_BME_SAVE:
		{
			BOOL bSucc = FALSE;
			char promtp[WIE_BM_NAMELEN], tmp[WIE_BM_NAMELEN + 30];
			int i = 0;
			BOOL bhaveone = FALSE;
			int nRet;
			int titlelen, nLinechalen;
			HDC hdc;
			
			nRet = (int)lParam;
			pStruct->EditId = nRet;
			InitBMT = totalbm;			
			if (nRet > -1) 
			{                        
				if(SaveBookMark(hWnd, wBookMark) == 1)
					bSucc = TRUE;							
				GetWindowText(pStruct->hEName, promtp, WIE_BM_NAMELEN);
				WML_DeleteSpace(promtp);
				memset(tmp, 0, WIE_BM_NAMELEN + 30);

				titlelen = strlen(promtp);
				
				hdc = GetDC(hWnd);
				GetTextExtentExPoint(hdc, promtp, titlelen, 175, &nLinechalen, NULL, NULL);
				ReleaseDC(hWnd, hdc);
				
				if(nLinechalen < titlelen)
				{
					strncpy(tmp, promtp, nLinechalen - 5);
					strcat(tmp, "...\r\n");
					strcat(tmp, WML_BMSAVESUCCESS);
				}
				else
				{
					strcpy(tmp, promtp);
					strcat(tmp, "\r\n");
					strcat(tmp, WML_BMSAVESUCCESS);
				}
				if(bSucc)
					PLXTipsWin(NULL, NULL, NULL, tmp, 
					NULL, Notify_Success, IDS_OK, "", WAITTIMEOUT);
			}
			
			for(i = 0; i < 10; i++)
			{
				if(hRefList[i] != NULL)
					BMListInit(wBookMark, hRefList[i]);
			}
			if(pStruct->EditId + 1 > totalbm)
				SendMessage(pStruct->hList, LB_SETCURSEL, 0, 0);
			else
			{
				if(totalbm == WIE_BM_MAXNUM)
					SendMessage(pStruct->hList, LB_SETCURSEL, pStruct->EditId, NULL);
				else
					SendMessage(pStruct->hList, LB_SETCURSEL, pStruct->EditId + 1, NULL);
			}
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			pStruct->bNotNew = FALSE;
		}
        break;              
        
    case WIE_IDM_BME_CLOSE:
 		pStruct->bNotNew = FALSE;
        Wml_ONCancelExit(hWnd);
        break;

    default:
		PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
}


int WIE_BM_Edit(int id, HWND hParent, HWND hList, char * url, RECT rClient, BOOL bEditorNot)
{
    WNDCLASS wc;
	HWND hwndIEBMEdit;
	WMLBMStruct pStruct;

	if(!GetClassInfo(NULL, "WMLIEBMEditClass", &wc))
    {
		wc.style         = 0;
		wc.lpfnWndProc   = WMLIEBMEditWndProc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = sizeof(WMLBMStruct);
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "WMLIEBMEditClass";
		
		RegisterClass(&wc);
	}
    
	pStruct.EditId = id;
	pStruct.bNotNew = bEditorNot;
	pStruct.hList = hList;
	if (url != NULL)
		strcpy(pStruct.szBMTmpFile, url);
	else
		strcpy(pStruct.szBMTmpFile, "");

    hwndIEBMEdit = CreateWindow(
        "WMLIEBMEditClass", 
        WML_MENU_1,
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        (PVOID)&pStruct
        );
    if (hwndIEBMEdit == NULL) 
    {
        return -1;
    }
	if(hwndIEBMEdit == GetWAPFrameWindow())
		hwndIEBMEditClo = hwndIEBMEdit;
	PostMessage(hwndIEBMEdit, WIE_SETCURSOR, NULL, NULL);
    ShowWindow(hwndIEBMEdit, SW_SHOW);            
    UpdateWindow(hwndIEBMEdit);
    
    return 1;
}
extern HWND hProcess;
extern BOOL g_bDestroying;

static LRESULT WMLIEBMEditWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)      
{
    LRESULT lResult = 0;
    HDC hdc;
	HWND hParent;
	PWMLBMStruct pBMStruct;
    
	hParent = GetParent(hWnd);

	pBMStruct = GetUserData(hWnd);

    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			PCREATESTRUCT pCreate;
			
			pCreate = (PCREATESTRUCT)lParam;
			memcpy(pBMStruct, pCreate->lpCreateParams, sizeof(WMLBMStruct));
			SendMessage(hParent, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, RIGHTICON), 
				(LPARAM)NULL);
			SendMessage(hParent, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, LEFTICON), 
				(LPARAM)NULL);
			lResult = (LRESULT)WmlIEBM_OnCreate(hWnd, pBMStruct);
			if(pBMStruct->hList != NULL)
			{
				if(pBMStruct->EditId == -1)
				{
					SetWindowText(hParent, (LPCTSTR)WML_NEWBOOKMARK);
					pBMStruct->bEditName = FALSE;
				}
				else
				{
					if (strlen(wBookMark[pBMStruct->EditId].szBMName) == 0)
						SetWindowText(hParent, (LPCTSTR)WML_NEWBOOKMARK);
					else
						SetWindowText(hParent, (LPCTSTR)wBookMark[pBMStruct->EditId].szBMName);
				}
			}
			else
			{
				char *szMulti = NULL;
				int nLenMulti = 0;
				
				if(pBMStruct->EditId == -1)
				{
					SetWindowText(hParent, (LPCTSTR)WML_NEWBOOKMARK);
					pBMStruct->bEditName = FALSE;
				}
				else
				{
					if (strlen(wBookMark[pBMStruct->EditId].szBMName) == 0)
						SetWindowText(hParent, (LPCTSTR)WML_NEWBOOKMARK);
					else
						SetWindowText(hParent, (LPCTSTR)wBookMark[pBMStruct->EditId].szBMName);
				}
				
			}
			SendMessage(hParent, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
			SendMessage(hParent, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SendMessage(hParent, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_WAPWINDOW_SAVE);
		}
        break;

	case PWM_SHOWWINDOW:
		PDASetMenu(hParent, NULL);
		if(GetWindowTextLength(pBMStruct->hEName) == 0)
		{
			if(pBMStruct->bEditName)
				SetWindowText(hParent, (LPCTSTR)"");
			else
				SetWindowText(hParent, (LPCTSTR)WML_NEWBOOKMARK);
		}
		else
		{
			char szShowtext[41];
			
			GetWindowText(pBMStruct->hEName, szShowtext, 41);
			SetWindowText(hParent, (LPCTSTR)szShowtext);
		}

		SendMessage(hParent, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
		SendMessage(hParent, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(hParent, PWM_SETBUTTONTEXT, 1, (LPARAM)STR_WAPWINDOW_SAVE);
		break;
	
	case WIE_SETCURSOR:
		SendMessage(pBMStruct->hEName, EM_SETSEL, -1, -1);
		SendMessage(pBMStruct->hEUrl, EM_SETSEL, -1, -1);
		break;

	case WM_SETRBTNTEXT:
		if (strcmp((char *)lParam, (LPCSTR)STR_WAPWINDOW_CANCEL) == 0)
			SendMessage(hParent, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
        else
		{
			if((hWnd == GetFocus()) || (hWnd == GetParent(GetFocus())))
				SendMessage(hParent, PWM_SETBUTTONTEXT, 0, lParam);
        }
		break;
      
	case WIE_IDM_BM_OVERWRITE:
		{
			int nSaveRet;
			char sztmpname[WIE_BM_NAMELEN], *sztmpurl;
			int index, itid;
			int urllen;
			
			if(lParam != 1)
				SetFocus(pBMStruct->hEName);
			else
			{
				if(pBMStruct->hList != NULL)
				{
					index = SendMessage(pBMStruct->hList, LB_GETCURSEL, NULL, NULL);
					if(index != 0)
						itid = SendMessage(pBMStruct->hList, LB_GETITEMDATA, index, NULL);
					else
						itid = -1;
				}
				else
				{
					index = 0;
					itid = -1;
				}
				
				GetWindowText(pBMStruct->hEName, sztmpname, WIE_BM_NAMELEN);  
				urllen = GetWindowTextLength(pBMStruct->hEUrl);
				sztmpurl = malloc(urllen+1);
				memset(sztmpurl, 0, urllen + 1);
				GetWindowText(pBMStruct->hEUrl, sztmpurl, urllen + 1);
				JudgetoStandardUrl(NULL, sztmpurl, &pBMStruct->nEditType);

				pBMStruct->bNotNew = TRUE;
				nSaveRet = BMSetValue(pBMStruct, wBookMark, index, itid, sztmpname, sztmpurl);
				SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(WIE_IDM_BME_SAVE, 0), (LPARAM)nSaveRet);
				free(sztmpurl);
			}
		}
		break;
		
	case WIE_IDM_BM_NOTSAVE:
		if (GetWindowTextLength(pBMStruct->hEName) == 0) 
			pBMStruct->hwndCurf = pBMStruct->hEName;
		else
		{
			if (GetWindowTextLength(pBMStruct->hEUrl) == 0) 
				pBMStruct->hwndCurf = pBMStruct->hEUrl;
			else 
				pBMStruct->hwndCurf = pBMStruct->hEName;
		}
		SetFocus(pBMStruct->hwndCurf);
		break;
        
    case WM_COMMAND:
        WmlIEBM_OnCommand(hParent, hWnd, pBMStruct, wMsgCmd, wParam, lParam);
        break;
        
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
            pBMStruct->hwndCurf = GetFocus();//取得焦点所在子窗口的句柄
        else
            SetFocus(pBMStruct->hwndCurf);//激活后设置焦点
        break;
		
    case WM_PAINT:
        {
            int OldStyle;
            
            hdc = BeginPaint(hWnd, NULL);//为窗口的重画准备DC，并填充给出的绘图结构。
            OldStyle = SetBkMode(hdc, TRANSPARENT);
            SetBkMode(hdc, OldStyle);
            EndPaint(hWnd, NULL);
        }        
        break;

    case WM_CLOSE:
		SendMessage(hParent, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        PostMessage(hwndIEBMViewClo, WM_COMMAND, MAKEWPARAM(WIE_ID_BM_LIST, LBN_SELCHANGE), 0);
        DestroyWindow(hWnd);
        UnregisterClass("WMLIEBMEditClass", NULL);
        break;            

	case WM_KEYDOWN:
        switch (wParam)
        {
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		case VK_RETURN:
			PostMessage(hWnd, WM_COMMAND, WIE_IDM_BM_JUDGETOSAVE, NULL);
			break;
			
        default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);            
        }
		break;

	case PWM_ACTIVATE:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		if(wParam == WA_ACTIVE)
		{
			SetFocus(pBMStruct->hwndCurf);
			if(g_bDestroying)
				lResult = 0;
		}	
		else if(wParam == WA_INACTIVE)
		{
			pBMStruct->hwndCurf = GetFocus();
		}
		break;
        
    default:        
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    
    return lResult;    
}




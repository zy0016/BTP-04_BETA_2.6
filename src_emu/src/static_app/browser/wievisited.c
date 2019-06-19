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
#include "sys\stat.h"
#include "plx_pdaex.h"
#include "setting.h"
#include "pubapp.h"

#include "wievisited.h"
#include "WIE.h"

#define WIE_IDM_FILE_LIST        100
#define WM_UPDATEHISTORY          (WM_USER+1110)
#define TMPLEN					 16

static HWND hwndIEhisApp = NULL;
static HMENU hhisMenu;
static WIEHistoryItem wHistory[WIE_HIS_MAXNUM];
static HWND HhisList = NULL;
static int nDelitemdata = 0;
int nhistotal;
//////////////////////////////////////////////////////////////////////////
//for history detail
int hislines = 0;
int hisdistance = 0, hiskeylines = 0, hismovekeylines = 0;
int startposurl = 84;
int startposadd = 56;
int startpostitle = 2;
#define EACHLINE 30
BOOL bhismove = FALSE;
char szDetailstr[50];
int nMaxWidth = 0;
RECT rHisClient;
int nHeight;
HWND hHistoryDetails;
//////////////////////////////////////////////////////////////////////////

extern HFONT hViewFont;

static LRESULT IEHisWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static int WIE_HisListInit(HWND hList, WIEHistoryItem *lHistory);
static BOOL On_History_Details(HWND hParent);
static LRESULT HistoryDetailsWNDProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

BOOL On_IDM_Visited(HWND hParent, RECT rClient)
{
    WNDCLASS wc;
    
    wc.style         = 0;
    wc.lpfnWndProc   = IEHisWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLIeHisWndClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
    
    
    hhisMenu = LoadMenuIndirect((PMENUTEMPLATE)&WMLHISMENUTEMPLATE);    
    ModifyMenu(hhisMenu, WIE_IDM_HIS_OK, MF_BYCOMMAND, WIE_IDM_HIS_OK, (LPCSTR)WML_GOTO);
	ModifyMenu(hhisMenu, WIE_IDM_DETAILS, MF_BYCOMMAND, WIE_IDM_DETAILS, (LPCSTR)WML_DETAILS);
    ModifyMenu(hhisMenu, WIE_IDM_HIS_DEL, MF_BYCOMMAND, WIE_IDM_HIS_DEL, (LPCSTR)WML_REMOVE);
    ModifyMenu(hhisMenu, WIE_IDM_EMPTYHIS, MF_BYCOMMAND, WIE_IDM_EMPTYHIS, (LPCSTR)WML_REMOVEALL);
    
    hwndIEhisApp = CreateWindow(
        "WMLIeHisWndClass",             
        (LPCSTR)WML_CAPTION_HIS,
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent,  
        NULL,
        NULL, 
        NULL
        );
    if (hwndIEhisApp == NULL)
    {
        UnregisterClass("WMLIeHisWndClass", NULL);
        return FALSE;
    }
    
    ShowWindow(hwndIEhisApp, SW_SHOW);        
    UpdateWindow(hwndIEhisApp);

    return TRUE;
}

static LRESULT IEHisWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{   
    LRESULT lResult = FALSE;
    HDC hdc;
    WORD mID;
    WORD msgBox;    
    char szSelFile[URLNAMELEN] = "";
    int nFileSel;
        
    switch (wMsgCmd)
    {
    case WM_CREATE:      
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_HISTORY);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);	
        
        HhisList = CreateWindow(
            "LISTBOX", 
            "", 
            WS_VISIBLE|WS_VSCROLL|WS_CHILD|WS_TABSTOP|LBS_BITMAP|LBS_MULTILINE,
            0, 0, 176, 150,
            hWnd, 
            (HMENU)WIE_IDM_FILE_LIST,
            NULL, 
            NULL
            );        
		
        nhistotal = WIE_HisListInit(HhisList, wHistory);		

        if (nhistotal == 0)
        {
			PDASetMenu(GetWAPFrameWindow(), NULL);
            SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
            SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");         
        }
        else
		{
			PDASetMenu(GetWAPFrameWindow(), hhisMenu);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
            SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION); 
		}
		
		if(nhistotal == 0)
		{
			SetFocus(GetWAPFrameWindow());
			ShowWindow(HhisList, SW_HIDE);
			SetFocus(hWnd);
		}
		else
			ShowWindow(HhisList, SW_SHOW);
		SetFocus(HhisList);
        break;

	case PWM_SHOWWINDOW:
		{
			int id;

			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_HISTORY);		
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
			
			id = SendMessage(HhisList, LB_GETCURSEL, 0, 0);
			if (id == LB_ERR)
			{
				PDASetMenu(GetWAPFrameWindow(), NULL);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");         
			}
			else
			{
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION); 
			}	
			if(nhistotal == 0)
			{
				PDASetMenu(GetWAPFrameWindow(), NULL);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SetFocus(GetWAPFrameWindow());
				ShowWindow(HhisList, SW_HIDE);
				SetFocus(hWnd);
			}
			else
			{
				PDASetMenu(GetWAPFrameWindow(), hhisMenu);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
				ShowWindow(HhisList, SW_SHOW);
			}
		}			
		break;

	case WM_SETFOCUS:
		SetFocus(HhisList);
		break;
        
    case WM_KEYDOWN:
        {            
            switch (wParam)
            {
            case VK_RETURN:
				if(nhistotal != 0)
					PostMessage(hWnd, WM_COMMAND, WIE_IDM_HIS_OK, NULL);
                break;

			case VK_F10:
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;

			case VK_F5:
				return PDADefWindowProc(GetWAPFrameWindow(), wMsgCmd, wParam, lParam);

            default:
                return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }            
        }
        break;

    case WM_ACTIVATE:
        if (WA_ACTIVE == LOWORD(wParam))
            SetFocus(HhisList);
        break;

    case WM_PAINT:        
		{
			RECT rtClient;
			int bkmodeold;

			hdc = BeginPaint(hWnd, NULL);
			GetClientRect(hWnd, &rtClient);
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			if(nhistotal == 0)
			{
				DrawText(hdc, WML_NOHISTORY, -1, &rtClient, DT_VCENTER|DT_HCENTER);
			}
			SetBkMode(hdc, bkmodeold);
			EndPaint(hWnd, NULL);  
		}
        break;

    case WM_SELOK:
        WIE_EmptyHistory(wHistory);
        SendMessage(HhisList, LB_RESETCONTENT, NULL, NULL);

		PDASetMenu(GetWAPFrameWindow(), NULL);
        SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
        SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SetFocus(GetWAPFrameWindow());
		ShowWindow(HhisList, SW_HIDE);
		SetFocus(hWnd);
//		SendMessage(HhisList, LB_RESETCONTENT, NULL, NULL);
//		SendMessage(HhisList, LB_ADDSTRING, 0, (long)(LPSTR)(WML_NOHISTORY));
//		SendMessage(HhisList, LB_SETCURSEL, 0, 0);
        break;
		
	case WIE_IDM_HIS_EMPTYCONFIRM:
		if(lParam == 1)
		{
			nhistotal = 0;
			SendMessage(hWnd, WM_SELOK, 0, 0);
			PDASetMenu(GetWAPFrameWindow(), NULL);
			WIE_Savehistory();
		}
		break;

	case WIE_IDM_HIS_DELCONFIRM:
		if(lParam == 1)
		{
			BOOL bRemoved = FALSE;
			
			free(wHistory[nDelitemdata].szURLName);
			wHistory[nDelitemdata].szURLName = NULL;
			memset(wHistory[nDelitemdata].szName, 0, 100);
			wHistory[nDelitemdata].nUrltype = 0;
			nhistotal--;
			
			if(WIE_Savehistory())
				bRemoved = TRUE;
			if(nhistotal == 0)
			{
				PDASetMenu(GetWAPFrameWindow(), NULL);
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)"");
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
				SetFocus(GetWAPFrameWindow());
				ShowWindow(HhisList, SW_HIDE);
				SetFocus(hWnd);
			}
			else
			{
				LoadHistory();
				WIE_HisListInit(HhisList, wHistory);
			}
			if(bRemoved)
			{
				bRemoved = FALSE;
				PLXTipsWin(NULL, NULL, NULL, WML_REMOVED, NULL,	Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			}
		}
		break;
        
    case WM_COMMAND:
        {                    
            mID = LOWORD(wParam);
            msgBox = HIWORD(wParam);
            
            switch (mID)
            {
            case WIE_IDM_FILE_LIST:
                if (msgBox == LBN_DBLCLK)
                {
                    SendMessage(hWnd, WM_KEYDOWN, VK_RETURN, NULL);
                }
                break;

            case WIE_IDM_EMPTYHIS:
                {
                    char s[100];

                    strcpy(s, WML_EMPTYHISTORY);
                    
					PLXConfirmWinEx(NULL, hWnd, s, Notify_Request, (char *)NULL, 
						IDS_YES, IDS_NO, WIE_IDM_HIS_EMPTYCONFIRM);
                }
                break;        
                
            case WIE_IDM_HIS_OK:
                {
					int id;
					
					nFileSel = SendMessage(HhisList, LB_GETCURSEL, NULL, NULL);
					if (nFileSel != LB_ERR)
					{
						SendMessage(HhisList, LB_GETTEXT, (WPARAM)nFileSel, (LPARAM)szSelFile);
						id = SendMessage(HhisList, LB_GETITEMDATA, (WPARAM)nFileSel, (LPARAM)NULL);
						
						if((wHistory[id].szURLName != NULL) && (strlen(wHistory[id].szURLName) != 0))
						{
							SetCurentPage(wHistory[id].szURLName, wHistory[id].nUrltype);                    
							UI_Message(UM_URLGO, (unsigned long)wHistory[id].szURLName, wHistory[id].nUrltype);
						}
						PostMessage(hWnd, WM_CLOSE, NULL, NULL);
						hwndIEhisApp = NULL;
					}
				}
                break;

			case WIE_IDM_HIS_DEL:
				{
					char title[100];
					int index;
					char szSelFile[URLNAMELEN] = "";

					index = SendMessage(HhisList, LB_GETCURSEL, NULL, NULL);
					if (index != LB_ERR)
					{
						SendMessage(HhisList, LB_GETTEXT, (WPARAM)index, (LPARAM)szSelFile);
						nDelitemdata = SendMessage(HhisList, LB_GETITEMDATA, index, NULL);
					} 

					printf("________________the current select is %d_____________\r\n", nDelitemdata);
					strcpy(title, szSelFile);
					strcat(title, "\r\n");
					strcat(title, WML_REMOVEHISTORY);
					
					PLXConfirmWinEx(NULL, hWnd, title, Notify_Request, (char *)NULL, 
						IDS_YES, IDS_NO, WIE_IDM_HIS_DELCONFIRM);
				}				
				break;

			case WIE_IDM_DETAILS:
				On_History_Details(GetWAPFrameWindow());				
				break;

            case WIE_IDM_HIS_CLOSE:
                PostMessage(hWnd, WM_CLOSE, NULL, NULL);                
                break;
                
            default:
                break;
            }        
        }
        break;

    case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		DestroyMenu(hhisMenu);
        DestroyWindow(hWnd);
        UnregisterClass("WMLIeHisWndClass", NULL);
        hwndIEhisApp = NULL; 
		HhisList = NULL;
        break;
        
    default:        
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
       
    return lResult;
}
/*********************************************************************\
* Function       
* Purpose    for history  
* Params       
* Return            
* Remarks       
**********************************************************************/
int LoadHistory()
{
    int nRetFileSize = 0;
    FILE* fIe;
    int flag = 0;
    int hiscount = 0;
    int i = 0;
    int nUrlLen = 0;
    int nRetLen = 0;
    int nRetUrl = 0;
	int namelen = 0;
	struct stat st;
    
    //修改到这里，设一个数组读入历史，到了10，依次清除最早的保存。
    fIe = fopen( WIE_HIS_FILENAME, "rb");
    if (fIe == NULL)
        flag = -1;
    else
    {
        stat(WIE_HIS_FILENAME, &st);        
		if (-1 == (nRetFileSize = st.st_size))
        {
            flag =  -1;
        }
        else
        {
            fseek(fIe, 0, SEEK_SET);
            
            for (i = 0; i < WIE_HIS_MAXNUM; i++)
            {
                wHistory[i].szURLName = NULL;
                wHistory[i].nUrltype = URL_NULL;
                nRetUrl = fread(&(wHistory[i].nUrltype), 1, sizeof(int), fIe);
				fread(&namelen, 1, sizeof(int), fIe);
				if(namelen >= 0 && namelen < 100)
				{
					if(namelen != 0)
						fread(&(wHistory[i].szName), namelen, sizeof(char), fIe);
				}
				else
				{
					flag = -1;
					break;
				}
				
                nRetLen = fread(&nUrlLen, 1, sizeof(int), fIe);
                if (nRetUrl != sizeof(int) || nRetLen != sizeof(int) || nUrlLen < 0 || nUrlLen >URLNAMELEN)
                {
                    flag = -1;
                    break;
                }
                
                if (nUrlLen > 0 && nUrlLen <= URLNAMELEN)
                {
                    wHistory[i].szURLName = malloc(nUrlLen + 1);
                    if (!wHistory[i].szURLName)
                    {
						free(wHistory[i].szURLName);
                        flag = -1;
                        break;
                    }
					memset(wHistory[i].szURLName, 0, nUrlLen + 1);
                    nRetFileSize = fread(wHistory[i].szURLName, 1, nUrlLen, fIe);
                    if (nUrlLen > nRetFileSize)
                    {
                        free(wHistory[i].szURLName);
                        wHistory[i].szURLName = NULL;
                        flag = -1;
                        break;
                    }
                }//end if
            }//end for
            hiscount = i;            
        }//end else        
        fclose(fIe);
    }
    if (flag == -1)
    {
        for (i = hiscount; i < WIE_HIS_MAXNUM; i++)
        {
            wHistory[i].nUrltype = URL_NULL;
            wHistory[i].szURLName = NULL;
			strcpy(wHistory[i].szName, "");
        }
    }
    
    return hiscount;
}

static int WIE_HisListInit(HWND hList, WIEHistoryItem *lHistory)
{
    int i;
    int listID;
//	int nLenMulti;
//	char *szMulti;    

	SendMessage(hList, LB_RESETCONTENT, NULL, NULL);
    for (i = 0; i < WIE_HIS_MAXNUM; i++)
    {
        if (lHistory[i].nUrltype != URL_NULL && lHistory[i].szURLName != NULL)    
        {
//			nLenMulti = UTF8ToMultiByte(CP_ACP, 0, lHistory[i].szName, -1, NULL, 0, NULL, NULL);
//			szMulti = (char *)malloc(nLenMulti + 1);
//			memset(szMulti, 0, nLenMulti + 1);
//			nLenMulti = UTF8ToMultiByte(CP_ACP, 0, lHistory[i].szName, -1, szMulti, nLenMulti, NULL, NULL);
//			szMulti[nLenMulti] = '\0';

            listID = SendMessage(hList, LB_ADDSTRING, NULL, (LPARAM)lHistory[i].szName);
//			free(szMulti);
            SendMessage(hList, LB_SETITEMDATA, listID, (LPARAM)i);
			
//			nLenMulti = UTF8ToMultiByte(CP_ACP, 0, lHistory[i].szURLName, -1, NULL, 0, NULL, NULL);
//			szMulti = (char *)malloc(nLenMulti + 1);
//			memset(szMulti, 0, nLenMulti + 1);
//			nLenMulti = UTF8ToMultiByte(CP_ACP, 0, lHistory[i].szURLName, -1, szMulti, nLenMulti, NULL, NULL);
//			szMulti[nLenMulti] = '\0';
			
			SendMessage(hList, LB_SETAUXTEXT, MAKEWPARAM(listID, -1), (LPARAM)lHistory[i].szURLName);
//			free(szMulti);
        }
    }
    if ((i = SendMessage(hList, LB_GETCOUNT, NULL, NULL)) != 0)
    {
        SendMessage(hList, LB_SETCURSEL, 0, NULL);
    } 
	
	return i;
}

void WIE_EmptyHistory(WIEHistoryItem *dHistory)
{
    int i;
    
    for (i = 0; i < WIE_HIS_MAXNUM; i++)
    {
        if (dHistory[i].szURLName) 
        {
            free(dHistory[i].szURLName);
            dHistory[i].szURLName = NULL;
			strcpy(dHistory[i].szName, "");
        }
        dHistory[i].nUrltype = URL_NULL;            
    }
}

int WIE_AddHisItem(int nUrltype, char *szUrl, char *szName)
{
    int i;
    int nStart = 0, nEnd = 0;
    char *stmp = NULL;
	char *szMultitle, *szMulUrl;
	int nLenTitle, nLenUrl;
    
    if (szUrl == NULL) 
        return -1;

    if (strlen(szUrl) == 0 ) 
        return -1;

    if (nUrltype == URL_NULL) 
        return -1;    

	nLenTitle = UTF8ToMultiByte(CP_ACP, 0, szName, -1, NULL, 0, NULL, NULL);
	szMultitle = (char *)malloc(nLenTitle + 1);
	memset(szMultitle, 0, nLenTitle + 1);
	nLenTitle = UTF8ToMultiByte(CP_ACP, 0, szName, -1, szMultitle, nLenTitle, NULL, NULL);
	szMultitle[nLenTitle] = '\0';

	nLenUrl = UTF8ToMultiByte(CP_ACP, 0, szUrl, -1, NULL, 0, NULL, NULL);
	szMulUrl = (char *)malloc(nLenUrl + 1);
	memset(szMulUrl, 0, nLenUrl + 1);
	nLenUrl = UTF8ToMultiByte(CP_ACP, 0, szUrl, -1, szMulUrl, nLenUrl, NULL, NULL);
	szMulUrl[nLenUrl] = '\0';
	   
    for (i = 0; i < WIE_HIS_MAXNUM; i++)
    {
        if (wHistory[i].nUrltype != URL_NULL && wHistory[i].szURLName != NULL)
        {
            if (strncasecmp(wHistory[i].szURLName, szMulUrl, nLenUrl) == 0)
            {
                nStart = 0;
                nEnd = i - 1;
                stmp = wHistory[i].szURLName;                
				return -1;
            }                        
            nEnd = i;            
        }        
    }
    if (nEnd == -1) 
	{
		free(szMultitle);
		free(szMulUrl);
		return 0;
	}
    
    if (nEnd >= WIE_HIS_MAXNUM - 1)
    {
        nEnd = WIE_HIS_MAXNUM - 2;                
        if (stmp == NULL && wHistory[nEnd+1].szURLName) 
            free(wHistory[nEnd +1].szURLName);
    }
    for (i = nEnd; i >= nStart; i--)
    {
        wHistory[i+1].nUrltype = wHistory[i].nUrltype;
        wHistory[i+1].szURLName = wHistory[i].szURLName;
		strcpy(wHistory[i+1].szName, wHistory[i].szName);
    }    
    
    wHistory[0].nUrltype = nUrltype;    
    if (stmp)
	{
        wHistory[0].szURLName = stmp;
		strcpy(wHistory[0].szName, szMultitle);
	}
    else
	{
        wHistory[0].szURLName = StrDup(szMulUrl);
		strcpy(wHistory[0].szName, szMultitle);
	}
    
	nhistotal++;
	if((HhisList != NULL) && (hHistoryDetails == NULL))
	{
		PDASetMenu(GetWAPFrameWindow(), hhisMenu);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_GOTO);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
		ShowWindow(HhisList, SW_SHOW);
		WIE_HisListInit(HhisList, wHistory);
	}
	free(szMultitle);
	free(szMulUrl);
    return nEnd;    
}

BOOL WIE_Savehistory()
{
	int i = 0, nFileLen = 0, nUrlLen;//nRetFileSize, 
    FILE* fIe;
	int tmplen;
        
    //修改到这里，设一个数组读入历史，到了10，依次清除最早的保存。
    if ((fIe = fopen(WIE_HIS_FILENAME, "wb")) == NULL)      //判断是否有此文件
    {
        if ((fIe = fopen(WIE_HIS_FILENAME, "wb+")) == NULL)      //如果没有则创建
        {
            WIE_EmptyHistory(wHistory);
            return FALSE;
        }
    }
   	{
        for (i = 0; i < WIE_HIS_MAXNUM; i++)
        {
			fwrite(&(wHistory[i].nUrltype), 1, sizeof(int), fIe);
			tmplen = strlen(wHistory[i].szName);
			fwrite(&tmplen, 1, sizeof(int), fIe);
			if(tmplen != 0)
				fwrite(wHistory[i].szName, 1, tmplen, fIe);

            nUrlLen = 0;
            if (wHistory[i].szURLName)
                nUrlLen = strlen(wHistory[i].szURLName);
            if (nUrlLen != 0)
            {
//				tmplen = strlen(wHistory[i].szURLName);
				fwrite(&nUrlLen, 1, sizeof(int), fIe);
				fwrite(wHistory[i].szURLName, 1, nUrlLen, fIe);
            }   
			else
				fwrite(&nUrlLen, 1, sizeof(int), fIe);
        }
    }
    
	WIE_EmptyHistory(wHistory);
    fclose(fIe);
    
    return TRUE;   
}


/*********************************************************************\
* Function     On_History_Details
* Purpose      create history details window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
static BOOL On_History_Details(HWND hParent)
{
	WNDCLASS wc;
	int index, urllen = 0, itemid;
	char szSelFile[URLNAMELEN] = "";

	GetClientRect(hParent, &rHisClient); 
    
    wc.style         = 0;
    wc.lpfnWndProc   = HistoryDetailsWNDProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WAP_History_detail";
    
	RegisterClass(&wc);     

	index = SendMessage(HhisList, LB_GETCURSEL, NULL, NULL);
	itemid = SendMessage(HhisList, LB_GETITEMDATA, (WPARAM)index, NULL);
	if(wHistory[itemid].szURLName == NULL)
	{
		UnregisterClass("WAP_History_detail", NULL);
		return FALSE;
	}
	else
	{
		if(strlen(wHistory[itemid].szURLName) == 0)
		{
			UnregisterClass("WAP_History_detail", NULL);
			return FALSE;
		}
	}
	if(wHistory[itemid].nUrltype == 0)
	{
		UnregisterClass("WAP_History_detail", NULL);
		return FALSE;
	}
	
	if(wHistory[itemid].szURLName != NULL)
		urllen = strlen(wHistory[itemid].szURLName);
	
	nMaxWidth = rHisClient.right-rHisClient.left - 6;
    
    hHistoryDetails = CreateWindow(
        "WAP_History_detail", 
        "", 
        WS_VISIBLE|WS_CHILD|WS_VSCROLL,
        rHisClient.left, rHisClient.top, 
		rHisClient.right-rHisClient.left, 
		rHisClient.bottom-rHisClient.top, 
        hParent, 
        NULL,
        NULL, 
        NULL
        );
    if (hHistoryDetails == NULL) 
    {
        UnregisterClass("WAP_History_detail", NULL);
        return FALSE;
    }
    
    ShowWindow(hHistoryDetails, SW_SHOW);            
    UpdateWindow(hHistoryDetails);
    
    return TRUE;
}

static LRESULT HistoryDetailsWNDProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    WORD mID;
    WORD msgBox;  
	int index;
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			WMLCFGFILE curconfig;
				
			WAP_GetCurConfig(&curconfig);
		
			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_DETAILS);
			PDASetMenu(GetWAPFrameWindow(), NULL);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)"");
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK); 
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			RedrawScrollBar(hWnd, 5, 0, 5);
			SetFocus(hWnd);
		}
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_DETAILS);
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK); 
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SetFocus(hWnd);
		break;  
        
    case WM_PAINT:        
		{
			int bkmodeold;
			int nLinechalen = 0;
			HFONT hBig;

			hdc = BeginPaint(hWnd, NULL);
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			hBig = SelectObject(hdc, hViewFont);
			TextOut(hdc, 0, startpostitle, WML_HISTORYTITLE, -1);
			SelectObject(hdc, hBig);
			index = SendMessage(HhisList, LB_GETCURSEL, NULL, NULL);
			
			if (index != LB_ERR)
			{
				int i, j = 0, k = 0;
				char szSelFile[URLNAMELEN] = "";
				char tmpurl[TMPLEN];
				int urllen;
				int id;
//				char *tmp;
				int tmplen;
				char *showname;
				char *showurlname;
				
				id = SendMessage(HhisList, LB_GETITEMDATA, (WPARAM)index, NULL);
				
				i = id;
				showname = (char *)malloc(strlen(wHistory[i].szName) + 1);
				memset(showname, 0, strlen(wHistory[i].szName) + 1);
				strcpy(showname, wHistory[i].szName);
				showurlname = (char *)malloc(strlen(wHistory[i].szURLName) + 1);
				memset(showurlname, 0, strlen(wHistory[i].szURLName) + 1);
				strcpy(showurlname, wHistory[i].szURLName);
				tmplen = strlen(showname);

//				tmplen = UTF8ToMultiByte(CP_ACP, 0, showname, -1, NULL, 0, NULL, NULL);
//				tmp = (char *)malloc(tmplen + 1);
//				memset(tmp, 0, tmplen + 1);
//				UTF8ToMultiByte(CP_ACP, 0, showname, -1, tmp, tmplen, NULL, NULL);
//				tmp[tmplen] = '\0';

				GetTextExtentExPoint(hdc, showname, tmplen, nMaxWidth, &nLinechalen, NULL, NULL);
				if(nLinechalen < tmplen)
				{
					memset(tmpurl, 0, TMPLEN);
					strncpy(tmpurl, showname, nLinechalen - 3);
					strcat(tmpurl, "...");
				
					TextOut(hdc, 0, EACHLINE - hismovekeylines * EACHLINE, tmpurl, strlen(tmpurl));
//					free(tmp);
				}
				else
				{	
					memset(tmpurl, 0, TMPLEN);
					strncpy(tmpurl, showname, nLinechalen);
					
					TextOut(hdc, 0, EACHLINE - hismovekeylines * EACHLINE, tmpurl, strlen(tmpurl));
//					free(tmp);
				}
				
				memset(szDetailstr, 0, 50);
				urllen = strlen(showurlname);
				do {
					memset(szDetailstr, 0, 50);
					GetTextExtentExPoint(hdc, &showurlname[j], urllen, nMaxWidth, &nLinechalen, NULL, NULL);
					strncpy(szDetailstr, &showurlname[j], nLinechalen);
					urllen -= nLinechalen;
					if(urllen > 0)
					{
						j += nLinechalen;
						
//						tmplen = UTF8ToMultiByte(CP_ACP, 0, szDetailstr, -1, NULL, 0, NULL, NULL);
//						tmp = (char *)malloc(tmplen + 1);
//						memset(tmp, 0, tmplen + 1);
//						UTF8ToMultiByte(CP_ACP, 0, szDetailstr, -1, tmp, tmplen, NULL, NULL);
//						tmp[tmplen] = '\0';
						
						TextOut(hdc, 0, startposurl + k*30, szDetailstr, nLinechalen);
						k++;
//						free(tmp);
					}
					else
					{
//						tmplen = UTF8ToMultiByte(CP_ACP, 0, szDetailstr, -1, NULL, 0, NULL, NULL);
//						tmp = (char *)malloc(tmplen + 1);
//						memset(tmp, 0, tmplen + 1);
//						UTF8ToMultiByte(CP_ACP, 0, szDetailstr, -1, tmp, tmplen, NULL, NULL);
//						tmp[tmplen] = '\0';
						
						TextOut(hdc, 0, startposurl + k*30, szDetailstr, nLinechalen);
						urllen = 0;
//						free(tmp);
					}
				} while(urllen > 0);
				
				hislines = k + 3;
				
				if((hislines * EACHLINE) < (rHisClient.bottom-rHisClient.top))
					nHeight = rHisClient.bottom-rHisClient.top;
				else
				{
					nHeight = hislines * EACHLINE;
					hisdistance = (hislines * EACHLINE) - (rHisClient.bottom - rHisClient.top);
					if(!(hisdistance%EACHLINE))
						hiskeylines = hisdistance/EACHLINE;
					else if(hisdistance < EACHLINE)
						hiskeylines = 1;
					else
						hiskeylines = hisdistance/EACHLINE + 1;
					
					bhismove = TRUE;
				}
			free(showname);
			free(showurlname);
			}
			
			RedrawScrollBar(hWnd, hislines + 1, hismovekeylines, 5);
			SelectObject(hdc, hViewFont);
			TextOut(hdc, 0, startposadd, WML_URLADDRESS, -1);
			SetBkMode(hdc, bkmodeold);
			EndPaint(hWnd, NULL);
		}
        break;
        
    case WM_COMMAND:        
        mID    = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {
        default:
            break;                
        }
        break;
        
    case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        hislines = 0;
		hisdistance = 0;
		hiskeylines = 0;
		hismovekeylines = 0;
		startposurl = 84;
		startposadd = 56;
		startpostitle = 2;
		bhismove = FALSE;
        DestroyWindow(hWnd);
		UnregisterClass("WAP_History_detail", NULL);
        break;

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_UP:
			{
				if(bhismove)
				{
					if(hismovekeylines > 0)
					{
						hismovekeylines--;
						startposurl += EACHLINE;
						startposadd += EACHLINE;
						startpostitle += EACHLINE;
						InvalidateRect(hWnd, NULL, TRUE);
						RedrawScrollBar(hWnd, hislines + 1, hismovekeylines, 5);
					}
				}
			}
			break;
			
		case VK_DOWN:
			{
				if(bhismove)
				{
					if(hismovekeylines <= hiskeylines)
					{
						hismovekeylines++;
						startposurl -= EACHLINE;
						startposadd -= EACHLINE;
						startpostitle -= EACHLINE;
						InvalidateRect(hWnd, NULL, TRUE);
						RedrawScrollBar(hWnd, hislines + 1, hismovekeylines, 5);
					}
				}
			}
			break;

		case VK_RETURN:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
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

void WML_HisCloseWindow(void)
{
	PostMessage(hwndIEhisApp, WM_CLOSE, NULL, NULL);
	PostMessage(hHistoryDetails, WM_CLOSE, NULL, NULL);
	WIE_Savehistory();
}

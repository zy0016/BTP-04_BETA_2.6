/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : connectivity setting
 *
 * Purpose  : connection setting
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallConnectionSetting.h"
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "plx_pdaex.h"
#include    "setting.h"
#include    "pubapp.h"
#include    "setup.h"
#include    "mullang.h"

static HWND hAppWnd;
static HWND hFrameWin;
static char * pClassName = "Connect_ConnectionSettingClass";
static int iMenuRefFlag = 0;

static ConnIndexTbl* G_ConnIndex = NULL;

static  HWND    hConnectionList;
static HBITMAP hNewConnIcon, hGPRSConnIcon, hCSDConnIcon;
static int iDelIndex, iConnListNum = 0;
static LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void ConnectionMainWinCommad(HWND hWnd,WPARAM wParam,LPARAM lParam);
static BOOL LoadConnectionList(HWND hList);
static void CreateMainMenu(HWND hWnd);
static BOOL CreateControl(HWND hWnd);
static void ModifyConnList(int nNewNum);

static BOOL CreateConnIndexTbl(int nNum);
static BOOL FreeConnIndexTbl(void);
static int GetConnFileIndex(HWND hList, int ListIndex);
static int GetConnType(HWND hList, int ListIndex);


int StrAlphabetNoCaseCmp(char *str1, char *str2);

extern  BOOL    IspReadInfo (UDB_ISPINFO * uIspInfo ,int iNum);
extern  BOOL    CallCreateNewConnection(HWND hwndCall, HWND hwnd, UINT iMsg);
extern  BOOL    GPRSSettingList(HWND hWnd, HWND hParent, int iConIndex);
extern  BOOL    DataCallSettingList(HWND hWnd, int iconindex);
extern  BOOL     DeleteIspInfo(int iNum);
extern  int     IspGetNum(void);

BOOL    CallConnectionSetting(HWND hwndCall)
{        
    WNDCLASS    wc;   
    RECT rect;
    hFrameWin = hwndCall;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    SetWindowText(hFrameWin,ML("Connections"));

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,
        (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back"));
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

  
    GetClientRect(hFrameWin, &rect);
    hAppWnd = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD,  
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hFrameWin, 
        NULL, NULL, NULL);
    
    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }    
    
/*
    ShowWindow(hFrameWin, SW_SHOW);
    UpdateWindow(hFrameWin); 
*/
    SetFocus(hAppWnd);


    return (TRUE);

}

/***************************************************************
* Function  AppWndProc
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static  int     iIndex, iTitleLen, iConnectionIndex, iTmp, nConnListNum;
    static  char    szTitleBuffer[UDB_K_NAME_SIZE + 1];
    LRESULT         lResult;
    HDC hdc;
    lResult = TRUE; 
    switch ( wMsgCmd )
	{
    case PWM_SHOWWINDOW:
		if ( 1 == iMenuRefFlag )
		{
			iMenuRefFlag = 0;
			break;
		}
        SetWindowText(hFrameWin,ML("Connections"));
        
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,
            (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back"));
        iTmp = SendMessage(hConnectionList, LB_GETCURSEL, 0, 0);
       
        LoadConnectionList(hConnectionList);

        nConnListNum = SendMessage(hConnectionList, LB_GETCOUNT, 0, 0);

        if(iTmp < nConnListNum)
            SendMessage(hConnectionList, LB_SETCURSEL, iTmp, 0);
        else
            SendMessage(hConnectionList, LB_SETCURSEL, 0, 0);

        iIndex = SendMessage(hConnectionList,LB_GETCURSEL,0,0);
        if( 0 == iIndex )
        {
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            
        }
        else
        {
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
            SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_BUTTON_EDIT,1),(LPARAM)ML("Edit") ) ;
        }
//        CreateMainMenu(hFrameWin);
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetFocus(hWnd);
        break;

    case WM_CREATE:
        hdc = GetDC(hWnd);
        hNewConnIcon  = LoadImageEx(hdc,NULL,ICON_CONNECTION_NEW,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hGPRSConnIcon = LoadImageEx(hdc,NULL,ICON_CONNECTION_GPRS,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hCSDConnIcon  = LoadImageEx(hdc,NULL,ICON_CONNECTION_CSD,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);
        CreateControl(hWnd);
        LoadConnectionList(hConnectionList);
        SendMessage(hConnectionList, LB_SETCURSEL, 0, 0);
        CreateMainMenu(hFrameWin);
		iMenuRefFlag = 0;

        break;
        
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_CONNECTIONLIST));
        break;        
        
    case WM_DESTROY : //Quit the application;
		FreeConnIndexTbl();
        DeleteObject(hNewConnIcon);
        DeleteObject(hGPRSConnIcon);
        DeleteObject(hCSDConnIcon);
        hAppWnd = NULL;
        UnregisterClass(pClassName,NULL);
        break;
        
    case WM_KEYDOWN:
        iIndex = SendMessage(hConnectionList,LB_GETCURSEL,0,0);
        
        switch(LOWORD(wParam))
        {       
        case VK_F10:
            //                   SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_RETURN:
            if(iIndex != 0)
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_EDIT,0);//if the focus is not on the first option(New item),the left softkey is used.				   
            break;
        case VK_F5:
            if(iIndex == 0)
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_NEW,0);//the focus is on the "New item", the middle softkey is used.
            else 
			{
				iMenuRefFlag = 1;
                return PDADefWindowProc(hFrameWin, wMsgCmd, wParam, lParam);                
			}
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
        }
        break;  
        
        
        case WM_COMMAND :
            switch (LOWORD(wParam))
            {
            case IDC_BUTTON_NEW:
                CallCreateNewConnection(hFrameWin, hWnd, CALLBACK_ADDLIST);
                break;
            case IDC_BUTTON_EDIT://left softkey	
                iIndex = SendMessage(hConnectionList,LB_GETCURSEL,0,0); 

                iConnectionIndex = GetConnFileIndex(hConnectionList, iIndex);

				if (iConnectionIndex == -1)
					break;
                   
                GPRSSettingList(hFrameWin, NULL, iConnectionIndex);
                break;                
            default:
                ConnectionMainWinCommad(hWnd, wParam, lParam);           
                break;
            }
            
            switch (HIWORD(wParam))
            {
            case LBN_SELCHANGE:
                iIndex = SendMessage(hConnectionList,LB_GETCURSEL,0,0);
                if( 0 == iIndex )
                {
                    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
                    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
                    
                }
                else
                {
                    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
                    SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, 
                        MAKEWPARAM(IDC_BUTTON_EDIT,1),(LPARAM)ML("Edit") ) ;
                }
                break;
                
            default:
                break;
            }
            break;
/*
    case CALLBACK_ADDLIST:
        ModifyConnList(lParam);
        break;
*/
    case CALLBACK_DELUSECONN:
        if(lParam)
        {
            if(DeleteIspInfo(iDelIndex))
                PLXTipsWin(hFrameWin, hWnd, NULL,ML("Deleted"),ML("Connections"),Notify_Success,ML("Ok"),0,WAITTIMEOUT);            
            break;
        }
		else
		{
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Edit")) ;
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back")) ;
			SetWindowText(hFrameWin,ML("Connections"));
		}
        break;
    case CALLBACK_DELCONN:
        if(lParam)
        {
            if(DeleteIspInfo(iDelIndex))
                PLXTipsWin(hFrameWin, hWnd, NULL,ML("Deleted"),ML("Connections"),Notify_Success,ML("Ok"),0,WAITTIMEOUT);
            break;
        }
		else
		{
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Edit")) ;
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back")) ;
			SetWindowText(hFrameWin,ML("Connections"));
		}
        break;
            
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    }
    return lResult;
}

static void ModifyConnList(int nNewNum)
{
    int i, nConnNum;
    UDB_ISPINFO tmp;
    nConnNum = IspGetNum();

    for(i=0; i<nNewNum; i++)
    {
        IspReadInfo(&tmp, i+nConnNum-nNewNum);
        SendMessage(hConnectionList, LB_ADDSTRING, 0, (LPARAM)tmp.ISPName);
    }
}
static BOOL CreateControl(HWND hWnd)
{
    RECT rect;

    GetClientRect(hWnd, &rect);

    hConnectionList = CreateWindow("LISTBOX", NULL, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right,rect.bottom,
        hWnd, (HMENU)IDC_CONNECTIONLIST, NULL, NULL);
     
    if (hConnectionList == NULL )
        return FALSE;

    return TRUE;
}

static BOOL LoadConnectionList(HWND hList)
{
    int nConnectionNum, nConnNumTmp;
    UDB_ISPINFO ConnectionInfo;
    DWORD dWord;
    int i, j, k;
	int nConnType;
	char ISPNameTmp[UDB_K_NAME_SIZE + 1];

    memset(&ConnectionInfo, 0, sizeof(UDB_ISPINFO));
    SendMessage(hList, LB_RESETCONTENT, 0, 0);
    nConnectionNum = IspGetNum();

	if ( !CreateConnIndexTbl(nConnectionNum) )
		return FALSE;

    SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)ML("New connection"));    

    for(i=0;i<nConnectionNum;i++)
    {
//        dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
        IspReadInfo(&ConnectionInfo, i);
		
		strcpy(G_ConnIndex[i].ConnInfoName, ConnectionInfo.ISPName);
		G_ConnIndex[i].iIndex = i;
		G_ConnIndex[i].nType = ConnectionInfo.DtType;
		
		if (i == 0)
		{
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)G_ConnIndex[i].ConnInfoName/*ConnectionInfo.ISPName*/);
			G_ConnIndex[i].iListIndex = 1;
		}
		else //i > 0
		{
			nConnNumTmp = SendMessage(hList, LB_GETCOUNT, 0, 0);
			for(j=1; j<nConnNumTmp; j++)
			{
				SendMessage(hList, LB_GETTEXT, j, (LPARAM)ISPNameTmp);
				if ( (StrAlphabetNoCaseCmp(G_ConnIndex[i].ConnInfoName, ISPNameTmp)) <= 0 )
				{
					SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)(ConnectionInfo.ISPName));
					{
						for (k=0;k<i;k++)
						{
							if (G_ConnIndex[k].iListIndex >= j)
							{
								G_ConnIndex[k].iListIndex++;
							}
						}
						G_ConnIndex[i].iListIndex = j;
					}
					break;
				}
			}
			if (j == nConnNumTmp)
			{
				SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)(ConnectionInfo.ISPName));
				G_ConnIndex[i].iListIndex = j;
			}
		}
    }
    i = 0;
    dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
    SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hNewConnIcon);  

	for (i=1; i<nConnectionNum+1; i++)
	{
		nConnType = GetConnType(hList, i);
		dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
		if (nConnType == 1)
			SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hGPRSConnIcon);
		else if (nConnType == 0)
			SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hCSDConnIcon);
	}
    
    return TRUE;
}

static void CreateMainMenu(HWND hWnd)
{
    HMENU hMainMenu;
    hMainMenu = CreateMenu();
    AppendMenu(hMainMenu,MF_STRING,IDM_EDIT,ML("Edit"));
    AppendMenu(hMainMenu,MF_STRING,IDM_DEL,ML("Delete"));
    PDASetMenu(hWnd,hMainMenu);
}


static void ConnectionMainWinCommad(HWND hWnd,WPARAM wParam,LPARAM lParam)
{
    int iConIndex;
    char szTipsBuffer[100];
    UDB_ISPINFO ConnTmp;
    HDC hdc;
	memset(szTipsBuffer, 0, sizeof(szTipsBuffer));
    switch(LOWORD(wParam))
    {
    case IDM_EDIT:
        PostMessage(hWnd,WM_COMMAND,IDC_BUTTON_EDIT,0);
        break;
    case IDM_DEL:
        memset(szTipsBuffer, 0, sizeof(szTipsBuffer));
        iConIndex = SendMessage(hConnectionList,LB_GETCURSEL,0,0); //get the current selected item number
        SendMessage(hConnectionList, LB_GETTEXT, iConIndex, (LPARAM)szTipsBuffer);

        hdc = GetDC(hWnd);
        GetExtentFittedText(hdc, szTipsBuffer, strlen(szTipsBuffer), szTipsBuffer,
                            sizeof(szTipsBuffer),SETT_TIPWIN_WIDTH, '.', 3);
        ReleaseDC(hWnd, hdc);
       
        strcat(szTipsBuffer, ":");
        
        iConIndex = GetConnFileIndex(hConnectionList, iConIndex);

		if (iConIndex == -1)
			return;

        IspReadInfo(&ConnTmp, iConIndex);
        
        if(ConnTmp.PrimarySelect != 0) //connection is in use
        {
            strcat(szTipsBuffer, ML("snConnection in use.snDelete anywayask"));
            PLXConfirmWinEx(hFrameWin, hWnd, szTipsBuffer, Notify_Request, ML("Connections"), ML("Yes"), ML("No"), CALLBACK_DELUSECONN);
			iMenuRefFlag = 1;
            iDelIndex = iConIndex;
            break;                                
        }
        else//connection is not in use
        {
            strcat(szTipsBuffer, ML("snDeleteask"));
            PLXConfirmWinEx(hFrameWin, hWnd, szTipsBuffer, Notify_Request, ML("Connections"), ML("Yes"), ML("No"), CALLBACK_DELCONN);
			iMenuRefFlag = 1;
            iDelIndex = iConIndex;
            break;
        }
        break;
    }
}

char LowerToUpper( char c )
{
	if( c >= 'a' && c <= 'z' )
		return (char)(c + 'A' - 'a');
	else
		return c;
}

int StrAlphabetNoCaseCmp(char *str1, char *str2)
{
	char c1, c2;
	while( (*str1 || *str2) )
	{
		c1 = LowerToUpper( *str1++ );
		c2 = LowerToUpper( *str2++ );
		if( c1 > c2 )
			return 1;
		if( c1 < c2 )
			return -1;
	}
	return 0;

}

static BOOL CreateConnIndexTbl(int nNum)
{
	if (G_ConnIndex != NULL)
	{
		free(G_ConnIndex);
		G_ConnIndex = NULL;
	}

	G_ConnIndex = (ConnIndexTbl*)malloc(nNum * sizeof(ConnIndexTbl));

	if(G_ConnIndex == NULL)
		return FALSE;
	iConnListNum = nNum;
	return TRUE;
}

static BOOL FreeConnIndexTbl(void)
{
	if (G_ConnIndex != NULL)
	{
		free(G_ConnIndex);
		G_ConnIndex = NULL;
		return TRUE;
	}
	return FALSE;
}

static int GetConnFileIndex(HWND hList, int ListIndex)
{
//	char ConnName[UDB_K_NAME_SIZE + 1];
	int nNum;
	int i;
	
//	memset(ConnName, 0, sizeof(ConnName));

	nNum = SendMessage(hList, LB_GETCOUNT, 0, 0);

//	SendMessage(hList, LB_GETTEXT, (WPARAM)ListIndex, (LPARAM)ConnName);

	for (i=0; i<nNum; i++)
	{
		if ( G_ConnIndex[i].iListIndex == ListIndex )
			return G_ConnIndex[i].iIndex;
	}

	return -1;
}

static int GetConnType(HWND hList, int ListIndex)
{
//	char ConnName[UDB_K_NAME_SIZE + 1];
	int nNum;
	int i;
//	memset(ConnName, 0, sizeof(ConnName));
//	SendMessage(hList, LB_GETTEXT, (WPARAM)ListIndex, (LPARAM)ConnName);
	nNum = SendMessage(hList, LB_GETCOUNT, 0, 0);
	for (i=0; i<nNum; i++)
	{
		if ( G_ConnIndex[i].iListIndex == ListIndex )
			return G_ConnIndex[i].nType;
	}
	return -1;

}

ConnIndexTbl* Sett_GetConnNameTbl(int* iNum)
{
	*iNum = iConnListNum;
	return G_ConnIndex;
}

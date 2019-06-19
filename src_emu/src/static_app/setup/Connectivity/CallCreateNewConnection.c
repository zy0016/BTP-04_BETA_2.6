/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : connectivity setting
 *
 * Purpose  : create new connection window
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallCreateNewConnection.h"
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

static HWND hWndApp = NULL;
static HWND hFrameWin = NULL;
static char * pClassName = "CreateNewConnectionClass";

static int iFlagNew = 0;
static HWND hCallWnd;
static UINT iCallAddConn;
static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hWnd,HWND * hListContent);
static void GetDefConnValue(UDB_ISPINFO * DefConn, BOOL iFlag);

extern  int  IspGetNum (void);
extern  BOOL IspReadInfo (UDB_ISPINFO * uIspInfo ,int iNum);
extern  BOOL GPRSSettingList(HWND hWnd, HWND hParent, int GPRSConnectIndex);
extern  BOOL IspCreateNewInfo(UDB_ISPINFO * uIspInfo);
extern	ConnIndexTbl* Sett_GetConnNameTbl(int* iNum);


BOOL CallCreateNewConnection(HWND hwndCall, HWND hwnd, UINT iCallback)
{
	WNDCLASS    wc;
    RECT rect;

    hFrameWin = hwndCall;
    hCallWnd = hwnd;
    iCallAddConn = iCallback;
    
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

    GetClientRect(hFrameWin, &rect);

    hWndApp = CreateWindow(pClassName,NULL, 
        WS_VISIBLE | WS_CHILD, 
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hFrameWin, NULL, NULL, NULL);

    if (NULL == hWndApp)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
     
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");         
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));        
    
    SetWindowText(hFrameWin, ML("Base connection"));
    SetFocus(hWndApp);

    return (TRUE);
}
static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;
	HDC             hdc;
    static  HWND    hListContent;
    static  int     i,j,nListNum;
    static int iIndex, iConnectionIndex;
    int nCurConnNum;
    UDB_ISPINFO ConnectionInfo, defGPRSConnInfo, defCSDConnInfo;
	static ConnIndexTbl* pConnNameTbl = NULL;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            CreateControl(hWnd,&hListContent);
            
            memset(&ConnectionInfo, 0, sizeof(UDB_ISPINFO));
            
            hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            ReleaseDC(hWnd,hdc);		

            SendMessage(hListContent, LB_ADDSTRING, 0, (LPARAM)ML("Default GPRS"));
            SendMessage(hListContent, LB_ADDSTRING, 0, (LPARAM)ML("Default data call"));
            
			pConnNameTbl = Sett_GetConnNameTbl(&nListNum);

			if(pConnNameTbl != NULL && nListNum > 0)
			{
				for(i=0;i<nListNum;i++)
				{
					for (j=0; j<nListNum; j++)
					{
						if (i == pConnNameTbl[j].iListIndex - 1)
						SendMessage(hListContent, LB_ADDSTRING, 0, (LPARAM)pConnNameTbl[j].ConnInfoName);
					}
				}
			}
            SendMessage(hListContent,LB_SETCURSEL,0,0);
            Load_Icon_SetupList(hListContent,hIconNormal,hIconSel,2 + nListNum,-1);
            
            break;
        case PWM_SHOWWINDOW:
            SetFocus(hWnd);
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");         
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));        
 #if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
           
            SetWindowText(hFrameWin, ML("Base connection"));
/*
            nListNum = SendMessage(hListContent, LB_GETCOUNT, 0, 0);
            nConnectionNum = IspGetNum();
            if(nListNum-2 != nConnectionNum)
            {
                IspReadInfo(&ConnectionInfo, nConnectionNum-1);
                SendMessage(hListContent, LB_ADDSTRING, 0, (LPARAM)ConnectionInfo.ISPName);
            }
            iIndex = SendMessage(hListContent, LB_GETCURSEL, 0, 0);
            Load_Icon_SetupList(hListContent,hIconNormal,hIconSel,2 + nConnectionNum,iIndex);
*/
            break;
            
        case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_MODELIST));
            break;            
           
        case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {
                
            case VK_F10:
                PostMessage(hCallWnd, iCallAddConn, 0, (LPARAM)iFlagNew);
                SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                PostMessage(hWnd, WM_CLOSE, 0, 0);
                break;
            case VK_F5:
                SendMessage(hWnd,WM_COMMAND,IDM_OK,0);
                break;
            default:
                lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }
            break;  
            
        case WM_DESTROY : 
            hWndApp = NULL;
            DeleteObject(hIconNormal);
            DeleteObject(hIconSel);
            KillTimer(hWnd, TIMER_ASURE);
            UnregisterClass(pClassName,NULL);
            break;
                
        case WM_COMMAND :          
            switch (LOWORD(wParam))
            {
            case IDM_OK:   
                iIndex = SendMessage(hListContent,LB_GETCURSEL,0,0);              
                //get the current selected item number
                Load_Icon_SetupList(hListContent,hIconNormal,hIconSel,2 + nListNum,iIndex);
                //change the radio button of the current selected item		
                SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                break;
            }
            break;
            
        case WM_TIMER:
            switch(wParam)
            {
            case TIMER_ASURE:
                KillTimer(hWnd, TIMER_ASURE);                
                if(iIndex > 1)
                {
                    iConnectionIndex = iIndex - 2;
					for (i=0; i<nListNum; i++)
					{
						if (pConnNameTbl[i].iListIndex - 1 == iConnectionIndex)
						{
							iConnectionIndex = pConnNameTbl[i].iIndex;
							break;
						}
					}
					IspReadInfo(&ConnectionInfo, iConnectionIndex);
                    IspCreateNewInfo(&ConnectionInfo);
                    iFlagNew++;
                    GPRSSettingList(hFrameWin, hWnd, IspGetNum()-1);
                    break;
                }
                if(0 == iIndex)//Default GPRS
                {
                    memset(&defGPRSConnInfo, 0, sizeof(UDB_ISPINFO));
                    GetDefConnValue(&defGPRSConnInfo, TRUE);
                    IspCreateNewInfo(&defGPRSConnInfo);
                    iFlagNew++;
                    nCurConnNum = IspGetNum() - 1;

                    if (nCurConnNum < 0)
                        break;

                    GPRSSettingList(hFrameWin, hWnd, nCurConnNum);
                    break;
                }
                if(1 == iIndex)// Default Data call
                {
                    memset(&defCSDConnInfo, 0, sizeof(UDB_ISPINFO));
                    GetDefConnValue(&defCSDConnInfo, FALSE);
                    IspCreateNewInfo(&defCSDConnInfo);
                    iFlagNew++;
		            
                    nCurConnNum = IspGetNum() - 1;
                    
                    if (nCurConnNum < 0)
                        break;

                    GPRSSettingList(hFrameWin, hWnd, nCurConnNum);
                    break;
                }
               break;
            }
            break;
        default :
            lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
            break;
    }
    return lResult;
}

static  BOOL    CreateControl(HWND hWnd,HWND * hListContent)
{
	RECT rect;
	
	GetClientRect(hWnd, &rect);

    * hListContent = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
		rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_MODELIST, NULL, NULL);
    if (* hListContent == NULL )
        return FALSE;
    
    return TRUE;
}

static void GetDefConnValue(UDB_ISPINFO * DefConn, BOOL iFlag)
{
    if(iFlag == TRUE)//default GPRS 
    {
        DefConn->AuthenType = 1; //Normal
        DefConn->DtType = 1; // GPRS
    }
    else // default data call
    {
        DefConn->AuthenType = 1; // Normal
        DefConn->DtType = 0;//data call
        DefConn->DatacallType = 0; //Analogue
        DefConn->MaxDataSpeed = 0; //Automatic
    }
}

 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting / shortcut setting
 *
 * Purpose  :  
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include    "CallShortCutSetting.h"
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
#include    "me_wnd.h"
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "setting.h"
#include    "setup.h"
#include    "mullang.h"
#include    "app.h"


#define CALLBACK_CALLAPP WM_USER + 200
static HWND hAppWnd;
//static HWND hAppGridWnd;
static HWND hParentWnd = NULL;
static HWND hFatherWnd = NULL;
static HWND hApplistWnd = NULL;
static HWND hApplistInFolderWnd = NULL;

static HWND hFrameWin;
static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static int  iCalBackMsg;
static int  iCallbackMessage;
static int  nCurFocus;
static int  nG_CurFocus;
static int  iAppIndex;
static int  iAppIndexDefFocus;
static int  iTitleFlag;
static HWND hWndFocus;
static HWND hGridWndFocus;
static char CurSelAppName[PMNODE_NAMELEN];
static int  CurSelFeatureID = 0;
static TCHAR* SettAppTbl[] =
{
	"Bluetooth",//0
	"Calculator",//1
	"Calendar",//2
	"Clock",//3
	"Contacts",//4
	"Currency converter",//5
	"GPS monitor",//6
	"Help",//7
	"Logs",//8
	"Memory management",//9
	"Messaging",//10
	"Navigation",//11
	"Network",//12
	"Notepad",//13
	"Pictures",//14
//	"PNS",
//	"Profiles",
	"Program manager",//15
	"Settings",//16
//	"Sound profiles",
	"Sounds",//17
	"Wap",//18
	"Diamond",//19
	"GoBang",//20
	"Pao",//21
	"Poker",//22
	"Pushbox",//23
	"DepthBomb",//24
	"",
};
#define  SETT_SHORTCUT_APP_NUM 25

static void ShortCutSet_OnVScroll(HWND hWnd,  UINT wParam);
static void ShortCutSet_InitVScrolls(HWND hWnd,int iItemNum);

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT AppListWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT AppListInFolderWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static int Shortcut_LoadAppList(HWND * hList, int icurSel);
static BOOL CallAppListView(HWND hFrame, UINT iCallbackMsg, const char *appName, int fID);
static BOOL CallAppListInFolder(HWND hFrame, int iappindex, int ideffocus);
//static int  Shortcut_LoadAppListInFolder(HWND * hList, int iGrpIndex);
static BOOL CreateControl(HWND hWnd,HWND* hLeftSoft,HWND* hRightSoft,HWND* hLeftArrow,HWND* hRightArrow);
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus);
static BOOL FindShortCutFocusPos(const char* AppName, int *flag, int *iGrp, int *iSec);
static int Shortcut_LoadPubFeatures(int appIndex, HWND *hList, char *szRetString);
static char* GetAppNameByIndex(int iIndex);
void GetShortcutDisplayName(char *szName, int fID);

void SettListProcessKeyDown(HWND hWnd, HWND *hFocus);
void SettListProcessKeyUp(HWND hWnd, HWND *hFocus);
void SetShortCutAppName(char* szAppName, SHORTCUTKEY keyName);
void ConfigShortCutKey(SHORTCUTKEY keyName, char* szAppName, int fID);
const TCHAR* Sett_GetShortCutAppName(SHORTCUTKEY keyName, int* fID);
int GetAppIndexByName(char* appName);

BOOL    CallShortCutSetting(HWND hwndCall)
{    
    WNDCLASS wc;            
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

    GetClientRect(hFrameWin, &rect);

    hAppWnd = CreateWindow(pClassName,NULL, 
        WS_VISIBLE|WS_CHILD|WS_VSCROLL,  
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,      
        hFrameWin, NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    hParentWnd = hAppWnd;
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Back") );
                    
    SetWindowText(hFrameWin, ML("Shortcuts"));
    SetFocus(hAppWnd);

    return TRUE;
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hLeftSoft,hRightSoft,hLeftArrow,hRightArrow;
    static HWND hFocus = 0;
    LRESULT     lResult;
    static  int iButtonJust;
    int iFlag = 0, iGrpIndex = 0, iSecondIndex = 0;
//    BOOL iret;
	int featureID;
	char appName[PMNODE_NAMELEN];
	int fID;
	char AppName[PMNODE_NAMELEN];
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
		memset(appName, 0, sizeof(appName));
        CreateControl(hWnd,&hLeftSoft,&hRightSoft,&hLeftArrow,&hRightArrow);
		
		strncpy(appName, Sett_GetShortCutAppName(SCK_LSK, &featureID), sizeof(appName));
		GetShortcutDisplayName(appName, featureID);
        SendMessage(hLeftSoft, SSBM_ADDSTRING, 0, (LPARAM)appName );

		strncpy(appName, Sett_GetShortCutAppName(SCK_RSK, &featureID), sizeof(appName));
		GetShortcutDisplayName(appName, featureID);
        SendMessage(hRightSoft, SSBM_ADDSTRING, 0, (LPARAM)appName );

		strncpy(appName, Sett_GetShortCutAppName(SCK_LAK, &featureID), sizeof(appName));
		GetShortcutDisplayName(appName, featureID);
        SendMessage(hLeftArrow, SSBM_ADDSTRING, 0, (LPARAM)appName );
		
		strncpy(appName, Sett_GetShortCutAppName(SCK_RAK, &featureID), sizeof(appName));
		GetShortcutDisplayName(appName, featureID);
        SendMessage(hRightArrow, SSBM_ADDSTRING, 0, (LPARAM)appName);       

        hFocus = hLeftSoft;
        ShortCutSet_InitVScrolls(hWnd,4);
    	break;
    
    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Back") );
        
        SetWindowText(hFrameWin, ML("Shortcuts"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetFocus(hWnd);       
        break;
    
    case WM_VSCROLL:
		ShortCutSet_OnVScroll(hWnd,wParam);
		break;

    case WM_SETFOCUS:
        SetFocus(hFocus);
    	break;

	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
		break;
	case PWM_ACTIVATE:
		if (wParam == WA_INACTIVE)
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
			wKeyCode   = 0;
		}
		break;
    
    case WM_DESTROY:
        hAppWnd = NULL;
        UnregisterClass(pClassName,NULL);
        break;
    
    case WM_KEYDOWN:
		if (RepeatFlag > 0 && wKeyCode != LOWORD(wParam))
		{
			KillTimer(hWnd, TIMER_ID);
			RepeatFlag = 0;
		}

		wKeyCode = LOWORD(wParam);
		RepeatFlag++;

		switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SEL,0);
            break;             
		case VK_DOWN:
            SettListProcessKeyDown(hWnd, &hFocus);            
			
		    SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);

			if ( 1 == RepeatFlag )
			{
	    		SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
			}
		    break;
            
        case VK_UP:
            SettListProcessKeyUp(hWnd, &hFocus);            

		    SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);

			if ( 1 == RepeatFlag )
			{
	    		SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
			}
		    break;
		default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
        }
        break;

    case WM_TIMER:
        OnTimeProcess(hWnd, wParam, hFocus);
        break;
		
    case WM_KEYUP:
		RepeatFlag = 0;
        switch(LOWORD(wParam))
        {
        case VK_UP:
        case VK_DOWN:
			KillTimer(hWnd, TIMER_ID);
            break;
        default:
            break;
        }        
        break;

    case WM_COMMAND:
		memset(AppName, 0, sizeof(AppName));

        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_SEL:
            switch(iButtonJust)
            {
            case IDC_LEFTSOFT:
				strncpy(AppName, Sett_GetShortCutAppName(SCK_LSK, &fID), sizeof(AppName));
				iTitleFlag = IDC_LEFTSOFT;
            	break;
            case IDC_RIGHTSOFT:
				strncpy(AppName, Sett_GetShortCutAppName(SCK_RSK, &fID), sizeof(AppName));
				iTitleFlag = IDC_RIGHTSOFT;
            	break;
            case IDC_LEFTARROW:
				strncpy(AppName, Sett_GetShortCutAppName(SCK_LAK, &fID), sizeof(AppName));
				iTitleFlag = IDC_LEFTARROW;
            	break;
            case IDC_RIGHTARROW:
				strncpy(AppName, Sett_GetShortCutAppName(SCK_RAK, &fID), sizeof(AppName));
				iTitleFlag = IDC_RIGHTARROW;
            	break;
            default:
                break;
            }
			CallAppListView(hFrameWin, CALLBACK_SETKEY, AppName, fID);
        	break;
        case IDC_LEFTSOFT:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_LEFTSOFT;
            break;
        case IDC_RIGHTSOFT:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_RIGHTSOFT;
            break;
        case IDC_LEFTARROW:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_LEFTARROW;
            break;
        case IDC_RIGHTARROW:
            if(HIWORD(wParam) == SSBN_SETFOCUS)
                iButtonJust = IDC_RIGHTARROW;
            break;
        }
        break;


     case CALLBACK_SETKEY:
			
		 strncpy(appName, (const char*)lParam, sizeof(appName));
		 GetShortcutDisplayName(appName, (int)wParam);
         
		 switch(iButtonJust)
         {
         case IDC_LEFTSOFT:
            ConfigShortCutKey(SCK_LSK, (char*)lParam, (int)wParam);
			SendMessage(hLeftSoft, SSBM_DELETESTRING, 0, 0);
            SendMessage(hLeftSoft, SSBM_ADDSTRING, 0, (LPARAM)appName);
            break;
         case IDC_RIGHTSOFT:
            ConfigShortCutKey(SCK_RSK, (char*)lParam, (int)wParam);
            SendMessage(hRightSoft, SSBM_DELETESTRING, 0, 0);
            SendMessage(hRightSoft, SSBM_ADDSTRING, 0, (LPARAM)appName);
            break;
         case IDC_LEFTARROW:
            ConfigShortCutKey(SCK_LAK, (char*)lParam, (int)wParam);
            SendMessage(hLeftArrow, SSBM_DELETESTRING, 0, 0);
            SendMessage(hLeftArrow, SSBM_ADDSTRING, 0,(LPARAM)appName);
            break;
         case IDC_RIGHTARROW:
            ConfigShortCutKey(SCK_RAK, (char*)lParam, (int)wParam);
            SendMessage(hRightArrow, SSBM_DELETESTRING, 0, 0);
            SendMessage(hRightArrow, SSBM_ADDSTRING, 0,(LPARAM)appName);
            break;                     
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

static BOOL CreateControl(HWND hwnd,HWND* hLeftSoft,HWND* hRightSoft,HWND* hLeftArrow,HWND* hRightArrow)
{
	int     xzero=0,yzero=0,iControlH,iControlW;
	RECT    rect;
    GetClientRect(hwnd, &rect);
	iControlH = rect.bottom/3;
    iControlW = rect.right;
    
    * hLeftSoft = CreateWindow( "SPINBOXEX", ML("Left soft key"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER */| WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero, iControlW, iControlH, 
        hwnd, (HMENU)IDC_LEFTSOFT, NULL, NULL);

    if (* hLeftSoft == NULL)
        return FALSE;

    * hRightSoft = CreateWindow( "SPINBOXEX", ML("Right soft key"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER */| WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH, iControlW, iControlH, 
        hwnd, (HMENU)IDC_RIGHTSOFT, NULL, NULL);

    if (* hRightSoft == NULL)
        return FALSE;

    * hLeftArrow = CreateWindow( "SPINBOXEX", ML("Left arrow key"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*2, iControlW, iControlH, 
        hwnd, (HMENU)IDC_LEFTARROW, NULL, NULL);

    if (* hLeftArrow == NULL)
        return FALSE;

    * hRightArrow = CreateWindow( "SPINBOXEX", ML("Right arrow key"), 
        WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST /*| WS_BORDER*/ | WS_TABSTOP | SSBS_LEFT | SSBS_ARROWRIGHT|CS_NOSYSCTRL,
        xzero, yzero+iControlH*3, iControlW, iControlH, 
        hwnd, (HMENU)IDC_RIGHTARROW, NULL, NULL);

    if (* hRightArrow == NULL)
        return FALSE;

return TRUE;
}

static BOOL CallAppListView(HWND hFrame, UINT iCallbackMsg, const char *appName, int fID)
{
    WNDCLASS wc;           
    RECT rect;

    hFrameWin = hFrame;
    iCalBackMsg = iCallbackMsg;
	memset(CurSelAppName, 0, sizeof(CurSelAppName));
	strncpy(CurSelAppName, appName, sizeof(CurSelAppName));
	CurSelFeatureID = fID;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppListWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pAppListClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rect);
    
    hApplistWnd = CreateWindow(pAppListClassName,NULL, 
        WS_VISIBLE | WS_CHILD,  
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,      
        hFrameWin, NULL, NULL, NULL);

    if (NULL == hApplistWnd)
    {
        UnregisterClass(pAppListClassName,NULL);
        return FALSE;
    }
    hFatherWnd= hApplistWnd;
        
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Cancel") );        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;

	switch(iTitleFlag) 
	{
	case IDC_LEFTSOFT:
		SetWindowText(hFrameWin, ML("Left soft key"));
		break;
	case IDC_RIGHTSOFT:
		SetWindowText(hFrameWin, ML("Right soft key"));
		break;
	case IDC_LEFTARROW:
		SetWindowText(hFrameWin, ML("Left arrow key"));
		break;
	case IDC_RIGHTARROW:
		SetWindowText(hFrameWin, ML("Right arrow key"));
		break;
	default:
		break;
	}    
	
    SetFocus(hApplistWnd);

    return TRUE;
}

static LRESULT AppListWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    static HWND hAppList;
    RECT rect;
    static int  iCurIndex = 0, nCurLev;
	static char szRetString[PMNODE_NAMELEN];
	static int iIndex;
    LRESULT     lResult;
    static int nAppNum;
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;

	HDC                 hdc;
    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
/*
		if ( 0 == CurSelFeatureID )
		{
			nCurLev = 1;
		}
		else
		{
			nCurLev = 2;
		}
*/
		nCurLev = 1;
        GetClientRect(hWnd, &rect);

        hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);

        hAppList = CreateWindow("LISTBOX", NULL, 
            WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
            0,0,rect.right - rect.left, rect.bottom - rect.top,
            hWnd, (HMENU)IDC_APPLIST, NULL, NULL);
        if ( hAppList == NULL )
        {
            lResult = FALSE;
            break;
        }
		
/*
		if ( 1 == nCurLev )
        {
*/
			iIndex = GetAppIndexByName(CurSelAppName);
			nAppNum = Shortcut_LoadAppList(&hAppList, iIndex); //set focus on "0"
			Load_Icon_SetupList(hAppList,hIconNormal,hIconSel,nAppNum,iIndex);
/*
		}
		else // nCurLev == 2
		{
			iIndex = GetAppIndexByName(CurSelAppName);
			nAppNum = Shortcut_LoadPubFeatures(iIndex, &hAppList, szRetString);
			Load_Icon_SetupList(hAppList,hIconNormal,hIconSel,nAppNum,CurSelFeatureID - 1);
		}
*/
    	break;
    
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_APPLIST));
    	break;
    
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Cancel") );        
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        if ( 1 == nCurLev )
		{
			switch(iTitleFlag) 
			{
			case IDC_LEFTSOFT:
				SetWindowText(hFrameWin, ML("Left soft key"));
				break;
			case IDC_RIGHTSOFT:
				SetWindowText(hFrameWin, ML("Right soft key"));
				break;
			case IDC_LEFTARROW:
				SetWindowText(hFrameWin, ML("Left arrow key"));
				break;
			case IDC_RIGHTARROW:
				SetWindowText(hFrameWin, ML("Right arrow key"));
				break;
			default:
				break;
			}    
		}
		else //if ( 2 == nCurLev )
			SetWindowText(hFrameWin, ML("Add shortcut"));
        break;

    case WM_DESTROY:
	    DeleteObject(hIconNormal);
	    DeleteObject(hIconSel);
		KillTimer(hWnd, TIMER_ASURE);
        hApplistWnd = NULL;
        hFatherWnd = NULL;
        UnregisterClass(pAppListClassName,NULL);
        break;
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
    case WM_KEYDOWN:
		switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_F5:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SEL,0);
            break;             
		default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
        }
        break;
    case WM_COMMAND:     
        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_SEL:                   
            iCurIndex = SendMessage(hAppList, LB_GETCURSEL, 0, 0);
            Load_Icon_SetupList(hAppList,hIconNormal,hIconSel,nAppNum,iCurIndex);
            SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
            break;
        }
        break;
	
    case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
            KillTimer(hWnd, TIMER_ASURE);
			if ( 2 == nCurLev )
			{
				PLXTipsWin(NULL, NULL, NULL, ML("Shortcut added"), NULL, Notify_Success,ML("Ok"),0,WAITTIMEOUT);
                PostMessage(hParentWnd, iCalBackMsg, iCurIndex+1, (LPARAM)szRetString);
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
				PostMessage(hWnd, WM_CLOSE, 0, 0);	
				break;
			}			
			else if ((nAppNum = Shortcut_LoadPubFeatures(iCurIndex, &hAppList, szRetString)) > 0)
			{
				SetWindowText(hFrameWin, ML("Add shortcut"));
				if (iCurIndex == iIndex)
				{
					Load_Icon_SetupList(hAppList,hIconNormal,hIconSel,nAppNum,CurSelFeatureID - 1);
					SendMessage(hAppList, LB_SETCURSEL, CurSelFeatureID - 1, 0);
				}
				else
				{
					Load_Icon_SetupList(hAppList,hIconNormal,hIconSel,nAppNum,-1);
					SendMessage(hAppList, LB_SETCURSEL, 0, 0);
				}

				nCurLev++;
			}
			else //nAppNum == 0
			{
				strncpy(szRetString, GetAppNameByIndex(iCurIndex), sizeof(szRetString));
				PLXTipsWin(NULL, NULL, NULL, ML("Shortcut added"), NULL, Notify_Success,ML("Ok"),0,WAITTIMEOUT);
                PostMessage(hParentWnd, iCalBackMsg, 0, (LPARAM)szRetString);
				SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
				PostMessage(hWnd, WM_CLOSE, 0, 0);				
			}
			break;
		}
		break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
int GetAppIndexByName(char* appName)
{
	int i=0;

	if (appName == NULL)
		return -1;
	
	while (SettAppTbl[i] != "")
	{
		if (strcmp(appName, SettAppTbl[i]) == 0)
		{
			return i;
		}
		i++;
	}

	return 0;
}

static char* GetAppNameByIndex(int iIndex)
{
	if (iIndex < SETT_SHORTCUT_APP_NUM && iIndex >= 0) 
		return SettAppTbl[iIndex];
	else
		return NULL;		
}

void GetShortcutDisplayName(char *szName, int fID)
{
	int iIndex;
/*
	if ( 0 == fID )
	{
		return;
	}
*/

	iIndex = GetAppIndexByName(szName);
	switch(iIndex)
	{
	case 0://Bluetooth
		if ( 1 == fID )
		{
			strncpy(szName, ML("Bt on/off"), PMNODE_NAMELEN);
		}
		else if ( 2 == fID )
		{
			strncpy(szName, ML("Bt search"), PMNODE_NAMELEN);			
		}
		if (0 == fID)
		{
			strncpy(szName, ML("Bluetooth"), PMNODE_NAMELEN);		

		}
		break;
	case 1://Calculator
		strncpy(szName, ML("Calculator"), PMNODE_NAMELEN);
		break;
	case 2://Calendar
		if ( 1 == fID )
		{
			strncpy(szName, ML("Cal. list"), PMNODE_NAMELEN);
		}
		else if (0 == fID)
		{
			strncpy(szName, ML("Calendar"), PMNODE_NAMELEN);
		}
		break;
	case 3://Clock
		if ( 1 == fID )
		{
			strncpy(szName, ML("Alarms"), PMNODE_NAMELEN);			
		}
		else if (0 == fID)
		{
			strncpy(szName, ML("Clock"), PMNODE_NAMELEN);
		}
		break;
	case 4://Contacts
		strncpy(szName, ML("Contacts"), PMNODE_NAMELEN);
		break;
	case 5://Currency converter
		strncpy(szName, ML("Currency converter"), PMNODE_NAMELEN);
		break;
	case 6://GPS monitor
		switch(fID)
		{
		case 0:
			strncpy(szName, ML("GPS"), PMNODE_NAMELEN);			
			break;
		case 1:
			strncpy(szName, ML("Satellites"), PMNODE_NAMELEN);			
			break;
		case 2:
			strncpy(szName, ML("Pos.refr."), PMNODE_NAMELEN);			
			break;
		case 3:
			strncpy(szName, ML("Pos.view"), PMNODE_NAMELEN);			
			break;
		case 4:
			strncpy(szName, ML("Compass"), PMNODE_NAMELEN);			
			break;
		default:
			break;
		}
		break;
	case 7://Help
		strncpy(szName, ML("Help"), PMNODE_NAMELEN);
		break;
	case 8://Logs
		switch(fID)
		{
		case 0:
			strncpy(szName, ML("Logs"), PMNODE_NAMELEN);			
			break;
		case 1:
			strncpy(szName, ML("Missed"), PMNODE_NAMELEN);			
			break;
		case 2:
			strncpy(szName, ML("Counters"), PMNODE_NAMELEN);			
			break;
		case 3:
			strncpy(szName, ML("Comms."), PMNODE_NAMELEN);			
			break;
		default:
			break;
		}
		break;
	case 9://Memory management
		strncpy(szName, ML("Memory management"), PMNODE_NAMELEN);
		break;
	case 10://Messaging
		switch(fID)
		{
		case 0:
			strncpy(szName, ML("Msg."), PMNODE_NAMELEN);			
			break;
		case 1:
			strncpy(szName, ML("Inbox"), PMNODE_NAMELEN);			
			break;
		case 2:
			strncpy(szName, ML("Mailbox"), PMNODE_NAMELEN);			
			break;
		case 3:
			strncpy(szName, ML("New SMS"), PMNODE_NAMELEN);			
			break;
		case 4:
			strncpy(szName, ML("New e-mail"), PMNODE_NAMELEN);			
			break;
		case 5:
			strncpy(szName, ML("New MMS"), PMNODE_NAMELEN);			
			break;
		default:
			break;
		}
		break;
	
	case 11: //Navigation client
		if ( 1 == fID )
		{
			strncpy(szName, ML("Map view"), PMNODE_NAMELEN);			
		}
		else if ( 2 == fID )
		{
			strncpy(szName, ML("Navigate"), PMNODE_NAMELEN);		
		}
		else if (0 == fID)
		{
			strncpy(szName, ML("Navigation"), PMNODE_NAMELEN);		
		}
		
		break;
	case 12://Network services
		if ( 1 == fID )
		{
			strncpy(szName, ML("Call forw."), PMNODE_NAMELEN);			
		}
		else if (0 == fID)
		{
			strncpy(szName, ML("Net.serv."), PMNODE_NAMELEN);			
		}
		break;
	case 13://Notepad
		if ( 1 == fID )
		{
			strncpy(szName, ML("New note"), PMNODE_NAMELEN);			
		}
		else if (0 == fID)
		{
			strncpy(szName, ML("Notepad"), PMNODE_NAMELEN);			
		}
		break;
	case 14://Pictures
		strncpy(szName, ML("Pictures"), PMNODE_NAMELEN);
		break;
	case 15://Program manager
		strncpy(szName, ML("Program manager"), PMNODE_NAMELEN);
		break;
	case 16://Settings
		if ( 1 == fID )
		{
			strncpy(szName, ML("S.profile"), PMNODE_NAMELEN);			
		}
		else if (0 == fID)
		{
			strncpy(szName, ML("Settings"), PMNODE_NAMELEN);			
		}
		break;
	case 17://sounds
		strncpy(szName, ML("sounds"), PMNODE_NAMELEN);			
		break;
	case 18://Wap browser
		if ( 1 == fID )
		{
			strncpy(szName, ML("Bookmarks"), PMNODE_NAMELEN);			
		}
		else if (0 == fID)
		{
			strncpy(szName, ML("Browser"), PMNODE_NAMELEN);			
		}
		break;
	case 19: //Diamond
		strncpy(szName, ML("Diamond"), PMNODE_NAMELEN);			
		break;
	case 20: //GoBang
		strncpy(szName, ML("GoBang"), PMNODE_NAMELEN);			
		break;
	case 21: //Pao
		strncpy(szName, ML("Pao"), PMNODE_NAMELEN);			
		break;
	case 22: //Poker
		strncpy(szName, ML("Poker"), PMNODE_NAMELEN);			
		break;
	case 23: //Pushbox
		strncpy(szName, ML("Pushbox"), PMNODE_NAMELEN);			
		break;
	case 24: //DepthBomb
		strncpy(szName, ML("DepthBomb"), PMNODE_NAMELEN);			
		break;
	default:
		break;
	}
}

static int Shortcut_LoadAppList(HWND * hList, int iCurSel)
{
	SendMessage(*hList, LB_RESETCONTENT, 0, 0);
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Bluetooth"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Calculator"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Calendar"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Clock"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Contacts"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Currency converter"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("GPS"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Help"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Logs"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Memory management"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Msg."));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Navigation"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Net.serv."));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Notepad"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Pictures"));
//	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("PNS"));
//	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Profiles"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Program manager"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Settings"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Sounds"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Browser"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Diamond"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("GoBang"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Pao"));
 	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Poker"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Pushbox"));
	SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("DepthBomb"));
    PostMessage(*hList, LB_SETCURSEL, iCurSel, 0);
	return SETT_SHORTCUT_APP_NUM;
}


static int Shortcut_LoadPubFeatures(int appIndex, HWND *hList, char *szRetString)
{
	int iretFeatNum = 0;
	SendMessage(*hList, LB_RESETCONTENT, 0, 0);

	switch(appIndex)
	{
	case 0://Bluetooth
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Bt on/off"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Bt search"));
		strcpy(szRetString, "Bluetooth");
		iretFeatNum = 2;
		break;
	case 1://Calculator
		strcpy(szRetString, "Calculator");
		break;
	case 2://Calendar
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Cal. list"));
		strcpy(szRetString, "Calendar");
		iretFeatNum = 1;
		break;
	case 3://Clock
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Alarms"));
		strcpy(szRetString, "Clock");
		iretFeatNum = 1;
		break;
	case 4://Contacts
		strcpy(szRetString, "Contacts");
		break;
	case 5://Currency converter
		strcpy(szRetString, "Currency converter");
		break;
	case 6://GPS monitor
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Satellites"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Pos.refr."));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Pos.view"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Compass"));
		strcpy(szRetString, "GPS monitor");
		iretFeatNum = 4;
		break;
	case 7://Help
		strcpy(szRetString, "Help");
		break;
	case 8://Logs
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Missed"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Counters"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Comms."));
		strcpy(szRetString, "Logs");
		iretFeatNum = 3;
		break;
	case 9://Memory management
		strcpy(szRetString, "Memory");
		break;
	case 10://Messaging
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Inbox"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Mailbox"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("New SMS"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("New e-mail"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("New MMS"));
		strcpy(szRetString, "Messaging");
		iretFeatNum = 5;
		break;

	case 11://navigation client
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Map view"));
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Navigate"));
		strcpy(szRetString, "Navigation");
		iretFeatNum = 2;
		break;

	case 12://Network services
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Call forw."));
		strcpy(szRetString, "Network");
		iretFeatNum = 1;
		break;
	case 13://Notepad
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("New note"));
		strcpy(szRetString, "Notepad");
		iretFeatNum = 1;
		break;
	case 14://Pictures
		strcpy(szRetString, "Pictures");
		break;
	case 15://Program manager
		strcpy(szRetString, "Program manager");
		break;
	case 16://Settings
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("S.profile"));
		strcpy(szRetString, "Settings");
		iretFeatNum = 1;
		break;
	case 17://Sounds
		strcpy(szRetString, "Sounds");
		break;
	case 18://Wap browser
		SendMessage(*hList, LB_ADDSTRING, 0, (LPARAM)ML("Bookmarks"));
		strcpy(szRetString, "Wap");
		iretFeatNum = 1;
		break;
	case 19: //games
		strcpy(szRetString, "Diamond");
		break;
	case 20: //games
		strcpy(szRetString, "GoBang");
		break;
	case 21: //games
		strcpy(szRetString, "Pao");
		break;
	case 22: //games
		strcpy(szRetString, "Poker");
		break;
	case 23: //games
		strcpy(szRetString, "Pushbox");
		break;
	case 24: //games
		strcpy(szRetString, "DepthBomb");
		break;
	default:
		break;
	}

	return iretFeatNum;
}


/*
static int  Shortcut_LoadAppListInFolder(HWND * hList, int iGrpIndex)
{
    int iFirst = 0;
    SendMessage(* hList, LB_RESETCONTENT, 0, 0);
    while(GrpTable[iGrpIndex]->AppTable[iFirst] != NULL)
        {
            SendMessage(* hList, LB_ADDSTRING, 0, (LPARAM)ML(GrpTable[iGrpIndex]->AppTable[iFirst]->achName));
            iFirst++;
        }
    SendMessage(* hList, LB_SETCURSEL, 0/ *iAppIndexDefFocus* /, 0);
    return iFirst;
}
*/

/*
BOOL FindShortCutFocusPos(const char* AppName, int *flag, int *iGrp, int *iSec)//flag: 0 First level, 1, Second level
{
    int iFirst = 0;
    int iSecond = 0;
    while ( GrpTable[iFirst] != NULL )
    {
        if (GrpTable[iFirst]->nType == DIR_TYPE)
        {
            iSecond = 0;
            while (GrpTable[iFirst]->AppTable[iSecond] != NULL)
            {
                if ( 0 == strcmp(AppName, GrpTable[iFirst]->AppTable[iSecond]->achName) )
                {
                    *flag =  1;
                    *iGrp = iFirst;
                    *iSec = iSecond;
                    return TRUE;
                }
                iSecond++;
            }
           
        }

        if ( 0 == strcmp(AppName, GrpTable[iFirst]->achName) )
        {
            *flag = 0;
            *iGrp = iFirst;
            return TRUE;
        }        
        iFirst++;
    }

    return FALSE;
}
*/




/*********************************************************************\
* Function        ShortCutSet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void ShortCutSet_InitVScrolls(HWND hWnd,int iItemNum)
{
    static SCROLLINFO   vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (iItemNum-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nCurFocus =0;
	return;
}

/*********************************************************************\
* Function        ShortCutSet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/


static void ShortCutSet_OnVScroll(HWND hWnd,  UINT wParam)
{
    static int  nY;
    static RECT rcClient;
    static SCROLLINFO      vsi;

    switch(wParam)
    {
    case SB_LINEDOWN:
		memset(&vsi, 0, sizeof(SCROLLINFO));
		
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus++;
		
        if(nCurFocus > vsi.nMax)	
        {
            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);	
            UpdateWindow(hWnd);
            nCurFocus = 0;
			vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
	
        if(((int)(vsi.nPos + vsi.nPage - 1) <= nCurFocus) && nCurFocus != vsi.nMax)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,-nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
		break;
		
    case SB_LINEUP:
		
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
		
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nCurFocus--;
		
        if(nCurFocus < vsi.nMin)	
        {
            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
			
            UpdateWindow(hWnd);
			nCurFocus = vsi.nMax;
		    vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;	
        }
		
        if((int)vsi.nPos == nCurFocus && nCurFocus != vsi.nMin)	//modified for UISG
        { 
            ScrollWindow(hWnd,0,nY,NULL,NULL);
            UpdateWindow(hWnd);
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   
            break;	
        }
		break;
    case SB_PAGEDOWN:
        break;
    case SB_PAGEUP:
        break;
    default:
        break;	
    }	
}
static void OnTimeProcess(HWND hWnd, WPARAM wParam, HWND hFocus)
{
	    switch(wParam)
		{
		case TIMER_ID:
            if (1 == RepeatFlag)
            {
                KillTimer(hWnd, TIMER_ID);
                SetTimer(hWnd, TIMER_ID, ET_REPEAT_LATER, NULL);
            }
			keybd_event(wKeyCode, 0, 0, 0);
			break;
       default:
			KillTimer(hWnd, wParam);
            break;
		}

}


/*
static BOOL CallShortCutAppGridView(HWND hwndCall)
{
    WNDCLASS wc;            

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppGridWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pGridClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    hAppGridWnd = CreateWindow(pGridClassName,"", 
        WS_CAPTION|WS_BORDER |PWS_STATICBAR|WS_VSCROLL,  
        PLX_WIN_POSITION,
        hwndCall, NULL, NULL, NULL);

    if (NULL == hAppGridWnd)
    {
        UnregisterClass(pGridClassName,NULL);
        return FALSE;
    }

    ShowWindow(hAppGridWnd, SW_SHOW);
    UpdateWindow(hAppGridWnd); 

    return TRUE;
}

*/
/*
static LRESULT AppGridWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT     lResult;
    int         nOldmode;
    RECT		rcIcon, rcText, rcFocus;
    COLORREF    bkColor, oldTxtColor;
    HDC         hdc;  
    BOOL        bFlag=TRUE;
    static      int     iPos=0,iStaticFirst=0;
    static      int     iAppFocus = 0;
    int         iCod,iFirst,nTempAppNum = 14;//according to "app.c"(2005.06.30) (Jesson)
    static      int         nRowNum;
    PAINTSTRUCT ps;

    lResult = (LRESULT)TRUE;

    switch ( wMsgCmd )
    {
    case WM_CREATE:
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
                (WPARAM)IDC_BUTTON_BACK,(LPARAM)ML("Back") );
        if(nTempAppNum%3 != 0)
            nRowNum = (nTempAppNum - (nTempAppNum%3))/3 + 1;
        else
            nRowNum = nTempAppNum/3;
        AppGridView_InitVScrolls(hWnd, nRowNum);
       break;
    case WM_PAINT:
        hdc         = BeginPaint(hWnd,&ps);
        nOldmode = GetBkMode(hdc);
//        SetBkMode(hdc, NEWTRANSPARENT);
        bkColor = SetBkColor(hdc, RGB(255,255,255));
        oldTxtColor = GetTextColor(hdc);
        iFirst = iStaticFirst;
        while (	GrpTable[iFirst] != NULL && iFirst < iStaticFirst + 9)
        {		
	
	        if(GrpTable[iFirst]->nType == APP_TYPE || GrpTable[iFirst]->nType == DIR_TYPE / *&& (GrpTable[i]->attrib & HIDE_APP)* /)
	        {
                iCod = iFirst-iStaticFirst;
		
		        DeskGetRect(&rcIcon,&rcText,iCod);
		
		        rcIcon.top = rcIcon.top +iPos;
		        rcText.top = rcText.top + iPos;
		        rcText.bottom = rcText.bottom + iPos;
		
		    if( iFirst != iAppFocus)
		    {
			    SetTextColor(hdc, oldTxtColor);
			    SetBkMode(hdc, ALPHATRANSPARENT);
			    DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, GrpTable[iFirst]->strIconName,SRCCOPY);
			    SetBkMode(hdc, TRANSPARENT);
			    DrawText(hdc, ML(GrpTable[iFirst]->achName), -1, &rcText, DT_CENTER | DT_VCENTER);
			
		    }
		    else
		    {
//			    char strIconName[PMICON_NAMELEN];
			
			    SetTextColor(hdc, RGB(255,255,255));
			    SetRect(&rcFocus, rcIcon.left -6, rcIcon.top - 3,
				    rcIcon.right+6, rcText.bottom+ 6);
			
			    DrawImageFromFile(hdc,"/rom/progman/select.gif", rcFocus.left, rcFocus.top, SRCCOPY);
			
//			    memset(strIconName, 0, PMICON_NAMELEN);
			
			    SetBkMode(hdc, ALPHATRANSPARENT);
			    DrawBitmapFromFile(hdc, rcIcon.left, rcIcon.top, GrpTable[iFirst]->strIconName,SRCCOPY);
			    SetBkMode(hdc, TRANSPARENT);
			    DrawText(hdc, ML(GrpTable[iFirst]->achName), -1, &rcText, DT_CENTER | DT_VCENTER);
			
		    }        
		
	        }
	        iFirst++;
        }

        SetTextColor(hdc, oldTxtColor);
        SetBkColor(hdc, bkColor);
        SetBkMode(hdc, nOldmode);
        EndPaint(hWnd,&ps);        


        break;
    case WM_VSCROLL:
        AppGridView_OnVScroll(hWnd,wParam);        
        break;
/ *
    case WM_ACTIVATE:
        break;
* /
    case WM_DESTROY:
        hAppGridWnd = NULL;
        UnregisterClass(pGridClassName,NULL);
        break;
    case WM_KEYDOWN:
		switch(LOWORD(wParam))
        {
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_BACK,0);
            break;
        case VK_F5:
            break;
        
        case VK_UP:
            if((iAppFocus-3) >= 0) //not reach the first row
            {
                if(iAppFocus == iStaticFirst || iAppFocus == iStaticFirst+1 || iAppFocus == iStaticFirst+2)                
                    iStaticFirst = iAppFocus - iAppFocus%3 - 3;
                iAppFocus = iAppFocus - 3;
            }
            else //reach the first row
            {
                iAppFocus = iAppFocus + (nRowNum - 1)*3;
                if(iAppFocus >= nTempAppNum)
                    iAppFocus = iAppFocus - 3;

                iStaticFirst = nTempAppNum - nTempAppNum%3 - 6;
            }

            InvalidateRect(hWnd,NULL,TRUE);

		    SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
		    break;
        
        case VK_DOWN:
            if((iAppFocus+3)>=nTempAppNum)//reach the last row
            {
                iAppFocus = iAppFocus%3;
                iStaticFirst = 0;
            }            
            else //not reach the last row
            {                
                if(iAppFocus == iStaticFirst+6 || iAppFocus == iStaticFirst+7 || iAppFocus == iStaticFirst+8)
                    iStaticFirst = iAppFocus - iAppFocus%3 - 3;
                iAppFocus = iAppFocus+3;
            }
            InvalidateRect(hWnd,NULL,TRUE);

		    SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
		    break;
        
        case VK_LEFT:
            if(iAppFocus != 0)
            {
                if(iAppFocus%3 != 0)
                    iAppFocus = iAppFocus - 1;
/ *
                else
* /


            }
            else
            {
                iAppFocus = nTempAppNum - 1;
                iStaticFirst = nTempAppNum - nTempAppNum%3 - 6;
            }

            InvalidateRect(hWnd,NULL,TRUE);
            
            break;
        
        case VK_RIGHT:
            
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
        }
        break;
    case WM_COMMAND:
        switch(LOWORD( wParam )) 
        {
        case IDC_BUTTON_BACK:
            DestroyWindow(hWnd);
        	break;
        }
        break;
    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
*/


/*********************************************************************\
* Function        AppGridView_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
/*
static void AppGridView_InitVScrolls(HWND hWnd,int iRowNum)
{
    static SCROLLINFO   vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (iRowNum-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nG_CurFocus =0;
	return;
}
*/
/*********************************************************************\
* Function        AppGridView_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
/*

static void AppGridView_OnVScroll(HWND hWnd,  UINT wParam)
{
    static int  nY;
    static RECT rcClient;
    static SCROLLINFO      vsi;

    switch(wParam)
    {
    case SB_LINEDOWN:
		memset(&vsi, 0, sizeof(SCROLLINFO));
		
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nG_CurFocus++;
		
        if(nG_CurFocus > vsi.nMax)	
        {
//            ScrollWindow(hWnd,0,(vsi.nMax-vsi.nPage+1)*nY,NULL,NULL);	
//            UpdateWindow(hWnd);
            nG_CurFocus = 0;
			vsi.nPos = 0;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
	
        if((int)(vsi.nPos + vsi.nPage) <= nG_CurFocus)	
        { 
//            ScrollWindow(hWnd,0,-nY,NULL,NULL);
//            UpdateWindow(hWnd);
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
		break;
		
    case SB_LINEUP:
		
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
		
        nY = (rcClient.bottom - rcClient.top)/3;
		
        nG_CurFocus--;
		
        if(nG_CurFocus < vsi.nMin)	
        {
//            ScrollWindow(hWnd,0,(vsi.nPage-vsi.nMax-1)*nY,NULL,NULL);
			
//            UpdateWindow(hWnd);
			nG_CurFocus = vsi.nMax;
		    vsi.nPos = vsi.nMax-vsi.nPage+1;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;	
        }
		
        if((int)vsi.nPos > nG_CurFocus)	
        { 
//            ScrollWindow(hWnd,0,nY,NULL,NULL);
//            UpdateWindow(hWnd);
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   
            break;	
        }
		break;
    case SB_PAGEDOWN:
        break;
    case SB_PAGEUP:
        break;
    default:
        break;	
    }	
}
*/

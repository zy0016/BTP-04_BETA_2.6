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
#include "setting.h"
#include "pubapp.h"
#include "hpimage.h"
#include "dialmgr.h"
#include "WIEsetting.h"
#include "UrlRatio.h"
#include "DownLoad.h"
#include "wiego.h"
#include "WIEBK.h"
#include "wievisited.h"
#include "WieSaveas.h"
#include "PWBE.h"
#include "WIE.h"
#include "AuthProc.h"
#include "UIData.h"
#include "func.h"
#include "wappushapi.h"
#include "imesys.h"
#include "dirent.h"
#include "unistd.h"
#include "sys\stat.h"
#include "pm\AppFile.h"
#include "project\plx_pdaex.h"

//position
#define WGIF_X                      130
#define WIE_STATIC_HEIGHT           20    
#define RC_WMLBROW_LT               0                
#define RC_WMLBROW_TOP              0
#define RC_WMLBROW_RT               176
#define RC_WMLBROW_BTM              150
#define RC_STATELINE_Y              (RC_WMLBROW_BTM+1)
#define RC_STATETEXT_LT             2
#define RC_STATETEXT_TOP            (RC_STATELINE_Y+3)
#define RC_STATETEXT_RT             RC_WMLBROW_RT
#define RC_STATETEXT_BTM            (RC_STATETEXT_TOP+20)

//status prioty
#define MSG_HIGH					5          //all network status
#define MSG_TOCOMMON				4        
#define MSG_COMMON					3          //stop0
#define MSG_TOLOW					2          //download status
#define MSG_LOW						1          //request end
#define MSG_ERRLOW					0
#define WIE_IMG_DEFAULT_WIDTH		16
#define WIE_IMG_DEFAULT_HEIGHT		16

//image
#define WIE_GIF_NORMAL				"ROM:/wml/Wapgif.gif"
#define WIE_GIF_SECURE				"ROM:/wml/Wapsgif.gif"
#define WIE_IMG_DEFAULT_WAIT		"ROM:/wml/wml_wsb.bmp"
#define WIE_IMG_DEFAULT_FAIL		"ROM:/wml/wml_bsf.bmp"
#define WIE_IMG_PICTOGRAMPATH		"ROM:/wml/core/"
#define WIE_IMG_CONNECT				"/rom/wml/wap_connected.bmp"
#define WIE_IMG_DISCONNECT			"/rom/wml/wap_disconnected.bmp"
#define WIE_IMG_TRANS				"/rom/wml/wap_transferring.gif"
#define WIE_IMG_NOTACTIVE			"/rom/wml/not_available.bmp"

//blank page
#define WIE_ABOUTBLANK				"/rom/wml/aboutblank.xhtml"

WMLCFGFILE  curCfgFile;

//variable
static HINSTANCE hInstance;
HWND hWapFrame = NULL;
static RECT rClient;
HMENU hMenu = NULL;
static HMENU hSaveMenu = NULL;    
static HMENU hPageMenu = NULL;
static const char * pWapClassName = "WapBrowserFrameClass";
static int ibExiting, ibSelExit;
static HGIFANIMATE hActiveGif;
HWND hwndIEApp = NULL;
HBITMAP hwmlnsbmp, hwmlnsbmpfs, hwmlnsbmpps, hwmlconnect, hwmldisconnect;
HBROWSER pUIBrowser;
static char sShowinfo[100] = "";
static char WIE_szGifFile[20];
static BOOL bDisconnectbymenu = FALSE;
static char sztmpTitle[URLNAMELEN] = "";
static BOOL bsave = FALSE;
static char szUrlTitle[40];
static char *requesturl;
//if a url request by the outer app
BOOL bCallUser = FALSE; 
//char disconchar[URLNAMELEN] = "";
//for hdc to the show window
HDC hViewDC = NULL;
//BOOL bBlankpage = FALSE;
//directly enter the push message
BOOL bPushenter = FALSE;
BOOL bPushexit = FALSE;
///////////////////////////////////////
//for shortcut to open a url
BOOL bUrlFromFile = FALSE;
//directly enter the bookmark
BOOL bBookenter = FALSE;
//keep the bookmark number the same as totlebm
int InitBMT = 0;
extern int totalbm;
/////////////////////////////////////
//if the wapbrowser is online
BOOL bConnectted;
//for process bar
int nAllDataSize;
int nInDataSize;
HWND hProcess;
int ps;
BOOL bMedia = TRUE;
BOOL bSet = TRUE;
//////////////////////////////////////
//for font size
HFONT hViewFont;
static DWORD ScreenSave;
//if history is enable, to show right button text
BOOL bHistory = FALSE;
//create main window
HWND hViewWnd = NULL;
//the struct pass to Hopen_DocCtrl.c
UIDATA brsUIData;
//to decide the text shown on right button
int rbtnTxtID = 0; //0 Exit 1 Cancel 2 Back
//if enable save page
static BOOL bSaveSource = FALSE;
//if enable save image
static BOOL bSavePicture = FALSE;
//save the text on left button
char LBtnInfo[10] = {0};
//to close all the window in wapbrowser
static HWND hSettingWnd = NULL;


//struct of gateway and port info
typedef struct tagSETINFO
{
    char MMSURL[511+1];
	char MMSIP[31+1];
	char MMSPORT[31+1];
	char WAPIP[31+1];
	char WAPPORT[4+1];
}SETINFO, *PSETINFO;
static SETINFO SetupInfo; 

//registe list of functions of GUI callback
static LIBLIST_GUI g_GUIFuncs =
{
	WB_DrawLine,
	WB_Rectangle,
	WB_Ellipse,
	WB_Polygon,

	WB_SetTextStyle,
	WB_DrawText,
	WB_GetTextInfo,
	WB_InputMethod,//EditText
	
	WB_DrawImageEx,
	WB_EndImageEx,
	WB_GetImageInfo,

	WIE_WapSelectOption,//SelectOption

	NULL,//StartRing
	NULL,//StopRing

	WB_BeginDisplay,//NULL,
	WB_EndDisplay//NULL
};


//local functions
static LRESULT IEAppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL CreateButtonsINIEW(HWND hWnd, Color_Scheme uClCfg);
static int WIE_ExitProc();
static void OnWieAppActive(HWND hParent, LPARAM lParam, RECT rClient, WPARAM wParam);
static void WIE_EnableSaveSource(BOOL ibenable);
static int WIE_Exit_Prompt();
static void WIE_SetShowGifFile(const char* szFileName);
static void StartWapGif();
static void EndWapGif();
static void WIE_EnableSaveImg(BOOL ibenable);
static int WIE_CutString(const char* strSrc, char* strDes, int nDesLen, int len);
static int WIE_GetStrRatio(char* strDes, int nDesLen, int nNumerator, int nDenominator);
void WML_BackOrExit(int bPrev, BOOL bHis);
DWORD WAP_RoomChecker(void);
BOOL WAP_ShowConnectornot(HWND hWnd, UINT msg, BOOL bConOrDis);
//////////////////////////////////////////////////////////////////////////

//for push message use
extern void WAP_PushMessage(HWND hWnd);
extern int WP_PushSIData(void *data);
extern BOOL WP_OnPushDetail(HWND hParent, PUSHFILERECORD record);
//////////////////////////////////////////////////////////////////////////

//for progman's function
extern BOOL CallAppEntryEx (PSTR pAppName, WPARAM wparam, LPARAM lparam);
//for process bar
BOOL ProcessBarCreate(HWND hFrame, HWND hWnd);
static LRESULT ProcessBarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawProcess(int process);
//////////////////////////////////////////////////////////////////////////
//for shortcut
BOOL WML_OnShortcut(void);
static LRESULT WIEShortcutProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
BOOL WIE_SaveShortcut(const char* DIR);
BOOL WIE_GetSTNum(struct dirent *dirp, DIR *dp);
//////////////////////////////////////////////////////////////////////////



DWORD WIE_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
    DWORD dwRet = TRUE;
    
    switch (nCode)
    {
    case APP_INIT:
        hInstance = pInstance;
		PWBE_InitPUSH(WP_PushSIData, NULL, NULL);
		DlmNotify(PS_NEWMSG, 3);
        break; 
    
    case APP_ACTIVE:
		ScreenSave = DlmNotify(PMM_NEWS_ENABLE_SCRSAVE, FALSE);
		if (IsWindow(hWapFrame))
		{
			//for frame exist condition, push directly enter
			if(wParam == PUSHMSG)
			{
				WAP_PushMessage(hWapFrame);
				bPushenter = TRUE;
				bPushexit = TRUE;
				
				ShowWindow(hWapFrame, SW_SHOW); 
				ShowWindow(hProcess, SW_SHOWNA);
				UpdateWindow(hWapFrame);
			}
			//for frame exist condition, bookmark directly enter
			else if(lParam == 1)
			{
				GetClientRect(hWapFrame, &rClient);
				On_IDM_BM(hWapFrame, rClient);

				ShowWindow(hWapFrame, SW_SHOW); 
				ShowWindow(hProcess, SW_SHOWNA);
				UpdateWindow(hWapFrame);
			}
			else
			{
				ShowWindow(hWapFrame, SW_SHOW);
				ShowWindow(hProcess, SW_SHOWNA);
				UpdateWindow(hWapFrame);
			}	
		}
		else
		{				
			hWapFrame = CreateFrameWindow(WS_CAPTION|PWS_STATICBAR|WS_CLIPCHILDREN);
			SendMessage(hWapFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(WIE_IDM_MENU_EXIT, 0), (LPARAM)"WAP");
			SendMessage(hWapFrame, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(WIE_IDM_MENU_OK, 1), (LPARAM)"WAP");
			GetClientRect(hWapFrame, &rClient);

			//push directly enter
			if(wParam == PUSHMSG)
			{
				bPushenter = TRUE;
				bPushexit = TRUE;
			}
			//bookmark directly enter
			else if(lParam == 1)
			{
				bBookenter = TRUE;
			}
			if(!bCallUser)
				//if user right this app, go to homepage
				OnWieAppActive(hWapFrame, WIE_USER_CALLDEF, rClient, wParam);
			else
				//if user called by some reason, go to the requested page
				OnWieAppActive(hWapFrame, WIE_USER_CALL, rClient, wParam);

			ShowWindow(hWapFrame, SW_SHOW); 
			UpdateWindow(hWapFrame);
		}                    
        break;
            
    case APP_INACTIVE:
		//for shortcut exit and enter again
		if(requesturl != NULL)
		{
			free(requesturl);
			requesturl = NULL;
		}
		ShowWindow(hWapFrame, SW_HIDE);
		ShowWindow(hProcess, SW_HIDE);
//		bPushexit = FALSE;
        break;
            
    default:
        break;
    }
    
    return dwRet;
}

HWND GetWAPFrameWindow()//get the wapbrowser frame handle
{
	return hWapFrame;
}

HMENU GetWAPMainMenu()
{
	return hMenu;
}

static void OnWieAppActive(HWND hParent, LPARAM lParam, RECT rClient, WPARAM wParam)
{	
	char szTmp[URLNAMELEN] = "";
	
    if (hwndIEApp)
    {        
        ShowWindow(hwndIEApp, SW_SHOW);            
        UpdateWindow(hwndIEApp);
        BringWindowToTop(hwndIEApp);
    }
    else
    {
        WNDCLASS wc;		    
        
        wc.style         = 0;
        wc.lpfnWndProc   = IEAppWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = NULL;
        wc.hIcon         = NULL;
        wc.hCursor       = NULL;
        wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName = "WMLIeWndClass";
        
        if (!RegisterClass(&wc))
            return;        
        
		hMenu = CreateMenu();
		hSaveMenu = CreateMenu();
		hPageMenu = CreateMenu();
		InsertMenu(hMenu, 0, MF_BYPOSITION, IDM_WML_GOTOURL, WML_GOTOURL); 
		InsertMenu(hMenu, 1, MF_BYPOSITION, WIE_IDM_REFRESH, WML_REFRESH);
		if(bsave)
		{
			InsertMenu(hMenu, 2, MF_BYPOSITION|MF_POPUP, (DWORD)hSaveMenu, WML_SAVES);
			InsertMenu(hMenu, 3, MF_BYPOSITION|MF_POPUP, (DWORD)hPageMenu, WML_PAGES);
			InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_WML_SETTINGS, WML_SETTINGS);
			InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_WML_CONNECTACTTION, WML_CONNECTACTTION);
			InsertMenu(hMenu, 6, MF_BYPOSITION, IDM_WML_PUSH, WML_PUSH);
			InsertMenu(hMenu, 7, MF_BYPOSITION, IDM_WML_EXITAPP, WML_EXITAPP);
		}
		else
		{
			InsertMenu(hMenu, 2, MF_BYPOSITION|MF_POPUP, (DWORD)hPageMenu, WML_PAGES);
			InsertMenu(hMenu, 3, MF_BYPOSITION, IDM_WML_SETTINGS, WML_SETTINGS);
			InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_WML_CONNECTACTTION, WML_CONNECTACTTION);
			InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_WML_PUSH, WML_PUSH);
			InsertMenu(hMenu, 6, MF_BYPOSITION, IDM_WML_EXITAPP, WML_EXITAPP);
		}

		InsertMenu(hSaveMenu, 0, MF_BYPOSITION, IDM_WML_BOOKMARK, WML_BOOKMARK);
		InsertMenu(hSaveMenu, 1, MF_BYPOSITION, IDM_WML_PAGE, WML_PAGE);
		InsertMenu(hSaveMenu, 2, MF_BYPOSITION, IDM_WML_FILE, WML_FILE);
		InsertMenu(hSaveMenu, 3, MF_BYPOSITION, IDM_WML_SHORTCUT, WML_SHORTCUT);

		InsertMenu(hPageMenu, 0, MF_BYPOSITION, IDM_WML_GOHOMEPAGE, WML_GOHOMEPAGE);
		InsertMenu(hPageMenu, 1, MF_BYPOSITION, IDM_WML_BOOKMARKS, WML_BOOKMARKS);
		InsertMenu(hPageMenu, 2, MF_BYPOSITION, IDM_WML_PAGESAVED, WML_PAGESAVED);
		InsertMenu(hPageMenu, 3, MF_BYPOSITION, IDM_WML_HISTORY, WML_HISTORY);			

        WIE_EnableSaveImg(FALSE);
		WIE_EnableSaveSource(FALSE);

        PDASetMenu(hParent, hMenu);
        hwndIEApp = CreateWindow(
            "WMLIeWndClass", 
            STR_WAPWML_CAPTION, 
            WS_VISIBLE|WS_CHILD,//|WS_CLIPSIBLINGS,
            rClient.left, rClient.top, 
			rClient.right-rClient.left, 
			rClient.bottom-rClient.top,
            (HWND)hParent, 
            NULL,
            NULL, 
            NULL
            );
		if (hwndIEApp == NULL)
			return;
        
        ShowWindow(hwndIEApp, SW_SHOW);
        UpdateWindow(hwndIEApp);       
        
		//create the window and enter push message directly. null go to pages
		if(wParam == PUSHMSG)
		{
			WAP_PushMessage(hWapFrame);
			return;
		}
		//create the window and enter bookmark directly. null go to pages
		if(bBookenter)
		{
			bBookenter = FALSE;
			GetClientRect(hWapFrame, &rClient);
			On_IDM_BM(hWapFrame, rClient);
			return ;
		}

		//if app right entered, go to homepage
        if (lParam == WIE_USER_CALLDEF)
        {
            if (strlen(szHomePage) != 0) 
				UI_Message(UM_URLGO, (unsigned long)curCfgFile.sHomepage, curCfgFile.nhomepagetype);
        }
		//if app called by other, go to page demanded
        else if (lParam != WIE_USER_CALLDEF)
        {
			if(!bCallUser)
			{
				if (strlen(szHomePage) != 0) 
				{
					UI_Message(UM_URLGO, (unsigned long)curCfgFile.sHomepage, curCfgFile.nhomepagetype);
				}	
				else
				{
					UI_Message(UM_URLGO, (unsigned long)"", URL_LOCAL);
				}
			}
			else
			{
				if(!bConnectted)
					WAP_ShowConnectornot(hwndIEApp, IDM_WML_CONNECTCONFIRM, TRUE);
				else
					Wml_Go();
			}
        }                
    }
}

static int WIE_OnCommand(HWND hParent, WPARAM wParam, LPARAM lParam)
{
    WORD mID = LOWORD(wParam);
        
    switch (mID)
    {
		//save the page to file entrance
	case IDM_WML_PAGE:
		{
			int ntailtype;

			ntailtype = PWBE_GetSourceType(pUIBrowser);
			if(ntailtype == MMT_WMLC || ntailtype == MMT_WML1 || ntailtype == MMT_WML2 ||
			   ntailtype == MMT_XHTMLMP || ntailtype == MMT_HTML)
			   	On_IDM_SaveAs(hWapFrame, pUIBrowser, rClient);
			else
				PLXTipsWin(NULL, NULL, NULL, WML_WRONGTYPE, NULL,
						Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
		}
		break;
		
		//go to url entrance
	case IDM_WML_GOTOURL:
		On_IDM_REMOTE(hWapFrame, TRUE, rClient);
		break;
		
		//save to bookmark entrance
	case IDM_WML_BOOKMARK:
		{
			char promtp[40];
			
			if(InitBMT != WIE_BM_MAXNUM)
			{
				char szTmp[URLNAMELEN] = "";
				
				strcpy(szTmp, PWBE_GetCurURLName(pUIBrowser));
				WIE_BM_Edit(-1, hWapFrame, NULL, szTmp, rClient, FALSE);
			}
			else
			{
				strcpy(promtp, WML_SAVEFAILED);
				strcat(promtp, "\n");
				strcat(promtp, WML_BMFULL);
				PLXTipsWin(NULL, NULL, NULL, promtp, NULL, 
					Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
			}
		}		
		break;
		
		//bookmark entrance
	case IDM_WML_BOOKMARKS:
		On_IDM_BM(hWapFrame, rClient);
		break;
		
		//save the media to file entrance
	case IDM_WML_FILE:
        PWBE_SaveFocusedImage(pUIBrowser, NULL);
        break;
        
		//save to shortcut entrance
	case IDM_WML_SHORTCUT:
		WML_OnShortcut();
		break;
		
		//go to homepage entrance
	case IDM_WML_GOHOMEPAGE:
		if(hProcess != NULL)
			SendMessage(hProcess, WM_CLOSE, NULL, NULL);
        UI_Message(UM_URLGO, (unsigned int)szHomePage, nHomepageType);
        break;
		
		//no back menu, so this function not used
    case WIE_IDM_BACK:
		if(hProcess != NULL)
			SendMessage(hProcess, WM_CLOSE, NULL, NULL);
        PWBE_PrevPage(pUIBrowser);
        break;
        
		//no stop menu, so this function not used
    case WIE_IDM_STOP:
		if(hProcess != NULL)
			SendMessage(hProcess, WM_CLOSE, NULL, NULL);
        PWBE_Stop(pUIBrowser);        
        break;
		
		//refresh this page
    case WIE_IDM_REFRESH:
		if(hProcess != NULL)
			SendMessage(hProcess, WM_CLOSE, NULL, NULL);
        PWBE_Refresh(pUIBrowser);  
		bSet = TRUE;
        break;
        
		//disconnect from net confirm window
	case IDM_WML_DISCONNECTACTTION:
		{	
			if(bConnectted)
				WAP_ShowConnectornot(hParent, IDM_WML_DISCONFIRM, FALSE);
		}
		break;

		//connect to net confirm window
	case IDM_WML_CONNECTACTTION:
		{
			if(!bConnectted)
			{
				char *tmp;
				
				tmp = (char *)PWBE_GetCurURLName(pUIBrowser);
				if((tmp != NULL) && (strncmp(tmp, "file://", strlen("file://")) == 0))
					UI_Message(UM_URLGO, (unsigned int)szHomePage, nHomepageType);
				else if((PWBE_GetCurURLName(pUIBrowser) == NULL) && (!bHistory))
					UI_Message(UM_URLGO, (unsigned int)szHomePage, nHomepageType);
				else
					WAP_ShowConnectornot(hParent, IDM_WML_CONCONFIRM, TRUE);
			}
		}
		break;

        //list saved pages entrance
	case IDM_WML_PAGESAVED:
        On_IDM_LF(hWapFrame, rClient);        
        break;
		
		//list history entrance
	case IDM_WML_HISTORY:
        On_IDM_Visited(hWapFrame, rClient);
        break;
        
		//net and display setting window entrance
	case IDM_WML_SETTINGS:
		WAP_Public_Setting(hWapFrame);
        break;
        
		//push message entrance
	case IDM_WML_PUSH:
		WAP_PushMessage(hWapFrame);
		bPushenter = FALSE;
		bPushexit = FALSE;
		break;
		
		//exit this app
    case IDM_WML_EXITAPP:
		PostMessage(hParent, WM_CLOSE, NULL, NULL);	
		break;
        
		//exit button
    case WIE_IDM_MENU_EXIT:
        PostMessage(hParent, WM_CLOSE, NULL, NULL);
        break;
        
    default:
        break;
    }
	
    return 1;        
}

static void DrawFirstView(HDC hdc)
{
    if (!hActiveGif) 
        DrawImageFromFile(hdc, WIE_szGifFile, WGIF_X, RC_WMLBROW_BTM+3, DM_NONE);
}

static void WIE_SetShowGifFile(const char* SZFILENAME)
{
    strcpy(WIE_szGifFile, SZFILENAME);
}

static void StartWapGif()
{
    if (!hActiveGif) 
		hActiveGif = StartAnimatedGIFFromFile(hwndIEApp, WIE_szGifFile, WGIF_X, RC_WMLBROW_BTM+3, DM_NONE);	
}

static void EndWapGif() 
{
	if (hActiveGif) 
    {
        EndAnimatedGIF(hActiveGif);
        hActiveGif = NULL;
    }	
}

static void OnWieWndCreate(HWND hWnd)
{    
    SetWindowText(hWapFrame, (LPCTSTR)STR_WAPWML_CAPTION);
	SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_EXIT);
	SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
	SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    
    ibExiting = FALSE;
    ibSelExit = FALSE;
    
    //load configure
    Loadconfig(&curCfgFile); 
	//load bookmark here in order to make save to bookmark more quick, and need not to load so many times
	InitBMT = WIE_LoadBookMark();
	strcpy(szHomePage, curCfgFile.sHomepage);
    WIE_Set_Systemcfg();    

    if (strlen(curCfgFile.sGateWay) == 0 || curCfgFile.nPort == 0)
    {
        strcpy(SetupInfo.WAPIP, "10.0.0.172");
        strcpy(SetupInfo.WAPPORT, "80");
    }
	else
	{
		strcpy(SetupInfo.WAPIP, curCfgFile.sGateWay);
        strcpy(SetupInfo.WAPPORT, curCfgFile.nPort);
	}

    LoadHistory();
    
    if (!CreateButtonsINIEW(hWnd, WSET_GetColorCfg()))
    {
        PLXTipsWin(NULL, NULL, NULL, WML_ERRORWINDOW, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);        
        return;
    }
    
	hwmlconnect = LoadImage(NULL, WIE_IMG_CONNECT, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hwmldisconnect = LoadImage(NULL, WIE_IMG_DISCONNECT, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if(bConnectted)
		SendMessage(GetWAPFrameWindow(), PWM_SETAPPICON, 
			MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)hwmlconnect);
	else
		SendMessage(GetWAPFrameWindow(), PWM_SETAPPICON, 
			MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)hwmldisconnect);

    UI_Message(UM_STATUSRESET, NULL, NULL);
    WIE_SetShowGifFile(WIE_GIF_NORMAL);
}

static LRESULT IEAppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    HWND hwndtemp = GetFocus();
    
	printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ wMsgCmd = %08x ########################\r\n", wMsgCmd);
    switch (wMsgCmd)
    {
    case WM_CREATE:
        OnWieWndCreate(hWnd);
		SetFocus(hViewWnd);
        break;

    case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_PAGEUP:
                PWBE_NextPage(pUIBrowser);                
                break;

            case VK_PAGEDOWN:    
				PWBE_PrevPage(pUIBrowser);
                break;

			case VK_F10:
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;

			case VK_F5:
				return PDADefWindowProc(hWapFrame, wMsgCmd, wParam, lParam);

            default:
				return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }            
        }
        break;
		
	case PWM_SHOWWINDOW:
		{		
			HWND child, prevchl;
			int k = 0;
			
			if(bConnectted)
			{	
				if(NULL != PWBE_GetCurURLName(pUIBrowser))
				{
					if(strlen(szUrlTitle) != 0)
						strcpy(sztmpTitle, szUrlTitle);
				}
				if(strlen(sztmpTitle) != 0)	
				{
					char *szMulti = NULL;
					int nLenMulti = 0;

					nLenMulti = UTF8ToMultiByte(CP_ACP, 0, sztmpTitle, -1, NULL, 0, NULL, NULL);
					szMulti = (char *)malloc(sizeof(char) * nLenMulti + 1);
					memset(szMulti, 0, nLenMulti + 1);
					nLenMulti = UTF8ToMultiByte(CP_ACP, 0, sztmpTitle, -1, szMulti, nLenMulti, NULL, NULL);
					szMulti[nLenMulti] = '\0';
					SetWindowText(hWapFrame, szMulti);	
					free(szMulti);
				}
//				SendMessage(GetWAPFrameWindow(), PWM_SETAPPICON, 
//					MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)hwmlconnect);
			}
			else 
			{
				SetWindowText(hWapFrame, (LPCTSTR)STR_WAPWML_CAPTION);
//				SendMessage(GetWAPFrameWindow(), PWM_SETAPPICON, 
//					MAKEWPARAM(IMAGE_BITMAP, LEFTICON), (LPARAM)hwmldisconnect);
			}
			PDASetMenu(hWapFrame, NULL);
			child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
			while(child != NULL)
			{
				prevchl = GetWindow(child, GW_HWNDPREV);
				child = prevchl;
				if(IsWindow(child))
					k++;
			}
			if(k <= 1)
			{
				PDASetMenu(hWapFrame, hMenu);
				WML_BackOrExit(rbtnTxtID, FALSE);
				SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_OPTION);
				SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)LBtnInfo);	
 			}
		}
		break;

	case PWM_ACTIVATE:
		if(wParam == WA_ACTIVE)
			SetFocus(hViewWnd);
		break;

    case WM_ACTIVATE:        
        if (WA_ACTIVE == LOWORD(wParam))
		{
			if(hViewWnd != NULL)
				SetFocus(hViewWnd);
			brsUIData.hdc = hViewDC;
		}
		else if(WA_INACTIVE == LOWORD(wParam))
		{
			brsUIData.hdc = NULL;
		}

        break;

	case IDM_WML_DISCONFIRM:
		if(lParam == 1)
		{
			bDisconnectbymenu = TRUE;
			PWBE_Hangup(pUIBrowser);
			
			if(bHistory)
				rbtnTxtID = 2;
			else
				rbtnTxtID = 0;
			WML_BackOrExit(rbtnTxtID, TRUE);
			if(hProcess != NULL)
				SendMessage(hProcess, WM_CLOSE, NULL, NULL);
		}
		break;

	case IDM_WML_CONCONFIRM:
		if(lParam == 1)
		{
//			WaitWin(hwndIEApp, TRUE, WML_GATEWAYFOUND, STR_WAPWML_CAPTION, NULL, STR_WAPWINDOW_CANCEL, IDM_WML_WAIT_CONNECT);
			PWBE_Refresh(pUIBrowser);
		}
		break;

	case IDM_WML_CONNECTCONFIRM:
		if(lParam == 1)
		{
//			WaitWin(hwndIEApp, TRUE, WML_GATEWAYFOUND, STR_WAPWML_CAPTION, NULL, STR_WAPWINDOW_CANCEL, IDM_WML_WAIT_CONNECT);
			Wml_Go();		
		}
		break;

	case IDM_WML_DISCONNECTCONFIRM:
		if(lParam == 1)
			PostMessage(hwndIEApp, WM_SELOK, NULL, NULL);
		else
			PostMessage(hwndIEApp, WM_SELCANCEL, NULL, NULL);
		break;

	case IDM_WML_WAIT_CONNECT:
		PWBE_Stop(pUIBrowser);
		break;
        
    case WM_PAINT:
        hdc = BeginPaint(hWnd, NULL);
        DrawFirstView(hdc);             
        if (hActiveGif != NULL) 
            PaintAnimatedGIF(hdc, hActiveGif);
        EndPaint(hWnd, NULL);        
        break;    

    case WM_SELCANCEL:
        ibSelExit = FALSE;
        break;

    case WM_SELOK:
        ibSelExit = TRUE;
        PWBE_Stop(pUIBrowser);
		WaitWin(hwndIEApp, TRUE, WML_DISCONNECTING, STR_WAPWML_CAPTION, NULL, 
			NULL, IDM_WML_WAIT_DISCONNECT);
		ibExiting = TRUE;
        PWBE_Destroy(pUIBrowser);
		
        break;

	case WM_BROHISTORY:
        {			
            switch (lParam)
            {
            case WML_ENABLEBF:
                SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLLEFT| SCROLLRIGHT, SCROLLLEFT| SCROLLRIGHT);
                EnableMenuItem(hMenu, WIE_IDM_BACK, MF_BYCOMMAND|MF_ENABLED);
                break;

            case WML_DISABLEBF:
                SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, SCROLLLEFT| SCROLLRIGHT);
                EnableMenuItem(hMenu, WIE_IDM_BACK, MF_BYCOMMAND|MF_GRAYED);
                break;

            case WML_ENABLEBACK:
				SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLLEFT, SCROLLLEFT| SCROLLRIGHT);
                EnableMenuItem(hMenu, WIE_IDM_BACK, MF_BYCOMMAND|MF_ENABLED);
                break;

            case WML_ENABLEFORWORD:
                SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLRIGHT, SCROLLLEFT| SCROLLRIGHT);
                EnableMenuItem(hMenu, WIE_IDM_BACK, MF_BYCOMMAND|MF_ENABLED);
                break;

            default:
                break;
            }
        }
        break;
                
    case WM_COMMAND:
        WIE_OnCommand(hWnd, wParam, lParam);
        break;
		
    case WM_CLOSE:
        PostMessage(hWnd, WM_CLOSE_PROMPT, NULL, NULL);
        break;

    case WM_CLOSE_PROMPT:
        WIE_Exit_Prompt();    
        break;
        
    case WM_DESTROY:
		{			
			BOOL bExit = FALSE;

			if(hProcess != NULL)
				SendMessage(hProcess, WM_CLOSE, NULL, NULL);
			
			if((requesturl != NULL) && (strlen(requesturl) != 0))
			{
				bExit = TRUE;
				ExitWapShortcut(requesturl);
			}
			
			SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			
			DlmNotify(PMM_NEWS_ENABLE_SCRSAVE, ScreenSave);
			WBK_Close_Window();
			WCFG_Close_Window();
			WML_GotoCloseWindow();
			WML_SaveCloseWindow();
			WML_HisCloseWindow();
			WML_SetCloseWindow();
			WML_PushCloseWindow();
			WIE_FreeAllBm();
			DestroyMenu(hMenu);
			DestroyMenu(hSaveMenu);
			DestroyMenu(hPageMenu);
			
			WaitWin(hwndIEApp, FALSE, WML_DISCONNECTING, STR_WAPWML_CAPTION, NULL, 
					NULL, IDM_WML_WAIT_DISCONNECT);
#ifndef _EMULATE_
			JAM_Install_Finalize();
#endif        
//			WIE_Savehistory();
			ibExiting = FALSE;
			ibSelExit = FALSE;
			Destroy_LF_WND();
			DialSuccessFlag = 0;    
			hwndIEApp = NULL;
			bHistory = FALSE;
			hWapFrame = NULL;
			pUIBrowser = NULL;
			bCallUser = FALSE;
			
			rbtnTxtID = 0;
			
			DeleteObject(hwmlnsbmp);
			DeleteObject(hwmlnsbmpfs);
			DeleteObject(hwmlnsbmpps); 
			DeleteObject(hwmlconnect);
			DeleteObject(hwmldisconnect);
			UnregisterClass("WMLIeWndClass", NULL);
			
			if(!bExit)
				DlmNotify((WPARAM)PES_STCQUIT, (LPARAM)hInstance);
			else
				bExit = FALSE;
		}
        break;    

    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
       
    return lResult;
}

void UI_SetScrollPos(void *pUIData, WB_SBTYPE iBar, WB_PAGEINFO *pPageInfo, WB_SBSTATE iState);
extern HWND CreateViewPort(HWND hUIWnd, int x, int y, int width, int height, HINSTANCE hInstance);
/*****************************************************************************\
* CreateButtonsINIEW
*
* create controls in IE 
*
* Arguments:
*
* Returns:
*        TRUE  ---------------  create success
*        FALSE ---------------  create failure
\*****************************************************************************/
static BOOL CreateButtonsINIEW(HWND hWnd, Color_Scheme uClCfg)
{    
    RECT rcWMLBrowser;
	static LIBLIST_APP callBackFuns;
	int  nProtocol;
	if(!bCallUser)
    {
#ifdef HOMEPAGE_START
		if(strcmp(szHomePage, "") == 0)
		{
			strcpy(szHomePage, "file://");
			strcat(szHomePage, WIE_ABOUTBLANK);
			nHomepageType = 1;
		}
    SetCurentPage(szHomePage, nHomepageType);
#else
    SetCurentPage("", URL_NULL);    
#endif
    }
    
    SetRect(&rcWMLBrowser, RC_WMLBROW_LT, RC_WMLBROW_TOP, RC_WMLBROW_RT, RC_WMLBROW_BTM);
    /* initialize global variable gBrowser */
#ifndef _EMULATE_
    JAM_Install_Init();
#endif
    
	if(strcmp(SetupInfo.WAPPORT, "9201") == 0)
        nProtocol = WBTRANS_WSP;
	else
		nProtocol = WBTRANS_WHTTP;
	
    hViewWnd = CreateViewPort(hWnd, rcWMLBrowser.left, rcWMLBrowser.top, 
		rcWMLBrowser.right - rcWMLBrowser.left, rcWMLBrowser.bottom - rcWMLBrowser.top, hInstance);		
				
	hViewDC = GetDC(hViewWnd);
	ReleaseDC(hViewWnd, hViewDC);
	
	GetClientRect(hViewWnd, &rcWMLBrowser);
	
	brsUIData.hwnd = hViewWnd;
	GetFontHandle(&hViewFont, SMALL_FONT);
	SelectObject(hViewDC, hViewFont);
	brsUIData.hdc = hViewDC;
	
	brsUIData.rc.left = rcWMLBrowser.left;
	brsUIData.rc.top = rcWMLBrowser.top;
	brsUIData.rc.right = rcWMLBrowser.right - 1;
	brsUIData.rc.bottom = rcWMLBrowser.bottom - 1;

	printf("\r\n@@@@@@@@@@@@@@@@@@@@ nProtocol = %d ############################\r\n", nProtocol);
	printf("\r\n@@@@@@@@@@@@@@@@@@@@ SetupInfo.WAPIP = %s #############################\r\n", SetupInfo.WAPIP);
	printf("\r\n@@@@@@@@@@@@@@@@@@@@ SetupInfo.WAPPORT = %s #############################\r\n", SetupInfo.WAPPORT);
	
    pUIBrowser = PWBE_Create((void *)&brsUIData, (WB_RECT*)&rcWMLBrowser,
		nProtocol, SetupInfo.WAPIP,atoi((const char*)SetupInfo.WAPPORT), NULL, NULL,
		&g_GUIFuncs);
    if (pUIBrowser == NULL)
        return FALSE;

	//configure User Agent "POLLEX WMLBrowser/1.0" "LenovoG900" "Nokia7650/1.0"
    //PWBE_SetUserAgent(pUIBrowser, "Nokia7650/1.0");
	PWBE_SetUserAgent(pUIBrowser, "Nokia7650/1.0 SymbianOS/6.1 Series60/0.9 Profile/MIDP-1.0 Configuration/CLDC-1.0");

	PWBE_SetDisplayOption(pUIBrowser, WBDOP_IMGDISPLAY, &(curCfgFile.bShowImg), 0);
    PWBE_SetDisplayOption(pUIBrowser, WBDOP_BGSOUND, &(curCfgFile.bPlayRing), 0);
	//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &uClCfg, 0);

	memset(&callBackFuns, 0, sizeof(LIBLIST_APP));
	callBackFuns.SetSrollPosProc = UI_SetScrollPos;
	callBackFuns.UINotifyProc = UI_Message;
	callBackFuns.DownloadProc = WLE_DownLoad;

	PWBE_RegisterCallbackProcs(pUIBrowser, &callBackFuns);
	
    return TRUE;
}

void Wml_Go()
{
    char szTmp[URLNAMELEN] = "";
    int nurltype;
    int nret;   
    
	memset(szTmp, 0, URLNAMELEN);
    nret = GetCurentPage(szTmp, &nurltype);
	
	if(strlen(szTmp) != 0)
		PWBE_NewPage(pUIBrowser, szTmp, WBTRANS_GET, NULL, NULL);
	else
		PWBE_NewPage(pUIBrowser, "", WBTRANS_GET, NULL, NULL);

	bCallUser = FALSE;
}

static INT WIE_Exit_Prompt()
{
	if(bConnectted)
		WAP_ShowConnectornot(hwndIEApp, IDM_WML_DISCONNECTCONFIRM, FALSE);
	else
		PostMessage(hwndIEApp, WM_SELOK, NULL, NULL);
	
    return 1;    
}

static int WIE_ExitProc()
{    
    ibExiting = FALSE;
    DestroyWindow(hwndIEApp);
    
    return 1;    
}

#define WIE_STRSHOWLEN  (int)(WGIF_X/8)
#define WIE_STRRATELEN  5

/*********************************************************************\
* Function       
* Purpose      
* Params       
* Return            
* Remarks       
**********************************************************************/
int UI_Message(unsigned int MsgID, unsigned long Param1, unsigned long Param2)
{
    char *buf;
    RECT rcText;

    rcText.left   = RC_STATETEXT_LT;
    rcText.top    = RC_STATETEXT_TOP-2;
    rcText.right  = RC_STATETEXT_RT;
    rcText.bottom = RC_STATETEXT_BTM;
    
    switch(MsgID)
    {
    case UM_DIALSTART:
        bConnectted = FALSE;
        StartWapGif();
		SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
		rbtnTxtID = 1;
		WaitWin(hwndIEApp, TRUE, WML_GATEWAYFOUND, STR_WAPWML_CAPTION, NULL, STR_WAPWINDOW_CANCEL, IDM_WML_WAIT_CONNECT);        
		printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_DIALSTART ######################\r\n");
        break;
		
    case UM_DIALOK:
        bConnectted = FALSE;
        EndWapGif();
		printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_DIALOK ######################\r\n");
        break;

    case UM_HANGUP:
        bConnectted = FALSE;
		if(bHistory)
			rbtnTxtID = 2;
		else
			rbtnTxtID = 0;
        StartWapGif();   
		printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_HANGUP ######################\r\n");
        break;

    case UM_DIALBREAK:
		{
			HWND child, prevchl;
			int k = 0;
			
			if(!ibExiting)
				WaitWin(hwndIEApp, FALSE, WML_WAITING, STR_WAPWML_CAPTION, NULL, STR_WAPWINDOW_CANCEL, IDM_WML_WAIT_CONNECT);
			bConnectted = FALSE;
			child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
			while(child != NULL)
			{
				prevchl = GetWindow(child, GW_HWNDPREV);
				child = prevchl;
				if(IsWindow(child))
					k++;
			}
			if(k <= 0 && IsWindowVisible(hWapFrame))
			{
				if (!bHistory)
				{
					SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_EXIT);
					rbtnTxtID = 0;
				}
				else			
				{
					SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);	
					rbtnTxtID = 2;
				}
			}
			else
			{
				if(bHistory)
					rbtnTxtID = 2;
				else
					rbtnTxtID = 0;
			}
			
			EndWapGif();
			if(!bsave)
			{
				DeleteMenu(hMenu, 4, MF_BYPOSITION);	
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_WML_CONNECTACTTION, WML_CONNECTACTTION);  
			}
			else
			{
				DeleteMenu(hMenu, 5, MF_BYPOSITION);	
				InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_WML_CONNECTACTTION, WML_CONNECTACTTION);
			}
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_DIALBREAK ######################\r\n");
		}
        break;
               
    case UM_CONNECTSTART:
        {            
            int nTransType;
            bConnectted = FALSE;
            nTransType = (int)LOWORD(Param1);
            
            if (nTransType == TRANS_NORMAL)
                WIE_SetShowGifFile(WIE_GIF_NORMAL);
            else
                WIE_SetShowGifFile(WIE_GIF_SECURE);
            
            StartWapGif();
			SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL); 
			rbtnTxtID = 1;
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_CONNECTSTART ######################\r\n");
        }
        break;

    case UM_CONNECTTED: 
        bConnectted = TRUE;
        EndWapGif();
		WaitWin(hwndIEApp, FALSE, WML_GATEWAYFOUND, STR_WAPWML_CAPTION, NULL, STR_WAPWINDOW_CANCEL,IDM_WML_WAIT_CONNECT);
		rbtnTxtID = 1;
		if(bsave)
		{
			DeleteMenu(hMenu, 5, MF_BYPOSITION);	
			InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_WML_DISCONNECTACTTION, WML_DISCONNECTACTTION);
		}
		else
		{
			DeleteMenu(hMenu, 4, MF_BYPOSITION);	
			InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_WML_DISCONNECTACTTION, WML_DISCONNECTACTTION);
		}
		printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_CONNECTTED ######################\r\n");
        break;
		
    case UM_CONNECTFAIL:
        {			
			bConnectted = FALSE;
			EndWapGif();
			PLXTipsWin(NULL, NULL, NULL, WML_FAILCONNECT, NULL,	Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
			if(bsave)
			{
				DeleteMenu(hMenu, 5, MF_BYPOSITION);		
				InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_WML_CONNECTACTTION, WML_CONNECTACTTION);
			}
			else
			{
				DeleteMenu(hMenu, 4, MF_BYPOSITION);		
				InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_WML_CONNECTACTTION, WML_CONNECTACTTION);
			}
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_CONNECTFAIL ######################\r\n");
		}
        break;

    case UM_DISCONNECT:    
        bConnectted = FALSE;
        EndWapGif();
		if(!bsave)
		{
			DeleteMenu(hMenu, 4, MF_BYPOSITION);
			InsertMenu(hMenu, 4, MF_BYPOSITION, IDM_WML_CONNECTACTTION, WML_CONNECTACTTION);
		}
		else
		{
			DeleteMenu(hMenu, 5, MF_BYPOSITION);
			InsertMenu(hMenu, 5, MF_BYPOSITION, IDM_WML_CONNECTACTTION, WML_CONNECTACTTION);
		}
		if(bDisconnectbymenu)
		{
			PLXTipsWin(NULL, NULL, NULL, WML_DISCONNECTSUCCESS, NULL, Notify_Success, IDS_OK, NULL, WAITTIMEOUT);
			bDisconnectbymenu = FALSE;
		}
		printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_DISCONNECT ######################\r\n");
		break;
        
    case UM_REQUEST:
        {
            int type;
            int nTransType;
			HWND child, prevchl;
			int k = 0;
			
            char strShow[WIE_STRSHOWLEN + WIE_STRRATELEN + 10];
            
            type       = (int)HIWORD(Param1);            
            nTransType = (int)LOWORD(Param1);
            
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_REQUEST ######################\r\n");
            if (type == MMT_DOCUMENT)
			{
				WIE_CutString((char*)Param2, strShow, WIE_STRSHOWLEN, WIE_STRSHOWLEN);
				ProcessBarCreate(hWapFrame, hViewWnd);
			}
            else
			{
                strcpy(strShow, WML_LOAD);
				if(bMedia)
				{
					ProcessBarCreate(hWapFrame, hViewWnd);
					bMedia = FALSE;
				}
			}
            
            if (nTransType == TRANS_NORMAL)
                WIE_SetShowGifFile(WIE_GIF_NORMAL);
            else
                WIE_SetShowGifFile(WIE_GIF_SECURE);
            
            StartWapGif(); 
			
			
			child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
			while(child != NULL)
			{
				prevchl = GetWindow(child, GW_HWNDPREV);
				child = prevchl;
				if(IsWindow(child))
					k++;
			}
			if(k <= 0 && IsWindowVisible(hWapFrame))
			{
				SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);    
			}
			rbtnTxtID = 1;
        }
        break;

    case UM_DATASTART:
        {            
            int nDataLen;
            int type;
            char strShow[WIE_STRSHOWLEN + WIE_STRRATELEN];
            int sec;
            HWND child, prevchl;
			int k = 0;
			
            nDataLen = (int)LOWORD(Param1);
            type     = (int)HIWORD(Param1);

			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_DATASTART ######################\r\n");
            
            if (type == MMT_DOCUMENT)
            {
				nAllDataSize = nDataLen;
				printf("\r\n@@@@@@@@@@@@@@@@@@@@ nAllDataSize = %d ######################\r\n", nAllDataSize);
				
                WIE_RatioInsert(nDataLen, 0, type, (char*)Param2);
                
                if ((int)(nDataLen/1024) > 1)
                    sec = 2;
                else
                    sec = 1;
                sprintf(strShow, WML_LEFT, ((double)nDataLen/(double)1000), sec);
            }
            StartWapGif();
			
			child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
			while(child != NULL)
			{
				prevchl = GetWindow(child, GW_HWNDPREV);
				child = prevchl;
				if(IsWindow(child))
					k++;
			}
			if(k <= 0 && IsWindowVisible(hWapFrame))
			{
				SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
			}
			rbtnTxtID = 1;
        }
        break;    
        
    case UM_DATAIN:
        {            
            int nRcvDataLen;
            WIEURLRATIO temp;
            int type;            
            char strShow[WIE_STRSHOWLEN + WIE_STRRATELEN];
            char strRate[WIE_STRRATELEN];
            
            nRcvDataLen = (int)LOWORD(Param1);
            type        = (int)HIWORD(Param1);

			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_DATAIN ######################\r\n");
            if (type == MMT_DOCUMENT)
            {
				nInDataSize += nRcvDataLen;
				
				printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ nInDataSize = %d ######################\r\n", nInDataSize);
				printf("@@@@@@@@@@@@@@@@@@@@@@@@ nRcvDataLen = %d ######################\r\n", nRcvDataLen);
				printf("@@@@@@@@@@@@@@@@@@@@@@@@ nAllDataSize = %d ######################\r\n", nAllDataSize);
                if (WIE_RatioAddRcv((char*)Param2, nRcvDataLen) == 1)
                    return FALSE;
                if (1 == WIE_RatioGet((char*)Param2, &temp))
                    return 1;
                
                WIE_CutString((const char*)temp.szUrl, strShow, WIE_STRSHOWLEN, WIE_STRSHOWLEN);
                if (0 == WIE_GetStrRatio(strRate, WIE_STRRATELEN, temp.nRecvDataLen, temp.nDataLen) )
					strcat(strShow, strRate);
                else
                    strcat(strShow, "100%");
				if(nAllDataSize != 0)
				{
					ps = (nInDataSize * 100)/nAllDataSize;
				}
				else
				{
					ps = 100;
				}
				if(ps <= 100)
					DrawProcess(ps);
            }
        }
        break;

	case UM_FOCUSCHANGE:
		{
			HWND child, prevchl;
			int k = 0;
			
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_FOCUSCHANGE ######################\r\n");
			switch (Param1)
			{
			case WBFOCUS_NULL:
				strcpy(LBtnInfo, "");
				break;
				
			case WBFOCUS_LINK:
				strcpy(LBtnInfo, WML_GOTO);			
				break;
				
			case WBFOCUS_WIDGET:
				strcpy(LBtnInfo, IDS_OK);			
				break;
				
			default:
				break;
			}		
			
			child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
			while(child != NULL)
			{
				prevchl = GetWindow(child, GW_HWNDPREV);
				child = prevchl;
				if(IsWindow(child))
					k++;
			}
			if(k <= 0 && IsWindowVisible(hWapFrame))
			{
				SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)LBtnInfo);
			}
		}
		break;
		

	case UM_DOCUMENTEND:
		{
			HWND child, prevchl;
			int k = 0, nLeft;
			
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_DOCUMENTEND ######################\r\n");
			if((nLeft = (int)Param1) == 0)
			{
				child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
				while(child != NULL)
				{
					prevchl = GetWindow(child, GW_HWNDPREV);
					child = prevchl;
					if(IsWindow(child))
						k++;
				}
				if(k <= 0 && IsWindowVisible(hWapFrame))
				{
					if (!bHistory)
					{
						SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_EXIT);
						rbtnTxtID = 0;
					}
					else			
					{
						SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);
						rbtnTxtID = 2;
					}
				}
				else
				{
					if(bHistory)
						rbtnTxtID = 2;
				}
			}
		}
		break;

    case UM_DATAEND:
        {
            int type;
            int nLeftNum;
            
            type     = (int)HIWORD(Param1);
            nLeftNum = (int)LOWORD(Param1);
            
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_DATAEND ######################\r\n");
            if (type == MMT_DOCUMENT)
            {
                WIE_RatioDelete((char*)Param2);
                EndWapGif();

				if (nLeftNum == 0)
				{
					DrawProcess(100);
					SendMessage(hProcess, WM_CLOSE, NULL, NULL);
					bMedia = TRUE;
				}				
				
            }
            else
            {
				if(bSet)
				{
					printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ nAllDataSize = %d ######################\r\n", nAllDataSize);
					printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ nLeftNum = %d ######################\r\n", nLeftNum);
					nAllDataSize = nLeftNum + 1;
					bSet = FALSE;
				}
				
                if (nLeftNum != 0)
                {
                    //sprintf(strShow, WML_LOADING, nLeftNum);
                    //UI_Message(UM_STATUS, (WPARAM)strShow, MSG_TOLOW);        
                }
                else
                {
					HWND child, prevchl;
					int k = 0;
					
						child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
						while(child != NULL)
						{
							prevchl = GetWindow(child, GW_HWNDPREV);
							child = prevchl;
							if(IsWindow(child))
								k++;
						}
						if(k <= 0 && IsWindowVisible(hWapFrame))
						{
							if (!bHistory)
							{
								SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_EXIT);
								rbtnTxtID = 0;
							}
							else			
							{
								SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);
								rbtnTxtID = 2;
							}
						}
						else
						{
							if(bHistory)
								rbtnTxtID = 2;
						}
					EndWapGif();
	            }
				nInDataSize = nAllDataSize - nLeftNum;
				printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ nInDataSize = %d ######################\r\n", nInDataSize);
				printf("@@@@@@@@@@@@@@@@@@@@@@@@ nAllDataSize = %d ######################\r\n", nAllDataSize);
				printf("@@@@@@@@@@@@@@@@@@@@@@@@ nLeftNum = %d ######################\r\n", nLeftNum);
				if(nInDataSize != nAllDataSize)
				{
					if(nAllDataSize != 0)
					{
						ps = (nInDataSize * 100)/nAllDataSize;
						DrawProcess(ps);
					}
					else
					{
						ps = 100;
						DrawProcess(ps);
					}
				}
				else
				{
					ps = 100;
					DrawProcess(ps);
					PostMessage(hProcess, WM_CLOSE, NULL, NULL);
					bMedia = TRUE;
					bSet = TRUE;
				}
            }
        }
        break;
        
    case UM_DATAERROR:
        {
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_DATAERROR ######################\r\n");
			if(hProcess != NULL)
				SendMessage(hProcess, WM_CLOSE, NULL, NULL);
            if (0 == WIE_RationEmpty())
            {
                EndWapGif();   
            }
        }
        break;
        
    case UM_STOP:
        {
			HWND child, prevchl;
			int k = 0;
			
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_STOP ######################\r\n");
			if (bConnectted == TRUE)
				EndWapGif();
			if(hProcess != NULL)
				SendMessage(hProcess, WM_CLOSE, NULL, NULL);
			
			child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
			while(child != NULL)
			{
				prevchl = GetWindow(child, GW_HWNDPREV);
				child = prevchl;
				if(IsWindow(child))
					k++;
			}
			if(k <= 0 && IsWindowVisible(hWapFrame))
			{
				if (!bHistory)
				{
					SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_EXIT);
					rbtnTxtID = 0;
				}
				else			
				{
					SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);	
					rbtnTxtID = 2;
				}
			}
			else
			{
				if(bHistory)
					rbtnTxtID = 2;
			}
		}
        break;

    case UM_STARTEXIT:
        break;

    case UM_EXIT:
		printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_EXIT ######################\r\n");
		if(hProcess != NULL)
			SendMessage(hProcess, WM_CLOSE, NULL, NULL);
        WIE_ExitProc();
        break;

    case UM_ERROR:        
		printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_ERROR ######################\r\n");
		if(hProcess != NULL)
			SendMessage(hProcess, WM_CLOSE, NULL, NULL);
		SetWindowText(hWapFrame, STR_WAPWML_CAPTION);
        switch (LOWORD(Param1))
        {
        case ERR_FORMATERR:
            PLXTipsWin(NULL, NULL, NULL, WML_ERR_FORMATERROR, NULL,
				Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
        	break;
        case ERR_ACCSSDENY:            
            PLXTipsWin(NULL, NULL, NULL, WML_ERR_ACCESSDENY, NULL,
				Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
        	break;
        case ERR_UNKNOWNCHARSET:
            PLXTipsWin(NULL, NULL, NULL, WML_ERR_UNKNOWNCHARSET, NULL,
				Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
            break;
        case ERR_UNKNOWNTYPE:
            PLXTipsWin(NULL, NULL, NULL, WML_ERR_UNKNOWNTYPE, NULL,
				Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
            break;
        case ERR_SCRIPT:
            PLXTipsWin(NULL, NULL, NULL, WML_ERR_SCRIPT, NULL,
				Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
            break;
        case ERR_PAGETOOLARGE:
            PLXTipsWin(NULL, NULL, NULL, WML_ERR_PAGETOOLARGE, NULL,
				Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
            break;
        default:
            PLXTipsWin(NULL, NULL, NULL, WML_ERR_DEFAULT, NULL,
				Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
            break;
        }        
        break;
        
#if 0        
    case UM_STATUSRESET:
    case UM_STATUS:
        {
            static int msgPRI;            
   
            if (MsgID == UM_STATUSRESET) 
            {
                msgPRI = MSG_LOW;
            }
            else 
            {
                int pridif = Param2 - msgPRI;

                if (pridif >= 0)
                {
                    if ((char*)Param1 != NULL)
                        strncpy(sShowinfo, (char*)Param1, 100);
                    msgPRI = Param2;                    
                    
                    InvalidateRect(hwndIEApp, &rcText, TRUE);
                }
            }
        }
        break;        
#endif
        
    case UM_SAVEENABLE:
        WIE_EnableSaveSource(TRUE);
        break;

    case UM_SAVEDISABLE:
        WIE_EnableSaveSource(FALSE);
        break;

    case UM_SAVEIMGENABLE:
        WIE_EnableSaveImg(TRUE);
        break;
		
    case UM_SAVEIMGDISABLE:
        WIE_EnableSaveImg(FALSE);
        break;
        		
    case UM_URLGO: 
		printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_URLGO ######################\r\n");
		if(hProcess != NULL)
			SendMessage(hProcess, WM_CLOSE, NULL, NULL);
		if ((strcmp((char *)Param1, "") != 0))
		{
			if(strncasecmp((char *)Param1, "file://", 7) == 0)
			{
				SetCurentPage((char *)Param1,URL_LOCAL);		
				Wml_Go();
			}
			else if((strncmp((char *)Param1, "http://", strlen("http://")) != 0) && (strncmp((char *)Param1, "HTTP://", strlen("HTTP://")) != 0))
			{
				char *tmp;
				int len;
				
				len = strlen((char *)Param1);
				tmp = (char *)malloc(len + 8);
				memset(tmp, 0, (len + 8));
				strncpy(tmp, "http://", 7);
				strcat(tmp, (char *)Param1);	
				SetCurentPage(tmp, URL_REMOTE);
				if(!bConnectted)
					WAP_ShowConnectornot(hwndIEApp, IDM_WML_CONNECTCONFIRM, TRUE);
				else
					Wml_Go();
			}
			else 
			{        
				SetCurentPage((char *)Param1, Param2);
				if(!bConnectted)
					WAP_ShowConnectornot(hwndIEApp, IDM_WML_CONNECTCONFIRM, TRUE);
				else
					Wml_Go();
			}
		}
		else
		{
			char	blankpage[70];
			
			strcpy(blankpage, "file://");
			strcat(blankpage, WIE_ABOUTBLANK);
			SetCurentPage(blankpage,URL_LOCAL);	
			Wml_Go();
			SetWindowText(hWapFrame, STR_WAPWML_CAPTION);
			strcpy(szUrlTitle, STR_WAPWML_CAPTION);
		}
		
        break;
		
    case UM_URLCHGN: 
		printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_URLCHGN ######################\r\n");
		printf("@@@@@@@@@@@@@@@@@@@@@@@@ (char *)Param1 = %s ######################\r\n", (char *)Param1);
        if (strncasecmp((char *)Param1, "FILE", 4) == 0)
		{
			buf = (char *)Param1 + 15;
		}
        else 
            buf = (char *)Param1;
        SetCurentPage(buf, Param2);        
        break;    
        
    case UM_HISTORYINFO:
        {
            int bPrev = (int)Param1;
			{
				if(bPrev != 0)
				{
					rbtnTxtID = 2;
					bHistory = TRUE;
				}
				else
				{
					rbtnTxtID = 0;
					bHistory = FALSE;
				}
            }            
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_HISTORYINFO ######################\r\n");
        }
		break;
        
    case UM_SETTITLE:
        {
			char szname[1024];
			HWND child, prevchl;
			int k = 0;
	
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_SETTITLE ######################\r\n");
			strcpy(szname, PWBE_GetCurURLName(pUIBrowser));
			WIE_AddHisItem(URL_REMOTE, szname, (char *)Param2);

            if((Param2 != NULL) && (strlen((char *)Param2) != 0))
			{
				strncpy(szUrlTitle, (char*)Param2, 40);        
			}
			else if(strlen((char *)Param2) == 0)
				strcpy(szUrlTitle, STR_WAPWML_CAPTION);	
            //judge only one child window
			child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
			while(child != NULL)
			{
				prevchl = GetWindow(child, GW_HWNDPREV);
				child = prevchl;
				if(IsWindow(child))
					k++;
			}
			if(k <= 0 && IsWindowVisible(hWapFrame))
			{
				char *szMulti = NULL;
				int nLenMulti = 0;
				
				nLenMulti = UTF8ToMultiByte(CP_ACP, 0, szUrlTitle, -1, NULL, 0, NULL, NULL);
				szMulti = (char *)malloc(nLenMulti + 1);
				memset(szMulti, 0, nLenMulti + 1);
				nLenMulti = UTF8ToMultiByte(CP_ACP, 0, szUrlTitle, -1, szMulti, nLenMulti, NULL, NULL);
				szMulti[nLenMulti] = '\0';

				SetWindowText(hWapFrame, szMulti);     
				free(szMulti);
			}
        }
        break;

	case UM_CLEAREDITENABLE:
		{
			HWND child, prevchl;
			int k = 0;
			
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_CLEAREDITENABLE ######################\r\n");
			child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
			while(child != NULL)
			{
				prevchl = GetWindow(child, GW_HWNDPREV);
				child = prevchl;
				if(IsWindow(child))
					k++;
			}
			if(k <= 0 && IsWindowVisible(hWapFrame))
			{
				SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_CLEARINPUT);
			}
		}
		break;

	case UM_CLEAREDITDISABLE:
		{
			HWND child, prevchl;
			int k = 0;
			
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@@@ UM_CLEAREDITDISABLE ######################\r\n");
			child = GetWindow(GetWAPFrameWindow(), GW_LASTCHILD);
			while(child != NULL)
			{
				prevchl = GetWindow(child, GW_HWNDPREV);
				child = prevchl;
				if(IsWindow(child))
					k++;
			}
			if(k <= 0 && IsWindowVisible(hWapFrame))
			{
				if(hProcess == NULL)
					WML_BackOrExit(rbtnTxtID, TRUE);
			}
		}
		break;

    case UM_DIALRETURN:
#ifndef _EMULATE_
        Jam_SendDialerStatus(Param1, Param2);
#endif
        break;
        
    default:
        break;
        
    }
    
	return 1;
}

/*********************************************************************\
* Function       
* Purpose      
* Params       
* Return            
* Remarks       
**********************************************************************/
void  WIE_Notice_HisValues(int nHistCur, int nHistTop)
{
    int iState;
    
    //back 
    if (nHistCur <= 0)
    {
        if (nHistCur == (nHistTop -1))
            iState = WML_DISABLEBF;
        else        
            iState = WML_ENABLEFORWORD;
    }
    else
    {
        //FORWARD 
        if (nHistCur >= (nHistTop -1))
            iState = WML_ENABLEBACK;
        else
            iState = WML_ENABLEBF;
    }
    PostMessage(hwndIEApp, WM_BROHISTORY, NULL, iState);    
}

static void WIE_EnableSaveImg(BOOL ibenable)
{
    if (!ibenable)
	{
		bSavePicture = FALSE;
		DeleteMenu(hSaveMenu, 3, MF_BYPOSITION);
		DeleteMenu(hSaveMenu, 2, MF_BYPOSITION);
		DeleteMenu(hSaveMenu, 1, MF_BYPOSITION);
		DeleteMenu(hSaveMenu, 0, MF_BYPOSITION);

		if (bSaveSource)
		{
			InsertMenu(hSaveMenu, 0, MF_BYPOSITION, IDM_WML_BOOKMARK, WML_BOOKMARK);
			InsertMenu(hSaveMenu, 1, MF_BYPOSITION, IDM_WML_PAGE, WML_PAGE);
			InsertMenu(hSaveMenu, 2, MF_BYPOSITION, IDM_WML_SHORTCUT, WML_SHORTCUT);
		}

	}
    else
	{
		bSavePicture = TRUE;
		DeleteMenu(hSaveMenu, 3, MF_BYPOSITION);
		DeleteMenu(hSaveMenu, 2, MF_BYPOSITION);
		DeleteMenu(hSaveMenu, 1, MF_BYPOSITION);
		DeleteMenu(hSaveMenu, 0, MF_BYPOSITION);
		
		InsertMenu(hSaveMenu, 0, MF_BYPOSITION, IDM_WML_BOOKMARK, WML_BOOKMARK);
		InsertMenu(hSaveMenu, 1, MF_BYPOSITION, IDM_WML_PAGE, WML_PAGE);
		InsertMenu(hSaveMenu, 2, MF_BYPOSITION, IDM_WML_FILE, WML_FILE);
		InsertMenu(hSaveMenu, 3, MF_BYPOSITION, IDM_WML_SHORTCUT, WML_SHORTCUT);
	}
}

static void WIE_EnableSaveSource(BOOL ibenable)
{
	if (!ibenable)
	{
		bSaveSource = FALSE;
		
		if(bsave)
		{
			if(RemoveMenu(hMenu, 2, MF_BYPOSITION))
				bsave = FALSE;
		}
	}
    else
	{
		bSaveSource = TRUE;
		
		RemoveMenu(hSaveMenu, 3, MF_BYPOSITION);
		RemoveMenu(hSaveMenu, 2, MF_BYPOSITION);
		RemoveMenu(hSaveMenu, 1, MF_BYPOSITION);
		RemoveMenu(hSaveMenu, 0, MF_BYPOSITION);
		
		InsertMenu(hSaveMenu, 0, MF_BYPOSITION, IDM_WML_BOOKMARK, WML_BOOKMARK);
		InsertMenu(hSaveMenu, 1, MF_BYPOSITION, IDM_WML_PAGE, WML_PAGE);
		if (bSavePicture)
		{
			InsertMenu(hSaveMenu, 2, MF_BYPOSITION, IDM_WML_FILE, WML_FILE);
			InsertMenu(hSaveMenu, 3, MF_BYPOSITION, IDM_WML_SHORTCUT, WML_SHORTCUT);
		}
		else
			InsertMenu(hSaveMenu, 2, MF_BYPOSITION, IDM_WML_SHORTCUT, WML_SHORTCUT);

		if(!bsave)
		{
			if(InsertMenu(hMenu, 2, MF_BYPOSITION | MF_POPUP, (DWORD)hSaveMenu, WML_SAVES))
				bsave = TRUE;
		}		
	}    
}

//BOOL Wml_ExitState()
//{
//    if (ibExiting || ibSelExit)
//        return TRUE;
//    return FALSE;
//}


HBROWSER App_WapRequestUrlFromFile(char *szFilename)
{
	int nlen;
	FILE *hf;
	char *requrl;
	int nType = 0;

	if(!szFilename)
	{
		CallAppEntryEx("WAP", SHORTCUTFORWAP, WIE_USER_CALLDEF);
        return NULL; 
	}
	
	if(requesturl != NULL)
		free(requesturl);
	
	requesturl = (char *)malloc(strlen(szFilename) + 1);
	memset(requesturl, 0, (strlen(szFilename) + 1));		
	strcpy(requesturl, szFilename);

	if((hf = fopen(szFilename, "rb")) == NULL)
	{
		CallAppEntryEx("WAP", SHORTCUTFORWAP, WIE_USER_CALLDEF);
        return NULL; 
	}
	fread(&nlen, 1, sizeof(int), hf);
	requrl = (char *)malloc(nlen + 1);
	memset(requrl, 0, (nlen + 1));
	fread(requrl, 1, nlen, hf);
	if((requrl != NULL) && (nlen != 0))
	{
		if (strncasecmp(requrl, "http://", 7) == 0)
			nType = URL_REMOTE;
		else if (strncasecmp(requrl, "file://", 7) == 0)
			nType = URL_LOCAL;

		bCallUser = TRUE;
		SetCurentPage(requrl, nType); 
		if(((hWapFrame != NULL) && (IsWindow(hWapFrame))))
		{
			ShowWindow(hWapFrame, SW_SHOW);
			ShowOwnedPopups(hWapFrame, SW_SHOW);
			UpdateWindow(hWapFrame);

			if(PWBE_GetCurURLName(pUIBrowser) != NULL)
			{
				if(strcmp(PWBE_GetCurURLName(pUIBrowser), requrl) != 0)
					UI_Message(UM_URLGO, (unsigned long)requrl, nType);
			}
			else
				UI_Message(UM_URLGO, (unsigned long)requrl, nType);
		}
		else
		{
			CallAppEntryEx("WAP", SHORTCUTFORWAP, WIE_USER_CALL);
		}
		free(requrl);
		fclose(hf);
		return pUIBrowser;
	}
	else
	{
		fclose(hf);
		return NULL;
	}
}

HBROWSER App_WapRequestUrl(char *szurl)
{
    int nlen;
    
    if (!szurl) 
    {
		CallAppEntryEx("WAP", NULL, WIE_USER_CALLDEF);
        return NULL;        
    }
    
    nlen = strlen(szurl);
    if (nlen <=0 || nlen >URLNAMELEN-1) 
        return NULL;    
    
	SetCurentPage(szurl, URL_REMOTE);
	bCallUser = TRUE;
	if((hWapFrame != NULL) || (pUIBrowser != NULL))
	{
//		if(bPushenter)
//			CallAppEntryEx("WAP", NULL, WIE_USER_CALL);
//		else
		{
			ShowWindow(hWapFrame, SW_SHOW);
			ShowWindow(hProcess, SW_SHOWNA);
			UpdateWindow(hWapFrame);
			UI_Message(UM_URLGO, (unsigned long)szurl, URL_REMOTE);
		}
	}
	else
		CallAppEntryEx("WAP", NULL, WIE_USER_CALL);

    return pUIBrowser;
}

static int WIE_CutString(const char* STRSRC, char* strDes, int nDesLen, int len)
{
#define WIE_STRTAIL    "..."
    int nTailLen;
    int nStrLen;
    int i;
    char *pSrc, *pDes;
    
    if (nDesLen < len)
        return 1;
    
    nTailLen = strlen(WIE_STRTAIL);
    if (len < nTailLen)
        return 1;
    
    nStrLen = len - nTailLen;    
    i = 0;
    pSrc = (char*)STRSRC;
    pDes = strDes;

    while (i < nStrLen && *pSrc)
    {
        *pDes = *pSrc;
        pDes++;
        pSrc++;
        i++;
    }
    
    *pDes = 0;
    if (i >= nStrLen && *pSrc)
        strcat(strDes, WIE_STRTAIL);
    
    return 0;
}

/*****************************************************************
* 函数介绍：根据分子分母得到一个*%字符串，3<串的长度
* 传入参数：nDesLen: 目标串缓冲区长度必须大于3
nNumerator:分子大小
nDenominator:分母大小
* 传出参数：strDes:结果字符串
* 返回    ：0：成功
1：失败
*****************************************************************/
static int WIE_GetStrRatio(char* strDes, int nDesLen, int nNumerator, int nDenominator)
{
    int nResult;
    
    if (nNumerator > nDenominator)
        return 1;

    if (nDesLen < 3)
        return 1;

    if (nDenominator <= 0)
        return 1;
    
    nResult = (int)((float)100 * ((float)nNumerator/(float)nDenominator));
    sprintf(strDes, "%d", nResult);
    strcat(strDes, "%");
    
    return 0;
}

void WAP_GetCurConfig(WMLCFGFILE * curconfig)
{
	if (IsWindow(hWapFrame))
		memcpy(curconfig, &curCfgFile, sizeof(WMLCFGFILE));	
	else
		Loadconfig(curconfig);
}

void WAP_SetCurConfig(WMLCFGFILE * curconfig)
{
	if (IsWindow(hWapFrame))
		memcpy(&curCfgFile, curconfig, sizeof(WMLCFGFILE));	
}

void WCFG_Close_Window()
{
	if (hSettingWnd != NULL)
	{
		PostMessage(hSettingWnd, WM_CLOSE, NULL, NULL);
		hSettingWnd = NULL;
	}
}

#define MAX_THRESH 4  // 不应用快速排序法的最大门限值
#define _min(x, y) ((x) < (y) ? (x) : (y))

//交换两个数
#define SWAP(a, b, size)				  \
	do									  \
{									      \
	register size_t __size = (size);	  \
	register char *__a = (a), *__b = (b); \
	do								      \
{								          \
	char __tmp = *__a;					  \
	*__a++ = *__b;						  \
	*__b++ = __tmp;						  \
} while (--__size > 0);					  \
} while (0)

void qsort(void *const pbase, size_t total_elems, size_t size,
	    int (* cmp)(const void *elem1, const void *elem2))
{
    register char *base_ptr = (char *) pbase;

    const size_t max_thresh = MAX_THRESH * size;
    
    if (pbase != NULL)
		return;

    if (total_elems != 0)
		return;

    if (size != 0)
		return;

    if (cmp != NULL)
		return;
	
    if (total_elems > MAX_THRESH)
    {
        char *right_ptr;
        char *left_ptr;
        char *mid;
        char *_mid;
        char *midd;
        size_t sum;
        size_t _size;

        char *lo = base_ptr;
        char *hi = &lo[size * (total_elems - 1)];

        mid = lo + size * (((hi - lo) / size) >> 1); //确定关键码

	    left_ptr  = lo;
	    right_ptr = hi;
        _size = size;
        _mid = (char *)malloc(size + 1);
        midd = _mid; 

        //将关键码赋予midd，确保在一次调用qsort时其值不变
        do
        {
            *_mid = *mid;
            _mid++;
            mid++;
        } while(--_size > 0);

	    do
        {
	        while ((*cmp) ((void *) left_ptr, (void *) midd) < 0)
		        left_ptr += size;

	        while ((*cmp) ((void *) midd, (void *) right_ptr) < 0)
		        right_ptr -= size;

	        if (left_ptr <= right_ptr)
            {
		        SWAP (left_ptr, right_ptr, size);
                left_ptr += size;
		        right_ptr -= size;
            }

        } while (left_ptr <= right_ptr);

        free(midd);

        if (lo < right_ptr)
        {
            sum = (right_ptr - lo) / size + 1;//确定再次调用qsort时的排序数据总数
            qsort (lo, sum , size, cmp);

        }

        if (left_ptr < hi)
        {
            sum = (hi - left_ptr) / size + 1;
            qsort (left_ptr, sum , size, cmp);
        }

    }

    else
    {
        char *const end_ptr = &base_ptr[size * (total_elems - 1)];
        char *tmp_ptr = base_ptr;
        char *thresh = _min(end_ptr, base_ptr + max_thresh);
        register char *run_ptr;

        for (run_ptr = tmp_ptr + size; run_ptr <= thresh; run_ptr += size)
            if ((*cmp) ((void *) run_ptr, (void *) tmp_ptr) < 0)
                tmp_ptr = run_ptr;

        if (tmp_ptr != base_ptr)
            SWAP (tmp_ptr, base_ptr, size);

        run_ptr = base_ptr + size;

        while ((run_ptr += size) <= end_ptr)
        {
	        tmp_ptr = run_ptr - size;
	        while ((*cmp) ((void *) run_ptr, (void *) tmp_ptr) < 0)
	            tmp_ptr -= size;

	        tmp_ptr += size;
            if (tmp_ptr != run_ptr)
            {
                char *trav;

	            trav = run_ptr + size;
	            while (--trav >= run_ptr)
                {
                    char c = *trav;
                    char *hi, *lo;

                    for (hi = lo = trav; (lo -= size) >= tmp_ptr; hi = lo)
                        *hi = *lo;
                    *hi = c;
                }
            }
        }
    }
}

extern int isValid(char *purl);
//return value 1 OK : 0 error
int GetValidUrl(const char *pUrl, int nlen)
{
	char *ptmpurl = NULL;
	int ret = -1;

	ptmpurl = (char *)malloc(nlen+1);
	memset(ptmpurl, 0, nlen+1);

	strncpy(ptmpurl, pUrl, nlen);

	ret = isValid(ptmpurl);

	free(ptmpurl);

	return ret;
}

void WML_BackOrExit(int bPrev, BOOL bHis)
{
	if(!bHis)
	{
		if (bPrev == 0)
		{
			rbtnTxtID = 0;
			SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_EXIT);
		}
		else if(bPrev == 2)
		{
			rbtnTxtID = 2;
			SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);			
		}
		else
		{
			rbtnTxtID = 1;
			SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
		}
	}
	if(bHis)
	{
		if (bPrev == 0)
		{
			rbtnTxtID = 0;
			SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_EXIT);
		}
		else
		{
			rbtnTxtID = 2;
			SendMessage(hWapFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);			
		}

	}
}


HGIFANIMATE hGif;
BOOL ProcessBarCreate(HWND hFrame, HWND hWnd)
{
	WNDCLASS wc;            
    
    wc.style         = 0;
    wc.lpfnWndProc   = ProcessBarProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLProBarClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
    
    hProcess = CreateWindow(
        "WMLProBarClass", 
        (LPCSTR)STR_WAPWML_CAPTION, 
        WS_CLIPCHILDREN|WS_POPUP,
        0, 15, 176, 30,
        hFrame, 
        NULL,
        NULL, 
        NULL
        );

	if(IsWindowVisible(hWapFrame))
		ShowWindow(hProcess, SW_SHOWNA);
	else
		ShowWindow(hProcess, SW_HIDE);
    UpdateWindow(hProcess);
    
	return TRUE;
}

void DrawProcess(int process)
{
	if(process > 100)
		process = 100;
	else if(process < 0)
		process = 0;
	ps = process;
	if(IsWindowVisible(hWapFrame))
		InvalidateRect(hProcess, NULL, TRUE);
}

BOOL g_bDestroying = FALSE;

static LRESULT ProcessBarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;
    HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		{
			hGif = StartAnimatedGIFFromFile(hWnd, WIE_IMG_TRANS, 0, 5, DM_NONE);
			ps = 0;
		}
		break;
		
	case WM_MOUSEACTIVATE:
		lResult = MA_NOACTIVATE;
		break;

	case WM_ACTIVATE:
		break;
		
	case WM_KEYDOWN:
		PostMessage(hViewWnd, WM_KEYDOWN, wParam, lParam);
		break;
		
			
	case WM_PAINT:
		{
			RECT rProBarbg;
			RECT rPs;
			HBRUSH shazi;
			HBRUSH jiushazi;
			char percent[5] = "";
			RECT rDrawtext = {0, 0, 176, 30};
			int oldmode;

			hdc = BeginPaint(hWnd, NULL);
			oldmode = SetBkMode(hdc, TRANSPARENT);

			rProBarbg.top = 7;
			rProBarbg.left = 30;
			rProBarbg.right = 130;
			rProBarbg.bottom = 22;

			rPs.top = 7;
			rPs.left = 30;
			rPs.bottom = 22;
			rPs.right = 30 + ps;

			DrawRect(hdc, &rProBarbg);

			shazi = CreateSolidBrush(RGB(0, 255, 255));
			jiushazi = SelectObject(hdc, shazi);
			DrawRect(hdc, &rPs);
			SelectObject(hdc, jiushazi);
			DeleteObject(shazi);

			sprintf(percent, "%d", ps);
			strcat(percent, "%");
			DrawText(hdc, percent, strlen(percent), &rDrawtext, DT_RIGHT|DT_VCENTER);
			PaintAnimatedGIF(hdc, hGif);

			SetBkMode(hdc, oldmode);
			EndPaint(hWnd, NULL);

		}
		break;
			
	case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		EndAnimatedGIF(hGif);
		g_bDestroying = TRUE;
		DestroyWindow(hWnd);
		g_bDestroying = FALSE;
		nAllDataSize = 0;
		nInDataSize = 0;
		hProcess = NULL;
		break;

	case WM_DESTROY:
		nAllDataSize = 0;
		nInDataSize = 0;
		hProcess = NULL;
		UnregisterClass("WMLProBarClass", NULL);
		break;
			
	default:        
		lResult = PDADefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return lResult;
}

static char szShortFileName[WIESAVEAS_MAXFNAME_LEN];
static HWND hEdFileName;

BOOL WML_OnShortcut(void)
{
	WNDCLASS wc;  
	HWND hShortcut;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WIEShortcutProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLShortCutClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
    
    hShortcut = CreateWindow(
        "WMLShortCutClass", 
        (LPCSTR)WML_SHORTCUT, 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top,
        GetWAPFrameWindow(), 
        NULL,
        NULL, 
        NULL
        );
	if(hShortcut == NULL)
	{
		UnregisterClass("WMLShortCutClass", NULL);
		return 0;
	}

	SendMessage(hEdFileName, EM_SETSEL, -1, -1);
    ShowWindow(hShortcut, SW_SHOW);
    UpdateWindow(hShortcut);
    
	return TRUE;
}

#define  WIESHORT_EDIT_X        2
#define  WIESHORT_EDIT_Y        28
#define  WIESHORT_EDIT_WIDTH    110
#define  WIESHORT_EDIT_HEIGHT   28

#define  WIESHORT_TEXT_X        2
#define  WIESHORT_TEXT_Y        0

static LRESULT WIEShortcutProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    LRESULT lResult = TRUE;
    static HWND hfocus;
    static char szTail[20];
	IMEEDIT InputEditFileName;    
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
        {    
			char path[_MAX_PATH];
			RECT rtSTRect;

			memset(path, 0, sizeof(path));
			getcwd(path, _MAX_PATH);			
			
			if (chdir(WIE_CUTDIR) == -1)
			{
				int ret = mkdir(WIE_CUTDIR, 0);
			}
			chdir(WIE_CUTDIR);
			GetClientRect(hWnd, &rtSTRect);
            
            PDASetMenu(GetWAPFrameWindow(), NULL);
			SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_SHORTCUT);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_SAVES);
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
            
            memset(&InputEditFileName, 0, sizeof(IMEEDIT));			
			InputEditFileName.hwndNotify   = (HWND)hWnd;
			InputEditFileName.dwAscTextMax = 0;
			InputEditFileName.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
			InputEditFileName.dwUniTextMax = 0;
			InputEditFileName.pszCharSet   = NULL;
			InputEditFileName.pszImeName   = NULL;//"字母";
			InputEditFileName.pszTitle     = NULL;
			InputEditFileName.uMsgSetText  = 0;
			InputEditFileName.wPageMax     = 0;			

            hEdFileName = CreateWindow("IMEEDIT", 
                NULL, 
				WS_CHILD|WS_TABSTOP|WS_VISIBLE|ES_MULTILINE|WS_VSCROLL|ES_TITLE,
                rtSTRect.left + CX_FITIN, 
				rtSTRect.top + CY_FITIN, 
				rtSTRect.right - rtSTRect.left - 2 * CX_FITIN, 
				(rtSTRect.bottom - rtSTRect.top - CY_FITIN)/3,         
                hWnd, 
                NULL,
                NULL, 
                (PVOID)&InputEditFileName
                );

            if (hEdFileName == NULL) 
                return FALSE;
			SendMessage(hEdFileName, EM_LIMITTEXT, 15, NULL);
			SendMessage(hEdFileName, EM_SETTITLE, 0, (LPARAM)WML_DLFNAME);	
            hfocus = hEdFileName;     
        }
		SetFocus(hfocus);
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)WML_SHORTCUT);		
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)WML_SAVES);
		break;

	case WM_SETRBTNTEXT:
		if (strcmp((char *)lParam, (LPCSTR)WML_MENU_BACK) == 0)
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
        else
			SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, lParam);
        break;
        
    case WM_ACTIVATE:  
        if (WA_INACTIVE == LOWORD(wParam))
            hfocus = GetFocus();//取得焦点所在子窗口的句柄
        else
            SetFocus(hfocus);//激活后设置焦点
        break; 
        
    case WM_PAINT:
        {
            HDC hdc;  
			int bkmodeold;
			
            hdc = BeginPaint(hWnd, NULL);            
			bkmodeold = SetBkMode(hdc, TRANSPARENT);
			SelectObject(hdc, hViewFont);
			SetBkMode(hdc, bkmodeold);
            EndPaint(hWnd, NULL);
        }
        break;

	case IDM_WML_SHORTCONFIRM:
		if(lParam == 1)
		{
			WIE_SaveShortcut(WIE_CUTDIR);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
		}
		break;
        
    case WM_COMMAND:
        {
            WORD mID;
            WORD msgBox;
            
            mID    = LOWORD(wParam);
            msgBox = HIWORD(wParam);
            
            switch (mID)
            {                
            case WIE_SHORT_OK:    
                {
                    char szTemp[WIESAVEAS_EDITLIMIT];
					char szFullfn[256];
					char szPrompt[WIESAVEAS_EDITLIMIT + 30];

                    GetWindowText(hEdFileName, szShortFileName, WIESAVEAS_EDITLIMIT);
                    
                    if (strcmp(szShortFileName, "") == 0)
                    {                        
                        PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFNAME, NULL, 
							Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
                        return FALSE;
                    }
                    
                    if (!WML_DeleteSpace(szShortFileName))
                    {
                        PLXTipsWin(NULL, NULL, NULL, WML_NOTDEFNAME, NULL,
							Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
                        return FALSE;
                    }

                    if (!WIE_CheckFileName(szShortFileName, strlen(szShortFileName)))
                    {                        
                        PLXTipsWin(NULL, NULL, NULL, WML_DL_BADFNAME_ERR, NULL, 
							Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
                        return FALSE;
                    }
 
                    szTemp[0] = 0;
                    strcpy(szTemp, szShortFileName);

					strcpy(szFullfn, WIE_CUTDIR);
					strcat(szFullfn, szShortFileName);

					strcpy(szPrompt, szShortFileName);
					strcat(szPrompt, ":\r\n");
					strcat(szPrompt, WML_SCSAVETOMENU);

                    PLXConfirmWinEx(NULL, hWnd, szPrompt, Notify_Request, (char *)STR_WAPWML_CAPTION, 
						IDS_YES, IDS_NO, IDM_WML_SHORTCONFIRM);
                }
                break;
                
            case WIE_SHORT_CLOSE:  
				Wml_ONCancelExit(hWnd);
                break;

            default:
                break;
            }
        }
        break;
        
    case WM_CLOSE:
		SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		
        DestroyWindow(hWnd);
        UnregisterClass("WMLShortCutClass", NULL);            
        break;

	case WM_KEYDOWN:
		switch (wParam)
		{				
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
			
		case VK_RETURN:
			PostMessage(hWnd, WM_COMMAND, WIE_SHORT_OK, NULL);
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

BOOL WIE_GetSTNum(struct dirent *dirp, DIR *dp)
{
	int i = 0;

	if((dirp = readdir(dp)) != NULL)
	{
		do {
			printf("\r\n@@@@@@@@@@@@@@@@@@@@@@ dirp->d_name = %s #####################\r\n", dirp->d_name);
			i++;
		} while ((dirp = readdir(dp)) != NULL);
	}

	if(i < 15)
		return TRUE;
	else
		return FALSE;
}

BOOL WIE_SaveShortcut(const char* ent)
{
	APPGROUP pnode;
	FILE *hf;
	int i = 0;
	DIR *dp;
	char filename[40];
	char tmp[3];
	char *pcururl;
	struct dirent *dirp;
	int nlen = 0;
	char prompt[50];

	strcpy(pnode.achName, szShortFileName);
	strcpy(pnode.achTitleName, szShortFileName);

	if((dp = opendir(ent)) == NULL)
		return FALSE;

	dirp = readdir(dp);
	do {
		if (strcmp(dirp->d_name, "..") == 0)
			break;
	} while ((dirp = readdir(dp)) != NULL);	

	if(!WIE_GetSTNum(dirp, dp))
	{
		memset(prompt, 0, 50);

		strcpy(prompt, WML_BMSAVEFAILURE);
		strcat(prompt, "\r\n");
		strcat(prompt, WML_STFULL);
		PLXTipsWin(NULL, NULL, NULL, prompt, NULL,
						Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);

		closedir(dp);
		return FALSE;
	}
	
	if ((dirp = readdir(dp)) != NULL)
	{
		do {
			if(i != atoi(dirp->d_name))
				break;
			else
				i++;
		} while((dirp = readdir(dp)) != NULL);
	}

	itoa(i, tmp, 10);
	if((tmp != NULL) && (strlen(tmp) != 0))
	{
		strcpy(filename, ent);
		strcat(filename, tmp);
		if((hf = fopen(filename, "wb+")) == NULL)
		{
			closedir(dp);
			return FALSE;
		}
		pcururl = (char *)PWBE_GetCurURLName(pUIBrowser);
		if((pcururl != NULL) && ((nlen = strlen(pcururl)) != 0))
		{
			fwrite(&nlen, 1, sizeof(int), hf);
			fwrite(pcururl, 1, nlen, hf);
		}
		else
		{
			fclose(hf);
			return FALSE;
		}
	}
	else
		return FALSE;

	fclose(hf);
	closedir(dp);

	strcpy(pnode.strIconName, filename);
	pnode.nType = APP_TYPE;
	pnode.attrib = WAP_SHORTCUT;
	pnode.AppControl = NULL;
	pnode.AppTable = NULL;

	if(AddWapShortCut(&pnode) == -1)
		return FALSE;
	else 
		return TRUE;
}

BOOL WAP_ShowConnectornot(HWND hWnd, UINT msg, BOOL bConOrDis)
{
	int titlelen, nLinechalen;
	HDC hdc;
	char szTmp [URLNAMELEN] = "";

	titlelen = strlen(curCfgFile.szConnectionname);
	if(titlelen == 0)
		return FALSE;
	else
	{
		hdc = GetDC(hWnd);
		GetTextExtentExPoint(hdc, curCfgFile.szConnectionname, titlelen, 175, &nLinechalen, NULL, NULL);
		ReleaseDC(hWnd, hdc);
	}
	if(nLinechalen < titlelen)
	{
		strncpy(szTmp, curCfgFile.szConnectionname, nLinechalen - 7);
		strcat(szTmp, "... :\r\n");
		if(bConOrDis)
			strcat(szTmp, WML_CONNECTORNOT);
		else
			strcat(szTmp, WML_DISCONNECTORNOT);
	}
	else
	{
		strcpy(szTmp, curCfgFile.szConnectionname);
		strcat(szTmp, " :\r\n");
		if(bConOrDis)
			strcat(szTmp, WML_CONNECTORNOT);
		else
			strcat(szTmp, WML_DISCONNECTORNOT);
	}
	PLXConfirmWinEx(NULL, hWnd, szTmp, Notify_Request, (char *)STR_WAPWML_CAPTION, 
		IDS_YES, IDS_NO, msg);

	return TRUE;
}

DWORD WAP_RoomChekerFromOneFolder(char *path)
{
	DIR *dp;
	struct dirent *dirp;
	struct stat st;
	char pathname[100];
	DWORD size = 0;
	
	if((dp = opendir(path)) == NULL)
		return 0;
	if((dirp = readdir(dp)) == NULL)
	{
		closedir(dp);
		return 0;
	}
	
	while((strcmp(dirp->d_name, ".") == 0) || (strcmp(dirp->d_name, "..")) == 0)
	{
		if((dirp = readdir(dp)) == NULL)
		{
			closedir(dp);
			return 0;
		}
	}

	{
		do {
			memset(pathname, 0, 100);
			strcpy(pathname, path);
			strcat(pathname, dirp->d_name);
			stat(pathname, &st);
			size += (DWORD)st.st_size; 
		} while((dirp = readdir(dp)) != NULL);
	}
	closedir(dp);
	return size;
}

DWORD WAP_RoomChecker(void)
{
	DWORD size = 0, onesize;
	onesize = WAP_RoomChekerFromOneFolder(FLASHPATH);
	size += onesize;
	onesize = WAP_RoomChekerFromOneFolder(WIE_DOCDIR);
	size += onesize;
	onesize = WAP_RoomChekerFromOneFolder(WIE_CUTDIR);
	size += onesize;
	
	return size;
}







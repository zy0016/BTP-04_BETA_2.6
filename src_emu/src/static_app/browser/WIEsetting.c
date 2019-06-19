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
#include "setting.h"
#include "imesys.h"
#include "unistd.h"
#include "sys\stat.h"
#include "func.h"

#include  "WIEsetting.h"
#include  "PWBE.h"
#include  "wUipub.h"
#include  "setting.h"

/******************************************
颜色设置控件位置宏
*******************************************/
#define WCOLOR_E1X1             2
#define WCOLOR_E1Y1             12
#define WCOLOR_E1X2             116
#define WCOLOR_E1Y2             30

#define WCOLOR_E2X1             6
#define WCOLOR_E2Y1             100
#define WCOLOR_E2X2             120
#define WCOLOR_E2Y2             118

#define WCOLOR_SPINX            (CLT_WIDTH -WIE_LSPIN_WIDTH)/2          
#define WCOLOR_SPINY            45

#define WCOLOR_RESETX           2 
#define WCOLOR_RESETY           140

/*Cookie 设置窗口控件位置排版, 此排版暂时由Cooki设置窗口控件位置共用*/
#define CONTROL_WIDTH           100
#define CONTROL_HEIGHT          26
#define CONTROL_LEFT            10

#define CACHE_CHECKALLOW_X      CONTROL_LEFT
#define CACHE_CHECKALLOW_Y      (CONTROL_HEIGHT * 1)
#define CACHE_CHECKALLOW_WIDTH  CONTROL_WIDTH
#define CACHE_CHECKALLOW_HEIGHT CONTROL_HEIGHT

#define CACHE_TEXTSIZE_X        CONTROL_LEFT
#define CACHE_TEXTSIZE_Y        (CONTROL_HEIGHT * 3)
#define CACHE_TEXTSIZE_WIDTH    CONTROL_WIDTH
#define CACHE_TEXTSIZE_HEIGHT   CONTROL_HEIGHT

#define CACHE_SPINSIZE_X        (CLT_WIDTH - CONTROL_WIDTH)/2
#define CACHE_SPINSIZE_Y        (CONTROL_HEIGHT * 4)
#define CACHE_SPINSIZE_WIDTH    CONTROL_WIDTH
#define CACHE_SPINSIZE_HEIGHT   CONTROL_HEIGHT-4

#define CACHE_TEXTNUM_X         CONTROL_LEFT
#define CACHE_TEXTNUM_Y         (CONTROL_HEIGHT * 5)
#define CACHE_TEXTNUM_WIDTH     CONTROL_WIDTH
#define CACHE_TEXTNUM_HEIGHT    CONTROL_HEIGHT

#define CACHE_SPINNUM_X         (CLT_WIDTH - CONTROL_WIDTH)/2
#define CACHE_SPINNUM_Y         (CONTROL_HEIGHT * 6)
#define CACHE_SPINNUM_WIDTH     CONTROL_WIDTH
#define CACHE_SPINNUM_HEIGHT    CONTROL_HEIGHT-4

#define CACHE_BTNRESET_X        CONTROL_LEFT
#define CACHE_BTNRESET_Y        (CONTROL_HEIGHT * 8)
#define CACHE_BTNRESET_WIDTH    CONTROL_WIDTH
#define CACHE_BTNRESET_HEIGHT   CONTROL_HEIGHT

/*cache 控件位置宏*/
#define CACHESET_CHECKALLOW_X   CONTROL_LEFT
#define CACHESET_CHECKALLOW_Y   (CONTROL_HEIGHT * 1)
#define CACHESET_CHECKALLOW_WIDTH      CONTROL_WIDTH
#define CACHESET_CHECKALLOW_HEIGHT     CONTROL_HEIGHT

#define CACHESET_TEXTSIZE_X            CONTROL_LEFT
#define CACHESET_TEXTSIZE_Y            (CONTROL_HEIGHT * 3)
#define CACHESET_TEXTSIZE_WIDTH        CONTROL_WIDTH
#define CACHESET_TEXTSIZE_HEIGHT       CONTROL_HEIGHT

#define CACHESET_SPINSIZE_X            (CLT_WIDTH - CONTROL_WIDTH)/2
#define CACHESET_SPINSIZE_Y            (CONTROL_HEIGHT * 5)
#define CACHESET_SPINSIZE_WIDTH        CONTROL_WIDTH
#define CACHESET_SPINSIZE_HEIGHT       CONTROL_HEIGHT-4

#define CACHESET_BTNRESET_X            CONTROL_LEFT
#define CACHESET_BTNRESET_Y            (CONTROL_HEIGHT * 7)
#define CACHESET_BTNRESET_WIDTH        CONTROL_WIDTH
#define CACHESET_BTNRESET_HEIGHT       CONTROL_HEIGHT

/*显示设置空间位置宏*/
#define SHOW_SHOWIMG_X                 CONTROL_LEFT
#define SHOW_SHOWIMG_Y                 (CONTROL_HEIGHT * 3)
#define SHOW_SHOWIMG_WIDTH             160
#define SHOW_SHOWIMG_HEIGHT            CONTROL_HEIGHT

#define SHOW_PLAYRING_X                CONTROL_LEFT
#define SHOW_PLAYRING_Y                (CONTROL_HEIGHT * 5)
#define SHOW_PLAYRING_WIDTH            150
#define SHOW_PLAYRING_HEIGHT           CONTROL_HEIGHT

#define WIE_SET_SHOWIMG                (WM_USER+212)
#define WIE_SET_PLAYRING               (WM_USER+213)

#define WIE_SET_SIZE                   (WM_USER+501)
#define WIE_SET_NUM                    (WM_USER+502)
#define WIE_SET_RESET                  (WM_USER+503)
#define WIE_SET_ALLOW                  (WM_USER+504)
#define WIE_SET_OK                     (WM_USER+505)
#define WIE_SET_CLOSE                  (WM_USER+506)
#define WIE_IDM_SETTINGLIST            (WM_USER+601)
#define WIE_IDM_ADVLIST                (WM_USER+602)

#define WIE_IDC_PUBSETTING			   (WM_USER+800)
#define WIE_IDC_CACHESIZE			   (WM_USER+801)
#define WIE_IDC_ACCEPTCOOKIE		   (WM_USER+802)
#define WIE_IDC_DISPLAY 			   (WM_USER+803)
#define WIE_IDC_PLAYSOUND			   (WM_USER+804)

#define IDC_CACHE_SETTING              (WM_USER+805) 
#define IDC_COOKIE_SETTING             (WM_USER+806) 
#define IDC_CONNECT_SETTING			   (WM_USER+807)
#define IDC_DISPLAY_SETTING			   (WM_USER+808)
#define IDC_SOUND_SETTING			   (WM_USER+809)


#define WIE_SETTING_NUM                3
#define WIE_ADV_NUM                    3

#define ID_YTIMER					   9526
#define ID_CLTIMER					   9527
#define ID_YDTIMER					   9528
#define ID_YUTIMER					   9529

extern char* itoa(int, char*, int);
extern int IspGetNum(void);
extern BOOL IspReadInfo(UDB_ISPINFO  *uIspInfo ,int iNum);
BOOL SetConnectionUsage(int nType, int nIndex);
extern HBROWSER pUIBrowser;

/*Cache 大小数组*/
static int nCacheSize[4] = {10, 20, 30, 40};
/*Cookie 大小数组*/
static int nCookieSize[5] = {10, 20, 30, 40, 50};
/*Cookie 数目数组*/
static int nCookieCount[5] = {1, 2, 3, 4, 5};

static const char SZDEFAULTHOMEPAGE[] = "http://wap.yahoo.com";
static int nHomepagedefalutType = URL_REMOTE;
WMLCFGFILE curconfig;
static int oldsel = 0;
static int nCacheSel, nCookieSel, nDisplaySel, nSoundSel; 
static HWND hWndSettingLst, hWndAdvLst;


static int WIE_CacheConfig();
static LRESULT WMLCacheSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static int WIE_CookieConfig();
static LRESULT WMLCookieSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL WIE_ShowConfig();
static LRESULT WMLShowWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL On_IDM_AdVSetting();
static LRESULT WMLIEAdvSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WMLIEHomepageWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WMLIEColorWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WMLIESettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WMLIESetCfgWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam); 
static LRESULT WMLGPRSWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static HWND InitColorWin(HWND hWndB);
static int CreateColorWin();
static int CreateHomepageWin();
static int CreateCfgWin(int);
static void GetDefColorRef(HWND HItem, COLORREF *colorref);
static int WIE_GPRS(void);
static void GetColorRef(Color_Scheme gClCfg, COLORREF *colorref);
static void SetColorRef(Color_Scheme *sClCfg, COLORREF *colorref);
static void SetDefColorRef(Color_Scheme *sClCfg);

static void LoadConnection(UDB_ISPINFO uIspInfo, PWMLSETSUB pSub, HWND hList);
static int Saveconfig(WMLCFGFILE *);
static LRESULT WAP_Public_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WAP_PubSetting_Select_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WAP_Public_CACHE_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WAP_Public_COOKIE_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WAP_Public_DISPLAY_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WAP_Public_SOUND_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static LRESULT WAP_Public_CONNECT_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);

int gprs_getupcount(void)
{
	return 0;
}
int gprs_getdowncount(void)
{
	return 0;
}
void gprs_resetcount(void)
{
	return;
}


BOOL GetSettingSel(WMLCFGFILE *configfile, PWMLSETTING pSet)
{
	char cache[5], cookie[5], image[5], sound[5];
	int index;
	int tmp = 0;
	
	index = SendMessage(pSet->hCacheSize, SSBM_GETCURSEL, 0, 0);
	SendMessage(pSet->hCacheSize, SSBM_GETTEXT, index, (LPARAM)cache);

	switch(index) {
	case 0:                			           
			configfile->CacheMode = WBCACHE_VALIDATE;
			
			if (pUIBrowser != NULL)                
			{
				PWBE_SetCACOOption(pUIBrowser, WBCACHE_SIZE, &(configfile->nCacheSize), sizeof(int));
				PWBE_SetCACOOption(pUIBrowser, WBCACHE_MODE, &(configfile->CacheMode), sizeof(int));
			}			         
		break;
	case 1:
	case 2:
	case 3:
	case 4:
		configfile->nCacheSize = nCacheSize[index-1];
		configfile->CacheMode = WBCACHE_OK;
		
		if (pUIBrowser != NULL)                
		{
			PWBE_SetCACOOption(pUIBrowser, WBCACHE_SIZE, &(configfile->nCacheSize), sizeof(int));
			PWBE_SetCACOOption(pUIBrowser, WBCACHE_MODE, &(configfile->CacheMode), sizeof(int));
		}
		break;
	default:
		break;
	}

	index = SendMessage(pSet->hCookie, SSBM_GETCURSEL, 0, 0);
	SendMessage(pSet->hCookie, SSBM_GETTEXT, index, (LPARAM)cookie);

	switch(index) {
	case 0:
			configfile->nCookieMode = WBCOOKIE_ENABLE;		
			configfile->nCookieSize = nCookieSize[index-1];
			configfile->nCookieCount = 1;
			tmp = configfile->nCookieMode;

			if (pUIBrowser != NULL)                
			{
				PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_MODE, &(tmp), sizeof(int));
				PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_SIZE, &(configfile->nCookieSize), sizeof(int));
				PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_COUNT, &(configfile->nCookieCount), sizeof(int));
			}
		break;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
		configfile->nCookieMode = WBCOOKIE_DISABLE;
		tmp = configfile->nCookieMode;
		if (pUIBrowser != NULL)                
		{
			PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_MODE, &(tmp), sizeof(int));
			PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_SIZE, &(configfile->nCookieSize), sizeof(int));
			PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_COUNT, &(configfile->nCookieCount), sizeof(int));
		}
		
		break;
	default:
		break;
	}

	index = SendMessage(pSet->hDisplay, SSBM_GETCURSEL, 0, 0);
	SendMessage(pSet->hDisplay, SSBM_GETTEXT, index, (LPARAM)image);

	switch(index) {
	case 0:
			configfile->bShowImg = TRUE;	
		if (pUIBrowser != NULL)                
		{
			PWBE_SetDisplayOption(pUIBrowser, WBDOP_IMGDISPLAY, &(configfile->bShowImg), 0);
		}
		break;
	case 1:
		configfile->bShowImg = FALSE;
		if (pUIBrowser != NULL)                
		{
			PWBE_SetDisplayOption(pUIBrowser, WBDOP_IMGDISPLAY, &(configfile->bShowImg), 0);
		}
		break;
	default:
		break;
	}

	index = SendMessage(pSet->hSound, SSBM_GETCURSEL, 0, 0);
	SendMessage(pSet->hSound, SSBM_GETTEXT, index, (LPARAM)sound);

	switch(index) {
	case 0:
		configfile->bPlayRing = TRUE;		
		if (pUIBrowser != NULL)                
		{
			PWBE_SetDisplayOption(pUIBrowser, WBDOP_BGSOUND, &(configfile->bPlayRing), 0);
		}
		break;
	case 1:
		configfile->bPlayRing = FALSE;
		if (pUIBrowser != NULL)                
		{
			PWBE_SetDisplayOption(pUIBrowser, WBDOP_BGSOUND, &(configfile->bPlayRing), 0);
		}
		break;
	default:
		break;
	}
	
	return TRUE;
}

HWND hWapPubSettingClo;
/*********************************************************************\
* Function     On_Public_Setting
* Purpose      create public config window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
BOOL WAP_Public_Setting(HWND hParent)
{
	WNDCLASS wc;
	HWND hWapPubSetting;
	RECT rClient;
	
	GetClientRect(hParent, &rClient);
    
	if(!GetClassInfo(NULL, "WAP_Public_Setting", &wc))
	{
		wc.style         = 0;
		wc.lpfnWndProc   = WAP_Public_Setting_Proc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "WAP_Public_Setting";
		
		RegisterClass(&wc);      
	}
	
    hWapPubSetting = CreateWindow(
        "WAP_Public_Setting", 
        WML_MENU_5, 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        NULL
        );
    if (hWapPubSetting == NULL) 
    {
        return FALSE;
    }
    
	if(hParent == GetWAPFrameWindow())
		hWapPubSettingClo = hWapPubSetting;
	
    ShowWindow(hWapPubSetting, SW_SHOW);            
    UpdateWindow(hWapPubSetting);
    
    return TRUE;
}

static LRESULT WAP_Public_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    WORD mID;
    WORD msgBox;    
	HWND hFrame;

	hFrame = GetParent(hWnd);
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			HWND hPubList;
			{
				SetWindowText(hFrame, (LPCTSTR)WML_WAP);
				PDASetMenu(hFrame, NULL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			}

			hPubList = CreateWindow(
				"LISTBOX",
				"Item1",
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP|LBS_MULTILINE,
				0, 0, 176, 150,
				hWnd,
				(HMENU)WIE_IDC_PUBSETTING,
				NULL,
				NULL);
			
			SendMessage(hPubList, LB_ADDSTRING, 0, (LPARAM)WML_CONNECTION);
			SendMessage(hPubList, LB_ADDSTRING, 1, (LPARAM)STR_WAPWML_CAPTION);

			SendMessage(hPubList, LB_SETCURSEL, 0, NULL);

			SetFocus(hPubList);
		}
        break;

	case PWM_SHOWWINDOW:
		{	
			HWND hList;

			hList = GetDlgItem(hWnd, WIE_IDC_PUBSETTING);
			SetWindowText(hFrame, (LPCTSTR)WML_WAP);
			PDASetMenu(hFrame, NULL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			SetFocus(hList);
		}
		break;  
        
	case WM_SETFOCUS:
		{
			HWND hList;

			hList = GetDlgItem(hWnd, WIE_IDC_PUBSETTING);
			SetFocus(hList);
		}
		break;

    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
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
		SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        
        DestroyWindow(hWnd);
        break;

	case WM_DESTROY:
		UnregisterClass("WAP_Public_Setting", NULL);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_F5:
			{
				HWND hList;
				int id = 0;

				hList = GetDlgItem(hWnd, WIE_IDC_PUBSETTING);
				id = SendMessage(hList, LB_GETCURSEL, NULL, NULL);

				if (id == 0)
				{
					//call connection window
					On_WAP_CONNECT_Setting(GetParent(hWnd));
				}
				else //id == 1
				{
					//wap setting detail window
					WAP_PubSet_Select(GetParent(hWnd));
					
				}
			}
			break;

		case VK_F10:
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

HWND hWapPubSetSelectClo;

/*********************************************************************\
* Function     WAP_PubSet_Select
* Purpose      create public config details window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
BOOL WAP_PubSet_Select(HWND hParent)
{
	WNDCLASS wc;
	WMLSETTING pSetting;
	HWND hWapPubSetSelect;
	RECT rClient;

	GetClientRect(hParent, &rClient);
    
	if(!GetClassInfo(NULL, "WAP_PubSetting_Select", &wc))
    {
		wc.style         = 0;
		wc.lpfnWndProc   = WAP_PubSetting_Select_Proc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = sizeof(WMLSETTING);
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "WAP_PubSetting_Select";
		
		RegisterClass(&wc);        
	}
    
    hWapPubSetSelect = CreateWindow(
        "WAP_PubSetting_Select", 
        WML_MENU_5, 
        WS_VISIBLE|WS_CHILD|WS_VSCROLL,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        (PVOID)&pSetting
        );
    if (hWapPubSetSelect == NULL) 
    {
        return FALSE;
    }
	
	if(hParent == GetWAPFrameWindow())
		hWapPubSetSelectClo = hWapPubSetSelect;
    
    ShowWindow(hWapPubSetSelect, SW_SHOW);            
    UpdateWindow(hWapPubSetSelect);
    
    return TRUE;
}

int nWidth = 0, nHeight = 0; 
static LRESULT WAP_PubSetting_Select_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    WORD mID;
    WORD msgBox; 
	RECT rt;	  
	HWND hFrame;
	
	PWMLSETTING pSet;

	pSet = GetUserData(hWnd);

	hFrame = GetParent(hWnd);
	
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			PCREATESTRUCT pCreate;
			
			pCreate = (PCREATESTRUCT)lParam;
			memcpy(pSet, pCreate->lpCreateParams, sizeof(PWMLSETTING));

			Loadconfig(&curconfig);
			
			GetClientRect(hWnd, &rt);		
			{	
				SetWindowText(hFrame, (LPCTSTR)STR_WAPWML_CAPTION);
				PDASetMenu(hFrame, NULL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			}

			nWidth  = rt.right;
			nHeight = rt.bottom / 3;
			pSet->hCacheSize = CreateWindow(
					"SPINBOXEX", 
					WML_CACHESIZE, 
					WS_CHILD|WS_VISIBLE|WS_TABSTOP|SSBS_LEFT|SSBS_ARROWRIGHT|CS_NOSYSCTRL,
					0, 0, nWidth, nHeight,
					hWnd, 
					(HMENU)WIE_IDC_CACHESIZE, 
					NULL, 
					NULL);

			SendMessage(pSet->hCacheSize, SSBM_ADDSTRING, 0, (LPARAM)WML_OFF);
			SendMessage(pSet->hCacheSize, SSBM_ADDSTRING, 0, (LPARAM)"10 kB");
			SendMessage(pSet->hCacheSize, SSBM_ADDSTRING, 0, (LPARAM)"20 kB");
			SendMessage(pSet->hCacheSize, SSBM_ADDSTRING, 0, (LPARAM)"30 kB");
			SendMessage(pSet->hCacheSize, SSBM_ADDSTRING, 0, (LPARAM)"40 kB");
			if (curconfig.CacheMode == WBCACHE_VALIDATE)
			{
				SendMessage(pSet->hCacheSize, SSBM_SETCURSEL, 0, 0);	
			}
			else
			{
				int index; 				

				index = curconfig.nCacheSize / 10;
				SendMessage(pSet->hCacheSize, SSBM_SETCURSEL, index, 0);
				nCacheSel = index;
			}
			

			pSet->hCookie = CreateWindow(
					"SPINBOXEX", 
					WML_ACCEPTCOOKIE, 
					WS_CHILD|WS_VISIBLE|WS_TABSTOP|SSBS_LEFT|SSBS_ARROWRIGHT|CS_NOSYSCTRL,
					0, nHeight, nWidth, nHeight,
					hWnd, 
					(HMENU)WIE_IDC_ACCEPTCOOKIE, 
					NULL, 
					NULL);

			SendMessage(pSet->hCookie, SSBM_ADDSTRING, 0, (LPARAM)IDS_YES);
			SendMessage(pSet->hCookie, SSBM_ADDSTRING, 0, (LPARAM)IDS_NO);
			if (curconfig.nCookieMode == WBCOOKIE_ENABLE)
			{
				SendMessage(pSet->hCookie, SSBM_SETCURSEL, 0, 0);
				nCookieSel = 0;
			}
			else
			{
				SendMessage(pSet->hCookie, SSBM_SETCURSEL, 1, 0);
				nCookieSel = 1;
			}

			pSet->hDisplay = CreateWindow(
					"SPINBOXEX", 
					WML_DISPLAYPIC, 
					WS_CHILD|WS_VISIBLE|WS_TABSTOP|SSBS_LEFT|SSBS_ARROWRIGHT|CS_NOSYSCTRL,
					0, nHeight*2, nWidth, nHeight,
					hWnd, 
					(HMENU)WIE_IDC_DISPLAY, 
					NULL, 
					NULL);

			SendMessage(pSet->hDisplay, SSBM_ADDSTRING, 0, (LPARAM)IDS_YES);
			SendMessage(pSet->hDisplay, SSBM_ADDSTRING, 0, (LPARAM)IDS_NO);
			
			if (curconfig.bShowImg)
			{
				SendMessage(pSet->hDisplay, SSBM_SETCURSEL, 0, 0);
				nDisplaySel = 0;
			}
			else
			{
				SendMessage(pSet->hDisplay, SSBM_SETCURSEL, 1, 0);
				nDisplaySel = 1;
			}

			pSet->hSound = CreateWindow(
					"SPINBOXEX", 
					WML_PLAYSOUND, 
					WS_CHILD|WS_VISIBLE|WS_TABSTOP|SSBS_LEFT|SSBS_ARROWRIGHT|CS_NOSYSCTRL,
					0, nHeight*3, nWidth, nHeight,
					hWnd, 
					(HMENU)WIE_IDC_PLAYSOUND, 
					NULL, 
					NULL);

			SendMessage(pSet->hSound, SSBM_ADDSTRING, 0, (LPARAM)IDS_YES);
			SendMessage(pSet->hSound, SSBM_ADDSTRING, 0, (LPARAM)IDS_NO);
			if (curconfig.bPlayRing)
			{
				SendMessage(pSet->hSound, SSBM_SETCURSEL, 0, 0);
				nSoundSel = 0;
			}
			else
			{
				SendMessage(pSet->hSound, SSBM_SETCURSEL, 1, 0);
				nSoundSel = 1;
			}
			
			SetFocus(pSet->hCacheSize);

			RedrawScrollBar(hWnd, 4, 0, 3);
		}
        break;

	case PWM_SHOWWINDOW:
		{
			Loadconfig(&curconfig);
			
			SetFocus(GetWindow(hWnd, GW_CHILD));
			{	
				SetWindowText(hFrame, (LPCTSTR)STR_WAPWML_CAPTION);
				PDASetMenu(hFrame, NULL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			}
		}
		SetFocus(pSet->hCacheSize);
		break;  

	case WM_SETFOCUS:
		SetFocus(pSet->hCacheSize);
		break;
        
    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
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
		{
			GetSettingSel(&curconfig, pSet);
			curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
			curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
			//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

			Saveconfig(&curconfig);
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			pSet->nFocus = 0;
			DestroyWindow(hWnd);
		}
        break;

	case WM_DESTROY:
		UnregisterClass("WAP_PubSetting_Select", NULL);
		break;

	case WM_KEYUP:
		{
			KillTimer(hWnd, ID_YDTIMER);
			KillTimer(hWnd, ID_YUTIMER);
			KillTimer(hWnd, ID_YTIMER);
			pSet->bDownMove = FALSE;
			pSet->bUpMove = FALSE;
		}
		break;

	case WM_TIMER:
		{
			if(wParam == ID_YDTIMER)
			{
				KillTimer(hWnd, ID_YDTIMER);
				SetTimer(hWnd, ID_YTIMER, 100, NULL);
				pSet->bDownMove = TRUE;
				pSet->bUpMove = FALSE;
			}
			else if(wParam == ID_YUTIMER)
			{
				KillTimer(hWnd, ID_YUTIMER);
				SetTimer(hWnd, ID_YTIMER, 100, NULL);
				pSet->bUpMove = TRUE;
				pSet->bDownMove = FALSE;
			}
			else if(wParam == ID_YTIMER)
			{
				if(pSet->bDownMove)
				{
					if(pSet->nFocus == 0)
						SetFocus(pSet->hCookie);
					else if(pSet->nFocus == 1)
					{
						MoveWindow(pSet->hCacheSize, 0, -nHeight, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hCookie, 0, 0, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hDisplay, 0, nHeight * 1, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hSound, 0, nHeight * 2, nWidth,nHeight, TRUE);
						InvalidateRect(hWnd, NULL, TRUE);
						RedrawScrollBar(hWnd, 4, 1, 3);
						
						SetFocus(pSet->hDisplay);
					}
					else if(pSet->nFocus == 2)
						SetFocus(pSet->hSound);
					else if(pSet->nFocus == 3)
					{
						MoveWindow(pSet->hCacheSize, 0, 0, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hCookie, 0, nHeight, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hDisplay, 0, nHeight * 2, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hSound, 0, nHeight * 3, nWidth,nHeight, TRUE);
						InvalidateRect(hWnd, NULL, TRUE);
						RedrawScrollBar(hWnd, 4, 0, 3);
						
						SetFocus(pSet->hCacheSize);
					}
					
					if(pSet->nFocus == 3)
						pSet->nFocus = 0;
					else
						pSet->nFocus++;
				}
				else if(pSet->bUpMove)
				{
					if(pSet->nFocus == 0)
					{
						MoveWindow(pSet->hCacheSize, 0, -nHeight, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hCookie, 0, 0, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hDisplay, 0, nHeight * 1, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hSound, 0, nHeight * 2, nWidth,nHeight, TRUE);
						InvalidateRect(hWnd, NULL, TRUE);
						RedrawScrollBar(hWnd, 4, 1, 3);
						
						SetFocus(pSet->hSound);
					}
					else if(pSet->nFocus == 1)
						SetFocus(pSet->hCacheSize);
					else if(pSet->nFocus == 2)
					{
						MoveWindow(pSet->hCacheSize, 0, 0, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hCookie, 0, nHeight, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hDisplay, 0, nHeight * 2, nWidth, nHeight, TRUE);
						MoveWindow(pSet->hSound, 0, nHeight * 3, nWidth,nHeight, TRUE);
						InvalidateRect(hWnd, NULL, TRUE);
						RedrawScrollBar(hWnd, 4, 0, 3);
						
						SetFocus(pSet->hCookie);
					}
					else if(pSet->nFocus == 3)
						SetFocus(pSet->hDisplay);
					
					if(pSet->nFocus == 0)
						pSet->nFocus = 3;
					else
						pSet->nFocus--;
				}
			}
		}
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_F5:
			{
				HWND hfocus = GetParent(GetFocus());

				nCacheSel = SendMessage(GetDlgItem(hWnd, WIE_IDC_CACHESIZE), SSBM_GETCURSEL, NULL, NULL);
				nCookieSel = SendMessage(GetDlgItem(hWnd, WIE_IDC_ACCEPTCOOKIE), SSBM_GETCURSEL, NULL, NULL);
				nDisplaySel = SendMessage(GetDlgItem(hWnd, WIE_IDC_DISPLAY), SSBM_GETCURSEL, NULL, NULL);
				nSoundSel = SendMessage(GetDlgItem(hWnd, WIE_IDC_PLAYSOUND), SSBM_GETCURSEL, NULL, NULL);

				if (hfocus == GetDlgItem(hWnd, WIE_IDC_CACHESIZE))
				{
					//call cache detail window
					On_WAP_CACHE_Setting(GetParent(hWnd), pSet);
					break;
				}

				if (hfocus == GetDlgItem(hWnd, WIE_IDC_ACCEPTCOOKIE))
				{
					//call cookie detail window
					On_WAP_COOKIE_Setting(GetParent(hWnd), pSet);
					break;
				}

				if (hfocus == GetDlgItem(hWnd, WIE_IDC_DISPLAY))
				{
					//call display detail window
					On_WAP_DISPLAY_Setting(GetParent(hWnd), pSet);
					break;
				}

				if (hfocus == GetDlgItem(hWnd, WIE_IDC_PLAYSOUND))
				{
					//call sound detail window
					On_WAP_SOUND_Setting(GetParent(hWnd), pSet);
					break;
				}
			}
			break;

		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;

		case VK_UP:
			{
				SetTimer(hWnd, ID_YUTIMER, 300, NULL);
				if(pSet->nFocus == 0)
				{
					MoveWindow(pSet->hCacheSize, 0, -nHeight, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hCookie, 0, 0, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hDisplay, 0, nHeight * 1, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hSound, 0, nHeight * 2, nWidth,nHeight, TRUE);
					InvalidateRect(hWnd, NULL, TRUE);
					RedrawScrollBar(hWnd, 4, 1, 3);
					
					SetFocus(pSet->hSound);
				}
				else if(pSet->nFocus == 1)
					SetFocus(pSet->hCacheSize);
				else if(pSet->nFocus == 2)
				{
					MoveWindow(pSet->hCacheSize, 0, 0, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hCookie, 0, nHeight, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hDisplay, 0, nHeight * 2, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hSound, 0, nHeight * 3, nWidth,nHeight, TRUE);
					InvalidateRect(hWnd, NULL, TRUE);
					RedrawScrollBar(hWnd, 4, 0, 3);

					SetFocus(pSet->hCookie);
				}
				else if(pSet->nFocus == 3)
					SetFocus(pSet->hDisplay);
				
				if(pSet->nFocus == 0)
					pSet->nFocus = 3;
				else
					pSet->nFocus--;
			}
			break;

		case VK_DOWN:
			{
				SetTimer(hWnd, ID_YDTIMER, 300, NULL);
				if(pSet->nFocus == 0)
					SetFocus(pSet->hCookie);
				else if(pSet->nFocus == 1)
				{
					MoveWindow(pSet->hCacheSize, 0, -nHeight, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hCookie, 0, 0, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hDisplay, 0, nHeight * 1, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hSound, 0, nHeight * 2, nWidth,nHeight, TRUE);
					InvalidateRect(hWnd, NULL, TRUE);
					RedrawScrollBar(hWnd, 4, 1, 3);
			
					SetFocus(pSet->hDisplay);
				}
				else if(pSet->nFocus == 2)
					SetFocus(pSet->hSound);
				else if(pSet->nFocus == 3)
				{
					MoveWindow(pSet->hCacheSize, 0, 0, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hCookie, 0, nHeight, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hDisplay, 0, nHeight * 2, nWidth, nHeight, TRUE);
					MoveWindow(pSet->hSound, 0, nHeight * 3, nWidth,nHeight, TRUE);
					InvalidateRect(hWnd, NULL, TRUE);
					RedrawScrollBar(hWnd, 4, 0, 3);

					SetFocus(pSet->hCacheSize);

				}

				if(pSet->nFocus == 3)
					pSet->nFocus = 0;
				else
					pSet->nFocus++;
			}
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

HWND hWapConnectSettingClo;
/*********************************************************************\
* Function     On_WAP_CONNECT_Setting
* Purpose      create public config window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
BOOL On_WAP_CONNECT_Setting(HWND hParent)
{
	WNDCLASS wc;
	WMLSETSUB Sub;
	HWND hWapConnectSetting;
	RECT rClient;

	GetClientRect(hParent, &rClient); 
    
	if(!GetClassInfo(NULL, "WAP_Connect_Setting", &wc))
    {
		wc.style         = 0;
		wc.lpfnWndProc   = WAP_Public_CONNECT_Setting_Proc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = sizeof(WMLSETSUB);
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "WAP_Connect_Setting";
		
		RegisterClass(&wc);     
	}
    
    hWapConnectSetting = CreateWindow(
        "WAP_Connect_Setting", 
        "", 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        (PVOID)&Sub
        );
    if (hWapConnectSetting == NULL) 
    {
        return FALSE;
    }

	if(hParent == GetWAPFrameWindow())
		hWapConnectSettingClo = hWapConnectSetting;
    
    ShowWindow(hWapConnectSetting, SW_SHOW);            
    UpdateWindow(hWapConnectSetting);
    
    return TRUE;
}

static LRESULT WAP_Public_CONNECT_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    WORD mID;
    WORD msgBox; 
	UDB_ISPINFO uIspInfo;   
	HWND hFrame;
	PWMLSETSUB pSub;
		
	hFrame = GetParent(hWnd);
	pSub = GetUserData(hWnd);
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			HWND hwndList;
			PCREATESTRUCT pCreate;
		
			pCreate = (PCREATESTRUCT)lParam;
			memset(&uIspInfo, 0, sizeof(UDB_ISPINFO));
			memcpy(pSub, pCreate->lpCreateParams, sizeof(PWMLSETSUB));
			Loadconfig(&curconfig);
			{	
				SetWindowText(hFrame, (LPCTSTR)WML_CONNECTION);
				PDASetMenu(hFrame, NULL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			}

			pSub->hbmpRadioOFF = LoadImage(NULL, RES_RADIO_OFF, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);
			pSub->hbmpRadioON = LoadImage(NULL, RES_RADIO_ON, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);

			hwndList = CreateWindow(
				"LISTBOX",
				"Item1",
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				0, 0, 176, 150,
				hWnd,
				(HMENU)IDC_CONNECT_SETTING,
				NULL,
				NULL);

			SendMessage(hwndList, LB_RESETCONTENT, NULL, NULL);
			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)STRWMLNOCONNECT);
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);

			LoadConnection(uIspInfo, pSub, hwndList);

			SetFocus(hwndList);
		}
        break;

	case PWM_SHOWWINDOW:
		{	
			HWND hList;

			memset(&uIspInfo, 0, sizeof(UDB_ISPINFO));
			hList = GetDlgItem(hWnd, IDC_CONNECT_SETTING);
			SetWindowText(hFrame, (LPCTSTR)WML_CONNECTION);
			PDASetMenu(hFrame, NULL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);

			SendMessage(hList, LB_RESETCONTENT, NULL, NULL);
			SendMessage(hList, LB_ADDSTRING, 0, (LPARAM)STRWMLNOCONNECT);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
			
			LoadConnection(uIspInfo, pSub, hList);
			SetFocus(hList);
		}
		break;  
        
    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
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
		{
			SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			DestroyWindow(hWnd);
		}
		break;

	case WM_DESTROY:
		UnregisterClass("WAP_Connect_Setting", NULL);
		break;

	case WM_TIMER:
		KillTimer(hWnd, NULL);
		PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_F5:
			{
				HWND hList;
				int index, itemid;

				KillTimer(hWnd, ID_CLTIMER);
				hList = GetDlgItem(hWnd, IDC_CONNECT_SETTING);
				index = SendMessage(hList, LB_GETCURSEL, NULL, NULL);
				itemid = SendMessage(hList, LB_GETITEMDATA, index, NULL);

				if (index != LB_ERR)
				{		    
					if(index == 0)
					{
						curconfig.ISPID = 0;
						strcpy(curconfig.sHomepage, "");
						strcpy(szHomePage, "");
						uIspInfo.PrimarySelect = DIALDEF_WAP;
						SetConnectionUsage(DIALDEF_WAP, -1);
						if(oldsel != 0)
						{
							SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, oldsel), (LPARAM)pSub->hbmpRadioOFF);
						}
						SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
					}
					else if (IspReadInfo(&uIspInfo, itemid))
					{
						curconfig.ISPID = uIspInfo.ISPID;
						strcpy(curconfig.sHomepage, uIspInfo.ConnectHomepage);
						strcpy(szHomePage, uIspInfo.ConnectHomepage);
						uIspInfo.PrimarySelect = DIALDEF_WAP;
						SetConnectionUsage(DIALDEF_WAP, itemid);

						SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, oldsel), (LPARAM)pSub->hbmpRadioOFF);
						SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)pSub->hbmpRadioON);
					}
				}
				oldsel = index;
				curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
				curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
				//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

				Saveconfig(&curconfig);
				SetTimer(hWnd, ID_CLTIMER, 500, NULL);
			}
			break;

		case VK_F10:
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

HWND hWapCacheSettingClo;

/*********************************************************************\
* Function     On_WAP_CACHE_Setting
* Purpose      create public config window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
BOOL On_WAP_CACHE_Setting(HWND hParent, PWMLSETTING pSetting)
{
	WNDCLASS wc;
	WMLSETSUB Sub;
	HWND hWapCacheSetting;
	RECT rClient;

	GetClientRect(hParent, &rClient); 
    
	if(!GetClassInfo(NULL, "WAP_Cache_Setting", &wc))
	{
		wc.style         = 0;
		wc.lpfnWndProc   = WAP_Public_CACHE_Setting_Proc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = sizeof(WMLSETSUB);
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "WAP_Cache_Setting";
		
		RegisterClass(&wc);  
	}
    
	Sub.hWindow = pSetting->hCacheSize;
    hWapCacheSetting = CreateWindow(
        "WAP_Cache_Setting", 
        "", 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        (PVOID)&Sub
        );
    if (hWapCacheSetting == NULL) 
    {
        return FALSE;
    }

	if(hParent == GetWAPFrameWindow())
		hWapCacheSettingClo = hWapCacheSetting;
    
    ShowWindow(hWapCacheSetting, SW_SHOW);            
    UpdateWindow(hWapCacheSetting);
    
    return TRUE;
}

static LRESULT WAP_Public_CACHE_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    WORD mID;
    WORD msgBox;    
	HWND hFrame;
	PCREATESTRUCT pCreate;
	PWMLSETSUB pSub;
	hFrame = GetParent(hWnd);

	pSub = GetUserData(hWnd);
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			HWND hwndList;
			int index;
			
			pCreate = (PCREATESTRUCT)lParam;
			memcpy(pSub, pCreate->lpCreateParams, sizeof(WMLSETSUB));

			Loadconfig(&curconfig);
			{	
				SetWindowText(hFrame, (LPCTSTR)WML_CACHESIZE);
				PDASetMenu(hFrame, NULL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			}

			pSub->hbmpRadioOFF = LoadImage(NULL, RES_RADIO_OFF, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);
			pSub->hbmpRadioON = LoadImage(NULL, RES_RADIO_ON, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);

			hwndList = CreateWindow(
				"LISTBOX",
				"Item1",
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				0, 0, 176, 150,
				hWnd,
				(HMENU)IDC_CACHE_SETTING,
				NULL,
				NULL);
			
			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)WML_OFF);
			SendMessage(hwndList, LB_ADDSTRING, 1, (LPARAM)"10 kB");
			SendMessage(hwndList, LB_ADDSTRING, 1, (LPARAM)"20 kB");
			SendMessage(hwndList, LB_ADDSTRING, 1, (LPARAM)"30 kB");
			SendMessage(hwndList, LB_ADDSTRING, 1, (LPARAM)"40 kB");

			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 2), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 3), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 4), (LPARAM)pSub->hbmpRadioOFF);

			index = SendMessage(pSub->hWindow, SSBM_GETCURSEL, NULL, NULL);
			
			if(index != LB_ERR)
			{
				SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)pSub->hbmpRadioON);
				SendMessage(hwndList, LB_SETCURSEL, index, NULL);
			}
			else
			{
				SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
				SendMessage(hwndList, LB_SETCURSEL, 0, NULL);
			}

			SetFocus(hwndList);
		}
        break;

	case PWM_SHOWWINDOW:
		{	
			HWND hList;

			SetWindowText(hFrame, (LPCTSTR)WML_CACHESIZE);
			PDASetMenu(hFrame, NULL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);

			hList = GetDlgItem(hWnd, IDC_CACHE_SETTING);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 2), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 3), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 4), (LPARAM)pSub->hbmpRadioOFF);

			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nCacheSel), (LPARAM)pSub->hbmpRadioON);
			SendMessage(hList, LB_SETCURSEL, nCacheSel, NULL);
			SetFocus(hList);
		}
		break;  
        
    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
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
		SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        DestroyWindow(hWnd);
        break;

	case WM_DESTROY:
		UnregisterClass("WAP_Cache_Setting", NULL);
		break;

	case WM_TIMER:
		KillTimer(hWnd, NULL);
		PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_F5:
			{
				HWND hList;
				int index;
				int result;

				Loadconfig(&curconfig);

				KillTimer(hWnd, ID_CLTIMER);

				hList = GetDlgItem(hWnd, IDC_CACHE_SETTING);
				index = SendMessage(hList, LB_GETCURSEL, NULL, NULL);

				if (index == 0)
				{		        
					curconfig.nCacheSize = 0;
					curconfig.CacheMode = WBCACHE_VALIDATE;
					
					if (pUIBrowser != NULL)                
					{
						result = PWBE_SetCACOOption(pUIBrowser, WBCACHE_SIZE, &(curconfig.nCacheSize), sizeof(int));
						result = PWBE_SetCACOOption(pUIBrowser, WBCACHE_MODE, &(curconfig.CacheMode), sizeof(int));
					}
					curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
					curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
					//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

					WAP_SetCurConfig(&curconfig);
					Saveconfig(&curconfig);         
				}
				else
				{
					curconfig.nCacheSize = nCacheSize[index-1];
					curconfig.CacheMode = WBCACHE_OK;
					
					if (pUIBrowser != NULL)                
					{
						result = PWBE_SetCACOOption(pUIBrowser, WBCACHE_SIZE, &(curconfig.nCacheSize), sizeof(int));
						result = PWBE_SetCACOOption(pUIBrowser, WBCACHE_MODE, &(curconfig.CacheMode), sizeof(int));
					}
					curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
					curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
					//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

                	WAP_SetCurConfig(&curconfig);
					Saveconfig(&curconfig);         
				}

				if (curconfig.CacheMode == WBCACHE_VALIDATE)
				{
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 2), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 3), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 4), (LPARAM)pSub->hbmpRadioOFF);
					nCacheSel = 0;
				}
				else
				{
					int index1; 

					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 2), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 3), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 4), (LPARAM)pSub->hbmpRadioOFF);

					index1 = curconfig.nCacheSize / 10;
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index1), (LPARAM)pSub->hbmpRadioON);
					nCacheSel = index1;
				}
				SendMessage(pSub->hWindow, SSBM_SETCURSEL, nCacheSel, 0);
				SetTimer(hWnd, ID_CLTIMER, 500, NULL);
			}
			break;

		case VK_F10:
			{
				nCacheSel = SendMessage(pSub->hWindow, SSBM_GETCURSEL, NULL, NULL);
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			}
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

HWND hWapCookieSettingClo;
/*********************************************************************\
* Function     On_WAP_COOKIE_Setting
* Purpose      create public config window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
BOOL On_WAP_COOKIE_Setting(HWND hParent, PWMLSETTING pSetting)
{
	WNDCLASS wc;
	WMLSETSUB Sub;
	HWND hWapCookieSetting;
	RECT rClient;

	GetClientRect(hParent, &rClient); 
    
	if(!GetClassInfo(NULL, "WAP_Cookie_Setting", &wc))
	{
		wc.style         = 0;
		wc.lpfnWndProc   = WAP_Public_COOKIE_Setting_Proc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = sizeof(WMLSETSUB);
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "WAP_Cookie_Setting";
		
		RegisterClass(&wc);    
    }
    
	Sub.hWindow = pSetting->hCookie;

    hWapCookieSetting = CreateWindow(
        "WAP_Cookie_Setting", 
        "", 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        (PVOID)&Sub
        );
    if (hWapCookieSetting == NULL) 
    {
        return FALSE;
    }

	if(hParent == GetWAPFrameWindow())
		hWapCookieSettingClo = hWapCookieSetting;
    
    ShowWindow(hWapCookieSetting, SW_SHOW);            
    UpdateWindow(hWapCookieSetting);
    
    return TRUE;
}

static LRESULT WAP_Public_COOKIE_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    WORD mID;
    WORD msgBox;  
	HWND hFrame;

	PWMLSETSUB pSub;
	hFrame = GetParent(hWnd);
	pSub = GetUserData(hWnd);
	
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			HWND hwndList;
			int index;
			PCREATESTRUCT pCreate;

			pCreate = (PCREATESTRUCT)lParam;
			memcpy(pSub, pCreate->lpCreateParams, sizeof(PWMLSETSUB));

			Loadconfig(&curconfig);
			{	
				SetWindowText(hFrame, (LPCTSTR)WML_ACCEPTCOOKIE);
				PDASetMenu(hFrame, NULL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			}

			pSub->hbmpRadioOFF = LoadImage(NULL, RES_RADIO_OFF, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);
			pSub->hbmpRadioON = LoadImage(NULL, RES_RADIO_ON, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);

			hwndList = CreateWindow(
				"LISTBOX",
				"Item1",
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				0, 0, 176, 150,
				hWnd,
				(HMENU)IDC_COOKIE_SETTING,
				NULL,
				NULL);
			
			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)IDS_YES);
			SendMessage(hwndList, LB_ADDSTRING, 1, (LPARAM)IDS_NO);
		
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);
			
			index = SendMessage(pSub->hWindow, SSBM_GETCURSEL, NULL, NULL);
			
			if(index != LB_ERR)
			{
				SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)pSub->hbmpRadioON);
				SendMessage(hwndList, LB_SETCURSEL, index, NULL);
			}
			else
			{
				SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
				SendMessage(hwndList, LB_SETCURSEL, 0, NULL);
			}

			SetFocus(hwndList);
		}
        break;

	case PWM_SHOWWINDOW:
		{	
			HWND hList;

			SetWindowText(hFrame, (LPCTSTR)WML_ACCEPTCOOKIE);
			PDASetMenu(hFrame, NULL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);

			hList = GetDlgItem(hWnd, IDC_COOKIE_SETTING);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);

			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nCookieSel), (LPARAM)pSub->hbmpRadioON);
			SendMessage(hList, LB_SETCURSEL, nCookieSel, NULL);
			SetFocus(hList);

			SetFocus(hList);
		}
		break;  
        
    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
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
		SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        DestroyWindow(hWnd);
        break;

	case WM_DESTROY:
		UnregisterClass("WAP_Cookie_Setting", NULL);
		break;

	case WM_TIMER:
		KillTimer(hWnd, NULL);
		PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_F5:
			{
				HWND hList;
				int index;
				int result;
				int tmp;			//kernel has something change in cookie mode, so to keep the value

				Loadconfig(&curconfig);

				KillTimer(hWnd, ID_CLTIMER);

				hList = GetDlgItem(hWnd, IDC_COOKIE_SETTING);
				index = SendMessage(hList, LB_GETCURSEL, NULL, NULL);

				if (index == 0)
				{
					curconfig.nCookieMode = WBCOOKIE_ENABLE;
					tmp = WBCOOKIE_ENABLE;
				}
				else
				{
					curconfig.nCookieMode = WBCOOKIE_DISABLE;
					tmp = WBCOOKIE_DISABLE;
				}

				if (pUIBrowser != NULL)                
				{
					int tmpm;

					tmpm = curconfig.nCookieMode;
					result = PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_MODE, &(tmpm), sizeof(int));
					PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_SIZE, &(curconfig.nCookieSize), sizeof(int));
					PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_COUNT, &(curconfig.nCookieCount), sizeof(int));
				}
				curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
				curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
				//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

				if(result)					//if return success, change the value
					curconfig.nCookieMode = tmp;
				
                WAP_SetCurConfig(&curconfig);
				Saveconfig(&curconfig);         
				nCookieSel = index;
				
				if (curconfig.nCookieMode == WBCOOKIE_ENABLE)
				{
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);				
				}
				else
				{
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioON);				
				}
				SendMessage(pSub->hWindow, SSBM_SETCURSEL, nCookieSel, 0);
				SetTimer(hWnd, ID_CLTIMER, 500, NULL);
			}
			break;

		case VK_F10:
			nCookieSel = SendMessage(pSub->hWindow, SSBM_GETCURSEL, NULL, NULL);
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

HWND hWapDisplaySettingClo;
/*********************************************************************\
* Function     On_WAP_DISPLAY_Setting
* Purpose      create public config window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
BOOL On_WAP_DISPLAY_Setting(HWND hParent, PWMLSETTING pCreate)
{
	WNDCLASS wc;
	WMLSETSUB Sub;
	HWND hWapDisplaySetting;
	RECT rClient;

	GetClientRect(hParent, &rClient); 
    
	if(!GetClassInfo(NULL, "WAP_Display_Setting", &wc))
	{
		wc.style         = 0;
		wc.lpfnWndProc   = WAP_Public_DISPLAY_Setting_Proc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = sizeof(WMLSETSUB);
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "WAP_Display_Setting";
		
		RegisterClass(&wc); 
	}
    
	Sub.hWindow = pCreate->hDisplay;

    hWapDisplaySetting = CreateWindow(
        "WAP_Display_Setting", 
        "", 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        (PVOID)&Sub
        );
    if (hWapDisplaySetting == NULL) 
    {
        return FALSE;
    }

	if(hParent == GetWAPFrameWindow())
		hWapDisplaySettingClo = hWapDisplaySetting;
    
    ShowWindow(hWapDisplaySetting, SW_SHOW);            
    UpdateWindow(hWapDisplaySetting);
    
    return TRUE;
}

static LRESULT WAP_Public_DISPLAY_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    WORD mID;
    WORD msgBox; 
	HWND hFrame;
	PWMLSETSUB pSub;

	hFrame = GetParent(hWnd);
	pSub = GetUserData(hWnd);
	
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			HWND hwndList;
			int index;
			PCREATESTRUCT pCreate;

			pCreate = (PCREATESTRUCT)lParam;
			memcpy(pSub, pCreate->lpCreateParams, sizeof(PWMLSETSUB));

			Loadconfig(&curconfig);
			{	
				SetWindowText(hFrame, (LPCTSTR)WML_DISPLAYPIC);
				PDASetMenu(hFrame, NULL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			}

			pSub->hbmpRadioOFF = LoadImage(NULL, RES_RADIO_OFF, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);
			pSub->hbmpRadioON = LoadImage(NULL, RES_RADIO_ON, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);

			hwndList = CreateWindow(
				"LISTBOX",
				"Item1",
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				0, 0, 176, 150,
				hWnd,
				(HMENU)IDC_DISPLAY_SETTING,
				NULL,
				NULL);
			
			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)IDS_YES);
			SendMessage(hwndList, LB_ADDSTRING, 1, (LPARAM)IDS_NO);
		
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);
			
			index = SendMessage(pSub->hWindow, SSBM_GETCURSEL, NULL, NULL);
			
			if(index != LB_ERR)
			{
				SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)pSub->hbmpRadioON);
				SendMessage(hwndList, LB_SETCURSEL, index, NULL);
			}
			else
			{
				SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
				SendMessage(hwndList, LB_SETCURSEL, 0, NULL);
			}

			SetFocus(hwndList);
		}
        break;

	case PWM_SHOWWINDOW:
		{	
			HWND hList;

			SetWindowText(hFrame, (LPCTSTR)WML_DISPLAYPIC);
			PDASetMenu(hFrame, NULL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);

			hList = GetDlgItem(hWnd, IDC_DISPLAY_SETTING);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);

			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nDisplaySel), (LPARAM)pSub->hbmpRadioON);
			SendMessage(hList, LB_SETCURSEL, nDisplaySel, NULL);
			SetFocus(hList);

			SetFocus(hList);
		}
		break;  
        
    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
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
		SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        DestroyWindow(hWnd);
        break;

	case WM_DESTROY:
		UnregisterClass("WAP_Display_Setting", NULL);
		break;

	case WM_TIMER:
		KillTimer(hWnd, NULL);
		PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_F5:
			{
				HWND hList;
				int index;
				int result;

				Loadconfig(&curconfig);

				KillTimer(hWnd, ID_CLTIMER);

				hList = GetDlgItem(hWnd, IDC_DISPLAY_SETTING);
				index = SendMessage(hList, LB_GETCURSEL, NULL, NULL);

				if (index == 0)
					curconfig.bShowImg = TRUE;
				else
					curconfig.bShowImg = FALSE;

				if (pUIBrowser != NULL)                
				{
					result = PWBE_SetDisplayOption(pUIBrowser, WBDOP_IMGDISPLAY, &(curconfig.bShowImg), 0);
				}
				curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
				curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
				//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

                WAP_SetCurConfig(&curconfig);
				Saveconfig(&curconfig);         
				nDisplaySel = index;
				
				if (curconfig.bShowImg)
				{
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);				
				}
				else
				{
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioON);				
				}
				SendMessage(pSub->hWindow, SSBM_SETCURSEL, nDisplaySel, 0);
				SetTimer(hWnd, ID_CLTIMER, 500, NULL);
			}
			break;

		case VK_F10:
			nDisplaySel = SendMessage(pSub->hWindow, SSBM_GETCURSEL, NULL, NULL);
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

HWND hWapSoundSettingClo;
/*********************************************************************\
* Function     On_WAP_SOUND_Setting
* Purpose      create public config window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
BOOL On_WAP_SOUND_Setting(HWND hParent, PWMLSETTING pCreate)
{
	WNDCLASS wc;
	WMLSETSUB Sub;
	HWND hWapSoundSetting;
	RECT rClient;

	GetClientRect(hParent, &rClient); 
    
	if(!GetClassInfo(NULL, "WAP_Sound_Setting", &wc))
	{
		wc.style         = 0;
		wc.lpfnWndProc   = WAP_Public_SOUND_Setting_Proc;
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = sizeof(WMLSETSUB);
		wc.hInstance     = NULL;
		wc.hIcon         = NULL;
		wc.hCursor       = NULL;
		wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName  = NULL;
		wc.lpszClassName = "WAP_Sound_Setting";
		
		RegisterClass(&wc); 
	}
    
	Sub.hWindow = pCreate->hSound;

    hWapSoundSetting = CreateWindow(
        "WAP_Sound_Setting", 
        "", 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        (PVOID)&Sub
        );
    if (hWapSoundSetting == NULL) 
    {
        return FALSE;
    }
    
	if(hParent == GetWAPFrameWindow())
		hWapSoundSettingClo = hWapSoundSetting;

    ShowWindow(hWapSoundSetting, SW_SHOW);            
    UpdateWindow(hWapSoundSetting);
    
    return TRUE;
}

static LRESULT WAP_Public_SOUND_Setting_Proc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = TRUE;	 
    HDC hdc;
    WORD mID;
    WORD msgBox;  
	HWND hFrame;
	
	PWMLSETSUB pSub;
	hFrame = GetParent(hWnd);
	pSub = GetUserData(hWnd);
	
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
		{
			HWND hwndList;
			int index;
			PCREATESTRUCT pCreate;

			pCreate = (PCREATESTRUCT)lParam;
			memcpy(pSub, pCreate->lpCreateParams, sizeof(PWMLSETSUB));
			Loadconfig(&curconfig);
			{	
				SetWindowText(hFrame, (LPCTSTR)WML_PLAYSOUND);
				PDASetMenu(hFrame, NULL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
				SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);
			}

			pSub->hbmpRadioOFF = LoadImage(NULL, RES_RADIO_OFF, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);
			pSub->hbmpRadioON = LoadImage(NULL, RES_RADIO_ON, IMAGE_BITMAP, 20, 20, LR_LOADFROMFILE);

			hwndList = CreateWindow(
				"LISTBOX",
				"Item1",
				WS_VISIBLE|WS_CHILD|WS_VSCROLL|LBS_NOTIFY|LBS_BITMAP,
				0, 0, 176, 150,
				hWnd,
				(HMENU)IDC_SOUND_SETTING,
				NULL,
				NULL);
			
			SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)IDS_YES);
			SendMessage(hwndList, LB_ADDSTRING, 1, (LPARAM)IDS_NO);
		
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);
			
			index = SendMessage(pSub->hWindow, SSBM_GETCURSEL, NULL, NULL);
			
			if(index != LB_ERR)
			{
				SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, index), (LPARAM)pSub->hbmpRadioON);
				SendMessage(hwndList, LB_SETCURSEL, index, NULL);
			}
			else
			{
				SendMessage(hwndList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
				SendMessage(hwndList, LB_SETCURSEL, 0, NULL);
			}

			SetFocus(hwndList);
		}
        break;

	case PWM_SHOWWINDOW:
		{	
			HWND hList;

			SetWindowText(hFrame, (LPCTSTR)WML_PLAYSOUND);
			PDASetMenu(hFrame, NULL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_CANCEL);
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 1, (LPARAM)""); 
			SendMessage(hFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)MENU_SELECT);

			hList = GetDlgItem(hWnd, IDC_SOUND_SETTING);

			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);
			
			SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, nSoundSel), (LPARAM)pSub->hbmpRadioON);
			SendMessage(hList, LB_SETCURSEL, nSoundSel, NULL);
			SetFocus(hList);

			SetFocus(hList);
		}
		break;  
        
    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
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
		SendMessage(hFrame, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
        DestroyWindow(hWnd);
        break;

	case WM_DESTROY:
		UnregisterClass("WAP_Sound_Setting", NULL);
		break;

	case WM_TIMER:
		{
			KillTimer(hWnd, NULL);
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
		}
		
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_F5:
			{
				HWND hList;
				int index;
				int result;

				Loadconfig(&curconfig);

				KillTimer(hWnd, ID_CLTIMER);

				hList = SetFocus(GetWindow(hWnd, GW_CHILD));
				index = SendMessage(hList, LB_GETCURSEL, NULL, NULL);

				if (index == 0)
					curconfig.bPlayRing = TRUE;
				else
					curconfig.bPlayRing = FALSE;

				if (pUIBrowser != NULL)                
				{
					result = PWBE_SetDisplayOption(pUIBrowser, WBDOP_BGSOUND, &(curconfig.bPlayRing), 0);
				}
				curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
				curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
				//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

                WAP_SetCurConfig(&curconfig);
				Saveconfig(&curconfig);       
				nSoundSel = index;
				
				if (curconfig.bPlayRing)
				{
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioOFF);				
				}
				else
				{
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioOFF);
					SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 1), (LPARAM)pSub->hbmpRadioON);				
				}
				SendMessage(pSub->hWindow, SSBM_SETCURSEL, nSoundSel, 0);
				SetTimer(hWnd, ID_CLTIMER, 500, NULL);
			}
			break;

		case VK_F10:
			nSoundSel = SendMessage(pSub->hWindow, SSBM_GETCURSEL, NULL, NULL);
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

static void LoadConnection(UDB_ISPINFO uIspInfo, PWMLSETSUB pSub, HWND hList)
{
	int ConNum = 0, i = 0, sel = 0;
	char chooseup[32];
	char conname[31 +1];
				
	memset(chooseup, 0, 32);
	memset(conname, 0, 32);
	ConNum = IspGetNum();
	for (i = 0; i < ConNum; i++)
	{
		if (IspReadInfo(&uIspInfo, i))
		{
			int j = 1;
			
			if(i != 0)
			{
				for(j = 1; j < (i + 1); j++)
				{
					SendMessage(hList, LB_GETTEXT, j, (LPARAM)conname);
					if(strncasecmp(uIspInfo.ISPName, conname, strlen(uIspInfo.ISPName)) < 0)
						break;
				}
			}
			if(uIspInfo.ISPID == curconfig.ISPID)
			{
				SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)uIspInfo.ISPName);
				SendMessage(hList, LB_SETITEMDATA, j, i);
				SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, j), (LPARAM)pSub->hbmpRadioON);
				strcpy(chooseup, uIspInfo.ISPName);
			}
			else
			{
				SendMessage(hList, LB_INSERTSTRING, j, (LPARAM)uIspInfo.ISPName);
				SendMessage(hList, LB_SETITEMDATA, j, i);
				SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, j), (LPARAM)pSub->hbmpRadioOFF);
			}
		}
	}	
	
	if(strlen(chooseup) == 0)
		sel = 0;
	else
	{
		for(i = 1; i <= ConNum; i++)
		{
			SendMessage(hList, LB_GETTEXT, i, (LPARAM)conname);
			if(strncasecmp(chooseup, conname, strlen(chooseup)) == 0)
				sel = i;
		}
	}
	
	if(sel == 0)
	{
		SendMessage(hList, LB_SETCURSEL, 0, 0);
		SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP, 0), (LPARAM)pSub->hbmpRadioON);
		oldsel = 0;
	}
	else
	{
		SendMessage(hList, LB_SETCURSEL, sel, NULL);
		oldsel = sel;
	}
}


static int Saveconfig(WMLCFGFILE *sCfgFile)
{
    int nRetFileSize = 0;    
    FILE* fIe;
    char szCurPath[_MAX_PATH];
    
    getcwd(szCurPath, _MAX_PATH);
    chdir(FLASHPATH);    
    
    if ((fIe = fopen(WIECONFIG_FILENAME, "rb+")) == NULL)
    {
        if ((fIe = fopen(WIECONFIG_FILENAME, "wb+")) == NULL)
        {
            chdir(szCurPath);
            return -1;
        }        
    }
    
    nRetFileSize = fwrite(sCfgFile, 1, sizeof(WMLCFGFILE), fIe);
    if (nRetFileSize == 0)
    {
        fclose(fIe);
        chdir(szCurPath);
        return -1;
    }
    
    fclose(fIe);
    chdir(szCurPath);

    return 1;
}

int Loadconfig(WMLCFGFILE *pCurrentCfg)
{
    int nRetFileSize = 0;    
    FILE* fIe;
    int flag = 1;
    char szCurPath[_MAX_PATH];
    struct stat st;
	int tmd;
	int i, ConNum;
	int handleopen = 0;
	UDB_ISPINFO uIspInfo;
	DWORD dGateway[4] = {0};

    getcwd(szCurPath, _MAX_PATH);
    chdir(FLASHPATH);

	handleopen = open(WIECONFIG_FILENAME, O_RDONLY);
	printf("\r\n\r\n\r\n__________________handleopen = %d__________________________\r\n\r\n\r\n", handleopen);
	close(handleopen);
    
    fIe = fopen(WIECONFIG_FILENAME, "rb+");

    if (fIe == NULL)
    {
		fIe = fopen(WIECONFIG_FILENAME, "wb+");
		if (fIe == NULL)
			return -1;
        flag = -1;
    }
    else
    {    
		stat(WIECONFIG_FILENAME, &st);        
		if (-1 == (nRetFileSize = st.st_size))
        {
            fclose(fIe);
            flag = -1;
        }
        else
        {
            nRetFileSize = fread(pCurrentCfg, 1, sizeof(WMLCFGFILE), fIe);
			ConNum = IspGetNum();
			for (i = 0; i < ConNum; i++)
			{
				if (IspReadInfo(&uIspInfo, i))
				{
					if(pCurrentCfg->ISPID == 0)
					{
						strcpy(pCurrentCfg->sHomepage, "");
						strcpy(pCurrentCfg->sGateWay, "195.156.25.4");
						strcpy(pCurrentCfg->nPort, "9201");
						strcpy(pCurrentCfg->szConnectionname, "");
						break;
					}
					else if(uIspInfo.ISPID == pCurrentCfg->ISPID)
					{
						strcpy(pCurrentCfg->sHomepage, uIspInfo.ConnectHomepage);
						dGateway[0] = FIRST_IPADDRESS(uIspInfo.ConnectGateway);
						dGateway[1] = SECOND_IPADDRESS(uIspInfo.ConnectGateway);
						dGateway[2] = THIRD_IPADDRESS(uIspInfo.ConnectGateway);
						dGateway[3] = FOURTH_IPADDRESS(uIspInfo.ConnectGateway);
						sprintf(pCurrentCfg->sGateWay, "%03d.%03d.%03d.%03d", 
							dGateway[0], dGateway[1], dGateway[2], dGateway[3]);
						strcpy(pCurrentCfg->nPort, uIspInfo.ConnPort);
						strcpy(pCurrentCfg->szConnectionname, uIspInfo.ISPName);
						break;
					}
				}
			}
			tmd = sizeof(WMLCFGFILE);
			if(st.st_size != 0)
            {
				if (nRetFileSize == 0 || nRetFileSize != sizeof(WMLCFGFILE))
				{
					fclose(fIe);
					flag = -1;
				}
			}
        }
        fclose(fIe);        
    }
    chdir(szCurPath);

    if (flag == -1)
    {
		strcpy(pCurrentCfg->sHomepage,"");
        pCurrentCfg->nhomepagetype= nHomepagedefalutType;

		pCurrentCfg->uColorCfg.link_bgcolor = RGB(255,255,255);
		pCurrentCfg->uColorCfg.link_Focusbgcolor = RGB(0,0,255);
		pCurrentCfg->uColorCfg.link_Focustextcolor = RGB(255,255,255);
		pCurrentCfg->uColorCfg.link_textcolor = 0;
		pCurrentCfg->uColorCfg.text_color = 0;
        
        //色彩重置
        SetDefColorRef(&(pCurrentCfg->uColorCfg));    
		
        pCurrentCfg->CacheMode = WBCACHE_OK;
        pCurrentCfg->nCacheSize = nCacheSize[0];        
        pCurrentCfg->nCookieCount = nCookieCount[0];
        pCurrentCfg->nCookieSize =  nCookieSize[0];
        pCurrentCfg->nCookieMode =  WBCOOKIE_ENABLE;        
        pCurrentCfg->bShowImg  =  TRUE;
        pCurrentCfg->bPlayRing =  TRUE;
		pCurrentCfg->ISPID = 0;
		strcpy(pCurrentCfg->szConnectionname, "");
		strcpy(pCurrentCfg->nPort, "9201");
		strcpy(pCurrentCfg->sGateWay, "195.156.25.4");
        
        return 0;
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
Color_Scheme WSET_GetColorCfg()
{				
	WAP_GetCurConfig(&curconfig);

    return curconfig.uColorCfg;
}

void WIE_Set_Systemcfg()
{
    int result; 
	int tmpmode;

	Loadconfig(&curconfig);
    
	if(strlen(curconfig.sHomepage) == 0)
    strcpy(szHomePage, curconfig.sHomepage);
    nHomepageType = curconfig.nhomepagetype;
	tmpmode = curconfig.nCookieMode;
    
    result = PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_MODE, &tmpmode, sizeof(int) );
    result = PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_SIZE, &curconfig.nCookieSize, sizeof(int) );
    result = PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_COUNT, &curconfig.nCookieCount, sizeof(int) );
    result = PWBE_SetCACOOption(pUIBrowser, WBCACHE_SIZE, &curconfig.nCacheSize, sizeof(int) );
    result = PWBE_SetCACOOption(pUIBrowser, WBCACHE_MODE, &curconfig.CacheMode, sizeof(int) );
}


/*********************************************************************\
* Function       On_IDM_Setting
* Purpose      create config window
* Params       
* Return        -1    fail
1    success            
* Remarks       
**********************************************************************/
HWND On_IDM_Setting(HWND hParent, RECT rClient)
{    
    WNDCLASS wc;
	HWND hwndIESETApp;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WMLIESettingWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLIESettingClass";
    if (!RegisterClass(&wc))
        return NULL;
    
    hwndIESETApp = CreateWindow(
        "WMLIESettingClass", 
        WML_MENU_5, 
        WS_VISIBLE|WS_CHILD,
        rClient.left, rClient.top, 
		rClient.right-rClient.left, 
		rClient.bottom-rClient.top, 
        hParent, 
        NULL,
        NULL, 
        NULL
        );
    if (hwndIESETApp == NULL) 
    {
        UnregisterClass("WMLIESettingClass", NULL);
        return NULL;
    }
    
    ShowWindow(hwndIESETApp, SW_SHOW);            
    UpdateWindow(hwndIESETApp);
    
    return hwndIESETApp;
}
/*********************************************************************\
* Function       WMLIESettingWndProc
* Purpose      设置窗口窗口过程
* Params       
* Return            
* Remarks       
**********************************************************************/
static LRESULT WMLIESettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    LRESULT lResult = TRUE;
    HDC hdc;
    WORD mID;
    WORD msgBox;    
    static int nType = WIE_IDM_BTNPAGE;
    int i;
    const char* SZNAME[WIE_SETTING_NUM];
    
    switch (wMsgCmd)
    {
    case WM_CREATE: 
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)STR_WAPWML_CAPTION);
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK); 
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		
        hWndSettingLst = CreateWindow(
            "LISTBOX", 
            WML_MENU_5, 
            WS_VISIBLE|WS_CHILD,
            WIE_LISTBOX_POS, 
            hWnd,
            (HMENU)(WIE_IDM_SETTINGLIST),
            NULL, 
            NULL
            ); 

        SZNAME[0] = WML_SET_HPBTN;
        SZNAME[1] = WML_SET_COLORBTN;
        SZNAME[2] = WML_SET_ADVANCED;
        //SZNAME[3] = GetString(WML_SET_GPRS);
		//SZNAME[4] = GetString(WML_PREVIEWCERT);

        for (i = 0; i < WIE_SETTING_NUM; i++)
            SendMessage(hWndSettingLst, LB_ADDSTRING, i, (LPARAM)SZNAME[i]);
        SendMessage(hWndSettingLst, LB_SETCURSEL, 0, NULL);        
		SetFocus(hWndSettingLst);
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(GetWAPFrameWindow(), (LPCTSTR)STR_WAPWML_CAPTION);
		PDASetMenu(GetWAPFrameWindow(), NULL);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 0, (LPARAM)WML_MENU_BACK);
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK); 
		SendMessage(GetWAPFrameWindow(), PWM_SETBUTTONTEXT, 2, (LPARAM)"");	
		break;
        
    case WM_ACTIVATE:
        if (WA_ACTIVE == LOWORD(wParam))
            SetFocus(hWndSettingLst);
        break;
        
    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
        break;
        
    case WM_COMMAND:        
        mID    = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {            
        case WIE_IDM_SETTINGLIST:    
            if (HIWORD(wParam) == LBN_DBLCLK)
				SendMessage(hWnd, WM_COMMAND, WIE_IDM_SET_OK, NULL);
            break;
            
        case WIE_IDM_SET_CLOSE:
            Wml_ONCancelExit(hWnd);
            break;
            
        case WIE_IDM_SET_OK: 
            nType = SendMessage(hWndSettingLst, LB_GETCURSEL, NULL, NULL);
            if (nType != LB_ERR)
            {                
                switch (nType)
                {
                case WIE_IDM_BTNADVANCECFG:
                    On_IDM_AdVSetting();
                    break;
                    
                case WIE_IDM_BTNPAGE:
                    CreateHomepageWin();
                    break;
                    
                case WIE_IDM_BTNCOLOR:
                    CreateColorWin();
                    break;
                    
                case WIE_IDM_BTNGPRS:
                    WIE_GPRS();
                    break;

				//case WIE_IDM_BTNCERTIFICATECFG:
                //  WIE_GPRS();
                //  break;
                    
                default:
                    break;                    
                }
            }
            break;
        
        default:
            break;                
        }
        break;
        
        case WM_CLOSE:
			SendMessage(GetWAPFrameWindow(), PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);

            nType = WIE_IDM_BTNPAGE;
            DestroyWindow(hWnd);
            UnregisterClass("WMLIESettingClass", NULL);
            break;
            
		case WM_KEYDOWN:
			switch (wParam)
			{	
			case VK_RETURN:
				SendMessage(hWnd, WM_COMMAND, WIE_IDM_SET_OK, NULL);
				break;	

			case VK_F10:
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

BOOL On_IDM_AdVSetting()
{    
    WNDCLASS wc;
	HWND hwndIEAdvSETApp;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WMLIEAdvSettingWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLIEAdvSettingClass";
    if (!RegisterClass(&wc))
        return FALSE;
    
    hwndIEAdvSETApp = CreateWindow(
        "WMLIEAdvSettingClass", 
        WML_SET_ADVANCED, 
        WS_VISIBLE|WS_CAPTION|WS_BORDER|PWS_STATICBAR,
        PLX_WIN_POSITION, 
        NULL, 
        NULL,
        NULL, 
        NULL
        );
    if (hwndIEAdvSETApp == NULL) 
    {
        UnregisterClass("WMLIEAdvSettingClass", NULL);
        return FALSE;
    }
    
    ShowWindow(hwndIEAdvSETApp, SW_SHOW);            
    UpdateWindow(hwndIEAdvSETApp);
    
    return TRUE;
}

static LRESULT WMLIEAdvSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    WORD mID;
    WORD msgBox;    
    static int nType;    
    const char* SZNAME[WIE_ADV_NUM];
    int i;
    
    switch (wMsgCmd)
    {
    case WM_CREATE:        
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, WIE_IDM_SET_CLOSE, (LPARAM)WML_MENU_BACK);
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(WIE_IDM_SET_OK,1), (LPARAM)STR_WAPWINDOW_OK);
        hWndAdvLst = CreateWindow(
            "LISTBOX", 
            WML_MENU_5, 
            WS_VISIBLE | WS_CHILD ,
            WIE_LISTBOX_POS, 
            hWnd,
            (HMENU)(WIE_IDM_ADVLIST),
            NULL, 
            NULL
            ); 

        SZNAME[0] = WML_SET_CACHE;
        SZNAME[1] = WML_SET_COOKIE;
        SZNAME[2] = WML_SET_SHOW;

        for (i = 0; i < WIE_ADV_NUM; i++)
            SendMessage(hWndAdvLst, LB_ADDSTRING, i, (LPARAM)SZNAME[i]);
        SendMessage(hWndAdvLst, LB_SETCURSEL, 0, NULL);
        break;
        
    case WM_ACTIVATE:
        if (WA_ACTIVE == LOWORD(wParam))
            SetFocus(hWndAdvLst);
        break;

    case WM_PAINT:        
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
        break;
        
    case WM_COMMAND:        
        mID    = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {
        case WIE_IDM_ADVLIST:    
            if (HIWORD(wParam) == LBN_DBLCLK)
				SendMessage(hWnd, WM_COMMAND, WIE_IDM_SET_OK, NULL);
            break;
            
        case WIE_IDM_SET_CLOSE:
            Wml_ONCancelExit(hWnd);
            break;
 
        case WIE_IDM_SET_OK:
            nType = SendMessage(hWndAdvLst, LB_GETCURSEL, NULL, NULL);
            if (nType != LB_ERR)
            {                
                switch (nType)
                {
                case WIE_IDM_BTNCACHECFG:
                    WIE_CacheConfig();
                    break;
                    
                case WIE_IDM_BTNCOOKIECFG:
                    WIE_CookieConfig();
                    break;
                    
                case WIE_IDM_BTNSHOWCFG:
                    WIE_ShowConfig();
                    break;
					
				default:
					break;
                }
            }
            break;
        }
        break;
         
    case WM_CLOSE:
        DestroyWindow(hWnd);
        UnregisterClass("WMLIEAdvSettingClass", NULL);
        break;            

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_RETURN:
			SendMessage(hWnd, WM_COMMAND, WIE_IDM_SET_OK, NULL);
			break;

		case VK_F10:
			SendMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			
		}
		break;
            
        default :            
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            break;
	}
      
    return lResult;       
}

/*********************************************************************\
* Function       CreateHomepageWin
* Purpose      创建主页编辑窗口
* Params       
* Return            
* Remarks       
**********************************************************************/
static int CreateHomepageWin( )
{        
    WNDCLASS wc;
	HWND hwndIEHomepage;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WMLIEHomepageWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLIEHomepageClass";
    
    if (!RegisterClass(&wc))
        return FALSE;      
    
    hwndIEHomepage = CreateWindow(
        "WMLIEHomepageClass",
        WML_SET_HPBTN,
        WS_VISIBLE|WS_CAPTION|WS_BORDER|PWS_STATICBAR,
        PLX_WIN_POSITION, 
        NULL, 
        NULL,
        NULL, 
        NULL
        );
    if (hwndIEHomepage == NULL) 
    {
        UnregisterClass("WMLIEHomepageClass", NULL);
        return -1;
    }
    
    ShowWindow(hwndIEHomepage, SW_SHOW);            
    UpdateWindow(hwndIEHomepage);

    return 1;
}

/*********************************************************************\
* Function       WMLIEHomepageWndProc
* Purpose      主页设置窗口过程
* Params       
* Return            
* Remarks       
**********************************************************************/
static LRESULT WMLIEHomepageWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{   
    LRESULT lResult = TRUE;
    HDC hdc;
    WORD mID;
    WORD msgBox;    
    static int tmpHomepageType = URL_NULL;
    int urllen;    
    static HWND hFocus;
    static HWND BTNOPage,BTNCurPage;
    static HWND hEDMAINURL;
    int ncurtype;
	IMEEDIT	InputMainURL;   
    
    switch (wMsgCmd)
    {
    case WM_CREATE :
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, WIE_IDM_SET_CLOSE, (LPARAM)WML_MENU_BACK);
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(WIE_IDM_SET_OK,1), (LPARAM)STR_WAPWINDOW_SAVE);
        memset(&InputMainURL, 0, sizeof(IMEEDIT));		
		InputMainURL.hwndNotify   = (HWND)hWnd;
		InputMainURL.dwAscTextMax = 0;
		InputMainURL.dwAttrib     = 0;//IME_ATTRIB_GENERAL;;
		InputMainURL.dwUniTextMax = 0;
		InputMainURL.pszCharSet   = NULL;
		InputMainURL.pszImeName   = NULL;//"字母";
		InputMainURL.pszTitle     = NULL;
		InputMainURL.uMsgSetText  = 0;
		InputMainURL.wPageMax     = 0;

        hEDMAINURL =  CreateWindow(
            "IMEEDIT", 
            "", 
            WS_BORDER |WS_CHILD |WS_VISIBLE|WS_TABSTOP|ES_UNDERLINE|ES_MULTILINE|WS_VSCROLL,
            2,     
            20,
            WIE_LEDIT_WIDTH, WIE_MEDIT_HEIGHT,    
            hWnd, 
            (HMENU)(WIE_IDM_EDITPAGE),
            NULL, 
            (PVOID)&InputMainURL
            ); 
        
        if (hEDMAINURL == NULL) 
            return FALSE;
        
        SendMessage(hEDMAINURL, EM_LIMITTEXT, URLNAMELEN -1, NULL);
        SetWindowText(hEDMAINURL, szHomePage);        
        
        BTNCurPage =  CreateWindow(
            "BUTTON", 
            WML_CURRENT,
            WS_VISIBLE |WS_CHILD|WS_TABSTOP|WS_BITMAP,
            2, WGO_BTMBTNY,WIE_MBTN_WIDTH, WIE_SBTN_HEIGHT,    
            hWnd, 
            (HMENU)WIE_IDM_CURPAGE,
            NULL, 
            NULL
            ); 
        if (BTNCurPage == NULL) 
            return FALSE;
        
        BTNOPage = CreateWindow(
            "BUTTON", 
            WML_DEFAULT,
            WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BITMAP,
            (CLT_WIDTH - 6 - WIE_MBTN_WIDTH*2)/2+ 4 + WIE_MBTN_WIDTH,  
            WGO_BTMBTNY,WIE_MBTN_WIDTH, WIE_SBTN_HEIGHT,    
            hWnd, 
            (HMENU)WIE_IDM_OPAGE,
            NULL, 
            NULL
            ); 
        if (BTNOPage == NULL) 
            return FALSE;        
        {
			//WMLCFGFILE curconfig;

        	WAP_GetCurConfig(&curconfig);
			SetWindowText(hEDMAINURL, curconfig.sHomepage);
			tmpHomepageType = curconfig.nhomepagetype;   
		}
		hFocus = hEDMAINURL;        
        break;
 
    case WM_SETRBTNTEXT:
        if (strcmp((char *)lParam, STR_WAPWINDOW_EXIT) == 0)
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)STR_WAPWINDOW_BACK);
        else
			SendMessage(hWnd, PWM_SETBUTTONTEXT, 0, lParam);
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
            hFocus = GetFocus();
        else
            SetFocus(hFocus);
        break;

    case WM_PAINT:   
		{
			int bkmodeold;
        hdc = BeginPaint(hWnd, NULL);
		bkmodeold = SetBkMode(hdc, TRANSPARENT);
        TextOut(hdc, 2, 4, (PCSTR)WML_HOMEPAGE, -1);
		SetBkMode(hdc, bkmodeold);
        EndPaint(hWnd, NULL);
		}
        break;
        
    case WM_COMMAND:        
        mID = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {            
        case WIE_IDM_OPAGE:
            if (msgBox == BN_CLICKED)
            {
                SetWindowText(hEDMAINURL, SZDEFAULTHOMEPAGE);            
                tmpHomepageType = nHomepagedefalutType;
            }
            break;
            
        case WIE_IDM_CURPAGE:
            if (msgBox == BN_CLICKED)
            {    
                char szCurPage[URLNAMELEN] = "";
                
                GetCurentPage(szCurPage, &ncurtype);
                SetWindowText(hEDMAINURL, szCurPage);            
                tmpHomepageType = curUrlType ; 
            }            
            break;

        case WIE_IDM_EDITPAGE:
            if (msgBox == EN_UPDATE )
            {                 
                if (GetWindowTextLength(hEDMAINURL) == 0)
                    tmpHomepageType = URL_NULL;
                else
                    tmpHomepageType = URL_REMOTE;                    
            }
            break;
			
        case WIE_IDM_SET_CLOSE:
            Wml_ONCancelExit(hWnd);
            break;
            
        case WIE_IDM_SET_OK:
            {
                char szTmphomepage[URLNAMELEN] = "";
                
                urllen = GetWindowTextLength(hEDMAINURL);         
                if (urllen != 0) 
                    GetWindowText(hEDMAINURL, szTmphomepage, urllen+1);
                else 
                    strcpy(szTmphomepage, "");
                
				{
					//WMLCFGFILE curconfig;

        			WAP_GetCurConfig(&curconfig);

					if (szTmphomepage != NULL) 
						strcpy(curconfig.sHomepage, szTmphomepage);
					curconfig.nhomepagetype = tmpHomepageType;
					
					curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
					curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
					//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

					WAP_SetCurConfig(&curconfig);
					Saveconfig(&curconfig);
					strcpy(szHomePage, curconfig.sHomepage);
					nHomepageType = curconfig.nhomepagetype;
				}
                                
                PostMessage(hWnd, WM_CLOSE, NULL, NULL);                
                break;
            }
        }
        break;
        
    case WM_CLOSE:
        DestroyWindow(hWnd);
        UnregisterClass("WMLIEHomepageClass", NULL);                
        break;

	case WM_KEYDOWN:
		switch (wParam)
		{	
		case VK_F10:
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
/*********************************************************************\
* Function       CreateColorWin
* Purpose      创建色彩配置窗口
* Params       
* Return            
* Remarks       
**********************************************************************/
static BOOL CreateColorWin()
{
    WNDCLASS wc;
	HWND hwndIEColor;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WMLIEColorWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName   = NULL;
    wc.lpszClassName = "WMLIEColorClass";
    
    if (!RegisterClass(&wc))
        return FALSE;      
    
    hwndIEColor = CreateWindow(
        "WMLIEColorClass", 
        WML_SET_COLORBTN,
        WS_VISIBLE|WS_CAPTION|WS_BORDER|PWS_STATICBAR,
        PLX_WIN_POSITION, 
        NULL, 
        NULL,
        NULL, 
        NULL
        );
    if (hwndIEColor == NULL) 
    {
        UnregisterClass("WMLIEColorClass", NULL);
        return -1;
    }
    
    ShowWindow(hwndIEColor, SW_SHOW);            
    UpdateWindow(hwndIEColor);

    return 1;
}

static void WSet_Color_ChgColorKillFocus(HWND hIEColor)
{
    SendMessage(hIEColor, WM_COMMAND, MAKEWPARAM(WIE_IDM_CHANGECOLOR,COLORBN_KILLFOCUS), NULL);    
}

static void WSet_Color_ChgColorSetFocus(HWND hIEColor)
{
    SendMessage(hIEColor, WM_COMMAND, MAKEWPARAM(WIE_IDM_CHANGECOLOR,BN_SETFOCUS), NULL);
}

static void WSet_Color_ChgColorClick(HWND hIEColor)
{
    SendMessage(hIEColor, WM_COMMAND, MAKEWPARAM(WIE_IDM_CHANGECOLOR,BN_CLICKED), NULL);
}


extern BOOL HPSelectColor(PCSTR szCaption, COLORREF cr, BYTE *pr, BYTE *pg, BYTE *pb, 
                          PCSTR szOk, PCSTR szCancel);
/*********************************************************************\
* Function       WMLIEColorWndProc
* Purpose      配色窗口过程
* Params       
* Return            
* Remarks       
**********************************************************************/
static LRESULT WMLIEColorWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    LRESULT lResult = TRUE;
    HDC hdc;
    WORD mID;
    WORD msgBox;        
    static HWND hFocus;
    static HWND BTNCItem, BTNReset;    
    static COLORREF curClRef[5];
    static int iCurItem;
    COLORREF oTextCl, oBackCl;
    RECT effRect[2] = {{WCOLOR_E1X1,WCOLOR_E1Y1,WCOLOR_E1X2,WCOLOR_E1Y2},
    {WCOLOR_E2X1,WCOLOR_E2Y1,WCOLOR_E2X2,WCOLOR_E2Y2}};
    static BOOL ibChgColorFocus;
    static int noffset = 0;
    
    switch (wMsgCmd)
    {
    case WM_CREATE:        
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, WIE_IDM_SET_CLOSE, (LPARAM)WML_MENU_BACK);
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(WIE_IDM_SET_OK,1), (LPARAM)STR_WAPWINDOW_SAVE);
        SendMessage(hWnd, PWM_SETSCROLLSTATE, MASKALL, SCROLLDOWN |SCROLLUP);
        
        BTNCItem = CreateWindow(
            "STRSPINBOX", 
            "", 
            WS_VISIBLE  |  WS_CHILD |WS_TABSTOP| SSBS_CENTER|CS_NOSYSCTRL|WS_BORDER,
            WCOLOR_SPINX, WCOLOR_SPINY,WIE_LSPIN_WIDTH - 10, WIE_MBTN_HEIGHT,
            hWnd, 
            (HMENU)WIE_IDM_ITEM,
            NULL, 
            NULL
            ); 
        if (BTNCItem == NULL) 
            return FALSE;                
        
        BTNReset = CreateWindow(
            "BUTTON", 
            WML_SET_RESET, 
            WS_VISIBLE  |   WS_CHILD |WS_TABSTOP|CS_NOSYSCTRL|WS_BITMAP,
            WCOLOR_RESETX, WCOLOR_RESETY,WIE_MBTN_WIDTH, WIE_MBTN_HEIGHT,
            hWnd, 
            (HMENU)WIE_IDM_RESET,
            NULL, 
            NULL
            ); 
        if (BTNReset == NULL) 
            return FALSE;                
        
        iCurItem = 0;        
		{
			//WMLCFGFILE curconfig;

			WAP_GetCurConfig(&curconfig);
			GetColorRef(curconfig.uColorCfg, curClRef);				
		}
                
        SendMessage(BTNCItem, SSBM_ADDSTRING, 0, (LPARAM)WML_SCROLL_1);
        SendMessage(BTNCItem, SSBM_ADDSTRING, 1, (LPARAM)WML_SCROLL_2);
        SendMessage(BTNCItem, SSBM_ADDSTRING, 2, (LPARAM)WML_SCROLL_3);
        SendMessage(BTNCItem, SSBM_ADDSTRING, 3, (LPARAM)WML_SCROLL_4);
        SendMessage(BTNCItem, SSBM_ADDSTRING, 4, (LPARAM)WML_SCROLL_5);        
        SendMessage(BTNCItem, SSBM_SETCURSEL, 0, NULL);        
        hFocus = BTNCItem;
        ibChgColorFocus = FALSE;        
        break;
        
    case WM_KEYDOWN:
        {
            switch (wParam)
            {
            case VK_RETURN:
                if (ibChgColorFocus)
                    WSet_Color_ChgColorClick(hWnd);                    
                break;

            case VK_UP:
                if (hFocus == BTNReset)
                {
                    WSet_Color_ChgColorSetFocus(hWnd);
                }
                else if (ibChgColorFocus)
                {
                    WSet_Color_ChgColorKillFocus(hWnd);
                    SetFocus(BTNCItem);
                }
                else
                {
                    SetFocus(BTNReset);
                }
                break;

            case VK_DOWN:
                if (hFocus == BTNCItem)
                {
                    WSet_Color_ChgColorSetFocus(hWnd);
                }
                else if (ibChgColorFocus)
                {                                        
                    WSet_Color_ChgColorKillFocus(hWnd);
                    SetFocus(BTNReset);
                }
                else
                {
                    SetFocus(BTNCItem);
                }
                break;

			case VK_F10:
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;

            default:
                return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }            
        }
        break;
        
    case WM_PENDOWN:
        {
            int x, y;
            
            x = LOWORD(lParam);
            y = HIWORD(lParam);
            if ((x >= effRect[1].left) && (x <=effRect[1].right -noffset) &&
                (y >=effRect[1].top) && (y <= effRect[1].bottom))
            {
                if (ibChgColorFocus) 
                    WSet_Color_ChgColorClick(hWnd);
                else
                {                    
                    WSet_Color_ChgColorSetFocus(hWnd);
                }                    
            }
            else
            {
                
                if (ibChgColorFocus) 
                {                        
                    WSet_Color_ChgColorKillFocus(hWnd);
                }
                else
                    SetFocus(hWnd);
            }
        }
        break;

    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
            hFocus = GetFocus();
        else
            SetFocus(hFocus);
        break;

    case WM_PAINT:
        {
            int  OldStyle;
            int  ntxtlen = 0;        
            
            hdc = BeginPaint(hWnd, NULL);
            OldStyle = SetBkMode(hdc, TRANSPARENT);
            oTextCl = SetTextColor(hdc, curClRef[iCurItem]);
            DrawText(hdc, (PCSTR)WML_COLORL_TEXT, -1, &effRect[0], DT_LEFT);
            
            if (!ibChgColorFocus)
            {
                SetTextColor(hdc, curClRef[iCurItem]);
                oBackCl = SetBkColor(hdc, curClRef[2]);
            }
            else
            {
                SetTextColor(hdc, curClRef[3]);
                SetBkColor(hdc, curClRef[4]);
            }
            
            if (strlen(WML_COLOR_FOCUS))
            {
                ntxtlen = strlen(WML_SET_CHANGECOLOR);
                noffset = effRect[1].right - effRect[1].left - ntxtlen *8;
            }
            DrawText(hdc, (PCSTR)WML_SET_CHANGECOLOR, ntxtlen,
                &effRect[1], DT_LEFT);
            DrawLine(hdc, effRect[1].left, effRect[1].bottom - 1,
                effRect[1].right -noffset, effRect[1].bottom - 1);            
            SetTextColor(hdc, oTextCl);
            SetBkColor(hdc, oBackCl);
            SetBkMode(hdc, OldStyle);
            EndPaint(hWnd, NULL);            
        }
		break;

    case WM_COLOR_SELECT:
        if (lParam == 1)
        {
            curClRef[iCurItem] = wParam;
            InvalidateRect(hWnd, &effRect[0], TRUE);
            InvalidateRect(hWnd, &effRect[1], TRUE);
            InvalidateRect(hWnd, &effRect[3], TRUE);
         }
        break;

    case WM_COMMAND:        
        mID = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {
        case WIE_IDM_ITEM:
            if (msgBox == SSBN_CHANGE)
            {
                iCurItem = SendMessage(BTNCItem, SSBM_GETCURSEL, 0, 0);
                InvalidateRect(hWnd, NULL, TRUE);                    
            }
            else  if (msgBox == SSBN_SETFOCUS)
            {
                if (ibChgColorFocus)
                    WSet_Color_ChgColorKillFocus(hWnd);
                hFocus = BTNCItem;
            }
            break;
			
        case WIE_IDM_RESET:
            if (msgBox == BN_CLICKED)
            {
                //SetDefColorRef( &(curCfgFile.uColorCfg));
                iCurItem = 0;
                GetDefColorRef(BTNCItem, curClRef);
                InvalidateRect(hWnd, &effRect[0], TRUE);
                InvalidateRect(hWnd, &effRect[1], TRUE);
                InvalidateRect(hWnd, &effRect[3], TRUE);                
            }
            else if (msgBox == BN_SETFOCUS)
            {
                if (ibChgColorFocus)
                    WSet_Color_ChgColorKillFocus(hWnd);
                
                hFocus = BTNReset;
            }
            break;            
            
        case WIE_IDM_CHANGECOLOR:
            if (msgBox == BN_CLICKED)                
            {
//              InitColorWinExt(hWnd, curClRef[iCurItem]);
                BYTE r = 0, g = 0, b = 0;
                
//              if ( !HPSelectColor("", curClRef[iCurItem], &r, &g, &b, 
//					WML_INBOX_DELETE_OK, WML_MENU_BACK))            
//              return FALSE;
                
                curClRef[iCurItem] = RGB(r, g, b);
            }
            else if (msgBox == BN_SETFOCUS)
            {
                SetFocus(hWnd);
                hFocus = hWnd;
                ibChgColorFocus = TRUE;
                InvalidateRect(hWnd, &effRect[1], TRUE);
                SendMessage(hWnd, PWM_SETSCROLLSTATE, SCROLLMIDDLE, SCROLLMIDDLE);
                SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, SCROLLLEFT);
                SendMessage(hWnd, PWM_SETSCROLLSTATE, NULL, SCROLLRIGHT);
            }
            else if (msgBox == COLORBN_KILLFOCUS)
            {
                ibChgColorFocus = FALSE; 
                InvalidateRect(hWnd, &effRect[1], TRUE);                
            }            
            break;
            
        case WIE_IDM_SET_CLOSE:
            Wml_ONCancelExit(hWnd);
            break;            
            
        case WIE_IDM_SET_OK:
			{
				//WMLCFGFILE curconfig;

				WAP_GetCurConfig(&curconfig);
				SetColorRef(&(curconfig.uColorCfg), curClRef);
				WAP_SetCurConfig(&curconfig);			            
            
				//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), 0);
			
				curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
				curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
				//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

				Saveconfig(&curconfig);
            }
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);            
            break;

        default:
            break;
        }
        break;        
        
    case WM_CLOSE:
        DestroyWindow(hWnd);        
        UnregisterClass("WMLIEColorClass", NULL);
        break;        
        
    default:        
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
       
    return lResult;
}

/*********************************************************************\
* Function       GetColorRef
* Purpose      get item color ,set coloritem NUMSPIN and 
* Params       HItem, hColorR, hColorG,  hColorB,gClCfg, *colorref
* Return            
* Remarks       
**********************************************************************/
static void GetColorRef(Color_Scheme gClCfg,COLORREF *colorref)
{
    colorref[0] = gClCfg.text_color;
    colorref[1] = gClCfg.link_textcolor;
    colorref[2] = gClCfg.link_bgcolor;
}

/**********************************************************
*void SetColorRef()
*argument: hColorR,hColorG, hColorB
*          colorref
*pupose:   显示相应的颜色值
***********************************************************/
static void SetColorRef(Color_Scheme *sClCfg, COLORREF *colorref)
{
    sClCfg->text_color = colorref[0];
    sClCfg->link_textcolor =colorref[1];
    sClCfg->link_bgcolor =colorref[2];
}

static void GetDefColorRef(HWND HItem, COLORREF *colorref)
{
    colorref[0] = DEFTEXTCOLOR;        
    colorref[1] = DEFHREFCOLOR;    
    colorref[2] = DEFWNDBACKCOLOR;    
    colorref[3] = DEFHREFFOCUSCOLOR;
    colorref[4] = DEFWNDBACKFOCUSCOLOR;  
    //ADD LIST ITEM    
    SendMessage(HItem, SSBM_SETCURSEL, 0, NULL);
}

/**********************************************************
*void SetColorRef()
*argument: hColorR,hColorG, hColorB
*          colorref
*pupose:   显示相应的颜色值
***********************************************************/
static void SetDefColorRef(Color_Scheme *sClCfg)
{
    sClCfg->text_color = DEFTEXTCOLOR;        
    sClCfg->link_textcolor = DEFHREFCOLOR;    
    sClCfg->link_bgcolor = DEFWNDBACKCOLOR;    
}

/*********************************************************************\
* Function       WIE_CacheConfig
* Purpose      Cache设置列表窗口
* Params       
* Return            
* Remarks       
**********************************************************************/
static int WIE_CacheConfig()
{
	HWND hwndCacheSETApp;
    WNDCLASS wc;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WMLCacheSettingWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLCacheSettingClass";
    
    if (!RegisterClass(&wc))
        return FALSE;

    hwndCacheSETApp = CreateWindow(
        "WMLCacheSettingClass", 
        WML_SET_CACHE, 
        WS_VISIBLE|WS_CAPTION|WS_BORDER|PWS_STATICBAR,
        PLX_WIN_POSITION, 
        NULL, 
        NULL,
        NULL, 
        NULL
        );
    if (hwndCacheSETApp == NULL) 
    {
        UnregisterClass("WMLCacheSettingClass", NULL);
        return FALSE;
    }
    
    ShowWindow(hwndCacheSETApp, SW_SHOW);            
    UpdateWindow(hwndCacheSETApp);
    
    return TRUE;
}

/*********************************************************************\
* Function       WMLCacheSettingWndProc
* Purpose      Cache设置窗口过程
* Params       
* Return            
* Remarks       
**********************************************************************/
static LRESULT WMLCacheSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    LRESULT lResult = TRUE;
    HDC hdc;
    static WORD mID;
    static WORD msgBox;    
    
    static HWND hWndSpinBoxSize;
    static HWND hWndBtnReset, hWndCheckBoxAllow;
    static HWND hFocus;    
    static BOOL bCacheSet;
    static WMLCFGFILE tempcfg;    
    
    switch (wMsgCmd)
    {
    case WM_CREATE:    
        {   
            int index;
            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, WIE_SET_CLOSE, (LPARAM)WML_MENU_BACK);
            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(WIE_SET_OK,1), (LPARAM)STR_WAPWINDOW_SAVE);
            
            hWndCheckBoxAllow = CreateWindow(
                "BUTTON",
                WML_USECACHE, 
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX,
                CACHESET_CHECKALLOW_X,
                CACHESET_CHECKALLOW_Y,
                CACHESET_CHECKALLOW_WIDTH + 20,
                CACHESET_CHECKALLOW_HEIGHT,
                hWnd, 
                (HMENU)WIE_SET_ALLOW, 
                NULL, 
                NULL);
            if (hWndCheckBoxAllow == NULL) 
                return (FALSE);                
            
            hWndSpinBoxSize = CreateWindow(
                "STRSPINBOX", 
                "", 
                WS_VISIBLE|WS_CHILD |WS_TABSTOP| SSBS_CENTER|WS_BORDER,
                CACHESET_SPINSIZE_X, 
                CACHESET_SPINSIZE_Y, 
                CACHESET_SPINSIZE_WIDTH, 
                CACHESET_SPINSIZE_HEIGHT,
                hWnd, 
                (HMENU)WIE_SET_SIZE,
                NULL, 
                NULL
                ); 
            if (hWndSpinBoxSize == NULL) 
                return FALSE;                    
            
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 0, (LPARAM)"10 kB");
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 1, (LPARAM)"20 kB");
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 2, (LPARAM)"30 kB");
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 3, (LPARAM)"40 kB");
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 4, (LPARAM)"50 kB");            
            
            Loadconfig(&tempcfg);
            index = 0;
            while (index < 5)
            {
                if (tempcfg.nCacheSize == nCacheSize[index])
                    break;
                index++;
            }
            SendMessage(hWndSpinBoxSize, SSBM_SETCURSEL, index, NULL);            
            
            if (tempcfg.CacheMode == WBCACHE_OK)
            {
                SendMessage(hWndCheckBoxAllow, BM_SETCHECK, BST_CHECKED, 0);
                bCacheSet = TRUE;            
            }
            else 
            {
                SendMessage(hWndCheckBoxAllow, BM_SETCHECK, BST_UNCHECKED, 0);
                bCacheSet = FALSE;
            }
            EnableWindow(hWndSpinBoxSize, bCacheSet);
            
            hWndBtnReset = CreateWindow(
                "BUTTON", 
                WML_SET_RESET,
                WS_VISIBLE|WS_CHILD|WS_TABSTOP|WS_BITMAP,
                CACHESET_BTNRESET_X, 
                CACHESET_BTNRESET_Y, 
                CACHESET_BTNRESET_WIDTH, 
                CACHE_BTNRESET_HEIGHT,
                hWnd, 
                (HMENU)WIE_SET_RESET,
                NULL, 
                NULL
                ); 
            if (hWndBtnReset == NULL) 
                return FALSE; 
			
			hFocus = hWndCheckBoxAllow;
        }        
        break;
        
    case WM_KEYDOWN:
		switch (wParam)
		{				
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);			
		}
		break;       
        
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
            hFocus = GetFocus();
        else
            SetFocus(hFocus);
        break;
		
    case WM_PAINT:
        {            
            int tempMode;
            hdc = BeginPaint(hWnd, NULL);            
            
            tempMode = GetBkMode(hdc);
            SetBkMode(hdc, BM_TRANSPARENT);
            TextOut(hdc, CACHESET_TEXTSIZE_X, CACHESET_TEXTSIZE_Y, WML_CACHSIZE, -1);            
            SetBkMode(hdc, tempMode);
            EndPaint(hWnd, NULL);            
        }
		break;
        
    case WM_COMMAND:        
        mID = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {
        case WIE_SET_OK:            
            {
                int index;
                int result;
				//WMLCFGFILE curconfig;
				
				WAP_GetCurConfig(&curconfig);			
            
                index = SendMessage(hWndSpinBoxSize, SSBM_GETCURSEL, 0, 0);
                
                curconfig.nCacheSize = nCacheSize[index];
                if (bCacheSet)
                    curconfig.CacheMode = WBCACHE_OK;
                else
                    curconfig.CacheMode = WBCACHE_VALIDATE;                
                
				result = PWBE_SetCACOOption(pUIBrowser, WBCACHE_SIZE, &(curconfig.nCacheSize), sizeof(int) );
                result = PWBE_SetCACOOption(pUIBrowser, WBCACHE_MODE, &(curconfig.CacheMode), sizeof(int) );
                
				curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
				curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
				//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

				WAP_SetCurConfig(&curconfig);
                Saveconfig(&curconfig);
                PLXTipsWin(NULL, NULL, NULL, WML_PROMPT_SET, NULL, Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
                PostMessage(hWnd, WM_CLOSE, NULL, NULL);            
            }
            break;            
            
        case WIE_SET_CLOSE:
            PostMessage(hWnd, WM_CLOSE, NULL, NULL);            
            break;
            
        case WIE_SET_ALLOW:
            {
                if (msgBox == BN_CLICKED)
                {
                    if (BST_CHECKED == SendMessage(hWndCheckBoxAllow, BM_GETCHECK, 0, 0))
                        bCacheSet = TRUE;                
                    else
                        bCacheSet = FALSE;
                    EnableWindow(hWndSpinBoxSize, bCacheSet);
                }
            }
            break;
            
        case WIE_SET_SIZE:
            break;

        case WIE_SET_NUM:
            break;            
            
        case WIE_SET_RESET:
            {
                if (msgBox == BN_CLICKED)
                {
                    SendMessage(hWndCheckBoxAllow, BM_SETCHECK, BST_CHECKED, 0);
                    bCacheSet = TRUE;            
                    SendMessage(hWndSpinBoxSize, SSBM_SETCURSEL, 0, NULL);
                    EnableWindow(hWndSpinBoxSize,bCacheSet);
                }
            }
            break;
			
        default:
            break;        
        }
        break;        
        
    case WM_CLOSE:        
        DestroyWindow(hWnd);            
        UnregisterClass("WMLCacheSettingClass", NULL);
        break;        
        
    default:        
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
       
	return lResult;
}

/*********************************************************************\
* Function       WIE_CookieConfig
* Purpose      Cache设置列表窗口
* Params       
* Return            
* Remarks       
**********************************************************************/
static int WIE_CookieConfig()
{
	HWND hwndCookieSETApp;
    WNDCLASS wc;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WMLCookieSettingWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLCookieSettingClass";
    
    if (!RegisterClass(&wc))
        return FALSE;        
    
    hwndCookieSETApp = CreateWindow(
        "WMLCookieSettingClass", 
        WML_SET_COOKIE,
        WS_VISIBLE|WS_CAPTION|WS_BORDER|PWS_STATICBAR,
        PLX_WIN_POSITION, 
        NULL, 
        NULL,
        NULL, 
        NULL
        );
    if (hwndCookieSETApp == NULL) 
    {
        UnregisterClass("WMLCookieSettingClass", NULL);
        return FALSE;
    }
    
    ShowWindow(hwndCookieSETApp, SW_SHOW);            
    UpdateWindow(hwndCookieSETApp);
    
    return TRUE;
}

/*********************************************************************\
* Function       WMLCacheSettingWndProc
* Purpose      Cache设置窗口过程
* Params       
* Return            
* Remarks       
**********************************************************************/
static LRESULT WMLCookieSettingWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    LRESULT lResult = TRUE;
    HDC hdc;
    static WORD mID;
    static WORD msgBox;    
    static HWND hWndSpinBoxSize, hWndSpinBoxNum;
    static HWND hWndBtnReset, hWndCheckBoxAllow;
    static HWND hFocus;    
    static BOOL bCookieSet;
    static WMLCFGFILE tempcfg;    
    
    switch (wMsgCmd)
    {
    case WM_CREATE:            
        {
            int index;
            
            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, WIE_SET_CLOSE, (LPARAM)WML_MENU_BACK);
            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(WIE_SET_OK,1), (LPARAM)STR_WAPWINDOW_SAVE);

            hWndCheckBoxAllow = CreateWindow(
                "BUTTON",
                WML_USECOOKIE,
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX,
                CACHE_CHECKALLOW_X,
                CACHE_CHECKALLOW_Y,
                CACHE_CHECKALLOW_WIDTH + 20,
                CACHE_CHECKALLOW_HEIGHT,
                hWnd, 
                (HMENU)WIE_SET_ALLOW, 
                NULL, 
                NULL);
            if (hWndCheckBoxAllow == NULL) 
                return FALSE;        
            
            bCookieSet = FALSE;
            hWndSpinBoxSize = CreateWindow(
                "STRSPINBOX", 
                "", 
                WS_VISIBLE  |  WS_CHILD |WS_TABSTOP| SSBS_CENTER|WS_BORDER,
                CACHE_SPINSIZE_X, CACHE_SPINSIZE_Y, CACHE_SPINSIZE_WIDTH, CACHE_SPINSIZE_HEIGHT,
                hWnd, 
                (HMENU)WIE_SET_SIZE,
                NULL, 
                NULL
                ); 
            if (hWndSpinBoxSize == NULL) 
                return (FALSE);
            
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 0, (LPARAM)"10 kB");
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 1, (LPARAM)"20 kB");
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 2, (LPARAM)"30 kB");
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 3, (LPARAM)"40 kB");
            SendMessage(hWndSpinBoxSize, SSBM_ADDSTRING, 4, (LPARAM)"50 kB");            
            SendMessage(hWndSpinBoxSize, SSBM_SETCURSEL, 0, NULL);    
            
            hWndSpinBoxNum = CreateWindow(
                "STRSPINBOX", 
                "", 
                WS_VISIBLE  |  WS_CHILD |WS_TABSTOP| SSBS_CENTER|WS_BORDER,
                CACHE_SPINNUM_X, CACHE_SPINNUM_Y, CACHE_SPINNUM_WIDTH, CACHE_SPINNUM_HEIGHT,
                hWnd, 
                (HMENU)WIE_SET_NUM,
                NULL, 
                NULL
                ); 
            if (hWndSpinBoxNum == NULL) 
                return FALSE;               
            
            SendMessage(hWndSpinBoxNum, SSBM_ADDSTRING, 0, (LPARAM)"1");
            SendMessage(hWndSpinBoxNum, SSBM_ADDSTRING, 1, (LPARAM)"2");
            SendMessage(hWndSpinBoxNum, SSBM_ADDSTRING, 2, (LPARAM)"3");
            SendMessage(hWndSpinBoxNum, SSBM_ADDSTRING, 3, (LPARAM)"4");
            SendMessage(hWndSpinBoxNum, SSBM_ADDSTRING, 4, (LPARAM)"5");
            SendMessage(hWndSpinBoxNum, SSBM_SETCURSEL, 0, NULL);
            Loadconfig(&curconfig);

            index = 0;
            while (index < 5)
            {
                if (tempcfg.nCookieSize == nCookieSize[index])
                    break;
                index++;
            }
            SendMessage(hWndSpinBoxSize, SSBM_SETCURSEL, index, NULL);           
            
            index = 0;
            while (index < 5)
            {
                if (tempcfg.nCookieCount == nCookieCount[index])
                    break;
                index++;
            }
            SendMessage(hWndSpinBoxNum, SSBM_SETCURSEL, index, NULL);    
            
            if (tempcfg.nCookieMode == WBCOOKIE_ENABLE)
            {
                SendMessage(hWndCheckBoxAllow, BM_SETCHECK, BST_CHECKED, 0);
                bCookieSet = TRUE; 
            }
            else 
            {
                SendMessage(hWndCheckBoxAllow, BM_SETCHECK, BST_UNCHECKED, 0);
                bCookieSet = FALSE;
            }
            EnableWindow(hWndSpinBoxSize,bCookieSet);                
            EnableWindow(hWndSpinBoxNum,bCookieSet);

            hWndBtnReset = CreateWindow(
                "BUTTON", 
                WML_SET_RESET,
                WS_VISIBLE  |   WS_CHILD |WS_TABSTOP|WS_BITMAP,
                CACHE_BTNRESET_X, CACHE_BTNRESET_Y, CACHE_BTNRESET_WIDTH, CACHE_BTNRESET_HEIGHT,
                hWnd, 
                (HMENU)WIE_SET_RESET,
                NULL, 
                NULL
                ); 
            if (hWndBtnReset == NULL) 
                return FALSE;				
			hFocus = hWndCheckBoxAllow;
        }        
        break;
        
    case WM_KEYDOWN:
		switch (wParam)
		{				
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);			
		}
		break;  
        
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
            hFocus = GetFocus();
        else
            SetFocus(hFocus);
        break;

    case WM_PAINT:
        {            
            int tempMode;

            hdc = BeginPaint(hWnd, NULL);
            tempMode = GetBkMode(hdc);
            SetBkMode(hdc, BM_TRANSPARENT);                        
            TextOut(hdc, CACHE_TEXTSIZE_X, CACHE_TEXTSIZE_Y, WML_COOKIESIZE, -1);
            TextOut(hdc, CACHE_TEXTNUM_X, CACHE_TEXTNUM_Y, WML_COOKIENUM, -1);            
            SetBkMode(hdc, tempMode);
            EndPaint(hWnd, NULL);           
        }
		break;
        
    case WM_COMMAND:        
        mID = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {
        case WIE_SET_OK:
            {
                int index;
                int result;
				int tmpmo;
				//WMLCFGFILE curconfig;
				
				WAP_GetCurConfig(&curconfig);

                index = SendMessage(hWndSpinBoxSize, SSBM_GETCURSEL, 0, 0);                
                curconfig.nCookieSize = nCookieSize[index];
                
                index = SendMessage(hWndSpinBoxNum, SSBM_GETCURSEL, 0, 0);                
                curconfig.nCookieCount = nCookieCount[index];
                
                if (bCookieSet)
                    curconfig.nCookieMode = WBCOOKIE_ENABLE;
                else
                    curconfig.nCookieMode = WBCOOKIE_DISABLE;

				tmpmo = curconfig.nCookieMode;
                
				result = PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_MODE, &(tmpmo), sizeof(int) );
                result = PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_SIZE, &(curconfig.nCookieSize), sizeof(int) );
                result = PWBE_SetCACOOption(pUIBrowser, WBCOOKIE_COUNT, &(curconfig.nCookieCount), sizeof(int) );
                
				curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
				curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
				//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

				WAP_SetCurConfig(&curconfig);
                Saveconfig(&curconfig);
                PLXTipsWin(NULL, NULL, NULL, WML_PROMPT_SET, NULL,
					Notify_Failure, IDS_OK, NULL, WAITTIMEOUT);
                PostMessage(hWnd, WM_CLOSE, 0, 0);                            
            }
            break;            
            
        case WIE_SET_CLOSE:
            PostMessage(hWnd, WM_CLOSE, 0, 0);            
            break;
            
        case WIE_SET_ALLOW:
            {
                if (msgBox == BN_CLICKED)
                {
                    if (BST_CHECKED == SendMessage(hWndCheckBoxAllow, BM_GETCHECK, 0, 0))
                        bCookieSet = TRUE;                
                    else
                        bCookieSet = FALSE;
                    
                    EnableWindow(hWndSpinBoxSize, bCookieSet);                
                    EnableWindow(hWndSpinBoxNum, bCookieSet);
                }
            }
            break;
            
        case WIE_SET_SIZE:
            break;

        case WIE_SET_NUM:
            break;            
            
        case WIE_SET_RESET:
            {
                if(msgBox == BN_CLICKED)
                {
                    SendMessage(hWndSpinBoxSize, SSBM_SETCURSEL, 0, NULL);
                    SendMessage(hWndSpinBoxNum, SSBM_SETCURSEL, 0, NULL);                        
                    SendMessage(hWndCheckBoxAllow, BM_SETCHECK, BST_CHECKED, 0);
                    bCookieSet = TRUE;                                
                    EnableWindow(hWndSpinBoxSize, bCookieSet);                
                    EnableWindow(hWndSpinBoxNum, bCookieSet);
                }
            }
            break;
        }
        break;        
        
    case WM_CLOSE:        
        DestroyWindow(hWnd);            
        UnregisterClass("WMLCookieSettingClass", NULL);
        break;            
        
    default:            
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
	}

	return lResult;
}



/*********************************************************************\
* Function       WIE_ShowConfig
* Purpose      显示设置列表窗口
* Params       
* Return            
* Remarks       
**********************************************************************/
static int WIE_ShowConfig()
{
    WNDCLASS wc;
	HWND hwndShowSETApp;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WMLShowWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLShowSettingClass";
    
    if (!RegisterClass(&wc))
        return FALSE;        
    
    hwndShowSETApp = CreateWindow(
        "WMLShowSettingClass", 
        WML_SET_SHOW,
        WS_VISIBLE|WS_CAPTION|WS_BORDER|PWS_STATICBAR,
        PLX_WIN_POSITION, 
        NULL, 
        NULL,
        NULL, 
        NULL
        );
    if (hwndShowSETApp == NULL) 
    {
        UnregisterClass("WMLShowSettingClass", NULL);
        return FALSE;
    }
    
    ShowWindow(hwndShowSETApp, SW_SHOW);            
    UpdateWindow(hwndShowSETApp);
    
    return TRUE;
}

/*********************************************************************\
* Function       WMLCacheSettingWndProc
* Purpose      显示设置窗口过程
* Params       
* Return            
* Remarks       
**********************************************************************/
static LRESULT WMLShowWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{    
    LRESULT lResult = TRUE;
    HDC hdc;
    static WORD mID;
    static WORD msgBox;    
    static HWND hWndCheckBoxShowImg, hWndCheckBoxPlayRing;
    static HWND hFocus;    
    static BOOL bShowImg;
    static BOOL bPlayRing;    
    static WMLCFGFILE tempcfg;    
    
    switch (wMsgCmd)
    {
    case WM_CREATE:            
        {
            
            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, WIE_SET_CLOSE, (LPARAM)WML_MENU_BACK);
            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(WIE_SET_OK,1), (LPARAM)STR_WAPWINDOW_SAVE);
            
            hWndCheckBoxShowImg = CreateWindow(
                "BUTTON",
                WML_SHOWIMG,   //"显示图片",
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX,
                SHOW_SHOWIMG_X,
                SHOW_SHOWIMG_Y,
                SHOW_SHOWIMG_WIDTH,
                SHOW_SHOWIMG_HEIGHT,
                hWnd, 
                (HMENU)WIE_SET_SHOWIMG, 
                NULL, 
                NULL);
            if (hWndCheckBoxShowImg == NULL) 
                return FALSE;        
            
            hWndCheckBoxPlayRing = CreateWindow(
                "BUTTON",
                WML_PLAYRING,
                WS_CHILD|WS_VISIBLE|WS_TABSTOP|BS_AUTOCHECKBOX,
                SHOW_PLAYRING_X,
                SHOW_PLAYRING_Y,
                SHOW_PLAYRING_WIDTH,
                SHOW_PLAYRING_HEIGHT,
                hWnd, 
                (HMENU)WIE_SET_PLAYRING, 
                NULL, 
                NULL);
            if (hWndCheckBoxPlayRing == NULL) 
                return FALSE;
            
            Loadconfig(&tempcfg);            
            if (tempcfg.bShowImg)
            {
                SendMessage(hWndCheckBoxShowImg, BM_SETCHECK, BST_CHECKED, 0);
                bShowImg = TRUE;
            }
            else
            {
                SendMessage(hWndCheckBoxShowImg, BM_SETCHECK, BST_UNCHECKED, 0);
                bPlayRing = FALSE;
            }
            
            if (tempcfg.bPlayRing)
            {
                SendMessage(hWndCheckBoxPlayRing, BM_SETCHECK, BST_CHECKED, 0);
                bPlayRing = TRUE;
            }
            else
            {
                SendMessage(hWndCheckBoxPlayRing, BM_SETCHECK, BST_UNCHECKED, 0);
                bPlayRing = FALSE;
            }
			hFocus = hWndCheckBoxShowImg;
        }        
        break;
        
    case WM_KEYDOWN:
		switch (wParam)
		{				
		case VK_F10:
			PostMessage(hWnd, WM_CLOSE, NULL, NULL);
			break;
			
		default:
			return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);			
		}
		break;  
        
    case WM_ACTIVATE:
        if (WA_INACTIVE == LOWORD(wParam))
            hFocus = GetFocus();
        else
            SetFocus(hFocus);
        break;

    case WM_PAINT:
        hdc = BeginPaint(hWnd, NULL);
        EndPaint(hWnd, NULL);
        break;        
        
    case WM_COMMAND:        
        mID = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {
        case WIE_SET_OK:
            {
				//WMLCFGFILE curconfig;
				
				WAP_GetCurConfig(&curconfig);
                if( BST_CHECKED == SendMessage(hWndCheckBoxShowImg, BM_GETCHECK, 0, 0))
                    bShowImg = TRUE;                
                else
                    bShowImg = FALSE;
                
                if( BST_CHECKED == SendMessage(hWndCheckBoxPlayRing, BM_GETCHECK, 0, 0))
                    bPlayRing = TRUE;                
                else
                    bPlayRing = FALSE;
                
                curconfig.bShowImg = bShowImg;
                curconfig.bPlayRing= bPlayRing; 
				
                PWBE_SetDisplayOption(pUIBrowser, WBDOP_IMGDISPLAY, &bShowImg, 0);
                PWBE_SetDisplayOption(pUIBrowser, WBDOP_BGSOUND, &bPlayRing, 0);
                
				curconfig.uColorCfg.link_Focusbgcolor = RGB(0,0,255);
				curconfig.uColorCfg.link_Focustextcolor = RGB(255,255,255);
				//PWBE_SetDisplayOption(pUIBrowser, WBDOP_COLOR, &(curconfig.uColorCfg), sizeof(Color_Scheme));

				WAP_SetCurConfig(&curconfig);
				Saveconfig(&curconfig);
                PostMessage(hWnd, WM_CLOSE, NULL, NULL);            
            }
            break;            
            
        case WIE_SET_CLOSE:
            PostMessage(hWnd, WM_CLOSE, NULL, NULL);
            break;

        default:
            break;
        }
        break;        
        
    case WM_CLOSE:
        DestroyWindow(hWnd);            
        UnregisterClass("WMLShowSettingClass", NULL);
        break;        
        
    default:        
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
       
    return lResult;
}

/*********************************************************************\
* Function     WIE_NetConfig
* Purpose      创建网关设置列表窗口
* Params       
* Return           
* Remarks      
**********************************************************************/
static BOOL WIE_GPRS()
{
    WNDCLASS wc;
	HWND hwndGPRSApp;
    
    wc.style         = 0;
    wc.lpfnWndProc   = WMLGPRSWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = "WMLGPRSClass";
    
    if (!RegisterClass(&wc))
        return FALSE;
    
    hwndGPRSApp = CreateWindow(
        "WMLGPRSClass", 
        WML_SET_GPRS,
        WS_VISIBLE|WS_CAPTION|WS_BORDER|PWS_STATICBAR,
        PLX_WIN_POSITION, 
        NULL, 
        NULL,
        NULL, 
        NULL
        );
    if (hwndGPRSApp == NULL) 
    {
        UnregisterClass("WMLGPRSClass", NULL);
        return FALSE;
    }
    
    ShowWindow(hwndGPRSApp, SW_SHOW);            
    UpdateWindow(hwndGPRSApp);
    
    return TRUE;
}

/****************************************************\
*   函数名称  : WMLNetSettingWndProc
*    函数参数  :    
*    函数功能  :    设置窗口函数
*    辅助说明  :    

*****************************************************/
static LRESULT WMLGPRSWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    LRESULT lResult = TRUE;
    HDC hdc;
    WORD mID;
    WORD msgBox; 
    static HWND hBtnGPRS;
    static int nyoff;
    static RECT rcRectUp, rcRectDown;
    int ntxtlen = 0; 
    static int nUpData, nDownData;
    char buffer[50];
    
    switch (wMsgCmd)
    {
    case WM_CREATE:
        nyoff = (CLT_HEIGHT - WIE_LBTN_HEIGHT*3)/4;        
        SetRect(&rcRectUp, (CLT_WIDTH + 16)/2, nyoff+WIE_LBTN_HEIGHT,
            (CLT_WIDTH + 16)/2+8*10, nyoff+2*WIE_LBTN_HEIGHT);
        SetRect(&rcRectDown, (CLT_WIDTH + 16)/2, 2*nyoff + 2*WIE_LBTN_HEIGHT,
            (CLT_WIDTH + 16)/2+8*10, 2*nyoff + 3*WIE_LBTN_HEIGHT);
        
        SendMessage(hWnd, PWM_SETAPPICON, 0, (LPARAM)"rom:wie.ico");                
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, WIE_IDM_SET_CLOSE, (LPARAM)WML_MENU_BACK);
        
        hBtnGPRS = CreateWindow(
            "BUTTON", 
            WML_SET_GPRS0, 
            WS_VISIBLE |WS_CHILD|WS_TABSTOP|WS_BITMAP,
            (CLT_WIDTH -WIE_LBTN_WIDTH)/2,
            3* nyoff +2* WIE_LBTN_HEIGHT,
            WIE_LBTN_WIDTH,WIE_LBTN_HEIGHT,    
            hWnd, 
            (HMENU)WIE_IDM_CFG0,
            NULL, 
            NULL
            ); 
        if (hBtnGPRS == NULL) 
            return FALSE;     
        
        nUpData = gprs_getupcount();
        nDownData = gprs_getdowncount();
        break;

    case WM_KEYDOWN:
        {   
            switch (wParam)
            {
            case VK_RETURN:
                SendMessage(hWnd, WM_COMMAND, WIE_IDM_CFG0, NULL);
                break;

			case VK_F10:
				PostMessage(hWnd, WM_CLOSE, NULL, NULL);
				break;

            default:
                return PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            }            
        }
        break;        
        
    case WM_ACTIVATE:
        if (WA_ACTIVE == LOWORD(wParam))
            SetFocus(hBtnGPRS);
        break;
        
    case WM_PAINT: 
		{
			int bkmodeold;
        hdc = BeginPaint(hWnd, NULL);        
		bkmodeold = SetBkMode(hdc, TRANSPARENT);
        ntxtlen = strlen(WML_SET_GPRSUP);
		TextOut(hdc, (CLT_WIDTH - ntxtlen*2*10)/2, nyoff, WML_SET_GPRSUP, -1);
        TextOut(hdc, (CLT_WIDTH - ntxtlen*2*10)/2, 2*nyoff + WIE_LBTN_HEIGHT, WML_SET_GPRSDOWN, -1);
        buffer[0] = 0;
        sprintf(buffer, "%dK byte", nUpData);
        DrawText(hdc, buffer, -1, &rcRectUp, DT_LEFT);
        buffer[0] = 0;
        sprintf(buffer, "%dK byte", nDownData);
        DrawText(hdc, buffer, -1, &rcRectDown, DT_LEFT);
		SetBkMode(hdc, bkmodeold);
        EndPaint(hWnd, NULL);     
		}
        break;
        
    case WM_SELCANCEL:
        break;
        
    case WM_SELOK:
        break;
        
    case WM_COMMAND:
        mID = LOWORD(wParam);
        msgBox = HIWORD(wParam);
        
        switch (mID)
        {
        case WIE_IDM_CFG0:
            if (msgBox == BN_CLICKED)
            {
                gprs_resetcount();
                nUpData = gprs_getupcount();
                nDownData = gprs_getdowncount();
                InvalidateRect(hWnd, NULL, TRUE);
                UpdateWindow(hWnd);
            }
            break;
            
        case WIE_IDM_SET_CLOSE:
            PostMessage(hWnd, WM_CLOSE, NULL, NULL);
            break;

        default:
            break;
        }
        break;
        
    case WM_CLOSE:
        DestroyWindow(hWnd);        
        UnregisterClass("WMLGPRSClass", NULL);
        break;        
        
    default:
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    
    return lResult;
}

void WML_SetCloseWindow(void)
{
	PostMessage(hWapPubSettingClo, WM_CLOSE, NULL, NULL);
	PostMessage(hWapPubSetSelectClo, WM_CLOSE, NULL, NULL);
	PostMessage(hWapConnectSettingClo, WM_CLOSE, NULL, NULL);
	PostMessage(hWapCacheSettingClo, WM_CLOSE, NULL, NULL);
	PostMessage(hWapCookieSettingClo, WM_CLOSE, NULL, NULL);
	PostMessage(hWapDisplaySettingClo, WM_CLOSE, NULL, NULL);
	PostMessage(hWapSoundSettingClo, WM_CLOSE, NULL, NULL);
}

BOOL WML_SetToDefault(void)
{
	if(!remove(WIECONFIG_FILENAME))
		return TRUE;
	else
		return FALSE;
}




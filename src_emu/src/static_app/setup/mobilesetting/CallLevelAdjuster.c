     /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : level adjustor
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
#include	"prioman.h"

#define IDC_LEVADJ 10

#define pClassName "LeveAdjClass"
#define IDC_BUTTON_QUIT	10
#define IDC_BUTTON_SET  11
#define TIMER_PLAYSOUND WM_USER+100
#define TIMER_PLAYNOTIFY WM_USER+101

static HWND hAppWnd;
static HWND hCallWnd;
static HWND hFrameWin;
static char TitleText[CAPTIONMAXLEN + 1];
static UINT wLevAdjCallBack;
static int  nLevel;
static int  nInputLev;
static unsigned long RingPlayFlag = 0, NotifyPlayFlag = 0;
static char cMusicFileName[RINGNAMEMAXLEN+1];
ADJTYPE AdjType;
static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static BOOL OnCreate(HWND hWnd, HWND* hControl);
static void OnKeyDown(HWND hWnd, UINT wMsgCmd,WPARAM wParam, LPARAM lParam);
static void OnShowWin(HWND hWnd);
static void	OnDestroy(HWND hWnd, ADJTYPE aj);
static LRESULT OnInactivate(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void OnTimer(HWND hWnd, WPARAM wParam);
static void ProcessKeyLeftRight(HWND hWnd, WPARAM wParam, LPARAM lParam);
static BOOL Sett_SetRingVolume(int levValue);

BOOL Sett_BritAdjusting(LEVELVALUE level);
BOOL Sett_FileIfExist(const char * pFileName);
BOOL DHI_SetRingVolume(int ivalue);
void IntToString( int nValue, char *buf );
BOOL Sett_GetNowPlayRing(char* cMusicFile);
void f_EnableKeyRing(bEnable); 

BOOL CallLevelAdjuster(HWND hFrame, HWND hcallWnd, ADJTYPE adjType, int nlevel,UINT wCallbackMsg, const char *szTitle)
{
    WNDCLASS wc;
    RECT rClient;

    nLevel = nlevel;
	nInputLev = nLevel;

    if(nLevel < 0 || nLevel > 5)
        return FALSE;

    hFrameWin = hFrame;   

    strncpy(TitleText, szTitle, sizeof(TitleText));        

    AdjType  = adjType;
    hCallWnd = hcallWnd;
    wLevAdjCallBack = wCallbackMsg;
    
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
    
    GetClientRect(hFrameWin, &rClient);

    hAppWnd = CreateWindow(pClassName,NULL, 
                WS_VISIBLE | WS_CHILD,                    
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin,
                NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, // set the right soft key
                (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, //set the left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)ML("Save") ) ;
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");

    SetWindowText(hFrameWin, TitleText);

    return TRUE;
}

static LRESULT  AppWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT     lResult;   
	HWND hControl;
    lResult = TRUE;
    switch(wMsgCmd)
    {
	case PWM_ACTIVATE:
		lResult = OnInactivate(hWnd, wMsgCmd, wParam, lParam);
		break;

    case WM_CREATE:
        OnCreate(hWnd, &hControl);
		SetFocus(hControl);
    	break;

	case WM_TIMER:
		OnTimer(hWnd, wParam);
		break;

    case PWM_SHOWWINDOW:
        OnShowWin(hWnd);
        break;

    case WM_KEYDOWN:
        OnKeyDown(hWnd,wMsgCmd,wParam,lParam);
        break;

    case WM_DESTROY:
		OnDestroy(hWnd, AdjType);
        break;

    default:
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    }
    return lResult;
}
static BOOL OnCreate(HWND hWnd, HWND* hControl)
{
	RECT rc;
	char cLev[1];

	memset(cMusicFileName, 0, sizeof(cMusicFileName));
	IntToString(nInputLev, cLev);
	GetClientRect(hWnd, &rc);
	
	*hControl = CreateWindow("LEVADJ", cLev, 
        WS_VISIBLE | WS_CHILD | CS_NOSYSCTRL,
        rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, 
        hWnd, (HMENU)IDC_LEVADJ, NULL, (PVOID)AdjType);

    if (*hControl == NULL)
        return FALSE;

	return TRUE;
}

static void OnKeyDown(HWND hWnd, UINT wMsgCmd,WPARAM wParam, LPARAM lParam)
{
    switch (wParam)
    {
    case VK_F10:
        PostMessage(hCallWnd,wLevAdjCallBack,0,(LPARAM)nLevel);
        SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;

    case VK_RETURN:
        PostMessage(hCallWnd,wLevAdjCallBack,1,(LPARAM)nLevel);
        SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        break;

	case VK_LEFT:
	case VK_RIGHT:
		ProcessKeyLeftRight(hWnd, wParam, lParam);
		break;

    default:
        PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
}

static void OnShowWin(HWND hWnd)
{
    SetFocus(hWnd);
	SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, // set the right soft key
                (WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Cancel") );

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, //set the left soft key
                MAKEWPARAM(IDC_BUTTON_SET,1),(LPARAM)ML("Save") ) ;
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif

    SetWindowText(hFrameWin, TitleText);
}

static void OnTimer(HWND hWnd, WPARAM wParam)
{
	switch(wParam)
	{
	case TIMER_PLAYSOUND:
		RingPlayFlag++;
		KillTimer(hWnd, TIMER_PLAYSOUND);
		Sett_GetNowPlayRing(cMusicFileName);
		if (Sett_FileIfExist(cMusicFileName))
		{
			PrioMan_CallMusicFile(cMusicFileName, PRIOMAN_PRIORITY_SETTING, 0, -1);
		}
		break;

	case TIMER_PLAYNOTIFY:
		NotifyPlayFlag++;
		KillTimer(hWnd, TIMER_PLAYNOTIFY);
		PrioMan_CallMusicFile(PRIOMAN_RINGFILE_WARNING, PRIOMAN_PRIORITY_WARNING, 0, -1);
		break;
	
	default:
		break;
	}
}

static LRESULT OnInactivate(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	if (wParam == WA_INACTIVE)
	{
		if (AdjType == LAS_RING)
		{
			if (1 == RingPlayFlag)	
			{
				RingPlayFlag = 0;
				KillTimer(hWnd, TIMER_PLAYSOUND);
			}
			if (RingPlayFlag > 1)
			{
				RingPlayFlag = 0;
				PrioMan_EndCallMusic(PRIOMAN_PRIORITY_SETTING, TRUE);
			}
		}
		else if (AdjType == LAS_NOTIFY)
		{
			if (1 == NotifyPlayFlag)	
			{
				NotifyPlayFlag = 0;
				KillTimer(hWnd, TIMER_PLAYNOTIFY);
			}
			if (NotifyPlayFlag > 1)
			{
				NotifyPlayFlag = 0;
				PrioMan_EndCallMusic(PRIOMAN_PRIORITY_WARNING, TRUE);
			}

		}
		else if (AdjType == LAS_KEY)
		{
			f_EnableKeyRing(TRUE);
		}
	}
	
	return PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
}

static void	OnDestroy(HWND hWnd, ADJTYPE aj)
{
	if (AdjType == LAS_RING)
	{
		if (1 == RingPlayFlag)	
		{
			RingPlayFlag = 0;
			KillTimer(hWnd, TIMER_PLAYSOUND);
		}
		
		if (RingPlayFlag > 1)
		{
			RingPlayFlag = 0;
			PrioMan_EndCallMusic(PRIOMAN_PRIORITY_SETTING, TRUE);
		}
	}
	else if (AdjType == LAS_NOTIFY)
	{
		if (1 == NotifyPlayFlag)	
		{
			NotifyPlayFlag = 0;
			KillTimer(hWnd, TIMER_PLAYNOTIFY);
		}
		
		if (NotifyPlayFlag > 1)
		{
			NotifyPlayFlag = 0;
			PrioMan_EndCallMusic(PRIOMAN_PRIORITY_WARNING, TRUE);
		}

	}
	else if (AdjType == LAS_KEY)
	{
		f_EnableKeyRing(TRUE);		
	}

	
	hAppWnd = NULL;
	UnregisterClass(pClassName,NULL);
}

static void ProcessKeyLeftRight(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

	nLevel = lParam;


	switch(AdjType)
	{
	
	case LAS_RING:		
		switch(wParam)
		{
		case VK_LEFT:
		case VK_RIGHT:
			if (!Sett_SetRingVolume(nLevel))
			{
				printf("\r\n DHI_SetRingVolume return false \r\n");
			}
			if (0 == RingPlayFlag)
			{
				RingPlayFlag++;
				SetTimer(hWnd, TIMER_PLAYSOUND, 1000, NULL);	
				break;
			}
			else if (1 == RingPlayFlag)
			{
				KillTimer(hWnd, TIMER_PLAYSOUND);
				SetTimer(hWnd, TIMER_PLAYSOUND, 1000, NULL);	
				break;
			}
			break;
		default:
			break;
		}
		break;
		
	case LAS_KEY:
		f_EnableKeyRing(FALSE);
		if (!Sett_SetRingVolume(nLevel))
		{
			printf("\r\n DHI_SetRingVolume return false \r\n");
		}
		PrioMan_CallMusicFile(PRIOMAN_RINGFILE_KEYBOARD, PRIOMAN_PRIORITY_SETTING, 1, -1);
		f_EnableKeyRing(TRUE);
/*
		PrioMan_EndPlayMusic(PRIOMAN_PRIORITY_KEYBOARD);
		PrioMan_CallMusic(PRIOMAN_PRIORITY_KEYBOARD, 1);
*/
		break;
	
	case LAS_NOTIFY:
		switch(wParam)
		{
		case VK_LEFT:
		case VK_RIGHT:
			if (!Sett_SetRingVolume(nLevel))
			{
				printf("\r\n DHI_SetRingVolume return false \r\n");
			}
			if (0 == NotifyPlayFlag)
			{
				NotifyPlayFlag++;
				SetTimer(hWnd, TIMER_PLAYNOTIFY, 1000, NULL);	
				break;
			}
			else if (1 == NotifyPlayFlag)
			{
				KillTimer(hWnd, TIMER_PLAYNOTIFY);
				SetTimer(hWnd, TIMER_PLAYNOTIFY, 1000, NULL);	
				break;
			}
			break;
		default:
			break;
		}
		break;
		
	case LAS_LIT:
		switch(wParam)
		{
		case VK_LEFT:
		case VK_RIGHT:
			Sett_BritAdjusting(nLevel);
			break;
		default:
			break;
		}
		break;
			
	case LAS_VOL:
		break;
	
	default:
		break;
	}
}

static BOOL Sett_SetRingVolume(int levValue)
{
	switch(levValue)
	{
	case 0:
		return DHI_SetRingVolume(LEVEL_ZERO);
	case 1:
		return DHI_SetRingVolume(LEVEL_ONE);
	case 2:
		return DHI_SetRingVolume(LEVEL_TWO);
	case 3:
		return DHI_SetRingVolume(LEVEL_THREE);
	case 4:
		return DHI_SetRingVolume(LEVEL_FOUR);
	case 5:
		return DHI_SetRingVolume(LEVEL_FIVE);
	default:
		return FALSE;
	}
}

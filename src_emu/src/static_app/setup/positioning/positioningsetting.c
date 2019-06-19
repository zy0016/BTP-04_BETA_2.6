/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : GPS profile
 *
 * Purpose  : Main window
 *
 * Autor    : 
 *
 *-----------------------------------------------------------
 *
 *$Archive::														$
 *$Workfile::														$
 *$Revision::		$		$Date::									$
\**************************************************************************/
#include	"PositioningSetting.h"
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
#include    "pubapp.h"
#include    "types.h"
#include    "stat.h"
#include	"unistd.h" 
#include	"fcntl.h"

static  HWND    hProfileList;    
static  HWND    hGPSwnd;
static  HWND    hFrameWin = NULL;
static  HBITMAP hNewIcon, hOffIcon, hPresetIconOne, hPresetIconTwo,hActProfileIcon;
static  HBITMAP* hUserItemIcon = NULL;
static  int iDelProIndex;
static	int iMenuRefFlag = 0;
static  GPSMODE* G_GPSMode = NULL;
static  int nGPSModeNum = 0;

static LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void     ProfileMainWinCommad(HWND hWnd,int iProfileIndex,WPARAM wParam,LPARAM lParam);
static BOOL CreateControl(HWND hwnd);
static void CreateMainMenu(HWND hwnd);
static int LoadGPSprofile(HWND hList);
static void LoadGPSprofileIcon(HWND hList, int nListNum);
static int ReLoadGPSprofile(HWND hList);
static void FreeGPSMode(void);

static void GetGPSIconBmp(char* IconName, char* BmpName);

extern  BOOL GetGPSprofile(GPSMODE *gm, int iGPSModeIndex);
extern  BOOL CallGPSSettingList(HWND hParentWnd, int iGPSIndex, int iNewOrEdit);
extern  BOOL CreateNewGPSProfile(void);
BOOL DelGPSProfile(int iCurMode);//
BOOL CallGPSSettingInfoView(HWND hFrameWin, int iIndex);

BOOL  PositioningSetting(HWND hwndCall)
{
    WNDCLASS    wc;
    RECT rClient;
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
    GetClientRect(hFrameWin, &rClient);
    
    hGPSwnd = CreateWindow(pClassName, NULL, 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hGPSwnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,IDC_BUTTON_QUIT, (LPARAM)ML("Back"));//right softkey        

    SetWindowText(hFrameWin, ML("GPS profiles"));
    SetFocus(hGPSwnd);

    return (TRUE);

}

static LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
    static int iIndex,iProfileIndex,iTmpNum,iListNum;
    LRESULT         lResult;
	int i;
    HDC     hdc, hDC;

	char	szIconName[GPSICONNAMEMAXLEN + 1];
    lResult = TRUE; 
    switch ( wMsgCmd )
    {
    case WM_CREATE:
		memset(szIconName, 0, sizeof(szIconName));
        CreateControl(hWnd);
        iListNum = LoadGPSprofile(hProfileList);
        
		hdc = GetDC(hWnd);
        hNewIcon        = LoadImageEx(hdc,NULL,ICON_GPS_NEW,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hOffIcon        = LoadImageEx(hdc,NULL,ICON_GPS_OFF,IMAGE_BITMAP,21,13,LR_LOADFROMFILE);
        hPresetIconOne  = LoadImageEx(hdc,NULL,ICON_GPS_PRESET_ONE,IMAGE_BITMAP,21,13,LR_LOADFROMFILE);
        hPresetIconTwo  = LoadImageEx(hdc,NULL,ICON_GPS_PRESET_TWO,IMAGE_BITMAP,21,13,LR_LOADFROMFILE);
        hActProfileIcon = LoadImageEx(hdc,NULL,ICON_PROFILE_ACTIVE,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
 		for(i=0;i<nGPSModeNum;i++)
		{
			GetGPSIconBmp(G_GPSMode[i+2].cGPSIconName, szIconName);
			G_GPSMode[i+2].hGPSIcon = LoadImageEx(hdc,NULL,szIconName,IMAGE_BITMAP,21,13,LR_LOADFROMFILE);
		}        

		ReleaseDC(hWnd,hdc);       
		LoadGPSprofileIcon(hProfileList, iListNum);
		
        SendMessage(hProfileList, LB_SETCURSEL, 0, 0);
        CreateMainMenu(hFrameWin);
		iMenuRefFlag = 0;
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_MODELIST));
        break;
        
    case PWM_SHOWWINDOW:
		if ( 1 == iMenuRefFlag )
		{
			iMenuRefFlag = 0;
			break;
		}

        SetFocus(hWnd);
        
        iIndex = SendMessage(hProfileList, LB_GETCURSEL, 0, 0);

        iListNum = ReLoadGPSprofile(hProfileList);
		
		hDC = GetDC(hWnd);
		for(i=0;i<nGPSModeNum;i++)
		{
			GetGPSIconBmp(G_GPSMode[i+2].cGPSIconName, szIconName);
			G_GPSMode[i+2].hGPSIcon = LoadImageEx(hDC,NULL,szIconName,IMAGE_BITMAP,21,13,LR_LOADFROMFILE);
		}        
		ReleaseDC(hWnd,hDC);       


        LoadGPSprofileIcon(hProfileList, iListNum);

        if(iIndex < iListNum)
            SendMessage(hProfileList, LB_SETCURSEL, iIndex, 0);
        else
            SendMessage(hProfileList, LB_SETCURSEL, 0, 0);

        SendMessage(hFrameWin, PWM_SETBUTTONTEXT,0, (LPARAM)ML("Back"));//right softkey        
        iIndex = SendMessage(hProfileList,LB_GETCURSEL,0,0);
        if( 0 == iIndex || 1 == iIndex)
        {
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            
        }
        else
        {
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
            SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, 
                MAKEWPARAM(IDC_BUTTON_EDIT,1),(LPARAM)ML("Open") ) ;
        }
        
        SetWindowText(hFrameWin, ML("GPS profiles"));     
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
    
        break;
    
    case WM_DESTROY : //Quit the application;
        DeleteObject(hNewIcon);
        DeleteObject(hOffIcon);
        DeleteObject(hPresetIconOne);
        DeleteObject(hPresetIconTwo);
        DeleteObject(hActProfileIcon);
		FreeGPSMode();
        hGPSwnd = NULL;
        UnregisterClass(pClassName,NULL);
        break;
    case WM_KEYDOWN:
	    iIndex = SendMessage(hProfileList,LB_GETCURSEL,0,0);
        switch(LOWORD(wParam))
        {       
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_RETURN:
            if(iIndex != 0 && iIndex != 1)
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_OPEN,0);				   
            break;
        case VK_F5:
            if(iIndex == 0 || iIndex == 1)
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_NEW,0);//the focus is on the "New item", the middle softkey is used.
            else 
			{
				iMenuRefFlag = 1;
				return PDADefWindowProc(hFrameWin, wMsgCmd, wParam, lParam);
			}
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;  
    
	case WM_COMMAND :
        switch (LOWORD(wParam))
        {
        case IDC_BUTTON_NEW:
            iIndex = SendMessage(hProfileList,LB_GETCURSEL,0,0); //get the current selected item number
            if( 0 == iIndex )
            {
                if( CreateNewGPSProfile())// call create new
                {
                    iTmpNum = GetGPSprofileNum();
                    CallGPSSettingList(hFrameWin, iTmpNum - 1, 0);

                }
                break;
            }
            else
            {
                SetOffGPSProfile();
				PLXTipsWin(hFrameWin, hWnd, NULL,ML("GPS switched off"),ML("GPS profiles"),Notify_Success,ML("Ok"),0,WAITTIMEOUT);
                break;
            }
            break;

		case IDC_BUTTON_OPEN:
            iIndex = SendMessage(hProfileList,LB_GETCURSEL,0,0); //get the current selected item number
            iIndex = iIndex - 2;
			CallGPSSettingInfoView(hFrameWin, iIndex);
			break;

        case IDC_BUTTON_EDIT://left softkey	
            iIndex = SendMessage(hProfileList,LB_GETCURSEL,0,0); //get the current selected item number
            iIndex = iIndex - 2;
			if ( 1 == iIndex || 0 == iIndex )
			{
				PLXTipsWin(NULL, NULL, NULL,ML("Not allowedsnfor preset profile"),NULL,Notify_Failure,ML("Ok"),0,WAITTIMEOUT);
				break;				
			}
            CallGPSSettingList(hFrameWin, iIndex, 1);
            break;
        case IDC_BUTTON_QUIT://quit
            DestroyWindow(hWnd);
            break;
        default:	   	        
            ProfileMainWinCommad(hWnd, iProfileIndex, wParam, lParam);           
            break;
        }

        switch (HIWORD(wParam))
        {
        case LBN_SELCHANGE:
            iIndex = SendMessage(hProfileList,LB_GETCURSEL,0,0);
            if( 0 == iIndex || 1 == iIndex)
            {
                SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
                SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
                
            }
            else
            {
                
                SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
                SendMessage(hFrameWin , PWM_CREATECAPTIONBUTTON, 
                    MAKEWPARAM(IDC_BUTTON_EDIT,1),(LPARAM)ML("Open") ) ;
            }
            break;
                       
        default:
            break;
        }
        break;
        
    case CALLBACK_DELACTPRO:
        if (1 == lParam)
        {                    
            DelGPSProfile(iDelProIndex);
printf("\r\n Delete the activated profile sucess \r\n");
            SetOffGPSProfile();
printf("\r\n set off the profile sucess \r\n");
            PLXTipsWin(hFrameWin, hWnd, NULL,ML("DeletedsnGPS switched off"),ML("GPS profiles"),Notify_Success,ML("Ok"),0,WAITTIMEOUT);
        }
		else
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT,0, (LPARAM)ML("Back"));//right softkey        
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)ML("Open")) ;
			SetWindowText(hFrameWin, ML("GPS profiles"));     
		}
        break;
    case CALLBACK_DELPRO:
        if (1 == lParam)
        {
            DelGPSProfile(iDelProIndex);
printf("\r\n Delete the normal profile sucess \r\n");
            PLXTipsWin(hFrameWin, hWnd, NULL,ML("Deleted"),ML("GPS profiles"),Notify_Success,ML("Ok"),0,WAITTIMEOUT);
        }
		else
		{
			SendMessage(hFrameWin, PWM_SETBUTTONTEXT,0, (LPARAM)ML("Back"));//right softkey        
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Options");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)ML("Open")) ;
			SetWindowText(hFrameWin, ML("GPS profiles"));     
		}
        break;
    
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    }
    return lResult;    
}

static BOOL CreateControl(HWND hwnd)
{
    RECT rect;

    GetClientRect(hwnd, &rect);
    
    hProfileList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right - rect.left, rect.bottom - rect.top,
        hwnd, (HMENU)IDC_MODELIST, NULL, NULL);
    if (hProfileList == NULL )
        return FALSE;
    return TRUE;

}
static void CreateMainMenu(HWND hwnd)
{
    HMENU hOptMenu;
    hOptMenu = CreateMenu();
    AppendMenu(hOptMenu,MF_STRING,IDM_OPEN,ML("Open"));
    AppendMenu(hOptMenu,MF_STRING,IDM_EDIT,ML("Edit"));
    AppendMenu(hOptMenu,MF_STRING,IDM_ACTIVE,ML("Activate"));
    AppendMenu(hOptMenu,MF_STRING,IDM_DEL,ML("Delete"));
    SetMenu(hwnd,hOptMenu);
}
static void ProfileMainWinCommad(HWND hWnd,int iProfileIndex,WPARAM wParam,LPARAM lParam)
{
	int iIndex;
	int iGPSIndex;
    char szTipsBuffer[100];
    HDC hdc;

    memset(szTipsBuffer, 0, sizeof(szTipsBuffer));

    switch(LOWORD(wParam))
    {
	case IDM_OPEN:
        PostMessage(hWnd, WM_COMMAND, IDC_BUTTON_OPEN, 0);
		break;
    case IDM_EDIT:
        PostMessage(hWnd, WM_COMMAND, IDC_BUTTON_EDIT, 0);
        break;
    case IDM_ACTIVE:
        iIndex = SendMessage(hProfileList,LB_GETCURSEL,0,0);        
        SendMessage(hProfileList, LB_GETTEXT, iIndex, (LPARAM)szTipsBuffer);
        iGPSIndex = iIndex - 2;
        SetActiveGPSProfile(iGPSIndex);

        hdc = GetDC(hWnd);
        GetExtentFittedText(hdc, szTipsBuffer, strlen(szTipsBuffer), szTipsBuffer,
                            sizeof(szTipsBuffer),SETT_TIPWIN_WIDTH, '.', 3);
        ReleaseDC(hWnd, hdc);

        strcat(szTipsBuffer, ":\n");

        strcat(szTipsBuffer, ML("Activated"));
        PLXTipsWin(NULL, NULL, NULL,szTipsBuffer, ML("Profile"), Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);

		break;
    case IDM_DEL:
        iIndex = SendMessage(hProfileList,LB_GETCURSEL,0,0); //get the current selected item number
        iGPSIndex = iIndex - 2;
        
        if (iGPSIndex >= 0 && iGPSIndex < PRESETGPSPROFILENUM)
        {
            PLXTipsWin(hFrameWin, hWnd, NULL,ML("Not allowedsnfor preset profile"),ML("GPS profiles"),Notify_Failure,ML("Ok"),0,WAITTIMEOUT);
            break;
        }   

        else
        {
            SendMessage(hProfileList, LB_GETTEXT, iIndex, (LPARAM)szTipsBuffer);
            hdc = GetDC(hWnd);
            GetExtentFittedText(hdc, szTipsBuffer, sizeof(szTipsBuffer), szTipsBuffer,
                sizeof(szTipsBuffer),SETT_TIPWIN_WIDTH, '.', 3);
            ReleaseDC(hWnd, hdc);
            strcat(szTipsBuffer, ":\n");           
            if(GetActiveGPSProfile() == iGPSIndex) //try to delete the activated GPS profile
            {
                iDelProIndex = iGPSIndex;
                strcat(szTipsBuffer, ML("Delete activesn profileask"));
                PLXConfirmWinEx(hFrameWin, hWnd, szTipsBuffer, Notify_Request, ML("GPS profiles"), ML("Yes"), ML("No"), CALLBACK_DELACTPRO);
				iMenuRefFlag = 1;
                break;
            }
            else
            {
                iDelProIndex = iGPSIndex;
                strcat(szTipsBuffer, ML("Deleteask"));
                PLXConfirmWinEx(hFrameWin, hWnd, szTipsBuffer, Notify_Request, ML("GPS profiles"), ML("Yes"), ML("No"),CALLBACK_DELPRO);
				iMenuRefFlag = 1;
                break;
            }
        }
        break;
    }
}

static int LoadGPSprofile(HWND hList)
{
    int iProfileNum, i, iret;
//    GPSMODE gm;
    iProfileNum = GetGPSprofileNum();    

	nGPSModeNum = iProfileNum - 2;
	
	G_GPSMode = (GPSMODE*)malloc(iProfileNum*sizeof(GPSMODE));

	if (G_GPSMode == NULL)
		return -1;
	memset(G_GPSMode, 0, iProfileNum * sizeof(GPSMODE));
	
       
    SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("New profile"));        
    SendMessage(hList,LB_ADDSTRING,1,(LPARAM)ML("GPS off"));

    for(i=0;i<iProfileNum;i++)
    {
        GetGPSprofile(&G_GPSMode[i], i);
		if ( 0 == i )
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Full power"));
		else if ( 1 == i)
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Economy"));
		else
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)G_GPSMode[i].cGPSModeName);
    }

    iret = SendMessage(hList, LB_GETCOUNT, 0, 0);
    return iret;
}
static int ReLoadGPSprofile(HWND hList)
{
    int iProfileNum, i, iret;

	if (G_GPSMode)
	{
		for (i=0; i<nGPSModeNum; i++)
		{
			DeleteObject(G_GPSMode[i+2].hGPSIcon);
		}
		free(G_GPSMode);
		G_GPSMode = NULL;
	}

    iProfileNum = GetGPSprofileNum();    
	nGPSModeNum = iProfileNum - 2;

	G_GPSMode = (GPSMODE*)malloc(iProfileNum*sizeof(GPSMODE));

	if (G_GPSMode == NULL)
		return -1;
	memset(G_GPSMode, 0, iProfileNum * sizeof(GPSMODE));

    SendMessage(hList, LB_RESETCONTENT, 0, 0);
    SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("New profile"));        
    SendMessage(hList,LB_ADDSTRING,1,(LPARAM)ML("GPS off"));

    for(i=0;i<iProfileNum;i++)
    {
        GetGPSprofile(&G_GPSMode[i], i);
		if ( 0 == i )
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Full power"));
		else if ( 1 == i)
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Economy"));
		else
			SendMessage(hList,LB_ADDSTRING,0,(LPARAM)G_GPSMode[i].cGPSModeName);
    }

    iret = SendMessage(hList, LB_GETCOUNT, 0, 0);
    return iret;

}


static void FreeGPSMode(void)
{
	int i;
	if(G_GPSMode)
	{
		for (i=0; i<nGPSModeNum; i++)
		{
			DeleteObject(G_GPSMode[i+2].hGPSIcon);
		}
		free(G_GPSMode);
		G_GPSMode = NULL;
	}
}

static void LoadGPSprofileIcon(HWND hList, int nListNum)
{
    DWORD dWord;
	int i;
	int iAct;
    
    i = 0;
    dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
    SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hNewIcon);
    i = 1;
    dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
    SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hOffIcon);
    i = 2;
    dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
    SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hPresetIconOne);
    i = 3;
    dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
    SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hPresetIconTwo);

    iAct = GetActiveGPSProfile();
    iAct+=2;
     

	for (i=4;i<nListNum;i++)
	{
		dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
		SendMessage(hList, LB_SETIMAGE, (WPARAM)dWord, (LPARAM)G_GPSMode[i-2].hGPSIcon);
	}
/*

    if(1 == iAct)
        return;
*/

    SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), iAct), (LPARAM)hActProfileIcon);
}

static void GetGPSIconBmp(char* IconName, char* BmpName)
{
	char* cTmp = NULL;
	
	if (IconName == NULL || BmpName == NULL)
		return;

	if (sizeof(IconName) > sizeof(BmpName))
		return;

	strcpy(BmpName, IconName);

	cTmp = strstr(BmpName, ".ico");

	if (cTmp != NULL)
	{
		strcpy(cTmp, "_Ex.bmp");
	}

}
GPSMODE* Sett_GetGPSModeList(void)
{
	return G_GPSMode;
}

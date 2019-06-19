/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : sound profile
 *
 * Purpose  : main window of sound profile
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
#include    "profilemode.h"

static const char * pClassName = "ProfileModeWndClass";

static int          iDelProIndex;
static BOOL         bShow; 
static HWND         hModeList;
static HWND         hAppWnd;
static HWND         hFrameWin=NULL;

static BOOL    iNewFlag = FALSE;
static BOOL    iDelFlag = FALSE;
    
static char szTitleBuffer[MODENAMEMAXLEN+1];

static int iActiveProfileIndex = 0;
static int iMenuRefFlag = 0;
/*
static SetProfileListFlag(BOOL iFlag, int iIndex); //set the flag when profile is deleted or created
BOOL   GetProfileListFlag(int iIndex); // get the profile flag
*/


static LRESULT AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static BOOL    CreateControl (HWND hWnd);
//static void    LoadUserModeList(HWND hwnd,int nUserProfile);
static void     CreateProfileMainMenu(HWND hWnd);
static void     ProfileMainWinCommad(HWND hWnd,WPARAM wParam,LPARAM lParam);
static  int    LoadFixedModeList(HWND hList);
static void   LoadProfileListIcon(HWND hList, int nListNum, HBITMAP hNew, 
                                  HBITMAP hSilent, HBITMAP hCommon, HBITMAP hActive);

extern void     SetCurSceneMode(int iscenemode);
extern BOOL     CallProfileList(HWND hwndCall,HWND hFather, int icurmode);
extern BOOL     CallCreateNewMode(hWnd);

/***************************************************************
* Function  CallSceneModeWindow
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/

BOOL    CallSceneModeWindow(HWND hwndCall)
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

    hAppWnd = CreateWindow(pClassName,NULL, 
                WS_VISIBLE | WS_CHILD,  

                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON,(WPARAM)IDC_BUTTON_QUIT,(LPARAM)ML("Back"));
                 
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, (LPARAM)"");   
    
    SetWindowText(hFrameWin,ML("Sound profiles"));
    
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
	static  HWND	hFocus;
    static  int		iscenemode = 0; 
    static  int     iIndex,iProfileIndex;
	static  int     nUserProfile=0;
    LRESULT         lResult;
	static  int     iTitleLen;	 
    static HBITMAP  hNewProfileIcon, hSilentProfileIcon, hNormalProfileIcon, hActProfileIcon;
    int             iTmp, nListNum;
    HDC     hdc;
    
    lResult = TRUE; 
    
    switch ( wMsgCmd )
    {
        
    case PWM_SHOWWINDOW:
        SetFocus(hWnd);
		if ( 1 == iMenuRefFlag )
		{
			iMenuRefFlag = 0;
			break;
		}
        SetWindowText(hFrameWin,ML("Sound profiles"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Back"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        
        iTmp = SendMessage(hModeList, LB_GETCURSEL, 0, 0);
        nListNum = LoadFixedModeList(hModeList);//build the list of scenemode
        LoadProfileListIcon(hModeList, nListNum, hNewProfileIcon, hSilentProfileIcon,
            hNormalProfileIcon, hActProfileIcon);  
            
        if(iTmp < nListNum)
            SendMessage(hModeList,LB_SETCURSEL,iTmp,0);
        else
            SendMessage(hModeList,LB_SETCURSEL,0,0);
        
        iIndex = SendMessage(hModeList, LB_GETCURSEL, 0, 0);
        
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
        
        CreateProfileMainMenu(hFrameWin);
        break;
        
    case WM_CREATE:
        CreateControl(hWnd);
        hdc = GetDC(hWnd);
        hNewProfileIcon    = LoadImageEx(hdc,NULL,ICON_PROFILE_NEW,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
        hSilentProfileIcon = LoadImageEx(hdc,NULL,ICON_PROFILE_SILENT,IMAGE_BITMAP,15,15,LR_LOADFROMFILE);
        hNormalProfileIcon = LoadImageEx(hdc,NULL,ICON_PROFILE_OTHERS,IMAGE_BITMAP,15,15,LR_LOADFROMFILE);
        hActProfileIcon    = LoadImageEx(hdc,NULL,ICON_PROFILE_ACTIVE,IMAGE_BITMAP,22,16,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);
        
        nListNum = LoadFixedModeList(hModeList);//build the list of scenemode

        LoadProfileListIcon(hModeList, nListNum, hNewProfileIcon, hSilentProfileIcon,
            hNormalProfileIcon, hActProfileIcon);  

        SendMessage(hModeList, LB_SETCURSEL, 0, 0);
        CreateProfileMainMenu(hFrameWin);
	 	iMenuRefFlag = 0;
        break;
        
    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_MODELIST));
        break;
    
    case WM_DESTROY : //Quit the application;
        DeleteObject(hNewProfileIcon);
        DeleteObject(hSilentProfileIcon);
        DeleteObject(hNormalProfileIcon);
        DeleteObject(hActProfileIcon);
        hAppWnd = NULL;
        UnregisterClass(pClassName,NULL);
        break;
        
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {       
        case VK_F10:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            break;
        case VK_RETURN:
            iIndex = SendMessage(hModeList,LB_GETCURSEL,0,0);
            
            if(iIndex != 0)
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_EDIT,0);//if the focus is not on the first option(New item),the left softkey is used.				   
            break;
        case VK_F5:
            iIndex = SendMessage(hModeList,LB_GETCURSEL,0,0);
            
            if(iIndex == 0)
                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_NEW,0);//the focus is on the "New item", the middle softkey is used.
            else // call menu
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
                CallCreateNewMode(hFrameWin);
                break;
            case IDC_BUTTON_EDIT://left softkey	
                iIndex = SendMessage(hModeList,LB_GETCURSEL,0,0); 
                iProfileIndex = iIndex - 1;
                CallProfileList(hFrameWin,NULL, iProfileIndex);
                break;
            default:
                ProfileMainWinCommad(hWnd,wParam, lParam);           
                break;
            }
            
            switch (HIWORD(wParam))
            {
            case LBN_SELCHANGE:
                iIndex = SendMessage(hModeList,LB_GETCURSEL,0,0);
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
        case CALLBACK_DELPROFILE:
            if(lParam) //yes
            {        
                if(DelSM(iDelProIndex))
			        PLXTipsWin(hFrameWin, hWnd, NULL,ML("Deleted"),ML("Sound profiles"),Notify_Success,ML("Ok"),0,WAITTIMEOUT);            
                break;             
            }
            else//no            
                break;
            


        default :
             lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
             break;
	}
return lResult;
}
/***************************************************************
* Function  CreateControl
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/
static BOOL CreateControl (HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);
    hModeList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        0,0,rect.right,rect.bottom,
        hWnd, (HMENU)IDC_MODELIST, NULL, NULL);
    if (hModeList == NULL )
        return FALSE;

    return TRUE;
}


/***************************************************************
* Function  CreateMainMenu
* Purpose           
* Params
* Return    
* Remarks
***************************************************************/
static void CreateProfileMainMenu(HWND hWnd)
{
    HMENU hMainMenu;
    hMainMenu = CreateMenu();
    AppendMenu(hMainMenu,MF_STRING,IDM_EDIT,ML("Edit"));
    AppendMenu(hMainMenu,MF_STRING,IDM_ACTIVE,ML("Activate"));
    AppendMenu(hMainMenu,MF_STRING,IDM_DEL,ML("Delete"));
    PDASetMenu(hWnd,hMainMenu);
}

static void ProfileMainWinCommad(HWND hWnd, WPARAM wParam, LPARAM lParam)
{

    static  char* szNameBuffer;
    static  int iNameLen, iProfileIndex;
    char szTipsBuffer[MODENAMEMAXLEN * 2 + 4];
    HDC hdc;

    memset(szTipsBuffer, 0, MODENAMEMAXLEN * 2 + 4);

    switch(LOWORD(wParam))
    {
    case IDM_EDIT:
        PostMessage(hWnd,WM_COMMAND,IDC_BUTTON_EDIT,0);
        break;

    case IDM_ACTIVE:
        iProfileIndex = SendMessage(hModeList,LB_GETCURSEL,0,0); //get the current selected item number
        iProfileIndex--;
        SendMessage(hModeList, LB_GETTEXT, iProfileIndex + 1, (LPARAM)szTipsBuffer);
        SetCurSceneMode(iProfileIndex);
 
        hdc = GetDC(hWnd);
        GetExtentFittedText(hdc, szTipsBuffer, sizeof(szTipsBuffer), szTipsBuffer,
                            sizeof(szTipsBuffer),SETT_TIPWIN_WIDTH, '.', 3);
        ReleaseDC(hWnd, hdc);

        strcat(szTipsBuffer, ":");
        strcat(szTipsBuffer, ML("snActivated"));
        PLXTipsWin(NULL, NULL, NULL,szTipsBuffer, ML("Profile"), Notify_Success, ML("Ok"), NULL, WAITTIMEOUT);
        break;

    case IDM_DEL:
        iProfileIndex = SendMessage(hModeList,LB_GETCURSEL,0,0); //get the current selected item number
        iProfileIndex--;
        if(iProfileIndex >= 0 && iProfileIndex < 4)
        {
            PLXTipsWin(NULL, NULL, NULL,ML("Not allowedsnfor preset profile"),ML("Sound profiles"),Notify_Failure,ML("Ok"),0,WAITTIMEOUT);
            break;
        }
        iProfileIndex = SendMessage(hModeList,LB_GETCURSEL,0,0); //get the current selected item number
        iProfileIndex--;
        SendMessage(hModeList, LB_GETTEXT, iProfileIndex + 1, (LPARAM)szTipsBuffer);
        hdc = GetDC(hWnd);
        GetExtentFittedText(hdc, szTipsBuffer, sizeof(szTipsBuffer), szTipsBuffer,
                            sizeof(szTipsBuffer),SETT_TIPWIN_WIDTH, '.', 3);
        ReleaseDC(hWnd, hdc);
        strcat(szTipsBuffer, ":");
        strcat(szTipsBuffer, ML("snDeleteask"));
        iDelProIndex = iProfileIndex;
        PLXConfirmWinEx(hFrameWin, hWnd, szTipsBuffer, Notify_Request,ML("Sound profiles"), ML("Yes"), ML("No"), CALLBACK_DELPROFILE); 
                                        
        break;
    }
}
/********************************************************************
* Function     LoadFixedModeList
* Purpose      add the name of profile
* Params       
* Return           
* Remarks      
**********************************************************************/
static int   LoadFixedModeList(HWND hList)
{
    static int i;
    static int nProfile;
    static int ret;
	SCENEMODE sm;

    nProfile = GetUserProfileNo();

	SendMessage(hList,LB_RESETCONTENT,0,0);  
             
    SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("New profile"));
    SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Normal"));
    SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Silent"));
    SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Noisy"));
    SendMessage(hList,LB_ADDSTRING,0,(LPARAM)ML("Discreet"));
            
	for(i=4; i<nProfile+FIXEDPROFILENUMBER; i++)
	{
		GetSM(&sm,i);
		SendMessage(hList,LB_ADDSTRING,0,(LPARAM)sm.cModeName);        
	}

    ret = SendMessage(hList, LB_GETCOUNT, 0, 0);
    return ret;
}

static void   LoadProfileListIcon(HWND hList, int nListNum, HBITMAP hNew, 
                                  HBITMAP hSilent, HBITMAP hCommon, HBITMAP hActive)
{
    DWORD dWord;
	int i, iActProfileIndex;
		    
    i = 0;
    dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
    SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hNew);
    
    i = 2;
    dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
    SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hSilent);

    for(i=3; i<nListNum; i++)
    {
    dWord = MAKEWPARAM((WORD)IMAGE_BITMAP, (WORD)i);
    SendMessage(hList,LB_SETIMAGE,(WPARAM)dWord, (LPARAM)hCommon);
    }

    iActProfileIndex = GetCurSceneMode();
    i = iActProfileIndex + 1;
    SendMessage(hList, LB_SETIMAGE, MAKEWPARAM(MAKEWORD(IMAGE_BITMAP, 1), i), (LPARAM)hActive);

}

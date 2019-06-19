/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : language setting
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
#include "langsettinglist.h"
#include "SettLangCodeTbl.h"

static  TCHAR**     LangName;
static  int         nLang;
static  HWND        LangAppWnd;
static  HWND        WriLangAppWnd;
static  HWND        hPAppWnd;
static  HWND        hFrameWin=NULL;
static void		Load_LangList(HWND hwnd,HWND hLangSel,BOOL iSwitch); //load the name of language into the list
static BOOL		CreateControl (HWND hWnd, HWND *hLangList);
static BOOL		WriLangCreateControl (HWND hWnd, HWND *hWriLangList);
static BOOL     PreInputCreateControl(HWND hWnd,HWND * hListContent);
static LRESULT  AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  WriLangAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static LRESULT  PreInputAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
BOOL SetPreInputSetting(SWITCHTYPE predictinput);
		
extern void	Sett_ProcessLangChange(void);
extern void Sett_ProcLangChanged(void);
extern void Sett_MobileProcLang(void);
extern BOOL SetWriLanguage(int iIndex);

extern void GetFavoriteLan(char* str);
//
BOOL    CallLangSettingWindow(HWND hwndCall)
{

    
	WNDCLASS wc;
    RECT rClient;
    
    hFrameWin = hwndCall;
    /*******Get the language string and amount*****/
    
    if(!GetLanguageInfo(NULL,&nLang)) //get the number of language
        return FALSE;
    
    if(!GetLanguageInfo(&LangName,NULL)) //get language string
        return FALSE;
    
    /******************End*************************/        

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    GetClientRect(hFrameWin, &rClient);

    LangAppWnd = CreateWindow(pClassName,NULL, 
                WS_VISIBLE | WS_CHILD, 
                rClient.left,
                rClient.top,
                rClient.right-rClient.left,
                rClient.bottom-rClient.top,
                hFrameWin, 
                NULL, NULL, NULL);

    if (NULL == LangAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }

    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT,(LPARAM)ML("Cancel"));
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SetWindowText(hFrameWin,ML("Phone language"));

    if (!IsWindowVisible(hFrameWin))
    {
        ShowWindow(hFrameWin, SW_SHOW);
        UpdateWindow(hFrameWin); 
    }
    SetFocus(LangAppWnd);

    return TRUE;
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;

	HDC                 hdc;

	static  int     iIndex=0,i;
    static  HWND    hLangList;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {

    case PWM_SHOWWINDOW:
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT,(LPARAM)ML("Cancel"));
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SetWindowText(hFrameWin,ML("Phone language"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetFocus(hWnd);
        break;
   
    
    case WM_CREATE :
        CreateControl (hWnd, &hLangList);
		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);
        
        if(TRUE == GetAutoLangState())//automatic
        {
            iIndex = 0;
        }
        else
        {
            for(i=0;i<nLang;i++)
            {
                if(strcmp(GetActiveLanguage(),LangName[i])==0)
                    iIndex = i+1;
            }
        }
		Load_LangList(hWnd, hLangList,TRUE);
		Load_Icon_SetupList(hLangList,hIconNormal,hIconSel,nLang+1,iIndex);
		SendMessage(hLangList,LB_SETCURSEL,iIndex,0);
  
		break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_LANGLIST));
        break;
        
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {       
		case VK_F10:
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

   case WM_COMMAND :
       switch (LOWORD(wParam))
        {
        case IDM_OK:   
             iIndex = SendMessage(hLangList,LB_GETCURSEL,0,0);              //get the current selected item number
			 if(iIndex == 0)//select automatic
                 SetAutoLanguage(SWITCH_ON);
             else
			 {
				 SetActiveLanguage(LangName[iIndex-1]);
                 SetAutoLanguage(SWITCH_OFF);
	         }  
			 Load_Icon_SetupList(hLangList,hIconNormal,hIconSel,nLang+1,iIndex);     //change the radio button of the current selected item		
		  	 Sett_ProcessLangChange();
		 	 Sett_ProcLangChanged();
			 Sett_MobileProcLang();
			 SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
             break;
        }
        break;

	case WM_LANGUAGECHANGED:
         SendMessage(hFrameWin,PWM_SETBUTTONTEXT,0,(LPARAM)ML("Cancel"));
         SetWindowText(hFrameWin,ML("Phone language"));
         break;

    case WM_DESTROY : 
	     DeleteObject(hIconNormal);
	     DeleteObject(hIconSel);
		 KillTimer(hWnd, TIMER_ASURE);
         LangAppWnd = NULL;
         UnregisterClass(pClassName,NULL);
         break;

    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;
	
	case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
			KillTimer(hWnd, TIMER_ASURE);
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    } 
    return lResult;
}
/*==========================================================================
*   Function:  static void CreateControl ( HWND hWnd )
*   Purpose :  
*   Argument:  HWND hWnd
*   Return  :  
*   Explain :               
*   NOTE    :   
*===========================================================================*/
static BOOL CreateControl (HWND hWnd, HWND *hLangList)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
    * hLangList = CreateWindow("LISTBOX", 0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_LANGLIST, NULL, NULL);
    if (* hLangList == NULL )
        return FALSE;
    return TRUE;
}

static  void   Load_LangList(HWND hwnd,HWND hLangSel,BOOL iSwitch)
{
	int i;

	SendMessage(hLangSel, LB_RESETCONTENT,0,0);  
    
	if(iSwitch == TRUE)
		SendMessage(hLangSel,LB_ADDSTRING,0,(LPARAM)ML("Automatic"));

	for(i=0;i<nLang;i++)
	{
		if (strcmp(LangName[i], "Chinese")==0)
			SendMessage(hLangSel,LB_ADDSTRING,0,(LPARAM)ML("Chinese"));
		else			
			SendMessage(hLangSel,LB_ADDSTRING,0,(LPARAM)LangName[i]);
	}
}

BOOL CallWriLangSettingWindow(HWND hwndCall)
{
    WNDCLASS wc;
    RECT rect;
    /*******Get the language string and amount*****/
    
    if(!GetLanguageInfo(NULL,&nLang)) //get the number of language
        return FALSE;
    
    if(!GetLanguageInfo(&LangName,NULL)) //get language string
        return FALSE;
    
    /******************End*************************/        
    
    hFrameWin = hwndCall;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = WriLangAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pWriLangClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    
    GetClientRect(hFrameWin, &rect);

    WriLangAppWnd = CreateWindow(pWriLangClassName,NULL, 
        WS_VISIBLE | WS_CHILD,  
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hFrameWin, NULL, NULL, NULL);

    if (NULL == WriLangAppWnd)
    {
        UnregisterClass(pWriLangClassName,NULL);
        return FALSE;
    }
        
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));
    SetWindowText(hFrameWin, ML("Writing language"));
    
    SetFocus(WriLangAppWnd);

    return TRUE;

}

static LRESULT  WriLangAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static      HBITMAP             hIconNormal;
    static      HBITMAP             hIconSel;
	HDC                 hdc;
	static  int     iIndex=0,i;
    static  HWND    hWriLangList;
    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
    {
    case WM_CREATE :
        WriLangCreateControl (hWnd, &hWriLangList);

		hdc         = GetDC(hWnd);
        hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
        hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		ReleaseDC(hWnd,hdc);

        iIndex = GetWriLanguage();
		Load_LangList(hWnd, hWriLangList,FALSE);
		Load_Icon_SetupList(hWriLangList,hIconNormal,hIconSel,nLang,iIndex);
		SendMessage(hWriLangList,LB_SETCURSEL,iIndex,0);  
		break;
        
    case PWM_SHOWWINDOW:    
        SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
        SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
        SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));
        SetWindowText(hFrameWin, ML("Writing language"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
        SetFocus(hWnd);
        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_WRILANGLIST));
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
       
		case VK_F10:
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

   case WM_COMMAND :
        switch (LOWORD(wParam))
        {
        case IDM_OK:   
		     iIndex = SendMessage(hWriLangList,LB_GETCURSEL,0,0);              //get the current selected item number
             SetWriLanguage(iIndex);
             Load_Icon_SetupList(hWriLangList,hIconNormal,hIconSel,nLang,iIndex);     //change the radio button of the current selected item		
	    	 SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
             break;
        }
        break;

    case WM_DESTROY :
	     DeleteObject(hIconNormal);
	     DeleteObject(hIconSel);
		 KillTimer(hWnd, TIMER_ASURE);
         WriLangAppWnd = NULL;
         UnregisterClass(pWriLangClassName,NULL);
         break;

	case WM_TIMER:
	    switch(wParam)
		{
		case TIMER_ASURE:
            SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
            PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;
		}
		break;
    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    } 
    return lResult;
}

static BOOL		WriLangCreateControl (HWND hWnd, HWND *hWriLangList)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

    * hWriLangList = CreateWindow("LISTBOX",0, 
       WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_WRILANGLIST, NULL, NULL);
    if (* hWriLangList == NULL )
        return FALSE;
    return TRUE;
}


BOOL CallPreInputWindow(HWND hwndCall)
{
    WNDCLASS    wc;
    RECT rect;

    hFrameWin = hwndCall;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = PreInputAppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL/*GetStockObject(WHITE_BRUSH)*/;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pPreInputClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
    
    GetClientRect(hFrameWin, &rect);

    hPAppWnd = CreateWindow(pPreInputClassName, NULL, 
        WS_VISIBLE | WS_CHILD, 
        rect.left,
        rect.top,
        rect.right-rect.left,
        rect.bottom-rect.top,
        hFrameWin, NULL, NULL, NULL);

    if (NULL == hPAppWnd)
    {
        UnregisterClass(pPreInputClassName,NULL);
        return FALSE;
    }
            
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));
   
    SetWindowText(hFrameWin, ML("Predictive input"));

    SetFocus(hPAppWnd);

    return (TRUE);
}
static LRESULT PreInputAppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{	
    static  HBITMAP hIconNormal;
    static  HBITMAP hIconSel;
	HDC hdc;
    static  HWND    hListContent;
	static  int     i,iIndex;

    LRESULT lResult;

    lResult = TRUE;   

    switch ( wMsgCmd )
	{
		case WM_CREATE :
            PreInputCreateControl (hWnd, &hListContent);
		    hdc         = GetDC(hWnd);
            hIconNormal = LoadImageEx(hdc,NULL,ICONORMAL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
            hIconSel = LoadImageEx(hdc,NULL,ICONSEL,IMAGE_BITMAP,ICONORMAL_WIDTH,ICONORMAL_HEIGHT,LR_LOADFROMFILE);
		    ReleaseDC(hWnd,hdc);
            iIndex = GetPreInputSetting();
		    Load_Icon_SetupList(hListContent,hIconNormal,hIconSel,2,iIndex);
		    SendMessage(hListContent,LB_SETCURSEL,iIndex,0);
		    break;

		case WM_SETFOCUS:
            SetFocus(GetDlgItem(hWnd,IDC_PREINPUTLIST));
            break;
            
        case PWM_SHOWWINDOW:  
            SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
            SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1,(LPARAM)"") ;
            SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, IDM_FILEEXIT, (LPARAM)ML("Cancel"));
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
            SetFocus(hWnd);
            break;
        
		case WM_KEYDOWN:
            switch(LOWORD(wParam))
            {       
		        case VK_F10:
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
            hPAppWnd = NULL;
	        DeleteObject(hIconNormal);
	        DeleteObject(hIconSel);
		    KillTimer(hWnd, TIMER_ASURE);
            UnregisterClass(pPreInputClassName,NULL);
            break;

	    case WM_COMMAND :
            switch (LOWORD(wParam))
		    {
                case IDM_OK:
	    	        iIndex = SendMessage(hListContent,LB_GETCURSEL,0,0);              //get the current selected item number
                    SetPreInputSetting(iIndex);
		    	    Load_Icon_SetupList(hListContent,hIconNormal,hIconSel,2,iIndex);     //change the radio button of the current selected item		
                    SetTimer(hWnd, TIMER_ASURE, SETT_SIGSEL_TIMEOUT, NULL);
                    break;
                default:
                    break;                 
		    }
            break;

        case WM_TIMER:
	        switch(wParam)
		    {
		        case TIMER_ASURE:
                    SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
			        break;
                default:
                    break;
		    }
	    	break;
        default :
            lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
            break;
	}
    return lResult;
}
/***************************************************************
* Function  CreateControl
* Purpose   create the control components of the window         
* Params
* Return    
* Remarks
***************************************************************/
static  BOOL    PreInputCreateControl(HWND hWnd,HWND * hListContent)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
    * hListContent = CreateWindow("LISTBOX", 0, 
        WS_VISIBLE | WS_CHILD | WS_TABSTOP |LBS_BITMAP|LBS_NOTIFY|WS_VSCROLL/*|CS_NOSYSCTRL*/,   
        rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,
        hWnd, (HMENU)IDC_PREINPUTLIST, NULL, NULL);
    if (* hListContent == NULL )
        return FALSE;
	SendMessage(* hListContent,LB_ADDSTRING,0,(LPARAM)ML("On"));
	SendMessage(* hListContent,LB_ADDSTRING,0,(LPARAM)ML("Off"));

    return TRUE;
}

SWITCHTYPE GetPreInputSetting(void)
{
    int iret;
    iret = FS_GetPrivateProfileInt(SN_WRILANGUAGE,KN_PREINPUTID,0,SETUPFILENAME);    
    if(iret == 0 || iret == 1)
        return iret;
    else 
        return SWITCH_OFF;
}

void SetAutoLanguage(SWITCHTYPE AutoLang)
{
    char LangStr[MAX_LANG_STR_LEN + 1];
    
    FS_WritePrivateProfileInt(SN_PHONELANG,KN_AUTOLANG,AutoLang,SETUPFILENAME);
   
	memset(LangStr, 0, sizeof(LangStr));
	
    if (AutoLang == SWITCH_ON)
    {
        if( GetAutoLangName(LangStr) )
        {
            SetActiveLanguage(LangStr);
        }
    }
}

BOOL GetAutoLangName(char * AutoLangName)
{
    int i = 0;
    char CountryCode[MAX_LANG_CODE_LEN + 1];

	if (AutoLangName == NULL)
		return FALSE;
	
    memset(CountryCode, 0, MAX_LANG_CODE_LEN + 1);
    GetFavoriteLan(CountryCode);
       
    while(1)
    {   
        if ( 0 == strlen(MultiLangCodeTable[i].chLangStr) )
        {
            strcpy(AutoLangName, DefaultLangStr);
            break;
        }

        if ( 0 == strcmp(CountryCode, MultiLangCodeTable[i].nchLangCode))
        {
            strcpy(AutoLangName, MultiLangCodeTable[i].chLangStr);
            break;
        }

        i++;
    }

    return TRUE;
}

BOOL GetAutoLangState(void)
{
    int ret = FS_GetPrivateProfileInt(SN_PHONELANG,KN_AUTOLANG,SWITCH_ON,SETUPFILENAME);
    if(ret == SWITCH_ON)
        return TRUE;
    else
        return FALSE;
}

BOOL SetPreInputSetting(SWITCHTYPE predictinput)
{
   FS_WritePrivateProfileInt(SN_WRILANGUAGE,KN_PREINPUTID,predictinput,SETUPFILENAME);
   return TRUE;
}

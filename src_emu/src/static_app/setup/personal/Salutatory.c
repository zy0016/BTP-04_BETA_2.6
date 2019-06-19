 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 桌面短语设置
 *            
\**************************************************************************/
//
//#include    "Salutatory.h"
//
//#define ITEMSNUM    4
//
//static const char * pClassName = "SalutatoryWndClass";
//static const int    iControlX = 10;
//static HINSTANCE    hInstance;
//
////static InputEdit    InputEditDesk;
////static InputEdit  InputEditScreen;
//
//static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//static  BOOL    CreateControl(HWND hwnd,HWND * hDesk,HWND * hScreen);//创建控件
//void    GetDeskPhrase  (char * );       //获得开机桌面短语的函数(char * 的长度应为DESK_LIMIT + 1)
//void    GetScreenPhrase(char * );       //获得开机桌面短语的函数(char * 的长度应为SCREEN_LIMIT + 1)
//BOOL    CountControlsYaxis(int * piConY,unsigned int iControlH,size_t count);
//extern  int     GetScreenUsableWH1(int iwh);
//int     GetEditControlHeight(void);
//int     GetCharactorHeight(HWND hWnd);
//
//BOOL    CallSalutatoryWindow(HWND hwndCall)
//{
//    HWND hwnd;
//    WNDCLASS wc;
//
//    wc.style         = CS_OWNDC;
//    wc.lpfnWndProc   = AppWndProc;
//    wc.cbClsExtra    = 0;
//    wc.cbWndExtra    = 0;
//    wc.hInstance     = NULL;
//    wc.hIcon         = NULL;
//    wc.hCursor       = NULL;
//    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
//    wc.lpszMenuName  = NULL;
//    wc.lpszClassName = pClassName;
//
//    if (!RegisterClass(&wc))
//    {
//        return FALSE;
//    }
//
//    hwnd = CreateWindow(pClassName,TITLECAPTION, 
//        WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
//        PLX_WIN_POSITION , hwndCall, NULL, NULL, NULL);
//
//    if (NULL == hwnd)
//    {
//        UnregisterClass(pClassName,NULL);
//        return FALSE;
//    }
//
//    ShowWindow(hwnd, SW_SHOW);
//    UpdateWindow(hwnd); 
//
//    return TRUE;
//}
//
//static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
//{    
//    static      HWND    hFocus = 0;
//    static      HWND    hDesk;//开机桌面短语
//    static      HWND    hScreen;//屏保桌面短语
//                char    cDesk[DESK_LIMIT + 1] = "";
//                //char    cScreen[SCREEN_LIMIT + 1] = "";
//                RECT    rect1;
//                int     iRectw,iy[ITEMSNUM];
//                LRESULT lResult;
//                int     nEnd = 0,nPos = 0;
//                HWND    hCurWin = 0;
//                HDC     hdc;
//
//    lResult = (LRESULT)TRUE;
//    switch ( wMsgCmd )
//    {
//    case WM_CREATE :
//        if (!CreateControl(hWnd,&hDesk,&hScreen))
//        {
////            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
//            return -1;
//        }
//        hFocus = hDesk;
//        GetDeskPhrase   (cDesk);    //开机桌面短语
//        //GetScreenPhrase (cScreen);  //屏保桌面短语
//        SetWindowText   (hDesk,cDesk);
//        //SetWindowText   (hScreen,cScreen);
//        break;
//
//    case WM_SETFOCUS:
//        SetFocus(hFocus);
//        break;
//
//    case WM_SETRBTNTEXT:
//        SendMessage (hWnd, PWM_SETBUTTONTEXT, 0, lParam);
//        break;
//
//    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
//        if (WA_INACTIVE != LOWORD(wParam))
//        {
//            if ((hFocus != hDesk) && (hFocus != hScreen))
//                hFocus = hDesk;
//            SetFocus(hFocus);//激活后设置焦点
//        }
//        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        InvalidateRect(hWnd,NULL,TRUE);
//        break;
//
//    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
//        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
//        break;
//
//    case WM_DESTROY ://通知程序管理器,此程序退出;
//        UnregisterClass(pClassName,NULL);
//        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
//        break;
//
//    case WM_PAINT :
//        hdc = BeginPaint( hWnd, NULL);
//        CountControlsYaxis(iy,GetEditControlHeight(),sizeof(iy));
//        iRectw = GetScreenUsableWH1(SM_CXSCREEN);
//
//        SetRect(&rect1,iControlX,iy[0],iRectw - iControlX * 2,iy[0] + GetCharactorHeight(hWnd));
//        DrawText(hdc,OPENDESKPHRASE,strlen(OPENDESKPHRASE),&rect1,DT_CENTER|DT_HCENTER);
//
//        //SetRect(&rect2,10,100,iRectw - 10,120);
//        //DrawText(hdc,SCREENPHRASE,strlen(SCREENPHRASE)    ,&rect2,DT_CENTER|DT_HCENTER);
//        EndPaint(hWnd, NULL);
//        break;
//    case WM_KEYDOWN:
//        switch(LOWORD(wParam))
//        {
//        case VK_F10:
//            DestroyWindow(hFocus);
//            DestroyWindow( hWnd );  
//            break;
//        case VK_RETURN:
//            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
//            break;  
//        default:
//            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        }
//        break;
//        
//    case WM_COMMAND:
//        switch( LOWORD( wParam ))
//        {
//        case IDC_BUTTON_SET://设置
//            GetWindowText(hDesk,cDesk,GetWindowTextLength(hDesk) + 1);
//            FS_WritePrivateProfileString(SN_DESK,KN_DESKPHRASE,cDesk,SETUPFILENAME);
//            
//            /*iScreen = GetWindowTextLength(hScreen) + 1;//屏保桌面短语
//            GetWindowText (hScreen,cScreen,iScreen);
//            FS_WritePrivateProfileString(SN_SCREEN,KN_SCREENPHRASE,cScreen,SETUPFILENAME);*/
//            
//            DestroyWindow(hFocus);
//            DestroyWindow( hWnd );  
//            break;
//        case IDC_BUTTON_QUIT://退出
//            hCurWin = GetFocus();
//            SendMessage(hCurWin, EM_GETSEL, (WPARAM)&nPos, (LPARAM)&nEnd);
//            if (!nEnd)
//                DestroyWindow( hWnd );  
//            else
//                SendMessage(hCurWin, WM_KEYDOWN, VK_BACK, 0);
//                
//            break;
//        }
//        break;
// 
//    default :
//        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        break;
//    }
//    return lResult;
//}
///********************************************************************
//* Function   CreateControl  
//* Purpose    创建控件 
//* Params     hwnd：窗口过程句柄
//* Return     
//* Remarks     创建本届面所需的界面元素，包括"确定"，"取消"按钮，为窗口加图标，添加指示箭头. 
//**********************************************************************/
//static  BOOL    CreateControl(HWND hwnd,HWND *hDesk,HWND *hScreen)
//{
//    int icontrolh = GetEditControlHeight(),icontrolw,iy[ITEMSNUM];
//	IMEEDIT	imeedit;
//
//    SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, // 右键退出
//                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
//
//    SendMessage(hwnd , PWM_CREATECAPTIONBUTTON, //左键确定
//                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );
//
//    SendMessage(hwnd, PWM_SETAPPICON, 0, WINDOWICO);//为窗口加图标
//    SendMessage( hwnd, PWM_SETSCROLLSTATE,
//                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );
//
//    icontrolw = GetScreenUsableWH1(SM_CXSCREEN) - iControlX * 2;
//    CountControlsYaxis(iy,icontrolh,sizeof(iy));
//
//	memset(&imeedit,0,sizeof(IMEEDIT));
//
//	imeedit.hwndNotify		= hwnd;
//	imeedit.dwAttrib		&= IME_ATTRIB_GENERAL;
//	imeedit.uMsgSetText		= 0;
//	imeedit.dwUniTextMax	= 0;
//	imeedit.dwAscTextMax	= 0;
//	imeedit.pszImeName		= "NULL";
//    imeedit.pszTitle		= "";
//
//    *hDesk = CreateWindow(//特殊的文字输入控件
//        "IMEEDIT", "", 
//        WS_CHILD | WS_VISIBLE|ES_UNDERLINE | WS_TABSTOP |WS_BORDER,
//        iControlX,iy[1] , icontrolw, icontrolh,    
//        hwnd,(HMENU)IDC_DESK,NULL, (PVOID)&imeedit);
//
//    if (* hDesk == NULL)
//        return FALSE;
//
//    /** hScreen =  CreateWindow(//特殊的文字输入控件
//        "INPUTEDIT", "", 
//        WS_CHILD | WS_VISIBLE|ES_UNDERLINE|WS_TABSTOP,
//        iControlX,iy[3], icontrolw, icontrolh,    
//        hwnd,(HMENU)IDC_SCREEN,NULL, NULL);
//
//    if (* hScreen == NULL)
//        return FALSE;*/
//
//    //开机桌面短语部分
//    SendMessage(* hDesk,EM_LIMITTEXT,DESK_LIMIT,0);//限制字符个数
//
//    //屏保桌面短语部分
//    /*SendMessage(* hScreen,EM_LIMITTEXT,SCREEN_LIMIT,0);//限制字符个数
//
//    InputEditScreen.lStyle        = GetWindowLong(* hScreen, GWL_STYLE);
//    InputEditScreen.nKeyBoardType = GetImeHandle(REGARDEDITENG);//英数
//    strcpy ( InputEditScreen.szName, SCREENPHRASE );
//    SetWindowLong ( * hScreen, GWL_USERDATA, (LONG)&InputEditScreen );*/
//
//    return TRUE;
//}
///********************************************************************
//* Function     GetDeskPhrase
//* Purpose     获得开机桌面短语(cDesk的长度应为DESK_LIMIT + 1)
//* Params     
//* Return     
//* Remarks     
//**********************************************************************/
//void    GetDeskPhrase(char * cDesk)
//{
//    char    cWord[DESK_LIMIT + 1] = "";
//    FS_GetPrivateProfileString(SN_DESK,KN_DESKPHRASE,DESK_DEFAULT,
//                                cWord,DESK_LIMIT,SETUPFILENAME);
//    strncpy(cDesk,cWord,DESK_LIMIT);
//}
///********************************************************************
//* Function     GetScreenPhrase
//* Purpose     获得屏保桌面短语(cScreen的长度应为SCREEN_LIMIT + 1)
//* Params     
//* Return     
//* Remarks     
//**********************************************************************/
//void    GetScreenPhrase(char * cScreen)
//{
//    char    cWord[SCREEN_LIMIT + 1] = "";
//    FS_GetPrivateProfileString(SN_SCREEN,KN_SCREENPHRASE,SCREEN_DEFAULT,
//                                cWord,SCREEN_LIMIT,SETUPFILENAME);
//    strncpy(cScreen,cWord,SCREEN_LIMIT);
//}
///********************************************************************
//* Function     Restore_Phrase
//* Purpose     用于恢复默认设置
//* Params     
//* Return     
//* Remarks     
//**********************************************************************/
//BOOL    Restore_Phrase(void)
//{
//    FS_WritePrivateProfileString(SN_DESK    ,KN_DESKPHRASE  ,DESK_DEFAULT   ,SETUPFILENAME);            
//    FS_WritePrivateProfileString(SN_SCREEN  ,KN_SCREENPHRASE,SCREEN_DEFAULT ,SETUPFILENAME);
//    return TRUE;
//}

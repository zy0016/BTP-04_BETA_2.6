 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : �����������
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
//static  BOOL    CreateControl(HWND hwnd,HWND * hDesk,HWND * hScreen);//�����ؼ�
//void    GetDeskPhrase  (char * );       //��ÿ����������ĺ���(char * �ĳ���ӦΪDESK_LIMIT + 1)
//void    GetScreenPhrase(char * );       //��ÿ����������ĺ���(char * �ĳ���ӦΪSCREEN_LIMIT + 1)
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
//    static      HWND    hDesk;//�����������
//    static      HWND    hScreen;//�����������
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
//        GetDeskPhrase   (cDesk);    //�����������
//        //GetScreenPhrase (cScreen);  //�����������
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
//    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
//        if (WA_INACTIVE != LOWORD(wParam))
//        {
//            if ((hFocus != hDesk) && (hFocus != hScreen))
//                hFocus = hDesk;
//            SetFocus(hFocus);//��������ý���
//        }
//        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        InvalidateRect(hWnd,NULL,TRUE);
//        break;
//
//    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
//        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
//        break;
//
//    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
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
//        case IDC_BUTTON_SET://����
//            GetWindowText(hDesk,cDesk,GetWindowTextLength(hDesk) + 1);
//            FS_WritePrivateProfileString(SN_DESK,KN_DESKPHRASE,cDesk,SETUPFILENAME);
//            
//            /*iScreen = GetWindowTextLength(hScreen) + 1;//�����������
//            GetWindowText (hScreen,cScreen,iScreen);
//            FS_WritePrivateProfileString(SN_SCREEN,KN_SCREENPHRASE,cScreen,SETUPFILENAME);*/
//            
//            DestroyWindow(hFocus);
//            DestroyWindow( hWnd );  
//            break;
//        case IDC_BUTTON_QUIT://�˳�
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
//* Purpose    �����ؼ� 
//* Params     hwnd�����ڹ��̾��
//* Return     
//* Remarks     ��������������Ľ���Ԫ�أ�����"ȷ��"��"ȡ��"��ť��Ϊ���ڼ�ͼ�꣬���ָʾ��ͷ. 
//**********************************************************************/
//static  BOOL    CreateControl(HWND hwnd,HWND *hDesk,HWND *hScreen)
//{
//    int icontrolh = GetEditControlHeight(),icontrolw,iy[ITEMSNUM];
//	IMEEDIT	imeedit;
//
//    SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, // �Ҽ��˳�
//                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
//
//    SendMessage(hwnd , PWM_CREATECAPTIONBUTTON, //���ȷ��
//                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );
//
//    SendMessage(hwnd, PWM_SETAPPICON, 0, WINDOWICO);//Ϊ���ڼ�ͼ��
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
//    *hDesk = CreateWindow(//�������������ؼ�
//        "IMEEDIT", "", 
//        WS_CHILD | WS_VISIBLE|ES_UNDERLINE | WS_TABSTOP |WS_BORDER,
//        iControlX,iy[1] , icontrolw, icontrolh,    
//        hwnd,(HMENU)IDC_DESK,NULL, (PVOID)&imeedit);
//
//    if (* hDesk == NULL)
//        return FALSE;
//
//    /** hScreen =  CreateWindow(//�������������ؼ�
//        "INPUTEDIT", "", 
//        WS_CHILD | WS_VISIBLE|ES_UNDERLINE|WS_TABSTOP,
//        iControlX,iy[3], icontrolw, icontrolh,    
//        hwnd,(HMENU)IDC_SCREEN,NULL, NULL);
//
//    if (* hScreen == NULL)
//        return FALSE;*/
//
//    //����������ﲿ��
//    SendMessage(* hDesk,EM_LIMITTEXT,DESK_LIMIT,0);//�����ַ�����
//
//    //����������ﲿ��
//    /*SendMessage(* hScreen,EM_LIMITTEXT,SCREEN_LIMIT,0);//�����ַ�����
//
//    InputEditScreen.lStyle        = GetWindowLong(* hScreen, GWL_STYLE);
//    InputEditScreen.nKeyBoardType = GetImeHandle(REGARDEDITENG);//Ӣ��
//    strcpy ( InputEditScreen.szName, SCREENPHRASE );
//    SetWindowLong ( * hScreen, GWL_USERDATA, (LONG)&InputEditScreen );*/
//
//    return TRUE;
//}
///********************************************************************
//* Function     GetDeskPhrase
//* Purpose     ��ÿ����������(cDesk�ĳ���ӦΪDESK_LIMIT + 1)
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
//* Purpose     ��������������(cScreen�ĳ���ӦΪSCREEN_LIMIT + 1)
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
//* Purpose     ���ڻָ�Ĭ������
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

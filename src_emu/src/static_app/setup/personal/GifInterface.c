 /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : �����б�ӿ�
 *            
\**************************************************************************/
//
//#include    "GifInterface.h"
//
//static const char * pClassName = "GifInterFaceWndClass";
//static const int    iRectY = 5;
//static const int    iRectW = 140;
//static const int    iRectH = 140;
//
//static HINSTANCE    hInstance;
//
//static HWND         hCallWnd;
//static int          hCallMsg;
//static char         cSelectGifFileName[CARTOONFILENAMELEN + 1] ;//����ַ���
//static char         cTitle[50];
//static char         cInitGifFileName[CARTOONFILENAMELEN + 1] ;
//
//static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//static  BOOL    CreateControl(HWND hWnd,HWND * hList);//�����ؼ�
////********************�ⲿ����********************
//extern  int     iCallCartoonMaxWidth(void);//���綯���ĺϷ����
//extern  int     iCallCartoonMaxHeight(void);//���綯���ĺϷ��߶�
//extern  BOOL    LoadCartoon(HWND hList,CARTOONS ** cartoons,int iControlType,BOOL bIfShowName);
//extern  BOOL    GetFactName(char * cfilename,unsigned int buflen,HWND hList,CARTOONS * cartoons,int iControlType);
//extern  void    GetScreenUsableWH2 (int * pwidth,int * pheight);
//extern  int     GetScreenUsableWH1(int iwh);
//
////*********************************************************
//BOOL    CallGifInterfaceWindow(HWND hwndCall,int iMsg,char * ptitle,char * pFileName)
//{
//    HWND        hWnd;
//    WNDCLASS    wc;
//    int         iSour;
//
//    if ((ptitle == NULL) || (pFileName == NULL))
//        return FALSE;
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
//    if ((ptitle == NULL) || (strlen(ptitle) == 0))
//    {
//        strcpy(cTitle,TITLECAPTION);
//    }
//    else
//    {
//        iSour = strlen(ptitle);
//        strncpy(cTitle,ptitle,sizeof(cTitle) > iSour ? iSour + 1 : sizeof(cTitle) - 1);
//    }
//    memset(cInitGifFileName,0x00,sizeof(cInitGifFileName));
//    if (pFileName != NULL)
//    {
//        iSour = strlen(pFileName);
//        if (iSour > 0)
//        {
//            strncpy(cInitGifFileName,pFileName,
//                sizeof(cInitGifFileName) > iSour ? iSour + 1 : sizeof(cInitGifFileName) - 1);
//        }
//        else
//        {
//#if defined(_EMULATE_)
//            //strcpy(cInitGifFileName,"/rom/prefab/image/g7.gif");
//#endif
//        }
//    }
//    hWnd = CreateWindow(pClassName,cTitle, 
//        WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
//        PLX_WIN_POSITION , hwndCall, NULL, NULL, NULL);
//
//    if (NULL == hWnd)
//    {
//        UnregisterClass(pClassName,NULL);
//        return FALSE;
//    }
//    ShowWindow(hWnd, SW_SHOW);
//    UpdateWindow(hWnd); 
//
//    hCallWnd = hwndCall;
//    hCallMsg = iMsg;
//
//    return (TRUE);
//}
//
//static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
//{
//    static      HWND            hList;
//    static      CARTOONS        * cartoons, * p;
//    static      HGIFANIMATE     hgifanimate;
//    static      char            cfilename[CARTOONFILENAMELEN + 1] = "";
//    static      int             iSizeType;//�Ƿ��ǺϷ��ĳߴ�,1�ߴ�Ϸ�,0,�ߴ糬��
//    static      SIZE            size;
//    static      int             iRECTX;//��ʾ�����ľ�������ĺ�����
//                int             index;
//                HBITMAP         htemp;
//                RECT            rect;
//                COLORREF        Color;
//                BOOL            bTran;
//                HPEN            hp;
//                HBRUSH          hbr;
//                LRESULT         lResult;
//                HDC             hdc;
//
//    lResult = (LRESULT)TRUE;
//    switch ( wMsgCmd )
//    {
//    case WM_CREATE :
//        if (!CreateControl(hWnd,&hList))
//        {
//            return -1;
//        }
//        hgifanimate = NULL;
//        cartoons    = NULL;
//        iRECTX      = (GetScreenUsableWH1(SM_CXSCREEN) - iRectW) / 2;
//        memset(cSelectGifFileName,0x00,sizeof(cSelectGifFileName));
//        LoadCartoon(hList,&cartoons,0,FALSE);
//        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, //ȫ��Ԥ��
//                MAKEWPARAM(IDC_BUTTON_PREVIEW,2), (LPARAM)"");
//
//        index = 0;
//        p = cartoons;
//        while (p != NULL)
//        {
//            if (strcasecmp(p->cFilename,cInitGifFileName) == 0)
//                break;
//
//            p = p->next;
//            index++;
//        }
//        SendMessage(hList,LB_SETCURSEL,index,0);
//        
//        GetFactName(cfilename,sizeof(cfilename),hList,cartoons,0);
//        SendMessage(hWnd,IDC_INIT,1,0);
//        break;
//    case IDC_INIT:
//        if (strlen(cfilename) == 0)
//        {
//            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, //ȫ��Ԥ��
//                MAKEWPARAM(IDC_BUTTON_PREVIEW,2), (LPARAM)"");
//            InvalidateRect(hWnd,NULL,TRUE);
//            memset(&size,0x00,sizeof(SIZE));
//            break;
//        }
//        GetImageDimensionFromFile(cfilename,&size);//����ļ��ߴ�
//        SetRect(&rect,iRECTX,iRectY,iRECTX + iRectW,iRectY + iRectH);
//
//        if ((size.cx > iRectW) || (size.cy > iRectH))//�ߴ糬��
//        {
//            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, //ȫ��Ԥ��
//                MAKEWPARAM(IDC_BUTTON_PREVIEW,2), BUTTONPREVIEW);
//            iSizeType = 0;//�ߴ糬��
//            InvalidateRect(hWnd,&rect,TRUE);
//        }
//        else
//        {
//            SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, //ȫ��Ԥ��
//                MAKEWPARAM(IDC_BUTTON_PREVIEW,2), (LPARAM)"");
//
//            SetFocus(hList);
//            iSizeType = 1;//�ߴ�Ϸ�
//            hgifanimate = StartAnimatedGIFFromFile(hWnd,cfilename,
//                iRECTX + (iRectW - size.cx) / 2,iRectY + (iRectH - size.cy) / 2,DM_NONE);
//
//            if (wParam)
//                InvalidateRect(hWnd,NULL,TRUE);
//            else
//                InvalidateRect(hWnd,&rect,TRUE);
//        }
//        break;
//
//    case WM_SETFOCUS:
//        SetFocus(GetDlgItem(hWnd,IDC_LIST));
//        break;
//
//    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
//        if (WA_INACTIVE != LOWORD(wParam))
//            SetFocus(GetDlgItem(hWnd,IDC_LIST));//��������ý���
//        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        break;
//
//    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
//        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
//        break;
//
//    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
//        EndAnimatedGIF(hgifanimate);
//        while (cartoons != NULL)
//        {
//            p = cartoons;
//            cartoons = cartoons->next;
//            free(p);
//        }
//        UnregisterClass(pClassName,NULL);
//        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
//        break;
//
//    case WM_PAINT :
//        hdc = BeginPaint( hWnd, NULL);
//        PaintAnimatedGIF(hdc,hgifanimate);
//
//        hp  = GetStockObject(BLACK_PEN);
//        hbr = GetStockObject(NULL_BRUSH);
//        SelectObject(hdc,hbr);
//        RoundRect(hdc,iRECTX,iRectY,iRECTX + iRectW,iRectY + iRectH,5,5);
//
//        if (0 == iSizeType)//�ߴ糬��
//        {
//            htemp = CreateBitmapFromImageFile(hdc, cfilename,&Color,&bTran);
//            StretchBlt( hdc,
//                iRECTX + 2,iRectY + 2,iRectW - 4,iRectH - 4,//Ŀ�ľ���
//                (HDC)htemp,
//                0,0,size.cx,size.cy,//ͼ��ԭ����
//                SRCCOPY);//��λͼ��Դ���ο�����Ŀ�ľ��Σ���չ��ѹ��λͼ��ʹ���ʺ�Ŀ�ľ��εķ�Χ
//            
//            DeleteObject(htemp);
//        }
//        EndPaint(hWnd, NULL);
//        break;
//
//    case WM_KEYDOWN:
//        switch(LOWORD(wParam))
//        {
//        case VK_F10:
//            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
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
//        case IDC_LIST:
//            switch(HIWORD(wParam))
//            {
//            case LBN_DBLCLK:
//                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SET,0);
//                break;
//            case LBN_SELCHANGE:
//                GetFactName(cfilename,sizeof(cfilename),hList,cartoons,0);
//                EndAnimatedGIF(hgifanimate);
//                SendMessage(hWnd,IDC_INIT,1,0);
//                break;
//            }
//            break;
//        case IDC_BUTTON_PREVIEW://ȫ��Ԥ��
//            CallCartoon_PreWindow(hWnd,cfilename);
//            break;
//        case IDC_BUTTON_SET://����
//            if ((size.cx > iCallCartoonMaxWidth()) || (size.cy > iCallCartoonMaxHeight()))
//            {
////                AppMessageBox(hWnd,SIZEINVALID,"",WAITTIMEOUT);//���ʵĶ�����СΪ240*120
//                break;
//            }
//            GetFactName(cSelectGifFileName,sizeof(cSelectGifFileName),hList,cartoons,0);
//            PostMessage(hCallWnd,hCallMsg,1,0);
//            SendMessage(hWnd,IDC_EXIT,0,0);
//            break;
//        case IDC_BUTTON_QUIT://�˳�
//            memset(cSelectGifFileName,0x00,sizeof(cSelectGifFileName));
//            //PostMessage(hCallWnd,hCallMsg,0,0);
//            SendMessage(hWnd,IDC_EXIT,0,0);
//            break;
//        }
//        break;
//    case IDC_EXIT://�˳�����
//        DestroyWindow( hWnd );  
//        break;
//    default :
//        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        break;
//    }
//    return lResult;
//}
//void    GetReplyGifNameData(char * ringname,unsigned int buflen)
//{
//    if (ringname != NULL)
//        strncpy(ringname,cSelectGifFileName,buflen);
//}
///********************************************************************
//* Function   CreateControl  
//* Purpose    �����ؼ� 
//* Params     hwnd�����ڹ��̾��
//* Return     
//* Remarks     ��������������Ľ���Ԫ�أ�����"ȷ��"��"ȡ��"��ť��Ϊ���ڼ�ͼ�꣬���ָʾ��ͷ. 
//**********************************************************************/
//static  BOOL    CreateControl(HWND hWnd,HWND * hList)
//{
//    int isw,ish;
//
//    GetScreenUsableWH2(&isw,&ish);
//    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, // �Ҽ��˳�
//                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
//    
//    SendMessage(hWnd , PWM_CREATECAPTIONBUTTON, //���ȷ��
//                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );
//    
//    SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, //ȫ��Ԥ��
//                MAKEWPARAM(IDC_BUTTON_PREVIEW,2), BUTTONPREVIEW);
//
//    SendMessage(hWnd, PWM_SETAPPICON, 0, WINDOWICO);//Ϊ���ڼ�ͼ��
//
//    SendMessage( hWnd, PWM_SETSCROLLSTATE,
//                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );
//    
//    * hList = CreateWindow("LISTBOX", "", 
//        WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP |WS_HSCROLL|WS_VSCROLL,
//        0, iRectY + iRectH + 2, isw, ish - (iRectY + iRectH + 2),
//        hWnd, (HMENU)IDC_LIST, NULL, NULL);
//
//    if ( * hList == NULL )
//        return FALSE;
//    
//    return TRUE;
//}

///***************************************************************************
// *
// *                      Pollex Mobile Platform
// *
// * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
// *                       All Rights Reserved 
// *
// * Module   : 
// *
// * Purpose  : ��������
// *            
//\**************************************************************************/
//
//#include    "cartoon.h"
//
//#define     RADIO1Y                 10
//#define     RADIO2Y                 30
//#define     RADIO3Y                 50
//#define     RADIO4Y                 70
//
//static const char * pClassName = "CartoonWndClass";
//static const char * pCHOMIT = "...";
//static const char * pPoint = ".";
//static const int    iRadioX = 5;
//static const int    iRadioW = 85;
//static const int    iRound1 = 76;
//static const int    iRound2 = 160;
//static const int    iRoundW = 25;
//
//static const int    iControlX = 90;
//static const int    iControlW = 70;
//static const int    iRectX = 5;//��ʾ�����ľ������������Ϳ��
//static const int    iRectY = RADIO3Y + 30;
//static const int    iRectW = 80;
//static const int    iRectH = 80;
//static const int    iLineMaxLen = 20;
//
//static HINSTANCE    hInstance;
//
//static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//static  BOOL    CreateControl(HWND hwnd,HWND * hRadio1,HWND * hRadio2,HWND * hRadio3,HWND * hRadio4,HWND * hList,HWND * hButton);//�����ؼ�
//        BOOL    GetCartoon(CARTOON * cartoon);
//static  int     SetCartoon(HWND hWnd,HWND hList,HWND hRadio1,HWND hRadio2,HWND hRadio3,HWND hRadio4,CARTOONS * cartoons);
//        BOOL    LoadCartoon(HWND hList,CARTOONS ** cartoons,int iControlType,BOOL bIfShowName);
//static  BOOL    SetCurSel(int type,char * cfilename);
//static  void    DisplayText(HDC hdc,CARTOONS * cartoons);
//static  BOOL    GetShowName(char * cfilename,char * cshowname,CARTOONS * cartoons);
//        BOOL    GetFactName(char * cfilename,unsigned int buflen,HWND hList,CARTOONS * cartoons,int iControlType);
//extern BOOL AppConfirmWin (HWND hwnd,const char * szPrompt,const char * szTipsPic, const char * szCaption,const char * szOk,const char * szCancel);
//extern  int GetButtonControlHeight (void);
////*********************************************************
//BOOL    CallCartoonWindow(HWND hwndCall)
//{
//    HWND        hwnd;
//    WNDCLASS    wc;
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
//#if 0
//    {   
//        BOOL    b;
//        b = CreateDirectory("/mnt/flash/test1");
//        b = CopyFile("/rom/setup/g8.gif","/mnt/flash/test1/test8.gif");
//    }
//#endif
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
//    ShowWindow(hwnd, SW_SHOW);
//    UpdateWindow(hwnd); 
//
//    return (TRUE);
//}
//static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
//{
//    static      HWND            hFocus = 0;
//    static      HWND            hRadio1,hRadio2,hRadio3,hRadio4;
//    static      HWND            hList,hButton;
//    static      HGIFANIMATE     hgifanimate;
//    static      char            cfilename[CARTOONFILENAMELEN + 1] = "";
//    static      CARTOONS    *   cartoons, * p;
//    static      int             iSizeType;//�Ƿ��ǺϷ��ĳߴ�,1�ߴ�Ϸ�,0,�ߴ糬��
//    static      SIZE            size;
//                RECT            rect;
//                HBITMAP         htemp;
//                COLORREF        Color;
//                BOOL            bTran;
//                int             iRes;
//                LRESULT         lResult;
//                HDC             hdc;
//
//    lResult = (LRESULT)TRUE;
//    switch ( wMsgCmd )
//    {
//    case WM_CREATE :
//        hgifanimate = NULL;
//        cartoons    = NULL;
//        p           = NULL;
//        if (!CreateControl(hWnd,&hRadio1,&hRadio2,&hRadio3,&hRadio4,&hList,&hButton))
//        {
////            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
//            return -1;
//        }
//        LoadCartoon(hList,&cartoons,1,TRUE);
//
//        hFocus = hRadio1;
//        break;
//
//    case IDC_INIT:
//        if (!strlen(cfilename))
//        {
//            ShowWindow(hButton,SW_HIDE);
//            InvalidateRect(hWnd,NULL,TRUE);
//            break;
//        }
//        GetImageDimensionFromFile(cfilename,&size);//����ļ��ߴ�
//        SetRect(&rect,iRectX,iRectY,iRectX + iRectW,iRectY + iRectH);
//
//        if ((size.cx > iRectW) || (size.cy > iRectH))//�ߴ糬��
//        {
//            ShowWindow(hButton,SW_SHOW);
//            iSizeType = 0;//�ߴ糬��
//            InvalidateRect(hWnd,&rect,TRUE);
//        }
//        else
//        {
//            iSizeType = 1;//�ߴ�Ϸ�
//            hgifanimate = StartAnimatedGIFFromFile(hWnd,cfilename,
//                iRectX + (iRectW - size.cx) / 2,
//                iRectY + (iRectH - size.cy) / 2,DM_NONE);
//
//            if (wParam)
//                InvalidateRect(hWnd,NULL,TRUE);
//            else
//                InvalidateRect(hWnd,&rect,TRUE);
//
//            ShowWindow(hButton,SW_HIDE);
//        }
//        break;
//    case WM_SETFOCUS:
//        SetFocus(hFocus);
//        break;
//
//    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
//        if (WA_INACTIVE != LOWORD(wParam))
//            SetFocus(hFocus);//��������ý���
//        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        InvalidateRect(hWnd,NULL,TRUE);
//        break;
//
//    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
//        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
//        break;
//
//    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
//        while (cartoons != NULL)
//        {
//            p = cartoons;
//            cartoons = cartoons->next;
//            free(p);
//        }
//        EndAnimatedGIF(hgifanimate);
//        UnregisterClass(pClassName,NULL);
//        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
//        break;
//
//    case WM_PAINT :
//        hdc = BeginPaint( hWnd, NULL);
//        
//        PaintAnimatedGIF(hdc,hgifanimate);
//        
//        DisplayText(hdc,cartoons);
//        if ((!iSizeType) && (strlen(cfilename) != 0))//�ߴ糬��
//        {
//            htemp = CreateBitmapFromImageFile(hdc, cfilename,&Color,&bTran);
//            StretchBlt( hdc,
//                iRectX + 2,iRectY + 2,iRectW - 4,iRectH - 4,//Ŀ�ľ���
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
//        case IDC_PREVIEW:
//            switch (HIWORD(wParam))
//            {
//            case BN_CLICKED:
//                CallCartoon_PreWindow(hWnd,cfilename);
//                break;
//            }
//            break;
//        case IDC_RADIO1:
//            switch(HIWORD(wParam))
//            {
//            case BN_SETFOCUS:
//                CheckRadioButton(hWnd,IDC_RADIO1,IDC_RADIO4,IDC_RADIO1);
//                memset(cfilename,0,sizeof(cfilename));
//                SetCurSel(IDC_RADIO1,cfilename);//��õ�ǰ�����ļ���
//
//                EndAnimatedGIF(hgifanimate);
//                SendMessage(hWnd,IDC_INIT,0,0);
//                break;
//            }
//            break;
//        case IDC_RADIO2:
//            switch(HIWORD(wParam))
//            {
//            case BN_SETFOCUS:
//                CheckRadioButton(hWnd,IDC_RADIO1,IDC_RADIO4,IDC_RADIO2);
//                memset(cfilename,0,sizeof(cfilename));
//                SetCurSel(IDC_RADIO2,cfilename);//��õ�ǰ�����ļ���
//
//                EndAnimatedGIF(hgifanimate);
//                SendMessage(hWnd,IDC_INIT,0,0);
//                break;
//            }
//            break;
//        case IDC_RADIO3:
//            switch(HIWORD(wParam))
//            {
//            case BN_SETFOCUS:
//                CheckRadioButton(hWnd,IDC_RADIO1,IDC_RADIO4,IDC_RADIO3);
//                memset(cfilename,0,sizeof(cfilename));
//                SetCurSel(IDC_RADIO3,cfilename);//��õ�ǰ�����ļ���
//
//                EndAnimatedGIF(hgifanimate);
//                SendMessage(hWnd,IDC_INIT,0,0);
//                break;
//            }
//            break;
//        case IDC_RADIO4:
//            switch(HIWORD(wParam))
//            {
//            case BN_SETFOCUS:
//                CheckRadioButton(hWnd,IDC_RADIO1,IDC_RADIO4,IDC_RADIO4);
//                memset(cfilename,0,sizeof(cfilename));
//                SetCurSel(IDC_RADIO4,cfilename);//��õ�ǰ�����ļ���
//
//                EndAnimatedGIF(hgifanimate);
//                SendMessage(hWnd,IDC_INIT,0,0);
//                break;
//            }
//            break;
//        case IDC_LIST:
//            switch (HIWORD(wParam))
//            {
//            case CBN_SELCHANGE:
//                GetFactName(cfilename,sizeof(cfilename),hList,cartoons,1);//����б���ж���ʵ���ļ���
//                
//                EndAnimatedGIF(hgifanimate);
//                SendMessage(hWnd,IDC_INIT,0,0);
//                break;
//            }
//            break;
//        case IDC_BUTTON_START://����
//            iRes = SetCartoon(hWnd,hList,hRadio1,hRadio2,hRadio3,hRadio4,cartoons);
//            switch (iRes)
//            {
//            case 1:
//                AppMessageBox(hWnd,SAVESMSUCC,TITLECAPTION, WAITTIMEOUT);
//                break;
//            case 0:
//                AppMessageBox(hWnd,SAVESMERROR,TITLECAPTION, WAITTIMEOUT);
//                break;
//            case -1:
//                return TRUE;
//            }
//            GetFactName(cfilename,sizeof(cfilename),hList,cartoons,1);//����б���ж���ʵ���ļ���
//            
//            EndAnimatedGIF(hgifanimate);
//            SendMessage(hWnd,IDC_INIT,1,0);//��־ˢ��ȫ��
//
//            break;
//        case IDC_BUTTON_SET://����
//            iRes = SetCartoon(hWnd,hList,hRadio1,hRadio2,hRadio3,hRadio4,cartoons);
//            switch (iRes)
//            {
//            case 1:
//                AppMessageBox(hWnd,SAVESMSUCC,TITLECAPTION, WAITTIMEOUT);
//                SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
//                break;
//            case 0:
//                AppMessageBox(hWnd,SAVESMERROR,TITLECAPTION, WAITTIMEOUT);
//                break;
//            case -1:
//                break;
//            }
//            break;
//        case IDC_BUTTON_QUIT://�˳�
//            //DestroyWindow(hFocus);?
//            //ѡ��"ȫ��Ԥ��"��ť֮��,��ѡ�񱾰�ť,��ִ��DestroyWindow(hFocus)��ʱ��
//            //����,����ѡ��"ȫ��Ԥ��"��ť֮��,��ѡ��"����"��ť��ʱ��,ִ��
//            //DestroyWindow(hFocus)��ʱ������,����Ϊʲô?
//            DestroyWindow( hWnd );  
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
//int iCallCartoonMaxWidth(void)//���綯���ĺϷ����
//{
//    return (240);
//}
//int iCallCartoonMaxHeight(void)//���綯���ĺϷ��߶�
//{
//    return (120);
//}
///********************************************************************
//* Function   DisplayText  
//* Purpose     ��ʾ�йص�ͼ��,�Լ�����
//* Params     
//* Return     
//* Remarks     
//**********************************************************************/
//static  void    DisplayText(HDC hdc,CARTOONS * cartoons)
//{
//    CARTOON     cartoon;
//    HPEN        hp,oldhp;
//    HBRUSH      hbr,oldhbr;
//    RECT        rect[4];
//    int         oldmode,i;
//    ROUNDRECT   round[] =
//    {
//        {iRound1,RADIO1Y,iRound2,RADIO1Y + iRoundW,5,5},
//        {iRound1,RADIO2Y,iRound2,RADIO2Y + iRoundW,5,5},
//        {iRound1,RADIO3Y,iRound2,RADIO3Y + iRoundW,5,5},
//        {iRound1,RADIO4Y,iRound2,RADIO4Y + iRoundW,5,5}
//    };
//    char showname[CSHOWNAMEMAXLEN + 1] = "";
//
//    hp      = CreatePen(PS_SOLID,0,RGB(192,192,192));
//    hbr     = CreateSolidBrush(RGB(255,255,113));
//    oldhp   = SelectObject(hdc,hp);
//    oldhbr  = SelectObject(hdc,hbr);
//    
//    for (i = 0;i < 3;i++)//������ʾ�������Ƶ��ĸ�����
//    {
//        RoundRect(hdc       ,round[i].x1,round[i].y1    ,round[i].x2,round[i].y2,round[i].nHeight,round[i].nWidth);
//        SetRect  (&rect[i]  ,round[i].x1,round[i].y1 + 4,round[i].x2,round[i].y2);
//    }
//    SelectObject(hdc,oldhp);
//    SelectObject(hdc,oldhbr);
//    DeleteObject(hp);
//    DeleteObject(hbr);
//
//    hbr = GetStockObject(NULL_BRUSH);
//    SelectObject(hdc,hbr);
//    RoundRect(hdc,iRectX,iRectY,iRectX + iRectW,iRectY + iRectH,5,5);
//
//    oldmode = SetBkMode(hdc, BM_TRANSPARENT);
//    
//    memset(&cartoon,0x00,sizeof(CARTOON));
//    GetCartoon(&cartoon);//��ȡ������Ϣ
//
//    memset(showname,0,sizeof(showname));
//    GetShowName(cartoon.cOpen,showname,cartoons);//��ÿ���������ʾ�ļ���
//    DrawText(hdc,showname,strlen(showname),&rect[0],DT_HCENTER|DT_CENTER);
//
//    memset(showname,0,sizeof(showname));
//    GetShowName(cartoon.cClose,showname,cartoons);//��ùػ�������ʾ�ļ���
//    DrawText(hdc,showname,strlen(showname),&rect[1],DT_HCENTER|DT_CENTER);
//
//    /*memset(showname,0,sizeof(showname));
//    GetShowName(cartoon.cScreen,showname,cartoons);//�������������ʾ�ļ���
//    DrawText(hdc,showname,strlen(showname),&rect[2],DT_HCENTER|DT_CENTER);*/
//
//    memset(showname,0,sizeof(showname));
//    GetShowName(cartoon.cCall,showname,cartoons);//������綯����ʾ�ļ���
//    DrawText(hdc,showname,strlen(showname),&rect[2],DT_HCENTER|DT_CENTER);
//
//    SetBkColor(hdc,oldmode);
//}
///********************************************************************
//* Function   GetShowName  
//* Purpose    ��������,���Ҷ����ļ�����Ӧ����ʾ����
//* Params     cfilename:��Ҫ���ҵĶ����ļ���,cshowname:��Ҫ��ʾ������
//* Return     �Ƿ��ҵ�
//* Remarks     
//**********************************************************************/
//static  BOOL    GetShowName(char * cfilename,char * cshowname,CARTOONS * cartoons)
//{
//    CARTOONS *  p;
//    
//    p = cartoons;
//    while (p != NULL)
//    {
//        if (strcmp(p->cFilename,cfilename) == 0)
//        {
//            strcpy(cshowname,p->cShowname);
//            return TRUE;
//        }
//        p = p->next;
//    }
//    return FALSE;
//}
///********************************************************************
//* Function   GetFactName  
//* Purpose    ��������,�����б��ѡ���Ӧ��ʵ���ļ���
//* Params     cfilename:���ض���ʵ���ļ���,buflen:������buflen,
//            hList:�б��,cartoons:����
//            iControlType:�ؼ�����,1Ϊ�����б�,0Ϊ�б��
//* Return     �Ƿ��ҵ�
//* Remarks     
//**********************************************************************/
//BOOL    GetFactName(char * cfilename,unsigned int buflen,HWND hList,CARTOONS * cartoons,int iControlType)
//{
//    int         i,icursel;
//    CARTOONS *  p;
//
//    icursel = SendMessage(hList,iControlType ? CB_GETCURSEL : LB_GETCURSEL,0,0);
//
//    memset(cfilename,0x00,buflen);
//    if (icursel != -1)
//    {
//        p = cartoons;
//        i = 0;
//        while (p != NULL)
//        {
//            if (i == icursel)
//                break;
//            p = p->next;
//            i++;
//        }
//        strcpy(cfilename,p->cFilename);
//    }
//    return TRUE;
//}
///********************************************************************
//* Function   SetCurSel  
//* Purpose     ѡ�е�ǰ��ѡ��ťʱ�Ĳ���
//* Params     type:��־��һ����ѡ��ť,cfilename:��Ҫ���ظ���Ŀ�Ķ����ļ���
//            cartoons:����ͷ�ڵ�
//* Return     
//* Remarks     
//**********************************************************************/
//static  BOOL    SetCurSel(int type,char * cfilename)
//{
//    char        filename[CARTOONFILENAMELEN + 1] = "";
//    int         ides,isour;
//    CARTOON     car;
//
//    memset(&car,0x00,sizeof(CARTOON));
//    GetCartoon(&car);
//
//    ides = sizeof(filename);
//    switch (type)
//    {
//    case IDC_RADIO1:
//        isour = strlen(car.cOpen);
//        strncpy(filename,car.cOpen,ides > isour ? isour + 1: ides - 1);
//        break;
//    case IDC_RADIO2:
//        isour = strlen(car.cClose);
//        strncpy(filename,car.cClose,ides > isour ? isour + 1: ides - 1);
//        break;
//    case IDC_RADIO3:
//        isour = strlen(car.cScreen);
//        strncpy(filename,car.cScreen,ides > isour ? isour + 1: ides - 1);
//        break;
//    case IDC_RADIO4:
//        isour = strlen(car.cCall);
//        strncpy(filename,car.cCall,ides > isour ? isour + 1: ides - 1);
//        break;
//    }
//    strcpy(cfilename,filename);
//    return TRUE;
//}
///********************************************************************
//* Function   LoadCartoon  
//* Purpose     ����ROM,FLASH�е�gif�ļ�
//* Params     CARTOONS:����ͷ�ڵ�
//            iControlType:�ؼ�����,1Ϊ�����б�,0Ϊ�б��
//            bIfShowName:TRUE��������ʵ���ļ�����ʾΪ����1,����2...
//            bIfShowName:FALSE��ʾ������ʵ���ļ���(û��·��)
//* Return     
//* Remarks     
//**********************************************************************/
//BOOL    LoadCartoon(HWND hList,CARTOONS ** cartoons,int iControlType,BOOL bIfShowName)
//{
//    int     index,ides,isour;
//    char    oldpath[30] = "";
//    char    filename[CARTOONFILENAMELEN + 1] = "",cshowname[CSHOWNAMEMAXLEN + 1] = "";
//    char *  pPath[] = //��Ҫ������·��
//    {
//        "/rom/",
//        "/mnt/flash/",
//        ""
//    };
//    char ** path = pPath;
//    char *  pexpend[] = //��Ҫ�������ļ�����
//    {
//        ".gif",
//        ""
//    };
//    char **     pex = pexpend;
//    struct dirent  *dirinfo  =NULL; 
//    CARTOONS *  p,* pnext;
//	DIR *dirtemp = NULL;
//
//    SendMessage(hList,iControlType ? CB_RESETCONTENT : LB_RESETCONTENT,0,0);
//   // GetCurrentDirectory(oldpath,sizeof(oldpath));//����ԭ·��
//	getcwd(oldpath, sizeof(oldpath));
//
//    index = 1;//���ڱ�־�Ƕ�����(����1,����2...),����COMBOBOX��һ����"��",���Դ�1��ʼ
//
//    p = (CARTOONS *)malloc(sizeof(CARTOONS));//��ӵ�һ���ڵ�:����"��"
//    if (p == NULL)
//        return FALSE;
//
//    memset(p,0x00,sizeof(CARTOONS));
//    strcpy(p->cFilename,"");        //��������Ϊ��
//    strcpy(p->cShowname,RADIOTXT0); //��ʾ��������Ϊ"��"
//    p->next = * cartoons;
//    * cartoons = p;
//
//    SendMessage(hList,iControlType ? CB_ADDSTRING : LB_ADDSTRING,0,(LPARAM)RADIOTXT0);//���"��"
//    //////////////////////��ʼ��ROM,FLASH2�в���GIF����
//    while (** path)
//    {
//		dirtemp = opendir(*path);
//
//		if (!dirtemp)
//        {
//            path++;
//            continue;
//        }
//        pex = pexpend;//��չ��
//
//		dirinfo = readdir(dirtemp);
//
//        while(** pex)
//        {
//		    
//            while (dirinfo && dirinfo->d_name[0])
//            {   
//                if(strstr(dirinfo->d_name, *pex) == 0)
//				{
//					dirinfo = readdir(dirtemp);
//					continue;
//				}
//                p = (CARTOONS *)malloc(sizeof(CARTOONS));
//                if (p == NULL)
//                    return FALSE;
//                
//                memset(p,0x00,sizeof(CARTOONS));
////////////////////////////////////////////////////////////////////////////
//                memset(filename ,0,sizeof(filename));
//                memset(cshowname,0,sizeof(cshowname));
//
//                strcpy(filename,* path);        //���·��
//                strcat(filename,dirinfo->d_name);  //����ļ���(filename����·�����ļ���)
//                //��Ӷ���ʵ���ļ���(����·��)
//                ides  = sizeof(p->cFilename);
//                isour = strlen(filename);
//                strncpy(p->cFilename,filename,ides > isour ? isour + 1: ides - 1);
//                //��Ӹö�������ʾ����(����index)
//                sprintf(cshowname,CARTOONTXT,index);
//                ides  = sizeof(p->cShowname);
//                isour = strlen(cshowname);
//                strncpy(p->cShowname,cshowname,ides > isour ? isour + 1: ides - 1);
//
//                pnext = * cartoons;
//                while (pnext->next != NULL)//�����������β
//                    pnext = pnext->next;
//
//                pnext->next = p;//���½��Ľڵ���ӵ������β
//                p->next = NULL;
////////////////////////////////////////////////////////////////////////////
//                if (bIfShowName)
//                {
//                    SendMessage(hList,iControlType ? CB_ADDSTRING : LB_ADDSTRING,0,(LPARAM)cshowname);
//                }
//                else
//                {
//                    char cFName[CSHOWNAMEMAXLEN + 1] = "",* pFName = NULL;
//                    char cFNameBack[CSHOWNAMEMAXLEN + 1] = "";
//                    int iFLen;
//                    pFName = strstr(dirinfo->d_name,pPoint);
//                    if (pFName == NULL)
//                    {
//                        strcpy(cFName,dirinfo->d_name);
//                    }
//                    else
//                    {
//                        iFLen = abs(dirinfo->d_name - pFName);
//                        strncpy(cFName,dirinfo->d_name,iFLen);
//                    }
//                    if ((int)strlen(cFName) > iLineMaxLen)
//                    {
//                        strncpy(cFNameBack,cFName,iLineMaxLen);
//                        strcat(cFNameBack,pCHOMIT);//"..."
//                        SendMessage(hList,iControlType ? CB_ADDSTRING : LB_ADDSTRING,0,
//                            (LPARAM)cFNameBack);//fd.cFileName
//                    }
//                    else
//                    {
//                        SendMessage(hList,iControlType ? CB_ADDSTRING : LB_ADDSTRING,0,
//                            (LPARAM)cFName);//fd.cFileName
//                    }
//                }
//                index++;
//                if (!(dirinfo = readdir(dirtemp))) 
//                    break;
//            }
//            pex++;
//			
//        }
//		closedir(dirtemp);
//        path++;
//		dirtemp = NULL;
//		dirinfo  =NULL;
//    }
//
//    chdir(oldpath);
//    SendMessage(hList,iControlType ? CB_SETCURSEL : LB_SETCURSEL,0,0);
//    return TRUE;
//}
///********************************************************************
//* Function   SetCartoon  
//* Purpose     ���ö�����Ϣ
//* Params     
//* Return     1:���óɹ�,0,����ʧ��,-1:����ȡ��
//* Remarks     
//**********************************************************************/
//static  int     SetCartoon(HWND hWnd,HWND hList,HWND hRadio1,HWND hRadio2,HWND hRadio3,HWND hRadio4,CARTOONS * cartoons)
//{
//    SIZE        size = {0,0};
//    int         ival[] = {0,0,0,0},icur,i;
//    CARTOONS *  p;
//    char        clew[40] = "";
//    char    *   txt[] = 
//    {
//        RADIOTXT1,
//        RADIOTXT2,
//        RADIOTXT3,
//        RADIOTXT4
//    };
//    ival[0] = SendMessage(hRadio1,BM_GETCHECK ,0,0);
//    ival[1] = SendMessage(hRadio2,BM_GETCHECK ,0,0);
//    ival[2] = SendMessage(hRadio3,BM_GETCHECK ,0,0);
//    ival[3] = SendMessage(hRadio4,BM_GETCHECK ,0,0);
//    icur    = SendMessage(hList  ,CB_GETCURSEL,0,0);
//
//    if (icur == 0)//ѡ��"��"֮�����ʾ,�����б�����ʾ�Ķ�����Ϊ"��"
//    {
//        for (i = 0;i < 4;i++)
//        {
//            if (ival[i])
//                break;
//        }
//        sprintf(clew,CLEW,txt[i]);
//        if (AppConfirmWin(hWnd,clew,"",TITLECAPTION,GetString(STR_WINDOW_OK),GetString(STR_WINDOW_CANCEL)) == FALSE)
//            return -1;
//    }
//    p = cartoons;
//    i = 0;
//    while (p != NULL)//����������Ҹö�����Ӧ��ʵ���ļ���
//    {
//        if (i == icur)
//            break;
//        p = p->next;
//        i++;
//    }
//    
//    if (ival[0])
//        FS_WritePrivateProfileString(SN_CARTOON,KN_CARTOON_OPEN  ,p->cFilename,SETUPFILENAME);
//    else if (ival[1])
//        FS_WritePrivateProfileString(SN_CARTOON,KN_CARTOON_CLOSE ,p->cFilename,SETUPFILENAME);
//    else if (ival[2])
//        FS_WritePrivateProfileString(SN_CARTOON,KN_CARTOON_SCREEN,p->cFilename,SETUPFILENAME);
//    else
//    {
//        GetImageDimensionFromFile(p->cFilename,&size);
//        if ((size.cx > iCallCartoonMaxWidth()) || (size.cy > iCallCartoonMaxHeight()))
//        {
//            AppMessageBox(hWnd,SIZEINVALID,TITLECAPTION,WAITTIMEOUT);//���ʵĶ�����СΪ240*120
//            return -1;
//        }
//        FS_WritePrivateProfileString(SN_CARTOON,KN_CARTOON_CALL,p->cFilename,SETUPFILENAME);
//    }
//    return 1;
//}
///********************************************************************
//* Function   Restore_Cartoon  
//* Purpose     ���ڻָ�Ĭ������
//* Params     
//* Return     
//* Remarks     
//**********************************************************************/
//BOOL    Restore_Cartoon(void)
//{
//    FS_WritePrivateProfileString(SN_CARTOON,KN_CARTOON_OPEN  ,DEFGIF0,SETUPFILENAME);
//    FS_WritePrivateProfileString(SN_CARTOON,KN_CARTOON_CLOSE ,DEFGIF1,SETUPFILENAME);
//    FS_WritePrivateProfileString(SN_CARTOON,KN_CARTOON_SCREEN,DEFGIF2,SETUPFILENAME);
//    FS_WritePrivateProfileString(SN_CARTOON,KN_CARTOON_CALL  ,DEFGIF3,SETUPFILENAME);
//    return TRUE;
//}
///********************************************************************
//* Function   GetCartoon  
//* Purpose     ��ö������ýṹ��Ϣ
//* Params     
//* Return     
//* Remarks     
//**********************************************************************/
//BOOL    GetCartoon(CARTOON * cartoon)
//{   
//    FS_GetPrivateProfileString(SN_CARTOON,KN_CARTOON_OPEN,DEFGIF0,//�����ؼ�����
//        cartoon->cOpen,sizeof(cartoon->cOpen),SETUPFILENAME);
//
//    FS_GetPrivateProfileString(SN_CARTOON,KN_CARTOON_CLOSE,DEFGIF1,//�ػ��ؼ�����
//        cartoon->cClose,sizeof(cartoon->cClose),SETUPFILENAME);
//
//    FS_GetPrivateProfileString(SN_CARTOON,KN_CARTOON_SCREEN,DEFGIF2,//�����ؼ�����
//        cartoon->cScreen,sizeof(cartoon->cScreen),SETUPFILENAME);
//
//    FS_GetPrivateProfileString(SN_CARTOON,KN_CARTOON_CALL,DEFGIF3,//����ؼ�����
//        cartoon->cCall,sizeof(cartoon->cCall),SETUPFILENAME);
//
//    if (strlen(cartoon->cOpen) != 0)
//    {
//        if (!FileIfExist(cartoon->cOpen))
//            strcpy(cartoon->cOpen,DEFGIF0);
//    }
//    
//    if (strlen(cartoon->cClose) != 0)
//    {
//        if (!FileIfExist(cartoon->cClose))
//            strcpy(cartoon->cClose,DEFGIF1);
//    }
//    
//    if (strlen(cartoon->cScreen) != 0)
//    {
//        if (!FileIfExist(cartoon->cScreen))
//            strcpy(cartoon->cScreen,DEFGIF2);
//    }
//    
//    if (strlen(cartoon->cCall) != 0)
//    {
//        if (!FileIfExist(cartoon->cCall))
//            strcpy(cartoon->cCall,DEFGIF3);
//    }
//    
//    return TRUE;
//}
///********************************************************************
//* Function   CreateControl  
//* Purpose    �����ؼ� 
//* Params     hwnd�����ڹ��̾��
//* Return     
//* Remarks     ��������������Ľ���Ԫ�أ�����"ȷ��"��"ȡ��"��ť��Ϊ���ڼ�ͼ�꣬���ָʾ��ͷ. 
//**********************************************************************/
//static  BOOL    CreateControl(HWND hwnd,HWND * hRadio1,HWND * hRadio2,
//                        HWND * hRadio3,HWND * hRadio4,HWND * hList,HWND * hButton)
//{
//    int icheckh = GetButtonControlHeight();
//
//    SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, // �Ҽ��˳�
//                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
//    
//    SendMessage(hwnd , PWM_CREATECAPTIONBUTTON, //���ȷ��
//                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );
//
//    SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, //����
//                MAKEWPARAM(IDC_BUTTON_START,2), BUTTONSTARTTXT);
//    
//    SendMessage(hwnd, PWM_SETAPPICON, 0, WINDOWICO);//Ϊ���ڼ�ͼ��
//
//    SendMessage( hwnd, PWM_SETSCROLLSTATE,
//                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );
//    
//    * hRadio1 = CreateWindow("BUTTON", RADIOTXT1, 
//        WS_VISIBLE | WS_CHILD | WS_TABSTOP|BS_AUTORADIOBUTTON,    
//        iRadioX, RADIO1Y, iRadioW, icheckh,
//        hwnd, (HMENU)IDC_RADIO1, NULL, NULL);
//    if ( * hRadio1 == NULL )
//        return FALSE;
//
//    * hRadio2 = CreateWindow("BUTTON", RADIOTXT2, 
//        WS_VISIBLE | WS_CHILD | WS_TABSTOP|BS_AUTORADIOBUTTON,    
//        iRadioX, RADIO2Y, iRadioW, icheckh,
//        hwnd, (HMENU)IDC_RADIO2, NULL, NULL);
//    if ( * hRadio2 == NULL )
//        return FALSE;
//
//    /** hRadio3 = CreateWindow("BUTTON", RADIOTXT3, 
//        WS_VISIBLE | WS_CHILD | WS_TABSTOP|BS_AUTORADIOBUTTON,    
//        iRadioX, RADIO3Y, iRadioW, icheckh,
//        hwnd, (HMENU)IDC_RADIO3, NULL, NULL);
//    if ( * hRadio3 == NULL )
//        return FALSE;*/
//
//    * hRadio4 = CreateWindow("BUTTON", RADIOTXT4, 
//        WS_VISIBLE | WS_CHILD | WS_TABSTOP|BS_AUTORADIOBUTTON,    
//        iRadioX, RADIO3Y, iRadioW, icheckh,
//        hwnd, (HMENU)IDC_RADIO4, NULL, NULL);
//    if ( * hRadio4 == NULL )
//        return FALSE;
//
//    SendMessage(* hRadio1,BM_SETCHECK,1,0);
//
//    * hList = CreateWindow("COMBOBOX", "",
//        CBS_HASSTRINGS|CBS_DROPDOWNLIST|WS_VISIBLE|WS_CHILD|WS_BORDER|WS_CLIPCHILDREN|WS_TABSTOP,
//        iControlX, iRectY , iControlW,100,
//        hwnd, (HMENU)IDC_LIST, NULL, NULL);
//    
//    if ( * hList == NULL )
//        return FALSE;
//
//    * hButton = CreateWindow("BUTTON",BUTTON_PREVIEW,//ȫ��Ԥ��
//        BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_BITMAP|WS_TABSTOP,
//        iControlX,120,iControlW,GetButtonControlHeight(),
//        hwnd,(HMENU)IDC_PREVIEW,hInstance,NULL);
//
//    if (* hButton == NULL)
//        return FALSE;
//    
//    return TRUE;
//}

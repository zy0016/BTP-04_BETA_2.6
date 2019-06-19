///***************************************************************************
// *
// *                      Pollex Mobile Platform
// *
// * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
// *                       All Rights Reserved 
// *
// * Module   : 
// *
// * Purpose  : 动画设置
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
//static const int    iRectX = 5;//显示动画的矩形区域的坐标和宽高
//static const int    iRectY = RADIO3Y + 30;
//static const int    iRectW = 80;
//static const int    iRectH = 80;
//static const int    iLineMaxLen = 20;
//
//static HINSTANCE    hInstance;
//
//static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//static  BOOL    CreateControl(HWND hwnd,HWND * hRadio1,HWND * hRadio2,HWND * hRadio3,HWND * hRadio4,HWND * hList,HWND * hButton);//创建控件
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
//    static      int             iSizeType;//是否是合法的尺寸,1尺寸合法,0,尺寸超大
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
//        GetImageDimensionFromFile(cfilename,&size);//获得文件尺寸
//        SetRect(&rect,iRectX,iRectY,iRectX + iRectW,iRectY + iRectH);
//
//        if ((size.cx > iRectW) || (size.cy > iRectH))//尺寸超大
//        {
//            ShowWindow(hButton,SW_SHOW);
//            iSizeType = 0;//尺寸超大
//            InvalidateRect(hWnd,&rect,TRUE);
//        }
//        else
//        {
//            iSizeType = 1;//尺寸合法
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
//    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
//        if (WA_INACTIVE != LOWORD(wParam))
//            SetFocus(hFocus);//激活后设置焦点
//        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        InvalidateRect(hWnd,NULL,TRUE);
//        break;
//
//    case WM_CLOSE://点击右上角的退出按钮时为保存退出;
//        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
//        break;
//
//    case WM_DESTROY ://通知程序管理器,此程序退出;
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
//        if ((!iSizeType) && (strlen(cfilename) != 0))//尺寸超大
//        {
//            htemp = CreateBitmapFromImageFile(hdc, cfilename,&Color,&bTran);
//            StretchBlt( hdc,
//                iRectX + 2,iRectY + 2,iRectW - 4,iRectH - 4,//目的矩形
//                (HDC)htemp,
//                0,0,size.cx,size.cy,//图象原矩形
//                SRCCOPY);//把位图从源矩形拷贝到目的矩形，扩展或压缩位图以使其适合目的矩形的范围
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
//                SetCurSel(IDC_RADIO1,cfilename);//获得当前动画文件名
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
//                SetCurSel(IDC_RADIO2,cfilename);//获得当前动画文件名
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
//                SetCurSel(IDC_RADIO3,cfilename);//获得当前动画文件名
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
//                SetCurSel(IDC_RADIO4,cfilename);//获得当前动画文件名
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
//                GetFactName(cfilename,sizeof(cfilename),hList,cartoons,1);//获得列表框中动画实际文件名
//                
//                EndAnimatedGIF(hgifanimate);
//                SendMessage(hWnd,IDC_INIT,0,0);
//                break;
//            }
//            break;
//        case IDC_BUTTON_START://启用
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
//            GetFactName(cfilename,sizeof(cfilename),hList,cartoons,1);//获得列表框中动画实际文件名
//            
//            EndAnimatedGIF(hgifanimate);
//            SendMessage(hWnd,IDC_INIT,1,0);//标志刷新全屏
//
//            break;
//        case IDC_BUTTON_SET://设置
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
//        case IDC_BUTTON_QUIT://退出
//            //DestroyWindow(hFocus);?
//            //选择"全屏预览"按钮之后,再选择本按钮,在执行DestroyWindow(hFocus)的时候
//            //死机,但是选择"全屏预览"按钮之后,再选择"设置"按钮的时候,执行
//            //DestroyWindow(hFocus)的时候不死机,这是为什么?
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
//int iCallCartoonMaxWidth(void)//来电动画的合法宽度
//{
//    return (240);
//}
//int iCallCartoonMaxHeight(void)//来电动画的合法高度
//{
//    return (120);
//}
///********************************************************************
//* Function   DisplayText  
//* Purpose     显示有关的图形,以及文字
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
//    for (i = 0;i < 3;i++)//绘制显示动画名称的四个方框
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
//    GetCartoon(&cartoon);//读取动画信息
//
//    memset(showname,0,sizeof(showname));
//    GetShowName(cartoon.cOpen,showname,cartoons);//获得开机动画显示文件名
//    DrawText(hdc,showname,strlen(showname),&rect[0],DT_HCENTER|DT_CENTER);
//
//    memset(showname,0,sizeof(showname));
//    GetShowName(cartoon.cClose,showname,cartoons);//获得关机动画显示文件名
//    DrawText(hdc,showname,strlen(showname),&rect[1],DT_HCENTER|DT_CENTER);
//
//    /*memset(showname,0,sizeof(showname));
//    GetShowName(cartoon.cScreen,showname,cartoons);//获得屏保动画显示文件名
//    DrawText(hdc,showname,strlen(showname),&rect[2],DT_HCENTER|DT_CENTER);*/
//
//    memset(showname,0,sizeof(showname));
//    GetShowName(cartoon.cCall,showname,cartoons);//获得来电动画显示文件名
//    DrawText(hdc,showname,strlen(showname),&rect[2],DT_HCENTER|DT_CENTER);
//
//    SetBkColor(hdc,oldmode);
//}
///********************************************************************
//* Function   GetShowName  
//* Purpose    遍历链表,查找动画文件名对应的显示名称
//* Params     cfilename:需要查找的动画文件名,cshowname:需要显示的名称
//* Return     是否找到
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
//* Purpose    遍历链表,查找列表框选项对应的实际文件名
//* Params     cfilename:返回动画实际文件名,buflen:缓冲区buflen,
//            hList:列表框,cartoons:链表
//            iControlType:控件类型,1为下拉列表,0为列表框
//* Return     是否找到
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
//* Purpose     选中当前单选按钮时的操作
//* Params     type:标志哪一个单选按钮,cfilename:需要返回该项目的动画文件名
//            cartoons:链表头节点
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
//* Purpose     加载ROM,FLASH中的gif文件
//* Params     CARTOONS:链表头节点
//            iControlType:控件类型,1为下拉列表,0为列表框
//            bIfShowName:TRUE将动画的实际文件名显示为动画1,动画2...
//            bIfShowName:FALSE显示动画的实际文件名(没有路径)
//* Return     
//* Remarks     
//**********************************************************************/
//BOOL    LoadCartoon(HWND hList,CARTOONS ** cartoons,int iControlType,BOOL bIfShowName)
//{
//    int     index,ides,isour;
//    char    oldpath[30] = "";
//    char    filename[CARTOONFILENAMELEN + 1] = "",cshowname[CSHOWNAMEMAXLEN + 1] = "";
//    char *  pPath[] = //需要搜索的路径
//    {
//        "/rom/",
//        "/mnt/flash/",
//        ""
//    };
//    char ** path = pPath;
//    char *  pexpend[] = //需要搜索的文件类型
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
//   // GetCurrentDirectory(oldpath,sizeof(oldpath));//保存原路径
//	getcwd(oldpath, sizeof(oldpath));
//
//    index = 1;//用于标志是动画几(动画1,动画2...),由于COMBOBOX第一项是"无",所以从1开始
//
//    p = (CARTOONS *)malloc(sizeof(CARTOONS));//添加第一个节点:包括"无"
//    if (p == NULL)
//        return FALSE;
//
//    memset(p,0x00,sizeof(CARTOONS));
//    strcpy(p->cFilename,"");        //动画名设为空
//    strcpy(p->cShowname,RADIOTXT0); //显示名称设置为"无"
//    p->next = * cartoons;
//    * cartoons = p;
//
//    SendMessage(hList,iControlType ? CB_ADDSTRING : LB_ADDSTRING,0,(LPARAM)RADIOTXT0);//添加"无"
//    //////////////////////开始在ROM,FLASH2中查找GIF动画
//    while (** path)
//    {
//		dirtemp = opendir(*path);
//
//		if (!dirtemp)
//        {
//            path++;
//            continue;
//        }
//        pex = pexpend;//扩展名
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
//                strcpy(filename,* path);        //添加路径
//                strcat(filename,dirinfo->d_name);  //添加文件名(filename包含路径和文件名)
//                //添加动画实际文件名(包含路径)
//                ides  = sizeof(p->cFilename);
//                isour = strlen(filename);
//                strncpy(p->cFilename,filename,ides > isour ? isour + 1: ides - 1);
//                //添加该动画的显示名称(动画index)
//                sprintf(cshowname,CARTOONTXT,index);
//                ides  = sizeof(p->cShowname);
//                isour = strlen(cshowname);
//                strncpy(p->cShowname,cshowname,ides > isour ? isour + 1: ides - 1);
//
//                pnext = * cartoons;
//                while (pnext->next != NULL)//遍历到链表结尾
//                    pnext = pnext->next;
//
//                pnext->next = p;//将新建的节点添加到链表结尾
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
//* Purpose     设置动画信息
//* Params     
//* Return     1:设置成功,0,设置失败,-1:设置取消
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
//    if (icur == 0)//选择"无"之后的提示,下拉列表中显示的动画名为"无"
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
//    while (p != NULL)//遍历链表查找该动画对应的实际文件名
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
//            AppMessageBox(hWnd,SIZEINVALID,TITLECAPTION,WAITTIMEOUT);//合适的动画大小为240*120
//            return -1;
//        }
//        FS_WritePrivateProfileString(SN_CARTOON,KN_CARTOON_CALL,p->cFilename,SETUPFILENAME);
//    }
//    return 1;
//}
///********************************************************************
//* Function   Restore_Cartoon  
//* Purpose     用于恢复默认设置
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
//* Purpose     获得动画设置结构信息
//* Params     
//* Return     
//* Remarks     
//**********************************************************************/
//BOOL    GetCartoon(CARTOON * cartoon)
//{   
//    FS_GetPrivateProfileString(SN_CARTOON,KN_CARTOON_OPEN,DEFGIF0,//开机关键字名
//        cartoon->cOpen,sizeof(cartoon->cOpen),SETUPFILENAME);
//
//    FS_GetPrivateProfileString(SN_CARTOON,KN_CARTOON_CLOSE,DEFGIF1,//关机关键字名
//        cartoon->cClose,sizeof(cartoon->cClose),SETUPFILENAME);
//
//    FS_GetPrivateProfileString(SN_CARTOON,KN_CARTOON_SCREEN,DEFGIF2,//屏保关键字名
//        cartoon->cScreen,sizeof(cartoon->cScreen),SETUPFILENAME);
//
//    FS_GetPrivateProfileString(SN_CARTOON,KN_CARTOON_CALL,DEFGIF3,//来电关键字名
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
//* Purpose    创建控件 
//* Params     hwnd：窗口过程句柄
//* Return     
//* Remarks     创建本届面所需的界面元素，包括"确定"，"取消"按钮，为窗口加图标，添加指示箭头. 
//**********************************************************************/
//static  BOOL    CreateControl(HWND hwnd,HWND * hRadio1,HWND * hRadio2,
//                        HWND * hRadio3,HWND * hRadio4,HWND * hList,HWND * hButton)
//{
//    int icheckh = GetButtonControlHeight();
//
//    SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, // 右键退出
//                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
//    
//    SendMessage(hwnd , PWM_CREATECAPTIONBUTTON, //左键确定
//                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );
//
//    SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, //启动
//                MAKEWPARAM(IDC_BUTTON_START,2), BUTTONSTARTTXT);
//    
//    SendMessage(hwnd, PWM_SETAPPICON, 0, WINDOWICO);//为窗口加图标
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
//    * hButton = CreateWindow("BUTTON",BUTTON_PREVIEW,//全屏预览
//        BS_PUSHBUTTON|WS_CHILD|WS_VISIBLE|WS_BITMAP|WS_TABSTOP,
//        iControlX,120,iControlW,GetButtonControlHeight(),
//        hwnd,(HMENU)IDC_PREVIEW,hInstance,NULL);
//
//    if (* hButton == NULL)
//        return FALSE;
//    
//    return TRUE;
//}

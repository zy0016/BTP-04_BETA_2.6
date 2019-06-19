/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : �ռ�鿴
 *            
\**************************************************************************/

#include    "MemoryStat.h"

static const char * pClassName      = "MemoryStatWndClass";
static const char * pSpace          = " ";
static const char * pPercent        = "%d/%d";
static const char * pSpaceK         = "%dK";
static const char * pSpaceM         = "%dM";
static const char * pPercentR       = "%d%%";
static const char * pFlash2         = "/mnt/flash/";
static const char * pFat16          = "/mnt/fat16/";
static const int    i1024           = 1024;

static const int    iListX          = 10;
static const int    iListY          = 70;

static const int    iPicRightSpace  = 40;
static const int    iRectnWidth     = 10;
static const int    iRectnHeight    = 10;
static const int    iLineNumMaxLen  = 20;
static HINSTANCE    hInstance;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hwnd,HWND * );//�����ؼ�
static  void    MemoryDisplayInfo(HDC hdc);
static  void    LoadData(HWND hList,MEMORYSTAT *);
static  void    copystring(char * des,char * sour,DWORD iused,DWORD itotal);
static  int     GetGameFileSize(void);//���������Ϸ�ļ��Ĵ�С,����k��

extern  int     GetScreenUsableWH1 (int iwh);
extern  int     GetCharactorHeight (HWND hWnd);
extern  BOOL    CountControslYaxisExt (int * piConY,unsigned int iControlH,size_t count,int iDirect,unsigned int iSpace);
extern  void    GetScreenUsableWH2 (int * pwidth,int * pheight);
//*********************************************************
BOOL    CallMemoryStatWindow(HWND hwndCall)
{
    HWND        hwnd;
    WNDCLASS    wc;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = AppWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = pClassName;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

    hwnd = CreateWindow(pClassName,TITLECAPTION, 
        WS_CAPTION|WS_BORDER |PWS_STATICBAR, 
        PLX_WIN_POSITION , hwndCall, NULL, NULL, NULL);

    if (NULL == hwnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd); 

    return (TRUE);
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT     lResult;
    HDC         hdc;
    HWND        hList;
    MEMORYSTAT  memorystat;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
        if (!CreateControl(hWnd,&hList))
        {
//            AppMessageBox(NULL,ERROR1, TITLECAPTION, WAITTIMEOUT);
            return -1;
        }
        memorystat.iUsedSMS         = 0;//GetSMSSpace();//����
        memorystat.iTotalSMS        = 0;

        memorystat.iUsedMMS         = 0;//APP_GetMMSSize();//����
        memorystat.iTotalMMS        = 0;
        
        memorystat.iUsedLinkman     = 0;//APP_GetPhoneBookFileSize();//�����ϵ���ļ��ߴ�,�ļ��ߴ�(KB)
        memorystat.iTotalLinkman    = 0;
        
        memorystat.iUsedNodepad     = 0;//GetAllTextSize() / 1024;//���±�
        memorystat.iTotalNodepad    = 0;
        
        memorystat.iUsedSteno       = 0;//GetLenthOfStenoFiles();//�ټ�
        memorystat.iTotalSteno      = 0;
        
        memorystat.iUsedCale        = 0;//GetDatebookFileSize();//�ճ�
        memorystat.iTotalCale       = 0;
        
        memorystat.iUsedPic         = 0;//GetAllImageFileSize() / 1024;//ͼƬ
        memorystat.iTotalPic        = 0;
        
        memorystat.iUsedRec         = 0;//GetRecorderFileSize();//¼����
        memorystat.iTotalRec        = 0;
        
        memorystat.iUsedGame        = GetGameFileSize();//��Ϸ
        memorystat.iTotalGame       = 0;
        
        LoadData(hList,&memorystat);

        break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_LIST));
        break;

    case WM_ACTIVATE://ȥ�������Ϣ�������Ҽ������˳��������˳�ʧ��
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(GetDlgItem(hWnd,IDC_LIST));//��������ý���

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        InvalidateRect(hWnd,NULL,TRUE);
        break;

    case WM_CLOSE://������Ͻǵ��˳���ťʱΪ�����˳�;
        SendMessage ( hWnd, WM_COMMAND, (WPARAM)IDC_BUTTON_QUIT, 0 );   
        break;

    case WM_DESTROY ://֪ͨ���������,�˳����˳�;
        UnregisterClass(pClassName,NULL);
        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;

    case WM_PAINT :
        hdc = BeginPaint( hWnd, NULL);
        MemoryDisplayInfo(hdc);
        EndPaint(hWnd, NULL);
        break;

    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_RETURN:
        case VK_F10:
            SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_QUIT,0);
            break;
        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        }
        break;
        
    case WM_COMMAND:
        switch( LOWORD( wParam ))
        {
        case IDC_BUTTON_SET://����
        case IDC_BUTTON_QUIT://�˳�
            DestroyWindow( hWnd );  
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
/********************************************************************
* Function   LoadData  
* Purpose     ��������
* Params     
* Return     
* Remarks     
**********************************************************************/
static  void    LoadData(HWND hList,MEMORYSTAT * memorystat)
{
    int i = 0;
    char * memory[] =
    {
        LISTTXT0,//"����"//(����)��
        LISTTXT1,//"����"//(������)��
        LISTTXT2,//"��ϵ��"//(��׿)��
        LISTTXT3,//"���±�"//(�)��
        LISTTXT4,//"�ټ�"//(������)��
        LISTTXT5,//"�ճ�"//(������)
        LISTTXT6,//"ͼƬ"//(�)��
        LISTTXT7,//"¼����"(������)��
        LISTTXT8,//"��Ϸ"//
        ""
    };
    char listtxt[9][30];
    memset(listtxt,0x00,sizeof(listtxt));

    copystring(listtxt[0],LISTTXT0,memorystat->iUsedSMS     ,memorystat->iTotalSMS      );
    copystring(listtxt[1],LISTTXT1,memorystat->iUsedMMS     ,memorystat->iTotalMMS      );
    copystring(listtxt[2],LISTTXT2,memorystat->iUsedLinkman ,memorystat->iTotalLinkman  );
    copystring(listtxt[3],LISTTXT3,memorystat->iUsedNodepad ,memorystat->iTotalNodepad  );
    copystring(listtxt[4],LISTTXT4,memorystat->iUsedSteno   ,memorystat->iTotalSteno    );
    copystring(listtxt[5],LISTTXT5,memorystat->iUsedCale    ,memorystat->iTotalCale     );
    copystring(listtxt[6],LISTTXT6,memorystat->iUsedPic     ,memorystat->iTotalPic      );
    copystring(listtxt[7],LISTTXT7,memorystat->iUsedRec     ,memorystat->iTotalRec      );
    copystring(listtxt[8],LISTTXT8,memorystat->iUsedGame    ,memorystat->iTotalGame     );

    while(memory[i] != NULL)
    {
        memory[i] = listtxt[i];
        i++;
    }
    SendMessage(hList,LB_RESETCONTENT,0,0);
    List_AddString(hList,memory,NULL,FALSE);
}
/********************************************************************
* Function   copystring  
* Purpose     
* Params     ���ɿռ�鿴������Ŀ�Ľ��
* Return     
* Remarks     
**********************************************************************/
static  void    copystring(char * des,char * sour,DWORD iused,DWORD itotal)
{
    int i,isour;
    char connum[10] = "";

    isour = strlen(sour);
    strcpy(des,sour);
    for (i = 0;i < iLineNumMaxLen - isour - 3;i++)
        strcat(des,pSpace);

    if (itotal != 0)
    {
        sprintf(connum,pPercent,iused,itotal);
    }
    else
    {
        if (iused < (DWORD)i1024)//С��1M
            sprintf(connum,pSpaceK,iused);
        else
            sprintf(connum,pSpaceM,iused / i1024);
    }
    strcat(des,connum);
}
/********************************************************************
* Function   MemoryDisplayInfo  
* Purpose     ��ʾͼ��
* Params     
* Return     
* Remarks     
**********************************************************************/
static  void    MemoryDisplayInfo(HDC hdc)
{
    int         ifTotal = 1,ifused = 0,imTotal = 1,imused = 0;
    //FREESPACE   freespace;
	struct  statfs * spaceinfo = NULL;
    RECT        rect1,rect2;
    HPEN        hp,oldhp;
    HBRUSH      hbr,oldhbr;
    char        cflash[10] = "",cmmc[10] = "";
    BOOL        bMMC = FALSE;
    int         iFontHeight,isw,ish,iy[4];

////////////////////////////����ռ�ʹ�����////////////////////////////////////////
    //if (GetFreeSpace(pFlash2,&freespace))
	statfs(pFlash2, spaceinfo);
	if(spaceinfo != NULL)
    {
        //ifTotal = freespace.dwTotalNumberOfClusters;
        //ifused  = ifTotal - freespace.dwNumberOfFreeClusters;
		ifTotal = spaceinfo->f_blocks;
		ifused = ifTotal - spaceinfo->f_bfree;

        if (ifused > ifTotal)
            ifused = ifTotal;

        if (0 == ifTotal)
            ifTotal = 1;
    }

	spaceinfo = NULL;

//    if (MMC_CheckCardStatus())//xlzhu
//    {
//        //if (GetFreeSpace(pFat16,&freespace))
//		statfs(pFat16, spaceinfo);
//		if(spaceinfo != NULL)
//        {
//            //imTotal = freespace.dwTotalNumberOfClusters;
//            //imused  = imTotal - freespace.dwNumberOfFreeClusters;
//            imTotal = spaceinfo->f_blocks;
//			imused = spaceinfo->f_bfree;
//
//            if (imused > imTotal)
//                imused = imTotal;
//
//            if (0 == imTotal)
//                imTotal = 1;
//
//            bMMC = TRUE;
//        }
 //   }
///////////////////////////��������///////////////////////////////////////////////
    isw         = GetScreenUsableWH1(SM_CXSCREEN);
    ish         = GetScreenUsableWH1(SM_CYSCREEN);
    iFontHeight = GetCharactorHeight(NULL);
    CountControslYaxisExt(iy,iFontHeight,sizeof(iy),-1,ish - iListY);

    SetRect(&rect1,iListX,iy[0],isw - iListX,iy[0] + iFontHeight);
    SetRect(&rect2,iListX,iy[2],isw - iListX,iy[2] + iFontHeight);

    if (bMMC)
        DrawText(hdc,MEMORYTXT0,strlen(MEMORYTXT0),&rect1,DT_LEFT);

    DrawText(hdc,MEMORYTXT1,strlen(MEMORYTXT1),&rect2,DT_LEFT);//flash�ռ�ʹ�����

///////////////////////////////��ͼ///////////////////////////////////////////
    hp      = CreatePen(PS_SOLID,0,RGB(0,255,255));
    hbr     = GetStockObject(NULL_BRUSH);
    oldhp   = SelectObject(hdc,hp);
    SelectObject(hdc,hbr);

    if (bMMC)
        RoundRect(hdc,iListX,iy[1],isw - iPicRightSpace,iy[1] + iFontHeight,
        iRectnWidth,iRectnHeight);//MMC��ʹ�����

    RoundRect(hdc,iListX,iy[3],isw - iPicRightSpace,iy[3] + iFontHeight,
        iRectnWidth,iRectnHeight);//�ռ�ʹ�����

    hbr     = CreateSolidBrush(RGB(0,255,255));
    oldhbr  = SelectObject(hdc,hbr);

    if ((bMMC) && ((isw - iPicRightSpace - iListX) * imused / imTotal > 0))
        RoundRect(hdc,iListX,iy[1],
        iListX + (isw - iPicRightSpace - iListX) * imused / imTotal,
        iy[1] + iFontHeight,iRectnWidth,iRectnHeight);

    if ((isw - iPicRightSpace - iListX) * ifused / ifTotal > 0)
        RoundRect(hdc,iListX,iy[3],
        iListX + (isw - iPicRightSpace - iListX) * ifused / ifTotal,
        iy[3] + iFontHeight,iRectnWidth,iRectnHeight);

    SelectObject(hdc,oldhp);
    SelectObject(hdc,oldhbr);
    DeleteObject(hp);
    DeleteObject(hbr);
///////////////////////////��ʾ�ٷֱ�///////////////////////////////////////////////
    sprintf(cflash  ,pPercentR,100 * ifused / ifTotal);
    sprintf(cmmc    ,pPercentR,100 * imused / imTotal);

    if (bMMC)
        TextOut(hdc,isw - 30,iy[1],cmmc,strlen(cmmc));

    TextOut(hdc,isw - 30,iy[3],cflash,strlen(cflash));
}
static  int     GetGameFileSize(void)//���������Ϸ�ļ��Ĵ�С,����k��
{
//    char		*cFilename; 
    int         hf;
    long        iFileSize;
    char *      pPaths[] = 
    {
        "/mnt/flash/game/gamerus/",
        "/mnt/flash/game/gamehrd/",
        "/mnt/flash/game/gamemj/",
        "/mnt/flash/game/poker/",
        "/mnt/flash/game/pushbox/",
        ""
    };
    char    **  path = pPaths;
	struct dirent  *dirinfo =NULL;
	DIR *dirtemp = NULL;

    iFileSize = 0;

    while (**path)
    {
        if (!(dirtemp = opendir(*path)))//����Ŀ¼
        {
            path++;//������һ��Ŀ¼
            continue;
        }

		dirinfo = readdir(dirtemp);
		
        while (dirinfo && dirinfo->d_name[0])
        {
			struct stat *buf = NULL;
            hf = open(dirinfo->d_name, O_RDONLY);
			
			buf = malloc(sizeof(struct stat));
			
			if(buf == NULL)
			{
				close(hf);
				return (int)NULL;
			}
			
			memset(buf, 0, sizeof(struct stat));
			stat(dirinfo->d_name, buf);
			
			iFileSize = buf->st_size;
			
			free(buf);
			buf = NULL;
            
            close(hf);
            
            if (!(dirinfo = readdir(dirtemp)))
                break;
        }
        closedir(dirtemp);   
        path++;//������һ��Ŀ¼
		dirtemp = NULL;
		dirinfo = NULL;
    }

    return (iFileSize / 1024);
}
/******************************************************************** 
* Function   CreateControl  
* Purpose    �����ؼ� 
* Params     hwnd�����ڹ��̾��
* Return     
* Remarks     ��������������Ľ���Ԫ�أ�����"ȷ��"��"ȡ��"��ť��Ϊ���ڼ�ͼ�꣬���ָʾ��ͷ. 
**********************************************************************/
static  BOOL    CreateControl(HWND hwnd,HWND * hList)
{
    int isw,ish;

    SendMessage(hwnd, PWM_CREATECAPTIONBUTTON, // �Ҽ��˳�
                (WPARAM)IDC_BUTTON_QUIT,BUTTONQUIT );
    
    SendMessage(hwnd , PWM_CREATECAPTIONBUTTON, //���ȷ��
                MAKEWPARAM(IDC_BUTTON_SET,1),BUTTONOK );
    
    SendMessage(hwnd, PWM_SETAPPICON, 0, WINDOWICO);//Ϊ���ڼ�ͼ��

    SendMessage( hwnd, PWM_SETSCROLLSTATE,
                SCROLLLEFT|SCROLLRIGHT|SCROLLDOWN|SCROLLUP, MASKALL );
    
    GetScreenUsableWH2(&isw,&ish);

    * hList = CreateWindow("LISTBOX", "", //��ʾ�龰ģʽ���������б���б��
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | LBS_BITMAP |WS_VSCROLL|WS_HSCROLL|WS_VSCROLL,
        iListX, iListY , isw - 2 * iListX, ish - iListY - 5,
        hwnd, (HMENU)IDC_LIST, NULL, NULL);
    
    if ( * hList == NULL )
        return FALSE;

    return TRUE;
}

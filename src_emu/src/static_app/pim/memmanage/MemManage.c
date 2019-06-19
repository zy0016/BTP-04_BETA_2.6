/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 空间查看
 *            
\**************************************************************************/
#define __MODULE__ "MEMMANAGE"
#include    "MemManage.h"
#include "PreBrowhead.h"
#include "plxdebug.h"
#include "Browser/func.h"
#include "sndmgr.h"

#define	MMMAINWC      "MemoryStatWndClass"

#define SPACEKB         "%d kB"

static	HINSTANCE    hInstance;

static	HWND	hMainWnd;
static	HWND	hFrameWnd;
static	HBITMAP		hPhoneIcon = NULL;
static	HBITMAP		hMMCIcon	= NULL;
static	HWND        hPhoneViewer;
static	HWND        hCardList = NULL;
static	HWND		hRenameWnd	=	NULL;
static	HWND		hEdit	=	NULL;

static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  LRESULT PhoneWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  LRESULT CardWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  LRESULT RenameWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static  BOOL    CreateControl(HWND hwnd,HWND * );
static	BOOL	CreateFormView(HWND	hParentWnd, HWND hFormViewer, PLISTBUF pListBuf, BOOL bPhone);
static  void    LoadData(PLISTBUF pListBuf,MemStat *,int iChoose);
static  void    copystring(char * des,const char * sour,DWORD iused);
BOOL	GetMMCName(char *name);
static	BOOL	RenameMMC();
static	BOOL SetMMCName(char *name);
static	BOOL	RegisterRenameWC(VOID *);
static BOOL		RegisteMemoryWindow();
static BOOL		CreateMemView(HWND,WNDPROC,PCSTR,PCSTR,HWND);
extern	long CALE_StatFlashFolder();
extern	long	APP_GetMsgSize();
//extern BOOL		MMC_CheckCardStatus();
extern int		Notepad_MemManage(void);
extern	BOOL	IsValidFileorFolderName(char * fName);
//*********************************************************
DWORD MemmoryManagement_AppControl(int nCode, void* pInstance, WPARAM wParam,  LPARAM lParam)
{

    DWORD dwRet;
	RECT	rc;

    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT :     
        
		hInstance = (HINSTANCE)pInstance;
		
        break;

	case APP_GETOPTION:
		switch(wParam) {
		case AS_APPWND:
			dwRet	= (DWORD)hFrameWnd;
			break;		
		}		
		break;
		
	case APP_ACTIVE:
		if (IsWindow(hFrameWnd)) 
		{
			ShowWindow(hFrameWnd,SW_SHOW);
			UpdateWindow(hFrameWnd);
			break;
		}
		else
		{
#ifdef _MODULE_DEBUG_
			StartObjectDebug();
#endif
			hFrameWnd = CreateFrameWindow(WS_VISIBLE|WS_CAPTION|PWS_STATICBAR);

			RegisteMemoryWindow();
			GetClientRect(hFrameWnd, &rc);	
			//create main window
			hMainWnd = CreateWindow(MMMAINWC,IDS_CAPTION, 
				WS_CHILD|WS_VISIBLE, 
				rc.left,
				rc.top,
				rc.right - rc.left,
				rc.bottom - rc.top,
				hFrameWnd, NULL, NULL, NULL);
			
			if (NULL == hMainWnd)
			{
				UnregisterClass(MMMAINWC,NULL);
				return FALSE;
			}
			
			//set caption、button and so on
			SetWindowText(hFrameWnd, IDS_CAPTION);
			SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_BACK,0), (LPARAM)IDS_BACK);
			SendMessage(hFrameWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(ID_OK,1), (LPARAM)"");
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
			UpdateWindow(hFrameWnd);
			ShowWindow(hFrameWnd, SW_SHOW);
			SetFocus(hMainWnd);
			
		}
		break;

	case APP_INACTIVE :
		ShowWindow(hFrameWnd, SW_HIDE);
        break;
    }

    return dwRet;
}

static BOOL    RegisteMemoryWindow()
{
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
    wc.lpszClassName = MMMAINWC;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
	return TRUE;    
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT     lResult;

    static	HWND		hMainList = NULL;
	static	HWND		hPhoneWnd = NULL;
	static	HWND		hCardWnd = NULL;
	int			index;
	char	MMCName[31] = "";

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE:
		{
			
			char * MainMem[] =
			{
				(PSTR)IDS_PHONE,//"Phone",
				(PSTR)IDS_CARD,//"Memory Card",
				""
			};
			hPhoneIcon = LoadImage(NULL, PHONEICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			hMMCIcon = LoadImage(NULL, MMCICON, IMAGE_BITMAP, 22, 16, LR_LOADFROMFILE);
			if (!CreateControl(hWnd,&hMainList))
			{
				AppMessageBox(NULL,ERROR1, IDS_CAPTION, WAITTIMEOUT);
				return -1;
			}
			SendMessage(hMainList,LB_RESETCONTENT,0,0);
			SendMessage(hMainList, LB_ADDSTRING, 0, (LPARAM)IDS_PHONE);
			SendMessage(hMainList, LB_ADDSTRING, 0, (LPARAM)IDS_CARD);
			SendMessage(hMainList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP ,0), (LPARAM)hPhoneIcon);
			SendMessage(hMainList, LB_SETIMAGE, MAKEWPARAM(IMAGE_BITMAP ,1), (LPARAM)hMMCIcon);

		
			SendMessage(hMainList, LB_SETAUXTEXT, MAKEWPARAM(0, -1), (LPARAM)"");

			if(MMC_CheckCardStatus())
			{
				GetMMCName(MMCName);
				SendMessage(hMainList, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)MMCName);
			}
			else
			{
				SendMessage(hMainList, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)ERROR2);
			}
		}
		SendMessage(hMainList, LB_SETCURSEL, 0, 0);
     
        break;

	case WM_ERASEBKGND:
		break;

	case PWM_SHOWWINDOW:
		SetWindowText(hFrameWnd, IDS_CAPTION);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)IDS_BACK);
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)"");
		SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
		if(MMC_CheckCardStatus())
		{
			GetMMCName(MMCName);
			SendMessage(hMainList, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)MMCName);
		}
		else
		{
			SendMessage(hMainList, LB_SETAUXTEXT, MAKEWPARAM(1, -1), (LPARAM)ERROR2);
		}
		//UpdateWindow(hFrameWnd);
		break;

    case WM_SETFOCUS:
        SetFocus(GetDlgItem(hWnd,IDC_LIST));
        break;

    case WM_CLOSE:
		PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
		DestroyWindow( hWnd ); 		
        break;

	case WM_DESTROY:
		if (hPhoneIcon) {			
			DeleteObject(hPhoneIcon);
			hPhoneIcon = NULL;
		}
		if (hMMCIcon) {
			DeleteObject(hMMCIcon);
			hMMCIcon = NULL;
		}
		UnregisterClass(MMMAINWC,NULL);
		DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
#ifdef _MODULE_DEBUG_
		EndObjectDebug();
#endif
		break;

	case WM_KEYDOWN:
        switch(wParam)
        {
//        case VK_RETURN://
		case VK_F5:
			index = SendMessage(hMainList,LB_GETCURSEL,0,0);
			if(0==index)
			{
				if(!CreateMemView(hPhoneWnd,PhoneWndProc,PHONEVIEWNAME,IDS_PHONEMEM,hWnd))
					return 0;
				//SetTimer(hPhoneWnd,2,5000,NULL);
			}
			else
			{
				if (!MMC_CheckCardStatus())
				{
					PLXTipsWin(hFrameWnd, hWnd, 0, ERROR2,IDS_CARD,Notify_Info,IDS_OK,NULL,WAITTIMEOUT);
					return 0;
				}
				else
				{
					//printf("memory check mmc success!");
					if(!CreateMemView(hCardWnd,CardWndProc,CARDVIEWNAME,IDS_CARD,hWnd))
					{
						PLXTipsWin(hFrameWnd, hWnd, 0, ERROR1, IDS_CARD, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
						return 0;
					}
				}
				//SetTimer(hCardWnd,1,5000,NULL);
			}
			break;
        case VK_F10://Exit
            PostMessage(hWnd,WM_CLOSE,0,0);  
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
* Purpose     加载数据
* Params     
* Return     
* Remarks     
**********************************************************************/
static  void    LoadData(PLISTBUF pListBuf, MemStat * memorystat,int iChoose)
{
    int i = 0;
//	int	ntmp;
	struct  statfs  spaceinfo ;
	char	**p;
    char * memory[] =
    {
        (PSTR)LISTTXT0,
        (PSTR)LISTTXT1,
        (PSTR)LISTTXT2,
        (PSTR)LISTTXT3,
        (PSTR)LISTTXT4,
        (PSTR)LISTTXT5,
        (PSTR)LISTTXT6,
        (PSTR)LISTTXT7,
        (PSTR)LISTTXT8,
        ""
    };
	char * cardMem[] =
	{
		(PSTR)MEMCARD,
		(PSTR)LISTTXT0,
        (PSTR)LISTTXT1,
        (PSTR)LISTTXT2,
        (PSTR)ML("Pictures:"),
        (PSTR)ML("Sounds:"),
        (PSTR)ML("Applications:"),
//        (PSTR)LISTTXT6,
//        (PSTR)LISTTXT7,
//        (PSTR)LISTTXT8,
		(PSTR)LISTTXT9,
		""
	};
    char listtxt[11][31];
    memset(listtxt,0x00,sizeof(listtxt));
	if(iChoose == PhoneMem)
	{
		// hardware [7/6/2005]
		statfs(PHONEFLASH,&spaceinfo);

		memorystat->iTotal	= spaceinfo.f_blocks * spaceinfo.f_bsize/1024;
		memorystat->iUsed	= (spaceinfo.f_blocks - spaceinfo.f_bfree)* spaceinfo.f_bsize/1024;
		memorystat->iAvailable = spaceinfo.f_bavail* spaceinfo.f_bsize/1024;

		memorystat->iContacts       = 0;
		//APP_GetContactPhoneMemory(&memorystat->iContacts);
		memorystat->iCalendar     = (float)CALE_StatFlashFolder()/1024;
		memorystat->iMessage    = (float)APP_GetMsgSize()/1024;	
    
		memorystat->iNavigator     = (float)WAP_RoomChecker()/1024;//WAP_RoomChecker(void);
		memorystat->iNodepad    = (float)Notepad_MemManage()/1024;
    
		memorystat->iImages       = (float)GetPhoneImageSpace("/mnt/flash/pictures/")/1024;

		

		copystring(listtxt[0],LISTTXT0,memorystat->iTotal	);
		copystring(listtxt[1],LISTTXT1,memorystat->iUsed	);
		copystring(listtxt[2],LISTTXT2,memorystat->iAvailable );
//		copystring(listtxt[3],LISTTXT3,memorystat->iContacts );
//		copystring(listtxt[4],LISTTXT4,memorystat->iCalendar   );
//		copystring(listtxt[5],LISTTXT5,memorystat->iMessage    );
//		copystring(listtxt[6],LISTTXT6,memorystat->iNavigator     );
//		copystring(listtxt[7],LISTTXT7,memorystat->iNodepad		);
//		copystring(listtxt[8],LISTTXT8,memorystat->iImages    );

		
		floattoa(memorystat->iContacts, listtxt[3]);
		strcat(listtxt[3], " kB");
		floattoa(memorystat->iCalendar, listtxt[4]);
		strcat(listtxt[4], " kB");
		floattoa(memorystat->iMessage, listtxt[5]);
		strcat(listtxt[5], " kB");
		floattoa(memorystat->iNavigator, listtxt[6]);
		strcat(listtxt[6], " kB");
		floattoa(memorystat->iNodepad, listtxt[7]);
		strcat(listtxt[7], " kB");
		floattoa(memorystat->iImages, listtxt[8]);
		strcat(listtxt[8], " kB");
//		SendMessage(hList,LB_RESETCONTENT,0,0);
//		List_AddString(hList,memory,&hPhoneIcon,FALSE);
//		for (i=0;i<9;i++) 
//		{
//			SendMessage(hList,LB_SETAUXTEXT,MAKEWPARAM(i, -1),(LPARAM)listtxt[i]);
//		}
		p = memory;
		for (i=0;i<9;i++)
		{
			PREBROW_AddData(pListBuf, *p++, listtxt[i]);
		}
	}
	else if(iChoose == MMCardMem)
	{
#ifndef _EMULATE_
		statfs(MMCFLASH,&spaceinfo);

		memorystat->iTotal	= spaceinfo.f_blocks * spaceinfo.f_bsize/1024;
		memorystat->iUsed	= (spaceinfo.f_blocks - spaceinfo.f_bfree)* spaceinfo.f_bsize/1024;
		memorystat->iAvailable = spaceinfo.f_bavail* spaceinfo.f_bsize/1024;
		memorystat->iImages       = (float)GetMMCImageSpace()/1024;
#else
		mkdir(MMCFLASH, 0666);
		memorystat->iTotal	= 0;
		memorystat->iUsed	= 0;
		memorystat->iAvailable = 0;
		memorystat->iImages       =0;
#endif		
		


//		memorystat->iTotal	= 0;
//		memorystat->iUsed	= 0;
//		memorystat->iAvailable = 0;

		memorystat->iContacts       = (float)SND_StatMMCFolder()/1024;	//sound
    
		memorystat->iMessage    = 0;	//application
    

    
	//	memorystat->iImages       = GetMMCImageSpace();
		memorystat->iMaps		= 0;	//


		//strcpy(listtxt[0],"pollex Memory card");
		GetMMCName(listtxt[0]);
		copystring(listtxt[1],NULL,memorystat->iTotal	);
		copystring(listtxt[2],NULL,memorystat->iUsed		);
		copystring(listtxt[3],NULL,memorystat->iAvailable );

//		copystring(listtxt[5],NULL,memorystat->iContacts );
//		copystring(listtxt[6],NULL,memorystat->iMessage    );
//		copystring(listtxt[4],NULL,memorystat->iImages    );
//		copystring(listtxt[7],NULL,memorystat->iMaps	);

		floattoa(memorystat->iContacts, listtxt[5]);
		strcat(listtxt[5], " kB");
		floattoa(memorystat->iMessage, listtxt[6]);
		strcat(listtxt[6], " kB");
		floattoa(memorystat->iMaps, listtxt[7]);
		strcat(listtxt[7], " kB");
		floattoa(memorystat->iImages, listtxt[4]);
		strcat(listtxt[4], " kB");

		SendMessage(hCardList,LB_RESETCONTENT,0,0);
		List_AddString(hCardList,cardMem,NULL,FALSE);
		for (i=0;i<8;i++) 
		{
			SendMessage(hCardList,LB_SETAUXTEXT,MAKEWPARAM(i, -1),(LPARAM)listtxt[i]);
		}
//		p = cardMem;
//		for (i=0;i<8;i++)
//		{
//			PREBROW_AddData(pListBuf, *p++, listtxt[i]);
//		}
	}
}
/********************************************************************
* Function   copystring  
* Purpose     
* Params     生成空间查看单个项目的结果
* Return     
* Remarks     
**********************************************************************/
static  void    copystring(char * des,const char * sour,DWORD iused)
{

    char connum[12] = "";

    sprintf(connum,SPACEKB,iused);

    
    strcpy(des,connum);
}

/******************************************************************** 
* Function   CreateControl  
* Purpose     
* Params     
* Return     
* Remarks     
**********************************************************************/
static  BOOL    CreateControl(HWND hwnd,HWND * hList)
{
    int isw,ish;

     
    GetScreenUsableWH2(&isw,&ish);

    * hList = CreateWindow("LISTBOX", "", 
        WS_VISIBLE | WS_CHILD | LBS_BITMAP |LBS_MULTILINE|WS_VSCROLL,
		0,0,isw,ish,
        hwnd, (HMENU)IDC_LIST, NULL, NULL);
    
    if ( * hList == NULL )
        return FALSE;

    return TRUE;
}

static	BOOL	CreateFormView(HWND	hParentWnd, HWND hFormViewer, PLISTBUF pListBuf, BOOL bPhone)
{
	RECT		rcClient;
//	HWND	hFormViewer;
//	FormView_Register();
	GetClientRect(hParentWnd, &rcClient);
	hFormViewer = CreateWindow(WC_FORMVIEW,NULL,
		WS_VISIBLE  |WS_CHILD|WS_VSCROLL,
		rcClient.left,
		rcClient.top,
		rcClient.right - rcClient.left,
		rcClient.bottom - rcClient.top,
		hParentWnd, NULL, NULL, (PVOID)pListBuf);
	if (!hFormViewer) {
		return FALSE;
	}
	if (bPhone) {
		hPhoneViewer = hFormViewer;
	}
//	else
//	{
//		hCardList = hFormViewer;
//	}
	ShowWindow(hFormViewer,SW_SHOW);
	UpdateWindow(hFormViewer);
	return	TRUE;
}

static BOOL		CreateMemView(HWND hWnd,WNDPROC wndProc,PCSTR WCLASSNAME,PCSTR szCaption,HWND hSuperWnd)
{
	WNDCLASS    wc;
	RECT		rf;

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = wndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = WCLASSNAME;

    if (!RegisterClass(&wc))
    {
        return FALSE;
    }

	GetClientRect(hFrameWnd, &rf);
    hWnd = CreateWindow(WCLASSNAME,szCaption, 
        WS_VISIBLE|WS_CHILD, 
        rf.left,
		rf.top,
		rf.right - rf.left,
		rf.bottom - rf.top,
		hFrameWnd, NULL, NULL, NULL);

    if (NULL == hWnd)
    {
        UnregisterClass(WCLASSNAME,NULL);
        return FALSE;
    }
//	SendMessage(hWnd,PWM_CREATECAPTIONBUTTON,MAKEWPARAM(ID_BACK,0),(LPARAM)IDS_BACK);
//	SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
	SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 0, (LPARAM)"");
	SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 2, (LPARAM)"");
	SetWindowText(hFrameWnd, szCaption);
	UpdateWindow(hFrameWnd); 
    ShowWindow(hFrameWnd, SW_SHOW);
	SetFocus(hWnd);
    

    return (TRUE);
}
static  LRESULT PhoneWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT     lResult;
//    HDC         hdc;

    MemStat		memorystat;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			LISTBUF	ListBuf;
			PREBROW_InitListBuf(&ListBuf);
			LoadData(&ListBuf,&memorystat,PhoneMem);
			if (!CreateFormView(hWnd, hPhoneViewer, &ListBuf, TRUE))
			{
				AppMessageBox(NULL,ERROR1, IDS_CAPTION, WAITTIMEOUT);
				return -1;
			}
			SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
//			LoadData(hPhoneViewer,&memorystat,PhoneMem);
		}
        break;

	case PWM_SHOWWINDOW:
		SetWindowText(hFrameWnd, IDS_PHONEMEM);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SetFocus(hPhoneViewer);
		UpdateWindow(hFrameWnd); 		
		break;
		/*
	case WM_TIMER:
		SendMessage(hPhoneViewer,LB_RESETCONTENT,0,0);
		LoadData(hPhoneViewer,&memorystat,PhoneMem);
		break;
*/
    case WM_SETFOCUS:
        SetFocus(hPhoneViewer);
        break;
/*
    case WM_ACTIVATE://
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hPhoneViewer);//

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        InvalidateRect(hWnd,NULL,TRUE);
        break;
*/
    case WM_CLOSE://
//		KillTimer(hWnd,2);
		DestroyWindow( hWnd );  
		UnregisterClass(PHONEVIEWNAME,NULL);
        break;
/*
    case WM_DESTROY ://notified programman that this application will exit
        UnregisterClass(MMMAINWC,NULL);
 //       DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;
*/
    case WM_KEYDOWN:
        switch( LOWORD( wParam ))
        {
        case VK_RETURN:
			PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
            PostMessage(hWnd,WM_CLOSE,0,0);
			
            break;
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}
static  LRESULT CardWndProc( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT     lResult;
    MemStat		memorystat;
	int			nCurSel;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			//LISTBUF	ListBuf;
			RECT	rClient;
			//PREBROW_InitListBuf(&ListBuf);
			//LoadData(&ListBuf,&memorystat,MMCardMem);
			GetClientRect(hWnd, &rClient);
			hCardList  = CreateWindow("LISTBOX",NULL,
				WS_CHILD | WS_VISIBLE|LBS_MULTILINE|WS_VSCROLL,
				0, 0,
				rClient.right,
				rClient.bottom,
				hWnd, NULL, NULL,NULL);
			/*
			if (!CreateFormView(hWnd, hCardList, &ListBuf, FALSE))
			{
				AppMessageBox(NULL,ERROR1, IDS_CAPTION, WAITTIMEOUT);
				return -1;
			}
			*/
			SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
			LoadData(NULL,&memorystat,MMCardMem);
		}
        break;

	case WM_COMMAND:
		switch(HIWORD(wParam)) 
		{
		case LBN_SELCHANGE:
			{
				nCurSel = SendMessage(hCardList, LB_GETCURSEL, 0, 0);
				if (nCurSel == 0)
				{
					SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
				}
				else
					SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
			}
			break;

		case LBN_SETFONT:
			{
				HFONT	hSmall;
				GetFontHandle(&hSmall, (lParam == 0)?SMALL_FONT:LARGE_FONT);
				return	(LONG)hSmall;
			}
			break;

		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}

		/*
		if (HIWORD(wParam) == LBN_SELCHANGE) 
		{
			nCurSel = SendMessage(hCardList, LB_GETCURSEL, 0, 0);
			if (nCurSel == 0)
			{
				SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
			}
			else
				SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
		}
		else
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			*/
		break;
		
	case PWM_SHOWWINDOW:
		nCurSel = SendMessage(hCardList, LB_GETCURSEL, 0, 0);
		if (nCurSel == 0)
		{
			SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
		}
		else
			SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_OK);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 0, (LPARAM)"");
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SetWindowText(hFrameWnd, IDS_CARD);
		SetFocus(hCardList);
		//UpdateWindow(hFrameWnd); 		
		break;

//	case WM_TIMER:
//		SendMessage(hCardList,LB_RESETCONTENT,0,0);
//		LoadData(hCardList,&memorystat,MMCardMem);
//		break;

    case WM_SETFOCUS:
        SetFocus(hCardList);
        break;
/*
    case WM_ACTIVATE://
        if (WA_INACTIVE != LOWORD(wParam))
            SetFocus(hCardList);//

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        InvalidateRect(hWnd,NULL,TRUE);
        break;
*/
    case WM_CLOSE://
//		KillTimer(hWnd,1);
		DestroyWindow( hWnd );  
		UnregisterClass(CARDVIEWNAME,NULL);
        break;
/*
    case WM_DESTROY ://notified programman that this application will exit
        UnregisterClass(MMMAINWC,NULL);
 //       DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
        break;
*/
    case WM_KEYDOWN:
        switch( LOWORD( wParam ))
        {
        case VK_RETURN:
			nCurSel = SendMessage(hCardList, LB_GETCURSEL, 0, 0);
			if (nCurSel == 0)
			{
				//SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 1, (LPARAM)IDS_EDIT);
				RenameMMC();
			}
			else
			{	
				PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
				PostMessage(hWnd,WM_CLOSE,0,0); 
			}
            break;
	
        }
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

BOOL GetMMCName(char *name)
{
	FILE	*fp;
	if (!name) {
		return FALSE;
	}
	*name = '\0';
//	strcpy(name, "Memory card");
	if((fp = fopen(MMCNAMEPATH, "r")) == NULL)
	{
		fp = fopen(MMCNAMEPATH, "w+");
		if (!fp) {
			return FALSE;
		}
		if(fwrite("Memory card", strlen("Memory card"), 1, fp) == 1)
		{
			strcpy(name, "Memory card");
			fclose(fp);
			return TRUE;
		}
		fclose(fp);
		FALSE;
	}
	else
	{
		fread(name, 30, 1, fp);
		if (*name == '\0')
		{
			strcpy(name, "Memory card");
		}

		fwrite("Memory card", strlen("Memory card"), 1, fp);

		fclose(fp);
	}
	return TRUE;
}

static	BOOL SetMMCName(char *name)
{
	FILE	*fp;
	
	fp = fopen(MMCNAMEPATH, "w+");
	if (!fp) {
		return FALSE;
	}
	if(fwrite(name, strlen(name), 1, fp) == 1)
	{
		fclose(fp);
		return TRUE;
	}
	fclose(fp);
	return FALSE;

}

static	BOOL	RegisterRenameWC(void* p)
{
	WNDCLASS    wc;
	
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = RenameWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = RENAMEWNDCLASS;
	
    if (!RegisterClass(&wc))
    {
        return FALSE;
    }
	return TRUE; 
}
static	BOOL	RenameMMC()
{
	RECT	rfc;
	RegisterRenameWC(NULL);

	GetClientRect(hFrameWnd, &rfc);

	hRenameWnd = CreateWindow(RENAMEWNDCLASS,
		NULL,
		WS_CHILD|WS_VISIBLE,
		0,0,
		rfc.right,
		rfc.bottom,
		hFrameWnd,
		NULL,NULL,NULL);

	if (!hRenameWnd) {
		return FALSE;
	}
	ShowWindow(hFrameWnd, SW_SHOW);
	SetFocus(hRenameWnd);
	return	TRUE;
}
static  LRESULT RenameWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	LRESULT	lResult;
	IMEEDIT	ie;
	RECT	rcClient;
	int		txtlen;
	char	buf[31] = "";
	switch(wMsgCmd) 
	{
	case WM_CREATE:
		{			
			memset(&ie, 0, sizeof(IMEEDIT));
			ie.hwndNotify	= hWnd ;    
			ie.dwAttrib	    = 0;                
			ie.dwAscTextMax	= 0;
			ie.dwUniTextMax	= 0;
			ie.wPageMax	    = 0;        
			ie.pszCharSet	= NULL;
			ie.pszTitle	    = NULL;
			ie.pszImeName	= NULL;
			
			GetClientRect(hWnd,&rcClient);
			hEdit	=	CreateWindow("IMEEDIT",NULL,
				WS_VISIBLE| WS_CHILD|ES_TITLE|WS_TABSTOP|ES_AUTOHSCROLL,
				rcClient.left,
				rcClient.top,
				rcClient.right - rcClient.left,
				52,//rcClient.bottom - rcClient.top,
				hWnd,NULL,NULL,(PVOID)&ie);
			SendMessage(hEdit,EM_LIMITTEXT,30,0);
			GetMMCName(buf);
			SendMessage(hEdit, WM_SETTEXT, 0, (LPARAM)buf);
			SendMessage(hEdit,EM_SETTITLE,0,(LPARAM)MEMCARD);
			SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 1, (LPARAM)ML("Save"));
			SetWindowText(hFrameWnd, buf);
			//SendMessage(hFrameWnd, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Clear"));
		}
		break;

	case PWM_SHOWWINDOW:
		GetWindowText(hEdit, buf, 30);
		SetWindowText( hFrameWnd,buf);
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT,  1,(LPARAM)IDS_SAVE);
		txtlen = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
		if (txtlen <= 0) {
			SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 0,(LPARAM)IDS_CANCEL);
		}		
		SendMessage(hFrameWnd,PWM_SETBUTTONTEXT, 2,(LPARAM)"");
		UpdateWindow(hFrameWnd);
		SetFocus(hEdit);
		break;

	case WM_COMMAND:
		if (HIWORD(wParam) == EN_CHANGE) {
			GetWindowText(hEdit, buf, 30);
			SetWindowText(hFrameWnd, buf);
		}
		else
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
		
	case WM_SETFOCUS:
		SetFocus(hEdit);		
		SendMessage(hEdit,EM_SETSEL, -1, -1);
		break;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		UnregisterClass(RENAMEWNDCLASS,NULL);
		break;

	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_RETURN:
			{
				txtlen = SendMessage(hEdit, WM_GETTEXTLENGTH, 0, 0);
				GetWindowText(hEdit, buf, 31);
				if ((txtlen <=0 ) ||(!IsValidFileorFolderName(buf)))
				{
					PLXTipsWin(hFrameWnd, hWnd, 0, IDS_DEFINENAME, NULL, Notify_Alert, IDS_OK, NULL, WAITTIMEOUT);
					//break;
				}
				else
				{
					//SendMessage(hEdit, WM_GETTEXT, 0, (LPARAM)buf);
					//GetWindowText(hEdit, buf, 31);
					if (SetMMCName(buf)) {
						SendMessage(hCardList,LB_SETAUXTEXT,MAKEWPARAM(0, -1),(LPARAM)buf);
					}
					
					PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
					PostMessage(hWnd, WM_CLOSE, 0, 0);
				}
			}
			break;
			
		case VK_F10:
			PostMessage(hFrameWnd, PWM_CLOSEWINDOW, (WPARAM)hWnd, 0);
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			break;

		default:
			break;
		}
		break;

	default:
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}
	return lResult;
}

    /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : TestApp
 *
 * Purpose  : ME demo
 *            
\**************************************************************************/
 
#include "window.h"
#include "winpda.h"
#include "malloc.h"
#include "string.h"
#include "stdio.h"
#include "plx_pdaex.h"
//#include "fapi.h"
#include "plxdebug.h"
#include "TestApp.h"
#include "pmalarm.h"
#include "windebug.h"
#include <fcntl.h>
#include <hopen/info.h>
#include <pm/progman.h>
#include <project/imesys.h>
#include <hopen/ipmc.h>
#include <project/pmi.h>

#define WINDOW1BUTTON_NUM	5

static  HWND hwndApp;
static  HWND hwndEdit1;
static  HWND hwndEdit2;
static  HWND hwndStatic1;
static  HWND hwndStatic2;
static	HWND hwnd1Button[WINDOW1BUTTON_NUM];
static  HMENU hMenuTestApp;
static	HMENU hMenuWindow1;
static	HMENU hMenuWindow2;
static  int   iDebugEnableFlag;
static	int iPdaSvrStart;
static	int iWindowFlag=IDM_WINDOW0;
static void SetCurRect(HDC hdc, RECT *curRect, int ntype);
static void	TestApp_InitMenu(void);

extern void SetUart0WriteMonitor( BOOL x );
extern void SetUart0ReadMonitor( BOOL x );
extern BOOL GetUart0WriteMonitor( void );
extern BOOL GetUart0ReadMonitor( void );

#ifdef TCPIP_DATA_RECORD

#define	BUF_LONGTH	512
static	BOOL	bRecordingFlag = FALSE;
static	DWORD	SendPointer = 0;
static	DWORD	GetPointer = 0;
static	char	Tcpip_Data_Buf_Send [ BUF_LONGTH ] ;	// 记录发送的数据
static	char	Tcpip_Data_Buf_Get [ BUF_LONGTH ]  ;	// 记录接收的数据
DWORD	TestApp_WriteToSendBuf( unsigned char * pChar, unsigned long uBytes );
DWORD	TestApp_WriteToGetBuf( unsigned char * pChar, unsigned long uBytes );
unsigned int ppp_decode_fail_counter =0;	// 统计decode failed数据包的数量
#endif

//文件长度
#define  FILESIZE   100000
//内存长度
#define  MEMSIZE     1024
//画线条数
#define  LINENUM     5000

DWORD TESTAPP_AppControl(int nCode, void* pInstance, WPARAM wParam, LPARAM lParam)
{
	DWORD		    dwRet;

    dwRet = TRUE;

    switch (nCode)
    {
    case APP_INIT:
        break;

   case APP_GETOPTION:
		break;

	case APP_SETOPTION:
		break;

    case APP_ACTIVE:
       break;

    case APP_INACTIVE:
        break;

	case APP_ALARM:
		{
			SYSTEMTIME	systime;
			GetLocalTime(&systime);
			printf("Get Alarm Information at:\r\n");
			printf("Year: %4d\r\n",systime.wYear);
			printf("Month:%4d\r\n",systime.wMonth);
			printf("Day:  %4d\r\n",systime.wDay);
			printf("Hour: %4d\r\n",systime.wHour);
			printf("Minute:%4d\r\n",systime.wMinute);
			printf("Second:%4d\r\n",systime.wSecond);
		}
		break;

    default:
        break;
    }

    return dwRet;
}
/*-------------------------------------------------
 *
 *  Function :	TESTAPP_AppControl
 *  Purpose  :	Application control function.
 *
 *-----------------------------------------------*/

void TESTAPP_EngineeringMode(void)
{
	WNDCLASS wc;
	static	int class_init=0;

	if ( !class_init )
    {
		// Register window class
        wc.style         = 0;
        wc.lpfnWndProc   = AppWndProc;
        wc.cbClsExtra    = 0;
        wc.cbWndExtra    = 0;
        wc.hInstance     = NULL;
        wc.hIcon         = NULL;
        wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = GetStockObject(WHITE_BRUSH);
        wc.lpszMenuName  = NULL;
        wc.lpszClassName  = "TestAppClass";
        
        if (!RegisterClass(&wc))
            printf("\r\nEngineering Mode: register class error!\r\n");

		class_init = 1;
	}

    if (hwndApp)
    {
        ShowWindow(hwndApp, SW_SHOW);
		BringWindowToTop(hwndApp);
    }
    else
    {
		hMenuTestApp = LoadMenuIndirect((PMENUTEMPLATE)&TestMenuTemplate);
		hMenuWindow1 = LoadMenuIndirect((PMENUTEMPLATE)&TestMenuTemplate_);
		hMenuWindow2 = LoadMenuIndirect((PMENUTEMPLATE)&TestMenuTemplate__);
        // Creates window
        hwndApp = CreateWindow(
            "TestAppClass", 
            "memory check", 
            WS_CAPTION | WS_CLIPCHILDREN | PWS_STATICBAR, 
            PLX_WIN_POSITION, 
            NULL, 
            (HMENU)hMenuTestApp,
            NULL, NULL
            );

		if ( !hwndApp )
		{
			DestroyMenu(hMenuTestApp);
			DestroyMenu(hMenuWindow1);
			DestroyMenu(hMenuWindow2);
		}

        ShowWindow(hwndApp, SW_SHOW);
        UpdateWindow(hwndApp);
    }
}

static LRESULT CALLBACK AppWndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam, 
                                   LPARAM lParam)
{
    HDC         hdc;
	IMEEDIT		ie;

    LONG lResult = (LRESULT)TRUE;
    
    switch (wMsgCmd)
    {
    case WM_CREATE :
		
        SendMessage(hWnd, PWM_CREATECAPTIONBUTTON, 
                (WPARAM)IDC_TEST_QUIT, (LPARAM)"Exit");
		SendMessage(hWnd, PWM_SETBUTTONTEXT, 2, (LPARAM)ML("Options"));
		TestApp_InitMenu();	// 初始化菜单
		SetTimer(hWnd,1,1000,NULL);	// 设置 Timer 实时刷新窗口

		break;

	case WM_INITMENU:
		TestApp_InitMenu();	// 初始化菜单
		break;

	case WM_KEYDOWN:
		switch(wParam) 
		{
		case VK_RETURN:			// left softkey
			{
				HWND hFocus;

				if ( IDM_WINDOW1 != iWindowFlag )
					return lResult;
				
				hFocus = GetFocus();

				if ( hFocus == hwnd1Button[0] )
					printf("Button1\r\n");
				else if ( hFocus == hwnd1Button[1] )
					printf("Button2\r\n");
				else if  ( hFocus == hwnd1Button[2] )
					printf("Button3\r\n");
				else if  ( hFocus == hwnd1Button[3] )
					printf("Button4\r\n");
				else if  ( hFocus == hwnd1Button[4] )
					printf("Button5\r\n");
			}
			break;
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

    case WM_COMMAND :
		switch ( LOWORD(wParam) )
		{
        case IDC_TEST_QUIT:
            DestroyWindow(hWnd);
            break;
		case IDM_WATCHDOG:
			{
				if (GetWatchDogState())
				{
					// now watchdog is enable, we will disable it
					DisableWatchDog();
				}
				else
				{
					EnableWatchDog();
				}
			}
			break;
		case IDM_BEGINDBG:
			//DBG_DumpMemObject();
			StartObjectDebug();	// 打开资源监视
		//	EnableMenuItem(hMenuTestApp, IDM_BEGINDBG,MF_BYCOMMAND|MF_GRAYED);
			EnableMenuItem(hMenuTestApp, IDM_SHOWDBG,MF_BYCOMMAND|MF_ENABLED);
			EnableMenuItem(hMenuTestApp, IDM_ENDDBG,MF_BYCOMMAND|MF_ENABLED);
			iDebugEnableFlag = 1;
			break;
		case IDM_SHOWDBG:
			DumpObjectLeaks();	// 显示没有释放的资源
			break;
		case IDM_ENDDBG:
			EndObjectDebug();	// 停止资源监视
			EnableMenuItem(hMenuTestApp, IDM_BEGINDBG,MF_BYCOMMAND|MF_ENABLED);
			EnableMenuItem(hMenuTestApp, IDM_SHOWDBG,MF_BYCOMMAND|MF_GRAYED);
			EnableMenuItem(hMenuTestApp, IDM_ENDDBG,MF_BYCOMMAND|MF_GRAYED);
			iDebugEnableFlag = 0;
			break;
		case IDM_PDASYNCH:
			if ( 0==iPdaSvrStart )
			{
				if ( !PdaSvr_Open(1) )	// open pda synch with usb 
					printf("\r\nTESTAPP Engineer: Start PDA Server error!\r\n");
				else
					iPdaSvrStart = 1;
			}
			else
			{
				PdaSvr_Close();
				iPdaSvrStart = 0;
			}
			break;
		case IDM_PMALARM:
			pmalarm_debug();
			break;
		case IDM_SETALARM:
			{
				int Ret;
				RTCTIME	rtctime;
				char tmpdate[20];
				char tmptime[20];
				char tmp[20];

				if (hwndEdit1)
				{
					Ret = GetWindowText(hwndEdit1,tmpdate,8+1);
					if ( 8!=Ret )
					{
						printf("Please input date!\r\n");
						break;
					}
				}
				else
				{
					printf("error create edit1!\r\n");
					break;
				}

				if (hwndEdit2)
				{
					Ret = GetWindowText(hwndEdit2,tmptime,6+1);
					if ( 6!=Ret )
					{
						printf("Please input time!\r\n");
						break;
					}
				}
				else
				{
					printf("error create edit2!\r\n");
					break;
				}

				memcpy(tmp,tmpdate,4);	tmp[4]='\0';	// Year
				rtctime.v_nYear = (short)atoi(tmp);
				memcpy(tmp,tmpdate+4,2); tmp[2]='\0';	// Month
				rtctime.v_nMonth = (char)atoi(tmp);
				memcpy(tmp,tmpdate+6,2); tmp[2]='\0';	// Day
				rtctime.v_nDay = (char)atoi(tmp);
				memcpy(tmp,tmptime,2); tmp[2]='\0';		// Hour
				rtctime.v_nHour = (char)atoi(tmp);
				memcpy(tmp,tmptime+2,2); tmp[2]='\0';		// Minute
				rtctime.v_nMinute = (char)atoi(tmp);
				memcpy(tmp,tmptime+4,2); tmp[2]='\0';		// Second
				rtctime.v_nSecond = (char)atoi(tmp);

				rtctime.v_nDayOfWeek = '\0';

				Ret = RTC_SetAlarms("LocationModule",1,ALMF_REPLACE|ALMF_POWEROFF,"LocationModule",&rtctime);
				if ( !Ret )
					printf("Set Alarm 1 error!\r\n");

				rtctime.v_nMinute += 3;

				Ret = RTC_SetAlarms("LocationModule",2,ALMF_REPLACE|ALMF_POWEROFF,"LocationModule",&rtctime);
				if ( !Ret )
					printf("Set Alarm 2 error!\r\n");
			}
			break;
		case IDM_SETSLEEPINTERVAL:
			{
				int Ret;
				int sleepinterval;
				int fd;
				char tmpinterval[20];

				if (hwndEdit1)
				{
					Ret = GetWindowTextLength(hwndEdit1);
					if ( 0==Ret || 20<=Ret )
					{
						printf("Please input correct interval seconds!\r\n");
						break;
					}
					else
					{
						GetWindowText(hwndEdit1,tmpinterval,Ret+1);
						sleepinterval = atoi(tmpinterval);
						fd = open("/dev/pmu",O_RDONLY);
						if ( 0>fd )
						{
							printf("Open pmu failed!\r\n");
							return lResult;
						}
						ioctl(fd,SET_SLEEP_INTERVAL,sleepinterval);
						close(fd);
						printf("Set sleep interval ok.\r\n");
					}
				}
				else
				{
					printf("error create edit1!\r\n");
					break;
				}
			}
			break;
		case IDM_WINDOW0:
			{
				int i;
				iWindowFlag = IDM_WINDOW0;
				PDASetMenu(hWnd,hMenuTestApp);

				if ( NULL!=hwnd1Button[0] )
				{
					for ( i=0;i<WINDOW1BUTTON_NUM;i++ )
					{
						DestroyWindow(hwnd1Button[i]);
						hwnd1Button[i] = NULL;
					}
				}
				if ( hwndEdit1 && hwndEdit2 )
				{
					DestroyWindow(hwndEdit1);
					DestroyWindow(hwndEdit2);
					DestroyWindow(hwndStatic1);
					DestroyWindow(hwndStatic2);
					hwndEdit1 = NULL;
					hwndEdit2 = NULL;
					hwndStatic1 = NULL;
					hwndStatic2 = NULL;
				}
			}
			break;
		case IDM_WINDOW1:
			{
				int i;
				iWindowFlag = IDM_WINDOW1;
				PDASetMenu(hWnd,hMenuWindow1);
				for ( i=0;i<WINDOW1BUTTON_NUM;i++ )
					hwnd1Button[i] = CreateWindow("BUTTON","",WS_CHILD|WS_VISIBLE|WS_TABSTOP|WS_BORDER|BS_PUSHBUTTON,
												20,10+i*25,100,20,hWnd,(HMENU)(IDC_BUTTON_1+i),NULL,NULL);
				SetWindowText(hwnd1Button[0],ML("Button1"));
				SetWindowText(hwnd1Button[1],ML("Button2"));
				SetWindowText(hwnd1Button[2],ML("Button3"));
				SetWindowText(hwnd1Button[3],ML("Button4"));
				SetWindowText(hwnd1Button[4],ML("Button5"));
				SetFocus(hwnd1Button[0]);
			}
			break;
		case IDM_WINDOW2:
			{
				iWindowFlag = IDM_WINDOW2;
				PDASetMenu(hWnd,hMenuWindow2);

				memset(&ie, 0, sizeof(IMEEDIT));
				ie.hwndNotify	= (HWND)hWnd;    // 可以指定为应用程序主窗口句柄
				ie.dwAttrib	= 0;
				ie.dwAscTextMax	= 0;
				ie.dwUniTextMax	= 0;
				ie.wPageMax	= 0;
				ie.pszImeName	= "号码";           // 指定使用"号码"输入法
				ie.pszCharSet	= "0123456789";     // 指定仅可输入数字
				ie.pszTitle	= NULL;
				ie.uMsgSetText	= 0;

				hwndEdit1 = CreateWindow("IMEEDIT","",WS_BORDER|WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,
					5,40,130,26,hWnd,NULL,NULL,(PVOID)&ie);
				hwndEdit2 = CreateWindow("IMEEDIT","",WS_BORDER|WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_AUTOHSCROLL,
					5,100,130,26,hWnd,NULL,NULL,(PVOID)&ie);
				hwndStatic1 = CreateWindow("STATIC","Date/Sleep Interval:",WS_BORDER|WS_CHILD|WS_VISIBLE,
					5,10,170,26,hWnd,NULL,NULL,NULL);
				hwndStatic2 = CreateWindow("STATIC","Time:",WS_BORDER|WS_CHILD|WS_VISIBLE,
					5,70,50,26,hWnd,NULL,NULL,NULL);

				SetFocus(hwndEdit1);
			}
			break;
		default:
			break;
		}
		break;

    case WM_PAINT:
	{
		int fhandle;
		HFONT hfontold;
		HFONT smallfont=NULL;
		int local_free_space = 0;
		char z[100];
		WINDEBUGINFO WinObjDebugInfo;
		RECT rectTestApp;

		HDC hMemDC;
		HBITMAP hCompatibleBmp;

		if ( iWindowFlag != IDM_WINDOW0 )	// 只有第一页显示Paint的信息
		{
			hdc = BeginPaint(hWnd, NULL);
			EndPaint(hWnd, NULL);
			return TRUE;
		}

		if ( !GetFontHandle(&smallfont,SMALL_FONT) )
			printf("Get small font error!\r\n");

		fhandle = open("/dev/info",O_RDONLY);

		if ( -1==fhandle )
		{
			printf("open info error.\r\n");
		}
		else
		{
			ioctl(fhandle,INFO_IOC_GETMEMMFREE,&local_free_space);
			close(fhandle);
		}

        hdc = BeginPaint(hWnd, NULL);

		hMemDC = CreateCompatibleDC(hdc);

		if ( smallfont )
			hfontold = SelectObject(hMemDC, smallfont);

		hCompatibleBmp = CreateCompatibleBitmap(hMemDC, 176,167);
		SelectObject(hMemDC,hCompatibleBmp);
		GetClientRect(hWnd,&rectTestApp);
		ClearRect(hMemDC,&rectTestApp,RGB(255,255,255));

#define LOCATION_X	5
#define LOCATION_Y	5
#define LOCATION_X_STEP  83
#define LOCATION_Y_STEP  15

		GetWinDebugInfo(&WinObjDebugInfo);
//		local_free_space = GetFreeMemory();
		memset(z,0,100);
		sprintf(z,"freespace: %d",local_free_space);
		TextOut( hMemDC, LOCATION_X, LOCATION_Y,z,-1);
		
		sprintf(z,"Bitmap: %d",WinObjDebugInfo.nBitmapNum);
		TextOut( hMemDC, LOCATION_X, LOCATION_Y+LOCATION_Y_STEP*1,z,-1);
		sprintf(z,"Brush: %d",WinObjDebugInfo.nBrushNum);
		TextOut( hMemDC, LOCATION_X+LOCATION_X_STEP, LOCATION_Y+LOCATION_Y_STEP*1,z,-1);

		sprintf(z,"Cursor: %d",WinObjDebugInfo.nCursorNum);
		TextOut( hMemDC, LOCATION_X, LOCATION_Y+LOCATION_Y_STEP*2,z,-1);
		sprintf(z,"Icon: %d",WinObjDebugInfo.nIconNum);
		TextOut( hMemDC, LOCATION_X+LOCATION_X_STEP, LOCATION_Y+LOCATION_Y_STEP*2,z,-1);

		sprintf(z,"Menu: %d",WinObjDebugInfo.nMenuNum);
		TextOut( hMemDC, LOCATION_X, LOCATION_Y+LOCATION_Y_STEP*3,z,-1);
		sprintf(z,"Window: %d",WinObjDebugInfo.nWindowNum);
		TextOut( hMemDC, LOCATION_X+LOCATION_X_STEP, LOCATION_Y+LOCATION_Y_STEP*3,z,-1);
		
		sprintf(z,"Pen: %d",WinObjDebugInfo.nPenNum);
		TextOut( hMemDC, LOCATION_X, LOCATION_Y+LOCATION_Y_STEP*4,z,-1);
		sprintf(z," DC: %d",WinObjDebugInfo.nDCNum);
		TextOut( hMemDC, LOCATION_X+LOCATION_X_STEP, LOCATION_Y+LOCATION_Y_STEP*4,z,-1);

		sprintf(z,"Font: %d",WinObjDebugInfo.nFontNum);
		TextOut( hMemDC, LOCATION_X, LOCATION_Y+LOCATION_Y_STEP*5,z,-1);
		sprintf(z,"Palette: %d",WinObjDebugInfo.nPaletteNum);
		TextOut( hMemDC, LOCATION_X+LOCATION_X_STEP, LOCATION_Y+LOCATION_Y_STEP*5,z,-1);

		sprintf(z,"GDI object: %d",WinObjDebugInfo.nGDIObjNum);
		TextOut( hMemDC, LOCATION_X, LOCATION_Y+LOCATION_Y_STEP*6,z,-1);
		sprintf(z,"User object: %d",WinObjDebugInfo.nUserObjNum);
		TextOut( hMemDC, LOCATION_X, LOCATION_Y+LOCATION_Y_STEP*7,z,-1);

		BitBlt(hdc,0,0,176,167,hMemDC,0,0,ROP_SRC);

		if ( smallfont )
			SelectObject(hMemDC, hfontold);
		
        EndPaint(hWnd, NULL);
		DeleteDC(hMemDC);
		DeleteObject(hCompatibleBmp);
        break;
	}
        
	case WM_TIMER:
		InvalidateRect(hWnd,NULL,FALSE);
		break;
    
    case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

	case WM_DESTROY : 
		KillTimer(hWnd,1);
        hwndApp = NULL;
		hwndEdit1 = NULL;
		hwndEdit2 = NULL;
		hwndStatic1 = NULL;
		hwndStatic2 = NULL;
		hwnd1Button[0] = NULL;
		hwnd1Button[1] = NULL;
		hwnd1Button[2] = NULL;
		hwnd1Button[3] = NULL;
		hwnd1Button[4] = NULL;
		DestroyMenu(hMenuTestApp);
		DestroyMenu(hMenuWindow1);
		DestroyMenu(hMenuWindow2);
		hMenuTestApp = NULL;
		hMenuWindow1 = NULL;
		hMenuWindow2 = NULL;
#ifdef  _STARTEL
        DlmNotify((WPARAM)PES_APPQUIT, (LPARAM)"S:测试");
#else
//	    DlmNotify((int)PES_STCQUIT, (int)TESTAPP_AppControl);
#endif
        break;

    default :
        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    
    return lResult;
}


/* 初始化程序的菜单 
 *
 */
static void	TestApp_InitMenu(void)
{
	if ( IDM_WINDOW0 != iWindowFlag )
	{
		if ( IDM_WINDOW1 == iWindowFlag )
		{
			if ( GetWatchDogState() )
				ModifyMenu(hMenuWindow1, IDM_WATCHDOG, MF_BYCOMMAND, IDM_WATCHDOG, (LPCSTR)"Disable WatchDog");
			else
				ModifyMenu(hMenuWindow1, IDM_WATCHDOG, MF_BYCOMMAND, IDM_WATCHDOG, (LPCSTR)"Enable WatchDog");
		}
		return;
	}

	if ( 0 == iDebugEnableFlag )
	{
		EnableMenuItem(hMenuTestApp, IDM_BEGINDBG,MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(hMenuTestApp, IDM_SHOWDBG,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hMenuTestApp, IDM_ENDDBG,MF_BYCOMMAND|MF_GRAYED);
	}
	else
	{
		EnableMenuItem(hMenuTestApp, IDM_BEGINDBG,MF_BYCOMMAND|MF_GRAYED);
		EnableMenuItem(hMenuTestApp, IDM_SHOWDBG,MF_BYCOMMAND|MF_ENABLED);
		EnableMenuItem(hMenuTestApp, IDM_ENDDBG,MF_BYCOMMAND|MF_ENABLED);
	}

	if ( 0 == iPdaSvrStart )
	{
		ModifyMenu(hMenuTestApp, IDM_PDASYNCH, MF_BYCOMMAND, IDM_PDASYNCH, (LPCSTR)"Start PDA Synch");
	}
	else
	{
		ModifyMenu(hMenuTestApp, IDM_PDASYNCH, MF_BYCOMMAND, IDM_PDASYNCH, (LPCSTR)"Stop PDA Synch");
	}
}

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 
 *
\**************************************************************************/
#include "window.h"
#include "plx_pdaex.h"
#include "windebug.h"

#include <fcntl.h>
#include <hopen/info.h>
#include <pm/progman.h>
#include <project/imesys.h>
#include <hopen/ipmc.h>


#define	PROJECTMEMORYCLASS	"Project_MemoryWndClass"


static	HWND	hProjectModeFrame;

static  LRESULT ProjectMemoryWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );

extern  HWND	GetProjectModeFrameWnd();

BOOL    CallProjectMemoryWindow()
{
    HWND hwnd;
    WNDCLASS wc;
	RECT rClient;
	
    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = ProjectMemoryWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = NULL;
    wc.hIcon         = NULL;
    wc.hCursor       = NULL;
    wc.hbrBackground = NULL;	
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = PROJECTMEMORYCLASS;
    
    if( !RegisterClass(&wc) )
    {
        return FALSE;
    }
	
	hProjectModeFrame = GetProjectModeFrameWnd();
	GetClientRect(hProjectModeFrame,&rClient);
	
    hwnd = CreateWindow(
        PROJECTMEMORYCLASS,
        ML("Memory Infomation"), 
        WS_VISIBLE | WS_CHILD, 
        rClient.left,rClient.top,rClient.right-rClient.left,rClient.bottom-rClient.top, 
        hProjectModeFrame, NULL, NULL, NULL);
	
    if (!hwnd)
        return FALSE;

	SetFocus(hwnd);
    
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd); 
	
    return TRUE;
}

static LRESULT ProjectMemoryWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
	static  BOOL	bPaint = FALSE;
			HDC		hdc;
			LRESULT lResult;

    lResult = (LRESULT)TRUE;
    switch ( wMsgCmd )
    {
    case WM_CREATE :
		{
			bPaint = TRUE;
			SendMessage(hProjectModeFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("BACK"));
			SendMessage(hProjectModeFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
			SetWindowText(hProjectModeFrame,ML("Memory Infomation"));
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

			if ( bPaint == TRUE )
			{
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

				bPaint = FALSE;
			}
		}
		break;

	case PWM_SHOWWINDOW:
		SendMessage(hProjectModeFrame, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("BACK"));
		SendMessage(hProjectModeFrame, PWM_SETBUTTONTEXT, 2, (LPARAM)"");
		SetWindowText(hProjectModeFrame,ML("Memory Infomation"));
		break;
		
    case WM_CLOSE:
		DestroyWindow(hWnd);   
        break;
		
    case WM_DESTROY :
        UnregisterClass(PROJECTMEMORYCLASS,NULL);
        break;

	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_F10:
			PostMessage(hWnd,WM_CLOSE,0,0);
			SendMessage(hProjectModeFrame, PWM_CLOSEWINDOW,  (WPARAM) hWnd, (LPARAM)0);
			break;
			
		default:
			lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
			break;
		}
		break;

	case WM_COMMAND:
		switch( LOWORD( wParam ))
		{

		}
		break;

	default :
		lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
    }
    return lResult;
	
}


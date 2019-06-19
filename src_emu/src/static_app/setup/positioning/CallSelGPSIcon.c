  /***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : setting\GPS Profile
 *
 * Purpose  : GPS profile icon selection window
 *            
\**************************************************************************/
#include    "window.h"
#include    "string.h"
#include    "stdlib.h"
#include    "stdio.h" 
#include    "winpda.h"
#include    "str_plx.h"
#include    "me_wnd.h"
#include    "plx_pdaex.h"
#include    "str_public.h"
#include    "setting.h"
#include    "setup.h"
#include    "mullang.h"


#define IDC_BUTTON_CANCEL	100
#define IDC_BUTTON_SEL		101

#define X_CHAR        (X_GRID + 1)
#define Y_CHAR        (Y_GRID + 1)
#define X_GRID        2
#define Y_GRID        5
#define CX_CHAR       23
#define CY_CHAR       17
#define CX_GRID       (CX_CHAR + 1)
#define CY_GRID       (CY_CHAR + 1)

#define CR_WHITE      RGB(255, 255, 255)
#define CR_BLACK      RGB(0, 0, 0)

#define CR_BK         GetSysColor(COLOR_MENU) // RGB(239, 241, 246)
#define CR_GRID       RGB(185, 190, 210)
#define CR_TOP        RGB(160, 175, 230)

#define TIMER_ID WM_USER + 100
#define NUM_GRID_ROW          9

#define NUM_GRID_COL		  7	

#define GET_ICON_INDEX(x,y)   (y - 1 + (x - 1) * NUM_GRID_COL)

#define GPS_ICON_WIDTH		  21
#define GPS_ICON_HEIGHT		  13

#define RES_FOCUSBK   "ROM:setup/gps/icon_focus.bmp"


static const char *GPS_Icon_Dir = "ROM:setup/gps";

typedef struct 
{
	int Coord_x;
	int Coord_y;
	/*HICON*/HBITMAP hGPSIcon;
	char szIconName[GPSICONNAMEMAXLEN];
}Focus_Pos_Info;

static Focus_Pos_Info *Icon_Pos = NULL;
static Focus_Pos_Info Icon_Focus_Pos;

static HBITMAP hFocusBmp = NULL;


static HWND hFrameWin = NULL;
static HWND hAppWnd = NULL;
static int  iCurMode= 0;
static GPSMODE G_gm;
static int nPageFlag = 0;
static char * pClassName = "SelectGPSIconClass";

static int nCurFocus;

static unsigned long  RepeatFlag = 0;
static WORD wKeyCode;

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
static void OnPaint(HWND hWnd);
static BOOL OnCreate(HWND hWnd);
static void OnKeyDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void DrawGPSProfileIcon(HDC hdc, RECT rcClient, RECT rcFocus);
static void GPSIconSet_OnVScroll(HWND hWnd,  UINT wParam);
static void GPSIconSet_InitVScrolls(HWND hWnd,int iItemNum, int init_Focus);
static void   OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam);
static void OnTimeProcess(HWND hWnd, WPARAM wParam);


void TransferBmpAndIcon(char* OldPath, char* NewPath);
extern BOOL Sett_FileIfExist(const char * pFileName);
extern BOOL SetGPSprofile(GPSMODE *gm, int iGPSModeIndex);

BOOL CallSelGPSIcon(HWND hWnd, int iCurGPSIndex)
{
    WNDCLASS wc;
    RECT rClient;

    hFrameWin = hWnd;
    iCurMode = iCurGPSIndex;
	memset(&G_gm, 0, sizeof(GPSMODE));
	GetGPSprofile(&G_gm, iCurMode); 

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

    GetGPSprofile(&G_gm, iCurMode);
    GetClientRect(hFrameWin, &rClient);
    hAppWnd = CreateWindow(pClassName, NULL, 
        WS_VISIBLE | WS_CHILD | WS_VSCROLL, 
        rClient.left,
        rClient.top,
        rClient.right-rClient.left,
        rClient.bottom-rClient.top,
        hFrameWin, 
        NULL, NULL, NULL);

    if (NULL == hAppWnd)
    {
        UnregisterClass(pClassName,NULL);
        return FALSE;
    }
    SendMessage(hFrameWin , PWM_SETBUTTONTEXT, 1, NULL);//LSK
            
        
    SendMessage(hFrameWin, PWM_CREATECAPTIONBUTTON, 
            IDC_BUTTON_CANCEL, (LPARAM)ML("Cancel"));//RSK  
    SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");

    SetWindowText(hFrameWin, ML("Select Icon"));
    SetFocus(hAppWnd);

    return TRUE;
	
}

static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
{
    LRESULT lResult;
	int i;
    lResult = TRUE;   

    switch ( wMsgCmd )
    {
    case PWM_SHOWWINDOW:
		SetFocus(hAppWnd);
		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 1, NULL);//LSK
		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 0, (LPARAM)ML("Cancel"));//RSK  
		SendMessage(hFrameWin, PWM_SETBUTTONTEXT, 2, (LPARAM)"Select");
#if(SETT_SHOW_TITLE_ICON)			
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, SIGNALICON), 0);
			SendMessage(hFrameWin, PWM_SETAPPICON, MAKEWPARAM(IMAGE_BITMAP, POWERICON), 0);
#endif
		
		SetWindowText(hFrameWin, ML("Select Icon"));
        break;   
    
    case WM_CREATE :
		lResult = OnCreate(hWnd);
	    break;
	
	case WM_TIMER:
		OnTimeProcess(hWnd, wParam);
		break;
	case WM_KILLFOCUS:
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
		wKeyCode   = 0;
		break;
		
	case WM_KEYUP:
		RepeatFlag = 0;
		switch(LOWORD(wParam))
		{
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			KillTimer(hWnd, TIMER_ID);
			break;
		default:
			break;
		}        
		break;
		
	case WM_VSCROLL:
		GPSIconSet_OnVScroll(hWnd,wParam);
		break;

	case WM_PAINT:
		OnPaint(hWnd);
		break;
		
    case WM_SETFOCUS:
        break;
        
    case WM_KEYDOWN:
		OnKeyDown(hWnd, wMsgCmd, wParam, lParam);
        break;  

   case WM_COMMAND :
	   OnCommand(hWnd, wMsgCmd, wParam, lParam);
        break;

    case WM_DESTROY : 
		for (i = 0; i < NUM_GRID_ROW * NUM_GRID_COL; i++)
		{
			if (Icon_Pos[i].hGPSIcon)
				/*DestroyIcon*/DeleteObject(Icon_Pos[i].hGPSIcon);
		}

		if (Icon_Pos)
		{
			free(Icon_Pos);
			Icon_Pos = NULL;
		}

		DeleteObject(hFocusBmp);
		hAppWnd = NULL;
		UnregisterClass(pClassName,NULL);
		break;
		
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    default :
        lResult = PDADefWindowProc ( hWnd, wMsgCmd, wParam, lParam );
        break;
    } 
    return lResult;
    
	
}
/*********************************************************************\
* Function	   OnCreate
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
static BOOL OnCreate(HWND hWnd)
{
	int i, j, iFocus = 0;
	HDC hdc;
	char szTmp[GPSICONNAMEMAXLEN + 1];
	
	hFocusBmp = LoadImage(NULL, RES_FOCUSBK, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);       

	if (hFocusBmp == NULL)
		return FALSE;

	Icon_Pos = (Focus_Pos_Info*)malloc(NUM_GRID_ROW * NUM_GRID_COL * sizeof(Focus_Pos_Info));
	
	if (Icon_Pos == NULL)
		return FALSE;

	memset(&Icon_Focus_Pos, 0, sizeof(Focus_Pos_Info));
	memset(Icon_Pos, 0, NUM_GRID_ROW * NUM_GRID_COL * sizeof(Focus_Pos_Info));

	for (i = 0; i < NUM_GRID_ROW; i++)
	{	
		for (j = 0; j < NUM_GRID_COL; j++)
		{
			Icon_Pos[j + i * NUM_GRID_COL].Coord_y = j + 1;
			Icon_Pos[j + i * NUM_GRID_COL].Coord_x = i + 1;
		}
	}
	
	hdc         = GetDC(hWnd);

	for (i = 0; i < NUM_GRID_ROW * NUM_GRID_COL; i++)
	{
		sprintf(Icon_Pos[i].szIconName, "%s%s%d%s", GPS_Icon_Dir, "/GPS_profile_", i+1, ".bmp"/*".ico"*/);
		Icon_Pos[i].hGPSIcon = LoadImageEx(hdc, NULL, Icon_Pos[i].szIconName, /*IMAGE_ICON*/IMAGE_BITMAP, GPS_ICON_WIDTH, 
			GPS_ICON_HEIGHT, LR_LOADFROMFILE);
		TransferBmpAndIcon(Icon_Pos[i].szIconName, szTmp);
		if (strcmp(G_gm.cGPSIconName, szTmp) == 0)
		{
			iFocus = i;
		}
	}

	ReleaseDC(hWnd,hdc);

	Icon_Focus_Pos.Coord_x = (int)( iFocus / NUM_GRID_COL ) + 1;
	Icon_Focus_Pos.Coord_y = (int)( iFocus % NUM_GRID_COL ) + 1; //focus on the top left at first;
	 
	if (Icon_Focus_Pos.Coord_x > 8)
		nPageFlag = 2;
	else
		nPageFlag = 1;
	GPSIconSet_InitVScrolls(hWnd,NUM_GRID_ROW, Icon_Focus_Pos.Coord_x - 1);

	return TRUE;
}
/*********************************************************************\
* Function	   OnPaint
* Purpose      
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/

static void OnPaint(HWND hWnd)
{
    HDC          hdc = NULL;
    PAINTSTRUCT  ps;
    RECT         rcClient;
    HPEN         hpenGrid = NULL, hpenTop = NULL, hpenLeft = NULL,
                 hpenOld = NULL;
    int          i = 0, nPos = 0;
    BITMAP       bmp;
	RECT		 rcFocus;


    hdc = BeginPaint(hWnd, &ps);

    GetClientRect(hWnd, &rcClient);
    ClearRect(hdc, &rcClient, CR_BK);

    hpenGrid = CreatePen(PS_SOLID, 1, CR_GRID);
    hpenTop = CreatePen(PS_SOLID, Y_GRID, CR_TOP);
    hpenLeft = CreatePen(PS_SOLID, X_GRID, CR_GRID);

    hpenOld = SelectObject(hdc, hpenGrid);
    for (i = 0; i < NUM_GRID_ROW + 1; i++)
    {
        nPos = rcClient.top + Y_GRID + i * CY_GRID;
        DrawLine(hdc, rcClient.left, nPos, /*rcClient.right*/rcClient.left + X_GRID + NUM_GRID_COL * CX_GRID, nPos);
    }
    for (i = 0; i < NUM_GRID_COL + 1; i++)
    {
        nPos = rcClient.left + X_GRID + i * CX_GRID;
        DrawLine(hdc, nPos, rcClient.top, nPos, /*rcClient.bottom*/rcClient.top + Y_GRID + NUM_GRID_ROW * CY_GRID);
    }
    SelectObject(hdc, hpenTop);
    DrawLine(hdc, rcClient.left + X_GRID, rcClient.top + (Y_GRID >> 1),
        rcClient.right, rcClient.top + (Y_GRID >> 1));
    SelectObject(hdc, hpenLeft);
    DrawLine(hdc, rcClient.left + (X_GRID >> 1), rcClient.top,
        rcClient.left + (X_GRID >> 1), rcClient.bottom);

    SelectObject(hdc, hpenOld);
    DeleteObject(hpenGrid);
    DeleteObject(hpenTop);
    DeleteObject(hpenLeft);

    GetObject(hFocusBmp, sizeof(BITMAP), (void*)&bmp);

	rcFocus.left = rcClient.left + X_GRID + (Icon_Focus_Pos.Coord_y - 1) * CX_GRID + 1;

	if (1 == nPageFlag)
	{
		rcFocus.top = rcClient.top + Y_GRID + (Icon_Focus_Pos.Coord_x - 1) * CY_GRID + 1;
	}
	else if (2 == nPageFlag)
	{
		rcFocus.top = rcClient.top + Y_GRID + (Icon_Focus_Pos.Coord_x - 2) * CY_GRID + 1;
	}

	rcFocus.right = bmp.bmWidth;
	rcFocus.bottom = bmp.bmHeight;

	DrawGPSProfileIcon(hdc, rcClient, rcFocus);
    EndPaint(hWnd, &ps);
}

static void OnKeyDown(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	int iIndex;
	int iTmp;
	RECT	rcRefArea;
    RECT         rcClient;
	BITMAP       bmp;
   
	GetClientRect(hWnd, &rcClient);
    GetObject(hFocusBmp, sizeof(BITMAP), (void*)&bmp);

	rcRefArea.left = rcClient.left + X_GRID + (Icon_Focus_Pos.Coord_y - 1) * CX_GRID + 1;

	if (1 == nPageFlag)
	{
		rcRefArea.top = rcClient.top + Y_GRID + (Icon_Focus_Pos.Coord_x - 1) * CY_GRID + 1;
	}
	else if (2 == nPageFlag)
	{
		rcRefArea.top = rcClient.top + Y_GRID + (Icon_Focus_Pos.Coord_x - 2) * CY_GRID + 1;
	}

	rcRefArea.right = rcRefArea.left + bmp.bmWidth;
	rcRefArea.bottom = rcRefArea.top + bmp.bmHeight;

	InvalidateRect(hWnd, &rcRefArea, TRUE);

	if (RepeatFlag > 0 && wKeyCode != LOWORD(wParam))
	{
		KillTimer(hWnd, TIMER_ID);
		RepeatFlag = 0;
	}
	
	wKeyCode = LOWORD(wParam);
	RepeatFlag++;


	switch(LOWORD(wParam))
	{       
	case VK_F10:
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	case VK_F5:
		SendMessage(hWnd,WM_COMMAND,IDC_BUTTON_SEL,0);
		break;
	case VK_DOWN:
		iTmp = Icon_Focus_Pos.Coord_x;
		do 
		{
			Icon_Focus_Pos.Coord_x++;			
			
			if (Icon_Focus_Pos.Coord_x > NUM_GRID_ROW)
			{
				Icon_Focus_Pos.Coord_x = 1;
			}
			
			if (iTmp == Icon_Focus_Pos.Coord_x)
				break;
			
			iIndex = GET_ICON_INDEX(Icon_Focus_Pos.Coord_x, Icon_Focus_Pos.Coord_y);
	
			SendMessage(hWnd, WM_VSCROLL, SB_LINEDOWN, NULL);
		}
		while (Icon_Pos[iIndex].hGPSIcon == NULL);
		
		if (1 == Icon_Focus_Pos.Coord_x)
		{
			nPageFlag = 1;
		}
		if (NUM_GRID_ROW == Icon_Focus_Pos.Coord_x)
		{
			nPageFlag = 2;
		}
//		InvalidateRect(hWnd,NULL,TRUE);
		if ( 1 == RepeatFlag )
		{
			SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
		}
		break;
		
	case VK_UP:
		iTmp = Icon_Focus_Pos.Coord_x;
		do 
		{
			Icon_Focus_Pos.Coord_x--;			
			
			if (Icon_Focus_Pos.Coord_x < 1)
			{
				Icon_Focus_Pos.Coord_x = NUM_GRID_ROW;
			}
			
			if (iTmp == Icon_Focus_Pos.Coord_x)
				break;
			
			iIndex = GET_ICON_INDEX(Icon_Focus_Pos.Coord_x, Icon_Focus_Pos.Coord_y);
	
			SendMessage(hWnd, WM_VSCROLL, SB_LINEUP, NULL);
		}
		while (Icon_Pos[iIndex].hGPSIcon == NULL);
		
		if (1 == Icon_Focus_Pos.Coord_x)
		{
			nPageFlag = 1;
		}
		if (NUM_GRID_ROW == Icon_Focus_Pos.Coord_x)
		{
			nPageFlag = 2;
		}
//		InvalidateRect(hWnd,NULL,TRUE);
		if ( 1 == RepeatFlag )
		{
			SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
		}
		break;
	case VK_LEFT:
		iTmp = Icon_Focus_Pos.Coord_y;
		do 
		{
			Icon_Focus_Pos.Coord_y--;			
			
			if (Icon_Focus_Pos.Coord_y < 1)
			{
				Icon_Focus_Pos.Coord_y = NUM_GRID_COL;
			}

			if (iTmp == Icon_Focus_Pos.Coord_y)
				break;
			
			iIndex = GET_ICON_INDEX(Icon_Focus_Pos.Coord_x, Icon_Focus_Pos.Coord_y);
		}
		while (Icon_Pos[iIndex].hGPSIcon == NULL);
			
//		InvalidateRect(hWnd,NULL,TRUE);		
		if ( 1 == RepeatFlag )
		{
			SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
		}
		break;

	case VK_RIGHT:
		iTmp = Icon_Focus_Pos.Coord_y;
		do 
		{
			Icon_Focus_Pos.Coord_y++;			
			
			if (Icon_Focus_Pos.Coord_y > NUM_GRID_COL)
			{
				Icon_Focus_Pos.Coord_y = 1;
			}

			if (iTmp == Icon_Focus_Pos.Coord_y)
				break;
			
			iIndex = GET_ICON_INDEX(Icon_Focus_Pos.Coord_x, Icon_Focus_Pos.Coord_y);
		}
		while (Icon_Pos[iIndex].hGPSIcon == NULL);
			
//		InvalidateRect(hWnd,NULL,TRUE);
		if ( 1 == RepeatFlag )
		{
			SetTimer(hWnd, TIMER_ID, ET_REPEAT_FIRST, NULL);
		}
		break;

	default:
	    PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
		break;
	}		

	rcRefArea.left = rcClient.left + X_GRID + (Icon_Focus_Pos.Coord_y - 1) * CX_GRID + 1;

	if (1 == nPageFlag)
	{
		rcRefArea.top = rcClient.top + Y_GRID + (Icon_Focus_Pos.Coord_x - 1) * CY_GRID + 1;
	}
	else if (2 == nPageFlag)
	{
		rcRefArea.top = rcClient.top + Y_GRID + (Icon_Focus_Pos.Coord_x - 2) * CY_GRID + 1;
	}

	rcRefArea.right = rcRefArea.left + bmp.bmWidth;
	rcRefArea.bottom = rcRefArea.top + bmp.bmHeight;

	InvalidateRect(hWnd, &rcRefArea, TRUE);

}
static void   OnCommand(HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam)
{
	int i;
	switch(LOWORD(wParam))
	{
	case IDC_BUTTON_SEL:
		for (i = 0; i < NUM_GRID_ROW * NUM_GRID_COL; i++)
		{
			if (Icon_Pos[i].Coord_x == Icon_Focus_Pos.Coord_x && Icon_Pos[i].Coord_y == Icon_Focus_Pos.Coord_y)
			{
				strncpy(G_gm.cGPSIconName, Icon_Pos[i].szIconName, GPSICONNAMEMAXLEN);
				TransferBmpAndIcon(G_gm.cGPSIconName, G_gm.cGPSIconName);
				SetGPSprofile(&G_gm, iCurMode);
				if ( 1 == G_gm.ActiFlag )
				{					
					DlmNotify(PS_FRASHGPS, TRUE);
				}
				break;
			}
		}
		SendMessage(hFrameWin,PWM_CLOSEWINDOW,(WPARAM)hWnd,0); 
		PostMessage(hWnd, WM_CLOSE, 0, 0);			
		
		break;
		
	default:
		break;
	}
}

static void DrawGPSProfileIcon(HDC hdc, RECT rcClient, RECT rcFocus)
{
	int i;
	int x = 0, y = 0;
    COLORREF   crOld = 0;
    int nMode = 0;
//	HDC hDCMem;
    HBITMAP hIconBmp = NULL;
//	RECT rc;

	StretchBlt(hdc, rcFocus.left, rcFocus.top, CX_CHAR, CY_CHAR,
        (HDC)hFocusBmp, 0, 0, rcFocus.right, rcFocus.bottom, ROP_SRC);

    nMode = SetBkMode(hdc, TRANSPARENT);

	if ( 1 == nPageFlag)
	{
		for (i = 0; i < NUM_GRID_ROW * NUM_GRID_COL; i++)
		{
			x = rcClient.left + X_GRID + (Icon_Pos[i].Coord_y - 1) * CX_GRID + 2;
			y = rcClient.top + Y_GRID + (Icon_Pos[i].Coord_x - 1) * CY_GRID + 2;
			
			if (Icon_Pos[i].hGPSIcon)
			{
				if (Icon_Pos[i].Coord_x == Icon_Focus_Pos.Coord_x && Icon_Pos[i].Coord_y == Icon_Focus_Pos.Coord_y)
				{
/*
					hDCMem = CreateCompatibleDC(hdc);
					hIconBmp = CreateCompatibleBitmap(hDCMem, GPS_ICON_WIDTH, GPS_ICON_HEIGHT);
					SelectObject(hDCMem, hIconBmp);
					
					SetRect(&rc, 0, 0, GPS_ICON_WIDTH, GPS_ICON_HEIGHT);
					ClearRect(hDCMem, &rc, CR_WHITE);
					
					crOld = SetBkColor(hDCMem, CR_BLACK);
					DrawIcon(hDCMem,0, 0, Icon_Pos[i].hGPSIcon);            
					SetBkColor(hDCMem, crOld);
			

					nMode = SetBkMode(hdc, NEWTRANSPARENT);
*/
					nMode = SetBkMode(hdc, NEWTRANSPARENT);
					crOld = SetBkColor(hdc, CR_WHITE);
					/*StretchBlt*/BitBlt(hdc, x, y, GPS_ICON_WIDTH, GPS_ICON_HEIGHT, (HDC)Icon_Pos[i].hGPSIcon/*hIconBmp*/, 0, 0/*,GPS_ICON_WIDTH, GPS_ICON_HEIGHT*/, ROP_NSRC);
	                SetBkColor(hdc, crOld);
	                SetBkMode(hdc, nMode);

/*
					SetBkMode(hdc, nMode);


					DeleteObject(hIconBmp);
  					DeleteDC(hDCMem);
*/
				}
				else
				{
//					DrawIcon(hdc, x, y, Icon_Pos[i].hGPSIcon);
					nMode = SetBkMode(hdc, NEWTRANSPARENT);
					crOld = SetBkColor(hdc, CR_WHITE);
					/*StretchBlt*/BitBlt(hdc, x, y, GPS_ICON_WIDTH, GPS_ICON_HEIGHT, (HDC)Icon_Pos[i].hGPSIcon/*hIconBmp*/, 0, 0/*,GPS_ICON_WIDTH, GPS_ICON_HEIGHT*/, ROP_SRC);
					SetBkColor(hdc, crOld);
					SetBkMode(hdc, nMode);
				}
			}
		}
	}

	else if ( 2 == nPageFlag )
	{
		for ( i = NUM_GRID_COL; i <  NUM_GRID_ROW * NUM_GRID_COL; i++)
		{
			x = rcClient.left + X_GRID + (Icon_Pos[i].Coord_y - 1) * CX_GRID + 2;
			y = rcClient.top + Y_GRID + (Icon_Pos[i].Coord_x - 2) * CY_GRID + 2;
			if (Icon_Pos[i].hGPSIcon)
			{
				if (Icon_Pos[i].Coord_x == Icon_Focus_Pos.Coord_x && Icon_Pos[i].Coord_y == Icon_Focus_Pos.Coord_y)
				{
/*
					hDCMem = CreateCompatibleDC(hdc);
					hIconBmp = CreateCompatibleBitmap(hDCMem, GPS_ICON_WIDTH, GPS_ICON_HEIGHT);
					SelectObject(hDCMem, hIconBmp);
					
					SetRect(&rc, 0, 0, GPS_ICON_WIDTH, GPS_ICON_HEIGHT);
					ClearRect(hDCMem, &rc, CR_WHITE);
					
					DrawIcon(hDCMem,0, 0, Icon_Pos[i].hGPSIcon);            
			
					nMode = SetBkMode(hdc, NEWTRANSPARENT);
					crOld = SetBkColor(hdc, CR_WHITE);
					BitBlt(hdc, x, y, GPS_ICON_WIDTH, GPS_ICON_HEIGHT, hDCMem, 0, 0, ROP_NSRC);
					SetBkColor(hdc, crOld);
					SetBkMode(hdc, nMode);

					DeleteObject(hIconBmp);
  					DeleteDC(hDCMem);
*/
					/*StretchBlt*/BitBlt(hdc, x, y, GPS_ICON_WIDTH, GPS_ICON_HEIGHT, (HDC)Icon_Pos[i].hGPSIcon/*hIconBmp*/, 0, 0/*,GPS_ICON_WIDTH, GPS_ICON_HEIGHT*/, ROP_NSRC);
				}
				else
				{
// 					DrawIcon(hdc, x, y, Icon_Pos[i].hGPSIcon);
					/*StretchBlt*/BitBlt(hdc, x, y, GPS_ICON_WIDTH, GPS_ICON_HEIGHT, (HDC)Icon_Pos[i].hGPSIcon/*hIconBmp*/, 0, 0/*,GPS_ICON_WIDTH, GPS_ICON_HEIGHT*/, ROP_NSRC);
				}
			}
		}
	}
    SetBkMode(hdc, nMode);
}

/*********************************************************************\
* Function        GPSIconSet_InitVScrolls
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/
static void GPSIconSet_InitVScrolls(HWND hWnd,int iItemNum, int init_Focus)
{
    static SCROLLINFO   vsi;

    memset(&vsi, 0, sizeof(SCROLLINFO));

    vsi.cbSize = sizeof(vsi);
    vsi.fMask  = SIF_ALL ;
    vsi.nMin   = 0;
    vsi.nPage  = 3;
    vsi.nMax   = (iItemNum-1);
    vsi.nPos   = 0;

    SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
	nCurFocus = init_Focus;
	return;
}
/*********************************************************************\
* Function        GPSIconSet_OnVScroll
* Purpose      
* Params          
* Return         
* Remarks        
**********************************************************************/

static void GPSIconSet_OnVScroll(HWND hWnd,  UINT wParam)
{
    static RECT rcClient;
    static SCROLLINFO      vsi;

    switch(wParam)
    {
    case SB_LINEDOWN:
		memset(&vsi, 0, sizeof(SCROLLINFO));
		
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
		
        nCurFocus++;
		
        if(nCurFocus > vsi.nMax)	
        {
			nCurFocus = 0;
			vsi.nPos = 0;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
	
        if(((int)(vsi.nPos + vsi.nPage - 1) <= nCurFocus)/* && nCurFocus != vsi.nMax*/)	//modified for UISG
        { 
            vsi.nPos++;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);
            break;
        }
		break;
		
    case SB_LINEUP:
		
        memset(&vsi, 0, sizeof(SCROLLINFO));
        vsi.fMask  = SIF_ALL ;
        GetScrollInfo(hWnd, SB_VERT, &vsi);
		
        GetClientRect(hWnd,&rcClient);
        nCurFocus--;
		
        if(nCurFocus < vsi.nMin)	
        {
			nCurFocus = vsi.nMax;
			vsi.nPos = vsi.nMax;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   

            break;	
        }
		
        if((int)vsi.nPos >= nCurFocus)	//modified for UISG
        { 
            vsi.nPos--;
            vsi.fMask  = SIF_POS ;
            SetScrollInfo(hWnd, SB_VERT, &vsi, TRUE);   
            break;	
        }
		break;
    case SB_PAGEDOWN:
        break;
    case SB_PAGEUP:
        break;
    default:
        break;	
    }	
}

static void OnTimeProcess(HWND hWnd, WPARAM wParam)
{
	    switch(wParam)
		{
		case TIMER_ID:
            if (1 == RepeatFlag)
            {
                KillTimer(hWnd, TIMER_ID);
                SetTimer(hWnd, TIMER_ID, ET_REPEAT_LATER, NULL);
            }
			keybd_event(wKeyCode, 0, 0, 0);
			break;
       default:
			KillTimer(hWnd, wParam);
            break;
		}

}

void TransferBmpAndIcon(char* OldPath, char* NewPath)
{
	char* cTmp = NULL;

	if (NewPath == NULL || OldPath == NULL)
		return;
	if (sizeof(NewPath) < sizeof(OldPath))
		return;
	strcpy(NewPath, OldPath);
	cTmp = strstr(NewPath, ".bmp");
	
	if (cTmp != NULL)
	{
		strcpy(cTmp, ".ico");
	}
	else if (cTmp == NULL)
	{
		cTmp = strstr(NewPath, ".ico");
		if (cTmp != NULL)
		{
			strcpy(cTmp, ".bmp");
		}
	}

}

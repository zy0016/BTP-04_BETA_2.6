/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Pass Change
 *
 * Purpose  : Provide a control to select a color.    
 *            
\**************************************************************************/

#include "window.h"
#include "str_public.h"
#include "str_plx.h"
#include "winpda.h"
//#include "hpdebug.h"

#include "pubapp.h"

#ifndef _COLORSEL_
#define _COLORSEL_
#endif

#define CS_EXITLOOP     WM_USER + 100

#define IDM_EXIT		1001  //取消
#define IDM_CONFIRM		1002  //确定
#define IDC_SPINR        1003
#define IDC_SPING        1004
#define IDC_SPINB        1005

//调色板位置与尺寸
#define PAL_LEFT       0
#define PAL_TOP        0
#define PAL_WIDTH      WNDWIDTH //240
#define PAL_HEIGHT     100 //120
//共有GRID_NUM*GRID_NUM个色块
#define GRID_NUM       8
//色块大小
#define GRID_WIDTH     (PAL_WIDTH / GRID_NUM)//28
#define GRID_HEIGHT    (PAL_HEIGHT / GRID_NUM)//17

//NumSpin控件尺寸
#define SPIN_WIDTH     80 //120
#define SPIN_HEIGHT    18 //30
//NumSpin控件位置
#define RSPIN_LEFT     30
#define RSPIN_TOP      102//123
#define GSPIN_LEFT     30
#define GSPIN_TOP      122//156
#define BSPIN_LEFT     30
#define BSPIN_TOP      142//189

//提示字符的大小与位置
//#define IDS_PREVIEW    "        \r\n        \r\n Preview\r\n        \r\n         "
#define IDS_PREVIEW    "        \r\n  颜色  \r\n  预览  \r\n         "
//预览区位置与尺寸
#define PREVIEW_LEFT   116 //156
#define PREVIEW_TOP    102 //123
#define PREVIEW_WIDTH  55  //80
#define PREVIEW_HEIGHT 60 //96

#define NSBN_KEYUP     WM_USER + 1
#define NSBN_KEYDOWN   WM_USER + 2
static BYTE g_r = 0, g_g = 0, g_b = 0;
static WNDPROC    OldNumSpinBoxProc;

COLORREF g_Pallete[GRID_NUM][GRID_NUM] = { 
    {0xffffff,0xc0c0ff,0x8080ff,0x4040ff,0x0000ff,0x0000c0,0x000080,0x000040},
    {0xe0e0e0,0xe0c0ff,0xc080ff,0xa040ff,0x8000ff,0x6000c0,0x400080,0x200040},
    {0xc0c0c0,0xffc0ff,0xff80ff,0xff40ff,0xff00ff,0xc000c0,0x800080,0x400040},
    {0xa0a0a0,0xffc0e0,0xff80c0,0xff40a0,0xff0080,0xc00060,0x800040,0x400020},
    {0x808080,0xffc0c0,0xff8080,0xff4040,0xff0000,0xc00000,0x800000,0x400000},
    {0x404040,0xffffc0,0xffff80,0xffff40,0xffff00,0xc0c000,0x808000,0x404000},
    {0x202020,0xc0ffc0,0x80ff80,0x40ff40,0x00ff00,0x00c000,0x008000,0x004000},
    {0x000000,0xc0ffff,0x80ffff,0x40ffff,0x00ffff,0x00c0c0,0x008080,0x004040}};


//Define data structure for passcheck control
typedef struct
{
    COLORREF colIn;
    COLORREF colReturn;
    PCSTR szOk;
    PCSTR szCancel;
    PCSTR szCaption;
    //BOOL  bPic;
    BOOL  bResult;
} COLORSELDATA , *PCOLORSELDATA;

BOOL    CreateColorSelBox(PCOLORSELDATA pColorSelData);
void    DrawNewSelect(HWND hWnd , int nXGridOld , int nYGridOld ,
                                  int nXGridNew , int nYGridNew ,
                                  HWND hSpinR,HWND hSpinG,HWND hSpinB,
                                  RECT* prcPreview );
void DrawFrame(HWND hWnd,int nX,int nY,HWND hSpinR,HWND hSpinG,HWND hSpinB,RECT* pRect,BOOL bBlack);

LRESULT CALLBACK ColorSel_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);
LRESULT CALLBACK MyNumSpin_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam);

/*********************************************************************\
* Function: PLXSelectColor   
* Purpose: Recieve application input and return the color selected      
* Params:  szCaption     Caption text
*          cr            The init color value input 
*          pr,pg,pb      The result color returned
*          szOk,szCancel Text on buttons	   
* Return:  TRUE if successful,FALSE if no color is selected.		   
* Remarks	   
**********************************************************************/
BOOL PLXSelectColor(PCSTR szCaption, COLORREF cr, BYTE *pr, BYTE *pg, BYTE *pb, 
                   PCSTR szOk, PCSTR szCancel)
{
    COLORSELDATA ColorData;

#ifdef _COLORSEL_
//    StartObjectDebug();
#endif

    ColorData.colIn     = RGB(g_r, g_g, g_b);//cr;
    ColorData.szCaption = szCaption;
    ColorData.szOk      = szOk;
    ColorData.szCancel  = szCancel;

    ColorData.bResult = FALSE;

    CreateColorSelBox(&ColorData);

    *pr = (BYTE)GetRValue(ColorData.colReturn);
    *pg = (BYTE)GetGValue(ColorData.colReturn);
    *pb = (BYTE)GetBValue(ColorData.colReturn);

    if(ColorData.bResult == TRUE)
    {
        g_r = *pr;
        g_g = *pg;
        g_b = *pb;
    }

#ifdef _COLORSEL_
//    EndObjectDebug();
#endif

    return(ColorData.bResult);
}

/*********************************************************************\
* Function: CreateColorSelBox
* Purpose:  Create and show the the ColorSelect interface.
* Params:   pColorSelData  Contain the application input data.	   
* Return	TRUE if successful,FALSE if no color is selected.	   
* Remarks	   
**********************************************************************/

BOOL CreateColorSelBox(PCOLORSELDATA pColorSelData)
{
    HWND hColorWnd;
	MSG msg;
    WNDCLASS wc;
	
	
    //Register window class.
    wc.style            = CS_OWNDC;//CS_VREDRAW;//CS_PARENTDC;
    wc.lpfnWndProc      = ColorSel_WndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hInstance        = NULL;
    wc.hIcon            = NULL;
    wc.hCursor          = NULL;//LoadCursor(NULL, IDC_IBEAM);
    wc.lpszMenuName     = NULL;
    wc.lpszClassName    = "COLORSEL";
    wc.hbrBackground    = GetStockObject(WHITE_BRUSH);

    if (!RegisterClass(&wc))
        return FALSE;

    //Set MyNumSpinBox
    GetClassInfo(NULL, "NumSpinBox", &wc);
    OldNumSpinBoxProc = wc.lpfnWndProc;
    wc.lpfnWndProc      = MyNumSpin_WndProc;
    wc.lpszClassName    = "MyNumSpinBox";
    if (!RegisterClass(&wc))
    {
        UnregisterClass("COLORSEL", NULL);
        return FALSE;
    }

    //Create window.
    hColorWnd = CreateWindow("COLORSEL",pColorSelData->szCaption,
		       WS_VISIBLE | WS_POPUP | WS_CAPTION | PWS_STATICBAR,/*| WS_CAPTION*/
		       0, 18/*24*/, WNDWIDTH, WNDHEIGHT, // 176, 216,
		       NULL,NULL,NULL,(LPVOID)pColorSelData
		       );
    
	
	SendMessage(hColorWnd, PWM_CREATECAPTIONBUTTON, MAKEWPARAM(IDM_CONFIRM,1), (LPARAM)pColorSelData->szOk);	
	SendMessage(hColorWnd, PWM_CREATECAPTIONBUTTON, (WPARAM)IDM_EXIT , (LPARAM)pColorSelData->szCancel);

    //MessageLoop.
    while (TRUE)
    {
        GetMessage(&msg, 0, 0, 0);

        if (msg.hwnd == hColorWnd && (msg.message == CS_EXITLOOP))
		{
			DestroyWindow(hColorWnd);
            break;
		}

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!IsWindowVisible(hColorWnd))
            break;
    }

    UnregisterClass("COLORSEL",NULL);
    UnregisterClass("MyNumSpinBox",NULL);

    return(TRUE);
}

/*********************************************************************\
* Function: ColorSel_WndProc
* Purpose:  Deal with messages.    
* Params:   hWnd
*           wMsgCmd
*           wParam
*           lParam	   
* Return		   
* Remarks	   
**********************************************************************/
LRESULT CALLBACK ColorSel_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
	LRESULT			     lResult;
	HDC				     hdc;

    static PCOLORSELDATA p;
    
    static HWND          hSpinR;
    static HWND          hSpinG;
    static HWND          hSpinB;
    static HWND          hLastFocus;

    static int           nR;
    static int           nG;
    static int           nB;
    
   
    //当前选中的色块在调色板中的位置
    static int           nXPendown;
    static int           nYPendown;
    //矩形预览区
    static RECT          rtPreview;

    lResult = 0;
    
    
    switch (wMsgCmd)	
    {
    case WM_CREATE :
        {
            PCREATESTRUCT   pCreateData;

            pCreateData = (PCREATESTRUCT)lParam;
            p = (PCOLORSELDATA)pCreateData->lpCreateParams;

            //传入的颜色初值
            nR = GetRValue(p->colIn);
            nG = GetGValue(p->colIn);
            nB = GetBValue(p->colIn);

            hSpinR = CreateWindow("MyNumSpinBox",NULL,
		               WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,
		               RSPIN_LEFT,RSPIN_TOP,SPIN_WIDTH,SPIN_HEIGHT,
		               hWnd,(HMENU)IDC_SPINR,NULL,NULL
		               );
		    hSpinG = CreateWindow("MyNumSpinBox",NULL,
		               WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,
		               GSPIN_LEFT,GSPIN_TOP,SPIN_WIDTH,SPIN_HEIGHT,
		               hWnd,(HMENU)IDC_SPING,NULL,NULL
		               );
            hSpinB = CreateWindow("MyNumSpinBox",NULL,
		               WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP,
		               BSPIN_LEFT,BSPIN_TOP,SPIN_WIDTH,SPIN_HEIGHT,
		               hWnd,(HMENU)IDC_SPINB,NULL,NULL
		               );

            SendMessage(hSpinR,NSBM_SETINITDATA,MAKELONG(0,255),MAKELONG(1,nR));
            SendMessage(hSpinG,NSBM_SETINITDATA,MAKELONG(0,255),MAKELONG(1,nG));
            SendMessage(hSpinB,NSBM_SETINITDATA,MAKELONG(0,255),MAKELONG(1,nB));

            //初始焦点
            hLastFocus = hSpinR;
            

            //预览区域
            rtPreview.left = PREVIEW_LEFT;
            rtPreview.top  = PREVIEW_TOP;
            rtPreview.right = PREVIEW_LEFT + PREVIEW_WIDTH;
            rtPreview.bottom = PREVIEW_TOP + PREVIEW_HEIGHT;

            //调色板选中色块初始值
            nXPendown = -1;
            nYPendown = -1;


            lResult = 0;

        }
        break;

    case WM_ACTIVATE:
        switch(LOWORD(wParam))
        {
        case WA_ACTIVE:
            SetFocus(hLastFocus);
            hLastFocus = NULL;
            break;
        case WA_INACTIVE:
            hLastFocus = GetFocus();
            
            break;
        }
        
        break;

    //case WM_DESTROY:
       
    //    break;


	case WM_PAINT:
		{
            HBRUSH   hBrush,hOldBrush;
            HPEN     hPen,hOldPen;
            COLORREF colTextOld,colBkOld,colBar;
            RECT     rtRGB;
            RECT     rc;
            int      i,j;

            colBar = GetWindowColor(BAR_COLOR);

		    hdc = BeginPaint(hWnd, NULL);
		
            //提示文字
            //R:
            rtRGB.left = 4;
            rtRGB.top  = 102; //123;
            rtRGB.right = 24;
            rtRGB.bottom = 120; //153;
            SetBkColor(hdc,RGB(255,0,0));
            SetTextColor(hdc,RGB(0,255,255));
            DrawText(hdc,"    \n R: \n    ",-1,&rtRGB,DT_VCENTER|DT_CENTER);

            //G:
            rtRGB.top  = 122; //156;
            rtRGB.bottom = 140; //186;
            SetBkColor(hdc,RGB(0,255,0));
            SetTextColor(hdc,RGB(255,0,255));
            DrawText(hdc,"    \n G: \n    ",-1,&rtRGB,DT_VCENTER|DT_CENTER);

            //B:
            rtRGB.top  = 142; //189;
            rtRGB.bottom = 160; //219;
            SetBkColor(hdc,RGB(0,0,255));
            SetTextColor(hdc,RGB(255,255,0));
            DrawText(hdc,"    \n B: \n    ",-1,&rtRGB,DT_VCENTER|DT_CENTER);

            
            //预览区域绘制
            hBrush = CreateSolidBrush(colBar);
            hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

            Rectangle(hdc, rtPreview.left, rtPreview.top, rtPreview.right, rtPreview.bottom);

            SelectObject(hdc, hOldBrush);
            DeleteObject(hBrush);

            colBkOld = SetBkColor(hdc,RGB(nR,nG,nB));
            colTextOld = SetTextColor(hdc,RGB(255-nR,255-nG,255-nB));
            
            DrawText(hdc,IDS_PREVIEW/*GetString(STR_PUBLICCON_PREVIEW)*/,-1,&rtPreview,DT_VCENTER|DT_CENTER);

            SetBkColor(hdc,colBkOld);
            SetTextColor(hdc,colTextOld);

            

            //绘制调色板
            hPen = CreatePen(PS_SOLID,0,RGB(255,255,255));
            hOldPen = (HPEN)SelectObject(hdc,hPen);

            hOldBrush = (HBRUSH)SelectObject(hdc,NULL);

            for(i = 0 ; i < GRID_NUM ; i++)
            {
                rc.left = PAL_LEFT + i * GRID_WIDTH;
                rc.right = PAL_LEFT + (i+1) * GRID_WIDTH;
                for(j = 0 ; j < GRID_NUM ; j++)
                {
                    hBrush = CreateSolidBrush(*(*(g_Pallete+i)+j));
                    SelectObject(hdc,hBrush);
                    
                    Rectangle(hdc,PAL_LEFT + i * GRID_WIDTH,PAL_TOP + j * GRID_HEIGHT,
                        PAL_LEFT + (i+1) * GRID_WIDTH,PAL_TOP + (j+1) * GRID_HEIGHT);
                    //FillRect(hdc, &rc, hBrush);

                    SelectObject(hdc, hOldBrush);
                    DeleteObject(hBrush);
                }
            }
            
            SelectObject(hdc,hOldPen);
            DeleteObject(hPen);

            //绘制当前选中色块的边框
            if( GetFocus() == hWnd &&
                nXPendown >= 0 && nXPendown < GRID_NUM && //6 && 
                nYPendown >= 0 && nYPendown < GRID_NUM) //6 )
            {
                //画黑框
                hPen = CreatePen(PS_SOLID,2,RGB(0,0,0));
                SelectObject(hdc,hPen);

                hBrush = GetStockObject(NULL_BRUSH);
                SelectObject(hdc,hBrush);

                Rectangle(hdc,PAL_LEFT+nXPendown*GRID_WIDTH,PAL_TOP+nYPendown*GRID_HEIGHT,
                    PAL_LEFT+(nXPendown+1)*GRID_WIDTH+1,PAL_TOP+(nYPendown+1)*GRID_HEIGHT+1);

                SelectObject(hdc,hOldPen);
                DeleteObject(hPen);

            }
        
            SelectObject(hdc,hOldBrush);

            EndPaint(hWnd,NULL);
		}
		break;

    case WM_SETFOCUS:
        {
            DrawFrame(hWnd,nXPendown,nYPendown,hSpinR,hSpinG,hSpinB,&rtPreview,TRUE);
            
        }
        break;
    case WM_KILLFOCUS:
        {
            if(hLastFocus == NULL)
                DrawFrame(hWnd,nXPendown,nYPendown,hSpinR,hSpinG,hSpinB,&rtPreview,FALSE);    
        
        }
        break;
    //控件收到VK_UP
    case NSBN_KEYUP:
        {
            if((HWND)lParam == hSpinR)
            {
                nYPendown = GRID_NUM - 1;
                if(nXPendown < 0)
                {
                    nXPendown = 0;
                }
                SetFocus(hWnd);
                break;
            }
            if((HWND)lParam == hSpinG)
            {
                SetFocus(hSpinR);
                break;
            }
            if((HWND)lParam == hSpinB)
            {
                SetFocus(hSpinG);
                break;
            }
        }
        break;
    //控件收到VK_DOWN
    case NSBN_KEYDOWN:
        {
            if((HWND)lParam == hSpinR)
            {
                SetFocus(hSpinG);
                break;
            }
            if((HWND)lParam == hSpinG)
            {
                SetFocus(hSpinB);
                break;
            }
            if((HWND)lParam == hSpinB)
            {
                nYPendown = 0;
                if(nXPendown < 0)
                {
                    nXPendown = 0;
                }
                SetFocus(hWnd);
                break;
            }
        }
        break;
    //窗口收到KEYDOWN，在窗口与控件、调色板中间进行切换
    case WM_KEYDOWN :
        switch(wParam)
        {
        case VK_UP:
            {
                if(GetFocus() == hWnd)
                {
                    int nYNew;
                    
                    if((nYPendown-1) < 0) //焦点切换到hSpinB
                    {
                        
                        SetFocus(hSpinB);

                        break;
                    }
                    else  //调色板中上移一行
                    {
                        nYNew = nYPendown - 1;

                        nR = GetRValue(g_Pallete[nXPendown][nYNew]);
                        nG = GetGValue(g_Pallete[nXPendown][nYNew]);
                        nB = GetBValue(g_Pallete[nXPendown][nYNew]);

                        DrawNewSelect(hWnd,nXPendown,nYPendown,nXPendown,nYNew,
                            hSpinR,hSpinG,hSpinB,&rtPreview);

                        nYPendown = nYNew;
                        
                    }

                }
                
            }
            break;
        case VK_DOWN:
            {
                if(GetFocus() == hWnd)
                {
                    int nYNew;
                    
                    if((nYPendown+1) >= GRID_NUM)
                    {
                        hdc = GetDC(hWnd);

                        
                        SetFocus(hSpinR);

                        break;
                    }
                    else
                    {
                        nYNew = nYPendown + 1;

                        nR = GetRValue(g_Pallete[nXPendown][nYNew]);
                        nG = GetGValue(g_Pallete[nXPendown][nYNew]);
                        nB = GetBValue(g_Pallete[nXPendown][nYNew]);

                        DrawNewSelect(hWnd,nXPendown,nYPendown,nXPendown,nYNew,
                            hSpinR,hSpinG,hSpinB,&rtPreview);

                        nYPendown = nYNew;
 
                    }

                }
                
            }
            break;
        case VK_LEFT:
            {
                int nXNew;

                if( (nXPendown - 1) < 0)
                {
                    nXNew = GRID_NUM - 1;//5;
                }
                else
                {
                    nXNew = nXPendown - 1;
                }

                nR = GetRValue(g_Pallete[nXNew][nYPendown]);
                nG = GetGValue(g_Pallete[nXNew][nYPendown]);
                nB = GetBValue(g_Pallete[nXNew][nYPendown]);

                DrawNewSelect(hWnd,nXPendown,nYPendown,nXNew,nYPendown,hSpinR,hSpinG,hSpinB,&rtPreview);

                nXPendown = nXNew;
 
            }
            break;

        case VK_RIGHT:
            {
                int nXNew;

                if( (nXPendown + 1) >= GRID_NUM)//6)
                {
                    nXNew = 0;
                }
                else
                {
                    nXNew = nXPendown+1;
                }

                nR = GetRValue(g_Pallete[nXNew][nYPendown]);
                nG = GetGValue(g_Pallete[nXNew][nYPendown]);
                nB = GetBValue(g_Pallete[nXNew][nYPendown]);

                DrawNewSelect(hWnd,nXPendown,nYPendown,nXNew,nYPendown,hSpinR,hSpinG,hSpinB,&rtPreview);

                nXPendown = nXNew;
 
            }
            break;
            
        case VK_RETURN:
            p->colReturn = RGB(nR,nG,nB);
            p->bResult = TRUE;

            //DestroyWindow(hWnd);
            PostMessage(hWnd,CS_EXITLOOP,0,0);
            
            break;

        default:
            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
            return lResult;
        }
        break;

	case WM_COMMAND:
       
        switch(LOWORD(wParam))
		{
        case IDM_CONFIRM: //确定
			{
                p->colReturn = RGB(nR,nG,nB);
                p->bResult = TRUE;

                DestroyWindow(hWnd);
                PostMessage(hWnd,CS_EXITLOOP,0,0);
			}
            break;
		case IDM_EXIT:		//取消
			
            p->bResult = FALSE;
            
            DestroyWindow(hWnd);
            PostMessage(hWnd,CS_EXITLOOP,0,0);
            break;

        case IDC_SPINR:
            {
                COLORREF OldColor;
                COLORREF OldTextColor;

                if(HIWORD(wParam) == NSBN_CHANGE)
                {
                    nR = SendMessage(hSpinR,NSBM_GETDATA,0,0);
                
                    hdc = GetDC(hWnd);

                    OldColor = SetBkColor(hdc,RGB(nR,nG,nB));
                    OldTextColor = SetTextColor(hdc,RGB(255-nR,255-nG,255-nB));
            
                    DrawText(hdc,IDS_PREVIEW/*GetString(STR_PUBLICCON_PREVIEW)*/,-1,&rtPreview,DT_VCENTER|DT_CENTER);
 
                    SetBkColor(hdc,OldColor);
                    SetTextColor(hdc,OldTextColor);

                    ReleaseDC(hWnd,hdc);
                }
            }
            break;
        case IDC_SPING:
            {
                COLORREF OldColor;
                COLORREF OldTextColor;

                if(HIWORD(wParam) == NSBN_CHANGE)
                {
                    nG = SendMessage(hSpinG,NSBM_GETDATA,0,0);

                    hdc = GetDC(hWnd);

                    OldColor = SetBkColor(hdc,RGB(nR,nG,nB));
                    OldTextColor = SetTextColor(hdc,RGB(255-nR,255-nG,255-nB));
            
                    DrawText(hdc,IDS_PREVIEW/*GetString(STR_PUBLICCON_PREVIEW)*/,-1,&rtPreview,DT_VCENTER|DT_CENTER);
 
                    SetBkColor(hdc,OldColor);
                    SetTextColor(hdc,OldTextColor);

                    ReleaseDC(hWnd,hdc);
                }
            }
            break;
        case IDC_SPINB:
            {
                COLORREF OldColor;
                COLORREF OldTextColor;

                if(HIWORD(wParam) == NSBN_CHANGE)
                {
                    nB = SendMessage(hSpinB,NSBM_GETDATA,0,0);

                    hdc = GetDC(hWnd);

                    OldColor = SetBkColor(hdc,RGB(nR,nG,nB));
                    OldTextColor = SetTextColor(hdc,RGB(255-nR,255-nG,255-nB));
            
                    DrawText(hdc,IDS_PREVIEW/*GetString(STR_PUBLICCON_PREVIEW)*/,-1,&rtPreview,DT_VCENTER|DT_CENTER);
 
                    SetBkColor(hdc,OldColor);
                    SetTextColor(hdc,OldTextColor);

                    ReleaseDC(hWnd,hdc);
                }
            }
            break;
            
		}
        
        

        break;

    case WM_PENDOWN:
        {
            int x,y;
            int i,j;

            //求鼠标点中的方格
            x = LOWORD(lParam);
            y = HIWORD(lParam);

            i = (x-PAL_LEFT) / GRID_WIDTH;
            j = (y-PAL_TOP) / GRID_HEIGHT;

            if(i < 0 || i >= GRID_NUM/*6*/ || j < 0 || j >= GRID_NUM/*6*/)
            {
                break;
            }

            if(GetFocus() != hWnd)
            {
                nXPendown = i;
                nYPendown = j;
                SetFocus(hWnd);
            }

            

            nR = GetRValue(g_Pallete[i][j]);
            nG = GetGValue(g_Pallete[i][j]);
            nB = GetBValue(g_Pallete[i][j]);

            DrawNewSelect(hWnd,nXPendown,nYPendown,i,j,hSpinR,hSpinG,hSpinB,&rtPreview);

            nXPendown = i;
            nYPendown = j;

            

            
        }
		break;

	default :

        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);

        break;
	
    };

	return lResult;

}

/*********************************************************************\
* Function: MyNumSpin_WndProc   
* Purpose:  拦截NumSpin的按键消息进行自定义处理
* Params	   
* Return		   
* Remarks	   
**********************************************************************/
LRESULT CALLBACK MyNumSpin_WndProc(HWND hWnd, UINT wMsgCmd, WPARAM wParam,
                              LPARAM lParam)
{
    LRESULT lResult;
    
    switch (wMsgCmd)
    {
    case WM_KEYDOWN:
        {
            switch(wParam)
            {
            case VK_UP:   
                
                SendMessage(GetParent(hWnd),NSBN_KEYUP,0,(LPARAM)hWnd);
                lResult = 0;
                break;

            case VK_DOWN:
                
                SendMessage(GetParent(hWnd),NSBN_KEYDOWN,0,(LPARAM)hWnd);
                lResult = 0;
                break;

            default:
                
                lResult = CallWindowProc(OldNumSpinBoxProc, hWnd, wMsgCmd, wParam, lParam);
                break;

            }
            
        }
        
        break;
    default:
        
        lResult = CallWindowProc(OldNumSpinBoxProc, hWnd, wMsgCmd, wParam, lParam);
        break;
    }
    return lResult;
}

/*********************************************************************\
* Function: DrawNewSelect	   
* Purpose:  Redraw the border of current selected color grid.   
* Params:   hWnd                  Control's handle
*           nXGridOld,nYGridOld   The position of the old selected grid
*           nXGridNew,nYGridNew   The position of the new selected grid  
*           hSpinR,hSpinG,hSpinB  Handles of NumSpin controls
*           prcPreview            Point to the preview rect.
* Return	No return.	   
* Remarks	   
**********************************************************************/
void DrawNewSelect(HWND hWnd,int nXGridOld,int nYGridOld,
                   int nXGridNew,int nYGridNew,
                   HWND hSpinR,HWND hSpinG,HWND hSpinB,
                   RECT* prcPreview)
{
    //重复点同一色块，不重新绘制边框
    if( (nXGridOld != nXGridNew || nYGridOld != nYGridNew)/* && 
        i >= 0 && i < 6 && j >= 0 && j < 6*/)
    {
        DrawFrame(hWnd,nXGridOld,nYGridOld,NULL,NULL,NULL,prcPreview,FALSE);

        DrawFrame(hWnd,nXGridNew,nYGridNew,hSpinR,hSpinG,hSpinB,prcPreview,TRUE); 
     
    }
}

/*********************************************************************\
* Function	DrawFrame   
* Purpose   Redraw the selected grid   
* Params	hWnd                  Handle of the control
*           nX,nY                 Position of current selected grid
*           hSpinR,hSpinG,hSpinB  Handles of NumSpin controls
*           pRect                 Preview rect
* Return	No return.	   
* Remarks	   
**********************************************************************/
void DrawFrame(HWND hWnd,int nX,int nY,HWND hSpinR,HWND hSpinG,HWND hSpinB,RECT* pRect,BOOL bBlack)
{
    HDC hdc;
    HPEN hPen,hOldPen;
    HBRUSH hBrush,hOldBrush;
    COLORREF colBkOld,colTextOld;
    int nR,nG,nB;

    hdc = GetDC(hWnd);

    if( /*hFocus*///GetFocus() == hWnd && 
        nX >= 0 && nX < GRID_NUM/*6*/ && 
        nY >= 0 && nY < GRID_NUM/*6*/ )
    {
        if(bBlack == TRUE)
        {
            //画黑框
            hPen = CreatePen(PS_SOLID,2,RGB(0,0,0));
            hOldPen = (HPEN)SelectObject(hdc,hPen);

            hBrush = GetStockObject(NULL_BRUSH);
            hOldBrush = (HBRUSH)SelectObject(hdc,hBrush);

            Rectangle(hdc,PAL_LEFT+nX*GRID_WIDTH,PAL_TOP+nY*GRID_HEIGHT,
                PAL_LEFT+(nX+1)*GRID_WIDTH+1,PAL_TOP+(nY+1)*GRID_HEIGHT+1);

            SelectObject(hdc,hOldPen);
            DeleteObject(hPen);

            //得到选中颜色
            nR = GetRValue(g_Pallete[nX][nY]);
            nG = GetGValue(g_Pallete[nX][nY]);
            nB = GetBValue(g_Pallete[nX][nY]);

            //重新设置控件值
            SendMessage(hSpinR,NSBM_SETINITDATA,MAKELONG(0,255),MAKELONG(1,nR));
            SendMessage(hSpinG,NSBM_SETINITDATA,MAKELONG(0,255),MAKELONG(1,nG));
            SendMessage(hSpinB,NSBM_SETINITDATA,MAKELONG(0,255),MAKELONG(1,nB));
        
            //绘制预览区
            colBkOld = SetBkColor(hdc,RGB(nR,nG,nB));
            colTextOld = SetTextColor(hdc,RGB(255-nR,255-nG,255-nB));
            
            DrawText(hdc,IDS_PREVIEW/*GetString(STR_PUBLICCON_PREVIEW)*/,-1,pRect,DT_VCENTER|DT_CENTER);

            SetBkColor(hdc,colBkOld);
            SetTextColor(hdc,colTextOld);
        }
        else
        {
            //去黑框
            hPen = CreatePen(PS_SOLID,2,RGB(255,255,255));
            hOldPen = (HPEN)SelectObject(hdc,hPen);

            hBrush = GetStockObject(NULL_BRUSH);
            hOldBrush = (HBRUSH)SelectObject(hdc,hBrush);

            Rectangle(hdc,PAL_LEFT+nX*GRID_WIDTH,PAL_TOP+nY*GRID_HEIGHT,
                PAL_LEFT+(nX+1)*GRID_WIDTH+1,PAL_TOP+(nY+1)*GRID_HEIGHT+1);

            SelectObject(hdc,hOldPen);
            DeleteObject(hPen);
        }
             
    }
    
    ReleaseDC(hWnd,hdc);

    return;
}

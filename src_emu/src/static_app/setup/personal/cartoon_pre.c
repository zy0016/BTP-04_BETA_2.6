/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : 
 *
 * Purpose  : 动画预览
 *            
\**************************************************************************/
//
//#include    "string.h"
//#include    "winpda.h"
//#include    "setting.h"
//#include    "plx_pdaex.h"
//#include    "hpimage.h"
//
//#define     IDC_INIT                WM_USER + 100
//
//static const char * pClassName = "Cartoon_PreWndClass";
//static HINSTANCE    hInstance;
//
//static char         cFileName[CARTOONFILENAMELEN + 1] = "";
//
//static  LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam );
//static  void    PaintBkFunc(HGIFANIMATE hGIFAnimate, int x, int y, HDC hdcMem);
//
//BOOL    CallCartoon_PreWindow(HWND hwndCall,const char * cfilename)
//{
//    HWND        hwnd;
//    WNDCLASS    wc;
//
//    if (cfilename == NULL)
//        return FALSE;
//
//    wc.style         = CS_OWNDC;
//    wc.lpfnWndProc   = AppWndProc;
//    wc.cbClsExtra    = 0;
//    wc.cbWndExtra    = 0;
//    wc.hInstance     = NULL;
//    wc.hIcon         = NULL;
//    wc.hCursor       = NULL;
//    wc.hbrBackground = NULL;//GetStockObject(WHITE_BRUSH);
//    wc.lpszMenuName  = NULL;
//    wc.lpszClassName = pClassName;
//
//    memset(cFileName,0,sizeof(cFileName));
//    strcpy(cFileName,cfilename);
//
//    if (!RegisterClass(&wc))
//    {
//        return FALSE;
//    }
//
//    hwnd = CreateWindow(pClassName,"",WS_VISIBLE,0,0,
//        GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN),
//        hwndCall,NULL,NULL,NULL);
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
//    return (TRUE);
//}
//static LRESULT AppWndProc ( HWND hWnd, UINT wMsgCmd, WPARAM wParam, LPARAM lParam )
//{
//    static      HGIFANIMATE hgifanimate;
//                RECT        rect;
//                HDC         hMemdc = NULL;
//                SIZE        size;
//                LRESULT     lResult;
//                HDC         hdc;
//                PAINTSTRUCT ps;
//
//    lResult = (LRESULT)TRUE;
//    switch ( wMsgCmd )
//    {
//    case WM_CREATE :
//        PostMessage(hWnd,IDC_INIT,0,0);
//        break;
//
//    case IDC_INIT:
//        GetImageDimensionFromFile(cFileName,&size);
//
//        hgifanimate = StartAnimatedGIFFromFile(hWnd,cFileName,
//            (GetSystemMetrics(SM_CXSCREEN) - size.cx) / 2,
//            (GetSystemMetrics(SM_CYSCREEN) - size.cy) / 2,DM_NONE);
//        
//        SetPaintBkFunc(hgifanimate, (PAINTBKFUNC)PaintBkFunc);
//
//        hdc = GetDC(hWnd);
//        PaintAnimatedGIF (hdc, hgifanimate);
//        ReleaseDC(hWnd, hdc);
//
//        
//        InvalidateRect(hWnd,NULL,TRUE);
//        break;
//
//    case WM_ACTIVATE://去掉这个消息，单击右键可以退出，否则退出失败
//        if (WA_INACTIVE == LOWORD(wParam))
//        {
//            PostMessage(hWnd,WM_PENDOWN,0,0);
//        }
//        else
//        {
//            SetFocus(hWnd);//激活后设置焦点
//            lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        }
//        break;
//
//    case WM_DESTROY ://通知程序管理器,此程序退出;
//        EndAnimatedGIF(hgifanimate);
//        UnregisterClass(pClassName,NULL);
//        DlmNotify ((WPARAM)PES_STCQUIT, (LPARAM)hInstance );
//        break;
//
//    case WM_PAINT :
//        hdc = BeginPaint( hWnd, &ps);
//
//        hMemdc = CreateMemoryDC(GetSystemMetrics(SM_CXSCREEN),GetSystemMetrics(SM_CYSCREEN));
//        GetClientRect(hWnd, &rect);
//        ClearRect(hMemdc, &rect, RGB(255, 255, 255));
//        PaintAnimatedGIF (hMemdc, hgifanimate);
//        SetBkMode(hMemdc, BM_TRANSPARENT);
//
//        BitBlt(hdc, 0, 0, rect.right, rect.bottom,hMemdc, 0, 0, SRCCOPY);   
//        DeleteDC(hMemdc);
//        hMemdc = NULL;
//
//        //PaintAnimatedGIF(hdc,hgifanimate);
//        EndPaint(hWnd, &ps);
//        break;
//        
//    case WM_KEYDOWN:
//    case WM_PENDOWN:
//        DestroyWindow(hWnd);
//        break;
//    default :
//        lResult = PDADefWindowProc(hWnd, wMsgCmd, wParam, lParam);
//        break;
//    }
//    return lResult;
//}
//// 画gif的回调函数，刷新背景
//static void PaintBkFunc(HGIFANIMATE hGIFAnimate, int x, int y, HDC hdcMem)
//{
//    /*int width, height;
//    RECT   rect;
//
//    width = GetDeviceCaps(hdcMem, HORZRES);
//    height = GetDeviceCaps(hdcMem, VERTRES);
//
//    rect.bottom = height;
//    rect.left = 0;
//    rect.top = 0;
//    rect.right = width;
//
//    ClearRect(hdcMem, &rect, RGB(255, 255, 255));*/
//}

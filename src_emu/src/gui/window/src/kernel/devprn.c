/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : 打印逻辑设备管理模块.
 *            
\**************************************************************************/

#include "hpwin.h"

#if (PRINTERSUPPORT)

#include "devprn.h"
#include "string.h"

#define EV_BANDEND      0xa
#define EV_PAGEEND      0xb
#define EV_PAPEROUT     0xc
#define EV_UNLINK       0xd

#define WM_PRINTNOTIFY  WM_USER

static HWND hAbortDlg;
static BOOL bSyncPrint; // 同步打印标志

#define DLG_ITEMS_NUM 2

typedef struct 
{
    DWORD   type;               // Dialog template type, must be zero
    DWORD   style;              // Dialog style
    DWORD   dwExStyle;          // Dialog extend style
    DWORD   cdit;               // Count of dialog control items
    short   x;                  // x-coordinates of dialog left-top corner
    short   y;                  // y-coordinates of dialog left-top corner
    short   cx;                 // width of dialog left-top corner
    short   cy;                 // height of dialog left-top corner
    PCSTR   lpszMenuName;       // Dialog menu name
    PCSTR   lpszClassName;      // Dialog class name
    PCSTR   lpszWindowText;     // Dialog title
    DLGITEMTEMPLATE dlgItems[DLG_ITEMS_NUM];
} ABORTDLGTEMPLATE;

const ABORTDLGTEMPLATE AbortDlgTemplate = 
{
    0, 
    WS_VISIBLE | WS_DLGFRAME | WS_CAPTION, 
    0, 
    DLG_ITEMS_NUM, 
    160, 
    120, 
    320, 
    240, 
    NULL, 
    NULL, 
    "打印", 
    {
        {
            WS_VISIBLE | SS_CENTER, 
            0, 
            0, 
            60, 
            320, 
            20, 
            0,
            "STATIC", 
            "正在打印，按取消按钮停止...", 
        }, 
        {
            WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON, 
            0, 
            120, 
            150, 
            72, 
            24, 
            IDCANCEL,
            "BUTTON", 
            "取消"
        }
    }
};

/* Golbal varible for dirplay driver interface */
PRINTERDRVINFO   g_PrinterDrv;       // Printer driver interface

// Internal function prototype
static UINT EndCallback (LONG lUserData, DWORD dwEventMask, DWORD param1, 
                         LONG param2);
static BOOL CALLBACK AbortDlgProc(HWND hDlg, UINT message, WPARAM wParam, 
                                  LPARAM lParam);
static int WaitPrintJobEnd(void* pDev);

/*
**  Function : PRNDEV_Init
**  Purpose  :
**      Get printer driver interface and initialize printer device. This
**      function must be called when window system initialized.
**  Params   :
**      None.
**  Return   :
**      If the function succeeds, return TRUE; If the function fails, 
**      return FALSE;
*/
BOOL PRNDEV_Init(PGRAPHDEV pGraphDev)
{
    PPRINTERDRVINFO pPrinterDrv;
    // Get printer driver interface
// EnablePrinterDev(&g_PrinterDrv);
    pPrinterDrv = EnablePrinterDev(NULL);

    /*Initial the graphic device driver interface */
    memset(&(pGraphDev->drv), NULL, sizeof(GRAPHDEVDRV));
    pGraphDev->drv.devices              = pPrinterDrv->devices;
    pGraphDev->drv.func_mask1           = pPrinterDrv->func_mask1;
    pGraphDev->drv.func_mask2           = pPrinterDrv->func_mask2;
    pGraphDev->drv.OpenDevice           = pPrinterDrv->OpenDevice;
    pGraphDev->drv.CloseDevice          = pPrinterDrv->CloseDevice;
    pGraphDev->drv.SuspendDevice        = pPrinterDrv->SuspendDevice;
    pGraphDev->drv.ResumeDevice         = pPrinterDrv->ResumeDevice;
    pGraphDev->drv.GetDevInfo           = pPrinterDrv->GetDevInfo;
    pGraphDev->drv.CreateBitmap         = pPrinterDrv->CreateBitmap;
    pGraphDev->drv.DestroyBitmap        = pPrinterDrv->DestroyBitmap;
    pGraphDev->drv.SetBitmapBuffer      = pPrinterDrv->SetBitmapBuffer;
    pGraphDev->drv.RealizePen           = pPrinterDrv->RealizePen;
    pGraphDev->drv.UnrealizePen         = pPrinterDrv->UnrealizePen;
    pGraphDev->drv.RealizeBrush         = pPrinterDrv->RealizeBrush;
    pGraphDev->drv.UnrealizeBrush       = pPrinterDrv->UnrealizeBrush;
    pGraphDev->drv.RealizeColor         = pPrinterDrv->RealizeColor;
    pGraphDev->drv.DrawGraph            = pPrinterDrv->DrawGraph;
    pGraphDev->drv.BitBlt               = pPrinterDrv->BitBlt;
    pGraphDev->drv.PatBlt               = pPrinterDrv->PatBlt;
    pGraphDev->drv.DIBitBlt             = pPrinterDrv->DIBitBlt;
    pGraphDev->drv.TextBlt              = pPrinterDrv->TextBlt;
    pGraphDev->drv.StretchBlt           = pPrinterDrv->StretchBlt;
    pGraphDev->drv.DIStretchBlt         = pPrinterDrv->DIStretchBlt;

    pGraphDev->drv.SetPageSize          = pPrinterDrv->SetPageSize;
    pGraphDev->drv.StartPrint           = pPrinterDrv->StartPrint;
    pGraphDev->drv.EndPrint             = pPrinterDrv->EndPrint;
    pGraphDev->drv.StartPage            = pPrinterDrv->StartPage;
    pGraphDev->drv.EndPage              = pPrinterDrv->EndPage;
    pGraphDev->drv.StartBand            = pPrinterDrv->StartBand;
    pGraphDev->drv.EndBand              = pPrinterDrv->EndBand;
    pGraphDev->drv.ResumePrint          = pPrinterDrv->ResumePrint;
    pGraphDev->drv.CancelPrint          = pPrinterDrv->CancelPrint;
    pGraphDev->drv.SetCallback          = pPrinterDrv->SetCallback;

    return TRUE;
}
#if 0
BOOL PRNDEV_Init(void)
{
    // Get printer driver interface
    EnablePrinterDev(&g_PrinterDrv);
    return TRUE;
}
#endif
/*
**  Function : PRNDEV_Exit
**  Purpose  :
**      Close the printer device and free the memory space of printer 
**      device. This function must be called when the window system 
**      shut down.
*/
void PRNDEV_Exit(PGRAPHDEV pGraphDev)
{
}
#if 0
void PRNDEV_Exit(void)
{
}
#endif
/*
**  Function : PRNDEV_GetCaps
**  Purpose  :
**      Retrieves device-specific information about printer device. 
**  Params   :
**      Specifies the item to return, this parameter can be one of the 
**      following values:
**          HORZSIZE  : Width, in millimeters, of the physical screen. 
**          VERTSIZE  : Height, in millimeters, of the physical screen. 
**          HORZRES   : Width, in pixels, of the screen. 
**          VERTRES   : Height, in pixels, of the screen. 
**          BITSPIXEL : Number of adjacent color bits for each pixel.
**          PLANES    : Number of color planes. 
**  Return   :
**      Return the value of the desired item.
*/
int PRNDEV_GetCaps(void* pDev, int nIndex)
{
    int nRet;
    PRINTERDEVINFO DevInfo;

    g_PrinterDrv.GetDevInfo(pDev, &DevInfo);

    nRet = 0;

    switch (nIndex)
    {
    case HORZSIZE :

        nRet = DevInfo.phys_width;
        break;

    case VERTSIZE :

        nRet = DevInfo.phys_height;
        break;

    case HORZRES :

        nRet = DevInfo.width;
        break;

    case VERTRES :

        nRet = DevInfo.height;
        break;

    case BITSPIXEL :

        nRet = DevInfo.bits_pixel;
        break;

    case PLANES :

        nRet = DevInfo.planes;
        break;
    }

    return nRet;
}

void* PRNDEV_CreatePrinterDev(int width, int height)
{
    int nRet;
    void* pDev;

    // Get size of printer device struct

    nRet = g_PrinterDrv.OpenDevice(NULL, 1);
    ASSERT(nRet > 0);

    if (nRet <= 0)
    {
        SetLastError(-nRet);
        return NULL;
    }

    // Allocate memory for printer device struct pointer

    pDev = (PDEV)MemAlloc(nRet);
    ASSERT(pDev != NULL);

    if (!pDev)
        return NULL;

    // Open the printer device
    if (g_PrinterDrv.OpenDevice(pDev, 1) <= 0)
    {
        MemFree(pDev);
        return NULL;
    }

    if (g_PrinterDrv.SetPageSize(pDev, width, height) < 0)
    {
        g_PrinterDrv.CloseDevice(pDev);
        MemFree(pDev);
        return NULL;
    }

    if (g_PrinterDrv.SetCallback(pDev, (DWORD)EndCallback) <= 0)
        bSyncPrint = TRUE;

    return pDev;
}

/*
**  Function : PRNDEV_DestroyPrinterDev
**  Purpose  :
**      
*/
void PRNDEV_DestroyPrinterDev(void* pDev)
{
    g_PrinterDrv.CloseDevice(pDev);
    MemFree(pDev);
}

/*
**  Function : PRNDEV_StartDoc
**  Purpose  :
*/
int PRNDEV_StartDoc(void* pDev, const DOCINFO* pDocInfo)
{
    if (!bSyncPrint)
    {
#ifndef _EMULATE_
        
        hAbortDlg = CreateDialogIndirect(NULL, 
            (PCDLGTEMPLATE)&AbortDlgTemplate, NULL, AbortDlgProc);
        
        if (!hAbortDlg)
            return 0;
        
        SetSysModalWindow(hAbortDlg);
        
#else
        
        hAbortDlg = (HWND)1;
        
#endif
    }

    return g_PrinterDrv.StartPrint(pDev);
}

/*
**  Function : PRNDEV_EndDoc
**  Purpose  :
*/
int PRNDEV_EndDoc(void* pDev)
{
    if (hAbortDlg)
    {
        DestroyWindow(hAbortDlg);
        hAbortDlg = NULL;
    }

    return g_PrinterDrv.EndPrint(pDev);
}

/*
**  Function : PRNDEV_StartPage
**  Purpose  :
*/
int PRNDEV_StartPage(void* pDev)
{
    return g_PrinterDrv.StartPage(pDev);
}

/*
**  Function : PRNDEV_EndPage
**  Purpose  :
*/
int PRNDEV_EndPage(void* pDev)
{
    int nRet;

    nRet = g_PrinterDrv.EndPage(pDev);
    if (nRet <= 0)
        return nRet;

    if (!bSyncPrint)
        return WaitPrintJobEnd(pDev);

    return nRet;
}

/*
**  Function : PRNDEV_StartBand
**  Purpose  :
*/
int PRNDEV_StartBand(void* pDev, PRECT pRect)
{
    return g_PrinterDrv.StartBand(pDev, pRect);
}

/*
**  Function : PRNDEV_EndBand
**  Purpose  :
*/
int PRNDEV_EndBand(void* pDev)
{
    int nRet;

    nRet = g_PrinterDrv.EndBand(pDev);
    if (nRet <= 0)
        return nRet;

    if (!bSyncPrint)
        return WaitPrintJobEnd(pDev);

    return nRet;
}

static UINT EndCallback (LONG lUserData, DWORD dwEventMask, DWORD param1, 
                         LONG param2)
{
	switch (dwEventMask)
	{
    case EV_UNLINK:
    {
        PostMessage(NULL, WM_PRINTNOTIFY, EV_UNLINK, 0);
        break;
    }

    case EV_PAPEROUT:
    {
        PostMessage(NULL, WM_PRINTNOTIFY, EV_PAPEROUT, 0);
        break;
    }

    case EV_BANDEND:
    {
        PostMessage(NULL, WM_PRINTNOTIFY, EV_BANDEND, 0);
        break;
    }

    case EV_PAGEEND:
    {
        PostMessage(NULL, WM_PRINTNOTIFY, EV_PAGEEND, 0);
        break;
    }

    default:

        break;
	}	// switch (dwEventMask)

	return 0 ;
}

static int WaitPrintJobEnd(void* pDev)
{
    MSG msg;
    int nResult;

    // Enter dialog message loop
    while (hAbortDlg)
    {
        GetMessage(&msg, 0, 0, 0);

        if (!msg.hwnd && msg.message == WM_PRINTNOTIFY)
        {
            switch (msg.wParam)
            {
            case EV_BANDEND :
            case EV_PAGEEND :

                // 成功返回
                return 1;

            case EV_UNLINK :
                
                nResult = MessageBox(NULL, 
                    "打印机未联机, 按重试继续\n\n打印, 取消按钮取消打印！", 
                    "提示", MB_RETRYCANCEL);
                
                break;;
                
            case EV_PAPEROUT :
                
                nResult = MessageBox(NULL, 
                    "打印机缺纸, 请安装打印纸! 按重试\n\n"
                    "按钮继续打印, 取消按钮取消打印！", 
                    "提示", MB_RETRYCANCEL);
                
                break;
            }

            // 用户取消返回
            if (nResult == IDCANCEL)
                break;

            // 用户选择retry，通知打印驱动程序继续
            g_PrinterDrv.ResumePrint(pDev);
        }

        if (!IsDialogMessage(hAbortDlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // User cancel the print job
    g_PrinterDrv.CancelPrint(pDev);

    if (hAbortDlg)
        DestroyWindow(hAbortDlg);

    return -1;
}

static BOOL CALLBACK AbortDlgProc(HWND hDlg, UINT message, WPARAM wParam, 
                                  LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND :

        DestroyWindow(hDlg);
        return TRUE;

    case WM_DESTROY :

        hAbortDlg = NULL;
        break;
    }

    return FALSE;
}

#else // PRINTERSUPPRT

// To avoid warning C4206: nonstandard extension used : translation unit 
// is empty
static void UnusedFunc(void)
{
}

#endif

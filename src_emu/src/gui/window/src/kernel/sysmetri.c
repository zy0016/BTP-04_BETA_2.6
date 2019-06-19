/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Define system metrics table and implements GetSystemMetrics 
 *            function
 *            
\**************************************************************************/

#ifndef NOSYSMETRICS

#include "hpwin.h"
#include "wsthread.h"

#define MAX_ITEM_COUNT  (SM_CYMINTRACK + 1)

static int SystemMetricsTable[MAX_ITEM_COUNT];

/*
**  Function : WS_InitSystemMetrics
**  Purpose  :
**      Initializes the system metrics table according to the current 
**      display mode. This function must be called when the window system
**      is being initialized.
**  Params   :
**      None.
**  Remarks  :
**      This function must be called after the display device is 
**      initialized. Because the system metrics initialization use the
**      display device infomation.
*/
void WS_InitSystemMetrics(void)
{
    int nScrW, nScrH;

    // Gets the width and height of the screen(pixels)
    LEAVEMONITOR;
    nScrW = GetDeviceCaps(NULL, HORZRES);
    nScrH = GetDeviceCaps(NULL, VERTRES);
    ENTERMONITOR;

    // 系统度量(System Metrics), 指窗口系统中各种显示元素的宽和高, 与显示
    // 设备的尺寸相关. 下面就根据显示设备的分辨率设置系统度量, 设置的原则
    // 是以320x240为标准, 显示设备的分辨率小于等于320x240, 采用小尺寸配置
    // 方案, 一般为PDA设备. 显示设备的分辨率大于320x240, 采用大尺寸配置方
    // 案, 一般为STB设备.
    
    if (nScrW > 320 && nScrH > 240)
    {
        SystemMetricsTable[0]   = nScrW;        // SM_CXSCREEN
        SystemMetricsTable[1]   = nScrH;        // SM_CYSCREEN
        SystemMetricsTable[2]   = 16;           // SM_CXVSCROLL
        SystemMetricsTable[3]   = 16;           // SM_CYHSCROLL
        SystemMetricsTable[4]   = 19;           // SM_CYCAPTION
        SystemMetricsTable[5]   = 1;            // SM_CXBORDER
        SystemMetricsTable[6]   = 1;            // SM_CYBORDER
        SystemMetricsTable[7]   = 4;            // SM_CXDLGFRAME
        SystemMetricsTable[8]   = 4;            // SM_CYDLGFRAME
        SystemMetricsTable[9]   = 16;           // SM_CYVTHUMB
        SystemMetricsTable[10]  = 16;           // SM_CXHTHUMB
        SystemMetricsTable[11]  = 32;           // SM_CXICON
        SystemMetricsTable[12]  = 32;           // SM_CYICON
        SystemMetricsTable[13]  = 32;           // SM_CXCURSOR
        SystemMetricsTable[14]  = 32;           // SM_CYCURSOR
        SystemMetricsTable[15]  = 19;           // SM_CYMENU
        SystemMetricsTable[16]  = nScrW;        // SM_CXFULLSCREEN
        SystemMetricsTable[17]  = nScrH;        // SM_CYFULLSCREEN
        SystemMetricsTable[18]  = 16;           // SM_CYVSCROLL
        SystemMetricsTable[19]  = 16;           // SM_CXHSCROLL
        SystemMetricsTable[20]  = 108;          // SM_CXMIN
        SystemMetricsTable[21]  = 27;           // SM_CYMIN
        SystemMetricsTable[22]  = 18;           // SM_CXSIZE
        SystemMetricsTable[23]  = 18;           // SM_CYSIZE
        SystemMetricsTable[24]  = 4;            // SM_CXFRAME
        SystemMetricsTable[25]  = 4;            // SM_CYFRAME
        SystemMetricsTable[26]  = 16;           // SM_CXMINTRACK
        SystemMetricsTable[27]  = 16;           // SM_CYMINTRACK
    }
    else
    {
        SystemMetricsTable[0]   = nScrW;        // SM_CXSCREEN
        SystemMetricsTable[1]   = nScrH;        // SM_CYSCREEN
        SystemMetricsTable[2]   = 5;           // SM_CXVSCROLL
        SystemMetricsTable[3]   = 5;           // SM_CYHSCROLL
        SystemMetricsTable[4]   = CAPTIONHEIGHT;// SM_CYCAPTION
        SystemMetricsTable[5]   = 1;            // SM_CXBORDER
        SystemMetricsTable[6]   = 1;            // SM_CYBORDER
        SystemMetricsTable[7]   = 3;            // SM_CXDLGFRAME
        SystemMetricsTable[8]   = 3;            // SM_CYDLGFRAME
        SystemMetricsTable[9]   = 9;            // SM_CYVTHUMB
        SystemMetricsTable[10]  = 9;            // SM_CXHTHUMB
        SystemMetricsTable[11]  = 32;           // SM_CXICON
        SystemMetricsTable[12]  = 32;           // SM_CYICON
        SystemMetricsTable[13]  = 2;           // SM_CXCURSOR
        SystemMetricsTable[14]  = 22;           // SM_CYCURSOR
        SystemMetricsTable[15]  = 24;           // SM_CYMENU
        SystemMetricsTable[16]  = nScrW;        // SM_CXFULLSCREEN
        SystemMetricsTable[17]  = nScrH;        // SM_CYFULLSCREEN
        SystemMetricsTable[18]  = 4;            // SM_CYVSCROLL
        SystemMetricsTable[19]  = 4;            // SM_CXHSCROLL
        SystemMetricsTable[20]  = 27;           // SM_CXMIN
        SystemMetricsTable[21]  = 27;           // SM_CYMIN
        SystemMetricsTable[22]  = 17;           // SM_CXSIZE
        SystemMetricsTable[23]  = 17;           // SM_CYSIZE
        SystemMetricsTable[24]  = 4;            // SM_CXFRAME
        SystemMetricsTable[25]  = 4;            // SM_CYFRAME
        SystemMetricsTable[26]  = 9;            // SM_CXMINTRACK
        SystemMetricsTable[27]  = 9;            // SM_CYMINTRACK
    }
}

/*
**  Function : GetSystemMetrics
**  Purpose  :
**      Retrieves various system metrics and system configuration settings. 
**  Params   :
**      nIndex : Specifies the system metric or configuration setting to 
**               retrieve. All SM_CX* values are widths. All SM_CY* values 
**               are heights. 
**  Return   :
**      If the function succeeds, return the requested system metric or 
**      configuration setting. If the function fails, return zero. 
*/
int WINAPI GetSystemMetrics(int nIndex)
{
    if (nIndex < 0 || nIndex > MAX_ITEM_COUNT)
        return 0;

    // The resolution may be changed, so we call GetDeviceCaps to get the 
    // width and height of screen.

    if (nIndex == SM_CXSCREEN)
        return GetDeviceCaps(NULL, HORZRES);

    if (nIndex == SM_CYSCREEN)
        return GetDeviceCaps(NULL, VERTRES);

    return SystemMetricsTable[nIndex];
}

#endif  /* NOSYSMETRICS */

/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Implements GDI API functions.
 *            
\**************************************************************************/

#include "hpwin.h"

#include "string.h"

#include "wsobj.h"
#include "wsownd.h"
#include "wsgraph.h"

#include "wsthread.h"

/**************************************************************************/
/*              Color support                                             */
/**************************************************************************/

// Define system color table

#if (INTERFACE_MONO)
COLORREF SysColorTable[] = 
{
    RGB(  0,   0,   0),      // COLOR_SCROLLBAR
    RGB(255, 255, 255),      // COLOR_BACKGROUND
    RGB(  0,   0,   0),      // COLOR_ACTIVECAPTION
    RGB(  0,   0,   0),      // COLOR_INACTIVECAPTION
    RGB(239, 241, 246),      // COLOR_MENU
    RGB(255, 255, 255),      // COLOR_WINDOW
    RGB(  0,   0,   0),      // COLOR_WINDOWFRAME
    RGB(  0,   0,   0),      // COLOR_MENUTEXT
    RGB(  0,   0,   0),      // COLOR_WINDOWTEXT
    RGB(255, 255, 255),      // COLOR_CAPTIONTEXT
    RGB(  0,   0,   0),      // COLOR_ACTIVEBORDER
    RGB(  0,   0,   0),      // COLOR_INACTIVEBORDER
    RGB(  0,   0,   0),      // COLOR_HIGHLIGHT
    RGB(255, 255, 255),      // COLOR_HIGHLIGHTTEXT
    RGB(255, 255, 255),      // COLOR_BTNFACE
    RGB(  0,   0,   0),      // COLOR_BTNSHADOW
    RGB(255, 255, 255),      // COLOR_GRAYTEXT
    RGB(  0,   0,   0),      // COLOR_BTNTEXT
    RGB(255, 255, 255),      // COLOR_INACTIVECAPTIONTEXT
    RGB(255, 255, 255)       // COLOR_BTNHIGHLIGHT
};
#else // INTERFACE_MONO
COLORREF SysColorTable[] = 
{
    RGB(192, 192, 192),      // COLOR_SCRLBAR
    RGB(255, 255, 255),      // COLOR_BACKGROUND
    RGB(  0,   0, 128),      // COLOR_ACTIVECAPTION
    RGB(128, 128, 128),      // COLOR_INACTIVECAPTION
    RGB(192, 192, 192),      // COLOR_MENU
    RGB(255, 255, 255),      // COLOR_WINDOW
    RGB(192, 192, 192),      // COLOR_WINDOWFRAME
    RGB(  0,   0,   0),      // COLOR_MENUTEXT
    RGB(  0,   0,   0),      // COLOR_WINDOWTEXT
    RGB(255, 255, 255),      // COLOR_CAPTIONTEXT
    RGB(192, 192, 192),      // COLOR_ACTIVEBORDER
    RGB(192, 192, 192),      // COLOR_INACTIVEBORDER
    RGB(  0,   0, 128),      // COLOR_HIGHLIGHT
    RGB(255, 255, 255),      // COLOR_HIGHLIGHTTEXT
    RGB(192, 192, 192),      // COLOR_BTNFACE
    RGB(128, 128, 128),      // COLOR_BTNSHADOW
    RGB(128, 128, 128),      // COLOR_GRAYTEXT
    RGB(  0,   0,   0),      // COLOR_BTNTEXT
    RGB(255, 255, 255),      // COLOR_INACTIVECAPTIONTEXT
    RGB(192, 192, 192)       // COLOR_BTNHIGHLIGHT
};
#endif // INTERFACE_MONO

/*
COLORREF SysColorTable[] = 
{
    RGB(192, 192, 192),      // COLOR_SCROLLBAR
    RGB(255, 255, 255),      // COLOR_BACKGROUND
    RGB(255, 255, 255),      // COLOR_ACTIVECAPTION
    RGB(255, 255, 255),      // COLOR_INACTIVECAPTION
    RGB(255, 255, 255),      // COLOR_MENU
    RGB(255, 255, 255),      // COLOR_WINDOW
    RGB(192, 192, 192),      // COLOR_WINDOWFRAME
    RGB(  0,   0,   0),      // COLOR_MENUTEXT
    RGB(  0,   0,   0),      // COLOR_WINDOWTEXT
    RGB(  0,   0,   0),      // COLOR_CAPTIONTEXT
    RGB(192, 192, 192),      // COLOR_ACTIVEBORDER
    RGB(192, 192, 192),      // COLOR_INACTIVEBORDER
    RGB(  0,   0, 128),      // COLOR_HIGHLIGHT
    RGB(255, 255, 255),      // COLOR_HIGHLIGHTTEXT
    RGB(192, 192, 192),      // COLOR_BTNFACE
    RGB(128, 128, 128),      // COLOR_BTNSHADOW
    RGB(192, 192, 192),      // COLOR_GRAYTEXT
    RGB(  0,   0,   0),      // COLOR_BTNTEXT
    RGB(  0,   0,   0),      // COLOR_INACTIVECAPTIONTEXT
    RGB(192, 192, 192)       // COLOR_BTNHIGHLIGHT
};


COLORREF SysColorTable[] = 
{
    RGB(192, 192, 192),      // COLOR_SCROLLBAR
    RGB(255, 255, 255),      // COLOR_BACKGROUND
    RGB(  0,   0, 128),      // COLOR_ACTIVECAPTION
    RGB(164, 164, 192),      // COLOR_INACTIVECAPTION
    RGB(255, 255, 255),      // COLOR_MENU
    RGB(255, 255, 255),      // COLOR_WINDOW
    RGB(192, 192, 192),      // COLOR_WINDOWFRAME
    RGB(  0,   0,   0),      // COLOR_MENUTEXT
    RGB(  0,   0,   0),      // COLOR_WINDOWTEXT
    RGB(255, 255, 255),      // COLOR_CAPTIONTEXT
    RGB(192, 192, 192),      // COLOR_ACTIVEBORDER
    RGB(192, 192, 192),      // COLOR_INACTIVEBORDER
    RGB(  0,   0, 128),      // COLOR_HIGHLIGHT
    RGB(255, 255, 255),      // COLOR_HIGHLIGHTTEXT
    RGB(192, 192, 192),      // COLOR_BTNFACE
    RGB(128, 128, 128),      // COLOR_BTNSHADOW
    RGB(192, 192, 192),      // COLOR_GRAYTEXT
    RGB(  0,   0,   0),      // COLOR_BTNTEXT
    RGB(240, 240, 240),      // COLOR_INACTIVECAPTIONTEXT
    RGB(192, 192, 192)       // COLOR_BTNHIGHLIGHT
};
*/

/*
**  Function : GetSysColor
**  Purpose  :
**      Retrieves the current color of the specified display element. 
**      Display elements are the parts of a window and the Windows 
**      display that appear on the system display screen. 
**  Params   :
**      nIndex : Specifies the display element whose color is to be 
**      retrieved.
**  Return  :
**      If the function succeeds, the return value is the red, green, 
**      blue (RGB) color value that specifies the color of the given 
**      element. 
*/
COLORREF WINAPI GetSysColor(int nIndex)
{
    if (nIndex < 0)
        nIndex = 0;

    if (nIndex > COLOR_MAX)
        nIndex = COLOR_MAX;

    return SysColorTable[(int)nIndex];
}

/*
**  Function : SetSysColors
**  Purpose  :
**      Sets the colors for one or more display elements. Display elements 
**      are the various parts of a window and the Windows display that 
**      appear on the system display screen.
*/
BOOL WINAPI SetSysColors(int nCount, const int* pIndexs, 
                         const COLORREF* pColors)
{
    int i;

    if (!pIndexs || !pColors)
        return FALSE;

    if (nCount > COLOR_MAX)
        nCount = COLOR_MAX;

    for (i = 0; i < nCount; i++)
    {
        SysColorTable[pIndexs[i]] = pColors[i];
    }

    return TRUE;
}

/**************************************************************************/
/*              DC Management                                             */
/**************************************************************************/
/*
**  Function : CreateDC
**  Purpose  :
**      Creates a device context.
**  Return   :
**      If the function succeeds, return the handle of the  
**      device context. If the function fails, return NULL.
*/
HDC WINAPI CreateDC(LPCTSTR lpszDriver,      // driver name
                  LPCTSTR lpszDevice,        // device name
                  LPCTSTR lpszOutput,        // not used; should be NULL
                  const void* lpInitData  // optional printer data
                  )
{
    PDC pDC;

    ENTERMONITOR;

    pDC = DC_CreateDC(lpszDriver, lpszDevice, lpszOutput, lpInitData);

    LEAVEMONITOR;

    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    return (HDC)WOT_GetHandle((PGDIOBJ)pDC);
}

/*
**  Function : CreateScreenDC
**  Purpose  :
**      Creates a screen device context.
**  Return   :
**      If the function succeeds, return the handle of the screen 
**      device context. If the function fails, return NULL.
*/
HDC WINAPI CreateScreenDC(void)
{
    PDC pDC;

    ENTERMONITOR;

    pDC = DC_CreateScreenDC();

    LEAVEMONITOR;

    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    return (HDC)WOT_GetHandle((PGDIOBJ)pDC);
}

/*
**  Function : CreateCompatibleDC
**  Purpose  :
**      Creates a memory device context (DC) compatible with the 
**      specified device. 
**  Params   :
**      hdc  : Identifies the device context. If this handle is NULL, 
**             the function creates a memory device context compatible 
**             with the application’s current screen. 
**  Return   :
**      If the function succeeds, return the handle to a memory device 
**      context. If the function fails, return NULL. 
*/
HDC WINAPI CreateCompatibleDC(HDC hdc)
{
    PDC pDC, pOldDC;

    if (hdc)
    {
        pOldDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
        
        if (!pOldDC)
        {
            SetLastError(1);
            return NULL;
        }
    }
    else
        pOldDC = NULL;

    ENTERMONITOR;

    pDC = DC_CreateCompatibleDC(pOldDC);

    LEAVEMONITOR;

    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    return (HDC)WOT_GetHandle((PGDIOBJ)pDC);
}

/*
**  Function : CreateMemoryDC
**  Purpose  :
**      Creates a memory device context (DC) compatible with the 
**      specified device. 
**  Params   :
**      nWidth  : Specifies the width of the memory DC.
**      nHeight : Specifies the height of the memory DC.
**  Return   :
**      If the function succeeds, return the handle to a memory device 
**      context. If the function fails, return NULL. 
*/
HDC WINAPI CreateMemoryDC(int nWidth, int nHeight)
{
    PDC pDC;

    ENTERMONITOR;

    pDC = DC_CreateMemoryDC(nWidth, nHeight);

    LEAVEMONITOR;

    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    return (HDC)WOT_GetHandle((PGDIOBJ)pDC);
}

/*
**  Function : DeleteDC
**  Purpose  :
**      Deletes the specified device context (DC).
**  Params   :
**      hdc  : Identifies the device context. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**      If the function fails, the return value is zero. 
**  Remark   :
**      An application must not delete a device context whose handle was 
**      obtained by calling the GetDC function. Instead, it must call the 
**      ReleaseDC function to free the device context. 
*/
BOOL WINAPI DeleteDC(HDC hdc)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    // 窗口DC不能使用本函数删除, 否则会在窗口重画时引起错误
    ENTERMONITOR;

    if (DC_IsWindowDC(pDC))
    {
        LEAVEMONITOR;

        SetLastError(1);
        return FALSE;
    }

    DC_Destroy(pDC);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : SetTextColor
**  Purpose  :
**      Sets the text color for the specified device context to the 
**      specified color.
**  Params   :
**      hdc     : Identifies the device context. 
**      color   : Specifies the color of the text.
**  Return   :
**      If the function succeeds, returns a the previous text color. 
**      If the function fails, returns CLR_INVALID. To get extended 
**      error information, call GetLastError. 
*/
COLORREF WINAPI SetTextColor(HDC hdc, COLORREF color)
{
    PDC pDC;
    COLORREF cRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return CLR_INVALID;
    }

    ENTERMONITOR;

    cRet = DC_SetTextColor(pDC, color);

    LEAVEMONITOR;

    return cRet;
}

/*
**  Function : GetTextColor
**  Purpose  :
**      Retrieves the current text color for the specified device context.
**  Params   :
**      hdc  : Identifies the device context. 
**  Return   :
**      If the function succeeds, returns the current text color.
**      If the funciton fails, returns CLR_INVALID.
*/
COLORREF WINAPI GetTextColor(HDC hdc)
{
    PDC pDC;
    COLORREF cRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return CLR_INVALID;
    }

    ENTERMONITOR;

    cRet = DC_GetTextColor(pDC);

    LEAVEMONITOR;

    return cRet;
}

COLORREF WINAPI SetDCPenColor(HDC hdc, COLORREF color)
{
    PDC pDC;
    COLORREF cRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return CLR_INVALID;
    }

    ENTERMONITOR;

    cRet = DC_SetPenColor(pDC, color);

    LEAVEMONITOR;

    return cRet;
}

COLORREF WINAPI GetDCPenColor(HDC hdc)
{
    PDC pDC;
    COLORREF cRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return CLR_INVALID;
    }

    ENTERMONITOR;

    cRet = DC_GetPenColor(pDC);

    LEAVEMONITOR;

    return cRet;
}

COLORREF WINAPI SetDCBrushColor(HDC hdc, COLORREF color)
{
    PDC pDC;
    COLORREF cRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return CLR_INVALID;
    }

    ENTERMONITOR;

    cRet = DC_SetBrushColor(pDC, color);

    LEAVEMONITOR;

    return cRet;
}

COLORREF WINAPI GetDCBrushColor(HDC hdc)
{
    PDC pDC;
    COLORREF cRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return CLR_INVALID;
    }

    ENTERMONITOR;

    cRet = DC_GetBrushColor(pDC);

    LEAVEMONITOR;

    return cRet;
}

/*
**  Function : SetBkColor
**  Purpose  :
**      Sets the current background color to the specified color value, 
**      or to the nearest physical color if the device cannot represent 
**      the specified color value. 
**  Params   :
**      hdc     : Indentifies the device context.
**      color   : Specifies the new background color.
**  Return   :
**      If the function succeeds, returns specifies the previous background
**      color as a COLORREF value. 
**      If the function fails, returns CLR_INVALID. 
**  Remarks
**      This function fills the gaps between styled lines drawn using a 
**      pen created by the CreatePen function; it does not fill the gaps 
**      between styled lines drawn using a pen created by the CreatePen 
**      function. 
**      If the background mode is OPAQUE, the background color is used to 
**      fill gaps between styled lines, gaps between hatched lines in 
**      brushes, and character cells. The background color is also used 
**      when converting bitmaps from color to monochrome and vice versa. 
*/
COLORREF WINAPI SetBkColor(HDC hdc, COLORREF color)
{
    PDC pDC;
    COLORREF cRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return CLR_INVALID;
    }

    ENTERMONITOR;

    cRet = DC_SetBkColor(pDC, color);

    LEAVEMONITOR;

    return cRet;
}

/*
**  Function : GetBkColor
**  Purpose  :
**      Returns the current background color for the specified device 
**      context. 
**  Params   :
**      hdc  : Indentifies the device context.
**  Return   :
**      If the function succeeds, returns a COLORREF value for the current
**      background color.
**      If the function fails, the return value is CLR_INVALID. 
*/
COLORREF WINAPI GetBkColor(HDC hdc)
{
    PDC pDC;
    COLORREF cRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return CLR_INVALID;
    }

    ENTERMONITOR;

    cRet = DC_GetBkColor(pDC);

    LEAVEMONITOR;

    return cRet;
}

/*
**  Function : SetTextAlign
**  Purpose  :
**      sets the text-alignment flags for the specified device context. 
**  Params   :
**      hdc   : Identifies the device context. 
**      uMode : Specifies the text alignment by using a mask of the values.
**              Only one flag can be chosen from those that affect 
**              horizontal and vertical alignment. 
**  Return   :
**      If the function succeeds, return is the previous text-alignment 
**      setting. If the function fails, return GDI_ERROR. To get extended 
**      error information, call GetLastError. 
**  Remarks  :
**      The TextOut functions use the text-alignment flags to position a 
**      string of text on a display or other device. The flags specify 
**      the relationship between a reference point and a rectangle that 
**      bounds the text. The reference point is either the current 
**      position or a point passed to a text output function. 
**      The rectangle that bounds the text is formed by the character 
**      cells in the text string. 
*/
UINT WINAPI SetTextAlign(HDC hdc, UINT uMode)
{
    PDC pDC;
    UINT uRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return (UINT)GDI_ERROR;
    }

    ENTERMONITOR;

    uRet = DC_SetTextAlign(pDC, uMode);

    LEAVEMONITOR;

    return uRet;
}

/*
**  Function : GetTextAlign
**  Purpose  :
**      Retrieves the text-alignment setting for the specified device 
**      context. 
**  Params   :
**      hdc  : Indentifies the device context.
**  Return   :
**      If the function succeeds, return the status of the text-alignment 
**      flags. If the function fails, the return value is GDI_ERROR. To 
**      get extended error information, call GetLastError. 
*/
UINT WINAPI GetTextAlign(HDC hdc)
{
    PDC pDC;
    UINT uRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return (UINT)GDI_ERROR;
    }

    ENTERMONITOR;

    uRet = DC_GetTextAlign(pDC);

    LEAVEMONITOR;

    return uRet;
}

/*
**  Function : SetBkMode
**  Purpose  :
**      Sets the background mix mode of the specified device context. The 
**      background mix mode is used with text, hatched brushes, and pen 
**      styles that are not solid lines. 
**  Params   :
**      hdc     : Indentifies the device context.
**      nBkMode : Specifies the background mode. This parameter can be 
**                either of the following values : 
**
**                  Value	Description
**
**                  OPAQUE	    Background is filled with the current 
**                              background color before the text, 
**                              hatched brush, or pen is drawn.
**                  TRANSPARENT	Background remains untouched.
**  Return   :
**      If the function succeeds, return value specifies the previous 
**      background mode. If the function fails, returns zero. 
**  Remarks  :
**      The SetBkMode function affects the line styles for lines drawn 
**      using a pen created by the CreatePen function. SetBkMode does 
**      not affect lines drawn using a pen created by the CreatePen 
**      function. The nBkMode parameter can also be set to driver-specific 
**      values. GDI passes such values to the device driver and otherwise 
**      ignores them.
*/
int WINAPI SetBkMode(HDC hdc, int nBkMode)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = DC_SetBkMode(pDC, nBkMode);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : GetBkMode
**  Purpose  :
**      Returns the current background mix mode for a specified device 
**      context. The background mix mode of a device context affects 
**      text, hatched brushes, and pen styles that are not solid lines. 
**  Params   :
**      hdc  : Indentifies the device context.
**  Return   :
**      If the function succeeds, return the current background mix mode, 
**      either BM_OPAQUE or BM_TRANSPARENT. If the function fails, return
**      zero. 
*/
int WINAPI GetBkMode(HDC hdc)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = DC_GetBkMode(pDC);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : SetROP2
**  Purpose  :
**      Sets the current foreground mix mode. The foreground mix mode is
**      used to combine pen and interiors of filled objects with the colors
**      already on the screen. The foreground mix mode defines how colors
**      from the brush or pen and the colors in the exist image are to be 
**      combined.
**  Params   :
**      hdc  : Indentifies the device context.
**      nRop : specified the new mix mode.
**  Return   :
**      If the function succeeds, the return value specifies the previous 
**      mix mode. If the function fails, the return value is zero. 
*/
int WINAPI SetROP2(HDC hdc, int nRop)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = DC_SetROP2(pDC, nRop);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : GetROP2
**  Purpose  :
**      Retrieves the foreground mix mode of the specified device context.
**      The mix mode specifies how the pen or interior color and the color
**      already on the screen are combined to yield a new color. 
**  Params   :
**      hdc  : Indentifies the device context.
**  Return   :
**      If the function succeeds, return the foreground mix mode. 
**      If the function fails, return zero. 
*/
int WINAPI GetROP2(HDC hdc)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = DC_GetROP2(pDC);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : GetMapMode
**  Purpose  :
**      The GetMapMode function retrieves the current mapping mode. 
**  Params   :
**      hdc		: handle of device context. 
**  Return   :
**      If the function succeeds, the return value specifies the mapping mode.  
**		If the function fails, the return value is zero. 
*/
int GetMapMode(HDC hdc)
{
	PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = DC_GetMapMode(pDC);

    LEAVEMONITOR;

	return nRet;
}

/*
**  Function : SetMapMode
**  Purpose  :
**      The SetMapMode function sets the mapping mode of the specified device
**		context. The mapping mode defines the unit of measure used to transform
**		page-space units into device-space units, and also defines the orientation
**		of the device’s x and y axes. 
**  Params   :
**      hdc	 : handle of device context. 
**  Return   :
**      If the function succeeds, the return value identifies the previous 
**		mapping mode. 
**		If the function fails, the return value is zero. 
*/
int SetMapMode(HDC hdc, int nMapMode)
{
	PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return 0;
    }

	if (nMapMode == MM_TEXT)
    {
        ENTERMONITOR;

        nRet = DC_SetMapMode(pDC, nMapMode);

        LEAVEMONITOR;

        return nRet;
    }

	return 0;
}

/*
**  Function : GetViewportExtEx
**  Purpose  :
**      The GetViewportExtEx function retrieves the x-extents and y-extents of
**		the current viewport for the specified device context.  
**  Params   :
**      hdc		: handle of device context. 
**      lpSize  : address of structure receiving viewport dimensions. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**		If the function fails, the return value is zero. 
*/
BOOL GetViewportExtEx(HDC hdc, PSIZE pSize)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    DC_GetViewportExt(pDC, pSize);

    LEAVEMONITOR;

	return TRUE;
}
 
/*
**  Function : GetViewportOrgEx
**  Purpose  :
**      retrieves the x-coordinates and y-coordinates of the viewport origin
**		for the specified device context. 
**  Params   :
**      hdc		: handle of device context. 
**      pPoint  : Points to a POINT structure that receives the origin coordinates. 
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL GetViewportOrgEx(HDC hdc, PPOINT pPoint)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    DC_GetViewportOrg(pDC, pPoint);

    LEAVEMONITOR;

	return TRUE;
}

/*
**  Function : GetWindowExtEx
**  Purpose  :
**      This function retrieves the x-extents and y-extents of the window
**		for the specified device context. 
**  Params   :
**      hdc		: handle of device context. 
**      pSize   : Points to a SIZE structure. The x- and y-extents in page-space
**				  units are placed in this structure.  
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL GetWindowExtEx(HDC hdc, PSIZE pSize)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    DC_GetWindowExt(pDC, pSize);

    LEAVEMONITOR;

	return TRUE;
}


/*
**  Function : GetWindowOrgEx
**  Purpose  :
**      retrieves the x-coordinates and y-coordinates of the window origin
**	    for the specified device context. 
**  Params   :
**      hdc		: handle of device context. 
**      pPoint  : Points to a POINT structure that receives the coordinates,
**				  in page units, of the window origin. 
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL GetWindowOrgEx(HDC hdc, PPOINT pPoint)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    DC_GetWindowOrg(pDC, pPoint);

    LEAVEMONITOR;

	return TRUE;
}

/*
**  Function : SetViewportExtEx
**  Purpose  :
**      sets the horizontal and vertical extents of the viewport for a device
**		context by using the specified values.  
**  Params   :
**      hdc		: handle of device context. 
**		nXExtent: Specifies the horizontal extent, in device units, of the viewport. 
**		nYExtent: Specifies the vertical extent, in device units, of the viewport. 
**      pSize   : Points to a SIZE structure. The previous viewport extents
**				  (in device units) are placed in this structure. If lpSize is
**				   NULL, nothing is returned. 
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL SetViewportExtEx(HDC hdc, int nXExtent, int nYExtent, PSIZE pSize)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return (BOOL)GDI_ERROR;
    }

    ENTERMONITOR;

	DC_SetViewportExt(pDC, nXExtent, nYExtent, pSize);
    
    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : SetViewportOrgEx
**  Purpose  :
**      sets the horizontal and vertical extents of the viewport for a device
**		context by using the specified values.  
**  Params   :
**      hdc		: handle of device context. 
**		x		: Specifies the horizontal extent, in device units, of the viewport. 
**		y		: Specifies the y-coordinate, in device units, of the new viewport
**				  origin. 
**      pPoint  : Points to a POINT structure. The previous viewport origin
**				  (in device coordinates) is placed in this structure. If lpPoint
**				  is NULL, nothing is returned. 
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL SetViewportOrgEx(HDC hdc, int x, int y, PPOINT pPoint)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return (BOOL)GDI_ERROR;
    }

    ENTERMONITOR;

    DC_SetViewportOrg(pDC, x, y, pPoint);
    
    LEAVEMONITOR;

    return TRUE;
}


/*
**  Function : SetWindowExtEx
**  Purpose  :
**      sets the horizontal and vertical extents of the window for a device
**		context by using the specified values. 
**  Params   :
**      hdc		: handle of device context. 
**		nXExtent: Specifies the window’s horizontal extent in logical units. 
**		nYExtent: Specifies the window’s vertical extent in logical units. 
**      pSize   : Points to a SIZE structure. The previous window extents
**				  (in logical units) are placed in this structure. If lpSize
**				  is NULL, nothing is returned.  
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL SetWindowExtEx(HDC hdc, int nXExtent, int nYExtent, PSIZE pSize)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return (BOOL)GDI_ERROR;
    }

    ENTERMONITOR;

	DC_SetWindowExt(pDC, nXExtent, nYExtent, pSize);
    
    LEAVEMONITOR;

    return TRUE;
}


/*
**  Function : SetWindowOrgEx
**  Purpose  :
**      sets the window origin of the device context by using the specified
**		coordinates. 
**  Params   :
**      hdc		: handle of device context. 
**		x		: Specifies the logical x-coordinate of the new window origin.  
**		y		: Specifies the logical y-coordinate of the new window origin. 
**      pPoint  : Points to a POINT structure. The previous origin of the window
**				  is placed in this structure. If lpPoint is NULL, nothing is
**				  returned.   
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL SetWindowOrgEx(HDC hdc, int x, int y, PPOINT pPoint)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return (BOOL)GDI_ERROR;
    }

    ENTERMONITOR;

    DC_SetWindowOrg(pDC, x, y, pPoint);
    
    LEAVEMONITOR;

    return TRUE;
}


/*
**  Function : OffsetViewportOrgEx
**  Purpose  :
**      modifies the viewport origin for a device context using the specified
**		horizontal and vertical offsets. 
**  Params   :
**      hdc		: handle of device context. 
**		nXOffset: Specifies the horizontal offset, in device units.   
**		nYOffset: Specifies the vertical offset, in device units. 
**      pPoint  : Points to a POINT structure. The previous viewport origin,
**				  in device units, is placed in this structure. If lpPoint 
**				  is NULL, the previous viewport origin is not returned.   
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL OffsetViewportOrgEx(HDC hdc, int nXOffset, int nYOffset, PPOINT pPoint)
{
	PDC		pDC;
	POINT	VportOrg;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

	DC_GetViewportOrg(pDC, &VportOrg);
	DC_SetViewportOrg(pDC, VportOrg.x + nXOffset, VportOrg.y + nYOffset, pPoint);

    LEAVEMONITOR;

	return TRUE;
}


/*
**  Function : OffsetWindowOrgEx
**  Purpose  :
**      modifies the window origin for a device context using the specified
**		horizontal and vertical offsets. 
**  Params   :
**      hdc		: handle of device context. 
**		nXOffset: Specifies the horizontal offset, in logical units. 
**		nYOffset: Specifies the vertical offset, in logical units.  
**      pPoint  : Points to a POINT structure. The logical coordinates of the
**				  previous window origin are placed in this structure. If lpPoint
**				  is NULL, the previous origin is not returned. 
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL OffsetWindowOrgEx(HDC hdc, int nXOffset, int nYOffset, PPOINT pPoint)
{
	PDC		pDC;
	POINT	WinOrg;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

	DC_GetWindowOrg(pDC, &WinOrg);
	DC_SetWindowOrg(pDC, WinOrg.x + nXOffset, WinOrg.y + nYOffset, pPoint);

    LEAVEMONITOR;

	return TRUE;
}

/*
**  Function : ScaleViewportExtEx
**  Purpose  :
**      modifies the viewport for a device context (DC) by using the ratios
**		formed by the specified multiplicands and divisors. 
**  Params   :
**      hdc		: handle of device context. 
**		Xnum	: Specifies the amount by which to multiply the current horizontal
**				  extent. 
**		Xdenom	: Specifies the amount by which to divide the current horizontal 
**				  extent.   
**		Ynum	: Specifies the amount by which to multiply the current vertical extent. 
**		Ydenom	: Specifies the amount by which to divide the current vertical extent. 
**      pSize	: Points to a SIZE structure. The previous viewport extents 
**				  (in device units) are placed in this structure. If lpSize is NULL,
**				  nothing is returned. 
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL ScaleViewportExtEx(HDC hdc, int Xnum, int Xdenom, int Ynum, int Ydenom, PSIZE pSize)
{
	PDC		pDC;
	SIZE	VportExt;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

	DC_GetViewportExt(pDC, &VportExt);
	DC_SetViewportExt(pDC, (VportExt.cx * Xnum) / Xdenom, 
						   (VportExt.cy * Ynum) / Ydenom, pSize);

    LEAVEMONITOR;

	return TRUE;
}


/*
**  Function : ScaleWindowExtEx
**  Purpose  :
**      modifies the viewport for a device context (DC) by using the ratios
**		formed by the specified multiplicands and divisors. 
**  Params   :
**      hdc		: handle of device context. 
**		Xnum	: Specifies the amount by which to multiply the current horizontal extent.  
**		Xdenom	: Specifies the amount by which to divide the current horizontal extent. 
**		Ynum	: Specifies the amount by which to multiply the current vertical extent. 
**		Ydenom	: Specifies the amount by which to divide the current vertical extent.  
**      pSize	: Points to a SIZE structure. The previous window extents 
**				  (in logical units) are placed in this structure. If lpSize is
**				  NULL, nothing is returned. 
**  Return   :
**      If the function succeeds, the return value is nonzero.  
**		If the function fails, the return value is zero. 
*/
BOOL ScaleWindowExtEx(HDC hdc, int Xnum, int Xdenom, int Ynum, int Ydenom, PSIZE pSize)
{
	PDC		pDC;
	SIZE	WinExt;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

	DC_GetWindowExt(pDC, &WinExt);
	DC_SetViewportExt(pDC, (WinExt.cx * Xnum) / Xdenom, 
						   (WinExt.cy * Ynum) / Ydenom, pSize);

    LEAVEMONITOR;

	return TRUE;
}

/*
**  Function : DPtoLP
**  Purpose  :
**      Converts device coordinates into logical coordinates. The conversion
**		depends on the mapping mode of the device context, the settings of 
**		the origins and extents for the window and viewport, and the world 
**		transformation. 
**  Params   :
**      hdc		: handle of device context. 
**		pPoints: Points to an array of POINT structures. The x- and y-coordinates
**				  contained in each POINT structure will be transformed.
**      nCount  : Specifies the number of points in the array. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**		If the function fails, the return value is zero. 
*/
BOOL DPtoLP(HDC hdc, PPOINT pPoints, int nCount)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

	if (!pPoints || nCount == 0)
	{
		SetLastError(1);
		return FALSE;
	}

    ENTERMONITOR;

	DC_DPtoLP(pDC, pPoints, nCount);

    LEAVEMONITOR;

	return TRUE;
}


/*
**  Function : LPtoDP
**  Purpose  :
**      Converts logical coordinates into device coordinates. The conversion
**		depends on the mapping mode of the device context, the settings of 
**		the origins and extents for the window and viewport, and the world 
**		transformation. 
**  Params   :
**      hdc		: handle of device context. 
**		pPoints: Points to an array of POINT structures. The x-coordinates
**				  and y-coordinates contained in each of the POINT structures 
**				  will be transformed. 
**      nCount  : Specifies the number of points in the array. 
**  Return   :
**      If the function succeeds, the return value is nonzero. 
**		If the function fails, the return value is zero. 
*/
BOOL LPtoDP(HDC hdc, PPOINT pPoints, int nCount)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

	if (!pPoints || nCount == 0)
	{
		SetLastError(1);
		return FALSE;
	}

    ENTERMONITOR;

	DC_LPtoDP(pDC, pPoints, nCount);

    LEAVEMONITOR;

	return TRUE;
}

BOOL WINAPI SetClipRect(HDC hdc, const RECT* pRect)
{
	PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    DC_SetClipRect(pDC, pRect);

    LEAVEMONITOR;

    return TRUE;
}

/**************************************************************************/
/*              Device Capabilities                                       */
/**************************************************************************/

/*
**  Function : GetDeviceCaps
**  Purpose  :
**      retrieves device-specific information about a specified device. 
**  Params   :
**      hdc    : Identifies the device context. 
**      nIndex : Specifies the item to return. 
**  Return   :
**      The return value specifies the value of the desired item. 
*/
int WINAPI GetDeviceCaps(HDC hdc, int nIndex)
{
    PDC pDC;
    int nRet;

    if (hdc)
    {
        pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
        if (!pDC)
        {
            SetLastError(1);
            return 0;
        }
    }
    else
        pDC = NULL;

    ENTERMONITOR;

    nRet = DC_GetDeviceCaps(pDC, nIndex);

    LEAVEMONITOR;

    return nRet;
}

/**************************************************************************/
/*              GDI Object Support                                        */
/**************************************************************************/

/*
**  Function : GetStockObject
**  Purpose  :
**      Retrieves a handle to one of the predefined stock pens, brushes, 
**      fonts. 
**  Params   :
**      nObjID : Specifies the type of stock object. 
**  Return   :
**      If the function succeeds, the return value identifies the logical 
**      object requested. If the function fails, the return value is NULL. 
**
*/
HGDIOBJ WINAPI GetStockObject(int nObjID)
{
    PGDIOBJ pObj;

    pObj = (PGDIOBJ)WOT_GetStockObj(nObjID);

    if (!pObj)
        return NULL;

    return (HGDIOBJ)WOT_GetHandle(pObj);
}

/*
**  Function : DeleteObject
**  Purpose  :
**      Deletes a logical pen, brush, font, bitmap, freeing all system 
**      resources associated with the object. After the object is deleted, 
**      the specified handle is no longer valid. 
**  Params   :
**      hGdiObj : Identifies a logical pen, brush, font, bitmap.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the specified handle is not valid or is currently selected into 
**      a device context, the return value is FALSE. 
**  Remarks  :
**      Do not delete a drawing object (pen or brush) while it is still 
**      selected into a device context. When a pattern brush is deleted, 
**      the bitmap associated with the brush is not deleted. The bitmap 
**      must be deleted independently. 
*/
BOOL WINAPI DeleteObject(HGDIOBJ hGdiObj)
{
    PXGDIOBJ pObj;
    BOOL  nRet = FALSE;

    ENTERMONITOR;

    pObj = WOT_LockObj((HANDLE)hGdiObj, OBJ_ANY);
    if (!pObj)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return FALSE;
    }

    ASSERT(WOT_GetObjType((PWSOBJ)pObj) >= OBJ_XGDI_MIN && 
        WOT_GetObjType((PWSOBJ)pObj) <= OBJ_XGDI_MAX);

    WOT_DelectObj(pObj);

    if (WOT_UnlockObj(pObj) == NULL)
        nRet = TRUE;
    
    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : SelectObject
**  Purpose  :
**      Selects an object into the specified device context. The new 
**      object replaces the previous object of the same type.
**  Params   :
**      hdc     : Identifies the device context. 
**      hGdiObj : Identifies the object to be selected. 
**  Return   :
**      If the function succeeds, return the previously selected object 
**      of the specified type. An application should always replace a 
**      new object with the original, default object after it has finished 
**      drawing with the new object. 
**      If the function fails, return NULL.
*/
HGDIOBJ WINAPI SelectObject(HDC hdc, HGDIOBJ hGdiObj)
{
    PDC pDC;
    HGDIOBJ hRet;
    PXGDIOBJ pObj, pOldObj;

    // Gets the DC object
    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pObj = WOT_LockObj((HANDLE)hGdiObj, OBJ_ANY);
    if (pObj == NULL)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return NULL;
    }

    pOldObj = DC_SelectObject(pDC, pObj);
    WOT_UnlockObj(pObj);
    if (pOldObj == NULL)
        hRet = NULL;
    else
        hRet = (HGDIOBJ)WOT_GetHandle((PGDIOBJ)pOldObj);

    LEAVEMONITOR;

    return hRet;
}

/*
**  Function : GetObject
**  Purpose  :
**      Obtains information about a specified graphics object. Depending 
**      on the graphics object, the function places a filled-in BITMAP, 
**      LOGBRUSH, LOGFONT, or LOGPEN structure, or a count of table entries
**      (for a logical palette), into a specified buffer. 
**  Params   :
**      hGdiObj : A handle to the graphics object of interest. This can be 
**                a handle to one of the following: a logical bitmap, a 
**                brush, a font, a palette, a pen,
**      cbBuffer: Specifies the number of bytes of information to be written
**                to the buffer. 
**      pObject : Points to a buffer that is to receive the information 
**                about the specified graphics object. 
**  Return   :
**      If the function succeeds, and pObject is a valid pointer, return the
**      number of bytes stored into the buffer. 
**      If the function succeeds, and pObject is NULL, return the number of 
**      bytes required to hold the information the function would store into
**      the buffer. 
*/
int WINAPI GetObject(HGDIOBJ hGdiObj, int cbBuffer, void* pObject)
{
    UINT uType;
    int nSize;
    LOGPEN logpen;
    LOGBRUSH logbrush;
    PXGDIOBJ pObj;

    ENTERMONITOR;

    pObj = WOT_LockObj((HANDLE)hGdiObj, OBJ_ANY);
    if (pObj == NULL)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return 0;
    }

    uType = WOT_GetObjType((PWSOBJ)pObj);
    ASSERT(uType >= OBJ_XGDI_MIN && uType <= OBJ_XGDI_MAX);
    
    switch (uType)
    {
    case OBJ_PEN :
        
        nSize = sizeof(LOGPEN);
        
        if (pObject)
        {
            if (nSize > cbBuffer)
                nSize = cbBuffer;
            
            PEN_GetLogObj((PPENOBJ)pObj, &logpen);
            memcpy(pObject, &logpen, nSize);
        }
        
        break;
        
    case OBJ_BRUSH :
        
        nSize = sizeof(LOGBRUSH);
        
        if (pObject)
        {
            if (nSize > cbBuffer)
            {
                LEAVEMONITOR;
                return 0;
            }
            
            BRUSH_GetLogObj((PBRUSHOBJ)pObj, &logbrush);
            memcpy(pObject, &logbrush, nSize);
        }
        
        break;
        
    case OBJ_BITMAP :
        
        nSize = sizeof(BITMAP);
        
        if (pObject)
        {
            if (nSize > cbBuffer)
                nSize = cbBuffer;
            
            memcpy(pObject, &((PBMPOBJ)pObj)->bitmap, nSize);
        }
        
        break;
        
    default :
        
        nSize = 0;
    }

    WOT_UnlockObj(pObj);

    LEAVEMONITOR;

    return nSize;
}

/*
**  Function : UnrealizeObject
**  Purpose  :
**      Resets a logical palette. It directs the system to realize the 
**      palette as though it had not previously been realized. The next 
**      time the application calls the RealizePalette function for the 
**      specified palette, the system completely remaps the logical 
**      palette to the system palette. 
**  Params   :
**      hGdi : Identifies the logical palette to be reset. 
**  Return   :
**      If the function succeeds, return TRUE.
**      If the function fails, return FALSE. To get the extented error
**      infomation, call GetLastError.
**  Remarks  :
**      The UnrealizeObject function should not be used with stock objects.
**      The default palette, obtained by calling GetStockObject
**      (DEFAULT_PALETTE), is a stock object. 
**      A palette identified by hgdiobj can be the currently selected 
**      palette of a device context. 
*/
BOOL WINAPI UnrealizeObject(HGDIOBJ hGdi)
{
    return TRUE;
}

/**************************************************************************/
/*              Pen Support                                               */
/**************************************************************************/

/*
**  Function : CreatePen
**  Purpose  :
**      Create a pen with specified width style and color.
**  Params   :
**      nStyle : Specifies the style of the pen to be created.
**      nWidth : Specifies the width of the pen to be created.
**      color  : Specified the color of the pen to be created.
**  Return  :
**      If the function succeeds, the return value is the handle of the
**      pen to be created. If the functions fails, return value is NULL.
**      To get the extended error infomation, call GetLastError.
*/
HPEN WINAPI CreatePen(int nStyle, int nWidth, COLORREF color)
{
    PPENOBJ pPen;

    ENTERMONITOR;

    pPen = PEN_Create(nStyle, nWidth, PES_DEFAULT, PFM_CENTER, color, FALSE);

    LEAVEMONITOR;

    if (!pPen)
    {
        SetLastError(1);
        return NULL;
    }

    return (HPEN)WOT_GetHandle((PGDIOBJ)pPen);
}

/*
**  Function : CreatePenEx
**  Purpose  :
**      Create a pen with specified width style and color.
**  Params   :
**      nStyle      : Specifies the style of the pen to be created.
**      nWidth      : Specifies the width of the pen to be created.
**      nEndStyle   : Specifies the end style of the pen to be created.
**      nFrameMode  : Specifies the end style of the pen to be created.
**      color       : Specified the color of the pen to be created.
**  Return  :
**      If the function succeeds, the return value is the handle of the
**      pen to be created. If the functions fails, return value is NULL.
**      To get the extended error infomation, call GetLastError.
*/
HPEN WINAPI CreatePenEx(int nStyle, int nWidth, int nEndStyle, 
                        int nFrameMode, COLORREF color)
{
    PPENOBJ pPen;

    ENTERMONITOR;

    pPen = PEN_Create(nStyle, nWidth, nEndStyle, nFrameMode, color, FALSE);

    LEAVEMONITOR;

    if (!pPen)
    {
        SetLastError(1);
        return NULL;
    }

    return (HPEN)WOT_GetHandle((PGDIOBJ)pPen);
}

#ifdef MSWIN_COMPATIBLE

/*
**  Function : CreatePenIndirect
**  Purpose  :
**      Creates a logical pen that has the style, width, and color 
**      specified in a structure. 
**  Params   :
**      pLogPen : Points to the LOGPEN structure that specifies the 
**                pen’s style, width, and color. 
**  Return   :
**      If the function succeeds, the return value is the handle of the
**      pen to be created. If the functions fails, return value is NULL.
**      To get the extended error infomation, call GetLastError.
*/
HPEN WINAPI CreatePenIndirect(const LOGPEN* pLogPen)
{
    PPENOBJ pPen;

    if (!pLogPen)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pPen = PEN_Create(pLogPen->lopnStyle, pLogPen->lopnWidth.x, 
        PES_DEFAULT, PFM_CENTER, pLogPen->lopnColor, FALSE);

    LEAVEMONITOR;

    if (!pPen)
    {
        SetLastError(1);
        return NULL;
    }

    return (HPEN)WOT_GetHandle((PGDIOBJ)pPen);
}

#endif /* MSWIN_COMPATIBLE */

/**************************************************************************/
/*              Brush Support                                             */
/**************************************************************************/

/*
**  Function : CreateBrush
**  Purpose  :
**      creates a logical brush that has the specified style, color, 
**      and pattern. 
**  Params   :
**      nStyle  : Specifies the brush style.
**      color   : Specifies the color in which the brush is to be drawn. 
**                If wStyle is the BS_HOLLOW or BS_PATTERN style, lbColor 
**                is ignored. 
**      lHatch  : Specifies a hatch style.
**  Return   :
**      If the function succeeds, the return value is the handle of the
**      brush to be created. If the functions fails, return value is 
**      NULL. To get the extended error infomation, call GetLastError.
*/
HBRUSH WINAPI CreateBrush(int nStyle, COLORREF color, LONG lHatch)
{
    PBRUSHOBJ pBrush;

    ENTERMONITOR;

    pBrush = BRUSH_Create(nStyle, color, lHatch, FALSE);

    LEAVEMONITOR;

    if (!pBrush)
    {
        SetLastError(1);
        return NULL;
    }

    return (HBRUSH)WOT_GetHandle((PGDIOBJ)pBrush);
}

#if (__MP_PLX_GUI)
/*********************************************************************\
* Function	   CreateSharedBrush
* Purpose      Create a system shared brush in MP GUI
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
HBRUSH WINAPI CreateSharedBrush(int nStyle, COLORREF color, LONG lHatch)
{
    PBRUSHOBJ pBrush;
    
    ENTERMONITOR;
    
    pBrush = BRUSH_Create(nStyle, color, lHatch, TRUE);
    
    LEAVEMONITOR;
    
    if (!pBrush)
    {
        SetLastError(1);
        return NULL;
    }
    
    return (HBRUSH)WOT_GetHandle((PGDIOBJ)pBrush);
}
#endif //__MP_PLX_GUI

#ifdef MSWIN_COMPATIBLE

/*
**  Function : CreateSolidBrush
**  Purpose  :
**      creates a logical brush that has the specified solid color. 
**  Params   :
**      color : Specifies the color of the brush. 
**  Return   :
**      If the function succeeds, the return value is the handle of the
**      brush to be created. If the functions fails, return value is 
**      NULL. To get the extended error infomation, call GetLastError.
*/
HBRUSH WINAPI CreateSolidBrush(COLORREF color)
{
    PBRUSHOBJ pBrush;

    ENTERMONITOR;

    pBrush = BRUSH_Create(BS_SOLID, color, 0, FALSE);

    LEAVEMONITOR;

    if (!pBrush)
    {
        SetLastError(1);
        return NULL;
    }

    return (HBRUSH)WOT_GetHandle((PGDIOBJ)pBrush);
}

/*
**  Function : CreateHatchBrush
**  Purpose  :
**      creates a logical brush that has the specified solid color. 
**  Params   :
**      nHatchStyle : Specifies the hatch style of the brush. 
**      color       : Specifies the foreground color of the brush that 
**                    is used for the hatches. 
**  Return   :
**      If the function succeeds, the return value is the handle of the
**      brush to be created. If the functions fails, return value is 
**      NULL. To get the extended error infomation, call GetLastError.
*/
HBRUSH WINAPI CreateHatchBrush(int nHatchStyle, COLORREF color)
{
    PBRUSHOBJ pBrush;

    ENTERMONITOR;

    pBrush = BRUSH_Create(BS_HATCHED, color, nHatchStyle, FALSE);

    LEAVEMONITOR;

    if (!pBrush)
    {
        SetLastError(1);
        return NULL;
    }

    return (HBRUSH)WOT_GetHandle((PGDIOBJ)pBrush);
}

/*
**  Function : CreatePatternBrush
**  Purpose  :
**      creates a logical brush that has the specified solid color. 
**  Params   :
**      hBitmap : Identifies the bitmap to be used to create the 
**                logical brush.  
**  Return   :
**      If the function succeeds, the return value is the handle of the
**      brush to be created. If the functions fails, return value is 
**      NULL. To get the extended error infomation, call GetLastError.
*/
HBRUSH WINAPI CreatePatternBrush(HBITMAP hBitmap)
{
    return NULL;
}

/*
**  Function : CreateBrushIndirect
**  Purpose  :
**      creates a logical brush that has the specified style, color, 
**      and pattern. 
**  Params   :
**      pLogBrush : Points to a LOGBRUSH structure that contains 
**                  information about the brush. 
**  Return   :
**      If the function succeeds, the return value is the handle of the
**      brush to be created. If the functions fails, return value is 
**      NULL. To get the extended error infomation, call GetLastError.
*/
HBRUSH WINAPI CreateBrushIndirect(const LOGBRUSH* pLogBrush)
{
    PBRUSHOBJ pBrush;

    if (!pLogBrush)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pBrush = BRUSH_Create(pLogBrush->lbStyle, pLogBrush->lbColor, 
        pLogBrush->lbHatch, FALSE);

    LEAVEMONITOR;

    if (!pBrush)
    {
        SetLastError(1);
        return NULL;
    }

    return (HBRUSH)WOT_GetHandle((PGDIOBJ)pBrush);
}

#endif /* MSWIN_COMPATIBLE */

/*
**  Function : SetBrushOrg
**  Purpose  :
**      Sets the brush origin that GDI assigns to the next brush an 
**      application selects into the specified device context. 
*/
BOOL WINAPI SetBrushOrg(HDC hdc, int nOrgX, int nOrgY, PPOINT pptPrev)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return (BOOL)GDI_ERROR;
    }

    ENTERMONITOR;

    DC_SetBrushOrg(pDC, nOrgX, nOrgY, pptPrev);

    LEAVEMONITOR;
    
    return TRUE;
}

/**************************************************************************/
/*              Color Palette Support                                     */
/**************************************************************************/

/**************************************************************************/
/*              Clipping support                                          */
/**************************************************************************/

#ifdef RGNSUPPORT

/*
**  Function : SelectClipRgn
**  Purpose  :
**      Selects a region as the current clipping region for the specified 
**      device context. 
**  Params   :
**      hdc  : Handle to the device context. 
**      hrgn : Handle to the region to be selected. 
**  Return   :
**      Return value specifies the region's complexity and can be any one 
**      of the following values: 
**          NULLREGION    Region is empty. 
**          SIMPLEREGION  Region is a single rectangle. 
**          COMPLEXREGION Region is more than one rectangle. 
**          ERROR         An error occurred. (The previous clipping region
**                        is unaffected.) 
**      To get extended error Information, call GetLastError.
**  Remarks  :
**      Only a copy of the selected region is used. The region itself can 
**      be selected for any number of other device contexts or it can be 
**      deleted. 
**      The SelectClipRgn function assumes that the coordinates for a 
**      region are specified in device units. 
**      To remove a device-context's clipping region, specify a NULL 
**      region handle. 
*/
int WINAPI SelectClipRgn(HDC hdc, HRGN hrgn)
{
    return ExtSelectClipRgn(hdc, hrgn, RGN_COPY);
}

/*
**  Function : ExtSelectClipRgn
**  Purpose  :
**      Combines the specified region with the current clipping region by 
**      using the specified mode. 
**  Params   :
**      hdc   : Handle to the device context. 
**      hrgn  : Handle to the region to be selected. 
**      nMode : Specifies the operation to be performed. It must be one of 
**              the following values: 
**                RGN_AND  : The new clipping region combines the 
**                           overlapping areas of the current clipping 
**                           region and the region identified by hrgn. 
**                RGN_COPY : The new clipping region is a copy of the 
**                           region identified by hrgn. This is identical 
**                           to SelectClipRgn. If the region identified by
**                           hrgn is NULL, the new clipping region is the 
**                           default clipping region (the default clipping
**                           region is a null region). 
**                RGN_DIFF : The new clipping region combines the areas of
**                           the current clipping region with those areas 
**                           excluded from the region identified by hrgn. 
**                RGN_OR   : The new clipping region combines the current 
**                           clipping region and the region identified by 
**                           hrgn. 
**                RGN_XOR  : The new clipping region combines the current 
**                           clipping region and the region identified by
**                           hrgn but excludes any overlapping areas. 
**  Return   :
**      Return value specifies the region's complexity and can be any one 
**      of the following values: 
**          NULLREGION    Region is empty. 
**          SIMPLEREGION  Region is a single rectangle. 
**          COMPLEXREGION Region is more than one rectangle. 
**          ERROR         An error occurred. (The previous clipping region
**                        is unaffected.) 
**      To get extended error Information, call GetLastError.
**  Remarks  :
**      If an error occurs when this function is called, the previous 
**      clipping region for the specified device context is not affected. 
**      The ExtSelectClipRgn function assumes that the coordinates for the
**      specified region are specified in device units. 
**      Only a copy of the region identified by the hrgn parameter is used.
**      The region itself can be reused after this call or it can be deleted. 
*/
int WINAPI ExtSelectClipRgn(HDC hdc, HRGN hrgn, int nMode)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return ERROR;
    }

    ENTERMONITOR;

    nRet = DC_SelectClipRgn(pDC, hrgn, nMode);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : GetClipBox
**  Purpose  :
**      Retrieves the dimensions of the tightest bounding rectangle that 
**      can be drawn around the current visible area on the device. The 
**      visible area is defined by the current clipping region  as well 
**      as any overlapping windows. 
**  Params   :
**      hdc  : Handle to the device context. 
**      pRect: Pointer to a RECT structure that is to receive the 
**             rectangle dimensions. 
**  Return   :
**      If the function succeeds, return a value specifies the clipping 
**      box's complexity and can be any one of the following values: 
**          NULLREGION      Region is empty. 
**          SIMPLEREGION    Region is a single rectangle. 
**          COMPLEXREGION   Region is more than one rectangle. 
**          ERROR           An error occurred. To get extended error 
**                          information, call GetLastError.
**      GetClipBox returns logical coordinates based on the given device 
**      context. 
*/
int WINAPI GetClipBox(HDC hdc, RECT* pRect)
{
    PDC pDC;
    int nRet;

    if (!pRect)
    {
        SetLastError(1);
        return ERROR;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return ERROR;
    }

    ENTERMONITOR;

    nRet = DC_GetClipBox(pDC, pRect);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : IntersectClipRect
**  Purpose  :
**      Creates a new clipping region from the intersection of the 
**      current clipping region and the specified rectangle. 
**  Params   :
**      hdc  : Handle to the device context.
**      x1   : Logical x-coordinate of upper-left corner of rectangle. 
**      y1   : Logical y-coordinate of upper-left corner of rectangle. 
**      x2   : Logical x-coordinate of lower-right corner of rectangle. 
**      y2   : Logical x-coordinate of lower-right corner of rectangle. 
**  Return  :
**      Return a value specifies the new region's complexity and can be any
**      one of the following values: 
**          NULLREGION      Region is empty. 
**          SIMPLEREGION    Region is a single rectangle. 
**          COMPLEXREGION   Region is more than one rectangle. 
**          ERROR           An error occurred. To get extended error 
**                          information, call GetLastError.
*/
int WINAPI IntersectClipRect(HDC hdc, int x1, int y1, int x2, int y2)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return ERROR;
    }

    ENTERMONITOR;

    nRet = DC_IntersectClipRect(pDC, x1, y1, x2, y2);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : OffsetClipRgn
**  Purpose  :
**      Moves the clipping region of a device context by the specified 
**      offsets. 
**  Params   :
**      hdc  : Handle to device context.
**      x    : The number of logical units to move left or right.
**      y    : The number of logical units to move up or down.
**  Return   :
**      Return a value specifies the new region's complexity and can be any
**      one of the following values: 
**          NULLREGION      Region is empty. 
**          SIMPLEREGION    Region is a single rectangle. 
**          COMPLEXREGION   Region is more than one rectangle. 
**          ERROR           An error occurred. To get extended error 
**                          information, call GetLastError.
*/
int WINAPI OffsetClipRgn(HDC hdc, int x, int y)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return ERROR;
    }

    ENTERMONITOR;

    nRet = DC_OffsetClipRgn(pDC, x, y);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : ExcludeClipRect
**  Purpose  :
**      Creates a new clipping region that consists of the existing 
**      clipping region minus the specified rectangle. 
**  Params   :
**      hdc  : Handle to device context.
**      x1   : Logical x-coordinate of upper-left corner of rectangle. 
**      y1   : Logical y-coordinate of upper-left corner of rectangle.
**      x2   : Logical x-coordinate of lower-right corner of rectangle.
**      y2   : Logical y-coordinate of lower-right corner of rectangle.
**  Return   :
**      Return a value specifies the new region's complexity and can be any
**      one of the following values: 
**          NULLREGION      Region is empty. 
**          SIMPLEREGION    Region is a single rectangle. 
**          COMPLEXREGION   Region is more than one rectangle. 
**          ERROR           An error occurred. To get extended error 
**                          information, call GetLastError.
*/
int WINAPI ExcludeClipRect(HDC hdc, int x1, int y1, int x2, int y2)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return ERROR;
    }

    ENTERMONITOR;

    nRet = DC_ExcludeClipRect(pDC, x1, y1, x2, y2);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : PtVisible
**  Purpose  :
**      Indicates whether the specified point is within the clipping region
**      of a device context. 
**  Params   :
**      hdc  : Handle to device context.
**      x    : Logical x-coordinate of point.
**      y    : Logical y-coordinate of point.
**  Return   :
**      If the specified point is within the clipping region of the device 
**      context, return nonzero. If the specified point is not within the 
**      clipping region of the device context, return zero.
*/
BOOL WINAPI PtVisible(HDC hdc, int x, int y)
{
    PDC pDC;
    BOOL bRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    bRet = DC_PtVisible(pDC, x, y);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : RectVisible
**  Purpose  :
**      Determines whether any part of the specified rectangle lies within
**      the clipping region of a device context. 
**  Params   :
**      hdc   : Handle to device context.
**      pRect : Pointer to a RECT structure that contains the logical 
**              coordinates of the specified rectangle.
**  Return   :
**      If some portion of the specified rectangle lies within the clipping
**      region, return nonzero. If no portion of the specified rectangle 
**      lies within the clipping region, return zero. 
*/
BOOL WINAPI RectVisible(HDC hdc, const RECT* pRect)
{
    PDC pDC;
    BOOL bRet;

    if (!pRect)
        return FALSE;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    bRet = DC_RectVisible(pDC, pRect);

    LEAVEMONITOR;

    return bRet;
}

#endif  // RGNSUPPORT

/**************************************************************************/
/*              General drawing support                                   */
/**************************************************************************/

/*
**  Function : DrawLine
**  Purpose  :
**      Draws a line from specified start point to, but not including, 
**      the specified end point.
**  Params   :
**      hdc : Specifies the window to draw in.
**      x1  : Specifies the x-coordinate of the start point.
**      y1  : Specifies the y-coordinate of the start point.
**      x2  : Specifies the x-coordinate of the end point.
**      y2  : Specifies the y-coordinate of the end point.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return (BOOL)GDI_ERROR;
    }

    ENTERMONITOR;

    DC_SetCurPos(pDC, x1, y1, NULL);
    WS_LineTo(pDC, x2, y2);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : DrawRect
**  Purpose  :
**      Draw a rectangle on the specified device. The rectangle is outlined
**      by using the current pen and filled by using the current brush.
**  Params   :
**      hdc     : Specifies the window to draw in.
**      pRect   : Specifies the coordinates of the rectangle. 
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI DrawRect(HDC hdc, const RECT* pRect)
{
    PDC pDC;
    RECT rect;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC || !pRect)
    {
        SetLastError(1);
        return FALSE;
    }

    CopyRect(&rect, pRect);
    NormalizeRect(&rect);

    ENTERMONITOR;

    WS_DrawRect(pDC, &rect);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : ClearRect
**  Purpose  :
**      Clears a rectangle on the specified device using the specified 
**      color. 
**  Params   :
**      hdc     : Specifies the window to draw in.
**      pRect   : Specifies the coordinates of the rectangle. 
**      color   : Specifies the color to fill the rectangle.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI ClearRect(HDC hdc, const RECT* pRect, COLORREF color)
{
    PDC pDC;
    RECT rect;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC || !pRect)
    {
        SetLastError(1);
        return FALSE;
    }

    CopyRect(&rect, pRect);
    NormalizeRect(&rect);

    ENTERMONITOR;

    WS_ClearRect(pDC, &rect, color);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : DrawCircle
**  Purpose  :
**      Draw a cicle using specified center and radius.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI	DrawCircle(HDC hdc, int x, int y, int r)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_DrawCircle(pDC, x, y, r);

    LEAVEMONITOR;

    return TRUE;
}

#ifdef MSWIN_COMPATIBLE

/*
**  Function : SetPixel
**  Purpose  :
*/
COLORREF WINAPI	SetPixel(HDC hdc, int x, int y, COLORREF crColor)
{
    PDC pDC;
    COLORREF crRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    crRet = WS_SetPixel(pDC, x, y, crColor);

    LEAVEMONITOR;

    return crRet;
}

COLORREF WINAPI	GetPixel(HDC hdc, int x, int y)
{
    PDC pDC;
    COLORREF crRet = (COLORREF)~0;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    crRet = WS_GetPixel(pDC, x, y);

    LEAVEMONITOR;

    return crRet;
}

/*
**  Function : Ellipse
**  Purpose  :
**      Draw a Ellipse using specified rect.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI	Ellipse(HDC hdc, int x1, int y1, int x2, int y2)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_DrawEllipse(pDC, x1, y1, x2, y2);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : SetArcDirection
**  Purpose  :
**      SetArcDirection
**  Return   :
**      If the function succeeds, the return value is old value. 
**      If the function fails, the return value is 0. 
*/
int WINAPI SetArcDirection(HDC hdc, int ArcDirection)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = DC_SetArcDirection(pDC, ArcDirection);

    LEAVEMONITOR;

    return nRet;

}

/*
**  Function : GetArcDirection
**  Purpose  :
**      GetArcDirection
**  Return   :
**      The return value is current value. 
*/
int WINAPI GetArcDirection(HDC hdc)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = DC_GetArcDirection(pDC);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : AngleArc
**  Purpose  :
**      Draw a AngleArc like windows
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/

BOOL WINAPI AngleArc(
  HDC hdc,            // handle to device context
  int x,              // x-coordinate of circle's center
  int y,              // y-coordinate of circle's center
  DWORD dwRadius,     // circle's radius
  double eStartAngle,  // arc's start angle
  double eSweepAngle   // arc's sweep angle
)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_AngleArc(pDC, x, y, dwRadius, eStartAngle, eSweepAngle);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : Arc
**  Purpose  :
**      Draw a Arc like windows
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI Arc(
  HDC hdc,         // handle to device context
  int nLeftRect,   // x-coord of rectangle's upper-left corner
  int nTopRect,    // y-coord of rectangle's upper-left corner
  int nRightRect,  // x-coord of rectangle's lower-right corner
  int nBottomRect, // y-coord of rectangle's lower-right corner
  int nXStartArc,  // x-coord of first radial ending point
  int nYStartArc,  // y-coord of first radial ending point
  int nXEndArc,    // x-coord of second radial ending point
  int nYEndArc     // y-coord of second radial ending point
)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_Arc(pDC, nLeftRect, nTopRect, nRightRect, nBottomRect, 
        nXStartArc, nYStartArc, nXEndArc, nYEndArc, AM_ARC);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : ArcTo
**  Purpose  :
**      ArcTo like windows
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI ArcTo(
  HDC hdc,          // handle to device context
  int nLeftRect,    // x-coord of rectangle's upper-left corner
  int nTopRect,     // y-coord of rectangle's upper-left corner
  int nRightRect,   // x-coord of rectangle's lower-right corner
  int nBottomRect,  // y-coord of rectangle's lower-right corner
  int nXRadial1,    // x-coord of first radial ending point
  int nYRadial1,    // y-coord of first radial ending point
  int nXRadial2,    // x-coord of second radial ending point
  int nYRadial2     // y-coord of second radial ending point
)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_ArcTo(pDC, nLeftRect, nTopRect, nRightRect, nBottomRect, 
        nXRadial1, nYRadial1, nXRadial2, nYRadial2);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : Chord
**  Purpose  :
**      Draw a Chord like windows.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI Chord(
  HDC hdc,         // handle to DC
  int nLeftRect,   // x-coord of upper-left corner of rectangle
  int nTopRect,    // y-coord of upper-left corner of rectangle
  int nRightRect,  // x-coord of lower-right corner of rectangle
  int nBottomRect, // y-coord of lower-right corner of rectangle
  int nXRadial1,   // x-coord of first radial's endpoint
  int nYRadial1,   // y-coord of first radial's endpoint
  int nXRadial2,   // x-coord of second radial's endpoint
  int nYRadial2    // y-coord of second radial's endpoint
)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_Arc(pDC, nLeftRect, nTopRect, nRightRect, nBottomRect, 
        nXRadial1, nYRadial1, nXRadial2, nYRadial2, AM_LUNE);

    LEAVEMONITOR;

    return TRUE;
}

BOOL WINAPI Pie(
  HDC hdc,         // handle to DC
  int nLeftRect,   // x-coord of upper-left corner of rectangle
  int nTopRect,    // y-coord of upper-left corner of rectangle
  int nRightRect,  // x-coord of lower-right corner of rectangle
  int nBottomRect, // y-coord of lower-right corner of rectangle
  int nXRadial1,   // x-coord of first radial's endpoint
  int nYRadial1,   // y-coord of first radial's endpoint
  int nXRadial2,   // x-coord of second radial's endpoint
  int nYRadial2    // y-coord of second radial's endpoint
)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_Arc(pDC, nLeftRect, nTopRect, nRightRect, nBottomRect, 
        nXRadial1, nYRadial1, nXRadial2, nYRadial2, AM_SECTOR);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : Polygon
**  Purpose  :
**      Draw a Polygon using specified points.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI	Polygon(HDC hdc, POINT* ppoints, int count)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_DrawPolygon(pDC, ppoints, count);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : MoveTo
**  Purpose  :
**      Updates the current position to the specified point and 
**      optionally returns the previous position. 
**  Params   :
**      hdc    : Identifies a device context. 
**      x      : Specifies the x-coordinate of the new position. 
**      y      : Specifies the y-coordinate of the new position, 
**      pPoint : Points to a POINT structure in which the previous 
**               current position is stored. If this parameter is a 
**               NULL pointer, the previous position is not returned. 
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI MoveTo(HDC hdc, int x, int y, PPOINT pPoint)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
             
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    DC_SetCurPos(pDC, x, y, pPoint);
    
    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : GetCurrentPosition
**  Purpose  :
**      Retrieves the current position in the specified DC. 
**  Params   :
**      hdc    : Indentifies a device context.
**      pPoint : Points to a POINT structure that receives the coordinates 
**               of the current position. 
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI GetCurrentPosition(HDC hdc, PPOINT pPoint)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC || !pPoint)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    DC_GetCurPos(pDC, pPoint);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : LineTo
**  Purpose  :
**      Draws a line from the current position up to, but not including, 
**      the specified point. 
**  Params   :
**      hdc : Indentifies a device context.
**      x   : Specifies the x-coordinate of the line’s ending point. 
**      y   : Specifies the y-coordinate of the line’s ending point. 
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
**  Remark   :
**      If the function succeeds, the current position is set to the 
**      specified ending point. 
*/
BOOL WINAPI LineTo(HDC hdc, int x, int y)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_LineTo(pDC, x, y);
    
    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : Rectangle
**  Purpose  :
**      Draws a rectangle. The rectangle is outlined by using the current 
**      pen and filled by using the current brush. 
**  Params   :
**      hdc  : Indentifies the device context.
**      x1   : The x-coordinate of the upper-left corner of the rectangle. 
**      y1   : The y-coordinate of the upper-left corner of the rectangle. 
**      x2   : The x-coordinate of the low-right corner of the rectangle. 
**      y2   : The x-coordinate of the low-right corner of the rectangle. 
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI Rectangle(HDC hdc, int x1, int y1, int x2, int y2)
{
    PDC pDC;
    RECT rect;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);

    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    SetRect(&rect, x1, y1, x2, y2);
    NormalizeRect(&rect);

    ENTERMONITOR;

    WS_DrawRect(pDC, &rect);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : RoundRect
**  Purpose  :
**      Draws a rectangle with rounded corners. The rectangle is outlined 
**      by using the current pen and filled by using the current brush. 
**  Params   :
**      hdc     : Indentifies the display device context.
**      x1      : x-coordinate of the upper-left corner of the rectangle. 
**      y1      : y-coord. of bounding rectangle’s upper-left corner.
**      x2      : x-coord. of bounding rectangle’s lower-right corner.
**      y2      : y-coord. of bounding rectangle’s lower-right corner.
**      nWidth  : width of ellipse used to draw rounded corners.
**      nHeight : height of  ellipse used to draw rounded corners.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI RoundRect(HDC hdc, int x1, int y1, int x2, int y2, int nWidth, 
                      int nHeight)
{
    PDC pDC;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_RoundRect(pDC, x1, y1, x2, y2, nWidth, nHeight);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : FillRect
**  Purpose  :
**      Fills a rectangle by using the specified brush. This function 
**      includes the left and top borders, but excludes the right and 
**      bottom borders of the rectangle. 
**  Params   :
**      hdc     : Indentifies the device context.
**      pRect   : Points to a RECT structure that contains the coordinates 
**                of the rectangle to be filled. 
**      hBrush  : Identifies the brush used to fill the rectangle. 
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI FillRect(HDC hdc, const RECT* pRect, HBRUSH hBrush)
{
    PDC pDC;
    RECT rect;
    PBRUSHOBJ pBrush, pOldBrush;

    if (!hdc || !pRect || !hBrush)
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    // Normallizes the specified rectangle
    CopyRect(&rect, pRect);
    NormalizeRect(&rect);

    ENTERMONITOR;

    // If the hBrush is a system color index, using WS_ClearRect to fill
    // the specified rectangle
    if (hBrush < (HBRUSH)(COLOR_MAX + 1))
        WS_ClearRect(pDC, &rect, GetSysColor((long)hBrush - 1));
    else
    {
        pBrush = (PBRUSHOBJ)WOT_LockObj((HANDLE)hBrush, OBJ_BRUSH);
        if (pBrush == NULL)
        {
            LEAVEMONITOR;
            SetLastError(1);
            return FALSE;
        }

        // Selects the specified brush into the specified DC
        pOldBrush = (PBRUSHOBJ)DC_SelectObject(pDC, (PXGDIOBJ)pBrush);
        
        // Call patblt to fill the rectangle using the specified brush
        WS_PatBlt(pDC, rect.left, rect.top, rect.right - rect.left, 
            rect.bottom - rect.top, DC_GetROP2(pDC));
        
        // Restore the old brush to DC
        DC_SelectObject(pDC, (PXGDIOBJ)pOldBrush);

        WOT_UnlockObj((PXGDIOBJ)pBrush);
    }

    LEAVEMONITOR;
    
    return TRUE;
}

/*
**  Funciton : FrameRect
**  Purpose  :
**      Draws a border around the specified rectangle by using the 
**      specified brush. The width and height of the border are always one
**      pixel. 
**  Params   :
**      hdc     : Indentifies the device context.
**      pRect   : Points to a RECT structure that contains the coordinates 
**                of the rectangle to be filled. 
**      hBrush  : Identifies the brush used to draw the rectangle. 
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI FrameRect(HDC  hdc, const RECT* pRect, HBRUSH hBrush)
{
    RECT rect;
    PDC pDC;
    PBRUSHOBJ pBrush;

    if (!pRect)
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pBrush = (PBRUSHOBJ)WOT_LockObj((HANDLE)hBrush, OBJ_BRUSH);
    if (!pBrush)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return FALSE;
    }

    CopyRect(&rect, pRect);
    NormalizeRect(&rect);

    WS_FrameRect(pDC, &rect, pBrush);

    WOT_UnlockObj((PXGDIOBJ)pBrush);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : InvertRect
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI InvertRect(HDC hdc, const RECT* pRect)
{
    RECT rect;
    PDC pDC;

    if (!pRect)
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    CopyRect(&rect, pRect);
    NormalizeRect(&rect);

    ENTERMONITOR;

    WS_PatBlt(pDC, rect.left, rect.top, rect.right - rect.left, 
        rect.bottom - rect.top, ROP_NDST);

    LEAVEMONITOR;

    return TRUE;
}

#endif /* MSWIN_COMPATIBLE */

static PBRUSHOBJ g_pFrameBrush = NULL;
static HWND g_hWndDrag = NULL;
BOOL WINAPI StartDrag(HWND hWnd, HBRUSH hBrush, int width)
{
    static HBRUSH hbrSystemFrame = NULL;
    PBRUSHOBJ pBrush;
    BOOL bRet;

    if (hWnd == NULL || g_hWndDrag != NULL)
        return FALSE;

    if (hbrSystemFrame == NULL)
    {
        hbrSystemFrame = CreateSharedBrush(BS_HATCHED, RGB(0, 0, 0), HS_SIEVE);
        if (hbrSystemFrame == NULL)
        {
            hbrSystemFrame = GetStockObject(LTGRAY_BRUSH);
        }
    }

    if (hBrush == NULL)
    {
        hBrush = hbrSystemFrame;
    }
    if (width == 0)
    {
        width = 4;
    }

    ENTERMONITOR;

    if (g_pFrameBrush == NULL)
    {
        pBrush = (PBRUSHOBJ)WOT_LockObj((HANDLE)hBrush, OBJ_BRUSH);
        if (!pBrush)
        {
            LEAVEMONITOR;
            SetLastError(1);
            return FALSE;
        }
        
        g_pFrameBrush = pBrush;
    }

    bRet = WS_StartDrag(g_pFrameBrush, width);

    g_hWndDrag = hWnd;

    LEAVEMONITOR;

    return bRet;
}

void WINAPI EndDrag(HWND hWnd)
{
    if (hWnd != g_hWndDrag)
        return;

    ENTERMONITOR;

    WS_EndDrag();

    if (g_pFrameBrush)
    {
        WOT_UnlockObj((PXGDIOBJ)g_pFrameBrush);
    }

    g_pFrameBrush = NULL;
    g_hWndDrag = NULL;

    LEAVEMONITOR;

    return;
}

void WINAPI DrawDragFrame(HDC hdc, const RECT* pRect)
{
    PDC pDC;
    
    if (!pRect)
    {
        SetLastError(1);
        return;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return;
    }

    ENTERMONITOR;

    WS_DrawDragFrame(pDC, pRect, NULL);
    LEAVEMONITOR;

}
/*
**  Funciton : FrameRectEx
**  Purpose  :
**      Draws a border around the specified rectangle by using the 
**      specified brush. The width and height of the border are always one
**      pixel. 
**  Params   :
**      hdc     : Indentifies the device context.
**      pRect   : Points to a RECT structure that contains the coordinates 
**                of the rectangle to be filled. 
**      hBrush  : Identifies the brush used to draw the rectangle. 
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI FrameRectEx(HDC  hdc, const RECT* pRect, HBRUSH hBrush, 
                        int width)
{
    RECT rect;
    PDC pDC;
    PBRUSHOBJ pBrush;

    if (!pRect)
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pBrush = (PBRUSHOBJ)WOT_LockObj((HANDLE)hBrush, OBJ_BRUSH);
    if (!pBrush)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return FALSE;
    }

    CopyRect(&rect, pRect);
    NormalizeRect(&rect);

    WS_FrameRectEx(pDC, &rect, pBrush, width, NULL);

    WOT_UnlockObj((PXGDIOBJ)pBrush);

    LEAVEMONITOR;

    return TRUE;
}

/**************************************************************************/
/*              Text Functions                                            */
/**************************************************************************/
DWORD WINAPI SetDefaultChar(HDC hdc, DWORD dwChar)
{
    PDC pDC;
    DWORD dwOldChar;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return 0xFFFFFFFFL;
    }

    dwOldChar = WS_SetDefaultChar(pDC, dwChar);

    return dwOldChar;
}

/*
**  Function : TextOut
**  Purpose  :
**      Writes a character string at the specified location, using the 
**      currently selected font. 
**  Params   :
**      hdc    :
**      x      :
**      y      :
**      pStr   :
**      nCount :
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI TextOut(HDC hdc, int x, int y, PCSTR pStr, int nCount)
{
    PDC pDC;

    if (!pStr || nCount < -1)
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    if (nCount == -1)
        nCount = strlen(pStr);

    if (nCount == 0)
        return TRUE;

    ENTERMONITOR;

    WS_TextOut(pDC, x, y, pStr, nCount);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : GetTextExtent
**  Purpose  :
**      Gets the extent of the specified text.
**  Params   :
**      hdc    : Indentifies the device context.
**      pStr   : Points to the null-terminated string for which extents 
**               are to be retrieved. 
**      nCount : Specifies the number of bytes in the string pointed to 
**               by the lpszStr parameter. 
**      pSize  : Points to a SIZE structure that contains the dimensions 
**               of the string, in pixel when the function returns. This 
**               value cannot be NULL. 
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI GetTextExtent(HDC hdc, PCSTR pStr, int nCount, PSIZE pSize)
{
    PDC pDC;
    BOOL bRet;

    if (!pSize)
    {
        SetLastError(1);
        return FALSE;
    }

    pSize->cx = 0;
    pSize->cy = 0;

    if (!pStr)
    {
        SetLastError(1);
        return FALSE;
    }

    if (hdc)
    {
        pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
        if (!pDC)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pDC = NULL;

    if (nCount == -1)
        nCount = strlen(pStr);

    if (nCount == 0)
        return TRUE;

    ENTERMONITOR;

    bRet = WS_GetTextExtent(pDC, pStr, nCount, pSize);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : GetTextExtentExPoint
**  Purpose  :
**      Gets the extent of the specified text.
**  Params   :
**    HDC hdc,         // handle to DC
**      LPCTSTR lpszStr, // character string
**      int cchString,   // number of characters
**      int nMaxExtent,  // maximum width of formatted string
**      LPINT lpnFit,    // maximum number of characters
**      LPINT alpDx,     // array of partial string widths
**      LPSIZE lpSize    // string dimensions
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI GetTextExtentExPoint(HDC hdc, PCSTR pStr, int nCount, 
                                 int nMaxExtent, PINT lpnFit, 
                                 PINT alpDx, LPSIZE pSize)
{
    PDC pDC;
    BOOL bRet;

    if (!pStr)
    {
        SetLastError(1);
        return FALSE;
    }

    if (lpnFit == NULL)
    {
        return GetTextExtent(hdc, pStr, nCount, pSize);
    }

    if (nCount == 0 && pSize != NULL)
    {
        *lpnFit = 0;
        pSize->cx = 0;
        pSize->cy = 0;
        return TRUE;
    }

    if (nMaxExtent == 0 && pSize == NULL)
    {
        *lpnFit = 0;
        return TRUE;
    }

    if (hdc)
    {
        pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
        if (!pDC)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pDC = NULL;

    if (nCount == -1)
        nCount = strlen(pStr);

    if (nCount <= 0)
        return FALSE;

    ENTERMONITOR;

    bRet = WS_GetTextExtentExPoint(pDC, pStr, nCount, nMaxExtent, 
        lpnFit, alpDx, pSize);

    LEAVEMONITOR;

    return bRet;
}

/*
**  Function : DrawText
**  Purpose  :
**      Draws text in the specified rectangle on specified window.
**  Params   :
**      hdc     : Specifies the device context to draw on.
**      pStr    : Specifies the string to be drawn.
**      nCount  : Specifies the number of characters in the string. 
**                If nCount is -1, then the pString parameter is 
**                assumed to be a pointer to a null-terminated 
**                string and WS_DrawText computes the character count 
**                automatically.
**      pRect   : Points to a RECT structure that contains the 
**                rectangle in which the text is to be formatted.
**      uFormat : Specifies the align mode for drawing text.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI DrawText(HDC hdc, PCSTR pStr, int nCount, PRECT pRect, 
                     UINT uFormat)
{
    PDC pDC;
    RECT rect;

    if (!pRect || (pStr && (nCount < -1)))
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    if (!pStr)
        nCount = 0;
    else
    {
        if (nCount == -1)
            nCount = strlen(pStr);
    }

    CopyRect(&rect, pRect);
    NormalizeRect(&rect);

    ENTERMONITOR;

    WS_DrawText(pDC, pStr, nCount, &rect, uFormat);

    LEAVEMONITOR;
    return TRUE;
}

/*
**  Function : GrayTextOut
**  Purpose  :
**      
*/
BOOL WINAPI GrayTextOut(HDC hdc, int x, int y, PCSTR pStr, int nCount)
{
    PDC pDC;

    if (!pStr || nCount < -1)
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    if (nCount == -1)
        nCount = strlen(pStr);

    if (nCount == 0)
        return TRUE;

    ENTERMONITOR;
    
    WS_TextOut(pDC, x, y, pStr, nCount);

    LEAVEMONITOR;

    return TRUE;
}

/**************************************************************************/
/*              Font Support                                              */
/**************************************************************************/

/*
**  Function : CreateFont
**  Purpose  :
**      Creates a logical font that has specific characteristics. The 
**      logical font can subsequently be selected as the font for any 
**      device. 
**  Return   :
**      If the function succeeds, return a handle to a logical font. 
**      If the function fails, return NULL. To get extended error 
**      information, call GetLastError. 
*/
HFONT   WINAPI CreateFont(int nHeight, int nWidth, int nEscapement, 
                          int nOrientation, int fnWeight, DWORD fdwItalic, 
                          DWORD fdwUnderline, DWORD fdwStrikeOut, 
                          DWORD fdwCharSet, DWORD fdwOutputPrecision, 
                          DWORD fdwClipPrecision, DWORD fdwQuality, 
                          DWORD fdwPitchAndFamily, PCSTR pszFace)
{
    PFONTOBJ pFontObj;
    LOGFONT logfont;

    if (!pszFace)
    {
        SetLastError(1);
        return NULL;
    }

    logfont.lfHeight            = nHeight;
    logfont.lfWidth             = nWidth;
    logfont.lfEscapement        = nEscapement;
    logfont.lfOrientation       = nOrientation;
    logfont.lfWeight            = fnWeight;
    logfont.lfItalic            = (BYTE)fdwItalic;
    logfont.lfUnderline         = (BYTE)fdwUnderline;
    logfont.lfStrikeOut         = (BYTE)fdwStrikeOut;
    logfont.lfCharSet           = (BYTE)fdwCharSet;
    logfont.lfOutPrecision      = (BYTE)fdwOutputPrecision;
    logfont.lfClipPrecision     = (BYTE)fdwClipPrecision;
    logfont.lfQuality           = (BYTE)fdwQuality;
    logfont.lfPitchAndFamily    = (BYTE)fdwPitchAndFamily;

    strncpy(logfont.lfFaceName, pszFace, LF_FACESIZE - 1);
    logfont.lfFaceName[LF_FACESIZE - 1] = 0;

    ENTERMONITOR;

    pFontObj = FONT_Create(&logfont, FALSE);

    LEAVEMONITOR;

    if (!pFontObj)
    {
        SetLastError(1);
        return NULL;
    }

    return (HFONT)WOT_GetHandle((PGDIOBJ)pFontObj);
}

/*
**  Function : CreateFontIndirect
**  Purpose  :
**      Creates a logical font that has the characteristics specified in 
**      the specified structure. The font can subsequently be selected as
**      the current font for any device context. 
**  Params   :
**      pLogFont : Points to a LOGFONT structure that defines the 
**                 characteristics of the logical font. 
**  Return   :
**      If the function succeeds, return a handle to a logical font. 
**      If the function fails, the return value is NULL. 
**  Remark   :
**      The CreateFontIndirect function creates a logical font with the 
**      characteristics specified in the LOGFONT structure. When this 
**      font is selected by using the SelectObject function, GDI’s font
**      mapper attempts to match the logical font with an existing 
**      physical font. If it fails to find an exact match, it provides
**      an alternative whose characteristics match as many of the 
**      requested characteristics as possible. 
**      When you no longer need the font, call the DeleteObject function
**      to delete it. 
*/
HFONT WINAPI CreateFontIndirect(const LOGFONT* pLogFont)
{
    PFONTOBJ pFontObj;

    if (!pLogFont)
    {
        SetLastError(1);
        return NULL;
    }

    ENTERMONITOR;

    pFontObj = FONT_Create(pLogFont, FALSE);

    LEAVEMONITOR;

    if (!pFontObj)
    {
        SetLastError(1);
        return NULL;
    }

    return (HFONT)WOT_GetHandle((PGDIOBJ)pFontObj);
}

/*
**  Function : GetTextMetrics
**  Purpose  :
**      Fills the specified buffer with the metrics for the currently 
**      selected font. 
**  Params   :
**      hdc  : Identifies the device context. 
**      lptm : Points to the TEXTMETRIC structure that is to receive 
**             the metrics. 
**  Return   :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*/
BOOL WINAPI GetTextMetrics(HDC hdc, PTEXTMETRIC lptm)
{
    PDC pDC;

    if (!lptm)
    {
        SetLastError(1);
        return FALSE;
    }

    if (hdc)
    {
        pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
        if (!pDC)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pDC = NULL;

    ENTERMONITOR;

    DC_GetTextMetrics(pDC, lptm);

    LEAVEMONITOR;

    return TRUE;
}

/**************************************************************************/
/*              Bitmap Functions                                          */
/**************************************************************************/

/*
**  Function : CreateBitmap
**  Purpose  :
**      Creates a bitmap with the specified width, height, and color 
**      format (color planes and bits per pixel). 
**  Params   :
**      nWidth      : Specifies the bitmap width, in pixels. 
**      nHeight     : Specifies the bitmap height, in pixels. 
**      cPlanes     : Specifies the number of color planes. 
**      cBitsPixel  : Specifies the number of bits required to identify 
**                    the color of a single pixel. 
**      pBits       : Points to an array of color data used to set the 
**                    colors in a rectangle of pixels. Each scan line in 
**                    the rectangle must be word aligned (scan lines that 
**                    are not word aligned must be padded with zeros). If 
**                    this parameter is NULL, the new bitmap is undefined.
**  Return  :
**      If the funciton succeeds, return a handle to a bitmap.
**      If the funciton fails, return NULL. 
*/
HBITMAP WINAPI CreateBitmap(int nWidth, int nHeight, UINT cPlanes, 
                            UINT cBitsPixel, const void* lpvBits)
{
    PBMPOBJ pBmpObj;
    BITMAP bitmap;

    bitmap.bmType = 0;
    bitmap.bmWidth = nWidth;
    bitmap.bmHeight = nHeight;
    bitmap.bmWidthBytes = ((nWidth * cBitsPixel + 31) & (unsigned long)~31) >> 3;
    bitmap.bmPlanes = cPlanes;
    bitmap.bmBitsPixel = cBitsPixel;
    bitmap.bmBits = (PVOID)lpvBits;

    ENTERMONITOR;

    pBmpObj = BMP_Create(&bitmap, FALSE, FALSE);

    LEAVEMONITOR;

    if (!pBmpObj)
    {
        SetLastError(1);
        return NULL;
    }

    return (HBITMAP)WOT_GetHandle((PGDIOBJ)pBmpObj);
}

/*
**  Function : CreateBitmapEx
**  Purpose  :
**      Creates a bitmap with the specified width, height, and color 
**      format (color planes and bits per pixel). 
**  Params   :
**      nWidth      : Specifies the bitmap width, in pixels. 
**      nHeight     : Specifies the bitmap height, in pixels. 
**      cPlanes     : Specifies the number of color planes. 
**      cBitsPixel  : Specifies the number of bits required to identify 
**                    the color of a single pixel. 
**      pvBits      : When bUserBuf is TRUE, specified the bitmap data 
**                    buffer. When bUserBuf is FALSE, Points to an array 
**                    of color data used to set the colors in a rectangle 
**                    of pixels. Each scan line in the rectangle must be 
**                    dword aligned (scan lines that are not word aligned 
**                    must be padded with zeros). If this parameter is 
**                    NULL, the new bitmap is undefined.
**      bUserBuf    : Specified whether pvBits is bitmap data buffer.
**  Return  :
**      If the funciton succeeds, return a handle to a bitmap.
**      If the funciton fails, return NULL. 
*/
HBITMAP WINAPI CreateBitmapEx(int nWidth, int nHeight, UINT cPlanes, 
                              UINT cBitsPixel, const void* lpvBits, 
                              BOOL bUserBuf)
{
    PBMPOBJ pBmpObj;
    BITMAP bitmap;

    bitmap.bmType = 0;
    bitmap.bmWidth = nWidth;
    bitmap.bmHeight = nHeight;
    bitmap.bmWidthBytes = ((nWidth * cBitsPixel + 31) & (unsigned long)~31) >> 3;
    bitmap.bmPlanes = cPlanes;
    bitmap.bmBitsPixel = cBitsPixel;
    bitmap.bmBits = (PVOID)lpvBits;

    ENTERMONITOR;

    pBmpObj = BMP_Create(&bitmap, bUserBuf, FALSE);

    LEAVEMONITOR;

    if (!pBmpObj)
    {
        SetLastError(1);
        return NULL;
    }

    return (HBITMAP)WOT_GetHandle((PGDIOBJ)pBmpObj);
}

#if (__MP_PLX_GUI)
/*********************************************************************\
* Function	   CreateSharedBitmap
* Purpose      Create a system shared bitmap in MP GUI
* Params	   
* Return	 	   
* Remarks	   
**********************************************************************/
HBITMAP WINAPI CreateSharedBitmap(int nWidth, int nHeight, UINT cPlanes, 
                                  UINT cBitsPixel, const void* lpvBits)
{
    PBMPOBJ pBmpObj;
    BITMAP bitmap;

    bitmap.bmType = 0;
    bitmap.bmWidth = nWidth;
    bitmap.bmHeight = nHeight;
    bitmap.bmWidthBytes = ((nWidth * cBitsPixel + 31) & (unsigned long)~31) >> 3;
    bitmap.bmPlanes = cPlanes;
    bitmap.bmBitsPixel = cBitsPixel;
    bitmap.bmBits = (PVOID)lpvBits;
    
    ENTERMONITOR;
    
    pBmpObj = BMP_Create(&bitmap, FALSE, TRUE);
    
    LEAVEMONITOR;
    
    if (!pBmpObj)
    {
        SetLastError(1);
        return NULL;
    }
    
    return (HBITMAP)WOT_GetHandle((PGDIOBJ)pBmpObj);
}
#endif //__MP_PLX_GUI

/*
**  Function : CreateBitmapIndirect
**  Purpose  :
**      Creates a bitmap with the specified width, height, and color 
**      format (color planes and bits per pixel). 
**  Params   :
**      pBitmap : Points to a BITMAP structure that contains information 
**                about the bitmap. If an application sets the bmWidth or 
**                bmHeight members to zero, CreateBitmapIndirect returns 
**                the handle of a 1- by 1-pixel, monochrome bitmap. 
**  Return   :
**      If the funciton succeeds, return a handle to a bitmap.
**      If the funciton fails, return NULL. 
*/
HBITMAP WINAPI CreateBitmapIndirect(const BITMAP* pBitmap)
{
    PBMPOBJ pBmpObj;

    if (!pBitmap)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pBmpObj = BMP_Create(pBitmap, FALSE, FALSE);

    LEAVEMONITOR;

    if (!pBmpObj)
    {
        SetLastError(1);
        return NULL;
    }

    return (HBITMAP)WOT_GetHandle((PGDIOBJ)pBmpObj);
}

/*
**  Function : CreateCompatibleBitmap
**  Purpose  :
**      Creates a bitmap compatible with the device that is associated 
**      with the specified device context. 
**  Params   :
**      hdc     : Identifies a device context. 
**      nWidth  : Specifies the bitmap width, in pixels. 
**      nHeight : Specifies the bitmap height, in pixels. 
**  Return   :
**      If the funciton succeeds, return a handle to a bitmap.
**      If the funciton fails, return NULL. 
*/
HBITMAP WINAPI CreateCompatibleBitmap(HDC hdc, int nWidth, int nHeight)
{
    PDC pDC;    
    BITMAP bitmap;
    PBMPOBJ pBmpObj;

    if (hdc)
    {
        pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
        if (!pDC)
        {
            SetLastError(1);
            return FALSE;
        }

        // 由于现在兼容位图对象只支持与当前显示设备兼容，如果给定兼容的DC
        // 不是显示DC，暂时设置为空(空默认为显示设备)。这样，无论给定何种
        // DC，都生成与显示设备兼容的位图。生成与其他设备(如打印设备)兼容
        // 的位图今后实现(需要改动wsobmp.c)。
        if (!ISDISPLAYDC(pDC))
            pDC = NULL;
    }
    else
        pDC = NULL;

    ENTERMONITOR;

    bitmap.bmType = 0;
    bitmap.bmWidth = nWidth;
    bitmap.bmHeight = nHeight;
    bitmap.bmPlanes = DC_GetDeviceCaps(pDC, PLANES);
    bitmap.bmBitsPixel = DC_GetDeviceCaps(pDC, BITSPIXEL);
    bitmap.bmBits = NULL;
    bitmap.bmWidthBytes = 
        (bitmap.bmWidth * bitmap.bmBitsPixel + 31) / 32 * 4;

    pBmpObj = BMP_Create(&bitmap, FALSE, FALSE);

    LEAVEMONITOR;

    if (!pBmpObj)
    {
        SetLastError(1);
        return NULL;
    }

    return (HBITMAP)WOT_GetHandle((PGDIOBJ)pBmpObj);
}

/*
**  Function : CreateDIBitmap
**  Purpose  :
**      Creates a device-dependent bitmap (DDB) from a device-independent 
**      bitmap (DIB) and, optionally, sets the bitmap bits. 
**  Params   :
**      hdc         : Identifies a device context. 
**      pbmih       : Points to a BITMAPINFOHEADER structure. 
**      dwInitFlag  : A set of bit flags that specify how the operating 
**                    system initializes the bitmap’s bits. 
**      pInitData   : Points to an array of bytes containing the initial 
**                    bitmap data. The format of the data depends on the 
**                    biBitCount member of the BITMAPINFO structure to 
**                    which the pbmi parameter points. 
**      pbmi        : Points to a BITMAPINFO structure that describes the 
**                    dimensions and color format of the array pointed to
**                    by the pInitData parameter. 
**      uUsage      : Specifies whether the bmiColors member of the 
**                    BITMAPINFO structure was initialized and, if so, 
**                    whether bmiColors contains explicit red, green, blue
**                    (RGB) values or palette indices.
**  Return   :
**      If the function succeeds, return a handle to the bitmap. 
**      If the function fails, return NULL. 
*/
HBITMAP WINAPI CreateDIBitmap(HDC hdc, BITMAPINFOHEADER* pbmih, 
                              DWORD dwInitFlag, const void* pInitData, 
                              BITMAPINFO* pbmi, UINT uUsage)
{
	HBITMAP	hBitmap;
	long	nHeight;

	if (!pbmih)
    {
        SetLastError(1);
        return NULL;
    }

    nHeight = (pbmih->biHeight > 0) ? pbmih->biHeight : -(pbmih->biHeight);
    hBitmap = CreateCompatibleBitmap(hdc, pbmih->biWidth, nHeight);

    if (!hBitmap)
    {
        SetLastError(1);
        return NULL;
    }

    if (dwInitFlag & CBM_INIT)
    {
        int nScanLines;

        nHeight = (pbmi->bmiHeader.biHeight > 0) ? 
            pbmi->bmiHeader.biHeight : -(pbmi->bmiHeader.biHeight);

#if (COLUMN_BITMAP)
        if (pbmi->bmiHeader.biCompression & BI_COLUMN)
            nScanLines = pbmi->bmiHeader.biWidth;
        else
            nScanLines = nHeight;
#else
        nScanLines = nHeight;
#endif

        SetDIBits(hdc, hBitmap, 0, nScanLines, pInitData, pbmi, uUsage);
    }

    return hBitmap;
}

/*********************************************************************\
* Function	   AlphaBlend
* Purpose      The AlphaBlend function displays bitmaps that have 
               transparent or semitransparent pixels.
* Params	   
    hdcDest         [in] Handle to the destination device context. 
    nXOriginDest    [in] Specifies the x-coordinate, in logical units, of 
                         the upper-left corner of the destination rectangle. 
    nYOriginDest    [in] Specifies the y-coordinate, in logical units, of 
                         the upper-left corner of the destination rectangle. 
    nWidthDest      [in] Specifies the width, in logical units, of the 
                         destination rectangle. 
    nHeightDest     [in] Specifies the height, in logical units, of the 
                         destination rectangle. 
    hdcSrc          [in] Handle to the source device context. 
    nXOriginSrc     [in] Specifies the x-coordinate, in logical units, of 
                         the upper-left corner of the source rectangle. 
    nYOriginSrc     [in] Specifies the y-coordinate, in logical units, of 
                         the upper-left corner of the source rectangle. 
    nWidthSrc       [in] Specifies the width, in logical units, of the 
                         source rectangle. 
    nHeightSrc      [in] Specifies the height, in logical units, of the 
                         source rectangle. 
    blendFunction   [in] Specifies the alpha-blending function for source 
                         and destination bitmaps, a global alpha value to 
                         be applied to the entire source bitmap, and format 
                         information for the source bitmap. The source and 
                         destination blend functions are currently limited 
                         to AC_SRC_OVER.  
* Return	 	   
    If the function succeeds, the return value is TRUE.
    If the function fails, the return value is FALSE. 
* Remarks	   
**********************************************************************/
BOOL WINAPI AlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, 
                int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, 
                int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
                BLENDFUNCTION blendFunction)
{
    PDC pDestDC, pSrcDC;

    pDestDC = (PDC)WOT_GetObj((HANDLE)hdcDest, OBJ_DC);
    if (!pDestDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pSrcDC = (PDC)WOT_GetObj((HANDLE)hdcSrc, OBJ_DC);
    if (!pSrcDC)
        pSrcDC = (PDC)WOT_GetObj((HANDLE)hdcSrc, OBJ_BITMAP);

    if (pSrcDC == NULL)
    {
        SetLastError(1);
        return FALSE;
    }


    WS_AlphaBlend(pDestDC, nXOriginDest, nYOriginDest, nWidthDest, 
        nHeightDest, pSrcDC, nXOriginSrc, nYOriginSrc, nWidthSrc, 
        nHeightSrc, &blendFunction);

    LEAVEMONITOR;

    return TRUE;
}

/*********************************************************************\
* Function	   AlphaDIBBlend
* Purpose      The AlphaDIBBlend function displays bitmaps that have 
               transparent or semitransparent pixels.
* Params	   
    hdcDest         [in] Handle to the destination device context. 
    nXOriginDest    [in] Specifies the x-coordinate, in logical units, of 
                         the upper-left corner of the destination rectangle. 
    nYOriginDest    [in] Specifies the y-coordinate, in logical units, of 
                         the upper-left corner of the destination rectangle. 
    nWidthDest      [in] Specifies the width, in logical units, of the 
                         destination rectangle. 
    nHeightDest     [in] Specifies the height, in logical units, of the 
                         destination rectangle. 
    pInitData       [in] Pointer to an array of bytes containing the 
                         initial bitmap data. The format of the data depends 
                         on the biBitCount member of the BITMAPINFO 
                         structure to which the lpbmi parameter points. 
    lpbmi           [in] Pointer to a BITMAPINFO structure that describes 
                         the dimensions and color format of the array 
                         pointed to by the lpbInit parameter. 
    fuUsage         [in] Specifies whether the bmiColors member of the 
                         BITMAPINFO structure was initialized and, if so, 
                         whether bmiColors contains explicit red, green, 
                         blue (RGB) values or palette indexes. The fuUsage 
                         parameter must be one of the following values. 
                         Value Meaning 
        DIB_PAL_COLORS  A color table is provided and consists of an array 
                        of 16-bit indexes into the logical palette of the 
                        device context into which the bitmap is to be 
                        selected. 
        DIB_RGB_COLORS  A color table is provided and contains literal 
                        RGB values. 
    nXOriginSrc     [in] Specifies the x-coordinate, in logical units, of 
                         the upper-left corner of the source rectangle. 
    nYOriginSrc     [in] Specifies the y-coordinate, in logical units, of 
                         the upper-left corner of the source rectangle. 
    nWidthSrc       [in] Specifies the width, in logical units, of the 
                         source rectangle. 
    nHeightSrc      [in] Specifies the height, in logical units, of the 
                         source rectangle. 
    blendFunction   [in] Specifies the alpha-blending function for source 
                         and destination bitmaps, a global alpha value to 
                         be applied to the entire source bitmap, and format 
                         information for the source bitmap. The source and 
                         destination blend functions are currently limited 
                         to AC_SRC_OVER.  
* Return	 	   
    If the function succeeds, the return value is TRUE.
    If the function fails, the return value is FALSE. 
* Remarks	   
**********************************************************************/
BOOL WINAPI AlphaDIBBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, 
                int nWidthDest, int nHeightDest, const void* pInitData, 
                const BITMAPINFO* pbmi, UINT uUsage, int nXOriginSrc, 
                int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
                BLENDFUNCTION blendFunction)
{
    PDC pDestDC;

    pDestDC = (PDC)WOT_GetObj((HANDLE)hdcDest, OBJ_DC);
    if (!pDestDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_AlphaDIBBlend(pDestDC, nXOriginDest, nYOriginDest, nWidthDest, 
        nHeightDest, pInitData, pbmi, uUsage, nXOriginSrc, nYOriginSrc, 
        nWidthSrc, nHeightSrc, &blendFunction);

    LEAVEMONITOR;

    return TRUE;
}

/*********************************************************************\
* Function	   TransparentBlt
* Purpose      The TransparentBlt function performs a bit-block transfer 
               of the color data corresponding to a rectangle of pixels 
               from the specified source device context into a destination 
               device context.
* Params	   
    crTransparent   [in] The RGB color in the source bitmap to treat as 
                         transparent. 

* Return
    If the function succeeds, the return value is TRUE.
    If the function fails, the return value is FALSE. 
* Remarks	   
**********************************************************************/
BOOL WINAPI TransparentBlt(HDC hdcDest, int nXOriginDest, int nYOriginDest, 
                int nWidthDest, int nHeightDest, HDC hdcSrc, int nXOriginSrc, 
                int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
                UINT crTransparent)
{
    PDC pDestDC, pSrcDC;

    pDestDC = (PDC)WOT_GetObj((HANDLE)hdcDest, OBJ_DC);
    if (!pDestDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pSrcDC = (PDC)WOT_GetObj((HANDLE)hdcSrc, OBJ_DC);
    if (!pSrcDC)
        pSrcDC = (PDC)WOT_GetObj((HANDLE)hdcSrc, OBJ_BITMAP);

    WS_TransparentBlt(pDestDC, nXOriginDest, nYOriginDest, nWidthDest, 
        nHeightDest, pSrcDC, nXOriginSrc, nYOriginSrc, nWidthSrc, 
        nHeightSrc, crTransparent);

    LEAVEMONITOR;

    return TRUE;
}

/*********************************************************************\
* Function	   TransparentDIBits
* Purpose      The TransparentDIBits function performs a bit-block transfer 
               of the color data corresponding to a rectangle of pixels 
               from the specified source DIB into a destination 
               device context.
* Params	   
    crTransparent   [in] The RGB color in the source bitmap to treat as 
                         transparent. 

* Return
    If the function succeeds, the return value is TRUE.
    If the function fails, the return value is FALSE. 
* Remarks	   
**********************************************************************/
BOOL WINAPI TransparentDIBits(HDC hdcDest, int nXOriginDest, int nYOriginDest, 
                int nWidthDest, int nHeightDest, const void* pInitData, 
                const BITMAPINFO* pbmi, UINT uUsage, int nXOriginSrc, 
                int nYOriginSrc, int nWidthSrc, int nHeightSrc, 
                UINT crTransparent)
{
    PDC pDestDC;

    pDestDC = (PDC)WOT_GetObj((HANDLE)hdcDest, OBJ_DC);
    if (!pDestDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_TransparentDIBits(pDestDC, nXOriginDest, nYOriginDest, nWidthDest, 
        nHeightDest, pInitData, pbmi, uUsage, nXOriginSrc, nYOriginSrc, 
        nWidthSrc, nHeightSrc, crTransparent);

    LEAVEMONITOR;

    return TRUE;
}


/*
**  Function : BitBlt
**  Purpose  :
**      Performs a bit-block transfer of the color data corresponding 
**      to a rectangle of pixels from the specified source device 
**      context into a destination device context. 
**  Params   :
**      hdcDest : Identifies the destination device context.
**      nDestX  : Specifies the x-coordinate of the upper-left corner 
**                of the destination rectangle. 
**      nDestY  : Specifies the y-coordinate of the upper-left corner 
**                of the destination rectangle. 
**      nWidth  : Specifies the width of the source and destination 
**                rectangles. 
**      nHeight : Specifies the height of the source and destination 
**                rectangles. 
**      hdcSrc  : Identifies the source device context. 
**      nSrcX   : Specifies the x-coordinate of the upper-left corner 
**                of the source rectangle. 
**      nSrcY   : Specifies the x-coordinate of the upper-left corner 
**                of the source rectangle. 
**      dwRop   : Specifies a raster-operation code.
**  Return   :
**      If the function succeeds, the return value is TRUE. 
**      If the function fails, the return value is FALSE. 
*/
BOOL WINAPI BitBlt(HDC hdcDest, int nDestX, int nDestY, int nWidth, 
                   int nHeight, HDC hdcSrc, int nSrcX, int nSrcY, 
                   DWORD dwRop)
{
    PDC pDestDC, pSrcDC;
    BOOL bBitmap = FALSE;

    pDestDC = (PDC)WOT_GetObj((HANDLE)hdcDest, OBJ_DC);
    if (!pDestDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pSrcDC = (PDC)WOT_GetObj((HANDLE)hdcSrc, OBJ_DC);
    if (!pSrcDC)
    {
        pSrcDC = (PDC)WOT_LockObj((HANDLE)hdcSrc, OBJ_BITMAP);
        if (pSrcDC != NULL)
            bBitmap = TRUE;
    }

    WS_BitBlt(pDestDC, nDestX, nDestY, nWidth, nHeight, pSrcDC, 
        nSrcX, nSrcY, dwRop);

    if (bBitmap)
        WOT_UnlockObj((PXGDIOBJ)pSrcDC);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : StretchBlt
**  Purpose  :
**      Copies a bitmap from a source rectangle into a destination 
**      rectangle, stretching or compressing the bitmap to fit the 
**      dimensions of the destination rectangle, if necessary. Windows
**      stretches or compresses the bitmap according to the stretching
**      mode currently set in the destination device context. 
**  Params   :
**      hdcDest : Identifies the destination device context. 
**      nDestX  : Specifies the x-coordinate, in logical units, of the 
**                upper-left corner of the destination rectangle. 
**      nDestY  : Specifies the y-coordinate, in logical units, of the 
**                upper-left corner of the destination rectangle. 
**      nDestW  : Specifies the width, in logical units, of the destination
**                rectangle. 
**      nDestH  : Specifies the height, in logical units, of the 
**                destination rectangle. 
**      hdcSrc  : Identifies the source device context. 
**      nSrcX   : Specifies the x-coordinate, in logical units, of the 
**                upper-left corner of the source rectangle. 
**      nSrcY   : Specifies the y-coordinate, in logical units, of the 
**                upper-left corner of the source rectangle. 
**      nSrcW   : Specifies the width, in logical units, of the source 
**                rectangle. 
**      nSrcH   : Specifies the height, in logical units, of the source 
**                rectangle. 
**      dwRop   : Specifies the raster operation to be performed. Raster 
**                operation codes define how Windows combines colors in 
**                output operations that involve a brush, a source bitmap,
**                and a destination bitmap. 
**  Return    :
**      If the function succeeds, return nonzero. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
*/
BOOL WINAPI StretchBlt(HDC hdcDest, int nDestX, int nDestY, int nDestW,
                       int nDestH, HDC hdcSrc, int nSrcX, int nSrcY, 
                       int nSrcW, int nSrcH, DWORD dwRop)
{
    PDC pDestDC, pSrcDC;
    BOOL bBitmap = FALSE;

    pDestDC = (PDC)WOT_GetObj((HANDLE)hdcDest, OBJ_DC);
    if (!pDestDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    pSrcDC = (PDC)WOT_GetObj((HANDLE)hdcSrc, OBJ_DC);
    if (!pSrcDC)
    {
        pSrcDC = (PDC)WOT_LockObj((HANDLE)hdcSrc, OBJ_BITMAP);
        if (pSrcDC != NULL)
            bBitmap = TRUE;
    }

    WS_StretchBlt(pDestDC, nDestX, nDestY, nDestW, nDestH, pSrcDC, 
        nSrcX, nSrcY, nSrcW, nSrcH, dwRop);

    if (bBitmap)
        WOT_UnlockObj((PXGDIOBJ)pSrcDC);

    LEAVEMONITOR;

    return TRUE;
}


/*
**  Function : PatBlt
**  Purpose  :
**      Paints the given rectangle using the brush that is currently 
**      selected into the specified device context. The brush color 
**      and the surface color(s) are combined by using the given 
**      raster operation. 
**  Params   :
**      hdc     : Indentifies the device context.
**      x       : x-coord. of upper-left corner of rect. to be filled 
**      y       : y-coord. of upper-left corner of rect. to be filled 
**      width   : Width of rectangle to be filled.
**      height  : Height of rectangle to be filled.
**      dwRop   : Specifies the raster operation code.
*/
BOOL WINAPI PatBlt(HDC hdc, int x, int y, int width, int height, 
                   DWORD dwRop)
{
    PDC pDC;

    if (width < 0 || height < 0)
    {
        SetLastError(1);
        return FALSE;
    }

    if (width == 0 || height == 0)
        return TRUE;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    ENTERMONITOR;

    WS_PatBlt(pDC, x, y, width, height, dwRop);

    LEAVEMONITOR;

    return TRUE;
}

/*
**  Function : SetBitmapBits
**  Purpose  :
**      Sets the bits of color data for a bitmap to the specified values. 
**  Params   :
**      hBitmap : Identifies the bitmap to be set. 
**      cBytes  : Specifies the number of bytes pointed to by the pBits. 
**      pBits   : Points to an array of bytes that contain color data for 
**                the specified bitmap. 
**  Return   :
**      If the function succeeds, return the number of bytes used in 
**      setting the bitmap bits. If the function fails, return zero. 
*/
LONG WINAPI SetBitmapBits(HBITMAP hBitmap, DWORD cBytes, const void* pBits)
{
    PBMPOBJ pBmpObj;
    LONG nRet;

    if (cBytes == 0 || !pBits)
        return 0;

    ENTERMONITOR;
    
    pBmpObj = (PBMPOBJ)WOT_LockObj((HANDLE)hBitmap, OBJ_BITMAP);
    if (!pBmpObj)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return 0;
    }

    nRet = BMP_SetBits(pBmpObj, cBytes, pBits);
    
    WOT_UnlockObj((PXGDIOBJ)pBmpObj);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : SetDIBits
**  Purpose  :
**      Sets the pixels in a bitmap using the color data found in the 
**      specified device-independent bitmap (DIB). 
*/
int WINAPI SetDIBits(HDC hdc, HBITMAP hbmp, UINT uStartScan, 
                     UINT cScanLines, CONST VOID *lpvBits, 
                     CONST BITMAPINFO *lpbmi, UINT fuColorUse)
{
    PDC     pDC;
    PBMPOBJ pBmpObj;
    int nRet;

    if (!lpbmi || !lpvBits)
    {
        SetLastError(1);
        return 0;
    }

    if (hdc)
    {
        pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
        if (!pDC)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pDC = NULL;

    ENTERMONITOR;
    pBmpObj = (PBMPOBJ)WOT_LockObj((HANDLE)hbmp, OBJ_BITMAP);
    if (!pBmpObj)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return 0;
    }

    nRet = BMP_SetDIBits(pDC, pBmpObj, uStartScan, cScanLines, lpvBits, 
        lpbmi, fuColorUse);

    WOT_UnlockObj((PXGDIOBJ)pBmpObj);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : GetDIBits
**  Purpose  :
**      Retrieves the bits of the specified bitmap and copies them into a 
**      buffer using the specified format. 
**  Params   :
**      hdc         : Identifies the device context. 
**      hbmp        : Identifies the bitmap.
**      uStartScan  : Specifies the first scan line to retrieve. 
**      cScanLines  : Specifies the number of scan lines to retrieve. 
**      lpvBits     : Points to a buffer to receive the bitmap data. If 
**                    this parameter is NULL, the function passes the 
**                    dimensions and format of the bitmap to the BITMAPINFO
**                    structure pointed to by the lpbi parameter. 
**      lpbi        : Points to a BITMAPINFO structure that specifies the 
**                    desired format for the device-independent bitmap 
**                    (DIB) data. 
**      uUsage      : Specifies the format of the bmiColors member of the 
**                    BITMAPINFO structure. It must DIB_RGB_COLORS.
**  Return   :
**      If the lpvBits parameter is non-NULL and the function succeeds, 
**      return the number of scan lines copied from the bitmap. 
**      If the lpvBits parameter is NULL and GetDIBits successfully fills
**      the BITMAPINFO structure, the return value is the total number of
**      scan lines in the bitmap. 
**  Remarks  :
**      If the lpvBits parameter is a valid pointer, the first six members 
**      of the BITMAPINFOHEADER structure must be initialized to specify 
**      the size and format of the DIB. Note that a bottom-up DIB is 
**      specified by setting the height to a positive number, while a 
**      top-down DIB is specified by setting the height to a negative 
**      number. The bitmap’s color table will be appended to the 
**      BITMAPINFO structure. 
**      If lpvBits is NULL, GetDIBits examines the first member of the 
**      first structure pointed to by lpbi. This member must specify the 
**      size, in bytes, of a BITMAPCOREHEADER or a BITMAPINFOHEADER 
**      structure. The function uses the specified size to determine how
**      the remaining members should be initialized. 
**      If lpvBits is NULL and the bit count member of BITMAPINFO is 
**      initialized to zero, GetDIBits fills in BITMAPINFOHEADER or 
**      BITMAPCOREHEADER without the color table. This technique can be 
**      used to query bitmap attributes. 
**      The bitmap identified by the hbmp parameter must not be selected 
**      into a device context when the application calls this function. 
**      The origin for a bottom-up DIB is the lower-left corner of the 
**      bitmap; the origin for a top-down DIB is the upper-left corner. 
*/
int WINAPI GetDIBits(HDC hdc, HBITMAP hbmp, UINT uStartScan, 
                     UINT cScanLines, PVOID lpvBits, PBITMAPINFO lpbi, 
                     UINT uUsage)
{
    PDC         pDC;
    PBMPOBJ     pBmpObj;
    int nRet;

    if (!lpbi)
    {
        SetLastError(1);
        return 0;
    }

    if (hdc)
    {
        pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
        if (!pDC)
        {
            SetLastError(1);
            return FALSE;
        }
    }
    else
        pDC = NULL;

    ENTERMONITOR;
    pBmpObj = (PBMPOBJ)WOT_LockObj((HANDLE)hbmp, OBJ_BITMAP);
    if (!pBmpObj)
    {
        LEAVEMONITOR;
        SetLastError(1);
        return 0;
    }

    nRet = BMP_GetDIBits(pDC, pBmpObj, uStartScan, cScanLines, lpvBits, 
        lpbi, uUsage);

    WOT_UnlockObj((PXGDIOBJ)pBmpObj);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : SetDIBitsToDevice
**  Purpose  :
**      Sets the pixels in the specified rectangle on the device that is 
**      associated with the destination device context using color data 
**      from a device-independent bitmap (DIB). 
**  Params   :
**      hdc         : Identifies the device context. 
**      nDestX      : Specifies the x-coordinate, in logical units, of the 
**                    upper-left corner of the destination rectangle. 
**      nDestY      : Specifies the y-coordinate, in logical units, of the 
**                    upper-left corner of the destination rectangle. 
**      dwWidth     : Specifies the width, in logical units, of the DIB. 
**      dwHeight    : Specifies the height, in logical units, of the DIB.  
**      nSrcX       : Specifies the x-coordinate, in logical units, of the 
**                    lower-left corner of the DIB.
**      nSrcY       : Specifies the y-coordinate, in logical units, of the 
**                    lower-left corner of the DIB. 
**      uStartScan  : Specifies the starting scan line in the DIB. 
**      cScanLines  : Specifies the number of DIB scan lines contained in 
**                    the array pointed to by the lpvBits parameter. 
**      lpvBits     : Points to DIB color data stored as an array of bytes. 
**      lpbmi       : Points to a BITMAPINFO structure that contains 
**                    information about the DIB. 
**      fuColorUse  : RGB or palette indices.
**  Return   :
**      If the function succeeds, return the number of scan lines set. 
**      If the function fails, return zero. To get extended error 
**      information, call GetLastError. 
**  Remarks  :
**      The origin of a bottom-up DIB is the lower-left corner of the 
**      bitmap; the origin of a top-down DIB is the upper-left corner. 
**      To reduce the amount of memory required to set bits from a large 
**      device-independent bitmap on a device surface, an application can
**      band the output by repeatedly calling SetDIBitsToDevice, placing 
**      a different portion of the bitmap into the lpvBits array each time. 
**      The values of the uStartScan and cScanLines parameters identify 
**      the portion of the bitmap contained in the lpvBits array. 
*/
int WINAPI SetDIBitsToDevice(HDC hdc, int nDestX, int nDestY, 
                             DWORD dwWidth, DWORD dwHeight, int nSrcX, 
                             int nSrcY, UINT uStartScan, 
                             UINT cScanLines, CONST VOID* lpvBits, 
                             CONST BITMAPINFO* lpbmi, UINT fuColorUse)
{
    PDC pDestDC;
    int nRet;

    if (!lpvBits || !lpbmi || lpbmi->bmiHeader.biHeight == 0 || 
        lpbmi->bmiHeader.biWidth <= 0)
    {
        SetLastError(1);
        return 0;
    }
    
    pDestDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDestDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = WS_SetDIBitsToDevice(pDestDC, nDestX, nDestY, dwWidth, dwHeight,
        nSrcX, nSrcY, uStartScan, cScanLines, lpvBits, lpbmi, fuColorUse);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : StretchDIBits
**  Purpose  :
**      Copies the color data for a rectangle of pixels in a 
**      device-independent bitmap (DIB) to the specified destination 
**      rectangle. If the destination rectangle is larger than the source 
**      rectangle, this function stretches the rows and columns of color 
**      data to fit the destination rectangle. If the destination rectangle
**      is smaller than the source rectangle, this function compresses the 
**      rows and columns by using the specified raster operation. 
*/
int WINAPI StretchDIBits(HDC hdc, int nDestX, int nDestY, int nDestWidth,
                         int nDestHeight, int nSrcX, int nSrcY, 
                         int nSrcWidth, int nSrcHeight, 
                         CONST VOID* lpvBits, CONST BITMAPINFO *lpbmi,
                         UINT iUsage, DWORD dwRop)
{
    PDC pDestDC;
    int nRet;

    if (!lpvBits || !lpbmi || lpbmi->bmiHeader.biHeight == 0 || 
        lpbmi->bmiHeader.biWidth <= 0)
    {
        SetLastError(1);
        return 0;
    }
    
    pDestDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDestDC)
    {
        SetLastError(1);
        return 0;
    }

    ENTERMONITOR;

    nRet = WS_StretchDIBits(pDestDC, nDestX, nDestY, nDestWidth, 
        nDestHeight, nSrcX, nSrcY, nSrcWidth, nSrcHeight, lpvBits, lpbmi, 
        DIB_RGB_COLORS, dwRop);

    LEAVEMONITOR;

    return nRet;
}

//*************************************************************************
//          Printer support API
//*************************************************************************

#if(PRINTERSUPPORT)

/*
**  Function : CreatePrinterDC
*/
HDC WINAPI CreatePrinterDC(int width, int height)
{
    PDC pDC;

    if (width <= 0 || height <= 0)
        return NULL;

    ENTERMONITOR;

    pDC = DC_CreatePrinterDC(width, height);

    LEAVEMONITOR;

    if (!pDC)
    {
        SetLastError(1);
        return NULL;
    }

    return (HDC)WOT_GetHandle(pDC);
}

int WINAPI StartDoc(HDC hdc, const DOCINFO* pDocInfo)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
        return 0;

    ENTERMONITOR;

    nRet = DC_StartDoc(pDC, pDocInfo);

    LEAVEMONITOR;

    return nRet;
}

int WINAPI EndDoc(HDC hdc)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
        return 0;

    ENTERMONITOR;

    nRet = DC_EndDoc(pDC);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : StartPage
**  Purpose  :
*/
int WINAPI StartPage(HDC hdc)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
        return 0;

    ENTERMONITOR;

    nRet = DC_StartPage(pDC);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : EndPage
**  Purpose  :
**      Informs the device that the application has finished writing to a 
**      page. 
*/
int WINAPI EndPage(HDC hdc)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
        return 0;

    ENTERMONITOR;

    nRet = DC_EndPage(pDC);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : StartBand
**  Purpose  :
*/
int WINAPI StartBand(HDC hdc, PRECT pRect)
{
    PDC pDC;
    int nRet;

    if (!pRect)
    {
        SetLastError(1);
        return 0;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
        return 0;

    ENTERMONITOR;

    nRet = DC_StartBand(pDC, pRect);

    LEAVEMONITOR;

    return nRet;
}

/*
**  Function : EndBand
**  Purpose  :
**      Informs the device that the application has finished writing to a 
**      band. 
*/
int WINAPI EndBand(HDC hdc)
{
    PDC pDC;
    int nRet;

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
        return 0;

    ENTERMONITOR;

    nRet = DC_EndBand(pDC);

    LEAVEMONITOR;

    return nRet;
}

#endif // PRINTERSUPPORT

//*************************************************************************
//          Two bitmap functions for Legend PDA applications
//*************************************************************************

#include "hpfile.h"

/*
**  Function : DrawBitmapFromData
**  Purposen  :
**      Draws compatiable bitmap or mono bitmap on the specified device.
**
*/
BOOL WINAPI DrawBitmapFromData(HDC hdc, int x, int y, int nWidth, 
                               int nHeight, UINT cPlanes, UINT cBitsPerPel,
                               const void* pvBits, DWORD dwRop)
{
    RGBQUAD MonoPalette[2] = 
    {
        {  0,   0,   0, 0}, 
        {255, 255, 255, 0}
    };

    PDC pDC;
    BOOL bRet;

    if (!pvBits || nWidth <= 0 || nHeight <= 0 || cPlanes != 1)
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    // 兼容位图
    if ((int)cBitsPerPel == DISPDEV_GetCaps(BITSPIXEL))
    {
        ENTERMONITOR;

        bRet = WS_DrawBitmap(pDC, x, y, nWidth, nHeight, 
            0, 0, pvBits, dwRop);

        LEAVEMONITOR;
        return bRet;
    }

    // 单色位图
    if (cBitsPerPel == 1)
    {
        DIBITMAP DIBitmap;
        COLORREF crBack, crText;

        ENTERMONITOR;

        crBack = DC_GetBkColor(pDC);
        crText = DC_GetTextColor(pDC);
        
        MonoPalette[0].rgbBlue = GetBValue(crText);
        MonoPalette[0].rgbGreen = GetGValue(crText);
        MonoPalette[0].rgbRed = GetRValue(crText);
        MonoPalette[1].rgbBlue = GetBValue(crBack);
        MonoPalette[1].rgbGreen = GetGValue(crBack);
        MonoPalette[1].rgbRed = GetRValue(crBack);

        DIBitmap.type = BMP_MONO;
        DIBitmap.width = nWidth;
        DIBitmap.height = -nHeight;
        DIBitmap.palette_entries = 2;
        DIBitmap.bmp_size = (((nWidth + 31) & ~31) >> 3) * nHeight;
        DIBitmap.pBuffer = (void*)pvBits;
        DIBitmap.pPalette = (void*)MonoPalette;

        bRet = WS_DrawDIBitmap(pDC, x, y, 0, 0, &DIBitmap, dwRop);

        LEAVEMONITOR;

        return bRet;
    }

    // 非兼容位图和单色位图不支持
    SetLastError(1);
    return FALSE;
}

/*
**  Function : DrawBitmapFromDataEx
**  Purpose  :
**      Draws compatiable bitmap or mono bitmap on the specified device.
**
*/
BOOL WINAPI DrawBitmapFromDataEx(HDC hdc, int x, int y, int nWidth, 
                                 int nHeight, int srcx, int srcy, 
                                 UINT cPlanes, UINT cBitsPerPel,
                                 const void* pvBits, DWORD dwRop)
{
    RGBQUAD MonoPalette[2] = 
    {
        {  0,   0,   0, 0}, 
        {255, 255, 255, 0}
    };

    PDC pDC;
    BOOL bRet;

    if (!pvBits || nWidth <= 0 || nHeight <= 0 || cPlanes != 1)
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    // 兼容位图
    if ((int)cBitsPerPel == DISPDEV_GetCaps(BITSPIXEL))
    {
        ENTERMONITOR;

        bRet = WS_DrawBitmap(pDC, x, y, nWidth, nHeight, 
            srcx, srcy, pvBits, dwRop);

        LEAVEMONITOR;

        return bRet;
    }

    // 单色位图
    if (cBitsPerPel == 1)
    {
        DIBITMAP DIBitmap;
        COLORREF crBack, crText;

        ENTERMONITOR;

        crBack = DC_GetBkColor(pDC);
        crText = DC_GetTextColor(pDC);
        
        MonoPalette[0].rgbBlue = GetBValue(crText);
        MonoPalette[0].rgbGreen = GetGValue(crText);
        MonoPalette[0].rgbRed = GetRValue(crText);
        MonoPalette[1].rgbBlue = GetBValue(crBack);
        MonoPalette[1].rgbGreen = GetGValue(crBack);
        MonoPalette[1].rgbRed = GetRValue(crBack);

        DIBitmap.type = BMP_MONO;
        DIBitmap.width = nWidth;
        DIBitmap.height = -nHeight;
        DIBitmap.palette_entries = 2;
        DIBitmap.bmp_size = (((nWidth + 31) & ~31) >> 3) * nHeight;
        DIBitmap.pBuffer = (void*)pvBits;
        DIBitmap.pPalette = (void*)MonoPalette;

        bRet = WS_DrawDIBitmap(pDC, x, y, srcx, srcy, &DIBitmap, dwRop);

        LEAVEMONITOR;

        return bRet;
    }

    // 非兼容位图和单色位图不支持
    SetLastError(1);
    return FALSE;
}

//*************************************************************************
//          Definitions for DrawBitmapFromFile function
//*************************************************************************

// 防止字序问题的两个宏
#define GET16(p) (WORD) (((WORD)(*((BYTE*)(p) + 1)) << 8) + *((BYTE*)(p)))
#define GET32(p) (DWORD)(((DWORD)(*((BYTE*)(p) + 3)) << 24) + \
                         ((DWORD)(*((BYTE*)(p) + 2)) << 16)  + \
                         ((DWORD)(*((BYTE*)(p) + 1)) << 8) +  *((BYTE*)(p)))

// 定义位图文件中一些字段的偏移量
#define OFFSET_TYPE             0       // 位图标识字段偏移量
#define OFFSET_OFFBITS          10      // 位图数据偏移字段偏移量
#define OFFSET_BITMAPINFOHEADER 14      // 位图头信息字段偏移量

/*
**  Function : DrawBitmapFromFile
**  Purpose  :
**      Draws a bitmap specified by a bitmap file on the specified DC.
**  Params   :
**      hdc         : Identifies the device context.
**      x           : Specifies the x-coordinate of the upper-left corner 
**                    of the destination rectangle. 
**      y           : Specifies the y-coordinate of the upper-left corner 
**                    of the destination rectangle. 
**      pszBmpFile  : Specifies the bitmap file.
**      dwRop       : Specifies a raster-operation code.
*/
BOOL WINAPI DrawBitmapFromFile(HDC hdc, int x, int y, LPCSTR pszBmpFile, 
                               DWORD dwRop)
{
    PDC     pDC;
    BYTE*   pFileData;
    BYTE*   pData;
    int     nType, nOffBits, nBitCount, nCompression;
    int     nHeight;
    BOOL    bReturn;
    int     hFile;
    DWORD   dwFileSize, biSizeImage;
    DIBITMAP DIBitmap;

	if (!pszBmpFile)
    {
        SetLastError(1);
        return FALSE;
    }

    pDC = (PDC)WOT_GetObj((HANDLE)hdc, OBJ_DC);
    if (!pDC)
    {
        SetLastError(1);
        return FALSE;
    }

    // Open the bitmap file.
    hFile = PLXOS_CreateFile(pszBmpFile, PLXFS_ACCESS_READ, 0);
    if (hFile == -1)
    {
        SetLastError(1);
        return FALSE;
    }

    // 尝试映射位图文件
    dwFileSize = PLXOS_GetFileSize(hFile);
    pFileData = (BYTE*)PLXOS_MapFile(hFile, 0, dwFileSize);

    // 暂时只支持能够映射的文件，不能映射失败返回
    if (!pFileData)
    {
        PLXOS_CloseFile(hFile);

        SetLastError(1);
        return FALSE;
    }

    pData = pFileData;

    // If the file is a invalid bitmap file, just return
    if ((nType = GET16(pData + OFFSET_TYPE)) != 0x4d42)
    {
        PLXOS_UnmapFile(hFile, pFileData, dwFileSize);
        PLXOS_CloseFile(hFile);

        SetLastError(1);
        return FALSE;
    }

    // Gets the bfOffbits field of BITMAPFILEHEADER
    nOffBits = GET32(pData + OFFSET_OFFBITS);

    // Skips the bitmap file header
    pData += OFFSET_BITMAPINFOHEADER;

    // Gets BITMAPINFOHEADER from bitmap file data
    DIBitmap.width = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biWidth));
    DIBitmap.height = 
        GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biHeight)); 
    nBitCount = GET16(pData + FIELDOFFSET(BITMAPINFOHEADER, biBitCount)); 
    nCompression = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, 
        biCompression)); 

    biSizeImage = GET32(pData + FIELDOFFSET(BITMAPINFOHEADER, biSizeImage));

	if (dwFileSize <= biSizeImage)
        return FALSE;

    switch (nBitCount)
    {
    case 1 :

        DIBitmap.type = BMP_MONO;
        break;

    case 2 :

        DIBitmap.type = BMP_GRAY4;
        break;

    case 4 :

        DIBitmap.type = BMP_16;
        break;

    case 8 :

        DIBitmap.type = BMP_256;
        break;

    case 16 :
        DIBitmap.type = BMP_RGB16;
        break;

    case 24 :

        DIBitmap.type = BMP_RGB24;
        break;

    case 32 :

        DIBitmap.type = BMP_RGB32;
        break;

    default :

        PLXOS_UnmapFile(hFile, pFileData, dwFileSize);
        PLXOS_CloseFile(hFile);

        SetLastError(1);
        return FALSE;
    }

    // Skips bitmap info header
    pData += sizeof(BITMAPINFOHEADER);

    if (nBitCount <= 8)
    {
        DIBitmap.palette_entries = 1 << nBitCount;
        DIBitmap.pPalette = pData;
    }
    else if (nCompression == BI_BITFIELDS)
    {
        DIBitmap.palette_entries = 1;
        DIBitmap.pPalette = pData;
    }
    else
    {
        DIBitmap.palette_entries = 0;
        DIBitmap.pPalette = NULL;
    }

    if (DIBitmap.height > 0)
        nHeight = DIBitmap.height;
    else
        nHeight = -DIBitmap.height;

#if (COLUMN_BITMAP)

    if (nCompression & BI_COLUMN)
    {
        DIBitmap.bmp_size = (((nHeight * nBitCount + 31) & ~31) / 8)
            * DIBitmap.width;
    }
    else
    {
        DIBitmap.bmp_size = (((DIBitmap.width * nBitCount + 31) & ~31) / 8)
            * nHeight;

        // 指定位图是正常位图，不是列位图
        DIBitmap.type |= 0x8000;
    }

#else

    DIBitmap.bmp_size = (((DIBitmap.width * nBitCount + 31) & ~31) / 8)
        * nHeight;

#endif

    DIBitmap.pBuffer = (BYTE*)(pFileData + nOffBits);

    ENTERMONITOR;

    bReturn = WS_DrawDIBitmap(pDC, x, y, 0, 0, &DIBitmap, dwRop);

    LEAVEMONITOR;

    // Unmap and close the bitmap file
    PLXOS_UnmapFile(hFile, pFileData, dwFileSize);
    PLXOS_CloseFile(hFile);

    return bReturn;
}

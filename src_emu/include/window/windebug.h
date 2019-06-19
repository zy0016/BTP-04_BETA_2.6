/***************************************************************************
 *
 *                      Pollex Mobile Platform
 *
 * Copyright (c) 2004 by Pollex Mobile Software Co., Ltd. 
 *                       All Rights Reserved 
 *
 * Module   : Window
 *
 * Purpose  : Gets the current object info, used in emulator.
 *            
\**************************************************************************/

#ifndef __WINDEBUG_H
#define __WINDEBUG_H

#ifndef _MSC_VER
/* WWH modified on 2004/02/26 
** Delete the error info for real environment.
*/
//#error This header file can only be used in VC compiler!
#endif

/* Assume C declarations for C++ */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct tagWINDEBUGINFO
{
    int nGDIObjNum;     // Store the GDI object number
    int nGDIObjMax;     // Store the maximum GDI object number
    int nPenNum;        // Store the pen object number
    int nBrushNum;      // Store the brush object number
    int nBitmapNum;     // Store the bitmap object number
    int nFontNum;       // Store the font object number
    int nPaletteNum;    // Store the palette object number
    int nRegionNum;     // Store the region object number
    int nDCNum;         // Store the dc object number

    int nUserObjNum;    // Store the user object number
    int nUserObjMax;    // Store the maximum user object number
    int nWindowNum;     // Store the window object number
    int nCursorNum;     // Store the cursor object number
    int nIconNum;       // Store the icon object number
    int nMenuNum;       // Store the menu object number

    int nTimerNum;      // Store the timer object number
    int nTimerObjMax;   // Store the maximum user object number
} WINDEBUGINFO, *PWINDEBUGINFO;

#ifdef _MSC_VER
_declspec(dllexport) 
#endif
void GetWinDebugInfo(PWINDEBUGINFO pWinDebugInfo);

#ifdef __cplusplus
}                       /* End of extern "C" */
#endif

#endif  // __WINDEBUG_H

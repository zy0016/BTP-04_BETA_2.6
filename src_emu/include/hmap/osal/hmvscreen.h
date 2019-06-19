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

#ifndef _VSCREEN_H_
#define _VSCREEN_H_

typedef struct DisplayMode
{
    int    dsp_mode;            //Display mode.
    int    dsp_width;           //srceen width in pix.
    int    dsp_height;          //srceen height in pix.
    int    dsp_planes;          //number of planes.
    int    dsp_scanline_bytes;  //bytes per scan line.
    void*  dsp_screen_buffer;   //Address of emulate screen buffer.
    int    dsp_updatescreen;    //forced update screen flag.
}DISPLAYMODE, *PDISPLAYMODE;

#ifdef __cplusplus
extern "C" {
#endif

//int VScreen_Init (HINSTANCE hInstance);
int VScreen_Init (HINSTANCE hInstance, void * param);
int VScreen_Update (int screen_id, int left, int top, int right, int bottom, int mode);
int VScreen_GetDisplayMode(int screen_id,PDISPLAYMODE pDisplayMode);

#ifdef __cplusplus
}
#endif

#endif //_VSCREEN_H_

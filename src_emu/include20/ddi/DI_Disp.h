/**************************************************************************\
**                                                                        **
**                  Hopen Device Driver Interface           		      **
**                                                                        **
**  Copyright (C) 1998 by CASS Corptation & Software Engineering Center   **
**                       All Rights Reserved                              **
**                                                                        **
** The contents of this file are subject to the CASS License;             **
**                                                                        **
** Software distributed under this packet is distributed on an "AS IS"    **
** basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.        **
**                                                                        **
** Duplicate this file without CASS license is illegal.                   **
**                                                                        **
\**************************************************************************/

/***************************************************************************
 *
 * Hopen Device Driver Interface
 *
 * $Revision: 1 $     $Date: 99-11-08 14:21 $    
 *
 * $Log: /Hopen/include/ddi/DI_Disp.h $
 * 
 * 1     99-11-08 14:21 Dling
 * 
 * 1     99-11-08 13:38 Dling
 * 
 ***************************************************************************
 */

#ifndef _DDI_DISPLAY_H
#define _DDI_DISPLAY_H

#include "gengine.h"

typedef struct tagGRAPHENGINE {
	GE_TVINFO*	(*GetTVInfo)(void);
	GE_ATTRIB*	(*GetAttrib) (void);
	GE_CRTMODE*	(*EnumMode)(int index);

	int		(*Init)(int mode, long	outdev);
	int		(*SetMode)(int mode, long outdev);
	int		(*GetMode)(void);
	void*	(*GetVRAMAddr)(void);

	int		(*CreateCursor)(int handle, int hot_spot_x, int hot_spot_y, 
					int width, int height, long fg_color, long bk_color, void *pCursorData);
	int		(*SelectCursor)(int handle);
	int		(*SetCursorPos)(int x, int y);
	int		(*ShowCursor)(int mode);

	int		(*CreatePen)(int handle, long pattern);
	int		(*CreatePattern)(int handle, void *pData);
	int		(*CreateMonoPattern)(int handle, void *pData, long fgcolor, long bkcolor);
	int		(*SetDrawPara)(GE_DRAWPARA *pDrawPara);

	int		(*ClearScreen)(long color);
	int		(*DrawPixel)(int x, int y, long color);
	int		(*DrawLine)(int x1, int y1, int x2, int y2);
	int		(*FillRect)(int x, int y, int width, int height, long color);
	int		(*BitBlt)(int destx, int desty, int width, int height, int pitch,
					int bmpheight, void *pBmpData, int srcx, int srcy);
	int		(*MonoBitBlt)(int destx, int desty, int width, int height, int pitch,
					int bmpheight, void *pBmpData, int srcx, int srcy);
	int		(*ScreenBitBlt)(int x, int y, int width, int height, int srcx, int srcy);
	int		(*Scroll)(int x, int y, int width, int height, int dx, int dy);

	int		(*GetImage)(int x, int y, int width, int height, int pitch, void *pMemBmp);
	int		(*PutImage)(int x, int y, int width, int height, int pitch, void *pMemBmp);
	int		(*GetPalette)(int out_p, GE_RGBQUAD *out_pal, int out_num);
	int		(*SetPalette)(int in_p, GE_RGBQUAD *in_pal, int in_num);
	int		(*UpdateScreen)(void);
}GRAPHENGINE, *PGRAPHENGINE;

#endif // _DDI_DISPLAY_H

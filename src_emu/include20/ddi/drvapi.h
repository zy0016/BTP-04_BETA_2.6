/**************************************************************************\
**                                                                        **
**                     Hopen OS Debug and emulate                         **
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
 * CPU program for Emulator.
 *
 * $Revision: 1 $     $Date: 01-04-12 11:05 $    
 *
 * $Log: /Hopen/include/ddi/drvapi.h $
 * 
 * 1     01-04-12 11:05 Dling
 * 
 * 2     01-03-16 4:03p Xygeng
 * 
 * 1     00-01-14 9:10 Dling
 * 
 * 1     99-07-22 10:48a Suxiaofeng
 * add by SUXF
 * 
 * 2     99-05-10 9:28a Xswang
 * 
 * 1     99-05-07 9:10a Xswang
 *
 * 1     2000-10-20 13:57 Xygeng
 * 
 ***************************************************************************
 */
#ifndef __DRVAPI_H__
#define __DRVAPI_H__


typedef struct OS_DisplayMode {
	int		dsp_mode;			//
	int		dsp_width;			// screen width in pix.
	int		dsp_height;			// screen height in pix.
	int		dsp_planes;			// number of planes.
	int		dsp_scanline_bytes;	// bytes per scan line.
	void *	dsp_screen_buffer;	// Address of emulate screen buffer.
	int     dsp_rotate;         // rotate flag  
}OS_DisplayMode;

#define	DDM_VGA			1
#define	DDM_VGA256		2
#define	DDM_VGA32K		3
#define	DDM_VGA64K		4
#define	DDM_RGB24		5
#define	DDM_RGB32		6
#define	DDM_YUV24		7
#define	DDM_YUV32		8
#define	DDM_MONO		9
#define DDM_GRAY4       10
#define	DDM_VERTICAL    11
#define DDM_HORIZONTAL  12
#define	DDM_OTHER	    13
#define DDM_RGB12       14

#ifdef __cplusplus
extern "C" {
#endif

OS_DisplayMode* OS_EnumDisplayModeScreen( int mode );
BOOL OS_SetDisplayMode( int mode );
OS_DisplayMode * OS_GetDisplayMode( void );
void OS_UpdateScreen( int left, int top, int right, int bottom );


#ifdef __cplusplus
}
#endif

#endif // __DRVAPI_H__
